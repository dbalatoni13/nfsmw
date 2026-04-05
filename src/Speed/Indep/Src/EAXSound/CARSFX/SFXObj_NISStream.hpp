#ifndef EAXSOUND_CARSFX_SFXOBJ_NISSTREAM_H
#define EAXSOUND_CARSFX_SFXOBJ_NISSTREAM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct SFXObj_NISStream : CARSFX {
    SFXObj_NISStream();
    ~SFXObj_NISStream() override;
    void InitSFX() override;
    void StartNIS();
    bool QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int), bool bButtonThrough);
    bool QueueNISStream(unsigned int anim_id, int camera_track_number, bool bButtonThrough, bool param4);
    void NISActivityDone();
    void StopStream();
    void UpdateParams(float t) override;
    static void PlayNISStream();
    static void PlayNISButtonThroughStream();
    void StartNISButtonThrough();
    void AnimationEnded(bool bunloaded);
    bool IsNISStreamReady() { return m_bNISAudioStreamReady; }
    int GetController(int Index) override { return -1; }
    void AttachController(SFXCTL *psfxctl) override {}
    void Destroy() override {}
    void ProcessUpdate() override {}

    static bool m_bNISButtonThroughAnimationReady;
    static bool m_bNISAudioStreamReady;
    static bool m_bNISButtonThroughReady;
    static bool m_bIsButtonThrough;
    static int m_mstimeelapsed;
    static int m_mslengthofstream;

    bool m_bNISAnimationReady;
    bool m_bBackupStreamCleared;
    bool m_bPlayingButtonThrough;
    unsigned int m_animid;
    void (*m_mselapsedtimecb)(unsigned int, int);
};

#endif
