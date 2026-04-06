#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Collision.hpp"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioimpact.h"
#include "Speed/Indep/Src/Generated/Messages/MAudioReflection.h"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"

extern bool IsWorldDataStreaming(unsigned int strmhandle);

SndBase::TypeInfo SFXObj_Collision::s_TypeInfo = { 0, "SFXObj_Collision", nullptr, SFXObj_Collision::CreateObject };

SndBase::TypeInfo *SFXObj_Collision::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_Collision::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_Collision::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXObj_Collision::GetStaticTypeInfo()->typeName, false) SFXObj_Collision();
    }

    return new (SFXObj_Collision::GetStaticTypeInfo()->typeName, true) SFXObj_Collision();
}

SFXObj_Collision::SFXObj_Collision()
    : CARSFX() {
    vCollisionPos = bVector3(0.0f, 0.0f, 0.0f);
    vScrapePos = bVector3(0.0f, 0.0f, 0.0f);
    vVelocity = bVector3(0.0f, 0.0f, 0.0f);
    VolSlot = eVOL_COLLISION_WORLD_WALL_FRONT;
    PitchSlot = ePCH_COLLISION_COL_PITCH;
    ReverbSlot = eVRB_COLLISION_RVRB_COL;
    FirstUpdate = false;
    AzimSlot = eAZI_COLLISION_COLLISION_AZI;
    m_pCollisionStich = nullptr;
    m_pcsisScrape = nullptr;
    m_pCollisionEvent = nullptr;
}

SFXObj_Collision::~SFXObj_Collision() {
    Destroy();
}

void SFXObj_Collision::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    m_pCollisionState = static_cast<CSTATE_Collision *>(m_pStateBase);
}

void SFXObj_Collision::InitSFX() {
    SndBase::InitSFX();

    FirstUpdate = true;
    m_pCollisionEvent = m_pCollisionState->m_pCollisionEvent;
    vCollisionPos = *m_pCollisionEvent->GetInitialContactPoint();
    vScrapePos = *m_pCollisionEvent->GetCurrentContactPoint();
    vVelocity = *m_pCollisionEvent->GetCurrentVelocity();

    m_p3DColPos->AssignPositionVector(&vCollisionPos);
    m_p3DColPos->AssignVelocityVector(&vVelocity);
    m_p3DScrapePos->AssignPositionVector(&vScrapePos);
    m_p3DScrapePos->AssignVelocityVector(&vVelocity);

    AzimSlot = eAZI_COLLISION_COLLISION_AZI;
    PitchSlot = ePCH_COLLISION_COL_PITCH;
    ReverbSlot = eVRB_COLLISION_RVRB_COL;

    if (m_pCollisionEvent->IsDescribed(0x12)) {
        VolSlot = eVOL_COLLISION_SMOKABLE_CAR;
        AzimSlot = eVOL_COLLISION_SMACKABLE_AZI;
        PitchSlot = ePCH_COLLISION_SMAK_PITCH;
    } else if (m_pCollisionEvent->IsDescribed(2)) {
        VolSlot = eVOL_COLLISION_SMOKABLE_WORLD;
        AzimSlot = eVOL_COLLISION_SMACKABLE_AZI;
        PitchSlot = ePCH_COLLISION_SMAK_PITCH;
        ReverbSlot = eVRB_COLLISION_RVRB_SMAK;
    } else if (m_pCollisionEvent->IsDescribed(0x10)) {
        if (m_pCollisionEvent->IsDescribed(4)) {
            VolSlot = eVOL_COLLISION_WORLD_WALL_FRONT;
            if (!m_pCollisionEvent->IsDescribed(0x80)) {
                VolSlot = eVOL_COLLISION_WORLD_WALL_SIDE;
            }
        } else if (m_pCollisionEvent->IsDescribed(0x40)) {
            VolSlot = eVOL_COLLISION_WORLD_BOTTEMOUT;
        } else if (m_pCollisionEvent->IsDescribed(0x20)) {
            VolSlot = eVOL_COLLISION_WORLD_ROLLOVER;
        } else if (m_pCollisionEvent->IsDescribed(0x100)) {
            VolSlot = eVOL_COLLISION_CAR_SIDE;
        } else if (m_pCollisionEvent->IsDescribed(0x80)) {
            VolSlot = eVOL_COLLISION_CAR_FRONT;
        }
    }

    if (m_pCollisionEvent->IsDescribed(1) && m_pCollisionEvent->IsDescribed(8)) {
        int intensity = m_pCollisionEvent->GetIntensity();
        Attrib::Gen::audioimpact AudioFX(m_pCollisionEvent->GetAudioFX(), 0, nullptr);
        unsigned int numstreams = AudioFX.Num_StreamSweetner();
        const CollisionStream *streamsweet = nullptr;

        {
            int n = static_cast<int>(numstreams) - 1;

            while (n >= 0) {
                const CollisionStream *tmpsweet = &AudioFX.StreamSweetner(n);
                if (static_cast<int>(tmpsweet->Threshold) <= intensity) {
                    streamsweet = tmpsweet;
                }
                n--;
            }
        }

        if (streamsweet && !IsWorldDataStreaming(0)) {
            Attrib::Instance streaminfo(streamsweet->StreamMoment, 0, nullptr);
            UMath::Vector4 vpos;
            unsigned int carsID;

            vpos.x = -vCollisionPos.y;
            vpos.y = vCollisionPos.z;
            vpos.z = vCollisionPos.x;
            vpos.w = 0.0f;
            carsID = 0;

            {
                int n = 0;

                while (n < UTL::Collections::Listable<EAX_CarState, 10>::Count()) {
                    carsID = UTL::Collections::Listable<EAX_CarState, 10>::GetList()[n]->mWorldID;
                    if ((carsID == m_pCollisionEvent->GetActor() || carsID == m_pCollisionEvent->GetActee()) &&
                        UTL::Collections::Listable<EAX_CarState, 10>::GetList()[n]->GetContext() != 0) {
                        break;
                    }
                    n++;
                }
            }

            new EMomentStrm(vpos, UMath::Vector4::kZero, UMath::Vector4::kZero, carsID, nullptr, streaminfo.GetCollection());
        }
    }

    if (m_pCollisionEvent->IsDescribed(0x200)) {
        g_pEAXSound->SetCsisName("Csis::Scrape");
        m_fScrapeFade = 1.0f;
        m_pcsisScrape = new FX_Scrape(0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
    } else {
        m_pCollisionStich = new cStichWrapper(*m_pCollisionEvent->GetImpactStich());
        m_pCollisionStich->Play(0, 0, 0);
    }
}

void SFXObj_Collision::Detach() {
    Destroy();
}

void SFXObj_Collision::UpdateParams(float t) {
    SndBase::UpdateParams(t);

    SetDMIX_Input(0, 0);
    SetDMIX_Input(1, 0);
    SetDMIX_Input(2, 0);

    if (m_pCollisionState->IsAttached()) {
        if (m_pCollisionStich) {
            if (!m_pCollisionStich->IsPlaying()) {
                m_pCollisionState->Detach();
                return;
            }
        }

        if (FirstUpdate && !m_pcsisScrape) {
            if (m_pCollisionEvent->IsDescribed(8)) {
                SetDMIX_Input(0, 0x7FFF);
            } else if (m_pCollisionEvent->IsDescribed(2)) {
                SetDMIX_Input(2, 0x7FFF);
            } else if (m_pCollisionEvent->IsDescribed(0x10)) {
                SetDMIX_Input(1, 0x7FFF);
                MAudioReflection(m_pCollisionEvent->GetParameters().object, 0.0f, false).Send(UCrc32("FRONT_BARRIER_HIT"));
            }

            SetDMIX_Input(4, m_pCollisionEvent->GetIntensity() * 0xFF);
        }

        if (m_pCollisionEvent->IsDescribed(1) && m_pCollisionEvent->IsDescribed(0x10) &&
            m_pCollisionEvent->IsDescribed(4) && m_pcsisScrape) {
            MAudioReflection(m_pCollisionEvent->GetParameters().object, 0.0f, false).Send(UCrc32("FRONT_BARRIER_HIT"));
        }

        vScrapePos = *m_pCollisionEvent->GetCurrentContactPoint();
    }
}

void SFXObj_Collision::ProcessUpdate() {
    SndBase::ProcessUpdate();

    SND_Params impactParams;

    if (FirstUpdate) {
        InitialAz = GetDMixOutput(AzimSlot, DMX_AZIM);
        FirstUpdate = false;
    }

    if (m_pCollisionEvent) {
        impactParams.Vol = GetDMixOutput(VolSlot, DMX_VOL) * m_pCollisionEvent->GetVolume() >> 15;
    } else {
        impactParams.Vol = GetDMixOutput(VolSlot, DMX_VOL);
    }

    impactParams.Pitch = GetDMixOutput(PitchSlot, DMX_PITCH);
    impactParams.RVerb = GetDMixOutput(ReverbSlot, DMX_VOL);
    impactParams.Az = InitialAz;

    if (m_pCollisionStich) {
        m_pCollisionStich->Update(&impactParams);
    }

    if (m_pcsisScrape) {
        if (!m_pCollisionEvent->IsStillActive()) {
            m_fScrapeFade -= m_pStateBase->GetDeltaTime() * 4.0f;
            if (m_fScrapeFade < 0.0f) {
                m_pStateBase->Detach();
                return;
            }
        }

        m_pcsisScrape->SetVolume(GetDMixOutput(eVOL_COLLISION_SCRAPE, DMX_VOL));
        m_pcsisScrape->SetPitch(GetDMixOutput(ePCH_COLLISION_SCRAPE_PITCH, DMX_PITCH));
        m_pcsisScrape->SetAzimuth(GetDMixOutput(eAZI_COLLISION_SCRAPE_AZI, DMX_AZIM));
        m_pcsisScrape->SetFilter_Effects_Dry_FX(0x7FFF);
        m_pcsisScrape->SetFilter_Effects_Wet_FX(GetDMixOutput(eVRB_COLLISION_RVRB_SCRP, DMX_VOL));
        m_pcsisScrape->SetFilter_Effects_LoPass(25000);
        m_pcsisScrape->SetImpulse_magnitude(m_pCollisionEvent->GetIntensity());
        m_pcsisScrape->CommitMemberData();
    }
}

int SFXObj_Collision::GetController(int Index) {
    if (Index == 0) {
        return 0;
    }
    if (Index == 1) {
        return 1;
    }
    return -1;
}

void SFXObj_Collision::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
    case 0:
        m_p3DColPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
        break;
    case 1:
        m_p3DScrapePos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
        break;
    }
}

void SFXObj_Collision::Destroy() {
    if (m_pCollisionStich) {
        delete m_pCollisionStich;
        m_pCollisionStich = nullptr;
    }

    if (m_pcsisScrape) {
        delete m_pcsisScrape;
        m_pcsisScrape = nullptr;
    }

    m_pCollisionEvent = nullptr;
}
