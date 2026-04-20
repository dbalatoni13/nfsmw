#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MMiscSound.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Generated/Messages/MRestartRace.h"

namespace MiscSpeech {
bool GetLocation(RoadNames road, int &region, int &location) asm("GetLocation__10MiscSpeech9RoadNamesRQ24Csis20Type_location_regionRQ24Csis13Type_location");
}

namespace Speech {
void Observer_Observe(void *observer, int observation, int speaker, float score) asm("Observe__Q26Speech8Observeriif");
void RoadblockFlow_NailedSomethingInRB(void *rbflow, unsigned int key) asm("NailedSomethingInRB__Q26Speech13RoadblockFlowUi");
}

extern float lbl_80407BA8;

void SoundAI::MessagePerpBusted(const MPerpBusted &) {
    mFocus = kTerminal;
    if (mMusicFlow) {
        MControlPathfinder msg(false, 0xF, 0, 0);
        msg.Send(UCrc32("Event"));
    }
    mFlags |= BUSTED;
}

void SoundAI::MessageAIPerpBusted(const MPerpBusted &) {
    if (mPursuitState != kInactive) {
        EAXCop *cop = FindFurthestCop(true);
        if (cop) {
            cop->Arrest();
        }
    }
}

void SoundAI::MessageInfraction(const MMiscSound &message) {
    mInfraction = message.GetSoundID();
}

void SoundAI::MessageRestart(const MRestartRace &) {
    ResetPursuit(true);
    Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
    if (cop_speech) {
        cop_speech->ReleaseResource();
    }
    Speech::Manager::ClearPlayback();
}

void SoundAI::MessageUnspawnCop(const MUnspawnCop &message) {
    EAXCop *cop = mActors.Find(message.GetCopHandle());
    int param;

    if (!cop) {
        return;
    }
    param = message.GetParam();
    if (param != 2) {
        if (param < 3) {
            if (param == 0) {
                goto remove_cop;
            }
            if (param == 1) {
                goto deactivate_cop;
            }
            goto remove_cop;
        }
        if (param < 7) {
deactivate_cop:
            cop->SetActive(false);
            return;
        }
    }
remove_cop:
    RemoveCop(message.GetCopHandle());
}

void SoundAI::MessageTireBlown(const MGamePlayMoment &) {
    EAXCop *spkr;
    void *observer;
    int speaker;

    if (mRoadblockFlow) {
        Speech::RoadblockFlow_NailedSomethingInRB(mRoadblockFlow, 0x10);
    }
    spkr = FindClosestCop(true, true);
    observer = mObserver;
    if (observer && spkr) {
        speaker = spkr->GetSpeakerID();
        Speech::Observer_Observe(observer, 0xE, speaker, lbl_80407BA8);
    }
}

void SoundAI::OnVehicleAdded(IVehicle *ivehicle) {
    Sim::Collision::AddListener(this, ivehicle, "SoundAI");
}

void SoundAI::OnAttached(IAttachable *pOther) {
    bool attached;
    IVehicle *ivehicle = 0;

    attached = pOther->QueryInterface(&ivehicle);
    if (attached) {
        OnVehicleAdded(ivehicle);
    }
}

void SoundAI::OnVehicleRemoved(IVehicle *ivehicle) {
    Sim::Collision::RemoveListener(this, ivehicle);

    EAXCop *cop = *reinterpret_cast<EAXCop **>(reinterpret_cast<char *>(mObserver) + 0x60);
    if (cop) {
        EAXCop *actor = mActors.Find(ivehicle->GetSimable()->GetOwnerHandle());
        if (actor == cop) {
            *reinterpret_cast<EAXCop **>(reinterpret_cast<char *>(mObserver) + 0x60) = 0;
        }
    }
}

void SoundAI::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle = 0;
    IPursuit *pursuit;
    bool detached;
    Timer t = WorldTimer;

    detached = pOther->QueryInterface(&ivehicle);
    if (detached) {
        OnVehicleRemoved(ivehicle);
    }

    detached = UTL::COM::ComparePtr(mPursuit, pOther);
    if (detached) {
        PursuitState state = kInactive;
        pursuit = mAIPursuit;
        mPursuit = 0;
        if (pursuit) {
            state = kOtherTarget;
        }
        mT_pursuitStart = t;
        mPursuitState = state;
    } else {
        pursuit = mAIPursuit;
    }

    detached = UTL::COM::ComparePtr(pursuit, pOther);
    if (detached) {
        mAIPursuit = 0;
        if ((mPursuitState == kOtherTarget) && !mPursuit) {
            mPursuitState = kInactive;
        }
    }
}

Sim::IActivity *SoundAI::Construct(Sim::Param) {
    if ((IsSoundEnabled == 0) || (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN)) {
        return 0;
    }

    SoundAI *instance = UTL::Collections::Singleton<SoundAI>::Get();
    if (instance) {
        Sim::IActivity *activity = static_cast<Sim::IActivity *>(instance);
        SoundAI::mRefCount = SoundAI::mRefCount + 1;
        return activity;
    }

    SoundAI *result = new SoundAI;
    if (result) {
        return static_cast<Sim::IActivity *>(result);
    }
    return 0;
}

bool SoundAI::IsMusicActive() {
    bool result;
    int i;
    char *music = reinterpret_cast<char *>(mMusicFlow);

    if (!music) {
        result = false;
    } else {
        i = (**reinterpret_cast<int (**)(void *)>(*reinterpret_cast<int **>(music + 0xC) + 0x2C / 4))(music + *reinterpret_cast<short *>(*reinterpret_cast<int *>(music + 0xC) + 0x28));
        result = true;
        if (i == -1) {
            result = false;
        }
    }
    return result;
}

void SoundAI::Release() {
    if ((mRefCount != 0) && ((mRefCount = mRefCount - 1) == 0)) {
        Sim::Activity::Release();
    }
}

bool SoundAI::IsHeadingValid() {
    if ((static_cast<unsigned int>(mPlayerCurrent[0].roadID - on_Highway99) < 0x50) && (mPlayerCurrent[0].direction != 0)) {
        return true;
    }
    return false;
}

int SoundAI::GetBattalionFromRoadID(int roadID) {
    int region;
    int location;
    bool result = MiscSpeech::GetLocation(static_cast<RoadNames>(roadID), region, location);

    if (!result) {
        return -1;
    }
    if (region != 2) {
        if (region < 3) {
            if (region == 1) {
                return 1;
            }
            return -1;
        }
        if (region != 4) {
            if (region != 8) {
                return -1;
            }
            return 4;
        } else {
            return 2;
        }
    }
    return 2;
}

int SoundAI::GetBattalionFromKey(unsigned int theKey) {
    if ((theKey == 0x38B38226) || (theKey == 0x54B10E38) || (theKey == 0x2E149EAC)) {
        return 0x20;
    }
    if ((theKey == 0x7A49CCCB) || (theKey == 0xA4EB6688) || (theKey == 0xB2F32FE2) || (theKey == 0xD37C806D)) {
        return 0x10;
    }
    return -1;
}

bool SoundAI::IsHighIntensity() {
    if ((mPlayerHeat >= static_cast<int>(mTune.HighIntensityMark())) || (mPursuitDuration >= mTune.PursuitDurationHighIntensity())) {
        return true;
    }
    return false;
}
