/*
 * Nintendo Dolphin SDK - vm/VMBase.c
 *
 * Like vm.c this is Metrowerks CodeWarrior code, not SN ProDG GCC: the ELF
 * STT_FILE entry for this range is literally "VMBase.c" (SDK CamelCase), and
 * the unit carries no `gcc2_compiled.` marker.  Built with GC/2.7 and
 * `-O4,p` (libsn_mw in configure.py).
 *
 * El orden de las funciones es el del `.s` del troceador.
 */

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int BOOL;

#define FALSE 0
#define TRUE 1

#define VMBASE_PAGE_TABLE_SIZE 0x10000
#define VMBASE_LOCKED_PAGE_TABLE_SIZE 0x1000
#define VMBASE_REVERSE_PAGE_TABLE_ENTRIES 0x1000

typedef struct VMBASEPageTableEntry {
    u32 word0;
    u32 word1;
} VMBASEPageTableEntry;

typedef struct OSContext {
    u32 gpr[32];      /* 0x000 */
    u32 cr;           /* 0x080 */
    u32 lr;           /* 0x084 */
    u32 ctr;          /* 0x088 */
    u32 xer;          /* 0x08C */
    double fpr[32];   /* 0x090 */
    u32 fpscr_pad;    /* 0x190 */
    u32 fpscr;        /* 0x194 */
    u32 srr0;         /* 0x198 */
    u32 srr1;         /* 0x19C */
    u16 mode;         /* 0x1A0 */
    u16 state;        /* 0x1A2 */
    u32 gqr[8];       /* 0x1A4 */
    u32 psf_pad;      /* 0x1C4 */
    double psf[32];   /* 0x1C8 */
} OSContext;          /* 0x2C8 */

void *OSGetArenaLo(void);
void OSSetArenaLo(void *newLo);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
void DCStoreRange(void *addr, u32 nBytes);
void DCFlushRangeNoSync(void *addr, u32 nBytes);
void OSClearContext(OSContext *context);
void OSSetCurrentContext(OSContext *context);
void OSLoadContext(OSContext *context);

static VMBASEPageTableEntry *g_vmBasePageTable;
static u32 *g_vmBaseVMReversePageTable;
static u8 *g_vmBaseLockedPageTable;
static void (*cbVMSwapPageIn)(u32 addr);
static BOOL g_baseInitialized;
static u32 g_originalSR7;
static u32 g_originalSDR1;

static VMBASEPageTableEntry *__VMBASEVirtualAddrToPageTableAddr(u32 addr);
static void __VMBASEClearPageFromTLB(u32 addr);
static void __VMBASEInitPageTable(void);
static void __VMBASEInitLockedPageTable(void);
static void __VMBASEInitReversePageTable(void);
static void __VMBASEInvalidatePageTable(void);
static void __VMBASEInvalidateLockedPageTable(void);
static void __VMBASEInvalidateReversePageTable(void);
static void __VMBASESetupSDR1(register u32 msr, register u32 sdr1, register u32 unused);
static void __VMBASEDSIExceptionHandler(void);
static void __VMBASEISIExceptionHandler(void);

void VMBASEInit(void (*swapPageInCallback)(u32));
void VMBASESetPageTableEntry(u32 addr, u32 physAddr, u32 page);
void VMBASEClearPageTableEntry(u32 addr, u32 page);
BOOL VMBASEIsPageValid(u32 addr);
BOOL VMBASEIsPageReferenced(u32 addr);
BOOL VMBASEIsPageDirty(u32 addr);
void VMBASESetPageReferenced(u32 addr, BOOL referenced);
u32 VMBASEGetVirtualAddrFromPageInMRAM(u32 page);
void __VMBASESetVirtualAddressForPageInMRAM(u32 page, u32 addr);
BOOL VMBASEIsPageLocked(u32 page);
void VMBASESetPageLocked(u32 page, BOOL locked);
void __VMBASESetSwapPageCallback(void *callback);
void __VMBASEInvalidateEntireTLB(void);
void __VMBASESetupVMRegisters(void);
void __VMBASESetupVMRegisters_SetSDR1(void);
void __VMBASESetupVMRegisters_End(void);
void __VMBASESetupExceptionHandlers(void);
void __VMBASEDSIExceptionHandler_SetOriginalInstruction(void);
void __VMBASEDSIExceptionHandler_SetBranchBack(void);
void __VMBASEDSIServiceExceptionPrep(void);
void __VMBASEDSIServiceException(OSContext *context, u32 addr);
void __VMBASEISIExceptionHandler_SetOriginalInstruction(void);
void __VMBASEISIExceptionHandler_SetBranchBack(void);
void __VMBASEISIServiceExceptionPrep(void);
void __VMBASEISIServiceException(OSContext *context);

void VMBASEInit(void (*swapPageInCallback)(u32))
{
    BOOL enabled;
    u32 free;

    if (g_baseInitialized) {
        return;
    }

    enabled = OSDisableInterrupts();
    g_baseInitialized = TRUE;
    __VMBASESetSwapPageCallback((void *)swapPageInCallback);

    free = VMBASE_PAGE_TABLE_SIZE - ((u32)OSGetArenaLo() & 0xFFFF);
    if (free >= 0x5000) {
        __VMBASEInitLockedPageTable();
        __VMBASEInitReversePageTable();
        __VMBASEInitPageTable();
    } else if (free >= 0x4000) {
        __VMBASEInitReversePageTable();
        __VMBASEInitPageTable();
        __VMBASEInitLockedPageTable();
    } else if (free >= 0x1000) {
        __VMBASEInitLockedPageTable();
        __VMBASEInitPageTable();
        __VMBASEInitReversePageTable();
    } else {
        __VMBASEInitPageTable();
        __VMBASEInitLockedPageTable();
        __VMBASEInitReversePageTable();
    }

    __VMBASESetupExceptionHandlers();
    __VMBASESetupVMRegisters();
    __VMBASEInvalidateEntireTLB();

    OSRestoreInterrupts(enabled);
}

void VMBASESetPageTableEntry(u32 addr, u32 physAddr, u32 page)
{
    VMBASEPageTableEntry *pte;
    BOOL enabled;

    pte = __VMBASEVirtualAddrToPageTableAddr(addr);

    enabled = OSDisableInterrupts();

    pte->word0 = 0x80000000 | ((addr >> 22) & 0x3F);
    pte->word1 = physAddr & 0x0FFFF000;
    DCStoreRange(pte, sizeof(VMBASEPageTableEntry));
    __VMBASEClearPageFromTLB(addr);
    __VMBASESetVirtualAddressForPageInMRAM(page, addr);

    OSRestoreInterrupts(enabled);
}

void VMBASEClearPageTableEntry(u32 addr, u32 page)
{
    BOOL enabled;
    VMBASEPageTableEntry *pte;

    enabled = OSDisableInterrupts();

    pte = __VMBASEVirtualAddrToPageTableAddr(addr);
    pte->word0 = 0;
    pte->word1 = 0;
    DCStoreRange(pte, sizeof(VMBASEPageTableEntry));
    __VMBASEClearPageFromTLB(addr);
    __VMBASESetVirtualAddressForPageInMRAM(page, 0);
    VMBASESetPageLocked(page, FALSE);

    OSRestoreInterrupts(enabled);
}

BOOL VMBASEIsPageValid(u32 addr)
{
    return __VMBASEVirtualAddrToPageTableAddr(addr)->word0 >> 31;
}

BOOL VMBASEIsPageReferenced(u32 addr)
{
    return (__VMBASEVirtualAddrToPageTableAddr(addr)->word1 >> 8) & 1;
}

BOOL VMBASEIsPageDirty(u32 addr)
{
    return (__VMBASEVirtualAddrToPageTableAddr(addr)->word1 >> 7) & 1;
}

void VMBASESetPageReferenced(u32 addr, BOOL referenced)
{
    BOOL enabled;
    VMBASEPageTableEntry *pte;

    enabled = OSDisableInterrupts();

    pte = __VMBASEVirtualAddrToPageTableAddr(addr);
    if (referenced) {
        pte->word1 |= 0x100;
    } else {
        pte->word1 &= ~0x100;
    }
    DCStoreRange(&pte->word1, sizeof(u32));
    __VMBASEClearPageFromTLB(addr);

    OSRestoreInterrupts(enabled);
}

static void __VMBASEClearPageFromTLB(u32 addr)
{
    register u32 ea = addr & 0x0003F000;

    asm {
        tlbsync
        tlbie ea
    }
}

u32 VMBASEGetVirtualAddrFromPageInMRAM(u32 page)
{
    return g_vmBaseVMReversePageTable[page];
}

void __VMBASESetVirtualAddressForPageInMRAM(u32 page, u32 addr)
{
    g_vmBaseVMReversePageTable[page] = addr;
}

BOOL VMBASEIsPageLocked(u32 page)
{
    return g_vmBaseLockedPageTable[page];
}

void VMBASESetPageLocked(u32 page, BOOL locked)
{
    if (locked) {
        g_vmBaseLockedPageTable[page] = TRUE;
    } else {
        g_vmBaseLockedPageTable[page] = FALSE;
    }
}

void __VMBASESetSwapPageCallback(void *callback)
{
    cbVMSwapPageIn = callback;
}

static void __VMBASEInitPageTable(void)
{
    u32 lo = (u32)OSGetArenaLo();

    g_vmBasePageTable = (VMBASEPageTableEntry *)((lo + 0x10000) - (lo & 0xFFFF));
    OSSetArenaLo((void *)((u32)g_vmBasePageTable + VMBASE_PAGE_TABLE_SIZE));
    __VMBASEInvalidatePageTable();
}

static void __VMBASEInitLockedPageTable(void)
{
    g_vmBaseLockedPageTable = (u8 *)OSGetArenaLo();
    OSSetArenaLo((void *)((u32)g_vmBaseLockedPageTable + VMBASE_LOCKED_PAGE_TABLE_SIZE));
    __VMBASEInvalidateLockedPageTable();
}

static void __VMBASEInitReversePageTable(void)
{
    g_vmBaseVMReversePageTable = (u32 *)OSGetArenaLo();
    OSSetArenaLo((void *)((u32)g_vmBaseVMReversePageTable + VMBASE_REVERSE_PAGE_TABLE_ENTRIES * sizeof(u32)));
    __VMBASEInvalidateReversePageTable();
}

static void __VMBASEInvalidatePageTable(void)
{
    BOOL enabled;
    u32 i;

    enabled = OSDisableInterrupts();

    for (i = 0; i < VMBASE_PAGE_TABLE_SIZE; i += sizeof(VMBASEPageTableEntry)) {
        ((VMBASEPageTableEntry *)((u32)g_vmBasePageTable + i))->word0 = 0;
        ((VMBASEPageTableEntry *)((u32)g_vmBasePageTable + i))->word1 = 0;
    }
    DCStoreRange(g_vmBasePageTable, VMBASE_PAGE_TABLE_SIZE);
    __VMBASEInvalidateEntireTLB();

    OSRestoreInterrupts(enabled);
}

static void __VMBASEInvalidateLockedPageTable(void)
{
    u32 i;

    for (i = 0; i < VMBASE_LOCKED_PAGE_TABLE_SIZE; i++) {
        *(u8 *)((u32)g_vmBaseLockedPageTable + i) = 0;
    }
}

static void __VMBASEInvalidateReversePageTable(void)
{
    u32 i;

    for (i = 0; i < VMBASE_REVERSE_PAGE_TABLE_ENTRIES * sizeof(u32); i += sizeof(u32)) {
        *(u32 *)((u32)g_vmBaseVMReversePageTable + i) = 0;
    }
}

static VMBASEPageTableEntry *__VMBASEVirtualAddrToPageTableAddr(u32 addr)
{
    return (VMBASEPageTableEntry *)((((addr >> 22) & 0x7) << 3) |
                                    ((u32)g_vmBasePageTable | (((addr >> 12) & 0x3FF) << 6)));
}

asm void __VMBASEInvalidateEntireTLB(void)
{
    nofralloc
    li      r3, 0
    tlbsync
    li      r0, 8
    mtctr   r0
loop:
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    tlbie   r3
    addi    r3, r3, 0x1000
    bdnz    loop
    blr
}

void __VMBASESetupVMRegisters(void)
{
    register u32 sr7;
    register u32 msr;
    register u32 sdr1;

    asm {
        li      r3, 0
        mfsr    sr7, 7
        mtsr    7, r3
    }
    g_originalSR7 = sr7;
    asm {
        mfmsr   msr
        mfsdr1  sdr1
    }
    g_originalSDR1 = sdr1;
    sdr1 = (sdr1 & 0xFFFF) + ((u32)g_vmBasePageTable & 0x7FFF0000);
    msr &= ~0x30;
    __VMBASESetupSDR1(msr, sdr1, 0);
}

static asm void __VMBASESetupSDR1(register u32 msr, register u32 sdr1, register u32 unused)
{
    nofralloc
    mtsrr1  r3
    lis     r5, __VMBASESetupVMRegisters_SetSDR1@ha
    addi    r5, r5, __VMBASESetupVMRegisters_SetSDR1@l
    clrlwi  r5, r5, 1
    mtsrr0  r5
    rfi
entry __VMBASESetupVMRegisters_SetSDR1
    sync
    mtsdr1  r4
    sync
    mfmsr   r3
    ori     r3, r3, 0x30
    mtsrr1  r3
    lis     r5, __VMBASESetupVMRegisters_End@ha
    addi    r5, r5, __VMBASESetupVMRegisters_End@l
    mtsrr0  r5
    rfi
entry __VMBASESetupVMRegisters_End
    nop
    blr
}

/* UNSOLVED, 380 B al 99,1579%. Es la UNICA que le falta a vmbase; las otras 29
   estan al 100%. Las 95 instrucciones son las mismas y el planificador ya casa
   (el `mr r3,r31` del argumento cae en la misma ranura): las 12 filas que
   quedan son todas NOMBRE DE REGISTRO.

   LO QUE SE ARREGLO (98,7368 -> 99,1579%, de 16 filas a 12): `branch` son
   CUATRO VARIABLES DISTINTAS en el original, no una de funcion. Se ve en que el
   objetivo usa un registro distinto en cada bloque --r6 en el primero, r5 en el
   tercero-- mientras una sola variable las coalesce todas en r6.

   LO QUE QUEDA: el registro de `instruction`. El objetivo la encadena entera
   por r0, el ultimo que reparte mwcc:
       objetivo:  neg r0,r5 / clrlwi r0,r0 / oris r0,r0,0x4800 / stw r0,0(r31)
       nuestro:   neg r5,r5 / clrlwi r5,r5 / oris r6,r5,0x4800 / stw r6,0(r31)

   DIECINUEVE formas medidas, y el contraste es el dato util:
     - siete de declaracion (orden, `register`, agrupadas): identicas. Sin
       ningun `register` NO COMPILA: `asm { icbi p, r0 }` necesita p en registro.
     - cinco de expresion, incluido el OR con el orden cambiado (el eje de mwcc
       de la ronda 20): identicas. La negacion metida en la resta EMPEORA a
       95,7895.
     - cuatro de reparto de pseudos: partir `branch` SIRVE (99,1579), partir
       `instruction` NO (98,7368), y partir las dos da lo mismo que partir solo
       branch.
     - tres de mutar `instruction` en sitio, ya con branch1..4: identicas.
     - una sin la variable `instruction`: EMPEORA a 98,3158 con 22 filas, o sea
       que la variable hace falta.

   PROBADO YA (r31): abrir un bloque `{ }` propio en cada uno de los cuatro
   sitios, con `u32` y con `register u32`. Las dos dan 99,1579% y 380 B, o sea
   IDENTICO a la base. Con eso son VEINTIUNA formas y el eje de la fuente esta
   agotado: lo que queda es que mwcc reparte r0 al temporal y GCC no. */
void __VMBASESetupExceptionHandlers(void)
{
    u32 original;
    register u32 branch1;
    register u32 branch2;
    register u32 branch3;
    register u32 branch4;
    u32 instruction;
    register u32 *p;

    original = *(u32 *)0x80000300;
    branch1 = ((u32)__VMBASEDSIExceptionHandler - 0x80000000) - 0x300;
    instruction = 0x48000000 | branch1;
    *(u32 *)0x80000300 = instruction;
    DCFlushRangeNoSync((void *)0x80000300, 4);
    asm { sync }
    p = (u32 *)0x80000300;
    asm {
        li      r0, 0
        isync
        icbi    p, r0
    }

    p = (u32 *)__VMBASEDSIExceptionHandler_SetOriginalInstruction;
    *p = original;
    DCFlushRangeNoSync(p, 4);
    asm {
        sync
        li      r0, 0
        isync
        icbi    p, r0
    }

    p = (u32 *)__VMBASEDSIExceptionHandler_SetBranchBack;
    branch2 = ((u32)p - 0x80000000) - 0x304;
    instruction = 0x48000000 | ((-(s32)branch2) & 0x03FFFFFF);
    *p = instruction;
    DCFlushRangeNoSync(p, 4);
    asm {
        sync
        li      r0, 0
        isync
        icbi    p, r0
    }

    original = *(u32 *)0x80000400;
    branch3 = ((u32)__VMBASEISIExceptionHandler - 0x80000000) - 0x400;
    instruction = 0x48000000 | branch3;
    *(u32 *)0x80000400 = instruction;
    DCFlushRangeNoSync((void *)0x80000400, 4);
    asm { sync }
    p = (u32 *)0x80000400;
    asm {
        li      r0, 0
        isync
        icbi    p, r0
    }

    p = (u32 *)__VMBASEISIExceptionHandler_SetOriginalInstruction;
    *p = original;
    DCFlushRangeNoSync(p, 4);
    asm {
        sync
        li      r0, 0
        isync
        icbi    p, r0
    }

    p = (u32 *)__VMBASEISIExceptionHandler_SetBranchBack;
    branch4 = ((u32)p - 0x80000000) - 0x404;
    instruction = 0x48000000 | ((-(s32)branch4) & 0x03FFFFFF);
    *p = instruction;
    DCFlushRangeNoSync(p, 4);
    asm {
        sync
        li      r0, 0
        isync
        icbi    p, r0
    }
}

static asm void __VMBASEDSIExceptionHandler(void)
{
    nofralloc
    mtsprg  0, r4
    mtsprg  1, r5
    mtsprg  2, r6
    mfcr    r4
    mtsprg  3, r4
    mfdar   r5
    li      r6, 0x7E
    slwi    r6, r6, 24
    cmplw   r5, r6
    blt     notVM
    mfdar   r5
    li      r6, 0x80
    slwi    r6, r6, 24
    cmplw   r5, r6
    bge     notVM
    mfdsisr r5
    rlwinm  r6, r5, 0, 1, 1
    cmplwi  r6, 0
    beq     notVM

    mfsprg  r4, 3
    mtcrf   255, r4
    mfsprg  r4, 0
    mfsprg  r5, 1
    mfsprg  r6, 2
    mtsprg  0, r4
    lwz     r4, 0xC0(r0)
    stw     r3, 0xC(r4)
    mfsprg  r3, 0
    stw     r3, 0x10(r4)
    stw     r5, 0x14(r4)
    lhz     r3, 0x1A2(r4)
    ori     r3, r3, 0x2
    sth     r3, 0x1A2(r4)
    mfcr    r3
    stw     r3, 0x80(r4)
    mflr    r3
    stw     r3, 0x84(r4)
    mfctr   r3
    stw     r3, 0x88(r4)
    mfxer   r3
    stw     r3, 0x8C(r4)
    mfsrr0  r3
    stw     r3, 0x198(r4)
    mfsrr1  r3
    stw     r3, 0x19C(r4)
    mr      r5, r3
    mfmsr   r3
    ori     r3, r3, 0x30
    mtsrr1  r3
    lwz     r3, 0xD4(r0)
    lis     r5, __VMBASEDSIServiceExceptionPrep@ha
    addi    r5, r5, __VMBASEDSIServiceExceptionPrep@l
    mtsrr0  r5
    rfi

notVM:
    mfsprg  r4, 3
    mtcrf   255, r4
    mfsprg  r4, 0
    mfsprg  r5, 1
    mfsprg  r6, 2
entry __VMBASEDSIExceptionHandler_SetOriginalInstruction
    nop
entry __VMBASEDSIExceptionHandler_SetBranchBack
    nop
}

asm void __VMBASEDSIServiceExceptionPrep(void)
{
    nofralloc
    stw     r0, 0x0(r3)
    stw     r1, 0x4(r3)
    stw     r2, 0x8(r3)
    stmw    r6, 0x18(r3)
    mfspr   r0, GQR1
    stw     r0, 0x1A8(r3)
    mfspr   r0, GQR2
    stw     r0, 0x1AC(r3)
    mfspr   r0, GQR3
    stw     r0, 0x1B0(r3)
    mfspr   r0, GQR4
    stw     r0, 0x1B4(r3)
    mfspr   r0, GQR5
    stw     r0, 0x1B8(r3)
    mfspr   r0, GQR6
    stw     r0, 0x1BC(r3)
    mfspr   r0, GQR7
    stw     r0, 0x1C0(r3)
    mfdar   r4
    b       __VMBASEDSIServiceException
}

void __VMBASEDSIServiceException(OSContext *context, u32 addr)
{
    OSContext exceptionContext;

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    cbVMSwapPageIn(addr);
    OSSetCurrentContext(context);
    OSLoadContext(context);
}

static asm void __VMBASEISIExceptionHandler(void)
{
    nofralloc
    mtsprg  0, r4
    mtsprg  1, r5
    mtsprg  2, r6
    mfcr    r4
    mtsprg  3, r4
    mfsrr0  r5
    li      r6, 0x7E
    slwi    r6, r6, 24
    cmplw   r5, r6
    blt     notVM
    mfsrr0  r5
    li      r6, 0x80
    slwi    r6, r6, 24
    cmplw   r5, r6
    bge     notVM
    mfsrr1  r5
    rlwinm  r6, r5, 0, 1, 1
    cmplwi  r6, 0
    beq     notVM

    mfsprg  r4, 3
    mtcrf   255, r4
    mfsprg  r4, 0
    mfsprg  r5, 1
    mfsprg  r6, 2
    mtsprg  0, r4
    lwz     r4, 0xC0(r0)
    stw     r3, 0xC(r4)
    mfsprg  r3, 0
    stw     r3, 0x10(r4)
    stw     r5, 0x14(r4)
    lhz     r3, 0x1A2(r4)
    ori     r3, r3, 0x2
    sth     r3, 0x1A2(r4)
    mfcr    r3
    stw     r3, 0x80(r4)
    mflr    r3
    stw     r3, 0x84(r4)
    mfctr   r3
    stw     r3, 0x88(r4)
    mfxer   r3
    stw     r3, 0x8C(r4)
    mfsrr0  r3
    stw     r3, 0x198(r4)
    mfsrr1  r3
    stw     r3, 0x19C(r4)
    mr      r5, r3
    mfmsr   r3
    ori     r3, r3, 0x30
    mtsrr1  r3
    lwz     r3, 0xD4(r0)
    lis     r5, __VMBASEISIServiceExceptionPrep@ha
    addi    r5, r5, __VMBASEISIServiceExceptionPrep@l
    mtsrr0  r5
    rfi

notVM:
    mfsprg  r4, 3
    mtcrf   255, r4
    mfsprg  r4, 0
    mfsprg  r5, 1
    mfsprg  r6, 2
entry __VMBASEISIExceptionHandler_SetOriginalInstruction
    nop
entry __VMBASEISIExceptionHandler_SetBranchBack
    nop
}

asm void __VMBASEISIServiceExceptionPrep(void)
{
    nofralloc
    stw     r0, 0x0(r3)
    stw     r1, 0x4(r3)
    stw     r2, 0x8(r3)
    stmw    r6, 0x18(r3)
    mfspr   r0, GQR1
    stw     r0, 0x1A8(r3)
    mfspr   r0, GQR2
    stw     r0, 0x1AC(r3)
    mfspr   r0, GQR3
    stw     r0, 0x1B0(r3)
    mfspr   r0, GQR4
    stw     r0, 0x1B4(r3)
    mfspr   r0, GQR5
    stw     r0, 0x1B8(r3)
    mfspr   r0, GQR6
    stw     r0, 0x1BC(r3)
    mfspr   r0, GQR7
    stw     r0, 0x1C0(r3)
    b       __VMBASEISIServiceException
}

void __VMBASEISIServiceException(OSContext *context)
{
    OSContext exceptionContext;

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    cbVMSwapPageIn(context->srr0);
    OSSetCurrentContext(context);
    OSLoadContext(context);
}
