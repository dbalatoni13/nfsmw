#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_MomentStrm.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

SFXObj_MomentStrm::TypeInfo SFXObj_MomentStrm::s_TypeInfo = {0x60, "SFXObj_MomentStrm", nullptr, SFXObj_MomentStrm::CreateObject};
float SFXObj_MomentStrm::m_TimeBeforeRetrigger = 0.0f;
bool SFXObj_MomentStrm::bHoldStream = false;
SFXObj_MomentStrm *g_MomentStream = nullptr;

SFXCTL_3DMomentPos::TypeInfo SFXCTL_3DMomentPos::s_TypeInfo = {0x20, "SFXCTL_3DMomentPos", nullptr, SFXCTL_3DMomentPos::CreateObject};

SFXObj_MomentStrm::TypeInfo *SFXObj_MomentStrm::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_MomentStrm::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_MomentStrm::CreateObject(unsigned int allocator) {
    return new (GetStaticTypeInfo()->typeName, false) SFXObj_MomentStrm();
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

void SFXObj_MomentStrm::ProcessUpdate() {
    SetDMIX_Input(5, 0);
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

SFXCTL_3DMomentPos::TypeInfo *SFXCTL_3DMomentPos::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXCTL_3DMomentPos::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXCTL_3DMomentPos::CreateObject(unsigned int allocator) {
    return new (GetStaticTypeInfo()->typeName, false) SFXCTL_3DMomentPos();
}
