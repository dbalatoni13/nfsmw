#include "LobbyUsers.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Physics/PhysicsInfo.hpp"

void *operator new(size_t size, const char *file, int line, int allocationParams);

extern "C" {
int TagFieldGetStructure(const char *field, void *buffer, int bufferSize, const char *format);
float TagFieldGetFloat(const char *field, float defaultValue);
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

int32 LobbyUsers::GetUserOnlineRecord(const char *persona, CommandCBFunc func, void *context) {
    lobbyMutex.Lock("LobbyUsers::GetUserOnlineRecord");
    if (bStrCmp(persona, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
        lobbyMutex.Unlock("LobbyUsers::GetUserOnlineRecord");
        return 0;
    }

    OnlineUsersData *oud;
    for (oud = userList.GetHead(); oud != userList.EndOfList(); oud = oud->GetNext()) {
        if (bStrCmp(oud->user.name, persona) == 0) {
            int32 rc = oud->commandID;
            lobbyMutex.Unlock("LobbyUsers::GetUserOnlineRecord");
            return rc;
        }
    }

    if (userList.CountElements() == 32) {
        oud = userList.GetTail();
        userList.Remove(oud);
        userList.AddHead(oud);
        oud->Reset();
        bStrCpy(oud->user.name, persona);
    } else {
        oud = new ("d:/p4_apex1666_d1001856/mw/speed/indep/src/online/LobbyUsers.cpp", 0x84,
                   8) OnlineUsersData;
        if (!oud) {
            lobbyMutex.Unlock("LobbyUsers::GetUserOnlineRecord");
            return -1;
        }
        bStrCpy(oud->user.name, persona);
        userList.AddHead(oud);
    }

    char buf[64] = "";
    TagFieldSetString(buf, sizeof(buf), "PERS", persona);
    int32 rc = oud->commandID =
        LobbyCore::Instance().QueueCommand('onln', buf, OnlnCB, this, func, context, false);
    if (rc == -1) {
        oud->Reset();
        lobbyMutex.Unlock("LobbyUsers::GetUserOnlineRecord");
        return -1;
    }
    lobbyMutex.Unlock("LobbyUsers::GetUserOnlineRecord");
    return rc;
}

void LobbyUsers::ClearUserOnlineRecordCache() {
    OnlineUsersData *oud;
    while ((oud = userList.GetHead()) != userList.EndOfList()) {
        oud = userList.Remove(oud);
        delete oud;
    }
}

void LobbyUsers::ClearUserOnlineRecordCache(const LobbyApiPlayT &game) {
    OnlineUsersData *oud;
    for (oud = userList.GetHead(); oud != userList.EndOfList(); oud = oud->GetNext()) {
        bool deleteUser = true;
        for (int i = 0; i < game.iCount; i++) {
            if (bStrCmp(oud->user.name, game.aOpponents[i].strPers) == 0) {
                deleteUser = false;
                break;
            }
        }

        if (deleteUser == true) {
            oud = oud->GetPrev();
            delete userList.Remove(oud->GetNext());
        }
    }
}

LobbyApiUserT *LobbyUsers::GetUserRecord(const char *persona) {
    LobbyApiUserT *user = nullptr;
    lobbyMutex.Lock("LobbyUsers::GetUserRecord");
    if (bStrCmp(persona, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
        lobbyMutex.Unlock("LobbyUsers::GetUserRecord");
        return GetMyUserRecord();
    }

    for (OnlineUsersData *oud = userList.GetHead(); oud != userList.EndOfList();
         oud = oud->GetNext()) {
        if (bStrCmp(oud->user.name, persona) == 0 && oud->commandID == 0) {
            user = &oud->user;
            break;
        }
    }

    lobbyMutex.Unlock("LobbyUsers::GetUserRecord");
    return user;
}

bool LobbyUsers::GetCarNameFromUserRecord(const char *persona, uint32 &carNameHash,
                                          Physics::Info::Performance &performance) {
    lobbyMutex.Lock("LobbyUsers::GetRideInfoFromUserRecord");
    if (bStrCmp(persona, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
        LobbyApiUserT *me = GetMyUserRecord();
        if (me) {
            if (me->aux[0]) {
                carNameHash = TagFieldGetNumber(TagFieldFind(me->aux, "CN"), 0);
                performance.TopSpeed =
                    TagFieldGetFloat(TagFieldFind(me->aux, "PT"), 0.0f);
                performance.Handling =
                    TagFieldGetFloat(TagFieldFind(me->aux, "PH"), 0.0f);
                performance.Acceleration =
                    TagFieldGetFloat(TagFieldFind(me->aux, "PA"), 0.0f);
                lobbyMutex.Unlock("LobbyUsers::GetCarNameFromUserRecord");
                return true;
            }
        }
    } else {
        for (OnlineUsersData *oud = userList.GetHead(); oud != userList.EndOfList();
             oud = oud->GetNext()) {
            if (bStrCmp(oud->user.name, persona) == 0) {
                if (oud->commandID == 0) {
                    if (oud->user.aux[0]) {
                        carNameHash =
                            TagFieldGetNumber(TagFieldFind(oud->user.aux, "CN"), 0);
                        lobbyMutex.Unlock("LobbyUsers::GetCarNameFromUserRecord");
                        return true;
                    }
                }
            }
        }
    }

    lobbyMutex.Unlock("LobbyUsers::GetCarNameFromUserRecord");
    return false;
}

bool LobbyUsers::GetPointsFromUserRecord(const char *persona, uint32 &points,
                                         OnlineRaceModeE mode) {
    lobbyMutex.Lock("LobbyUsers::GetPointsFromUserRecord");
    if (bStrCmp(persona, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
        if (gotMyStats) {
            points = myStats.stats.raceModeStats[mode].points;
            lobbyMutex.Unlock("LobbyUsers::GetPointsFromUserRecord");
            return true;
        }

        LobbyApiUserT *me = GetMyUserRecord();
        if (me) {
            PlayerDataT tmp;
            tmp.ExtractRecord(*me);
            points = tmp.stats.raceModeStats[mode].points;
            lobbyMutex.Unlock("LobbyUsers::GetPointsFromUserRecord");
            return true;
        }
    } else {
        for (OnlineUsersData *oud = userList.GetHead(); oud != userList.EndOfList();
             oud = oud->GetNext()) {
            if (bStrCmp(oud->user.name, persona) == 0) {
                if (oud->commandID == 0) {
                    PlayerDataT tmp;
                    tmp.ExtractRecord(oud->user);
                    points = tmp.stats.raceModeStats[mode].points;
                    lobbyMutex.Unlock("LobbyUsers::GetPointsFromUserRecord");
                    return true;
                }
            }
        }
    }

    lobbyMutex.Unlock("LobbyUsers::GetPointsFromUserRecord");
    return false;
}

bool LobbyUsers::GetDisqualificationsFromUserRecord(const char *persona, uint32 &disqs) {
    lobbyMutex.Lock("LobbyUsers::GetDisqualificationsFromUserRecord");
    if (bStrCmp(persona, FEDatabase->OnlineSettings.GetLobbyPersona()) == 0) {
        if (gotMyStats == true) {
            disqs = myStats.stats.disqualifications;
            lobbyMutex.Unlock("LobbyUsers::GetDisqualificationsFromUserRecord");
            return true;
        }

        LobbyApiUserT *me = GetMyUserRecord();
        if (me) {
            PlayerDataT tmp;
            tmp.ExtractRecord(*me);
            disqs = tmp.stats.disqualifications;
            lobbyMutex.Unlock("LobbyUsers::GetDisqualificationsFromUserRecord");
            return true;
        }
    } else {
        for (OnlineUsersData *oud = userList.GetHead(); oud != userList.EndOfList();
             oud = oud->GetNext()) {
            if (bStrCmp(oud->user.name, persona) == 0) {
                if (oud->commandID == 0) {
                    PlayerDataT tmp;
                    tmp.ExtractRecord(oud->user);
                    disqs = tmp.stats.disqualifications;
                    lobbyMutex.Unlock("LobbyUsers::GetDisqualificationsFromUserRecord");
                    return true;
                }
            }
        }
    }

    lobbyMutex.Unlock("LobbyUsers::GetDisqualificationsFromUserRecord");
    return false;
}

int32 LobbyUsers::GetMyUserStats(PlayerDataT &userStats, CommandCBFunc func, void *context) {
    lobbyMutex.Lock("LobbyUsers::GetMyUserStats");
    if (gotMyStats == true) {
        bMemCpy(&userStats, &myStats, sizeof(PlayerDataT));
        lobbyMutex.Unlock("LobbyUsers::GetMyUserStats");
        return 0;
    }

    char buf[128] = "";
    TagFieldSetString(buf, sizeof(buf), "PERS", FEDatabase->OnlineSettings.GetLobbyPersona());
    int32 rc = LobbyCore::Instance().QueueCommand('user', buf, UserCB, this, func, context,
                                                   false);
    lobbyMutex.Unlock("LobbyUsers::GetMyUserStats");
    return rc;
}

void LobbyUsers::ClearUserStats() {
    lobbyMutex.Lock("LobbyUsers::ClearUserStats");
    myStats.Clear();
    gotMyStats = false;
    lobbyMutex.Unlock("LobbyUsers::ClearUserStats");
}

int32 LobbyUsers::GetOtherUserStats(PlayerDataT &userStats, const char *persona,
                                    CommandCBFunc func, void *context) {
    lobbyMutex.Lock("LobbyUsers::GetOtherUserStats");
    char buf[128] = "";
    TagFieldSetString(buf, sizeof(buf), "PERS", persona);
    int32 rc = LobbyCore::Instance().QueueCommand('user', buf, LobbyCore::DefaultCB, this, func,
                                                   context, false);
    lobbyMutex.Unlock("LobbyUsers::GetOtherUserStats");
    return rc;
}
