#include "spch/spch.h"

unsigned char *iSPCH_GetSampleParmAddr(VOXBANKHDR *hdr, int sampleIndex) {
    int sampleSize;
    int numParms;
    unsigned char *sampleParmData;

    sampleParmData = 0;

    if (sampleIndex >= hdr->numSamples) {
        goto abort;
    }
    sampleSize = hdr->parmFlags & 0x7F;
    numParms = sampleSize + 2;
    sampleParmData = reinterpret_cast<unsigned char *>(hdr);
    sampleParmData += sampleIndex * numParms;
    sampleParmData += 0xE;
abort:
    return sampleParmData;
}

int iSPCH_GetSampleSizeData(VOXBANKHDR *hdr, int sampleIndex, unsigned int *sampleOffset, unsigned int *dataBytes) {
    unsigned int offset;
    int sampleSize;
    unsigned char *sampleData;
    int result;
    int numParms;
    int blockSize;
    int nextIndex;
    unsigned int endOffset;
    unsigned char *nextSampleData;

    result = 0;
    if (sampleIndex >= hdr->numSamples) {
        goto abort;
    }
    sampleSize = hdr->parmFlags & 0x7F;
    numParms = sampleSize + 2;
    blockSize = (hdr->blockSize + 1) << 8;
    sampleData = reinterpret_cast<unsigned char *>(&hdr[1]);
    sampleData += sampleIndex * numParms;
    offset = (sampleData[0] << 8) + sampleData[1];
    offset *= blockSize;
    nextIndex = sampleIndex + 1;
    nextSampleData = sampleData + numParms;
    if (nextIndex >= hdr->numSamples) {
        endOffset = hdr->bankBlocks;
    } else {
        endOffset = (nextSampleData[0] << 8) + nextSampleData[1];
    }
    endOffset *= blockSize;
    *sampleOffset = offset;
    *dataBytes = endOffset - offset;
    result = 1;
abort:
    return result;
}
