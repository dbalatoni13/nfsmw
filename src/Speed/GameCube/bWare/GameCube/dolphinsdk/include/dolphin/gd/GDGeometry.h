#ifndef _DOLPHIN_GD_GEOMETRY_H_
#define _DOLPHIN_GD_GEOMETRY_H_

#include <dolphin/gx/GXStruct.h>

#ifdef __cplusplus
extern "C" {
#endif

// Command processor register IDs
#define CP_REG_MTXIDXA_ID     0x30  // Matrix index A
#define CP_REG_MTXIDXB_ID     0x40  // Matrix index B
#define CP_REG_VCD_LO_ID      0x50  // Vertex descriptor (lo)
#define CP_REG_VCD_HI_ID      0x60  // Vertex descriptor (hi)
#define CP_REG_VAT_GRP0_ID    0x70  // Vertex attribute table (group 0)
#define CP_REG_VAT_GRP1_ID    0x80  // Vertex attribute table (group 1)
#define CP_REG_VAT_GRP2_ID    0x90  // Vertex attribute table (group 2)
#define CP_REG_ARRAYBASE_ID   0xA0  // Vertex array start/base
#define CP_REG_ARRAYSTRIDE_ID 0xB0  // Vertex array stride

#define CP_REG_VCD_LO(pnMtxIdx, txMtxIdxMask, posn, norm, col0, col1) \
    ( \
        (pnMtxIdx) << 0  | \
        (txMtxIdxMask) << 1  | \
        (posn) << 9  | \
        (norm) << 11 | \
        (col0) << 13 | \
        (col1) << 15   \
    )

#define CP_REG_VCD_HI(tex0, tex1, tex2, tex3, tex4, tex5, tex6, tex7) \
    ( \
        (tex0) << 0  | \
        (tex1) << 2  | \
        (tex2) << 4  | \
        (tex3) << 6  | \
        (tex4) << 8  | \
        (tex5) << 10 | \
        (tex6) << 12 | \
        (tex7) << 14   \
    )

#define CP_REG_VAT_GRP0(posCnt, posType, posFrac, nrmCnt, nrmType, c0Cnt, c0Type, c1Cnt, c1Type, tx0Cnt, tx0Type, tx0Frac, p12, nrmIdx3) \
    ( \
        (posCnt) << 0 | \
        (posType) << 1 | \
        (posFrac) << 4 | \
        (nrmCnt) << 9 | \
        (nrmType) << 10 | \
        (c0Cnt) << 13 | \
        (c0Type) << 14 | \
        (c1Cnt) << 17 | \
        (c1Type) << 18 | \
        (tx0Cnt) << 21 | \
        (tx0Type) << 22 | \
        (tx0Frac) << 25 | \
        (p12) << 30 | \
        (nrmIdx3) << 31 \
    )

#define CP_REG_VAT_GRP1(tx1Cnt, tx1Type, tx1Frac, tx2Cnt, tx2Type, tx2Frac, tx3Cnt, tx3Type, tx3Frac, tx4Cnt, tx4Type, p11) \
    ( \
        (tx1Cnt) << 0 | \
        (tx1Type) << 1 | \
        (tx1Frac) << 4 | \
        (tx2Cnt) << 9 | \
        (tx2Type) << 10 | \
        (tx2Frac) << 13 | \
        (tx3Cnt) << 18 | \
        (tx3Type) << 19 | \
        (tx3Frac) << 22 | \
        (tx4Cnt) << 27 | \
        (tx4Type) << 28 | \
        p11 << 31 \
    )

#define CP_REG_VAT_GRP2(tx4Frac, tx5Cnt, tx5Type, tx5Frac, tx6Cnt, tx6Type, tx6Frac, tx7Cnt, tx7Type, tx7Frac) \
    ( \
        (tx4Frac) << 0 | \
        (tx5Cnt) << 5 | \
        (tx5Type) << 6 | \
        (tx5Frac) << 9 | \
        (tx6Cnt) << 14 | \
        (tx6Type) << 15 | \
        (tx6Frac) << 18 | \
        (tx7Cnt) << 23 | \
        (tx7Type) << 24 | \
        (tx7Frac) << 27 \
    )

// Transform unit register IDs
#define XF_REG_ERROR_ID        0x1000
#define XF_REG_DIAGNOSTICS_ID  0x1001
#define XF_REG_STATE0_ID       0x1002
#define XF_REG_STATE1_ID       0x1003
#define XF_REG_CLOCK_ID        0x1004
#define XF_REG_CLIPDISABLE_ID  0x1005
#define XF_REG_PERF0_ID        0x1006
#define XF_REG_PERF1_ID        0x1007
#define XF_REG_INVERTEXSPEC_ID 0x1008
#define XF_REG_NUMCOLORS_ID    0x1009
#define XF_REG_DUALTEXTRAN_ID  0x1012
#define XF_REG_SCALEX_ID       0x101A
#define XF_REG_SCALEY_ID       0x101B
#define XF_REG_SCALEZ_ID       0x101C
#define XF_REG_OFFSETX_ID      0x101D
#define XF_REG_OFFSETY_ID      0x101E
#define XF_REG_OFFSETZ_ID      0x101F
#define XF_REG_NUMTEX_ID       0x103F
#define XF_REG_TEX0_ID         0x1040
#define XF_REG_TEX1_ID         0x1041
#define XF_REG_TEX2_ID         0x1042
#define XF_REG_TEX3_ID         0x1043
#define XF_REG_TEX4_ID         0x1044
#define XF_REG_TEX5_ID         0x1045
#define XF_REG_TEX6_ID         0x1046
#define XF_REG_TEX7_ID         0x1047
#define XF_REG_DUALTEX0_ID     0x1050
#define XF_REG_DUALTEX1_ID     0x1051
#define XF_REG_DUALTEX2_ID     0x1052
#define XF_REG_DUALTEX3_ID     0x1053
#define XF_REG_DUALTEX4_ID     0x1054
#define XF_REG_DUALTEX5_ID     0x1055
#define XF_REG_DUALTEX6_ID     0x1056
#define XF_REG_DUALTEX7_ID     0x1057

#define XF_REG_INVTXSPEC(ncols, nnorms, ntexs) \
    ( \
        (ncols) << 0 | \
        (nnorms) << 2 | \
        (ntexs) << 4 \
    )

#define XF_REG_TEX(proj, form, tgType, row, embossRow, embossLit) \
    ( \
        (proj) << 1 | \
        (form) << 2 | \
        (tgType) << 4 | \
        (row) << 7 | \
        (embossRow) << 12 | \
        (embossLit) << 15 \
    )

#define XF_REG_DUALTEX(mtx, normalize) \
    ( \
        (mtx) << 0 | \
        (normalize) << 8 \
    )

#define BP_GEN_MODE(nTexGens, nChans, nTevs, p4, nInds) \
    ( \
        (u32)(nTexGens) << 0 | \
        (u32)(nChans) << 4 | \
        (u32)(nTevs) << 10 | \
        (u32)(p4) << 14 | \
        (u32)(nInds) << 16 \
    )

#define BP_LP_SIZE(lineWidth, pointSize, lineOffset, pointOffset, lineHalfAspect, p5) \
    ( \
        (u32)(lineWidth) << 0 | \
        (u32)(pointSize) << 8 | \
        (u32)(lineOffset) << 16 | \
        (u32)(pointOffset) << 19 | \
        (u32)(lineHalfAspect) << 22 | \
        (u32)(p5) << 24 \
    )

void GDSetVtxDescv(const GXVtxDescList* attrPtr);
void GDSetVtxAttrFmtv(GXVtxFmt vtxfmt, const GXVtxAttrFmtList* list);
void GDSetArray(GXAttr attr, void* base_ptr, u8 stride);
void GDSetArrayRaw(GXAttr attr, u32 base_ptr_raw, u8 stride);
void GDPatchArrayPtr(void* base_ptr);
void GDSetTexCoordGen(GXTexCoordID dst_coord, GXTexGenType func, GXTexGenSrc src_param, u8 normalize, u32 postmtx);
void GDSetCullMode(GXCullMode mode);
void GDSetGenMode(u8 nTexGens, u8 nChans, u8 nTevs);
void GDSetGenMode2(u8 nTexGens, u8 nChans, u8 nTevs, u8 nInds, GXCullMode cm);
void GDSetLPSize(u8 lineWidth, u8 pointSize, GXTexOffset lineOffset, GXTexOffset pointOffset, u8 lineHalfAspect);
void GDSetCoPlanar(u8 enable);

#ifdef __cplusplus
}
#endif

#endif
