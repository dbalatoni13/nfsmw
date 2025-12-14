#ifndef ANIMATION_ANIMPLAYER_H
#define ANIMATION_ANIMPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimScene.hpp"
#include "AnimWorldScene.hpp"

// total size: 0x10
class CAnimPlayer {
  public:
    CAnimScene *FindAnimScene(int anim_handle);
    bool Stop(int anim_handle, bool delete_instance);
    bool Load(unsigned int anim_id, int camera_track_number, bool DisableZoneSwitching);
    bool Unload(unsigned int anim_id);

  private:
    bTList<CAnimScene> mInstancedAnimSceneList; // offset 0x0, size 0x8
    CAnimWorldScene *mWorldAnimScene;           // offset 0x8, size 0x4
};

extern CAnimPlayer TheAnimPlayer;

#endif
