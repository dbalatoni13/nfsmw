#ifndef REALCORE_COMMON_FILE_FILESYS_H
#define REALCORE_COMMON_FILE_FILESYS_H

#include "Allocator/iallocator.h"
#include "types.h"

typedef int FILEOP;
typedef void FILESYS_CALLBACK(int, int, void *);
typedef int FILESYS_ATOM(int, void *);

struct FILESYSOPTS {
    int size;
    EA::Allocator::IAllocator *allocator;
    int MaxOpenFiles;
    int MaxFileOps;
    int nSearchLocs;
    int nSearchPathLength;
    int MaxDevices;
    int ThreadStackSize;
    int (*decompresssize)(const void *);
    int (*decompress)(const void *, void *);
    unsigned int LargeReadSliceSize;
    unsigned int AllocAlignBoundary;
    int DiscType;
    int mErrorRetryCount;
};

#ifdef __cplusplus
extern "C" {
#endif

int FILESYS_completeop(FILEOP ophandle);

FILEOP FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
FILEOP FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority, void *userdata);
FILEOP FILESYS_readlarge(int filehandle, unsigned long long offset, void *buffer, unsigned long long bytes, int priority, void *userdata);
FILEOP FILESYS_write(int filehandle, int offset, void *buffer, int bytes, int priority, void *userdata);
int FILESYS_close(int filehandle, int timeout, void *userdata);
FILEOP FILESYS_size(int filehandle, int priority, void *userdata);
FILEOP FILESYS_exists(const char *name, int priority, void *userdata);

#ifdef __cplusplus
}
#endif

bool FILE_getopts(FILESYSOPTS *pfso);
bool FILE_setopts(FILESYSOPTS *pfso);
int FILESYS_opensync(const char *name, unsigned int modeflags, int priority);
int FILESYS_readsync(int filehandle, int offset, void *buffer, int bytes, int priority);
int FILESYS_sizesync(int filehandle, int priority);
int FILESYS_writesync(int filehandle, int offset, void *buffer, int bytes, int priority);
bool FILESYS_closesync(int filehandle, int priority);
int FILESYS_waitop(FILEOP ophandle);
int FILESYS_size(int filehandle, int priority, void *userdata);
bool FILESYS_existssync(const char *name, int priority);
void FILESYS_callbackop(FILEOP ophandle, FILESYS_CALLBACK func);
bool FILE_exists(const char *name);
int FILE_size(const char *name);

namespace RealFile {

bool GetInfoFastByName(const char *name, const unsigned int modeflags, uint64_t &location, uint64_t &size);

};

#endif
