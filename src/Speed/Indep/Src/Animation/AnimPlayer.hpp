#ifndef ANIMATION_ANIMPLAYER_H
#define ANIMATION_ANIMPLAYER_H

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimScene.hpp"
#include "AnimWorldScene.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

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

    bool Load(unsigned int anim_id, int camera_track_number, bool DisableZoneSwitching);

    bool Unload(unsigned int anim_id);

    bool UnloadAll();

    bool UnloadCategory(eAnimCategory category_id);

    bool IsLoaded(unsigned int anim_id);

    int CreateAnimInstance(unsigned int anim_id, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    void DeleteAnimInstance(int anim_handle);

    int CreateAnimScene(CAnimSceneData *anim_scene_data, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    void DestroyAnimScene(int anim_handle);

    int CreateAndPlayAnim(unsigned int anim_id, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    CAnimScene *FindAnimScene(int anim_handle);

    int FindHandle(eAnimCategory category_id);

    int GetSceneType(int anim_handle);

    bool SetPropertyEnabled(int anim_handle, eAnimProperty property_id, bool enable);

    bool IsPropertyEnabled(int anim_handle, eAnimProperty property_id);

    bool AreAnyWithThisPropertyEnabled(eAnimProperty property_id);

    bool AreAnyControllingCamera();

    bool SetTime(int anim_handle, float time);

    bool GetTime(int anim_handle, float &time);

    bool Cue(int anim_handle);

    bool Play(int anim_handle);

    bool Stop(int anim_handle, bool delete_instance);

    bool Pause(int anim_handle);

    bool UnPause(int anim_handle);

    bool StopAll(bool delete_instances);

    bool StopCategory(eAnimCategory category_id, bool delete_instances);

    bool PauseAll();

    bool UnPauseAll();

    bool IsCued(int anim_handle);

    bool IsPlaying(int anim_handle);

    bool IsStopped(int anim_handle);

    bool IsPaused(int anim_handle);

    bool IsFinished(int anim_handle);

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

    bool GetWorlAnim(unsigned int name_hash);

  private:
    static bool m_audioQueued; // size: 0x1, address: 0x80415710

    bTList<CAnimScene> mInstancedAnimSceneList; // offset 0x0, size 0x8
    CAnimWorldScene *mWorldAnimScene;           // offset 0x8, size 0x4
};

struct CarAnimationState {
    CarAnimationState();
    CAnimCtrl *AnimCtrl;   // offset 0x0
    IVehicle *mIVehicle;   // offset 0x4
    int HaveLastCarPosition; // offset 0x8
    int CarIndex;          // offset 0xC
};

extern CAnimPlayer TheAnimPlayer;
extern CarAnimationState gCarAnimationStates[16];

#endif
