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
    Attrib::Gen::gameplay objectTemplate(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), kObjectTemplateKey), 0, nullptr);
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
                Attrib::Gen::gameplay instanceObj(Attrib::FindCollection(Attrib::Gen::gameplay::ClassKey(), collectionKey), 0, nullptr);

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

GObjectBlock::GObjectBlock(GVault *vault, unsigned char *buffer)
    : mVault(vault), //
      mObjectBuffer(buffer) {}

GObjectBlock::~GObjectBlock() {}

void GObjectBlock::Initialize(unsigned int bufferSize) {}

unsigned int GObjectBlock::CalcSpaceRequired(GVault *vault, unsigned int *outObjCount) {
    unsigned int objCount = 0;
    unsigned int spaceRequired = FindInstances<GHandler>(vault, nullptr, &objCount, nullptr);

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

                    if (attribKey != 0x916E0E78 && attrib.IsValid()) {
                        const Attrib::TypeDesc &typeDesc = Attrib::Database::Get().GetTypeDesc(attrib.GetType());
                        const char *typeName =
                            *reinterpret_cast<const char *const *>(reinterpret_cast<const char *>(&typeDesc) + sizeof(unsigned int));

                        if (bStrCmp(typeName, "GCollectionKey") == 0) {
                            const Attrib::Definition *definition = gameplayClass->GetDefinition(attrib.GetKey());
                            int count = ((reinterpret_cast<const unsigned char *>(definition)[0xE] & 1) != 0) ? attrib.GetLength() : 1;

                            numConnections += count;
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
