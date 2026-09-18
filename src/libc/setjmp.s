# setjmp / longjmp de la libc de SN (newlib, libc/machine/powerpc/setjmp.S).
# setjmp guarda r1, r2, r13..r31, LR, CR y f14..f31 en el jmp_buf realineado
# a 8 bytes; longjmp los restaura y devuelve 1 cuando el valor pedido es 0.
.include "macros.inc"
.file "setjmp.S"
.section .text, "ax"
.balign 4

.fn setjmp, global
    addi r3, r3, 7
    rlwinm r3, r3, 0, 0, 28
    stw r1, 0(r3)
    stwu r2, 4(r3)
    stwu r13, 4(r3)
    stwu r14, 4(r3)
    stwu r15, 4(r3)
    stwu r16, 4(r3)
    stwu r17, 4(r3)
    stwu r18, 4(r3)
    stwu r19, 4(r3)
    stwu r20, 4(r3)
    stwu r21, 4(r3)
    stwu r22, 4(r3)
    stwu r23, 4(r3)
    stwu r24, 4(r3)
    stwu r25, 4(r3)
    stwu r26, 4(r3)
    stwu r27, 4(r3)
    stwu r28, 4(r3)
    stwu r29, 4(r3)
    stwu r30, 4(r3)
    stwu r31, 4(r3)
    mflr r4
    stwu r4, 4(r3)
    mfcr r4
    stwu r4, 4(r3)
    stfdu f14, 8(r3)
    stfdu f15, 8(r3)
    stfdu f16, 8(r3)
    stfdu f17, 8(r3)
    stfdu f18, 8(r3)
    stfdu f19, 8(r3)
    stfdu f20, 8(r3)
    stfdu f21, 8(r3)
    stfdu f22, 8(r3)
    stfdu f23, 8(r3)
    stfdu f24, 8(r3)
    stfdu f25, 8(r3)
    stfdu f26, 8(r3)
    stfdu f27, 8(r3)
    stfdu f28, 8(r3)
    stfdu f29, 8(r3)
    stfdu f30, 8(r3)
    stfdu f31, 8(r3)
    li r3, 0
    blr
.endfn setjmp

.fn longjmp, global
    addi r3, r3, 7
    rlwinm r3, r3, 0, 0, 28
    lwz r1, 0(r3)
    lwzu r2, 4(r3)
    lwzu r13, 4(r3)
    lwzu r14, 4(r3)
    lwzu r15, 4(r3)
    lwzu r16, 4(r3)
    lwzu r17, 4(r3)
    lwzu r18, 4(r3)
    lwzu r19, 4(r3)
    lwzu r20, 4(r3)
    lwzu r21, 4(r3)
    lwzu r22, 4(r3)
    lwzu r23, 4(r3)
    lwzu r24, 4(r3)
    lwzu r25, 4(r3)
    lwzu r26, 4(r3)
    lwzu r27, 4(r3)
    lwzu r28, 4(r3)
    lwzu r29, 4(r3)
    lwzu r30, 4(r3)
    lwzu r31, 4(r3)
    lwzu r5, 4(r3)
    mtlr r5
    lwzu r5, 4(r3)
    mtcrf 255, r5
    lfdu f14, 8(r3)
    lfdu f15, 8(r3)
    lfdu f16, 8(r3)
    lfdu f17, 8(r3)
    lfdu f18, 8(r3)
    lfdu f19, 8(r3)
    lfdu f20, 8(r3)
    lfdu f21, 8(r3)
    lfdu f22, 8(r3)
    lfdu f23, 8(r3)
    lfdu f24, 8(r3)
    lfdu f25, 8(r3)
    lfdu f26, 8(r3)
    lfdu f27, 8(r3)
    lfdu f28, 8(r3)
    lfdu f29, 8(r3)
    lfdu f30, 8(r3)
    lfdu f31, 8(r3)
    mr. r3, r4
    bnelr+
    li r3, 1
    blr
.endfn longjmp
