#include "../../../include/vp6_pbdll.h"

extern int VP6_DecodeBool128(BOOL_CODER *br);

inline unsigned int VP6_bitread(BOOL_CODER *br, int bits) {
    unsigned int z = 0;
    int bit;

    for (bit = bits - 1; bit >= 0; bit--) {
        z |= VP6_DecodeBool128(br) << bit;
    }
    return z;
}

typedef struct FRAME_HEADER FRAME_HEADER;

void InitHeaderBuffer(FRAME_HEADER *Header, unsigned char *Buffer) {
    Header->buffer = Buffer;
    Header->value = (Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3];
    Header->bits_available = 32;
    Header->pos = 4;
}

unsigned int ReadHeaderBits(FRAME_HEADER *Header, unsigned int BitsRequired) {
    unsigned int pos = Header->pos;
    unsigned int available = Header->bits_available;
    unsigned int value = Header->value;
    unsigned char *Buffer = &Header->buffer[pos];
    unsigned int RetVal = 0;

    if (available < BitsRequired) {
        RetVal = value >> (32 - available);
        BitsRequired -= available;
        RetVal <<= BitsRequired;

        value = (Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3];
        pos += 4;
        available = 32;
    }

    RetVal |= value >> (32 - BitsRequired);
    Header->value = value << BitsRequired;
    Header->bits_available = available - BitsRequired;
    Header->pos = pos;

    return RetVal;
}

extern void VP6_InitQTables(void *qi, unsigned char Vp3VersionNo);
extern void VP6_StartDecode(BOOL_CODER *br, unsigned char *source);
extern int VP6_DecodeBool(BOOL_CODER *br, int probability);
extern int VP6_InitFrameDetails(struct PB_INSTANCE *pbi);
extern void SetPPInterlacedMode(struct POSTPROC_INSTANCE *ppi, int Interlaced);
extern void SetDeInterlaceMode(struct POSTPROC_INSTANCE *ppi, int DeInterlaceMode);
extern void VP6_UpdateQ(void *qi, unsigned char Vp3VersionNo);
extern void *memset(void *dest, int value, unsigned int size);

static int LoadFrameHeader(struct PB_INSTANCE *pbi) {
    unsigned char DctQMask;
    FRAME_HEADER *Header;
    int RetVal;

    Header = &pbi->Header;
    pbi->FrameType = (unsigned char)ReadHeaderBits(Header, 1);
    DctQMask = (unsigned char)ReadHeaderBits(Header, 6);
    pbi->MultiStream = (unsigned char)ReadHeaderBits(Header, 1);

    if (pbi->FrameType == 0) {
        pbi->Vp3VersionNo = (unsigned char)ReadHeaderBits(Header, 5);
        pbi->VpProfile = (unsigned char)ReadHeaderBits(Header, 2);
        if (pbi->Vp3VersionNo > 7) {
            RetVal = 0;
            return RetVal;
        }

        VP6_InitQTables((void *)pbi->quantizer, pbi->Vp3VersionNo);
        pbi->Configuration.Interlaced = (unsigned char)ReadHeaderBits(Header, 1);
        if (pbi->MultiStream || pbi->VpProfile == 0) {
            VP6_StartDecode((BOOL_CODER *)&pbi->br, Header->buffer + 4);
            pbi->Buff2Offset = ReadHeaderBits(Header, 16);
        } else {
            VP6_StartDecode((BOOL_CODER *)&pbi->br, Header->buffer + 2);
        }

        SetPPInterlacedMode(pbi->postproc, pbi->Configuration.Interlaced);
        if (pbi->Configuration.Interlaced != 0) {
            SetDeInterlaceMode(pbi->postproc, pbi->DeInterlaceMode);
        }

        {
            unsigned int HFragments;
            unsigned int VFragments;
            unsigned int HOldScaled;
            unsigned int VOldScaled;
            unsigned int HNewScaled;
            unsigned int VNewScaled;
            unsigned int OutputHFragments;
            unsigned int OutputVFragments;

            VFragments = (unsigned char)VP6_bitread((BOOL_CODER *)&pbi->br, 8) << 1;
            HFragments = (unsigned char)VP6_bitread((BOOL_CODER *)&pbi->br, 8) << 1;
            OutputVFragments = (unsigned char)VP6_bitread((BOOL_CODER *)&pbi->br, 8) << 1;
            OutputHFragments = (unsigned char)VP6_bitread((BOOL_CODER *)&pbi->br, 8) << 1;

            if (pbi->Configuration.HRatio == 0) {
                pbi->Configuration.HRatio = 1;
            }
            if (pbi->Configuration.VRatio == 0) {
                pbi->Configuration.VRatio = 1;
            }

            HOldScaled = (pbi->Configuration.HScale * pbi->HFragments << 3) / pbi->Configuration.HRatio;
            VOldScaled = (pbi->Configuration.VScale * pbi->VFragments << 3) / pbi->Configuration.VRatio;

            pbi->Configuration.ExpandedFrameWidth = OutputHFragments << 3;
            pbi->Configuration.ExpandedFrameHeight = OutputVFragments << 3;

            if (VFragments >= OutputVFragments) {
                pbi->Configuration.VScale = 1;
                pbi->Configuration.VRatio = 1;
            } else if (5 * VFragments >= 4 * OutputVFragments) {
                pbi->Configuration.VScale = 5;
                pbi->Configuration.VRatio = 4;
            } else if (5 * VFragments >= 3 * OutputVFragments) {
                pbi->Configuration.VScale = 5;
                pbi->Configuration.VRatio = 3;
            } else {
                pbi->Configuration.VScale = 2;
                pbi->Configuration.VRatio = 1;
            }

            if (HFragments >= OutputHFragments) {
                pbi->Configuration.HScale = 1;
                pbi->Configuration.HRatio = 1;
            } else if (5 * HFragments >= 4 * OutputHFragments) {
                pbi->Configuration.HScale = 5;
                pbi->Configuration.HRatio = 4;
            } else if (5 * HFragments >= 3 * OutputHFragments) {
                pbi->Configuration.HScale = 5;
                pbi->Configuration.HRatio = 3;
            } else {
                pbi->Configuration.HScale = 2;
                pbi->Configuration.HRatio = 1;
            }

            HNewScaled = (pbi->Configuration.HScale * HFragments << 3) / pbi->Configuration.HRatio;
            VNewScaled = (pbi->Configuration.VScale * VFragments << 3) / pbi->Configuration.VRatio;
            pbi->ScaleWidth = HNewScaled;
            pbi->ScaleHeight = VNewScaled;
            pbi->Configuration.ScalingMode = VP6_bitread((BOOL_CODER *)&pbi->br, 2);

            if (VFragments != pbi->VFragments || HFragments != pbi->HFragments) {
                pbi->Configuration.VideoFrameWidth = HFragments << 3;
                pbi->Configuration.VideoFrameHeight = VFragments << 3;
                VP6_InitFrameDetails(pbi);
            }

            if (pbi->ScaleBuffer != 0 && (HOldScaled != HNewScaled || VOldScaled != VNewScaled)) {
                memset(pbi->ScaleBuffer, 0, (pbi->OutputWidth + 0x20) * (pbi->OutputHeight + 0x20));
                memset(pbi->ScaleBuffer + (pbi->OutputWidth + 0x20) * (pbi->OutputHeight + 0x20), 0x80,
                       ((pbi->OutputWidth + 0x20) * (pbi->OutputHeight + 0x20)) >> 1);
            }
        }

        if (pbi->VpProfile != 0) {
            if (VP6_DecodeBool((BOOL_CODER *)&pbi->br, 128)) {
                pbi->PredictionFilterMode = 2;
                pbi->PredictionFilterVarThresh =
                    VP6_bitread((BOOL_CODER *)&pbi->br, 5) << 5;
                pbi->PredictionFilterMvSizeThresh =
                    VP6_bitread((BOOL_CODER *)&pbi->br, 3);
            } else {
                pbi->PredictionFilterMode =
                    VP6_DecodeBool((BOOL_CODER *)&pbi->br, 128);
            }
        }
    } else {
        if (pbi->MultiStream || pbi->VpProfile == 0) {
            VP6_StartDecode((BOOL_CODER *)&pbi->br, Header->buffer + 1);
        } else {
            VP6_StartDecode((BOOL_CODER *)&pbi->br, Header->buffer + 3);
        }

        pbi->RefreshGoldenFrame = VP6_DecodeBool((BOOL_CODER *)&pbi->br, 128);
        if (pbi->VpProfile != 0) {
            pbi->UseLoopFilter =
                (unsigned char)VP6_DecodeBool((BOOL_CODER *)&pbi->br, 128);
            if (pbi->UseLoopFilter != 0) {
                pbi->UseLoopFilter =
                    (pbi->UseLoopFilter << 1) |
                    VP6_DecodeBool((BOOL_CODER *)&pbi->br, 128);
            }
        }
    }

    pbi->UseHuffman = VP6_DecodeBool((BOOL_CODER *)&pbi->br, 128);
    pbi->quantizer->FrameQIndex = DctQMask;
    pbi->quantizer->LastQuantizerValue =
        pbi->quantizer->QThreshTable[DctQMask];
    VP6_UpdateQ((void *)pbi->quantizer, pbi->Vp3VersionNo);
    RetVal = 1;

    return RetVal;
}

int VP6_LoadFrame(struct PB_INSTANCE *pbi) {
    int RetVal;

    RetVal = 1;
    if (LoadFrameHeader(pbi) == 0) {
        RetVal = 0;
    }
    return RetVal;
}
