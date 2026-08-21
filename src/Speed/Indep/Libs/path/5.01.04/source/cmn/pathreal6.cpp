#include <types.h>
#define USEREALCORE
#include "path/PathToReal.h"
#undef PATH_vectortoreal6

extern int STREAM_overhead(int requests, int, int);
extern int FILE_exists(const char *filename);
extern int FILE_size(const char *filename);
extern int FILESYS_opensync(const char *filename, unsigned int mode, int);
extern int FILESYS_readsync(int filehandle, int, void *filedata, int filesize, int);
extern int FILESYS_closesync(int filehandle, int);
extern void PATHI_servicetask();
extern void *PATHI_memalloc(int size);
extern void PATHI_memfree(void *ptr);
extern void REAL_abortmessage(const char *format, ...);
extern void SYNCTASK_add(void (*func)(void *, int), int priority, int interval, void *data);
extern void SYNCTASK_del(void (*func)(void *, int));
extern "C" int printf(const char *format, ...);
extern int TIMER_gettick();
extern int TIMERhz;

void PATH_vectortoreal6() {
    if (Path::IPathToReal::realimp != 0) {
        delete Path::IPathToReal::realimp;
    }
    Path::IPathToReal::realimp = new PathToReal6();
}

void *PathToReal6::operator new(PATH_SIZE_T size) {
    return PATHI_memalloc(size);
}

void PathToReal6::operator delete(void *ptr) {
    PATHI_memfree(ptr);
}

PathToReal6::PathToReal6() {
    this->SetAbortMessageFunc(REAL_abortmessage);
    this->SetDebugPrintFunc(printf);
    this->SetLogPrintFunc(0);
    SYNCTASK_add(PathToReal6::TaskService, 5, 100, 0);
}

PathToReal6::~PathToReal6() {
    this->SetAbortMessageFunc(0);
    this->SetDebugPrintFunc(0);
    this->SetLogPrintFunc(0);
    SYNCTASK_del(PathToReal6::TaskService);
}

void PathToReal6::TaskService(void *, int) {
    PATHI_servicetask();
}

int PathToReal6::GetMinStreamBufferSize(int requests) {
    int minbufsize;

    minbufsize = STREAM_overhead(requests, 0, 0);
    return minbufsize + 0x1800;
}

unsigned int PathToReal6::GetMilliseconds() {
    float ms;

    ms = static_cast<float>(TIMER_gettick()) / static_cast<float>(TIMERhz);
    ms *= 1000.0f;
    return ms < 2147483648.0 ? static_cast<int>(ms)
                              : static_cast<int>(ms - 2147483648.0) ^ 0x80000000;
}

int PathToReal6::FileExists(const char *filename) {
    return FILE_exists(filename);
}

int PathToReal6::FileSize(const char *filename) {
    return FILE_size(filename);
}

char *PathToReal6::LoadFile(const char *filepath, int &fileop, int filesize) {
    char *filedata;
    int result;
    int filehandle;

    if (filesize == 0) {
        filesize = FILE_size(filepath);
    }
    if (filesize == 0) {
        return 0;
    }
    filedata = static_cast<char *>(PATHI_memalloc(filesize));
    if (filedata != 0) {
        filehandle = FILESYS_opensync(filepath, 1, 0);
        result = FILESYS_readsync(filehandle, 0, filedata, filesize, 0);
        result = FILESYS_closesync(filehandle, 0);
    }
    fileop++;
    return filedata;
}

char *PathToReal6::LoadFileSync(const char *filepath, int filesize) {
    int fileop;

    fileop = 0;
    return this->PathToReal6::LoadFile(filepath, fileop, filesize);
}

int PathToReal6::LoadFileDone(int, char *&) {
    return 1;
}
