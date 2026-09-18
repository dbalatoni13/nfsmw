#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

// Prototipos locales con el manglado C++ del original (ver la nota de linkage en
// include/common/realcore/file/filesys.h).
int FILESYS_completeop(int ophandle);
unsigned long long FILESYS_completeop64(int ophandle);
int FILESYS_waitop(int ophandle);
int FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata);
int FILESYS_close(int filehandle, int timeout, void *userdata);
int FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                 void *userdata);
int FILESYS_size(int filehandle, int priority, void *userdata);
void FILESYS_callbackop(int ophandle, void (*func)(int, int, void *));

// total size: 0xC
struct TagValuePair {
    unsigned int mTag; // offset 0x0, size 0x4
    union {
        int mInt;             // offset 0x0, size 0x4
        unsigned int mSize;   // offset 0x0, size 0x4
        float mFloat;         // offset 0x0, size 0x4
        const void *mPointer; // offset 0x0, size 0x4
    } mValue;                 // offset 0x4, size 0x4
    mutable const TagValuePair *mNext; // offset 0x8, size 0x4

    TagValuePair(unsigned int tag, int value) {
        mTag = tag;
        mValue.mInt = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, unsigned int value) {
        mTag = tag;
        mValue.mSize = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, float value) {
        mTag = tag;
        mValue.mFloat = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, const void *value) {
        mTag = tag;
        mValue.mPointer = value;
        mNext = 0;
    }

    const TagValuePair &operator+(const TagValuePair &rhs) const {
        rhs.mNext = this;
        return rhs;
    }
};

// total size: 0x4
struct IAllocator {
    virtual void *Alloc(unsigned int size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, unsigned int size) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;

  protected:
    virtual ~IAllocator() {}
};

struct FILESYSOPTS {
    int size;                                // +0x00
    IAllocator *allocator;                   // +0x04
    int MaxOpenFiles;                        // +0x08
    int MaxFileOps;                          // +0x0C
    int nSearchLocs;                         // +0x10
    int nSearchPathLength;                   // +0x14
    int MaxDevices;                          // +0x18
    int ThreadStackSize;                     // +0x1C
    int (*decompresssize)(const void *);     // +0x20
    int (*decompress)(const void *, void *); // +0x24
    unsigned int LargeReadSliceSize;         // +0x28
    unsigned int AllocAlignBoundary;         // +0x2C
    int DiscType;                            // +0x30
    int mErrorRetryCount;                    // +0x34
};

extern FILESYSOPTS gFileSysOpts;

// total size: 0x30
struct REQUESTSTRUCTtag {
    int id;                // offset 0x0, size 0x4
    REQUESTSTRUCTtag *nextreq; // offset 0x4, size 0x4
    int bytesread;         // offset 0x8, size 0x4
    int releaseflag;       // offset 0xC, size 0x4
    int cancelflag;        // offset 0x10, size 0x4
    char *loadaddr;        // offset 0x14, size 0x4
    void (*callback)(int); // offset 0x18, size 0x4
    volatile int fop;      // offset 0x1C, size 0x4
    int fhandle;           // offset 0x20, size 0x4
    int foffset;           // offset 0x24, size 0x4
    int readsize;          // offset 0x28, size 0x4
    char *destaddr;        // offset 0x2C, size 0x4
};

// total size: 0x8
struct REQUESTQUEUE {
    REQUESTSTRUCTtag *head; // offset 0x0, size 0x4
    REQUESTSTRUCTtag *tail; // offset 0x4, size 0x4
};

static REQUESTSTRUCTtag *request = 0;
static int numrequests;
static REQUESTQUEUE freequeue;
static int requestidcounter;
/* .bss del objeto extraido: 32 B alineados a 8 (el objeto mide 28). */
static RealSystem::Mutex mutex __attribute__((aligned(8)));

static void loadfileclosecallback(int, int, void *userdata);
static void loadfilereadcallback(int, int, void *userdata);
static void loadfilesizecallback(int, int, void *userdata);
static void loadfileopencallback(int, int, void *userdata);

static void queueadd(REQUESTQUEUE *queue, REQUESTSTRUCTtag *req) {
    mutex.Lock();

    if (queue->head == 0) {
        queue->head = req;
    } else {
        queue->tail->nextreq = req;
    }

    queue->tail = req;
    req->nextreq = 0;

    mutex.Unlock();
}

static REQUESTSTRUCTtag *queuefetch(REQUESTQUEUE *queue) {
    REQUESTSTRUCTtag *req;

    mutex.Lock();

    req = 0;

    if (queue->head != 0) {
        req = queue->head;

        queue->head = req->nextreq;
    }

    mutex.Unlock();

    return req;
}

static void newrequestid(REQUESTSTRUCTtag *req) {
    requestidcounter += 0x100;
    if (requestidcounter == 0) {
        requestidcounter = 0x100;
    }
    req->id = (req->id & 0xFF) | requestidcounter;
}

static REQUESTSTRUCTtag *locaterequest(int id) {
    REQUESTSTRUCTtag *req;
    int index;

    index = id & 0xFF;

    if (id <= 0xFF || index >= numrequests) {
        return 0;
    }

    req = &request[index];

    if (req->id != id) {
        return 0;
    }

    return req;
}

static void releaserequest(REQUESTSTRUCTtag *req) {
    if (req->cancelflag != 0 && (unsigned int)req->loadaddr > 1) {
        gFileSysOpts.allocator->Free(req->loadaddr, 0);
    }
    req->id = req->id & 0xFF;
    req->fop = 0;
    queueadd(&freequeue, req);
}

static void finishrequest(REQUESTSTRUCTtag *req) {
    void (*callback)(int);
    int cancelflag;
    int releaseflag;

    mutex.Lock();
    callback = req->callback;
    cancelflag = req->cancelflag;
    releaseflag = req->releaseflag;
    req->fop = 0;
    mutex.Unlock();

    if (cancelflag != 0) {
        if (releaseflag == 0) {
            releaserequest(req);
        }
    } else if (callback != 0) {
        callback(req->id);
    }
}

static void loadfileclosecallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = (REQUESTSTRUCTtag *)userdata;

    FILESYS_completeop(req->fop);

    finishrequest(req);
}

static void loadfilereadcallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = (REQUESTSTRUCTtag *)userdata;
    unsigned int bytesread;
    unsigned int readsize;

    bytesread = FILESYS_completeop64(req->fop);
    req->bytesread += bytesread;
    req->destaddr += bytesread;

    if (bytesread < gFileSysOpts.LargeReadSliceSize || req->cancelflag != 0) {
        req->fop = FILESYS_close(req->fhandle, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfileclosecallback);
        }
    } else {
        req->foffset += bytesread;
        req->readsize -= bytesread;

        readsize = req->readsize;
        if (readsize > gFileSysOpts.LargeReadSliceSize) {
            readsize = gFileSysOpts.LargeReadSliceSize;
        }

        req->fop = FILESYS_read(req->fhandle, req->foffset, req->destaddr, readsize, 99, req);

        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfilereadcallback);
        }
    }
}

static void loadfilesizecallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = (REQUESTSTRUCTtag *)userdata;
    int filesize;
    int memtype;
    unsigned int readsize;

    filesize = FILESYS_completeop64(req->fop);

    if (req->cancelflag != 0) {
        req->fop = FILESYS_close(req->fhandle, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfileclosecallback);
        }
    } else {
        memtype = req->readsize;
        req->readsize = filesize;

        req->loadaddr = (char *)gFileSysOpts.allocator->Alloc(
            filesize, TagValuePair((memtype & 0x100) ? 4 : 0, 1) +
                          TagValuePair((memtype & 0x400) ? 2 : 0, gFileSysOpts.AllocAlignBoundary) +
                          TagValuePair(1, (const void *)"ASYNCFILE BUF") +
                          // El __FILE__ y el __LINE__ del original, como en filesys.cpp.
                          TagValuePair(5, (const void *)"d:/packages/realcore/6.24.00/source/file/cmn/hlafile.cpp") + TagValuePair(6, 319));

        req->destaddr = req->loadaddr;

        readsize = req->readsize;
        if (readsize > gFileSysOpts.LargeReadSliceSize) {
            readsize = gFileSysOpts.LargeReadSliceSize;
        }

        req->fop = FILESYS_read(req->fhandle, req->foffset, req->loadaddr, readsize, 99, req);

        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfilereadcallback);
        }
    }
}

static void loadfileopencallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = (REQUESTSTRUCTtag *)userdata;
    unsigned int readsize;

    req->fhandle = FILESYS_completeop64(req->fop);
    if (req->fhandle == 0) {
        finishrequest(req);
        return;
    }

    if (req->cancelflag != 0) {
        req->fop = FILESYS_close(req->fhandle, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfileclosecallback);
        }
    } else if (req->loadaddr == 0) {
        readsize = req->readsize;
        if (readsize > gFileSysOpts.LargeReadSliceSize) {
            readsize = gFileSysOpts.LargeReadSliceSize;
        }

        req->fop = FILESYS_read(req->fhandle, req->foffset, req->destaddr, readsize, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfilereadcallback);
        }
    } else {
        req->fop = FILESYS_size(req->fhandle, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfilesizecallback);
        }
    }
}

void ASYNCFILE_init(int requests, int memtype) {
    REQUESTSTRUCTtag *req;
    int i;

    if (request != 0) {
        return;
    }

    if (requests > 256) {
        return;
    }

    numrequests = requests;

    request = (REQUESTSTRUCTtag *)gFileSysOpts.allocator->Alloc(
        requests * sizeof(REQUESTSTRUCTtag),
        TagValuePair((memtype & 0x100) ? 4 : 0, 1) +
            TagValuePair((memtype & 0x400) ? 2 : 0, gFileSysOpts.AllocAlignBoundary) +
            TagValuePair(1, (const void *)"ASYNCFILE") +
            TagValuePair(5, (const void *)"d:/packages/realcore/6.24.00/source/file/cmn/hlafile.cpp") + TagValuePair(6, 498));

    /* head PRIMERO: el planificador saca el `stw` de la cola delante y deja el
       de `request` en medio, que es el orden del objetivo (0x80384D80..8C).
       Con tail primero salen `freequeue` y `request` intercambiados (4 B). */
    freequeue.head = request;
    freequeue.tail = &request[requests - 1];

    mutex.Create();

    for (i = 0; i < requests; i++) {
        request[i].id = i;
        request[i].nextreq = &request[i + 1];
        request[i].fop = 0;
    }

    request[requests - 1].nextreq = 0;
}

int ASYNCFILE_load(const char *name, int memtype) {
    REQUESTSTRUCTtag *req;

    req = queuefetch(&freequeue);
    if (req == 0) {
        return 0;
    }

    newrequestid(req);

    req->readsize = memtype;
    req->bytesread = 0;
    req->releaseflag = 0;
    req->cancelflag = 0;
    req->callback = 0;
    req->foffset = 0;
    req->destaddr = 0;
    req->loadaddr = (char *)1;

    req->fop = FILESYS_open(name, 1, 100, req);
    if (req->fop == 0) {
        return 0;
    }

    FILESYS_callbackop(req->fop, loadfileopencallback);

    return req->id;
}

int ASYNCFILE_release(int handle, void **address, int *size) {
    REQUESTSTRUCTtag *req;
    int cancelflag;
    int releaseflag;

    cancelflag = 0;

    mutex.Lock();

    req = locaterequest(handle);
    if (req != 0) {
        cancelflag = req->cancelflag;
        if (cancelflag == 0) {
            req->releaseflag = 1;
        }
    }

    mutex.Unlock();

    if (req == 0 || cancelflag != 0) {
        return -1;
    }

    while (req->fop != 0) {
        FILESYS_waitop(req->fop);
    }

    if (req->cancelflag != 0) {
        if (address != 0) {
            *address = 0;
        }
        if (size != 0) {
            *size = 0;
        }
        releaserequest(req);

        return -1;
    }

    if (address != 0) {
        *address = req->destaddr - req->bytesread;
    }
    if (size != 0) {
        *size = req->bytesread;
    }
    releaserequest(req);

    return 1;
}
