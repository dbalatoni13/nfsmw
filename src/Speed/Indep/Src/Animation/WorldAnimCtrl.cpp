#include "WorldAnimCtrl.hpp"
#include "WorldAnimInstanceDirectory.hpp"
#include "Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.h"

static int NumWorldAnimCtrls;
static int MaxNumWorldAnimCtrls;

struct WorldAnimInstanceDirectoryLayout {
    SlotPool *WorldAnimEntitySlotPool;
    SlotPool *WorldAnimEntityTreeSlotPool;
    SlotPool *WorldAnimEntityTreeInfoSlotPool;
    SlotPool *WorldAnimCtrlSlotPool;
};

void *CWorldAnimCtrl::operator new(size_t size, const char *debug_name) {
    NumWorldAnimCtrls++;
    if (MaxNumWorldAnimCtrls < NumWorldAnimCtrls) {
        MaxNumWorldAnimCtrls = NumWorldAnimCtrls;
    }
    return bOMalloc(reinterpret_cast< WorldAnimInstanceDirectoryLayout * >(&TheWorldAnimInstanceDirectory)->WorldAnimCtrlSlotPool);
}

void CWorldAnimCtrl::operator delete(void *ptr) {
    NumWorldAnimCtrls--;
    bFree(reinterpret_cast< WorldAnimInstanceDirectoryLayout * >(&TheWorldAnimInstanceDirectory)->WorldAnimCtrlSlotPool, ptr);
}

CWorldAnimCtrl::CWorldAnimCtrl() {
    Clear();
}

CWorldAnimCtrl::~CWorldAnimCtrl() {
    Purge();
}

void CWorldAnimCtrl::Purge() {
    Cleanup();
    Clear();
}

void CWorldAnimCtrl::Play() {
    PlayState = eACPS_PLAYING;
}

void CWorldAnimCtrl::Pause() {
    PlayState = eACPS_PAUSED;
}

void CWorldAnimCtrl::Stop() {
    m_evalTime = 0.0f;
    PlayState = eACPS_STOPPED;
}

void CWorldAnimCtrl::SetLoopRange(unsigned int loop_range_start, unsigned int loop_range_end) {
    m_loop_range_end = loop_range_end;
    m_loop_range_start = loop_range_start;
    m_flags |= 0x40;
}

float CWorldAnimCtrl::GetLoopRangeScaledStart() {
    return static_cast< float >(m_loop_range_start) * m_timeScale * m_f_speed_modifier;
}

float CWorldAnimCtrl::GetLoopRangeScaledEnd() {
    return static_cast< float >(m_loop_range_end) * m_timeScale * m_f_speed_modifier;
}

void CWorldAnimCtrl::ApplySpeedModifier(float speed_modifier) {
    m_f_speed_modifier = speed_modifier;
}

void CWorldAnimCtrl::JumpToEndForTrigger() {
    PlayState = eACPS_PAUSED;
    PlayDirection = eACPD_REV;
    m_evalTime = m_animLength;
}

void CWorldAnimCtrl::JumpToBeginForTrigger() {
    PlayDirection = eACPD_FWD;
    PlayState = eACPS_PAUSED;
    m_evalTime = 0.0f;
}

void CWorldAnimCtrl::Clear() {
    for (int i = 3; i > -1; i--) {
        m_pFnAnim[i] = nullptr;
    }
    m_flags = 0;
    m_f_speed_modifier = 1.0f;
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
    MasterDelayElapsed = 0.0f;
    PlayDirection = eACPD_FWD;
    PlayState = eACPS_STOPPED;
    StartType = eACST_IMMEDIATE;
}

void CWorldAnimCtrl::Cleanup() {
    EAGL4Anim::FnAnim **fn_anim = m_pFnAnim;
    int i = 3;

    m_animPart.Purge();
    do {
        i--;
        if (*fn_anim != nullptr) {
            EAGL4Anim::MemoryPoolManager::DeleteFnAnim(*fn_anim);
            *fn_anim = nullptr;
        }
        fn_anim++;
    } while (i > -1);
}
