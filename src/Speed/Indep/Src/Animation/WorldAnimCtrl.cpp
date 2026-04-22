#include "WorldAnimCtrl.hpp"
#include "AnimBank.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

static int NumWorldAnimCtrls = 0;
static int MaxNumWorldAnimCtrls = 0;

struct WorldAnimInstanceDirectoryLayout {
    SlotPool *WorldAnimEntitySlotPool;
    SlotPool *WorldAnimEntityTreeSlotPool;
    SlotPool *WorldAnimEntityTreeInfoSlotPool;
    SlotPool *WorldAnimCtrlSlotPool;
};

// STRIPPED
int GetMaxNumWorldAnimCtrls() {
    return MaxNumWorldAnimCtrls;
}

void *CWorldAnimCtrl::operator new(size_t size, const char *debug_name) {
    NumWorldAnimCtrls++;
    if (NumWorldAnimCtrls > MaxNumWorldAnimCtrls) {
        MaxNumWorldAnimCtrls = NumWorldAnimCtrls;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimCtrlSlotPool());
}

void CWorldAnimCtrl::operator delete(void *ptr) {
    NumWorldAnimCtrls--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimCtrlSlotPool(), ptr);
}

CWorldAnimCtrl::CWorldAnimCtrl() {
    Clear();
}

CWorldAnimCtrl::~CWorldAnimCtrl() {
    Purge();
}

// STRIPPED
void CWorldAnimCtrl::Init() {}

void CWorldAnimCtrl::Purge() {
    Cleanup();
    Clear();
}

void CWorldAnimCtrl::Clear() {
    for (int i = 0; i < 4; i++) {
        m_pFnAnim[i] = nullptr;
    }
    m_flags = 0;
    m_loop_range_start = 0;
    m_loop_range_end = 0;
    m_fframe = 0.0f;
    m_startTime = 0;
    m_timeScale = 1.0f;
    m_animLength = 0.0f;
    m_evalTime = 0.0f;
    m_masterDelayTime = 0.0f;
    m_localDelayTime = 0.0f;
    m_isAllocated = 0;
    LocalDelayElapsed = 0.0f;
    m_f_speed_modifier = 1.0f;
    PlayDirection = eACPD_FWD;
    PlayState = eACPS_STOPPED;
    StartType = eACST_IMMEDIATE;
    MasterDelayElapsed = 0.0f;
}

void CWorldAnimCtrl::SetEvalTime(float time) {
    m_evalTime = time;
    PlayDirection = eACPD_FWD;
    LocalDelayElapsed = 0.0f;
    if ((m_flags & 0x10) != 0) {
        float end_of_anim;
        if (m_flags & 0x40) {
            end_of_anim = GetLoopRangeScaledEnd();
        } else {
            end_of_anim = m_animLength;
        }
        if (time > end_of_anim) {
            PlayDirection = eACPD_REV;
            m_evalTime = end_of_anim - (time - end_of_anim);
        }
    }
}

void CWorldAnimCtrl::Cleanup() {
    m_animPart.Purge();
    for (int i = 0; i < 4; i++) {
        if (m_pFnAnim[i]) {
            EAGL4Anim::MemoryPoolManager::DeleteFnAnim(m_pFnAnim[i]);
            m_pFnAnim[i] = nullptr;
        }
    }
}

void CWorldAnimCtrl::SetLoopRange(uint32 loop_range_start, uint32 loop_range_end) {
    m_loop_range_end = loop_range_end;
    m_loop_range_start = loop_range_start;
    m_flags |= 0x40;
}

float CWorldAnimCtrl::GetLoopRangeScaledStart() {
    return m_loop_range_start * GetEffectiveTimeScale();
}

float CWorldAnimCtrl::GetLoopRangeScaledEnd() {
    return m_loop_range_end * GetEffectiveTimeScale();
}

// STRIPPED
int CWorldAnimCtrl::CreateFnAnim(EAGL4Anim::AnimMemoryMap *memMap, int dof) {}

int CWorldAnimCtrl::CreateFnAnimFromBank(EAGL4Anim::AnimBank *animBank, int animIndex, int dof) {
    m_pFnAnim[dof] = animBank->NewFnAnim(animIndex);
    m_pFnAnim[dof]->GetLength(m_animLength);

    if (m_pFnAnim[dof] != nullptr) {
        m_isAllocated = 1;
        return 1;
    }
    return 0;
}

int CWorldAnimCtrl::CreateFnAnimFromNamehash(uint32 namehash, int dof) {
    EAGL4Anim::AnimBank *animBank = nullptr;
    int item_index = 0;
    if (GetAnimFromBankByNamehash(namehash, &animBank, &item_index)) {
        CreateFnAnimFromBank(animBank, item_index, dof);
        m_isAllocated = 1;
        return 1;
    }
    return 0;
}

void CWorldAnimCtrl::Play() {
    PlayState = eACPS_PLAYING;
}

void CWorldAnimCtrl::Stop() {
    m_evalTime = 0.0f;
    PlayState = eACPS_STOPPED;
}

void CWorldAnimCtrl::Pause() {
    PlayState = eACPS_PAUSED;
}

// STRIPPED
void CWorldAnimCtrl::AdvanceAnim() {}

// STRIPPED
void CWorldAnimCtrl::GetFlagString(uint32 flag, char *buffer, int size) {}

void CWorldAnimCtrl::JumpToEndForTrigger() {
    m_evalTime = m_animLength;
    PlayState = eACPS_PAUSED;
    PlayDirection = eACPD_REV;
}

void CWorldAnimCtrl::JumpToBeginForTrigger() {
    PlayState = eACPS_PAUSED;
    m_evalTime = 0.0f;
    PlayDirection = eACPD_FWD;
}

void CWorldAnimCtrl::ApplySpeedModifier(float speed_modifier) {
    m_f_speed_modifier = speed_modifier;
}

// STRIPPED
bool CWorldAnimCtrl::IsCurrentlyDelayingBeforeWorldStart() {}

// STRIPPED
bool CWorldAnimCtrl::IsCurrentlyDelayingBetweenLoops() {}

// UNSOLVED
int CWorldAnimCtrl::AdvanceAnimTime(float timestep) {
    int result_anim_is_done = 0;

    float effective_time_scale = GetEffectiveTimeScale();
    float this_time_step = timestep * 30 * effective_time_scale;
    float this_master_delay_elapsed = MasterDelayElapsed * 30 * effective_time_scale;
    float this_master_delay_len = m_masterDelayTime * 30;
    float this_local_delay_elapsed = LocalDelayElapsed * 30 * effective_time_scale;
    float this_local_delay_len = m_localDelayTime * 30;

    float range_len = m_flags & 0x40 ? GetLoopRangeScaledEnd() - GetLoopRangeScaledStart() : m_animLength;

    float begin_of_anim = m_flags & 0x40 ? GetLoopRangeScaledStart() : 0.0f;

    float end_of_anim = m_flags & 0x40 ? GetLoopRangeScaledEnd() : m_animLength;

    bool linear = m_flags & 8;
    bool triggered = m_flags & 0x800;
    bool loop = m_flags & 0x20 ? !triggered : false;
    bool pingpong = m_flags & 0x10;

    if (linear) {
        if (pingpong) {
            ClearFlags(0x10);
            pingpong = false;
        }
    } else {
        if (!pingpong) {
            SetFlags(8);
            linear = true;
        }
    }

    bool delay_world_start = (m_flags & 0x80) && this_master_delay_elapsed < this_master_delay_len && m_evalTime < end_of_anim;

    bool delay_loop_start = (m_flags & 0x100) && this_local_delay_elapsed < this_local_delay_len && m_evalTime < end_of_anim;

    float new_evaltime = m_evalTime;

    if (PlayState != eACPS_PLAYING) {
        return 0;
    }

    if (delay_world_start) {
        float new_timestep = this_master_delay_elapsed + this_time_step;
        if (new_timestep > this_master_delay_len) {
            new_evaltime += bFMod(new_timestep, this_master_delay_len);
        }
        MasterDelayElapsed = (new_timestep / effective_time_scale) / 30;
    } else if (delay_loop_start) {
        float new_timestep = this_local_delay_elapsed + this_time_step;
        if (new_timestep > this_local_delay_len) {
            new_evaltime += bFMod(new_timestep, this_local_delay_len);
        }
        LocalDelayElapsed = (new_timestep / effective_time_scale) / 30;
    } else if (linear) {
        if (m_flags & 0x1000) {
            new_evaltime -= this_time_step;
            if (new_evaltime < begin_of_anim) {
                if (loop) {
                    new_evaltime = new_evaltime + range_len;
                    LocalDelayElapsed = 0.0f;
                } else {
                    new_evaltime = begin_of_anim;
                    result_anim_is_done = 1;
                }
            }
        } else {
            new_evaltime += this_time_step;
            if (new_evaltime > end_of_anim) {
                if (loop) {
                    new_evaltime = new_evaltime - range_len;
                    LocalDelayElapsed = 0.0f;
                } else {
                    new_evaltime = end_of_anim;
                    result_anim_is_done = 1;
                }
            }
        }
    } else if (pingpong) {
        switch (PlayDirection) {
            case eACPD_ERROR:
                break;
            case eACPD_FWD:
                new_evaltime += this_time_step;
                if (new_evaltime > end_of_anim) {
                    if (triggered) {
                        PlayState = eACPS_PAUSED;
                    }
                    PlayDirection = eACPD_REV;
                    new_evaltime = end_of_anim - (new_evaltime - end_of_anim);
                }
                break;
            case eACPD_REV:
                new_evaltime -= this_time_step;
                if (new_evaltime < begin_of_anim) {
                    if (triggered) {
                        PlayState = eACPS_PAUSED;
                    }
                    PlayDirection = eACPD_FWD;
                    new_evaltime = begin_of_anim;
                    LocalDelayElapsed = 0.0f;
                }
                break;
        }
    }

    m_evalTime = new_evaltime;
    return result_anim_is_done;
}

// STRIPPED
float CWorldAnimCtrl::GetEvalTimeInSeconds() {
    return m_evalTime / 30 / GetEffectiveTimeScale();
}

// STRIPPED
void CWorldAnimCtrl::SetEvalTimeInSeconds(float seconds) {
    m_evalTime = seconds * 30 * GetEffectiveTimeScale();
}

float CWorldAnimCtrl::GetAnimLengthInSeconds() {
    return m_animLength / 30 / GetEffectiveTimeScale();
}

// STRIPPED
void CWorldAnimCtrl::UpdateAnim() {}

int CWorldAnimCtrl::UpdateAnimPose() {
    EAGL4Anim::Skeleton *world_skel = m_animPart.GetSkeleton()->GetEAGLSkeleton();
    float *sqtBuffer = m_animPart.GetSQTptr();
    EAGL4::Transform *skinningMatrices = m_animPart.GetGlobalMatrices();
    float eval_time = GetEvalTime();

    if (GetFnAnim(1)) {
        GetFnAnim(1)->EvalSQT(eval_time, sqtBuffer, nullptr);
    }
    if (GetFnAnim(2)) {
        GetFnAnim(2)->EvalSQT(eval_time, sqtBuffer, nullptr);
    }
    if (GetFnAnim(0)) {
        GetFnAnim(0)->EvalSQT(eval_time, sqtBuffer, nullptr);
    }

    world_skel->PoseSQTToGlobal(sqtBuffer, skinningMatrices, nullptr);

    if (m_flags & 1) {
        EAGL4Anim::Skeleton *pSkeleton = m_animPart.GetSkeleton()->GetEAGLSkeleton();
        int number_of_bones = pSkeleton->GetNumBones();
        bMatrix4 *blended_matrices = reinterpret_cast<bMatrix4 *>(m_animPart.GetGlobalMatrices());

        for (int bone_index = 0; bone_index < number_of_bones; bone_index++) {
            EAGL4Anim::BoneData *bone_data = &pSkeleton->GetBoneData(bone_index);
            bMulMatrix(&blended_matrices[bone_index], &blended_matrices[bone_index], reinterpret_cast<bMatrix4 *>(&bone_data->mInvBaseMatrix));
        }
    }

    return 0;
}

// STRIPPED
void CWorldAnimCtrl::PredictPositionAtTime(float time, bMatrix4 *world_position) {}

// STRIPPED
void CWorldAnimCtrl::DoSnapshot(ReplaySnapshot *snapshot) {}
