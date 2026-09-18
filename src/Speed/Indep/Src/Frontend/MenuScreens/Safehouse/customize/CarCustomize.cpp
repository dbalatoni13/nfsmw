#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeWorldMapQuickList.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

// La DEFINICION vive en zFe2 (FECustomize.cpp), no aqui: el objetivo la pone en
// .rodata:0x803E89B0, dentro del rango de zFe2 (0x803E4380..0x803EA7E8), y el
// zFeOverlay.o extraido la tiene UND.  Aqui solo se declara.
extern const float gTradeInFactor; // size: 0x4

// Los catorce globales de CarCustomize.hpp.  El objetivo los emite en
// .data:0x8043918C..0x804391C0, justo DELANTE de pParentPkg (0x804391C4) y
// detras de los estaticos de Showcase (uiShowcase.cpp): o sea aqui, al
// principio del .data de CarCustomize.cpp.  En la cabecera solo se declaran.
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

// Siete definiciones que faltaban en todo el arbol: la cabecera las declara
// (con la direccion al lado) y no las define nadie. Eran 7 de los 13 simbolos
// que impedian enlazar zFeOverlay. El orden es el de las direcciones, que a su
// vez es el de declaracion de las clases en CarCustomize.hpp (126, 411, 620, 660).
// r60 (feov): el objetivo lo inicializa con la cadena vacia (0x803C6A9C, el $LC
// de "" que cae detras del bloque de cabecera), no con NULL.
const char *CustomizeShoppingCart::pParentPkg = ""; // .data:0x804391C4

// En .bss. Los tamanos casan con `bool` de 4 B: 11*4 = 0x2C y 11*5*4 = 0xDC.
bool CustomizeHUDTexPackResources[11];        // .bss:0x804AB5B0, 0x2C
uint32 CustomizeHUDTexTextureResources[11][5]; // .bss:0x804AB5DC, 0xDC

uint32 TranslateCustomizeCatToMarker(eCustomizeCategory cat) {
    switch (cat) {
        case CC_BODY_KIT:
            return FEMarkerManager::MARKER_BODY;
        case CC_SPOILERS:
            return FEMarkerManager::MARKER_SPOILER;
        case CC_HOODS:
            return FEMarkerManager::MARKER_HOOD;
        case CC_ROOF_SCOOPS:
            return FEMarkerManager::MARKER_ROOF_SCOOP;
        case CC_CUSTOM_HUD:
            return FEMarkerManager::MARKER_CUSTOM_HUD;
        case CC_ENGINE:
            return FEMarkerManager::MARKER_ENGINE;
        case CC_TRANSMISSION:
            return FEMarkerManager::MARKER_TRANSMISSION;
        case CC_SUSPENSION:
            return FEMarkerManager::MARKER_CHASSIS;
        case CC_NITROUS:
            return FEMarkerManager::MARKER_NOS;
        case CC_TIRES:
            return FEMarkerManager::MARKER_TIRES;
        case CC_BRAKES:
            return FEMarkerManager::MARKER_BRAKES;
        case CC_FORCED_INDUCTION:
            return FEMarkerManager::MARKER_INDUCTION;
        case CC_PAINT:
        case CC_RIM_PAINT:
            return FEMarkerManager::MARKER_PAINT;
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

uint32 GetMarkerNameFromCategory(eCustomizeCategory cat) {
    switch (static_cast<unsigned int>(cat)) {
        case CC_PARTS:
            return 0xd3a2fbe1;
        case CC_PERFORMANCE:
            return 0x3c27a989;
        case CC_VISUAL:
            return 0x5692be6b;
        case CC_BODY_KIT:
            return 0x7c50498c;
        case CC_SPOILERS:
            return 0x52012995;
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
        case CC_HOODS:
            return 0x8a4699e1;
        case CC_ROOF_SCOOPS:
            return 0x830100f0;
        case CC_CUSTOM_HUD:
            return 0xc253ec92;
        case CC_ENGINE:
            return 0x2f3ec04d;
        case CC_TRANSMISSION:
            return 0xd1e77ca1;
        case CC_SUSPENSION:
            return 0xb7cbfcce;
        case CC_NITROUS:
            return 0xc129562b;
        case CC_TIRES:
            return 0xd3efbefe;
        case CC_BRAKES:
            return 0x2884658f;
        case CC_FORCED_INDUCTION:
            if (gCarCustomizeManager.IsTurbo()) {
                return 0xd3f65323;
            }
            return 0x63a51aa2;
        case CC_PAINT:
            return 0xd3a2d4d3;
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

uint32 GetNumMarkersFromCategory(eCustomizeCategory cat) {
    switch (cat) {
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
            return TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(TranslateCustomizeCatToMarker(cat)), 0);
    }
}

CustomizeMeter::CustomizeMeter()
    : Min(0.0f),             //
      Max(1.0f),             //
      Current(0.0f),         //
      Preview(0.0f),         //
      PreviousPreview(0.0f), //
      NumStages(5),          //
      pMultiplier(nullptr),  //
      pMeterGroup(nullptr)   //
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
        unsigned int hash = FEngHashString("HEAT_BASE_LED_%d", i + 1);
        pBases[i] = FEngFindImage(pkg_name, hash);
    }
}

void CustomizeMeter::SetCurrent(float current) {
    Current = bMin(bMax(current, Min), Max);
}

void CustomizeMeter::SetPreview(float preview) {
    PreviousPreview = Preview;
    Preview = bMin(bMax(preview, Min), Max);
}

void CustomizeMeter::Draw() {
    float stage_size = 1.0f;
    float multiplier = 1.0f;
    float stage_bottom = Min;
    const u32 FEObj_ZOOM = 0x209c24;

    if (Preview - stage_bottom >= stage_size) {
        do {
            stage_bottom = stage_bottom + stage_size;
            multiplier = multiplier + 1.0f;
        } while (Preview - stage_bottom >= stage_size);
    }

    multiplier = bMin(multiplier, 5.0f);
    FEngSetTextureHash(pMultiplier, FEngHashString("HEAT_X%.0f", multiplier));
    FEngSetTextureHash(pMultiplierZoom, FEngHashString("HEAT_X%.0f", multiplier));
    if (Preview != PreviousPreview) {
        FEngSetScript(pMultiplierZoom, FEObj_ZOOM, true);
    }

    float segment_size = stage_size * 0.1f;
    float segment_bottom = stage_bottom + stage_size;

    int cur_icon = NUM_STAGE_SEGMENTS;
    float current_segment = segment_bottom;

    if (current_segment >= stage_bottom) {
        do {
            current_segment = current_segment - segment_size;
            cur_icon = cur_icon - 1;

            unsigned int script;
            if (current_segment + 0.0005f >= Current) {
                const u32 FEObj_OFF = 0xccfa;
                script = FEObj_OFF;
            } else if (current_segment + 0.0005f >= Preview) {
                const u32 FEObj_COOL = 0x13ff6c;
                script = FEObj_COOL;
            } else {
                const u32 FEObj_ON = 0x63c;
                script = FEObj_ON;
            }

            FEngSetScript(pBases[cur_icon], script, true);
        } while (current_segment >= stage_bottom && cur_icon >= 0);
    }
}

void CustomizeMeter::SetVisibility(bool b) {
    if (b) {
        FEngSetVisible(pMeterGroup);
    } else {
        FEngSetInvisible(pMeterGroup);
    }
}

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

void FEShoppingCartItem::Draw() {
    if (TheItem->IsActive()) {
        FEngSetTextureHash(pCheckIcon, 0x696ae039);
    } else {
        FEngSetTextureHash(pCheckIcon, 0xe719881c);
    }
    DrawPartName();
    if (TheItem->GetTradeInPart() && gCarCustomizeManager.IsCareerMode() && !CustomizeIsInBackRoom()) {
        FEPrintf(pTradeInPrice, "%$d", TheItem->GetTradeInPrice());
    } else {
        FEPrintf(pTradeInPrice, "");
    }
    if (gCarCustomizeManager.IsCareerMode() && !CustomizeIsInBackRoom()) {
        FEPrintf(GetDataObject(), "%$d", TheItem->GetPartPrice());
    } else {
        FEPrintf(GetDataObject(), "");
    }
}

void FEShoppingCartItem::Position() {
    FEngSetTopLeft(pCheckIcon, GetTopLeftX(), GetTopLeftY() - 4.0f);
    FEngSetTopLeft(GetTitleObject(), GetTopLeftX() + 36.0f, GetTopLeftY());
    FEngSetTopLeftY(pTradeInPrice, GetTopLeftY());
    FEngSetBottomRightX(pTradeInPrice, GetDataPosX() + 16.0f);
    FEngSetTopLeftY(GetDataObject(), GetTopLeftY());
    FEngSetBottomRightX(GetDataObject(), GetDataPosX() + 104.0f);
    if (GetBacking()) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FEShoppingCartItem::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), FEHASH_HIGHLIGHT, true);
    FEngSetScript(GetDataObject(), FEHASH_HIGHLIGHT, true);
    FEngSetScript(pTradeInPrice, FEHASH_HIGHLIGHT, true);
    if (GetBacking()) {
        FEngSetVisible(GetBacking());
        FEngSetScript(GetBacking(), FEHASH_HIGHLIGHT, true);
    }
}

void FEShoppingCartItem::UnsetFocus() {
    unsigned int script = FEHASH_UNHIGHLIGHT;
    if (!TheItem->IsActive()) {
        script = 0x163c76;
    }
    FEngSetScript(GetTitleObject(), script, true);
    FEngSetScript(GetDataObject(), script, true);
    FEngSetScript(pTradeInPrice, script, true);
    if (GetBacking()) {
        FEngSetInvisible(GetBacking());
        FEngSetScript(GetBacking(), FEHASH_UNHIGHLIGHT, true);
    }
}

void FEShoppingCartItem::SetCheckScripts() {
    if (TheItem->IsActive()) {
        FEngSetScript(pCheckIcon, FEHASH_CHECKED, true);
    } else {
        FEngSetScript(pCheckIcon, FEHASH_UNCHECKED, true);
    }
}

void FEShoppingCartItem::SetActiveScripts() {
    if (!TheItem->IsActive()) {
        FEngSetScript(pCheckIcon, 0x163c76, true);
    }
}

void FEShoppingCartItem::DrawPartName() {
    if (TheItem->GetBuyingPart()->IsPerformancePkg()) {
        Physics::Upgrades::Type phys_type = static_cast<Physics::Upgrades::Type>(static_cast<int>(TheItem->GetBuyingPart()->GetPhysicsType()));
        if (TheItem->GetBuyingPart()->GetUpgradeLevel() == 7) {
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)), GetLocalizedString(0xedd14807));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)), GetLocalizedString(0xedd14807));
            }
        } else {
            int level = TheItem->GetBuyingPart()->GetUpgradeLevel() - (gCarCustomizeManager.GetNumPackages(phys_type) - 6);
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)),
                         GetLocalizedString(GetPerfPkgLevelHash(level)));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)),
                         GetLocalizedString(GetPerfPkgLevelHash(level)));
            }
        }
        return;
    }

    SelectablePart *part = TheItem->GetBuyingPart();
    switch (part->GetSlotID()) {
        case 0x4e: {
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0xb3100a3e),
                         GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0xb3100a3e),
                         GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            }
            return;
        }

        case 0x4c: {
            unsigned int paint_type = part->GetPart()->GetBrandNameHash();
            unsigned int colorHash = 0x452b5481;
            switch (paint_type) {
                case 0x2daab07:
                    colorHash = 0xb6763cde;
                    break;
                case 0x3437a52:
                    break;
                case 0x3797533:
                    colorHash = 0xb715070a;
                    break;
                case 0xda27:
                    colorHash = 0xb3100a3e;
                    break;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(colorHash),
                         GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(colorHash),
                         GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
            }
            return;
        }

        case 0x71: {
            ShoppingCartItem *leftItem = gCarCustomizeManager.IsPartTypeInCart(0x71);
            ShoppingCartItem *rightItem = gCarCustomizeManager.IsPartTypeInCart(0x72);
            if (!leftItem)
                return;
            if (!rightItem)
                return;
            CarPart *left_part = leftItem->GetBuyingPart()->GetPart();
            CarPart *right_part = rightItem->GetBuyingPart()->GetPart();
            if (!left_part) {
                goto missing_parts;
            }
            if (!right_part) {
                goto missing_parts;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s%s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), left_part->GetName(),
                         right_part->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s%s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), left_part->GetName(),
                         right_part->GetName());
            }
            return;
        missing_parts:
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0xbe434a38));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0xbe434a38));
            }
            return;
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
            if (!part->GetPart()) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s - %s", GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(0x7177dc17));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s - %s", GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(0x7177dc17));
                }
                return;
            }
            unsigned int name_hash = part->GetPart()->GetBrandNameHash();
            bool black = part->GetPart()->GetAppliedAttributeUParam(bStringHash("NAME"), 0);
            unsigned int slot_hash = 0;
            switch (part->GetSlotID()) {
                case 0x63:
                case 0x6b:
                    slot_hash = 0x7d212cfa;
                    break;
                case 0x64:
                case 0x6c:
                    slot_hash = 0x7d212cfb;
                    break;
                case 0x65:
                case 0x6d:
                    slot_hash = 0x7d212cfc;
                    break;
                case 0x66:
                case 0x6e:
                    slot_hash = 0x7d212cfd;
                    break;
                case 0x67:
                case 0x6f:
                    slot_hash = 0x7d212cfe;
                    break;
                case 0x68:
                case 0x70:
                    slot_hash = 0x7d212cff;
                    break;
            }
            if (slot_hash != 0) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s %s %s", GetLocalizedString(0x955980bc),
                             GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(slot_hash), part->GetPart()->GetName());
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s %s %s", GetLocalizedString(0x955980bc),
                             GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(slot_hash), part->GetPart()->GetName());
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                         part->GetPart()->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(0x955980bc), GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                         part->GetPart()->GetName());
            }
            return;
        }

        case 0x17: {
            if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
            }
            return;
        }

        case 0x42: {
            if (part->GetPart() != gCarCustomizeManager.GetStockCarPart(0x42)) {
                char sztemp[64];
                bSNPrintf(sztemp, 64, "%s", part->GetPart()->GetName());
                int len = bStrLen(sztemp);
                if (len < 1)
                    return;
                for (int i = len - 6; len >= i; len--) {
                    sztemp[len] = 0;
                }
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s %$d\"", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), sztemp,
                             part->GetPart()->GetInnerRadius());
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s %$d\"", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), sztemp,
                             part->GetPart()->GetInnerRadius());
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0x60a662f5));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0x60a662f5));
            }
            return;
        }

        case 0x2c:
        case 0x3e:
        case 0x3f: {
            if (part->GetPart()->HasAppliedAttribute(bStringHash("CARBONFIBRE"))) {
                if (part->GetPart()->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0) != 0) {
                    if (GetCurrentLanguage() == 1) {
                        FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(0x5415b874),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(0x5415b874),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    }
                    return;
                }
            }
            if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
            }
            return;
        }

        case 0x4d: {
            if (part->GetPart() != 0) {
                if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                    if (GetCurrentLanguage() == 1) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    }
                    return;
                }
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0x60a662f5));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0x60a662f5));
            }
            return;
        }

        default: {
            if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                if (GetCurrentLanguage() == 1) {
                    FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                }
                return;
            }
            if (GetCurrentLanguage() == 1) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
            }
            return;
        }
    }
}

uint32 FEShoppingCartItem::GetPerfPkgCatHash(Physics::Upgrades::Type phys_type) {
    uint32 hash = 0;
    switch (phys_type) {
        case Physics::Upgrades::PUT_TIRES:
            hash = 0x5aa9137;
            break;
        case Physics::Upgrades::PUT_BRAKES:
            hash = 0x91997ee8;
            break;
        case Physics::Upgrades::PUT_CHASSIS:
            hash = 0x6e101aa7;
            break;
        case Physics::Upgrades::PUT_TRANSMISSION:
            hash = 0x29aa74ba;
            break;
        case Physics::Upgrades::PUT_ENGINE:
            hash = 0x9853d9a6;
            break;
        case Physics::Upgrades::PUT_INDUCTION:
            if (gCarCustomizeManager.IsTurbo()) {
                hash = 0x5b1255c;
            } else {
                hash = 0xbb6812bb;
            }
            break;
        case Physics::Upgrades::PUT_NOS:
            hash = 0x4ce19aa4;
            break;
        default:
            break;
    }
    return hash;
}

uint32 FEShoppingCartItem::GetPerfPkgLevelHash(int level) {
    uint32 hash = 0x69c270c3;
    switch (level) {
        case 1:
            hash = 0x69c270c4;
            break;
        case 2:
            hash = 0x69c270c5;
            break;
        case 3:
            hash = 0x69c270c6;
            break;
        case 4:
            hash = 0x69c270c7;
            break;
        case 5:
            hash = 0x69c270c8;
            break;
        case 6:
            hash = 0x69c270c9;
            break;
    }
    return hash;
}

uint32 FEShoppingCartItem::GetCarPartCatHash(uint32 slot_id) {
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

CustomizeShoppingCart::CustomizeShoppingCart(ScreenConstructorData *sd) : UIWidgetMenu(sd) {
    bScrollWrapped = false;
    if (gCarCustomizeManager.IsCareerMode()) {
        iMaxWidgetsOnScreen = 4;
    } else {
        iMaxWidgetsOnScreen = 6;
    }
    Setup();
}

void CustomizeShoppingCart::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    UIWidgetMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xc519bfc3:
            ToggleChecked();
            RefreshHeader();
            break;
        case 0x406415e3:
            if (!gCarCustomizeManager.DoesCartHaveActiveParts()) {
                CarCustomizeManager &mgr = gCarCustomizeManager;
                mgr.EmptyCart();
                mgr.ResetPreview();
                gCarCustomizeManager.ResetPreview();
                cFEng::Get()->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
                cFEng::Get()->QueuePackagePop(1);
                break;
            }
            if (CanCheckout()) {
                unsigned int blurb_hash = 0x71d9e710;
                if (gCarCustomizeManager.IsCareerMode()) {
                    blurb_hash = 0x8ebaa44b;
                    if (CustomizeIsInBackRoom()) {
                        blurb_hash = 0x4810898;
                    }
                }
                DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0xd05fc3a3, 0x34dc1bcf, 0x34dc1bcf,
                                                first_dialog_button2, blurb_hash);
            } else {
                DialogInterface::ShowOk(GetPackageName(), "", dialog_alert, 0xa984a42);
            }
            break;
        case 0xd05fc3a3:
            gCarCustomizeManager.Checkout();
            cFEng::Get()->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
            cFEng::Get()->QueuePackagePop(1);
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
            cFEng::Get()->QueueGameMessage(0x5a928018, pParentPkg, 0xFF);
            cFEng::Get()->QueuePackagePop(1);
            break;
    }
}

void CustomizeShoppingCart::ShowShoppingCart(const char *pkg) {
    pParentPkg = pkg;
    cFEng::Get()->QueuePackagePush(g_pCustomizeShoppingCartPkg, 0, 0, false);
}

void CustomizeShoppingCart::ExitShoppingCart() {
    if (gCarCustomizeManager.IsInBackRoom()) {
        gCarCustomizeManager.SetInBackRoom(false);
        FEManager::Get()->SetGarageType(GARAGETYPE_CUSTOMIZATION_SHOP);
    }
    cFEng::Get()->QueuePackageSwitch(g_pCustomizeMainPkg, 0, 0, false);
}

bool CustomizeShoppingCart::IsSlotIDNumberDecal(int slot_id) {
    if (slot_id == CARSLOTID_DECAL_RIGHT_DOOR_TEX6 || slot_id == CARSLOTID_DECAL_RIGHT_DOOR_TEX7 || slot_id == CARSLOTID_DECAL_LEFT_DOOR_TEX6 ||
        slot_id == CARSLOTID_DECAL_LEFT_DOOR_TEX7) {
        return true;
    }
    return false;
}

void CustomizeShoppingCart::ToggleAllNumberDecals() {
    int count = gCarCustomizeManager.GetNumCartItems();
    for (int i = 0; i < count; i++) {
        ShoppingCartItem *item = static_cast<ShoppingCartItem *>(gCarCustomizeManager.GetCartItem(i));
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

bool CustomizeShoppingCart::CanCheckout() {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (gCarCustomizeManager.IsInBackRoom()) {
            return true;
        }
        return gCarCustomizeManager.GetCartTotal(CCT_TOTAL) <= FEDatabase->GetCareerSettings()->GetCash();
    }
    return true;
}

void CustomizeShoppingCart::SetMarkerData(int num, ShoppingCartItem *item, int num_markers) {
    const u32 FEObj_GREY = 0x163c76;
    const u32 FEObj_NORMAL = FEHASH_NORMAL;
    FEngSetScript(GetPackageName(), FEngHashString("MARKER_GROUP_%d", num), num_markers == 0 ? FEObj_GREY : FEObj_NORMAL, true);
    FEPrintf(GetPackageName(), FEngHashString("MARKER_NUM_%d", num), "%$d", num_markers);
    FEPrintf(GetPackageName(), FEngHashString("MARKER_BLOOM_%d", num), "%$d", num_markers);
}

int CustomizeShoppingCart::GetNumMarkersSpending(unsigned int marker) {
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(marker);
    int result = 0;
    if (item && item->IsActive()) {
        result = 1;
    }
    return result;
}

void CustomizeShoppingCart::SetMarkerAmounts() {
    if (gCarCustomizeManager.IsInPerformance()) {


        static Physics::Upgrades::Type phys_type[7] = {
            Physics::Upgrades::PUT_BRAKES,  Physics::Upgrades::PUT_ENGINE, Physics::Upgrades::PUT_NOS, Physics::Upgrades::PUT_INDUCTION,
            Physics::Upgrades::PUT_CHASSIS, Physics::Upgrades::PUT_TIRES,  Physics::Upgrades::PUT_TRANSMISSION,
        };
        static int markers[7] = {
            FEMarkerManager::MARKER_BRAKES,  FEMarkerManager::MARKER_ENGINE, FEMarkerManager::MARKER_NOS,          FEMarkerManager::MARKER_INDUCTION,
            FEMarkerManager::MARKER_CHASSIS, FEMarkerManager::MARKER_TIRES,  FEMarkerManager::MARKER_TRANSMISSION,
        };
        int num_thingies = 7;
        for (int i = 0; i < num_thingies; i++) {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(phys_type[i]);
            int num = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(markers[i]), 0);
            if (item && item->IsActive()) {
                num--;
            }
            SetMarkerData(i + 1, item, num);
        }
    } else if (gCarCustomizeManager.IsInParts()) {
        static unsigned int slot_id[5] = {0x17, 0x3F, 0x2C, 0x42, 0x3E};
        static int markers[5] = {
            FEMarkerManager::MARKER_BODY, FEMarkerManager::MARKER_HOOD,       FEMarkerManager::MARKER_SPOILER,
            FEMarkerManager::MARKER_RIMS, FEMarkerManager::MARKER_ROOF_SCOOP,
        };
        int num_thingies = 5;
        for (int i = 0; i < num_thingies; i++) {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id[i]);
            int num = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(markers[i]), 0);
            if (item && item->IsActive()) {
                num--;
            }
            SetMarkerData(i + 1, item, num);
        }
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e22));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e23));
    } else {
        ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x4d));
        int num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0);
        if (item && item->IsActive()) {
            num--;
        }
        SetMarkerData(1, item, num);

        num = GetNumMarkersSpending(0x53);
        num += GetNumMarkersSpending(0x5b);
        num += GetNumMarkersSpending(99);
        num += GetNumMarkersSpending(100);
        num += GetNumMarkersSpending(0x65);
        num += GetNumMarkersSpending(0x66);
        num += GetNumMarkersSpending(0x67);
        num += GetNumMarkersSpending(0x68);
        num += GetNumMarkersSpending(0x6b);
        num += GetNumMarkersSpending(0x6c);
        num += GetNumMarkersSpending(0x6d);
        num += GetNumMarkersSpending(0x6e);
        num += GetNumMarkersSpending(0x6f);
        num += GetNumMarkersSpending(0x70);
        num += GetNumMarkersSpending(0x73);
        num += GetNumMarkersSpending(0x7b);
        num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_DECAL, 0) - num;
        SetMarkerData(2, item, num);

        item = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x4c));
        int paint_num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0);
        if (item && item->IsActive()) {
            paint_num--;
        }
        SetMarkerData(3, item, paint_num);

        item = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(0x84));
        int hud_num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_CUSTOM_HUD, 0);
        if (item && item->IsActive()) {
            hud_num--;
        }
        SetMarkerData(4, item, hud_num);

        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e21));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e22));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x47df0e23));
    }
}





bool CustomizeParts::TexturePackLoaded = false;          // .data:0x80439228
uint32 CustomizeDecals::CurrentDecalLocation = 0x501;    // .data:0x8043922C (el objetivo lo arranca en 0x501, no en 0)
bool CustomizeNumbers::bShowcaseOn = false;              // .data:0x80439230

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
            int num_markers;
            if (CustomizeIsInParts()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0xa03a752f);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x4ac68298);
                num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BODY, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_HOOD, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_SPOILER, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_RIMS, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ROOF_SCOOP, 0);
            } else if (CustomizeIsInPerformance()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0x358db897);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x68342700);
                num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BRAKES, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ENGINE, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_NOS, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_INDUCTION, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_CHASSIS, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_TIRES, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_TRANSMISSION, 0);
            } else {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0x93296e59);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x78f1c602);
                num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_DECAL, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_CUSTOM_HUD, 0);
            }
            FEPrintf(GetPackageName(), 0xd1497a06, "%$d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0)));
            FEPrintf(GetPackageName(), 0x18661565, "%$d",
                     num_markers - gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0)));
        } else {
            FEPrintf(GetPackageName(), 0xd1497a06, "%$d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(0)));
            FEPrintf(GetPackageName(), 0x34f7c0e8, "%$d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(1)));
            int totalCost = gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(2));
            FEPrintf(GetPackageName(), 0x18661565, "%$d", totalCost);
            FEPrintf(GetPackageName(), 0x8531e22e, "%$d", FEDatabase->GetCareerSettings()->GetCash() - totalCost);
        }
    } else {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x9ea22e0b));
    }
}

void CustomizeShoppingCart::AddItem(ShoppingCartItem *item) {
    FEShoppingCartItem *widget = new ("FEShoppingCartItem", 0) FEShoppingCartItem(item);
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

    float data_right_edge = FEngGetTopLeftX(reinterpret_cast<FEObject *>(widget->GetDataObject())) +
                            FEngGetSizeX(reinterpret_cast<FEObject *>(widget->GetDataObject()));

    widget->SetWidth(bAbs(widget->GetTopLeftX() - data_right_edge));
}

void CustomizeShoppingCart::ClearUncheckedItems() {
    ShoppingCartItem *item = gCarCustomizeManager.GetFirstCartItem();
    while (item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
        if (!item->IsActive()) {
            if (item->GetBuyingPart()->GetSlotID() == 0x4d) {
                ShoppingCartItem *color_item = gCarCustomizeManager.GetFirstCartItem();
                while (color_item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
                    switch (color_item->GetBuyingPart()->GetSlotID()) {
                        case CARSLOTID_VINYL_COLOUR0_0:
                        case CARSLOTID_VINYL_COLOUR0_1:
                        case CARSLOTID_VINYL_COLOUR0_2:
                        case CARSLOTID_VINYL_COLOUR0_3: {
                            ShoppingCartItem *color_temp = color_item;
                            color_item = color_item->GetNext();
                            gCarCustomizeManager.RemoveFromCart(color_temp);
                            continue;
                        }
                    }
                    color_item = color_item->GetNext();
                }
            }
            ShoppingCartItem *temp = item;
            item = item->GetNext();
            gCarCustomizeManager.RemoveFromCart(temp);
            continue;
        }
        item = item->GetNext();
    }
    gCarCustomizeManager.ResetPreview();
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

void CustomizeShoppingCart::SetMarkerImages() {
    if (gCarCustomizeManager.IsInPerformance()) {
        FEngSetTextureHash(GetPackageName(), 0xeb957471, 0x4887f351);
        FEngSetTextureHash(GetPackageName(), 0xeb957472, 0x4f424e0f);
        FEngSetTextureHash(GetPackageName(), 0xeb957473, 0x6fea04c8);
        FEngSetTextureHash(GetPackageName(), 0xeb957474, 0x8e284227);
        FEngSetTextureHash(GetPackageName(), 0xeb957475, 0x190eb6);
        FEngSetTextureHash(GetPackageName(), 0xeb957476, 0x7373f1ef);
        FEngSetTextureHash(GetPackageName(), 0xeb957477, 0xd142d3e3);
    } else if (gCarCustomizeManager.IsInParts()) {
        FEngSetTextureHash(GetPackageName(), 0xeb957471, 0xaf393dba);
        FEngSetTextureHash(GetPackageName(), 0xeb957472, 0xf375276e);
        FEngSetTextureHash(GetPackageName(), 0xeb957473, 0xc51a4f62);
        FEngSetTextureHash(GetPackageName(), 0xeb957474, 0xc19491cc);
        FEngSetTextureHash(GetPackageName(), 0xeb957475, 0x25a4375e);
        FEngSetInvisible(GetPackageName(), 0x47df0e22);
        FEngSetInvisible(GetPackageName(), 0x47df0e23);
    } else {
        FEngSetTextureHash(GetPackageName(), 0xeb957471, 0xd35f04c0);
        FEngSetTextureHash(GetPackageName(), 0xeb957472, 0xa9135927);
        FEngSetTextureHash(GetPackageName(), 0xeb957473, 0xdb89e17);
        FEngSetTextureHash(GetPackageName(), 0xeb957474, 0x8ba602fc);
        FEngSetInvisible(GetPackageName(), 0x47df0e21);
        FEngSetInvisible(GetPackageName(), 0x47df0e22);
        FEngSetInvisible(GetPackageName(), 0x47df0e23);
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

CustomizeCategoryScreen::CustomizeCategoryScreen(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), //
      bBackingOut(false),   //
      BackToPkg(nullptr),   //
      HeatMeter() {
    Category = sd->Arg & CC_TO_CAT_MASK;
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

CustomizeCategoryScreen::~CustomizeCategoryScreen() {}

void CustomizeCategoryScreen::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    const u32 FEObj_APPEAR = FEHASH_APPEAR;
    uint32 unlock_status = static_cast<CustomizeMainOption *>(Options.GetCurrentOption())->UnlockStatus;
    if (unlock_status == 2) {
        FEngSetVisible(GetPackageName(), 0xcffb7033);
        FEngSetTextureHash(GetPackageName(), 0xcffb7033, 0xf0574bb2);
        FEngSetScript(GetPackageName(), 0xcffb7033, FEObj_APPEAR, true);
    } else if (unlock_status == 3) {
        FEngSetVisible(GetPackageName(), 0xcffb7033);
        FEngSetTextureHash(GetPackageName(), 0xcffb7033, 0xcffb7033);
        FEngSetScript(GetPackageName(), 0xcffb7033, FEObj_APPEAR, true);
    } else {
        FEngSetInvisible(GetPackageName(), 0xcffb7033);
    }
    if (gCarCustomizeManager.IsCareerMode()) {
        HeatMeter.SetCurrent(gCarCustomizeManager.GetActualHeat());
        HeatMeter.SetPreview(gCarCustomizeManager.GetCartHeat());
        HeatMeter.Draw();
        if (gCarCustomizeManager.IsInBackRoom()) {
            FEngSetLanguageHash(GetPackageName(), 0x63ca8308, GetMarkerNameFromCategory(static_cast<eCustomizeCategory>(Category)));
            FEPrintf(GetPackageName(), 0x83e3cd39, "%$d", GetNumMarkersFromCategory(static_cast<eCustomizeCategory>(Category)));
            FEPrintf(GetPackageName(), 0x23d918fe, "%$d", TheFEMarkerManager.GetNumCustomizeMarkers());
        } else {
            FEPrintf(GetPackageName(), 0x7a6d2f71, "%$d", gCarCustomizeManager.GetCartTotal(CCT_TOTAL));
            FEPrintf(GetPackageName(), 0xc60adcfd, "%$d", FEDatabase->GetCareerSettings()->GetCash());
        }
    } else {
        HeatMeter.SetVisibility(false);
        FEngSetInvisible(GetPackageName(), 0x8d1559a4);
    }
}

int CustomizeCategoryScreen::AddCustomOption(const char *to_pkg, unsigned int tex_hash, unsigned int name_hash, unsigned int to_cat) {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            if (gCarCustomizeManager.IsCategoryLocked(to_cat, true)) {
                return -1;
            }
        }
    }
    CustomizeMainOption *opt = new ("CustomizeMainOption", 0) CustomizeMainOption(to_pkg, tex_hash, name_hash, to_cat, Category);
    AddOption(opt);
    if (gCarCustomizeManager.IsCategoryLocked(to_cat, false)) {
        opt->UnlockStatus = CPS_LOCKED;
    } else if (gCarCustomizeManager.IsCategoryNew(to_cat)) {
        opt->UnlockStatus = CPS_NEW;
    }
    return Options.GetIndexToAdd() - 3;
}

void CustomizeCategoryScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xb5af2461:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
        case 0xe1fde1d1:
            if (!bBackingOut) {
                break;
            }
            cFEng::Get()->QueuePackageSwitch(BackToPkg, FromCategory | (Category << 16), 0, false);
            break;
          case 0x911ab364: {
            bool leave = true;
            switch (Category) {
                case 0x801:
                case 0x802:
                case 0x803: {
                    CarCustomizeManager &mgr = gCarCustomizeManager;
                    if (mgr.DoesCartHaveActiveParts()) {
                        cFEng::Get()->QueueGameMessage(0x1720b124, GetPackageName(), 0xFF);
                        leave = false;
                        Options.SetReactToInput(true);
                    } else {
                        mgr.EmptyCart();
                        mgr.ResetPreview();
                    }
                    break;
                }
            }
            if (leave) {
                const u32 FEObj_leavescreen = 0x587c018b;
                bBackingOut = true;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;
        }
        case 0xc519bfbf: {
            CustomizeMainOption *opt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
            Showcase::FromArgs = Category | (opt->Category << 16);
            Showcase::FromPackage = GetPackageName();
            cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(gCarCustomizeManager.GetTuningCar()), 0, false);
            break;
        }
        case 0xb4edeb6d:
            Options.SetReactToInput(true);
            break;
        case 0x7a318ee0: {
            const u32 FEObj_leavescreen = 0x587c018b;
            CarCustomizeManager &mgr = gCarCustomizeManager;
            mgr.EmptyCart();
            mgr.ResetPreview();
            cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            break;
        }
        case 0x1720b124:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
    }
}

void SetStockPartOption::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (!ThePart->IsInstalled()) {
        gCarCustomizeManager.AddToCart(ThePart);
        ThePart->SetInCart();
    }
}

CustomizeSub::CustomizeSub(ScreenConstructorData *sd)
    : CustomizeCategoryScreen(sd), //
      InstalledPartOptionIndex(0), //
      InCartPartOptionIndex(0),    //
      TitleHash(0) {
    Setup();
    gCarCustomizeManager.ResetPreview();
}

void CustomizeSub::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    unsigned int to_cat = static_cast<unsigned short>(static_cast<CustomizeMainOption *>(Options.GetCurrentOption())->Category);

    if (Category != CC_VISUAL || to_cat != CC_RIM_PAINT || msg != 0xc407210) {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case 0x5a928018: {
            CustomizeMainOption *opt = FindInCartOption();
            if (!opt)
                return;
            int slot_id = 0;
            switch (opt->Category) {
                case 0x103:
                    slot_id = 0x42;
                    break;
                case 0x302:
                    slot_id = 0x4d;
                    break;
            }
            if (slot_id == 0)
                return;
            if (gCarCustomizeManager.IsPartTypeInCart(slot_id))
                return;
            InCartPartOptionIndex = 0;
            RefreshHeader();
            break;
        }
        case 0xc407210: {
            switch (to_cat) {
                case 0x501:
                case 0x502:
                case 0x503:
                case 0x504:
                case 0x505:
                case 0x506:
                    CustomizeDecals::CurrentDecalLocation = to_cat;
                    break;
            }

            SetStockPartOption *copt = static_cast<SetStockPartOption *>(Options.GetCurrentOption());
            copt->IsStockOption();
            bool stockOption = copt->IsStockOption();

            if (stockOption && (copt->ThePart->GetPartState() & CPS_PLAYER_STATE_MASK) == CPS_INSTALLED && InCartPartOptionIndex != 0) {
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

            if (bStrICmp(GetPackageName(), g_pCustomizeSubTopPkg) != 0 && bStrICmp(GetPackageName(), g_pCustomizeSubPkg) != 0) {
                return;
            }

            bool ok_to_leave = false;
            switch (Category) {
                case 0x803:
                    if (to_cat == CC_RIM_PAINT) {
                        CarPart *stock_rim = gCarCustomizeManager.GetStockCarPart(0x42);
                        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x42);
                        if (stock_rim == installed) {
                            DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601u, 0xb4edeb6du, 0xbdb19a9fu);
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
                            unsigned int brandHash = item->GetBuyingPart()->GetPart()->GetBrandNameHash();
                            InCartPartOptionIndex = GetRimBrandIndex(brandHash);
                        }
                        CarPart *installed2 = gCarCustomizeManager.GetInstalledCarPart(0x42);
                        if (installed2) {
                            unsigned int brandHash = installed2->GetBrandNameHash();
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
            if (!ok_to_leave)
                return;
            cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            break;
        }
        case 0xc519bfc3:
            if (gCarCustomizeManager.IsCareerMode())
                return;
            if (Category != CC_PERFORMANCE)
                return;
            DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, 0x70e01038u, 0x417b25e4u, 0x6820e23eu, 0xb4edeb6du,
                                            first_dialog_button1, 0x892cb612u);
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

void CustomizeSub::RefreshHeader() {
    CustomizeCategoryScreen::RefreshHeader();
    char sztemp[64];
    bSNPrintf(sztemp, sizeof(sztemp), "%s", GetLocalizedString(TitleHash));
    eLanguages cLang = GetCurrentLanguage();
    if (cLang != eLANGUAGE_GERMAN && cLang != eLANGUAGE_FINNISH) {
        for (int n = 0; sztemp[n] != 0; n++) {
            sztemp[n] = bToLower(sztemp[n]);
        }
    }
    const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;
    FEPrintf(GetPackageName(), FEObj_TITLEGROUP, "%s", sztemp);
    if (Category == CC_RIM_BRANDS || Category == CC_VINYL_TYPES) {
        int index = Options.GetCurrentIndex();
        if (index == InCartPartOptionIndex) {
            FEngSetVisible(GetPackageName(), 0xd0582feb);
            FEngSetTextureHash(GetPackageName(), 0xd0582feb, 0x1a777e25);
        } else if (index == InstalledPartOptionIndex) {
            FEngSetVisible(GetPackageName(), 0xd0582feb);
            FEngSetTextureHash(GetPackageName(), 0xd0582feb, 0x696ae039);
        } else {
            FEngSetInvisible(GetPackageName(), 0xd0582feb);
        }
    } else {
        FEngSetInvisible(GetPackageName(), 0xd0582feb);
    }

    uint32 maxHash = 0x5aec8d91;
    if (!gCarCustomizeManager.IsCareerMode() && Category == CC_PERFORMANCE) {
        FEngSetVisible(GetPackageName(), maxHash);
    } else {
        FEngSetInvisible(GetPackageName(), maxHash);
    }
}

CustomizeMainOption *CustomizeSub::FindInCartOption() {
    if (InCartPartOptionIndex) {
        return static_cast<CustomizeMainOption *>(Options.GetOption(InCartPartOptionIndex));
    }
    return nullptr;
}

void CustomizeSub::Setup() {
    switch (Category) {
        case 0x801:
            SetupParts();
            break;
        case 0x802:
            SetupPerformance();
            break;
        case 0x803:
            SetupVisual();
            break;
        case 0x302:
            SetupVinylGroups();
            break;
        case 0x305:
            SetupDecalLocations();
            break;
        case 0x103:
            SetupRimBrands();
            break;
        case 0x501:
        case 0x502:
        case 0x503:
        case 0x504:
        case 0x505:
        case 0x506:
            SetupDecalPositions();
            break;
    }
    RefreshHeader();
}

void CustomizeSub::SetupParts() {
    if (gCarCustomizeManager.IsInBackRoom()) {
        TitleHash = 0x5d285ae7;
    } else {
        TitleHash = 0x055dce1a;
    }
    gCarCustomizeManager.SetInParts(true);
    BackToPkg = g_pCustomizeMainPkg;
    if (!gCarCustomizeManager.IsInBackRoom()) {
        AddCustomOption(g_pCustomizePartsPkg, 0x028c24f6, 0x6134c218, CC_BODY_KIT);
        AddCustomOption(g_pCustomizeSpoilerPkg, 0xbb034ea6, 0x94e73021, CC_SPOILERS);
        AddCustomOption(g_pCustomizeSubTopPkg, 0x0294d2a3, 0xf868eb0b, CC_RIM_BRANDS);
        AddCustomOption(g_pCustomizePartsPkg, 0x028f7092, 0x04d4a88d, CC_HOODS);
        AddCustomOption(g_pCustomizePartsPkg, 0x79165861, 0x61e8f83c, CC_ROOF_SCOOPS);
    } else {
        AddCustomOption(g_pCustomizePartsPkg, 0xaf393dba, 0x6134c218, CC_BODY_KIT);
        AddCustomOption(g_pCustomizeSpoilerPkg, 0xc51a4f62, 0x94e73021, CC_SPOILERS);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xc19491cc, 0xf868eb0b, CC_RIM_BRANDS);
        AddCustomOption(g_pCustomizePartsPkg, 0xf375276e, 0x04d4a88d, CC_HOODS);
        AddCustomOption(g_pCustomizePartsPkg, 0x25a4375e, 0x61e8f83c, CC_ROOF_SCOOPS);
    }
    SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
}

void CustomizeSub::SetupPerformance() {
    if (CustomizeIsInBackRoom()) {
        TitleHash = 0xbfd5b50f;
    } else {
        TitleHash = 0xbaef8282;
    }
    BackToPkg = g_pCustomizeMainPkg;
    gCarCustomizeManager.SetInPerformance(true);
    if (!CustomizeIsInBackRoom()) {
        AddCustomOption(g_pCustomizePerfPkg, 0xc15c94e6, 0x9853d9a6, CC_ENGINE);
        AddCustomOption(g_pCustomizePerfPkg, 0x01a29ffa, 0x29aa74ba, CC_TRANSMISSION);
        AddCustomOption(g_pCustomizePerfPkg, 0x178475e7, 0x6e101aa7, CC_SUSPENSION);
        AddCustomOption(g_pCustomizePerfPkg, 0x9701bde4, 0x4ce19aa4, CC_NITROUS);
        AddCustomOption(g_pCustomizePerfPkg, 0x06e8e477, 0x05aa9137, CC_TIRES);
        AddCustomOption(g_pCustomizePerfPkg, 0xbaa23a28, 0x91997ee8, CC_BRAKES);
        if (gCarCustomizeManager.IsTurbo()) {
            AddCustomOption(g_pCustomizePerfPkg, 0x06ef789c, 0x05b1255c, CC_FORCED_INDUCTION);
        } else {
            AddCustomOption(g_pCustomizePerfPkg, 0x93603dfb, 0xbb6812bb, CC_FORCED_INDUCTION);
        }
    } else {
        AddCustomOption(g_pCustomizePerfPkg, 0x4f424e0f, 0x9853d9a6, CC_ENGINE);
        AddCustomOption(g_pCustomizePerfPkg, 0xd142d3e3, 0x29aa74ba, CC_TRANSMISSION);
        AddCustomOption(g_pCustomizePerfPkg, 0x00190eb6, 0x6e101aa7, CC_SUSPENSION);
        AddCustomOption(g_pCustomizePerfPkg, 0x6fea04c8, 0x4ce19aa4, CC_NITROUS);
        AddCustomOption(g_pCustomizePerfPkg, 0x7373f1ef, 0x05aa9137, CC_TIRES);
        AddCustomOption(g_pCustomizePerfPkg, 0x4887f351, 0x91997ee8, CC_BRAKES);
        if (gCarCustomizeManager.IsTurbo()) {
            AddCustomOption(g_pCustomizePerfPkg, 0x12fe30a5, 0x05b1255c, CC_FORCED_INDUCTION);
        } else {
            AddCustomOption(g_pCustomizePerfPkg, 0x630071e4, 0xbb6812bb, CC_FORCED_INDUCTION);
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
        AddCustomOption(g_pCustomizePaintPkg, 0xa3b76154, 0x055da70c, CC_PAINT);
        AddCustomOption(g_pCustomizeSubTopPkg, 0x55778e5a, 0xbfa52c55, CC_VINYL_TYPES);
        if (!gCarCustomizeManager.IsHeroCar()) {
            AddCustomOption(g_pCustomizePaintPkg, 0xd223f84a, 0xe126ff53, CC_RIM_PAINT);
        }
        AddCustomOption(g_pCustomizePartsPkg, 0x3f23165c, 0xd32729a6, CC_WINDOW_TINT);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xda1dae54, 0x955980bc, CC_DECAL_LOCATION);
        AddCustomOption("Numbers.fng", 0x45a1c644, 0x6857e5ac, CC_NUMBERS);
        AddCustomOption(g_pCustomizeHudPkg, 0x028f88bc, 0x78980a6b, CC_CUSTOM_HUD);
    } else {
        AddCustomOption(g_pCustomizePaintPkg, 0x0db89e17, 0x055da70c, CC_PAINT);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xd35f04c0, 0xbfa52c55, CC_VINYL_TYPES);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xa9135927, 0x955980bc, CC_DECAL_LOCATION);
        AddCustomOption(g_pCustomizeHudPkg, 0x8ba602fc, 0x78980a6b, CC_CUSTOM_HUD);
    }
    SetInitialOption(FromCategory & 0xFFFF00FF);
}

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
    CarPart *stockCarPart = gCarCustomizeManager.GetStockCarPart(CARSLOTID_FRONT_WHEEL);
    SelectablePart *stockPart =
        new ("SelectablePart 14", 0) SelectablePart(stockCarPart, CARSLOTID_FRONT_WHEEL, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockPart->SetPartState(CPS_INSTALLED);
    }
    SetStockPartOption *stockOpt = new ("SetStockPartOption", 0) SetStockPartOption(stockPart, 0xf3990b6, 0x701);
    AddOption(stockOpt);

    AddCustomOption(g_pCustomizeRimsPkg, 0xb0da3de4, 0x56b51a0e, CC_RIM_BRAND_5_ZIGEN);
    AddCustomOption(g_pCustomizeRimsPkg, 0xf224a729, 0xf93f2d34, CC_RIM_BRAND_ADR);
    AddCustomOption(g_pCustomizeRimsPkg, 0xf224ab29, 0xf93f3134, CC_RIM_BRAND_BBS);
    AddCustomOption(g_pCustomizeRimsPkg, 0xe38de9e, 0x460d1369, CC_RIM_BRAND_ENKEI);
    AddCustomOption(g_pCustomizeRimsPkg, 0xea60b4a, 0x467a4015, CC_RIM_BRAND_KONIG);
    AddCustomOption(g_pCustomizeRimsPkg, 0xafc6b9cb, 0x9bb17a11, CC_RIM_BRAND_LOWENHART);
    AddCustomOption(g_pCustomizeRimsPkg, 0x27ebd095, 0xcca3063f, CC_RIM_BRAND_RACING_HART);
    AddCustomOption(g_pCustomizeRimsPkg, 0x6c2fa9db, 0xc1bc1a86, CC_RIM_BRAND_OZ);
    AddCustomOption(g_pCustomizeRimsPkg, 0x36c53e8e, 0x213085f9, CC_RIM_BRAND_VOLK);
    AddCustomOption(g_pCustomizeRimsPkg, 0x36c2d130, 0x212e5429, CC_RIM_BRAND_ROJA);

    ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_FRONT_WHEEL);
    if (inCart) {
        InCartPartOptionIndex = GetRimBrandIndex(inCart->GetBuyingPart()->GetPart()->GetBrandNameHash());
    }
    CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_FRONT_WHEEL);
    if (installed) {
        InstalledPartOptionIndex = GetRimBrandIndex(installed->GetBrandNameHash());
    }

    if (FromCategory == CC_PARTS) {
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
        FromCategory = CC_PARTS;
    }
}

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

void CustomizeSub::SetupVinylGroups() {
    TitleHash = 0xda129752;
    BackToPkg = g_pCustomizeSubPkg;
    SelectablePart *stockPart =
        new ("SelectablePart 14", 0) SelectablePart(static_cast<CarPart *>(nullptr), 0x4d, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockPart->SetPartState(CPS_INSTALLED);
    }
    SetStockPartOption *stockOpt = new ("SetStockPartOption", 0) SetStockPartOption(stockPart, 0x21f3d114, 0x401);
    AddOption(stockOpt);

    AddCustomOption(g_pCustomizePartsPkg, 0xf8148554, 0xd9228fc6, CC_VINYL_GROUP_FLAME);
    AddCustomOption(g_pCustomizePartsPkg, 0x192d84da, 0x1e8d885f, CC_VINYL_GROUP_TRIBAL);
    AddCustomOption(g_pCustomizePartsPkg, 0xf7352706, 0x1c619fd8, CC_VINYL_GROUP_STRIPE);
    AddCustomOption(g_pCustomizePartsPkg, 0x1223cc89, 0x9c1b8935, CC_VINYL_GROUP_RACING_FLAG);
    AddCustomOption(g_pCustomizePartsPkg, 0xbc44bbcb, 0x7956f7b0, CC_VINYL_GROUP_NATIONAL_FLAG);
    AddCustomOption(g_pCustomizePartsPkg, 0x694ca0ca, 0x2d5bff0f, CC_VINYL_GROUP_BODY);
    AddCustomOption(g_pCustomizePartsPkg, 0x1b3a8dd3, 0x209a9158, CC_VINYL_GROUP_UNIQUE);
    AddCustomOption(g_pCustomizePartsPkg, 0x1ba508fc, 0xcd057d21, CC_VINYL_GROUP_CONTEST);

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

    if (FromCategory == CC_VISUAL) {
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
        FromCategory = CC_VISUAL;
    }
}

void CustomizeSub::SetupDecalLocations() {
    TitleHash = 0x9de6e6e1;
    BackToPkg = g_pCustomizeSubPkg;
    AddCustomOption(g_pCustomizeDecalsPkg, 0x52ded91d, 0x301dedd3, CC_DECAL_WINDSHIELD);
    AddCustomOption(g_pCustomizeDecalsPkg, 0xac7937b4, 0x48e6ca49, CC_DECAL_REAR_WINDOW);
    AddCustomOption(g_pCustomizeSubTopPkg, 0xda88b711, 0x34367c86, CC_DECAL_LEFT_DOOR);
    AddCustomOption(g_pCustomizeSubTopPkg, 0xc9a967c4, 0xddf80259, CC_DECAL_RIGHT_DOOR);
    AddCustomOption(g_pCustomizeDecalsPkg, 0x2c710c4d, 0x8a7697d6, CC_DECAL_LEFT_QP);
    AddCustomOption(g_pCustomizeDecalsPkg, 0xffa7d360, 0xb1f9b0c9, CC_DECAL_RIGHT_QP);
    if (FromCategory == CC_VISUAL) {
        SetInitialOption(1);
    } else {
        SetInitialOption(FromCategory & 0xFFFF00FF);
    }
    if (FromCategory - 0x501u < 6u) {
        FromCategory = CC_VISUAL;
    }
}

void CustomizeSub::SetupDecalPositions() {
    TitleHash = 0x74d1887d;
    BackToPkg = g_pCustomizeSubTopPkg;
    switch (Category) {
        case 0x503:
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f48, 0x7d212cfa, CC_DECAL_SLOT_1);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f49, 0x7d212cfb, CC_DECAL_SLOT_2);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4a, 0x7d212cfc, CC_DECAL_SLOT_3);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4b, 0x7d212cfd, CC_DECAL_SLOT_4);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4c, 0x7d212cfe, CC_DECAL_SLOT_5);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4d, 0x7d212cff, CC_DECAL_SLOT_6);
            break;
        case 0x504:
            AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadb, 0x7d212cfa, CC_DECAL_SLOT_1);
            AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadc, 0x7d212cfb, CC_DECAL_SLOT_2);
            AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadd, 0x7d212cfc, CC_DECAL_SLOT_3);
            AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eade, 0x7d212cfd, CC_DECAL_SLOT_4);
            AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eadf, 0x7d212cfe, CC_DECAL_SLOT_5);
            AddCustomOption(g_pCustomizeDecalsPkg, 0x2e40eae0, 0x7d212cff, CC_DECAL_SLOT_6);
            break;
    }
    if (FromCategory == CC_DECAL_LOCATION) {
        SetInitialOption(1);
    } else {
        SetInitialOption(FromCategory & 0xFFFF00FF);
        FromCategory = CC_DECAL_LOCATION;
    }
}

CustomizeMain::CustomizeMain(ScreenConstructorData *sd) : CustomizeCategoryScreen(sd) {
    int entryPoint = g_TheCustomizeEntryPoint;
    iPerfIndex = 0;
    invalidMarkers = 0;
    if (entryPoint == 0) {
        CarViewer::haveLoadedOnce = 0;
    }
    gCarCustomizeManager.TakeControl(static_cast<eCustomizeEntryPoint>(entryPoint), g_pCustomizeCarRecordToUse);
    for (int i = 0; i < NUM_UNLOCKABLES; i++) {
        MarkUnlockableThingSeen(static_cast<eUnlockableEntity>(i), gCarCustomizeManager.GetUnlockFilter());
    }
    Setup();
    if (gCarCustomizeManager.IsCareerMode()) {
        FEDatabase->BackupCarStable();
    }
}

void CustomizeMain::SwitchRooms() {
    bool in_back_room = !gCarCustomizeManager.IsInBackRoom();
    gCarCustomizeManager.SetInBackRoom(in_back_room);
    SetTitle(in_back_room);
    int index = Options.GetCurrentIndex();
    if (in_back_room) {
        const u32 FEObj_BACKROOM = 0xa1caff8d;
        cFEng::Get()->QueuePackageMessage(FEObj_BACKROOM, GetPackageName(), nullptr);
        FEManager::Get()->SetGarageType(GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM);
    } else {
        const u32 FEObj_FRONTROOM = 0x5c01c5;
        cFEng::Get()->QueuePackageMessage(FEObj_FRONTROOM, GetPackageName(), nullptr);
        FEManager::Get()->SetGarageType(GARAGETYPE_CUSTOMIZATION_SHOP);
    }
    SetScreenNames();
    Options.RemoveAll();
    Options.AddInitialBookEnds();
    BuildOptionsList();
    SetInitialOption(index);
    RefreshHeader();
}

// SEPTIMA FALSIFICACION RETIRADA (r26). Aqui habia un
//     asm("" : : "r"(engine), "r"(pkg) : "r4");
// con dos locales `pkg`/`engine` que el DWARF del original NO tiene, forzando
// el orden de evaluacion de los argumentos. Sostenia 0,99 pp (98,88614 ->
// 97,89604) pero CERO BYTES: la funcion no llega al 100% en ninguna de las dos
// formas, y matched_code es todo-o-nada. Los dos locales por su cuenta cuestan
// cero; todo el porcentaje lo ponia el asm.
//
// Ademas: el original llama al MIEMBRO gCarCustomizeManager.IsInBackRoom() --que
// es un envoltorio inline de una linea sobre la funcion libre-- y por eso su
// DWARF tiene TRES expansiones de IsInBackRoom que a nosotros nos faltaban.
// Corregido en los tres sitios, coste medido CERO.
//
// Queda UNA sola diferencia de DWARF: la referencia `mgr`, que el original no
// tiene. Quitarla cuesta 1,29 pp (98,886 -> 97,599) y se queda: es la quinta
// vez que disolver la local de mas empeora.
void CustomizeMain::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (!gCarCustomizeManager.IsCareerMode() || msg != 0x911ab364) {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case 0x1265ece9: {
            GarageMainScreen::GetInstance()->UpdateCurrentCameraView(false);
            if (gCarCustomizeManager.IsInBackRoom()) {
                const u32 FEObj_BACKROOM = 0xa1caff8d;
                cFEng::Get()->QueuePackageMessage(FEObj_BACKROOM, GetPackageName(), nullptr);
            } else {
                const u32 FEObj_FRONTROOM = 0x5c01c5;
                cFEng::Get()->QueuePackageMessage(FEObj_FRONTROOM, GetPackageName(), nullptr);
            }
            break;
        }
        case 0x911ab364: {
            const u32 FEObj_QUIT_SCREEN = 0x6d5d86a1;
































            if (gCarCustomizeManager.IsCareerMode()) {
                if (!gCarCustomizeManager.IsInBackRoom()) {
                    cFEng::Get()->QueuePackageMessage(FEObj_QUIT_SCREEN, GetPackageName(), nullptr);
                    GarageMainScreen::GetInstance()->SetCustomizationCategory(-1);
                    FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));
                    if (!FEDatabase->IsCarStableDirty()) {
                        MemoryCard::GetInstance()->CancelNextAutoSave();
                    }
                    CarViewer::haveLoadedOnce = 0;
                    RaceStarter::StartCareerFreeRoam();
                } else {
                    SwitchRooms();
                    return;
                }
            } else {
                cFEng::Get()->QueuePackageMessage(FEObj_QUIT_SCREEN, GetPackageName(), nullptr);
            }
            gCarCustomizeManager.RelinquishControl();
            break;
        }
        case 0x34dc1bec:
            if (gCarCustomizeManager.GetNumCustomizeMarkers() > invalidMarkers) {
                SwitchRooms();
            }
            invalidMarkers = 0;
            break;
        case 0xc519bfc4:
            if (!gCarCustomizeManager.IsCareerMode() || gCarCustomizeManager.GetNumCustomizeMarkers() != 0) {
                if (gCarCustomizeManager.IsCareerMode() && !gCarCustomizeManager.IsInBackRoom() && !gCarCustomizeManager.IsHeroCar()) {
                    invalidMarkers = 0;
                    if (TheFEMarkerManager.IsMarkerAvailable(FEMarkerManager::MARKER_INDUCTION, 0) &&
                        !gCarCustomizeManager.CanInstallJunkman(Physics::Upgrades::PUT_INDUCTION)) {
                        invalidMarkers++;
                    }
                    if (TheFEMarkerManager.IsMarkerAvailable(FEMarkerManager::MARKER_NOS, 0) &&
                        !gCarCustomizeManager.CanInstallJunkman(Physics::Upgrades::PUT_NOS)) {
                        invalidMarkers++;
                    }
                    if (invalidMarkers > 0) {
                        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_info, LANGUAGE_COMMON_OK, 0x34dc1bec, 0x3b3e83);
                    } else {
                        SwitchRooms();
                    }
                }
            }
            break;
    }
}

void CustomizeMain::SetScreenNames() {






    if (gCarCustomizeManager.IsInBackRoom()) {
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

void CustomizeMain::RefreshHeader() {
    CustomizeCategoryScreen::RefreshHeader();
    if (gCarCustomizeManager.IsCareerMode() && !gCarCustomizeManager.IsHeroCar()) {
        if (!gCarCustomizeManager.IsInBackRoom() && gCarCustomizeManager.GetNumCustomizeMarkers() > 0) {
            FEngSetVisible(GetPackageName(), 0xdc6ee739);
        } else {
            FEngSetInvisible(GetPackageName(), 0xdc6ee739);
        }
    } else {
        FEngSetInvisible(GetPackageName(), 0xdc6ee739);
    }

    CustomizeMainOption *op = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
    if (op != nullptr) {
        uint32 to_cat = op->Category;
        gCarCustomizeManager.IsCategoryNew(to_cat & CC_TO_CAT_MASK);
    }
}

void CustomizeMain::SetTitle(bool isInBackroom) {
    char sztemp[64];
    if (isInBackroom) {
        bSNPrintf(sztemp, sizeof(sztemp), "%s", GetLocalizedString(0x92fcdbf0));
    } else {
        bSNPrintf(sztemp, sizeof(sztemp), "%s", GetLocalizedString(0x1f242e03));
    }
    eLanguages cLang = GetCurrentLanguage();
    if (cLang != eLANGUAGE_GERMAN && cLang != eLANGUAGE_FINNISH) {
        for (int n = 0; sztemp[n] != 0; n++) {
            sztemp[n] = bToLower(sztemp[n]);
        }
    }
    const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;
    FEPrintf(GetPackageName(), FEObj_TITLEGROUP, "%s", sztemp);
}

void CustomizeMain::Setup() {
    BackToPkg = "FeGarageMain.fng";
    SetTitle(gCarCustomizeManager.IsInBackRoom());
    SetScreenNames();
    gCarCustomizeManager.SetInPerformance(false);
    gCarCustomizeManager.SetInParts(false);
    Category = 0;
    BuildOptionsList();
    SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
    RefreshHeader();
}

void CustomizeMain::BuildOptionsList() {
    int isHero = gCarCustomizeManager.IsHeroCar();
    if (!CustomizeIsInBackRoom()) {
        if (!isHero) {
            AddCustomOption(g_pCustomizeSubPkg, 0x6e0ca66c, 0x55dce1a, CC_PARTS);
            iPerfIndex = AddCustomOption(g_pCustomizeSubPkg, 0x3987d054, 0xbaef8282, CC_PERFORMANCE);
        }
        AddCustomOption(g_pCustomizeSubPkg, 0x3e31ba56, 0xbfa7d7c4, CC_VISUAL);
    } else {
        if (!isHero) {
            AddCustomOption(g_pCustomizeSubPkg, 0x73272ed2, 0x55dce1a, CC_PARTS);
            AddCustomOption(g_pCustomizeSubPkg, 0xc61c8d3a, 0xbaef8282, CC_PERFORMANCE);
        }
        AddCustomOption(g_pCustomizeSubPkg, 0xe69d4f7c, 0xbfa7d7c4, CC_VISUAL);
    }
}

CustomizationScreenHelper::CustomizationScreenHelper(const char *pkg_name)
    : pPackageName(pkg_name), bInitComplete(false), bUnlockOverlayShowing(false), TitleHash(0) {
    HeatMeter.Init(GetPackageName(), "HEAT_METER", 1.0f, 5.0f, gCarCustomizeManager.GetActualHeat(), gCarCustomizeManager.GetCartHeat());
}

void CustomizationScreenHelper::DrawTitle() {
    char sztemp[64];
    bSNPrintf(sztemp, sizeof(sztemp), "%s", GetLocalizedString(TitleHash));

    eLanguages cLang = GetCurrentLanguage();
    GetTitleHash(); // unknown purpose
    if (cLang != eLANGUAGE_GERMAN && cLang != eLANGUAGE_FINNISH) {
        for (int n = 0; sztemp[n] != 0; n++) {
            sztemp[n] = bToLower(sztemp[n]);
        }
    }

    const u32 FEObj_TITLEGROUP = __TITLE_GROUP__;
    FEPrintf(GetPackageName(), FEObj_TITLEGROUP, "%s", sztemp);
}

void CustomizationScreenHelper::SetCareerStatusIcon(eCustomizePartState state) {
    const u32 FEObj_APPEAR = FEHASH_APPEAR;

    switch (state) {
        case CPS_AVAILABLE: {
            const u32 FEObj_HIDE = FEHASH_HIDE;
            FEngSetScript(GetPackageName(), 0xcffb7033, FEObj_HIDE, true);
            break;
        }
        case CPS_LOCKED: {
            FEngSetVisible(GetPackageName(), 0xcffb7033);
            FEngSetTextureHash(GetPackageName(), 0xcffb7033, 0xf0574bb2);
            const u32 FEObj_APPEAR = FEHASH_APPEAR;
            if (FEngGetScript(GetPackageName(), 0xcffb7033, FEObj_APPEAR) != nullptr) {
                FEngSetScript(GetPackageName(), 0xcffb7033, FEObj_APPEAR, true);
            }
            break;
        }
        case CPS_NEW: {
            FEngSetVisible(GetPackageName(), 0xcffb7033);
            FEngSetTextureHash(GetPackageName(), 0xcffb7033, 0xcffb7033);
            const u32 FEObj_BLINK = FEHASH_BLINK;
            if (FEngGetScript(GetPackageName(), 0xcffb7033, FEObj_BLINK) != nullptr) {
                FEngSetScript(GetPackageName(), 0xcffb7033, FEObj_BLINK, true);
            }
            break;
        }
    }
}

void CustomizationScreenHelper::SetPlayerCarStatusIcon(eCustomizePartState state) {
    switch (state) {
        case CPS_AVAILABLE:
            FEngSetInvisible(pPackageName, 0xd0582feb);
            break;
        case CPS_INSTALLED:
            FEngSetVisible(pPackageName, 0xd0582feb);
            FEngSetTextureHash(pPackageName, 0xd0582feb, 0x696ae039);
            break;
        case CPS_IN_CART:
            FEngSetVisible(pPackageName, 0xd0582feb);
            FEngSetTextureHash(pPackageName, 0xd0582feb, 0x1a777e25);
            break;
        default:
            break;
    }
}

void CustomizationScreenHelper::SetCashVisibility(bool visible) {
    if (visible) {
        FEngSetVisible(GetPackageName(), 0x8d1559a4);
    } else {
        FEngSetInvisible(GetPackageName(), 0x8d1559a4);
    }
}

void CustomizationScreenHelper::SetUnlockOverlayState(bool show, unsigned int blurb_hash) {
    unsigned int script = FEHASH_APPEAR;
    bUnlockOverlayShowing = show;
    if (show) {
        FEngSetLanguageHash(pPackageName, 0xa6298e25, blurb_hash);
    } else {
        script = 0x33113ac;
    }
    FEngSetScript(pPackageName, 0xebc3e6b7, script, true);
}

void CustomizationScreenHelper::SetCareerStuff(SelectablePart *part, unsigned int cat, unsigned int tradeInValue) {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (CustomizeIsInBackRoom()) {
            FEngSetLanguageHash(GetPackageName(), 0x63ca8308, GetMarkerNameFromCategory(static_cast<eCustomizeCategory>(cat)));
            FEPrintf(GetPackageName(), 0x23d918fe, "1");
            FEPrintf(GetPackageName(), 0x83e3cd39, "%$d", GetNumMarkersFromCategory(static_cast<eCustomizeCategory>(cat)));
        } else {
            if (part) {
                FEPrintf(GetPackageName(), 0xdbb80edd, "%$d", part->GetPrice());
            } else {
                SelectablePart *tempPart = gCarCustomizeManager.GetTempColoredPart();
                if (tempPart) {
                    FEPrintf(GetPackageName(), 0xdbb80edd, "%$d", tempPart->GetPrice());
                } else {
                    FEPrintf(GetPackageName(), 0xdbb80edd, "-1");
                }
            }
            FEPrintf(GetPackageName(), 0xc60adcfd, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEPrintf(GetPackageName(), 0x7a6d2f71, "%$d", gCarCustomizeManager.GetCartTotal(static_cast<eCustomizeCartTotals>(2)));
            FEPrintf(GetPackageName(), 0xa91eda8a, "%$d", tradeInValue);
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
    FEPrintf(GetPackageName(), __NUMBER__, "%$d", part_num);
    FEPrintf(GetPackageName(), __NUMBER_OF__, "%$d", max_parts);
}

void CustomizationScreenHelper::FlashStatusIcon(eCustomizePartState state, bool play_sound) {
    unsigned int hash = 0;
    switch (state) {
        case CPS_LOCKED:
            hash = 0xcffb7033;
            break;
        case CPS_INSTALLED:
        case CPS_IN_CART:
            hash = 0xd0582feb;
            break;
        default:
            break;
    }
    FEngSetScript(pPackageName, hash, 0x280164f, true);
    if (play_sound) {
        g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
    }
}

CustomizationScreen::CustomizationScreen(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), pReplacingOption(nullptr), DisplayHelper(sd->PackageFilename) {
    DelayFadeIn();
    Category = sd->Arg & CC_TO_CAT_MASK;
    FromCategory = static_cast<int>(static_cast<short>(sd->Arg >> 16));
    GarageMainScreen *gms;
    switch (Category) {
        case 0x601:
        case 0x602:
        case 0x603:
        case 0x604:
        case 0x605:
        case 0x606:
            gms = GarageMainScreen::GetInstance();
            *(unsigned int *)((char *)gms + 0x8c) = CustomizeDecals::CurrentDecalLocation;
            break;
        default:
            gms = GarageMainScreen::GetInstance();
            *(unsigned int *)((char *)gms + 0x8c) = Category;
            break;
    }
}

CustomizationScreen::~CustomizationScreen() {
    GarageMainScreen *gms = GarageMainScreen::GetInstance();
    *(unsigned int *)((char *)gms + 0x8c) = 0xFFFFFFFF;
}

void CustomizationScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == 0x35f8620b) {
        DisplayHelper.SetInitComplete(true);
        RefreshHeader();
    }
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        ScrollTime = RealTimer;
    }
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xc98356ba:
            if (!bNeedsRefresh) {
                return;
            }
            if ((RealTimer - ScrollTime).GetSeconds() <= 0.3f) {
                return;
            }
            bNeedsRefresh = false;
            RefreshHeader();
            break;
        case 0x5e6ea975:
            Options.StartFadeIn();
            break;
        case 0x406415e3: {
            SelectablePart *selected = GetSelectedPart();
            if (selected) {
                if (selected->IsLocked()) {
                    PlayLocked();
                    return;
                }
                if (selected->IsInCart()) {
                    PlayInCart();
                    return;
                }
                if (selected->IsInstalled()) {
                    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(GetSelectedPart());
                    if (item) {
                        gCarCustomizeManager.RemoveFromCart(item);
                        SelectablePart *incart = FindInCartPart();
                        if (incart) {
                            incart->UnSetInCart();
                        }
                    }
                    PlayInstalled();
                    return;
                }
            }
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(GetSelectedPart());
            if (item) {
                pReplacingOption = FindMatchingOption(item->GetBuyingPart());
            }
            cFEng::Get()->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xFF);
            break;
        }
        case 0x91dfdf84: {
            if (pReplacingOption) {
                pReplacingOption->GetPart()->UnSetInCart();
                pReplacingOption = nullptr;
            }
            gCarCustomizeManager.AddToCart(GetSelectedPart());
            GetSelectedPart()->SetInCart();
            RefreshHeader();
            break;
        }
        case 0xcf91aacd:
            CustomizeShoppingCart::ExitShoppingCart();
            break;
        case 0xc519bfbf: {
            switch (Category) {
                case 0x201:
                case 0x202:
                case 0x203:
                case 0x204:
                case 0x205:
                case 0x206:
                case 0x207:
                case 0x307:
                    return;
            }
            Showcase::FromIndex = Options.GetCurrentOption() ? Options.GetCurrentIndex() : 0;
            Showcase::FromArgs = Category | (FromCategory << 16);
            Showcase::FromPackage = GetPackageName();
            cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(gCarCustomizeManager.GetTuningCar()), 0, false);
            break;
        }
        case 0xb5af2461:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
    }
}

void CustomizationScreen::RefreshHeader() {
    IconScrollerMenu::RefreshHeader();
    DisplayHelper.DrawTitle();
    if (!Options.IsEmpty()) {
        int tradeInValue = 0;
        if (gCarCustomizeManager.IsCareerMode()) {
            if (!CustomizeIsInBackRoom()) {
                SelectablePart *spart = GetSelectedPart();
                if (spart) {
                    if (gCarCustomizeManager.CanTradeIn(spart)) {
                        CarPart *cpart = gCarCustomizeManager.GetInstalledCarPart(spart->GetSlotID());
                        if (cpart) {
                            tradeInValue = UnlockSystem::GetCarPartCost(gCarCustomizeManager.GetUnlockFilter(), spart->GetSlotID(), cpart, 0);
                        }
                        tradeInValue = static_cast<int>(static_cast<float>(tradeInValue) * gTradeInFactor);
                    }
                }
            }
        }
        DisplayHelper.SetCareerStuff(GetSelectedPart(), Category, tradeInValue);

        DisplayHelper.SetPartStatus(GetSelectedPart(), static_cast<CustomizePartOption *>(Options.GetCurrentOption())->GetUnlockBlurb(),
                                    Options.GetCurrentIndex(), Options.CountElements() - Options.iNumBookEnds);
    }
}

void CustomizationScreen::AddPartOption(SelectablePart *part, unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash,
                                        unsigned int unlock_hash, bool locked) {
    CustomizePartOption *opt = new ("CustomizePartOption", 0) CustomizePartOption(part, tex_hash, name_hash, desc_hash, unlock_hash);
    AddOption(opt);
    opt->SetLocked(locked);
}

SelectablePart *CustomizationScreen::FindInCartPart() {
    for (CustomizePartOption *opt = static_cast<CustomizePartOption *>(Options.GetHead()); !Options.IsEndOfList(opt);
         opt = static_cast<CustomizePartOption *>(opt->GetNext())) {
        SelectablePart *part = opt->GetPart();
        if (part && part->IsInCart()) {
            return part;
        }
    }
    return nullptr;
}

CustomizePartOption *CustomizationScreen::FindMatchingOption(SelectablePart *to_find) {
    for (CustomizePartOption *opt = static_cast<CustomizePartOption *>(Options.GetHead()); !Options.IsEndOfList(opt);
         opt = static_cast<CustomizePartOption *>(opt->GetNext())) {
        SelectablePart *part = opt->GetPart();
        if (to_find->IsPerformancePkg()) {
            if (part->GetPhysicsType() == to_find->GetPhysicsType() && part->GetUpgradeLevel() == to_find->GetUpgradeLevel()) {
                return opt;
            }
        } else {
            if (part->GetPart() == to_find->GetPart()) {
                return opt;
            }
        }
    }
    return nullptr;
}

static void UnLoadCustomHUDPacksAndTextures() {
    for (int i = 0; i < 11; i++) {
        for (uint32 j = 0; j < 5; j++) {
            if (CustomizeHUDTexTextureResources[i][j] != 0) {
                uint32 handle = CustomizeHUDTexTextureResources[i][j];
                eUnloadStreamingTexture(&handle, 1);
            }
            CustomizeHUDTexTextureResources[i][j] = 0;
        }
        if (CustomizeHUDTexPackResources[i] != 0) {
            char buf[64];
            bSPrintf(buf, "GLOBAL\\HUDS_Custom_%2.2d.bin", i);
            eUnloadStreamingTexturePack(buf);
        }
        CustomizeHUDTexPackResources[i] = 0;
    }
    CustomizeParts::TexturePackLoaded = 0;
}

CustomizeParts::CustomizeParts(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    bTexturesNeedUnload = false;
    if (GetCategory() == CC_CUSTOM_HUD) {
        if (!TexturePackLoaded) {
            for (int i = 0; i < 11; i++) {
                CustomizeHUDTexPackResources[i] = 0;
                for (unsigned int j = 0; j < 5; j++) {
                    CustomizeHUDTexTextureResources[i][j] = 0;
                }
            }
        }
        TachRPM = gCarCustomizeManager.GetMaxRPM();
        if (TachRPM >= 0x251d) {
            TachRPM = 10000;
        } else if (TachRPM >= 0x2135) {
            TachRPM = 9000;
        } else if (TachRPM > 0x1d4c) {
            TachRPM = 8000;
        } else {
            TachRPM = 7000;
        }
        FEngSetInvisible(GetPackageName(), 0xdee8632b);
    }
    Setup();
}

CustomizeParts::~CustomizeParts() {
    if (TexturePackLoaded && bTexturesNeedUnload) {
        UnLoadCustomHUDPacksAndTextures();
    }
}

void CustomizeParts::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg != 0x406415e3) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case 0x406415e3:
            switch (GetCategory()) {
                case 0x307: {
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
                    gCarCustomizeManager.SetTempColoredPart(new ("SelectablePart 1", 0) SelectablePart(sel));
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeHudColorPkg, GetCategory() | (GetFromCategory() << 16), 0, false);
                    break;
                }
                case 0x402:
                case 0x403:
                case 0x404:
                case 0x405:
                case 0x406:
                case 0x407:
                case 0x408:
                case 0x409: {
                    SelectablePart *sel = GetSelectedPart();
                    if (sel && (sel->GetPartState() & CPS_GAME_STATE_MASK) == CPS_LOCKED) {
                        DisplayHelper.FlashStatusIcon(CPS_LOCKED, true);
                        return;
                    }
                    unsigned int tunable = sel->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0);
                    if (tunable == 0) {
                        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
                        return;
                    }
                    gCarCustomizeManager.SetTempColoredPart(new ("SelectablePart 2", 0) SelectablePart(sel));
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizePaintPkg, GetCategory() | (GetFromCategory() << 16), 0, false);
                    break;
                }
                default:
                    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
                    return;
            }
            break;
        case 0xcf91aacd:
            if (GetCategory() != CC_CUSTOM_HUD) {
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
            switch (GetCategory()) {
                case 0x307:
                    if (!TexturePackLoaded) {
                        return;
                    }
                    bTexturesNeedUnload = true;
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | (CC_CUSTOM_HUD << 16), 0, false);
                    break;
                case 0x402:
                case 0x403:
                case 0x404:
                case 0x405:
                case 0x406:
                case 0x407:
                case 0x408:
                case 0x409:
                    gCarCustomizeManager.ClearTempColoredPart();
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
                    break;
                default:
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
                    break;
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
    int installed_index;
    int current_part_index;
    unsigned int original_icon_hash;
    SelectablePart *part;

    switch (Category) {
        case 0x101:
            SetTitleHash(0x6134c218);
            icon_hash = 0x28c24f6;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0xaf393dba;
            }
            car_slot_id = 0x17;
            break;
        case 0x104:
            SetTitleHash(0x4d4a88d);
            icon_hash = 0x28f7092;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0xf375276e;
            }
            car_slot_id = 0x3f;
            break;
        case 0x105:
            SetTitleHash(0x61e8f83c);
            icon_hash = 0x79165861;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0x25a4375e;
            }
            car_slot_id = 0x3e;
            break;
        case 0x307:
            if (!CustomizeParts::TexturePackLoaded) {
                const u32 FEObj_DISABLE_INPUTS = __DISABLE_INPUTS__;
                cFEng::Get()->QueuePackageMessage(FEObj_DISABLE_INPUTS, GetPackageName(), nullptr);
                LoadHudTextures();
            } else {
                ShowHudObjects();
                const u32 FEObj_ENABLE_INPUTS = __ENABLE_INPUTS__;
                cFEng::Get()->QueuePackageMessage(FEObj_ENABLE_INPUTS, GetPackageName(), nullptr);
            }
            if (gCarCustomizeManager.GetTempColoredPart()) {
                installed_part = gCarCustomizeManager.GetTempColoredPart()->GetPart();
                part_found = true;
            }
            SetTitleHash(0x78980a6b);
            icon_hash = 0x28f88bc;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0x8ba602fc;
            }
            car_slot_id = 0x84;
            break;
        case 0x304:
            SetTitleHash(0xd32729a6);
            icon_hash = 0x3f23165c;
            car_slot_id = 0x83;
            break;
        case 0x402:
            SetTitleHash(0xd9228fc6);
            icon_hash = 0xf8148554;
            car_slot_id = 0x4d;
            vinyl_group_number = 0;
            is_vinyl = true;
            break;
        case 0x403:
            SetTitleHash(0x1e8d885f);
            icon_hash = 0x192d84da;
            car_slot_id = 0x4d;
            vinyl_group_number = 1;
            is_vinyl = true;
            break;
        case 0x404:
            SetTitleHash(0x1c619fd8);
            icon_hash = 0xf7352706;
            car_slot_id = 0x4d;
            vinyl_group_number = 2;
            is_vinyl = true;
            break;
        case 0x405:
            SetTitleHash(0x9c1b8935);
            icon_hash = 0x1223cc89;
            car_slot_id = 0x4d;
            vinyl_group_number = 3;
            is_vinyl = true;
            break;
        case 0x406:
            SetTitleHash(0x7956f7b0);
            icon_hash = 0xbc44bbcb;
            car_slot_id = 0x4d;
            vinyl_group_number = 4;
            is_vinyl = true;
            break;
        case 0x407:
            SetTitleHash(0x2d5bff0f);
            icon_hash = 0x694ca0ca;
            car_slot_id = 0x4d;
            vinyl_group_number = 5;
            is_vinyl = true;
            break;
        case 0x408:
            SetTitleHash(0x209a9158);
            icon_hash = 0x1b3a8dd3;
            car_slot_id = 0x4d;
            vinyl_group_number = 6;
            is_vinyl = true;
            break;
        case 0x409:
            SetTitleHash(0xcd057d21);
            icon_hash = 0x1ba508fc;
            car_slot_id = 0x4d;
            vinyl_group_number = 7;
            is_vinyl = true;
            break;
        default:
            break;
    }

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

    installed_index = 0;
    current_part_index = 1;
    original_icon_hash = icon_hash;
    part = part_list.GetHead();

    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        unsigned int unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), part->GetUpgradeLevel());

        if (is_vinyl) {
            if ((part->GetPart()->GetGroupNumber()) == vinyl_group_number) {
                if (UnlockSystem::IsUnlockableAvailable(part->GetPart()->GetPartNameHash())) {
                    AddPartOption(part, icon_hash, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash,
                                  gCarCustomizeManager.IsPartLocked(part, 0));
                } else {
                    delete part;
                    part = nullptr;
                }
            } else {
                delete part;
                part = nullptr;
            }
        } else {
            if (part->GetPart()->HasAppliedAttribute(bStringHash("CARBONFIBRE")) &&
                part->GetPart()->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0) != 0) {
                switch (Category) {
                    case 0x104:
                        icon_hash = 0x68495926;
                        if (gCarCustomizeManager.IsInBackRoom()) {
                            icon_hash = 0x2478e136;
                        }
                        break;
                    case 0x105:
                        icon_hash = 0xfc618215;
                        if (gCarCustomizeManager.IsInBackRoom()) {
                            icon_hash = 0xcd6b4e26;
                        }
                        break;
                }
            } else {
                icon_hash = original_icon_hash;
            }
            AddPartOption(part, icon_hash, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash,
                          gCarCustomizeManager.IsPartLocked(part, 0));
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
}

void CustomizeParts::LoadHudTextures() {
    PacksLoadedCount = 0;
    LoadNextHudTexturePack();
}

void CustomizeParts::LoadNextHudTexturePack() {
    char buf[64];
    bSPrintf(buf, "GLOBAL\\HUDS_Custom_%2.2d.bin", PacksLoadedCount);
    int result = eLoadStreamingTexturePack(buf, reinterpret_cast<void (*)(void *)>(TexturePackLoadedCallbackAccessor), this, 0);
    CustomizeHUDTexPackResources[PacksLoadedCount] = (result != 0) ? 1 : 0;
}

void CustomizeParts::TexturePackLoadedCallback() {
    int hud_num = PacksLoadedCount;
    PacksLoadedCount = hud_num + 1;
    CustomizeHUDTexTextureResources[hud_num][0] = FEngHashString("TACH_FILL_%2.2d", hud_num);
    CustomizeHUDTexTextureResources[hud_num][1] = FEngHashString("%0.4d_LINES_%2.2d", TachRPM, hud_num);
    CustomizeHUDTexTextureResources[hud_num][2] = FEngHashString("TURBO_LINES_%2.2d", hud_num);
    CustomizeHUDTexTextureResources[hud_num][3] = FEngHashString("TACH_NEEDLE_%2.2d", hud_num);
    CustomizeHUDTexTextureResources[hud_num][4] = FEngHashString("TURBO_NEEDLE_%2.2d", hud_num);
    eLoadStreamingTexture(CustomizeHUDTexTextureResources[hud_num], 5, TextureLoadedCallbackAccessor, reinterpret_cast<uint32>(this), 0);
}

void CustomizeParts::TextureLoadedCallback() {
    if (PacksLoadedCount > 10) {
        TexturePackLoaded = true;
        cFEng::Get()->MakeLoadedPackagesDirty();
        ShowHudObjects();
        RefreshHeader();
        cFEng::Get()->QueuePackageMessage(__ENABLE_INPUTS__, GetPackageName(), nullptr);
    } else {
        LoadNextHudTexturePack();
    }
}

void CustomizeParts::ShowHudObjects() {
    const uint32 FEObj_APPEAR = FEHASH_APPEAR;
    FEngSetScript(GetPackageName(), 0xDEE8632B, FEObj_APPEAR, true);
    FEngSetVisible(GetPackageName(), 0xDEE8632B);
}

void CustomizeParts::SetHUDTextures() {
    int hud_index = GetSelectedPart()->GetPart()->GetAppliedAttributeIParam(FEngHashString("HUDINDEX"), 0);
    FEngSetTextureHash(GetPackageName(), 0xc0721eb9, FEngHashString("%0.4d_LINES_%2.2d", TachRPM, hud_index));
    FEngSetTextureHash(GetPackageName(), 0x5d19f25, FEngHashString("TACH_FILL_%2.2d", hud_index));
    FEngSetTextureHash(GetPackageName(), 0xd312f0cb, FEngHashString("TACH_NEEDLE_%2.2d", hud_index));
    if (gCarCustomizeManager.IsTurbo()) {
        FEngSetTextureHash(GetPackageName(), 0xc62ad685, FEngHashString("TURBO_LINES_%2.2d", hud_index));
        FEngSetTextureHash(GetPackageName(), 0x8fe2a217, FEngHashString("TURBO_NEEDLE_%2.2d", hud_index));
        FEngSetVisible(GetPackageName(), 0xc5d551b7);
    } else {
        FEngSetInvisible(GetPackageName(), 0xc5d551b7);
    }
}

void CustomizeParts::SetHUDColors() {
    ShoppingCartItem *hud_item = gCarCustomizeManager.IsPartTypeInCart(0x84u);
    CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(0x84);
    if (GetSelectedPart()->GetPart() == installed_hud ||
        (hud_item && GetSelectedPart()->GetPart() == hud_item->GetBuyingPart()->GetPart())) {
        unsigned int slot_id = 0x85;
        unsigned int colors[3];
        for (int i = 0; i < 3; i++) {
            CarPart *color_part;
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
            if (item && hud_item && GetSelectedPart()->GetPart() == hud_item->GetBuyingPart()->GetPart()) {
                color_part = item->GetBuyingPart()->GetPart();
            } else {
                color_part = gCarCustomizeManager.GetInstalledCarPart(slot_id);
            }
            unsigned char r = color_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            unsigned char g = color_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            unsigned char b = color_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            colors[i] = 0xff000000 | (r << 16) | (g << 8) | (b & 0xff);
            slot_id++;
        }
        FEngSetColor(GetPackageName(), 0x5d19f25, colors[0]);
        FEngSetColor(GetPackageName(), 0xc0721eb9, colors[2]);
        FEngSetColor(GetPackageName(), 0xc62ad685, colors[2]);
        FEngSetColor(GetPackageName(), 0xb8f1f802, colors[2]);
        FEngSetColor(GetPackageName(), 0xd312f0cb, colors[1]);
        FEngSetColor(GetPackageName(), 0x8fe2a217, colors[1]);
    } else {
        FEngSetColor(GetPackageName(), 0x5d19f25, 0xffffc373u);
        FEngSetColor(GetPackageName(), 0xc0721eb9, 0xffffffffu);
        FEngSetColor(GetPackageName(), 0xc62ad685, 0xffffffffu);
        FEngSetColor(GetPackageName(), 0xb8f1f802, 0xffffffffu);
        FEngSetColor(GetPackageName(), 0xd312f0cb, 0xffffae40u);
        FEngSetColor(GetPackageName(), 0x8fe2a217, 0xffffae40u);
    }
}

void CustomizeParts::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    if (!Options.IsEmpty()) {
        SelectablePart *sel = GetSelectedPart();
        if (sel->GetPart()->HasAppliedAttribute(0x6212682b)) {
            unsigned int tunable = sel->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0);
            if (tunable) {
                FEngSetLanguageHash(GetPackageName(), 0xb94139f4, LANGUAGE_COMMON_CONTINUE);
            } else {
                FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x649f4a65);
            }
        }
        if (Category == CC_CUSTOM_HUD) {
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

CustomizeSpoiler::CustomizeSpoiler(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    TheFilter = 0;
    for (int i = 0; i < 4; i++) {
        SelectedIndex[i] = 1;
    }
    Setup();
}

void CustomizeSpoiler::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case __PAD_LTRIGGER__:
            ScrollFilters(eSD_PREV);
            break;
        case __PAD_RTRIGGER__:
            ScrollFilters(eSD_NEXT);
            break;
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            SelectedIndex[TheFilter] = Options.GetCurrentIndex();
            break;
        case __PAD_BUTTON0__:
            Showcase::FromFilter = TheFilter;
            break;
        case FEMSG_BACK_OUT: {
            SelectablePart *part = FindInCartPart();
            if (!part) {
                return;
            }
            if (gCarCustomizeManager.IsPartInCart(part)) {
                return;
            }
            part->UnSetInCart();
            RefreshHeader();
            break;
        }
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
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

void CustomizeSpoiler::BuildPartOptionListFromFilter(CarPart *installed) {
    SelectablePart *part;
    unsigned int current_part_index;

    Options.RemoveAll();
    current_part_index = 1;
    Options.AddInitialBookEnds();
    bTList<SelectablePart> part_list;
    gCarCustomizeManager.GetCarPartList(0x2c, part_list, 0);
    part = part_list.GetHead();
    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        if (part->GetPart()->GetGroupNumber() == static_cast<unsigned int>(TheFilter) || part->GetPart()->GetGroupNumber() == 4) {
            unsigned int unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), part->GetUpgradeLevel());
            unsigned int icon = 0xbb034ea6;
            if (CustomizeIsInBackRoom()) {
                icon = 0xc51a4f62;
            }
            if (part->GetPart()->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0) != 0) {
                icon = 0x4d1c18ba;
                if (CustomizeIsInBackRoom()) {
                    icon = 0x611d142a;
                }
            }
            AddPartOption(part, icon, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
            if (SelectedIndex[TheFilter] == 1) {
                if (installed && part->GetPart() == installed) {
                    SelectedIndex[TheFilter] = current_part_index;
                }
                current_part_index++;
            }
        } else {
            delete part;
        }
    }
    if (Showcase::FromIndex != 0) {
        SelectedIndex[TheFilter] = Showcase::FromIndex;
        SetInitialOption(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(SelectedIndex[TheFilter]);
    }
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
    Timer scrollDelay(0.3f);
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

HUDLayerOption::HUDLayerOption(uint32 layer, uint32 icon_hash, uint32 name_hash)
    : CustomizePartOption(nullptr, icon_hash, name_hash, 0, 0), //
      HUDLayer(layer),                                          //
      SelectedPart(nullptr) {
    gCarCustomizeManager.GetCarPartList(layer, TheColors, 0);
}

CustomizeHUDColor::CustomizeHUDColor(ScreenConstructorData *sd)
    : CustomizationScreen(sd), //
      SelectedColor(nullptr),  //
      Cursor(nullptr),         //
      bTexturesNeedUnload(false) {
    Cursor = FEngFindObject(GetPackageName(), 0xB893252A);
    Setup();
}

CustomizeHUDColor::~CustomizeHUDColor() {
    if (CustomizeParts::TexturePackLoaded && bTexturesNeedUnload) {
        UnLoadCustomHUDPacksAndTextures();
    }
}

void CustomizeHUDColor::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == 0x9120409e || msg == 0xb5971bf1) {
        HUDLayerOption *layer = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
        layer->SelectedPart = SelectedColor->ThePart;
    }
    if (msg != 0x91dfdf84) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case 0x72619778:
            ScrollColors(eSD_PREV);
            break;
        case 0x911c0a4b:
            ScrollColors(eSD_NEXT);
            break;
        case 0x9120409e:
        case 0xb5971bf1:
            BuildColorOptions();
            RefreshHeader();
            break;
        case 0xcf91aacd:
            gCarCustomizeManager.ClearTempColoredPart();
            bTexturesNeedUnload = true;
            break;
        case 0x91dfdf84: {
            ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(0x84u);
            if (inCart && gCarCustomizeManager.GetTempColoredPart()->GetPart() != inCart->GetBuyingPart()->GetPart()) {
                int slot = 0x85;
                do {
                    ShoppingCartItem *colorItem = gCarCustomizeManager.IsPartTypeInCart(static_cast<unsigned int>(slot));
                    slot++;
                    gCarCustomizeManager.RemoveFromCart(colorItem);
                } while (slot < 0x88);
            }
            gCarCustomizeManager.AddToCart(gCarCustomizeManager.GetTempColoredPart());
            gCarCustomizeManager.ClearTempColoredPart();
            HUDLayerOption *node = static_cast<HUDLayerOption *>(Options.GetHead());
            while (!Options.IsEndOfList(node)) {
                if (node->SelectedPart != nullptr) {
                    gCarCustomizeManager.AddToCart(node->SelectedPart);
                }
                node = static_cast<HUDLayerOption *>(node->GetNext());
            }
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeHudPkg, GetCategory() | (GetFromCategory() << 16), 0, false);
            break;
        }
        case 0x911ab364:
            gCarCustomizeManager.ClearTempColoredPart();
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeHudPkg, GetCategory() | (GetFromCategory() << 16), 0, false);
            break;
    }
}

void CustomizeHUDColor::ScrollColors(eScrollDir dir) {
    HUDColorOption *prev = SelectedColor;
    if (dir == eSD_PREV) {
        SelectedColor = ColorOptions.GetPrevCircular(prev);
    } else if (dir == eSD_NEXT) {
        SelectedColor = ColorOptions.GetNextCircular(prev);
    }
    if (SelectedColor != prev) {
        HUDLayerOption *opt = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
        opt->SelectedPart = SelectedColor->ThePart;
        FEngSetScript(prev->FEngObject, FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(SelectedColor->FEngObject, FEHASH_HIGHLIGHT, true);
        float x_offset = 69.0f;
        float y_offset = 56.0f;
        FEngSetTopLeft(Cursor, FEngGetTopLeftX(SelectedColor->FEngObject) + x_offset, FEngGetTopLeftY(SelectedColor->FEngObject) + y_offset);
        RefreshHeader();
    }
}

void CustomizeHUDColor::AddLayerOption(uint32 layer, uint32 icon_hash, uint32 name_hash) {
    AddOption(new ("HUDLayerOption", 0) HUDLayerOption(layer, icon_hash, name_hash));
}

void CustomizeHUDColor::Setup() {
    DisplayHelper.SetTitleHash(0xb1b0e8af);
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

void CustomizeHUDColor::SetInitialColors() {
    ShoppingCartItem *cart_item = gCarCustomizeManager.IsPartTypeInCart(0x84u);
    CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(0x84);
    unsigned int colors[3] = {0xffffc373u, 0xffffae40u, 0xffffffffu};
    if (cart_item && gCarCustomizeManager.GetTempColoredPart()->GetPart() == cart_item->GetBuyingPart()->GetPart()) {
        unsigned int carslot_id = 0x85;
        for (int i = 0; i < 3; i++) {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(carslot_id);
            carslot_id++;
            if (item) {
                CarPart *color_part = item->GetBuyingPart()->GetPart();
                unsigned char r = color_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                unsigned char g = color_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                unsigned char b = color_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                colors[i] = 0xff000000 | (r << 16) | (g << 8) | (b & 0xff);
            }
        }
    } else if (gCarCustomizeManager.GetTempColoredPart()->GetPart() == installed_hud) {
        unsigned int carslot_id = 0x85;
        for (int i = 0; i < 3; i++) {
            CarPart *color_part = gCarCustomizeManager.GetInstalledCarPart(carslot_id);
            carslot_id++;
            if (color_part) {
                unsigned char r = color_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                unsigned char g = color_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                unsigned char b = color_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                colors[i] = 0xff000000 | (r << 16) | (g << 8) | (b & 0xff);
            }
        }
    }
    FEngSetColor(GetPackageName(), 0x5d19f25, colors[0]);
    FEngSetColor(GetPackageName(), 0xd312f0cb, colors[1]);
    FEngSetColor(GetPackageName(), 0x8fe2a217, colors[1]);
    FEngSetColor(GetPackageName(), 0xc0721eb9, colors[2]);
    FEngSetColor(GetPackageName(), 0xc62ad685, colors[2]);
    FEngSetColor(GetPackageName(), 0xb8f1f802, colors[2]);
}

void CustomizeHUDColor::SetHUDTextures() {
    int rpm = gCarCustomizeManager.GetMaxRPM();
    if (rpm >= 0x251d) {
        rpm = 10000;
    } else if (rpm >= 0x2135) {
        rpm = 9000;
    } else if (rpm > 0x1d4c) {
        rpm = 8000;
    } else {
        rpm = 7000;
    }
    SelectablePart *temp_colored = gCarCustomizeManager.GetTempColoredPart();
    int hud_index = temp_colored->GetPart()->GetAppliedAttributeIParam(FEngHashString("HUDINDEX"), 0);
    FEngSetTextureHash(GetPackageName(), 0xc0721eb9, FEngHashString("%d_LINES_%2.2d", rpm, hud_index));
    FEngSetTextureHash(GetPackageName(), 0x5d19f25, FEngHashString("TACH_FILL_%2.2d", hud_index));
    FEngSetTextureHash(GetPackageName(), 0xd312f0cb, FEngHashString("TACH_NEEDLE_%2.2d", hud_index));
    if (gCarCustomizeManager.IsTurbo()) {
        FEngSetTextureHash(GetPackageName(), 0xc62ad685, FEngHashString("TURBO_LINES_%2.2d", hud_index));
        FEngSetTextureHash(GetPackageName(), 0x8fe2a217, FEngHashString("TURBO_NEEDLE_%2.2d", hud_index));
        FEngSetVisible(GetPackageName(), 0xc5d551b7);
    } else {
        FEngSetInvisible(GetPackageName(), 0xc5d551b7);
    }
}

void CustomizeHUDColor::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    switch (SelectedColor->ThePart->GetSlotID()) {
        case CARSLOTID_HUD_BACKING_COLOUR:
            FEngSetColor(GetPackageName(), 0x5d19f25, SelectedColor->color);
            break;

        case CARSLOTID_HUD_CHARACTER_COLOUR:
            FEngSetColor(GetPackageName(), 0xc0721eb9, SelectedColor->color);
            FEngSetColor(GetPackageName(), 0xc62ad685, SelectedColor->color);
            FEngSetColor(GetPackageName(), 0xb8f1f802, SelectedColor->color);
            break;

        case CARSLOTID_HUD_NEEDLE_COLOUR:
            FEngSetColor(GetPackageName(), 0xd312f0cb, SelectedColor->color);
            FEngSetColor(GetPackageName(), 0x8fe2a217, SelectedColor->color);
            break;
    }
}

void CustomizeHUDColor::BuildColorOptions() {
    if (SelectedColor) {
        FEngSetScript(SelectedColor->FEngObject, FEHASH_UNHIGHLIGHT, true);
        SelectedColor = nullptr;
    }
    HUDLayerOption *opt = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
    if (opt && !opt->TheColors.IsEmpty()) {
        ColorOptions.DeleteAllElements();
        ShoppingCartItem *cart_item = gCarCustomizeManager.IsPartTypeInCart(0x84u);
        CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(0x84);
        int i = 0;
        for (SelectablePart *part = opt->TheColors.GetHead(); part != opt->TheColors.EndOfList(); part = part->GetNext()) {
            i++;
            HUDColorOption *color_option = new ("HUDColorOption", 0) HUDColorOption(part);
            FEImage *obj = FEngFindImage(GetPackageName(), FEngHashString("COLOR_%d", i));
            color_option->SetFEngObject(obj);
            ColorOptions.AddTail(color_option);
            unsigned char r = part->GetPart()->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            unsigned char g = part->GetPart()->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            unsigned char b = part->GetPart()->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            color_option->color = 0xFF000000 | (static_cast<unsigned int>(r) << 16) | (static_cast<unsigned int>(g) << 8) | static_cast<unsigned int>(b);
            FEngSetColor(obj, color_option->color);
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
        }
        if (!SelectedColor) {
            SelectedColor = ColorOptions.GetHead();
        }
        float x_offset = 69.0f;
        float y_offset = 56.0f;
        FEngSetTopLeft(Cursor, FEngGetTopLeftX(SelectedColor->FEngObject) + x_offset, FEngGetTopLeftY(SelectedColor->FEngObject) + y_offset);
    }
}

CustomizeRims::CustomizeRims(ScreenConstructorData *sd)
    : CustomizationScreen(sd), //
      InnerRadius(0xf),        //
      MinRadius(0xf),          //
      MaxRadius(0xf) {
    Setup();
}

void CustomizeRims::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
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
        case FEMSG_BACK_OUT: {
            SelectablePart *part = FindInCartPart();
            if (part && !gCarCustomizeManager.IsPartInCart(part)) {
                part->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case 0x406415e3:
            break;
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
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
        InnerRadius = radius;
        BuildRimsList(Options.GetCurrentIndex());
        RefreshHeader();
    }
}

void CustomizeRims::Setup() {
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    DisplayHelper.SetTitleHash(0xe167f7c8);
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
    SelectablePart *part;
    int installed_index;
    int current_part_index;
    CarPart *installed_part;

    Options.RemoveAll();
    installed_index = 0;
    Options.AddInitialBookEnds();
    current_part_index = 1;
    installed_part = nullptr;
    bTList<SelectablePart> part_list;
    gCarCustomizeManager.GetCarPartList(0x42, part_list, GetCategoryBrandHash());
    if (selected_index == -1) {
        installed_part = gCarCustomizeManager.GetActivePartFromSlot(0x42);
    }
    part = part_list.GetHead();
    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        CarPart *carpet = part->GetPart();
        if (carpet->GetInnerRadius() == InnerRadius) {
            unsigned int unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), part->GetUpgradeLevel());
            AddPartOption(part, 0x294d2a3, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
            if (installed_part && part->GetPart() == installed_part) {
                installed_index = current_part_index;
            }
            current_part_index++;
        } else {
            delete part;
        }
    }
    if (selected_index == -1) {
        selected_index = 1;
        if (installed_part) {
            selected_index = installed_index;
        }
    }
    if (Showcase::FromIndex != 0) {
        SetInitialOption(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(selected_index);
    }
}

void CustomizeRims::RefreshHeader() {
    CustomizationScreen::RefreshHeader();

    if (Options.IsEmpty()) {
        return;
    }

    SelectablePart *part = GetSelectedPart();
    gCarCustomizeManager.PreviewPart(part->GetSlotID(), part->GetPart());
    FEPrintf(GetPackageName(), 0xe6782841, "%$d\"", InnerRadius);
    char sztemp[64];
    bSNPrintf(sztemp, 64, "%s", part->GetPart()->GetName());
    int len = bStrLen(sztemp);
    for (int i = len; i >= len - 6; i--) {
        sztemp[i] = 0;
    }
    FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", sztemp);
}

uint32 CustomizeRims::GetCategoryBrandHash() {
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

CustomizePaint::CustomizePaint(ScreenConstructorData *sd)
    : CustomizationScreen(sd),            //
      TheFilter(-1),                      //
      MatchingPaint(nullptr, 0, 0, 0, 0), //
      ThePaints(sd->PackageFilename, 20, 4, true) {
    NumRemapColors = 0;
    for (int i = 0; i <= 2; i++) {
        VinylColors[i] = nullptr;
    }
    ThePaints.SetMouseDownMsg(0x406415e3);
    Setup();
}

eMenuSoundTriggers CustomizePaint::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (static_cast<unsigned int>(maybe - 0x29) < 2) {
        if (Category == CC_RIM_PAINT) {
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

void CustomizePaint::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x9120409e:
        case 0xb5971bf1:
            break;
        case 0x406415e3:
            if (Category == CC_PAINT || Category == CC_RIM_PAINT) {
                CustomizationScreen::NotificationMessage(0x406415e3, pobj, param1, param2);
            }
            break;
        default:
            CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
            break;
    }

    ThePaints.NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case 0xc519bfbf:
            Showcase::FromFilter = TheFilter;
            Showcase::FromIndex = ThePaints.GetCurrentDatumNum();
#ifndef EA_BUILD_A124
            for (int i = 0; i < 3; i++) {
                Showcase::FromColor[i] = VinylColors[i];
            }
#endif
            break;
        case 0x5073ef13:
            ScrollFilters(static_cast<eScrollDir>(-1));
            break;
        case 0xd9feec59:
            ScrollFilters(static_cast<eScrollDir>(1));
            break;
        case 0x406415e3: {
            CarPart *installed;
            CarPart *temp_colorable;
            bool add_to_cart;
            if (GetCategory() == CC_PAINT || GetCategory() == CC_RIM_PAINT) {
                return;
            }
            if (VinylColors[TheFilter]) {
                delete VinylColors[TheFilter];
            }
            VinylColors[TheFilter] = new ("SelectablePart 3", 0) SelectablePart(GetSelectedPart());
            installed = gCarCustomizeManager.GetActivePartFromSlot(0x4d);
            temp_colorable = gCarCustomizeManager.GetTempColoredPart()->GetPart();
            add_to_cart = false;
            if (installed != temp_colorable) {
                add_to_cart = true;
            } else {
                for (int i = 0; i < NumRemapColors; i++) {
                    installed = gCarCustomizeManager.GetActivePartFromSlot(i + 0x4f);
                    if (VinylColors[i] && installed != VinylColors[i]->GetPart()) {
                        add_to_cart = true;
                        break;
                    }
                }
            }
            if (!add_to_cart) {
                return;
            }
            AddVinylAndColorsToCart();
            cFEng::Get()->QueuePackageSwitch(g_pCustomizePartsPkg, GetCategory() | (GetFromCategory() << 16), 0, false);
            break;
        }
        case 0x911ab364:
            if (GetCategory() == CC_PAINT || GetCategory() == CC_RIM_PAINT) {
                cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
                return;
            }
            for (int i = 0; i < 3; i++) {
                if (VinylColors[i]) {
                    delete VinylColors[i];
                }
                VinylColors[i] = nullptr;
#ifndef EA_BUILD_A124
                Showcase::FromColor[i] = nullptr;
#endif
            }
            gCarCustomizeManager.ResetPreview();
            cFEng::Get()->QueuePackageSwitch(g_pCustomizePartsPkg, GetCategory() | (GetFromCategory() << 16), 0, false);
            break;
        case 0x5a928018: {
            SelectablePart *part = FindInCartPart();
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

void CustomizePaint::AddVinylAndColorsToCart() {
    SelectablePart *mainPart = gCarCustomizeManager.GetTempColoredPart();
    gCarCustomizeManager.AddToCart(mainPart);
    for (int i = 0; i < 3; i++) {
        if (i < NumRemapColors && VinylColors[i]) {
            gCarCustomizeManager.AddToCart(VinylColors[i]);
        }
        delete VinylColors[i];
        VinylColors[i] = nullptr;
#ifndef EA_BUILD_A124
        Showcase::FromColor[i] = nullptr;
#endif
    }
}

void CustomizePaint::ScrollFilters(eScrollDir dir) {
    int max;
    if (Category == CC_PAINT) {
        max = 2;
    } else if (Category == CC_RIM_PAINT) {
        return;
    } else {
        max = NumRemapColors - 1;
        if (max != 0) {
            if (GetSelectedPart() != VinylColors[TheFilter]) {
                if (VinylColors[TheFilter]) {
                    delete VinylColors[TheFilter];
                }
                VinylColors[TheFilter] = new ("SelectablePart 4", 0) SelectablePart(GetSelectedPart());
            }
        }
    }
    int filter = TheFilter;
    if (dir == static_cast<eScrollDir>(-1)) {
        filter = filter - 1;
        if (filter < 0) {
            filter = max;
        }
    } else if (dir == static_cast<eScrollDir>(1)) {
        filter = filter + 1;
        if (filter > max) {
            filter = 0;
        }
    }
    if (filter != TheFilter) {
        SelectedIndex[TheFilter] = ThePaints.GetCurrentDatumNum() - 1;
        TheFilter = filter;
        if (Category == CC_PAINT || Category == CC_RIM_PAINT) {
            BuildSwatchList(GetSelectedPart()->GetSlotID());
        } else {
            BuildSwatchList(filter + 0x4f);
        }
        RefreshHeader();
    }
}

void CustomizePaint::Setup() {
    FEImage *leftBtn = FEngFindImage(GetPackageName(), 0x91c4a50);
    FEngSetButtonTexture(leftBtn, 0x5bc);
    FEImage *rightBtn = FEngFindImage(GetPackageName(), 0x2d145be3);
    FEngSetButtonTexture(rightBtn, 0x682);
    for (int i = 1; i <= 0x50; i++) {
        ArraySlot *slot = new ("ArraySlot", 0) ArraySlot(FEngFindImage(GetPackageName(), static_cast<int>(FEngHashString("COLOR_%d", i))));
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
            DisplayHelper.SetTitleHash(0x55da70c);
            SetupBasePaint();
            break;
        case 0x303:
            DisplayHelper.SetTitleHash(0xe126ff53);
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
            DisplayHelper.SetTitleHash(0xd8ee1a80);
            SetupVinylColor();
            break;
    }
    Showcase::FromFilter = -1;
    Options.SetInitialized();
    RefreshHeader();
}

void CustomizePaint::SetupBasePaint() {
    BuildSwatchList(0x4C);
}

void CustomizePaint::SetupRimPaint() {
    FEngSetInvisible(GetPackageName(), 0x2C3CC2D3);
    FEngSetInvisible(GetPackageName(), 0x53639A10);
    BuildSwatchList(0x4E);
}

void CustomizePaint::SetupVinylColor() {
    unsigned int start_slot_id = 0x4f;
    if (Showcase::FromFilter != -1) {
        switch (Showcase::FromFilter) {
            case 0:
                break;
            case 1:
                start_slot_id = 0x50;
                break;
            case 2:
                start_slot_id = 0x51;
                break;
        }
        Showcase::FromFilter = -1;
    }
    BuildSwatchList(start_slot_id);
    NumRemapColors = gCarCustomizeManager.GetTempColoredPart()->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0);
    if (NumRemapColors < 2) {
        FEngSetInvisible(GetPackageName(), 0x2c3cc2d3);
        FEngSetInvisible(GetPackageName(), 0x53639a10);
    } else {
        const u32 FEObj_TRIGGER = 0x1a7240f3;
        cFEng::Get()->QueuePackageMessage(FEObj_TRIGGER, GetPackageName(), nullptr);
    }
#ifndef EA_BUILD_A124
    for (int i = 0; i < 3; i++) {
        int slot_id = i + 0x4f;
        if (Showcase::FromColor[i]) {
            VinylColors[i] = static_cast<SelectablePart *>(Showcase::FromColor[i]);
            Showcase::FromColor[i] = nullptr;
        } else {
            CarPart *part = gCarCustomizeManager.GetActivePartFromSlot(slot_id);
            if (part) {
                VinylColors[i] = new ("SelectablePart 5", 0) SelectablePart(part, slot_id, part->GetUpgradeLevel(), Physics::Upgrades::PUT_MAX, false,
                                                     static_cast<eCustomizePartState>(1), 0, false);
            } else {
                VinylColors[i] = nullptr;
            }
        }
    }
#endif
}

uint32 CustomizePaint::CalcBrandHash(CarPart *part) {
    switch (Category) {
        case 0x301:
            switch (TheFilter) {
                case 0:
                    return 0x02daab07;
                case 1:
                    return 0x03437a52;
                case 2:
                    return 0x03797533;
                default:
                    return part->GetBrandNameHash();
            }
        case 0x303:
            return 0xda27;
    }
    return 0x3e871f1;
}

void CustomizePaint::BuildSwatchList(unsigned int slot_id) {
    CarPart *installed_part = nullptr;
    ThePaints.ClearData();
    switch (slot_id) {
        case 0x4f:
        case 0x50:
        case 0x51:
            break;
        default:
            goto skip_color;
    }
    {
        int showcase_index_num = 0;
        switch (slot_id) {
            case 0x4f:
                showcase_index_num = 0;
                break;
            case 0x50:
                showcase_index_num = 1;
                break;
            case 0x51:
                showcase_index_num = 2;
                break;
        }
#ifndef EA_BUILD_A124
        if (Showcase::FromColor[showcase_index_num] && !VinylColors[showcase_index_num]) {
            installed_part = static_cast<SelectablePart *>(Showcase::FromColor[showcase_index_num])->GetPart();
        }
#endif
    }
skip_color:
    if (!installed_part) {
        installed_part = gCarCustomizeManager.GetActivePartFromSlot(slot_id);
    }
    unsigned int brand_hash = CalcBrandHash(installed_part);
    if (TheFilter == -1) {
        switch (brand_hash) {
            case 0x2daab07:
                TheFilter = 0;
                break;
            case 0xda27:
                TheFilter = 0;
                break;
            case 0x3437a52:
                TheFilter = 1;
                break;
            case 0x3797533:
                TheFilter = 2;
                break;
            default:
                TheFilter = 0;
                break;
        }
    }
    bTList<SelectablePart> the_list;
    gCarCustomizeManager.GetCarPartList(slot_id, the_list, 0);
    int selected_index = 0;
    while (!the_list.IsEmpty()) {
        if (the_list.GetHead()->GetPart()->GetBrandNameHash() == brand_hash) {
            unsigned int unlock_hash =
                gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), the_list.GetHead()->GetPart()->GetUpgradeLevel());
            CustomizePaintDatum *datum = new ("CustomizePaintDatum", 0) CustomizePaintDatum(the_list.RemoveHead(), unlock_hash);
            if (SelectedIndex[TheFilter] == -1 && installed_part == datum->ThePart->GetPart()) {
                SelectedIndex[TheFilter] = selected_index;
            }
            ThePaints.AddDatum(datum);
            ImageArraySlot *slot = static_cast<ImageArraySlot *>(ThePaints.GetSlotAt(selected_index));
            if (slot) {
                unsigned char r = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                unsigned char g = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                unsigned char b = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                unsigned int color = 0xff000000 | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
                FEngSetColor(slot->GetFEngObject(), color);
            }
            selected_index++;
        } else {
            delete the_list.RemoveHead();
        }
    }
    if (Showcase::FromIndex != 0) {
        SelectedIndex[TheFilter] = Showcase::FromIndex - 1;
        ThePaints.SetInitialPosition(Showcase::FromIndex - 1);
        Showcase::FromIndex = 0;
    } else {
        if (SelectedIndex[TheFilter] == -1) {
            SelectedIndex[TheFilter] = 0;
        }
        ThePaints.SetInitialPosition(SelectedIndex[TheFilter]);
    }
    RefreshHeader();
}

void CustomizePaint::RefreshHeader() {
    DisplayHelper.DrawTitle();
    ThePaints.RefreshHeader();
    unsigned int hash = 0;
    switch (TheFilter) {
        case 0:
            if (Category == CC_PAINT) {
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
            if (Category == CC_PAINT) {
                hash = 0x452b5481;
            } else if (NumRemapColors == 2) {
                hash = 0x5198be57;
            } else if (NumRemapColors == 3) {
                hash = 0x5198be58;
            }
            break;
        case 2:
            if (Category == CC_PAINT) {
                hash = 0xb715070a;
            } else if (NumRemapColors == 3) {
                hash = 0x5198c299;
            }
            break;
    }
    FEngSetLanguageHash(GetPackageName(), 0x78008599, hash);
    switch (GetCategory()) {
        case 0x301:
            gCarCustomizeManager.PreviewPart(GetSelectedPart()->GetSlotID(), GetSelectedPart()->GetPart());
            break;
        case 0x303:
            gCarCustomizeManager.PreviewPart(GetSelectedPart()->GetSlotID(), GetSelectedPart()->GetPart());
            FEngSetInvisible(GetPackageName(), 0x2c526172);
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0xb3100a3e);
            break;
        default: {
            SelectablePart *temp = gCarCustomizeManager.GetTempColoredPart();
            gCarCustomizeManager.PreviewPart(temp->GetSlotID(), temp->GetPart());
            if (NumRemapColors == 1) {
                FEngSetInvisible(GetPackageName(), 0x2c526172);
            }
            for (int i = 0; i < 3; i++) {
                if (i < NumRemapColors && VinylColors[i]) {
                    gCarCustomizeManager.PreviewPart(VinylColors[i]->GetSlotID(), VinylColors[i]->GetPart());
                }
            }
            gCarCustomizeManager.PreviewPart(GetSelectedPart()->GetSlotID(), GetSelectedPart()->GetPart());
            break;
        }
    }
    DisplayHelper.SetCareerStuff(GetSelectedPart(), GetCategory(), 0);
    DisplayHelper.SetPartStatus(GetSelectedPart(), GetUnlockBlurb(), ThePaints.GetCurrentDatumNum(), ThePaints.GetNumDatum());
}

CustomizeDecals::CustomizeDecals(ScreenConstructorData *sd)
    : CustomizationScreen(sd), //
      bIsBlack(true) {
    Setup();
}

void CustomizeDecals::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xc519bfbf:
            Showcase::FromFilter = bIsBlack;
            break;
        case 0x5073ef13:
        case 0xd9feec59:
            bIsBlack = !bIsBlack;
            {
                if (GetSelectedPart()->GetPart()) {
                    unsigned int nameHash = GetSelectedPart()->GetPart()->GetBrandNameHash();
                    BuildDecalList(nameHash);
                } else {
                    BuildDecalList(0);
                }
            }
            RefreshHeader();
            break;
        case 0x5a928018: {
            SelectablePart *sel = FindInCartPart();
            if (sel) {
                if (gCarCustomizeManager.IsPartInCart(sel)) {
                    return;
                }
                sel->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case 0x911ab364:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
            break;
        case 0xc519bfc3:
            return;
    }
}

uint32 CustomizeDecals::GetSlotIDFromCategory() {
    switch (CurrentDecalLocation) {
        case CC_DECAL_WINDSHIELD:
            return CARSLOTID_DECAL_FRONT_WINDOW_TEX0;
        case CC_DECAL_REAR_WINDOW:
            return CARSLOTID_DECAL_REAR_WINDOW_TEX0;
        case CC_DECAL_LEFT_DOOR:
            switch (Category) {
                case CC_DECAL_SLOT_1:
                    return CARSLOTID_DECAL_LEFT_DOOR_TEX0;
                case CC_DECAL_SLOT_2:
                    return CARSLOTID_DECAL_LEFT_DOOR_TEX1;
                case CC_DECAL_SLOT_3:
                    return CARSLOTID_DECAL_LEFT_DOOR_TEX2;
                case CC_DECAL_SLOT_4:
                    return CARSLOTID_DECAL_LEFT_DOOR_TEX3;
                case CC_DECAL_SLOT_5:
                    return CARSLOTID_DECAL_LEFT_DOOR_TEX4;
                case CC_DECAL_SLOT_6:
                    return CARSLOTID_DECAL_LEFT_DOOR_TEX5;
            }
            // fall through
        case CC_DECAL_RIGHT_DOOR:
            switch (Category) {
                case CC_DECAL_SLOT_1:
                    return CARSLOTID_DECAL_RIGHT_DOOR_TEX0;
                case CC_DECAL_SLOT_2:
                    return CARSLOTID_DECAL_RIGHT_DOOR_TEX1;
                case CC_DECAL_SLOT_3:
                    return CARSLOTID_DECAL_RIGHT_DOOR_TEX2;
                case CC_DECAL_SLOT_4:
                    return CARSLOTID_DECAL_RIGHT_DOOR_TEX3;
                case CC_DECAL_SLOT_5:
                    return CARSLOTID_DECAL_RIGHT_DOOR_TEX4;
                case CC_DECAL_SLOT_6:
                    return CARSLOTID_DECAL_RIGHT_DOOR_TEX5;
            }
        case CC_DECAL_LEFT_QP:
            return CARSLOTID_DECAL_LEFT_QUARTER_TEX0;
        case CC_DECAL_RIGHT_QP:
            return CARSLOTID_DECAL_RIGHT_QUARTER_TEX0;
        default:
            return CARSLOTID_DECAL_FRONT_WINDOW_TEX0;
    }
}

void CustomizeDecals::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    if (GetSelectedPart()->GetPart()) {
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", GetSelectedPart()->GetPart()->GetName());
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x5e7b09c9, Options.GetCurrentOption()->GetName());
    }
    FEngSetLanguageHash(GetPackageName(), 0x889bacb6, bIsBlack ? 0x41f0a3a5 : 0x436a98e9);
    if ((RealTimer - ScrollTime).GetSeconds() > 0.3f) {
        gCarCustomizeManager.PreviewPart(GetSlotIDFromCategory(), GetSelectedPart()->GetPart());
    } else {
        bNeedsRefresh = true;
    }
}

void CustomizeDecals::BuildDecalList(uint32 selected_name_hash) {
    ScrollTime = 0;
    bNeedsRefresh = true;
    Options.RemoveAll();
    Options.AddInitialBookEnds();

    uint32 slotID = GetSlotIDFromCategory();
    SelectablePart *stockPart = new ("SelectablePart", 0) SelectablePart(nullptr, slotID, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    eCustomizePartState stockState = CPS_AVAILABLE;
    if (gCarCustomizeManager.IsPartInstalled(stockPart)) {
        stockState = static_cast<eCustomizePartState>(0x11);
    } else if (gCarCustomizeManager.IsPartInCart(stockPart)) {
        stockState = static_cast<eCustomizePartState>(0x21);
    }
    stockPart->SetPartState(stockState);
    AddPartOption(stockPart, 0x697b4ad4, 0x60a662f5, 0, 0, false);

    bTList<SelectablePart> tempList;
    gCarCustomizeManager.GetCarPartList(slotID, tempList, 0);

    int unlockLevel = MapCarPartToUnlockable(slotID, nullptr);
    switch (unlockLevel) {
        case 0x2c:
            unlockLevel = 1;
            break;
        case 0x2e:
            unlockLevel = 2;
            break;
        case 0x30:
            unlockLevel = 3;
            break;
    }

    int curIdx = 2;
    int matchIdx = 1;
    while (tempList.GetHead() != tempList.EndOfList()) {
        unsigned int nameHash = tempList.GetHead()->GetPart()->GetBrandNameHash();
        if (!bIsBlack) {
            nameHash = bStringHash("_WHITE", nameHash);
        }
        unsigned int mirrorHash = tempList.GetHead()->GetPart()->GetAppliedAttributeUParam(bStringHash("NAME"), 0);
        if (nameHash == mirrorHash) {
            unsigned int unlockHash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), unlockLevel);
            SelectablePart *node = tempList.GetHead();
            node->Remove();
            AddPartOption(node, 0x697b4ad4, nameHash, 0, unlockHash, gCarCustomizeManager.IsPartLocked(node, 0));
            if (node->GetPart()->GetBrandNameHash() == selected_name_hash) {
                matchIdx = curIdx;
            }
            curIdx++;
        } else {
            SelectablePart *node = tempList.GetHead();
            node->Remove();
            delete node;
        }
    }
    if (Showcase::FromIndex != 0) {
        SetInitialOption(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(matchIdx);
    }
}

void CustomizeDecals::Setup() {
    uint32 slot_id = GetSlotIDFromCategory();
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x91c4a50), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x2d145be3), 0x682);
    uint32 title = 0;
    switch (Category) {
        case CC_DECAL_WINDSHIELD:
            title = 0x301dedd3;
            break;
        case CC_DECAL_REAR_WINDOW:
            title = 0x48e6ca49;
            break;
        case CC_DECAL_LEFT_QP:
            title = 0x8a7697d6;
            break;
        case CC_DECAL_RIGHT_QP:
            title = 0xb1f9b0c9;
            break;
        case CC_DECAL_SLOT_1:
            title = 0x7d212cfa;
            break;
        case CC_DECAL_SLOT_2:
            title = 0x7d212cfb;
            break;
        case CC_DECAL_SLOT_3:
            title = 0x7d212cfc;
            break;
        case CC_DECAL_SLOT_4:
            title = 0x7d212cfd;
            break;
        case CC_DECAL_SLOT_5:
            title = 0x7d212cfe;
            break;
        case CC_DECAL_SLOT_6:
            title = 0x7d212cff;
            break;
        default:
            break;
    }
    DisplayHelper.SetTitleHash(title);
    uint32 installed_part_base_name = 0;
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
    if (item && item->GetBuyingPart()->GetPart()) {
        CarPart *buying = item->GetBuyingPart()->GetPart();
        unsigned int name_hash = buying->GetBrandNameHash();
        bIsBlack = (name_hash == buying->GetAppliedAttributeUParam(bStringHash("NAME"), 0));
        installed_part_base_name = buying->GetBrandNameHash();
    } else {
        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(slot_id);
        if (installed) {
            unsigned int name_hash = installed->GetBrandNameHash();
            bIsBlack = (name_hash == installed->GetAppliedAttributeUParam(bStringHash("NAME"), 0));
            installed_part_base_name = installed->GetBrandNameHash();
        }
    }
    if (Showcase::FromFilter != -1) {
        bIsBlack = (Showcase::FromFilter != 0);
        Showcase::FromFilter = -1;
    }
    BuildDecalList(installed_part_base_name);
    RefreshHeader();
}

CustomizeNumbers::CustomizeNumbers(ScreenConstructorData *sd)
    : MenuScreen(sd),          //
      LeftNumberList(),        //
      RightNumberList(),       //
      TheLeftNumber(nullptr),  //
      TheRightNumber(nullptr), //
      LeftDisplayValue(-1),    //
      RightDisplayValue(-1),   //
      bLeft(1),                //
      DisplayHelper(sd->PackageFilename) {
    Category = sd->Arg & CC_TO_CAT_MASK;
    FromCategory = static_cast<int>(static_cast<short>(sd->Arg >> 16));
    Setup();
}

void CustomizeNumbers::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x35f8620b:
            DisplayHelper.SetInitComplete(true);
            FEngSetCurrentButton(GetPackageName(), 0x2a08ba92);
            break;
        case 0xc519bfbf:
            Showcase::FromFilter = RightDisplayValue;
            Showcase::FromIndex = LeftDisplayValue;
            Showcase::FromArgs = GetCategory() | (GetFromCategory() << 16);
            Showcase::FromPackage = GetPackageName();
            bShowcaseOn = 1;
            cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<uint32>(gCarCustomizeManager.GetTuningCar()), 0, false);
            break;
        case 0xb5af2461:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
        case 0x9120409e:
        case 0xb5971bf1:
            bLeft = !bLeft;
            FEngSetCurrentButton(GetPackageName(), bLeft ? 0x2a08ba92 : 0x1a88dc05);
            RefreshHeader();
            break;
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
                cFEng::Get()->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xff);
                return;
            }
            break;
        case 0xc519bfc3:
            if (gCarCustomizeManager.GetInstalledCarPart(0x71)) {
                UnsetShoppingCart();
                SelectablePart remover(nullptr, 0x71, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
                gCarCustomizeManager.AddToCart(&remover);
                remover.SetSlotID(0x72);
                gCarCustomizeManager.AddToCart(&remover);
                remover.SetSlotID(0x69);
                gCarCustomizeManager.AddToCart(&remover);
                remover.SetSlotID(0x6a);
                gCarCustomizeManager.AddToCart(&remover);
            } else if (TheLeftNumber->IsInCart() || TheRightNumber->IsInCart()) {
                UnsetShoppingCart();
                ShoppingCartItem *current = gCarCustomizeManager.GetFirstCartItem();
                ShoppingCartItem *last = gCarCustomizeManager.GetLastCartItem();
                while (current) {
                    ShoppingCartItem *next = static_cast<ShoppingCartItem *>(current->Next);
                    SelectablePart *part = current->GetBuyingPart();
                    if (part->GetSlotID() == 0x71 || part->GetSlotID() == 0x72 || part->GetSlotID() == 0x69 || part->GetSlotID() == 0x6a) {
                        gCarCustomizeManager.RemoveFromCart(current);
                    }
                    if (current == last)
                        break;
                    current = next;
                }
            }
            LeftDisplayValue = -1;
            RightDisplayValue = -1;
            TheLeftNumber = LeftNumberList.GetHead();
            TheRightNumber = RightNumberList.GetHead();
            RefreshHeader();
            break;
        case 0x91dfdf84:
            UnsetShoppingCart();
            TheLeftNumber->SetInCartPreserve();
            TheRightNumber->SetInCartPreserve();
            gCarCustomizeManager.AddToCart(TheLeftNumber);
            gCarCustomizeManager.AddToCart(TheRightNumber);
            {
                SelectablePart *copyLeft = new ("SelectablePart 6", 0) SelectablePart(TheLeftNumber);
                SelectablePart *copyRight = new ("SelectablePart 7", 0) SelectablePart(TheRightNumber);
                copyLeft->SetPrice(0);
                copyRight->SetPrice(0);
                copyLeft->SetInCart();
                copyRight->SetInCart();
                copyLeft->SetSlotID(0x69);
                copyRight->SetSlotID(0x6a);
                gCarCustomizeManager.AddToCart(copyLeft);
                gCarCustomizeManager.AddToCart(copyRight);
                delete copyLeft;
                delete copyRight;
            }
            RefreshHeader();
            break;
        case 0xcf91aacd: {
            SelectablePart *le_monde = LeftNumberList.GetHead();
            while (le_monde != LeftNumberList.EndOfList()) {
                if (le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(0x69) ||
                    le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(0x6a)) {
                    le_monde->SetInstalled();
                }
                le_monde = le_monde->GetNext();
            }
            le_monde = RightNumberList.GetHead();
            while (le_monde != RightNumberList.EndOfList()) {
                if (le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(0x71) ||
                    le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(0x72)) {
                    le_monde->SetInstalled();
                }
                le_monde = le_monde->GetNext();
            }
            CustomizeShoppingCart::ExitShoppingCart();
            break;
        }
        case 0x5a928018: {
            ShoppingCartItem *tex_6 = gCarCustomizeManager.IsPartTypeInCart(0x69u);
            ShoppingCartItem *tex_7 = gCarCustomizeManager.IsPartTypeInCart(0x6au);
            if (!tex_6 && !tex_7) {
                SelectablePart *le_monde = LeftNumberList.GetHead();
                while (le_monde != LeftNumberList.EndOfList()) {
                    if (le_monde->IsInCart()) {
                        le_monde->UnSetInCart();
                        break;
                    }
                    le_monde = le_monde->GetNext();
                }
                le_monde = RightNumberList.GetHead();
                while (le_monde != RightNumberList.EndOfList()) {
                    if (le_monde->IsInCart()) {
                        le_monde->UnSetInCart();
                        break;
                    }
                    le_monde = le_monde->GetNext();
                }
            }
            RefreshHeader();
            break;
        }
        case 0x911ab364:
            bShowcaseOn = 0;
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | (GetCategory() << 16), 0, false);
            break;
    }
}

void CustomizeNumbers::UnsetShoppingCart() {
    for (SelectablePart *number = LeftNumberList.GetHead(); number != LeftNumberList.EndOfList(); number = number->GetNext()) {
        if (number->IsInCart()) {
            number->UnSetInCartPreserve();
            break;
        }
    }
    for (SelectablePart *number = RightNumberList.GetHead(); number != RightNumberList.EndOfList(); number = number->GetNext()) {
        if (number->IsInCart()) {
            number->UnSetInCartPreserve();
            break;
        }
    }
}

void CustomizeNumbers::ScrollNumbers(eScrollDir dir) {
    if (LeftDisplayValue == -1 || RightDisplayValue == -1) {
        LeftDisplayValue = 0;
        RightDisplayValue = 0;
        TheLeftNumber = LeftNumberList.GetHead();
        TheRightNumber = RightNumberList.GetHead();
        RefreshHeader();
    } else {
        SelectablePart *new_part = bLeft ? TheLeftNumber : TheRightNumber;
        if (dir == eSD_PREV) {
            if (bLeft) {
                new_part = LeftNumberList.GetPrevCircular(new_part);
                if (--LeftDisplayValue < 0) {
                    LeftDisplayValue = 9;
                }
            } else {
                new_part = RightNumberList.GetPrevCircular(new_part);
                if (--RightDisplayValue < 0) {
                    RightDisplayValue = 9;
                }
            }
        } else if (dir == eSD_NEXT) {
            if (bLeft) {
                new_part = LeftNumberList.GetNextCircular(new_part);
                if (++LeftDisplayValue > 9) {
                    LeftDisplayValue = 0;
                }
            } else {
                new_part = RightNumberList.GetNextCircular(new_part);
                if (++RightDisplayValue > 9) {
                    RightDisplayValue = 0;
                }
            }
        }
        if (new_part != (bLeft ? TheLeftNumber : TheRightNumber)) {
            if (bLeft) {
                TheLeftNumber = new_part;
            } else {
                TheRightNumber = new_part;
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
        SelectablePart part(TheLeftNumber);
        uint32 status = CPS_AVAILABLE;
        if (TheLeftNumber->IsLocked() && TheRightNumber->IsLocked()) {
            status = CPS_LOCKED;
        } else if (TheLeftNumber->IsNew() && TheRightNumber->IsNew()) {
            status = CPS_NEW;
        }
        if (TheLeftNumber->IsInstalledX() && TheRightNumber->IsInstalledX()) {
            status = status | CPS_INSTALLED;
        } else if (TheLeftNumber->IsInCartX() && TheRightNumber->IsInCartX()) {
            status = status | CPS_IN_CART;
        }
        part.SetPartState(status);
        DisplayHelper.SetPartStatus(&part, gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), 1), 0, 0);
        FEPrintf(GetPackageName(), 0x2a08ba92, "%$d", static_cast<int>(LeftDisplayValue));
        FEPrintf(GetPackageName(), 0x1a88dc05, "%$d", static_cast<int>(RightDisplayValue));
        gCarCustomizeManager.PreviewPart(0x71, TheLeftNumber->GetPart());
        gCarCustomizeManager.PreviewPart(0x72, TheRightNumber->GetPart());
        gCarCustomizeManager.PreviewPart(0x69, TheLeftNumber->GetPart());
        gCarCustomizeManager.PreviewPart(0x6a, TheRightNumber->GetPart());
    } else {
        FEObject *numGroup = FEngFindObject(GetPackageName(), 0x7a8355d9);
        FEngSetInvisible(numGroup);
        ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(TheLeftNumber);
        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(0x71);
        if (!installed) {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_INSTALLED);
        } else if (inCart && !inCart->GetBuyingPart()->GetPart()) {
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
    DisplayHelper.SetTitleHash(0x6857e5ac);
    gCarCustomizeManager.GetCarPartList(0x71, LeftNumberList, 0);
    gCarCustomizeManager.GetCarPartList(0x72, RightNumberList, 0);
    CarPart *installed_part = gCarCustomizeManager.GetActivePartFromSlot(0x71);
    int i = 0;
    SelectablePart *test_part = LeftNumberList.GetHead();
    bool found = false;
    while (test_part != LeftNumberList.EndOfList()) {
        if (test_part->GetPart()->GetBrandNameHash() == bStringHash("NUMBER_LEFT")) {
            if (!found) {
                if (bShowcaseOn == 1 && Showcase::FromIndex == i) {
                    TheLeftNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part)) {
                        TheLeftNumber->SetInCartPreserve();
                    }
                    LeftDisplayValue = static_cast<short>(i);
                    Showcase::FromIndex = 0;
                    found = true;
                } else if (test_part->GetPart() == installed_part) {
                    TheLeftNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part)) {
                        TheLeftNumber->SetInCartPreserve();
                    }
                    LeftDisplayValue = static_cast<short>(i);
                }
            }
            i++;
            test_part = test_part->GetNext();
        } else {
            SelectablePart *to_kill = test_part;
            test_part = test_part->GetNext();
            delete LeftNumberList.Remove(to_kill);
        }
    }
    found = false;
    if (!TheLeftNumber) {
        LeftDisplayValue = -1;
        TheLeftNumber = LeftNumberList.GetHead();
    }

    i = 0;
    installed_part = gCarCustomizeManager.GetActivePartFromSlot(0x72);
    test_part = RightNumberList.GetHead();
    while (test_part != RightNumberList.EndOfList()) {
        if (test_part->GetPart()->GetBrandNameHash() == bStringHash("NUMBER_RIGHT")) {
            if (!found) {
                if (bShowcaseOn == 1 && Showcase::FromFilter == i) {
                    TheRightNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part)) {
                        TheRightNumber->SetInCartPreserve();
                    }
                    RightDisplayValue = static_cast<short>(i);
                    Showcase::FromFilter = -1;
                    found = true;
                } else if (test_part->GetPart() == installed_part) {
                    TheRightNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part)) {
                        TheRightNumber->SetInCartPreserve();
                    }
                    RightDisplayValue = static_cast<short>(i);
                }
            }
            i++;
            test_part = test_part->GetNext();
        } else {
            SelectablePart *to_kill = test_part;
            test_part = test_part->GetNext();
            delete RightNumberList.Remove(to_kill);
        }
    }
    if (!TheRightNumber) {
        RightDisplayValue = -1;
        TheRightNumber = RightNumberList.GetHead();
    }
    RefreshHeader();
}

CustomizePerformance::CustomizePerformance(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    Setup();
}

eMenuSoundTriggers CustomizePerformance::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
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

void CustomizePerformance::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case 0xE1FDE1D1:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
            break;
        case 0x5a928018: {
            SelectablePart *part = FindInCartPart();
            if ((part != nullptr) && (gCarCustomizeManager.IsPartInCart(part) == nullptr)) {
                part->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case __PAD_BACK__: {
            const u32 FEObj_leavescreen = 0x587c018b;
            cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            break;
        }
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
    switch (type) {
        case static_cast<Physics::Upgrades::Type>(0):
            return FEngHashString("PD_TIRES_%d_%d", level, num_packages);
        case static_cast<Physics::Upgrades::Type>(1):
            return FEngHashString("PD_BRAKES_%d_%d", level, num_packages);
        case static_cast<Physics::Upgrades::Type>(2):
            return FEngHashString("PD_CHASSIS_%d_%d", level, num_packages);
        case static_cast<Physics::Upgrades::Type>(3):
            return FEngHashString("PD_TRANSMISSION_%d_%d", level, num_packages);
        case static_cast<Physics::Upgrades::Type>(4):
            if (gCarCustomizeManager.IsCastrolCar() && level == 4 && num_packages == 3) {
                return FEngHashString("PD_ENGINE_%d_%d_CASTROL", 4, 3);
            }
            if (gCarCustomizeManager.IsRotaryCar() && (level == 2 || level == 4) && num_packages == 1) {
                return FEngHashString("PD_ENGINE_%d_%d_ROTARY", level, 1);
            }
            return FEngHashString("PD_ENGINE_%d_%d", level, num_packages);
        case static_cast<Physics::Upgrades::Type>(5):
            if (has_turbo) {
                return FEngHashString("PD_TURBO_%d_%d", level, num_packages);
            }
            return FEngHashString("PD_SUPERCHARGER_%d_%d", level, num_packages);
        case static_cast<Physics::Upgrades::Type>(6):
            return FEngHashString("PD_NITROUS_%d_%d", level, num_packages);
        default:
            return 0;
    }
}

uint32 CustomizePerformance::GetPerfPkgBrand(Physics::Upgrades::Type type, int level, int num_packages) {
    uint32 hash = 0;
    Attrib::Gen::frontend fe_attrib(gCarCustomizeManager.GetTuningCar()->FEKey, 0, nullptr);
    switch (type) {
        case Physics::Upgrades::PUT_TIRES:
            switch (level) {
                case 0:
                    hash = 0xad6a0504;
                    break;
                case 1:
                    hash = fe_attrib.p_tires_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_tires_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_tires_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_tires_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_BRAKES:
            switch (level) {
                case 0:
                    hash = 0xa1a5e9e5;
                    break;
                case 1:
                    hash = fe_attrib.p_brakes_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_brakes_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_brakes_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_brakes_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_CHASSIS:
            switch (level) {
                case 0:
                    hash = 0xad6a0504;
                    break;
                case 1:
                    hash = fe_attrib.p_suspension_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_suspension_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_suspension_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_suspension_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_TRANSMISSION:
            switch (level) {
                case 0:
                    hash = 0x98ed935e;
                    break;
                case 1:
                    hash = fe_attrib.p_transmission_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_transmission_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_transmission_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_transmission_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_ENGINE:
            if (gCarCustomizeManager.IsCastrolCar() && level == 4 && num_packages == 2) {
                return 0xb95d4df;
            }
            switch (level) {
                case 0:
                    hash = 0x7d0ac98f;
                    break;
                case 1:
                    hash = fe_attrib.p_engine_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_engine_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_engine_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_engine_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_INDUCTION:
            switch (level) {
                case 0:
                    hash = 0x9e8f71ad;
                    break;
                case 1:
                    hash = fe_attrib.p_induction_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_induction_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_induction_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_induction_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_NOS:
            switch (level) {
                case 0:
                    hash = 0x98ed935e;
                    break;
                case 1:
                    hash = fe_attrib.p_nitrous_1(num_packages);
                    break;
                case 2:
                    hash = fe_attrib.p_nitrous_2(num_packages);
                    break;
                case 3:
                    hash = fe_attrib.p_nitrous_3(num_packages);
                    break;
                case 4:
                    hash = fe_attrib.p_nitrous_4(num_packages);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return hash;
}

void CustomizePerformance::RefreshHeader() {
    int num_lines = 3;

    gCarCustomizeManager.PreviewPerfPkg(static_cast<Physics::Upgrades::Type>(GetSelectedPart()->GetPhysicsType()),
                                        GetSelectedPart()->GetUpgradeLevel());

    AccelSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, true));
    HandlingSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, true));
    TopSpeedSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, true));

    AccelSlider.Draw();
    HandlingSlider.Draw();
    TopSpeedSlider.Draw();

    int phys_type = GetSelectedPart()->GetPhysicsType();
    int level = GetSelectedPart()->GetUpgradeLevel();

    int desc_level = (gCarCustomizeManager.GetMaxPackages(static_cast<Physics::Upgrades::Type>(phys_type)) -
                      gCarCustomizeManager.GetNumPackages(static_cast<Physics::Upgrades::Type>(phys_type))) +
                     level;

    if (CustomizeIsInBackRoom() || level == 7) {
        level = 0;
        desc_level = 0;
        num_lines = 1;
    }

    int i = 0;
    while (i < num_lines) {
        unsigned int hash_desc =
            GetPerfPkgDesc(static_cast<Physics::Upgrades::Type>(phys_type), desc_level, i + 1, gCarCustomizeManager.IsTurbo());
        if (DoesStringExist(hash_desc)) {
            FEngSetVisible(DescLines[i]);
            FEngSetVisible(DescBullets[i]);
            FEngSetLanguageHash(GetPackageName(), DescLines[i]->NameHash, hash_desc);
        } else {
            FEngSetInvisible(DescLines[i]);
            FEngSetInvisible(DescBullets[i]);
        }

        Attrib::Gen::frontend fe_attrib(gCarCustomizeManager.GetTuningCar()->FEKey, 0, nullptr);

        unsigned int tex_hash = GetPerfPkgBrand(static_cast<Physics::Upgrades::Type>(phys_type), desc_level, i);
        unsigned int feimage_hash = FEngHashString("BRAND_ICON_%d", i + 1);

        if (GetTextureInfo(tex_hash, 0, 0)) {
            FEngSetVisible(GetPackageName(), feimage_hash);
            FEngSetTextureHash(GetPackageName(), feimage_hash, tex_hash);
        } else {
            FEngSetInvisible(GetPackageName(), feimage_hash);
        }

        i = i + 1;
    }

    while (i < 3) {
        FEngSetInvisible(DescLines[i]);
        FEngSetInvisible(DescBullets[i]);
        unsigned int feimage_hash = FEngHashString("BRAND_ICON_%d", i + 1);
        FEngSetInvisible(GetPackageName(), feimage_hash);
        i = i + 1;
    }

    CustomizationScreen::RefreshHeader();

    unsigned int level_hash;
    if (GetSelectedPart()->GetUpgradeLevel() == 7) {
        level_hash = 0xedd14807;
    } else {
        int num = gCarCustomizeManager.GetNumPackages(static_cast<Physics::Upgrades::Type>(phys_type));
        int level6 = level + 6;
        level = level6 - num;
        level_hash = FEngHashString("PN_LEVEL_%d", level);
    }
    FEngSetLanguageHash(pOptionName, level_hash);
}

void CustomizePerformance::Setup() {
    if (!gCarCustomizeManager.IsCareerMode()) {
        const u32 FEObj_QUICKRACE = 0xde511657;
        cFEng::Get()->QueuePackageMessage(FEObj_QUICKRACE, GetPackageName(), nullptr);
    }

    for (int i = 0; i < 3; i++) {
        DescLines[i] = FEngFindString(GetPackageName(), FEngHashString("DETAIL_TEXT_LINE%d", i + 1));
        DescBullets[i] = FEngFindImage(GetPackageName(), FEngHashString("PERFORMANCE_DETAILS_ICON%d", i + 1));
    }

    AccelSlider.Init(GetPackageName(), "ACCELERATION", 0.0f, 1.0f, 0.0f, gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, true),
                     gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, false), 160.0f);
    HandlingSlider.Init(GetPackageName(), "HANDLING", 0.0f, 1.0f, 0.0f, gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, true),
                        gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, false), 160.0f);
    TopSpeedSlider.Init(GetPackageName(), "TOPSPEED", 0.0f, 1.0f, 0.0f, gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, true),
                        gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, false), 160.0f);

    Physics::Upgrades::Type type = Physics::Upgrades::PUT_ENGINE;
    switch (Category) {
        case CC_ENGINE:
            SetTitleHash(0x9853d9a6);
            break;
        case CC_TRANSMISSION:
            type = Physics::Upgrades::PUT_TRANSMISSION;
            SetTitleHash(0x29aa74ba);
            break;
        case CC_SUSPENSION:
            type = Physics::Upgrades::PUT_CHASSIS;
            SetTitleHash(0x6e101aa7);
            break;
        case CC_NITROUS:
            type = Physics::Upgrades::PUT_NOS;
            SetTitleHash(0x4ce19aa4);
            break;
        case CC_TIRES:
            type = Physics::Upgrades::PUT_TIRES;
            SetTitleHash(0x5aa9137);
            break;
        case CC_BRAKES:
            type = Physics::Upgrades::PUT_BRAKES;
            SetTitleHash(0x91997ee8);
            break;
        case CC_FORCED_INDUCTION:
            type = Physics::Upgrades::PUT_INDUCTION;
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
        SelectablePart *new_part = new ("SelectablePart 8", 0) SelectablePart(nullptr, 0, 7, type, true, CPS_AVAILABLE, 0, true);
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
    part = part_list.GetHead();
    for (j = 1;; j++) {
        if (part_list.IsEmpty()) {
            break;
        }
        part = part_list.RemoveHead();
        int true_unlock_level = gCarCustomizeManager.GetMaxPackages(type) - gCarCustomizeManager.GetNumPackages(type) + part->GetUpgradeLevel();
        unsigned int unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(Category), true_unlock_level);
        is_locked = gCarCustomizeManager.IsPartLocked(part, 0);
        AddPartOption(part, icon_hash, j, desc_hash, unlock_hash, is_locked);
    }

    if (((FEDatabase->GetCareerSettings()->HasBeenAwardedBKReward() && !FEDatabase->IsCareerMode()) ||
         (FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce && !gCarCustomizeManager.IsHeroCar())) &&
        gCarCustomizeManager.CanInstallJunkman(type)) {
        SelectablePart *new_part = new ("SelectablePart 8", 0) SelectablePart(nullptr, 0, 7, type, true, CPS_AVAILABLE, 0, true);
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
