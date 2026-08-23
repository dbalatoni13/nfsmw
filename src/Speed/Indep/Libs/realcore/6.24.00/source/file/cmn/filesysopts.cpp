#include "../../../include/common/realcore/file/filesys.h"
#include "../../../include/common/realcore/std.h"

FILESYSOPTS gFileSysOpts = {
    sizeof(FILESYSOPTS),
    nullptr,
    8,
    128,
    2,
    1024,
    4,
    4096,
    nullptr,
    nullptr,
    32768,
    128,
    1,
    0,
};

EA::TagValuePair FileAlignTvp(EA::Allocator::ATT_ALIGNMENT, 128U);
bool bIsFileSysInitialized = false;

bool FILE_getopts(FILESYSOPTS *pfso) {
    int ssize = pfso->size;
    if (ssize > static_cast<int>(sizeof(FILESYSOPTS))) {
        MEM_clear(pfso, ssize);
    }
    MEM_copy(pfso, &gFileSysOpts, ssize);
    return true;
}

bool FILE_setopts(FILESYSOPTS *pfso) {
    if (pfso->size < static_cast<int>(sizeof(FILESYSOPTS))) {
        MEM_clear(&gFileSysOpts, sizeof(FILESYSOPTS));
    }
    MEM_copy(&gFileSysOpts, pfso, pfso->size);
    FileAlignTvp.mValue.mSize = pfso->AllocAlignBoundary;
    return true;
}
