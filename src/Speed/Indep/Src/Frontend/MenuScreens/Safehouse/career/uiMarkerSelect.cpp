#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiMarkerSelect.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalFlow.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern cFrontendDatabase *FEDatabase;
extern unsigned int FEngHashString(const char *, ...);
extern void FEngSetScript(FEObject *, unsigned int, bool);
extern void FEngSetCurrentButton(const char *, unsigned int);
extern FEObject *FEngGetCurrentButton(const char *);
extern bool DoesCategoryHaveNewUnlock(eUnlockableEntity);
extern void GetLocalizedString(char *buffer, unsigned int bufsize, unsigned int string_label);

// total size: 0x1C
struct MarkerSelectInfo {
    FEMarkerManager::ePossibleMarker Marker; // offset 0x0
    unsigned int IconHash;                   // offset 0x4
    unsigned int CategoryIconHash;           // offset 0x8
    unsigned int NameHash;                   // offset 0xC
    unsigned int CategoryNameHash;           // offset 0x10
    unsigned int BlurbHash;                  // offset 0x14
    unsigned int CategoryBlurbHash;          // offset 0x18
};

extern MarkerSelectInfo MarkerSelectInfos[];

MarkerSelectInfo *GetMarkerSelectInfo(FEMarkerManager::ePossibleMarker marker) {
    for (int i = 0; i < 0x15; i++) {
        if (MarkerSelectInfos[i].Marker == marker) {
            return &MarkerSelectInfos[i];
        }
    }
    return nullptr;
}

unsigned int FEMarkerSelection::GetIconHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->IconHash;
}

unsigned int FEMarkerSelection::GetCategoryIconHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->CategoryIconHash;
}

unsigned int FEMarkerSelection::GetNameHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->NameHash;
}

unsigned int FEMarkerSelection::GetCategoryNameHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->CategoryNameHash;
}

unsigned int FEMarkerSelection::GetBlurbHashForType(FEMarkerManager::ePossibleMarker marker) {
    MarkerSelectInfo *info = GetMarkerSelectInfo(marker);
    return info->BlurbHash;
}

unsigned int FEMarkerSelection::GetCategoryBlurbHashForType(FEMarkerManager::ePossibleMarker marker) {
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

int FEMarkerSelection::GetButtonIndex(unsigned int hash) {
    if (hash == 0xcda0a66b)
        return 0;
    if (hash == 0xcda0a66c)
        return 1;
    if (hash == 0xcda0a66d)
        return 2;
    if (hash == 0xcda0a66e)
        return 3;
    if (hash == 0xcda0a66f)
        return 4;
    if (hash == 0xcda0a670)
        return 5;
    return 0;
}

int FEMarkerSelection::GetSelectedButtonIndex() {
    FEObject *btn = FEngGetCurrentButton(GetPackageName());
    if (btn) {
        return GetButtonIndex(btn->NameHash);
    }
    return 0;
}

FEMarkerSelection::FEMarkerSelection(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      NumVisibleMarkers(0) //
      ,
      RivalStreamer(sd->PackageFilename, false) {
    unsigned int CategoryOrder[] = {0xbdaa5794, 0xe69d4f7c, 0x73272ed2, 0xc61c8d3a};
    for (int cat = 0; cat < 4; cat++) {
        unsigned int categoryHash = CategoryOrder[cat];
        for (int j = 0; j < 6; j++) {
            FEMarkerManager::ePossibleMarker marker = static_cast<FEMarkerManager::ePossibleMarker>(0);
            int param = 0;
            TheFEMarkerManager.GetMarkerForLaterSelection(j, marker, param);
            if (marker != static_cast<FEMarkerManager::ePossibleMarker>(0) && categoryHash == GetCategoryIconHashForType(marker)) {
                TheMarkers[NumVisibleMarkers].Marker = marker;
                TheMarkers[NumVisibleMarkers].Param = param;
                TheMarkers[NumVisibleMarkers].Selected = false;
                NumVisibleMarkers++;
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
                found = (fe.UnlockedAt() == FEDatabase->GetCareerSettings()->GetCurrentBin()) || found;
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

void FEMarkerSelection::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0xe1fde1d1:
            TheFEMarkerManager.ClearMarkersForLaterSelection();
            uiRepSheetRivalFlow::Get()->Next();
            break;
        case 0x35f8620b:
            FEngSetCurrentButton(GetPackageName(), 0xcda0a66b);
            break;
        case 0x0c407210: {
            if (GetNumSelected() < 2) {
                int idx = GetSelectedButtonIndex();
                if (TheMarkers[idx].Selected)
                    break;
                FEngSetScript(pobj, 0x15970a, true);
                TheMarkers[idx].Selected = true;
                switch (static_cast<int>(TheMarkers[idx].Marker)) {
                    case 0x12:
                        FEDatabase->GetPlayerCarStable(0)->AwardRivalCar(TheMarkers[idx].Param);
                        break;
                    case 0x13:
                        FEDatabase->GetCareerSettings()->AddCash(TheMarkers[idx].Param);
                        break;
                    default:
                        TheFEMarkerManager.AddMarkerToInventory(TheMarkers[idx].Marker, TheMarkers[idx].Param);
                        break;
                }
                if (GetNumSelected() >= 2) {
                    FEngSetLanguageHash(GetPackageName(), 0xbdb541b3, 0x8098a54c);
                    FEngSetLanguageHash(GetPackageName(), 0x7603f3d5, 0x8098a54c);
                }
            } else {
                cFEng::Get()->QueuePackageMessage(0x587c018b, GetPackageName(), nullptr);
            }
            break;
        }
        case 0xabc08912: {
            FEPackage *pkg = cFEng::Get()->FindPackage(GetPackageName());
            if (!pkg->bInputEnabled)
                return;
            int idx = GetButtonIndex(pobj->NameHash);
            if (TheMarkers[idx].Selected) {
                FEngSetScript(pobj, 0x6b718fa1, true);
            } else {
                FEngSetScript(pobj, 0x249db7b7, true);
            }
        }
        case 0xbb3e313d:
        case 0xf0966d46:
            Redraw();
            break;
        case 0x55d1e635: {
            FEPackage *pkg = cFEng::Get()->FindPackage(GetPackageName());
            if (!pkg->bInputEnabled)
                return;
            int idx = GetButtonIndex(pobj->NameHash);
            if (TheMarkers[idx].Selected) {
                FEngSetScript(pobj, 0xc5decc84, true);
            } else {
                FEngSetScript(pobj, 0x7ab5521a, true);
            }
            break;
        }
    }
}

void FEMarkerSelection::Redraw() {
    for (int i = 0; i < NumVisibleMarkers; i++) {
        FEMarkerManager::ePossibleMarker marker = TheMarkers[i].Marker;
        if (TheMarkers[i].Selected) {
            FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("BUTTON_%d", i + 1));
            FEngSetTextureHash(img, GetIconHashForType(marker));
        } else {
            FEImage *img = FEngFindImage(GetPackageName(), FEngHashString("BUTTON_%d", i + 1));
            FEngSetTextureHash(img, GetCategoryIconHashForType(marker));
        }
    }

    int idx = GetSelectedButtonIndex();
    Selection selection = TheMarkers[idx];

    if (selection.Selected && selection.Marker != FEMarkerManager::MARKER_NONE) {
        FEngSetLanguageHash(GetPackageName(), 0x4960f369, GetNameHashForType(selection.Marker));
        unsigned int blurb = GetBlurbHashForType(selection.Marker);
        if (selection.Marker == static_cast<FEMarkerManager::ePossibleMarker>(0x13)) {
            const char *str = GetLocalizedString(blurb);
            FEPrintf(GetPackageName(), 0xeb0a8abd, str, selection.Param);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0xeb0a8abd, blurb);
        }
    } else {
        FEngSetLanguageHash(GetPackageName(), 0x4960f369, GetCategoryNameHashForType(selection.Marker));
        FEngSetLanguageHash(GetPackageName(), 0xeb0a8abd, GetCategoryBlurbHashForType(selection.Marker));
    }

    const char *remaining_str = GetLocalizedString(0x5bb3a130);
    FEPrintf(GetPackageName(), 0x38deac6b, remaining_str, 2 - GetNumSelected());

    int current_bin = FEDatabase->GetCareerSettings()->GetCurrentBin() + 1;
    char buf[256];
    GetLocalizedString(buf, 0x100, 0xae5bc899);
    unsigned int rival_hash = FEngHashString("BLACKLIST_RIVAL_%02d_AKA", current_bin);
    const char *rival_name = GetLocalizedString(rival_hash);
    FEPrintf(GetPackageName(), 0xd6c0e097, buf, 2 - GetNumSelected(), rival_name);
}
