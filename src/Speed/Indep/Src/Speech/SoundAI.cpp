#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IRenderable.h"
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
void LostSuspect(int speaker) asm("LostSuspect__10MiscSpeechi");
void Bailout(int speaker) asm("Bailout__10MiscSpeechi");
}

namespace Speech {
void Observer_Observe(void *observer, int observation, int speaker, float score) asm("Observe__Q26Speech8Observeriif");
void RoadblockFlow_NailedSomethingInRB(void *rbflow, unsigned int key) asm("NailedSomethingInRB__Q26Speech13RoadblockFlowUi");
bool Manager_IsCopSpeechBusy() asm("IsCopSpeechBusy__Q26Speech7Manager");
Timer Manager_GetTimeSinceLastEvent(SpeechModuleIndex module) asm("GetTimeSinceLastEvent__Q26Speech7Manager17SpeechModuleIndex");
void SpeechFlow_ChangeStateTo(void *flow, int state) asm("ChangeStateTo__Q26Speech10SpeechFlowi");
int SpeechFlow_GetState(void *flow) asm("GetState__Q26Speech10SpeechFlow");
void PursuitFlow_Update(void *flow) asm("Update__Q26Speech11PursuitFlow");
int PursuitFlow_IsTransitionable(void *flow) asm("IsTransitionable__Q26Speech11PursuitFlow");
void StrategyFlow_Update(void *flow) asm("Update__Q26Speech12StrategyFlow");
int StrategyFlow_IsTransitionable(void *flow) asm("IsTransitionable__Q26Speech12StrategyFlow");
void MusicFlow_Update(void *flow) asm("Update__Q26Speech9MusicFlow");
void Observer_Update(void *observer) asm("Update__Q26Speech8Observer");
void RoadblockFlow_Update(void *flow) asm("Update__Q26Speech13RoadblockFlow");
void PursuitFlow_Reset(void *flow) asm("Reset__Q26Speech11PursuitFlow");
void StrategyFlow_Reset(void *flow) asm("Reset__Q26Speech12StrategyFlow");
void RoadblockFlow_Reset(void *flow) asm("Reset__Q26Speech13RoadblockFlow");
void PursuitFlow_OnCopRemoved(void *flow, EAXCop *cop) asm("OnCopRemoved__Q26Speech11PursuitFlowP6EAXCop");
void Observer_Reset(void *observer) asm("Reset__Q26Speech8Observer");
void MusicFlow_Reset(void *flow) asm("Reset__Q26Speech9MusicFlow");
void Manager_ResetGlobalHistory() asm("ResetGlobalHistory__Q26Speech7Manager");
}

extern float lbl_80407BA8;
extern int FORCE_VOICE_RANDOMIZATION;
extern bool IsSpeechEnabled;
extern short Speech_Manager_mLastSpeakerID asm("_Q26Speech7Manager.mLastSpeakerID");

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

bool SoundAI::OnTask(HSIMTASK htask, float) {
    float tout = 0.0f;

    if (!Speech::Manager_IsCopSpeechBusy()) {
        tout = (WorldTimer - Speech::Manager_GetTimeSinceLastEvent(COPSPEECH_MODULE)).GetSeconds();
    }
    mDeadAir = tout;

    if (htask == mMainUpdate) {
        if (FORCE_VOICE_RANDOMIZATION != 0) {
            ForceGlobalVoiceChange();
            FORCE_VOICE_RANDOMIZATION = 0;
        }
        if ((mFlags & BUSTED) == 0) {
            SyncPursuit();
            SyncCarsToActors();
            SyncPlayers();
            SyncFormations();
            ShuffleActors();
            UpdateStateMachines();
            if ((IsSpeechEnabled != 0) && (mDeadAir > 0.0f)) {
                DealWithDeadAir();
            }
        }
    }

    if (htask == mProcessObservations) {
        if ((mFlags & BUSTED) == 0) {
            Speech::Observer_Update(mObserver);
            Speech::RoadblockFlow_Update(mRoadblockFlow);
        }
        Speech::Manager::Deduce();
    }
    return true;
}

void SoundAI::UpdateStateMachines() {
    if ((IsSpeechEnabled == 0) || ((mFlags & BUSTED) != 0)) {
        Speech::MusicFlow_Update(mMusicFlow);
        return;
    }

    int focus = mFocus;
    if (focus == kPursuitFlow) {
        Speech::PursuitFlow_Update(mPursuitFlow);
        if (Speech::PursuitFlow_IsTransitionable(mPursuitFlow)) {
            mFocus = kStrategyFlow;
            Speech::SpeechFlow_ChangeStateTo(mStrategyFlow, 0);
        }
    } else if ((focus == kStrategyFlow) || (focus == kRoadblockFlow)) {
        if (Speech::StrategyFlow_IsTransitionable(mStrategyFlow)) {
            Speech::SpeechFlow_ChangeStateTo(mStrategyFlow, kWaiting);
        }
        Speech::StrategyFlow_Update(mStrategyFlow);
    } else if (focus == kWaiting) {
        if ((mPursuit != 0) && (mPursuitState == kSearching)) {
            if (Speech::SpeechFlow_GetState(mStrategyFlow) != kOtherTarget) {
                Speech::SpeechFlow_ChangeStateTo(mStrategyFlow, kOtherTarget);
            }
            Speech::StrategyFlow_Update(mStrategyFlow);
        }
    } else if (!mPursuit) {
        ResetPursuit(false);
    }

    Speech::MusicFlow_Update(mMusicFlow);
}

void SoundAI::AttemptReattachPursuit() {
    bool playerfound = false;
    bool aifound = false;

    if (mPursuit && mPursuit->IsPlayerPursuit()) {
        playerfound = true;
    }
    if (mAIPursuit && !mAIPursuit->IsPlayerPursuit()) {
        aifound = true;
    }

    const UTL::Collections::Listable<IPursuit, 8>::List &pursuits = UTL::Collections::Listable<IPursuit, 8>::GetList();
    UTL::Collections::Listable<IPursuit, 8>::List::const_iterator i = pursuits.begin();
    while (i != pursuits.end()) {
        IPursuit *pursuit = *i;
        if (pursuit->IsPlayerPursuit()) {
            playerfound = true;
            if (mPursuit != pursuit) {
                mPursuitCount++;
                if (!mPursuit) {
                    mT_pursuitStart = WorldTimer;
                }
                mPursuit = pursuit;
            }
        } else {
            if (!aifound) {
                aifound = true;
                mAIPursuit = pursuit;
            }
        }
        ++i;
    }

    if (!playerfound) {
        mPursuit = 0;
        mPursuitState = kInactive;
        mT_pursuitStart = WorldTimer;
    }

    if (!aifound) {
        mAIPursuit = 0;
        if ((mPursuitState == kOtherTarget) && !mPursuit) {
            mPursuitState = kInactive;
        }
    }
}

void SoundAI::DealWithDeadAir() {
    if (mPursuitState != kActive) {
        return;
    }
    if (!mLeader) {
        return;
    }
    if (mFocus == kPursuitFlow) {
        return;
    }

    if (mPursuitDuration > 60.0f) {
        mDispatch->PursuitUpdate(mLeader);
    }

    if ((WorldTimer - mT_noLOS).GetSeconds() < mTune.NoLOSCommentaryTime()) {
        if (mPursuitDuration > 60.0f) {
            if (IsHeadingValid() && (mPlayerOffroadID > -1) && (bRandom(1.0f) <= 0.5f)) {
                mLeader->LocationReport();
            } else {
                mLeader->PursuitUpdateReply();
            }
        }
        return;
    }

    if (!mHeli) {
        mLeader->LostVisual();
        return;
    }

    if (!mHeli->HasLOS()) {
        mHeli->LostVisual();
        return;
    }

    if (IsHeadingValid() && (mPlayerOffroadID > -1) && (bRandom(1.0f) <= 0.5f)) {
        mHeli->LocationReport();
    } else {
        mHeli->PursuitUpdateReply();
    }
}

EAXCop *SoundAI::GetRandomCop(int type) {
    EAXCop *spkr = 0;
    unsigned int actor_count = mActors.size();

    if ((type == 0) && (actor_count > 1)) {
        unsigned int idx = bRandom(static_cast<int>(actor_count));
        return mActors[idx].cop;
    }

    if (actor_count == 1) {
        EAXCop *cop = mActors.begin()->cop;
        if (type == 1) {
            if (!cop->IsPrimary()) {
                return 0;
            }
        } else if (type == 2) {
            if (cop->IsPrimary()) {
                return 0;
            }
        } else if (type != 0) {
            return 0;
        }
        return cop;
    }

    if ((type != 1) && (type != 2)) {
        return 0;
    }

    Speech::copList secondaries;
    secondaries.reserve(actor_count);

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if ((type == 1 && cop->IsPrimary()) || (type == 2 && !cop->IsPrimary())) {
            secondaries.push_back(cop);
        }
        ++iter;
    }

    if (!secondaries.empty()) {
        spkr = secondaries[bRandom(static_cast<int>(secondaries.size()))];
    }
    return spkr;
}

EAXCop *SoundAI::GetRandomActiveCop(int type, bool reqLOS) {
    Speech::copList active;
    active.reserve(mActors.size());

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive() && (!reqLOS || cop->HasLOS())) {
            active.push_back(cop);
        }
        ++iter;
    }

    if (active.empty()) {
        return 0;
    }

    if ((type == 0) && (active.size() > 1)) {
        return active[bRandom(static_cast<int>(active.size()))];
    }

    if (active.size() == 1) {
        EAXCop *cop = active.front();
        if (type == 1) {
            if (!cop->IsPrimary()) {
                return 0;
            }
        } else if (type == 2) {
            if (cop->IsPrimary()) {
                return 0;
            }
        } else if (type != 0) {
            return 0;
        }
        return cop;
    }

    if ((type != 1) && (type != 2)) {
        return 0;
    }

    Speech::copList secondaries;
    secondaries.reserve(active.size());
    Speech::copList::iterator i = active.begin();
    while (i != active.end()) {
        EAXCop *cop = *i;
        if ((type == 1 && cop->IsPrimary()) || (type == 2 && !cop->IsPrimary())) {
            secondaries.push_back(cop);
        }
        ++i;
    }

    if (!secondaries.empty()) {
        return secondaries[bRandom(static_cast<int>(secondaries.size()))];
    }
    return 0;
}

void SoundAI::RandomizeCallsign(Speech::voiceIDs &cs, Csis::Type_speaker_call_sign_id start, Csis::Type_speaker_call_sign_id finish) {
    if (cs.empty()) {
        int i = start;
        while (i <= finish) {
            cs.push_back(i);
            i <<= 1;
        }
        for (unsigned int ndx = 0; ndx < cs.size(); ndx++) {
            int rand = bRandom(static_cast<int>(cs.size()));
            int rand_cs = cs[rand];
            int curr_cs = cs[ndx];
            if (rand_cs != curr_cs) {
                cs[ndx] = rand_cs;
                cs[rand] = curr_cs;
            }
        }
    }
}

int SoundAI::GetCallsign(Csis::Type_speaker_battalion battalion) {
    Speech::voiceIDs *cs_pool = 0;

    switch (battalion) {
    case Csis::Type_speaker_battalion_City:
        cs_pool = &mUsage.cs_City;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign20);
        }
        break;
    case Csis::Type_speaker_battalion_Rosewood:
        cs_pool = &mUsage.cs_Rosewood;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
        }
        break;
    case Csis::Type_speaker_battalion_Coastal:
        cs_pool = &mUsage.cs_Coastal;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
        }
        break;
    case Csis::Type_speaker_battalion_Super_Pursuit:
        cs_pool = &mUsage.cs_SuperPursuit;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign05);
        }
        break;
    case Csis::Type_speaker_battalion_Alpine:
        cs_pool = &mUsage.cs_Alpine;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign10);
        }
        break;
    case Csis::Type_speaker_battalion_Rhino_Units:
        cs_pool = &mUsage.cs_Rhino;
        if (cs_pool->empty()) {
            RandomizeCallsign(*cs_pool, Csis::Type_speaker_call_sign_id_CallSign01, Csis::Type_speaker_call_sign_id_CallSign06);
        }
        break;
    default:
        break;
    }

    if (!cs_pool) {
        return -1;
    }

    Speech::voiceIDs::iterator iter = cs_pool->begin();
    int id = *iter;
    cs_pool->erase(iter);
    return id;
}

const float SoundAI::GetTimeLastNailedCop() {
    Speech::copList nailed;

    nailed.reserve(mActors.size());
    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop->GetTimesRammed() > 0) {
            nailed.push_back(cop);
        }
        ++iter;
    }

    if (nailed.empty()) {
        return 0.0f;
    }

    float t_mostrecent = nailed.front()->GetTimeLastRammed();
    Speech::copList::iterator i = nailed.begin();
    while (i != nailed.end()) {
        EAXCop *unfortunate = *i;
        if (unfortunate->GetTimeLastRammed() < t_mostrecent) {
            t_mostrecent = unfortunate->GetTimeLastRammed();
        }
        ++i;
    }
    return t_mostrecent;
}

void SoundAI::SyncFormations() {
    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (!cop->GetInFormation() || cop->IsHeli()) {
            Speech::copList::iterator i = mCopsInFormation.begin();
            while (i != mCopsInFormation.end()) {
                EAXCop *copInFormation = *i;
                if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
                    mCopsInFormation.erase(i);
                    break;
                }
                ++i;
            }
        } else {
            bool found = false;
            Speech::copList::iterator i = mCopsInFormation.begin();
            while (i != mCopsInFormation.end()) {
                EAXCop *copInFormation = *i;
                if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
                    found = true;
                    break;
                }
                ++i;
            }
            if (!found && !cop->IsHeli()) {
                mCopsInFormation.push_back(cop);
            }
        }
        ++iter;
    }

    if (mCopsInFormation.size() == 1) {
        mLastCopInFormation = mCopsInFormation.front();
    }
    if (!mCopsInFormation.empty()) {
        mT_outofFormation = WorldTimer;
    }
}

void SoundAI::ResetPursuit(bool including_music) {
    Speech::PursuitFlow_Reset(mPursuitFlow);
    Speech::StrategyFlow_Reset(mStrategyFlow);
    Speech::RoadblockFlow_Reset(mRoadblockFlow);
    Speech::Observer_Reset(mObserver);
    if (including_music) {
        Speech::MusicFlow_Reset(mMusicFlow);
    }
    Speech::Manager_ResetGlobalHistory();

    mFocus = kPursuitFlow;
    mQuadrantState = kReset;
    mRacerCount = 0;

    while (!mActors.empty()) {
        RemoveCop(mActors.begin()->hsimable);
    }

    mLeader = 0;
    mCopsInFormation.clear();
    mPursuit = 0;
    mAIPursuit = 0;
    mLastCopInFormation = 0;
    mLatestCop = 0;
    mHavoc = 0;
    mT_pursuitStart = WorldTimer;
    mPursuitState = kInactive;
    mPursuitDist = 0.0f;
    mT_lastCopNailed = Timer(0);
    mT_lastCrashed = Timer(0);
    mT_noLOS = Timer(0);
    mT_reallylowspeed = Timer(0);
    mT_outofFormation = Timer(0);
    mT_LOS = Timer(0);
    mCopsInView = 0;
    mInfraction = 0;
    mFlags = 0;
    mTrafficHits911 = 0;
    mCTS911 = 0;
}

void SoundAI::TerminatePursuit(BailoutType type) {
    if ((type == kOutrunBail) || (type != kForcedBail)) {
        bool cops_visible = false;
        const UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List &vehicles =
            UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::GetList(VEHICLE_AICOPS);
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, 10>::List::const_iterator i = vehicles.begin();
        while (i != vehicles.end()) {
            IVehicle *vehicle = *i;
            IRenderable *renderable = 0;
            if (vehicle->QueryInterface(&renderable) && renderable->InView()) {
                cops_visible = true;
            }
            ++i;
        }
        Speech::Manager::ClearPlayback();
        if (!cops_visible) {
            MiscSpeech::LostSuspect(static_cast<int>(Speech_Manager_mLastSpeakerID));
        }
        mDispatch->BreakAway();
        mFocus = kLost;
        mQuadrantState = kInitial;
        mFlags &= ~DISP911_ACTIVE;
        return;
    }

    if (mMusicFlow) {
        Speech::SpeechFlow_ChangeStateTo(mMusicFlow, kTerminal);
    }

    bool is_DDay = false;
    GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
    if (parms) {
        is_DDay = parms->GetIsDDayRace();
    }

    if (!is_DDay) {
        Speech::Module *cop_speech = Speech::Manager::GetSpeechModule(1);
        if (cop_speech) {
            cop_speech->ReleaseResource();
        }
        Speech::Manager::ClearPlayback();
    }

    EAXCop *bailer = GetRandomActiveCop(0, false);
    if (bailer) {
        MiscSpeech::Bailout(bailer->GetSpeakerID());
    } else {
        MiscSpeech::Bailout(0);
    }
    mFocus = kTerminal;
}

void SoundAI::RemoveCop(HSIMABLE seeya) {
    if (mActors.size() == 0) {
        return;
    }

    EAXCop *cop = mActors.Remove(seeya);
    if (!cop) {
        return;
    }

    Speech::copList::iterator i = mCopsInFormation.begin();
    while (i != mCopsInFormation.end()) {
        EAXCop *copInFormation = *i;
        if (copInFormation && copInFormation->GetHandle() == cop->GetHandle()) {
            mCopsInFormation.erase(i);
            break;
        }
        ++i;
    }

    if (mLastCopInFormation && (mLastCopInFormation->GetHandle() == cop->GetHandle())) {
        mLastCopInFormation = 0;
    }
    if (mLeader == cop) {
        mLeader = 0;
    }
    if (mHeli == cop) {
        mHeli = 0;
    }
    if (mLatestCop == cop) {
        mLatestCop = 0;
    }

    Speech::PursuitFlow_OnCopRemoved(mPursuitFlow, cop);
    mUsage.voices.push_back(cop->GetSpeakerID());
    cop->SetActive(false);
}

void SoundAI::AddNewCop(IVehicle *newcop) {
    HSIMABLE newbie = newcop->GetSimable()->GetOwnerHandle();
    IVehicleAI *vai = newcop->GetAIVehiclePtr();
    int roadID = -1;
    bool is_rb_cop = false;

    if (vai) {
        WRoadNav *nav = vai->GetDriveToNav();
        if (nav) {
            roadID = nav->GetRoadSpeechId();
        }
    }

    bool is_cross = (newcop->GetVehicleKey() == static_cast<unsigned int>(-0x2c837f93));
    IRoadBlock *block = GetRoadblock();
    if (block) {
        is_rb_cop = (block->IsComprisedOf(newbie) == newcop);
    }

    int bID = GetBattalionFromKey(newcop->GetVehicleKey());
    if (bID < 1) {
        if (roadID == 0x6b) {
            return;
        }
        bID = GetBattalionFromRoadID(roadID);
        if (bID < 1) {
            bID = 1 << (bRandom(4) & 0x3F);
        }
    }

    EAXCop *latest_cop = 0;
    if (mUsage.voices.empty() || !is_cross) {
        UMath::Vector3 cop_pos = newcop->GetPosition();
        UMath::Vector3 delta;
        VU0_v3sub(mPlayerPos, cop_pos, delta);
        float distance = VU0_sqrt(VU0_v3lengthsquare(delta));

        Speech::copMap::iterator i = mActors.begin();
        while (i != mActors.end()) {
            EAXCop *cop = i->cop;
            if (!cop->IsActive() && !is_rb_cop && (distance < cop->GetDistance()) && !cop->IsHeli()) {
                if (cop->GetCallsign() != bID) {
                    int keyedID = GetBattalionFromKey(newcop->GetVehicleKey());
                    if (keyedID > 0) {
                        bID = keyedID;
                    } else if (roadID != 0x6b) {
                        keyedID = GetBattalionFromRoadID(roadID);
                        if (keyedID > 0) {
                            bID = keyedID;
                        } else {
                            bID = 1 << (bRandom(4) & 0x3F);
                        }
                    } else {
                        return;
                    }
                }

                mActors.ModifyHandle(cop->GetHandle(), newbie);
                cop->SetHandle(newbie);
                cop->SetCallsign(bID);
                cop->SetUnitNumber(GetCallsign(static_cast<Csis::Type_speaker_battalion>(bID)));
                cop->Reset();
                latest_cop = cop;
                break;
            }
            ++i;
        }
    }

    if (!latest_cop) {
        if (!mUsage.voices.empty() || !is_cross) {
            int type = 0;
            if (is_cross) {
                type = 3;
            } else if (is_rb_cop) {
                type = 2;
            }

            int voice = GetVoice(type);
            if (voice > 0) {
                int cID = GetCallsign(static_cast<Csis::Type_speaker_battalion>(bID));
                latest_cop = new EAXCop(voice, newbie, bID, cID);
                latest_cop->SetRank(mActors.size());
                mActors.Add(newbie, latest_cop);
                latest_cop->Reset();
            }
        }
        if (!latest_cop) {
            return;
        }
    }

    mLatestCop = latest_cop;
    if ((mPursuitState < kInactive) && !is_rb_cop && (mFocus == kStrategyFlow)) {
        float random = bRandom(1.0f);
        if (random <= 0.5f) {
            mLatestCop->PrimaryEngage();
        } else {
            mLatestCop->BackupArrives();
        }
    }
}

void SoundAI::ShuffleActors() {
    if (mActors.empty()) {
        return;
    }

    Speech::copList active;
    Speech::copList inactive;
    active.reserve(mActors.size());
    inactive.reserve(mActors.size());

    Speech::copMap::iterator iter = mActors.begin();
    while (iter != mActors.end()) {
        EAXCop *cop = iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        } else {
            inactive.push_back(cop);
        }
        ++iter;
    }

    Speech::copList::iterator i = inactive.begin();
    while (i != inactive.end()) {
        EAXCop *inact = *i;
        Speech::copList::iterator j = active.begin();
        while (j != active.end()) {
            EAXCop *act = *j;
            if (inact->GetSpeakerID() == act->GetSpeakerID()) {
                RemoveCop(inact->GetHandle());
                break;
            }
            ++j;
        }
        ++i;
    }

    bool changed = true;
    while (changed) {
        changed = false;
        active.clear();
        Speech::copMap::iterator it = mActors.begin();
        while (it != mActors.end()) {
            EAXCop *cop = it->cop;
            if (cop->IsActive()) {
                active.push_back(cop);
            }
            ++it;
        }

        Speech::copList::iterator a = active.begin();
        while (a != active.end()) {
            Speech::copList::iterator b = a;
            ++b;
            while (b != active.end()) {
                if ((*a)->GetSpeakerID() == (*b)->GetSpeakerID() && ((*a)->GetHandle() != (*b)->GetHandle())) {
                    if ((*b)->GetDistance() <= (*a)->GetDistance()) {
                        RemoveCop((*a)->GetHandle());
                    } else {
                        RemoveCop((*b)->GetHandle());
                    }
                    changed = true;
                    break;
                }
                ++b;
            }
            if (changed) {
                break;
            }
            ++a;
        }
    }

    active.clear();
    Speech::copMap::iterator final_iter = mActors.begin();
    while (final_iter != mActors.end()) {
        EAXCop *cop = final_iter->cop;
        if (cop->IsActive()) {
            active.push_back(cop);
        }
        ++final_iter;
    }

    if (!active.empty()) {
        if (!mLeader || !mLeader->IsActive() || mLeader->IsHeli()) {
            Speech::copList::iterator c = active.begin();
            while (c != active.end()) {
                EAXCop *cop = *c;
                if (cop->IsPrimary() && cop->HasLOS()) {
                    mLeader = cop;
                    if ((mFocus == kStrategyFlow) && !mLeader->HasLOS()) {
                        mLeader->SpotterWanted();
                    }
                    break;
                }
                ++c;
            }
        }
        if (mLeader && !mLeader->IsPrimary()) {
            MakeLeader(mLeader);
        }
    }
}
