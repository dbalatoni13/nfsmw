#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern void bEndianSwap32(void *data);
extern void bEndianSwap16(void *data);
extern int bSNPrintf(char *buf, int size, const char *fmt, ...);
extern int LZDecompress(unsigned char *pSrc, unsigned char *pDst);
extern void UnloadChunks(bChunk *chunks, int sizeof_chunks, const char *debug_name);
extern void LoadEmbeddedChunks(bChunk *chunk, int sizeof_chunks, const char *debug_name);

ChoppedMiniMapManager *gChoppedMiniMapManager;

void ChoppedMiniMapManager::Init() {
    if (gChoppedMiniMapManager == nullptr) {
        gChoppedMiniMapManager = new ChoppedMiniMapManager(9);
    }
}

ChoppedMiniMapManager::ChoppedMiniMapManager(int numSections) {
    LoadingChopNum = 0;
    NumSections = numSections;
    for (int i = 0; i <= 63; i++) {
        CompressedMiniMaps[i] = nullptr;
    }
}

int ChoppedMiniMapManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x3A100) {
        LZHeader *header = reinterpret_cast<LZHeader *>(chunk->GetData());
        bPlatEndianSwap(reinterpret_cast<unsigned int *>(header));
        bPlatEndianSwap(reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(chunk) + 0xE));
        bPlatEndianSwap(reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(header) + 8));
        bPlatEndianSwap(reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(chunk) + 0x14));
        CompressedMiniMaps[LoadingChopNum++] = header;
        return 1;
    }
    return 0;
}

int ChoppedMiniMapManager::Unloader(bChunk *chunk) {
    if (chunk->GetID() == 0x3A100) {
        LoadingChopNum = LoadingChopNum - 1;
        CompressedMiniMaps[LoadingChopNum] = nullptr;
        if (LoadingChopNum == 0) {
            RemoveUncompressedMaps();
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
void ChoppedMiniMapManager::UncompressMaps(short *chop_nums, int num_chops) {
    for (int n = 0; n < NumSections; n++) {
        UncompressedMiniMap *map = &UncompressedMiniMaps[n];
        if (map->Chunks) {
            int keep_map = -1;
            for (int i = 0; i < num_chops; i++) {
                if (chop_nums[i] == static_cast<short>(map->ChopNum)) {
                    keep_map = i;
                    break;
                }
            }
            if (keep_map < 0) {
                UnloadChunks(map->Chunks, map->SizeofChunks, "MiniMap Chop");
                bFree(map->Chunks);
                map->Chunks = nullptr;
                map->SizeofChunks = 0;
            }
        }
    }

    for (int i = 0; i < num_chops; i++) {
        int chop_num = chop_nums[i];
        int n = 0;
        UncompressedMiniMap *free_map = nullptr;

        for (; n < NumSections; n++) {
            UncompressedMiniMap *map = &UncompressedMiniMaps[n];
            if (!map->Chunks) {
                free_map = map;
            } else if (map->ChopNum == chop_num) {
                break;
            }
        }

        if (n == NumSections && chop_num >= 0) {
            void *lz_header = CompressedMiniMaps[chop_num];
            if (lz_header) {
                free_map->ChopNum = chop_num;
                int size = *reinterpret_cast<int *>(reinterpret_cast<char *>(lz_header) + 8);
                free_map->SizeofChunks = size;
                free_map->Chunks = static_cast<bChunk *>(bMalloc(size, 0x2000));
                LZDecompress(reinterpret_cast<unsigned char *>(lz_header),
                             reinterpret_cast<unsigned char *>(free_map->Chunks));
                LoadEmbeddedChunks(free_map->Chunks, free_map->SizeofChunks, "MiniMap Chop embedded");
            }
        }
    }
}
