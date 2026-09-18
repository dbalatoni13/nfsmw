#ifndef EAXSOUND_DYNAMIC_MIXER_NFSLIVELINK_H
#define EAXSOUND_DYNAMIC_MIXER_NFSLIVELINK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x98
struct NFSLiveLink {
    NFSLiveLink();
    ~NFSLiveLink();

    void ProcessLiveLink(float dt);
    void CheckInputBuffer();
    void ReadMasterVolumes();
    void SetupChannelMonitoring();
    void UpdateChannelMonitoring();
    void ReadVolPresets();
    void ReceiveData();
    void ClearInputBuffer();
    void ClearOutputBuffer();

    int *CurOutputPtr;    // offset 0x0, size 0x4
    bool bMonitorChannel; // offset 0x4, size 0x1
    int ChunkSize;        // offset 0x8, size 0x4
    bool bFirstRun;       // offset 0xC, size 0x1
    int InstID;           // offset 0x10, size 0x4
    int ChannelID;        // offset 0x14, size 0x4
    int InputID[32];      // offset 0x18, size 0x80
};

extern NFSLiveLink *g_NFSLiveLink;

#endif
