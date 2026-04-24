// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/FEng/FEString.h"

extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern FEObject *FEngFindObject(const char *pkg, unsigned int hash);
extern FEImage *FEngFindImage(const char *pkg, int hash);
extern void FEngSetTextureHash(FEImage *img, unsigned int hash);
extern bool CustomizeIsInBackRoom();
extern void CustomizeSetInParts(bool b);
extern void CustomizeSetInPerformance(bool b);
extern int GetCurrentLanguage();
extern const char *GetLocalizedString(unsigned int hash);
extern int FEPrintf(const char *pkg, int hash, const char *fmt, ...);
extern int bSNPrintf(char *buf, int size, const char *fmt, ...);

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

extern CarCustomizeManager gCarCustomizeManager;
extern FEMarkerManager TheFEMarkerManager;

// --- CustomizeMeter ---

void CustomizeMeter::SetCurrent(float current) {
    Current = bMin(bMax(current, Min), Max);
}

void CustomizeMeter::SetPreview(float preview) {
    PreviousPreview = Preview;
    Preview = bMin(bMax(preview, Min), Max);
}

void CustomizeMeter::SetVisibility(bool b) {
    if (b) {
        FEngSetVisible(pMeterGroup);
    } else {
        FEngSetInvisible(pMeterGroup);
    }
}

// --- FEShoppingCartItem ---

void FEShoppingCartItem::Show() {
    FEStatWidget::Show();
    FEngSetVisible(pTradeInPrice);
    FEngSetVisible(pCheckIcon);
}

void FEShoppingCartItem::Hide() {
    FEStatWidget::Hide();
    FEngSetInvisible(pTradeInPrice);
    FEngSetInvisible(pCheckIcon);
}

// --- CustomizeSub ---

CustomizeMainOption *CustomizeSub::FindInCartOption() {
    if (InCartPartOptionIndex) {
        return static_cast<CustomizeMainOption *>(Options.GetOption(InCartPartOptionIndex));
    }
    return nullptr;
}

// --- CustomizeParts ---

void CustomizeParts::LoadHudTextures() {
    PacksLoadedCount = 0;
    LoadNextHudTexturePack();
}

// --- CustomizePaint ---

void CustomizePaint::SetupBasePaint() {
    BuildSwatchList(0x4C);
}

// --- CustomizeSub Setup functions ---

void CustomizeSub::Setup() {
    switch (Category) {
        case 0x801: SetupParts(); break;
        case 0x802: SetupPerformance(); break;
        case 0x803: SetupVisual(); break;
        case 0x302: SetupVinylGroups(); break;
        case 0x305: SetupDecalLocations(); break;
        case 0x103: SetupRimBrands(); break;
        case 0x501: case 0x502: case 0x503:
        case 0x504: case 0x505: case 0x506:
            SetupDecalPositions(); break;
    }
    RefreshHeader();
}

void CustomizeSub::SetupParts() {
    if (CustomizeIsInBackRoom()) {
        TitleHash = 0x5d285ae7;
    } else {
        TitleHash = 0x055dce1a;
    }
    CustomizeSetInParts(true);
    BackToPkg = g_pCustomizeMainPkg;
    if (!CustomizeIsInBackRoom()) {
        AddCustomOption(g_pCustomizePartsPkg, 0x028c24f6, 0x6134c218, 0x101);
        AddCustomOption(g_pCustomizeSpoilerPkg, 0xbb034ea6, 0x94e73021, 0x102);
        AddCustomOption(g_pCustomizeSubTopPkg, 0x0294d2a3, 0xf868eb0b, 0x103);
        AddCustomOption(g_pCustomizePartsPkg, 0x028f7092, 0x04d4a88d, 0x104);
        AddCustomOption(g_pCustomizePartsPkg, 0x79165861, 0x61e8f83c, 0x105);
    } else {
        AddCustomOption(g_pCustomizePartsPkg, 0xaf393dba, 0x6134c218, 0x101);
        AddCustomOption(g_pCustomizeSpoilerPkg, 0xc51a4f62, 0x94e73021, 0x102);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xc19491cc, 0xf868eb0b, 0x103);
        AddCustomOption(g_pCustomizePartsPkg, 0xf375276e, 0x04d4a88d, 0x104);
        AddCustomOption(g_pCustomizePartsPkg, 0x25a4375e, 0x61e8f83c, 0x105);
    }
    SetInitialOption(FromCategory & 0xFFFF00FF);
}

void CustomizeSub::SetupPerformance() {
    if (CustomizeIsInBackRoom()) {
        TitleHash = 0xbfd5b50f;
    } else {
        TitleHash = 0xbaef8282;
    }
    BackToPkg = g_pCustomizeMainPkg;
    CustomizeSetInPerformance(true);
    if (!CustomizeIsInBackRoom()) {
        AddCustomOption(g_pCustomizePerfPkg, 0xc15c94e6, 0x9853d9a6, 0x201);
        AddCustomOption(g_pCustomizePerfPkg, 0x01a29ffa, 0x29aa74ba, 0x202);
        AddCustomOption(g_pCustomizePerfPkg, 0x178475e7, 0x6e101aa7, 0x203);
        AddCustomOption(g_pCustomizePerfPkg, 0x9701bde4, 0x4ce19aa4, 0x204);
        AddCustomOption(g_pCustomizePerfPkg, 0x06e8e477, 0x05aa9137, 0x205);
        AddCustomOption(g_pCustomizePerfPkg, 0xbaa23a28, 0x91997ee8, 0x206);
        if (gCarCustomizeManager.IsTurbo()) {
            AddCustomOption(g_pCustomizePerfPkg, 0x06ef789c, 0x05b1255c, 0x207);
        } else {
            AddCustomOption(g_pCustomizePerfPkg, 0x93603dfb, 0xbb6812bb, 0x207);
        }
    } else {
        AddCustomOption(g_pCustomizePerfPkg, 0x4f424e0f, 0x9853d9a6, 0x201);
        AddCustomOption(g_pCustomizePerfPkg, 0xd142d3e3, 0x29aa74ba, 0x202);
        AddCustomOption(g_pCustomizePerfPkg, 0x00190eb6, 0x6e101aa7, 0x203);
        AddCustomOption(g_pCustomizePerfPkg, 0x6fea04c8, 0x4ce19aa4, 0x204);
        AddCustomOption(g_pCustomizePerfPkg, 0x7373f1ef, 0x05aa9137, 0x205);
        AddCustomOption(g_pCustomizePerfPkg, 0x4887f351, 0x91997ee8, 0x206);
        if (gCarCustomizeManager.IsTurbo()) {
            AddCustomOption(g_pCustomizePerfPkg, 0x12fe30a5, 0x05b1255c, 0x207);
        } else {
            AddCustomOption(g_pCustomizePerfPkg, 0x630071e4, 0xbb6812bb, 0x207);
        }
    }
    SetInitialOption(FromCategory & 0xFFFF00FF);
}

void CustomizeSub::SetupVisual() {
    if (CustomizeIsInBackRoom()) {
        TitleHash = 0x10c3fe31;
    } else {
        TitleHash = 0xbfa7d7c4;
    }
    BackToPkg = g_pCustomizeMainPkg;
    if (!CustomizeIsInBackRoom()) {
        AddCustomOption(g_pCustomizePaintPkg, 0xa3b76154, 0x055da70c, 0x301);
        AddCustomOption(g_pCustomizeSubTopPkg, 0x55778e5a, 0xbfa52c55, 0x302);
        if (!gCarCustomizeManager.IsHeroCar()) {
            AddCustomOption(g_pCustomizePaintPkg, 0xd223f84a, 0xe126ff53, 0x303);
        }
        AddCustomOption(g_pCustomizePartsPkg, 0x3f23165c, 0xd32729a6, 0x304);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xda1dae54, 0x955980bc, 0x305);
        AddCustomOption("FeCustomize_ToolBox", 0x45a1c644, 0x6857e5ac, 0x306);
        AddCustomOption(g_pCustomizeHudPkg, 0x028f88bc, 0x78980a6b, 0x307);
    } else {
        AddCustomOption(g_pCustomizePaintPkg, 0x0db89e17, 0x055da70c, 0x301);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xd35f04c0, 0xbfa52c55, 0x302);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xa9135927, 0x955980bc, 0x305);
        AddCustomOption(g_pCustomizeHudPkg, 0x8ba602fc, 0x78980a6b, 0x307);
    }
    SetInitialOption(FromCategory & 0xFFFF00FF);
}

void CustomizeSub::SetupDecalLocations() {
    TitleHash = 0x9de6e6e1;
    BackToPkg = g_pCustomizeSubPkg;
    AddCustomOption(g_pCustomizeDecalsPkg, 0x52ded91d, 0x301dedd3, 0x501);
    AddCustomOption(g_pCustomizeDecalsPkg, 0xac7937b4, 0x48e6ca49, 0x502);
    AddCustomOption(g_pCustomizeSubTopPkg, 0xda88b711, 0x34367c86, 0x503);
    AddCustomOption(g_pCustomizeSubTopPkg, 0xc9a967c4, 0xddf80259, 0x504);
    AddCustomOption(g_pCustomizeDecalsPkg, 0x2c710c4d, 0x8a7697d6, 0x505);
    AddCustomOption(g_pCustomizeDecalsPkg, 0xffa7d360, 0xb1f9b0c9, 0x506);
    if (FromCategory == 0x803) {
        SetInitialOption(1);
    } else {
        SetInitialOption(FromCategory & 0xFFFF00FF);
    }
    if (FromCategory - 0x501u < 6u) {
        FromCategory = 0x803;
    }
}

void CustomizeSub::SetupDecalPositions() {
    TitleHash = 0x74d1887d;
    BackToPkg = g_pCustomizeSubTopPkg;
    switch (Category) {
    case 0x503:
        AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f48, 0x7d212cfa, 0x601);
        AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f49, 0x7d212cfb, 0x602);
        AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4a, 0x7d212cfc, 0x603);
        AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4b, 0x7d212cfd, 0x604);
        AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4c, 0x7d212cfe, 0x605);
        AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4d, 0x7d212cff, 0x606);
        break;
    case 0x504:
        AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadb, 0x7d212cfa, 0x601);
        AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadc, 0x7d212cfb, 0x602);
        AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadd, 0x7d212cfc, 0x603);
        AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eade, 0x7d212cfd, 0x604);
        AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadf, 0x7d212cfe, 0x605);
        AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eae0, 0x7d212cff, 0x606);
        break;
    }
    if (FromCategory == 0x305) {
        SetInitialOption(1);
    } else {
        SetInitialOption(FromCategory & 0xFFFF00FF);
        FromCategory = 0x305;
    }
}

void CustomizeSub::RefreshHeader() {
    CustomizeCategoryScreen::RefreshHeader();
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
    FEPrintf(GetPackageName(), 0xb71b576d, "%s", buf);
    if (Category == 0x103 || Category == 0x302) {
        int sel = Options.GetCurrentIndex();
        if (sel == InCartPartOptionIndex) {
            FEngSetVisible(FEngFindObject(GetPackageName(), 0xd0582feb));
            FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xd0582feb), 0x1a777e25);
        } else if (sel == InstalledPartOptionIndex) {
            FEngSetVisible(FEngFindObject(GetPackageName(), 0xd0582feb));
            FEngSetTextureHash(FEngFindImage(GetPackageName(), 0xd0582feb), 0x696ae039);
        } else {
            FEngSetInvisible(FEngFindObject(GetPackageName(), 0xd0582feb));
        }
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xd0582feb));
    }
    if (!gCarCustomizeManager.IsCareerMode() && Category == 0x802) {
        FEngSetVisible(FEngFindObject(GetPackageName(), 0x5aec8d91));
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x5aec8d91));
    }
}

// --- Free functions ---

int TranslateCustomizeCatToMarker(eCustomizeCategory cat) {
    switch (static_cast<unsigned int>(cat)) {
    case CC_BODY_KIT:      return FEMarkerManager::MARKER_BODY;
    case CC_SPOILERS:      return FEMarkerManager::MARKER_SPOILER;
    case CC_HOODS:         return FEMarkerManager::MARKER_HOOD;
    case CC_ROOF_SCOOPS:   return FEMarkerManager::MARKER_ROOF_SCOOP;
    case CC_CUSTOM_HUD:    return FEMarkerManager::MARKER_CUSTOM_HUD;
    case CC_ENGINE:        return FEMarkerManager::MARKER_ENGINE;
    case CC_TRANSMISSION:  return FEMarkerManager::MARKER_TRANSMISSION;
    case CC_SUSPENSION:    return FEMarkerManager::MARKER_CHASSIS;
    case CC_NITROUS:       return FEMarkerManager::MARKER_NOS;
    case CC_TIRES:         return FEMarkerManager::MARKER_TIRES;
    case CC_BRAKES:        return FEMarkerManager::MARKER_BRAKES;
    case CC_FORCED_INDUCTION: return FEMarkerManager::MARKER_INDUCTION;
    case CC_PAINT:
    case CC_RIM_PAINT:     return FEMarkerManager::MARKER_PAINT;
    case CC_VINYL_TYPES:
    case CC_VINYL_GROUP_FLAME:
    case CC_VINYL_GROUP_TRIBAL:
    case CC_VINYL_GROUP_STRIPE:
    case CC_VINYL_GROUP_RACING_FLAG:
    case CC_VINYL_GROUP_NATIONAL_FLAG:
    case CC_VINYL_GROUP_BODY:
    case CC_VINYL_GROUP_UNIQUE:
    case CC_VINYL_GROUP_CONTEST:
        return FEMarkerManager::MARKER_VINYL;
    case CC_RIM_BRANDS:
    case CC_RIM_BRAND_5_ZIGEN:
    case CC_RIM_BRAND_ADR:
    case CC_RIM_BRAND_BBS:
    case CC_RIM_BRAND_ENKEI:
    case CC_RIM_BRAND_KONIG:
    case CC_RIM_BRAND_LOWENHART:
    case CC_RIM_BRAND_RACING_HART:
    case CC_RIM_BRAND_OZ:
    case CC_RIM_BRAND_VOLK:
    case CC_RIM_BRAND_ROJA:
        return FEMarkerManager::MARKER_RIMS;
    case CC_DECAL_LOCATION:
    case CC_DECAL_WINDSHIELD:
    case CC_DECAL_REAR_WINDOW:
    case CC_DECAL_LEFT_DOOR:
    case CC_DECAL_RIGHT_DOOR:
    case CC_DECAL_LEFT_QP:
    case CC_DECAL_RIGHT_QP:
    case CC_DECAL_SLOT_1:
    case CC_DECAL_SLOT_2:
    case CC_DECAL_SLOT_3:
    case CC_DECAL_SLOT_4:
    case CC_DECAL_SLOT_5:
    case CC_DECAL_SLOT_6:
        return FEMarkerManager::MARKER_DECAL;
    default:
        return 0;
    }
}

unsigned int GetMarkerNameFromCategory(eCustomizeCategory cat) {
    switch (static_cast<unsigned int>(cat)) {
    case CC_PARTS:         return 0xd3a2fbe1;
    case CC_PERFORMANCE:   return 0x3c27a989;
    case CC_VISUAL:        return 0x5692be6b;
    case CC_BODY_KIT:      return 0x7c50498c;
    case CC_SPOILERS:      return 0x52012995;
    case CC_RIM_BRANDS:
    case CC_RIM_BRAND_5_ZIGEN:
    case CC_RIM_BRAND_ADR:
    case CC_RIM_BRAND_BBS:
    case CC_RIM_BRAND_ENKEI:
    case CC_RIM_BRAND_KONIG:
    case CC_RIM_BRAND_LOWENHART:
    case CC_RIM_BRAND_RACING_HART:
    case CC_RIM_BRAND_OZ:
    case CC_RIM_BRAND_VOLK:
    case CC_RIM_BRAND_ROJA:
        return 0x8a4bfbf2;
    case CC_HOODS:         return 0x8a4699e1;
    case CC_ROOF_SCOOPS:   return 0x830100f0;
    case CC_CUSTOM_HUD:    return 0xc253ec92;
    case CC_ENGINE:        return 0x2f3ec04d;
    case CC_TRANSMISSION:  return 0xd1e77ca1;
    case CC_SUSPENSION:    return 0xb7cbfcce;
    case CC_NITROUS:       return 0xc129562b;
    case CC_TIRES:         return 0xd3efbefe;
    case CC_BRAKES:        return 0x2884658f;
    case CC_FORCED_INDUCTION:
        if (gCarCustomizeManager.IsTurbo()) {
            return 0xd3f65323;
        }
        return 0x63a51aa2;
    case CC_PAINT:         return 0xd3a2d4d3;
    case CC_VINYL_TYPES:
    case CC_VINYL_GROUP_FLAME:
    case CC_VINYL_GROUP_TRIBAL:
    case CC_VINYL_GROUP_STRIPE:
    case CC_VINYL_GROUP_RACING_FLAG:
    case CC_VINYL_GROUP_NATIONAL_FLAG:
    case CC_VINYL_GROUP_BODY:
    case CC_VINYL_GROUP_UNIQUE:
    case CC_VINYL_GROUP_CONTEST:
        return 0xd413e189;
    case CC_DECAL_LOCATION:
    case CC_DECAL_WINDSHIELD:
    case CC_DECAL_REAR_WINDOW:
    case CC_DECAL_LEFT_DOOR:
    case CC_DECAL_RIGHT_DOOR:
    case CC_DECAL_LEFT_QP:
    case CC_DECAL_RIGHT_QP:
    case CC_DECAL_SLOT_1:
    case CC_DECAL_SLOT_2:
    case CC_DECAL_SLOT_3:
    case CC_DECAL_SLOT_4:
    case CC_DECAL_SLOT_5:
    case CC_DECAL_SLOT_6:
        return 0xd2cbc510;
    default:
        return 0;
    }
}

unsigned int GetNumMarkersFromCategory(eCustomizeCategory cat) {
    switch (static_cast<unsigned int>(cat)) {
    case CC_PARTS: {
        int total = GetNumMarkersFromCategory(CC_BODY_KIT);
        total += GetNumMarkersFromCategory(CC_SPOILERS);
        total += GetNumMarkersFromCategory(CC_RIM_BRANDS);
        total += GetNumMarkersFromCategory(CC_HOODS);
        total += GetNumMarkersFromCategory(CC_ROOF_SCOOPS);
        return total + GetNumMarkersFromCategory(CC_CUSTOM_HUD);
    }
    case CC_PERFORMANCE: {
        int total = GetNumMarkersFromCategory(CC_ENGINE);
        total += GetNumMarkersFromCategory(CC_TRANSMISSION);
        total += GetNumMarkersFromCategory(CC_SUSPENSION);
        total += GetNumMarkersFromCategory(CC_NITROUS);
        total += GetNumMarkersFromCategory(CC_TIRES);
        total += GetNumMarkersFromCategory(CC_BRAKES);
        return total + GetNumMarkersFromCategory(CC_FORCED_INDUCTION);
    }
    case CC_VISUAL: {
        int total = GetNumMarkersFromCategory(CC_PAINT);
        total += GetNumMarkersFromCategory(CC_VINYL_TYPES);
        return total + GetNumMarkersFromCategory(CC_DECAL_LOCATION);
    }
    default:
        return TheFEMarkerManager.GetNumMarkers(
            static_cast<FEMarkerManager::ePossibleMarker>(TranslateCustomizeCatToMarker(cat)), 0);
    }
}

void CustomizeSub::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    unsigned int to_cat = static_cast<unsigned short>(
        static_cast<CustomizeMainOption *>(Options.GetCurrentOption())->Category);

    if (Category != 0x803 || to_cat != 0x303 || msg != 0xc407210) {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
    case 0x5a928018: {
        CustomizeMainOption *opt = FindInCartOption();
        if (!opt) return;
        int slot_id = 0;
        switch (opt->Category) {
        case 0x103:
            slot_id = 0x42;
            break;
        case 0x302:
            slot_id = 0x4d;
            break;
        }
        if (slot_id == 0) return;
        if (gCarCustomizeManager.IsPartTypeInCart(slot_id)) return;
        InCartPartOptionIndex = 0;
        RefreshHeader();
        break;
    }
    case 0xc407210: {
        if (to_cat <= 0x506) {
            if (to_cat >= 0x501) {
                CustomizeDecals::CurrentDecalLocation = to_cat;
            }
        }

        SetStockPartOption *copt = static_cast<SetStockPartOption *>(Options.GetCurrentOption());
        copt->IsStockOption();
        bool stockOption = copt->IsStockOption();

        if (stockOption &&
            (copt->ThePart->GetPartState() & CPS_PLAYER_STATE_MASK) == CPS_INSTALLED &&
            InCartPartOptionIndex != 0) {
            int slot_id = 0;
            switch (static_cast<unsigned short>(copt->Category)) {
            case 0x701:
                slot_id = 0x42;
                break;
            case 0x401:
                slot_id = 0x4d;
                for (int i = 0; i <= 2; i++) {
                    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(i + 0x4f);
                    if (item) {
                        gCarCustomizeManager.RemoveFromCart(item);
                    }
                }
                break;
            }
            if (slot_id != 0) {
                ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
                if (item) {
                    gCarCustomizeManager.RemoveFromCart(item);
                    InCartPartOptionIndex = 0;
                    RefreshHeader();
                }
            }
        }

        if (bStrICmp(GetPackageName(), g_pCustomizeSubTopPkg) != 0 &&
            bStrICmp(GetPackageName(), g_pCustomizeSubPkg) != 0) {
            return;
        }

        bool ok_to_leave = false;
        switch (Category) {
        case 0x803:
            if (to_cat == 0x303) {
                CarPart *stock_rim = gCarCustomizeManager.GetStockCarPart(0x42);
                CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x42);
                if (stock_rim == installed) {
                    DialogInterface::ShowOneButton(GetPackageName(), "",
                        static_cast<eDialogTitle>(1), 0x417b2601u, 0xb4edeb6du, 0xbdb19a9fu);
                } else {
                    CustomizeMainOption *opt2 = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
                    cFEng::Get()->QueuePackageSwitch(opt2->ToPkg, opt2->Category, 0, false);
                    ok_to_leave = true;
                }
            }
            break;
        case 0x103:
            if (Options.GetCurrentIndex() == 1) {
                ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(0x42);
                if (item) {
                    unsigned int brandHash = item->GetBuyingPart()->GetPart()->GetAppliedAttributeUParam(0xebb03e66, 0);
                    InCartPartOptionIndex = GetRimBrandIndex(brandHash);
                }
                CarPart *installed2 = gCarCustomizeManager.GetInstalledCarPart(0x42);
                if (installed2) {
                    unsigned int brandHash = installed2->GetAppliedAttributeUParam(0xebb03e66, 0);
                    InstalledPartOptionIndex = GetRimBrandIndex(brandHash);
                }
                Options.SetReactToInput(true);
                RefreshHeader();
            } else {
                ok_to_leave = true;
            }
            break;
        case 0x302:
            if (Options.GetCurrentIndex() == 1) {
                ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(0x4d);
                if (item) {
                    CarPart *car_part = item->GetBuyingPart()->GetPart();
                    if (car_part) {
                        InCartPartOptionIndex = GetVinylGroupIndex(car_part->GetGroupNumber() & 0x1f);
                    } else {
                        InCartPartOptionIndex = 1;
                    }
                }
                CarPart *installed3 = gCarCustomizeManager.GetInstalledCarPart(0x4d);
                if (installed3) {
                    InstalledPartOptionIndex = GetVinylGroupIndex(installed3->GetGroupNumber() & 0x1f);
                } else {
                    InstalledPartOptionIndex = 1;
                }
                Options.SetReactToInput(true);
                RefreshHeader();
            } else {
                ok_to_leave = true;
            }
            break;
        default:
            ok_to_leave = true;
            break;
        }
        if (!ok_to_leave) return;
        cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
        break;
    }
    case 0xc519bfc3:
        if (gCarCustomizeManager.IsCareerMode()) return;
        if (Category != 0x802) return;
        DialogInterface::ShowTwoButtons(GetPackageName(), "",
            static_cast<eDialogTitle>(3), 0x70e01038u, 0x417b25e4u, 0x6820e23eu, 0xb4edeb6du,
            static_cast<eDialogFirstButtons>(0), 0x892cb612u);
        RefreshHeader();
        break;
    case 0x6820e23e:
        gCarCustomizeManager.MaxOutPerformance();
        RefreshHeader();
        break;
    case 0xb4edeb6d:
        Options.SetReactToInput(true);
        RefreshHeader();
        break;
    case 0xcf91aacd:
        CustomizeShoppingCart::ExitShoppingCart();
        return;
    }
}

CustomizeSub::~CustomizeSub() {}
