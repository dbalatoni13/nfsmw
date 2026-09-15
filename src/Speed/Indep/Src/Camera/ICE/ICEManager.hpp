#ifndef ICEMANAGER_HPP_
#define ICEMANAGER_HPP_

#include "Speed/Indep/Src/Camera/ICE/ICEData.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed\Indep\Src\Camera\ICE\ICEAnimScene.hpp"
#include "Speed\Indep\Src\Interfaces\SimActivities\INIS.h"
#include "Speed\Indep\Src\Camera\ICE\ICEMath.hpp"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
// #include "Speed\Indep\Src\Interfaces\Simables\IRigidBody.h"
#include "Speed\Indep\Src\Physics\Behaviors\RigidBody.h"

bool bMirrorICEData = false; // move?

class ICEGroup {
  private:
    // total size: 0x14
    uint32 Handle;                     // offset 0x0, size 0x4
    int Context;                       // offset 0x4, size 0x4
    int NumTracks;                     // offset 0x8, size 0x4
    bTList<struct ICETrack> TrackList; // offset 0xC, size 0x8
  public:
    inline unsigned int GetHandle() {
        return Handle;
    }
    int GetNumTracks() const {
        return NumTracks;
    }
    ICETrack *GetTrack(int n);

    ICETrack *GetTrack(char);
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
  public:
    inline ICEData *GetKey(int n) {
        return &this->Keys[ICE::Clamp(n, 0, this->NumKeys - 1)];
    }
    inline int GetNumKeys() {
        return NumKeys - 1;
    }
    int GetKeyNumber(float f_param);
};
int ICETrack::GetKeyNumber(float f_param) {
    int n = 0;
    for (int i = 0; i < NumKeys; ++i) {
        if (Keys[i].fParameter <= f_param) {
            n = i;
        }
    }
    return n;
}
// total size: 0x80
// Decl: 14
class ICEManager {
  public:
    ICEManager();  // Decl: 17
    ~ICEManager(); // Decl: 18

    bool IsEditorOn() { // Decl: 26
        // TODO maybe negated?
        return nState >= 1;
    }
    bool IsEditorOff() { // Decl: 27
        return nState == 0;
    }

    int GetState() {
        return nState;
    }

    float GetParameter();

// total size: 0x80
class ICEManager {
  public:
    float GetAnimElevationFixup(ICE::Vector3 *position);

    void FixAnimElevation(ICE::Vector3 *position);

    void SetGenericCameraToPlay(char const *group_name, char const *track_name);

    ICEGroup *GetNisCameraGroup(uint32 scene_hash);

    float GetTimerSeconds();

    bool RefreshCameraSplines();

    int ChooseGoodSceneCameraTrackIndex(uint32 scene_hash, const ICE::Matrix4 *scene_origin);

    ICEManager();
    float GetParameterLength() {
        return fParameterLength;
    }

    void MaybeAllocate();

    void Init();                                // Decl: 36
    void Update();                              // Decl: 37
    void Resolve();                             // Decl: 38
    void Render(eView *p_view);                 // Decl: 39
    void LoadCameraSet(bChunk *set_chunk);      // Decl: 40
    void UnloadCameraSet(bChunk *set_chunk);    // Decl: 41
    void LoadCameraShakes(bChunk *set_chunk);   // Decl: 42
    void UnloadCameraShakes(bChunk *set_chunk); // Decl: 43

    int GetCameraIndex(float f_param, struct ICETrack *track);

    ICEData *GetCameraData(uint32 scene_hash, int camTrack);
    ICEData *GetCameraData(ICETrack **p_track, float *p_start, float *p_end);

    void GetSlope(Vector3 *p_eye_slope, Vector3 *p_look_slope, float *p_dutch_slope, float *p_lens_slope, ICEData *p_camera, int n_key,
                  ICETrack *p_track);

    void Update();
    bool RefreshCameraSplines();

    void FixAnimElevation(Vector3 *position);
    void SetupAnimElevation();
    float GetAnimElevationFixup(Vector3 *position);

    void SetGenericCameraToPlay(const char *group_name, const char *track_name); // Decl: 48
    bool IsGenericCameraPlaying() {}                                             // Decl: 49

    ICEGroup *GetCurrentGroup();
    ICETrack *GetCurrentTrack();
    ICETrack *GetPlaybackTrack() {}
    ICEShakeTrack *GetShakeTrack(uint32 shake_type);
    char *GetShakeTypeName(uint32 shake_type);
    uint32 GetRelativeShakeType(uint32 shake_type, int inc);

    float GetTrackLengthByName(char *trackName);

    void ChooseReplayCamera();        // Decl: 51
    bool ChooseCameraPlaybackTrack(); // Decl: 52
    int GetNumSceneCameraTrack(uint32 scene_hash);
    int GetCameraIndex(float f_param, ICETrack *track);
    void SetSmoothExit(bool smooth) {} // Decl: 53
    bool IsSmoothExit() {}             // Decl: 54
    // int ChooseGoodSceneCameraTrackIndex(uint32 scene_hash, Matrix4 *scene_origin);
    void SetUseRealTime(bool val) { // Decl: 55
        bUseRealTime = val;
    }
    float IsUsingRealTime() { // Decl: 56
        return bUseRealTime;
    }
    float GetTimerSeconds() { // Decl: 57
        return bUseRealTime ? WorldTimer.GetSeconds() : RealTimer.GetSeconds();
    }

    bool IsEditorOff() {
        // TODO maybe negated?
        return nState < 1;
    }

  private:
    float GetParameter(int i, ICETrack *track);
    float GetIntervalSize(ICEData *data, ICETrack *track);
    float GetIntervalSize(int i);

    ICEData *GetNeighbour(ICEData *data, int key, ICETrack *track);

    ICETrack *ChooseGenericCamera();

    ICEGroup *GetNisCameraGroup(uint32 scene_hash);              // Decl: 65
    ICEGroup *GetFmvCameraGroup(uint32 scene_hash);              // Decl: 66
    ICEGroup *GetReplayCameraGroup(uint32 category_hash);        // Decl: 68
    ICEGroup *GetGenericCameraGroup(uint32 name_hash);           // Decl: 69
    ICEGroup *AddCameraGroup(ICEContext context, uint32 handle); // Decl: 62
    ICEGroup *GetCameraGroup(ICEContext context, uint32 handle); // Decl: 63

    int GetNumGroupsWithData(ICEContext context);
    int GetNumGroups();
    int GetNumTracks();

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
    uint32 nPlayGenericGroupHash;   // offset 0x60, size 0x4, Decl: 83
    char nPlayGenericTrackName[14]; // offset 0x64, size 0xE
    bool bSmoothExit;               // offset 0x74, size 0x1, Decl: 85
    int nMarkerIndex;               // offset 0x78, size 0x4
    bool bUseRealTime;              // offset 0x7C, size 0x1
};

extern ICEManager TheICEManager; // size: 0x80, Decl: 93

void ICECompleteEventTags();

#endif
