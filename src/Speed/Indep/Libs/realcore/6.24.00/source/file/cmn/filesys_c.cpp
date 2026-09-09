#include "../../../include/common/realcore/file/filesys.h"

extern int _FILESYS_opstatus(int ophandle);
extern int _FILESYS_completeop(int ophandle);
extern int _FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
extern int _FILESYS_close(int filehandle, int priority, void *userdata);
extern int _FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                         void *userdata);
extern int _STREAM_overhead(int requests, int, int);
extern int _STREAM_create(int, int, int, void *, int);
extern void _STREAM_destroy(int);
extern void _STREAM_setgreedylevel(int, int);
extern int _STREAM_queuefile(int, const char *, int, int);
extern int _STREAM_queuemem(int, void *, int, int);
extern void _STREAM_kill(int);
extern STREAMCHUNKHDR *_STREAM_get(int);
extern void _STREAM_release(int, STREAMCHUNKHDR *);
extern int _STREAM_gettable(int);
extern STREAMSTATE _STREAM_state(int);
extern int _STREAM_buffersize(int);

extern "C" int FILESYS_opstatus(int ophandle) {
    return _FILESYS_opstatus(ophandle);
}

extern "C" int FILESYS_completeop(int ophandle) {
    return _FILESYS_completeop(ophandle);
}

extern "C" int FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata) {
    return _FILESYS_open(name, modeflags, priority, userdata);
}

extern "C" int FILESYS_close(int filehandle, int priority, void * userdata) {
    return _FILESYS_close(filehandle, priority, userdata);
}

extern "C" int FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                            void *userdata) {
    return _FILESYS_read(filehandle, offset, buffer, bytes, priority, userdata);
}

extern "C" int STREAM_overhead(int requests, int filters, int taps) {
    return _STREAM_overhead(requests, filters, taps);
}

extern "C" int STREAM_create(int requests, int filters, int taps, void * buffer, int size) {
    return _STREAM_create(requests, filters, taps, buffer, size);
}

extern "C" void STREAM_destroy(int handle) {
    _STREAM_destroy(handle);
}

extern "C" void STREAM_setgreedylevel(int handle, int greedylevel) {
    _STREAM_setgreedylevel(handle, greedylevel);
}

extern "C" int STREAM_queuefile(int handle, const char * fname, int offset, int endchunkid) {
    return _STREAM_queuefile(handle, fname, offset, endchunkid);
}

extern "C" int STREAM_queuemem(int handle, void * address, int length, int endchunkid) {
    return _STREAM_queuemem(handle, address, length, endchunkid);
}

extern "C" void STREAM_kill(int handle) {
    _STREAM_kill(handle);
}

extern "C" struct STREAMCHUNKHDR * STREAM_get(int handle) {
    return _STREAM_get(handle);
}

extern "C" void STREAM_release(int handle, struct STREAMCHUNKHDR * chunk) {
    _STREAM_release(handle, chunk);
}

extern "C" int STREAM_gettable(int handle) {
    return _STREAM_gettable(handle);
}

extern "C" enum STREAMSTATE STREAM_state(int handle) {
    return _STREAM_state(handle);
}

extern "C" int STREAM_buffersize(int handle) {
    return _STREAM_buffersize(handle);
}
