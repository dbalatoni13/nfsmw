#include "DeltaCompressedData.h"
#include <cstring>

namespace EAGL4Anim {

void DeltaCompressedData::DecompressValues(int startDof, int numDofs, int prevFrame, int currFrame, const float *prevValues,
                                           float *currValues) const {
    if (currFrame == prevFrame)
        return;

    const unsigned char *dataBlock = GetDataBlock();
    const DofInfo *dofInfoStart = &reinterpret_cast<const DofInfo *>(dataBlock)[startDof];

    const DofInfo *dofInfo;
    int idof;

    if (currFrame < prevFrame || currFrame == 0 || prevFrame == -1) {
        for (dofInfo = dofInfoStart, idof = 0; idof < numDofs; idof++, dofInfo++) {
            currValues[idof] = dofInfo->mStartValue;
        }
        prevFrame = 0;
    } else if (currValues != prevValues) {
        memcpy(currValues, prevValues, numDofs * sizeof(*prevValues));
    }
    if (currFrame != 0) {
        const unsigned char *deltaBlock = &dataBlock[GetDeltaDataOffset()];
        int iframe;

        switch (mNumQuantBits) {
            case 16: {
                const unsigned short *delta16 = &reinterpret_cast<const unsigned short *>(&deltaBlock[(prevFrame * mNumDofs * 2)])[startDof];
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    for (dofInfo = dofInfoStart, idof = 0; idof < numDofs; idof++, dofInfo++, delta16++) {
                        currValues[idof] += *delta16 * dofInfo->mQuantRange + dofInfo->mQuantMin;
                    }
                }
                break;
            }
            case 8: {
                const unsigned char *delta8 = &(&deltaBlock[prevFrame * mNumDofs])[startDof];
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    for (dofInfo = dofInfoStart, idof = 0; idof < numDofs; idof++, dofInfo++, delta8++) {
                        currValues[idof] += *delta8 * dofInfo->mQuantRange + dofInfo->mQuantMin;
                    }
                }
                break;
            }
            case 4: {
                const unsigned char *delta4 = &(&deltaBlock[prevFrame * ((mNumDofs + 1) >> 1)])[startDof];
                unsigned char quant;
                int endDof = numDofs - 1;
                if (!(startDof & 1)) {
                    for (iframe = prevFrame; iframe < currFrame; iframe++) {
                        dofInfo = dofInfoStart;
                        for (idof = 0; idof < endDof; idof++, dofInfo++, delta4++) {
                            quant = *delta4;
                            currValues[idof] += (quant & 0xf) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                            idof++;
                            dofInfo++;

                            currValues[idof] += (quant >> 4) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                        }
                        if (numDofs & 1) {
                            quant = *delta4;
                            currValues[idof] += (quant & 0xf) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                            delta4++;
                        }
                    }
                } else {
                    for (iframe = prevFrame; iframe < currFrame; iframe++) {
                        idof = 0;
                        dofInfo = dofInfoStart;
                        if (idof < endDof) {
                            quant = *delta4;
                            delta4++;
                            currValues[0] += (quant >> 4) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                            dofInfo++;
                            for (idof = 1; idof < endDof; idof++, dofInfo++, delta4++) {
                                quant = *delta4;

                                currValues[idof] += (quant & 0xf) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                                idof++;
                                dofInfo++;

                                currValues[idof] += (quant >> 4) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                            }
                        }
                        if (endDof & 1) {
                            quant = *delta4;
                            currValues[idof] += (quant & 0xf) * dofInfo->mQuantRange + dofInfo->mQuantMin;
                            delta4++;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

void DeltaCompressedData::DecompressValuesIndexed(int startDof, int numDofs, int prevFrame, int currFrame, const float *prevValues, float *currValues,
                                                  int valuesPerIndex, unsigned short *indices, float scale) const {
    if (currFrame == prevFrame) {
        return;
    }

    const unsigned char *dataBlock = GetDataBlock();
    const DofInfo *dofInfoStart = &reinterpret_cast<const DofInfo *>(dataBlock)[startDof];
    const int numStrides = numDofs / valuesPerIndex;
    const struct DofInfo *dofInfo;
    int istride;
    int ioffset;

    if (currFrame == 0 || prevFrame == -1 || currFrame < prevFrame) {
        dofInfo = dofInfoStart;
        for (istride = 0; istride < numStrides; istride++) {
            int index = indices[istride];
            for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, dofInfo++) {
                currValues[index + ioffset] = dofInfo->mStartValue;
            }
        }
        prevFrame = 0;
    } else {
        int idof = 0;
        for (istride = 0; istride < numStrides; istride++) {
            int index = indices[istride];
            for (ioffset = 0; ioffset < valuesPerIndex; ioffset++) {
                currValues[index + ioffset] = prevValues[idof];
                idof++;
            }
        }
    }

    if (currFrame != 0) {
        const unsigned char *deltaBlock = &dataBlock[GetDeltaDataOffset()];
        int iframe;

        switch (mNumQuantBits) {
            case 16: {
                const unsigned short *delta16 = reinterpret_cast<const unsigned short *>(&deltaBlock[prevFrame * mNumDofs * 2]);
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    dofInfo = dofInfoStart;
                    for (istride = 0; istride < numStrides; istride++) {
                        int index = indices[istride];
                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, delta16++, dofInfo++) {
                            currValues[index + ioffset] += (*delta16 * dofInfo->mQuantRange + dofInfo->mQuantMin) * scale;
                        }
                    }
                }
                break;
            }
            case 8: {
                const unsigned char *delta8 = reinterpret_cast<const unsigned char *>(&deltaBlock[prevFrame * mNumDofs]);
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    dofInfo = dofInfoStart;
                    for (istride = 0; istride < numStrides; istride++) {
                        int index = indices[istride];
                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, delta8++, dofInfo++) {
                            currValues[index + ioffset] += (*delta8 * dofInfo->mQuantRange + dofInfo->mQuantMin) * scale;
                        }
                    }
                }
                break;
            }
            case 4: {
                const unsigned char *delta4 = reinterpret_cast<const unsigned char *>(&deltaBlock[prevFrame * ((mNumDofs + 1) >> 1)]);
                bool dofEven = !(startDof & ~1);
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    dofInfo = dofInfoStart;
                    for (istride = 0; istride < numStrides; istride++) {
                        int index = indices[istride];
                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++) {
                            unsigned char quant = *delta4;
                            if (dofEven) {
                                quant &= 0xf;
                            } else {
                                quant >>= 4;
                                delta4++;
                            }
                            currValues[index + ioffset] += (quant * dofInfo->mQuantRange + dofInfo->mQuantMin) * scale;
                            dofEven = !dofEven;
                            dofInfo++;
                        }
                    }
                    if (!dofEven) {
                        delta4++;
                        dofEven = true;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

void DeltaCompressedData::DecompressValues(int valuesPerIndex, int prevFrame, int currFrame, const float *prevValues, float *currValues,
                                           int numDofMask, const unsigned short *dofMask) const {
    if (currFrame == prevFrame) {
        return;
    }

    const unsigned char *dataBlock = GetDataBlock();
    const DofInfo *dofInfoStart = reinterpret_cast<const DofInfo *>(dataBlock);
    const DofInfo *dofInfo;
    int idof;
    int ioffset;
    int index;

    if (currFrame < prevFrame || currFrame == 0 || prevFrame == -1) {
        for (idof = 0; idof < numDofMask; idof++) {
            index = dofMask[idof] * valuesPerIndex;
            dofInfo = &dofInfoStart[index];
            for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, index++, dofInfo++) {
                currValues[index] = dofInfo->mStartValue;
            }
        }
        prevFrame = 0;
    } else if (currValues != prevValues) {
        for (idof = 0; idof < numDofMask; idof++) {
            index = dofMask[idof] * valuesPerIndex;
            for (ioffset = 0; ioffset < valuesPerIndex; ioffset++) {
                currValues[index] = prevValues[index];
                index++;
            }
        }
    }
    if (currFrame != 0) {
        const unsigned char *deltaBlock = &dataBlock[GetDeltaDataOffset()];
        int iframe;

        switch (mNumQuantBits) {
            case 16: {
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    for (idof = 0; idof < numDofMask; idof++) {
                        index = dofMask[idof] * valuesPerIndex;
                        const unsigned short *delta16 = &reinterpret_cast<const unsigned short *>(&deltaBlock[iframe * mNumDofs * 2])[index];
                        dofInfo = &dofInfoStart[index];

                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, delta16++, dofInfo++) {
                            currValues[index + ioffset] += *delta16 * dofInfo->mQuantRange + dofInfo->mQuantMin;
                        }
                    }
                }
                break;
            }
            case 8: {
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    for (idof = 0; idof < numDofMask; idof++) {
                        index = dofMask[idof] * valuesPerIndex;
                        const unsigned char *delta8 = &(&deltaBlock[iframe * mNumDofs])[index];
                        dofInfo = &dofInfoStart[index];

                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, delta8++, dofInfo++) {
                            currValues[index + ioffset] += *delta8 * dofInfo->mQuantRange + dofInfo->mQuantMin;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

void DeltaCompressedData::DecompressValuesIndexed(int prevFrame, int currFrame, const float *prevValues, float *currValues, int valuesPerIndex,
                                                  unsigned short *indices, float scale, int numDofMask, const unsigned short *dofMask) const {
    if (currFrame == prevFrame) {
        return;
    }

    const unsigned char *dataBlock = GetDataBlock();
    const DofInfo *dofInfoStart = reinterpret_cast<const DofInfo *>(dataBlock);
    const DofInfo *dofInfo;
    int idof;
    int ioffset;
    int index;

    if (currFrame == 0 || prevFrame == -1 || currFrame < prevFrame) {
        for (idof = 0; idof < numDofMask; idof++) {
            index = indices[dofMask[idof]];
            dofInfo = &dofInfoStart[dofMask[idof] * valuesPerIndex];
            for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, index++, dofInfo++) {
                currValues[index] = dofInfo->mStartValue;
            }
        }
        prevFrame = 0;
    } else {
        for (idof = 0; idof < numDofMask; idof++) {
            index = indices[dofMask[idof]];
            for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, index++) {
                currValues[index] = prevValues[dofMask[idof] * valuesPerIndex + ioffset];
            }
        }
    }

    if (currFrame != 0) {
        const unsigned char *deltaBlock = &dataBlock[GetDeltaDataOffset()];
        int iframe;

        switch (mNumQuantBits) {
            case 16: {
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    for (idof = 0; idof < numDofMask; idof++) {
                        index = dofMask[idof] * valuesPerIndex;
                        const unsigned short *delta16 = &reinterpret_cast<const unsigned short *>(&deltaBlock[iframe * mNumDofs * 2])[index];
                        dofInfo = &dofInfoStart[index];

                        index = indices[dofMask[idof]];
                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, index++, dofInfo++, delta16++) {
                            currValues[index] += (*delta16 * dofInfo->mQuantRange + dofInfo->mQuantMin) * scale;
                        }
                    }
                }
                break;
            }
            case 8: {
                for (iframe = prevFrame; iframe < currFrame; iframe++) {
                    for (idof = 0; idof < numDofMask; idof++) {
                        index = dofMask[idof] * valuesPerIndex;
                        const unsigned char *delta8 = &(&deltaBlock[iframe * mNumDofs])[index];
                        dofInfo = &dofInfoStart[index];

                        index = indices[dofMask[idof]];
                        for (ioffset = 0; ioffset < valuesPerIndex; ioffset++, index++, dofInfo++, delta8++) {
                            currValues[index] += (*delta8 * dofInfo->mQuantRange + dofInfo->mQuantMin) * scale;
                        }
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

}; // namespace EAGL4Anim
