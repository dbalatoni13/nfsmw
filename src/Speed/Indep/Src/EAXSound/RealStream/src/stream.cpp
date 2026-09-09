
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "realcore/file/filesys_cc.h"
#include "realcore/system.h"
#include "realcore/std/memory.h"
#include "endian/big/endian.h"

#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamManager.h"
#include "realcore/system/systask.h"
#include "realcore/system/threads.h"

#include <cstring>

// TODO we don't know where these are declared, where do they go?

// TODO fix the 0x20 magic in this file

typedef intptr_t STREAMHANDLE;
typedef int STREAMREQUESTID;

// total size: 0x8
typedef struct {
    int type; // offset 0x0, size 0x4
    int size; // offset 0x4, size 0x4
} STREAMCHUNKHDR;

typedef enum STREAMSTATE {
    STREAM_IDLE = 0,
    STREAM_RUNNING = 1,
    STREAM_STOPPED = 2,
} STREAMSTATE;

typedef enum STREAMREQUESTSTATE {
    STREAMREQUEST_FREE = 0,
    STREAMREQUEST_PENDING = 1,
    STREAMREQUEST_EXECUTING = 2,
    STREAMREQUEST_COMPLETED = 3,
    STREAMREQUEST_CANCELED = 4,
} STREAMREQUESTSTATE;

typedef enum STREAM_TAP {
    STREAM_NULLTAP = -1,
    STREAM_ABORTTAP = -2,
} STREAM_TAP;

typedef enum STREAM_PRIORITY {
    STREAM_PRIORITYLOW = 150,
    STREAM_PRIORITYHIGH = 50,
} STREAM_PRIORITY;

typedef enum READTYPE {
    FILEREAD = 0,
    MEMREAD = 1,
} READTYPE;

struct STREAMHEADERtag;

// total size: 0x10
typedef struct TAPSTRUCTtag {
    STREAMHEADERtag *stream; // offset 0x0, size 0x4
    int tapnum;              // offset 0x4, size 0x4
    int gettable;            // offset 0x8, size 0x4
    char *getptr;            // offset 0xC, size 0x4
} TAPSTRUCT;

// total size: 0x124
typedef struct REQUESTSTRUCTtag {
    STREAMREQUESTID id;       // offset 0x0, size 0x4
    STREAMREQUESTSTATE state; // offset 0x4, size 0x4
    REQUESTSTRUCTtag *prev;   // offset 0x8, size 0x4
    REQUESTSTRUCTtag *next;   // offset 0xC, size 0x4
    READTYPE type;            // offset 0x10, size 0x4
    char fname[255];          // offset 0x14, size 0xFF
    char *address;            // offset 0x114, size 0x4
    int parm;                 // offset 0x118, size 0x4
    int endchunkid;           // offset 0x11C, size 0x4
    char *datastart;          // offset 0x120, size 0x4
} REQUESTSTRUCT;

// total size: 0x8
typedef struct {
    REQUESTSTRUCT *head; // offset 0x0, size 0x4
    REQUESTSTRUCT *tail; // offset 0x4, size 0x4
} REQUESTQUEUE;

// total size: 0xC
typedef struct FILTERSTRUCTtag {
    int mask;   // offset 0x0, size 0x4
    int value;  // offset 0x4, size 0x4
    int tapnum; // offset 0x8, size 0x4
} FILTERSTRUCT;

// total size: 0x18C
typedef struct STREAMHEADERtag {
    int id;                  // offset 0x0, size 0x4
    MUTEX mutex;             // offset 0x4, size 0x1C
    REQUESTSTRUCT *request;  // offset 0x20, size 0x4
    int requests;            // offset 0x24, size 0x4
    FILTERSTRUCT *filter;    // offset 0x28, size 0x4
    int filters;             // offset 0x2C, size 0x4
    TAPSTRUCT *tap;          // offset 0x30, size 0x4
    int taps;                // offset 0x34, size 0x4
    char *actualbufferstart; // offset 0x38, size 0x4
    char *bufferstart;       // offset 0x3C, size 0x4
    char *bufferend;         // offset 0x40, size 0x4
    STREAMSTATE state;       // offset 0x44, size 0x4
    int prioritylow;         // offset 0x48, size 0x4
    int priorityhigh;        // offset 0x4C, size 0x4
    int greedylevel;         // offset 0x50, size 0x4
    int greedystate;         // offset 0x54, size 0x4
    int bufferusage;         // offset 0x58, size 0x4
    char *datastart;         // offset 0x5C, size 0x4
    char *datatail;          // offset 0x60, size 0x4
    char *dataend;           // offset 0x64, size 0x4
    REQUESTSTRUCT *firstreq; // offset 0x68, size 0x4
    REQUESTSTRUCT *curreq;   // offset 0x6C, size 0x4
    REQUESTSTRUCT *lastreq;  // offset 0x70, size 0x4
    REQUESTSTRUCT *freereq;  // offset 0x74, size 0x4
    char fname[255];         // offset 0x78, size 0xFF
    intptr_t fhandle;        // offset 0x178, size 0x4 TODO 64 bit, does this being intptr_t mess with it?
    int foffset;             // offset 0x17C, size 0x4
    FILEOP fop;              // offset 0x180, size 0x4
    int readsize;            // offset 0x184, size 0x4
    int readblocksize;       // offset 0x188, size 0x4
} STREAMHEADER;

static int validatehandle(STREAMHANDLE handle, STREAMHEADER **strmptr, TAPSTRUCT **tapptr) {
    if (handle == 0) {
        return 1;
    }

    STREAMHEADER *strm = *reinterpret_cast<STREAMHEADER **>(handle);
    if (strm->id != 'MRTS') {
        return 1;
    }
    TAPSTRUCT *tap = reinterpret_cast<TAPSTRUCT *>(handle);
    *tapptr = tap;
    *strmptr = strm;
    return 0;
}

static int inbetween(char *startptr, char *endptr, char *ptr) {
    if (startptr <= endptr) {
        if (ptr < startptr) {
            return 0;
        } else {
            return static_cast<int>(ptr < endptr);
        }
    } else {
        return static_cast<int>(ptr >= startptr || ptr < endptr);
    }
}

static void decbufferusage(STREAMHEADER *strm, int amount) {
    int lockstate;

    MUTEX_lock(&strm->mutex);
    int oldbufferusage = strm->bufferusage;
    int bufferusage = oldbufferusage - amount;
    strm->bufferusage = bufferusage;
    MUTEX_unlock(&strm->mutex);

    int greedylevel = strm->greedylevel;
    if (oldbufferusage >= greedylevel && bufferusage < greedylevel) {
        strm->greedystate = 1;
        if (strm->state == STREAM_RUNNING) {
            FILESYS_priorityop(strm->fop, strm->priorityhigh);
        }
    }
}

static int numrequests;        // size: 0x4, address: 0x804FFD44
static REQUESTSTRUCT *request; // size: 0x4, address: 0x804FF61C
static REQUESTQUEUE freequeue; // size: 0x8, address: 0x804FFD48
static int requestidcounter;   // size: 0x4, address: 0x804FFD50

static REQUESTSTRUCT *getfreerequest(STREAMHEADER *strm) {
    REQUESTSTRUCT *req = nullptr;
    int lockstate;

    MUTEX_lock(&strm->mutex);
    if (strm->freereq != nullptr) {
        req = strm->freereq;
        strm->freereq = req->next;
        requestidcounter += 256;
        if (requestidcounter == 0) {
            requestidcounter = 256;
        }
        req->id = (req->id & 0xFF) | requestidcounter;
    }
    MUTEX_unlock(&strm->mutex);

    return req;
}

static void queuerequest(STREAMHEADER *strm, REQUESTSTRUCT *req) {
    int lockstate;

    req->state = STREAMREQUEST_PENDING;
    req->next = nullptr;
    MUTEX_lock(&strm->mutex);
    if (strm->lastreq == nullptr) {
        req->prev = nullptr;
        strm->firstreq = req;
        strm->curreq = req;
        strm->lastreq = req;
    } else {
        req->prev = strm->lastreq;
        strm->lastreq->next = req;
        strm->lastreq = req;
    }
    MUTEX_unlock(&strm->mutex);
}

static REQUESTSTRUCT *locaterequest(STREAMHEADER *strm, int requestid) {
    REQUESTSTRUCT *req;
    int index = requestid & 0xFF;
    if (index >= strm->requests) {
        return nullptr;
    }

    req = strm->request + index;
    if (requestid != strm->request[index].id) {
        return nullptr;
    }
    if (req->state == STREAMREQUEST_FREE) {
        return nullptr;
    }
    return req;
}

static void freerequest(STREAMHEADER *strm, REQUESTSTRUCT *req) {
    if (req == strm->firstreq) {
        strm->firstreq = req->next;
    } else {
        req->prev->next = req->next;
    }

    if (req == strm->lastreq) {
        strm->lastreq = req->prev;
    } else {
        req->next->prev = req->prev;
    }

    if (req == strm->curreq) {
        if (req->next == nullptr) {
            strm->curreq = req->prev;
        } else {
            strm->curreq = req->next;
        }
    }

    req->state = STREAMREQUEST_FREE;
    req->next = strm->freereq;
    strm->freereq = req;
}

static int filterchunk(STREAMHEADER *strm, STREAMCHUNKHDR *chunk) {
    FILTERSTRUCT *filt;
    int chunktype = EA::Endian::little_get(&chunk->type, 4);
    int i;

    for (i = 0; i < strm->filters; i++) {
        filt = strm->filter + i;
        if ((chunktype & filt->mask) == filt->value) {
            return filt->tapnum;
        }
    }
    return -2;
}

static int parsechunks(STREAMHEADER *strm) {
    REQUESTSTRUCT *req = strm->curreq;
    TAPSTRUCT *tap;
    STREAMCHUNKHDR *chunk;
    int lockstate;
    int chunksize;
    int tapnum;
    int canceled;
    int oldbufferusage;
    int bufferusage;
    int greedylevel;

    while (strm->dataend - strm->datatail > 7) {
        chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);

        chunksize = static_cast<int>(EA::Endian::little_get(&chunk->size, 4));

        if ((static_cast<unsigned int>(chunksize) & 0xFF000000) != 0) {
            chunksize = 8;
            EA::Endian::little_put(&chunk->type, req->endchunkid, 4);
            EA::Endian::little_put(&chunk->size, chunksize, 4);
        }

        if (strm->datatail + chunksize > strm->dataend) {
            break;
        }

        if (EA::Endian::little_get(&chunk->type, 4) == static_cast<unsigned int>(req->endchunkid)) {
            int tail_floor = reinterpret_cast<unsigned int>(strm->datatail) & 0x1F;
            chunksize = ((chunksize + tail_floor + 0x1F) & ~0x1F) - tail_floor;
        }

        tapnum = filterchunk(strm, chunk);

        if (tapnum < 0) {
            MUTEX_lock(&strm->mutex);

            canceled = static_cast<int>(req->state == STREAMREQUEST_CANCELED);

            if (!canceled) {
                EA::Endian::little_put(&chunk->type, -2, 4);
                strm->datatail += chunksize;
            }

            MUTEX_unlock(&strm->mutex);
        } else {
            lockstate = 0;

            EA::Endian::little_put(&chunk->size, static_cast<unsigned int>(chunksize) | (static_cast<unsigned int>(tapnum) << 24), 4);

            MUTEX_lock(&strm->mutex);

            canceled = static_cast<int>(req->state == STREAMREQUEST_CANCELED);

            if (!canceled) {
                tap = &strm->tap[tapnum - 1];

                tap->gettable += chunksize;

                if (tap->gettable == chunksize) {
                    tap->getptr = reinterpret_cast<char *>(chunk);
                }

                oldbufferusage = strm->bufferusage;
                bufferusage = oldbufferusage + chunksize;
                greedylevel = strm->greedylevel;

                strm->bufferusage = bufferusage;
                strm->datatail += chunksize;

                if (oldbufferusage < greedylevel && bufferusage >= greedylevel) {
                    strm->greedystate = lockstate;
                }
            }

            MUTEX_unlock(&strm->mutex);
        }

        if (canceled) {
            if (EA::Endian::little_get(&chunk->type, 4) == static_cast<unsigned int>(req->endchunkid)) {
                break;
            } else {
                int tail_floor = reinterpret_cast<unsigned int>(strm->datatail) & 0x1F;
                chunksize = ((chunksize + tail_floor + 0x1F) & ~0x1F) - tail_floor;

                EA::Endian::little_put(&chunk->size, static_cast<unsigned int>(chunksize) | (static_cast<unsigned int>(tapnum) << 24), 4);
                break;
            }
        }

        if (EA::Endian::little_get(&chunk->type, 4) == static_cast<unsigned int>(req->endchunkid)) {
            return 1;
        }
    }

    return 0;
}

static void startnextrequest(STREAMHEADER *strm, int priority);
static void restartstream(STREAMHEADER *strm, int priority);

static void opencallback(int, int, void *userdata) {
    STREAMHEADER *strm = static_cast<STREAMHEADER *>(userdata);

    strm->fhandle = static_cast<intptr_t>(FILESYS_completeop64(strm->fop));
    if (strm->fhandle == 0) {
        strm->state = STREAM_IDLE;
        freerequest(strm, strm->firstreq);
        if (strm->greedystate != 0) {
            startnextrequest(strm, strm->priorityhigh);
        } else {
            startnextrequest(strm, strm->prioritylow);
        }
    } else {
        restartstream(strm, strm->priorityhigh);
    }
}

static void closecallback(int, int, void *userdata) {
    STREAMHEADER *strm = static_cast<STREAMHEADER *>(userdata);

    FILESYS_completeop(strm->fop);
    strm->fop = FILESYS_open(strm->fname, 1, strm->priorityhigh, strm);
    if (strm->fop != 0) {
        FILESYS_callbackop(strm->fop, opencallback);
    }
}

extern bool bReadCallbackToggle;

static void readcallback(int, int, void *userdata) {
    bReadCallbackToggle = true;
    STREAMHEADER *strm = static_cast<STREAMHEADER *>(userdata);
    REQUESTSTRUCT *req = strm->curreq;
    int lockstate;
    int bytesread;
    int endoffile;
    int endchunk;

    if (req->type == 1) {
        bytesread = strm->readsize;
        endoffile = static_cast<int>(strm->foffset + bytesread >= req->parm);
    } else {
        bytesread = static_cast<int>(FILESYS_completeop64(strm->fop));
        endoffile = static_cast<int>(bytesread < strm->readsize);
    }

    strm->foffset += bytesread;
    strm->dataend += bytesread;
    endchunk = parsechunks(strm);
    if (req->state == STREAMREQUEST_CANCELED || endoffile || endchunk != 0) {
        if (req->state != STREAMREQUEST_CANCELED) {
            MUTEX_lock(&strm->mutex);
            if (req->state != STREAMREQUEST_CANCELED) {
                req->state = STREAMREQUEST_COMPLETED;
            }
            MUTEX_unlock(&strm->mutex);
        }
        startnextrequest(strm, strm->priorityhigh);
    } else {
        restartstream(strm, strm->priorityhigh - 1);
    }
}

static void startnextrequest(STREAMHEADER *strm, int priority) {
    REQUESTSTRUCT *req = nullptr;
    int lockstate;
    int nopendingrequest;

    MUTEX_lock(&strm->mutex);

    if (strm->curreq == nullptr) {
        nopendingrequest = 1;
    } else if (strm->curreq->state == STREAMREQUEST_PENDING) {
        nopendingrequest = 0;
    } else if (strm->curreq->next == nullptr) {
        nopendingrequest = 1;
    } else {
        strm->curreq = strm->curreq->next;
        nopendingrequest = 0;
    }

    if (nopendingrequest) {
        strm->state = STREAM_IDLE;
    } else {
        req = strm->curreq;
        req->datastart = strm->datatail;
        req->state = STREAMREQUEST_EXECUTING;
    }

    MUTEX_unlock(&strm->mutex);

    if (!nopendingrequest) {
        strm->dataend = strm->datatail;

        if (req->type == 1) {
            strm->foffset = 0;
        } else {
            strm->foffset = req->parm;

            if (strcmp(req->fname, strm->fname) != 0) {
                strcpy(strm->fname, req->fname);

                if (strm->fhandle == 0) {
                    strm->fop = FILESYS_open(strm->fname, 1, priority, strm);
                    if (strm->fop == 0) {
                        return;
                    }

                    FILESYS_callbackop(strm->fop, opencallback);
                    return;
                }

                strm->fop = FILESYS_close(strm->fhandle, priority, strm);
                if (strm->fop == 0) {
                    return;
                }

                FILESYS_callbackop(strm->fop, closecallback);
                return;
            }
        }

        restartstream(strm, priority);
    }
}

unsigned int utickreadcallback = 0;
unsigned int uTicksSinceLastAudioReadBailed = 0;

// UNSOLVED, around little_put inline
static void restartstream(STREAMHEADER *strm, int priority) {
    REQUESTSTRUCT *req;
    STREAMCHUNKHDR *chunk;
    char *reqend;
    int lockstate;
    int largestread;
    int tailsize;

    while (strm->datastart != strm->datatail) {
        chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datastart);

        if (EA::Endian::little_get(&chunk->type, 4) == -1) {
            strm->datastart = strm->bufferstart;
        } else {
            if (EA::Endian::little_get(&chunk->type, 4) != -2) {
                break;
            }

            strm->datastart = reinterpret_cast<char *>(chunk) + EA::Endian::little_get(&chunk->size, 4);
        }
    }

    MUTEX_lock(&strm->mutex);

    while (true) {
        req = strm->firstreq;

        if (req->next == nullptr || req->next->state == STREAMREQUEST_PENDING ||
            inbetween(strm->datastart, strm->dataend, req->next->datastart - 1)) {
            break;
        }

        freerequest(strm, req);
    }

    MUTEX_unlock(&strm->mutex);

    if (strm->datastart > strm->dataend) {
        largestread = strm->datastart - strm->dataend - 0x21;
    } else {
        largestread = strm->bufferend - strm->dataend - 0x20;

        if (largestread < strm->readblocksize) {
            tailsize = strm->dataend - strm->datatail;

            if (strm->curreq->type == 1) {
                if (strm->datastart - strm->bufferstart < tailsize + 1) {
                    strm->state = STREAM_STOPPED;
                    return;
                }
            } else if (strm->datastart - strm->bufferstart - 0x20 < tailsize + 1) {
                strm->state = STREAM_STOPPED;
                return;
            }

            if ((tailsize & 0x1F) == 0 || strm->curreq->type == 1) {
                strm->bufferstart = strm->actualbufferstart;
            } else {
                strm->bufferstart = strm->actualbufferstart - (tailsize % 0x20 - 0x20);
            }

            MEM_copy(strm->bufferstart, strm->datatail, tailsize);

            chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);
            EA::Endian::little_put(&chunk->type, -1, 4);
            EA::Endian::little_put(&chunk->size, 8, 4);

            reqend = strm->bufferstart + tailsize;
            strm->datatail = strm->bufferstart;
            strm->dataend = reqend;

            chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datastart);

            if (EA::Endian::little_get(&chunk->type, 4) == -1) {
                strm->datastart = strm->bufferstart;
                largestread = strm->bufferend - reqend - 0x20;
            } else {
                largestread = reinterpret_cast<char *>(chunk) - reqend - 1;
            }
        }
    }

    if (strm->greedystate == 0 && priority > strm->priorityhigh && IsWorldDataStreaming(reinterpret_cast<unsigned int>(strm->tap))) {
        float ftimesincelast = bGetTickerDifference(utickreadcallback);

        gbWorldDataBlocksAudioRead = true;
        strm->state = STREAM_STOPPED;
        uTicksSinceLastAudioReadBailed = bGetTicker();
        return;
    }

    if (IsWorldDataStreaming(static_cast<unsigned int>(strm->fhandle))) {
        gbAudioInterruptsWorldDataRead = true;
    } else {
        gbAudioInterruptsWorldDataRead = false;
    }

    gbWorldDataBlocksAudioRead = false;

    if (largestread < strm->readblocksize) {
        strm->state = STREAM_STOPPED;
        return;
    }

    req = strm->curreq;

    if (req->type == 1) {
        if (strm->foffset + largestread > req->parm) {
            strm->readsize = req->parm - strm->foffset;
        } else {
            strm->readsize = largestread;
        }

        MEM_copy(strm->dataend, req->address, strm->readsize);
        req->address += strm->readsize;
        readcallback(0, 0, strm);
        return;
    }

    if (largestread > strm->readblocksize) {
        int nBlocks = largestread / strm->readblocksize;
        strm->readsize = nBlocks * strm->readblocksize;
    } else {
        strm->readsize = strm->readblocksize;
    }

    bReadCallbackToggle = false;
    strm->fop = FILESYS_read(strm->fhandle, strm->foffset, strm->dataend, strm->readsize, priority, strm);

    if (!strm->fop) {
        return;
    }

    FILESYS_callbackop(strm->fop, readcallback);
}

int STREAM_overhead(int requests, int filters, int taps) {
    int filterBytes = filters * sizeof(FILTERSTRUCT);
    int tapBytes = taps << 4;
    int requestBytes = requests * sizeof(REQUESTSTRUCT);
    filterBytes += sizeof(STREAMHEADER); // TODO 64 bit
    requestBytes += filterBytes;
    requestBytes += tapBytes;
    requestBytes += 0x20;
    return requestBytes;
}

int STREAM_create(int requests, int filters, int taps, void *buffer, int size) {
    STREAMHEADER *strm;
    REQUESTSTRUCT *req;
    FILTERSTRUCT *filt;
    TAPSTRUCT *tap;
    int i;
    int overhead;

    overhead = STREAM_overhead(requests, filters, taps);

    if (size - overhead < 0x1800) {
        return 0;
    }
    if (requests < 2) {
        return 0;
    }
    if (requests > 0x100) {
        return 0;
    }
    if (static_cast<unsigned int>(filters - 1) > 15) {
        return 0;
    }
    if (taps < 1 || taps > filters) {
        return 0;
    }

    strm = static_cast<STREAMHEADER *>(buffer);
    strm->id = 'MRTS';
    MUTEX_create(&strm->mutex);

    strm->request = reinterpret_cast<REQUESTSTRUCT *>(static_cast<char *>(buffer) + sizeof(STREAMHEADER));
    strm->requests = requests;
    strm->filter = reinterpret_cast<FILTERSTRUCT *>(strm->request + requests);
    strm->filters = filters;
    strm->tap = reinterpret_cast<TAPSTRUCT *>(strm->filter + filters);
    strm->taps = taps;
    // TODO 64 bit
    strm->actualbufferstart = reinterpret_cast<char *>((reinterpret_cast<unsigned int>(strm->tap + taps) & 0xFFFFFFE0U) + 0x20);
    strm->bufferstart = strm->actualbufferstart;
    strm->bufferend = static_cast<char *>(buffer) + size;
    strm->state = STREAM_IDLE;
    strm->prioritylow = 150;
    strm->priorityhigh = 50;
    strm->greedylevel = 0;
    strm->greedystate = 0;
    strm->bufferusage = 0;
    strm->datastart = strm->actualbufferstart;
    strm->datatail = strm->actualbufferstart;
    strm->dataend = strm->actualbufferstart;
    strm->firstreq = nullptr;
    strm->curreq = nullptr;
    strm->lastreq = nullptr;
    strm->freereq = strm->request;

    MEM_clear(strm->fname, 0xFF);
    strm->fhandle = 0;

    if (size - overhead < 0x4000) {
        strm->readblocksize = 0x800;
    } else if (size - overhead < 0x8000) {
        strm->readblocksize = 0x1000;
    } else if (size - overhead < 0x10000) {
        strm->readblocksize = 0x2000;
    } else if (size - overhead < 0x18000) {
        strm->readblocksize = 0x4000;
    } else {
        if (size - overhead < 0x20000) {
            strm->readblocksize = 0x8000;
        } else if (size - overhead < 0x40000) {
            strm->readblocksize = 0x8000;
        } else {
            strm->readblocksize = 0x8000;
        }
    }

    for (i = 0; i < requests; i++) {
        req = strm->request + i;
        req->id = i;
        req->state = STREAMREQUEST_FREE;
        req->next = &strm->request[i + 1];
    }

    strm->request[requests - 1].next = nullptr;

    for (i = 0; i < filters; i++) {
        filt = strm->filter + i;
        filt->mask = 0;
        filt->value = 0;
        filt->tapnum = 1;
    }

    for (i = 0; i < taps; i++) {
        tap = strm->tap + i;
        tap->stream = strm;
        tap->tapnum = i + 1;
        tap->gettable = 0;
    }

    AssignAudioStreamHandle(reinterpret_cast<unsigned int>(strm->tap));
    return reinterpret_cast<int>(strm->tap);
}

void STREAM_setfilter(STREAMHANDLE handle, int filternum, int mask, int value, int tapnum) {
    STREAMHEADER *strm;
    FILTERSTRUCT *filt;
    TAPSTRUCT *tap;
    if (validatehandle(handle, &strm, &tap) == 0 && filternum > 0) {
        if (filternum <= strm->filters && (filternum != strm->filters || (mask | value) == 0) && (tapnum > 0 || tapnum == -1 || tapnum == -2) &&
            tapnum <= strm->taps && strm->state == STREAM_IDLE) {
            filt = strm->filter + (filternum - 1);
            filt->mask = mask;
            filt->value = value;
            filt->tapnum = tapnum;
        }
    }
}

void STREAM_kill(STREAMHANDLE handle);

void STREAM_destroy(STREAMHANDLE sndstreamhandle) {
    STREAMHEADER *strm;
    TAPSTRUCT *tap;
    int status = validatehandle(sndstreamhandle, &strm, &tap);
    if (status != 0) {
        return;
    }

    STREAM_kill(sndstreamhandle);
    while (strm->state == STREAM_RUNNING) {
        SYNCTASK_run();
        THREAD_yield(0);
    }

    strm->id = 0;
    MUTEX_destroy(&strm->mutex);
    if (strm->fhandle != 0) {
        FILESYS_closesync(strm->fhandle, 100);
    }
}

void STREAM_setpriority(STREAMHANDLE sndstreamhandle, int prioritylow, int priorityhigh) {
    STREAMHEADER *streamRaw;
    TAPSTRUCT *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        streamRaw->prioritylow = prioritylow;
        streamRaw->priorityhigh = priorityhigh;
    }
}

void STREAM_setgreedystate(STREAMHANDLE handle, int greedystate);

void STREAM_setgreedylevel(STREAMHANDLE handle, int greedylevel) {
    STREAMHEADER *strm;
    TAPSTRUCT *tap;
    int oldgreedylevel;
    int bufferusage;
    int oldside;
    int newside;
    if (validatehandle(handle, &strm, &tap) == 0) {
        oldgreedylevel = strm->greedylevel;
        strm->greedylevel = greedylevel;
        bufferusage = strm->bufferusage;
        oldside = static_cast<int>(bufferusage < oldgreedylevel);
        newside = static_cast<int>(bufferusage < greedylevel);
        if (oldside != newside) {
            STREAM_setgreedystate(handle, newside);
        }
    }
}

void STREAM_setgreedystate(STREAMHANDLE sndstreamhandle, int greedystate) {
    STREAMHEADER *strm;
    TAPSTRUCT *tap;
    if (validatehandle(sndstreamhandle, &strm, &tap) == 0) {
        strm->greedystate = greedystate;
        if (greedystate != 0 && strm->state == STREAM_RUNNING) {
            FILESYS_priorityop(strm->fop, strm->priorityhigh);
        }
    }
}

intptr_t STREAM_taphandle(STREAMHANDLE handle, int tapnum) {
    STREAMHEADER *strm;
    TAPSTRUCT *tap;
    if (validatehandle(handle, &strm, &tap) != 0 || tapnum <= 0) {
        return 0;
    }
    if (tapnum <= strm->taps) {
        return reinterpret_cast<intptr_t>(strm->tap + (tapnum - 1));
    }
    return 0;
}

STREAMREQUESTID STREAM_queuefile(STREAMHANDLE handle, const char *fname, int offset, int endchunkid) {
    STREAMHEADER *strm;
    REQUESTSTRUCT *req;
    TAPSTRUCT *tap;
    STREAMSTATE streamstate;
    int lockstate;
    if (validatehandle(handle, &strm, &tap) != 0) {
        return 0;
    }

    req = getfreerequest(strm);
    if (req == nullptr) {
        return 0;
    }

    req->type = FILEREAD;
    strncpy(req->fname, fname, sizeof(req->fname) - 1);
    req->parm = offset;
    req->endchunkid = endchunkid;
    queuerequest(strm, req);

    MUTEX_lock(&strm->mutex);
    streamstate = strm->state;
    if (streamstate == STREAM_IDLE) {
        strm->state = STREAM_RUNNING;
    }
    MUTEX_unlock(&strm->mutex);

    if (streamstate == STREAM_IDLE) {
        if (strm->greedystate != 0) {
            startnextrequest(strm, strm->priorityhigh);
        } else {
            startnextrequest(strm, strm->prioritylow);
        }
    }

    return req->id;
}

STREAMREQUESTID STREAM_queuemem(STREAMHANDLE handle, void *address, int length, int endchunkid) {
    STREAMHEADER *strm;
    REQUESTSTRUCT *req;
    TAPSTRUCT *tap;
    STREAMCHUNKHDR *chunk;
    STREAMSTATE streamstate;
    int lockstate;
    int chunksize;

    if (validatehandle(handle, &strm, &tap) != 0) {
        return 0;
    }

    req = getfreerequest(strm);
    if (req == nullptr) {
        return 0;
    }

    if (length == 0) {
        chunk = static_cast<STREAMCHUNKHDR *>(address);
        while (static_cast<int>(EA::Endian::little_get(&chunk->type, 4)) != endchunkid) {
            chunksize = EA::Endian::little_get(&chunk->size, 4);
            length += chunksize;
            chunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(chunk) + chunksize);
        }
        length += EA::Endian::little_get(&chunk->size, 4);
    }

    req->parm = length;
    req->address = static_cast<char *>(address);
    req->endchunkid = endchunkid;
    req->type = MEMREAD;
    queuerequest(strm, req);

    MUTEX_lock(&strm->mutex);
    streamstate = strm->state;
    if (streamstate == STREAM_IDLE) {
        strm->state = STREAM_RUNNING;
    }
    MUTEX_unlock(&strm->mutex);

    if (streamstate == STREAM_IDLE) {
        startnextrequest(strm, 0);
    }
    return req->id;
}

STREAMCHUNKHDR *STREAM_get(STREAMHANDLE handle);
void STREAM_release(STREAMHANDLE handle, STREAMCHUNKHDR *chunk);

// UNSOLVED
void STREAM_cancelrequest(STREAMHANDLE handle, int requestid) {
    STREAMHEADER *strm;
    REQUESTSTRUCT *req;
    TAPSTRUCT *tap;
    STREAMCHUNKHDR *chunk;
    char *datastart = nullptr;
    char *reqstart = nullptr;
    char *reqend = nullptr;
    int chunksize;
    int chunktap;
    int lockstate;
    int finished;
    int i;

    if (validatehandle(handle, &strm, &tap) == 0) {
        MUTEX_lock(&strm->mutex);

        req = locaterequest(strm, requestid);

        if (req == nullptr || req->state == STREAMREQUEST_CANCELED) {
            finished = 1;
        } else if (req->state == STREAMREQUEST_PENDING) {
            finished = 1;
            freerequest(strm, req);
        } else {
            req->state = STREAMREQUEST_CANCELED;
            datastart = strm->datastart;

            if (req == strm->firstreq) {
                reqstart = datastart;
            } else {
                reqstart = req->datastart;
            }

            if (req->next == nullptr || req->next->state == STREAMREQUEST_PENDING) {
                reqend = strm->datatail;
            } else {
                reqend = req->next->datastart;
            }

            finished = 0;
        }

        MUTEX_unlock(&strm->mutex);

        if (!finished) {
            for (i = 0; i < strm->taps; i++) {
                tap = strm->tap + i;

                if (tap->gettable > 0) {
                    lockstate = inbetween(datastart, reqstart, tap->getptr);

                    if (lockstate != 0) {
                        finished = reinterpret_cast<int>(reqstart);
                        chunktap = tap->tapnum << 24;

                        if (reqstart != reqend) {
                            while (reinterpret_cast<char *>(finished) != reqend) {
                                chunk = reinterpret_cast<STREAMCHUNKHDR *>(finished);

                                if (static_cast<int>(EA::Endian::little_get(&chunk->type, 4)) == -1) {
                                    finished = reinterpret_cast<int>(strm->bufferstart);
                                } else {
                                    chunksize = EA::Endian::little_get(&chunk->size, 4) & 0xFFFFFF;

                                    if ((EA::Endian::little_get(&chunk->size, 4) & 0xFF000000U) == static_cast<unsigned int>(chunktap)) {
                                        MUTEX_lock(&strm->mutex);
                                        tap->gettable -= chunksize;
                                        MUTEX_unlock(&strm->mutex);

                                        decbufferusage(strm, chunksize);
                                        EA::Endian::little_put(&chunk->type, -2, 4);
                                        EA::Endian::little_put(&chunk->size, chunksize, 4);
                                    }

                                    finished += chunksize;
                                }
                            }
                        }
                    } else {
                        // TODO does this maybe mean different compiler flags for this part?
                        if (inbetween(reqstart, reqend, tap->getptr)) {
                            do {
                                chunk = STREAM_get(reinterpret_cast<int>(tap));
                                STREAM_release(reinterpret_cast<int>(tap), chunk);

                                if (tap->gettable < 1) {
                                    break;
                                }
                            } while (inbetween(reqstart, reqend, tap->getptr));
                        }
                    }
                }
            }
        }
    }
}

void STREAM_kill(STREAMHANDLE handle) {
    volatile STREAMHEADER *strm;
    REQUESTSTRUCT *req;
    TAPSTRUCT *tap;
    STREAMCHUNKHDR *chunk;
    int chunksize = 0;
    int i;

    if (validatehandle(handle, const_cast<STREAMHEADER **>(&strm), &tap) != 0) {
        return;
    }
    req = strm->lastreq;
    if (req == nullptr) {
        return;
    }

    while (req->state == STREAMREQUEST_PENDING || req->state == STREAMREQUEST_EXECUTING) {
        STREAM_cancelrequest(handle, req->id);
        req = strm->lastreq;
    }
    while (strm->firstreq != strm->curreq) {
        freerequest(const_cast<STREAMHEADER *>(strm), strm->firstreq);
    }

    strm->curreq->state = STREAMREQUEST_CANCELED;
    for (i = 0; i < strm->taps; i++) {
        strm->tap[i].gettable = 0;
    }

    decbufferusage(const_cast<STREAMHEADER *>(strm), strm->bufferusage);
    for (chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datastart); chunk != reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);) {
        if (EA::Endian::little_get(&chunk->type, 4) == -1) {
            chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->bufferstart);
        } else {
            chunksize = EA::Endian::little_get(&chunk->size, 4) & 0x00FFFFFF;
            EA::Endian::little_put(&chunk->type, -2, 4);
            EA::Endian::little_put(&chunk->size, chunksize, 4);
            chunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(chunk) + chunksize);
        }
    }

    if (strm->state == STREAM_STOPPED) {
        if (chunk == reinterpret_cast<STREAMCHUNKHDR *>(strm->bufferstart)) {
            strm->bufferstart = strm->datatail = strm->actualbufferstart;
        } else {
            int pad = 0x20 - (reinterpret_cast<unsigned int>(strm->datatail) & 0x1F);
            if (pad == 0x20) {
                pad = 0;
            }
            chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail - chunksize);
            chunksize += pad;
            EA::Endian::little_put(&chunk->size, chunksize, 4);
            strm->datatail = strm->datatail + pad;
        }
        strm->state = STREAM_IDLE;
    }
}

// TODO missing EA::Endian::little_get call
STREAMCHUNKHDR *STREAM_get(STREAMHANDLE sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    STREAMCHUNKHDR *chunk = nullptr;
    STREAMCHUNKHDR *nextChunk;
    int chunkSize;
    int chunkTap;
    int bytesLeft;
    unsigned int nextChunkSize;
    int chunkType;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);

    if (status != 0) {
        return nullptr;
    }

    if (tapRaw->gettable == 0) {
        return nullptr;
    }

    chunk = reinterpret_cast<STREAMCHUNKHDR *>(tapRaw->getptr);
    chunkSize = EA::Endian::little_get(&chunk->size, 4) & 0xFFFFFF;
    EA::Endian::little_put(&chunk->size, chunkSize, 4);

    MUTEX_lock(&streamRaw->mutex);
    bytesLeft = tapRaw->gettable - chunkSize;
    tapRaw->gettable = bytesLeft;
    MUTEX_unlock(&streamRaw->mutex);

    if (bytesLeft > 0) {
        nextChunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(chunk) + chunkSize);
        chunkTap = tapRaw->tapnum << 24;
        while (true) {
            nextChunkSize = EA::Endian::little_get(&nextChunk->size, 4);
            if (static_cast<int>(nextChunkSize & 0xFF000000) == chunkTap) {
                break;
            }
            chunkType = EA::Endian::little_get(&nextChunk->type, 4);
            if (chunkType == -1) {
                nextChunk = reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->bufferstart);
            } else {
                chunkSize = static_cast<int>(nextChunkSize);
                chunkSize &= 0xFFFFFF;
                nextChunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(nextChunk) + chunkSize);
            }
        }
        tapRaw->getptr = reinterpret_cast<char *>(nextChunk);
    }

    return chunk;
}

void STREAM_release(STREAMHANDLE handle, STREAMCHUNKHDR *chunk) {
    STREAMHEADER *strm;
    TAPSTRUCT *tap;
    STREAMSTATE streamstate;
    int lockstate;

    if (validatehandle(handle, &strm, &tap) != 0) {
        return;
    }
    if (chunk < reinterpret_cast<STREAMCHUNKHDR *>(strm->bufferstart) || chunk > reinterpret_cast<STREAMCHUNKHDR *>(strm->bufferend - 8)) {
        return;
    }
    if (static_cast<int>(EA::Endian::little_get(&chunk->type, 4)) == -2) {
        return;
    }

    EA::Endian::little_put(&chunk->type, -2, 4);
    decbufferusage(strm, EA::Endian::little_get(&chunk->size, 4));
    MUTEX_lock(&strm->mutex);
    streamstate = strm->state;
    if (streamstate == STREAM_STOPPED) {
        strm->state = STREAM_RUNNING;
    }
    MUTEX_unlock(&strm->mutex);

    if (streamstate == STREAM_STOPPED) {
        if (strm->greedystate != 0) {
            restartstream(strm, strm->priorityhigh);
        } else {
            restartstream(strm, strm->prioritylow);
        }
    }
}

int STREAM_gettable(STREAMHANDLE sndstreamhandle) {
    STREAMHEADER *streamRaw;
    TAPSTRUCT *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return 0;
    }
    return tapRaw->gettable;
}

int STREAM_state(STREAMHANDLE sndstreamhandle) {
    STREAMHEADER *streamRaw;
    TAPSTRUCT *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return STREAM_IDLE;
    }
    return streamRaw->state;
}

int STREAM_isendofstream(STREAMHANDLE sndstreamhandle) {
    STREAMHEADER *strm;
    TAPSTRUCT *tap;
    int status = validatehandle(sndstreamhandle, &strm, &tap);
    if (status != 0) {
        return 0;
    }
    int isEnd = 0;
    if (strm->state == STREAM_IDLE) {
        isEnd = static_cast<int>(tap->gettable == 0);
    }
    return isEnd;
}

int STREAM_buffersize(STREAMHANDLE sndstreamhandle) {
    STREAMHEADER *streamRaw;
    TAPSTRUCT *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return 0;
    }

    return static_cast<int>(streamRaw->bufferend - streamRaw->actualbufferstart);
}
