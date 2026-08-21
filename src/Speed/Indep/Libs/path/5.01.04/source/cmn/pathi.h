#ifndef __PATHIH__
#define __PATHIH__

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
struct PATHEVENT;

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

void *PATHI_memalloc(int size);
void PATHI_memfree(void *pmem);

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
extern unsigned int milliseconds;

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
void PATHI_setfadevolume(PATHTRACK *track);
void PATHI_setsfxfadevolume(PATHTRACK *track);
void PATHI_setdrylevelfadevolume(PATHTRACK *track);
void PATHI_setpitchfadevolume(PATHTRACK *track);
void PATHI_setstretchfadevolume(PATHTRACK *track);
void PATHI_subbankready(PATHTRACK *track, int subbank);
int PATHI_timeremaining(PATHTRACK *track);
void PATHI_seeknextnode(int trackhandle);
PATHTRACK *PATHI_gettrackptr(unsigned int trackhandle);
PATHTRACKPLAYSTATUS PATHI_trackstatus(PATHTRACK *track);
int PATHI_status(PATHTRACK *track, PATHSTATUS *psps);
int PATHI_stop(PATHTRACK *track);

inline int PATHI_readyfornewrequest(PATHTRACK *track) {
    return track->trackimp->ReadyForNewRequest();
}

#endif
