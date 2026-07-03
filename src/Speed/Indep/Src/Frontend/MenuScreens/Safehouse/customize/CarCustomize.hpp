#ifndef __CARCUSTOMIZE_HPP__
#define __CARCUSTOMIZE_HPP__

#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

typedef enum {
    CM_STAGE_1 = 0,
    CM_STAGE_2 = 1,
    CM_STAGE_3 = 2,
    CM_STAGE_4 = 3,
    CM_STAGE_5 = 4,
    NUM_SLIDER_STAGES = 5,
    NUM_STAGE_SEGMENTS = 10
} eCustomizeMeterConstants;

// total size: 0x50
class CustomizeMeter {
  public:
    CustomizeMeter();

    virtual ~CustomizeMeter() {}

    void Init(const char *pkg_name, const char *name, float min, float max, float current, float preview);
    void SetCurrent(float current);
    void SetPreview(float preview);
    void Draw();
    void SetVisibility(bool b);

  private:
    float Min;                // offset 0x0, size 0x4
    float Max;                // offset 0x4, size 0x4
    float Current;            // offset 0x8, size 0x4
    float Preview;            // offset 0xC, size 0x4
    float PreviousPreview;    // offset 0x10, size 0x4
    int NumStages;            // offset 0x14, size 0x4
    FEImage *pMultiplier;     // offset 0x18, size 0x4
    FEImage *pMultiplierZoom; // offset 0x1C, size 0x4
    FEImage *pBases[10];      // offset 0x20, size 0x28
    FEObject *pMeterGroup;    // offset 0x48, size 0x4
};

// total size: 0x60
class FEShoppingCartItem : public FEStatWidget {
  public:
    FEShoppingCartItem(ShoppingCartItem *item) : FEStatWidget(true), TheItem(item) {}
    ~FEShoppingCartItem() override {}

    void Show() override;
    void Hide() override;
    void Draw() override;
    void Position() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;

    void SetCheckScripts();
    void SetActiveScripts();
    void SetCheckIcon(FEImage *img) {
        pCheckIcon = img;
    }
    void SetTradeInString(FEString *string) {
        pTradeInPrice = string;
    }
    ShoppingCartItem *GetItem() {
        return TheItem;
    }

  private:
    void DrawPartName();
    uint32 GetPerfPkgCatHash(Physics::Upgrades::Type phys_type);
    uint32 GetPerfPkgLevelHash(int level);
    uint32 GetCarPartCatHash(uint32 slot_id);

    FEImage *pCheckIcon;       // offset 0x54, size 0x4
    FEString *pTradeInPrice;   // offset 0x58, size 0x4
    ShoppingCartItem *TheItem; // offset 0x5C, size 0x4
};

// total size: 0x188
class CustomizeShoppingCart : public UIWidgetMenu {
  public:
    CustomizeShoppingCart(ScreenConstructorData *sd);
    ~CustomizeShoppingCart() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    static void ShowShoppingCart(const char *parent_pkg);
    static void ExitShoppingCart();
    FEShoppingCartItem *GetCurrentItem() {
        return nullptr;
    }
    void ToggleChecked();
    bool CanCheckout();

  private:
    void SetMarkerBloomScript(unsigned int group, unsigned int bloom, ShoppingCartItem *item);
    void SetMarkerData(int num, ShoppingCartItem *item, int num_markers);
    int GetNumMarkersSpending(unsigned int slot_id);
    void SetMarkerAmounts();
    void RefreshHeader();
    void AddItem(ShoppingCartItem *item);
    void ClearUncheckedItems();
    void UncheckAllItems();
    void SetMarkerImages();
    void Setup() override;
    bool IsSlotIDNumberDecal(int slotID);
    void ToggleAllNumberDecals();

    static const char *pParentPkg; // address: 0x804391C4

    CustomizeMeter HeatMeter; // offset 0x138, size 0x50
};

// total size: 0x68
class CustomizeMainOption : public IconOption {
  public:
    CustomizeMainOption(const char *to_pkg, uint32 tex_hash, uint32 name_hash, uint32 to_cat, uint32 from_cat)
        : IconOption(tex_hash, name_hash, 0), ToPkg(to_pkg) {
        UnlockStatus = CPS_AVAILABLE;
        Category = to_cat | (from_cat << 16);
    }

    ~CustomizeMainOption() override {}

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {
        if (data == 0xc407210) {
            cFEng::Get()->QueuePackageSwitch(ToPkg, Category, 0, false);
        }
    }

    virtual bool IsStockOption() {
        return false;
    }

    const char *ToPkg;                // offset 0x5C, size 0x4
    uint32 Category;                  // offset 0x60, size 0x4
    eCustomizePartState UnlockStatus; // offset 0x64, size 0x4
};

// total size: 0x1CC
class CustomizeCategoryScreen : public IconScrollerMenu {
  public:
    CustomizeCategoryScreen(ScreenConstructorData *sd);
    ~CustomizeCategoryScreen() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  protected:
    void RefreshHeader() override;
    int AddCustomOption(const char *to_pkg, uint32 tex_hash, uint32 name_hash, uint32 to_cat);
    void Setup() override {}

    bool bBackingOut;         // offset 0x16C, size 0x1
    const char *BackToPkg;    // offset 0x170, size 0x4
    uint32 Category;          // offset 0x174, size 0x4
    uint32 FromCategory;      // offset 0x178, size 0x4
    CustomizeMeter HeatMeter; // offset 0x17C, size 0x50
};

// total size: 0x6C
class SetStockPartOption : public CustomizeMainOption {
  public:
    SetStockPartOption(SelectablePart *part, uint32 icon, uint32 to_cat) : CustomizeMainOption("", icon, 0x60a662f5, to_cat, to_cat), ThePart(part) {
        SetReactImmediately(true);
    }

    ~SetStockPartOption() override {
        delete ThePart;
    }

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
    bool IsStockOption() override {
        return true;
    }

    SelectablePart *ThePart; // offset 0x68, size 0x4
};

// total size: 0x1D8
class CustomizeSub : public CustomizeCategoryScreen {
  public:
    CustomizeSub(ScreenConstructorData *sd);
    ~CustomizeSub() override {};

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void RefreshHeader() override;
    void RemoveCarPart();
    void AddRemovalPart(uint32 slot_id);
    CustomizeMainOption *FindInCartOption();
    void Setup() override;
    void SetupParts();
    void SetupPerformance();
    void SetupVisual();
    int GetRimBrandIndex(uint32 brand);
    int GetVinylGroupIndex(int group);
    void SetupRimBrands();
    void SetupVinylGroups();
    void SetupDecalLocations();
    void SetupDecalPositions();

    int InstalledPartOptionIndex; // offset 0x1CC, size 0x4
    int InCartPartOptionIndex;    // offset 0x1D0, size 0x4
    uint32 TitleHash;             // offset 0x1D4, size 0x4
};

// total size: 0x1D4
class CustomizeMain : public CustomizeCategoryScreen {
  public:
    CustomizeMain(ScreenConstructorData *sd);
    ~CustomizeMain() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  protected:
    void SetScreenNames();
    void RefreshHeader() override;
    void Setup() override;
    void BuildOptionsList();

  private:
    void SwitchRooms();
    void SetTitle(bool isInBackroom);

  protected:
    int iPerfIndex;     // offset 0x1CC, size 0x4
    int invalidMarkers; // offset 0x1D0, size 0x4
};

// total size: 0x64
class CustomizePartOption : public IconOption {
  public:
    CustomizePartOption(SelectablePart *part, uint32 tex_hash, uint32 name_hash, uint32 desc_hash, uint32 unlock_blurb)
        : IconOption(tex_hash, name_hash, desc_hash), //
          ThePart(part),                              //
          UnlockBlurb(unlock_blurb) {}

    ~CustomizePartOption() override {
        delete ThePart;
    }

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {}

    void SetPart(SelectablePart *part) {
        ThePart = part;
    }
    SelectablePart *GetPart() {
        return ThePart;
    }
    uint32 GetUnlockBlurb() {
        return UnlockBlurb;
    }

  protected:
    SelectablePart *ThePart; // offset 0x5C, size 0x4
    uint32 UnlockBlurb;      // offset 0x60, size 0x4
};

// total size: 0x64
class CustomizationScreenHelper {
  public:
    CustomizationScreenHelper(const char *pkg_name);
    virtual ~CustomizationScreenHelper() {}

    const char *GetPackageName() {
        return pPackageName;
    }
    void DrawTitle();
    void SetTitleHash(uint32 hash) {
        TitleHash = hash;
    }
    uint32 GetTitleHash() {
        return TitleHash;
    }
    void SetInitComplete(bool b) {
        bInitComplete = b;
    }
    bool IsInitComplete() {
        return bInitComplete;
    }
    void SetCareerStatusIcon(eCustomizePartState state);
    void SetPlayerCarStatusIcon(eCustomizePartState state);
    void SetCashVisibility(bool visible);
    void SetUnlockOverlayState(bool show, uint32 blurb_hash);
    void SetCareerStuff(SelectablePart *part, uint32 cat, uint32 tradeInValue);
    void SetPartStatus(SelectablePart *part, uint32 unlock_blurb, int part_num, int max_parts);
    void FlashStatusIcon(eCustomizePartState state, bool play_sound);
    void PlayLocked() {
        FlashStatusIcon(CPS_LOCKED, true);
    }
    void PlayInCart() {
        FlashStatusIcon(CPS_IN_CART, true);
    }
    void PlayInstalled() {}
    void DrawMeters() {
        HeatMeter.Draw();
    }
    void SetHeatValue(float f) {
        HeatMeter.SetCurrent(f);
    }
    void SetHeatPreview(float f) {
        HeatMeter.SetPreview(f);
    }

  private:
    const char *pPackageName;   // offset 0x0, size 0x4
    uint32 TitleHash;           // offset 0x4, size 0x4
    bool bUnlockOverlayShowing; // offset 0x8, size 0x1
    bool bInitComplete;         // offset 0xC, size 0x1
    CustomizeMeter HeatMeter;   // offset 0x10, size 0x50
};

// total size: 0x1E4
class CustomizationScreen : public IconScrollerMenu {
  public:
    CustomizationScreen(ScreenConstructorData *sd);
    ~CustomizationScreen() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader() override;

  protected:
    void SetCareerStatusIcon(eCustomizePartState state) {
        DisplayHelper.SetCareerStatusIcon(state);
    }
    void SetPlayerCarStatusIcon(eCustomizePartState state) {
        DisplayHelper.SetPlayerCarStatusIcon(state);
    }
    void AddPartOption(SelectablePart *part, uint32 tex_hash, uint32 name_hash, uint32 desc_hash, uint32 unlock_hash, bool locked);
    CustomizePartOption *GetSelectedOption() {
        return static_cast<CustomizePartOption *>(Options.GetCurrentOption());
    }
    virtual SelectablePart *GetSelectedPart() {
        return GetSelectedOption()->GetPart();
    }
    virtual SelectablePart *FindInCartPart();
    void SetTitleHash(uint32 title_hash) {
        DisplayHelper.SetTitleHash(title_hash);
    }
    uint32 GetCategory() {
        return Category;
    }
    uint32 GetFromCategory() {
        return FromCategory;
    }
    void SetCashVisibility(bool visible) {
        DisplayHelper.SetCashVisibility(visible);
    }
    void SetUnlockOverlayState(bool show, unsigned int blurb_hash) {
        DisplayHelper.SetUnlockOverlayState(show, blurb_hash);
    }
    void PlayLocked() {
        DisplayHelper.PlayLocked();
    }
    void PlayInCart() {
        DisplayHelper.PlayInCart();
    }
    void PlayInstalled() {
        DisplayHelper.PlayInstalled();
    }
    virtual CustomizePartOption *FindMatchingOption(SelectablePart *to_find);

    uint32 Category;                         // offset 0x16C, size 0x4
    uint32 FromCategory;                     // offset 0x170, size 0x4
    CustomizePartOption *pReplacingOption;   // offset 0x174, size 0x4
    CustomizationScreenHelper DisplayHelper; // offset 0x178, size 0x64
    bool bNeedsRefresh;                      // offset 0x1DC, size 0x1
    Timer ScrollTime;                        // offset 0x1E0, size 0x4
};

typedef enum {
    CHT_TACH_FILL = 0,
    CHT_TACH_LINES = 1,
    CHT_TURBO_LINES = 2,
    CHT_TACH_NEEDLE = 3,
    CHT_TURBO_NEEDLE = 4,
    CHT_NUM_TEXTURES = 5,
    CHT_DEFAULT_FILL_COLOR = -15501,
    CHT_DEFAULT_NEEDLE_COLOR = -20928,
    CHT_DEFAULT_LINE_COLOR = -1
} eCustomizeHUDTextures;

// total size: 0x1F4
class CustomizeParts : public CustomizationScreen {
  public:
    CustomizeParts(ScreenConstructorData *sd);
    ~CustomizeParts() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    static bool TexturePackLoaded; // address: 0x80439228
  private:
    void RefreshHeader() override;
    void Setup() override;

    void LoadHudTextures();
    void LoadNextHudTexturePack();
    void TexturePackLoadedCallback();
    void TextureLoadedCallback();
    static void TexturePackLoadedCallbackAccessor(uint32 parts_screen) {
        reinterpret_cast<CustomizeParts *>(parts_screen)->TexturePackLoadedCallback();
    }
    static void TextureLoadedCallbackAccessor(uint32 parts_screen) {
        reinterpret_cast<CustomizeParts *>(parts_screen)->TextureLoadedCallback();
    }
    void ShowHudObjects();
    void SetHUDTextures();
    void SetHUDColors();

    int PacksLoadedCount;     // offset 0x1E4, size 0x4
    int TexturesLoadedCount;  // offset 0x1E8, size 0x4
    int TachRPM;              // offset 0x1EC, size 0x4
    bool bTexturesNeedUnload; // offset 0x1F0, size 0x1
};

typedef enum {
    CSF_NO_FILTER = -1,
    CSF_SPORT = 0,
    CSF_TUNER = 1,
    CSF_SPORT_CARBON = 2,
    CSF_TUNER_CARBON = 3,
    CSF_NUM_FILTERS = 4
} eCustomizeSpoilerFilter;

// total size: 0x1F8
class CustomizeSpoiler : public CustomizationScreen {
  public:
    CustomizeSpoiler(ScreenConstructorData *sd);
    ~CustomizeSpoiler() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void RefreshHeader() override;
    void Setup() override;

    void BuildPartOptionListFromFilter(CarPart *installed);
    void ScrollFilters(eScrollDir dir);

    int TheFilter;           // offset 0x1E4, size 0x4
    uint32 SelectedIndex[4]; // offset 0x1E8, size 0x10
};

// total size: 0x74
class HUDLayerOption : public CustomizePartOption {
  public:
    HUDLayerOption(uint32 layer, uint32 icon_hash, uint32 name_hash);
    ~HUDLayerOption() override {}

    void React(const char *parent_pkg, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {}

    uint32 GetLayer() {
        return HUDLayer;
    }

    uint32 HUDLayer;                  // offset 0x64, size 0x4
    bTList<SelectablePart> TheColors; // offset 0x68, size 0x8
    SelectablePart *SelectedPart;     // offset 0x70, size 0x4
};

// total size: 0x64
class HUDColorOption : public IconOption {
  public:
    HUDColorOption(SelectablePart *part)
        : IconOption(0, 0, 0), //
          ThePart(part) {}

    ~HUDColorOption() override {}

    void React(const char *parent_pkg, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {}

    SelectablePart *ThePart; // offset 0x5C, size 0x4
    uint32 color;            // offset 0x60, size 0x4
};

// total size: 0x1F8
class CustomizeHUDColor : public CustomizationScreen {
  public:
    CustomizeHUDColor(ScreenConstructorData *sd);
    ~CustomizeHUDColor() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void ScrollColors(eScrollDir dir);
    void AddLayerOption(uint32 layer, uint32 icon_hash, uint32 name_hash);
    void Setup() override;
    void SetInitialColors();
    void SetHUDTextures();
    void RefreshHeader() override;
    void BuildColorOptions();

    bTList<HUDColorOption> ColorOptions; // offset 0x1E4, size 0x8
    HUDColorOption *SelectedColor;       // offset 0x1EC, size 0x4
    FEObject *Cursor;                    // offset 0x1F0, size 0x4
    bool bTexturesNeedUnload;            // offset 0x1F4, size 0x1
};

// total size: 0x1F0
class CustomizeRims : public CustomizationScreen {
  public:
    CustomizeRims(ScreenConstructorData *sd);
    ~CustomizeRims() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void ScrollRimSizes(eScrollDir dir);
    void Setup() override;
    void BuildRimsList(int selected_index);
    void RefreshHeader() override;
    uint32 GetCategoryLanguageHash();
    uint32 GetCategoryBrandHash();

    int InnerRadius; // offset 0x1E4, size 0x4
    int MinRadius;   // offset 0x1E8, size 0x4
    int MaxRadius;   // offset 0x1EC, size 0x4
};

typedef enum {
    CP_NUM_PAINT_SWATCHES = 80,
    CP_RIM_PAINT_FILTER = 0,
    CP_BASE_PAINT_FILTER_GLOSS = 0,
    CP_BASE_PAINT_FILTER_METALLIC = 1,
    CP_BASE_PAINT_FILTER_PEARL = 2,
    CP_NUM_BASE_PAINT_FILTERS = 3,
    CP_FIRST_BASE_PAINT_FILTERS = 0,
    CP_LAST_BASE_PAINT_FILTERS = 2,
    CP_VINYL_PAINT_LAYER_1 = 0,
    CP_VINYL_PAINT_LAYER_2 = 1,
    CP_VINYL_PAINT_LAYER_3 = 2,
    CP_VINYL_NUM_COLORS = 3
} eCustomizePaintConstants;

// total size: 0x2C
class CustomizePaintDatum : public ArrayDatum {
  public:
    CustomizePaintDatum(SelectablePart *part, uint32 unlock_blurb)
        : ArrayDatum(0xc6afdd7e, 0), //
          ThePart(part),             //
          UnlockBlurb(unlock_blurb) {}

    ~CustomizePaintDatum() override {
        if (ThePart) {
            delete ThePart;
        }
    };

    SelectablePart *ThePart; // offset 0x24, size 0x4
    uint32 UnlockBlurb;      // offset 0x28, size 0x4
};

// total size: 0x324
class CustomizePaint : public CustomizationScreen {
  public:
    CustomizePaint(ScreenConstructorData *sd);
    ~CustomizePaint() override {
        MatchingPaint.SetPart(nullptr);
    };

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    uint32 GetUnlockBlurb() {
        return 0;
    }
    SelectablePart *GetSelectedPart() override {
        return static_cast<CustomizePaintDatum *>(ThePaints.GetCurrentDatum())->ThePart;
    };
    SelectablePart *FindInCartPart() override;
    CustomizePartOption *FindMatchingOption(SelectablePart *to_find) override;

    void AddVinylAndColorsToCart();
    void ScrollFilters(eScrollDir dir);
    void RefreshHeader() override;
    void Setup() override;
    void SetupBasePaint();
    void SetupRimPaint();
    void SetupVinylColor();
    uint32 CalcBrandHash(CarPart *part);
    void BuildSwatchList(uint32 slot_id);

    int TheFilter;                     // offset 0x1E4, size 0x4
    int SelectedIndex[3];              // offset 0x1E8, size 0xC
    CustomizePartOption MatchingPaint; // offset 0x1F4, size 0x64
    ArrayScroller ThePaints;           // offset 0x258, size 0xBC
    SelectablePart *VinylColors[3];    // offset 0x314, size 0xC
    int NumRemapColors;                // offset 0x320, size 0x4
};

// total size: 0x1E8
class CustomizeDecals : public CustomizationScreen {
  public:
    CustomizeDecals(ScreenConstructorData *sd);
    ~CustomizeDecals() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    static uint32 CurrentDecalLocation; // address: 0x8043922C
  private:
    uint32 GetSlotIDFromCategory();
    void RefreshHeader() override;
    void BuildDecalList(uint32 selected_name_hash);
    void Setup() override;

    bool bIsBlack; // offset 0x1E4, size 0x4
};

// total size: 0xB8
class CustomizeNumbers : public MenuScreen {
  public:
    CustomizeNumbers(ScreenConstructorData *sd);
    ~CustomizeNumbers() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    uint32 GetCategory() {
        return Category;
    }
    uint32 GetFromCategory() {
        return FromCategory;
    }

  private:
    void ScrollNumbers(eScrollDir dir);
    void RefreshHeader();
    void Setup();
    void UnsetShoppingCart();

    bTList<SelectablePart> LeftNumberList;   // offset 0x2C, size 0x8
    bTList<SelectablePart> RightNumberList;  // offset 0x34, size 0x8
    SelectablePart *TheLeftNumber;           // offset 0x3C, size 0x4
    SelectablePart *TheRightNumber;          // offset 0x40, size 0x4
    uint32 Category;                         // offset 0x44, size 0x4
    uint32 FromCategory;                     // offset 0x48, size 0x4
    short LeftDisplayValue;                  // offset 0x4C, size 0x2
    short RightDisplayValue;                 // offset 0x4E, size 0x2
    bool bLeft;                              // offset 0x50, size 0x1
    static bool bShowcaseOn;                 // address: 0x80439230
    CustomizationScreenHelper DisplayHelper; // offset 0x54, size 0x64
};

// total size: 0x2C8
class CustomizePerformance : public CustomizationScreen {
  public:
    CustomizePerformance(ScreenConstructorData *sd);
    ~CustomizePerformance() override {}

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  public:
    uint32 GetPerfPkgDesc(Physics::Upgrades::Type type, int level, int line, bool turbo);
    uint32 GetPerfPkgBrand(Physics::Upgrades::Type type, int level, int line);
    void RefreshHeader() override;
    void Setup() override;

    FEString *DescLines[3];        // offset 0x1E4, size 0xC
    FEImage *DescBullets[3];       // offset 0x1F0, size 0xC
    TwoStageSlider AccelSlider;    // offset 0x1FC, size 0x44
    TwoStageSlider HandlingSlider; // offset 0x240, size 0x44
    TwoStageSlider TopSpeedSlider; // offset 0x284, size 0x44
};

bool g_bCustomizeManagerHasControl = false;
bool g_bTestCareerCustomization = false;
char *g_pCustomizeMainPkg = "CustomizeMain.fng";
char *g_pCustomizeSubPkg = "CustomizeCategory.fng";
char *g_pCustomizeSubTopPkg = "CustomizeGenericTop.fng";
char *g_pCustomizePartsPkg = "CustomizeParts.fng";
char *g_pCustomizePerfPkg = "CustomizePerformance.fng";
char *g_pCustomizeDecalsPkg = "Decals.fng";
char *g_pCustomizePaintPkg = "Paint.fng";
char *g_pCustomizeRimsPkg = "Rims.fng";
char *g_pCustomizeHudPkg = "CustomHUD.fng";
char *g_pCustomizeHudColorPkg = "CustomHUDColor.fng";
char *g_pCustomizeSpoilerPkg = "Spoilers.fng";
char *g_pCustomizeShoppingCartPkg = "ShoppingCart.fng";

static uint32 gCarTypeNameHash;

extern eSetRideInfoReasons TopOrFullScreenLoadingReason;
extern bool CustomizeHUDTexPackResources[11];
extern uint32 CustomizeHUDTexTextureResources[11][5];

#endif
