#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Libs/realcore/include/common/realcore/system.h"

struct STREAMCHUNKHDR {
    int type; // offset 0x0, size 0x4
    int size; // offset 0x4, size 0x4
};

struct REQUESTSTRUCTtag;
struct TAPSTRUCTtag;
struct FILTERSTRUCTtag;
struct STREAMHEADERtag;

typedef enum READTYPE {
    FILEREAD = 0,
    MEMREAD = 1,
} READTYPE;

typedef enum STREAMREQUESTSTATE {
    STREAMREQUEST_FREE = 0,
    STREAMREQUEST_PENDING = 1,
    STREAMREQUEST_EXECUTING = 2,
    STREAMREQUEST_COMPLETED = 3,
    STREAMREQUEST_CANCELED = 4,
} STREAMREQUESTSTATE;

typedef enum STREAMSTATE {
    STREAM_IDLE_STATE = 0,
    STREAM_RUNNING_STATE = 1,
    STREAM_STOPPED_STATE = 2,
} STREAMSTATE;

// total size: 0x124
typedef struct REQUESTSTRUCTtag {
    int id;                          // offset 0x0, size 0x4
    STREAMREQUESTSTATE state;        // offset 0x4, size 0x4
    REQUESTSTRUCTtag *prev;         // offset 0x8, size 0x4
    REQUESTSTRUCTtag *next;         // offset 0xC, size 0x4
    READTYPE type;                   // offset 0x10, size 0x4
    char fname[255];                 // offset 0x14, size 0xFF
    char *address;                   // offset 0x114, size 0x4
    int parm;                        // offset 0x118, size 0x4
    int endchunkid;                  // offset 0x11C, size 0x4
    char *datastart;                 // offset 0x120, size 0x4
} REQUESTSTRUCT;

// total size: 0x10
typedef struct TAPSTRUCTtag {
    STREAMHEADERtag *stream; // offset 0x0, size 0x4
    int tapnum;              // offset 0x4, size 0x4
    int gettable;            // offset 0x8, size 0x4
    char *getptr;            // offset 0xC, size 0x4
} TAPSTRUCT;

// total size: 0xC
typedef struct FILTERSTRUCTtag {
    int mask;   // offset 0x0, size 0x4
    int value;  // offset 0x4, size 0x4
    int tapnum; // offset 0x8, size 0x4
} FILTERSTRUCT;

// total size: 0x18C
typedef struct STREAMHEADERtag {
    int id;                    // offset 0x0, size 0x4
    MUTEX mutex;               // offset 0x4, size 0x1C
    REQUESTSTRUCT *request;    // offset 0x20, size 0x4
    int requests;              // offset 0x24, size 0x4
    FILTERSTRUCT *filter;      // offset 0x28, size 0x4
    int filters;               // offset 0x2C, size 0x4
    TAPSTRUCT *tap;            // offset 0x30, size 0x4
    int taps;                  // offset 0x34, size 0x4
    char *actualbufferstart;   // offset 0x38, size 0x4
    char *bufferstart;         // offset 0x3C, size 0x4
    char *bufferend;           // offset 0x40, size 0x4
    volatile STREAMSTATE state; // offset 0x44, size 0x4
    int prioritylow;           // offset 0x48, size 0x4
    int priorityhigh;          // offset 0x4C, size 0x4
    int greedylevel;           // offset 0x50, size 0x4
    int greedystate;           // offset 0x54, size 0x4
    int bufferusage;           // offset 0x58, size 0x4
    char *datastart;           // offset 0x5C, size 0x4
    char *datatail;            // offset 0x60, size 0x4
    char *dataend;             // offset 0x64, size 0x4
    REQUESTSTRUCT *firstreq;   // offset 0x68, size 0x4
    REQUESTSTRUCT *curreq;     // offset 0x6C, size 0x4
    REQUESTSTRUCT *lastreq;    // offset 0x70, size 0x4
    REQUESTSTRUCT *freereq;    // offset 0x74, size 0x4
    char fname[255];           // offset 0x78, size 0xFF
    int fhandle;               // offset 0x178, size 0x4
    int foffset;               // offset 0x17C, size 0x4
    int fop;                   // offset 0x180, size 0x4
    int readsize;              // offset 0x184, size 0x4
    int readblocksize;         // offset 0x188, size 0x4
} STREAMHEADER;

typedef char StreamRequestSizeCheck[(sizeof(REQUESTSTRUCT) == 0x124) ? 1 : -1];
typedef char StreamTapSizeCheck[(sizeof(TAPSTRUCT) == 0x10) ? 1 : -1];
typedef char StreamFilterSizeCheck[(sizeof(FILTERSTRUCT) == 0xC) ? 1 : -1];
typedef char StreamHeaderSizeCheck[(sizeof(STREAMHEADER) == 0x18C) ? 1 : -1];

inline int ReadChunkWord(const int *field) {
    const unsigned char *bytes = static_cast<const unsigned char *>(static_cast<const void *>(field));
    return (static_cast<int>(bytes[3]) << 24) | (static_cast<int>(bytes[2]) << 16) | (static_cast<int>(bytes[1]) << 8) |
           static_cast<int>(bytes[0]);
}

inline unsigned int little_get(const void *src, int n) {
    return (static_cast<const unsigned char *>(src)[3] << 24) |
           (static_cast<const unsigned char *>(src)[2] << 16) |
           (static_cast<const unsigned char *>(src)[1] << 8) |
           static_cast<const unsigned char *>(src)[0];
}

inline void little_put(void *dst, unsigned int data, int n) {
    static_cast<unsigned char *>(dst)[0] = static_cast<unsigned char>(data);
    static_cast<unsigned char *>(dst)[1] = static_cast<unsigned char>(data >> 8);
    static_cast<unsigned char *>(dst)[2] = static_cast<unsigned char>(data >> 16);
    static_cast<unsigned char *>(dst)[3] = static_cast<unsigned char>(data >> 24);
}

inline void WriteChunkWord(int *field, int value) {
    unsigned char *bytes = static_cast<unsigned char *>(static_cast<void *>(field));
    bytes[0] = static_cast<unsigned char>(value);
    bytes[1] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 8);
    bytes[2] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 16);
    bytes[3] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 24);
}

inline unsigned int ReadChunkTapTag(const int *field) {
    return static_cast<unsigned int>(ReadChunkWord(field)) & 0xFF000000U;
}

inline void OrChunkTapTag(int *field, int tapIndex) {
    unsigned int chunkWord = static_cast<unsigned int>(ReadChunkWord(field));
    WriteChunkWord(field, static_cast<int>(chunkWord | (static_cast<unsigned int>(tapIndex) << 24)));
}

class FILEOPERATION;

static STREAMHEADERtag *g_StreamTable[64];
static int g_NextStreamHandle = 1;
static int requestidcounter;

void AssignAudioStreamHandle(unsigned int realstrmhandle);
extern FILEOPERATION *FILESYS_read(int fhandle, int foffset, void *buffer, int size, int priority, void *userdata);
extern FILEOPERATION *FILESYS_open(const char *filename, unsigned int mode, int priority, void *userdata);
extern FILEOPERATION *FILESYS_close(int fhandle, int priority, void *userdata);
extern void FILESYS_callbackop(FILEOPERATION *op, int (*cb)(int, int, void *));
extern int FILESYS_completeop(int fop);
extern long long FILESYS_completeop64(int fop);
extern int FILESYS_closesync(int fhandle, int priority);
extern int FILESYS_priorityop(int fop, int priority);
extern bool IsWorldDataStreaming(unsigned int strmhandle);
extern bool gbAudioInterruptsWorldDataRead;
extern bool gbWorldDataBlocksAudioRead;
extern bool bReadCallbackToggle;
extern unsigned int utickreadcallback;
extern unsigned int uTicksSinceLastAudioReadBailed;
extern float bGetTickerDifference(unsigned int start_ticks);
extern void SYNCTASK_run();
extern void THREAD_yield(int);
extern void MEM_clear(void *dest, int bytes);
extern void MEM_copy(void *dest, const void *src, int bytes);
STREAMCHUNKHDR *STREAM_get(int sndstreamhandle);
void STREAM_release(int sndstreamhandle, STREAMCHUNKHDR *chunk);
void STREAM_kill(int sndstreamhandle);
void STREAM_setgreedystate(int sndstreamhandle, int greedystate);
void restartstream(STREAMHEADERtag *stream, int priority);
void startnextrequest(STREAMHEADERtag *stream, int priority);

static STREAMHEADERtag *GetHeader(int handle) {
    if (handle <= 0 || handle >= 64) {
        return nullptr;
    }
    return g_StreamTable[handle];
}

static int validatehandle(int handle, STREAMHEADERtag **strmptr, TAPSTRUCTtag **tapptr) {
    if (handle == 0) {
        return 1;
    }

    STREAMHEADERtag *strm = *reinterpret_cast<STREAMHEADERtag **>(handle);
    TAPSTRUCTtag *tap;
    if (strm->id != 0x4D525453) {
        return 1;
    }
    tap = reinterpret_cast<TAPSTRUCTtag *>(handle);
    *tapptr = tap;
    *strmptr = strm;
    return 0;
}

static int inbetween(char *startptr, char *endptr, char *ptr) {
    if (startptr <= endptr) {
        if (ptr < startptr) {
            return 0;
        }
        return ptr < endptr;
    }
    if (ptr < startptr) {
        if (ptr >= endptr) {
            return 0;
        }
    }
    return 1;
}

static void decbufferusage(STREAMHEADERtag *strm, int amount) {
    int lockstate;
    int oldbufferusage;
    int bufferusage;
    int greedylevel;

    MUTEX_lock(&strm->mutex);
    oldbufferusage = strm->bufferusage;
    bufferusage = oldbufferusage - amount;
    strm->bufferusage = bufferusage;
    MUTEX_unlock(&strm->mutex);

    greedylevel = strm->greedylevel;
    if (oldbufferusage >= greedylevel && bufferusage < greedylevel &&
        (strm->greedystate = 1, strm->state == STREAM_RUNNING_STATE)) {
        FILESYS_priorityop(strm->fop, strm->priorityhigh);
    }
}

static REQUESTSTRUCTtag *getfreerequest(STREAMHEADERtag *strm) {
    REQUESTSTRUCTtag *req = nullptr;
    int lockstate;

    MUTEX_lock(&strm->mutex);
    if (strm->freereq) {
        req = strm->freereq;
        strm->freereq = req->next;
        requestidcounter += 0x100;
        if (requestidcounter == 0) {
            requestidcounter = 0x100;
        }
        req->id = (req->id & 0xFF) | requestidcounter;
    }
    MUTEX_unlock(&strm->mutex);

    return req;
}

static void queuerequest(STREAMHEADERtag *strm, REQUESTSTRUCTtag *req) {
    int lockstate;

    req->state = STREAMREQUEST_PENDING;
    req->next = nullptr;
    MUTEX_lock(&strm->mutex);
    if (!strm->lastreq) {
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

static REQUESTSTRUCTtag *locaterequest(STREAMHEADERtag *strm, int requestid) {
    REQUESTSTRUCTtag *req;
    int index = requestid & 0xFF;
    if (index >= strm->requests) {
        return nullptr;
    }

    req = strm->request + index;
    if (requestid == strm->request[index].id) {
        if (req->state == STREAMREQUEST_FREE) {
            return nullptr;
        }
        return req;
    }
    return nullptr;
}

static void freerequest(STREAMHEADERtag *strm, REQUESTSTRUCTtag *req) {
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
        if (!req->next) {
            strm->curreq = req->prev;
        } else {
            strm->curreq = req->next;
        }
    }

    req->state = STREAMREQUEST_FREE;
    req->next = strm->freereq;
    strm->freereq = req;
}

static int filterchunk(STREAMHEADERtag *strm, STREAMCHUNKHDR *chunk) {
    FILTERSTRUCTtag *filt;
    int chunktype = little_get(&chunk->type, 4);
    int i;

    for (i = 0; i < strm->filters; i++) {
        filt = strm->filter + i;
        if ((chunktype & filt->mask) == filt->value) {
            return filt->tapnum;
        }
    }
    return -2;
}

int parsechunks(STREAMHEADERtag *stream) {
    if (!stream) {
        return -1;
    }

    STREAMHEADER *strm = reinterpret_cast<STREAMHEADER *>(stream);
    STREAMCHUNKHDR *chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);
    REQUESTSTRUCT *req = strm->curreq;
    int bytesAvailable = strm->dataend - reinterpret_cast<char *>(chunk);

    while (bytesAvailable > 7) {
        int chunkSize = ReadChunkWord(&chunk->size);
        if (ReadChunkTapTag(&chunk->size) != 0) {
            chunkSize = 8;
            WriteChunkWord(&chunk->type, req->endchunkid);
            WriteChunkWord(&chunk->size, chunkSize);
        }

        if (strm->dataend < strm->datatail + chunkSize) {
            break;
        }

        if (ReadChunkWord(&chunk->type) == req->endchunkid) {
            unsigned int alignmentOffset = static_cast<unsigned int>(reinterpret_cast<unsigned int>(strm->datatail) & 0x1F);
            chunkSize = ((chunkSize + alignmentOffset + 0x1F) & ~0x1F) - alignmentOffset;
        }

        int tapIndex = filterchunk(stream, chunk);
        bool requestCanceled;

        if (tapIndex < 0) {
            MUTEX_lock(&strm->mutex);
            requestCanceled = req->state == STREAMREQUEST_CANCELED;
            if (!requestCanceled) {
                WriteChunkWord(&chunk->type, -2);
                strm->datatail = strm->datatail + chunkSize;
            }
            MUTEX_unlock(&strm->mutex);
        } else {
            WriteChunkWord(&chunk->size, chunkSize);
            OrChunkTapTag(&chunk->size, tapIndex);

            MUTEX_lock(&strm->mutex);
            requestCanceled = req->state == STREAMREQUEST_CANCELED;
            if (!requestCanceled) {
                TAPSTRUCT *tap = strm->tap;
                int tapBytes = tap[tapIndex - 1].gettable + chunkSize;
                tap[tapIndex - 1].gettable = tapBytes;
                if (tapBytes == chunkSize) {
                    tap[tapIndex - 1].getptr = reinterpret_cast<char *>(chunk);
                }

                int oldUsage = strm->bufferusage;
                int newUsage = oldUsage + chunkSize;
                strm->bufferusage = newUsage;
                strm->datatail = strm->datatail + chunkSize;
                if (oldUsage < strm->greedylevel && strm->greedylevel <= newUsage) {
                    strm->greedystate = 0;
                }
            }
            MUTEX_unlock(&strm->mutex);
        }

        if (requestCanceled) {
            if (ReadChunkWord(&chunk->type) == req->endchunkid) {
                return 0;
            }

            unsigned int alignmentOffset = static_cast<unsigned int>(reinterpret_cast<unsigned int>(strm->datatail) & 0x1F);
            int alignedSize = ((chunkSize + alignmentOffset + 0x1F) & ~0x1F) - alignmentOffset;
            WriteChunkWord(&chunk->size, alignedSize);
            OrChunkTapTag(&chunk->size, tapIndex);
            return 0;
        }

        if (ReadChunkWord(&chunk->type) == req->endchunkid) {
            return 1;
        }

        chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);
        bytesAvailable = strm->dataend - reinterpret_cast<char *>(chunk);
    }

    return 0;
}

static void opencallback(int, int, void *userdata) {
    STREAMHEADERtag *strm = static_cast<STREAMHEADERtag *>(userdata);

    strm->fhandle = static_cast<int>(FILESYS_completeop64(strm->fop));
    if (strm->fhandle == 0) {
        strm->state = STREAM_IDLE_STATE;
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
    STREAMHEADERtag *strm = static_cast<STREAMHEADERtag *>(userdata);

    FILESYS_completeop(strm->fop);
    strm->fop = reinterpret_cast<int>(FILESYS_open(strm->fname, 1, strm->priorityhigh, strm));
    if (strm->fop) {
        FILESYS_callbackop(reinterpret_cast<FILEOPERATION *>(strm->fop),
            reinterpret_cast<int (*)(int, int, void *)>(opencallback));
    }
}

static void readcallback(int, int, void *userdata) {
    bReadCallbackToggle = true;
    STREAMHEADERtag *strm = static_cast<STREAMHEADERtag *>(userdata);
    REQUESTSTRUCTtag *req = strm->curreq;
    int lockstate;
    int bytesread;
    int endoffile;
    int endchunk;

    if (req->type == 1) {
        bytesread = strm->readsize;
        endoffile = strm->foffset + bytesread >= req->parm;
    } else {
        bytesread = static_cast<int>(FILESYS_completeop64(strm->fop));
        endoffile = bytesread < strm->readsize;
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

void startnextrequest(STREAMHEADERtag *stream, int priority) {
    STREAMHEADER *header = reinterpret_cast<STREAMHEADER *>(stream);
    REQUESTSTRUCT *request = nullptr;
    int noPendingRequest;

    MUTEX_lock(&header->mutex);
    request = header->curreq;
    if (!request) {
        noPendingRequest = 1;
    } else {
        if (request->state != STREAMREQUEST_PENDING) {
            if (!request->next) {
                noPendingRequest = 1;
            } else {
                header->curreq = request->next;
                noPendingRequest = 0;
            }
        } else {
            noPendingRequest = 0;
        }
    }

    if (noPendingRequest) {
        header->state = STREAM_IDLE_STATE;
    } else {
        request = header->curreq;
        request->state = STREAMREQUEST_EXECUTING;
        request->datastart = header->datatail;
    }
    MUTEX_unlock(&header->mutex);

    if (!noPendingRequest) {
        header->dataend = header->datatail;
        if (request->type == 1) {
            header->foffset = 0;
        } else {
            header->foffset = request->parm;
            if (strcmp(request->fname, header->fname) != 0) {
                strcpy(header->fname, request->fname);
                if (header->fhandle == 0) {
                    FILEOPERATION *fop = FILESYS_open(header->fname, 1, priority, stream);
                    header->fop = reinterpret_cast<int>(fop);
                    if (!fop) {
                        return;
                    }
                    FILESYS_callbackop(fop, reinterpret_cast<int (*)(int, int, void *)>(opencallback));
                    return;
                }
                FILEOPERATION *fop = FILESYS_close(header->fhandle, priority, stream);
                header->fop = reinterpret_cast<int>(fop);
                if (!fop) {
                    return;
                }
                FILESYS_callbackop(fop, reinterpret_cast<int (*)(int, int, void *)>(closecallback));
                return;
            }
        }
        restartstream(stream, priority);
    }
}

void restartstream(STREAMHEADERtag *stream, int priority) {
    STREAMHEADER *strm = reinterpret_cast<STREAMHEADER *>(stream);
    REQUESTSTRUCT *req;
    STREAMCHUNKHDR *chunk;
    char *reqend;
    int lockstate;
    int largestread;
    int tailsize;
    FILEOPERATION *op;

    while (strm->datastart != strm->datatail) {
        chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datastart);
        int marker = ReadChunkWord(&chunk->type);
        if (marker == -1) {
            strm->datastart = strm->bufferstart;
        } else {
            if (marker != -2) {
                break;
            }
            strm->datastart = reinterpret_cast<char *>(chunk) + ReadChunkWord(&chunk->size);
        }
    }

    MUTEX_lock(&strm->mutex);
    while (true) {
        req = strm->firstreq;
        if (!req->next || req->next->state == STREAMREQUEST_PENDING ||
            inbetween(strm->datastart, strm->dataend, req->next->datastart - 1)) {
            break;
        }
        freerequest(stream, reinterpret_cast<REQUESTSTRUCTtag *>(req));
    }
    MUTEX_unlock(&strm->mutex);

    if (strm->datastart > strm->dataend) {
        largestread = strm->datastart - strm->dataend - 0x21;
    } else {
        largestread = strm->bufferend - strm->dataend - 0x20;
        if (largestread < strm->readblocksize) {
            unsigned int count = strm->dataend - strm->datatail;
            if (strm->curreq->type == 1) {
                if (strm->datastart - strm->bufferstart < static_cast<int>(count + 1)) {
                    goto stream_stop;
                }
            } else if (strm->datastart - strm->bufferstart - 0x20 < static_cast<int>(count + 1)) {
                goto stream_stop;
            }

            if ((count & 0x1F) == 0 || strm->curreq->type == 1) {
                strm->bufferstart = strm->actualbufferstart;
            } else {
                strm->bufferstart = strm->actualbufferstart - (static_cast<int>(count) % 0x20 - 0x20);
            }
            MEM_copy(strm->bufferstart, strm->datatail, count);

            chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);
            WriteChunkWord(&chunk->type, -1);
            WriteChunkWord(&chunk->size, 8);

            reqend = strm->bufferstart + count;
            strm->datatail = strm->bufferstart;
            strm->dataend = reqend;

            chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datastart);
            if (ReadChunkWord(&chunk->type) == -1) {
                strm->datastart = strm->bufferstart;
                largestread = strm->bufferend - reqend - 0x20;
            } else {
                largestread = reinterpret_cast<char *>(chunk) - reqend - 1;
            }
        }
    }

    if (strm->greedystate == 0 && strm->priorityhigh < priority &&
        IsWorldDataStreaming(reinterpret_cast<unsigned int>(strm->tap))) {
        bGetTickerDifference(utickreadcallback);
        gbWorldDataBlocksAudioRead = true;
        strm->state = STREAM_STOPPED_STATE;
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
        goto stream_stop;
    }

    req = strm->curreq;
    if (req->type == 1) {
        if (largestread + strm->foffset > req->parm) {
            strm->readsize = req->parm - strm->foffset;
        } else {
            strm->readsize = largestread;
        }

        MEM_copy(strm->dataend, req->address, strm->readsize);
        req->address = req->address + strm->readsize;
        readcallback(0, 0, stream);
        return;
    }

    if (largestread > strm->readblocksize) {
        strm->readsize = (largestread / strm->readblocksize) * strm->readblocksize;
    } else {
        strm->readsize = strm->readblocksize;
    }

    bReadCallbackToggle = false;
    op = FILESYS_read(strm->fhandle, strm->foffset, strm->dataend, strm->readsize, priority, stream);
    strm->fop = reinterpret_cast<int>(op);
    if (!op) {
        return;
    }

    FILESYS_callbackop(op, reinterpret_cast<int (*)(int, int, void *)>(readcallback));
    return;

stream_stop:
    strm->state = STREAM_STOPPED_STATE;
}

int STREAM_overhead(int requests, int filters, int taps) {
    int filterBytes = filters * 0xC;
    int tapBytes = taps << 4;
    int requestBytes = requests * 0x124;
    filterBytes += 0x18C;
    requestBytes += filterBytes;
    requestBytes += tapBytes;
    requestBytes += 0x20;
    return requestBytes;
}

int STREAM_create(int requests, int filters, int taps, void *buffer, int size) {
    STREAMHEADER *strm;
    REQUESTSTRUCT *req;
    TAPSTRUCT *tap;
    int i;
    int overhead = STREAM_overhead(requests, filters, taps);
    overhead = size - overhead;
    if (overhead < 0x1800 || requests < 2 || requests > 0x100 || static_cast<unsigned int>(filters - 1) > 0xF ||
        taps < 1 || filters < taps) {
        return 0;
    }

    strm = static_cast<STREAMHEADER *>(buffer);
    strm->id = 0x4D525453;
    MUTEX_create(&strm->mutex);

    req = reinterpret_cast<REQUESTSTRUCT *>(static_cast<char *>(buffer) + sizeof(STREAMHEADER));
    strm->requests = requests;
    strm->tap = reinterpret_cast<TAPSTRUCT *>(static_cast<void *>(req[requests].fname + filters * sizeof(FILTERSTRUCT) - 0x14));
    strm->filters = filters;
    strm->taps = taps;
    strm->state = STREAM_IDLE_STATE;

    tap = reinterpret_cast<TAPSTRUCT *>((reinterpret_cast<unsigned int>(reinterpret_cast<char *>(strm->tap) + taps * sizeof(TAPSTRUCT)) &
                                         0xFFFFFFE0U) +
                                        0x20);

    strm->greedylevel = 0;
    strm->greedystate = 0;
    strm->bufferusage = 0;
    strm->firstreq = nullptr;
    strm->curreq = nullptr;
    strm->lastreq = nullptr;
    strm->bufferend = static_cast<char *>(buffer) + size;
    strm->prioritylow = 0x96;
    strm->priorityhigh = 0x32;
    strm->dataend = static_cast<char *>(static_cast<void *>(tap));
    strm->freereq = req;
    strm->request = req;
    strm->filter = reinterpret_cast<FILTERSTRUCT *>(req + requests);
    strm->actualbufferstart = static_cast<char *>(static_cast<void *>(tap));
    strm->bufferstart = static_cast<char *>(static_cast<void *>(tap));
    strm->datastart = static_cast<char *>(static_cast<void *>(tap));
    strm->datatail = static_cast<char *>(static_cast<void *>(tap));

    MEM_clear(strm->fname, 0xFF);
    strm->fhandle = 0;

    if (overhead < 0x4000) {
        strm->readblocksize = 0x800;
    } else if (overhead <= 0x7FFF) {
        strm->readblocksize = 0x1000;
    } else if (overhead <= 0xFFFF) {
        strm->readblocksize = 0x2000;
    } else if (overhead <= 0x17FFF) {
        strm->readblocksize = 0x4000;
    } else {
        strm->readblocksize = 0x8000;
    }

    i = 0;
    while (i < requests) {
        REQUESTSTRUCT *request = static_cast<REQUESTSTRUCT *>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(strm->request)) + i * sizeof(REQUESTSTRUCT)));
        request->id = i;
        ++i;
        request->state = STREAMREQUEST_FREE;
        request->next = static_cast<REQUESTSTRUCT *>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(strm->request)) + i * sizeof(REQUESTSTRUCT)));
    }
    reinterpret_cast<REQUESTSTRUCT *>(static_cast<char *>(static_cast<void *>(strm->request)) + requests * sizeof(REQUESTSTRUCT) -
                                     sizeof(REQUESTSTRUCT))
        ->next = nullptr;

    for (i = 0; i < filters; ++i) {
        FILTERSTRUCT *filter = strm->filter + i;
        filter->mask = 0;
        filter->value = 0;
        filter->tapnum = 1;
    }

    i = 0;
    while (i < taps) {
        TAPSTRUCT *streamTap = strm->tap + i;
        streamTap->stream = static_cast<STREAMHEADERtag *>(buffer);
        ++i;
        streamTap->tapnum = i;
        streamTap->gettable = 0;
    }

    AssignAudioStreamHandle(reinterpret_cast<unsigned int>(strm->tap));
    return reinterpret_cast<int>(strm->tap);
}

void STREAM_setfilter(int handle, int filternum, int mask, int value, int tapnum) {
    STREAMHEADERtag *strm;
    FILTERSTRUCTtag *filt;
    TAPSTRUCTtag *tap;
    if (validatehandle(handle, &strm, &tap) == 0 && filternum > 0) {
        if (filternum <= strm->filters &&
            (filternum != strm->filters || (mask | value) == 0) &&
            (tapnum > 0 || tapnum == -1 || tapnum == -2) && tapnum <= strm->taps &&
            strm->state == STREAM_IDLE_STATE) {
            filt = strm->filter + (filternum - 1);
            filt->mask = mask;
            filt->value = value;
            filt->tapnum = tapnum;
        }
    }
}

void STREAM_destroy(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status != 0) {
        return;
    }

    STREAM_kill(sndstreamhandle);
    while (streamRaw->state == STREAM_RUNNING_STATE) {
        SYNCTASK_run();
        THREAD_yield(0);
    }

    streamRaw->id = 0;
    MUTEX_destroy(&streamRaw->mutex);
    if (streamRaw->fhandle != 0) {
        FILESYS_closesync(streamRaw->fhandle, 100);
    }
}

void STREAM_setpriority(int sndstreamhandle, int prioritylow, int priorityhigh) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        streamRaw->prioritylow = prioritylow;
        streamRaw->priorityhigh = priorityhigh;
    }
}

void STREAM_setgreedylevel(int handle, int greedylevel) {
    STREAMHEADERtag *strm;
    TAPSTRUCTtag *tap;
    int oldgreedylevel;
    int bufferusage;
    int oldside;
    int newside;
    if (validatehandle(handle, &strm, &tap) == 0) {
        oldgreedylevel = strm->greedylevel;
        strm->greedylevel = greedylevel;
        bufferusage = strm->bufferusage;
        oldside = bufferusage < oldgreedylevel;
        newside = bufferusage < greedylevel;
        if (oldside != newside) {
            STREAM_setgreedystate(handle, newside);
        }
    }
}

void STREAM_setgreedystate(int sndstreamhandle, int greedystate) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        streamRaw->greedystate = greedystate;
        if (greedystate != 0 && streamRaw->state == STREAM_RUNNING_STATE) {
            FILESYS_priorityop(streamRaw->fop, streamRaw->priorityhigh);
        }
    }
}

int STREAM_taphandle(int handle, int tapnum) {
    STREAMHEADERtag *strm;
    TAPSTRUCTtag *tap;
    if (validatehandle(handle, &strm, &tap) != 0 || tapnum <= 0) {
        return 0;
    }
    if (tapnum <= strm->taps) {
        return reinterpret_cast<int>(strm->tap + (tapnum - 1));
    }
    return 0;
}

int STREAM_queuefile(int handle, const char *fname, int offset, int endchunkid) {
    STREAMHEADERtag *strm;
    REQUESTSTRUCTtag *req;
    TAPSTRUCTtag *tap;
    STREAMSTATE streamstate;
    int lockstate;
    if (validatehandle(handle, &strm, &tap) != 0) {
        return 0;
    }

    req = getfreerequest(strm);
    if (!req) {
        return 0;
    }

    req->type = FILEREAD;
    strncpy(req->fname, fname, 0xFE);
    req->parm = offset;
    req->endchunkid = endchunkid;
    queuerequest(strm, req);

    MUTEX_lock(&strm->mutex);
    streamstate = strm->state;
    if (streamstate == STREAM_IDLE_STATE) {
        strm->state = STREAM_RUNNING_STATE;
    }
    MUTEX_unlock(&strm->mutex);

    if (streamstate == STREAM_IDLE_STATE) {
        if (strm->greedystate != 0) {
            startnextrequest(strm, strm->priorityhigh);
        } else {
            startnextrequest(strm, strm->prioritylow);
        }
    }

    return req->id;
}

int STREAM_queuemem(int handle, void *address, int length, int endchunkid) {
    STREAMHEADERtag *strm;
    REQUESTSTRUCTtag *req;
    TAPSTRUCTtag *tap;
    STREAMCHUNKHDR *chunk;
    STREAMSTATE streamstate;
    int lockstate;
    int chunksize;
    if (validatehandle(handle, &strm, &tap) != 0) {
        return 0;
    }
    req = getfreerequest(strm);
    if (!req) {
        return 0;
    }
    if (length == 0) {
        chunk = static_cast<STREAMCHUNKHDR *>(address);
        while (static_cast<int>(little_get(&chunk->type, 4)) != endchunkid) {
            chunksize = little_get(&chunk->size, 4);
            length += chunksize;
            chunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(chunk) + chunksize);
        }
        length += little_get(&chunk->size, 4);
    }

    req->parm = length;
    req->address = static_cast<char *>(address);
    req->endchunkid = endchunkid;
    req->type = MEMREAD;
    queuerequest(strm, req);

    MUTEX_lock(&strm->mutex);
    streamstate = strm->state;
    if (streamstate == STREAM_IDLE_STATE) {
        strm->state = STREAM_RUNNING_STATE;
    }
    MUTEX_unlock(&strm->mutex);

    if (streamstate == STREAM_IDLE_STATE) {
        startnextrequest(strm, 0);
    }
    return req->id;
}

void STREAM_cancelrequest(int handle, int requestid) {
    STREAMHEADERtag *strm;
    REQUESTSTRUCTtag *req;
    TAPSTRUCTtag *tap;
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
        if (!req || req->state == STREAMREQUEST_CANCELED) {
            finished = true;
        } else if (req->state == STREAMREQUEST_PENDING) {
            finished = true;
            freerequest(strm, req);
        } else {
            req->state = STREAMREQUEST_CANCELED;
            datastart = strm->datastart;
            if (req == strm->firstreq) {
                reqstart = datastart;
            } else {
                reqstart = req->datastart;
            }
            req = req->next;
            if (!req || req->state == STREAMREQUEST_PENDING) {
                reqend = strm->datatail;
            } else {
                reqend = req->datastart;
            }
            finished = false;
        }
        MUTEX_unlock(&strm->mutex);
        if ((!finished) && (i = 0, i < strm->taps)) {
            do {
                tap = strm->tap + i;
                if (tap->gettable > 0) {
                    lockstate = inbetween(datastart, reqstart, tap->getptr);
                    if (lockstate != 0) {
                        finished = reinterpret_cast<int>(reqstart);
                        chunktap = tap->tapnum << 24;
                        if (reqstart != reqend) {
                            while (reinterpret_cast<char *>(finished) != reqend) {
                                chunk = reinterpret_cast<STREAMCHUNKHDR *>(finished);
                                if (static_cast<int>(little_get(&chunk->type, 4)) == -1) {
                                    finished = reinterpret_cast<int>(strm->bufferstart);
                                } else {
                                    chunksize = little_get(&chunk->size, 4) & 0xFFFFFF;
                                    if ((little_get(&chunk->size, 4) & 0xFF000000U) ==
                                        static_cast<unsigned int>(chunktap)) {
                                        MUTEX_lock(&strm->mutex);
                                        tap->gettable -= chunksize;
                                        MUTEX_unlock(&strm->mutex);
                                        decbufferusage(strm, chunksize);
                                        little_put(&chunk->type, -2, 4);
                                        little_put(&chunk->size, chunksize, 4);
                                    }
                                    finished += chunksize;
                                }
                            }
                        }
                    } else {
                        while (inbetween(reqstart, reqend, tap->getptr)) {
                            chunk = STREAM_get(reinterpret_cast<int>(tap));
                            STREAM_release(reinterpret_cast<int>(tap), chunk);
                            if (tap->gettable < 1) {
                                break;
                            }
                        }
                    }
                }
                i++;
            } while (i < strm->taps);
        }
    }
}

void STREAM_kill(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    int chunkSize = 0;
    if (status == 0) {
        REQUESTSTRUCT *request = streamRaw->lastreq;
        if (request) {
            while (request->state - STREAMREQUEST_PENDING < 2) {
                STREAM_cancelrequest(sndstreamhandle, request->id);
                request = streamRaw->lastreq;
            }
            while (streamRaw->firstreq != streamRaw->curreq) {
                freerequest(streamRaw, reinterpret_cast<REQUESTSTRUCTtag *>(streamRaw->firstreq));
            }

            int tapIndex = 0;
            streamRaw->curreq->state = STREAMREQUEST_CANCELED;
            if (streamRaw->taps > 0) {
                int nextTap;
                do {
                    nextTap = tapIndex + 1;
                    streamRaw->tap[tapIndex].gettable = 0;
                    tapIndex = nextTap;
                } while (nextTap < streamRaw->taps);
            }

            decbufferusage(streamRaw, streamRaw->bufferusage);
            STREAMCHUNKHDR *chunk = reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->datastart);
            if (chunk != reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->datatail)) {
                do {
                    if (ReadChunkWord(&chunk->type) == -1) {
                        chunk = reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->bufferstart);
                    } else {
                        chunkSize = ReadChunkWord(&chunk->size) & 0x00FFFFFF;
                        WriteChunkWord(&chunk->type, -2);
                        WriteChunkWord(&chunk->size, chunkSize);
                        chunk = reinterpret_cast<STREAMCHUNKHDR *>(
                            reinterpret_cast<char *>(chunk) + chunkSize);
                    }
                } while (chunk != reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->datatail));
            }

            if (streamRaw->state == STREAM_STOPPED_STATE) {
                if (chunk == reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->bufferstart)) {
                    streamRaw->datatail = streamRaw->actualbufferstart;
                    streamRaw->bufferstart = streamRaw->datatail;
                } else {
                    int align = 0x20 - (reinterpret_cast<unsigned int>(streamRaw->datatail) & 0x1F);
                    if (align == 0x20) {
                        align = 0;
                    }
                    int alignedSize = chunkSize + align;
                    STREAMCHUNKHDR *lastChunk = reinterpret_cast<STREAMCHUNKHDR *>(
                        streamRaw->datatail - chunkSize);
                    WriteChunkWord(&lastChunk->size, alignedSize);
                    streamRaw->datatail = streamRaw->datatail + align;
                }
                streamRaw->state = STREAM_IDLE_STATE;
            }
        }
    }
}

STREAMCHUNKHDR *STREAM_get(int sndstreamhandle) {
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
    chunkSize = ReadChunkWord(&chunk->size) & 0xFFFFFF;
    WriteChunkWord(&chunk->size, chunkSize);

    MUTEX_lock(&streamRaw->mutex);
    bytesLeft = tapRaw->gettable - chunkSize;
    tapRaw->gettable = bytesLeft;
    MUTEX_unlock(&streamRaw->mutex);

    if (bytesLeft > 0) {
        nextChunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(&chunk->type) + chunkSize);
        chunkTap = tapRaw->tapnum << 24;
        while (true) {
            const unsigned char *nextChunkSizeBytes =
                static_cast<const unsigned char *>(static_cast<const void *>(&nextChunk->size));
            nextChunkSize = (static_cast<unsigned int>(nextChunkSizeBytes[3]) << 24) |
                            (static_cast<unsigned int>(nextChunkSizeBytes[2]) << 16) |
                            (static_cast<unsigned int>(nextChunkSizeBytes[1]) << 8) |
                            static_cast<unsigned int>(nextChunkSizeBytes[0]);
            if (static_cast<int>(nextChunkSize & 0xFF000000) == chunkTap) {
                break;
            }
            chunkType = ReadChunkWord(&nextChunk->type);
            if (chunkType == -1) {
                nextChunk = reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->bufferstart);
            } else {
                chunkSize = static_cast<int>(nextChunkSize);
                chunkSize &= 0xFFFFFF;
                nextChunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(&nextChunk->type) + chunkSize);
            }
        }
        tapRaw->getptr = reinterpret_cast<char *>(nextChunk);
    }

    return chunk;
}

void STREAM_release(int handle, STREAMCHUNKHDR *chunk) {
    STREAMHEADERtag *strm;
    TAPSTRUCTtag *tap;
    STREAMSTATE streamstate;
    int lockstate;
    if (validatehandle(handle, &strm, &tap) == 0 &&
        chunk >= reinterpret_cast<STREAMCHUNKHDR *>(strm->bufferstart) &&
        chunk <= reinterpret_cast<STREAMCHUNKHDR *>(strm->bufferend - 8) &&
        static_cast<int>(little_get(&chunk->type, 4)) != -2) {
        little_put(&chunk->type, -2, 4);
        decbufferusage(strm, little_get(&chunk->size, 4));
        MUTEX_lock(&strm->mutex);
        streamstate = strm->state;
        if (streamstate == STREAM_STOPPED_STATE) {
            strm->state = STREAM_RUNNING_STATE;
        }
        MUTEX_unlock(&strm->mutex);

        if (streamstate == STREAM_STOPPED_STATE) {
            if (strm->greedystate != 0) {
                restartstream(strm, strm->priorityhigh);
            } else {
                restartstream(strm, strm->prioritylow);
            }
        }
    }
}

int STREAM_gettable(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return 0;
    }
    return tapRaw->gettable;
}

int STREAM_state(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return STREAM_IDLE_STATE;
    }
    return streamRaw->state;
}

int STREAM_isendofstream(int sndstreamhandle) {
    STREAMHEADERtag *strm;
    TAPSTRUCTtag *tap;
    int status = validatehandle(sndstreamhandle, &strm, &tap);
    if (status != 0) {
        return 0;
    }
    int isEnd = 0;
    if (strm->state == STREAM_IDLE_STATE) {
        isEnd = tap->gettable == 0;
    }
    return isEnd;
}

int STREAM_buffersize(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return 0;
    }

    return static_cast<int>(streamRaw->bufferend - streamRaw->actualbufferstart);
}
