#include "LobbyUsers.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

extern "C" {
int TagFieldGetStructure(const char *field, void *buffer, int bufferSize, const char *format);
void TagFieldSetFloat(char *record, int recordLength, const char *name, float value);
}

LobbyUsers *pLobbyUsersInstance;

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

uint32 PlayerDataT::GetTotalWinsAsClient() {
    uint32 sum = 0;
    for (int mode = 0; mode < 3; mode++) {
        sum += stats.raceModeStats[mode].clientWins;
    }
    return sum;
}

uint32 PlayerDataT::GetTotalWinsAsHost() {
    uint32 sum = 0;
    for (int mode = 0; mode < 3; mode++) {
        sum += stats.raceModeStats[mode].hostWins;
    }
    return sum;
}

uint32 PlayerDataT::GetTotalWins(int mode) {
    uint32 sum = 0;
    if (mode == 3) {
        for (int i = 0; i < 3; i++) {
            sum += stats.raceModeStats[i].clientWins + stats.raceModeStats[i].hostWins;
        }
        return sum;
    }
    return stats.raceModeStats[mode].clientWins + stats.raceModeStats[mode].hostWins;
}

uint32 PlayerDataT::GetTotalLossesAsClient() {
    uint32 sum = 0;
    for (int mode = 0; mode < 3; mode++) {
        sum += stats.raceModeStats[mode].clientLosses;
    }
    return sum;
}

uint32 PlayerDataT::GetTotalLossesAsHost() {
    uint32 sum = 0;
    for (int mode = 0; mode < 3; mode++) {
        sum += stats.raceModeStats[mode].hostLosses;
    }
    return sum;
}

uint32 PlayerDataT::GetTotalLosses(int mode) {
    uint32 sum = 0;
    if (mode == 3) {
        for (int i = 0; i < 3; i++) {
            sum += stats.raceModeStats[i].clientLosses + stats.raceModeStats[i].hostLosses;
        }
        return sum;
    }
    return stats.raceModeStats[mode].clientLosses + stats.raceModeStats[mode].hostLosses;
}

uint32 PlayerDataT::GetMonthlyPointTotals(int mode) {
    if (static_cast<uint32>(mode) > 2) {
        return 0;
    }
    return stats.raceModeStats[mode].points - lastMonthStats.raceModeStats[mode].points;
}

float PlayerDataT::GetWinPercentage(int mode) {
    uint32 gamesPlayed = GetTotalGamesPlayed(mode);
    if (gamesPlayed != 0) {
        return (static_cast<float>(GetTotalWins(mode)) / static_cast<float>(gamesPlayed)) *
               100.0f;
    }
    return 0.0f;
}

float PlayerDataT::GetTotalDisconnectPercentage() {
    uint32 gamesPlayed = GetTotalGamesPlayedAsHost();
    gamesPlayed += GetTotalGamesPlayedAsClient();
    float totalDisconnects = static_cast<float>(stats.hostDisconnects + stats.clientDisconnects);
    if (gamesPlayed != 0) {
        return (totalDisconnects / static_cast<float>(gamesPlayed)) * 100.0f;
    }
    return 0.0f;
}

float PlayerDataT::GetTopSpeedInMPS(int mode) {
    return static_cast<float>(stats.raceModeStats[mode].topSpeed) * 0.001f;
}

uint32 PlayerDataT::GetTotalNOS(int mode) { return stats.raceModeStats[mode].totalNOSused; }

float PlayerDataT::GetLongestJumpDuration(int mode) {
    return static_cast<float>(stats.raceModeStats[mode].jumpTime) * 0.001f;
}

void PlayerDataT::Clear() {
    persona[0] = '\0';
    lastGameTime = 0;
    stats.Clear();
    lastMonthStats.Clear();
}

void PlayerDataT::Stats::Clear() {
    topSpeed = 0;
    hostDisconnects = 0;
    clientDisconnects = 0;
    disqualifications = 0;
    totalNOSusage = 0;
    fastestDragFinish = 0;
    longestJumpDuration = 0;
    longestPowerSlideDistance = 0;
    for (int mode = 0; mode < 3; mode++) {
        raceModeStats[mode].Clear();
    }
}

LobbyUsers &LobbyUsers::Instance() {
    static LobbyUsers theLobbyUsers;
    pLobbyUsersInstance = &theLobbyUsers;
    return theLobbyUsers;
}

void LobbyUsers::UpdateCarName() {
    lobbyMutex.Lock("LobbyUsers::SetAuxiCarName");
    uint32 selectedCar = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SelectedCar[0];
    if (mCar != selectedCar) {
        mCar = selectedCar;
        MaybeCreateAuxiBuffer();
        FEPlayerCarDB *stable = &FEDatabase->CurrentUserProfiles[0]->PlayersCarStable;
        FECarRecord *car = stable->GetCarRecordByHandle(selectedCar);
        TagFieldSetNumber(auxiData, 128, "CN", car->GetNameHash());
        Attrib::Key collectionKey = car->VehicleKey;
        Attrib::Gen::pvehicle pvehicle(collectionKey, 0, nullptr);
        if (car->IsCustomized()) {
            FECustomizationRecord *customization =
                stable->GetCustomizationRecordByHandle(car->Customization);
            customization->WriteRecordIntoPhysics(pvehicle);
        }
        Physics::Info::Performance performance;
        Physics::Info::ComputePerformance(pvehicle, performance);
        TagFieldSetFloat(auxiData, 128, "PT", performance.TopSpeed);
        TagFieldSetFloat(auxiData, 128, "PH", performance.Handling);
        TagFieldSetFloat(auxiData, 128, "PA", performance.Acceleration);
        SendAuxiData();
    }
    lobbyMutex.Unlock(nullptr);
}
