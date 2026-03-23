#ifndef REALCORE_COMMON_FILE_FILESYS_H
#define REALCORE_COMMON_FILE_FILESYS_H

#include "types.h"

typedef int FILEOP;
typedef void FILESYS_CALLBACK(int, int, void *);
typedef int FILESYS_ATOM(int, void *);

int FILESYS_completeop(FILEOP ophandle);
FILEOP FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
FILEOP FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority, void *userdata);
int FILESYS_opensync(const char *name, unsigned int modeflags, int priority);
bool FILESYS_closesync(int filehandle, int priority);
int FILESYS_close(int filehandle, int timeout, void *userdata);
int FILESYS_writesync(int filehandle, int offset, void *buffer, int bytes, int priority);
int FILESYS_waitop(FILEOP ophandle);
int FILESYS_size(int filehandle, int priority, void *userdata);
bool FILESYS_existssync(const char *name, int priority);
void FILESYS_callbackop(FILEOP ophandle, FILESYS_CALLBACK func);

namespace RealFile {

bool GetInfoFastByName(const char *name, const unsigned int modeflags, uint64_t &location, uint64_t &size);

};

#endif
