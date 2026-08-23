#include "spch/spch.h"

unsigned int seedX[6] = {
    0xF22D0E56,
    0x883126E9,
    0xC624DD2F,
    0x0702C49C,
    0x9E353F7D,
    0x6FDF3B64,
};

int gRandArrayIndex = 0;

struct RandHandle {
    unsigned short rand;
    unsigned short handle;
};

RandHandle gRandArray[32];

unsigned int iSPCH_EACrandom() {
    unsigned int ax;
    unsigned int c;

    c = 0;
    ax = seedX[5] + seedX[4];
    if (ax < seedX[5] || ax < seedX[4]) {
        c = 1;
    }
    seedX[4] = ax;
    ax = ax + seedX[3] + c;
    c = 0;
    if (ax < seedX[3]) {
        c = 1;
    }
    seedX[3] = ax;
    ax = ax + seedX[2] + c;
    c = 0;
    if (ax < seedX[2]) {
        c = 1;
    }
    seedX[2] = ax;
    ax = ax + seedX[1] + c;
    c = 0;
    if (ax < seedX[1]) {
        c = 1;
    }
    seedX[1] = ax;
    ax = ax + seedX[0] + c;
    seedX[0] = ax;
    seedX[5]++;
    if (seedX[5] != 0) {
        return ax;
    }
    seedX[4]++;
    if (seedX[4] != 0) {
        return ax;
    }
    seedX[3]++;
    if (seedX[3] != 0) {
        return ax;
    }
    seedX[2]++;
    if (seedX[2] != 0) {
        return ax;
    }
    seedX[1]++;
    if (seedX[1] != 0) {
        return ax;
    }
    seedX[0] = ax + 1;
    return seedX[0];
}

static void iSPCH_EACseedrandom(unsigned int seed) {
    unsigned int ax = seed + 0xF22D0E56;

    seedX[0] = ax;
    seedX[1] = ax += 0x96041893;
    seedX[2] = ax += 0x3DF3B646;
    seedX[3] = ax += 0x40DDE76D;
    seedX[4] = ax += 0x97327AE1;
    seedX[5] = ax += 0xD1A9FBE7;
}

static int iSPCH_FindRandInQueue(unsigned short rand, int checkBack, unsigned short randHandle) {
    int result;
    int index;
    int done;
    int numTests;
    int numChecks;

    result = -1;
    if (checkBack > 0x20) {
        checkBack = 0x20;
    }
    numTests = 0;
    numChecks = 0;
    done = 0;
    index = gRandArrayIndex;
    do {
        if (numTests < checkBack) {
            if (gRandArray[index].handle == randHandle) {
                numTests++;
                if (gRandArray[index].rand == rand) {
                    done = 1;
                    result = numTests - 1;
                }
            }
        } else {
            done = 1;
        }
        index--;
        if (index < 0) {
            index += 0x20;
        }
        numChecks++;
        if (numChecks > 0x20) {
            done = 1;
        }
    } while (done == 0);
    return result;
}

static void iSPCH_AddRandToQueue(unsigned short rand, unsigned short handle) {
    gRandArrayIndex++;
    if (gRandArrayIndex > 0x1F) {
        gRandArrayIndex = 0;
    }
    gRandArray[gRandArrayIndex].rand = rand;
    gRandArray[gRandArrayIndex].handle = handle;
}

unsigned short iSPCH_Rand(int max, int randHandle);

void iSPCH_InitRandom(unsigned int gameSeed) {
    int i;

    i = 0;
    do {
        gRandArray[i].rand = 0xFFFF;
        gRandArray[i].handle = 0xFFFF;
        i++;
    } while (i < 0x20);
    gRandArrayIndex = 0;
    gameSeed += gameSeed << 16;
    iSPCH_EACseedrandom(gameSeed);
    int warmup;
    warmup = 0x20 - static_cast<int>(gameSeed) % 0x20;
    i = 0;
    if (i < warmup) {
        do {
            iSPCH_Rand(10, -1);
            i++;
        } while (i < warmup);
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

    threshold = max / 2;
    numTests = 0;
    done = 0;
    oldest = -1;
    if (threshold > 10) {
        threshold = 10;
    }
    result = (iSPCH_EACrandom() >> 16) * max >> 16;
    bestResult = result;
    if (randHandle != -1) {
        randHandle = randHandle & 0xFFFF;
        do {
            foundAt = iSPCH_FindRandInQueue(result, threshold, randHandle);
            if (foundAt == -1) {
                bestResult = result;
                done = 1;
            } else if (foundAt > oldest) {
                oldest = foundAt;
                bestResult = result;
            }
            numTests++;
            if (numTests > 0x1F) {
                done = 1;
            }
            if (done == 0) {
                result = (iSPCH_EACrandom() >> 16) * max >> 16;
            }
        } while (done == 0);
        iSPCH_AddRandToQueue(bestResult, randHandle);
    }
    return result;
}
