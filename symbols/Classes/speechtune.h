#ifndef ATTRIBSYS_CLASSES_SPEECHTUNE_H
#define ATTRIBSYS_CLASSES_SPEECHTUNE_H


#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct speechtune : Instance {
struct _LayoutStruct {
Private _Array_PlayerSmashSpeedRange; // offset 0x0, size 0x8
float PlayerSmashSpeedRange[2]; // offset 0x8, size 0x8
Private _Array_PursuitInactivityTimer; // offset 0x10, size 0x8
float PursuitInactivityTimer[3]; // offset 0x18, size 0xc
UMath::Vector2 SpeechDropoffRamp; // offset 0x24, size 0x8
float OutcomeFailSpeed; // offset 0x2c, size 0x4
float MinIntensitySideswipe; // offset 0x30, size 0x4
float SpeedDiffForBlowby; // offset 0x34, size 0x4
float BlowbyInterval; // offset 0x38, size 0x4
float HangTimeForCommentary; // offset 0x3c, size 0x4
float BURemindTime; // offset 0x40, size 0x4
float AIRacerProximity; // offset 0x44, size 0x4
float MinIntensityCopSmash; // offset 0x48, size 0x4
float MaxRangeFor180; // offset 0x4c, size 0x4
float MaxRangeForPrimaryBranch; // offset 0x50, size 0x4
float MinIntensityTrafficSmash; // offset 0x54, size 0x4
float MinPursuitDurationForBailouts; // offset 0x58, size 0x4
float FlipTimeForCommentary; // offset 0x5c, size 0x4
float PriorityWeight; // offset 0x60, size 0x4
float OutcomeTrackTime; // offset 0x64, size 0x4
float RangeForSpotterBranch; // offset 0x68, size 0x4
float RBOutcomeTimer; // offset 0x6c, size 0x4
float SpeedThreshFlyFlipIntensity; // offset 0x70, size 0x4
float PursuitInitDelay; // offset 0x74, size 0x4
float TimeWaitForSpotterReply; // offset 0x78, size 0x4
float TimeConsideredLostNoLOS; // offset 0x7c, size 0x4
float RBPostOutcomeResetTime; // offset 0x80, size 0x4
float MinHeightAirborne; // offset 0x84, size 0x4
int MinHavocForSuspectBehavior; // offset 0x88, size 0x4
float MinSpeedConsideredStopped; // offset 0x8c, size 0x4
float MinTimeConsideredStopped; // offset 0x90, size 0x4
float PursuitDurationHighIntensity; // offset 0x94, size 0x4
float CrashSlowdownPct; // offset 0x98, size 0x4
float MinHealthForCommentary; // offset 0x9c, size 0x4
float CollisionMinClosingVelSq; // offset 0xa0, size 0x4
float MaxTimeFor180; // offset 0xa4, size 0x4
float SuspectOutrunRange; // offset 0xa8, size 0x4
float NoLOSCommentaryTime; // offset 0xac, size 0x4
char HighIntensityMark; // offset 0xb0, size 0x1
char MinContigFramesFor180; // offset 0xb1, size 0x1
};

void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "speechtune");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "speechtune");
}

speechtune(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

speechtune(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

speechtune(const speechtune &src) : Instance(src) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

speechtune(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    SetDefaultLayout(sizeof(_LayoutStruct));
}

~speechtune() {}

void Change(const Collection *c) {
    Instance::Change(c);
}

void Change(Key collectionkey) {
    Change(FindCollection(ClassKey(), collectionkey));
}

void Change(const RefSpec &refspec) {
    Instance::Change(refspec);
}

static Key ClassKey() {
    return 0xbc683501;
}

const UMath::Vector2 &CacheDisplayCoords() const {
        const UMath::Vector2 *resultptr = reinterpret_cast<const UMath::Vector2 *>(GetAttributePointer(0x3e02ef95, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const UMath::Vector2 *>(DefaultDataArea(sizeof(UMath::Vector2)));
        }
        return *resultptr;
    }
        
const float &PlayerSmashSpeedRange(unsigned int index) const {
            const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(GetLayoutPointer());
            if (index < lp->_Array_PlayerSmashSpeedRange.GetLength()) {
            return lp->PlayerSmashSpeedRange[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        }

        unsigned int Num_PlayerSmashSpeedRange() const {
            return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->_Array_PlayerSmashSpeedRange.GetLength();
        }
        
        const float &PursuitInactivityTimer(unsigned int index) const {
            const _LayoutStruct *lp = reinterpret_cast<_LayoutStruct *>(GetLayoutPointer());
            if (index < lp->_Array_PursuitInactivityTimer.GetLength()) {
            return lp->PursuitInactivityTimer[index];
        } else {
            return *reinterpret_cast<const float *>(DefaultDataArea(sizeof(float)));
        }
        }

        unsigned int Num_PursuitInactivityTimer() const {
            return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->_Array_PursuitInactivityTimer.GetLength();
        }
        
        const UMath::Vector2 &SpeechDropoffRamp() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SpeechDropoffRamp;
}

const float &OutcomeFailSpeed() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->OutcomeFailSpeed;
}

const float &MinIntensitySideswipe() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinIntensitySideswipe;
}

const float &SpeedDiffForBlowby() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SpeedDiffForBlowby;
}

const float &BlowbyInterval() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BlowbyInterval;
}

const float &HangTimeForCommentary() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->HangTimeForCommentary;
}

const float &BURemindTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->BURemindTime;
}

const float &AIRacerProximity() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->AIRacerProximity;
}

const float &MinIntensityCopSmash() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinIntensityCopSmash;
}

const float &MaxRangeFor180() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MaxRangeFor180;
}

const float &MaxRangeForPrimaryBranch() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MaxRangeForPrimaryBranch;
}

const float &MinIntensityTrafficSmash() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinIntensityTrafficSmash;
}

const float &MinPursuitDurationForBailouts() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinPursuitDurationForBailouts;
}

const float &FlipTimeForCommentary() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->FlipTimeForCommentary;
}

const float &PriorityWeight() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->PriorityWeight;
}

const float &OutcomeTrackTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->OutcomeTrackTime;
}

const float &RangeForSpotterBranch() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->RangeForSpotterBranch;
}

const float &RBOutcomeTimer() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->RBOutcomeTimer;
}

const float &SpeedThreshFlyFlipIntensity() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SpeedThreshFlyFlipIntensity;
}

const float &PursuitInitDelay() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->PursuitInitDelay;
}

const float &TimeWaitForSpotterReply() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TimeWaitForSpotterReply;
}

const float &TimeConsideredLostNoLOS() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->TimeConsideredLostNoLOS;
}

const float &RBPostOutcomeResetTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->RBPostOutcomeResetTime;
}

const float &MinHeightAirborne() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinHeightAirborne;
}

const int &MinHavocForSuspectBehavior() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinHavocForSuspectBehavior;
}

const float &MinSpeedConsideredStopped() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinSpeedConsideredStopped;
}

const float &MinTimeConsideredStopped() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinTimeConsideredStopped;
}

const float &PursuitDurationHighIntensity() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->PursuitDurationHighIntensity;
}

const float &CrashSlowdownPct() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CrashSlowdownPct;
}

const float &MinHealthForCommentary() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinHealthForCommentary;
}

const float &CollisionMinClosingVelSq() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->CollisionMinClosingVelSq;
}

const float &MaxTimeFor180() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MaxTimeFor180;
}

const float &SuspectOutrunRange() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->SuspectOutrunRange;
}

const float &NoLOSCommentaryTime() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->NoLOSCommentaryTime;
}

const char &HighIntensityMark() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->HighIntensityMark;
}

const char &MinContigFramesFor180() const {
    return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->MinContigFramesFor180;
}

};

} // namespace Gen
} // namespace Attrib

#endif
