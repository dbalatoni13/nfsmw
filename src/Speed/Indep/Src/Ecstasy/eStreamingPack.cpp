#include "./eStreamingPack.hpp"
#include "./EcstasyE.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include <types.h>

int AllowCompressedStreamingTexturesInThisPoolNum; // size: 0x4, address: 0x8041A5E4
SlotPool *eStreamingPackSlotPool;
eStreamingPackLoadTable QueuedLoadingTables[64]; // size: 0x400, address: 0x8046A770

void InitStreamingPacks() {
    bMemSet(QueuedLoadingTables, 0x0, 0x400);

    const char *test = "eStreamingPackSlotPool";
    eStreamingPackSlotPool = bNewSlotPool(0x44, 0x40, test, GetVirtualMemoryAllocParams());
}

// STRIPPED
void CloseStreamingPacks() {}

void PrintStreamingPackMemoryWarning(const char *malloc_name /* r3 */, int malloc_size /* r4 */, int memory_pool_num /* r5 */) {
    int language_pool_num; // probably used for EU release

    bGetMemoryPoolNum("LanguageMemoryPool");
}

void *ScanHashTableKey32(unsigned int key_value, void *table_start, int table_length, int entry_key_offset, int entry_size) {
    unsigned char *table_base;
    int low_index;
    int high_index;

    if (table_start == 0 || table_length <= 0 || entry_key_offset + 4U > entry_size) {
        return nullptr;
    }

    high_index = table_length - 1;
    low_index = 0;

    while (low_index <= high_index) {
        int mid_index;
        unsigned int mid_key_value;
        unsigned char *mid_entry;

        mid_key_value = low_index + high_index;
        mid_index = (int)(mid_key_value + (mid_key_value >> 0x1FU)) >> 1;
        table_base = &((unsigned char *)table_start)[mid_index * entry_size];
        if (key_value < *(unsigned int *)&table_base[entry_key_offset]) {
            high_index = mid_index - 1;
            continue;
        }
        if (key_value > *(unsigned int *)&table_base[entry_key_offset]) {
            low_index = mid_index + 1;
            continue;
        }
        if (mid_index > 0) {
            mid_entry = table_base - entry_size;
            if (*(unsigned int *)(mid_entry + entry_key_offset) == key_value) {
                while (true) {
                    table_base = mid_entry;
                    if (--mid_index > 0) {
                        mid_entry = table_base - entry_size;
                        if (*(unsigned int *)(mid_entry + entry_key_offset) != key_value) {
                            break;
                        }
                        continue;
                    }
                    break;
                }
            }
        }
        return table_base;
    }
    return nullptr;
}

// STRIPPED
void *ScanHashTableKey16(unsigned short key_value /* r8 */, void *table_start /* r4 */, int table_length /* r5 */, int entry_key_offset /* r6 */,
                         int entry_size /* r7 */);

void *ScanHashTableKey8(unsigned char key_value /* r8 */, void *table_start /* r4 */, int table_length /* r5 */, int entry_key_offset /* r6 */,
                        int entry_size /* r7 */) {
    unsigned char *table_base;
    int low_index;
    int high_index;

    if (table_start == 0 || table_length <= 0 || entry_key_offset + 1U > entry_size) {
        return nullptr;
    }

    high_index = table_length - 1;
    low_index = 0;

    while (low_index <= high_index) {
        int mid_index;
        unsigned int mid_key_value;
        unsigned char *mid_entry;

        mid_key_value = low_index + high_index;
        mid_index = (int)(mid_key_value + (mid_key_value >> 0x1FU)) >> 1;
        table_base = &((unsigned char *)table_start)[mid_index * entry_size];
        if (key_value < table_base[entry_key_offset]) {
            high_index = mid_index - 1;
            continue;
        }
        if (key_value > table_base[entry_key_offset]) {
            low_index = mid_index + 1;
            continue;
        }
        if (mid_index > 0) {
            mid_entry = table_base - entry_size;
            if (mid_entry[entry_key_offset] == key_value) {
                while (true) {
                    table_base = mid_entry;
                    if (--mid_index > 0) {
                        mid_entry = table_base - entry_size;
                        if (mid_entry[entry_key_offset] != key_value) {
                            break;
                        }
                        continue;
                    }
                    break;
                }
            }
        }
        return table_base;
    }
    return nullptr;
}

int eStreamingPack::GetHeaderMemoryEntries(void **memory_entries, int num_memory_entries) {
    if (this->HeaderChunks) {
        memory_entries[num_memory_entries++] = (void *)this->HeaderChunks;
    }
    if (this->pResourceFile) {
        memory_entries[num_memory_entries++] = (void *)this->pResourceFile->GetMemory();
    }

    return num_memory_entries;
}

eStreamPackLoader::eStreamPackLoader(int required_chunk_alignment,
                                     void (*loaded_streaming_entry_callback)(struct bChunk *, struct eStreamingEntry *, struct eStreamingPack *),
                                     void (*unloaded_streaming_entry_callback)(struct bChunk *, struct eStreamingEntry *, struct eStreamingPack *),
                                     void (*loading_header_phase1_callback)(struct eStreamingPackHeaderLoadingInfoPhase1 *),
                                     void (*loading_header_phase2_callback)(struct eStreamingPackHeaderLoadingInfoPhase2 *),
                                     void (*unloading_header_callback)(struct eStreamingPack *)) {
    this->PrintLoading = 0;
    this->NumLoadedStreamingPacks = 0;
    this->NumLoadedStreamingEntries = 0;
    this->NumLoadedBytes = 0;
    this->RequiredChunkAlignment = required_chunk_alignment;
    this->LoadedStreamingEntryCallback = loaded_streaming_entry_callback;
    this->UnloadedStreamingEntryCallback = unloaded_streaming_entry_callback;
    this->LoadingHeaderPhase1Callback = loading_header_phase1_callback;
    this->LoadingHeaderPhase2Callback = loading_header_phase2_callback;
    this->UnloadingHeaderCallback = unloading_header_callback;
}

int eStreamPackLoader::GetMemoryEntries(unsigned int *name_hash_table /* r27 */, int num_hashes /* r29 */, void **memory_entries /* r28 */,
                                        int num_memory_entries /* r31 */) {
    for (int n = 0; n < num_hashes; n++) {
        unsigned int name_hash = name_hash_table[n];
        if (name_hash != 0) {
            eStreamingEntry *streaming_entry = this->GetStreamingEntry(name_hash);
            if (streaming_entry && streaming_entry->ChunkData) {
                memory_entries[num_memory_entries++] = streaming_entry->ChunkData;
            }
        }
    }

    return num_memory_entries;
}

eStreamingPack *eStreamPackLoader::GetLoadedStreamingPack(const char *filename) {
    eStreamingPack *streaming_pack;

    for (streaming_pack = this->LoadedStreamingPackList.GetHead(); streaming_pack != this->LoadedStreamingPackList.EndOfList();
         streaming_pack = streaming_pack->GetNext()) {
        if (!bStrCmp(filename, streaming_pack->Filename)) {
            streaming_pack->Remove();
            this->LoadedStreamingPackList.AddHead(streaming_pack);
            return streaming_pack;
        }
    }

    return nullptr;
}

eStreamingPack *eStreamPackLoader::GetLoadedStreamingPack(unsigned int name_hash) {
    eStreamingPack *streaming_pack;

    for (streaming_pack = this->LoadedStreamingPackList.GetHead(); streaming_pack != this->LoadedStreamingPackList.EndOfList();
         streaming_pack = streaming_pack->GetNext()) {
        if (this->GetStreamingEntry(name_hash, streaming_pack)) {
            streaming_pack->Remove();
            this->LoadedStreamingPackList.AddHead(streaming_pack);
            return streaming_pack;
        }
    }

    return nullptr;
}

eStreamingEntry *eStreamPackLoader::GetStreamingEntry(unsigned int name_hash, eStreamingPack *streaming_pack) {
    if (!streaming_pack->DisabledFlag) {
        int num_entries = streaming_pack->StreamingEntryNumEntries;
        if (streaming_pack->StreamingEntryTable) {
            eStreamingEntry *streaming_entry =
                (eStreamingEntry *)ScanHashTableKey32(name_hash, streaming_pack->StreamingEntryTable, num_entries, 0x0, 0x18);
            if (streaming_entry) {
                return streaming_entry;
            }
        }
    }
    return nullptr;
}

eStreamingEntry *eStreamPackLoader::GetStreamingEntry(unsigned int name_hash) {
    eStreamingPack *streaming_pack;

    for (streaming_pack = this->LoadedStreamingPackList.GetHead(); streaming_pack != this->LoadedStreamingPackList.EndOfList();
         streaming_pack = streaming_pack->GetNext()) {
        eStreamingEntry *streaming_entry = this->GetStreamingEntry(name_hash, streaming_pack);
        if (streaming_entry) {
            return streaming_entry;
        }
    }

    return nullptr;
}

bChunk *eStreamPackLoader::GetAlignedChunkDataPtr(unsigned char *chunk_data) {
    int byte_alignment = this->RequiredChunkAlignment - 1;
    return (bChunk *)((size_t)(chunk_data + byte_alignment) & ~(size_t)byte_alignment);
}

eStreamingPackLoadTable *eStreamPackLoader::GetStreamPackLoadingTable() {
    for (int i = 0; i < 0x40; i++) {
        if (QueuedLoadingTables[i].Locked != 0)
            continue;

        QueuedLoadingTables[i].StreamPackLoader = this;
        QueuedLoadingTables[i].Locked = 1;
        return &QueuedLoadingTables[i];
    }

    return nullptr;
}

void eStreamPackLoader::InternalLoadedStreamingEntryCallback(void *callback_param, int error_status, void *callback_param2) {
    eStreamingPackLoadTable *loading_table = (eStreamingPackLoadTable *)callback_param2;
    eStreamingEntry *streaming_entry = (eStreamingEntry *)callback_param;
    eStreamPackLoader *stream_pack_loader = loading_table->StreamPackLoader;
    unsigned int name_hash = streaming_entry->NameHash;
    eStreamingPack *streaming_pack = stream_pack_loader->GetLoadedStreamingPack(name_hash);

    streaming_entry->Flags &= ~0x10;
    if (error_status == 0) {
        stream_pack_loader->NumLoadedStreamingEntries++;
        stream_pack_loader->NumLoadedBytes += streaming_entry->ChunkByteSize;
        if (streaming_entry->Flags & 0x20) {
            stream_pack_loader->InternalUnloadStreamingEntry(streaming_pack, streaming_entry);
            streaming_entry->Flags &= ~0x20;
        } else {
            bChunk *loaded_chunks = stream_pack_loader->GetAlignedChunkDataPtr(streaming_entry->ChunkData);
            if (streaming_entry->Flags & 1) {
                LZHeader *lz_header;
                char malloc_name[128];
                int uncompressed_size;
                int malloc_size;
                int memory_pool_num;
                int allocation_params = 0x2000;
                unsigned char *uncompressed_data;
                unsigned char *compressed_data;
                unsigned char *base_loaded_data;
                int actual_decompressed_size;

                bPlatEndianSwap((unsigned int *)loaded_chunks);
                bPlatEndianSwap((unsigned short *)loaded_chunks + 3);
                bPlatEndianSwap((unsigned char *)loaded_chunks); // extra call
                bPlatEndianSwap((unsigned int *)loaded_chunks + 2);
                bPlatEndianSwap((unsigned int *)loaded_chunks + 3);

                bSPrintf(malloc_name, "%s", streaming_pack->Filename);

                malloc_size = streaming_entry->UncompressedSize + stream_pack_loader->RequiredChunkAlignment;
                memory_pool_num = loading_table->MemoryPoolNum;
                if (memory_pool_num != 0) {
                    if (bLargestMalloc(memory_pool_num) > malloc_size) {
                        allocation_params |= (memory_pool_num & 0xF);
                    } else {
                        PrintStreamingPackMemoryWarning(malloc_name, malloc_size, memory_pool_num);
                    }
                }
                compressed_data = (unsigned char *)loaded_chunks;
                uncompressed_data = (unsigned char *)bMALLOC(malloc_size, __FILE__, __LINE__, allocation_params);
                base_loaded_data = streaming_entry->ChunkData;
                streaming_entry->ChunkData = uncompressed_data;
                loaded_chunks = stream_pack_loader->GetAlignedChunkDataPtr(streaming_entry->ChunkData);
                // what?
                LZDecompress(compressed_data, (unsigned char *)loaded_chunks);
                bFree(base_loaded_data);
            }

            stream_pack_loader->LoadedStreamingEntryCallback(loaded_chunks, streaming_entry, streaming_pack);
        }
    }

    if (streaming_pack) {
        streaming_pack->NumLoadsPending--;
    }
    if (loading_table) {
        loading_table->NumLoadsPending--;
        if (!loading_table->NumLoadsPending) {
            if (loading_table->Callback) {
                loading_table->Callback(loading_table->Param);
            }
            loading_table->StreamPackLoader = 0;
            loading_table->Locked = 0;
            loading_table->NumLoadsPending = 0;
            loading_table->Callback = 0;
            loading_table->Param = 0;
        }
    }
}

void eStreamPackLoader::InternalLoadStreamingEntry(eStreamingPackLoadTable *loading_table, eStreamingPack *streaming_pack,
                                                   eStreamingEntry *streaming_entry) {
    unsigned int name_hash = streaming_entry->NameHash;

    if (!loading_table || !streaming_pack || !streaming_entry)
        return;

    if (streaming_entry->RefCount != 0) {
        streaming_entry->RefCount++;
        return;
    }

    if (streaming_entry->Flags & 0x10) {
        streaming_entry->Flags &= ~0x20;
        streaming_entry->RefCount++;
    } else {
        char malloc_name[128];

        bSPrintf(malloc_name, "%s%s", streaming_pack->Filename, (streaming_entry->Flags & 0x1) ? " - Compressed" : "");

        int malloc_size = streaming_entry->ChunkByteSize + this->RequiredChunkAlignment;
        int allocation_params = 0x2000;
        int memory_pool_num = loading_table->MemoryPoolNum;

        if (streaming_entry->Flags & 0x1) {
            allocation_params = 0x2040;
            if (memory_pool_num != AllowCompressedStreamingTexturesInThisPoolNum) {
                memory_pool_num = 0;
            }
        }
        if (memory_pool_num != 0) {
            if (bLargestMalloc(memory_pool_num) > malloc_size) {
                allocation_params |= (memory_pool_num & 0xF);
            } else {
                PrintStreamingPackMemoryWarning(malloc_name, malloc_size, memory_pool_num);
            }
        }

        streaming_entry->ChunkData = (unsigned char *)bMALLOC(malloc_size, __FILE__, __LINE__, allocation_params);
        streaming_entry->RefCount++;
        streaming_pack->NumLoadsPending++;
        if (loading_table) {
            loading_table->NumLoadsPending++;
        }
        streaming_entry->Flags |= 0x10;
        bChunk *aligned_chunk_data = this->GetAlignedChunkDataPtr(streaming_entry->ChunkData);
        AddQueuedFile2(aligned_chunk_data, streaming_pack->Filename, streaming_entry->ChunkByteOffset, streaming_entry->ChunkByteSize,
                       eStreamPackLoader::InternalLoadedStreamingEntryCallback, streaming_entry, loading_table, nullptr);
    }

    streaming_pack->NumLoadedStreamingEntries++;
    streaming_pack->NumLoadedBytes += streaming_entry->ChunkByteSize;
}

// FIXME
int eStreamPackLoaderSortLoadEntries(eStreamPackLoadEntryInfo *a, eStreamPackLoadEntryInfo *b /* r4 */) {
    if (b->StreamingPack < a->StreamingPack) {
        return 0;
    }

    if (b->StreamingPack == a->StreamingPack) {
        if (b->FileOffset < a->FileOffset) {
            return 0;
        }
    }

    return 1;
}

extern void SetDelayedResourceCallback(void (*)(void *), void *);

// FIXME
//  name_hash_table /* r20 */
//  num_hashes      /* r22 */
//  callback        /* r21 */
//  param0          /* r19 */
//  memory_pool_num /* r31 */
void eStreamPackLoader::LoadStreamingEntry(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0,
                                           int memory_pool_num) {
    ProfileNode profile_node;
    eStreamingPackLoadTable *loading_table = this->GetStreamPackLoadingTable(); // r26
    bTList<eStreamPackLoadEntryInfo> load_info_list;                            // r1+0x8
    eStreamPackLoadEntryInfo *load_info_table;                                  // r25
    int num_load_entries;                                                       // r23
    unsigned int smallest_file_offset;                                          // r28
    int mem_required;

    if (!loading_table)
        return;

    smallest_file_offset = 0x7FFF0000;

    loading_table->MemoryPoolNum = memory_pool_num;
    loading_table->Callback = callback;
    loading_table->Param = param0;

    load_info_table = (eStreamPackLoadEntryInfo *)bMalloc(num_hashes * 0x14, 0x40);
    num_load_entries = 0;
    smallest_file_offset |= 0xFFFF;

    for (int i = 0; i < num_hashes; i++) {
        unsigned int name_hash = name_hash_table[i];
        eStreamingPack *streaming_pack = this->GetLoadedStreamingPack(name_hash);
        eStreamingEntry *streaming_entry = nullptr;

        if (streaming_pack) {
            streaming_entry = this->GetStreamingEntry(name_hash, streaming_pack);
        }
        if (streaming_entry) {
            eStreamPackLoadEntryInfo *load_info = &load_info_table[num_load_entries];
            load_info->StreamingPack = streaming_pack;
            load_info->StreamingEntry = streaming_entry;
            load_info->FileOffset = streaming_entry->ChunkByteOffset;
            if (streaming_entry->ChunkByteOffset < smallest_file_offset) {
                streaming_entry->ChunkByteOffset = smallest_file_offset;
            }
            num_load_entries++;

            load_info_list.AddTail(load_info);
        }
    }

    if (memory_pool_num == 0) {
        bLargestMalloc(0);
    }

    load_info_list.Sort(eStreamPackLoaderSortLoadEntries);
    eStreamPackLoadEntryInfo *load_info = load_info_list.GetHead();
    while (load_info != load_info_list.EndOfList()) {
        this->InternalLoadStreamingEntry(loading_table, load_info->StreamingPack, load_info->StreamingEntry);
        load_info = load_info->GetNext();
    }

    bFree(load_info_table);

    if (!loading_table->NumLoadsPending && callback) {
        loading_table->Locked = 0;
        SetDelayedResourceCallback(callback, param0);
    }
}

// STRIPPED
void eStreamPackLoader::LoadStreamingEntry(unsigned int name_hash /* r1+0x8 */, void (*callback)(void *) /* r5 */, void *param0 /* r0 */,
                                           int memory_pool_num /* r8 */) {}

void eStreamPackLoader::InternalUnloadStreamingEntry(eStreamingPack *streaming_pack, eStreamingEntry *streaming_entry) {
    if ((streaming_entry->Flags & 0x20) == 0) {
        bChunk *aligned_chunk = this->GetAlignedChunkDataPtr(streaming_entry->ChunkData);
        this->UnloadedStreamingEntryCallback(aligned_chunk, streaming_entry, streaming_pack);
    }
    bFree(streaming_entry->ChunkData);

    streaming_pack->RegisterUnloadStreamingEntry(streaming_entry);
    this->RegisterUnloadStreamingEntry(streaming_entry);
}

// STRIPPED
int eStreamPackLoader::IsLoaded(unsigned int name_hash) {}

void eStreamPackLoader::UnloadStreamingEntry(unsigned int name_hash, eStreamingPack *streaming_pack) {

    if (name_hash == 0)
        return;

    if (streaming_pack == nullptr) {
        streaming_pack = this->GetLoadedStreamingPack(name_hash);
        if (streaming_pack == nullptr)
            return;
    }

    eStreamingEntry *streaming_entry = this->GetStreamingEntry(name_hash, streaming_pack);
    if (streaming_entry == nullptr)
        return;

    if (--streaming_entry->RefCount != 0)
        return;

    eWaitUntilRenderingDone();

    if ((streaming_entry->Flags & 0x10) != 0) {
        streaming_entry->Flags |= 0x20;
        return;
    }

    this->InternalUnloadStreamingEntry(streaming_pack, streaming_entry);
}

void eStreamPackLoader::UnloadStreamingEntry(unsigned int *name_hash_table, int num_hashes) {
    for (int i = 0; i < num_hashes; i++) {
        this->UnloadStreamingEntry(name_hash_table[i], nullptr);
    }

    ProfileNode profile_node;
}

void eStreamPackLoader::UnloadAllStreamingEntries(const char *filename) {
    eStreamingPack *streaming_pack = this->GetLoadedStreamingPack(filename);

    if (!streaming_pack)
        return;

    eStreamingEntry *streaming_entry = streaming_pack->StreamingEntryTable;
    int num_entries = streaming_pack->StreamingEntryNumEntries;

    for (int i = 0; i < num_entries; i++, streaming_entry++) {
        if (streaming_entry->ChunkData == nullptr)
            continue;

        streaming_entry->RefCount = 1;
        this->UnloadStreamingEntry(streaming_entry->NameHash, streaming_pack);
    }
}

int eStreamPackLoader::IsLoading(const char *filename) {
    eStreamingPack *streaming_pack = this->LoadedStreamingPackList.GetHead();

    for (; streaming_pack != this->LoadedStreamingPackList.EndOfList(); streaming_pack = streaming_pack->GetNext()) {
        if (filename == nullptr || bStrCmp(filename, streaming_pack->Filename) == 0) {
            if (streaming_pack->HeaderLoaded == 0 || streaming_pack->NumLoadsPending > 0) {
                return 1;
            }
        }
    }

    return 0;
}

// FIXME
int eStreamPackLoader::TestLoadStreamingEntry(unsigned int *name_hash_table, int num_hashes, int memory_pool_num, bool error_if_out_in_main_pool) {
    ProfileNode profile_node;
    int amount_not_alloc = 0;
    int amount_not_alloc_main_mem = 0;
    bTList<eStreamPackLoadEntryInfo> load_info_list;
    eStreamPackLoadEntryInfo *load_info_table = (eStreamPackLoadEntryInfo *)bMalloc(num_hashes * 0x14, 0x40);
    int num_load_entries = 0;

    for (int i = 0; i < num_hashes; i++) {
        unsigned int name_hash = name_hash_table[i];
        eStreamingPack *streaming_pack = this->GetLoadedStreamingPack(name_hash_table[i]);
        if (streaming_pack) {
            eStreamingEntry *streaming_entry = this->GetStreamingEntry(name_hash, streaming_pack);
            if (streaming_entry) {
                eStreamPackLoadEntryInfo *load_info = &load_info_table[num_load_entries++];
                load_info->FileOffset = streaming_entry->ChunkByteOffset;
                load_info->StreamingPack = streaming_pack;
                load_info->StreamingEntry = streaming_entry;
                load_info_list.AddTail(load_info);
            }
        }
    }

    load_info_list.Sort(eStreamPackLoaderSortLoadEntries);

    for (int pass = 0; pass <= 2; pass++) {
        unsigned int prev_name_hash = 0x10;
        for (eStreamPackLoadEntryInfo *load_info = load_info_list.GetHead(); load_info != load_info_list.EndOfList();
             load_info = load_info->GetNext()) {
            eStreamingEntry *streaming_entry = load_info->StreamingEntry;
            unsigned int name_hash = streaming_entry->NameHash;

            if (streaming_entry && (streaming_entry->UserFlags & prev_name_hash) == 0 && name_hash != prev_name_hash) {
                eStreamingPack *streaming_pack = load_info->StreamingPack;
                prev_name_hash = name_hash;

                switch (pass) {
                    case 0: {
                        char malloc_name[128];
                        int malloc_size;
                        int allocation_params;

                        bSPrintf(malloc_name, "%s%s", streaming_pack->Filename, (name_hash & 1) ? " - Compressed" : "");

                        allocation_params = 0x2000;
                        malloc_size = streaming_entry->ChunkByteSize + this->RequiredChunkAlignment;

                        if (streaming_entry->Flags & 1) {
                            allocation_params = 0x2040;
                        } else if (memory_pool_num != 0) {
                            allocation_params = memory_pool_num | 0x2000;
                        }

                        if (bLargestMalloc(allocation_params) > malloc_size) {
                            streaming_entry->ChunkData = (unsigned char *)bMalloc(malloc_size, allocation_params);
                        } else if ((allocation_params & 0xF) == 0) {
                            amount_not_alloc_main_mem += malloc_size;
                        }
                        amount_not_alloc += malloc_size;
                        break;
                    }
                    case 1: {
                        char malloc_name[128];
                        int malloc_size;
                        int allocation_params;

                        bSPrintf(malloc_name, "%s", streaming_pack->Filename);

                        allocation_params = 0x2000;
                        malloc_size = streaming_entry->ChunkByteSize + this->RequiredChunkAlignment;

                        if (memory_pool_num != 0) {
                            allocation_params = memory_pool_num | 0x2000;
                        }

                        unsigned char *compressed_data = streaming_entry->ChunkData;
                        // streaming_entry->ChunkData = num_hashes;

                        if (bLargestMalloc(allocation_params) > malloc_size) {
                            streaming_entry->ChunkData = (unsigned char *)bMalloc(malloc_size, allocation_params);
                        } else {
                            amount_not_alloc += malloc_size;
                            if ((allocation_params & 0xF) == 0) {
                                amount_not_alloc_main_mem += malloc_size;
                            }
                        }

                        if (compressed_data) {
                            bFree(compressed_data);
                        }
                        break;
                    }
                    case 2:
                        if (error_if_out_in_main_pool && streaming_entry->ChunkData) {
                            bFree(streaming_entry->ChunkData);
                        }
                        break;
                }
            }
        }
    }

    // TODO

    bFree(load_info_table);
}

bool eStreamPackLoader::DefragmentAllocation(void *allocation) {
    for (eStreamingPack *streaming_pack = this->LoadedStreamingPackList.GetHead(); streaming_pack != this->LoadedStreamingPackList.EndOfList();
         streaming_pack = streaming_pack->GetNext()) {
        for (int i = 0; i < streaming_pack->StreamingEntryNumEntries; i++) {
            eStreamingEntry *streaming_entry = &streaming_pack->StreamingEntryTable[i];
            if (streaming_entry->ChunkData == allocation) {
                if (streaming_pack->NumLoadsPending != 0) {
                    bBreak();
                }
                bChunk *chunks = this->GetAlignedChunkDataPtr(streaming_entry->ChunkData);
                this->UnloadedStreamingEntryCallback(chunks, streaming_entry, streaming_pack);
                streaming_entry->ChunkData = (unsigned char *)MoveDefragmentAllocation(allocation);
                chunks = this->GetAlignedChunkDataPtr(streaming_entry->ChunkData);
                this->LoadedStreamingEntryCallback(chunks, streaming_entry, streaming_pack);
                this->LoadedStreamingPackList.Remove(streaming_pack);
                this->LoadedStreamingPackList.AddHead(streaming_pack);
                return 1;
            }
        }
    }

    return 0;
}

extern void DVDErrorTask(void *, int);
extern void bThreadYield(int);

void eStreamPackLoader::WaitForLoadingToFinish(const char *filename) {
    while (this->IsLoading(filename)) {
        DVDErrorTask(nullptr, 0);
        bThreadYield(8);
        ServiceResourceLoading();
    }
}

eStreamingPack *eStreamPackLoader::CreateStreamingPack(const char *filename, void (*callback_function)(void *), void *callback_param,
                                                       int memory_pool_num) {
    eStreamingPack *streaming_pack = this->GetLoadedStreamingPack(filename);

    if (streaming_pack) {
        streaming_pack->RefCount++;
        return streaming_pack;
    }

    bFile *file = bOpen(filename, 1, 1);
    if (!file)
        return nullptr;

    int file_size = bFileSize(file);
    bClose(file);
    if (file_size <= 0x40)
        return nullptr;

    eStreamingPackHeaderLoadingInfo *loading_info = (eStreamingPackHeaderLoadingInfo *)bMalloc(0x10, 0x40);
    bMemSet(loading_info, 0x0, 0x10);

    loading_info->LoadingDoneCallback = callback_function;
    loading_info->LoadingDoneCallbackParam = callback_param;

    streaming_pack = new eStreamingPack();
    streaming_pack->Filename = bAllocateSharedString(filename);
    streaming_pack->HeaderMemoryPoolNum = memory_pool_num;
    streaming_pack->HeaderSize = 0;
    streaming_pack->RefCount = 1;
    streaming_pack->LoadingInfo = loading_info;
    this->LoadedStreamingPackList.AddTail(streaming_pack);
    this->NumLoadedStreamingPacks++;

    int amount_to_load;
    if (file_size > 0x8000) {
        amount_to_load = 0x8000;
        streaming_pack->HeaderChunks = (bChunk *)bMalloc(amount_to_load, 0x2040);
        AddQueuedFile2(streaming_pack->HeaderChunks, streaming_pack->Filename, 0, loading_info->HeaderChunksSize,
                       eStreamPackLoader::InternalLoadingHeaderPhase1Callback, (void *)this, (void *)streaming_pack, nullptr);
    }

    return streaming_pack;
}

void eStreamPackLoader::InternalLoadingHeaderPhase1Callback(void *callback_param, int error_status, void *callback_param2) {
    eStreamPackLoader *stream_pack_loader = (eStreamPackLoader *)callback_param;
    eStreamingPack *streaming_pack = (eStreamingPack *)callback_param2;
    eStreamingPackHeaderLoadingInfo *loading_info = streaming_pack->LoadingInfo;
    eStreamingPackHeaderLoadingInfoPhase1 user_load_info;
    bMemSet(&user_load_info, 0x0, 0x10);

    char malloc_name[256];
    int next_load_position;
    int next_load_amount;
    int allocation_params = 0x2000;

    user_load_info.Filename = streaming_pack->Filename;
    user_load_info.TempHeaderChunks = streaming_pack->HeaderChunks;

    stream_pack_loader->LoadingHeaderPhase1Callback(&user_load_info);

    bSPrintf(malloc_name, "%s: StrmHdrChks", streaming_pack->Filename);

    next_load_amount = user_load_info.NextLoadAmount;
    next_load_position = user_load_info.NextLoadPosition;
    streaming_pack->HeaderSize = next_load_amount;

    if (streaming_pack->HeaderMemoryPoolNum != 0 && bLargestMalloc(streaming_pack->HeaderMemoryPoolNum) > next_load_amount) {
        allocation_params = (streaming_pack->HeaderMemoryPoolNum & 0xF) | 0x2040;
    }

    bChunk *perm_header_chunks = (bChunk *)bMalloc(next_load_amount, allocation_params);
    bChunk *temp_header_chunks = streaming_pack->HeaderChunks;

    if (next_load_position + next_load_amount <= loading_info->HeaderChunksSize) {
        bMemCpy(perm_header_chunks, &((char *)temp_header_chunks)[next_load_position], next_load_amount);
        loading_info->HeaderChunksMemCopied = 1; // ^^^ hack?
    }
    bFree(temp_header_chunks);

    streaming_pack->HeaderChunks = perm_header_chunks;
    if (loading_info->HeaderChunksMemCopied == 0) {
        AddQueuedFile2(perm_header_chunks, streaming_pack->Filename, next_load_position, next_load_amount,
                       eStreamPackLoader::InternalLoadingHeaderPhase2Callback, stream_pack_loader, streaming_pack, nullptr);
    }

    if (loading_info->HeaderChunksMemCopied != 0) {
        eStreamPackLoader::InternalLoadingHeaderPhase2Callback(stream_pack_loader, 0x0, streaming_pack);
    }
}

// FIXME
void eStreamPackLoader::InternalLoadingHeaderPhase2Callback(void *callback_param, int error_status, void *callback_param2) {
    eStreamPackLoader *stream_pack_loader = (eStreamPackLoader *)callback_param;
    eStreamingPack *streaming_pack = (eStreamingPack *)callback_param2;
    eStreamingPackHeaderLoadingInfo *loading_info = streaming_pack->LoadingInfo;
    eStreamingPackHeaderLoadingInfoPhase2 user_load_info;
    bMemSet(&user_load_info, 0x0, 0x28);

    user_load_info.Filename = streaming_pack->Filename;
    user_load_info.HeaderChunks = streaming_pack->HeaderChunks;
    user_load_info.HeaderChunksMemCopied = loading_info->HeaderChunksMemCopied;
    user_load_info.pStreamingPack = streaming_pack;

    stream_pack_loader->LoadingHeaderPhase2Callback(&user_load_info);

    streaming_pack->StreamingEntryTable = user_load_info.StreamingEntryTable;
    streaming_pack->StreamingEntryNumEntries = user_load_info.StreamingEntryNumEntries;
    streaming_pack->SolidListHeader = user_load_info.SolidListHeader;
    streaming_pack->pTexturePackHeader = user_load_info.pTexturePackHeader;

    if (user_load_info.LoadResourceFileAmount != 0) {
        loading_info = (eStreamingPackHeaderLoadingInfo *)(&user_load_info) + 1;
        streaming_pack->pResourceFile = CreateResourceFile(streaming_pack->Filename, RESOURCE_FILE_CAR, 0x0, user_load_info.LoadResourceFilePosition,
                                                           user_load_info.LoadResourceFileAmount);

        char malloc_name[1024];
        int allocation_params = 0x2000;
        bSPrintf(malloc_name, "%s: StrmHdrRes", streaming_pack->Filename);

        if (streaming_pack->HeaderMemoryPoolNum != 0 && bLargestMalloc(streaming_pack->HeaderMemoryPoolNum) > user_load_info.LoadResourceFileAmount) {
            allocation_params = (streaming_pack->HeaderMemoryPoolNum & 0xF) | 0x2040;
        }

        streaming_pack->pResourceFile->SetAllocationParams(allocation_params, malloc_name);
        streaming_pack->pResourceFile->BeginLoading(eStreamPackLoader::InternalLoadingHeaderPhase3Callback, streaming_pack);
    }

    if (streaming_pack->pResourceFile == nullptr) {
        eStreamPackLoader::InternalLoadingHeaderPhase3Callback(streaming_pack);
    }
}

void eStreamPackLoader::InternalLoadingHeaderPhase3Callback(void *callback_param) {
    eStreamingPack *streaming_pack = (eStreamingPack *)callback_param;
    eStreamingPackHeaderLoadingInfo *loading_info = streaming_pack->LoadingInfo;
    void (*user_callback_function)(void *) = loading_info->LoadingDoneCallback;
    void *user_callback_param = loading_info->LoadingDoneCallbackParam;

    streaming_pack->HeaderLoaded = 1;
    bFree(loading_info);

    streaming_pack->LoadingInfo = 0;
    if (user_callback_function) {
        user_callback_function(user_callback_param);
    }
}

int eStreamPackLoader::DeleteStreamingPack(const char *filename) {
    eStreamingPack *streaming_pack = GetLoadedStreamingPack(filename);

    if (!streaming_pack)
        return 0;
    if (--streaming_pack->RefCount != 0)
        return 1;

    streaming_pack->DisabledFlag = 0;

    this->UnloadAllStreamingEntries(filename);
    this->UnloadingHeaderCallback(streaming_pack);

    streaming_pack->Remove();

    if (streaming_pack->HeaderChunks) {
        bFree(streaming_pack->HeaderChunks);
    }
    if (streaming_pack->pResourceFile) {
        UnloadResourceFile(streaming_pack->pResourceFile);
    }

    bFreeSharedString(streaming_pack->Filename);
    delete streaming_pack;

    this->NumLoadedStreamingPacks--;

    return 1;
}
