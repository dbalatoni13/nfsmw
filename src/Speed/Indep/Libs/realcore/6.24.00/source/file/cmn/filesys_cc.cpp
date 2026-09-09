#define REALCORE_FILESYS_IMPLEMENTATION
#include "../../../include/common/realcore/file/filesys.h"
#undef REALCORE_FILESYS_IMPLEMENTATION

extern int STREAM_overhead(int requests, int, int);
extern int STREAM_create(int, int, int, void *, int);
extern void STREAM_destroy(int);
extern void STREAM_setgreedylevel(int, int);
extern int STREAM_queuefile(int, const char *, int, int);
extern int STREAM_queuemem(int, void *, int, int);
extern void STREAM_kill(int);
extern STREAMCHUNKHDR *STREAM_get(int);
extern void STREAM_release(int, STREAMCHUNKHDR *);
extern int STREAM_gettable(int);
extern STREAMSTATE STREAM_state(int);
extern int STREAM_buffersize(int);

int _FILESYS_opstatus(int ophandle) {
    return FILESYS_opstatus(ophandle);
}

int _FILESYS_completeop(int ophandle) {
    return FILESYS_completeop(ophandle);
}

int _FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata) {
    return FILESYS_open(name, modeflags, priority, userdata);
}

int _FILESYS_close(int filehandle, int priority, void * userdata) {
    return FILESYS_close(filehandle, priority, userdata);
}

int _FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                  void *userdata) {
    return FILESYS_read(filehandle, offset, buffer, bytes, priority, userdata);
}

int _STREAM_overhead(int requests, int filters, int taps) {
    return STREAM_overhead(requests, filters, taps);
}

int _STREAM_create(int requests, int filters, int taps, void * buffer, int size) {
    return STREAM_create(requests, filters, taps, buffer, size);
}

void _STREAM_destroy(int handle) {
    STREAM_destroy(handle);
}

void _STREAM_setgreedylevel(int handle, int greedylevel) {
    STREAM_setgreedylevel(handle, greedylevel);
}

int _STREAM_queuefile(int handle, const char * fname, int offset, int endchunkid) {
    return STREAM_queuefile(handle, fname, offset, endchunkid);
}

int _STREAM_queuemem(int handle, void * address, int length, int endchunkid) {
    return STREAM_queuemem(handle, address, length, endchunkid);
}

void _STREAM_kill(int handle) {
    STREAM_kill(handle);
}

struct STREAMCHUNKHDR * _STREAM_get(int handle) {
    return STREAM_get(handle);
}

void _STREAM_release(int handle, struct STREAMCHUNKHDR * chunk) {
    STREAM_release(handle, chunk);
}

int _STREAM_gettable(int handle) {
    return STREAM_gettable(handle);
}

enum STREAMSTATE _STREAM_state(int handle) {
    return STREAM_state(handle);
}

int _STREAM_buffersize(int handle) {
    return STREAM_buffersize(handle);
}
