// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"

static bool gInBackRoom;
static bool gInPerformance;
static bool gInParts;

int CustomizeIsInBackRoom() { return gInBackRoom; }
void CustomizeSetInBackRoom(bool b) { gInBackRoom = b; }
bool CustomizeIsInPerformance() { return gInPerformance; }
void CustomizeSetInPerformance(bool b) { gInPerformance = b; }
bool CustomizeIsInParts() { return gInParts; }
void CustomizeSetInParts(bool b) { gInParts = b; }

#ifdef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_CARCUSTOMIZE_H
// The rest of this file only compiles when CarCustomize.hpp has been
// included earlier in the jumbo build (zFeOverlay). In zFe2 the
// header is absent, so only the static getters/setters above survive.

#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"

namespace Attrib { namespace Gen { struct pvehicle; } }
namespace Physics { namespace Info { float Redline(const Attrib::Gen::pvehicle &pvehicle); } }

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

extern Timer RealTimer;
extern float gTradeInFactor;

extern int Showcase_FromIndex;
extern const char *Showcase_FromPackage;
extern unsigned int Showcase_FromArgs;
extern int Showcase_FromFilter;
extern SelectablePart *_8Showcase_FromColor;

extern int eLoadStreamingTexturePack(const char *name, void (*callback)(void *), void *param, int priority);
extern void eUnloadStreamingTexturePack(const char *name);
extern void eUnloadStreamingTexture(int *handles, int count);

extern int CustomizeHUDTexPackResources[11];
extern int CustomizeHUDTexTextureResources[55];

extern int g_TheCustomizeEntryPoint;
struct FECarRecord;
extern FECarRecord *g_pCustomizeCarRecordToUse;
extern int CarViewer_haveLoadedOnce;

extern void MarkUnlockableThingSeen(int idx, unsigned int filter);
extern unsigned long FEHashUpper(const char *str);
extern unsigned int bStringHash(const char *str);

extern unsigned int GetNumMarkersFromCategory(eCustomizeCategory cat);
extern unsigned int GetMarkerNameFromCategory(eCustomizeCategory cat);

extern void FEngSetColor(FEObject *obj, unsigned int color);
extern unsigned int FEngHashString(const char *fmt, ...);
extern bool DoesStringExist(unsigned int hash);
struct TextureInfo;
extern TextureInfo *GetTextureInfo(unsigned int handle, int p2, int p3);
extern void FEngSetLanguageHash(FEString *str, unsigned int hash);
extern void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(void *), void *param, int priority);

extern const char *g_pCustomizeShowcasePkg;
extern const char *g_pCustomizeDlgPkg;
extern void StartCareerFreeRoam();
extern char FEngMapJoyParamToJoyport(unsigned long param);
extern void *MemoryCard_s_pThis;

class GarageMainScreen;
extern GarageMainScreen *GetInstance_GarageMainScreen();

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
            FEPrintf(GetPackageName(), 0x23d918fe, "%d", TheFEMarkerManager.GetNumCustomizeMarkers());
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

// --- CustomizationScreenHelper additional ---

void CustomizationScreenHelper::SetCareerStatusIcon(eCustomizePartState state) {
    if (state == CPS_LOCKED) {
        FEngSetVisible(FEngFindObject(pPackageName, 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(pPackageName, 0xcffb7033), 0xf0574bb2);
    } else if (state == CPS_NEW) {
        FEngSetVisible(FEngFindObject(pPackageName, 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(pPackageName, 0xcffb7033), 0xcffb7033);
    } else {
        FEngSetInvisible(FEngFindObject(pPackageName, 0xcffb7033));
    }
}

void CustomizationScreenHelper::SetCashVisibility(bool visible) {
    if (visible) {
        FEngSetVisible(FEngFindObject(pPackageName, 0x9ea22e0b));
    } else {
        FEngSetInvisible(FEngFindObject(pPackageName, 0x9ea22e0b));
    }
}

void CustomizationScreenHelper::SetCareerStuff(SelectablePart *part, unsigned int name_hash, unsigned int desc_hash) {
    if (!gCarCustomizeManager.IsCareerMode()) {
        SetCashVisibility(false);
        FEngSetInvisible(FEngFindObject(pPackageName, 0xcffb7033));
        return;
    }
    SetCashVisibility(true);
    FEPrintf(pPackageName, 0x8531e22e, "%d", FEDatabase->GetCareerSettings()->GetCash());
    SetCareerStatusIcon(part->GetPartState());
    if (name_hash != 0) {
        FEngSetLanguageHash(pPackageName, 0xd57c95e1, name_hash);
    }
    if (desc_hash != 0) {
        FEngSetLanguageHash(pPackageName, 0x3cb2b36c, desc_hash);
    }
}

void CustomizationScreenHelper::SetPartStatus(SelectablePart *part, unsigned int unlock_hash, int part_num, int max_parts) {
    if (!gCarCustomizeManager.IsCareerMode()) {
        return;
    }
    eCustomizePartState state = part->GetPartState();
    SetCareerStatusIcon(state);
    if (state == CPS_LOCKED) {
        FEngSetLanguageHash(pPackageName, 0xd57c95e1, unlock_hash);
    } else if (max_parts > 1) {
        FEPrintf(pPackageName, 0xd57c95e1, "%d/%d", part_num, max_parts);
    }
}

// --- CustomizationScreen additional ---

void CustomizationScreen::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    CustomizePartOption *opt = static_cast<CustomizePartOption *>(Options.GetCurrentOption());
    if (opt) {
        SelectablePart *part = opt->GetPart();
        DisplayHelper.SetCareerStuff(part, 0, 0);
        DisplayHelper.SetUnlockOverlayState(part->GetPartState() == CPS_LOCKED, 0);
    }
    DisplayHelper.DrawTitle();
}

SelectablePart *CustomizationScreen::FindInCartPart() {
    IconOption *cur = Options.GetHead();
    while (!Options.IsEndOfList(cur)) {
        CustomizePartOption *opt = static_cast<CustomizePartOption *>(cur);
        if ((opt->GetPart()->GetPartState() & 0xF0) == CPS_IN_CART) {
            return opt->GetPart();
        }
        cur = cur->GetNext();
    }
    return nullptr;
}

void CustomizationScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x35f8620b) {
        bNeedsRefresh = true;
        RefreshHeader();
    }
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        ScrollTime.SetPackedTime(RealTimer.GetPackedTime());
    }
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0xb5af2461) {
        CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
        return;
    }
    if (msg == 0x5e6ea975) {
        Options.SetAllowFade(true);
        Options.bDelayUpdate = false;
        return;
    }
    if (msg == 0x406415e3) {
        CustomizePartOption *curOpt = static_cast<CustomizePartOption *>(Options.GetCurrentOption());
        if (curOpt) {
            eCustomizePartState state = curOpt->GetPart()->GetPartState();
            if ((state & 0xF) == CPS_LOCKED) {
                DisplayHelper.FlashStatusIcon(CPS_LOCKED, true);
                return;
            }
            unsigned int stateFlags = state & 0xF0;
            if (stateFlags == CPS_IN_CART) {
                DisplayHelper.FlashStatusIcon(CPS_IN_CART, true);
                return;
            }
            if (stateFlags == CPS_INSTALLED) {
                ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(curOpt->GetPart());
                if (inCart) {
                    gCarCustomizeManager.RemoveFromCart(inCart);
                    CustomizePartOption *found = FindMatchingOption(inCart->GetBuyingPart());
                    if (found) {
                        found->GetPart()->PartState = static_cast<eCustomizePartState>(found->GetPart()->PartState & 0xF);
                    }
                }
                DisplayHelper.FlashStatusIcon(CPS_INSTALLED, true);
                return;
            }
        }
        ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(static_cast<CustomizePartOption *>(Options.GetCurrentOption())->GetPart());
        if (inCart) {
            pReplacingOption = FindMatchingOption(inCart->GetBuyingPart());
        }
        cFEng_mInstance->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xFF);
        return;
    }
    if (msg == 0x91dfdf84) {
        if (pReplacingOption) {
            SelectablePart *rp = pReplacingOption->GetPart();
            rp->PartState = static_cast<eCustomizePartState>(rp->PartState & 0xF);
            pReplacingOption = nullptr;
        }
        CustomizePartOption *curOpt = static_cast<CustomizePartOption *>(Options.GetCurrentOption());
        gCarCustomizeManager.AddToCart(curOpt->GetPart());
        curOpt = static_cast<CustomizePartOption *>(Options.GetCurrentOption());
        curOpt->GetPart()->PartState = static_cast<eCustomizePartState>((curOpt->GetPart()->PartState & 0xF) | CPS_IN_CART);
        RefreshHeader();
        return;
    }
    if (msg == 0xc519bfbf) {
        unsigned int cat = Category;
        if (cat > 0x200 && cat < 0x208) {
            return;
        }
        if (cat == 0x307) {
            return;
        }
        if (Options.pCurrentNode == nullptr) {
            Showcase_FromIndex = 0;
        } else {
            Showcase_FromIndex = Options.GetCurrentIndex();
        }
        Showcase_FromPackage = GetPackageName();
        Showcase_FromArgs = Category | (FromCategory << 16);
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeShowcasePkg, 0, 0, false);
        return;
    }
    if (msg == 0xcf91aacd) {
        CustomizeShoppingCart::ExitShoppingCart();
        return;
    }
    if (msg == 0xc98356ba) {
        if (!bNeedsRefresh) {
            return;
        }
        float elapsed = static_cast<float>(static_cast<double>(RealTimer.GetPackedTime() - ScrollTime.GetPackedTime())) * 0.001f;
        if (elapsed <= 0.25f) {
            return;
        }
        bNeedsRefresh = false;
        RefreshHeader();
        return;
    }
}

// --- CustomizeShoppingCart additional ---

void CustomizeShoppingCart::ShowShoppingCart(const char *pkg) {
    pParentPkg = pkg;
    cFEng_mInstance->QueuePackageMessage(0x911c0a4b, pkg, nullptr);
}

void CustomizeShoppingCart::ExitShoppingCart() {
    cFEng_mInstance->QueuePackageMessage(0xcf91aacd, pParentPkg, nullptr);
}

bool CustomizeShoppingCart::IsSlotIDNumberDecal(int slot_id) {
    return slot_id == 0x53 || slot_id == 0x4e || slot_id == 0x4f;
}

void CustomizeShoppingCart::ClearUncheckedItems() {
    int count = Options.TraversebList(nullptr);
    for (int i = 0; i < count; i++) {
        FEShoppingCartItem *widget = static_cast<FEShoppingCartItem *>(Options.GetNode(i));
        if (!widget->GetItem()->IsActive()) {
            gCarCustomizeManager.RemoveFromCart(widget->GetItem());
        }
    }
}

void CustomizeShoppingCart::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x406415e3) {
        if (gCarCustomizeManager.DoesCartHaveActiveParts()) {
            if (CanCheckout()) {
                unsigned int dialog_hash;
                if (gCarCustomizeManager.IsCareerMode()) {
                    if (CustomizeIsInBackRoom()) {
                        dialog_hash = 0x4810898;
                    } else {
                        dialog_hash = 0x8ebaa44b;
                    }
                } else {
                    dialog_hash = 0x71d9e710;
                }
                DialogInterface::ShowTwoButtons(GetPackageName(), g_pCustomizeDlgPkg, static_cast<eDialogTitle>(1),
                    0x70e01038, 0x417b25e4, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), dialog_hash);
            } else {
                DialogInterface::ShowOk(GetPackageName(), g_pCustomizeDlgPkg, static_cast<eDialogTitle>(1), 0xa984a42);
            }
        } else {
            gCarCustomizeManager.EmptyCart();
            gCarCustomizeManager.ResetPreview();
            gCarCustomizeManager.ResetPreview();
            cFEng_mInstance->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
            cFEng_mInstance->QueuePackagePop(1);
        }
        return;
    }
    if (msg == 0x72619778) {
        gCarCustomizeManager.EmptyCart();
        gCarCustomizeManager.ResetPreview();
        gCarCustomizeManager.ResetPreview();
        cFEng_mInstance->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
        cFEng_mInstance->QueuePackagePop(1);
        return;
    }
    if (msg == 0x911ab364) {
        ClearUncheckedItems();
        cFEng_mInstance->QueueGameMessage(0x5a928018, pParentPkg, 0xFF);
        cFEng_mInstance->QueuePackagePop(1);
        return;
    }
    if (msg == 0xc519bfc4) {
        UncheckAllItems();
    } else if (msg == 0xc519bfc3) {
        ToggleChecked();
    } else if (msg == 0xd05fc3a3) {
        gCarCustomizeManager.Checkout();
        cFEng_mInstance->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
        cFEng_mInstance->QueuePackagePop(1);
        return;
    } else if (msg == 0x911c0a4b) {
        // fall through to RefreshHeader
    } else {
        return;
    }
    RefreshHeader();
}

void CustomizeShoppingCart::RefreshHeader() {
    if (pCurrentOption == nullptr) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x842b0e89));
    } else {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0x842b0e89));
        ShoppingCartItem *item = static_cast<FEShoppingCartItem *>(pCurrentOption)->GetItem();
        if (item->GetBuyingPart()->IsPerformancePkg()) {
            FEngSetLanguageHash(GetPackageName(), 0xd57c95e1, 0x28feadd);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xd57c95e1, 0x5dabcbc0);
        }
    }
    HeatMeter.SetCurrent(gCarCustomizeManager.GetActualHeat());
    HeatMeter.SetPreview(gCarCustomizeManager.GetCartHeat());
    HeatMeter.Draw();
    if (!gCarCustomizeManager.IsCareerMode()) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x9ea22e0b));
    } else {
        if (!CustomizeIsInBackRoom()) {
            FEPrintf(GetPackageName(), 0xd1497a06, "%d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0)));
            FEPrintf(GetPackageName(), 0x34f7c0e8, "%d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(1)));
            int totalCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(2));
            FEPrintf(GetPackageName(), 0x18661565, "%d", totalCost);
            FEPrintf(GetPackageName(), 0x8531e22e, "%d", FEDatabase->GetCareerSettings()->GetCash() - totalCost);
        } else {
            SetMarkerAmounts();
            if (CustomizeIsInParts()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0xa03a752f);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x4ac68298);
                int total = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BODY, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_SPOILER, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_HOOD, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ROOF_SCOOP, 0);
                int available = 0;
                int cartCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0));
                FEPrintf(GetPackageName(), 0xd1497a06, "%d", cartCost);
                FEPrintf(GetPackageName(), 0x18661565, "%d", (total + available) - cartCost);
            } else if (CustomizeIsInPerformance()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0x358db897);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x68342700);
                int total = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BRAKES, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ENGINE, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_NOS, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_INDUCTION, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_TIRES, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_TRANSMISSION, 0);
                int available = 0;
                int cartCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0));
                FEPrintf(GetPackageName(), 0xd1497a06, "%d", cartCost);
                FEPrintf(GetPackageName(), 0x18661565, "%d", (total + available) - cartCost);
            } else {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0x93296e59);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x78f1c602);
                int total = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0)
                    + TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0);
                int available = 0;
                int cartCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0));
                FEPrintf(GetPackageName(), 0xd1497a06, "%d", cartCost);
                FEPrintf(GetPackageName(), 0x18661565, "%d", (total + available) - cartCost);
            }
        }
    }
}

void CustomizeShoppingCart::SetMarkerAmounts() {
    // TODO: implement marker tracking
}

void CustomizeShoppingCart::SetMarkerData(int idx, ShoppingCartItem *item, int spending) {
    // TODO: implement marker data
}

int CustomizeShoppingCart::GetNumMarkersSpending(unsigned int marker) {
    return 0;
}

void CustomizeShoppingCart::SetMarkerImages() {
    // TODO: implement marker images
}

void CustomizeShoppingCart::Setup() {
    int count = gCarCustomizeManager.ShoppingCart.TraversebList(nullptr);
    for (int i = 0; i < count; i++) {
        ShoppingCartItem *item = static_cast<ShoppingCartItem *>(gCarCustomizeManager.ShoppingCart.GetNode(i));
        AddItem(item);
    }
}

void CustomizeShoppingCart::AddItem(ShoppingCartItem *item) {
    FEShoppingCartItem *widget = new FEShoppingCartItem(item);
    AddStatOption(widget);
    widget->Draw();
}

// --- CustomizeCategoryScreen additional ---

CustomizeCategoryScreen::CustomizeCategoryScreen(ScreenConstructorData *sd) : IconScrollerMenu(sd) //
    , HeatMeter() {
    bBackingOut = false;
    BackToPkg = nullptr;
    Category = sd->Arg & 0xFFFF;
    FromCategory = static_cast<int>(static_cast<short>(sd->Arg >> 16));
}

int CustomizeCategoryScreen::AddCustomOption(const char *to_pkg, unsigned int tex_hash, unsigned int name_hash, unsigned int to_cat) {
    CustomizeMainOption *opt = new CustomizeMainOption(to_pkg, tex_hash, name_hash, to_cat, Category);
    if (gCarCustomizeManager.IsCategoryLocked(to_cat, CustomizeIsInBackRoom())) {
        opt->UnlockStatus = CPS_LOCKED;
    } else if (gCarCustomizeManager.IsCategoryNew(to_cat)) {
        opt->UnlockStatus = CPS_NEW;
    }
    AddOption(opt);
    return opt->UnlockStatus;
}

void CustomizeCategoryScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0xb4edeb6d) {
        bBackingOut = true;
        return;
    }
    if (msg == 0xc519bfbf) {
        Showcase_FromPackage = GetPackageName();
        Showcase_FromArgs = Category | (Options.GetCurrentIndex() << 16);
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeShowcasePkg, 0, 0, false);
        return;
    }
    if (msg == 0xe1fde1d1) {
        if (!bBackingOut) {
            return;
        }
        cFEng_mInstance->QueuePackageSwitch(BackToPkg, Category | (FromCategory << 16), 0, false);
        return;
    }
    if (msg == 0xb5af2461 || msg == 0x1720b124) {
        CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
        return;
    }
    if (msg == 0x7a318ee0) {
        gCarCustomizeManager.EmptyCart();
        gCarCustomizeManager.ResetPreview();
        cFEng_mInstance->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
        return;
    }
    if (msg == 0x911ab364) {
        bool shouldPop = true;
        if (Category > 0x800 && Category < 0x804) {
            if (gCarCustomizeManager.DoesCartHaveActiveParts()) {
                gCarCustomizeManager.EmptyCart();
                gCarCustomizeManager.ResetPreview();
                shouldPop = false;
                cFEng_mInstance->QueueGameMessage(0x1720b124, GetPackageName(), 0xFF);
                bBackingOut = true;
            }
        }
        if (shouldPop) {
            bBackingOut = true;
            cFEng_mInstance->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
        }
        return;
    }
}

// --- SetStockPartOption ---

// SetStockPartOption::React is out-of-line (not inline)
void SetStockPartOption::React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) {
    cFEng_mInstance->QueueGameMessage(0x406415e3, pkg_name, 0xFF);
}

// --- CustomizeMain additional ---

void CustomizeMain::SetTitle(bool isInBackroom) {
    unsigned int title;
    if (isInBackroom) {
        title = 0xa1caff8d;
    } else {
        title = 0x5c01c5;
    }
    FEngSetLanguageHash(GetPackageName(), 0x50fe8b76, title);
    if (gCarCustomizeManager.IsCareerMode()) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0x23d918fe));
        FEPrintf(GetPackageName(), 0x23d918fe, "%d", TheFEMarkerManager.GetNumCustomizeMarkers());
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x23d918fe));
    }
}

void CustomizeMain::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    CustomizeMainOption *curOpt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
    int status = curOpt ? curOpt->UnlockStatus : 0;
    if (status == CPS_LOCKED) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xcffb7033), 0xf0574bb2);
    } else if (status == CPS_NEW) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xcffb7033), 0xcffb7033);
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xcffb7033));
    }
}

void CustomizeMain::SwitchRooms() {
    if (CustomizeIsInBackRoom()) {
        CustomizeSetInBackRoom(false);
    } else {
        CustomizeSetInBackRoom(true);
    }
    GarageMainScreen *gms = GetInstance_GarageMainScreen();
    gms->UpdateCurrentCameraView(false);
    SetTitle(CustomizeIsInBackRoom());
    BuildOptionsList();
    SetScreenNames();
    bFadeInIconsImmediately = true;
    Options.RemoveAll();
    Setup();
}

void CustomizeMain::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (!gCarCustomizeManager.IsCareerMode() || msg != 0x911ab364) {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    if (msg == 0x34dc1bec) {
        if (invalidMarkers < TheFEMarkerManager.GetNumCustomizeMarkers()) {
            SwitchRooms();
        }
        invalidMarkers = 0;
    } else if (msg == 0x1265ece9) {
        GarageMainScreen *gms = GetInstance_GarageMainScreen();
        gms->UpdateCurrentCameraView(false);
        unsigned int qmsg;
        if (CustomizeIsInBackRoom()) {
            qmsg = 0xa1caff8d;
        } else {
            qmsg = 0x5c01c5;
        }
        cFEng_mInstance->QueuePackageMessage(qmsg, GetPackageName(), nullptr);
    } else if (msg == 0x911ab364) {
        if (!gCarCustomizeManager.IsCareerMode()) {
            cFEng_mInstance->QueuePackageMessage(0x6d5d86a1, GetPackageName(), nullptr);
        } else {
            if (CustomizeIsInBackRoom()) {
                SwitchRooms();
                return;
            }
            cFEng_mInstance->QueuePackageMessage(0x6d5d86a1, GetPackageName(), nullptr);
            GarageMainScreen *gms = GetInstance_GarageMainScreen();
            *(unsigned int *)((char *)gms + 0x8c) = 0xFFFFFFFF;
            char port = FEngMapJoyParamToJoyport(param1);
            FEDatabase->SetPlayersJoystickPort(0, port);
            if (!FEDatabase->IsCarStableDirty()) {
                *(int *)((char *)MemoryCard_s_pThis + 0x78) = 1;
            }
            CarViewer_haveLoadedOnce = 0;
            StartCareerFreeRoam();
        }
        gCarCustomizeManager.RelinquishControl();
    } else if (msg == 0xc519bfc4) {
        if ((!gCarCustomizeManager.IsCareerMode() || TheFEMarkerManager.GetNumCustomizeMarkers() != 0) &&
            gCarCustomizeManager.IsCareerMode() && !CustomizeIsInBackRoom() && !gCarCustomizeManager.IsHeroCar()) {
            invalidMarkers = 0;
            if (TheFEMarkerManager.IsMarkerAvailable(FEMarkerManager::MARKER_INDUCTION, 0) &&
                !gCarCustomizeManager.CanInstallJunkman(Physics::Upgrades::kType_Brakes)) {
                invalidMarkers++;
            }
            if (TheFEMarkerManager.IsMarkerAvailable(FEMarkerManager::MARKER_NOS, 0) &&
                !gCarCustomizeManager.CanInstallJunkman(Physics::Upgrades::kType_Induction)) {
                invalidMarkers++;
            }
            if (invalidMarkers < 1) {
                SwitchRooms();
            } else {
                DialogInterface::ShowOneButton(GetPackageName(), g_pCustomizeDlgPkg, static_cast<eDialogTitle>(2),
                    0x417b2601, 0x34dc1bec, 0x3b3e83);
            }
        }
    }
}

// --- Constructors ---

CustomizeSub::CustomizeSub(ScreenConstructorData *sd) : CustomizeCategoryScreen(sd) {
    Setup();
}

CustomizePerformance::CustomizePerformance(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    Setup();
}

CustomizeRims::CustomizeRims(ScreenConstructorData *sd) : CustomizationScreen(sd) {
}

CustomizeDecals::CustomizeDecals(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    Setup();
}

CustomizeHUDColor::CustomizeHUDColor(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    Setup();
}

// --- CustomizePaint helpers ---

SelectablePart *CustomizePaint::FindInCartPart() {
    IconOption *cur = Options.GetHead();
    while (!Options.IsEndOfList(cur)) {
        CustomizePartOption *opt = static_cast<CustomizePartOption *>(cur);
        SelectablePart *part = opt->GetPart();
        if ((part->GetPartState() & 0xF0) == CPS_IN_CART) {
            return part;
        }
        cur = cur->GetNext();
    }
    return nullptr;
}

CustomizePartOption *CustomizePaint::FindMatchingOption(SelectablePart *to_find) {
    IconOption *cur = Options.GetHead();
    while (!Options.IsEndOfList(cur)) {
        CustomizePartOption *opt = static_cast<CustomizePartOption *>(cur);
        if (opt->GetPart()->GetPart() == to_find->GetPart()) {
            return opt;
        }
        cur = cur->GetNext();
    }
    return nullptr;
}

void CustomizePaint::SetupRimPaint() {
    bTList<SelectablePart> partList;
    gCarCustomizeManager.GetCarPartList(0x5b, partList, 0);
    int count = partList.TraversebList(nullptr);
    for (int i = 0; i < count; i++) {
        SelectablePart *cur = static_cast<SelectablePart *>(partList.GetNode(i));
        AddPartOption(cur, 0, cur->GetPrice(), 0, 0, cur->IsLocked());
    }
}

// --- CustomizeParts helpers ---

void CustomizeParts::ShowHudObjects() {
    FEngSetVisible(FEngFindObject(GetPackageName(), 0x85e907a3));
    FEngSetVisible(FEngFindObject(GetPackageName(), 0x2a413997));
}

// --- CustomizeNumbers helpers ---

void CustomizeNumbers::UnsetShoppingCart() {
    // TODO: iterate number parts and clear cart state
}

// --- CustomizeMain additional ---

extern const char *g_pCustomizeHudColorPkg;
extern const char *g_pCustomizeShoppingCartPkg;
extern int Showcase_FromFilter;

void CustomizeMain::SetScreenNames() {
    if (!CustomizeIsInBackRoom()) {
        g_pCustomizeSubPkg = "CustomizeCategory.fng";
        g_pCustomizeSubTopPkg = "CustomizeGenericTop.fng";
        g_pCustomizePartsPkg = "CustomizeParts.fng";
        g_pCustomizePerfPkg = "CustomizePerformance.fng";
        g_pCustomizeDecalsPkg = "Decals.fng";
        g_pCustomizePaintPkg = "Paint.fng";
        g_pCustomizeRimsPkg = "Rims.fng";
        g_pCustomizeHudColorPkg = "CustomHUDColor.fng";
        if (!gCarCustomizeManager.IsCareerMode()) {
            g_pCustomizeShoppingCartPkg = "ShoppingCart_QR.fng";
        } else {
            g_pCustomizeShoppingCartPkg = "ShoppingCart.fng";
        }
        g_pCustomizeHudPkg = "CustomHUD.fng";
        g_pCustomizeSpoilerPkg = "Spoilers.fng";
    } else {
        g_pCustomizeSubPkg = "CustomizeCategory_BACKROOM.fng";
        g_pCustomizeSubTopPkg = "CustomizeGenericTop_BACKROOM.fng";
        g_pCustomizePartsPkg = "CustomizeParts_BACKROOM.fng";
        g_pCustomizePerfPkg = "CustomizePerformance_BACKROOM.fng";
        g_pCustomizeDecalsPkg = "Decals_BACKROOM.fng";
        g_pCustomizePaintPkg = "Paint_BACKROOM.fng";
        g_pCustomizeRimsPkg = "Rims_BACKROOM.fng";
        g_pCustomizeHudColorPkg = "CustomHUDColor_BACKROOM.fng";
        g_pCustomizeShoppingCartPkg = "ShoppingCart_BACKROOM.fng";
        g_pCustomizeHudPkg = "CustomHUD_BACKROOM.fng";
        g_pCustomizeSpoilerPkg = "Spoilers_BACKROOM.fng";
    }
}

void CustomizeMain::BuildOptionsList() {
    int isHero = gCarCustomizeManager.IsHeroCar();
    if (!CustomizeIsInBackRoom()) {
        if (!isHero) {
            AddCustomOption(g_pCustomizeSubPkg, 0x6e0ca66c, 0x55dce1a, 0x801);
            invalidMarkers = AddCustomOption(g_pCustomizeSubPkg, 0x3987d054, 0xbaef8282, 0x802);
        }
        AddCustomOption(g_pCustomizeSubPkg, 0x3e31ba56, 0xbfa7d7c4, 0x803);
    } else {
        if (!isHero) {
            AddCustomOption(g_pCustomizeSubPkg, 0x73272ed2, 0x55dce1a, 0x801);
            AddCustomOption(g_pCustomizeSubPkg, 0xc61c8d3a, 0xbaef8282, 0x802);
        }
        AddCustomOption(g_pCustomizeSubPkg, 0xe69d4f7c, 0xbfa7d7c4, 0x803);
    }
}

// --- CustomizeSpoiler ---

void CustomizeSpoiler::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x5073ef13) {
        ScrollFilters(eSD_PREV);
        return;
    }
    if (msg == 0xd9feec59) {
        ScrollFilters(eSD_NEXT);
        return;
    }
    if (msg == 0x911ab364) {
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
        return;
    }
    if (msg == 0xc519bfbf) {
        Showcase_FromFilter = TheFilter;
        return;
    }
    if (msg == 0x5a928018) {
        SelectablePart *sel = GetSelectedPart();
        if (!sel) {
            return;
        }
        if (gCarCustomizeManager.IsPartInCart(sel)) {
            return;
        }
        sel->SetPartState(sel->GetPartState() & CPS_GAME_STATE_MASK);
        RefreshHeader();
        return;
    }
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        SelectedIndex[TheFilter] = Options.GetCurrentIndex();
    }
}

void CustomizeSpoiler::Setup() {
    SetTitleHash(0x94e73021);
    FEImage *img1 = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(img1, 0x5bc);
    FEImage *img2 = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(img2, 0x682);
    CarPart *activePart = gCarCustomizeManager.GetActivePartFromSlot(0x2c);
    if (Showcase_FromFilter == -1) {
        if (activePart) {
            unsigned int filter = activePart->GetGroupNumber();
            if (filter != 4) {
                TheFilter = filter;
            }
        }
    } else {
        TheFilter = Showcase_FromFilter;
        Showcase_FromFilter = -1;
    }
    BuildPartOptionListFromFilter(activePart);
    RefreshHeader();
}

void CustomizeSpoiler::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    int filter = TheFilter;
    if (filter == 1) {
        FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x205b328);
    } else if (filter == 0) {
        FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x1f0e2b2);
    } else if (filter == 2) {
        FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x9912746);
    } else if (filter == 3) {
        FEngSetLanguageHash(GetPackageName(), 0x78008599, 0xe7416fc);
    }
    CustomizePartOption *opt = GetSelectedOption();
    Timer scrollDelay;
    scrollDelay.SetTime(0.25f);
    if (scrollDelay.GetPackedTime() < RealTimer.GetPackedTime() - ScrollTime.GetPackedTime()) {
        gCarCustomizeManager.PreviewPart(opt->GetPart()->GetSlotID(), opt->GetPart()->GetPart());
    } else {
        bNeedsRefresh = true;
    }
    CarPart *part = opt->GetPart()->GetPart();
    if (!part->HasAppliedAttribute(bStringHash("BRAND_NAME"))) {
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", part->GetName());
    } else {
        unsigned int brandHash = part->GetAppliedAttributeUParam(bStringHash("BRAND_NAME"), 0);
        FEngSetLanguageHash(GetPackageName(), 0x5e7b09c9, brandHash);
    }
}

void CustomizeSpoiler::BuildPartOptionListFromFilter(CarPart *activePart) {
    Options.RemoveAll();
    int activeIdx = 1;
    Options.AddInitialBookEnds();
    bTList<SelectablePart> partList;
    gCarCustomizeManager.GetCarPartList(0x2c, partList, 0);
    while (!partList.IsEmpty()) {
        SelectablePart *cur = static_cast<SelectablePart *>(partList.GetHead());
        cur->Remove();
        unsigned int groupNum = cur->GetPart()->GetGroupNumber();
        if (groupNum == static_cast<unsigned int>(TheFilter) || groupNum == 4) {
            unsigned int texHash = 0xbb034ea6;
            unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), cur->GetUpgradeLevel());
            if (CustomizeIsInBackRoom()) {
                texHash = 0xc51a4f62;
            }
            CarPart *cpart = cur->GetPart();
            if (cpart->GetAppliedAttributeIParam(bStringHash("SPOILER_TYPE"), 0) != 0) {
                texHash = 0x4d1c18ba;
                if (CustomizeIsInBackRoom()) {
                    texHash = 0x611d142a;
                }
            }
            bool locked = gCarCustomizeManager.IsPartLocked(cur, 0);
            AddPartOption(cur, texHash, cpart->GetUpgradeLevel(), 0, unlockHash, locked);
            if (SelectedIndex[TheFilter] == 1) {
                if (activePart && cur->GetPart() == activePart) {
                    SelectedIndex[TheFilter] = activeIdx;
                }
                activeIdx++;
            }
        } else {
            delete cur;
        }
    }
    if (Showcase_FromIndex == 0) {
        Options.SetInitialPos(SelectedIndex[TheFilter]);
    } else {
        SelectedIndex[TheFilter] = Showcase_FromIndex;
        Options.SetInitialPos(Showcase_FromIndex);
        Showcase_FromIndex = 0;
    }
}

// --- CustomizePerformance ---

void CustomizePerformance::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x406415e3) {
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
    } else if (msg == 0xc407210) {
        cFEng_mInstance->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
    }
}

eMenuSoundTriggers CustomizePerformance::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg == 0x406415e3) {
        return static_cast<eMenuSoundTriggers>(0);
    }
    return CustomizationScreen::NotifySoundMessage(msg, maybe);
}

// --- CustomizeHUDColor ---

CustomizeHUDColor::~CustomizeHUDColor() {
}

void CustomizeHUDColor::AddLayerOption(unsigned int layer, unsigned int icon_hash, unsigned int name_hash) {
    HUDLayerOption *opt = new HUDLayerOption(layer, icon_hash, name_hash);
    Options.AddOption(opt);
}

void CustomizeHUDColor::Setup() {
    AddLayerOption(0xb98c46c3, 0xa1faff6e, 0x74acecbf);
    AddLayerOption(0x93e1e0ee, 0xc0f8c27, 0x66126b05);
    AddLayerOption(0xa2c44293, 0xd094b1c2, 0x17d84e58);
    SetInitialColors();
    RefreshHeader();
}


// --- CustomizeParts ---

void CustomizeParts::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg != 0x406415e3) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    if (msg == 0x5a928018) {
        SelectablePart *sel = GetSelectedPart();
        if (!sel) {
            return;
        }
        if (gCarCustomizeManager.IsPartInCart(sel)) {
            return;
        }
        sel->SetPartState(sel->GetPartState() & CPS_GAME_STATE_MASK);
        RefreshHeader();
        return;
    }
    if (msg == 0x406415e3) {
        if (Category == 0x307) {
            if (!TexturePackLoaded) {
                return;
            }
            SelectablePart *sel = GetSelectedPart();
            if (sel && (sel->GetPartState() & CPS_GAME_STATE_MASK) == CPS_LOCKED) {
                DisplayHelper.FlashStatusIcon(CPS_LOCKED, true);
                return;
            }
            if (gCarCustomizeManager.GetTempColoredPart()) {
                gCarCustomizeManager.ClearTempColoredPart();
            }
            SelectablePart *copy = new SelectablePart(sel);
            gCarCustomizeManager.SetTempColoredPart(copy);
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeHudColorPkg, Category | (FromCategory << 16), 0, false);
        } else if (Category >= 0x402 && Category <= 0x409) {
            SelectablePart *sel = GetSelectedPart();
            if (sel && (sel->GetPartState() & CPS_GAME_STATE_MASK) == CPS_LOCKED) {
                DisplayHelper.FlashStatusIcon(CPS_LOCKED, true);
                return;
            }
            unsigned int tunable = sel->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0);
            if (tunable == 0) {
                CustomizationScreen::NotificationMessage(0x406415e3, pobj, param1, param2);
                return;
            }
            SelectablePart *copy = new SelectablePart(sel);
            gCarCustomizeManager.SetTempColoredPart(copy);
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizePaintPkg, Category | (FromCategory << 16), 0, false);
        } else {
            CustomizationScreen::NotificationMessage(0x406415e3, pobj, param1, param2);
            return;
        }
        return;
    }
    if (msg == 0x911ab364) {
        if (Category == 0x307) {
            if (!TexturePackLoaded) {
                return;
            }
            bTexturesNeedUnload = true;
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (0x307 << 16), 0, false);
        } else if (Category >= 0x402 && Category <= 0x409) {
            gCarCustomizeManager.ClearTempColoredPart();
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubTopPkg, Category | (FromCategory << 16), 0, false);
        } else {
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
        }
        return;
    }
    if (msg == 0xcf91aacd) {
        if (Category != 0x307) {
            return;
        }
        if (!TexturePackLoaded) {
            return;
        }
        bTexturesNeedUnload = true;
        return;
    }
}

void CustomizeParts::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    int numOpts = Options.Options.TraversebList(nullptr);
    if (numOpts != Options.iNumBookEnds) {
        CustomizePartOption *opt = GetSelectedOption();
        CarPart *part = opt->GetPart()->GetPart();
        if (part->HasAppliedAttribute(0x6212682b)) {
            unsigned int tunable = part->GetAppliedAttributeUParam(0x6212682b, 0);
            if (tunable == 0) {
                FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x649f4a65);
            } else {
                FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x8098a54c);
            }
        }
        if (Category == 0x307) {
            SetHUDTextures();
            SetHUDColors();
        } else {
            int timeDiff = RealTimer.GetPackedTime() - ScrollTime.GetPackedTime();
            if (static_cast<float>(static_cast<double>(timeDiff ^ 0x80000000 | 0x4330000000000000ULL) - 4503599627370496.0) * 0.001f <= 0.25f) {
                bNeedsRefresh = true;
            } else {
                gCarCustomizeManager.PreviewPart(opt->GetPart()->GetSlotID(), opt->GetPart()->GetPart());
            }
        }
        if (!part->HasAppliedAttribute(bStringHash("BRAND_NAME"))) {
            FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", part->GetName());
        } else {
            unsigned int brandHash = part->GetAppliedAttributeUParam(bStringHash("BRAND_NAME"), 0);
            FEngSetLanguageHash(GetPackageName(), 0x5e7b09c9, brandHash);
        }
    }
}

// --- CustomizeMeter ---

CustomizeMeter::CustomizeMeter()
    : Min(0.0f) //
    , Max(0.0f) //
    , Current(0.0f) //
    , Preview(0.0f) //
    , PreviousPreview(0.0f) //
    , NumStages(5) //
    , pMeterGroup(nullptr) //
{
    pMultiplier = nullptr;
    pMultiplierZoom = nullptr;
    for (int i = 0; i < 10; i++) {
        pBases[i] = nullptr;
    }
}

void CustomizeMeter::Init(const char *pkg_name, const char *name, float min, float max, float current, float preview) {
    Min = min;
    Max = max;
    SetCurrent(current);
    SetPreview(preview);
    pMultiplier = FEngFindImage(pkg_name, 0x5ffee1d8);
    pMultiplierZoom = FEngFindImage(pkg_name, 0xe637955c);
    pMeterGroup = FEngFindObject(pkg_name, 0xf2492598);
    for (int i = 0; i < 10; i++) {
        unsigned int hash = FEngHashString("METER_BASE_%d", i + 1);
        pBases[i] = FEngFindImage(pkg_name, hash);
    }
}

// --- CustomizeSub ---

int CustomizeSub::GetRimBrandIndex(unsigned int brand) {
    if (brand == 0x1e6a3b) return 10;
    if (brand < 0x1e6a3c) {
        if (brand == 0x9136) return 3;
        if (brand < 0x9137) {
            if (brand == 0x648) return 9;
        } else {
            if (brand == 0x9536) return 4;
            if (brand == 0x1c386b) return 0xb;
        }
    } else {
        if (brand == 0x352d08d1) return 2;
        if (brand < 0x352d08d2) {
            if (brand == 0x2b77feb) return 5;
            if (brand == 0x324ac97) return 6;
        } else {
            if (brand == 0x48e25793) return 7;
            if (brand == 0xdd544a02) return 8;
        }
    }
    return 1;
}

// --- CustomizeHUDColor ---

void CustomizeHUDColor::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        HUDLayerOption *layer = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
        layer->SelectedPart = SelectedColor->ThePart;
    }
    if (msg != 0x91dfdf84) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        BuildColorOptions();
        RefreshHeader();
    } else if (msg == 0x72619778) {
        ScrollColors(eSD_PREV);
    } else if (msg == 0x911c0a4b) {
        ScrollColors(eSD_NEXT);
    } else if (msg == 0x911ab364) {
        gCarCustomizeManager.ClearTempColoredPart();
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeHudPkg, Category | (FromCategory << 16), 0, false);
    } else if (msg == 0xcf91aacd) {
        gCarCustomizeManager.ClearTempColoredPart();
        bNeedsRefresh = true;
    } else if (msg == 0x91dfdf84) {
        SelectablePart *temp = gCarCustomizeManager.GetTempColoredPart();
        ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(0x84u);
        if (inCart && temp->ThePart == inCart->GetBuyingPart()->ThePart) {
            int slot = 0x85;
            do {
                ShoppingCartItem *colorItem = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(slot));
                slot++;
                gCarCustomizeManager.RemoveFromCart(colorItem);
            } while (slot < 0x88);
        }
        gCarCustomizeManager.AddToCart(temp);
        gCarCustomizeManager.ClearTempColoredPart();
        HUDColorOption *node = static_cast<HUDColorOption *>(ColorOptions.GetHead());
        while (node != reinterpret_cast<HUDColorOption *>(&ColorOptions)) {
            if (node->ThePart->PartState != 0) {
                gCarCustomizeManager.AddToCart(node->ThePart);
            }
            node = static_cast<HUDColorOption *>(node->Next);
        }
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeHudPkg, Category | (FromCategory << 16), 0, false);
    }
}

void CustomizeHUDColor::ScrollColors(eScrollDir dir) {
    HUDColorOption *prev = SelectedColor;
    if (dir == eSD_PREV) {
        HUDColorOption *node = static_cast<HUDColorOption *>(prev->Prev);
        if (node == reinterpret_cast<HUDColorOption *>(&ColorOptions)) {
            node = static_cast<HUDColorOption *>(ColorOptions.GetTail());
        }
        SelectedColor = node;
    } else if (dir == eSD_NEXT) {
        HUDColorOption *node = static_cast<HUDColorOption *>(prev->Next);
        if (node == reinterpret_cast<HUDColorOption *>(&ColorOptions)) {
            node = static_cast<HUDColorOption *>(ColorOptions.GetHead());
        }
        SelectedColor = node;
    }
    if (SelectedColor != prev) {
        HUDLayerOption *layer = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
        layer->SelectedPart = SelectedColor->ThePart;
        FEngSetScript(prev->FEngObject, 0x7ab5521a, true);
        FEngSetScript(SelectedColor->FEngObject, 0x249db7b7, true);
        float x, y;
        FEngGetTopLeft(SelectedColor->FEngObject, x, y);
        float dx, dy;
        FEngGetTopLeft(SelectedColor->FEngObject, dx, dy);
        FEngSetTopLeft(Cursor, x + dx, y + dy);
        RefreshHeader();
    }
}

void CustomizeHUDColor::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    HUDColorOption *sel = SelectedColor;
    int slotID = sel->ThePart->CarSlotID;
    if (slotID == 0x85) {
        FEObject *obj = FEngFindObject(GetPackageName(), 0x5d19f25);
        FEngSetColor(obj, sel->color);
    } else if (slotID == 0x86) {
        FEObject *obj = FEngFindObject(GetPackageName(), 0xd312f0cb);
        FEngSetColor(obj, sel->color);
        FEObject *obj2 = FEngFindObject(GetPackageName(), 0x8fe2a217);
        FEngSetColor(obj2, SelectedColor->color);
    } else if (slotID == 0x87) {
        FEObject *obj = FEngFindObject(GetPackageName(), 0xc0721eb9);
        FEngSetColor(obj, sel->color);
        FEObject *obj2 = FEngFindObject(GetPackageName(), 0xc62ad685);
        FEngSetColor(obj2, SelectedColor->color);
        FEObject *obj3 = FEngFindObject(GetPackageName(), 0xb8f1f802);
        FEngSetColor(obj3, SelectedColor->color);
    }
}

// --- CustomizeParts ---

void CustomizeParts::TexturePackLoadedCallback() {
    int idx = PacksLoadedCount;
    int offset = idx * 5;
    PacksLoadedCount = idx + 1;
    CustomizeHUDTexTextureResources[offset] = FEngHashString("HUD_GAUGE_%02d", idx);
    CustomizeHUDTexTextureResources[offset + 1] = FEngHashString("HUD_NEEDLE_%d_%02d", TachRPM, idx);
    CustomizeHUDTexTextureResources[offset + 2] = FEngHashString("HUD_NEEDLE_TURBO_%02d", idx);
    CustomizeHUDTexTextureResources[offset + 3] = FEngHashString("HUD_SPEEDOMETER_%02d", idx);
    CustomizeHUDTexTextureResources[offset + 4] = FEngHashString("HUD_NOS_%02d", idx);
    eLoadStreamingTexture(reinterpret_cast<unsigned int *>(&CustomizeHUDTexTextureResources[offset]), 5,
                          reinterpret_cast<void (*)(void *)>(TextureLoadedCallbackAccessor),
                          reinterpret_cast<void *>(reinterpret_cast<unsigned int>(this)), 0);
}

void CustomizeParts::SetHUDTextures() {
    SelectablePart *sel = GetSelectedPart();
    CarPart *part = sel->ThePart;
    unsigned int hudStyleHash = FEngHashString("HUD_STYLE");
    int hudStyle = part->GetAppliedAttributeIParam(hudStyleHash, 0);
    FEImage *gauge = FEngFindImage(GetPackageName(), 0xc0721eb9);
    FEngSetTextureHash(gauge, FEngHashString("HUD_NEEDLE_%d_%02d", TachRPM, hudStyle));
    FEImage *needle = FEngFindImage(GetPackageName(), 0x5d19f25);
    FEngSetTextureHash(needle, FEngHashString("HUD_GAUGE_%02d", hudStyle));
    FEImage *speedo = FEngFindImage(GetPackageName(), 0xd312f0cb);
    FEngSetTextureHash(speedo, FEngHashString("HUD_SPEEDOMETER_%02d", hudStyle));
    if (!gCarCustomizeManager.IsTurbo()) {
        FEObject *turboGroup = FEngFindObject(GetPackageName(), 0xc5d551b7);
        FEngSetInvisible(turboGroup);
    } else {
        FEImage *turboNeedle = FEngFindImage(GetPackageName(), 0xc62ad685);
        FEngSetTextureHash(turboNeedle, FEngHashString("HUD_NEEDLE_TURBO_%02d", hudStyle));
        FEImage *nos = FEngFindImage(GetPackageName(), 0x8fe2a217);
        FEngSetTextureHash(nos, FEngHashString("HUD_NOS_%02d", hudStyle));
        FEObject *turboGroup = FEngFindObject(GetPackageName(), 0xc5d551b7);
        FEngSetVisible(turboGroup);
    }
}

void CustomizeParts::SetHUDColors() {
    ShoppingCartItem *hudInCart = gCarCustomizeManager.IsPartTypeInCart(0x84u);
    CarPart *installedHud = gCarCustomizeManager.GetInstalledCarPart(0x84);
    SelectablePart *sel = GetSelectedPart();
    if (sel->ThePart == installedHud) {
        goto use_installed_colors;
    } else {
        if (hudInCart) {
            SelectablePart *selPart = GetSelectedPart();
            if (selPart->ThePart == hudInCart->GetBuyingPart()->ThePart) {
                goto use_installed_colors;
            }
        }
        FEngSetColor(FEngFindObject(GetPackageName(), 0x5d19f25), 0xffffc373u);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xc0721eb9), 0xffffffffu);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xc62ad685), 0xffffffffu);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xb8f1f802), 0xffffffffu);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xd312f0cb), 0xffffae40u);
        FEngSetColor(FEngFindObject(GetPackageName(), 0x8fe2a217), 0xffffae40u);
        return;
    }
use_installed_colors:
    {
        unsigned int colors[5];
        int slot = 0x85;
        int idx = 0;
        do {
            ShoppingCartItem *colorInCart = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(slot));
            CarPart *colorPart;
            if (colorInCart && hudInCart) {
                SelectablePart *selPart = GetSelectedPart();
                if (selPart->ThePart == hudInCart->GetBuyingPart()->ThePart) {
                    colorPart = colorInCart->GetBuyingPart()->ThePart;
                } else {
                    colorPart = gCarCustomizeManager.GetInstalledCarPart(slot);
                }
            } else {
                colorPart = gCarCustomizeManager.GetInstalledCarPart(slot);
            }
            slot++;
            unsigned int r = colorPart->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            unsigned int g = colorPart->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            unsigned int b = colorPart->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            colors[idx] = ((r & 0xff) << 16) | ((g & 0xff) << 8) | 0xff000000 | (b & 0xff);
            idx++;
        } while (idx < 3);
        FEngSetColor(FEngFindObject(GetPackageName(), 0x5d19f25), colors[0]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xc0721eb9), colors[2]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xc62ad685), colors[2]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xb8f1f802), colors[2]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xd312f0cb), colors[1]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0x8fe2a217), colors[1]);
    }
}

// --- CustomizeHUDColor ---

void CustomizeHUDColor::SetHUDTextures() {
    float redline = Physics::Info::Redline(*reinterpret_cast<const Attrib::Gen::pvehicle *>(0x804ab3ec));
    int rpm = static_cast<int>(redline);
    int tachRPM;
    if (rpm >= 0x251d) {
        tachRPM = 10000;
    } else if (rpm >= 0x2135) {
        tachRPM = 9000;
    } else if (rpm > 0x1d4c) {
        tachRPM = 8000;
    } else {
        tachRPM = 7000;
    }
    CarPart *part = gCarCustomizeManager.GetTempColoredPart()->ThePart;
    unsigned int hudStyleHash = FEngHashString("HUD_STYLE");
    int hudStyle = part->GetAppliedAttributeIParam(hudStyleHash, 0);
    FEImage *gauge = FEngFindImage(GetPackageName(), 0xc0721eb9);
    FEngSetTextureHash(gauge, FEngHashString("HUD_NEEDLE_%d_%02d", tachRPM, hudStyle));
    FEImage *needle = FEngFindImage(GetPackageName(), 0x5d19f25);
    FEngSetTextureHash(needle, FEngHashString("HUD_GAUGE_%02d", hudStyle));
    FEImage *speedo = FEngFindImage(GetPackageName(), 0xd312f0cb);
    FEngSetTextureHash(speedo, FEngHashString("HUD_SPEEDOMETER_%02d", hudStyle));
    if (!gCarCustomizeManager.IsTurbo()) {
        FEObject *turboGroup = FEngFindObject(GetPackageName(), 0xc5d551b7);
        FEngSetInvisible(turboGroup);
    } else {
        FEImage *turboNeedle = FEngFindImage(GetPackageName(), 0xc62ad685);
        FEngSetTextureHash(turboNeedle, FEngHashString("HUD_NEEDLE_TURBO_%02d", hudStyle));
        FEImage *nos = FEngFindImage(GetPackageName(), 0x8fe2a217);
        FEngSetTextureHash(nos, FEngHashString("HUD_NOS_%02d", hudStyle));
        FEObject *turboGroup = FEngFindObject(GetPackageName(), 0xc5d551b7);
        FEngSetVisible(turboGroup);
    }
}

void CustomizeHUDColor::SetInitialColors() {
    ShoppingCartItem *hudInCart = gCarCustomizeManager.IsPartTypeInCart(0x84u);
    CarPart *installedHud = gCarCustomizeManager.GetInstalledCarPart(0x84);
    unsigned int colors[5];
    colors[0] = 0xffffc373u;
    colors[1] = 0xffffae40u;
    colors[2] = 0xffffffffu;
    SelectablePart *temp = gCarCustomizeManager.GetTempColoredPart();
    if (hudInCart && temp->ThePart == hudInCart->GetBuyingPart()->ThePart) {
        int slot = 0x85;
        int idx = 0;
        do {
            ShoppingCartItem *colorItem = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(slot));
            slot++;
            if (colorItem) {
                CarPart *colorPart = colorItem->GetBuyingPart()->ThePart;
                unsigned int r = colorPart->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                unsigned int g = colorPart->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                unsigned int b = colorPart->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                colors[idx] = ((r & 0xff) << 16) | ((g & 0xff) << 8) | 0xff000000 | (b & 0xff);
            }
            idx++;
        } while (idx < 3);
    } else if (temp->ThePart == installedHud) {
        int slot = 0x85;
        int idx = 0;
        do {
            CarPart *colorPart = gCarCustomizeManager.GetInstalledCarPart(slot);
            slot++;
            if (colorPart) {
                unsigned int r = colorPart->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                unsigned int g = colorPart->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                unsigned int b = colorPart->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                colors[idx] = ((r & 0xff) << 16) | ((g & 0xff) << 8) | 0xff000000 | (b & 0xff);
            }
            idx++;
        } while (idx < 3);
    }
    FEngSetColor(FEngFindObject(GetPackageName(), 0x5d19f25), colors[0]);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xd312f0cb), colors[1]);
    FEngSetColor(FEngFindObject(GetPackageName(), 0x8fe2a217), colors[1]);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xc0721eb9), colors[2]);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xc62ad685), colors[2]);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xb8f1f802), colors[2]);
}

// --- CustomizeRims ---

void CustomizeRims::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x5a928018) {
        SelectablePart *sel = GetSelectedPart();
        if (sel && !gCarCustomizeManager.IsPartInCart(sel)) {
            sel->UnSetInCart();
            RefreshHeader();
        }
    } else if (msg == 0x5073ef13) {
        ScrollRimSizes(eSD_PREV);
    } else if (msg == 0xc519bfbf) {
        Showcase_FromFilter = InnerRadius;
    } else if (msg == 0x911ab364) {
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubTopPkg, Category | (FromCategory << 16), 0, false);
    } else if (msg == 0xd9feec59) {
        ScrollRimSizes(eSD_NEXT);
    }
}

void CustomizeRims::ScrollRimSizes(eScrollDir dir) {
    int radius = InnerRadius;
    if (dir == eSD_PREV) {
        radius--;
        if (radius < MinRadius) {
            radius = MaxRadius;
        }
    } else if (dir == eSD_NEXT) {
        radius++;
        if (MaxRadius < radius) {
            radius = MinRadius;
        }
    }
    if (radius != InnerRadius) {
        InnerRadius = radius;
        int sel;
        if (Options.pCurrentNode) {
            sel = Options.GetCurrentIndex();
        } else {
            sel = 0;
        }
        BuildRimsList(sel);
        RefreshHeader();
    }
}

void CustomizeRims::Setup() {
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    DisplayHelper.TitleHash = 0xe167f7c8;
    MinRadius = gCarCustomizeManager.GetMinInnerRadius();
    InnerRadius = MinRadius;
    MaxRadius = gCarCustomizeManager.GetMaxInnerRadius();
    if (Showcase_FromFilter == -1) {
        CarPart *activePart = gCarCustomizeManager.GetActivePartFromSlot(0x42);
        if (activePart) {
            InnerRadius = static_cast<int>(activePart->GetAppliedAttributeIParam(0xeb0101e2, 0));
        }
    } else {
        InnerRadius = Showcase_FromFilter;
        Showcase_FromFilter = -1;
    }
    BuildRimsList(-1);
    RefreshHeader();
}

void CustomizeRims::BuildRimsList(int selected_index) {
    Options.RemoveAll();
    Options.AddInitialBookEnds();
    int matchIdx = 0;
    int curIdx = 1;
    CarPart *activeMatch = nullptr;
    bTList<SelectablePart> tempList;
    unsigned int brandHash = GetCategoryBrandHash();
    gCarCustomizeManager.GetCarPartList(0x42, tempList, brandHash);
    if (selected_index == -1) {
        activeMatch = gCarCustomizeManager.GetActivePartFromSlot(0x42);
    }
    SelectablePart *node = static_cast<SelectablePart *>(tempList.GetHead());
    while (node != reinterpret_cast<SelectablePart *>(&tempList)) {
        SelectablePart *next = static_cast<SelectablePart *>(node->Next);
        node->Prev->Next = node->Next;
        node->Next->Prev = node->Prev;
        int rimSize = static_cast<int>(node->ThePart->GetAppliedAttributeIParam(0xeb0101e2, 0));
        if (rimSize == InnerRadius) {
            unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), node->GetUpgradeLevel());
            unsigned char gl = *reinterpret_cast<unsigned char *>(reinterpret_cast<int>(node->ThePart) + 5);
            bool locked = gCarCustomizeManager.IsPartLocked(node, 0);
            AddPartOption(node, 0x294d2a3, gl >> 5, 0, unlockHash, locked);
            if (activeMatch && node->ThePart == activeMatch) {
                matchIdx = curIdx;
            }
            curIdx++;
        } else {
            delete node;
        }
        node = next;
    }
    if (selected_index == -1 && activeMatch) {
        selected_index = matchIdx;
    } else if (selected_index == -1) {
        selected_index = 1;
    }
    if (Showcase_FromIndex == 0) {
        if (bFadeInIconsImmediately) {
            Options.bFadingOut = false;
            Options.bFadingIn = true;
            Options.bDelayUpdate = false;
            Options.fCurFadeTime = 0.0f;
        }
        Options.SetInitialPos(selected_index);
    } else {
        if (bFadeInIconsImmediately) {
            Options.bFadingIn = true;
            Options.bFadingOut = false;
            Options.bDelayUpdate = false;
            Options.fCurFadeTime = 0.0f;
        }
        Options.SetInitialPos(0);
        Showcase_FromIndex = 0;
    }
    // Clean up remaining temp list nodes
    while (tempList.GetHead() != reinterpret_cast<bNode *>(&tempList)) {
        SelectablePart *del = static_cast<SelectablePart *>(tempList.GetHead());
        del->Prev->Next = del->Next;
        del->Next->Prev = del->Prev;
        delete del;
    }
}

void CustomizeRims::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    int numOpts = Options.Options.TraversebList(nullptr);
    if (numOpts != Options.iNumBookEnds) {
        CustomizePartOption *opt = static_cast<CustomizePartOption *>(GetSelectedOption());
        gCarCustomizeManager.PreviewPart(opt->ThePart->GetSlotID(), opt->ThePart->ThePart);
        FEPrintf(GetPackageName(), 0xe6782841, "%$d\"", InnerRadius);
        char buf[64];
        const char *name = opt->ThePart->ThePart->GetName();
        bSNPrintf(buf, 64, "%s", name);
        int len = bStrLen(buf);
        for (int i = len; i >= len - 6; i--) {
            buf[i] = 0;
        }
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", buf);
    }
}

unsigned int CustomizeRims::GetCategoryBrandHash() {
    switch (Category) {
        case 0x702: return 0x352d08d1;
        case 0x703: return 0x9136;
        case 0x704: return 0x9536;
        case 0x705: return 0x2b77feb;
        case 0x706: return 0x324ac97;
        case 0x707: return 0x48e25793;
        case 0x708: return 0xdd544a02;
        case 0x709: return 0x648;
        case 0x70a: return 0x1e6a3b;
        case 0x70b: return 0x1c386b;
        default: return 0;
    }
}

// --- CustomizeNumbers ---

CustomizeNumbers::CustomizeNumbers(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , LeftNumberList() //
    , RightNumberList() //
    , TheLeftNumber(nullptr) //
    , TheRightNumber(nullptr) //
    , Category(sd->Arg & 0xFFFF) //
    , FromCategory(static_cast<int>(static_cast<short>(sd->Arg >> 16))) //
    , LeftDisplayValue(-1) //
    , RightDisplayValue(-1) //
    , bLeft(1) //
    , DisplayHelper(sd->PackageFilename) {
    Setup();
}

void CustomizeNumbers::ScrollNumbers(eScrollDir dir) {
    if (LeftDisplayValue == -1 || RightDisplayValue == -1) {
        RightDisplayValue = 0;
        TheLeftNumber = static_cast<SelectablePart *>(LeftNumberList.GetHead());
        TheRightNumber = static_cast<SelectablePart *>(RightNumberList.GetHead());
        LeftDisplayValue = 0;
        RefreshHeader();
    } else {
        SelectablePart *current;
        if (bLeft) {
            current = TheRightNumber;
        } else {
            current = TheLeftNumber;
        }
        if (dir == eSD_PREV) {
            if (!bLeft) {
                current = static_cast<SelectablePart *>(current->Prev);
                if (current == reinterpret_cast<SelectablePart *>(&RightNumberList)) {
                    current = static_cast<SelectablePart *>(RightNumberList.GetTail());
                }
                RightDisplayValue = static_cast<short>(RightDisplayValue - 1);
                if (RightDisplayValue < 0) {
                    RightDisplayValue = 9;
                }
            } else {
                current = static_cast<SelectablePart *>(current->Prev);
                if (current == reinterpret_cast<SelectablePart *>(&LeftNumberList)) {
                    current = static_cast<SelectablePart *>(LeftNumberList.GetTail());
                }
                unsigned short val = static_cast<unsigned short>(LeftDisplayValue - 1);
                LeftDisplayValue = static_cast<short>(val);
                if (val & 0x8000) {
                    LeftDisplayValue = 9;
                }
            }
        } else if (dir == eSD_NEXT) {
            if (!bLeft) {
                current = static_cast<SelectablePart *>(current->Next);
                if (current == reinterpret_cast<SelectablePart *>(&RightNumberList)) {
                    current = static_cast<SelectablePart *>(RightNumberList.GetHead());
                }
                RightDisplayValue = static_cast<short>(RightDisplayValue + 1);
                if (RightDisplayValue > 9) {
                    RightDisplayValue = 0;
                }
            } else {
                current = static_cast<SelectablePart *>(current->Next);
                if (current == reinterpret_cast<SelectablePart *>(&LeftNumberList)) {
                    current = static_cast<SelectablePart *>(LeftNumberList.GetHead());
                }
                short val = static_cast<short>(LeftDisplayValue + 1);
                LeftDisplayValue = val;
                if (val > 9) {
                    LeftDisplayValue = 0;
                }
            }
        }
        SelectablePart *prev;
        if (!bLeft) {
            prev = TheRightNumber;
        } else {
            prev = TheLeftNumber;
        }
        if (current != prev) {
            if (!bLeft) {
                TheRightNumber = current;
            } else {
                TheLeftNumber = current;
            }
            RefreshHeader();
        }
    }
}

void CustomizeNumbers::RefreshHeader() {
    DisplayHelper.DrawTitle();
    DisplayHelper.SetCareerStuff(TheLeftNumber, Category, 0);
    if (LeftDisplayValue != -1 && RightDisplayValue != -1) {
        FEObject *numGroup = FEngFindObject(GetPackageName(), 0x7a8355d9);
        FEngSetVisible(numGroup);
        SelectablePart tempPart(TheLeftNumber);
        eCustomizePartState state = CPS_AVAILABLE;
        if ((TheLeftNumber->PartState & CPS_GAME_STATE_MASK) == CPS_LOCKED &&
            (TheRightNumber->PartState & CPS_GAME_STATE_MASK) == CPS_LOCKED) {
            state = CPS_LOCKED;
        } else if ((TheLeftNumber->PartState & CPS_GAME_STATE_MASK) == CPS_NEW &&
                   (TheRightNumber->PartState & CPS_GAME_STATE_MASK) == CPS_NEW) {
            state = CPS_NEW;
        }
        if ((TheLeftNumber->PartState & CPS_INSTALLED) != 0 &&
            (TheRightNumber->PartState & CPS_INSTALLED) != 0) {
            state = static_cast<eCustomizePartState>(state | CPS_INSTALLED);
        } else if ((TheLeftNumber->PartState & CPS_IN_CART) != 0 &&
                   (TheRightNumber->PartState & CPS_IN_CART) != 0) {
            state = static_cast<eCustomizePartState>(state | CPS_IN_CART);
        }
        tempPart.PartState = state;
        unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), 1);
        DisplayHelper.SetPartStatus(&tempPart, unlockHash, 0, 0);
        FEPrintf(GetPackageName(), 0x2a08ba92, "%$d", static_cast<int>(LeftDisplayValue));
        FEPrintf(GetPackageName(), 0x1a88dc05, "%$d", static_cast<int>(RightDisplayValue));
        gCarCustomizeManager.PreviewPart(0x71, TheLeftNumber->ThePart);
        gCarCustomizeManager.PreviewPart(0x72, TheRightNumber->ThePart);
        gCarCustomizeManager.PreviewPart(0x69, TheLeftNumber->ThePart);
        gCarCustomizeManager.PreviewPart(0x6a, TheRightNumber->ThePart);
    } else {
        FEObject *numGroup = FEngFindObject(GetPackageName(), 0x7a8355d9);
        FEngSetInvisible(numGroup);
        ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(TheLeftNumber);
        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x71);
        if (!installed) {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_INSTALLED);
        } else if (!inCart || inCart->GetBuyingPart()->ThePart != nullptr) {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_AVAILABLE);
        } else {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_IN_CART);
        }
        FEPrintf(GetPackageName(), 0x2a08ba92, "-");
        FEPrintf(GetPackageName(), 0x1a88dc05, "-");
        gCarCustomizeManager.ResetPreview();
    }
}

void CustomizeNumbers::Setup() {
    DisplayHelper.TitleHash = 0x6857e5ac;
    gCarCustomizeManager.GetCarPartList(0x71, LeftNumberList, 0);
    gCarCustomizeManager.GetCarPartList(0x72, RightNumberList, 0);
    bool leftFound = false;
    int leftIdx = 0;
    CarPart *activeLeft = gCarCustomizeManager.GetActivePartFromSlot(0x71);
    SelectablePart *node = static_cast<SelectablePart *>(LeftNumberList.GetHead());
    while (node != reinterpret_cast<SelectablePart *>(&LeftNumberList)) {
        unsigned int attrVal = node->ThePart->GetAppliedAttributeUParam(0xebb03e66, 0);
        unsigned int expectedHash = bStringHash("NUMBER_LEFT");
        if (attrVal == expectedHash) {
            if (!leftFound) {
                if (bShowcaseOn == 1 && Showcase_FromIndex == leftIdx) {
                    TheLeftNumber = node;
                    if (gCarCustomizeManager.IsPartInCart(node)) {
                        TheLeftNumber->PartState = static_cast<eCustomizePartState>(TheLeftNumber->PartState | CPS_IN_CART);
                    }
                    LeftDisplayValue = static_cast<short>(leftIdx);
                    leftFound = true;
                    Showcase_FromIndex = 0;
                } else if (node->ThePart == activeLeft) {
                    TheLeftNumber = node;
                    if (gCarCustomizeManager.IsPartInCart(node)) {
                        TheLeftNumber->PartState = static_cast<eCustomizePartState>(TheLeftNumber->PartState | CPS_IN_CART);
                    }
                    LeftDisplayValue = static_cast<short>(leftIdx);
                }
            }
            leftIdx++;
            node = static_cast<SelectablePart *>(node->Next);
        } else {
            SelectablePart *next = static_cast<SelectablePart *>(node->Next);
            node->Prev->Next = node->Next;
            node->Next->Prev = node->Prev;
            delete node;
            node = next;
        }
    }
    if (!TheLeftNumber) {
        LeftDisplayValue = -1;
        TheLeftNumber = static_cast<SelectablePart *>(LeftNumberList.GetHead());
    }

    bool rightFound = false;
    int rightIdx = 0;
    CarPart *activeRight = gCarCustomizeManager.GetActivePartFromSlot(0x72);
    node = static_cast<SelectablePart *>(RightNumberList.GetHead());
    while (node != reinterpret_cast<SelectablePart *>(&RightNumberList)) {
        unsigned int attrVal = node->ThePart->GetAppliedAttributeUParam(0xebb03e66, 0);
        unsigned int expectedHash = bStringHash("NUMBER_RIGHT");
        if (attrVal == expectedHash) {
            if (!rightFound) {
                if (bShowcaseOn == 1 && Showcase_FromFilter == rightIdx) {
                    TheRightNumber = node;
                    if (gCarCustomizeManager.IsPartInCart(node)) {
                        TheRightNumber->PartState = static_cast<eCustomizePartState>(TheRightNumber->PartState | CPS_IN_CART);
                    }
                    RightDisplayValue = static_cast<short>(rightIdx);
                    rightFound = true;
                    Showcase_FromFilter = -1;
                } else if (node->ThePart == activeRight) {
                    TheRightNumber = node;
                    if (gCarCustomizeManager.IsPartInCart(node)) {
                        TheRightNumber->PartState = static_cast<eCustomizePartState>(TheRightNumber->PartState | CPS_IN_CART);
                    }
                    RightDisplayValue = static_cast<short>(rightIdx);
                }
            }
            rightIdx++;
            node = static_cast<SelectablePart *>(node->Next);
        } else {
            SelectablePart *next = static_cast<SelectablePart *>(node->Next);
            node->Prev->Next = node->Next;
            node->Next->Prev = node->Prev;
            delete node;
            node = next;
        }
    }
    if (!TheRightNumber) {
        RightDisplayValue = -1;
        TheRightNumber = static_cast<SelectablePart *>(RightNumberList.GetHead());
    }
    RefreshHeader();
}

void CustomizeNumbers::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        unsigned int newSide;
        newSide = bLeft ^ 1;
        bLeft = newSide;
        unsigned int hash;
        if (newSide) {
            hash = 0x2a08ba92;
        } else {
            hash = 0x1a88dc05;
        }
        FEngSetCurrentButton(GetPackageName(), hash);
    } else if (msg == 0x5a928018) {
        ShoppingCartItem *leftInCart = gCarCustomizeManager.IsPartTypeInCart(0x69u);
        ShoppingCartItem *rightInCart = gCarCustomizeManager.IsPartTypeInCart(0x6au);
        if (!leftInCart && !rightInCart) {
            SelectablePart *lnode = static_cast<SelectablePart *>(LeftNumberList.GetHead());
            while (lnode != reinterpret_cast<SelectablePart *>(&LeftNumberList)) {
                if ((lnode->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART) {
                    lnode->PartState = static_cast<eCustomizePartState>(lnode->PartState & CPS_GAME_STATE_MASK);
                    break;
                }
                lnode = static_cast<SelectablePart *>(lnode->Next);
            }
            SelectablePart *rnode = static_cast<SelectablePart *>(RightNumberList.GetHead());
            while (rnode != reinterpret_cast<SelectablePart *>(&RightNumberList)) {
                if ((rnode->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART) {
                    rnode->PartState = static_cast<eCustomizePartState>(rnode->PartState & CPS_GAME_STATE_MASK);
                    break;
                }
                rnode = static_cast<SelectablePart *>(rnode->Next);
            }
        }
        RefreshHeader();
    } else if (msg == 0x35f8620b) {
        bLeft = 1;
        FEngSetCurrentButton(GetPackageName(), 0x2a08ba92);
    } else if (msg == 0x406415e3) {
        if (LeftDisplayValue == -1 || RightDisplayValue == -1) return;
        if (!TheLeftNumber || !TheRightNumber) return;
        eCustomizePartState leftState = TheLeftNumber->PartState;
        eCustomizePartState rightState = TheRightNumber->PartState;
        eCustomizePartState status;
        if ((leftState & CPS_GAME_STATE_MASK) == CPS_LOCKED && (rightState & CPS_GAME_STATE_MASK) == CPS_LOCKED) {
            status = CPS_LOCKED;
        } else if ((leftState & CPS_IN_CART) != 0 && (rightState & CPS_IN_CART) != 0) {
            status = CPS_IN_CART;
        } else if ((leftState & CPS_INSTALLED) != 0 && (rightState & CPS_INSTALLED) != 0) {
            status = CPS_INSTALLED;
        } else {
            cFEng_mInstance->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xff);
            return;
        }
        DisplayHelper.FlashStatusIcon(status, true);
    } else if (msg == 0x72619778) {
        ScrollNumbers(eSD_NEXT);
    } else if (msg == 0x911ab364) {
        bShowcaseOn = 0;
        cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
    } else if (msg == 0x911c0a4b) {
        ScrollNumbers(eSD_PREV);
    } else if (msg == 0x91dfdf84) {
        UnsetShoppingCart();
        TheLeftNumber->PartState = static_cast<eCustomizePartState>(TheLeftNumber->PartState | CPS_IN_CART);
        TheRightNumber->PartState = static_cast<eCustomizePartState>(TheRightNumber->PartState | CPS_IN_CART);
        gCarCustomizeManager.AddToCart(TheLeftNumber);
        gCarCustomizeManager.AddToCart(TheRightNumber);
        SelectablePart *copyLeft = new SelectablePart(TheLeftNumber);
        SelectablePart *copyRight = new SelectablePart(TheRightNumber);
        copyLeft->Price = 0;
        copyLeft->PartState = static_cast<eCustomizePartState>((copyLeft->PartState & CPS_GAME_STATE_MASK) | CPS_IN_CART);
        copyRight->PartState = static_cast<eCustomizePartState>((copyRight->PartState & CPS_GAME_STATE_MASK) | CPS_IN_CART);
        copyLeft->CarSlotID = 0x69;
        copyRight->CarSlotID = 0x6a;
        copyRight->Price = 0;
        gCarCustomizeManager.AddToCart(copyLeft);
        gCarCustomizeManager.AddToCart(copyRight);
        delete copyLeft;
        delete copyRight;
        RefreshHeader();
    } else if (msg == 0xb5af2461) {
        CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
    } else if (msg == 0xc519bfbf) {
        Showcase_FromFilter = static_cast<int>(RightDisplayValue);
        Showcase_FromIndex = static_cast<int>(LeftDisplayValue);
        Showcase_FromArgs = Category | (FromCategory << 16);
        Showcase_FromPackage = GetPackageName();
        bShowcaseOn = 1;
        cFEng_mInstance->QueuePackageSwitch("Showcase.fng", gCarCustomizeManager.TuningCar->FEKey, 0, false);
    } else if (msg == 0xc519bfc3) {
        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x71);
        if (!installed) {
            if ((TheLeftNumber->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART ||
                (TheRightNumber->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART) {
                UnsetShoppingCart();
                ShoppingCartItem *cartNode = gCarCustomizeManager.ShoppingCart.GetHead();
                while (cartNode) {
                    ShoppingCartItem *nextCart = static_cast<ShoppingCartItem *>(cartNode->Next);
                    int slotID = cartNode->GetBuyingPart()->GetSlotID();
                    if (slotID == 0x71 || slotID == 0x72 || slotID == 0x69 || slotID == 0x6a) {
                        gCarCustomizeManager.RemoveFromCart(cartNode);
                    }
                    bool more = (cartNode != gCarCustomizeManager.ShoppingCart.GetTail());
                    cartNode = nextCart;
                    if (!more) break;
                }
            }
        } else {
            UnsetShoppingCart();
            SelectablePart stockPart(nullptr, 0x71, 0, static_cast<GRace::Type>(7), false, CPS_AVAILABLE, 0, false);
            gCarCustomizeManager.AddToCart(&stockPart);
            stockPart.CarSlotID = 0x72;
            gCarCustomizeManager.AddToCart(&stockPart);
            stockPart.CarSlotID = 0x69;
            gCarCustomizeManager.AddToCart(&stockPart);
            stockPart.CarSlotID = 0x6a;
            gCarCustomizeManager.AddToCart(&stockPart);
        }
        RightDisplayValue = -1;
        TheLeftNumber = static_cast<SelectablePart *>(LeftNumberList.GetHead());
        TheRightNumber = static_cast<SelectablePart *>(RightNumberList.GetHead());
        LeftDisplayValue = -1;
        RefreshHeader();
    } else if (msg == 0xcf91aacd) {
        SelectablePart *lnode = static_cast<SelectablePart *>(LeftNumberList.GetHead());
        while (lnode != reinterpret_cast<SelectablePart *>(&LeftNumberList)) {
            CarPart *lpart = lnode->ThePart;
            if (lpart == gCarCustomizeManager.GetInstalledCarPart(0x69) ||
                lpart == gCarCustomizeManager.GetInstalledCarPart(0x6a)) {
                lnode->PartState = static_cast<eCustomizePartState>((lnode->PartState & CPS_GAME_STATE_MASK) | CPS_INSTALLED);
            }
            lnode = static_cast<SelectablePart *>(lnode->Next);
        }
        SelectablePart *rnode = static_cast<SelectablePart *>(RightNumberList.GetHead());
        while (rnode != reinterpret_cast<SelectablePart *>(&RightNumberList)) {
            CarPart *rpart = rnode->ThePart;
            if (rpart == gCarCustomizeManager.GetInstalledCarPart(0x71) ||
                rpart == gCarCustomizeManager.GetInstalledCarPart(0x72)) {
                rnode->PartState = static_cast<eCustomizePartState>((rnode->PartState & CPS_GAME_STATE_MASK) | CPS_INSTALLED);
            }
            rnode = static_cast<SelectablePart *>(rnode->Next);
        }
        CustomizeShoppingCart::ExitShoppingCart();
    }
}

// --- CustomizeDecals ---

unsigned int CustomizeDecals::GetSlotIDFromCategory() {
    if (CurrentDecalLocation == 0x503) {
        switch (Category) {
        case 0x601: return 99;
        case 0x602: return 100;
        case 0x603: return 0x65;
        case 0x604: return 0x66;
        case 0x605: return 0x67;
        case 0x606: return 0x68;
        default: break;
        }
    } else if (CurrentDecalLocation == 0x501) {
        return 0x53;
    } else if (CurrentDecalLocation == 0x502) {
        return 0x5b;
    } else if (CurrentDecalLocation == 0x505) {
        return 0x73;
    } else if (CurrentDecalLocation == 0x506) {
        return 0x7b;
    } else if (CurrentDecalLocation == 0x504) {
        switch (Category) {
        case 0x601: return 0x6b;
        case 0x602: return 0x6c;
        case 0x603: return 0x6d;
        case 0x604: return 0x6e;
        case 0x605: return 0x6f;
        case 0x606: return 0x70;
        default: break;
        }
    } else {
        return 0x53;
    }
    return 0x73;
}

void CustomizeDecals::Setup() {
    unsigned int slotID = GetSlotIDFromCategory();
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    unsigned int titleHash = 0;
    switch (Category) {
    case 0x501: titleHash = 0x301dedd3; break;
    case 0x502: titleHash = 0x48e6ca49; break;
    case 0x505: titleHash = 0x8a7697d6; break;
    case 0x506: titleHash = 0xb1f9b0c9; break;
    case 0x601: titleHash = 0x7d212cfa; break;
    case 0x602: titleHash = 0x7d212cfb; break;
    case 0x603: titleHash = 0x7d212cfc; break;
    case 0x604: titleHash = 0x7d212cfd; break;
    case 0x605: titleHash = 0x7d212cfe; break;
    case 0x606: titleHash = 0x7d212cff; break;
    default: break;
    }
    DisplayHelper.TitleHash = titleHash;
    ShoppingCartItem *cartItem = gCarCustomizeManager.IsPartTypeInCart(slotID);
    unsigned int selectedHash = 0;
    CarPart *activePart = nullptr;
    if (cartItem && (activePart = cartItem->GetBuyingPart()->GetPart(), activePart)) {
    } else {
        activePart = gCarCustomizeManager.GetInstalledCarPart(slotID);
    }
    if (activePart) {
        unsigned int brand = activePart->GetAppliedAttributeUParam(0xebb03e66, 0);
        unsigned int mirrorHash = activePart->GetAppliedAttributeUParam(bStringHash("DECAL_MIRROR_HASH"), 0);
        bIsBlack = (brand == mirrorHash);
        selectedHash = activePart->GetAppliedAttributeUParam(0xebb03e66, 0);
    }
    if (Showcase_FromFilter != -1) {
        bIsBlack = (Showcase_FromFilter != 0);
        Showcase_FromFilter = -1;
    }
    BuildDecalList(selectedHash);
    RefreshHeader();
}

void CustomizeDecals::BuildDecalList(unsigned int selected_name_hash) {
    int matchIdx = 0;
    int curIdx = 1;
    CarPart *activeMatch = nullptr;
    unsigned int slotID = GetSlotIDFromCategory();
    bTList<SelectablePart> tempList;
    unsigned int filterHash = 0;
    if (bIsBlack) {
        filterHash = bStringHash("DECAL_MIRROR_HASH");
    }
    gCarCustomizeManager.GetCarPartList(slotID, tempList, filterHash);
    if (Showcase_FromIndex == 0) {
        activeMatch = gCarCustomizeManager.GetActivePartFromSlot(slotID);
    }
    SetStockPartOption *stockOpt = new SetStockPartOption(nullptr, 0x21f3d114, 0x60a662f5);
    SelectablePart *stockPart = new SelectablePart(nullptr, slotID, 0, static_cast<GRace::Type>(7), false, static_cast<eCustomizePartState>(1), 0, false);
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockPart->PartState = static_cast<eCustomizePartState>(0x10);
    }
    stockOpt->ThePart = stockPart;
    AddOption(stockOpt);
    SelectablePart *node = tempList.GetHead();
    while (node != reinterpret_cast<SelectablePart *>(&tempList)) {
        SelectablePart *next = static_cast<SelectablePart *>(static_cast<bTNode<SelectablePart> *>(node)->GetNext());
        unsigned int nameHash = node->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0);
        if (selected_name_hash != 0 && nameHash == selected_name_hash) {
            matchIdx = curIdx;
        }
        unsigned char gl = *reinterpret_cast<unsigned char *>(reinterpret_cast<int>(node->ThePart) + 5);
        unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), gl >> 5);
        bool locked = gCarCustomizeManager.IsPartLocked(node, 0);
        AddPartOption(node, 0x294d2a3, gl >> 5, 0, unlockHash, locked);
        curIdx++;
        node = next;
    }
    if (Showcase_FromIndex == 0) {
        if (bFadeInIconsImmediately) {
            Options.bFadingOut = false;
            Options.bFadingIn = true;
            Options.bDelayUpdate = false;
            Options.fCurFadeTime = 0.0f;
        }
        Options.SetInitialPos(matchIdx);
    } else {
        if (bFadeInIconsImmediately) {
            Options.bFadingIn = true;
            Options.bFadingOut = false;
            Options.bDelayUpdate = false;
            Options.fCurFadeTime = 0.0f;
        }
        Options.SetInitialPos(Showcase_FromIndex - 1);
        Showcase_FromIndex = 0;
    }
    RefreshHeader();
}

void CustomizeDecals::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0x911ab364) {
        cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, FromCategory | Category << 16, 0, false);
    } else if (msg == 0x5a928018) {
        SelectablePart *sel = GetSelectedPart();
        if (sel) {
            if (gCarCustomizeManager.IsPartInCart(sel)) {
                return;
            }
            sel->PartState = static_cast<eCustomizePartState>(sel->PartState & 0xf);
            RefreshHeader();
        }
    } else if (msg == 0x5073ef13) {
        // do nothing
    } else if (msg == 0xc519bfbf) {
        Showcase_FromFilter = bIsBlack;
    } else if (msg == 0xc519bfc3) {
        return;
    } else if (msg == 0xd9feec59) {
        bIsBlack ^= 1;
        CustomizePartOption *opt = GetSelectedOption();
        if (!opt->GetPart()) {
            BuildDecalList(0);
        } else {
            unsigned int nameHash = opt->GetPart()->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0);
            BuildDecalList(nameHash);
        }
        RefreshHeader();
    } else {
        return;
    }
}

void CustomizeDecals::RefreshHeader() {
    DisplayHelper.DrawTitle();
    CustomizationScreen::RefreshHeader();
    CustomizePartOption *opt = GetSelectedOption();
    if (opt) {
        SelectablePart *sel = opt->GetPart();
        if (sel) {
            gCarCustomizeManager.PreviewPart(sel->CarSlotID, sel->GetPart());
        } else {
            gCarCustomizeManager.PreviewPart(GetSlotIDFromCategory(), nullptr);
        }
    }
}

// --- CustomizePaint helpers ---

unsigned int CustomizePaint::CalcBrandHash(CarPart *part) {
    if (!part) {
        if (Category == 0x301 || Category == 0x303) {
            return 0;
        }
        return 0;
    }
    return part->GetAppliedAttributeUParam(0xebb03e66, 0);
}

CustomizePaint::CustomizePaint(ScreenConstructorData *sd)
    : CustomizationScreen(sd) //
    , TheFilter(-1) //
    , MatchingPaint(nullptr, 0, 0, 0, 0) //
    , ThePaints(GetPackageName(), 5, 16, false) {
    VinylColors[0] = nullptr;
    VinylColors[1] = nullptr;
    VinylColors[2] = nullptr;
    NumRemapColors = 0;
}

void CustomizePaint::Setup() {
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    for (int i = 1; i <= 0x50; i++) {
        ArraySlot *slot = new ArraySlot(FEngFindImage(GetPackageName(), FEngHashString("COLOUR_%d", i)));
        ThePaints.AddSlot(slot);
    }
    for (int i = 0; i < 3; i++) {
        SelectedIndex[i] = -1;
    }
    if (Showcase_FromFilter != -1) {
        TheFilter = Showcase_FromFilter;
    }
    if (Category == 0x303) {
        DisplayHelper.TitleHash = 0xe126ff53;
        SetupRimPaint();
    } else if (Category == 0x301) {
        cFEng::Get()->QueuePackageMessage(0x1a7240f3, GetPackageName(), nullptr);
        DisplayHelper.TitleHash = 0x55da70c;
        SetupBasePaint();
    } else if (static_cast<unsigned int>(Category) > 0x401 && static_cast<unsigned int>(Category) < 0x40a) {
        DisplayHelper.TitleHash = 0xd8ee1a80;
        SetupVinylColor();
    }
    Showcase_FromFilter = -1;
    Options.bFadingIn = true;
    RefreshHeader();
}

void CustomizePaint::AddVinylAndColorsToCart() {
    SelectablePart *mainPart = gCarCustomizeManager.GetTempColoredPart();
    gCarCustomizeManager.AddToCart(mainPart);
    for (int i = 0; i < NumRemapColors; i++) {
        if (VinylColors[i]) {
            gCarCustomizeManager.AddToCart(VinylColors[i]);
            VinylColors[i] = nullptr;
        }
    }
}

eMenuSoundTriggers CustomizePaint::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    CustomizationScreen::NotifySoundMessage(msg, maybe);
    if (Category != 0x301 && Category != 0x303) {
        if (msg == 0xc407210 || msg == 0xd9feec59 || msg == 0x5073ef13) {
            return static_cast<eMenuSoundTriggers>(0);
        }
    }
    return maybe;
}

#endif // FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_CARCUSTOMIZE_H
