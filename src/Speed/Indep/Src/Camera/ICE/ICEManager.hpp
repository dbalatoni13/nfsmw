#ifndef CAMERA_ICE_ICEMANAGER_H
#define CAMERA_ICE_ICEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEData.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct bChunk;
class ICETrack;

enum ICEContext {
    ICE_CONTEXT_NIS = 0,
    ICE_CONTEXT_FMV = 1,
    ICE_CONTEXT_REPLAY = 2,
    ICE_CONTEXT_GENERIC = 3,
};

// total size: 0x14
class ICEGroup {
  public:
    ICEGroup();

    void SetHandle(unsigned int n) {
        Handle = n;
    }

    void SetContext(int context) {
        Context = context;
    }

    unsigned int GetHandle() { return Handle; }

    int GetNumTracks() { return NumTracks; }

    void FlushTrack(ICETrack *track);
    bTList<ICETrack> *GetTrackList();
    void FlushAllocatedTracks();
    struct ICETrack *GetTrack(int n);
    struct ICETrack *GetTrack(char *name);

    void FlushTracks();

    void AddTrack(struct ICETrack *track);

    uint32 Handle;                     // offset 0x0, size 0x4
    int Context;                       // offset 0x4, size 0x4
    int NumTracks;                     // offset 0x8, size 0x4
    bTList<struct ICETrack> TrackList; // offset 0xC, size 0x8
};

// total size: 0x19F0
class ICETrack : public bTNode<ICETrack> {
  public:
    ICEGroup *GetGroup();

    bool IsAllocated() { return Allocated != 0; }

    void PlatEndianSwap();
    int GetContext();
    float GetParameter();
    int GetKeyNumber(float f_parameter);

    ICEData *GetCameraData(float *pParameter0, float *pParameter1, float *pParameter2);

    ICEData *GetKey(int n) {
        int c = ICE::Clamp(n, 0, NumKeys - 1);
        if (n == c) {
            return &Keys[c];
        }
        return 0;
    }

    int GetKeyIndex(ICEData *pKey) {
        return pKey - Keys;
    }

    float GetKeyParameter(int n) {
        if (n < 0) {
            return 0.0f;
        }
        if (n >= NumKeys) {
            return 1.0f;
        }
        return Keys[n].fParameter;
    }

    int GetNumKeys() { return NumKeys; }

    char *GetName() { return Name; }

    float GetStart() { return Start; }

    float GetLength() { return Length; }

    void SetGroup(ICEGroup *group) { Group = group; }

    int MemoryImageSize() { return sizeof(ICETrack) - (50 - NumKeys) * sizeof(ICEData); }

    void SetStart(float start) { Start = start; }

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
  public:
enum eICEEditState {
    eDCE_OFF = 0,
    eDCE_BROWSER = 1,
    eDCE_EXIT_CONFIRM = 2,
    eDCE_DELETE_CONFIRM = 3,
    eDCE_LOADING_ANIMATION = 4,
    eDCE_PLAYING_ANIMATION = 5,
    eDCE_SCREENSHOT = 6,
    eDCE_DISTANCE = 7,
    eDCE_CAMERA_SETTINGS = 8,
    eDCE_START_DISTANCE = 9,
    eDCE_END_DISTANCE = 10,
    eDCE_START_KEY = 11,
    eDCE_END_KEY = 12,
    eDCE_START_KEY_AUX = 13,
    eDCE_END_KEY_AUX = 14,
    eDCE_SELECT_COPY_MODE = 15,
    eDCE_NUM_OF_EDIT_STATES = 16
};

enum eICEEditAction {
    eDCE_ADD = 0,
    eDCE_REMOVE = 1,
    eDCE_SET_TYPE = 2,
    eDCE_SET_DATA = 3,
    eDCE_SET_TANGENT_LENGTH = 4,
    eDCE_SET_DISTANCE = 5,
    eDCE_SET_SPACE_EYE = 6,
    eDCE_SET_SPACE_LOOK = 7,
    eDCE_TOGGLE_SMOOTH_ENTRY = 8,
    eDCE_TOGGLE_SMOOTH_EXIT = 9,
    eDCE_TOGGLE_CUBIC_EYE = 10,
    eDCE_TOGGLE_CUBIC_LOOK = 11,
    eDCE_TOGGLE_CONSTRAIN_CARS = 12,
    eDCE_TOGGLE_CONSTRAIN_WORLD = 13,
    eDCE_TOGGLE_IGNORE_ORIENTATION = 14,
    eDCE_TOGGLE_CAR_SPACE_LAG = 15,
    eDCE_SET_SPLINE_DISTANCE = 16,
    eDCE_SET_OVERLAY = 17,
    eDCE_SET_SHAKE_TYPE = 18,
    eDCE_SET_PARAMETER_LENGTH = 19
};

enum eICECopyMode {
    eDCM_COPY_ALL = 0,
    eDCM_COPY_SPACE = 1,
    eDCM_COPY_TARGET = 2,
    eDCM_COPY_LENS = 3,
    eDCM_COPY_DOF = 4,
    eDCM_COPY_SHAKE = 5,
    eDCM_COPY_FX = 6,
    eDCM_NUM_COPY_MODES = 7
};

enum eICESetting {
    eDCS_TYPE = 0,
    eDCS_SMOOTH_ENTRY = 1,
    eDCS_SMOOTH_EXIT = 2,
    eDCS_SPACE_EYE = 3,
    eDCS_CUBIC_EYE = 4,
    eDCS_SPACE_LOOK = 5,
    eDCS_CUBIC_LOOK = 6,
    eDCS_CONSTRAIN_CARS = 7,
    eDCS_CONSTRAIN_WORLD = 8,
    eDCS_IGNORE_ORIENTATION = 9,
    eDCS_CAR_SPACE_LAG = 10,
    eDCS_OVERLAY = 11,
    eDCS_SHAKE_TYPE = 12,
    eDCS_NUM_SETTINGS = 13
};

    ICEShakeGroup();

    void FlushAllocatedTracks();
    struct ICEShakeTrack *GetTrack(int n);

    int GetNumTracks() { return NumTracks; }

    void FlushTracks();

    void AddTrack(struct ICEShakeTrack *track);

    int NumTracks;                                 // offset 0x0, size 0x4
    struct bTList<struct ICEShakeTrack> TrackList; // offset 0x4, size 0x8
};

// total size: 0x18
struct ICEShakeData {
    void PlatEndianSwap();

    float q[3]; // offset 0x0, size 0xC
    float p[3]; // offset 0xC, size 0xC
};

// total size: 0xB60
class ICEShakeTrack : public bTNode<ICEShakeTrack> {
  public:
    bool IsAllocated() { return Allocated != 0; }

    void PlatEndianSwap();

    int GetNumKeys() { return NumKeys; }

    char *GetName() { return Name; }

    ICEShakeData *GetKey(int n) { return (n == ICE::Clamp(n, 0, NumKeys - 1)) ? &Keys[n] : 0; }

    void SetGroup(ICEShakeGroup *group) { Group = group; }

    int MemoryImageSize() { return sizeof(ICEShakeTrack) - (120 - NumKeys) * sizeof(ICEShakeData); }

    ICEShakeGroup *Group;   // offset 0x8, size 0x4
    int16 NumKeys;          // offset 0xC, size 0x2
    int8 Allocated;         // offset 0xE, size 0x1
    char Name[14];          // offset 0xF, size 0xE
    ICEShakeData Keys[120]; // offset 0x20, size 0xB40
};

// total size: 0x80
class ICEManager {
  public:
    void MaybeAllocate();

    void SetupAnimElevation();

    bool IsGenericCameraPlaying();  // Decl: 49

    ICEGroup *GetCurrentGroup();

    ICETrack *GetCurrentTrack();

    char *GetShakeTypeName(uint32 shake_type);

    uint32 GetRelativeShakeType(uint32 shake_type, int inc);

    float GetTrackLengthByName(char *trackName);

    bool IsSmoothExit();  // Decl: 54

    float IsUsingRealTime() { // Decl: 56
        return bUseRealTime;
    }

    int GetNumGroupsWithData(ICEContext context);

    int GetNumGroups();

    ICEManager();

    void Init();

    void Resolve();

    ICEGroup *AddCameraGroup(ICEContext context, unsigned int handle);
    ICEGroup *GetCameraGroup(ICEContext ctx, unsigned int handle);
    ICEGroup *GetNisCameraGroup(unsigned int scene_hash);
    ICEGroup *GetFmvCameraGroup(unsigned int scene_hash);
    ICEGroup *GetReplayCameraGroup(unsigned int category_hash);
    ICEGroup *GetGenericCameraGroup(unsigned int name_hash);
    void LoadCameraSet(bChunk *chunk);
    void UnloadCameraSet(bChunk *chunk);
    void LoadCameraShakes(bChunk *chunk);
    void UnloadCameraShakes(bChunk *chunk);

    ICEData *GetCameraData(uint32 scene_hash, int camTrack);

    int ChooseGoodSceneCameraTrackIndex(uint32 scene_hash, const ICE::Matrix4 *matrix);

    ICEData *GetCameraData(ICETrack **ppTrack, float *pParameter0, float *pParameter1);

    int GetCameraIndex(float f_parameter, ICETrack *pTrack);

    void GetSlope(ICE::Vector3 *pEye, ICE::Vector3 *pLook, float *pDutch, float *pLens, ICEData *pKey, int n,
                  ICETrack *pTrack);

    float GetParameter();

    bool RefreshCameraSplines();

    ICEShakeTrack *GetShakeTrack(unsigned int handle);

    float GetParameter(int n, ICETrack *pTrack);

    float GetIntervalSize(ICEData *pKey, ICETrack *pTrack);

    unsigned int GetUseRealTimeRaw() const {
        return *reinterpret_cast<const unsigned int *>(&bUseRealTime);
    }

    ICETrack *GetPlaybackTrack() {
        return pPlaybackTrack;
    }

    int GetState() {
        return nState;
    }

    void SetSmoothExit(bool b) {
        bSmoothExit = b;
    }

    bool GetSmoothExit() const {
        return bSmoothExit;
    }

    void FixAnimElevation(ICE::Vector3 *p);

    float GetAnimElevationFixup(ICE::Vector3 *p);

    int GetNumSceneCameraTrack(uint32 scene_hash);

    void SetGenericCameraToPlay(const char *group_name, const char *track_name);

    ICETrack *ChooseGenericCamera();

    ICEData *GetNeighbour(ICEData *pKey, int dir, ICETrack *pTrack);

    int ChooseCameraPlaybackTrack();

    void ChooseReplayCamera();

    void Update();

    float GetParameterLength() const {
        return fParameterLength;
    }

    float GetTimerSeconds();

    void SetUseRealTime(bool b) {
        bUseRealTime = b;
    }

    bool IsEditorOn() {
        return nState >= 1;
    }

    bool IsEditorOff() {
        return nState < 1;
    }

    unsigned int GetPlayGenericGroupHash() const {
        return nPlayGenericGroupHash;
    }

    void SetUseRealTimeWord(unsigned int v) {
        *reinterpret_cast<unsigned int *>(&bUseRealTime) = v;
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
    bool bSmoothExit;                // offset 0x74, size 0x4
    int nMarkerIndex;               // offset 0x78, size 0x4
    bool bUseRealTime;              // offset 0x7C, size 0x1
};

inline ICEGroup::ICEGroup() : Handle(0), Context(4), NumTracks(0) {}

inline ICEShakeGroup::ICEShakeGroup() : NumTracks(0) {}

inline void ICEGroup::FlushTracks() {

    NumTracks = 0;

    while (!TrackList.IsEmpty()) {

        ICETrack *track = TrackList.RemoveHead();

        if (track->IsAllocated()) {

            delete track;
        }
    }
}

inline void ICEGroup::AddTrack(ICETrack *track) {

    track->SetGroup(this);

    TrackList.AddTail(track);

    NumTracks++;
}

inline void ICEShakeGroup::FlushTracks() {

    NumTracks = 0;

    while (!TrackList.IsEmpty()) {

        ICEShakeTrack *track = TrackList.RemoveHead();

        if (track->IsAllocated()) {

            delete track;
        }
    }
}

inline void ICEShakeGroup::AddTrack(ICEShakeTrack *track) {

    track->SetGroup(this);

    TrackList.AddTail(track);

    NumTracks++;
}

extern ICEManager TheICEManager;

extern bool bMirrorICEData;

class ICEScene;

namespace ICE {

int KeysShared(ICEData *pKey0, int n0, ICEData *pKey1, int n1);
int KeysSharedSpace(ICEData *pKey0, int n0, ICEData *pKey1, int n1);

void FireEventTag(int n);
class ICEScene *FindAnimScene();

unsigned int GetSceneCount();
unsigned int GetSceneHash(unsigned int scene_slot_position);
void GetNameOfSceneHash(unsigned int scene_hash, char *buffer);

}; // namespace ICE

void ICECompleteEventTags();

#endif
