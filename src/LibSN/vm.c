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








extern u32 g_vmNextPageToSwap;
extern BOOL g_vmFreePagesExist;
extern int g_vmPageReplacementPolicy;
extern u32 g_totalAllocatedVM;
extern u32 *g_baseVMtoARAM;
extern u32 *g_baseARAMtoVM;
typedef void (*VMLogStatsCallback)(u32 addr, u32 mramAddr, u32 page, u32 microseconds,
                                  BOOL dirty);



static u32 nextPageToCheck;
static u32 g_vmBaseVMARAM = 0x4000;
static BOOL g_vmInitialized;
static VMLogStatsCallback g_cbLogStats;
static u32 g_vmNumPagesInMRAM;
static u32 g_vmSizeVMARAM;
static u32 g_vmBaseVMMainMemory;
static u32 g_vmSizeVMMainMemory;

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
void VMInit(u32 mram, u32 aramBase, u32 aramSize);

void VMInit(u32 mram, u32 aramBase, u32 aramSize)
{
    BOOL enabled;

    if (!g_vmInitialized) {
        enabled = OSDisableInterrupts();
        g_vmInitialized = TRUE;
        g_vmBaseVMARAM = aramBase;
        g_vmSizeVMARAM = aramSize;
        g_vmSizeVMMainMemory = mram;
        g_vmNumPagesInMRAM = mram >> VM_PAGE_SHIFT;
        VMBASEInit(__VMSwapPageIn);
        __VMAllocVirtualToARAMLUT();
        __VMAllocARAMToVirtualLUT();
        __VMAllocMRAMSwapSpace();
        OSRestoreInterrupts(enabled);
    }
}

u32 __VMGetNumPagesInMRAM(void)
{
    return g_vmNumPagesInMRAM;
}

u32 VMGetARAMSize(void)
{
    return g_vmSizeVMARAM;
}

u32 VMGetARAMBase(void)
{
    return g_vmBaseVMARAM;
}

void __VMAllocMRAMSwapSpace(void)
{
    g_vmBaseVMMainMemory = (u32)OSGetArenaLo();
    OSSetArenaLo((void *)(g_vmBaseVMMainMemory + g_vmSizeVMMainMemory));
}

void __VMSwapPageIn(u32 addr)
{
    u32 startTime;
    u32 vmPage;
    u32 mramAddr;
    BOOL wasDirty;
    u32 page;
    int intStatus;
    BOOL enabled;
    u32 oldAddr;

    startTime = OSTicksToMicroseconds(OSGetTime());
    vmPage = addr & ~(VM_PAGE_SIZE - 1);
    wasDirty = FALSE;

    page = __VMGetPageToReplace();
    mramAddr = g_vmBaseVMMainMemory + (page << VM_PAGE_SHIFT);
    oldAddr = VMBASEGetVirtualAddrFromPageInMRAM(page);

    enabled = OSDisableInterrupts();

    do {
    } while (ARGetDMAStatus() != 0);

    intStatus = __ARGetInterruptStatus();

    if (oldAddr != 0) {
        if (VMBASEIsPageDirty(oldAddr)) {
            __VMSetARAMPageAsDirty(oldAddr);
            wasDirty = TRUE;
            DCFlushRange((void *)mramAddr, VM_PAGE_SIZE);
            ARStartDMA(0, mramAddr, __VMTranslateVMPageToARAMPage(oldAddr), VM_PAGE_SIZE);
            do {
            } while (ARGetDMAStatus() != 0);
        }
        VMBASEClearPageTableEntry(oldAddr, page);
    }

    if (__VMIsARAMPageDirty(vmPage)) {
        ARStartDMA(1, mramAddr, __VMTranslateVMPageToARAMPage(vmPage), VM_PAGE_SIZE);
        do {
        } while (ARGetDMAStatus() != 0);
        DCInvalidateRange((void *)mramAddr, VM_PAGE_SIZE);
        ICInvalidateRange((void *)mramAddr, VM_PAGE_SIZE);
    } else if (!__VMDoesMappingExist(vmPage)) {
        __VMMappingErrorAlert(vmPage);
    }

    if (intStatus == 0) {
        __ARClearInterrupt();
    }

    VMBASESetPageTableEntry(vmPage, mramAddr, page);
    OSRestoreInterrupts(enabled);

    if (g_cbLogStats != 0) {
        g_cbLogStats(addr, mramAddr, page,
                     (u32)OSTicksToMicroseconds(OSGetTime()) - startTime, wasDirty);
    }
}
