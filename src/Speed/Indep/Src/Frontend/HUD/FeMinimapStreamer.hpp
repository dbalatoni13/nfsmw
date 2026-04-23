#ifndef FRONTEND_HUD_FEMINIMAPSTREAMER_H
#define FRONTEND_HUD_FEMINIMAPSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct bChunk;
struct LZHeader;

// total size: 0xC
struct UncompressedMiniMap {
    int ChopNum;         // offset 0x0
    bChunk *Chunks;      // offset 0x4
    int SizeofChunks;    // offset 0x8

    UncompressedMiniMap() : ChopNum(0) //
        , Chunks(nullptr) //
        , SizeofChunks(0) {}
};

// total size: 0x1B4
struct ChoppedMiniMapManager {
    static void Init();
    static void Close();

    ChoppedMiniMapManager(int numSections);
    ~ChoppedMiniMapManager();

    int Loader(bChunk *chunk);
    int Unloader(bChunk *chunk);
    void SetMapHeader(char *header);
    void GetTextureName(char *buffer, int buffer_size, int chop_num);
    int CountAllocated();
    void UncompressMaps(short *chop_nums, int num_chops);
    void Resize(int newNumSections);

    bool IsLoaded() {
        return LoadingChopNum > 0;
    }

    void RemoveUncompressedMaps() {
        UncompressMaps(nullptr, 0);
    }

    int LoadingChopNum;                     // offset 0x0
    int NumSections;                        // offset 0x4
    char map_header[64];                    // offset 0x8
    LZHeader *CompressedMiniMaps[64];       // offset 0x48
    UncompressedMiniMap UncompressedMiniMaps[9]; // offset 0x148
};

extern ChoppedMiniMapManager *gChoppedMiniMapManager;

#endif
