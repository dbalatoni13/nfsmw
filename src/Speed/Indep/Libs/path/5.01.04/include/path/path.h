#ifndef __PATHH__
#define __PATHH__ // Decl: 2

#include "Allocator/iallocator.h"

#define PATH_H_MAJORREV 5              // Decl: 7
#define PATH_H_MINORREV 01             // Decl: 8
#define PATH_H_PATCHREV 04             // Decl: 9
#define PATH_H_VERSIONSTR4NG "5.01.04" // Decl: 10

#define PATH_OK 0 // Decl: 15

#define PATHERR_GENERAL -1         // Decl: 19
#define PATHERR_INV_PARAM -8       // Decl: 20
#define PATHERR_TOOMANY -13        // Decl: 21
#define PATHERR_NOT_INITED -14     // Decl: 22
#define PATHERR_INUSE -18          // Decl: 23
#define PATHERR_CANTOPEN -39       // Decl: 24
#define PATHERR_NOTFOUND -43       // Decl: 25
#define PATHERR_FAILALLOC -108     // Decl: 26
#define PATHERR_ALREADYLOADED -188 // Decl: 27
#define PATHERR_PENDING -9999      // Decl: 28

#define PATH_MAX_EVENTS 16  // Decl: 33
#define PATH_MAX_VOICES 4   // Decl: 34
#define PATH_MAX_PROJECTS 4 // Decl: 35
#define PATH_MAX_TRACKS 24  // Decl: 36

#define PATH_ALL 0xFFFFFFFF          // Decl: 38
#define PATH_ALL_VOICES 0xF0000000   // Decl: 39
#define PATH_ALL_PROJECTS 0x0F000000 // Decl: 40
#define PATH_ALL_TRACKS 0x00FFFFFF   // Decl: 41

#define PATH_BAD_VALUE 0xDEADBABE      // Decl: 43
#define PATH_UNLIKELY_VALUE 0xBEDFACED // Decl: 44
#define PATH_SYNCFILEOP 0xBEEFDEAF     // Decl: 45
#define PATH_VOICE_0 0x11000000        // Decl: 46

#define PATH_PROJECT(A) (0x01000000 << A)                        // Decl: 48
#define PATH_VOICE(A, B) ((0x10000000 << A) | (0x01000000 << B)) // Decl: 49
#define PATH_TRACK(A, B, C) (PATH_VOICE(A, B) | (1L << C))       // Decl: 50

#define PATH_INTENSITY 0 // Decl: 52
#define PATH_RANDOM 1    // Decl: 53

#define kAllDebugChannels 0xFFFFFFFF       // Decl: 67
#define kNoDebugChannels 0x00000000        // Decl: 68
#define kOverlayDebugChannel 0x00000001    // Decl: 69
#define kStreamDebugChannel 0x00000002     // Decl: 70
#define kControllerDebugChannel 0x00000004 // Decl: 71
#define kVolumeDebugChannel 0x00000008     // Decl: 72
#define kFadeDebugChannel 0x00000010       // Decl: 73
#define kEventDebugChannel 0x00000020      // Decl: 74
#define kStatusDebugChannel 0x00000040     // Decl: 75
#define kAuthorDebugChannel 0x00000080     // Decl: 76
#define kPathErrorDebugChannel 0x00000100  // Decl: 77
#define kPathIODebugChannel 0x00000200     // Decl: 78
#define kPathFXDebugChannel 0x00000400     // Decl: 79
#define kPathTimingDebugChannel 0x00000800 // Decl: 80

#define kRemoveChannel 0 // Decl: 85
#define kAddChannel 1    // Decl: 86
#define kAbsolute 2      // Decl: 87

// total size: 0x38
// Decl: 139
typedef struct PATHSTATUS {
    int playingnode;             // offset 0x0, size 0x4, Decl: 140
    int playingbar;              // offset 0x4, size 0x4, Decl: 141
    int playingbeat;             // offset 0x8, size 0x4, Decl: 142
    int currentnode;             // offset 0xC, size 0x4, Decl: 143
    int currentrequest;          // offset 0x10, size 0x4, Decl: 144
    int readyforrequest;         // offset 0x14, size 0x4, Decl: 145
    unsigned int nodeduration;   // offset 0x18, size 0x4, Decl: 146
    unsigned int barduration;    // offset 0x1C, size 0x4, Decl: 147
    unsigned int beatduration;   // offset 0x20, size 0x4, Decl: 148
    unsigned int timeleftinnode; // offset 0x24, size 0x4, Decl: 149
    unsigned int timeleftinbar;  // offset 0x28, size 0x4, Decl: 150
    unsigned int timeleftinbeat; // offset 0x2C, size 0x4, Decl: 151
    int timebuffered;            // offset 0x30, size 0x4, Decl: 152
    int timeremaining;           // offset 0x34, size 0x4, Decl: 153
} PATHSTATUS;

// Decl: 187
typedef enum PATHTRACKPLAYSTATUS {
    PATHTRACK_INVALID = 0,
    PATHTRACK_STOPPED = 1,
    PATHTRACK_QUEUEING = 2,
    PATHTRACK_PAUSED = 3,
    PATHTRACK_READYFORNEXT = 4,
    PATHTRACK_PLAYING = 5,
    PATHTRACK_FADING = 6,
    PATHTRACK_MAXVALUE = 7,
} PATHTRACKPLAYSTATUS;

typedef void (*SongProgressCallback)(int, int); // Decl: 214

// Decl: 216
typedef enum PATHEVENTRESULT {
    PATHEVENT_PENDING = 0,
    PATHEVENT_EXPIRED = 1,
    PATHEVENT_PURGED = 2,
    PATHEVENT_FINISHED = 3,
} PATHEVENTRESULT;

typedef void (*EventReleaseCallback)(void *, PATHEVENTRESULT); // Decl: 239

typedef void (*EventActionCallback)(int, int, int); // Decl: 260

typedef void (*PATHAbortMsgFunc)(const char *, ...); // Decl: 271

typedef int (*PATHDebugPrintFunc)(const char *, ...); // Decl: 282

namespace Path {
class IPathTrack;
};

int PATH_control(int tracks, unsigned int controller);
int PATH_pause(int tracks, unsigned char pause);
PATHTRACKPLAYSTATUS PATH_trackstatus(unsigned int trackhandle);
int PATH_status(int trackhandle, PATHSTATUS *psps);
int PATH_stop(int tracks);

int PATH_event(int tracks, unsigned int eventID);
int PATH_clearallevents(int projects);

int PATH_volume(int tracks, signed char scale);

void PATH_vectortosnd();

int PATH_shutdown();
int PATH_addmapfile(char *pmap);
void PATH_callbacks(SongProgressCallback progresscb, EventReleaseCallback eventcb, EventActionCallback actioncb);
int PATH_destroy(int trackhandle);
Path::IPathTrack *PATH_gettrackimp(int trackhandle);

void PATH_setallocator(EA::Allocator::IAllocator *allocator, const EA::TagValuePair &flags);

int PATH_createstreamtrack(int trackhandle, char *musfilename, int latency);
Path::IPathTrack *PATH_createstreamimp(int trackhandle, int maxrequests, float buffertime);

void PATH_vectortoreal6();

#endif
