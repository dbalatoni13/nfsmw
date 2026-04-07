#ifndef ECSTASY_ESTREAMING_PACK_H
#define ECSTASY_ESTREAMING_PACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>
#include <cstddef>

class eStreamingEntry {
    // total size: 0x18
  public:
    uint32 NameHash;        // offset 0x0, size 0x4
    uint32 ChunkByteOffset; // offset 0x4, size 0x4
    int32 ChunkByteSize;    // offset 0x8, size 0x4
    int32 UncompressedSize; // offset 0xC, size 0x4
    uint8 UserFlags;        // offset 0x10, size 0x1
    uint8 Flags;            // offset 0x11, size 0x1
    uint16 RefCount;        // offset 0x12, size 0x2
    uint8 *ChunkData;       // offset 0x14, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->ChunkByteOffset);
        bPlatEndianSwap(&this->ChunkByteSize);
        bPlatEndianSwap(&this->UncompressedSize);
    }
};

class eStreamingPackHeaderLoadingInfo {
    // total size: 0x10
  public:
    int32 HeaderChunksSize;              // offset 0x0, size 0x4
    int32 HeaderChunksMemCopied;         // offset 0x4, size 0x4
    void (*LoadingDoneCallback)(void *); // offset 0x8, size 0x4
    void *LoadingDoneCallbackParam;      // offset 0xC, size 0x4
};

extern SlotPool *eStreamingPackSlotPool;

class eStreamingPack : public bTNode<eStreamingPack> {
    // total size: 0x44
  public:
    const char *Filename;                         // offset 0x8, size 0x4
    int16 HeaderMemoryPoolNum;                    // offset 0xC, size 0x2
    int8 DisabledFlag;                            // offset 0xE, size 0x1
    eStreamingPackHeaderLoadingInfo *LoadingInfo; // offset 0x10, size 0x4
    int32 HeaderLoaded;                           // offset 0x14, size 0x4
    int32 HeaderSize;                             // offset 0x18, size 0x4
    bChunk *HeaderChunks;                         // offset 0x1C, size 0x4
    ResourceFile *pResourceFile;                  // offset 0x20, size 0x4
    eStreamingEntry *StreamingEntryTable;         // offset 0x24, size 0x4
    int32 StreamingEntryNumEntries;               // offset 0x28, size 0x4
    int32 NumLoadsPending;                        // offset 0x2C, size 0x4
    int32 NumLoadedStreamingEntries;              // offset 0x30, size 0x4
    int32 NumLoadedBytes;                         // offset 0x34, size 0x4
    int32 RefCount;                               // offset 0x38, size 0x4
    struct eSolidListHeader *SolidListHeader;     // offset 0x3C, size 0x4
    struct TexturePackHeader *pTexturePackHeader; // offset 0x40, size 0x4

    void *operator new(size_t size) {
        void *streaming_pack = bOMalloc(eStreamingPackSlotPool);
        bMemSet(streaming_pack, 0x0, 0x44);
        return streaming_pack;
    }
    void operator delete(void *ptr) {
        bFree(eStreamingPackSlotPool, ptr);
    }

    // STRIPPED
    void InitForHibernation();
    // STRIPPED
    bool IsHeaderInMemoryPool(void *mem, int mem_size);
    int GetHeaderMemoryEntries(void **memory_entries, int num_memory_entries);

    eStreamingPack() {};

    void RegisterLoadStreamingEntry(eStreamingEntry *entry) {}

    void RegisterUnloadStreamingEntry(eStreamingEntry *entry) {
        entry->ChunkData = nullptr;
        this->NumLoadedStreamingEntries--;
        this->NumLoadedBytes -= entry->ChunkByteSize;
    }
};

class eStreamingPackHeaderLoadingInfoPhase1 {
    // total size: 0x10
  public:
    const char *Filename;     // offset 0x0, size 0x4
    bChunk *TempHeaderChunks; // offset 0x4, size 0x4
    int32 NextLoadPosition;   // offset 0x8, size 0x4
    int32 NextLoadAmount;     // offset 0xC, size 0x4
};

class eStreamingPackHeaderLoadingInfoPhase2 {
    // total size: 0x28
  public:
    const char *Filename;                     // offset 0x0, size 0x4
    bChunk *HeaderChunks;                     // offset 0x4, size 0x4
    int32 HeaderChunksMemCopied;              // offset 0x8, size 0x4
    eStreamingPack *pStreamingPack;           // offset 0xC, size 0x4
    eStreamingEntry *StreamingEntryTable;     // offset 0x10, size 0x4
    int32 StreamingEntryNumEntries;           // offset 0x14, size 0x4
    struct eSolidListHeader *SolidListHeader; // offset 0x18, size 0x4
    TexturePackHeader *pTexturePackHeader;    // offset 0x1C, size 0x4
    int32 LoadResourceFilePosition;           // offset 0x20, size 0x4
    int32 LoadResourceFileAmount;             // offset 0x24, size 0x4
};

class eStreamPackLoader {
    // total size: 0x38
    int32 RequiredChunkAlignment;                                                          // offset 0x0, size 0x4
    void (*LoadedStreamingEntryCallback)(bChunk *, eStreamingEntry *, eStreamingPack *);   // offset 0x4, size 0x4
    void (*UnloadedStreamingEntryCallback)(bChunk *, eStreamingEntry *, eStreamingPack *); // offset 0x8, size 0x4
    void (*LoadingHeaderPhase1Callback)(eStreamingPackHeaderLoadingInfoPhase1 *);          // offset 0xC, size 0x4
    void (*LoadingHeaderPhase2Callback)(eStreamingPackHeaderLoadingInfoPhase2 *);          // offset 0x10, size 0x4
    void (*UnloadingHeaderCallback)(eStreamingPack *);                                     // offset 0x14, size 0x4
    bTList<eStreamingPack> LoadedStreamingPackList;                                        // offset 0x18, size 0x8
  public:
    int32 PrintLoading;                      // offset 0x20, size 0x4
    int32 PrintInventory;                    // offset 0x24, size 0x4
    const char *(*DebugGetNameFunc)(uint32); // offset 0x28, size 0x4
    int32 NumLoadedStreamingPacks;           // offset 0x2C, size 0x4
    int32 NumLoadedStreamingEntries;         // offset 0x30, size 0x4
    int32 NumLoadedBytes;                    // offset 0x34, size 0x4

    eStreamPackLoader(int required_chunk_alignment, void (*loaded_streaming_entry_callback)(bChunk *, eStreamingEntry *, eStreamingPack *),
                      void (*unloaded_streaming_entry_callback)(bChunk *, eStreamingEntry *, eStreamingPack *),
                      void (*loading_header_phase1_callback)(eStreamingPackHeaderLoadingInfoPhase1 *),
                      void (*loading_header_phase2_callback)(eStreamingPackHeaderLoadingInfoPhase2 *),
                      void (*unloading_header_callback)(eStreamingPack *));
    int GetMemoryEntries(uint32 *name_hash_table, int num_hashes, void **memory_entries, int num_memory_entries);
    eStreamingPack *GetLoadedStreamingPack(const char *filename);
    eStreamingPack *GetLoadedStreamingPack(uint32 name_hash);
    eStreamingEntry *GetStreamingEntry(uint32 name_hash, eStreamingPack *streaming_pack);
    eStreamingEntry *GetStreamingEntry(uint32 name_hash);
    bChunk *GetAlignedChunkDataPtr(uint8 *chunk_data);
    struct eStreamingPackLoadTable *GetStreamPackLoadingTable();
    static void InternalLoadedStreamingEntryCallback(void *callback_param, int error_status, void *callback_param2);
    void InternalLoadStreamingEntry(eStreamingPackLoadTable *loading_table, struct eStreamingPack *streaming_pack,
                                    struct eStreamingEntry *streaming_entry);

    void LoadStreamingEntry(uint32 *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
    void LoadStreamingEntry(uint32 name_hash, void (*callback)(void *) /* r5 */, void *param0 /* r0 */, int memory_pool_num /* r8 */);
    int IsLoaded(uint32 name_hash);
    void UnloadStreamingEntry(uint32 name_hash, eStreamingPack *streaming_pack);
    void UnloadStreamingEntry(uint32 *name_hash_table, int num_hashes);
    void UnloadAllStreamingEntries(const char *filename);
    int IsLoading(const char *filename);
    int TestLoadStreamingEntry(uint32 *name_hash_table, int num_hashes, int memory_pool_num, bool error_if_out_in_main_pool);
    bool DefragmentAllocation(void *allocation);
    void WaitForLoadingToFinish(const char *filename);
    static void InternalLoadingHeaderPhase1Callback(void *callback_param, int error_status, void *callback_param2);
    static void InternalLoadingHeaderPhase2Callback(void *callback_param, int error_status, void *callback_param2);
    static void InternalLoadingHeaderPhase3Callback(void *callback_param);
    eStreamingPack *CreateStreamingPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num);

    int DeleteStreamingPack(const char *filename);

    void DisableStreamingPack(const char *filename) {}

    void EnableStreamingPack(const char *filename) {}

    void RegisterLoadStreamingEntry(eStreamingEntry *entry) {}

    void RegisterUnloadStreamingEntry(eStreamingEntry *entry) {
        this->NumLoadedStreamingEntries--;
        this->NumLoadedBytes -= entry->ChunkByteSize;
    }

    void InternalUnloadStreamingEntry(eStreamingPack *streaming_pack, eStreamingEntry *streaming_entry);

    bTList<eStreamingPack> *GetLoadedStreamingPackList() {}
};

class eStreamingPackLoadTable {
    // total size: 0x10
  public:
    int8 MemoryPoolNum;                  // offset 0x0, size 0x1
    int8 Locked;                         // offset 0x1, size 0x1
    int16 NumLoadsPending;               // offset 0x2, size 0x2
    void (*Callback)(void *);            // offset 0x4, size 0x4
    void *Param;                         // offset 0x8, size 0x4
    eStreamPackLoader *StreamPackLoader; // offset 0xC, size 0x4
};

class eStreamPackLoadEntryInfo : public bTNode<eStreamPackLoadEntryInfo> {
    // total size: 0x14
  public:
    eStreamingPack *StreamingPack;   // offset 0x8, size 0x4
    eStreamingEntry *StreamingEntry; // offset 0xC, size 0x4
    uint32 FileOffset;               // offset 0x10, size 0x4
};

extern eStreamPackLoader StreamingTexturePackLoader;
extern eStreamPackLoader StreamingSolidPackLoader;

void InitStreamingPacks();
void *ScanHashTableKey32(uint32 key_value, void *table_start, int table_length, int entry_key_offset, int entry_size);

#endif
