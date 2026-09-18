#include "Speed/Indep/bWare/Inc/bWare.hpp"

// total size: 0xC
struct VirtualMemoryStats {
    unsigned int fPageIns;  // offset 0x0, size 0x4
    unsigned int fPageOuts; // offset 0x4, size 0x4
    unsigned int fLatency;  // offset 0x8, size 0x4
};

// total size: 0x901C
class VirtualMemory {
  public:
    struct PhysicalPageInfo;

    // total size: 0x10
    struct VirtualPageInfo {
        PhysicalPageInfo *fPhysicalInfo; // offset 0x0, size 0x4
        unsigned int fTotalPageIns;      // offset 0x4, size 0x4
        unsigned int fTotalPageOuts;     // offset 0x8, size 0x4
        unsigned int fDrawFlags;         // offset 0xC, size 0x4
    };

    // total size: 0x4
    struct PhysicalPageInfo {
        VirtualPageInfo *fVirtualInfo; // offset 0x0, size 0x4
    };

    static VirtualMemory &Get() {
        return fgInstance;
    }

    VirtualMemoryStats &GetStats() {
        return fgStats;
    }

    unsigned int GetAddressARAM() {
        return fAddressARAM;
    }

    VirtualMemory();
    ~VirtualMemory();

    void Init(unsigned int offsetARAM, unsigned int sizeARAM, unsigned int sizePhysical);
    void ClearState();
    void InitDefaults();
    void Shutdown();
    static void LogStatsCallback(unsigned long realVirtualAddress, unsigned long physicalAddress, unsigned long pageNumber,
                                 unsigned long pageMissLatency, int pageSwappedOut);
    void Flush();
    void EndFrame();
    void Debug();
    void CreateHeap();
    int GetMemPool();
    void *Alloc(int bytes, const char *name);
    bool IsAllocated(void *pBlock);
    void Free(void *pBlock);
    int GetLargestUnused();
    int GetTotalUnused();

    // El enlazador borro estos dos del binario (el volcado DWARF les da
    // address 0xFFFFFFFF), asi que se declaran y NO se definen: definirlos
    // metería .bss que el objetivo no tiene.
    static VirtualMemoryStats fgStats;
    static bool fgDebugDisplay;
    static VirtualMemory fgInstance;

    unsigned int fAddressARAM;              // offset 0x0, size 0x4
    unsigned int fSizeARAM;                 // offset 0x4, size 0x4
    unsigned int fAddressPhysical;          // offset 0x8, size 0x4
    unsigned int fSizePhysical;             // offset 0xC, size 0x4
    unsigned int fNumPhysicalPages;         // offset 0x10, size 0x4
    PhysicalPageInfo fPhysicalPage[1024];   // offset 0x14, size 0x1000
    unsigned int fNumVirtualPages;          // offset 0x1014, size 0x4
    VirtualPageInfo fVirtualPage[2048];     // offset 0x1018, size 0x8000
    int fMemPoolNum;                        // offset 0x9018, size 0x4
};

VirtualMemory::VirtualMemory() {}

VirtualMemory VirtualMemory::fgInstance;
