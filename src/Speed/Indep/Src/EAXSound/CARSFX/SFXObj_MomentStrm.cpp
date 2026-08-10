#include "Speed/Indep/Src/Animation/AnimCandidates.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_MomentStrm.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_moment_strm.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_moment_strm_hash.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"

// size: 0x78, address: 0x8045E5EC, Decl: 20
stMomentMapping g_MomentMappings[15] = {
    stMomentMapping(1, Attrib::Hash::aud_moment_strm::key_expl_gas_station),
    stMomentMapping(2, Attrib::Hash::aud_moment_strm::key_comm_tower_crash),
    stMomentMapping(15, Attrib::Hash::aud_moment_strm::key_comm_tower_crash),
    stMomentMapping(4, Attrib::Hash::aud_moment_strm::key_wtr_tower_crash),
    stMomentMapping(5, Attrib::Hash::aud_moment_strm::key_porch),
    stMomentMapping(6, Attrib::Hash::aud_moment_strm::key_gate),
    stMomentMapping(7, Attrib::Hash::aud_moment_strm::key_scaffold_crash_big),
    stMomentMapping(8, Attrib::Hash::aud_moment_strm::key_gazebo_crash),
    stMomentMapping(9, Attrib::Hash::aud_moment_strm::key_fish_sign_crash),
    stMomentMapping(10, Attrib::Hash::aud_moment_strm::key_boat_fall),
    stMomentMapping(17, Attrib::Hash::aud_moment_strm::key_trailer_park),
    stMomentMapping(19, Attrib::Hash::aud_moment_strm::key_amphitheatre),
    stMomentMapping(20, Attrib::Hash::aud_moment_strm::key_stripmall),
    stMomentMapping(16, Attrib::Hash::aud_moment_strm::key_drive_in),
    stMomentMapping(18, Attrib::Hash::aud_moment_strm::key_torus),
};

DEFINE_CREATABLE(0x60, SFXObj_MomentStrm, SndBase);

int SND_PRINT_STRM_BLOCK = 0; // size: 0x4, address: 0xFFFFFFFF, Decl: 49
int DEBUG_MOMENT_STRM = 0;    // size: 0x4, address: 0xFFFFFFFF, Decl: 50

bool SFXObj_MomentStrm::bHoldStream = false;           // size: 0x1, Decl: 52
float SFXObj_MomentStrm::m_TimeBeforeRetrigger = 0.0f; // size: 0x1, Decl: 53

SFXObj_MomentStrm *g_MomentStream = nullptr; // size: 0x4, address: 0x80418370, Decl: 55

SFXObj_MomentStrm::SFXObj_MomentStrm()
    : CARSFX(), //
      mMsgReceiveMoment(Hermes::Handler::Create<MGamePlayMoment, SFXObj_MomentStrm, SFXObj_MomentStrm>(this, &SFXObj_MomentStrm::ReceiveMoment,
                                                                                                       UCrc32("MomentStrm"), 0)), //
      mMsgPursuitBreaker(Hermes::Handler::Create<MPursuitBreaker, SFXObj_MomentStrm, SFXObj_MomentStrm>(
          this, &SFXObj_MomentStrm::ReceivePursuitBreaker, UCrc32("PursuitBreaker"), 0)), //
      mMomentPositonsList() {
    this->m_p3DPos = nullptr;
    this->VolSlot = eVOL_MOMENT_GASPUMP;
    this->fPosition = bVector3(0.0f, 0.0f, 0.0f);
    this->fVector = bVector3(0.0f, 0.0f, 0.0f);
    this->fVelocity = bVector3(0.0f, 0.0f, 0.0f);
    this->m_CurMoment = 0;
    this->UseUserPos = false;
    this->mHeldMoment = nullptr;
    this->bHoldStream = false;
    g_MomentStream = this;
    this->mCarsID = 0;
    this->mbUseTRafficsID = false;
}

SFXObj_MomentStrm::~SFXObj_MomentStrm() {
    g_MomentStream = nullptr;
    this->Destroy();

    if (this->mMsgReceiveMoment != nullptr) {
        Hermes::Handler::Destroy(this->mMsgReceiveMoment);
    }

    if (this->mMsgPursuitBreaker != nullptr) {
        Hermes::Handler::Destroy(this->mMsgPursuitBreaker);
    }

    this->mMomentPositonsList.clear();
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 6, 0);
}

void SFXObj_MomentStrm::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

void SFXObj_MomentStrm::InitSFX() {
    SndBase::InitSFX();
    this->m_p3DPos->AssignPositionVector(&fPosition);
    this->m_p3DPos->AssignDirectionVector(&fVector);
    this->m_p3DPos->AssignVelocityVector(&fVelocity);
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 6, 0);
    this->m_TimeBeforeRetrigger = 0.0f;
    this->mMomentPositonsList.clear();

    for (int n = 0; n < NUM_ELEMENTS(g_MomentMappings); n++) {
        const char *markerName = CAnimCandidateData::GetMomentMarkerName(g_MomentMappings[n].MarkerType);

        if (markerName[0] != '\0') {
            unsigned int markerHash = bStringHash(markerName);
            int numTrackMarkers = GetNumTrackPositionMarkers(0, markerHash);
            float closestMarkerDist;

            for (int index = 0; index < numTrackMarkers; index++) {
                TrackPositionMarker *marker = GetTrackPositionMarker(markerHash, index);

                if (marker != nullptr) {
                    stMomentDecription newmoment;

                    newmoment.vPos = UMath::Vector4::kZero;
                    newmoment.key = 0;
                    newmoment.vPos.z = marker->Position.x;
                    newmoment.vPos.x = -marker->Position.y;
                    newmoment.vPos.y = marker->Position.z;
                    newmoment.vPos.w = 1.0f;
                    newmoment.key = g_MomentMappings[n].MomentType;
                    mMomentPositonsList.push_back(newmoment);
                }
            }
        }
    }
}

int SFXObj_MomentStrm::GetController(int Index) {
    return Index != 0 ? -1 : 2;
}

void SFXObj_MomentStrm::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 2) {
        m_p3DPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_MomentStrm::Destroy() {}

// UNSOLVED branching, but it's functionally matching
bool SFXObj_MomentStrm::ShouldStreamPlay(Attrib::Key key, bool IsQueueing, float dist_sqrd) {
    if (m_CurMoment == key && m_TimeBeforeRetrigger > 0.0f && key != Attrib::Hash::aud_moment_strm::key_tollbooth &&
        key != Attrib::Hash::aud_moment_strm::key_window) {
        return false;
    }

    if (m_CurMoment != 0 && static_cast<int>(bHoldStream) != 0 && (mHeldMoment != nullptr) && IsQueueing) {
        for (int num_play = 0; num_play < SndCamera::NumPlayers; num_play++) {
            float xdist = bAbs(SndCamera::GetWorldCarPos(num_play)->x - mHeldMoment->vPos.z);
            float ydist = bAbs(SndCamera::GetWorldCarPos(num_play)->y + mHeldMoment->vPos.x);

            if (xdist * xdist + ydist * ydist < dist_sqrd) {
                return false;
            }
        }
    }

    if (!GRaceStatus::Exists()) {
        return false;
    }

    if (GRaceStatus::Get().GetRaceParameters() != nullptr && !GRaceStatus::Get().GetActivelyRacing()) {
        return false;
    }

    Speech::SED_NISSFX *nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));
    if (nismgr->GetStreamType() != STRM_SFX_MOMENT) {
        if (nismgr->GetStreamChannel()->IsPlaying()) {
            return false;
        }
    }

    Attrib::Gen::aud_moment_strm momentstrm(key, 0, nullptr);
    char streampriority = momentstrm.strmpriority();

    if (m_CurMoment != 0) {
        if (IsQueueing && !momentstrm.CanInterupt()) {
            return false;
        }

        Attrib::Gen::aud_moment_strm curmoment(m_CurMoment, 0, nullptr);

        if (curmoment.strmpriority() > streampriority) {
            return false;
        }
    }

    return true;
}

extern int IsAudioStreamingEnabled; // Decl: 263
extern int IsNISAudioEnabled;       // size: 0x4, Decl: 264

void SFXObj_MomentStrm::CommitStreamReq(UMath::Vector4 pos4, unsigned int collectionkey) {
    Attrib::Gen::aud_moment_strm momentstrm(collectionkey, 0, nullptr);
    Speech::SED_NISSFX *nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));

    this->m_CurMoment = collectionkey;
    this->mCarsID = 0;
    this->fPosition.x = pos4.z;
    this->fPosition.y = -pos4.x;
    this->fPosition.z = pos4.y;
    this->fVector = bVector3(0.0f, 0.0f, 0.0f);
    this->fVelocity = this->fVector;

    UMath::Vector3 pos3 = UMath::Vector4To3(pos4);

    if (UMath::Distance(pos3, UMath::Vector3::kZero) < 1.0f) {
        this->UseUserPos = true;
    } else {
        this->UseUserPos = false;
    }

    Csis::SoundFX_Select(momentstrm.stream(), momentstrm.param());
    nismgr->ClearStream();
    nismgr->GetStreamChannel()->Stop();

    this->VolSlot = momentstrm.VolSlot();
    this->m_IsPositioned = momentstrm.IsPositioned();
    this->bHoldStream = false;
    this->mHeldMoment = nullptr;

    bool breturn = nismgr->QueStream(STRM_SFX_MOMENT, CBPlayMomentStream, false);
}

void SFXObj_MomentStrm::ReceiveMoment(const MGamePlayMoment &message) {
    if (IsAudioStreamingEnabled == 0 || IsNISAudioEnabled == 0) {
        return;
    }

    unsigned int collectionkey = message.GetAttribKey();
    unsigned int unpause = Attrib::StringToKey("unpause");

    if (collectionkey == unpause) {
        UMath::Vector4 pos4 = message.GetPosition();

        for (int n = 0; n < static_cast<int>(this->mMomentPositonsList.size()); n++) {
            if (bAbs(pos4.x - this->mMomentPositonsList[n].vPos.x) < 25.0f && bAbs(pos4.z - this->mMomentPositonsList[n].vPos.z) < 25.0f) {
                collectionkey = this->mMomentPositonsList[n].key;
            }
        }

        if (collectionkey == unpause) {
            return;
        }
    }

    if (this->bHoldStream && this->m_CurMoment != 0 && collectionkey == this->m_CurMoment) {
        this->bHoldStream = false;
        this->mHeldMoment = nullptr;
        this->CBPlayMomentStream();
    } else if (this->ShouldStreamPlay(collectionkey, false, 0.0f)) {
        this->CommitStreamReq(message.GetPosition(), collectionkey);

        if (collectionkey == Attrib::Hash::aud_moment_strm::key_car2car || collectionkey == Attrib::Hash::aud_moment_strm::key_collision) {
            this->mCarsID = message.GethSimable();
        } else {
            this->mCarsID = 0;
        }

        this->bHoldStream = false;
        this->mHeldMoment = nullptr;
    }
}

void SFXObj_MomentStrm::CBPlayMomentStream() {
    if (!bHoldStream) {
        if (g_MomentStream != nullptr) {
            Attrib::Gen::aud_moment_strm momentstrm(g_MomentStream->m_CurMoment, 0, nullptr);

            if (momentstrm.GetParent() == Attrib::Hash::aud_moment_strm::key_pursuit_breaker_moments) {
                g_MomentStream->SetDMIX_Input(5, 0x7FFF);
            }
        }

        bool bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(2);
        Speech::Manager::GetSpeechModule(0)->UnPause();
        m_TimeBeforeRetrigger = 2.0f;
    }
}

static const float MOMENT_QUEUE_RADIUS = 20.0f; // size: 0x4, Decl: 427

static const int DEBUG_DRAW_STREAM_POS = 0; // size: 0x4, Decl: 429

// TODO dwarf
void SFXObj_MomentStrm::UpdateParams(float t) {
    this->SndBase::UpdateParams(t);

    this->m_TimeBeforeRetrigger -= t;
    if (this->m_TimeBeforeRetrigger < 0.0f) {
        this->m_TimeBeforeRetrigger = 0.0f;
    }

    if (this->UseUserPos) {
        this->fPosition = *SndCamera::GetWorldCarPos3(0);
    }

    if (this->mCarsID != 0) {
        EAX_CarState *pcar = EAX_CarState::Find(this->mCarsID);

        if (pcar != nullptr) {
            this->fPosition = *pcar->GetPosition();
        }
    }

    Speech::SED_NISSFX *nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));

    if (this->m_CurMoment != 0) {
        if (this->bHoldStream && this->mHeldMoment != nullptr) {
            this->bHoldStream = false;

            for (int num_play = 0; num_play < SndCamera::NumPlayers; ++num_play) {
                float xdist = bAbs(SndCamera::GetWorldCarPos(num_play)->x - this->mHeldMoment->vPos.z);
                float ydist = bAbs(SndCamera::GetWorldCarPos(num_play)->y + this->mHeldMoment->vPos.x);

                if (xdist < MOMENT_QUEUE_RADIUS || ydist < MOMENT_QUEUE_RADIUS) {
                    this->bHoldStream = true;
                    break;
                }
            }

            if (!this->bHoldStream) {
                nismgr->GetStreamChannel()->PurgeStream();
                this->m_CurMoment = 0;
                this->mHeldMoment = nullptr;
                this->mCarsID = 0;
            }
        }

        if (nismgr->GetStreamType() == STRM_SFX_MOMENT && !nismgr->GetStreamChannel()->IsPlaying()) {
            this->m_CurMoment = 0;
            this->mCarsID = 0;
        } else if (nismgr->GetStreamType() != STRM_SFX_MOMENT) {
            this->m_CurMoment = 0;
            this->mCarsID = 0;
        }

        if (!this->UseUserPos) {
            EAX_CarState *pcar = GetClosestPlayerCar(&this->fPosition);

            if (bDistBetween(&this->fPosition, pcar->GetPosition()) > 120.0f) {
                this->m_CurMoment = 0;
                nismgr->GetStreamChannel()->PurgeStream();
            }
        }
    }

    for (int num_play = 0; num_play < SndCamera::NumPlayers; ++num_play) {
        for (int n = 0; n < static_cast<int>(this->mMomentPositonsList.size()); ++n) {
            if (this->mMomentPositonsList[n].key != this->m_CurMoment) {
                float xdist = bAbs(SndCamera::GetWorldCarPos(num_play)->x - this->mMomentPositonsList[n].vPos.z);
                float ydist = bAbs(SndCamera::GetWorldCarPos(num_play)->y + this->mMomentPositonsList[n].vPos.x);

                if (xdist <= MOMENT_QUEUE_RADIUS && ydist <= MOMENT_QUEUE_RADIUS &&
                    this->ShouldStreamPlay(this->mMomentPositonsList[n].key, true, xdist * xdist + ydist * ydist)) {
                    this->CommitStreamReq(this->mMomentPositonsList[n].vPos, this->mMomentPositonsList[n].key);
                    this->bHoldStream = true;
                    this->mHeldMoment = &this->mMomentPositonsList[n];
                }
            }
        }
    }
}

void SFXObj_MomentStrm::ProcessUpdate() {
    SndBase::ProcessUpdate();
    SetDMIX_Input(5, 0);
}

void SFXObj_MomentStrm::ReceivePursuitBreaker(const MPursuitBreaker &message) {
    int id = 0x40010010; // TODO magic
    SFXObj_PFEATrax *peatrax = static_cast<SFXObj_PFEATrax *>(g_pEAXSound->GetSFXBase_Object(id));
    eMUSIC_TYPE etype = peatrax->GetMusicType();

    if (message.GetStartBreaker()) {
        bool IsWorldDataStreaming(unsigned int strmhandle);

        if (!IsWorldDataStreaming(0)) {
            if (etype == eMUSIC_TYPE_INTERACTIVE) {
                MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0,
                                Attrib::Hash::aud_moment_strm::key_purs_break_start_music)
                    .Send(UCrc32("MomentStrm"));
            } else {
                MGamePlayMoment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0,
                                Attrib::Hash::aud_moment_strm::key_purs_break_start_fx)
                    .Send(UCrc32("MomentStrm"));
            }
        }

        MMiscSound(3).Send(UCrc32("Snd"));
    } else {
        if (this->m_CurMoment == Attrib::Hash::aud_moment_strm::key_purs_break_start_fx ||
            this->m_CurMoment == Attrib::Hash::aud_moment_strm::key_purs_break_start_music) {
            Speech::Manager::GetSpeechModule(0)->GetStreamChannel()->Stop();
        }

        MMiscSound(4).Send(UCrc32("Snd"));
    }
}

DEFINE_CREATABLE(0x20, SFXCTL_3DMomentPos, SFXCTL_3DObjPos);
