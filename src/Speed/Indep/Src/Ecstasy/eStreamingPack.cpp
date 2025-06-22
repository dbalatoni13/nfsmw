#include "./eStreamingPack.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"


int AllowCompressedStreamingTexturesInThisPoolNum; // size: 0x4, address: 0x8041A5E4
SlotPool *eStreamingPackSlotPool;
eStreamingPackLoadTable QueuedLoadingTables[64]; // size: 0x400, address: 0x8046A770

void InitStreamingPacks() {
    bMemSet(QueuedLoadingTables, 0x0, 0x400);

    const char *test = "eStreamingPackSlotPool";
    eStreamingPackSlotPool = bNewSlotPool(0x44, 0x40, test, GetVirtualMemoryAllocParams());
}

//STRIPPED
void CloseStreamingPacks() {}

void PrintStreamingPackMemoryWarning(const char * malloc_name /* r3 */, int malloc_size /* r4 */, int memory_pool_num /* r5 */) {
    int language_pool_num; // probably used for EU release

    bGetMemoryPoolNum("LanguageMemoryPool");
}

void *ScanHashTableKey32(unsigned int key_value, void *table_start, int table_length, int entry_key_offset, int entry_size) {
    unsigned char* table_base;
    int low_index;
    int high_index;


    if (table_start == 0 || table_length <= 0 || entry_key_offset + 4U > entry_size) {
        return NULL;
    }
    
    high_index = table_length - 1;
    low_index = 0;

    while (low_index <= high_index) {
        int mid_index;
        unsigned int mid_key_value;
        unsigned char* mid_entry;
        
        mid_key_value = low_index + high_index;
        mid_index = (int) (mid_key_value + (mid_key_value >> 0x1FU)) >> 1;
        table_base = &((unsigned char*)table_start)[mid_index * entry_size];
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
                        if (*(unsigned int*)(mid_entry + entry_key_offset) != key_value) {
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

//STRIPPED
void *ScanHashTableKey16(unsigned short key_value /* r8 */, void *table_start /* r4 */, int table_length /* r5 */, int entry_key_offset /* r6 */, int entry_size /* r7 */);

void *ScanHashTableKey8(unsigned char key_value /* r8 */, void *table_start /* r4 */, int table_length /* r5 */, int entry_key_offset /* r6 */, int entry_size /* r7 */) {
    unsigned char* table_base;
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
        unsigned char* mid_entry;
        
        mid_key_value = low_index + high_index;
        mid_index = (int) (mid_key_value + (mid_key_value >> 0x1FU)) >> 1;
        table_base = &((unsigned char*)table_start)[mid_index * entry_size];
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
    if (this->HeaderChunks != nullptr) {
        memory_entries[num_memory_entries++] = (void *)this->HeaderChunks;
    }
    if (this->pResourceFile != nullptr) {
        memory_entries[num_memory_entries++] = (void *)this->pResourceFile->GetMemory();
    }
    
    return num_memory_entries;
}

eStreamPackLoader::eStreamPackLoader(
    int required_chunk_alignment,
    void (* loaded_streaming_entry_callback)(struct bChunk *, struct eStreamingEntry *, struct eStreamingPack *),
    void (* unloaded_streaming_entry_callback)(struct bChunk *, struct eStreamingEntry *, struct eStreamingPack *),
    void (* loading_header_phase1_callback)(struct eStreamingPackHeaderLoadingInfoPhase1 *),
    void (* loading_header_phase2_callback)(struct eStreamingPackHeaderLoadingInfoPhase2 *),
    void (* unloading_header_callback)(struct eStreamingPack *))
{
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

int eStreamPackLoader::GetMemoryEntries(unsigned int * name_hash_table /* r27 */, int num_hashes /* r29 */, void **memory_entries /* r28 */, int num_memory_entries /* r31 */) {
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

extern int bStrCmp(const char * s1 /* r3 */, const char * s2 /* r4 */);
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
