#include "Speed/GameCube/Src/Ecstasy/eMatrixE.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "dolphin/gx.h"

void eConvertToGX34(Mtx &mGX, bMatrix4 &mBW) {
    {
        double FP0, FP1, FP2, FP3, FP4, FP5, FP6, FP7, FP8, FP9, FP10, FP11;

        asm volatile("psq_l %3, 0(%9), 0, 0\n"
                     "addi 9, %9, 0x10\n"
                     "psq_l %0, 0(9), 0, 0\n"
                     "addi 11, %9, 0x20\n"
                     "psq_l %4, 0(11), 0, 0\n"
                     "addi 9, %9, 0x30\n"
                     "psq_l %1, 0(9), 0, 0\n"
                     "addi 11, %9, 8\n"
                     "psq_l %7, 0(11), 1, 0\n"
                     "addi 9, %9, 0x18\n"
                     "psq_l %5, 0(9), 1, 0\n"
                     "addi 11, %9, 0x28\n"
                     "psq_l %6, 0(11), 1, 0\n"
                     "addi %9, %9, 0x38\n"
                     "psq_l %2, 0(%9), 1, 0\n"
                     "ps_merge00 %7, %7, %5\n"
                     "ps_merge11 %8, %3, %0"
                     : "=&f"(FP0), "=&f"(FP1), "=&f"(FP2), "=&f"(FP3), "=&f"(FP4), "=&f"(FP5), "=&f"(FP6), "=&f"(FP7), "=&f"(FP8)
                     : "r"(&mBW)
                     : "r9", "r11", "memory");

        asm volatile("psq_st %7, 0x20(%9), 0, 0\n"
                     "ps_merge11 %5, %4, %1\n"
                     "psq_st %8, 0x10(%9), 0, 0\n"
                     "ps_merge00 %6, %6, %2\n"
                     "psq_st %5, 0x18(%9), 0, 0\n"
                     "ps_merge00 %3, %3, %0\n"
                     "psq_st %6, 0x28(%9), 0, 0\n"
                     "ps_merge00 %4, %4, %1\n"
                     "psq_st %3, 0(%9), 0, 0\n"
                     "psq_st %4, 8(%9), 0, 0"
                     :
                     : "f"(FP0), "f"(FP1), "f"(FP2), "f"(FP3), "f"(FP4), "f"(FP5), "f"(FP6), "f"(FP7), "f"(FP8), "r"(&mGX)
                     : "memory");
    }
}

Mtx *eLoadPosMtxImm(bMatrix4 &mL2V, _GXPosNrmMtx stage) {
    static Mtx mhL2V;

    eConvertToGX34(mhL2V, mL2V);
    GXLoadPosMtxImm(mhL2V, stage);
    GXLoadNrmMtxImm(mhL2V, stage);
    GXSetCurrentMtx(stage);
    return &mhL2V;
}
