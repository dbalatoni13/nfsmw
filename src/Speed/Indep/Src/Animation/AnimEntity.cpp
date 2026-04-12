#include "AnimEntity_BasicCharacter.hpp"
#include "AnimEntity_Prop.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

IAnimEntity *CAnimEntityFactory::CreateAnimEntity(int anim_entity_type) {
    switch (anim_entity_type) {
        case eAnimEntityType_BasicCharacter:
            return new ("CBasicCharacterAnimEntity", 0) CBasicCharacterAnimEntity();
        case eAnimEntityType_Prop:
            return new ("CPopAnimEntity", 0) CPropAnimEntity();
        case eAnimEntityType_None:
        default:
            return nullptr;
    }
}

void CAnimEntityFactory::EndianSwapEntityData(void *data, int size) {
    bPlatEndianSwap(reinterpret_cast<uint32 *>(data));
    uint32 anim_entity_type = *reinterpret_cast<uint32 *>(data);
    switch (anim_entity_type) {
        case eAnimEntityType_BasicCharacter:
            CBasicCharacterAnimEntity::EndianSwapEntityData(data, size);
            break;
        case eAnimEntityType_Prop:
            CPropAnimEntity::EndianSwapEntityData(data, size);
            break;
        case eAnimEntityType_None:
        default:
            break;
    }
}
