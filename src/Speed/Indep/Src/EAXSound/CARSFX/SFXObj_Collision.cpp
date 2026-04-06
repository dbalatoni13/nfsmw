#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Collision.hpp"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/MAIN_AEMS.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/audioimpact.h"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"

extern bool IsWorldDataStreaming(unsigned int strmhandle);

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
