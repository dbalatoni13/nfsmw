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
    bool BackupCallTimer(TAttrib<EA::Reflection::Float> &result) const;
    bool BackupCallTimer(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BackupCallTimer() const;
    bool SET_BackupCallTimer(const EA::Reflection::Float &input);

    bool BoxinDuration(TAttrib<EA::Reflection::Float> &result) const;
    bool BoxinDuration(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BoxinDuration() const;
    bool SET_BoxinDuration(const EA::Reflection::Float &input);

    bool BoxinTightness(TAttrib<EA::Reflection::Float> &result) const;
    bool BoxinTightness(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BoxinTightness() const;
    bool SET_BoxinTightness(const EA::Reflection::Float &input);

    bool BustSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool BustSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BustSpeed() const;
    bool SET_BustSpeed(const EA::Reflection::Float &input);

    bool CTSFor911(TAttrib<EA::Reflection::Int32> &result) const;
    bool CTSFor911(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &CTSFor911() const;
    bool SET_CTSFor911(const EA::Reflection::Int32 &input);

    bool CollapseAggression(TAttrib<EA::Reflection::Float> &result) const;
    bool CollapseAggression(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CollapseAggression() const;
    bool SET_CollapseAggression(const EA::Reflection::Float &input);

    bool CollapseInnerRadius(TAttrib<EA::Reflection::Int32> &result) const;
    bool CollapseInnerRadius(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &CollapseInnerRadius() const;
    bool SET_CollapseInnerRadius(const EA::Reflection::Int32 &input);

    bool CollapseOuterRadius(TAttrib<EA::Reflection::Float> &result) const;
    bool CollapseOuterRadius(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CollapseOuterRadius() const;
    bool SET_CollapseOuterRadius(const EA::Reflection::Float &input);

    bool CollapseSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool CollapseSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CollapseSpeed() const;
    bool SET_CollapseSpeed(const EA::Reflection::Float &input);

    bool CopFormations(TAttrib<CopFormationRecord> &result) const;
    bool CopFormations(CopFormationRecord &result, unsigned int index) const;
    const CopFormationRecord &CopFormations(unsigned int index) const;
    unsigned int Num_CopFormations() const;
    bool SET_CopFormations(const CopFormationRecord &input, unsigned int index);

    bool DestroyCopBonusTime(TAttrib<EA::Reflection::Float> &result) const;
    bool DestroyCopBonusTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DestroyCopBonusTime() const;
    bool SET_DestroyCopBonusTime(const EA::Reflection::Float &input);

    bool EvadeSuccessHeatAdjust(TAttrib<EA::Reflection::Float> &result) const;
    bool EvadeSuccessHeatAdjust(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EvadeSuccessHeatAdjust() const;
    bool SET_EvadeSuccessHeatAdjust(const EA::Reflection::Float &input);

    bool EventWinHeatAdjust(TAttrib<EA::Reflection::Float> &result) const;
    bool EventWinHeatAdjust(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &EventWinHeatAdjust() const;
    bool SET_EventWinHeatAdjust(const EA::Reflection::Float &input);

    bool FullEngagementCopCount(TAttrib<EA::Reflection::Int32> &result) const;
    bool FullEngagementCopCount(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &FullEngagementCopCount() const;
    bool SET_FullEngagementCopCount(const EA::Reflection::Int32 &input);

    bool FullEngagementRadius(TAttrib<EA::Reflection::Float> &result) const;
    bool FullEngagementRadius(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FullEngagementRadius() const;
    bool SET_FullEngagementRadius(const EA::Reflection::Float &input);

    bool HeliFuelTime(TAttrib<EA::Reflection::Float> &result) const;
    bool HeliFuelTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &HeliFuelTime() const;
    bool SET_HeliFuelTime(const EA::Reflection::Float &input);

    bool HiddenZoneTimeMultiplier(TAttrib<EA::Reflection::Float> &result) const;
    bool HiddenZoneTimeMultiplier(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &HiddenZoneTimeMultiplier() const;
    bool SET_HiddenZoneTimeMultiplier(const EA::Reflection::Float &input);

    bool Lifetime911(TAttrib<EA::Reflection::Float> &result) const;
    bool Lifetime911(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Lifetime911() const;
    bool SET_Lifetime911(const EA::Reflection::Float &input);

    bool MaxCopsCollapsing(TAttrib<EA::Reflection::Int32> &result) const;
    bool MaxCopsCollapsing(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MaxCopsCollapsing() const;
    bool SET_MaxCopsCollapsing(const EA::Reflection::Int32 &input);

    bool MeterDeadZoneBustedDistance(TAttrib<EA::Reflection::Float> &result) const;
    bool MeterDeadZoneBustedDistance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MeterDeadZoneBustedDistance() const;
    bool SET_MeterDeadZoneBustedDistance(const EA::Reflection::Float &input);

    bool MeterDeadZoneEvadeDist(TAttrib<EA::Reflection::Float> &result) const;
    bool MeterDeadZoneEvadeDist(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MeterDeadZoneEvadeDist() const;
    bool SET_MeterDeadZoneEvadeDist(const EA::Reflection::Float &input);

    bool MilestoneCompleteHeatAdjust(TAttrib<EA::Reflection::Float> &result) const;
    bool MilestoneCompleteHeatAdjust(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MilestoneCompleteHeatAdjust() const;
    bool SET_MilestoneCompleteHeatAdjust(const EA::Reflection::Float &input);

    bool NumCiviHitsFor911(TAttrib<EA::Reflection::Int8> &result) const;
    bool NumCiviHitsFor911(EA::Reflection::Int8 &result) const;
    const EA::Reflection::Int8 &NumCiviHitsFor911() const;
    bool SET_NumCiviHitsFor911(const EA::Reflection::Int8 &input);

    bool NumCopsToTriggerBackup(TAttrib<EA::Reflection::Int32> &result) const;
    bool NumCopsToTriggerBackup(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &NumCopsToTriggerBackup() const;
    bool SET_NumCopsToTriggerBackup(const EA::Reflection::Int32 &input);

    bool NumPatrolCars(TAttrib<EA::Reflection::Int32> &result) const;
    bool NumPatrolCars(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &NumPatrolCars() const;
    bool SET_NumPatrolCars(const EA::Reflection::Int32 &input);

    bool RepPointsPerMinute(TAttrib<EA::Reflection::Int32> &result) const;
    bool RepPointsPerMinute(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &RepPointsPerMinute() const;
    bool SET_RepPointsPerMinute(const EA::Reflection::Int32 &input);

    bool RollingBlockDuration(TAttrib<EA::Reflection::Float> &result) const;
    bool RollingBlockDuration(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RollingBlockDuration() const;
    bool SET_RollingBlockDuration(const EA::Reflection::Float &input);

    bool RollingBlockTightness(TAttrib<EA::Reflection::Float> &result) const;
    bool RollingBlockTightness(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RollingBlockTightness() const;
    bool SET_RollingBlockTightness(const EA::Reflection::Float &input);

    bool ScaleEscalationPerBucket(TAttrib<EA::Reflection::Float> &result) const;
    bool ScaleEscalationPerBucket(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &ScaleEscalationPerBucket(unsigned int index) const;
    unsigned int Num_ScaleEscalationPerBucket() const;
    bool SET_ScaleEscalationPerBucket(const EA::Reflection::Float &input, unsigned int index);

    bool SearchModeCityMPH(TAttrib<EA::Reflection::Float> &result) const;
    bool SearchModeCityMPH(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SearchModeCityMPH() const;
    bool SET_SearchModeCityMPH(const EA::Reflection::Float &input);

    bool SearchModeHeliSpawnChance(TAttrib<EA::Reflection::Float> &result) const;
    bool SearchModeHeliSpawnChance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SearchModeHeliSpawnChance() const;
    bool SET_SearchModeHeliSpawnChance(const EA::Reflection::Float &input);

    bool SearchModeHwyMPH(TAttrib<EA::Reflection::Float> &result) const;
    bool SearchModeHwyMPH(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SearchModeHwyMPH() const;
    bool SET_SearchModeHwyMPH(const EA::Reflection::Float &input);

    bool SearchModeRoadblockChance(TAttrib<EA::Reflection::Float> &result) const;
    bool SearchModeRoadblockChance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SearchModeRoadblockChance() const;
    bool SET_SearchModeRoadblockChance(const EA::Reflection::Float &input);

    bool SearchModeRoadblockRadius(TAttrib<EA::Reflection::Float> &result) const;
    bool SearchModeRoadblockRadius(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SearchModeRoadblockRadius() const;
    bool SET_SearchModeRoadblockRadius(const EA::Reflection::Float &input);

    bool SirenInitMinPeriod(TAttrib<EA::Reflection::Float> &result) const;
    bool SirenInitMinPeriod(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SirenInitMinPeriod() const;
    bool SET_SirenInitMinPeriod(const EA::Reflection::Float &input);

    bool SirenInitVariation(TAttrib<EA::Reflection::Float> &result) const;
    bool SirenInitVariation(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SirenInitVariation() const;
    bool SET_SirenInitVariation(const EA::Reflection::Float &input);

    bool SirenMaxScreamTime(TAttrib<EA::Reflection::Float> &result) const;
    bool SirenMaxScreamTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SirenMaxScreamTime() const;
    bool SET_SirenMaxScreamTime(const EA::Reflection::Float &input);

    bool SirenMaxYelpTime(TAttrib<EA::Reflection::Float> &result) const;
    bool SirenMaxYelpTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SirenMaxYelpTime() const;
    bool SET_SirenMaxYelpTime(const EA::Reflection::Float &input);

    bool SirenScreamPeriod(TAttrib<EA::Reflection::Float> &result) const;
    bool SirenScreamPeriod(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SirenScreamPeriod() const;
    bool SET_SirenScreamPeriod(const EA::Reflection::Float &input);

    bool SirenWailPeriod(TAttrib<EA::Reflection::Float> &result) const;
    bool SirenWailPeriod(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SirenWailPeriod() const;
    bool SET_SirenWailPeriod(const EA::Reflection::Float &input);

    bool SpeedReactionTime(TAttrib<EA::Reflection::Float> &result) const;
    bool SpeedReactionTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &SpeedReactionTime() const;
    bool SET_SpeedReactionTime(const EA::Reflection::Float &input);

    bool StaggerFormationTime(TAttrib<EA::Reflection::Float> &result) const;
    bool StaggerFormationTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &StaggerFormationTime() const;
    bool SET_StaggerFormationTime(const EA::Reflection::Float &input);

    bool TimeBetweenCopSpawn(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeBetweenCopSpawn(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeBetweenCopSpawn() const;
    bool SET_TimeBetweenCopSpawn(const EA::Reflection::Float &input);

    bool TimeBetweenFirstFourSpawn(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeBetweenFirstFourSpawn(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeBetweenFirstFourSpawn() const;
    bool SET_TimeBetweenFirstFourSpawn(const EA::Reflection::Float &input);

    bool TimeBetweenHeliActive(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeBetweenHeliActive(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeBetweenHeliActive() const;
    bool SET_TimeBetweenHeliActive(const EA::Reflection::Float &input);

    bool TimeInactiveFor911(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeInactiveFor911(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeInactiveFor911() const;
    bool SET_TimeInactiveFor911(const EA::Reflection::Float &input);

    bool TimePerHeatLevel(TAttrib<EA::Reflection::Float> &result) const;
    bool TimePerHeatLevel(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimePerHeatLevel() const;
    bool SET_TimePerHeatLevel(const EA::Reflection::Float &input);

    bool TimeToHideInZone(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeToHideInZone(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeToHideInZone() const;
    bool SET_TimeToHideInZone(const EA::Reflection::Float &input);

    bool cops(TAttrib<CopCountRecord> &result) const;
    bool cops(CopCountRecord &result, unsigned int index) const;
    const CopCountRecord &cops(unsigned int index) const;
    unsigned int Num_cops() const;
    bool SET_cops(const CopCountRecord &input, unsigned int index);

    bool evadetimeout(TAttrib<EA::Reflection::Float> &result) const;
    bool evadetimeout(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &evadetimeout() const;
    bool SET_evadetimeout(const EA::Reflection::Float &input);

    bool formations(TAttrib<EA::Reflection::UInt32> &result) const;
    bool formations(EA::Reflection::UInt32 &result, unsigned int index) const;
    const EA::Reflection::UInt32 &formations(unsigned int index) const;
    unsigned int Num_formations() const;
    bool SET_formations(const EA::Reflection::UInt32 &input, unsigned int index);

    bool frontLOSdistance(TAttrib<EA::Reflection::Float> &result) const;
    bool frontLOSdistance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &frontLOSdistance() const;
    bool SET_frontLOSdistance(const EA::Reflection::Float &input);

    bool heliLOSdistance(TAttrib<EA::Reflection::Float> &result) const;
    bool heliLOSdistance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &heliLOSdistance() const;
    bool SET_heliLOSdistance(const EA::Reflection::Float &input);

    bool rearLOSdistance(TAttrib<EA::Reflection::Float> &result) const;
    bool rearLOSdistance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &rearLOSdistance() const;
    bool SET_rearLOSdistance(const EA::Reflection::Float &input);

    bool roadblockhelichance(TAttrib<EA::Reflection::Float> &result) const;
    bool roadblockhelichance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &roadblockhelichance() const;
    bool SET_roadblockhelichance(const EA::Reflection::Float &input);

    bool roadblockprobability(TAttrib<EA::Reflection::Float> &result) const;
    bool roadblockprobability(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &roadblockprobability() const;
    bool SET_roadblockprobability(const EA::Reflection::Float &input);

    bool roadblockspikechance(TAttrib<EA::Reflection::Float> &result) const;
    bool roadblockspikechance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &roadblockspikechance() const;
    bool SET_roadblockspikechance(const EA::Reflection::Float &input);

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

inline bool Gen::pursuitlevels::BackupCallTimer(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xac6e1ec7);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::BackupCallTimer() const {
    ATTRIB_CODEGEN_GETLAYOUT(BackupCallTimer);
}
inline bool Gen::pursuitlevels::BackupCallTimer(EA::Reflection::Float &result) const {
    result = BackupCallTimer();
    return true;
}
inline bool Gen::pursuitlevels::SET_BackupCallTimer(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(BackupCallTimer, input);
}

inline bool Gen::pursuitlevels::BoxinDuration(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x858b1097);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::BoxinDuration() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x858b1097);
}
inline bool Gen::pursuitlevels::BoxinDuration(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x858b1097, result);
}
inline bool Gen::pursuitlevels::SET_BoxinDuration(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x858b1097, input);
}

inline bool Gen::pursuitlevels::BoxinTightness(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x67a15750);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::BoxinTightness() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x67a15750);
}
inline bool Gen::pursuitlevels::BoxinTightness(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x67a15750, result);
}
inline bool Gen::pursuitlevels::SET_BoxinTightness(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x67a15750, input);
}

inline bool Gen::pursuitlevels::BustSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x769e8d9e);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::BustSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x769e8d9e);
}
inline bool Gen::pursuitlevels::BustSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x769e8d9e, result);
}
inline bool Gen::pursuitlevels::SET_BustSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x769e8d9e, input);
}

inline bool Gen::pursuitlevels::CTSFor911(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xa00de933);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::CTSFor911() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xa00de933);
}
inline bool Gen::pursuitlevels::CTSFor911(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xa00de933, result);
}
inline bool Gen::pursuitlevels::SET_CTSFor911(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xa00de933, input);
}

inline bool Gen::pursuitlevels::CollapseAggression(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x594e1492);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::CollapseAggression() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x594e1492);
}
inline bool Gen::pursuitlevels::CollapseAggression(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x594e1492, result);
}
inline bool Gen::pursuitlevels::SET_CollapseAggression(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x594e1492, input);
}

inline bool Gen::pursuitlevels::CollapseInnerRadius(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x1e0af662);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::CollapseInnerRadius() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x1e0af662);
}
inline bool Gen::pursuitlevels::CollapseInnerRadius(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x1e0af662, result);
}
inline bool Gen::pursuitlevels::SET_CollapseInnerRadius(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x1e0af662, input);
}

inline bool Gen::pursuitlevels::CollapseOuterRadius(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x947542f2);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::CollapseOuterRadius() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x947542f2);
}
inline bool Gen::pursuitlevels::CollapseOuterRadius(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x947542f2, result);
}
inline bool Gen::pursuitlevels::SET_CollapseOuterRadius(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x947542f2, input);
}

inline bool Gen::pursuitlevels::CollapseSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdb66950c);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::CollapseSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xdb66950c);
}
inline bool Gen::pursuitlevels::CollapseSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xdb66950c, result);
}
inline bool Gen::pursuitlevels::SET_CollapseSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xdb66950c, input);
}

inline bool Gen::pursuitlevels::CopFormations(TAttrib<CopFormationRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CopFormationRecord, 0x5c2a7972);
}
inline const CopFormationRecord &Gen::pursuitlevels::CopFormations(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(CopFormationRecord, 0x5c2a7972, index);
}
inline bool Gen::pursuitlevels::CopFormations(CopFormationRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(CopFormationRecord, 0x5c2a7972, result, index);
}
inline unsigned int Gen::pursuitlevels::Num_CopFormations() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5c2a7972);
}
inline bool Gen::pursuitlevels::SET_CopFormations(const CopFormationRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(CopFormationRecord, 0x5c2a7972, input, index);
}

inline bool Gen::pursuitlevels::DestroyCopBonusTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbef78612);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::DestroyCopBonusTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xbef78612);
}
inline bool Gen::pursuitlevels::DestroyCopBonusTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xbef78612, result);
}
inline bool Gen::pursuitlevels::SET_DestroyCopBonusTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xbef78612, input);
}

inline bool Gen::pursuitlevels::EvadeSuccessHeatAdjust(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb5a584d2);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::EvadeSuccessHeatAdjust() const {
    ATTRIB_CODEGEN_GETLAYOUT(EvadeSuccessHeatAdjust);
}
inline bool Gen::pursuitlevels::EvadeSuccessHeatAdjust(EA::Reflection::Float &result) const {
    result = EvadeSuccessHeatAdjust();
    return true;
}
inline bool Gen::pursuitlevels::SET_EvadeSuccessHeatAdjust(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EvadeSuccessHeatAdjust, input);
}

inline bool Gen::pursuitlevels::EventWinHeatAdjust(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7ad04eca);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::EventWinHeatAdjust() const {
    ATTRIB_CODEGEN_GETLAYOUT(EventWinHeatAdjust);
}
inline bool Gen::pursuitlevels::EventWinHeatAdjust(EA::Reflection::Float &result) const {
    result = EventWinHeatAdjust();
    return true;
}
inline bool Gen::pursuitlevels::SET_EventWinHeatAdjust(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(EventWinHeatAdjust, input);
}

inline bool Gen::pursuitlevels::FullEngagementCopCount(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x73fea6db);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::FullEngagementCopCount() const {
    ATTRIB_CODEGEN_GETLAYOUT(FullEngagementCopCount);
}
inline bool Gen::pursuitlevels::FullEngagementCopCount(EA::Reflection::Int32 &result) const {
    result = FullEngagementCopCount();
    return true;
}
inline bool Gen::pursuitlevels::SET_FullEngagementCopCount(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FullEngagementCopCount, input);
}

inline bool Gen::pursuitlevels::FullEngagementRadius(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x01cec2b4);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::FullEngagementRadius() const {
    ATTRIB_CODEGEN_GETLAYOUT(FullEngagementRadius);
}
inline bool Gen::pursuitlevels::FullEngagementRadius(EA::Reflection::Float &result) const {
    result = FullEngagementRadius();
    return true;
}
inline bool Gen::pursuitlevels::SET_FullEngagementRadius(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(FullEngagementRadius, input);
}

inline bool Gen::pursuitlevels::HeliFuelTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe766eb78);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::HeliFuelTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(HeliFuelTime);
}
inline bool Gen::pursuitlevels::HeliFuelTime(EA::Reflection::Float &result) const {
    result = HeliFuelTime();
    return true;
}
inline bool Gen::pursuitlevels::SET_HeliFuelTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(HeliFuelTime, input);
}

inline bool Gen::pursuitlevels::HiddenZoneTimeMultiplier(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7fcee250);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::HiddenZoneTimeMultiplier() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7fcee250);
}
inline bool Gen::pursuitlevels::HiddenZoneTimeMultiplier(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7fcee250, result);
}
inline bool Gen::pursuitlevels::SET_HiddenZoneTimeMultiplier(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7fcee250, input);
}

inline bool Gen::pursuitlevels::Lifetime911(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0e823327);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::Lifetime911() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0e823327);
}
inline bool Gen::pursuitlevels::Lifetime911(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0e823327, result);
}
inline bool Gen::pursuitlevels::SET_Lifetime911(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0e823327, input);
}

inline bool Gen::pursuitlevels::MaxCopsCollapsing(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0c77d463);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::MaxCopsCollapsing() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x0c77d463);
}
inline bool Gen::pursuitlevels::MaxCopsCollapsing(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x0c77d463, result);
}
inline bool Gen::pursuitlevels::SET_MaxCopsCollapsing(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x0c77d463, input);
}

inline bool Gen::pursuitlevels::MeterDeadZoneBustedDistance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8ce3219f);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::MeterDeadZoneBustedDistance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8ce3219f);
}
inline bool Gen::pursuitlevels::MeterDeadZoneBustedDistance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8ce3219f, result);
}
inline bool Gen::pursuitlevels::SET_MeterDeadZoneBustedDistance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8ce3219f, input);
}

inline bool Gen::pursuitlevels::MeterDeadZoneEvadeDist(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xeda0e29c);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::MeterDeadZoneEvadeDist() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xeda0e29c);
}
inline bool Gen::pursuitlevels::MeterDeadZoneEvadeDist(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xeda0e29c, result);
}
inline bool Gen::pursuitlevels::SET_MeterDeadZoneEvadeDist(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xeda0e29c, input);
}

inline bool Gen::pursuitlevels::MilestoneCompleteHeatAdjust(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa6cb6eff);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::MilestoneCompleteHeatAdjust() const {
    ATTRIB_CODEGEN_GETLAYOUT(MilestoneCompleteHeatAdjust);
}
inline bool Gen::pursuitlevels::MilestoneCompleteHeatAdjust(EA::Reflection::Float &result) const {
    result = MilestoneCompleteHeatAdjust();
    return true;
}
inline bool Gen::pursuitlevels::SET_MilestoneCompleteHeatAdjust(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(MilestoneCompleteHeatAdjust, input);
}

inline bool Gen::pursuitlevels::NumCiviHitsFor911(TAttrib<EA::Reflection::Int8> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int8, 0x6e590f57);
}
inline const EA::Reflection::Int8 &Gen::pursuitlevels::NumCiviHitsFor911() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int8, 0x6e590f57);
}
inline bool Gen::pursuitlevels::NumCiviHitsFor911(EA::Reflection::Int8 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int8, 0x6e590f57, result);
}
inline bool Gen::pursuitlevels::SET_NumCiviHitsFor911(const EA::Reflection::Int8 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int8, 0x6e590f57, input);
}

inline bool Gen::pursuitlevels::NumCopsToTriggerBackup(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc467015c);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::NumCopsToTriggerBackup() const {
    ATTRIB_CODEGEN_GETLAYOUT(NumCopsToTriggerBackup);
}
inline bool Gen::pursuitlevels::NumCopsToTriggerBackup(EA::Reflection::Int32 &result) const {
    result = NumCopsToTriggerBackup();
    return true;
}
inline bool Gen::pursuitlevels::SET_NumCopsToTriggerBackup(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(NumCopsToTriggerBackup, input);
}

inline bool Gen::pursuitlevels::NumPatrolCars(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x24f7a1bc);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::NumPatrolCars() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x24f7a1bc);
}
inline bool Gen::pursuitlevels::NumPatrolCars(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x24f7a1bc, result);
}
inline bool Gen::pursuitlevels::SET_NumPatrolCars(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x24f7a1bc, input);
}

inline bool Gen::pursuitlevels::RepPointsPerMinute(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x1e2a1051);
}
inline const EA::Reflection::Int32 &Gen::pursuitlevels::RepPointsPerMinute() const {
    ATTRIB_CODEGEN_GETLAYOUT(RepPointsPerMinute);
}
inline bool Gen::pursuitlevels::RepPointsPerMinute(EA::Reflection::Int32 &result) const {
    result = RepPointsPerMinute();
    return true;
}
inline bool Gen::pursuitlevels::SET_RepPointsPerMinute(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(RepPointsPerMinute, input);
}

inline bool Gen::pursuitlevels::RollingBlockDuration(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5c9f5f55);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::RollingBlockDuration() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5c9f5f55);
}
inline bool Gen::pursuitlevels::RollingBlockDuration(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5c9f5f55, result);
}
inline bool Gen::pursuitlevels::SET_RollingBlockDuration(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5c9f5f55, input);
}

inline bool Gen::pursuitlevels::RollingBlockTightness(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc146fc03);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::RollingBlockTightness() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc146fc03);
}
inline bool Gen::pursuitlevels::RollingBlockTightness(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc146fc03, result);
}
inline bool Gen::pursuitlevels::SET_RollingBlockTightness(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc146fc03, input);
}

inline bool Gen::pursuitlevels::ScaleEscalationPerBucket(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x80deb840);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::ScaleEscalationPerBucket(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x80deb840, index);
}
inline bool Gen::pursuitlevels::ScaleEscalationPerBucket(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x80deb840, result, index);
}
inline unsigned int Gen::pursuitlevels::Num_ScaleEscalationPerBucket() const {
    ATTRIB_CODEGEN_GETLENGTH(0x80deb840);
}
inline bool Gen::pursuitlevels::SET_ScaleEscalationPerBucket(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x80deb840, input, index);
}

inline bool Gen::pursuitlevels::SearchModeCityMPH(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3fa01f3d);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SearchModeCityMPH() const {
    ATTRIB_CODEGEN_GETLAYOUT(SearchModeCityMPH);
}
inline bool Gen::pursuitlevels::SearchModeCityMPH(EA::Reflection::Float &result) const {
    result = SearchModeCityMPH();
    return true;
}
inline bool Gen::pursuitlevels::SET_SearchModeCityMPH(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SearchModeCityMPH, input);
}

inline bool Gen::pursuitlevels::SearchModeHeliSpawnChance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3f11fbfc);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SearchModeHeliSpawnChance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3f11fbfc);
}
inline bool Gen::pursuitlevels::SearchModeHeliSpawnChance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3f11fbfc, result);
}
inline bool Gen::pursuitlevels::SET_SearchModeHeliSpawnChance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3f11fbfc, input);
}

inline bool Gen::pursuitlevels::SearchModeHwyMPH(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x04fcf097);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SearchModeHwyMPH() const {
    ATTRIB_CODEGEN_GETLAYOUT(SearchModeHwyMPH);
}
inline bool Gen::pursuitlevels::SearchModeHwyMPH(EA::Reflection::Float &result) const {
    result = SearchModeHwyMPH();
    return true;
}
inline bool Gen::pursuitlevels::SET_SearchModeHwyMPH(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SearchModeHwyMPH, input);
}

inline bool Gen::pursuitlevels::SearchModeRoadblockChance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf49339cd);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SearchModeRoadblockChance() const {
    ATTRIB_CODEGEN_GETLAYOUT(SearchModeRoadblockChance);
}
inline bool Gen::pursuitlevels::SearchModeRoadblockChance(EA::Reflection::Float &result) const {
    result = SearchModeRoadblockChance();
    return true;
}
inline bool Gen::pursuitlevels::SET_SearchModeRoadblockChance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SearchModeRoadblockChance, input);
}

inline bool Gen::pursuitlevels::SearchModeRoadblockRadius(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x88c3ee99);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SearchModeRoadblockRadius() const {
    ATTRIB_CODEGEN_GETLAYOUT(SearchModeRoadblockRadius);
}
inline bool Gen::pursuitlevels::SearchModeRoadblockRadius(EA::Reflection::Float &result) const {
    result = SearchModeRoadblockRadius();
    return true;
}
inline bool Gen::pursuitlevels::SET_SearchModeRoadblockRadius(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SearchModeRoadblockRadius, input);
}

inline bool Gen::pursuitlevels::SirenInitMinPeriod(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4096a291);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SirenInitMinPeriod() const {
    ATTRIB_CODEGEN_GETLAYOUT(SirenInitMinPeriod);
}
inline bool Gen::pursuitlevels::SirenInitMinPeriod(EA::Reflection::Float &result) const {
    result = SirenInitMinPeriod();
    return true;
}
inline bool Gen::pursuitlevels::SET_SirenInitMinPeriod(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SirenInitMinPeriod, input);
}

inline bool Gen::pursuitlevels::SirenInitVariation(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xba4ad395);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SirenInitVariation() const {
    ATTRIB_CODEGEN_GETLAYOUT(SirenInitVariation);
}
inline bool Gen::pursuitlevels::SirenInitVariation(EA::Reflection::Float &result) const {
    result = SirenInitVariation();
    return true;
}
inline bool Gen::pursuitlevels::SET_SirenInitVariation(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SirenInitVariation, input);
}

inline bool Gen::pursuitlevels::SirenMaxScreamTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xbb607f29);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SirenMaxScreamTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(SirenMaxScreamTime);
}
inline bool Gen::pursuitlevels::SirenMaxScreamTime(EA::Reflection::Float &result) const {
    result = SirenMaxScreamTime();
    return true;
}
inline bool Gen::pursuitlevels::SET_SirenMaxScreamTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SirenMaxScreamTime, input);
}

inline bool Gen::pursuitlevels::SirenMaxYelpTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7f154ad5);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SirenMaxYelpTime() const {
    ATTRIB_CODEGEN_GETLAYOUT(SirenMaxYelpTime);
}
inline bool Gen::pursuitlevels::SirenMaxYelpTime(EA::Reflection::Float &result) const {
    result = SirenMaxYelpTime();
    return true;
}
inline bool Gen::pursuitlevels::SET_SirenMaxYelpTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SirenMaxYelpTime, input);
}

inline bool Gen::pursuitlevels::SirenScreamPeriod(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x55e85455);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SirenScreamPeriod() const {
    ATTRIB_CODEGEN_GETLAYOUT(SirenScreamPeriod);
}
inline bool Gen::pursuitlevels::SirenScreamPeriod(EA::Reflection::Float &result) const {
    result = SirenScreamPeriod();
    return true;
}
inline bool Gen::pursuitlevels::SET_SirenScreamPeriod(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SirenScreamPeriod, input);
}

inline bool Gen::pursuitlevels::SirenWailPeriod(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3c76b17d);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SirenWailPeriod() const {
    ATTRIB_CODEGEN_GETLAYOUT(SirenWailPeriod);
}
inline bool Gen::pursuitlevels::SirenWailPeriod(EA::Reflection::Float &result) const {
    result = SirenWailPeriod();
    return true;
}
inline bool Gen::pursuitlevels::SET_SirenWailPeriod(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(SirenWailPeriod, input);
}

inline bool Gen::pursuitlevels::SpeedReactionTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0f575b64);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::SpeedReactionTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0f575b64);
}
inline bool Gen::pursuitlevels::SpeedReactionTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0f575b64, result);
}
inline bool Gen::pursuitlevels::SET_SpeedReactionTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0f575b64, input);
}

inline bool Gen::pursuitlevels::StaggerFormationTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7648c884);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::StaggerFormationTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7648c884);
}
inline bool Gen::pursuitlevels::StaggerFormationTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7648c884, result);
}
inline bool Gen::pursuitlevels::SET_StaggerFormationTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7648c884, input);
}

inline bool Gen::pursuitlevels::TimeBetweenCopSpawn(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9f3d8bee);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::TimeBetweenCopSpawn() const {
    ATTRIB_CODEGEN_GETLAYOUT(TimeBetweenCopSpawn);
}
inline bool Gen::pursuitlevels::TimeBetweenCopSpawn(EA::Reflection::Float &result) const {
    result = TimeBetweenCopSpawn();
    return true;
}
inline bool Gen::pursuitlevels::SET_TimeBetweenCopSpawn(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TimeBetweenCopSpawn, input);
}

inline bool Gen::pursuitlevels::TimeBetweenFirstFourSpawn(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9bf0f433);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::TimeBetweenFirstFourSpawn() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9bf0f433);
}
inline bool Gen::pursuitlevels::TimeBetweenFirstFourSpawn(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9bf0f433, result);
}
inline bool Gen::pursuitlevels::SET_TimeBetweenFirstFourSpawn(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9bf0f433, input);
}

inline bool Gen::pursuitlevels::TimeBetweenHeliActive(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2b154825);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::TimeBetweenHeliActive() const {
    ATTRIB_CODEGEN_GETLAYOUT(TimeBetweenHeliActive);
}
inline bool Gen::pursuitlevels::TimeBetweenHeliActive(EA::Reflection::Float &result) const {
    result = TimeBetweenHeliActive();
    return true;
}
inline bool Gen::pursuitlevels::SET_TimeBetweenHeliActive(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TimeBetweenHeliActive, input);
}

inline bool Gen::pursuitlevels::TimeInactiveFor911(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x06cb70d5);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::TimeInactiveFor911() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x06cb70d5);
}
inline bool Gen::pursuitlevels::TimeInactiveFor911(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x06cb70d5, result);
}
inline bool Gen::pursuitlevels::SET_TimeInactiveFor911(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x06cb70d5, input);
}

inline bool Gen::pursuitlevels::TimePerHeatLevel(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3c4e7ace);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::TimePerHeatLevel() const {
    ATTRIB_CODEGEN_GETLAYOUT(TimePerHeatLevel);
}
inline bool Gen::pursuitlevels::TimePerHeatLevel(EA::Reflection::Float &result) const {
    result = TimePerHeatLevel();
    return true;
}
inline bool Gen::pursuitlevels::SET_TimePerHeatLevel(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TimePerHeatLevel, input);
}

inline bool Gen::pursuitlevels::TimeToHideInZone(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xff761484);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::TimeToHideInZone() const {
    ATTRIB_CODEGEN_GETLAYOUT(TimeToHideInZone);
}
inline bool Gen::pursuitlevels::TimeToHideInZone(EA::Reflection::Float &result) const {
    result = TimeToHideInZone();
    return true;
}
inline bool Gen::pursuitlevels::SET_TimeToHideInZone(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(TimeToHideInZone, input);
}

inline bool Gen::pursuitlevels::cops(TAttrib<CopCountRecord> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(CopCountRecord, 0xa9811b93);
}
inline const CopCountRecord &Gen::pursuitlevels::cops(unsigned int index) const {
    ATTRIB_CODEGEN_GETLAYOUTINDEXED(CopCountRecord, cops, index);
}
inline bool Gen::pursuitlevels::cops(CopCountRecord &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETLAYOUTINDEXED(cops, result, index);
}
inline unsigned int Gen::pursuitlevels::Num_cops() const {
    ATTRIB_CODEGEN_GETLAYOUTLENGTH(cops);
}
inline bool Gen::pursuitlevels::SET_cops(const CopCountRecord &input, unsigned int index) {
    ATTRIB_CODEGEN_SETLAYOUTINDEXED(cops, input, index);
}

inline bool Gen::pursuitlevels::evadetimeout(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa109bcce);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::evadetimeout() const {
    ATTRIB_CODEGEN_GETLAYOUT(evadetimeout);
}
inline bool Gen::pursuitlevels::evadetimeout(EA::Reflection::Float &result) const {
    result = evadetimeout();
    return true;
}
inline bool Gen::pursuitlevels::SET_evadetimeout(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(evadetimeout, input);
}

inline bool Gen::pursuitlevels::formations(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x92f3d64e);
}
inline const EA::Reflection::UInt32 &Gen::pursuitlevels::formations(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::UInt32, 0x92f3d64e, index);
}
inline bool Gen::pursuitlevels::formations(EA::Reflection::UInt32 &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::UInt32, 0x92f3d64e, result, index);
}
inline unsigned int Gen::pursuitlevels::Num_formations() const {
    ATTRIB_CODEGEN_GETLENGTH(0x92f3d64e);
}
inline bool Gen::pursuitlevels::SET_formations(const EA::Reflection::UInt32 &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::UInt32, 0x92f3d64e, input, index);
}

inline bool Gen::pursuitlevels::frontLOSdistance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x90269ac5);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::frontLOSdistance() const {
    ATTRIB_CODEGEN_GETLAYOUT(frontLOSdistance);
}
inline bool Gen::pursuitlevels::frontLOSdistance(EA::Reflection::Float &result) const {
    result = frontLOSdistance();
    return true;
}
inline bool Gen::pursuitlevels::SET_frontLOSdistance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(frontLOSdistance, input);
}

inline bool Gen::pursuitlevels::heliLOSdistance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa0449b72);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::heliLOSdistance() const {
    ATTRIB_CODEGEN_GETLAYOUT(heliLOSdistance);
}
inline bool Gen::pursuitlevels::heliLOSdistance(EA::Reflection::Float &result) const {
    result = heliLOSdistance();
    return true;
}
inline bool Gen::pursuitlevels::SET_heliLOSdistance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(heliLOSdistance, input);
}

inline bool Gen::pursuitlevels::rearLOSdistance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xdc6b7f2b);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::rearLOSdistance() const {
    ATTRIB_CODEGEN_GETLAYOUT(rearLOSdistance);
}
inline bool Gen::pursuitlevels::rearLOSdistance(EA::Reflection::Float &result) const {
    result = rearLOSdistance();
    return true;
}
inline bool Gen::pursuitlevels::SET_rearLOSdistance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(rearLOSdistance, input);
}

inline bool Gen::pursuitlevels::roadblockhelichance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x54159af1);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::roadblockhelichance() const {
    ATTRIB_CODEGEN_GETLAYOUT(roadblockhelichance);
}
inline bool Gen::pursuitlevels::roadblockhelichance(EA::Reflection::Float &result) const {
    result = roadblockhelichance();
    return true;
}
inline bool Gen::pursuitlevels::SET_roadblockhelichance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(roadblockhelichance, input);
}

inline bool Gen::pursuitlevels::roadblockprobability(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb09d16bd);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::roadblockprobability() const {
    ATTRIB_CODEGEN_GETLAYOUT(roadblockprobability);
}
inline bool Gen::pursuitlevels::roadblockprobability(EA::Reflection::Float &result) const {
    result = roadblockprobability();
    return true;
}
inline bool Gen::pursuitlevels::SET_roadblockprobability(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETLAYOUT(roadblockprobability, input);
}

inline bool Gen::pursuitlevels::roadblockspikechance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5a318af6);
}
inline const EA::Reflection::Float &Gen::pursuitlevels::roadblockspikechance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5a318af6);
}
inline bool Gen::pursuitlevels::roadblockspikechance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5a318af6, result);
}
inline bool Gen::pursuitlevels::SET_roadblockspikechance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5a318af6, input);
}

}; // namespace Attrib

#endif
