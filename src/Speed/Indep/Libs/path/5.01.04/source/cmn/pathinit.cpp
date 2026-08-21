#include "pathi.h"
#include "path/IPathToReal.h"
#include "path/IPathToSnd.h"

namespace Path {

PATHFINDERSTATE *pfstates[PATH_MAX_PROJECTS];
PATHFINDERSTATE *pfstate;
SongProgressCallback songprogress;
EventReleaseCallback eventrelease;
EventActionCallback eventaction;
unsigned char inited;
unsigned char paused;
signed char volscale;
char bankservice;
int timercallsinarow;
int lasttimercb;
unsigned int milliseconds;
int debugchannels;
int autoverify;
int defaultfxbus;
PATHMemAllocFunc memalloc;
PATHMemFreeFunc memfree;
int timercalls;
int timertimespent;
int taskcalls;
int tasktimespent;

}

PathSemaphore *pathsemaphore;

int PATHI_lock() {
    unsigned int timeoutImmediate;

    if (Path::inited == 0) {
        return 0;
    }
    timeoutImmediate = 0;
    return pathsemaphore->Wait(timeoutImmediate) == 0;
}

void PATHI_unlock() {
    Path::pfstate = 0;
    pathsemaphore->Post(1);
}

int PATHI_init() {
    unsigned char wasinited;

    if (pathsemaphore == 0) {
        pathsemaphore = new PathSemaphore(1);
    }
    wasinited = Path::inited;
    Path::inited = 1;
    if (PATHI_lock() == 0) {
        Path::inited = wasinited;
        return PATHERR_INUSE;
    }
    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++) {
            Path::pfstates[p] = 0;
        }
    }
    Path::inited = 1;
    Path::timercalls = 0;
    Path::pfstate = 0;
    Path::paused = 0;
    Path::tasktimespent = 0;
    Path::taskcalls = 0;
    Path::timertimespent = 0;
    PATHI_unlock();
    return 0;
}

int PATH_shutdown() {
    int numtracks;

    if (Path::inited != 0) {
        PATH_destroy(PATH_ALL_PROJECTS);
    }
    numtracks = PATH_numtracks(PATH_ALL_PROJECTS);
    if (numtracks == 0 && (Path::inited == 0 || PATHI_lock() != 0)) {
        Path::IPathToReal *deadrealimp;
        Path::IPathToSnd *deadsndimp;

        deadrealimp = Path::IPathToReal::realimp;
        Path::IPathToReal::realimp = 0;
        if (deadrealimp != 0) {
            delete deadrealimp;
        }
        deadsndimp = Path::IPathToSnd::sndimp;
        Path::IPathToSnd::sndimp = 0;
        if (deadsndimp != 0) {
            delete deadsndimp;
        }
        if (Path::inited != 0) {
            PATHI_unlock();
        }
        if (pathsemaphore != 0) {
            pathsemaphore->~PathSemaphore();
            PATHI_memfree(pathsemaphore);
        }
        Path::inited = 0;
        pathsemaphore = 0;
    }
    return Path::inited == 0;
}

void *PATHI_memalloc(int size) {
    return Path::memalloc != 0 ? Path::memalloc(size) : 0;
}

void PATHI_memfree(void *pmem) {
    if (Path::memfree != 0) {
        Path::memfree(pmem);
    }
}

void PATH_callbacks(SongProgressCallback progresscb, EventReleaseCallback eventcb,
                    EventActionCallback actioncb) {
    Path::songprogress = progresscb;
    Path::eventrelease = eventcb;
    Path::eventaction = actioncb;
}

Path::IPathTrack *PATH_gettrackimp(int trackhandle) {
    PATHTRACK *track;

    track = PATHI_gettrackptr(trackhandle);
    if (track == 0) {
        return 0;
    }
    return track->trackimp;
}
