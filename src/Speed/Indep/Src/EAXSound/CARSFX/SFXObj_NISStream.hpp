#ifndef EAXSOUND_CARSFX_SFXOBJ_NISSTREAM_H
#define EAXSOUND_CARSFX_SFXOBJ_NISSTREAM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"

struct SFXObj_NISStream : CARSFX {
    void StartNIS();
    void QueueNISStream(unsigned int anim_id, int camera_track_number, void (*setmstimecb)(unsigned int, int), bool bButtonThrough);
    void QueueNISStream(unsigned int anim_id, int camera_track_number, bool bButtonThrough, bool param4);
    void NISActivityDone();
    void StopStream();
    bool IsNISStreamReady() { return m_bNISAudioStreamReady; }

    static bool m_bNISAudioStreamReady;
};

#endif
