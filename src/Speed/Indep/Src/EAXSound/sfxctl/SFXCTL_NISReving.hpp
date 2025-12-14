#ifndef EAXSOUND_SFXCTL_SFXCTL_NISREVING_H
#define EAXSOUND_SFXCTL_SFXCTL_NISREVING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"

// total size: 0xC
struct EngRevDataPoint {
    float time; // offset 0x0, size 0x4
    int RPM;    // offset 0x4, size 0x4
    int Trq;    // offset 0x8, size 0x4
};

// total size: 0x8
struct EngRevDataSet {
    int NumPoints;               // offset 0x0, size 0x4
    EngRevDataPoint *DataPoints; // offset 0x4, size 0x4
};

// total size: 0x98
class NIS_RevManager : public AudioMemBase {
  public:
    void OpenNISRevData(unsigned int anim_id);

    int *pRevData;                     // offset 0x4, size 0x4
    EngRevDataSet m_EngineDataSet[16]; // offset 0x8, size 0x80
    bool IsInitialized;                // offset 0x88, size 0x1
    EngRevDataPoint *pBuffer;          // offset 0x8C, size 0x4
    EngRevDataPoint *pCurEntry;        // offset 0x90, size 0x4
    int RecordingCount;                // offset 0x94, size 0x4
};

extern NIS_RevManager *g_pNISRevMgr;

#endif
