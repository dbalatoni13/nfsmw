#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"

struct FECarRecord;

static bool gInBackRoom;
static bool gInPerformance;
static bool gInParts;

bool CustomizeIsInBackRoom() {
    return gInBackRoom;
}
void CustomizeSetInBackRoom(bool b) {
    gInBackRoom = b;
}
bool CustomizeIsInPerformance() {
    return gInPerformance;
}
void CustomizeSetInPerformance(bool b) {
    gInPerformance = b;
}
bool CustomizeIsInParts() {
    return gInParts;
}
void CustomizeSetInParts(bool b) {
    gInParts = b;
}

extern int g_TheCustomizeEntryPoint;
extern FECarRecord *g_pCustomizeCarRecordToUse;

void BeginCarCustomize(eCustomizeEntryPoint entry_point, FECarRecord *theCustomCar) {
    CustomizeSetInBackRoom(false);
    CustomizeSetInPerformance(false);
    CustomizeSetInParts(false);
    if (entry_point) {
        cFEng::Get()->QueuePackageSwitch("CustomizeMain.fng", 0, 0, false);
    }
    g_TheCustomizeEntryPoint = entry_point;
    g_pCustomizeCarRecordToUse = theCustomCar;
}

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

namespace Attrib {
namespace Gen {
struct pvehicle;
}
} // namespace Attrib
namespace Physics {
namespace Info {
float Redline(const Attrib::Gen::pvehicle &pvehicle);
}
} // namespace Physics

#include <types.h>

struct EAXSound;

extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern FEObject *FEngFindObject(const char *pkg, unsigned int hash);
extern FEImage *FEngFindImage(const char *pkg, int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern void FEngSetScript(const char *pkg, unsigned int hash, unsigned int script, bool b);
extern void FEngSetScript(FEObject *obj, unsigned int script, bool b);
extern void FEngSetLanguageHash(const char *pkg, unsigned int obj_hash, unsigned int lang_hash);
extern void FEngSetCurrentButton(const char *pkg, unsigned int hash);
inline void FEngSetCurrentButton(const char *pkg_name, FEObject *obj) {
    FEngSetCurrentButton(pkg_name, obj->NameHash);
}
extern void FEngSetTopLeft(FEObject *obj, float x, float y);
extern void FEngGetTopLeft(FEObject *obj, float &x, float &y);
extern void FEngGetSize(FEObject *obj, float &x, float &y);

inline float FEngGetTopLeftX(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return x;
}

inline float FEngGetTopLeftY(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return y;
}
extern void FEngSetBottomRight(FEObject *obj, float x, float y);
extern void FEngGetBottomRight(FEObject *obj, float &x, float &y);
extern bool CustomizeIsInPerformance();
extern bool CustomizeIsInParts();
extern void CustomizeSetInParts(bool b);
extern void CustomizeSetInPerformance(bool b);
extern const char *GetLocalizedString(unsigned int hash);
extern void GetLocalizedString(char *buf, int size, unsigned int hash);
extern int FEPrintf(const char *pkg, int hash, const char *fmt, ...);
extern int FEPrintf(FEString *text, const char *fmt, ...);
extern int bSNPrintf(char *buf, int size, const char *fmt, ...);
extern int bSPrintf(char *buf, const char *fmt, ...);
extern int bStrLen(const char *s);

extern CarCustomizeManager gCarCustomizeManager;
extern cFrontendDatabase *FEDatabase;
extern cFEng *cFEng_mInstance;

extern const char *g_pCustomizeMainPkg;
const char *g_pCustomizeSubPkg = nullptr;
const char *g_pCustomizeSubTopPkg = nullptr;
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
extern unsigned int bStringHash(const char *str, int prefix_hash);

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
extern const char g_pCustomizeDlgPkg[];
extern void StartCareerFreeRoam();
extern char FEngMapJoyParamToJoyport(unsigned long param);
extern void *MemoryCard_s_pThis;

class GarageMainScreen;
extern GarageMainScreen *GetInstance_GarageMainScreen();

inline void CustomizeFEngSetVisible(const char *pkg_name, unsigned int obj_hash) {
    FEngSetVisible(FEngFindObject(pkg_name, obj_hash));
}

inline void CustomizeFEngSetInvisible(const char *pkg_name, unsigned int obj_hash) {
    FEngSetInvisible(FEngFindObject(pkg_name, obj_hash));
}

inline void CustomizeFEngSetTextureHash(const char *pkg_name, unsigned int obj_hash, unsigned int texture_hash) {
    FEngSetTextureHash(FEngFindImage(pkg_name, obj_hash), texture_hash);
}

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

CustomizeCategoryScreen::~CustomizeCategoryScreen() {}

void CustomizeCategoryScreen::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    int status = static_cast<CustomizeMainOption *>(Options.GetCurrentOption())->UnlockStatus;
    if (status == 2) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xcffb7033), 0xf0574bb2);
        FEngSetScript(GetPackageName(), 0xcffb7033, 0x5079c8f8, true);
    } else if (status == 3) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xcffb7033));
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xcffb7033), 0xcffb7033);
        FEngSetScript(GetPackageName(), 0xcffb7033, 0x5079c8f8, true);
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xcffb7033));
    }
    CarCustomizeManager &mgr = gCarCustomizeManager;
    if (mgr.IsCareerMode()) {
        CustomizeMeter *meter = &HeatMeter;
        meter->SetCurrent(mgr.GetActualHeat());
        meter->SetPreview(mgr.GetCartHeat());
        meter->Draw();
        if (CustomizeIsInBackRoom()) {
            FEngSetLanguageHash(GetPackageName(), 0x63ca8308, GetMarkerNameFromCategory(static_cast<eCustomizeCategory>(Category)));
            const char *fmt = "%d";
            FEPrintf(GetPackageName(), 0x83e3cd39, fmt, GetNumMarkersFromCategory(static_cast<eCustomizeCategory>(Category)));
            FEPrintf(GetPackageName(), 0x23d918fe, fmt, TheFEMarkerManager.GetNumCustomizeMarkers());
        } else {
            FEPrintf(GetPackageName(), 0x7a6d2f71, "%d", mgr.GetCartTotal(CCT_TOTAL));
            FEPrintf(GetPackageName(), 0xc60adcfd, "%d", FEDatabase->GetCareerSettings()->GetCash());
        }
    } else {
        HeatMeter.SetVisibility(false);
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x8d1559a4));
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

void CustomizationScreen::AddPartOption(SelectablePart *part, unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash,
                                        unsigned int unlock_hash, bool locked) {
    CustomizePartOption *opt = new CustomizePartOption(part, tex_hash, name_hash, desc_hash, unlock_hash);
    AddOption(opt);
    opt->SetLocked(locked);
}

CustomizePartOption *CustomizationScreen::FindMatchingOption(SelectablePart *to_find) {
    IconOption *tail = Options.TailBookEnd;
    IconOption *cur = Options.HeadBookEnd->GetNext();
    for (;;) {
        bool atEnd = (cur == tail) || (cur == Options.HeadBookEnd);
        if (atEnd)
            break;
        SelectablePart *part = static_cast<CustomizePartOption *>(cur)->ThePart;
        if (to_find->PerformancePkg) {
            if (part->PhysicsType == to_find->PhysicsType && part->UpgradeLevel == to_find->UpgradeLevel) {
                return static_cast<CustomizePartOption *>(cur);
            }
        } else {
            if (part->ThePart == to_find->ThePart) {
                return static_cast<CustomizePartOption *>(cur);
            }
        }
        cur = cur->GetNext();
    }
    return nullptr;
}

// --- FEShoppingCartItem ---

void FEShoppingCartItem::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), 0x249db7b7, true);
    FEngSetScript(GetDataObject(), 0x249db7b7, true);
    FEngSetScript(pTradeInPrice, 0x249db7b7, true);
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
        FEngSetScript(GetBacking(), 0x249db7b7, true);
    }
}

void FEShoppingCartItem::UnsetFocus() {
    unsigned int script = 0x7ab5521a;
    if (!TheItem->IsActive()) {
        script = 0x163c76;
    }
    FEngSetScript(pTitle, script, true);
    FEngSetScript(pData, script, true);
    FEngSetScript(pTradeInPrice, script, true);
    if (pBacking) {
        FEngSetInvisible(pBacking);
        FEngSetScript(pBacking, 0x7ab5521a, true);
    }
}

void FEShoppingCartItem::SetCheckScripts() {
    if (TheItem->IsActive()) {
        FEngSetScript(pCheckIcon, 0xe6361f46, true);
    } else {
        FEngSetScript(pCheckIcon, 0x77cdc4e9, true);
    }
}

void FEShoppingCartItem::SetActiveScripts() {
    if (!TheItem->IsActive()) {
        FEngSetScript(pCheckIcon, 0x163c76, true);
    }
}

void FEShoppingCartItem::Draw() {
    if (TheItem->IsActive()) {
        FEngSetTextureHash(pCheckIcon, 0x696ae039);
    } else {
        FEngSetTextureHash(pCheckIcon, 0xe719881c);
    }
    DrawPartName();
    if (TheItem->GetTradeInPart() && gCarCustomizeManager.IsCareerMode() && !CustomizeIsInBackRoom()) {
        int tradeIn = TheItem->GetTradeInPart()->GetPrice();
        if (tradeIn == 0) {
            tradeIn = 0;
        } else {
            tradeIn = static_cast<int>(static_cast<float>(tradeIn) * gTradeInFactor);
        }
        FEPrintf(pTradeInPrice, "%d", tradeIn);
    } else {
        FEPrintf(pTradeInPrice, "");
    }
    if (gCarCustomizeManager.IsCareerMode() && !CustomizeIsInBackRoom()) {
        FEPrintf(pData, "%d", TheItem->GetBuyingPart()->GetPrice());
    } else {
        FEPrintf(pData, "");
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
        case Physics::Upgrades::kType_Tires:
            hash = 0x5aa9137;
            break;
        case Physics::Upgrades::kType_Brakes:
            hash = 0x91997ee8;
            break;
        case Physics::Upgrades::kType_Chassis:
            hash = 0x6e101aa7;
            break;
        case Physics::Upgrades::kType_Transmission:
            hash = 0x29aa74ba;
            break;
        case Physics::Upgrades::kType_Engine:
            hash = 0x9853d9a6;
            break;
        case Physics::Upgrades::kType_Induction:
            if (gCarCustomizeManager.IsTurbo()) {
                hash = 0x5b1255c;
            } else {
                hash = 0xbb6812bb;
            }
            break;
        case Physics::Upgrades::kType_Nitrous:
            hash = 0x4ce19aa4;
            break;
        default:
            break;
    }
    return hash;
}

unsigned int FEShoppingCartItem::GetPerfPkgLevelHash(int level) {
    switch (level) {
        case 0:
            return 0x69c270c3;
        case 1:
            return 0x69c270c4;
        case 2:
            return 0x69c270c5;
        case 3:
            return 0x69c270c6;
        case 4:
            return 0x69c270c7;
        case 5:
            return 0x69c270c8;
        case 6:
            return 0x69c270c9;
        default:
            return 0x69c270c3;
    }
}

unsigned int FEShoppingCartItem::GetCarPartCatHash(unsigned int slot_id) {
    switch (slot_id) {
        case 0x17:
            return 0x6134c218;
        case 0x2c:
            return 0x94e73021;
        case 0x3e:
            return 0x61e8f83c;
        case 0x3f:
            return 0x4d4a88d;
        case 0x42:
            return 0xf868eb0b;
        case 0x4c:
            return 0x55da70c;
        case 0x84:
            return 0x78980a6b;
        case 0x83:
            return 0xd32729a6;
        case 0x4e:
            return 0xe126ff53;
        case 0x53:
            return 0x301dedd3;
        case 0x5b:
            return 0x48e6ca49;
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
            return 0x34367c86;
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
            return 0xddf80259;
        case 0x73:
            return 0x8a7697d6;
        case 0x7b:
            return 0xb1f9b0c9;
        case 0x71:
            return 0x6857e5ac;
        case 0x4d:
            return 0xbfa52c55;
        default:
            return 0;
    }
}

void FEShoppingCartItem::DrawPartName() {
    SelectablePart *buyPart = TheItem->GetBuyingPart();
    if (buyPart->PerformancePkg) {
        unsigned int level = buyPart->UpgradeLevel;
        Physics::Upgrades::Type phys_type = static_cast<Physics::Upgrades::Type>(static_cast<int>(buyPart->PhysicsType));
        if (static_cast<int>(level) == 7) {
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)), GetLocalizedString(0xedd14807));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)), GetLocalizedString(0xedd14807));
            }
        } else {
            int numPkgs = gCarCustomizeManager.GetNumPackages(phys_type);
            int displayLevel = static_cast<int>(level) - (numPkgs - 6);
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)),
                         GetLocalizedString(GetPerfPkgLevelHash(displayLevel)));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)),
                         GetLocalizedString(GetPerfPkgLevelHash(displayLevel)));
            }
        }
        return;
    }

    switch (buyPart->CarSlotID) {
        case 0x4e: {
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0xb3100a3e),
                         GetLocalizedString(buyPart->ThePart->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0xb3100a3e),
                         GetLocalizedString(buyPart->ThePart->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            }
            return;
        }

        case 0x4c: {
            unsigned int paint_type = buyPart->ThePart->GetAppliedAttributeUParam(0xebb03e66, 0);
            unsigned int colorHash = 0x452b5481;
            if (paint_type == 0x2daab07) {
                colorHash = 0xb6763cde;
            } else if (paint_type > 0x2daab07) {
                if (paint_type != 0x3437a52) {
                    if (paint_type == 0x3797533) {
                        colorHash = 0xb715070a;
                    }
                }
            } else if (paint_type == 0xda27) {
                colorHash = 0xb3100a3e;
            }
            const char *fmt;
            if (GetCurrentLanguage() == 1) {
                fmt = "%s : %s %s";
            } else {
                fmt = "%s: %s %s";
            }
            FEPrintf(GetTitleObject(), fmt, GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(colorHash),
                     GetLocalizedString(buyPart->ThePart->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            return;
        }

        case 0x71: {
            ShoppingCartItem *leftItem = gCarCustomizeManager.IsPartTypeInCart(0x71);
            ShoppingCartItem *rightItem = gCarCustomizeManager.IsPartTypeInCart(0x72);
            if (!leftItem)
                return;
            if (!rightItem)
                return;
            CarPart *left_part = leftItem->GetBuyingPart()->ThePart;
            CarPart *right_part = rightItem->GetBuyingPart()->ThePart;
            if (!left_part) {
                goto missing_parts;
            }
            if (!right_part) {
                goto missing_parts;
            }
            const char *fmt;
            if (GetCurrentLanguage() == 1) {
                fmt = "%s : %s%s";
            } else {
                fmt = "%s: %s%s";
            }
            FEPrintf(GetTitleObject(), fmt, GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), left_part->GetName(), right_part->GetName());
            return;
        missing_parts: {
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0xbe434a38));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0xbe434a38));
            }
            return;
        }
        }

        case 0x53:
        case 0x5b:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x73:
        case 0x7b: {
            if (!buyPart->ThePart) {
                const char *fmt;
                if (GetCurrentLanguage() == 1) {
                    fmt = "%s : %s - %s";
                } else {
                    fmt = "%s: %s - %s";
                }
                FEPrintf(GetTitleObject(), fmt, GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                         GetLocalizedString(0x7177dc17));
                return;
            }
            buyPart->ThePart->GetAppliedAttributeUParam(0xebb03e66, 0);
            buyPart->ThePart->GetAppliedAttributeUParam(bStringHash("NAME"), 0);
            unsigned int subCatHash = 0;
            switch (buyPart->CarSlotID) {
                case 0x63:
                case 0x6b:
                    subCatHash = 0x7d212cfa;
                    break;
                case 0x64:
                case 0x6c:
                    subCatHash = 0x7d212cfb;
                    break;
                case 0x65:
                case 0x6d:
                    subCatHash = 0x7d212cfc;
                    break;
                case 0x66:
                case 0x6e:
                    subCatHash = 0x7d212cfd;
                    break;
                case 0x67:
                case 0x6f:
                    subCatHash = 0x7d212cfe;
                    break;
                case 0x68:
                case 0x70:
                    subCatHash = 0x7d212cff;
                    break;
            }
            if (subCatHash) {
                const char *fmt;
                if (GetCurrentLanguage() == 1) {
                    fmt = "%s : %s %s %s";
                } else {
                    fmt = "%s: %s %s %s";
                }
                FEPrintf(GetTitleObject(), fmt, GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                         GetLocalizedString(subCatHash), buyPart->ThePart->GetName());
                return;
            }
            const char *fmt;
            if (GetCurrentLanguage() == 1) {
                fmt = "%s : %s %s";
            } else {
                fmt = "%s: %s %s";
            }
            FEPrintf(GetTitleObject(), fmt, GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                     buyPart->ThePart->GetName());
            return;
        }

        case 0x17: {
            CarPart *part = buyPart->ThePart;
            const char *lang_str = "LANGUAGEHASH";
            if (part->HasAppliedAttribute(bStringHash(lang_str))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            }
            return;
        }

        case 0x42: {
            CarPart *car_part = buyPart->ThePart;
            CarPart *stock = gCarCustomizeManager.GetStockCarPart(0x42);
            if (car_part != stock) {
                char buf[64];
                bSNPrintf(buf, 64, "%s", car_part->GetName());
                int len = bStrLen(buf);
                if (len < 1)
                    return;
                int trimStart = len - 6;
                for (; trimStart <= len; len--) {
                    buf[len] = 0;
                }
                const char *fmt;
                if (GetCurrentLanguage() == 1) {
                    fmt = "%s : %s %$d\"";
                } else {
                    fmt = "%s: %s %$d\"";
                }
                FEPrintf(GetTitleObject(), fmt, GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), buf,
                         static_cast<signed char>(car_part->GetAppliedAttributeIParam(0xeb0101e2, 0)));
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0x60a662f5));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0x60a662f5));
            }
            return;
        }

        case 0x2c:
        case 0x3e:
        case 0x3f: {
            CarPart *part = buyPart->ThePart;
            if (part->HasAppliedAttribute(bStringHash("CARBONFIBRE"))) {
                if (part->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0) != 0) {
                    const char *fmt;
                    if (GetCurrentLanguage() == 1) {
                        fmt = "%s : %s %s";
                    } else {
                        fmt = "%s: %s %s";
                    }
                    FEPrintf(GetTitleObject(), fmt, GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0x5415b874),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    return;
                }
            }
            const char *lang_str = "LANGUAGEHASH";
            if (part->HasAppliedAttribute(bStringHash(lang_str))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            }
            return;
        }

        case 0x4d: {
            CarPart *part = buyPart->ThePart;
            if (!part) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0x60a662f5));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), GetLocalizedString(0x60a662f5));
                }
                return;
            }
            const char *lang_str = "LANGUAGEHASH";
            if (part->HasAppliedAttribute(bStringHash(lang_str))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            }
            return;
        }

        default: {
            CarPart *part = buyPart->ThePart;
            const char *lang_str = "LANGUAGEHASH";
            if (part->HasAppliedAttribute(bStringHash(lang_str))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)),
                             GetLocalizedString(part->GetAppliedAttributeUParam(bStringHash(lang_str), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(buyPart->CarSlotID)), part->GetName());
            }
            return;
        }
    }
}

CustomizeShoppingCart::CustomizeShoppingCart(ScreenConstructorData *sd) : UIWidgetMenu(sd) {
    bScrollWrapped = false;
    if (gCarCustomizeManager.IsCareerMode()) {
        iMaxWidgetsOnScreen = 4;
    } else {
        iMaxWidgetsOnScreen = 6;
    }
    Setup();
}

bool CustomizeShoppingCart::CanCheckout() {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            return true;
        }
        return gCarCustomizeManager.GetCartTotal(CCT_TOTAL) <= FEDatabase->GetCareerSettings()->GetCash();
    }
    return true;
}

void CustomizeShoppingCart::ToggleAllNumberDecals() {
    int count = gCarCustomizeManager.ShoppingCart.CountElements();
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
    ShoppingCartItem *cart_item = gCarCustomizeManager.GetFirstCartItem();
    while (cart_item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
        if (cart_item->bActive) {
            cart_item->ToggleActive();
        }
        cart_item = cart_item->GetNext();
    }
    FEShoppingCartItem *w = static_cast<FEShoppingCartItem *>(Options.GetHead());
    while (w != Options.EndOfList()) {
        w->SetCheckScripts();
        w->Draw();
        w = static_cast<FEShoppingCartItem *>(w->GetNext());
    }
}

// --- CustomizeParts ---

static void UnLoadCustomHUDPacksAndTextures();

CustomizeParts::CustomizeParts(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    bTexturesNeedUnload = false;
    if (Category == 0x307) {
        if (!TexturePackLoaded) {
            for (int i = 0; i < 11; i++) {
                CustomizeHUDTexPackResources[i] = 0;
                for (unsigned int j = 0; j < 5; j++) {
                    CustomizeHUDTexTextureResources[i * 5 + j] = 0;
                }
            }
        }
        TachRPM = static_cast<int>(Physics::Info::Redline(gCarCustomizeManager.ThePVehicle));
        if (TachRPM >= 0x251d) {
            TachRPM = 10000;
        } else if (TachRPM >= 0x2135) {
            TachRPM = 9000;
        } else if (TachRPM > 0x1d4c) {
            TachRPM = 8000;
        } else {
            TachRPM = 7000;
        }
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xdee8632b));
    }
    Setup();
}

CustomizeParts::~CustomizeParts() {
    if (TexturePackLoaded && bTexturesNeedUnload) {
        UnLoadCustomHUDPacksAndTextures();
    }
}

void CustomizeParts::LoadNextHudTexturePack() {
    char buf[64];
    bSPrintf(buf, "HUD_TEX_%02d", PacksLoadedCount);
    int result = eLoadStreamingTexturePack(buf, reinterpret_cast<void (*)(void *)>(TexturePackLoadedCallbackAccessor), this, 0);
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
    iPerfIndex = 0;
    invalidMarkers = 0;
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
    SetInitialOption(FromCategory & 0xFFFF00FF);
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
        case 0:
            return 2;
        case 1:
            return 3;
        case 2:
            return 4;
        case 3:
            return 5;
        case 4:
            return 6;
        case 5:
            return 7;
        case 6:
            return 8;
        case 7:
            return 9;
        default:
            return 1;
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
        FEngSetVisible(FEngFindObject(pPackageName, 0x8d1559a4));
    } else {
        FEngSetInvisible(FEngFindObject(pPackageName, 0x8d1559a4));
    }
}

void CustomizationScreenHelper::SetCareerStuff(SelectablePart *part, unsigned int cat, unsigned int tradeInValue) {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            FEngSetLanguageHash(GetPackageName(), 0x63ca8308, GetMarkerNameFromCategory(static_cast<eCustomizeCategory>(cat)));
            FEPrintf(GetPackageName(), 0x23d918fe, "1");
            FEPrintf(GetPackageName(), 0x83e3cd39, "%d", GetNumMarkersFromCategory(static_cast<eCustomizeCategory>(cat)));
        } else {
            if (part) {
                FEPrintf(GetPackageName(), 0xdbb80edd, "%d", part->GetPrice());
            } else {
                SelectablePart *tempPart = gCarCustomizeManager.GetTempColoredPart();
                if (tempPart) {
                    FEPrintf(GetPackageName(), 0xdbb80edd, "%d", tempPart->GetPrice());
                } else {
                    FEPrintf(GetPackageName(), 0xdbb80edd, "-1");
                }
            }
            FEPrintf(GetPackageName(), 0xc60adcfd, "%d", FEDatabase->GetCareerSettings()->GetCash());
            FEPrintf(GetPackageName(), 0x7a6d2f71, "%d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(2)));
            FEPrintf(GetPackageName(), 0xa91eda8a, "%d", tradeInValue);
        }
        SetHeatValue(gCarCustomizeManager.GetActualHeat());
        SetHeatPreview(gCarCustomizeManager.GetPreviewHeat(part));
        DrawMeters();
    } else {
        SetCareerStatusIcon(CPS_AVAILABLE);
        SetCashVisibility(false);
        HeatMeter.SetVisibility(false);
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x24c6bfad));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xea903012));
    }
}

void CustomizationScreenHelper::SetPartStatus(SelectablePart *part, unsigned int unlock_blurb, int part_num, int max_parts) {
    if (part) {
        if (part->IsInstalled()) {
            SetPlayerCarStatusIcon(CPS_INSTALLED);
        } else if (part->IsInCart()) {
            SetPlayerCarStatusIcon(CPS_IN_CART);
        } else {
            SetPlayerCarStatusIcon(CPS_AVAILABLE);
        }
        if (part->IsLocked() && unlock_blurb) {
            if (IsInitComplete()) {
                SetUnlockOverlayState(true, unlock_blurb);
            }
            SetCareerStatusIcon(CPS_LOCKED);
        } else {
            if (bUnlockOverlayShowing) {
                SetUnlockOverlayState(false, 0);
            }
            if (part->IsNew()) {
                SetCareerStatusIcon(CPS_NEW);
            } else {
                SetCareerStatusIcon(CPS_AVAILABLE);
            }
        }
    }
    FEPrintf(GetPackageName(), 0x6f25a248, "%d", part_num);
    FEPrintf(GetPackageName(), 0xb2037bdc, "%d", max_parts);
}

// --- CustomizationScreen additional ---

void CustomizationScreen::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    DisplayHelper.DrawTitle();
    int count = Options.Options.CountElements();
    if (count != Options.iNumBookEnds) {
        int tradeInValue = 0;
        if (gCarCustomizeManager.IsCareerMode()) {
            if (!CustomizeIsInBackRoom()) {
                SelectablePart *part = GetSelectedPart();
                if (part) {
                    if (gCarCustomizeManager.CanTradeIn(part)) {
                        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(part->GetSlotID());
                        if (installed) {
                            eUnlockFilters filter = gCarCustomizeManager.GetUnlockFilter();
                            tradeInValue = UnlockSystem::GetCarPartCost(filter, part->GetSlotID(), installed, 0);
                        }
                        tradeInValue = static_cast<int>(static_cast<float>(tradeInValue) * gTradeInFactor);
                    }
                }
            }
        }
        SelectablePart *selPart = GetSelectedPart();
        DisplayHelper.SetCareerStuff(selPart, Category, tradeInValue);
        SelectablePart *selPart2 = GetSelectedPart();
        unsigned int unlockBlurb = static_cast<CustomizePartOption *>(Options.GetCurrentOption())->UnlockBlurb;
        int partNum = Options.GetCurrentIndex();
        int numParts = Options.Options.CountElements();
        DisplayHelper.SetPartStatus(selPart2, unlockBlurb, partNum, numParts - Options.iNumBookEnds);
    }
}

SelectablePart *CustomizationScreen::FindInCartPart() {
    IconOption *tail = Options.TailBookEnd;
    IconOption *cur = Options.HeadBookEnd->GetNext();
    for (;;) {
        bool atEnd = (cur == tail) || (cur == Options.HeadBookEnd);
        if (atEnd)
            break;
        SelectablePart *part = static_cast<CustomizePartOption *>(cur)->ThePart;
        if (part && (part->GetPartState() & 0xF0) == CPS_IN_CART) {
            return part;
        }
        cur = cur->GetNext();
    }
    return nullptr;
}

void CustomizationScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0x35f8620b) {
        DisplayHelper.SetInitComplete(true);
        RefreshHeader();
    }
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        ScrollTime.SetPackedTime(RealTimer.GetPackedTime());
    }
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xc98356ba:
            if (!bNeedsRefresh) {
                return;
            }
            {
                float elapsed = static_cast<float>(static_cast<double>(RealTimer.GetPackedTime() - ScrollTime.GetPackedTime())) * 0.001f;
                if (elapsed <= 0.25f) {
                    return;
                }
            }
            bNeedsRefresh = false;
            RefreshHeader();
            break;
        case 0x5e6ea975:
            Options.SetAllowFade(true);
            Options.bDelayUpdate = false;
            break;
        case 0x406415e3: {
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
            ShoppingCartItem *inCart =
                gCarCustomizeManager.IsPartTypeInCart(static_cast<CustomizePartOption *>(Options.GetCurrentOption())->GetPart());
            if (inCart) {
                pReplacingOption = FindMatchingOption(inCart->GetBuyingPart());
            }
            cFEng_mInstance->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xFF);
            break;
        }
        case 0x91dfdf84: {
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
            break;
        }
        case 0xcf91aacd:
            CustomizeShoppingCart::ExitShoppingCart();
            break;
        case 0xc519bfbf: {
            unsigned int cat = Category;
            if (cat > 0x200 && cat < 0x208) {
                return;
            }
            if (cat == 0x307) {
                return;
            }
            if (Options.pCurrentNode == nullptr) {
                Showcase::FromIndex = 0;
            } else {
                Showcase::FromIndex = Options.GetCurrentIndex();
            }
            Showcase::FromPackage = GetPackageName();
            Showcase::FromArgs = Category | (FromCategory << 16);
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeShowcasePkg, 0, 0, false);
            break;
        }
        case 0xb5af2461:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
    }
}

// --- CustomizeShoppingCart additional ---

extern const char *g_pCustomizeShoppingCartPkg;

void CustomizeShoppingCart::ShowShoppingCart(const char *pkg) {
    pParentPkg = pkg;
    cFEng_mInstance->QueuePackagePush(g_pCustomizeShoppingCartPkg, 0, 0, false);
}

void CustomizeShoppingCart::ExitShoppingCart() {
    if (gCarCustomizeManager.IsInBackRoom()) {
        gCarCustomizeManager.SetInBackRoom(false);
        FEManager::Get()->SetGarageType(GARAGETYPE_CUSTOMIZATION_SHOP);
    }
    cFEng::Get()->QueuePackageSwitch(g_pCustomizeMainPkg, 0, 0, false);
}

bool CustomizeShoppingCart::IsSlotIDNumberDecal(int slot_id) {
    if (slot_id == 0x71 || slot_id == 0x72 || slot_id == 0x69 || slot_id == 0x6a) {
        return true;
    }
    return false;
}

void CustomizeShoppingCart::ClearUncheckedItems() {
    ShoppingCartItem *item = gCarCustomizeManager.ShoppingCart.GetHead();
    while (item != gCarCustomizeManager.ShoppingCart.EndOfList()) {
        if (!item->IsActive()) {
            if (item->GetBuyingPart()->GetSlotID() == 0x4d) {
                ShoppingCartItem *inner = gCarCustomizeManager.ShoppingCart.GetHead();
                while (inner != gCarCustomizeManager.ShoppingCart.EndOfList()) {
                    if (inner->GetBuyingPart()->GetSlotID() < 0x53 && inner->GetBuyingPart()->GetSlotID() > 0x4e) {
                        ShoppingCartItem *next_inner = static_cast<ShoppingCartItem *>(inner->Next);
                        gCarCustomizeManager.RemoveFromCart(inner);
                        inner = next_inner;
                    } else {
                        inner = static_cast<ShoppingCartItem *>(inner->Next);
                    }
                }
            }
            ShoppingCartItem *next = static_cast<ShoppingCartItem *>(item->Next);
            gCarCustomizeManager.RemoveFromCart(item);
            item = next;
        } else {
            item = static_cast<ShoppingCartItem *>(item->Next);
        }
    }
    gCarCustomizeManager.ResetPreview();
}

void CustomizeShoppingCart::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xc519bfc3:
            ToggleChecked();
            RefreshHeader();
            break;
        case 0x406415e3:
            if (!gCarCustomizeManager.DoesCartHaveActiveParts()) {
                gCarCustomizeManager.EmptyCart();
                gCarCustomizeManager.ResetPreview();
                gCarCustomizeManager.ResetPreview();
                cFEng_mInstance->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
                cFEng_mInstance->QueuePackagePop(1);
                break;
            }
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
                DialogInterface::ShowTwoButtons(GetPackageName(), g_pCustomizeDlgPkg, static_cast<eDialogTitle>(1), 0x70e01038, 0x417b25e4,
                                                0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf, static_cast<eDialogFirstButtons>(1), dialog_hash);
            } else {
                DialogInterface::ShowOk(GetPackageName(), g_pCustomizeDlgPkg, static_cast<eDialogTitle>(1), 0xa984a42);
            }
            break;
        case 0xd05fc3a3:
            gCarCustomizeManager.Checkout();
            cFEng_mInstance->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
            cFEng_mInstance->QueuePackagePop(1);
            break;
        case 0xc519bfc4:
            UncheckAllItems();
            RefreshHeader();
            break;
        case 0x72619778:
        case 0x911c0a4b:
            RefreshHeader();
            break;
        case 0x911ab364:
            ClearUncheckedItems();
            cFEng_mInstance->QueueGameMessage(0x5a928018, pParentPkg, 0xFF);
            cFEng_mInstance->QueuePackagePop(1);
            break;
    }
}

void CustomizeShoppingCart::RefreshHeader() {
    if (pCurrentOption) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0x842b0e89));
        ShoppingCartItem *item = static_cast<FEShoppingCartItem *>(pCurrentOption)->GetItem();
        if (item->IsActive()) {
            FEngSetLanguageHash(GetPackageName(), 0xd57c95e1, 0x5dabcbc0);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xd57c95e1, 0x28feadd);
        }
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x842b0e89));
    }
    HeatMeter.SetCurrent(gCarCustomizeManager.GetActualHeat());
    HeatMeter.SetPreview(gCarCustomizeManager.GetCartHeat());
    HeatMeter.Draw();
    if (gCarCustomizeManager.IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            SetMarkerAmounts();
            int total;
            FEMarkerManager::ePossibleMarker availableMarker;
            if (CustomizeIsInParts()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0xa03a752f);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x4ac68298);
                total = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BODY, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_HOOD, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_SPOILER, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_RIMS, 0);
                availableMarker = FEMarkerManager::MARKER_ROOF_SCOOP;
            } else if (CustomizeIsInPerformance()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0x358db897);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x68342700);
                total = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BRAKES, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ENGINE, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_NOS, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_INDUCTION, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_CHASSIS, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_TIRES, 0);
                availableMarker = FEMarkerManager::MARKER_TRANSMISSION;
            } else {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0x93296e59);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x78f1c602);
                total = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_DECAL, 0);
                total += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0);
                availableMarker = FEMarkerManager::MARKER_CUSTOM_HUD;
            }
            int available = TheFEMarkerManager.GetNumMarkers(availableMarker, 0);
            FEPrintf(GetPackageName(), 0xd1497a06, "%d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0)));
            int cartCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0));
            FEPrintf(GetPackageName(), 0x18661565, "%d", (total + available) - cartCost);
        } else {
            FEPrintf(GetPackageName(), 0xd1497a06, "%d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0)));
            FEPrintf(GetPackageName(), 0x34f7c0e8, "%d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(1)));
            int totalCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(2));
            FEPrintf(GetPackageName(), 0x18661565, "%d", totalCost);
            FEPrintf(GetPackageName(), 0x8531e22e, "%d", FEDatabase->GetCareerSettings()->GetCash() - totalCost);
        }
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x9ea22e0b));
    }
}

void CustomizeShoppingCart::SetMarkerAmounts() {
    if (CustomizeIsInPerformance()) {
        static Physics::Upgrades::Type phys_type[7] = {
            Physics::Upgrades::kType_Transmission, Physics::Upgrades::kType_Tires,   Physics::Upgrades::kType_Induction,
            Physics::Upgrades::kType_Brakes,       Physics::Upgrades::kType_Chassis, Physics::Upgrades::kType_Engine,
            Physics::Upgrades::kType_Nitrous,
        };
        static int markers[7] = {
            FEMarkerManager::MARKER_BRAKES,  FEMarkerManager::MARKER_ENGINE, FEMarkerManager::MARKER_NOS,          FEMarkerManager::MARKER_INDUCTION,
            FEMarkerManager::MARKER_CHASSIS, FEMarkerManager::MARKER_TIRES,  FEMarkerManager::MARKER_TRANSMISSION,
        };
        int i = 0;
        do {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(phys_type[i]);
            int num = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(markers[i]), 0);
            if (item && item->IsActive()) {
                num--;
            }
            i++;
            SetMarkerData(i, item, num);
        } while (i < 7);
    } else if (CustomizeIsInParts()) {
        static unsigned int slot_id[5] = {0x17, 0x3F, 0x2C, 0x42, 0x3E};
        static int markers[5] = {
            FEMarkerManager::MARKER_BODY, FEMarkerManager::MARKER_HOOD,       FEMarkerManager::MARKER_SPOILER,
            FEMarkerManager::MARKER_RIMS, FEMarkerManager::MARKER_ROOF_SCOOP,
        };
        int i = 0;
        do {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id[i]);
            int num = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(markers[i]), 0);
            if (item && item->IsActive()) {
                num--;
            }
            i++;
            SetMarkerData(i, item, num);
        } while (i < 5);
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e22));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e23));
    } else {
        ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x4d));
        int num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0);
        if (item && item->IsActive()) {
            num--;
        }
        SetMarkerData(1, item, num);

        int spending = GetNumMarkersSpending(0x53);
        spending += GetNumMarkersSpending(0x5b);
        spending += GetNumMarkersSpending(99);
        spending += GetNumMarkersSpending(100);
        spending += GetNumMarkersSpending(0x65);
        spending += GetNumMarkersSpending(0x66);
        spending += GetNumMarkersSpending(0x67);
        spending += GetNumMarkersSpending(0x68);
        spending += GetNumMarkersSpending(0x6b);
        spending += GetNumMarkersSpending(0x6c);
        spending += GetNumMarkersSpending(0x6d);
        spending += GetNumMarkersSpending(0x6e);
        spending += GetNumMarkersSpending(0x6f);
        spending += GetNumMarkersSpending(0x70);
        spending += GetNumMarkersSpending(0x73);
        spending += GetNumMarkersSpending(0x7b);
        int numDecals = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_DECAL, 0);
        SetMarkerData(2, item, numDecals - spending);

        item = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x4c));
        num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0);
        if (item && item->IsActive()) {
            num--;
        }
        SetMarkerData(3, item, num);

        item = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x84));
        num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_CUSTOM_HUD, 0);
        if (item && item->IsActive()) {
            num--;
        }
        SetMarkerData(4, item, num);

        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e21));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e22));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e23));
    }
}

void CustomizeShoppingCart::SetMarkerData(int idx, ShoppingCartItem *item, int spending) {
    const char *pkg = GetPackageName();
    unsigned int hash = FEngHashString("MARKER_GROUP_%d", idx);
    unsigned int script = 0x6ebbfb68;
    if (spending == 0) {
        script = 0x163c76;
    }
    FEngSetScript(pkg, hash, script, true);

    hash = FEngHashString("MARKER_NUM_%d", idx);
    FEPrintf(GetPackageName(), hash, "%$d", spending);

    hash = FEngHashString("MARKER_BLOOM_%d", idx);
    FEPrintf(GetPackageName(), hash, "%$d", spending);
}

int CustomizeShoppingCart::GetNumMarkersSpending(unsigned int marker) {
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(marker);
    int result = 0;
    if (item && item->IsActive()) {
        result = 1;
    }
    return result;
}

void CustomizeShoppingCart::SetMarkerImages() {
    if (CustomizeIsInPerformance()) {
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957471), 0x4887f351);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957472), 0x4f424e0f);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957473), 0x6fea04c8);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957474), 0x8e284227);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957475), 0x190eb6);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957476), 0x7373f1ef);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957477), 0xd142d3e3);
    } else if (CustomizeIsInParts()) {
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957471), 0xaf393dba);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957472), 0xf375276e);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957473), 0xc51a4f62);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957474), 0xc19491cc);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957475), 0x25a4375e);
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e22));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e23));
    } else {
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957471), 0xd35f04c0);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957472), 0xa9135927);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957473), 0xdb89e17);
        FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xeb957474), 0x8ba602fc);
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e21));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e22));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e23));
    }
}

void CustomizeShoppingCart::Setup() {
    const char *pkg = GetPackageName();
    CustomizeMeter *meter = &HeatMeter;
    const char *meter_name = "HEAT_METER";
    float actual = gCarCustomizeManager.GetActualHeat();
    float cart = gCarCustomizeManager.GetCartHeat();
    meter->Init(pkg, meter_name, 1.0f, 5.0f, actual, cart);

    ShoppingCartItem *item = gCarCustomizeManager.GetFirstCartItem();
    while (item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
        switch (item->GetBuyingPart()->GetSlotID()) {
            case 0x4f:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x69:
            case 0x6a:
            case 0x72:
            case 0x85:
            case 0x86:
            case 0x87:
                break;
            default:
                AddItem(item);
                break;
        }
        item = item->GetNext();
    }

    if (CustomizeIsInBackRoom()) {
        SetMarkerImages();
    }

    SetInitialOption(0);

    FEShoppingCartItem *widget = static_cast<FEShoppingCartItem *>(Options.GetHead());
    while (widget != static_cast<FEShoppingCartItem *>(Options.EndOfList())) {
        widget->SetCheckScripts();
        widget->SetActiveScripts();
        widget = static_cast<FEShoppingCartItem *>(widget->GetNext());
    }

    RefreshHeader();
}

void CustomizeShoppingCart::AddItem(ShoppingCartItem *item) {
    FEShoppingCartItem *widget = new FEShoppingCartItem(item);
    widget->SetTitleObject(GetCurrentFEString("PART_NAME_"));
    widget->SetDataObject(GetCurrentFEString("PRICE_"));
    widget->SetBacking(GetCurrentFEObject(pBackingName));
    widget->SetCheckIcon(GetCurrentFEImage("CHECK_ICON_"));
    widget->SetTradeInString(GetCurrentFEString("TRADE_IN_"));
    widget->SetTopLeft(vLastWidgetPos);
    widget->SetMaxTitleSize(vMaxTitleSize);
    widget->SetMaxDataSize(vMaxDataSize);
    widget->SetDataPos(vDataPos);
    Options.AddTail(widget);
    iIndexToAdd++;
    IncrementStartPos();
    widget->Show();
    widget->Draw();
    widget->Position();

    float data_x, data_y;
    float data_w, data_h;
    FEngGetTopLeft(reinterpret_cast<FEObject *>(widget->GetDataObject()), data_x, data_y);
    FEngGetSize(reinterpret_cast<FEObject *>(widget->GetDataObject()), data_w, data_h);
    widget->SetWidth(bAbs(widget->GetTopLeftX() - (data_x + data_w)));
}

// --- CustomizeCategoryScreen additional ---

CustomizeCategoryScreen::CustomizeCategoryScreen(ScreenConstructorData *sd)
    : IconScrollerMenu(sd) //
      ,
      bBackingOut(false) //
      ,
      BackToPkg(nullptr) //
      ,
      HeatMeter() {
    Category = sd->Arg & 0xFFFF;
    FromCategory = static_cast<int>(static_cast<short>(sd->Arg >> 16));
    if (Category != 0 || !CustomizeIsInBackRoom()) {
        GarageMainScreen::GetInstance()->SetCustomizationCategory(Category);
    }
    const char *meter_name = "HEAT_METER";
    const char *pkg = GetPackageName();
    float actual = gCarCustomizeManager.GetActualHeat();
    CustomizeMeter *meter = &HeatMeter;
    float cart = gCarCustomizeManager.GetCartHeat();
    meter->Init(pkg, meter_name, 1.0f, 5.0f, actual, cart);
}

int CustomizeCategoryScreen::AddCustomOption(const char *to_pkg, unsigned int tex_hash, unsigned int name_hash, unsigned int to_cat) {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            if (gCarCustomizeManager.IsCategoryLocked(to_cat, true)) {
                return -1;
            }
        }
    }
    CustomizeMainOption *opt = new CustomizeMainOption(to_pkg, tex_hash, name_hash, to_cat, Category);
    AddOption(opt);
    if (gCarCustomizeManager.IsCategoryLocked(to_cat, false)) {
        opt->UnlockStatus = CPS_LOCKED;
    } else if (gCarCustomizeManager.IsCategoryNew(to_cat)) {
        opt->UnlockStatus = CPS_NEW;
    }
    return Options.GetIndexToAdd() - 3;
}

void CustomizeCategoryScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xb5af2461:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
        case 0xe1fde1d1:
            if (!bBackingOut) {
                break;
            }
            cFEng_mInstance->QueuePackageSwitch(BackToPkg, FromCategory | (Category << 16), 0, false);
            break;
        case 0x911ab364: {
            bool leave = true;
            if (Category <= 0x803) {
                if (Category >= 0x801) {
                    if (gCarCustomizeManager.DoesCartHaveActiveParts()) {
                        cFEng_mInstance->QueueGameMessage(0x1720b124, GetPackageName(), 0xFF);
                        leave = false;
                        Options.bReactToInput = true;
                    } else {
                        gCarCustomizeManager.EmptyCart();
                        gCarCustomizeManager.ResetPreview();
                    }
                }
            }
            if (leave) {
                bBackingOut = true;
                cFEng_mInstance->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            }
            break;
        }
        case 0xc519bfbf: {
            CustomizeMainOption *opt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
            Showcase::FromPackage = GetPackageName();
            Showcase::FromArgs = Category | (opt->Category << 16);
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeShowcasePkg, gCarCustomizeManager.EntryPoint, 0, false);
            break;
        }
        case 0xb4edeb6d:
            Options.bReactToInput = true;
            break;
        case 0x7a318ee0:
            gCarCustomizeManager.EmptyCart();
            gCarCustomizeManager.ResetPreview();
            cFEng_mInstance->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            break;
        case 0x1720b124:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
    }
}

// --- SetStockPartOption ---

// SetStockPartOption::React is out-of-line (not inline)
void SetStockPartOption::React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) {
    if (!ThePart->IsInstalled()) {
        gCarCustomizeManager.AddToCart(ThePart);
        ThePart->SetInCart();
    }
}

// --- CustomizeMain additional ---

void CustomizeMain::SetTitle(bool isInBackroom) {
    char local_48[64];
    if (isInBackroom) {
        const char *fmt = "%s";
        const char *str = GetLocalizedString(0x92fcdbf0);
        bSNPrintf(local_48, 0x40, fmt, str);
    } else {
        const char *fmt = "%s";
        const char *str = GetLocalizedString(0x1f242e03);
        bSNPrintf(local_48, 0x40, fmt, str);
    }
    int lang = GetCurrentLanguage();
    if (lang != 2 && lang != 0xd) {
        int n = 0;
        while (local_48[n] != 0) {
            char c = local_48[n];
            if (static_cast<unsigned int>(c - 0x41) < 0x1a) {
                c = c | 0x20;
            }
            local_48[n] = c;
            n++;
        }
    }
    FEPrintf(GetPackageName(), 0xb71b576d, "%s", local_48);
}

void CustomizeMain::RefreshHeader() {
    CustomizeCategoryScreen::RefreshHeader();
    int isCareer = gCarCustomizeManager.IsCareerMode();
    if (isCareer && !gCarCustomizeManager.IsHeroCar()) {
        int inBackRoom = CustomizeIsInBackRoom();
        if (!inBackRoom && gCarCustomizeManager.GetNumCustomizeMarkers() > 0) {
            FEngSetVisible(FEngFindObject(GetPackageName(), 0xdc6ee739));
        } else {
            FEngSetInvisible(FEngFindObject(GetPackageName(), 0xdc6ee739));
        }
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xdc6ee739));
    }
    if (Options.GetCurrentOption()) {
        gCarCustomizeManager.IsCategoryNew(static_cast<unsigned short>(static_cast<CustomizeMainOption *>(Options.GetCurrentOption())->Category));
    }
}

void CustomizeMain::SwitchRooms() {
    bool newState = !CustomizeIsInBackRoom();
    CustomizeSetInBackRoom(newState);
    SetTitle(newState);
    int savedIdx = Options.GetCurrentIndex();
    if (newState) {
        cFEng_mInstance->QueuePackageMessage(0xa1caff8d, GetPackageName(), nullptr);
        FEManager::Get()->SetGarageType(static_cast<eGarageType>(4));
    } else {
        cFEng_mInstance->QueuePackageMessage(0x5c01c5, GetPackageName(), nullptr);
        FEManager::Get()->SetGarageType(static_cast<eGarageType>(3));
    }
    SetScreenNames();
    Options.RemoveAll();
    Options.AddInitialBookEnds();
    BuildOptionsList();
    if (bFadeInIconsImmediately) {
        Options.StartFadeIn();
    }
    Options.SetInitialPos(savedIdx);
    RefreshHeader();
}

void CustomizeMain::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (!gCarCustomizeManager.IsCareerMode() || msg != 0x911ab364) {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case 0x1265ece9: {
            GetInstance_GarageMainScreen()->UpdateCurrentCameraView(false);
            if (CustomizeIsInBackRoom()) {
                cFEng_mInstance->QueuePackageMessage(0xa1caff8d, GetPackageName(), nullptr);
            } else {
                cFEng_mInstance->QueuePackageMessage(0x5c01c5, GetPackageName(), nullptr);
            }
            break;
        }
        case 0x911ab364:
            if (gCarCustomizeManager.IsCareerMode()) {
                if (!CustomizeIsInBackRoom()) {
                    cFEng_mInstance->QueuePackageMessage(0x6d5d86a1, GetPackageName(), nullptr);
                    cFrontendDatabase *db = FEDatabase;
                    GarageMainScreen *gms = GetInstance_GarageMainScreen();
                    *(unsigned int *)((char *)gms + 0x8c) = 0xFFFFFFFF;
                    char port = FEngMapJoyParamToJoyport(param1);
                    db->SetPlayersJoystickPort(0, port);
                    if (!db->IsCarStableDirty()) {
                        *(int *)((char *)MemoryCard_s_pThis + 0x78) = 1;
                    }
                    CarViewer_haveLoadedOnce = 0;
                    StartCareerFreeRoam();
                } else {
                    SwitchRooms();
                    return;
                }
            } else {
                cFEng_mInstance->QueuePackageMessage(0x6d5d86a1, GetPackageName(), nullptr);
            }
            gCarCustomizeManager.RelinquishControl();
            break;
        case 0x34dc1bec:
            if (gCarCustomizeManager.GetNumCustomizeMarkers() > invalidMarkers) {
                SwitchRooms();
            }
            invalidMarkers = 0;
            break;
        case 0xc519bfc4:
            if (!gCarCustomizeManager.IsCareerMode() || gCarCustomizeManager.GetNumCustomizeMarkers() != 0) {
                if (gCarCustomizeManager.IsCareerMode() && !CustomizeIsInBackRoom() && !gCarCustomizeManager.IsHeroCar()) {
                    invalidMarkers = 0;
                    if (TheFEMarkerManager.IsMarkerAvailable(FEMarkerManager::MARKER_INDUCTION, 0) &&
                        !gCarCustomizeManager.CanInstallJunkman(Physics::Upgrades::kType_Brakes)) {
                        invalidMarkers++;
                    }
                    if (TheFEMarkerManager.IsMarkerAvailable(FEMarkerManager::MARKER_NOS, 0) &&
                        !gCarCustomizeManager.CanInstallJunkman(Physics::Upgrades::kType_Induction)) {
                        invalidMarkers++;
                    }
                    if (invalidMarkers > 0) {
                        DialogInterface::ShowOneButton(GetPackageName(), g_pCustomizeDlgPkg, static_cast<eDialogTitle>(2), 0x417b2601, 0x34dc1bec,
                                                       0x3b3e83);
                    } else {
                        SwitchRooms();
                    }
                }
            }
            break;
    }
}

// --- Constructors ---

CustomizeSub::CustomizeSub(ScreenConstructorData *sd)
    : CustomizeCategoryScreen(sd) //
      ,
      InstalledPartOptionIndex(0) //
      ,
      InCartPartOptionIndex(0) //
      ,
      TitleHash(0) {
    Setup();
    gCarCustomizeManager.ResetPreview();
}

CustomizePerformance::CustomizePerformance(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    Setup();
}

CustomizeRims::CustomizeRims(ScreenConstructorData *sd)
    : CustomizationScreen(sd) //
      ,
      InnerRadius(0xf) //
      ,
      MinRadius(0xf) //
      ,
      MaxRadius(0xf) {
    Setup();
}

CustomizeDecals::CustomizeDecals(ScreenConstructorData *sd)
    : CustomizationScreen(sd) //
      ,
      bIsBlack(true) {
    Setup();
}

CustomizeHUDColor::CustomizeHUDColor(ScreenConstructorData *sd)
    : CustomizationScreen(sd) //
      ,
      SelectedColor(nullptr) //
      ,
      Cursor(nullptr) //
      ,
      bTexturesNeedUnload(false) {
    Cursor = FEngFindObject(GetPackageName(), 0xB893252A);
    Setup();
}

// --- CustomizePaint helpers ---

struct CustomizePaintDatum : public ArrayDatum {
    CustomizePaintDatum(SelectablePart *part, unsigned int unlock_blurb)
        : ArrayDatum(0xc6afdd7e, 0) //
          ,
          ThePart(part) //
          ,
          UnlockBlurb(unlock_blurb) {}

    ~CustomizePaintDatum() override;

    SelectablePart *ThePart;  // offset 0x24, size 0x4
    unsigned int UnlockBlurb; // offset 0x28, size 0x4
};

SelectablePart *CustomizePaint::FindInCartPart() {
    int count = ThePaints.GetNumDatum();
    for (int i = 0; i < count; i++) {
        CustomizePaintDatum *datum = static_cast<CustomizePaintDatum *>(ThePaints.GetDatumAt(i));
        SelectablePart *part = datum->ThePart;
        if ((part->GetPartState() & 0xF0) == CPS_IN_CART) {
            return part;
        }
    }
    return nullptr;
}

CustomizePartOption *CustomizePaint::FindMatchingOption(SelectablePart *to_find) {
    int count = ThePaints.GetNumDatum();
    CustomizePaintDatum *found = nullptr;
    for (int i = 0; i < count; i++) {
        CustomizePaintDatum *datum = static_cast<CustomizePaintDatum *>(ThePaints.GetDatumAt(i));
        if (datum->ThePart->GetPart() == to_find->GetPart()) {
            found = datum;
            break;
        }
    }
    if (found) {
        MatchingPaint.SetPart(found->ThePart);
        return &MatchingPaint;
    } else {
        return nullptr;
    }
}

void CustomizePaint::SetupRimPaint() {
    FEngSetInvisible(GetPackageName(), 0x2C3CC2D3);
    FEngSetInvisible(GetPackageName(), 0x53639A10);
    BuildSwatchList(0x4E);
}

// --- CustomizeParts helpers ---

void CustomizeParts::ShowHudObjects() {
    FEngSetScript(GetPackageName(), 0xDEE8632B, 0x5079C8F8, true);
    FEngSetVisible(FEngFindObject(GetPackageName(), 0xDEE8632B));
}

// --- CustomizeNumbers helpers ---

void CustomizeNumbers::UnsetShoppingCart() {
    SelectablePart *cur = LeftNumberList.GetHead();
    while (cur != reinterpret_cast<SelectablePart *>(&LeftNumberList)) {
        if ((cur->PartState & 0xF0) == CPS_IN_CART) {
            cur->PartState = static_cast<eCustomizePartState>(cur->PartState & ~CPS_IN_CART);
            break;
        }
        cur = static_cast<SelectablePart *>(cur->Next);
    }
    cur = RightNumberList.GetHead();
    while (cur != reinterpret_cast<SelectablePart *>(&RightNumberList)) {
        if ((cur->PartState & 0xF0) == CPS_IN_CART) {
            cur->PartState = static_cast<eCustomizePartState>(cur->PartState & ~CPS_IN_CART);
            return;
        }
        cur = static_cast<SelectablePart *>(cur->Next);
    }
}

// --- CustomizeMain additional ---

extern const char *g_pCustomizeHudColorPkg;
extern const char *g_pCustomizeShoppingCartPkg;

void CustomizeMain::SetScreenNames() {
    if (CustomizeIsInBackRoom()) {
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
    } else {
        g_pCustomizeSubPkg = "CustomizeCategory.fng";
        g_pCustomizeSubTopPkg = "CustomizeGenericTop.fng";
        g_pCustomizePartsPkg = "CustomizeParts.fng";
        g_pCustomizePerfPkg = "CustomizePerformance.fng";
        g_pCustomizeDecalsPkg = "Decals.fng";
        g_pCustomizePaintPkg = "Paint.fng";
        g_pCustomizeRimsPkg = "Rims.fng";
        g_pCustomizeHudColorPkg = "CustomHUDColor.fng";
        if (gCarCustomizeManager.IsCareerMode()) {
            g_pCustomizeShoppingCartPkg = "ShoppingCart.fng";
        } else {
            g_pCustomizeShoppingCartPkg = "ShoppingCart_QR.fng";
        }
        g_pCustomizeHudPkg = "CustomHUD.fng";
        g_pCustomizeSpoilerPkg = "Spoilers.fng";
    }
}

void CustomizeMain::BuildOptionsList() {
    int isHero = gCarCustomizeManager.IsHeroCar();
    if (!CustomizeIsInBackRoom()) {
        if (!isHero) {
            AddCustomOption(g_pCustomizeSubPkg, 0x6e0ca66c, 0x55dce1a, 0x801);
            iPerfIndex = AddCustomOption(g_pCustomizeSubPkg, 0x3987d054, 0xbaef8282, 0x802);
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
    switch (msg) {
        case 0x5073ef13:
            ScrollFilters(eSD_PREV);
            break;
        case 0xd9feec59:
            ScrollFilters(eSD_NEXT);
            break;
        case 0x9120409e:
        case 0xb5971bf1:
            SelectedIndex[TheFilter] = Options.GetCurrentIndex();
            break;
        case 0xc519bfbf:
            Showcase::FromFilter = TheFilter;
            break;
        case 0x5a928018: {
            SelectablePart *sel = FindInCartPart();
            if (!sel) {
                return;
            }
            if (gCarCustomizeManager.IsPartInCart(sel)) {
                return;
            }
            sel->PartState = static_cast<eCustomizePartState>(sel->PartState & 0xF);
            RefreshHeader();
            break;
        }
        case 0x911ab364:
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
            break;
    }
}

void CustomizeSpoiler::Setup() {
    SetTitleHash(0x94e73021);
    FEImage *img1 = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(img1, 0x5bc);
    FEImage *img2 = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(img2, 0x682);
    CarPart *activePart = gCarCustomizeManager.GetActivePartFromSlot(0x2c);
    if (Showcase::FromFilter != -1) {
        TheFilter = Showcase::FromFilter;
        Showcase::FromFilter = -1;
    } else if (activePart) {
        unsigned int filter = activePart->GetGroupNumber();
        if (filter != 4) {
            TheFilter = filter;
        }
    }
    BuildPartOptionListFromFilter(activePart);
    RefreshHeader();
}

void CustomizeSpoiler::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    int filter = TheFilter;
    switch (filter) {
        case 0:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x1f0e2b2);
            break;
        case 1:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x205b328);
            break;
        case 2:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x9912746);
            break;
        case 3:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0xe7416fc);
            break;
        default:
            break;
    }
    SelectablePart *sel = GetSelectedPart();
    int elapsed = RealTimer.GetPackedTime() - ScrollTime.GetPackedTime();
    Timer scrollDelay;
    scrollDelay.SetTime(0.3f);
    if (elapsed > scrollDelay.GetPackedTime()) {
        gCarCustomizeManager.PreviewPart(sel->GetSlotID(), sel->GetPart());
    } else {
        bNeedsRefresh = true;
    }
    const char *lang_str = "LANGUAGEHASH";
    CarPart *part = sel->GetPart();
    if (part->HasAppliedAttribute(bStringHash(lang_str))) {
        const char *pkg = GetPackageName();
        unsigned int langHash = sel->GetPart()->GetAppliedAttributeUParam(bStringHash(lang_str), 0);
        FEngSetLanguageHash(pkg, 0x5e7b09c9, langHash);
    } else {
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", sel->GetPart()->GetName());
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
    if (Showcase::FromIndex == 0) {
        Options.SetInitialPos(SelectedIndex[TheFilter]);
    } else {
        SelectedIndex[TheFilter] = Showcase::FromIndex;
        Options.SetInitialPos(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    }
}

// --- CustomizePerformance ---

void CustomizePerformance::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xe1fde1d1:
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
            break;
        case 0x5a928018: {
            SelectablePart *sel = FindInCartPart();
            if (!sel)
                return;
            if (gCarCustomizeManager.IsPartInCart(sel))
                return;
            sel->PartState = static_cast<eCustomizePartState>(sel->PartState & CPS_GAME_STATE_MASK);
            RefreshHeader();
            break;
        }
        case 0x911ab364:
            cFEng_mInstance->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            break;
    }
}

eMenuSoundTriggers CustomizePerformance::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    eMenuSoundTriggers toBeReturned = maybe;
    if (toBeReturned == static_cast<eMenuSoundTriggers>(4)) {
        switch (GetCategory()) {
            case 0x201:
                toBeReturned = static_cast<eMenuSoundTriggers>(0x31);
                break;
            case 0x202:
                toBeReturned = static_cast<eMenuSoundTriggers>(0x35);
                break;
            case 0x204:
                toBeReturned = static_cast<eMenuSoundTriggers>(0x34);
                break;
            case 0x205:
                toBeReturned = static_cast<eMenuSoundTriggers>(0x36);
                break;
            case 0x203:
            case 0x206:
                toBeReturned = static_cast<eMenuSoundTriggers>(0x32);
                break;
            case 0x207:
                toBeReturned = static_cast<eMenuSoundTriggers>(0x33);
                break;
        }
    }
    return toBeReturned;
}

void CustomizePerformance::Setup() {
    if (!gCarCustomizeManager.IsCareerMode()) {
        const unsigned long FEObj_QUICKRACE = 0xde511657;
        cFEng::Get()->QueuePackageMessage(FEObj_QUICKRACE, GetPackageName(), nullptr);
    }

    for (int i = 0; i < 3; i++) {
        DescLines[i] = FEngFindString(GetPackageName(), FEngHashString("DETAIL_TEXT_LINE%d", i + 1));
        DescBullets[i] = FEngFindImage(GetPackageName(), FEngHashString("PERFORMANCE_DETAILS_ICON%d", i + 1));
    }

    AccelSlider.Init(GetPackageName(), "ACCELERATION", 0.0f, 10.0f, 0.0f, gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, true),
                     gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, false), 1.0f);
    HandlingSlider.Init(GetPackageName(), "HANDLING", 0.0f, 10.0f, 0.0f, gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, true),
                        gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, false), 1.0f);
    TopSpeedSlider.Init(GetPackageName(), "TOPSPEED", 0.0f, 10.0f, 0.0f, gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, true),
                        gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, false), 1.0f);

    Physics::Upgrades::Type type = Physics::Upgrades::kType_Tires;
    switch (Category) {
        case CC_ENGINE:
            SetTitleHash(0x9853d9a6);
            break;
        case CC_TRANSMISSION:
            type = Physics::Upgrades::kType_Nitrous;
            SetTitleHash(0x29aa74ba);
            break;
        case CC_SUSPENSION:
            type = Physics::Upgrades::kType_Chassis;
            SetTitleHash(0x6e101aa7);
            break;
        case CC_NITROUS:
            type = Physics::Upgrades::kType_Induction;
            SetTitleHash(0x4ce19aa4);
            break;
        case CC_TIRES:
            type = Physics::Upgrades::kType_Engine;
            SetTitleHash(0x5aa9137);
            break;
        case CC_BRAKES:
            type = Physics::Upgrades::kType_Transmission;
            SetTitleHash(0x91997ee8);
            break;
        case CC_FORCED_INDUCTION:
            type = Physics::Upgrades::kType_Brakes;
            if (gCarCustomizeManager.IsTurbo()) {
                SetTitleHash(0x5b1255c);
            } else {
                SetTitleHash(0xbb6812bb);
            }
            break;
    }

    unsigned int icon_hash = 0xb8c8c0d4;
    bTList<SelectablePart> part_list;
    int j;
    bool is_locked;
    unsigned int desc_hash = 0;
    SelectablePart *part;

    if (!gCarCustomizeManager.IsInBackRoom()) {
        goto get_part_list;
    }
    if (!gCarCustomizeManager.IsCareerMode()) {
        goto get_part_list;
    }
    if (gCarCustomizeManager.IsHeroCar()) {
        goto get_part_list;
    }

    {
        unsigned int unlock_hash = 0;
        if (!gCarCustomizeManager.IsInBackRoom()) {
            unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), 7);
        }
        SelectablePart *new_part = new SelectablePart(nullptr, 0, 7, static_cast<GRace::Type>(static_cast<int>(type)), true, CPS_AVAILABLE, 0, true);
        AddPartOption(new_part, icon_hash, 7, desc_hash, unlock_hash, false);
        if (gCarCustomizeManager.IsPartInstalled(new_part)) {
            new_part->SetInstalled();
        } else if (gCarCustomizeManager.IsPartInCart(new_part)) {
            new_part->SetInCart();
        }
        goto after_initial_part_list;
    }

get_part_list:
    gCarCustomizeManager.GetPerformancePartsList(type, part_list);

after_initial_part_list:
    for (j = 1;; j++) {
        bNode *end = part_list.GetHead();
        if (part_list.IsEmpty()) {
            break;
        }
        part_list.RemoveHead();
        int unlock_level = gCarCustomizeManager.GetMaxPackages(type) - gCarCustomizeManager.GetNumPackages(type) + part->GetUpgradeLevel();
        unsigned int unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), unlock_level);
        is_locked = gCarCustomizeManager.IsPartLocked(part, 0);
        AddPartOption(part, icon_hash, j, desc_hash, unlock_hash, is_locked);
    }

    if (((FEDatabase->GetCareerSettings()->HasBeenAwardedBKReward() && !FEDatabase->IsCareerMode()) ||
         (FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce && !gCarCustomizeManager.IsHeroCar())) &&
        gCarCustomizeManager.CanInstallJunkman(type)) {
        SelectablePart *new_part = new SelectablePart(nullptr, 0, 7, static_cast<GRace::Type>(static_cast<int>(type)), true, CPS_AVAILABLE, 0, true);
        AddPartOption(new_part, icon_hash, 7, desc_hash, 0, false);
        if (gCarCustomizeManager.IsPartInstalled(new_part)) {
            new_part->SetInstalled();
        } else if (gCarCustomizeManager.IsPartInCart(new_part)) {
            new_part->SetInCart();
        }
    }

    if (!gCarCustomizeManager.IsInBackRoom()) {
        goto set_installed_option;
    }
    if (!gCarCustomizeManager.IsCareerMode()) {
        goto set_installed_option;
    }
    SetInitialOption(1);
    goto after_initial_option;

set_installed_option: {
    int installed_index = gCarCustomizeManager.GetInstalledPerfPkg(type);
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(type);
    if (item) {
        installed_index = item->GetBuyingPart()->GetUpgradeLevel();
    }
    SetInitialOption(installed_index);
}

after_initial_option:
    RefreshHeader();
}

void CustomizePerformance::RefreshHeader() {
    int num_lines = 3;

    SelectablePart *sel = GetSelectedPart();
    int phys_type = sel->GetPhysicsType();
    int upg_level = GetSelectedPart()->GetUpgradeLevel();

    gCarCustomizeManager.PreviewPerfPkg(static_cast<Physics::Upgrades::Type>(phys_type), upg_level);

    AccelSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, true));
    HandlingSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, true));
    TopSpeedSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, true));

    AccelSlider.Draw();
    HandlingSlider.Draw();
    TopSpeedSlider.Draw();

    sel = GetSelectedPart();
    phys_type = sel->GetPhysicsType();
    int level = GetSelectedPart()->GetUpgradeLevel();

    int maxPkgs = gCarCustomizeManager.GetMaxPackages(static_cast<Physics::Upgrades::Type>(phys_type));
    int numPkgs = gCarCustomizeManager.GetNumPackages(static_cast<Physics::Upgrades::Type>(phys_type));
    int pkg_index = (maxPkgs - numPkgs) + level;

    if (CustomizeIsInBackRoom() || level == 7) {
        level = 0;
        pkg_index = 0;
        num_lines = 1;
    }

    int i = 0;
    while (i < num_lines) {
        int line_idx = i;
        unsigned int desc_hash =
            GetPerfPkgDesc(static_cast<Physics::Upgrades::Type>(phys_type), pkg_index, line_idx + 1, gCarCustomizeManager.IsTurbo());
        i = line_idx + 1;
        if (DoesStringExist(desc_hash)) {
            FEngSetVisible(DescLines[line_idx]);
            FEngSetVisible(DescBullets[line_idx]);
            FEngSetLanguageHash(GetPackageName(), DescLines[line_idx]->NameHash, desc_hash);
        } else {
            FEngSetInvisible(DescLines[line_idx]);
            FEngSetInvisible(DescBullets[line_idx]);
        }

        Attrib::Gen::frontend inst(Attrib::FindCollection(0x85885722, gCarCustomizeManager.GetTuningCar()->FEKey), 0, nullptr);

        unsigned int brand_hash = GetPerfPkgBrand(static_cast<Physics::Upgrades::Type>(phys_type), pkg_index, line_idx);
        unsigned int brand_icon_hash = FEngHashString("BRAND_ICON_%d", i);

        if (GetTextureInfo(brand_hash, 0, 0)) {
            CustomizeFEngSetVisible(GetPackageName(), brand_icon_hash);
            CustomizeFEngSetTextureHash(GetPackageName(), brand_icon_hash, brand_hash);
        } else {
            CustomizeFEngSetInvisible(GetPackageName(), brand_icon_hash);
        }
    }

    while (i < 3) {
        int line_idx = i;
        i = line_idx + 1;
        FEngSetInvisible(DescLines[line_idx]);
        FEngSetInvisible(DescBullets[line_idx]);
        unsigned int icon_hash = FEngHashString("BRAND_ICON_%d", i);
        CustomizeFEngSetInvisible(GetPackageName(), icon_hash);
    }

    CustomizationScreen::RefreshHeader();

    unsigned int level_hash;
    if (GetSelectedPart()->GetUpgradeLevel() == 7) {
        level_hash = 0xedd14807;
    } else {
        int num = gCarCustomizeManager.GetNumPackages(static_cast<Physics::Upgrades::Type>(phys_type));
        level_hash = FEngHashString("PN_LEVEL_%d", level - (num - 6));
    }
    FEngSetLanguageHash(pOptionName, level_hash);
}

HUDLayerOption::HUDLayerOption(unsigned int layer, unsigned int icon_hash, unsigned int name_hash)
    : CustomizePartOption(nullptr, icon_hash, name_hash, 0, 0) //
      ,
      HUDLayer(layer) //
      ,
      SelectedPart(nullptr) {
    gCarCustomizeManager.GetCarPartList(layer, TheColors, 0);
}

CustomizeHUDColor::~CustomizeHUDColor() {
    if (CustomizeParts::TexturePackLoaded && bTexturesNeedUnload) {
        UnLoadCustomHUDPacksAndTextures();
    }
}

void CustomizeHUDColor::AddLayerOption(unsigned int layer, unsigned int icon_hash, unsigned int name_hash) {
    HUDLayerOption *opt = new HUDLayerOption(layer, icon_hash, name_hash);
    AddOption(opt);
}

void CustomizeHUDColor::Setup() {
    DisplayHelper.TitleHash = 0xb1b0e8af;
    AddLayerOption(0x86, 0x70f56628, 0xe18ddce1);
    AddLayerOption(0x87, 0xbf6682c9, 0xe18ddce0);
    AddLayerOption(0x85, 0xcc9e1ce4, 0xe18ddcdf);
    if (Showcase::FromIndex) {
        SetInitialOption(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(1);
    }
    BuildColorOptions();
    SetHUDTextures();
    SetInitialColors();
    RefreshHeader();
}

// --- CustomizeParts ---

void CustomizeParts::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg != 0x406415e3) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case 0x406415e3:
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
                gCarCustomizeManager.SetTempColoredPart(new SelectablePart(sel));
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
                gCarCustomizeManager.SetTempColoredPart(new SelectablePart(sel));
                cFEng_mInstance->QueuePackageSwitch(g_pCustomizePaintPkg, Category | (FromCategory << 16), 0, false);
            } else {
                CustomizationScreen::NotificationMessage(0x406415e3, pobj, param1, param2);
                return;
            }
            break;
        case 0xcf91aacd:
            if (Category != 0x307) {
                return;
            }
            if (!TexturePackLoaded) {
                return;
            }
            bTexturesNeedUnload = true;
            break;
        case 0x5a928018: {
            SelectablePart *sel = FindInCartPart();
            if (!sel) {
                return;
            }
            if (gCarCustomizeManager.IsPartInCart(sel)) {
                return;
            }
            sel->SetPartState(sel->GetPartState() & CPS_GAME_STATE_MASK);
            RefreshHeader();
            break;
        }
        case 0x911ab364:
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
            break;
    }
}

void CustomizeParts::Setup() {
    unsigned int icon_hash = 0;
    int car_slot_id = 0;
    unsigned int vinyl_group_number = 0;
    bool is_vinyl = false;
    CarPart *installed_part = nullptr;
    bool part_found = false;

    switch (Category) {
        case 0x101:
            SetTitleHash(0x6134c218);
            icon_hash = 0x28c24f6;
            if (CustomizeIsInBackRoom()) {
                icon_hash = 0xaf393dba;
            }
            car_slot_id = 0x17;
            goto after_switch;
        case 0x104:
            SetTitleHash(0x4d4a88d);
            icon_hash = 0x28f7092;
            if (CustomizeIsInBackRoom()) {
                icon_hash = 0xf375276e;
            }
            car_slot_id = 0x3f;
            goto after_switch;
        case 0x105:
            SetTitleHash(0x61e8f83c);
            icon_hash = 0x79165861;
            if (CustomizeIsInBackRoom()) {
                icon_hash = 0x25a4375e;
            }
            car_slot_id = 0x3e;
            goto after_switch;
        case 0x307:
            if (!CustomizeParts::TexturePackLoaded) {
                cFEng::Get()->QueuePackageMessage(0x13fd3296, GetPackageName(), nullptr);
                LoadHudTextures();
            } else {
                ShowHudObjects();
                cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
            }
            if (gCarCustomizeManager.GetTempColoredPart()) {
                installed_part = gCarCustomizeManager.GetTempColoredPart()->GetPart();
                part_found = true;
            }
            SetTitleHash(0x78980a6b);
            icon_hash = 0x28f88bc;
            if (CustomizeIsInBackRoom()) {
                icon_hash = 0x8ba602fc;
            }
            car_slot_id = 0x84;
            goto after_switch;
        case 0x304:
            SetTitleHash(0xd32729a6);
            icon_hash = 0x3f23165c;
            car_slot_id = 0x83;
            goto after_switch;
        case 0x402:
            SetTitleHash(0xd9228fc6);
            icon_hash = 0xf8148554;
            car_slot_id = 0x4d;
            vinyl_group_number = 0;
            goto set_vinyl;
        case 0x403:
            SetTitleHash(0x1e8d885f);
            icon_hash = 0x192d84da;
            car_slot_id = 0x4d;
            vinyl_group_number = 1;
            goto set_vinyl;
        case 0x404:
            SetTitleHash(0x1c619fd8);
            icon_hash = 0xf7352706;
            car_slot_id = 0x4d;
            vinyl_group_number = 2;
            goto set_vinyl;
        case 0x405:
            SetTitleHash(0x9c1b8935);
            icon_hash = 0x1223cc89;
            car_slot_id = 0x4d;
            vinyl_group_number = 3;
            goto set_vinyl;
        case 0x406:
            SetTitleHash(0x7956f7b0);
            icon_hash = 0xbc44bbcb;
            car_slot_id = 0x4d;
            vinyl_group_number = 4;
            goto set_vinyl;
        case 0x407:
            SetTitleHash(0x2d5bff0f);
            icon_hash = 0x694ca0ca;
            car_slot_id = 0x4d;
            vinyl_group_number = 5;
            goto set_vinyl;
        case 0x408:
            SetTitleHash(0x209a9158);
            icon_hash = 0x1b3a8dd3;
            car_slot_id = 0x4d;
            vinyl_group_number = 6;
            goto set_vinyl;
        case 0x409:
            SetTitleHash(0xcd057d21);
            icon_hash = 0x1ba508fc;
            car_slot_id = 0x4d;
            vinyl_group_number = 7;
            goto set_vinyl;
        default:
            goto after_switch;
    }

set_vinyl:
    is_vinyl = true;

after_switch:
    if (is_vinyl && gCarCustomizeManager.GetTempColoredPart()) {
        installed_part = gCarCustomizeManager.GetTempColoredPart()->GetPart();
        part_found = true;
    }
    if (!part_found) {
        installed_part = gCarCustomizeManager.GetActivePartFromSlot(car_slot_id);
    }

    bTList<SelectablePart> part_list;
    if (is_vinyl) {
        gCarCustomizeManager.GetCarPartList(car_slot_id, part_list, vinyl_group_number);
    } else {
        gCarCustomizeManager.GetCarPartList(car_slot_id, part_list, 0);
    }

    int installed_index;
    int current_part_index;
    unsigned int original_icon_hash;
    SelectablePart *part;
    installed_index = 0;
    current_part_index = 1;
    original_icon_hash = icon_hash;
    part = part_list.GetHead();

    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        unsigned int unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), part->GetUpgradeLevel());

        if (is_vinyl) {
            CarPart *cpart = part->GetPart();
            unsigned char gl = cpart->GroupNumber_UpgradeLevel;
            if ((gl & 0x1f) == vinyl_group_number) {
                unsigned int part_name_hash = static_cast<unsigned int>(cpart->PartNameHashTop) << 16 | cpart->PartNameHashBot;
                if (UnlockSystem::IsUnlockableAvailable(part_name_hash)) {
                    unsigned int upgrade_level = gl >> 5;
                    bool locked = gCarCustomizeManager.IsPartLocked(part, 0);
                    AddPartOption(part, icon_hash, upgrade_level, 0, unlock_hash, locked);
                } else {
                    delete part;
                    part = nullptr;
                }
            } else {
                delete part;
                part = nullptr;
            }
        } else {
            CarPart *cpart = part->GetPart();
            if (cpart->HasAppliedAttribute(bStringHash("CARBONFIBRE"))) {
                int cfVal = cpart->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0);
                if (cfVal != 0) {
                    if (Category != 0x104) {
                        if (Category == 0x105) {
                            if (CustomizeIsInBackRoom()) {
                                icon_hash = 0xcd6b4e26;
                            } else {
                                icon_hash = 0xfc618215;
                            }
                        } else {
                            icon_hash = original_icon_hash;
                        }
                    } else {
                        if (CustomizeIsInBackRoom()) {
                            icon_hash = 0x2478e136;
                        } else {
                            icon_hash = 0x68495926;
                        }
                    }
                } else {
                    icon_hash = original_icon_hash;
                }
            } else {
                icon_hash = original_icon_hash;
            }
            unsigned int upgrade_level = cpart->GroupNumber_UpgradeLevel >> 5;
            bool locked = gCarCustomizeManager.IsPartLocked(part, 0);
            AddPartOption(part, icon_hash, upgrade_level, 0, unlock_hash, locked);
        }
        if (part) {
            if (installed_part && part->GetPart() == installed_part) {
                installed_index = current_part_index;
            }
            current_part_index++;
        }
    }

    if (Showcase::FromIndex != 0) {
        SetInitialOption(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(installed_index);
    }
    RefreshHeader();

    while (!part_list.IsEmpty()) {
        delete part_list.RemoveHead();
    }
}

void CustomizeParts::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    int numOpts = Options.Options.CountElements();
    if (numOpts != Options.iNumBookEnds) {
        SelectablePart *sel = GetSelectedPart();
        if (sel->GetPart()->HasAppliedAttribute(0x6212682b)) {
            unsigned int tunable = sel->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0);
            if (tunable) {
                FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x8098a54c);
            } else {
                FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x649f4a65);
            }
        }
        if (Category == 0x307) {
            SetHUDTextures();
            SetHUDColors();
        } else {
            if ((RealTimer - ScrollTime).GetSeconds() > 0.3f) {
                gCarCustomizeManager.PreviewPart(sel->GetSlotID(), sel->GetPart());
            } else {
                bNeedsRefresh = true;
            }
        }
        const char *lang_str = "LANGUAGEHASH";
        if (sel->GetPart()->HasAppliedAttribute(bStringHash(lang_str))) {
            const char *pkg = GetPackageName();
            unsigned int langHash = sel->GetPart()->GetAppliedAttributeUParam(bStringHash(lang_str), 0);
            FEngSetLanguageHash(pkg, 0x5e7b09c9, langHash);
        } else {
            FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", sel->GetPart()->GetName());
        }
    }
}

// --- CustomizeMeter ---

CustomizeMeter::CustomizeMeter()
    : Min(0.0f) //
      ,
      Max(1.0f) //
      ,
      Current(0.0f) //
      ,
      Preview(0.0f) //
      ,
      PreviousPreview(0.0f) //
      ,
      NumStages(5) //
      ,
      pMultiplier(nullptr) //
      ,
      pMeterGroup(nullptr) //
{
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

void CustomizeMeter::Draw() {
    float stage_size = 1.0f;
    float multiplier = 5.0f;
    float stage_bottom = Min;
    while (Preview - stage_bottom >= stage_size) {
        stage_bottom = stage_bottom + stage_size;
        multiplier = multiplier + stage_size;
    }
    float heat_level = bMin(multiplier, 5.0f);
    unsigned int hash = FEngHashString("HEAT_X%.0f", heat_level);
    FEngSetTextureHash(pMultiplier, hash);
    hash = FEngHashString("HEAT_X%.0f", heat_level);
    FEngSetTextureHash(pMultiplierZoom, hash);
    if (Preview != PreviousPreview) {
        FEngSetScript(pMultiplierZoom, 0x209c24, true);
    }
    float segment_bottom = stage_bottom + stage_size;
    float segment_size = 0.1f;
    if (stage_bottom <= segment_bottom) {
        int cur_icon = 10;
        do {
            float current_segment = segment_bottom - stage_size * segment_size;
            segment_bottom = current_segment;
            int icon_idx = cur_icon - 1;
            unsigned int script;
            if (current_segment + 0.0005f < Current) {
                script = 0x63c;
                if (Preview <= current_segment + 0.0005f) {
                    script = 0x13ff6c;
                }
            } else {
                script = 0xccfa;
            }
            FEngSetScript(pBases[cur_icon - 1], script, true);
            if (stage_bottom > current_segment || icon_idx <= -1)
                break;
            cur_icon = icon_idx;
        } while (true);
    }
}

// --- CustomizeSub ---

int CustomizeSub::GetRimBrandIndex(unsigned int brand) {
    switch (brand) {
        case 0x352d08d1:
            return 2;
        case 0x9136:
            return 3;
        case 0x9536:
            return 4;
        case 0x2b77feb:
            return 5;
        case 0x324ac97:
            return 6;
        case 0x48e25793:
            return 7;
        case 0xdd544a02:
            return 8;
        case 0x648:
            return 9;
        case 0x1e6a3b:
            return 10;
        case 0x1c386b:
            return 0xb;
        default:
            return 1;
    }
}

void CustomizeSub::SetupRimBrands() {
    BackToPkg = g_pCustomizeSubPkg;
    TitleHash = 0xe032d89e;
    CarPart *stockCarPart = gCarCustomizeManager.GetStockCarPart(0x42);
    SelectablePart *stockPart = new SelectablePart(stockCarPart, 0x42, 0, static_cast<GRace::Type>(7), false, CPS_AVAILABLE, 0, false);
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockPart->SetPartState(CPS_INSTALLED);
    }
    SetStockPartOption *stockOpt = new SetStockPartOption(stockPart, 0xf3990b6, 0x701);
    AddOption(stockOpt);

    AddCustomOption(g_pCustomizeRimsPkg, 0xb0da3de4, 0x56b51a0e, 0x702);
    AddCustomOption(g_pCustomizeRimsPkg, 0xf224a729, 0xf93f2d34, 0x703);
    AddCustomOption(g_pCustomizeRimsPkg, 0xf224ab29, 0xf93f3134, 0x704);
    AddCustomOption(g_pCustomizeRimsPkg, 0xe38de9e, 0x460d1369, 0x705);
    AddCustomOption(g_pCustomizeRimsPkg, 0xea60b4a, 0x467a4015, 0x706);
    AddCustomOption(g_pCustomizeRimsPkg, 0xafc6b9cb, 0x9bb17a11, 0x707);
    AddCustomOption(g_pCustomizeRimsPkg, 0x27ebd095, 0xcca3063f, 0x708);
    AddCustomOption(g_pCustomizeRimsPkg, 0x6c2fa9db, 0xc1bc1a86, 0x709);
    AddCustomOption(g_pCustomizeRimsPkg, 0x36c53e8e, 0x213085f9, 0x70a);
    AddCustomOption(g_pCustomizeRimsPkg, 0x36c2d130, 0x212e5429, 0x70b);

    ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x42));
    if (inCart) {
        InCartPartOptionIndex = GetRimBrandIndex(inCart->GetBuyingPart()->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0));
    }
    CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x42);
    if (installed) {
        InstalledPartOptionIndex = GetRimBrandIndex(installed->GetAppliedAttributeUParam(0xebb03e66, 0));
    }

    if (FromCategory == 0x801) {
        int pos = InCartPartOptionIndex;
        if (pos == 0) {
            pos = InstalledPartOptionIndex;
        }
        if (pos != 0) {
            SetInitialOption(pos);
        } else {
            SetInitialOption(1);
        }
    } else {
        SetInitialOption(FromCategory & 0xFFFF00FF);
    }
    if (FromCategory - 0x701u < 0xbu) {
        FromCategory = 0x801;
    }
}

void CustomizeSub::SetupVinylGroups() {
    TitleHash = 0xda129752;
    BackToPkg = g_pCustomizeSubPkg;
    SelectablePart *stockPart =
        new SelectablePart(static_cast<CarPart *>(nullptr), 0x4d, 0, static_cast<GRace::Type>(7), false, CPS_AVAILABLE, 0, false);
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockPart->SetPartState(CPS_INSTALLED);
    }
    SetStockPartOption *stockOpt = new SetStockPartOption(stockPart, 0x21f3d114, 0x401);
    AddOption(stockOpt);

    AddCustomOption(g_pCustomizePartsPkg, 0xf8148554, 0xd9228fc6, 0x402);
    AddCustomOption(g_pCustomizePartsPkg, 0x192d84da, 0x1e8d885f, 0x403);
    AddCustomOption(g_pCustomizePartsPkg, 0xf7352706, 0x1c619fd8, 0x404);
    AddCustomOption(g_pCustomizePartsPkg, 0x1223cc89, 0x9c1b8935, 0x405);
    AddCustomOption(g_pCustomizePartsPkg, 0xbc44bbcb, 0x7956f7b0, 0x406);
    AddCustomOption(g_pCustomizePartsPkg, 0x694ca0ca, 0x2d5bff0f, 0x407);
    AddCustomOption(g_pCustomizePartsPkg, 0x1b3a8dd3, 0x209a9158, 0x408);
    AddCustomOption(g_pCustomizePartsPkg, 0x1ba508fc, 0xcd057d21, 0x409);

    ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x4d));
    if (inCart && inCart->GetBuyingPart()) {
        CarPart *part = inCart->GetBuyingPart()->GetPart();
        if (part) {
            InCartPartOptionIndex = GetVinylGroupIndex(part->GetGroupNumber());
        } else {
            InCartPartOptionIndex = 1;
        }
    }
    CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x4d);
    if (installed) {
        InstalledPartOptionIndex = GetVinylGroupIndex(installed->GetGroupNumber());
    } else {
        InstalledPartOptionIndex = 1;
    }

    if (FromCategory == 0x803) {
        if (InCartPartOptionIndex != 0) {
            SetInitialOption(InCartPartOptionIndex);
        } else if (InstalledPartOptionIndex != 0) {
            SetInitialOption(InstalledPartOptionIndex);
        } else {
            SetInitialOption(1);
        }
    } else {
        SetInitialOption(FromCategory & 0xFFFF00FF);
    }
    if (FromCategory - 0x401u < 9u) {
        FromCategory = 0x803;
    }
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
    switch (msg) {
        case 0x9120409e:
        case 0xb5971bf1:
            BuildColorOptions();
            RefreshHeader();
            break;
        case 0x72619778:
            ScrollColors(eSD_PREV);
            break;
        case 0x911c0a4b:
            ScrollColors(eSD_NEXT);
            break;
        case 0x91dfdf84: {
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
            break;
        }
        case 0x911ab364:
            gCarCustomizeManager.ClearTempColoredPart();
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeHudPkg, Category | (FromCategory << 16), 0, false);
            break;
        case 0xcf91aacd:
            gCarCustomizeManager.ClearTempColoredPart();
            bNeedsRefresh = true;
            break;
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
    switch (sel->ThePart->CarSlotID) {
        case 0x85: {
            unsigned int color = sel->color;
            FEngSetColor(FEngFindObject(GetPackageName(), 0x5d19f25), color);
            break;
        }
        case 0x87: {
            unsigned int color = sel->color;
            FEngSetColor(FEngFindObject(GetPackageName(), 0xc0721eb9), color);
            color = SelectedColor->color;
            FEngSetColor(FEngFindObject(GetPackageName(), 0xc62ad685), color);
            color = SelectedColor->color;
            FEngSetColor(FEngFindObject(GetPackageName(), 0xb8f1f802), color);
            break;
        }
        case 0x86: {
            unsigned int color = sel->color;
            FEngSetColor(FEngFindObject(GetPackageName(), 0xd312f0cb), color);
            color = SelectedColor->color;
            FEngSetColor(FEngFindObject(GetPackageName(), 0x8fe2a217), color);
            break;
        }
    }
}

void CustomizeHUDColor::BuildColorOptions() {
    if (SelectedColor) {
        FEngSetScript(SelectedColor->FEngObject, 0x7ab5521a, true);
        SelectedColor = nullptr;
    }
    HUDLayerOption *opt = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
    if (opt && !opt->TheColors.IsEmpty()) {
        ColorOptions.DeleteAllElements();
        int i = 0;
        ShoppingCartItem *cart_item = gCarCustomizeManager.IsPartTypeInCart(0x84u);
        CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(0x84);
        SelectablePart *part = opt->TheColors.GetHead();
        while (part != opt->TheColors.EndOfList()) {
            i++;
            HUDColorOption *color_option = new HUDColorOption(part);
            FEImage *obj = FEngFindImage(GetPackageName(), FEngHashString("COLOR_%d", i));
            color_option->SetFEngObject(obj);
            ColorOptions.AddTail(color_option);
            unsigned char r = part->GetPart()->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            unsigned char g = part->GetPart()->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            unsigned char b = part->GetPart()->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            unsigned int color = static_cast<unsigned int>(g) << 8;
            color |= 0xFF000000;
            color |= static_cast<unsigned int>(r) << 16;
            color |= static_cast<unsigned int>(b);
            color_option->color = color;
            FEngSetColor(obj, color);
            if (!opt->SelectedPart) {
                if (cart_item && gCarCustomizeManager.GetTempColoredPart()->GetPart() == cart_item->GetBuyingPart()->GetPart()) {
                    if (gCarCustomizeManager.IsPartInCart(part)) {
                        SelectedColor = color_option;
                        opt->SelectedPart = part;
                    }
                } else if (gCarCustomizeManager.GetTempColoredPart()->GetPart() == installed_hud) {
                    if (gCarCustomizeManager.IsPartInstalled(part)) {
                        SelectedColor = color_option;
                        opt->SelectedPart = part;
                    }
                }
            } else if (opt->SelectedPart == part) {
                SelectedColor = color_option;
            }
            part = part->GetNext();
        }
        if (!SelectedColor) {
            SelectedColor = ColorOptions.GetHead();
        }
        float x_offset = 69.0f;
        float y_offset = 56.0f;
        FEObject *cursor_obj = Cursor;
        float x = FEngGetTopLeftX(SelectedColor->FEngObject);
        x += x_offset;
        float y = FEngGetTopLeftY(SelectedColor->FEngObject);
        FEngSetTopLeft(cursor_obj, x, y + y_offset);
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
    const char *pkg = GetPackageName();
    unsigned int hash = FEngHashString("HUD_NEEDLE_%d_%02d", TachRPM, hudStyle);
    FEngSetTextureHash(FEngFindImage(pkg, 0xc0721eb9), hash);
    pkg = GetPackageName();
    hash = FEngHashString("HUD_GAUGE_%02d", hudStyle);
    FEngSetTextureHash(FEngFindImage(pkg, 0x5d19f25), hash);
    pkg = GetPackageName();
    hash = FEngHashString("HUD_SPEEDOMETER_%02d", hudStyle);
    FEngSetTextureHash(FEngFindImage(pkg, 0xd312f0cb), hash);
    if (gCarCustomizeManager.IsTurbo()) {
        pkg = GetPackageName();
        hash = FEngHashString("HUD_NEEDLE_TURBO_%02d", hudStyle);
        FEngSetTextureHash(FEngFindImage(pkg, 0xc62ad685), hash);
        pkg = GetPackageName();
        hash = FEngHashString("HUD_NOS_%02d", hudStyle);
        FEngSetTextureHash(FEngFindImage(pkg, 0x8fe2a217), hash);
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xc5d551b7));
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xc5d551b7));
    }
}

void CustomizeParts::SetHUDColors() {
    ShoppingCartItem *hudInCart = gCarCustomizeManager.IsPartTypeInCart(0x84u);
    CarPart *installedHud = gCarCustomizeManager.GetInstalledCarPart(0x84);
    SelectablePart *sel = GetSelectedPart();
    if (sel->ThePart != installedHud) {
        if (!hudInCart)
            goto not_installed;
        SelectablePart *selPart = GetSelectedPart();
        if (selPart->ThePart != hudInCart->GetBuyingPart()->ThePart)
            goto not_installed;
    }
    {
        unsigned int colors[3];
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
            unsigned char r = colorPart->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            unsigned char g = colorPart->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            unsigned int b = colorPart->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            colors[idx] = (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | 0xff000000 | (b & 0xff);
            slot++;
            idx++;
        } while (idx < 3);
        FEngSetColor(FEngFindObject(GetPackageName(), 0x5d19f25), colors[0]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xc0721eb9), colors[2]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xc62ad685), colors[2]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xb8f1f802), colors[2]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0xd312f0cb), colors[1]);
        FEngSetColor(FEngFindObject(GetPackageName(), 0x8fe2a217), colors[1]);
        return;
    }
not_installed:
    FEngSetColor(FEngFindObject(GetPackageName(), 0x5d19f25), 0xffffc373u);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xc0721eb9), 0xffffffffu);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xc62ad685), 0xffffffffu);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xb8f1f802), 0xffffffffu);
    FEngSetColor(FEngFindObject(GetPackageName(), 0xd312f0cb), 0xffffae40u);
    FEngSetColor(FEngFindObject(GetPackageName(), 0x8fe2a217), 0xffffae40u);
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
    const char *pkg = GetPackageName();
    unsigned int hash = FEngHashString("HUD_NEEDLE_%d_%02d", tachRPM, hudStyle);
    FEngSetTextureHash(FEngFindImage(pkg, 0xc0721eb9), hash);
    pkg = GetPackageName();
    hash = FEngHashString("HUD_GAUGE_%02d", hudStyle);
    FEngSetTextureHash(FEngFindImage(pkg, 0x5d19f25), hash);
    pkg = GetPackageName();
    hash = FEngHashString("HUD_SPEEDOMETER_%02d", hudStyle);
    FEngSetTextureHash(FEngFindImage(pkg, 0xd312f0cb), hash);
    if (gCarCustomizeManager.IsTurbo()) {
        pkg = GetPackageName();
        hash = FEngHashString("HUD_NEEDLE_TURBO_%02d", hudStyle);
        FEngSetTextureHash(FEngFindImage(pkg, 0xc62ad685), hash);
        pkg = GetPackageName();
        hash = FEngHashString("HUD_NOS_%02d", hudStyle);
        FEngSetTextureHash(FEngFindImage(pkg, 0x8fe2a217), hash);
        FEngSetVisible(FEngFindObject(GetPackageName(), 0xc5d551b7));
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xc5d551b7));
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
    switch (msg) {
        case 0xc519bfbf:
            Showcase::FromFilter = InnerRadius;
            break;
        case 0x5073ef13:
            ScrollRimSizes(eSD_PREV);
            break;
        case 0xd9feec59:
            ScrollRimSizes(eSD_NEXT);
            break;
        case 0x5a928018: {
            SelectablePart *sel = FindInCartPart();
            if (sel && !gCarCustomizeManager.IsPartInCart(sel)) {
                sel->PartState = static_cast<eCustomizePartState>(sel->PartState & CPS_GAME_STATE_MASK);
                RefreshHeader();
            }
            break;
        }
        case 0x406415e3:
            break;
        case 0x911ab364:
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubTopPkg, FromCategory | (Category << 16), 0, false);
            break;
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
        if (radius > MaxRadius) {
            radius = MinRadius;
        }
    }
    if (radius != InnerRadius) {
        IconScroller *options = &Options;
        InnerRadius = radius;
        int sel;
        if (options->pCurrentNode) {
            sel = options->GetCurrentIndex();
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
    InnerRadius = gCarCustomizeManager.GetMinInnerRadius();
    MinRadius = InnerRadius;
    MaxRadius = gCarCustomizeManager.GetMaxInnerRadius();
    CarPart *activePart = gCarCustomizeManager.GetActivePartFromSlot(0x42);
    if (Showcase::FromFilter != -1) {
        InnerRadius = Showcase::FromFilter;
        Showcase::FromFilter = -1;
    } else if (activePart) {
        InnerRadius = static_cast<signed char>(activePart->GetAppliedAttributeIParam(0xeb0101e2, 0));
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
    gCarCustomizeManager.GetCarPartList(0x42, tempList, GetCategoryBrandHash());
    if (selected_index == -1) {
        activeMatch = gCarCustomizeManager.GetActivePartFromSlot(0x42);
    }
    while (tempList.GetHead() != reinterpret_cast<bNode *>(&tempList)) {
        SelectablePart *node = static_cast<SelectablePart *>(tempList.GetHead());
        node->Prev->Next = node->Next;
        node->Next->Prev = node->Prev;
        signed char rimSize = static_cast<signed char>(node->ThePart->GetAppliedAttributeIParam(0xeb0101e2, 0));
        if (static_cast<int>(rimSize) == InnerRadius) {
            unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), node->GetUpgradeLevel());
            unsigned char gl = *reinterpret_cast<unsigned char *>(reinterpret_cast<int>(node->ThePart) + 5);
            unsigned int wheelPos = gl >> 5;
            bool locked = gCarCustomizeManager.IsPartLocked(node, 0);
            AddPartOption(node, 0x294d2a3, wheelPos, 0, unlockHash, locked);
            if (activeMatch && node->ThePart == activeMatch) {
                matchIdx = curIdx;
            }
            curIdx++;
        } else {
            delete node;
        }
    }
    if (selected_index == -1) {
        selected_index = 1;
        if (activeMatch) {
            selected_index = matchIdx;
        }
    }
    if (Showcase::FromIndex != 0) {
        if (bFadeInIconsImmediately) {
            Options.bFadingIn = true;
            Options.bFadingOut = false;
            Options.bDelayUpdate = false;
            Options.fCurFadeTime = 0.0f;
        }
        Options.SetInitialPos(0);
        Showcase::FromIndex = 0;
    } else {
        if (bFadeInIconsImmediately) {
            Options.bFadingIn = true;
            Options.bFadingOut = false;
            Options.bDelayUpdate = false;
            Options.fCurFadeTime = 0.0f;
        }
        Options.SetInitialPos(selected_index);
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
    int numOpts = Options.Options.CountElements();
    if (numOpts != Options.iNumBookEnds) {
        SelectablePart *part = GetSelectedPart();
        gCarCustomizeManager.PreviewPart(part->GetSlotID(), part->GetPart());
        FEPrintf(GetPackageName(), 0xe6782841, "%$d\"", InnerRadius);
        char buf[64];
        const char *name = part->GetPart()->GetName();
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
        case 0x702:
            return 0x352d08d1;
        case 0x703:
            return 0x9136;
        case 0x704:
            return 0x9536;
        case 0x705:
            return 0x2b77feb;
        case 0x706:
            return 0x324ac97;
        case 0x707:
            return 0x48e25793;
        case 0x708:
            return 0xdd544a02;
        case 0x709:
            return 0x648;
        case 0x70a:
            return 0x1e6a3b;
        case 0x70b:
            return 0x1c386b;
        default:
            return 0;
    }
}

// --- CustomizeNumbers ---

CustomizeNumbers::CustomizeNumbers(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      LeftNumberList() //
      ,
      RightNumberList() //
      ,
      TheLeftNumber(nullptr) //
      ,
      TheRightNumber(nullptr) //
      ,
      LeftDisplayValue(-1) //
      ,
      RightDisplayValue(-1) //
      ,
      bLeft(1) //
      ,
      DisplayHelper(sd->PackageFilename) {
    Category = sd->Arg & 0xFFFF;
    FromCategory = static_cast<int>(static_cast<short>(sd->Arg >> 16));
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
        if ((TheLeftNumber->PartState & CPS_GAME_STATE_MASK) == CPS_LOCKED && (TheRightNumber->PartState & CPS_GAME_STATE_MASK) == CPS_LOCKED) {
            state = CPS_LOCKED;
        } else if ((TheLeftNumber->PartState & CPS_GAME_STATE_MASK) == CPS_NEW && (TheRightNumber->PartState & CPS_GAME_STATE_MASK) == CPS_NEW) {
            state = CPS_NEW;
        }
        if ((TheLeftNumber->PartState & CPS_INSTALLED) != 0 && (TheRightNumber->PartState & CPS_INSTALLED) != 0) {
            state = static_cast<eCustomizePartState>(state | CPS_INSTALLED);
        } else if ((TheLeftNumber->PartState & CPS_IN_CART) != 0 && (TheRightNumber->PartState & CPS_IN_CART) != 0) {
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
        } else if (inCart && !inCart->GetBuyingPart()->ThePart) {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_IN_CART);
        } else {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_AVAILABLE);
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
                if (bShowcaseOn == 1 && Showcase::FromIndex == leftIdx) {
                    TheLeftNumber = node;
                    if (gCarCustomizeManager.IsPartInCart(node)) {
                        TheLeftNumber->PartState = static_cast<eCustomizePartState>(TheLeftNumber->PartState | CPS_IN_CART);
                    }
                    LeftDisplayValue = static_cast<short>(leftIdx);
                    leftFound = true;
                    Showcase::FromIndex = 0;
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
                if (bShowcaseOn == 1 && Showcase::FromFilter == rightIdx) {
                    TheRightNumber = node;
                    if (gCarCustomizeManager.IsPartInCart(node)) {
                        TheRightNumber->PartState = static_cast<eCustomizePartState>(TheRightNumber->PartState | CPS_IN_CART);
                    }
                    RightDisplayValue = static_cast<short>(rightIdx);
                    rightFound = true;
                    Showcase::FromFilter = -1;
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
    switch (msg) {
        case 0x35f8620b:
            DisplayHelper.SetInitComplete(true);
            FEngSetCurrentButton(GetPackageName(), 0x2a08ba92);
            break;
        case 0xc519bfbf:
            Showcase::FromFilter = static_cast<int>(RightDisplayValue);
            Showcase::FromIndex = static_cast<int>(LeftDisplayValue);
            Showcase::FromArgs = Category | (FromCategory << 16);
            Showcase::FromPackage = GetPackageName();
            bShowcaseOn = 1;
            cFEng_mInstance->QueuePackageSwitch("Showcase.fng", gCarCustomizeManager.TuningCar->FEKey, 0, false);
            break;
        case 0xb5af2461:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
        case 0x9120409e:
        case 0xb5971bf1: {
            bLeft = !bLeft;
            FEngSetCurrentButton(GetPackageName(), bLeft ? 0x2a08ba92 : 0x1a88dc05);
            break;
        }
        case 0x72619778:
            ScrollNumbers(eSD_NEXT);
            break;
        case 0x911c0a4b:
            ScrollNumbers(eSD_PREV);
            break;
        case 0x406415e3:
            if (LeftDisplayValue == -1 || RightDisplayValue == -1)
                return;
            if (!TheLeftNumber || !TheRightNumber)
                return;
            if (TheLeftNumber->IsLocked() && TheRightNumber->IsLocked()) {
                DisplayHelper.FlashStatusIcon(CPS_LOCKED, true);
            } else if (TheLeftNumber->IsInCartX() && TheRightNumber->IsInCartX()) {
                DisplayHelper.FlashStatusIcon(CPS_IN_CART, true);
            } else if (TheLeftNumber->IsInstalledX() && TheRightNumber->IsInstalledX()) {
                DisplayHelper.FlashStatusIcon(CPS_INSTALLED, true);
            } else {
                cFEng_mInstance->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xff);
                return;
            }
            break;
        case 0xc519bfc3: {
            CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x71);
            if (installed) {
                UnsetShoppingCart();
                SelectablePart stockPart(nullptr, 0x71, 0, static_cast<GRace::Type>(7), false, CPS_AVAILABLE, 0, false);
                gCarCustomizeManager.AddToCart(&stockPart);
                stockPart.CarSlotID = 0x72;
                gCarCustomizeManager.AddToCart(&stockPart);
                stockPart.CarSlotID = 0x69;
                gCarCustomizeManager.AddToCart(&stockPart);
                stockPart.CarSlotID = 0x6a;
                gCarCustomizeManager.AddToCart(&stockPart);
            } else {
                if ((TheLeftNumber->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART ||
                    (TheRightNumber->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART) {
                    UnsetShoppingCart();
                    ShoppingCartItem *current = gCarCustomizeManager.GetFirstCartItem();
                    ShoppingCartItem *last = gCarCustomizeManager.GetLastCartItem();
                    while (current) {
                        ShoppingCartItem *next = static_cast<ShoppingCartItem *>(current->Next);
                        int slotID = current->GetBuyingPart()->GetSlotID();
                        if (slotID == 0x71 || slotID == 0x72 || slotID == 0x69 || slotID == 0x6a) {
                            gCarCustomizeManager.RemoveFromCart(current);
                        }
                        if (current == last)
                            break;
                        current = next;
                    }
                }
            }
            RightDisplayValue = -1;
            TheLeftNumber = static_cast<SelectablePart *>(LeftNumberList.GetHead());
            TheRightNumber = static_cast<SelectablePart *>(RightNumberList.GetHead());
            LeftDisplayValue = -1;
            RefreshHeader();
            break;
        }
        case 0x91dfdf84:
            UnsetShoppingCart();
            TheLeftNumber->PartState = static_cast<eCustomizePartState>(TheLeftNumber->PartState | CPS_IN_CART);
            TheRightNumber->PartState = static_cast<eCustomizePartState>(TheRightNumber->PartState | CPS_IN_CART);
            gCarCustomizeManager.AddToCart(TheLeftNumber);
            gCarCustomizeManager.AddToCart(TheRightNumber);
            {
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
            }
            RefreshHeader();
            break;
        case 0xcf91aacd: {
            SelectablePart *lnode = static_cast<SelectablePart *>(LeftNumberList.GetHead());
            SelectablePart *lsentinel = reinterpret_cast<SelectablePart *>(&LeftNumberList);
            while (lnode != lsentinel) {
                if (lnode->ThePart == gCarCustomizeManager.GetInstalledCarPart(0x69) ||
                    lnode->ThePart == gCarCustomizeManager.GetInstalledCarPart(0x6a)) {
                    lnode->PartState = static_cast<eCustomizePartState>((lnode->PartState & CPS_GAME_STATE_MASK) | CPS_INSTALLED);
                }
                lnode = static_cast<SelectablePart *>(lnode->Next);
            }
            SelectablePart *rnode = static_cast<SelectablePart *>(RightNumberList.GetHead());
            SelectablePart *rsentinel = reinterpret_cast<SelectablePart *>(&RightNumberList);
            while (rnode != rsentinel) {
                if (rnode->ThePart == gCarCustomizeManager.GetInstalledCarPart(0x71) ||
                    rnode->ThePart == gCarCustomizeManager.GetInstalledCarPart(0x72)) {
                    rnode->PartState = static_cast<eCustomizePartState>((rnode->PartState & CPS_GAME_STATE_MASK) | CPS_INSTALLED);
                }
                rnode = static_cast<SelectablePart *>(rnode->Next);
            }
            CustomizeShoppingCart::ExitShoppingCart();
            break;
        }
        case 0x5a928018: {
            ShoppingCartItem *leftInCart = gCarCustomizeManager.IsPartTypeInCart(0x69u);
            ShoppingCartItem *rightInCart = gCarCustomizeManager.IsPartTypeInCart(0x6au);
            if (!leftInCart && !rightInCart) {
                SelectablePart *lnode = static_cast<SelectablePart *>(LeftNumberList.GetHead());
                SelectablePart *lsentinel = reinterpret_cast<SelectablePart *>(&LeftNumberList);
                for (; lnode != lsentinel; lnode = static_cast<SelectablePart *>(lnode->Next)) {
                    if ((lnode->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART) {
                        lnode->PartState = static_cast<eCustomizePartState>(lnode->PartState & CPS_GAME_STATE_MASK);
                        break;
                    }
                }
                SelectablePart *rnode = static_cast<SelectablePart *>(RightNumberList.GetHead());
                SelectablePart *rsentinel = reinterpret_cast<SelectablePart *>(&RightNumberList);
                for (; rnode != rsentinel; rnode = static_cast<SelectablePart *>(rnode->Next)) {
                    if ((rnode->PartState & CPS_PLAYER_STATE_MASK) == CPS_IN_CART) {
                        rnode->PartState = static_cast<eCustomizePartState>(rnode->PartState & CPS_GAME_STATE_MASK);
                        break;
                    }
                }
            }
            RefreshHeader();
            break;
        }
        case 0x911ab364:
            bShowcaseOn = 0;
            cFEng_mInstance->QueuePackageSwitch(g_pCustomizeSubPkg, FromCategory | (Category << 16), 0, false);
            break;
    }
}

// --- CustomizeDecals ---

unsigned int CustomizeDecals::GetSlotIDFromCategory() {
    switch (CurrentDecalLocation) {
        case 0x501:
            return 0x53;
        case 0x502:
            return 0x5b;
        case 0x503:
            switch (Category) {
                case 0x601:
                    return 0x63;
                case 0x602:
                    return 0x64;
                case 0x603:
                    return 0x65;
                case 0x604:
                    return 0x66;
                case 0x605:
                    return 0x67;
                case 0x606:
                    return 0x68;
            }
            // fall through
        case 0x504:
            switch (Category) {
                case 0x601:
                    return 0x6b;
                case 0x602:
                    return 0x6c;
                case 0x603:
                    return 0x6d;
                case 0x604:
                    return 0x6e;
                case 0x605:
                    return 0x6f;
                case 0x606:
                    return 0x70;
                default:
                    return 0x73;
            }
        case 0x505:
            return 0x73;
        case 0x506:
            return 0x7b;
        default:
            return 0x53;
    }
}

void CustomizeDecals::Setup() {
    unsigned int slotID = GetSlotIDFromCategory();
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    unsigned int titleHash = 0;
    switch (Category) {
        case 0x501:
            titleHash = 0x301dedd3;
            break;
        case 0x502:
            titleHash = 0x48e6ca49;
            break;
        case 0x505:
            titleHash = 0x8a7697d6;
            break;
        case 0x506:
            titleHash = 0xb1f9b0c9;
            break;
        case 0x601:
            titleHash = 0x7d212cfa;
            break;
        case 0x602:
            titleHash = 0x7d212cfb;
            break;
        case 0x603:
            titleHash = 0x7d212cfc;
            break;
        case 0x604:
            titleHash = 0x7d212cfd;
            break;
        case 0x605:
            titleHash = 0x7d212cfe;
            break;
        case 0x606:
            titleHash = 0x7d212cff;
            break;
        default:
            break;
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
    if (Showcase::FromFilter != -1) {
        bIsBlack = (Showcase::FromFilter != 0);
        Showcase::FromFilter = -1;
    }
    BuildDecalList(selectedHash);
    RefreshHeader();
}

void CustomizeDecals::BuildDecalList(unsigned int selected_name_hash) {
    int matchIdx = 1;
    int curIdx = 2;
    bNeedsRefresh = true;
    ScrollTime = 0;
    Options.RemoveAll();
    Options.AddInitialBookEnds();

    unsigned int slotID = GetSlotIDFromCategory();
    bTList<SelectablePart> tempList;
    SelectablePart *stockPart =
        new SelectablePart(nullptr, slotID, 0, static_cast<GRace::Type>(7), false, static_cast<eCustomizePartState>(1), 0, false);
    eCustomizePartState stockState = CPS_AVAILABLE;
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockState = static_cast<eCustomizePartState>(0x11);
    } else if (gCarCustomizeManager.IsPartInCart(stockPart)) {
        stockState = static_cast<eCustomizePartState>(0x21);
    }
    stockPart->PartState = stockState;
    AddPartOption(stockPart, 0x697b4ad4, 0x60a662f5, 0, 0, false);

    gCarCustomizeManager.GetCarPartList(slotID, tempList, 0);

    int unlockLevel = MapCarPartToUnlockable(slotID, nullptr);
    if (unlockLevel == 0x2e) {
        unlockLevel = 2;
    } else if (unlockLevel > 0x2e) {
        if (unlockLevel == 0x30) {
            unlockLevel = 3;
        }
    } else if (unlockLevel == 0x2c) {
        unlockLevel = 1;
    }

    SelectablePart *node = static_cast<SelectablePart *>(tempList.GetHead());
    SelectablePart *sentinel = reinterpret_cast<SelectablePart *>(&tempList);
    while (node != sentinel) {
        SelectablePart *next = static_cast<SelectablePart *>(node->Next);
        unsigned int nameHash = node->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0);
        if (!bIsBlack) {
            nameHash = bStringHash("_WHITE", nameHash);
        }
        unsigned int mirrorHash = node->GetPart()->GetAppliedAttributeUParam(bStringHash("DECAL_MIRROR_HASH"), 0);
        if (nameHash == mirrorHash) {
            unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), unlockLevel);
            node->Prev->Next = node->Next;
            node->Next->Prev = node->Prev;
            bool locked = gCarCustomizeManager.IsPartLocked(node, 0);
            AddPartOption(node, 0x697b4ad4, nameHash, 0, unlockHash, locked);
            if (node->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0) == selected_name_hash) {
                matchIdx = curIdx;
            }
            curIdx++;
        } else {
            node->Prev->Next = node->Next;
            node->Next->Prev = node->Prev;
            delete node;
        }
        node = next;
    }
    if (Showcase::FromIndex != 0) {
        SetInitialOption(0);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(matchIdx);
    }

    while (tempList.GetHead() != tempList.EndOfList()) {
        SelectablePart *sp = static_cast<SelectablePart *>(tempList.GetHead());
        sp->Remove();
        delete sp;
    }
}

void CustomizeDecals::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xc519bfbf:
            Showcase::FromFilter = bIsBlack;
            break;
        case 0x5073ef13:
        case 0xd9feec59:
            bIsBlack ^= 1;
            {
                CustomizePartOption *opt = GetSelectedOption();
                if (opt->GetPart()) {
                    unsigned int nameHash = opt->GetPart()->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0);
                    BuildDecalList(nameHash);
                } else {
                    BuildDecalList(0);
                }
            }
            RefreshHeader();
            break;
        case 0x5a928018: {
            SelectablePart *sel = GetSelectedPart();
            if (sel) {
                if (gCarCustomizeManager.IsPartInCart(sel)) {
                    return;
                }
                sel->PartState = static_cast<eCustomizePartState>(sel->PartState & 0xf);
                RefreshHeader();
            }
            break;
        }
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, FromCategory | Category << 16, 0, false);
            break;
        case 0xc519bfc3:
            return;
    }
}

void CustomizeDecals::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    SelectablePart *sel = GetSelectedPart();
    if (sel->GetPart() == nullptr) {
        FEngSetLanguageHash(GetPackageName(), 0x5e7b09c9, Options.GetCurrentName());
    } else {
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", GetSelectedPart()->GetPart()->GetName());
    }
    unsigned int hash = 0x436a98e9;
    if (bIsBlack) {
        hash = 0x41f0a3a5;
    }
    FEngSetLanguageHash(GetPackageName(), 0x889bacb6, hash);
    if ((RealTimer - ScrollTime).GetSeconds() <= 0.3f) {
        bNeedsRefresh = true;
    } else {
        unsigned int slotId = GetSlotIDFromCategory();
        SelectablePart *cur = GetSelectedPart();
        gCarCustomizeManager.PreviewPart(slotId, cur->GetPart());
    }
}

// --- CustomizePaint helpers ---

unsigned int CustomizePaint::CalcBrandHash(CarPart *part) {
    if (Category == 0x301) {
        switch (TheFilter) {
            case 0:
                return 0x02daab07;
            case 1:
                return 0x03437a52;
            case 2:
                return 0x03797533;
            default:
                return part->GetAppliedAttributeUParam(0xebb03e66, 0);
        }
    } else if (Category == 0x303) {
        return 0xda27;
    }
    return 0x3e871f1;
}

CustomizePaint::CustomizePaint(ScreenConstructorData *sd)
    : CustomizationScreen(sd) //
      ,
      TheFilter(-1) //
      ,
      MatchingPaint(nullptr, 0, 0, 0, 0) //
      ,
      ThePaints(sd->PackageFilename, 20, 4, true) {
    NumRemapColors = 0;
    for (int i = 0; i <= 2; i++) {
        VinylColors[i] = nullptr;
    }
    ThePaints.SetMouseDownMsg(0x406415e3);
    Setup();
}

void CustomizePaint::Setup() {
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    for (int i = 1; i <= 0x50; i++) {
        ArraySlot *slot = new ArraySlot(FEngFindImage(GetPackageName(), static_cast<int>(FEngHashString("COLOR_%d", i))));
        ThePaints.AddSlot(slot);
    }
    for (int i = 0; i < 3; i++) {
        SelectedIndex[i] = -1;
    }
    if (Showcase::FromFilter != -1) {
        TheFilter = Showcase::FromFilter;
    }
    switch (Category) {
        case 0x301:
            cFEng::Get()->QueuePackageMessage(0x1a7240f3, GetPackageName(), nullptr);
            DisplayHelper.TitleHash = 0x55da70c;
            SetupBasePaint();
            break;
        case 0x303:
            DisplayHelper.TitleHash = 0xe126ff53;
            SetupRimPaint();
            break;
        case 0x402:
        case 0x403:
        case 0x404:
        case 0x405:
        case 0x406:
        case 0x407:
        case 0x408:
        case 0x409:
            DisplayHelper.TitleHash = 0xd8ee1a80;
            SetupVinylColor();
            break;
    }
    Showcase::FromFilter = -1;
    Options.bInitialized = true;
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
    if (static_cast<unsigned int>(maybe - 0x29) < 2) {
        if (Category == 0x303) {
            return static_cast<eMenuSoundTriggers>(-1);
        }
        SelectablePart *temp = gCarCustomizeManager.GetTempColoredPart();
        if (temp) {
            CarPart *part = temp->GetPart();
            if (part && part->GetAppliedAttributeUParam(0x6212682b, 0) < 2) {
                return static_cast<eMenuSoundTriggers>(-1);
            }
        }
    }
    return maybe;
}

void CustomizePaint::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0x406415e3:
            if (Category == 0x301 || Category == 0x303) {
                CustomizationScreen::NotificationMessage(0x406415e3, pobj, param1, param2);
            }
            break;
        default:
            CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
            break;
        case 0x9120409e:
        case 0xb5971bf1:
            break;
    }

    ThePaints.NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case 0xc519bfbf:
            Showcase::FromFilter = TheFilter;
            Showcase::FromIndex = ThePaints.GetCurrentDatumNum();
            for (int i = 0; i < 3; i++) {
                Showcase::FromColor[i] = VinylColors[i];
            }
            break;
        case 0x5073ef13:
            ScrollFilters(static_cast<eScrollDir>(-1));
            break;
        case 0xd9feec59:
            ScrollFilters(static_cast<eScrollDir>(1));
            break;
        case 0x406415e3:
            if (Category == 0x301 || Category == 0x303) {
                return;
            }
            {
                CarPart *installed = gCarCustomizeManager.GetTempColoredPart()->GetPart();
                if (VinylColors[TheFilter]) {
                    delete VinylColors[TheFilter];
                }
                int savedFilter = TheFilter;
                bool add_to_cart = false;
                SelectablePart *newPart = new SelectablePart(gCarCustomizeManager.GetTempColoredPart());
                VinylColors[savedFilter] = newPart;
                if (installed != gCarCustomizeManager.GetActivePartFromSlot(0x4d)) {
                    add_to_cart = true;
                } else {
                    for (int i = 0; i < NumRemapColors; i++) {
                        CarPart *active = gCarCustomizeManager.GetActivePartFromSlot(i + 0x4f);
                        if (VinylColors[i] && active != VinylColors[i]->GetPart()) {
                            add_to_cart = true;
                            break;
                        }
                    }
                }
                if (!add_to_cart) {
                    return;
                }
                AddVinylAndColorsToCart();
            }
            {
                unsigned int cat = Category | (FromCategory << 16);
                cFEng::Get()->QueuePackageSwitch(g_pCustomizePartsPkg, cat, 0, false);
            }
            break;
        case 0x911ab364:
            if (Category == 0x301 || Category == 0x303) {
                unsigned int cat = Category | (FromCategory << 16);
                cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, cat, 0, false);
                return;
            }
            for (int i = 0; i < 3; i++) {
                if (VinylColors[i]) {
                    delete VinylColors[i];
                }
                VinylColors[i] = nullptr;
                Showcase::FromColor[i] = nullptr;
            }
            gCarCustomizeManager.ResetPreview();
            {
                unsigned int cat = Category | (FromCategory << 16);
                cFEng::Get()->QueuePackageSwitch(g_pCustomizePartsPkg, cat, 0, false);
            }
            break;
        case 0x5a928018: {
            SelectablePart *part = GetSelectedPart();
            if (part && !gCarCustomizeManager.IsPartInCart(part)) {
                part->UnSetInCart();
                RefreshHeader();
            }
            RefreshHeader();
            break;
        }
        case 0x9120409e:
        case 0xb5971bf1:
        case 0x911c0a4b:
        case 0x72619778:
            RefreshHeader();
            break;
        case 0xcf91aacd:
            for (int i = 0; i < 3; i++) {
                if (VinylColors[i]) {
                    delete VinylColors[i];
                }
                VinylColors[i] = nullptr;
            }
            break;
    }
}

void CustomizePaint::ScrollFilters(eScrollDir dir) {
    int maxFilter;
    if (Category == 0x301) {
        maxFilter = 2;
    } else if (Category == 0x303) {
        return;
    } else {
        maxFilter = NumRemapColors - 1;
        if (maxFilter != 0) {
            SelectablePart *current = GetSelectedPart();
            if (current != VinylColors[TheFilter]) {
                if (VinylColors[TheFilter]) {
                    delete VinylColors[TheFilter];
                }
                int savedFilter = TheFilter;
                SelectablePart *newPart = new SelectablePart(gCarCustomizeManager.GetTempColoredPart());
                VinylColors[savedFilter] = newPart;
            }
        }
    }
    int cur = TheFilter;
    int next;
    if (dir == static_cast<eScrollDir>(-1)) {
        next = cur - 1;
        if (next < 0) {
            next = maxFilter;
        }
    } else if (dir == static_cast<eScrollDir>(1)) {
        next = cur + 1;
        if (next > maxFilter) {
            next = 0;
        }
    } else {
        next = cur;
    }
    if (next != cur) {
        SelectedIndex[TheFilter] = ThePaints.GetCurrentDatumNum() - 1;
        TheFilter = next;
        if (Category == 0x301 || Category == 0x303) {
            SelectablePart *sel = GetSelectedPart();
            BuildSwatchList(sel->GetSlotID());
        } else {
            BuildSwatchList(next + 0x4f);
        }
        RefreshHeader();
    }
}

void CustomizePaint::SetupVinylColor() {
    unsigned int slot = 0x4f;
    if (Showcase::FromFilter != -1) {
        if (Showcase::FromFilter == 1) {
            slot = 0x50;
        } else if (Showcase::FromFilter == 2) {
            slot = 0x51;
        }
        Showcase::FromFilter = -1;
    }
    BuildSwatchList(slot);
    CarPart *activePart = gCarCustomizeManager.GetActivePartFromSlot(0x4d);
    NumRemapColors = activePart->GetAppliedAttributeUParam(0x6212682b, 0);
    if (NumRemapColors < 2) {
        FEObject *obj = FEngFindObject(PackageFilename, 0x2c3cc2d3);
        FEngSetInvisible(obj);
        obj = FEngFindObject(PackageFilename, 0x53639a10);
        FEngSetInvisible(obj);
    } else {
        cFEng::Get()->QueuePackageMessage(0x1a7240f3, PackageFilename, nullptr);
    }
    for (int i = 0; i < 3; i++) {
        int slotID = i + 0x4f;
        if (!Showcase::FromColor[i]) {
            CarPart *active = gCarCustomizeManager.GetActivePartFromSlot(slotID);
            if (!active) {
                VinylColors[i] = nullptr;
            } else {
                SelectablePart *sp = new SelectablePart(active, slotID, active->GetUpgradeLevel(), static_cast<GRace::Type>(7), false,
                                                        static_cast<eCustomizePartState>(1), 0, false);
                VinylColors[i] = sp;
            }
        } else {
            VinylColors[i] = static_cast<SelectablePart *>(Showcase::FromColor[i]);
            Showcase::FromColor[i] = nullptr;
        }
    }
}

SelectablePart *CustomizePaint::GetSelectedPart() {
    return static_cast<CustomizePaintDatum *>(ThePaints.GetCurrentDatum())->ThePart;
}

void CustomizePaint::BuildSwatchList(unsigned int slot) {
    CarPart *matchPart = nullptr;
    ThePaints.ClearData();
    int colorIndex = 0;
    switch (slot) {
        case 0x4f:
            colorIndex = 0;
            break;
        case 0x50:
            colorIndex = 1;
            break;
        case 0x51:
            colorIndex = 2;
            break;
        default:
            goto skip_color;
    }
    if (Showcase::FromColor[colorIndex] && !VinylColors[colorIndex]) {
        matchPart = static_cast<SelectablePart *>(Showcase::FromColor[colorIndex])->GetPart();
    }
skip_color:
    if (!matchPart) {
        matchPart = gCarCustomizeManager.GetActivePartFromSlot(slot);
    }
    unsigned int brand = CalcBrandHash(matchPart);
    if (TheFilter == -1) {
        if (brand == 0x2daab07) {
            TheFilter = 0;
        } else if (brand > 0x2daab07) {
            if (brand == 0x3437a52) {
                TheFilter = 1;
            } else if (brand == 0x3797533) {
                TheFilter = 2;
            }
        } else if (brand == 0xda27) {
            TheFilter = 0;
        } else {
            TheFilter = 0;
        }
    }
    bTList<SelectablePart> partList;
    gCarCustomizeManager.GetCarPartList(slot, partList, 0);
    int datumIndex = 0;
    while (partList.GetHead() != partList.EndOfList()) {
        SelectablePart *sp = static_cast<SelectablePart *>(partList.GetHead());
        unsigned int partBrand = sp->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0);
        if (partBrand == brand) {
            sp->Remove();
            unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category),
                                                                         static_cast<unsigned int>(sp->GetPart()->GroupNumber_UpgradeLevel >> 5));
            CustomizePaintDatum *datum = new CustomizePaintDatum(sp, unlockHash);
            if (SelectedIndex[TheFilter] == -1 && matchPart == sp->GetPart()) {
                SelectedIndex[TheFilter] = datumIndex;
            }
            ThePaints.AddDatum(datum);
            ArraySlot *aslot = ThePaints.GetSlotAt(datumIndex);
            if (aslot) {
                unsigned char r = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                unsigned char g = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                unsigned char b = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                FEngSetColor(aslot->GetFEngObject(), 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));
            }
            datumIndex++;
        } else {
            sp->Remove();
            delete sp;
        }
    }
    if (Showcase::FromIndex != 0) {
        SelectedIndex[TheFilter] = Showcase::FromIndex - 1;
        Showcase::FromIndex = 0;
    } else {
        if (SelectedIndex[TheFilter] == -1) {
            SelectedIndex[TheFilter] = 0;
        }
        ThePaints.SetInitialPosition(SelectedIndex[TheFilter]);
    }
    RefreshHeader();
    while (partList.GetHead() != partList.EndOfList()) {
        SelectablePart *sp = static_cast<SelectablePart *>(partList.GetHead());
        sp->Remove();
        delete sp;
    }
}

void CustomizePaint::RefreshHeader() {
    DisplayHelper.DrawTitle();
    ThePaints.RefreshHeader();
    int filter = TheFilter;
    unsigned int hash = 0;
    switch (filter) {
        case 0:
            if (Category == 0x301) {
                hash = 0xb6763cde;
            } else if (NumRemapColors == 2) {
                hash = 0x5198ba16;
            } else if (NumRemapColors == 3) {
                hash = 0x5198ba17;
            } else {
                hash = 0xd8ee1a80;
            }
            break;
        case 1:
            if (Category == 0x301) {
                hash = 0x452b5481;
            } else if (NumRemapColors == 2) {
                hash = 0x5198be57;
            } else if (NumRemapColors == 3) {
                hash = 0x5198be58;
            }
            break;
        case 2:
            if (Category == 0x301) {
                hash = 0xb715070a;
            } else if (NumRemapColors == 3) {
                hash = 0x5198c299;
            }
            break;
    }
    FEngSetLanguageHash(PackageFilename, 0x78008599, hash);
    if (Category == 0x301) {
        SelectablePart *sel = GetSelectedPart();
        int slotID = sel->GetSlotID();
        sel = GetSelectedPart();
        gCarCustomizeManager.PreviewPart(slotID, sel->GetPart());
    } else if (Category == 0x303) {
        SelectablePart *sel = GetSelectedPart();
        int slotID = sel->GetSlotID();
        sel = GetSelectedPart();
        gCarCustomizeManager.PreviewPart(slotID, sel->GetPart());
        FEObject *obj = FEngFindObject(PackageFilename, 0x2c526172);
        FEngSetInvisible(obj);
        FEngSetLanguageHash(PackageFilename, 0x78008599, 0xb3100a3e);
    } else {
        gCarCustomizeManager.PreviewPart(gCarCustomizeManager.GetTempColoredPart()->GetSlotID(),
                                         gCarCustomizeManager.GetTempColoredPart()->GetPart());
        if (NumRemapColors == 1) {
            FEObject *obj = FEngFindObject(PackageFilename, 0x2c526172);
            FEngSetInvisible(obj);
        }
        for (int i = 0; i < 3; i++) {
            if (i < NumRemapColors && VinylColors[i]) {
                gCarCustomizeManager.PreviewPart(VinylColors[i]->GetSlotID(), VinylColors[i]->GetPart());
            }
        }
        SelectablePart *sel = GetSelectedPart();
        int slotID = sel->GetSlotID();
        sel = GetSelectedPart();
        gCarCustomizeManager.PreviewPart(slotID, sel->GetPart());
    }
    SelectablePart *sel = GetSelectedPart();
    DisplayHelper.SetCareerStuff(sel, Category, 0);
    sel = GetSelectedPart();
    unsigned int unlockBlurb = static_cast<CustomizePaintDatum *>(ThePaints.GetCurrentDatum())->UnlockBlurb;
    int curNum = ThePaints.GetCurrentDatumNum();
    int totalNum = ThePaints.GetCurrentDatumNum();
    DisplayHelper.SetPartStatus(sel, unlockBlurb, curNum, totalNum);
}

CustomizePaint::~CustomizePaint() {
    MatchingPaint.ThePart = nullptr;
}

CustomizePaintDatum::~CustomizePaintDatum() {
    if (ThePart) {
        delete ThePart;
    }
}

unsigned int CustomizePerformance::GetPerfPkgDesc(Physics::Upgrades::Type type, int level, int num_packages, bool has_turbo) {
    if (level == 0) {
        switch (type) {
            case static_cast<Physics::Upgrades::Type>(0):
                return 0xe5c1020c;
            case static_cast<Physics::Upgrades::Type>(1):
                return 0x927db4fd;
            case static_cast<Physics::Upgrades::Type>(2):
                return 0x8c96b853;
            case static_cast<Physics::Upgrades::Type>(3):
                return 0x2f525e4f;
            case static_cast<Physics::Upgrades::Type>(4):
                return 0xe74dedbb;
            case static_cast<Physics::Upgrades::Type>(5):
                if (has_turbo)
                    return 0x5317eb31;
                return 0x704a6d50;
            case static_cast<Physics::Upgrades::Type>(6):
                return 0x9a0ef8f9;
            default:
                return 0;
        }
    }
    const char *fmt;
    switch (type) {
        case static_cast<Physics::Upgrades::Type>(0):
            fmt = "PD_TIRES_%d_%d";
            break;
        case static_cast<Physics::Upgrades::Type>(1):
            fmt = "PD_BRAKES_%d_%d";
            break;
        case static_cast<Physics::Upgrades::Type>(2):
            fmt = "PD_CHASSIS_%d_%d";
            break;
        case static_cast<Physics::Upgrades::Type>(3):
            fmt = "PD_TRANSMISSION_%d_%d";
            break;
        case static_cast<Physics::Upgrades::Type>(4):
            if (gCarCustomizeManager.IsCastrolCar() && level == 4 && num_packages == 3) {
                return FEngHashString("PD_ENGINE_%d_%d_CASTROL", 4, 3);
            }
            if (gCarCustomizeManager.IsRotaryCar() && (level == 2 || level == 4) && num_packages == 1) {
                return FEngHashString("PD_ENGINE_%d_%d_ROTARY", level, 1);
            }
            fmt = "PD_ENGINE_%d_%d";
            break;
        case static_cast<Physics::Upgrades::Type>(5):
            if (!has_turbo) {
                fmt = "PD_SUPERCHARGER_%d_%d";
            } else {
                fmt = "PD_TURBO_%d_%d";
            }
            break;
        case static_cast<Physics::Upgrades::Type>(6):
            fmt = "PD_NITROUS_%d_%d";
            break;
        default:
            return 0;
    }
    return FEngHashString(fmt, level, num_packages);
}

unsigned int CustomizePerformance::GetPerfPkgBrand(Physics::Upgrades::Type type, int level, int num_packages) {
    unsigned int hash = 0;
    CarCustomizeManager *mgr = &gCarCustomizeManager;
    Attrib::Gen::frontend inst(mgr->GetTuningCar()->FEKey, 0, nullptr);
    Attrib::Instance *inst_ptr = &inst;
    unsigned int *ptr = nullptr;
    switch (type) {
        case static_cast<Physics::Upgrades::Type>(0):
            switch (level) {
                case 0:
                    hash = 0xad6a0504;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xf0c7c400, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x1e6ddf1, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x92378a0a, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x16b700d6, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        case static_cast<Physics::Upgrades::Type>(1):
            switch (level) {
                case 0:
                    hash = 0xa1a5e9e5;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xe4af1260, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x70b14851, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x8e8b78e1, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xb4df5439, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        case static_cast<Physics::Upgrades::Type>(2):
            switch (level) {
                case 0:
                    hash = 0xad6a0504;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x37ea2169, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xe5650914, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xe321687d, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xfb1ef23f, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        case static_cast<Physics::Upgrades::Type>(3):
            switch (level) {
                case 0:
                    hash = 0x98ed935e;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x1e823f0b, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x79c8d7e9, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xa1b53a33, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xf424c06d, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        case static_cast<Physics::Upgrades::Type>(4):
            if (mgr->IsCastrolCar() && level == 4 && num_packages == 2) {
                return 0xb95d4df;
            }
            switch (level) {
                case 0:
                    hash = 0x7d0ac98f;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x512303af, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xdb8a8a1d, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x4f56a655, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x85ab21da, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        case static_cast<Physics::Upgrades::Type>(5):
            switch (level) {
                case 0:
                    hash = 0x9e8f71ad;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xe141cde, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x4d3b62f3, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xea7f3fe4, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xb6be1d52, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        case static_cast<Physics::Upgrades::Type>(6):
            switch (level) {
                case 0:
                    hash = 0x98ed935e;
                    goto done;
                case 1:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x7f6e85a3, num_packages));
                    break;
                case 2:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xd810d2dc, num_packages));
                    break;
                case 3:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0xa459ecef, num_packages));
                    break;
                case 4:
                    ptr = static_cast<unsigned int *>(inst_ptr->GetAttributePointer(0x8da087a4, num_packages));
                    break;
                default:
                    goto done;
            }
            break;
        default:
            goto done;
    }
    if (!ptr) {
        ptr = static_cast<unsigned int *>(Attrib::DefaultDataArea(4));
    }
    hash = *ptr;
done:
    return hash;
}

#endif // FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_CARCUSTOMIZE_H
