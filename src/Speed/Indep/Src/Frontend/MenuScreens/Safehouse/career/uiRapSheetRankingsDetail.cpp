#include "uiRapSheetRankingsDetail.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
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
        case __PAD_BUTTON5__:
            career_view = !career_view;
            Setup();
            break;
        case __PAD_DOWN__:
        case FEHASH_INITCOMPLETE:
        case __PAD_UP__:
            UpdateHighlight();
            break;
        case FEHASH_EXITCOMPLETE:
            uiRapSheetRankings::career_view = career_view;
            cFEng::Get()->QueuePackageSwitch("RapSheetRankings.fng", 0, 0, false);
            break;
    }
}

void uiRapSheetRankingsDetail::Setup() {
    this->ClearData();

    uint32 category_str_hash;
    UserProfile &prof = *FEDatabase->GetUserProfile(0);

    this->player_rank = prof.GetHighScores()->CalcPursuitRank(this->rank_type, this->career_view);

    Attrib::Key type_key;
    switch (this->rank_type) {
        case PD_PURUSIT_LENGTH:
            if (this->career_view) {
                type_key = Attrib::StringToKey("pursuit_length");
            } else {
                type_key = Attrib::StringToKey("pursuit_length_in_pursuit");
            }
            category_str_hash = 0xd70811d1;
            break;

        case PD_COPS_INVOLVED:
            if (this->career_view) {
                type_key = Attrib::StringToKey("cops_involved");
            } else {
                type_key = Attrib::StringToKey("cops_involved_in_pursuit");
            }
            category_str_hash = 0xc6113fcf;
            break;

        case PD_COPS_DAMAGED:
            if (this->career_view) {
                type_key = Attrib::StringToKey("cops_damaged");
            } else {
                type_key = Attrib::StringToKey("cops_damaged_in_pursuit");
            }
            category_str_hash = 0x2a1815d9;
            break;

        case PD_COPS_DESTROYED:
            if (this->career_view) {
                type_key = Attrib::StringToKey("cops_destroyed");
            } else {
                type_key = Attrib::StringToKey("cops_destroyed_in_pursuit");
            }
            category_str_hash = 0x189eaf7b;
            break;

        case PD_SPIKESTRIPS_DODGED:
            if (this->career_view) {
                type_key = Attrib::StringToKey("tire_spikes_dodged");
            } else {
                type_key = Attrib::StringToKey("tire_spikes_dodged_in_pursuit");
            }
            category_str_hash = 0xdcd6b9ba;
            break;

        case PD_ROADBLOCKS_DODGED:
            if (this->career_view) {
                type_key = Attrib::StringToKey("roadblocks_dodged");
            } else {
                type_key = Attrib::StringToKey("roadblocks_dodged_in_pursuit");
            }
            category_str_hash = 0x9ef589be;
            break;

        case PD_HELICOPTERS_INVOLVED:
            if (this->career_view) {
                type_key = Attrib::StringToKey("helis_involved");
            } else {
                type_key = Attrib::StringToKey("helis_involved_in_pursuit");
            }
            category_str_hash = 0x39a1413c;
            break;
        case PD_NUM_INFRACTIONS:
            if (this->career_view) {
                type_key = Attrib::StringToKey("total_infractions");
            } else {
                type_key = Attrib::StringToKey("total_infractions_in_pursuit");
            }
            category_str_hash = 0xb3f963f8;
            break;

        case PD_COST_TO_STATE:
            if (this->career_view) {
                type_key = Attrib::StringToKey("cost_to_state");
            } else {
                type_key = Attrib::StringToKey("cost_to_state_in_pursuit");
            }
            category_str_hash = 0xe34b2e6f;
            break;

        case PD_BOUNTY:
            if (this->career_view) {
                type_key = Attrib::StringToKey("bounty");
            } else {
                type_key = Attrib::StringToKey("bounty_in_pursuit");
            }
            category_str_hash = 0x48b4b99c;
            break;

        default:
            type_key = 0;
            category_str_hash = 0;
            break;
    }

    Attrib::Gen::frontend rapsheet(type_key, 0, nullptr);

    if (rapsheet.IsValid()) {
        if (rapsheet.Num_RapSheetRanks() == 15) {
            int last = rapsheet.Num_RapSheetRanks();
            int rival_offset = 0;
            bool is_time;
            int player_rank_index = this->player_rank - 1;
            int num_rankings_to_show = 15;

            if (this->player_rank == 16) {
                num_rankings_to_show = 16;
            }

            for (int i = 0; i < num_rankings_to_show; i++) {
                if (i == player_rank_index) {
                    uint32 car_name_hash = 0;
                    int tmp_player_value;
                    float player_value;

                    if (this->career_view) {
                        tmp_player_value = prof.GetHighScores()->GetCareerPursuitScore(this->rank_type);
                    } else {
                        car_name_hash = GetFECarNameHashFromFEKey(prof.GetHighScores()->GetBestPursuitScore(this->rank_type).CarFEKey);
                        tmp_player_value = prof.GetHighScores()->GetBestPursuitScore(this->rank_type).Value;
                    }

                    is_time = this->rank_type == PD_PURUSIT_LENGTH;
                    if (is_time) {
                        player_value = Timer(tmp_player_value).GetSeconds();
                    } else {
                        player_value = tmp_player_value;
                    }

                    this->AddDatum(new ("RapSheetRankingsDatum", 0) RapSheetRankingsDatum(this->player_rank, 1, car_name_hash, player_value));

                    rival_offset--;
                } else {
                    uint32 aka_name = FEngHashString("BLACKLIST_RIVAL_%.2d_AKA", rapsheet.NameId(i + rival_offset));
                    uint32 car_name;

                    if (this->career_view) {
                        car_name = 0;
                    } else {
                        car_name = FEngHashString("BLACKLIST_RIVAL_%.2d_CAR", rapsheet.NameId(i + rival_offset));
                    }

                    this->AddDatum(new ("RapSheetRankingsDatum", 0)
                                       RapSheetRankingsDatum(i + 1, aka_name, car_name, rapsheet.RapSheetRanks(i + rival_offset)));
                }
            }

            this->SetInitialPosition(0);

            int dist_off_screen = this->player_rank - this->GetHeight() + 4;
            for (; dist_off_screen > 0; dist_off_screen--) {
                this->ScrollDown();
            }
        }
    }

    FEngSetLanguageHash(this->GetPackageName(), 0x8224e17c, category_str_hash);
    this->UpdateHighlight();
    this->RefreshHeader();
}

void uiRapSheetRankingsDetail::RefreshHeader() {
    UserProfile &prof = *FEDatabase->GetUserProfile(0);
    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof.GetCareer()->GetCaseFileName());
    FEngSetLanguageHash(GetPackageName(), 0x1E4FDA, career_view ? 0x96DDF504 : 0x56E940F4);
    FEngSetLanguageHash(GetPackageName(), 0xDD2F4FB, career_view ? 0x554BBDB5 : 0xA88B3FC5);
    FEngSetLanguageHash(GetPackageName(), 0x9AE9B5CD, career_view ? 0x554BBDB5 : 0xA88B3FC5);
    ArrayScrollerMenu::RefreshHeader();
}

void uiRapSheetRankingsDetail::UpdateHighlight() {
    int player_pos = player_rank - GetStartDatumNum();
    if (player_pos > 0) {
        if (player_pos <= GetNumSlots()) {
            cFEng::Get()->QueuePackageMessage(FEngHashString("POS%d", player_pos), nullptr, nullptr);
            return;
        }
    }
    cFEng::Get()->QueuePackageMessage(0x58B123F7, nullptr, nullptr);
}
