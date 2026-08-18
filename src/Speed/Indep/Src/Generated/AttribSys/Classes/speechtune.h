#ifndef _attrib_gen_speechtune_h
#define _attrib_gen_speechtune_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct speechtune : Instance {
    struct _LayoutStruct {
        Private _Array_PlayerSmashSpeedRange;                // offset 0x0, size 0x8
        EA::Reflection::Float PlayerSmashSpeedRange[2];      // offset 0x8, size 0x8
        Private _Array_PursuitInactivityTimer;               // offset 0x10, size 0x8
        EA::Reflection::Float PursuitInactivityTimer[3];     // offset 0x18, size 0xc
        UMath::Vector2 SpeechDropoffRamp;                    // offset 0x24, size 0x8
        EA::Reflection::Float OutcomeFailSpeed;              // offset 0x2c, size 0x4
        EA::Reflection::Float MinIntensitySideswipe;         // offset 0x30, size 0x4
        EA::Reflection::Float SpeedDiffForBlowby;            // offset 0x34, size 0x4
        EA::Reflection::Float BlowbyInterval;                // offset 0x38, size 0x4
        EA::Reflection::Float HangTimeForCommentary;         // offset 0x3c, size 0x4
        EA::Reflection::Float BURemindTime;                  // offset 0x40, size 0x4
        EA::Reflection::Float AIRacerProximity;              // offset 0x44, size 0x4
        EA::Reflection::Float MinIntensityCopSmash;          // offset 0x48, size 0x4
        EA::Reflection::Float MaxRangeFor180;                // offset 0x4c, size 0x4
        EA::Reflection::Float MaxRangeForPrimaryBranch;      // offset 0x50, size 0x4
        EA::Reflection::Float MinIntensityTrafficSmash;      // offset 0x54, size 0x4
        EA::Reflection::Float MinPursuitDurationForBailouts; // offset 0x58, size 0x4
        EA::Reflection::Float FlipTimeForCommentary;         // offset 0x5c, size 0x4
        EA::Reflection::Float PriorityWeight;                // offset 0x60, size 0x4
        EA::Reflection::Float OutcomeTrackTime;              // offset 0x64, size 0x4
        EA::Reflection::Float RangeForSpotterBranch;         // offset 0x68, size 0x4
        EA::Reflection::Float RBOutcomeTimer;                // offset 0x6c, size 0x4
        EA::Reflection::Float SpeedThreshFlyFlipIntensity;   // offset 0x70, size 0x4
        EA::Reflection::Float PursuitInitDelay;              // offset 0x74, size 0x4
        EA::Reflection::Float TimeWaitForSpotterReply;       // offset 0x78, size 0x4
        EA::Reflection::Float TimeConsideredLostNoLOS;       // offset 0x7c, size 0x4
        EA::Reflection::Float RBPostOutcomeResetTime;        // offset 0x80, size 0x4
        EA::Reflection::Float MinHeightAirborne;             // offset 0x84, size 0x4
        EA::Reflection::Int32 MinHavocForSuspectBehavior;    // offset 0x88, size 0x4
        EA::Reflection::Float MinSpeedConsideredStopped;     // offset 0x8c, size 0x4
        EA::Reflection::Float MinTimeConsideredStopped;      // offset 0x90, size 0x4
        EA::Reflection::Float PursuitDurationHighIntensity;  // offset 0x94, size 0x4
        EA::Reflection::Float CrashSlowdownPct;              // offset 0x98, size 0x4
        EA::Reflection::Float MinHealthForCommentary;        // offset 0x9c, size 0x4
        EA::Reflection::Float CollisionMinClosingVelSq;      // offset 0xa0, size 0x4
        EA::Reflection::Float MaxTimeFor180;                 // offset 0xa4, size 0x4
        EA::Reflection::Float SuspectOutrunRange;            // offset 0xa8, size 0x4
        EA::Reflection::Float NoLOSCommentaryTime;           // offset 0xac, size 0x4
        EA::Reflection::Int8 HighIntensityMark;              // offset 0xb0, size 0x1
        EA::Reflection::Int8 MinContigFramesFor180;          // offset 0xb1, size 0x1
    };

    typedef EA::Reflection::Float TypeOf_AIRacerProximity;
    typedef EA::Reflection::Float TypeOf_BURemindTime;
    typedef EA::Reflection::Float TypeOf_BlowbyInterval;
    typedef UMath::Vector2 TypeOf_CacheDisplayCoords;
    typedef EA::Reflection::Float TypeOf_CollisionMinClosingVelSq;
    typedef EA::Reflection::Float TypeOf_CrashSlowdownPct;
    typedef EA::Reflection::Float TypeOf_FlipTimeForCommentary;
    typedef EA::Reflection::Float TypeOf_HangTimeForCommentary;
    typedef EA::Reflection::Int8 TypeOf_HighIntensityMark;
    typedef EA::Reflection::Float TypeOf_MaxRangeFor180;
    typedef EA::Reflection::Float TypeOf_MaxRangeForPrimaryBranch;
    typedef EA::Reflection::Float TypeOf_MaxTimeFor180;
    typedef EA::Reflection::Int8 TypeOf_MinContigFramesFor180;
    typedef EA::Reflection::Int32 TypeOf_MinHavocForSuspectBehavior;
    typedef EA::Reflection::Float TypeOf_MinHealthForCommentary;
    typedef EA::Reflection::Float TypeOf_MinHeightAirborne;
    typedef EA::Reflection::Float TypeOf_MinIntensityCopSmash;
    typedef EA::Reflection::Float TypeOf_MinIntensitySideswipe;
    typedef EA::Reflection::Float TypeOf_MinIntensityTrafficSmash;
    typedef EA::Reflection::Float TypeOf_MinPursuitDurationForBailouts;
    typedef EA::Reflection::Float TypeOf_MinSpeedConsideredStopped;
    typedef EA::Reflection::Float TypeOf_MinTimeConsideredStopped;
    typedef EA::Reflection::Float TypeOf_NoLOSCommentaryTime;
    typedef EA::Reflection::Float TypeOf_OutcomeFailSpeed;
    typedef EA::Reflection::Float TypeOf_OutcomeTrackTime;
    typedef EA::Reflection::Float TypeOf_PlayerSmashSpeedRange;
    typedef EA::Reflection::Float TypeOf_PriorityWeight;
    typedef EA::Reflection::Float TypeOf_PursuitDurationHighIntensity;
    typedef EA::Reflection::Float TypeOf_PursuitInactivityTimer;
    typedef EA::Reflection::Float TypeOf_PursuitInitDelay;
    typedef EA::Reflection::Float TypeOf_RBOutcomeTimer;
    typedef EA::Reflection::Float TypeOf_RBPostOutcomeResetTime;
    typedef EA::Reflection::Float TypeOf_RangeForSpotterBranch;
    typedef UMath::Vector2 TypeOf_SpeechDropoffRamp;
    typedef EA::Reflection::Float TypeOf_SpeedDiffForBlowby;
    typedef EA::Reflection::Float TypeOf_SpeedThreshFlyFlipIntensity;
    typedef EA::Reflection::Float TypeOf_SuspectOutrunRange;
    typedef EA::Reflection::Float TypeOf_TimeConsideredLostNoLOS;
    typedef EA::Reflection::Float TypeOf_TimeWaitForSpotterReply;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("speechtune");
    speechtune(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speechtune(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speechtune(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speechtune(const speechtune &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    speechtune(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~speechtune() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0xbc683501;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0xbc683501, dynamicCollectionKey, spaceForAdditionalAttributes);
    }
    Key GenerateUniqueKey(const char *name, bool registerName) const {
        return GenerateUniqueKey(name, registerName);
    }
    void Change(const Collection *c) {
        Instance::Change(c);
    }
    void Change(const RefSpec &refspec) {
        Instance::Change(refspec);
    }
    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }
    void ChangeWithDefault(const RefSpec &refspec) {
        Instance::ChangeWithDefault(refspec);
    }
    void ChangeWithDefault(Key collectionkey) {
        Change(FindCollectionWithDefault(ClassKey(), collectionkey));
    }
    const speechtune &operator=(const speechtune &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const speechtune &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AIRacerProximity(TAttrib<EA::Reflection::Float> &result) const;
    bool AIRacerProximity(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &AIRacerProximity() const;
    bool SET_AIRacerProximity(const EA::Reflection::Float &input);

    bool BURemindTime(TAttrib<EA::Reflection::Float> &result) const;
    bool BURemindTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BURemindTime() const;
    bool SET_BURemindTime(const EA::Reflection::Float &input);

    bool BlowbyInterval(TAttrib<EA::Reflection::Float> &result) const;
    bool BlowbyInterval(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BlowbyInterval() const;
    bool SET_BlowbyInterval(const EA::Reflection::Float &input);

    bool CacheDisplayCoords(TAttrib<UMath::Vector2> &result) const;
    bool CacheDisplayCoords(UMath::Vector2 &result) const;
    const UMath::Vector2 &CacheDisplayCoords() const;
    bool SET_CacheDisplayCoords(const UMath::Vector2 &input);

    bool CollisionMinClosingVelSq(TAttrib<EA::Reflection::Float> &result) const;
    bool CollisionMinClosingVelSq(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CollisionMinClosingVelSq() const;
    bool SET_CollisionMinClosingVelSq(const EA::Reflection::Float &input);

    bool CrashSlowdownPct(TAttrib<EA::Reflection::Float> &result) const;
    bool CrashSlowdownPct(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CrashSlowdownPct() const;
    bool SET_CrashSlowdownPct(const EA::Reflection::Float &input);

    bool FlipTimeForCommentary(TAttrib<EA::Reflection::Float> &result) const;
    bool FlipTimeForCommentary(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FlipTimeForCommentary() const;
    bool SET_FlipTimeForCommentary(const EA::Reflection::Float &input);

    bool HangTimeForCommentary(TAttrib<EA::Reflection::Float> &result) const;
    bool HangTimeForCommentary(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &HangTimeForCommentary() const;
    bool SET_HangTimeForCommentary(const EA::Reflection::Float &input);

    bool HighIntensityMark(TAttrib<EA::Reflection::Int8> &result) const;
    bool HighIntensityMark(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &HighIntensityMark() const;
    bool SET_HighIntensityMark(const EA::Reflection::Int8 &input);

    bool MaxRangeFor180(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxRangeFor180(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxRangeFor180() const;
    bool SET_MaxRangeFor180(const EA::Reflection::Float &input);

    bool MaxRangeForPrimaryBranch(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxRangeForPrimaryBranch(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxRangeForPrimaryBranch() const;
    bool SET_MaxRangeForPrimaryBranch(const EA::Reflection::Float &input);

    bool MaxTimeFor180(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxTimeFor180(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxTimeFor180() const;
    bool SET_MaxTimeFor180(const EA::Reflection::Float &input);

    bool MinContigFramesFor180(TAttrib<EA::Reflection::Int8> &result) const;
    bool MinContigFramesFor180(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &MinContigFramesFor180() const;
    bool SET_MinContigFramesFor180(const EA::Reflection::Int8 &input);

    bool MinHavocForSuspectBehavior(TAttrib<EA::Reflection::Int32> &result) const;
    bool MinHavocForSuspectBehavior(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MinHavocForSuspectBehavior() const;
    bool SET_MinHavocForSuspectBehavior(const EA::Reflection::Int32 &input);

    bool MinHealthForCommentary(TAttrib<EA::Reflection::Float> &result) const;
    bool MinHealthForCommentary(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinHealthForCommentary() const;
    bool SET_MinHealthForCommentary(const EA::Reflection::Float &input);

    bool MinHeightAirborne(TAttrib<EA::Reflection::Float> &result) const;
    bool MinHeightAirborne(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinHeightAirborne() const;
    bool SET_MinHeightAirborne(const EA::Reflection::Float &input);

    bool MinIntensityCopSmash(TAttrib<EA::Reflection::Float> &result) const;
    bool MinIntensityCopSmash(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinIntensityCopSmash() const;
    bool SET_MinIntensityCopSmash(const EA::Reflection::Float &input);

    bool MinIntensitySideswipe(TAttrib<EA::Reflection::Float> &result) const;
    bool MinIntensitySideswipe(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinIntensitySideswipe() const;
    bool SET_MinIntensitySideswipe(const EA::Reflection::Float &input);

    bool MinIntensityTrafficSmash(TAttrib<EA::Reflection::Float> &result) const;
    bool MinIntensityTrafficSmash(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinIntensityTrafficSmash() const;
    bool SET_MinIntensityTrafficSmash(const EA::Reflection::Float &input);

    bool MinPursuitDurationForBailouts(TAttrib<EA::Reflection::Float> &result) const;
    bool MinPursuitDurationForBailouts(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinPursuitDurationForBailouts() const;
    bool SET_MinPursuitDurationForBailouts(const EA::Reflection::Float &input);

    bool MinSpeedConsideredStopped(TAttrib<EA::Reflection::Float> &result) const;
    bool MinSpeedConsideredStopped(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinSpeedConsideredStopped() const;
    bool SET_MinSpeedConsideredStopped(const EA::Reflection::Float &input);

    bool MinTimeConsideredStopped(TAttrib<EA::Reflection::Float> &result) const;
    bool MinTimeConsideredStopped(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinTimeConsideredStopped() const;
    bool SET_MinTimeConsideredStopped(const EA::Reflection::Float &input);

    bool NoLOSCommentaryTime(TAttrib<EA::Reflection::Float> &result) const;
    bool NoLOSCommentaryTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &NoLOSCommentaryTime() const;
    bool SET_NoLOSCommentaryTime(const EA::Reflection::Float &input);

    bool OutcomeFailSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool OutcomeFailSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &OutcomeFailSpeed() const;
    bool SET_OutcomeFailSpeed(const EA::Reflection::Float &input);

    bool OutcomeTrackTime(TAttrib<EA::Reflection::Float> &result) const;
    bool OutcomeTrackTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &OutcomeTrackTime() const;
    bool SET_OutcomeTrackTime(const EA::Reflection::Float &input);

    bool PlayerSmashSpeedRange(TAttrib<EA::Reflection::Float> &result) const;
    bool PlayerSmashSpeedRange(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &PlayerSmashSpeedRange(unsigned int index) const;
    unsigned int Num_PlayerSmashSpeedRange() const;
    bool SET_PlayerSmashSpeedRange(const EA::Reflection::Float &input, unsigned int index);

    bool PriorityWeight(TAttrib<EA::Reflection::Float> &result) const;
    bool PriorityWeight(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PriorityWeight() const;
    bool SET_PriorityWeight(const EA::Reflection::Float &input);

    bool PursuitDurationHighIntensity(TAttrib<EA::Reflection::Float> &result) const;
    bool PursuitDurationHighIntensity(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PursuitDurationHighIntensity() const;
    bool SET_PursuitDurationHighIntensity(const EA::Reflection::Float &input);

    bool PursuitInactivityTimer(TAttrib<EA::Reflection::Float> &result) const;
    bool PursuitInactivityTimer(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &PursuitInactivityTimer(unsigned int index) const;
    unsigned int Num_PursuitInactivityTimer() const;
    bool SET_PursuitInactivityTimer(const EA::Reflection::Float &input, unsigned int index);

    bool PursuitInitDelay(TAttrib<EA::Reflection::Float> &result) const;
    bool PursuitInitDelay(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PursuitInitDelay() const;
    bool SET_PursuitInitDelay(const EA::Reflection::Float &input);

    bool RBOutcomeTimer(TAttrib<EA::Reflection::Float> &result) const;
    bool RBOutcomeTimer(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RBOutcomeTimer() const;
    bool SET_RBOutcomeTimer(const EA::Reflection::Float &input);

    bool RBPostOutcomeResetTime(TAttrib<EA::Reflection::Float> &result) const;
    bool RBPostOutcomeResetTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RBPostOutcomeResetTime() const;
    bool SET_RBPostOutcomeResetTime(const EA::Reflection::Float &input);

    bool RangeForSpotterBranch(TAttrib<EA::Reflection::Float> &result) const;
    bool RangeForSpotterBranch(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RangeForSpotterBranch() const;
    bool SET_RangeForSpotterBranch(const EA::Reflection::Float &input);

    bool SpeechDropoffRamp(TAttrib<UMath::Vector2> &result) const;
    bool SpeechDropoffRamp(UMath::Vector2 &result) const;
    const UMath::Vector2 &SpeechDropoffRamp() const;
    bool SET_SpeechDropoffRamp(const UMath::Vector2 &input);

    bool SpeedDiffForBlowby(TAttrib<EA::Reflection::Float> &result) const;
    bool SpeedDiffForBlowby(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SpeedDiffForBlowby() const;
    bool SET_SpeedDiffForBlowby(const EA::Reflection::Float &input);

    bool SpeedThreshFlyFlipIntensity(TAttrib<EA::Reflection::Float> &result) const;
    bool SpeedThreshFlyFlipIntensity(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SpeedThreshFlyFlipIntensity() const;
    bool SET_SpeedThreshFlyFlipIntensity(const EA::Reflection::Float &input);

    bool SuspectOutrunRange(TAttrib<EA::Reflection::Float> &result) const;
    bool SuspectOutrunRange(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SuspectOutrunRange() const;
    bool SET_SuspectOutrunRange(const EA::Reflection::Float &input);

    bool TimeConsideredLostNoLOS(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeConsideredLostNoLOS(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeConsideredLostNoLOS() const;
    bool SET_TimeConsideredLostNoLOS(const EA::Reflection::Float &input);

    bool TimeWaitForSpotterReply(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeWaitForSpotterReply(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeWaitForSpotterReply() const;
    bool SET_TimeWaitForSpotterReply(const EA::Reflection::Float &input);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    speechtune &ConvertFromInstance(Instance &src) {}
    const speechtune &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key speechtune = 0xbc683501;

}; // namespace ClassName

namespace Hash {
namespace speechtune {

static const Key AIRacerProximity = 0x8f15e43b;
static const Key BURemindTime = 0xdab90d16;
static const Key BlowbyInterval = 0x375d8568;
static const Key CacheDisplayCoords = 0x3e02ef95;
static const Key CollisionMinClosingVelSq = 0xdff720b4;
static const Key CrashSlowdownPct = 0x5bb49375;
static const Key FlipTimeForCommentary = 0x9fa7ac96;
static const Key HangTimeForCommentary = 0x1b847cb0;
static const Key HighIntensityMark = 0xbeb03668;
static const Key MaxRangeFor180 = 0x76afc83c;
static const Key MaxRangeForPrimaryBranch = 0xabd1b31a;
static const Key MaxTimeFor180 = 0xbb9798af;
static const Key MinContigFramesFor180 = 0xe68a7fc6;
static const Key MinHavocForSuspectBehavior = 0xe076d2a4;
static const Key MinHealthForCommentary = 0x2d6dfc91;
static const Key MinHeightAirborne = 0x71a53db4;
static const Key MinIntensityCopSmash = 0xe06bbf89;
static const Key MinIntensitySideswipe = 0xe7610895;
static const Key MinIntensityTrafficSmash = 0xbd436d17;
static const Key MinPursuitDurationForBailouts = 0x323fa2bf;
static const Key MinSpeedConsideredStopped = 0x698003eb;
static const Key MinTimeConsideredStopped = 0xb65a6f44;
static const Key NoLOSCommentaryTime = 0x5d203696;
static const Key OutcomeFailSpeed = 0xe9291fb9;
static const Key OutcomeTrackTime = 0x6be5c1fc;
static const Key PlayerSmashSpeedRange = 0xbb2c9bd3;
static const Key PriorityWeight = 0x430abd7b;
static const Key PursuitDurationHighIntensity = 0x8a676402;
static const Key PursuitInactivityTimer = 0xee887188;
static const Key PursuitInitDelay = 0xbd9678ec;
static const Key RBOutcomeTimer = 0xa2c49064;
static const Key RBPostOutcomeResetTime = 0x6dd726c0;
static const Key RangeForSpotterBranch = 0x10048a7a;
static const Key SpeechDropoffRamp = 0x549a04da;
static const Key SpeedDiffForBlowby = 0x441c924b;
static const Key SpeedThreshFlyFlipIntensity = 0xc81f3736;
static const Key SuspectOutrunRange = 0x220a24f8;
static const Key TimeConsideredLostNoLOS = 0x4156301f;
static const Key TimeWaitForSpotterReply = 0x8a3ec259;

}; // namespace speechtune
}; // namespace Hash

inline Key Gen::speechtune::ClassKey() {
    return ClassName::speechtune;
}

inline bool Gen::speechtune::AIRacerProximity(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8f15e43b);
}
inline const EA::Reflection::Float &Gen::speechtune::AIRacerProximity() const {
    ATTRIB_CODEGEN_GETLAYOUT(AIRacerProximity);
}
inline bool Gen::speechtune::AIRacerProximity(EA::Reflection::Float &result) const {
    result = AIRacerProximity();
    return true;
}
inline bool Gen::speechtune::SET_AIRacerProximity(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(AIRacerProximity, input);
}

inline bool Gen::speechtune::BURemindTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdab90d16);
}
inline const EA::Reflection::Float &Gen::speechtune::BURemindTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(BURemindTime);
}
inline bool Gen::speechtune::BURemindTime(EA::Reflection::Float &result) const {
    result = BURemindTime();
    return true;
}
inline bool Gen::speechtune::SET_BURemindTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BURemindTime, input);
}

inline bool Gen::speechtune::BlowbyInterval(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x375d8568);
}
inline const EA::Reflection::Float &Gen::speechtune::BlowbyInterval() const {
    ATTRIB_CODEGEN_GETLAYOUT(BlowbyInterval);
}
inline bool Gen::speechtune::BlowbyInterval(EA::Reflection::Float &result) const {
    result = BlowbyInterval();
    return true;
}
inline bool Gen::speechtune::SET_BlowbyInterval(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BlowbyInterval, input);
}

inline bool Gen::speechtune::CacheDisplayCoords(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x3e02ef95);
}
inline const UMath::Vector2 &Gen::speechtune::CacheDisplayCoords() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector2, 0x3e02ef95);
}
inline bool Gen::speechtune::CacheDisplayCoords(UMath::Vector2 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector2, 0x3e02ef95, result);
}
inline bool Gen::speechtune::SET_CacheDisplayCoords(const UMath::Vector2 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector2, 0x3e02ef95, input);
}

inline bool Gen::speechtune::CollisionMinClosingVelSq(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdff720b4);
}
inline const EA::Reflection::Float &Gen::speechtune::CollisionMinClosingVelSq() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollisionMinClosingVelSq);
}
inline bool Gen::speechtune::CollisionMinClosingVelSq(EA::Reflection::Float &result) const {
    result = CollisionMinClosingVelSq();
    return true;
}
inline bool Gen::speechtune::SET_CollisionMinClosingVelSq(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollisionMinClosingVelSq, input);
}

inline bool Gen::speechtune::CrashSlowdownPct(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5bb49375);
}
inline const EA::Reflection::Float &Gen::speechtune::CrashSlowdownPct() const {
    ATTRIB_CODEGEN_GETLAYOUT(CrashSlowdownPct);
}
inline bool Gen::speechtune::CrashSlowdownPct(EA::Reflection::Float &result) const {
    result = CrashSlowdownPct();
    return true;
}
inline bool Gen::speechtune::SET_CrashSlowdownPct(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CrashSlowdownPct, input);
}

inline bool Gen::speechtune::FlipTimeForCommentary(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9fa7ac96);
}
inline const EA::Reflection::Float &Gen::speechtune::FlipTimeForCommentary() const {
    ATTRIB_CODEGEN_GETLAYOUT(FlipTimeForCommentary);
}
inline bool Gen::speechtune::FlipTimeForCommentary(EA::Reflection::Float &result) const {
    result = FlipTimeForCommentary();
    return true;
}
inline bool Gen::speechtune::SET_FlipTimeForCommentary(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FlipTimeForCommentary, input);
}

inline bool Gen::speechtune::HangTimeForCommentary(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1b847cb0);
}
inline const EA::Reflection::Float &Gen::speechtune::HangTimeForCommentary() const {
    ATTRIB_CODEGEN_GETLAYOUT(HangTimeForCommentary);
}
inline bool Gen::speechtune::HangTimeForCommentary(EA::Reflection::Float &result) const {
    result = HangTimeForCommentary();
    return true;
}
inline bool Gen::speechtune::SET_HangTimeForCommentary(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HangTimeForCommentary, input);
}

inline bool Gen::speechtune::HighIntensityMark(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xbeb03668);
}
inline const EA::Reflection::Int8 &Gen::speechtune::HighIntensityMark() const {
    ATTRIB_CODEGEN_GETLAYOUT(HighIntensityMark);
}
inline bool Gen::speechtune::HighIntensityMark(EA::Reflection::Int8 &result) const {
    result = HighIntensityMark();
    return true;
}
inline bool Gen::speechtune::SET_HighIntensityMark(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HighIntensityMark, input);
}

inline bool Gen::speechtune::MaxRangeFor180(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x76afc83c);
}
inline const EA::Reflection::Float &Gen::speechtune::MaxRangeFor180() const {
    ATTRIB_CODEGEN_GETLAYOUT(MaxRangeFor180);
}
inline bool Gen::speechtune::MaxRangeFor180(EA::Reflection::Float &result) const {
    result = MaxRangeFor180();
    return true;
}
inline bool Gen::speechtune::SET_MaxRangeFor180(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MaxRangeFor180, input);
}

inline bool Gen::speechtune::MaxRangeForPrimaryBranch(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xabd1b31a);
}
inline const EA::Reflection::Float &Gen::speechtune::MaxRangeForPrimaryBranch() const {
    ATTRIB_CODEGEN_GETLAYOUT(MaxRangeForPrimaryBranch);
}
inline bool Gen::speechtune::MaxRangeForPrimaryBranch(EA::Reflection::Float &result) const {
    result = MaxRangeForPrimaryBranch();
    return true;
}
inline bool Gen::speechtune::SET_MaxRangeForPrimaryBranch(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MaxRangeForPrimaryBranch, input);
}

inline bool Gen::speechtune::MaxTimeFor180(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbb9798af);
}
inline const EA::Reflection::Float &Gen::speechtune::MaxTimeFor180() const {
    ATTRIB_CODEGEN_GETLAYOUT(MaxTimeFor180);
}
inline bool Gen::speechtune::MaxTimeFor180(EA::Reflection::Float &result) const {
    result = MaxTimeFor180();
    return true;
}
inline bool Gen::speechtune::SET_MaxTimeFor180(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MaxTimeFor180, input);
}

inline bool Gen::speechtune::MinContigFramesFor180(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xe68a7fc6);
}
inline const EA::Reflection::Int8 &Gen::speechtune::MinContigFramesFor180() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinContigFramesFor180);
}
inline bool Gen::speechtune::MinContigFramesFor180(EA::Reflection::Int8 &result) const {
    result = MinContigFramesFor180();
    return true;
}
inline bool Gen::speechtune::SET_MinContigFramesFor180(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinContigFramesFor180, input);
}

inline bool Gen::speechtune::MinHavocForSuspectBehavior(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe076d2a4);
}
inline const EA::Reflection::Int32 &Gen::speechtune::MinHavocForSuspectBehavior() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinHavocForSuspectBehavior);
}
inline bool Gen::speechtune::MinHavocForSuspectBehavior(EA::Reflection::Int32 &result) const {
    result = MinHavocForSuspectBehavior();
    return true;
}
inline bool Gen::speechtune::SET_MinHavocForSuspectBehavior(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinHavocForSuspectBehavior, input);
}

inline bool Gen::speechtune::MinHealthForCommentary(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2d6dfc91);
}
inline const EA::Reflection::Float &Gen::speechtune::MinHealthForCommentary() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinHealthForCommentary);
}
inline bool Gen::speechtune::MinHealthForCommentary(EA::Reflection::Float &result) const {
    result = MinHealthForCommentary();
    return true;
}
inline bool Gen::speechtune::SET_MinHealthForCommentary(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinHealthForCommentary, input);
}

inline bool Gen::speechtune::MinHeightAirborne(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x71a53db4);
}
inline const EA::Reflection::Float &Gen::speechtune::MinHeightAirborne() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinHeightAirborne);
}
inline bool Gen::speechtune::MinHeightAirborne(EA::Reflection::Float &result) const {
    result = MinHeightAirborne();
    return true;
}
inline bool Gen::speechtune::SET_MinHeightAirborne(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinHeightAirborne, input);
}

inline bool Gen::speechtune::MinIntensityCopSmash(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe06bbf89);
}
inline const EA::Reflection::Float &Gen::speechtune::MinIntensityCopSmash() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinIntensityCopSmash);
}
inline bool Gen::speechtune::MinIntensityCopSmash(EA::Reflection::Float &result) const {
    result = MinIntensityCopSmash();
    return true;
}
inline bool Gen::speechtune::SET_MinIntensityCopSmash(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinIntensityCopSmash, input);
}

inline bool Gen::speechtune::MinIntensitySideswipe(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe7610895);
}
inline const EA::Reflection::Float &Gen::speechtune::MinIntensitySideswipe() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinIntensitySideswipe);
}
inline bool Gen::speechtune::MinIntensitySideswipe(EA::Reflection::Float &result) const {
    result = MinIntensitySideswipe();
    return true;
}
inline bool Gen::speechtune::SET_MinIntensitySideswipe(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinIntensitySideswipe, input);
}

inline bool Gen::speechtune::MinIntensityTrafficSmash(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbd436d17);
}
inline const EA::Reflection::Float &Gen::speechtune::MinIntensityTrafficSmash() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinIntensityTrafficSmash);
}
inline bool Gen::speechtune::MinIntensityTrafficSmash(EA::Reflection::Float &result) const {
    result = MinIntensityTrafficSmash();
    return true;
}
inline bool Gen::speechtune::SET_MinIntensityTrafficSmash(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinIntensityTrafficSmash, input);
}

inline bool Gen::speechtune::MinPursuitDurationForBailouts(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x323fa2bf);
}
inline const EA::Reflection::Float &Gen::speechtune::MinPursuitDurationForBailouts() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinPursuitDurationForBailouts);
}
inline bool Gen::speechtune::MinPursuitDurationForBailouts(EA::Reflection::Float &result) const {
    result = MinPursuitDurationForBailouts();
    return true;
}
inline bool Gen::speechtune::SET_MinPursuitDurationForBailouts(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinPursuitDurationForBailouts, input);
}

inline bool Gen::speechtune::MinSpeedConsideredStopped(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x698003eb);
}
inline const EA::Reflection::Float &Gen::speechtune::MinSpeedConsideredStopped() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinSpeedConsideredStopped);
}
inline bool Gen::speechtune::MinSpeedConsideredStopped(EA::Reflection::Float &result) const {
    result = MinSpeedConsideredStopped();
    return true;
}
inline bool Gen::speechtune::SET_MinSpeedConsideredStopped(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinSpeedConsideredStopped, input);
}

inline bool Gen::speechtune::MinTimeConsideredStopped(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb65a6f44);
}
inline const EA::Reflection::Float &Gen::speechtune::MinTimeConsideredStopped() const {
    ATTRIB_CODEGEN_GETLAYOUT(MinTimeConsideredStopped);
}
inline bool Gen::speechtune::MinTimeConsideredStopped(EA::Reflection::Float &result) const {
    result = MinTimeConsideredStopped();
    return true;
}
inline bool Gen::speechtune::SET_MinTimeConsideredStopped(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MinTimeConsideredStopped, input);
}

inline bool Gen::speechtune::NoLOSCommentaryTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5d203696);
}
inline const EA::Reflection::Float &Gen::speechtune::NoLOSCommentaryTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(NoLOSCommentaryTime);
}
inline bool Gen::speechtune::NoLOSCommentaryTime(EA::Reflection::Float &result) const {
    result = NoLOSCommentaryTime();
    return true;
}
inline bool Gen::speechtune::SET_NoLOSCommentaryTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NoLOSCommentaryTime, input);
}

inline bool Gen::speechtune::OutcomeFailSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe9291fb9);
}
inline const EA::Reflection::Float &Gen::speechtune::OutcomeFailSpeed() const {
    ATTRIB_CODEGEN_GETLAYOUT(OutcomeFailSpeed);
}
inline bool Gen::speechtune::OutcomeFailSpeed(EA::Reflection::Float &result) const {
    result = OutcomeFailSpeed();
    return true;
}
inline bool Gen::speechtune::SET_OutcomeFailSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OutcomeFailSpeed, input);
}

inline bool Gen::speechtune::OutcomeTrackTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6be5c1fc);
}
inline const EA::Reflection::Float &Gen::speechtune::OutcomeTrackTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(OutcomeTrackTime);
}
inline bool Gen::speechtune::OutcomeTrackTime(EA::Reflection::Float &result) const {
    result = OutcomeTrackTime();
    return true;
}
inline bool Gen::speechtune::SET_OutcomeTrackTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(OutcomeTrackTime, input);
}

inline bool Gen::speechtune::PlayerSmashSpeedRange(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbb2c9bd3);
}
inline const EA::Reflection::Float &Gen::speechtune::PlayerSmashSpeedRange(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, PlayerSmashSpeedRange, index);
}
inline bool Gen::speechtune::PlayerSmashSpeedRange(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(PlayerSmashSpeedRange, result, index);
}
inline unsigned int Gen::speechtune::Num_PlayerSmashSpeedRange() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(PlayerSmashSpeedRange);
}
inline bool Gen::speechtune::SET_PlayerSmashSpeedRange(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(PlayerSmashSpeedRange, input, index);
}

inline bool Gen::speechtune::PriorityWeight(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x430abd7b);
}
inline const EA::Reflection::Float &Gen::speechtune::PriorityWeight() const {
    ATTRIB_CODEGEN_GETLAYOUT(PriorityWeight);
}
inline bool Gen::speechtune::PriorityWeight(EA::Reflection::Float &result) const {
    result = PriorityWeight();
    return true;
}
inline bool Gen::speechtune::SET_PriorityWeight(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PriorityWeight, input);
}

inline bool Gen::speechtune::PursuitDurationHighIntensity(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8a676402);
}
inline const EA::Reflection::Float &Gen::speechtune::PursuitDurationHighIntensity() const {
    ATTRIB_CODEGEN_GETLAYOUT(PursuitDurationHighIntensity);
}
inline bool Gen::speechtune::PursuitDurationHighIntensity(EA::Reflection::Float &result) const {
    result = PursuitDurationHighIntensity();
    return true;
}
inline bool Gen::speechtune::SET_PursuitDurationHighIntensity(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PursuitDurationHighIntensity, input);
}

inline bool Gen::speechtune::PursuitInactivityTimer(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xee887188);
}
inline const EA::Reflection::Float &Gen::speechtune::PursuitInactivityTimer(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, PursuitInactivityTimer, index);
}
inline bool Gen::speechtune::PursuitInactivityTimer(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(PursuitInactivityTimer, result, index);
}
inline unsigned int Gen::speechtune::Num_PursuitInactivityTimer() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(PursuitInactivityTimer);
}
inline bool Gen::speechtune::SET_PursuitInactivityTimer(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(PursuitInactivityTimer, input, index);
}

inline bool Gen::speechtune::PursuitInitDelay(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbd9678ec);
}
inline const EA::Reflection::Float &Gen::speechtune::PursuitInitDelay() const {
    ATTRIB_CODEGEN_GETLAYOUT(PursuitInitDelay);
}
inline bool Gen::speechtune::PursuitInitDelay(EA::Reflection::Float &result) const {
    result = PursuitInitDelay();
    return true;
}
inline bool Gen::speechtune::SET_PursuitInitDelay(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(PursuitInitDelay, input);
}

inline bool Gen::speechtune::RBOutcomeTimer(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa2c49064);
}
inline const EA::Reflection::Float &Gen::speechtune::RBOutcomeTimer() const {
    ATTRIB_CODEGEN_GETLAYOUT(RBOutcomeTimer);
}
inline bool Gen::speechtune::RBOutcomeTimer(EA::Reflection::Float &result) const {
    result = RBOutcomeTimer();
    return true;
}
inline bool Gen::speechtune::SET_RBOutcomeTimer(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RBOutcomeTimer, input);
}

inline bool Gen::speechtune::RBPostOutcomeResetTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6dd726c0);
}
inline const EA::Reflection::Float &Gen::speechtune::RBPostOutcomeResetTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(RBPostOutcomeResetTime);
}
inline bool Gen::speechtune::RBPostOutcomeResetTime(EA::Reflection::Float &result) const {
    result = RBPostOutcomeResetTime();
    return true;
}
inline bool Gen::speechtune::SET_RBPostOutcomeResetTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RBPostOutcomeResetTime, input);
}

inline bool Gen::speechtune::RangeForSpotterBranch(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x10048a7a);
}
inline const EA::Reflection::Float &Gen::speechtune::RangeForSpotterBranch() const {
    ATTRIB_CODEGEN_GETLAYOUT(RangeForSpotterBranch);
}
inline bool Gen::speechtune::RangeForSpotterBranch(EA::Reflection::Float &result) const {
    result = RangeForSpotterBranch();
    return true;
}
inline bool Gen::speechtune::SET_RangeForSpotterBranch(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RangeForSpotterBranch, input);
}

inline bool Gen::speechtune::SpeechDropoffRamp(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x549a04da);
}
inline const UMath::Vector2 &Gen::speechtune::SpeechDropoffRamp() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpeechDropoffRamp);
}
inline bool Gen::speechtune::SpeechDropoffRamp(UMath::Vector2 &result) const {
    result = SpeechDropoffRamp();
    return true;
}
inline bool Gen::speechtune::SET_SpeechDropoffRamp(const UMath::Vector2 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpeechDropoffRamp, input);
}

inline bool Gen::speechtune::SpeedDiffForBlowby(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x441c924b);
}
inline const EA::Reflection::Float &Gen::speechtune::SpeedDiffForBlowby() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpeedDiffForBlowby);
}
inline bool Gen::speechtune::SpeedDiffForBlowby(EA::Reflection::Float &result) const {
    result = SpeedDiffForBlowby();
    return true;
}
inline bool Gen::speechtune::SET_SpeedDiffForBlowby(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpeedDiffForBlowby, input);
}

inline bool Gen::speechtune::SpeedThreshFlyFlipIntensity(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc81f3736);
}
inline const EA::Reflection::Float &Gen::speechtune::SpeedThreshFlyFlipIntensity() const {
    ATTRIB_CODEGEN_GETLAYOUT(SpeedThreshFlyFlipIntensity);
}
inline bool Gen::speechtune::SpeedThreshFlyFlipIntensity(EA::Reflection::Float &result) const {
    result = SpeedThreshFlyFlipIntensity();
    return true;
}
inline bool Gen::speechtune::SET_SpeedThreshFlyFlipIntensity(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SpeedThreshFlyFlipIntensity, input);
}

inline bool Gen::speechtune::SuspectOutrunRange(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x220a24f8);
}
inline const EA::Reflection::Float &Gen::speechtune::SuspectOutrunRange() const {
    ATTRIB_CODEGEN_GETLAYOUT(SuspectOutrunRange);
}
inline bool Gen::speechtune::SuspectOutrunRange(EA::Reflection::Float &result) const {
    result = SuspectOutrunRange();
    return true;
}
inline bool Gen::speechtune::SET_SuspectOutrunRange(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SuspectOutrunRange, input);
}

inline bool Gen::speechtune::TimeConsideredLostNoLOS(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4156301f);
}
inline const EA::Reflection::Float &Gen::speechtune::TimeConsideredLostNoLOS() const {
    ATTRIB_CODEGEN_GETLAYOUT(TimeConsideredLostNoLOS);
}
inline bool Gen::speechtune::TimeConsideredLostNoLOS(EA::Reflection::Float &result) const {
    result = TimeConsideredLostNoLOS();
    return true;
}
inline bool Gen::speechtune::SET_TimeConsideredLostNoLOS(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TimeConsideredLostNoLOS, input);
}

inline bool Gen::speechtune::TimeWaitForSpotterReply(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8a3ec259);
}
inline const EA::Reflection::Float &Gen::speechtune::TimeWaitForSpotterReply() const {
    ATTRIB_CODEGEN_GETLAYOUT(TimeWaitForSpotterReply);
}
inline bool Gen::speechtune::TimeWaitForSpotterReply(EA::Reflection::Float &result) const {
    result = TimeWaitForSpotterReply();
    return true;
}
inline bool Gen::speechtune::SET_TimeWaitForSpotterReply(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TimeWaitForSpotterReply, input);
}

}; // namespace Attrib

#endif
