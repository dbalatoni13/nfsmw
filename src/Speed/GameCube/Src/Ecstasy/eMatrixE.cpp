#include "Speed/GameCube/Src/Ecstasy/eMatrixE.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "dolphin/gx.h"

void eConvertToGX34(float (&mGX)[3][4], bMatrix4 &mBW) {
    {
        register double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

        asm("psq_l 9, 0(4), 0, 0\n"
            "addi 9, 4, 0x10\n"
            "psq_l 6, 0(9), 0, 0\n"
            "addi 11, 4, 0x20\n"
            "psq_l 10, 0(11), 0, 0\n"
            "addi 9, 4, 0x30\n"
            "psq_l 7, 0(9), 0, 0\n"
            "addi 11, 4, 8\n"
            "psq_l 13, 0(11), 1, 0\n"
            "addi 9, 4, 0x18\n"
            "psq_l 11, 0(9), 1, 0\n"
            "addi 11, 4, 0x28\n"
            "psq_l 12, 0(11), 1, 0\n"
            "addi 4, 4, 0x38\n"
            "psq_l 8, 0(4), 1, 0\n"
            "ps_merge00 13, 13, 11\n"
            "ps_merge11 0, 9, 6\n"
            "psq_st 13, 0x20(3), 0, 0\n"
            "ps_merge11 11, 10, 7\n"
            "psq_st 0, 0x10(3), 0, 0\n"
            "ps_merge00 12, 12, 8\n"
            "psq_st 11, 0x18(3), 0, 0\n"
            "ps_merge00 9, 9, 6\n"
            "psq_st 12, 0x28(3), 0, 0\n"
            "ps_merge00 10, 10, 7\n"
            "psq_st 9, 0(3), 0, 0\n"
            "psq_st 10, 8(3), 0, 0");
    }
}

float (*eLoadPosMtxImm(bMatrix4 &mL2V, _GXPosNrmMtx stage))[3][4] {
    static float mhL2V[3][4];

    eConvertToGX34(mhL2V, mL2V);
    GXLoadPosMtxImm(mhL2V, stage);
    GXLoadNrmMtxImm(mhL2V, stage);
    GXSetCurrentMtx(stage);
    return &mhL2V;
}
