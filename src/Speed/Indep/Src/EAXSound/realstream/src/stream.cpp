#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Libs/realcore/include/common/realcore/system.h"

struct STREAMCHUNKHDR {
    int type;
    int size;
};

struct REQUESTSTRUCTtag;
struct TAPSTRUCTtag;
struct FILTERSTRUCTtag;
struct STREAMHEADERtag;

// total size: 0x124
typedef struct REQUESTSTRUCTtag {
    int id;
    int state;
    REQUESTSTRUCTtag *prev;
    REQUESTSTRUCTtag *next;
    int type;
    char fname[255];
    char *address;
    int parm;
    int endchunkid;
    char *datastart;
} REQUESTSTRUCT;

// total size: 0x10
typedef struct TAPSTRUCTtag {
    STREAMHEADERtag *stream;
    int tapnum;
    int gettable;
    char *getptr;
} TAPSTRUCT;

// total size: 0xC
typedef struct FILTERSTRUCTtag {
    int mask;
    int value;
    int tapnum;
} FILTERSTRUCT;

// total size: 0x18C
typedef struct STREAMHEADERtag {
    int id;
    MUTEX mutex;
    REQUESTSTRUCT *request;
    int requests;
    FILTERSTRUCT *filter;
    int filters;
    TAPSTRUCT *tap;
    int taps;
    char *actualbufferstart;
    char *bufferstart;
    char *bufferend;
    int state;
    int prioritylow;
    int priorityhigh;
    int greedylevel;
    int greedystate;
    int bufferusage;
    char *datastart;
    char *datatail;
    char *dataend;
    REQUESTSTRUCT *firstreq;
    REQUESTSTRUCT *curreq;
    REQUESTSTRUCT *lastreq;
    REQUESTSTRUCT *freereq;
    char fname[255];
    int fhandle;
    int foffset;
    int fop;
    int readsize;
    int readblocksize;
} STREAMHEADER;

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

typedef char StreamRequestSizeCheck[(sizeof(REQUESTSTRUCT) == 0x124) ? 1 : -1];
typedef char StreamTapSizeCheck[(sizeof(TAPSTRUCT) == 0x10) ? 1 : -1];
typedef char StreamFilterSizeCheck[(sizeof(FILTERSTRUCT) == 0xC) ? 1 : -1];
typedef char StreamHeaderSizeCheck[(sizeof(STREAMHEADER) == 0x18C) ? 1 : -1];

inline int ReadChunkWord(const int *field) {
    const unsigned char *bytes = static_cast<const unsigned char *>(static_cast<const void *>(field));
    return (static_cast<int>(bytes[3]) << 24) | (static_cast<int>(bytes[2]) << 16) | (static_cast<int>(bytes[1]) << 8) |
           static_cast<int>(bytes[0]);
}

inline void WriteChunkWord(int *field, int value) {
    unsigned char *bytes = static_cast<unsigned char *>(static_cast<void *>(field));
    bytes[0] = static_cast<unsigned char>(value);
    bytes[1] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 8);
    bytes[2] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 16);
    bytes[3] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 24);
}

struct FILEOPERATION;

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

int inbetween(char *startptr, char *endptr, char *ptr) {
    if (startptr <= endptr) {
        if (ptr < startptr) {
            return 0;
        }
        return ptr < endptr;
    }
    if (ptr < startptr) {
        if (endptr <= ptr) {
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

REQUESTSTRUCTtag *getfreerequest(STREAMHEADERtag *stream) {
    STREAMHEADER *header = reinterpret_cast<STREAMHEADER *>(stream);
    REQUESTSTRUCT *request = nullptr;

    MUTEX_lock(&header->mutex);
    request = header->freereq;
    if (request != nullptr) {
        requestidcounter += 0x100;
        header->freereq = request->next;
        if (requestidcounter == 0) {
            requestidcounter = 0x100;
        }
        request->id = (request->id & 0xFF) | requestidcounter;
    }
    MUTEX_unlock(&header->mutex);

    return reinterpret_cast<REQUESTSTRUCTtag *>(request);
}

static void queuerequest(STREAMHEADERtag *strm, REQUESTSTRUCTtag *req) {
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

REQUESTSTRUCTtag *locaterequest(STREAMHEADERtag *stream, int requesthandle) {
    STREAMHEADER *header = reinterpret_cast<STREAMHEADER *>(stream);
    unsigned int requestIndex = static_cast<unsigned int>(requesthandle) & 0xFF;
    if (static_cast<int>(requestIndex) >= header->requests) {
        return nullptr;
    }

    REQUESTSTRUCT *request = header->request + requestIndex;
    if (requesthandle == request->id) {
        if (request->state == STREAMREQUEST_FREE) {
            return nullptr;
        }
        return reinterpret_cast<REQUESTSTRUCTtag *>(request);
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

int filterchunk(STREAMHEADERtag *stream, STREAMCHUNKHDR *chunk) {
    int chunktype = ReadChunkWord(&chunk->type);
    int i = 0;

    if (i < stream->filters) {
        FILTERSTRUCTtag *filt = stream->filter;
        int filters = stream->filters;

        do {
            if ((chunktype & filt[i].mask) == filt[i].value) {
                return filt[i].tapnum;
            }
            i++;
        } while (i < filters);
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
        if (reinterpret_cast<unsigned char *>(&chunk->size)[3] != 0) {
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
            reinterpret_cast<unsigned char *>(&chunk->size)[3] |= static_cast<unsigned char>(tapIndex);

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
            reinterpret_cast<unsigned char *>(&chunk->size)[3] |= static_cast<unsigned char>(tapIndex);
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
    if (request == nullptr) {
        noPendingRequest = 1;
    } else {
        if (request->state != STREAMREQUEST_PENDING) {
            if (request->next == nullptr) {
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
                    if (fop == nullptr) {
                        return;
                    }
                    FILESYS_callbackop(fop, reinterpret_cast<int (*)(int, int, void *)>(opencallback));
                    return;
                }
                FILEOPERATION *fop = FILESYS_close(header->fhandle, priority, stream);
                header->fop = reinterpret_cast<int>(fop);
                if (fop == nullptr) {
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
        if (req->next == nullptr || req->next->state == STREAMREQUEST_PENDING ||
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
        strm->state = 2;
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
    if (op == nullptr) {
        return;
    }

    FILESYS_callbackop(op, reinterpret_cast<int (*)(int, int, void *)>(readcallback));
    return;

stream_stop:
    strm->state = 2;
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
    if (requests > 0) {
        reinterpret_cast<REQUESTSTRUCT *>(static_cast<char *>(static_cast<void *>(strm->request)) + requests * sizeof(REQUESTSTRUCT) -
                                         sizeof(REQUESTSTRUCT))
            ->next = nullptr;
    }

    for (i = 0; i < filters; ++i) {
        strm->filter[i].mask = 0;
        strm->filter[i].tapnum = 1;
        strm->filter[i].value = 0;
    }

    i = 0;
    while (i < taps) {
        strm->tap[i].stream = static_cast<STREAMHEADERtag *>(buffer);
        strm->tap[i].gettable = 0;
        strm->tap[i].tapnum = i + 1;
        ++i;
    }

    AssignAudioStreamHandle(reinterpret_cast<unsigned int>(strm->tap));
    return reinterpret_cast<int>(strm->tap);
}

void STREAM_setfilter(int sndstreamhandle, int filternum, int mask, int value, int tapnum) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status != 0) {
        return;
    }

    if (filternum <= 0 || filternum > streamRaw->filters) {
        return;
    }
    if (filternum == streamRaw->filters && (mask != 0 || value != 0)) {
        return;
    }

    int taps = streamRaw->taps;
    if (tapnum < 1 && tapnum != -1 && tapnum != -2) {
        return;
    }
    if (tapnum > taps) {
        return;
    }
    if (streamRaw->state != STREAM_IDLE_STATE) {
        return;
    }

    FILTERSTRUCT *filter =
        static_cast<FILTERSTRUCT *>(static_cast<void *>(streamRaw->filter)) + (filternum - 1);
    filter->tapnum = tapnum;
    filter->mask = mask;
    filter->value = value;
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

void STREAM_setgreedylevel(int sndstreamhandle, int greedylevel) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        int bufferUsage = streamRaw->bufferusage;
        int oldGreedyLevel = streamRaw->greedylevel;
        streamRaw->greedylevel = greedylevel;
        if ((bufferUsage < oldGreedyLevel) != (bufferUsage < greedylevel)) {
            STREAM_setgreedystate(sndstreamhandle, static_cast<int>(bufferUsage < greedylevel));
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

int STREAM_taphandle(int sndstreamhandle, int tapindex) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status != 0 || tapindex <= 0) {
        return 0;
    }
    if (tapindex <= streamRaw->taps) {
        return reinterpret_cast<int>(streamRaw->tap + tapindex - 1);
    }
    return 0;
}

int STREAM_queuefile(int sndstreamhandle, const char *filename, int offset, int holdtime) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    int requestId = 0;
    if (status != 0) {
        return 0;
    }

    REQUESTSTRUCTtag *requestRaw = getfreerequest(streamRaw);
    if (requestRaw == nullptr) {
        return 0;
    }

    requestRaw->type = 0;
    strncpy(requestRaw->fname, filename, 0xFE);
    requestRaw->parm = offset;
    requestRaw->endchunkid = holdtime;
    queuerequest(streamRaw, requestRaw);

    MUTEX_lock(&streamRaw->mutex);
    bool wasIdle = streamRaw->state == STREAM_IDLE_STATE;
    if (wasIdle) {
        streamRaw->state = STREAM_RUNNING_STATE;
    }
    MUTEX_unlock(&streamRaw->mutex);

    if (wasIdle) {
        if (streamRaw->greedystate == 0) {
            startnextrequest(streamRaw, streamRaw->prioritylow);
        } else {
            startnextrequest(streamRaw, streamRaw->priorityhigh);
        }
    }

    requestId = requestRaw->id;
    return requestId;
}

int STREAM_queuemem(int sndstreamhandle, void *address, int length, int holdtime) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    int requestId = 0;
    if (status == 0) {
        REQUESTSTRUCTtag *requestRaw = getfreerequest(streamRaw);
        if (requestRaw != nullptr) {
            if (length == 0) {
                int totalLength = 0;
                STREAMCHUNKHDR *chunk = static_cast<STREAMCHUNKHDR *>(address);
                while (ReadChunkWord(&chunk->type) != holdtime) {
                    int chunkLength = ReadChunkWord(&chunk->size);
                    chunk = static_cast<STREAMCHUNKHDR *>(
                        static_cast<void *>(static_cast<char *>(static_cast<void *>(&chunk->type)) + chunkLength));
                    totalLength += chunkLength;
                }
                length = totalLength + ReadChunkWord(&chunk->size);
            }

            requestRaw->parm = length;
            requestRaw->address = static_cast<char *>(address);
            requestRaw->endchunkid = holdtime;
            requestRaw->type = 1;
            queuerequest(streamRaw, requestRaw);

            MUTEX_lock(&streamRaw->mutex);
            int streamState = streamRaw->state;
            if (streamState == STREAM_IDLE_STATE) {
                streamRaw->state = STREAM_RUNNING_STATE;
            }
            MUTEX_unlock(&streamRaw->mutex);

            if (streamState == STREAM_IDLE_STATE) {
                startnextrequest(streamRaw, 0);
            }
            requestId = requestRaw->id;
        }
    }
    return requestId;
}

void STREAM_cancelrequest(int sndstreamhandle, int requesthandle) {
    STREAMHEADERtag *strm;
    TAPSTRUCTtag *tap;
    REQUESTSTRUCTtag *req;
    STREAMCHUNKHDR *chunk;
    char *dataStart = nullptr;
    int lockstate = validatehandle(sndstreamhandle, &strm, &tap);
    char *requestStart = nullptr;
    char *requestEnd = nullptr;
    int chunksize;
    int chunktap;
    int finished;
    int i;
    int next;

    if (lockstate == 0) {
        MUTEX_lock(&strm->mutex);
        req = locaterequest(strm, requesthandle);
        if (req == nullptr || req->state == STREAMREQUEST_CANCELED) {
            finished = true;
        } else if (req->state == STREAMREQUEST_PENDING) {
            finished = true;
            freerequest(strm, req);
        } else {
            req->state = STREAMREQUEST_CANCELED;
            dataStart = strm->datastart;
            requestStart = dataStart;
            if (req != strm->firstreq) {
                requestStart = req->datastart;
            }
            req = req->next;
            if (req == nullptr || req->state == STREAMREQUEST_PENDING) {
                requestEnd = strm->datatail;
            } else {
                requestEnd = req->datastart;
            }
            finished = false;
        }
        MUTEX_unlock(&strm->mutex);
        if ((!finished) && (i = 0, i < strm->taps)) {
            do {
                next = i + 1;
                tap = strm->tap + i;
                if (tap->gettable > 0) {
                    lockstate = inbetween(dataStart, requestStart, tap->getptr);
                    if (lockstate != 0) {
                        finished = reinterpret_cast<int>(requestStart);
                        chunktap = tap->tapnum;
                        if (requestStart != requestEnd) {
                            while (reinterpret_cast<char *>(finished) != requestEnd) {
                                chunk = reinterpret_cast<STREAMCHUNKHDR *>(finished);
                                if (ReadChunkWord(&chunk->type) == -1) {
                                    finished = reinterpret_cast<int>(strm->bufferstart);
                                } else {
                                    chunksize = ReadChunkWord(&chunk->size) & 0xFFFFFF;
                                    if ((ReadChunkWord(&chunk->size) & 0xFF000000U) ==
                                        (static_cast<unsigned int>(chunktap) << 24)) {
                                        MUTEX_lock(&strm->mutex);
                                        tap->gettable -= chunksize;
                                        MUTEX_unlock(&strm->mutex);
                                        decbufferusage(strm, chunksize);
                                        WriteChunkWord(&chunk->size, chunksize);
                                        WriteChunkWord(&chunk->type, -2);
                                    }
                                    finished += chunksize;
                                }
                            }
                        }
                    } else {
                        while (true) {
                            lockstate = inbetween(requestStart, requestEnd, tap->getptr);
                            if (lockstate == 0) {
                                break;
                            }
                            chunk = STREAM_get(reinterpret_cast<int>(tap));
                            STREAM_release(reinterpret_cast<int>(tap), chunk);
                            if (tap->gettable < 1) {
                                break;
                            }
                        }
                    }
                }
                i = next;
            } while (next < strm->taps);
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
        if (request != nullptr) {
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

void STREAM_release(int sndstreamhandle, STREAMCHUNKHDR *chunk) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status != 0) {
        return;
    }

    STREAMCHUNKHDR *bufferStart = reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->bufferstart);
    STREAMCHUNKHDR *bufferEnd = reinterpret_cast<STREAMCHUNKHDR *>(streamRaw->bufferend - 8);
    if (!(bufferStart <= chunk && chunk <= bufferEnd && ReadChunkWord(&chunk->type) != -2)) {
        return;
    }

    WriteChunkWord(&chunk->type, -2);
    decbufferusage(streamRaw, ReadChunkWord(&chunk->size));
    MUTEX_lock(&streamRaw->mutex);
    int streamState = streamRaw->state;
    if (streamState == STREAM_STOPPED_STATE) {
        streamRaw->state = STREAM_RUNNING_STATE;
    }
    MUTEX_unlock(&streamRaw->mutex);

    if (streamState == STREAM_STOPPED_STATE) {
        if (streamRaw->greedystate == 0) {
            restartstream(streamRaw, streamRaw->prioritylow);
        } else {
            restartstream(streamRaw, streamRaw->priorityhigh);
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
