#ifndef ECSTASY_ESTREAMING_PACK_H
#define ECSTASY_ESTREAMING_PACK_H

#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "dolphin/types.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include <cstddef>

struct eStreamingEntry {
    // total size: 0x18
    unsigned int NameHash;        // offset 0x0, size 0x4
    unsigned int ChunkByteOffset; // offset 0x4, size 0x4
    int ChunkByteSize;            // offset 0x8, size 0x4
    int UncompressedSize;         // offset 0xC, size 0x4
    unsigned char UserFlags;      // offset 0x10, size 0x1
    unsigned char Flags;          // offset 0x11, size 0x1
    unsigned short RefCount;      // offset 0x12, size 0x2
    unsigned char *ChunkData;     // offset 0x14, size 0x4

    void EndianSwap() {
        bPlatEndianSwap(&this->NameHash);
        bPlatEndianSwap(&this->ChunkByteOffset);
        bPlatEndianSwap(&this->ChunkByteSize);
        bPlatEndianSwap(&this->UncompressedSize);
    }
};

struct eStreamingPackHeaderLoadingInfo {
    // total size: 0x10
    int HeaderChunksSize;                // offset 0x0, size 0x4
    int HeaderChunksMemCopied;           // offset 0x4, size 0x4
    void (*LoadingDoneCallback)(void *); // offset 0x8, size 0x4
    void *LoadingDoneCallbackParam;      // offset 0xC, size 0x4
};

extern SlotPool *eStreamingPackSlotPool;

struct eStreamingPack : public bTNode<eStreamingPack> {
    // total size: 0x44
    const char *Filename;                         // offset 0x8, size 0x4
    short HeaderMemoryPoolNum;                    // offset 0xC, size 0x2
    char DisabledFlag;                            // offset 0xE, size 0x1
    eStreamingPackHeaderLoadingInfo *LoadingInfo; // offset 0x10, size 0x4
    int HeaderLoaded;                             // offset 0x14, size 0x4
    int HeaderSize;                               // offset 0x18, size 0x4
    bChunk *HeaderChunks;                         // offset 0x1C, size 0x4
    ResourceFile *pResourceFile;                  // offset 0x20, size 0x4
    eStreamingEntry *StreamingEntryTable;         // offset 0x24, size 0x4
    int StreamingEntryNumEntries;                 // offset 0x28, size 0x4
    int NumLoadsPending;                          // offset 0x2C, size 0x4
    int NumLoadedStreamingEntries;                // offset 0x30, size 0x4
    int NumLoadedBytes;                           // offset 0x34, size 0x4
    int RefCount;                                 // offset 0x38, size 0x4
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
    bool IsHeaderInMemoryPool();
    int GetHeaderMemoryEntries(void **memory_entries, int num_memory_entries);

    eStreamingPack() {};

    void RegisterLoadStreamingEntry(eStreamingEntry *entry) {}

    void RegisterUnloadStreamingEntry(eStreamingEntry *entry) {
        entry->ChunkData = nullptr;
        this->NumLoadedStreamingEntries--;
        this->NumLoadedBytes -= entry->ChunkByteSize;
    }
};

struct eStreamingPackHeaderLoadingInfoPhase1 {
    // total size: 0x10
    const char *Filename;     // offset 0x0, size 0x4
    bChunk *TempHeaderChunks; // offset 0x4, size 0x4
    int NextLoadPosition;     // offset 0x8, size 0x4
    int NextLoadAmount;       // offset 0xC, size 0x4
};

struct eStreamingPackHeaderLoadingInfoPhase2 {
    // total size: 0x28
    const char *Filename;                     // offset 0x0, size 0x4
    bChunk *HeaderChunks;                     // offset 0x4, size 0x4
    int HeaderChunksMemCopied;                // offset 0x8, size 0x4
    eStreamingPack *pStreamingPack;           // offset 0xC, size 0x4
    eStreamingEntry *StreamingEntryTable;     // offset 0x10, size 0x4
    int StreamingEntryNumEntries;             // offset 0x14, size 0x4
    struct eSolidListHeader *SolidListHeader; // offset 0x18, size 0x4
    TexturePackHeader *pTexturePackHeader;    // offset 0x1C, size 0x4
    int LoadResourceFilePosition;             // offset 0x20, size 0x4
    int LoadResourceFileAmount;               // offset 0x24, size 0x4
};

struct eStreamPackLoader {
    // total size: 0x38
    int RequiredChunkAlignment;                                                            // offset 0x0, size 0x4
    void (*LoadedStreamingEntryCallback)(bChunk *, eStreamingEntry *, eStreamingPack *);   // offset 0x4, size 0x4
    void (*UnloadedStreamingEntryCallback)(bChunk *, eStreamingEntry *, eStreamingPack *); // offset 0x8, size 0x4
    void (*LoadingHeaderPhase1Callback)(eStreamingPackHeaderLoadingInfoPhase1 *);          // offset 0xC, size 0x4
    void (*LoadingHeaderPhase2Callback)(eStreamingPackHeaderLoadingInfoPhase2 *);          // offset 0x10, size 0x4
    void (*UnloadingHeaderCallback)(eStreamingPack *);                                     // offset 0x14, size 0x4
    bTList<eStreamingPack> LoadedStreamingPackList;                                        // offset 0x18, size 0x8
    int PrintLoading;                                                                      // offset 0x20, size 0x4
    int PrintInventory;                                                                    // offset 0x24, size 0x4
    const char *(*DebugGetNameFunc)(unsigned int);                                         // offset 0x28, size 0x4
    int NumLoadedStreamingPacks;                                                           // offset 0x2C, size 0x4
    int NumLoadedStreamingEntries;                                                         // offset 0x30, size 0x4
    int NumLoadedBytes;                                                                    // offset 0x34, size 0x4

    eStreamPackLoader(
        int required_chunk_alignment,
        void (* loaded_streaming_entry_callback)(bChunk *, eStreamingEntry *, eStreamingPack *),
        void (* unloaded_streaming_entry_callback)(bChunk *, eStreamingEntry *, eStreamingPack *),
        void (* loading_header_phase1_callback)(eStreamingPackHeaderLoadingInfoPhase1 *),
        void (* loading_header_phase2_callback)(eStreamingPackHeaderLoadingInfoPhase2 *),
        void (* unloading_header_callback)(eStreamingPack *)
    );
    int GetMemoryEntries(unsigned int * name_hash_table, int num_hashes, void **memory_entries, int num_memory_entries);
    eStreamingPack *GetLoadedStreamingPack(const char *filename);
    eStreamingPack *GetLoadedStreamingPack(unsigned int name_hash);
    eStreamingEntry *GetStreamingEntry(unsigned int name_hash, eStreamingPack *streaming_pack);
    eStreamingEntry *GetStreamingEntry(unsigned int name_hash);
    bChunk *GetAlignedChunkDataPtr(unsigned char *chunk_data);
    struct eStreamingPackLoadTable *GetStreamPackLoadingTable();
    static void InternalLoadedStreamingEntryCallback(void *callback_param, int error_status, void *callback_param2);
    void InternalLoadStreamingEntry(eStreamingPackLoadTable *loading_table, struct eStreamingPack *streaming_pack, struct eStreamingEntry *streaming_entry);

    void LoadStreamingEntry(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
    void LoadStreamingEntry(unsigned int name_hash, void (* callback)(void *) /* r5 */, void * param0 /* r0 */, int memory_pool_num /* r8 */);
    int IsLoaded(unsigned int name_hash);
    void UnloadStreamingEntry(unsigned int name_hash, eStreamingPack *streaming_pack);
    void UnloadStreamingEntry(unsigned int *name_hash_table, int num_hashes);
    void UnloadAllStreamingEntries(const char *filename);
    int IsLoading(const char *filename);
    int TestLoadStreamingEntry(unsigned int *name_hash_table, int num_hashes, int memory_pool_num, bool error_if_out_in_main_pool);
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

struct eStreamingPackLoadTable {
    // total size: 0x10
    char MemoryPoolNum;                  // offset 0x0, size 0x1
    char Locked;                         // offset 0x1, size 0x1
    short NumLoadsPending;               // offset 0x2, size 0x2
    void (*Callback)(void *);            // offset 0x4, size 0x4
    void *Param;                         // offset 0x8, size 0x4
    eStreamPackLoader *StreamPackLoader; // offset 0xC, size 0x4
};

struct eStreamPackLoadEntryInfo : public bTNode<eStreamPackLoadEntryInfo> {
    // total size: 0x14
    eStreamingPack * StreamingPack;     // offset 0x8, size 0x4
    eStreamingEntry * StreamingEntry;   // offset 0xC, size 0x4
    unsigned int FileOffset;            // offset 0x10, size 0x4
};

extern eStreamPackLoader StreamingTexturePackLoader;
extern eStreamPackLoader StreamingSolidPackLoader;

void InitStreamingPacks();
void *ScanHashTableKey32(unsigned int key_value, void *table_start, int table_length, int entry_key_offset, int entry_size);

#endif
