#include "./EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSND8Wrapper.hpp"
#include "Speed/Indep/Src/EAXSound/NFSMixMaster.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSLiveLink.hpp"
#include "Speed/Indep/Src/EAXSound/SimStates/EAX_HeliState.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_PFEATrax.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Reverb.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_AICar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CarState.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CopCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Main.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Music.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_PlayerCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_TrafficCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Truck.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include <new>

extern Attrib::Gen::shiftpattern *g_ShiftInfo;
extern Attrib::Gen::turbosfx *g_TurboInfo;

#include "Speed/Indep/Src/EAXSound/EAXCommon.hpp"
#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"

Attrib::StringKey crcEngineAudio("EngineAudio");
Attrib::StringKey crcAudioSystem("AudioSystem");
Attrib::StringKey crcMostWanted("MostWanted");
Attrib::StringKey crcDrivetrain("Drivetrain");
Attrib::StringKey crcSkidParams("SkidParams");
Attrib::StringKey crcTurboSFX("TurboSFX");
Attrib::StringKey crcAccelTrans("AccelTrans");
Attrib::StringKey crcShiftPattern("ShiftPattern");
Attrib::StringKey crcSweetener("Sweetener");
Attrib::StringKey crcEnglish("English");
Attrib::StringKey crcCarHitWall("CarHitWall");
Attrib::StringKey crcLicensedMusic("LicensedMusic");
Attrib::StringKey crcMusic("Music");
EAXAemsManager gAEMSMgr;
stSndDataLoadParams g_SndAssetList[48];
Slope TablePitch(0.0f, 1.0f, 3500.0f, 4500.0f);
Slope BreakingPitchVsSpeed(3000.0f, 5000.0f, 30.0f, 100.0f);

extern unsigned int SoundRandomSeed;
extern float g_SliderValue;
extern float g_fMasterSFXVolume;
extern int g_iMasterSFXVolume;
extern int g_DMIX_DummyInputBlock[];
extern char *g_pcsCSISAllocString;
extern char csCSISdebug[];
extern bool bIsMapInQueuedFileLoad;
extern unsigned int g_ActiveCtlStates;
extern unsigned int g_laststartanimid;
extern bool g_bWasLastNISaStart;
extern bool IsAudioStreamingEnabled;
extern bool IsSpeechEnabled;
extern int _RUN_SOUND_STATE;
extern unsigned int g_ActiveSFXStates;
extern unsigned int g_PrevActiveCtlStates;
extern unsigned int g_PrevActiveSFXStates;
extern unsigned int g_CtlStateActions[18];
extern const char *csfxedit[];
extern bool bHasStartNewGameOccured;
extern bool gbHasStartNewGamePlayBeenProcessed;
extern bool bIsAnFEToIngameTransition;
extern bool bHasDataLoadOccured;
extern int gIsPauseForPause;
extern int gnHasStartLoadFEBeenProcessed;
extern int gb_DORESTART_RACE;
extern int gb_Is321;
extern int SkipFE;
extern int SkipFELanguage;
extern int DisableSoundUpdate;
extern bool gbAudioInterruptsWorldDataRead;
extern bool gbWorldDataBlocksAudioRead;
extern bool bReadCallbackToggle;
extern bool bStreamBlockState;
extern bool bAudioInterrupt;
extern bool bStreamReadTiming;
extern unsigned int uStreamBlockTicks;
extern unsigned int uStreamReadTicks;
extern unsigned int uAudioInterruptTicks;
extern int GameFlowSndState[];
extern void SNDSYS_service();
extern int GetCurrentLanguage();
void RegisterStates();
void RegisterSFX();

unsigned int bRandom(int range, unsigned int *seed);
float bRandom(float range, unsigned int *seed);
int bSPrintf(char *destString, const char *fmt, ...);
int bStrICmp(const char *s1, const char *s2);
void SoundPause(bool on, eSNDPAUSE_REASON reason);
void SetSoundControlState(bool on, eSNDCTLSTATE state, const char *caller);

bool g_EAXIsPaused() {
    return (g_ActiveCtlStates & 0x3483b) != 0;
}

void EAXSound::START_321Countdown() {
    int id;
    SFXObj_Pathfinder *ppf;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40010010;
        ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(id));
        if (ppf) {
            ppf->Set321(true);
        }
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        if (pnis) {
            pnis->StartNIS();
        }
    }
}

bool EAXSound::AreResourceLoadsPending() {
    return gAEMSMgr.AreResourceLoadsPending();
}

void EAXSound::QueueNISButtonThrough(unsigned int anim_id, int camera_track_number) {
    int id;
    SFXObj_NISStream *pnis;
    Speech::Module *cop_speech;

    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return;
    }

    id = 0x40000050;
    pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
    if (camera_track_number == -1) {
        if (g_laststartanimid != 0x0D0E5A9D && g_laststartanimid != 0xCBFF6594) {
            pnis->QueueNISStream(g_laststartanimid, -1, true, false);
        }
    } else if (g_bWasLastNISaStart) {
        pnis->QueueNISStream(anim_id, camera_track_number, true, false);
    } else {
        pnis->StopStream();
    }

    Speech::Manager::ClearPlayback();
    id = COPSPEECH_MODULE;
    cop_speech = Speech::Manager::GetSpeechModule(id);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
    }
}

void EAXSound::QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int)) {
    int id;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        pnis->QueueNISStream(anim_id, camera_track_number, setmstimecb, false);
    }
}

bool EAXSound::IsNISStreamQueued() {
    int id;
    SFXObj_NISStream *pnis;

    if (!IsSoundEnabled || !IsAudioStreamingEnabled) {
        return true;
    }
    id = 0x40000050;
    pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
    return pnis->IsNISStreamReady();
}

void EAXSound::NISFinished() {
    int id;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        pnis->NISActivityDone();
        SoundPause(false, eSNDPAUSE_NISON);
        SetSoundControlState(false, SNDSTATE_NIS_STORY, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_INTRO, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_BLK, "EAXSound::NISFinished");
        SetSoundControlState(false, SNDSTATE_NIS_ARREST, "EAXSound::NISFinished");
    }
}

// NISFinished goes here when implemented

void EAXSound::PlayNIS() {
    int id;
    SFXObj_NISStream *pnis;

    if (IsSoundEnabled && IsAudioStreamingEnabled) {
        id = 0x40000050;
        pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
        pnis->StartNIS();
        SoundPause(true, eSNDPAUSE_NISON);
    }
}

void EAXSound::PlayUISoundFX(eMenuSoundTriggers etriggertype) {
    if (IsSoundEnabled) {
        if (etriggertype < UISND_COMMON_MAX_NUM) {
            if (m_pCmnSnd != nullptr) {
                m_pCmnSnd->Play(etriggertype);
            }
        } else if (etriggertype < UISND_FRONTEND_MAX_NUM) {
            etriggertype = static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM);
            if (m_pFESnd != nullptr) {
                m_pFESnd->Play(etriggertype);
            }
        }
    }
}

// PlayUISoundFX, StopUISoundFX go here when implemented

void EAXSound::StopUISoundFX(eMenuSoundTriggers etriggertype) {
    if (IsSoundEnabled) {
        if (etriggertype < UISND_COMMON_MAX_NUM) {
            if (m_pCmnSnd != nullptr) {
                m_pCmnSnd->Stop(etriggertype);
            }
        } else {
            etriggertype = static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM);
            if (m_pFESnd != nullptr) {
                m_pFESnd->Stop(etriggertype);
            }
        }
    }
}

void EAXSound::SetCsisName(SndBase *psndbase) {
    int ninst = psndbase->GetUniqueID();
    bSPrintf(csCSISdebug, " %s, 0x%x ", psndbase->GetTypeName(), ninst);
    SetCsisName(csCSISdebug);
}

void EAXSound::SetCsisName(char *pcsAllocName) {
    g_pcsCSISAllocString = pcsAllocName;
}

EAXSound::EAXSound() {
    int nloop;

    m_pcsCsisName = "SOUND";
    bPlayCarSounds = true;
    mAttributes = nullptr;
    mLocalAttr = nullptr;
    m_pEAXSND8Wrapper = nullptr;
    m_bAudioIsPaused = false;
    m_X360_UI_Override = false;
    m_eSndGameMode = SND_MODE_NONE;
    m_prevSndGameMode = SND_MODE_NONE;
    m_pStreamManager = nullptr;
    m_nStereoUpgradeLevel = 0;
    m_bIsPaused = false;
    m_pCurAudioSettings = nullptr;
    m_nSpeechLoadBankIndex = 0;
    m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
    m_pNewSongInfoSt = nullptr;
    EngineLoadingBlocked = false;
    FrameCnt = 0;
    m_bIsSpecialUGMovie = false;
    m_pCmnSnd = nullptr;
    m_pFESnd = nullptr;
    m_pSTICH_Playback = nullptr;
    m_pNFSLiveLink = nullptr;
    m_pNFSMixMaster = nullptr;
    bPlayCameraSnapShot = false;
    mmsgMRestartRace = nullptr;
    m_bPause_MainFNG = false;
    mEventID = 0;
    mData.fEventID = 0xF2D10992; // TODO magic
    nloop = 0;
}

EAXSound::~EAXSound() {
    gSpeechCache.Dump();

    if (mAttributes) {
        delete mAttributes;
        mAttributes = nullptr;
    }

    if (mLocalAttr) {
        delete mLocalAttr;
        mLocalAttr = nullptr;
    }

    if (m_pCmnSnd) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    if (m_pFESnd) {
        delete m_pFESnd;
        m_pFESnd = nullptr;
    }

    if (m_pEAXSND8Wrapper) {
        delete m_pEAXSND8Wrapper;
        m_pEAXSND8Wrapper = nullptr;
    }

    if (g_pNISRevMgr) {
        delete g_pNISRevMgr;
    }
}

int *EAXSound::GetPointerCallback(int nid) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs) {
        return pbs->GetOutputBlockPtr() ? pbs->GetOutputBlockPtr() : nullptr;
    }
    return g_DMIX_DummyInputBlock;
}

void EAXSound::SetSFXOutCallback(int nid, int *ptr) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs) {
        pbs->SetOutputsPtr(ptr);
    }
    int *pclear = ptr;
    for (int n = 0; n < 15; n++) {
        *pclear = 0;
        pclear++;
    }
}

bool EAXSound::SetSFXInputCallback(int nid, int *ptr) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs) {
        pbs->SetInputsPtr(ptr);
        if (pbs->GetStateBase()) {
            return pbs->GetStateBase()->IsAttached();
        }
    }
    return false;
}

int EAXSound::GetStateRefCount(int nstate) {
    if (!m_pStateMgr[nstate]) {
        return 0;
    }
    return m_pStateMgr[nstate]->GetStateObjCount();
}

void EAXSound::SetSFXBaseObject(SFX_Base *psb, eMAINMAPSTATES estate, int ntype, int instance) {
    switch (estate) {
    case eMM_MAIN:
        switch (ntype) {
        case SFXOBJ_MUSIC:
        case SFXOBJ_AMBIENCE:
        case SFXOBJ_MOVIES:
        case SFXOBJ_COMMON:
            break;
        case SFXOBJ_SPEECH:
            Speech::Manager::AttachSFXOBJ(COPSPEECH_MODULE, psb, SFXOBJ_SPEECH);
            return;
        case SFXOBJ_NISPROJ_STRMS:
        case SFXOBJ_MOMENT_STRMS:
            Speech::Manager::AttachSFXOBJ(NISSFX_MODULE, psb, static_cast<eSFXOBJ_MAIN_TYPES>(ntype));
            return;
        case SFXOBJ_FEHUD:
            if (m_pFESnd) {
                m_pFESnd->AttachSFXOBJ(psb, SFXOBJ_FEHUD);
            }
            if (m_pCmnSnd) {
                m_pCmnSnd->AttachSFXOBJ(psb, SFXOBJ_FEHUD);
            }
            return;
        }
        break;
    case eMM_MUSIC:
    case eMM_PLAYERCAR:
    case eMM_AIRACECAR:
    case eMM_COPCAR:
    case eMM_TRAFFIC:
    case eMM_ENVIRONMENT:
        break;
    case eMM_COLLISION:
        Speech::Manager::AttachSFXOBJ(NISSFX_MODULE, psb, static_cast<eSFXOBJ_MAIN_TYPES>(ntype));
        return;
    }
}

// GetSFXBase_Object, GetSndBase_Object go here when implemented

SFX_Base *EAXSound::GetSFXBase_Object(int nID) {
    SFX_Base *ReturnObj = nullptr;
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    if (m_pStateMgr[nState]) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        if (!m_pStateMgr[nState]->GetStateObj(nInstanceID)) {
            return nullptr;
        }
        if ((nID & 0xE0000000) == 0x40000000) {
            ReturnObj = static_cast<SFX_Base *>(m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number));
        }
    }
    return ReturnObj;
}

SndBase *EAXSound::GetSndBase_Object(int nID) {
    SndBase *ReturnObj = nullptr;
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    if (m_pStateMgr[nState]) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        if (!m_pStateMgr[nState]->GetStateObj(nInstanceID)) {
            return nullptr;
        }
        if ((nID & 0xE0000000) == 0x40000000) {
            ReturnObj = m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number);
        }
        if ((nID & 0xE0000000) == 0x60000000) {
            ReturnObj = m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXCTLObject(SFXID_Number);
        }
    }
    if (!ReturnObj) {
        return nullptr;
    }
    return ReturnObj;
}

float EAXSound::GetCurMusicVolume() {
    if (m_eSndGameMode == SND_FRONTEND) {
        return m_pCurAudioSettings->GetMasteredFEMusicVol();
    }
    return m_pCurAudioSettings->GetMasteredIGMusicVol();
}

void EAXSound::ReInitMasterVolumes() {
    int i;
}

void EAXSound::UpdateVolumes(AudioSettings *paudiosettings, float NewValue) {
    m_pCurAudioSettings = paudiosettings;
    ReInitMasterVolumes();
    g_fMasterSFXVolume = paudiosettings->AmbientVol;
    g_iMasterSFXVolume = static_cast<int>(paudiosettings->AmbientVol * 32767.0f);
    g_SliderValue = NewValue;
}

unsigned int EAXSound::Random(int range) {
    return bRandom(range, &SoundRandomSeed);
}

float EAXSound::Random(float range) {
    return bRandom(range, &SoundRandomSeed);
}

void EAXSound::UpdateSongInfo() {}

void EAXSound::InitializeDriver() {
    if (!IsSoundEnabled) {
        return;
    }

    m_pEAXSND8Wrapper =
        new (gAudioMemoryManager.AllocateMemory(sizeof(EAXSND8Wrapper), "EAXSND8Wrapper", false)) EAXSND8Wrapper();
    if (!m_pEAXSND8Wrapper->Initialize()) {
        IsSoundEnabled = false;
        return;
    }

    gAEMSMgr.InitSPUram();
    m_pNFSMixMaster = new (gAudioMemoryManager.AllocateMemory(sizeof(NFSMixMaster), "NFSMixMaster", false))
        NFSMixMaster();
    m_pSTICH_Playback = new (gAudioMemoryManager.AllocateMemory(sizeof(cSTICH_PlayBack), "STICH_PlayBack", false))
        cSTICH_PlayBack();
    new (gAudioMemoryManager.AllocateMemory(sizeof(NIS_RevManager), "NISRevMan", false)) NIS_RevManager();

    for (int n = 0; n < 13; n++) {
        m_pStateMgr[n] = nullptr;
    }

    m_pStateMgr[0] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_Main), "SND: CSTATEMGR_MAIN", false))
            CSTATEMGR_Main();
    m_pStateMgr[0]->Initialize(static_cast<eMAINMAPSTATES>(0));

    m_pStateMgr[1] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_Music), "SND: CSTATEMGR_Music", false))
            CSTATEMGR_Music();
    m_pStateMgr[1]->Initialize(static_cast<eMAINMAPSTATES>(1));

    m_pStateMgr[2] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_PlayerCar), "SND: CSTATEMGR_PlyrCar", false))
            CSTATEMGR_PlayerCar();
    m_pStateMgr[2]->Initialize(static_cast<eMAINMAPSTATES>(2));

    m_pStateMgr[3] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_AICar), "SND: CSTATEMGR_AICar", false))
            CSTATEMGR_AICar();
    m_pStateMgr[3]->Initialize(static_cast<eMAINMAPSTATES>(3));

    m_pStateMgr[4] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_CopCar), "SND: CSTATEMGR_CarState", false))
            CSTATEMGR_CopCar();
    m_pStateMgr[4]->Initialize(static_cast<eMAINMAPSTATES>(4));

    m_pStateMgr[5] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_TrafficCar), "SND: CSTATEMGR_TrafficCar", false))
            CSTATEMGR_TrafficCar();
    m_pStateMgr[5]->Initialize(static_cast<eMAINMAPSTATES>(5));

    m_pStateMgr[6] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_Enviro), "SND: CSTATEMGR_ENVIRO", false))
            CSTATEMGR_Enviro();
    m_pStateMgr[6]->Initialize(static_cast<eMAINMAPSTATES>(6));

    m_pStateMgr[7] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_Collision), "SND: CSTATEMGR_Collision", false))
            CSTATEMGR_Collision();
    m_pStateMgr[7]->Initialize(static_cast<eMAINMAPSTATES>(7));

    m_pStateMgr[8] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_DriveBy), "SND: CSTATEMGR_DriveBy", false))
            CSTATEMGR_DriveBy();
    m_pStateMgr[8]->Initialize(static_cast<eMAINMAPSTATES>(8));

    m_pStateMgr[11] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_Helicopter), "SND: CSTATEMGR_Helicopter", false))
            CSTATEMGR_Helicopter();
    m_pStateMgr[11]->Initialize(static_cast<eMAINMAPSTATES>(11));

    m_pStateMgr[12] =
        new (gAudioMemoryManager.AllocateMemory(sizeof(CSTATEMGR_Truck), "SND: CSTATEMGR_Truck", false))
            CSTATEMGR_Truck();
    m_pStateMgr[12]->Initialize(static_cast<eMAINMAPSTATES>(12));

    m_pNFSMixMaster->AssignSFXCallbacks(EAXSound::GetPointerCallback, EAXSound::SetSFXOutCallback,
                                        EAXSound::SetSFXInputCallback, EAXSound::GetStateRefCount,
                                        EAXSound::MixMapReadyCallback);
}

void EAXSound::RefreshLocalAttr() {
    if (mLocalAttr != nullptr) {
        delete mLocalAttr;
        mLocalAttr = nullptr;
    }

    switch ((SkipFE == 0) ? GetCurrentLanguage() : SkipFELanguage) {
    case 1:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(1).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 2:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(2).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 3:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(3).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 4:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(4).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 10:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(5).GetCollectionWithDefault(), 0, nullptr);
        break;
    case 0:
    default:
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(0).GetCollectionWithDefault(), 0, nullptr);
        break;
    }
}

void EAXSound::InitializeSoundBootLoad() {
    if (!IsSoundEnabled) {
        return;
    }

    mAttributes = new Attrib::Gen::audiosystem(0x7e4b0ed2, 0, nullptr);
    RefreshLocalAttr();
    gAEMSMgr.Init();
    gSpeechCache.Init(0x1B000);
    CSTATEMGR_Base::ClearClassLists();
    RegisterStates();
    RegisterSFX();
}

void EAXSound::StartNewGamePlay() {
    if (!IsSoundEnabled) {
        return;
    }

    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, "RestartRace");

    if (bHasStartNewGameOccured) {
        if (m_pCmnSnd) {
            delete m_pCmnSnd;
            m_pCmnSnd = nullptr;
        }

        if (m_pStreamManager) {
            for (int i = 0; i < 4; i++) {
                EAXS_StreamChannel *channel = m_pStreamManager->GetStreamChannel(i);
                if (channel && i != 1) {
                    m_pStreamManager->GetStreamChannel(i)->Stop();
                    m_pStreamManager->GetStreamChannel(i)->PurgeStream();
                }
            }
        }

        for (int i = 0; i < 13; i++) {
            if (i != 1) {
                CSTATEMGR_Base *mgr = m_pStateMgr[i];
                if (mgr) {
                    mgr->ExitWorld();
                }
            } else if (m_pStateMgr[i]) {
                m_pStateMgr[i]->DisconnectMixMap();
            }
        }

        if (m_pNFSMixMaster->m_bMapReady) {
            m_pNFSMixMaster->DestroyMainMainMap();
        }

        gAEMSMgr.DestroySlots(false);
    }

    if (m_ePlayerMixMode != EAXS3D_TWO_PLAYER_MIX) {
        EAXAemsManager::m_RequiredSlots[3]++;
    }

    CSTATEMGR_AICar::QueueSlots();
    gAEMSMgr.InitializeSlots(!bHasStartNewGameOccured);
    g_ShiftInfo = nullptr;
    bHasStartNewGameOccured = true;
    g_TurboInfo = nullptr;

    if (!bIsMapInQueuedFileLoad) {
        bIsMapInQueuedFileLoad = true;

        GRaceParameters *race = nullptr;
        if (GRaceStatus::Exists()) {
            race = GRaceStatus::Get().GetRaceParameters();
        } else {
            GRaceCustom *startupRace = GRaceDatabase::Get().GetStartupRace();
            race = startupRace;
        }

        if (race) {
            GRace::Type raceType = race->GetRaceType();
            if (raceType == GRace::kRaceType_Drag) {
                eSndGameMode oldMode = m_eSndGameMode;
                m_eSndGameMode = SND_DRAGRACE;
                m_prevSndGameMode = oldMode;
                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    m_pNFSMixMaster->CreateMainMainMap(eRACE_TWODRG);
                } else {
                    m_pNFSMixMaster->CreateMainMainMap(eRACE_DRAG);
                }
            } else {
                SFXObj_PFEATrax *track = static_cast<SFXObj_PFEATrax *>(GetSFXBase_Object(0x40010010));
                if (track) {
                    track->RestartRace();
                }
                eSndGameMode oldMode = m_eSndGameMode;
                eSndGameMode gameMode;
                if (raceType == GRace::kRaceType_Challenge) {
                    gameMode = SND_CHALLENGERACE;
                } else {
                    gameMode = SND_STREETRACE;
                }
                m_eSndGameMode = gameMode;
                m_prevSndGameMode = oldMode;
                if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN) {
                    m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
                } else {
                    m_pNFSMixMaster->CreateMainMainMap(eRACE_TWOCIRC);
                }
            }
        } else {
            eSndGameMode oldMode = m_eSndGameMode;
            m_eSndGameMode = SND_FREEROAM;
            m_prevSndGameMode = oldMode;
            if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN) {
                m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
            } else {
                m_pNFSMixMaster->CreateMainMainMap(eRACE_TWOCIRC);
            }
        }
    }

    InitializeInGame();
    m_pNFSMixMaster->InitMixMap(0);

    SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(0x40010010));
    if (ppf) {
        if (m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
            ppf->m_Flags |= 2;
        } else {
            ppf->m_Flags &= ~2u;
        }

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceParameters() &&
            GRaceStatus::Get().GetRaceParameters()->GetRaceType() != GRace::kRaceType_Drag) {
            ppf->m_Flags |= 1;
        }
    }

    gbHasStartNewGamePlayBeenProcessed = true;
}

void EAXSound::InitializeInGame() {
    if (!IsSoundEnabled) {
        return;
    }

    int nstate = -1;
    m_pCmnSnd = new EAXCommon();
    m_pCmnSnd->Initialize();

    SoundRandomSeed = bRandom(-1);
    if (!bIsAnFEToIngameTransition) {
        nstate = 1;
    }
    bIsAnFEToIngameTransition = false;

    for (int n = 0; n < 13; n++) {
        if (n != nstate && m_pStateMgr[n]) {
            m_pStateMgr[n]->EnterWorld(m_eSndGameMode);
        }
    }

    mEventID = Scheduler::Get().fSchedule_OncePerGameLoop->AddTask(-0x0D2EF66E, &mData, 6, true, 0, 0);
}

void EAXSound::InitializeFrontEnd() {
    if (!IsSoundEnabled) {
        return;
    }

    if (mAttributes == nullptr) {
        mAttributes = new Attrib::Gen::audiosystem(0x7e4b0ed2, 0, nullptr);
    }

    if (mLocalAttr == nullptr) {
        mLocalAttr = new Attrib::Gen::audiosystem(mAttributes->Locales(0).GetCollectionWithDefault(), 0, nullptr);
    }

    m_pCmnSnd = new EAXCommon();
    m_pFESnd = new EAXFrontEnd();
    RefreshLocalAttr();
    Speech::Manager::Init(2);
    SoundRandomSeed = bRandom(-1);
    m_pCurAudioSettings = &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings;
    EAXAemsManager::m_RequiredSlots[3]++;
    gAEMSMgr.InitializeSlots(true);

    if (m_pStateMgr[0] != nullptr) {
        m_pStateMgr[0]->EnterWorld(m_eSndGameMode);
    }
    if (m_pStateMgr[1] != nullptr) {
        m_pStateMgr[1]->EnterWorld(m_eSndGameMode);
    }

    g_pEAXSound->InitEATRAX();
}

void EAXSound::MixMapReadyCallback() {
    bIsMapInQueuedFileLoad = false;
    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->SafeConnectOrphanObjects();
        }
    }
    g_pEAXSound->AttachPlayerCars();
}

void EAXSound::AttachPlayerCars() {
    int num_attached = 0;
    for (int n = 0; n < UTL::Collections::Listable<EAX_CarState, 10>::Count(); n++) {
        if (UTL::Collections::Listable<EAX_CarState, 10>::GetList()[n]->GetContext() == Sound::CONTEXT_PLAYER) {
            num_attached++;
            CSTATE_Base *newcar =
                m_pStateMgr[eMM_PLAYERCAR]->GetFreeState(UTL::Collections::Listable<EAX_CarState, 10>::GetList()[n]);
            if (newcar != nullptr) {
                newcar->Attach(UTL::Collections::Listable<EAX_CarState, 10>::GetList()[n]);
                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    if (num_attached == 2) {
                        return;
                    }
                }
                if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN && num_attached == 1) {
                    return;
                }
            }
        }
    }
}

void EAXSound::InitEATRAX() {
    UpdateSongInfo();
}

void EAXSound::PlayFEMusic(int nIndex) {
    MControlPathfinder(true, 0, 0, 0).Send(UCrc32("Pathfinder5"));
}

unsigned int GenerateUpgradedEngine(EAX_CarState *pCar, int playerUpgrade);

namespace Physics {
namespace Info {

float MaxTorque(const Attrib::Gen::engine &engine, float &atrpm);

} // namespace Info
} // namespace Physics

// Sound::Wheel::Reset and Sound::Engine::Reset are defined inline in EAXCarState.hpp.




















EAX_CarState::EAX_CarState(const Attrib::Collection *atr, Sound::Context context, unsigned int wuid, HSIMABLE__ *handle)
    : mVel0(0.0f, 0.0f, 0.0f) //
    , mRacePos(0) //
    , mBrake(0.0f) //
    , mEBrake(0.0f) //
    , mNosEmptyFlag(0) //
    , mMovementMode(Sound::PHYSICS_MOVEMENT) //
    , mPlayerZone(Sound::PLAYER_ZONE_NONE) //
    , mSteering(0) //
    , mAngle(0) //
    , mSirenState(SIREN_OFF) //
    , mHotPursuit(0) //
    , mAttributes(atr, 0, nullptr) //
    , mEngineInfo(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) {
    static int PlayerUpgrade;
    float max_torque_rpm;

    mWorldID = wuid;
    mContext = context;
    mSimUpdating = 1;
    mHandle = handle;
    mTrailerID = 0;
    mNISCarID = -1;
    mAssetsLoaded = 0;
    mControlSource = Sound::CONTROL_AI;
    mDesiredSpeed = 0.0f;
    mVel1 = mVel0;
    mOversteer = 0.0f;
    mUndersteer = 0.0f;
    mSlipAngle = 0.0f;
    mVisualRPM = 0.0f;
    PSMTX44Identity((Mtx44)&mMatrix);

    Attrib::Gen::pvehicle vehicleinfo(mAttributes);
    Attrib::Gen::engine engine(vehicleinfo.engine(0), 0, nullptr);

    if (mContext == Sound::kRaceContext_QuickRace) {
        int phys_cur_upgrade = mAttributes.engine_current();
        int phys_num_upgrades = mAttributes.engine_upgrades();
        int base_upgrade = 4 - phys_num_upgrades;
        int curupgade_offset = phys_cur_upgrade;

        if (base_upgrade < 0) {
            base_upgrade = 0;
        }
        if (base_upgrade > 4) {
            base_upgrade = 4;
        }
        if (curupgade_offset < 0) {
            curupgade_offset = 0;
        }
        if (curupgade_offset > 4) {
            curupgade_offset = 4;
        }

        PlayerUpgrade = base_upgrade + curupgade_offset;
        if (PlayerUpgrade < 0) {
            PlayerUpgrade = 0;
        }
        if (PlayerUpgrade > 4) {
            PlayerUpgrade = 4;
        }
    }

    mEngineInfo.ChangeWithDefault(GenerateUpgradedEngine(this, PlayerUpgrade));
    mMaxTorque = Physics::Info::MaxTorque(engine, max_torque_rpm);
    mMaxRPM = engine.MAX_RPM();
    mMinRPM = engine.IDLE();
    mRedline = engine.RED_LINE();

    if (mContext == Sound::kRaceContext_Count) {
        mAssetsLoaded = 1;
    } else if (mContext > Sound::kRaceContext_Count) {
        if (mContext <= Sound::CONTEXT_TRAILER && mContext >= Sound::CONTEXT_TRACTOR) {
            mAssetsLoaded = 1;
        }
    }
}

template class UTL::Collections::Listable<EAX_CarState, 10>::List;
template class UTL::Vector<EAX_HeliState *, 16>;

void EAXSound::Update(float t) {
    if (!IsSoundEnabled || t < 0.0f) {
        return;
    }

    if (gnHasStartLoadFEBeenProcessed != 0) {
        gnHasStartLoadFEBeenProcessed--;
        if (gnHasStartLoadFEBeenProcessed < 0) {
            gnHasStartLoadFEBeenProcessed = 0;
        }
        return;
    }

    if (gbHasStartNewGamePlayBeenProcessed != 0) {
        gbHasStartNewGamePlayBeenProcessed = 0;
        return;
    }

    if (DisableSoundUpdate != 0) {
        return;
    }

    if (!bReadCallbackToggle) {
        if (bStreamReadTiming == false) {
            bStreamReadTiming = true;
            bGetTickerDifference(uStreamReadTicks, bGetTicker());
            uStreamReadTicks = bGetTicker();
        }
    } else if (bStreamReadTiming == true) {
        bGetTickerDifference(uStreamReadTicks, bGetTicker());
        uStreamReadTicks = bGetTicker();
        bStreamReadTiming = false;
    }

    if (gbWorldDataBlocksAudioRead == true) {
        if (bStreamBlockState == false) {
            bStreamBlockState = gbWorldDataBlocksAudioRead;
            uStreamBlockTicks = bGetTicker();
        }
    } else if (bStreamBlockState == true) {
        bGetTickerDifference(uStreamBlockTicks, bGetTicker());
        bStreamBlockState = false;
    }

    if (gbAudioInterruptsWorldDataRead == true) {
        if (bAudioInterrupt == false) {
            uAudioInterruptTicks = bGetTicker();
        }
        bAudioInterrupt = gbAudioInterruptsWorldDataRead;
    } else if (bAudioInterrupt == true) {
        bGetTickerDifference(uAudioInterruptTicks);
        bAudioInterrupt = false;
    } else {
        bAudioInterrupt = false;
    }

    if (gb_DORESTART_RACE != 0) {
        if (mEventID != 0) {
            Scheduler::Get().fSchedule_OncePerGameLoop->RemoveTask(mEventID);
        }
        g_ShiftInfo = nullptr;
        g_TurboInfo = nullptr;
        m_pNFSMixMaster->DestroyMap();
        InitializeInGame();
        m_pNFSMixMaster->InitMixMap(0);

        Speech::Module *module = Speech::Manager::GetSpeechModule(1);
        if (module != nullptr) {
            module = Speech::Manager::GetSpeechModule(1);
            module->PurgeSpeech();
        }

        SFXObj_PFEATrax *ppf = static_cast<SFXObj_PFEATrax *>(GetSFXBase_Object(0x40010010));
        if (ppf != nullptr) {
            ppf->RestartRace();
            if (m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
                ppf->m_Flags |= 2;
            } else {
                ppf->m_Flags &= ~2u;
            }
        }

        bMemSet(GameFlowSndState, '\0', 0x3C);
        gb_DORESTART_RACE = 0;
        SetSoundControlState(false, SNDSTATE_PAUSE, "PauseMenu");
    } else {
        if (gb_Is321 != 0) {
            if (g_pNISRevMgr != nullptr) {
                g_pNISRevMgr->Start321Reving();
            }
            gb_Is321 = 0;
        }

        if (GameFlowSndState[13] != 0) {
            Speech::Manager::Deduce();
        }

        gAEMSMgr.Update();
        SndCamera::UpdateCameras();
        m_pEAXSND8Wrapper->Update();
        if (g_pNISRevMgr != nullptr) {
            g_pNISRevMgr->Update(t);
        }

        if (m_pNFSMixMaster->IsMixMapReady() == true) {
            if (m_pSTICH_Playback != nullptr) {
                m_pSTICH_Playback->Update(t);
            }
            Speech::Manager::Update(t);

            if (GetFrontEnd() != nullptr) {
                GetFrontEnd()->Update(nullptr);
            }

            SndBase::m_fRunningTime += t;
            SndBase::m_fDeltaTime = t;
            for (int n = 0; n < 13; n++) {
                CSTATEMGR_Base *mgr = m_pStateMgr[n];
                if (mgr != nullptr) {
                    mgr->UpdateParams(g_EAXIsPaused() ? 0.0f : t);
                }
            }
        }

        if (m_pNFSMixMaster != nullptr) {
            m_pNFSMixMaster->ProcessMixMap(t, SndCamera::GetCurCamState(0));
        }

        for (int n = 0; n < 13; n++) {
            CSTATEMGR_Base *mgr = m_pStateMgr[n];
            if (mgr != nullptr) {
                mgr->ProcessUpdate();
            }
        }

        if (m_pCmnSnd != nullptr) {
            m_pCmnSnd->Update(nullptr);
        }

        m_prevSndGameMode = m_eSndGameMode;
        if (!AreResourceLoadsPending()) {
            UTL::Collections::Listable<CarSoundConn, 10>::ForEach(CarSoundConn::SetAssetsLoaded);
        }
        SNDSYS_service();
    }
}

void EAXSound::CommitAssets() {
    CSTATEMGR_CarState::ResolveCarBanks();
    for (CarSoundConn *const *iter = UTL::Collections::Listable<CarSoundConn, 10>::GetList().begin();
         iter != UTL::Collections::Listable<CarSoundConn, 10>::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;
        if (!pconn->mConnected) {
            pconn->mConnected = true;
        }
    }
}

EAXCar *EAXSound::GetPlayerTunerCar(int nindex) {
    CSTATEMGR_Base *mgr = m_pStateMgr[2];
    if (mgr == nullptr) {
        return nullptr;
    }
    return static_cast<EAXCar *>(mgr->GetStateObj(nindex));
}

CSTATE_Helicopter *EAXSound::SpawnHelicopter(EAX_HeliState *pHeli) {
    if (!IsSoundEnabled) {
        return nullptr;
    }
    CSTATE_Base *newheli = nullptr;
    eMAINMAPSTATES eStateMgrType = eMM_HELICOPTER;
    newheli = m_pStateMgr[eStateMgrType]->GetFreeState(pHeli);
    if (newheli != nullptr) {
        newheli->Attach(pHeli);
        return reinterpret_cast<CSTATE_Helicopter *>(newheli);
    }
    return nullptr;
}

void EAXSound::DestroyEAXHeli(EAX_HeliState *pHeli) {
    CSTATE_Base *newheli;
    eMAINMAPSTATES eStateMgrType;
    if (IsSoundEnabled) {
        eStateMgrType = eMM_HELICOPTER;
        newheli = m_pStateMgr[eStateMgrType]->GetStateObj(pHeli);
        if (newheli != nullptr) {
            newheli->Detach();
        }
    }
}

void EAXSound::DestroyEAXCar(EAX_CarState *pCar) {
    if (!IsSoundEnabled) {
        return;
    }
    CSTATE_Base *attachedcar = nullptr;
    switch (pCar->GetContext()) {
    case GRace::kRaceContext_QuickRace:
        if (m_pStateMgr[eMM_PLAYERCAR] != nullptr) {
            attachedcar = m_pStateMgr[eMM_PLAYERCAR]->GetStateObj(pCar);
        }
        break;
    case GRace::kRaceContext_TimeTrial:
        if (m_pStateMgr[eMM_AIRACECAR] != nullptr) {
            attachedcar = m_pStateMgr[eMM_AIRACECAR]->GetStateObj(pCar);
        }
        break;
    case GRace::kRaceContext_Career:
        if (m_pStateMgr[eMM_COPCAR] != nullptr) {
            attachedcar = m_pStateMgr[eMM_COPCAR]->GetStateObj(pCar);
        }
        break;
    case GRace::kRaceContext_Count:
        if (m_pStateMgr[eMM_TRAFFIC] != nullptr) {
            attachedcar = m_pStateMgr[eMM_TRAFFIC]->GetStateObj(pCar);
        }
        break;
    case 4:
    default:
        if (m_pStateMgr[eMM_TRUCK] != nullptr) {
            attachedcar = m_pStateMgr[eMM_TRUCK]->GetStateObj(pCar);
        }
        break;
    }
    if (attachedcar != nullptr) {
        attachedcar->Detach();
    }
    CSTATEMGR_CarState::DestroyCar(pCar);
}

void EAXSound::LoadFrontEndSoundBanks(void (*callback)(int), int callback_param) {
    if (!IsSoundEnabled) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    bSyncTaskRun();
    gnHasStartLoadFEBeenProcessed = 2;
    m_prevSndGameMode = m_eSndGameMode;
    m_eSndGameMode = SND_FRONTEND;
    gAEMSMgr.ResetBankLoadParams();
    m_pStreamManager = new ("EAXS_StreamManager", false) EAXS_StreamManager();
    m_pStreamManager->InitializeStreams(SNDGM_FRONTEND);

    if ((g_ActiveCtlStates & 0x20000) == 0) {
        g_ActiveCtlStates = 0;
        g_ActiveSFXStates = 0;
        g_PrevActiveCtlStates = 0;
        g_PrevActiveSFXStates = 0;
    }

    InitializeFrontEnd();

    m_pCmnSnd->Initialize();
    m_pFESnd->Initialize();

    m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
    m_pNFSMixMaster->InitMixMap(0);

    gAEMSMgr.m_ExternalLoadCallback = callback;
    gAEMSMgr.m_ExternalLoadCallbackParam = callback_param;

    ReInitMasterVolumes();
    bSyncTaskRun();

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }
}

void EAXSound::UnloadFrontEndSoundBanks() {
    if (!IsSoundEnabled) {
        return;
    }
    int n;

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.0f);
        ServiceQueuedFiles();
    }

    Speech::Manager::Destroy();

    if (GetFrontEnd()) {
        GetFrontEnd()->DestroyAllDriveOnSnds();
    }

    for (n = 0; n < 13; n++) {
        if (m_pStateMgr[n]) {
            m_pStateMgr[n]->ExitWorld();
        }
    }

    delete m_pStreamManager;
    m_pStreamManager = nullptr;

    if (m_pNFSMixMaster) {
        m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (m_pNFSLiveLink) {
        m_pNFSLiveLink->bMonitorChannel = false;
    }

    if (m_pFESnd) {
        delete m_pFESnd;
        m_pFESnd = nullptr;
    }

    if (m_pCmnSnd) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    while (gAEMSMgr.m_nEndOfList != 0) {
        gAEMSMgr.UnloadSndData(0);
    }

    gAEMSMgr.DestroySlots(false);
    gAEMSMgr.ResetBankLoadParams();
    bSyncTaskRun();

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }
}

void g_LoadSndAsset(Attrib::StringKey filename, eSNDDATAPATH path, eSNDDATATYPE datatype) {
    stSndAssetQueue requeststruct;
    requeststruct.Asset.Clear();
    requeststruct.Asset.eDataType = datatype;
    requeststruct.Asset.FileName = filename;
    requeststruct.Asset.DataPath = path;
    requeststruct.pThis = nullptr;
    requeststruct.pCar = nullptr;
    gAEMSMgr.QueueFileLoad(requeststruct, eBANK_SLOT_NONE);
}

void LoadCommonIngameFiles() {
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_FEBanks(1), SNDPATH_GLOBAL, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_EnvBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(5), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(4), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(3), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(1), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_RNBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(Attrib::StringKey("SIREN_MB.abk"), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_StitchBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_StitchBanks(2), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_StitchBanks(1), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(2), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_WNBanks(0), SNDPATH_INGAME, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(6), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(7), SNDPATH_ENGINE, EAXSND_DT_AEMS_ASYNCSPUMEM);

    for (int n = 0; n < 12; n++) {
        g_LoadSndAsset(Attrib::StringKey(csfxedit[n]), SNDPATH_FXEDIT, EAXSND_DT_GENERIC_DATA);
    }
}

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}

void EAXSound::LoadInGameSoundBanks(void (*callback)(int), int callback_param) {
    if (!IsSoundEnabled) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    bIsAnFEToIngameTransition = true;
    m_pStreamManager = new ("EAXS_StreamManager", false) EAXS_StreamManager();

    bool splitScreen = FEDatabase->IsSplitScreenMode() && FEDatabase->iNumPlayers == 2;
    if (splitScreen) {
        m_pStreamManager->InitializeStreams(SNDGM_SPLITSCREEN);
        m_ePlayerMixMode = EAXS3D_TWO_PLAYER_MIX;
    } else {
        m_pStreamManager->InitializeStreams(SNDGM_FREEROAM);
        m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
    }

    bHasDataLoadOccured = true;
    bHasStartNewGameOccured = false;
    m_pCurAudioSettings = &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings;
    m_pEAXSND8Wrapper->ReInit();

    m_prevSndGameMode = m_eSndGameMode;
    m_eSndGameMode = SND_STREETRACE;
    gAEMSMgr.ResetBankLoadParams();
    CSTATEMGR_CarState::ResetCarBanks();
    RefreshLocalAttr();

    if (m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
        IsSpeechEnabled = false;
        Speech::Manager::Init(2);
    } else {
        IsSpeechEnabled = true;
        Speech::Manager::Init(1);
    }

    LoadCommonIngameFiles();

    if ((g_ActiveCtlStates & 0x20000) == 0) {
        g_ActiveCtlStates = 0;
        g_PrevActiveCtlStates = 0;
        g_ActiveSFXStates = 0;
        g_PrevActiveSFXStates = 0;
    }

    gAEMSMgr.m_ExternalLoadCallback = callback;
    gAEMSMgr.m_ExternalLoadCallbackParam = callback_param;

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }
}

void EAXSound::CloseSound() {
    bHasDataLoadOccured = false;

    for (int n = 0; n < 13; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->ExitWorld();
        }
    }

    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.0f);
        ServiceQueuedFiles();
    }

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (g_pNISRevMgr != nullptr) {
        g_pNISRevMgr->CloseNIS();
    }

    bSyncTaskRun();
}

void EAXSound::UnLoadInGameSoundBanks() {
    if (!IsSoundEnabled) {
        return;
    }

    IsSpeechEnabled = true;
    gIsPauseForPause = 0;
    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.0f);
        ServiceQueuedFiles();
    }

    bHasDataLoadOccured = false;
    if (mmsgMRestartRace != nullptr) {
        Hermes::Handler::Destroy(mmsgMRestartRace);
        mmsgMRestartRace = nullptr;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    Speech::Manager::Destroy();
    bSyncTaskRun();

    if (m_pNFSMixMaster != nullptr) {
        m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (m_pStateMgr[0] != nullptr) {
        m_pStateMgr[0]->ExitWorld();
    }

    delete m_pStreamManager;
    m_pStreamManager = nullptr;

    for (int n = 0; n < 12; n++) {
        if (SFXObj_Reverb::m_pFXEditPatch[n] != nullptr) {
            gAudioMemoryManager.FreeMemory(SFXObj_Reverb::m_pFXEditPatch[n]);
        }
        SFXObj_Reverb::m_pFXEditPatch[n] = nullptr;
    }

    if (m_pCmnSnd != nullptr) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    while (gAEMSMgr.m_nEndOfList != 0) {
        gAEMSMgr.UnloadSndData(0);
    }

    gAEMSMgr.DestroySlots(true);
    gAEMSMgr.ResetBankLoadParams();
    bSyncTaskRun();
}

// LoadInGameSoundBanks, etc. go here when implemented

void EAXSound::ReStartRace(bool bIs321) {
    if (!IsSoundEnabled) {
        return;
    }

    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, "RestartRace");

    for (int s = 0; s < 4; s++) {
        if (m_pStreamManager->GetStreamChannel(s) != nullptr) {
            m_pStreamManager->GetStreamChannel(s)->Stop();
            m_pStreamManager->GetStreamChannel(s)->PurgeStream();
            m_pStreamManager->GetStreamChannel(s)->Resume();
        }
    }

    if (m_pCmnSnd != nullptr) {
        delete m_pCmnSnd;
        m_pCmnSnd = nullptr;
    }

    for (int n = 0; n < 13; n++) {
        if (n != 1) {
            if (m_pStateMgr[n] != nullptr) {
                m_pStateMgr[n]->ExitWorld();
            }
        } else {
            m_pStateMgr[1]->DisconnectMixMap();
        }
    }

    gb_DORESTART_RACE = 1;
    gb_Is321 = bIs321;
}

void InitializeSoundDriver() {
    int poolSize = 0x4000;
    if (IsSoundEnabled) {
        poolSize = 0x11F000;
    }

    gAudioMemoryManager.InitMemoryPool(AUD_MAIN_MEM_POOL, poolSize);
    void *mem = gAudioMemoryManager.AllocateMemory(0xBC, "AUD:EAXSound", false);
    g_pEAXSound = new (mem) EAXSound();
    g_pEAXSound->InitializeDriver();
}

void InitializeSoundLoad() {
    g_pEAXSound->InitializeSoundBootLoad();
}

void LoadAemsFrontEnd(void (*callback)(int), int callback_param) {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->LoadFrontEndSoundBanks(callback, callback_param);
    } else {
        callback(callback_param);
    }
}

void UnloadAemsFrontEnd() {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->UnloadFrontEndSoundBanks();
    }
}

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause) {}

void FESoundControl(bool bOn, const char *name) {
    if (g_pEAXSound == nullptr) {
        return;
    }

    unsigned int key = Attrib::StringHash32(name);
    Attrib::StringKey FengList[37] = {
        "Pause_Main.fng",
        "Pause_Options.fng",
        "Pause_Controller.fng",
        "InGamePhotoMaster.fng",
        "EA_Trax.fng",
        "FadeScreen.fng",
        "SMS_Message.fng",
        "BUSTED_OVERLAY.fng",
        "SixDaysLater.fng",
        "InGame_MC_Main_GC.fng",
        "InGameAnyMovie.fng",
        "Pause_Performance_Tuning.fng",
        "InGame_MC_Main.fng",
        "InGameDialog.fng",
        "WS_InGameAnyMovie.fng",
        "InGameAnyMovie.fng",
        "InGameAnyTutorial.fng",
        "FEAnyMovie.fng",
        "WS_FEAnyMovie.fng",
        "FEAnyTutorial.fng",
        "InGameAnyTutorial.fng",
        "LS_EALogo.fng",
        "LS_EA_hidef.fng",
        "LS_PSA.fng",
        "MW_LS_IntroFMV.fng",
        "MW_LS_AttractFMV.fng",
        "WS_LS_EALogo.fng",
        "WS_LS_EA_hidef.fng",
        "WS_LS_PSA.fng",
        "WS_LS_IntroFMV.fng",
        "WS_MW_LS_AttractFMV.fng",
        "PostRace_MilestoneRewards.fng",
        "PostRace_Pursuit.fng",
        "Game_StartRace",
        "InGameMilestones.fng",
        "InGameBounty.fng",
        "InGameRaceSheet.fng",
    };

    bStringHash(name);
    int index = -1;
    int namehash = 0;
    do {
        if (key == FengList[namehash].GetHash32()) {
            index = namehash;
            break;
        }
        int n = namehash + 1;
        namehash = n;
    } while (namehash < 37);

    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        goto FRONTEND_FMV_STATE;
    }

    switch (index) {
        case 0:
        case 1:
        case 2:
        case 11:
            g_pEAXSound->m_bPause_MainFNG = bOn;
            SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
            return;
        case 3:
            SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
            return;
        case 4:
        case 5:
        case 6:
        case 7:
            return;
        case 8:
            SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
            SetSoundControlState(bOn, SNDSTATE_OFF, name);
            return;
        case 9:
        case 12:
            if (g_pEAXSound->m_bPause_MainFNG) {
                return;
            }
            SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
            return;
        case 10:
            goto FMV_STATE;
        case 13:
            return;
        case 16:
        case 20:
            SetSoundControlState(bOn, SNDSTATE_FMV, name);
            goto STOP_MUSIC;
        case 31:
        case 32:
            goto FE_SMS_MESSAGE;
        case 33:
            goto STOP_MUSIC;
        case 34:
        case 35:
        case 36:
            goto PURGE_SPEECH;
        case 14:
        case 15:
        case 17:
        case 18:
        case 19:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            break;
        default:
            goto FE_UPSCREEN;
    }

FMV_STATE: if (bOn) {
        SetSoundControlState(bOn, SNDSTATE_FMV, name);
        return;
    }
    SetSoundControlState(false, SNDSTATE_FMV, name);
    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, name);
    return;

FE_SMS_MESSAGE: SetSoundControlState(bOn, SNDSTATE_FE_SMS_MESSAGE, name);
    return;

STOP_MUSIC: SetSoundControlState(bOn, SNDSTATE_STOP_MUSIC, name);
    return;

PURGE_SPEECH: if (Speech::Manager::GetSpeechModule(1) != nullptr) {
        Speech::Manager::GetSpeechModule(1)->PurgeSpeech();
    }
    goto FE_UPSCREEN;

FE_UPSCREEN: SetSoundControlState(bOn, SNDSTATE_FE_UPSCREEN, name);
    return;

FRONTEND_FMV_STATE: if (index == 10 || index >= 14) {
        SetSoundControlState(bOn, SNDSTATE_FMV, "name");
    }
}

void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason) {
    if (_RUN_SOUND_STATE == 0 || g_pEAXSound == nullptr || !IsSoundEnabled) {
        return;
    }

    if (g_pEAXSound->m_bPause_MainFNG) {
        if (!bON) {
            if (esndstate != SNDSTATE_MINILOAD) {
                if (esndstate <= SNDSTATE_FE_SMS_MESSAGE) {
                    if (esndstate > SNDSTATE_NIS_ARREST) {
                        return;
                    }
                }
            }
        }
    }

    if ((g_ActiveCtlStates & (1 << esndstate)) != 0) {
        if (bON) {
            return;
        }
    } else if (!bON) {
        return;
    }

    g_PrevActiveCtlStates = g_ActiveCtlStates;
    g_PrevActiveSFXStates = g_ActiveSFXStates;

    if (!bON) {
        g_ActiveCtlStates &= ~(1 << esndstate);
    } else {
        g_ActiveCtlStates |= 1 << esndstate;
    }

    g_ActiveSFXStates = 0;
    for (int n = 0; n < 18; n++) {
        if ((g_ActiveCtlStates & (1 << n)) != 0) {
            g_ActiveSFXStates |= g_CtlStateActions[n];
        }
    }

    for (int state = 0; state < 13; state++) {
        if ((g_ActiveSFXStates & (1 << state)) != (g_PrevActiveSFXStates & (1 << state)) &&
            g_pEAXSound->GetStreamManager()) {
            if ((g_ActiveSFXStates & (1 << state)) != 0) {
                switch (state) {
                    case 0:
                        if (g_pEAXSound->GetStreamManager()->GetStreamChannel(1)) {
                            g_pEAXSound->GetStreamManager()->GetStreamChannel(1)->Pause();
                        }
                        break;
                    case 1:
                        if (g_pEAXSound->GetStreamManager()->GetStreamChannel(0)) {
                            g_pEAXSound->GetStreamManager()->GetStreamChannel(0)->Pause();
                        }
                        break;
                    case 2:
                        if (g_pEAXSound->GetStreamManager()->GetStreamChannel(2)) {
                            g_pEAXSound->GetStreamManager()->GetStreamChannel(2)->Pause();
                        }
                        break;
                }
            } else {
                switch (state) {
                    case 0:
                        if (g_pEAXSound->GetStreamManager()->GetStreamChannel(1)) {
                            g_pEAXSound->GetStreamManager()->GetStreamChannel(1)->Resume();
                        }
                        break;
                    case 1:
                        if (g_pEAXSound->GetStreamManager()->GetStreamChannel(0)) {
                            g_pEAXSound->GetStreamManager()->GetStreamChannel(0)->Resume();
                        }
                        break;
                    case 2:
                        if (g_pEAXSound->GetStreamManager()->GetStreamChannel(2)) {
                            g_pEAXSound->GetStreamManager()->GetStreamChannel(2)->Resume();
                        }
                        break;
                }
            }
        }
    }

    if (esndstate == SNDSTATE_ERROR) {
        g_pEAXSound->Update(0.0f);
    }
}

void LoadAemsInGame(void (*callback)(int), int callback_param) {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->LoadInGameSoundBanks(callback, callback_param);
    } else {
        callback(callback_param);
    }
}

void UnloadAemsInGame() {
    if (IsSoundEnabled == 1) {
        g_pEAXSound->UnLoadInGameSoundBanks();
    }
}

void CloseSound() {
    if (IsSoundEnabled) {
        g_pEAXSound->CloseSound();
    }
}

eSndAudioMode EAXSound::GetDefaultPlatformAudioMode() {
    return m_pEAXSND8Wrapper->GetDefaultPlatformAudioMode();
}

eSndAudioMode EAXSound::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    return m_pEAXSND8Wrapper->SetAudioModeFromMemoryCard(mode);
}
