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

int validatehandle(int sndstreamhandle, STREAMHEADERtag **stream, TAPSTRUCTtag **tap) {
    if (sndstreamhandle == 0) {
        return 1;
    }

    STREAMHEADERtag *strm = *reinterpret_cast<STREAMHEADERtag **>(sndstreamhandle);
    if (strm->id != 0x4D525453) {
        return 1;
    }
    *tap = reinterpret_cast<TAPSTRUCTtag *>(sndstreamhandle);
    *stream = strm;
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

void decbufferusage(STREAMHEADERtag *stream, int amount) {
    int oldbufferusage;
    int bufferusage;

    MUTEX_lock(&stream->mutex);
    oldbufferusage = stream->bufferusage;
    bufferusage = oldbufferusage - amount;
    stream->bufferusage = bufferusage;
    MUTEX_unlock(&stream->mutex);

    if (oldbufferusage >= stream->greedylevel && bufferusage < stream->greedylevel &&
        (stream->greedystate = 1, stream->state == STREAM_RUNNING_STATE)) {
        FILESYS_priorityop(stream->fop, stream->priorityhigh);
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

void queuerequest(STREAMHEADERtag *stream, REQUESTSTRUCTtag *reqRaw) {
    STREAMHEADER *header = reinterpret_cast<STREAMHEADER *>(stream);
    REQUESTSTRUCT *request = reinterpret_cast<REQUESTSTRUCT *>(reqRaw);

    request->state = STREAMREQUEST_PENDING;
    request->next = nullptr;
    MUTEX_lock(&header->mutex);
    if (header->lastreq == nullptr) {
        request->prev = nullptr;
        header->firstreq = request;
        header->curreq = request;
        header->lastreq = request;
    } else {
        request->prev = header->lastreq;
        header->lastreq->next = request;
        header->lastreq = request;
    }
    MUTEX_unlock(&header->mutex);
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

void freerequest(STREAMHEADERtag *stream, REQUESTSTRUCTtag *reqRaw) {
    STREAMHEADER *header = reinterpret_cast<STREAMHEADER *>(stream);
    REQUESTSTRUCT *request = reinterpret_cast<REQUESTSTRUCT *>(reqRaw);
    if (request == header->firstreq) {
        header->firstreq = request->next;
    } else {
        request->prev->next = request->next;
    }

    if (request == header->lastreq) {
        header->lastreq = request->prev;
    } else {
        request->next->prev = request->prev;
    }

    if (request == header->curreq) {
        REQUESTSTRUCT *current = request->next;
        if (current == nullptr) {
            current = request->prev;
        }
        header->curreq = current;
    }

    request->state = STREAMREQUEST_FREE;
    request->next = header->freereq;
    header->freereq = request;
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
    STREAMHEADERtag *strm = static_cast<STREAMHEADERtag *>(userdata);
    REQUESTSTRUCTtag *req = strm->curreq;
    int lockstate;
    int bytesread;
    int endoffile;
    int endchunk;

    *(int *)&bReadCallbackToggle = 1;
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

int STREAM_create(int requests, int filters, int taps, void *buffer, int size) {
    int freeBytes = size - STREAM_overhead(requests, filters, taps);
    if (freeBytes < 0x1800 || requests < 2 || requests > 0x100 || filters < 1 || filters > 0x10 || taps < 1 ||
        filters < taps) {
        return 0;
    }

    STREAMHEADER *stream = static_cast<STREAMHEADER *>(buffer);
    stream->id = 0x4D525453;
    MUTEX_create(&stream->mutex);

    REQUESTSTRUCT *requestBase = static_cast<REQUESTSTRUCT *>(
        static_cast<void *>(static_cast<char *>(buffer) + sizeof(STREAMHEADER)));
    FILTERSTRUCT *filterBase = reinterpret_cast<FILTERSTRUCT *>(requestBase + requests);
    TAPSTRUCT *tapBase = static_cast<TAPSTRUCT *>(
        static_cast<void *>(static_cast<char *>(static_cast<void *>(filterBase)) + filters * sizeof(FILTERSTRUCT)));

    char *dataBase =
        reinterpret_cast<char *>((reinterpret_cast<unsigned int>(reinterpret_cast<char *>(tapBase) + taps * sizeof(TAPSTRUCT)) &
                                  0xFFFFFFE0U) +
                                 0x20);

    stream->request = requestBase;
    stream->requests = requests;
    stream->filter = filterBase;
    stream->filters = filters;
    stream->tap = tapBase;
    stream->taps = taps;
    stream->actualbufferstart = dataBase;
    stream->bufferstart = dataBase;
    stream->bufferend = static_cast<char *>(buffer) + size;
    stream->state = STREAM_IDLE_STATE;
    stream->prioritylow = 0x96;
    stream->priorityhigh = 0x32;
    stream->greedylevel = 0;
    stream->greedystate = 0;
    stream->bufferusage = 0;
    stream->datastart = dataBase;
    stream->datatail = dataBase;
    stream->dataend = dataBase;
    stream->firstreq = nullptr;
    stream->curreq = nullptr;
    stream->lastreq = nullptr;
    stream->freereq = requestBase;

    MEM_clear(stream->fname, 0xFF);
    stream->fhandle = 0;
    stream->foffset = 0;
    stream->fop = 0;
    stream->readsize = 0;

    if (freeBytes < 0x4000) {
        stream->readblocksize = 0x800;
    } else if (freeBytes <= 0x7FFF) {
        stream->readblocksize = 0x1000;
    } else if (freeBytes <= 0xFFFF) {
        stream->readblocksize = 0x2000;
    } else if (freeBytes <= 0x17FFF) {
        stream->readblocksize = 0x4000;
    } else {
        stream->readblocksize = 0x8000;
    }

    for (int i = 0; i < requests; ++i) {
        REQUESTSTRUCT *request = static_cast<REQUESTSTRUCT *>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(requestBase)) + i * sizeof(REQUESTSTRUCT)));
        request->id = i;
        request->state = STREAMREQUEST_FREE;
        request->next = static_cast<REQUESTSTRUCT *>(
            static_cast<void *>(static_cast<char *>(static_cast<void *>(request)) + sizeof(REQUESTSTRUCT)));
    }
    if (requests > 0) {
        requestBase[requests - 1].next = nullptr;
    }

    for (int i = 0; i < filters; ++i) {
        filterBase[i].mask = 0;
        filterBase[i].value = 0;
        filterBase[i].tapnum = 1;
    }

    for (int i = 0; i < taps; ++i) {
        tapBase[i].stream = static_cast<STREAMHEADERtag *>(buffer);
        tapBase[i].tapnum = i + 1;
        tapBase[i].gettable = 0;
    }

    AssignAudioStreamHandle(reinterpret_cast<unsigned int>(stream->tap));
    return reinterpret_cast<int>(stream->tap);
}

void STREAM_setfilter(int sndstreamhandle, int filternum, int mask, int value, int tapnum) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status != 0) {
        return;
    }

    STREAMHEADER *stream = static_cast<STREAMHEADER *>(static_cast<void *>(streamRaw));
    if (filternum <= 0 || filternum > stream->filters) {
        return;
    }
    if (filternum == stream->filters && (mask != 0 || value != 0)) {
        return;
    }

    int taps = stream->taps;
    if (tapnum < 1 && tapnum != -1 && tapnum != -2) {
        return;
    }
    if (tapnum > taps) {
        return;
    }
    if (stream->state != STREAM_IDLE_STATE) {
        return;
    }

    FILTERSTRUCT *filter =
        static_cast<FILTERSTRUCT *>(static_cast<void *>(stream->filter)) + (filternum - 1);
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

    STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
    STREAM_kill(sndstreamhandle);
    while (stream->state == STREAM_RUNNING_STATE) {
        SYNCTASK_run();
        THREAD_yield(0);
    }

    stream->id = 0;
    MUTEX_destroy(&stream->mutex);
    if (stream->fhandle != 0) {
        FILESYS_closesync(stream->fhandle, 100);
    }
}

void STREAM_setpriority(int sndstreamhandle, int prioritylow, int priorityhigh) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
        stream->prioritylow = prioritylow;
        stream->priorityhigh = priorityhigh;
    }
}

void STREAM_setgreedylevel(int sndstreamhandle, int greedylevel) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
        int bufferUsage = stream->bufferusage;
        int oldGreedyLevel = stream->greedylevel;
        stream->greedylevel = greedylevel;
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
        STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
        stream->greedystate = greedystate;
        if (greedystate != 0 && stream->state == STREAM_RUNNING_STATE) {
            FILESYS_priorityop(stream->fop, stream->priorityhigh);
        }
    }
}

int STREAM_taphandle(int sndstreamhandle, int tapindex) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    TAPSTRUCT *tap = nullptr;
    if (status == 0 && tapindex > 0) {
        STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
        if (stream->taps < tapindex) {
            tap = nullptr;
        } else {
            tap = stream->tap + tapindex - 1;
        }
    }
    return reinterpret_cast<int>(tap);
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

    REQUESTSTRUCT *request = reinterpret_cast<REQUESTSTRUCT *>(requestRaw);
    request->type = 0;
    strncpy(request->fname, filename, 0xFE);
    request->parm = offset;
    request->endchunkid = holdtime;
    queuerequest(streamRaw, requestRaw);

    STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
    MUTEX_lock(&stream->mutex);
    bool wasIdle = stream->state == STREAM_IDLE_STATE;
    if (wasIdle) {
        stream->state = STREAM_RUNNING_STATE;
    }
    MUTEX_unlock(&stream->mutex);

    if (wasIdle) {
        if (stream->greedystate == 0) {
            startnextrequest(streamRaw, stream->prioritylow);
        } else {
            startnextrequest(streamRaw, stream->priorityhigh);
        }
    }

    requestId = request->id;
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

            REQUESTSTRUCT *request = reinterpret_cast<REQUESTSTRUCT *>(requestRaw);
            request->parm = length;
            request->address = static_cast<char *>(address);
            request->endchunkid = holdtime;
            request->type = 1;
            queuerequest(streamRaw, requestRaw);

            STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
            MUTEX_lock(&stream->mutex);
            int streamState = stream->state;
            if (streamState == STREAM_IDLE_STATE) {
                stream->state = STREAM_RUNNING_STATE;
            }
            MUTEX_unlock(&stream->mutex);

            if (streamState == STREAM_IDLE_STATE) {
                startnextrequest(streamRaw, 0);
            }
            requestId = request->id;
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
                    if (lockstate == 0) {
                        while ((lockstate = inbetween(requestStart, requestEnd, tap->getptr)) != 0) {
                            chunk = STREAM_get(reinterpret_cast<int>(tap));
                            STREAM_release(reinterpret_cast<int>(tap), chunk);
                            if (tap->gettable < 1) {
                                break;
                            }
                        }
                    } else {
                        chunktap = tap->tapnum;
                        if (requestStart != requestEnd) {
                            finished = reinterpret_cast<int>(requestStart);
                            while (reinterpret_cast<char *>(finished) != requestEnd) {
                                if (ReadChunkWord(reinterpret_cast<const int *>(finished)) == -1) {
                                    finished = reinterpret_cast<int>(strm->bufferstart);
                                } else {
                                    unsigned char *bytes =
                                        static_cast<unsigned char *>(static_cast<void *>(reinterpret_cast<int *>(finished)));
                                    chunksize = ReadChunkWord(reinterpret_cast<const int *>(finished + 4)) & 0xFFFFFF;
                                    if ((ReadChunkWord(reinterpret_cast<const int *>(finished + 4)) & 0xFF000000U) ==
                                        (static_cast<unsigned int>(chunktap) << 24)) {
                                        MUTEX_lock(&strm->mutex);
                                        tap->gettable -= chunksize;
                                        MUTEX_unlock(&strm->mutex);
                                        decbufferusage(strm, chunksize);
                                        bytes[5] = static_cast<unsigned char>(static_cast<unsigned int>(chunksize) >> 8);
                                        bytes[6] = static_cast<unsigned char>(static_cast<unsigned int>(chunksize) >> 16);
                                        bytes[7] = 0;
                                        bytes[0] = 0xFE;
                                        bytes[1] = 0xFF;
                                        bytes[2] = 0xFF;
                                        bytes[3] = 0xFF;
                                        bytes[4] = static_cast<unsigned char>(chunksize);
                                    }
                                    finished += chunksize;
                                }
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
        STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
        REQUESTSTRUCT *request = stream->lastreq;
        if (request != nullptr) {
            while (request->state - STREAMREQUEST_PENDING < 2) {
                STREAM_cancelrequest(sndstreamhandle, request->id);
                request = stream->lastreq;
            }
            while (stream->firstreq != stream->curreq) {
                freerequest(streamRaw, reinterpret_cast<REQUESTSTRUCTtag *>(stream->firstreq));
            }

            int tapIndex = 0;
            stream->curreq->state = STREAMREQUEST_CANCELED;
            if (stream->taps > 0) {
                int nextTap;
                do {
                    nextTap = tapIndex + 1;
                    stream->tap[tapIndex].gettable = 0;
                    tapIndex = nextTap;
                } while (nextTap < stream->taps);
            }

            decbufferusage(streamRaw, stream->bufferusage);
            STREAMCHUNKHDR *chunk = reinterpret_cast<STREAMCHUNKHDR *>(stream->datastart);
            if (chunk != reinterpret_cast<STREAMCHUNKHDR *>(stream->datatail)) {
                do {
                    if (ReadChunkWord(&chunk->type) == -1) {
                        chunk = reinterpret_cast<STREAMCHUNKHDR *>(stream->bufferstart);
                    } else {
                        unsigned char b1 = reinterpret_cast<unsigned char *>(&chunk->size)[2];
                        unsigned char b2 = reinterpret_cast<unsigned char *>(&chunk->size)[1];
                        unsigned char b3 = reinterpret_cast<unsigned char *>(&chunk->size)[0];
                        reinterpret_cast<unsigned char *>(&chunk->type)[0] = 0xFE;
                        reinterpret_cast<unsigned char *>(&chunk->type)[1] = 0xFF;
                        chunkSize = (static_cast<int>(b1) << 16) | (static_cast<int>(b2) << 8) | static_cast<int>(b3);
                        reinterpret_cast<unsigned char *>(&chunk->type)[2] = 0xFF;
                        reinterpret_cast<unsigned char *>(&chunk->size)[1] = b2;
                        reinterpret_cast<unsigned char *>(&chunk->size)[2] = b1;
                        reinterpret_cast<unsigned char *>(&chunk->size)[3] = 0;
                        reinterpret_cast<unsigned char *>(&chunk->type)[3] = 0xFF;
                        reinterpret_cast<unsigned char *>(&chunk->size)[0] = b3;
                        chunk = static_cast<STREAMCHUNKHDR *>(
                            static_cast<void *>(static_cast<char *>(static_cast<void *>(&chunk->type)) + chunkSize));
                    }
                } while (chunk != reinterpret_cast<STREAMCHUNKHDR *>(stream->datatail));
            }

            if (stream->state == STREAM_STOPPED_STATE) {
                if (chunk == reinterpret_cast<STREAMCHUNKHDR *>(stream->bufferstart)) {
                    stream->datatail = stream->actualbufferstart;
                    stream->bufferstart = stream->datatail;
                } else {
                    int align = 0x20 - (reinterpret_cast<unsigned int>(stream->datatail) & 0x1F);
                    if (align == 0x20) {
                        align = 0;
                    }
                    char *dataTail = stream->datatail;
                    int alignedSize = chunkSize + align;
                    dataTail[7 - chunkSize] = static_cast<char>(static_cast<unsigned int>(alignedSize) >> 24);
                    dataTail[5 - chunkSize] = static_cast<char>(static_cast<unsigned int>(alignedSize) >> 8);
                    dataTail[6 - chunkSize] = static_cast<char>(static_cast<unsigned int>(alignedSize) >> 16);
                    dataTail[4 - chunkSize] = static_cast<char>(alignedSize);
                    stream->datatail = stream->datatail + align;
                }
                stream->state = STREAM_IDLE_STATE;
            }
        }
    }
}

STREAMCHUNKHDR *STREAM_get(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    STREAMCHUNKHDR *chunk = nullptr;
    if (status != 0) {
        return nullptr;
    }

    STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
    TAPSTRUCT *tap = reinterpret_cast<TAPSTRUCT *>(tapRaw);
    if (tap->gettable == 0) {
        return nullptr;
    }

    chunk = reinterpret_cast<STREAMCHUNKHDR *>(tap->getptr);
    unsigned int amount = static_cast<unsigned int>(ReadChunkWord(&chunk->size)) & 0xFFFFFF;
    WriteChunkWord(&chunk->size, static_cast<int>(amount));

    MUTEX_lock(&stream->mutex);
    int bytesLeft = tap->gettable - static_cast<int>(amount);
    tap->gettable = bytesLeft;
    MUTEX_unlock(&stream->mutex);

    if (bytesLeft > 0) {
        int *nextChunk =
            static_cast<int *>(static_cast<void *>(static_cast<char *>(static_cast<void *>(&chunk->type)) + amount));
        unsigned int encodedSize = static_cast<unsigned int>(nextChunk[1]);
        unsigned int tapMask = static_cast<unsigned int>(tap->tapnum) << 24;
        if ((encodedSize & 0xFF000000) == tapMask) {
            tap->getptr = reinterpret_cast<char *>(nextChunk);
        } else {
            int type = *nextChunk;
            while (true) {
                if (type == -1) {
                    nextChunk = reinterpret_cast<int *>(stream->bufferstart);
                } else {
                    nextChunk = static_cast<int *>(
                        static_cast<void *>(static_cast<char *>(static_cast<void *>(nextChunk)) + (encodedSize & 0xFFFFFF)));
                }
                encodedSize = static_cast<unsigned int>(nextChunk[1]);
                if ((encodedSize & 0xFF000000) == tapMask) {
                    break;
                }
                type = *nextChunk;
            }
            tap->getptr = reinterpret_cast<char *>(nextChunk);
        }
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

    STREAMHEADER *stream = reinterpret_cast<STREAMHEADER *>(streamRaw);
    STREAMCHUNKHDR *bufferStart = reinterpret_cast<STREAMCHUNKHDR *>(stream->bufferstart);
    STREAMCHUNKHDR *bufferEnd = reinterpret_cast<STREAMCHUNKHDR *>(stream->bufferend - 8);
    if (!(bufferStart <= chunk && chunk <= bufferEnd && ReadChunkWord(&chunk->type) != -2)) {
        return;
    }

    WriteChunkWord(&chunk->type, -2);
    decbufferusage(streamRaw, ReadChunkWord(&chunk->size));
    MUTEX_lock(&stream->mutex);
    int streamState = stream->state;
    if (streamState == STREAM_STOPPED_STATE) {
        stream->state = STREAM_RUNNING_STATE;
    }
    MUTEX_unlock(&stream->mutex);

    if (streamState == STREAM_STOPPED_STATE) {
        if (stream->greedystate == 0) {
            restartstream(streamRaw, stream->prioritylow);
        } else {
            restartstream(streamRaw, stream->priorityhigh);
        }
    }
}

int STREAM_gettable(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return 0;
    }
    return reinterpret_cast<TAPSTRUCT *>(tapRaw)->gettable;
}

int STREAM_state(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return STREAM_IDLE_STATE;
    }
    return reinterpret_cast<STREAMHEADER *>(streamRaw)->state;
}

int STREAM_isendofstream(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    bool isEnd = false;
    if (status == 0) {
        isEnd = false;
        if (reinterpret_cast<STREAMHEADER *>(streamRaw)->state == STREAM_IDLE_STATE) {
            isEnd = reinterpret_cast<TAPSTRUCT *>(tapRaw)->gettable == 0;
        }
    }
    return isEnd;
}

int STREAM_buffersize(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return 0;
    }

    STREAMHEADER *stream = static_cast<STREAMHEADER *>(static_cast<void *>(streamRaw));
    return static_cast<int>(stream->bufferend - stream->actualbufferstart);
}
