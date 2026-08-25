#include "../../../include/common/realcore/file/filesys.h"

extern int _FILESYS_opstatus(int ophandle);
extern int _FILESYS_completeop(int ophandle);
extern int _FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
extern int _FILESYS_close(int filehandle, int timeout, void *userdata);
extern int _FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                         void *userdata);
extern int _STREAM_overhead(int requests, int, int);
extern int _STREAM_create(int, int, int, void *, int);
extern void _STREAM_destroy(int);
extern void _STREAM_setgreedylevel(int, int);
extern int _STREAM_queuefile(int, const char *, int, int);
extern int _STREAM_queuemem(int, void *, int, int);
extern void _STREAM_kill(int);
extern void *_STREAM_get(int);
struct STREAMCHUNKHDR;
extern void _STREAM_release(int, STREAMCHUNKHDR *);
extern int _STREAM_gettable(int);
extern int _STREAM_state(int);
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

extern "C" int FILESYS_close(int filehandle, int timeout, void *userdata) {
    return _FILESYS_close(filehandle, timeout, userdata);
}

extern "C" int FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                            void *userdata) {
    return _FILESYS_read(filehandle, offset, buffer, bytes, priority, userdata);
}

extern "C" int STREAM_overhead(int requests, int type, int flags) {
    return _STREAM_overhead(requests, type, flags);
}

extern "C" int STREAM_create(int type, int flags, int requests, void *buffer, int buffersize) {
    return _STREAM_create(type, flags, requests, buffer, buffersize);
}

extern "C" void STREAM_destroy(int stream) {
    _STREAM_destroy(stream);
}

extern "C" void STREAM_setgreedylevel(int stream, int level) {
    _STREAM_setgreedylevel(stream, level);
}

extern "C" int STREAM_queuefile(int stream, const char *name, int priority, int flags) {
    return _STREAM_queuefile(stream, name, priority, flags);
}

extern "C" int STREAM_queuemem(int stream, void *buffer, int size, int flags) {
    return _STREAM_queuemem(stream, buffer, size, flags);
}

extern "C" void STREAM_kill(int stream) {
    _STREAM_kill(stream);
}

extern "C" void *STREAM_get(int stream) {
    return _STREAM_get(stream);
}

extern "C" void STREAM_release(int stream, STREAMCHUNKHDR *chunk) {
    _STREAM_release(stream, chunk);
}

extern "C" int STREAM_gettable(int stream) {
    return _STREAM_gettable(stream);
}

extern "C" int STREAM_state(int stream) {
    return _STREAM_state(stream);
}

extern "C" int STREAM_buffersize(int stream) {
    return _STREAM_buffersize(stream);
}
