#include "Speed/Indep/Src/Gameplay/GObjectBlock.h"

#include "Speed/Indep/Src/Gameplay/GVault.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

GObjectBlock::GObjectBlock(GVault *vault, unsigned char *buffer)
    : mVault(vault), //
      mObjectBuffer(buffer) {}

GObjectBlock::~GObjectBlock() {}

void GObjectBlock::Initialize(unsigned int bufferSize) {}

unsigned int GObjectBlock::CalcSpaceRequired(GVault *vault, unsigned int *outObjCount) {
    if (outObjCount) {
        *outObjCount = 0;
    }

    return 0;
}

bool GObjectBlock::CollectionIsInstanceOfTemplate(Attrib::Gen::gameplay &instanceObj, Attrib::Gen::gameplay &templateObj) {
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
