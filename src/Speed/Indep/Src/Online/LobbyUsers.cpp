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
