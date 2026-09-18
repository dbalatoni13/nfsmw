#include "../cmn/sndcmn.h"
#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/ax.h>
#endif

extern "C" {
int OSDisableInterrupts();
int OSRestoreInterrupts(int level);
}

#define AX_PB_MIXCTRL_L 0x0001
#define AX_PB_MIXCTRL_R 0x0002
#define AX_PB_MIXCTRL_S 0x0004
#define AX_PB_MIXCTRL_DL 0x0008
#define AX_PB_MIXCTRL_AUXAL 0x0010
#define AX_PB_MIXCTRL_AUXAR 0x0020
#define AX_PB_MIXCTRL_AUXADL 0x0040
#define AX_PB_MIXCTRL_AUXAS 0x0080
#define AX_PB_MIXCTRL_AUXADS 0x0100
#define AX_PB_MIXCTRL_AUXBL 0x0200
#define AX_PB_MIXCTRL_AUXBR 0x0400
#define AX_PB_MIXCTRL_AUXBDL 0x0800
#define AX_PB_MIXCTRL_AUXBS 0x1000
#define AX_PB_MIXCTRL_AUXBDS 0x2000
#define AX_PB_MIXCTRL_SURROUND 0x4000

#define MIXMODE_LFE 0x00000004
#define MIXMODE_SETINPUT 0x10000000
#define MIXMODE_NEWINPUT 0x20000000
#define MIXMODE_SETMIX 0x40000000
#define MIXMODE_NEWMIX 0x80000000

#define MIXVOLRAMPFRAMES 320

// total size: 0x58
typedef struct SNDDRVMIXCHANNEL {
    AXVPB *axvpb;       // offset 0x0
    unsigned int mode;  // offset 0x4
    int input;          // offset 0x8
    int auxA;           // offset 0xC
    int auxB;           // offset 0x10
    int balances[6];    // offset 0x14
    int fader;          // offset 0x2C
    unsigned short v;   // offset 0x30
    unsigned short v1;  // offset 0x32
    unsigned short vL;  // offset 0x34
    unsigned short vL1; // offset 0x36
    unsigned short vR;  // offset 0x38
    unsigned short vR1; // offset 0x3A
    unsigned short vS;  // offset 0x3C
    unsigned short vS1; // offset 0x3E
    unsigned short vAL;  // offset 0x40
    unsigned short vAL1; // offset 0x42
    unsigned short vAR;  // offset 0x44
    unsigned short vAR1; // offset 0x46
    unsigned short vAS;  // offset 0x48
    unsigned short vAS1; // offset 0x4A
    unsigned short vBL;  // offset 0x4C
    unsigned short vBL1; // offset 0x4E
    unsigned short vBR;  // offset 0x50
    unsigned short vBR1; // offset 0x52
    unsigned short vBS;  // offset 0x54
    unsigned short vBS1; // offset 0x56
} SNDDRVMIXCHANNEL;

// Preserve the original table's effective 8-byte boundary without a dummy datum.
// This alignment is inferred from the binary layout, not encoded by DWARF1.
unsigned short SNDDRV_dolbypl2balances[256][4] __attribute__((aligned(8))) = {
    { 0x5A82, 0x5A82, 0x0000, 0x0000 },
    { 0x58BF, 0x5C3C, 0x0000, 0x0000 },
    { 0x56F3, 0x5DEF, 0x0000, 0x0000 },
    { 0x551D, 0x5F99, 0x0000, 0x0000 },
    { 0x533E, 0x613B, 0x0000, 0x0000 },
    { 0x5154, 0x62D6, 0x0000, 0x0000 },
    { 0x4F60, 0x646A, 0x0000, 0x0000 },
    { 0x4D5F, 0x65F7, 0x0000, 0x0000 },
    { 0x4B53, 0x677D, 0x0000, 0x0000 },
    { 0x4939, 0x68FC, 0x0000, 0x0000 },
    { 0x4712, 0x6A74, 0x0000, 0x0000 },
    { 0x44DC, 0x6BE5, 0x0000, 0x0000 },
    { 0x4297, 0x6D50, 0x0000, 0x0000 },
    { 0x4042, 0x6EB3, 0x0000, 0x0000 },
    { 0x3DDB, 0x700F, 0x0000, 0x0000 },
    { 0x3B62, 0x7164, 0x0000, 0x0000 },
    { 0x38D5, 0x72B0, 0x0000, 0x0000 },
    { 0x3633, 0x73F5, 0x0000, 0x0000 },
    { 0x337B, 0x7530, 0x0000, 0x0000 },
    { 0x30AB, 0x7662, 0x0000, 0x0000 },
    { 0x2DC2, 0x778A, 0x0000, 0x0000 },
    { 0x2ABE, 0x78A6, 0x0000, 0x0000 },
    { 0x279D, 0x79B7, 0x0000, 0x0000 },
    { 0x245C, 0x7AB9, 0x0000, 0x0000 },
    { 0x20FB, 0x7BAD, 0x0000, 0x0000 },
    { 0x1D76, 0x7C90, 0x0000, 0x0000 },
    { 0x19CB, 0x7D5F, 0x0000, 0x0000 },
    { 0x15F7, 0x7E19, 0x0000, 0x0000 },
    { 0x11F7, 0x7EBB, 0x0000, 0x0000 },
    { 0x0DC9, 0x7F41, 0x0000, 0x0000 },
    { 0x0968, 0x7FA7, 0x0000, 0x0000 },
    { 0x04D0, 0x7FE8, 0x0000, 0x0000 },
    { 0x0000, 0x7FFF, 0x0000, 0x0000 },
    { 0x0000, 0x7FE8, 0x025C, 0x0432 },
    { 0x0000, 0x7FA7, 0x049C, 0x0832 },
    { 0x0000, 0x7F41, 0x06C1, 0x0C04 },
    { 0x0000, 0x7EBB, 0x08CE, 0x0FA9 },
    { 0x0000, 0x7E19, 0x0AC4, 0x1325 },
    { 0x0000, 0x7D5F, 0x0CA4, 0x167B },
    { 0x0000, 0x7C90, 0x0E70, 0x19AE },
    { 0x0000, 0x7BAD, 0x102A, 0x1CBF },
    { 0x0000, 0x7AB9, 0x11D2, 0x1FB1 },
    { 0x0000, 0x79B7, 0x136A, 0x2287 },
    { 0x0000, 0x78A6, 0x14F3, 0x2541 },
    { 0x0000, 0x778A, 0x166E, 0x27E2 },
    { 0x0000, 0x7662, 0x17DB, 0x2A6C },
    { 0x0000, 0x7530, 0x193C, 0x2CDF },
    { 0x0000, 0x73F5, 0x1A91, 0x2F3E },
    { 0x0000, 0x72B0, 0x1BDB, 0x3189 },
    { 0x0000, 0x7164, 0x1D1B, 0x33C2 },
    { 0x0000, 0x700F, 0x1E51, 0x35EA },
    { 0x0000, 0x6EB3, 0x1F7F, 0x3802 },
    { 0x0000, 0x6D50, 0x20A4, 0x3A0B },
    { 0x0000, 0x6BE5, 0x21C0, 0x3C05 },
    { 0x0000, 0x6A74, 0x22D6, 0x3DF2 },
    { 0x0000, 0x68FC, 0x23E4, 0x3FD3 },
    { 0x0000, 0x677D, 0x24EB, 0x41A7 },
    { 0x0000, 0x65F7, 0x25EC, 0x4371 },
    { 0x0000, 0x646A, 0x26E8, 0x452F },
    { 0x0000, 0x62D6, 0x27DD, 0x46E4 },
    { 0x0000, 0x613B, 0x28CD, 0x488F },
    { 0x0000, 0x5F99, 0x29B8, 0x4A30 },
    { 0x0000, 0x5DEF, 0x2A9E, 0x4BCA },
    { 0x0000, 0x5C3C, 0x2B80, 0x4D5B },
    { 0x0000, 0x5A82, 0x2C5D, 0x4EE4 },
    { 0x0000, 0x58BF, 0x2D35, 0x5065 },
    { 0x0000, 0x56F3, 0x2E0A, 0x51E0 },
    { 0x0000, 0x551D, 0x2EDB, 0x5353 },
    { 0x0000, 0x533E, 0x2FA8, 0x54C0 },
    { 0x0000, 0x5154, 0x3072, 0x5626 },
    { 0x0000, 0x4F60, 0x3138, 0x5786 },
    { 0x0000, 0x4D5F, 0x31FA, 0x58E0 },
    { 0x0000, 0x4B53, 0x32B9, 0x5A34 },
    { 0x0000, 0x4939, 0x3375, 0x5B82 },
    { 0x0000, 0x4712, 0x342D, 0x5CCA },
    { 0x0000, 0x44DC, 0x34E3, 0x5E0C },
    { 0x0000, 0x4297, 0x3594, 0x5F48 },
    { 0x0000, 0x4042, 0x3642, 0x607D },
    { 0x0000, 0x3DDB, 0x36ED, 0x61AD },
    { 0x0000, 0x3B62, 0x3794, 0x62D5 },
    { 0x0000, 0x38D5, 0x3837, 0x63F7 },
    { 0x0000, 0x3633, 0x38D6, 0x6512 },
    { 0x0000, 0x337B, 0x3970, 0x6625 },
    { 0x0000, 0x30AB, 0x3A06, 0x6730 },
    { 0x0000, 0x2DC2, 0x3A97, 0x6832 },
    { 0x0000, 0x2ABE, 0x3B23, 0x692A },
    { 0x0000, 0x279D, 0x3BA8, 0x6A17 },
    { 0x0000, 0x245C, 0x3C27, 0x6AF8 },
    { 0x0000, 0x20FB, 0x3C9F, 0x6BCD },
    { 0x0000, 0x1D76, 0x3D0E, 0x6C92 },
    { 0x0000, 0x19CB, 0x3D73, 0x6D47 },
    { 0x0000, 0x15F7, 0x3DCF, 0x6DE9 },
    { 0x0000, 0x11F7, 0x3E1E, 0x6E76 },
    { 0x0000, 0x0DC9, 0x3E5F, 0x6EEB },
    { 0x0000, 0x0968, 0x3E91, 0x6F44 },
    { 0x0000, 0x04D0, 0x3EB2, 0x6F7D },
    { 0x0000, 0x0000, 0x3EBD, 0x6F91 },
    { 0x0000, 0x0000, 0x4031, 0x6EBD },
    { 0x0000, 0x0000, 0x4192, 0x6DED },
    { 0x0000, 0x0000, 0x42E1, 0x6D23 },
    { 0x0000, 0x0000, 0x4420, 0x6C5D },
    { 0x0000, 0x0000, 0x4550, 0x6B9B },
    { 0x0000, 0x0000, 0x4673, 0x6ADE },
    { 0x0000, 0x0000, 0x4789, 0x6A24 },
    { 0x0000, 0x0000, 0x4894, 0x696E },
    { 0x0000, 0x0000, 0x4995, 0x68BC },
    { 0x0000, 0x0000, 0x4A8C, 0x680C },
    { 0x0000, 0x0000, 0x4B7A, 0x6760 },
    { 0x0000, 0x0000, 0x4C61, 0x66B6 },
    { 0x0000, 0x0000, 0x4D40, 0x660F },
    { 0x0000, 0x0000, 0x4E18, 0x656A },
    { 0x0000, 0x0000, 0x4EEA, 0x64C7 },
    { 0x0000, 0x0000, 0x4FB6, 0x6426 },
    { 0x0000, 0x0000, 0x507D, 0x6386 },
    { 0x0000, 0x0000, 0x513F, 0x62E8 },
    { 0x0000, 0x0000, 0x51FC, 0x624C },
    { 0x0000, 0x0000, 0x52B5, 0x61B0 },
    { 0x0000, 0x0000, 0x536A, 0x6115 },
    { 0x0000, 0x0000, 0x541C, 0x607C },
    { 0x0000, 0x0000, 0x54CA, 0x5FE3 },
    { 0x0000, 0x0000, 0x5576, 0x5F4A },
    { 0x0000, 0x0000, 0x561E, 0x5EB2 },
    { 0x0000, 0x0000, 0x56C4, 0x5E19 },
    { 0x0000, 0x0000, 0x5768, 0x5D81 },
    { 0x0000, 0x0000, 0x580A, 0x5CE9 },
    { 0x0000, 0x0000, 0x58AA, 0x5C50 },
    { 0x0000, 0x0000, 0x5949, 0x5BB7 },
    { 0x0000, 0x0000, 0x59E6, 0x5B1D },
    { 0x0000, 0x0000, 0x5A82, 0x5A82 },
    { 0x0000, 0x0000, 0x5B1D, 0x59E6 },
    { 0x0000, 0x0000, 0x5BB7, 0x5949 },
    { 0x0000, 0x0000, 0x5C50, 0x58AA },
    { 0x0000, 0x0000, 0x5CE9, 0x580A },
    { 0x0000, 0x0000, 0x5D81, 0x5768 },
    { 0x0000, 0x0000, 0x5E19, 0x56C4 },
    { 0x0000, 0x0000, 0x5EB2, 0x561E },
    { 0x0000, 0x0000, 0x5F4A, 0x5576 },
    { 0x0000, 0x0000, 0x5FE3, 0x54CA },
    { 0x0000, 0x0000, 0x607C, 0x541C },
    { 0x0000, 0x0000, 0x6115, 0x536A },
    { 0x0000, 0x0000, 0x61B0, 0x52B5 },
    { 0x0000, 0x0000, 0x624C, 0x51FC },
    { 0x0000, 0x0000, 0x62E8, 0x513F },
    { 0x0000, 0x0000, 0x6386, 0x507D },
    { 0x0000, 0x0000, 0x6426, 0x4FB6 },
    { 0x0000, 0x0000, 0x64C7, 0x4EEA },
    { 0x0000, 0x0000, 0x656A, 0x4E18 },
    { 0x0000, 0x0000, 0x660F, 0x4D40 },
    { 0x0000, 0x0000, 0x66B6, 0x4C61 },
    { 0x0000, 0x0000, 0x6760, 0x4B7A },
    { 0x0000, 0x0000, 0x680C, 0x4A8C },
    { 0x0000, 0x0000, 0x68BC, 0x4995 },
    { 0x0000, 0x0000, 0x696E, 0x4894 },
    { 0x0000, 0x0000, 0x6A24, 0x4789 },
    { 0x0000, 0x0000, 0x6ADE, 0x4673 },
    { 0x0000, 0x0000, 0x6B9B, 0x4550 },
    { 0x0000, 0x0000, 0x6C5D, 0x4420 },
    { 0x0000, 0x0000, 0x6D23, 0x42E1 },
    { 0x0000, 0x0000, 0x6DED, 0x4192 },
    { 0x0000, 0x0000, 0x6EBD, 0x4031 },
    { 0x0000, 0x0000, 0x6F91, 0x3EBD },
    { 0x04D0, 0x0000, 0x6F7D, 0x3EB2 },
    { 0x0968, 0x0000, 0x6F44, 0x3E91 },
    { 0x0DC9, 0x0000, 0x6EEB, 0x3E5F },
    { 0x11F7, 0x0000, 0x6E76, 0x3E1E },
    { 0x15F7, 0x0000, 0x6DEA, 0x3DCF },
    { 0x19CB, 0x0000, 0x6D47, 0x3D73 },
    { 0x1D76, 0x0000, 0x6C92, 0x3D0E },
    { 0x20FB, 0x0000, 0x6BCD, 0x3C9F },
    { 0x245C, 0x0000, 0x6AF8, 0x3C27 },
    { 0x279D, 0x0000, 0x6A17, 0x3BA8 },
    { 0x2ABE, 0x0000, 0x692A, 0x3B23 },
    { 0x2DC2, 0x0000, 0x6832, 0x3A97 },
    { 0x30AB, 0x0000, 0x6730, 0x3A06 },
    { 0x337B, 0x0000, 0x6625, 0x3970 },
    { 0x3633, 0x0000, 0x6512, 0x38D6 },
    { 0x38D5, 0x0000, 0x63F7, 0x3837 },
    { 0x3B62, 0x0000, 0x62D5, 0x3794 },
    { 0x3DDB, 0x0000, 0x61AD, 0x36ED },
    { 0x4042, 0x0000, 0x607D, 0x3642 },
    { 0x4297, 0x0000, 0x5F48, 0x3594 },
    { 0x44DC, 0x0000, 0x5E0C, 0x34E3 },
    { 0x4712, 0x0000, 0x5CCA, 0x342D },
    { 0x4939, 0x0000, 0x5B82, 0x3375 },
    { 0x4B53, 0x0000, 0x5A34, 0x32B9 },
    { 0x4D5F, 0x0000, 0x58E0, 0x31FA },
    { 0x4F60, 0x0000, 0x5786, 0x3138 },
    { 0x5154, 0x0000, 0x5626, 0x3072 },
    { 0x533E, 0x0000, 0x54C0, 0x2FA8 },
    { 0x551D, 0x0000, 0x5353, 0x2EDB },
    { 0x56F3, 0x0000, 0x51E0, 0x2E0A },
    { 0x58BF, 0x0000, 0x5065, 0x2D35 },
    { 0x5A82, 0x0000, 0x4EE4, 0x2C5D },
    { 0x5C3C, 0x0000, 0x4D5B, 0x2B80 },
    { 0x5DEF, 0x0000, 0x4BCA, 0x2A9E },
    { 0x5F99, 0x0000, 0x4A30, 0x29B8 },
    { 0x613B, 0x0000, 0x488F, 0x28CD },
    { 0x62D6, 0x0000, 0x46E4, 0x27DD },
    { 0x646A, 0x0000, 0x452F, 0x26E7 },
    { 0x65F7, 0x0000, 0x4371, 0x25EC },
    { 0x677D, 0x0000, 0x41A7, 0x24EB },
    { 0x68FC, 0x0000, 0x3FD3, 0x23E4 },
    { 0x6A74, 0x0000, 0x3DF2, 0x22D6 },
    { 0x6BE5, 0x0000, 0x3C05, 0x21C0 },
    { 0x6D50, 0x0000, 0x3A0B, 0x20A4 },
    { 0x6EB3, 0x0000, 0x3802, 0x1F7F },
    { 0x700F, 0x0000, 0x35EA, 0x1E51 },
    { 0x7164, 0x0000, 0x33C2, 0x1D1B },
    { 0x72B0, 0x0000, 0x3189, 0x1BDB },
    { 0x73F5, 0x0000, 0x2F3E, 0x1A91 },
    { 0x7530, 0x0000, 0x2CDF, 0x193C },
    { 0x7662, 0x0000, 0x2A6C, 0x17DB },
    { 0x778A, 0x0000, 0x27E2, 0x166E },
    { 0x78A6, 0x0000, 0x2541, 0x14F3 },
    { 0x79B7, 0x0000, 0x2287, 0x136A },
    { 0x7AB9, 0x0000, 0x1FB1, 0x11D2 },
    { 0x7BAD, 0x0000, 0x1CBF, 0x102A },
    { 0x7C90, 0x0000, 0x19AE, 0x0E70 },
    { 0x7D5F, 0x0000, 0x167B, 0x0CA4 },
    { 0x7E19, 0x0000, 0x1325, 0x0AC4 },
    { 0x7EBB, 0x0000, 0x0FA9, 0x08CE },
    { 0x7F41, 0x0000, 0x0C04, 0x06C1 },
    { 0x7FA7, 0x0000, 0x0832, 0x049C },
    { 0x7FE8, 0x0000, 0x0432, 0x025C },
    { 0x8000, 0x0000, 0x0000, 0x0000 },
    { 0x7FE8, 0x04D0, 0x0000, 0x0000 },
    { 0x7FA7, 0x0968, 0x0000, 0x0000 },
    { 0x7F41, 0x0DC9, 0x0000, 0x0000 },
    { 0x7EBB, 0x11F7, 0x0000, 0x0000 },
    { 0x7E19, 0x15F7, 0x0000, 0x0000 },
    { 0x7D5F, 0x19CB, 0x0000, 0x0000 },
    { 0x7C90, 0x1D76, 0x0000, 0x0000 },
    { 0x7BAD, 0x20FB, 0x0000, 0x0000 },
    { 0x7AB9, 0x245C, 0x0000, 0x0000 },
    { 0x79B7, 0x279D, 0x0000, 0x0000 },
    { 0x78A6, 0x2ABE, 0x0000, 0x0000 },
    { 0x778A, 0x2DC2, 0x0000, 0x0000 },
    { 0x7662, 0x30AB, 0x0000, 0x0000 },
    { 0x7530, 0x337B, 0x0000, 0x0000 },
    { 0x73F5, 0x3633, 0x0000, 0x0000 },
    { 0x72B0, 0x38D5, 0x0000, 0x0000 },
    { 0x7164, 0x3B62, 0x0000, 0x0000 },
    { 0x700F, 0x3DDB, 0x0000, 0x0000 },
    { 0x6EB3, 0x4042, 0x0000, 0x0000 },
    { 0x6D50, 0x4297, 0x0000, 0x0000 },
    { 0x6BE5, 0x44DC, 0x0000, 0x0000 },
    { 0x6A74, 0x4712, 0x0000, 0x0000 },
    { 0x68FC, 0x4939, 0x0000, 0x0000 },
    { 0x677D, 0x4B53, 0x0000, 0x0000 },
    { 0x65F7, 0x4D5F, 0x0000, 0x0000 },
    { 0x646A, 0x4F60, 0x0000, 0x0000 },
    { 0x62D6, 0x5154, 0x0000, 0x0000 },
    { 0x613B, 0x533E, 0x0000, 0x0000 },
    { 0x5F99, 0x551D, 0x0000, 0x0000 },
    { 0x5DEF, 0x56F3, 0x0000, 0x0000 },
    { 0x5C3C, 0x58BF, 0x0000, 0x0000 },
};

static SNDDRVMIXCHANNEL __MIXChannel[64];
static int numoutputchannels;

void SNDDRV_DSPMixerResetChannel(SNDDRVMIXCHANNEL *channel) {
    channel->mode = 0x50000000;
    channel->input = 0;
    channel->auxA = 0;
    channel->auxB = 0;
    channel->vBS = 0;
    channel->vBR = 0;
    channel->vBL = 0;
    channel->vAS = 0;
    channel->vAR = 0;
    channel->vAL = 0;
    channel->vS = 0;
    channel->vR = 0;
    channel->vL = 0;
    channel->v = 0;
}

void SNDDRV_DSPMixerSetPan(AXVPB *p, float *balances, unsigned short azimuth, int lfechan) {
    SNDDRVMIXCHANNEL *channel = &__MIXChannel[p->index];

    if (numoutputchannels == 4) {
        int mappedaz = azimuth >> 8;

        if (lfechan == 0) {
            channel->vR1 = SNDDRV_dolbypl2balances[mappedaz][1];
            channel->vBR1 = SNDDRV_dolbypl2balances[mappedaz][3];
            channel->vBL1 = SNDDRV_dolbypl2balances[mappedaz][2];
            channel->vL1 = SNDDRV_dolbypl2balances[mappedaz][0];
        } else {
            channel->vL1 = 0;
            channel->vBL1 = 0;
            channel->vBR1 = 0;
            channel->vR1 = 0;
        }
    } else {
        channel->vR1 = (unsigned short)(balances[0] * 32767.0f);
        channel->vL1 = (unsigned short)(balances[1] * 32767.0f);
    }
    channel->mode |= MIXMODE_SETMIX;
}

void SNDDRV_DSPMixerSetAuxBus(AXVPB *p, unsigned short fxlevel, int auxbus) {
    SNDDRVMIXCHANNEL *channel = &__MIXChannel[p->index];

    if (auxbus == 1) {
        channel->vAR1 = fxlevel;
        channel->vAL1 = fxlevel;
        if (numoutputchannels == 4) {
            channel->vAS1 = fxlevel;
            channel->vBS1 = fxlevel;
        }
    } else if (auxbus == 2) {
        channel->vBR1 = fxlevel;
        channel->vBL1 = fxlevel;
        if (numoutputchannels == 3) {
            channel->vBS1 = fxlevel;
        }
    }
    channel->mode |= MIXMODE_SETMIX;
}

void SNDDRV_DSPMixerInit(int outputchannels) {
    int i;

    for (i = 0; i < 64; i++) {
        SNDDRV_DSPMixerResetChannel(&__MIXChannel[i]);
    }
    numoutputchannels = outputchannels;
}

void SNDDRV_DSPMixerInitChannel(AXVPB *axvpb, unsigned long mode, int input, unsigned short auxA,
                                unsigned short auxB, float *balances, unsigned short azimuth,
                                int lfechan) {
    int old;
    SNDDRVMIXCHANNEL *c = &__MIXChannel[axvpb->index];
    unsigned short mixerCtrl = 0;
    unsigned short *p;

    c->axvpb = axvpb;
    c->mode = mode & MIXMODE_LFE;
    c->input = input;
    SNDDRV_DSPMixerSetAuxBus(axvpb, auxA, 1);
    c->vAL = c->vAL1;
    c->vAR = c->vAR1;
    c->vAS = c->vAS1;
    SNDDRV_DSPMixerSetPan(axvpb, balances, azimuth, lfechan);
    c->vL = c->vL1;
    c->vR = c->vR1;
    c->vBL = c->vBL1;
    c->vBR = c->vBR1;
    if (c->mode & MIXMODE_LFE) {
        c->v = 0;
    } else {
        c->v = input;
    }
    old = OSDisableInterrupts();
    axvpb->pb.ve.currentVolume = c->v;
    axvpb->pb.ve.currentDelta = 0;
    axvpb->pb.mix.vL = c->vL;
    if (axvpb->pb.mix.vL) {
        mixerCtrl |= AX_PB_MIXCTRL_L;
    }
    axvpb->pb.mix.vDeltaL = 0;
    axvpb->pb.mix.vR = c->vR;
    if (axvpb->pb.mix.vR) {
        mixerCtrl |= AX_PB_MIXCTRL_R;
    }
    axvpb->pb.mix.vDeltaR = 0;
    axvpb->pb.mix.vAuxAL = c->vAL;
    if (axvpb->pb.mix.vAuxAL) {
        mixerCtrl |= AX_PB_MIXCTRL_AUXAL;
    }
    axvpb->pb.mix.vDeltaAuxAL = 0;
    axvpb->pb.mix.vAuxAR = c->vAR;
    if (axvpb->pb.mix.vAuxAR) {
        mixerCtrl |= AX_PB_MIXCTRL_AUXAR;
    }
    axvpb->pb.mix.vDeltaAuxAR = 0;
    axvpb->pb.mix.vAuxBL = c->vBL;
    if (axvpb->pb.mix.vAuxBL) {
        mixerCtrl |= AX_PB_MIXCTRL_AUXBL;
    }
    axvpb->pb.mix.vDeltaAuxBL = 0;
    axvpb->pb.mix.vAuxBR = c->vBR;
    if (axvpb->pb.mix.vAuxBR) {
        mixerCtrl |= AX_PB_MIXCTRL_AUXBR;
    }
    axvpb->pb.mix.vDeltaAuxBR = 0;
    axvpb->pb.mix.vAuxBS = c->vBS;
    if (axvpb->pb.mix.vAuxBS) {
        mixerCtrl |= AX_PB_MIXCTRL_AUXBS;
    }
    axvpb->pb.mix.vDeltaAuxBS = 0;
    axvpb->pb.mix.vS = c->vS;
    if (axvpb->pb.mix.vS) {
        mixerCtrl |= AX_PB_MIXCTRL_S;
    }
    axvpb->pb.mix.vDeltaS = 0;
    axvpb->pb.mix.vAuxAS = c->vAS;
    if (axvpb->pb.mix.vAuxAS) {
        mixerCtrl |= AX_PB_MIXCTRL_AUXAS;
    }
    axvpb->pb.mix.vDeltaAuxAS = 0;
    axvpb->pb.mixerCtrl = mixerCtrl;
    axvpb->sync |= AX_SYNC_FLAG_COPYVOL | AX_SYNC_FLAG_COPYAXPBMIX | AX_SYNC_FLAG_COPYMXRCTRL;
    OSRestoreInterrupts(old);
}

void SNDDRV_DSPMixerReleaseChannel(AXVPB *axvpb) {
    __MIXChannel[axvpb->index].axvpb = 0;
}

void SNDDRV_DSPMixerSetVol(AXVPB *p, int IP) {
    SNDDRVMIXCHANNEL *channel = &__MIXChannel[p->index];

    channel->input = IP;
    channel->mode |= MIXMODE_SETINPUT;
}

void SNDDRV_DSPMixerUpdate() {
    int i;

    for (i = 0; i < 64; i++) {
        int setNewInputLevel = 0;
        int setNewMixLevel = 0;
        SNDDRVMIXCHANNEL *c = &__MIXChannel[i];
        AXVPB *axvpb = __MIXChannel[i].axvpb;

        if (axvpb) {
            unsigned short mixerCtrl = 0;

            if (c->mode & MIXMODE_NEWINPUT) {
                c->mode &= ~MIXMODE_NEWINPUT;
                c->v = c->v1;
                setNewInputLevel = 1;
            }
            if (c->mode & MIXMODE_SETINPUT) {
                if (c->mode & MIXMODE_LFE) {
                    c->v1 = 0;
                } else {
                    c->v1 = c->input;
                }
                c->mode &= ~MIXMODE_SETINPUT;
                c->mode |= MIXMODE_NEWINPUT;
                setNewInputLevel = 1;
            }
            if (c->mode & MIXMODE_NEWMIX) {
                setNewMixLevel = 1;
                c->vL = c->vL1;
                c->vR = c->vR1;
                c->vS = c->vS1;
                c->vAL = c->vAL1;
                c->vAR = c->vAR1;
                c->vAS = c->vAS1;
                c->vBL = c->vBL1;
                c->vBR = c->vBR1;
                c->vBS = c->vBS1;
                c->mode &= ~MIXMODE_NEWMIX;
            }
            if (c->mode & MIXMODE_SETMIX) {
                c->mode &= ~MIXMODE_SETMIX;
                c->mode |= MIXMODE_NEWMIX;
                setNewMixLevel = 1;
            }
            if (setNewInputLevel && axvpb) {
                axvpb->pb.ve.currentVolume = c->v1;
                axvpb->pb.ve.currentDelta = 0;
                axvpb->sync |= AX_SYNC_FLAG_COPYVOL;
            }
            if (setNewMixLevel) {
                unsigned short *p;

                axvpb->pb.mix.vL = c->vL;
                if (axvpb->pb.mix.vL) {
                    mixerCtrl |= AX_PB_MIXCTRL_L;
                }
                axvpb->pb.mix.vDeltaL = (c->vL1 - c->vL) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaL) {
                    mixerCtrl |= AX_PB_MIXCTRL_DL;
                }
                axvpb->pb.mix.vR = c->vR;
                if (axvpb->pb.mix.vR) {
                    mixerCtrl |= AX_PB_MIXCTRL_R;
                }
                axvpb->pb.mix.vDeltaR = (c->vR1 - c->vR) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaR) {
                    mixerCtrl |= AX_PB_MIXCTRL_DL;
                }
                axvpb->pb.mix.vAuxAL = c->vAL;
                if (axvpb->pb.mix.vAuxAL) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXAL;
                }
                axvpb->pb.mix.vDeltaAuxAL = (c->vAL1 - c->vAL) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaAuxAL) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXADL;
                }
                axvpb->pb.mix.vAuxAR = c->vAR;
                if (axvpb->pb.mix.vAuxAR) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXAR;
                }
                axvpb->pb.mix.vDeltaAuxAR = (c->vAR1 - c->vAR) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaAuxAR) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXADL;
                }
                axvpb->pb.mix.vAuxBL = c->vBL;
                if (axvpb->pb.mix.vAuxBL) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXBL;
                }
                axvpb->pb.mix.vDeltaAuxBL = (c->vBL1 - c->vBL) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaAuxBL) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXBDL;
                }
                axvpb->pb.mix.vAuxBR = c->vBR;
                if (axvpb->pb.mix.vAuxBR) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXBR;
                }
                axvpb->pb.mix.vDeltaAuxBR = (c->vBR1 - c->vBR) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaAuxBR) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXBDL;
                }
                axvpb->pb.mix.vAuxBS = c->vBS;
                if (axvpb->pb.mix.vAuxBS) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXBS;
                }
                axvpb->pb.mix.vDeltaAuxBS = (c->vBS1 - c->vBS) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaAuxBS) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXBDS;
                }
                axvpb->pb.mix.vS = c->vS;
                if (axvpb->pb.mix.vS) {
                    mixerCtrl |= AX_PB_MIXCTRL_S;
                }
                axvpb->pb.mix.vDeltaS = (c->vS1 - c->vS) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaS) {
                    mixerCtrl |= AX_PB_MIXCTRL_DL;
                }
                axvpb->pb.mix.vAuxAS = c->vAS;
                if (axvpb->pb.mix.vAuxAS) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXAS;
                }
                axvpb->pb.mix.vDeltaAuxAS = (c->vAS1 - c->vAS) / MIXVOLRAMPFRAMES;
                if (axvpb->pb.mix.vDeltaAuxAS) {
                    mixerCtrl |= AX_PB_MIXCTRL_AUXADS;
                }
                if (sndgs.sso.set.outputchannels == 4) {
                    mixerCtrl |= AX_PB_MIXCTRL_SURROUND;
                }
                axvpb->pb.mixerCtrl = mixerCtrl;
                axvpb->sync |= AX_SYNC_FLAG_COPYAXPBMIX | AX_SYNC_FLAG_COPYMXRCTRL;
            }
        }
    }
}
