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
    sampleParmData = reinterpret_cast<unsigned char *>(hdr) + numParms * sampleIndex + 0xE;
abort:
    return sampleParmData;
}
