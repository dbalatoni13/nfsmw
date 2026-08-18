#ifndef _attrib_gen_gameplay_h
#define _attrib_gen_gameplay_h

#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {
namespace Gen {

struct gameplay : Instance {
    struct _LayoutStruct {
        EA::Reflection::Text CollectionName; // offset 0x0, size 0x4
        EA::Reflection::UInt32 message_id;   // offset 0x4, size 0x4
    };

    typedef EA::Reflection::Bool TypeOf_AllowInvisibleSpawn;
    typedef EA::Reflection::Bool TypeOf_AutoActivateGPS;
    typedef EA::Reflection::Text TypeOf_AutoSpawnTriggerType;
    typedef EA::Reflection::Bool TypeOf_AutoStart;
    typedef EA::Reflection::Bool TypeOf_AvailableOnline;
    typedef EA::Reflection::Bool TypeOf_AvailableQR;
    typedef GCollectionKey TypeOf_BarrierExemptions;
    typedef EA::Reflection::Text TypeOf_Barriers;
    typedef EA::Reflection::Float TypeOf_BaseOpenWorldHeat;
    typedef GCollectionKey TypeOf_BaselineUnlocks;
    typedef EA::Reflection::Int32 TypeOf_BinIndex;
    typedef GCollectionKey TypeOf_Boss;
    typedef EA::Reflection::Bool TypeOf_BossRace;
    typedef GCollectionKey TypeOf_BossRaces;
    typedef EA::Reflection::Int32 TypeOf_BossReputation;
    typedef EA::Reflection::Int32 TypeOf_Bounty;
    typedef EA::Reflection::Int32 TypeOf_BustedLives;
    typedef GCollectionKey TypeOf_CameraModelMarker;
    typedef GCollectionKey TypeOf_CannedPath;
    typedef EA::Reflection::Text TypeOf_CarType;
    typedef EA::Reflection::Text TypeOf_CarTypeLowMem;
    typedef EA::Reflection::Float TypeOf_CashReward;
    typedef GCollectionKey TypeOf_CashRewards;
    typedef EA::Reflection::Float TypeOf_CashValue;
    typedef EA::Reflection::Bool TypeOf_CatchUp;
    typedef EA::Reflection::Float TypeOf_CatchUpDerivative;
    typedef EA::Reflection::Float TypeOf_CatchUpIntegral;
    typedef EA::Reflection::Bool TypeOf_CatchUpOverride;
    typedef EA::Reflection::Text TypeOf_CatchUpSkill;
    typedef EA::Reflection::Text TypeOf_CatchUpSpread;
    typedef GCollectionKey TypeOf_CellChallengeRace;
    typedef EA::Reflection::Bool TypeOf_ChallengeSeriesRace;
    typedef EA::Reflection::Float TypeOf_ChanceOfRain;
    typedef GCollectionKey TypeOf_Checkpoint;
    typedef EA::Reflection::Bool TypeOf_CheckpointsVisible;
    typedef GCollectionKey TypeOf_Children;
    typedef EA::Reflection::Text TypeOf_CollectionName;
    typedef EA::Reflection::Bool TypeOf_CollectorsEditionRace;
    typedef EA::Reflection::Int32 TypeOf_CopDensity;
    typedef GCollectionKey TypeOf_CopSpawnPoints;
    typedef EA::Reflection::Text TypeOf_CopSpawnType;
    typedef EA::Reflection::Bool TypeOf_CopsInRace;
    typedef EA::Reflection::Bool TypeOf_DDayRace;
    typedef GCollectionKey TypeOf_DebugJumpPoint;
    typedef EA::Reflection::Float TypeOf_DelayTime;
    typedef EA::Reflection::Int32 TypeOf_DifficultyLevel;
    typedef UMath::Vector3 TypeOf_Dimensions;
    typedef EA::Reflection::Bool TypeOf_Directional;
    typedef EA::Reflection::Bool TypeOf_DoCountdown;
    typedef EA::Reflection::Bool TypeOf_DoPhotofinish;
    typedef GCollectionKey TypeOf_EntryActivity;
    typedef EA::Reflection::Int32 TypeOf_EntryCellCallID;
    typedef EA::Reflection::Text TypeOf_EventID;
    typedef EA::Reflection::Text TypeOf_EventIconType;
    typedef GCollectionKey TypeOf_ExcludedCharacters;
    typedef Attrib::Blob TypeOf_FilterBlocks;
    typedef EA::Reflection::Bool TypeOf_FilterModePassAll;
    typedef EA::Reflection::Text TypeOf_FinishCamera;
    typedef EA::Reflection::Bool TypeOf_FireOnExit;
    typedef EA::Reflection::Float TypeOf_FlareSpacing;
    typedef EA::Reflection::Int32 TypeOf_ForceHeatLevel;
    typedef EA::Reflection::Bool TypeOf_ForcePreload;
    typedef GCollectionKey TypeOf_ForceStartPosition;
    typedef EA::Reflection::Int32 TypeOf_ForceTrafficDensity;
    typedef EA::Reflection::Bool TypeOf_FreeRoamOnly;
    typedef GCollectionKey TypeOf_GateActivity;
    typedef EA::Reflection::Float TypeOf_GoalAddPrevBest;
    typedef EA::Reflection::Float TypeOf_GoalEasy;
    typedef EA::Reflection::Float TypeOf_GoalHard;
    typedef EA::Reflection::Float TypeOf_IconModelBounceAmp;
    typedef EA::Reflection::Float TypeOf_IconModelBounceRate;
    typedef EA::Reflection::Float TypeOf_IconModelFloatHeight;
    typedef EA::Reflection::Text TypeOf_IconModelName;
    typedef EA::Reflection::Bool TypeOf_IconModelScale;
    typedef EA::Reflection::Float TypeOf_IconModelSpinRate;
    typedef EA::Reflection::Float TypeOf_InitialPlayerSpeed;
    typedef EA::Reflection::Float TypeOf_InitialSpeed;
    typedef EA::Reflection::Bool TypeOf_InitiallyUnlocked;
    typedef EA::Reflection::Float TypeOf_InternalRaceIndex;
    typedef EA::Reflection::Int32 TypeOf_IntroCameraTrack;
    typedef EA::Reflection::Int32 TypeOf_IntroMessageID;
    typedef EA::Reflection::Text TypeOf_IntroMovie;
    typedef EA::Reflection::Text TypeOf_IntroNIS;
    typedef EA::Reflection::Bool TypeOf_IsBoss;
    typedef EA::Reflection::Bool TypeOf_IsEpicPursuitRace;
    typedef EA::Reflection::Bool TypeOf_IsLoopingRace;
    typedef EA::Reflection::Bool TypeOf_IsMarkerRace;
    typedef GCollectionKey TypeOf_JumpRaces;
    typedef EA::Reflection::Float TypeOf_KnockoutTime;
    typedef EA::Reflection::Int32 TypeOf_KnockoutsPerLap;
    typedef EA::Reflection::Int32 TypeOf_LocalizationTag;
    typedef GCollectionKey TypeOf_MasterCheckpoint;
    typedef EA::Reflection::Float TypeOf_MaxCarRep;
    typedef EA::Reflection::Float TypeOf_MaxHeatLevel;
    typedef EA::Reflection::Float TypeOf_MaxOpenWorldHeat;
    typedef EA::Reflection::Float TypeOf_MaxPursuitRep;
    typedef EA::Reflection::Int32 TypeOf_MedalBonusBronze;
    typedef EA::Reflection::Int32 TypeOf_MedalBonusGold;
    typedef EA::Reflection::Int32 TypeOf_MedalBonusSilver;
    typedef EA::Reflection::Bool TypeOf_MilestoneBiggerIsBetter;
    typedef GCollectionKey TypeOf_MilestoneChallenge;
    typedef EA::Reflection::Text TypeOf_MilestoneName;
    typedef GCollectionKey TypeOf_MiniMapItem;
    typedef EA::Reflection::Float TypeOf_MinimumAIPerformance;
    typedef EA::Reflection::Bool TypeOf_NISShell;
    typedef EA::Reflection::Text TypeOf_Name;
    typedef EA::Reflection::Bool TypeOf_NeverInQuickRace;
    typedef EA::Reflection::Bool TypeOf_NoPostRaceScreen;
    typedef EA::Reflection::Int32 TypeOf_NumLaps;
    typedef EA::Reflection::Int32 TypeOf_NumRacesRequired;
    typedef EA::Reflection::Bool TypeOf_OneShot;
    typedef EA::Reflection::Bool TypeOf_OpenWorldSpeedTrap;
    typedef GCollectionKey TypeOf_Opponents;
    typedef EA::Reflection::Int32 TypeOf_OutroCameraTrack;
    typedef EA::Reflection::Int32 TypeOf_OutroMessageID;
    typedef EA::Reflection::Text TypeOf_OutroMovie;
    typedef EA::Reflection::Text TypeOf_OutroNIS;
    typedef GCollectionKey TypeOf_OutroNISMarker;
    typedef EA::Reflection::Int32 TypeOf_OvertimePenaltyPerSec;
    typedef EA::Reflection::Text TypeOf_ParticleEffect;
    typedef EA::Reflection::Bool TypeOf_Persistent;
    typedef EA::Reflection::Float TypeOf_PlayerCarPerformance;
    typedef EA::Reflection::Text TypeOf_PlayerCarType;
    typedef UMath::Vector3 TypeOf_Position;
    typedef GCollectionKey TypeOf_PostRaceActivity;
    typedef EA::Reflection::Text TypeOf_PostRaceScreenTexture;
    typedef EA::Reflection::Text TypeOf_PresetRide;
    typedef EA::Reflection::Int32 TypeOf_ProgressionLevel;
    typedef EA::Reflection::Int32 TypeOf_PursuitLevel;
    typedef EA::Reflection::Bool TypeOf_PursuitRace;
    typedef EA::Reflection::Text TypeOf_QuickRaceNIS;
    typedef EA::Reflection::Bool TypeOf_QuickRaceUnlocked;
    typedef EA::Reflection::Float TypeOf_RaceLength;
    typedef GCollectionKey TypeOf_RaceList;
    typedef GCollectionKey TypeOf_RaceTriggers;
    typedef EA::Reflection::Text TypeOf_RacerName;
    typedef EA::Reflection::Float TypeOf_Radius;
    typedef GCollectionKey TypeOf_RandomSpawnTriggers;
    typedef EA::Reflection::Bool TypeOf_RankPlayersByDistance;
    typedef EA::Reflection::Bool TypeOf_RankPlayersByPoints;
    typedef EA::Reflection::Text TypeOf_Region;
    typedef EA::Reflection::Int32 TypeOf_Reputation;
    typedef EA::Reflection::Int32 TypeOf_ReputationRequired;
    typedef EA::Reflection::Int32 TypeOf_RequiredBounty;
    typedef EA::Reflection::Int32 TypeOf_RequiredChallenges;
    typedef EA::Reflection::Int32 TypeOf_RequiredRacesWon;
    typedef EA::Reflection::Float TypeOf_ResetTime;
    typedef EA::Reflection::Bool TypeOf_ResetsPlayer;
    typedef GCollectionKey TypeOf_RespawnMarker;
    typedef GCollectionKey TypeOf_RestartActivity;
    typedef EA::Reflection::Text TypeOf_RewardMarkerType;
    typedef GCollectionKey TypeOf_RewardsForWinner;
    typedef EA::Reflection::Float TypeOf_RingTime;
    typedef EA::Reflection::Float TypeOf_RivalBestTime;
    typedef EA::Reflection::Text TypeOf_RoadList;
    typedef EA::Reflection::Bool TypeOf_RollingStart;
    typedef EA::Reflection::Float TypeOf_Rotation;
    typedef EA::Reflection::Int32 TypeOf_SMSCellChallenge;
    typedef EA::Reflection::Int32 TypeOf_SMSRivalChallenge;
    typedef EA::Reflection::Float TypeOf_ScaleOpenWorldHeat;
    typedef EA::Reflection::Bool TypeOf_ScriptedCopsInRace;
    typedef EA::Reflection::Bool TypeOf_SharedCheckpoints;
    typedef EA::Reflection::Float TypeOf_ShortcutMaxChance;
    typedef EA::Reflection::Float TypeOf_ShortcutMinChance;
    typedef GCollectionKey TypeOf_Shortcuts;
    typedef EA::Reflection::Int32 TypeOf_SkillLevel;
    typedef GCollectionKey TypeOf_SpawnPoint;
    typedef EA::Reflection::Text TypeOf_SpeedTrapCamera;
    typedef GCollectionKey TypeOf_SpeedTrapList;
    typedef GCollectionKey TypeOf_SpeedTrapTrigger;
    typedef GCollectionKey TypeOf_SpeedTrapsRequired;
    typedef EA::Reflection::Float TypeOf_StartPercent;
    typedef EA::Reflection::Float TypeOf_StartTime;
    typedef EA::Reflection::Float TypeOf_TOD;
    typedef GCollectionKey TypeOf_TargetActivities;
    typedef GCollectionKey TypeOf_TargetActivity;
    typedef EA::Reflection::Float TypeOf_TargetBronze;
    typedef EA::Reflection::Float TypeOf_TargetGold;
    typedef GCollectionKey TypeOf_TargetMarker;
    typedef EA::Reflection::Float TypeOf_TargetSilver;
    typedef EA::Reflection::Bool TypeOf_Template;
    typedef EA::Reflection::Float TypeOf_ThreshholdSpeed;
    typedef EA::Reflection::Float TypeOf_ThreshholdValue;
    typedef EA::Reflection::Int32 TypeOf_TimeBonus;
    typedef EA::Reflection::Float TypeOf_TimeLimit;
    typedef EA::Reflection::Int32 TypeOf_TokenValue;
    typedef GCollectionKey TypeOf_TrafficCharacter;
    typedef EA::Reflection::Int32 TypeOf_TrafficLevel;
    typedef EA::Reflection::Text TypeOf_TrafficPattern;
    typedef GCollectionKey TypeOf_UnlockRaces;
    typedef EA::Reflection::Int32 TypeOf_UpgradeLevel;
    typedef EA::Reflection::Text TypeOf_UpgradePartID;
    typedef EA::Reflection::Text TypeOf_UpgradePartName;
    typedef EA::Reflection::Text TypeOf_UpgradeType;
    typedef EA::Reflection::Bool TypeOf_UseWorldHeat;
    typedef EA::Reflection::Float TypeOf_Width;
    typedef GCollectionKey TypeOf_WorldRaces;
    typedef GCollectionKey TypeOf_ZoneList;
    typedef EA::Reflection::Text TypeOf_ZoneType;
    typedef GCollectionKey TypeOf_actionscript;
    typedef Attrib::Blob TypeOf_bytecode;
    typedef GCollectionKey TypeOf_disengagetrigger;
    typedef EA::Reflection::Float TypeOf_distance;
    typedef GCollectionKey TypeOf_engagetrigger;
    typedef EA::Reflection::Text TypeOf_gameplayvault;
    typedef GCollectionKey TypeOf_handler_owner;
    typedef UMath::Vector2 TypeOf_layoutpos;
    typedef EA::Reflection::UInt32 TypeOf_message_id;
    typedef EA::Reflection::Int32 TypeOf_nitrouslevel;
    typedef GCollectionKey TypeOf_parentstate;
    typedef GCollectionKey TypeOf_racefinish;
    typedef GCollectionKey TypeOf_racefinishReverse;
    typedef GCollectionKey TypeOf_racestart;
    typedef GCollectionKey TypeOf_racestartReverse;
    typedef EA::Reflection::Text TypeOf_scriptname;
    typedef GCollectionKey TypeOf_sender;
    typedef GCollectionKey TypeOf_stateref;
    typedef GCollectionKey TypeOf_target;
    typedef GCollectionKey TypeOf_templateref;
    typedef GCollectionKey TypeOf_transitionlist;

    static Key ClassKey();
    USE_ATTRIB_ALLOC("gameplay");
    gameplay(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    gameplay(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    gameplay(const Instance &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    gameplay(const gameplay &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    gameplay(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }
    ~gameplay() {}
    Instance &GetBase() {
        return *this;
    }
    const Instance &GetBase() const {
        return *this;
    }
    Key GetClass() {
        return 0x5cea9d46;
    }
    void Modify(Key dynamicCollectionKey, unsigned int spaceForAdditionalAttributes) {
        ModifyInternal(0x5cea9d46, dynamicCollectionKey, spaceForAdditionalAttributes);
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
    const gameplay &operator=(const gameplay &rhs) {
        operator=(rhs.GetBase());
        return *this;
    }
    const gameplay &operator=(const Instance &rhs) {
        Instance::operator=(rhs);
        return *this;
    }
    bool AllowInvisibleSpawn(TAttrib<EA::Reflection::Bool> &result) const;
    bool AllowInvisibleSpawn(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &AllowInvisibleSpawn() const;
    bool SET_AllowInvisibleSpawn(const EA::Reflection::Bool &input);

    bool AutoActivateGPS(TAttrib<EA::Reflection::Bool> &result) const;
    bool AutoActivateGPS(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &AutoActivateGPS() const;
    bool SET_AutoActivateGPS(const EA::Reflection::Bool &input);

    bool AutoSpawnTriggerType(TAttrib<EA::Reflection::Text> &result) const;
    bool AutoSpawnTriggerType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &AutoSpawnTriggerType() const;
    bool SET_AutoSpawnTriggerType(const EA::Reflection::Text &input);

    bool AutoStart(TAttrib<EA::Reflection::Bool> &result) const;
    bool AutoStart(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &AutoStart() const;
    bool SET_AutoStart(const EA::Reflection::Bool &input);

    bool AvailableOnline(TAttrib<EA::Reflection::Bool> &result) const;
    bool AvailableOnline(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &AvailableOnline() const;
    bool SET_AvailableOnline(const EA::Reflection::Bool &input);

    bool AvailableQR(TAttrib<EA::Reflection::Bool> &result) const;
    bool AvailableQR(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &AvailableQR() const;
    bool SET_AvailableQR(const EA::Reflection::Bool &input);

    bool BarrierExemptions(TAttrib<GCollectionKey> &result) const;
    bool BarrierExemptions(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &BarrierExemptions(unsigned int index) const;
    unsigned int Num_BarrierExemptions() const;
    bool SET_BarrierExemptions(const GCollectionKey &input, unsigned int index);

    bool Barriers(TAttrib<EA::Reflection::Text> &result) const;
    bool Barriers(EA::Reflection::Text &result, unsigned int index) const;
    const EA::Reflection::Text &Barriers(unsigned int index) const;
    unsigned int Num_Barriers() const;
    bool SET_Barriers(const EA::Reflection::Text &input, unsigned int index);

    bool BaseOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const;
    bool BaseOpenWorldHeat(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &BaseOpenWorldHeat() const;
    bool SET_BaseOpenWorldHeat(const EA::Reflection::Float &input);

    bool BaselineUnlocks(TAttrib<GCollectionKey> &result) const;
    bool BaselineUnlocks(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &BaselineUnlocks(unsigned int index) const;
    unsigned int Num_BaselineUnlocks() const;
    bool SET_BaselineUnlocks(const GCollectionKey &input, unsigned int index);

    bool BinIndex(TAttrib<EA::Reflection::Int32> &result) const;
    bool BinIndex(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &BinIndex() const;
    bool SET_BinIndex(const EA::Reflection::Int32 &input);

    bool Boss(TAttrib<GCollectionKey> &result) const;
    bool Boss(GCollectionKey &result) const;
    const GCollectionKey &Boss() const;
    bool SET_Boss(const GCollectionKey &input);

    bool BossRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool BossRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &BossRace() const;
    bool SET_BossRace(const EA::Reflection::Bool &input);

    bool BossRaces(TAttrib<GCollectionKey> &result) const;
    bool BossRaces(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &BossRaces(unsigned int index) const;
    unsigned int Num_BossRaces() const;
    bool SET_BossRaces(const GCollectionKey &input, unsigned int index);

    bool BossReputation(TAttrib<EA::Reflection::Int32> &result) const;
    bool BossReputation(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &BossReputation() const;
    bool SET_BossReputation(const EA::Reflection::Int32 &input);

    bool Bounty(TAttrib<EA::Reflection::Int32> &result) const;
    bool Bounty(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &Bounty() const;
    bool SET_Bounty(const EA::Reflection::Int32 &input);

    bool BustedLives(TAttrib<EA::Reflection::Int32> &result) const;
    bool BustedLives(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &BustedLives() const;
    bool SET_BustedLives(const EA::Reflection::Int32 &input);

    bool CameraModelMarker(TAttrib<GCollectionKey> &result) const;
    bool CameraModelMarker(GCollectionKey &result) const;
    const GCollectionKey &CameraModelMarker() const;
    bool SET_CameraModelMarker(const GCollectionKey &input);

    bool CannedPath(TAttrib<GCollectionKey> &result) const;
    bool CannedPath(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &CannedPath(unsigned int index) const;
    unsigned int Num_CannedPath() const;
    bool SET_CannedPath(const GCollectionKey &input, unsigned int index);

    bool CarType(TAttrib<EA::Reflection::Text> &result) const;
    bool CarType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CarType() const;
    bool SET_CarType(const EA::Reflection::Text &input);

    bool CarTypeLowMem(TAttrib<EA::Reflection::Text> &result) const;
    bool CarTypeLowMem(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CarTypeLowMem() const;
    bool SET_CarTypeLowMem(const EA::Reflection::Text &input);

    bool CashReward(TAttrib<EA::Reflection::Float> &result) const;
    bool CashReward(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CashReward() const;
    bool SET_CashReward(const EA::Reflection::Float &input);

    bool CashRewards(TAttrib<GCollectionKey> &result) const;
    bool CashRewards(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &CashRewards(unsigned int index) const;
    unsigned int Num_CashRewards() const;
    bool SET_CashRewards(const GCollectionKey &input, unsigned int index);

    bool CashValue(TAttrib<EA::Reflection::Float> &result) const;
    bool CashValue(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CashValue() const;
    bool SET_CashValue(const EA::Reflection::Float &input);

    bool CatchUp(TAttrib<EA::Reflection::Bool> &result) const;
    bool CatchUp(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &CatchUp() const;
    bool SET_CatchUp(const EA::Reflection::Bool &input);

    bool CatchUpDerivative(TAttrib<EA::Reflection::Float> &result) const;
    bool CatchUpDerivative(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CatchUpDerivative() const;
    bool SET_CatchUpDerivative(const EA::Reflection::Float &input);

    bool CatchUpIntegral(TAttrib<EA::Reflection::Float> &result) const;
    bool CatchUpIntegral(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &CatchUpIntegral() const;
    bool SET_CatchUpIntegral(const EA::Reflection::Float &input);

    bool CatchUpOverride(TAttrib<EA::Reflection::Bool> &result) const;
    bool CatchUpOverride(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &CatchUpOverride() const;
    bool SET_CatchUpOverride(const EA::Reflection::Bool &input);

    bool CatchUpSkill(TAttrib<EA::Reflection::Text> &result) const;
    bool CatchUpSkill(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CatchUpSkill() const;
    bool SET_CatchUpSkill(const EA::Reflection::Text &input);

    bool CatchUpSpread(TAttrib<EA::Reflection::Text> &result) const;
    bool CatchUpSpread(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CatchUpSpread() const;
    bool SET_CatchUpSpread(const EA::Reflection::Text &input);

    bool CellChallengeRace(TAttrib<GCollectionKey> &result) const;
    bool CellChallengeRace(GCollectionKey &result) const;
    const GCollectionKey &CellChallengeRace() const;
    bool SET_CellChallengeRace(const GCollectionKey &input);

    bool ChallengeSeriesRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool ChallengeSeriesRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &ChallengeSeriesRace() const;
    bool SET_ChallengeSeriesRace(const EA::Reflection::Bool &input);

    bool ChanceOfRain(TAttrib<EA::Reflection::Float> &result) const;
    bool ChanceOfRain(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ChanceOfRain() const;
    bool SET_ChanceOfRain(const EA::Reflection::Float &input);

    bool Checkpoint(TAttrib<GCollectionKey> &result) const;
    bool Checkpoint(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &Checkpoint(unsigned int index) const;
    unsigned int Num_Checkpoint() const;
    bool SET_Checkpoint(const GCollectionKey &input, unsigned int index);

    bool CheckpointsVisible(TAttrib<EA::Reflection::Bool> &result) const;
    bool CheckpointsVisible(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &CheckpointsVisible() const;
    bool SET_CheckpointsVisible(const EA::Reflection::Bool &input);

    bool Children(TAttrib<GCollectionKey> &result) const;
    bool Children(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &Children(unsigned int index) const;
    unsigned int Num_Children() const;
    bool SET_Children(const GCollectionKey &input, unsigned int index);

    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const;
    bool CollectionName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CollectionName() const;
    bool SET_CollectionName(const EA::Reflection::Text &input);

    bool CollectorsEditionRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool CollectorsEditionRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &CollectorsEditionRace() const;
    bool SET_CollectorsEditionRace(const EA::Reflection::Bool &input);

    bool CopDensity(TAttrib<EA::Reflection::Int32> &result) const;
    bool CopDensity(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &CopDensity() const;
    bool SET_CopDensity(const EA::Reflection::Int32 &input);

    bool CopSpawnPoints(TAttrib<GCollectionKey> &result) const;
    bool CopSpawnPoints(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &CopSpawnPoints(unsigned int index) const;
    unsigned int Num_CopSpawnPoints() const;
    bool SET_CopSpawnPoints(const GCollectionKey &input, unsigned int index);

    bool CopSpawnType(TAttrib<EA::Reflection::Text> &result) const;
    bool CopSpawnType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &CopSpawnType() const;
    bool SET_CopSpawnType(const EA::Reflection::Text &input);

    bool CopsInRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool CopsInRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &CopsInRace() const;
    bool SET_CopsInRace(const EA::Reflection::Bool &input);

    bool DDayRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool DDayRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &DDayRace() const;
    bool SET_DDayRace(const EA::Reflection::Bool &input);

    bool DebugJumpPoint(TAttrib<GCollectionKey> &result) const;
    bool DebugJumpPoint(GCollectionKey &result) const;
    const GCollectionKey &DebugJumpPoint() const;
    bool SET_DebugJumpPoint(const GCollectionKey &input);

    bool DelayTime(TAttrib<EA::Reflection::Float> &result) const;
    bool DelayTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &DelayTime() const;
    bool SET_DelayTime(const EA::Reflection::Float &input);

    bool DifficultyLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool DifficultyLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &DifficultyLevel() const;
    bool SET_DifficultyLevel(const EA::Reflection::Int32 &input);

    bool Dimensions(TAttrib<UMath::Vector3> &result) const;
    bool Dimensions(UMath::Vector3 &result) const;
    const UMath::Vector3 &Dimensions() const;
    bool SET_Dimensions(const UMath::Vector3 &input);

    bool Directional(TAttrib<EA::Reflection::Bool> &result) const;
    bool Directional(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &Directional() const;
    bool SET_Directional(const EA::Reflection::Bool &input);

    bool DoCountdown(TAttrib<EA::Reflection::Bool> &result) const;
    bool DoCountdown(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &DoCountdown() const;
    bool SET_DoCountdown(const EA::Reflection::Bool &input);

    bool DoPhotofinish(TAttrib<EA::Reflection::Bool> &result) const;
    bool DoPhotofinish(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &DoPhotofinish() const;
    bool SET_DoPhotofinish(const EA::Reflection::Bool &input);

    bool EntryActivity(TAttrib<GCollectionKey> &result) const;
    bool EntryActivity(GCollectionKey &result) const;
    const GCollectionKey &EntryActivity() const;
    bool SET_EntryActivity(const GCollectionKey &input);

    bool EntryCellCallID(TAttrib<EA::Reflection::Int32> &result) const;
    bool EntryCellCallID(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &EntryCellCallID() const;
    bool SET_EntryCellCallID(const EA::Reflection::Int32 &input);

    bool EventID(TAttrib<EA::Reflection::Text> &result) const;
    bool EventID(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &EventID() const;
    bool SET_EventID(const EA::Reflection::Text &input);

    bool EventIconType(TAttrib<EA::Reflection::Text> &result) const;
    bool EventIconType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &EventIconType() const;
    bool SET_EventIconType(const EA::Reflection::Text &input);

    bool ExcludedCharacters(TAttrib<GCollectionKey> &result) const;
    bool ExcludedCharacters(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &ExcludedCharacters(unsigned int index) const;
    unsigned int Num_ExcludedCharacters() const;
    bool SET_ExcludedCharacters(const GCollectionKey &input, unsigned int index);

    bool FilterBlocks(TAttrib<Attrib::Blob> &result) const;
    bool FilterBlocks(Attrib::Blob &result, unsigned int index) const;
    const Attrib::Blob &FilterBlocks(unsigned int index) const;
    unsigned int Num_FilterBlocks() const;
    bool SET_FilterBlocks(const Attrib::Blob &input, unsigned int index);

    bool FilterModePassAll(TAttrib<EA::Reflection::Bool> &result) const;
    bool FilterModePassAll(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &FilterModePassAll() const;
    bool SET_FilterModePassAll(const EA::Reflection::Bool &input);

    bool FinishCamera(TAttrib<EA::Reflection::Text> &result) const;
    bool FinishCamera(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &FinishCamera() const;
    bool SET_FinishCamera(const EA::Reflection::Text &input);

    bool FireOnExit(TAttrib<EA::Reflection::Bool> &result) const;
    bool FireOnExit(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &FireOnExit() const;
    bool SET_FireOnExit(const EA::Reflection::Bool &input);

    bool FlareSpacing(TAttrib<EA::Reflection::Float> &result) const;
    bool FlareSpacing(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &FlareSpacing() const;
    bool SET_FlareSpacing(const EA::Reflection::Float &input);

    bool ForceHeatLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool ForceHeatLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &ForceHeatLevel() const;
    bool SET_ForceHeatLevel(const EA::Reflection::Int32 &input);

    bool ForcePreload(TAttrib<EA::Reflection::Bool> &result) const;
    bool ForcePreload(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &ForcePreload() const;
    bool SET_ForcePreload(const EA::Reflection::Bool &input);

    bool ForceStartPosition(TAttrib<GCollectionKey> &result) const;
    bool ForceStartPosition(GCollectionKey &result) const;
    const GCollectionKey &ForceStartPosition() const;
    bool SET_ForceStartPosition(const GCollectionKey &input);

    bool ForceTrafficDensity(TAttrib<EA::Reflection::Int32> &result) const;
    bool ForceTrafficDensity(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &ForceTrafficDensity() const;
    bool SET_ForceTrafficDensity(const EA::Reflection::Int32 &input);

    bool FreeRoamOnly(TAttrib<EA::Reflection::Bool> &result) const;
    bool FreeRoamOnly(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &FreeRoamOnly() const;
    bool SET_FreeRoamOnly(const EA::Reflection::Bool &input);

    bool GateActivity(TAttrib<GCollectionKey> &result) const;
    bool GateActivity(GCollectionKey &result) const;
    const GCollectionKey &GateActivity() const;
    bool SET_GateActivity(const GCollectionKey &input);

    bool GoalAddPrevBest(TAttrib<EA::Reflection::Float> &result) const;
    bool GoalAddPrevBest(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GoalAddPrevBest() const;
    bool SET_GoalAddPrevBest(const EA::Reflection::Float &input);

    bool GoalEasy(TAttrib<EA::Reflection::Float> &result) const;
    bool GoalEasy(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GoalEasy() const;
    bool SET_GoalEasy(const EA::Reflection::Float &input);

    bool GoalHard(TAttrib<EA::Reflection::Float> &result) const;
    bool GoalHard(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &GoalHard() const;
    bool SET_GoalHard(const EA::Reflection::Float &input);

    bool IconModelBounceAmp(TAttrib<EA::Reflection::Float> &result) const;
    bool IconModelBounceAmp(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &IconModelBounceAmp() const;
    bool SET_IconModelBounceAmp(const EA::Reflection::Float &input);

    bool IconModelBounceRate(TAttrib<EA::Reflection::Float> &result) const;
    bool IconModelBounceRate(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &IconModelBounceRate() const;
    bool SET_IconModelBounceRate(const EA::Reflection::Float &input);

    bool IconModelFloatHeight(TAttrib<EA::Reflection::Float> &result) const;
    bool IconModelFloatHeight(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &IconModelFloatHeight() const;
    bool SET_IconModelFloatHeight(const EA::Reflection::Float &input);

    bool IconModelName(TAttrib<EA::Reflection::Text> &result) const;
    bool IconModelName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &IconModelName() const;
    bool SET_IconModelName(const EA::Reflection::Text &input);

    bool IconModelScale(TAttrib<EA::Reflection::Bool> &result) const;
    bool IconModelScale(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IconModelScale() const;
    bool SET_IconModelScale(const EA::Reflection::Bool &input);

    bool IconModelSpinRate(TAttrib<EA::Reflection::Float> &result) const;
    bool IconModelSpinRate(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &IconModelSpinRate() const;
    bool SET_IconModelSpinRate(const EA::Reflection::Float &input);

    bool InitialPlayerSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool InitialPlayerSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &InitialPlayerSpeed() const;
    bool SET_InitialPlayerSpeed(const EA::Reflection::Float &input);

    bool InitialSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool InitialSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &InitialSpeed() const;
    bool SET_InitialSpeed(const EA::Reflection::Float &input);

    bool InitiallyUnlocked(TAttrib<EA::Reflection::Bool> &result) const;
    bool InitiallyUnlocked(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &InitiallyUnlocked() const;
    bool SET_InitiallyUnlocked(const EA::Reflection::Bool &input);

    bool InternalRaceIndex(TAttrib<EA::Reflection::Float> &result) const;
    bool InternalRaceIndex(EA::Reflection::Float &result, unsigned int index) const;
    const EA::Reflection::Float &InternalRaceIndex(unsigned int index) const;
    unsigned int Num_InternalRaceIndex() const;
    bool SET_InternalRaceIndex(const EA::Reflection::Float &input, unsigned int index);

    bool IntroCameraTrack(TAttrib<EA::Reflection::Int32> &result) const;
    bool IntroCameraTrack(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &IntroCameraTrack() const;
    bool SET_IntroCameraTrack(const EA::Reflection::Int32 &input);

    bool IntroMessageID(TAttrib<EA::Reflection::Int32> &result) const;
    bool IntroMessageID(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &IntroMessageID() const;
    bool SET_IntroMessageID(const EA::Reflection::Int32 &input);

    bool IntroMovie(TAttrib<EA::Reflection::Text> &result) const;
    bool IntroMovie(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &IntroMovie() const;
    bool SET_IntroMovie(const EA::Reflection::Text &input);

    bool IntroNIS(TAttrib<EA::Reflection::Text> &result) const;
    bool IntroNIS(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &IntroNIS() const;
    bool SET_IntroNIS(const EA::Reflection::Text &input);

    bool IsBoss(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsBoss(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsBoss() const;
    bool SET_IsBoss(const EA::Reflection::Bool &input);

    bool IsEpicPursuitRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsEpicPursuitRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsEpicPursuitRace() const;
    bool SET_IsEpicPursuitRace(const EA::Reflection::Bool &input);

    bool IsLoopingRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsLoopingRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsLoopingRace() const;
    bool SET_IsLoopingRace(const EA::Reflection::Bool &input);

    bool IsMarkerRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool IsMarkerRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &IsMarkerRace() const;
    bool SET_IsMarkerRace(const EA::Reflection::Bool &input);

    bool JumpRaces(TAttrib<GCollectionKey> &result) const;
    bool JumpRaces(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &JumpRaces(unsigned int index) const;
    unsigned int Num_JumpRaces() const;
    bool SET_JumpRaces(const GCollectionKey &input, unsigned int index);

    bool KnockoutTime(TAttrib<EA::Reflection::Float> &result) const;
    bool KnockoutTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &KnockoutTime() const;
    bool SET_KnockoutTime(const EA::Reflection::Float &input);

    bool KnockoutsPerLap(TAttrib<EA::Reflection::Int32> &result) const;
    bool KnockoutsPerLap(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &KnockoutsPerLap() const;
    bool SET_KnockoutsPerLap(const EA::Reflection::Int32 &input);

    bool LocalizationTag(TAttrib<EA::Reflection::Int32> &result) const;
    bool LocalizationTag(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &LocalizationTag() const;
    bool SET_LocalizationTag(const EA::Reflection::Int32 &input);

    bool MasterCheckpoint(TAttrib<GCollectionKey> &result) const;
    bool MasterCheckpoint(GCollectionKey &result) const;
    const GCollectionKey &MasterCheckpoint() const;
    bool SET_MasterCheckpoint(const GCollectionKey &input);

    bool MaxCarRep(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxCarRep(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxCarRep() const;
    bool SET_MaxCarRep(const EA::Reflection::Float &input);

    bool MaxHeatLevel(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxHeatLevel(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxHeatLevel() const;
    bool SET_MaxHeatLevel(const EA::Reflection::Float &input);

    bool MaxOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxOpenWorldHeat(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxOpenWorldHeat() const;
    bool SET_MaxOpenWorldHeat(const EA::Reflection::Float &input);

    bool MaxPursuitRep(TAttrib<EA::Reflection::Float> &result) const;
    bool MaxPursuitRep(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MaxPursuitRep() const;
    bool SET_MaxPursuitRep(const EA::Reflection::Float &input);

    bool MedalBonusBronze(TAttrib<EA::Reflection::Int32> &result) const;
    bool MedalBonusBronze(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MedalBonusBronze() const;
    bool SET_MedalBonusBronze(const EA::Reflection::Int32 &input);

    bool MedalBonusGold(TAttrib<EA::Reflection::Int32> &result) const;
    bool MedalBonusGold(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MedalBonusGold() const;
    bool SET_MedalBonusGold(const EA::Reflection::Int32 &input);

    bool MedalBonusSilver(TAttrib<EA::Reflection::Int32> &result) const;
    bool MedalBonusSilver(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &MedalBonusSilver() const;
    bool SET_MedalBonusSilver(const EA::Reflection::Int32 &input);

    bool MilestoneBiggerIsBetter(TAttrib<EA::Reflection::Bool> &result) const;
    bool MilestoneBiggerIsBetter(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &MilestoneBiggerIsBetter() const;
    bool SET_MilestoneBiggerIsBetter(const EA::Reflection::Bool &input);

    bool MilestoneChallenge(TAttrib<GCollectionKey> &result) const;
    bool MilestoneChallenge(GCollectionKey &result) const;
    const GCollectionKey &MilestoneChallenge() const;
    bool SET_MilestoneChallenge(const GCollectionKey &input);

    bool MilestoneName(TAttrib<EA::Reflection::Text> &result) const;
    bool MilestoneName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &MilestoneName() const;
    bool SET_MilestoneName(const EA::Reflection::Text &input);

    bool MiniMapItem(TAttrib<GCollectionKey> &result) const;
    bool MiniMapItem(GCollectionKey &result) const;
    const GCollectionKey &MiniMapItem() const;
    bool SET_MiniMapItem(const GCollectionKey &input);

    bool MinimumAIPerformance(TAttrib<EA::Reflection::Float> &result) const;
    bool MinimumAIPerformance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &MinimumAIPerformance() const;
    bool SET_MinimumAIPerformance(const EA::Reflection::Float &input);

    bool NISShell(TAttrib<EA::Reflection::Bool> &result) const;
    bool NISShell(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NISShell() const;
    bool SET_NISShell(const EA::Reflection::Bool &input);

    bool Name(TAttrib<EA::Reflection::Text> &result) const;
    bool Name(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &Name() const;
    bool SET_Name(const EA::Reflection::Text &input);

    bool NeverInQuickRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool NeverInQuickRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NeverInQuickRace() const;
    bool SET_NeverInQuickRace(const EA::Reflection::Bool &input);

    bool NoPostRaceScreen(TAttrib<EA::Reflection::Bool> &result) const;
    bool NoPostRaceScreen(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &NoPostRaceScreen() const;
    bool SET_NoPostRaceScreen(const EA::Reflection::Bool &input);

    bool NumLaps(TAttrib<EA::Reflection::Int32> &result) const;
    bool NumLaps(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &NumLaps() const;
    bool SET_NumLaps(const EA::Reflection::Int32 &input);

    bool NumRacesRequired(TAttrib<EA::Reflection::Int32> &result) const;
    bool NumRacesRequired(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &NumRacesRequired() const;
    bool SET_NumRacesRequired(const EA::Reflection::Int32 &input);

    bool OneShot(TAttrib<EA::Reflection::Bool> &result) const;
    bool OneShot(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &OneShot() const;
    bool SET_OneShot(const EA::Reflection::Bool &input);

    bool OpenWorldSpeedTrap(TAttrib<EA::Reflection::Bool> &result) const;
    bool OpenWorldSpeedTrap(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &OpenWorldSpeedTrap() const;
    bool SET_OpenWorldSpeedTrap(const EA::Reflection::Bool &input);

    bool Opponents(TAttrib<GCollectionKey> &result) const;
    bool Opponents(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &Opponents(unsigned int index) const;
    unsigned int Num_Opponents() const;
    bool SET_Opponents(const GCollectionKey &input, unsigned int index);

    bool OutroCameraTrack(TAttrib<EA::Reflection::Int32> &result) const;
    bool OutroCameraTrack(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &OutroCameraTrack() const;
    bool SET_OutroCameraTrack(const EA::Reflection::Int32 &input);

    bool OutroMessageID(TAttrib<EA::Reflection::Int32> &result) const;
    bool OutroMessageID(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &OutroMessageID() const;
    bool SET_OutroMessageID(const EA::Reflection::Int32 &input);

    bool OutroMovie(TAttrib<EA::Reflection::Text> &result) const;
    bool OutroMovie(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &OutroMovie() const;
    bool SET_OutroMovie(const EA::Reflection::Text &input);

    bool OutroNIS(TAttrib<EA::Reflection::Text> &result) const;
    bool OutroNIS(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &OutroNIS() const;
    bool SET_OutroNIS(const EA::Reflection::Text &input);

    bool OutroNISMarker(TAttrib<GCollectionKey> &result) const;
    bool OutroNISMarker(GCollectionKey &result) const;
    const GCollectionKey &OutroNISMarker() const;
    bool SET_OutroNISMarker(const GCollectionKey &input);

    bool OvertimePenaltyPerSec(TAttrib<EA::Reflection::Int32> &result) const;
    bool OvertimePenaltyPerSec(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &OvertimePenaltyPerSec() const;
    bool SET_OvertimePenaltyPerSec(const EA::Reflection::Int32 &input);

    bool ParticleEffect(TAttrib<EA::Reflection::Text> &result) const;
    bool ParticleEffect(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &ParticleEffect() const;
    bool SET_ParticleEffect(const EA::Reflection::Text &input);

    bool Persistent(TAttrib<EA::Reflection::Bool> &result) const;
    bool Persistent(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &Persistent() const;
    bool SET_Persistent(const EA::Reflection::Bool &input);

    bool PlayerCarPerformance(TAttrib<EA::Reflection::Float> &result) const;
    bool PlayerCarPerformance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &PlayerCarPerformance() const;
    bool SET_PlayerCarPerformance(const EA::Reflection::Float &input);

    bool PlayerCarType(TAttrib<EA::Reflection::Text> &result) const;
    bool PlayerCarType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &PlayerCarType() const;
    bool SET_PlayerCarType(const EA::Reflection::Text &input);

    bool Position(TAttrib<UMath::Vector3> &result) const;
    bool Position(UMath::Vector3 &result) const;
    const UMath::Vector3 &Position() const;
    bool SET_Position(const UMath::Vector3 &input);

    bool PostRaceActivity(TAttrib<GCollectionKey> &result) const;
    bool PostRaceActivity(GCollectionKey &result) const;
    const GCollectionKey &PostRaceActivity() const;
    bool SET_PostRaceActivity(const GCollectionKey &input);

    bool PostRaceScreenTexture(TAttrib<EA::Reflection::Text> &result) const;
    bool PostRaceScreenTexture(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &PostRaceScreenTexture() const;
    bool SET_PostRaceScreenTexture(const EA::Reflection::Text &input);

    bool PresetRide(TAttrib<EA::Reflection::Text> &result) const;
    bool PresetRide(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &PresetRide() const;
    bool SET_PresetRide(const EA::Reflection::Text &input);

    bool ProgressionLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool ProgressionLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &ProgressionLevel() const;
    bool SET_ProgressionLevel(const EA::Reflection::Int32 &input);

    bool PursuitLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool PursuitLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &PursuitLevel() const;
    bool SET_PursuitLevel(const EA::Reflection::Int32 &input);

    bool PursuitRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool PursuitRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &PursuitRace() const;
    bool SET_PursuitRace(const EA::Reflection::Bool &input);

    bool QuickRaceNIS(TAttrib<EA::Reflection::Text> &result) const;
    bool QuickRaceNIS(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &QuickRaceNIS() const;
    bool SET_QuickRaceNIS(const EA::Reflection::Text &input);

    bool QuickRaceUnlocked(TAttrib<EA::Reflection::Bool> &result) const;
    bool QuickRaceUnlocked(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &QuickRaceUnlocked() const;
    bool SET_QuickRaceUnlocked(const EA::Reflection::Bool &input);

    bool RaceLength(TAttrib<EA::Reflection::Float> &result) const;
    bool RaceLength(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RaceLength() const;
    bool SET_RaceLength(const EA::Reflection::Float &input);

    bool RaceList(TAttrib<GCollectionKey> &result) const;
    bool RaceList(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &RaceList(unsigned int index) const;
    unsigned int Num_RaceList() const;
    bool SET_RaceList(const GCollectionKey &input, unsigned int index);

    bool RaceTriggers(TAttrib<GCollectionKey> &result) const;
    bool RaceTriggers(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &RaceTriggers(unsigned int index) const;
    unsigned int Num_RaceTriggers() const;
    bool SET_RaceTriggers(const GCollectionKey &input, unsigned int index);

    bool RacerName(TAttrib<EA::Reflection::Text> &result) const;
    bool RacerName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &RacerName() const;
    bool SET_RacerName(const EA::Reflection::Text &input);

    bool Radius(TAttrib<EA::Reflection::Float> &result) const;
    bool Radius(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Radius() const;
    bool SET_Radius(const EA::Reflection::Float &input);

    bool RandomSpawnTriggers(TAttrib<GCollectionKey> &result) const;
    bool RandomSpawnTriggers(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &RandomSpawnTriggers(unsigned int index) const;
    unsigned int Num_RandomSpawnTriggers() const;
    bool SET_RandomSpawnTriggers(const GCollectionKey &input, unsigned int index);

    bool RankPlayersByDistance(TAttrib<EA::Reflection::Bool> &result) const;
    bool RankPlayersByDistance(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &RankPlayersByDistance() const;
    bool SET_RankPlayersByDistance(const EA::Reflection::Bool &input);

    bool RankPlayersByPoints(TAttrib<EA::Reflection::Bool> &result) const;
    bool RankPlayersByPoints(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &RankPlayersByPoints() const;
    bool SET_RankPlayersByPoints(const EA::Reflection::Bool &input);

    bool Region(TAttrib<EA::Reflection::Text> &result) const;
    bool Region(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &Region() const;
    bool SET_Region(const EA::Reflection::Text &input);

    bool Reputation(TAttrib<EA::Reflection::Int32> &result) const;
    bool Reputation(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &Reputation() const;
    bool SET_Reputation(const EA::Reflection::Int32 &input);

    bool ReputationRequired(TAttrib<EA::Reflection::Int32> &result) const;
    bool ReputationRequired(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &ReputationRequired() const;
    bool SET_ReputationRequired(const EA::Reflection::Int32 &input);

    bool RequiredBounty(TAttrib<EA::Reflection::Int32> &result) const;
    bool RequiredBounty(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &RequiredBounty() const;
    bool SET_RequiredBounty(const EA::Reflection::Int32 &input);

    bool RequiredChallenges(TAttrib<EA::Reflection::Int32> &result) const;
    bool RequiredChallenges(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &RequiredChallenges() const;
    bool SET_RequiredChallenges(const EA::Reflection::Int32 &input);

    bool RequiredRacesWon(TAttrib<EA::Reflection::Int32> &result) const;
    bool RequiredRacesWon(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &RequiredRacesWon() const;
    bool SET_RequiredRacesWon(const EA::Reflection::Int32 &input);

    bool ResetTime(TAttrib<EA::Reflection::Float> &result) const;
    bool ResetTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ResetTime() const;
    bool SET_ResetTime(const EA::Reflection::Float &input);

    bool ResetsPlayer(TAttrib<EA::Reflection::Bool> &result) const;
    bool ResetsPlayer(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &ResetsPlayer() const;
    bool SET_ResetsPlayer(const EA::Reflection::Bool &input);

    bool RespawnMarker(TAttrib<GCollectionKey> &result) const;
    bool RespawnMarker(GCollectionKey &result) const;
    const GCollectionKey &RespawnMarker() const;
    bool SET_RespawnMarker(const GCollectionKey &input);

    bool RestartActivity(TAttrib<GCollectionKey> &result) const;
    bool RestartActivity(GCollectionKey &result) const;
    const GCollectionKey &RestartActivity() const;
    bool SET_RestartActivity(const GCollectionKey &input);

    bool RewardMarkerType(TAttrib<EA::Reflection::Text> &result) const;
    bool RewardMarkerType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &RewardMarkerType() const;
    bool SET_RewardMarkerType(const EA::Reflection::Text &input);

    bool RewardsForWinner(TAttrib<GCollectionKey> &result) const;
    bool RewardsForWinner(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &RewardsForWinner(unsigned int index) const;
    unsigned int Num_RewardsForWinner() const;
    bool SET_RewardsForWinner(const GCollectionKey &input, unsigned int index);

    bool RingTime(TAttrib<EA::Reflection::Float> &result) const;
    bool RingTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RingTime() const;
    bool SET_RingTime(const EA::Reflection::Float &input);

    bool RivalBestTime(TAttrib<EA::Reflection::Float> &result) const;
    bool RivalBestTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &RivalBestTime() const;
    bool SET_RivalBestTime(const EA::Reflection::Float &input);

    bool RoadList(TAttrib<EA::Reflection::Text> &result) const;
    bool RoadList(EA::Reflection::Text &result, unsigned int index) const;
    const EA::Reflection::Text &RoadList(unsigned int index) const;
    unsigned int Num_RoadList() const;
    bool SET_RoadList(const EA::Reflection::Text &input, unsigned int index);

    bool RollingStart(TAttrib<EA::Reflection::Bool> &result) const;
    bool RollingStart(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &RollingStart() const;
    bool SET_RollingStart(const EA::Reflection::Bool &input);

    bool Rotation(TAttrib<EA::Reflection::Float> &result) const;
    bool Rotation(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Rotation() const;
    bool SET_Rotation(const EA::Reflection::Float &input);

    bool SMSCellChallenge(TAttrib<EA::Reflection::Int32> &result) const;
    bool SMSCellChallenge(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &SMSCellChallenge() const;
    bool SET_SMSCellChallenge(const EA::Reflection::Int32 &input);

    bool SMSRivalChallenge(TAttrib<EA::Reflection::Int32> &result) const;
    bool SMSRivalChallenge(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &SMSRivalChallenge() const;
    bool SET_SMSRivalChallenge(const EA::Reflection::Int32 &input);

    bool ScaleOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const;
    bool ScaleOpenWorldHeat(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ScaleOpenWorldHeat() const;
    bool SET_ScaleOpenWorldHeat(const EA::Reflection::Float &input);

    bool ScriptedCopsInRace(TAttrib<EA::Reflection::Bool> &result) const;
    bool ScriptedCopsInRace(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &ScriptedCopsInRace() const;
    bool SET_ScriptedCopsInRace(const EA::Reflection::Bool &input);

    bool SharedCheckpoints(TAttrib<EA::Reflection::Bool> &result) const;
    bool SharedCheckpoints(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &SharedCheckpoints() const;
    bool SET_SharedCheckpoints(const EA::Reflection::Bool &input);

    bool ShortcutMaxChance(TAttrib<EA::Reflection::Float> &result) const;
    bool ShortcutMaxChance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ShortcutMaxChance() const;
    bool SET_ShortcutMaxChance(const EA::Reflection::Float &input);

    bool ShortcutMinChance(TAttrib<EA::Reflection::Float> &result) const;
    bool ShortcutMinChance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ShortcutMinChance() const;
    bool SET_ShortcutMinChance(const EA::Reflection::Float &input);

    bool Shortcuts(TAttrib<GCollectionKey> &result) const;
    bool Shortcuts(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &Shortcuts(unsigned int index) const;
    unsigned int Num_Shortcuts() const;
    bool SET_Shortcuts(const GCollectionKey &input, unsigned int index);

    bool SkillLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool SkillLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &SkillLevel() const;
    bool SET_SkillLevel(const EA::Reflection::Int32 &input);

    bool SpawnPoint(TAttrib<GCollectionKey> &result) const;
    bool SpawnPoint(GCollectionKey &result) const;
    const GCollectionKey &SpawnPoint() const;
    bool SET_SpawnPoint(const GCollectionKey &input);

    bool SpeedTrapCamera(TAttrib<EA::Reflection::Text> &result) const;
    bool SpeedTrapCamera(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &SpeedTrapCamera() const;
    bool SET_SpeedTrapCamera(const EA::Reflection::Text &input);

    bool SpeedTrapList(TAttrib<GCollectionKey> &result) const;
    bool SpeedTrapList(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &SpeedTrapList(unsigned int index) const;
    unsigned int Num_SpeedTrapList() const;
    bool SET_SpeedTrapList(const GCollectionKey &input, unsigned int index);

    bool SpeedTrapTrigger(TAttrib<GCollectionKey> &result) const;
    bool SpeedTrapTrigger(GCollectionKey &result) const;
    const GCollectionKey &SpeedTrapTrigger() const;
    bool SET_SpeedTrapTrigger(const GCollectionKey &input);

    bool SpeedTrapsRequired(TAttrib<GCollectionKey> &result) const;
    bool SpeedTrapsRequired(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &SpeedTrapsRequired(unsigned int index) const;
    unsigned int Num_SpeedTrapsRequired() const;
    bool SET_SpeedTrapsRequired(const GCollectionKey &input, unsigned int index);

    bool StartPercent(TAttrib<EA::Reflection::Float> &result) const;
    bool StartPercent(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &StartPercent() const;
    bool SET_StartPercent(const EA::Reflection::Float &input);

    bool StartTime(TAttrib<EA::Reflection::Float> &result) const;
    bool StartTime(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &StartTime() const;
    bool SET_StartTime(const EA::Reflection::Float &input);

    bool TOD(TAttrib<EA::Reflection::Float> &result) const;
    bool TOD(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TOD() const;
    bool SET_TOD(const EA::Reflection::Float &input);

    bool TargetActivities(TAttrib<GCollectionKey> &result) const;
    bool TargetActivities(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &TargetActivities(unsigned int index) const;
    unsigned int Num_TargetActivities() const;
    bool SET_TargetActivities(const GCollectionKey &input, unsigned int index);

    bool TargetActivity(TAttrib<GCollectionKey> &result) const;
    bool TargetActivity(GCollectionKey &result) const;
    const GCollectionKey &TargetActivity() const;
    bool SET_TargetActivity(const GCollectionKey &input);

    bool TargetBronze(TAttrib<EA::Reflection::Float> &result) const;
    bool TargetBronze(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TargetBronze() const;
    bool SET_TargetBronze(const EA::Reflection::Float &input);

    bool TargetGold(TAttrib<EA::Reflection::Float> &result) const;
    bool TargetGold(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TargetGold() const;
    bool SET_TargetGold(const EA::Reflection::Float &input);

    bool TargetMarker(TAttrib<GCollectionKey> &result) const;
    bool TargetMarker(GCollectionKey &result) const;
    const GCollectionKey &TargetMarker() const;
    bool SET_TargetMarker(const GCollectionKey &input);

    bool TargetSilver(TAttrib<EA::Reflection::Float> &result) const;
    bool TargetSilver(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TargetSilver() const;
    bool SET_TargetSilver(const EA::Reflection::Float &input);

    bool Template(TAttrib<EA::Reflection::Bool> &result) const;
    bool Template(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &Template() const;
    bool SET_Template(const EA::Reflection::Bool &input);

    bool ThreshholdSpeed(TAttrib<EA::Reflection::Float> &result) const;
    bool ThreshholdSpeed(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ThreshholdSpeed() const;
    bool SET_ThreshholdSpeed(const EA::Reflection::Float &input);

    bool ThreshholdValue(TAttrib<EA::Reflection::Float> &result) const;
    bool ThreshholdValue(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &ThreshholdValue() const;
    bool SET_ThreshholdValue(const EA::Reflection::Float &input);

    bool TimeBonus(TAttrib<EA::Reflection::Int32> &result) const;
    bool TimeBonus(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &TimeBonus() const;
    bool SET_TimeBonus(const EA::Reflection::Int32 &input);

    bool TimeLimit(TAttrib<EA::Reflection::Float> &result) const;
    bool TimeLimit(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &TimeLimit() const;
    bool SET_TimeLimit(const EA::Reflection::Float &input);

    bool TokenValue(TAttrib<EA::Reflection::Int32> &result) const;
    bool TokenValue(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &TokenValue() const;
    bool SET_TokenValue(const EA::Reflection::Int32 &input);

    bool TrafficCharacter(TAttrib<GCollectionKey> &result) const;
    bool TrafficCharacter(GCollectionKey &result) const;
    const GCollectionKey &TrafficCharacter() const;
    bool SET_TrafficCharacter(const GCollectionKey &input);

    bool TrafficLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool TrafficLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &TrafficLevel() const;
    bool SET_TrafficLevel(const EA::Reflection::Int32 &input);

    bool TrafficPattern(TAttrib<EA::Reflection::Text> &result) const;
    bool TrafficPattern(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &TrafficPattern() const;
    bool SET_TrafficPattern(const EA::Reflection::Text &input);

    bool UnlockRaces(TAttrib<GCollectionKey> &result) const;
    bool UnlockRaces(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &UnlockRaces(unsigned int index) const;
    unsigned int Num_UnlockRaces() const;
    bool SET_UnlockRaces(const GCollectionKey &input, unsigned int index);

    bool UpgradeLevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool UpgradeLevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &UpgradeLevel() const;
    bool SET_UpgradeLevel(const EA::Reflection::Int32 &input);

    bool UpgradePartID(TAttrib<EA::Reflection::Text> &result) const;
    bool UpgradePartID(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &UpgradePartID() const;
    bool SET_UpgradePartID(const EA::Reflection::Text &input);

    bool UpgradePartName(TAttrib<EA::Reflection::Text> &result) const;
    bool UpgradePartName(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &UpgradePartName() const;
    bool SET_UpgradePartName(const EA::Reflection::Text &input);

    bool UpgradeType(TAttrib<EA::Reflection::Text> &result) const;
    bool UpgradeType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &UpgradeType() const;
    bool SET_UpgradeType(const EA::Reflection::Text &input);

    bool UseWorldHeat(TAttrib<EA::Reflection::Bool> &result) const;
    bool UseWorldHeat(EA::Reflection::Bool &result) const;
    const EA::Reflection::Bool &UseWorldHeat() const;
    bool SET_UseWorldHeat(const EA::Reflection::Bool &input);

    bool Width(TAttrib<EA::Reflection::Float> &result) const;
    bool Width(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &Width() const;
    bool SET_Width(const EA::Reflection::Float &input);

    bool WorldRaces(TAttrib<GCollectionKey> &result) const;
    bool WorldRaces(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &WorldRaces(unsigned int index) const;
    unsigned int Num_WorldRaces() const;
    bool SET_WorldRaces(const GCollectionKey &input, unsigned int index);

    bool ZoneList(TAttrib<GCollectionKey> &result) const;
    bool ZoneList(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &ZoneList(unsigned int index) const;
    unsigned int Num_ZoneList() const;
    bool SET_ZoneList(const GCollectionKey &input, unsigned int index);

    bool ZoneType(TAttrib<EA::Reflection::Text> &result) const;
    bool ZoneType(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &ZoneType() const;
    bool SET_ZoneType(const EA::Reflection::Text &input);

    bool actionscript(TAttrib<GCollectionKey> &result) const;
    bool actionscript(GCollectionKey &result) const;
    const GCollectionKey &actionscript() const;
    bool SET_actionscript(const GCollectionKey &input);

    bool bytecode(TAttrib<Attrib::Blob> &result) const;
    bool bytecode(Attrib::Blob &result) const;
    const Attrib::Blob &bytecode() const;
    bool SET_bytecode(const Attrib::Blob &input);

    bool disengagetrigger(TAttrib<GCollectionKey> &result) const;
    bool disengagetrigger(GCollectionKey &result) const;
    const GCollectionKey &disengagetrigger() const;
    bool SET_disengagetrigger(const GCollectionKey &input);

    bool distance(TAttrib<EA::Reflection::Float> &result) const;
    bool distance(EA::Reflection::Float &result) const;
    const EA::Reflection::Float &distance() const;
    bool SET_distance(const EA::Reflection::Float &input);

    bool engagetrigger(TAttrib<GCollectionKey> &result) const;
    bool engagetrigger(GCollectionKey &result) const;
    const GCollectionKey &engagetrigger() const;
    bool SET_engagetrigger(const GCollectionKey &input);

    bool gameplayvault(TAttrib<EA::Reflection::Text> &result) const;
    bool gameplayvault(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &gameplayvault() const;
    bool SET_gameplayvault(const EA::Reflection::Text &input);

    bool handler_owner(TAttrib<GCollectionKey> &result) const;
    bool handler_owner(GCollectionKey &result) const;
    const GCollectionKey &handler_owner() const;
    bool SET_handler_owner(const GCollectionKey &input);

    bool layoutpos(TAttrib<UMath::Vector2> &result) const;
    bool layoutpos(UMath::Vector2 &result) const;
    const UMath::Vector2 &layoutpos() const;
    bool SET_layoutpos(const UMath::Vector2 &input);

    bool message_id(TAttrib<EA::Reflection::UInt32> &result) const;
    bool message_id(EA::Reflection::UInt32 &result) const;
    const EA::Reflection::UInt32 &message_id() const;
    bool SET_message_id(const EA::Reflection::UInt32 &input);

    bool nitrouslevel(TAttrib<EA::Reflection::Int32> &result) const;
    bool nitrouslevel(EA::Reflection::Int32 &result) const;
    const EA::Reflection::Int32 &nitrouslevel() const;
    bool SET_nitrouslevel(const EA::Reflection::Int32 &input);

    bool parentstate(TAttrib<GCollectionKey> &result) const;
    bool parentstate(GCollectionKey &result) const;
    const GCollectionKey &parentstate() const;
    bool SET_parentstate(const GCollectionKey &input);

    bool racefinish(TAttrib<GCollectionKey> &result) const;
    bool racefinish(GCollectionKey &result) const;
    const GCollectionKey &racefinish() const;
    bool SET_racefinish(const GCollectionKey &input);

    bool racefinishReverse(TAttrib<GCollectionKey> &result) const;
    bool racefinishReverse(GCollectionKey &result) const;
    const GCollectionKey &racefinishReverse() const;
    bool SET_racefinishReverse(const GCollectionKey &input);

    bool racestart(TAttrib<GCollectionKey> &result) const;
    bool racestart(GCollectionKey &result) const;
    const GCollectionKey &racestart() const;
    bool SET_racestart(const GCollectionKey &input);

    bool racestartReverse(TAttrib<GCollectionKey> &result) const;
    bool racestartReverse(GCollectionKey &result) const;
    const GCollectionKey &racestartReverse() const;
    bool SET_racestartReverse(const GCollectionKey &input);

    bool scriptname(TAttrib<EA::Reflection::Text> &result) const;
    bool scriptname(EA::Reflection::Text &result) const;
    const EA::Reflection::Text &scriptname() const;
    bool SET_scriptname(const EA::Reflection::Text &input);

    bool sender(TAttrib<GCollectionKey> &result) const;
    bool sender(GCollectionKey &result) const;
    const GCollectionKey &sender() const;
    bool SET_sender(const GCollectionKey &input);

    bool stateref(TAttrib<GCollectionKey> &result) const;
    bool stateref(GCollectionKey &result) const;
    const GCollectionKey &stateref() const;
    bool SET_stateref(const GCollectionKey &input);

    bool target(TAttrib<GCollectionKey> &result) const;
    bool target(GCollectionKey &result) const;
    const GCollectionKey &target() const;
    bool SET_target(const GCollectionKey &input);

    bool templateref(TAttrib<GCollectionKey> &result) const;
    bool templateref(GCollectionKey &result) const;
    const GCollectionKey &templateref() const;
    bool SET_templateref(const GCollectionKey &input);

    bool transitionlist(TAttrib<GCollectionKey> &result) const;
    bool transitionlist(GCollectionKey &result, unsigned int index) const;
    const GCollectionKey &transitionlist(unsigned int index) const;
    unsigned int Num_transitionlist() const;
    bool SET_transitionlist(const GCollectionKey &input, unsigned int index);

  private:
    unsigned int GetLayoutSize() {
        return sizeof(_LayoutStruct);
    }
    gameplay &ConvertFromInstance(Instance &src) {}
    const gameplay &ConvertFromInstance(const Instance &src) {}
};
}; // namespace Gen

namespace ClassName {

static const Key gameplay = 0x5cea9d46;

}; // namespace ClassName

namespace Hash {
namespace gameplay {

static const Key AllowInvisibleSpawn = 0xa1009a23;
static const Key AutoActivateGPS = 0x29b9c312;
static const Key AutoSpawnTriggerType = 0xd5c7e9c3;
static const Key AutoStart = 0x883c65e3;
static const Key AvailableOnline = 0x39509746;
static const Key AvailableQR = 0xb39ed8c3;
static const Key BarrierExemptions = 0xf380286b;
static const Key Barriers = 0xe244f26b;
static const Key BaseOpenWorldHeat = 0x8f186ac4;
static const Key BaselineUnlocks = 0xbaf89280;
static const Key BinIndex = 0x6ce23062;
static const Key Boss = 0xa5f39dc7;
static const Key BossRace = 0xff5ee5d6;
static const Key BossRaces = 0xd5a174aa;
static const Key BossReputation = 0x1d33241a;
static const Key Bounty = 0x8e1904c7;
static const Key BustedLives = 0x462f2e36;
static const Key CameraModelMarker = 0x52dc742c;
static const Key CannedPath = 0x9c19e56f;
static const Key CarType = 0xf833c06f;
static const Key CarTypeLowMem = 0xfd3cf790;
static const Key CashReward = 0xab0179f4;
static const Key CashRewards = 0x0550fbc2;
static const Key CashValue = 0xd8baa07b;
static const Key CatchUp = 0x10db04e6;
static const Key CatchUpDerivative = 0x515aa4e4;
static const Key CatchUpIntegral = 0x4545ab74;
static const Key CatchUpOverride = 0x9eb17c1e;
static const Key CatchUpSkill = 0x8069b5a9;
static const Key CatchUpSpread = 0xa18a07ba;
static const Key CellChallengeRace = 0xc686cd34;
static const Key ChallengeSeriesRace = 0x1c650104;
static const Key ChanceOfRain = 0x547486ae;
static const Key Checkpoint = 0x34aae3fc;
static const Key CheckpointsVisible = 0x2ad67092;
static const Key Children = 0x916e0e78;
static const Key CollectionName = 0x9ca1c8f9;
static const Key CollectorsEditionRace = 0x637584fe;
static const Key CopDensity = 0xdbc08d32;
static const Key CopSpawnPoints = 0xf124b151;
static const Key CopSpawnType = 0xd686d61e;
static const Key CopsInRace = 0x3918e889;
static const Key DDayRace = 0x8cb01abf;
static const Key DebugJumpPoint = 0xa7e9e456;
static const Key DelayTime = 0x20259346;
static const Key DifficultyLevel = 0x88a7e3be;
static const Key Dimensions = 0x6d9e21ad;
static const Key Directional = 0x6b37e124;
static const Key DoCountdown = 0x3e33da0f;
static const Key DoPhotofinish = 0xede6017e;
static const Key EntryActivity = 0xc27dfda8;
static const Key EntryCellCallID = 0xe2d26232;
static const Key EventID = 0xa78403ec;
static const Key EventIconType = 0x0f6bcde1;
static const Key ExcludedCharacters = 0xa5c5d25b;
static const Key FilterBlocks = 0x56e1436d;
static const Key FilterModePassAll = 0x0d038cfa;
static const Key FinishCamera = 0x62dfc259;
static const Key FireOnExit = 0xb2ac32c7;
static const Key FlareSpacing = 0x394abbc6;
static const Key ForceHeatLevel = 0xe4211f4f;
static const Key ForcePreload = 0x9652af0f;
static const Key ForceStartPosition = 0xfb84be75;
static const Key ForceTrafficDensity = 0x5e161bba;
static const Key FreeRoamOnly = 0xaa0135e9;
static const Key GateActivity = 0xc795b8d4;
static const Key GoalAddPrevBest = 0x006ec903;
static const Key GoalEasy = 0x8445af47;
static const Key GoalHard = 0x3b9bbfc2;
static const Key IconModelBounceAmp = 0xd5f4eda2;
static const Key IconModelBounceRate = 0xb4985085;
static const Key IconModelFloatHeight = 0x4037d3c5;
static const Key IconModelName = 0xa62cb4f0;
static const Key IconModelScale = 0xcd41cd40;
static const Key IconModelSpinRate = 0x697332e8;
static const Key InitialPlayerSpeed = 0x3a0e4b19;
static const Key InitialSpeed = 0x0a91596d;
static const Key InitiallyUnlocked = 0xea855eaf;
static const Key InternalRaceIndex = 0x0d4c1055;
static const Key IntroCameraTrack = 0x36bbeee9;
static const Key IntroMessageID = 0x5468366d;
static const Key IntroMovie = 0xf572ede8;
static const Key IntroNIS = 0xdec18d3e;
static const Key IsBoss = 0x79c5d68d;
static const Key IsEpicPursuitRace = 0x4393f69b;
static const Key IsLoopingRace = 0x6a9a6f5b;
static const Key IsMarkerRace = 0xf2fe50d7;
static const Key JumpRaces = 0xb671abb6;
static const Key KnockoutTime = 0x777ece27;
static const Key KnockoutsPerLap = 0x181462da;
static const Key LocalizationTag = 0xdb89ab5c;
static const Key MasterCheckpoint = 0x609febe8;
static const Key MaxCarRep = 0xe10fb7a3;
static const Key MaxHeatLevel = 0xf5a03629;
static const Key MaxOpenWorldHeat = 0xe8c24416;
static const Key MaxPursuitRep = 0xa07ae814;
static const Key MedalBonusBronze = 0x3bb31211;
static const Key MedalBonusGold = 0xf5e43987;
static const Key MedalBonusSilver = 0x767b00a9;
static const Key MilestoneBiggerIsBetter = 0x0896d043;
static const Key MilestoneChallenge = 0xbcd98737;
static const Key MilestoneName = 0x704f72e8;
static const Key MiniMapItem = 0x051e90ca;
static const Key MinimumAIPerformance = 0xb1ece070;
static const Key NISShell = 0x4c17fe41;
static const Key Name = 0x3e225ec1;
static const Key NeverInQuickRace = 0xa4e6fcfd;
static const Key NoPostRaceScreen = 0x40f9929f;
static const Key NumLaps = 0x0ebdc165;
static const Key NumRacesRequired = 0x90a22a3f;
static const Key OneShot = 0xce4261ac;
static const Key OpenWorldSpeedTrap = 0x1bb16f14;
static const Key Opponents = 0x5839fa1a;
static const Key OutroCameraTrack = 0x7054ff5b;
static const Key OutroMessageID = 0xc36e3532;
static const Key OutroMovie = 0xb70268c0;
static const Key OutroNIS = 0x54932966;
static const Key OutroNISMarker = 0x0e265c88;
static const Key OvertimePenaltyPerSec = 0x26fd42b0;
static const Key ParticleEffect = 0x5ef34802;
static const Key Persistent = 0xe4542e9b;
static const Key PlayerCarPerformance = 0xfb42c0b9;
static const Key PlayerCarType = 0xc0eeb909;
static const Key Position = 0x9f743a0e;
static const Key PostRaceActivity = 0x64273c71;
static const Key PostRaceScreenTexture = 0x038a3b53;
static const Key PresetRide = 0x416a8409;
static const Key ProgressionLevel = 0x740e9b4a;
static const Key PursuitLevel = 0x0261ae99;
static const Key PursuitRace = 0x2b1f54f6;
static const Key QuickRaceNIS = 0x5987fb25;
static const Key QuickRaceUnlocked = 0xc4db4e71;
static const Key RaceLength = 0x7c11c52e;
static const Key RaceList = 0x9f914008;
static const Key RaceTriggers = 0x58dc14c0;
static const Key RacerName = 0xbeab64c5;
static const Key Radius = 0x39bf8002;
static const Key RandomSpawnTriggers = 0xfdfe1c3e;
static const Key RankPlayersByDistance = 0x9e7a18ce;
static const Key RankPlayersByPoints = 0x5ec1880f;
static const Key Region = 0xcb01e454;
static const Key Reputation = 0x477ec5aa;
static const Key ReputationRequired = 0xffd69c94;
static const Key RequiredBounty = 0xd3657d92;
static const Key RequiredChallenges = 0x6dd4b98b;
static const Key RequiredRacesWon = 0xd617fedc;
static const Key ResetTime = 0x2c44ff10;
static const Key ResetsPlayer = 0x6ccd5819;
static const Key RespawnMarker = 0x2241f4cd;
static const Key RestartActivity = 0xdc44bd08;
static const Key RewardMarkerType = 0x06a077d5;
static const Key RewardsForWinner = 0x50104d90;
static const Key RingTime = 0xc516e9c2;
static const Key RivalBestTime = 0xf9120d73;
static const Key RoadList = 0x13b11b40;
static const Key RollingStart = 0xb809d19c;
static const Key Rotation = 0x5a6a57c6;
static const Key SMSCellChallenge = 0x6a4cd2d4;
static const Key SMSRivalChallenge = 0xbb30c804;
static const Key ScaleOpenWorldHeat = 0x1823b89e;
static const Key ScriptedCopsInRace = 0x0e47fe63;
static const Key SharedCheckpoints = 0x0e34a1f3;
static const Key ShortcutMaxChance = 0x16faba11;
static const Key ShortcutMinChance = 0x4efb950a;
static const Key Shortcuts = 0x7b6d296e;
static const Key SkillLevel = 0x3b798aa2;
static const Key SpawnPoint = 0xddf411f5;
static const Key SpeedTrapCamera = 0xcbd7adf9;
static const Key SpeedTrapList = 0x822179d1;
static const Key SpeedTrapTrigger = 0x5f95c3a0;
static const Key SpeedTrapsRequired = 0x6d7e73c9;
static const Key StartPercent = 0xe0d01505;
static const Key StartTime = 0x839602ab;
static const Key TOD = 0x9dff3c3d;
static const Key TargetActivities = 0x0f37d221;
static const Key TargetActivity = 0x277566f3;
static const Key TargetBronze = 0x00df8eb4;
static const Key TargetGold = 0x728e43ff;
static const Key TargetMarker = 0x8fc356fb;
static const Key TargetSilver = 0x51ce16b7;
static const Key Template = 0x3e9156ca;
static const Key ThreshholdSpeed = 0xc3710777;
static const Key ThreshholdValue = 0x4e90219d;
static const Key TimeBonus = 0xf52cc30e;
static const Key TimeLimit = 0x7585f041;
static const Key TokenValue = 0xaa10914c;
static const Key TrafficCharacter = 0x797d9654;
static const Key TrafficLevel = 0xc64bc341;
static const Key TrafficPattern = 0x6319b692;
static const Key UnlockRaces = 0xfc8995c8;
static const Key UpgradeLevel = 0xd267facc;
static const Key UpgradePartID = 0x3c2fdaab;
static const Key UpgradePartName = 0xc385f75d;
static const Key UpgradeType = 0x0e0113fe;
static const Key UseWorldHeat = 0x45f2ad6c;
static const Key Width = 0x5816c1fc;
static const Key WorldRaces = 0xa7ef40ef;
static const Key ZoneList = 0x64893da8;
static const Key ZoneType = 0xf3ea3201;
static const Key actionscript = 0xe62083d0;
static const Key bytecode = 0x9a4a020a;
static const Key disengagetrigger = 0x106285c0;
static const Key distance = 0xc5857615;
static const Key engagetrigger = 0xf05931ab;
static const Key gameplayvault = 0x93fd9fda;
static const Key handler_owner = 0x857fe432;
static const Key layoutpos = 0x4075ec46;
static const Key message_id = 0x9e8910ef;
static const Key nitrouslevel = 0xd4797aa8;
static const Key parentstate = 0x4acc6d63;
static const Key racefinish = 0xb0a24adc;
static const Key racefinishReverse = 0x7c7cf20f;
static const Key racestart = 0xe43b2ccc;
static const Key racestartReverse = 0xfd945479;
static const Key scriptname = 0x7148ae82;
static const Key sender = 0xa590a98b;
static const Key stateref = 0x918c796e;
static const Key target = 0x1a7d2859;
static const Key templateref = 0xa0697302;
static const Key transitionlist = 0x25621dc5;

}; // namespace gameplay
}; // namespace Hash

inline Key Gen::gameplay::ClassKey() {
    return ClassName::gameplay;
}

inline bool Gen::gameplay::AllowInvisibleSpawn(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xa1009a23);
}
inline const EA::Reflection::Bool &Gen::gameplay::AllowInvisibleSpawn() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xa1009a23);
}
inline bool Gen::gameplay::AllowInvisibleSpawn(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xa1009a23, result);
}
inline bool Gen::gameplay::SET_AllowInvisibleSpawn(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xa1009a23, input);
}

inline bool Gen::gameplay::AutoActivateGPS(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x29b9c312);
}
inline const EA::Reflection::Bool &Gen::gameplay::AutoActivateGPS() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x29b9c312);
}
inline bool Gen::gameplay::AutoActivateGPS(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x29b9c312, result);
}
inline bool Gen::gameplay::SET_AutoActivateGPS(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x29b9c312, input);
}

inline bool Gen::gameplay::AutoSpawnTriggerType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xd5c7e9c3);
}
inline const EA::Reflection::Text &Gen::gameplay::AutoSpawnTriggerType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xd5c7e9c3);
}
inline bool Gen::gameplay::AutoSpawnTriggerType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xd5c7e9c3, result);
}
inline bool Gen::gameplay::SET_AutoSpawnTriggerType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xd5c7e9c3, input);
}

inline bool Gen::gameplay::AutoStart(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x883c65e3);
}
inline const EA::Reflection::Bool &Gen::gameplay::AutoStart() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x883c65e3);
}
inline bool Gen::gameplay::AutoStart(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x883c65e3, result);
}
inline bool Gen::gameplay::SET_AutoStart(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x883c65e3, input);
}

inline bool Gen::gameplay::AvailableOnline(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x39509746);
}
inline const EA::Reflection::Bool &Gen::gameplay::AvailableOnline() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x39509746);
}
inline bool Gen::gameplay::AvailableOnline(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x39509746, result);
}
inline bool Gen::gameplay::SET_AvailableOnline(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x39509746, input);
}

inline bool Gen::gameplay::AvailableQR(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb39ed8c3);
}
inline const EA::Reflection::Bool &Gen::gameplay::AvailableQR() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb39ed8c3);
}
inline bool Gen::gameplay::AvailableQR(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb39ed8c3, result);
}
inline bool Gen::gameplay::SET_AvailableQR(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb39ed8c3, input);
}

inline bool Gen::gameplay::BarrierExemptions(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xf380286b);
}
inline const GCollectionKey &Gen::gameplay::BarrierExemptions(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xf380286b, index);
}
inline bool Gen::gameplay::BarrierExemptions(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xf380286b, result, index);
}
inline unsigned int Gen::gameplay::Num_BarrierExemptions() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf380286b);
}
inline bool Gen::gameplay::SET_BarrierExemptions(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xf380286b, input, index);
}

inline bool Gen::gameplay::Barriers(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xe244f26b);
}
inline const EA::Reflection::Text &Gen::gameplay::Barriers(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Text, 0xe244f26b, index);
}
inline bool Gen::gameplay::Barriers(EA::Reflection::Text &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Text, 0xe244f26b, result, index);
}
inline unsigned int Gen::gameplay::Num_Barriers() const {
    ATTRIB_CODEGEN_GETLENGTH(0xe244f26b);
}
inline bool Gen::gameplay::SET_Barriers(const EA::Reflection::Text &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Text, 0xe244f26b, input, index);
}

inline bool Gen::gameplay::BaseOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8f186ac4);
}
inline const EA::Reflection::Float &Gen::gameplay::BaseOpenWorldHeat() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8f186ac4);
}
inline bool Gen::gameplay::BaseOpenWorldHeat(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8f186ac4, result);
}
inline bool Gen::gameplay::SET_BaseOpenWorldHeat(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8f186ac4, input);
}

inline bool Gen::gameplay::BaselineUnlocks(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xbaf89280);
}
inline const GCollectionKey &Gen::gameplay::BaselineUnlocks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xbaf89280, index);
}
inline bool Gen::gameplay::BaselineUnlocks(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xbaf89280, result, index);
}
inline unsigned int Gen::gameplay::Num_BaselineUnlocks() const {
    ATTRIB_CODEGEN_GETLENGTH(0xbaf89280);
}
inline bool Gen::gameplay::SET_BaselineUnlocks(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xbaf89280, input, index);
}

inline bool Gen::gameplay::BinIndex(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x6ce23062);
}
inline const EA::Reflection::Int32 &Gen::gameplay::BinIndex() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x6ce23062);
}
inline bool Gen::gameplay::BinIndex(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x6ce23062, result);
}
inline bool Gen::gameplay::SET_BinIndex(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x6ce23062, input);
}

inline bool Gen::gameplay::Boss(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa5f39dc7);
}
inline const GCollectionKey &Gen::gameplay::Boss() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa5f39dc7);
}
inline bool Gen::gameplay::Boss(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa5f39dc7, result);
}
inline bool Gen::gameplay::SET_Boss(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa5f39dc7, input);
}

inline bool Gen::gameplay::BossRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xff5ee5d6);
}
inline const EA::Reflection::Bool &Gen::gameplay::BossRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xff5ee5d6);
}
inline bool Gen::gameplay::BossRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xff5ee5d6, result);
}
inline bool Gen::gameplay::SET_BossRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xff5ee5d6, input);
}

inline bool Gen::gameplay::BossRaces(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xd5a174aa);
}
inline const GCollectionKey &Gen::gameplay::BossRaces(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xd5a174aa, index);
}
inline bool Gen::gameplay::BossRaces(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xd5a174aa, result, index);
}
inline unsigned int Gen::gameplay::Num_BossRaces() const {
    ATTRIB_CODEGEN_GETLENGTH(0xd5a174aa);
}
inline bool Gen::gameplay::SET_BossRaces(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xd5a174aa, input, index);
}

inline bool Gen::gameplay::BossReputation(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x1d33241a);
}
inline const EA::Reflection::Int32 &Gen::gameplay::BossReputation() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x1d33241a);
}
inline bool Gen::gameplay::BossReputation(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x1d33241a, result);
}
inline bool Gen::gameplay::SET_BossReputation(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x1d33241a, input);
}

inline bool Gen::gameplay::Bounty(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x8e1904c7);
}
inline const EA::Reflection::Int32 &Gen::gameplay::Bounty() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x8e1904c7);
}
inline bool Gen::gameplay::Bounty(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x8e1904c7, result);
}
inline bool Gen::gameplay::SET_Bounty(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x8e1904c7, input);
}

inline bool Gen::gameplay::BustedLives(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x462f2e36);
}
inline const EA::Reflection::Int32 &Gen::gameplay::BustedLives() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x462f2e36);
}
inline bool Gen::gameplay::BustedLives(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x462f2e36, result);
}
inline bool Gen::gameplay::SET_BustedLives(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x462f2e36, input);
}

inline bool Gen::gameplay::CameraModelMarker(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x52dc742c);
}
inline const GCollectionKey &Gen::gameplay::CameraModelMarker() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x52dc742c);
}
inline bool Gen::gameplay::CameraModelMarker(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x52dc742c, result);
}
inline bool Gen::gameplay::SET_CameraModelMarker(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x52dc742c, input);
}

inline bool Gen::gameplay::CannedPath(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x9c19e56f);
}
inline const GCollectionKey &Gen::gameplay::CannedPath(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x9c19e56f, index);
}
inline bool Gen::gameplay::CannedPath(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x9c19e56f, result, index);
}
inline unsigned int Gen::gameplay::Num_CannedPath() const {
    ATTRIB_CODEGEN_GETLENGTH(0x9c19e56f);
}
inline bool Gen::gameplay::SET_CannedPath(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x9c19e56f, input, index);
}

inline bool Gen::gameplay::CarType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xf833c06f);
}
inline const EA::Reflection::Text &Gen::gameplay::CarType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xf833c06f);
}
inline bool Gen::gameplay::CarType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xf833c06f, result);
}
inline bool Gen::gameplay::SET_CarType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xf833c06f, input);
}

inline bool Gen::gameplay::CarTypeLowMem(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xfd3cf790);
}
inline const EA::Reflection::Text &Gen::gameplay::CarTypeLowMem() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xfd3cf790);
}
inline bool Gen::gameplay::CarTypeLowMem(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xfd3cf790, result);
}
inline bool Gen::gameplay::SET_CarTypeLowMem(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xfd3cf790, input);
}

inline bool Gen::gameplay::CashReward(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xab0179f4);
}
inline const EA::Reflection::Float &Gen::gameplay::CashReward() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xab0179f4);
}
inline bool Gen::gameplay::CashReward(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xab0179f4, result);
}
inline bool Gen::gameplay::SET_CashReward(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xab0179f4, input);
}

inline bool Gen::gameplay::CashRewards(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x0550fbc2);
}
inline const GCollectionKey &Gen::gameplay::CashRewards(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x0550fbc2, index);
}
inline bool Gen::gameplay::CashRewards(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x0550fbc2, result, index);
}
inline unsigned int Gen::gameplay::Num_CashRewards() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0550fbc2);
}
inline bool Gen::gameplay::SET_CashRewards(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x0550fbc2, input, index);
}

inline bool Gen::gameplay::CashValue(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd8baa07b);
}
inline const EA::Reflection::Float &Gen::gameplay::CashValue() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xd8baa07b);
}
inline bool Gen::gameplay::CashValue(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xd8baa07b, result);
}
inline bool Gen::gameplay::SET_CashValue(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xd8baa07b, input);
}

inline bool Gen::gameplay::CatchUp(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x10db04e6);
}
inline const EA::Reflection::Bool &Gen::gameplay::CatchUp() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x10db04e6);
}
inline bool Gen::gameplay::CatchUp(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x10db04e6, result);
}
inline bool Gen::gameplay::SET_CatchUp(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x10db04e6, input);
}

inline bool Gen::gameplay::CatchUpDerivative(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x515aa4e4);
}
inline const EA::Reflection::Float &Gen::gameplay::CatchUpDerivative() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x515aa4e4);
}
inline bool Gen::gameplay::CatchUpDerivative(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x515aa4e4, result);
}
inline bool Gen::gameplay::SET_CatchUpDerivative(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x515aa4e4, input);
}

inline bool Gen::gameplay::CatchUpIntegral(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4545ab74);
}
inline const EA::Reflection::Float &Gen::gameplay::CatchUpIntegral() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4545ab74);
}
inline bool Gen::gameplay::CatchUpIntegral(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4545ab74, result);
}
inline bool Gen::gameplay::SET_CatchUpIntegral(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4545ab74, input);
}

inline bool Gen::gameplay::CatchUpOverride(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x9eb17c1e);
}
inline const EA::Reflection::Bool &Gen::gameplay::CatchUpOverride() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x9eb17c1e);
}
inline bool Gen::gameplay::CatchUpOverride(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x9eb17c1e, result);
}
inline bool Gen::gameplay::SET_CatchUpOverride(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x9eb17c1e, input);
}

inline bool Gen::gameplay::CatchUpSkill(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x8069b5a9);
}
inline const EA::Reflection::Text &Gen::gameplay::CatchUpSkill() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x8069b5a9);
}
inline bool Gen::gameplay::CatchUpSkill(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x8069b5a9, result);
}
inline bool Gen::gameplay::SET_CatchUpSkill(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x8069b5a9, input);
}

inline bool Gen::gameplay::CatchUpSpread(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xa18a07ba);
}
inline const EA::Reflection::Text &Gen::gameplay::CatchUpSpread() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xa18a07ba);
}
inline bool Gen::gameplay::CatchUpSpread(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xa18a07ba, result);
}
inline bool Gen::gameplay::SET_CatchUpSpread(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xa18a07ba, input);
}

inline bool Gen::gameplay::CellChallengeRace(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xc686cd34);
}
inline const GCollectionKey &Gen::gameplay::CellChallengeRace() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xc686cd34);
}
inline bool Gen::gameplay::CellChallengeRace(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xc686cd34, result);
}
inline bool Gen::gameplay::SET_CellChallengeRace(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xc686cd34, input);
}

inline bool Gen::gameplay::ChallengeSeriesRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1c650104);
}
inline const EA::Reflection::Bool &Gen::gameplay::ChallengeSeriesRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x1c650104);
}
inline bool Gen::gameplay::ChallengeSeriesRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x1c650104, result);
}
inline bool Gen::gameplay::SET_ChallengeSeriesRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x1c650104, input);
}

inline bool Gen::gameplay::ChanceOfRain(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x547486ae);
}
inline const EA::Reflection::Float &Gen::gameplay::ChanceOfRain() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x547486ae);
}
inline bool Gen::gameplay::ChanceOfRain(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x547486ae, result);
}
inline bool Gen::gameplay::SET_ChanceOfRain(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x547486ae, input);
}

inline bool Gen::gameplay::Checkpoint(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x34aae3fc);
}
inline const GCollectionKey &Gen::gameplay::Checkpoint(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x34aae3fc, index);
}
inline bool Gen::gameplay::Checkpoint(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x34aae3fc, result, index);
}
inline unsigned int Gen::gameplay::Num_Checkpoint() const {
    ATTRIB_CODEGEN_GETLENGTH(0x34aae3fc);
}
inline bool Gen::gameplay::SET_Checkpoint(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x34aae3fc, input, index);
}

inline bool Gen::gameplay::CheckpointsVisible(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2ad67092);
}
inline const EA::Reflection::Bool &Gen::gameplay::CheckpointsVisible() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2ad67092);
}
inline bool Gen::gameplay::CheckpointsVisible(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2ad67092, result);
}
inline bool Gen::gameplay::SET_CheckpointsVisible(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2ad67092, input);
}

inline bool Gen::gameplay::Children(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x916e0e78);
}
inline const GCollectionKey &Gen::gameplay::Children(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x916e0e78, index);
}
inline bool Gen::gameplay::Children(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x916e0e78, result, index);
}
inline unsigned int Gen::gameplay::Num_Children() const {
    ATTRIB_CODEGEN_GETLENGTH(0x916e0e78);
}
inline bool Gen::gameplay::SET_Children(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x916e0e78, input, index);
}

inline bool Gen::gameplay::CollectionName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
}
inline const EA::Reflection::Text &Gen::gameplay::CollectionName() const {
    ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
}
inline bool Gen::gameplay::CollectionName(EA::Reflection::Text &result) const {
    result = CollectionName();
    return true;
}
inline bool Gen::gameplay::SET_CollectionName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
}

inline bool Gen::gameplay::CollectorsEditionRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x637584fe);
}
inline const EA::Reflection::Bool &Gen::gameplay::CollectorsEditionRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x637584fe);
}
inline bool Gen::gameplay::CollectorsEditionRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x637584fe, result);
}
inline bool Gen::gameplay::SET_CollectorsEditionRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x637584fe, input);
}

inline bool Gen::gameplay::CopDensity(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xdbc08d32);
}
inline const EA::Reflection::Int32 &Gen::gameplay::CopDensity() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xdbc08d32);
}
inline bool Gen::gameplay::CopDensity(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xdbc08d32, result);
}
inline bool Gen::gameplay::SET_CopDensity(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xdbc08d32, input);
}

inline bool Gen::gameplay::CopSpawnPoints(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xf124b151);
}
inline const GCollectionKey &Gen::gameplay::CopSpawnPoints(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xf124b151, index);
}
inline bool Gen::gameplay::CopSpawnPoints(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xf124b151, result, index);
}
inline unsigned int Gen::gameplay::Num_CopSpawnPoints() const {
    ATTRIB_CODEGEN_GETLENGTH(0xf124b151);
}
inline bool Gen::gameplay::SET_CopSpawnPoints(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xf124b151, input, index);
}

inline bool Gen::gameplay::CopSpawnType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xd686d61e);
}
inline const EA::Reflection::Text &Gen::gameplay::CopSpawnType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xd686d61e);
}
inline bool Gen::gameplay::CopSpawnType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xd686d61e, result);
}
inline bool Gen::gameplay::SET_CopSpawnType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xd686d61e, input);
}

inline bool Gen::gameplay::CopsInRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3918e889);
}
inline const EA::Reflection::Bool &Gen::gameplay::CopsInRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3918e889);
}
inline bool Gen::gameplay::CopsInRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3918e889, result);
}
inline bool Gen::gameplay::SET_CopsInRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3918e889, input);
}

inline bool Gen::gameplay::DDayRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x8cb01abf);
}
inline const EA::Reflection::Bool &Gen::gameplay::DDayRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x8cb01abf);
}
inline bool Gen::gameplay::DDayRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x8cb01abf, result);
}
inline bool Gen::gameplay::SET_DDayRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x8cb01abf, input);
}

inline bool Gen::gameplay::DebugJumpPoint(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa7e9e456);
}
inline const GCollectionKey &Gen::gameplay::DebugJumpPoint() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa7e9e456);
}
inline bool Gen::gameplay::DebugJumpPoint(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa7e9e456, result);
}
inline bool Gen::gameplay::SET_DebugJumpPoint(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa7e9e456, input);
}

inline bool Gen::gameplay::DelayTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x20259346);
}
inline const EA::Reflection::Float &Gen::gameplay::DelayTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x20259346);
}
inline bool Gen::gameplay::DelayTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x20259346, result);
}
inline bool Gen::gameplay::SET_DelayTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x20259346, input);
}

inline bool Gen::gameplay::DifficultyLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x88a7e3be);
}
inline const EA::Reflection::Int32 &Gen::gameplay::DifficultyLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x88a7e3be);
}
inline bool Gen::gameplay::DifficultyLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x88a7e3be, result);
}
inline bool Gen::gameplay::SET_DifficultyLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x88a7e3be, input);
}

inline bool Gen::gameplay::Dimensions(TAttrib<UMath::Vector3> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector3, 0x6d9e21ad);
}
inline const UMath::Vector3 &Gen::gameplay::Dimensions() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector3, 0x6d9e21ad);
}
inline bool Gen::gameplay::Dimensions(UMath::Vector3 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector3, 0x6d9e21ad, result);
}
inline bool Gen::gameplay::SET_Dimensions(const UMath::Vector3 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector3, 0x6d9e21ad, input);
}

inline bool Gen::gameplay::Directional(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6b37e124);
}
inline const EA::Reflection::Bool &Gen::gameplay::Directional() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6b37e124);
}
inline bool Gen::gameplay::Directional(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6b37e124, result);
}
inline bool Gen::gameplay::SET_Directional(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6b37e124, input);
}

inline bool Gen::gameplay::DoCountdown(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3e33da0f);
}
inline const EA::Reflection::Bool &Gen::gameplay::DoCountdown() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3e33da0f);
}
inline bool Gen::gameplay::DoCountdown(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3e33da0f, result);
}
inline bool Gen::gameplay::SET_DoCountdown(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3e33da0f, input);
}

inline bool Gen::gameplay::DoPhotofinish(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xede6017e);
}
inline const EA::Reflection::Bool &Gen::gameplay::DoPhotofinish() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xede6017e);
}
inline bool Gen::gameplay::DoPhotofinish(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xede6017e, result);
}
inline bool Gen::gameplay::SET_DoPhotofinish(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xede6017e, input);
}

inline bool Gen::gameplay::EntryActivity(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xc27dfda8);
}
inline const GCollectionKey &Gen::gameplay::EntryActivity() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xc27dfda8);
}
inline bool Gen::gameplay::EntryActivity(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xc27dfda8, result);
}
inline bool Gen::gameplay::SET_EntryActivity(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xc27dfda8, input);
}

inline bool Gen::gameplay::EntryCellCallID(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe2d26232);
}
inline const EA::Reflection::Int32 &Gen::gameplay::EntryCellCallID() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xe2d26232);
}
inline bool Gen::gameplay::EntryCellCallID(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xe2d26232, result);
}
inline bool Gen::gameplay::SET_EntryCellCallID(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xe2d26232, input);
}

inline bool Gen::gameplay::EventID(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xa78403ec);
}
inline const EA::Reflection::Text &Gen::gameplay::EventID() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xa78403ec);
}
inline bool Gen::gameplay::EventID(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xa78403ec, result);
}
inline bool Gen::gameplay::SET_EventID(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xa78403ec, input);
}

inline bool Gen::gameplay::EventIconType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x0f6bcde1);
}
inline const EA::Reflection::Text &Gen::gameplay::EventIconType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x0f6bcde1);
}
inline bool Gen::gameplay::EventIconType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x0f6bcde1, result);
}
inline bool Gen::gameplay::SET_EventIconType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x0f6bcde1, input);
}

inline bool Gen::gameplay::ExcludedCharacters(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa5c5d25b);
}
inline const GCollectionKey &Gen::gameplay::ExcludedCharacters(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xa5c5d25b, index);
}
inline bool Gen::gameplay::ExcludedCharacters(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xa5c5d25b, result, index);
}
inline unsigned int Gen::gameplay::Num_ExcludedCharacters() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa5c5d25b);
}
inline bool Gen::gameplay::SET_ExcludedCharacters(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xa5c5d25b, input, index);
}

inline bool Gen::gameplay::FilterBlocks(TAttrib<Attrib::Blob> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::Blob, 0x56e1436d);
}
inline const Attrib::Blob &Gen::gameplay::FilterBlocks(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::Blob, 0x56e1436d, index);
}
inline bool Gen::gameplay::FilterBlocks(Attrib::Blob &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::Blob, 0x56e1436d, result, index);
}
inline unsigned int Gen::gameplay::Num_FilterBlocks() const {
    ATTRIB_CODEGEN_GETLENGTH(0x56e1436d);
}
inline bool Gen::gameplay::SET_FilterBlocks(const Attrib::Blob &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::Blob, 0x56e1436d, input, index);
}

inline bool Gen::gameplay::FilterModePassAll(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0d038cfa);
}
inline const EA::Reflection::Bool &Gen::gameplay::FilterModePassAll() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0d038cfa);
}
inline bool Gen::gameplay::FilterModePassAll(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0d038cfa, result);
}
inline bool Gen::gameplay::SET_FilterModePassAll(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0d038cfa, input);
}

inline bool Gen::gameplay::FinishCamera(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x62dfc259);
}
inline const EA::Reflection::Text &Gen::gameplay::FinishCamera() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x62dfc259);
}
inline bool Gen::gameplay::FinishCamera(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x62dfc259, result);
}
inline bool Gen::gameplay::SET_FinishCamera(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x62dfc259, input);
}

inline bool Gen::gameplay::FireOnExit(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb2ac32c7);
}
inline const EA::Reflection::Bool &Gen::gameplay::FireOnExit() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb2ac32c7);
}
inline bool Gen::gameplay::FireOnExit(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb2ac32c7, result);
}
inline bool Gen::gameplay::SET_FireOnExit(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb2ac32c7, input);
}

inline bool Gen::gameplay::FlareSpacing(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x394abbc6);
}
inline const EA::Reflection::Float &Gen::gameplay::FlareSpacing() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x394abbc6);
}
inline bool Gen::gameplay::FlareSpacing(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x394abbc6, result);
}
inline bool Gen::gameplay::SET_FlareSpacing(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x394abbc6, input);
}

inline bool Gen::gameplay::ForceHeatLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe4211f4f);
}
inline const EA::Reflection::Int32 &Gen::gameplay::ForceHeatLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xe4211f4f);
}
inline bool Gen::gameplay::ForceHeatLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xe4211f4f, result);
}
inline bool Gen::gameplay::SET_ForceHeatLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xe4211f4f, input);
}

inline bool Gen::gameplay::ForcePreload(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x9652af0f);
}
inline const EA::Reflection::Bool &Gen::gameplay::ForcePreload() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x9652af0f);
}
inline bool Gen::gameplay::ForcePreload(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x9652af0f, result);
}
inline bool Gen::gameplay::SET_ForcePreload(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x9652af0f, input);
}

inline bool Gen::gameplay::ForceStartPosition(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfb84be75);
}
inline const GCollectionKey &Gen::gameplay::ForceStartPosition() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xfb84be75);
}
inline bool Gen::gameplay::ForceStartPosition(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xfb84be75, result);
}
inline bool Gen::gameplay::SET_ForceStartPosition(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xfb84be75, input);
}

inline bool Gen::gameplay::ForceTrafficDensity(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x5e161bba);
}
inline const EA::Reflection::Int32 &Gen::gameplay::ForceTrafficDensity() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x5e161bba);
}
inline bool Gen::gameplay::ForceTrafficDensity(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x5e161bba, result);
}
inline bool Gen::gameplay::SET_ForceTrafficDensity(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x5e161bba, input);
}

inline bool Gen::gameplay::FreeRoamOnly(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xaa0135e9);
}
inline const EA::Reflection::Bool &Gen::gameplay::FreeRoamOnly() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xaa0135e9);
}
inline bool Gen::gameplay::FreeRoamOnly(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xaa0135e9, result);
}
inline bool Gen::gameplay::SET_FreeRoamOnly(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xaa0135e9, input);
}

inline bool Gen::gameplay::GateActivity(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xc795b8d4);
}
inline const GCollectionKey &Gen::gameplay::GateActivity() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xc795b8d4);
}
inline bool Gen::gameplay::GateActivity(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xc795b8d4, result);
}
inline bool Gen::gameplay::SET_GateActivity(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xc795b8d4, input);
}

inline bool Gen::gameplay::GoalAddPrevBest(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x006ec903);
}
inline const EA::Reflection::Float &Gen::gameplay::GoalAddPrevBest() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x006ec903);
}
inline bool Gen::gameplay::GoalAddPrevBest(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x006ec903, result);
}
inline bool Gen::gameplay::SET_GoalAddPrevBest(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x006ec903, input);
}

inline bool Gen::gameplay::GoalEasy(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8445af47);
}
inline const EA::Reflection::Float &Gen::gameplay::GoalEasy() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8445af47);
}
inline bool Gen::gameplay::GoalEasy(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8445af47, result);
}
inline bool Gen::gameplay::SET_GoalEasy(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8445af47, input);
}

inline bool Gen::gameplay::GoalHard(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3b9bbfc2);
}
inline const EA::Reflection::Float &Gen::gameplay::GoalHard() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3b9bbfc2);
}
inline bool Gen::gameplay::GoalHard(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3b9bbfc2, result);
}
inline bool Gen::gameplay::SET_GoalHard(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3b9bbfc2, input);
}

inline bool Gen::gameplay::IconModelBounceAmp(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd5f4eda2);
}
inline const EA::Reflection::Float &Gen::gameplay::IconModelBounceAmp() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xd5f4eda2);
}
inline bool Gen::gameplay::IconModelBounceAmp(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xd5f4eda2, result);
}
inline bool Gen::gameplay::SET_IconModelBounceAmp(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xd5f4eda2, input);
}

inline bool Gen::gameplay::IconModelBounceRate(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb4985085);
}
inline const EA::Reflection::Float &Gen::gameplay::IconModelBounceRate() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb4985085);
}
inline bool Gen::gameplay::IconModelBounceRate(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb4985085, result);
}
inline bool Gen::gameplay::SET_IconModelBounceRate(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb4985085, input);
}

inline bool Gen::gameplay::IconModelFloatHeight(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4037d3c5);
}
inline const EA::Reflection::Float &Gen::gameplay::IconModelFloatHeight() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4037d3c5);
}
inline bool Gen::gameplay::IconModelFloatHeight(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4037d3c5, result);
}
inline bool Gen::gameplay::SET_IconModelFloatHeight(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4037d3c5, input);
}

inline bool Gen::gameplay::IconModelName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xa62cb4f0);
}
inline const EA::Reflection::Text &Gen::gameplay::IconModelName() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xa62cb4f0);
}
inline bool Gen::gameplay::IconModelName(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xa62cb4f0, result);
}
inline bool Gen::gameplay::SET_IconModelName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xa62cb4f0, input);
}

inline bool Gen::gameplay::IconModelScale(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xcd41cd40);
}
inline const EA::Reflection::Bool &Gen::gameplay::IconModelScale() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xcd41cd40);
}
inline bool Gen::gameplay::IconModelScale(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xcd41cd40, result);
}
inline bool Gen::gameplay::SET_IconModelScale(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xcd41cd40, input);
}

inline bool Gen::gameplay::IconModelSpinRate(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x697332e8);
}
inline const EA::Reflection::Float &Gen::gameplay::IconModelSpinRate() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x697332e8);
}
inline bool Gen::gameplay::IconModelSpinRate(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x697332e8, result);
}
inline bool Gen::gameplay::SET_IconModelSpinRate(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x697332e8, input);
}

inline bool Gen::gameplay::InitialPlayerSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3a0e4b19);
}
inline const EA::Reflection::Float &Gen::gameplay::InitialPlayerSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3a0e4b19);
}
inline bool Gen::gameplay::InitialPlayerSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3a0e4b19, result);
}
inline bool Gen::gameplay::SET_InitialPlayerSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3a0e4b19, input);
}

inline bool Gen::gameplay::InitialSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0a91596d);
}
inline const EA::Reflection::Float &Gen::gameplay::InitialSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0a91596d);
}
inline bool Gen::gameplay::InitialSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0a91596d, result);
}
inline bool Gen::gameplay::SET_InitialSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0a91596d, input);
}

inline bool Gen::gameplay::InitiallyUnlocked(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xea855eaf);
}
inline const EA::Reflection::Bool &Gen::gameplay::InitiallyUnlocked() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xea855eaf);
}
inline bool Gen::gameplay::InitiallyUnlocked(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xea855eaf, result);
}
inline bool Gen::gameplay::SET_InitiallyUnlocked(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xea855eaf, input);
}

inline bool Gen::gameplay::InternalRaceIndex(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0d4c1055);
}
inline const EA::Reflection::Float &Gen::gameplay::InternalRaceIndex(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x0d4c1055, index);
}
inline bool Gen::gameplay::InternalRaceIndex(EA::Reflection::Float &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x0d4c1055, result, index);
}
inline unsigned int Gen::gameplay::Num_InternalRaceIndex() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0d4c1055);
}
inline bool Gen::gameplay::SET_InternalRaceIndex(const EA::Reflection::Float &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x0d4c1055, input, index);
}

inline bool Gen::gameplay::IntroCameraTrack(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x36bbeee9);
}
inline const EA::Reflection::Int32 &Gen::gameplay::IntroCameraTrack() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x36bbeee9);
}
inline bool Gen::gameplay::IntroCameraTrack(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x36bbeee9, result);
}
inline bool Gen::gameplay::SET_IntroCameraTrack(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x36bbeee9, input);
}

inline bool Gen::gameplay::IntroMessageID(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x5468366d);
}
inline const EA::Reflection::Int32 &Gen::gameplay::IntroMessageID() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x5468366d);
}
inline bool Gen::gameplay::IntroMessageID(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x5468366d, result);
}
inline bool Gen::gameplay::SET_IntroMessageID(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x5468366d, input);
}

inline bool Gen::gameplay::IntroMovie(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xf572ede8);
}
inline const EA::Reflection::Text &Gen::gameplay::IntroMovie() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xf572ede8);
}
inline bool Gen::gameplay::IntroMovie(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xf572ede8, result);
}
inline bool Gen::gameplay::SET_IntroMovie(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xf572ede8, input);
}

inline bool Gen::gameplay::IntroNIS(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xdec18d3e);
}
inline const EA::Reflection::Text &Gen::gameplay::IntroNIS() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xdec18d3e);
}
inline bool Gen::gameplay::IntroNIS(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xdec18d3e, result);
}
inline bool Gen::gameplay::SET_IntroNIS(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xdec18d3e, input);
}

inline bool Gen::gameplay::IsBoss(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x79c5d68d);
}
inline const EA::Reflection::Bool &Gen::gameplay::IsBoss() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x79c5d68d);
}
inline bool Gen::gameplay::IsBoss(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x79c5d68d, result);
}
inline bool Gen::gameplay::SET_IsBoss(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x79c5d68d, input);
}

inline bool Gen::gameplay::IsEpicPursuitRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4393f69b);
}
inline const EA::Reflection::Bool &Gen::gameplay::IsEpicPursuitRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4393f69b);
}
inline bool Gen::gameplay::IsEpicPursuitRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4393f69b, result);
}
inline bool Gen::gameplay::SET_IsEpicPursuitRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4393f69b, input);
}

inline bool Gen::gameplay::IsLoopingRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6a9a6f5b);
}
inline const EA::Reflection::Bool &Gen::gameplay::IsLoopingRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6a9a6f5b);
}
inline bool Gen::gameplay::IsLoopingRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6a9a6f5b, result);
}
inline bool Gen::gameplay::SET_IsLoopingRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6a9a6f5b, input);
}

inline bool Gen::gameplay::IsMarkerRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xf2fe50d7);
}
inline const EA::Reflection::Bool &Gen::gameplay::IsMarkerRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xf2fe50d7);
}
inline bool Gen::gameplay::IsMarkerRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xf2fe50d7, result);
}
inline bool Gen::gameplay::SET_IsMarkerRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xf2fe50d7, input);
}

inline bool Gen::gameplay::JumpRaces(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xb671abb6);
}
inline const GCollectionKey &Gen::gameplay::JumpRaces(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xb671abb6, index);
}
inline bool Gen::gameplay::JumpRaces(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xb671abb6, result, index);
}
inline unsigned int Gen::gameplay::Num_JumpRaces() const {
    ATTRIB_CODEGEN_GETLENGTH(0xb671abb6);
}
inline bool Gen::gameplay::SET_JumpRaces(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xb671abb6, input, index);
}

inline bool Gen::gameplay::KnockoutTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x777ece27);
}
inline const EA::Reflection::Float &Gen::gameplay::KnockoutTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x777ece27);
}
inline bool Gen::gameplay::KnockoutTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x777ece27, result);
}
inline bool Gen::gameplay::SET_KnockoutTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x777ece27, input);
}

inline bool Gen::gameplay::KnockoutsPerLap(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x181462da);
}
inline const EA::Reflection::Int32 &Gen::gameplay::KnockoutsPerLap() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x181462da);
}
inline bool Gen::gameplay::KnockoutsPerLap(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x181462da, result);
}
inline bool Gen::gameplay::SET_KnockoutsPerLap(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x181462da, input);
}

inline bool Gen::gameplay::LocalizationTag(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xdb89ab5c);
}
inline const EA::Reflection::Int32 &Gen::gameplay::LocalizationTag() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xdb89ab5c);
}
inline bool Gen::gameplay::LocalizationTag(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xdb89ab5c, result);
}
inline bool Gen::gameplay::SET_LocalizationTag(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xdb89ab5c, input);
}

inline bool Gen::gameplay::MasterCheckpoint(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x609febe8);
}
inline const GCollectionKey &Gen::gameplay::MasterCheckpoint() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x609febe8);
}
inline bool Gen::gameplay::MasterCheckpoint(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x609febe8, result);
}
inline bool Gen::gameplay::SET_MasterCheckpoint(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x609febe8, input);
}

inline bool Gen::gameplay::MaxCarRep(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe10fb7a3);
}
inline const EA::Reflection::Float &Gen::gameplay::MaxCarRep() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe10fb7a3);
}
inline bool Gen::gameplay::MaxCarRep(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe10fb7a3, result);
}
inline bool Gen::gameplay::SET_MaxCarRep(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe10fb7a3, input);
}

inline bool Gen::gameplay::MaxHeatLevel(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf5a03629);
}
inline const EA::Reflection::Float &Gen::gameplay::MaxHeatLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf5a03629);
}
inline bool Gen::gameplay::MaxHeatLevel(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf5a03629, result);
}
inline bool Gen::gameplay::SET_MaxHeatLevel(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf5a03629, input);
}

inline bool Gen::gameplay::MaxOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe8c24416);
}
inline const EA::Reflection::Float &Gen::gameplay::MaxOpenWorldHeat() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe8c24416);
}
inline bool Gen::gameplay::MaxOpenWorldHeat(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe8c24416, result);
}
inline bool Gen::gameplay::SET_MaxOpenWorldHeat(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe8c24416, input);
}

inline bool Gen::gameplay::MaxPursuitRep(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa07ae814);
}
inline const EA::Reflection::Float &Gen::gameplay::MaxPursuitRep() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa07ae814);
}
inline bool Gen::gameplay::MaxPursuitRep(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa07ae814, result);
}
inline bool Gen::gameplay::SET_MaxPursuitRep(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa07ae814, input);
}

inline bool Gen::gameplay::MedalBonusBronze(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x3bb31211);
}
inline const EA::Reflection::Int32 &Gen::gameplay::MedalBonusBronze() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x3bb31211);
}
inline bool Gen::gameplay::MedalBonusBronze(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x3bb31211, result);
}
inline bool Gen::gameplay::SET_MedalBonusBronze(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x3bb31211, input);
}

inline bool Gen::gameplay::MedalBonusGold(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf5e43987);
}
inline const EA::Reflection::Int32 &Gen::gameplay::MedalBonusGold() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xf5e43987);
}
inline bool Gen::gameplay::MedalBonusGold(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xf5e43987, result);
}
inline bool Gen::gameplay::SET_MedalBonusGold(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xf5e43987, input);
}

inline bool Gen::gameplay::MedalBonusSilver(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x767b00a9);
}
inline const EA::Reflection::Int32 &Gen::gameplay::MedalBonusSilver() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x767b00a9);
}
inline bool Gen::gameplay::MedalBonusSilver(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x767b00a9, result);
}
inline bool Gen::gameplay::SET_MedalBonusSilver(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x767b00a9, input);
}

inline bool Gen::gameplay::MilestoneBiggerIsBetter(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0896d043);
}
inline const EA::Reflection::Bool &Gen::gameplay::MilestoneBiggerIsBetter() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0896d043);
}
inline bool Gen::gameplay::MilestoneBiggerIsBetter(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0896d043, result);
}
inline bool Gen::gameplay::SET_MilestoneBiggerIsBetter(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0896d043, input);
}

inline bool Gen::gameplay::MilestoneChallenge(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xbcd98737);
}
inline const GCollectionKey &Gen::gameplay::MilestoneChallenge() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xbcd98737);
}
inline bool Gen::gameplay::MilestoneChallenge(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xbcd98737, result);
}
inline bool Gen::gameplay::SET_MilestoneChallenge(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xbcd98737, input);
}

inline bool Gen::gameplay::MilestoneName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x704f72e8);
}
inline const EA::Reflection::Text &Gen::gameplay::MilestoneName() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x704f72e8);
}
inline bool Gen::gameplay::MilestoneName(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x704f72e8, result);
}
inline bool Gen::gameplay::SET_MilestoneName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x704f72e8, input);
}

inline bool Gen::gameplay::MiniMapItem(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x051e90ca);
}
inline const GCollectionKey &Gen::gameplay::MiniMapItem() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x051e90ca);
}
inline bool Gen::gameplay::MiniMapItem(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x051e90ca, result);
}
inline bool Gen::gameplay::SET_MiniMapItem(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x051e90ca, input);
}

inline bool Gen::gameplay::MinimumAIPerformance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb1ece070);
}
inline const EA::Reflection::Float &Gen::gameplay::MinimumAIPerformance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb1ece070);
}
inline bool Gen::gameplay::MinimumAIPerformance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb1ece070, result);
}
inline bool Gen::gameplay::SET_MinimumAIPerformance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb1ece070, input);
}

inline bool Gen::gameplay::NISShell(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4c17fe41);
}
inline const EA::Reflection::Bool &Gen::gameplay::NISShell() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4c17fe41);
}
inline bool Gen::gameplay::NISShell(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4c17fe41, result);
}
inline bool Gen::gameplay::SET_NISShell(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4c17fe41, input);
}

inline bool Gen::gameplay::Name(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x3e225ec1);
}
inline const EA::Reflection::Text &Gen::gameplay::Name() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x3e225ec1);
}
inline bool Gen::gameplay::Name(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x3e225ec1, result);
}
inline bool Gen::gameplay::SET_Name(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x3e225ec1, input);
}

inline bool Gen::gameplay::NeverInQuickRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xa4e6fcfd);
}
inline const EA::Reflection::Bool &Gen::gameplay::NeverInQuickRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xa4e6fcfd);
}
inline bool Gen::gameplay::NeverInQuickRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xa4e6fcfd, result);
}
inline bool Gen::gameplay::SET_NeverInQuickRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xa4e6fcfd, input);
}

inline bool Gen::gameplay::NoPostRaceScreen(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x40f9929f);
}
inline const EA::Reflection::Bool &Gen::gameplay::NoPostRaceScreen() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x40f9929f);
}
inline bool Gen::gameplay::NoPostRaceScreen(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x40f9929f, result);
}
inline bool Gen::gameplay::SET_NoPostRaceScreen(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x40f9929f, input);
}

inline bool Gen::gameplay::NumLaps(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0ebdc165);
}
inline const EA::Reflection::Int32 &Gen::gameplay::NumLaps() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x0ebdc165);
}
inline bool Gen::gameplay::NumLaps(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x0ebdc165, result);
}
inline bool Gen::gameplay::SET_NumLaps(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x0ebdc165, input);
}

inline bool Gen::gameplay::NumRacesRequired(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x90a22a3f);
}
inline const EA::Reflection::Int32 &Gen::gameplay::NumRacesRequired() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x90a22a3f);
}
inline bool Gen::gameplay::NumRacesRequired(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x90a22a3f, result);
}
inline bool Gen::gameplay::SET_NumRacesRequired(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x90a22a3f, input);
}

inline bool Gen::gameplay::OneShot(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xce4261ac);
}
inline const EA::Reflection::Bool &Gen::gameplay::OneShot() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xce4261ac);
}
inline bool Gen::gameplay::OneShot(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xce4261ac, result);
}
inline bool Gen::gameplay::SET_OneShot(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xce4261ac, input);
}

inline bool Gen::gameplay::OpenWorldSpeedTrap(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1bb16f14);
}
inline const EA::Reflection::Bool &Gen::gameplay::OpenWorldSpeedTrap() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x1bb16f14);
}
inline bool Gen::gameplay::OpenWorldSpeedTrap(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x1bb16f14, result);
}
inline bool Gen::gameplay::SET_OpenWorldSpeedTrap(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x1bb16f14, input);
}

inline bool Gen::gameplay::Opponents(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x5839fa1a);
}
inline const GCollectionKey &Gen::gameplay::Opponents(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x5839fa1a, index);
}
inline bool Gen::gameplay::Opponents(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x5839fa1a, result, index);
}
inline unsigned int Gen::gameplay::Num_Opponents() const {
    ATTRIB_CODEGEN_GETLENGTH(0x5839fa1a);
}
inline bool Gen::gameplay::SET_Opponents(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x5839fa1a, input, index);
}

inline bool Gen::gameplay::OutroCameraTrack(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7054ff5b);
}
inline const EA::Reflection::Int32 &Gen::gameplay::OutroCameraTrack() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x7054ff5b);
}
inline bool Gen::gameplay::OutroCameraTrack(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x7054ff5b, result);
}
inline bool Gen::gameplay::SET_OutroCameraTrack(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x7054ff5b, input);
}

inline bool Gen::gameplay::OutroMessageID(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc36e3532);
}
inline const EA::Reflection::Int32 &Gen::gameplay::OutroMessageID() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xc36e3532);
}
inline bool Gen::gameplay::OutroMessageID(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xc36e3532, result);
}
inline bool Gen::gameplay::SET_OutroMessageID(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xc36e3532, input);
}

inline bool Gen::gameplay::OutroMovie(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xb70268c0);
}
inline const EA::Reflection::Text &Gen::gameplay::OutroMovie() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xb70268c0);
}
inline bool Gen::gameplay::OutroMovie(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xb70268c0, result);
}
inline bool Gen::gameplay::SET_OutroMovie(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xb70268c0, input);
}

inline bool Gen::gameplay::OutroNIS(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x54932966);
}
inline const EA::Reflection::Text &Gen::gameplay::OutroNIS() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x54932966);
}
inline bool Gen::gameplay::OutroNIS(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x54932966, result);
}
inline bool Gen::gameplay::SET_OutroNIS(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x54932966, input);
}

inline bool Gen::gameplay::OutroNISMarker(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x0e265c88);
}
inline const GCollectionKey &Gen::gameplay::OutroNISMarker() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x0e265c88);
}
inline bool Gen::gameplay::OutroNISMarker(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x0e265c88, result);
}
inline bool Gen::gameplay::SET_OutroNISMarker(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x0e265c88, input);
}

inline bool Gen::gameplay::OvertimePenaltyPerSec(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x26fd42b0);
}
inline const EA::Reflection::Int32 &Gen::gameplay::OvertimePenaltyPerSec() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x26fd42b0);
}
inline bool Gen::gameplay::OvertimePenaltyPerSec(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x26fd42b0, result);
}
inline bool Gen::gameplay::SET_OvertimePenaltyPerSec(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x26fd42b0, input);
}

inline bool Gen::gameplay::ParticleEffect(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x5ef34802);
}
inline const EA::Reflection::Text &Gen::gameplay::ParticleEffect() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x5ef34802);
}
inline bool Gen::gameplay::ParticleEffect(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x5ef34802, result);
}
inline bool Gen::gameplay::SET_ParticleEffect(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x5ef34802, input);
}

inline bool Gen::gameplay::Persistent(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xe4542e9b);
}
inline const EA::Reflection::Bool &Gen::gameplay::Persistent() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xe4542e9b);
}
inline bool Gen::gameplay::Persistent(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xe4542e9b, result);
}
inline bool Gen::gameplay::SET_Persistent(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xe4542e9b, input);
}

inline bool Gen::gameplay::PlayerCarPerformance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfb42c0b9);
}
inline const EA::Reflection::Float &Gen::gameplay::PlayerCarPerformance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xfb42c0b9);
}
inline bool Gen::gameplay::PlayerCarPerformance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xfb42c0b9, result);
}
inline bool Gen::gameplay::SET_PlayerCarPerformance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xfb42c0b9, input);
}

inline bool Gen::gameplay::PlayerCarType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xc0eeb909);
}
inline const EA::Reflection::Text &Gen::gameplay::PlayerCarType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xc0eeb909);
}
inline bool Gen::gameplay::PlayerCarType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xc0eeb909, result);
}
inline bool Gen::gameplay::SET_PlayerCarType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xc0eeb909, input);
}

inline bool Gen::gameplay::Position(TAttrib<UMath::Vector3> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector3, 0x9f743a0e);
}
inline const UMath::Vector3 &Gen::gameplay::Position() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector3, 0x9f743a0e);
}
inline bool Gen::gameplay::Position(UMath::Vector3 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector3, 0x9f743a0e, result);
}
inline bool Gen::gameplay::SET_Position(const UMath::Vector3 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector3, 0x9f743a0e, input);
}

inline bool Gen::gameplay::PostRaceActivity(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x64273c71);
}
inline const GCollectionKey &Gen::gameplay::PostRaceActivity() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x64273c71);
}
inline bool Gen::gameplay::PostRaceActivity(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x64273c71, result);
}
inline bool Gen::gameplay::SET_PostRaceActivity(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x64273c71, input);
}

inline bool Gen::gameplay::PostRaceScreenTexture(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x038a3b53);
}
inline const EA::Reflection::Text &Gen::gameplay::PostRaceScreenTexture() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x038a3b53);
}
inline bool Gen::gameplay::PostRaceScreenTexture(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x038a3b53, result);
}
inline bool Gen::gameplay::SET_PostRaceScreenTexture(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x038a3b53, input);
}

inline bool Gen::gameplay::PresetRide(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x416a8409);
}
inline const EA::Reflection::Text &Gen::gameplay::PresetRide() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x416a8409);
}
inline bool Gen::gameplay::PresetRide(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x416a8409, result);
}
inline bool Gen::gameplay::SET_PresetRide(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x416a8409, input);
}

inline bool Gen::gameplay::ProgressionLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x740e9b4a);
}
inline const EA::Reflection::Int32 &Gen::gameplay::ProgressionLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x740e9b4a);
}
inline bool Gen::gameplay::ProgressionLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x740e9b4a, result);
}
inline bool Gen::gameplay::SET_ProgressionLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x740e9b4a, input);
}

inline bool Gen::gameplay::PursuitLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0261ae99);
}
inline const EA::Reflection::Int32 &Gen::gameplay::PursuitLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x0261ae99);
}
inline bool Gen::gameplay::PursuitLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x0261ae99, result);
}
inline bool Gen::gameplay::SET_PursuitLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x0261ae99, input);
}

inline bool Gen::gameplay::PursuitRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2b1f54f6);
}
inline const EA::Reflection::Bool &Gen::gameplay::PursuitRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2b1f54f6);
}
inline bool Gen::gameplay::PursuitRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2b1f54f6, result);
}
inline bool Gen::gameplay::SET_PursuitRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2b1f54f6, input);
}

inline bool Gen::gameplay::QuickRaceNIS(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x5987fb25);
}
inline const EA::Reflection::Text &Gen::gameplay::QuickRaceNIS() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x5987fb25);
}
inline bool Gen::gameplay::QuickRaceNIS(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x5987fb25, result);
}
inline bool Gen::gameplay::SET_QuickRaceNIS(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x5987fb25, input);
}

inline bool Gen::gameplay::QuickRaceUnlocked(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xc4db4e71);
}
inline const EA::Reflection::Bool &Gen::gameplay::QuickRaceUnlocked() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xc4db4e71);
}
inline bool Gen::gameplay::QuickRaceUnlocked(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xc4db4e71, result);
}
inline bool Gen::gameplay::SET_QuickRaceUnlocked(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xc4db4e71, input);
}

inline bool Gen::gameplay::RaceLength(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7c11c52e);
}
inline const EA::Reflection::Float &Gen::gameplay::RaceLength() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7c11c52e);
}
inline bool Gen::gameplay::RaceLength(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7c11c52e, result);
}
inline bool Gen::gameplay::SET_RaceLength(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7c11c52e, input);
}

inline bool Gen::gameplay::RaceList(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x9f914008);
}
inline const GCollectionKey &Gen::gameplay::RaceList(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x9f914008, index);
}
inline bool Gen::gameplay::RaceList(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x9f914008, result, index);
}
inline unsigned int Gen::gameplay::Num_RaceList() const {
    ATTRIB_CODEGEN_GETLENGTH(0x9f914008);
}
inline bool Gen::gameplay::SET_RaceList(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x9f914008, input, index);
}

inline bool Gen::gameplay::RaceTriggers(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x58dc14c0);
}
inline const GCollectionKey &Gen::gameplay::RaceTriggers(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x58dc14c0, index);
}
inline bool Gen::gameplay::RaceTriggers(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x58dc14c0, result, index);
}
inline unsigned int Gen::gameplay::Num_RaceTriggers() const {
    ATTRIB_CODEGEN_GETLENGTH(0x58dc14c0);
}
inline bool Gen::gameplay::SET_RaceTriggers(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x58dc14c0, input, index);
}

inline bool Gen::gameplay::RacerName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xbeab64c5);
}
inline const EA::Reflection::Text &Gen::gameplay::RacerName() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xbeab64c5);
}
inline bool Gen::gameplay::RacerName(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xbeab64c5, result);
}
inline bool Gen::gameplay::SET_RacerName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xbeab64c5, input);
}

inline bool Gen::gameplay::Radius(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x39bf8002);
}
inline const EA::Reflection::Float &Gen::gameplay::Radius() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x39bf8002);
}
inline bool Gen::gameplay::Radius(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x39bf8002, result);
}
inline bool Gen::gameplay::SET_Radius(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x39bf8002, input);
}

inline bool Gen::gameplay::RandomSpawnTriggers(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfdfe1c3e);
}
inline const GCollectionKey &Gen::gameplay::RandomSpawnTriggers(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xfdfe1c3e, index);
}
inline bool Gen::gameplay::RandomSpawnTriggers(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xfdfe1c3e, result, index);
}
inline unsigned int Gen::gameplay::Num_RandomSpawnTriggers() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfdfe1c3e);
}
inline bool Gen::gameplay::SET_RandomSpawnTriggers(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xfdfe1c3e, input, index);
}

inline bool Gen::gameplay::RankPlayersByDistance(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x9e7a18ce);
}
inline const EA::Reflection::Bool &Gen::gameplay::RankPlayersByDistance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x9e7a18ce);
}
inline bool Gen::gameplay::RankPlayersByDistance(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x9e7a18ce, result);
}
inline bool Gen::gameplay::SET_RankPlayersByDistance(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x9e7a18ce, input);
}

inline bool Gen::gameplay::RankPlayersByPoints(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x5ec1880f);
}
inline const EA::Reflection::Bool &Gen::gameplay::RankPlayersByPoints() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x5ec1880f);
}
inline bool Gen::gameplay::RankPlayersByPoints(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x5ec1880f, result);
}
inline bool Gen::gameplay::SET_RankPlayersByPoints(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x5ec1880f, input);
}

inline bool Gen::gameplay::Region(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xcb01e454);
}
inline const EA::Reflection::Text &Gen::gameplay::Region() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xcb01e454);
}
inline bool Gen::gameplay::Region(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xcb01e454, result);
}
inline bool Gen::gameplay::SET_Region(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xcb01e454, input);
}

inline bool Gen::gameplay::Reputation(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x477ec5aa);
}
inline const EA::Reflection::Int32 &Gen::gameplay::Reputation() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x477ec5aa);
}
inline bool Gen::gameplay::Reputation(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x477ec5aa, result);
}
inline bool Gen::gameplay::SET_Reputation(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x477ec5aa, input);
}

inline bool Gen::gameplay::ReputationRequired(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xffd69c94);
}
inline const EA::Reflection::Int32 &Gen::gameplay::ReputationRequired() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xffd69c94);
}
inline bool Gen::gameplay::ReputationRequired(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xffd69c94, result);
}
inline bool Gen::gameplay::SET_ReputationRequired(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xffd69c94, input);
}

inline bool Gen::gameplay::RequiredBounty(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd3657d92);
}
inline const EA::Reflection::Int32 &Gen::gameplay::RequiredBounty() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd3657d92);
}
inline bool Gen::gameplay::RequiredBounty(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd3657d92, result);
}
inline bool Gen::gameplay::SET_RequiredBounty(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd3657d92, input);
}

inline bool Gen::gameplay::RequiredChallenges(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x6dd4b98b);
}
inline const EA::Reflection::Int32 &Gen::gameplay::RequiredChallenges() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x6dd4b98b);
}
inline bool Gen::gameplay::RequiredChallenges(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x6dd4b98b, result);
}
inline bool Gen::gameplay::SET_RequiredChallenges(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x6dd4b98b, input);
}

inline bool Gen::gameplay::RequiredRacesWon(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd617fedc);
}
inline const EA::Reflection::Int32 &Gen::gameplay::RequiredRacesWon() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd617fedc);
}
inline bool Gen::gameplay::RequiredRacesWon(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd617fedc, result);
}
inline bool Gen::gameplay::SET_RequiredRacesWon(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd617fedc, input);
}

inline bool Gen::gameplay::ResetTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2c44ff10);
}
inline const EA::Reflection::Float &Gen::gameplay::ResetTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x2c44ff10);
}
inline bool Gen::gameplay::ResetTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x2c44ff10, result);
}
inline bool Gen::gameplay::SET_ResetTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x2c44ff10, input);
}

inline bool Gen::gameplay::ResetsPlayer(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6ccd5819);
}
inline const EA::Reflection::Bool &Gen::gameplay::ResetsPlayer() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6ccd5819);
}
inline bool Gen::gameplay::ResetsPlayer(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6ccd5819, result);
}
inline bool Gen::gameplay::SET_ResetsPlayer(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6ccd5819, input);
}

inline bool Gen::gameplay::RespawnMarker(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x2241f4cd);
}
inline const GCollectionKey &Gen::gameplay::RespawnMarker() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x2241f4cd);
}
inline bool Gen::gameplay::RespawnMarker(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x2241f4cd, result);
}
inline bool Gen::gameplay::SET_RespawnMarker(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x2241f4cd, input);
}

inline bool Gen::gameplay::RestartActivity(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xdc44bd08);
}
inline const GCollectionKey &Gen::gameplay::RestartActivity() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xdc44bd08);
}
inline bool Gen::gameplay::RestartActivity(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xdc44bd08, result);
}
inline bool Gen::gameplay::SET_RestartActivity(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xdc44bd08, input);
}

inline bool Gen::gameplay::RewardMarkerType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x06a077d5);
}
inline const EA::Reflection::Text &Gen::gameplay::RewardMarkerType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x06a077d5);
}
inline bool Gen::gameplay::RewardMarkerType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x06a077d5, result);
}
inline bool Gen::gameplay::SET_RewardMarkerType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x06a077d5, input);
}

inline bool Gen::gameplay::RewardsForWinner(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x50104d90);
}
inline const GCollectionKey &Gen::gameplay::RewardsForWinner(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x50104d90, index);
}
inline bool Gen::gameplay::RewardsForWinner(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x50104d90, result, index);
}
inline unsigned int Gen::gameplay::Num_RewardsForWinner() const {
    ATTRIB_CODEGEN_GETLENGTH(0x50104d90);
}
inline bool Gen::gameplay::SET_RewardsForWinner(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x50104d90, input, index);
}

inline bool Gen::gameplay::RingTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc516e9c2);
}
inline const EA::Reflection::Float &Gen::gameplay::RingTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc516e9c2);
}
inline bool Gen::gameplay::RingTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc516e9c2, result);
}
inline bool Gen::gameplay::SET_RingTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc516e9c2, input);
}

inline bool Gen::gameplay::RivalBestTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf9120d73);
}
inline const EA::Reflection::Float &Gen::gameplay::RivalBestTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf9120d73);
}
inline bool Gen::gameplay::RivalBestTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf9120d73, result);
}
inline bool Gen::gameplay::SET_RivalBestTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf9120d73, input);
}

inline bool Gen::gameplay::RoadList(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x13b11b40);
}
inline const EA::Reflection::Text &Gen::gameplay::RoadList(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Text, 0x13b11b40, index);
}
inline bool Gen::gameplay::RoadList(EA::Reflection::Text &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Text, 0x13b11b40, result, index);
}
inline unsigned int Gen::gameplay::Num_RoadList() const {
    ATTRIB_CODEGEN_GETLENGTH(0x13b11b40);
}
inline bool Gen::gameplay::SET_RoadList(const EA::Reflection::Text &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Text, 0x13b11b40, input, index);
}

inline bool Gen::gameplay::RollingStart(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb809d19c);
}
inline const EA::Reflection::Bool &Gen::gameplay::RollingStart() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb809d19c);
}
inline bool Gen::gameplay::RollingStart(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb809d19c, result);
}
inline bool Gen::gameplay::SET_RollingStart(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb809d19c, input);
}

inline bool Gen::gameplay::Rotation(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5a6a57c6);
}
inline const EA::Reflection::Float &Gen::gameplay::Rotation() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5a6a57c6);
}
inline bool Gen::gameplay::Rotation(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5a6a57c6, result);
}
inline bool Gen::gameplay::SET_Rotation(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5a6a57c6, input);
}

inline bool Gen::gameplay::SMSCellChallenge(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x6a4cd2d4);
}
inline const EA::Reflection::Int32 &Gen::gameplay::SMSCellChallenge() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x6a4cd2d4);
}
inline bool Gen::gameplay::SMSCellChallenge(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x6a4cd2d4, result);
}
inline bool Gen::gameplay::SET_SMSCellChallenge(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x6a4cd2d4, input);
}

inline bool Gen::gameplay::SMSRivalChallenge(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xbb30c804);
}
inline const EA::Reflection::Int32 &Gen::gameplay::SMSRivalChallenge() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xbb30c804);
}
inline bool Gen::gameplay::SMSRivalChallenge(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xbb30c804, result);
}
inline bool Gen::gameplay::SET_SMSRivalChallenge(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xbb30c804, input);
}

inline bool Gen::gameplay::ScaleOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1823b89e);
}
inline const EA::Reflection::Float &Gen::gameplay::ScaleOpenWorldHeat() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x1823b89e);
}
inline bool Gen::gameplay::ScaleOpenWorldHeat(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x1823b89e, result);
}
inline bool Gen::gameplay::SET_ScaleOpenWorldHeat(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x1823b89e, input);
}

inline bool Gen::gameplay::ScriptedCopsInRace(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0e47fe63);
}
inline const EA::Reflection::Bool &Gen::gameplay::ScriptedCopsInRace() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0e47fe63);
}
inline bool Gen::gameplay::ScriptedCopsInRace(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0e47fe63, result);
}
inline bool Gen::gameplay::SET_ScriptedCopsInRace(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0e47fe63, input);
}

inline bool Gen::gameplay::SharedCheckpoints(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0e34a1f3);
}
inline const EA::Reflection::Bool &Gen::gameplay::SharedCheckpoints() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0e34a1f3);
}
inline bool Gen::gameplay::SharedCheckpoints(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0e34a1f3, result);
}
inline bool Gen::gameplay::SET_SharedCheckpoints(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0e34a1f3, input);
}

inline bool Gen::gameplay::ShortcutMaxChance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x16faba11);
}
inline const EA::Reflection::Float &Gen::gameplay::ShortcutMaxChance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x16faba11);
}
inline bool Gen::gameplay::ShortcutMaxChance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x16faba11, result);
}
inline bool Gen::gameplay::SET_ShortcutMaxChance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x16faba11, input);
}

inline bool Gen::gameplay::ShortcutMinChance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4efb950a);
}
inline const EA::Reflection::Float &Gen::gameplay::ShortcutMinChance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4efb950a);
}
inline bool Gen::gameplay::ShortcutMinChance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4efb950a, result);
}
inline bool Gen::gameplay::SET_ShortcutMinChance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4efb950a, input);
}

inline bool Gen::gameplay::Shortcuts(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x7b6d296e);
}
inline const GCollectionKey &Gen::gameplay::Shortcuts(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x7b6d296e, index);
}
inline bool Gen::gameplay::Shortcuts(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x7b6d296e, result, index);
}
inline unsigned int Gen::gameplay::Num_Shortcuts() const {
    ATTRIB_CODEGEN_GETLENGTH(0x7b6d296e);
}
inline bool Gen::gameplay::SET_Shortcuts(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x7b6d296e, input, index);
}

inline bool Gen::gameplay::SkillLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x3b798aa2);
}
inline const EA::Reflection::Int32 &Gen::gameplay::SkillLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x3b798aa2);
}
inline bool Gen::gameplay::SkillLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x3b798aa2, result);
}
inline bool Gen::gameplay::SET_SkillLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x3b798aa2, input);
}

inline bool Gen::gameplay::SpawnPoint(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xddf411f5);
}
inline const GCollectionKey &Gen::gameplay::SpawnPoint() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xddf411f5);
}
inline bool Gen::gameplay::SpawnPoint(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xddf411f5, result);
}
inline bool Gen::gameplay::SET_SpawnPoint(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xddf411f5, input);
}

inline bool Gen::gameplay::SpeedTrapCamera(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xcbd7adf9);
}
inline const EA::Reflection::Text &Gen::gameplay::SpeedTrapCamera() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xcbd7adf9);
}
inline bool Gen::gameplay::SpeedTrapCamera(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xcbd7adf9, result);
}
inline bool Gen::gameplay::SET_SpeedTrapCamera(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xcbd7adf9, input);
}

inline bool Gen::gameplay::SpeedTrapList(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x822179d1);
}
inline const GCollectionKey &Gen::gameplay::SpeedTrapList(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x822179d1, index);
}
inline bool Gen::gameplay::SpeedTrapList(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x822179d1, result, index);
}
inline unsigned int Gen::gameplay::Num_SpeedTrapList() const {
    ATTRIB_CODEGEN_GETLENGTH(0x822179d1);
}
inline bool Gen::gameplay::SET_SpeedTrapList(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x822179d1, input, index);
}

inline bool Gen::gameplay::SpeedTrapTrigger(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x5f95c3a0);
}
inline const GCollectionKey &Gen::gameplay::SpeedTrapTrigger() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x5f95c3a0);
}
inline bool Gen::gameplay::SpeedTrapTrigger(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x5f95c3a0, result);
}
inline bool Gen::gameplay::SET_SpeedTrapTrigger(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x5f95c3a0, input);
}

inline bool Gen::gameplay::SpeedTrapsRequired(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x6d7e73c9);
}
inline const GCollectionKey &Gen::gameplay::SpeedTrapsRequired(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x6d7e73c9, index);
}
inline bool Gen::gameplay::SpeedTrapsRequired(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x6d7e73c9, result, index);
}
inline unsigned int Gen::gameplay::Num_SpeedTrapsRequired() const {
    ATTRIB_CODEGEN_GETLENGTH(0x6d7e73c9);
}
inline bool Gen::gameplay::SET_SpeedTrapsRequired(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x6d7e73c9, input, index);
}

inline bool Gen::gameplay::StartPercent(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe0d01505);
}
inline const EA::Reflection::Float &Gen::gameplay::StartPercent() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe0d01505);
}
inline bool Gen::gameplay::StartPercent(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe0d01505, result);
}
inline bool Gen::gameplay::SET_StartPercent(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe0d01505, input);
}

inline bool Gen::gameplay::StartTime(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x839602ab);
}
inline const EA::Reflection::Float &Gen::gameplay::StartTime() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x839602ab);
}
inline bool Gen::gameplay::StartTime(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x839602ab, result);
}
inline bool Gen::gameplay::SET_StartTime(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x839602ab, input);
}

inline bool Gen::gameplay::TOD(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9dff3c3d);
}
inline const EA::Reflection::Float &Gen::gameplay::TOD() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9dff3c3d);
}
inline bool Gen::gameplay::TOD(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9dff3c3d, result);
}
inline bool Gen::gameplay::SET_TOD(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9dff3c3d, input);
}

inline bool Gen::gameplay::TargetActivities(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x0f37d221);
}
inline const GCollectionKey &Gen::gameplay::TargetActivities(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x0f37d221, index);
}
inline bool Gen::gameplay::TargetActivities(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x0f37d221, result, index);
}
inline unsigned int Gen::gameplay::Num_TargetActivities() const {
    ATTRIB_CODEGEN_GETLENGTH(0x0f37d221);
}
inline bool Gen::gameplay::SET_TargetActivities(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x0f37d221, input, index);
}

inline bool Gen::gameplay::TargetActivity(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x277566f3);
}
inline const GCollectionKey &Gen::gameplay::TargetActivity() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x277566f3);
}
inline bool Gen::gameplay::TargetActivity(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x277566f3, result);
}
inline bool Gen::gameplay::SET_TargetActivity(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x277566f3, input);
}

inline bool Gen::gameplay::TargetBronze(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x00df8eb4);
}
inline const EA::Reflection::Float &Gen::gameplay::TargetBronze() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x00df8eb4);
}
inline bool Gen::gameplay::TargetBronze(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x00df8eb4, result);
}
inline bool Gen::gameplay::SET_TargetBronze(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x00df8eb4, input);
}

inline bool Gen::gameplay::TargetGold(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x728e43ff);
}
inline const EA::Reflection::Float &Gen::gameplay::TargetGold() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x728e43ff);
}
inline bool Gen::gameplay::TargetGold(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x728e43ff, result);
}
inline bool Gen::gameplay::SET_TargetGold(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x728e43ff, input);
}

inline bool Gen::gameplay::TargetMarker(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x8fc356fb);
}
inline const GCollectionKey &Gen::gameplay::TargetMarker() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x8fc356fb);
}
inline bool Gen::gameplay::TargetMarker(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x8fc356fb, result);
}
inline bool Gen::gameplay::SET_TargetMarker(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x8fc356fb, input);
}

inline bool Gen::gameplay::TargetSilver(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x51ce16b7);
}
inline const EA::Reflection::Float &Gen::gameplay::TargetSilver() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x51ce16b7);
}
inline bool Gen::gameplay::TargetSilver(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x51ce16b7, result);
}
inline bool Gen::gameplay::SET_TargetSilver(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x51ce16b7, input);
}

inline bool Gen::gameplay::Template(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3e9156ca);
}
inline const EA::Reflection::Bool &Gen::gameplay::Template() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3e9156ca);
}
inline bool Gen::gameplay::Template(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3e9156ca, result);
}
inline bool Gen::gameplay::SET_Template(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3e9156ca, input);
}

inline bool Gen::gameplay::ThreshholdSpeed(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc3710777);
}
inline const EA::Reflection::Float &Gen::gameplay::ThreshholdSpeed() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc3710777);
}
inline bool Gen::gameplay::ThreshholdSpeed(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc3710777, result);
}
inline bool Gen::gameplay::SET_ThreshholdSpeed(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc3710777, input);
}

inline bool Gen::gameplay::ThreshholdValue(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4e90219d);
}
inline const EA::Reflection::Float &Gen::gameplay::ThreshholdValue() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4e90219d);
}
inline bool Gen::gameplay::ThreshholdValue(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4e90219d, result);
}
inline bool Gen::gameplay::SET_ThreshholdValue(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4e90219d, input);
}

inline bool Gen::gameplay::TimeBonus(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf52cc30e);
}
inline const EA::Reflection::Int32 &Gen::gameplay::TimeBonus() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xf52cc30e);
}
inline bool Gen::gameplay::TimeBonus(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xf52cc30e, result);
}
inline bool Gen::gameplay::SET_TimeBonus(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xf52cc30e, input);
}

inline bool Gen::gameplay::TimeLimit(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7585f041);
}
inline const EA::Reflection::Float &Gen::gameplay::TimeLimit() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7585f041);
}
inline bool Gen::gameplay::TimeLimit(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7585f041, result);
}
inline bool Gen::gameplay::SET_TimeLimit(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7585f041, input);
}

inline bool Gen::gameplay::TokenValue(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xaa10914c);
}
inline const EA::Reflection::Int32 &Gen::gameplay::TokenValue() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xaa10914c);
}
inline bool Gen::gameplay::TokenValue(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xaa10914c, result);
}
inline bool Gen::gameplay::SET_TokenValue(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xaa10914c, input);
}

inline bool Gen::gameplay::TrafficCharacter(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x797d9654);
}
inline const GCollectionKey &Gen::gameplay::TrafficCharacter() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x797d9654);
}
inline bool Gen::gameplay::TrafficCharacter(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x797d9654, result);
}
inline bool Gen::gameplay::SET_TrafficCharacter(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x797d9654, input);
}

inline bool Gen::gameplay::TrafficLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc64bc341);
}
inline const EA::Reflection::Int32 &Gen::gameplay::TrafficLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xc64bc341);
}
inline bool Gen::gameplay::TrafficLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xc64bc341, result);
}
inline bool Gen::gameplay::SET_TrafficLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xc64bc341, input);
}

inline bool Gen::gameplay::TrafficPattern(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x6319b692);
}
inline const EA::Reflection::Text &Gen::gameplay::TrafficPattern() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x6319b692);
}
inline bool Gen::gameplay::TrafficPattern(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x6319b692, result);
}
inline bool Gen::gameplay::SET_TrafficPattern(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x6319b692, input);
}

inline bool Gen::gameplay::UnlockRaces(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfc8995c8);
}
inline const GCollectionKey &Gen::gameplay::UnlockRaces(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xfc8995c8, index);
}
inline bool Gen::gameplay::UnlockRaces(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xfc8995c8, result, index);
}
inline unsigned int Gen::gameplay::Num_UnlockRaces() const {
    ATTRIB_CODEGEN_GETLENGTH(0xfc8995c8);
}
inline bool Gen::gameplay::SET_UnlockRaces(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xfc8995c8, input, index);
}

inline bool Gen::gameplay::UpgradeLevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd267facc);
}
inline const EA::Reflection::Int32 &Gen::gameplay::UpgradeLevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd267facc);
}
inline bool Gen::gameplay::UpgradeLevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd267facc, result);
}
inline bool Gen::gameplay::SET_UpgradeLevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd267facc, input);
}

inline bool Gen::gameplay::UpgradePartID(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x3c2fdaab);
}
inline const EA::Reflection::Text &Gen::gameplay::UpgradePartID() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x3c2fdaab);
}
inline bool Gen::gameplay::UpgradePartID(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x3c2fdaab, result);
}
inline bool Gen::gameplay::SET_UpgradePartID(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x3c2fdaab, input);
}

inline bool Gen::gameplay::UpgradePartName(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xc385f75d);
}
inline const EA::Reflection::Text &Gen::gameplay::UpgradePartName() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xc385f75d);
}
inline bool Gen::gameplay::UpgradePartName(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xc385f75d, result);
}
inline bool Gen::gameplay::SET_UpgradePartName(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xc385f75d, input);
}

inline bool Gen::gameplay::UpgradeType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x0e0113fe);
}
inline const EA::Reflection::Text &Gen::gameplay::UpgradeType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x0e0113fe);
}
inline bool Gen::gameplay::UpgradeType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x0e0113fe, result);
}
inline bool Gen::gameplay::SET_UpgradeType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x0e0113fe, input);
}

inline bool Gen::gameplay::UseWorldHeat(TAttrib<EA::Reflection::Bool> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x45f2ad6c);
}
inline const EA::Reflection::Bool &Gen::gameplay::UseWorldHeat() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x45f2ad6c);
}
inline bool Gen::gameplay::UseWorldHeat(EA::Reflection::Bool &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x45f2ad6c, result);
}
inline bool Gen::gameplay::SET_UseWorldHeat(const EA::Reflection::Bool &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x45f2ad6c, input);
}

inline bool Gen::gameplay::Width(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5816c1fc);
}
inline const EA::Reflection::Float &Gen::gameplay::Width() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5816c1fc);
}
inline bool Gen::gameplay::Width(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5816c1fc, result);
}
inline bool Gen::gameplay::SET_Width(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5816c1fc, input);
}

inline bool Gen::gameplay::WorldRaces(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa7ef40ef);
}
inline const GCollectionKey &Gen::gameplay::WorldRaces(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xa7ef40ef, index);
}
inline bool Gen::gameplay::WorldRaces(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xa7ef40ef, result, index);
}
inline unsigned int Gen::gameplay::Num_WorldRaces() const {
    ATTRIB_CODEGEN_GETLENGTH(0xa7ef40ef);
}
inline bool Gen::gameplay::SET_WorldRaces(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xa7ef40ef, input, index);
}

inline bool Gen::gameplay::ZoneList(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x64893da8);
}
inline const GCollectionKey &Gen::gameplay::ZoneList(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x64893da8, index);
}
inline bool Gen::gameplay::ZoneList(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x64893da8, result, index);
}
inline unsigned int Gen::gameplay::Num_ZoneList() const {
    ATTRIB_CODEGEN_GETLENGTH(0x64893da8);
}
inline bool Gen::gameplay::SET_ZoneList(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x64893da8, input, index);
}

inline bool Gen::gameplay::ZoneType(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xf3ea3201);
}
inline const EA::Reflection::Text &Gen::gameplay::ZoneType() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xf3ea3201);
}
inline bool Gen::gameplay::ZoneType(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xf3ea3201, result);
}
inline bool Gen::gameplay::SET_ZoneType(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xf3ea3201, input);
}

inline bool Gen::gameplay::actionscript(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xe62083d0);
}
inline const GCollectionKey &Gen::gameplay::actionscript() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xe62083d0);
}
inline bool Gen::gameplay::actionscript(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xe62083d0, result);
}
inline bool Gen::gameplay::SET_actionscript(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xe62083d0, input);
}

inline bool Gen::gameplay::bytecode(TAttrib<Attrib::Blob> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(Attrib::Blob, 0x9a4a020a);
}
inline const Attrib::Blob &Gen::gameplay::bytecode() const {
    ATTRIB_CODEGEN_GETVALUE(Attrib::Blob, 0x9a4a020a);
}
inline bool Gen::gameplay::bytecode(Attrib::Blob &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::Blob, 0x9a4a020a, result);
}
inline bool Gen::gameplay::SET_bytecode(const Attrib::Blob &input) {
    ATTRIB_CODEGEN_SETVALUE(Attrib::Blob, 0x9a4a020a, input);
}

inline bool Gen::gameplay::disengagetrigger(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x106285c0);
}
inline const GCollectionKey &Gen::gameplay::disengagetrigger() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x106285c0);
}
inline bool Gen::gameplay::disengagetrigger(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x106285c0, result);
}
inline bool Gen::gameplay::SET_disengagetrigger(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x106285c0, input);
}

inline bool Gen::gameplay::distance(TAttrib<EA::Reflection::Float> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc5857615);
}
inline const EA::Reflection::Float &Gen::gameplay::distance() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc5857615);
}
inline bool Gen::gameplay::distance(EA::Reflection::Float &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc5857615, result);
}
inline bool Gen::gameplay::SET_distance(const EA::Reflection::Float &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc5857615, input);
}

inline bool Gen::gameplay::engagetrigger(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xf05931ab);
}
inline const GCollectionKey &Gen::gameplay::engagetrigger() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xf05931ab);
}
inline bool Gen::gameplay::engagetrigger(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xf05931ab, result);
}
inline bool Gen::gameplay::SET_engagetrigger(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xf05931ab, input);
}

inline bool Gen::gameplay::gameplayvault(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x93fd9fda);
}
inline const EA::Reflection::Text &Gen::gameplay::gameplayvault() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x93fd9fda);
}
inline bool Gen::gameplay::gameplayvault(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x93fd9fda, result);
}
inline bool Gen::gameplay::SET_gameplayvault(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x93fd9fda, input);
}

inline bool Gen::gameplay::handler_owner(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x857fe432);
}
inline const GCollectionKey &Gen::gameplay::handler_owner() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x857fe432);
}
inline bool Gen::gameplay::handler_owner(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x857fe432, result);
}
inline bool Gen::gameplay::SET_handler_owner(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x857fe432, input);
}

inline bool Gen::gameplay::layoutpos(TAttrib<UMath::Vector2> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x4075ec46);
}
inline const UMath::Vector2 &Gen::gameplay::layoutpos() const {
    ATTRIB_CODEGEN_GETVALUE(UMath::Vector2, 0x4075ec46);
}
inline bool Gen::gameplay::layoutpos(UMath::Vector2 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector2, 0x4075ec46, result);
}
inline bool Gen::gameplay::SET_layoutpos(const UMath::Vector2 &input) {
    ATTRIB_CODEGEN_SETVALUE(UMath::Vector2, 0x4075ec46, input);
}

inline bool Gen::gameplay::message_id(TAttrib<EA::Reflection::UInt32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x9e8910ef);
}
inline const EA::Reflection::UInt32 &Gen::gameplay::message_id() const {
    ATTRIB_CODEGEN_GETLAYOUT(message_id);
}
inline bool Gen::gameplay::message_id(EA::Reflection::UInt32 &result) const {
    result = message_id();
    return true;
}
inline bool Gen::gameplay::SET_message_id(const EA::Reflection::UInt32 &input) {
    ATTRIB_CODEGEN_SETLAYOUT(message_id, input);
}

inline bool Gen::gameplay::nitrouslevel(TAttrib<EA::Reflection::Int32> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd4797aa8);
}
inline const EA::Reflection::Int32 &Gen::gameplay::nitrouslevel() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd4797aa8);
}
inline bool Gen::gameplay::nitrouslevel(EA::Reflection::Int32 &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd4797aa8, result);
}
inline bool Gen::gameplay::SET_nitrouslevel(const EA::Reflection::Int32 &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd4797aa8, input);
}

inline bool Gen::gameplay::parentstate(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x4acc6d63);
}
inline const GCollectionKey &Gen::gameplay::parentstate() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x4acc6d63);
}
inline bool Gen::gameplay::parentstate(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x4acc6d63, result);
}
inline bool Gen::gameplay::SET_parentstate(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x4acc6d63, input);
}

inline bool Gen::gameplay::racefinish(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xb0a24adc);
}
inline const GCollectionKey &Gen::gameplay::racefinish() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xb0a24adc);
}
inline bool Gen::gameplay::racefinish(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xb0a24adc, result);
}
inline bool Gen::gameplay::SET_racefinish(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xb0a24adc, input);
}

inline bool Gen::gameplay::racefinishReverse(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x7c7cf20f);
}
inline const GCollectionKey &Gen::gameplay::racefinishReverse() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x7c7cf20f);
}
inline bool Gen::gameplay::racefinishReverse(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x7c7cf20f, result);
}
inline bool Gen::gameplay::SET_racefinishReverse(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x7c7cf20f, input);
}

inline bool Gen::gameplay::racestart(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xe43b2ccc);
}
inline const GCollectionKey &Gen::gameplay::racestart() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xe43b2ccc);
}
inline bool Gen::gameplay::racestart(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xe43b2ccc, result);
}
inline bool Gen::gameplay::SET_racestart(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xe43b2ccc, input);
}

inline bool Gen::gameplay::racestartReverse(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfd945479);
}
inline const GCollectionKey &Gen::gameplay::racestartReverse() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xfd945479);
}
inline bool Gen::gameplay::racestartReverse(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xfd945479, result);
}
inline bool Gen::gameplay::SET_racestartReverse(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xfd945479, input);
}

inline bool Gen::gameplay::scriptname(TAttrib<EA::Reflection::Text> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x7148ae82);
}
inline const EA::Reflection::Text &Gen::gameplay::scriptname() const {
    ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x7148ae82);
}
inline bool Gen::gameplay::scriptname(EA::Reflection::Text &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x7148ae82, result);
}
inline bool Gen::gameplay::SET_scriptname(const EA::Reflection::Text &input) {
    ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x7148ae82, input);
}

inline bool Gen::gameplay::sender(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa590a98b);
}
inline const GCollectionKey &Gen::gameplay::sender() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa590a98b);
}
inline bool Gen::gameplay::sender(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa590a98b, result);
}
inline bool Gen::gameplay::SET_sender(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa590a98b, input);
}

inline bool Gen::gameplay::stateref(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x918c796e);
}
inline const GCollectionKey &Gen::gameplay::stateref() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x918c796e);
}
inline bool Gen::gameplay::stateref(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x918c796e, result);
}
inline bool Gen::gameplay::SET_stateref(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x918c796e, input);
}

inline bool Gen::gameplay::target(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x1a7d2859);
}
inline const GCollectionKey &Gen::gameplay::target() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x1a7d2859);
}
inline bool Gen::gameplay::target(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x1a7d2859, result);
}
inline bool Gen::gameplay::SET_target(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x1a7d2859, input);
}

inline bool Gen::gameplay::templateref(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa0697302);
}
inline const GCollectionKey &Gen::gameplay::templateref() const {
    ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa0697302);
}
inline bool Gen::gameplay::templateref(GCollectionKey &result) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa0697302, result);
}
inline bool Gen::gameplay::SET_templateref(const GCollectionKey &input) {
    ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa0697302, input);
}

inline bool Gen::gameplay::transitionlist(TAttrib<GCollectionKey> &result) const {
    ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x25621dc5);
}
inline const GCollectionKey &Gen::gameplay::transitionlist(unsigned int index) const {
    ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x25621dc5, index);
}
inline bool Gen::gameplay::transitionlist(GCollectionKey &result, unsigned int index) const {
    ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x25621dc5, result, index);
}
inline unsigned int Gen::gameplay::Num_transitionlist() const {
    ATTRIB_CODEGEN_GETLENGTH(0x25621dc5);
}
inline bool Gen::gameplay::SET_transitionlist(const GCollectionKey &input, unsigned int index) {
    ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x25621dc5, input, index);
}

}; // namespace Attrib

#endif
