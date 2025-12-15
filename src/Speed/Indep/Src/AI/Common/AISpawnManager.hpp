#ifndef AI_COMMON_AISPAWNMANAGER_H
#define AI_COMMON_AISPAWNMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
class AISpawnManager {
  public:
    ~AISpawnManager();

  private:
    float mMinSpawnDist; // offset 0x0, size 0x4
    float mMaxSpawnDist; // offset 0x4, size 0x4
};

#endif
