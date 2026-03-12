#ifndef ANIMATION_ANIMCTRL_H
#define ANIMATION_ANIMCTRL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimPart.hpp"
#include "Speed/Indep/Src/EAGL4Anim/AnimMemoryMap.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4AnimBank.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x80
class CAnimCtrl : public bTNode<CAnimCtrl> {
  public:
    enum eAC_PlaybackDirection {
        eACPD_ERROR = -1,
        eACPD_FWD = 0,
        eACPD_REV = 1,
    };

    enum eAC_StartType {
        eACST_ERROR = -1,
        eACST_IMMEDIATE = 0,
        eACST_TRIGGERED = 1,
    };

    enum eAC_PlayState {
        eACPS_ERROR = -1,
        eACPS_STOPPED = 0,
        eACPS_PLAYING = 1,
        eACPS_PAUSED = 2,
    };

    EAGL4Anim::FnAnim *GetFnAnim(int dof) {
        return m_pFnAnim[dof];
    }

    void JumpToEnd() {}

    CAnimPart *GetAnimPart() {
        return &m_animPart;
    }

    unsigned int GetNameHash() {
        return m_NameHash;
    }

    void SetNameHash(unsigned int name_hash) {
        m_NameHash = name_hash;
    }

    unsigned int GetFlags() {
        return m_flags;
    }

    unsigned int IsCyclic() {}

    unsigned int IsHalted() {}

    int IsPlaying() {
        return PlayState == eACPS_PLAYING;
    }

    int IsStopped() {
        return PlayState == eACPS_STOPPED;
    }

    int IsPaused() {
        return PlayState == eACPS_PAUSED;
    }

    int IsDone() {}

    void SetDone() {}

    int IsMirror() {}

    void SetFlags(int flags) {
        m_flags |= flags;
    }

    void ClearFlags(int flags) {
        m_flags &= ~flags;
    }

    void XorFlags(int flags) {
        m_flags ^= flags;
    }

    void HaltAnim() {}

    int GetCurIntFrame() {}

    float GetCurFloatFrame() {}

    float GetLength() {
        return m_animLength;
    }

    int GetNumFrames() {}

    int GetStartTime() {
        return m_startTime;
    }

    void SetStartTime(int time) {
        m_startTime = time;
    }

    float GetTimeScale() {
        return m_timeScale;
    }

    void SetTimeScale(float time) {
        m_timeScale = time;
    }

    float GetMasterDelayTime() {
        return m_masterDelayTime;
    }

    void SetMasterDelayTime(float time) {
        m_masterDelayTime = time;
    }

    float GetLocalDelayTime() {
        return m_localDelayTime;
    }

    void SetLocalDelayTime(float time) {
        m_localDelayTime = time;
    }

    float GetAnimLength() {
        return m_animLength;
    }

    float GetEvalTime() {
        return m_evalTime;
    }

    void SetEvalTime(float time) {
        m_evalTime = time;
    }

    int GetAllocated() {
        return m_isAllocated;
    }

    void SetAllocated() {
        m_isAllocated = true;
    }

    CAnimCtrl();

    ~CAnimCtrl();

    void *operator new(size_t size, const char *debug_name);

    void operator delete(void *ptr);

    void Init();

    void Purge();

    void Clear();

    void Cleanup();

    void SetLoopRange(unsigned int loop_range_start, unsigned int loop_range_end);

    int CreateFnAnim(EAGL4Anim::AnimMemoryMap *memMap, int dof);

    int CreateFnAnimFromBank(EAGL4Anim::AnimBank *animBank, int animIndex, int dof);

    int CreateFnAnimFromNamehash(unsigned int namehash, int dof);

    void SetPlay();

    void AdvanceAnim();

    void GetFlagString(unsigned int flag, char *buffer, int size);

    int AdvanceAnimTime(float timestep);

    float GetEvalTimeInSeconds();

    void SetEvalTimeInSeconds(float seconds);

    float GetAnimLengthInSeconds();

    void UpdateAnim();

    int UpdateAnimPose(bool force_calc);

    void PredictPositionAtTime(float time, bMatrix4 *world_position);

    eAC_PlaybackDirection PlayDirection; // offset 0x8, size 0x4
    eAC_StartType StartType;             // offset 0xC, size 0x4
    eAC_PlayState PlayState;             // offset 0x10, size 0x4
    float MasterDelayElapsed;            // offset 0x14, size 0x4
    float LocalDelayElapsed;             // offset 0x18, size 0x4

  private:
    uint32 m_NameHash;               // offset 0x1C, size 0x4
    CAnimPart m_animPart;            // offset 0x20, size 0x14
    uint32 m_flags;                  // offset 0x34, size 0x4
    int m_loop_range_start;          // offset 0x38, size 0x4
    int m_loop_range_end;            // offset 0x3C, size 0x4
    float m_f_loop_start;            // offset 0x40, size 0x4
    float m_f_loop_end;              // offset 0x44, size 0x4
    EAGL4Anim::FnAnim *m_pFnAnim[4]; // offset 0x48, size 0x10
    float m_fframe;                  // offset 0x58, size 0x4
    int m_startTime;                 // offset 0x5C, size 0x4
    float m_timeScale;               // offset 0x60, size 0x4
    float m_animLength;              // offset 0x64, size 0x4
    float m_evalTime;                // offset 0x68, size 0x4
    float m_masterDelayTime;         // offset 0x6C, size 0x4
    float m_localDelayTime;          // offset 0x70, size 0x4
    float m_evalTimeStamp;           // offset 0x74, size 0x4
    uint32 m_eFrameCounterTimeStamp; // offset 0x78, size 0x4
    int m_isAllocated;               // offset 0x7C, size 0x4
};

void InitAnimCtrls();
void CloseAnimCtrls();

#endif
