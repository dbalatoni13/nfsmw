#ifndef _attrib_gen_pursuitlevels_h
#define _attrib_gen_pursuitlevels_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct pursuitlevels : Instance {
    struct _LayoutStruct {
        Private _Array_cops;                               // offset 0x0, size 0x8
        CopCountRecord cops[3];                            // offset 0x8, size 0x48
        EA::Reflection::Float heliLOSdistance;             // offset 0x50, size 0x4
        EA::Reflection::Float roadblockhelichance;         // offset 0x54, size 0x4
        EA::Reflection::Float SirenMaxYelpTime;            // offset 0x58, size 0x4
        EA::Reflection::Float roadblockprobability;        // offset 0x5c, size 0x4
        EA::Reflection::Float MilestoneCompleteHeatAdjust; // offset 0x60, size 0x4
        EA::Reflection::Float SirenMaxScreamTime;          // offset 0x64, size 0x4
        EA::Reflection::Float SearchModeRoadblockChance;   // offset 0x68, size 0x4
        EA::Reflection::Float HeliFuelTime;                // offset 0x6c, size 0x4
        EA::Reflection::Int32 FullEngagementCopCount;      // offset 0x70, size 0x4
        EA::Reflection::Float TimeBetweenHeliActive;       // offset 0x74, size 0x4
        EA::Reflection::Float TimePerHeatLevel;            // offset 0x78, size 0x4
        EA::Reflection::Float SirenWailPeriod;             // offset 0x7c, size 0x4
        EA::Reflection::Float TimeBetweenCopSpawn;         // offset 0x80, size 0x4
        EA::Reflection::Float EventWinHeatAdjust;          // offset 0x84, size 0x4
        EA::Reflection::Int32 RepPointsPerMinute;          // offset 0x88, size 0x4
        EA::Reflection::Float SearchModeCityMPH;           // offset 0x8c, size 0x4
        EA::Reflection::Float evadetimeout;                // offset 0x90, size 0x4
        EA::Reflection::Float SearchModeHwyMPH;            // offset 0x94, size 0x4
        EA::Reflection::Float FullEngagementRadius;        // offset 0x98, size 0x4
        EA::Reflection::Int32 NumCopsToTriggerBackup;      // offset 0x9c, size 0x4
        EA::Reflection::Float BackupCallTimer;             // offset 0xa0, size 0x4
        EA::Reflection::Float SirenInitVariation;          // offset 0xa4, size 0x4
        EA::Reflection::Float EvadeSuccessHeatAdjust;      // offset 0xa8, size 0x4
        EA::Reflection::Float SirenInitMinPeriod;          // offset 0xac, size 0x4
        EA::Reflection::Float frontLOSdistance;            // offset 0xb0, size 0x4
        EA::Reflection::Float TimeToHideInZone;            // offset 0xb4, size 0x4
        EA::Reflection::Float rearLOSdistance;             // offset 0xb8, size 0x4
        EA::Reflection::Float SirenScreamPeriod;           // offset 0xbc, size 0x4
        EA::Reflection::Float SearchModeRoadblockRadius;   // offset 0xc0, size 0x4
    };

    typedef EA::Reflection::Float TypeOf_BackupCallTimer;
    typedef EA::Reflection::Float TypeOf_BoxinDuration;
    typedef EA::Reflection::Float TypeOf_BoxinTightness;
    typedef EA::Reflection::Float TypeOf_BustSpeed;
    typedef EA::Reflection::Int32 TypeOf_CTSFor911;
    typedef EA::Reflection::Float TypeOf_CollapseAggression;
    typedef EA::Reflection::Int32 TypeOf_CollapseInnerRadius;
    typedef EA::Reflection::Float TypeOf_CollapseOuterRadius;
    typedef EA::Reflection::Float TypeOf_CollapseSpeed;
    typedef CopFormationRecord TypeOf_CopFormations;
    typedef EA::Reflection::Float TypeOf_DestroyCopBonusTime;
    typedef EA::Reflection::Float TypeOf_EvadeSuccessHeatAdjust;
    typedef EA::Reflection::Float TypeOf_EventWinHeatAdjust;
    typedef EA::Reflection::Int32 TypeOf_FullEngagementCopCount;
    typedef EA::Reflection::Float TypeOf_FullEngagementRadius;
    typedef EA::Reflection::Float TypeOf_HeliFuelTime;
    typedef EA::Reflection::Float TypeOf_HiddenZoneTimeMultiplier;
    typedef EA::Reflection::Float TypeOf_Lifetime911;
    typedef EA::Reflection::Int32 TypeOf_MaxCopsCollapsing;
    typedef EA::Reflection::Float TypeOf_MeterDeadZoneBustedDistance;
    typedef EA::Reflection::Float TypeOf_MeterDeadZoneEvadeDist;
    typedef EA::Reflection::Float TypeOf_MilestoneCompleteHeatAdjust;
    typedef EA::Reflection::Int8 TypeOf_NumCiviHitsFor911;
    typedef EA::Reflection::Int32 TypeOf_NumCopsToTriggerBackup;
    typedef EA::Reflection::Int32 TypeOf_NumPatrolCars;
    typedef EA::Reflection::Int32 TypeOf_RepPointsPerMinute;
    typedef EA::Reflection::Float TypeOf_RollingBlockDuration;
    typedef EA::Reflection::Float TypeOf_RollingBlockTightness;
    typedef EA::Reflection::Float TypeOf_ScaleEscalationPerBucket;
    typedef EA::Reflection::Float TypeOf_SearchModeCityMPH;
    typedef EA::Reflection::Float TypeOf_SearchModeHeliSpawnChance;
    typedef EA::Reflection::Float TypeOf_SearchModeHwyMPH;
    typedef EA::Reflection::Float TypeOf_SearchModeRoadblockChance;
    typedef EA::Reflection::Float TypeOf_SearchModeRoadblockRadius;
    typedef EA::Reflection::Float TypeOf_SirenInitMinPeriod;
    typedef EA::Reflection::Float TypeOf_SirenInitVariation;
    typedef EA::Reflection::Float TypeOf_SirenMaxScreamTime;
    typedef EA::Reflection::Float TypeOf_SirenMaxYelpTime;
    typedef EA::Reflection::Float TypeOf_SirenScreamPeriod;
    typedef EA::Reflection::Float TypeOf_SirenWailPeriod;
    typedef EA::Reflection::Float TypeOf_SpeedReactionTime;
    typedef EA::Reflection::Float TypeOf_StaggerFormationTime;
    typedef EA::Reflection::Float TypeOf_TimeBetweenCopSpawn;
    typedef EA::Reflection::Float TypeOf_TimeBetweenFirstFourSpawn;
    typedef EA::Reflection::Float TypeOf_TimeBetweenHeliActive;
    typedef EA::Reflection::Float TypeOf_TimeInactiveFor911;
    typedef EA::Reflection::Float TypeOf_TimePerHeatLevel;
    typedef EA::Reflection::Float TypeOf_TimeToHideInZone;
    typedef CopCountRecord TypeOf_cops;
    typedef EA::Reflection::Float TypeOf_evadetimeout;
    typedef EA::Reflection::UInt32 TypeOf_formations;
    typedef EA::Reflection::Float TypeOf_frontLOSdistance;
    typedef EA::Reflection::Float TypeOf_heliLOSdistance;
    typedef EA::Reflection::Float TypeOf_rearLOSdistance;
    typedef EA::Reflection::Float TypeOf_roadblockhelichance;
    typedef EA::Reflection::Float TypeOf_roadblockprobability;
    typedef EA::Reflection::Float TypeOf_roadblockspikechance;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("pursuitlevels");
    pursuitlevels(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pursuitlevels(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pursuitlevels(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pursuitlevels(const pursuitlevels &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    pursuitlevels(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~pursuitlevels() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x551e22b3;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x551e22b3, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const pursuitlevels &operator=(const pursuitlevels &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const pursuitlevels &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool BackupCallTimer(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xac6e1ec7);
    }
    bool BackupCallTimer(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(BackupCallTimer, result);
    }
    const EA::Reflection::Float &BackupCallTimer() const {
        ATTRIB_CODEGEN_GETLAYOUT(BackupCallTimer);
    }
    bool SET_BackupCallTimer(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(BackupCallTimer, input);
    }
    bool BoxinDuration(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x858b1097);
    }
    bool BoxinDuration(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x858b1097, result);
    }
    const EA::Reflection::Float &BoxinDuration() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x858b1097);
    }
    bool SET_BoxinDuration(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x858b1097, input);
    }
    bool BoxinTightness(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x67a15750);
    }
    bool BoxinTightness(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x67a15750, result);
    }
    const EA::Reflection::Float &BoxinTightness() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x67a15750);
    }
    bool SET_BoxinTightness(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x67a15750, input);
    }
    bool BustSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x769e8d9e);
    }
    bool BustSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x769e8d9e, result);
    }
    const EA::Reflection::Float &BustSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x769e8d9e);
    }
    bool SET_BustSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x769e8d9e, input);
    }
    bool CTSFor911(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xa00de933);
    }
    bool CTSFor911(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xa00de933, result);
    }
    const EA::Reflection::Int32 &CTSFor911() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xa00de933);
    }
    bool SET_CTSFor911(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xa00de933, input);
    }
    bool CollapseAggression(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x594e1492);
    }
    bool CollapseAggression(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x594e1492, result);
    }
    const EA::Reflection::Float &CollapseAggression() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x594e1492);
    }
    bool SET_CollapseAggression(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x594e1492, input);
    }
    bool CollapseInnerRadius(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x1e0af662);
    }
    bool CollapseInnerRadius(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x1e0af662, result);
    }
    const EA::Reflection::Int32 &CollapseInnerRadius() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x1e0af662);
    }
    bool SET_CollapseInnerRadius(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x1e0af662, input);
    }
    bool CollapseOuterRadius(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x947542f2);
    }
    bool CollapseOuterRadius(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x947542f2, result);
    }
    const EA::Reflection::Float &CollapseOuterRadius() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x947542f2);
    }
    bool SET_CollapseOuterRadius(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x947542f2, input);
    }
    bool CollapseSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdb66950c);
    }
    bool CollapseSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xdb66950c, result);
    }
    const EA::Reflection::Float &CollapseSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xdb66950c);
    }
    bool SET_CollapseSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xdb66950c, input);
    }
    bool CopFormations(TAttrib<CopFormationRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(CopFormationRecord, 0x5c2a7972);
    }
    bool CopFormations(CopFormationRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(CopFormationRecord, 0x5c2a7972, result, index);
    }
    const CopFormationRecord &CopFormations(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(CopFormationRecord, 0x5c2a7972, index);
    }
    unsigned int Num_CopFormations() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5c2a7972);
    }
    bool SET_CopFormations(const CopFormationRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(CopFormationRecord, 0x5c2a7972, input, index);
    }
    bool DestroyCopBonusTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbef78612);
    }
    bool DestroyCopBonusTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xbef78612, result);
    }
    const EA::Reflection::Float &DestroyCopBonusTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xbef78612);
    }
    bool SET_DestroyCopBonusTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xbef78612, input);
    }
    bool EvadeSuccessHeatAdjust(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb5a584d2);
    }
    bool EvadeSuccessHeatAdjust(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(EvadeSuccessHeatAdjust, result);
    }
    const EA::Reflection::Float &EvadeSuccessHeatAdjust() const {
        ATTRIB_CODEGEN_GETLAYOUT(EvadeSuccessHeatAdjust);
    }
    bool SET_EvadeSuccessHeatAdjust(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(EvadeSuccessHeatAdjust, input);
    }
    bool EventWinHeatAdjust(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7ad04eca);
    }
    bool EventWinHeatAdjust(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(EventWinHeatAdjust, result);
    }
    const EA::Reflection::Float &EventWinHeatAdjust() const {
        ATTRIB_CODEGEN_GETLAYOUT(EventWinHeatAdjust);
    }
    bool SET_EventWinHeatAdjust(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(EventWinHeatAdjust, input);
    }
    bool FullEngagementCopCount(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x73fea6db);
    }
    bool FullEngagementCopCount(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FullEngagementCopCount, result);
    }
    const EA::Reflection::Int32 &FullEngagementCopCount() const {
        ATTRIB_CODEGEN_GETLAYOUT(FullEngagementCopCount);
    }
    bool SET_FullEngagementCopCount(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FullEngagementCopCount, input);
    }
    bool FullEngagementRadius(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x01cec2b4);
    }
    bool FullEngagementRadius(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(FullEngagementRadius, result);
    }
    const EA::Reflection::Float &FullEngagementRadius() const {
        ATTRIB_CODEGEN_GETLAYOUT(FullEngagementRadius);
    }
    bool SET_FullEngagementRadius(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(FullEngagementRadius, input);
    }
    bool HeliFuelTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe766eb78);
    }
    bool HeliFuelTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(HeliFuelTime, result);
    }
    const EA::Reflection::Float &HeliFuelTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(HeliFuelTime);
    }
    bool SET_HeliFuelTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(HeliFuelTime, input);
    }
    bool HiddenZoneTimeMultiplier(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7fcee250);
    }
    bool HiddenZoneTimeMultiplier(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7fcee250, result);
    }
    const EA::Reflection::Float &HiddenZoneTimeMultiplier() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7fcee250);
    }
    bool SET_HiddenZoneTimeMultiplier(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7fcee250, input);
    }
    bool Lifetime911(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0e823327);
    }
    bool Lifetime911(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0e823327, result);
    }
    const EA::Reflection::Float &Lifetime911() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0e823327);
    }
    bool SET_Lifetime911(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0e823327, input);
    }
    bool MaxCopsCollapsing(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0c77d463);
    }
    bool MaxCopsCollapsing(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x0c77d463, result);
    }
    const EA::Reflection::Int32 &MaxCopsCollapsing() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x0c77d463);
    }
    bool SET_MaxCopsCollapsing(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x0c77d463, input);
    }
    bool MeterDeadZoneBustedDistance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8ce3219f);
    }
    bool MeterDeadZoneBustedDistance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8ce3219f, result);
    }
    const EA::Reflection::Float &MeterDeadZoneBustedDistance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8ce3219f);
    }
    bool SET_MeterDeadZoneBustedDistance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8ce3219f, input);
    }
    bool MeterDeadZoneEvadeDist(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xeda0e29c);
    }
    bool MeterDeadZoneEvadeDist(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xeda0e29c, result);
    }
    const EA::Reflection::Float &MeterDeadZoneEvadeDist() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xeda0e29c);
    }
    bool SET_MeterDeadZoneEvadeDist(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xeda0e29c, input);
    }
    bool MilestoneCompleteHeatAdjust(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa6cb6eff);
    }
    bool MilestoneCompleteHeatAdjust(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(MilestoneCompleteHeatAdjust, result);
    }
    const EA::Reflection::Float &MilestoneCompleteHeatAdjust() const {
        ATTRIB_CODEGEN_GETLAYOUT(MilestoneCompleteHeatAdjust);
    }
    bool SET_MilestoneCompleteHeatAdjust(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(MilestoneCompleteHeatAdjust, input);
    }
    bool NumCiviHitsFor911(TAttrib<EA::Reflection::Int8> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x6e590f57);
    }
    bool NumCiviHitsFor911(EA::Reflection::Int8 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int8, 0x6e590f57, result);
    }
    const EA::Reflection::Int8 &NumCiviHitsFor911() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int8, 0x6e590f57);
    }
    bool SET_NumCiviHitsFor911(const EA::Reflection::Int8 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int8, 0x6e590f57, input);
    }
    bool NumCopsToTriggerBackup(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc467015c);
    }
    bool NumCopsToTriggerBackup(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(NumCopsToTriggerBackup, result);
    }
    const EA::Reflection::Int32 &NumCopsToTriggerBackup() const {
        ATTRIB_CODEGEN_GETLAYOUT(NumCopsToTriggerBackup);
    }
    bool SET_NumCopsToTriggerBackup(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(NumCopsToTriggerBackup, input);
    }
    bool NumPatrolCars(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x24f7a1bc);
    }
    bool NumPatrolCars(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x24f7a1bc, result);
    }
    const EA::Reflection::Int32 &NumPatrolCars() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x24f7a1bc);
    }
    bool SET_NumPatrolCars(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x24f7a1bc, input);
    }
    bool RepPointsPerMinute(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x1e2a1051);
    }
    bool RepPointsPerMinute(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(RepPointsPerMinute, result);
    }
    const EA::Reflection::Int32 &RepPointsPerMinute() const {
        ATTRIB_CODEGEN_GETLAYOUT(RepPointsPerMinute);
    }
    bool SET_RepPointsPerMinute(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(RepPointsPerMinute, input);
    }
    bool RollingBlockDuration(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5c9f5f55);
    }
    bool RollingBlockDuration(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5c9f5f55, result);
    }
    const EA::Reflection::Float &RollingBlockDuration() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5c9f5f55);
    }
    bool SET_RollingBlockDuration(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5c9f5f55, input);
    }
    bool RollingBlockTightness(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc146fc03);
    }
    bool RollingBlockTightness(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc146fc03, result);
    }
    const EA::Reflection::Float &RollingBlockTightness() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc146fc03);
    }
    bool SET_RollingBlockTightness(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc146fc03, input);
    }
    bool ScaleEscalationPerBucket(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x80deb840);
    }
    bool ScaleEscalationPerBucket(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x80deb840, result, index);
    }
    const EA::Reflection::Float &ScaleEscalationPerBucket(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x80deb840, index);
    }
    unsigned int Num_ScaleEscalationPerBucket() const {
        ATTRIB_CODEGEN_GETLENGTH(0x80deb840);
    }
    bool SET_ScaleEscalationPerBucket(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x80deb840, input, index);
    }
    bool SearchModeCityMPH(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3fa01f3d);
    }
    bool SearchModeCityMPH(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SearchModeCityMPH, result);
    }
    const EA::Reflection::Float &SearchModeCityMPH() const {
        ATTRIB_CODEGEN_GETLAYOUT(SearchModeCityMPH);
    }
    bool SET_SearchModeCityMPH(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SearchModeCityMPH, input);
    }
    bool SearchModeHeliSpawnChance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3f11fbfc);
    }
    bool SearchModeHeliSpawnChance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3f11fbfc, result);
    }
    const EA::Reflection::Float &SearchModeHeliSpawnChance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3f11fbfc);
    }
    bool SET_SearchModeHeliSpawnChance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3f11fbfc, input);
    }
    bool SearchModeHwyMPH(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x04fcf097);
    }
    bool SearchModeHwyMPH(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SearchModeHwyMPH, result);
    }
    const EA::Reflection::Float &SearchModeHwyMPH() const {
        ATTRIB_CODEGEN_GETLAYOUT(SearchModeHwyMPH);
    }
    bool SET_SearchModeHwyMPH(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SearchModeHwyMPH, input);
    }
    bool SearchModeRoadblockChance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf49339cd);
    }
    bool SearchModeRoadblockChance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SearchModeRoadblockChance, result);
    }
    const EA::Reflection::Float &SearchModeRoadblockChance() const {
        ATTRIB_CODEGEN_GETLAYOUT(SearchModeRoadblockChance);
    }
    bool SET_SearchModeRoadblockChance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SearchModeRoadblockChance, input);
    }
    bool SearchModeRoadblockRadius(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x88c3ee99);
    }
    bool SearchModeRoadblockRadius(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SearchModeRoadblockRadius, result);
    }
    const EA::Reflection::Float &SearchModeRoadblockRadius() const {
        ATTRIB_CODEGEN_GETLAYOUT(SearchModeRoadblockRadius);
    }
    bool SET_SearchModeRoadblockRadius(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SearchModeRoadblockRadius, input);
    }
    bool SirenInitMinPeriod(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4096a291);
    }
    bool SirenInitMinPeriod(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SirenInitMinPeriod, result);
    }
    const EA::Reflection::Float &SirenInitMinPeriod() const {
        ATTRIB_CODEGEN_GETLAYOUT(SirenInitMinPeriod);
    }
    bool SET_SirenInitMinPeriod(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SirenInitMinPeriod, input);
    }
    bool SirenInitVariation(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xba4ad395);
    }
    bool SirenInitVariation(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SirenInitVariation, result);
    }
    const EA::Reflection::Float &SirenInitVariation() const {
        ATTRIB_CODEGEN_GETLAYOUT(SirenInitVariation);
    }
    bool SET_SirenInitVariation(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SirenInitVariation, input);
    }
    bool SirenMaxScreamTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbb607f29);
    }
    bool SirenMaxScreamTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SirenMaxScreamTime, result);
    }
    const EA::Reflection::Float &SirenMaxScreamTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(SirenMaxScreamTime);
    }
    bool SET_SirenMaxScreamTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SirenMaxScreamTime, input);
    }
    bool SirenMaxYelpTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7f154ad5);
    }
    bool SirenMaxYelpTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SirenMaxYelpTime, result);
    }
    const EA::Reflection::Float &SirenMaxYelpTime() const {
        ATTRIB_CODEGEN_GETLAYOUT(SirenMaxYelpTime);
    }
    bool SET_SirenMaxYelpTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SirenMaxYelpTime, input);
    }
    bool SirenScreamPeriod(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x55e85455);
    }
    bool SirenScreamPeriod(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SirenScreamPeriod, result);
    }
    const EA::Reflection::Float &SirenScreamPeriod() const {
        ATTRIB_CODEGEN_GETLAYOUT(SirenScreamPeriod);
    }
    bool SET_SirenScreamPeriod(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SirenScreamPeriod, input);
    }
    bool SirenWailPeriod(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3c76b17d);
    }
    bool SirenWailPeriod(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(SirenWailPeriod, result);
    }
    const EA::Reflection::Float &SirenWailPeriod() const {
        ATTRIB_CODEGEN_GETLAYOUT(SirenWailPeriod);
    }
    bool SET_SirenWailPeriod(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(SirenWailPeriod, input);
    }
    bool SpeedReactionTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0f575b64);
    }
    bool SpeedReactionTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0f575b64, result);
    }
    const EA::Reflection::Float &SpeedReactionTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0f575b64);
    }
    bool SET_SpeedReactionTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0f575b64, input);
    }
    bool StaggerFormationTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7648c884);
    }
    bool StaggerFormationTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7648c884, result);
    }
    const EA::Reflection::Float &StaggerFormationTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7648c884);
    }
    bool SET_StaggerFormationTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7648c884, input);
    }
    bool TimeBetweenCopSpawn(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9f3d8bee);
    }
    bool TimeBetweenCopSpawn(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TimeBetweenCopSpawn, result);
    }
    const EA::Reflection::Float &TimeBetweenCopSpawn() const {
        ATTRIB_CODEGEN_GETLAYOUT(TimeBetweenCopSpawn);
    }
    bool SET_TimeBetweenCopSpawn(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TimeBetweenCopSpawn, input);
    }
    bool TimeBetweenFirstFourSpawn(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9bf0f433);
    }
    bool TimeBetweenFirstFourSpawn(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9bf0f433, result);
    }
    const EA::Reflection::Float &TimeBetweenFirstFourSpawn() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9bf0f433);
    }
    bool SET_TimeBetweenFirstFourSpawn(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9bf0f433, input);
    }
    bool TimeBetweenHeliActive(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2b154825);
    }
    bool TimeBetweenHeliActive(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TimeBetweenHeliActive, result);
    }
    const EA::Reflection::Float &TimeBetweenHeliActive() const {
        ATTRIB_CODEGEN_GETLAYOUT(TimeBetweenHeliActive);
    }
    bool SET_TimeBetweenHeliActive(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TimeBetweenHeliActive, input);
    }
    bool TimeInactiveFor911(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x06cb70d5);
    }
    bool TimeInactiveFor911(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x06cb70d5, result);
    }
    const EA::Reflection::Float &TimeInactiveFor911() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x06cb70d5);
    }
    bool SET_TimeInactiveFor911(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x06cb70d5, input);
    }
    bool TimePerHeatLevel(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3c4e7ace);
    }
    bool TimePerHeatLevel(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TimePerHeatLevel, result);
    }
    const EA::Reflection::Float &TimePerHeatLevel() const {
        ATTRIB_CODEGEN_GETLAYOUT(TimePerHeatLevel);
    }
    bool SET_TimePerHeatLevel(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TimePerHeatLevel, input);
    }
    bool TimeToHideInZone(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xff761484);
    }
    bool TimeToHideInZone(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(TimeToHideInZone, result);
    }
    const EA::Reflection::Float &TimeToHideInZone() const {
        ATTRIB_CODEGEN_GETLAYOUT(TimeToHideInZone);
    }
    bool SET_TimeToHideInZone(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(TimeToHideInZone, input);
    }
    bool cops(TAttrib<CopCountRecord> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(CopCountRecord, 0xa9811b93);
    }
    bool cops(CopCountRecord &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(cops, result, index);
    }
    const CopCountRecord &cops(unsigned int index) const {
        ATTRIB_CODEGEN_GETLAYOUTINDEXED(CopCountRecord, cops, index);
    }
    unsigned int Num_cops() const {
        ATTRIB_CODEGEN_GETLAYOUTLENGTH(cops);
    }
    bool SET_cops(const CopCountRecord &input, unsigned int index) {
        ATTRIB_CODEGEN_SETLAYOUTINDEXED(cops, input, index);
    }
    bool evadetimeout(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa109bcce);
    }
    bool evadetimeout(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(evadetimeout, result);
    }
    const EA::Reflection::Float &evadetimeout() const {
        ATTRIB_CODEGEN_GETLAYOUT(evadetimeout);
    }
    bool SET_evadetimeout(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(evadetimeout, input);
    }
    bool formations(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x92f3d64e);
    }
    bool formations(EA::Reflection::UInt32 &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::UInt32, 0x92f3d64e, result, index);
    }
    const EA::Reflection::UInt32 &formations(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::UInt32, 0x92f3d64e, index);
    }
    unsigned int Num_formations() const {
        ATTRIB_CODEGEN_GETLENGTH(0x92f3d64e);
    }
    bool SET_formations(const EA::Reflection::UInt32 &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::UInt32, 0x92f3d64e, input, index);
    }
    bool frontLOSdistance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x90269ac5);
    }
    bool frontLOSdistance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(frontLOSdistance, result);
    }
    const EA::Reflection::Float &frontLOSdistance() const {
        ATTRIB_CODEGEN_GETLAYOUT(frontLOSdistance);
    }
    bool SET_frontLOSdistance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(frontLOSdistance, input);
    }
    bool heliLOSdistance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa0449b72);
    }
    bool heliLOSdistance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(heliLOSdistance, result);
    }
    const EA::Reflection::Float &heliLOSdistance() const {
        ATTRIB_CODEGEN_GETLAYOUT(heliLOSdistance);
    }
    bool SET_heliLOSdistance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(heliLOSdistance, input);
    }
    bool rearLOSdistance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdc6b7f2b);
    }
    bool rearLOSdistance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(rearLOSdistance, result);
    }
    const EA::Reflection::Float &rearLOSdistance() const {
        ATTRIB_CODEGEN_GETLAYOUT(rearLOSdistance);
    }
    bool SET_rearLOSdistance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(rearLOSdistance, input);
    }
    bool roadblockhelichance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x54159af1);
    }
    bool roadblockhelichance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(roadblockhelichance, result);
    }
    const EA::Reflection::Float &roadblockhelichance() const {
        ATTRIB_CODEGEN_GETLAYOUT(roadblockhelichance);
    }
    bool SET_roadblockhelichance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(roadblockhelichance, input);
    }
    bool roadblockprobability(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb09d16bd);
    }
    bool roadblockprobability(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(roadblockprobability, result);
    }
    const EA::Reflection::Float &roadblockprobability() const {
        ATTRIB_CODEGEN_GETLAYOUT(roadblockprobability);
    }
    bool SET_roadblockprobability(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETLAYOUT(roadblockprobability, input);
    }
    bool roadblockspikechance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5a318af6);
    }
    bool roadblockspikechance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5a318af6, result);
    }
    const EA::Reflection::Float &roadblockspikechance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5a318af6);
    }
    bool SET_roadblockspikechance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5a318af6, input);
    }

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    pursuitlevels &ConvertFromInstance(Instance &src) {}
    const pursuitlevels &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key pursuitlevels = 0x551e22b3;

}; // namespace ClassName

namespace Hash {
namespace pursuitlevels {

static const Key BackupCallTimer = 0xac6e1ec7;
static const Key BoxinDuration = 0x858b1097;
static const Key BoxinTightness = 0x67a15750;
static const Key BustSpeed = 0x769e8d9e;
static const Key CTSFor911 = 0xa00de933;
static const Key CollapseAggression = 0x594e1492;
static const Key CollapseInnerRadius = 0x1e0af662;
static const Key CollapseOuterRadius = 0x947542f2;
static const Key CollapseSpeed = 0xdb66950c;
static const Key CopFormations = 0x5c2a7972;
static const Key DestroyCopBonusTime = 0xbef78612;
static const Key EvadeSuccessHeatAdjust = 0xb5a584d2;
static const Key EventWinHeatAdjust = 0x7ad04eca;
static const Key FullEngagementCopCount = 0x73fea6db;
static const Key FullEngagementRadius = 0x01cec2b4;
static const Key HeliFuelTime = 0xe766eb78;
static const Key HiddenZoneTimeMultiplier = 0x7fcee250;
static const Key Lifetime911 = 0x0e823327;
static const Key MaxCopsCollapsing = 0x0c77d463;
static const Key MeterDeadZoneBustedDistance = 0x8ce3219f;
static const Key MeterDeadZoneEvadeDist = 0xeda0e29c;
static const Key MilestoneCompleteHeatAdjust = 0xa6cb6eff;
static const Key NumCiviHitsFor911 = 0x6e590f57;
static const Key NumCopsToTriggerBackup = 0xc467015c;
static const Key NumPatrolCars = 0x24f7a1bc;
static const Key RepPointsPerMinute = 0x1e2a1051;
static const Key RollingBlockDuration = 0x5c9f5f55;
static const Key RollingBlockTightness = 0xc146fc03;
static const Key ScaleEscalationPerBucket = 0x80deb840;
static const Key SearchModeCityMPH = 0x3fa01f3d;
static const Key SearchModeHeliSpawnChance = 0x3f11fbfc;
static const Key SearchModeHwyMPH = 0x04fcf097;
static const Key SearchModeRoadblockChance = 0xf49339cd;
static const Key SearchModeRoadblockRadius = 0x88c3ee99;
static const Key SirenInitMinPeriod = 0x4096a291;
static const Key SirenInitVariation = 0xba4ad395;
static const Key SirenMaxScreamTime = 0xbb607f29;
static const Key SirenMaxYelpTime = 0x7f154ad5;
static const Key SirenScreamPeriod = 0x55e85455;
static const Key SirenWailPeriod = 0x3c76b17d;
static const Key SpeedReactionTime = 0x0f575b64;
static const Key StaggerFormationTime = 0x7648c884;
static const Key TimeBetweenCopSpawn = 0x9f3d8bee;
static const Key TimeBetweenFirstFourSpawn = 0x9bf0f433;
static const Key TimeBetweenHeliActive = 0x2b154825;
static const Key TimeInactiveFor911 = 0x06cb70d5;
static const Key TimePerHeatLevel = 0x3c4e7ace;
static const Key TimeToHideInZone = 0xff761484;
static const Key cops = 0xa9811b93;
static const Key evadetimeout = 0xa109bcce;
static const Key formations = 0x92f3d64e;
static const Key frontLOSdistance = 0x90269ac5;
static const Key heliLOSdistance = 0xa0449b72;
static const Key rearLOSdistance = 0xdc6b7f2b;
static const Key roadblockhelichance = 0x54159af1;
static const Key roadblockprobability = 0xb09d16bd;
static const Key roadblockspikechance = 0x5a318af6;

}; // namespace pursuitlevels
}; // namespace Hash

inline Key Gen::pursuitlevels::ClassKey() {
    return ClassName::pursuitlevels;
}

}; // namespace Attrib

#endif
