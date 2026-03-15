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
        if (d->getItemNum() == 0x10) { FEngSetLanguageHash(pItemNum, 0xFC1BF40); }
        else { FEPrintf(pItemNum, "%d"); }
        if (d->getCarName() == 0) { FEPrintf(pCarName, ""); }
        else { FEngSetLanguageHash(pCarName, d->getCarName()); }
        if (d->getPlayerName() == 1) { FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName()); }
        else { FEngSetLanguageHash(pPlayerName, d->getPlayerName()); }
    }
}
void RapSheetRankingsTimerArraySlot::Update(ArrayDatum* datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetRankingsDatum* d = static_cast<RapSheetRankingsDatum*>(datum);
        if (d->getItemNum() == 0x10) { FEPrintf(pItemNum, "#"); }
        else { FEPrintf(pItemNum, "%d"); }
        if (d->getCarName() == 0) { FEPrintf(pCarName, ""); }
        else { FEngSetLanguageHash(pCarName, d->getCarName()); }
        Timer t;
        t.SetTime(d->getValue());
        char time_str[16];
        t.PrintToString(time_str, 16);
        FEPrintf(pValue, "%s", time_str);
        if (d->getPlayerName() == 1) { FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName()); }
        else { FEngSetLanguageHash(pPlayerName, d->getPlayerName()); }
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
    case 0x911C0A4B:
    case 0x35F8620B:
    case 0x72619778:
        UpdateHighlight();
        break;
    case 0xC519BFC4:
        career_view = !career_view;
        Setup();
        break;
    case 0xE1FDE1D1:
        uiRapSheetRankings::career_view = career_view;
        cFEng::Get()->QueuePackageSwitch("RapSheetRankings.fng", 0, 0, false);
        break;
    }
}
void uiRapSheetRankingsDetail::Setup() {
    ClearData();
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    HighScoresDatabase* scores = prof->GetHighScores();
    int rank = scores->CalcPursuitRank(rank_type, career_view);
    player_rank = rank;
    const char* attrib_name = nullptr;
    unsigned int value_label = 0;
    switch (static_cast<int>(rank_type)) {
    case 0: attrib_name = career_view ? "pursuit_length" : "pursuit_length_in_pursuit"; value_label = 0xD70811D1; break;
    case 1: attrib_name = career_view ? "cops_involved" : "cops_involved_in_pursuit"; value_label = 0xC6113FCF; break;
    case 2: attrib_name = career_view ? "cops_damaged" : "cops_damaged_in_pursuit"; value_label = 0x2A1815D9; break;
    case 3: attrib_name = career_view ? "cops_destroyed" : "cops_destroyed_in_pursuit"; value_label = 0x189EAF7B; break;
    case 4: attrib_name = career_view ? "tire_spikes_dodged" : "tire_spikes_dodged_in_pursuit"; value_label = 0xDCD6B9BA; break;
    case 5: attrib_name = career_view ? "roadblocks_dodged" : "roadblocks_dodged_in_pursuit"; value_label = 0x9EF589BE; break;
    case 6: attrib_name = career_view ? "helis_involved" : "helis_involved_in_pursuit"; value_label = 0x39A1413C; break;
    case 7: attrib_name = career_view ? "cost_to_state" : "cost_to_state_in_pursuit"; value_label = 0xE34B2E6F; break;
    case 8: attrib_name = career_view ? "total_infractions" : "total_infractions_in_pursuit"; value_label = 0xB3F963F8; break;
    case 9: attrib_name = career_view ? "bounty" : "bounty_in_pursuit"; value_label = 0x48B4B99C; break;
    default: break;
    }
    Attrib::Key key = attrib_name ? Attrib::StringToKey(attrib_name) : 0;
    Attrib::Gen::frontend rankingsData(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
    if (rankingsData.IsValid()) {
        unsigned int numRanks = rankingsData.Num_RapSheetRanks();
        if (numRanks == 15) {
            int num_rows = 15;
            int rank_shift = 0;
            if (player_rank == 0x10) {
                num_rows = 0x10;
            }
            for (int i = 0; i < num_rows; i++) {
                if (i == player_rank - 1) {
                    unsigned int car_hash = 0;
                    int player_value;
                    if (career_view) {
                        player_value = scores->GetCareerPursuitScore(rank_type);
                    } else {
                        const PursuitScore& best_score = scores->GetBestPursuitScore(rank_type);
                        car_hash = GetFECarNameHashFromFEKey(best_score.CarFEKey);
                        player_value = best_score.Value;
                    }

                    float value = static_cast<float>(player_value);
                    if (rank_type == ePDT_CostToState) {
                        value = Timer(player_value).GetSeconds();
                    }

                    AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(i + 1, 1, car_hash, value));
                    rank_shift--;
                } else {
                    int rank_index = i + rank_shift;
                    unsigned int name_hash = 0;
                    unsigned int car_hash = 0;
                    if (rankingsData.Num_NameId() > static_cast<unsigned int>(rank_index)) {
                        int rival_id = rankingsData.NameId(static_cast<unsigned int>(rank_index));
                        name_hash = FEngHashString("BLACKLIST_RIVAL_%.2d_AKA", rival_id);
                        if (!career_view) {
                            car_hash = FEngHashString("BLACKLIST_RIVAL_%.2d_CAR", rival_id);
                        }
                    }
                    float value = rankingsData.RapSheetRanks(static_cast<unsigned int>(rank_index));
                    AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(i + 1, name_hash, car_hash, value));
                }
            }

            SetInitialPosition(0);
            for (int i = player_rank - GetHeight() + 4; i > 0; i--) {
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
    unsigned int best_hash = career_view ? 0x96DDF504 : 0x56E940F4;
    FEngSetLanguageHash(GetPackageName(), 0x1E4FDA, best_hash);
    unsigned int toggle_hash = career_view ? 0x554BBDB5 : 0xA88B3FC5;
    FEngSetLanguageHash(GetPackageName(), 0xDD2F4FB, toggle_hash);
    FEngSetLanguageHash(GetPackageName(), 0x9AE9B5CD, toggle_hash);
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
