#ifndef CAMERA_ICE_ICEMANAGER_H
#define CAMERA_ICE_ICEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEData.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

enum ICEContext {
    eDCE_NIS = 0,
    eDCE_FMV = 1,
    eDCE_REPLAY = 2,
    eDCE_GENERIC = 3,
    eDCE_NUM_CONTEXTS = 4,
    eDCE_NOCONTEXT = 4,
};

struct ICEGroup {
    // total size: 0x14
    void FlushAllocatedTracks();
    struct ICETrack *GetTrack(int n);
    struct ICETrack *GetTrack(char *s);

    ICEGroup();
    ~ICEGroup();

    unsigned int GetHandle() {
        return Handle;
    }

    int GetContext() {
        return Context;
    }

    void SetHandle(unsigned int n) {
        Handle = n;
    }

    void SetContext(int context) {
        Context = context;
    }

    int GetNumTracks() {
        return NumTracks;
    }

    void AddTrack(struct ICETrack *track);

    void FlushTracks();

    unsigned int Handle;               // offset 0x0, size 0x4
    int Context;                       // offset 0x4, size 0x4
    int NumTracks;                     // offset 0x8, size 0x4
    bTList<struct ICETrack> TrackList; // offset 0xC, size 0x8
};

// total size: 0x19F0
struct ICETrack : public bTNode<ICETrack> {
    void PlatEndianSwap();
    int GetContext();
    int GetKeyNumber(float f_param);
    int GetKeyNumber(ICEData *data) {
        return static_cast<int>(data - Keys);
    }
    float GetParameter();
    struct ICEData *GetCameraData(float *p_start, float *p_end, float *p_current);

    int GetNumKeys() {
        return NumKeys;
    }

    ICEData *GetKey(int n) {
        if (n == UMath::Clamp(n, 0, NumKeys - 1)) return &Keys[n];
        return 0;
    }

    char *GetName() {
        return Name;
    }

    ICEGroup *GetGroup() {
        return Group;
    }

    float GetStart() {
        return Start;
    }

    float GetLength() {
        return Length;
    }

    bool IsAllocated() {
        return Allocated != 0;
    }

    void SetGroup(ICEGroup *g) {
        Group = g;
    }

    void SetStart(float s) {
        Start = s;
    }

    void SetLength(float l) {
        Length = l;
    }

    int MemoryImageSize() {
        return static_cast<int>(sizeof(ICETrack)) - (50 - NumKeys) * static_cast<int>(sizeof(ICEData));
    }

    ICEGroup *Group;  // offset 0x8, size 0x4
    float Start;      // offset 0xC, size 0x4
    float Length;     // offset 0x10, size 0x4
    short NumKeys;    // offset 0x14, size 0x2
    char Allocated;   // offset 0x16, size 0x1
    char Name[14];    // offset 0x17, size 0xE
    ICEData Keys[50]; // offset 0x28, size 0x19C8
};

inline ICEGroup::ICEGroup()
    : Handle(0) //
    , Context(eDCE_NOCONTEXT) //
    , NumTracks(0) {}

inline ICEGroup::~ICEGroup() {}

inline void ICEGroup::AddTrack(ICETrack *track) {
    track->SetGroup(this);
    TrackList.AddTail(track);
    NumTracks++;
}

inline void ICEGroup::FlushTracks() {
    while (!TrackList.IsEmpty()) {
        ICETrack *track = TrackList.RemoveHead();
        if (track->IsAllocated() && track != 0) {
            delete track;
        }
    }
}

// total size: 0xC
struct ICEShakeGroup {
    void FlushAllocatedTracks();
    struct ICEShakeTrack *GetTrack(int n);
    void FlushTracks();

    ICEShakeGroup();
    ~ICEShakeGroup();

    void AddTrack(struct ICEShakeTrack *track);

    int NumTracks;                                 // offset 0x0, size 0x4
    struct bTList<struct ICEShakeTrack> TrackList; // offset 0x4, size 0x8
};

// total size: 0x18
struct ICEShakeData {
    void InitData();
    void PlatEndianSwap();

    float q[3]; // offset 0x0, size 0xC
    float p[3]; // offset 0xC, size 0xC
};

// total size: 0xB60
struct ICEShakeTrack : public bTNode<ICEShakeTrack> {
    void PlatEndianSwap();
    bool IsAllocated() { return Allocated != 0; }

    void SetGroup(ICEShakeGroup *g) {
        Group = g;
    }

    int MemoryImageSize() {
        return static_cast<int>(sizeof(ICEShakeTrack)) - (120 - NumKeys) * static_cast<int>(sizeof(ICEShakeData));
    }

    ICEShakeGroup *Group;   // offset 0x8, size 0x4
    short NumKeys;          // offset 0xC, size 0x2
    char Allocated;         // offset 0xE, size 0x1
    char Name[14];          // offset 0xF, size 0xE
    ICEShakeData Keys[120]; // offset 0x20, size 0xB40
};

inline ICEShakeGroup::~ICEShakeGroup() {}

inline ICEShakeGroup::ICEShakeGroup() : NumTracks(0) {}

inline void ICEShakeGroup::AddTrack(ICEShakeTrack *track) {
    track->SetGroup(this);
    TrackList.AddTail(track);
    NumTracks++;
}

// total size: 0x80
class ICEManager {
  public:
    ICEManager();

    void Init();
    void Resolve();
    ICEData *GetCameraData(unsigned int scene_hash, int camTrack);
    ICEShakeTrack *GetShakeTrack(unsigned int shake_type);
    int GetCameraIndex(float f_param, ICETrack *track);
    float GetParameter();
    float GetTimerSeconds();
    bool RefreshCameraSplines();
    void Update();
    int GetNumSceneCameraTrack(unsigned int scene_hash);
    void ChooseReplayCamera();

    bool IsEditorOn() {
        // TODO maybe negated?
        return nState >= 1;
    }

    bool IsEditorOff() {
        return nState <= 0;
    }

    int GetState() {
        return nState;
    }

    ICETrack *GetPlaybackTrack() {
        return pPlaybackTrack;
    }

    void SetSmoothExit(bool smooth) {
        bSmoothExit = smooth;
    }

    bool IsSmoothExit() {
        return bSmoothExit;
    }

    void SetUseRealTime(bool val) {
        bUseRealTime = val;
    }

    float IsUsingRealTime() {
        return bUseRealTime;
    }

    float GetParameterLength() {
        return fParameterLength;
    }

    bool IsGenericCameraPlaying() {
        return nPlayGenericGroupHash != bStringHash("");
    }

    float GetAnimElevationFixup(ICE::Vector3 *position);
    void SetupAnimElevation();
    void FixAnimElevation(ICE::Vector3 *position);

    int ChooseGoodSceneCameraTrackIndex(unsigned int scene_hash, const ICE::Matrix4 *scene_origin);
    void SetGenericCameraToPlay(const char *group_name, const char *track_name);
    ICEGroup *GetCurrentGroup();
    unsigned int GetRelativeShakeType(unsigned int shake_type, int inc);
    char *GetShakeTypeName(unsigned int shake_type);
    ICETrack *GetCurrentTrack();
    bool ChooseCameraPlaybackTrack();
    ICEData *GetCameraData(ICETrack **p_track, float *p_start, float *p_end);
    ICEData *GetNeighbour(ICEData *data, int key, ICETrack *track);
    void GetSlope(ICE::Vector3 *eye, ICE::Vector3 *look, float *fov, float *dutch, ICEData *data, int key, ICETrack *track);
    ICEGroup *AddCameraGroup(ICEContext context, unsigned int handle);
    ICEGroup *GetCameraGroup(ICEContext context, unsigned int handle);
    void LoadCameraSet(bChunk *chunk);
    void UnloadCameraSet(bChunk *chunk);
    void LoadCameraShakes(bChunk *chunk);
    void UnloadCameraShakes(bChunk *chunk);

  private:
    float GetParameter(int i, ICETrack *track);
    float GetIntervalSize(ICEData *data, ICETrack *track);
    ICETrack *ChooseGenericCamera();
    ICEGroup *GetNisCameraGroup(unsigned int scene_hash);
    ICEGroup *GetFmvCameraGroup(unsigned int scene_hash);
    ICEGroup *GetReplayCameraGroup(unsigned int category_hash);
    ICEGroup *GetGenericCameraGroup(unsigned int name_hash);

    ICEGroup *pNisCameras;              // offset 0x0, size 0x4
    ICEGroup *pFmvCameras;              // offset 0x4, size 0x4
    ICEGroup *pReplayCameras;           // offset 0x8, size 0x4
    ICEGroup *pGenericCameras;          // offset 0xC, size 0x4
    ICEShakeGroup *pShakeGroup;         // offset 0x10, size 0x4
    int nNisCameras;                    // offset 0x14, size 0x4
    int nFmvCameras;                    // offset 0x18, size 0x4
    int nReplayCameras;                 // offset 0x1C, size 0x4
    int nGenericCameras;                // offset 0x20, size 0x4
    ICETrack *pPlaybackTrack;           // offset 0x24, size 0x4
    int nState;                         // offset 0x28, size 0x4
    int nTrack;                         // offset 0x2C, size 0x4
    int nHandle;                        // offset 0x30, size 0x4
    int nOption;                        // offset 0x34, size 0x4
    int nSetting;                       // offset 0x38, size 0x4
    int nExitConfirmOption;             // offset 0x3C, size 0x4
    int nDeleteConfirmOption;           // offset 0x40, size 0x4
    int nContext;                       // offset 0x44, size 0x4
    int nCopyMode;                      // offset 0x48, size 0x4
    unsigned int nSceneHash;            // offset 0x4C, size 0x4
    float fAnimElevation;               // offset 0x50, size 0x4
    float fParameterStart;              // offset 0x54, size 0x4
    float fParameterLength;             // offset 0x58, size 0x4
    float fParameterLengthBackup;       // offset 0x5C, size 0x4
    unsigned int nPlayGenericGroupHash; // offset 0x60, size 0x4
    char nPlayGenericTrackName[14];     // offset 0x64, size 0xE
    bool bSmoothExit;                   // offset 0x74, size 0x1
    int nMarkerIndex;                   // offset 0x78, size 0x4
    bool bUseRealTime;                  // offset 0x7C, size 0x1
};

extern ICEManager TheICEManager;

void ICECompleteEventTags();

#endif
