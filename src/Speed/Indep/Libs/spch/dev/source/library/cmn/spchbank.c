#include "spch/spch.h"

extern unsigned int gGameNum;
unsigned int iSPCH_GetStartSample(unsigned int gameNum, int numSamples, int cycleLength);

int SPCH_GetBankPtrMemSize(int numBanks) {
    return numBanks << 3;
}

void SPCH_InitBankMem(int numBanks, char *bankMem) {
    int i;

    gNumBanks = numBanks;
    gVoxBanks = reinterpret_cast<VoxBankInfo *>(bankMem);
    if (bankMem == 0) {
        return;
    }
    for (i = 0; i < numBanks; i++) {
        reinterpret_cast<VoxBankInfo *>(bankMem)[i].bankHandle = 0;
        reinterpret_cast<VoxBankInfo *>(bankMem)[i].voxHdr = 0;
    }
}

static short iSPCH_FindInsertPosition(VOXBANKHDR *hdr) {
    int start;
    int mid;
    int end;

    start = 0;
    end = gBankCount - 1;
    if (gBankCount == 0) {
        return 0;
    }
    if (start > end) {
        goto abort;
    }
    do {
        mid = (start + end) / 2;
        if (hdr->type == gVoxBanks[mid].voxHdr->type && hdr->subID == gVoxBanks[mid].voxHdr->subID) {
            goto abort;
        }
        if (mid == 0) {
            if ((hdr->type == gVoxBanks[0].voxHdr->type && hdr->subID < gVoxBanks[0].voxHdr->subID) ||
                hdr->type < gVoxBanks[0].voxHdr->type) {
                return static_cast<short>(mid);
            }
        }
        if (hdr->type < gVoxBanks[mid].voxHdr->type ||
            (hdr->type == gVoxBanks[mid].voxHdr->type && hdr->subID < gVoxBanks[mid].voxHdr->subID)) {
            if ((hdr->type == gVoxBanks[mid - 1].voxHdr->type && hdr->subID > gVoxBanks[mid - 1].voxHdr->subID) ||
                hdr->type > gVoxBanks[mid - 1].voxHdr->type) {
                return static_cast<short>(mid);
            }
        }
        if (gVoxBanks[mid].voxHdr->type < hdr->type ||
            (hdr->type == gVoxBanks[mid].voxHdr->type && gVoxBanks[mid].voxHdr->subID < hdr->subID)) {
            start = mid + 1;
            if (gVoxBanks[start].voxHdr == 0) {
                return static_cast<short>(start);
            }
        } else {
            end = mid - 1;
        }
    } while (start <= end);
abort:
    return -1;
}

void iSPCH_SetCycleBits(VOXBANKHDR *bank) {
    unsigned char *data;
    int cycleLength;
    unsigned int gameNum;
    int numSamples;
    int i;
    int numValid;
    unsigned int startSample;
    unsigned int nextStartSample;
    unsigned int cycleByte;
    unsigned int cycleBit;
    unsigned char mask;

    data = BANKHDR_GetCycleBitsAddr(bank);
    cycleLength = data[0];
    if (cycleLength <= 0) {
        goto abort;
    }
    gameNum = gGameNum;
    numSamples = bank->numSamples;
    gameNum = gameNum - (gameNum / cycleLength) * cycleLength;
    startSample = iSPCH_GetStartSample(gameNum, numSamples, cycleLength);
    nextStartSample = iSPCH_GetStartSample(gameNum + 1, numSamples, cycleLength);
    numValid = nextStartSample - startSample;
    i = 0;
    cycleBit = startSample & 7;
    cycleByte = (startSample >> 3) + 1;
    while (i < numValid) {
        mask = 1 << cycleBit;
        data[cycleByte] = data[cycleByte] | mask;
        if (cycleBit++ == 7) {
            cycleBit = 0;
            cycleByte++;
        }
        i++;
    }
abort:
    ;
}

int SPCH_AddBank(char *bankHdr) {
    VOXBANKHDR *hdr;
    int bankIndex;
    int bankHandle;

    bankHandle = -1;
    if (gVoxBanks == 0) {
        goto abort;
    }
    hdr = reinterpret_cast<VOXBANKHDR *>(bankHdr);
    bankIndex = iSPCH_FindInsertPosition(hdr);
    if (bankIndex >= gNumBanks) {
        goto abort;
    }
    if (bankIndex < 0) {
        goto abort;
    }
    if ((hdr->parmFlags & 0x80) != 0) {
        iSPCH_SetCycleBits(hdr);
    }
    for (int j = gBankCount; j > bankIndex; j--) {
        gVoxBanks[j] = gVoxBanks[j - 1];
    }
    bankHandle = gUniqueBankHandle;
    gVoxBanks[bankIndex].voxHdr = hdr;
    gVoxBanks[bankIndex].bankHandle = bankHandle;
    gUniqueBankHandle = bankHandle + 1;
    gBankCount = gBankCount + 1;
abort:
    return bankHandle;
}

short iSPCH_FindBank(unsigned short bankType, int &bankHandle) {
    short bankIndex;
    int start;
    int mid;
    int end;

    start = 0;
    end = gBankCount - 1;
    if (gVoxBanks == 0) {
        goto abort;
    }
    if (gBankCount == 0) {
        goto abort;
    }
    if (start > end) {
        goto abort;
    }
    do {
        mid = (start + end) / 2;
        if (bankType == gVoxBanks[mid].voxHdr->type) {
            bankHandle = gVoxBanks[mid].bankHandle;
            return static_cast<short>(mid);
        }
        if (bankType > gVoxBanks[mid].voxHdr->type) {
            start = mid + 1;
        } else {
            end = mid - 1;
        }
    } while (start <= end);
abort:
    return -1;
}

short iSPCH_FindSubBank(unsigned short bankID, unsigned short subBankID, int &bankHandle) {
    short bankIndex;
    int start;
    int mid;
    int end;

    start = 0;
    end = gBankCount - 1;
    if (gVoxBanks == 0) {
        goto abort;
    }
    if (gBankCount == 0) {
        goto abort;
    }
    if (start > end) {
        goto abort;
    }
    do {
        mid = (start + end) / 2;
        if (bankID == gVoxBanks[mid].voxHdr->type && subBankID == gVoxBanks[mid].voxHdr->subID) {
            bankHandle = gVoxBanks[mid].bankHandle;
            return static_cast<short>(mid);
        }
        if (bankID > gVoxBanks[mid].voxHdr->type ||
            (bankID == gVoxBanks[mid].voxHdr->type && subBankID > gVoxBanks[mid].voxHdr->subID)) {
            start = mid + 1;
        } else {
            end = mid - 1;
        }
    } while (start <= end);
abort:
    return -1;
}

void iSPCHBank_AddToQueue(VOXBANKHDR *hdr, int sampleIndex) {
    unsigned char *queue;
    unsigned char index;

    if (hdr->sampleRepeat == 0) {
        return;
    }
    queue = BANKHDR_GetSampleRepeatAddr(hdr);
    index = queue[0];
    if (index < hdr->sampleRepeat) {
        queue[index + 1] = sampleIndex;
    }
    index++;
    if (index >= hdr->sampleRepeat) {
        index = 0;
    }
    queue[0] = index;
}

int iSPCHBank_GetSampleTimeInQueue(VOXBANKHDR *hdr, int sampleIndex) {
    int i;
    int testIndex;
    unsigned char *queue;
    int numInQueue;
    int age;

    numInQueue = hdr->sampleRepeat;
    age = 0;
    if (numInQueue <= 0) {
        goto done;
    }
    queue = BANKHDR_GetSampleRepeatAddr(hdr);
    testIndex = queue[0];
    queue++;
    i = 0;
    for (; i < numInQueue; i++) {
        testIndex--;
        if (testIndex < 0) {
            testIndex = numInQueue - 1;
        }
        if (queue[testIndex] == sampleIndex) {
            age = i + 1;
            goto done;
        }
    }
done:
    return age;
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

    partitionSize = numSamples / cycleLength;
    overflowSamples = numSamples - partitionSize * cycleLength;
    startSample = gameNum;
    if (startSample <= static_cast<unsigned int>(overflowSamples)) {
        return partitionSize * startSample + startSample;
    }
    return partitionSize * startSample + overflowSamples;
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
