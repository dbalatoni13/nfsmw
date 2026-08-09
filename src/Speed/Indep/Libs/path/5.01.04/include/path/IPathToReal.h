#ifndef __IPATHREALH__
#define __IPATHREALH__ // Decl: 2

#include "path/path.h"

#include <cstddef>

#define PATH_SIZE_T size_t // Decl: 9

namespace Path {

// Decl: 19
typedef enum SynchMode {
    FILELOAD_SYNC = 0,
    FILELOAD_ASYNC = 1,
} SynchMode;

// total size: 0x14
// Decl: 41
class IPathToReal {
  public:
    static IPathToReal *realimp; // size: 0x4, address: 0x804FF5F8, Decl: 44
    static char *abortfilename;  // size: 0x4, address: 0xFFFFFFFF
    static int abortfileline;    // size: 0x4, address: 0xFFFFFFFF

    IPathToReal() {} // Decl: 48

    virtual ~IPathToReal() {} // Decl: 54

    virtual void SetAbortMessageFunc(PATHAbortMsgFunc f) {} // Decl: 68

    virtual void SetDebugPrintFunc(PATHDebugPrintFunc f) {} // Decl: 84

    virtual void SetLogPrintFunc(PATHDebugPrintFunc f) {} // Decl: 120

    void AbortMessage(char *msg) {} // Decl: 122

    void PrintMessage(char *msg) {} // Decl: 127

    void LogMessage(char *msg, char *code) {} // Decl: 132

    virtual unsigned int GetMilliseconds(); // Decl: 160

    virtual int GetMinStreamBufferSize(int requests); // Decl: 176

    virtual char *LoadFile(const char *filepath, int &fileop, int filesize); // Decl: 201

    virtual int LoadFileDone(int fileop, char *&filedata); // Decl: 218

    virtual char *LoadFileSync(const char *filepath, int filesize); // Decl: 239

    virtual int FileExists(const char *filename); // Decl: 249

    virtual int FileSize(const char *filename); // Decl: 259

    virtual void SetSynchMode(SynchMode mode) {} // Decl: 265
    virtual SynchMode GetSynchMode() {}          // Decl: 266

  private:
    PATHAbortMsgFunc pathabortmsg; // offset 0x0, size 0x4, Decl: 269
    PATHDebugPrintFunc pathprintf; // offset 0x4, size 0x4, Decl: 270
    PATHDebugPrintFunc pathlogf;   // offset 0x8, size 0x4, Decl: 271
    SynchMode synchmode;           // offset 0xC, size 0x4, Decl: 272
};

}; // namespace Path

#endif
