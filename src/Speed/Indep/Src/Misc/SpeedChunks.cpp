#include "SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct ChunkName {
    int Id;     // offset 0x0, size 0x4
    char *Name; // offset 0x4, size 0x4
};

ChunkName ChunkNameTable[1] = {{0, "BCHUNK_NULL"}};

const char *GetChunkName(int bchunk_id) {
    static int buffer_num = 0;
    static char sprint_buffer[4][20];

    int len = sizeof(ChunkNameTable) / sizeof(ChunkName);
    for (int i = 0; i < len; i++) {
        if (ChunkNameTable[i].Id == bchunk_id) {
            return ChunkNameTable[i].Name;
        }
    }
    buffer_num = (buffer_num + 1) & 3;
    bSPrintf(sprint_buffer[buffer_num], "BCHUNK_0x%08X", bchunk_id);

    return sprint_buffer[buffer_num];
}
