#ifndef VP6_GCFILTERBLOCK_INL
#define VP6_GCFILTERBLOCK_INL

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

/* Native Gekko paired-single routines recovered from the retail source owner.
 * The GQR formats are configured by the VP6 machine-specific initialization.
 * These out-of-line assembler bodies intentionally keep the retail register ABI;
 * do not inline them into C callers or replace them with generic POWER opcodes.
 */

static float f64 = 64.0f;

void UnpackBlock_GC(unsigned char *ReconPtr, short *ReconRefPtr,
                    unsigned int ReconPixelsPerLine) {
    __asm__ volatile(
        "li 0, 8\n"
        "mtctr 0\n"
        "1:\n"
        "psq_l 0, 0(3), 0, 2\n"
        "psq_l 1, 2(3), 0, 2\n"
        "psq_l 2, 4(3), 0, 2\n"
        "psq_l 3, 6(3), 0, 2\n"
        "add 3, 3, 5\n"
        "psq_st 0, 0(4), 0, 5\n"
        "psq_st 1, 4(4), 0, 5\n"
        "psq_st 2, 8(4), 0, 5\n"
        "psq_st 3, 12(4), 0, 5\n"
        "addi 4, 4, 16\n"
        "bdnz 1b\n"
    );
}

void FilterBlock1dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                        unsigned int SrcPixelsPerLine, unsigned int PixelStep,
                        int *Filter) {
    __asm__ volatile(
        "li 9, f64@sda21\n"
        "li 0, 8\n"
        "psq_l 8, 3(7), 1, 2\n"
        "psq_l 9, 7(7), 1, 2\n"
        "psq_l 10, 0(9), 1, 0\n"
        "mtctr 0\n"
        "add 6, 3, 6\n"
        "ps_merge00 10, 10, 10\n"
        "1:\n"
        "psq_l 0, 0(3), 0, 2\n"
        "psq_l 1, 2(3), 0, 2\n"
        "psq_l 2, 4(3), 0, 2\n"
        "psq_l 3, 6(3), 0, 2\n"
        "add 3, 3, 5\n"
        "psq_l 4, 0(6), 0, 2\n"
        "psq_l 5, 2(6), 0, 2\n"
        "psq_l 6, 4(6), 0, 2\n"
        "psq_l 7, 6(6), 0, 2\n"
        "add 6, 6, 5\n"
        "ps_madds0 0, 0, 8, 10\n"
        "ps_madds0 1, 1, 8, 10\n"
        "ps_madds0 2, 2, 8, 10\n"
        "ps_madds0 3, 3, 8, 10\n"
        "ps_madds0 0, 4, 9, 0\n"
        "ps_madds0 1, 5, 9, 1\n"
        "ps_madds0 2, 6, 9, 2\n"
        "ps_madds0 3, 7, 9, 3\n"
        "psq_st 0, 0(4), 0, 6\n"
        "psq_st 1, 4(4), 0, 6\n"
        "psq_st 2, 8(4), 0, 6\n"
        "psq_st 3, 12(4), 0, 6\n"
        "addi 4, 4, 16\n"
        "bdnz 1b\n"
    );
}

/* Two-pass bilinear filtering owns its private intermediate row buffer. */
void FilterBlock2dBil_GC(unsigned char *SrcPtr, unsigned short *OutputPtr,
                        unsigned int SrcPixelsPerLine, int *HFilter,
                        int *VFilter) {
    static unsigned short FData[72];

    __asm__ volatile(
        "lis 9, %0@ha\n"
        "li 11, f64@sda21\n"
        "addi 9, 9, %0@l\n"
        "li 0, 9\n"
        "psq_l 8, 3(6), 1, 2\n"
        "psq_l 9, 7(6), 1, 2\n"
        "psq_l 10, 0(11), 1, 0\n"
        "mtctr 0\n"
        "addi 6, 3, 1\n"
        "ps_merge00 10, 10, 10\n"
        "1:\n"
        "psq_l 0, 0(3), 0, 2\n"
        "psq_l 1, 2(3), 0, 2\n"
        "psq_l 2, 4(3), 0, 2\n"
        "psq_l 3, 6(3), 0, 2\n"
        "add 3, 3, 5\n"
        "psq_l 4, 0(6), 0, 2\n"
        "psq_l 5, 2(6), 0, 2\n"
        "psq_l 6, 4(6), 0, 2\n"
        "psq_l 7, 6(6), 0, 2\n"
        "add 6, 6, 5\n"
        "ps_madds0 0, 0, 8, 10\n"
        "ps_madds0 1, 1, 8, 10\n"
        "ps_madds0 2, 2, 8, 10\n"
        "ps_madds0 3, 3, 8, 10\n"
        "ps_madds0 0, 4, 9, 0\n"
        "ps_madds0 1, 5, 9, 1\n"
        "ps_madds0 2, 6, 9, 2\n"
        "ps_madds0 3, 7, 9, 3\n"
        "psq_st 0, 0(9), 0, 6\n"
        "psq_st 1, 4(9), 0, 6\n"
        "psq_st 2, 8(9), 0, 6\n"
        "psq_st 3, 12(9), 0, 6\n"
        "addi 9, 9, 16\n"
        "bdnz 1b\n"
        "li 0, 8\n"
        "subi 3, 9, 144\n"
        "psq_l 8, 3(7), 1, 2\n"
        "psq_l 9, 7(7), 1, 2\n"
        "mtctr 0\n"
        "addi 6, 3, 16\n"
        "2:\n"
        "psq_l 0, 0(3), 0, 3\n"
        "psq_l 1, 4(3), 0, 3\n"
        "psq_l 2, 8(3), 0, 3\n"
        "psq_l 3, 12(3), 0, 3\n"
        "addi 3, 3, 16\n"
        "psq_l 4, 0(6), 0, 3\n"
        "psq_l 5, 4(6), 0, 3\n"
        "psq_l 6, 8(6), 0, 3\n"
        "psq_l 7, 12(6), 0, 3\n"
        "addi 6, 6, 16\n"
        "ps_madds0 0, 0, 8, 10\n"
        "ps_madds0 1, 1, 8, 10\n"
        "ps_madds0 2, 2, 8, 10\n"
        "ps_madds0 3, 3, 8, 10\n"
        "ps_madds0 0, 4, 9, 0\n"
        "ps_madds0 1, 5, 9, 1\n"
        "ps_madds0 2, 6, 9, 2\n"
        "ps_madds0 3, 7, 9, 3\n"
        "psq_st 0, 0(4), 0, 6\n"
        "psq_st 1, 4(4), 0, 6\n"
        "psq_st 2, 8(4), 0, 6\n"
        "psq_st 3, 12(4), 0, 6\n"
        "addi 4, 4, 16\n"
        "bdnz 2b\n"
        : : "i"(FData) : "memory"
    );
}

#endif
