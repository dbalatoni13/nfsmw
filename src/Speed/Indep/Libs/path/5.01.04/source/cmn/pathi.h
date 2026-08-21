#ifndef __PATHIH__
#define __PATHIH__

#include "../../../../../../../../include/dol2asm.h"
#include "eathread/eathread_semaphore.h"
#include "path/IPathTrack.h"

struct PATHFINDHEADER {
    int id;
    unsigned char majorRev;
    unsigned char minorRev;
    unsigned char release;
    unsigned char prerelease;
    unsigned short saveIncrement;
    unsigned short generateID;
    unsigned char projectID;
    unsigned char numtracks;
    unsigned char numsections;
    unsigned char numevents;
    unsigned char numrouters;
    unsigned char numnamedvars;
    unsigned short numnodes;
    unsigned int nodeoffsets;
    unsigned int nodedata;
    unsigned int eventoffsets;
    unsigned int eventdata;
    unsigned int namedvars;
    unsigned int noderouters;
    unsigned int trackoffsets;
    unsigned int trackinfos;
    unsigned int sampleoffsets;
    unsigned int mapfilelen;
    unsigned int v40reserve[3];
};
struct PATHNAMEDVAR {
    char name[16];
    int value;
};
struct PATHFINDSAMPLE {
    unsigned int offset;
    unsigned int duration;
};

struct PATHFINDBRANCH {
    signed char controlmin;
    signed char controlmax;
    unsigned short dstnode;
};

enum PATHVALUETYPE {
    PATH_VALUE_BADTYPE = 0,
    PATH_VALUE_SPECIAL = 1,
    PATH_VALUE_VARIABLE = 2,
    PATH_VALUE_INTEGER = 3,
    PATH_VALUE_MAXTYPES = 4
};

struct PATHACTCONDITION {
    int value : 16;
    int compareValue : 16;
};

struct PATHACTWAITTIME {
    int millisecs : 16;
    int lowest : 16;
};

struct PATHACTWAITBEAT {
    int millisecs : 16;
    unsigned int every : 4;
    unsigned int note : 4;
    unsigned int offset : 4;
};

struct PATHACTBRANCHTO {
    int node : 16;
    int ofsection : 8;
    int immediate : 1;
};

struct PATHACTFADE {
    unsigned int tovol : 8;
    int id : 7;
    unsigned int flip : 1;
    unsigned int ms : 16;
};

struct PATHACTSFXFADE {
    unsigned int tovol : 8;
    int id : 7;
    unsigned int flip : 1;
    unsigned int ms : 16;
};

struct PATHACTDRYFADE {
    unsigned int tovol : 8;
    int id : 7;
    unsigned int flip : 1;
    unsigned int ms : 16;
};

struct PATHACTPITCHFADE {
    unsigned int tovol : 14;
    int id : 3;
    unsigned int flip : 1;
    unsigned int ms : 14;
};

struct PATHACTSTRETCHFADE {
    unsigned int tovol : 14;
    int id : 3;
    unsigned int flip : 1;
    unsigned int ms : 14;
};

struct PATHACTSETVALUE {
    int towhat : 16;
    unsigned int setwhat : 8;
};

struct PATHACTEVENT {
    unsigned int eventid : 24;
};

struct PATHACTFILTER {
    unsigned int eventid : 24;
    unsigned int beingFiltered : 1;
};

struct PATHACTCALLBACK {
    int value : 16;
    unsigned int id : 16;
};

struct PATHACTCALC {
    unsigned int value : 8;
    unsigned int op : 8;
    int by : 16;
};

struct PATHACTPAUSE {
    unsigned int when : 16;
    unsigned int on : 1;
};

struct PATHLOADBANK {
    unsigned int subbanknum : 8;
    unsigned int unload : 8;
};

union PATHACT {
    PATHACTCONDITION only;
    PATHACTWAITTIME waittime;
    PATHACTWAITBEAT waitbeat;
    PATHACTBRANCHTO branch;
    PATHACTFADE fade;
    PATHACTSFXFADE sfxfade;
    PATHACTDRYFADE dryfade;
    PATHACTPITCHFADE pitchfade;
    PATHACTSTRETCHFADE stretchfade;
    PATHACTSETVALUE setval;
    PATHACTEVENT event;
    PATHACTFILTER filter;
    PATHACTCALLBACK callback;
    PATHACTCALC calc;
    PATHACTPAUSE pause;
    PATHLOADBANK loadbank;
};

struct PATHACTION {
    int track;
    int sectionID : 8;
    unsigned int type : 7;
    unsigned int done : 1;
    unsigned int leftvaluetype : 2;
    unsigned int rightvaluetype : 2;
    unsigned int assess : 3;
    unsigned int comparison : 3;
    unsigned int indent : 3;
    unsigned int unused : 3;
    PATHACT act;
};

struct PATHEVENT {
    unsigned int queued;
    unsigned int expiry;
    unsigned int lastact;
    unsigned int eventID : 24;
    unsigned int numactions : 8;
    unsigned int currentaction : 8;
    unsigned int voices : 4;
    int priority : 4;
    unsigned int bumplower : 1;
    unsigned int beingFiltered : 1;
    int project : 3;
    int unused : 11;
};

struct PATHNODEBEATS {
    unsigned int forcesynch : 1;
    unsigned int playbeats : 1;
};

struct PATHNODEEVENT {
    unsigned int eventID : 24;
};

union PATHNODEEXTRA {
    PATHNODEBEATS beat;
    PATHNODEEVENT sendevent;
};

struct PATHFINDNODE {
    int index : 16;
    unsigned int trackID : 5;
    unsigned int sectionID : 6;
    int repeat : 5;
    unsigned int routerID : 12;
    unsigned int numbranches : 5;
    unsigned int controller : 3;
    unsigned int beats : 4;
    unsigned int bars : 8;
    unsigned int partID : 16;
    unsigned int synchevery : 4;
    unsigned int synchoffset : 4;
    unsigned int notes : 4;
    unsigned int synch : 2;
    unsigned int unused : 2;
    PATHNODEEXTRA extra;
};

struct PATHBEATINFO {
    unsigned int beats : 4;
    unsigned int notes : 4;
    unsigned int playingbeat : 8;
    unsigned int beatduration;
    unsigned int barduration;
    unsigned int nodeduration;
    unsigned int timetonextbeat;
    unsigned int timetonextbar;
    unsigned int timetonextnode;
};

void *PATHI_memalloc(int size);
void PATHI_memfree(void *pmem);

struct PATHFADEINFO {
    unsigned int id : 7;
    unsigned int flip : 1;
    unsigned int unused : 8;
    unsigned int ms : 16;
};

struct PATHFADESTATS {
    unsigned int fadestart;
    unsigned int fadetime;
    short fadefrom;
    short fadeto;
    short fadenum;
    short unused;
};

struct PATHTRACK {
    unsigned int trackID : 8;
    unsigned int control : 8;
    int volscale : 8;
    int volume : 8;
    int node : 16;
    int repeatnode : 16;
    unsigned int latency : 15;
    int repeat : 5;
    int nodebeat : 5;
    unsigned int paused : 1;
    unsigned int nobranch : 1;
    unsigned int ramtrack : 1;
    unsigned int sfxbus : 3;
    unsigned int mainvoice : 1;
    unsigned int sfxlevel : 16;
    unsigned int drylevel : 16;
    PATHFADESTATS volumefade;
    PATHFADESTATS sfxsendfade;
    PATHFADESTATS drylevelfade;
    PATHFADESTATS pitchfade;
    PATHFADESTATS stretchfade;
    PATHFINDNODE *entryinfo;
    unsigned int beattime;
    unsigned int bartime;
    unsigned int nextbeattime;
    unsigned int pauseat;
    unsigned int resumeat;
    int newestrequesthandle;
    char musicfilename[128];
    char trackname[32];
    int loadingsubbank;
    int fileop;
    int loadingfile;
    int freeable;
    Path::IPathTrack *trackimp;
    unsigned short maxsubbanks;
    unsigned int subbankaramuse;
    unsigned int subbankmramuse;
    PATHTRACKPLAYSTATUS status;
};

struct PATHFINDERSTATE {
    int mastertrack;
    unsigned int masterlatency;
    unsigned int mastersection;
    unsigned int taskinterval;
    unsigned int timerinterval;
    unsigned int idflags;
    PATHFINDHEADER *pmap;
    short *pnodeoffsets;
    PATHFINDNODE *pnodes;
    short *peventoffsets;
    PATHEVENT *pevents;
    PATHNAMEDVAR *pnamedvars;
    int *prouters;
    int *ptrackoffsets;
    PATHTRACKINFO *ptrackinfos;
    PATHFINDSAMPLE *psampleoffsets;
    PATHTRACK *track[24];
    char *pnamfile;
    int namevents;
    int namparts;
    int namdefines;
    int nameventoffset;
    int nampartoffset;
    int namdefineoffset;
    int namend;
    PATHEVENT *eventqueue[16];
    char eventheap[4096];
    int eventindex;
};

struct PathSemaphore : public EA::Thread::Semaphore {
    static void *operator new(unsigned int size) {
        return PATHI_memalloc(size);
    }

    static void operator delete(void *ptr) {
        PATHI_memfree(ptr);
    }

    PathSemaphore(int initialCount) : EA::Thread::Semaphore(initialCount) {}
    PathSemaphore() {}
    PathSemaphore &operator=(const PathSemaphore &) { return *this; }
    ~PathSemaphore() {}
};

typedef void *(*PATHMemAllocFunc)(int);
typedef void (*PATHMemFreeFunc)(void *);

extern PathSemaphore *pathsemaphore;

namespace Path {

extern PATHFINDERSTATE *pfstates[4];
extern PATHFINDERSTATE *pfstate;
extern PATHMemAllocFunc memalloc;
extern PATHMemFreeFunc memfree;
extern SongProgressCallback songprogress;
extern EventReleaseCallback eventrelease;
extern EventActionCallback eventaction;
extern unsigned char paused;
extern unsigned char inited;
extern char bankservice;
extern signed char volscale;
extern int defaultfxbus;
extern int lasttimercb;
extern int timercallsinarow;
extern unsigned int milliseconds;
extern int debugchannels;

};

inline PATHTRACKINFO *PATHI_gettrackinfo(int trackID) {
    PATHTRACKINFO *trackinfo;

    trackinfo = 0;
    if (trackID >= 0 && trackID < Path::pfstate->pmap->numtracks) {
        trackinfo = reinterpret_cast<PATHTRACKINFO *>(
            reinterpret_cast<char *>(Path::pfstate->pmap) + Path::pfstate->ptrackoffsets[trackID] * 4);
    }
    return trackinfo;
}

inline PATHFINDNODE *PATHI_getnode(int nodeIndex) {
    if (nodeIndex >= 0 && nodeIndex <= Path::pfstate->pmap->numnodes) {
        return reinterpret_cast<PATHFINDNODE *>(
            reinterpret_cast<char *>(Path::pfstate->pmap) +
            reinterpret_cast<unsigned short *>(Path::pfstate->pnodeoffsets)[nodeIndex] * 4);
    }
    return 0;
}

inline PATHEVENT *PATHI_getevent(unsigned int eventID, unsigned int eventIDMask) {
    PATHEVENT *eventp;
    int eventIndex;

    eventp = 0;
    for (eventIndex = Path::pfstate->pmap->numevents - 1; eventIndex >= 0; eventIndex--) {
        eventp = reinterpret_cast<PATHEVENT *>(reinterpret_cast<char *>(Path::pfstate->pmap) +
                                              reinterpret_cast<unsigned short *>(Path::pfstate->peventoffsets)[eventIndex] * 4);
        if ((eventp->eventID & eventIDMask) == (eventID & eventIDMask)) {
            break;
        }
    }
    if (eventIndex < 0) {
        eventp = 0;
    }
    return eventp;
}

int PATHI_lock();
void PATHI_unlock();
int PATH_addmapfile(char *pmap);
int PATH_milliseconds();
int PATHI_bytesperms(int trackID);
int PATH_destroy(int trackhandle);
int PATH_numtracks(unsigned int projects);
int PATH_createstreamtrack(int trackhandle, char *musfilename, int latency);
Path::IPathTrack *PATH_createstreamimp(int trackhandle, int maxrequests, float buffertime);
int PATHI_createtrack(int trackhandle, char *musfilename);
int PATHI_inittrack(int trackhandle, char *musfilename);
int PATHI_switchproject(int p, int idflags);
int PATHI_switchvoice(unsigned int voiceflags);
void PATHI_sortprojects();
void PATHI_serviceproject();
void PATHI_service(char service);
void PATHI_servicetask();
void PATHI_servicetimer();
void PATHI_getmastertrack();
void PATHI_statusall(int clear);
int PATHI_serviceeventqueue();
int PATHI_addevent(unsigned int projectflags, PATHEVENT *event);
PATHEVENT *PATHI_copyevent(PATHEVENT *event);
int PATHI_serviceevent(int eventindex);
int PATHI_eventtakespriority(int eventindex);
int PATHI_serviceaction(PATHEVENT *event, PATHACTION *action);
void PATHI_seteventfilter(PATHEVENT *event, int onOff);
void PATHI_clearalleventfilters();
void PATHI_releaseevent(int eventindex, PATHEVENTRESULT result);
void PATHI_removeevent(PATHEVENT *event);
void PATHI_moveevent(PATHEVENT *event, PATHEVENT *newlocation);
void PATHI_setfadevolume(PATHTRACK *track);
void PATHI_setsfxfadevolume(PATHTRACK *track);
void PATHI_setdrylevelfadevolume(PATHTRACK *track);
void PATHI_setpitchfadevolume(PATHTRACK *track);
void PATHI_setstretchfadevolume(PATHTRACK *track);
int PATHI_subbankready(PATHTRACK *track, int subbank);
int PATHI_timeremaining(PATHTRACK *track);
void PATHI_seeknextnode(int trackhandle);
PATHTRACK *PATHI_gettrackptr(unsigned int trackhandle);
PATHTRACKPLAYSTATUS PATHI_trackstatus(PATHTRACK *track);
int PATHI_status(PATHTRACK *track, PATHSTATUS *psps);
int PATHI_beatinfo(PATHTRACK *track, PATHBEATINFO *beatinfo);
int PATHI_calcwaitbeat(int every, int note, int offset, PATHBEATINFO *beatinfo);
void PATHI_fade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_customsfxfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_customdrylevelfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_custompitchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
void PATHI_customstretchfade(PATHTRACK *track, int fadeto, int fadetime, int fadenum);
int PATHI_loadbank(PATHTRACK *track, int subbanknum);
void PATHI_volume(PATHTRACK *track, signed char volume);
void PATHI_mainvoice(PATHTRACK *track, int mainvoice);
int PATHI_stop(PATHTRACK *track);
unsigned int PATHI_random();
int PATHI_routenode(int origin, int node);
int PATHI_nextnode(int node, int control, int forreal);
int PATHI_enternode(int origin, int node, int control, int forreal);
int PATHI_queuenode(PATHTRACK *track);
unsigned int PATHI_sampleoffset(int node);
PATHEVENT *PATHI_copyevent(PATHEVENT *event);
int PATHI_addevent(unsigned int projectflags, PATHEVENT *event);
int PATHI_loadbankdata(PATHTRACK *track, int subbanknum, int subbanksize);
int PATHI_unloadbank(PATHTRACK *track, int subbanknum);
int PATHI_unloadmostneglectedsubbank(PATHTRACK *track);
void PATHI_printf(char *format, ...);

inline int PATHI_readyfornewrequest(PATHTRACK *track) {
    return track->trackimp->ReadyForNewRequest();
}

#endif
