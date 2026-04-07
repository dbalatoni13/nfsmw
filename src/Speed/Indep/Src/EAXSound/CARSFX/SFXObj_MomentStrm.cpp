#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_MomentStrm.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_PFEATrax.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/EAXSound/Stream/NISSFXModule.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Animation/AnimCandidates.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/NISAudio.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/World/TrackPositionMarker.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

SFXObj_MomentStrm::TypeInfo SFXObj_MomentStrm::s_TypeInfo = {0x60, "SFXObj_MomentStrm", nullptr, SFXObj_MomentStrm::CreateObject};
float SFXObj_MomentStrm::m_TimeBeforeRetrigger = 0.0f;
bool SFXObj_MomentStrm::bHoldStream = false;
SFXObj_MomentStrm *g_MomentStream = nullptr;

SFXCTL_3DMomentPos::TypeInfo SFXCTL_3DMomentPos::s_TypeInfo = {0x20, "SFXCTL_3DMomentPos", nullptr, SFXCTL_3DMomentPos::CreateObject};

struct MomentMapping {
    int markerType;
    unsigned int key;
};

extern MomentMapping g_MomentMappings[15];
extern bool IsWorldDataStreaming(unsigned int strmhandle);

SFXObj_MomentStrm::TypeInfo *SFXObj_MomentStrm::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_MomentStrm::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_MomentStrm::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXObj_MomentStrm::GetStaticTypeInfo()->typeName, false) SFXObj_MomentStrm();
    }

    return new (SFXObj_MomentStrm::GetStaticTypeInfo()->typeName, true) SFXObj_MomentStrm();
}

SFXObj_MomentStrm::SFXObj_MomentStrm()
    : CARSFX()
    , mMsgReceiveMoment(Hermes::Handler::Create<MGamePlayMoment, SFXObj_MomentStrm, SFXObj_MomentStrm>(
          this, &SFXObj_MomentStrm::ReceiveMoment, UCrc32("MomentStrm"), 0)) //
    , mMsgPursuitBreaker(Hermes::Handler::Create<MPursuitBreaker, SFXObj_MomentStrm, SFXObj_MomentStrm>(
          this, &SFXObj_MomentStrm::ReceivePursuitBreaker, UCrc32("PursuitBreaker"), 0)) //
    , mMomentPositonsList() {
    m_p3DPos = nullptr;
    VolSlot = eVOL_MOMENT_GASPUMP;
    fPosition = bVector3(0.0f, 0.0f, 0.0f);
    fVector = bVector3(0.0f, 0.0f, 0.0f);
    fVelocity = bVector3(0.0f, 0.0f, 0.0f);
    m_CurMoment = 0;
    mHeldMoment = nullptr;
    UseUserPos = false;
    mCarsID = 0;
    mbUseTRafficsID = false;
    bHoldStream = false;
    g_MomentStream = this;
}

SFXObj_MomentStrm::~SFXObj_MomentStrm() {
    g_MomentStream = nullptr;
    Destroy();

    if (mMsgReceiveMoment) {
        Hermes::Handler::Destroy(mMsgReceiveMoment);
    }

    if (mMsgPursuitBreaker) {
        Hermes::Handler::Destroy(mMsgPursuitBreaker);
    }

    mMomentPositonsList.clear();
    g_pEAXSound->SetSFXBaseObject(nullptr, eMM_MAIN, 6, 0);
}

void SFXObj_MomentStrm::SetupSFX(CSTATE_Base *_StateBase) {
    SndBase::SetupSFX(_StateBase);
}

int SFXObj_MomentStrm::GetController(int Index) {
    return Index == 0 ? 2 : -1;
}

void SFXObj_MomentStrm::AttachController(SFXCTL *psfxctl) {
    if (psfxctl->GetObjectIndex() == 2) {
        m_p3DPos = static_cast<SFXCTL_3DObjPos *>(psfxctl);
    }
}

void SFXObj_MomentStrm::Destroy() {}

void SFXObj_MomentStrm::UpdateParams(float t) {
    Speech::SED_NISSFX *nismgr;

    SndBase::UpdateParams(t);
    m_TimeBeforeRetrigger -= t;
    if (m_TimeBeforeRetrigger < 0.0f) {
        m_TimeBeforeRetrigger = 0.0f;
    }

    if (UseUserPos != 0) {
        fPosition = *SndCamera::GetWorldCarPos3(0);
    }

    if (mCarsID != 0) {
        {
            EAX_CarState *pcar = EAX_CarState::Find(mCarsID);

            if (pcar) {
                fPosition = *pcar->GetPosition();
            }
        }
    }

    nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));
    if (m_CurMoment != 0) {
        if (bHoldStream != 0 && mHeldMoment) {
            bHoldStream = false;

            for (int num_play = 0; num_play < SndCamera::NumPlayers; num_play++) {
                float xdist = bAbs(SndCamera::GetWorldCarPos(num_play)->x - mHeldMoment->vPos.z);
                float ydist = bAbs(SndCamera::GetWorldCarPos(num_play)->y + mHeldMoment->vPos.x);

                if (xdist < 20.0f || ydist < 20.0f) {
                    bHoldStream = true;
                    break;
                }
            }

            if (bHoldStream == 0) {
                nismgr->GetStreamChannel()->PurgeStream();
                mCarsID = 0;
                m_CurMoment = 0;
                mHeldMoment = nullptr;
            }
        }

        if (nismgr->GetStreamType() == STRM_SFX_MOMENT) {
            if (!nismgr->GetStreamChannel()->IsPlaying()) {
                mCarsID = 0;
                m_CurMoment = 0;
            }
        } else {
            mCarsID = 0;
            m_CurMoment = 0;
        }

        if (UseUserPos == 0) {
            {
                EAX_CarState *pcar;

                pcar = GetClosestPlayerCar(&fPosition);
                if (bDistBetween(&fPosition, pcar->GetPosition()) > 120.0f) {
                    m_CurMoment = 0;
                    nismgr->GetStreamChannel()->PurgeStream();
                }
            }
        }
    }

    for (int num_play = 0; num_play < SndCamera::NumPlayers; num_play++) {
        for (int n = 0; n < static_cast<int>(mMomentPositonsList.size()); n++) {
            if (mMomentPositonsList[n].key != m_CurMoment) {
                float xdist = bAbs(SndCamera::GetWorldCarPos(num_play)->x - mMomentPositonsList[n].vPos.z);
                float ydist = bAbs(SndCamera::GetWorldCarPos(num_play)->y + mMomentPositonsList[n].vPos.x);

                if (xdist <= 20.0f && ydist <= 20.0f &&
                    ShouldStreamPlay(mMomentPositonsList[n].key, true, xdist * xdist + ydist * ydist)) {
                    CommitStreamReq(mMomentPositonsList[n].vPos, mMomentPositonsList[n].key);
                    bHoldStream = true;
                    mHeldMoment = &mMomentPositonsList[n];
                }
            }
        }
    }
}

void SFXObj_MomentStrm::ProcessUpdate() {
    SetDMIX_Input(5, 0);
}

void SFXObj_MomentStrm::CBPlayMomentStream() {
    if (bHoldStream == 0) {
        bool bresult;

        if (g_MomentStream) {
            Attrib::Gen::aud_moment_strm momentstrm(g_MomentStream->m_CurMoment, 0, nullptr);

            if (momentstrm.GetParent() == 0x5A3E90B0) {
                g_MomentStream->SetDMIX_Input(5, 0x7FFF);
            }
        }

        bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(2);
        Speech::Manager::GetSpeechModule(0)->UnPause();
        m_TimeBeforeRetrigger = 2.0f;
    }
}

void SFXObj_MomentStrm::ReceiveMoment(const MGamePlayMoment &message) {
    unsigned int collectionkey;
    unsigned int unpause;

    if (IsAudioStreamingEnabled == 0) {
        return;
    }

    if (IsNISAudioEnabled == 0) {
        return;
    }

    collectionkey = message.GetAttribKey();
    unpause = Attrib::StringToKey("unpause");

    if (collectionkey == unpause) {
        UMath::Vector4 pos4 = message.GetPosition();

        for (int n = 0; n < static_cast<int>(mMomentPositonsList.size()); n++) {
            if (bAbs(pos4.x - mMomentPositonsList[n].vPos.x) < 25.0f &&
                bAbs(pos4.z - mMomentPositonsList[n].vPos.z) < 25.0f) {
                collectionkey = mMomentPositonsList[n].key;
            }
        }

        if (collectionkey == unpause) {
            return;
        }
    }

    if (bHoldStream != 0 && m_CurMoment != 0 && collectionkey == m_CurMoment) {
        bHoldStream = false;
        mHeldMoment = nullptr;
        CBPlayMomentStream();
    } else if (ShouldStreamPlay(collectionkey, false, 0.0f)) {
        CommitStreamReq(message.GetPosition(), collectionkey);

        if (collectionkey == 0x0D6D4198 || collectionkey == 0xA6E3EF3E) {
            mCarsID = message.GethSimable();
        } else {
            mCarsID = 0;
        }

        bHoldStream = false;
        mHeldMoment = nullptr;
    }
}

bool SFXObj_MomentStrm::ShouldStreamPlay(unsigned int key, bool IsQueueing, float dist_sqrd) {
    Speech::SED_NISSFX *nismgr;

    if (m_CurMoment == key && m_TimeBeforeRetrigger > 0.0f && key != 0x5AEFC6A8 && key != 0xA80F0E08) {
        return false;
    }

    if (m_CurMoment != 0 && bHoldStream != 0 && mHeldMoment && IsQueueing) {
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

    if (GRaceStatus::Get().GetRaceParameters()) {
        if (!GRaceStatus::Get().GetActivelyRacing()) {
            return false;
        }
    }

    if ((nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0)))->GetStreamType() != STRM_SFX_MOMENT) {
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

void SFXObj_MomentStrm::CommitStreamReq(UMath::Vector4 pos4, unsigned int collectionkey) {
    Attrib::Gen::aud_moment_strm momentstrm(collectionkey, 0, nullptr);
    Speech::SED_NISSFX *nismgr;

    nismgr = static_cast<Speech::SED_NISSFX *>(Speech::Manager::GetSpeechModule(0));
    m_CurMoment = collectionkey;
    mCarsID = 0;
    fPosition.x = pos4.z;
    fPosition.y = -pos4.x;
    fPosition.z = pos4.y;
    fVector = bVector3(0.0f, 0.0f, 0.0f);
    fVelocity = fVector;

    {
        UMath::Vector3 pos3;

        pos3 = UMath::Vector4To3(pos4);
        if (UMath::Distance(pos3, UMath::Vector3::kZero) < 1.0f) {
            UseUserPos = true;
        } else {
            UseUserPos = false;
        }
    }

    Csis::SoundFX_Select(momentstrm.stream(), momentstrm.param());
    nismgr->ClearStream();
    nismgr->GetStreamChannel()->Stop();
    VolSlot = momentstrm.VolSlot();
    m_IsPositioned = momentstrm.IsPositioned();
    bHoldStream = false;
    mHeldMoment = nullptr;
    {
        bool breturn;

        breturn = nismgr->QueStream(STRM_SFX_MOMENT, CBPlayMomentStream, false);
    }
}

void SFXObj_MomentStrm::InitSFX() {
    SndBase::InitSFX();
    m_p3DPos->AssignPositionVector(&fPosition);
    m_p3DPos->AssignDirectionVector(&fVector);
    m_p3DPos->AssignVelocityVector(&fVelocity);
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 6, 0);
    m_TimeBeforeRetrigger = 0.0f;
    mMomentPositonsList.clear();

    for (int n = 0; n <= 14; n++) {
        const char *markerName = CAnimCandidateData::GetMomentMarkerName(g_MomentMappings[n].markerType);

        if (markerName[0] != '\0') {
            unsigned int markerHash = bStringHash(markerName);
            int numTrackMarkers = GetNumTrackPositionMarkers(0, markerHash);
            float closestMarkerDist;

            for (int index = 0; index < numTrackMarkers; index++) {
                TrackPositionMarker *marker = GetTrackPositionMarker(markerHash, index);

                if (marker) {
                    stMomentDecription newmoment;

                    newmoment.vPos = UMath::Vector4::kZero;
                    newmoment.key = 0;
                    newmoment.vPos.z = marker->Position.x;
                    newmoment.vPos.x = -marker->Position.y;
                    newmoment.key = g_MomentMappings[n].key;
                    newmoment.vPos.y = marker->Position.z;
                    newmoment.vPos.w = 0.0f;
                    mMomentPositonsList.push_back(newmoment);
                }
            }
        }
    }
}

void SFXObj_MomentStrm::ReceivePursuitBreaker(const MPursuitBreaker &message) {
    int id = 0x40010010;
    SFXObj_PFEATrax *peatrax = static_cast<SFXObj_PFEATrax *>(g_pEAXSound->GetSFXBase_Object(id));
    eMUSIC_TYPE etype = peatrax->GenMusicType();

    if (message.GetStartBreaker()) {
        if (!IsWorldDataStreaming(0)) {
            if (etype == eMUSIC_TYPE_INTERACTIVE) {
                MGamePlayMoment moment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x2DF96F7D);

                moment.Send(UCrc32("MomentStrm"));
            } else {
                MGamePlayMoment moment(UMath::Vector4::kZero, UMath::Vector4::kZero, UMath::Vector4::kZero, 0, 0x14D32C41);

                moment.Send(UCrc32("MomentStrm"));
            }
        }

        MMiscSound sound(3);

        sound.Send(UCrc32("Snd"));
    } else {
        if (m_CurMoment == 0x14D32C41 || m_CurMoment == 0x2DF96F7D) {
            Speech::Manager::GetSpeechModule(0)->GetStreamChannel()->Stop();
        }

        MMiscSound sound(4);

        sound.Send(UCrc32("Snd"));
    }
}

SFXCTL_3DMomentPos::TypeInfo *SFXCTL_3DMomentPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DMomentPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DMomentPos::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXCTL_3DMomentPos::GetStaticTypeInfo()->typeName, false) SFXCTL_3DMomentPos();
    }

    return new (SFXCTL_3DMomentPos::GetStaticTypeInfo()->typeName, true) SFXCTL_3DMomentPos();
}
