#include "GObjectBlock.h"

#include "GActivity.h"
#include "GCharacter.h"
#include "GHandler.h"
#include "GManager.h"
#include "GMarker.h"
#include "GRuntimeInstance.h"
#include "GState.h"
#include "GTrigger.h"
#include "GVault.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay_hash.h"
#include "Speed/Indep/Src/World/WTrigger.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

template <class T> unsigned int GetPaddedObjectSize();
template <class T> unsigned int FindInstances(GVault *vault, AttribKeyList *keyList, unsigned int *outObjCount, unsigned int *outNumConnections);

GObjectBlock::GObjectBlock(GVault *vault, unsigned char *buffer) : mVault(vault), mObjectBuffer(buffer) {
    for (unsigned int onType = 0; onType < kGameplayObjType_Count; onType++) {
        mObjectCount[onType] = 0;
        mObjectSize[onType] = 0;
        mObjectList[onType] = 0;
    }
}

GObjectBlock::~GObjectBlock() {
    DeleteObjects<GTrigger>();
    DeleteObjects<GMarker>();
    DeleteObjects<GCharacter>();
    DeleteObjects<GActivity>();
    DeleteObjects<GState>();
    DeleteObjects<GHandler>();

    mVault = 0;
    mObjectBuffer = 0;

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
    buffer += CreateObjects<GTrigger>(mVault, buffer);
}

template <class T> unsigned int GetPaddedObjectSize() {
    return (sizeof(T) + 15) & ~15;
}

template <class T> unsigned int FindInstances(GVault *vault, AttribKeyList *keyList, unsigned int *outObjCount, unsigned int *outNumConnections) {
    static unsigned int kObjectTemplateKey[kGameplayObjType_Count] = {
        Attrib::Hash::gameplay::key_activity,
        Attrib::Hash::gameplay::key_character,
        Attrib::Hash::gameplay::key_messagehandler,
        Attrib::Hash::gameplay::key_marker,
        Attrib::Hash::gameplay::key_state,
        Attrib::Hash::gameplay::key_trigger,
    };

    Attrib::Gen::gameplay templateObj(kObjectTemplateKey[GetGameplayType<T>()], 0, 0);

    Attrib::Vault *attribVault = vault->GetAttribVault();
    Attrib::Type collectionType = Attrib::StringToTypeID("Attrib::CollectionLoadData");
    unsigned int numExports = attribVault->CountExports();
    unsigned int objCount = 0;
    unsigned int numConnections = 0;

    for (unsigned int onExport = 0; onExport < numExports; onExport++) {
        if (attribVault->GetExportType(onExport) == collectionType) {
            const Attrib::Collection *collection = (const Attrib::Collection *) attribVault->GetExportData(onExport);
            unsigned int collectionKey = Attrib::GetCollectionKey(collection);

            Attrib::Gen::gameplay instanceObj(collectionKey, 0, 0);

            if (GObjectBlock::CollectionIsInstanceOfTemplate(instanceObj, templateObj)) {
                if (keyList != 0) {
                    keyList->push_back(collectionKey);
                }

                numConnections += GObjectBlock::CalcNumConnections(collectionKey);
                objCount++;
            }
        }
    }

    if (outObjCount != 0) {
        *outObjCount += objCount;
    }
    if (outNumConnections != 0) {
        *outNumConnections += numConnections;
    }

    unsigned int paddedSize = GetPaddedObjectSize<T>();
    unsigned int bytesUsed = objCount * paddedSize;
    unsigned int connBytes = numConnections * sizeof(GRuntimeInstance::ConnectedInstance);

    return (bytesUsed + connBytes + 15) & ~15;
}

unsigned int GObjectBlock::CalcSpaceRequired(GVault *vault, unsigned int *outObjCount) {
    unsigned int bytesUsed = 0;
    unsigned int objCount = 0;

    bytesUsed += FindInstances<GHandler>(vault, 0, &objCount, 0);
    bytesUsed += FindInstances<GState>(vault, 0, &objCount, 0);
    bytesUsed += FindInstances<GActivity>(vault, 0, &objCount, 0);
    bytesUsed += FindInstances<GCharacter>(vault, 0, &objCount, 0);
    bytesUsed += FindInstances<GMarker>(vault, 0, &objCount, 0);
    bytesUsed += FindInstances<GTrigger>(vault, 0, &objCount, 0);

    if (outObjCount != 0) {
        *outObjCount = objCount;
    }

    return bytesUsed;
}

bool GObjectBlock::CollectionIsInstanceOfTemplate(Attrib::Gen::gameplay &instanceObj, Attrib::Gen::gameplay &templateObj) {
    if (instanceObj.Template()) {
        return false;
    }

    unsigned int parentKey = instanceObj.GetParent();

    while (parentKey != 0) {
        Attrib::Gen::gameplay parentObj(parentKey, 0, 0);

        if (parentObj.GetCollection() == templateObj.GetCollection()) {
            return true;
        }

        parentKey = parentObj.GetParent();
    }

    return false;
}

unsigned int GObjectBlock::CalcNumConnections(unsigned int collectionKey) {
    Attrib::Class *gameplayClass = Attrib::Database::Get().GetClass(Attrib::ClassName::gameplay);

    Attrib::Gen::gameplay collection(collectionKey, 0, 0);

    unsigned int numConnections = collection.Num_Children();

    while (collectionKey != 0) {
        Attrib::Gen::gameplay collection(collectionKey, 0, 0);

        for (Attrib::AttributeIterator iter = collection.Iterator(); iter.Valid(); iter.Advance()) {
            unsigned int attributeKey = iter.GetKey();

            Attrib::Attribute attr = collection.Get(attributeKey);

            if (attributeKey == Attrib::Hash::gameplay::Children) {
                continue;
            }

            if (attr.IsValid()) {
                if (bStrCmp(Attrib::Database::Get().GetTypeDesc(attr.GetType()).GetName(), "GCollectionKey") == 0) {
                    unsigned int numAttribConnections = gameplayClass->GetDefinition(attr.GetKey())->IsArray()
                                                            ? attr.GetLength()
                                                            : 1;

                    numConnections += numAttribConnections;
                }
            }
        }

        collectionKey = collection.GetParent();
    }

    return numConnections;
}

template <class T> unsigned int GObjectBlock::CreateObjects(GVault *vault, unsigned char *buffer) {
    const GameplayObjType type = GetGameplayType<T>();

    AttribKeyList keys;

    FindInstances<T>(vault, &keys, 0, 0);

    unsigned int objSize = GetPaddedObjectSize<T>();
    unsigned int objCount = 0;

    GRuntimeInstance::ConnectedInstance *connectionBase = (GRuntimeInstance::ConnectedInstance *) (buffer + (objSize * keys.size()));
    GRuntimeInstance::ConnectedInstance *connectionDest = connectionBase;

    for (AttribKeyList::const_iterator iterObj = keys.begin(); iterObj != keys.end(); ++iterObj) {
        unsigned int collectionKey = *iterObj;

        T *pMem = (T *) (buffer + (objCount * objSize));
        T *newObj = new (pMem) T(collectionKey);

        unsigned int numConnections = GObjectBlock::CalcNumConnections(collectionKey);

        newObj->SetConnectionBuffer(connectionDest, numConnections);
        connectionDest += numConnections;

        objCount++;
    }

    mObjectCount[type] = objCount;
    mObjectSize[type] = objSize;
    mObjectList[type] = (GRuntimeInstance *) buffer;

    unsigned int spaceUsed = ((unsigned char *) connectionDest - buffer);

    return (spaceUsed + 15) & ~15;
}

template <class T> void GObjectBlock::DeleteObjects() {
    GameplayObjType type = GetGameplayType<T>();

    unsigned char *objects = (unsigned char *) mObjectList[type];
    unsigned int paddedSize = GetPaddedObjectSize<T>();

    for (unsigned int onObject = 0; onObject < mObjectCount[type]; onObject++) {
        T *object = (T *) (objects + (onObject * paddedSize));
        object->~T();
    }

    mObjectCount[type] = 0;
    mObjectSize[type] = 0;
    mObjectList[type] = 0;
}
