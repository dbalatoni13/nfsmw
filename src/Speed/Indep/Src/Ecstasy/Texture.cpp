#include "Texture.hpp"
#include "Ecstasy.hpp"
#include "Speed/GameCube/Src/Ecstasy/TextureInfoPlat.hpp"
#include "Speed/Indep/Libs/Support/Miscellaneous/StringHash.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "eStreamingPack.hpp"
#include "version.h"

TextureInfo *DefaultTextureInfo;
SlotPool *TexturePackSlotPool;
BOOL DuplicateTextureWarningEnabled;
bTList<TextureVRAMDataHeader> TextureVRAMDataHeaderList;
bTList<TextureAnimPack> TextureAnimPackList;
bTList<TexturePack> TexturePackList;
TextureInfo *TextureInfoCache[256];
unsigned int TextureInfoCacheSafety[256];
TexturePack *PrevLoadedTexturePack;
int eDirtyTextures;
LoadedTable TextureLoadedTable;

void eInitTextures(void) {
    int size = 28;
    TexturePackSlotPool = bNewSlotPool(bMax(size, 32), 128, "TexturePackSlotPool", 0);
}

void SetDuplicateTextureWarning(BOOL enabled) {
    DuplicateTextureWarningEnabled = enabled;
}

TextureVRAMDataHeader *FindVRAMData(unsigned int filename_hash) {
    for (TextureVRAMDataHeader *vram_header = TextureVRAMDataHeaderList.GetHead(); vram_header != TextureVRAMDataHeaderList.EndOfList();
         vram_header = vram_header->GetNext()) {
        if (vram_header->FilenameHash == filename_hash) {
            return vram_header;
        }
    }
    return nullptr;
}

// UNSOLVED
TexturePackHeader *InternalLoadTexturePackHeaderChunks(bChunk *chunk) {}

// UNSOLVED
void InternalUnloadTexturePackHeaderChunks(bChunk *chunk) {
    TexturePackHeader *texture_pack_header = nullptr;
    TextureInfo *texture_info_table = nullptr;
    TextureInfoPlatInfo *plat_info_table = nullptr;
    int num_texture_info = 0;

    bChunk *current_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    while (current_chunk != last_chunk) {
        unsigned int current_chunk_id = current_chunk->GetID();

        if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO_HEADER) {
            texture_pack_header = reinterpret_cast<TexturePackHeader *>(current_chunk->GetData());
        } else if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO_KEYS) {
        } else if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO_ENTRIES) {
        } else if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO_TEXTURES) {
            texture_info_table = reinterpret_cast<TextureInfo *>(current_chunk->GetData());
            num_texture_info = current_chunk->GetSize() / sizeof(TextureInfo);
        } else if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO_COMPS) {
            plat_info_table = reinterpret_cast<TextureInfoPlatInfo *>(current_chunk->GetData());
        }

        current_chunk = current_chunk->GetNext();
    }
    if (texture_pack_header != nullptr) { // fake
        texture_pack_header->pTexturePack->UnAssignTextureData(0, texture_pack_header->pTexturePack->GetTextureDataSize());
    }
    if (texture_info_table != nullptr) {
        texture_pack_header->pTexturePack->UnattachTextureTable(texture_info_table, plat_info_table, num_texture_info);
    }
    if (texture_pack_header->pTexturePack != nullptr) {
        delete texture_pack_header->pTexturePack;
    }
    texture_pack_header->pTexturePack = nullptr;
}

TextureVRAMDataHeader *InternalLoadVRAMDataChunks(bChunk *chunk) {
    bChunk *vram_header_chunk = chunk->GetFirstChunk();
    bChunk *null_chunk = vram_header_chunk->GetNext();
    bChunk *vram_data_chunk = null_chunk->GetNext();

    TextureVRAMDataHeader *vram_header = reinterpret_cast<TextureVRAMDataHeader *>(vram_header_chunk->GetData());
    if (!vram_header->EndianSwapped) {
        vram_header->EndianSwap();
        vram_header->EndianSwapped = true;
    }
    vram_header->VRAMDataChunk = vram_data_chunk;
    TextureVRAMDataHeaderList.AddTail(vram_header);

    return vram_header;
}

int LoaderTexturePack(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TEXTURE_PACK) {
        TexturePackHeader *texture_pack_header = nullptr;
        char *texture_data;    // UNUSED
        int texture_data_size; // UNUSED
        bChunk *current_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();

        while (current_chunk != last_chunk) {
            unsigned int current_chunk_id = current_chunk->GetID();
            if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO) {
                texture_pack_header = InternalLoadTexturePackHeaderChunks(current_chunk);
                PrevLoadedTexturePack = texture_pack_header->pTexturePack;
            } else if (current_chunk_id == BCHUNK_TEXTURE_PACK_DATA) {
                InternalLoadVRAMDataChunks(current_chunk);
            }
            current_chunk = current_chunk->GetNext();
        }
        TextureVRAMDataHeader *vram_header = FindVRAMData(texture_pack_header->FilenameHash);
        if (vram_header != nullptr) {
            TexturePack *texture_pack = texture_pack_header->pTexturePack;
            bChunk *vram_data_chunk = vram_header->VRAMDataChunk;
            char *texture_data = vram_data_chunk->GetAlignedData(128);
            int texture_data_size = vram_data_chunk->GetAlignedSize(128);
            texture_pack->AssignTextureData(texture_data, 0, texture_data_size);
            texture_pack->SetTextureDataSize(texture_data_size);
            vram_header->Remove();
        }
        return 1;
    } else if (chunk->GetID() == BCHUNK_TEXTURE_PACK_DATA) {
        InternalLoadVRAMDataChunks(chunk);
        return 1;
    } else if (chunk->GetID() == BCHUNK_TEXTURE_ANIM_PACK) {
        bChunk *chunk_header = chunk->GetFirstChunk();
        bChunk *chunk_anims = chunk_header->GetNext();
        bChunk *chunk_entries = chunk_anims->GetNext();
        TextureAnimPackHeader *header = reinterpret_cast<TextureAnimPackHeader *>(chunk_header->GetData());
        if (!header->EndianSwapped) {
            header->EndianSwap();
        }
        if (header->Version == 1) {
            TextureAnim *anims = reinterpret_cast<TextureAnim *>(chunk_anims->GetData());
            TextureAnimEntry *entries = reinterpret_cast<TextureAnimEntry *>(chunk_entries->GetData());
            int num_anims = chunk_anims->GetSize() / sizeof(*anims);
            int num_anim_entries = chunk_entries->GetSize() / sizeof(*entries);
            if (!header->EndianSwapped) {
                for (int i = 0; i < num_anims; i++) {
                    anims[i].EndianSwap();
                }
                for (int j = 0; j < num_anim_entries; j++) {
                    entries[j].EndianSwap();
                }
            }
            header->pTextureAnimPack = new TextureAnimPack(PrevLoadedTexturePack, anims, entries, num_anims, num_anim_entries);
            header->pTextureAnimPack->InitAnims();
        } else {
            header->pTextureAnimPack = nullptr;
        }
        if (header != nullptr) {
            header->EndianSwapped = true;
        }
        return 1;
    }
    return 0;
}

int UnloaderTexturePack(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TEXTURE_PACK) {
        TexturePackHeader *texture_pack_header;
        bChunk *current_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();

        while (current_chunk != last_chunk) {
            unsigned int current_chunk_id = current_chunk->GetID();
            if (current_chunk_id == BCHUNK_TEXTURE_PACK_INFO) {
                InternalUnloadTexturePackHeaderChunks(current_chunk);
                PrevLoadedTexturePack = nullptr;
            }
            current_chunk = current_chunk->GetNext();
        }
        return 1;
    } else if (chunk->GetID() == BCHUNK_TEXTURE_PACK_DATA) {
        return 1;
    } else if (chunk->GetID() == BCHUNK_TEXTURE_ANIM_PACK) {
        bChunk *chunk_header = chunk->GetFirstChunk();
        TextureAnimPackHeader *header = reinterpret_cast<TextureAnimPackHeader *>(chunk_header->GetData());
        if (header->Version == 1) {
            delete header->pTextureAnimPack;
        }
        return 1;
    }
    return 0;
}

TexturePack::TexturePack(TexturePackHeader *pack_header, int num_textures, const char *pack_name, const char *filename) {
    if (pack_name == nullptr && pack_header != nullptr) {
        pack_name = pack_header->Name;
    }
    if (filename == nullptr && pack_header != nullptr) {
        filename = pack_header->Filename;
    }
    if (pack_name == nullptr) {
        pack_name = "<null>";
    }
    if (filename == nullptr) {
        filename = "<null>";
    }
    this->Name = bAllocateSharedString(pack_name);
    this->Filename = bAllocateSharedString(filename);
    this->pPackHeader = pack_header;
    this->NameHash = bStringHash(this->Name);
    this->NumTextures = num_textures;
    this->TextureDataSize = 0;

    TexturePackList.AddTail(this);
}

TexturePack::~TexturePack() {
    this->Remove();
    bFreeSharedString(this->Name);
    bFreeSharedString(this->Filename);
}

void TexturePack::AttachTextureTable(TextureInfo *texture_info_table, TextureInfoPlatInfo *plat_info_table, int num_texture_info) {
    for (int n = 0; n < num_texture_info; n++) {
        TextureInfo *t = &texture_info_table[n];
        TextureInfoPlatInfo *plat_info = &plat_info_table[n];
        TextureIndexEntry *index_entry = &this->pPackHeader->TextureIndexEntryTable[n];

        this->AttachTextureInfo(t, plat_info, index_entry);
    }
}

void TexturePack::UnattachTextureTable(TextureInfo *texture_info_table, TextureInfoPlatInfo *plat_info_table, int num_texture_info) {
    for (int n = 0; n < num_texture_info; n++) {
        TextureInfo *t = &texture_info_table[n];
        TextureInfoPlatInfo *plat_info = &plat_info_table[n];
        TextureIndexEntry *index_entry = &this->pPackHeader->TextureIndexEntryTable[n];

        this->UnattachTextureInfo(t, plat_info, index_entry);
    }
}

void TexturePack::AttachTextureInfo(TextureInfo *texture_info, TextureInfoPlatInfo *plat_info, TextureIndexEntry *index_entry) {
    if (index_entry == nullptr) {
        index_entry = this->GetTextureIndexEntry(texture_info->NameHash);
    }
    if (index_entry != nullptr) {
        index_entry->pTextureInfo = texture_info;
        TextureLoadedTable.SetLoaded(texture_info->NameHash);
    }
    if (texture_info->ImageParentHash != 0) {
        TextureInfo *t = this->GetTexture(texture_info->ImageParentHash);
        texture_info->pImageParent = t;
    } else {
        texture_info->pImageParent = texture_info;
    }
    texture_info->pTexturePack = this;
    texture_info->ImageData = nullptr;
    texture_info->PaletteData = nullptr;
    texture_info->SetPlatInfo(plat_info);
    if (texture_info->NameHash == BinHash<'D', 'E', 'F', 'A', 'U', 'L', 'T', 'A', 'L', 'P', 'H', 'A'>::value) {
        DefaultTextureInfo = texture_info;
    }
    eDirtyTextures = 1;
}

void TexturePack::UnattachTextureInfo(TextureInfo *texture_info, TextureInfoPlatInfo *plat_info, TextureIndexEntry *index_entry) {
    if (index_entry == nullptr) {
        index_entry = this->GetTextureIndexEntry(texture_info->NameHash);
    }
    if (index_entry != nullptr) {
        index_entry->pTextureInfo = nullptr;
        TextureLoadedTable.SetUnloaded(texture_info->NameHash);
    }
    if (texture_info == DefaultTextureInfo) {
        DefaultTextureInfo = nullptr;
    }
}

void TexturePack::AssignTextureData(char *texture_data, int begin_pos, int num_bytes) {
    int num_assigned = 0;
    TextureInfo *assigned_texture_info = nullptr;
    for (int n = 0; n < this->NumTextures; n++) {
        TextureInfo *t = this->pPackHeader->TextureIndexEntryTable[n].pTextureInfo;
        if (t == nullptr) {
            continue;
        }
        bool initialized_something = false;
        if (t->ImageSize != 0 && (t->ImagePlacement >= begin_pos) && (t->ImagePlacement < begin_pos + num_bytes)) {
            initialized_something = true;
            t->ImageData = texture_data + (t->ImagePlacement - begin_pos);
        }
        if (t->PaletteSize != 0 && (t->PalettePlacement >= begin_pos) && (t->PalettePlacement < begin_pos + num_bytes)) {
            initialized_something = true;
            t->PaletteData = texture_data + (t->PalettePlacement - begin_pos);
        }
        if (initialized_something && t->ImageData != nullptr) {
            num_assigned++;
            t->Init();
            assigned_texture_info = t;
        }
    }
    if (num_assigned != 1) {
        assigned_texture_info = nullptr;
    }
    eNotifyTextureLoading(this, assigned_texture_info, true);
    eDirtyTextures = 1;
}

inline void ClearTextureInfoCache(unsigned int name_hash) {
    int cache_index = name_hash & 0xFF;
    TextureInfoCache[cache_index] = nullptr;
}

inline TextureInfo *GetTextureInfoCache(unsigned int name_hash) {
    int cache_index = name_hash & 0xFF;
    TextureInfo *t = TextureInfoCache[cache_index];
    if ((t != nullptr) && (t->NameHash == name_hash)) {
        return t;
    }
    return nullptr;
}

inline void SetTextureInfoCache(TextureInfo *t) {
    int cache_index = t->NameHash & 0xFF;
    TextureInfoCache[cache_index] = t;
    TextureInfoCacheSafety[cache_index] = t->NameHash;
}

void TexturePack::UnAssignTextureData(int begin_pos, int num_bytes) {
    int num_unassigned = 0;
    TextureInfo *unassigned_texture_info = nullptr;
    for (int n = 0; n < this->NumTextures; n++) {
        TextureInfo *t = this->pPackHeader->TextureIndexEntryTable[n].pTextureInfo;
        if (t == nullptr) {
            continue;
        }
        bool unassigned_something = false;
        if (t->ImageSize != 0 && (t->ImagePlacement >= begin_pos) && (t->ImagePlacement < begin_pos + num_bytes)) {
            unassigned_something = true;
            t->ImageData = nullptr;
        }
        if (t->PaletteSize != 0 && (t->PalettePlacement >= begin_pos) && (t->PalettePlacement < begin_pos + num_bytes)) {
            unassigned_something = true;
            t->PaletteData = nullptr;
        }
        if (t->ImageData == nullptr && unassigned_something) {
            t->Close();
            unsigned int texture_name_hash = t->NameHash;
            ClearTextureInfoCache(texture_name_hash);
            if (t->NameHash + 0xacf47d50 < 4) {
                FlushFromSkinCompositeCache(t->NameHash);
            }
            num_unassigned++;
            unassigned_texture_info = t;
        }
    }
    if (num_unassigned != 1) {
        unassigned_texture_info = nullptr;
    }
    eNotifyTextureLoading(this, unassigned_texture_info, false);
    eDirtyTextures = 1;
}

TextureIndexEntry *TexturePack::GetTextureIndexEntry(unsigned int name_hash) {

    if ((this->pPackHeader != nullptr) && (this->pPackHeader->TextureIndexEntryTable != nullptr)) {
        return reinterpret_cast<TextureIndexEntry *>(
            ScanHashTableKey32(name_hash, this->pPackHeader->TextureIndexEntryTable, this->NumTextures, 0, 8));
    }
    return nullptr;
}

TextureInfo *TexturePack::GetLoadedTexture(unsigned int name_hash) {
    bool bVar1;

    if (TextureLoadedTable.IsLoaded(name_hash)) {
        return nullptr;
    }
    TextureIndexEntry *index_entry = this->GetTextureIndexEntry(name_hash);
    if (index_entry != nullptr && index_entry->pTextureInfo != nullptr) {
        // TODO
        if ((index_entry->pTextureInfo->ImageData == nullptr) ||
            (bVar1 = true, index_entry->pTextureInfo->PaletteSize != 0 && (index_entry->pTextureInfo->PaletteData == nullptr))) {
            bVar1 = false;
        }
        if (bVar1) {
            return index_entry->pTextureInfo;
        }
    }
    return nullptr;
}

TextureInfo *TexturePack::GetTexture(unsigned int name_hash) {
    TextureIndexEntry *index_entry = this->GetTextureIndexEntry(name_hash);
    if (index_entry != nullptr) {
        return index_entry->pTextureInfo;
    }
    return nullptr;
}

TextureInfo *eCreateTextureInfo() {
    TextureInfo *texture_info = new TextureInfo();
    TextureInfoPlatInfo *plat_info = new TextureInfoPlatInfo();
    texture_info->SetPlatInfo(plat_info);
    return texture_info;
}

void eDestroyTextureInfo(TextureInfo *texture_info) {
    TextureInfoPlatInfo *plat_info = texture_info->GetPlatInfo();
    if (plat_info != nullptr) {
        delete plat_info;
    }
    if (texture_info != nullptr) {
        delete texture_info;
    }
}

TextureAnimPack::TextureAnimPack(TexturePack *parent_texture_pack, TextureAnim *texture_anim_table, TextureAnimEntry *texture_anim_entry_table,
                                 int num_anims, int num_anim_entries) {
    this->ParentTexturePack = parent_texture_pack;
    this->TextureAnimTable = texture_anim_table;
    this->TextureAnimEntryTable = texture_anim_entry_table;
    this->NumTextureAnims = num_anims;
    this->NumTextureAnimEntries = num_anim_entries;

    TextureAnimEntry *anim_entry = texture_anim_entry_table;
    for (int i = 0; i < num_anims; i++) {
        TextureAnim *texture_anim = &this->TextureAnimTable[i];
        texture_anim->TextureAnimTable = anim_entry;
        anim_entry += texture_anim->NumFrames;
    }
    TextureAnimPackList.AddTail(this);
}

TextureAnimPack::~TextureAnimPack() {
    for (int i = 0; i < this->NumTextureAnims; i++) {
        TextureAnim *texture_anim = &this->TextureAnimTable[i];
        TextureAnimEntry *base_entry = texture_anim->TextureAnimTable;
        int num_frames = texture_anim->NumFrames;

        for (int j = 0; j < num_frames; j++) {
            TextureAnimEntry *frame_entry = &base_entry[j];
            if (frame_entry->pTextureInfo != nullptr) {
                frame_entry->pTextureInfo->ReleaseAnimData(frame_entry->pPlatAnimData);
            }
        }
    }
    this->Remove();
}

void TextureAnimPack::InitAnims() {
    TextureAnim *texture_anim = this->TextureAnimTable;
    TextureAnimEntry *anim_entry;
    int num_anims = this->NumTextureAnims;

    for (int i = 0; i < num_anims; i++) {
        TextureAnimEntry *base_entry = texture_anim->TextureAnimTable;
        int num_frames = texture_anim->NumFrames;
        BOOL valid = true;
        for (int j = 0; j < num_frames; j++) {
            TextureAnimEntry *frame_entry = &base_entry[j];
            TextureInfo *texture_info = this->ParentTexturePack->GetLoadedTexture(frame_entry->NameHash);
            if (texture_info != nullptr) {
                frame_entry->pTextureInfo = texture_info;
                frame_entry->pPlatAnimData = texture_info->CreateAnimData();
            } else {
                frame_entry->pTextureInfo = nullptr;
                frame_entry->pPlatAnimData = nullptr;
                valid = false;
            }
        }
        texture_anim->Valid = valid;
        texture_anim->CurrentFrame = 0;
        texture_anim++;
    }
}

// UNSOLVED
void UpdateTextureAnimations() {
    // ProfileNode profile_node;

    if ((TheGameFlowManager.CurrentGameFlowState != GAMEFLOW_STATE_IN_FRONTEND) && (WorldTimeElapsed == 0.0f)) {
        return;
    }

    float world_time = WorldTimeFrames * (1.0f / REFRESH_RATE); // f29
    float real_time = RealTimeFrames * (1.0f / REFRESH_RATE);   // f30
    int tweak_fps;

    for (TextureAnimPack *anim_pack /* r26 */ = TextureAnimPackList.GetHead(); anim_pack != TextureAnimPackList.EndOfList();
         anim_pack = anim_pack->GetNext()) {
        TextureAnim *texture_anim = anim_pack->TextureAnimTable; // r8
        int num_anims = anim_pack->NumTextureAnims;              // r23

        for (int i /* r9 */ = 0; i < num_anims; i++) {
            float current_time;                       // f13
            int num_frames = texture_anim->NumFrames; // r30
            int fps = texture_anim->FramesPerSecond;  // r0
            int current_frame;                        // r28

            if (!texture_anim->Valid) {
                continue;
            }
            TextureAnimEntry *base_entry = texture_anim->TextureAnimTable; // r27
            TextureAnimEntry *current_entry = &base_entry[i];              // r31
            current_time = texture_anim->TimeBase == 1 ? real_time : world_time;
            current_frame = fps * current_time;
            current_frame -= (current_frame / num_frames) * num_frames;

            if (current_frame == texture_anim->CurrentFrame) {
                continue;
            }
            texture_anim->CurrentFrame = current_frame;
            for (int j /* r29 */ = 0; j < num_frames; j++) {
                void *new_anim_data = current_entry[j].pPlatAnimData;
                TextureInfo *texture_info = current_entry[j].pTextureInfo;
                texture_info->SetAnimData(new_anim_data);
            }
        }
    }

    // TODO ProfileNode and IsInFrontEnd stuff
}

void TextureLoadedStreamingEntryCallback(bChunk *chunk, eStreamingEntry *streaming_entry, eStreamingPack *streaming_pack) {
    // ProfileNode profile_node;
    // TODO ProfileNode

    TexturePackHeader *texture_pack_header = streaming_pack->pTexturePackHeader;
    TexturePack *texture_pack = texture_pack_header->pTexturePack;
    unsigned char *pdata = reinterpret_cast<unsigned char *>(chunk);
    TextureInfoPlatInfo *plat_info =
        reinterpret_cast<TextureInfoPlatInfo *>(pdata + (streaming_entry->UncompressedSize - sizeof(TextureInfoPlatInfo)));
    TextureInfo *texture_info =
        reinterpret_cast<TextureInfo *>(pdata + (streaming_entry->UncompressedSize - sizeof(TextureInfoPlatInfo) - sizeof(TextureInfo)));
    texture_info->EndianSwap();
    texture_pack->AttachTextureInfo(texture_info, plat_info, nullptr);

    int data_start = texture_info->ImagePlacement;
    int data_end;
    if (texture_info->PalettePlacement != -1) {
        data_end = texture_info->PalettePlacement + texture_info->PaletteSize;
    } else {
        data_end = data_start + texture_info->ImageSize;
    }
    int data_size = data_end - data_start;
    texture_pack->AssignTextureData(reinterpret_cast<char *>(pdata), data_start, data_size);
}

void TextureUnLoadedStreamingEntryCallback(bChunk *chunk, eStreamingEntry *streaming_entry, eStreamingPack *streaming_pack) {
    // ProfileNode profile_node;
    // TODO ProfileNode

    TexturePackHeader *texture_pack_header = streaming_pack->pTexturePackHeader;
    TexturePack *texture_pack = texture_pack_header->pTexturePack;
    unsigned char *pdata = reinterpret_cast<unsigned char *>(chunk);
    TextureInfoPlatInfo *plat_info =
        reinterpret_cast<TextureInfoPlatInfo *>(pdata + (streaming_entry->UncompressedSize - sizeof(TextureInfoPlatInfo)));
    TextureInfo *texture_info =
        reinterpret_cast<TextureInfo *>(pdata + (streaming_entry->UncompressedSize - sizeof(TextureInfoPlatInfo) - sizeof(TextureInfo)));

    int data_start = texture_info->ImagePlacement;
    int data_end;
    if (texture_info->PalettePlacement != -1) {
        data_end = texture_info->PalettePlacement + texture_info->PaletteSize;
    } else {
        data_end = data_start + texture_info->ImageSize;
    }
    int data_size = data_end - data_start;
    eFixupReplacementTexturesAfterUnloading(texture_info);
    texture_pack->UnAssignTextureData(data_start, data_size);
    texture_pack->UnattachTextureInfo(texture_info, plat_info, nullptr);
    texture_info->EndianSwap();
}

void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num) {
    StreamingTexturePackLoader.LoadStreamingEntry(name_hash_table, num_hashes, callback, param0, memory_pool_num);
}

void eUnloadStreamingTexture(unsigned int *name_hash_table, int num_hashes) {
    StreamingTexturePackLoader.UnloadStreamingEntry(name_hash_table, num_hashes);
}

void eWaitForStreamingTexturePackLoading(const char *filename) {
    StreamingTexturePackLoader.WaitForLoadingToFinish(filename);
}

void eUnloadAllStreamingTextures(const char *filename) {
    StreamingTexturePackLoader.UnloadAllStreamingEntries(filename);
}

int eIsStreamingTexturePackLoaded(const char *filename) {
    return StreamingTexturePackLoader.GetLoadedStreamingPack(filename) != nullptr;
}

int eLoadStreamingTexturePack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num) {
    eStreamingPack *streaming_pack;

    streaming_pack = StreamingTexturePackLoader.CreateStreamingPack(filename, callback_function, callback_param, memory_pool_num);
    return streaming_pack != nullptr;
}

void TextureLoadingStreamingPackPhase1(eStreamingPackHeaderLoadingInfoPhase1 *loading_info) {
    bChunk *list_chunk;
    bChunk *null_chunk;
    bChunk *header_chunk;
    int header_chunks_size;

    list_chunk = loading_info->TempHeaderChunks;
    EndianSwapChunkHeader(list_chunk);
    header_chunk = list_chunk->GetFirstChunk();
    EndianSwapChunkHeader(header_chunk);
    null_chunk = header_chunk->GetNext();
    EndianSwapChunkHeader(null_chunk);
    header_chunks_size = null_chunk->GetSize();
    loading_info->NextLoadAmount = header_chunks_size + 16;
    loading_info->NextLoadPosition = (uintptr_t)null_chunk - (uintptr_t)list_chunk;
}

void TextureLoadingStreamingPackPhase2(eStreamingPackHeaderLoadingInfoPhase2 *loading_info) {
    bChunk *header_chunks = loading_info->HeaderChunks;
    if (loading_info->HeaderChunksMemCopied == 0) {
        EndianSwapChunkHeader(header_chunks);
    }
    EndianSwapChunkHeadersRecursive(header_chunks->GetFirstChunk(), header_chunks->GetLastChunk());

    TexturePackHeader *texture_pack_header = InternalLoadTexturePackHeaderChunks(header_chunks);
    loading_info->StreamingEntryTable = texture_pack_header->TextureStreamEntryTable;
    loading_info->StreamingEntryNumEntries = texture_pack_header->pTexturePack->GetNumTextures();
    loading_info->pTexturePackHeader = texture_pack_header;
}

void eUnloadStreamingTexturePack(const char *filename) {
    StreamingTexturePackLoader.DeleteStreamingPack(filename);
}

void TextureUnloadingStreamingPack(eStreamingPack *streaming_pack) {
    InternalUnloadTexturePackHeaderChunks(streaming_pack->HeaderChunks);
}

float TextureInfo::GetScroll(float time, float speed, int scroll_type, float time_step) {
    float t;

    if (scroll_type == TEXSCROLL_NONE) {
        return 0.0f;
    }
    if (scroll_type == TEXSCROLL_SNAP) {
        time /= time_step;
        t = (int)time;
    } else {
        t = time;
    }
    float p = (int)(t * speed);
    return t * speed - p;
}

// STRIPPED
// void TextureInfo::Print() {}

void MaybePrintUnusedTextures() {}

// UNSOLVED https://decomp.me/scratch/3GvPG
TextureInfo *GetTextureInfo(unsigned int name_hash /* r30 */, BOOL return_default_texture_if_not_found /* r27 */,
                            BOOL include_unloaded_textures /* r5 */) {
    if (name_hash != 0) {
        TextureInfo *texture_info = GetTextureInfoCache(name_hash);
        if (texture_info != nullptr) {
            return texture_info;
        }
        for (TexturePack *p = TexturePackList.GetHead(); p != TexturePackList.EndOfList(); p = p->GetNext()) {
            if (TextureLoadedTable.IsLoaded(name_hash)) {
                continue; // wrong
            }
            TextureInfo *texture_info;
            if (include_unloaded_textures) {
                texture_info = p->GetTexture(name_hash);
            } else {
                texture_info = p->GetLoadedTexture(name_hash);
            }
            if (texture_info != nullptr) {
                TexturePackList.Remove(p);
                TexturePackList.AddHead(p);
                texture_info->UsedFlag = true;
                SetTextureInfoCache(texture_info);
                return texture_info;
            }
        }
        {
            TextureInfo *texture_info = eGetRenderTargetTextureInfo(name_hash);
            if (texture_info != nullptr) {
                return texture_info;
            }
            texture_info = eGetOtherEcstacyTexture(name_hash);
            if (texture_info != nullptr) {
                return texture_info;
            }
        }
    }
    if (return_default_texture_if_not_found) {
        TextureInfo *texture_info = DefaultTextureInfo;
        if (texture_info == nullptr) {
            bBreak();
            return nullptr;
        }
        return texture_info;
    }
    return nullptr;
}

TextureInfo *FixupTextureInfo(TextureInfo *texture_info, unsigned int name_hash, TexturePack *texture_pack, bool loading) {
    if (loading) {
        if (texture_info == DefaultTextureInfo) {
            TextureInfo *info = texture_pack->GetLoadedTexture(name_hash);
            if (info != nullptr) {
                info->UsedFlag = true;
                return info;
            }
        }
    } else if (texture_info->pTexturePack == texture_pack) {
        TextureInfo *info = texture_pack->GetLoadedTexture(name_hash);
        if (info == nullptr) {
            info = GetTextureInfo(name_hash, false, false);
            if (info != nullptr) {
                return info;
            }
            info = DefaultTextureInfo;
            if (info == nullptr) {
                bBreak();
            }
            return info;
        }
    }
    return texture_info;
}

TextureInfo *FixupTextureInfoNull(TextureInfo *texture_info, unsigned int name_hash, TexturePack *texture_pack, bool loading) {
    if (loading) {
        if (texture_info == nullptr) {
            TextureInfo *info = texture_pack->GetLoadedTexture(name_hash);
            if (info != nullptr) {
                info->UsedFlag = true;
                return info;
            }
        }
    } else if (texture_info != nullptr && texture_info->pTexturePack == texture_pack) {
        TextureInfo *info = texture_pack->GetLoadedTexture(name_hash);
        if (info == nullptr) {
            return GetTextureInfo(name_hash, false, false);
        }
    }
    return texture_info;
}
