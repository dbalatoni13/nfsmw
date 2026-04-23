#ifndef ANIMATION_ANIMWORLDTYPES_H
#define ANIMATION_ANIMWORLDTYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

static unsigned int WAM_START_TRIGGER = bStringHash("WAM_START_TRIGGER");
static unsigned int WAM_STOP_TRIGGER = bStringHash("WAM_STOP_TRIGGER");
static unsigned int WAM_FIRST_FRAME = bStringHash("WAM_FIRST_FRAME");
static unsigned int WAM_LAST_FRAME = bStringHash("WAM_LAST_FRAME");
static unsigned int WAM_SOUND_TRIGGER_START = bStringHash("WAM_SOUND_TRIGGER_START");
static unsigned int WAM_SOUND_TRIGGER_STOP = bStringHash("WAM_SOUND_TRIGGER_STOP");
static unsigned int WAM_NIS_GENERIC_CONTROL_MSG = bStringHash("WAM_NIS_GENERIC_CONTROL_MSG");
static unsigned int WAM_FWD_REV_TRACK_CONTROL_MSG = bStringHash("WAM_FWD_REV_TRACK_CONTROL_MSG");

enum eControlScenarioType {
    eCST_ERROR = -1,
    eCST_TriggerZone = 0,
    eCST_NIS_Generic = 1,
    eCST_FwdRevTrack = 2,
    eCST_NUM_TYPES = 3,
};

// total size: 0xA0
struct WorldAnimInstance {
    uint32 unique_instance_id;  // offset 0x0, size 0x4
    uint32 anim_tree_name_hash; // offset 0x4, size 0x4
    int32 section_number;       // offset 0x8, size 0x4
    uint32 play_flags;          // offset 0xC, size 0x4
    float speed;                // offset 0x10, size 0x4
    float master_delay;         // offset 0x14, size 0x4
    float loop_delay;           // offset 0x18, size 0x4
    uint16 begin_range;         // offset 0x1C, size 0x2
    uint16 end_range;           // offset 0x1E, size 0x2
    uint32 start_trigger_hash;  // offset 0x20, size 0x4
    uint32 stop_trigger_hash;   // offset 0x24, size 0x4
    uint32 lodb_hash;           // offset 0x28, size 0x4
    uint32 lodz_hash;           // offset 0x2C, size 0x4
    uint32 named_range_hash;    // offset 0x30, size 0x4
    uint32 event_track_hash;    // offset 0x34, size 0x4
    uint32 track_num;           // offset 0x38, size 0x4
    int16 track_dir;            // offset 0x3C, size 0x2
    uint16 pad0;                // offset 0x3E, size 0x2
    bMatrix4 instance_matrix;   // offset 0x40, size 0x40
    bVector3 bbox_min;          // offset 0x80, size 0x10
    bVector3 bbox_max;          // offset 0x90, size 0x10
};

// total size: 0x70
struct WorldAnimEntityInfo {
    WorldAnimEntityInfo() {}

    uint32 mTypeID;                         // offset 0x0, size 0x4
    uint32 mThisInstanceNameHash;           // offset 0x4, size 0x4
    uint32 mParentInstanceNameHash;         // offset 0x8, size 0x4
    uint32 mModelHash;                      // offset 0xC, size 0x4
    uint32 mLODB;                           // offset 0x10, size 0x4
    uint32 mLODZ;                           // offset 0x14, size 0x4
    WorldAnimEntityInfo *mParentEntityInfo; // offset 0x18, size 0x4
    uint32 padB;                            // offset 0x1C, size 0x4
    uint32 mAnimTreeHash;                   // offset 0x20, size 0x4
    uint32 mAnimNameHash;                   // offset 0x24, size 0x4
    uint8 mAnimContentFlags;                // offset 0x28, size 0x1
    int8 mParentIndex;                      // offset 0x29, size 0x1
    uint16 padC;                            // offset 0x2A, size 0x2
    WorldAnimInstance *instance_data;       // offset 0x2C, size 0x4
    bMatrix4 mLocalMatrix;                  // offset 0x30, size 0x40
};

// total size: 0x10
struct WorldAnimNamedRange {
    uint32 name_hash; // offset 0x0, size 0x4
    uint32 range;     // offset 0x4, size 0x4
    uint32 pad0;      // offset 0x8, size 0x4
    uint32 pad1;      // offset 0xC, size 0x4
};

#endif
