#ifndef __FECUSTOMIZE_HPP__
#define __FECUSTOMIZE_HPP__

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"

#define CC_MAKE_HIWORD(_a) (_a << 16) // :63
#define CC_GET_HIWORD(_a) (_a >> 16)  // :64

enum eCustomizeScreenMessages {
    FEMSG_ADD_TO_CART = 0x91DFDF84,
    FEMSG_ICON_START = 0x5E6EA975,
    FEMSG_EXIT_PURCHASE = 0x1720B124,
    FEMSG_EXIT_NO_PURCHASE = 0x7A318EE0,
    FEMSG_FROM_CART = 0xCF91AACD,
    FEMSG_BACK_OUT = 0x5A928018,
    FEMSG_MAX_OUT_PERFORMANCE = 0x6820E23E,
};

enum eCustomizeCategory {
    CC_TO_CAT_MASK = 0x0000FFFF,
    CC_FROM_CAT_MASK = 0xFFFF0000,
    CC_SCREEN_ID_MAIN = 0x800,
    CC_SCREEN_ID_PARTS = 0x100,
    CC_SCREEN_ID_PERFORMANCE = 0x200,
    CC_SCREEN_ID_VISUAL = 0x300,
    CC_SCREEN_ID_VINYL_TYPES = 0x400,
    CC_SCREEN_ID_DECAL_LOCATION = 0x500,
    CC_SCREEN_ID_DECAL_POSITION = 0x600,
    CC_SCREEN_ID_RIM_BRANDS = 0x700,
    CC_ID_MASK = 0xFF00,
    CC_NO_CATEGORY = 0,
    CC_PARTS = 0x801,
    CC_PERFORMANCE = 0x802,
    CC_VISUAL = 0x803,
    CC_BODY_KIT = 0x101,
    CC_SPOILERS = 0x102,
    CC_RIM_BRANDS = 0x103,
    CC_HOODS = 0x104,
    CC_ROOF_SCOOPS = 0x105,
    CC_ENGINE = 0x201,
    CC_TRANSMISSION = 0x202,
    CC_SUSPENSION = 0x203,
    CC_NITROUS = 0x204,
    CC_TIRES = 0x205,
    CC_BRAKES = 0x206,
    CC_FORCED_INDUCTION = 0x207,
    CC_PAINT = 0x301,
    CC_VINYL_TYPES = 0x302,
    CC_RIM_PAINT = 0x303,
    CC_WINDOW_TINT = 0x304,
    CC_DECAL_LOCATION = 0x305,
    CC_NUMBERS = 0x306,
    CC_CUSTOM_HUD = 0x307,
    CC_VINYL_GROUP_STOCK = 0x401,
    CC_VINYL_GROUP_FLAME = 0x402,
    CC_VINYL_GROUP_TRIBAL = 0x403,
    CC_VINYL_GROUP_STRIPE = 0x404,
    CC_VINYL_GROUP_RACING_FLAG = 0x405,
    CC_VINYL_GROUP_NATIONAL_FLAG = 0x406,
    CC_VINYL_GROUP_BODY = 0x407,
    CC_VINYL_GROUP_UNIQUE = 0x408,
    CC_VINYL_GROUP_CONTEST = 0x409,
    CC_RIM_BRAND_STOCK = 0x701,
    CC_RIM_BRAND_5_ZIGEN = 0x702,
    CC_RIM_BRAND_ADR = 0x703,
    CC_RIM_BRAND_BBS = 0x704,
    CC_RIM_BRAND_ENKEI = 0x705,
    CC_RIM_BRAND_KONIG = 0x706,
    CC_RIM_BRAND_LOWENHART = 0x707,
    CC_RIM_BRAND_RACING_HART = 0x708,
    CC_RIM_BRAND_OZ = 0x709,
    CC_RIM_BRAND_VOLK = 0x70A,
    CC_RIM_BRAND_ROJA = 0x70B,
    CC_DECAL_WINDSHIELD = 0x501,
    CC_DECAL_REAR_WINDOW = 0x502,
    CC_DECAL_LEFT_DOOR = 0x503,
    CC_DECAL_RIGHT_DOOR = 0x504,
    CC_DECAL_LEFT_QP = 0x505,
    CC_DECAL_RIGHT_QP = 0x506,
    CC_DECAL_SLOT_1 = 0x601,
    CC_DECAL_SLOT_2 = 0x602,
    CC_DECAL_SLOT_3 = 0x603,
    CC_DECAL_SLOT_4 = 0x604,
    CC_DECAL_SLOT_5 = 0x605,
    CC_DECAL_SLOT_6 = 0x606,
};

enum ePerformancePkgLevels {
    PPL_STOCK = 0,
    PPL_LEVEL_1 = 1,
    PPL_LEVEL_2 = 2,
    PPL_LEVEL_3 = 3,
    PPL_LEVEL_4 = 4,
    PPL_LEVEL_5 = 5,
    PPL_LEVEL_6 = 6,
    PPL_LEVEL_JUNKMAN = 7,
};

// Decl: 82
enum eCustomizeEntryPoint {
    CEP_GAMEPLAY = 0,
    CEP_MAIN_MENU = 1,
    CEP_ONLINE_MENU = 2,
};

enum eCustomizePartState {
    CPS_AVAILABLE = 1,
    CPS_LOCKED = 2,
    CPS_NEW = 3,
    CPS_INSTALLED = 16,
    CPS_IN_CART = 32,
    CPS_GAME_STATE_MASK = 0xF,
    CPS_PLAYER_STATE_MASK = 0xF0,
};

// Decl: 106
enum eCustomizeCartTotals {
    CCT_PART_PRICES = 0,
    CCT_TRADE_IN = 1,
    CCT_TOTAL = 2,
};

// total size: 0x2C
class SelectablePart : public bTNode<SelectablePart> {
  public:
    SelectablePart(SelectablePart *part)
        : ThePart(part->GetPart()),                 //
          CarSlotID(part->GetSlotID()),             //
          UpgradeLevel(part->GetUpgradeLevel()),    //
          PhysicsType(part->GetPhysicsType()),      //
          PerformancePkg(part->IsPerformancePkg()), //
          PartState(part->GetPartState()),          //
          Price(part->GetPrice()),                  //
          JunkmanPart(part->IsJunkmanPart()) {}

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

// TODO where go?
extern const float gTradeInFactor; // size: 0x4

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
        return (TradeIn != nullptr) ? static_cast<int>(TradeIn->GetPrice() * gTradeInFactor) : 0;
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
