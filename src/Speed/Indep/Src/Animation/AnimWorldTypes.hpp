#ifndef ANIMATION_ANIMWORLDTYPES_H
#define ANIMATION_ANIMWORLDTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Animation/AnimScene.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x24
struct CWorldAnimEntity : public IAnimEntity {
    unsigned int mTypeID;                    // offset 0x4, size 0x4
    unsigned int mThisInstanceNameHash;      // offset 0x8, size 0x4
    struct SpaceNode *mSpaceNode;            // offset 0xC, size 0x4
    struct WorldModel *mWorldModel;          // offset 0x10, size 0x4
    struct CWorldAnimCtrl *mAnimCtrl;        // offset 0x14, size 0x4
    struct WorldAnimEntityInfo *mEntityInfo; // offset 0x18, size 0x4
    unsigned int mParentInstanceNameHash;    // offset 0x1C, size 0x4
    struct CWorldAnimEntityTree *mAnimTree;  // offset 0x20, size 0x4
};

enum eControlScenarioType {
    eCST_ERROR = -1,
    eCST_TriggerZone = 0,
    eCST_NIS_Generic = 1,
    eCST_FwdRevTrack = 2,
    eCST_NUM_TYPES = 3,
};

// total size: 0xA0
struct WorldAnimInstance {
    unsigned int unique_instance_id;  // offset 0x0, size 0x4
    unsigned int anim_tree_name_hash; // offset 0x4, size 0x4
    int section_number;               // offset 0x8, size 0x4
    unsigned int play_flags;          // offset 0xC, size 0x4
    float speed;                      // offset 0x10, size 0x4
    float master_delay;               // offset 0x14, size 0x4
    float loop_delay;                 // offset 0x18, size 0x4
    unsigned short begin_range;       // offset 0x1C, size 0x2
    unsigned short end_range;         // offset 0x1E, size 0x2
    unsigned int start_trigger_hash;  // offset 0x20, size 0x4
    unsigned int stop_trigger_hash;   // offset 0x24, size 0x4
    unsigned int lodb_hash;           // offset 0x28, size 0x4
    unsigned int lodz_hash;           // offset 0x2C, size 0x4
    unsigned int named_range_hash;    // offset 0x30, size 0x4
    unsigned int event_track_hash;    // offset 0x34, size 0x4
    unsigned int track_num;           // offset 0x38, size 0x4
    short track_dir;                  // offset 0x3C, size 0x2
    unsigned short pad0;              // offset 0x3E, size 0x2
    bMatrix4 instance_matrix;         // offset 0x40, size 0x40
    bVector3 bbox_min;                // offset 0x80, size 0x10
    bVector3 bbox_max;                // offset 0x90, size 0x10
};

// total size: 0x2C
struct CWorldAnimEntityTree : public bTNode<CWorldAnimEntityTree> {
    unsigned int tree_name_hash;                               // offset 0x8, size 0x4
    CWorldAnimEntity *root_entity;                             // offset 0xC, size 0x4
    bPList<CWorldAnimEntity> instantiated_world_anim_entities; // offset 0x10, size 0x8
    unsigned int start_trigger_hash;                           // offset 0x18, size 0x4
    unsigned int stop_trigger_hash;                            // offset 0x1C, size 0x4
    struct WorldAnimInstance *mInstanceData;                   // offset 0x20, size 0x4
    eControlScenarioType mControlScenarioType;                 // offset 0x24, size 0x4
};

#endif
