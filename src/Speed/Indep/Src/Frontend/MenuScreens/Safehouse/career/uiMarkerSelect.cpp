#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiMarkerSelect.hpp"

#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

FEMarkerSelection::FEMarkerSelection(ScreenConstructorData *sd)
    : MenuScreen(sd),       //
      NumVisibleMarkers(0), //
      RivalStreamer(sd->PackageFilename, false) {
    uint32 types[] = {0xbdaa5794, 0xe69d4f7c, 0x73272ed2, 0xc61c8d3a};
    const u32 FEObj_AcceptText = 0;
    const u32 FEObj_CHOOSE_MARKERS = 0;
    for (int i = 0; i < 4; i++) {
        uint32 type = types[i];
        for (int j = 0; j < 6u; j++) { // TODO: 6u probably comes from somewhere
            Selection temp;
            temp.Selected = false;
            temp.Marker = FEMarkerManager::MARKER_NONE;
            temp.Param = 0;
            TheFEMarkerManager.GetMarkerForLaterSelection(j, temp.Marker, temp.Param);
            if (temp.Marker != FEMarkerManager::MARKER_NONE && type == GetCategoryIconHashForType(temp.Marker)) {
                TheMarkers[NumVisibleMarkers++] = temp;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        int spot = bRandom(3);
        Selection bonk = TheMarkers[i];
        TheMarkers[i] = TheMarkers[spot];
        TheMarkers[spot] = bonk;
    }

    pRivalImg = FEngFindImage(GetPackageName(), 0xc1f62308);
    pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
    pBGImg = FEngFindImage(GetPackageName(), 0x2cbe1dd0);

    RivalStreamer.Init(FEDatabase->GetCareerSettings()->GetCurrentBin() + 1, pRivalImg, pTagImg, pBGImg);
    Redraw();
    FEngSetLanguageHash(GetPackageName(), 0xbdb541b3, 0x9a375734);
    FEngSetLanguageHash(GetPackageName(), 0x7603f3d5, 0x9a375734);
    SetUnlockIcon(UNLOCKABLE_THING_CUSTOMIZE_PARTS, 0x9f04347d);
    SetUnlockIcon(UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE, 0x5b032d25);
    SetUnlockIcon(UNLOCKABLE_THING_CUSTOMIZE_VISUAL, 0x96b11f47);
    SetUnlockIcon(UNLOCKABLE_THING_UNKNOWN, 0x7f8aaf09);
}

void FEMarkerSelection::SetUnlockIcon(eUnlockableEntity ent, uint32 message) {
    if (ent == UNLOCKABLE_THING_UNKNOWN) {
        int unlock_filter;
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        bool carUnlocked = false;
        for (int i = 0; i < 200; i++) {
            FECarRecord *fe_car = stable->GetCarByIndex(i);
            if (fe_car == nullptr || !fe_car->IsValid()) {
                continue;
            }
            Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
            carUnlocked |= (CarAttribs.UnlockedAt() == FEDatabase->GetCareerSettings()->GetCurrentBin());
        }
        if (carUnlocked) {
            cFEng::Get()->QueuePackageMessage(message, GetPackageName(), nullptr);
        }
    } else {
        if (DoesCategoryHaveNewUnlock(ent)) {
            cFEng::Get()->QueuePackageMessage(message, GetPackageName(), nullptr);
        }
    }
}

// UNSOLVED
void FEMarkerSelection::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_EXITCOMPLETE:
            TheFEMarkerManager.ClearMarkersForLaterSelection();
            uiRepSheetRivalFlow::Get()->Next();
            break;
        case FEHASH_INITCOMPLETE:
            FEngSetCurrentButton(GetPackageName(), __BUTTON_1__);
            break;
        case __BUTTON_PRESSED__:
            if (GetNumSelected() < 2) {
                int index = GetSelectedButtonIndex();
                if (!TheMarkers[index].Selected) {
                    const u32 FLIPINTRO = 0; // TODO
                    const u32 FEObj_FLIP = 0x15970a;
                    FEngSetScript(pobj, FEObj_FLIP, true);
                    TheMarkers[index].Selected = true;
                    switch (TheMarkers[index].Marker) {
                        case FEMarkerManager::MARKER_PINK_SLIP: {
                            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                            stable->AwardRivalCar(TheMarkers[index].Param);
                            break;
                        }
                        case FEMarkerManager::MARKER_CASH:
                            FEDatabase->GetCareerSettings()->AwardCash(TheMarkers[index].Param);
                            break;
                        default:
                            TheFEMarkerManager.AddMarkerToInventory(TheMarkers[index].Marker, TheMarkers[index].Param);
                            break;
                    }
                    if (GetNumSelected() >= 2) {
                        const u32 FEObj_AcceptText = 0xbdb541b3;
                        const u32 FEObj_CHOOSE_MARKERS = 0x7603f3d5;
                        FEngSetLanguageHash(GetPackageName(), FEObj_AcceptText, 0x8098a54c);
                        FEngSetLanguageHash(GetPackageName(), FEObj_CHOOSE_MARKERS, 0x8098a54c);
                    }
                }
            } else {
                const u32 FEObj_leavescreen = 0x587c018b;
                cFEng::Get()->QueuePackageMessage(FEObj_leavescreen, GetPackageName(), nullptr);
            }
            break;

        case 0xbb3e313d:
        case 0xf0966d46:
            Redraw();
            break;

        case 0xabc08912: {
            const u32 FLIPHIGHLIGHT = 0x6b718fa1;
            const u32 HIGHLIGHT = 0x249db7b7;
            FEPackage *pkg = cFEng::Get()->FindPackage(GetPackageName());
            if (pkg->IsInputEnabled()) {
                int index = GetButtonIndex(pobj->NameHash);
                if (TheMarkers[index].Selected) {
                    FEngSetScript(pobj, FLIPHIGHLIGHT, true);
                } else {
                    FEngSetScript(pobj, HIGHLIGHT, true);
                }
                Redraw();
            }
            break;
        }

        case 0x55d1e635: {
            const u32 UNHIGHLIGHT = 0x7ab5521a;
            const u32 FLIPUNHIGHLIGHT = 0xc5decc84;
            FEPackage *pkg = cFEng::Get()->FindPackage(GetPackageName());
            if (pkg->IsInputEnabled()) {
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

int FEMarkerSelection::GetButtonIndex(uint32 hash) {
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
        default:
            return 0;
    }
}

int FEMarkerSelection::GetSelectedButtonIndex() {
    int index;
    FEObject *button = FEngGetCurrentButton(GetPackageName());
    if (button == nullptr) {
        index = 0;
    } else {
        index = GetButtonIndex(button->NameHash);
    }
    return index;
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

MarkerSelectInfo MarkerSelectInfos[21] = {
    {FEMarkerManager::MARKER_BRAKES, 0x4887F351, 0xC61C8D3A, 0x140000B8, 0x946FD2EF, 0x862BFA1B, 0x734A54CD},
    {FEMarkerManager::MARKER_ENGINE, 0x4F424E0F, 0xC61C8D3A, 0x1ABA5B76, 0x946FD2EF, 0x8CE654D9, 0x734A54CD},
    {FEMarkerManager::MARKER_NOS, 0x6FEA04C8, 0xC61C8D3A, 0x972BB750, 0x946FD2EF, 0x81C7393, 0x734A54CD},
    {FEMarkerManager::MARKER_INDUCTION, 0x8E284227, 0xC61C8D3A, 0x963CB08B, 0x946FD2EF, 0xE684E82E, 0x734A54CD},
    {FEMarkerManager::MARKER_CHASSIS, 0x190EB6, 0xC61C8D3A, 0x15A0581B, 0x946FD2EF, 0x87CC517E, 0x734A54CD},
    {FEMarkerManager::MARKER_TIRES, 0x7373F1EF, 0xC61C8D3A, 0x7CA5D234, 0x946FD2EF, 0xBAE16D7, 0x734A54CD},
    {FEMarkerManager::MARKER_TRANSMISSION, 0xD142D3E3, 0xC61C8D3A, 0x47F128A, 0x946FD2EF, 0x54C74A2D, 0x734A54CD},
    {FEMarkerManager::MARKER_BODY, 0xAF393DBA, 0x73272ED2, 0x8D3EAD36, 0x4F7990C7, 0x44E9D2F9, 0x2D078425},
    {FEMarkerManager::MARKER_HOOD, 0xF375276E, 0x73272ED2, 0x7C9F56CA, 0x4F7990C7, 0xBA79B6D, 0x2D078425},
    {FEMarkerManager::MARKER_SPOILER, 0xC51A4F62, 0x73272ED2, 0xACF029DE, 0x4F7990C7, 0x649B4FA1, 0x2D078425},
    {FEMarkerManager::MARKER_RIMS, 0xC19491CC, 0x73272ED2, 0x7CA4B8DB, 0x4F7990C7, 0xBACFD7E, 0x2D078425},
    {FEMarkerManager::MARKER_ROOF_SCOOP, 0x25A4375E, 0x73272ED2, 0x8E9F2DAD, 0x4F7990C7, 0xBF1F2090, 0x2D078425},
    {FEMarkerManager::MARKER_VINYL, 0xD35F04C0, 0xE69D4F7C, 0x420BAE25, 0x4D3BF011, 0xB437A788, 0xDC884F2F},
    {FEMarkerManager::MARKER_DECAL, 0xA9135927, 0xE69D4F7C, 0x0, 0x4D3BF011, 0x0, 0xDC884F2F},
    {FEMarkerManager::MARKER_CUSTOMIZE_LAST, 0xDB89E17, 0xE69D4F7C, 0x0, 0x4D3BF011, 0x0, 0xDC884F2F},
    {FEMarkerManager::MARKER_CUSTOM_HUD, 0x8BA602FC, 0xE69D4F7C, 0x972B9E81, 0x4D3BF011, 0x81C5AC4, 0xDC884F2F},
    {FEMarkerManager::MARKER_GET_OUT_OF_JAIL, 0x28D4FE33, 0xBDAA5794, 0xCDF089BA, 0x9D8DCEE9, 0x7B0067DD, 0x15BCE87},
    {FEMarkerManager::MARKER_PINK_SLIP, 0x67E2461D, 0xBDAA5794, 0x692B39AA, 0x9D8DCEE9, 0x163B17CD, 0x15BCE87},
    {FEMarkerManager::MARKER_CASH, 0x28D128D2, 0xBDAA5794, 0x7C9C5DDF, 0x9D8DCEE9, 0xBA4A282, 0x15BCE87},
    {FEMarkerManager::MARKER_ADD_IMPOUND_BOX, 0xB06C3D98, 0xBDAA5794, 0xA43FDA9C, 0x9D8DCEE9, 0x5BEB005F, 0x15BCE87},
    {FEMarkerManager::MARKER_IMPOUND_RELEASE, 0x189F4EF0, 0xBDAA5794, 0x46148621, 0x9D8DCEE9, 0xFDBFABE4, 0x15BCE87}};

MarkerSelectInfo *GetMarkerSelectInfo(FEMarkerManager::ePossibleMarker marker) {
    for (int i = 0; i < 0x15; i++) {
        if (MarkerSelectInfos[i].Marker == marker) {
            return &MarkerSelectInfos[i];
        }
    }
    return nullptr;
}

uint32 FEMarkerSelection::GetIconHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *mi = GetMarkerSelectInfo(marker);
    return mi->IconHash;
}

uint32 FEMarkerSelection::GetCategoryIconHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *mi = GetMarkerSelectInfo(marker);
    return mi->CategoryIconHash;
}

uint32 FEMarkerSelection::GetNameHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *mi = GetMarkerSelectInfo(marker);
    return mi->NameHash;
}

uint32 FEMarkerSelection::GetCategoryNameHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *mi = GetMarkerSelectInfo(marker);
    return mi->CategoryNameHash;
}

uint32 FEMarkerSelection::GetBlurbHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *mi = GetMarkerSelectInfo(marker);
    return mi->BlurbHash;
}

uint32 FEMarkerSelection::GetCategoryBlurbHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *mi = GetMarkerSelectInfo(marker);
    return mi->CategoryBlurbHash;
}

int FEMarkerSelection::GetNumSelected() {
    int n = 0;
    for (int i = 0; i < NumVisibleMarkers; i++) {
        if (TheMarkers[i].Marker != FEMarkerManager::MARKER_NONE && TheMarkers[i].Selected) {
            n++;
        }
    }
    return n;
}

void FEMarkerSelection::Redraw() {
    for (int i = 0; i < NumVisibleMarkers; i++) {
        FEMarkerManager::ePossibleMarker marker = TheMarkers[i].Marker;
        if (TheMarkers[i].Selected) {
            FEngSetTextureHash(GetPackageName(), FEngHashString("BUTTON_%d", i + 1), GetIconHashForType(marker));
        } else {
            FEngSetTextureHash(GetPackageName(), FEngHashString("BUTTON_%d", i + 1), GetCategoryIconHashForType(marker));
        }
    }

    const u32 FEObj_MARKERINFO1 = 0xeb0a8abd;
    const u32 FEObj_MARKERHEADER = 0x4960f369;

    Selection selected = TheMarkers[GetSelectedButtonIndex()];

    if (selected.Selected && selected.Marker != FEMarkerManager::MARKER_NONE) {
        FEngSetLanguageHash(GetPackageName(), FEObj_MARKERHEADER, GetNameHashForType(selected.Marker));
        uint32 hash = GetBlurbHashForType(selected.Marker);
        if (selected.Marker == FEMarkerManager::MARKER_CASH) {
            FEPrintf(GetPackageName(), FEObj_MARKERINFO1, GetLocalizedString(hash), selected.Param);
        } else {
            FEngSetLanguageHash(GetPackageName(), FEObj_MARKERINFO1, hash);
        }
    } else {
        FEngSetLanguageHash(GetPackageName(), FEObj_MARKERHEADER, GetCategoryNameHashForType(selected.Marker));
        FEngSetLanguageHash(GetPackageName(), FEObj_MARKERINFO1, GetCategoryBlurbHashForType(selected.Marker));
    }

    const u32 FEObj_SELECTIONSREMAININGBASE = 0x5bb3a130;
    FEPrintf(GetPackageName(), 0x38deac6b, GetLocalizedString(FEObj_SELECTIONSREMAININGBASE), 2 - GetNumSelected());

    int cur_bin = FEDatabase->GetCareerSettings()->GetCurrentBin() + 1;
    char sztemp[256];
    GetLocalizedString(sztemp, 0x100, 0xae5bc899);
    const u32 FEObj_PICKRIVALMARKERS = 0xd6c0e097;
    FEPrintf(GetPackageName(), FEObj_PICKRIVALMARKERS, sztemp, 2 - GetNumSelected(),
             GetLocalizedString(FEngHashString("BLACKLIST_RIVAL_%02d_AKA", cur_bin)));
}
