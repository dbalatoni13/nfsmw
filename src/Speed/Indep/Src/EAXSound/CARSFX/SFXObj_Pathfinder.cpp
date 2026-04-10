#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_PFEATrax.hpp"

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMusicFlow.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/EAXSound/EAXAemsManager.h"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"

#include <vector>

struct SongInfoList : public std::vector<Sound::stSongInfo *> {};

extern int PATH_stop(int tracks);
extern int PATH_pause(int tracks, unsigned char pause);
extern int PATH_event(int tracks, unsigned int event);
extern int PATH_volume(int tracks, signed char volume);
extern int PATH_setnamedvalue(int tracks, char *name, int value);
extern int PATH_control(int tracks, unsigned int controller);
extern void PATH_clearallevents(int mask);
extern int GameFlowSndState[];
extern int PURSUIT_TO_LIC_DELAY;
extern int PFXMAP[4][21][2];
extern unsigned int AmbientCrossMap[14];
extern void InitializeEATrax(bool breset);
extern int g_MaxSongs;
extern unsigned int g_ActiveSFXStates;
extern unsigned int SoundRandomSeed;
extern SongInfoList Songs;
extern void SummonChyron(char *title, char *artist, char *album);
extern void SNDSYS_service();
extern ParameterAccessor AmbientAccessor;
extern MoviePlayer *gMoviePlayer;

SndBase::TypeInfo *SFXObj_Pathfinder::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_Pathfinder::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_Pathfinder::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXObj_Pathfinder::GetStaticTypeInfo()->typeName, false) SFXObj_Pathfinder();
    }

    return new (SFXObj_Pathfinder::GetStaticTypeInfo()->typeName, true) SFXObj_Pathfinder();
}

SFXObj_Pathfinder::SFXObj_Pathfinder()
    : CARSFX() {
    m_pSFXCTL_Pathfinder = nullptr;
    m_Flags = 0x40;
    m_PFParms[0].pmapfile = nullptr;
    m_PFParms[1].pmapfile = nullptr;
}

int SFXObj_Pathfinder::GetController(int Index) {
    return Index == 0 ? 0 : -1;
}

void SFXObj_Pathfinder::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 0) {
        m_pSFXCTL_Pathfinder = static_cast<SFXCTL_Pathfinder *>(psfxctl);
    }
}

SndBase::TypeInfo *SFXObj_PFEATrax::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_PFEATrax::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_PFEATrax::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXObj_PFEATrax::GetStaticTypeInfo()->typeName, false) SFXObj_PFEATrax();
    }

    return new (SFXObj_PFEATrax::GetStaticTypeInfo()->typeName, true) SFXObj_PFEATrax();
}

SFXObj_PFEATrax::SFXObj_PFEATrax()
    : SFXObj_Pathfinder() {
    m_FilterFade = nullptr;
    m_Volume = 0;
    m_CurPart = -1;
    m_PartStart = Timer(0);
    mT_ambienceStart = Timer();
    m_FilterFreq = 0xFFFF;
    mMsgControlPF5 = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessageStartPathfinder, UCrc32("Pathfinder5"), 0);
    mMsgControlInitSongslist = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessageInitSongsList, UCrc32("EATraxInit"), 0);
    mMsgSendPathEvent = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessageSendPathEvent, UCrc32("Event"), 0);
    mMsgSendPathControl = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessageSendPathControl, UCrc32("Control"), 0);
    mMsgNotifyPartUpdate = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessagePartUpdate, UCrc32("PartUpdate"), 0);
    mMsgPerpBusted = Hermes::Handler::Create<MPerpBusted, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessagePerpBusted, UCrc32(0x20D60DBF), 0);
    mMsgInteractiveDone = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessageInteractiveDone, UCrc32("InteractiveDone"), 0);
    mMsgSwapInteractive = Hermes::Handler::Create<MControlPathfinder, SFXObj_PFEATrax, SFXObj_PFEATrax>(
        this, &SFXObj_PFEATrax::MessageSwapInteractive, UCrc32("Swap"), 0);
    m_EATraxState = EATRAX_OFF;
    m_CurPathEvent = 0;
    m_PrevPathEvent = static_cast<unsigned int>(-1);
    m_nAmbientZone = -1;
    m_AmbZoneVol = 0;
    m_CurIntensity = 0;
    m_PrevIntensity = 0;
    m_ActiveProject = PF_LICENSED_MUSIC;
    m_PrevActiveProject = PF_PROJECTRESET;
    m_MusicType = eMUSIC_TYPE_SPLASH;
    m_PrevMusicType = eMUSIC_TYPE_SPLASH;
    m_Flags = 0x40;
    m_InteractiveProj = static_cast<eINTERACTIVE_PROJ_ID>((bRandom(4) + PF_INTERACTIVE_01) & PF_INTERACTIVE_03);
    m_FEPreviewEvent = 0;
    m_FEPreviewIndex = -1;
    m_bSkipUpdate = false;
    m_bClearSkipUpdate = false;
    mT_ambienceStart = Timer(0);
    m_bPathFAILED = false;
}

SFXObj_PFEATrax::~SFXObj_PFEATrax() {
    if (mMsgControlPF5) {
        Hermes::Handler::Destroy(mMsgControlPF5);
    }
    if (mMsgControlInitSongslist) {
        Hermes::Handler::Destroy(mMsgControlInitSongslist);
    }
    if (mMsgSendPathEvent) {
        Hermes::Handler::Destroy(mMsgSendPathEvent);
    }
    if (mMsgSendPathControl) {
        Hermes::Handler::Destroy(mMsgSendPathControl);
    }
    if (mMsgNotifyPartUpdate) {
        Hermes::Handler::Destroy(mMsgNotifyPartUpdate);
    }
    if (mMsgPerpBusted) {
        Hermes::Handler::Destroy(mMsgPerpBusted);
    }
    if (mMsgInteractiveDone) {
        Hermes::Handler::Destroy(mMsgInteractiveDone);
    }
    if (mMsgSwapInteractive) {
        Hermes::Handler::Destroy(mMsgSwapInteractive);
    }

    Destroy();
}

void SFXObj_PFEATrax::RestartRace() {
    if (static_cast<unsigned int>(m_MusicType) < eMUSIC_TYPE_AMBIENCE) {
        PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
        PATH_clearallevents(0x0F000000);
        m_MusicType = eMUSIC_TYPE_LICENCED;
        m_bPathFAILED = false;
    }

    mT_ambienceStart = Timer(0);
}

void SFXObj_PFEATrax::SendPathEvent() {
    if (m_EATraxState != EATRAX_UNINIT && (m_Flags & 4) != 0) {
        if (m_CurPathEvent != m_PrevPathEvent && (m_Flags & 0x800) == 0) {
            {
                int status;

                m_bPathFAILED = false;
                m_PrevPathEvent = m_CurPathEvent;
                status = PATH_event(m_PFParms[m_ActiveProject].PATH_TRACK, m_CurPathEvent);
                if (status != 0) {
                    m_bPathFAILED = true;
                    m_PrevPathEvent = 0;
                } else {
                    PATH_volume(m_PFParms[m_ActiveProject].PATH_TRACK, '\0');
                    if (m_pSFXCTL_Pathfinder) {
                        SNDSYS_entercritical();
                        SNDSTRM_lowpass(m_pSFXCTL_Pathfinder->GetHandle(m_ActiveProject), GetDMixOutput(9, DMX_FREQ));
                        SNDSYS_leavecritical();
                    }
                    m_PFParms[m_ActiveProject].queue_next = 0;
                    if (m_CurPathEvent == 0x01C53FC7) {
                        m_PrevMusicType = m_MusicType;
                        m_MusicType = eMUSIC_TYPE_SPLASH;
                    }
                }
            }
        }
    }
}

void SFXObj_PFEATrax::SwapInteractiveProjects() {
    m_Flags |= 0x18;
}

void SFXObj_PFEATrax::StartLicensedMusic(unsigned int PathEvent) {
    eSndGameMode esgm;

    if ((m_Flags & 0x800) == 0) {
        if (m_ActiveProject != PF_LICENSED_MUSIC) {
            PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
            m_ActiveProject = PF_LICENSED_MUSIC;
            return;
        }
        m_ActiveProject = PF_LICENSED_MUSIC;
        if (m_PrevActiveProject != PF_LICENSED_MUSIC) {
            if (m_PrevActiveProject != PF_PROJECTRESET && SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject] &&
                SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject]->bAttached) {
                PATH_stop(m_PFParms[m_PrevActiveProject].PATH_TRACK);
                m_pSFXCTL_Pathfinder->DetachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject]);
            }
            m_pSFXCTL_Pathfinder->AttachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]);
            m_PrevActiveProject = m_ActiveProject;
            if ((m_Flags & 2) == 0) {
                SwapInteractiveProjects();
            }
        }
    }
    esgm = g_pEAXSound->GetSndGameMode();
    if (esgm != SND_FRONTEND && esgm != SND_FREEROAM) {
        if ((m_Flags & 1) == 0) {
            if (esgm != SND_CHALLENGERACE) {
                return;
            }
        } else if (esgm != SND_CHALLENGERACE && (m_Flags & 0x200) != 0) {
            return;
        }
        if (esgm == SND_CHALLENGERACE && g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode != 0) {
            return;
        }
    }
    m_PrevMusicType = m_MusicType;
    m_MusicType = eMUSIC_TYPE_LICENCED;
    if ((m_Flags & 0x800) == 0) {
        {
            int PathEventToUse;
            int status;

            PATH_clearallevents(0x0F000000);
            m_PrevPathEvent = m_CurPathEvent;
            if (m_EATrax[m_EATraxState].PlayTrackIndex != -1) {
                m_EATrax[m_EATraxState].LastPlaylistSong = m_EATrax[m_EATraxState].PlayTrackIndex;
            }
            PathEventToUse = 0;
            if (PathEvent != 0) {
                bool foundevent;

                foundevent = false;
                {
                    int n;

                    n = 0;
                    while (n < g_MaxSongs) {
                        {
                            stSongInfo *CurSong;

                            CurSong = Songs[n];
                            if (CurSong->PathEvent == static_cast<int>(PathEvent)) {
                                foundevent = true;
                                m_EATrax[m_EATraxState].PlayTrackIndex = n;
                                {
                                    stSongInfo *CurSong;

                                    CurSong = Songs[m_EATrax[m_EATraxState].PlayTrackIndex];
                                    PathEventToUse = CurSong->PathEvent;
                                }
                                m_Flags &= ~0x40u;
                                m_PrevPathEvent = 0;
                            }
                        }
                        ++n;
                    }
                }
                if (!foundevent) {
                    m_EATrax[m_EATraxState].PlayTrackIndex = 0;
                    m_EATrax[m_EATraxState].PlayBits ^= 1;
                    PathEventToUse = static_cast<int>(PathEvent);
                }
            } else {
                stSongInfo *CurSong;

                GenNextMusicTrackID();
                if (m_EATrax[m_EATraxState].PlayTrackIndex == -1) {
                    return;
                }
                CurSong = Songs[m_EATrax[m_EATraxState].PlayTrackIndex];
                PathEventToUse = CurSong->PathEvent;
                m_Flags &= ~0x40u;
                m_PrevPathEvent = 0;
            }
            m_CurPathEvent = static_cast<unsigned int>(PathEventToUse);
            m_PFParms[0].queue_next = 1;
            status = 1;
            if (SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]) {
                status = SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]->track_status;
            }
            if (status == 3) {
                PATH_pause(m_PFParms[m_ActiveProject].PATH_TRACK, '\0');
            }
            if (m_CurPathEvent != m_PrevPathEvent) {
                m_bSkipUpdate = true;
                m_bClearSkipUpdate = false;
            }
        }
    }
}

void SFXObj_PFEATrax::StartAmbience(unsigned int PathEvent) {
    int status;
    EAXS_StreamChannel *pch;

    m_PrevMusicType = m_MusicType;
    m_MusicType = eMUSIC_TYPE_AMBIENCE;
    PATH_clearallevents(0x0F000000);
    m_ActiveProject = PF_LICENSED_MUSIC;
    if (m_PrevActiveProject != PF_LICENSED_MUSIC) {
        if (m_PrevActiveProject != PF_PROJECTRESET && SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject] &&
            SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject]->bAttached) {
            m_pSFXCTL_Pathfinder->DetachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject]);
        }
        m_pSFXCTL_Pathfinder->AttachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]);
        m_PrevActiveProject = m_ActiveProject;
    }
    status = 1;
    if (SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]) {
        status = SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]->track_status;
    }
    if (status == 3) {
        PATH_pause(m_PFParms[m_ActiveProject].PATH_TRACK, '\0');
    }
    m_PrevPathEvent = m_CurPathEvent;
    m_PFParms[0].queue_next = 1;
    m_CurPathEvent = PathEvent;
    m_bSkipUpdate = true;
    m_bClearSkipUpdate = false;
    pch = g_pEAXSound->GetStreamManager()->GetStreamChannel(1);
    if (pch) {
        pch->SetVol(0, false);
    }
}

void SFXObj_PFEATrax::StartInteractiveMusic(unsigned int PathEvent) {
    int status;

    if ((m_Flags & 0x80C) == 4) {
        m_PrevMusicType = m_MusicType;
        m_MusicType = eMUSIC_TYPE_INTERACTIVE;
        PATH_clearallevents(0x0F000000);
        status = 1;
        if (SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]) {
            status = SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]->track_status;
        }
        if (status == 3) {
            PATH_pause(m_PFParms[m_ActiveProject].PATH_TRACK, '\0');
        }
        m_ActiveProject = PF_INTERACTIVE_MUSIC;
        if (m_PrevActiveProject != PF_INTERACTIVE_MUSIC) {
            if (m_PrevActiveProject != PF_PROJECTRESET && SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject] &&
                SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject]->bAttached) {
                PATH_stop(m_PFParms[m_PrevActiveProject].PATH_TRACK);
                m_pSFXCTL_Pathfinder->DetachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[m_PrevActiveProject]);
            }
            m_pSFXCTL_Pathfinder->AttachStreamInstance(SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]);
            m_PrevActiveProject = m_ActiveProject;
        }
        m_PFParms[1].queue_next = 1;
        m_PrevPathEvent = m_CurPathEvent;
        m_CurPathEvent = PathEvent;
        MNotifyMusicFlow(PathEvent).Send(UCrc32("Init"));
        m_Flags |= 0x100;
    }
}

void SFXObj_PFEATrax::Stop() {
    m_EATraxState = EATRAX_OFF;
    PATH_clearallevents(0x0F000000);
    PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
    m_PFParms[m_ActiveProject].queue_next = 0;
}

void SFXObj_PFEATrax::Destroy() {
    PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
    m_PFParms[m_ActiveProject].queue_next = 0;
    m_EATraxState = EATRAX_UNINIT;
    if ((m_Flags & 2) == 0 && g_pEAXSound->GetSndGameMode() != SND_FRONTEND) {
        int np;
        int index;

        index = gAEMSMgr.IsAssetInList(Attrib::StringKey(m_PFParms[1].mapfile));
        gAEMSMgr.UnloadSndData(index);
    }
    if (m_FilterFade) {
        delete m_FilterFade;
    }
    m_FilterFade = nullptr;
    SNDSYS_service();
}

void InitializeEATrax(bool breset) {
    int songindex;
    int playability;
    JukeboxEntry *playlist;
    int n;

    n = 0;
    SFXObj_PFEATrax::m_EATrax[0].PBMode = FEDatabase->GetAudioSettings()->PlayState;
    SFXObj_PFEATrax::m_EATrax[0].TraxMask = 0;
    SFXObj_PFEATrax::m_EATrax[1].PBMode = FEDatabase->GetAudioSettings()->PlayState;
    SFXObj_PFEATrax::m_EATrax[1].NumEnabledSongs = 0;
    SFXObj_PFEATrax::m_EATrax[1].TraxMask = 0;
    SFXObj_PFEATrax::m_EATrax[0].NumEnabledSongs = 0;
    if (g_MaxSongs > 0) {
        playlist = FEDatabase->GetUserProfile(0)->Playlist;
        do {
            songindex = playlist[n].SongIndex;
            playability = playlist[n].PlayabilityField;
            if (playability == 1) {
                SFXObj_PFEATrax::m_EATrax[0].TraxMask |= 1 << (songindex & 0x1F);
                SFXObj_PFEATrax::m_EATrax[0].NumEnabledSongs++;
            } else if (playability > 1) {
                if (playability == 2) {
                    SFXObj_PFEATrax::m_EATrax[1].TraxMask |= 1 << (songindex & 0x1F);
                    SFXObj_PFEATrax::m_EATrax[1].NumEnabledSongs++;
                } else if (playability == 3) {
                    SFXObj_PFEATrax::m_EATrax[0].TraxMask |= 1 << (songindex & 0x1F);
                    SFXObj_PFEATrax::m_EATrax[0].NumEnabledSongs++;
                    SFXObj_PFEATrax::m_EATrax[1].TraxMask |= 1 << (songindex & 0x1F);
                    SFXObj_PFEATrax::m_EATrax[1].NumEnabledSongs++;
                }
            }
            n++;
        } while (n < g_MaxSongs);
    }
    SFXObj_PFEATrax::m_EATrax[0].PlayBits = SFXObj_PFEATrax::m_EATrax[0].TraxMask;
    SFXObj_PFEATrax::m_EATrax[1].PlayBits = SFXObj_PFEATrax::m_EATrax[1].TraxMask;
    if (breset) {
        SFXObj_PFEATrax::m_EATrax[1].LastPlaylistSong = -1;
        SFXObj_PFEATrax::m_EATrax[0].LastPlaylistSong = -1;
    }
}

void SFXObj_PFEATrax::MessageInitSongsList(const MControlPathfinder &message) {
    unsigned int utype;

    utype = message.GetPathEvent();
    if (utype == static_cast<unsigned int>(-1)) {
        PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
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
    if (m_EATraxState == EATRAX_IG) {
        if (FEDatabase->GetAudioSettings()->IGMusicVol > 0.0f) {
            if ((m_Flags & 0x800) == 0) {
                {
                    bool pursuitisactive;
                    SoundAI *ai;

                    pursuitisactive = false;
                    ai = SoundAI::Get();
                    if (ai &&
                        (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching)) {
                        pursuitisactive = true;
                    }

                    if (pursuitisactive) {
                        return eMUSIC_TYPE_INTERACTIVE;
                    }
                }
                return eMUSIC_TYPE_LICENCED;
            }

            return eMUSIC_TYPE_LICENCED;
        }
    } else if (m_EATraxState == EATRAX_FE) {
        if (FEDatabase->GetAudioSettings()->FEMusicVol > 0.0f) {
            return eMUSIC_TYPE_LICENCED;
        }
    } else if (m_EATraxState != EATRAX_OFF) {
        return eMUSIC_TYPE_AMBIENCE;
    }

    return static_cast<eMUSIC_TYPE>(static_cast<unsigned char>(((m_Flags & 0x800) == 0) << 1));
}

bool SFXObj_PFEATrax::TestToPursuit() {
    bool pursuit_exists;
    bool pursuit_active;
    SoundAI *ai;

    if (g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode != 0 && g_pEAXSound->GetCurMusicVolume() != 0.0f &&
        (m_Flags & 0x802) == 0) {
        pursuit_active = false;
        pursuit_exists = false;
        ai = SoundAI::Get();
        if (ai) {
            pursuit_exists = true;
        }

        if (pursuit_exists &&
            (ai->GetPursuitState() == SoundAI::kActive || ai->GetPursuitState() == SoundAI::kSearching)) {
            pursuit_active = true;
        }

        if (!pursuit_active) {
            return false;
        }

        m_CurPathEvent = 0;
        StartInteractiveMusic(0x026E7282);
        return true;
    }

    return false;
}

bool SFXObj_PFEATrax::TestToLicensed(bool bstart) {
    const GRaceParameters *raceparms;
    float t_playing_ambience;

    if (m_bSkipUpdate || (m_Flags & 0x800)) {
        return false;
    }

    if (g_pEAXSound->GetCurMusicVolume() > 0.0f && g_pEAXSound->GetCurAudioSettings()->EATraxMode == 1 &&
        m_EATrax[m_EATraxState].TraxMask != 0) {
        if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
            if (FEDatabase->IsRapSheetMode()) {
                return false;
            }

            if (bstart) {
                PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
                StartLicensedMusic(0);
                return true;
            }

            return true;
        } else {
            if (GRaceStatus::Exists()) {
                raceparms = GRaceStatus::Get().GetRaceParameters();
                if (raceparms && raceparms->GetIsLoaded() == true) {
                    if (raceparms->GetIsDDayRace()) {
                        return false;
                    }

                    if (GRaceStatus::IsDragRace()) {
                        return false;
                    }
                }
            }

            if (m_MusicType == eMUSIC_TYPE_AMBIENCE) {
                if (mT_ambienceStart > Timer(0)) {
                    t_playing_ambience = (WorldTimer - mT_ambienceStart).GetSeconds();
                } else {
                    t_playing_ambience = 0.0f;
                }

                if (t_playing_ambience <= static_cast<float>(PURSUIT_TO_LIC_DELAY)) {
                    return false;
                }

                if (bstart) {
                    StartLicensedMusic(0);
                }

                mT_ambienceStart = Timer(0);

                return true;
            }

            if (bstart) {
                StartLicensedMusic(0);
            }
        }

        return true;
    }

    if (m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
        if (g_pEAXSound->GetCurMusicVolume() > 0.0f && g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode == 0) {
            PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
            if (g_pEAXSound->GetCurAudioSettings()->EATraxMode != 0) {
                m_CurPathEvent = 0;
                StartLicensedMusic(0);
                return true;
            }
        }
    } else if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
        if (FEDatabase->IsRapSheetMode()) {
            return false;
        }

        if (g_pEAXSound->GetCurMusicVolume() > 0.0f && g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0) {
            return false;
        }
    }

    return false;
}

bool SFXObj_PFEATrax::TestToAmbience() {
    if ((m_Flags & 0x800) != 0) {
        return false;
    }
    UpdateAmbience(0.1f);
    do {
        if (m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
            {
                bool interactive_on;
                SoundAI *ai;

                interactive_on = true;
                ai = SoundAI::Get();
                if (!ai || !ai->IsMusicActive() || g_pEAXSound->GetCurMusicVolume() <= 0.0f ||
                    g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode <= 0) {
                    interactive_on = false;
                }
                if (interactive_on) {
                    break;
                }
            }
        } else if (m_MusicType == eMUSIC_TYPE_LICENCED) {
            if (g_pEAXSound->GetSndGameMode() == SND_FRONTEND) {
                if (g_pEAXSound->GetCurMusicVolume() == 0.0f || g_pEAXSound->GetCurAudioSettings()->EATraxMode == 0 ||
                    m_EATrax[m_EATraxState].TraxMask == 0) {
                    PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
                } else {
                    {
                        unsigned int fegm;

                        fegm = FEDatabase->GetGameMode();
                        if ((fegm & 0x200) == 0) {
                            break;
                        }
                    }
                    if (g_pEAXSound->GetCurMusicVolume() != 0.0f && g_pEAXSound->GetCurAudioSettings()->EATraxMode != 0 &&
                        m_EATrax[m_EATraxState].TraxMask != 0) {
                        if (FEDatabase->IsRapSheetMode()) {
                            int ntmp;

                            ntmp = (m_InteractiveProj + PF_INTERACTIVE_01) & PF_INTERACTIVE_03;
                            if (ntmp == 3) {
                                ntmp = 2;
                            }
                            PATH_setnamedvalue(m_PFParms[0].PATH_TRACK, "rapsheet", ntmp + 1);
                        }
                    }
                }
            } else {
                if (g_pEAXSound->GetCurMusicVolume() != 0.0f && g_pEAXSound->GetCurAudioSettings()->EATraxMode != 0 &&
                    m_EATrax[m_EATraxState].TraxMask != 0) {
                    break;
                }
                PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
            }
        } else {
            if (g_pEAXSound->GetCurMusicVolume() != 0.0f && g_pEAXSound->GetCurAudioSettings()->InteractiveMusicMode != 0) {
                break;
            }
            PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
        }

        UpdateAmbience(0.1f);
        StartAmbience(AmbientCrossMap[m_nAmbientZone]);
        return true;
    } while (0);

    return false;
}

void SFXObj_PFEATrax::UpdateInGame(float t) {
    int status;

    if (m_bPathFAILED) {
        return;
    }
    if ((g_ActiveSFXStates & 1) != 0) {
        return;
    }
    if ((m_Flags & 0x800) == 0) {
        if ((m_Flags & 0x404) != 4) {
            return;
        }
        if (m_PFParms[m_ActiveProject].track_status == 3) {
            return;
        }
        if (m_bSkipUpdate) {
            return;
        }
    }
    status = 1;
    if (SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]) {
        status = SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]->track_status;
    }
    if (m_MusicType != eMUSIC_TYPE_INTERACTIVE) {
        if (m_MusicType > eMUSIC_TYPE_INTERACTIVE) {
            if (m_MusicType == eMUSIC_TYPE_AMBIENCE) {
                if (TestToPursuit()) {
                    return;
                }
                if (TestToLicensed(true)) {
                    return;
                }
                if ((m_Flags & 0x800) != 0) {
                    return;
                }
                if (status == 1) {
                    if (m_CurPathEvent == m_PrevPathEvent) {
                        m_CurPathEvent = 0;
                    }
                    UpdateAmbience(t);
                    StartAmbience(AmbientCrossMap[m_nAmbientZone]);
                    return;
                }
                UpdateAmbience(t);
                if (AmbientCrossMap[m_nAmbientZone] == m_CurPathEvent) {
                    return;
                }
                StartAmbience(AmbientCrossMap[m_nAmbientZone]);
            }
        } else if (m_MusicType == eMUSIC_TYPE_LICENCED) {
            if (TestToPursuit()) {
                return;
            }
            if (TestToAmbience()) {
                return;
            }
            if (TestToLicensed(false)) {
                if ((m_Flags & 0x800) != 0) {
                    return;
                }
                if (status != 1) {
                    return;
                }
                StartLicensedMusic(0);
            } else {
                if ((m_Flags & 0x800) != 0) {
                    return;
                }
                UpdateAmbience(t);
                StartAmbience(AmbientCrossMap[m_nAmbientZone]);
                return;
            }
        }
    } else {
        if (FEDatabase->GetAudioSettings()->InteractiveMusicMode == 0 || g_pEAXSound->GetCurMusicVolume() == 0.0f) {
            PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
            if (TestToLicensed(true)) {
                return;
            }
            if (TestToAmbience()) {
                return;
            }
            if (status != 1) {
                return;
            }
            {
                SoundAI *ai;

                m_CurPathEvent = 0;
                ai = SoundAI::Get();
                if (!ai) {
                    return;
                }
                if (ai->IsMusicActive()) {
                    return;
                }
            }
            StartInteractiveMusic(0x026E7282);
        } else {
            if (TestToAmbience()) {
                return;
            }
            if (status != 1) {
                return;
            }
            {
                SoundAI *ai;

                m_CurPathEvent = 0;
                ai = SoundAI::Get();
                if (!ai) {
                    return;
                }
                if (ai->IsMusicActive()) {
                    return;
                }
            }
            StartInteractiveMusic(0x026E7282);
            return;
        }
    }
}

void SFXObj_PFEATrax::UpdateAmbience(float t) {
    if (m_EATraxState == EATRAX_IG) {
        int newzone;
        EAXCar *peaxcar_0;

        newzone = 0;
        peaxcar_0 = g_pEAXSound->GetPlayerTunerCar(0);
        if (peaxcar_0) {
            EAX_CarState *pcar;

            pcar = peaxcar_0->GetPhysCar();
            if (pcar && AmbientAccessor.IsValid()) {
                const bVector2 *ppos;

                ppos = pcar->GetPosition2D();
                AmbientAccessor.CaptureData(ppos->x, ppos->y);
                newzone = AmbientAccessor.GetDataInt(0);
            }
        }
        if ((m_nAmbientZone == newzone) && SFXCTL_Pathfinder::m_pPFParms[m_ActiveProject]->track_status != 1) {
            return;
        }
        if (static_cast<unsigned int>(newzone) > 0xD) {
            return;
        }
        m_nAmbientZone = newzone;
    } else {
        int oldzone;

        oldzone = 0xD;
        if ((FEDatabase->GetGameMode() & 0x200) == 0) {
            oldzone = 0xC;
            if (!FEDatabase->IsRapSheetMode()) {
                oldzone = 9;
                if ((FEDatabase->GetGameMode() & 0x10000) == 0) {
                    oldzone = 0xB;
                }
            }
        }
        m_nAmbientZone = oldzone;
    }
}

void SFXObj_PFEATrax::UpdatePursuitBreaker(float t) {
    if (g_pEAXSound->GetSndGameMode() == SND_PURSUITBREAKER &&
        g_pEAXSound->GetPrevSndGameMode() != SND_PURSUITBREAKER) {
        if (!m_FilterFade) {
            m_FilterFade = new ("PATH5: Pursuit breaker sound filter fader", 0) cPathLine();
            m_FilterFade->AddStage(65535.0f, 630.0f, 1000, EQ_PWR_SQ);
            m_FilterFade->AddLinkedStage(1800.0f, 1000, EQ_PWR_SQ);
        }
    } else if (g_pEAXSound->GetSndGameMode() != SND_PURSUITBREAKER &&
               g_pEAXSound->GetPrevSndGameMode() == SND_PURSUITBREAKER) {
        {
            int curr_freq;

            if (!m_FilterFade) {
                curr_freq = 0x708;
                m_FilterFade = new ("PATH5: Pursuit breaker sound filter fader", 0) cPathLine();
                if (!m_FilterFade) {
                    m_FilterFreq = 0xFFFF;
                    return;
                }
            } else {
                curr_freq = m_FilterFade->iGetValue();
            }
            m_FilterFade->ClearStages();
            m_FilterFade->AddStage(static_cast<float>(curr_freq), 65535.0f, 1000, EQ_PWR_SQ);
        }
    }
    if (m_FilterFade && m_FilterFade->IsFinished() && g_pEAXSound->GetSndGameMode() != SND_PURSUITBREAKER) {
        delete m_FilterFade;
        m_FilterFade = nullptr;
    }
    if (m_FilterFade) {
        m_FilterFade->Update(t);
    }
    if (m_FilterFade) {
        m_FilterFreq = m_FilterFade->iGetValue();
    } else {
        m_FilterFreq = 0xFFFF;
    }
}

void SFXObj_PFEATrax::UpdateParams(float t) {
    int status;
    bool path_playing;
    bool user_playing;

    if ((m_Flags & 0x800) == 0) {
        status = m_PFParms[m_ActiveProject].track_status;
    } else {
        status = -1;
    }
    path_playing = false;
    if (status - 5U < 2 || status == 2) {
        path_playing = true;
    }
    if (m_PFParms[m_ActiveProject].track_status == 3 && (m_Flags & 0x800) == 0) {
        return;
    }
    user_playing = false;
    if ((m_Flags & 0x10) != 0 && g_pEAXSound->GetSndGameMode() != SND_FRONTEND && (m_Flags & 0x802) == 0) {
        user_playing = true;
    }
    if (user_playing) {
        m_InteractiveProj = static_cast<eINTERACTIVE_PROJ_ID>((m_InteractiveProj + PF_INTERACTIVE_01) & PF_INTERACTIVE_03);
        SFXCTL_Pathfinder::SetCurInteractive(m_InteractiveProj);
        if (!g_pEAXSound->AreResourceLoadsPending()) {
            m_Flags &= ~0x10u;
            m_pSFXCTL_Pathfinder->DestroyTrack(m_PFParms + 1);
            {
                int index;

                index = gAEMSMgr.IsAssetInList(Attrib::StringKey(m_PFParms[1].mapfile));
                gAEMSMgr.UnloadSndData(index);
            }
            switch (m_InteractiveProj) {
            case PF_INTERACTIVE_01:
                m_PFParms[1].mapfile = "MW_Mus_2.mpf";
                m_PFParms[1].musfile = "MW_Mus_2.mus";
                break;
            case PF_INTERACTIVE_02:
                m_PFParms[1].mapfile = "MW_Mus_3.mpf";
                m_PFParms[1].musfile = "MW_Mus_3.mus";
                break;
            case PF_INTERACTIVE_03:
                m_PFParms[1].mapfile = "MW_Mus_4.mpf";
                m_PFParms[1].musfile = "MW_Mus_4.mus";
                break;
            case PF_INTERACTIVE_00:
            default:
                m_PFParms[1].mapfile = "MW_Mus_1.mpf";
                m_PFParms[1].musfile = "MW_Mus_1.mus";
                break;
            }
            LoadAsset(
                Attrib::StringKey(m_PFParms[1].mapfile), SNDPATH_PATHFINDER, EAXSND_DT_GENERIC_DATA, eBANK_SLOT_PATHFINDER, true);
        }
    } else {
        m_Flags &= ~0x10u;
    }
    if (!path_playing) {
        SetDMIX_Input(6, 0);
        SetDMIX_Input(7, 0);
        SetDMIX_Input(8, 0);
    } else {
        switch ((m_Flags & 0x100) != 0 ? m_PrevMusicType : m_MusicType) {
        case eMUSIC_TYPE_LICENCED:
            SetDMIX_Input(6, 0x7FFF);
            SetDMIX_Input(7, 0);
            SetDMIX_Input(8, 0);
            break;
        case eMUSIC_TYPE_INTERACTIVE:
        case eMUSIC_TYPE_SPLASH:
            SetDMIX_Input(6, 0);
            SetDMIX_Input(7, 0x7FFF);
            SetDMIX_Input(8, 0);
            break;
        case eMUSIC_TYPE_AMBIENCE:
            SetDMIX_Input(6, 0);
            SetDMIX_Input(7, 0);
            SetDMIX_Input(8, 0x7FFF);
            break;
        }
    }
    if (IsAudioStreamingEnabled != 0 && (m_Flags & 0x2000) == 0) {
        if (m_EATraxState == EATRAX_OFF) {
            if (GameFlowSndState[4] == 0 && GameFlowSndState[5] == 0 && GameFlowSndState[7] == 0 && GameFlowSndState[6] == 0 &&
                GameFlowSndState[11] == 0) {
                m_EATraxState = GenEATraxState();
            } else {
                UpdateInGame(t);
            }
        } else {
            UpdateInGame(t);
        }
        UpdatePursuitBreaker(t);
    }
}

void SFXObj_PFEATrax::ProcessUpdate() {
    bool path_playing;
    bool user_playing;

    if (IsAudioStreamingEnabled == 0) {
        return;
    }
    if (m_bClearSkipUpdate) {
        m_bSkipUpdate = false;
        m_bClearSkipUpdate = false;
    }
    if ((m_Flags & 0x800) == 0) {
        if ((m_Flags & 0x404) != 4) {
            return;
        }
        if (m_PFParms[m_ActiveProject].queue_next == 1 && (m_Flags & 4) != 0) {
            SendPathEvent();
            SNDSYS_service();
            goto CHECK_SKIP_UPDATE;
        }
        if (m_MusicType == eMUSIC_TYPE_LICENCED && (m_Flags & 0x40) == 0) {
            if (m_bSkipUpdate) {
                goto CHECK_SKIP_UPDATE;
            }
            if (m_CurPathEvent != 0x01C53FC7 && m_CurPathEvent != 0x01C3FA91) {
                user_playing = false;
                if (gMoviePlayer && gMoviePlayer->IsMoviePlaying()) {
                    user_playing = true;
                }
                if (!user_playing) {
                    NotifyChyron();
                }
            }
        }
    }
    if (!m_bSkipUpdate) {
        if ((m_Flags & 0x800) == 0) {
            path_playing = false;
            if (m_PFParms[m_ActiveProject].track_status == 5 || m_PFParms[m_ActiveProject].track_status == 2) {
                path_playing = true;
            }
        } else {
            path_playing = false;
        }
        if (!path_playing) {
            return;
        }
        {
            int nvol;

            nvol = 0;
            if (m_EATraxState == EATRAX_IG) {
                if (m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
                    nvol = 3;
                } else if (m_MusicType == eMUSIC_TYPE_LICENCED || m_MusicType == eMUSIC_TYPE_SPLASH) {
                    nvol = 1;
                } else if (m_MusicType == eMUSIC_TYPE_AMBIENCE) {
                    nvol = 5;
                }
                if (nvol != 0 || m_MusicType == eMUSIC_TYPE_INTERACTIVE) {
                    nvol = GetDMixOutput(nvol, DMX_VOL);
                    nvol = nvol * 100 >> 0xF;
                }
            } else if (m_EATraxState == EATRAX_FE) {
                if (m_MusicType == eMUSIC_TYPE_AMBIENCE) {
                    nvol = 4;
                    nvol = GetDMixOutput(nvol, DMX_VOL);
                    nvol = nvol * 100 >> 0xF;
                } else if (m_MusicType != eMUSIC_TYPE_LICENCED && m_MusicType != eMUSIC_TYPE_SPLASH) {
                    nvol = 0;
                }
            }
            m_Volume = nvol;
            if ((m_Flags & 0x800) != 0) {
                return;
            }
            PATH_volume(m_PFParms[m_ActiveProject].PATH_TRACK, static_cast<signed char>(nvol));
            SNDSYS_entercritical();
            SNDSTRM_lowpass(m_pSFXCTL_Pathfinder->GetHandle(m_ActiveProject), GetDMixOutput(9, DMX_FREQ));
            SNDSYS_leavecritical();
            return;
        }
    }

CHECK_SKIP_UPDATE:
    {
        EAXS_StreamChannel *pch;

        pch = g_pEAXSound->GetStreamManager()->GetStreamChannel(1);
        if (pch) {
            pch->SetVol(0, false);
        }
    }
}

void SFXObj_PFEATrax::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_PFParms[1].pmapfile = nullptr;
    m_PFParms[0].bAttached = false;
    m_PFParms[0].bdataloaded = false;
    m_PFParms[0].ramfile = nullptr;
    m_PFParms[0].pmapfile = nullptr;
    m_PFParms[1].bAttached = false;
    m_PFParms[1].bdataloaded = false;
    m_PFParms[1].ramfile = nullptr;
    m_PFParms[0].mapfile = "MW_Music.mpf";
    m_PFParms[0].musfile = "MW_Music.mus";
    m_PFParms[0].PATH_TRACK = 0x11000001;
    m_PFParms[1].procflags = 2;
    m_PFParms[1].mapfile = "MW_Mus_1.mpf";
    m_PFParms[1].musfile = "MW_Mus_1.mus";
    m_PFParms[1].PATH_TRACK = 0x12000001;
    m_PFParms[1].PATH_TRACKID = -1;
    m_PFParms[1].PATH_TRACK_BYTESPERSEC = 0x8D78;
    m_PFParms[0].procflags = 2;
    m_PFParms[0].projnum = -1;
    m_PFParms[0].curnodeparm = -1;
    m_PFParms[0].PATH_VOICE = -1;
    m_PFParms[0].PATH_TRACKID = -1;
    m_PFParms[0].PATH_TRACK_BYTESPERSEC = 0x8D78;
    m_PFParms[1].projnum = -1;
    m_PFParms[1].curnodeparm = -1;
    m_PFParms[1].PATH_VOICE = -1;
    m_EATraxState = GenEATraxState();
    m_Flags &= ~0x8u;
}

void SFXObj_PFEATrax::SetupLoadData() {
    if (IsAudioStreamingEnabled != 0) {
        LoadAsset(Attrib::StringKey(m_PFParms[0].mapfile), SNDPATH_PATHFINDER, EAXSND_DT_GENERIC_DATA, eBANK_SLOT_NONE, true);
        if (g_pEAXSound->GetPlayerMixMode() == EAXS3D_TWO_PLAYER_MIX) {
            m_Flags |= 2;
        } else {
            LoadAsset(Attrib::StringKey(m_PFParms[1].mapfile), SNDPATH_PATHFINDER, EAXSND_DT_GENERIC_DATA, eBANK_SLOT_PATHFINDER, true);
            m_Flags &= ~2u;
        }
    }
}

void SFXObj_PFEATrax::InitSFX() {
    int assetlocation;

    SndBase::InitSFX();
    SFXCTL_Pathfinder::SetCurInteractive(m_InteractiveProj);
    if (IsAudioStreamingEnabled != 0) {
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
}

void SFXObj_PFEATrax::GenNextMusicTrackID() {
    UserProfile *puser;

    m_EATrax[m_EATraxState].PlayTrackIndex = -1;
    m_EATrax[m_EATraxState].TraxMask &= 0x0FFFFFFF;
    if (m_EATraxState != EATRAX_OFF && m_EATrax[m_EATraxState].TraxMask != 0) {
        puser = FEDatabase->GetUserProfile(0);
        if (m_EATrax[m_EATraxState].PlayBits == 0 && m_EATrax[m_EATraxState].NumEnabledSongs != 0) {
            m_EATrax[m_EATraxState].PlayBits = m_EATrax[m_EATraxState].TraxMask & 0x0FFFFFFF;
            m_EATrax[m_EATraxState].LastPlaylistSong = -1;
        }
        if (m_EATrax[m_EATraxState].PBMode == 0) {
            {
                int n;

                n = m_EATrax[m_EATraxState].LastPlaylistSong;
                do {
                    ++n;
                    if (n < g_MaxSongs) {
                        {
                            int nSongindex;

                            nSongindex = puser->Playlist[n].SongIndex;
                            if ((m_EATrax[m_EATraxState].PlayBits & 1 << (nSongindex & 0x1F)) != 0) {
                                m_EATrax[m_EATraxState].PlayTrackIndex = nSongindex;
                                m_EATrax[m_EATraxState].PlayBits ^= 1 << (nSongindex & 0x1F);
                                if (m_EATrax[m_EATraxState].PlayBits == 0) {
                                    m_EATrax[m_EATraxState].PlayBits = m_EATrax[m_EATraxState].TraxMask & 0x0FFFFFFF;
                                    m_EATrax[m_EATraxState].PlayBits ^= 1 << (n & 0x1F);
                                }
                                return;
                            }
                        }
                    } else {
                        if (m_EATrax[m_EATraxState].NumEnabledSongs == 0) {
                            return;
                        }
                        m_EATrax[m_EATraxState].PlayBits = m_EATrax[m_EATraxState].TraxMask & 0x0FFFFFFF;
                        m_EATrax[m_EATraxState].LastPlaylistSong = -1;
                        GenNextMusicTrackID();
                        return;
                    }
                } while (true);
            }
        } else {
            {
                int nrandcount;
                int nSongNumber;
                int ncount;

                {
                    int npb;

                    nrandcount = 0;
                    npb = 0;
                    while (npb < g_MaxSongs) {
                        if ((m_EATrax[m_EATraxState].PlayBits & 1 << (npb & 0x1F)) != 0) {
                            ++nrandcount;
                        }
                        ++npb;
                    }
                }
                SoundRandomSeed = bGetTicker();
                nSongNumber = g_pEAXSound->Random(nrandcount);
                ncount = 0;
                {
                    int n;

                    n = 0;
                    while (n < g_MaxSongs) {
                        if ((m_EATrax[m_EATraxState].PlayBits & 1 << (n & 0x1F)) != 0) {
                            if (nSongNumber == ncount) {
                                m_EATrax[m_EATraxState].PlayTrackIndex = n;
                                m_EATrax[m_EATraxState].PlayBits ^= 1 << (n & 0x1F);
                                if (m_EATrax[m_EATraxState].PlayBits == 0) {
                                    m_EATrax[m_EATraxState].PlayBits = m_EATrax[m_EATraxState].TraxMask & 0x0FFFFFFF;
                                    m_EATrax[m_EATraxState].PlayBits ^= 1 << (n & 0x1F);
                                }
                                return;
                            }
                            ++ncount;
                        }
                        ++n;
                    }
                }
            }
        }
    }
}

void SFXObj_PFEATrax::NotifyChyron() {
    stSongInfo *currSong;

    if (m_EATrax[m_EATraxState].PlayTrackIndex >= 0 &&
        m_EATrax[m_EATraxState].PlayTrackIndex <= static_cast<int>(Songs.size())) {
        currSong = Songs[m_EATrax[m_EATraxState].PlayTrackIndex];
        SummonChyron(currSong->SongName, currSong->Artist, currSong->Album);
        m_Flags |= 0x40;
    }
}

void SFXObj_PFEATrax::MessageStartPathfinder(const MControlPathfinder &message) {
    unsigned int event;

    if (m_EATraxState == EATRAX_OFF) {
        Stop();
    } else if (IsAudioStreamingEnabled != 0 &&
               ((event = message.GetPathEvent()), m_MusicType != eMUSIC_TYPE_SPLASH || event != static_cast<unsigned int>(-1))) {
        if (event == 0) {
            m_MusicType = GenMusicType();
        } else if (event == static_cast<unsigned int>(-1)) {
            if (0.0f < g_pEAXSound->GetCurMusicVolume() && (m_Flags & 0x800) == 0 &&
                g_pEAXSound->GetCurAudioSettings()->EATraxMode != 0 && m_EATrax[m_EATraxState].TraxMask != 0 &&
                m_MusicType == eMUSIC_TYPE_LICENCED && !m_bSkipUpdate) {
                PATH_clearallevents(0x0F000000);
                PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
            }
        } else {
            if (m_EATrax[0].TraxMask == 0) {
                m_EATrax[0].TraxMask = 0x80000000;
            }
            PATH_clearallevents(0x0F000000);
            m_MusicType = GenMusicType();
            m_CurPathEvent = event;
            m_PrevPathEvent = event;
            StartLicensedMusic(event);
            m_bSkipUpdate = true;
            m_bClearSkipUpdate = false;
        }
    }
}

void SFXObj_PFEATrax::MessageSendPathEvent(const MControlPathfinder &message) {
    unsigned int curr_event;
    int ntmp;

    if (IsAudioStreamingEnabled != 0) {
        curr_event = m_CurPathEvent;
        ntmp = static_cast<int>(message.GetPathEvent());
        if (m_MusicType == eMUSIC_TYPE_LICENCED || m_MusicType == eMUSIC_TYPE_AMBIENCE || m_MusicType == eMUSIC_TYPE_SPLASH) {
            if (ntmp == 9) {
                m_CurPathEvent = 0x01C3FA91;
            } else if (ntmp == 0x10) {
                m_CurPathEvent = 0x01C53FC7;
            } else if (ntmp > 9) {
                return;
            }
        } else {
            m_CurPathEvent = PFXMAP[m_InteractiveProj][ntmp][0];
        }
        if (m_CurPathEvent != curr_event) {
            m_PrevPathEvent = curr_event;
            m_PFParms[m_ActiveProject].queue_next = 1;
            if (m_MusicType == eMUSIC_TYPE_AMBIENCE) {
                PATH_pause(m_PFParms[m_ActiveProject].PATH_TRACK, '\0');
            }
            SendPathEvent();
            {
                int nproj;

                if (static_cast<unsigned int>(ntmp) - 0x13 < 2) {
                    SwapInteractiveProjects();
                }
            }
        }
    }
}
void SFXObj_PFEATrax::MessageSendPathControl(const MControlPathfinder &message) {
    if (IsAudioStreamingEnabled != 0 && m_MusicType == eMUSIC_TYPE_INTERACTIVE &&
        message.GetPathControl() != static_cast<unsigned int>(m_CurIntensity)) {
        m_PrevIntensity = m_CurIntensity;
        m_CurIntensity = static_cast<char>(message.GetPathControl());
        PATH_control(m_PFParms[m_ActiveProject].PATH_TRACKID, static_cast<int>(m_CurIntensity));
    }
}

void SFXObj_PFEATrax::MessagePartUpdate(const MControlPathfinder &message) {
    m_CurPart = message.GetPartID();
    m_bClearSkipUpdate = true;
    if (m_CurPart > 8) {
        return;
    }

    if (m_CurPart < -1) {
        return;
    }

    if (m_Flags & 0x100) {
        m_Flags &= ~0x100u;
    }
}

void SFXObj_PFEATrax::MessagePerpBusted(const MPerpBusted &message) {}

void SFXObj_PFEATrax::MessageInteractiveDone(const MControlPathfinder &message) {
    mT_ambienceStart = WorldTimer;
}

void SFXObj_PFEATrax::MessageSwapInteractive(const MControlPathfinder &message) {
    m_CurPathEvent = PFXMAP[m_InteractiveProj][18][0];
    m_PrevPathEvent = 0;
    m_PFParms[m_ActiveProject].queue_next = 1;
}

SFXObj_Pathfinder::~SFXObj_Pathfinder() {}

ParameterAccessor AmbientAccessor("Ambience");

SndBase::TypeInfo SFXObj_Pathfinder::s_TypeInfo = { 0, "SFXObj_Pathfinder", nullptr, SFXObj_Pathfinder::CreateObject };
SndBase::TypeInfo SFXObj_PFEATrax::s_TypeInfo = { 0, "SFXObj_PFEATrax", nullptr, SFXObj_PFEATrax::CreateObject };
stEATraxParms SFXObj_PFEATrax::m_EATrax[2];
