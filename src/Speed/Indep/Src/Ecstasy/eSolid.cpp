#include "eSolid.hpp"
#include "EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "eLight.hpp"

#include <types.h>

int eUnloadSolidListPlatChunks(bChunk *chunk);
eSolidIndexEntry *GetSolidIndexEntry(eSolidListHeader *list_header, unsigned int name_hash);
eSolid *eFindSolid(unsigned int name_hash);
eSolid *eFindSolid(unsigned int name_hash, eSolidListHeader *solid_list_header);

LoadedTable SolidLoadedTable;
bTList<eSolidListHeader> SolidListHeaderList;
int eDirtySolids;
float TotalFindSolidTime;

void eSolid::GetBoundingBox(bVector3 *min, bVector3 *max) {
    float minx = this->AABBMinX;
    float miny = this->AABBMinY;
    float minz = this->AABBMinZ;

    float maxx = this->AABBMaxX;
    float maxy = this->AABBMaxY;
    float maxz = this->AABBMaxZ;

    min->x = minx;
    min->y = miny;
    min->z = minz;
    max->x = maxx;
    max->y = maxy;
    max->z = maxz;
}

void eSolid::FixTextureTable() {
    if (!AreChunksBeingMoved()) {
        for (int n = 0; n < this->NumTextureTableEntries; n++) {
            eTextureEntry *texture_entry = &this->pTextureTable[n];
            TextureInfo *texture_info = GetTextureInfo(texture_entry->NameHash, 1, 0);
            texture_entry->pTextureInfo = texture_info;
        }
    }
}

void eSolid::FixLightMaterialTable() {
    eLightMaterialEntry *elme = this->LightMaterialTable;
    int num_light_materials = this->NumLightMaterials;
    if (num_light_materials != 0) {
        for (int i = 0; i < num_light_materials; i++) {
            elme->LightMaterial = elGetLightMaterial(elme->NameHash);
            elme++;
        }
    }
}

void EmptySolidTextureFixupInfo(eSolidListHeader *list_header) {
    while (!list_header->TexturePackList.IsEmpty()) {
        list_header->TexturePackList.RemoveHead();
    }
    while (!list_header->DefaultTextureList.IsEmpty()) {
        list_header->DefaultTextureList.RemoveHead();
    }
    list_header->NumTexturePacks = 0;
    list_header->NumDefaultTextures = 0;
}

void eSolidNotifyTextureMoving(TexturePack *texture_pack /* r29 */, TextureInfo *texture_info /* r28 */) {}

// UNSOLVED https://decomp.me/scratch/sdOFh
bool eSolid::NotifyTextureLoading(TexturePack *texture_pack /* r27 */) {
    bool textures_changed = false;

    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];
        TextureInfo *texture_info = texture_entry->pTextureInfo;

        if (texture_info == DefaultTextureInfo) {
            texture_info = FixupTextureInfoLoading(texture_info, texture_entry->NameHash, texture_pack);
        }
        if (texture_info != texture_entry->pTextureInfo) {
            texture_entry->pTextureInfo = texture_info;
            textures_changed = true;
        }
    }
    return textures_changed;
}

// UNSOLVED
bool eSolid::NotifyTextureUnloading(TexturePack *texture_pack) {
    bool textures_changed = false;

    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];
        TextureInfo *texture_info = texture_entry->pTextureInfo;

        if (texture_info->pTexturePack == texture_pack) {
            texture_info = FixupTextureInfoUnloading(texture_info, texture_entry->NameHash, texture_pack);
        }
        if (texture_info != texture_entry->pTextureInfo) {
            texture_entry->pTextureInfo = texture_info;
            textures_changed = true;
        }
    }
    return textures_changed;
}

void eSolid::NotifyTextureMoving(TexturePack *texture_pack) {
    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];
        TextureInfo *texture_info = texture_entry->pTextureInfo;

        if (texture_info->pTexturePack == texture_pack) {
            // what?...
            texture_entry->pTextureInfo = reinterpret_cast<TextureInfo *>((reinterpret_cast<char *>(texture_info)) + GetChunkMovementOffset());
        }
    }
}

void eSolid::NotifyTextureMoving(TexturePack *texture_pack, TextureInfo *texture_info) {
    for (int n = 0; n < this->NumTextureTableEntries; n++) {
        eTextureEntry *texture_entry = &this->pTextureTable[n];

        if (texture_entry->pTextureInfo == texture_info) {
            // what?...
            texture_entry->pTextureInfo = reinterpret_cast<TextureInfo *>((reinterpret_cast<char *>(texture_info)) + GetChunkMovementOffset());
        }
    }
}

void eSolid::ReplaceLightMaterial(unsigned int old_name_hash, eLightMaterial *new_light_material) {
    if (!new_light_material) {
        return;
    }
    eLightMaterialEntry *elme = this->LightMaterialTable;
    unsigned int new_name_hash;
    int num_light_materials = this->NumLightMaterials;

    for (int i = 0; i < num_light_materials; i++) {
        if (elme->NameHash == old_name_hash) {
            elme->LightMaterial = new_light_material;
        }
        elme++;
    }
}

// UNSOLVED regswap between r4 and r11
ePositionMarker *eSolid::GetPostionMarker(ePositionMarker *prev_marker /* r11 */) {
    ePositionMarker *position_marker_table = this->PositionMarkerTable;
    int numposition_markers = this->NumPositionMarkerTableEntries;
    ePositionMarker *next_marker;

    if (!position_marker_table || (numposition_markers == 0)) {
        return nullptr;
    }
    if (prev_marker) {
        if (prev_marker >= position_marker_table) {
            if (prev_marker < &position_marker_table[numposition_markers - 1]) {
                return prev_marker + 1;
            }
        }
        return nullptr;
    }
    return position_marker_table;
}

// UNSOLVED
ePositionMarker *eSolid::GetPostionMarker(unsigned int namehash /* r31 */) {
    ePositionMarker *position_marker = nullptr;
    for (position_marker = this->GetPostionMarker(position_marker); position_marker; position_marker = this->GetPostionMarker(position_marker)) {
        if (position_marker->NameHash == namehash) {
            return position_marker;
        }
    }
    return nullptr;
}

eSolidListHeader *InternalUnloaderSolidHeaderChunks(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_MESH_CONTAINER_INFO) {
        return nullptr;
    }
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk < last_chunk) {
        unsigned int current_chunk_id = current_chunk->GetID();

        if (current_chunk_id == BCHUNK_MESH_CONTAINER_HEADER) {
            solid_list_header = reinterpret_cast<eSolidListHeader *>(current_chunk->GetData());
            solid_list_header->Remove();
            EmptySolidTextureFixupInfo(solid_list_header);
            LoadedSolidStats.NumLoadedLists--;
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_KEYS) {
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_OFFSETS) {
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_EMPTY) {
            eUnloadSolidListPlatChunks(current_chunk);
        }
        current_chunk = current_chunk->GetNext();
    }

    return solid_list_header;
}

eSolidListHeader *InternalLoaderSolidHeaderChunks(bChunk *chunk) {
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();

    while (current_chunk < last_chunk) {
        unsigned int current_chunk_id = current_chunk->GetID();
        if (current_chunk_id == BCHUNK_MESH_CONTAINER_HEADER) {
            solid_list_header = reinterpret_cast<eSolidListHeader *>(current_chunk->GetData());
            if (!solid_list_header->EndianSwapped) {
                solid_list_header->EndianSwap();
            }
            SolidListHeaderList.AddTail(solid_list_header);
            LoadedSolidStats.NumLoadedLists++;
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_KEYS) {
            solid_list_header->SolidIndexEntryTable = reinterpret_cast<eSolidIndexEntry *>(current_chunk->GetData());
            if (solid_list_header->EndianSwapped == 0) {
                for (int i = 0; i < solid_list_header->NumSolids; i++) {
                    solid_list_header->SolidIndexEntryTable[i].EndianSwap();
                };
            }
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_OFFSETS) {
            solid_list_header->SolidStreamEntryTable = reinterpret_cast<eStreamingEntry *>(current_chunk->GetData());
            if (solid_list_header->EndianSwapped == 0) {
                for (int i = 0; i < solid_list_header->NumSolids; i++) {
                    solid_list_header->SolidStreamEntryTable[i].EndianSwap();
                };
            }
        } else if (current_chunk_id == BCHUNK_MESH_CONTAINER_EMPTY) {
            eLoadSolidListPlatChunks(current_chunk);
        }
        current_chunk = current_chunk->GetNext();
    }
    if (solid_list_header) {
        solid_list_header->EndianSwapped = 1;
    }
    solid_list_header->TexturePackList.InitList();
    solid_list_header->DefaultTextureList.InitList();
    return solid_list_header;
}

eSolid *InternalLoaderSolidChunks(bChunk *chunk /* r14 */, eSolidListHeader *solid_list_header /* r1+0x8 */) {}

void InternalUnloaderSolidChunks(bChunk *chunk, eSolidListHeader *solid_list_header) {
    if (chunk->GetID() != BCHUNK_SOLID_PACK) {
        return;
    }
    bChunk *solid_chunk = chunk->GetFirstChunk();
    bChunk *solid_last_chunk = chunk->GetLastChunk();
    eSolid *solid = nullptr;
    while (solid_chunk < solid_last_chunk) {
        unsigned int current_chunk_id = solid_chunk->GetID();
        if (current_chunk_id == BCHUNK_SOLID_INFO) {
            solid = reinterpret_cast<eSolid *>(solid_chunk->GetAlignedData(16));
            solid->UnFixPlatInfo();
        } else if (current_chunk_id == BCHUNK_MESH_NORMAL_SMOOTHER) {
            LoadedSolidStats.TotalNormalSmootherBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == BCHUNK_MESH_SMOTH_VERTICES) {
            LoadedSolidStats.TotalNormalSmootherBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == BCHUNK_MESH_SMOTH_VERTEX_PLATS) {
            LoadedSolidStats.TotalNormalSmootherBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == 0x13401b) {
            LoadedSolidStats.TotalDamageBytes -= solid_chunk->GetSize();
        } else if (current_chunk_id == BCHUNK_SOLID_MARKERS) {
        } else if (current_chunk_id == BCHUNK_MESH_INFO_CONTAINER) {
            solid->UnloaderPlatChunks(solid_chunk);
        }
        solid_chunk = solid_chunk->GetNext();
    }
    if (solid_list_header) {
        eSolidIndexEntry *index_entry = GetSolidIndexEntry(solid_list_header, solid->NameHash);
        if (index_entry) {
            index_entry->Solid = nullptr;
            SolidLoadedTable.SetUnloaded(solid->NameHash);
        }
    }
    solid_list_header->NumTexturePacks = -1;
    NotifySolidUnloader(solid);
    solid->Remove();
    eDirtySolids = 1;
    LoadedSolidStats.NumLoadedSolids--;
    LoadedSolidStats.TotalSolidsByteSize -= chunk->GetSize();
}

int LoaderSolidList(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_GEOMETRY_PACK) {
        return 0;
    }
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk < last_chunk) {
        unsigned int current_chunk_id = current_chunk->GetID();
        if (current_chunk_id == BCHUNK_MESH_CONTAINER_INFO) {
            if (solid_list_header) {
                NotifySolidLoader(solid_list_header);
            }
            solid_list_header = InternalLoaderSolidHeaderChunks(current_chunk);
        } else if (current_chunk_id == BCHUNK_SOLID_PACK) {
            InternalLoaderSolidChunks(current_chunk, solid_list_header);
        }
        current_chunk = current_chunk->GetNext();
    }
    if (solid_list_header) {
        NotifySolidLoader(solid_list_header);
    }
    return 1;
}

int UnloaderSolidList(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_GEOMETRY_PACK) {
        return 0;
    }
    eSolidListHeader *solid_list_header = nullptr;
    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk < last_chunk) {
        unsigned int current_chunk_id = current_chunk->GetID();
        if (current_chunk_id == BCHUNK_MESH_CONTAINER_INFO) {
            solid_list_header = InternalUnloaderSolidHeaderChunks(current_chunk);
        } else if (current_chunk_id == BCHUNK_SOLID_PACK) {
            InternalUnloaderSolidChunks(current_chunk, solid_list_header);
        }
        current_chunk = current_chunk->GetNext();
    }
    return 1;
}

void SolidLoadedStreamingEntryCallback(bChunk *chunk, eStreamingEntry *streaming_entry, eStreamingPack *streaming_pack) {
    if (chunk->GetID() != BCHUNK_SOLID_PACK) {
        EndianSwapChunkHeader(chunk);
        EndianSwapChunkHeadersRecursive(chunk->GetFirstChunk(), chunk->GetLastChunk());
    }
    InternalLoaderSolidChunks(chunk, streaming_pack->SolidListHeader);
    NotifySolidLoader(streaming_pack->SolidListHeader);
}

void SolidUnloadedStreamingEntryCallback(bChunk *chunk, eStreamingEntry *streaming_entry, eStreamingPack *streaming_pack) {
    eSolid *solid = eFindSolid(streaming_entry->NameHash);
    if (solid) {
        InternalUnloaderSolidChunks(chunk, streaming_pack->SolidListHeader);
    }
}

void eLoadStreamingSolid(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num) {
    StreamingSolidPackLoader.LoadStreamingEntry(name_hash_table, num_hashes, callback, param0, memory_pool_num);
}

void eUnloadStreamingSolid(unsigned int *name_hash_table, int num_hashes) {
    StreamingSolidPackLoader.UnloadStreamingEntry(name_hash_table, num_hashes);
}

void eWaitForStreamingSolidPackLoading(const char *filename) {
    StreamingSolidPackLoader.WaitForLoadingToFinish(filename);
}

int eLoadStreamingSolidPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num) {
    eStreamingPack *streaming_pack = StreamingSolidPackLoader.CreateStreamingPack(filename, callback_function, callback_param, memory_pool_num);
    return streaming_pack != nullptr;
}

eSolidIndexEntry *GetSolidIndexEntry(eSolidListHeader *list_header, unsigned int name_hash) {
    if (!list_header) {
        return nullptr;
    } else {
        return reinterpret_cast<eSolidIndexEntry *>(ScanHashTableKey32(name_hash, list_header->SolidIndexEntryTable, list_header->NumSolids, 0, 8));
    }
}

void SolidLoadingStreamingPackPhase1(eStreamingPackHeaderLoadingInfoPhase1 *loading_info) {
    bChunk *list_chunk = loading_info->TempHeaderChunks;
    EndianSwapChunkHeader(list_chunk);
    bChunk *null_chunk = list_chunk->GetFirstChunk();
    EndianSwapChunkHeader(null_chunk);
    bChunk *header_chunk = null_chunk->GetNext();
    EndianSwapChunkHeader(header_chunk);
    int header_chunks_size = header_chunk->GetSize();
    loading_info->NextLoadAmount = header_chunks_size + 16;
    loading_info->NextLoadPosition = (uintptr_t)header_chunk - (uintptr_t)list_chunk;
}

void SolidLoadingStreamingPackPhase2(eStreamingPackHeaderLoadingInfoPhase2 *loading_info) {
    bChunk *header_chunks = loading_info->HeaderChunks;
    if (loading_info->HeaderChunksMemCopied == 0) {
        EndianSwapChunkHeader(header_chunks);
    }
    EndianSwapChunkHeadersRecursive(header_chunks->GetFirstChunk(), header_chunks->GetLastChunk());

    eSolidListHeader *solid_list_header = InternalLoaderSolidHeaderChunks(header_chunks);
    loading_info->StreamingEntryTable = solid_list_header->SolidStreamEntryTable;
    loading_info->StreamingEntryNumEntries = solid_list_header->NumSolids;
    loading_info->SolidListHeader = solid_list_header;

    if (solid_list_header->PermChunkByteSize != 0) {
        loading_info->LoadResourceFilePosition = solid_list_header->PermChunkByteOffset;
        loading_info->LoadResourceFileAmount = solid_list_header->PermChunkByteSize;
    }
}

int eUnloadStreamingSolidPack(const char *filename) {
    return StreamingSolidPackLoader.DeleteStreamingPack(filename);
}

void SolidUnloadingStreamingPack(eStreamingPack *streaming_pack) {
    InternalUnloaderSolidHeaderChunks(streaming_pack->HeaderChunks);
}

void eInitSolids() {
    bMemSet(&LoadedSolidStats, 0, sizeof(LoadedSolidStats));
    InitStreamingPacks();
}

eSolid *eFindSolid(unsigned int name_hash) {
    return eFindSolid(name_hash, nullptr);
}

// UNSOLVED
eSolid *eFindSolid(unsigned int name_hash /* r31 */, eSolidListHeader *solid_list_header /* r30 */) {
    if (SolidLoadedTable.IsLoaded(name_hash)) {
        return nullptr;
    }
    unsigned int start_time = bGetTicker();
    eSolid *solid = nullptr; // r27
    if (solid_list_header) {
        eSolidIndexEntry *index_entry = GetSolidIndexEntry(solid_list_header, name_hash);
        if (index_entry) {
            solid = index_entry->Solid;
        }
    } else {
        for (eSolidListHeader *list_header /* r30 */ = SolidListHeaderList.GetHead(); list_header != SolidListHeaderList.EndOfList();
             list_header = list_header->GetNext()) {
            eSolidIndexEntry *index_entry = GetSolidIndexEntry(list_header, name_hash);
            if (!index_entry) {
                continue;
            }
            solid = index_entry->Solid;
            if (solid) {
                SolidListHeaderList.Remove(list_header);
                SolidListHeaderList.AddHead(list_header);
                break;
            }
        }
    }
    TotalFindSolidTime += bGetTickerDifference(start_time, bGetTicker());

    return solid;
}
