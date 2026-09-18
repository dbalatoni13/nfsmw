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



extern u32 g_totalAllocatedVM;
extern u32 *g_baseVMtoARAM;
extern u32 *g_baseARAMtoVM;
typedef void (*VMLogStatsCallback)(u32 addr, u32 mramAddr, u32 page, u32 microseconds,
                                  BOOL dirty);

extern VMLogStatsCallback g_cbLogStats;

static BOOL g_vmFreePagesExist = TRUE;
static int g_vmPageReplacementPolicy = 1;
static u32 g_vmNextPageToSwap;

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

u32 __VMGetPageToReplace(void)
{
    if (g_vmPageReplacementPolicy == 0) {
        return __VMPageReplacementLRU();
    } else if (g_vmPageReplacementPolicy == 1) {
        return __VMPageReplacementRandom();
    } else {
        return __VMPageReplacementFIFO();
    }
}

u32 __VMPageReplacementLRU(void)
{
    u32 startPage = g_vmNextPageToSwap;
    u32 page = 0;
    s32 notRefDirty = -1;
    s32 refNotDirty = -1;
    s32 refDirty = -1;

    if (!g_vmFreePagesExist) {
        for (;;) {
            u32 addr;
            BOOL referenced;
            BOOL dirty;

            addr = VMBASEGetVirtualAddrFromPageInMRAM(g_vmNextPageToSwap);
            if (addr == 0 || !VMBASEIsPageValid(addr)) {
                goto found;
            }

            referenced = VMBASEIsPageReferenced(addr);
            dirty = VMBASEIsPageDirty(addr);

            if (!referenced && !dirty && !VMBASEIsPageLocked(g_vmNextPageToSwap)) {
                page = g_vmNextPageToSwap;
                break;
            }

            if (!referenced && dirty) {
                if (notRefDirty < 0 && !VMBASEIsPageLocked(g_vmNextPageToSwap)) {
                    notRefDirty = g_vmNextPageToSwap;
                }
            } else if (referenced && !dirty) {
                if (refNotDirty < 0 && !VMBASEIsPageLocked(g_vmNextPageToSwap)) {
                    refNotDirty = g_vmNextPageToSwap;
                }
            } else {
                if (refDirty < 0 && !VMBASEIsPageLocked(g_vmNextPageToSwap)) {
                    refDirty = g_vmNextPageToSwap;
                }
            }

            if (referenced) {
                VMBASESetPageReferenced(addr, FALSE);
            }

            if (startPage == g_vmNextPageToSwap) {
                if (notRefDirty >= 0) {
                    page = notRefDirty;
                } else if (refNotDirty >= 0) {
                    page = refNotDirty;
                } else if (refDirty >= 0) {
                    page = refDirty;
                }
                break;
            }

            g_vmNextPageToSwap++;
            if (g_vmNextPageToSwap >= __VMGetNumPagesInMRAM()) {
                g_vmNextPageToSwap = 0;
            }
        }
        goto done;
    found:
        page = g_vmNextPageToSwap;
    } else {
        page = startPage;
    }
done:

    g_vmNextPageToSwap++;
    if (g_vmNextPageToSwap >= __VMGetNumPagesInMRAM()) {
        g_vmFreePagesExist = FALSE;
        g_vmNextPageToSwap = 0;
    }

    return page;
}

u32 __VMPageReplacementRandom(void)
{
    u32 page;

    do {
        if (g_vmFreePagesExist) {
            page = g_vmNextPageToSwap++;
            if (g_vmNextPageToSwap >= __VMGetNumPagesInMRAM()) {
                g_vmFreePagesExist = FALSE;
                g_vmNextPageToSwap = 0;
            }
        } else {
            page = OSGetTick() % __VMGetNumPagesInMRAM();
        }
    } while (VMBASEIsPageLocked(page));

    return page;
}

u32 __VMPageReplacementFIFO(void)
{
    u32 page;

    do {
        page = g_vmNextPageToSwap++;
        if (g_vmNextPageToSwap >= __VMGetNumPagesInMRAM()) {
            g_vmFreePagesExist = FALSE;
            g_vmNextPageToSwap = 0;
        }
    } while (VMBASEIsPageLocked(page));

    return page;
}
