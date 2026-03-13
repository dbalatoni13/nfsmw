#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern void bEndianSwap32(void *data);
extern void bEndianSwap16(void *data);
extern int bSNPrintf(char *buf, int size, const char *fmt, ...);

ChoppedMiniMapManager *gChoppedMiniMapManager;

void ChoppedMiniMapManager::Init() {
    if (gChoppedMiniMapManager == nullptr) {
        gChoppedMiniMapManager = new ChoppedMiniMapManager(9);
    }
}

ChoppedMiniMapManager::ChoppedMiniMapManager(int numSections) {
    UncompressedMiniMap *p = &UncompressedMiniMaps[0];
    for (int i = 8; i >= 0; i--) {
        p->ChopNum = 0;
        p->Chunks = nullptr;
        p->SizeofChunks = 0;
        p++;
    }
    LoadingChopNum = 0;
    NumSections = numSections;
    for (int i = 0; i <= 63; i++) {
        CompressedMiniMaps[i] = nullptr;
    }
}

int ChoppedMiniMapManager::Loader(bChunk *chunk) {
    int id = *reinterpret_cast<int *>(chunk);
    if (id == 0x3A100) {
        LZHeader *lzh = reinterpret_cast<LZHeader *>(reinterpret_cast<char *>(chunk) + 8);
        bEndianSwap32(lzh);
        bEndianSwap16(reinterpret_cast<char *>(chunk) + 0xE);
        bEndianSwap32(reinterpret_cast<char *>(lzh) + 8);
        bEndianSwap32(reinterpret_cast<char *>(chunk) + 0x14);
        int idx = LoadingChopNum;
        CompressedMiniMaps[idx] = lzh;
        LoadingChopNum = idx + 1;
        return 1;
    }
    return 0;
}

int ChoppedMiniMapManager::Unloader(bChunk *chunk) {
    if (*reinterpret_cast<int *>(chunk) == 0x3A100) {
        LoadingChopNum = LoadingChopNum - 1;
        CompressedMiniMaps[LoadingChopNum] = nullptr;
        if (LoadingChopNum == 0) {
            UncompressMaps(nullptr, 0);
        }
        return 1;
    }
    return 0;
}

void ChoppedMiniMapManager::SetMapHeader(char *header) {
    bSNPrintf(map_header, 0x40, header);
}

void ChoppedMiniMapManager::GetTextureName(char *buffer, int buffer_size, int chop_num) {
    bSNPrintf(buffer, buffer_size, "%s_%d", map_header, chop_num);
}