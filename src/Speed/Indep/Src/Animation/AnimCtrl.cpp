#include "AnimCtrl.hpp"
#include "AnimBank.hpp"
#include "Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

SlotPool *AnimCtrlSlotPool = nullptr;

CAnimCtrl::CAnimCtrl() {
    Clear();
}

CAnimCtrl::~CAnimCtrl() {
    Purge();
}

void InitAnimCtrls() {
#ifdef EA_BUILD_A124
    AnimCtrlSlotPool = bNewSlotPool(128, 64, "AnimCtrlSlotPool", 0);
#else
    AnimCtrlSlotPool = bNewSlotPool(128, 64, "AnimCtrlSlotPool", GetVirtualMemoryAllocParams());
#endif
}

#ifdef MILESTONE_OPT
static int NumAnimCtrls = 0;
static int MaxNumAnimCtrls = 0;
#endif

void *CAnimCtrl::operator new(size_t size, const char *debug_name) {
#ifdef MILESTONE_OPT
    NumAnimCtrls++;
    if (MaxNumAnimCtrls < NumAnimCtrls) {
        MaxNumAnimCtrls = NumAnimCtrls;
    }
#endif
    return bOMalloc(AnimCtrlSlotPool);
}

void CAnimCtrl::operator delete(void *ptr) {
#ifdef MILESTONE_OPT
    NumAnimCtrls--;
#endif
    bFree(AnimCtrlSlotPool, ptr);
}

// STRIPPED
void CAnimCtrl::Init() {}

void CAnimCtrl::Purge() {
    Cleanup();
    Clear();
}

void CAnimCtrl::Clear() {
    for (int i = 0; i < 4; i++) {
        m_pFnAnim[i] = nullptr;
    }
    m_flags = 0;
    m_loop_range_start = 0;
    m_loop_range_end = 0;
    m_f_loop_start = 0.0f;
    m_f_loop_end = 0.0f;
    m_fframe = 0.0f;
    m_startTime = 0;
    m_animLength = 0.0f;
    m_evalTimeStamp = 0.0f;
    m_eFrameCounterTimeStamp = 0;
    m_evalTime = 0.0f;
    m_masterDelayTime = 0.0f;
    m_localDelayTime = 0.0f;
    m_isAllocated = 0;
    LocalDelayElapsed = 0.0f;
    PlayDirection = eACPD_FWD;
    PlayState = eACPS_STOPPED;
    m_timeScale = 1.0f;
    MasterDelayElapsed = 0.0f;
    StartType = eACST_IMMEDIATE;
}

void CAnimCtrl::Cleanup() {
    m_animPart.Purge();
    for (int i = 0; i < 4; i++) {
        if (m_pFnAnim[i]) {
            EAGL4Anim::AnimBank::DeleteFnAnim(m_pFnAnim[i]);
            m_pFnAnim[i] = nullptr;
        }
    }
}

void CAnimCtrl::SetLoopRange(uint32 loop_range_start, uint32 loop_range_end) {
    m_flags |= 0x40;
    m_loop_range_start = loop_range_start;
    m_f_loop_start = m_loop_range_start * m_timeScale;
    m_loop_range_end = loop_range_end;
    m_f_loop_end = m_loop_range_end * m_timeScale;
}

int CAnimCtrl::CreateFnAnimFromBank(EAGL4Anim::AnimBank *animBank, int animIndex, int dof) {
    m_pFnAnim[dof] = animBank->NewFnAnim(animIndex);
    m_pFnAnim[dof]->GetLength(m_animLength);

    if (m_pFnAnim[dof]) {
        SetAllocated();
        return true;
    } else {
        return false;
    }
}

int CAnimCtrl::CreateFnAnimFromNamehash(uint32 namehash, int dof) {
    EAGL4Anim::AnimBank *animBank = nullptr;
    int item_index = 0;
    if (GetAnimFromBankByNamehash(namehash, &animBank, &item_index)) {
        int res = CreateFnAnimFromBank(animBank, item_index, dof);
        SetAllocated();
        return true;
    } else {
        return false;
    }
}

// STRIPPED
void CAnimCtrl::SetPlay() {}

// STRIPPED
void CAnimCtrl::AdvanceAnim() {}

// STRIPPED
void CAnimCtrl::GetFlagString(uint32 flag, char *buffer, int size) {}

// UNSOLVED
int CAnimCtrl::AdvanceAnimTime(float timestep) {
    int result_anim_is_done = false; // r30

    float this_time_step = timestep * 30.0f * m_timeScale;
    float this_master_delay_elapsed = MasterDelayElapsed * 30.0f * m_timeScale;
    float this_master_delay_len = m_masterDelayTime * 30.0f;
    float this_local_delay_elapsed = LocalDelayElapsed * 30.0f * m_timeScale;
    float this_local_delay_len = m_localDelayTime * 30.0f;

    float range_len = m_flags & 0x40 ? m_f_loop_end - m_f_loop_start : m_animLength; // f13
    float begin_of_anim = m_flags & 0x40 ? m_f_loop_start : 0.0f;                    // f9
    float end_of_anim = m_flags & 0x40 ? m_f_loop_end : m_animLength;                // f10

    bool linear = m_flags & 8;      // r10
    bool loop = m_flags & 0x20;     // r7
    bool pingpong = m_flags & 0x10; // r9
    if (linear) {
        if (pingpong) {
            ClearFlags(0x10);
            pingpong = false;
        }
    } else {
        if (!pingpong) {
            linear = true;
            SetFlags(8);
        }
    }
    bool delay_world_start = m_flags & 0x80 && this_master_delay_elapsed < this_master_delay_len && m_evalTime < end_of_anim; // r8
    bool delay_loop_start = m_flags & 0x100 && this_local_delay_elapsed < this_local_delay_len && m_evalTime < end_of_anim;   // r0
    float new_evaltime = m_evalTime;                                                                                          // f31

    if (delay_world_start) {
        float new_timestep = this_master_delay_elapsed + this_time_step;
        if (new_timestep > this_master_delay_len) {
            new_evaltime += bFMod(new_timestep, this_master_delay_len);
        }
        MasterDelayElapsed = (new_timestep / m_timeScale) / 3;
    } else if (delay_loop_start) {
        float new_timestep = this_local_delay_elapsed + this_time_step;
        if (new_timestep > this_local_delay_len) {
            new_evaltime += bFMod(new_timestep, this_local_delay_len);
        }
        LocalDelayElapsed = (new_timestep / m_timeScale) / 3;
    } else if (linear) {
        new_evaltime += this_time_step;
        if (new_evaltime > end_of_anim) {
            if (loop) {
                new_evaltime -= range_len;
                LocalDelayElapsed = 0.0f;
            } else {
                new_evaltime = end_of_anim;
                result_anim_is_done = true;
            }
        }
    } else if (pingpong) {
        switch (PlayDirection) {
            case eACPD_ERROR:
                break;
            case eACPD_FWD:
                new_evaltime += this_time_step;
                if (new_evaltime > end_of_anim) {
                    new_evaltime = end_of_anim - (new_evaltime - end_of_anim);
                    PlayDirection = eACPD_REV;
                }
                break;
            case eACPD_REV:
                new_evaltime -= this_time_step;
                if (new_evaltime < begin_of_anim) {
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

int Anim_Apply_Trans = 1;  // size: 0x4, address: 0x80415688
int Anim_Apply_Rots = 1;   // size: 0x4, address: 0x8041568C
int Anim_Apply_Scales = 1; // size: 0x4, address: 0x80415690

int CAnimCtrl::UpdateAnimPose(bool force_calc) {
    if (!force_calc && m_eFrameCounterTimeStamp == eFrameCounter) {
        return 1;
    }

    m_eFrameCounterTimeStamp = eFrameCounter;
    m_evalTimeStamp = m_evalTime;

    EAGL4Anim::Skeleton *world_skel = m_animPart.GetSkeleton()->GetEAGLSkeleton();
    float *sqtBuffer = m_animPart.GetSQTptr();
    EAGL4::Transform *skinningMatrices = m_animPart.GetGlobalMatrices();
    CAnimCtrl *anim_ctrl; // ?
    float eval_time = GetEvalTime();

    if (Anim_Apply_Rots && GetFnAnim(1)) {
        GetFnAnim(1)->EvalSQT(eval_time, sqtBuffer, nullptr);
    }
    if (Anim_Apply_Scales && GetFnAnim(2)) {
        GetFnAnim(2)->EvalSQT(eval_time, sqtBuffer, nullptr);
    }
    if (Anim_Apply_Trans && GetFnAnim(0)) {
        GetFnAnim(0)->EvalSQT(eval_time, sqtBuffer, nullptr);
    }

    world_skel->PoseSQTToGlobal(sqtBuffer, skinningMatrices, nullptr);

    if (m_flags & 1) {
        bMatrix4 *blended_matrices = reinterpret_cast<bMatrix4 *>(m_animPart.GetGlobalMatrices());
        EAGL4Anim::Skeleton *pSkeleton = m_animPart.GetSkeleton()->GetEAGLSkeleton();
        int number_of_bones = pSkeleton->GetNumBones();

        for (int bone_index = 0; bone_index < number_of_bones; bone_index++) {
            EAGL4Anim::BoneData *bone_data = &pSkeleton->GetBoneData(bone_index);
            bMulMatrix(&blended_matrices[bone_index], &blended_matrices[bone_index], reinterpret_cast<bMatrix4 *>(&bone_data->mInvBaseMatrix));
        }
    }
    return 0;
}
