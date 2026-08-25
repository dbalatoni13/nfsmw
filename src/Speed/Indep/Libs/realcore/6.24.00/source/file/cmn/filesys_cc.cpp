#define REALCORE_FILESYS_IMPLEMENTATION
#include "../../../include/common/realcore/file/filesys.h"
#undef REALCORE_FILESYS_IMPLEMENTATION

struct STREAMCHUNKHDR;
extern int STREAM_overhead(int requests, int, int);
extern int STREAM_create(int, int, int, void *, int);
extern void STREAM_destroy(int);
extern void STREAM_setgreedylevel(int, int);
extern int STREAM_queuefile(int, const char *, int, int);
extern int STREAM_queuemem(int, void *, int, int);
extern void STREAM_kill(int);
extern void *STREAM_get(int);
extern void STREAM_release(int, STREAMCHUNKHDR *);
extern int STREAM_gettable(int);
extern int STREAM_state(int);
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

int _FILESYS_close(int filehandle, int timeout, void *userdata) {
    return FILESYS_close(filehandle, timeout, userdata);
}

int _FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                  void *userdata) {
    return FILESYS_read(filehandle, offset, buffer, bytes, priority, userdata);
}

int _STREAM_overhead(int requests, int type, int flags) {
    return STREAM_overhead(requests, type, flags);
}

int _STREAM_create(int type, int flags, int requests, void *buffer, int buffersize) {
    return STREAM_create(type, flags, requests, buffer, buffersize);
}

void _STREAM_destroy(int stream) {
    STREAM_destroy(stream);
}

void _STREAM_setgreedylevel(int stream, int level) {
    STREAM_setgreedylevel(stream, level);
}

int _STREAM_queuefile(int stream, const char *name, int priority, int flags) {
    return STREAM_queuefile(stream, name, priority, flags);
}

int _STREAM_queuemem(int stream, void *buffer, int size, int flags) {
    return STREAM_queuemem(stream, buffer, size, flags);
}

void _STREAM_kill(int stream) {
    STREAM_kill(stream);
}

void *_STREAM_get(int stream) {
    return STREAM_get(stream);
}

void _STREAM_release(int stream, STREAMCHUNKHDR *chunk) {
    STREAM_release(stream, chunk);
}

int _STREAM_gettable(int stream) {
    return STREAM_gettable(stream);
}

int _STREAM_state(int stream) {
    return STREAM_state(stream);
}

int _STREAM_buffersize(int stream) {
    return STREAM_buffersize(stream);
}
