#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

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

static STREAMHEADERtag *g_StreamTable[64];
static int g_NextStreamHandle = 1;

void AssignAudioStreamHandle(unsigned int realstrmhandle);

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

int restartstream(STREAMHEADERtag *stream, int flags) {
    if (!stream) {
        return -1;
    }

    stream->state = 0;
    stream->eof = 0;
    (void)flags;
    return startnextrequest(stream, 0);
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
