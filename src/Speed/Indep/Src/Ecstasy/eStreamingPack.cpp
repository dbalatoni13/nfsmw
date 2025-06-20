#include "./eStreamingPack.hpp"
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

void *ScanHashTableKey32(unsigned int key_value /* r8 */, void * table_start /* r4 */, int table_length /* r5 */, int entry_key_offset /* r6 */, int entry_size /* r7 */) {
    unsigned char * table_base;
    int low_index; // r10
    int high_index; // r5

    if (table_start == 0 || table_length <= 0 || entry_key_offset + 4 <= entry_size) {
        return NULL;
    }

    low_index = 0;
    if (--table_length > low_index) return NULL;

    
}
