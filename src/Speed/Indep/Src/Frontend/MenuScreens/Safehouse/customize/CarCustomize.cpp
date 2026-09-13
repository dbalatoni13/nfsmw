#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/EAXSound/AemsDef.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
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
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "types.h"

extern const float gTradeInFactor = 0.25f; // size: 0x4

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
    switch (cat) {
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
            int num = GetNumMarkersFromCategory(CC_BODY_KIT);
            num += GetNumMarkersFromCategory(CC_SPOILERS);
            num += GetNumMarkersFromCategory(CC_RIM_BRANDS);
            num += GetNumMarkersFromCategory(CC_HOODS);
            num += GetNumMarkersFromCategory(CC_ROOF_SCOOPS);
            return num + GetNumMarkersFromCategory(CC_CUSTOM_HUD);
        }
        case CC_PERFORMANCE: {
            int num = GetNumMarkersFromCategory(CC_ENGINE);
            num += GetNumMarkersFromCategory(CC_TRANSMISSION);
            num += GetNumMarkersFromCategory(CC_SUSPENSION);
            num += GetNumMarkersFromCategory(CC_NITROUS);
            num += GetNumMarkersFromCategory(CC_TIRES);
            num += GetNumMarkersFromCategory(CC_BRAKES);
            return num + GetNumMarkersFromCategory(CC_FORCED_INDUCTION);
        }
        case CC_VISUAL: {
            int num = GetNumMarkersFromCategory(CC_PAINT);
            num += GetNumMarkersFromCategory(CC_VINYL_TYPES);
            return num + GetNumMarkersFromCategory(CC_DECAL_LOCATION);
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
    for (int i = 0; i < NUM_STAGE_SEGMENTS; i++) {
        pBases[i] = nullptr;
    }
}

void CustomizeMeter::Init(const char *pkg_name, const char *name, float min, float max, float current, float preview) {
    Min = min;
    Max = max;
    SetCurrent(current);
    SetPreview(preview);
    const u32 FEObj_HeatX = 0x5ffee1d8;
    const u32 FEObj_HeatXZOOM = 0xe637955c;
    pMultiplier = FEngFindImage(pkg_name, FEObj_HeatX);
    pMultiplierZoom = FEngFindImage(pkg_name, FEObj_HeatXZOOM);
    pMeterGroup = FEngFindObject(pkg_name, 0xf2492598);
    for (int i = 0; i < NUM_STAGE_SEGMENTS; i++) {
        pBases[i] = FEngFindImage(pkg_name, FEngHashString("HEAT_BASE_LED_%d", i + 1));
    }
}

void CustomizeMeter::SetCurrent(float current) {
    Current = bMin(bMax(current, Min), Max);
}

void CustomizeMeter::SetPreview(float preview) {
    PreviousPreview = Preview;
    Preview = bMin(bMax(preview, Min), Max);
}

// UNSOLVED
void CustomizeMeter::Draw() {
    float stage_size = 1.0f;
    float multiplier = stage_size;
    float stage_bottom = Min;
    while (Preview - stage_bottom >= stage_size) {
        multiplier += stage_size;
        stage_bottom += stage_size;
    }
    multiplier = bMin(multiplier, 5.0f);
    // TODO: no need to double hash
    FEngSetTextureHash(pMultiplier, FEngHashString("HEAT_X%.0f", multiplier));
    FEngSetTextureHash(pMultiplierZoom, FEngHashString("HEAT_X%.0f", multiplier));

    const u32 FEObj_ZOOM = 0x209c24;
    if (Preview != PreviousPreview) {
        FEngSetScript(pMultiplierZoom, FEObj_ZOOM, true);
    }

    float segment_size = stage_size * 0.1f;
    float segment_bottom = stage_bottom;
    int cur_icon = 10;
    float current_segment = segment_bottom + stage_size;
    if (current_segment >= stage_bottom) {
        do {
            current_segment -= segment_size;
            cur_icon--;
            uint32 script;
            if (current_segment + 0.0005f >= this->Current) {
                const u32 FEObj_OFF = 0xccfa;
                script = FEObj_OFF;
            } else if (current_segment + 0.0005f >= this->Preview) {
                const u32 FEObj_COOL = 0x13ff6c;
                script = 0x13ff6c;
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
    if ((TheItem->GetTradeInPart() != nullptr) && gCarCustomizeManager.IsCareerMode() && !gCarCustomizeManager.IsInBackRoom()) {
        FEPrintf(pTradeInPrice, "%$d", TheItem->GetTradeInPrice());
    } else {
        FEPrintf(pTradeInPrice, "");
    }
    if (gCarCustomizeManager.IsCareerMode() && !gCarCustomizeManager.IsInBackRoom()) {
        FEPrintf(GetDataObject(), "%$d", TheItem->GetPartPrice());
    } else {
        FEPrintf(GetDataObject(), "");
    }
}

void FEShoppingCartItem::Position() {
    FEngSetTopLeft(pCheckIcon, GetTopLeftX(), GetTopLeftY() - 10.0f);
    FEngSetTopLeft(GetTitleObject(), GetTopLeftX() + 30.0f, GetTopLeftY());
    FEngSetTopLeftY(pTradeInPrice, GetTopLeftY());
    FEngSetBottomRightX(pTradeInPrice, GetDataPosX() + 150.0f);
    FEngSetTopLeftY(GetDataObject(), GetTopLeftY());
    FEngSetBottomRightX(GetDataObject(), GetDataPosX() + 40.0f);
    if (GetBacking() != nullptr) {
        FEngSetTopLeft(GetBacking(), GetTopLeftX() - GetBackingOffsetX(), GetTopLeftY() - GetBackingOffsetY());
    }
}

void FEShoppingCartItem::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject());
    FEngSetScript(GetTitleObject(), FEHASH_HIGHLIGHT, true);
    FEngSetScript(GetDataObject(), FEHASH_HIGHLIGHT, true);
    FEngSetScript(pTradeInPrice, FEHASH_HIGHLIGHT, true);
    if (GetBacking() != nullptr) {
        FEngSetVisible(GetBacking());
        FEngSetScript(GetBacking(), FEHASH_HIGHLIGHT, true);
    }
}

void FEShoppingCartItem::UnsetFocus() {
    uint32 script = FEHASH_UNHIGHLIGHT;
    const uint32 FEObj_GREY = FEHASH_GREY;
    if (!TheItem->IsActive()) {
        script = FEObj_GREY;
    }
    FEngSetScript(GetTitleObject(), script, true);
    FEngSetScript(GetDataObject(), script, true);
    FEngSetScript(pTradeInPrice, script, true);
    if (GetBacking() != nullptr) {
        FEngSetInvisible(GetBacking());
        FEngSetScript(GetBacking(), FEHASH_UNHIGHLIGHT, true);
    }
}

void FEShoppingCartItem::SetCheckScripts() {
    if (TheItem->IsActive()) {
        const uint32 FEObj_CHECKED = FEHASH_CHECKED;
        FEngSetScript(pCheckIcon, FEObj_CHECKED, true);
    } else {
        const uint32 FEObj_UNCHECKED = FEHASH_UNCHECKED;
        FEngSetScript(pCheckIcon, FEObj_UNCHECKED, true);
    }
}

void FEShoppingCartItem::SetActiveScripts() {
    if (!TheItem->IsActive()) {
        const uint32 FEObj_GREY = FEHASH_GREY;
        FEngSetScript(pCheckIcon, FEObj_GREY, true);
    }
}

void FEShoppingCartItem::DrawPartName() {
    if (TheItem->GetBuyingPart()->IsPerformancePkg()) {
        Physics::Upgrades::Type phys_type = TheItem->GetBuyingPart()->GetPhysicsType();
        if (TheItem->GetBuyingPart()->GetUpgradeLevel() == PPL_LEVEL_JUNKMAN) {
            if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)), GetLocalizedString(0xedd14807));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)), GetLocalizedString(0xedd14807));
            }
        } else {
            int level = TheItem->GetBuyingPart()->GetUpgradeLevel() - (gCarCustomizeManager.GetNumPackages(phys_type) - 6);
            if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)),
                         GetLocalizedString(GetPerfPkgLevelHash(level)));
            } else {
                FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetPerfPkgCatHash(phys_type)),
                         GetLocalizedString(GetPerfPkgLevelHash(level)));
            }
        }
    } else {
        SelectablePart *part = TheItem->GetBuyingPart();
        switch (part->GetSlotID()) {
            case CARSLOTID_PAINT_RIM:
                if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                    FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0xb3100a3e),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0xb3100a3e),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
                }
                break;

            case CARSLOTID_BASE_PAINT: {
                uint32 paint_type = 0x452b5481;
                switch (part->GetPart()->GetBrandNameHash()) {
                    case 0x2daab07:
                        paint_type = 0xb6763cde;
                        break;
                    case 0x3797533:
                        paint_type = 0xb715070a;
                        break;
                    case 0xda27:
                        paint_type = 0xb3100a3e;
                        break;
                    case 0x3437a52:
                        break;
                }
                if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                    FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(paint_type),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
                } else {
                    FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(paint_type),
                             GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("SPEECHCOLOUR"), 0)));
                }
                break;
            }

            case CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER: {
                ShoppingCartItem *left_num_item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER);
                ShoppingCartItem *right_num_item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER);
                if ((left_num_item != nullptr) && (right_num_item != nullptr)) {
                    CarPart *left_part = left_num_item->GetBuyingPart()->GetPart();
                    CarPart *right_part = right_num_item->GetBuyingPart()->GetPart();
                    if ((left_part != nullptr) && (right_part != nullptr)) {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s%s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), left_part->GetName(),
                                     right_part->GetName());
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s%s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), left_part->GetName(),
                                     right_part->GetName());
                        }
                    } else {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     GetLocalizedString(0xbe434a38));
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     GetLocalizedString(0xbe434a38));
                        }
                    }
                }
                break;
            }

            case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
            case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
            case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
            case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
            case CARSLOTID_DECAL_LEFT_QUARTER_TEX0:
            case CARSLOTID_DECAL_RIGHT_QUARTER_TEX0:
                if (part->GetPart() == nullptr) {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s - %s", GetLocalizedString(0x955980bc),
                                 GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0x7177dc17));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s - %s", GetLocalizedString(0x955980bc),
                                 GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(0x7177dc17));
                    }

                } else {
                    uint32 name_hash = part->GetPart()->GetBrandNameHash();
                    bool black = part->GetPart()->GetAppliedAttributeUParam(bStringHash("NAME"), 0) != 0;
                    uint32 slot_hash = 0;
                    switch (part->GetSlotID()) {
                        case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
                        case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
                            slot_hash = 0x7d212cfa;
                            break;
                        case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
                        case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
                            slot_hash = 0x7d212cfb;
                            break;
                        case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
                        case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
                            slot_hash = 0x7d212cfc;
                            break;
                        case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
                        case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
                            slot_hash = 0x7d212cfd;
                            break;
                        case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
                        case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
                            slot_hash = 0x7d212cfe;
                            break;
                        case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
                        case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
                            slot_hash = 0x7d212cff;
                            break;
                    }
                    if (slot_hash != 0) {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s %s %s", GetLocalizedString(0x955980bc),
                                     GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(slot_hash),
                                     part->GetPart()->GetName());
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s %s %s", GetLocalizedString(0x955980bc),
                                     GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), GetLocalizedString(slot_hash),
                                     part->GetPart()->GetName());
                        }
                    } else {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(0x955980bc),
                                     GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(0x955980bc),
                                     GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                        }
                    }
                }
                break;

            case CARSLOTID_BODY:
                if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    }
                } else {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                    }
                }
                break;

            case CARSLOTID_FRONT_WHEEL:
                if (part->GetPart() != gCarCustomizeManager.GetStockCarPart(CARSLOTID_FRONT_WHEEL)) {
                    char sztemp[64];
                    bSNPrintf(sztemp, sizeof(sztemp), "%s", part->GetPart()->GetName());
                    int len = bStrLen(sztemp);
                    if (len > 0) {
                        for (int i = len; i >= len - 6; i--) {
                            sztemp[i] = 0;
                        }
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s %$d\"", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), sztemp,
                                     static_cast<signed char>(part->GetPart()->GetInnerRadius()));
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s %$d\"", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), sztemp,
                                     static_cast<signed char>(part->GetPart()->GetInnerRadius()));
                        }
                    }
                } else {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(0x60a662f5));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(0x60a662f5));
                    }
                }
                break;

            case CARSLOTID_SPOILER:
            case CARSLOTID_ROOF:
            case CARSLOTID_HOOD:
                if (part->GetPart()->HasAppliedAttribute(bStringHash("CARBONFIBRE"))) {
                    if (part->GetPart()->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0) != 0) {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     GetLocalizedString(0x5415b874),
                                     GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     GetLocalizedString(0x5415b874),
                                     GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                        }
                    }
                } else if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    }
                } else {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                    }
                }
                break;

            case CARSLOTID_VINYL_LAYER0:
                if (part->GetPart() != nullptr) {
                    if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                        }
                    } else {
                        if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                            FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     part->GetPart()->GetName());
                        } else {
                            FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                     part->GetPart()->GetName());
                        }
                    }
                } else {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(0x60a662f5));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(0x60a662f5));
                    }
                }
                break;

            default:
                if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())),
                                 GetLocalizedString(part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0)));
                    }

                } else {
                    if (GetCurrentLanguage() == eLANGUAGE_FRENCH) {
                        FEPrintf(GetTitleObject(), "%s : %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                    } else {
                        FEPrintf(GetTitleObject(), "%s: %s", GetLocalizedString(GetCarPartCatHash(part->GetSlotID())), part->GetPart()->GetName());
                    }
                }
                break;
        }
    }
}

uint32 FEShoppingCartItem::GetPerfPkgCatHash(Physics::Upgrades::Type phys_type) {
    uint32 type_hash = 0;
    switch (phys_type) {
        case Physics::Upgrades::PUT_TIRES:
            type_hash = 0x5aa9137;
            break;
        case Physics::Upgrades::PUT_BRAKES:
            type_hash = 0x91997ee8;
            break;
        case Physics::Upgrades::PUT_CHASSIS:
            type_hash = 0x6e101aa7;
            break;
        case Physics::Upgrades::PUT_TRANSMISSION:
            type_hash = 0x29aa74ba;
            break;
        case Physics::Upgrades::PUT_ENGINE:
            type_hash = 0x9853d9a6;
            break;
        case Physics::Upgrades::PUT_INDUCTION:
            if (gCarCustomizeManager.IsTurbo()) {
                type_hash = 0x5b1255c;
            } else {
                type_hash = 0xbb6812bb;
            }
            break;
        case Physics::Upgrades::PUT_NOS:
            type_hash = 0x4ce19aa4;
            break;
        default:
            break;
    }
    return type_hash;
}

uint32 FEShoppingCartItem::GetPerfPkgLevelHash(int level) {
    uint32 pkg_hash = 0x69c270c3;
    switch (level) {
        case PPL_LEVEL_1:
            pkg_hash = 0x69c270c4;
            break;
        case PPL_LEVEL_2:
            pkg_hash = 0x69c270c5;
            break;
        case PPL_LEVEL_3:
            pkg_hash = 0x69c270c6;
            break;
        case PPL_LEVEL_4:
            pkg_hash = 0x69c270c7;
            break;
        case PPL_LEVEL_5:
            pkg_hash = 0x69c270c8;
            break;
        case PPL_LEVEL_6:
            pkg_hash = 0x69c270c9;
            break;
    }
    return pkg_hash;
}

uint32 FEShoppingCartItem::GetCarPartCatHash(uint32 slot_id) {
    switch (slot_id) {
        case CARSLOTID_BODY:
            return 0x6134c218;
        case CARSLOTID_SPOILER:
            return 0x94e73021;
        case CARSLOTID_ROOF:
            return 0x61e8f83c;
        case CARSLOTID_HOOD:
            return 0x4d4a88d;
        case CARSLOTID_FRONT_WHEEL:
            return 0xf868eb0b;
        case CARSLOTID_BASE_PAINT:
            return 0x55da70c;
        case CARSLOTID_CUSTOM_HUD:
            return 0x78980a6b;
        case CARSLOTID_WINDOW_TINT:
            return 0xd32729a6;
        case CARSLOTID_PAINT_RIM:
            return 0xe126ff53;
        case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
            return 0x301dedd3;
        case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
            return 0x48e6ca49;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
            return 0x34367c86;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
            return 0xddf80259;
        case CARSLOTID_DECAL_LEFT_QUARTER_TEX0:
            return 0x8a7697d6;
        case CARSLOTID_DECAL_RIGHT_QUARTER_TEX0:
            return 0xb1f9b0c9;
        case CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER:
            return 0x6857e5ac;
        case CARSLOTID_VINYL_LAYER0:
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
        case __PAD_BUTTON4__:
            ToggleChecked();
            RefreshHeader();
            break;
        case __PAD_ACCEPT__:
            if (!gCarCustomizeManager.DoesCartHaveActiveParts()) {
                gCarCustomizeManager.EmptyCart();
                gCarCustomizeManager.ResetPreview(); // TODO: not needed, EmptyCart already resets preview
                cFEng::Get()->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
                cFEng::Get()->QueuePackagePop(1);
            } else {
                if (CanCheckout()) {
                    uint32 blurb_hash = 0x71d9e710;
                    if (gCarCustomizeManager.IsCareerMode()) {
                        blurb_hash = 0x8ebaa44b;
                        if (gCarCustomizeManager.IsInBackRoom()) {
                            blurb_hash = 0x4810898;
                        }
                    }
                    DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, dialog_message_yes,
                                                    dialog_message_no, dialog_message_no, first_dialog_button2, blurb_hash);
                } else {
                    DialogInterface::ShowOk(GetPackageName(), "", dialog_alert, 0xa984a42);
                }
            }
            break;
        case dialog_message_yes:
            gCarCustomizeManager.Checkout();
            cFEng::Get()->QueueGameMessage(0xcf91aacd, pParentPkg, 0xFF);
            cFEng::Get()->QueuePackagePop(1);
            break;
        case __PAD_BUTTON5__:
            UncheckAllItems();
            RefreshHeader();
            break;
        case __PAD_UP__:
        case __PAD_DOWN__:
            RefreshHeader();
            break;
        case __PAD_BACK__:
            ClearUncheckedItems();
            cFEng::Get()->QueueGameMessage(0x5a928018, pParentPkg, 0xFF);
            cFEng::Get()->QueuePackagePop(1);
            break;
    }
}

void CustomizeShoppingCart::ShowShoppingCart(const char *parent_pkg) {
    pParentPkg = parent_pkg;
    cFEng::Get()->QueuePackagePush(g_pCustomizeShoppingCartPkg, 0, 0, false);
}

void CustomizeShoppingCart::ExitShoppingCart() {
    if (gCarCustomizeManager.IsInBackRoom()) {
        gCarCustomizeManager.SetInBackRoom(false);
        FEManager::Get()->SetGarageType(GARAGETYPE_CUSTOMIZATION_SHOP);
    }
    cFEng::Get()->QueuePackageSwitch(g_pCustomizeMainPkg, 0, 0, false);
}

bool CustomizeShoppingCart::IsSlotIDNumberDecal(int slotID) {
    if (slotID == CARSLOTID_DECAL_RIGHT_DOOR_TEX6 || slotID == CARSLOTID_DECAL_RIGHT_DOOR_TEX7 || slotID == CARSLOTID_DECAL_LEFT_DOOR_TEX6 ||
        slotID == CARSLOTID_DECAL_LEFT_DOOR_TEX7) {
        return true;
    }
    return false;
}

void CustomizeShoppingCart::ToggleAllNumberDecals() {
    int numOptions = gCarCustomizeManager.GetNumCartItems();
    for (int i = 0; i < numOptions; i++) {
        ShoppingCartItem *si = gCarCustomizeManager.GetCartItem(i);
        if (si != nullptr) {
            SelectablePart *sp = si->GetBuyingPart();
            if (sp != nullptr) {
                int slotID = sp->GetSlotID();
                if (IsSlotIDNumberDecal(slotID)) {
                    si->ToggleActive();
                }
            }
        }
    }
}

void CustomizeShoppingCart::ToggleChecked() {
    if (GetCurrentItem() != nullptr) {
        ShoppingCartItem *item = GetCurrentItem()->GetItem();
        if (item != nullptr) {
            item->ToggleActive();
            SelectablePart *spart = item->GetBuyingPart();
            if (spart != nullptr) {
                int slotID = spart->GetSlotID();
                if (IsSlotIDNumberDecal(slotID)) {
                    ToggleAllNumberDecals();
                    item->ToggleActive();
                }
            }
        }
        GetCurrentItem()->SetCheckScripts();
        GetCurrentItem()->Draw();
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
    const u32 FEObj_GREY = FEHASH_GREY;
    const u32 FEObj_NORMAL = FEHASH_NORMAL;

    FEngSetScript(GetPackageName(), FEngHashString("MARKER_GROUP_%d", num), num_markers == 0 ? FEObj_GREY : FEObj_NORMAL, true);

    FEPrintf(GetPackageName(), FEngHashString("MARKER_NUM_%d", num), "%$d", num_markers);

    FEPrintf(GetPackageName(), FEngHashString("MARKER_BLOOM_%d", num), "%$d", num_markers);
}

int CustomizeShoppingCart::GetNumMarkersSpending(uint32 slot_id) {
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
    return static_cast<int>(item != nullptr && item->IsActive());
}

// UNSOLVED
void CustomizeShoppingCart::SetMarkerAmounts() {
    const u32 FEObj_GREY = FEHASH_GREY;
    const u32 FEObj_NORMAL = FEHASH_NORMAL;
    if (gCarCustomizeManager.IsInPerformance()) {
        static int phys_type[Physics::Upgrades::PUT_MAX] = {
            Physics::Upgrades::PUT_BRAKES,  Physics::Upgrades::PUT_ENGINE, Physics::Upgrades::PUT_NOS,          Physics::Upgrades::PUT_INDUCTION,
            Physics::Upgrades::PUT_CHASSIS, Physics::Upgrades::PUT_TIRES,  Physics::Upgrades::PUT_TRANSMISSION,
        };
        static int markers[Physics::Upgrades::PUT_MAX] = {
            FEMarkerManager::MARKER_BRAKES,  FEMarkerManager::MARKER_ENGINE, FEMarkerManager::MARKER_NOS,          FEMarkerManager::MARKER_INDUCTION,
            FEMarkerManager::MARKER_CHASSIS, FEMarkerManager::MARKER_TIRES,  FEMarkerManager::MARKER_TRANSMISSION,
        };
        int num_thingies = Physics::Upgrades::PUT_MAX;
        for (int i = 0; i < num_thingies; i++) {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(phys_type[i]);
            int num = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(markers[i]), 0);
            if (item != nullptr && item->IsActive()) {
                num--;
            }
            SetMarkerData(i + 1, item, num);
        }
    } else if (gCarCustomizeManager.IsInParts()) {
        static int slot_id[5] = {CARSLOTID_BODY, CARSLOTID_HOOD, CARSLOTID_SPOILER, CARSLOTID_FRONT_WHEEL, CARSLOTID_ROOF};
        static int markers[5] = {
            FEMarkerManager::MARKER_BODY, FEMarkerManager::MARKER_HOOD,       FEMarkerManager::MARKER_SPOILER,
            FEMarkerManager::MARKER_RIMS, FEMarkerManager::MARKER_ROOF_SCOOP,
        };
        int num_thingies = 5;
        for (int i = 0; i < num_thingies; i++) {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id[i]);
            int num = TheFEMarkerManager.GetNumMarkers(static_cast<FEMarkerManager::ePossibleMarker>(markers[i]), 0);
            if (item != nullptr && item->IsActive()) {
                num--;
            }
            SetMarkerData(i + 1, item, num);
        }
        FEngSetInvisible(GetPackageName(), 0x47df0e22);
        FEngSetInvisible(GetPackageName(), 0x47df0e23);
    } else {
        ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_VINYL_LAYER0);
        int num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_VINYL, 0);
        if (item != nullptr && item->IsActive()) {
            num--;
        }
        SetMarkerData(1, item, num);

        num = GetNumMarkersSpending(CARSLOTID_DECAL_FRONT_WINDOW_TEX0);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_REAR_WINDOW_TEX0);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_DOOR_TEX0);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_DOOR_TEX1);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_DOOR_TEX2);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_DOOR_TEX3);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_DOOR_TEX4);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_DOOR_TEX5);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_DOOR_TEX0);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_DOOR_TEX1);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_DOOR_TEX2);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_DOOR_TEX3);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_DOOR_TEX4);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_DOOR_TEX5);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_LEFT_QUARTER_TEX0);
        num += GetNumMarkersSpending(CARSLOTID_DECAL_RIGHT_QUARTER_TEX0);
        num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_DECAL, 0) - num;
        SetMarkerData(2, item, num);

        item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_BASE_PAINT);
        num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_PAINT, 0);
        if (item != nullptr && item->IsActive()) {
            num--;
        }
        SetMarkerData(3, item, num);

        item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_CUSTOM_HUD);
        num = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_CUSTOM_HUD, 0);
        if (item != nullptr && item->IsActive()) {
            num--;
        }
        SetMarkerData(4, item, num);

        FEngSetInvisible(GetPackageName(), 0x47df0e21);
        FEngSetInvisible(GetPackageName(), 0x47df0e22);
        FEngSetInvisible(GetPackageName(), 0x47df0e23);
    }
}

void CustomizeShoppingCart::RefreshHeader() {
    if (this->GetCurrentItem() != nullptr) {
        FEngSetVisible(GetPackageName(), 0x842b0e89);
        uint32 check_text = 0xd57c95e1;
        if (this->GetCurrentItem()->GetItem()->IsActive()) {
            FEngSetLanguageHash(GetPackageName(), check_text, 0x5dabcbc0);
        } else {
            FEngSetLanguageHash(GetPackageName(), check_text, 0x28feadd);
        }
    } else {
        FEngSetInvisible(GetPackageName(), 0x842b0e89);
    }
    HeatMeter.SetCurrent(gCarCustomizeManager.GetActualHeat());
    HeatMeter.SetPreview(gCarCustomizeManager.GetCartHeat());
    HeatMeter.Draw();
    if (gCarCustomizeManager.IsCareerMode()) {
        if (gCarCustomizeManager.IsInBackRoom()) {
            SetMarkerAmounts();
            int num_markers;
            if (gCarCustomizeManager.IsInParts()) {
                FEngSetLanguageHash(GetPackageName(), 0x8cdcb8ed, 0xa03a752f);
                FEngSetLanguageHash(GetPackageName(), 0xd3d3b1f4, 0x4ac68298);
                num_markers = TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_BODY, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_HOOD, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_SPOILER, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_RIMS, 0);
                num_markers += TheFEMarkerManager.GetNumMarkers(FEMarkerManager::MARKER_ROOF_SCOOP, 0);
            } else if (gCarCustomizeManager.IsInPerformance()) {
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
            FEPrintf(GetPackageName(), 0xd1497a06, "%$d", gCarCustomizeManager.GetCartTotal(CCT_PART_PRICES));
            FEPrintf(GetPackageName(), 0x18661565, "%$d", num_markers - gCarCustomizeManager.GetCartTotal(CCT_PART_PRICES));
        } else {
            FEPrintf(GetPackageName(), 0xd1497a06, "%$d", gCarCustomizeManager.GetCartTotal(CCT_PART_PRICES));
            FEPrintf(GetPackageName(), 0x34f7c0e8, "%$d", gCarCustomizeManager.GetCartTotal(CCT_TRADE_IN));
            int total = gCarCustomizeManager.GetCartTotal(CCT_TOTAL);
            FEPrintf(GetPackageName(), 0x18661565, "%$d", total);
            int remaining = FEDatabase->GetCareerSettings()->GetCash() - total;
            FEPrintf(GetPackageName(), 0x8531e22e, "%$d", remaining);
        }
    } else {
        FEngSetInvisible(GetPackageName(), 0x9ea22e0b);
    }
}

void CustomizeShoppingCart::AddItem(ShoppingCartItem *item) {
    FEShoppingCartItem *option = new ("FEShoppingCartItem", 0) FEShoppingCartItem(item);
    option->SetTitleObject(GetCurrentFEString("PART_NAME_"));
    option->SetDataObject(GetCurrentFEString("PRICE_"));
    option->SetBacking(GetCurrentFEObject(pBackingName));
    option->SetCheckIcon(GetCurrentFEImage("CHECK_ICON_"));
    option->SetTradeInString(GetCurrentFEString("TRADE_IN_"));
    option->SetTopLeft(vLastWidgetPos);
    option->SetMaxTitleSize(vMaxTitleSize);
    option->SetMaxDataSize(vMaxDataSize);
    option->SetDataPos(vDataPos);
    Options.AddTail(option);
    iIndexToAdd++;
    IncrementStartPos();
    option->Show();
    option->Draw();
    option->Position();
    float data_left = FEngGetTopLeftX(option->GetDataObject());
    float data_right = data_left + FEngGetSizeX(option->GetDataObject());
    option->SetWidth(bAbs(option->GetTopLeftX() - data_right));
}

// UNSOLVED
void CustomizeShoppingCart::ClearUncheckedItems() {
    ShoppingCartItem *item = gCarCustomizeManager.GetFirstCartItem();
    while (item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
        if (!item->IsActive()) {
            if (item->GetBuyingPart()->GetSlotID() == CARSLOTID_VINYL_LAYER0) {
                ShoppingCartItem *color_item = gCarCustomizeManager.GetFirstCartItem();
                while (color_item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
                    switch (color_item->GetBuyingPart()->GetSlotID()) {
                        case CARSLOTID_VINYL_COLOUR0_0:
                        case CARSLOTID_VINYL_COLOUR0_1:
                        case CARSLOTID_VINYL_COLOUR0_2:
                        case CARSLOTID_VINYL_COLOUR0_3: {
                            ShoppingCartItem *color_temp = color_item->GetNext();
                            gCarCustomizeManager.RemoveFromCart(color_item);
                            color_item = color_temp;
                            break;
                        }

                        default:
                            color_item = color_item->GetNext();
                            break;
                    }
                }
            }
            ShoppingCartItem *temp = item->GetNext();
            gCarCustomizeManager.RemoveFromCart(item);
            item = temp;
        } else {
            item = item->GetNext();
        }
    }
    gCarCustomizeManager.ResetPreview();
}

void CustomizeShoppingCart::UncheckAllItems() {
    ShoppingCartItem *cart_item = gCarCustomizeManager.GetFirstCartItem();
    while (cart_item != gCarCustomizeManager.GetLastCartItem()->GetNext()) {
        if (cart_item->IsActive()) {
            cart_item->ToggleActive();
        }
        cart_item = cart_item->GetNext();
    }
    for (FEShoppingCartItem *item = static_cast<FEShoppingCartItem *>(Options.GetHead()); item != Options.EndOfList();
         item = static_cast<FEShoppingCartItem *>(item->GetNext())) {
        item->SetCheckScripts();
        item->Draw();
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
    HeatMeter.Init(GetPackageName(), "HEAT_METER", 1.0f, 5.0f, gCarCustomizeManager.GetActualHeat(), gCarCustomizeManager.GetCartHeat());

    ShoppingCartItem *item = gCarCustomizeManager.GetFirstCartItem();
    for (ShoppingCartItem *item = gCarCustomizeManager.GetFirstCartItem(); item != gCarCustomizeManager.GetLastCartItem()->GetNext();
         item = item->GetNext()) {
        switch (item->GetBuyingPart()->GetSlotID()) {
            case CARSLOTID_VINYL_COLOUR0_0:
            case CARSLOTID_VINYL_COLOUR0_1:
            case CARSLOTID_VINYL_COLOUR0_2:
            case CARSLOTID_VINYL_COLOUR0_3:
            case CARSLOTID_DECAL_LEFT_DOOR_LNUMBER:
            case CARSLOTID_DECAL_LEFT_DOOR_RNUMBER:
            // case CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER: // *NOT* LNUMBER
            case CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER:
            case CARSLOTID_HUD_BACKING_COLOUR:
            case CARSLOTID_HUD_NEEDLE_COLOUR:
            case CARSLOTID_HUD_CHARACTER_COLOUR:
                break;
            default:
                AddItem(item);
                break;
        }
    }

    if (gCarCustomizeManager.IsInBackRoom()) {
        SetMarkerImages();
    }

    SetInitialOption(0);

    for (FEShoppingCartItem *item = static_cast<FEShoppingCartItem *>(Options.GetHead());
         item != static_cast<FEShoppingCartItem *>(Options.EndOfList()); item = static_cast<FEShoppingCartItem *>(item->GetNext())) {
        item->SetCheckScripts();
        item->SetActiveScripts();
    }

    RefreshHeader();
}

CustomizeCategoryScreen::CustomizeCategoryScreen(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), //
      bBackingOut(false),   //
      BackToPkg(nullptr),   //
      HeatMeter() {
    Category = sd->Arg & CC_TO_CAT_MASK;
    FromCategory = CC_GET_HIWORD(sd->Arg);
    if (Category != 0 || !gCarCustomizeManager.IsInBackRoom()) {
        GarageMainScreen::GetInstance()->SetCustomizationCategory(Category);
    }

    HeatMeter.Init(GetPackageName(), "HEAT_METER", 1.0f, 5.0f, gCarCustomizeManager.GetActualHeat(), gCarCustomizeManager.GetCartHeat());
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

int CustomizeCategoryScreen::AddCustomOption(const char *to_pkg, uint32 tex_hash, uint32 name_hash, uint32 to_cat) {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (gCarCustomizeManager.IsInBackRoom()) {
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
        case __PAD_START__:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
        case FEHASH_EXITCOMPLETE:
            if (!bBackingOut) {
                break;
            }
            cFEng::Get()->QueuePackageSwitch(BackToPkg, FromCategory | CC_MAKE_HIWORD(Category), 0, false);
            break;
        case __PAD_BACK__: {
            bool leave = true;
            switch (Category) {
                case CC_PARTS:
                case CC_PERFORMANCE:
                case CC_VISUAL:
                    if (gCarCustomizeManager.DoesCartHaveActiveParts()) {
                        cFEng::Get()->QueueGameMessage(0x1720b124, GetPackageName(), 0xFF);
                        leave = false;
                        Options.SetReactToInput(true);
                    } else {
                        gCarCustomizeManager.EmptyCart();
                    }
                    break;
            }
            if (leave) {
                bBackingOut = true;
                const u32 FEObj_leavescreen = 0x587c018b;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;
        }
        case __PAD_BUTTON0__: {
            CustomizeMainOption *opt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
            Showcase::FromArgs = Category | CC_MAKE_HIWORD(opt->Category);
            Showcase::FromPackage = GetPackageName();
            cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<int>(gCarCustomizeManager.GetTuningCar()), 0, false);
            break;
        }
        case dialog_message_cancelled:
            Options.SetReactToInput(true);
            break;
        case 0x7a318ee0: {
            gCarCustomizeManager.EmptyCart();
            const u32 FEObj_leavescreen = 0x587c018b;
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
    uint32 to_cat = static_cast<CustomizeMainOption *>(Options.GetCurrentOption())->Category & CC_TO_CAT_MASK;

    if (Category != CC_VISUAL || to_cat != CC_RIM_PAINT || msg != __BUTTON_PRESSED__) {
        CustomizeCategoryScreen::NotificationMessage(msg, pobj, param1, param2);
    }

    switch (msg) {
        case 0x5a928018: {
            CustomizeMainOption *opt = FindInCartOption();
            if (opt != nullptr) {
                int slot_id = 0;
                switch (opt->Category) {
                    case CC_RIM_BRANDS:
                        slot_id = CARSLOTID_FRONT_WHEEL;
                        break;
                    case CC_VINYL_TYPES:
                        slot_id = CARSLOTID_VINYL_LAYER0;
                        break;
                }
                if (slot_id == 0)
                    return;
                if (gCarCustomizeManager.IsPartTypeInCart(slot_id) != nullptr)
                    return;
                InCartPartOptionIndex = 0;
                RefreshHeader();
            }
            break;
        }
        case __BUTTON_PRESSED__: {
            switch (to_cat) {
                case CC_DECAL_WINDSHIELD:
                case CC_DECAL_REAR_WINDOW:
                case CC_DECAL_LEFT_DOOR:
                case CC_DECAL_RIGHT_DOOR:
                case CC_DECAL_LEFT_QP:
                case CC_DECAL_RIGHT_QP:
                    CustomizeDecals::CurrentDecalLocation = to_cat;
                    break;
            }

            CustomizeMainOption *copt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
            copt->IsStockOption();
            bool stockOption = copt->IsStockOption();

            if (stockOption && static_cast<SetStockPartOption *>(copt)->ThePart->IsInstalled() && InCartPartOptionIndex != 0) {
                int slot_id = 0;
                switch (static_cast<unsigned short>(copt->Category)) {
                    case CC_RIM_BRAND_STOCK:
                        slot_id = CARSLOTID_FRONT_WHEEL;
                        break;
                    case CC_VINYL_GROUP_STOCK:
                        slot_id = CARSLOTID_VINYL_LAYER0;
                        for (int i = 0; i <= 2; i++) {
                            int color_slot = CARSLOTID_VINYL_COLOUR_FIRST + i;
                            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(color_slot);
                            if (item != nullptr) {
                                gCarCustomizeManager.RemoveFromCart(item);
                            }
                        }
                        break;
                }
                if (slot_id != 0) {
                    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
                    if (item != nullptr) {
                        gCarCustomizeManager.RemoveFromCart(item);
                        InCartPartOptionIndex = 0;
                        RefreshHeader();
                    }
                }
            }

            if (bStrICmp(GetPackageName(), g_pCustomizeSubTopPkg) == 0 || bStrICmp(GetPackageName(), g_pCustomizeSubPkg) == 0) {
                bool ok_to_leave = false;
                switch (Category) {
                    case CC_VISUAL:
                        if (to_cat == CC_RIM_PAINT) {
                            CarPart *stock_rim = gCarCustomizeManager.GetStockCarPart(CARSLOTID_FRONT_WHEEL);
                            CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_FRONT_WHEEL);
                            if (stock_rim == installed) {
                                DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, dialog_message_cancelled, 0xbdb19a9f);
                            } else {
                                CustomizeMainOption *opt = static_cast<CustomizeMainOption *>(Options.GetCurrentOption());
                                cFEng::Get()->QueuePackageSwitch(opt->ToPkg, opt->Category, 0, false);
                                ok_to_leave = true;
                            }
                        }
                        break;
                    case CC_RIM_BRANDS:
                        if (Options.GetCurrentIndex() == 1) {
                            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_FRONT_WHEEL);
                            if (item != nullptr) {
                                CarPart *car_part = item->GetBuyingPart()->GetPart();
                                InCartPartOptionIndex = GetRimBrandIndex(car_part->GetBrandNameHash());
                            }
                            CarPart *car_part = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_FRONT_WHEEL);
                            if (car_part != nullptr) {
                                InstalledPartOptionIndex = GetRimBrandIndex(car_part->GetBrandNameHash());
                            }
                            Options.SetReactToInput(true);
                            RefreshHeader();
                        } else {
                            ok_to_leave = true;
                        }
                        break;
                    case CC_VINYL_TYPES:
                        if (Options.GetCurrentIndex() == 1) {
                            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_VINYL_LAYER0);
                            if (item != nullptr) {
                                CarPart *car_part = item->GetBuyingPart()->GetPart();
                                if (car_part != nullptr) {
                                    InCartPartOptionIndex = GetVinylGroupIndex(car_part->GetGroupNumber() & 0x1f);
                                } else {
                                    InCartPartOptionIndex = 1;
                                }
                            }
                            CarPart *car_part = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_VINYL_LAYER0);
                            if (car_part != nullptr) {
                                InstalledPartOptionIndex = GetVinylGroupIndex(car_part->GetGroupNumber() & 0x1f);
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
                if (ok_to_leave) {
                    const u32 FEObj_leavescreen = 0x587c018b;
                    cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
                }
            }
            break;
        }
        case __PAD_BUTTON4__:
            if (gCarCustomizeManager.IsCareerMode())
                return;
            if (Category == CC_PERFORMANCE) {
                DialogInterface::ShowTwoButtons(GetPackageName(), "", dialog_confirmation, LANGUAGE_COMMON_YES, LANGUAGE_COMMON_NO, 0x6820e23e,
                                                dialog_message_cancelled, first_dialog_button1, 0x892cb612);
                RefreshHeader();
            }
            break;
        case 0x6820e23e:
            gCarCustomizeManager.MaxOutPerformance();
            RefreshHeader();
            break;
        case dialog_message_cancelled: // TODO: should probably get from hash
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
        case CC_PARTS:
            SetupParts();
            break;
        case CC_PERFORMANCE:
            SetupPerformance();
            break;
        case CC_VISUAL:
            SetupVisual();
            break;
        case CC_VINYL_TYPES:
            SetupVinylGroups();
            break;
        case CC_DECAL_LOCATION:
            SetupDecalLocations();
            break;
        case CC_RIM_BRANDS:
            SetupRimBrands();
            break;
        case CC_DECAL_WINDSHIELD:
        case CC_DECAL_REAR_WINDOW:
        case CC_DECAL_LEFT_DOOR:
        case CC_DECAL_RIGHT_DOOR:
        case CC_DECAL_LEFT_QP:
        case CC_DECAL_RIGHT_QP:
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
    if (gCarCustomizeManager.IsInBackRoom()) {
        TitleHash = 0xbfd5b50f;
    } else {
        TitleHash = 0xbaef8282;
    }
    BackToPkg = g_pCustomizeMainPkg;
    gCarCustomizeManager.SetInPerformance(true);
    if (!gCarCustomizeManager.IsInBackRoom()) {
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
    SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
}

void CustomizeSub::SetupVisual() {
    if (gCarCustomizeManager.IsInBackRoom()) {
        TitleHash = 0x10c3fe31;
    } else {
        TitleHash = 0xbfa7d7c4;
    }
    BackToPkg = g_pCustomizeMainPkg;
    if (!gCarCustomizeManager.IsInBackRoom()) {
        AddCustomOption(g_pCustomizePaintPkg, 0xa3b76154, 0x055da70c, CC_PAINT);
        AddCustomOption(g_pCustomizeSubTopPkg, 0x55778e5a, 0xbfa52c55, CC_VINYL_TYPES);
        if (!gCarCustomizeManager.IsHeroCar()) {
            AddCustomOption(g_pCustomizePaintPkg, 0xd223f84a, 0xe126ff53, CC_RIM_PAINT);
        }
        AddCustomOption(g_pCustomizePartsPkg, 0x3f23165c, 0xd32729a6, CC_WINDOW_TINT);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xda1dae54, 0x955980bc, CC_DECAL_LOCATION);
        AddCustomOption("FeCustomize_ToolBox", 0x45a1c644, 0x6857e5ac, CC_NUMBERS);
        AddCustomOption(g_pCustomizeHudPkg, 0x028f88bc, 0x78980a6b, CC_CUSTOM_HUD);
    } else {
        AddCustomOption(g_pCustomizePaintPkg, 0x0db89e17, 0x055da70c, CC_PAINT);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xd35f04c0, 0xbfa52c55, CC_VINYL_TYPES);
        AddCustomOption(g_pCustomizeSubTopPkg, 0xa9135927, 0x955980bc, CC_DECAL_LOCATION);
        AddCustomOption(g_pCustomizeHudPkg, 0x8ba602fc, 0x78980a6b, CC_CUSTOM_HUD);
    }
    SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
}

int CustomizeSub::GetRimBrandIndex(uint32 brand) {
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

    CarPart *stock_car_part = gCarCustomizeManager.GetStockCarPart(CARSLOTID_FRONT_WHEEL);
    SelectablePart *part = new ("SelectablePart 14", 0)
        SelectablePart(stock_car_part, CARSLOTID_FRONT_WHEEL, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    if (gCarCustomizeManager.IsPartInstalled(part)) {
        part->SetPartState(CPS_INSTALLED);
    }

    SetStockPartOption *stock_opt = new ("SetStockPartOption", 0) SetStockPartOption(part, 0xf3990b6, CC_RIM_BRAND_STOCK);
    AddOption(stock_opt);

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

    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_FRONT_WHEEL);
    if (item != nullptr) {
        CarPart *car_part = item->GetBuyingPart()->GetPart();
        InCartPartOptionIndex = GetRimBrandIndex(car_part->GetBrandNameHash());
    }

    CarPart *car_part = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_FRONT_WHEEL);
    if (car_part != nullptr) {
        InstalledPartOptionIndex = GetRimBrandIndex(car_part->GetBrandNameHash());
    }

    if (FromCategory == CC_PARTS) {
        if (InCartPartOptionIndex != 0) {
            SetInitialOption(InCartPartOptionIndex);
        } else if (InstalledPartOptionIndex != 0) {
            SetInitialOption(InstalledPartOptionIndex);
        } else {
            SetInitialOption(1);
        }
    } else {
        SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
    }
    if (FromCategory >= CC_RIM_BRAND_STOCK && FromCategory <= CC_RIM_BRAND_ROJA) {
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
    SelectablePart *part =
        new ("SelectablePart 14", 0) SelectablePart(nullptr, CARSLOTID_VINYL_LAYER0, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    if (gCarCustomizeManager.IsPartInstalled(part)) {
        part->SetPartState(CPS_INSTALLED);
    }
    SetStockPartOption *stock_opt = new ("SetStockPartOption", 0) SetStockPartOption(part, 0x21f3d114, CC_VINYL_GROUP_STOCK);
    AddOption(stock_opt);

    AddCustomOption(g_pCustomizePartsPkg, 0xf8148554, 0xd9228fc6, CC_VINYL_GROUP_FLAME);
    AddCustomOption(g_pCustomizePartsPkg, 0x192d84da, 0x1e8d885f, CC_VINYL_GROUP_TRIBAL);
    AddCustomOption(g_pCustomizePartsPkg, 0xf7352706, 0x1c619fd8, CC_VINYL_GROUP_STRIPE);
    AddCustomOption(g_pCustomizePartsPkg, 0x1223cc89, 0x9c1b8935, CC_VINYL_GROUP_RACING_FLAG);
    AddCustomOption(g_pCustomizePartsPkg, 0xbc44bbcb, 0x7956f7b0, CC_VINYL_GROUP_NATIONAL_FLAG);
    AddCustomOption(g_pCustomizePartsPkg, 0x694ca0ca, 0x2d5bff0f, CC_VINYL_GROUP_BODY);
    AddCustomOption(g_pCustomizePartsPkg, 0x1b3a8dd3, 0x209a9158, CC_VINYL_GROUP_UNIQUE);
    AddCustomOption(g_pCustomizePartsPkg, 0x1ba508fc, 0xcd057d21, CC_VINYL_GROUP_CONTEST);

    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_VINYL_LAYER0);
    if ((item != nullptr) && (item->GetBuyingPart() != nullptr)) {
        CarPart *car_part = item->GetBuyingPart()->GetPart();
        if (car_part != nullptr) {
            InCartPartOptionIndex = GetVinylGroupIndex(car_part->GetGroupNumber());
        } else {
            InCartPartOptionIndex = 1;
        }
    }

    CarPart *car_part = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_VINYL_LAYER0);
    if (car_part != nullptr) {
        InstalledPartOptionIndex = GetVinylGroupIndex(car_part->GetGroupNumber());
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
        SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
    }
    if (FromCategory >= CC_VINYL_GROUP_STOCK && FromCategory <= CC_VINYL_GROUP_CONTEST) {
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
        SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
    }
    if (FromCategory >= CC_DECAL_WINDSHIELD && FromCategory <= CC_DECAL_RIGHT_QP) {
        FromCategory = CC_VISUAL;
    }
}

void CustomizeSub::SetupDecalPositions() {
    TitleHash = 0x74d1887d;
    BackToPkg = g_pCustomizeSubTopPkg;
    switch (Category) {
        case CC_DECAL_LEFT_DOOR:
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f48, 0x7d212cfa, CC_DECAL_SLOT_1);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f49, 0x7d212cfb, CC_DECAL_SLOT_2);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4a, 0x7d212cfc, CC_DECAL_SLOT_3);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4b, 0x7d212cfd, CC_DECAL_SLOT_4);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4c, 0x7d212cfe, CC_DECAL_SLOT_5);
            AddCustomOption(g_pCustomizeDecalsPkg, 0xfe957f4d, 0x7d212cff, CC_DECAL_SLOT_6);
            break;
        case CC_DECAL_RIGHT_DOOR:
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
        SetInitialOption(FromCategory & (CC_FROM_CAT_MASK | 0xFF));
        FromCategory = CC_DECAL_LOCATION;
    }
}

CustomizeMain::CustomizeMain(ScreenConstructorData *sd) : CustomizeCategoryScreen(sd) {
    iPerfIndex = 0;
    invalidMarkers = 0;
    if (g_TheCustomizeEntryPoint == 0) {
        CarViewer::haveLoadedOnce = false;
    }
    gCarCustomizeManager.TakeControl(g_TheCustomizeEntryPoint, g_pCustomizeCarRecordToUse);
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

// UNSOLVED
void CustomizeMain::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (!gCarCustomizeManager.IsCareerMode() || msg != __PAD_BACK__) {
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
        case __PAD_BACK__: {
            const u32 FEObj_QUIT_SCREEN = 0x6d5d86a1;
            if (gCarCustomizeManager.IsCareerMode()) {
                if (!gCarCustomizeManager.IsInBackRoom()) {
                    cFEng::Get()->QueuePackageMessage(FEObj_QUIT_SCREEN, GetPackageName(), nullptr);
                    GarageMainScreen::GetInstance()->SetCustomizationCategory(-1);
                    FEDatabase->SetPlayersJoystickPort(0, FEngMapJoyParamToJoyport(param1));
                    if (!FEDatabase->IsCarStableDirty()) {
                        MemoryCard::GetInstance()->CancelNextAutoSave();
                    }
                    CarViewer::haveLoadedOnce = false;
                    RaceStarter::StartCareerFreeRoam();
                } else {
                    SwitchRooms();
                    break;
                }
            } else {
                cFEng::Get()->QueuePackageMessage(0x6d5d86a1, GetPackageName(), nullptr);
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
        case __PAD_BUTTON5__:
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
                        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_info, 0x417b2601, 0x34dc1bec, 0x3b3e83);
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
    bool isReallyHeroCar = gCarCustomizeManager.IsHeroCar();
    if (!gCarCustomizeManager.IsInBackRoom()) {
        if (!isReallyHeroCar) {
            AddCustomOption(g_pCustomizeSubPkg, 0x6e0ca66c, 0x55dce1a, CC_PARTS);
            iPerfIndex = AddCustomOption(g_pCustomizeSubPkg, 0x3987d054, 0xbaef8282, CC_PERFORMANCE);
        }
        AddCustomOption(g_pCustomizeSubPkg, 0x3e31ba56, 0xbfa7d7c4, CC_VISUAL);
    } else {
        if (!isReallyHeroCar) {
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
        case CPS_AVAILABLE: {
            FEngSetInvisible(GetPackageName(), 0xd0582feb);
            break;
        }
        case CPS_INSTALLED: {
            FEngSetVisible(GetPackageName(), 0xd0582feb);
            FEngSetTextureHash(GetPackageName(), 0xd0582feb, 0x696ae039);
            break;
        }
        case CPS_IN_CART: {
            FEngSetVisible(GetPackageName(), 0xd0582feb);
            FEngSetTextureHash(GetPackageName(), 0xd0582feb, 0x1a777e25);
            break;
        }
    }
}

void CustomizationScreenHelper::SetCashVisibility(bool visible) {
    if (visible) {
        FEngSetVisible(GetPackageName(), 0x8d1559a4);
    } else {
        FEngSetInvisible(GetPackageName(), 0x8d1559a4);
    }
}

void CustomizationScreenHelper::SetUnlockOverlayState(bool show, uint32 blurb_hash) {
    const u32 FEObj_APPEAR = FEHASH_APPEAR;
    const u32 FEObj_LEAVE = FEHASH_LEAVE;

    uint32 script_hash = FEObj_APPEAR;
    bUnlockOverlayShowing = show;
    if (show) {
        FEngSetLanguageHash(GetPackageName(), 0xa6298e25, blurb_hash);
    } else {
        script_hash = FEObj_LEAVE;
    }
    FEngSetScript(GetPackageName(), 0xebc3e6b7, script_hash, true);
}

void CustomizationScreenHelper::SetCareerStuff(SelectablePart *part, uint32 cat, uint32 tradeInValue) {
    if (gCarCustomizeManager.IsCareerMode()) {
        if (gCarCustomizeManager.IsInBackRoom()) {
            FEngSetLanguageHash(GetPackageName(), 0x63ca8308, GetMarkerNameFromCategory(static_cast<eCustomizeCategory>(cat)));
            FEPrintf(GetPackageName(), 0x23d918fe, "1");
            FEPrintf(GetPackageName(), 0x83e3cd39, "%$d", GetNumMarkersFromCategory(static_cast<eCustomizeCategory>(cat)));
        } else {
            if (part != nullptr) {
                FEPrintf(GetPackageName(), 0xdbb80edd, "%$d", part->GetPrice());
            } else {
                if (gCarCustomizeManager.GetTempColoredPart() != nullptr) {
                    FEPrintf(GetPackageName(), 0xdbb80edd, "%$d", gCarCustomizeManager.GetTempColoredPart()->GetPrice());
                } else {
                    FEPrintf(GetPackageName(), 0xdbb80edd, "-1");
                }
            }
            FEPrintf(GetPackageName(), 0xc60adcfd, "%$d", FEDatabase->GetCareerSettings()->GetCash());
            FEPrintf(GetPackageName(), 0x7a6d2f71, "%$d", gCarCustomizeManager.GetCartTotal(CCT_TOTAL));
            FEPrintf(GetPackageName(), 0xa91eda8a, "%$d", tradeInValue);
        }
        SetHeatValue(gCarCustomizeManager.GetActualHeat());
        SetHeatPreview(gCarCustomizeManager.GetPreviewHeat(part));
        DrawMeters();
    } else {
        SetCareerStatusIcon(CPS_AVAILABLE);
        SetCashVisibility(false);
        HeatMeter.SetVisibility(false);
        FEngSetInvisible(GetPackageName(), 0x24c6bfad);
        FEngSetInvisible(GetPackageName(), 0xea903012);
    }
}

void CustomizationScreenHelper::SetPartStatus(SelectablePart *part, uint32 unlock_blurb, int part_num, int max_parts) {
    if (part != nullptr) {
        if (part->IsInstalled()) {
            SetPlayerCarStatusIcon(CPS_INSTALLED);
        } else if (part->IsInCart()) {
            SetPlayerCarStatusIcon(CPS_IN_CART);
        } else {
            SetPlayerCarStatusIcon(CPS_AVAILABLE);
        }
        if (part->IsLocked() && unlock_blurb) {
            if (bInitComplete) {
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
    FEPrintf(GetPackageName(), 0x6f25a248, "%$d", part_num);
    FEPrintf(GetPackageName(), 0xb2037bdc, "%$d", max_parts);
}

void CustomizationScreenHelper::FlashStatusIcon(eCustomizePartState state, bool play_sound) {
    uint32 object_hash = 0;

    switch (state) {
        case CPS_LOCKED:
            object_hash = 0xcffb7033;
            break;
        case CPS_INSTALLED:
        case CPS_IN_CART:
            object_hash = 0xd0582feb;
            break;
    }

    FEngSetScript(GetPackageName(), object_hash, 0x280164f, true);
    if (play_sound) {
        extern EAXSound *g_pEAXSound;
        g_pEAXSound->PlayUISoundFX(UISND_COMMON_WRONG);
    }
}

CustomizationScreen::CustomizationScreen(ScreenConstructorData *sd)
    : IconScrollerMenu(sd), DisplayHelper(sd->PackageFilename), pReplacingOption(nullptr), ScrollTime() {
    DelayFadeIn();
    Category = sd->Arg & CC_TO_CAT_MASK;
    FromCategory = CC_GET_HIWORD(sd->Arg);

    switch (GetCategory()) {
        case CC_DECAL_SLOT_1:
        case CC_DECAL_SLOT_2:
        case CC_DECAL_SLOT_3:
        case CC_DECAL_SLOT_4:
        case CC_DECAL_SLOT_5:
        case CC_DECAL_SLOT_6:
            GarageMainScreen::GetInstance()->SetCustomizationCategory(CustomizeDecals::CurrentDecalLocation);
            break;
        default:
            GarageMainScreen::GetInstance()->SetCustomizationCategory(Category);
            break;
    }
}

CustomizationScreen::~CustomizationScreen() {
    GarageMainScreen::GetInstance()->SetCustomizationCategory(-1);
}

// UNSOLVED
void CustomizationScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == FEHASH_INITCOMPLETE) {
        DisplayHelper.SetInitComplete(true);
        RefreshHeader();
    }
    if (msg == __PAD_LEFT__ || msg == __PAD_RIGHT__) {
        ScrollTime = RealTimer;
    }
    IconScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            if (!bNeedsRefresh) {
                return;
            }

            if ((RealTimer - ScrollTime).GetSeconds() * 0.001f <= 0.25f) {
                return;
            }

            bNeedsRefresh = false;
            RefreshHeader();
            break;
        case 0x5e6ea975:
            Options.StartFadeIn();
            break;
        case __PAD_ACCEPT__: {
            SelectablePart *selected = GetSelectedPart();
            if (selected != nullptr) {
                if (selected->IsLocked()) {
                    PlayLocked();
                    return;
                }
                if (selected->IsInCart()) {
                    PlayInCart();
                    return;
                }
                if (selected->IsInstalled()) {
                    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(selected);
                    if (item != nullptr) {
                        gCarCustomizeManager.RemoveFromCart(item);
                        SelectablePart *part = item->GetBuyingPart();
                        FindMatchingOption(part)->GetPart()->UnSetInCart();
                    }
                    PlayInstalled();
                    return;
                }
            }
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(GetSelectedPart());
            if (item != nullptr) {
                pReplacingOption = FindMatchingOption(item->GetBuyingPart());
            }
            cFEng::Get()->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xFF);
            break;
        }
        case 0x91dfdf84: {
            if (pReplacingOption != nullptr) {
                pReplacingOption->GetPart()->UnSetInCart();
                pReplacingOption = nullptr;
            }
            gCarCustomizeManager.AddToCart(static_cast<CustomizePartOption *>(Options.GetCurrentOption())->GetPart());
            static_cast<CustomizePartOption *>(Options.GetCurrentOption())->GetPart()->SetInCart();
            RefreshHeader();
            break;
        }
        case 0xcf91aacd:
            CustomizeShoppingCart::ExitShoppingCart();
            break;
        case __PAD_BUTTON0__:
            switch (GetCategory()) {
                case CC_ENGINE:
                case CC_TRANSMISSION:
                case CC_SUSPENSION:
                case CC_NITROUS:
                case CC_TIRES:
                case CC_BRAKES:
                case CC_FORCED_INDUCTION:
                    break;
                case CC_CUSTOM_HUD:
                    break;
                default: {
                    if (Options.GetCurrentOption() == nullptr) {
                        Showcase::FromIndex = 0;
                    } else {
                        Showcase::FromIndex = Options.GetCurrentIndex();
                    }
                    Showcase::FromPackage = GetPackageName();
                    Showcase::FromArgs = Category | CC_MAKE_HIWORD(FromCategory);
                    cFEng::Get()->QueuePackageSwitch("Showcase.fng", 0, 0, false);
                    break;
                }
            }
            break;

        case __PAD_START__:
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
            if (!gCarCustomizeManager.IsInBackRoom()) {
                SelectablePart *spart = GetSelectedPart();
                if (spart != nullptr) {
                    if (gCarCustomizeManager.CanTradeIn(spart)) {
                        CarPart *cpart = gCarCustomizeManager.GetInstalledCarPart(spart->GetSlotID());
                        if (cpart != nullptr) {
                            tradeInValue = UnlockSystem::GetCarPartCost(gCarCustomizeManager.GetUnlockFilter(), spart->GetSlotID(), cpart, 0);
                        }
                        tradeInValue = static_cast<int>(static_cast<float>(tradeInValue) * gTradeInFactor);
                    }
                }
            }
        }
        DisplayHelper.SetCareerStuff(GetSelectedPart(), GetCategory(), tradeInValue);

        DisplayHelper.SetPartStatus(GetSelectedPart(), GetSelectedOption()->GetUnlockBlurb(), Options.GetCurrentIndex(), Options.CountElements());
    }
}

void CustomizationScreen::AddPartOption(SelectablePart *part, uint32 tex_hash, uint32 name_hash, uint32 desc_hash, uint32 unlock_hash, bool locked) {
    CustomizePartOption *opt = new ("CustomizePartOption", 0) CustomizePartOption(part, tex_hash, name_hash, desc_hash, unlock_hash);
    AddOption(opt);
    opt->SetLocked(locked);
}

SelectablePart *CustomizationScreen::FindInCartPart() {
    for (CustomizePartOption *opt = static_cast<CustomizePartOption *>(Options.GetHead()); !Options.IsEndOfList(opt);
         opt = static_cast<CustomizePartOption *>(opt->GetNext())) {
        SelectablePart *part = opt->GetPart();
        if ((part != nullptr) && part->IsInCart()) {
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

void UnLoadCustomHUDPacksAndTextures() {
    for (int i = 0; i < 11; i++) {
        for (int j = 0; j < 5u; j++) { // TODO: where does 5u (and 11) come from?
            if (CustomizeHUDTexTextureResources[i][j] != 0) {
                eUnloadStreamingTexture(CustomizeHUDTexTextureResources[i][j]);
            }
            CustomizeHUDTexTextureResources[i][j] = 0;
        }
        if (CustomizeHUDTexPackResources[i]) {
            char sztemp[64];
            bSPrintf(sztemp, "GLOBAL\\HUDS_Custom_%2.2d.bin", i);
            eUnloadStreamingTexturePack(sztemp);
        }
        CustomizeHUDTexPackResources[i] = false;
    }
    CustomizeParts::TexturePackLoaded = false;
}

CustomizeParts::CustomizeParts(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    bTexturesNeedUnload = false;
    if (GetCategory() == CC_CUSTOM_HUD) {
        if (!TexturePackLoaded) {
            for (int i = 0; i < 11; i++) {
                CustomizeHUDTexPackResources[i] = false;
                for (int j = 0; j < 5u; j++) { // TODO: where does 5u (and 11) come from?
                    CustomizeHUDTexTextureResources[i][j] = 0;
                }
            }
        }
        TachRPM = gCarCustomizeManager.GetMaxRPM();
        if (TachRPM > 9500) {
            TachRPM = 10000;
        } else if (TachRPM > 8500) {
            TachRPM = 9000;
        } else if (TachRPM > 7500) {
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
    if (msg != __PAD_ACCEPT__) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case __PAD_ACCEPT__:
            switch (GetCategory()) {
                case CC_CUSTOM_HUD:
                    if (TexturePackLoaded) {

                        SelectablePart *selected = GetSelectedPart();
                        if (selected != nullptr && selected->IsLocked()) {
                            PlayLocked();
                            return;
                        }
                        if (gCarCustomizeManager.GetTempColoredPart() != nullptr) {
                            gCarCustomizeManager.ClearTempColoredPart();
                        }
                        gCarCustomizeManager.SetTempColoredPart(new ("SelectablePart 1", 0) SelectablePart(selected));
                        cFEng::Get()->QueuePackageSwitch(g_pCustomizeHudColorPkg, GetCategory() | CC_MAKE_HIWORD(GetFromCategory()), 0, false);
                    }
                    break;

                case CC_VINYL_GROUP_FLAME:
                case CC_VINYL_GROUP_TRIBAL:
                case CC_VINYL_GROUP_STRIPE:
                case CC_VINYL_GROUP_RACING_FLAG:
                case CC_VINYL_GROUP_NATIONAL_FLAG:
                case CC_VINYL_GROUP_BODY:
                case CC_VINYL_GROUP_UNIQUE:
                case CC_VINYL_GROUP_CONTEST: {
                    SelectablePart *selected = GetSelectedPart();
                    if (selected != nullptr && selected->IsLocked()) {
                        PlayLocked();
                        break;
                    }
                    if (selected->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0) == 0) {
                        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
                        break;
                    }
                    gCarCustomizeManager.SetTempColoredPart(new ("SelectablePart 2", 0) SelectablePart(selected));
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizePaintPkg, GetCategory() | CC_MAKE_HIWORD(GetFromCategory()), 0, false);
                    break;
                }
                default:
                    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
                    break;
            }
            break;
        case 0xcf91aacd:
            if (GetCategory() == CC_CUSTOM_HUD && TexturePackLoaded) {
                bTexturesNeedUnload = true;
            }
            break;
        case 0x5a928018: {
            SelectablePart *part = FindInCartPart();
            if ((part != nullptr) && (gCarCustomizeManager.IsPartInCart(part) == nullptr)) {
                part->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case __PAD_BACK__:
            switch (GetCategory()) {
                case CC_CUSTOM_HUD:
                    if (TexturePackLoaded) {
                        bTexturesNeedUnload = true;
                        cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
                    }
                    break;

                case CC_VINYL_GROUP_FLAME:
                case CC_VINYL_GROUP_TRIBAL:
                case CC_VINYL_GROUP_STRIPE:
                case CC_VINYL_GROUP_RACING_FLAG:
                case CC_VINYL_GROUP_NATIONAL_FLAG:
                case CC_VINYL_GROUP_BODY:
                case CC_VINYL_GROUP_UNIQUE:
                case CC_VINYL_GROUP_CONTEST:
                    gCarCustomizeManager.ClearTempColoredPart();
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
                    break;
                default:
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);

                    break;
            }
            break;
    }
}

// UNSOLVED regswap
void CustomizeParts::Setup() {
    uint32 icon_hash = 0;
    int car_slot_id = 0;
    uint32 vinyl_group_number = 0;
    bool is_vinyl = false;
    CarPart *installed_part = nullptr;
    bool part_found = false;

    switch (GetCategory()) {
        case CC_BODY_KIT:
            SetTitleHash(0x6134c218);
            icon_hash = 0x28c24f6;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0xaf393dba;
            }
            car_slot_id = CARSLOTID_BODY;
            break;
        case CC_HOODS:
            SetTitleHash(0x4d4a88d);
            icon_hash = 0x28f7092;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0xf375276e;
            }
            car_slot_id = CARSLOTID_HOOD;
            break;
        case CC_ROOF_SCOOPS:
            SetTitleHash(0x61e8f83c);
            icon_hash = 0x79165861;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0x25a4375e;
            }
            car_slot_id = CARSLOTID_ROOF;
            break;
        case CC_CUSTOM_HUD:
            if (!CustomizeParts::TexturePackLoaded) {
                const u32 FEObj_DISABLE_INPUTS = 0;
                cFEng::Get()->QueuePackageMessage(0x13fd3296, GetPackageName(), nullptr);
                LoadHudTextures();
            } else {
                ShowHudObjects();
                const u32 FEObj_ENABLE_INPUTS = 0;
                cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
            }
            if (gCarCustomizeManager.GetTempColoredPart() != nullptr) {
                installed_part = gCarCustomizeManager.GetTempColoredPart()->GetPart();
                part_found = true;
            }
            SetTitleHash(0x78980a6b);
            icon_hash = 0x28f88bc;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon_hash = 0x8ba602fc;
            }
            car_slot_id = CARSLOTID_CUSTOM_HUD;
            break;
        case CC_WINDOW_TINT:
            SetTitleHash(0xd32729a6);
            icon_hash = 0x3f23165c;
            car_slot_id = CARSLOTID_WINDOW_TINT;
            break;
        case CC_VINYL_GROUP_FLAME:
            SetTitleHash(0xd9228fc6);
            icon_hash = 0xf8148554;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 0;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_TRIBAL:
            SetTitleHash(0x1e8d885f);
            icon_hash = 0x192d84da;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 1;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_STRIPE:
            SetTitleHash(0x1c619fd8);
            icon_hash = 0xf7352706;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 2;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_RACING_FLAG:
            SetTitleHash(0x9c1b8935);
            icon_hash = 0x1223cc89;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 3;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_NATIONAL_FLAG:
            SetTitleHash(0x7956f7b0);
            icon_hash = 0xbc44bbcb;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 4;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_BODY:
            SetTitleHash(0x2d5bff0f);
            icon_hash = 0x694ca0ca;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 5;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_UNIQUE:
            SetTitleHash(0x209a9158);
            icon_hash = 0x1b3a8dd3;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 6;
            is_vinyl = true;
            break;
        case CC_VINYL_GROUP_CONTEST:
            SetTitleHash(0xcd057d21);
            icon_hash = 0x1ba508fc;
            car_slot_id = CARSLOTID_VINYL_LAYER0;
            vinyl_group_number = 7;
            is_vinyl = true;
            break;
    }

    if (is_vinyl && (gCarCustomizeManager.GetTempColoredPart() != nullptr)) {
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

    int installed_index = 0;
    int current_part_index = 1;
    uint32 original_icon_hash = icon_hash;
    SelectablePart *part = part_list.GetHead();

    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        uint32 unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), part->GetUpgradeLevel());

        if (is_vinyl) {
            if ((part->GetPart()->GetGroupNumber()) == vinyl_group_number) {
                if (UnlockSystem::IsUnlockableAvailable(part->GetPart()->GetPartNameHash())) {
                    AddPartOption(part, icon_hash, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
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
                switch (GetCategory()) {
                    case CC_HOODS:
                        if (gCarCustomizeManager.IsInBackRoom()) {
                            icon_hash = 0x2478e136;
                        } else {
                            icon_hash = 0x68495926;
                        }
                        break;
                    case CC_ROOF_SCOOPS:
                        if (gCarCustomizeManager.IsInBackRoom()) {
                            icon_hash = 0xcd6b4e26;
                        } else {
                            icon_hash = 0xfc618215;
                        }
                        break;
                }
            } else {
                icon_hash = original_icon_hash;
            }
            AddPartOption(part, icon_hash, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
        }
        if (part != nullptr) {
            if ((installed_part != nullptr) && part->GetPart() == installed_part) {
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
    char sztemp[64];
    bSPrintf(sztemp, "GLOBAL\\HUDS_Custom_%2.2d.bin", PacksLoadedCount);
    CustomizeHUDTexPackResources[PacksLoadedCount] =
        (eLoadStreamingTexturePack(sztemp, TexturePackLoadedCallbackAccessor, reinterpret_cast<uint32>(this), 0) != 0);
}

void CustomizeParts::TexturePackLoadedCallback() {
    PacksLoadedCount++;
    int hud_num = PacksLoadedCount - 1;
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
        const u32 FEObj_ENABLE_INPUTS = 0; // TODO
        cFEng::Get()->QueuePackageMessage(0x8cb81f09, GetPackageName(), nullptr);
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
    ShoppingCartItem *hud_item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_CUSTOM_HUD);
    CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_CUSTOM_HUD);
    if (GetSelectedPart()->GetPart() == installed_hud ||
        (hud_item != nullptr && GetSelectedPart()->GetPart() == hud_item->GetBuyingPart()->GetPart())) {

        uint32 slot_id = CARSLOTID_HUD_BACKING_COLOUR;
        uint32 colors[3];
        for (int i = 0; i < 3; i++) {
            CarPart *color_part;
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);
            if (item != nullptr && hud_item != nullptr && GetSelectedPart()->GetPart() == hud_item->GetBuyingPart()->GetPart()) {
                color_part = item->GetBuyingPart()->GetPart();
            } else {
                color_part = gCarCustomizeManager.GetInstalledCarPart(slot_id);
            }
            uint8 r = color_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            uint8 g = color_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            uint8 b = color_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            colors[i] = 0xff000000 | (r << 16) | (g << 8) | b;
            slot_id++;
        }
        FEngSetColor(GetPackageName(), 0x5d19f25, colors[0]);
        FEngSetColor(GetPackageName(), 0xc0721eb9, colors[2]);
        FEngSetColor(GetPackageName(), 0xc62ad685, colors[2]);
        FEngSetColor(GetPackageName(), 0xb8f1f802, colors[2]);
        FEngSetColor(GetPackageName(), 0xd312f0cb, colors[1]);
        FEngSetColor(GetPackageName(), 0x8fe2a217, colors[1]);
    } else {
        FEngSetColor(GetPackageName(), 0x5d19f25, CHT_DEFAULT_FILL_COLOR);
        FEngSetColor(GetPackageName(), 0xc0721eb9, CHT_DEFAULT_LINE_COLOR);
        FEngSetColor(GetPackageName(), 0xc62ad685, CHT_DEFAULT_LINE_COLOR);
        FEngSetColor(GetPackageName(), 0xb8f1f802, CHT_DEFAULT_LINE_COLOR);
        FEngSetColor(GetPackageName(), 0xd312f0cb, CHT_DEFAULT_NEEDLE_COLOR);
        FEngSetColor(GetPackageName(), 0x8fe2a217, CHT_DEFAULT_NEEDLE_COLOR);
    }
}

void CustomizeParts::RefreshHeader() {
    CustomizationScreen::RefreshHeader();
    if (Options.IsEmpty()) {
        return;
    }

    SelectablePart *part = GetSelectedPart();
    if (part->GetPart()->HasAppliedAttribute(0x6212682b)) {
        if (part->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0)) {
            FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x8098a54c);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xb94139f4, 0x649f4a65);
        }
    }
    if (GetCategory() == CC_CUSTOM_HUD) {
        SetHUDTextures();
        SetHUDColors();
    } else {
        if ((RealTimer - ScrollTime).GetSeconds() > 0.3f) {
            gCarCustomizeManager.PreviewPart(part->GetSlotID(), part->GetPart());
        } else {
            bNeedsRefresh = true;
        }
    }
    if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
        FEngSetLanguageHash(GetPackageName(), 0x5e7b09c9, part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0));
    } else {
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", part->GetPart()->GetName());
    }
}

CustomizeSpoiler::CustomizeSpoiler(ScreenConstructorData *sd) : CustomizationScreen(sd) {
    TheFilter = 0;
    for (int i = 0; i < CSF_NUM_FILTERS; i++) {
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
            if ((part != nullptr) && (gCarCustomizeManager.IsPartInCart(part) != nullptr)) {
                part->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
            break;
    }
}

void CustomizeSpoiler::Setup() {
    SetTitleHash(0x94e73021);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x91c4a50), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x2d145be3), 0x682);
    CarPart *installed_part = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_SPOILER);
    if (Showcase::FromFilter != CSF_NO_FILTER) {
        TheFilter = Showcase::FromFilter;
        Showcase::FromFilter = CSF_NO_FILTER;
    } else if (installed_part != nullptr) {
        if (installed_part->GetGroupNumber() != 4) {
            TheFilter = installed_part->GetGroupNumber();
        }
    }
    BuildPartOptionListFromFilter(installed_part);
    RefreshHeader();
}

void CustomizeSpoiler::BuildPartOptionListFromFilter(CarPart *installed) {
    Options.RemoveAll();
    Options.AddInitialBookEnds();
    bTList<SelectablePart> part_list;
    gCarCustomizeManager.GetCarPartList(CARSLOTID_SPOILER, part_list, 0);
    SelectablePart *part = static_cast<SelectablePart *>(part_list.GetHead());
    uint32 current_part_index = 1;
    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        if (part->GetPart()->GetGroupNumber() == TheFilter || part->GetPart()->GetGroupNumber() == 4) {
            uint32 unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), part->GetUpgradeLevel());
            uint32 icon = 0xbb034ea6;
            if (gCarCustomizeManager.IsInBackRoom()) {
                icon = 0xc51a4f62;
            }
            if (part->GetPart()->GetAppliedAttributeIParam(bStringHash("CARBONFIBRE"), 0) != 0) {
                icon = 0x4d1c18ba;
                if (gCarCustomizeManager.IsInBackRoom()) {
                    icon = 0x611d142a;
                }
            }
            AddPartOption(part, icon, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
            if (SelectedIndex[TheFilter] == 1) {
                if ((installed != nullptr) && part->GetPart() == installed) {
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
    switch (TheFilter) {
        case CSF_SPORT:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x1f0e2b2);
            break;
        case CSF_TUNER:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x205b328);
            break;
        case CSF_SPORT_CARBON:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0x9912746);
            break;
        case CSF_TUNER_CARBON:
            FEngSetLanguageHash(GetPackageName(), 0x78008599, 0xe7416fc);
            break;
    }
    SelectablePart *part = GetSelectedPart();
    if (((RealTimer - ScrollTime) > Timer(0.3f)) != 0) {
        gCarCustomizeManager.PreviewPart(part->GetSlotID(), part->GetPart());
    } else {
        bNeedsRefresh = true;
    }
    if (part->GetPart()->HasAppliedAttribute(bStringHash("LANGUAGEHASH"))) {
        FEngSetLanguageHash(GetPackageName(), 0x5e7b09c9, part->GetPart()->GetAppliedAttributeUParam(bStringHash("LANGUAGEHASH"), 0));
    } else {
        FEPrintf(GetPackageName(), 0x5e7b09c9, "%s", part->GetPart()->GetName());
    }
}

void CustomizeSpoiler::ScrollFilters(eScrollDir dir) {
    int filter = TheFilter;
    if (dir == eSD_PREV) {
        filter--;
        if (filter < 0) {
            filter = 3;
        }
    } else if (dir == eSD_NEXT) {
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
    if (msg == __PAD_LEFT__ || msg == __PAD_RIGHT__) {
        HUDLayerOption *opt = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
        opt->SelectedPart = SelectedColor->ThePart;
    }
    if (msg != 0x91dfdf84) {
        CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    }
    switch (msg) {
        case __PAD_UP__:
            ScrollColors(eSD_PREV);
            break;
        case __PAD_DOWN__:
            ScrollColors(eSD_NEXT);
            break;
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            BuildColorOptions();
            RefreshHeader();
            break;
        case 0xcf91aacd:
            gCarCustomizeManager.ClearTempColoredPart();
            bTexturesNeedUnload = true;
            break;
        case 0x91dfdf84: {
            ShoppingCartItem *cart_item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_CUSTOM_HUD);
            if ((cart_item != nullptr) && gCarCustomizeManager.GetTempColoredPart()->GetPart() != cart_item->GetBuyingPart()->GetPart()) {
                for (int i = CARSLOTID_HUD_BACKING_COLOUR; i <= CARSLOTID_HUD_CHARACTER_COLOUR; i++) {
                    gCarCustomizeManager.RemoveFromCart(gCarCustomizeManager.IsPartTypeInCart(i));
                }
            }
            gCarCustomizeManager.AddToCart(gCarCustomizeManager.GetTempColoredPart());
            gCarCustomizeManager.ClearTempColoredPart();
            for (HUDLayerOption *layer = static_cast<HUDLayerOption *>(Options.GetHead()); !Options.IsEndOfList(layer);
                 layer = static_cast<HUDLayerOption *>(layer->GetNext())) {
                if (layer->SelectedPart != nullptr) {
                    gCarCustomizeManager.AddToCart(layer->SelectedPart);
                }
            }
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeHudPkg, GetCategory() | CC_MAKE_HIWORD(GetFromCategory()), 0, false);
            break;
        }

        case __PAD_BACK__:
            gCarCustomizeManager.ClearTempColoredPart();
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeHudPkg, GetCategory() | CC_MAKE_HIWORD(GetFromCategory()), 0, false);
            break;
    }
}

// UNSOLVED
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
        FEngSetScript(prev->FEngObject, 0x7ab5521a, true);
        FEngSetScript(SelectedColor->FEngObject, 0x249db7b7, true);
        float x_offset = 69.0f;
        float y_offset = 56.0f;
        FEngSetTopLeft(Cursor, x_offset + FEngGetTopLeftX(SelectedColor->FEngObject), y_offset + FEngGetTopLeftY(SelectedColor->FEngObject));
        RefreshHeader();
    }
}

void CustomizeHUDColor::AddLayerOption(uint32 layer, uint32 icon_hash, uint32 name_hash) {
    AddOption(new ("HUDLayerOption", 0) HUDLayerOption(layer, icon_hash, name_hash));
}

void CustomizeHUDColor::Setup() {
    DisplayHelper.SetTitleHash(0xb1b0e8af);
    AddLayerOption(CARSLOTID_HUD_NEEDLE_COLOUR, 0x70f56628, 0xe18ddce1);
    AddLayerOption(CARSLOTID_HUD_CHARACTER_COLOUR, 0xbf6682c9, 0xe18ddce0);
    AddLayerOption(CARSLOTID_HUD_BACKING_COLOUR, 0xcc9e1ce4, 0xe18ddcdf);
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
    ShoppingCartItem *cart_item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_CUSTOM_HUD);
    CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_CUSTOM_HUD);
    uint32 colors[3] = {CHT_DEFAULT_FILL_COLOR, CHT_DEFAULT_NEEDLE_COLOR, CHT_DEFAULT_LINE_COLOR};
    if ((cart_item != nullptr) && gCarCustomizeManager.GetTempColoredPart()->GetPart() == cart_item->GetBuyingPart()->GetPart()) {
        uint32 carslot_id = 0x85;
        for (int i = 0; i < 3; i++) {
            ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(carslot_id);
            carslot_id++;
            if (item != nullptr) {
                CarPart *color_part = item->GetBuyingPart()->GetPart();
                uint8 r = color_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                uint8 g = color_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                uint8 b = color_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                colors[i] = 0xff000000 | (r << 16) | (g << 8) | b;
            }
        }
    } else if (gCarCustomizeManager.GetTempColoredPart()->GetPart() == installed_hud) {
        uint32 carslot_id = 0x85;
        for (int i = 0; i < 3; i++) {
            CarPart *color_part = gCarCustomizeManager.GetInstalledCarPart(carslot_id);
            carslot_id++;
            if (color_part != nullptr) {
                uint8 r = color_part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                uint8 g = color_part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                uint8 b = color_part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                colors[i] = 0xff000000 | (r << 16) | (g << 8) | b;
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
    if (rpm > 9500) {
        rpm = 10000;
    } else if (rpm > 8500) {
        rpm = 9000;
    } else if (rpm > 7500) {
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
    if (SelectedColor != nullptr) {
        FEngSetScript(SelectedColor->FEngObject, 0x7ab5521a, true);
        SelectedColor = nullptr;
    }

    HUDLayerOption *opt = static_cast<HUDLayerOption *>(Options.GetCurrentOption());
    if ((opt != nullptr) && !opt->TheColors.IsEmpty()) {
        ColorOptions.DeleteAllElements();
        ShoppingCartItem *cart_item = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_CUSTOM_HUD);
        CarPart *installed_hud = gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_CUSTOM_HUD);
        int i = 0;
        for (SelectablePart *part = opt->TheColors.GetHead(); part != opt->TheColors.EndOfList(); part = part->GetNext()) {
            i++;
            HUDColorOption *color_option = new ("HUDColorOption", 0) HUDColorOption(part);
            FEImage *obj = FEngFindImage(GetPackageName(), FEngHashString("COLOR_%d", i));
            color_option->SetFEngObject(obj);
            ColorOptions.AddTail(color_option);
            uint8 r = part->GetPart()->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            uint8 g = part->GetPart()->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            uint8 b = part->GetPart()->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);

            color_option->color = 0xff000000 | (r << 16) | (g << 8) | b;
            FEngSetColor(obj, color_option->color);
            if (opt->SelectedPart == nullptr) {
                if ((cart_item != nullptr) && gCarCustomizeManager.GetTempColoredPart()->GetPart() == cart_item->GetBuyingPart()->GetPart()) {
                    if (gCarCustomizeManager.IsPartInCart(part) != nullptr) {
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
        if (SelectedColor == nullptr) {
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
        case __PAD_BUTTON0__:
            Showcase::FromFilter = InnerRadius;
            break;
        case __PAD_LTRIGGER__:
            ScrollRimSizes(eSD_PREV);
            break;
        case __PAD_RTRIGGER__:
            ScrollRimSizes(eSD_NEXT);
            break;
        case FEMSG_BACK_OUT: {
            SelectablePart *part = FindInCartPart();
            if ((part != nullptr) && (gCarCustomizeManager.IsPartInCart(part) == nullptr)) {
                part->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case __PAD_ACCEPT__:
            break;
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
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
    uint32 unlock_hash; // TODO which hash?
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x91c4a50), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x2d145be3), 0x682);
    SetTitleHash(0xe167f7c8);
    InnerRadius = gCarCustomizeManager.GetMinInnerRadius();
    MinRadius = InnerRadius;
    MaxRadius = gCarCustomizeManager.GetMaxInnerRadius();
    CarPart *installed_part = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_FRONT_WHEEL);
    if (Showcase::FromFilter != -1) {
        InnerRadius = Showcase::FromFilter;
        Showcase::FromFilter = -1;
    } else if (installed_part != nullptr) {
        InnerRadius = installed_part->GetInnerRadius();
    }
    BuildRimsList(-1);
    RefreshHeader();
}

void CustomizeRims::BuildRimsList(int selected_index) {
    Options.RemoveAll();
    Options.AddInitialBookEnds();
    bTList<SelectablePart> part_list;
    gCarCustomizeManager.GetCarPartList(CARSLOTID_FRONT_WHEEL, part_list, GetCategoryBrandHash());
    SelectablePart *part = part_list.GetHead();
    int installed_index = 0;
    int current_part_index = 1;
    uint32 unlock_hash;
    CarPart *installed_part = nullptr;
    if (selected_index == -1) {
        installed_part = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_FRONT_WHEEL);
    }
    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        CarPart *carpet = part->GetPart();
        if (carpet->GetInnerRadius() == InnerRadius) {
            uint32 unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), part->GetUpgradeLevel());
            AddPartOption(part, 0x294d2a3, part->GetPart()->GetUpgradeLevel(), 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
            if (installed_part != nullptr && part->GetPart() == installed_part) {
                installed_index = current_part_index;
            }
            current_part_index++;
        } else {
            delete part;
        }
    }
    if (selected_index == -1) {
        selected_index = 1;
        if (installed_part != nullptr) {
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
    switch (GetCategory()) {
        case CC_RIM_BRAND_5_ZIGEN:
            return 0x352d08d1;
        case CC_RIM_BRAND_ADR:
            return 0x9136;
        case CC_RIM_BRAND_BBS:
            return 0x9536;
        case CC_RIM_BRAND_ENKEI:
            return 0x2b77feb;
        case CC_RIM_BRAND_KONIG:
            return 0x324ac97;
        case CC_RIM_BRAND_LOWENHART:
            return 0x48e25793;
        case CC_RIM_BRAND_RACING_HART:
            return 0xdd544a02;
        case CC_RIM_BRAND_OZ:
            return 0x648;
        case CC_RIM_BRAND_VOLK:
            return 0x1e6a3b;
        case CC_RIM_BRAND_ROJA:
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
    for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
        VinylColors[i] = nullptr;
    }
    ThePaints.SetMouseDownMsg(__PAD_ACCEPT__);
    Setup();
}

eMenuSoundTriggers CustomizePaint::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (maybe == UISND_CUST_PAINT_TYPE_LEFT || maybe == UISND_CUST_PAINT_TYPE_RIGHT) {
        if (GetCategory() == CC_RIM_PAINT) {
            return UISND_NONE;
        }
        if (gCarCustomizeManager.GetTempColoredPart() != nullptr) {
            if (gCarCustomizeManager.GetTempColoredPart()->GetPart() != nullptr &&
                gCarCustomizeManager.GetTempColoredPart()->GetPart()->GetAppliedAttributeUParam(0x6212682b, 0) < 2) {
                return UISND_NONE;
            }
        }
    }
    return maybe;
}

void CustomizePaint::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            break;
        case __PAD_ACCEPT__:
            if (GetCategory() == CC_PAINT || GetCategory() == CC_RIM_PAINT) {
                CustomizationScreen::NotificationMessage(__PAD_ACCEPT__, pobj, param1, param2);
            }
            break;
        default:
            CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
            break;
    }

    ThePaints.NotificationMessage(msg, pobj, param1, param2);

    switch (msg) {
        case __PAD_BUTTON0__:
            Showcase::FromFilter = TheFilter;
            Showcase::FromIndex = ThePaints.GetCurrentDatumNum();
#ifndef EA_BUILD_A124
            for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
                Showcase::FromColor[i] = VinylColors[i];
            }
#endif
            break;
        case __PAD_LTRIGGER__:
            ScrollFilters(eSD_PREV);
            break;
        case __PAD_RTRIGGER__:
            ScrollFilters(eSD_NEXT);
            break;

        case __PAD_ACCEPT__: {
            if (GetCategory() == CC_PAINT || GetCategory() == CC_RIM_PAINT) {
                break;
            }

            if (VinylColors[TheFilter] != nullptr) {
                delete VinylColors[TheFilter];
            }

            VinylColors[TheFilter] = new ("SelectablePart 3", 0) SelectablePart(GetSelectedPart());
            CarPart *installed = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_VINYL_LAYER0);
            CarPart *temp_colorable = gCarCustomizeManager.GetTempColoredPart()->GetPart();
            bool add_to_cart = false;
            if (installed != temp_colorable) {
                add_to_cart = true;
            } else {
                for (int i = 0; i < NumRemapColors; i++) {
                    installed = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_FROM_VINYL_COLOUR_INDEX(i));
                    if (VinylColors[i] != nullptr && installed != VinylColors[i]->GetPart()) {
                        add_to_cart = true;
                        break;
                    }
                }
            }
            if (add_to_cart) {
                AddVinylAndColorsToCart();

                cFEng::Get()->QueuePackageSwitch(g_pCustomizePartsPkg, GetCategory() | CC_MAKE_HIWORD(GetFromCategory()), 0, false);
            }
        }

        break;

        case __PAD_BACK__:
            switch (GetCategory()) {
                case CC_PAINT:
                case CC_RIM_PAINT:
                    cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
                    break;

                default:
                    for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
                        if (VinylColors[i] != nullptr) {
                            delete VinylColors[i];
                        }
                        VinylColors[i] = nullptr;
#ifndef EA_BUILD_A124
                        Showcase::FromColor[i] = nullptr;
#endif
                    }

                    gCarCustomizeManager.ResetPreview();

                    cFEng::Get()->QueuePackageSwitch(g_pCustomizePartsPkg, GetCategory() | CC_MAKE_HIWORD(GetFromCategory()), 0, false);
                    break;
            }

            break;
        case 0x5a928018: {
            SelectablePart *part = FindInCartPart();
            if ((part != nullptr) && (gCarCustomizeManager.IsPartInCart(part) == nullptr)) {
                part->UnSetInCart();
                RefreshHeader(); // TODO: double refresh header?
            }
            RefreshHeader();
            break;
        }
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
        case __PAD_DOWN__:
        case __PAD_UP__:
            RefreshHeader();
            break;
        case 0xcf91aacd:
            for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
                if (VinylColors[i] != nullptr) {
                    delete VinylColors[i];
                }
                VinylColors[i] = nullptr;
            }
            break;
    }
}

SelectablePart *CustomizePaint::FindInCartPart() {
    CustomizePaintDatum *match = nullptr;
    int num_datum = ThePaints.GetNumDatum();
    for (int i = 0; i < num_datum; i++) {
        CustomizePaintDatum *iter = static_cast<CustomizePaintDatum *>(ThePaints.GetDatumAt(i));
        if (iter->ThePart->IsInCart()) {
            return iter->ThePart;
        }
    }
    return nullptr;
}

CustomizePartOption *CustomizePaint::FindMatchingOption(SelectablePart *to_find) {
    CustomizePaintDatum *match = nullptr;
    int num_datum = ThePaints.GetNumDatum();
    for (int i = 0; i < num_datum; i++) {
        CustomizePaintDatum *iter = static_cast<CustomizePaintDatum *>(ThePaints.GetDatumAt(i));
        if (iter->ThePart->GetPart() == to_find->GetPart()) {
            match = iter;
            break;
        }
    }
    if (match != nullptr) {
        MatchingPaint.SetPart(match->ThePart);
        return &MatchingPaint;
    } else {
        return nullptr;
    }
}

void CustomizePaint::AddVinylAndColorsToCart() {
    SelectablePart *temp_colored = gCarCustomizeManager.GetTempColoredPart();
    gCarCustomizeManager.AddToCart(temp_colored);
    for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
        if (i < NumRemapColors && VinylColors[i] != nullptr) {
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
    switch (GetCategory()) {
        case CC_PAINT:
            max = 2;
            break;
        case CC_RIM_PAINT:
            return;
        default:
            max = NumRemapColors - 1;
            if (max != 0) {
                if (GetSelectedPart() != VinylColors[TheFilter]) {
                    if (VinylColors[TheFilter] != nullptr) {
                        delete VinylColors[TheFilter];
                    }
                    VinylColors[TheFilter] = new ("SelectablePart 4", 0) SelectablePart(GetSelectedPart());
                }
            }
            break;
    }

    int filter = TheFilter;
    if (dir == eSD_PREV) {
        filter--;
        if (filter < 0) {
            filter = max;
        }
    } else if (dir == eSD_NEXT) {
        filter++;
        if (filter > max) {
            filter = 0;
        }
    }

    if (filter != TheFilter) {
        SelectedIndex[TheFilter] = ThePaints.GetCurrentDatumNum() - 1;
        TheFilter = filter;
        switch (GetCategory()) {
            case CC_PAINT:
            case CC_RIM_PAINT:
                BuildSwatchList(GetSelectedPart()->GetSlotID());
                break;
            default:
                BuildSwatchList(CARSLOTID_FROM_VINYL_COLOUR_INDEX(filter));
                break;
        }
        RefreshHeader();
    }
}

void CustomizePaint::Setup() {
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x91c4a50), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x2d145be3), 0x682);
    for (int i = 1; i <= CP_NUM_PAINT_SWATCHES; i++) {
        ThePaints.AddSlot(new ("ArraySlot", 0) ArraySlot(FEngFindImage(GetPackageName(), FEngHashString("COLOR_%d", i))));
    }
    for (int i = 0; i < CP_NUM_BASE_PAINT_FILTERS; i++) {
        SelectedIndex[i] = -1;
    }
    if (Showcase::FromFilter != -1) {
        TheFilter = Showcase::FromFilter;
    }
    switch (GetCategory()) {
        case CC_PAINT: {
            const u32 FEObj_TRIGGER = 0x1a7240f3;
            cFEng::Get()->QueuePackageMessage(FEObj_TRIGGER, GetPackageName(), nullptr);
            DisplayHelper.SetTitleHash(0x55da70c);
            SetupBasePaint();
            break;
        }
        case CC_RIM_PAINT:
            DisplayHelper.SetTitleHash(0xe126ff53);
            SetupRimPaint();
            break;
        case CC_VINYL_GROUP_FLAME:
        case CC_VINYL_GROUP_TRIBAL:
        case CC_VINYL_GROUP_STRIPE:
        case CC_VINYL_GROUP_RACING_FLAG:
        case CC_VINYL_GROUP_NATIONAL_FLAG:
        case CC_VINYL_GROUP_BODY:
        case CC_VINYL_GROUP_UNIQUE:
        case CC_VINYL_GROUP_CONTEST:
            DisplayHelper.SetTitleHash(0xd8ee1a80);
            SetupVinylColor();
            break;
    }
    Showcase::FromFilter = -1;
    Options.SetInitialized();
    RefreshHeader();
}

void CustomizePaint::SetupBasePaint() {
    BuildSwatchList(CARSLOTID_BASE_PAINT);
}

void CustomizePaint::SetupRimPaint() {
    FEngSetInvisible(GetPackageName(), 0x2C3CC2D3);
    FEngSetInvisible(GetPackageName(), 0x53639A10);
    BuildSwatchList(CARSLOTID_PAINT_RIM);
}

void CustomizePaint::SetupVinylColor() {
    uint32 start_slot_id = CARSLOTID_VINYL_COLOUR0_0;
    if (Showcase::FromFilter != -1) {
        switch (Showcase::FromFilter) {
            case 0:
                start_slot_id = CARSLOTID_VINYL_COLOUR0_0;
                break;
            case 1:
                start_slot_id = CARSLOTID_VINYL_COLOUR0_1;
                break;
            case 2:
                start_slot_id = CARSLOTID_VINYL_COLOUR0_2;
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

    for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
        int slot_id = CARSLOTID_FROM_VINYL_COLOUR_INDEX(i);
#ifndef EA_BUILD_A124
        if (Showcase::FromColor[i] != nullptr) {
            VinylColors[i] = static_cast<SelectablePart *>(Showcase::FromColor[i]);
            Showcase::FromColor[i] = nullptr;
        } else {
#endif
            CarPart *part = gCarCustomizeManager.GetActivePartFromSlot(slot_id);
            if (part != nullptr) {
                VinylColors[i] = new ("SelectablePart 5", 0)
                    SelectablePart(part, slot_id, part->GetUpgradeLevel(), Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
            } else {
                VinylColors[i] = nullptr;
            }
#ifndef EA_BUILD_A124
        }
#endif
    }
}

uint32 CustomizePaint::CalcBrandHash(CarPart *part) {
    uint32 brand_hash;
    switch (GetCategory()) {
        case CC_PAINT:
            switch (TheFilter) {
                case CP_BASE_PAINT_FILTER_GLOSS:
                    brand_hash = 0x02daab07;
                    break;
                case CP_BASE_PAINT_FILTER_METALLIC:
                    brand_hash = 0x03437a52;
                    break;
                case CP_BASE_PAINT_FILTER_PEARL:
                    brand_hash = 0x03797533;
                    break;
                default:
                    brand_hash = part->GetBrandNameHash();
                    break;
            }
            break;

        case CC_RIM_PAINT:
            brand_hash = 0xda27;
            break;

        default:
            brand_hash = 0x3e871f1;
            break;
    }

    return brand_hash;
}

void CustomizePaint::BuildSwatchList(uint32 slot_id) {
    ThePaints.ClearData();

#ifdef EA_BUILD_A124
    CarPart *installed_part = gCarCustomizeManager.GetActivePartFromSlot(slot_id);
#else
    CarPart *installed_part = nullptr;
    switch (slot_id) {
        case CARSLOTID_VINYL_COLOUR0_0:
        case CARSLOTID_VINYL_COLOUR0_1:
        case CARSLOTID_VINYL_COLOUR0_2: {
            int showcase_index_num = 0;
            switch (slot_id) {
                case CARSLOTID_VINYL_COLOUR0_0:
                    showcase_index_num = CP_VINYL_PAINT_LAYER_1;
                    break;
                case CARSLOTID_VINYL_COLOUR0_1:
                    showcase_index_num = CP_VINYL_PAINT_LAYER_2;
                    break;
                case CARSLOTID_VINYL_COLOUR0_2:
                    showcase_index_num = CP_VINYL_PAINT_LAYER_3;
                    break;
            }
            if ((Showcase::FromColor[showcase_index_num] != nullptr) && (VinylColors[showcase_index_num] == nullptr)) {
                installed_part = static_cast<SelectablePart *>(Showcase::FromColor[showcase_index_num])->GetPart();
            }
            break;
        }
    }

    if (installed_part == nullptr) {
        installed_part = gCarCustomizeManager.GetActivePartFromSlot(slot_id);
    }
#endif

    uint32 brand_hash = CalcBrandHash(installed_part);

    if (TheFilter == -1) {
        switch (brand_hash) {
            case 0x2daab07:
            case 0xda27:
                TheFilter = CP_BASE_PAINT_FILTER_GLOSS;
                break;
            case 0x3437a52:
                TheFilter = CP_BASE_PAINT_FILTER_METALLIC;
                break;
            case 0x3797533:
                TheFilter = CP_BASE_PAINT_FILTER_PEARL;
                break;
            default:
                TheFilter = CP_BASE_PAINT_FILTER_GLOSS;
                break;
        }
    }

    bTList<SelectablePart> the_list;
    gCarCustomizeManager.GetCarPartList(slot_id, the_list, 0);
    int selected_index = 0;
    while (!the_list.IsEmpty()) {
        if (the_list.GetHead()->GetPart()->GetBrandNameHash() == brand_hash) {
            uint32 unlock_hash =
                gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), the_list.GetHead()->GetPart()->GetUpgradeLevel());
            CustomizePaintDatum *datum = new ("CustomizePaintDatum", 0) CustomizePaintDatum(the_list.RemoveHead(), unlock_hash);
            if (SelectedIndex[TheFilter] == -1 && installed_part == datum->ThePart->GetPart()) {
                SelectedIndex[TheFilter] = selected_index;
            }
            ThePaints.AddDatum(datum);
            ImageArraySlot *slot = static_cast<ImageArraySlot *>(ThePaints.GetSlotAt(selected_index));
            if (slot != nullptr) {
                uint8 r = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("RED"), 0);
                uint8 g = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
                uint8 b = datum->ThePart->GetPart()->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
                uint32 color = 0xff000000 | r << 16 | g << 8 | b;
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

    uint32 filter_hash = 0;
    switch (TheFilter) {
        case CP_BASE_PAINT_FILTER_GLOSS:
            if (GetCategory() == CC_PAINT) {
                filter_hash = 0xb6763cde;
            } else if (NumRemapColors == 2) {
                filter_hash = 0x5198ba16;
            } else if (NumRemapColors == 3) {
                filter_hash = 0x5198ba17;
            } else {
                filter_hash = 0xd8ee1a80;
            }
            break;
        case CP_BASE_PAINT_FILTER_METALLIC:
            if (GetCategory() == CC_PAINT) {
                filter_hash = 0x452b5481;
            } else if (NumRemapColors == 2) {
                filter_hash = 0x5198be57;
            } else if (NumRemapColors == 3) {
                filter_hash = 0x5198be58;
            }
            break;
        case CP_BASE_PAINT_FILTER_PEARL:
            if (GetCategory() == CC_PAINT) {
                filter_hash = 0xb715070a;
            } else if (NumRemapColors == 3) {
                filter_hash = 0x5198c299;
            }
            break;
    }
    FEngSetLanguageHash(GetPackageName(), 0x78008599, filter_hash);

    switch (GetCategory()) {
        case CC_PAINT:
            gCarCustomizeManager.PreviewPart(GetSelectedPart()->GetSlotID(), GetSelectedPart()->GetPart());
            break;
        case CC_RIM_PAINT:
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
            for (int i = 0; i < CP_VINYL_NUM_COLORS; i++) {
                if (i < NumRemapColors && (VinylColors[i] != nullptr)) {
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
        case __PAD_BUTTON0__:
            Showcase::FromFilter = static_cast<int32>(bIsBlack);
            break;
        case __PAD_LTRIGGER__:
        case __PAD_RTRIGGER__:
            bIsBlack = !bIsBlack;

            if (GetSelectedPart()->GetPart() != nullptr) {
                BuildDecalList(GetSelectedPart()->GetPart()->GetBrandNameHash());
            } else {
                BuildDecalList(0);
            }

            RefreshHeader();
            break;
        case 0x5a928018: {
            SelectablePart *part = FindInCartPart();
            if (part != nullptr) {
                if (gCarCustomizeManager.IsPartInCart(part) != nullptr) {
                    return;
                }
                part->UnSetInCart();
                RefreshHeader();
            }
            break;
        }
        case __PAD_BACK__:
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubTopPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
            break;
        case __PAD_BUTTON4__:
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
            switch (GetCategory()) {
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
            switch (GetCategory()) {
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
            // fall through
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

    if (GetSelectedPart()->GetPart() != nullptr) {
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
    ScrollTime.ResetLow();
    bNeedsRefresh = true;
    Options.RemoveAll();
    Options.AddInitialBookEnds();

    uint32 slot_id = GetSlotIDFromCategory();
    SelectablePart *removal_part =
        new ("SelectablePart", 0) SelectablePart(nullptr, slot_id, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
    uint32 status = CPS_AVAILABLE;
    if (gCarCustomizeManager.IsPartInstalled(removal_part)) {
        status = CPS_INSTALLED | CPS_AVAILABLE;
    } else if (gCarCustomizeManager.IsPartInCart(removal_part) != nullptr) {
        status = CPS_IN_CART | CPS_AVAILABLE;
    }
    removal_part->SetPartState(status);
    AddPartOption(removal_part, 0x697b4ad4, 0x60a662f5, 0, 0, false);

    bTList<SelectablePart> the_list;
    gCarCustomizeManager.GetCarPartList(slot_id, the_list, 0);

    int level = MapCarPartToUnlockable(slot_id, nullptr);
    switch (level) {
        case UNLOCKABLE_DECAL_WINDSHIELD:
            level = 1;
            break;
        case UNLOCKABLE_DECAL_LEFT_DOOR:
            level = 2;
            break;
        case UNLOCKABLE_DECAL_LEFT_QP:
            level = 3;
            break;
    }

    int i = 2;
    int index = 1;
    while (!the_list.IsEmpty()) {
        uint32 name_hash = the_list.GetHead()->GetPart()->GetBrandNameHash();
        if (!bIsBlack) {
            name_hash = bStringHash("_WHITE", name_hash);
        }
        if (name_hash == the_list.GetHead()->GetPart()->GetAppliedAttributeUParam(bStringHash("NAME"), 0)) {
            uint32 unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), level);
            SelectablePart *part = the_list.RemoveHead();
            AddPartOption(part, 0x697b4ad4, name_hash, 0, unlock_hash, gCarCustomizeManager.IsPartLocked(part, 0));
            if (part->GetPart()->GetBrandNameHash() == selected_name_hash) {
                index = i;
            }
            i++;
        } else {
            delete the_list.RemoveHead();
        }
    }

    if (Showcase::FromIndex != 0) {
        SetInitialOption(Showcase::FromIndex);
        Showcase::FromIndex = 0;
    } else {
        SetInitialOption(index);
    }
}

void CustomizeDecals::Setup() {
    uint32 slot_id = GetSlotIDFromCategory();

    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x91c4a50), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x2d145be3), 0x682);

    uint32 title = 0;
    switch (GetCategory()) {
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
    SetTitleHash(title);

    uint32 installed_part_base_name = 0;
    ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(slot_id);

    if ((item != nullptr && item->GetBuyingPart()->GetPart() != nullptr)) {
        CarPart *buying = item->GetBuyingPart()->GetPart();

        uint32 name_hash = buying->GetBrandNameHash();
        bIsBlack = (name_hash == buying->GetAppliedAttributeUParam(bStringHash("NAME"), 0));
        installed_part_base_name = buying->GetBrandNameHash();

    } else {
        CarPart *installed = gCarCustomizeManager.GetInstalledCarPart(slot_id);
        if (installed != nullptr) {
            uint32 name_hash = installed->GetBrandNameHash();
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
      bLeft(true),             //
      DisplayHelper(sd->PackageFilename) {
    Category = sd->Arg & CC_TO_CAT_MASK;
    FromCategory = CC_GET_HIWORD(sd->Arg);
    Setup();
}

// UNSOLVED: FEngSetCurrentButton
void CustomizeNumbers::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_INITCOMPLETE:
            DisplayHelper.SetInitComplete(true);
            FEngSetCurrentButton(GetPackageName(), 0x2a08ba92);
            break;
        case __PAD_BUTTON0__:
            Showcase::FromFilter = RightDisplayValue;
            Showcase::FromIndex = LeftDisplayValue;
            Showcase::FromArgs = Category | CC_MAKE_HIWORD(FromCategory);
            Showcase::FromPackage = GetPackageName();
            bShowcaseOn = true;
            cFEng::Get()->QueuePackageSwitch("Showcase.fng", reinterpret_cast<uint32>(gCarCustomizeManager.GetTuningCar()), 0, false);
            break;
        case __PAD_START__:
            CustomizeShoppingCart::ShowShoppingCart(GetPackageName());
            break;
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            bLeft = !bLeft;
            FEngSetCurrentButton(GetPackageName(), bLeft ? 0x2a08ba92 : 0x1a88dc05);
            break;
        case __PAD_UP__:
            ScrollNumbers(eSD_NEXT);
            break;
        case __PAD_DOWN__:
            ScrollNumbers(eSD_PREV);
            break;
        case __PAD_ACCEPT__:
            if (LeftDisplayValue == -1 || RightDisplayValue == -1) {
                return;
            }
            if ((TheLeftNumber == nullptr) || (TheRightNumber == nullptr)) {
                return;
            }
            if (TheLeftNumber->IsLocked() && TheRightNumber->IsLocked()) {
                DisplayHelper.PlayLocked();
            } else if (TheLeftNumber->IsInCartX() && TheRightNumber->IsInCartX()) {
                DisplayHelper.PlayInCart();
            } else if (TheLeftNumber->IsInstalledX() && TheRightNumber->IsInstalledX()) {
                DisplayHelper.PlayInstalled();
            } else {
                cFEng::Get()->QueueGameMessage(0x91dfdf84, GetPackageName(), 0xff);
            }
            break;
        case __PAD_BUTTON4__:
            if (gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER) != nullptr) {
                UnsetShoppingCart();
                SelectablePart remover(nullptr, CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER, 0, Physics::Upgrades::PUT_MAX, false, CPS_AVAILABLE, 0, false);
                gCarCustomizeManager.AddToCart(&remover);
                remover.SetSlotID(CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER);
                gCarCustomizeManager.AddToCart(&remover);
                remover.SetSlotID(CARSLOTID_DECAL_LEFT_DOOR_LNUMBER);
                gCarCustomizeManager.AddToCart(&remover);
                remover.SetSlotID(CARSLOTID_DECAL_LEFT_DOOR_RNUMBER);
                gCarCustomizeManager.AddToCart(&remover);
            } else {
                if (TheLeftNumber->IsInCart() || TheRightNumber->IsInCart()) {
                    UnsetShoppingCart();
                    ShoppingCartItem *current = gCarCustomizeManager.GetFirstCartItem();
                    ShoppingCartItem *last = gCarCustomizeManager.GetLastCartItem();
                    while (current != nullptr) {
                        ShoppingCartItem *next = current->GetNext();
                        SelectablePart *part = current->GetBuyingPart();
                        if (part->GetSlotID() == CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER || part->GetSlotID() == CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER ||
                            part->GetSlotID() == CARSLOTID_DECAL_LEFT_DOOR_LNUMBER || part->GetSlotID() == CARSLOTID_DECAL_LEFT_DOOR_RNUMBER) {
                            gCarCustomizeManager.RemoveFromCart(current);
                        }
                        if (current == last)
                            break;
                        current = next;
                    }
                }
            }

            LeftDisplayValue = -1;
            RightDisplayValue = -1;
            TheLeftNumber = LeftNumberList.GetHead();
            TheRightNumber = RightNumberList.GetHead();

            RefreshHeader();
            break;

        case 0x91dfdf84: {
            UnsetShoppingCart();
            TheLeftNumber->SetInCartPreserve();
            TheRightNumber->SetInCartPreserve();
            gCarCustomizeManager.AddToCart(TheLeftNumber);
            gCarCustomizeManager.AddToCart(TheRightNumber);
            SelectablePart *driver_left = new ("SelectablePart 6", 0) SelectablePart(TheLeftNumber);
            SelectablePart *driver_right = new ("SelectablePart 7", 0) SelectablePart(TheRightNumber);
            driver_left->SetPrice(0);
            driver_right->SetPrice(0);
            driver_left->SetInCart();
            driver_right->SetInCart();
            driver_left->SetSlotID(CARSLOTID_DECAL_LEFT_DOOR_LNUMBER);
            driver_right->SetSlotID(CARSLOTID_DECAL_LEFT_DOOR_RNUMBER);
            gCarCustomizeManager.AddToCart(driver_left);
            gCarCustomizeManager.AddToCart(driver_right);
            delete driver_left;
            delete driver_right;
            RefreshHeader();
            break;
        }
        case 0xcf91aacd: {
            SelectablePart *le_monde = LeftNumberList.GetHead();
            while (le_monde != LeftNumberList.EndOfList()) {
                if (le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_DECAL_LEFT_DOOR_LNUMBER) ||
                    le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_DECAL_LEFT_DOOR_RNUMBER)) {
                    le_monde->UnSetInCart();
                    le_monde->SetInstalled();
                }
                le_monde = le_monde->GetNext();
            }

            le_monde = RightNumberList.GetHead();
            while (le_monde != RightNumberList.EndOfList()) {
                if (le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER) ||
                    le_monde->GetPart() == gCarCustomizeManager.GetInstalledCarPart(CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER)) {
                    le_monde->UnSetInCart();
                    le_monde->SetInstalled();
                }
                le_monde = le_monde->GetNext();
            }
            CustomizeShoppingCart::ExitShoppingCart();
            break;
        }
        case 0x5a928018: {
            ShoppingCartItem *tex_6 = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_DECAL_LEFT_DOOR_TEX6);
            ShoppingCartItem *tex_7 = gCarCustomizeManager.IsPartTypeInCart(CARSLOTID_DECAL_LEFT_DOOR_TEX7);
            if ((tex_6 == nullptr) && (tex_7 == nullptr)) {
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
        case __PAD_BACK__:
            bShowcaseOn = false;
            cFEng::Get()->QueuePackageSwitch(g_pCustomizeSubPkg, GetFromCategory() | CC_MAKE_HIWORD(GetCategory()), 0, false);
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

// UNSOLVED
void CustomizeNumbers::ScrollNumbers(eScrollDir dir) {
    if (LeftDisplayValue == -1 || RightDisplayValue == -1) {
        LeftDisplayValue = 0;
        RightDisplayValue = 0;
        TheLeftNumber = LeftNumberList.GetHead();
        TheRightNumber = RightNumberList.GetHead();
        RefreshHeader();
        return;
    }

    SelectablePart *new_part = bLeft ? TheLeftNumber : TheRightNumber;

    if (dir == eSD_PREV) {
        if (bLeft) {
            new_part = LeftNumberList.GetPrevCircular(new_part);
            LeftDisplayValue--;
            if (LeftDisplayValue < 0) {
                LeftDisplayValue = 9;
            }
        } else {
            new_part = RightNumberList.GetPrevCircular(new_part);
            RightDisplayValue--;
            if (RightDisplayValue < 0) {
                RightDisplayValue = 9;
            }
        }
    } else if (dir == eSD_NEXT) {
        if (bLeft) {
            new_part = LeftNumberList.GetNextCircular(new_part);
            LeftDisplayValue++;
            if (LeftDisplayValue > 9) {
                LeftDisplayValue = 0;
            }
        } else {
            new_part = RightNumberList.GetNextCircular(new_part);
            RightDisplayValue++;
            if (RightDisplayValue > 9) {
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

void CustomizeNumbers::RefreshHeader() {
    DisplayHelper.DrawTitle();
    DisplayHelper.SetCareerStuff(TheLeftNumber, GetCategory(), 0);
    if (LeftDisplayValue != -1 && RightDisplayValue != -1) {
        FEngSetVisible(GetPackageName(), 0x7a8355d9);
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
        DisplayHelper.SetPartStatus(&part, gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), 1), 0, 0);
        FEPrintf(GetPackageName(), 0x2a08ba92, "%$d", LeftDisplayValue);
        FEPrintf(GetPackageName(), 0x1a88dc05, "%$d", RightDisplayValue);
        gCarCustomizeManager.PreviewPart(0x71, TheLeftNumber->GetPart());
        gCarCustomizeManager.PreviewPart(0x72, TheRightNumber->GetPart());
        gCarCustomizeManager.PreviewPart(0x69, TheLeftNumber->GetPart());
        gCarCustomizeManager.PreviewPart(0x6a, TheRightNumber->GetPart());
    } else {
        FEngSetInvisible(GetPackageName(), 0x7a8355d9);
        ShoppingCartItem *inCart = gCarCustomizeManager.IsPartTypeInCart(TheLeftNumber);
        CarPart *part = gCarCustomizeManager.GetInstalledCarPart(0x71);
        if (part == nullptr) {
            DisplayHelper.SetPlayerCarStatusIcon(CPS_INSTALLED);
        } else if ((inCart != nullptr) && (inCart->GetBuyingPart()->GetPart() == nullptr)) {
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
    gCarCustomizeManager.GetCarPartList(CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER, LeftNumberList, 0);
    gCarCustomizeManager.GetCarPartList(CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER, RightNumberList, 0);

    CarPart *installed_part = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_DECAL_RIGHT_DOOR_LNUMBER);
    int i = 0;
    SelectablePart *test_part = LeftNumberList.GetHead();
    bool found = false;
    while (test_part != LeftNumberList.EndOfList()) {
        if (test_part->GetPart()->GetBrandNameHash() == bStringHash("NUMBER_LEFT")) {
            if (!found) {
                if (bShowcaseOn == true && Showcase::FromIndex == i) {
                    TheLeftNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part) != nullptr) {
                        TheLeftNumber->SetInCartPreserve();
                    }
                    LeftDisplayValue = i;
                    Showcase::FromIndex = 0;
                    found = true;
                } else if (test_part->GetPart() == installed_part) {
                    TheLeftNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part) != nullptr) {
                        TheLeftNumber->SetInCartPreserve();
                    }
                    LeftDisplayValue = i;
                }
            }
            i++;
            test_part = test_part->GetNext();
        } else {
            SelectablePart *to_kill = test_part;
            test_part = test_part->GetNext();
            LeftNumberList.Remove(to_kill);
            delete to_kill;
        }
    }

    found = false;

    if (TheLeftNumber == nullptr) {
        LeftDisplayValue = -1;
        TheLeftNumber = LeftNumberList.GetHead();
    }

    installed_part = gCarCustomizeManager.GetActivePartFromSlot(CARSLOTID_DECAL_RIGHT_DOOR_RNUMBER);
    i = 0;
    test_part = RightNumberList.GetHead();
    while (test_part != RightNumberList.EndOfList()) {
        if (test_part->GetPart()->GetBrandNameHash() == bStringHash("NUMBER_RIGHT")) {
            if (!found) {
                if (bShowcaseOn == true && Showcase::FromFilter == i) {
                    TheRightNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part) != nullptr) {
                        TheRightNumber->SetInCartPreserve();
                    }
                    RightDisplayValue = i;
                    Showcase::FromFilter = -1;
                    found = true;
                } else if (test_part->GetPart() == installed_part) {
                    TheRightNumber = test_part;
                    if (gCarCustomizeManager.IsPartInCart(test_part) != nullptr) {
                        TheRightNumber->SetInCartPreserve();
                    }
                    RightDisplayValue = i;
                }
            }
            i++;
            test_part = test_part->GetNext();
        } else {
            SelectablePart *to_kill = test_part;
            test_part = test_part->GetNext();
            RightNumberList.Remove(to_kill);
            delete to_kill;
        }
    }
    if (TheRightNumber == nullptr) {
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
    if (toBeReturned == UISND_COMMON_SELECT) {
        switch (GetCategory()) {
            case CC_ENGINE:
                toBeReturned = UISND_CUST_INST_EXHAUST;
                break;
            case CC_TRANSMISSION:
                toBeReturned = UISND_CUST_INST_TRANSMISSION;
                break;
            case CC_NITROUS:
                toBeReturned = UISND_CUST_INST_NOS;
                break;
            case CC_TIRES:
                toBeReturned = UISND_CUST_INST_TIRES;
                break;
            case CC_SUSPENSION:
            case CC_BRAKES:
                toBeReturned = UISND_CUST_INST_GENERIC;
                break;
            case CC_FORCED_INDUCTION:
                toBeReturned = UISND_CUST_INST_TURBO;
                break;
        }
    }
    return toBeReturned;
}

void CustomizePerformance::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    CustomizationScreen::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
        case FEHASH_EXITCOMPLETE:
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

uint32 CustomizePerformance::GetPerfPkgDesc(Physics::Upgrades::Type type, int level, int line, bool turbo) {
    if (level == 0) {
        switch (type) {
            case Physics::Upgrades::PUT_TIRES:
                return 0xe5c1020c;
            case Physics::Upgrades::PUT_BRAKES:
                return 0x927db4fd;
            case Physics::Upgrades::PUT_CHASSIS:
                return 0x8c96b853;
            case Physics::Upgrades::PUT_TRANSMISSION:
                return 0x2f525e4f;
            case Physics::Upgrades::PUT_ENGINE:
                return 0xe74dedbb;
            case Physics::Upgrades::PUT_INDUCTION:
                if (turbo)
                    return 0x5317eb31;
                return 0x704a6d50;
            case Physics::Upgrades::PUT_NOS:
                return 0x9a0ef8f9;
            default:
                return 0;
        }
    }
    switch (type) {
        case Physics::Upgrades::PUT_TIRES:
            return FEngHashString("PD_TIRES_%d_%d", level, line);
        case Physics::Upgrades::PUT_BRAKES:
            return FEngHashString("PD_BRAKES_%d_%d", level, line);
        case Physics::Upgrades::PUT_CHASSIS:
            return FEngHashString("PD_CHASSIS_%d_%d", level, line);
        case Physics::Upgrades::PUT_TRANSMISSION:
            return FEngHashString("PD_TRANSMISSION_%d_%d", level, line);
        case Physics::Upgrades::PUT_ENGINE:
            if (gCarCustomizeManager.IsCastrolCar() && level == 4 && line == 3) {
                return FEngHashString("PD_ENGINE_%d_%d_CASTROL", 4, 3);
            }
            if (gCarCustomizeManager.IsRotaryCar() && (level == 2 || level == 4) && line == 1) {
                return FEngHashString("PD_ENGINE_%d_%d_ROTARY", level, 1);
            }
            return FEngHashString("PD_ENGINE_%d_%d", level, line);
        case Physics::Upgrades::PUT_INDUCTION:
            if (turbo) {
                return FEngHashString("PD_TURBO_%d_%d", level, line);
            } else {
                return FEngHashString("PD_SUPERCHARGER_%d_%d", level, line);
            }
        case Physics::Upgrades::PUT_NOS:
            return FEngHashString("PD_NITROUS_%d_%d", level, line);
        default:
            return 0;
    }
}

uint32 CustomizePerformance::GetPerfPkgBrand(Physics::Upgrades::Type type, int level, int line) {
    uint32 hash = 0;
    Attrib::Gen::frontend fe_attrib(gCarCustomizeManager.GetTuningCar()->FEKey, 0, nullptr);
    switch (type) {
        case Physics::Upgrades::PUT_TIRES:
            switch (level) {
                case 0:
                    hash = 0xad6a0504;
                    break;
                case 1:
                    hash = fe_attrib.p_tires_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_tires_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_tires_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_tires_4(line).hash;
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
                    hash = fe_attrib.p_brakes_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_brakes_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_brakes_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_brakes_4(line).hash;
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
                    hash = fe_attrib.p_suspension_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_suspension_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_suspension_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_suspension_4(line).hash;
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
                    hash = fe_attrib.p_transmission_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_transmission_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_transmission_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_transmission_4(line).hash;
                    break;
                default:
                    break;
            }
            break;
        case Physics::Upgrades::PUT_ENGINE:
            if (gCarCustomizeManager.IsCastrolCar() && level == 4 && line == 2) {
                return 0xb95d4df;
            }
            switch (level) {
                case 0:
                    hash = 0x7d0ac98f;
                    break;
                case 1:
                    hash = fe_attrib.p_engine_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_engine_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_engine_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_engine_4(line).hash;
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
                    hash = fe_attrib.p_induction_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_induction_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_induction_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_induction_4(line).hash;
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
                    hash = fe_attrib.p_nitrous_1(line).hash;
                    break;
                case 2:
                    hash = fe_attrib.p_nitrous_2(line).hash;
                    break;
                case 3:
                    hash = fe_attrib.p_nitrous_3(line).hash;
                    break;
                case 4:
                    hash = fe_attrib.p_nitrous_4(line).hash;
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

// UNSOLVED
void CustomizePerformance::RefreshHeader() {
    gCarCustomizeManager.PreviewPerfPkg(GetSelectedPart()->GetPhysicsType(), GetSelectedPart()->GetUpgradeLevel());

    AccelSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_ACCELERATION, true));
    HandlingSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_HANDLING, true));
    TopSpeedSlider.SetValue(gCarCustomizeManager.GetPerformanceRating(PRT_TOP_SPEED, true));

    AccelSlider.Draw();
    HandlingSlider.Draw();
    TopSpeedSlider.Draw();

    Physics::Upgrades::Type phys_type = GetSelectedPart()->GetPhysicsType();
    int level = GetSelectedPart()->GetUpgradeLevel();
    int loop = 3;

    int desc_level = (gCarCustomizeManager.GetMaxPackages(phys_type) - gCarCustomizeManager.GetNumPackages(phys_type)) + level;

    if (gCarCustomizeManager.IsInBackRoom() || level == PPL_LEVEL_JUNKMAN) {
        level = 0;
        desc_level = 0;
        loop = 1;
    }

    int i = 0;
    while (i < loop) {
        uint32 hash_desc = GetPerfPkgDesc(phys_type, desc_level, i + 1, gCarCustomizeManager.IsTurbo());
        if (DoesStringExist(hash_desc)) {
            FEngSetVisible(DescLines[i]);
            FEngSetVisible(DescBullets[i]);
            FEngSetLanguageHash(GetPackageName(), DescLines[i]->NameHash, hash_desc);
        } else {
            FEngSetInvisible(DescLines[i]);
            FEngSetInvisible(DescBullets[i]);
        }

        Attrib::Gen::frontend fe_attrib(gCarCustomizeManager.GetTuningCar()->FEKey, 0, nullptr);

        uint32 tex_hash = GetPerfPkgBrand(phys_type, desc_level, i);
        uint32 feimage_hash = FEngHashString("BRAND_ICON_%d", i + 1);

        if (GetTextureInfo(tex_hash, 0, 0) != nullptr) {
            FEngSetVisible(GetPackageName(), feimage_hash);
            FEngSetTextureHash(GetPackageName(), feimage_hash, tex_hash);
        } else {
            FEngSetInvisible(GetPackageName(), feimage_hash);
        }
        i++;
    }

    while (i < 3) {
        FEngSetInvisible(DescLines[i]);
        FEngSetInvisible(DescBullets[i]);
        uint32 feimage_hash = FEngHashString("BRAND_ICON_%d", i + 1);
        FEngSetInvisible(GetPackageName(), feimage_hash);
        i++;
    }

    CustomizationScreen::RefreshHeader();

    uint32 pkg_hash;
    if (GetSelectedPart()->GetUpgradeLevel() == PPL_LEVEL_JUNKMAN) {
        pkg_hash = 0xedd14807;
    } else {
        pkg_hash = FEngHashString("PN_LEVEL_%d", (PPL_LEVEL_6 - gCarCustomizeManager.GetNumPackages(phys_type)) + level);
    }
    FEngSetLanguageHash(pOptionName, pkg_hash);
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
    switch (GetCategory()) {
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

    uint32 icon_hash = 0xb8c8c0d4;
    bTList<SelectablePart> part_list;
    int j;
    bool is_locked;
    uint32 desc_hash = 0;

    if (gCarCustomizeManager.IsInBackRoom() && gCarCustomizeManager.IsCareerMode() && !gCarCustomizeManager.IsHeroCar()) {
        uint32 unlock_hash = 0;
        if (!gCarCustomizeManager.IsInBackRoom()) {
            unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), PPL_LEVEL_JUNKMAN);
        }
        SelectablePart *part = new ("SelectablePart 8", 0) SelectablePart(nullptr, 0, PPL_LEVEL_JUNKMAN, type, true, CPS_AVAILABLE, 0, true);
        AddPartOption(part, icon_hash, 7, desc_hash, unlock_hash, false);
        if (gCarCustomizeManager.IsPartInstalled(part)) {
            part->SetInstalled();
        } else if (gCarCustomizeManager.IsPartInCart(part) != nullptr) {
            part->SetInCart();
        }

    } else {
        gCarCustomizeManager.GetPerformancePartsList(type, part_list);
    }

    SelectablePart *part = part_list.GetHead();
    j = 1;
    while (!part_list.IsEmpty()) {
        part = part_list.RemoveHead();
        int true_unlock_level = gCarCustomizeManager.GetMaxPackages(type) - gCarCustomizeManager.GetNumPackages(type) + part->GetUpgradeLevel();
        uint32 unlock_hash = gCarCustomizeManager.GetUnlockHash(static_cast<eCustomizeCategory>(GetCategory()), true_unlock_level);
        AddPartOption(part, icon_hash, j, desc_hash, unlock_hash, gCarCustomizeManager.IsPartLocked(part, PPL_STOCK));
        j++;
    }

    if (((FEDatabase->GetCareerSettings()->HasBeenAwardedBKReward() && !FEDatabase->IsCareerMode()) ||
         (FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce && !gCarCustomizeManager.IsHeroCar())) &&
        gCarCustomizeManager.CanInstallJunkman(type)) {
        SelectablePart *part = new ("SelectablePart 8", 0) SelectablePart(nullptr, 0, PPL_LEVEL_JUNKMAN, type, true, CPS_AVAILABLE, 0, true);
        AddPartOption(part, icon_hash, 7, desc_hash, 0, false);
        if (gCarCustomizeManager.IsPartInstalled(part)) {
            part->SetInstalled();
        } else if (gCarCustomizeManager.IsPartInCart(part) != nullptr) {
            part->SetInCart();
        }
    }

    if (gCarCustomizeManager.IsInBackRoom() && gCarCustomizeManager.IsCareerMode()) {
        SetInitialOption(1);
    } else {
        int installed_index = gCarCustomizeManager.GetInstalledPerfPkg(type);
        ShoppingCartItem *item = gCarCustomizeManager.IsPartTypeInCart(type);
        if (item != nullptr) {
            installed_index = item->GetBuyingPart()->GetUpgradeLevel();
        }
        SetInitialOption(installed_index);
    }
    RefreshHeader();
}
