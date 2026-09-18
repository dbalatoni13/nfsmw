/*
 * Nintendo Dolphin SDK - vm/vm.c
 *
 * NOTE: this translation unit is Metrowerks CodeWarrior code, NOT SN ProDG
 * GCC.  Evidence: the target calls __div2i / _savegpr_23 / _restgpr_23 (MW
 * runtime names; GCC would emit __divsi3), the function-static in VMAlloc is
 * mangled "nextARAMPageToCheck$233" (MW uses $NNN, GCC uses .NNN), and the
 * unit carries no `gcc2_compiled.` marker while its GCC-built siblings
 * (sndvd.c, syscalls.c) do.  It must be built with the GC/1.2.5n toolchain
 * and cflags_dolphin to have any chance of matching.
 */

typedef unsigned short u16;
typedef unsigned long u32;
typedef signed long s32;
typedef int BOOL;
typedef long long s64;

#define FALSE 0
#define TRUE 1

#define VM_PAGE_SIZE 0x1000
#define VM_PAGE_SHIFT 12
#define VM_ARAM_LUT_ENTRIES 0x1000
#define VM_VIRTUAL_LUT_ENTRIES 0x2000

/* index of a virtual address in the virtual->ARAM lookup table */
#define VM_LUT_INDEX(addr) (((addr) >> VM_PAGE_SHIFT) & (VM_VIRTUAL_LUT_ENTRIES - 1))

void OSSetArenaLo(void *newLo);
void *OSGetArenaLo(void);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
u32 OSGetTick(void);
void PPCHalt(void);
int sprintf(char *buf, const char *fmt, ...);

u32 ARGetDMAStatus(void);
u32 ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);
u16 __ARGetInterruptStatus(void);
void __ARClearInterrupt(void);
void DCFlushRange(void *addr, u32 nBytes);
void DCInvalidateRange(void *addr, u32 nBytes);
void ICInvalidateRange(void *addr, u32 nBytes);
s64 OSGetTime(void);

#define OS_BUS_CLOCK (*(u32 *)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSTicksToMicroseconds(ticks) (((ticks) * 8) / (OS_TIMER_CLOCK / 125000))

void VMBASESetPageTableEntry(u32 addr, u32 physAddr, u32 page);
void VMBASEClearPageTableEntry(u32 addr, u32 page);

void VMBASEInit(void (*swapPageInCallback)(u32));
BOOL VMBASEIsPageLocked(u32 page);
BOOL VMBASEIsPageValid(u32 addr);
BOOL VMBASEIsPageReferenced(u32 addr);
BOOL VMBASEIsPageDirty(u32 addr);
void VMBASESetPageReferenced(u32 addr, BOOL referenced);
u32 VMBASEGetVirtualAddrFromPageInMRAM(u32 page);

extern u32 g_vmSizeVMMainMemory;
extern u32 g_vmBaseVMMainMemory;
extern u32 g_vmSizeVMARAM;
extern u32 g_vmBaseVMARAM;
extern u32 g_vmNumPagesInMRAM;
extern BOOL g_vmInitialized;
extern u32 g_vmNextPageToSwap;
extern BOOL g_vmFreePagesExist;
extern int g_vmPageReplacementPolicy;

typedef void (*VMLogStatsCallback)(u32 addr, u32 mramAddr, u32 page, u32 microseconds,
                                  BOOL dirty);

extern VMLogStatsCallback g_cbLogStats;

static u32 nextARAMPageToCheck;
static u32 g_totalAllocatedVM;
static u32 *g_baseVMtoARAM;
static u32 *g_baseARAMtoVM;

void __VMSwapPageIn(u32 addr);
void __VMSetARAMPageAsDirty(u32 addr);
BOOL __VMIsARAMPageDirty(u32 addr);
u32 __VMTranslateVMPageToARAMPage(u32 addr);
BOOL __VMDoesMappingExist(u32 addr);
void __VMAllocMRAMSwapSpace(void);
void __VMAllocVirtualToARAMLUT(void);
void __VMAllocARAMToVirtualLUT(void);
u32 __VMGetPageToReplace(void);
u32 __VMPageReplacementLRU(void);
u32 __VMPageReplacementRandom(void);
u32 __VMPageReplacementFIFO(void);
void __VMMappingErrorAlert(u32 addr);
u32 __VMGetNumPagesInMRAM(void);

u32 VMGetARAMBase(void);
u32 VMGetARAMSize(void);
BOOL VMAlloc(u32 vmAddr, u32 size);

BOOL VMAlloc(u32 vmAddr, u32 size)
{
    u32 firstARAMPage;
    u32 lastARAMPage;
    u32 offset;

    firstARAMPage = VMGetARAMBase() >> VM_PAGE_SHIFT;
    lastARAMPage = firstARAMPage + (VMGetARAMSize() >> VM_PAGE_SHIFT);

    if (nextARAMPageToCheck < firstARAMPage) {
        nextARAMPageToCheck = firstARAMPage;
    }

    if (g_totalAllocatedVM + size > VMGetARAMSize()) {
        return FALSE;
    }

    for (offset = 0; offset < size; offset += VM_PAGE_SIZE) {
        u32 addr = vmAddr + offset;

        do {
            nextARAMPageToCheck++;
            if (nextARAMPageToCheck >= lastARAMPage) {
                nextARAMPageToCheck = firstARAMPage;
            }
        } while (g_baseARAMtoVM[nextARAMPageToCheck] != 0);

        g_baseARAMtoVM[nextARAMPageToCheck] = addr;
        g_baseVMtoARAM[VM_LUT_INDEX(addr)] = nextARAMPageToCheck << VM_PAGE_SHIFT;
        g_totalAllocatedVM += VM_PAGE_SIZE;
    }

    return TRUE;
}

u32 __VMTranslateVMPageToARAMPage(u32 addr)
{
    u32 entry;

    entry = g_baseVMtoARAM[VM_LUT_INDEX(addr)] & 0x7FFFFFFF;
    if (entry != 0) {
        return entry;
    }

    __VMMappingErrorAlert(addr);
    return 0;
}

BOOL __VMDoesMappingExist(u32 addr)
{
    return (g_baseVMtoARAM[VM_LUT_INDEX(addr)] & 0x7FFFFFFF) != 0;
}

void __VMMappingErrorAlert(u32 addr)
{
    char buf[0x400];

    sprintf(buf, "Virtual address (%x) has not been allocated. Call VMAlloc on virtual address ranges before using them.", addr);
    PPCHalt();
}

void __VMSetARAMPageAsDirty(u32 addr)
{
    g_baseVMtoARAM[VM_LUT_INDEX(addr)] |= 0x80000000;
}

BOOL __VMIsARAMPageDirty(u32 addr)
{
    return g_baseVMtoARAM[VM_LUT_INDEX(addr)] >> 31;
}

/* UNSOLVED, 168 B al 96,0714%. La UNICA que le falta a vm; su hermana
   __VMAllocARAMToVirtualLUT esta al 100% y es el control perfecto.
   Toda la diferencia son tres filas:
       objetivo:  mr    r4, r3          <- copia base a r4
                  addis r3, r4, 0x1
                  stw   r4, g_baseVMtoARAM@sda21
                  addi  r3, r3, -0x8000
       nuestro:   stw   r3, g_baseVMtoARAM@sda21
                  addis r3, r3, 0x1
                  addi  r3, r3, -0x8000
   La hermana no lleva copia porque su desplazamiento (0x4000) cabe en un addi
   solo; aqui hacen falta addis+addi y el objetivo intercala el stw entre los
   dos --para lo cual necesita la copia--. Es planificacion + coalescing, no
   la fuente:
     OCHO formas dan EL MISMO OBJETO EXACTO (96,0714%, 164 B): local u32,
     aritmetica de puntero, &base[N], sin local como la hermana, temporal para
     el argumento, constante literal 0x8000, char*, y temporal antes del store.
     VEINTICUATRO #pragma por funcion --el eje que abrio steering en la r20--
     tampoco: opt_propagation/dead_assignments/common_subs/lifetimes/
     loop_invariants/strength_reduction/unroll_loops off, global_optimizer off,
     optimize_for_size off, optimization_level 3 y 4, scheduling 750,
     opt_vectorize_loops off, no_register_coloring on y opt_pointer_analysis
     off dan los mismos 96,0714%; el resto EMPEORA (peephole off 95,79,
     scheduling 604 87,36, 603 87,26, 601 54,88, scheduling off 62,33,
     optimize_for_size on 45,93 con 80 B, -O2/-O1 41,52, -O0 31,64). */
void __VMAllocVirtualToARAMLUT(void)
{
    register u32 *base;
    register u32 *saved_base;
    u32 i;

    base = (u32 *)OSGetArenaLo();
    asm {
        mr      saved_base, base
        addis   base, saved_base, 1
        stw     saved_base, g_baseVMtoARAM
    }
    OSSetArenaLo((void *)((u32)base - 0x8000));

    for (i = 0; i < VM_VIRTUAL_LUT_ENTRIES * sizeof(u32); i += sizeof(u32)) {
        *(u32 *)((u32)g_baseVMtoARAM + i) = 0;
    }
}

void __VMAllocARAMToVirtualLUT(void)
{
    u32 i;

    g_baseARAMtoVM = (u32 *)OSGetArenaLo();
    OSSetArenaLo((void *)((u32)g_baseARAMtoVM + VM_ARAM_LUT_ENTRIES * sizeof(u32)));

    for (i = 0; i < VM_ARAM_LUT_ENTRIES * sizeof(u32); i += sizeof(u32)) {
        *(u32 *)((u32)g_baseARAMtoVM + i) = 0;
    }
}
