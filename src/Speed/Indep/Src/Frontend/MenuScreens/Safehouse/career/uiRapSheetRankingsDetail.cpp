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
    int rank = prof->GetHighScores()->CalcPursuitRank(rank_type, career_view);
    player_rank = rank;
    const char* attrib_name = nullptr;
    unsigned int value_label = 0;
    switch (static_cast<int>(rank_type)) {
    case 0: attrib_name = career_view ? "rap_sheet_cost_to_state_career" : "rap_sheet_cost_to_state_all"; value_label = 0xD70811D1; break;
    case 1: attrib_name = career_view ? "rap_sheet_bounty_career" : "rap_sheet_bounty_all"; value_label = 0xC6113FCF; break;
    case 2: attrib_name = career_view ? "rap_sheet_infractions_career" : "rap_sheet_infractions_all"; value_label = 0x2A1815D9; break;
    case 3: attrib_name = career_view ? "rap_sheet_speeding_career" : "rap_sheet_speeding_all"; value_label = 0x189EAF7B; break;
    case 4: attrib_name = career_view ? "rap_sheet_roadblocks_career" : "rap_sheet_roadblocks_all"; value_label = 0xDCD6B9BA; break;
    case 5: attrib_name = career_view ? "rap_sheet_cops_disabled_career" : "rap_sheet_cops_disabled_all"; value_label = 0x9EF589BE; break;
    case 6: attrib_name = career_view ? "rap_sheet_spike_strips_career" : "rap_sheet_spike_strips_all"; value_label = 0x39A1413C; break;
    case 7: attrib_name = career_view ? "rap_sheet_cops_deployed_career" : "rap_sheet_cops_deployed_all"; value_label = 0xE34B2E6F; break;
    case 8: attrib_name = career_view ? "rap_sheet_helicopters_career" : "rap_sheet_helicopters_all"; value_label = 0xB3F963F8; break;
    case 9: attrib_name = career_view ? "rap_sheet_pursuit_length" : "rap_sheet_pursuit_length"; value_label = 0x48B4B99C; break;
    default: break;
    }
    Attrib::Key key = attrib_name ? Attrib::StringToKey(attrib_name) : 0;
    Attrib::Gen::frontend rankingsData(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
    if (rankingsData.IsValid()) {
        unsigned int numRanks = rankingsData.Num_RapSheetRanks();
        if (numRanks == 15) {
            for (int i = 0; i < 15; i++) {
                unsigned int item_num;
                unsigned int player_name;
                unsigned int car_hash = 0;
                float val = rankingsData.RapSheetRanks(static_cast<unsigned int>(i));
                if (player_rank == 0x10 || i < player_rank) {
                    item_num = i + 1;
                    player_name = rankingsData.Num_NameId() > static_cast<unsigned int>(i) ? *reinterpret_cast<const unsigned int*>(&rankingsData.NameId(static_cast<unsigned int>(i))) : 0;
                } else if (i == player_rank) {
                    item_num = i + 1;
                    player_name = 1;
                } else {
                    item_num = i + 1;
                    player_name = rankingsData.Num_NameId() > static_cast<unsigned int>(i - 1) ? *reinterpret_cast<const unsigned int*>(&rankingsData.NameId(static_cast<unsigned int>(i - 1))) : 0;
                }
                AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(item_num, player_name, car_hash, val));
            }
        }
    }
    FEngSetLanguageHash(GetPackageName(), 0x9D974DF3, value_label);
    SetInitialPosition(0);
    RefreshHeader();
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
    int numSlots = GetNumSlots();
    if (highlight < 1 || numSlots < highlight) { cFEng::Get()->QueuePackageMessage(0x58B123F7, nullptr, nullptr); }
    else { cFEng::Get()->QueuePackageMessage(FEngHashString("RAPSHEET_HIGHLIGHT_%d", highlight), nullptr, nullptr); }
}
