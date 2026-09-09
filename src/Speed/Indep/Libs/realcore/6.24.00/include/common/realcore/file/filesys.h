#ifndef REALCORE_COMMON_FILE_FILESYS_H
#define REALCORE_COMMON_FILE_FILESYS_H

#include "types.h"

typedef int FILEOP;
typedef void FILESYS_CALLBACK(int, int, void *);
typedef int FILESYS_ATOM(int, void *);

#ifdef __cplusplus
extern "C" {
#endif

int FILESYS_completeop(FILEOP ophandle);

FILEOP FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
FILEOP FILESYS_read(intptr_t filehandle, int offset, void *buffer, int bytes, int priority, void *userdata);
FILEOP FILESYS_close(intptr_t filehandle, int timeout, void *userdata);

#ifdef __cplusplus
}
#endif

int FILESYS_opensync(const char *name, unsigned int modeflags, int priority);
int FILESYS_writesync(intptr_t filehandle, int offset, void *buffer, int bytes, int priority);
bool FILESYS_closesync(intptr_t filehandle, int priority);
int FILESYS_size(intptr_t filehandle, int priority, void *userdata);
bool FILESYS_existssync(const char *name, int priority);
void FILESYS_callbackop(FILEOP ophandle, FILESYS_CALLBACK func);

int FILESYS_waitop(FILEOP ophandle);
int64_t FILESYS_completeop64(FILEOP ophandle);
void FILESYS_priorityop(FILEOP ophandle, int priority);

namespace RealFile {

bool GetInfoFastByName(const char *name, const unsigned int modeflags, uint64_t &location, uint64_t &size);

};

#endif
