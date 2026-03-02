#ifndef ANIMATION_ANIMENTITY_H
#define ANIMATION_ANIMENTITY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/World/WorldModel.hpp"

enum eAnimEntityType {
    eAnimEntityType_None = 0,
    eAnimEntityType_BasicCharacter = 1,
    eAnimEntityType_Prop = 2,
    eAnimEntityType_WorldAnimNode = 3,
    eAnimEntityType_MaxAnimEntityTypes = 4,
};

class IAnimEntity {
  public:
    virtual ~IAnimEntity() {
        Purge();
    }

    virtual bool Init(void *init_data, struct SpaceNode *parent_space_node);
    virtual void Purge();
    virtual unsigned int GetInstanceNameHash();
    virtual SpaceNode *GetSpaceNode();
    virtual void SetTime(float time);
    virtual void UpdateTimeStep(float time_step);
    virtual void RenderEffects(eView *view, int is_reflection);
    virtual WorldModel *GetWorldModel();
};

// total size: 0x1
class CAnimEntityFactory {
  public:
    static IAnimEntity *CreateAnimEntity(int anim_entity_type);

    static void EndianSwapEntityData(void *data, int size);
};

#endif
