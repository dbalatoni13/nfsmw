#include "LobbyUsers.hpp"

extern "C" {
int TagFieldGetStructure(const char *field, void *buffer, int bufferSize, const char *format);
}

int PlayerDataT::ExtractRecord(const char *record) {
    if (!record || !record[0]) {
        return -1;
    }

    Clear();
    TagFieldGetString(TagFieldFind(record, "PERS"), persona, sizeof(persona), "");
    TagFieldGetStructure(TagFieldFind(record, "STAT"), &stats, sizeof(stats), "l*");
    TagFieldGetStructure(TagFieldFind(record, "LWSTAT"), &lastWeekStats, sizeof(lastWeekStats),
                         "l*");
    TagFieldGetStructure(TagFieldFind(record, "LMSTAT"), &lastMonthStats,
                         sizeof(lastMonthStats), "l*");
    lastGameTime = TagFieldGetNumber(TagFieldFind(record, "LGAME"), 0);
    return 0;
}

int PlayerDataT::ExtractRecord(const LobbyApiUserT &user) {
    Clear();
    if (!user.name[0]) {
        return -1;
    }
    bStrCpy(persona, user.name);
    TagFieldGetStructure(user.stat, &stats, sizeof(stats), "l*");
    return 0;
}

uint32 PlayerDataT::GetTotalGamesPlayedAsClient() {
    return GetTotalWinsAsClient() + GetTotalLossesAsClient();
}

uint32 PlayerDataT::GetTotalGamesPlayedAsHost() {
    return GetTotalWinsAsHost() + GetTotalLossesAsHost();
}

uint32 PlayerDataT::GetTotalGamesPlayed(int mode) {
    if (mode == 3) {
        return GetTotalWins(mode) + GetTotalLosses(mode);
    }
    return (stats.raceModeStats[mode].clientLosses + stats.raceModeStats[mode].hostWins) +
           (stats.raceModeStats[mode].hostLosses + stats.raceModeStats[mode].clientWins);
}
