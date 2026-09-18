#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "path/path.h"
#include "snd/sndo.h"
#include "Speed/Indep/Libs/path/5.01.04/include/path/path.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"

extern unsigned int g_ActiveSFXStates;

// int DEBUG_PATHFINDER = 0;          // size: 0x4, Decl: 45
int DEBUG_STREAMS = 0;                         // size: 0x4, address: 0xFFFFFFFF, Decl: 46
ParameterAccessor AmbientAccessor("Ambience"); // size: 0x1C, address: 0x8045E5A0, Decl: 47

static const int OVER_RIDE_INTERACTIVE = 0; // size: 0x4, Decl: 50
static const int INTERACTIVE_PROJECT = 0;   // size: 0x4, Decl: 51
extern int MUSICFLOW_DISPLAY;               // size: 0x4, Decl: 52 // I hate that this is actually here....
int PURSUIT_TO_LIC_DELAY = 40;              // size: 0x4, address: 0x804182A8, Decl: 53
extern unsigned int SoundRandomSeed;        // size: 0x4, Decl: 54

int DEBUG_SFXOBJ_INIT = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 60

DEFINE_CREATABLE(0x1010000, SFXObj_Pathfinder, SndBase);

// size: 0x8, address: 0xFFFFFFFF, Decl: 65
uint32 SPECIAL_EVENTS[2] = {0x17B768A, 0};

// size: 0x38, address: 0x804182C0, Decl: 108
uint32 AmbientCrossMap[14] = {
    0x011BBC15, 0x01E40616, 0x01919B1B, 0x01C505B7, 0x0134209F, 0x012F7671, 0x0127205F,
    0x01B2D374, 0x019509F2, 0x0146EDA3, 0x0196E300, 0x017690D2, 0x01E6FF17, 0x01E45B3F,
};

SFXObj_Pathfinder::SFXObj_Pathfinder() : CARSFX() {
    this->m_pSFXCTL_Pathfinder = nullptr;
    this->m_PFParms[0].pmapfile = nullptr;
    this->m_PFParms[1].pmapfile = nullptr;
    this->m_Flags = 0x40;
}

int SFXObj_Pathfinder::GetController(int Index) {
    return Index == 0 ? 0 : -1;
}

void SFXObj_Pathfinder::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        m_pSFXCTL_Pathfinder = static_cast<SFXCTL_Pathfinder *>(psfxctl);
    }
}

extern int g_MaxSongs; // lo define FEDatabase.cpp (zFe2), como el original

// TODO use
static const int DebugPrintIntensity = 0;     // size: 0x4, Decl: 210
static const int DebugTransitionLicToInt = 0; // size: 0x4, Decl: 211
static const int DebugTransitionIntToLic = 0; // size: 0x4, Decl: 212
static const int X360OverrideUserTunes = 0;   // size: 0x4, Decl: 213

int DBGPRNT_EATRAX = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 215

// size: 0x30, Decl: 217
stEATraxParms SFXObj_PFEATrax::m_EATrax[2];

DEFINE_CREATABLE(0x1010010, SFXObj_PFEATrax, SFXObj_Pathfinder);

SFXObj_PFEATrax::SFXObj_PFEATrax()
    : SFXObj_Pathfinder(),   //
      m_FilterFreq(0xFFFF),  //
      m_Volume(0),           //
      m_FilterFade(nullptr), //
      m_CurPart(-1),         //
      m_PartStart(0),        //
      mT_ambienceStart(),    //
      mMsgControlPF5(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(this, &SFXObj_PFEATrax::MessageStartPathfinder,
                                                                                                   UCrc32("Pathfinder5"), 0)), //
      mMsgControlInitSongslist(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
          this, &SFXObj_PFEATrax::MessageInitSongsList, UCrc32("EATraxInit"), 0)), //
      mMsgSendPathEvent(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(this, &SFXObj_PFEATrax::MessageSendPathEvent,
                                                                                                      UCrc32("Event"), 0)), //
      mMsgSendPathControl(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
          this, &SFXObj_PFEATrax::MessageSendPathControl, UCrc32("Control"), 0)), //
      mMsgNotifyPartUpdate(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(this, &SFXObj_PFEATrax::MessagePartUpdate,
                                                                                                         UCrc32("PartUpdate"), 0)), //
      mMsgPerpBusted(Hermes::Handler::Create<MPerpBusted, SFXObj_PFEATrax, SFXObj_PFEATrax>(this, &SFXObj_PFEATrax::MessagePerpBusted,
                                                                                            UCrc32(UCRC32_Gameplay), 0)), //
      mMsgInteractiveDone(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
          this, &SFXObj_PFEATrax::MessageInteractiveDone, UCrc32("InteractiveDone"), 0))
#ifndef EA_BUILD_A124 // la alpha 124 no tiene mMsgSwapInteractive (SFXObj_Pathfinder.hpp)
      , //
      mMsgSwapInteractive(Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
          this, &SFXObj_PFEATrax::MessageSwapInteractive, UCrc32("Swap"), 0))
#endif
{
    this->m_EATraxState = EATRAX_OFF;
    this->m_CurPathEvent = 0;
    this->m_PrevPathEvent = static_cast<unsigned int>(-1);
    this->m_nAmbientZone = -1;
    this->m_AmbZoneVol = 0;
    this->m_CurIntensity = 0;
    this->m_PrevIntensity = 0;
    this->m_ActiveProject = PF_LICENSED_MUSIC;
    this->m_PrevActiveProject = PF_PROJECTRESET;
    this->m_PrevMusicType = eMUSIC_TYPE_SPLASH;
    this->m_MusicType = eMUSIC_TYPE_SPLASH;
    this->m_InteractiveProj = static_cast<eINTERACTIVE_PROJ_ID>((bRandom(4) + 1) & PF_INTERACTIVE_03);
    this->m_Flags = 0x40;
    this->m_bSkipUpdate = false;
#ifndef EA_BUILD_A124
    this->m_bClearSkipUpdate = false;
#endif
    this->mT_ambienceStart = Timer(0);
#ifndef EA_BUILD_A124
    this->m_bPathFAILED = false;
    this->m_FEPreviewEvent = 0;
    this->m_FEPreviewIndex = -1;
#endif
}

SFXObj_PFEATrax::~SFXObj_PFEATrax() {
    if (this->mMsgControlPF5 != nullptr) {
        Hermes::Handler::Destroy(this->mMsgControlPF5);
    }
    if (this->mMsgControlInitSongslist != nullptr) {
        Hermes::Handler::Destroy(this->mMsgControlInitSongslist);
    }
    if (this->mMsgSendPathEvent != nullptr) {
        Hermes::Handler::Destroy(this->mMsgSendPathEvent);
    }
    if (this->mMsgSendPathControl != nullptr) {
        Hermes::Handler::Destroy(this->mMsgSendPathControl);
    }
    if (this->mMsgNotifyPartUpdate != nullptr) {
        Hermes::Handler::Destroy(this->mMsgNotifyPartUpdate);
    }
    if (this->mMsgPerpBusted != nullptr) {
        Hermes::Handler::Destroy(this->mMsgPerpBusted);
    }
    if (this->mMsgInteractiveDone != nullptr) {
        Hermes::Handler::Destroy(this->mMsgInteractiveDone);
    }
#ifndef EA_BUILD_A124
    if (this->mMsgSwapInteractive != nullptr) {
        Hermes::Handler::Destroy(this->mMsgSwapInteractive);
    }
#endif

    this->Destroy();
}

void SFXObj_PFEATrax::RestartRace() {
    if (this->m_MusicType == eMUSIC_TYPE_LICENCED || this->m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
        PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
        PATH_clearallevents(PATH_ALL_PROJECTS);
        this->m_MusicType = eMUSIC_TYPE_LICENCED;
#ifndef EA_BUILD_A124
        this->m_bPathFAILED = false;
#endif
    }

    this->mT_ambienceStart = Timer(0);
}

void SFXObj_PFEATrax::SendPathEvent() {
    // TODO magic
    if (this->m_EATraxState != EATRAX_UNINIT && (this->m_Flags & 4) != 0) {
        if (this->m_CurPathEvent != this->m_PrevPathEvent && (this->m_Flags & 0x800) == 0) {
#ifndef EA_BUILD_A124
            this->m_bPathFAILED = false;
#endif
            this->m_PrevPathEvent = this->m_CurPathEvent;

            int status = PATH_event(this->m_PFParms[this->m_ActiveProject].PATH_TRACK, this->m_CurPathEvent);
            switch (status) {
                case PATH_OK:
                    PATH_volume(this->m_PFParms[this->m_ActiveProject].PATH_TRACK, 0);

                    if (this->m_pSFXCTL_Pathfinder != nullptr) {
                        SNDSYS_entercritical();
                        SNDSTRM_lowpass(this->m_pSFXCTL_Pathfinder->GetHandle(this->m_ActiveProject), this->GetDMixOutput(9, DMX_FREQ));
                        SNDSYS_leavecritical();
                    }
                    break;

                case PATHERR_PENDING:
                case PATHERR_ALREADYLOADED:
                case PATHERR_FAILALLOC:
                case PATHERR_NOTFOUND:
                case PATHERR_CANTOPEN:
                case PATHERR_INUSE:
                case PATHERR_NOT_INITED:
                case PATHERR_TOOMANY:
                case PATHERR_INV_PARAM:
                case PATHERR_GENERAL:
                default:
                    this->m_PrevPathEvent = 0;
#ifndef EA_BUILD_A124
                    this->m_bPathFAILED = true;
#endif
                    return;
            }

            this->m_PFParms[this->m_ActiveProject].queue_next = 0;
            if (status == 1) {
                this->m_PFParms[this->m_ActiveProject].queue_next += 0;
            }

            // TODO magic
            if (this->m_CurPathEvent == 0x01C53FC7) {
                this->m_PrevMusicType = this->m_MusicType;
                this->m_MusicType = eMUSIC_TYPE_SPLASH;
            }
        }
    }
}

void SFXObj_PFEATrax::SwapInteractiveProjects() {
    this->m_Flags |= 0x18; // TODO magic
}

// TODO move
extern Sound::SongInfoList Songs;

void SFXObj_PFEATrax::StartLicensedMusic(unsigned int PathEvent) {
    if ((this->m_Flags & 0x800) == 0) {
        if (this->m_ActiveProject != PF_LICENSED_MUSIC) {
            PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
            this->m_ActiveProject = PF_LICENSED_MUSIC;
            return;
        }
        this->m_ActiveProject = PF_LICENSED_MUSIC;
        if (this->m_PrevActiveProject != PF_LICENSED_MUSIC) {
            if (this->m_PrevActiveProject != PF_PROJECTRESET && (SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject] != nullptr) &&
                SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject]->bAttached) {
                PATH_stop(this->m_PFParms[this->m_PrevActiveProject].PATH_TRACK);
                this->m_pSFXCTL_Pathfinder->DetachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject]);
            }
            this->m_pSFXCTL_Pathfinder->AttachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]);
            this->m_PrevActiveProject = m_ActiveProject;
            if ((this->m_Flags & 2) == 0) {
                SwapInteractiveProjects();
            }
        }
    }
    eSndGameMode esgm = g_pEAXSound->GetSndGameMode();
    switch (esgm) {
        case SND_FRONTEND:
        case SND_FREEROAM:
            break;

        default:
            if (esgm == SND_FRONTEND || (this->m_Flags & 1) == 0 || (esgm != SND_CHALLENGERACE && (this->m_Flags & 0x200) != 0)) {
                if (esgm != SND_CHALLENGERACE) {
                    return;
                }
            } else if (esgm != SND_CHALLENGERACE) {
                break;
            }

            if (g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode != 0) {
                return;
            }
            break;
    }

    this->m_PrevMusicType = this->m_MusicType;
    this->m_MusicType = eMUSIC_TYPE_LICENCED;
    if ((this->m_Flags & 0x800) == 0) {
        PATH_clearallevents(PATH_ALL_PROJECTS);
        this->m_PrevPathEvent = this->m_CurPathEvent;
        if (this->m_EATrax[this->m_EATraxState].PlayTrackIndex != -1) {
            this->m_EATrax[this->m_EATraxState].LastPlaylistSong = this->m_EATrax[this->m_EATraxState].PlayTrackIndex;
        }
        int PathEventToUse = 0;
        if (PathEvent != 0) {
            bool foundevent = false;
            for (int n = 0; n < g_MaxSongs; n++) {
                Sound::stSongInfo *CurSong = Songs[n];
                if (CurSong->PathEvent == static_cast<int>(PathEvent)) {
                    foundevent = true;
                    this->m_EATrax[this->m_EATraxState].PlayTrackIndex = n;
                    {
                        Sound::stSongInfo *CurSong = Songs[this->m_EATrax[this->m_EATraxState].PlayTrackIndex];
                        PathEventToUse = CurSong->PathEvent;
                    }
                    this->m_Flags &= ~0x40;
                    this->m_PrevPathEvent = 0;
                }
            }
            if (!foundevent) {
                this->m_EATrax[m_EATraxState].PlayTrackIndex = 0;
                this->m_EATrax[m_EATraxState].PlayBits ^= 1;
                PathEventToUse = static_cast<int>(PathEvent);
            }
        } else {
            this->GenNextMusicTrackID();
            if (this->m_EATrax[m_EATraxState].PlayTrackIndex == -1) {
                return;
            }
            Sound::stSongInfo *CurSong = Songs[this->m_EATrax[m_EATraxState].PlayTrackIndex];
            PathEventToUse = CurSong->PathEvent;
            this->m_Flags &= ~0x40;
            this->m_PrevPathEvent = 0;
        }
        this->m_CurPathEvent = static_cast<unsigned int>(PathEventToUse);
        this->m_PFParms[0].queue_next = 1;
        int status = 1;
        if (SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject] != nullptr) {
            status = SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]->track_status;
        }
        if (status == 3) {
            PATH_pause(this->m_PFParms[this->m_ActiveProject].PATH_TRACK, 0);
        }
        if (this->m_CurPathEvent != this->m_PrevPathEvent) {
            this->m_bSkipUpdate = true;
#ifndef EA_BUILD_A124
            this->m_bClearSkipUpdate = false;
#endif
        }
    }
}

void SFXObj_PFEATrax::StartAmbience(unsigned int PathEvent) {
    this->m_PrevMusicType = this->m_MusicType;
    this->m_MusicType = eMUSIC_TYPE_AMBIENCE;
    PATH_clearallevents(PATH_ALL_PROJECTS);
    this->m_ActiveProject = PF_LICENSED_MUSIC;
    if (this->m_PrevActiveProject != PF_LICENSED_MUSIC) {
        if (this->m_PrevActiveProject != PF_PROJECTRESET && (SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject] != nullptr) &&
            SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject]->bAttached) {
            m_pSFXCTL_Pathfinder->DetachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject]);
        }
        this->m_pSFXCTL_Pathfinder->AttachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]);
        this->m_PrevActiveProject = this->m_ActiveProject;
    }
    int status = 1;
    if (SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject] != nullptr) {
        status = SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]->track_status;
    }
    if (status == 3) {
        PATH_pause(this->m_PFParms[this->m_ActiveProject].PATH_TRACK, 0);
    }
    this->m_PrevPathEvent = this->m_CurPathEvent;
    this->m_PFParms[0].queue_next = 1;
    this->m_CurPathEvent = PathEvent;
    this->m_bSkipUpdate = true;
#ifndef EA_BUILD_A124
    this->m_bClearSkipUpdate = false;
#endif

    EAXS_StreamChannel *pch = g_pEAXSound->GetStreamManager()->GetStreamChannel(1);
    if (pch != nullptr) {
        pch->SetVol(0, false);
    }
}

void SFXObj_PFEATrax::StartInteractiveMusic(unsigned int PathEvent) {
    if ((this->m_Flags & 0x80C) != 4) {
        return;
    }

    this->m_PrevMusicType = this->m_MusicType;
    this->m_MusicType = eMUSIC_TYPE_INTERACTIVE;
    PATH_clearallevents(PATH_ALL_PROJECTS);
    int status = 1;
    if (SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject] != nullptr) {
        status = SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]->track_status;
    }
    if (status == 3) {
        PATH_pause(m_PFParms[this->m_ActiveProject].PATH_TRACK, 0);
    }
    this->m_ActiveProject = PF_INTERACTIVE_MUSIC;
    if (this->m_PrevActiveProject != PF_INTERACTIVE_MUSIC) {
        if (this->m_PrevActiveProject != PF_PROJECTRESET && (SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject] != nullptr) &&
            SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject]->bAttached) {
            PATH_stop(this->m_PFParms[this->m_PrevActiveProject].PATH_TRACK);
            this->m_pSFXCTL_Pathfinder->DetachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[this->m_PrevActiveProject]);
        }
        this->m_pSFXCTL_Pathfinder->AttachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]);
        this->m_PrevActiveProject = this->m_ActiveProject;
    }
    MNotifyMusicFlow(PathEvent).Send(UCrc32("Init"));
    this->m_Flags |= 0x100;
}

void SFXObj_PFEATrax::Stop() {
    this->m_EATraxState = EATRAX_OFF;
    PATH_clearallevents(PATH_ALL_PROJECTS);
    PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
    this->m_PFParms[this->m_ActiveProject].queue_next = 0;
}

void SFXObj_PFEATrax::Destroy() {
    PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
    m_PFParms[this->m_ActiveProject].queue_next = 0;
    this->m_EATraxState = EATRAX_UNINIT;
    if ((this->m_Flags & 2) == 0 && g_pEAXSound->GetSndGameMode() != SND_FRONTEND) {
        int np;
        int index = gAEMSMgr.IsAssetInList(Attrib::StringKey(this->m_PFParms[1].mapfile));
        gAEMSMgr.UnloadSndData(index);
    }
    delete this->m_FilterFade;
    this->m_FilterFade = nullptr;
    SNDSYS_service();
}

void InitializeEATrax(bool breset) {
    SFXObj_PFEATrax::m_EATrax[0].PBMode = FEDatabase->GetUserProfile(0)->GetOptions()->TheAudioSettings.PlayState;
    SFXObj_PFEATrax::m_EATrax[1].PBMode = FEDatabase->GetUserProfile(0)->GetOptions()->TheAudioSettings.PlayState;
    SFXObj_PFEATrax::m_EATrax[0].TraxMask = 0;
    SFXObj_PFEATrax::m_EATrax[1].TraxMask = 0;
    SFXObj_PFEATrax::m_EATrax[0].NumEnabledSongs = 0;
    SFXObj_PFEATrax::m_EATrax[1].NumEnabledSongs = 0;

    JukeboxEntry *playlist = FEDatabase->GetUserProfile(0)->Playlist;
    for (int n = 0; n < g_MaxSongs; n++) {
        unsigned int songindex = playlist[n].SongIndex;
        int playability = playlist[n].PlayabilityField;
        switch (playability) {
            case 0:
                break;
            case 1:
                SFXObj_PFEATrax::m_EATrax[0].TraxMask |= 1 << songindex;
                SFXObj_PFEATrax::m_EATrax[0].NumEnabledSongs++;
                break;
            case 2:
                SFXObj_PFEATrax::m_EATrax[1].TraxMask |= 1 << songindex;
                SFXObj_PFEATrax::m_EATrax[1].NumEnabledSongs++;
                break;
            case 3:
                SFXObj_PFEATrax::m_EATrax[0].TraxMask |= 1 << songindex;
                SFXObj_PFEATrax::m_EATrax[0].NumEnabledSongs++;
                SFXObj_PFEATrax::m_EATrax[1].TraxMask |= 1 << songindex;
                SFXObj_PFEATrax::m_EATrax[1].NumEnabledSongs++;
                break;
        }
    }
    SFXObj_PFEATrax::m_EATrax[0].PlayBits = SFXObj_PFEATrax::m_EATrax[0].TraxMask;
    SFXObj_PFEATrax::m_EATrax[1].PlayBits = SFXObj_PFEATrax::m_EATrax[1].TraxMask;
    if (breset) {
        SFXObj_PFEATrax::m_EATrax[0].LastPlaylistSong = -1;
        SFXObj_PFEATrax::m_EATrax[1].LastPlaylistSong = -1;
    }
}

void SFXObj_PFEATrax::MessageInitSongsList(const MControlPathfinder &message) {
    unsigned int utype = message.GetPathEvent();
    if (utype == static_cast<unsigned int>(-1)) {
        PATH_stop(this->m_PFParms[m_ActiveProject].PATH_TRACK);
        InitializeEATrax(true);
    } else {
        InitializeEATrax(false);
    }
}

eEATRAXSTATES SFXObj_PFEATrax::GenEATraxState() {
    if (FEDatabase->GetAudioSettings()->MasterVol <= 0.0f) {
        return EATRAX_OFF;
    }
    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        return EATRAX_FE;
    }
    return EATRAX_IG;
}

eMUSIC_TYPE SFXObj_PFEATrax::GenMusicType() {
    if (this->m_EATraxState == EATRAX_IG) {
        if (FEDatabase->GetAudioSettings()->IGMusicVol > 0.0f) {
            if ((this->m_Flags & 0x800) != 0) {
                return eMUSIC_TYPE_LICENCED;
            }
            bool pursuit = false;
            SoundAI *ai = SoundAI::Get();
            if (ai != nullptr && (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching)) {
                pursuit = true;
            }
            if (pursuit) {
                return eMUSIC_TYPE_INTERACTIVE;
            }
            return eMUSIC_TYPE_LICENCED;
        }
        return (this->m_Flags & 0x800) != 0 ? eMUSIC_TYPE_LICENCED : eMUSIC_TYPE_AMBIENCE;
    }
    if (this->m_EATraxState == EATRAX_FE) {
        if (FEDatabase->GetAudioSettings()->FEMusicVol > 0.0f) {
            if ((this->m_Flags & 0x800) != 0) {
                return eMUSIC_TYPE_LICENCED;
            }
            return eMUSIC_TYPE_LICENCED;
        }
        return (this->m_Flags & 0x800) != 0 ? eMUSIC_TYPE_LICENCED : eMUSIC_TYPE_AMBIENCE;
    }
    if (this->m_EATraxState != EATRAX_OFF) {
        return eMUSIC_TYPE_AMBIENCE;
    }
    return (this->m_Flags & 0x800) != 0 ? eMUSIC_TYPE_LICENCED : eMUSIC_TYPE_AMBIENCE;
}

bool SFXObj_PFEATrax::TestToPursuit() {
    if (g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode == 0 || g_pEAXSound->GetCurMusicVolume() == 0.0f || (m_Flags & 0x802) != 0) {
        return false;
    }
    bool pursuit_exists = false;
    bool pursuit_active = false;
    SoundAI *ai = SoundAI::Get();
    if ((ai != nullptr) && (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching)) {
        pursuit_active = true;
    }

    if (pursuit_active) {
        m_CurPathEvent = 0;
        // TODO magic
        StartInteractiveMusic(0x026E7282);
        return true;
    }

    return false;
}

bool SFXObj_PFEATrax::TestToLicensed(bool bstart) {
    if (this->m_bSkipUpdate || (this->m_Flags & 0x800) != 0) {
        return false;
    }

    if (g_pEAXSound->GetCurMusicVolume() > 0.0f && g_pEAXSound->GetCurAudioSettings()->EATraxMode == 1 &&
        this->m_EATrax[this->m_EATraxState].TraxMask != 0) {
        if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
            unsigned int fegm = FEDatabase->GetGameMode();

            (void)fegm;
            if (FEDatabase->IsRapSheetMode()) {
                return false;
            }
            if (bstart) {
                PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
                this->StartLicensedMusic(0);
            }
            return true;
        }

        if (GRaceStatus::Exists()) {
            const GRaceParameters *raceparms = GRaceStatus::Get().GetRaceParameters();

            if (raceparms != nullptr && raceparms->GetIsLoaded() == true &&
                (raceparms->GetIsDDayRace() || GRaceStatus::IsDragRace())) {
                return false;
            }
        }

        if (this->m_MusicType == eMUSIC_TYPE_AMBIENCE) {
            float t_playing_ambience;

            if (this->mT_ambienceStart > Timer(0)) {
                t_playing_ambience = (WorldTimer - this->mT_ambienceStart).GetSeconds();
            } else {
                t_playing_ambience = 65535.0f;
            }
            if (t_playing_ambience <= static_cast<float>(PURSUIT_TO_LIC_DELAY)) {
                return false;
            }
            if (bstart) {
                this->StartLicensedMusic(0);
            }
            this->mT_ambienceStart = Timer(0);
            return true;
        }

        if (bstart) {
            this->StartLicensedMusic(0);
        }
        return true;
    }

    if (this->m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
        if (g_pEAXSound->GetCurMusicVolume() <= 0.0f) {
            return false;
        }
        if (g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode != 0) {
            return false;
        }
        PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
        if (g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0) {
            return false;
        }
        this->m_CurPathEvent = 0;
        this->StartLicensedMusic(0);
        return true;
    }

    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (FEDatabase->IsRapSheetMode()) {
            return false;
        }
        if (g_pEAXSound->GetCurMusicVolume() > 0.0f) {
            // los dos caminos devuelven false; GCC borra el salto pero deja la comparacion
            if (g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0) {
                return false;
            }
        }
    }
    return false;
}

bool SFXObj_PFEATrax::TestToAmbience() {
    if ((this->m_Flags & 0x800) != 0) {
        return false;
    }

    this->UpdateAmbience(0.1f);

    if (this->m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
        bool keep_interactive = true;
        SoundAI *ai = SoundAI::Get();
        if (ai == nullptr || !ai->IsMusicActive() || g_pEAXSound->GetCurMusicVolume() <= 0.0f ||
            g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode <= 0) {
            keep_interactive = false;
        }
        if (!keep_interactive) {
            this->UpdateAmbience(0.1f);
            this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
            return true;
        }
        return false;
    }

    if (this->m_MusicType == eMUSIC_TYPE_LICENCED) {
        if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
            if (g_pEAXSound->GetCurMusicVolume() == 0.0f || g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0 ||
                this->m_EATrax[this->m_EATraxState].TraxMask == 0) {
                PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
                this->UpdateAmbience(0.1f);
                this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
                return true;
            }
            if (g_pEAXSound->GetCurMusicVolume() == 0.0f || g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0 ||
                this->m_EATrax[this->m_EATraxState].TraxMask == 0) {
                PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
                this->UpdateAmbience(0.1f);
                this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
                return true;
            }
            if (!FEDatabase->IsRapSheetMode()) {
                return false;
            }
            int val = (this->m_InteractiveProj + 1) & 3;
            if (val == 3) {
                val = 2;
            }
            PATH_setnamedvalue(this->m_PFParms[0].PATH_TRACK, "rapsheet", val + 1);
            this->UpdateAmbience(0.1f);
            this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
            return true;
        }

        if (g_pEAXSound->GetCurMusicVolume() == 0.0f || g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0 ||
            this->m_EATrax[this->m_EATraxState].TraxMask == 0) {
            PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
            this->UpdateAmbience(0.1f);
            this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
            return true;
        }
        return false;
    }

    if (this->m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
        if (g_pEAXSound->GetCurMusicVolume() == 0.0f || g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode == 0) {
            PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
            this->UpdateAmbience(0.1f);
            this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
            return true;
        }
    }
    return false;
}

void SFXObj_PFEATrax::UpdateInGame(float t) {
#ifndef EA_BUILD_A124
    if (this->m_bPathFAILED || (g_ActiveSFXStates & 1) != 0) {
#else
    if ((g_ActiveSFXStates & 1) != 0) {
#endif
        return;
    }

    if ((this->m_Flags & 0x800) == 0) {
        if ((this->m_Flags & 0x404) != 4) {
            return;
        }
        if (this->m_PFParms[this->m_ActiveProject].track_status == PATHTRACK_PAUSED) {
            return;
        }
        if (this->m_bSkipUpdate) {
            return;
        }
    }

    int status = 1;
    if (SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject] != nullptr) {
        status = SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]->track_status;
    }

    switch (this->m_MusicType) {
        case eMUSIC_TYPE_AMBIENCE:
            if (this->TestToPursuit() || this->TestToLicensed(true)) {
                return;
            }
            if ((this->m_Flags & 0x800) != 0) {
                return;
            }
            if (status == PATHTRACK_STOPPED) {
                if (this->m_CurPathEvent == this->m_PrevPathEvent) {
                    this->m_CurPathEvent = 0;
                }
                this->UpdateAmbience(t);
                this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
                return;
            }
            this->UpdateAmbience(t);
            if (AmbientCrossMap[this->m_nAmbientZone] != this->m_CurPathEvent) {
                this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
            }
            return;

        case eMUSIC_TYPE_LICENCED:
            if (this->TestToPursuit() || this->TestToAmbience()) {
                return;
            }
            if (this->TestToLicensed(false)) {
                if ((this->m_Flags & 0x800) != 0) {
                    return;
                }
                if (status != PATHTRACK_STOPPED) {
                    return;
                }
                this->StartLicensedMusic(0);
                return;
            }
            if ((this->m_Flags & 0x800) != 0) {
                return;
            }
            this->UpdateAmbience(t);
            this->StartAmbience(AmbientCrossMap[this->m_nAmbientZone]);
            return;

        case eMUSIC_TYPE_INTERACTIVE:
            if (FEDatabase->GetAudioSettings()->InteractiveMusicMode == 0 || g_pEAXSound->GetCurMusicVolume() == 0.0f) {
                PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
                if (this->TestToLicensed(true) || this->TestToAmbience()) {
                    return;
                }
                if (status != PATHTRACK_STOPPED) {
                    return;
                }
                this->m_CurPathEvent = 0;
                SoundAI *ai = SoundAI::Get();
                if (ai == nullptr || ai->IsMusicActive()) {
                    return;
                }
                this->StartInteractiveMusic(0x026E7282);
                return;
            }
            if (this->TestToAmbience()) {
                return;
            }
            if (status != PATHTRACK_STOPPED) {
                return;
            }
            this->m_CurPathEvent = 0;
            {
                SoundAI *ai = SoundAI::Get();
                if (ai == nullptr || ai->IsMusicActive()) {
                    return;
                }
                this->StartInteractiveMusic(0x026E7282);
            }
            return;

        default:
            return;
    }
}

void SFXObj_PFEATrax::UpdateAmbience(float t) {
    if (this->m_EATraxState == EATRAX_IG) {
        int newzone = 0;
        EAXCar *peaxcar_0 = g_pEAXSound->GetPlayerTunerCar(0);
        if (peaxcar_0 != nullptr && peaxcar_0->GetPhysCar() != nullptr && AmbientAccessor.IsValid()) {
            EAX_CarState *pcar = peaxcar_0->GetPhysCar();
            {
                const bVector2 *ppos = pcar->GetPosition2D();
                AmbientAccessor.CaptureData(ppos->x, ppos->y);
            }
            newzone = AmbientAccessor.GetDataInt(0);
        }
        if (this->m_nAmbientZone == newzone) {
            if (SFXCTL_Pathfinder::m_pPFParms[this->m_ActiveProject]->track_status != PATHTRACK_STOPPED) {
                return;
            }
        }
        if (static_cast<unsigned int>(newzone) <= 13) {
            this->m_nAmbientZone = newzone;
        }
        return;
    }

    {
        int oldzone = this->m_nAmbientZone;

        if (FEDatabase->IsRapSheetMode()) {
            this->m_nAmbientZone = 13;
        } else {
            switch (FEManager::Get()->GetGarageType()) {
                case GARAGETYPE_CAREER_SAFEHOUSE:
                    this->m_nAmbientZone = 9;
                    break;
                case GARAGETYPE_CUSTOMIZATION_SHOP:
                case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM:
                    this->m_nAmbientZone = 10;
                    break;
                case GARAGETYPE_CAR_LOT:
                    this->m_nAmbientZone = 12;
                    break;
                case GARAGETYPE_NONE:
                case GARAGETYPE_MAIN_FE:
                default:
                    this->m_nAmbientZone = 11;
                    break;
            }
        }
    }
}

// UNSOLVED
void SFXObj_PFEATrax::UpdatePursuitBreaker(float t) {
    if (g_pEAXSound->GetSndGameMode() == SND_PURSUITBREAKER && g_pEAXSound->GetPrevSndGameMode() != SND_PURSUITBREAKER) {
        if (this->m_FilterFade == nullptr) {
            this->m_FilterFade = new ("PATH5: Pursuit breaker sound filter fader", 0) cPathLine();
            this->m_FilterFade->AddStage(65535.0f, 630.0f, 1000, EQ_PWR_SQ);
            this->m_FilterFade->AddLinkedStage(1800.0f, 1000, EQ_PWR_SQ);
        }
    } else if (g_pEAXSound->GetSndGameMode() != SND_PURSUITBREAKER && g_pEAXSound->GetPrevSndGameMode() == SND_PURSUITBREAKER) {
        int curr_freq = this->m_FilterFade != nullptr ? this->m_FilterFade->iGetValue() : 1800;

        if (this->m_FilterFade == nullptr) {
            this->m_FilterFade = new ("PATH5: Pursuit breaker sound filter fader", 0) cPathLine();
        }

        if (this->m_FilterFade != nullptr) {
            this->m_FilterFade->ClearStages();
            this->m_FilterFade->AddStage(static_cast<float>(curr_freq), 65535.0f, 1000, EQ_PWR_SQ);
        }
    }

    if (this->m_FilterFade != nullptr && this->m_FilterFade->IsFinished() && g_pEAXSound->GetSndGameMode() != SND_PURSUITBREAKER) {
        delete this->m_FilterFade;
        this->m_FilterFade = nullptr;
    }

    if (this->m_FilterFade != nullptr) {
        this->m_FilterFade->Update(t);
    }

    this->m_FilterFreq = this->m_FilterFade != nullptr ? this->m_FilterFade->iGetValue() : 0xFFFF;
}

// UNSOLVED
void SFXObj_PFEATrax::UpdateParams(float t) {
    int status = (this->m_Flags & 0x800) == 0 ? this->m_PFParms[this->m_ActiveProject].track_status : -1;

    bool path_playing = false;
    // TODO enum values
    if (status - 5U < 2 || status == 2) {
        path_playing = true;
    }

    bool user_playing = false;

    if (this->m_PFParms[this->m_ActiveProject].track_status == 3 && (this->m_Flags & 0x800) == 0) {
        return;
    }

    if ((this->m_Flags & 0x10) != 0 && g_pEAXSound->GetSndGameMode() != SND_FRONTEND && (this->m_Flags & 0x802) == 0) {
        this->m_InteractiveProj = static_cast<eINTERACTIVE_PROJ_ID>((this->m_InteractiveProj + 1) & 3);
        SFXCTL_Pathfinder::SetCurInteractive(this->m_InteractiveProj);

        if (!g_pEAXSound->AreResourceLoadsPending()) {
            this->m_Flags &= ~0x10u;
            this->m_pSFXCTL_Pathfinder->DestroyTrack(&this->m_PFParms[1]);

            int index = gAEMSMgr.IsAssetInList(Attrib::StringKey(this->m_PFParms[1].mapfile));
            gAEMSMgr.UnloadSndData(index);

            switch (this->m_InteractiveProj) {
                case PF_INTERACTIVE_00:
                default:
                    this->m_PFParms[1].mapfile = "MW_Mus_1.mpf";
                    this->m_PFParms[1].musfile = "MW_Mus_1.mus";
                    break;
                case PF_INTERACTIVE_01:
                    this->m_PFParms[1].mapfile = "MW_Mus_2.mpf";
                    this->m_PFParms[1].musfile = "MW_Mus_2.mus";
                    break;
                case PF_INTERACTIVE_02:
                    this->m_PFParms[1].mapfile = "MW_Mus_3.mpf";
                    this->m_PFParms[1].musfile = "MW_Mus_3.mus";
                    break;
                case PF_INTERACTIVE_03:
                    this->m_PFParms[1].mapfile = "MW_Mus_4.mpf";
                    this->m_PFParms[1].musfile = "MW_Mus_4.mus";
                    break;
            }

            this->LoadAsset(Attrib::StringKey(this->m_PFParms[1].mapfile), SNDPATH_PATHFINDER, SDT_GENERIC_DATA, eBANK_SLOT_PATHFINDER, true);
        }
    }

    if (MUSICFLOW_DISPLAY != 0) {
        int y = static_cast<int>((WorldTimer - this->mT_ambienceStart).GetSeconds());
        int x = static_cast<int>(this->m_PFParms[this->m_ActiveProject].track_status == 3);
        if (x != 0 && t != 0.0f)
            x = static_cast<int>(x != 0);
    }

    if (path_playing || user_playing) {
        switch (this->m_MusicType) {
            case eMUSIC_TYPE_INTERACTIVE:
            case eMUSIC_TYPE_SPLASH:
                this->SetDMIX_Input(6, 0);
                this->SetDMIX_Input(7, 0x7FFF);
                this->SetDMIX_Input(8, 0);
                break;
            case eMUSIC_TYPE_LICENCED:
                this->SetDMIX_Input(6, 0x7FFF);
                this->SetDMIX_Input(7, 0);
                this->SetDMIX_Input(8, 0);
                break;
            case eMUSIC_TYPE_AMBIENCE:
                this->SetDMIX_Input(6, 0);
                this->SetDMIX_Input(7, 0);
                this->SetDMIX_Input(8, 0x7FFF);
                break;
        }
    } else {
        this->SetDMIX_Input(6, 0);
        this->SetDMIX_Input(7, 0);
        this->SetDMIX_Input(8, 0);
    }

    if (IsAudioStreamingEnabled != 0 && (this->m_Flags & 0x2000) == 0) {
        if (this->m_EATraxState == EATRAX_OFF) {
            if (GameFlowSndState[4] != 0 || GameFlowSndState[5] != 0 || GameFlowSndState[7] != 0 || GameFlowSndState[6] != 0 ||
                GameFlowSndState[11] != 0) {
                this->UpdateInGame(t);
            } else {
                this->m_EATraxState = this->GenEATraxState();
            }
        } else {
            this->UpdateInGame(t);
        }

        this->UpdatePursuitBreaker(t);
    }
}

void SFXObj_PFEATrax::ProcessUpdate() {
    if (IsAudioStreamingEnabled == 0) {
        return;
    }

#ifndef EA_BUILD_A124
    if (this->m_bClearSkipUpdate) {
        this->m_bClearSkipUpdate = false;
        this->m_bSkipUpdate = false;
    }
#endif

    if ((this->m_Flags & 0x800) == 0) {
        if ((this->m_Flags & 0x404) != 4) {
            return;
        }
        if (this->m_PFParms[this->m_ActiveProject].queue_next == 1 && (this->m_Flags & 4) != 0) {
            this->SendPathEvent();
            SNDSYS_service();
            goto mute_stream;
        }
        if (this->m_MusicType == eMUSIC_TYPE_LICENCED && (this->m_Flags & 0x40) == 0 && !this->m_bSkipUpdate) {
            if (this->m_CurPathEvent != 0x01C53FC7 && this->m_CurPathEvent != 0x01C3FA91) {
                if (gMoviePlayer == nullptr || static_cast<unsigned int>(gMoviePlayer->GetStatus() - 3) > 2) {
                    this->NotifyChyron();
                }
            }
        }
    }

    if (this->m_bSkipUpdate) {
    mute_stream:
        EAXS_StreamChannel *pch = g_pEAXSound->GetStreamManager()->GetStreamChannel(1);
        if (pch != nullptr) {
            pch->SetVol(0, false);
        }
        return;
    }

    bool path_playing;
    if ((this->m_Flags & 0x800) == 0) {
        path_playing = false;
        int status = this->m_PFParms[this->m_ActiveProject].track_status;
        if (status == PATHTRACK_PLAYING) {
            path_playing = true;
        } else if (status == PATHTRACK_QUEUEING) {
            path_playing = true;
        }
    } else {
        path_playing = false;
    }

    if (!path_playing) {
        return;
    }

    int vol = 0;
    if (this->m_EATraxState == EATRAX_IG) {
        switch (this->m_MusicType) {
            case eMUSIC_TYPE_LICENCED:
            case eMUSIC_TYPE_SPLASH:
                vol = (this->GetDMixOutput(1, DMX_VOL) * 100) >> 15;
                break;
            case eMUSIC_TYPE_INTERACTIVE:
                vol = (this->GetDMixOutput(3, DMX_VOL) * 100) >> 15;
                break;
            case eMUSIC_TYPE_AMBIENCE:
                vol = (this->GetDMixOutput(5, DMX_VOL) * 100) >> 15;
                break;
            default:
                vol = 0;
                break;
        }
    } else if (this->m_EATraxState == EATRAX_FE) {
        switch (this->m_MusicType) {
            case eMUSIC_TYPE_LICENCED:
            case eMUSIC_TYPE_SPLASH:
                vol = (this->GetDMixOutput(0, DMX_VOL) * 100) >> 15;
                break;
            case eMUSIC_TYPE_AMBIENCE:
                vol = (this->GetDMixOutput(4, DMX_VOL) * 100) >> 15;
                break;
            default:
                vol = 0;
                break;
        }
    }

    this->m_Volume = vol;
    if ((this->m_Flags & 0x800) == 0) {
        PATH_volume(this->m_PFParms[this->m_ActiveProject].PATH_TRACK, static_cast<signed char>(vol));
        SNDSYS_entercritical();
        SNDSTRM_lowpass(this->m_pSFXCTL_Pathfinder->GetHandle(this->m_ActiveProject), this->GetDMixOutput(9, DMX_FREQ));
        SNDSYS_leavecritical();
    }
}

void SFXObj_PFEATrax::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_PFParms[0].bAttached = false;
    this->m_PFParms[0].bdataloaded = false;
    this->m_PFParms[0].ramfile = nullptr;
    this->m_PFParms[0].pmapfile = nullptr;
    this->m_PFParms[0].mapfile = "MW_Music.mpf";
    this->m_PFParms[0].musfile = "MW_Music.mus";
    this->m_PFParms[0].projnum = -1;
    this->m_PFParms[0].curnodeparm = -1;
    this->m_PFParms[0].procflags = 2;
    this->m_PFParms[0].PATH_TRACK = PATH_TRACK(0, 0, 0);
    this->m_PFParms[0].PATH_VOICE = -1;
    this->m_PFParms[0].PATH_TRACKID = -1;
    this->m_PFParms[0].PATH_TRACK_BYTESPERSEC = 36216;

    this->m_PFParms[1].bAttached = false;
    this->m_PFParms[1].bdataloaded = false;
    this->m_PFParms[1].ramfile = nullptr;
    this->m_PFParms[1].pmapfile = nullptr;
    this->m_PFParms[1].procflags = 2;
    this->m_PFParms[1].mapfile = "MW_Mus_1.mpf";
    this->m_PFParms[1].musfile = "MW_Mus_1.mus";
    this->m_PFParms[1].PATH_TRACK = PATH_TRACK(0, 1, 0);
    this->m_PFParms[1].projnum = -1;
    this->m_PFParms[1].curnodeparm = -1;
    this->m_PFParms[1].PATH_VOICE = -1;
    this->m_PFParms[1].PATH_TRACKID = -1;
    this->m_PFParms[1].PATH_TRACK_BYTESPERSEC = 36216;
    this->m_EATraxState = this->GenEATraxState();
    this->m_Flags &= ~0x8;
}

void SFXObj_PFEATrax::SetupLoadData() {
    if (IsAudioStreamingEnabled != 0) {
        LoadAsset(Attrib::StringKey(this->m_PFParms[0].mapfile), SNDPATH_PATHFINDER, SDT_GENERIC_DATA, eBANK_SLOT_NONE, true);
        if (g_pEAXSound->GetPlayerMixMode() != EAXS3D_TWO_PLAYER_MIX) {
            LoadAsset(Attrib::StringKey(this->m_PFParms[1].mapfile), SNDPATH_PATHFINDER, SDT_GENERIC_DATA, eBANK_SLOT_PATHFINDER, true);
            this->m_Flags &= ~2;
        } else {
            this->m_Flags |= 2;
        }
    }
}

void SFXObj_PFEATrax::InitSFX() {
    SndBase::InitSFX();
    SFXCTL_Pathfinder::SetCurInteractive(m_InteractiveProj);
    if (IsAudioStreamingEnabled == 0) {
        return;
    }

    int assetlocation;
    if ((m_Flags & 8) == 0) {
        assetlocation = gAEMSMgr.IsAssetInList(Attrib::StringKey(m_PFParms[0].mapfile));
        if (assetlocation != -1) {
            m_PFParms[0].pmapfile = static_cast<char *>(g_SndAssetList[assetlocation].pmem);
            m_pSFXCTL_Pathfinder->InitPFParms(m_PFParms, 0, 0);
        }
    }
CHECK_INTERACTIVE_PROJECT:
    if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND || (m_Flags & 2) != 0) {
        if ((m_Flags & 2) != 0) {
            m_MusicType = eMUSIC_TYPE_LICENCED;
        }
        m_Flags |= 4;
    } else {
        m_MusicType = eMUSIC_TYPE_LICENCED;
        assetlocation = gAEMSMgr.IsAssetInList(Attrib::StringKey(m_PFParms[1].mapfile));
        if (assetlocation == -1) {
            return;
        }
        m_PFParms[1].pmapfile = g_SndAssetList[assetlocation].mBankSlot->MAINmemLocation;
        m_pSFXCTL_Pathfinder->InitPFParms(m_PFParms + 1, 1, 0);
        m_Flags |= 4;
        m_Flags &= ~8;
    }
}

void SFXObj_PFEATrax::GenNextMusicTrackID() {
    this->m_EATrax[this->m_EATraxState].PlayTrackIndex = -1;
    this->m_EATrax[this->m_EATraxState].TraxMask &= 0x0FFFFFFF;
    if (this->m_EATraxState == EATRAX_OFF || this->m_EATrax[this->m_EATraxState].TraxMask == 0) {
        return;
    }

    UserProfile *profile = FEDatabase->GetUserProfile(0);
    if (this->m_EATrax[this->m_EATraxState].PlayBits == 0 && this->m_EATrax[this->m_EATraxState].NumEnabledSongs != 0) {
        this->m_EATrax[this->m_EATraxState].PlayBits = this->m_EATrax[this->m_EATraxState].TraxMask & 0x0FFFFFFF;
        this->m_EATrax[this->m_EATraxState].LastPlaylistSong = -1;
    }

    if (this->m_EATrax[this->m_EATraxState].PBMode == 0) {
        int n = this->m_EATrax[this->m_EATraxState].LastPlaylistSong + 1;

        for (; n < g_MaxSongs; n++) {
            int nSongindex = profile->Playlist[n].SongIndex;

            if ((this->m_EATrax[this->m_EATraxState].PlayBits & (1 << nSongindex)) != 0) {
                this->m_EATrax[this->m_EATraxState].PlayTrackIndex = nSongindex;
                this->m_EATrax[this->m_EATraxState].PlayBits ^= 1 << nSongindex;
                if (this->m_EATrax[this->m_EATraxState].PlayBits == 0) {
                    this->m_EATrax[this->m_EATraxState].PlayBits = this->m_EATrax[this->m_EATraxState].TraxMask & 0x0FFFFFFF;
                    this->m_EATrax[this->m_EATraxState].PlayBits ^= 1 << n;
                }
                return;
            }
        }
        if (this->m_EATrax[this->m_EATraxState].NumEnabledSongs != 0) {
            this->m_EATrax[this->m_EATraxState].PlayBits = this->m_EATrax[this->m_EATraxState].TraxMask & 0x0FFFFFFF;
            this->m_EATrax[this->m_EATraxState].LastPlaylistSong = -1;
            this->GenNextMusicTrackID();
        }
        return;
    } else {
        int nrandcount = 0;
        int nSongNumber;
        int ncount;

        for (int npb = 0; npb < g_MaxSongs; npb++) {
            if ((this->m_EATrax[this->m_EATraxState].PlayBits & (1 << npb)) != 0) {
                nrandcount++;
            }
        }
        SoundRandomSeed = bGetTicker();
        nSongNumber = g_pEAXSound->Random(nrandcount);
        ncount = 0;
        for (int n = 0; n < g_MaxSongs; n++) {
            if ((this->m_EATrax[this->m_EATraxState].PlayBits & (1 << n)) != 0) {
                if (nSongNumber == ncount) {
                    this->m_EATrax[this->m_EATraxState].PlayTrackIndex = n;
                    this->m_EATrax[this->m_EATraxState].PlayBits ^= 1 << n;
                    if (this->m_EATrax[this->m_EATraxState].PlayBits == 0) {
                        this->m_EATrax[this->m_EATraxState].PlayBits = this->m_EATrax[this->m_EATraxState].TraxMask & 0x0FFFFFFF;
                        this->m_EATrax[this->m_EATraxState].PlayBits ^= 1 << n;
                    }
                    return;
                }
                ncount++;
            }
        }
    }
}

// TODO move

void SFXObj_PFEATrax::NotifyChyron() {
    if (this->m_EATrax[this->m_EATraxState].PlayTrackIndex < 0 ||
        this->m_EATrax[this->m_EATraxState].PlayTrackIndex > static_cast<int>(Songs.size())) {
        return;
    }

    Sound::stSongInfo *currSong = Songs[this->m_EATrax[this->m_EATraxState].PlayTrackIndex];
    SummonChyron(currSong->SongName, currSong->Artist, currSong->Album);
    this->m_Flags |= 0x40;
}

void SFXObj_PFEATrax::MessageStartPathfinder(const MControlPathfinder &message) {
    if (this->m_EATraxState == EATRAX_OFF) {
        this->Stop();
        return;
    } else if (IsAudioStreamingEnabled == 0) {
        return;
    }

    unsigned int event = message.GetPathEvent();
    if (m_MusicType == eMUSIC_TYPE_SPLASH && event == static_cast<unsigned int>(-1)) {
        return;
    }
    if (event == 0) {
        this->m_MusicType = GenMusicType();
    } else if (event == static_cast<unsigned int>(-1)) {
        if (0.0f < g_pEAXSound->GetCurMusicVolume() && (this->m_Flags & 0x800) == 0 && g_pEAXSound->GetCurAudioSettings()->EATraxMode != 0 &&
            this->m_EATrax[this->m_EATraxState].TraxMask != 0 && this->m_MusicType == eMUSIC_TYPE_LICENCED && !m_bSkipUpdate) {
            PATH_clearallevents(PATH_ALL_PROJECTS);
            PATH_stop(this->m_PFParms[this->m_ActiveProject].PATH_TRACK);
        }
    } else {
        if (this->m_EATrax[0].TraxMask == 0) {
            this->m_EATrax[0].TraxMask = 0x80000000;
        }
        PATH_clearallevents(PATH_ALL_PROJECTS);
        this->m_MusicType = GenMusicType();
        this->m_CurPathEvent = event;
        this->m_PrevPathEvent = event;
        this->StartLicensedMusic(event);
        this->m_bSkipUpdate = true;
#ifndef EA_BUILD_A124
        this->m_bClearSkipUpdate = false;
#endif
    }
}

// TODO move
enum ePFENTRIES {
    PF_ENDPAUSE_1 = 0,
    PF_LOSE = 1,
    PF_ELUDE = 2,
    PF_WIN = 3,
    PF_NEUTRAL = 4,
    PF_WIN_END = 5,
    PF_LOSE_END = 6,
    PF_ELUDE_END = 7,
    PF_END_PAUSE_2 = 8,
    PF_EVT_STOP = 9,
    PF_EVT_NEUTRAL = 10,
    PF_EVT_LOSE = 11,
    PF_EVT_WIN = 12,
    PF_EVT_ELUDE = 13,
    PF_EVT_ENDWIN = 14,
    PF_EVT_ENDBUSTED = 15,
    PF_EVT_STARTSCREEN = 16,
    PF_EVT_START = 17,
    PF_EVT_INITSWAP = 18,
    PF_EVT_SWAP1 = 19,
    PF_EVT_SWAP2 = 20,
    PF_XMAP_MAX = 21,
};

extern int PFXMAP[4][PF_XMAP_MAX][2];

void SFXObj_PFEATrax::MessageSendPathEvent(const MControlPathfinder &message) {
    if (IsAudioStreamingEnabled == 0) {
        return;
    }

    unsigned int curr_event = m_CurPathEvent;
    int ntmp = static_cast<int>(message.GetPathEvent());
    if (this->m_MusicType == eMUSIC_TYPE_LICENCED || this->m_MusicType == eMUSIC_TYPE_AMBIENCE || this->m_MusicType == eMUSIC_TYPE_SPLASH) {
        if (ntmp == 9) {
            this->m_CurPathEvent = 0x01C3FA91;
        } else if (ntmp == 0x10) {
            this->m_CurPathEvent = 0x01C53FC7;
        } else if (ntmp > 9) {
            return;
        }
    } else {
        this->m_CurPathEvent = PFXMAP[this->m_InteractiveProj][ntmp][0];
    }
    if (this->m_CurPathEvent != curr_event) {
        this->m_PrevPathEvent = curr_event;
        this->m_PFParms[this->m_ActiveProject].queue_next = 1;
        if (this->m_MusicType == eMUSIC_TYPE_AMBIENCE) {
            PATH_pause(this->m_PFParms[this->m_ActiveProject].PATH_TRACK, 0);
        }
        this->SendPathEvent();
        int nproj;

        if (ntmp == PF_EVT_SWAP1 || ntmp == PF_EVT_SWAP2) {
            this->SwapInteractiveProjects();
        }
    }
}

void SFXObj_PFEATrax::MessageSendPathControl(const MControlPathfinder &message) {
    if (IsAudioStreamingEnabled != 0 && this->m_MusicType == eMUSIC_TYPE_INTERACTIVE && message.GetPathControl() != this->m_CurIntensity) {
        this->m_PrevIntensity = m_CurIntensity;
        this->m_CurIntensity = static_cast<int8>(message.GetPathControl());
        PATH_control(this->m_PFParms[this->m_ActiveProject].PATH_TRACKID, static_cast<unsigned int>(this->m_CurIntensity));
    }
}

void SFXObj_PFEATrax::MessagePartUpdate(const MControlPathfinder &message) {
    this->m_CurPart = message.GetPartID();
#ifndef EA_BUILD_A124
    this->m_bClearSkipUpdate = true;
#endif
    if (this->m_CurPart > 8) {
        return;
    }

    if (this->m_CurPart < -1) {
        return;
    }

    if (this->m_Flags & 0x100) {
        this->m_Flags &= ~0x100;
    }
}

void SFXObj_PFEATrax::MessagePerpBusted(const MPerpBusted &message) {}

void SFXObj_PFEATrax::MessageInteractiveDone(const MControlPathfinder &message) {
    this->mT_ambienceStart = WorldTimer;
}

#ifndef EA_BUILD_A124
void SFXObj_PFEATrax::MessageSwapInteractive(const MControlPathfinder &message) {
    this->m_CurPathEvent = PFXMAP[m_InteractiveProj][PF_EVT_INITSWAP][0];
    this->m_PrevPathEvent = 0;
    this->m_PFParms[m_ActiveProject].queue_next = 1;
}
#endif
