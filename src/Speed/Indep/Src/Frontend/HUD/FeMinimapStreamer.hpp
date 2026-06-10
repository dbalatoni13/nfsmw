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
};

// File: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp
// total size: 0x1B4
// Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:33
class ChoppedMiniMapManager {
  public:
    ChoppedMiniMapManager(int numSections); // Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:35
    ~ChoppedMiniMapManager();               // Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:36

    static void Init();  // Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:38
    static void Close(); // Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:39

    void Resize(int newNumSections);

    bool IsLoaded() { // Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:49
        return LoadingChopNum > 0;
    }

    int Loader(struct bChunk *chunk);

    int Unloader(struct bChunk *chunk);

    void GetTextureName(char *buffer, int buffer_size, int chop_num); // Decl: speed/indep/src/frontend/hud/FeMinimapStreamer.hpp:52

    int CountAllocated();

    void UncompressMaps(short *chop_nums, int num_chops);

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
