#ifndef VP6_GCRECONSTRUCT_INL
#define VP6_GCRECONSTRUCT_INL

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

/* Native Gekko paired-single routines recovered from the retail source owner.
 * The GQR formats are configured by the VP6 machine-specific initialization.
 * These out-of-line assembler bodies intentionally keep the retail register ABI;
 * do not inline them into C callers or replace them with generic POWER opcodes.
 */

static float f128 = 128.0f;

void ScalarReconIntra_GC(short *TmpDataBuffer, unsigned char *ReconPtr,
                         unsigned short *ChangePtr, unsigned int LineStep) {
    __asm__ volatile(
        "li 9, f128@sda21\n"
        "li 0, 8\n"
        "psq_l 4, 0(9), 1, 0\n"
        "mtctr 0\n"
        "ps_merge00 4, 4, 4\n"
        "1:\n"
        "psq_l 0, 0(5), 0, 5\n"
        "psq_l 1, 4(5), 0, 5\n"
        "psq_l 2, 8(5), 0, 5\n"
        "psq_l 3, 12(5), 0, 5\n"
        "addi 5, 5, 16\n"
        "ps_add 0, 0, 4\n"
        "ps_add 1, 1, 4\n"
        "ps_add 2, 2, 4\n"
        "ps_add 3, 3, 4\n"
        "psq_st 0, 0(4), 0, 2\n"
        "psq_st 1, 2(4), 0, 2\n"
        "psq_st 2, 4(4), 0, 2\n"
        "psq_st 3, 6(4), 0, 2\n"
        "add 4, 4, 6\n"
        "bdnz 1b\n"
    );
}

void ScalarReconInter_GC(short *TmpDataBuffer, unsigned char *ReconPtr,
                         unsigned char *RefPtr, short *ChangePtr,
                         unsigned int LineStep) {
    __asm__ volatile(
        "li 0, 8\n"
        "mtctr 0\n"
        "1:\n"
        "psq_l 0, 0(5), 0, 2\n"
        "psq_l 1, 2(5), 0, 2\n"
        "psq_l 2, 4(5), 0, 2\n"
        "psq_l 3, 6(5), 0, 2\n"
        "add 5, 5, 7\n"
        "psq_l 4, 0(6), 0, 5\n"
        "psq_l 5, 4(6), 0, 5\n"
        "psq_l 6, 8(6), 0, 5\n"
        "psq_l 7, 12(6), 0, 5\n"
        "addi 6, 6, 16\n"
        "ps_add 0, 0, 4\n"
        "ps_add 1, 1, 5\n"
        "ps_add 2, 2, 6\n"
        "ps_add 3, 3, 7\n"
        "psq_st 0, 0(4), 0, 2\n"
        "psq_st 1, 2(4), 0, 2\n"
        "psq_st 2, 4(4), 0, 2\n"
        "psq_st 3, 6(4), 0, 2\n"
        "add 4, 4, 7\n"
        "bdnz 1b\n"
    );
}

void ReconBlock_GC(short *SrcBlock, short *ReconRefPtr, unsigned char *DestBlock,
                   unsigned int LineStep) {
    __asm__ volatile(
        "li 0, 8\n"
        "mtctr 0\n"
        "1:\n"
        "psq_l 0, 0(3), 0, 5\n"
        "psq_l 1, 4(3), 0, 5\n"
        "psq_l 2, 8(3), 0, 5\n"
        "psq_l 3, 12(3), 0, 5\n"
        "addi 3, 3, 16\n"
        "psq_l 4, 0(4), 0, 5\n"
        "psq_l 5, 4(4), 0, 5\n"
        "psq_l 6, 8(4), 0, 5\n"
        "psq_l 7, 12(4), 0, 5\n"
        "addi 4, 4, 16\n"
        "ps_add 0, 0, 4\n"
        "ps_add 1, 1, 5\n"
        "ps_add 2, 2, 6\n"
        "ps_add 3, 3, 7\n"
        "psq_st 0, 0(5), 0, 2\n"
        "psq_st 1, 2(5), 0, 2\n"
        "psq_st 2, 4(5), 0, 2\n"
        "psq_st 3, 6(5), 0, 2\n"
        "add 5, 5, 6\n"
        "bdnz 1b\n"
    );
}

#endif
