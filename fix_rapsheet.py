#!/usr/bin/env python3
"""Fix rapsheet source files - chmod + apply edits."""
import os
import stat

BASE = "/Users/johannberger/nfsmw-zFE/src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career"

files = [
    "uiRapSheetUS.cpp",
    "uiRapSheetVD.cpp",
    "uiRapSheetCTS.cpp",
    "uiRapSheetRankings.cpp",
    "uiRapSheetRankingsDetail.cpp",
]

# chmod u+w
for f in files:
    path = os.path.join(BASE, f)
    st = os.stat(path)
    os.chmod(path, st.st_mode | stat.S_IWUSR)
    print(f"chmod u+w {f}")

print("All files made writable. Now applying edits...")

# 1. uiRapSheetUS.cpp - view_unserved(false) -> view_unserved(true)
p = os.path.join(BASE, "uiRapSheetUS.cpp")
txt = open(p).read()
txt = txt.replace("view_unserved(false)", "view_unserved(true)", 1)
open(p, "w").write(txt)
print("1. uiRapSheetUS.cpp: view_unserved(false) -> view_unserved(true)")

# 2. uiRapSheetVD.cpp - ArrayScrollerMenu(sd, 1, 5, false) -> (sd, 1, 2, false)
p = os.path.join(BASE, "uiRapSheetVD.cpp")
txt = open(p).read()
txt = txt.replace(
    "uiRapSheetVD::uiRapSheetVD(ScreenConstructorData* sd) : ArrayScrollerMenu(sd, 1, 5, false) {",
    "uiRapSheetVD::uiRapSheetVD(ScreenConstructorData* sd) : ArrayScrollerMenu(sd, 1, 2, false) {",
    1
)
open(p, "w").write(txt)
print("2. uiRapSheetVD.cpp: height 5 -> 2")

# 3. uiRapSheetCTS.cpp - add ArrayScrollerMenu::NotificationMessage call
p = os.path.join(BASE, "uiRapSheetCTS.cpp")
txt = open(p).read()
old = """void uiRapSheetCTS::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0xE1FDE1D1) { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); }
}"""
new = """void uiRapSheetCTS::NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) {
    ArrayScrollerMenu::NotificationMessage(msg, pobj, param1, param2);
    if (msg == 0xE1FDE1D1) { cFEng::Get()->QueuePackageSwitch("RapSheetMain.fng", 0, 0, false); }
}"""
txt = txt.replace(old, new, 1)
open(p, "w").write(txt)
print("3. uiRapSheetCTS.cpp: added ArrayScrollerMenu::NotificationMessage call")

# 4a. uiRapSheetRankings.cpp - add GetProfileName FEPrintf in RefreshHeader
p = os.path.join(BASE, "uiRapSheetRankings.cpp")
txt = open(p).read()
old = """    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    unsigned int best_hash = career_view ? 0x96DDF504 : 0x56E940F4;"""
new = """    FEPrintf(GetPackageName(), 0x1232703A, GetLocalizedString(0xE21D083C), prof->GetCareer()->GetCaseFileName());
    FEPrintf(GetPackageName(), 0xEB406FEC, GetLocalizedString(0x6031106E), prof->GetProfileName());
    unsigned int best_hash = career_view ? 0x96DDF504 : 0x56E940F4;"""
txt = txt.replace(old, new, 1)

# 4b. PrintRanking - add rank + 1 argument
old = '    else { FEPrintf(GetPackageName(), fe_rank, "%d"); }'
new = '    else { FEPrintf(GetPackageName(), fe_rank, "%d", rank + 1); }'
txt = txt.replace(old, new, 1)
open(p, "w").write(txt)
print("4. uiRapSheetRankings.cpp: added GetProfileName FEPrintf + rank+1 arg")

# 5. uiRapSheetRankingsDetail.cpp - rewrite Setup() + add forward declaration
p = os.path.join(BASE, "uiRapSheetRankingsDetail.cpp")
txt = open(p).read()

# Add forward declaration after the existing forward declarations
old_fwd = "const char* GetLocalizedString(unsigned int hash);"
new_fwd = """const char* GetLocalizedString(unsigned int hash);
unsigned int GetFECarNameHashFromFEKey(unsigned int fekey);"""
txt = txt.replace(old_fwd, new_fwd, 1)

# Replace entire Setup function
old_setup = """void uiRapSheetRankingsDetail::Setup() {
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
}"""

new_setup = """void uiRapSheetRankingsDetail::Setup() {
    ClearData();
    UserProfile* prof = FEDatabase->GetUserProfile(0);
    int rank = prof->GetHighScores()->CalcPursuitRank(rank_type, career_view);
    player_rank = rank;
    unsigned int category_str_hash = 0;
    unsigned int type_key = 0;
    switch (static_cast<int>(rank_type)) {
    case 0: type_key = Attrib::StringToKey(career_view ? "rap_sheet_cost_to_state_career" : "rap_sheet_cost_to_state_all"); category_str_hash = 0xD70811D1; break;
    case 1: type_key = Attrib::StringToKey(career_view ? "rap_sheet_bounty_career" : "rap_sheet_bounty_all"); category_str_hash = 0xC6113FCF; break;
    case 2: type_key = Attrib::StringToKey(career_view ? "rap_sheet_infractions_career" : "rap_sheet_infractions_all"); category_str_hash = 0x2A1815D9; break;
    case 3: type_key = Attrib::StringToKey(career_view ? "rap_sheet_speeding_career" : "rap_sheet_speeding_all"); category_str_hash = 0x189EAF7B; break;
    case 4: type_key = Attrib::StringToKey(career_view ? "rap_sheet_roadblocks_career" : "rap_sheet_roadblocks_all"); category_str_hash = 0xDCD6B9BA; break;
    case 5: type_key = Attrib::StringToKey(career_view ? "rap_sheet_cops_disabled_career" : "rap_sheet_cops_disabled_all"); category_str_hash = 0x9EF589BE; break;
    case 6: type_key = Attrib::StringToKey(career_view ? "rap_sheet_spike_strips_career" : "rap_sheet_spike_strips_all"); category_str_hash = 0x39A1413C; break;
    case 7: type_key = Attrib::StringToKey(career_view ? "rap_sheet_cops_deployed_career" : "rap_sheet_cops_deployed_all"); category_str_hash = 0xE34B2E6F; break;
    case 8: type_key = Attrib::StringToKey(career_view ? "rap_sheet_helicopters_career" : "rap_sheet_helicopters_all"); category_str_hash = 0xB3F963F8; break;
    case 9: type_key = Attrib::StringToKey(career_view ? "rap_sheet_pursuit_length" : "rap_sheet_pursuit_length"); category_str_hash = 0x48B4B99C; break;
    default: break;
    }
    Attrib::Gen::frontend rapsheet(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), type_key), 0, nullptr);
    if (rapsheet.IsValid()) {
        unsigned int numRanks = rapsheet.Num_RapSheetRanks();
        if (numRanks == 15) {
            int rival_offset = 0;
            int num_rankings_to_show = 15;
            int player_rank_index = player_rank;
            if (player_rank_index == 0x10) {
                num_rankings_to_show = 0x10;
            }
            for (int i = 0; i < num_rankings_to_show; i++) {
                if (i == player_rank_index - 1) {
                    unsigned int car_name_hash = 0;
                    int tmp_player_value;
                    if (!career_view) {
                        car_name_hash = GetFECarNameHashFromFEKey(prof->GetHighScores()->GetBestPursuitScore(rank_type).CarFEKey);
                        tmp_player_value = prof->GetHighScores()->GetBestPursuitScore(rank_type).Value;
                    } else {
                        tmp_player_value = prof->GetHighScores()->GetCareerPursuitScore(rank_type);
                    }
                    float player_value;
                    if (rank_type == ePDT_CostToState) {
                        player_value = static_cast<float>(tmp_player_value) * 0.001f;
                    } else {
                        player_value = static_cast<float>(tmp_player_value);
                    }
                    rival_offset--;
                    AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(player_rank, 1, car_name_hash, player_value));
                } else {
                    int adjusted = i + rival_offset;
                    unsigned int aka_name = FEngHashString("AKA_%d", static_cast<int>(rapsheet.NameId(static_cast<unsigned int>(adjusted))));
                    unsigned int car_name = 0;
                    if (!career_view) {
                        car_name = FEngHashString("RIVAL_CAR_%d", static_cast<int>(rapsheet.NameId(static_cast<unsigned int>(adjusted))));
                    }
                    float val = rapsheet.RapSheetRanks(static_cast<unsigned int>(adjusted));
                    AddDatum(new(__FILE__, __LINE__) RapSheetRankingsDatum(i + 1, aka_name, car_name, val));
                }
            }
            SetInitialPosition(0);
            int dist_off_screen = (player_rank - GetHeight()) + 4;
            for (; dist_off_screen > 0; dist_off_screen--) {
                ScrollDown();
            }
        }
    }
    FEngSetLanguageHash(GetPackageName(), 0x8224E17C, category_str_hash);
    UpdateHighlight();
    RefreshHeader();
}"""

txt = txt.replace(old_setup, new_setup, 1)
open(p, "w").write(txt)
print("5. uiRapSheetRankingsDetail.cpp: rewrote Setup() + added forward declaration")

print("\nAll edits applied successfully!")
