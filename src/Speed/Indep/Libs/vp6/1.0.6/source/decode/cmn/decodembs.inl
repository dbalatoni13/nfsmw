#ifndef VP6_DECODEMBS_INL
#define VP6_DECODEMBS_INL

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

extern void VP6_ReconstructBlock(struct PB_INSTANCE *, BLOCK_POSITION);

static const unsigned int VP6_HuffTokenMinVal[12] = {
    0, 1, 2, 3, 4, 5, 7, 11,
    19, 35, 67, 0
};
static const int VP6_CoeffToHuffBand[65] = {
    -1, 0, 1, 1, 1, 2, 2, 2,
    2, 2, 2, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3,
    3
};
static const unsigned int loMaskTbl_VP60[33] = {
    0x00000000u, 0x00000001u, 0x00000003u, 0x00000007u, 0x0000000fu, 0x0000001fu, 0x0000003fu, 0x0000007fu,
    0x000000ffu, 0x000001ffu, 0x000003ffu, 0x000007ffu, 0x00000fffu, 0x00001fffu, 0x00003fffu, 0x00007fffu,
    0x0000ffffu, 0x0001ffffu, 0x0003ffffu, 0x0007ffffu, 0x000fffffu, 0x001fffffu, 0x003fffffu, 0x007fffffu,
    0x00ffffffu, 0x01ffffffu, 0x03ffffffu, 0x07ffffffu, 0x0fffffffu, 0x1fffffffu, 0x3fffffffu, 0x7fffffffu,
    0xffffffffu
};
struct HUFF_TABLE_NODE {
    unsigned short length : 4;
    unsigned short unused : 6;
    unsigned short value : 5;
    unsigned short flag : 1;
};
extern const unsigned char VP6_QTableSelect[6];
extern void (*idct[65])(short *, short *, short *);
extern unsigned char VP6_ReadTokensPredictA(struct PB_INSTANCE *, short *, unsigned int,
    __typeof__(((struct PB_INSTANCE *)0)->mbi.Above),
    __typeof__(((struct PB_INSTANCE *)0)->mbi.Left));
extern void *memset();


#define VP6_READ_WORD(br) (((unsigned int)(br)->position[0] << 24) + \
                          ((unsigned int)(br)->position[1] << 16) + \
                          ((unsigned int)(br)->position[2] << 8) + (br)->position[3])

static inline unsigned int bitreadonly(struct _BITREADER *br, unsigned int bits) {
    unsigned int x;
    unsigned int z;

    x = br->bitsinremainder;
    z = (1 << x);
    z--;
    z &= br->remainder;
    if (x >= bits) {
        return z >> (x - bits);
    }
    z = (z << 8) | *br->position;
    return z >> (x + 8 - bits);
}

static inline void bitShift(struct _BITREADER *br, int bits) {
    br->bitsinremainder -= bits;
    if (br->bitsinremainder < 0) {
        br->remainder = VP6_READ_WORD(br);
        br->position += 4;
        br->bitsinremainder += 32;
    }
}

static inline unsigned int bitread1(struct _BITREADER *br) {
    if (br->bitsinremainder != 0) {
        br->bitsinremainder--;
        return (br->remainder >> br->bitsinremainder) & 1;
    }
    br->remainder = VP6_READ_WORD(br);
    br->position += 4;
    br->bitsinremainder = 31;
    return br->remainder >> 31;
}

static inline unsigned int bitread(struct _BITREADER *br, int bits) {
    unsigned int z;

    z = 0;
    br->remainder &= loMaskTbl_VP60[br->bitsinremainder];
    bits -= br->bitsinremainder;
    if (bits > 0) {
        z = br->remainder << bits;
        br->remainder = VP6_READ_WORD(br);
        br->position += 4;
        bits -= 32;
    }
    br->bitsinremainder = -bits;
    return z | (br->remainder >> br->bitsinremainder);
}

static inline int VP6_ExtractTokenN(struct _BITREADER *br, struct _huffnode *hn,
                                    unsigned short *hlt) {
    struct _tokenorptr torp;
    struct HUFF_TABLE_NODE *htptr;
    unsigned int x;

    htptr = (struct HUFF_TABLE_NODE *)hlt;
    x = bitreadonly(br, 6);
    bitShift(br, htptr[x].length);
    if (htptr[x].flag) {
        return htptr[x].value;
    }
    torp.value = htptr[x].value;
    do {
        if (bitread1(br)) {
            torp = hn[torp.value].rightunion.right;
        } else {
            torp = hn[torp.value].leftunion.left;
        }
    } while (!torp.selector);
    return torp.value;
}

static inline unsigned char ReadHuffTokensPredictA(struct PB_INSTANCE *pbi,
                                                    short *CoeffData, unsigned int Plane) {
    struct _BITREADER *br;
    int SignBit;
    unsigned int Prec;
    int EncodedCoeffs;
    int token;
    int value;

    br = &pbi->br3;
    EncodedCoeffs = 1;
    if (pbi->CurrentDcRunLen[Plane] > 0) {
        pbi->CurrentDcRunLen[Plane]--;
        Prec = 0;
    } else {
        token = VP6_ExtractTokenN(br, pbi->DcHuffTree[Plane], pbi->DcHuffLUT[Plane]);
        value = VP6_HuffTokenMinVal[token];
        if (token == 11) {
            goto Finished;
        }
        if (token == 0) {
            unsigned int val;

            val = bitread(br, 2) + 1;
            if (val == 3) {
                val = bitread(br, 2) + 3;
            } else if (val == 4) {
                if (bitread1(br)) {
                    val = bitread(br, 6) + 11;
                } else {
                    val = bitread(br, 2) + 7;
                }
            }
            pbi->CurrentDcRunLen[Plane] = val - 1;
            Prec = 0;
        } else {
            if (token < 5) {
                SignBit = bitread1(br);
            } else if (token < 10) {
                value += bitread(br, token - 4);
                SignBit = bitread1(br);
            } else {
                value += bitread(br, 11);
                SignBit = bitread1(br);
            }
            CoeffData[0] = (value ^ -SignBit) + SignBit;
            Prec = 1;
            if (value > 1) {
                Prec = 2;
            }
        }
    }
    if (pbi->CurrentAc1RunLen[Plane] > 0) {
        pbi->CurrentAc1RunLen[Plane]--;
    } else {
        do {
            unsigned int Band;

            Band = VP6_CoeffToHuffBand[EncodedCoeffs];
            token = VP6_ExtractTokenN(br, pbi->AcHuffTree[Prec][Plane][Band],
                                     pbi->AcHuffLUT[Prec][Plane][Band]);
            value = VP6_HuffTokenMinVal[token];
            if (token == 0) {
                unsigned int ZrlBand;
                unsigned int ZrlToken;

                ZrlBand = EncodedCoeffs > 5;
                ZrlToken = VP6_ExtractTokenN(br, pbi->ZeroHuffTree[ZrlBand],
                                            pbi->ZeroHuffLUT[ZrlBand]);
                if (ZrlToken < 8) {
                    EncodedCoeffs += ZrlToken;
                } else {
                    EncodedCoeffs += 8 + bitread(br, 6);
                }
                Prec = 0;
            } else if (token == 11) {
                if (EncodedCoeffs == 1) {
                    unsigned int val;

                    val = bitread(br, 2) + 1;
                    if (val == 3) {
                        val = bitread(br, 2) + 3;
                    } else if (val == 4) {
                        if (bitread1(br)) {
                            val = bitread(br, 6) + 11;
                        } else {
                            val = bitread(br, 2) + 7;
                        }
                    }
                    pbi->CurrentAc1RunLen[Plane] = val - 1;
                }
                break;
            } else {
                if (token < 5) {
                    SignBit = bitread1(br);
                } else if (token < 10) {
                    value += bitread(br, token - 4);
                    SignBit = bitread1(br);
                } else {
                    value += bitread(br, 11);
                    SignBit = bitread1(br);
                }
                CoeffData[pbi->MergedScanOrder[EncodedCoeffs]] = (value ^ -SignBit) + SignBit;
                Prec = 1;
                if (value > 1) {
                    Prec = 2;
                }
            }
            EncodedCoeffs++;
        } while (EncodedCoeffs < 64);
    }
Finished:
    return pbi->EobOffsetTable[EncodedCoeffs - 1];
}

static inline void VP6_PredictDC(struct PB_INSTANCE *pbi, BLOCK_POSITION bp, short *LastDC,
                                  __typeof__(((struct PB_INSTANCE *)0)->mbi.Above) Above,
                                  __typeof__(((struct PB_INSTANCE *)0)->mbi.Left) Left) {
    unsigned char Frame;
    unsigned char Count;
    int Avg;

    Count = 0;
    Avg = 0;
    Frame = VP6_Mode2Frame[pbi->mbi.Mode];
    if (Frame == Left->Frame) {
        Avg = Left->Dc;
        Count++;
    }
    if (Frame == Above->Frame) {
        Avg += Above->Dc;
        Count++;
    }
    if (Count == 0) {
        Avg = LastDC[Frame];
    }
    if (Count == 2) {
        Avg += (Avg >> 15) & 1;
        Avg >>= 1;
    }
    pbi->mbi.Coeffs[bp][0] += Avg;
    LastDC[Frame] = pbi->mbi.Coeffs[bp][0];
}

/* NON_MATCHING: the Huffman path is reconstructed from retail tables and
 * control flow, but its inline register lifetimes still differ in ASM/DWARF.
 * Keep the ordinary combined verification gate; do not count this as exact.
 */
void VP6_DecodeBlock(struct PB_INSTANCE *pbi, unsigned int MBrow,
                      unsigned int MBcol, BLOCK_POSITION bp) {
    __typeof__(pbi->mbi) *mbi;
    int EOBPos;

    mbi = &pbi->mbi;
    if (pbi->UseHuffman) {
        EOBPos = ReadHuffTokensPredictA(pbi, mbi->Coeffs[bp], mbi->Plane != 0);
    } else {
        EOBPos = VP6_ReadTokensPredictA(pbi, mbi->Coeffs[bp], mbi->Plane != 0, mbi->Above, mbi->Left);
    }
    VP6_PredictDC(pbi, bp, mbi->LastDc, mbi->Above, mbi->Left);
    mbi->Above->Mode = mbi->Left->Mode = mbi->BlockMode[bp];
    mbi->Above->Dc = mbi->Left->Dc = mbi->Coeffs[bp][0];
    mbi->Above->Frame = mbi->Left->Frame = VP6_Mode2Frame[mbi->Mode];
    if (EOBPos <= 1) {
        idct[1](mbi->Coeffs[bp], pbi->quantizer->dequant_coeffs[VP6_QTableSelect[bp]], pbi->ReconDataBuffer);
        mbi->Coeffs[bp][0] = 0;
    } else if (EOBPos <= 10) {
        idct[9](mbi->Coeffs[bp], pbi->quantizer->dequant_coeffs[VP6_QTableSelect[bp]], pbi->ReconDataBuffer);
        memset(mbi->Coeffs[bp], 0, 16);
        memset(mbi->Coeffs[bp] + 8, 0, 8);
        memset(mbi->Coeffs[bp] + 16, 0, 8);
        memset(mbi->Coeffs[bp] + 24, 0, 8);
        mbi->Coeffs[bp][32] = 0;
    } else {
        idct[63](mbi->Coeffs[bp], pbi->quantizer->dequant_coeffs[VP6_QTableSelect[bp]], pbi->ReconDataBuffer);
        memset(mbi->Coeffs[bp], 0, 128);
    }
    VP6_ReconstructBlock(pbi, bp);
}
#undef VP6_READ_WORD

void VP6_DecodeMacroBlock(struct PB_INSTANCE *pbi, unsigned int MBrow,
                          unsigned int MBcol) {
    unsigned int MBPointer;
    int NextBlock;

    if (pbi->Configuration.Interlaced != 0) {
        {
            unsigned char prob;

            prob = pbi->probInterlaced;
            if (MBcol > 3) {
                if (pbi->mbi.Interlaced != 0) {
                    prob -= prob >> 1;
                } else {
                    prob += (256 - prob) >> 1;
                }
            }
            pbi->mbi.Interlaced = nDecodeBool(&pbi->br, prob);
        }
    } else {
        pbi->mbi.Interlaced = 0;
    }

    if (pbi->FrameType == 0) {
        pbi->mbi.Mode = CODE_INTRA;
    } else {
        VP6_decodeModeAndMotionVector(pbi, MBrow, MBcol);
    }

    if (pbi->mbi.Interlaced == 0) {
        pbi->mbi.CurrentReconStride = pbi->Configuration.YStride;
        NextBlock = 8;
    } else {
        pbi->mbi.CurrentReconStride = pbi->Configuration.YStride * 2;
        NextBlock = 1;
    }

    pbi->mbi.MvShift = 2;
    pbi->mbi.LastDc = pbi->fc.LastDcY;
    pbi->mbi.Plane = 0;
    pbi->mbi.MvModMask = 3;
    pbi->mbi.FrameReconStride = pbi->Configuration.YStride;
    pbi->mbi.SourceY = MBrow << 4;
    pbi->mbi.SourceX = MBcol << 4;
    MBPointer = pbi->ReconYDataOffset + pbi->mbi.SourceY * pbi->mbi.FrameReconStride + pbi->mbi.SourceX;

    pbi->mbi.Recon = MBPointer;
    pbi->mbi.Above = (void *)&pbi->fc.AboveY[MBcol * 2];
    pbi->mbi.Left = (void *)&pbi->fc.LeftY[0];
    VP6_DecodeBlock(pbi, MBrow, MBcol, TOP_LEFT_Y_BLOCK);

    pbi->mbi.Recon += 8;
    pbi->mbi.Above = (void *)&pbi->fc.AboveY[MBcol * 2 + 1];
    pbi->mbi.Left = (void *)&pbi->fc.LeftY[0];
    pbi->mbi.SourceX += 8;
    VP6_DecodeBlock(pbi, MBrow, MBcol, TOP_RIGHT_Y_BLOCK);

    pbi->mbi.Recon = MBPointer += NextBlock * pbi->Configuration.YStride;
    pbi->mbi.Above = (void *)&pbi->fc.AboveY[MBcol * 2];
    pbi->mbi.Left = (void *)&pbi->fc.LeftY[1];
    pbi->mbi.SourceX -= 8;
    pbi->mbi.SourceY += NextBlock;
    VP6_DecodeBlock(pbi, MBrow, MBcol, BOTTOM_LEFT_Y_BLOCK);

    pbi->mbi.Recon += 8;
    pbi->mbi.Above = (void *)&pbi->fc.AboveY[MBcol * 2 + 1];
    pbi->mbi.Left = (void *)&pbi->fc.LeftY[1];
    pbi->mbi.SourceX += 8;
    VP6_DecodeBlock(pbi, MBrow, MBcol, BOTTOM_RIGHT_Y_BLOCK);

    pbi->mbi.FrameReconStride = pbi->Configuration.UVStride;
    pbi->mbi.CurrentReconStride = pbi->Configuration.UVStride;
    pbi->mbi.SourceY = MBrow << 3;
    pbi->mbi.SourceX = MBcol << 3;
    pbi->mbi.MvShift = 3;
    pbi->mbi.MvModMask = 7;

    pbi->mbi.Recon = pbi->ReconUDataOffset + pbi->mbi.SourceY * pbi->mbi.CurrentReconStride + pbi->mbi.SourceX;
    pbi->mbi.Above = (void *)&pbi->fc.AboveU[MBcol];
    pbi->mbi.Left = (void *)&pbi->fc.LeftU;
    pbi->mbi.LastDc = pbi->fc.LastDcU;
    pbi->mbi.Plane = 1;
    VP6_DecodeBlock(pbi, MBrow, MBcol, U_BLOCK);

    pbi->mbi.Above = (void *)&pbi->fc.AboveV[MBcol];
    pbi->mbi.Left = (void *)&pbi->fc.LeftV;
    pbi->mbi.Recon = pbi->ReconVDataOffset + pbi->mbi.SourceY * pbi->mbi.CurrentReconStride + pbi->mbi.SourceX;
    pbi->mbi.LastDc = pbi->fc.LastDcV;
    pbi->mbi.Plane = 2;
    VP6_DecodeBlock(pbi, MBrow, MBcol, V_BLOCK);
}

#endif
