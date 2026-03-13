#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

extern unsigned int FEngHashString(const char *, ...);

const char* UserProfile::GetProfileName() {}

bool UserProfile::IsProfileNamed() {
    return m_bNamed;
}

SMSMessage *CareerSettings::GetSMSMessage(unsigned int index) {
    if (index < 0x96) {
        return &SMSMessages[index];
    }
    return nullptr;
}

unsigned short CareerSettings::GetSMSSortOrder() {
    SMSSortOrder = SMSSortOrder + 1;
    return SMSSortOrder;
}

void CareerSettings::SpendCash(int amount) {
    if (CurrentCash < static_cast<unsigned int>(amount)) {
        CurrentCash = 0;
        return;
    }
    CurrentCash = CurrentCash - amount;
}

void CareerSettings::AwardOneTimeCashBonus(bool bOldSaveExists) {
    SpecialFlags = SpecialFlags | 2;
    if (!bOldSaveExists) {
        return;
    }
    CurrentCash = CurrentCash + 10000;
}

void CareerSettings::SetPlayerHasBeatenTheGame() {
    SpecialFlags = SpecialFlags | 0x4000;
}

int CareerSettings::GetSaveBufferSize(bool bExcludeGameplay) {
    if (bExcludeGameplay) {
        return 0x735;
    }
    return 0x59F9;
}

bool GameplaySettings::IsMapItemEnabled(unsigned int type) {
    if ((MapItems & type) != 0) {
        return true;
    }
    return false;
}

void GameplaySettings::SetMapItem(unsigned int type, bool enabled) {
    if (enabled) {
        MapItems = MapItems | type;
        return;
    }
    MapItems = MapItems & ~type;
}

void VideoSettings::Default() {
    FEScale = 1.0f;
    ScreenOffsetX = 0.0f;
    ScreenOffsetY = 0.0f;
    WideScreen = 0;
}

RaceSettings *cFrontendDatabase::GetQuickRaceSettings(GRace::Type type) {
    if (static_cast<int>(type) < 11) {
        return &TheQuickRaceSettings[type];
    }
    return &TheQuickRaceSettings[RaceMode];
}

unsigned int cFrontendDatabase::GetUserProfileSaveSize(bool bExcludeGameplay) {
    return CurrentUserProfiles[0]->GetSaveBufferSize(bExcludeGameplay);
}

void cFrontendDatabase::SaveUserProfileToBuffer(void *buffer, unsigned int size) {
    CurrentUserProfiles[0]->SaveToBuffer(buffer, size);
}

unsigned int cFrontendDatabase::GetChallengeHeaderHash(unsigned int index) {
    return FEngHashString("CHALLENGE_HEADER_%d", index);
}

unsigned int cFrontendDatabase::GetChallengeDescHash(unsigned int index) {
    return FEngHashString("CHALLENGE_DESCRIPTION_%d", index);
}

unsigned int cFrontendDatabase::GetBountyIconHash(unsigned int index) {
    if (index < 5) {
        return 0x8A21B882;
    }
    if (index < 9) {
        return 0x895EC0AE;
    }
    return 0x9129E7FB;
}

unsigned int cFrontendDatabase::GetBountyHeaderHash(unsigned int index) {
    return FEngHashString("BOUNTY_HEADER_%d", index);
}

unsigned int cFrontendDatabase::GetBountyDescHash(unsigned int index) {
    return FEngHashString("BOUNTY_DESCRIPTION_%d", index);
}

void RaceSettings::Default() {
    NumOpponents = 3;
    CatchUp = true;
    TrafficDensity = 1;
    NumLaps = 2;
    IsLapKO = false;
    AISkill = 1;
    CopDensity = 1;
    CopsOn = false;
    TrackDirection = 0;
    for (int i = 0; i < 2; i++) {
        SelectedCar[i] = 0;
    }
    RegionFilterBits = 3;
}

void cFrontendDatabase::SetPlayersJoystickPort(int player, signed char port) {
    if (port == -1 && PlayerJoyports[player] != -1) {
        cFEngJoyInput::mInstance->SetRequiredJoy(static_cast<JoystickPort>(PlayerJoyports[player]), false);
    }
    PlayerJoyports[player] = port;
}

static int MikeMannBuild;

int GetMikeMannBuild() {
    return MikeMannBuild;
}

static bool IsCollectorsEdition;

bool GetIsCollectorsEdition() {
    return IsCollectorsEdition;
}