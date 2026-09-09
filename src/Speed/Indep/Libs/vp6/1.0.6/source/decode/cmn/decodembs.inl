#ifndef VP6_DECODEMBS_INL
#define VP6_DECODEMBS_INL

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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
