#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiMarkerSelect.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

FEMarkerSelection::FEMarkerSelection(ScreenConstructorData *sd)
    : MenuScreen(sd),       //
      NumVisibleMarkers(0), //
      RivalStreamer(sd->PackageFilename, false) {
    unsigned int CategoryOrder[] = {0xbdaa5794, 0xe69d4f7c, 0x73272ed2, 0xc61c8d3a};
    for (int cat = 0; cat < 4; cat++) {
        unsigned int categoryHash = CategoryOrder[cat];
        for (unsigned int j = 0; j < 6; j++) {
            Selection sel;
            sel.Selected = false;
            sel.Marker = static_cast<FEMarkerManager::ePossibleMarker>(0);
            sel.Param = 0;
            TheFEMarkerManager.GetMarkerForLaterSelection(j, sel.Marker, sel.Param);
            if (sel.Marker != static_cast<FEMarkerManager::ePossibleMarker>(0) && categoryHash == GetCategoryIconHashForType(sel.Marker)) {
                int index = NumVisibleMarkers;
                TheMarkers[index] = sel;
                NumVisibleMarkers = index + 1;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        int r = bRandom(3);
        Selection temp = TheMarkers[i];
        TheMarkers[i] = TheMarkers[r];
        TheMarkers[r] = temp;
    }

    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    pBGImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);

    RivalStreamer.Init(FEDatabase->GetCareerSettings()->GetCurrentBin() + 1, pRivalImg, pTagImg, pBGImg);
    Redraw();
    FEngSetLanguageHash(GetPackageName(), 0xbdb541b3, 0x9a375734);
    FEngSetLanguageHash(GetPackageName(), 0x7603f3d5, 0x9a375734);
    SetUnlockIcon(static_cast<eUnlockableEntity>(1), 0x9f04347d);
    SetUnlockIcon(static_cast<eUnlockableEntity>(2), 0x5b032d25);
    SetUnlockIcon(static_cast<eUnlockableEntity>(3), 0x96b11f47);
    SetUnlockIcon(static_cast<eUnlockableEntity>(0), 0x7f8aaf09);
}

void FEMarkerSelection::SetUnlockIcon(eUnlockableEntity ent, unsigned int message) {
    if (ent == static_cast<eUnlockableEntity>(0)) {
        bool found = false;
        FEPlayerCarDB *carDB = FEDatabase->GetPlayerCarStable(0);
        for (int i = 0; i < 200; i++) {
            FECarRecord *car = carDB->GetCarByIndex(i);
            if (car && car->IsValid()) {
                Attrib::Gen::frontend fe(car->FEKey, 0, nullptr);
                found |= (fe.UnlockedAt() == FEDatabase->GetCareerSettings()->GetCurrentBin());
            }
        }
        if (found) {
            cFEng::Get()->QueuePackageMessage(message, GetPackageName(), nullptr);
        }
    } else {
        if (DoesCategoryHaveNewUnlock(ent)) {
            cFEng::Get()->QueuePackageMessage(message, GetPackageName(), nullptr);
        }
    }
}

void FEMarkerSelection::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0xe1fde1d1:
            TheFEMarkerManager.ClearMarkersForLaterSelection();
            uiRepSheetRivalFlow::Get()->Next();
            break;
        case 0x35f8620b:
            FEngSetCurrentButton(GetPackageName(), __BUTTON_1__);
            break;
        case 0xbb3e313d:
            Redraw();
            break;
        case 0xf0966d46:
            Redraw();
            break;
        case 0x0c407210:
            if (GetNumSelected() < 2) {
                int index = GetSelectedButtonIndex();
                if (TheMarkers[index].Selected)
                    break;
                {
                    const u32 FLIPINTRO = 0x15970a;
                    const u32 FEObj_FLIP = 0xf0966d46;
                    FEngSetScript(pobj, FLIPINTRO, true);
                    TheMarkers[index].Selected = true;
                    switch (static_cast<int>(TheMarkers[index].Marker)) {
                        case 0x12: {
                            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                            stable->AwardRivalCar(TheMarkers[index].Param);
                            break;
                        }
                        case 0x13:
                            FEDatabase->GetCareerSettings()->AwardCash(TheMarkers[index].Param);
                            break;
                        default:
                            TheFEMarkerManager.AddMarkerToInventory(TheMarkers[index].Marker, TheMarkers[index].Param);
                            break;
                    }
                    if (GetNumSelected() >= 2) {
                        const u32 FEObj_AcceptText = 0xbdb541b3;
                        const u32 FEObj_CHOOSE_MARKERS = 0x7603f3d5;
                        FEngSetLanguageHash(GetPackageName(), FEObj_AcceptText, LANGUAGE_COMMON_CONTINUE);
                        FEngSetLanguageHash(GetPackageName(), FEObj_CHOOSE_MARKERS, LANGUAGE_COMMON_CONTINUE);
                    }
                }
            } else {
                const u32 FEObj_leavescreen = 0x587c018b;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;
        case 0xabc08912: {
            const u32 FLIPHIGHLIGHT = 0x6b718fa1;
            const u32 HIGHLIGHT = FEHASH_HIGHLIGHT;
            FEPackage *pkg = cFEng::Get()->FindPackage(GetPackageName());
            if (!pkg->IsInputEnabled())
                return;
            {
                int index = GetButtonIndex(pobj->NameHash);
                if (TheMarkers[index].Selected) {
                    FEngSetScript(pobj, FLIPHIGHLIGHT, true);
                } else {
                    FEngSetScript(pobj, HIGHLIGHT, true);
                }
            }
            Redraw();
            break;
        }
        case 0x55d1e635: {
            const u32 UNHIGHLIGHT = FEHASH_UNHIGHLIGHT;
            const u32 FLIPUNHIGHLIGHT = 0xc5decc84;
            FEPackage *pkg = cFEng::Get()->FindPackage(GetPackageName());
            if (!pkg->IsInputEnabled())
                return;
            {
                int index = GetButtonIndex(pobj->NameHash);
                if (TheMarkers[index].Selected) {
                    FEngSetScript(pobj, FLIPUNHIGHLIGHT, true);
                } else {
                    FEngSetScript(pobj, UNHIGHLIGHT, true);
                }
            }
            break;
        }
    }
}

int FEMarkerSelection::GetButtonIndex(unsigned int hash) {
    switch (hash) {
        case __BUTTON_1__:
            return 0;
        case __BUTTON_2__:
            return 1;
        case 0xcda0a66d:
            return 2;
        case 0xcda0a66e:
            return 3;
        case 0xcda0a66f:
            return 4;
        case 0xcda0a670:
            return 5;
    }
    return 0;
}

int FEMarkerSelection::GetSelectedButtonIndex() {
    FEObject *btn = FEngGetCurrentButton(GetPackageName());
    if (btn) {
        return GetButtonIndex(btn->NameHash);
    }
    return 0;
}

// total size: 0x1C
struct MarkerSelectInfo {
    FEMarkerManager::ePossibleMarker Marker; // offset 0x0
    uint32 IconHash;                         // offset 0x4
    uint32 CategoryIconHash;                 // offset 0x8
    uint32 NameHash;                         // offset 0xC
    uint32 CategoryNameHash;                 // offset 0x10
    uint32 BlurbHash;                        // offset 0x14
    uint32 CategoryBlurbHash;                // offset 0x18
};

MarkerSelectInfo MarkerSelectInfos[] = {
    {FEMarkerManager::MARKER_BRAKES, 0x4887F351, 0xC61C8D3A, LANGUAGE_MARKER_NAME_BRAKES, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_BRAKES, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_ENGINE, 0x4F424E0F, 0xC61C8D3A, LANGUAGE_MARKER_NAME_ENGINE, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_ENGINE, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_NOS, 0x6FEA04C8, 0xC61C8D3A, LANGUAGE_MARKER_NAME_NOS, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_NOS, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_INDUCTION, 0x8E284227, 0xC61C8D3A, LANGUAGE_MARKER_NAME_SUPERCHARGER, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_SUPERCHARGER, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_CHASSIS, 0x00190EB6, 0xC61C8D3A, LANGUAGE_MARKER_NAME_CHASSI, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_CHASSI, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_TIRES, 0x7373F1EF, 0xC61C8D3A, LANGUAGE_MARKER_NAME_TIRE, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_TIRE, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_TRANSMISSION, 0xD142D3E3, 0xC61C8D3A, LANGUAGE_MARKER_NAME_TRANSMISSION, LANGUAGE_MARKER_CATEGORY_PERFORMANCE, LANGUAGE_MARKER_NAME_DESCRIPTION_TRANSMISSION, LANGUAGE_MARKER_CATEGORY_DESC_PERFORMANCE},
    {FEMarkerManager::MARKER_BODY, 0xAF393DBA, 0x73272ED2, LANGUAGE_MARKER_NAME_BODYKIT, LANGUAGE_MARKER_CATEGORY_PARTS, LANGUAGE_MARKER_NAME_DESCRIPTION_BODYKIT, LANGUAGE_MARKER_CATEGORY_DESC_PARTS},
    {FEMarkerManager::MARKER_HOOD, 0xF375276E, 0x73272ED2, LANGUAGE_MARKER_NAME_HOOD, LANGUAGE_MARKER_CATEGORY_PARTS, LANGUAGE_MARKER_NAME_DESCRIPTION_HOOD, LANGUAGE_MARKER_CATEGORY_DESC_PARTS},
    {FEMarkerManager::MARKER_SPOILER, 0xC51A4F62, 0x73272ED2, LANGUAGE_MARKER_NAME_SPOILER, LANGUAGE_MARKER_CATEGORY_PARTS, LANGUAGE_MARKER_NAME_DESCRIPTION_SPOILER, LANGUAGE_MARKER_CATEGORY_DESC_PARTS},
    {FEMarkerManager::MARKER_RIMS, 0xC19491CC, 0x73272ED2, LANGUAGE_MARKER_NAME_RIMS, LANGUAGE_MARKER_CATEGORY_PARTS, LANGUAGE_MARKER_NAME_DESCRIPTION_RIMS, LANGUAGE_MARKER_CATEGORY_DESC_PARTS},
    {FEMarkerManager::MARKER_ROOF_SCOOP, 0x25A4375E, 0x73272ED2, LANGUAGE_MARKER_NAME_ROOFSCOOPS, LANGUAGE_MARKER_CATEGORY_PARTS, LANGUAGE_MARKER_NAME_DESCRIPTION_ROOFSCOOPS, LANGUAGE_MARKER_CATEGORY_DESC_PARTS},
    {FEMarkerManager::MARKER_VINYL, 0xD35F04C0, 0xE69D4F7C, LANGUAGE_MARKER_NAME_VINYLS, LANGUAGE_MARKER_CATEGORY_VISUAL, LANGUAGE_MARKER_NAME_DESCRIPTION_VINYLS, LANGUAGE_MARKER_CATEGORY_DESC_VISUAL},
    {FEMarkerManager::MARKER_DECAL, 0xA9135927, 0xE69D4F7C, 0x00000000, LANGUAGE_MARKER_CATEGORY_VISUAL, 0x00000000, LANGUAGE_MARKER_CATEGORY_DESC_VISUAL},
    {FEMarkerManager::MARKER_PAINT, 0x0DB89E17, 0xE69D4F7C, 0x00000000, LANGUAGE_MARKER_CATEGORY_VISUAL, 0x00000000, LANGUAGE_MARKER_CATEGORY_DESC_VISUAL},
    {FEMarkerManager::MARKER_CUSTOM_HUD, 0x8BA602FC, 0xE69D4F7C, LANGUAGE_MARKER_NAME_HUD, LANGUAGE_MARKER_CATEGORY_VISUAL, LANGUAGE_MARKER_NAME_DESCRIPTION_HUD, LANGUAGE_MARKER_CATEGORY_DESC_VISUAL},
    {FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0x28D4FE33, 0xBDAA5794, LANGUAGE_MARKER_NAME_FREEBAIL, LANGUAGE_MARKER_CATEGORY_MISC, LANGUAGE_MARKER_NAME_DESCRIPTION_FREEBAIL, LANGUAGE_MARKER_CATEGORY_DESC_MISC},
    {FEMarkerManager::MARKER_PINK_SLIP, 0x67E2461D, 0xBDAA5794, LANGUAGE_MARKER_NAME_PINKSLIP, LANGUAGE_MARKER_CATEGORY_MISC, LANGUAGE_MARKER_NAME_DESCRIPTION_PINKSLIP, LANGUAGE_MARKER_CATEGORY_DESC_MISC},
    {FEMarkerManager::MARKER_CASH, 0x28D128D2, 0xBDAA5794, LANGUAGE_MARKER_NAME_CASH, LANGUAGE_MARKER_CATEGORY_MISC, LANGUAGE_MARKER_NAME_DESCRIPTION_CASH, LANGUAGE_MARKER_CATEGORY_DESC_MISC},
    {FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0xB06C3D98, 0xBDAA5794, LANGUAGE_MARKER_NAME_IMPOUND, LANGUAGE_MARKER_CATEGORY_MISC, LANGUAGE_MARKER_NAME_DESCRIPTION_IMPOUND, LANGUAGE_MARKER_CATEGORY_DESC_MISC},
    {FEMarkerManager::MARKER_IMPOUND_RELEASE, 0x189F4EF0, 0xBDAA5794, LANGUAGE_MARKER_NAME_RELEASE, LANGUAGE_MARKER_CATEGORY_MISC, LANGUAGE_MARKER_NAME_DESCRIPTION_RELEASE, LANGUAGE_MARKER_CATEGORY_DESC_MISC},
};

MarkerSelectInfo *GetMarkerSelectInfo(FEMarkerManager::ePossibleMarker marker) {
    for (int i = 0; i < 0x15; i++) {
        if (MarkerSelectInfos[i].Marker == marker) {
            return &MarkerSelectInfos[i];
        }
    }
    return nullptr;
}

uint32 FEMarkerSelection::GetIconHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->IconHash;
}

uint32 FEMarkerSelection::GetCategoryIconHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->CategoryIconHash;
}

uint32 FEMarkerSelection::GetNameHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->NameHash;
}

uint32 FEMarkerSelection::GetCategoryNameHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->CategoryNameHash;
}

uint32 FEMarkerSelection::GetBlurbHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->BlurbHash;
}

uint32 FEMarkerSelection::GetCategoryBlurbHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->CategoryBlurbHash;
}

int FEMarkerSelection::GetNumSelected() {
    int count = 0;
    for (int i = 0; i < NumVisibleMarkers; i++) {
        if (TheMarkers[i].Marker != static_cast<FEMarkerManager::ePossibleMarker>(0) && TheMarkers[i].Selected) {
            count++;
        }
    }
    return count;
}

void FEMarkerSelection::Redraw() {
    for (int i = 0; i < NumVisibleMarkers; i++) {
        FEMarkerManager::ePossibleMarker marker = TheMarkers[i].Marker;
        if (TheMarkers[i].Selected) {
            const char *pkg = GetPackageName();
            unsigned int hash = FEngHashString("BUTTON_%d", i + 1);
            unsigned int icon = GetIconHashForType(marker);
            FEngSetTextureHash(FEngFindImage(pkg, hash), icon);
        } else {
            const char *pkg = GetPackageName();
            unsigned int hash = FEngHashString("BUTTON_%d", i + 1);
            unsigned int icon = GetCategoryIconHashForType(marker);
            FEngSetTextureHash(FEngFindImage(pkg, hash), icon);
        }
    }

    int idx = GetSelectedButtonIndex();
    Selection selection = TheMarkers[idx];

    if (selection.Selected && selection.Marker != FEMarkerManager::MARKER_NONE) {
        FEngSetLanguageHash(GetPackageName(), 0x4960f369, GetNameHashForType(selection.Marker));
        unsigned int blurb = GetBlurbHashForType(selection.Marker);
        if (selection.Marker == static_cast<FEMarkerManager::ePossibleMarker>(0x13)) {
            const char *pkg = GetPackageName();
            const char *str = GetLocalizedString(blurb);
            FEPrintf(pkg, 0xeb0a8abd, str, selection.Param);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xeb0a8abd, blurb);
        }
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x4960f369, GetCategoryNameHashForType(selection.Marker));
        FEngSetLanguageHash(GetPackageName(), 0xeb0a8abd, GetCategoryBlurbHashForType(selection.Marker));
    }

    const char *pkg2 = GetPackageName();
    const char *remaining_str = GetLocalizedString(0x5bb3a130);
    FEPrintf(pkg2, 0x38deac6b, remaining_str, 2 - GetNumSelected());

    int current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin() + 1;
    char buf[256];
    GetLocalizedString(buf, 0x100, 0xae5bc899);
    const char *pkg3 = GetPackageName();
    int remaining = 2 - GetNumSelected();
    unsigned int rival_hash = FEngHashString("BLACKLIST_RIVAL_%02d_AKA", current_bin);
    const char *rival_name = GetLocalizedString(rival_hash);
    FEPrintf(pkg3, 0xd6c0e097, buf, remaining, rival_name);
}
