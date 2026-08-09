#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/NISAudio.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"

bool SFXObj_NISStream::m_bNISAudioStreamReady = false;            // size: 0x1, address: 0x8041831C, Decl: 33
bool SFXObj_NISStream::m_bNISButtonThroughAnimationReady = false; // size: 0x1, address: 0x80418320, Decl: 34
bool SFXObj_NISStream::m_bNISButtonThroughReady = false;          // size: 0x1, address: 0x80418324, Decl: 35
bool SFXObj_NISStream::m_bIsButtonThrough = false;                // size: 0x1, address: 0x80418328, Decl: 36
int SFXObj_NISStream::m_mstimeelapsed = -1;                       // size: 0x4, address: 0x8041832C, Decl: 37
int SFXObj_NISStream::m_mslengthofstream = -1;                    // size: 0x4, address: 0x80418330, Decl: 38

unsigned int g_laststartanimid = 0;

bool g_bWasLastNISaStart = true;

int DBGPRNT_NIS; // size: 0x4, address: 0xFFFFFFFF, Decl: 40

DEFINE_CREATABLE(0x50, SFXObj_NISStream, SndBase);

uintptr_t uNIS_STRINGHASHMAP[68][3];

#define INIT_NIS_ENTRY(index, scene, name)                                                                                                           \
    uNIS_STRINGHASHMAP[index][1] = scene;                                                                                                            \
    uNIS_STRINGHASHMAP[index][2] = reinterpret_cast<uintptr_t>(name);                                                                                \
    uNIS_STRINGHASHMAP[index][0] = bStringHash(name)

void GenerateNISAnimHashMap() {
    INIT_NIS_ENTRY(0, Csis::Type_NIS_Scene_Start_generic, "GenericStart");
    INIT_NIS_ENTRY(1, Csis::Type_NIS_Scene_Start_IntroNis01, "IntroNis01");
    INIT_NIS_ENTRY(2, Csis::Type_NIS_Scene_Start_IntroNis02, "IntroNis02");
    INIT_NIS_ENTRY(3, Csis::Type_NIS_Scene_Start_IntroNis03, "IntroNis03");
    INIT_NIS_ENTRY(4, Csis::Type_NIS_Scene_Start_IntroNis04, "IntroNis04");
    INIT_NIS_ENTRY(5, Csis::Type_NIS_Scene_Start_IntroNis05, "IntroNis05");
    INIT_NIS_ENTRY(6, Csis::Type_NIS_Scene_Start_IntroNis06, "IntroNis06");
    INIT_NIS_ENTRY(7, Csis::Type_NIS_Scene_Start_IntroNis07, "IntroNis07");
    INIT_NIS_ENTRY(8, Csis::Type_NIS_Scene_Start_IntroNis08, "IntroNis08");
    INIT_NIS_ENTRY(9, Csis::Type_NIS_Scene_Start_IntroNis09, "IntroNis09");
    INIT_NIS_ENTRY(10, Csis::Type_NIS_Scene_Start_IntroNis10, "IntroNis10");
    INIT_NIS_ENTRY(11, Csis::Type_NIS_Scene_Start_flythrough, "Flythrough");
    INIT_NIS_ENTRY(12, Csis::Type_NIS_Scene_Start_dday, "IntroNisDD");
    INIT_NIS_ENTRY(13, Csis::Type_NIS_Scene_Start_ddayend, "IntroNisDDEnd");
    INIT_NIS_ENTRY(14, Csis::Type_NIS_Scene_Start_toll, "IntroNisToll01");
    INIT_NIS_ENTRY(15, Csis::Type_NIS_Scene_Start_safehouse, "GarageEnter");
    INIT_NIS_ENTRY(16, Csis::Type_NIS_Scene_Start_safehouse, "GarageExit");
    INIT_NIS_ENTRY(17, Csis::Type_NIS_Scene_Start_e3, "OPMRivalIntro");
    INIT_NIS_ENTRY(18, Csis::Type_NIS_Blacklist_Black01, "IntroNisBL01");
    INIT_NIS_ENTRY(19, Csis::Type_NIS_Blacklist_Black02, "IntroNisBL02");
    INIT_NIS_ENTRY(20, Csis::Type_NIS_Blacklist_Black03, "IntroNisBL03");
    INIT_NIS_ENTRY(21, Csis::Type_NIS_Blacklist_Black04, "IntroNisBL04");
    INIT_NIS_ENTRY(22, Csis::Type_NIS_Blacklist_Black05, "IntroNisBL05");
    INIT_NIS_ENTRY(23, Csis::Type_NIS_Blacklist_Black06, "IntroNisBL06");
    INIT_NIS_ENTRY(24, Csis::Type_NIS_Blacklist_Black06, "IntroNisBL06End");
    INIT_NIS_ENTRY(25, Csis::Type_NIS_Blacklist_Black07, "IntroNisBL07");
    INIT_NIS_ENTRY(26, Csis::Type_NIS_Blacklist_Black07, "IntroNisBL07End");
    INIT_NIS_ENTRY(27, Csis::Type_NIS_Blacklist_Black08, "IntroNisBL08");
    INIT_NIS_ENTRY(28, Csis::Type_NIS_Blacklist_Black09, "IntroNisBL09");
    INIT_NIS_ENTRY(29, Csis::Type_NIS_Blacklist_Black10, "IntroNisBL10");
    INIT_NIS_ENTRY(30, Csis::Type_NIS_Blacklist_Black11, "IntroNisBL11");
    INIT_NIS_ENTRY(31, Csis::Type_NIS_Blacklist_Black12, "IntroNisBL12");
    INIT_NIS_ENTRY(32, Csis::Type_NIS_Blacklist_Black13, "IntroNisBL13");
    INIT_NIS_ENTRY(33, Csis::Type_NIS_Blacklist_Black14, "IntroNisBL14");
    INIT_NIS_ENTRY(34, Csis::Type_NIS_Blacklist_Black15, "IntroNisBL15");
    INIT_NIS_ENTRY(35, Csis::Type_NIS_Blacklist_Black16, "IntroNisBL16");
    INIT_NIS_ENTRY(36, Csis::Type_NIS_Scene_End_Arrest_F02, "ArrestF02");
    INIT_NIS_ENTRY(37, Csis::Type_NIS_Scene_End_Arrest_F06, "ArrestF06");
    INIT_NIS_ENTRY(38, Csis::Type_NIS_Scene_End_Arrest_F07, "ArrestF07");
    INIT_NIS_ENTRY(39, Csis::Type_NIS_Scene_End_Arrest_F14, "ArrestF14");
    INIT_NIS_ENTRY(40, Csis::Type_NIS_Scene_End_Arrest_F18, "ArrestF18");
    INIT_NIS_ENTRY(41, Csis::Type_NIS_Scene_End_Arrest_F23, "ArrestF23");
    INIT_NIS_ENTRY(42, Csis::Type_NIS_Scene_End_Arrest_M01, "ArrestM01");
    INIT_NIS_ENTRY(43, Csis::Type_NIS_Scene_End_Arrest_M04, "ArrestM04");
    INIT_NIS_ENTRY(44, Csis::Type_NIS_Scene_End_Arrest_M06, "ArrestM06");
    INIT_NIS_ENTRY(45, Csis::Type_NIS_Scene_End_Arrest_M07, "ArrestM07");
    INIT_NIS_ENTRY(46, Csis::Type_NIS_Scene_End_Arrest_M14, "ArrestM14");
    INIT_NIS_ENTRY(47, Csis::Type_NIS_Scene_End_Arrest_M16, "ArrestM16");
    INIT_NIS_ENTRY(48, Csis::Type_NIS_Scene_End_Arrest_M19, "ArrestM19");
    INIT_NIS_ENTRY(49, Csis::Type_NIS_Scene_End_Arrest_M23, "ArrestM23");
    INIT_NIS_ENTRY(50, Csis::Type_NIS_Scene_End_Arrest_F02, "ArrestF02b");
    INIT_NIS_ENTRY(51, Csis::Type_NIS_Scene_End_Arrest_F06, "ArrestF06b");
    INIT_NIS_ENTRY(52, Csis::Type_NIS_Scene_End_Arrest_F07, "ArrestF07b");
    INIT_NIS_ENTRY(53, Csis::Type_NIS_Scene_End_Arrest_F14, "ArrestF14b");
    INIT_NIS_ENTRY(54, Csis::Type_NIS_Scene_End_Arrest_F18, "ArrestF18b");
    INIT_NIS_ENTRY(55, Csis::Type_NIS_Scene_End_Arrest_F23, "ArrestF23b");
    INIT_NIS_ENTRY(56, Csis::Type_NIS_Scene_End_Arrest_M01, "ArrestM01b");
    INIT_NIS_ENTRY(57, Csis::Type_NIS_Scene_End_Arrest_M04, "ArrestM04b");
    INIT_NIS_ENTRY(58, Csis::Type_NIS_Scene_End_Arrest_M06, "ArrestM06b");
    INIT_NIS_ENTRY(59, Csis::Type_NIS_Scene_End_Arrest_M07, "ArrestM07b");
    INIT_NIS_ENTRY(60, Csis::Type_NIS_Scene_End_Arrest_M14, "ArrestM14b");
    INIT_NIS_ENTRY(61, Csis::Type_NIS_Scene_End_Arrest_M16, "ArrestM16b");
    INIT_NIS_ENTRY(62, Csis::Type_NIS_Scene_End_Arrest_M19, "ArrestM19b");
    INIT_NIS_ENTRY(63, Csis::Type_NIS_Scene_End_Arrest_M23, "ArrestM23b");
    INIT_NIS_ENTRY(64, Csis::Type_NIS_Scene_End_end01, "EndingNis01");
    INIT_NIS_ENTRY(65, Csis::Type_NIS_Scene_End_end02, "EndingNis02");
    INIT_NIS_ENTRY(66, Csis::Type_NIS_Scene_End_end03, "EndingNis03");
    INIT_NIS_ENTRY(67, Csis::Type_NIS_Scene_End_end04, "EndingNis04");
}

int GetCsisEventIndex(unsigned int hashid) {
    for (int n = 0; n < NUM_ELEMENTS(uNIS_STRINGHASHMAP); n++) {
        if (uNIS_STRINGHASHMAP[n][2] == hashid) {
            return n;
        }
    }
    return -1;
}

SFXObj_NISStream::SFXObj_NISStream() : CARSFX() {
    this->m_bNISAnimationReady = false;
    this->m_bNISButtonThroughAnimationReady = false;
    this->m_bNISAudioStreamReady = false;
    this->m_bNISButtonThroughReady = false;
    this->m_bBackupStreamCleared = true;
    this->m_mselapsedtimecb = nullptr;
}

SFXObj_NISStream::~SFXObj_NISStream() {
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 5, 0);
}

void SFXObj_NISStream::InitSFX() {
    SndBase::InitSFX();
    GenerateNISAnimHashMap();
    m_bNISAnimationReady = false;
    m_bNISButtonThroughReady = false;
    m_bNISButtonThroughAnimationReady = false;
    m_bNISAudioStreamReady = false;
    m_bBackupStreamCleared = true;
    g_pEAXSound->GetSndGameMode();
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 5, 0);
}

bool SFXObj_NISStream::QueueNISStream(Attrib::Key anim_id, int camera_track_number, void (*setmstimecb)(uint32, int), bool bbuttonthrough) {
    this->m_mselapsedtimecb = setmstimecb;
    this->m_mstimeelapsed = 0;
    return this->QueueNISStream(anim_id, camera_track_number, bbuttonthrough, true);
}

void SFXObj_NISStream::StopStream() {
    Speech::Manager::GetSpeechModule(0)->PurgeSpeech();
}

bool SFXObj_NISStream::QueueNISStream(unsigned int anim_id, int camera_track_number, bool bbuttonthrough, bool btracktime) {
    this->m_bNISAnimationReady = false;
    this->m_animid = anim_id;

    if (!btracktime) {
        this->m_mselapsedtimecb = nullptr;
    }

    Speech::SED_NISSFX *nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));
    this->m_bIsButtonThrough = bbuttonthrough;

    if (bbuttonthrough == true || nismgr->GetStreamChannel()->IsPlaying()) {
        nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));
        nismgr->PurgeSpeech();
    }

    Csis::Type_NIS_Track csiscamtrack = Csis::Type_NIS_Track_Track00;

    switch (camera_track_number) {
        case 0:
            csiscamtrack = Csis::Type_NIS_Track_Track00;
            break;
        case 1:
            csiscamtrack = Csis::Type_NIS_Track_Track01;
            break;
        case 2:
            csiscamtrack = Csis::Type_NIS_Track_Track02;
            break;
        case 3:
            csiscamtrack = Csis::Type_NIS_Track_Track03;
            break;
        case 4:
            csiscamtrack = Csis::Type_NIS_Track_Track04;
            break;
        case 5:
            csiscamtrack = Csis::Type_NIS_Track_Track05;
            break;
        case 6:
            csiscamtrack = Csis::Type_NIS_Track_Track06;
            break;
        case 7:
            csiscamtrack = Csis::Type_NIS_Track_Track07;
            break;
        case 8:
            csiscamtrack = Csis::Type_NIS_Track_Track08;
            break;
        case 9:
            csiscamtrack = Csis::Type_NIS_Track_Track09;
            break;
        case 10:
            csiscamtrack = Csis::Type_NIS_Track_Track10;
            break;
        case 11:
            csiscamtrack = Csis::Type_NIS_Track_Track11;
            break;
        case 12:
            csiscamtrack = Csis::Type_NIS_Track_Track12;
            break;
        case 13:
            csiscamtrack = Csis::Type_NIS_Track_Track13;
            break;
        default:
            break;
    }

    bool breturn = false;
    int CSISindex = GetCsisEventIndex(anim_id);
    int id;

    if (CSISindex == -1) {
        this->m_bNISAudioStreamReady = true;
        SetSoundControlState(false, SNDSTATE_NIS_STORY, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_INTRO, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_321, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_BLK, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_ARREST, "Clear NIS");
        return breturn;
    }

    if (CSISindex != 0) {
        csiscamtrack = Csis::Type_NIS_Track_Track00;
    }

    SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(g_pEAXSound->GetSFXBase_Object(0x40010010));

    if (CSISindex < 18) {
        if (CSISindex == 24) {
            bbuttonthrough = true;
            CSISindex = 23;
        } else if (CSISindex == 26) {
            bbuttonthrough = true;
            CSISindex = 25;
        }

        g_laststartanimid = anim_id;
        g_bWasLastNISaStart = true;

        if (bbuttonthrough == true) {
            SetSoundControlState(false, SNDSTATE_NIS_321, "NIS 321");

            Csis::NIS_Select_Start(static_cast<Csis::Type_NIS_Scene_Start>(uNIS_STRINGHASHMAP[CSISindex][1]), csiscamtrack,
                                   Csis::Type_NIS_Section_End);

            breturn = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);

            this->SetDMIX_Input(6, 0);

            if (ppf != nullptr) {
                ppf->SetNISPlaying(false);
            }
        } else {
            Csis::NIS_Select_Start(static_cast<Csis::Type_NIS_Scene_Start>(uNIS_STRINGHASHMAP[CSISindex][1]), csiscamtrack,
                                   Csis::Type_NIS_Section_Complete);

            breturn = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);

            this->SetDMIX_Input(6, 0x7FFF);

            if (ppf != nullptr) {
                ppf->SetNISPlaying(true);
            }
        }
    } else if (CSISindex < 36) {
        g_laststartanimid = anim_id;
        g_bWasLastNISaStart = true;

        if (bbuttonthrough == true) {
            SetSoundControlState(false, SNDSTATE_NIS_321, "NIS 321");

            Csis::NIS_Select_Blacklist(csiscamtrack, Csis::Type_NIS_Section_End,
                                       static_cast<Csis::Type_NIS_Blacklist>(uNIS_STRINGHASHMAP[CSISindex][1]));

            breturn = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);

            this->SetDMIX_Input(6, 0);

            if (ppf != nullptr) {
                ppf->SetNISPlaying(false);
            }
        } else {
            Csis::NIS_Select_Blacklist(csiscamtrack, Csis::Type_NIS_Section_Complete,
                                       static_cast<Csis::Type_NIS_Blacklist>(uNIS_STRINGHASHMAP[CSISindex][1]));

            breturn = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);

            this->SetDMIX_Input(6, 0x7FFF);

            if (ppf != nullptr) {
                ppf->SetNISPlaying(true);
            }
        }
    } else {
        g_bWasLastNISaStart = false;

        Csis::NIS_Select_End(static_cast<Csis::Type_NIS_Scene_End>(uNIS_STRINGHASHMAP[CSISindex][1]), Csis::Type_NIS_Track_Track00,
                             Csis::Type_NIS_Section_Complete);

        breturn = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_BUSTED, &SFXObj_NISStream::PlayNISStream, false);

        this->SetDMIX_Input(7, 0x7FFF);

        if (ppf != nullptr) {
            ppf->SetNISPlaying(true);
        }
    }

    if (breturn == false) {
        this->m_bNISAudioStreamReady = true;
    }

    return breturn;
}

void SFXObj_NISStream::NISActivityDone() {
    this->SetDMIX_Input(6, 0);
    this->SetDMIX_Input(7, 0);
    this->m_mselapsedtimecb = nullptr;
    this->m_mstimeelapsed = -1;
    this->m_mslengthofstream = -1;

    // TODO magic
    int id = 0x40010010;
    SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(g_pEAXSound->GetSFXBase_Object(id));
    if (ppf != nullptr) {
        ppf->SetNISPlaying(false);
    }
}

void SFXObj_NISStream::StartNIS() {
    this->m_bNISAnimationReady = true;
    if (this->m_bNISAudioStreamReady) {
        bool bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(2);
        SNDSYS_service();
        this->m_bNISAudioStreamReady = false;
    }
}

void SFXObj_NISStream::UpdateParams(float t) {
    if (this->m_bNISAnimationReady && this->m_bNISAudioStreamReady) {
        bool bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(2);
        EAXS_StreamChannel *pch = Speech::Manager::GetSpeechModule(0)->GetStreamChannel();

        if (this->m_mslengthofstream == 0) {
            SNDSYS_service();
            this->m_mslengthofstream = pch->GetTimeRemaining();
        }

        if (bresult) {
            this->m_bNISAnimationReady = false;
            this->m_bNISAudioStreamReady = false;
            this->m_bBackupStreamCleared = false;
        }
    } else if (this->m_bNISButtonThroughAnimationReady && this->m_bNISButtonThroughReady) {
        bool bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(7);

        if (bresult) {
            this->m_bNISButtonThroughAnimationReady = false;
            this->m_bNISButtonThroughReady = false;
        }
    } else if (this->m_mselapsedtimecb != nullptr) {
        EAXS_StreamChannel *pch = Speech::Manager::GetSpeechModule(0)->GetStreamChannel();

        if (this->m_mslengthofstream == 0) {
            this->m_mslengthofstream = pch->GetTimeRemaining();
        }

        this->m_mstimeelapsed = pch->GetCurrentTime();
        this->m_mselapsedtimecb(this->m_animid, this->m_mstimeelapsed);
    }
}

void SFXObj_NISStream::PlayNISStream() {
    m_bNISButtonThroughReady = false;
    m_bNISAudioStreamReady = true;
    m_mstimeelapsed = 0;
    m_mslengthofstream = 0;

    EAXS_StreamChannel *pch = Speech::Manager::GetSpeechModule(0)->GetStreamChannel();
    m_mslengthofstream = pch->GetTimeRemaining();
}

void SFXObj_NISStream::Destroy() {}
