#ifndef FRONTEND_HUD_FEMINIMAPSTREAMER_H
#define FRONTEND_HUD_FEMINIMAPSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"

// total size: 0xC
class UncompressedMiniMap {
  public:
    UncompressedMiniMap() {
        ChopNum = 0;
        Chunks = nullptr;
        SizeofChunks = 0;
    }

    int ChopNum;      // offset 0x0
    bChunk *Chunks;   // offset 0x4
    int SizeofChunks; // offset 0x8
};

// total size: 0x1B4
class ChoppedMiniMapManager {
  public:
    ChoppedMiniMapManager(int numSections);
    ~ChoppedMiniMapManager();

    void Resize(int newNumSections);
    bool IsLoaded() {
        return LoadingChopNum > 0;
    }
    int Loader(bChunk *chunk);
    int Unloader(bChunk *chunk);
    static void Init();
    static void Close();
    void GetTextureName(char *buffer, int buffer_size, int chop_num);
    int CountAllocated();
    void UncompressMaps(short *chop_nums, int num_chops);
    void RemoveUncompressedMaps() {
        UncompressMaps(nullptr, 0);
    }
    void SetMapHeader(char *header);

  private:
    int LoadingChopNum;                          // offset 0x0
    int NumSections;                             // offset 0x4
    char map_header[64];                         // offset 0x8
    LZHeader *CompressedMiniMaps[64];            // offset 0x48
    UncompressedMiniMap UncompressedMiniMaps[9]; // offset 0x148
};

extern ChoppedMiniMapManager *gChoppedMiniMapManager;

#endif
