#ifndef EAXSOUND_STREAM_EAXS_STREAMMANAGER_H
#define EAXSOUND_STREAM_EAXS_STREAMMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"

enum eGAMEMODE {
    SNDGM_FREEROAM = 0,
    SNDGM_RACE = 1,
    SNDGM_FRONTEND = 2,
    SNDGM_SPLITSCREEN = 3,
};

struct EAXS_StreamManager : public AudioMemBase {
    EAXS_StreamManager();
    virtual ~EAXS_StreamManager();
    void InitializeStreams(eGAMEMODE mode);
    EAXS_StreamChannel *GetStreamChannel(int nchannel);
    void AddStreamChannel(EAXS_StreamChannel *pstrmchannel, eSTRMTYPE strmtype);
    void RemoveStreamChannel(eSTRMTYPE strmtype);

    int m_nTickStartZoneChange;         // offset 0x04, size 0x4
    int m_nNumStreamsAdded;             // offset 0x08, size 0x4
    int m_nWDRCount;                   // offset 0x0C, size 0x4
    bool m_bIsWDRStreaming;            // offset 0x10, size 0x1
    bool m_bWasWDRStreaming;           // offset 0x14, size 0x1

  private:
    EAXS_StreamChannel *m_pStrmCh[4]; // offset 0x18, size 0x10
};

bool IsWorldDataStreaming(unsigned int strmhandle);

#endif
