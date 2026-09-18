#include "./spchi.h"

// total size: 0x8
struct VoxBankInfo {
    int bankHandle;     // offset 0x0, size 0x4
    VOXBANKHDR *voxHdr; // offset 0x4, size 0x4
};

extern VoxBankInfo *gVoxBanks;
extern int gNumBanks;
extern int gBankCount;
extern int gUniqueBankHandle;
extern unsigned int gGameNum;

static short iSPCH_FindInsertPosition(VOXBANKHDR *hdr);
void iSPCH_SetCycleBits(VOXBANKHDR *bank);

void iSPCH_InitBanks(void) {
    gVoxBanks = 0;
    gNumBanks = 0;
    gBankCount = 0;
    gUniqueBankHandle = 0;
}

int SPCH_GetBankPtrMemSize(int numBanks) {
    return numBanks * 8;
}

void SPCH_InitBankMem(int numBanks, char *bankMem) {
    int i;

    gNumBanks = numBanks;
    gVoxBanks = (VoxBankInfo *)bankMem;
    if (bankMem == 0) {
        return;
    }
    for (i = 0; i < numBanks; i++) {
        gVoxBanks[i].bankHandle = 0;
        gVoxBanks[i].voxHdr = 0;
    }
}

static short iSPCH_FindInsertPosition(VOXBANKHDR *hdr) {
    int start = 0;
    int mid;
    int end = gBankCount - 1;

    if (gBankCount == 0) {
        return 0;
    }

    while (start <= end) {

        mid = (start + end) / 2;

        if (hdr->type == gVoxBanks[mid].voxHdr->type &&
            hdr->subID == gVoxBanks[mid].voxHdr->subID) {
            goto abort;
        }

        if (mid == 0 && ((hdr->type == gVoxBanks[mid].voxHdr->type && hdr->subID < gVoxBanks[mid].voxHdr->subID) || hdr->type < gVoxBanks[mid].voxHdr->type)) {
            return mid;
        }

        if (((hdr->type == gVoxBanks[mid].voxHdr->type && hdr->subID < gVoxBanks[mid].voxHdr->subID) || hdr->type < gVoxBanks[mid].voxHdr->type) && ((hdr->type == gVoxBanks[mid - 1].voxHdr->type && hdr->subID > gVoxBanks[mid - 1].voxHdr->subID) || hdr->type > gVoxBanks[mid - 1].voxHdr->type)) {

            return mid;
        }

        if (hdr->type > gVoxBanks[mid].voxHdr->type || (hdr->type == gVoxBanks[mid].voxHdr->type && hdr->subID > gVoxBanks[mid].voxHdr->subID)) {
            start = mid + 1;
            if (gVoxBanks[start].voxHdr == 0) {
                return start;
            }
        } else {

            end = mid - 1;
        }
    }

abort:
    return -1;
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
    while (start <= end) {
        mid = (start + end) / 2;
        if (bankType == gVoxBanks[mid].voxHdr->type) {
            bankHandle = gVoxBanks[mid].bankHandle;
            return mid;
        }
        if (bankType > gVoxBanks[mid].voxHdr->type) {
            start = mid + 1;
        } else {
            end = mid - 1;
        }
    }
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
    while (start <= end) {
        mid = (start + end) / 2;
        if (bankID == gVoxBanks[mid].voxHdr->type) {
            if (subBankID == gVoxBanks[mid].voxHdr->subID) {
                bankHandle = gVoxBanks[mid].bankHandle;
                return mid;
            }
        }
        if (bankID > gVoxBanks[mid].voxHdr->type ||
            (bankID == gVoxBanks[mid].voxHdr->type && subBankID > gVoxBanks[mid].voxHdr->subID)) {
            start = mid + 1;
        } else {
            end = mid - 1;
        }
    }
abort:
    return -1;
}

int iSPCH_TestSubBankBounds(int bankIndex, unsigned int subBankIndex) {
    int result;

    result = 0;
    if (gVoxBanks != 0) {
        if (bankIndex >= 0) {
            if (gVoxBanks[bankIndex].voxHdr->numSubBanks != 0xFFFF) {
                result = subBankIndex < gVoxBanks[bankIndex].voxHdr->numSubBanks;
            }
        }
    }
    return result;
}

int iSPCH_FindBankIndexFromHandle(int bankHandle) {
    int i;

    for (i = 0; i < gBankCount; i++) {
        if (gVoxBanks[i].bankHandle == bankHandle) {
            return i;
        }
    }
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
    index = index + 1;
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

    age = 0;
    numInQueue = hdr->sampleRepeat;
    if (numInQueue <= 0) {
        goto done;
    }
    queue = BANKHDR_GetSampleRepeatAddr(hdr);
    testIndex = queue[0];
    queue = queue + 1;
    i = 0;
    while (i < numInQueue) {
        testIndex = testIndex - 1;
        if (testIndex < 0) {
            testIndex = numInQueue - 1;
        }
        if (queue[testIndex] == sampleIndex) {
            age = i + 1;
            goto done;
        }
        i = i + 1;
    }
done:
    return age;
}

unsigned int iSPCH_GetStartSample(unsigned int gameNum, int numSamples, int cycleLength) {
    int partitionSize;
    int overflowSamples;
    unsigned int startSample;

    partitionSize = numSamples / cycleLength;
    overflowSamples = numSamples - partitionSize * cycleLength;
    if (gameNum > overflowSamples) {
        startSample = partitionSize * gameNum + overflowSamples;
    } else {
        startSample = partitionSize * gameNum + gameNum;
    }
    return startSample;
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

    numSamples = bank->numSamples;
    data = BANKHDR_GetCycleBitsAddr(bank);
    cycleLength = *data;
    if (cycleLength <= 0) {
        return;
    }
    gameNum = gGameNum % cycleLength;
    startSample = iSPCH_GetStartSample(gameNum, numSamples, cycleLength);
    nextStartSample = iSPCH_GetStartSample(gameNum + 1, numSamples, cycleLength);
    numValid = nextStartSample - startSample;
    cycleByte = startSample / 8 + 1;
    i = 0;
    cycleBit = startSample % 8;
    while (i < numValid) {
        mask = 1 << cycleBit;
        data[cycleByte] = data[cycleByte] | mask;
        cycleBit = cycleBit + 1;
        if (cycleBit == 8) {
            cycleBit = 0;
            cycleByte = cycleByte + 1;
        }
        i = i + 1;
    }
}

int SPCH_AddBank(char *bankHdr) {
    VOXBANKHDR *hdr;
    int bankIndex;
    int bankHandle;
    int j;

    hdr = (VOXBANKHDR *)bankHdr;
    bankHandle = -1;
    if (gVoxBanks == 0) {
        goto abort;
    }
    bankIndex = iSPCH_FindInsertPosition(hdr);
    if (bankIndex >= gNumBanks) {
        goto abort;
    }
    if (bankIndex < 0) {
        goto abort;
    }
    if (hdr->parmFlags & 0x80) {
        iSPCH_SetCycleBits(hdr);
    }
    for (j = gBankCount; j > bankIndex; j--) {
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
