#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMaster.hpp"
#include "Speed/Indep/Src/EAXSound/EAXFrontEnd.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Reverb.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_AICar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_CopCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Main.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Music.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_PlayerCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_TrafficCar.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Truck.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/SoundConn.h"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Gameplay/GRace.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audiosystem_hash.h"
#include "Speed/Indep/Src/Generated/Events/EAudioWorldTest.hpp"
#include "Speed/Indep/Src/Main/Scheduler.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

#define USE_DYNAMIC_MIXER // Decl: 176

bool gbAudioInterruptsWorldDataRead; // Decl: 191
bool gbWorldDataBlocksAudioRead;     // Decl: 192
bool bReadCallbackToggle;            // Decl: 193

bool gbHasStartNewGamePlayBeenProcessed = false; // Decl: 195
int gnHasStartLoadFEBeenProcessed = 0;           // Decl: 196

static const int DoEAXSndPrint = 0; // Decl: 206

const Attrib::StringKey crcEngineAudio("EngineAudio");     // Decl: 212
const Attrib::StringKey crcAudioSystem("AudioSystem");     // Decl: 213
const Attrib::StringKey crcMostWanted("MostWanted");       // Decl: 214
const Attrib::StringKey crcDrivetrain("Drivetrain");       // Decl: 215
const Attrib::StringKey crcSkidParams("SkidParams");       // Decl: 216
const Attrib::StringKey crcTurboSFX("TurboSFX");           // Decl: 217
const Attrib::StringKey crcAccelTrans("AccelTrans");       // Decl: 218
const Attrib::StringKey crcShiftPattern("ShiftPattern");   // Decl: 219
const Attrib::StringKey crcSweetener("Sweetener");         // Decl: 220
const Attrib::StringKey crcEnglish("English");             // Decl: 221
const Attrib::StringKey crcCarHitWall("CarHitWall");       // Decl: 222
const Attrib::StringKey crcLicensedMusic("LicensedMusic"); // Decl: 223
const Attrib::StringKey crcMusic("Music");                 // Decl: 224

char prevSoundFile[50]; // Decl: 229

int gIsPauseForPause = 0; // Decl: 231

EAXSound *g_pEAXSound = nullptr; // Decl: 233

int g_PlayerID[2]; // Decl: 237

bool debug_memory_holes = false; // Decl: 239
int DEBUG_SNDCTRLSTATES = 0;     // Decl: 240

char *g_pcsCSISAllocString = "CSISSOUND"; // Decl: 246

void *NullPointer = nullptr; // Decl: 249

float g_fMasterSFXVolume = 1.0f; // Decl: 263
int g_iMasterSFXVolume = 0x7FFF; // Decl: 264

static int gLastFETRAXmask = 0; // Decl: 270

static int gLastIGTRAXmask = 0; // Decl: 271

extern char *szMixMapFiles[4]; // Decl: 294

// Decl: 305
#define CASE_STRING_RETURN(text)                                                                                                                     \
    case text:                                                                                                                                       \
        return #text;

static const float gfEngineBiasBase = 0.5f; // Decl: 312
static const float gfEnviroBiasBase = 0.5f; // Decl: 313

float g_fPS2DemoVolume = 0.99999899f; // Decl: 315

int UsedSPURam = 0;  // Decl: 317
int UsedMainRam = 0; // Decl: 318

ASYNCFILE_CALLBACK *SoundRegionsLoadedCallback = nullptr;
int32 SoundRegionsLoadedCallbackParam = 0;  // Decl: 324
bool bSoundRegionLoadingInProgress = false; // Decl: 325
int StartLoadingSoundRegionsTicks = 0;      // Decl: 326

static const int EAX_tc_exhupgrade[4] = {0, 0, 0, 0}; // Decl: 337

static const int EAX_stereoupgrade = 0; // Decl: 339

unsigned int SoundRandomSeed = 0; // Decl: 341

bool gb_DORESTART_RACE = false; // Decl: 343
bool gb_Is321 = false;          // Decl: 344

uint32 g_ActiveCtlStates = 0;
uint32 g_PrevActiveCtlStates = 0;
uint32 g_ActiveSFXStates = 0;
uint32 g_PrevActiveSFXStates = 0;
uint32 g_CtlStateActions[18] = {
    0xFFFFFFFF, 0x3F, 0x0, 0x1C, 0xBE, 0xBC, 0x479, 0x121, 0x200, 0x33, 0x838, 0x7F, 0x1, 0x7F, 0x38, 0x1000, 0xFFFFFFFF, 0xFFFFFFFF,
};

bool g_EAXIsPaused() {
    return (g_ActiveCtlStates & 0x3483b) != 0;
}

// STRIPPED
void g_AdjustVolPause(int &Volume) {}

// TODO look at dwarf
// Decl: 393
void EAXSound::START_321Countdown() {
    if (IsSoundEnabled == 0 || IsAudioStreamingEnabled == 0) {
        return;
    }

    int id = 0x40010010;
    // TODO magic
    SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(GetSFXBase_Object(id));
    if (ppf != nullptr) {
        ppf->Set321(true);
    }
    id = 0x40000050;
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(GetSFXBase_Object(id));
    if (pnis != nullptr) {
        pnis->StartNIS();
    }
}

bool EAXSound::AreResourceLoadsPending() {
    return gAEMSMgr.AreResourceLoadsPending();
}

extern unsigned int g_laststartanimid; // TODO move?
extern bool g_bWasLastNISaStart;       // TODO move?

void EAXSound::QueueNISButtonThrough(unsigned int anim_id, int camera_track_number) {
    if (IsSoundEnabled == 0 || IsAudioStreamingEnabled == 0) {
        return;
    }

    int id = 0x40000050;
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(this->GetSFXBase_Object(id));
    if (camera_track_number == -1) {
        // TODO magic
        if (g_laststartanimid != 0x0D0E5A9D && g_laststartanimid != 0xCBFF6594) {
            pnis->QueueNISStream(g_laststartanimid, -1, true, false);
        }
    } else if (g_bWasLastNISaStart) {
        pnis->QueueNISStream(anim_id, camera_track_number, true, false);
    } else {
        pnis->StopStream();
    }

    Speech::Manager::ClearPlayback();
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(COPSPEECH_MODULE);
    if (cop_speech != nullptr) {
        cop_speech->ReleaseResource();
    }
}

void EAXSound::QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int)) {
    if (IsSoundEnabled == 0 || IsAudioStreamingEnabled == 0) {
        return;
    }

    int id = 0x40000050;
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(this->GetSFXBase_Object(id));
    pnis->QueueNISStream(anim_id, camera_track_number, setmstimecb, false);
}

bool EAXSound::IsNISStreamQueued() {
    if (IsSoundEnabled == 0 || IsAudioStreamingEnabled == 0) {
        return true;
    }

    int id = 0x40000050;
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(this->GetSFXBase_Object(id));
    return pnis->IsNISStreamReady();
}

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause);

void EAXSound::NISFinished() {
    if (IsSoundEnabled == 0 || IsAudioStreamingEnabled == 0) {
        return;
    }
    int id = 0x40000050;
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(this->GetSFXBase_Object(id));
    pnis->NISActivityDone();
    SoundPause(false, eSNDPAUSE_NISON);
    SetSoundControlState(false, SNDSTATE_NIS_STORY, "EAXSound::NISFinished");
    SetSoundControlState(false, SNDSTATE_NIS_INTRO, "EAXSound::NISFinished");
    SetSoundControlState(false, SNDSTATE_NIS_BLK, "EAXSound::NISFinished");
    SetSoundControlState(false, SNDSTATE_NIS_ARREST, "EAXSound::NISFinished");
}

void EAXSound::PlayNIS() {
    if (IsSoundEnabled == 0 || IsAudioStreamingEnabled == 0) {
        return;
    }

    int id = 0x40000050;
    SFXObj_NISStream *pnis = static_cast<SFXObj_NISStream *>(this->GetSFXBase_Object(id));
    pnis->StartNIS();
    SoundPause(true, eSNDPAUSE_NISON);
}

void EAXSound::PlayUISoundFX(eMenuSoundTriggers etriggertype) {
    if (IsSoundEnabled == 0) {
        return;
    }
    if (etriggertype < UISND_COMMON_MAX_NUM) {
        if (this->m_pCmnSnd != nullptr) {
            this->m_pCmnSnd->Play(etriggertype);
        }
    } else if (etriggertype < UISND_FRONTEND_MAX_NUM) {
        etriggertype = static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM);
        if (this->m_pFESnd != nullptr) {
            this->m_pFESnd->Play(etriggertype);
        }
    }
}

void EAXSound::StopUISoundFX(eMenuSoundTriggers etriggertype) {
    if (IsSoundEnabled == 0) {
        return;
    }
    if (etriggertype < UISND_COMMON_MAX_NUM) {
        if (this->m_pCmnSnd != nullptr) {
            this->m_pCmnSnd->Stop(etriggertype);
        }
    } else {
        etriggertype = static_cast<eMenuSoundTriggers>(etriggertype - UISND_COMMON_MAX_NUM);
        if (this->m_pFESnd != nullptr) {
            this->m_pFESnd->Stop(etriggertype);
        }
    }
}

char csCSISdebug[32]; // Decl: 680

void EAXSound::SetCsisName(SndBase *psndbase) {
    int ninst = psndbase->GetUniqueID();
    bSPrintf(csCSISdebug, " %s, 0x%x ", psndbase->GetTypeName(), ninst);
    this->SetCsisName(csCSISdebug);
}

void EAXSound::SetCsisName(char *pcsAllocName) {
    g_pcsCSISAllocString = pcsAllocName;
}

EAXSound::EAXSound() {
    this->m_pcsCsisName = "SOUND";
    this->bPlayCarSounds = true;
    this->mAttributes = nullptr;
    this->mLocalAttr = nullptr;
    this->m_pEAXSND8Wrapper = nullptr;
    this->m_bAudioIsPaused = false;
    this->m_X360_UI_Override = false;
    this->m_eSndGameMode = SND_MODE_NONE;
    this->m_prevSndGameMode = SND_MODE_NONE;
    this->m_pStreamManager = nullptr;
    this->m_nStereoUpgradeLevel = 0;
    this->m_bIsPaused = false;
    this->m_pCurAudioSettings = nullptr;
    this->m_nSpeechLoadBankIndex = 0;
    this->m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
    this->m_pNewSongInfoSt = nullptr;
    this->EngineLoadingBlocked = false;
    this->FrameCnt = 0;
    this->m_bIsSpecialUGMovie = false;
    this->m_pCmnSnd = nullptr;
    this->m_pFESnd = nullptr;
    this->m_pSTICH_Playback = nullptr;
    this->m_pNFSLiveLink = nullptr;
    this->m_pNFSMixMaster = nullptr;
    this->bPlayCameraSnapShot = false;
    this->mmsgMRestartRace = nullptr;
    this->m_bPause_MainFNG = false;
    this->mEventID = 0;
    this->mData.fEventID = EAudioWorldTest::kEventID;
    int nloop = 0;
}

EAXSound::~EAXSound() {
    gSpeechCache.Dump();

    if (this->mAttributes != nullptr) {
        delete this->mAttributes;
        this->mAttributes = nullptr;
    }

    if (this->mLocalAttr != nullptr) {
        delete this->mLocalAttr;
        this->mLocalAttr = nullptr;
    }

    if (this->m_pCmnSnd != nullptr) {
        delete this->m_pCmnSnd;
        this->m_pCmnSnd = nullptr;
    }

    if (this->m_pFESnd != nullptr) {
        delete this->m_pFESnd;
        this->m_pFESnd = nullptr;
    }

    if (this->m_pEAXSND8Wrapper != nullptr) {
        delete this->m_pEAXSND8Wrapper;
        this->m_pEAXSND8Wrapper = nullptr;
    }

    delete g_pNISRevMgr;
}

extern int g_DMIX_DummyInputBlock[16];

int *EAXSound::GetPointerCallback(int nid) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs != nullptr) {
        return pbs->GetOutputBlockPtr() != nullptr ? pbs->GetOutputBlockPtr() : nullptr;
    }
    return g_DMIX_DummyInputBlock;
}

void EAXSound::SetSFXOutCallback(int nid, int *ptr) {
    SndBase *pbs = GetSndBase_Object(nid);
    if (pbs != nullptr) {
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
    if (pbs != nullptr) {
        pbs->SetInputsPtr(ptr);
        if (pbs->GetStateBase() != nullptr) {
            return pbs->GetStateBase()->IsAttached();
        }
    }
    return false;
}

int EAXSound::GetStateRefCount(int nstate) {
    if (m_pStateMgr[nstate] == nullptr) {
        return 0;
    }
    return m_pStateMgr[nstate]->GetStateObjCount();
}

int DEBUGPRINT_MIXERCONNECT = 0; // size: 0x4, Decl: 861

void EAXSound::SetSFXBaseObject(SFX_Base *psb, eMAINMAPSTATES estate, int ntype, int instance) {
    switch (estate) {
        case eMM_MAIN:
            switch (ntype) {
                case SFXOBJ_MUSIC:
                    break;
                case SFXOBJ_SPEECH:
                    Speech::Manager::AttachSFXOBJ(COPSPEECH_MODULE, psb, SFXOBJ_SPEECH);
                    break;
                case SFXOBJ_MOVIES:
                    break;
                case SFXOBJ_NISPROJ_STRMS:
                case SFXOBJ_MOMENT_STRMS:
                    Speech::Manager::AttachSFXOBJ(NISSFX_MODULE, psb, static_cast<eSFXOBJ_MAIN_TYPES>(ntype));
                    break;
                case SFXOBJ_FEHUD:
                    if (this->m_pFESnd != nullptr) {
                        this->m_pFESnd->AttachSFXOBJ(psb, SFXOBJ_FEHUD);
                    }
                    if (this->m_pCmnSnd != nullptr) {
                        this->m_pCmnSnd->AttachSFXOBJ(psb, SFXOBJ_FEHUD);
                    }
                    break;
            }
            break;
        case eMM_PLAYERCAR:
        case eMM_AIRACECAR:
        case eMM_TRAFFIC:
        case eMM_ENVIRONMENT:
            break;
        case eMM_COLLISION:
            Speech::Manager::AttachSFXOBJ(NISSFX_MODULE, psb, static_cast<eSFXOBJ_MAIN_TYPES>(ntype));
            break;
        default:
            break;
    }
}

// STRIPPED
void EAXSound::SetSndBaseObject(SndBase *psb, eMAINMAPSTATES estate, int ntype, int instance) {}

SFX_Base *EAXSound::GetSFXBase_Object(int nID) {
    SFX_Base *ReturnObj = nullptr;
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    if (m_pStateMgr[nState] != nullptr) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        if (m_pStateMgr[nState]->GetStateObj(nInstanceID) == nullptr) {
            return nullptr;
        }
        if ((nID & MASK_OBJID) == 0x40000000) {
            ReturnObj = static_cast<SFX_Base *>(m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number));
        }
    }
    return ReturnObj;
}

SndBase *EAXSound::GetSndBase_Object(int nID) {
    SndBase *ReturnObj = nullptr;
    int nState = (static_cast<unsigned int>(nID) >> 16) & 0xFF;
    if (m_pStateMgr[nState] != nullptr) {
        int nInstanceID = (static_cast<unsigned int>(nID) >> 11) & 0x1F;
        int SFXID_Number = (static_cast<unsigned int>(nID) >> 4) & 0x7F;
        if (m_pStateMgr[nState]->GetStateObj(nInstanceID) == nullptr) {
            return nullptr;
        }
        if ((nID & MASK_OBJID) == 0x40000000) {
            ReturnObj = m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXObject(SFXID_Number);
        }
        if ((nID & MASK_OBJID) == 0x60000000) {
            ReturnObj = m_pStateMgr[nState]->GetStateObj(nInstanceID)->GetSFXCTLObject(SFXID_Number);
        }
    }
    if (ReturnObj == nullptr) {
        return nullptr;
    }
    return ReturnObj;
}

// TODO after we merge FE
// float EAXSound::GetCurMusicVolume() {
//     if (this->m_eSndGameMode == SND_FRONTEND) {
//         return this->m_pCurAudioSettings->GetMasteredFEMusicVol();
//     }
//     return this->m_pCurAudioSettings->GetMasteredIGMusicVol();
// }

void EAXSound::ReInitMasterVolumes() {
    int i;
}

float g_SliderValue = -1.0f; // Decl: 1275

void EAXSound::UpdateVolumes(AudioSettings *paudiosettings, float NewValue) {
    this->m_pCurAudioSettings = paudiosettings;
    this->ReInitMasterVolumes();
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

// STRIPPED
void EAXSound::RestoreDriver() {}

void EAXSound::UpdateSongInfo() {}

void EAXSound::InitializeDriver() {
    if (IsSoundEnabled == 0) {
        return;
    }

    this->m_pEAXSND8Wrapper = new ("EAXSND8Wrapper") EAXSND8Wrapper();
    if (!this->m_pEAXSND8Wrapper->Initialize()) {
        IsSoundEnabled = 0;
        return;
    }

    gAEMSMgr.InitSPUram();

    this->m_pNFSMixMaster = new ("NFSMixMaster") NFSMixMaster();
    this->m_pSTICH_Playback = new ("STICH_PlayBack") cSTICH_PlayBack();
    new ("NISRevMan") NIS_RevManager();

    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        this->m_pStateMgr[n] = nullptr;
    }

    this->m_pStateMgr[eMM_MAIN] = new ("SND: CSTATEMGR_MAIN") CSTATEMGR_Main();
    this->m_pStateMgr[eMM_MAIN]->Initialize(eMM_MAIN);

    this->m_pStateMgr[eMM_MUSIC] = new ("SND: CSTATEMGR_Music") CSTATEMGR_Music();
    this->m_pStateMgr[eMM_MUSIC]->Initialize(eMM_MUSIC);

    this->m_pStateMgr[eMM_PLAYERCAR] = new ("SND: CSTATEMGR_PlyrCar") CSTATEMGR_PlayerCar();
    this->m_pStateMgr[eMM_PLAYERCAR]->Initialize(eMM_PLAYERCAR);

    this->m_pStateMgr[eMM_AIRACECAR] = new ("SND: CSTATEMGR_AICar") CSTATEMGR_AICar();
    this->m_pStateMgr[eMM_AIRACECAR]->Initialize(eMM_AIRACECAR);

    this->m_pStateMgr[eMM_COPCAR] = new ("SND: CSTATEMGR_CarState") CSTATEMGR_CopCar();
    this->m_pStateMgr[eMM_COPCAR]->Initialize(eMM_COPCAR);

    this->m_pStateMgr[eMM_TRAFFIC] = new ("SND: CSTATEMGR_TrafficCar") CSTATEMGR_TrafficCar();
    this->m_pStateMgr[eMM_TRAFFIC]->Initialize(eMM_TRAFFIC);

    this->m_pStateMgr[eMM_ENVIRONMENT] = new ("SND: CSTATEMGR_ENVIRO") CSTATEMGR_Enviro();
    this->m_pStateMgr[eMM_ENVIRONMENT]->Initialize(eMM_ENVIRONMENT);

    this->m_pStateMgr[eMM_COLLISION] = new ("SND: CSTATEMGR_Collision") CSTATEMGR_Collision();
    this->m_pStateMgr[eMM_COLLISION]->Initialize(eMM_COLLISION);

    this->m_pStateMgr[eMM_DRIVEBY] = new ("SND: CSTATEMGR_DriveBy") CSTATEMGR_DriveBy();
    this->m_pStateMgr[eMM_DRIVEBY]->Initialize(eMM_DRIVEBY);

    this->m_pStateMgr[eMM_HELICOPTER] = new ("SND: CSTATEMGR_Helicopter") CSTATEMGR_Helicopter();
    this->m_pStateMgr[eMM_HELICOPTER]->Initialize(eMM_HELICOPTER);

    this->m_pStateMgr[eMM_TRUCK] = new ("SND: CSTATEMGR_Truck") CSTATEMGR_Truck();
    this->m_pStateMgr[eMM_TRUCK]->Initialize(eMM_TRUCK);

    this->m_pNFSMixMaster->AssignSFXCallbacks(EAXSound::GetPointerCallback, EAXSound::SetSFXOutCallback, EAXSound::SetSFXInputCallback,
                                              EAXSound::GetStateRefCount, EAXSound::MixMapReadyCallback);
}

void EAXSound::RefreshLocalAttr() {
    if (this->mLocalAttr != nullptr) {
        delete this->mLocalAttr;
        this->mLocalAttr = nullptr;
    }

    extern int SkipFE;
    extern eLanguages SkipFELanguage;

    switch (SkipFE == 0 ? GetCurrentLanguage() : SkipFELanguage) {
        case eLANGUAGE_FRENCH:
            this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(1).GetCollectionWithDefault(), 0, nullptr);
            break;
        case eLANGUAGE_GERMAN:
            this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(2).GetCollectionWithDefault(), 0, nullptr);
            break;
        case eLANGUAGE_ITALIAN:
            this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(3).GetCollectionWithDefault(), 0, nullptr);
            break;
        case eLANGUAGE_SPANISH:
            this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(4).GetCollectionWithDefault(), 0, nullptr);
            break;
        case eLANGUAGE_JAPANESE:
            this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(5).GetCollectionWithDefault(), 0, nullptr);
            break;
        case eLANGUAGE_ENGLISH:
        default:
            this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(0).GetCollectionWithDefault(), 0, nullptr);
            break;
    }
}

void EAXSound::InitializeSoundBootLoad() {
    if (!IsSoundEnabled) {
        return;
    }

    this->mAttributes = new Attrib::Gen::audiosystem(Attrib::Hash::audiosystem::key_mostwanted, 0, nullptr);
    this->RefreshLocalAttr();
    gAEMSMgr.Init();
    gSpeechCache.Init(SPEECH_CACHE_SIZE);
    CSTATEMGR_Base::ClearClassLists();

    void RegisterStates();
    void RegisterSFX();

    RegisterStates();
    RegisterSFX();
}

bool bIsAnFEToIngameTransition = false; // Decl: 1505
bool bHasStartNewGameOccured = false;   // Decl: 1506
bool bIsMapInQueuedFileLoad = false;    // Decl: 1507

void EAXSound::StartNewGamePlay() {
    if (IsSoundEnabled == 0) {
        return;
    }

    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, "RestartRace");

    if (bHasStartNewGameOccured) {
        if (this->m_pCmnSnd != nullptr) {
            delete this->m_pCmnSnd;
            this->m_pCmnSnd = nullptr;
        }

        for (int s = 0; s < 4; s++) {
            if (this->m_pStreamManager->GetStreamChannel(s) != nullptr && s != 1) {
                this->m_pStreamManager->GetStreamChannel(s)->Stop();
                this->m_pStreamManager->GetStreamChannel(s)->PurgeStream();
            }
        }

        for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
            if (n != eMM_MUSIC) {
                if (m_pStateMgr[n] != nullptr) {
                    m_pStateMgr[n]->ExitWorld();
                }
            } else {
                m_pStateMgr[n]->DisconnectMixMap();
            }
        }

        if (this->m_pNFSMixMaster->IsMixMapReady()) {
            this->m_pNFSMixMaster->DestroyMainMainMap();
        }

        gAEMSMgr.DestroySlots(false);
    }

    if (this->m_ePlayerMixMode != EAXS3D_TWO_PLAYER_MIX) {
        EAXAemsManager::QueueSlots(eBANK_SLOT_PATHFINDER, 1);
    }

    CSTATEMGR_AICar::QueueSlots();
    gAEMSMgr.InitializeSlots(!bHasStartNewGameOccured);
    bHasStartNewGameOccured = true;
    EAXCar::g_TurboInfo = nullptr;
    EAXCar::g_ShiftInfo = nullptr;

    if (!bIsMapInQueuedFileLoad) {
        bIsMapInQueuedFileLoad = true;

        GRaceParameters *race = nullptr;
        if (GRaceStatus::Exists()) {
            race = GRaceStatus::Get().GetRaceParameters();
        } else {
            race = GRaceDatabase::Get().GetStartupRace();
        }

        if (race != nullptr) {
            GRace::Type rt = race->GetRaceType();
            if (rt == GRace::kRaceType_Drag) {
                this->SetSndGameMode(SND_DRAGRACE);
                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    this->m_pNFSMixMaster->CreateMainMainMap(eRACE_TWODRG);
                } else {
                    this->m_pNFSMixMaster->CreateMainMainMap(eRACE_DRAG);
                }
            } else {
                int id = 0x40010010;
                SFXObj_PFEATrax *ppf = static_cast<SFXObj_PFEATrax *>(this->GetSFXBase_Object(id));
                if (ppf != nullptr) {
                    ppf->RestartRace();
                }
                if (rt == GRace::kRaceType_Challenge) {
                    this->SetSndGameMode(SND_CHALLENGERACE);
                } else {
                    this->SetSndGameMode(SND_STREETRACE);
                }
                if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                    this->m_pNFSMixMaster->CreateMainMainMap(eRACE_TWOCIRC);
                } else {
                    this->m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
                }
            }
        } else {
            this->SetSndGameMode(SND_FREEROAM);
            if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                this->m_pNFSMixMaster->CreateMainMainMap(eRACE_TWOCIRC);
            } else {
                this->m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
            }
        }
    }

    this->InitializeInGame();
    this->m_pNFSMixMaster->InitMixMap(0);

    int id = 0x40010010;
    SFXObj_Pathfinder *ppf = static_cast<SFXObj_Pathfinder *>(this->GetSFXBase_Object(id));
    if (ppf != nullptr) {
        if (this->m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
            ppf->SetSplitScreen(true);
        } else {
            ppf->SetSplitScreen(false);
        }
    }

    gbHasStartNewGamePlayBeenProcessed = true;
}

void EAXSound::InitializeInGame() {
    if (IsSoundEnabled == 0) {
        return;
    }

    int nstate = -1;
    this->m_pCmnSnd = new ("AUD:EAXCommon", 0) EAXCommon();
    this->m_pCmnSnd->Initialize();

    SoundRandomSeed = bRandom(-1);
    if (!bIsAnFEToIngameTransition) {
        nstate = 1;
    }
    bIsAnFEToIngameTransition = false;

    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (n != nstate && m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->EnterWorld(this->m_eSndGameMode);
        }
    }

    this->mEventID = Scheduler::Get().fSchedule_OncePerGameLoop->AddTask(EAudioWorldTest::kEventID, &this->mData, 6, true, 0, 0);
}

void EAXSound::InitializeFrontEnd() {
    if (!IsSoundEnabled) {
        return;
    }

    if (this->mAttributes == nullptr) {
        this->mAttributes = new Attrib::Gen::audiosystem(Attrib::Hash::audiosystem::key_mostwanted, 0, nullptr);
    }

    if (this->mLocalAttr == nullptr) {
        this->mLocalAttr = new Attrib::Gen::audiosystem(this->mAttributes->Locales(0).GetCollectionWithDefault(), 0, nullptr);
    }

    this->m_pCmnSnd = new ("AUD:EAXCommon", 0) EAXCommon();
    this->m_pFESnd = new ("AUD:EAXFrontEnd", 0) EAXFrontEnd();
    this->RefreshLocalAttr();
    Speech::Manager::Init(SPEECH_FRONTEND_MODE);
    SoundRandomSeed = bRandom(-1);
    this->m_pCurAudioSettings = &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings; // TODO inline

    if (TheDemoDiscManager.IsActive()) {
        TheDemoDiscManager.GetMasterVolumeScale();
    }

    gAEMSMgr.QueueSlots(eBANK_SLOT_PATHFINDER, 1);
    gAEMSMgr.InitializeSlots(true);

    if (m_pStateMgr[eMM_MAIN] != nullptr) {
        m_pStateMgr[eMM_MAIN]->EnterWorld(this->m_eSndGameMode);
    }
    if (m_pStateMgr[eMM_MUSIC] != nullptr) {
        m_pStateMgr[eMM_MUSIC]->EnterWorld(this->m_eSndGameMode);
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
    for (int n = 0; n < EAX_CarState::Count(); n++) {
        if (EAX_CarState::GetList()[n]->GetContext() != Sound::CONTEXT_PLAYER) {
            continue;
        }
        num_attached++;
        CSTATE_Base *newcar = m_pStateMgr[eMM_PLAYERCAR]->GetFreeState(EAX_CarState::GetList()[n]);
        if (newcar == nullptr) {
            continue;
        }

        newcar->Attach(EAX_CarState::GetList()[n]);
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN && num_attached == 2) {
            break;
        }
        if (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN && num_attached == 1) {
            break;
        }
    }
}

// STRIPPED
void EAXSound::Destroy() {}

void EAXSound::InitEATRAX() {
    this->UpdateSongInfo();
}

// STRIPPED
void EAXSound::PlayEATraxSong(int nindex) {}

void EAXSound::PlayFEMusic(int nIndex) {
    MControlPathfinder(true, 0, 0, 0).Send(UCrc32("Pathfinder5"));
}

// STRIPPED
void EAXSound::PauseFEMusic(const char *pMovieString) {}

// STRIPPED
void EAXSound::ResumeFEMusic() {}

static const int PRINT_MEMORY_COUNT_DEBUG = 0; // Decl: 2202

static const int SPEW_HEAP_HIGH_WATER = 1; // Decl: 2205

int DEBUG_GAME_STATE = 0; // Decl: 2207

int MaxVoiceCnt = 0; // Decl: 2209

int nDebug3DMix = 0;                      // Decl: 2212
int nt1 = 0;                              // Decl: 2213
int nt2 = 0;                              // Decl: 2214
int gnStopMusic = 0;                      // Decl: 2215
static const int gnTweak3DMasterVols = 0; // Decl: 2216

eRaceStage rs_cur = RACE_STAGE_NONE;
int DisableSoundUpdate = 0; // Decl: 2219

int nTestNFSMixShape = 0;                  // Decl: 2223
int nIncrement = 0;                        // Decl: 2224
bool bStreamBlockState = false;            // Decl: 2225
bool bAudioInterrupt = false;              // Decl: 2226
bool bStreamReadTiming = false;            // Decl: 2227
uint32 uStreamBlockTicks = 0;              // Decl: 2228
uint32 uStreamReadTicks = 0;               // Decl: 2229
uint32 uAudioInterruptTicks = 0;           // Decl: 2230
uint32 uDynMixUpdateTicks = 0;             // Decl: 2231
float fdmixpeaktime = 0.0f;                // Decl: 2232
float ftest_controlfp;                     // Decl: 2233
int bTestSpeech = 0;                       // Decl: 2234
int testspeech = 0;                        // Decl: 2235
static const int SNDPRINT_STREAM_DATA = 0; // Decl: 2236
bool gbIs360XMPOverrideOn;                 // Decl: 2237

void EAXSound::Update(float t) {
    if (IsSoundEnabled == 0 || t < 0.0f) {
        return;
    }

    if (gnHasStartLoadFEBeenProcessed != 0) {
        gnHasStartLoadFEBeenProcessed--;
        if (gnHasStartLoadFEBeenProcessed < 0) {
            gnHasStartLoadFEBeenProcessed = 0;
        }
        return;
    }

    if (gbHasStartNewGamePlayBeenProcessed) {
        gbHasStartNewGamePlayBeenProcessed = false;
        return;
    }

    if (DisableSoundUpdate != 0) {
        return;
    }

    if (!bReadCallbackToggle) {
        if (bStreamReadTiming == false) {
            bStreamReadTiming = true;
            {
                float freadtime = bGetTickerDifference(uStreamReadTicks, bGetTicker());
            }
            uStreamReadTicks = bGetTicker();
        }
    } else if (bStreamReadTiming == true) {
        {
            float freadtime = bGetTickerDifference(uStreamReadTicks, bGetTicker());
            uStreamReadTicks = bGetTicker();
            bStreamReadTiming = false;
        }
    }

    if (gbWorldDataBlocksAudioRead == true) {
        if (bStreamBlockState == false) {
            bStreamBlockState = gbWorldDataBlocksAudioRead;
            uStreamBlockTicks = bGetTicker();
        }
    } else if (bStreamBlockState == true) {
        float time = bGetTickerDifference(uStreamBlockTicks, bGetTicker());
        bStreamBlockState = false;
    }

    // TODO
    bool FAKE = gbAudioInterruptsWorldDataRead;
    if (gbAudioInterruptsWorldDataRead == true) {
        if (bAudioInterrupt == false) {
            uAudioInterruptTicks = bGetTicker();
        }
        bAudioInterrupt = FAKE;
    } else if (bAudioInterrupt == true) {
        float time = bGetTickerDifference(uAudioInterruptTicks);
        bAudioInterrupt = false;
    } else {
        bAudioInterrupt = false;
    }

    if (gb_DORESTART_RACE) {
        int id = this->mEventID;

        if (id != 0) {
            Scheduler::Get().fSchedule_OncePerGameLoop->RemoveTask(id);
        }

        EAXCar::g_TurboInfo = nullptr;
        EAXCar::g_ShiftInfo = nullptr;
        this->m_pNFSMixMaster->DestroyMap();
        this->InitializeInGame();
        this->m_pNFSMixMaster->InitMixMap(0);

        if (Speech::Manager::GetSpeechModule(1) != nullptr) {
            Speech::Manager::GetSpeechModule(1)->PurgeSpeech();
        }

        SFXObj_PFEATrax *ppf = static_cast<SFXObj_PFEATrax *>(this->GetSFXBase_Object(0x40010010));
        if (ppf != nullptr) {
            ppf->RestartRace();

            if (this->m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
                ppf->SetSplitScreen(true);
            } else {
                ppf->SetSplitScreen(false);
            }
        }

        bMemSet(GameFlowSndState, 0, sizeof(GameFlowSndState));
        gb_DORESTART_RACE = false;
        SetSoundControlState(false, SNDSTATE_PAUSE, "PauseMenu");

        return;
    }

    ProfileNode profile_node;

    if (gb_Is321) {
        if (g_pNISRevMgr != nullptr) {
            g_pNISRevMgr->Start321Reving();
        }
        gb_Is321 = false;
    }

    if (GameFlowSndState[13] != 0) {
        Speech::Manager::Deduce();
    }

    profile_node.Begin("gAEMSMgr.Update()", 0);
    gAEMSMgr.Update();

    SndCamera::UpdateCameras();
    profile_node.Begin("m_pEAXSND8Wrapper->Update()", 0);
    this->m_pEAXSND8Wrapper->Update();

    if (g_pNISRevMgr != nullptr) {
        g_pNISRevMgr->Update(t);
    }

    if (this->m_pNFSMixMaster->IsMixMapReady() == true) {
        if (this->m_pSTICH_Playback != nullptr) {
            profile_node.Begin("m_pSTICH_Playback->Update(t)", 0);
            this->m_pSTICH_Playback->Update(t);
        }

        profile_node.Begin("Speech::Manager::Update()", 0);
        Speech::Manager::Update(t);

        if (this->GetFrontEnd() != nullptr) {
            profile_node.Begin("GetFrontEnd()->Update()", 0);
            this->GetFrontEnd()->Update(nullptr);
        }

        SndBase::m_fRunningTime += t;
        SndBase::m_fDeltaTime = t;

        profile_node.Begin("m_pStateMgr[n]->UpdateParams()", 0);
        for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
            if (this->m_pStateMgr[n] != nullptr) {
                this->m_pStateMgr[n]->UpdateParams(g_EAXIsPaused() ? 0.0f : t);
            }
        }

        if (this->m_pNFSMixMaster != nullptr) {
            profile_node.Begin("m_pNFSMixMaster->ProcessMixMap(t)", 0);
            eCamStates ecam = SndCamera::GetCurCamState(0);
            this->m_pNFSMixMaster->ProcessMixMap(t, ecam);
        }

        profile_node.Begin("m_pStateMgr[n]->ProcessUpdate()", 0);
        for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
            if (this->m_pStateMgr[n] != nullptr) {
                this->m_pStateMgr[n]->ProcessUpdate();
            }
        }

        if (this->m_pCmnSnd != nullptr) {
            profile_node.Begin("m_pCmnSnd->Update()", 0);
            this->m_pCmnSnd->Update(nullptr);
        }
    }

    this->m_prevSndGameMode = this->m_eSndGameMode;

    if (!this->AreResourceLoadsPending()) {
        CarSoundConn::ForEach(CarSoundConn::SetAssetsLoaded);
    }

    SNDSYS_service();
}

void EAXSound::CommitAssets() {
    CSTATEMGR_CarState::ResolveCarBanks();
    for (CarSoundConn::List::const_iterator iter = CarSoundConn::GetList().begin(); iter != CarSoundConn::GetList().end(); ++iter) {
        CarSoundConn *pconn = *iter;
        if (!pconn->mConnected) {
            pconn->mConnected = true;
        }
    }
}

int PRINT_AUDIO_MEM_POOL = 0; // Decl: 2683
int PRINT_SND11_MEM_POOL = 0; // Decl: 2684

// STRIPPED
void EAXSound::DebugAndProfile() {}

EAXCar *EAXSound::GetPlayerTunerCar(int nindex) {
    if (m_pStateMgr[eMM_PLAYERCAR] != nullptr) {
        return static_cast<EAXCar *>(m_pStateMgr[eMM_PLAYERCAR]->GetStateObj(nindex));
    }
    return nullptr;
}

// STRIPPED
EAXCar *EAXSound::GetAITunerCar(int nindex) {
    return nullptr;
}

// STRIPPED
EAXCar *EAXSound::ConnectCarSnd(EAX_CarState *pcar) {
    return nullptr;
}

CSTATE_Helicopter *EAXSound::SpawnHelicopter(EAX_HeliState *pHeli) {
    if (IsSoundEnabled == 0) {
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
    if (IsSoundEnabled == 0) {
        return;
    }
    CSTATE_Base *newheli;
    eMAINMAPSTATES eStateMgrType = eMM_HELICOPTER;
    newheli = m_pStateMgr[eStateMgrType]->GetStateObj(pHeli);
    if (newheli != nullptr) {
        newheli->Detach();
    }
}

void EAXSound::DestroyEAXCar(EAX_CarState *pCar) {
    if (IsSoundEnabled == 0) {
        return;
    }
    CSTATE_Base *attachedcar = nullptr;
    switch (pCar->GetContext()) {
        case Sound::CONTEXT_PLAYER:
            if (m_pStateMgr[eMM_PLAYERCAR] != nullptr) {
                attachedcar = m_pStateMgr[eMM_PLAYERCAR]->GetStateObj(pCar);
            }
            break;
        case Sound::CONTEXT_AIRACER:
            if (m_pStateMgr[eMM_AIRACECAR] != nullptr) {
                attachedcar = m_pStateMgr[eMM_AIRACECAR]->GetStateObj(pCar);
            }
            break;
        case Sound::CONTEXT_COP:
            if (m_pStateMgr[eMM_COPCAR] != nullptr) {
                attachedcar = m_pStateMgr[eMM_COPCAR]->GetStateObj(pCar);
            }
            break;
        case Sound::CONTEXT_TRAFFIC:
            if (m_pStateMgr[eMM_TRAFFIC] != nullptr) {
                attachedcar = m_pStateMgr[eMM_TRAFFIC]->GetStateObj(pCar);
            }
            break;
        case Sound::CONTEXT_ONLINE:
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

int nengineupgradelevel = 0; // Decl: 2987
int ExhManufacterer = 0;     // Decl: 2988

void EAXSound::LoadFrontEndSoundBanks(void (*callback)(int), int32 callback_param) {
    if (IsSoundEnabled == 0) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    bSyncTaskRun();
    gnHasStartLoadFEBeenProcessed = 2;
    g_pEAXSound->SetSndGameMode(SND_FRONTEND);
    gAEMSMgr.ResetBankLoadParams();
    this->m_pStreamManager = new ("EAXS_StreamManager", false) EAXS_StreamManager();
    this->m_pStreamManager->InitializeStreams(SNDGM_FRONTEND);

    if ((g_ActiveCtlStates & 0x20000) == 0) {
        g_ActiveCtlStates = 0;
        g_ActiveSFXStates = 0;
        g_PrevActiveCtlStates = 0;
        g_PrevActiveSFXStates = 0;
    }

    this->InitializeFrontEnd();

    this->m_pCmnSnd->Initialize();
    this->m_pFESnd->Initialize();

    this->m_pNFSMixMaster->CreateMainMainMap(eRACE_CIRCUIT);
    this->m_pNFSMixMaster->InitMixMap(0);

    gAEMSMgr.m_ExternalLoadCallback = callback;
    gAEMSMgr.m_ExternalLoadCallbackParam = callback_param;

    this->ReInitMasterVolumes();
    bSyncTaskRun();

    if (IsSpeechEnabled != 0) {
        gSpeechCache.Validate();
    }
}

void EAXSound::UnloadFrontEndSoundBanks() {
    if (IsSoundEnabled == 0) {
        return;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.1f);
        ServiceQueuedFiles();
    }

    Speech::Manager::Destroy();

    if (this->GetFrontEnd() != nullptr) {
        this->GetFrontEnd()->DestroyAllDriveOnSnds();
    }

    int n;
    for (n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->ExitWorld();
        }
    }

    delete this->m_pStreamManager;
    this->m_pStreamManager = nullptr;

    if (this->m_pNFSMixMaster != nullptr) {
        this->m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (this->m_pNFSLiveLink != nullptr) {
        this->m_pNFSLiveLink->bMonitorChannel = false;
    }

    if (this->m_pFESnd != nullptr) {
        delete this->m_pFESnd;
        this->m_pFESnd = nullptr;
    }

    if (this->m_pCmnSnd != nullptr) {
        delete this->m_pCmnSnd;
        this->m_pCmnSnd = nullptr;
    }

    while (gAEMSMgr.m_nEndOfList != 0) {
        gAEMSMgr.UnloadSndData(0);
    }

    gAEMSMgr.DestroySlots(false);
    gAEMSMgr.ResetBankLoadParams();
    bSyncTaskRun();

    if (IsSpeechEnabled != 0) {
        gSpeechCache.Validate();
    }
}

void g_LoadSndAsset(Attrib::StringKey filename, eSNDDATAPATH path, eSNDDATATYPE datatype) {
    stSndAssetQueue requeststruct;
    requeststruct.pThis = nullptr;
    requeststruct.pCar = nullptr;
    requeststruct.Asset.FileName = filename;
    requeststruct.Asset.DataPath = path;
    requeststruct.Asset.eDataType = datatype;
    gAEMSMgr.QueueFileLoad(requeststruct, eBANK_SLOT_NONE);
}

extern char *csfxedit[12];

void LoadCommonIngameFiles() {
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_FEBanks(1), SNDPATH_GLOBAL, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_EnvBanks(), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(5), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(4), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(3), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(1), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_RNBanks(), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(Attrib::StringKey("SIREN_MB.abk"), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_StitchBanks(0), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_StitchBanks(2), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_StitchBanks(1), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(2), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_WNBanks(0), SNDPATH_INGAME, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(6), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM);
    g_LoadSndAsset(g_pEAXSound->GetAttributes().AEMS_MiscBanks(7), SNDPATH_ENGINE, SDT_AEMS_ASYNCSPUMEM);

    for (int n = 0; n < 12; n++) {
        g_LoadSndAsset(Attrib::StringKey(csfxedit[n]), SNDPATH_FXEDIT, SDT_GENERIC_DATA);
    }
}

void EAXSound::StopSND11() {}

void EAXSound::StartSND11() {}

bool bHasDataLoadOccured = false; // Decl: 3396

void EAXSound::LoadInGameSoundBanks(void (*callback)(int), int32 callback_param) {
    if (IsSoundEnabled == 0) {
        return;
    }

    if (IsSpeechEnabled != 0) {
        gSpeechCache.Validate();
    }

    gIsPauseForPause = 0;
    bIsAnFEToIngameTransition = true;
    this->m_pStreamManager = new ("EAXS_StreamManager") EAXS_StreamManager();

    if (FEDatabase->IsSplitScreenMode()) {
        this->m_pStreamManager->InitializeStreams(SNDGM_SPLITSCREEN);
        this->m_ePlayerMixMode = EAXS3D_TWO_PLAYER_MIX;
    } else {
        this->m_pStreamManager->InitializeStreams(SNDGM_FREEROAM);
        this->m_ePlayerMixMode = EAXS3D_SINGLE_PLAYER_MIX;
    }

    bHasDataLoadOccured = true;
    bHasStartNewGameOccured = false;
    this->m_pCurAudioSettings = &FEDatabase->CurrentUserProfiles[0]->GetOptions()->TheAudioSettings; // TODO inline
    this->m_pEAXSND8Wrapper->ReInit();

    g_pEAXSound->SetSndGameMode(SND_STREETRACE);
    gAEMSMgr.ResetBankLoadParams();
    CSTATEMGR_CarState::ResetCarBanks();
    bMemSet(g_SndAssetList, 0, sizeof(g_SndAssetList));
    this->RefreshLocalAttr();

    if (this->m_ePlayerMixMode == EAXS3D_TWO_PLAYER_MIX) {
        IsSpeechEnabled = 0;
        Speech::Manager::Init(SPEECH_SPLITSCREEN_MODE);
    } else {
        IsSpeechEnabled = 1;
        Speech::Manager::Init(SPEECH_GAME_MODE);
    }

    LoadCommonIngameFiles();

    if ((g_ActiveCtlStates & 0x20000) == 0) {
        g_ActiveCtlStates = 0;
        g_ActiveSFXStates = 0;
        g_PrevActiveCtlStates = 0;
        g_PrevActiveSFXStates = 0;
    }

    gAEMSMgr.m_ExternalLoadCallback = callback;
    gAEMSMgr.m_ExternalLoadCallbackParam = callback_param;

    if (IsSpeechEnabled != 0) {
        gSpeechCache.Validate();
    }
}

void EAXSound::CloseSound() {
    bHasDataLoadOccured = false;

    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (m_pStateMgr[n] != nullptr) {
            m_pStateMgr[n]->ExitWorld();
        }
    }

    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.1f);
        ServiceQueuedFiles();
    }

    if (this->m_pNFSMixMaster != nullptr) {
        this->m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (g_pNISRevMgr != nullptr) {
        g_pNISRevMgr->CloseNIS();
    }

    bSyncTaskRun();
}

void EAXSound::UnLoadInGameSoundBanks() {
    if (IsSoundEnabled == 0) {
        return;
    }

    IsSpeechEnabled = 1;
    gIsPauseForPause = 0;
    while (g_pEAXSound->AreResourceLoadsPending()) {
        g_pEAXSound->Update(0.1f);
        ServiceQueuedFiles();
    }

    bHasDataLoadOccured = false;
    if (this->mmsgMRestartRace != nullptr) {
        Hermes::Handler::Destroy(this->mmsgMRestartRace);
        this->mmsgMRestartRace = nullptr;
    }

    if (IsSpeechEnabled) {
        gSpeechCache.Validate();
    }

    Speech::Manager::Destroy();
    bSyncTaskRun();

    if (this->m_pNFSMixMaster != nullptr) {
        this->m_pNFSMixMaster->DestroyMainMainMap();
    }

    if (m_pStateMgr[0] != nullptr) {
        m_pStateMgr[0]->ExitWorld();
    }

    delete this->m_pStreamManager;
    this->m_pStreamManager = nullptr;

    for (int n = 0; n < NUM_ELEMENTS(SFXObj_Reverb::m_pFXEditPatch); n++) {
        if (SFXObj_Reverb::m_pFXEditPatch[n] != nullptr) {
            gAudioMemoryManager.FreeMemory(SFXObj_Reverb::m_pFXEditPatch[n]);
        }
        SFXObj_Reverb::m_pFXEditPatch[n] = nullptr;
    }

    if (this->m_pCmnSnd != nullptr) {
        delete this->m_pCmnSnd;
        this->m_pCmnSnd = nullptr;
    }

    while (gAEMSMgr.m_nEndOfList != 0) {
        gAEMSMgr.UnloadSndData(0);
    }

    gAEMSMgr.DestroySlots(true);
    gAEMSMgr.ResetBankLoadParams();
    bSyncTaskRun();
}

int DEBUG_SNDPAUSE = 0; // Decl: 3638

// STRIPPED
void EAXSound::EnterPauseMenu(eSNDPAUSE_REASON pause_reason) {}

// STRIPPED
void EAXSound::ExitPauseMenu(eSNDPAUSE_REASON pause_reason) {}

void EAXSound::ReStartRace(bool bIs321) {
    if (IsSoundEnabled == 0) {
        return;
    }

    SetSoundControlState(true, SNDSTATE_STOP_MUSIC, "RestartRace");

    for (int s = 0; s < 4; s++) {
        if (this->m_pStreamManager->GetStreamChannel(s) != nullptr) {
            this->m_pStreamManager->GetStreamChannel(s)->Stop();
            this->m_pStreamManager->GetStreamChannel(s)->PurgeStream();
            this->m_pStreamManager->GetStreamChannel(s)->Resume();
        }
    }

    if (this->m_pCmnSnd != nullptr) {
        delete this->m_pCmnSnd;
        this->m_pCmnSnd = nullptr;
    }

    for (int n = 0; n < eMM_MAX_MAIN_MIXSTATES; n++) {
        if (n != eMM_MUSIC) {
            if (m_pStateMgr[n] != nullptr) {
                m_pStateMgr[n]->ExitWorld();
            }
        } else {
            m_pStateMgr[1]->DisconnectMixMap();
        }
    }

    gb_DORESTART_RACE = true;
    gb_Is321 = bIs321;
}

// STRIPPED
void EAXSound::InitSndCars() {}

static const int kAudioMemPoolSize = 1148 * 1024; // size: 0x4, Decl: 3759

void InitializeSoundDriver() {
    int size = 16 * 1024;
    if (IsSoundEnabled != 0) {
        size = kAudioMemPoolSize;
    }

    gAudioMemoryManager.InitMemoryPool(AUD_MAIN_MEM_POOL, size);
    g_pEAXSound = new ("AUD:EAXSound") EAXSound();
    g_pEAXSound->InitializeDriver();
}

void InitializeSoundLoad() {
    g_pEAXSound->InitializeSoundBootLoad();
}

// STRIPPED
void DestroySoundDriver() {}

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

bool RUN_SOUND_STATE = true; // Decl: 3845

void SoundPause(bool bpause, eSNDPAUSE_REASON esndpause) {}

void FESoundControl(bool bOn, const char *name) {
    if (g_pEAXSound == nullptr) {
        return;
    }

    unsigned int key = Attrib::StringHash32(name);
    Attrib::StringKey FengList[37] = {
        "Pause_Main.fng",        "Pause_Options.fng",     "Pause_Controller.fng",    "InGamePhotoMaster.fng",
        "EA_Trax.fng",           "FadeScreen.fng",        "SMS_Message.fng",         "BUSTED_OVERLAY.fng",
        "SixDaysLater.fng",      "InGame_MC_Main_GC.fng", "InGameAnyMovie.fng",      "Pause_Performance_Tuning.fng",
        "InGame_MC_Main.fng",    "InGameDialog.fng",      "WS_InGameAnyMovie.fng",   "InGameAnyMovie.fng",
        "InGameAnyTutorial.fng", "FEAnyMovie.fng",        "WS_FEAnyMovie.fng",       "FEAnyTutorial.fng",
        "InGameAnyTutorial.fng", "LS_EALogo.fng",         "LS_EA_hidef.fng",         "LS_PSA.fng",
        "MW_LS_IntroFMV.fng",    "MW_LS_AttractFMV.fng",  "WS_LS_EALogo.fng",        "WS_LS_EA_hidef.fng",
        "WS_LS_PSA.fng",         "WS_LS_IntroFMV.fng",    "WS_MW_LS_AttractFMV.fng", "PostRace_MilestoneRewards.fng",
        "PostRace_Pursuit.fng",  "Game_StartRace",        "InGameMilestones.fng",    "InGameBounty.fng",
        "InGameRaceSheet.fng",
    };

    int namehash = static_cast<int>(bStringHash(name));
    int index = -1;
    for (int n = 0; n < 37; n++) {
        if (key == FengList[n].GetHash32()) {
            index = n;
            break;
        }
    }

    if (g_pEAXSound->GetSndGameMode() != SND_FRONTEND) {
        switch (index) {
            case 0:
            case 1:
            case 2:
            case 11:
                g_pEAXSound->m_bPause_MainFNG = bOn;
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                break;

            case 3:
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                break;

            case 4:
            case 5:
            case 6:
            case 7:
                break;

            case 8:
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                SetSoundControlState(bOn, SNDSTATE_OFF, name);
                break;

            case 9:
            case 12:
                if (g_pEAXSound->m_bPause_MainFNG) {
                    return;
                }
                SetSoundControlState(bOn, SNDSTATE_PAUSE, name);
                break;

            case 16:
            case 20:
                SetSoundControlState(bOn, SNDSTATE_FMV, name);
                SetSoundControlState(bOn, SNDSTATE_STOP_MUSIC, name);
                break;

            case 10:
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
                if (bOn) {
                    SetSoundControlState(bOn, SNDSTATE_FMV, name);
                    return;
                }

                SetSoundControlState(false, SNDSTATE_FMV, name);
                SetSoundControlState(true, SNDSTATE_STOP_MUSIC, name);
                break;

            case 13:
                break;

            case 31:
            case 32:
                SetSoundControlState(bOn, SNDSTATE_FE_SMS_MESSAGE, name);
                break;

            case 33:
                SetSoundControlState(bOn, SNDSTATE_STOP_MUSIC, name);
                break;

            case 34:
            case 35:
            case 36:
                if (Speech::Manager::GetSpeechModule(1) != nullptr) {
                    Speech::Manager::GetSpeechModule(1)->PurgeSpeech();
                }
                SetSoundControlState(bOn, SNDSTATE_FE_UPSCREEN, name);
                break;

            default:
                SetSoundControlState(bOn, SNDSTATE_FE_UPSCREEN, name);
                break;
        }
    } else if (index == 10 || index >= 14) {
        SetSoundControlState(bOn, SNDSTATE_FMV, "name");
    }
}

void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason) {
    if (!RUN_SOUND_STATE || g_pEAXSound == nullptr || !IsSoundEnabled) {
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
        if ((g_ActiveSFXStates & (1 << state)) == (g_PrevActiveSFXStates & (1 << state))) {
            continue;
        }
        if (g_pEAXSound->GetStreamManager() == nullptr) {
            continue;
        }
        if ((g_ActiveSFXStates & (1 << state)) != 0) {
            switch (state) {
                case 0:
                    if (g_pEAXSound->GetStreamManager()->GetStreamChannel(1) != nullptr) {
                        g_pEAXSound->GetStreamManager()->GetStreamChannel(1)->Pause();
                    }
                    break;
                case 1:
                    if (g_pEAXSound->GetStreamManager()->GetStreamChannel(0) != nullptr) {
                        g_pEAXSound->GetStreamManager()->GetStreamChannel(0)->Pause();
                    }
                    break;
                case 2:
                    if (g_pEAXSound->GetStreamManager()->GetStreamChannel(2) != nullptr) {
                        g_pEAXSound->GetStreamManager()->GetStreamChannel(2)->Pause();
                    }
                    break;
            }
        } else {
            switch (state) {
                case 0:
                    if (g_pEAXSound->GetStreamManager()->GetStreamChannel(1) != nullptr) {
                        g_pEAXSound->GetStreamManager()->GetStreamChannel(1)->Resume();
                    }
                    break;
                case 1:
                    if (g_pEAXSound->GetStreamManager()->GetStreamChannel(0) != nullptr) {
                        g_pEAXSound->GetStreamManager()->GetStreamChannel(0)->Resume();
                    }
                    break;
                case 2:
                    if (g_pEAXSound->GetStreamManager()->GetStreamChannel(2) != nullptr) {
                        g_pEAXSound->GetStreamManager()->GetStreamChannel(2)->Resume();
                    }
                    break;
            }
        }
    }

    if (esndstate == SNDSTATE_ERROR) {
        g_pEAXSound->Update(0.1f);
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
    if (IsSoundEnabled != 0) {
        g_pEAXSound->CloseSound();
    }
}

// STRIPPED
eSndAudioMode EAXSound::SetAudioRenderMode(eSndAudioMode mode) {
    return AUDIO_MODE_STEREO;
}

eSndAudioMode EAXSound::GetDefaultPlatformAudioMode() {
    return this->m_pEAXSND8Wrapper->GetDefaultPlatformAudioMode();
}

eSndAudioMode EAXSound::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    return this->m_pEAXSND8Wrapper->SetAudioModeFromMemoryCard(mode);
}

#define MAKEID(a, b, c, d) (((int)(a) << 24) | ((int)(b) << 16) | ((int)(c) << 8) | (int)(d)) // Decl: 4022

bool EAXSound::ValidateStreamChunks(const char *filepath, int start, int end) {}
