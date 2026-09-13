#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "uiRapSheetRankingsDetail.hpp"
#include "uiRapSheetRankings.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

bool uiRapSheetRankings::career_view = false;

uiRapSheetRankings::uiRapSheetRankings(ScreenConstructorData *sd) : MenuScreen(sd), button_pressed(0), init_button(0) {
    Setup();
}

void uiRapSheetRankings::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case __BUTTON_PRESSED__:
            button_pressed = pobj->NameHash;
            break;
        case __PAD_BUTTON5__:
            career_view = !career_view;
            Setup();
            break;
        case FEHASH_INITCOMPLETE:
            FEngSetCurrentButton(GetPackageName(), init_button);
            break;
        case FEHASH_EXITCOMPLETE: {
            ePursuitDetailTypes type = PD_NUM_PD_TYPES;
            switch (button_pressed) {
                case 0xCDA0A66B:
                    type = PD_PURUSIT_LENGTH;
                    break;
                case 0xCDA0A66C:
                    type = PD_COPS_INVOLVED;
                    break;
                case 0xCDA0A66D:
                    type = PD_COPS_DAMAGED;
                    break;
                case 0xCDA0A66E:
                    type = PD_COPS_DESTROYED;
                    break;
                case 0xCDA0A66F:
                    type = PD_ROADBLOCKS_DODGED;
                    break;
                case 0xCDA0A670:
                    type = PD_SPIKESTRIPS_DODGED;
                    break;
                case 0xCDA0A671:
                    type = PD_COST_TO_STATE;
                    break;
                case 0xCDA0A672:
                    type = PD_NUM_INFRACTIONS;
                    break;
                case 0xCDA0A673:
                    type = PD_HELICOPTERS_INVOLVED;
                    break;
                case 0x81B573FB:
                    type = PD_BOUNTY;
                    break;
            }
            if (type != PD_NUM_PD_TYPES) {
                uiRapSheetRankingsDetail::career_view = career_view;
                cFEng::Get()->QueuePackageSwitch("RapSheetRankingsDetail.fng", type, 0, false);
                FEngSetLastButton(GetPackageName(), type); // TODO: bug? shouldnt this be `button_pressed`
            } else {
                cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false);
                FEngSetLastButton(GetPackageName(), 0);
            }
            break;
        }
    }
}

void uiRapSheetRankings::RefreshHeader() {
    UserProfile &prof = *FEDatabase->GetUserProfile(0);
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof.GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xEB406FEC, GetLocalizedString(0x6031106E), prof.GetProfileName());
    FEngSetLanguageHash(GetPackageName(), 0x1E4FDA, career_view ? 0x96DDF504 : 0x56E940F4);
    FEngSetLanguageHash(GetPackageName(), 0xDD2F4FB, career_view ? 0x554BBDB5 : 0xA88B3FC5);
    FEngSetLanguageHash(GetPackageName(), 0x9AE9B5CD, career_view ? 0x554BBDB5 : 0xA88B3FC5);
}

void uiRapSheetRankings::Setup() {
    PrintRanking(0x7711109B, 0xCDA0A66B, PD_PURUSIT_LENGTH);
    PrintRanking(0x7711109C, 0xCDA0A66C, PD_COPS_INVOLVED);
    PrintRanking(0x7711109D, 0xCDA0A66D, PD_COPS_DAMAGED);
    PrintRanking(0x7711109E, 0xCDA0A66E, PD_COPS_DESTROYED);
    PrintRanking(0x7711109F, 0xCDA0A66F, PD_ROADBLOCKS_DODGED);
    PrintRanking(0x771110A0, 0xCDA0A670, PD_SPIKESTRIPS_DODGED);
    PrintRanking(0x771110A1, 0xCDA0A671, PD_COST_TO_STATE);
    PrintRanking(0x771110A2, 0xCDA0A672, PD_NUM_INFRACTIONS);
    PrintRanking(0x771110A3, 0xCDA0A673, PD_HELICOPTERS_INVOLVED);
    PrintRanking(0x5933242B, 0x81B573FB, PD_BOUNTY);
    RefreshHeader();
}

void uiRapSheetRankings::PrintRanking(uint32 fe_rank, uint32 button_hash, ePursuitDetailTypes type) {
    UserProfile &prof = *FEDatabase->GetUserProfile(0);
    int rank = prof.GetHighScores()->CalcPursuitRank(type, career_view);
    if (rank != 0x10) {
        FEPrintf(GetPackageName(), fe_rank, "%$d", rank);
    } else {
        FEPrintf(GetPackageName(), fe_rank, "%s", GetLocalizedString(0xF3799455));
    }
    if (static_cast<uint8>(type) == FEngGetLastButton(GetPackageName())) {
        init_button = button_hash;
    }
}
