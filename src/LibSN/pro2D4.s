# GOWE69 PPC ABI save/restore helpers, reconstructed from the original bodies.
# r11 points just beyond the GPR save area. Each public interior label starts
# at the selected register and falls through the remaining registers to blr.
# There is no C prologue: SP, LR, r11, CR and unselected registers are untouched.
.include "macros.inc"
.file "pro2D4.s"
.section .text, "ax"
.balign 4

.global _savegpr_14
_savegpr_14:
.fn __save_gpr, global
    stw r14, -0x48(r11)
.global _savegpr_15
_savegpr_15:
    stw r15, -0x44(r11)
.global _savegpr_16
_savegpr_16:
    stw r16, -0x40(r11)
.global _savegpr_17
_savegpr_17:
    stw r17, -0x3c(r11)
.global _savegpr_18
_savegpr_18:
    stw r18, -0x38(r11)
.global _savegpr_19
_savegpr_19:
    stw r19, -0x34(r11)
.global _savegpr_20
_savegpr_20:
    stw r20, -0x30(r11)
.global _savegpr_21
_savegpr_21:
    stw r21, -0x2c(r11)
.global _savegpr_22
_savegpr_22:
    stw r22, -0x28(r11)
.global _savegpr_23
_savegpr_23:
    stw r23, -0x24(r11)
.global _savegpr_24
_savegpr_24:
    stw r24, -0x20(r11)
.global _savegpr_25
_savegpr_25:
    stw r25, -0x1c(r11)
.global _savegpr_26
_savegpr_26:
    stw r26, -0x18(r11)
.global _savegpr_27
_savegpr_27:
    stw r27, -0x14(r11)
.global _savegpr_28
_savegpr_28:
    stw r28, -0x10(r11)
.global _savegpr_29
_savegpr_29:
    stw r29, -0x0c(r11)
.global _savegpr_30
_savegpr_30:
    stw r30, -0x08(r11)
.global _savegpr_31
_savegpr_31:
    stw r31, -0x04(r11)
    blr
.endfn __save_gpr

.global _restgpr_14
_restgpr_14:
.fn __restore_gpr, global
    lwz r14, -0x48(r11)
.global _restgpr_15
_restgpr_15:
    lwz r15, -0x44(r11)
.global _restgpr_16
_restgpr_16:
    lwz r16, -0x40(r11)
.global _restgpr_17
_restgpr_17:
    lwz r17, -0x3c(r11)
.global _restgpr_18
_restgpr_18:
    lwz r18, -0x38(r11)
.global _restgpr_19
_restgpr_19:
    lwz r19, -0x34(r11)
.global _restgpr_20
_restgpr_20:
    lwz r20, -0x30(r11)
.global _restgpr_21
_restgpr_21:
    lwz r21, -0x2c(r11)
.global _restgpr_22
_restgpr_22:
    lwz r22, -0x28(r11)
.global _restgpr_23
_restgpr_23:
    lwz r23, -0x24(r11)
.global _restgpr_24
_restgpr_24:
    lwz r24, -0x20(r11)
.global _restgpr_25
_restgpr_25:
    lwz r25, -0x1c(r11)
.global _restgpr_26
_restgpr_26:
    lwz r26, -0x18(r11)
.global _restgpr_27
_restgpr_27:
    lwz r27, -0x14(r11)
.global _restgpr_28
_restgpr_28:
    lwz r28, -0x10(r11)
.global _restgpr_29
_restgpr_29:
    lwz r29, -0x0c(r11)
.global _restgpr_30
_restgpr_30:
    lwz r30, -0x08(r11)
.global _restgpr_31
_restgpr_31:
    lwz r31, -0x04(r11)
    blr
.endfn __restore_gpr
