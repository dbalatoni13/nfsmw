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
    bool AIRacerProximity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8f15e43b);
    }
    bool AIRacerProximity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(AIRacerProximity, result);
    }
    const EA::Reflection::Float &AIRacerProximity() const {
        ATTRIB_CODEGEN_GETLAYOUT(AIRacerProximity);
    }
    bool SET_AIRacerProximity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(AIRacerProximity, input);
    }
    bool BURemindTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdab90d16);
    }
    bool BURemindTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BURemindTime, result);
    }
    const EA::Reflection::Float &BURemindTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(BURemindTime);
    }
    bool SET_BURemindTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BURemindTime, input);
    }
    bool BlowbyInterval(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x375d8568);
    }
    bool BlowbyInterval(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BlowbyInterval, result);
    }
    const EA::Reflection::Float &BlowbyInterval() const {
        ATTRIB_CODEGEN_GETLAYOUT(BlowbyInterval);
    }
    bool SET_BlowbyInterval(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BlowbyInterval, input);
    }
    bool CacheDisplayCoords(TAttrib<UMath::Vector2> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x3e02ef95);
    }
    bool CacheDisplayCoords(UMath::Vector2 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector2, 0x3e02ef95, result);
    }
    const UMath::Vector2 &CacheDisplayCoords() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector2, 0x3e02ef95);
    }
    bool SET_CacheDisplayCoords(const UMath::Vector2 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector2, 0x3e02ef95, input);
    }
    bool CollisionMinClosingVelSq(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdff720b4);
    }
    bool CollisionMinClosingVelSq(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CollisionMinClosingVelSq, result);
    }
    const EA::Reflection::Float &CollisionMinClosingVelSq() const {
        ATTRIB_CODEGEN_GETLAYOUT(CollisionMinClosingVelSq);
    }
    bool SET_CollisionMinClosingVelSq(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CollisionMinClosingVelSq, input);
    }
    bool CrashSlowdownPct(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5bb49375);
    }
    bool CrashSlowdownPct(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CrashSlowdownPct, result);
    }
    const EA::Reflection::Float &CrashSlowdownPct() const {
        ATTRIB_CODEGEN_GETLAYOUT(CrashSlowdownPct);
    }
    bool SET_CrashSlowdownPct(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CrashSlowdownPct, input);
    }
    bool FlipTimeForCommentary(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9fa7ac96);
    }
    bool FlipTimeForCommentary(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FlipTimeForCommentary, result);
    }
    const EA::Reflection::Float &FlipTimeForCommentary() const {
        ATTRIB_CODEGEN_GETLAYOUT(FlipTimeForCommentary);
    }
    bool SET_FlipTimeForCommentary(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FlipTimeForCommentary, input);
    }
    bool HangTimeForCommentary(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1b847cb0);
    }
    bool HangTimeForCommentary(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(HangTimeForCommentary, result);
    }
    const EA::Reflection::Float &HangTimeForCommentary() const {
        ATTRIB_CODEGEN_GETLAYOUT(HangTimeForCommentary);
    }
    bool SET_HangTimeForCommentary(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(HangTimeForCommentary, input);
    }
    bool HighIntensityMark(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xbeb03668);
    }
    bool HighIntensityMark(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(HighIntensityMark, result);
    }
    const EA::Reflection::Int8 &HighIntensityMark() const {
        ATTRIB_CODEGEN_GETLAYOUT(HighIntensityMark);
    }
    bool SET_HighIntensityMark(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(HighIntensityMark, input);
    }
    bool MaxRangeFor180(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x76afc83c);
    }
    bool MaxRangeFor180(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MaxRangeFor180, result);
    }
    const EA::Reflection::Float &MaxRangeFor180() const {
        ATTRIB_CODEGEN_GETLAYOUT(MaxRangeFor180);
    }
    bool SET_MaxRangeFor180(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MaxRangeFor180, input);
    }
    bool MaxRangeForPrimaryBranch(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xabd1b31a);
    }
    bool MaxRangeForPrimaryBranch(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MaxRangeForPrimaryBranch, result);
    }
    const EA::Reflection::Float &MaxRangeForPrimaryBranch() const {
        ATTRIB_CODEGEN_GETLAYOUT(MaxRangeForPrimaryBranch);
    }
    bool SET_MaxRangeForPrimaryBranch(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MaxRangeForPrimaryBranch, input);
    }
    bool MaxTimeFor180(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbb9798af);
    }
    bool MaxTimeFor180(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MaxTimeFor180, result);
    }
    const EA::Reflection::Float &MaxTimeFor180() const {
        ATTRIB_CODEGEN_GETLAYOUT(MaxTimeFor180);
    }
    bool SET_MaxTimeFor180(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MaxTimeFor180, input);
    }
    bool MinContigFramesFor180(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0xe68a7fc6);
    }
    bool MinContigFramesFor180(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinContigFramesFor180, result);
    }
    const EA::Reflection::Int8 &MinContigFramesFor180() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinContigFramesFor180);
    }
    bool SET_MinContigFramesFor180(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinContigFramesFor180, input);
    }
    bool MinHavocForSuspectBehavior(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe076d2a4);
    }
    bool MinHavocForSuspectBehavior(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinHavocForSuspectBehavior, result);
    }
    const EA::Reflection::Int32 &MinHavocForSuspectBehavior() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinHavocForSuspectBehavior);
    }
    bool SET_MinHavocForSuspectBehavior(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinHavocForSuspectBehavior, input);
    }
    bool MinHealthForCommentary(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2d6dfc91);
    }
    bool MinHealthForCommentary(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinHealthForCommentary, result);
    }
    const EA::Reflection::Float &MinHealthForCommentary() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinHealthForCommentary);
    }
    bool SET_MinHealthForCommentary(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinHealthForCommentary, input);
    }
    bool MinHeightAirborne(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x71a53db4);
    }
    bool MinHeightAirborne(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinHeightAirborne, result);
    }
    const EA::Reflection::Float &MinHeightAirborne() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinHeightAirborne);
    }
    bool SET_MinHeightAirborne(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinHeightAirborne, input);
    }
    bool MinIntensityCopSmash(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe06bbf89);
    }
    bool MinIntensityCopSmash(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinIntensityCopSmash, result);
    }
    const EA::Reflection::Float &MinIntensityCopSmash() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinIntensityCopSmash);
    }
    bool SET_MinIntensityCopSmash(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinIntensityCopSmash, input);
    }
    bool MinIntensitySideswipe(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe7610895);
    }
    bool MinIntensitySideswipe(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinIntensitySideswipe, result);
    }
    const EA::Reflection::Float &MinIntensitySideswipe() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinIntensitySideswipe);
    }
    bool SET_MinIntensitySideswipe(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinIntensitySideswipe, input);
    }
    bool MinIntensityTrafficSmash(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbd436d17);
    }
    bool MinIntensityTrafficSmash(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinIntensityTrafficSmash, result);
    }
    const EA::Reflection::Float &MinIntensityTrafficSmash() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinIntensityTrafficSmash);
    }
    bool SET_MinIntensityTrafficSmash(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinIntensityTrafficSmash, input);
    }
    bool MinPursuitDurationForBailouts(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x323fa2bf);
    }
    bool MinPursuitDurationForBailouts(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinPursuitDurationForBailouts, result);
    }
    const EA::Reflection::Float &MinPursuitDurationForBailouts() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinPursuitDurationForBailouts);
    }
    bool SET_MinPursuitDurationForBailouts(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinPursuitDurationForBailouts, input);
    }
    bool MinSpeedConsideredStopped(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x698003eb);
    }
    bool MinSpeedConsideredStopped(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinSpeedConsideredStopped, result);
    }
    const EA::Reflection::Float &MinSpeedConsideredStopped() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinSpeedConsideredStopped);
    }
    bool SET_MinSpeedConsideredStopped(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinSpeedConsideredStopped, input);
    }
    bool MinTimeConsideredStopped(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb65a6f44);
    }
    bool MinTimeConsideredStopped(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MinTimeConsideredStopped, result);
    }
    const EA::Reflection::Float &MinTimeConsideredStopped() const {
        ATTRIB_CODEGEN_GETLAYOUT(MinTimeConsideredStopped);
    }
    bool SET_MinTimeConsideredStopped(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MinTimeConsideredStopped, input);
    }
    bool NoLOSCommentaryTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5d203696);
    }
    bool NoLOSCommentaryTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NoLOSCommentaryTime, result);
    }
    const EA::Reflection::Float &NoLOSCommentaryTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(NoLOSCommentaryTime);
    }
    bool SET_NoLOSCommentaryTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NoLOSCommentaryTime, input);
    }
    bool OutcomeFailSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe9291fb9);
    }
    bool OutcomeFailSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OutcomeFailSpeed, result);
    }
    const EA::Reflection::Float &OutcomeFailSpeed() const {
        ATTRIB_CODEGEN_GETLAYOUT(OutcomeFailSpeed);
    }
    bool SET_OutcomeFailSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OutcomeFailSpeed, input);
    }
    bool OutcomeTrackTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6be5c1fc);
    }
    bool OutcomeTrackTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(OutcomeTrackTime, result);
    }
    const EA::Reflection::Float &OutcomeTrackTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(OutcomeTrackTime);
    }
    bool SET_OutcomeTrackTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(OutcomeTrackTime, input);
    }
    bool PlayerSmashSpeedRange(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbb2c9bd3);
    }
    bool PlayerSmashSpeedRange(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(PlayerSmashSpeedRange, result, index);
    }
    const EA::Reflection::Float &PlayerSmashSpeedRange(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, PlayerSmashSpeedRange, index);
    }
    unsigned int Num_PlayerSmashSpeedRange() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(PlayerSmashSpeedRange);
    }
    bool SET_PlayerSmashSpeedRange(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(PlayerSmashSpeedRange, input, index);
    }
    bool PriorityWeight(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x430abd7b);
    }
    bool PriorityWeight(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(PriorityWeight, result);
    }
    const EA::Reflection::Float &PriorityWeight() const {
        ATTRIB_CODEGEN_GETLAYOUT(PriorityWeight);
    }
    bool SET_PriorityWeight(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(PriorityWeight, input);
    }
    bool PursuitDurationHighIntensity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8a676402);
    }
    bool PursuitDurationHighIntensity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(PursuitDurationHighIntensity, result);
    }
    const EA::Reflection::Float &PursuitDurationHighIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(PursuitDurationHighIntensity);
    }
    bool SET_PursuitDurationHighIntensity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(PursuitDurationHighIntensity, input);
    }
    bool PursuitInactivityTimer(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xee887188);
    }
    bool PursuitInactivityTimer(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(PursuitInactivityTimer, result, index);
    }
    const EA::Reflection::Float &PursuitInactivityTimer(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(EA::Reflection::Float, PursuitInactivityTimer, index);
    }
    unsigned int Num_PursuitInactivityTimer() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(PursuitInactivityTimer);
    }
    bool SET_PursuitInactivityTimer(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(PursuitInactivityTimer, input, index);
    }
    bool PursuitInitDelay(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbd9678ec);
    }
    bool PursuitInitDelay(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(PursuitInitDelay, result);
    }
    const EA::Reflection::Float &PursuitInitDelay() const {
        ATTRIB_CODEGEN_GETLAYOUT(PursuitInitDelay);
    }
    bool SET_PursuitInitDelay(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(PursuitInitDelay, input);
    }
    bool RBOutcomeTimer(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa2c49064);
    }
    bool RBOutcomeTimer(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RBOutcomeTimer, result);
    }
    const EA::Reflection::Float &RBOutcomeTimer() const {
        ATTRIB_CODEGEN_GETLAYOUT(RBOutcomeTimer);
    }
    bool SET_RBOutcomeTimer(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RBOutcomeTimer, input);
    }
    bool RBPostOutcomeResetTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x6dd726c0);
    }
    bool RBPostOutcomeResetTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RBPostOutcomeResetTime, result);
    }
    const EA::Reflection::Float &RBPostOutcomeResetTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(RBPostOutcomeResetTime);
    }
    bool SET_RBPostOutcomeResetTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RBPostOutcomeResetTime, input);
    }
    bool RangeForSpotterBranch(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x10048a7a);
    }
    bool RangeForSpotterBranch(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RangeForSpotterBranch, result);
    }
    const EA::Reflection::Float &RangeForSpotterBranch() const {
        ATTRIB_CODEGEN_GETLAYOUT(RangeForSpotterBranch);
    }
    bool SET_RangeForSpotterBranch(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RangeForSpotterBranch, input);
    }
    bool SpeechDropoffRamp(TAttrib<UMath::Vector2> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x549a04da);
    }
    bool SpeechDropoffRamp(UMath::Vector2 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpeechDropoffRamp, result);
    }
    const UMath::Vector2 &SpeechDropoffRamp() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpeechDropoffRamp);
    }
    bool SET_SpeechDropoffRamp(const UMath::Vector2 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpeechDropoffRamp, input);
    }
    bool SpeedDiffForBlowby(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x441c924b);
    }
    bool SpeedDiffForBlowby(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpeedDiffForBlowby, result);
    }
    const EA::Reflection::Float &SpeedDiffForBlowby() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpeedDiffForBlowby);
    }
    bool SET_SpeedDiffForBlowby(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpeedDiffForBlowby, input);
    }
    bool SpeedThreshFlyFlipIntensity(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc81f3736);
    }
    bool SpeedThreshFlyFlipIntensity(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SpeedThreshFlyFlipIntensity, result);
    }
    const EA::Reflection::Float &SpeedThreshFlyFlipIntensity() const {
        ATTRIB_CODEGEN_GETLAYOUT(SpeedThreshFlyFlipIntensity);
    }
    bool SET_SpeedThreshFlyFlipIntensity(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SpeedThreshFlyFlipIntensity, input);
    }
    bool SuspectOutrunRange(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x220a24f8);
    }
    bool SuspectOutrunRange(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SuspectOutrunRange, result);
    }
    const EA::Reflection::Float &SuspectOutrunRange() const {
        ATTRIB_CODEGEN_GETLAYOUT(SuspectOutrunRange);
    }
    bool SET_SuspectOutrunRange(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SuspectOutrunRange, input);
    }
    bool TimeConsideredLostNoLOS(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4156301f);
    }
    bool TimeConsideredLostNoLOS(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TimeConsideredLostNoLOS, result);
    }
    const EA::Reflection::Float &TimeConsideredLostNoLOS() const {
        ATTRIB_CODEGEN_GETLAYOUT(TimeConsideredLostNoLOS);
    }
    bool SET_TimeConsideredLostNoLOS(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TimeConsideredLostNoLOS, input);
    }
    bool TimeWaitForSpotterReply(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8a3ec259);
    }
    bool TimeWaitForSpotterReply(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TimeWaitForSpotterReply, result);
    }
    const EA::Reflection::Float &TimeWaitForSpotterReply() const {
        ATTRIB_CODEGEN_GETLAYOUT(TimeWaitForSpotterReply);
    }
    bool SET_TimeWaitForSpotterReply(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TimeWaitForSpotterReply, input);
    }

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

}; // namespace Attrib

#endif
