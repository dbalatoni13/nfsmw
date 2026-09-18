#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/controller_hash.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/music.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Misc/MD5.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/RaceStarter.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

int g_MaxSongs = (int)0xFFFFFFFF;

extern Sound::SongInfoList Songs;

// .rodata:0x803EA794 (0x28 B) y .rodata:0x803EA7BC (0x28 B), las dos
// scope:local -> `static`, y en .rodata -> `const`.  Los diez hashes salen del
// ELF original y coinciden uno a uno con Attrib::Hash::controller (DriveConfigs
// primero, que es el orden de direcciones del objetivo).
static const Attrib::Key DriveConfigs[5][2] = {
    {Attrib::Hash::controller::key_drive, Attrib::Hash::controller::key_drive_analog},
    {Attrib::Hash::controller::key_drive2, Attrib::Hash::controller::key_drive2_analog},
    {Attrib::Hash::controller::key_drive3, Attrib::Hash::controller::key_drive3_analog},
    {Attrib::Hash::controller::key_drive4, Attrib::Hash::controller::key_drive4_analog},
    {Attrib::Hash::controller::key_drive5, Attrib::Hash::controller::key_drive5_analog},
};

static const Attrib::Key HudConfigs[5][2] = {
    {Attrib::Hash::controller::key_hud_analog, Attrib::Hash::controller::key_hud},
    {Attrib::Hash::controller::key_hud_analog, Attrib::Hash::controller::key_hud},
    {Attrib::Hash::controller::key_hud_analog, Attrib::Hash::controller::key_hud},
    {Attrib::Hash::controller::key_hud_analog, Attrib::Hash::controller::key_hud},
    {Attrib::Hash::controller::key_hud_analog, Attrib::Hash::controller::key_hud},
};

cFrontendDatabase *FEDatabase = nullptr;
cFrontendDatabase *gFEDatabase = FEDatabase;

char *SaveSomeData(void *save_to, void *save_from, int bytes, void *maxptr) {
    char *dest = static_cast<char *>(save_to);

    if (reinterpret_cast<uint32>(dest) + bytes <= reinterpret_cast<uint32>(maxptr)) {

        bMemCpy(save_to, save_from, bytes);
        dest = dest + bytes;

        return dest;
    }
    return static_cast<char *>(save_to);
}

char *LoadSomeData(void *load_to, void *load_from, int bytes, void *maxptr) {
    if (reinterpret_cast<uint32>(load_from) + bytes <= reinterpret_cast<uint32>(maxptr)) {
        bMemCpy(load_to, load_from, bytes);
    }
    return static_cast<char *>(load_from) + bytes;
}

FEKeyboardSettings::FEKeyboardSettings() {
    AcceptCallbackHash = 0xAE83B9DB;
    DeclineCallbackHash = 0x6A97B51F;
    DefaultTextHash = 0;
    MaxTextLength = 64;
    Mode = 0;
    Buffer[0] = 0;
}

void PlayerSettings::Default() {
    GaugesOn = 1;
    PositionOn = 1;
    LapInfoOn = 1;
    ScoreOn = 1;
    LeaderboardOn = 1;
    TransmissionPromptOn = 1;




    Rumble = 1;











    DriveWithAnalog = 1;
    Config = CC_CONFIG_1;

    SplitTimeType = 0;
    CurCam = PSC_DEFAULT;
    Transmission = 0;
    Handling = 1;
}

bool PlayerSettings::operator==(const PlayerSettings &rhs) const {
    return bMemCmp(this, &rhs, 0x2C) == 0;
}

void PlayerSettings::DefaultFromOptionsScreen() {
    bool savedDriveWithAnalog = DriveWithAnalog;
    eControllerConfig savedConfig = Config;
    bool savedRumble = Rumble;
    Default();
    DriveWithAnalog = savedDriveWithAnalog;
    Config = savedConfig;
    Rumble = savedRumble;
}

Attrib::Key PlayerSettings::GetControllerAttribs(eControllerAttribs type, bool wheel_connected) const {
    int analog = 0;
    if (DriveWithAnalog) {
        analog = 1;
    }
    int config = Config;

    if (wheel_connected) {

        config = 0;
        analog = 1;
    }

    switch (type) {
        case CA_DRIVING:
            return DriveConfigs[config][analog];

        case CA_HUD:
            return HudConfigs[config][analog];
    }

    return 0;
}

void PlayerSettings::ScrollDriveCam(int dir) {
    int cam = CurCam;
    if (dir == 1) {
        do {
            cam++;
            if (cam > 6) {
                cam = 0;
            }
        } while (!IsPlayerCameraSelectable(GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cam))));
        CurCam = static_cast<ePlayerSettingsCameras>(cam);
    } else if (dir == -1) {
        do {
            cam--;
            if (cam < 0) {
                cam = 6;
            }
        } while (!IsPlayerCameraSelectable(GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cam))));
        CurCam = static_cast<ePlayerSettingsCameras>(cam);
    } else {
        CurCam = static_cast<ePlayerSettingsCameras>(cam);
    }
}

void GameplaySettings::Default() {
    AutoSaveOn = 1;
    RearviewOn = 1;
    Damage = 1;

    RacingMiniMapMode = 1;
    ExploringMiniMapMode = 0;

    MapItems = static_cast<unsigned int>(-1);
    LastMapZoom = 1;
    LastPursuitMapZoom = 2;
    LastMapView = 0;

    JumpCam = 1;
    HighlightCam = 127.5f;
    eLanguages lang = GetCurrentLanguage();
    if (lang) {
        SpeedoUnits = 1;
    } else {
        SpeedoUnits = 0;
    }
}

bool GameplaySettings::IsMapItemEnabled(eWorldMapItemType type) {
    if ((MapItems & type) != 0) {
        return true;
    }
    return false;
}

void GameplaySettings::SetMapItem(eWorldMapItemType type, bool enabled) {
    if (enabled) {
        MapItems = MapItems | type;
        return;
    }
    MapItems = MapItems & ~type;
}

bool GameplaySettings::operator==(const GameplaySettings &rhs) const {
    return bMemCmp(this, &rhs, sizeof(GameplaySettings)) == 0;
}

void VideoSettings::Default() {
#ifdef EA_PLATFORM_GAMECUBE
    FEScale = 0.84f;
#else
    FEScale = 1.0f;
#endif
    ScreenOffsetX = 0.0f;
    ScreenOffsetY = 0.0f;
    WideScreen = false;
}

bool VideoSettings::operator==(const VideoSettings &rhs) const {
    return bMemCmp(this, &rhs, 0x10) == 0;
}

void AudioSettings::Default() {
    MasterVol = 1.0f;
    SpeechVol = 1.0f;
    FEMusicVol = 0.8f;
    IGMusicVol = 0.8f;
    SoundEffectsVol = 1.0f;
    EngineVol = 1.0f;
    CarVol = 1.0f;
    AmbientVol = 1.0f;
    SpeedVol = 1.0f;
    AudioMode = 2;
#ifndef EA_BUILD_A124
    AudioMode = g_pEAXSound->GetDefaultPlatformAudioMode();
#endif
    InteractiveMusicMode = 1;
    EATraxMode = 1;
    PlayState = 0;
}

bool AudioSettings::operator==(const AudioSettings &rhs) const {
    if (MasterVol != rhs.MasterVol)
        return false;
    if (SpeechVol != rhs.SpeechVol)
        return false;
    if (FEMusicVol != rhs.FEMusicVol)
        return false;
    if (IGMusicVol != rhs.IGMusicVol)
        return false;
    if (SoundEffectsVol != rhs.SoundEffectsVol)
        return false;
    if (EngineVol != rhs.EngineVol)
        return false;
    if (CarVol != rhs.CarVol)
        return false;
    if (AmbientVol != rhs.AmbientVol)
        return false;
    if (SpeedVol != rhs.SpeedVol)
        return false;
    if (InteractiveMusicMode != rhs.InteractiveMusicMode)
        return false;
    if (EATraxMode != rhs.EATraxMode)
        return false;
    if (PlayState != rhs.PlayState)
        return false;
    if (AudioMode != rhs.AudioMode && AudioMode != AUDIO_MODE_MONO && AudioMode != AUDIO_MODE_STEREO)
        return false;
    return true;
}

void OptionsSettings::Default() {
    CurrentCategory = static_cast<eOptionsCategory>(0);
    TheVideoSettings.Default();
    TheAudioSettings.Default();
    TheGameplaySettings.Default();
    ThePlayerSettings[0].Default();
    ThePlayerSettings[1].Default();
}

void CareerSettings::Default() {
    CurrentBin = 0x10;
    CurrentCar = 0;
    SpecialFlags = 0;
    AdaptiveDifficulty = 0;
    CurrentCash = 0;
    for (int i = 0; i < 150; i++) {
        SMSMessages[i].SetHandle(static_cast<FESMSHandle>(i));
        if (!DoesStringExist(SMSMessages[i].GetMsgHash())) {
            SMSMessages[i].SetHandle(0xFF);
        }
        SMSMessages[i].ClearFlags();
    }
    SMSSortOrder = 0;
}

SMSMessage *CareerSettings::GetSMSMessage(uint32 index) {
    if (index >= 0x96) {
        return nullptr;
    }
    return &SMSMessages[index];
}

unsigned short CareerSettings::GetSMSSortOrder() {
    SMSSortOrder = SMSSortOrder + 1;
    return SMSSortOrder;
}

bool SMSMessage::IsVoice() {
    switch (Handle) {
        case 0x6E:
        case 0x6F:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x7D:
        case 0x7E:
        case 0x7F:
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
            return false;
        default:
            return true;
    }
}

void CareerSettings::SpendCash(int amount) {
    if (static_cast<unsigned int>(amount) > CurrentCash) {
        CurrentCash = 0;
        return;
    }
    CurrentCash = CurrentCash - amount;
}

void CareerSettings::StartNewCareer(bool bEnterGameplay) {
    Default();
    CurrentCar = FEDatabase->GetDefaultCar();
    GenerateCaseFileName();
    SpecialFlags |= 1;

    if (SkipCareerIntro && SkipDDayRaces) {
        CurrentBin = 0xF;
        GRaceDatabase::Get().SimulateDDayComplete();
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *rec = stable->CreateNewCareerCar(0x2CF385B2);
        CurrentCar = rec->Handle;
        rec = stable->CreateNewCareerCar(0x03A94520);
        CurrentCar = rec->Handle;
    }

    TryAwardDemoMarker();

    if (!bEnterGameplay) {
        return;
    }

    FEDatabase->ResetGameMode();
    FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);

    if (SkipDDayRaces) {
        FEManager::Get()->SetGarageType(GARAGETYPE_CAREER_SAFEHOUSE);
        FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
        if (SkipCareerIntro) {
            CurrentBin = 0xF;
        }
    } else {
        unsigned int hash = FEHashUpper("M3GTRCAREERSTART");
        FEDatabase->GetCareerSettings()->SetCurrentCar(hash);
        FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SetSelectedCar(hash, 0);
        gMemcardSetup.mMemScreen = nullptr;
        gMemcardSetup.mToScreen = nullptr;
        gMemcardSetup.mFromScreen = nullptr;
        gMemcardSetup.mTermFunc = nullptr;
        gMemcardSetup.mTermFuncParam = nullptr;
        gMemcardSetup.mLastMessage = 0;
        gMemcardSetup.mSuccessMsg = 0;
        gMemcardSetup.mFailedMsg = 0;
        gMemcardSetup.mInBootFlow = false;
        gMemcardSetup.mPreviousCommand = 0;
        gMemcardSetup.mOp = 0;
        gMemcardSetup.mPreviousPrompt = 0;
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName("16.1.0");
        GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
        GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(race);
        RaceStarter::StartCareerFreeRoam();
    }
}

void CareerSettings::TryAwardDemoMarker() {
    if (!HasBeenAwardedDemoMarker() && gEasterEggs.IsEasterEggUnlocked(static_cast<EasterEggsSpecial>(5))) {
        TheFEMarkerManager.AddMarkerToInventory(FEMarkerManager::ePossibleMarker(2), 0);
        SetAwardedDemoMarker();
    }
}

void CareerSettings::ResumeCareer() {
    const char *lastDDayRace = GRaceDatabase::Get().GetDDayEndRace();
    bool dday_flow_completed = false;
    if (SkipDDayRaces || GRaceDatabase::Get().IsCareerRaceComplete(GRaceDatabase::Get().GetRaceFromName(lastDDayRace)->GetEventHash())) {
        dday_flow_completed = true;
    }

    const char *lastBossRace = GRaceDatabase::Get().GetFinalBossRace();
    bool finalBossRaceCompleted = false;
    if (!HasBeatenCareer()) {
        if (GRaceDatabase::Get().IsCareerRaceComplete(GRaceDatabase::Get().GetRaceFromName(lastBossRace)->GetEventHash())) {
            finalBossRaceCompleted = true;
        }
    }

    if (CurrentBin == 0x10) {
        if (!dday_flow_completed) {
            unsigned int hash = FEHashUpper("M3GTRCAREERSTART");
            FEDatabase->GetCareerSettings()->CurrentCar = hash;
            FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SelectedCar[0] = hash;
            const char *nextDDayRace = GRaceDatabase::Get().GetNextDDayRace();
            GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(nextDDayRace);
            GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
            GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
            GRaceDatabase::Get().FreeCustomRace(race);
            if (bStrCmp(nextDDayRace, "16.1.0") != 0) {
                MemoryCard::GetInstance()->CancelNextAutoSave();
            }
        }
        RaceStarter::StartCareerFreeRoam();
    } else if (finalBossRaceCompleted) {
        const char *finalEpicChaseRace = "1.8.1";
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromName(finalEpicChaseRace);
        GRaceCustom *race = GRaceDatabase::Get().AllocCustomRace(parms);
        GRaceDatabase::Get().SetStartupRace(race, GRace::kRaceContext_Career);
        GRaceDatabase::Get().FreeCustomRace(race);
        RaceStarter::StartCareerFreeRoam();
        MemoryCard::GetInstance()->CancelNextAutoSave();
    } else {
        FEManager::Get()->SetGarageType(static_cast<eGarageType>(2));
        FEDatabase->ClearGameMode(eFE_GAME_MODE_CAREER_MANAGER);
    }
    FEDatabase->SetGameMode(eFE_GAME_MODE_CAREER);
}

void CareerSettings::AwardOneTimeCashBonus(bool bOldSaveExists) {
    SpecialFlags = SpecialFlags | CS_ONE_TIME_CASH_BONUS;
    if (!bOldSaveExists) {
        return;
    }
    CurrentCash = CurrentCash + 10000;
}

void CareerSettings::SetPlayerHasBeatenTheGame() {
    SetHasBeatenCareer();
}

void CareerSettings::GenerateCaseFileName() {
    const int SCOTTS_RAND_CASE_FILE_NUMBER_RANGE = 0x19B3;
    const int SCOTTS_RAND_CASE_FILE_NUMBER_START = 0x42D;
    unsigned int num = bRandom(SCOTTS_RAND_CASE_FILE_NUMBER_RANGE) + SCOTTS_RAND_CASE_FILE_NUMBER_START;
    const char *profile_name = FEDatabase->GetUserProfile(0)->GetProfileName();
    bSNPrintf(CaseFileName, 13, "MW-%d-%.4sXXX", num, profile_name);
    bToUpper(CaseFileName);
}

char *CareerSettings::SaveToBuffer(void *buffer, void *maxbuf) {
    char *buf = SaveGameplayData(buffer, maxbuf);
    buf = SaveSomeData(buf, &CurrentCar, 4, maxbuf);
    buf = SaveSomeData(buf, &CurrentBin, 1, maxbuf);
    buf = SaveSomeData(buf, &CurrentCash, 4, maxbuf);
    buf = SaveSomeData(buf, &AdaptiveDifficulty, 2, maxbuf);
    buf = SaveSomeData(buf, &SpecialFlags, 4, maxbuf);
    buf = SaveSomeData(buf, SMSMessages, 600, maxbuf);
    buf = SaveSomeData(buf, &SMSSortOrder, 2, maxbuf);
    buf = SaveSomeData(buf, CaseFileName, 16, maxbuf);
    buf = SaveRaceData(buf, maxbuf);
    buf = SaveUnlockData(buf, maxbuf);
    return TheFEMarkerManager.SaveToBuffer(buf);
}

char *CareerSettings::LoadFromBuffer(void *buffer, void *maxbuf) {
    char *buf = LoadGameplayData(buffer, maxbuf);
    buf = LoadSomeData(&CurrentCar, buf, 4, maxbuf);
    buf = LoadSomeData(&CurrentBin, buf, 1, maxbuf);
    buf = LoadSomeData(&CurrentCash, buf, 4, maxbuf);
    buf = LoadSomeData(&AdaptiveDifficulty, buf, 2, maxbuf);
    buf = LoadSomeData(&SpecialFlags, buf, 4, maxbuf);
    buf = LoadSomeData(SMSMessages, buf, 600, maxbuf);
    buf = LoadSomeData(&SMSSortOrder, buf, 2, maxbuf);
    buf = LoadSomeData(CaseFileName, buf, 16, maxbuf);
    buf = LoadRaceData(buf, maxbuf);
    buf = LoadUnlockData(buf, maxbuf);
    return TheFEMarkerManager.LoadFromBuffer(buf);
}

int32 CareerSettings::GetSaveBufferSize(bool bExcludeGameplay) {
    int size = TheFEMarkerManager.GetSaveBufferSize() + 0x441;
    if (!bExcludeGameplay) {
        size += 0x52C4;
    }
    return size;
}

char *CareerSettings::SaveRaceData(void *save_to, void *maxptr) {
    if (!GRaceDatabase::Exists()) {
        return static_cast<char *>(save_to);
    }

    unsigned int nEntries = GRaceDatabase::Get().GetScoreInfoCount();
    nEntries = bMin(300, nEntries);

    char *buf = SaveSomeData(save_to, &nEntries, 4, maxptr);

    GRaceSaveInfo *current = GRaceDatabase::Get().GetScoreInfo();
    for (unsigned int index = 0; index < nEntries; index++) {

        if (gVerboseTesterOutput && current->mRaceHash != 0 && (current->mFlags & 2)) {

            GRaceDatabase::Get().GetRaceFromHash(current->mRaceHash);
        }




        buf = SaveSomeData(buf, current, 0x10, maxptr);
        current++;
    }

    return static_cast<char *>(save_to) + 0x12C4;
}

char *CareerSettings::SaveUnlockData(void *save_to, void *maxptr) {
    char *buf = static_cast<char *>(save_to);
    for (unsigned int i = 0; i < 0x39; i++) {
        buf = SaveSomeData(buf, &TheUnlockData[i], 8, maxptr);
    }
    return buf;
}

char *CareerSettings::SaveGameplayData(void *save_to, void *maxptr) {
    char *buf = static_cast<char *>(save_to);
    if (GManager::Exists()) {
        GManager::Get().SaveGameplayData(reinterpret_cast<uint8 *>(buf), 0x4000);
    } else {
        bMemSet(buf, 0, 0x4000);
    }
    return buf + 0x4000;
}

char *CareerSettings::LoadRaceData(void *load_from_here, void *maxptr) {
    if (!GRaceDatabase::Exists()) {
        return static_cast<char *>(load_from_here);
    }

    unsigned int nEntries = 0;
    char *buf = LoadSomeData(&nEntries, static_cast<char *>(load_from_here), 4, maxptr);
    nEntries = bMin(300, static_cast<int>(nEntries));
    GRaceSaveInfo saveInfoEntries[300];
    GRaceSaveInfo *current = saveInfoEntries;
    for (unsigned int index = 0; index < nEntries; index++) {
        buf = LoadSomeData(current, buf, 0x10, maxptr);
        if (gVerboseTesterOutput && current->mRaceHash != 0 && (current->mFlags & 2)) {
            GRaceDatabase::Get().GetRaceFromHash(current->mRaceHash);
        }
        current++;
    }
    GRaceDatabase::Get().LoadBestScores(saveInfoEntries, nEntries);
    return static_cast<char *>(load_from_here) + 0x12C4;
}

char *CareerSettings::LoadUnlockData(void *load_from, void *maxptr) {
    char *buf = static_cast<char *>(load_from);
    for (unsigned int i = 0; i < 0x39; i++) {
        buf = LoadSomeData(&TheUnlockData[i], buf, 8, maxptr);
    }
    return buf;
}

char *CareerSettings::LoadGameplayData(void *load_from_here, void *maxptr) {
    char *buf = static_cast<char *>(load_from_here);
    if (GManager::Exists()) {
        GManager::Get().LoadGameplayData(reinterpret_cast<unsigned char *>(buf), 0x4000);
    }
    return buf + 0x4000;
}

UserProfile::UserProfile() : TheOptionsSettings(), TheCareerSettings(), HighScores() {}

UserProfile::~UserProfile() {}

void UserProfile::SetProfileName(const char *pName, bool isP1) {
    bool named = false;
    if (pName && bStrLen(pName) > 0) {
        named = true;
    }
    bMemSet(m_aProfileName, 0, 0x20);
    if (named) {
        bStrNCpy(m_aProfileName, pName, 0x20);
        m_bNamed = true;
    } else {
        char defaultName[32];
        if (isP1 == true) {
            GetLocalizedString(defaultName, 0x20, LANGUAGE_COMMON_PLAYER_1);
        } else {
            GetLocalizedString(defaultName, 0x20, LANGUAGE_COMMON_PLAYER_2);
        }
        bStrNCpy(m_aProfileName, defaultName, 0x20);
        m_bNamed = false;
    }
}

const char *UserProfile::GetProfileName() {}

bool UserProfile::IsProfileNamed() {
    return m_bNamed;
}

void UserProfile::Default(int player_number, bool commit_default) {
    static bool song_info_loaded = false;

    if (!player_number) {
        SetProfileName(nullptr, true);
    } else {
        SetProfileName(nullptr, false);
    }

    PlayersCarStable.Default();

    if (!player_number) {
        TheOptionsSettings.Default();
        TheCareerSettings.Default();
        HighScores.Default();
        CareerModeHasBeenCompletedAtLeastOnce = false;

        if (!song_info_loaded) {
            song_info_loaded = true;

            Attrib::Gen::audiosystem *playlist_atrs = new Attrib::Gen::audiosystem(0x7E4B0ED2, 0, nullptr);
            if (playlist_atrs->IsValid()) {
                Attrib::Gen::audiosystem licensed_music(playlist_atrs->LicensedMusic(), 0, nullptr);
                g_MaxSongs = licensed_music.Num_PFMapping();

                for (int i = 0; i < static_cast<int>(Songs.size()); i++) {
                    delete Songs[i];
                }
                Songs.clear();

                if (static_cast<int>(Songs.capacity()) < g_MaxSongs) {
                    Songs.reserve(g_MaxSongs);
                }

                for (int i = 0; i < g_MaxSongs; i++) {
                    Sound::stSongInfo *newsong = new ("Sound::stSongInfo", 0) Sound::stSongInfo;
                    Attrib::Gen::music currsong(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr);
                    const char *tmpSongName;

                    currsong.ChangeWithDefault(licensed_music.PFMapping(i));

                    tmpSongName = currsong.SongName().GetString();
                    newsong->SongName = const_cast<char *>(tmpSongName);
                    tmpSongName = currsong.Album().GetString();
                    newsong->Album = const_cast<char *>(tmpSongName);
                    tmpSongName = currsong.Artist().GetString();
                    newsong->Artist = const_cast<char *>(tmpSongName);
                    tmpSongName = currsong.DefPlay().GetString();
                    newsong->DefPlay = const_cast<char *>(tmpSongName);
                    newsong->PathEvent = currsong.PathEvent();
                    Songs.push_back(newsong);
                }
            }
        }

        for (int i = 0; i < g_MaxSongs; i++) {
            Playlist[i].SongIndex = i;

            Sound::stSongInfo *song = Songs[i];
            unsigned char playability;
            if (song == nullptr) {
                playability = 0;
            } else {
                if (bStrCmp(song->DefPlay, "FE") == 0) {
                    playability = 1;
                } else if (bStrCmp(song->DefPlay, "IG") == 0) {
                    playability = 2;
                } else if (bStrCmp(song->DefPlay, "AL") == 0) {
                    playability = 3;
                } else {
                    playability = 0;
                }
            }
            Playlist[i].PlayabilityField = playability;
        }

        InitializeEATrax(true);
    }

    PlayersCarStable.AwardBonusCars();
    TheCareerSettings.TryAwardDemoMarker();
}

void UserProfile::CommitHighScoresPauseQuit() {
    HighScores.CommitHighScoresPauseQuit();
}

void UserProfile::CommitPursuitInfo(IPursuit *iPursuit, uint32 car_handle, uint32 bounty, unsigned int num_infractions) {
    HighScores.CommitPursuitInfo(iPursuit, car_handle, bounty, num_infractions);
}

void UserProfile::IncInfration(GInfractionManager::InfractionType infrat, unsigned int car) {}
void UserProfile::CommitServeInfractions(unsigned int car) {}

void UserProfile::WriteProfileHash(void *bufferToHash, void *bufferToWrite, int bytes, void *maxptr) {
    MD5 md5;
    md5.Update(bufferToHash, bytes);
    md5.GetRaw();
    SaveSomeData(bufferToWrite, md5.GetRaw(), 0x10, maxptr);
}

bool UserProfile::VerifyProfileHash(void *bufferToHash, void *bufferHash, int bytes) {
    MD5 md5;
    md5.Update(bufferToHash, bytes);
    md5.GetRaw();
    return bMemCmp(md5.GetRaw(), bufferHash, 0x10) == 0;
}

void UserProfile::SaveToBuffer(void *buffer, int size) {
    char *buf = static_cast<char *>(buffer);
    char *maxbuf = buf + size;
    bMemSet(buf, 0, size);
    char aVersion[16];
    bMemSet(aVersion, 0, 0x10);
    buf = SaveSomeData(buf, aVersion, 0x10, maxbuf);
    buf = TheCareerSettings.SaveToBuffer(buf, maxbuf);
    buf = SaveSomeData(buf, &FEDatabase->iDefaultStableHash, 4, maxbuf);
    buf = SaveSomeData(buf, m_aProfileName, 0x20, maxbuf);
    buf = SaveSomeData(buf, Playlist, 0xF0, maxbuf);
    buf = SaveSomeData(buf, &TheOptionsSettings, 0xC0, maxbuf);
    int stableSize = PlayersCarStable.GetSaveBufferSize();
    buf = PlayersCarStable.SaveToBuffer(buf, stableSize);
    buf = SaveSomeData(buf, &CareerModeHasBeenCompletedAtLeastOnce, 4, maxbuf);
    buf = SaveSomeData(buf, &HighScores, 0xBD8, maxbuf);
    for (int i = 0; i < 11; i++) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i));
        unsigned int h = settings->GetSelectedCar(0);
        buf = SaveSomeData(buf, &h, 4, maxbuf);
    }
    WriteProfileHash(static_cast<char *>(buffer) + 0x10, buf, size - 0x20, maxbuf);
}

bool UserProfile::LoadFromBuffer(void *buffer, int size, bool commit_changes, int player_id) {
    char *buf = static_cast<char *>(buffer);
    char *maxbuf = buf + size;
    char aVersion[16];
    buf = LoadSomeData(aVersion, buf, 0x10, maxbuf);
    if (!player_id) {
        buf = TheCareerSettings.LoadFromBuffer(buf, maxbuf);
        TheCareerSettings.TryAwardDemoMarker();
    } else {
        int careerSize = TheCareerSettings.GetSaveBufferSize(false);
        buf = buf + careerSize;
    }
    unsigned int version;
    buf = LoadSomeData(&version, buf, 4, maxbuf);
    if (version != FEDatabase->iDefaultStableHash) {
        return false;
    }
    buf = LoadSomeData(m_aProfileName, buf, 0x20, maxbuf);
    if (!player_id) {
        buf = LoadSomeData(Playlist, buf, 0xF0, maxbuf);
    } else {
        buf = buf + 0xF0;
    }
    buf = LoadSomeData(&TheOptionsSettings, buf, 0xC0, maxbuf);
    int stableSize = PlayersCarStable.GetSaveBufferSize();
    buf = PlayersCarStable.LoadFromBuffer(buf, stableSize);
    PlayersCarStable.AwardBonusCars();
    buf = LoadSomeData(&CareerModeHasBeenCompletedAtLeastOnce, buf, 4, maxbuf);
    buf = LoadSomeData(&HighScores, buf, 0xBD8, maxbuf);
    for (int i = 0; i < 11; i++) {
        unsigned int h;
        buf = LoadSomeData(&h, buf, 4, maxbuf);
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i));
        settings->SelectedCar[player_id] = h;
    }
    if (!VerifyProfileHash(static_cast<char *>(buffer) + 0x10, buf, size - 0x20)) {
        return false;
    }
    m_bNamed = true;
    return true;
}

int32 UserProfile::GetSaveBufferSize(bool bExcludeGameplay) {
    int size = TheCareerSettings.GetSaveBufferSize(bExcludeGameplay) + 0x1e4;
    return size + PlayersCarStable.GetSaveBufferSize() + 0xc18;
}

cFrontendDatabase::cFrontendDatabase()
    : iDefaultStableHash(0), m_pCarStableBackup(nullptr), m_pDBBackup(nullptr), SplitScreenCustomization(nullptr) {
    for (int i = 0; i < 2; i++) {
        CurrentUserProfiles[i] = nullptr;
    }
    CurrentUserProfiles[0] = new ("Player 1 UserProfile", 0) UserProfile();
}

void cFrontendDatabase::Default() {
    iNumPlayers = 1;
    bComingFromBoot = true;
    bProfileLoaded = false;
    bIsOptionsDirty = false;
#ifndef EA_BUILD_A124
    bAutoSaveOverwriteConfirmed = false;
#endif
    GetUserProfile(0)->Default(0, true);
    iCurPauseSubOptionType = 0;
    iCurPauseOptionType = 0;
    FEGameMode = 0;
    if (SkipFE && SkipFESplitScreen) {
        FEGameMode = 4;
        iNumPlayers = 2;
    }
    PlayerJoyports[0] = 0;
    PlayerJoyports[1] = -1;
    RaceMode = GRace::kRaceType_Circuit;
    unsigned int default_car = GetDefaultCar();
    DefaultRaceSettings();
    GetUserProfile(0)->GetCareer()->SetCurrentCar(default_car);
    if (!iDefaultStableHash) {
        FEPlayerCarDB *stable = &GetUserProfile(0)->PlayersCarStable;
        int buf_size = stable->GetSaveBufferSize();
        char *buf = static_cast<char *>(bMalloc(buf_size, "Memcard Car Stable Hashing", 0, 0x40));
        int save_size = stable->GetSaveBufferSize();
        stable->SaveToBuffer(buf, save_size);
        int crc_size = stable->GetSaveBufferSize();
        iDefaultStableHash = bCalculateCrc32(buf, crc_size, 0xFFFFFFFF);
        bFree(buf);
    }
}

void cFrontendDatabase::DefaultProfile() {
    CurrentUserProfiles[0]->Default(0, true);
#ifndef EA_BUILD_A124
    bAutoSaveOverwriteConfirmed = false;
#endif
    DefaultRaceSettings();
    unsigned int default_car = GetDefaultCar();
    GetCareerSettings()->SetCurrentCar(default_car);
    bIsOptionsDirty = false;
    GetPlayerCarStable(0)->Default();
    MemoryCard::GetInstance()->SetCardRemovedWithAutoSaveEnabled(false);
    DefaultUnlockData();
    TheFEMarkerManager.Default();
    if (GRaceDatabase::Exists()) {
        GRaceDatabase::Get().ClearRaceScores();
    }
    if (GManager::Exists()) {
        GManager::Get().ResetAllGameplayData();
    }
}

void cFrontendDatabase::DefaultRaceSettings() {
    unsigned int default_car = GetDefaultCar();
    for (unsigned int i = 0; i < 11; i++) {
        RaceSettings &settings = TheQuickRaceSettings[i];
        settings.Default();
        settings.SetSelectedCar(default_car, 0);
        settings.SetSelectedCar(default_car, 1);
    }
    TheQuickRaceSettings[0].NumLaps = 1;
    TheQuickRaceSettings[2].NumLaps = 1;
    TheQuickRaceSettings[5].NumLaps = 1;
    TheQuickRaceSettings[4].NumOpponents = 0;
    TheQuickRaceSettings[3].NumLaps = TheQuickRaceSettings[3].NumOpponents;
    TheQuickRaceSettings[4].NumLaps = 1;
}
void cFrontendDatabase::NotifyDeleteCar(uint32 handle) {
    uint32 default_car = GetDefaultCar();
    for (unsigned int i = 0; i < 11; i++) {
        RaceSettings &settings = TheQuickRaceSettings[i];
        if (settings.SelectedCar[0] == handle) {
            settings.SelectedCar[0] = default_car;
        }
        if (settings.SelectedCar[1] == handle) {
            settings.SelectedCar[0] = default_car;
        }
    }
}

void cFrontendDatabase::SetPlayersJoystickPort(int player, int8 joy_port) {
    if (joy_port == -1 && PlayerJoyports[player] != -1) {
        cFEngJoyInput::mInstance->SetRequiredJoy(static_cast<JoystickPort>(PlayerJoyports[player]), false);
    }
    PlayerJoyports[player] = joy_port;
}

unsigned int cFrontendDatabase::GetDefaultCar() {
    Attrib::Gen::frontend frontend(0xeec2271a, 0, nullptr);
    Attrib::RefSpec spec;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    unsigned int handle = 0;

    spec = frontend.default_car();

    Attrib::Gen::pvehicle vehicle(spec, 0, nullptr);
    Attrib::Key collection = vehicle.GetCollection();

    for (int i = 0; i < 200; i++) {
        FECarRecord *car = stable->GetCarByIndex(i);

        if (car->IsValid() && car->VehicleKey == collection) {
            handle = car->Handle;
            break;
        }
    }
    return handle;
}

void cFrontendDatabase::CreateMultiplayerProfile(int player) {
    if (!CurrentUserProfiles[player]) {
        CurrentUserProfiles[player] = new ("FEDatabase UserProfile", 0) UserProfile;
        CurrentUserProfiles[player]->Default(player, true);
    }
}

void cFrontendDatabase::DeleteMultiplayerProfile(int player) {
    if (player == 1 && CurrentUserProfiles[1]) {
        RaceSettings *settings = GetQuickRaceSettings(GRace::kRaceType_NumTypes);
        FEPlayerCarDB *stable = GetPlayerCarStable(1);
        FECarRecord *record = stable->GetCarRecordByHandle(settings->GetSelectedCar(1));
        FECustomizationRecord *customization = stable->GetCustomizationRecordByHandle(record->Customization);
        bStrCpy(SplitScreenCarType, record->GetDebugName());
        if (customization) {
            SplitScreenCustomization = static_cast<FECustomizationRecord *>(bMalloc(sizeof(FECustomizationRecord), "Splitscreen FECustomizationRecord", 0, 0x47));
            bMemCpy(SplitScreenCustomization, customization, sizeof(FECustomizationRecord));
        } else {
            SplitScreenCustomization = nullptr;
        }
        delete CurrentUserProfiles[player];
        CurrentUserProfiles[player] = nullptr;
    }
}
void cFrontendDatabase::AllocBackupDB(bool bForce) {
    if (!m_pDBBackup && bForce) {
        m_pDBBackup = static_cast<char *>(bMalloc(GetUserProfileSaveSize(false), "BackupDB Buffer", 0, BMEMORY_ALLOCATE_FROM_TOP));
        SaveUserProfileToBuffer(m_pDBBackup, GetUserProfileSaveSize(false));
    }
}

void cFrontendDatabase::DeallocBackupDB() {
    if (m_pDBBackup) {
        bFree(m_pDBBackup);
        m_pDBBackup = nullptr;
    }
}

void cFrontendDatabase::RestoreFromBackupDB() {
    if (m_pDBBackup) {
        LoadUserProfileFromBuffer(m_pDBBackup, GetUserProfileSaveSize(false), 0);
        DeallocBackupDB();
    }
}
void cFrontendDatabase::BackupCarStable() {
    if (!m_pCarStableBackup) {
        m_pCarStableBackup = static_cast<char *>(bMalloc(GetPlayerCarStable(0)->GetSaveBufferSize(), "CarStable backup buffer", 0, 0));
        bMemCpy(m_pCarStableBackup, GetPlayerCarStable(0), GetPlayerCarStable(0)->GetSaveBufferSize());
    }
}

bool cFrontendDatabase::IsCarStableDirty() {
    if (!m_pCarStableBackup) {
        return false;
    }
    bool result = bMemCmp(m_pCarStableBackup, GetPlayerCarStable(0), GetPlayerCarStable(0)->GetSaveBufferSize()) != 0;
    bFree(m_pCarStableBackup);
    m_pCarStableBackup = nullptr;
    return result;
}

void cFrontendDatabase::RefreshCurrentRide() {
    RideInfo ride;
    FEPlayerCarDB *stable = GetPlayerCarStable(0);
    if (IsCareerMode() || IsSafehouseMode() || IsCareerManagerMode()) {
        BuildCurrentRideForPlayer(0, &ride);
    } else {
        RaceSettings *settings = GetQuickRaceSettings(GRace::kRaceType_NumTypes);
        unsigned int handle = settings->GetSelectedCar(0);
        stable->BuildRideForPlayer(handle, 0, &ride);
    }
    CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(2), static_cast<eCarViewerWhichCar>(0));
}

RaceSettings *cFrontendDatabase::GetQuickRaceSettings(GRace::Type type) {
    if (static_cast<int>(type) > 10) {
        return &TheQuickRaceSettings[RaceMode];
    }
    return &TheQuickRaceSettings[type];
}

bool cFrontendDatabase::IsFinalEpicChase() {
    if (GRaceStatus::Exists()) {
        bool ret = false;
        GRaceParameters *params = GRaceStatus::Get().GetRaceParameters();

        if (params != nullptr) {
            ret = params->GetEventHash() == Attrib::StringHash32("1.8.1");
        }
        return ret;
    }
    return false;
}

void cFrontendDatabase::GetRandomRaceOptions(RaceSettings *race, GRace::Type type) {
    race->CatchUp = true;
    race->CopDensity = static_cast<uint8>(bRandom(4));
    race->AISkill = 1;
    int num_opponents = bRandom(3) + 1;
    race->NumOpponents = static_cast<uint8>(num_opponents);
    switch (type) {
    case GRace::kRaceType_Knockout:
        race->NumLaps = static_cast<uint8>(num_opponents);
        break;
    case GRace::kRaceType_Circuit:
        race->NumLaps = static_cast<uint8>(bRandom(5) + 1);
        break;
    default:
        race->NumLaps = 1;
        break;
    }
    race->TrafficDensity = static_cast<uint8>(bRandom(4));
    race->TrackDirection = static_cast<uint8>(bRandom(1));
}

void cFrontendDatabase::FillCustomRace(GRaceCustom *parms, RaceSettings *race) {
    if (!race) {
        return;
    }
    if (!parms) {
        return;
    }
    parms->SetCatchUp(race->CatchUp);
    parms->SetCopsEnabled(race->CopsOn);
#ifdef EA_BUILD_A124
    parms->SetCopDensity(static_cast<GRace::CopDensity>(race->CopDensity));
#else
    if (race->CopsOn) {
        parms->SetHeatLevel(race->CopDensity);
    }
#endif
    parms->SetDifficulty(static_cast<GRace::Difficulty>(race->AISkill));
    parms->SetNumLaps(race->NumLaps);
    parms->SetNumOpponents(race->NumOpponents);
    switch (race->TrafficDensity) {
        case 0:
        default:
            parms->SetTrafficDensity(0);
            break;
        case 1:
            parms->SetTrafficDensity(10);
            break;
        case 2:
            parms->SetTrafficDensity(30);
            break;
        case 3:
            parms->SetTrafficDensity(50);
            break;
    }
    parms->SetReversed(race->TrackDirection == 1);
}

void cFrontendDatabase::BuildCurrentRideForPlayer(int player, RideInfo *ride) {
    FEPlayerCarDB *stable = (player >= 0 && player <= 1) ? GetPlayerCarStable(player) : nullptr;
    unsigned int car;

    // El original mezcla accesos por this y por el global FEDatabase.
    if ((FEDatabase->FEGameMode & eFE_GAME_MODE_QUICK_RACE) || FEDatabase->IsLanMode() || FEDatabase->IsOnlineMode()) {
        car = GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(player);
    } else {
        car = FEDatabase->GetCareerSettings()->GetCurrentCar();
    }

    stable->BuildRideForPlayer(car, player, ride);
}

void cFrontendDatabase::NotifyExitRaceToFrontend(eExitRacePlaces from_where) {
    {
        int is_split = IsSplitScreenMode();
    }
    PostRaceOptionChosen = static_cast<ePostRaceOptions>(1);
    if (from_where == EXIT_RACE_FROM_PAUSE) {
        CurrentUserProfiles[0]->CommitHighScoresPauseQuit();
    }
}

int32 cFrontendDatabase::GetUserProfileSaveSize(bool bExcludeGameplay) {
    return CurrentUserProfiles[0]->GetSaveBufferSize(bExcludeGameplay);
}

void cFrontendDatabase::SaveUserProfileToBuffer(void *buffer, int32 bufsize) {
    CurrentUserProfiles[0]->SaveToBuffer(buffer, bufsize);
}

bool cFrontendDatabase::LoadUserProfileFromBuffer(void *buffer, int bufsize, int player) {
    if (player == 0) {
        return CurrentUserProfiles[0]->LoadFromBuffer(buffer, bufsize, true, 0);
    } else {
        bool result = CurrentUserProfiles[player]->LoadFromBuffer(buffer, bufsize, false, player);
        bMemCpy(&CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[1], &CurrentUserProfiles[1]->GetOptions()->ThePlayerSettings[0],
                sizeof(PlayerSettings));
        return result;
    }
}

unsigned int cFrontendDatabase::GetChallengeHeaderHash(unsigned int series) {
    return FEngHashString("CHALLENGE_SERIES_DESC_%02d_HEADER", series);
}

unsigned int cFrontendDatabase::GetChallengeDescHash(unsigned int series) {
    return FEngHashString("CHALLENGE_SERIES_DESC_%02d", series);
}

uint32 cFrontendDatabase::GetBountyIconHash(uint32 hal_id) {
    if (hal_id < 5) {
        return 0x8A21B882;
    }
    if (hal_id < 9) {
        return 0x895EC0AE;
    }
    return 0x9129E7FB;
}

unsigned int cFrontendDatabase::GetBountyHeaderHash(unsigned int spawn_point) {
    return FEngHashString("BLACKLIST_BOUNTY_SPAWN_POINT_%02d", spawn_point);
}

unsigned int cFrontendDatabase::GetBountyDescHash(unsigned int spawn_point) {
    return FEngHashString("BLACKLIST_BOUNTY_SPAWN_POINT_%02d_DESC", spawn_point);
}

uint32 cFrontendDatabase::GetMilestoneHeaderHash(uint32 hal_id) {
    return FEngHashString("BLACKLIST_PURSUIT_MILESTONES_%02d_SHORT", hal_id);
}

uint32 cFrontendDatabase::GetMilestoneDescHash(uint32 hal_id) {
    return FEngHashString("BLACKLIST_PURSUIT_MILESTONES_%02d", hal_id);
}

unsigned int cFrontendDatabase::GetMilestoneIconHash(unsigned int type, bool bMilestone) {
    unsigned int hash = 0;

    switch (type) {
    case 0x850A64BC:
        hash = 0x88E8DE9E; // COPS_DAMAGED_ICON
        break;
    case 0x3FD1884D:
    case 0x4FC942CA:
        hash = 0x0FE608E6; // COPS_TAKENOUT_ICON
        break;
    case 0xFD989A3A:
        hash = 0x87807869; // MILESTONE_COST_TO_STATE
        break;
    case 0x1334DAE6:
    case 0x15E88693:
    case 0x1BF724E1:
    case 0x20F1AEF3:
    case 0x23B1BF0E:
    case 0x2CB7CAF4:
    case 0x411B084E:
    case 0x4D9777F1:
    case 0x7457EED4:
    case 0x755F7845:
    case 0x8ED622AD:
    case 0xABDF316E:
    case 0xC8993341:
    case 0xE9A4423C:
        hash = 0;
        break;
    case 0x5392E4FD:
        hash = 0x831B7EBE; // MILESTONE_PURSUIT_MAXTIME
        break;
    case 0x033FA23A:
        hash = 0x8C76CD0F; // MILESTONE_PURSUIT_DURATION
        if (bMilestone) {
            hash = 0x950FCEBC; // MILESTONE_PURSUIT_MINTIME
        }
        break;
    case 0xEB45F99D:
        hash = 0xC43959D2; // MILESTONE_ROADBLOCKS
        break;
    case 0x9E3EBB78:
        hash = 0x3FFE9EC9;
        break;
    case 0xCDF36FC2:
        hash = 0xE621B2EF; // MILESTONE_SPIKESTRIPS
        break;
    case 0xA61CAC24:
        hash = 0x6784A80E; // MILESTONE_INFRACTIONS
        break;
    case 0x2377E50D:
        hash = 0xB4E6456B; // MILESTONE_PURSUITBOUNTY
        break;
    // Estos cuatro tienen cuerpo vacio en el original: saltan a la etiqueta
    // final y devuelven el hash = 0 del inicializador. De ahi el beqlr extra.
    case 0x254230F5:
    case 0x9201E1F4:
    case 0x9F8E56CE:
    case 0xCA9AFDF0:
        break;
    default:
        hash = 0;
        break;
    }
    return hash;
}

void cFrontendDatabase::SetMilestoneDescriptionString(char *const outputStr, const int milestoneType, float currVal, const float goalVal,
                                                      const bool showCurrVal) const {
    if (showCurrVal && currVal > goalVal) {
        currVal = goalVal;
    }
    switch (milestoneType) {
        case 0x33fa23a: {
            char currValTimeToPrint[16];
            char goalValTimeToPrint[16];
            Timer currValTimer(currVal);
            currValTimer.PrintToString(currValTimeToPrint, 4);
            Timer goalValTimer(goalVal);
            goalValTimer.PrintToString(goalValTimeToPrint, 4);
            if (showCurrVal) {
                bSPrintf(outputStr, "%s/%s", currValTimeToPrint, goalValTimeToPrint);
            } else {
                bSPrintf(outputStr, "%s", goalValTimeToPrint);
            }
            break;
        }
        case 0x5392e4fd: {
            float printTime = currVal;
            char currValTimeToPrint[16];
            char goalValTimeToPrint[16];
            if (currVal == 0.0f) {
                IPlayer *player = IPlayer::First(PLAYER_LOCAL);
                if (player) {
                    ISimable *simable = player->GetSimable();
                    if (simable) {
                        IVehicle *vehicle;
                        if (simable->QueryInterface(&vehicle)) {
                            IVehicleAI *vehicleai = vehicle->GetAIVehiclePtr();
                            if (vehicleai) {
                                IPursuit *ipursuit = vehicleai->GetPursuit();
                                if (ipursuit) {
                                    float pursuitElapsedTime = ipursuit->GetPursuitDuration();
                                    float timeRemaining = goalVal - pursuitElapsedTime;
                                    printTime = UMath::Max(0.0f, timeRemaining);
                                }
                            }
                        }
                    }
                }
            }
            Timer currValTimer(printTime);
            currValTimer.PrintToString(currValTimeToPrint, 4);
            Timer goalValTimer(goalVal);
            goalValTimer.PrintToString(goalValTimeToPrint, 4);
            if (showCurrVal) {
                bSPrintf(outputStr, "%s/%s", currValTimeToPrint, goalValTimeToPrint);
            } else {
                bSPrintf(outputStr, "%s", goalValTimeToPrint);
            }
            break;
        }
        default:
            if (showCurrVal) {
                bSPrintf(outputStr, "%$0.0f/%$0.0f", currVal, goalVal);
            } else {
                bSPrintf(outputStr, "%$0.0f", goalVal);
            }
            break;
    }
}

bool cFrontendDatabase::IsMilestoneTimeFormat(const int milestoneType) const {
    if (milestoneType == 0x33fa23a || milestoneType == 0x5392e4fd) {
        return true;
    }
    return false;
}

uint32 cFrontendDatabase::GetRaceNameHash(GRace::Type raceType) {
    switch (raceType) {
        case GRace::kRaceType_P2P:
            return 0xb94fd70e;
        case GRace::kRaceType_Circuit:
            return 0x034fa2c1;
        case GRace::kRaceType_Drag:
            return 0x6f547e4c;
        case GRace::kRaceType_Knockout:
            return 0x4930f5fc;
        case GRace::kRaceType_Tollbooth:
            return 0xa15e4505;
        case GRace::kRaceType_SpeedTrap:
            return 0xee1edc76;
        case GRace::kRaceType_Challenge:
            return 0x213cc8d1;
        default:
            return 0x7818f85e;
    }
}

unsigned int cFrontendDatabase::GetRaceIconHash(GRace::Type type) {
    unsigned int hash = 0;

    switch (type) {
    case 0:
        hash = 0x2521E5EB;
        break;
    case 1:
        hash = 0xE9638D3E;
        break;
    case 2:
        hash = 0xAAAB31E9;
        break;
    case 3:
        hash = 0x3A015595;
        break;
    case 4:
        hash = 0x1A091045;
        break;
    case 5:
        hash = 0x66C9A7B6;
        break;
    case 9:
        hash = 0x66C9A7B6;
        break;
    case 10:
        hash = 0x1A091045;
        break;
    }
    return hash;
}

uint32 cFrontendDatabase::GetSafehouseIconHash(const char *safehouseType) {
    uint32 hash = 0;
    if (bStrICmp(safehouseType, "carlot") == 0) {
        hash = 0x4eaee18b;
    } else if (bStrICmp(safehouseType, "safehouse") == 0) {
        hash = 0x0ed39f69;
    } else if (bStrICmp(safehouseType, "customshop") == 0) {
        hash = 0x0cf07089;
    }
    return hash;
}

GameCompletionStats::GameCompletionStats() {
    m_nOverall = 0;
    m_nCareer = 0;
    m_nRapSheetRankings = 0;
    m_nChallenge = 0;
    m_nTotalChallengeRaces = 0;
    m_nCompletedChallengeRaces = 0;
}

GameCompletionStats cFrontendDatabase::GetGameCompletionStats() {
    GameCompletionStats stats;
    float nTotalCareerRaces = 0.0f;
    float nCompletedCareerRaces = 0.0f;
    float nTotalMilestones = static_cast<float>(GManager::Get().GetNumMilestones());
    float nMilestonesAwarded = 0.0f;
    float nTotalRapSheetRankings = 140.0f;
    float nRapSheetRankings = 0.0f;

    for (unsigned int i = 1; i < 17; i++) {
        GRaceBin *pBin = GRaceDatabase::Get().GetBinNumber(i);
        unsigned int nBossRaces = pBin->GetBossRaceCount();
        for (unsigned int j = 0; j < nBossRaces; j++) {
            if (GRaceDatabase::Get().IsCareerRaceComplete(pBin->GetBossRaceHash(j))) {
                nCompletedCareerRaces += 1.0f;
            }
        }
        unsigned int nWorldRaces = pBin->GetWorldRaceCount();
        for (unsigned int j = 0; j < nWorldRaces; j++) {
            if (GRaceDatabase::Get().IsCareerRaceComplete(pBin->GetWorldRaceHash(j))) {
                nCompletedCareerRaces += 1.0f;
            }
        }
        nTotalCareerRaces += static_cast<float>(nBossRaces + nWorldRaces);
    }

    for (unsigned int i = 0; i < GRaceDatabase::Get().GetRaceCount(); i++) {
        GRaceParameters *pParams = GRaceDatabase::Get().GetRaceParameters(i);
        if (bStrCmp(pParams->GetEventID(), GRaceDatabase::Get().GetBurgerKingRace()) != 0) {
            if (GetIsCollectorsEdition() || !pParams->GetIsCollectorsEditionRace()) {
                if (pParams->GetIsChallengeSeriesRace()) {
                    stats.m_nTotalChallengeRaces++;
                    if (GRaceDatabase::Get().IsQuickRaceComplete(pParams->GetEventHash())) {
                        stats.m_nCompletedChallengeRaces++;
                    }
                }
            }
        }
    }

    for (unsigned int i = 0; static_cast<float>(i) < nTotalMilestones; i++) {
        if (GManager::Get().GetMilestone(i)->GetIsAwarded()) {
            nMilestonesAwarded += 1.0f;
        }
    }

    for (unsigned int i = 0; i < 10; i++) {
        int rankMovement = FEDatabase->GetMultiplayerProfile(0)->GetHighScores()->CalcPursuitRank(static_cast<ePursuitDetailTypes>(i), true);
        if (15 - rankMovement >= 0) {
            nRapSheetRankings += static_cast<float>(15 - rankMovement);
        }
    }

    stats.m_nRapSheetRankings = static_cast<unsigned char>(nRapSheetRankings / nTotalRapSheetRankings * 100.0f);
    stats.m_nCareer = static_cast<unsigned char>((nCompletedCareerRaces + nMilestonesAwarded) / (nTotalCareerRaces + nTotalMilestones) * 100.0f);
    stats.m_nChallenge =
        static_cast<unsigned char>(static_cast<float>(stats.m_nCompletedChallengeRaces) / static_cast<float>(stats.m_nTotalChallengeRaces) * 100.0f);
    stats.m_nOverall = static_cast<unsigned char>(static_cast<float>(static_cast<int>(stats.m_nCareer)) * 0.7f +
                                                  static_cast<float>(static_cast<int>(stats.m_nChallenge)) * 0.2f +
                                                  static_cast<float>(static_cast<int>(stats.m_nRapSheetRankings)) * 0.1f);

    return stats;
}

void InitFrontendDatabase() {
    FEDatabase = new (GetVirtualMemoryAllocParams()) cFrontendDatabase();
    FEDatabase->Default();
}

int GetMikeMannBuild() {
    return MikeMannBuild;
}

int GetIsCollectorsEdition() {
    return IsCollectorsEdition;
}
