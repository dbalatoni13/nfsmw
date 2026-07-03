#ifndef FEOBJECTSORTER_H_
#define FEOBJECTSORTER_H_

#include <types.h>
#include "FEObject.h"
#include "FEngStandard.h"

// File: speed/indep/src/feng/FEObjectSorter.h
// total size: 0x8
// Decl: speed/indep/src/feng/FEObjectSorter.h:24
struct SFERadixKey {
    FEObject *pobObject; // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEObjectSorter.h:25
    u32 ulKey;           // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEObjectSorter.h:26
};

// total size: 0x4004
// Decl: speed/indep/src/feng/FEObjectSorter.h:34
template <int N> class FEObjectSorter {
  private:
    u32 mulNumObjects;              // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEObjectSorter.h:36
    SFERadixKey mastFinalList[N];   // offset 0x4, size 0x2000, Decl: speed/indep/src/feng/FEObjectSorter.h:37
    SFERadixKey mastScratchList[N]; // offset 0x2004, size 0x2000, Decl: speed/indep/src/feng/FEObjectSorter.h:38

  public:
    FEObjectSorter() {} // Decl: speed/indep/src/feng/FEObjectSorter.h:41

    void Zero() {} // Decl: speed/indep/src/feng/FEObjectSorter.h:46

    void AddObject(FEObject *pobObject, float fZValue) {} // Decl: speed/indep/src/feng/FEObjectSorter.h:51

    u32 GetNumObjects() {} // Decl: speed/indep/src/feng/FEObjectSorter.h:59

    SFERadixKey *GetListPtr() {} // Decl: speed/indep/src/feng/FEObjectSorter.h:70

    void SortObjects() { // Decl: speed/indep/src/feng/FEObjectSorter.h:81
        int lNumBytes = mulNumObjects << 3;
        SFERadixKey *pstDestList = mastScratchList;
        SFERadixKey *pstSrcList = mastFinalList;
        int b = 3;
        do {
            long alElemCount[256];
            FEngMemSet(alElemCount, 0, sizeof(alElemCount));
            int byteOffset = b + 4;
            int nextB = b - 1;
            unsigned char *pucByte = reinterpret_cast<unsigned char *>(pstSrcList) + byteOffset;
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
                SFERadixKey *pOut = pstDestList + alElemIndex[ucIndex];
                pOut->pobObject = pstSrcList[k].pobObject;
                pOut->ulKey = pstSrcList[k].ulKey;
                alElemIndex[ucIndex]++;
            }
            SFERadixKey *pstTemp = pstSrcList;
            pstSrcList = pstDestList;
            pstDestList = pstTemp;
            b = nextB;
        } while (b >= 0);
    };
};

#endif
