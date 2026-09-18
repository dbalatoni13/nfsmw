#ifndef REALCORE_COMMON_FILE_FILESYS_H
#define REALCORE_COMMON_FILE_FILESYS_H

#include "types.h"

typedef int FILEOP;
typedef void FILESYS_CALLBACK(int, int, void *);
typedef int FILESYS_ATOM(int, void *);


// NOTA (linkage): las unidades snd compilan con -x c++ y deben emitir llamadas SIN
// decorar (como la libreria C original), de ahi el extern "C". Las TU C++ de realcore
// del binario original SI llaman a las versiones mangladas (FILESYS_close__FiiPv,
// FILESYS_completeop__Fi...); cuando se decompilen, esos .cpp deberan declarar sus
// propios prototipos sin este guard en vez de usar este header.
// filesys_cc.cpp -el shim inverso- define REALCORE_FILESYS_IMPLEMENTATION, que hace lo
// mismo: sus _FILESYS_* mangladas reenvian a las FILESYS_* mangladas de filesys.cpp.
// Una TU C++ que necesite las versiones mangladas define REALCORE_FILESYS_CXX_LINKAGE
// antes de incluir este header (lo hace bFile.cpp). No se pueden declarar los
// prototipos por separado en el .cpp porque chocarian de linkage con estos, y del
// header hacen falta ademas FILEOP y RealFile::GetInfoFastByName.
#if defined(__cplusplus) && !defined(REALCORE_FILESYS_CXX_LINKAGE) && !defined(REALCORE_FILESYS_IMPLEMENTATION)
extern "C" {
#endif

int FILESYS_completeop(FILEOP ophandle);
int FILESYS_opstatus(FILEOP ophandle);

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

#if defined(__cplusplus) && !defined(REALCORE_FILESYS_CXX_LINKAGE) && !defined(REALCORE_FILESYS_IMPLEMENTATION)
}
#endif


namespace RealFile {

bool GetInfoFastByName(const char *name, const unsigned int modeflags, uint64_t &location, uint64_t &size);

};

#endif
