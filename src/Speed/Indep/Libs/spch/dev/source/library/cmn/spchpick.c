#include "spch/spch.h"

extern int gPreLoadTicks;

static EventChoice gEventChoice[8];

static unsigned int multiple[8] = {
    1,
    4,
    0x10,
    0x40,
    0x100,
    0x400,
    0x1000,
    0x4000,
};

void SPCH_GetEventDatInfo(char *eventData, int *projID, int *datID) {
    *projID = static_cast<unsigned char>(eventData[8]);
    *datID = static_cast<unsigned char>(eventData[9]);
}

static int iSPCH_TestBit(UInt8 *bitArray, int bitIndex) {
    UInt8 mask;
    int byteIndex;
    int bit;
    int result;

    byteIndex = bitIndex;
    if (bitIndex < 0) {
        byteIndex += 7;
    }
    byteIndex >>= 3;
    bit = bitIndex - byteIndex * 8;
    mask = 1 << bit;
    result = bitArray[byteIndex] & mask;
    return result;
}

static int iSPCH_ConvertTime(int dataBytes) {
    int centiSecs;

    centiSecs = 0;
    if (gDataRate == 0) {
        goto abort;
    }
    centiSecs = dataBytes * 100 / gDataRate;
abort:
    ;
    return centiSecs;
}

unsigned int iSPCH_DecodeWeight(unsigned char weight) {
    unsigned int exp;
    unsigned int mant;

    exp = weight >> 5;
    mant = weight & 0x1F;
    unsigned int num;
    num = multiple[exp];
    return mant * num;
}

int iSPCH_OneChosen(unsigned int inChannel) {
    int validChoice;

    validChoice = gEventChoice[inChannel].validChoice;
    return validChoice;
}

void SPCH_SetPreLoadTicks(int ticks) {
    gPreLoadTicks = ticks;
}
