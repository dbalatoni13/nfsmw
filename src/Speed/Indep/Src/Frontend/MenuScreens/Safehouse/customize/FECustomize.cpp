// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/FEng/FEString.h"

#include <types.h>

struct EAXSound;

extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern FEObject *FEngFindObject(const char *pkg, unsigned int hash);
extern FEImage *FEngFindImage(const char *pkg, unsigned int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern void FEngSetScript(const char *pkg, unsigned int hash, unsigned int script, bool b);
extern void FEngSetScript(FEObject *obj, unsigned int script, bool b);
extern void FEngSetLanguageHash(const char *pkg, unsigned int obj_hash, unsigned int lang_hash);
extern void FEngSetCurrentButton(const char *pkg, unsigned int hash);
extern void FEngSetTopLeft(FEObject *obj, float x, float y);
extern void FEngGetTopLeft(FEObject *obj, float &x, float &y);
extern void FEngSetBottomRight(FEObject *obj, float x, float y);
extern void FEngGetBottomRight(FEObject *obj, float &x, float &y);
extern bool CustomizeIsInPerformance();
extern bool CustomizeIsInParts();
extern void CustomizeSetInParts(bool b);
extern void CustomizeSetInPerformance(bool b);
extern int GetCurrentLanguage();
extern const char *GetLocalizedString(unsigned int hash);
extern void GetLocalizedString(char *buf, int size, unsigned int hash);
extern void FEPrintf(const char *pkg, unsigned int hash, const char *fmt, ...);
extern int bSNPrintf(char *buf, int size, const char *fmt, ...);
extern int bSPrintf(char *buf, const char *fmt, ...);

extern CarCustomizeManager gCarCustomizeManager;
extern cFrontendDatabase *FEDatabase;
extern cFEng *cFEng_mInstance;

extern const char *g_pCustomizeMainPkg;
extern const char *g_pCustomizeSubPkg;
extern const char *g_pCustomizeSubTopPkg;
extern const char *g_pCustomizePartsPkg;
extern const char *g_pCustomizePerfPkg;
extern const char *g_pCustomizeDecalsPkg;
extern const char *g_pCustomizePaintPkg;
extern const char *g_pCustomizeRimsPkg;
extern const char *g_pCustomizeHudPkg;
extern const char *g_pCustomizeSpoilerPkg;

extern EAXSound *g_pEAXSound;
extern void PlayUISoundFX(EAXSound *snd, int trigger);

// RealTimer accessed via Timer.hpp
extern float gTradeInFactor;

extern int Showcase_FromIndex;
extern const char *Showcase_FromPackage;
extern unsigned int Showcase_FromArgs;

extern int eLoadStreamingTexturePack(const char *name, void (*callback)(void *), void *param, int priority);
extern void eUnloadStreamingTexturePack(const char *name);
extern void eUnloadStreamingTexture(int *handles, int count);

extern int CustomizeHUDTexPackResources[11];
extern int CustomizeHUDTexTextureResources[55]; // 11*5

extern int g_TheCustomizeEntryPoint;
struct FECarRecord;
extern FECarRecord *g_pCustomizeCarRecordToUse;
extern int CarViewer_haveLoadedOnce;

extern void MarkUnlockableThingSeen(int idx, unsigned int filter);
extern unsigned long FEHashUpper(const char *str);
extern unsigned int bStringHash(const char *str);

extern unsigned int GetNumMarkersFromCategory(eCustomizeCategory cat);
extern unsigned int GetMarkerNameFromCategory(eCustomizeCategory cat);

class FEMarkerManager;
extern FEMarkerManager TheFEMarkerManager;
extern int GetNumCustomizeMarkers(FEMarkerManager *mgr);

class GarageMainScreen;
extern GarageMainScreen *GetInstance_GarageMainScreen();

static bool gInBackRoom;
static bool gInPerformance;
static bool gInParts;

int CustomizeIsInBackRoom() { return gInBackRoom; }
void CustomizeSetInBackRoom(bool b) { gInBackRoom = b; }
bool CustomizeIsInPerformance() { return gInPerformance; }
void CustomizeSetInPerformance(bool b) { gInPerformance = b; }
bool CustomizeIsInParts() { return gInParts; }
void CustomizeSetInParts(bool b) { gInParts = b; }

// --- CustomizationScreenHelper ---

CustomizationScreenHelper::CustomizationScreenHelper(const char *pkg_name) {
    pPackageName = pkg_name;
    bInitComplete = false;
    bUnlockOverlayShowing = false;
    float actual = gCarCustomizeManager.GetActualHeat();
    float cart = gCarCustomizeManager.GetCartHeat();
    HeatMeter.Init(pkg_name, "HEAT_METER", 0.0f, 5.0f, actual, cart);
}

void CustomizationScreenHelper::DrawTitle() {
    const char *title_str = GetLocalizedString(TitleHash);
    char buf[64];
    bSNPrintf(buf, 64, "%s", title_str);
    int lang = GetCurrentLanguage();
    if (lang != 2 && lang != 13) {
        int i = 0;
        while (buf[i] != 0) {
            char c = buf[i];
            if (static_cast<unsigned int>(c - 'A') < 26u) {
                c = c | 0x20;
            }
            buf[i] = c;
            i++;
        }
    }
    FEPrintf(pPackageName, 0xb71b576d, "%s", buf);
}

void CustomizationScreenHelper::SetPlayerCarStatusIcon(eCustomizePartState state) {
    if (state == CPS_INSTALLED) {
        FEngSetVisible(FEngFindObject(pPackageName, 0xd0582feb));
        FEngSetTextureHash(FEngFindImage(pPackageName, 0xd0582feb), 0x696ae039);
    } else if (state < CPS_INSTALLED + 1) {
        if (state == CPS_AVAILABLE) {
            FEngSetInvisible(FEngFindObject(pPackageName, 0xd0582feb));
        }
    } else if (state == CPS_IN_CART) {
        FEngSetVisible(FEngFindObject(pPackageName, 0xd0582feb));
        FEngSetTextureHash(FEngFindImage(pPackageName, 0xd0582feb), 0x1a777e25);
    }
}

void CustomizationScreenHelper::SetUnlockOverlayState(bool show, unsigned int blurb_hash) {
    unsigned int script = 0x5079c8f8;
    bUnlockOverlayShowing = show;
    if (!show) {
        script = 0x33113ac;
    } else {
        FEngSetLanguageHash(pPackageName, 0xa6298e25, blurb_hash);
    }
    FEngSetScript(pPackageName, 0xebc3e6b7, script, true);
}

void CustomizationScreenHelper::FlashStatusIcon(eCustomizePartState state, bool play_sound) {
    unsigned int hash = 0;
    if (state == CPS_INSTALLED || state == CPS_IN_CART) {
        hash = 0xd0582feb;
    } else if (state == CPS_LOCKED) {
        hash = 0xcffb7033;
    }
    FEngSetScript(pPackageName, hash, 0x280164f, true);
    if (play_sound) {
        PlayUISoundFX(g_pEAXSound, 7);
    }
}

// --- CustomizeCategoryScreen ---

CustomizeCategoryScreen::~CustomizeCategoryScreen() {
}

void CustomizeCategoryScreen::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    CustomizeMainOption *curOpt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
    int status = curOpt ? curOpt->UnlockStatus : 0;
    if (status == 2) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xcffb7033), 0xf0574bb2);
    } else if (status == 3) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xcffb7033), 0xcffb7033);
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        goto after_icon;
    }
    FEngSetScript(GetPackageName(), 0xcffb7033, 0x5079c8f8, true);
after_icon:
    if (!gCarCustomizeManager.IsCareerMode()) {
        HeatMeter.SetVisibility(false);
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x8d1559a4));
    } else {
        HeatMeter.SetCurrent(gCarCustomizeManager.GetActualHeat());
        HeatMeter.SetPreview(gCarCustomizeManager.GetCartHeat());
        HeatMeter.Draw();
        if (!CustomizeIsInBackRoom()) {
            FEPrintf(GetPackageName(), 0x7a6d2f71, "%d", gCarCustomizeManager.GetCartTotal(CCT_TOTAL));
            FEPrintf(GetPackageName(), 0xc60adcfd, "%d", FEDatabase->GetCareerSettings()->GetCash());
        } else {
            FEngSetLanguageHash(GetPackageName(), 0x63ca8308, GetMarkerNameFromCategory(static_cast<eCustomizeCategory>(Category)));
            FEPrintf(GetPackageName(), 0x83e3cd39, "%d", GetNumMarkersFromCategory(static_cast<eCustomizeCategory>(Category)));
            FEPrintf(GetPackageName(), 0x23d918fe, "%d", GetNumCustomizeMarkers(&TheFEMarkerManager));
        }
    }
}

// --- CustomizationScreen ---

CustomizationScreen::CustomizationScreen(ScreenConstructorData *sd) : IconScrollerMenu(sd), DisplayHelper(GetPackageName()) {
    pReplacingOption = nullptr;
    bNeedsRefresh = false;
    Options.bFadingIn = true;
    ScrollTime = 0;
    Category = sd->Arg & 0xFFFF;
    FromCategory = static_cast<int>(static_cast<short>(sd->Arg >> 16));
    unsigned int cat = Category;
    GarageMainScreen *gms;
    if (cat > 0x600 && cat < 0x607) {
        gms = GetInstance_GarageMainScreen();
        *(unsigned int *)((char *)gms + 0x8c) = CustomizeDecals::CurrentDecalLocation;
    } else {
        gms = GetInstance_GarageMainScreen();
        *(unsigned int *)((char *)gms + 0x8c) = Category;
    }
}

CustomizationScreen::~CustomizationScreen() {
    GarageMainScreen *gms = GetInstance_GarageMainScreen();
    *(unsigned int *)((char *)gms + 0x8c) = 0xFFFFFFFF;
}

void CustomizationScreen::AddPartOption(SelectablePart *part, unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash, unsigned int unlock_hash, bool locked) {
    CustomizePartOption *opt = new CustomizePartOption(part, tex_hash, name_hash, desc_hash, unlock_hash);
    AddOption(opt);
    opt->SetLocked(locked);
}

CustomizePartOption *CustomizationScreen::FindMatchingOption(SelectablePart *to_find) {
    IconOption *cur = Options.GetHead();
    while (!Options.IsEndOfList(cur)) {
        SelectablePart *part = static_cast<CustomizePartOption *>(cur)->GetPart();
        if (!to_find->PerformancePkg) {
            if (part->ThePart == to_find->ThePart) {
                return static_cast<CustomizePartOption *>(cur);
            }
        } else {
            if (part->PhysicsType == to_find->PhysicsType && part->UpgradeLevel == to_find->UpgradeLevel) {
                return static_cast<CustomizePartOption *>(cur);
            }
        }
        cur = cur->GetNext();
    }
    return nullptr;
}

// --- FEShoppingCartItem ---

void FEShoppingCartItem::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, pBacking->NameHash);
    FEngSetScript(pBacking, 0x249db7b7, true);
    FEngSetScript(pData, 0x249db7b7, true);
    FEngSetScript(pTradeInPrice, 0x249db7b7, true);
    if (pCheckIcon) {
        FEngSetVisible(pCheckIcon);
        FEngSetScript(pCheckIcon, 0x249db7b7, true);
    }
}

void FEShoppingCartItem::UnsetFocus() {
    unsigned int script = 0x7ab5521a;
    if (!TheItem->IsActive()) {
        script = 0x163c76;
    }
    FEngSetScript(pBacking, script, true);
    FEngSetScript(pData, script, true);
    FEngSetScript(pTradeInPrice, script, true);
    if (pCheckIcon) {
        FEngSetInvisible(pCheckIcon);
        FEngSetScript(pCheckIcon, 0x7ab5521a, true);
    }
}

void FEShoppingCartItem::SetCheckScripts() {
    if (!TheItem->IsActive()) {
        FEngSetScript(pCheckIcon, 0x77cdc4e9, true);
    } else {
        FEngSetScript(pCheckIcon, 0xe6361f46, true);
    }
}

void FEShoppingCartItem::SetActiveScripts() {
    if (!TheItem->IsActive()) {
        FEngSetScript(pCheckIcon, 0x163c76, true);
    }
}

void FEShoppingCartItem::Draw() {
    if (!TheItem->IsActive()) {
        FEngSetTextureHash(pCheckIcon, 0xe719881c);
    } else {
        FEngSetTextureHash(pCheckIcon, 0x696ae039);
    }
    DrawPartName();
    if (!TheItem->GetBuyingPart() || !gCarCustomizeManager.IsCareerMode() || CustomizeIsInBackRoom()) {
        FEPrintf(pTradeInPrice, "");
    } else {
        int tradeIn = TheItem->GetBuyingPart()->GetPrice();
        if (tradeIn == 0) {
            tradeIn = 0;
        } else {
            tradeIn = static_cast<int>(static_cast<float>(tradeIn) * gTradeInFactor);
        }
        FEPrintf(pTradeInPrice, "%d", tradeIn);
    }
    if (!gCarCustomizeManager.IsCareerMode() || CustomizeIsInBackRoom()) {
        FEPrintf(pData, "");
    } else {
        FEPrintf(pData, "%d", TheItem->GetBuyingPart()->GetPrice());
    }
}

void FEShoppingCartItem::Position() {
    FEngSetTopLeft(pCheckIcon, vTopLeft.x, vTopLeft.y - 10.0f);
    FEngSetTopLeft(pBacking, vTopLeft.x + 30.0f, vTopLeft.y);
    float tx, ty;
    FEngGetTopLeft(pTradeInPrice, tx, ty);
    FEngSetTopLeft(pTradeInPrice, tx, vTopLeft.y);
    float bx, by;
    FEngGetBottomRight(pTradeInPrice, bx, by);
    FEngSetBottomRight(pTradeInPrice, vSize.x + 150.0f, by);
    FEngGetTopLeft(pData, tx, ty);
    FEngSetTopLeft(pData, tx, vTopLeft.y);
    FEngGetBottomRight(pData, bx, by);
    FEngSetBottomRight(pData, vSize.x + 40.0f, by);
    if (pCheckIcon) {
        FEngSetTopLeft(pCheckIcon, vTopLeft.x - vBackingOffset.x, vTopLeft.y - vBackingOffset.y);
    }
}

unsigned int FEShoppingCartItem::GetPerfPkgCatHash(Physics::Upgrades::Type phys_type) {
    unsigned int hash = 0;
    switch (phys_type) {
        case Physics::Upgrades::kType_Tires: hash = 0x5aa9137; break;
        case Physics::Upgrades::kType_Brakes: hash = 0x91997ee8; break;
        case Physics::Upgrades::kType_Chassis: hash = 0x6e101aa7; break;
        case Physics::Upgrades::kType_Transmission: hash = 0x29aa74ba; break;
        case Physics::Upgrades::kType_Engine: hash = 0x9853d9a6; break;
        case Physics::Upgrades::kType_Induction:
            if (gCarCustomizeManager.IsTurbo()) {
                hash = 0x5b1255c;
            } else {
                hash = 0xbb6812bb;
            }
            break;
        case Physics::Upgrades::kType_Nitrous: hash = 0x4ce19aa4; break;
        default: break;
    }
    return hash;
}

unsigned int FEShoppingCartItem::GetPerfPkgLevelHash(int level) {
    switch (level) {
        case 1: return 0x69c270c4;
        case 2: return 0x69c270c5;
        case 3: return 0x69c270c6;
        case 4: return 0x69c270c7;
        case 5: return 0x69c270c8;
        case 6: return 0x69c270c9;
        default: return 0x69c270c3;
    }
}

unsigned int FEShoppingCartItem::GetCarPartCatHash(unsigned int slot_id) {
    switch (slot_id) {
        case 0x17: return 0x6134c218;
        case 0x2c: return 0x94e73021;
        case 0x3e: return 0x61e8f83c;
        case 0x3f: return 0x4d4a88d;
        case 0x42: return 0xf868eb0b;
        case 0x4c: return 0x55da70c;
        case 0x4d: return 0xbfa52c55;
        case 0x4e: return 0xe126ff53;
        case 0x53: return 0x301dedd3;
        case 0x5b: return 0x48e6ca49;
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68: return 0x34367c86;
        case 0x69:
        case 0x6a: return 0xddf80259;
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70: return 0x955980bc;
        case 0x71: return 0x6857e5ac;
        case 0x73: return 0x78980a6b;
        case 0x7b: return 0xb1f9b0c9;
        case 0x84: return 0xd32729a6;
        default: return 0;
    }
}

// --- CustomizeShoppingCart ---

CustomizeShoppingCart::CustomizeShoppingCart(ScreenConstructorData *sd) : UIWidgetMenu(sd) {
    iMaxWidgetsOnScreen = 0;
    if (gCarCustomizeManager.IsCareerMode()) {
        iMaxWidgetsOnScreen = 4;
    } else {
        iMaxWidgetsOnScreen = 6;
    }
    Setup();
}

bool CustomizeShoppingCart::CanCheckout() {
    if (!gCarCustomizeManager.IsCareerMode()) {
        return true;
    }
    if (CustomizeIsInBackRoom()) {
        return true;
    }
    return gCarCustomizeManager.GetCartTotal(CCT_TOTAL) <= FEDatabase->GetCareerSettings()->GetCash();
}

void CustomizeShoppingCart::ToggleAllNumberDecals() {
    int count = gCarCustomizeManager.ShoppingCart.TraversebList(nullptr);
    for (int i = 0; i < count; i++) {
        ShoppingCartItem *item = static_cast<ShoppingCartItem *>(gCarCustomizeManager.ShoppingCart.GetNode(i));
        if (item && item->GetBuyingPart()) {
            if (IsSlotIDNumberDecal(item->GetBuyingPart()->GetSlotID())) {
                item->ToggleActive();
            }
        }
    }
}

void CustomizeShoppingCart::ToggleChecked() {
    if (pCurrentOption) {
        ShoppingCartItem *item = static_cast<FEShoppingCartItem *>(pCurrentOption)->GetItem();
        if (item) {
            item->ToggleActive();
            if (item->GetBuyingPart() && IsSlotIDNumberDecal(item->GetBuyingPart()->GetSlotID())) {
                ToggleAllNumberDecals();
                item->ToggleActive();
            }
        }
        static_cast<FEShoppingCartItem *>(pCurrentOption)->SetCheckScripts();
        pCurrentOption->Draw();
    }
}

void CustomizeShoppingCart::UncheckAllItems() {
    int count = gCarCustomizeManager.ShoppingCart.TraversebList(nullptr);
    for (int i = 0; i < count; i++) {
        ShoppingCartItem *item = static_cast<ShoppingCartItem *>(gCarCustomizeManager.ShoppingCart.GetNode(i));
        if (item->IsActive()) {
            item->ToggleActive();
        }
    }
    int wcount = Options.TraversebList(nullptr);
    for (int j = 0; j < wcount; j++) {
        FEShoppingCartItem *w = static_cast<FEShoppingCartItem *>(Options.GetNode(j));
        w->SetCheckScripts();
        w->Draw();
    }
}

// --- CustomizeParts ---

static void UnLoadCustomHUDPacksAndTextures();

CustomizeParts::~CustomizeParts() {
    if (TexturePackLoaded && bTexturesNeedUnload) {
        UnLoadCustomHUDPacksAndTextures();
    }
}

void CustomizeParts::LoadNextHudTexturePack() {
    char buf[64];
    bSPrintf(buf, "HUD_TEX_%02d", PacksLoadedCount);
    int result = eLoadStreamingTexturePack(buf, reinterpret_cast<void(*)(void*)>(TexturePackLoadedCallbackAccessor), this, 0);
    CustomizeHUDTexPackResources[PacksLoadedCount] = (result != 0) ? 1 : 0;
}

void CustomizeParts::TextureLoadedCallback() {
    if (PacksLoadedCount < 11) {
        LoadNextHudTexturePack();
    } else {
        TexturePackLoaded = true;
        cFEng_mInstance->MakeLoadedPackagesDirty();
        ShowHudObjects();
        RefreshHeader();
        cFEng_mInstance->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
    }
}

// --- CustomizeMain ---

CustomizeMain::CustomizeMain(ScreenConstructorData *sd) : CustomizeCategoryScreen(sd) {
    int entryPoint = g_TheCustomizeEntryPoint;
    invalidMarkers = 0;
    iPerfIndex = 0;
    if (entryPoint == 0) {
        CarViewer_haveLoadedOnce = 0;
    }
    gCarCustomizeManager.TakeControl(static_cast<eCustomizeEntryPoint>(entryPoint), g_pCustomizeCarRecordToUse);
    for (int i = 0; i < 0x39; i++) {
        MarkUnlockableThingSeen(i, gCarCustomizeManager.GetUnlockFilter());
    }
    Setup();
    if (gCarCustomizeManager.IsCareerMode()) {
        FEDatabase->BackupCarStable();
    }
}

void CustomizeMain::Setup() {
    BackToPkg = "FeGarageMain.fng";
    SetTitle(CustomizeIsInBackRoom());
    SetScreenNames();
    CustomizeSetInPerformance(false);
    CustomizeSetInParts(false);
    Category = 0;
    BuildOptionsList();
    if (bFadeInIconsImmediately) {
        Options.bFadingOut = false;
        Options.bFadingIn = true;
        Options.bDelayUpdate = false;
        Options.fMaxFadeTime = 0.0f;
    }
    SetInitialOption(FromCategory & 0xFF);
    RefreshHeader();
}

// --- CustomizeSpoiler ---

CustomizeSpoiler::CustomizeSpoiler(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    TheFilter = 0;
    for (int i = 0; i < 4; i++) {
        SelectedIndex[i] = 1;
    }
    Setup();
}

void CustomizeSpoiler::ScrollFilters(eScrollDir dir) {
    int filter = TheFilter;
    if (dir == eScrollDir(-1)) {
        filter--;
        if (filter < 0) {
            filter = 3;
        }
    } else if (dir == eScrollDir(1)) {
        filter++;
        if (filter > 3) {
            filter = 0;
        }
    }
    if (filter != TheFilter) {
        TheFilter = filter;
        BuildPartOptionListFromFilter(nullptr);
        RefreshHeader();
    }
}

// --- CustomizeSub ---

int CustomizeSub::GetVinylGroupIndex(int group) {
    switch (group) {
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
        case 3: return 5;
        case 4: return 6;
        case 5: return 7;
        case 6: return 8;
        case 7: return 9;
        default: return 1;
    }
}

// --- UnLoadCustomHUDPacksAndTextures ---

static void UnLoadCustomHUDPacksAndTextures() {
    for (int i = 0; i < 11; i++) {
        for (unsigned int j = 0; j < 5; j++) {
            int idx = j * 4 + i * 0x14;
            if (CustomizeHUDTexTextureResources[i * 5 + j] != 0) {
                int handle = CustomizeHUDTexTextureResources[i * 5 + j];
                eUnloadStreamingTexture(&handle, 1);
            }
            CustomizeHUDTexTextureResources[i * 5 + j] = 0;
        }
        if (CustomizeHUDTexPackResources[i] != 0) {
            char buf[64];
            bSPrintf(buf, "HUD_TEX_%02d", i);
            eUnloadStreamingTexturePack(buf);
        }
        CustomizeHUDTexPackResources[i] = 0;
    }
    CustomizeParts::TexturePackLoaded = 0;
}
