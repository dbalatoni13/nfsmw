#ifndef ANIMATION_ANIMPLAYER_H
#define ANIMATION_ANIMPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimScene.hpp"
#include "AnimWorldScene.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class CAnimMarker : bTNode<CAnimMarker> {
  public:
    CAnimMarker(uint32 name_hash, float time);
    virtual ~CAnimMarker();
    uint32 GetNameHash();
    float GetTime();

    static uint32 mMarkerHash_StartCountdown;

  private:
    uint32 mNameHash; // offset 0x8, size 0x4
    float mTime;      // offset 0xC, size 0x4
};

// total size: 0x1
class CAnimSettings {
  public:
    static void SetDebugPrintEnabled(bool enable);
    static bool IsDebugPrintEnabled();

  private:
    static bool mDebugPrintEnabled; // size: 0x1
};

// TODO move?
enum eAnimCategory {
    eAnimCategory_None = 0,
    eAnimCategory_Race = 1,
    eAnimCategory_MaxAnimCategory = 2,
};

// total size: 0x10
class CAnimPlayer {
  public:
    bTList<CAnimScene> *GetInstancedAnimSceneList() {}

    CAnimPlayer();

    virtual ~CAnimPlayer();

    static void StreamCued();

    bool Load(uint32 anim_id, int camera_track_number, bool DisableZoneSwitching);

    bool Unload(uint32 anim_id);

    bool UnloadAll();

    bool UnloadCategory(eAnimCategory category_id);

    bool IsLoaded(uint32 anim_id);

    int CreateAnimInstance(uint32 anim_id, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    void DeleteAnimInstance(AnimHandle anim_handle);

    int CreateAnimScene(CAnimSceneData *anim_scene_data, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    void DestroyAnimScene(AnimHandle anim_handle);

    int CreateAndPlayAnim(uint32 anim_id, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    CAnimScene *FindAnimScene(AnimHandle anim_handle);

    int FindHandle(eAnimCategory category_id);

    int GetSceneType(AnimHandle anim_handle);

    bool SetPropertyEnabled(AnimHandle anim_handle, eAnimProperty property_id, bool enable);

    bool IsPropertyEnabled(AnimHandle anim_handle, eAnimProperty property_id);

    bool AreAnyWithThisPropertyEnabled(eAnimProperty property_id);

    bool AreAnyControllingCamera();

    bool SetTime(AnimHandle anim_handle, float time);

    bool GetTime(AnimHandle anim_handle, float &time);

    bool Cue(AnimHandle anim_handle);

    bool Play(AnimHandle anim_handle);

    bool Stop(AnimHandle anim_handle, bool delete_instance);

    bool Pause(AnimHandle anim_handle);

    bool UnPause(AnimHandle anim_handle);

    bool StopAll(bool delete_instances);

    bool StopCategory(eAnimCategory category_id, bool delete_instances);

    bool PauseAll();

    bool UnPauseAll();

    bool IsCued(AnimHandle anim_handle);

    bool IsPlaying(AnimHandle anim_handle);

    bool IsStopped(AnimHandle anim_handle);

    bool IsPaused(AnimHandle anim_handle);

    bool IsFinished(AnimHandle anim_handle);

    bool AreAllPlaying();

    bool AreAllStopped();

    bool AreAllPaused();

    void UpdateTime(float time_step);

    void BeginOnlinePause();

    void EndOnlinePause();

    bool Init();

    void Purge();

    void InitWorldAnimScene();

    CAnimWorldScene *GetWorldAnimScene();

    void KillWorldAnimScene(bool full_unload, bool quickrace_drag_restart);

    bool GetWorlAnim(uint32 name_hash);

  private:
    static bool m_audioQueued; // size: 0x1, address: 0x80415710

    bTList<CAnimScene> mInstancedAnimSceneList; // offset 0x0, size 0x8
    CAnimWorldScene *mWorldAnimScene;           // offset 0x8, size 0x4
};

class CarAnimationState {
  public:
    CarAnimationState();
    CAnimCtrl *AnimCtrl;     // offset 0x0
    IVehicle *mIVehicle;     // offset 0x4
    int HaveLastCarPosition; // offset 0x8
    int CarIndex;            // offset 0xC
};

extern CAnimPlayer TheAnimPlayer;
extern CarAnimationState gCarAnimationStates[16];

#endif
