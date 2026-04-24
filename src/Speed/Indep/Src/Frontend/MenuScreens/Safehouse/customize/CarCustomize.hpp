// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_CARCUSTOMIZE_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_CARCUSTOMIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconPanel.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRBrief.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

struct FEImage;
struct FEString;
struct CarPart;
struct HUDColorOption;

// total size: 0x1CC
struct CustomizeCategoryScreen : public IconScrollerMenu {
    CustomizeCategoryScreen(ScreenConstructorData *sd);
    ~CustomizeCategoryScreen() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    void RefreshHeader() override;
    void Setup() override {}

    int AddCustomOption(const char *to_pkg, unsigned int tex_hash, unsigned int name_hash, unsigned int to_cat);

    bool bBackingOut;              // offset 0x16C, size 0x1
    const char *BackToPkg;         // offset 0x170, size 0x4
    unsigned int Category;         // offset 0x174, size 0x4
    unsigned int FromCategory;     // offset 0x178, size 0x4
    CustomizeMeter HeatMeter;      // offset 0x17C, size 0x50
};

// total size: 0x1D4
struct CustomizeMain : public CustomizeCategoryScreen {
    CustomizeMain(ScreenConstructorData *sd);
    ~CustomizeMain() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader() override;
    void Setup() override;

    void SwitchRooms();
    void SetScreenNames();
    void SetTitle(bool isInBackroom);
    void BuildOptionsList();

    int iPerfIndex;      // offset 0x1CC, size 0x4
    int invalidMarkers;  // offset 0x1D0, size 0x4
};

// total size: 0x1D8
struct CustomizeSub : public CustomizeCategoryScreen {
    CustomizeSub(ScreenConstructorData *sd);
    ~CustomizeSub() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader() override;
    void Setup() override;

    void RemoveCarPart();
    void AddRemovalPart(unsigned int slot_id);
    CustomizeMainOption *FindInCartOption();
    void SetupParts();
    void SetupPerformance();
    void SetupVisual();
    int GetRimBrandIndex(unsigned int brand);
    void SetupRimBrands();
    int GetVinylGroupIndex(int group);
    void SetupVinylGroups();
    void SetupDecalLocations();
    void SetupDecalPositions();

    int InstalledPartOptionIndex;  // offset 0x1CC, size 0x4
    int InCartPartOptionIndex;    // offset 0x1D0, size 0x4
    unsigned int TitleHash;       // offset 0x1D4, size 0x4
};

// total size: 0x1E4
struct CustomizationScreen : public IconScrollerMenu {
    CustomizationScreen(ScreenConstructorData *sd);
    ~CustomizationScreen() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader() override;

    void AddPartOption(SelectablePart *part, unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash, unsigned int unlock_hash, bool locked);
    CustomizePartOption *GetSelectedOption() { return static_cast<CustomizePartOption *>(Options.GetCurrentOption()); }
    virtual SelectablePart *GetSelectedPart() { return GetSelectedOption()->GetPart(); }
    virtual SelectablePart *FindInCartPart();
    virtual CustomizePartOption *FindMatchingOption(SelectablePart *to_find);

    void SetCareerStatusIcon(eCustomizePartState state) { DisplayHelper.SetCareerStatusIcon(state); }
    void SetPlayerCarStatusIcon(eCustomizePartState state) { DisplayHelper.SetPlayerCarStatusIcon(state); }
    void SetTitleHash(unsigned int title_hash) { DisplayHelper.SetTitleHash(title_hash); }
    unsigned int GetCategory() { return Category; }
    unsigned int GetFromCategory() { return FromCategory; }
    void SetCashVisibility(bool visible) { DisplayHelper.SetCashVisibility(visible); }
    void SetUnlockOverlayState(bool show, unsigned int blurb_hash) { DisplayHelper.SetUnlockOverlayState(show, blurb_hash); }
    void PlayLocked() { DisplayHelper.PlayLocked(); }
    void PlayInCart() { DisplayHelper.PlayInCart(); }
    void PlayInstalled() { DisplayHelper.PlayInstalled(); }

    unsigned int Category;                            // offset 0x16C, size 0x4
    unsigned int FromCategory;                        // offset 0x170, size 0x4
    CustomizePartOption *pReplacingOption;             // offset 0x174, size 0x4
    CustomizationScreenHelper DisplayHelper;           // offset 0x178, size 0x64
    bool bNeedsRefresh;                               // offset 0x1DC, size 0x1
    Timer ScrollTime;                                 // offset 0x1E0, size 0x4
};

// total size: 0x1F4
struct CustomizeParts : public CustomizationScreen {
    static void TexturePackLoadedCallbackAccessor(unsigned int parts_screen) {
        reinterpret_cast<CustomizeParts *>(parts_screen)->TexturePackLoadedCallback();
    }
    static void TextureLoadedCallbackAccessor(unsigned int parts_screen) {
        reinterpret_cast<CustomizeParts *>(parts_screen)->TextureLoadedCallback();
    }

    CustomizeParts(ScreenConstructorData *sd);
    ~CustomizeParts() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void RefreshHeader() override;

    void LoadHudTextures();
    void LoadNextHudTexturePack();
    void TexturePackLoadedCallback();
    void TextureLoadedCallback();
    void ShowHudObjects();
    void SetHUDTextures();
    void SetHUDColors();

    static bool TexturePackLoaded; // address: 0x80439228

    int PacksLoadedCount;      // offset 0x1E4, size 0x4
    int TexturesLoadedCount;   // offset 0x1E8, size 0x4
    int TachRPM;               // offset 0x1EC, size 0x4
    bool bTexturesNeedUnload;  // offset 0x1F0, size 0x1
};

// total size: 0x324
struct CustomizePaint : public CustomizationScreen {
    CustomizePaint(ScreenConstructorData *sd);
    ~CustomizePaint() override;

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    SelectablePart *FindInCartPart() override;
    CustomizePartOption *FindMatchingOption(SelectablePart *to_find) override;
    SelectablePart *GetSelectedPart() override;

    unsigned int GetUnlockBlurb() { return 0; }

    void AddVinylAndColorsToCart();
    void ScrollFilters(eScrollDir dir);
    void Setup() override;
    void SetupBasePaint();
    void SetupRimPaint();
    void SetupVinylColor();
    unsigned int CalcBrandHash(CarPart *part);
    void BuildSwatchList(unsigned int slot_id);
    void RefreshHeader() override;

    int TheFilter;                    // offset 0x1E4, size 0x4
    int SelectedIndex[3];             // offset 0x1E8, size 0xC
    CustomizePartOption MatchingPaint; // offset 0x1F4, size 0x64
    ArrayScroller ThePaints;          // offset 0x258, size 0xBC
    SelectablePart *VinylColors[3];   // offset 0x314, size 0xC
    int NumRemapColors;               // offset 0x320, size 0x4
};

// total size: 0x2C8
struct CustomizePerformance : public CustomizationScreen {
    CustomizePerformance(ScreenConstructorData *sd);
    ~CustomizePerformance() override {}

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader() override;
    void Setup() override;

    unsigned int GetPerfPkgDesc(Physics::Upgrades::Type type, int level, int line, bool turbo);
    unsigned int GetPerfPkgBrand(Physics::Upgrades::Type type, int level, int line);

    FEString *DescLines[3];              // offset 0x1E4, size 0xC
    FEImage *DescBullets[3];             // offset 0x1F0, size 0xC
    TwoStageSlider AccelSlider;          // offset 0x1FC, size 0x44
    TwoStageSlider HandlingSlider;       // offset 0x240, size 0x44
    TwoStageSlider TopSpeedSlider;       // offset 0x284, size 0x44
};

// total size: 0xB8
struct CustomizeNumbers : public MenuScreen {
    CustomizeNumbers(ScreenConstructorData *sd);
    ~CustomizeNumbers() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    unsigned int GetCategory() { return Category; }
    unsigned int GetFromCategory() { return FromCategory; }

    void UnsetShoppingCart();
    void ScrollNumbers(eScrollDir dir);
    void RefreshHeader();
    void Setup();

    static bool bShowcaseOn; // address: 0x80439230

    bTList<SelectablePart> LeftNumberList;  // offset 0x2C, size 0x8
    bTList<SelectablePart> RightNumberList; // offset 0x34, size 0x8
    SelectablePart *TheLeftNumber;         // offset 0x3C, size 0x4
    SelectablePart *TheRightNumber;        // offset 0x40, size 0x4
    unsigned int Category;                 // offset 0x44, size 0x4
    unsigned int FromCategory;             // offset 0x48, size 0x4
    short LeftDisplayValue;                // offset 0x4C, size 0x2
    short RightDisplayValue;               // offset 0x4E, size 0x2
    bool bLeft;                            // offset 0x50, size 0x1
    CustomizationScreenHelper DisplayHelper; // offset 0x54, size 0x64
};

// total size: 0x1F8
struct CustomizeHUDColor : public CustomizationScreen {
    CustomizeHUDColor(ScreenConstructorData *sd);
    ~CustomizeHUDColor() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void RefreshHeader() override;

    void ScrollColors(eScrollDir dir);
    void AddLayerOption(unsigned int layer, unsigned int icon_hash, unsigned int name_hash);
    void SetInitialColors();
    void SetHUDTextures();
    void BuildColorOptions();

    bTList<HUDColorOption> ColorOptions;   // offset 0x1E4, size 0x8
    HUDColorOption *SelectedColor;         // offset 0x1EC, size 0x4
    FEObject *Cursor;                      // offset 0x1F0, size 0x4
    bool bTexturesNeedUnload;              // offset 0x1F4, size 0x1
};

// total size: 0x1E8
struct CustomizeDecals : public CustomizationScreen {
    CustomizeDecals(ScreenConstructorData *sd);
    ~CustomizeDecals() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader() override;
    void Setup() override;

    unsigned int GetSlotIDFromCategory();
    void BuildDecalList(unsigned int selected_name_hash);

    static unsigned int CurrentDecalLocation; // address: 0x8043922C

    int bIsBlack;  // offset 0x1E4, size 0x4
};

// total size: 0x1F8
struct CustomizeSpoiler : public CustomizationScreen {
    CustomizeSpoiler(ScreenConstructorData *sd);
    ~CustomizeSpoiler() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void RefreshHeader() override;

    void BuildPartOptionListFromFilter(CarPart *installed);
    void ScrollFilters(eScrollDir dir);

    int TheFilter;                  // offset 0x1E4, size 0x4
    unsigned int SelectedIndex[4];  // offset 0x1E8, size 0x10
};

// total size: 0x1F0
struct CustomizeRims : public CustomizationScreen {
    CustomizeRims(ScreenConstructorData *sd);
    ~CustomizeRims() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void RefreshHeader() override;

    void ScrollRimSizes(eScrollDir dir);
    void BuildRimsList(int selected_index);
    unsigned int GetCategoryLanguageHash();
    unsigned int GetCategoryBrandHash();

    int InnerRadius;   // offset 0x1E4, size 0x4
    int MinRadius;     // offset 0x1E8, size 0x4
    int MaxRadius;     // offset 0x1EC, size 0x4
};

// total size: 0x60
struct FEShoppingCartItem : public FEStatWidget {
    FEShoppingCartItem(ShoppingCartItem *item)
        : FEStatWidget(true) //
        , TheItem(item) {}
    ~FEShoppingCartItem() override {}

    void SetCheckIcon(FEImage *img) { pCheckIcon = img; }
    void SetTradeInString(FEString *string) { pTradeInPrice = string; }
    ShoppingCartItem *GetItem() { return TheItem; }

    void Show() override;
    void Hide() override;
    void Draw() override;
    void Position() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetCheckScripts();
    void SetActiveScripts();
    void DrawPartName();
    unsigned int GetPerfPkgCatHash(Physics::Upgrades::Type phys_type);
    unsigned int GetPerfPkgLevelHash(int level);
    unsigned int GetCarPartCatHash(unsigned int slot_id);

    FEImage *pCheckIcon;            // offset 0x54, size 0x4
    FEString *pTradeInPrice;        // offset 0x58, size 0x4
    ShoppingCartItem *TheItem;      // offset 0x5C, size 0x4
};
// total size: 0x188
struct CustomizeShoppingCart : public UIWidgetMenu {
    CustomizeShoppingCart(ScreenConstructorData *sd);
    ~CustomizeShoppingCart() override {}

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;

    FEShoppingCartItem *GetCurrentItem() { return nullptr; }

    static void ShowShoppingCart(const char *parent_pkg);
    static void ExitShoppingCart();
    bool IsSlotIDNumberDecal(int slotID);
    void ToggleAllNumberDecals();
    void ToggleChecked();
    bool CanCheckout();
    void SetMarkerBloomScript(unsigned int group, unsigned int bloom, ShoppingCartItem *item);
    void SetMarkerData(int num, ShoppingCartItem *item, int num_markers);
    int GetNumMarkersSpending(unsigned int slot_id);
    void SetMarkerAmounts();
    void RefreshHeader();
    void AddItem(ShoppingCartItem *item);
    void ClearUncheckedItems();
    void UncheckAllItems();
    void SetMarkerImages();

    static const char *pParentPkg; // address: 0x804391C4

    CustomizeMeter HeatMeter;  // offset 0x138, size 0x50
};


#endif
