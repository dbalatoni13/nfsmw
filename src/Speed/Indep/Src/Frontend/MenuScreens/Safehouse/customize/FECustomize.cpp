// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"

extern FEObject *FEngFindObject(const char *pkg, unsigned int hash);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetLanguageHash(const char *pkg, unsigned int obj_hash, unsigned int lang_hash);
extern FEImage *FEngFindImage(const char *pkg, unsigned int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern void FEngSetScript(const char *pkg, unsigned int obj_hash, unsigned int script_hash, bool b);
extern int FEngGetScript(const char *pkg, unsigned int obj_hash, unsigned int script_hash);
extern FEString *FEngFindString(const char *pkg, unsigned int hash);

extern int CustomizeIsInBackRoom();
extern int CustomizeIsInParts();
extern int CustomizeIsInPerformance();
extern void CustomizeSetInBackRoom(bool b);

extern const char *g_pCustomizeMainPkg;
extern const char *g_pCustomizeShoppingCartPkg;
extern const char *g_pCustomizeSubPkg;

struct HUDColorOption : public IconOption {
    HUDColorOption(SelectablePart *part)
        : IconOption(0, 0, 0) //
        , ThePart(part) //
        , color(0) {}

    ~HUDColorOption() override {}

    void React(const char *parent_pkg, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {}

    SelectablePart *ThePart;
    unsigned int color;
};

// --- CustomizationScreenHelper ---

void CustomizationScreenHelper::SetCareerStatusIcon(eCustomizePartState state) {
    if (state == CPS_LOCKED) {
        FEngSetVisible(FEngFindObject(pPackageName, 0xcffb7033));
        FEImage *img = FEngFindImage(pPackageName, 0xcffb7033);
        FEngSetTextureHash(img, 0xf0574bb2);
        if (FEngGetScript(pPackageName, 0xcffb7033, 0x5079c8f8)) {
            FEngSetScript(pPackageName, 0xcffb7033, 0x5079c8f8, true);
        }
    } else if (state == CPS_NEW) {
        FEngSetVisible(FEngFindObject(pPackageName, 0xcffb7033));
        FEImage *img = FEngFindImage(pPackageName, 0xcffb7033);
        FEngSetTextureHash(img, 0xcffb7033);
        if (FEngGetScript(pPackageName, 0xcffb7033, 0x280164f)) {
            FEngSetScript(pPackageName, 0xcffb7033, 0x280164f, true);
        }
    } else if (state == CPS_AVAILABLE) {
        FEngSetScript(pPackageName, 0xcffb7033, 0x16a259, true);
    }
}

void CustomizationScreenHelper::SetPlayerCarStatusIcon(eCustomizePartState state) {
    if (state == CPS_INSTALLED) {
        FEngSetVisible(FEngFindObject(pPackageName, 0x6d6ae820));
        FEImage *img = FEngFindImage(pPackageName, 0x6d6ae820);
        FEngSetTextureHash(img, 0xfd47c093);
    } else if (state == CPS_IN_CART) {
        FEngSetVisible(FEngFindObject(pPackageName, 0x6d6ae820));
        FEImage *img = FEngFindImage(pPackageName, 0x6d6ae820);
        FEngSetTextureHash(img, 0x927bb4e8);
    } else {
        FEngSetInvisible(FEngFindObject(pPackageName, 0x6d6ae820));
    }
}

void CustomizationScreenHelper::FlashStatusIcon(eCustomizePartState state, bool flash) {
    if (flash && state == CPS_NEW) {
        FEngSetScript(pPackageName, 0xcffb7033, 0x3e3eee7e, true);
    } else {
        FEngSetScript(pPackageName, 0xcffb7033, 0x14adc15c, true);
    }
}

// --- CustomizeCategoryScreen ---

CustomizeCategoryScreen::CustomizeCategoryScreen(ScreenConstructorData *sd) : IconScrollerMenu(sd) {
    bBackingOut = false;
    BackToPkg = nullptr;
    HeatMeter = CustomizeMeter();
    Category = static_cast<unsigned int>(static_cast<unsigned short>(sd->Arg >> 16));
    FromCategory = static_cast<unsigned int>(static_cast<short>(sd->Arg));
    if (Category != 0 || !CustomizeIsInBackRoom()) {
        GarageMainScreen *inst = GarageMainScreen::GetInstance();
        inst->SetCustomizationCategory(Category);
    }
    float heat = gCarCustomizeManager.GetActualHeat();
    float cart_heat = gCarCustomizeManager.GetCartHeat();
    HeatMeter.Init(PackageFilename, "HEAT_METER_GROUP", 0.0f, 5.0f, heat, cart_heat);
}

CustomizeCategoryScreen::~CustomizeCategoryScreen() {
}

void CustomizeCategoryScreen::RefreshHeader() {
    HeatMeter.SetCurrent(gCarCustomizeManager.GetActualHeat());
    HeatMeter.SetPreview(gCarCustomizeManager.GetCartHeat());
    HeatMeter.Draw();
}

int CustomizeCategoryScreen::AddCustomOption(const char *to_pkg, unsigned int tex_hash, unsigned int name_hash, unsigned int to_cat) {
    if (gCarCustomizeManager.IsCareerMode() && CustomizeIsInBackRoom() &&
        gCarCustomizeManager.IsCategoryLocked(to_cat, true)) {
        return -1;
    }
    CustomizeMainOption *opt = new CustomizeMainOption(to_pkg, tex_hash, name_hash, to_cat, Category);
    opt->UnlockStatus = CPS_AVAILABLE;
    AddOption(opt);
    if (gCarCustomizeManager.IsCategoryLocked(to_cat, false)) {
        opt->UnlockStatus = CPS_LOCKED;
    } else if (gCarCustomizeManager.IsCategoryNew(to_cat)) {
        opt->UnlockStatus = CPS_NEW;
    }
    return Options.iIndexToAdd - 3;
}

void CustomizeCategoryScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x911ab364) {
        bBackingOut = true;
        if (BackToPkg) {
            cFEng::mInstance->QueuePackageSwitch(BackToPkg, 0, 0, false);
        } else {
            cFEng::mInstance->QueuePackagePop(1);
        }
    } else if (msg == 0x72619778) {
        CustomizeMainOption *opt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
        if (opt) {
            cFEng::mInstance->QueuePackagePush(opt->ToPkg, 0, opt->Category, false);
        }
    }
    if (!bBackingOut) {
        RefreshHeader();
    }
}

// --- CustomizeMain ---

CustomizeMain::CustomizeMain(ScreenConstructorData *sd) : CustomizeCategoryScreen(sd) {
    iPerfIndex = -1;
    invalidMarkers = 0;
    SetTitle(CustomizeIsInBackRoom() != 0);
    BuildOptionsList();
    SetScreenNames();
    Setup();
}

void CustomizeMain::SwitchRooms() {
    if (CustomizeIsInBackRoom()) {
        CustomizeSetInBackRoom(false);
        FEManager *mgr = FEManager::Get();
        mgr->SetGarageType(static_cast<eGarageType>(3));
    } else {
        CustomizeSetInBackRoom(true);
        FEManager *mgr = FEManager::Get();
        mgr->SetGarageType(static_cast<eGarageType>(4));
    }
    cFEng::mInstance->QueuePackageSwitch(g_pCustomizeMainPkg, 0, 0, false);
}

void CustomizeMain::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x911ab364) {
        gCarCustomizeManager.RelinquishControl();
        bBackingOut = true;
        cFEng::mInstance->QueuePackagePop(1);
    } else if (msg == 0x72619778) {
        CustomizeMainOption *opt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
        if (opt) {
            if (opt->ToPkg == g_pCustomizeShoppingCartPkg) {
                CustomizeShoppingCart::ShowShoppingCart(g_pCustomizeMainPkg);
            } else if (opt->ToPkg) {
                cFEng::mInstance->QueuePackagePush(opt->ToPkg, 0, opt->Category, false);
            } else {
                SwitchRooms();
            }
        }
    } else {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    if (!bBackingOut) {
        RefreshHeader();
    }
}

void CustomizeMain::SetScreenNames() {
}

void CustomizeMain::RefreshHeader() {
    CustomizeCategoryScreen::RefreshHeader();
}

void CustomizeMain::SetTitle(bool isInBackroom) {
    if (isInBackroom) {
        FEngSetLanguageHash(PackageFilename, 0xe75b8cb2, 0x2f7b4b1f);
        FEngSetLanguageHash(PackageFilename, 0xd03c0e21, 0x28feadd);
    } else {
        if (gCarCustomizeManager.IsCareerMode()) {
            FEngSetLanguageHash(PackageFilename, 0xe75b8cb2, 0x98e8c6ce);
        } else {
            FEngSetLanguageHash(PackageFilename, 0xe75b8cb2, 0x2f7b4b1f);
        }
        FEngSetLanguageHash(PackageFilename, 0xd03c0e21, 0x71d9e710);
    }
}

void CustomizeMain::Setup() {
    IconScrollerMenu::Setup();
}

void CustomizeMain::BuildOptionsList() {
}

// --- CustomizeSub ---

CustomizeSub::CustomizeSub(ScreenConstructorData *sd) : CustomizeCategoryScreen(sd) {
    InstalledPartOptionIndex = 0;
    InCartPartOptionIndex = 0;
    TitleHash = 0;
    Setup();
    gCarCustomizeManager.ResetPreview();
}

int CustomizeSub::GetRimBrandIndex(unsigned int hash) {
    return 0;
}

int CustomizeSub::GetVinylGroupIndex(int group) {
    return 0;
}

// --- CustomizationScreen ---

CustomizationScreen::CustomizationScreen(ScreenConstructorData *sd) : IconScrollerMenu(sd) //
    , DisplayHelper(sd->PackageFilename)
{
    Category = static_cast<unsigned int>(static_cast<unsigned short>(sd->Arg >> 16));
    FromCategory = static_cast<unsigned int>(static_cast<short>(sd->Arg));
    pReplacingOption = nullptr;
    bNeedsRefresh = false;
}

CustomizationScreen::~CustomizationScreen() {
}

void CustomizationScreen::AddPartOption(SelectablePart *part, unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash, unsigned int unlock_hash, bool locked) {
    CustomizePartOption *opt = new CustomizePartOption(part, tex_hash, name_hash, desc_hash, unlock_hash);
    AddOption(opt);
}

SelectablePart *CustomizationScreen::FindInCartPart() {
    ShoppingCartItem *item = gCarCustomizeManager.GetFirstCartItem();
    while (item != reinterpret_cast<ShoppingCartItem *>(&gCarCustomizeManager.ShoppingCart)) {
        SelectablePart *buy = item->GetBuyingPart();
        if (!buy->IsPerformancePkg() && buy->GetSlotID() == static_cast<int>(Category & 0xFFFF)) {
            return buy;
        }
        item = item->GetNext();
    }
    return nullptr;
}

CustomizePartOption *CustomizationScreen::FindMatchingOption(SelectablePart *to_find) {
    if (!to_find) return nullptr;
    return nullptr;
}

void CustomizationScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x911ab364) {
        gCarCustomizeManager.ResetPreview();
        cFEng::mInstance->QueuePackagePop(1);
    } else if (msg == 0x72619778) {
        CustomizePartOption *opt = GetSelectedOption();
        if (opt && opt->ThePart) {
            gCarCustomizeManager.AddToCart(opt->ThePart);
            cFEng::mInstance->QueuePackagePop(1);
        }
    }
}

void CustomizationScreen::RefreshHeader() {
}

// --- CustomizeParts ---

CustomizeParts::CustomizeParts(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    PacksLoadedCount = 0;
    TexturesLoadedCount = 0;
    TachRPM = 0;
    bTexturesNeedUnload = false;
    Setup();
}

CustomizeParts::~CustomizeParts() {
}

void CustomizeParts::Setup() {
}

void CustomizeParts::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizeParts::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

// --- CustomizePaint ---

void CustomizePaint::BuildSwatchList(unsigned int slot_id) {
}

void CustomizePaint::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizePaint::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

SelectablePart *CustomizePaint::FindInCartPart() {
    return nullptr;
}

CustomizePartOption *CustomizePaint::FindMatchingOption(SelectablePart *to_find) {
    return CustomizationScreen::FindMatchingOption(to_find);
}

void CustomizePaint::SetupRimPaint() {
}

void CustomizePaint::SetupVinylColor() {
}

void CustomizePaint::ScrollFilters(eScrollDir dir) {
}

void CustomizePaint::Setup() {
}

eMenuSoundTriggers CustomizePaint::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return maybe;
}

unsigned int CustomizePaint::CalcBrandHash(CarPart *part) {
    return 0;
}

void CustomizePaint::AddVinylAndColorsToCart() {
}

// --- CustomizePerformance ---

CustomizePerformance::CustomizePerformance(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    DescLines[0] = nullptr;
    DescLines[1] = nullptr;
    DescLines[2] = nullptr;
    DescBullets[0] = nullptr;
    DescBullets[1] = nullptr;
    DescBullets[2] = nullptr;
    Setup();
}

void CustomizePerformance::Setup() {
}

void CustomizePerformance::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizePerformance::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

eMenuSoundTriggers CustomizePerformance::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    return maybe;
}

unsigned int CustomizePerformance::GetPerfPkgDesc(Physics::Upgrades::Type type, int level, int line, bool turbo) {
    return 0;
}

unsigned int CustomizePerformance::GetPerfPkgBrand(Physics::Upgrades::Type type, int level, int line) {
    return 0;
}

// --- CustomizeNumbers ---

CustomizeNumbers::CustomizeNumbers(ScreenConstructorData *sd) : MenuScreen(sd) //
    , DisplayHelper(sd->PackageFilename)
{
    TheLeftNumber = nullptr;
    TheRightNumber = nullptr;
    Category = static_cast<unsigned int>(static_cast<unsigned short>(sd->Arg >> 16));
    FromCategory = static_cast<unsigned int>(static_cast<short>(sd->Arg));
    LeftDisplayValue = 0;
    RightDisplayValue = 0;
    bLeft = true;
    Setup();
}

void CustomizeNumbers::Setup() {
}

void CustomizeNumbers::RefreshHeader() {
}

void CustomizeNumbers::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    MenuScreen::NotificationMessage(msg, pobj, param1, param2);
}

void CustomizeNumbers::UnsetShoppingCart() {
}

void CustomizeNumbers::ScrollNumbers(eScrollDir dir) {
}

// --- CustomizeHUDColor ---

CustomizeHUDColor::CustomizeHUDColor(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    SelectedColor = nullptr;
    Cursor = nullptr;
    bTexturesNeedUnload = false;
    Setup();
}

CustomizeHUDColor::~CustomizeHUDColor() {
}

void CustomizeHUDColor::Setup() {
}

void CustomizeHUDColor::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizeHUDColor::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

void CustomizeHUDColor::BuildColorOptions() {
}

void CustomizeHUDColor::SetInitialColors() {
}

void CustomizeHUDColor::SetHUDTextures() {
}

void CustomizeHUDColor::ScrollColors(eScrollDir dir) {
}

void CustomizeHUDColor::AddLayerOption(unsigned int layer, unsigned int icon_hash, unsigned int name_hash) {
}

// --- CustomizeDecals ---

CustomizeDecals::CustomizeDecals(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    bIsBlack = false;
    Setup();
}

void CustomizeDecals::Setup() {
}

void CustomizeDecals::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizeDecals::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

unsigned int CustomizeDecals::GetSlotIDFromCategory() {
    unsigned int cat = Category & 0xFFFF;
    return cat;
}

void CustomizeDecals::BuildDecalList(unsigned int selected_name_hash) {
}

// --- CustomizeSpoiler ---

CustomizeSpoiler::CustomizeSpoiler(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    TheFilter = 0;
    SelectedIndex[0] = 0;
    SelectedIndex[1] = 0;
    SelectedIndex[2] = 0;
    SelectedIndex[3] = 0;
    Setup();
}

void CustomizeSpoiler::Setup() {
}

void CustomizeSpoiler::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizeSpoiler::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

void CustomizeSpoiler::BuildPartOptionListFromFilter(CarPart *installed) {
}

void CustomizeSpoiler::ScrollFilters(eScrollDir dir) {
}

// --- CustomizeRims ---

CustomizeRims::CustomizeRims(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    InnerRadius = 0;
    MinRadius = 0;
    MaxRadius = 0;
    Setup();
}

void CustomizeRims::Setup() {
}

void CustomizeRims::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
}

void CustomizeRims::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
}

void CustomizeRims::ScrollRimSizes(eScrollDir dir) {
}

void CustomizeRims::BuildRimsList(int selected_index) {
}

unsigned int CustomizeRims::GetCategoryBrandHash() {
    return 0;
}
