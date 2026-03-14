#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Libs/realcore/include/common/realcore/system.h"

struct STREAMCHUNKHDR {
    int size;
    int tag;
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

struct FILEOPERATION;

static STREAMHEADERtag *g_StreamTable[64];
static int g_NextStreamHandle = 1;

void AssignAudioStreamHandle(unsigned int realstrmhandle);
extern FILEOPERATION *FILESYS_read(int fhandle, int foffset, void *buffer, int size, int priority, void *userdata);
extern void FILESYS_callbackop(FILEOPERATION *op, int (*cb)(int, int, void *));
extern bool IsWorldDataStreaming(unsigned int strmhandle);
extern bool gbAudioInterruptsWorldDataRead;
extern bool gbWorldDataBlocksAudioRead;
extern bool bReadCallbackToggle;
extern unsigned int utickreadcallback;
extern unsigned int uTicksSinceLastAudioReadBailed;
extern float bGetTickerDifference(unsigned int start_ticks);
extern void MEM_copy(void *dest, const void *src, int bytes);

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
    if (!stream) {
        return nullptr;
    }

    for (int i = 0; i < stream->maxrequests; i++) {
        if (stream->requests[i].state == 0) {
            return &stream->requests[i];
        }
    }
    return nullptr;
}

int queuerequest(STREAMHEADERtag *stream, REQUESTSTRUCTtag *req) {
    if (!stream || !req) {
        return -1;
    }

    req->state = 1;
    req->next = nullptr;
    stream->outstandingrequests++;
    return req->handle;
}

REQUESTSTRUCTtag *locaterequest(STREAMHEADERtag *stream, int requesthandle) {
    if (!stream) {
        return nullptr;
    }

    for (int i = 0; i < stream->maxrequests; i++) {
        if (stream->requests[i].handle == requesthandle && stream->requests[i].state != 0) {
            return &stream->requests[i];
        }
    }
    return nullptr;
}

int freerequest(STREAMHEADERtag *stream, REQUESTSTRUCTtag *req) {
    if (!stream || !req) {
        return -1;
    }

    if (req->state != 0) {
        req->state = 0;
        req->hold = 0;
        if (stream->outstandingrequests > 0) {
            stream->outstandingrequests--;
        }
    }
    return 0;
}

int filterchunk(STREAMHEADERtag *stream, STREAMCHUNKHDR *chunk) {
    (void)stream;
    return (chunk != nullptr);
}

int parsechunks(STREAMHEADERtag *stream) {
    if (!stream) {
        return -1;
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

int startnextrequest(STREAMHEADERtag *stream, int flags) {
    (void)flags;
    if (!stream) {
        return -1;
    }

    for (int i = 0; i < stream->maxrequests; i++) {
        if (stream->requests[i].state == 1) {
            stream->requests[i].state = 2;
            stream->currentrequest = stream->requests[i].handle;
            stream->state = 2;
            return stream->currentrequest;
        }
    }
    stream->currentrequest = -1;
    return -1;
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
        char *nextDataStart = *reinterpret_cast<char **>(reinterpret_cast<char *>(next) + 0x120);
        char *dataEnd = *reinterpret_cast<char **>(reinterpret_cast<char *>(stream) + 0x64);
        if (nextState == 1 || inbetween(dataStart, dataEnd, nextDataStart - 1)) {
            break;
        }
        freerequest(stream, req);
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

    gbAudioInterruptsWorldDataRead = IsWorldDataStreaming(*reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(stream) + 0x178));
    gbWorldDataBlocksAudioRead = false;

    if (readBlockSize <= reinterpret_cast<int>(bytesAvailable)) {
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

    if (g_NextStreamHandle >= 64) {
        return 0;
    }

    STREAMHEADERtag *stream = new STREAMHEADERtag();
    if (!stream) {
        return 0;
    }

    stream->magic = 0x4D525453;
    stream->state = 0;
    stream->priority = 0x96;
    stream->greedylevel = 0x32;
    stream->greedystate = 0;
    stream->eof = 0;
    stream->buffersize = freeBytes;
    stream->maxrequests = requests;
    stream->maxfilters = filters;
    stream->maxtaps = taps;
    stream->currentrequest = -1;
    stream->outstandingrequests = 0;

    for (int i = 0; i < requests; i++) {
        stream->requests[i].handle = (g_NextStreamHandle << 16) | i;
        stream->requests[i].state = 0;
        stream->requests[i].hold = 0;
        stream->requests[i].next = nullptr;
    }
    for (int i = requests; i < 256; i++) {
        stream->requests[i].handle = 0;
        stream->requests[i].state = 0;
        stream->requests[i].hold = 0;
        stream->requests[i].next = nullptr;
    }
    for (int i = 0; i < 16; i++) {
        stream->taps[i].owner = buffer;
        stream->taps[i].index = i + 1;
        stream->taps[i].used = 0;
        stream->taps[i].pad = 0;
    }

    int handle = g_NextStreamHandle++;
    g_StreamTable[handle] = stream;
    AssignAudioStreamHandle(reinterpret_cast< unsigned int >(buffer));
    return handle;
}

int STREAM_setfilter(int sndstreamhandle, int sourceChannel, int lowpasscutoff, int highpasscutoff, int filterType) {
    (void)sourceChannel;
    (void)lowpasscutoff;
    (void)highpasscutoff;
    (void)filterType;
    return GetHeader(sndstreamhandle) ? 0 : -1;
}

int STREAM_destroy(int sndstreamhandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }

    delete stream;
    g_StreamTable[sndstreamhandle] = nullptr;
    return 0;
}

int STREAM_setpriority(int sndstreamhandle, int sourceChannel, int priority) {
    (void)sourceChannel;
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }
    stream->priority = priority;
    return 0;
}

int STREAM_setgreedylevel(int sndstreamhandle, int greedylevel) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }
    stream->greedylevel = greedylevel;
    return 0;
}

int STREAM_setgreedystate(int sndstreamhandle, int greedystate) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }
    stream->greedystate = greedystate;
    return 0;
}

int STREAM_taphandle(int sndstreamhandle, int tapindex) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream || tapindex < 0 || tapindex >= stream->maxtaps) {
        return -1;
    }
    return tapindex;
}

int STREAM_queuefile(int sndstreamhandle, const char *filename, int offset, int holdtime) {
    (void)filename;
    (void)offset;
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    REQUESTSTRUCTtag *req = getfreerequest(stream);
    if (!req) {
        return -1;
    }
    req->hold = holdtime;
    return queuerequest(stream, req);
}

int STREAM_queuemem(int sndstreamhandle, void *address, int length, int holdtime) {
    (void)address;
    (void)length;
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    REQUESTSTRUCTtag *req = getfreerequest(stream);
    if (!req) {
        return -1;
    }
    req->hold = holdtime;
    return queuerequest(stream, req);
}

int STREAM_cancelrequest(int sndstreamhandle, int requesthandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    REQUESTSTRUCTtag *req = locaterequest(stream, requesthandle);
    if (!req) {
        return -1;
    }
    return freerequest(stream, req);
}

int STREAM_kill(int sndstreamhandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }

    for (int i = 0; i < stream->maxrequests; i++) {
        stream->requests[i].state = 0;
    }
    stream->outstandingrequests = 0;
    stream->currentrequest = -1;
    stream->state = 0;
    return 0;
}

int STREAM_get(int sndstreamhandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return 0;
    }
    return startnextrequest(stream, 0);
}

int STREAM_release(int sndstreamhandle, STREAMCHUNKHDR *chunk) {
    (void)chunk;
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }
    return 0;
}

STREAMHEADERtag *STREAM_gettable(int sndstreamhandle) {
    return GetHeader(sndstreamhandle);
}

int STREAM_state(int sndstreamhandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return -1;
    }
    return stream->state;
}

int STREAM_isendofstream(int sndstreamhandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return 1;
    }
    return stream->eof;
}

int STREAM_buffersize(int sndstreamhandle) {
    STREAMHEADERtag *stream = GetHeader(sndstreamhandle);
    if (!stream) {
        return 0;
    }
    return stream->buffersize;
}
