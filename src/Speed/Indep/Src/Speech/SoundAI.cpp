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
    Sim::IActivity *activity;

    if ((IsSoundEnabled != 0) && (Sim::GetUserMode() != Sim::USER_SPLIT_SCREEN)) {
        SoundAI *instance = UTL::Collections::Singleton<SoundAI>::Get();
        if (instance) {
            activity = static_cast<Sim::IActivity *>(instance);
            SoundAI::mRefCount = SoundAI::mRefCount + 1;
        } else {
            SoundAI *result = new SoundAI;
            activity = 0;
            if (result) {
                activity = static_cast<Sim::IActivity *>(result);
            }
        }
    } else {
        activity = 0;
    }
    return activity;
}

IRoadBlock *SoundAI::GetRoadblock() {
    IPursuit *pursuit = mPursuit;

    if (pursuit) {
        return pursuit->GetRoadBlock();
    }

    const UTL::Collections::Listable<IRoadBlock, 8>::List &blocks = UTL::Collections::Listable<IRoadBlock, 8>::GetList();
    UTL::Collections::Listable<IRoadBlock, 8>::List::const_iterator i = blocks.begin();
    while (i != blocks.end()) {
        IRoadBlock *rb = *i;
        if (rb) {
            pursuit = rb->GetPursuit();
            if (pursuit) {
                int active = (**reinterpret_cast<int (**)(void *)>(*reinterpret_cast<int **>(reinterpret_cast<char *>(pursuit) + 4) + 0x124 / 4))(
                    reinterpret_cast<char *>(pursuit) + *reinterpret_cast<short *>(*reinterpret_cast<int *>(reinterpret_cast<char *>(pursuit) + 4) + 0x120)
                );
                if (active) {
                    return rb;
                }
            }
        }
        ++i;
    }
    return 0;
}

bool SoundAI::IsMusicActive() {
    bool result;
    int i;
    char *music = reinterpret_cast<char *>(mMusicFlow);

    if (music) {
        i = (**reinterpret_cast<int (**)(void *)>(*reinterpret_cast<int **>(music + 0xC) + 0x2C / 4))(music + *reinterpret_cast<short *>(*reinterpret_cast<int *>(music + 0xC) + 0x28));
        result = true;
        if (i == -1) {
            result = false;
        }
    } else {
        result = false;
    }
    return result;
}

EAXCop *SoundAI::GetCopInRB() {
    IRoadBlock *block = GetRoadblock();

    if (block) {
        Speech::copMap::iterator iter = mActors.begin();
        while (iter != mActors.end()) {
            EAXCop *cop = iter->cop;
            if ((cop->GetInFormation() != 0) && !cop->IsPrimary()) {
                if (block->IsComprisedOf(cop->GetHandle())) {
                    return cop;
                }
            }
            ++iter;
        }
    }
    return 0;
}

EAXCop *SoundAI::GetCop(int speaker) {
    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop && cop->GetSpeakerID() == speaker) {
            return iter->cop;
        }
        ++iter;
    }
    return 0;
}

void SoundAI::RandomBailoutDeny(EAXCop *wimp) {
    if (wimp->GetInFormation() == 1) {
        if ((mActors.size() > 1) && (mPursuitState != kInactive)) {
            Speech::copMap::iterator iter = mActors.begin();
            while (iter != mActors.end()) {
                EAXCop *cop = iter->cop;
                if (cop->IsPrimary()) {
                    if (cop->GetSpeakerID() != wimp->GetSpeakerID()) {
                        cop->DenyBailout();
                    }
                }
                ++iter;
            }
        }
    }
}

int SoundAI::GetVoice(int type) {
    int return_voice;
    Speech::voiceIDs::iterator i = mUsage.voices.begin();
    Speech::voiceIDs::iterator end = mUsage.voices.end();

    if (i == end) {
        return -1;
    }

    if (type == 2) {
        while (i != end) {
            return_voice = *i;
            if (static_cast<unsigned int>(return_voice - 6) < 3) {
                mUsage.voices.erase(i);
                return return_voice;
            }
            ++i;
            end = mUsage.voices.end();
        }
        return -1;
    }

    if (type < 3) {
        if (type == 1) {
            while (i != end) {
                return_voice = *i;
                if (static_cast<unsigned int>(return_voice - 3) < 3) {
                    mUsage.voices.erase(i);
                    return return_voice;
                }
                ++i;
                end = mUsage.voices.end();
            }
            return -1;
        }
    } else if (type == 3) {
        while (i != end) {
            return_voice = *i;
            if (return_voice == 9) {
                mUsage.voices.erase(i);
                return 9;
            }
            ++i;
            end = mUsage.voices.end();
        }
        return -1;
    }

    while (i != end) {
        return_voice = *i;
        if (static_cast<unsigned int>(return_voice - 3) < 6) {
            mUsage.voices.erase(i);
            return return_voice;
        }
        ++i;
        end = mUsage.voices.end();
    }
    return -1;
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

void SoundAI::AddNewHeli(IVehicle *heli) {
    HSIMABLE handle = heli->GetSimable()->GetOwnerHandle();
    EAXAirSupport *chopper = new EAXAirSupport(2, handle);
    int focus;

    mActors.Add(handle, chopper);
    focus = mFocus;
    mHeli = chopper;

    if (focus != 1) {
        if ((focus != 999) && (focus != 0)) {
            chopper->BackupArrives();
            return;
        }
        if (mFocus != 1) {
            return;
        }
    }
    mFlags |= HELI_INTRO_REQ;
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
