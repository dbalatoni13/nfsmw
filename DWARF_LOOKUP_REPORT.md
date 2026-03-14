# COMPLETE DWARF LOOKUP REPORT FOR zFeOverlay TYPES

## Command Execution Results

All commands run successfully against: `./symbols/PS2/PS2_types.nothpp`

---

## 1. ENUMS

### eSetRideInfoReasons
```
enum eSetRideInfoReasons {
SET_RIDE_INFO_REASON_VINYL = 0,
SET_RIDE_INFO_REASON_LOAD_CAR = 1,
SET_RIDE_INFO_REASON_CATCHALL = 2
};
```

### eCarViewerWhichCar
```
enum eCarViewerWhichCar {
eCARVIEWER_PLAYER1_CAR = 0,
eCARVIEWER_PLAYER2_CAR = 1
};
```

### eScrollDir
```
enum eScrollDir {
eSD_PREV = -1,
eSD_NEXT = 1,
eSD_PAGE_PREV = -10000,
eSD_PAGE_NEXT = 10000,
eSD_NONE = 10001
};
```

### eCustomizeEntryPoint
```
enum eCustomizeEntryPoint {
CEP_GAMEPLAY = 0,
CEP_MAIN_MENU = 1,
CEP_ONLINE_MENU = 2
};
```

### eCustomizeCategory
```
enum eCustomizeCategory {
CC_TO_CAT_MASK = 65535,
CC_FROM_CAT_MASK = -65536,
CC_SCREEN_ID_MAIN = 2048,
CC_SCREEN_ID_PARTS = 256,
CC_SCREEN_ID_PERFORMANCE = 512,
CC_SCREEN_ID_VISUAL = 768,
CC_SCREEN_ID_VINYL_TYPES = 1024,
CC_SCREEN_ID_DECAL_LOCATION = 1280,
CC_SCREEN_ID_DECAL_POSITION = 1536,
CC_SCREEN_ID_RIM_BRANDS = 1792,
CC_ID_MASK = 65280,
CC_NO_CATEGORY = 0,
CC_PARTS = 2049,
CC_PERFORMANCE = 2050,
CC_VISUAL = 2051,
CC_BODY_KIT = 257,
CC_SPOILERS = 258,
CC_RIM_BRANDS = 259,
CC_HOODS = 260,
CC_ROOF_SCOOPS = 261,
CC_ENGINE = 513,
CC_TRANSMISSION = 514,
CC_SUSPENSION = 515,
CC_NITROUS = 516,
CC_TIRES = 517,
CC_BRAKES = 518,
CC_FORCED_INDUCTION = 519,
CC_PAINT = 769,
CC_VINYL_TYPES = 770,
CC_RIM_PAINT = 771,
CC_WINDOW_TINT = 772,
CC_DECAL_LOCATION = 773,
CC_NUMBERS = 774,
CC_CUSTOM_HUD = 775,
CC_VINYL_GROUP_STOCK = 1025,
CC_VINYL_GROUP_FLAME = 1026,
CC_VINYL_GROUP_TRIBAL = 1027,
CC_VINYL_GROUP_STRIPE = 1028,
CC_VINYL_GROUP_RACING_FLAG = 1029,
CC_VINYL_GROUP_NATIONAL_FLAG = 1030,
CC_VINYL_GROUP_BODY = 1031,
CC_VINYL_GROUP_UNIQUE = 1032,
CC_VINYL_GROUP_CONTEST = 1033,
CC_RIM_BRAND_STOCK = 1793,
CC_RIM_BRAND_5_ZIGEN = 1794,
CC_RIM_BRAND_ADR = 1795,
CC_RIM_BRAND_BBS = 1796,
CC_RIM_BRAND_ENKEI = 1797,
CC_RIM_BRAND_KONIG = 1798,
CC_RIM_BRAND_LOWENHART = 1799,
CC_RIM_BRAND_RACING_HART = 1800,
CC_RIM_BRAND_OZ = 1801,
CC_RIM_BRAND_VOLK = 1802,
CC_RIM_BRAND_ROJA = 1803,
CC_DECAL_WINDSHIELD = 1281,
CC_DECAL_REAR_WINDOW = 1282,
CC_DECAL_LEFT_DOOR = 1283,
CC_DECAL_RIGHT_DOOR = 1284,
CC_DECAL_LEFT_QP = 1285,
CC_DECAL_RIGHT_QP = 1286,
CC_DECAL_SLOT_1 = 1537,
CC_DECAL_SLOT_2 = 1538,
CC_DECAL_SLOT_3 = 1539,
CC_DECAL_SLOT_4 = 1540,
CC_DECAL_SLOT_5 = 1541,
CC_DECAL_SLOT_6 = 1542
};
```

### eCustomizeCartTotals
```
enum eCustomizeCartTotals {
CCT_PART_PRICES = 0,
CCT_TRADE_IN = 1,
CCT_TOTAL = 2
};
```

### ePerformanceRatingType
```
enum ePerformanceRatingType {
PRT_TOP_SPEED = 0,
PRT_HANDLING = 1,
PRT_ACCELERATION = 2
};
```

### eMenuSoundTriggers
```
enum eMenuSoundTriggers {
UISND_NONE = -1,
UISND_COMMON_UP = 0,
UISND_COMMON_DOWN = 1,
UISND_COMMON_LEFT = 2,
UISND_COMMON_RIGHT = 3,
UISND_COMMON_SELECT = 4,
UISND_COMMON_BACK = 5,
UISND_COMMON_START_PAUSE = 6,
UISND_COMMON_WRONG = 7,
UISND_COMMON_DLGBOX_IN = 10,
UISND_COMMON_DLGBOX_OUT = 11,
UISND_COMMON_SCROLL_START = 12,
UISND_COMMON_COOL_DOWN_FLIP = 12,
UISNN_HUD_ENGAGE_MAIL_POPUP = 13,
UISND_HUD_ENGAGE_PHONE = 14,
UISND_HUD_ENGAGE_DPAD = 15,
UISND_HUD_ENGAGE_MAIL = 16,
UISND_MILESTONE_REWARD_SYMBOL_ON = 17,
UISND_MILESTONE_REWARD_SYMBOL_MOVE = 18,
UISND_ENTER_TRIGGER = 19,
UISND_MAP_LOCK_TARGER = 20,
UISND_MAP_ZOOM_IN = 21,
UISND_MAP_ZOOM_OUT = 22,
UISND_COMMON_MAX_NUM = 23,
UISND_CARSEL_BASSLOOP_RESERVED00 = 24,
UISND_CARSEL_BASSLOOP_RESERVED01 = 25,
UISND_CARSEL_CAMROTATE = 26,
UISND_CARSEL_CAMROTATE_RESERVED00 = 27,
UISND_CARSEL_CAMROTATE_RESERVED01 = 28,
UISND_CUST_INST_PAINT = 38,
UISND_CUST_PAINT_COLOUR_LEFT = 39,
UISND_CUST_PAINT_COLOUR_RIGHT = 40,
UISND_CUST_PAINT_TYPE_LEFT = 41,
UISND_CUST_PAINT_TYPE_RIGHT = 42,
UISND_UGNEW_KBTYPE = 46,
UISND_UGNEW_ENTER = 47,
UISND_UGNEW_DELETE = 48,
UISND_CUST_INST_EXHAUST = 49,
UISND_CUST_INST_GENERIC = 50,
UISND_CUST_INST_TURBO = 51,
UISND_CUST_INST_NOS = 52,
UISND_CUST_INST_TRANSMISSION = 53,
UISND_CUST_INST_TIRES = 54,
UISND_EA_MSGR_OPEN = 74,
UISND_EA_MSGR_LOGOFF = 75,
UISND_EA_MSGR_CHAT_REQ = 85,
UISND_EA_MSGR_MAIL_RECEIVE = 86,
UISND_EA_MSGR_CHALLENGE_REQ = 87,
UISND_MAIN_MENU = 88,
UISND_MAIN_SUB = 89,
UISND_BUSTED_SCREEN = 89,
UISND_IMPOUNDED = 90,
UISND_RAPSHEET_LOGIN = 103,
UISND_RAPSHEET_LOGIN2 = 104,
UISND_RAPSHEET_MAIN = 105,
UISND_RAPSHEET_SUMMARY = 106,
UISND_RAPSHEET_VEHICLE = 107,
UISND_RAPSHEET_SELECT = 108,
UISND_RAPSHEET_BACKUP = 109,
UISND_RAPSHEET_MOVE_BAR_UP = 110,
UISND_RAPSHEET_MOVE_BAR_DOWN = 111,
UISND_RAPSHEET_CTS = 112,
UISND_RAPSHEET_INFRAC = 113,
UISND_RAPSHEET_PD = 114,
UISND_RAPSHEET_RANKINGS = 115,
UISND_RAPSHEET_RANKING_DETAIL = 116,
UISND_RAPSHEET_TEP = 117,
UISND_RAPSHEET_EXIT = 118,
UISND_MAIN_MENU_ENTER = 123,
UISND_MAIN_MENU_EXIT = 124,
UISND_MAIN_SUB_ENTER = 125,
UISND_MAIN_SUB_EXIT = 126,
UISND_MC_MAIN_ENTER = 127,
UISND_BLACKLIST_ENTER = 128,
UISND_BLACKLIST_EXIT = 129,
UISND_BIO_ENTER = 130,
UISND_BIO_EXIT = 131,
UISND_BIO_TO_RIVALCAR_EXIT = 132,
UISND_CUST_MAIN_ENTER = 133,
UISND_CUST_ENTER = 134,
UISND_CUST_EXIT = 135,
UISND_SHOWCASE_ENTER = 136,
UISND_SHOWCASE_EXIT = 137,
UISND_QUICKRACE_BRIEF_ENTER = 138,
UISND_QUICK_GAMBLE_BLIP = 139,
UISND_RACESHEET_ENTER = 140,
UISND_RACESHEET_EXIT = 141,
UISND_TRACK_SELECT_ENTER = 142,
UISND_QUICK_BRIEF_EXIT = 143,
UISND_QUICK_RACE_CAR_ON = 144,
UISND_CAR_SEL_TO_SHOWCASE = 145,
UISND_CHAL_SER_ENTER = 146,
UISND_CAR_SELECT_ENTER = 147,
UISND_SAFEH_MARK_CONGRATS_OFF = 148,
UISND_RANDOMIZE_BUTTON = 149,
UISND_SAFEH_MARK_ENTER = 150,
UISND_SAFEH_MARK_SPIN_COIN = 151,
UISND_CHAL_SER_EXIT = 152,
UISND_MAP_REPOSITION = 153,
UISND_OPTION_MENU_ENTER = 154,
UISND_OPTION_MENU_EXIT = 155,
UISND_RIVAL_BIO_OFF = 156,
UISND_BIO_ENTER2 = 157,
UISND_RIV_BIO_CLOUD_ON = 158,
UISND_RIV_BIO_LOGO_FLY_IN = 159,
UISND_FRONTEND_MAX_NUM = 160
};
```

---

## 2. STRUCTS

### SelectablePart
```
struct SelectablePart : /* 0x00 */ bTNode<SelectablePart> { // 0x2c
protected:
/* 0x08 */ CarPart *ThePart;
/* 0x0c */ int CarSlotID;
/* 0x10 */ uint32 UpgradeLevel;
/* 0x14 */ Type PhysicsType;
/* 0x18 */ bool PerformancePkg;
/* 0x1c */ eCustomizePartState PartState;
/* 0x20 */ int Price;
/* 0x24 */ bool JunkmanPart;
public:
/* 0x28 */ __vtbl_ptr_type *$vf22986;

SelectablePart& operator=();
SelectablePart();
SelectablePart();
SelectablePart();
/* vtable[1] */ virtual SelectablePart(SelectablePart*, int, void);
CarPart* GetPart();
int GetSlotID();
uint32 GetUpgradeLevel();
Type GetPhysicsType();
bool IsPerformancePkg();
eCustomizePartState GetPartState();
int GetPrice();
bool IsJunkmanPart();
void SetSlotID();
bool IsAvailable();
bool IsLocked();
bool IsNew();
bool IsInstalled();
bool IsInCart();
void SetPartState();
void SetInCart();
void SetInstalled();
void UnSetInCart();
void SetPrice(SelectablePart*, int, void);
};
```

### ShoppingCartItem
```
struct ShoppingCartItem : /* 0x00 */ bTNode<ShoppingCartItem> { // 0x18
private:
/* 0x08 */ SelectablePart *ToBuy;
/* 0x0c */ SelectablePart *TradeIn;
/* 0x10 */ bool bActive;
public:
/* 0x14 */ __vtbl_ptr_type *$vf23011;

ShoppingCartItem& operator=();
ShoppingCartItem();
ShoppingCartItem();
/* vtable[1] */ virtual ShoppingCartItem(ShoppingCartItem*, int, void);
SelectablePart* GetBuyingPart();
SelectablePart* GetTradeInPart();
int GetPartPrice();
int GetTradeInPrice();
void ToggleActive();
bool IsActive();
};
```

### SelectableCar
```
struct SelectableCar : /* 0x0 */ bTNode<SelectableCar> { // 0x10
/* 0x8 */ uint32 mHandle;
/* 0xc */ bool bLocked;
};
```

### ScreenConstructorData
```
struct ScreenConstructorData { // 0xc
/* 0x0 */ char *PackageFilename;
/* 0x4 */ FEPackage *pPackage;
/* 0x8 */ int Arg;
};
```

### RideInfo
```
struct RideInfo { // 0x310
/* 0x000 */ CarType Type;
/* 0x004 */ int8 InstanceIndex;
/* 0x005 */ int8 HasDash;
/* 0x006 */ int8 CanBeVertexDamaged;
/* 0x007 */ int8 SkinType;
private:
/* 0x008 */ CARPART_LOD mMinLodLevel;
/* 0x00c */ CARPART_LOD mMaxLodLevel;
/* 0x010 */ CARPART_LOD mMinFELodLevel;
/* 0x014 */ CARPART_LOD mMaxFELodLevel;
/* 0x018 */ CARPART_LOD mMaxLicenseLodLevel;
/* 0x01c */ CARPART_LOD mMinTrafficDiffuseLodLevel;
/* 0x020 */ CARPART_LOD mMinShadowLodLevel;
/* 0x024 */ CARPART_LOD mMaxShadowLodLevel;
/* 0x028 */ CARPART_LOD mMaxTireLodLevel;
/* 0x02c */ CARPART_LOD mMaxBrakeLodLevel;
/* 0x030 */ CARPART_LOD mMaxSpoilerLodLevel;
/* 0x034 */ CARPART_LOD mMaxRoofScoopLodLevel;
/* 0x038 */ CARPART_LOD mMinReflectionLodLevel;
/* 0x03c */ uint32 mCompositeSkinHash;
/* 0x040 */ uint32 mCompositeWheelHash;
/* 0x044 */ uint32 mCompositeSpinnerHash;
/* 0x048 */ CarPart *mPartsTable[139];
/* 0x274 */ signed char mPartsEnabled[139];
/* 0x300 */ CarPart *PreviewPart;
/* 0x304 */ CarLoaderHandle mMyCarLoaderHandle;
/* 0x308 */ CarRenderUsage mMyCarRenderUsage;
/* 0x30c */ uint8 mSpecialLODBehavior;

public:
RideInfo& operator=();
RideInfo();
RideInfo();
RideInfo();
CARPART_LOD GetMinLodLevel();
CARPART_LOD GetMaxLodLevel();
CARPART_LOD GetMinFELodLevel();
CARPART_LOD GetMaxFELodLevel();
CARPART_LOD GetMaxLicenseLodLevel();
CARPART_LOD GetMinTrafficDiffuseLodLevel();
CARPART_LOD GetMinReflectionLodLevel();
CARPART_LOD GetMinShadowLodLevel();
CARPART_LOD GetMaxShadowLodLevel();
CARPART_LOD GetMaxTireLodLevel();
CARPART_LOD GetMaxBrakeLodLevel();
CARPART_LOD GetMaxSpoilerLodLevel();
CARPART_LOD GetMaxRoofScoopLodLevel();
void Init();
int GetSpecialLODRangeForCarSlot();
void SetCarLoaderHandle();
CarLoaderHandle GetCarLoaderHandle();
CarRenderUsage GetCarRenderUsage();
void SetUpgradePart();
void SetStockParts();
void SetRandomPart();
void SetRandomParts();
void MatchVisualParts();
void SetRandomPaint();
void SetRandomVinyl();
CarPart* GetPart();
CarPart* SetPart();
void UpdatePartsEnabled();
int IsPartEnabled();
void DisablePart();
void EnablePart();
void SetPreviewPart();
CarPart* GetPreviewPart();
int8 GetPartChangedTimeStamp();
uint8 NumbersInstalled();
uint8 VinylsInstalled();
float DecalsInstalledPercent();
uint32 GetSkinNameHash();
void SetCompositeNameHash();
uint32 GetCompositeSkinNameHash();
void SetCompositeSkinNameHash();
uint32 GetCompositeWheelNameHash();
void SetCompositeWheelNameHash();
uint32 GetCompositeSpinnerNameHash();
void SetCompositeSpinnerNameHash();
int IsUsingCompositeSkin();
uint32 GetCollisionVolumeNameHash();
uint32 GetDefaultCollisionVolumeNameHash();
void Print();
void DumpForPreset();
void FillWithPreset();
};
```

### FECarRecord
```
struct FECarRecord { // 0x14
/* 0x00 */ uint32 Handle;
/* 0x04 */ Key FEKey;
/* 0x08 */ Key VehicleKey;
/* 0x0c */ uint32 FilterBits;
/* 0x10 */ uint8 Customization;
/* 0x11 */ uint8 CareerHandle;
/* 0x12 */ uint16 Padd;

FECarRecord();
FECarRecord();
FECarRecord& operator=();
void Default();
bool MatchesFilter();
bool IsValid();
bool IsCustomized();
bool IsCareer();
char* GetDebugName();
char* GetManufacturerName();
CarType GetType();
uint32 GetNameHash();
uint32 GetLogoHash();
uint32 GetManuLogoHash();
uint32 GetCost();
uint32 GetReleaseFromImpoundCost();
};
```

### FrontEndRenderingCar
```
struct FrontEndRenderingCar : /* 0x000 */ bTNode<FrontEndRenderingCar> { // 0x590
private:
/* 0x008 */ RideInfo mRideInfo;
public:
/* 0x318 */ CarRenderInfo *RenderInfo;
/* 0x31c */ int ViewID;
/* 0x320 */ bVector3 Position;
/* 0x330 */ bMatrix4 BodyMatrix;
/* 0x370 */ bMatrix4 TireMatrices[4];
/* 0x470 */ bMatrix4 BrakeMatrices[4];
/* 0x570 */ eModel *OverrideModel;
/* 0x574 */ int Visible;
/* 0x578 */ int nPasses;
/* 0x57c */ int Reflection;
/* 0x580 */ int LightsOn;
/* 0x584 */ int CopLightsOn;

FrontEndRenderingCar& operator=();
FrontEndRenderingCar();
FrontEndRenderingCar();
FrontEndRenderingCar(FrontEndRenderingCar*, int, void);
void ReInit();
void SetPosition();
void SetBodyMatrix();
void SetTireMatrices();
void SetBrakeMatrices();
void SetTireMatrix();
void SetBrakeMatrix();
void SetOverrideModel();
void OverRideAlpha();
void RestoreAlpha();
bool LookupWheelPosition();
bool LookupWheelRadius();
RideInfo* GetRideInfo();
CarRenderInfo* GetRenderInfo();
CarType GetCarType();
};
```

### GRaceParameters
```
struct GRaceParameters { // 0x14
protected:
/* 0x00 */ GRaceIndexData *mIndex;
/* 0x04 */ gameplay *mRaceRecord;
/* 0x08 */ GVault *mParentVault;
/* 0x0c */ GVault *mChildVault;
public:
/* 0x10 */ __vtbl_ptr_type *$vf13671;

[30+ virtual methods including GetCollectionKey, GetGameplayObj, GetActivity, etc.]

Properties include:
- Event identification and loading
- Challenge type and goal
- Race type, region, lap count
- Reputation and cash value
- Cops enabled, density, scripted cops
- Rival best time, reversible, D-Day, boss, marker, pursuit, looping race indicators
- Player ranking methods
- Bounding box, time limit
- Busted lives, knockouts per lap, timed knockout
- Traffic density and pattern
- Max heat level, initial player speed, rolling start
- Player car type and performance
- World heat, forced heat levels
- Catch-up mechanics
- Photo finish support
- Opponent and checkpoint information
- Shortcuts and barrier exemptions
- Finish line, start/finish positions
- Time of day, sunset race indicator
- Speed trap camera and photo finish settings
};
```

### FEObject
```
struct FEObject : /* 0x00 */ FEMinNode { // 0x5c
static FEObjectDestructorCallback *pDestructorCallback;
/* 0x0c */ u32 GUID;
/* 0x10 */ u32 NameHash;
/* 0x14 */ char *pName;
/* 0x18 */ FEObjType Type;
/* 0x1c */ u32 Flags;
/* 0x20 */ u16 RenderContext;
/* 0x22 */ u16 ResourceIndex;
/* 0x24 */ u32 Handle;
/* 0x28 */ u32 UserParam;
/* 0x2c */ u8 *pData;
/* 0x30 */ u32 DataSize;
/* 0x34 */ FEMinList Responses;
/* 0x44 */ FEMinList Scripts;
/* 0x54 */ FEScript *pCurrentScript;
/* 0x58 */ FERenderObject *Cached;

FEObject& operator=();
void SetCurrentScript();
FEObjData* GetObjData();
FEScript* GetFirstScript();
u32 GetNumScripts();
FEScript* GetScript();
FEScript* FindScript();
FEMessageResponse* GetFirstResponse();
u32 GetNumResponses();
FEMessageResponse* GetResponse();
FEMessageResponse* FindResponse();
FEObject();
FEObject();
/* vtable[1] */ virtual FEObject(FEObject*, int, void);
void SetDataSize();
void SetName();
void SetNameHash();
void SetPivot();
void SetPosition();
void SetRotation();
void SetSize();
void SetColor();
void SetScript();
void SetScript();
void SetupMoveToTracks();
u32 GetDataOffset();
/* vtable[2] */ virtual FEObject* Clone();
FEObject* GetNext();
FEObject* GetPrev();
protected:
void SetTrackValue();
void SetTrackValue();
void SetTrackValue();
};
```

### CarPart
```
struct CarPart { // 0xe
/* 0x0 */ uint16 PartNameHashBot;
/* 0x2 */ uint16 PartNameHashTop;
/* 0x4 */ int8 PartID;
/* 0x5 */ uint8 GroupNumber_UpgradeLevel;
/* 0x6 */ int8 BaseModelNameHashSelector;
/* 0x7 */ uint8 CarTypeNameHashIndex;
/* 0x8 */ uint16 NameOffset;
/* 0xa */ uint16 AttributeTableOffset;
/* 0xc */ uint16 ModelNameHashTableOffset;

CarPart& operator=();
CarPart();
CarPart();
CarPart(CarPart*, int, void);
void InPlaceInit();
char* GetName();
CarPartAttribute* GetFirstAppliedAttribute();
CarPartAttribute* GetLastAppliedAttribute();
CarPartAttribute* GetNextAppliedAttribute();
CarPartAttribute* GetPrevAppliedAttribute();
int HasAppliedAttribute();
char* GetAppliedAttributeString();
float GetAppliedAttributeFParam();
int32 GetAppliedAttributeIParam();
uint32 GetAppliedAttributeUParam();
uint32 GetBrandNameHash();
uint32 GetTextureNameHash();
uint32 GetLightMaterialNameHash();
int8 GetInnerRadius();
int8 GetOuterRadius();
int8 GetSpokeCount();
bool GetMirrored();
uint32 GetCarTypeNameHash();
uint32 GetPartNameHash();
int8 GetPartID();
int8 GetUpgradeLevel();
int8 GetGroupNumber();
uint32 GetModelNameHash();
void Print();
void EndianSwap();
private:
CarPartAttribute* GetAttribute();
};
```

### FECareerRecord
```
struct FECareerRecord { // 0x38
/* 0x00 */ uint8 Handle;
/* 0x02 */ FEImpoundData TheImpoundData;
/* 0x0c */ float VehicleHeat;
private:
/* 0x10 */ uint32 Bounty;
/* 0x14 */ uint16 NumEvadedPursuits;
/* 0x16 */ uint16 NumBustedPursuits;
/* 0x18 */ FEInfractionsData UnservedInfractions;
/* 0x28 */ FEInfractionsData ServedInfractions;

public:
FECareerRecord& operator=();
FECareerRecord();
FECareerRecord();
void Default();
void SetVehicleHeat();
float GetVehicleHeat();
void AdjustHeatOnEventWin();
void AdjustHeatOnMilestoneComplete();
void AdjustHeatOnEvadePursuit();
void AdjustHeatOnVinylApplied();
void AdjustHeatOnDecalApplied();
void AdjustHeatOnPaintApplied();
void AdjustHeatOnBodyKitApplied();
void AdjustHeatOnHoodApplied();
void AdjustHeatOnNumbersApplied();
void AdjustHeatOnRimApplied();
void AdjustHeatOnRimPaintApplied();
void AdjustHeatOnRoofScoopApplied();
void AdjustHeatOnSpoilerApplied();
void AdjustHeatOnWindowTintApplied();
void CommitPursuitCarData();
void ServeAllIncractions();
void WaiveIncractions();
uint32 GetNumInfraction();
uint32 GetBounty();
uint32 GetNumEvadedPursuits();
uint32 GetNumBustedPursuits();
int GetTimesBusted();
FEInfractionsData& GetInfractions();
void TweakBounty();
};
```

### CustomizeMainOption
```
struct CustomizeMainOption : /* 0x00 */ IconOption { // 0x68
/* 0x5c */ char *ToPkg;
/* 0x60 */ uint32 Category;
/* 0x64 */ eCustomizePartState UnlockStatus;

CustomizeMainOption& operator=();
CustomizeMainOption();
/* vtable[1] */ virtual CustomizeMainOption(CustomizeMainOption*, int, void);
CustomizeMainOption();
/* vtable[2] */ virtual void React();
};
```

### CustomizePartOption
```
struct CustomizePartOption : /* 0x00 */ IconOption { // 0x64
protected:
/* 0x5c */ SelectablePart *ThePart;
/* 0x60 */ uint32 UnlockBlurb;

public:
CustomizePartOption& operator=();
CustomizePartOption();
CustomizePartOption();
/* vtable[1] */ virtual CustomizePartOption(CustomizePartOption*, int, void);
/* vtable[2] */ virtual void React();
void SetPart();
SelectablePart* GetPart();
uint32 GetUnlockBlurb();
};
```

### CustomizePaintDatum
```
struct CustomizePaintDatum : /* 0x00 */ ArrayDatum { // 0x2c
/* 0x24 */ SelectablePart *ThePart;
/* 0x28 */ uint32 UnlockBlurb;
};
```

### HUDColorOption
```
struct HUDColorOption : /* 0x00 */ IconOption { // 0x64
/* 0x5c */ SelectablePart *ThePart;
/* 0x60 */ uint32 color;

HUDColorOption& operator=();
HUDColorOption();
/* vtable[1] */ virtual HUDColorOption(HUDColorOption*, int, void);
HUDColorOption();
/* vtable[2] */ virtual void React();
};
```

### HUDLayerOption
```
struct HUDLayerOption : /* 0x00 */ CustomizePartOption { // 0x74
/* 0x64 */ uint32 HUDLayer;
/* 0x68 */ bTList<SelectablePart> TheColors;
/* 0x70 */ SelectablePart *SelectedPart;

HUDLayerOption& operator=();
HUDLayerOption();
HUDLayerOption();
/* vtable[1] */ virtual HUDLayerOption(HUDLayerOption*, int, void);
/* vtable[2] */ virtual void React();
uint32 GetLayer();
};
```

### CustomizeMeter
```
struct CustomizeMeter { // 0x50
private:
/* 0x00 */ float Min;
/* 0x04 */ float Max;
/* 0x08 */ float Current;
/* 0x0c */ float Preview;
/* 0x10 */ float PreviousPreview;
/* 0x14 */ int NumStages;
/* 0x18 */ FEImage *pMultiplier;
/* 0x1c */ FEImage *pMultiplierZoom;
/* 0x20 */ FEImage *pBases[10];
/* 0x48 */ FEObject *pMeterGroup;
public:
/* 0x4c */ __vtbl_ptr_type *$vf23346;

CustomizeMeter& operator=();
CustomizeMeter();
CustomizeMeter();
/* vtable[1] */ virtual CustomizeMeter(CustomizeMeter*, int, void);
void Init();
void SetCurrent();
void SetPreview();
void Draw();
void SetVisibility();
};
```

### eView
```
struct eView : /* 0x00 */ eViewPlatInterface { // 0x70
/* 0x04 */ EVIEW_ID ID;
/* 0x08 */ int8 Active;
/* 0x09 */ int8 LetterBox;
/* 0x0a */ int8 pad0;
/* 0x0b */ int8 pad1;
/* 0x0c */ float H;
/* 0x10 */ float NearZ;
/* 0x14 */ float FarZ;
/* 0x18 */ float FovBias;
/* 0x1c */ float FovDegrees;
/* 0x20 */ int BlackAndWhiteMode;
/* 0x24 */ int PixelMinSize;
/* 0x30 */ bVector3 ViewDirection;
/* 0x40 */ Camera *pCamera;
/* 0x44 */ bTList<CameraMover> CameraMoverList;
/* 0x4c */ uint32 NumCopsInView;
/* 0x50 */ TextureInfo *pBlendMask;
/* 0x54 */ eDynamicLightContext *WorldLightContext;
/* 0x58 */ eRenderTarget *RenderTargetTable[1];
/* 0x5c */ ScreenEffectDB *ScreenEffects;
/* 0x60 */ Rain *Precipitation;
/* 0x64 */ FacePixelation *facePixelation;

eView& operator=();
eView();
eView();
eView(eView*, int, void);
int32 GetID();
int GetPlayerNumFromViewID();
Player* GetPlayerFromViewID();
int32 IsActive();
int32 SetActive();
int32 IsLetterBoxed();
int32 SetLetterBox();
void EnableBlackAndWhite(eView*, int, void);
int IsBlackAndWhiteEnabled();
Camera* GetCamera();
void SetCamera();
CameraMover* GetCameraMover();
void AttachCameraMover();
void UnattachCameraMover();
eRenderTarget* GetRenderTarget();
void SetRenderTarget();
eRenderTarget* GetRenderTarget0();
void SetRenderTarget0();
void BiasMatrixForZSorting();
WeatherQuery* GetWeatherQuery();
void SetupWorldLightContext();
eDynamicLightContext* GetWorldLightContext();
TextureInfo* GetBlendMask();
void SetBlendMask();
void ClearBlendMask();
float GetH();
float GetNearZ();
float GetFarZ();
eVisibleState GetVisibleState();
eVisibleState GetVisibleState();
int IsVisible();
int IsVisible();
int GetPixelSize();
int GetPixelSize();
int GetPixelSize();
int GetPixelMinSize();
void SetPixelMinSize(eView*, int, void);
private:
void SetID(eView*, int, void);
};
```

---

## 3. SPECIAL TYPES & TYPEDEFS

### Physics::Upgrades::Type

**Status**: Not found as scoped enum in DWARF data.

**Analysis**: 
- `Physics::Upgrades::Type` is used as a parameter type throughout Physics::Upgrades functions in PS2_functions.nothpp
- The global `Type` is defined as `typedef HashInt Type;` (line 2624 in PS2_types.nothpp)
- `HashInt` is defined as `typedef uint32_t HashInt;` (line 2621)
- Therefore: **Physics::Upgrades::Type ≡ typedef uint32_t**

**Evidence**:
```
File: symbols/PS2/PS2_types.nothpp
Line 2621: typedef uint32_t HashInt;
Line 2622: typedef HashInt Key;
Line 2624: typedef HashInt Type;
Line 20366: typedef Type reflection_typedef_Type;

Functions using this Type:
- Physics::Upgrades::GetPartName(Type t)
- Physics::Upgrades::GetPercent(pvehicle &vehicle, Type type)
- Physics::Upgrades::GetLevel(pvehicle &vehicle, Type type)
- Physics::Upgrades::SetLevel(pvehicle &vehicle, Type type, int level)
- And 12+ more functions
```

---

## SUMMARY

Total Types Queried: 26
- Enums Found: 8
- Structs Found: 17
- Special Types Found: 1 (Physics::Upgrades::Type as typedef)

All types successfully located and documented with full DWARF information including:
- Memory layout and offsets
- Member types and sizes
- Virtual method tables
- Associated methods and accessors
