#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_PFEATrax.hpp"

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"

extern int PATH_stop(int tracks);
extern void PATH_clearallevents(int mask);
extern int PURSUIT_TO_LIC_DELAY;
extern int PFXMAP[4][21][2];

SndBase::TypeInfo SFXObj_Pathfinder::s_TypeInfo = { 0, "SFXObj_Pathfinder", nullptr, SFXObj_Pathfinder::CreateObject };
SndBase::TypeInfo SFXObj_PFEATrax::s_TypeInfo = { 0, "SFXObj_PFEATrax", nullptr, SFXObj_PFEATrax::CreateObject };
stEATraxParms SFXObj_PFEATrax::m_EATrax[2];

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
    m_PFParms[1].pmapfile = nullptr;
    m_Flags = 0x40;
    m_pSFXCTL_Pathfinder = nullptr;
    m_PFParms[0].pmapfile = nullptr;
}

SFXObj_Pathfinder::~SFXObj_Pathfinder() {}

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

void SFXObj_PFEATrax::RestartRace() {
    if (static_cast<unsigned int>(m_MusicType) < eMUSIC_TYPE_AMBIENCE) {
        PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
        PATH_clearallevents(0x0F000000);
        m_MusicType = eMUSIC_TYPE_LICENCED;
        m_bPathFAILED = false;
    }

    mT_ambienceStart = Timer(0);
}

void SFXObj_PFEATrax::SwapInteractiveProjects() {
    m_Flags |= 0x18;
}

void SFXObj_PFEATrax::Stop() {
    m_EATraxState = EATRAX_OFF;
    PATH_clearallevents(0x0F000000);
    PATH_stop(m_PFParms[m_ActiveProject].PATH_TRACK);
    m_PFParms[m_ActiveProject].queue_next = 0;
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
