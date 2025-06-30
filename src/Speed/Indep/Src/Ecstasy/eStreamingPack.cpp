#include "./eStreamingPack.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"


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
    return NULL;
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
        return NULL;
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
    return NULL;
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

    for (
        streaming_pack = this->LoadedStreamingPackList.GetHead();
        streaming_pack != this->LoadedStreamingPackList.EndOfList();
        streaming_pack = streaming_pack->GetNext())
    {
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

    for (
        streaming_pack = this->LoadedStreamingPackList.GetHead();
        streaming_pack != this->LoadedStreamingPackList.EndOfList();
        streaming_pack = streaming_pack->GetNext())
    {
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
        eStreamingEntry *streaming_entry = (eStreamingEntry *)ScanHashTableKey32(name_hash, streaming_pack->StreamingEntryTable, num_entries, 0x0, 0x18);
        if (streaming_entry) {
            return streaming_entry;
        }
    }
    }
    return nullptr;
}

eStreamingEntry *eStreamPackLoader::GetStreamingEntry(unsigned int name_hash) {
    eStreamingPack *streaming_pack;

    for (
        streaming_pack = this->LoadedStreamingPackList.GetHead();
        streaming_pack != this->LoadedStreamingPackList.EndOfList();
        streaming_pack = streaming_pack->GetNext())
    {
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
        if (QueuedLoadingTables[i].Locked != 0) continue;

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
                LZHeader * lz_header;
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

void eStreamPackLoader::InternalLoadStreamingEntry(eStreamingPackLoadTable *loading_table, eStreamingPack *streaming_pack, eStreamingEntry *streaming_entry) {
    unsigned int name_hash = streaming_entry->NameHash;

    if (!loading_table || !streaming_pack || !streaming_entry) return;

    if (streaming_entry->RefCount != 0) {
        streaming_entry->RefCount++;
        return;
    }

    if (streaming_entry->Flags & 0x10) {
        streaming_entry->Flags &= ~0x20;
        streaming_entry->RefCount++;
    } else {
        char malloc_name[128];

        bSPrintf(malloc_name, "%s%s", streaming_pack->Filename,
            (streaming_entry->Flags & 0x1) ? " - Compressed" : "");

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
        AddQueuedFile2(
            aligned_chunk_data,
            streaming_pack->Filename,
            streaming_entry->ChunkByteOffset,
            streaming_entry->ChunkByteSize,
            eStreamPackLoader::InternalLoadedStreamingEntryCallback,
            streaming_entry, 
            loading_table,
            nullptr
        );
    }

    streaming_pack->NumLoadedStreamingEntries++;
    streaming_pack->NumLoadedBytes += streaming_entry->ChunkByteSize;
}
