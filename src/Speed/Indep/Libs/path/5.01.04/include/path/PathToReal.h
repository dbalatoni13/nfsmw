#ifndef __IPATHTOREALH__
#define __IPATHTOREALH__

#include "path/IPathToReal.h"

// total size: 0x14
class PathToReal : public Path::IPathToReal {
  public:
    PathToReal();
    ~PathToReal() override;
};

// total size: 0x14
class PathToReal5 : public PathToReal {
  public:
    virtual int GetMinStreamBufferSize(int requests);
    virtual unsigned int GetMilliseconds();
    virtual char *LoadFile(const char *filepath, int &fileop, int filesize);
    virtual char *LoadFileSync(const char *filepath, int filesize);
    virtual int FileExists(const char *filename);
    virtual int FileSize(const char *filename);
};

// total size: 0x14
class PathToReal5Async : public PathToReal5 {
  public:
    virtual char *LoadFile(const char *filepath, int &fileop, int filesize);
};

// total size: 0x14
class PathToReal6 : public PathToReal {
  public:
    PathToReal6();

    void *operator new(PATH_SIZE_T size);

    void operator delete(void *ptr);

    // Overrides: IPathToReal
    ~PathToReal6() override;
    int GetMinStreamBufferSize(int requests) override;
    unsigned int GetMilliseconds() override;
    char *LoadFile(const char *filepath, int &fileop, int filesize) override;
    int LoadFileDone(int fileop, char *&filedata) override;
    char *LoadFileSync(const char *filepath, int filesize) override;
    int FileExists(const char *filename) override;
    int FileSize(const char *filename) override;

  private:
    static void TaskService(void *, int);
};

// total size: 0x14
class PathToReal6Async : public PathToReal6 {
  public:
    PathToReal6Async();

    void *operator new(size_t size);

    void operator delete(void *ptr);

    // Overrides: IPathToReal
    ~PathToReal6Async() override;
    char *LoadFile(const char *filepath, int &fileop, int filesize) override;
    int LoadFileDone(int fileop, char *&filedata) override;

  private:
    static void AsyncCallback(int);
};

#endif
