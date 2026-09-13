#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

ChoppedMiniMapManager *gChoppedMiniMapManager;

void ChoppedMiniMapManager::Init() {
    if (gChoppedMiniMapManager == nullptr) {
        gChoppedMiniMapManager = new ("ChoppedMiniMapManager", 0) ChoppedMiniMapManager(9);
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
        bPlatEndianSwap(&header->ID);
        bPlatEndianSwap(&header->Flags);
        bPlatEndianSwap(&header->UncompressedSize);
        bPlatEndianSwap(&header->CompressedSize);
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
    bSNPrintf(map_header, sizeof(map_header), header);
}

void ChoppedMiniMapManager::GetTextureName(char *buffer, int buffer_size, int chop_num) {
    bSNPrintf(buffer, buffer_size, "%s_CHOP%d", map_header, chop_num);
}
void ChoppedMiniMapManager::UncompressMaps(int16 *chop_nums, int num_chops) {
    for (int n = 0; n < NumSections; n++) {
        UncompressedMiniMap *map = &UncompressedMiniMaps[n];

        if (map->Chunks != nullptr) {
            if (!bIsInTable(chop_nums, num_chops, map->ChopNum)) {
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
            if (map->Chunks != nullptr) {
                if (map->ChopNum == chop_num) {
                    break;
                }
            } else {
                free_map = map;
            }
        }

        if (n == NumSections && chop_num >= 0) {
            LZHeader *lz_header = CompressedMiniMaps[chop_num];
            if (lz_header != nullptr) {
                free_map->ChopNum = chop_num;
                int size = lz_header->UncompressedSize;
                free_map->SizeofChunks = lz_header->UncompressedSize;
                free_map->Chunks = static_cast<bChunk *>(bMalloc(free_map->SizeofChunks, "MiniMap Chop", 0, BMEMORY_ALIGNMENT(0x80)));
                LZDecompress(reinterpret_cast<u8 *>(lz_header), reinterpret_cast<u8 *>(free_map->Chunks));
                LoadEmbeddedChunks(free_map->Chunks, free_map->SizeofChunks, "MiniMap Chop embedded");
            }
        }
    }
}
