#include "./spchi.h"

// total size: 0x4
struct RandHandle {
    unsigned short rand;    // offset 0x0, size 0x2
    unsigned short handle;  // offset 0x2, size 0x2
};

extern unsigned int seedX[6];
extern RandHandle gRandArray[32];
extern int gRandArrayIndex;

unsigned short iSPCH_Rand(int max, int randHandle);

unsigned int iSPCH_EACrandom(void) {
    unsigned int ax;
    unsigned int c;

    ax = seedX[5] + seedX[4];
    c = (ax < seedX[5]) || (ax < seedX[4]);
    seedX[4] = ax;
    ax = ax + seedX[3] + c;
    c = ax < seedX[3];
    seedX[3] = ax;
    ax = ax + seedX[2] + c;
    c = ax < seedX[2];
    seedX[2] = ax;
    ax = ax + seedX[1] + c;
    c = ax < seedX[1];
    seedX[1] = ax;
    ax = ax + seedX[0] + c;
    seedX[0] = ax;
    seedX[5] = seedX[5] + 1;
    if (seedX[5] != 0) {
        return ax;
    }
    seedX[4] = seedX[4] + 1;
    if (seedX[4] != 0) {
        return ax;
    }
    seedX[3] = seedX[3] + 1;
    if (seedX[3] != 0) {
        return ax;
    }
    seedX[2] = seedX[2] + 1;
    if (seedX[2] != 0) {
        return ax;
    }
    seedX[1] = seedX[1] + 1;
    if (seedX[1] != 0) {
        return ax;
    }
    seedX[0] = seedX[0] + 1;
    /* Misma barrera que la hermana snd/srandom (que casa al 100%): sin ella
     * GCC fusiona el resultado con r3 y se pierde el `mr r3, r0` del objetivo. */
    return seedX[0];
}

static void iSPCH_EACseedrandom(unsigned int seed) {
    unsigned int ax;

    ax = seed - 0xDD2F1AA;
    seedX[0] = ax;
    ax = ax - 0x69FBE76D;
    seedX[1] = ax;
    ax = ax + 0x3DF3B646;
    seedX[2] = ax;
    ax = ax + 0x40DDE76D;
    seedX[3] = ax;
    ax = ax - 0x68CD851F;
    seedX[4] = ax;
    seedX[5] = ax - 0x2E560419;
}

static int iSPCH_FindRandInQueue(unsigned short rand, int checkBack, unsigned short randHandle) {
    int result;
    int index;
    int done;
    int numTests;
    int numChecks;

    result = -1;
    if (checkBack > 32) {
        checkBack = 32;
    }
    index = gRandArrayIndex;
    numTests = 0;
    numChecks = 0;
    done = 0;
    while (done == 0) {
        if (numTests < checkBack) {
            if (gRandArray[index].handle == randHandle) {
                numTests = numTests + 1;
                if (gRandArray[index].rand == rand) {
                    done = 1;
                    result = numTests - 1;
                }
            }
        } else {
            done = 1;
        }
        index = index - 1;
        if (index < 0) {
            index = index + 32;
        }
        numChecks = numChecks + 1;
        if (numChecks > 32) {
            done = 1;
        }
    }
    return result;
}

static void iSPCH_AddRandToQueue(unsigned short rand, unsigned short handle) {
    gRandArrayIndex = gRandArrayIndex + 1;
    if (gRandArrayIndex > 31) {
        gRandArrayIndex = 0;
    }
    gRandArray[gRandArrayIndex].rand = rand;
    gRandArray[gRandArrayIndex].handle = handle;
}

void iSPCH_InitRandom(unsigned int gameSeed) {
    int i;
    int warmup;

    for (i = 0; i < 32; i++) {
        gRandArray[i].rand = -1;
        gRandArray[i].handle = -1;
    }
    gameSeed = gameSeed + (gameSeed << 16);
    gRandArrayIndex = 0;
    iSPCH_EACseedrandom(gameSeed);
    warmup = 32 - ((int)gameSeed % 32);
    for (i = 0; i < warmup; i++) {
        iSPCH_Rand(10, -1);
    }
}

unsigned short iSPCH_Rand(int max, int randHandle) {
    int threshold;
    unsigned short result;
    unsigned short bestResult;
    int numTests;
    int done;
    int foundAt;
    int oldest;
    unsigned short handle;

    numTests = 0;
    done = 0;
    oldest = -1;
    threshold = max / 2;
    if (threshold > 10) {
        threshold = 10;
    }
    result = ((iSPCH_EACrandom() >> 16) * max) >> 16;
    bestResult = result;
    if (randHandle != -1) {
        handle = randHandle;
        while (done == 0) {
            foundAt = iSPCH_FindRandInQueue(result, threshold, handle);
            if (foundAt == -1) {
                bestResult = result;
                done = 1;
            } else if (foundAt > oldest) {
                oldest = foundAt;
                bestResult = result;
            }
            numTests = numTests + 1;
            if (numTests > 31) {
                done = 1;
            }
            if (done == 0) {
                result = ((iSPCH_EACrandom() >> 16) * max) >> 16;
            }
        }
        iSPCH_AddRandToQueue(bestResult, handle);
    }
    return result;
}
