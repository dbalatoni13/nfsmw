#ifndef ANIMATION_ANIMDIRECTORY_H
#define ANIMATION_ANIMDIRECTORY_H

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#define ANIM_SCENE_MAX (256)

// total size: 0x4004
struct AnimFileLoadInfo {
    AnimFileLoadInfo() {}

    unsigned int mAnimFileCount;                 // offset 0x0, size 0x4
    char mAnimFileNameTable[ANIM_SCENE_MAX][64]; // offset 0x4, size 0x4000
};

// total size: 0x8
struct AnimSceneLoadInfo {
    AnimSceneLoadInfo()
        : mAnimSceneHash(0), //
          mSharedFileCount(0), //
          mSharedFileStartIndex(0), //
          mSceneFileCount(0), //
          mSceneFileStartIndex(0) {}

    unsigned int mAnimSceneHash;         // offset 0x0, size 0x4
    unsigned char mSharedFileCount;      // offset 0x4, size 0x1
    unsigned char mSharedFileStartIndex; // offset 0x5, size 0x1
    unsigned char mSceneFileCount;       // offset 0x6, size 0x1
    unsigned char mSceneFileStartIndex;  // offset 0x7, size 0x1
};

// total size: 0x4808
class AnimDirectory {
  public:
    AnimDirectory() {}

    ~AnimDirectory() {}

    unsigned int GetFileCount() { return mAnimFileLoadInfo.mAnimFileCount; }

    char *GetFileName(unsigned int file_slot_position) {
        return mAnimFileLoadInfo.mAnimFileNameTable[file_slot_position];
    }

    unsigned int GetSceneCount() { return mAnimSceneCount; }

    void GetSceneLoadInfo(unsigned int scene_slot_position, AnimSceneLoadInfo &info) {
        info = mAnimSceneLoadInfo[scene_slot_position];
    }

    AnimSceneLoadInfo *GetSceneLoadInfo(int slot) { return &mAnimSceneLoadInfo[slot]; }

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

    void GetNameOfSceneHash(unsigned int scene_hash, char *buffer) {
        *buffer = '\0';
        for (unsigned int i = 0; i < GetSceneCount(); i++) {
            AnimSceneLoadInfo *info = GetSceneLoadInfo(i);
            if (scene_hash == info->mAnimSceneHash) {
                GetNameOfSceneNumber(i, buffer);
                return;
            }
        }
    }

    void SetFileCount(unsigned int file_count) {}

    void SetFileName(unsigned int file_slot_position, char *file_name) {}

    void SetSceneCount(unsigned int count) {}

    void SetSceneLoadInfo(unsigned int scene_slot_position, const AnimSceneLoadInfo &info) {}

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
    unsigned int mAnimSceneCount;                         // offset 0x0, size 0x4
    AnimSceneLoadInfo mAnimSceneLoadInfo[ANIM_SCENE_MAX]; // offset 0x4, size 0x800
    AnimFileLoadInfo mAnimFileLoadInfo;                   // offset 0x804, size 0x4004
};

#endif
