#ifndef FENG_FEOBJECTSORTER_H
#define FENG_FEOBJECTSORTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEngStandard.h"

template <int N>
void FEObjectSorter<N>::SortObjects() {
    int lNumBytes = mulNumObjects << 3;
    SFERadixKey* pstDestList = mastScratchList;
    SFERadixKey* pstSrcList = mastFinalList;
    int b = 3;
    do {
        long alElemCount[256];
        FEngMemSet(alElemCount, 0, sizeof(alElemCount));
        int byteOffset = b + 4;
        int nextB = b - 1;
        unsigned char* pucByte = reinterpret_cast<unsigned char*>(pstSrcList) + byteOffset;
        int i = 0;
        if (i < lNumBytes) {
            do {
                unsigned char ucIndex = pucByte[i];
                i += 8;
                alElemCount[ucIndex]++;
            } while (i < lNumBytes);
        }
        long alElemIndex[256];
        alElemIndex[0] = 0;
        for (int j = 0; j < 255; j++) {
            alElemIndex[j + 1] = alElemIndex[j] + alElemCount[j];
        }
        for (int k = 0; k < static_cast<int>(mulNumObjects); k++) {
            unsigned char ucIndex = pucByte[k * 8];
            SFERadixKey* pOut = pstDestList + alElemIndex[ucIndex];
            pOut->pobObject = pstSrcList[k].pobObject;
            pOut->ulKey = pstSrcList[k].ulKey;
            alElemIndex[ucIndex]++;
        }
        SFERadixKey* pstTemp = pstSrcList;
        pstSrcList = pstDestList;
        pstDestList = pstTemp;
        b = nextB;
    } while (b >= 0);
}

#endif
