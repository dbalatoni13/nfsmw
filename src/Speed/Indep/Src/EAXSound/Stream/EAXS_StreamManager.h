//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef STREAMMANAGER_H
#define STREAMMANAGER_H // Decl: 16

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"

// Decl: 46
enum eSTRMTYPE {
    STYPE_NONE = -1,
    STYPE_COPSPEECH = 0,
    STYPE_MUSIC = 1,
    STYPE_NISSFX = 2,
    STYPE_AUX = 3,
    STYPE_MAX = 4,
};

class EAXS_StreamChannel;

// total size: 0x28
// Decl: 76
class EAXS_StreamManager : public AudioMemBase {
  public:
    EAXS_StreamManager();

    // Overrides: AudioMemBase
    ~EAXS_StreamManager() override;

    void InitializeStreams(eGAMEMODE gamemode);

    void ThinkAboutIt();

    void SetupWorldDataTransitionData();

    void UpdateStreams();

    void AddStreamChannel(EAXS_StreamChannel *pstrmchannel, eSTRMTYPE strmtype);

    void RemoveStreamChannel(eSTRMTYPE strmtype);

    void UpdateTrackStreamerTransition(bool bstate);

    EAXS_StreamChannel *GetStreamChannel(int nchannel);

    int m_nTickStartZoneChange; // offset 0x4, size 0x4, Decl: 93
    int m_nNumStreamsAdded;     // offset 0x8, size 0x4, Decl: 94
    int m_nWDRCount;            // offset 0xC, size 0x4, Decl: 95
    bool m_bIsWDRStreaming;     // offset 0x10, size 0x1, Decl: 96
    bool m_bWasWDRStreaming;    // offset 0x14, size 0x1, Decl: 97

  private:
    EAXS_StreamChannel *m_pStrmCh[4]; // offset 0x18, size 0x10, Decl: 100
};

#define MAX_STREAM_CHANNELS 3; // Decl: 57

extern EAXS_StreamManager *gpEAXS_StrmMgr; // Decl: unknown

void AssignAudioStreamHandle(uintptr_t realstrmhandle);
bool IsWorldDataStreaming(uintptr_t strmhandle);

#endif
