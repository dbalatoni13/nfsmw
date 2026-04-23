#ifndef CAMERA_ICE_ICEMANAGER_H
#define CAMERA_ICE_ICEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEData.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

class ICEGroup {
  private:
    // total size: 0x14
    uint32 Handle;                     // offset 0x0, size 0x4
    int Context;                       // offset 0x4, size 0x4
    int NumTracks;                     // offset 0x8, size 0x4
    bTList<struct ICETrack> TrackList; // offset 0xC, size 0x8
};

// total size: 0x19F0
class ICETrack : public bTNode<ICETrack> {
  private:
    ICEGroup *Group;  // offset 0x8, size 0x4
    float Start;      // offset 0xC, size 0x4
    float Length;     // offset 0x10, size 0x4
    int16 NumKeys;    // offset 0x14, size 0x2
    int8 Allocated;   // offset 0x16, size 0x1
    char Name[14];    // offset 0x17, size 0xE
    ICEData Keys[50]; // offset 0x28, size 0x19C8
};

// total size: 0xC
class ICEShakeGroup {
  private:
    int NumTracks;                                 // offset 0x0, size 0x4
    struct bTList<struct ICEShakeTrack> TrackList; // offset 0x4, size 0x8
};

// total size: 0x18
struct ICEShakeData {
    float q[3]; // offset 0x0, size 0xC
    float p[3]; // offset 0xC, size 0xC
};

// total size: 0xB60
class ICEShakeTrack : public bTNode<ICEShakeTrack> {
  public:
    ICEShakeGroup *Group;   // offset 0x8, size 0x4
    int16 NumKeys;          // offset 0xC, size 0x2
    int8 Allocated;         // offset 0xE, size 0x1
    char Name[14];          // offset 0xF, size 0xE
    ICEShakeData Keys[120]; // offset 0x20, size 0xB40
};

// total size: 0x80
class ICEManager {
  public:
    void Init();
    void Resolve();
    ICEData *GetCameraData(uint32 scene_hash, int camTrack);
    int GetNumSceneCameraTrack(uint32 scene_hash);
    void SetGenericCameraToPlay(const char *group_name, const char *track_name);

    bool IsEditorOn() {
        // TODO maybe negated?
        return nState >= 1;
    }

  private:
    ICEGroup *pNisCameras;          // offset 0x0, size 0x4
    ICEGroup *pFmvCameras;          // offset 0x4, size 0x4
    ICEGroup *pReplayCameras;       // offset 0x8, size 0x4
    ICEGroup *pGenericCameras;      // offset 0xC, size 0x4
    ICEShakeGroup *pShakeGroup;     // offset 0x10, size 0x4
    int nNisCameras;                // offset 0x14, size 0x4
    int nFmvCameras;                // offset 0x18, size 0x4
    int nReplayCameras;             // offset 0x1C, size 0x4
    int nGenericCameras;            // offset 0x20, size 0x4
    ICETrack *pPlaybackTrack;       // offset 0x24, size 0x4
    int nState;                     // offset 0x28, size 0x4
    int nTrack;                     // offset 0x2C, size 0x4
    int nHandle;                    // offset 0x30, size 0x4
    int nOption;                    // offset 0x34, size 0x4
    int nSetting;                   // offset 0x38, size 0x4
    int nExitConfirmOption;         // offset 0x3C, size 0x4
    int nDeleteConfirmOption;       // offset 0x40, size 0x4
    int nContext;                   // offset 0x44, size 0x4
    int nCopyMode;                  // offset 0x48, size 0x4
    uint32 nSceneHash;              // offset 0x4C, size 0x4
    float fAnimElevation;           // offset 0x50, size 0x4
    float fParameterStart;          // offset 0x54, size 0x4
    float fParameterLength;         // offset 0x58, size 0x4
    float fParameterLengthBackup;   // offset 0x5C, size 0x4
    uint32 nPlayGenericGroupHash;   // offset 0x60, size 0x4
    char nPlayGenericTrackName[14]; // offset 0x64, size 0xE
    bool bSmoothExit;               // offset 0x74, size 0x1
    int nMarkerIndex;               // offset 0x78, size 0x4
    bool bUseRealTime;              // offset 0x7C, size 0x1
};

extern ICEManager TheICEManager;

void ICECompleteEventTags();

#endif
