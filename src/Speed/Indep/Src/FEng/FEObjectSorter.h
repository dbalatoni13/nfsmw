#ifndef FENG_FEOBJECTSORTER_H
#define FENG_FEOBJECTSORTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEngStandard.h"

template <int N>
void FEObjectSorter<N>::SortObjects() {
    int pass = 3;
    int count = mulNumObjects;
    SFERadixKey* pSrc = mastFinalList;
    SFERadixKey* pDst = mastScratchList;
    do {
        SFERadixKey* pDstCur = pDst;
        unsigned long histogram[256];
        FEngMemSet(histogram, 0, sizeof(histogram));
        int byteOffset = pass + 4;
        pass--;
        int i = 0;
        if (count * 8 > 0) {
            do {
                unsigned char b = reinterpret_cast<unsigned char*>(pSrc)[i + byteOffset];
                i += 8;
                histogram[b]++;
            } while (i < count * 8);
        }
        unsigned long offsets[256];
        offsets[0] = 0;
        for (int j = 0; j < 255; j++) {
            offsets[j + 1] = offsets[j] + histogram[j];
        }
        int numObj = mulNumObjects;
        for (int k = 0; k < numObj; k++) {
            unsigned char b = reinterpret_cast<unsigned char*>(pSrc)[k * 8 + byteOffset];
            SFERadixKey* pOut = pDstCur + offsets[b];
            pOut->pObject = pSrc[k].pObject;
            pOut->fZValue = pSrc[k].fZValue;
            offsets[b]++;
        }
        pDst = pSrc;
        pSrc = pDstCur;
    } while (pass > -1);
}

#endif
