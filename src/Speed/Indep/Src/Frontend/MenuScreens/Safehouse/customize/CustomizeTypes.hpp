// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_CUSTOMIZETYPES_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_CUSTOMIZETYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
extern cFEng *cFEng_mInstance;
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include <types.h>

struct FEImage;
struct FEObject;

struct CarPart;

enum eCustomizePartState {
    CPS_AVAILABLE = 1,
    CPS_LOCKED = 2,
    CPS_NEW = 3,
    CPS_INSTALLED = 16,
    CPS_IN_CART = 32,
    CPS_GAME_STATE_MASK = 15,
    CPS_PLAYER_STATE_MASK = 240,
};

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
    CC_DECAL_SLOT_6 = 1542,
};

enum eCustomizeEntryPoint {
    CEP_GAMEPLAY = 0,
    CEP_MAIN_MENU = 1,
    CEP_ONLINE_MENU = 2,
};

enum eCustomizeCartTotals {
    CCT_PART_PRICES = 0,
    CCT_TRADE_IN = 1,
    CCT_TOTAL = 2,
};

enum ePerformanceRatingType {
    PRT_TOP_SPEED = 0,
    PRT_HANDLING = 1,
    PRT_ACCELERATION = 2,
};

enum eUnlockFilters {
    UNLOCK_QUICK_RACE = 1,
    UNLOCK_CAREER_MODE = 2,
    UNLOCK_ONLINE = 4,
    UNLOCK_BACKROOM = 10,
};

// total size: 0x50
struct CustomizeMeter {
    CustomizeMeter();

    virtual ~CustomizeMeter() {}

    void Init(const char *pkg_name, const char *name, float min, float max, float current, float preview);
    void SetCurrent(float current);
    void SetPreview(float preview);
    void Draw();
    void SetVisibility(bool b);

    float Min;                      // offset 0x0, size 0x4
    float Max;                      // offset 0x4, size 0x4
    float Current;                  // offset 0x8, size 0x4
    float Preview;                  // offset 0xC, size 0x4
    float PreviousPreview;          // offset 0x10, size 0x4
    int NumStages;                  // offset 0x14, size 0x4
    FEImage *pMultiplier;           // offset 0x18, size 0x4
    FEImage *pMultiplierZoom;       // offset 0x1C, size 0x4
    FEImage *pBases[10];            // offset 0x20, size 0x28
    FEObject *pMeterGroup;          // offset 0x48, size 0x4
    // vtable at 0x4C
};

// total size: 0x2C
struct SelectablePart : public bTNode<SelectablePart> {
    static void *operator new(size_t s) { return ::operator new[](s); }
    static void operator delete(void *p) { ::operator delete[](p); }

    SelectablePart(SelectablePart *part)
        : ThePart(part->ThePart) //
        , CarSlotID(part->CarSlotID) //
        , UpgradeLevel(part->UpgradeLevel) //
        , PhysicsType(part->PhysicsType) //
        , PerformancePkg(part->PerformancePkg) //
        , PartState(part->PartState) //
        , Price(part->Price) //
        , JunkmanPart(part->JunkmanPart) {}

    SelectablePart(CarPart *part, int slot_id, unsigned int lvl, GRace::Type phys_type, bool is_perf, eCustomizePartState state, int price, bool junkman)
        : ThePart(part) //
        , CarSlotID(slot_id) //
        , UpgradeLevel(lvl) //
        , PhysicsType(phys_type) //
        , PerformancePkg(is_perf) //
        , PartState(state) //
        , Price(price) //
        , JunkmanPart(junkman) {}

    virtual ~SelectablePart() {}

    CarPart *GetPart() { return ThePart; }
    int GetSlotID() { return CarSlotID; }
    unsigned int GetUpgradeLevel() { return UpgradeLevel; }
    GRace::Type GetPhysicsType() { return PhysicsType; }
    int IsPerformancePkg() { return PerformancePkg; }
    eCustomizePartState GetPartState() { return PartState; }
    int GetPrice() { return Price; }
    int IsJunkmanPart() { return JunkmanPart; }

    void SetSlotID(unsigned int id) { CarSlotID = static_cast<int>(id); }

    bool IsAvailable() { return (PartState & CPS_GAME_STATE_MASK) == CPS_AVAILABLE; }
    bool IsLocked() { return (PartState & CPS_GAME_STATE_MASK) == CPS_LOCKED; }
    bool IsNew() { return (PartState & CPS_GAME_STATE_MASK) == CPS_NEW; }
    bool IsInstalled() { return (PartState & CPS_PLAYER_STATE_MASK) == CPS_INSTALLED; }
    bool IsInCart() { return (PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART; }
    bool IsInstalledX() { return (PartState & CPS_INSTALLED) != 0; }
    bool IsInCartX() { return (PartState & CPS_IN_CART) != 0; }

    void SetPartState(unsigned int state) { PartState = static_cast<eCustomizePartState>(state); }
    void SetInCart() { PartState = static_cast<eCustomizePartState>((PartState & CPS_GAME_STATE_MASK) | CPS_IN_CART); }
    void SetInCartPreserve() { PartState = static_cast<eCustomizePartState>(PartState | CPS_IN_CART); }
    void SetInstalled() { PartState = static_cast<eCustomizePartState>((PartState & CPS_GAME_STATE_MASK) | CPS_INSTALLED); }
    void UnSetInCart() { PartState = static_cast<eCustomizePartState>(PartState & CPS_GAME_STATE_MASK); }
    void UnSetInCartPreserve() { PartState = static_cast<eCustomizePartState>(PartState & ~CPS_IN_CART); }
    void SetPrice(int price) { Price = price; }

    CarPart *ThePart;               // offset 0x8, size 0x4
    int CarSlotID;                  // offset 0xC, size 0x4
    unsigned int UpgradeLevel;      // offset 0x10, size 0x4
    GRace::Type PhysicsType;          // offset 0x14, size 0x4
    int PerformancePkg;             // offset 0x18, size 0x4
    eCustomizePartState PartState;  // offset 0x1C, size 0x4
    int Price;                      // offset 0x20, size 0x4
    int JunkmanPart;                // offset 0x24, size 0x4
    // vtable at 0x28
};

// total size: 0x18
struct ShoppingCartItem : public bTNode<ShoppingCartItem> {
    static void *operator new(size_t s) { return ::operator new[](s); }
    static void operator delete(void *p) { ::operator delete[](p); }

    ShoppingCartItem(SelectablePart *to_buy, SelectablePart *trade_in)
        : ToBuy(to_buy) //
        , TradeIn(trade_in) //
        , bActive(true) {}

    virtual ~ShoppingCartItem() {
        delete ToBuy;
        delete TradeIn;
    }

    SelectablePart *GetBuyingPart() { return ToBuy; }
    SelectablePart *GetTradeInPart() { return TradeIn; }
    int GetPartPrice() { return ToBuy->GetPrice(); }
    int GetTradeInPrice() { return TradeIn ? TradeIn->GetPrice() : 0; }
    void ToggleActive() { bActive = !bActive; }
    bool IsActive() { return bActive; }

    SelectablePart *ToBuy;   // offset 0x8, size 0x4
    SelectablePart *TradeIn; // offset 0xC, size 0x4
    bool bActive;            // offset 0x10, size 0x1
    // vtable at 0x14
};

// total size: 0x64
struct CustomizePartOption : public IconOption {
    CustomizePartOption(SelectablePart *part, unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash, unsigned int unlock_blurb)
        : IconOption(tex_hash, name_hash, desc_hash) //
        , ThePart(part) //
        , UnlockBlurb(unlock_blurb) {}

    ~CustomizePartOption() override {
        delete ThePart;
    }

    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {}

    void SetPart(SelectablePart *part) { ThePart = part; }
    SelectablePart *GetPart() { return ThePart; }
    unsigned int GetUnlockBlurb() { return UnlockBlurb; }

    SelectablePart *ThePart;   // offset 0x5C, size 0x4
    unsigned int UnlockBlurb;  // offset 0x60, size 0x4
};

// total size: 0x68
struct CustomizeMainOption : public IconOption {
    CustomizeMainOption(const char *to_pkg, unsigned int tex_hash, unsigned int name_hash, unsigned int to_cat, unsigned int from_cat)
        : IconOption(tex_hash, name_hash, 0) //
        , ToPkg(to_pkg) {
        UnlockStatus = CPS_AVAILABLE;
        Category = to_cat | (from_cat << 16);
    }

    ~CustomizeMainOption() override {}

    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {
        if (data == 0xc407210) {
            cFEng_mInstance->QueuePackageSwitch(ToPkg, Category, 0, false);
        }
    }

    virtual bool IsStockOption() { return false; }

    const char *ToPkg;                     // offset 0x5C, size 0x4
    unsigned int Category;                 // offset 0x60, size 0x4
    eCustomizePartState UnlockStatus;      // offset 0x64, size 0x4
};

// total size: 0x6C
struct SetStockPartOption : public CustomizeMainOption {
    SetStockPartOption(SelectablePart *part, unsigned int icon, unsigned int to_cat)
        : CustomizeMainOption("", icon, 0x60a662f5, to_cat, to_cat) //
        , ThePart(part) {
        SetReactImmediately(true);
    }

    ~SetStockPartOption() override {
        delete ThePart;
    }

    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override;
    bool IsStockOption() override { return true; }

    SelectablePart *ThePart; // offset 0x68, size 0x4
};

// total size: 0x74
struct HUDLayerOption : public CustomizePartOption {
    HUDLayerOption(unsigned int layer, unsigned int icon_hash, unsigned int name_hash);
    ~HUDLayerOption() override {}

    void React(const char *parent_pkg, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {}

    unsigned int GetLayer() { return HUDLayer; }

    unsigned int HUDLayer;               // offset 0x64, size 0x4
    bTList<SelectablePart> TheColors;    // offset 0x68, size 0x8
    SelectablePart *SelectedPart;        // offset 0x70, size 0x4
};

// total size: 0x64
struct HUDColorOption : public IconOption {
    HUDColorOption(SelectablePart *part)
        : IconOption(0, 0, 0) //
        , ThePart(part) {}

    ~HUDColorOption() override {}

    void React(const char *parent_pkg, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {}

    SelectablePart *ThePart; // offset 0x5C, size 0x4
    unsigned int color;      // offset 0x60, size 0x4
};

// total size: 0x64
struct CustomizationScreenHelper {
    CustomizationScreenHelper(const char *pkg_name);

    virtual ~CustomizationScreenHelper() {}

    const char *GetPackageName() { return pPackageName; }
    void SetTitleHash(unsigned int hash) { TitleHash = hash; }
    unsigned int GetTitleHash() { return TitleHash; }
    void SetInitComplete(bool b) { bInitComplete = b; }
    bool IsInitComplete() { return bInitComplete; }
    void PlayLocked() {}
    void PlayInCart() {}
    void PlayInstalled() {}
    void DrawMeters() { HeatMeter.Draw(); }
    void SetHeatValue(float f) { HeatMeter.SetCurrent(f); }
    void SetHeatPreview(float f) { HeatMeter.SetPreview(f); }

    void DrawTitle();
    void SetCareerStatusIcon(eCustomizePartState state);
    void SetPlayerCarStatusIcon(eCustomizePartState state);
    void SetCashVisibility(bool visible);
    void SetUnlockOverlayState(bool show, unsigned int blurb_hash);
    void SetCareerStuff(SelectablePart *part, unsigned int cat, unsigned int tradeInValue);
    void SetPartStatus(SelectablePart *part, unsigned int unlock_blurb, int part_num, int max_parts);
    void FlashStatusIcon(eCustomizePartState state, bool play_sound);

    const char *pPackageName;       // offset 0x0, size 0x4
    unsigned int TitleHash;         // offset 0x4, size 0x4
    bool bUnlockOverlayShowing;     // offset 0x8, size 0x1
    bool bInitComplete;             // offset 0xC, size 0x1
    CustomizeMeter HeatMeter;       // offset 0x10, size 0x50
    // vtable at 0x60
};

#endif
