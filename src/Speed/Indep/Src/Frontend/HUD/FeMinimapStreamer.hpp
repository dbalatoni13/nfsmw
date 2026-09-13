#ifndef FEMINIMAPSTREAMER_H
#define FEMINIMAPSTREAMER_H

#include <types.h>

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"

#define MAX_UNCOMPRESSED_MINIMAPS 6 // :16

// total size: 0xC
class UncompressedMiniMap {
  public:
    int ChopNum;      // offset 0x0
    bChunk *Chunks;   // offset 0x4
    int SizeofChunks; // offset 0x8

    UncompressedMiniMap() {
        ChopNum = 0;
        Chunks = nullptr;
        SizeofChunks = 0;
    }
};

// total size: 0x1B4
// Decl: 33
class ChoppedMiniMapManager {
  public:
    ChoppedMiniMapManager(int numSections); // Decl: 35
    ~ChoppedMiniMapManager();               // Decl: 36

    static void Init();  // Decl: 38
    static void Close(); // Decl: 39

    void Resize(int newNumSections);

    bool IsLoaded() { // Decl: 49
        return LoadingChopNum > 0;
    }

    int Loader(bChunk *chunk);

    int Unloader(bChunk *chunk);

    void GetTextureName(char *buffer, int buffer_size, int chop_num); // Decl: 52

    int CountAllocated();

    void UncompressMaps(int16 *chop_nums, int num_chops);

    void RemoveUncompressedMaps() {
        UncompressMaps(nullptr, 0);
    }

    void SetMapHeader(char *header);

  private:
    int LoadingChopNum;                          // offset 0x0, size 0x4
    int NumSections;                             // offset 0x4, size 0x4
    char map_header[64];                         // offset 0x8, size 0x40
    LZHeader *CompressedMiniMaps[64];            // offset 0x48, size 0x100
    UncompressedMiniMap UncompressedMiniMaps[9]; // offset 0x148, size 0x6C
};

extern ChoppedMiniMapManager *gChoppedMiniMapManager;

#endif
