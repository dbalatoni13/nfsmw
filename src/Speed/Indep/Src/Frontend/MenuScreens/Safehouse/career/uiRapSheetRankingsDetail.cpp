#include "uiRapSheetRankingsDetail.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "uiRapSheetRankings.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"

bool uiRapSheetRankingsDetail::career_view = false;

void RapSheetRankingsArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetRankingsDatum *dat = static_cast<RapSheetRankingsDatum *>(datum);
        FEPrintf(pValue, "%$.0f", dat->getValue());
        if (dat->getItemNum() != 0x10) {
            FEPrintf(pItemNum, "%$d", dat->getItemNum());
        } else {
            FEngSetLanguageHash(pItemNum, 0xFC1BF40);
        }
        if (dat->getCarName() != 0) {
            FEngSetLanguageHash(pCarName, dat->getCarName());
        } else {
            FEPrintf(pCarName, "");
        }
        if (dat->getPlayerName() != 1) {
            FEngSetLanguageHash(pPlayerName, dat->getPlayerName());
        } else {
            FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName());
        }
    }
}

void RapSheetRankingsTimerArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        RapSheetRankingsDatum *dat = static_cast<RapSheetRankingsDatum *>(datum);
        if (dat->getItemNum() != 0x10) {
            FEPrintf(pItemNum, "%$d", dat->getItemNum());
        } else {
            FEPrintf(pItemNum, "--");
        }
        if (dat->getCarName() != 0) {
            FEngSetLanguageHash(pCarName, dat->getCarName());
        } else {
            FEPrintf(pCarName, "");
        }
        char time_str[16];
        Timer(dat->getValue()).PrintToString(time_str, 16);
        FEPrintf(pValue, "%s", time_str);
        if (dat->getPlayerName() != 1) {
            FEngSetLanguageHash(pPlayerName, dat->getPlayerName());
        } else {
            FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName());
        }
    }
}

uiRapSheetRankingsDetail::uiRapSheetRankingsDetail(ScreenConstructorData *sd)
    : ArrayScrollerMenu(sd, 1, 10, false), rank_type(static_cast<ePursuitDetailTypes>(sd->Arg)), player_rank(0x10) {
    for (int i = 0; i < GetWidth() * GetHeight(); i++) {
        FEString *pItemNum = FEngFindString(GetPackageName(), FEngHashString("RANK_%d", i + 1));
        FEString *pPlayerName = FEngFindString(GetPackageName(), FEngHashString("PLAYER_NAME_%d", i + 1));
        FEString *pCarName = FEngFindString(GetPackageName(), FEngHashString("VEHICLE_%d", i + 1));
        FEString *pValue = FEngFindString(GetPackageName(), FEngHashString("TIME_%d", i + 1));
        if (rank_type == PD_PURUSIT_LENGTH) {
            AddSlot(new ("RapSheetRankingsTimerArraySlot", 0) RapSheetRankingsTimerArraySlot(pItemNum, pPlayerName, pCarName, pValue));
        } else {
            AddSlot(new ("RapSheetRankingsArraySlot", 0) RapSheetRankingsArraySlot(pItemNum, pPlayerName, pCarName, pValue));
        }
    }
    Setup();
}

void uiRapSheetRankingsDetail::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
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
    unsigned int value_label;
    UserProfile &prof = *FEDatabase->GetUserProfile(0);
    Attrib::Key key;
    player_rank = prof.GetHighScores()->CalcPursuitRank(rank_type, career_view);
    switch (static_cast<int>(rank_type)) {
        case 0:
            if (career_view) {
                key = Attrib::StringToKey("pursuit_length");
            } else {
                key = Attrib::StringToKey("pursuit_length_in_pursuit");
            }
            value_label = 0xD70811D1;
            break;
        case 1:
            if (career_view) {
                key = Attrib::StringToKey("cops_involved");
            } else {
                key = Attrib::StringToKey("cops_involved_in_pursuit");
            }
            value_label = 0xC6113FCF;
            break;
        case 2:
            if (career_view) {
                key = Attrib::StringToKey("cops_damaged");
            } else {
                key = Attrib::StringToKey("cops_damaged_in_pursuit");
            }
            value_label = 0x2A1815D9;
            break;
        case 3:
            if (career_view) {
                key = Attrib::StringToKey("cops_destroyed");
            } else {
                key = Attrib::StringToKey("cops_destroyed_in_pursuit");
            }
            value_label = 0x189EAF7B;
            break;
        case 4:
            if (career_view) {
                key = Attrib::StringToKey("tire_spikes_dodged");
            } else {
                key = Attrib::StringToKey("tire_spikes_dodged_in_pursuit");
            }
            value_label = 0xDCD6B9BA;
            break;
        case 5:
            if (career_view) {
                key = Attrib::StringToKey("roadblocks_dodged");
            } else {
                key = Attrib::StringToKey("roadblocks_dodged_in_pursuit");
            }
            value_label = 0x9EF589BE;
            break;
        case 6:
            if (career_view) {
                key = Attrib::StringToKey("helis_involved");
            } else {
                key = Attrib::StringToKey("helis_involved_in_pursuit");
            }
            value_label = 0x39A1413C;
            break;
        case 8:
            if (career_view) {
                key = Attrib::StringToKey("total_infractions");
            } else {
                key = Attrib::StringToKey("total_infractions_in_pursuit");
            }
            value_label = 0xB3F963F8;
            break;
        case 7:
            if (career_view) {
                key = Attrib::StringToKey("cost_to_state");
            } else {
                key = Attrib::StringToKey("cost_to_state_in_pursuit");
            }
            value_label = 0xE34B2E6F;
            break;
        case 9:
            if (career_view) {
                key = Attrib::StringToKey("bounty");
            } else {
                key = Attrib::StringToKey("bounty_in_pursuit");
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
        int rival_offset;
        int player_rank_index;
        int num_rankings_to_show;
        int dist_off_screen;
        if (last == 15) {
            int i;
            rival_offset = 0;
            num_rankings_to_show = rankingsData.Num_RapSheetRanks();
            num_rankings_to_show = last;
            player_rank_index = player_rank - 1;
            if (player_rank == 0x10) {
                num_rankings_to_show = 0x10;
            }
            for (i = 0; i < num_rankings_to_show; i++) {
                if (i == player_rank_index) {
                    unsigned int car_name_hash = 0;
                    int tmp_player_value;
                    float player_value;
                    if (career_view) {
                        tmp_player_value = prof.GetHighScores()->GetCareerPursuitScore(rank_type);
                    } else {
                        car_name_hash = GetFECarNameHashFromFEKey(prof.GetHighScores()->GetBestPursuitScore(rank_type).CarFEKey);
                        tmp_player_value = prof.GetHighScores()->GetBestPursuitScore(rank_type).Value;
                    }

                    if (rank_type == PD_PURUSIT_LENGTH) {
                        player_value = static_cast<float>(tmp_player_value) * 0.00025f;
                    } else {
                        player_value = static_cast<float>(tmp_player_value);
                    }

                    AddDatum(new ("RapSheetRankingsDatum", 0) RapSheetRankingsDatum(player_rank, 1, car_name_hash, player_value));
                    rival_offset--;
                } else {
                    unsigned int aka_name =
                        FEngHashString("BLACKLIST_RIVAL_%.2d_AKA", static_cast<int>(rankingsData.NameId(i + rival_offset)));
                    unsigned int car_name;
                    if (career_view) {
                        car_name = 0;
                    } else {
                        car_name =
                            FEngHashString("BLACKLIST_RIVAL_%.2d_CAR", static_cast<int>(rankingsData.NameId(i + rival_offset)));
                    }
                    AddDatum(new ("RapSheetRankingsDatum", 0)
                                 RapSheetRankingsDatum(i + 1, aka_name, car_name, rankingsData.RapSheetRanks(i + rival_offset)));
                }
            }

            SetInitialPosition(0);
            dist_off_screen = player_rank - GetHeight() + 4;
            for (; dist_off_screen > 0; dist_off_screen--) {
                ScrollDown();
            }
        }
    }
    FEngSetLanguageHash(GetPackageName(), 0x8224E17C, value_label);
    UpdateHighlight();
    ArrayScroller *scroller = this;
    scroller->RefreshHeader();
}

void uiRapSheetRankingsDetail::RefreshHeader() {
    UserProfile *prof = FEDatabase->GetUserProfile(0);
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
            cFEng::Get()->QueuePackageMessage(FEngHashString("POS%d", highlight), nullptr, nullptr);
            return;
        }
    }
    cFEng::Get()->QueuePackageMessage(0x58B123F7, nullptr, nullptr);
}
