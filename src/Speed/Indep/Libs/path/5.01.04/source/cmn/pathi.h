#ifndef __PATHIH__
#define __PATHIH__

#include "path/path.h"

#include "path/IPathToReal.h"
#include "path/IPathToSnd.h"
#include "path/IPathTrack.h"

#include "eathread/eathread_semaphore.h"

/*----------------------------------------------------------------------------
    Node / map file structures
----------------------------------------------------------------------------*/

// total size: 0x4
typedef struct PATHNODEBEATS {
    unsigned int forcesynch : 1; // offset 0x0
    unsigned int playbeats : 1;  // offset 0x0
} PATHNODEBEATS;

// total size: 0x4
typedef struct PATHNODEEVENT {
    unsigned int eventID : 24; // offset 0x0
} PATHNODEEVENT;

typedef union PATHNODEEXTRA {
    PATHNODEBEATS beat;     // offset 0x0
    PATHNODEEVENT sendevent; // offset 0x0
} PATHNODEEXTRA;

// total size: 0x10
typedef struct PATHFINDNODE {
    int index : 16;              // offset 0x0
    unsigned int trackID : 5;    // offset 0x0
    unsigned int sectionID : 6;  // offset 0x0
    int repeat : 5;              // offset 0x0
    unsigned int routerID : 12;  // offset 0x4
    unsigned int numbranches : 5; // offset 0x4
    unsigned int controller : 3; // offset 0x4
    unsigned int beats : 4;      // offset 0x4
    unsigned int bars : 8;       // offset 0x4
    unsigned int partID : 16;    // offset 0x8
    unsigned int synchevery : 4; // offset 0x8
    unsigned int synchoffset : 4; // offset 0x8
    unsigned int notes : 4;      // offset 0x8
    unsigned int synch : 2;      // offset 0x8
    unsigned int unused : 2;     // offset 0x8
    PATHNODEEXTRA extra;         // offset 0xC
} PATHFINDNODE;

// total size: 0x4
typedef struct PATHFINDBRANCH {
    signed char controlmin;   // offset 0x0
    signed char controlmax;   // offset 0x1
    unsigned short dstnode;   // offset 0x2
} PATHFINDBRANCH;

// total size: 0x8
typedef struct PATHFINDSAMPLE {
    unsigned int offset;   // offset 0x0
    unsigned int duration; // offset 0x4
} PATHFINDSAMPLE;

// total size: 0x14
typedef struct PATHNAMEDVAR {
    char name[16]; // offset 0x0
    int value;     // offset 0x10
} PATHNAMEDVAR;

// total size: 0x48
typedef struct PATHFINDHEADER {
    int id;                        // offset 0x0
    unsigned char majorRev;        // offset 0x4
    unsigned char minorRev;        // offset 0x5
    unsigned char release;         // offset 0x6
    unsigned char prerelease;      // offset 0x7
    unsigned short saveIncrement;  // offset 0x8
    unsigned short generateID;     // offset 0xA
    unsigned char projectID;       // offset 0xC
    unsigned char numtracks;       // offset 0xD
    unsigned char numsections;     // offset 0xE
    unsigned char numevents;       // offset 0xF
    unsigned char numrouters;      // offset 0x10
    unsigned char numnamedvars;    // offset 0x11
    unsigned short numnodes;       // offset 0x12
    unsigned int nodeoffsets;      // offset 0x14
    unsigned int nodedata;         // offset 0x18
    unsigned int eventoffsets;     // offset 0x1C
    unsigned int eventdata;        // offset 0x20
    unsigned int namedvars;        // offset 0x24
    unsigned int noderouters;      // offset 0x28
    unsigned int trackoffsets;     // offset 0x2C
    unsigned int trackinfos;       // offset 0x30
    unsigned int sampleoffsets;    // offset 0x34
    unsigned int mapfilelen;       // offset 0x38
    unsigned int v40reserve[3];    // offset 0x3C
} PATHFINDHEADER;

// total size: 0x100
typedef struct PATHMUSHEADER {
    int checksum;        // offset 0x0
    char padbytes[252];  // offset 0x4
} PATHMUSHEADER;

/*----------------------------------------------------------------------------
    Events and actions
----------------------------------------------------------------------------*/

// total size: 0x14
typedef struct PATHEVENT {
    unsigned int queued;           // offset 0x0
    unsigned int expiry;           // offset 0x4
    unsigned int lastact;          // offset 0x8
    unsigned int eventID : 24;     // offset 0xC
    unsigned int numactions : 8;   // offset 0xC
    unsigned int currentaction : 8; // offset 0x10
    unsigned int voices : 4;       // offset 0x10
    int priority : 4;              // offset 0x10
    unsigned int bumplower : 1;    // offset 0x10
    unsigned int beingFiltered : 1; // offset 0x10
    int project : 3;               // offset 0x10
    int unused : 11;               // offset 0x10
} PATHEVENT;

// total size: 0x4
typedef struct PATHACTCONDITION {
    int value : 16;        // offset 0x0
    int compareValue : 16; // offset 0x0
} PATHACTCONDITION;

// total size: 0x4
typedef struct PATHACTWAITTIME {
    int millisecs : 16; // offset 0x0
    int lowest : 16;    // offset 0x0
} PATHACTWAITTIME;

// total size: 0x4
typedef struct PATHACTWAITBEAT {
    int millisecs : 16;      // offset 0x0
    unsigned int every : 4;  // offset 0x0
    unsigned int note : 4;   // offset 0x0
    unsigned int offset : 4; // offset 0x0
} PATHACTWAITBEAT;

// total size: 0x4
typedef struct PATHACTBRANCHTO {
    int node : 16;     // offset 0x0
    int ofsection : 8; // offset 0x0
    int immediate : 1; // offset 0x0
} PATHACTBRANCHTO;

// total size: 0x4
typedef struct PATHACTFADE {
    unsigned int tovol : 8; // offset 0x0
    int id : 7;             // offset 0x0
    unsigned int flip : 1;  // offset 0x0
    unsigned int ms : 16;   // offset 0x0
} PATHACTFADE;

// total size: 0x4
typedef struct PATHACTSFXFADE {
    unsigned int tovol : 8; // offset 0x0
    int id : 7;             // offset 0x0
    unsigned int flip : 1;  // offset 0x0
    unsigned int ms : 16;   // offset 0x0
} PATHACTSFXFADE;

// total size: 0x4
typedef struct PATHACTDRYFADE {
    unsigned int tovol : 8; // offset 0x0
    int id : 7;             // offset 0x0
    unsigned int flip : 1;  // offset 0x0
    unsigned int ms : 16;   // offset 0x0
} PATHACTDRYFADE;

// total size: 0x4
typedef struct PATHACTPITCHFADE {
    unsigned int tovol : 14; // offset 0x0
    int id : 3;              // offset 0x0
    unsigned int flip : 1;   // offset 0x0
    unsigned int ms : 14;    // offset 0x0
} PATHACTPITCHFADE;

// total size: 0x4
typedef struct PATHACTSTRETCHFADE {
    unsigned int tovol : 14; // offset 0x0
    int id : 3;              // offset 0x0
    unsigned int flip : 1;   // offset 0x0
    unsigned int ms : 14;    // offset 0x0
} PATHACTSTRETCHFADE;

// total size: 0x4
typedef struct PATHACTFXLEVEL {
    int bus : 8;   // offset 0x0
    int level : 8; // offset 0x0
} PATHACTFXLEVEL;

// total size: 0x4
typedef struct PATHACTDRYLEVEL {
    int level : 8; // offset 0x0
} PATHACTDRYLEVEL;

// total size: 0x4
typedef struct PATHACTSETVOL {
    int volume : 8; // offset 0x0
} PATHACTSETVOL;

// total size: 0x4
typedef struct PATHACTSETVALUE {
    int towhat : 16;          // offset 0x0
    unsigned int setwhat : 8; // offset 0x0
} PATHACTSETVALUE;

// total size: 0x4
typedef struct PATHACTEVENT {
    unsigned int eventid : 24; // offset 0x0
} PATHACTEVENT;

// total size: 0x4
typedef struct PATHACTFILTER {
    unsigned int eventid : 24;      // offset 0x0
    unsigned int beingFiltered : 1; // offset 0x0
} PATHACTFILTER;

// total size: 0x4
typedef struct PATHACTCALLBACK {
    int value : 16;         // offset 0x0
    unsigned int id : 16;   // offset 0x0
} PATHACTCALLBACK;

// total size: 0x4
typedef struct PATHACTCALC {
    unsigned int value : 8; // offset 0x0
    unsigned int op : 8;    // offset 0x0
    int by : 16;            // offset 0x0
} PATHACTCALC;

// total size: 0x4
typedef struct PATHACTPAUSE {
    unsigned int when : 16; // offset 0x0
    unsigned int on : 1;    // offset 0x0
} PATHACTPAUSE;

// total size: 0x4
typedef struct PATHLOADBANK {
    unsigned int subbanknum : 8; // offset 0x0
    unsigned int unload : 8;     // offset 0x0
} PATHLOADBANK;

typedef union PATHACT {
    PATHACTCONDITION only;          // offset 0x0
    PATHACTWAITTIME waittime;       // offset 0x0
    PATHACTWAITBEAT waitbeat;       // offset 0x0
    PATHACTBRANCHTO branch;         // offset 0x0
    PATHACTFADE fade;               // offset 0x0
    PATHACTSFXFADE sfxfade;         // offset 0x0
    PATHACTDRYFADE dryfade;         // offset 0x0
    PATHACTPITCHFADE pitchfade;     // offset 0x0
    PATHACTSTRETCHFADE stretchfade; // offset 0x0
    PATHACTSETVALUE setval;         // offset 0x0
    PATHACTEVENT event;             // offset 0x0
    PATHACTFILTER filter;           // offset 0x0
    PATHACTCALLBACK callback;       // offset 0x0
    PATHACTCALC calc;               // offset 0x0
    PATHACTPAUSE pause;             // offset 0x0
    PATHLOADBANK loadbank;          // offset 0x0
} PATHACT;

// total size: 0xC
typedef struct PATHACTION {
    int track;                       // offset 0x0
    int sectionID : 8;               // offset 0x4
    unsigned int type : 7;           // offset 0x4
    unsigned int done : 1;           // offset 0x4
    unsigned int leftvaluetype : 2;  // offset 0x4
    unsigned int rightvaluetype : 2; // offset 0x4
    unsigned int assess : 3;         // offset 0x4
    unsigned int comparison : 3;     // offset 0x4
    unsigned int indent : 3;         // offset 0x4
    unsigned int unused : 3;         // offset 0x4
    PATHACT act;                     // offset 0x8
} PATHACTION;

typedef enum PATHACTIONTYPE {
    PATHACTION_NONE = 0,
    PATHACTION_CONDITION = 1,
    PATHACTION_WAITTIME = 2,
    PATHACTION_WAITBEAT = 3,
    PATHACTION_BRANCHTO = 4,
    PATHACTION_FADE = 5,
    PATHACTION_DRYLEVELFADE = 6,
    PATHACTION_SFXFADE = 7,
    PATHACTION_SETVALUE = 8,
    PATHACTION_EVENT = 9,
    PATHACTION_FILTER_ON = 10,
    PATHACTION_FILTER_OFF = 11,
    PATHACTION_FILTER_CLEAR = 12,
    PATHACTION_CALLBACK = 13,
    PATHACTION_CALC = 14,
    PATHACTION_PAUSE = 15,
    PATHACTION_LOADBANK = 16,
    PATHACTION_PITCHFADE = 17,
    PATHACTION_STRETCHFADE = 18,
    PATHACTION_MAX = 19,
} PATHACTIONTYPE;

typedef enum PATHVALUETYPE {
    PATH_VALUE_BADTYPE = 0,
    PATH_VALUE_SPECIAL = 1,
    PATH_VALUE_VARIABLE = 2,
    PATH_VALUE_INTEGER = 3,
    PATH_VALUE_MAXTYPES = 4,
} PATHVALUETYPE;

typedef enum PATHSPECIALVALUETYPE {
    PATH_SPECIALVALUE_BAD = 0,
    PATH_CONTROLLER = 1,
    PATH_CURRENTNODE = 2,
    PATH_CURRENTPART = 3,
    PATH_CURRENTSECTION = 4,
    PATH_EVENTEXPIRY = 5,
    PATH_EVENTPRIORITY = 6,
    PATH_FXBUS = 7,
    PATH_FXDRYLEVEL = 8,
    PATH_FXSENDLEVEL = 9,
    PATH_MAINVOICE = 10,
    PATH_NEXTNODE = 11,
    PATH_NOBRANCHING = 12,
    PATH_NODEDURATION = 13,
    PATH_PAUSE = 14,
    PATH_PITCHMULT = 15,
    PATH_PLAYINGNODE = 16,
    PATH_PLAYSTATUS = 17,
    PATH_RANDOMSHORT = 18,
    PATH_TIMENOW = 19,
    PATH_TIMETONEXTBEAT = 20,
    PATH_TIMETONEXTBAR = 21,
    PATH_TIMETONEXTNODE = 22,
    PATH_VOLUME = 23,
    PATH_TIMESTRETCH = 24,
    PATH_BARDURATION = 25,
    PATH_BEATDURATION = 26,
    PATH_SPECIALVALUE_MAX = 27,
} PATHSPECIALVALUETYPE;

typedef enum PATHCOMPARE {
    PATH_COMPARE_INVALID = 0,
    PATH_COMPARE_EQUALS = 1,
    PATH_COMPARE_NOT_EQUAL = 2,
    PATH_COMPARE_GREATER_THAN = 3,
    PATH_COMPARE_LESS_THAN = 4,
    PATH_COMPARE_GREATER_OR_EQUAL = 5,
    PATH_COMPARE_LESS_OR_EQUAL = 6,
    PATH_COMPARE_MAX = 7,
} PATHCOMPARE;

typedef enum PATHOPERATOR {
    PATH_OPERATOR_INVALID = 0,
    PATH_OPERATOR_ADD = 1,
    PATH_OPERATOR_SUB = 2,
    PATH_OPERATOR_MULT = 3,
    PATH_OPERATOR_DIV = 4,
    PATH_OPERATOR_MOD = 5,
    PATH_OPERATOR_MAX = 6,
} PATHOPERATOR;

typedef enum PATHFADETYPE {
    PATH_FADE_LINEAR = 1,
    PATH_FADE_EQPOWER = 2,
    PATH_FADE_EXPONENTIAL = 3,
    PATH_FADE_COSINE = 4,
} PATHFADETYPE;

/*----------------------------------------------------------------------------
    Track
----------------------------------------------------------------------------*/

// total size: 0x10
typedef struct PATHFADESTATS {
    unsigned int fadestart; // offset 0x0
    unsigned int fadetime;  // offset 0x4
    short fadefrom;         // offset 0x8
    short fadeto;           // offset 0xA
    short fadenum;          // offset 0xC
    short unused;           // offset 0xE
} PATHFADESTATS;

// total size: 0x4
typedef struct PATHFADEINFO {
    unsigned int id : 7;      // offset 0x0
    unsigned int flip : 1;    // offset 0x0
    unsigned int unused : 8;  // offset 0x0
    unsigned int ms : 16;     // offset 0x0
} PATHFADEINFO;

// total size: 0x1C
typedef struct PATHBEATINFO {
    unsigned int beats : 4;       // offset 0x0
    unsigned int notes : 4;       // offset 0x0
    unsigned int playingbeat : 8; // offset 0x0
    unsigned int beatduration;    // offset 0x4
    unsigned int barduration;     // offset 0x8
    unsigned int nodeduration;    // offset 0xC
    unsigned int timetonextbeat;  // offset 0x10
    unsigned int timetonextbar;   // offset 0x14
    unsigned int timetonextnode;  // offset 0x18
} PATHBEATINFO;

// total size: 0x140
typedef struct PATHTRACK {
    unsigned int trackID : 8;    // offset 0x0
    unsigned int control : 8;    // offset 0x0
    int volscale : 8;            // offset 0x0
    int volume : 8;              // offset 0x0
    int node : 16;               // offset 0x4
    int repeatnode : 16;         // offset 0x4
    unsigned int latency : 15;   // offset 0x8
    int repeat : 5;              // offset 0x8
    int nodebeat : 5;            // offset 0x8
    unsigned int paused : 1;     // offset 0x8
    unsigned int nobranch : 1;   // offset 0x8
    unsigned int ramtrack : 1;   // offset 0x8
    unsigned int sfxbus : 3;     // offset 0x8
    unsigned int mainvoice : 1;  // offset 0x8
    unsigned int sfxlevel : 16;  // offset 0xC
    unsigned int drylevel : 16;  // offset 0xC
    PATHFADESTATS volumefade;    // offset 0x10
    PATHFADESTATS sfxsendfade;   // offset 0x20
    PATHFADESTATS drylevelfade;  // offset 0x30
    PATHFADESTATS pitchfade;     // offset 0x40
    PATHFADESTATS stretchfade;   // offset 0x50
    PATHFINDNODE *entryinfo;     // offset 0x60
    unsigned int beattime;       // offset 0x64
    unsigned int bartime;        // offset 0x68
    unsigned int nextbeattime;   // offset 0x6C
    unsigned int pauseat;        // offset 0x70
    unsigned int resumeat;       // offset 0x74
    int newestrequesthandle;     // offset 0x78
    char musicfilename[128];     // offset 0x7C
    char trackname[32];          // offset 0xFC
    int loadingsubbank;          // offset 0x11C
    int fileop;                  // offset 0x120
    int loadingfile;             // offset 0x124
    int freeable;                // offset 0x128
    Path::IPathTrack *trackimp;  // offset 0x12C
    unsigned short maxsubbanks;  // offset 0x130
    unsigned int subbankaramuse; // offset 0x134
    unsigned int subbankmramuse; // offset 0x138
    PATHTRACKPLAYSTATUS status;  // offset 0x13C
} PATHTRACK;

/*----------------------------------------------------------------------------
    Global finder state
----------------------------------------------------------------------------*/

#define PATH_EVENTHEAPSIZE 4096

// total size: 0x1104
typedef struct PATHFINDERSTATE {
    int mastertrack;                    // offset 0x0
    unsigned int masterlatency;         // offset 0x4
    unsigned int mastersection;         // offset 0x8
    unsigned int taskinterval;          // offset 0xC
    unsigned int timerinterval;         // offset 0x10
    unsigned int idflags;               // offset 0x14
    PATHFINDHEADER *pmap;               // offset 0x18
    short *pnodeoffsets;                // offset 0x1C
    PATHFINDNODE *pnodes;               // offset 0x20
    short *peventoffsets;               // offset 0x24
    PATHEVENT *pevents;                 // offset 0x28
    PATHNAMEDVAR *pnamedvars;           // offset 0x2C
    int *prouters;                      // offset 0x30
    int *ptrackoffsets;                 // offset 0x34
    PATHTRACKINFO *ptrackinfos;         // offset 0x38
    PATHFINDSAMPLE *psampleoffsets;     // offset 0x3C
    PATHTRACK *track[PATH_MAX_TRACKS];  // offset 0x40
    char *pnamfile;                     // offset 0xA0
    int namevents;                      // offset 0xA4
    int namparts;                       // offset 0xA8
    int namdefines;                     // offset 0xAC
    int nameventoffset;                 // offset 0xB0
    int nampartoffset;                  // offset 0xB4
    int namdefineoffset;                // offset 0xB8
    int namend;                         // offset 0xBC
    PATHEVENT *eventqueue[PATH_MAX_EVENTS]; // offset 0xC0
    char eventheap[PATH_EVENTHEAPSIZE]; // offset 0x100
    int eventindex;                     // offset 0x1100
} PATHFINDERSTATE;

namespace Path {


extern PATHFINDERSTATE *pfstates[PATH_MAX_PROJECTS];
extern PATHFINDERSTATE *pfstate;

extern SongProgressCallback songprogress;
extern EventReleaseCallback eventrelease;
extern EventActionCallback eventaction;
extern unsigned char inited;
extern unsigned char paused;
extern signed char volscale;
extern char bankservice;
extern int timercallsinarow;
extern int lasttimercb;
extern unsigned int milliseconds;
extern int debugchannels;
extern int defaultfxbus;
extern void *(*memalloc)(int);
extern void (*memfree)(void *);
extern int timercalls;
extern int timertimespent;
extern int taskcalls;
extern int tasktimespent;

}; // namespace Path

/*----------------------------------------------------------------------------
    Internal API
----------------------------------------------------------------------------*/

int PATHI_lock();
void PATHI_unlock();
int PATHI_init();
void *PATHI_memalloc(int size);
void PATHI_memfree(void *pmem);
int PATHI_bytesperms(int trackID);

// The semaphore wrapper only exists to route allocation through the library's
// own allocator; the (int) constructor of EA::Thread::Semaphore is private and
// PathSemaphore is its only friend.
struct PathSemaphore : public EA::Thread::Semaphore
{
    static void *operator new(unsigned int size) { return PATHI_memalloc(size); }
    static void operator delete(void *ptr) { PATHI_memfree(ptr); }

    PathSemaphore(int initialCount) : EA::Thread::Semaphore(initialCount) {}
    PathSemaphore() {}
    PathSemaphore &operator=(const PathSemaphore &) { return *this; }
    ~PathSemaphore() {}
};

// Defined in the small-data blob, like every other Path global.
extern PathSemaphore *pathsemaphore;

int PATHI_addevent(unsigned int projectflags, PATHEVENT *event);
PATHEVENT *PATHI_copyevent(PATHEVENT *event);
void PATHI_releaseevent(int e, PATHEVENTRESULT result);
void PATHI_removeevent(PATHEVENT *event);
void PATHI_moveevent(PATHEVENT *event, PATHEVENT *newlocation);
void PATHI_seteventfilter(PATHEVENT *in_event, int onOff);
void PATHI_clearalleventfilters();
int PATHI_serviceevent(int eventindex);
int PATHI_serviceeventqueue();
int PATHI_eventtakespriority(int e);
void PATHI_addaction(PATHEVENT *event, PATHACTION *action);

int PATHI_serviceaction(PATHEVENT *event, PATHACTION *action);
void PATHI_conditiondone(PATHACTION *action, int skiptonextelse, PATHACTION *endaction);
int PATHI_restoretolastwhile(PATHACTION *action, PATHEVENT *event);
PATHTRACKPLAYSTATUS PATHI_trackstatus(PATHTRACK *track);
int PATHI_getvalue(int val, int inValType, PATHTRACK *track, PATHEVENT *event);
void PATHI_setvalue(int setwhat, int inValType, int towhat, PATHTRACK *track, PATHEVENT *event);

int PATHI_loadbank(PATHTRACK *track, int subbanknum);
int PATHI_subbankready(PATHTRACK *track, int subbanknum);
int PATHI_loadbankdata(PATHTRACK *track, int subbanknum, int subbanksize);
int PATHI_unloadbank(PATHTRACK *track, int subbanknum);
int PATHI_unloadmostneglectedsubbank(PATHTRACK *track);
int PATHI_verifymusfile(PATHTRACK *track, int subbanknum, char *musfilepath);

int PATHI_status(PATHTRACK *track, PATHSTATUS *psps);
int PATHI_stop(PATHTRACK *track);

unsigned int PATHI_sampleoffset(int node);
int PATHI_beatinfo(PATHTRACK *track, PATHBEATINFO *beatinfo);
int PATHI_calcwaitbeat(int every, int note, int offset, PATHBEATINFO *beatinfo);
int PATHI_choosesynchtime(int node, const PATHFINDNODE &entryinfo, const PATHBEATINFO &masterinfo, unsigned int &waitms);
int PATHI_timeremaining(PATHTRACK *track);
int PATHI_timetonextbeat(PATHTRACK *track, unsigned int *outbeatlen);
int PATHI_pickclosestbranch(int numBranches, int control, PATHFINDBRANCH *branch);
int PATHI_nextnode(int node, int control, int forreal);
int PATHI_enternode(int origin, int node, int control, int forreal);
int PATHI_routenode(int fromnode, int destnode);
void PATHI_seeknextnode(int trackindex);
int PATHI_queuenode(PATHTRACK *track);

int PATHI_switchproject(int p, int idflags);
int PATHI_switchvoice(unsigned int voiceflags);
void PATHI_sortprojects();
void PATHI_serviceproject();
void PATHI_service(char isbankservice);
void PATHI_servicetask();
void PATHI_servicetimer();
int PATHI_playingtrackhandle(int projectID, int trackID);

int PATHI_createtrack(int trackhandle, char *musfilename);
int PATHI_inittrack(int trackhandle, char *musfilename);
PATHTRACK *PATHI_gettrackptr(unsigned int trackhandle);
void PATHI_getmastertrack();
void PATHI_mainvoice(PATHTRACK *track, int mainvoice);
void PATHI_statusall(int clear);

void PATHI_volume(PATHTRACK *track, signed char volume);
void PATHI_fade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_sfxfade(PATHTRACK *track, int fadeto, int ms, int fadenum);
void PATHI_customsfxfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_customdrylevelfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_custompitchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_customstretchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_setfadevolume(PATHTRACK *track);
void PATHI_setsfxfadevolume(PATHTRACK *track);
void PATHI_setdrylevelfadevolume(PATHTRACK *track);
void PATHI_setpitchfadevolume(PATHTRACK *track);
void PATHI_setstretchfadevolume(PATHTRACK *track);

unsigned int PATHI_random();

void PATHI_printf(char *format, ...);
void PATHI_logf(char *format, ...);
void PATHI_abortmessage(char *format, ...);
void PATHI_SetDebuggingChannels(int mode, int channels);
int PATHI_GetDebuggingChannels();

/*----------------------------------------------------------------------------
    Inline helpers
----------------------------------------------------------------------------*/

inline PATHEVENT *PATHI_getevent(unsigned int eventID, unsigned int eventIDMask)
{
    PATHEVENT *eventp;
    int eventIndex;

    eventIndex = Path::pfstate->pmap->numevents;

    while (--eventIndex >= 0)
    {
        eventp = (PATHEVENT *)((int *)Path::pfstate->pmap +
                               (unsigned short)Path::pfstate->peventoffsets[eventIndex]);

        if ((eventp->eventID & eventIDMask) == (eventID & eventIDMask))
            return eventp;
    }

    return 0;
}

inline PATHFINDNODE *PATHI_getnode(int nodeIndex)
{
    if (nodeIndex < 0 || nodeIndex > Path::pfstate->pmap->numnodes)
        return 0;

    return (PATHFINDNODE *)((int *)Path::pfstate->pmap +
                            (unsigned short)Path::pfstate->pnodeoffsets[nodeIndex]);
}

inline PATHTRACKINFO *PATHI_gettrackinfo(int trackID)
{
    PATHTRACKINFO *trackinfo = 0;

    if (trackID >= 0 && trackID < Path::pfstate->pmap->numtracks)
        trackinfo = (PATHTRACKINFO *)((int *)Path::pfstate->pmap +
                                      Path::pfstate->ptrackoffsets[trackID]);

    return trackinfo;
}

inline int PATHI_readyfornewrequest(PATHTRACK *track)
{
    if (track->loadingsubbank >= 0)
        return 0;

    return track->trackimp->ReadyForNewRequest();
}

inline int PATHI_numprojects()
{
    int numprojects = 0;
    {
        int p;
        for (p = 0; p < PATH_MAX_PROJECTS; p++)
        {
            if (Path::pfstates[p])
                numprojects++;
        }
    }
    return numprojects;
}

#endif
