#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Libs/realcore/include/common/realcore/system.h"

struct STREAMCHUNKHDR {
    int type;
    int size;
};

struct REQUESTSTRUCTtag {
    int handle;
    int state;
    int hold;
    REQUESTSTRUCTtag *next;
};

struct TAPSTRUCTtag {
    void *owner;
    int index;
    int used;
    int pad;
};

struct STREAMHEADERtag {
    int magic;
    int state;
    int priority;
    int greedylevel;
    int greedystate;
    int eof;
    int buffersize;
    int maxrequests;
    int maxfilters;
    int maxtaps;
    int currentrequest;
    int outstandingrequests;
    REQUESTSTRUCTtag requests[256];
    TAPSTRUCTtag taps[16];
};

namespace {

// total size: 0x124
struct StreamRequestView {
    int id;
    int state;
    StreamRequestView *prev;
    StreamRequestView *next;
    int type;
    char fname[255];
    char *address;
    int parm;
    int endchunkid;
    char *datastart;
};

// total size: 0x10
struct StreamTapView {
    STREAMHEADERtag *stream;
    int tapnum;
    int gettable;
    char *getptr;
};

// total size: 0xC
struct StreamFilterView {
    int mask;
    int value;
    int tapnum;
};

// total size: 0x18C
struct StreamHeaderView {
    int id;
    MUTEX mutex;
    StreamRequestView *request;
    int requests;
    void *filter;
    int filters;
    StreamTapView *tap;
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
    StreamRequestView *firstreq;
    StreamRequestView *curreq;
    StreamRequestView *lastreq;
    StreamRequestView *freereq;
    char fname[255];
    int fhandle;
    int foffset;
    int fop;
    int readsize;
    int readblocksize;
};

enum StreamRequestState {
    STREAMREQUEST_FREE = 0,
    STREAMREQUEST_PENDING = 1,
    STREAMREQUEST_EXECUTING = 2,
    STREAMREQUEST_COMPLETED = 3,
    STREAMREQUEST_CANCELED = 4,
};

enum StreamState {
    STREAM_IDLE_STATE = 0,
    STREAM_RUNNING_STATE = 1,
    STREAM_STOPPED_STATE = 2,
};

typedef char StreamRequestViewSizeCheck[(sizeof(StreamRequestView) == 0x124) ? 1 : -1];
typedef char StreamTapViewSizeCheck[(sizeof(StreamTapView) == 0x10) ? 1 : -1];
typedef char StreamFilterViewSizeCheck[(sizeof(StreamFilterView) == 0xC) ? 1 : -1];
typedef char StreamHeaderViewSizeCheck[(sizeof(StreamHeaderView) == 0x18C) ? 1 : -1];

inline int ReadChunkWord(const int *field) {
    const unsigned char *bytes = reinterpret_cast<const unsigned char *>(field);
    return (static_cast<int>(bytes[3]) << 24) | (static_cast<int>(bytes[2]) << 16) | (static_cast<int>(bytes[1]) << 8) |
           static_cast<int>(bytes[0]);
}

inline void WriteChunkWord(int *field, int value) {
    unsigned char *bytes = reinterpret_cast<unsigned char *>(field);
    bytes[0] = static_cast<unsigned char>(value);
    bytes[1] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 8);
    bytes[2] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 16);
    bytes[3] = static_cast<unsigned char>(static_cast<unsigned int>(value) >> 24);
}

} // namespace

struct FILEOPERATION;

static STREAMHEADERtag *g_StreamTable[64];
static int g_NextStreamHandle = 1;
static int requestidcounter;

void AssignAudioStreamHandle(unsigned int realstrmhandle);
extern FILEOPERATION *FILESYS_read(int fhandle, int foffset, void *buffer, int size, int priority, void *userdata);
extern FILEOPERATION *FILESYS_open(const char *filename, unsigned int mode, int priority, void *userdata);
extern FILEOPERATION *FILESYS_close(int fhandle, int priority, void *userdata);
extern void FILESYS_callbackop(FILEOPERATION *op, int (*cb)(int, int, void *));
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
    STREAMHEADERtag *header = GetHeader(sndstreamhandle);
    if (!header) {
        if (stream != nullptr) {
            *stream = nullptr;
        }
        if (tap != nullptr) {
            *tap = nullptr;
        }
        return -1;
    }

    if (stream != nullptr) {
        *stream = header;
    }
    if (tap != nullptr) {
        *tap = &header->taps[0];
    }
    return 0;
}

int inbetween(char *value, char *low, char *high) {
    return value >= low && value < high;
}

int decbufferusage(STREAMHEADERtag *stream, int amount) {
    if (!stream) {
        return -1;
    }

    stream->buffersize -= amount;
    if (stream->buffersize < 0) {
        stream->buffersize = 0;
    }
    return stream->buffersize;
}

REQUESTSTRUCTtag *getfreerequest(STREAMHEADERtag *stream) {
    StreamHeaderView *header = reinterpret_cast<StreamHeaderView *>(stream);
    StreamRequestView *request = nullptr;

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
    StreamHeaderView *header = reinterpret_cast<StreamHeaderView *>(stream);
    StreamRequestView *request = reinterpret_cast<StreamRequestView *>(reqRaw);

    request->state = STREAMREQUEST_PENDING;
    request->next = nullptr;
    MUTEX_lock(&header->mutex);
    if (header->lastreq == nullptr) {
        request->prev = nullptr;
        header->lastreq = request;
        header->firstreq = request;
        header->curreq = request;
    } else {
        request->prev = header->lastreq;
        header->lastreq->next = request;
        header->lastreq = request;
    }
    MUTEX_unlock(&header->mutex);
}

REQUESTSTRUCTtag *locaterequest(STREAMHEADERtag *stream, int requesthandle) {
    StreamHeaderView *header = reinterpret_cast<StreamHeaderView *>(stream);
    unsigned int requestIndex = static_cast<unsigned int>(requesthandle) & 0xFF;
    if (static_cast<int>(requestIndex) >= header->requests) {
        return nullptr;
    }

    StreamRequestView *request = header->request + requestIndex;
    if (requesthandle == request->id) {
        if (request->state == STREAMREQUEST_FREE) {
            return nullptr;
        }
        return reinterpret_cast<REQUESTSTRUCTtag *>(request);
    }
    return nullptr;
}

void freerequest(STREAMHEADERtag *stream, REQUESTSTRUCTtag *reqRaw) {
    StreamHeaderView *header = reinterpret_cast<StreamHeaderView *>(stream);
    StreamRequestView *request = reinterpret_cast<StreamRequestView *>(reqRaw);
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
        StreamRequestView *current = request->next;
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
    (void)stream;
    return (chunk != nullptr);
}

int parsechunks(STREAMHEADERtag *stream) {
    if (!stream) {
        return -1;
    }

    StreamHeaderView *strm = reinterpret_cast<StreamHeaderView *>(stream);
    STREAMCHUNKHDR *chunk = reinterpret_cast<STREAMCHUNKHDR *>(strm->datatail);
    StreamRequestView *req = strm->curreq;
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
                StreamTapView *tap = strm->tap;
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

int opencallback(int sndstreamhandle, int status, void *userdata) {
    (void)status;
    (void)userdata;
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    return parsechunks(stream);
}

int closecallback(int sndstreamhandle, int status, void *userdata) {
    (void)status;
    (void)userdata;
    return (GetHeader(sndstreamhandle) != nullptr) ? 0 : -1;
}

int readcallback(int sndstreamhandle, int status, void *userdata) {
    (void)status;
    (void)userdata;
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    return parsechunks(stream);
}

void startnextrequest(STREAMHEADERtag *stream, int priority) {
    StreamHeaderView *header = reinterpret_cast<StreamHeaderView *>(stream);
    StreamRequestView *request = nullptr;
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
                    FILESYS_callbackop(fop, opencallback);
                    return;
                }
                FILEOPERATION *fop = FILESYS_close(header->fhandle, priority, stream);
                header->fop = reinterpret_cast<int>(fop);
                if (fop == nullptr) {
                    return;
                }
                FILESYS_callbackop(fop, closecallback);
                return;
            }
        }
        restartstream(stream, priority);
    }
}

void restartstream(STREAMHEADERtag *stream, int priority) {
    char *dataStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x5C);
    char *dataTail = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x60);

    while (dataStart != dataTail) {
        unsigned char *head = reinterpret_cast<unsigned char *>(dataStart);
        int marker = (head[3] << 24) | (head[2] << 16) | (head[1] << 8) | head[0];
        if (marker == -1) {
            dataStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x3C);
        } else {
            if (marker != -2) {
                break;
            }
            int skip = (head[7] << 24) | (head[6] << 16) | (head[5] << 8) | head[4];
            dataStart += skip;
        }
        *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x5C) = dataStart;
    }

    RealSystem::Mutex *mutex = reinterpret_cast<RealSystem::Mutex *>(reinterpret_cast<char *>(stream) + 0x4);
    mutex->Lock();
    while (true) {
        REQUESTSTRUCTtag *req = *reinterpret_cast<REQUESTSTRUCTtag **>(reinterpret_cast<char *>(stream) + 0x68);
        REQUESTSTRUCTtag *next = req != nullptr ? *reinterpret_cast<REQUESTSTRUCTtag **>(reinterpret_cast<char *>(req) + 0x0C) : nullptr;
        if (next == nullptr) {
            break;
        }

        int nextState = *reinterpret_cast<int *>(reinterpret_cast<char *>(next) + 0x4);
        if (nextState != 1) {
            char *nextDataStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(next) + 0x120);
            char *dataEnd = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x64);
            if (!inbetween(dataStart, dataEnd, nextDataStart - 1)) {
                freerequest(stream, req);
                continue;
            }
        }
        break;
    }
    mutex->Unlock();

    dataStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x5C);
    char *dataEnd = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x64);
    unsigned char *bytesAvailable;
    int readBlockSize = *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x188);
    if (dataEnd < dataStart) {
        bytesAvailable = reinterpret_cast<unsigned char *>(dataStart - dataEnd - 0x21);
    } else {
        char *bufferEnd = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x40);
        bytesAvailable = reinterpret_cast<unsigned char *>(bufferEnd - dataEnd - 0x20);
        if (reinterpret_cast<int>(bytesAvailable) < readBlockSize) {
            char *dataTailNow = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x60);
            int count = dataEnd - dataTailNow;
            REQUESTSTRUCTtag *curReq = *reinterpret_cast<REQUESTSTRUCTtag **>(reinterpret_cast<char *>(stream) + 0x6C);
            int reqType = *reinterpret_cast<int *>(reinterpret_cast<char *>(curReq) + 0x10);
            char *bufferStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x3C);

            if (reqType == 1) {
                if ((dataStart - bufferStart) < (count + 1)) {
                    goto stream_stop;
                }
            } else if ((dataStart - bufferStart - 0x20) < (count + 1)) {
                goto stream_stop;
            }

            char *actualBufferStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x38);
            if ((count & 0x1F) == 0 || reqType == 1) {
                bufferStart = actualBufferStart;
            } else {
                bufferStart = actualBufferStart - ((count % 0x20) - 0x20);
            }

            *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x3C) = bufferStart;
            MEM_copy(bufferStart, dataTailNow, count);

            dataTailNow[7] = '\0';
            dataTailNow[3] = static_cast<char>(-1);
            dataTailNow[4] = '\b';
            dataTailNow[0] = static_cast<char>(-1);
            dataTailNow[1] = static_cast<char>(-1);
            dataTailNow[2] = static_cast<char>(-1);
            dataTailNow[5] = '\0';
            dataTailNow[6] = '\0';

            char *newDataEnd = bufferStart + count;
            *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x60) = bufferStart;
            *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x64) = newDataEnd;

            unsigned char *check = reinterpret_cast<unsigned char *>(dataStart);
            int marker = (check[3] << 24) | (check[2] << 16) | (check[1] << 8) | check[0];
            if (marker == -1) {
                *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x5C) = bufferStart;
                char *bufferEnd2 = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x40);
                bytesAvailable = reinterpret_cast<unsigned char *>(bufferEnd2 - newDataEnd - 0x20);
            } else {
                bytesAvailable = reinterpret_cast<unsigned char *>(dataStart - newDataEnd - 1);
            }
        }
    }

    if (*reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x54) == 0 &&
        *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x4C) < priority &&
        IsWorldDataStreaming(*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(stream) + 0x30))) {
        bGetTickerDifference(utickreadcallback);
        gbWorldDataBlocksAudioRead = true;
        *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x44) = 2;
        uTicksSinceLastAudioReadBailed = bGetTicker();
        return;
    }

    if (IsWorldDataStreaming(*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(stream) + 0x178))) {
        gbAudioInterruptsWorldDataRead = true;
    } else {
        gbAudioInterruptsWorldDataRead = false;
    }
    gbWorldDataBlocksAudioRead = false;

    if (reinterpret_cast<int>(bytesAvailable) >= readBlockSize) {
        REQUESTSTRUCTtag *curReq = *reinterpret_cast<REQUESTSTRUCTtag **>(reinterpret_cast<char *>(stream) + 0x6C);
        int reqType = *reinterpret_cast<int *>(reinterpret_cast<char *>(curReq) + 0x10);
        if (reqType == 1) {
            int reqParm = *reinterpret_cast<int *>(reinterpret_cast<char *>(curReq) + 0x118);
            int foffset = *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x17C);
            if (reqParm < reinterpret_cast<int>(bytesAvailable) + foffset) {
                *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x184) = reqParm - foffset;
            } else {
                *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x184) = reinterpret_cast<int>(bytesAvailable);
            }

            int readSize = *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x184);
            char *dataEndNow = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x64);
            char *address = *reinterpret_cast<char **>(reinterpret_cast<char *>(curReq) + 0x114);
            MEM_copy(dataEndNow, address, readSize);
            *reinterpret_cast<char **>(reinterpret_cast<char *>(curReq) + 0x114) = address + readSize;
            readcallback(0, 0, stream);
            return;
        }

        if (readBlockSize < reinterpret_cast<int>(bytesAvailable)) {
            *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x184) =
                (reinterpret_cast<int>(bytesAvailable) / readBlockSize) * readBlockSize;
        } else {
            *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x184) = readBlockSize;
        }

        bReadCallbackToggle = false;
        FILEOPERATION *op = FILESYS_read(*reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x178),
                                         *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x17C),
                                         *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x64),
                                         *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x184), priority, stream);
        *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x180) = reinterpret_cast<int>(op);
        if (op == nullptr) {
            return;
        }

        FILESYS_callbackop(op, readcallback);
        return;
    }

stream_stop:
    *reinterpret_cast<int *>(reinterpret_cast<char *>(stream) + 0x44) = 2;
}

int STREAM_create(int requests, int filters, int taps, void *buffer, int size) {
    int freeBytes = size - STREAM_overhead(requests, filters, taps);
    if (freeBytes < 0x1800 || requests < 2 || requests > 0x100 || filters < 1 || filters > 0x10 || taps < 1 ||
        filters < taps) {
        return 0;
    }

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(buffer);
    stream->id = 0x4D525453;
    MUTEX_create(&stream->mutex);

    StreamRequestView *requestBase =
        reinterpret_cast<StreamRequestView *>(reinterpret_cast<char *>(buffer) + sizeof(StreamHeaderView));
    StreamFilterView *filterBase = reinterpret_cast<StreamFilterView *>(requestBase + requests);
    StreamTapView *tapBase = reinterpret_cast<StreamTapView *>(reinterpret_cast<char *>(filterBase) + filters * sizeof(StreamFilterView));

    char *dataBase =
        reinterpret_cast<char *>((reinterpret_cast<unsigned int>(reinterpret_cast<char *>(tapBase) + taps * sizeof(StreamTapView)) &
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
    stream->bufferend = reinterpret_cast<char *>(buffer) + size;
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
        StreamRequestView *request = reinterpret_cast<StreamRequestView *>(reinterpret_cast<char *>(requestBase) + i * sizeof(StreamRequestView));
        request->id = i;
        request->state = STREAMREQUEST_FREE;
        request->next = reinterpret_cast<StreamRequestView *>(reinterpret_cast<char *>(request) + sizeof(StreamRequestView));
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
        tapBase[i].stream = reinterpret_cast<STREAMHEADERtag *>(buffer);
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

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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

    StreamFilterView *filter = reinterpret_cast<StreamFilterView *>(stream->filter) + (filternum - 1);
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

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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
        StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
        stream->prioritylow = prioritylow;
        stream->priorityhigh = priorityhigh;
    }
}

void STREAM_setgreedylevel(int sndstreamhandle, int greedylevel) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    if (status == 0) {
        StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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
        StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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
    StreamTapView *tap = nullptr;
    if (status == 0 && tapindex > 0) {
        StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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

    StreamRequestView *request = reinterpret_cast<StreamRequestView *>(requestRaw);
    request->type = 0;
    strncpy(request->fname, filename, 0xFE);
    request->parm = offset;
    request->endchunkid = holdtime;
    queuerequest(streamRaw, requestRaw);

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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
                STREAMCHUNKHDR *chunk = reinterpret_cast<STREAMCHUNKHDR *>(address);
                while (ReadChunkWord(&chunk->type) != holdtime) {
                    int chunkLength = ReadChunkWord(&chunk->size);
                    chunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(&chunk->type) + chunkLength);
                    totalLength += chunkLength;
                }
                length = totalLength + ReadChunkWord(&chunk->size);
            }

            StreamRequestView *request = reinterpret_cast<StreamRequestView *>(requestRaw);
            request->parm = length;
            request->address = reinterpret_cast<char *>(address);
            request->endchunkid = holdtime;
            request->type = 1;
            queuerequest(streamRaw, requestRaw);

            StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    StreamHeaderView *stream;
    StreamTapView *tap;
    StreamRequestView *request;
    StreamRequestView *nextRequest;
    STREAMCHUNKHDR *chunk;
    unsigned int amount;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    char *dataStart = nullptr;
    char *requestStart = nullptr;
    char *requestEnd = nullptr;
    if (status == 0) {
        stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
        MUTEX_lock(&stream->mutex);
        request = reinterpret_cast<StreamRequestView *>(locaterequest(streamRaw, requesthandle));
        bool finished;
        if (request == nullptr || request->state == STREAMREQUEST_CANCELED) {
            finished = true;
        } else if (request->state == STREAMREQUEST_PENDING) {
            finished = true;
            freerequest(streamRaw, reinterpret_cast<REQUESTSTRUCTtag *>(request));
        } else {
            request->state = STREAMREQUEST_CANCELED;
            dataStart = stream->datastart;
            requestStart = dataStart;
            if (request != stream->firstreq) {
                requestStart = request->datastart;
            }
            nextRequest = request->next;
            if (nextRequest == nullptr || nextRequest->state == STREAMREQUEST_PENDING) {
                requestEnd = stream->datatail;
            } else {
                requestEnd = nextRequest->datastart;
            }
            finished = false;
        }
        MUTEX_unlock(&stream->mutex);
        if ((!finished) && (status = 0, stream->taps > 0)) {
            do {
                tap = stream->tap + status;
                if (tap->gettable > 0) {
                    int inRange = inbetween(dataStart, requestStart, tap->getptr);
                    if (inRange == 0) {
                        inRange = inbetween(requestStart, requestEnd, tap->getptr);
                        while (inRange != 0) {
                            chunk = reinterpret_cast<STREAMCHUNKHDR *>(STREAM_get(reinterpret_cast<int>(tap)));
                            STREAM_release(reinterpret_cast<int>(tap), chunk);
                            if (tap->gettable < 1) {
                                break;
                            }
                            inRange = inbetween(requestStart, requestEnd, tap->getptr);
                        }
                    } else {
                        int tapNumber = tap->tapnum;
                        if (requestStart != requestEnd) {
                            int *cursor = reinterpret_cast<int *>(requestStart);
                            int type = *cursor;
                            while (true) {
                                if (type == -1) {
                                    cursor = reinterpret_cast<int *>(stream->bufferstart);
                                } else {
                                    amount = static_cast<unsigned int>(cursor[1]) & 0xFFFFFF;
                                    if ((static_cast<unsigned int>(cursor[1]) & 0xFF000000U) ==
                                        (static_cast<unsigned int>(tapNumber) << 24)) {
                                        MUTEX_lock(&stream->mutex);
                                        tap->gettable -= static_cast<int>(amount);
                                        MUTEX_unlock(&stream->mutex);
                                        decbufferusage(streamRaw, static_cast<int>(amount));
                                        cursor[1] = static_cast<int>(amount);
                                        cursor[0] = -2;
                                        cursor = reinterpret_cast<int *>(reinterpret_cast<char *>(cursor) + amount);
                                    } else {
                                        cursor = reinterpret_cast<int *>(reinterpret_cast<char *>(cursor) + amount);
                                    }
                                }
                                if (reinterpret_cast<char *>(cursor) == requestEnd) {
                                    break;
                                }
                                type = *cursor;
                            }
                        }
                    }
                }
                status += 1;
                if (stream->taps <= status) {
                    return;
                }
            } while (true);
        }
    }
}

void STREAM_kill(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    int chunkSize = 0;
    if (status == 0) {
        StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
        StreamRequestView *request = stream->lastreq;
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
                        chunk = reinterpret_cast<STREAMCHUNKHDR *>(reinterpret_cast<char *>(&chunk->type) + chunkSize);
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

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
    StreamTapView *tap = reinterpret_cast<StreamTapView *>(tapRaw);
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
        int *nextChunk = reinterpret_cast<int *>(reinterpret_cast<char *>(&chunk->type) + amount);
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
                    nextChunk = reinterpret_cast<int *>(reinterpret_cast<char *>(nextChunk) + (encodedSize & 0xFFFFFF));
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

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
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
    return reinterpret_cast<StreamTapView *>(tapRaw)->gettable;
}

int STREAM_state(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    if (validatehandle(sndstreamhandle, &streamRaw, &tapRaw) != 0) {
        return STREAM_IDLE_STATE;
    }
    return reinterpret_cast<StreamHeaderView *>(streamRaw)->state;
}

int STREAM_isendofstream(int sndstreamhandle) {
    STREAMHEADERtag *streamRaw;
    TAPSTRUCTtag *tapRaw;
    int status = validatehandle(sndstreamhandle, &streamRaw, &tapRaw);
    bool isEnd = false;
    if (status == 0) {
        isEnd = false;
        if (reinterpret_cast<StreamHeaderView *>(streamRaw)->state == STREAM_IDLE_STATE) {
            isEnd = reinterpret_cast<StreamTapView *>(tapRaw)->gettable == 0;
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

    StreamHeaderView *stream = reinterpret_cast<StreamHeaderView *>(streamRaw);
    return static_cast<int>(stream->bufferend - stream->actualbufferstart);
}
