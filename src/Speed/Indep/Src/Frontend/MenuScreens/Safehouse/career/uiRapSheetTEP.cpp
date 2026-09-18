#include "uiRapSheetTEP.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_FeBusted.hpp"

uiRapSheetTEP::uiRapSheetTEP(ScreenConstructorData *sd) : UIWidgetMenu(sd), button_pressed(0), num_pursuits(0) {
    Setup();
}

void uiRapSheetTEP::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x0C407210:
            button_pressed = pobj->NameHash;
            break;
        case 0x406415E3:
            if (num_pursuits == 0) {
                return;
            }
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            break;
        case 0x72619778:
            if (pobj == nullptr) {
                return;
            }
            if (pobj->NameHash != __BUTTON_1__) {
                return;
            }
            FEngSetCurrentButton(GetPackageName(), FEngHashString("BUTTON_%d", num_pursuits));
            break;
        case 0x911C0A4B:
            if (pobj == nullptr) {
                return;
            }
            if (pobj->NameHash != static_cast<u32>(FEngHashString("BUTTON_%d", num_pursuits))) {
                return;
            }
            FEngSetCurrentButton(GetPackageName(), __BUTTON_1__);
            break;
        case 0x35F8620B:
            if (num_pursuits == 0) {
                return;
            }
            {
                unsigned char button = FEngGetLastButton(GetPackageName());
                if (button == 0) {
                    button = 1;
                }
                FEngSetCurrentButton(GetPackageName(), FEngHashString("BUTTON_%d", button));
            }
            break;
        case 0xE1FDE1D1: {
            int index;
            switch (button_pressed) {
                case __BUTTON_1__:
                    index = 0;
                    break;
                case __BUTTON_2__:
                    index = 1;
                    break;
                case 0xCDA0A66D:
                    index = 2;
                    break;
                case 0xCDA0A66E:
                    index = 3;
                    break;
                case 0xCDA0A66F:
                    index = 4;
                    break;
                default:
                    index = -1;
                    break;
            }
            if (index != -1) {
                cFEng::Get()->QueuePackageSwitch("RapSheetPD.fng", index, 0, false);
                FEngSetLastButton(GetPackageName(), static_cast<unsigned char>(index + 1));
            } else {
                cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
                FEngSetLastButton(GetPackageName(), 1);
            }
            break;
        }
    }
}

void uiRapSheetTEP::Setup() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    UserProfile &prof = *FEDatabase->GetUserProfile(0);
    HighScoresDatabase *scores = prof.GetHighScores();
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof.GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xE3DA78E7, GetLocalizedString(0x6031106E), prof.GetProfileName());
    FEPrintf(GetPackageName(), 0xE3DA78E8, GetLocalizedString(0x364E4525), stable->GetTotalBounty());
    for (int i = 0; i < 5; i++) {
        const TopEvadedPursuitDetail &pursuit =
            prof.GetHighScores()->GetTopEvadedPursuitScores(static_cast<unsigned short>(i));
        if (Timer(pursuit.Length) != Timer()) {
            char time_str[16];
            Timer(pursuit.Length).PrintToString(time_str, 0);
            FEPrintf(GetPackageName(), FEngHashString("CAR_USED_%d", i + 1), GetLocalizedString(0x69EAB50F),
                     GetLocalizedString(GetFECarNameHashFromFEKey(pursuit.CarFEKey)));
            FEPrintf(GetPackageName(), FEngHashString("BOUNTY_%d", i + 1), GetLocalizedString(0x060C058A), pursuit.Bounty);
            FEPrintf(GetPackageName(), FEngHashString("PURSUIT_ID_%d", i + 1), GetLocalizedString(0x41474FB1),
                     pursuit.PursuitName);
            FEPrintf(GetPackageName(), FEngHashString("PURSUIT_LENGTH_%d", i + 1), GetLocalizedString(0x36175146), time_str);
            num_pursuits++;
        } else {
            FEngSetButtonState(GetPackageName(), FEngHashString("BUTTON_%d", i + 1), false);
            FEPrintf(GetPackageName(), FEngHashString("CAR_USED_%d", i + 1), GetLocalizedString(0xE3274304));
            FEPrintf(GetPackageName(), FEngHashString("BOUNTY_%d", i + 1), "");
            FEPrintf(GetPackageName(), FEngHashString("PURSUIT_ID_%d", i + 1), "");
            FEPrintf(GetPackageName(), FEngHashString("PURSUIT_LENGTH_%d", i + 1), "");
        }
    }
    if (num_pursuits == 0) {
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0xEB5E7757));
        FEngSetInvisible(FEngFindObject(GetPackageName(), 0x73DCB662));
    }
}
