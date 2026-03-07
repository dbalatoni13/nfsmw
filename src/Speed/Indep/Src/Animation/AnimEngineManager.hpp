#ifndef ANIMATION_ANIM_ENGINE_MANAGER_H
#define ANIMATION_ANIM_ENGINE_MANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x1
class CAnimEngineManager {
  public:
    CAnimEngineManager();

    ~CAnimEngineManager();

    static void Init();

    static void Purge();
};

void InitNFSAnimEngine();

#endif
