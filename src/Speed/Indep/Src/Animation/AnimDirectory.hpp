#ifndef ANIMATION_ANIMDIRECTORY_H
#define ANIMATION_ANIMDIRECTORY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define ANIM_SCENE_MAX (256)

// total size: 0x4004
class AnimFileLoadInfo {
  public:
    AnimFileLoadInfo() {}

    uint32 mAnimFileCount;                       // offset 0x0, size 0x4
    char mAnimFileNameTable[ANIM_SCENE_MAX][64]; // offset 0x4, size 0x4000
};

// total size: 0x8
class AnimSceneLoadInfo {
  public:
    AnimSceneLoadInfo()
        : mAnimSceneHash(0),        //
          mSharedFileCount(0),      //
          mSharedFileStartIndex(0), //
          mSceneFileCount(0),       //
          mSceneFileStartIndex(0) {}

    uint32 mAnimSceneHash;       // offset 0x0, size 0x4
    uint8 mSharedFileCount;      // offset 0x4, size 0x1
    uint8 mSharedFileStartIndex; // offset 0x5, size 0x1
    uint8 mSceneFileCount;       // offset 0x6, size 0x1
    uint8 mSceneFileStartIndex;  // offset 0x7, size 0x1
};

// total size: 0x4808
class AnimDirectory {
  public:
    AnimDirectory() {}

    ~AnimDirectory() {}

    uint32 GetFileCount() {
        return mAnimFileLoadInfo.mAnimFileCount;
    }

    char *GetFileName(uint32 file_slot_position) {
        return mAnimFileLoadInfo.mAnimFileNameTable[file_slot_position];
    }

    uint32 GetSceneCount() {
        return mAnimSceneCount;
    }

    void GetSceneLoadInfo(uint32 scene_slot_position, AnimSceneLoadInfo &info) {
        info = mAnimSceneLoadInfo[scene_slot_position];
    }

    AnimSceneLoadInfo *GetSceneLoadInfo(int slot) {
        return &mAnimSceneLoadInfo[slot];
    }

    void GetNameOfSceneNumber(int scene_slot_position, char *buffer) {
        AnimSceneLoadInfo info;
        info = mAnimSceneLoadInfo[scene_slot_position];
        char *file_name = GetFileName(info.mSceneFileStartIndex);
        int pos = 0;
        while (file_name[pos] != '_') {
            pos++;
        }
        pos++;
        int start_pos = pos;
        while (file_name[pos] != '_') {
            pos++;
        }
        int len = pos - start_pos;
        if (len != 0) {
            pos = start_pos;
            len--;
            do {
                *buffer++ = file_name[pos++];
            } while (len-- != 0);
        }
        *buffer = '\0';
    }

    void GetNameOfSceneHash(uint32 scene_hash, char *buffer) {
        *buffer = '\0';
        for (uint32 i = 0; i < GetSceneCount(); i++) {
            AnimSceneLoadInfo *info = GetSceneLoadInfo(i);
            if (scene_hash == info->mAnimSceneHash) {
                GetNameOfSceneNumber(i, buffer);
                return;
            }
        }
    }

    void SetFileCount(uint32 file_count) {}

    void SetFileName(uint32 file_slot_position, char *file_name) {}

    void SetSceneCount(uint32 count) {}

    void SetSceneLoadInfo(uint32 scene_slot_position, const AnimSceneLoadInfo &info) {}

    void EndianSwap() {
        bPlatEndianSwap(&mAnimSceneCount);

        for (int i = 0; i < ANIM_SCENE_MAX; i++) {
            bPlatEndianSwap(&mAnimSceneLoadInfo[i].mAnimSceneHash);
            bPlatEndianSwap(&mAnimSceneLoadInfo[i].mSharedFileCount);
            bPlatEndianSwap(&mAnimSceneLoadInfo[i].mSharedFileStartIndex);
            bPlatEndianSwap(&mAnimSceneLoadInfo[i].mSceneFileCount);
            bPlatEndianSwap(&mAnimSceneLoadInfo[i].mSceneFileStartIndex);
        }

        bPlatEndianSwap(&mAnimFileLoadInfo.mAnimFileCount);
    }

  private:
    uint32 mAnimSceneCount;                               // offset 0x0, size 0x4
    AnimSceneLoadInfo mAnimSceneLoadInfo[ANIM_SCENE_MAX]; // offset 0x4, size 0x800
    AnimFileLoadInfo mAnimFileLoadInfo;                   // offset 0x804, size 0x4004
};

extern AnimDirectory *TheAnimDirectory;

#endif
