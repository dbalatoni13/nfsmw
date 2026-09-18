// Prototipos locales (linkage C++ manglado, como la TU original).
void MEM_clear(void *ptr, int size);
void MEM_copy(void *dst, const void *src, int size);

struct FILESYSOPTS {
    int size;                              // +0x00
    struct IAllocator *allocator;          // +0x04
    int MaxOpenFiles;                      // +0x08
    int MaxFileOps;                        // +0x0C
    int nSearchLocs;                       // +0x10
    int nSearchPathLength;                 // +0x14
    int MaxDevices;                        // +0x18
    int ThreadStackSize;                   // +0x1C
    int (*decompresssize)(const void *);   // +0x20
    int (*decompress)(const void *, void *); // +0x24
    unsigned int LargeReadSliceSize;       // +0x28
    unsigned int AllocAlignBoundary;       // +0x2C
    int DiscType;                          // +0x30
    int mErrorRetryCount;                  // +0x34
};

struct TVPALIGN {
    int mAlign;
    int mTvp;
    int mFlag;

    TVPALIGN() {
        mAlign = 2;
        mTvp = 0x80;
        mFlag = 0;
    }
};

extern FILESYSOPTS gFileSysOpts;
extern int bIsFileSysInitialized;
extern TVPALIGN FileAlignTvp;

int FILE_getopts(FILESYSOPTS *opts) {
    int size = opts->size;

    if (size > 56) {
        MEM_clear(opts, size);
    }
    MEM_copy(opts, &gFileSysOpts, size);
    return 1;
}

int FILE_setopts(FILESYSOPTS *opts) {
    if (opts->size <= 55) {
        MEM_clear(&gFileSysOpts, 56);
    }
    MEM_copy(&gFileSysOpts, opts, opts->size);
    FileAlignTvp.mTvp = opts->AllocAlignBoundary;
    return 1;
}

FILESYSOPTS gFileSysOpts = {
    56, 0, 8, 0x80, 2, 0x400, 4, 0x1000, 0, 0, 0x8000, 0x80, 1, 0,
};
int bIsFileSysInitialized = 0;
TVPALIGN FileAlignTvp;
