#ifndef __FECUSTOMIZE_HPP__
#define __FECUSTOMIZE_HPP__

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"

#define CC_MAKE_HIWORD(_a) (_a << 16) // :63
#define CC_GET_HIWORD(_a) (_a >> 16)  // :64

typedef enum {
    FEMSG_ADD_TO_CART = -0x6e20207c,
    FEMSG_ICON_START = 0x5e6ea975,
    FEMSG_EXIT_PURCHASE = 0x1720b124,
    FEMSG_EXIT_NO_PURCHASE = 0x7a318ee0,
    FEMSG_FROM_CART = -0x306e5533,
    FEMSG_BACK_OUT = 0x5a928018,
    FEMSG_MAX_OUT_PERFORMANCE = 0x6820e23e
} eCustomizeScreenMessages;

typedef enum {
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
} eCustomizeCategory;

typedef enum {
    PPL_STOCK = 0,
    PPL_LEVEL_1 = 1,
    PPL_LEVEL_2 = 2,
    PPL_LEVEL_3 = 3,
    PPL_LEVEL_4 = 4,
    PPL_LEVEL_5 = 5,
    PPL_LEVEL_6 = 6,
    PPL_LEVEL_JUNKMAN = 7
} ePerformancePkgLevels;

// File: speed/indep/src/frontend/menuscreens/customize/FECustomize.hpp
// Decl: speed/indep/src/frontend/menuscreens/customize/FECustomize.hpp:82
typedef enum { CEP_GAMEPLAY = 0, CEP_MAIN_MENU = 1, CEP_ONLINE_MENU = 2 } eCustomizeEntryPoint;

typedef enum {
    CPS_AVAILABLE = 1,
    CPS_LOCKED = 2,
    CPS_NEW = 3,
    CPS_INSTALLED = 16,
    CPS_IN_CART = 32,
    CPS_GAME_STATE_MASK = 15,
    CPS_PLAYER_STATE_MASK = 240
} eCustomizePartState;

// Decl: speed/indep/src/frontend/menuscreens/customize/FECustomize.hpp:106

typedef enum { CCT_PART_PRICES = 0, CCT_TRADE_IN = 1, CCT_TOTAL = 2 } eCustomizeCartTotals;

// total size: 0x2C
class SelectablePart : public bTNode<SelectablePart> {
  public:
    SelectablePart(SelectablePart *part)
        : ThePart(part->ThePart),               //
          CarSlotID(part->CarSlotID),           //
          UpgradeLevel(part->UpgradeLevel),     //
          PhysicsType(part->PhysicsType),       //
          PerformancePkg(part->PerformancePkg), //
          PartState(part->PartState),           //
          Price(part->Price),                   //
          JunkmanPart(part->JunkmanPart) {}

    SelectablePart(CarPart *part, int slot_id, uint32 lvl, Physics::Upgrades::Type phys_type, bool is_perf, eCustomizePartState state, int price,
                   bool junkman)
        : ThePart(part),           //
          CarSlotID(slot_id),      //
          UpgradeLevel(lvl),       //
          PhysicsType(phys_type),  //
          PerformancePkg(is_perf), //
          PartState(state),        //
          Price(price),            //
          JunkmanPart(junkman) {}

    virtual ~SelectablePart() {}

    CarPart *GetPart() {
        return ThePart;
    }
    int GetSlotID() {
        return CarSlotID;
    }
    uint32 GetUpgradeLevel() {
        return UpgradeLevel;
    }
    Physics::Upgrades::Type GetPhysicsType() {
        return PhysicsType;
    }
    bool IsPerformancePkg() {
        return PerformancePkg;
    }
    eCustomizePartState GetPartState() {
        return PartState;
    }
    int GetPrice() {
        return Price;
    }
    bool IsJunkmanPart() {
        return JunkmanPart;
    }

    void SetSlotID(uint32 id) {
        CarSlotID = static_cast<int>(id);
    }

    bool IsAvailable() {
        return (PartState & CPS_GAME_STATE_MASK) == CPS_AVAILABLE;
    }
    bool IsLocked() {
        return (PartState & CPS_GAME_STATE_MASK) == CPS_LOCKED;
    }
    bool IsNew() {
        return (PartState & CPS_GAME_STATE_MASK) == CPS_NEW;
    }
    bool IsInstalled() {
        return (PartState & CPS_PLAYER_STATE_MASK) == CPS_INSTALLED;
    }
    bool IsInCart() {
        return (PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART;
    }
    bool IsInstalledX() {
        return (PartState & CPS_INSTALLED) != 0;
    }
    bool IsInCartX() {
        return (PartState & CPS_IN_CART) != 0;
    }

    void SetPartState(uint32 state) {
        PartState = static_cast<eCustomizePartState>(state);
    }
    void SetInCart() {
        PartState = static_cast<eCustomizePartState>((PartState & CPS_GAME_STATE_MASK) | CPS_IN_CART);
    }
    void SetInCartPreserve() {
        PartState = static_cast<eCustomizePartState>(PartState | CPS_IN_CART);
    }
    void SetInstalled() {
        PartState = static_cast<eCustomizePartState>((PartState & CPS_GAME_STATE_MASK) | CPS_INSTALLED);
    }
    void UnSetInCart() {
        PartState = static_cast<eCustomizePartState>(PartState & CPS_GAME_STATE_MASK);
    }
    void UnSetInCartPreserve() {
        PartState = static_cast<eCustomizePartState>(PartState & ~CPS_IN_CART);
    }
    void SetPrice(int price) {
        Price = price;
    }

  protected:
    CarPart *ThePart;                    // offset 0x8, size 0x4
    int CarSlotID;                       // offset 0xC, size 0x4
    uint32 UpgradeLevel;                 // offset 0x10, size 0x4
    Physics::Upgrades::Type PhysicsType; // offset 0x14, size 0x4
    bool PerformancePkg;                 // offset 0x18, size 0x4
    eCustomizePartState PartState;       // offset 0x1C, size 0x4
    int Price;                           // offset 0x20, size 0x4
    bool JunkmanPart;                    // offset 0x24, size 0x4
};

// total size: 0x18
class ShoppingCartItem : public bTNode<ShoppingCartItem> {
  public:
    ShoppingCartItem(SelectablePart *to_buy, SelectablePart *trade_in)
        : ToBuy(to_buy),     //
          TradeIn(trade_in), //
          bActive(true) {}

    virtual ~ShoppingCartItem() {
        delete ToBuy;
        delete TradeIn;
    }

    SelectablePart *GetBuyingPart() {
        return ToBuy;
    }
    SelectablePart *GetTradeInPart() {
        return TradeIn;
    }
    int GetPartPrice() {
        return ToBuy->GetPrice();
    }
    int GetTradeInPrice() {
        return TradeIn ? TradeIn->GetPrice() : 0;
    }
    void ToggleActive() {
        bActive = !bActive;
    }
    bool IsActive() {
        return bActive;
    }

    SelectablePart *ToBuy;   // offset 0x8, size 0x4
    SelectablePart *TradeIn; // offset 0xC, size 0x4
    bool bActive;            // offset 0x10, size 0x1
};

extern eCustomizeEntryPoint g_TheCustomizeEntryPoint;
extern FECarRecord *g_pCustomizeCarRecordToUse;

void BeginCarCustomize(eCustomizeEntryPoint entry_point, FECarRecord *theCustomCar);
bool CustomizeIsInBackRoom();
void CustomizeSetInBackRoom(bool b);
bool CustomizeIsInPerformance();
void CustomizeSetInPerformance(bool b);
bool CustomizeIsInParts();
void CustomizeSetInParts(bool b);

#endif
