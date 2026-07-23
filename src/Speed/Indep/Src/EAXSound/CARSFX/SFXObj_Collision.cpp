#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_moment_strm.h"
#include "Speed/Indep/Src/Generated/Events/EMomentStrm.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAudioReflection.h"

static const int PRINT_COLLISION_INFO = 0; // size: 0x4, Decl: 12

DEFINE_CREATABLE(0x70000, SFXObj_Collision, SndBase);

SFXObj_Collision::SFXObj_Collision() : CARSFX() {
    this->vCollisionPos = bVector3(0.0f, 0.0f, 0.0f);
    this->vScrapePos = bVector3(0.0f, 0.0f, 0.0f);
    this->vVelocity = bVector3(0.0f, 0.0f, 0.0f);
    this->VolSlot = eVOL_COLLISION_WORLD_WALL_FRONT;
    this->PitchSlot = ePCH_COLLISION_COL_PITCH;
    this->ReverbSlot = eVRB_COLLISION_RVRB_COL;
    this->FirstUpdate = false;
    this->AzimSlot = eAZI_COLLISION_COLLISION_AZI;
    this->m_pCollisionStich = nullptr;
    this->m_pcsisScrape = nullptr;
    this->m_pCollisionEvent = nullptr;
}

SFXObj_Collision::~SFXObj_Collision() {
    this->Destroy();
}

void SFXObj_Collision::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
    this->m_pCollisionState = static_cast<CSTATE_Collision *>(this->m_pStateBase);
}

// UNSOLVED regswap
void SFXObj_Collision::InitSFX() {
    this->SndBase::InitSFX();

    this->m_pCollisionEvent = this->m_pCollisionState->m_pCollisionEvent;
    this->FirstUpdate = true;
    this->vCollisionPos = *this->m_pCollisionEvent->GetInitialContactPoint();
    this->vScrapePos = *this->m_pCollisionEvent->GetCurrentContactPoint();
    this->vVelocity = *this->m_pCollisionEvent->GetCurrentVelocity();

    this->m_p3DColPos->AssignPositionVector(&this->vCollisionPos);
    this->m_p3DColPos->AssignVelocityVector(&this->vVelocity);
    this->m_p3DScrapePos->AssignPositionVector(&this->vScrapePos);
    this->m_p3DScrapePos->AssignVelocityVector(&this->vVelocity);

    this->AzimSlot = eAZI_COLLISION_COLLISION_AZI;
    this->PitchSlot = ePCH_COLLISION_COL_PITCH;
    this->ReverbSlot = eVRB_COLLISION_RVRB_COL;

    // TODO magic, macro or anynomous enum?
    if (this->m_pCollisionEvent->IsDescribed(0x412)) {
        this->VolSlot = eVOL_COLLISION_SMOKABLE_CAR;
        this->AzimSlot = eVOL_COLLISION_SMACKABLE_AZI;
        this->PitchSlot = ePCH_COLLISION_SMAK_PITCH;

    } else if (this->m_pCollisionEvent->IsDescribed(2)) {
        this->VolSlot = eVOL_COLLISION_SMOKABLE_WORLD;
        this->AzimSlot = eVOL_COLLISION_SMACKABLE_AZI;
        this->PitchSlot = ePCH_COLLISION_SMAK_PITCH;
        this->ReverbSlot = eVRB_COLLISION_RVRB_SMAK;

    } else if (this->m_pCollisionEvent->IsDescribed(0x10)) {
        if (this->m_pCollisionEvent->IsDescribed(4)) {
            if (this->m_pCollisionEvent->IsDescribed(0x80)) {
                this->VolSlot = eVOL_COLLISION_WORLD_WALL_FRONT;
            } else {
                this->VolSlot = eVOL_COLLISION_WORLD_WALL_SIDE;
            }
        } else if (this->m_pCollisionEvent->IsDescribed(0x40)) {
            this->VolSlot = eVOL_COLLISION_WORLD_BOTTEMOUT;
        } else if (this->m_pCollisionEvent->IsDescribed(0x20)) {
            this->VolSlot = eVOL_COLLISION_WORLD_ROLLOVER;
        } else if (this->m_pCollisionEvent->IsDescribed(0x100)) {
            this->VolSlot = eVOL_COLLISION_CAR_SIDE;
        } else if (this->m_pCollisionEvent->IsDescribed(0x80)) {
            this->VolSlot = eVOL_COLLISION_CAR_FRONT;
        }
    }

    if ((this->m_pCollisionEvent->IsDescribed(1) && this->m_pCollisionEvent->IsDescribed(8)) || this->m_pCollisionEvent->IsDescribed(0x400)) {
        int intensity = this->m_pCollisionEvent->GetIntensity();
        Attrib::Gen::audioimpact AudioFX(this->m_pCollisionEvent->GetAudioFX(), 0, nullptr);
        unsigned int numstreams = AudioFX.Num_StreamSweetner();
        const CollisionStream *streamsweet = nullptr;

        for (int n = static_cast<int>(numstreams) - 1; n >= 0; n--) {
            const CollisionStream *tmpsweet = &AudioFX.StreamSweetner(n);

            if (static_cast<int>(tmpsweet->Threshold) <= intensity) {
                streamsweet = tmpsweet;
            }
        }

        if ((streamsweet != nullptr)) {
            bool IsWorldDataStreaming(unsigned int strmhandle);

            if (IsWorldDataStreaming(0) != true) {
                Attrib::Gen::aud_moment_strm streaminfo(streamsweet->StreamMoment, 0, nullptr);
                UMath::Vector4 vpos;
                unsigned int carsID = 0;

                vpos.z = this->vCollisionPos.x;
                vpos.x = -this->vCollisionPos.y;
                vpos.y = this->vCollisionPos.z;

                for (int n = 0; n < EAX_CarState::Count(); n++) {
                    if ((EAX_CarState::GetList()[n]->mWorldID == this->m_pCollisionEvent->GetActor() ||
                         EAX_CarState::GetList()[n]->mWorldID == this->m_pCollisionEvent->GetActee()) &&
                        EAX_CarState::GetList()[n]->GetContext() != 0) {
                        carsID = EAX_CarState::GetList()[n]->mWorldID;
                        break;
                    }
                }

                new EMomentStrm(vpos, UMath::Vector4::kZero, UMath::Vector4::kZero, carsID, nullptr, streaminfo.GetCollection());
            }
        }
    }

    if (this->m_pCollisionEvent->IsDescribed(0x200)) {
        g_pEAXSound->SetCsisName("Csis::Scrape");
        this->m_fScrapeFade = 1.0f;
        this->m_pcsisScrape = new Csis::FX_Scrape(0, 0, 0, 0, 0, 25000, 0, 0x7FFF, 0);
    } else {
        this->m_pCollisionStich = new cStichWrapper(*this->m_pCollisionEvent->GetImpactStich());
        this->m_pCollisionStich->Play(0, 0, 0);
    }
}

void SFXObj_Collision::Detach() {
    this->Destroy();
}

void SFXObj_Collision::UpdateParams(float t) {
    SndBase::UpdateParams(t);

    this->SetDMIX_Input(0, 0);
    this->SetDMIX_Input(1, 0);
    this->SetDMIX_Input(2, 0);

    if (this->m_pCollisionState->IsAttached()) {
        if (this->m_pCollisionStich != nullptr) {
            if (!this->m_pCollisionStich->IsPlaying()) {
                this->m_pCollisionState->Detach();
                return;
            }
        }

        if (FirstUpdate && (this->m_pcsisScrape == nullptr)) {
            if (this->m_pCollisionEvent->IsDescribed(8)) {
                this->SetDMIX_Input(0, 0x7FFF);
            } else if (this->m_pCollisionEvent->IsDescribed(2)) {
                this->SetDMIX_Input(2, 0x7FFF);
            } else if (this->m_pCollisionEvent->IsDescribed(0x10)) {
                this->SetDMIX_Input(1, 0x7FFF);
                MAudioReflection(this->m_pCollisionEvent->GetParameters().object, 0.0f, false).Send(UCrc32("FRONT_BARRIER_HIT"));
            }

            this->SetDMIX_Input(4, this->m_pCollisionEvent->GetIntensity() * 0xFF);
        }

        if (this->m_pCollisionEvent->IsDescribed(1) && this->m_pCollisionEvent->IsDescribed(0x10) && this->m_pCollisionEvent->IsDescribed(4) &&
            (this->m_pcsisScrape != nullptr)) {
            MAudioReflection(this->m_pCollisionEvent->GetParameters().object, 0.0f, false).Send(UCrc32("FRONT_BARRIER_HIT"));
        }

        vScrapePos = *this->m_pCollisionEvent->GetCurrentContactPoint();
    }
}

static const float SCRAPE_FADE_OUT_MS = 0.25f; // size: 0x4, Decl: 263

void SFXObj_Collision::ProcessUpdate() {
    SndBase::ProcessUpdate();

    SND_Params impactParams;
    impactParams.ID = 0;
    impactParams.Az = 0;
    impactParams.Mag = 0;
    impactParams.RVerb = 0;

    if (this->FirstUpdate) {
        this->InitialAz = this->GetDMixOutput(AzimSlot, DMX_AZIM);
        this->FirstUpdate = false;
    }

    if (this->m_pCollisionEvent != nullptr) {
        impactParams.Vol = (this->GetDMixOutput(VolSlot, DMX_VOL) * this->m_pCollisionEvent->GetVolume()) >> 15;
    } else {
        impactParams.Vol = this->GetDMixOutput(VolSlot, DMX_VOL);
    }

    impactParams.Pitch = this->GetDMixOutput(PitchSlot, DMX_PITCH);
    impactParams.RVerb = this->GetDMixOutput(ReverbSlot, DMX_VOL);
    impactParams.Az = InitialAz;

    if (this->m_pCollisionStich != nullptr) {
        this->m_pCollisionStich->Update(&impactParams);
    }

    if (this->m_pcsisScrape != nullptr) {
        if (!this->m_pCollisionEvent->IsStillActive()) {
            this->m_fScrapeFade -= this->m_pStateBase->GetDeltaTime() / SCRAPE_FADE_OUT_MS;
            if (this->m_fScrapeFade < 0.0f) {
                m_pStateBase->Detach();
                return;
            }
        }

        this->m_pcsisScrape->SetVolume(this->GetDMixOutput(eVOL_COLLISION_SCRAPE, DMX_VOL));
        this->m_pcsisScrape->SetPitch(this->GetDMixOutput(ePCH_COLLISION_SCRAPE_PITCH, DMX_PITCH));
        this->m_pcsisScrape->SetAzimuth(this->GetDMixOutput(eAZI_COLLISION_SCRAPE_AZI, DMX_AZIM));
        this->m_pcsisScrape->SetFilter_Effects_Dry_FX(0x7FFF);
        this->m_pcsisScrape->SetFilter_Effects_Wet_FX(this->GetDMixOutput(eVRB_COLLISION_RVRB_SCRP, DMX_VOL));
        this->m_pcsisScrape->SetFilter_Effects_LoPass(25000);
        this->m_pcsisScrape->SetImpulse_magnitude(this->m_pCollisionEvent->GetIntensity());
        this->m_pcsisScrape->CommitMemberData();
    }
}

int SFXObj_Collision::GetController(int Index) {
    switch (Index) {
        case 0:
            return 0;
        case 1:
            return 1;
        default:
            return -1;
    }
}

void SFXObj_Collision::AttachController(SFXCTL *psfxctl) {
    switch (psfxctl->GetObjectIndex()) {
        case 0:
            this->m_p3DColPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
            break;
        case 1:
            this->m_p3DScrapePos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
            break;
    }
}

void SFXObj_Collision::Destroy() {
    if (this->m_pCollisionStich != nullptr) {
        delete this->m_pCollisionStich;
        this->m_pCollisionStich = nullptr;
    }

    if (this->m_pcsisScrape != nullptr) {
        delete this->m_pcsisScrape;
        this->m_pcsisScrape = nullptr;
    }

    this->m_pCollisionEvent = nullptr;
}
