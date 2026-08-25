#include "types.h"

#include "../../../include/common/realcore/file/filesys.h"
#include "../../../include/common/realcore/system.h"

struct REQUESTSTRUCTtag {
    int id;
    REQUESTSTRUCTtag *nextreq;
    int bytesread;
    int releaseflag;
    int cancelflag;
    char *loadaddr;
    void (*callback)(int);
    volatile int fop;
    int fhandle;
    int foffset;
    int readsize;
    char *destaddr;
};

struct REQUESTQUEUE {
    REQUESTSTRUCTtag *head;
    REQUESTSTRUCTtag *tail;
};

static RealSystem::Mutex mutex;
static REQUESTSTRUCTtag *request;
static int numrequests;
static REQUESTQUEUE freequeue;
static int requestidcounter;

static void queueadd(REQUESTQUEUE *queue, REQUESTSTRUCTtag *req) {
    mutex.Lock();
    if (queue->head == nullptr) {
        queue->head = req;
    } else {
        queue->tail->nextreq = req;
    }
    queue->tail = req;
    req->nextreq = nullptr;
    mutex.Unlock();
}

static REQUESTSTRUCTtag *queuefetch(REQUESTQUEUE *queue) {
    mutex.Lock();
    REQUESTSTRUCTtag *req = nullptr;
    if (queue->head != nullptr) {
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
    req->id = static_cast<unsigned char>(req->id) | requestidcounter;
}

static REQUESTSTRUCTtag *locaterequest(int id) {
    REQUESTSTRUCTtag *req;
    int index = id & 0xff;
    if (id <= 0xff || index >= numrequests) {
        return nullptr;
    }
    req = request + index;
    if (req->id != id) {
        return nullptr;
    }
    return req;
}

static void releaserequest(REQUESTSTRUCTtag *req) {
    if (req->cancelflag != 0 && reinterpret_cast<uintptr_t>(req->loadaddr) > 1) {
        gFileSysOpts.allocator->Free(req->loadaddr, 0);
    }
    req->id = static_cast<unsigned char>(req->id);
    req->fop = 0;
    queueadd(&freequeue, req);
}

static void finishrequest(REQUESTSTRUCTtag *req) {
    mutex.Lock();
    void (*callback)(int) = req->callback;
    int cancelflag = req->cancelflag;
    int releaseflag = req->releaseflag;
    req->fop = 0;
    mutex.Unlock();
    if (cancelflag != 0) {
        if (releaseflag == 0) {
            releaserequest(req);
        }
    } else if (callback != nullptr) {
        callback(req->id);
    }
}

static void loadfileclosecallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = static_cast<REQUESTSTRUCTtag *>(userdata);
    FILESYS_completeop(req->fop);
    finishrequest(req);
}

static void loadfilereadcallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = static_cast<REQUESTSTRUCTtag *>(userdata);
    unsigned int bytesread = static_cast<unsigned int>(FILESYS_completeop64(req->fop));
    unsigned int readsize;
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
    REQUESTSTRUCTtag *req = static_cast<REQUESTSTRUCTtag *>(userdata);
    int filesize;
    int memtype;
    unsigned int readsize;
    filesize = static_cast<int>(FILESYS_completeop64(req->fop));
    if (req->cancelflag != 0) {
        req->fop = FILESYS_close(req->fhandle, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfileclosecallback);
        }
    } else {
        memtype = req->readsize;
        req->readsize = filesize;
        req->loadaddr = static_cast<char *>(gFileSysOpts.allocator->Alloc(
            filesize,
            EA::TagValuePair((memtype & 0x100) != 0 ? EA::Allocator::ATT_ALLOC_HIGH
                                                    : EA::Allocator::ATT_NULL,
                             1) +
                EA::TagValuePair((memtype & 0x400) != 0 ? EA::Allocator::ATT_ALIGNMENT
                                                        : EA::Allocator::ATT_NULL,
                                 gFileSysOpts.AllocAlignBoundary) +
                EA::TagValuePair(EA::Allocator::ATT_NAME, "ASYNCFILE BUF") +
                EA::TagValuePair(EA::Allocator::ATT_FILE,
                                 "d:/packages/realcore/6.24.00/source/file/cmn/hlafile.cpp") +
                EA::TagValuePair(EA::Allocator::ATT_LINE, 319)));
        req->destaddr = req->loadaddr;
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

static void loadfileopencallback(int, int, void *userdata) {
    REQUESTSTRUCTtag *req = static_cast<REQUESTSTRUCTtag *>(userdata);
    unsigned int readsize;
    req->fhandle = static_cast<int>(FILESYS_completeop64(req->fop));
    if (req->fhandle == 0) {
        finishrequest(req);
    } else if (req->cancelflag != 0) {
        req->fop = FILESYS_close(req->fhandle, 99, req);
        if (req->fop != 0) {
            FILESYS_callbackop(req->fop, loadfileclosecallback);
        }
    } else if (req->loadaddr == nullptr) {
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
    if (request != nullptr || requests > 256) {
        return;
    }
    numrequests = requests;
    request = static_cast<REQUESTSTRUCTtag *>(gFileSysOpts.allocator->Alloc(
        requests * sizeof(REQUESTSTRUCTtag),
        EA::TagValuePair((memtype & 0x100) != 0 ? EA::Allocator::ATT_ALLOC_HIGH
                                                : EA::Allocator::ATT_NULL,
                         1) +
            EA::TagValuePair((memtype & 0x400) != 0 ? EA::Allocator::ATT_ALIGNMENT
                                                    : EA::Allocator::ATT_NULL,
                             gFileSysOpts.AllocAlignBoundary) +
            EA::TagValuePair(EA::Allocator::ATT_NAME, "ASYNCFILE") +
            EA::TagValuePair(EA::Allocator::ATT_FILE,
                             "d:/packages/realcore/6.24.00/source/file/cmn/hlafile.cpp") +
            EA::TagValuePair(EA::Allocator::ATT_LINE, 498)));
    freequeue.tail = request + requests - 1;
    freequeue.head = request;
    mutex.Create();
    for (i = 0; i < requests; i++) {
        request[i].id = i;
        request[i].nextreq = &request[i + 1];
        request[i].fop = 0;
    }
    request[requests - 1].nextreq = nullptr;
}

int ASYNCFILE_load(const char *name, int memtype) {
    REQUESTSTRUCTtag *req = queuefetch(&freequeue);
    if (req == nullptr) {
        return 0;
    }
    newrequestid(req);
    req->readsize = memtype;
    req->bytesread = 0;
    req->releaseflag = 0;
    req->cancelflag = 0;
    req->callback = nullptr;
    req->foffset = 0;
    req->destaddr = nullptr;
    req->loadaddr = reinterpret_cast<char *>(1);
    req->fop = FILESYS_open(name, 1, 100, req);
    if (req->fop == 0) {
        return 0;
    }
    FILESYS_callbackop(req->fop, loadfileopencallback);
    return req->id;
}

int ASYNCFILE_release(int handle, void **address, int *size) {
    REQUESTSTRUCTtag *req;
    int cancelflag = 0;
    int releaseflag;
    mutex.Lock();
    req = locaterequest(handle);
    if (req != nullptr && (cancelflag = req->cancelflag) == 0) {
        req->releaseflag = 1;
    }
    mutex.Unlock();
    if (req == nullptr || cancelflag != 0) {
        return -1;
    }
    while (req->fop != 0) {
        FILESYS_waitop(req->fop);
    }
    if (req->cancelflag != 0) {
        if (address != nullptr) {
            *address = nullptr;
        }
        if (size != nullptr) {
            *size = 0;
        }
        releaserequest(req);
        return -1;
    }
    if (address != nullptr) {
        *address = req->destaddr - req->bytesread;
    }
    if (size != nullptr) {
        *size = req->bytesread;
    }
    releaserequest(req);
    return 1;
}
