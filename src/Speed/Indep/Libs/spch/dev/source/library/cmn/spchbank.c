#include "spch/spch.h"

int SPCH_GetBankPtrMemSize(int numBanks) {
    return numBanks << 3;
}

void iSPCH_InitBanks() {
    gVoxBanks = 0;
    gUniqueBankHandle = 0;
    gNumBanks = 0;
    gBankCount = 0;
}

unsigned int iSPCH_GetStartSample(unsigned int gameNum, int numSamples, int cycleLength) {
    int partitionSize;
    int overflowSamples;
    unsigned int startSample;

    startSample = gameNum;
    partitionSize = numSamples / cycleLength;
    overflowSamples = numSamples - partitionSize * cycleLength;
    if (static_cast<unsigned int>(overflowSamples) < startSample) {
        return partitionSize * startSample + overflowSamples;
    }
    return partitionSize * startSample + startSample;
}

int iSPCH_TestSubBankBounds(int bankIndex, unsigned int subBankIndex) {
    int result;

    result = 0;
    if (gVoxBanks == 0) {
        goto abort;
    }
    if (bankIndex < 0) {
        goto abort;
    }
    if (gVoxBanks[bankIndex].voxHdr->numSubBanks == 0xFFFF) {
        goto abort;
    }
    result = subBankIndex < gVoxBanks[bankIndex].voxHdr->numSubBanks;
abort:
    return result;
}

int iSPCH_FindBankIndexFromHandle(int bankHandle) {
    {
        int i;

        i = 0;
        if (i < gBankCount) {
            do {
                if (gVoxBanks[i].bankHandle == bankHandle) {
                    return i;
                }
                i++;
            } while (i < gBankCount);
        }
    }
    return -1;
}
