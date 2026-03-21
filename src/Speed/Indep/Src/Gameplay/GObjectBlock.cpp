#include "Speed/Indep/Src/Gameplay/GObjectBlock.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GCharacter.h"
#include "Speed/Indep/Src/Gameplay/GHandler.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GState.h"
#include "Speed/Indep/Src/Gameplay/GTrigger.h"
#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

template <typename T>
static unsigned int GetPaddedObjectSize() {
    return (sizeof(T) + 15) & ~15;
}

template <typename T>
static unsigned int FindInstances(GVault *vault, AttribKeyList *attribKeyList, unsigned int *outObjCount, unsigned int *outConnCount) {
    static unsigned int kObjectTemplateKey = 0x7FCB7ABA;
    int numConnections = 0;
    Attrib::Gen::gameplay objectTemplate(kObjectTemplateKey, 0, nullptr);
    int numObjects = 0;
    Attrib::Vault *attribVault = vault->GetAttribVault();
    unsigned int exportIndex = 0;
    unsigned int exportCount;
    int collectionType = Attrib::StringToTypeID("Attrib::CollectionLoadData");

    exportCount = attribVault->CountExports();
    if (exportCount != 0) {
        do {
            if (attribVault->GetExportType(exportIndex) == collectionType) {
                unsigned int collectionKey = Attrib::GetCollectionKey(reinterpret_cast<Attrib::Collection *>(attribVault->GetExportData(exportIndex)));
                Attrib::Gen::gameplay instanceObj(collectionKey, 0, nullptr);

                if (GObjectBlock::CollectionIsInstanceOfTemplate(instanceObj, objectTemplate)) {
                    if (attribKeyList) {
                        attribKeyList->push_back(collectionKey);
                    }

                    numObjects = numObjects + 1;
                    numConnections = numConnections + GObjectBlock::CalcNumConnections(collectionKey);
                }
            }

            exportIndex = exportIndex + 1;
        } while (exportIndex < exportCount);
    }

    if (outObjCount) {
        *outObjCount = *outObjCount + numObjects;
    }

    if (outConnCount) {
        *outConnCount = *outConnCount + numConnections;
    }

    return (numObjects * GetPaddedObjectSize<T>() + numConnections * 8 + 0xFU) & ~0xFU;
}

template <typename T>
struct GObjectBlockTypeTraits;

template <>
struct GObjectBlockTypeTraits<GActivity> {
    enum { kType = kGameplayObjType_Activity };
};

template <>
struct GObjectBlockTypeTraits<GCharacter> {
    enum { kType = kGameplayObjType_Character };
};

template <>
struct GObjectBlockTypeTraits<GHandler> {
    enum { kType = kGameplayObjType_Handler };
};

template <>
struct GObjectBlockTypeTraits<GMarker> {
    enum { kType = kGameplayObjType_Marker };
};

template <>
struct GObjectBlockTypeTraits<GState> {
    enum { kType = kGameplayObjType_State };
};

template <>
struct GObjectBlockTypeTraits<GTrigger> {
    enum { kType = kGameplayObjType_Trigger };
};

template <typename T>
static inline unsigned int GetGameplayType() {
    return GObjectBlockTypeTraits<T>::kType;
}

template <typename T>
void GObjectBlock::DeleteObjects() {
    const unsigned int type = GetGameplayType<T>();
    unsigned char *buffer = reinterpret_cast<unsigned char *>(mObjectList[type]);
    unsigned int objSize = GetPaddedObjectSize<T>();

    for (unsigned int onObj = 0; onObj < mObjectCount[type]; ++onObj) {
        T *obj = reinterpret_cast<T *>(buffer + onObj * objSize);
        obj->~T();
    }

    mObjectCount[type] = 0;
    mObjectSize[type] = 0;
    mObjectList[type] = nullptr;
}

template <typename T>
unsigned int GObjectBlock::CreateObjects(GVault *vault, unsigned char *buffer) {
    const unsigned int type = GetGameplayType<T>();
    AttribKeyList keys;
    unsigned int objSize;
    unsigned int objCount;
    GRuntimeInstance::ConnectedInstance *connectionBase;
    GRuntimeInstance::ConnectedInstance *connectionDest;
    unsigned int spaceUsed;

    FindInstances<T>(vault, &keys, nullptr, nullptr);

    objSize = GetPaddedObjectSize<T>();
    objCount = 0;
    connectionBase = reinterpret_cast<GRuntimeInstance::ConnectedInstance *>(buffer + objSize * keys.size());
    connectionDest = connectionBase;

    for (AttribKeyList::iterator iterObj = keys.begin(); iterObj != keys.end(); ++iterObj) {
        unsigned int collectionKey = *iterObj;
        T *pMem = ::new (buffer + objCount * objSize) T(collectionKey);
        T *newObj = pMem;
        unsigned int numConnections = CalcNumConnections(collectionKey);

        newObj->SetConnectionBuffer(connectionDest, numConnections);
        connectionDest += numConnections;
        objCount += 1;
    }

    mObjectCount[type] = objCount;
    mObjectSize[type] = objSize;
    mObjectList[type] = reinterpret_cast<GRuntimeInstance *>(buffer);

    spaceUsed = reinterpret_cast<unsigned char *>(connectionDest) - buffer;
    return (spaceUsed + 0xFU) & ~0xFU;
}

GObjectBlock::GObjectBlock(GVault *vault, unsigned char *buffer)
    : mVault(vault), //
      mObjectBuffer(buffer) {
    unsigned int onType = 0;

    do {
        mObjectCount[onType] = 0;
        mObjectSize[onType] = 0;
        mObjectList[onType] = nullptr;
        onType += 1;
    } while (onType < 6);
}

GObjectBlock::~GObjectBlock() {
    DeleteObjects<GTrigger>();
    DeleteObjects<GMarker>();
    DeleteObjects<GCharacter>();
    DeleteObjects<GActivity>();
    DeleteObjects<GState>();
    DeleteObjects<GHandler>();

    mObjectBuffer = nullptr;
    mVault = nullptr;

    if (WTriggerManager::Exists()) {
        WTriggerManager::Get().ClearAllFireOnExit();
    }
}

void GObjectBlock::Initialize(unsigned int bufferSize) {
    unsigned char *buffer = mObjectBuffer;

    buffer += CreateObjects<GHandler>(mVault, buffer);
    buffer += CreateObjects<GState>(mVault, buffer);
    buffer += CreateObjects<GActivity>(mVault, buffer);
    buffer += CreateObjects<GCharacter>(mVault, buffer);
    buffer += CreateObjects<GMarker>(mVault, buffer);
    CreateObjects<GTrigger>(mVault, buffer);
}

unsigned int GObjectBlock::CalcSpaceRequired(GVault *vault, unsigned int *outObjCount) {
    unsigned int objCount;
    unsigned int spaceRequired;

    objCount = 0;
    spaceRequired = FindInstances<GHandler>(vault, nullptr, &objCount, nullptr);

    spaceRequired += FindInstances<GState>(vault, nullptr, &objCount, nullptr);
    spaceRequired += FindInstances<GActivity>(vault, nullptr, &objCount, nullptr);
    spaceRequired += FindInstances<GCharacter>(vault, nullptr, &objCount, nullptr);
    spaceRequired += FindInstances<GMarker>(vault, nullptr, &objCount, nullptr);
    spaceRequired += FindInstances<GTrigger>(vault, nullptr, &objCount, nullptr);

    if (outObjCount) {
        *outObjCount = objCount;
    }

    return spaceRequired;
}

bool GObjectBlock::CollectionIsInstanceOfTemplate(Attrib::Gen::gameplay &instanceObj, Attrib::Gen::gameplay &templateObj) {
    const int *isObject = reinterpret_cast<const int *>(instanceObj.GetAttributePointer(0x3E9156CA, 0));
    unsigned int parentKey;

    if (!isObject) {
        isObject = reinterpret_cast<const int *>(Attrib::DefaultDataArea(sizeof(int)));
    }

    if (*isObject != 0) {
        return false;
    }

    parentKey = instanceObj.GetParent();
    while (parentKey != 0) {
        Attrib::Gen::gameplay parent(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), parentKey), 0, nullptr);

        if (parent.GetCollection() == templateObj.GetCollection()) {
            return true;
        }

        parentKey = parent.GetParent();
    }

    return false;
}

unsigned int GObjectBlock::CalcNumConnections(unsigned int collectionKey) {
    Attrib::Class *gameplayClass = Attrib::Database::Get().GetClass(Attrib::Gen::gameplay::ClassKey());
    unsigned int numConnections;
    Attrib::Gen::gameplay collection(collectionKey, 0, nullptr);

    numConnections = collection.Num_Children();
    if (collectionKey != 0) {
        do {
            Attrib::Gen::gameplay collection(collectionKey, 0, nullptr);
            Attrib::AttributeIterator iter = collection.Iterator();

            while (iter.Valid()) {
                unsigned int attribKey = iter.GetKey();

                {
                    Attrib::Attribute attrib = collection.Get(attribKey);

                    if (attribKey != 0x916E0E78) {
                        if (attrib.IsValid()) {
                            const Attrib::TypeDesc &typeDesc = Attrib::Database::Get().GetTypeDesc(attrib.GetType());
                            const char *typeName =
                                *reinterpret_cast<const char *const *>(reinterpret_cast<const char *>(&typeDesc) + sizeof(unsigned int));

                            if (bStrCmp(typeName, "GCollectionKey") == 0) {
                                const Attrib::Definition *definition = gameplayClass->GetDefinition(attrib.GetKey());
                                int count = 1;

                                if (definition->GetFlag(1)) {
                                    count = attrib.GetLength();
                                }

                                numConnections += count;
                            }
                        }
                    }
                }

                iter.Advance();
            }

            collectionKey = collection.GetParent();
        } while (collectionKey != 0);
    }

    return numConnections;
}

template void GObjectBlock::DeleteObjects<GActivity>();
template void GObjectBlock::DeleteObjects<GCharacter>();
template void GObjectBlock::DeleteObjects<GHandler>();
template void GObjectBlock::DeleteObjects<GMarker>();
template void GObjectBlock::DeleteObjects<GState>();
template void GObjectBlock::DeleteObjects<GTrigger>();

template unsigned int GObjectBlock::CreateObjects<GActivity>(GVault *vault, unsigned char *buffer);
template unsigned int GObjectBlock::CreateObjects<GCharacter>(GVault *vault, unsigned char *buffer);
template unsigned int GObjectBlock::CreateObjects<GHandler>(GVault *vault, unsigned char *buffer);
template unsigned int GObjectBlock::CreateObjects<GMarker>(GVault *vault, unsigned char *buffer);
template unsigned int GObjectBlock::CreateObjects<GState>(GVault *vault, unsigned char *buffer);
template unsigned int GObjectBlock::CreateObjects<GTrigger>(GVault *vault, unsigned char *buffer);
