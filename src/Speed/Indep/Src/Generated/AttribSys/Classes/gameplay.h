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
    bool AllowInvisibleSpawn(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xa1009a23);
    }
    bool AllowInvisibleSpawn(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xa1009a23, result);
    }
    const EA::Reflection::Bool &AllowInvisibleSpawn() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xa1009a23);
    }
    bool SET_AllowInvisibleSpawn(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xa1009a23, input);
    }
    bool AutoActivateGPS(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x29b9c312);
    }
    bool AutoActivateGPS(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x29b9c312, result);
    }
    const EA::Reflection::Bool &AutoActivateGPS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x29b9c312);
    }
    bool SET_AutoActivateGPS(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x29b9c312, input);
    }
    bool AutoSpawnTriggerType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xd5c7e9c3);
    }
    bool AutoSpawnTriggerType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xd5c7e9c3, result);
    }
    const EA::Reflection::Text &AutoSpawnTriggerType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xd5c7e9c3);
    }
    bool SET_AutoSpawnTriggerType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xd5c7e9c3, input);
    }
    bool AutoStart(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x883c65e3);
    }
    bool AutoStart(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x883c65e3, result);
    }
    const EA::Reflection::Bool &AutoStart() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x883c65e3);
    }
    bool SET_AutoStart(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x883c65e3, input);
    }
    bool AvailableOnline(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x39509746);
    }
    bool AvailableOnline(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x39509746, result);
    }
    const EA::Reflection::Bool &AvailableOnline() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x39509746);
    }
    bool SET_AvailableOnline(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x39509746, input);
    }
    bool AvailableQR(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb39ed8c3);
    }
    bool AvailableQR(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb39ed8c3, result);
    }
    const EA::Reflection::Bool &AvailableQR() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb39ed8c3);
    }
    bool SET_AvailableQR(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb39ed8c3, input);
    }
    bool BarrierExemptions(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xf380286b);
    }
    bool BarrierExemptions(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xf380286b, result, index);
    }
    const GCollectionKey &BarrierExemptions(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xf380286b, index);
    }
    unsigned int Num_BarrierExemptions() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf380286b);
    }
    bool SET_BarrierExemptions(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xf380286b, input, index);
    }
    bool Barriers(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xe244f26b);
    }
    bool Barriers(EA::Reflection::Text &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Text, 0xe244f26b, result, index);
    }
    const EA::Reflection::Text &Barriers(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Text, 0xe244f26b, index);
    }
    unsigned int Num_Barriers() const {
        ATTRIB_CODEGEN_GETLENGTH(0xe244f26b);
    }
    bool SET_Barriers(const EA::Reflection::Text &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Text, 0xe244f26b, input, index);
    }
    bool BaseOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8f186ac4);
    }
    bool BaseOpenWorldHeat(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8f186ac4, result);
    }
    const EA::Reflection::Float &BaseOpenWorldHeat() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8f186ac4);
    }
    bool SET_BaseOpenWorldHeat(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8f186ac4, input);
    }
    bool BaselineUnlocks(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xbaf89280);
    }
    bool BaselineUnlocks(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xbaf89280, result, index);
    }
    const GCollectionKey &BaselineUnlocks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xbaf89280, index);
    }
    unsigned int Num_BaselineUnlocks() const {
        ATTRIB_CODEGEN_GETLENGTH(0xbaf89280);
    }
    bool SET_BaselineUnlocks(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xbaf89280, input, index);
    }
    bool BinIndex(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x6ce23062);
    }
    bool BinIndex(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x6ce23062, result);
    }
    const EA::Reflection::Int32 &BinIndex() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x6ce23062);
    }
    bool SET_BinIndex(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x6ce23062, input);
    }
    bool Boss(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa5f39dc7);
    }
    bool Boss(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa5f39dc7, result);
    }
    const GCollectionKey &Boss() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa5f39dc7);
    }
    bool SET_Boss(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa5f39dc7, input);
    }
    bool BossRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xff5ee5d6);
    }
    bool BossRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xff5ee5d6, result);
    }
    const EA::Reflection::Bool &BossRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xff5ee5d6);
    }
    bool SET_BossRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xff5ee5d6, input);
    }
    bool BossRaces(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xd5a174aa);
    }
    bool BossRaces(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xd5a174aa, result, index);
    }
    const GCollectionKey &BossRaces(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xd5a174aa, index);
    }
    unsigned int Num_BossRaces() const {
        ATTRIB_CODEGEN_GETLENGTH(0xd5a174aa);
    }
    bool SET_BossRaces(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xd5a174aa, input, index);
    }
    bool BossReputation(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x1d33241a);
    }
    bool BossReputation(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x1d33241a, result);
    }
    const EA::Reflection::Int32 &BossReputation() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x1d33241a);
    }
    bool SET_BossReputation(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x1d33241a, input);
    }
    bool Bounty(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x8e1904c7);
    }
    bool Bounty(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x8e1904c7, result);
    }
    const EA::Reflection::Int32 &Bounty() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x8e1904c7);
    }
    bool SET_Bounty(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x8e1904c7, input);
    }
    bool BustedLives(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x462f2e36);
    }
    bool BustedLives(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x462f2e36, result);
    }
    const EA::Reflection::Int32 &BustedLives() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x462f2e36);
    }
    bool SET_BustedLives(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x462f2e36, input);
    }
    bool CameraModelMarker(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x52dc742c);
    }
    bool CameraModelMarker(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x52dc742c, result);
    }
    const GCollectionKey &CameraModelMarker() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x52dc742c);
    }
    bool SET_CameraModelMarker(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x52dc742c, input);
    }
    bool CannedPath(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x9c19e56f);
    }
    bool CannedPath(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x9c19e56f, result, index);
    }
    const GCollectionKey &CannedPath(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x9c19e56f, index);
    }
    unsigned int Num_CannedPath() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9c19e56f);
    }
    bool SET_CannedPath(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x9c19e56f, input, index);
    }
    bool CarType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xf833c06f);
    }
    bool CarType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xf833c06f, result);
    }
    const EA::Reflection::Text &CarType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xf833c06f);
    }
    bool SET_CarType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xf833c06f, input);
    }
    bool CarTypeLowMem(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xfd3cf790);
    }
    bool CarTypeLowMem(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xfd3cf790, result);
    }
    const EA::Reflection::Text &CarTypeLowMem() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xfd3cf790);
    }
    bool SET_CarTypeLowMem(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xfd3cf790, input);
    }
    bool CashReward(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xab0179f4);
    }
    bool CashReward(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xab0179f4, result);
    }
    const EA::Reflection::Float &CashReward() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xab0179f4);
    }
    bool SET_CashReward(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xab0179f4, input);
    }
    bool CashRewards(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x0550fbc2);
    }
    bool CashRewards(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x0550fbc2, result, index);
    }
    const GCollectionKey &CashRewards(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x0550fbc2, index);
    }
    unsigned int Num_CashRewards() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0550fbc2);
    }
    bool SET_CashRewards(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x0550fbc2, input, index);
    }
    bool CashValue(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd8baa07b);
    }
    bool CashValue(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xd8baa07b, result);
    }
    const EA::Reflection::Float &CashValue() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xd8baa07b);
    }
    bool SET_CashValue(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xd8baa07b, input);
    }
    bool CatchUp(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x10db04e6);
    }
    bool CatchUp(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x10db04e6, result);
    }
    const EA::Reflection::Bool &CatchUp() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x10db04e6);
    }
    bool SET_CatchUp(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x10db04e6, input);
    }
    bool CatchUpDerivative(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x515aa4e4);
    }
    bool CatchUpDerivative(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x515aa4e4, result);
    }
    const EA::Reflection::Float &CatchUpDerivative() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x515aa4e4);
    }
    bool SET_CatchUpDerivative(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x515aa4e4, input);
    }
    bool CatchUpIntegral(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4545ab74);
    }
    bool CatchUpIntegral(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4545ab74, result);
    }
    const EA::Reflection::Float &CatchUpIntegral() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4545ab74);
    }
    bool SET_CatchUpIntegral(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4545ab74, input);
    }
    bool CatchUpOverride(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x9eb17c1e);
    }
    bool CatchUpOverride(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x9eb17c1e, result);
    }
    const EA::Reflection::Bool &CatchUpOverride() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x9eb17c1e);
    }
    bool SET_CatchUpOverride(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x9eb17c1e, input);
    }
    bool CatchUpSkill(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x8069b5a9);
    }
    bool CatchUpSkill(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x8069b5a9, result);
    }
    const EA::Reflection::Text &CatchUpSkill() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x8069b5a9);
    }
    bool SET_CatchUpSkill(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x8069b5a9, input);
    }
    bool CatchUpSpread(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xa18a07ba);
    }
    bool CatchUpSpread(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xa18a07ba, result);
    }
    const EA::Reflection::Text &CatchUpSpread() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xa18a07ba);
    }
    bool SET_CatchUpSpread(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xa18a07ba, input);
    }
    bool CellChallengeRace(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xc686cd34);
    }
    bool CellChallengeRace(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xc686cd34, result);
    }
    const GCollectionKey &CellChallengeRace() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xc686cd34);
    }
    bool SET_CellChallengeRace(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xc686cd34, input);
    }
    bool ChallengeSeriesRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1c650104);
    }
    bool ChallengeSeriesRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x1c650104, result);
    }
    const EA::Reflection::Bool &ChallengeSeriesRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x1c650104);
    }
    bool SET_ChallengeSeriesRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x1c650104, input);
    }
    bool ChanceOfRain(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x547486ae);
    }
    bool ChanceOfRain(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x547486ae, result);
    }
    const EA::Reflection::Float &ChanceOfRain() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x547486ae);
    }
    bool SET_ChanceOfRain(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x547486ae, input);
    }
    bool Checkpoint(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x34aae3fc);
    }
    bool Checkpoint(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x34aae3fc, result, index);
    }
    const GCollectionKey &Checkpoint(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x34aae3fc, index);
    }
    unsigned int Num_Checkpoint() const {
        ATTRIB_CODEGEN_GETLENGTH(0x34aae3fc);
    }
    bool SET_Checkpoint(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x34aae3fc, input, index);
    }
    bool CheckpointsVisible(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2ad67092);
    }
    bool CheckpointsVisible(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2ad67092, result);
    }
    const EA::Reflection::Bool &CheckpointsVisible() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2ad67092);
    }
    bool SET_CheckpointsVisible(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2ad67092, input);
    }
    bool Children(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x916e0e78);
    }
    bool Children(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x916e0e78, result, index);
    }
    const GCollectionKey &Children(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x916e0e78, index);
    }
    unsigned int Num_Children() const {
        ATTRIB_CODEGEN_GETLENGTH(0x916e0e78);
    }
    bool SET_Children(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x916e0e78, input, index);
    }
    bool CollectionName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x9ca1c8f9);
    }
    bool CollectionName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(CollectionName, result);
    }
    const EA::Reflection::Text &CollectionName() const {
        ATTRIB_CODEGEN_GETLAYOUT(CollectionName);
    }
    bool SET_CollectionName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETLAYOUT(CollectionName, input);
    }
    bool CollectorsEditionRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x637584fe);
    }
    bool CollectorsEditionRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x637584fe, result);
    }
    const EA::Reflection::Bool &CollectorsEditionRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x637584fe);
    }
    bool SET_CollectorsEditionRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x637584fe, input);
    }
    bool CopDensity(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xdbc08d32);
    }
    bool CopDensity(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xdbc08d32, result);
    }
    const EA::Reflection::Int32 &CopDensity() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xdbc08d32);
    }
    bool SET_CopDensity(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xdbc08d32, input);
    }
    bool CopSpawnPoints(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xf124b151);
    }
    bool CopSpawnPoints(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xf124b151, result, index);
    }
    const GCollectionKey &CopSpawnPoints(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xf124b151, index);
    }
    unsigned int Num_CopSpawnPoints() const {
        ATTRIB_CODEGEN_GETLENGTH(0xf124b151);
    }
    bool SET_CopSpawnPoints(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xf124b151, input, index);
    }
    bool CopSpawnType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xd686d61e);
    }
    bool CopSpawnType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xd686d61e, result);
    }
    const EA::Reflection::Text &CopSpawnType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xd686d61e);
    }
    bool SET_CopSpawnType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xd686d61e, input);
    }
    bool CopsInRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3918e889);
    }
    bool CopsInRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3918e889, result);
    }
    const EA::Reflection::Bool &CopsInRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3918e889);
    }
    bool SET_CopsInRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3918e889, input);
    }
    bool DDayRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x8cb01abf);
    }
    bool DDayRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x8cb01abf, result);
    }
    const EA::Reflection::Bool &DDayRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x8cb01abf);
    }
    bool SET_DDayRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x8cb01abf, input);
    }
    bool DebugJumpPoint(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa7e9e456);
    }
    bool DebugJumpPoint(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa7e9e456, result);
    }
    const GCollectionKey &DebugJumpPoint() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa7e9e456);
    }
    bool SET_DebugJumpPoint(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa7e9e456, input);
    }
    bool DelayTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x20259346);
    }
    bool DelayTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x20259346, result);
    }
    const EA::Reflection::Float &DelayTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x20259346);
    }
    bool SET_DelayTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x20259346, input);
    }
    bool DifficultyLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x88a7e3be);
    }
    bool DifficultyLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x88a7e3be, result);
    }
    const EA::Reflection::Int32 &DifficultyLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x88a7e3be);
    }
    bool SET_DifficultyLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x88a7e3be, input);
    }
    bool Dimensions(TAttrib<UMath::Vector3> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector3, 0x6d9e21ad);
    }
    bool Dimensions(UMath::Vector3 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector3, 0x6d9e21ad, result);
    }
    const UMath::Vector3 &Dimensions() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector3, 0x6d9e21ad);
    }
    bool SET_Dimensions(const UMath::Vector3 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector3, 0x6d9e21ad, input);
    }
    bool Directional(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6b37e124);
    }
    bool Directional(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6b37e124, result);
    }
    const EA::Reflection::Bool &Directional() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6b37e124);
    }
    bool SET_Directional(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6b37e124, input);
    }
    bool DoCountdown(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3e33da0f);
    }
    bool DoCountdown(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3e33da0f, result);
    }
    const EA::Reflection::Bool &DoCountdown() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3e33da0f);
    }
    bool SET_DoCountdown(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3e33da0f, input);
    }
    bool DoPhotofinish(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xede6017e);
    }
    bool DoPhotofinish(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xede6017e, result);
    }
    const EA::Reflection::Bool &DoPhotofinish() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xede6017e);
    }
    bool SET_DoPhotofinish(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xede6017e, input);
    }
    bool EntryActivity(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xc27dfda8);
    }
    bool EntryActivity(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xc27dfda8, result);
    }
    const GCollectionKey &EntryActivity() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xc27dfda8);
    }
    bool SET_EntryActivity(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xc27dfda8, input);
    }
    bool EntryCellCallID(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe2d26232);
    }
    bool EntryCellCallID(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xe2d26232, result);
    }
    const EA::Reflection::Int32 &EntryCellCallID() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xe2d26232);
    }
    bool SET_EntryCellCallID(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xe2d26232, input);
    }
    bool EventID(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xa78403ec);
    }
    bool EventID(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xa78403ec, result);
    }
    const EA::Reflection::Text &EventID() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xa78403ec);
    }
    bool SET_EventID(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xa78403ec, input);
    }
    bool EventIconType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x0f6bcde1);
    }
    bool EventIconType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x0f6bcde1, result);
    }
    const EA::Reflection::Text &EventIconType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x0f6bcde1);
    }
    bool SET_EventIconType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x0f6bcde1, input);
    }
    bool ExcludedCharacters(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa5c5d25b);
    }
    bool ExcludedCharacters(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xa5c5d25b, result, index);
    }
    const GCollectionKey &ExcludedCharacters(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xa5c5d25b, index);
    }
    unsigned int Num_ExcludedCharacters() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa5c5d25b);
    }
    bool SET_ExcludedCharacters(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xa5c5d25b, input, index);
    }
    bool FilterBlocks(TAttrib<Attrib::Blob> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::Blob, 0x56e1436d);
    }
    bool FilterBlocks(Attrib::Blob &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(Attrib::Blob, 0x56e1436d, result, index);
    }
    const Attrib::Blob &FilterBlocks(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(Attrib::Blob, 0x56e1436d, index);
    }
    unsigned int Num_FilterBlocks() const {
        ATTRIB_CODEGEN_GETLENGTH(0x56e1436d);
    }
    bool SET_FilterBlocks(const Attrib::Blob &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(Attrib::Blob, 0x56e1436d, input, index);
    }
    bool FilterModePassAll(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0d038cfa);
    }
    bool FilterModePassAll(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0d038cfa, result);
    }
    const EA::Reflection::Bool &FilterModePassAll() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0d038cfa);
    }
    bool SET_FilterModePassAll(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0d038cfa, input);
    }
    bool FinishCamera(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x62dfc259);
    }
    bool FinishCamera(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x62dfc259, result);
    }
    const EA::Reflection::Text &FinishCamera() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x62dfc259);
    }
    bool SET_FinishCamera(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x62dfc259, input);
    }
    bool FireOnExit(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb2ac32c7);
    }
    bool FireOnExit(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb2ac32c7, result);
    }
    const EA::Reflection::Bool &FireOnExit() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb2ac32c7);
    }
    bool SET_FireOnExit(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb2ac32c7, input);
    }
    bool FlareSpacing(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x394abbc6);
    }
    bool FlareSpacing(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x394abbc6, result);
    }
    const EA::Reflection::Float &FlareSpacing() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x394abbc6);
    }
    bool SET_FlareSpacing(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x394abbc6, input);
    }
    bool ForceHeatLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xe4211f4f);
    }
    bool ForceHeatLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xe4211f4f, result);
    }
    const EA::Reflection::Int32 &ForceHeatLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xe4211f4f);
    }
    bool SET_ForceHeatLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xe4211f4f, input);
    }
    bool ForcePreload(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x9652af0f);
    }
    bool ForcePreload(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x9652af0f, result);
    }
    const EA::Reflection::Bool &ForcePreload() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x9652af0f);
    }
    bool SET_ForcePreload(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x9652af0f, input);
    }
    bool ForceStartPosition(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfb84be75);
    }
    bool ForceStartPosition(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xfb84be75, result);
    }
    const GCollectionKey &ForceStartPosition() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xfb84be75);
    }
    bool SET_ForceStartPosition(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xfb84be75, input);
    }
    bool ForceTrafficDensity(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x5e161bba);
    }
    bool ForceTrafficDensity(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x5e161bba, result);
    }
    const EA::Reflection::Int32 &ForceTrafficDensity() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x5e161bba);
    }
    bool SET_ForceTrafficDensity(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x5e161bba, input);
    }
    bool FreeRoamOnly(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xaa0135e9);
    }
    bool FreeRoamOnly(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xaa0135e9, result);
    }
    const EA::Reflection::Bool &FreeRoamOnly() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xaa0135e9);
    }
    bool SET_FreeRoamOnly(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xaa0135e9, input);
    }
    bool GateActivity(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xc795b8d4);
    }
    bool GateActivity(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xc795b8d4, result);
    }
    const GCollectionKey &GateActivity() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xc795b8d4);
    }
    bool SET_GateActivity(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xc795b8d4, input);
    }
    bool GoalAddPrevBest(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x006ec903);
    }
    bool GoalAddPrevBest(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x006ec903, result);
    }
    const EA::Reflection::Float &GoalAddPrevBest() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x006ec903);
    }
    bool SET_GoalAddPrevBest(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x006ec903, input);
    }
    bool GoalEasy(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x8445af47);
    }
    bool GoalEasy(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x8445af47, result);
    }
    const EA::Reflection::Float &GoalEasy() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x8445af47);
    }
    bool SET_GoalEasy(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x8445af47, input);
    }
    bool GoalHard(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3b9bbfc2);
    }
    bool GoalHard(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3b9bbfc2, result);
    }
    const EA::Reflection::Float &GoalHard() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3b9bbfc2);
    }
    bool SET_GoalHard(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3b9bbfc2, input);
    }
    bool IconModelBounceAmp(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xd5f4eda2);
    }
    bool IconModelBounceAmp(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xd5f4eda2, result);
    }
    const EA::Reflection::Float &IconModelBounceAmp() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xd5f4eda2);
    }
    bool SET_IconModelBounceAmp(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xd5f4eda2, input);
    }
    bool IconModelBounceRate(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb4985085);
    }
    bool IconModelBounceRate(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb4985085, result);
    }
    const EA::Reflection::Float &IconModelBounceRate() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb4985085);
    }
    bool SET_IconModelBounceRate(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb4985085, input);
    }
    bool IconModelFloatHeight(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4037d3c5);
    }
    bool IconModelFloatHeight(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4037d3c5, result);
    }
    const EA::Reflection::Float &IconModelFloatHeight() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4037d3c5);
    }
    bool SET_IconModelFloatHeight(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4037d3c5, input);
    }
    bool IconModelName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xa62cb4f0);
    }
    bool IconModelName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xa62cb4f0, result);
    }
    const EA::Reflection::Text &IconModelName() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xa62cb4f0);
    }
    bool SET_IconModelName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xa62cb4f0, input);
    }
    bool IconModelScale(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xcd41cd40);
    }
    bool IconModelScale(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xcd41cd40, result);
    }
    const EA::Reflection::Bool &IconModelScale() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xcd41cd40);
    }
    bool SET_IconModelScale(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xcd41cd40, input);
    }
    bool IconModelSpinRate(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x697332e8);
    }
    bool IconModelSpinRate(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x697332e8, result);
    }
    const EA::Reflection::Float &IconModelSpinRate() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x697332e8);
    }
    bool SET_IconModelSpinRate(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x697332e8, input);
    }
    bool InitialPlayerSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x3a0e4b19);
    }
    bool InitialPlayerSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x3a0e4b19, result);
    }
    const EA::Reflection::Float &InitialPlayerSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x3a0e4b19);
    }
    bool SET_InitialPlayerSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x3a0e4b19, input);
    }
    bool InitialSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0a91596d);
    }
    bool InitialSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x0a91596d, result);
    }
    const EA::Reflection::Float &InitialSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x0a91596d);
    }
    bool SET_InitialSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x0a91596d, input);
    }
    bool InitiallyUnlocked(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xea855eaf);
    }
    bool InitiallyUnlocked(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xea855eaf, result);
    }
    const EA::Reflection::Bool &InitiallyUnlocked() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xea855eaf);
    }
    bool SET_InitiallyUnlocked(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xea855eaf, input);
    }
    bool InternalRaceIndex(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x0d4c1055);
    }
    bool InternalRaceIndex(EA::Reflection::Float &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Float, 0x0d4c1055, result, index);
    }
    const EA::Reflection::Float &InternalRaceIndex(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Float, 0x0d4c1055, index);
    }
    unsigned int Num_InternalRaceIndex() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0d4c1055);
    }
    bool SET_InternalRaceIndex(const EA::Reflection::Float &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Float, 0x0d4c1055, input, index);
    }
    bool IntroCameraTrack(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x36bbeee9);
    }
    bool IntroCameraTrack(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x36bbeee9, result);
    }
    const EA::Reflection::Int32 &IntroCameraTrack() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x36bbeee9);
    }
    bool SET_IntroCameraTrack(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x36bbeee9, input);
    }
    bool IntroMessageID(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x5468366d);
    }
    bool IntroMessageID(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x5468366d, result);
    }
    const EA::Reflection::Int32 &IntroMessageID() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x5468366d);
    }
    bool SET_IntroMessageID(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x5468366d, input);
    }
    bool IntroMovie(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xf572ede8);
    }
    bool IntroMovie(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xf572ede8, result);
    }
    const EA::Reflection::Text &IntroMovie() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xf572ede8);
    }
    bool SET_IntroMovie(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xf572ede8, input);
    }
    bool IntroNIS(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xdec18d3e);
    }
    bool IntroNIS(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xdec18d3e, result);
    }
    const EA::Reflection::Text &IntroNIS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xdec18d3e);
    }
    bool SET_IntroNIS(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xdec18d3e, input);
    }
    bool IsBoss(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x79c5d68d);
    }
    bool IsBoss(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x79c5d68d, result);
    }
    const EA::Reflection::Bool &IsBoss() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x79c5d68d);
    }
    bool SET_IsBoss(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x79c5d68d, input);
    }
    bool IsEpicPursuitRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4393f69b);
    }
    bool IsEpicPursuitRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4393f69b, result);
    }
    const EA::Reflection::Bool &IsEpicPursuitRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4393f69b);
    }
    bool SET_IsEpicPursuitRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4393f69b, input);
    }
    bool IsLoopingRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6a9a6f5b);
    }
    bool IsLoopingRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6a9a6f5b, result);
    }
    const EA::Reflection::Bool &IsLoopingRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6a9a6f5b);
    }
    bool SET_IsLoopingRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6a9a6f5b, input);
    }
    bool IsMarkerRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xf2fe50d7);
    }
    bool IsMarkerRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xf2fe50d7, result);
    }
    const EA::Reflection::Bool &IsMarkerRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xf2fe50d7);
    }
    bool SET_IsMarkerRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xf2fe50d7, input);
    }
    bool JumpRaces(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xb671abb6);
    }
    bool JumpRaces(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xb671abb6, result, index);
    }
    const GCollectionKey &JumpRaces(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xb671abb6, index);
    }
    unsigned int Num_JumpRaces() const {
        ATTRIB_CODEGEN_GETLENGTH(0xb671abb6);
    }
    bool SET_JumpRaces(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xb671abb6, input, index);
    }
    bool KnockoutTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x777ece27);
    }
    bool KnockoutTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x777ece27, result);
    }
    const EA::Reflection::Float &KnockoutTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x777ece27);
    }
    bool SET_KnockoutTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x777ece27, input);
    }
    bool KnockoutsPerLap(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x181462da);
    }
    bool KnockoutsPerLap(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x181462da, result);
    }
    const EA::Reflection::Int32 &KnockoutsPerLap() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x181462da);
    }
    bool SET_KnockoutsPerLap(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x181462da, input);
    }
    bool LocalizationTag(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xdb89ab5c);
    }
    bool LocalizationTag(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xdb89ab5c, result);
    }
    const EA::Reflection::Int32 &LocalizationTag() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xdb89ab5c);
    }
    bool SET_LocalizationTag(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xdb89ab5c, input);
    }
    bool MasterCheckpoint(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x609febe8);
    }
    bool MasterCheckpoint(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x609febe8, result);
    }
    const GCollectionKey &MasterCheckpoint() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x609febe8);
    }
    bool SET_MasterCheckpoint(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x609febe8, input);
    }
    bool MaxCarRep(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe10fb7a3);
    }
    bool MaxCarRep(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe10fb7a3, result);
    }
    const EA::Reflection::Float &MaxCarRep() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe10fb7a3);
    }
    bool SET_MaxCarRep(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe10fb7a3, input);
    }
    bool MaxHeatLevel(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf5a03629);
    }
    bool MaxHeatLevel(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf5a03629, result);
    }
    const EA::Reflection::Float &MaxHeatLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf5a03629);
    }
    bool SET_MaxHeatLevel(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf5a03629, input);
    }
    bool MaxOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe8c24416);
    }
    bool MaxOpenWorldHeat(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe8c24416, result);
    }
    const EA::Reflection::Float &MaxOpenWorldHeat() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe8c24416);
    }
    bool SET_MaxOpenWorldHeat(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe8c24416, input);
    }
    bool MaxPursuitRep(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xa07ae814);
    }
    bool MaxPursuitRep(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xa07ae814, result);
    }
    const EA::Reflection::Float &MaxPursuitRep() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xa07ae814);
    }
    bool SET_MaxPursuitRep(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xa07ae814, input);
    }
    bool MedalBonusBronze(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x3bb31211);
    }
    bool MedalBonusBronze(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x3bb31211, result);
    }
    const EA::Reflection::Int32 &MedalBonusBronze() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x3bb31211);
    }
    bool SET_MedalBonusBronze(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x3bb31211, input);
    }
    bool MedalBonusGold(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf5e43987);
    }
    bool MedalBonusGold(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xf5e43987, result);
    }
    const EA::Reflection::Int32 &MedalBonusGold() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xf5e43987);
    }
    bool SET_MedalBonusGold(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xf5e43987, input);
    }
    bool MedalBonusSilver(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x767b00a9);
    }
    bool MedalBonusSilver(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x767b00a9, result);
    }
    const EA::Reflection::Int32 &MedalBonusSilver() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x767b00a9);
    }
    bool SET_MedalBonusSilver(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x767b00a9, input);
    }
    bool MilestoneBiggerIsBetter(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0896d043);
    }
    bool MilestoneBiggerIsBetter(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0896d043, result);
    }
    const EA::Reflection::Bool &MilestoneBiggerIsBetter() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0896d043);
    }
    bool SET_MilestoneBiggerIsBetter(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0896d043, input);
    }
    bool MilestoneChallenge(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xbcd98737);
    }
    bool MilestoneChallenge(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xbcd98737, result);
    }
    const GCollectionKey &MilestoneChallenge() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xbcd98737);
    }
    bool SET_MilestoneChallenge(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xbcd98737, input);
    }
    bool MilestoneName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x704f72e8);
    }
    bool MilestoneName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x704f72e8, result);
    }
    const EA::Reflection::Text &MilestoneName() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x704f72e8);
    }
    bool SET_MilestoneName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x704f72e8, input);
    }
    bool MiniMapItem(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x051e90ca);
    }
    bool MiniMapItem(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x051e90ca, result);
    }
    const GCollectionKey &MiniMapItem() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x051e90ca);
    }
    bool SET_MiniMapItem(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x051e90ca, input);
    }
    bool MinimumAIPerformance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xb1ece070);
    }
    bool MinimumAIPerformance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xb1ece070, result);
    }
    const EA::Reflection::Float &MinimumAIPerformance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xb1ece070);
    }
    bool SET_MinimumAIPerformance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xb1ece070, input);
    }
    bool NISShell(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x4c17fe41);
    }
    bool NISShell(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x4c17fe41, result);
    }
    const EA::Reflection::Bool &NISShell() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x4c17fe41);
    }
    bool SET_NISShell(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x4c17fe41, input);
    }
    bool Name(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x3e225ec1);
    }
    bool Name(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x3e225ec1, result);
    }
    const EA::Reflection::Text &Name() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x3e225ec1);
    }
    bool SET_Name(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x3e225ec1, input);
    }
    bool NeverInQuickRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xa4e6fcfd);
    }
    bool NeverInQuickRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xa4e6fcfd, result);
    }
    const EA::Reflection::Bool &NeverInQuickRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xa4e6fcfd);
    }
    bool SET_NeverInQuickRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xa4e6fcfd, input);
    }
    bool NoPostRaceScreen(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x40f9929f);
    }
    bool NoPostRaceScreen(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x40f9929f, result);
    }
    const EA::Reflection::Bool &NoPostRaceScreen() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x40f9929f);
    }
    bool SET_NoPostRaceScreen(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x40f9929f, input);
    }
    bool NumLaps(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0ebdc165);
    }
    bool NumLaps(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x0ebdc165, result);
    }
    const EA::Reflection::Int32 &NumLaps() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x0ebdc165);
    }
    bool SET_NumLaps(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x0ebdc165, input);
    }
    bool NumRacesRequired(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x90a22a3f);
    }
    bool NumRacesRequired(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x90a22a3f, result);
    }
    const EA::Reflection::Int32 &NumRacesRequired() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x90a22a3f);
    }
    bool SET_NumRacesRequired(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x90a22a3f, input);
    }
    bool OneShot(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xce4261ac);
    }
    bool OneShot(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xce4261ac, result);
    }
    const EA::Reflection::Bool &OneShot() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xce4261ac);
    }
    bool SET_OneShot(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xce4261ac, input);
    }
    bool OpenWorldSpeedTrap(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x1bb16f14);
    }
    bool OpenWorldSpeedTrap(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x1bb16f14, result);
    }
    const EA::Reflection::Bool &OpenWorldSpeedTrap() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x1bb16f14);
    }
    bool SET_OpenWorldSpeedTrap(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x1bb16f14, input);
    }
    bool Opponents(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x5839fa1a);
    }
    bool Opponents(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x5839fa1a, result, index);
    }
    const GCollectionKey &Opponents(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x5839fa1a, index);
    }
    unsigned int Num_Opponents() const {
        ATTRIB_CODEGEN_GETLENGTH(0x5839fa1a);
    }
    bool SET_Opponents(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x5839fa1a, input, index);
    }
    bool OutroCameraTrack(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x7054ff5b);
    }
    bool OutroCameraTrack(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x7054ff5b, result);
    }
    const EA::Reflection::Int32 &OutroCameraTrack() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x7054ff5b);
    }
    bool SET_OutroCameraTrack(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x7054ff5b, input);
    }
    bool OutroMessageID(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc36e3532);
    }
    bool OutroMessageID(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xc36e3532, result);
    }
    const EA::Reflection::Int32 &OutroMessageID() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xc36e3532);
    }
    bool SET_OutroMessageID(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xc36e3532, input);
    }
    bool OutroMovie(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xb70268c0);
    }
    bool OutroMovie(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xb70268c0, result);
    }
    const EA::Reflection::Text &OutroMovie() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xb70268c0);
    }
    bool SET_OutroMovie(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xb70268c0, input);
    }
    bool OutroNIS(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x54932966);
    }
    bool OutroNIS(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x54932966, result);
    }
    const EA::Reflection::Text &OutroNIS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x54932966);
    }
    bool SET_OutroNIS(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x54932966, input);
    }
    bool OutroNISMarker(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x0e265c88);
    }
    bool OutroNISMarker(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x0e265c88, result);
    }
    const GCollectionKey &OutroNISMarker() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x0e265c88);
    }
    bool SET_OutroNISMarker(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x0e265c88, input);
    }
    bool OvertimePenaltyPerSec(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x26fd42b0);
    }
    bool OvertimePenaltyPerSec(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x26fd42b0, result);
    }
    const EA::Reflection::Int32 &OvertimePenaltyPerSec() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x26fd42b0);
    }
    bool SET_OvertimePenaltyPerSec(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x26fd42b0, input);
    }
    bool ParticleEffect(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x5ef34802);
    }
    bool ParticleEffect(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x5ef34802, result);
    }
    const EA::Reflection::Text &ParticleEffect() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x5ef34802);
    }
    bool SET_ParticleEffect(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x5ef34802, input);
    }
    bool Persistent(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xe4542e9b);
    }
    bool Persistent(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xe4542e9b, result);
    }
    const EA::Reflection::Bool &Persistent() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xe4542e9b);
    }
    bool SET_Persistent(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xe4542e9b, input);
    }
    bool PlayerCarPerformance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xfb42c0b9);
    }
    bool PlayerCarPerformance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xfb42c0b9, result);
    }
    const EA::Reflection::Float &PlayerCarPerformance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xfb42c0b9);
    }
    bool SET_PlayerCarPerformance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xfb42c0b9, input);
    }
    bool PlayerCarType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xc0eeb909);
    }
    bool PlayerCarType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xc0eeb909, result);
    }
    const EA::Reflection::Text &PlayerCarType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xc0eeb909);
    }
    bool SET_PlayerCarType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xc0eeb909, input);
    }
    bool Position(TAttrib<UMath::Vector3> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector3, 0x9f743a0e);
    }
    bool Position(UMath::Vector3 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector3, 0x9f743a0e, result);
    }
    const UMath::Vector3 &Position() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector3, 0x9f743a0e);
    }
    bool SET_Position(const UMath::Vector3 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector3, 0x9f743a0e, input);
    }
    bool PostRaceActivity(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x64273c71);
    }
    bool PostRaceActivity(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x64273c71, result);
    }
    const GCollectionKey &PostRaceActivity() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x64273c71);
    }
    bool SET_PostRaceActivity(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x64273c71, input);
    }
    bool PostRaceScreenTexture(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x038a3b53);
    }
    bool PostRaceScreenTexture(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x038a3b53, result);
    }
    const EA::Reflection::Text &PostRaceScreenTexture() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x038a3b53);
    }
    bool SET_PostRaceScreenTexture(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x038a3b53, input);
    }
    bool PresetRide(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x416a8409);
    }
    bool PresetRide(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x416a8409, result);
    }
    const EA::Reflection::Text &PresetRide() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x416a8409);
    }
    bool SET_PresetRide(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x416a8409, input);
    }
    bool ProgressionLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x740e9b4a);
    }
    bool ProgressionLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x740e9b4a, result);
    }
    const EA::Reflection::Int32 &ProgressionLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x740e9b4a);
    }
    bool SET_ProgressionLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x740e9b4a, input);
    }
    bool PursuitLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x0261ae99);
    }
    bool PursuitLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x0261ae99, result);
    }
    const EA::Reflection::Int32 &PursuitLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x0261ae99);
    }
    bool SET_PursuitLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x0261ae99, input);
    }
    bool PursuitRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x2b1f54f6);
    }
    bool PursuitRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x2b1f54f6, result);
    }
    const EA::Reflection::Bool &PursuitRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x2b1f54f6);
    }
    bool SET_PursuitRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x2b1f54f6, input);
    }
    bool QuickRaceNIS(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x5987fb25);
    }
    bool QuickRaceNIS(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x5987fb25, result);
    }
    const EA::Reflection::Text &QuickRaceNIS() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x5987fb25);
    }
    bool SET_QuickRaceNIS(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x5987fb25, input);
    }
    bool QuickRaceUnlocked(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xc4db4e71);
    }
    bool QuickRaceUnlocked(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xc4db4e71, result);
    }
    const EA::Reflection::Bool &QuickRaceUnlocked() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xc4db4e71);
    }
    bool SET_QuickRaceUnlocked(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xc4db4e71, input);
    }
    bool RaceLength(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7c11c52e);
    }
    bool RaceLength(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7c11c52e, result);
    }
    const EA::Reflection::Float &RaceLength() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7c11c52e);
    }
    bool SET_RaceLength(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7c11c52e, input);
    }
    bool RaceList(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x9f914008);
    }
    bool RaceList(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x9f914008, result, index);
    }
    const GCollectionKey &RaceList(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x9f914008, index);
    }
    unsigned int Num_RaceList() const {
        ATTRIB_CODEGEN_GETLENGTH(0x9f914008);
    }
    bool SET_RaceList(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x9f914008, input, index);
    }
    bool RaceTriggers(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x58dc14c0);
    }
    bool RaceTriggers(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x58dc14c0, result, index);
    }
    const GCollectionKey &RaceTriggers(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x58dc14c0, index);
    }
    unsigned int Num_RaceTriggers() const {
        ATTRIB_CODEGEN_GETLENGTH(0x58dc14c0);
    }
    bool SET_RaceTriggers(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x58dc14c0, input, index);
    }
    bool RacerName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xbeab64c5);
    }
    bool RacerName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xbeab64c5, result);
    }
    const EA::Reflection::Text &RacerName() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xbeab64c5);
    }
    bool SET_RacerName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xbeab64c5, input);
    }
    bool Radius(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x39bf8002);
    }
    bool Radius(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x39bf8002, result);
    }
    const EA::Reflection::Float &Radius() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x39bf8002);
    }
    bool SET_Radius(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x39bf8002, input);
    }
    bool RandomSpawnTriggers(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfdfe1c3e);
    }
    bool RandomSpawnTriggers(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xfdfe1c3e, result, index);
    }
    const GCollectionKey &RandomSpawnTriggers(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xfdfe1c3e, index);
    }
    unsigned int Num_RandomSpawnTriggers() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfdfe1c3e);
    }
    bool SET_RandomSpawnTriggers(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xfdfe1c3e, input, index);
    }
    bool RankPlayersByDistance(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x9e7a18ce);
    }
    bool RankPlayersByDistance(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x9e7a18ce, result);
    }
    const EA::Reflection::Bool &RankPlayersByDistance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x9e7a18ce);
    }
    bool SET_RankPlayersByDistance(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x9e7a18ce, input);
    }
    bool RankPlayersByPoints(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x5ec1880f);
    }
    bool RankPlayersByPoints(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x5ec1880f, result);
    }
    const EA::Reflection::Bool &RankPlayersByPoints() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x5ec1880f);
    }
    bool SET_RankPlayersByPoints(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x5ec1880f, input);
    }
    bool Region(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xcb01e454);
    }
    bool Region(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xcb01e454, result);
    }
    const EA::Reflection::Text &Region() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xcb01e454);
    }
    bool SET_Region(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xcb01e454, input);
    }
    bool Reputation(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x477ec5aa);
    }
    bool Reputation(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x477ec5aa, result);
    }
    const EA::Reflection::Int32 &Reputation() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x477ec5aa);
    }
    bool SET_Reputation(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x477ec5aa, input);
    }
    bool ReputationRequired(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xffd69c94);
    }
    bool ReputationRequired(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xffd69c94, result);
    }
    const EA::Reflection::Int32 &ReputationRequired() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xffd69c94);
    }
    bool SET_ReputationRequired(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xffd69c94, input);
    }
    bool RequiredBounty(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd3657d92);
    }
    bool RequiredBounty(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd3657d92, result);
    }
    const EA::Reflection::Int32 &RequiredBounty() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd3657d92);
    }
    bool SET_RequiredBounty(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd3657d92, input);
    }
    bool RequiredChallenges(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x6dd4b98b);
    }
    bool RequiredChallenges(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x6dd4b98b, result);
    }
    const EA::Reflection::Int32 &RequiredChallenges() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x6dd4b98b);
    }
    bool SET_RequiredChallenges(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x6dd4b98b, input);
    }
    bool RequiredRacesWon(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd617fedc);
    }
    bool RequiredRacesWon(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd617fedc, result);
    }
    const EA::Reflection::Int32 &RequiredRacesWon() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd617fedc);
    }
    bool SET_RequiredRacesWon(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd617fedc, input);
    }
    bool ResetTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x2c44ff10);
    }
    bool ResetTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x2c44ff10, result);
    }
    const EA::Reflection::Float &ResetTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x2c44ff10);
    }
    bool SET_ResetTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x2c44ff10, input);
    }
    bool ResetsPlayer(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x6ccd5819);
    }
    bool ResetsPlayer(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x6ccd5819, result);
    }
    const EA::Reflection::Bool &ResetsPlayer() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x6ccd5819);
    }
    bool SET_ResetsPlayer(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x6ccd5819, input);
    }
    bool RespawnMarker(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x2241f4cd);
    }
    bool RespawnMarker(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x2241f4cd, result);
    }
    const GCollectionKey &RespawnMarker() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x2241f4cd);
    }
    bool SET_RespawnMarker(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x2241f4cd, input);
    }
    bool RestartActivity(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xdc44bd08);
    }
    bool RestartActivity(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xdc44bd08, result);
    }
    const GCollectionKey &RestartActivity() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xdc44bd08);
    }
    bool SET_RestartActivity(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xdc44bd08, input);
    }
    bool RewardMarkerType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x06a077d5);
    }
    bool RewardMarkerType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x06a077d5, result);
    }
    const EA::Reflection::Text &RewardMarkerType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x06a077d5);
    }
    bool SET_RewardMarkerType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x06a077d5, input);
    }
    bool RewardsForWinner(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x50104d90);
    }
    bool RewardsForWinner(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x50104d90, result, index);
    }
    const GCollectionKey &RewardsForWinner(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x50104d90, index);
    }
    unsigned int Num_RewardsForWinner() const {
        ATTRIB_CODEGEN_GETLENGTH(0x50104d90);
    }
    bool SET_RewardsForWinner(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x50104d90, input, index);
    }
    bool RingTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc516e9c2);
    }
    bool RingTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc516e9c2, result);
    }
    const EA::Reflection::Float &RingTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc516e9c2);
    }
    bool SET_RingTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc516e9c2, input);
    }
    bool RivalBestTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xf9120d73);
    }
    bool RivalBestTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xf9120d73, result);
    }
    const EA::Reflection::Float &RivalBestTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xf9120d73);
    }
    bool SET_RivalBestTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xf9120d73, input);
    }
    bool RoadList(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x13b11b40);
    }
    bool RoadList(EA::Reflection::Text &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(EA::Reflection::Text, 0x13b11b40, result, index);
    }
    const EA::Reflection::Text &RoadList(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(EA::Reflection::Text, 0x13b11b40, index);
    }
    unsigned int Num_RoadList() const {
        ATTRIB_CODEGEN_GETLENGTH(0x13b11b40);
    }
    bool SET_RoadList(const EA::Reflection::Text &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(EA::Reflection::Text, 0x13b11b40, input, index);
    }
    bool RollingStart(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0xb809d19c);
    }
    bool RollingStart(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0xb809d19c, result);
    }
    const EA::Reflection::Bool &RollingStart() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0xb809d19c);
    }
    bool SET_RollingStart(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0xb809d19c, input);
    }
    bool Rotation(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5a6a57c6);
    }
    bool Rotation(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5a6a57c6, result);
    }
    const EA::Reflection::Float &Rotation() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5a6a57c6);
    }
    bool SET_Rotation(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5a6a57c6, input);
    }
    bool SMSCellChallenge(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x6a4cd2d4);
    }
    bool SMSCellChallenge(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x6a4cd2d4, result);
    }
    const EA::Reflection::Int32 &SMSCellChallenge() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x6a4cd2d4);
    }
    bool SET_SMSCellChallenge(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x6a4cd2d4, input);
    }
    bool SMSRivalChallenge(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xbb30c804);
    }
    bool SMSRivalChallenge(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xbb30c804, result);
    }
    const EA::Reflection::Int32 &SMSRivalChallenge() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xbb30c804);
    }
    bool SET_SMSRivalChallenge(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xbb30c804, input);
    }
    bool ScaleOpenWorldHeat(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x1823b89e);
    }
    bool ScaleOpenWorldHeat(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x1823b89e, result);
    }
    const EA::Reflection::Float &ScaleOpenWorldHeat() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x1823b89e);
    }
    bool SET_ScaleOpenWorldHeat(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x1823b89e, input);
    }
    bool ScriptedCopsInRace(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0e47fe63);
    }
    bool ScriptedCopsInRace(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0e47fe63, result);
    }
    const EA::Reflection::Bool &ScriptedCopsInRace() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0e47fe63);
    }
    bool SET_ScriptedCopsInRace(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0e47fe63, input);
    }
    bool SharedCheckpoints(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x0e34a1f3);
    }
    bool SharedCheckpoints(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x0e34a1f3, result);
    }
    const EA::Reflection::Bool &SharedCheckpoints() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x0e34a1f3);
    }
    bool SET_SharedCheckpoints(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x0e34a1f3, input);
    }
    bool ShortcutMaxChance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x16faba11);
    }
    bool ShortcutMaxChance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x16faba11, result);
    }
    const EA::Reflection::Float &ShortcutMaxChance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x16faba11);
    }
    bool SET_ShortcutMaxChance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x16faba11, input);
    }
    bool ShortcutMinChance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4efb950a);
    }
    bool ShortcutMinChance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4efb950a, result);
    }
    const EA::Reflection::Float &ShortcutMinChance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4efb950a);
    }
    bool SET_ShortcutMinChance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4efb950a, input);
    }
    bool Shortcuts(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x7b6d296e);
    }
    bool Shortcuts(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x7b6d296e, result, index);
    }
    const GCollectionKey &Shortcuts(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x7b6d296e, index);
    }
    unsigned int Num_Shortcuts() const {
        ATTRIB_CODEGEN_GETLENGTH(0x7b6d296e);
    }
    bool SET_Shortcuts(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x7b6d296e, input, index);
    }
    bool SkillLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0x3b798aa2);
    }
    bool SkillLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0x3b798aa2, result);
    }
    const EA::Reflection::Int32 &SkillLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0x3b798aa2);
    }
    bool SET_SkillLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0x3b798aa2, input);
    }
    bool SpawnPoint(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xddf411f5);
    }
    bool SpawnPoint(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xddf411f5, result);
    }
    const GCollectionKey &SpawnPoint() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xddf411f5);
    }
    bool SET_SpawnPoint(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xddf411f5, input);
    }
    bool SpeedTrapCamera(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xcbd7adf9);
    }
    bool SpeedTrapCamera(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xcbd7adf9, result);
    }
    const EA::Reflection::Text &SpeedTrapCamera() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xcbd7adf9);
    }
    bool SET_SpeedTrapCamera(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xcbd7adf9, input);
    }
    bool SpeedTrapList(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x822179d1);
    }
    bool SpeedTrapList(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x822179d1, result, index);
    }
    const GCollectionKey &SpeedTrapList(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x822179d1, index);
    }
    unsigned int Num_SpeedTrapList() const {
        ATTRIB_CODEGEN_GETLENGTH(0x822179d1);
    }
    bool SET_SpeedTrapList(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x822179d1, input, index);
    }
    bool SpeedTrapTrigger(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x5f95c3a0);
    }
    bool SpeedTrapTrigger(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x5f95c3a0, result);
    }
    const GCollectionKey &SpeedTrapTrigger() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x5f95c3a0);
    }
    bool SET_SpeedTrapTrigger(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x5f95c3a0, input);
    }
    bool SpeedTrapsRequired(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x6d7e73c9);
    }
    bool SpeedTrapsRequired(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x6d7e73c9, result, index);
    }
    const GCollectionKey &SpeedTrapsRequired(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x6d7e73c9, index);
    }
    unsigned int Num_SpeedTrapsRequired() const {
        ATTRIB_CODEGEN_GETLENGTH(0x6d7e73c9);
    }
    bool SET_SpeedTrapsRequired(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x6d7e73c9, input, index);
    }
    bool StartPercent(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xe0d01505);
    }
    bool StartPercent(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xe0d01505, result);
    }
    const EA::Reflection::Float &StartPercent() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xe0d01505);
    }
    bool SET_StartPercent(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xe0d01505, input);
    }
    bool StartTime(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x839602ab);
    }
    bool StartTime(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x839602ab, result);
    }
    const EA::Reflection::Float &StartTime() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x839602ab);
    }
    bool SET_StartTime(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x839602ab, input);
    }
    bool TOD(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x9dff3c3d);
    }
    bool TOD(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x9dff3c3d, result);
    }
    const EA::Reflection::Float &TOD() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x9dff3c3d);
    }
    bool SET_TOD(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x9dff3c3d, input);
    }
    bool TargetActivities(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x0f37d221);
    }
    bool TargetActivities(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x0f37d221, result, index);
    }
    const GCollectionKey &TargetActivities(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x0f37d221, index);
    }
    unsigned int Num_TargetActivities() const {
        ATTRIB_CODEGEN_GETLENGTH(0x0f37d221);
    }
    bool SET_TargetActivities(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x0f37d221, input, index);
    }
    bool TargetActivity(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x277566f3);
    }
    bool TargetActivity(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x277566f3, result);
    }
    const GCollectionKey &TargetActivity() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x277566f3);
    }
    bool SET_TargetActivity(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x277566f3, input);
    }
    bool TargetBronze(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x00df8eb4);
    }
    bool TargetBronze(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x00df8eb4, result);
    }
    const EA::Reflection::Float &TargetBronze() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x00df8eb4);
    }
    bool SET_TargetBronze(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x00df8eb4, input);
    }
    bool TargetGold(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x728e43ff);
    }
    bool TargetGold(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x728e43ff, result);
    }
    const EA::Reflection::Float &TargetGold() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x728e43ff);
    }
    bool SET_TargetGold(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x728e43ff, input);
    }
    bool TargetMarker(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x8fc356fb);
    }
    bool TargetMarker(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x8fc356fb, result);
    }
    const GCollectionKey &TargetMarker() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x8fc356fb);
    }
    bool SET_TargetMarker(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x8fc356fb, input);
    }
    bool TargetSilver(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x51ce16b7);
    }
    bool TargetSilver(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x51ce16b7, result);
    }
    const EA::Reflection::Float &TargetSilver() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x51ce16b7);
    }
    bool SET_TargetSilver(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x51ce16b7, input);
    }
    bool Template(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x3e9156ca);
    }
    bool Template(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x3e9156ca, result);
    }
    const EA::Reflection::Bool &Template() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x3e9156ca);
    }
    bool SET_Template(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x3e9156ca, input);
    }
    bool ThreshholdSpeed(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc3710777);
    }
    bool ThreshholdSpeed(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc3710777, result);
    }
    const EA::Reflection::Float &ThreshholdSpeed() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc3710777);
    }
    bool SET_ThreshholdSpeed(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc3710777, input);
    }
    bool ThreshholdValue(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x4e90219d);
    }
    bool ThreshholdValue(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x4e90219d, result);
    }
    const EA::Reflection::Float &ThreshholdValue() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x4e90219d);
    }
    bool SET_ThreshholdValue(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x4e90219d, input);
    }
    bool TimeBonus(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xf52cc30e);
    }
    bool TimeBonus(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xf52cc30e, result);
    }
    const EA::Reflection::Int32 &TimeBonus() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xf52cc30e);
    }
    bool SET_TimeBonus(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xf52cc30e, input);
    }
    bool TimeLimit(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x7585f041);
    }
    bool TimeLimit(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x7585f041, result);
    }
    const EA::Reflection::Float &TimeLimit() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x7585f041);
    }
    bool SET_TimeLimit(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x7585f041, input);
    }
    bool TokenValue(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xaa10914c);
    }
    bool TokenValue(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xaa10914c, result);
    }
    const EA::Reflection::Int32 &TokenValue() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xaa10914c);
    }
    bool SET_TokenValue(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xaa10914c, input);
    }
    bool TrafficCharacter(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x797d9654);
    }
    bool TrafficCharacter(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x797d9654, result);
    }
    const GCollectionKey &TrafficCharacter() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x797d9654);
    }
    bool SET_TrafficCharacter(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x797d9654, input);
    }
    bool TrafficLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xc64bc341);
    }
    bool TrafficLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xc64bc341, result);
    }
    const EA::Reflection::Int32 &TrafficLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xc64bc341);
    }
    bool SET_TrafficLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xc64bc341, input);
    }
    bool TrafficPattern(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x6319b692);
    }
    bool TrafficPattern(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x6319b692, result);
    }
    const EA::Reflection::Text &TrafficPattern() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x6319b692);
    }
    bool SET_TrafficPattern(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x6319b692, input);
    }
    bool UnlockRaces(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfc8995c8);
    }
    bool UnlockRaces(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xfc8995c8, result, index);
    }
    const GCollectionKey &UnlockRaces(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xfc8995c8, index);
    }
    unsigned int Num_UnlockRaces() const {
        ATTRIB_CODEGEN_GETLENGTH(0xfc8995c8);
    }
    bool SET_UnlockRaces(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xfc8995c8, input, index);
    }
    bool UpgradeLevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd267facc);
    }
    bool UpgradeLevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd267facc, result);
    }
    const EA::Reflection::Int32 &UpgradeLevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd267facc);
    }
    bool SET_UpgradeLevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd267facc, input);
    }
    bool UpgradePartID(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x3c2fdaab);
    }
    bool UpgradePartID(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x3c2fdaab, result);
    }
    const EA::Reflection::Text &UpgradePartID() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x3c2fdaab);
    }
    bool SET_UpgradePartID(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x3c2fdaab, input);
    }
    bool UpgradePartName(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xc385f75d);
    }
    bool UpgradePartName(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xc385f75d, result);
    }
    const EA::Reflection::Text &UpgradePartName() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xc385f75d);
    }
    bool SET_UpgradePartName(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xc385f75d, input);
    }
    bool UpgradeType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x0e0113fe);
    }
    bool UpgradeType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x0e0113fe, result);
    }
    const EA::Reflection::Text &UpgradeType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x0e0113fe);
    }
    bool SET_UpgradeType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x0e0113fe, input);
    }
    bool UseWorldHeat(TAttrib<EA::Reflection::Bool> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Bool, 0x45f2ad6c);
    }
    bool UseWorldHeat(EA::Reflection::Bool &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Bool, 0x45f2ad6c, result);
    }
    const EA::Reflection::Bool &UseWorldHeat() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Bool, 0x45f2ad6c);
    }
    bool SET_UseWorldHeat(const EA::Reflection::Bool &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Bool, 0x45f2ad6c, input);
    }
    bool Width(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0x5816c1fc);
    }
    bool Width(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0x5816c1fc, result);
    }
    const EA::Reflection::Float &Width() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0x5816c1fc);
    }
    bool SET_Width(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0x5816c1fc, input);
    }
    bool WorldRaces(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa7ef40ef);
    }
    bool WorldRaces(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0xa7ef40ef, result, index);
    }
    const GCollectionKey &WorldRaces(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0xa7ef40ef, index);
    }
    unsigned int Num_WorldRaces() const {
        ATTRIB_CODEGEN_GETLENGTH(0xa7ef40ef);
    }
    bool SET_WorldRaces(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0xa7ef40ef, input, index);
    }
    bool ZoneList(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x64893da8);
    }
    bool ZoneList(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x64893da8, result, index);
    }
    const GCollectionKey &ZoneList(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x64893da8, index);
    }
    unsigned int Num_ZoneList() const {
        ATTRIB_CODEGEN_GETLENGTH(0x64893da8);
    }
    bool SET_ZoneList(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x64893da8, input, index);
    }
    bool ZoneType(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0xf3ea3201);
    }
    bool ZoneType(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0xf3ea3201, result);
    }
    const EA::Reflection::Text &ZoneType() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0xf3ea3201);
    }
    bool SET_ZoneType(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0xf3ea3201, input);
    }
    bool actionscript(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xe62083d0);
    }
    bool actionscript(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xe62083d0, result);
    }
    const GCollectionKey &actionscript() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xe62083d0);
    }
    bool SET_actionscript(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xe62083d0, input);
    }
    bool bytecode(TAttrib<Attrib::Blob> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(Attrib::Blob, 0x9a4a020a);
    }
    bool bytecode(Attrib::Blob &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(Attrib::Blob, 0x9a4a020a, result);
    }
    const Attrib::Blob &bytecode() const {
        ATTRIB_CODEGEN_GETVALUE(Attrib::Blob, 0x9a4a020a);
    }
    bool SET_bytecode(const Attrib::Blob &input) {
        ATTRIB_CODEGEN_SETVALUE(Attrib::Blob, 0x9a4a020a, input);
    }
    bool disengagetrigger(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x106285c0);
    }
    bool disengagetrigger(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x106285c0, result);
    }
    const GCollectionKey &disengagetrigger() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x106285c0);
    }
    bool SET_disengagetrigger(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x106285c0, input);
    }
    bool distance(TAttrib<EA::Reflection::Float> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Float, 0xc5857615);
    }
    bool distance(EA::Reflection::Float &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Float, 0xc5857615, result);
    }
    const EA::Reflection::Float &distance() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Float, 0xc5857615);
    }
    bool SET_distance(const EA::Reflection::Float &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Float, 0xc5857615, input);
    }
    bool engagetrigger(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xf05931ab);
    }
    bool engagetrigger(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xf05931ab, result);
    }
    const GCollectionKey &engagetrigger() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xf05931ab);
    }
    bool SET_engagetrigger(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xf05931ab, input);
    }
    bool gameplayvault(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x93fd9fda);
    }
    bool gameplayvault(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x93fd9fda, result);
    }
    const EA::Reflection::Text &gameplayvault() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x93fd9fda);
    }
    bool SET_gameplayvault(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x93fd9fda, input);
    }
    bool handler_owner(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x857fe432);
    }
    bool handler_owner(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x857fe432, result);
    }
    const GCollectionKey &handler_owner() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x857fe432);
    }
    bool SET_handler_owner(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x857fe432, input);
    }
    bool layoutpos(TAttrib<UMath::Vector2> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(UMath::Vector2, 0x4075ec46);
    }
    bool layoutpos(UMath::Vector2 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(UMath::Vector2, 0x4075ec46, result);
    }
    const UMath::Vector2 &layoutpos() const {
        ATTRIB_CODEGEN_GETVALUE(UMath::Vector2, 0x4075ec46);
    }
    bool SET_layoutpos(const UMath::Vector2 &input) {
        ATTRIB_CODEGEN_SETVALUE(UMath::Vector2, 0x4075ec46, input);
    }
    bool message_id(TAttrib<EA::Reflection::UInt32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::UInt32, 0x9e8910ef);
    }
    bool message_id(EA::Reflection::UInt32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETLAYOUT(message_id, result);
    }
    const EA::Reflection::UInt32 &message_id() const {
        ATTRIB_CODEGEN_GETLAYOUT(message_id);
    }
    bool SET_message_id(const EA::Reflection::UInt32 &input) {
        ATTRIB_CODEGEN_SETLAYOUT(message_id, input);
    }
    bool nitrouslevel(TAttrib<EA::Reflection::Int32> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Int32, 0xd4797aa8);
    }
    bool nitrouslevel(EA::Reflection::Int32 &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Int32, 0xd4797aa8, result);
    }
    const EA::Reflection::Int32 &nitrouslevel() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Int32, 0xd4797aa8);
    }
    bool SET_nitrouslevel(const EA::Reflection::Int32 &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Int32, 0xd4797aa8, input);
    }
    bool parentstate(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x4acc6d63);
    }
    bool parentstate(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x4acc6d63, result);
    }
    const GCollectionKey &parentstate() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x4acc6d63);
    }
    bool SET_parentstate(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x4acc6d63, input);
    }
    bool racefinish(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xb0a24adc);
    }
    bool racefinish(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xb0a24adc, result);
    }
    const GCollectionKey &racefinish() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xb0a24adc);
    }
    bool SET_racefinish(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xb0a24adc, input);
    }
    bool racefinishReverse(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x7c7cf20f);
    }
    bool racefinishReverse(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x7c7cf20f, result);
    }
    const GCollectionKey &racefinishReverse() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x7c7cf20f);
    }
    bool SET_racefinishReverse(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x7c7cf20f, input);
    }
    bool racestart(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xe43b2ccc);
    }
    bool racestart(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xe43b2ccc, result);
    }
    const GCollectionKey &racestart() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xe43b2ccc);
    }
    bool SET_racestart(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xe43b2ccc, input);
    }
    bool racestartReverse(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xfd945479);
    }
    bool racestartReverse(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xfd945479, result);
    }
    const GCollectionKey &racestartReverse() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xfd945479);
    }
    bool SET_racestartReverse(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xfd945479, input);
    }
    bool scriptname(TAttrib<EA::Reflection::Text> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(EA::Reflection::Text, 0x7148ae82);
    }
    bool scriptname(EA::Reflection::Text &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(EA::Reflection::Text, 0x7148ae82, result);
    }
    const EA::Reflection::Text &scriptname() const {
        ATTRIB_CODEGEN_GETVALUE(EA::Reflection::Text, 0x7148ae82);
    }
    bool SET_scriptname(const EA::Reflection::Text &input) {
        ATTRIB_CODEGEN_SETVALUE(EA::Reflection::Text, 0x7148ae82, input);
    }
    bool sender(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa590a98b);
    }
    bool sender(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa590a98b, result);
    }
    const GCollectionKey &sender() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa590a98b);
    }
    bool SET_sender(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa590a98b, input);
    }
    bool stateref(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x918c796e);
    }
    bool stateref(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x918c796e, result);
    }
    const GCollectionKey &stateref() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x918c796e);
    }
    bool SET_stateref(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x918c796e, input);
    }
    bool target(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x1a7d2859);
    }
    bool target(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0x1a7d2859, result);
    }
    const GCollectionKey &target() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0x1a7d2859);
    }
    bool SET_target(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0x1a7d2859, input);
    }
    bool templateref(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0xa0697302);
    }
    bool templateref(GCollectionKey &result) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUE(GCollectionKey, 0xa0697302, result);
    }
    const GCollectionKey &templateref() const {
        ATTRIB_CODEGEN_GETVALUE(GCollectionKey, 0xa0697302);
    }
    bool SET_templateref(const GCollectionKey &input) {
        ATTRIB_CODEGEN_SETVALUE(GCollectionKey, 0xa0697302, input);
    }
    bool transitionlist(TAttrib<GCollectionKey> &result) const {
        ATTRIB_CODEGEN_GETATTRIB(GCollectionKey, 0x25621dc5);
    }
    bool transitionlist(GCollectionKey &result, unsigned int index) const {
        ATTRIB_CODEGEN_CHECKEDGETVALUEINDEXED(GCollectionKey, 0x25621dc5, result, index);
    }
    const GCollectionKey &transitionlist(unsigned int index) const {
        ATTRIB_CODEGEN_GETVALUEINDEXED(GCollectionKey, 0x25621dc5, index);
    }
    unsigned int Num_transitionlist() const {
        ATTRIB_CODEGEN_GETLENGTH(0x25621dc5);
    }
    bool SET_transitionlist(const GCollectionKey &input, unsigned int index) {
        ATTRIB_CODEGEN_SETVALUEINDEXED(GCollectionKey, 0x25621dc5, input, index);
    }

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

}; // namespace Attrib

#endif
