#include "uiRapSheetRankingsDetail.hpp"
#include "uiRapSheetRankings.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
int FEPrintf(const char* pkg_name, int hash, const char* fmt, ...);
int FEPrintf(FEString* text, const char* fmt, ...);
void FEngSetLanguageHash(FEString* text, unsigned int hash);
void FEngSetLanguageHash(const char* pkg_name, unsigned int obj_hash, unsigned int lang_hash);
unsigned int FEngHashString(const char* format, ...);
unsigned int GetFECarNameHashFromFEKey(unsigned int feKey);
const char* GetLocalizedString(unsigned int hash);
bool uiRapSheetRankingsDetail::career_view = false;
void RapSheetRankingsArraySlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetRankingsDatum* d = static_cast<RapSheetRankingsDatum*>(datum);
        FEPrintf(pValue, "%.0f", d->getValue());
        if (d->getItemNum() != 0x10) { FEPrintf(pItemNum, "%d", d->getItemNum()); }
        else { FEngSetLanguageHash(pItemNum, 0xFC1BF40); }
        if (d->getCarName() != 0) { FEngSetLanguageHash(pCarName, d->getCarName()); }
        else { FEPrintf(pCarName, ""); }
        if (d->getPlayerName() != 1) { FEngSetLanguageHash(pPlayerName, d->getPlayerName()); }
        else { FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName()); }
    }
}
void RapSheetRankingsTimerArraySlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetRankingsDatum* d = static_cast<RapSheetRankingsDatum*>(datum);
        if (d->getItemNum() != 0x10) { FEPrintf(pItemNum, "%d"); }
        else { FEPrintf(pItemNum, "#"); }
        if (d->getCarName() != 0) { FEngSetLanguageHash(pCarName, d->getCarName()); }
        else { FEPrintf(pCarName, ""); }
        Timer t;
        t.SetTime(d->getValue());
        char time_str[16];
        t.PrintToString(time_str, 16);
        FEPrintf(pValue, "%s", time_str);
        if (d->getPlayerName() != 1) { FEngSetLanguageHash(pPlayerName, d->getPlayerName()); }
        else { FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName()); }
    }
}
uiRapSheetRankingsDetail::uiRapSheetRankingsDetail(ScreenConstructorData* sd)
    : ArrayScrollerMenu(sd, 1, 10, false) //
    , rank_type(static_cast<ePursuitDetailTypes>(sd->Arg)) //
    , player_rank(0x10)
{
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString* pItemNum = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_RANK_%d", i + 1));
        FEString* pPlayerName = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_PLAYER_%d", i + 1));
        FEString* pCarName = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE_%d", i + 1));
        FEString* pValue = FEngFindString(GetPackageName(), FEngHashString("RAPSHEET_VALUE2_%d", i + 1));
        if (rank_type == ePDT_CostToState) { AddSlot(new(__FILE__, __LINE__) RapSheetRankingsTimerArraySlot(pItemNum, pPlayerName, pCarName, pValue)); }
        else { AddSlot(new(__FILE__, __LINE__) RapSheetRankingsArraySlot(pItemNum, pPlayerName, pCarName, pValue)); }
    }
    Setup();
}
uiRapSheetRankingsDetail::~uiRapSheetRankingsDetail() {}
void uiRapSheetRankingsDetail::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    switch (msg) {
    case 0xC519BFC4:
        career_view = !career_view;
        Setup();
        break;
    case 0x911C0A4B:
    case 0x35F8620B:
    case 0x72619778:
        UpdateHighlight();
        break;
    case 0xE1FDE1D1:
        uiRapSheetRankings::career_view = career_view;
        cFEng::Get()->QueuePackageSwitch("RapSheetRankings.fng", 0, 0, false);
        break;
    }
}
void uiRapSheetRankingsDetail::Setup() {
    ClearData();
    HighScoresDatabase* const scores = FEDatabase->GetUserProfile(0)->GetHighScores();
    player_rank = scores->CalcPursuitRank(rank_type, career_view);
    Attrib::Key key;
    unsigned int value_label;
    switch (static_cast<int>(rank_type)) {
    case 0:
        if (career_view) {
            key = Attrib::StringToKey("pursuit_length_in_pursuit");
        } else {
            key = Attrib::StringToKey("pursuit_length");
        }
        value_label = 0xD70811D1;
        break;
    case 1:
        if (career_view) {
            key = Attrib::StringToKey("cops_involved_in_pursuit");
        } else {
            key = Attrib::StringToKey("cops_involved");
        }
        value_label = 0xC6113FCF;
        break;
    case 2:
        if (career_view) {
            key = Attrib::StringToKey("cops_damaged_in_pursuit");
        } else {
            key = Attrib::StringToKey("cops_damaged");
        }
        value_label = 0x2A1815D9;
        break;
    case 3:
        if (career_view) {
            key = Attrib::StringToKey("cops_destroyed_in_pursuit");
        } else {
            key = Attrib::StringToKey("cops_destroyed");
        }
        value_label = 0x189EAF7B;
        break;
    case 4:
        if (career_view) {
            key = Attrib::StringToKey("tire_spikes_dodged_in_pursuit");
        } else {
            key = Attrib::StringToKey("tire_spikes_dodged");
        }
        value_label = 0xDCD6B9BA;
        break;
    case 5:
        if (career_view) {
            key = Attrib::StringToKey("roadblocks_dodged_in_pursuit");
        } else {
            key = Attrib::StringToKey("roadblocks_dodged");
        }
        value_label = 0x9EF589BE;
        break;
    case 6:
        if (career_view) {
            key = Attrib::StringToKey("helis_involved_in_pursuit");
        } else {
            key = Attrib::StringToKey("helis_involved");
        }
        value_label = 0x39A1413C;
        break;
    case 7:
        if (career_view) {
            key = Attrib::StringToKey("cost_to_state_in_pursuit");
        } else {
            key = Attrib::StringToKey("cost_to_state");
        }
        value_label = 0xB3F963F8;
        break;
    case 8:
        if (career_view) {
            key = Attrib::StringToKey("total_infractions_in_pursuit");
        } else {
            key = Attrib::StringToKey("total_infractions");
        }
        value_label = 0xE34B2E6F;
        break;
    case 9:
        if (career_view) {
            key = Attrib::StringToKey("bounty_in_pursuit");
        } else {
            key = Attrib::StringToKey("bounty");
        }
        value_label = 0x48B4B99C;
        break;
    default:
        key = 0;
        value_label = 0;
        break;
    }
    Attrib::Gen::frontend rankingsData(key, 0, nullptr);
    if (rankingsData.IsValid()) {
        int last = rankingsData.Num_RapSheetRanks();
        if (last == 15) {
            int num_rankings_to_show = rankingsData.Num_RapSheetRanks();
            int rival_offset = 0;
            int player_rank_index = player_rank - 1;
            if (player_rank == 0x10) {
                num_rankings_to_show = 0x10;
            }
            for (int i = 0; i < num_rankings_to_show; i++) {
                if (i == player_rank_index) {
                    unsigned int car_hash = 0;
                    int player_value;
                    if (career_view) {
                        player_value = scores->GetCareerPursuitScore(rank_type);
                    } else {
                        car_hash = GetFECarNameHashFromFEKey(scores->GetBestPursuitScore(rank_type).CarFEKey);
                        player_value = scores->GetBestPursuitScore(rank_type).Value;
                    }

                    float value;
                    if (rank_type == ePDT_CostToState) {
                        value = static_cast<float>(player_value) * 0.00025f;
                    } else {
                            value = static_cast<float>(player_value);
                    }

                    AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(player_rank, 1, car_hash, value));
                    rival_offset--;
                } else {
                    int rank_index = i + rival_offset;
                    unsigned int name_hash = FEngHashString("BLACKLIST_RIVAL_%.2d_AKA", static_cast<int>(rankingsData.NameId(rank_index)));
                    unsigned int car_hash;
                    if (career_view) {
                        car_hash = 0;
                    } else {
                        car_hash = FEngHashString("BLACKLIST_RIVAL_%.2d_CAR", static_cast<int>(rankingsData.NameId(rank_index)));
                    }
                    AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(i + 1, name_hash, car_hash,
                                                                          rankingsData.RapSheetRanks(rank_index)));
                }
            }

            SetInitialPosition(0);
            int dist_off_screen = player_rank - GetHeight() + 4;
            for (; dist_off_screen > 0; dist_off_screen--) {
                ScrollDown();
            }
        }
    }
    FEngSetLanguageHash(GetPackageName(), 0x8224E17C, value_label);
    UpdateHighlight();
    ArrayScroller* scroller = this;
    scroller->RefreshHeader();
}
void uiRapSheetRankingsDetail::RefreshHeader() {
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEngSetLanguageHash(GetPackageName(), 0x1E4FDA, career_view ? 0x96DDF504 : 0x56E940F4);
    FEngSetLanguageHash(GetPackageName(), 0xDD2F4FB, career_view ? 0x554BBDB5 : 0xA88B3FC5);
    FEngSetLanguageHash(GetPackageName(), 0x9AE9B5CD, career_view ? 0x554BBDB5 : 0xA88B3FC5);
    ArrayScrollerMenu::RefreshHeader();
}
void uiRapSheetRankingsDetail::UpdateHighlight() {
    int highlight = player_rank - GetStartDatumNum();
    if (highlight > 0) {
        int numSlots = GetNumSlots();
        if (highlight <= numSlots) {
            cFEng::Get()->QueuePackageMessage(FEngHashString("RAPSHEET_HIGHLIGHT_%d", highlight), nullptr, nullptr);
            return;
        }
    }
    cFEng::Get()->QueuePackageMessage(0x58B123F7, nullptr, nullptr);
}
