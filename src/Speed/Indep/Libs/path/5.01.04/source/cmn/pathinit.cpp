#include "pathi.h"
#include "path/IPathToReal.h"
#include "path/IPathToSnd.h"
#include <string.h>

namespace Path {

PATHFINDERSTATE *pfstates[PATH_MAX_PROJECTS] = {0};
PATHFINDERSTATE *pfstate = 0;
SongProgressCallback songprogress = 0;
EventReleaseCallback eventrelease = 0;
EventActionCallback eventaction = 0;
unsigned char inited = 0;
unsigned char paused = 0;
signed char volscale = 0;
char bankservice = 0;
int timercallsinarow = 0;
int lasttimercb = 0;
unsigned int milliseconds = 0;
int debugchannels = 0;
int autoverify = 0;
int defaultfxbus = 0;
PATHMemAllocFunc memalloc = 0;
PATHMemFreeFunc memfree = 0;
int timercalls = 0;
int timertimespent = 0;
int taskcalls = 0;
int tasktimespent = 0;

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

int PATH_addmapfile(char *pmap) {
    int p;
    int e;
    int slot;
    int projectID;
    int voiceID;
    int result;
    PATHFINDHEADER *header;

    if (Path::inited == 0) {
        PATHI_init();
    }
    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    {
        slot = PATH_MAX_PROJECTS;
        projectID = static_cast<unsigned char>(pmap[12]);
        result = -1;
        voiceID = 0;
        p = 0;
        for (e = 0; e < PATH_MAX_PROJECTS; e++) {
            if (Path::pfstates[p] == 0) {
                if (slot == PATH_MAX_PROJECTS) {
                    slot = p;
                }
            }
            else {
                if (Path::pfstates[p]->pmap->projectID == projectID) {
                    if (reinterpret_cast<PATHFINDHEADER *>(pmap)->generateID !=
                        Path::pfstates[p]->pmap->generateID) {
                        goto abort;
                    }
                    voiceID++;
                }
            }
            p++;
        }
        if (slot < PATH_MAX_PROJECTS &&
            *reinterpret_cast<unsigned int *>(pmap) == 0x50464478 &&
            (*reinterpret_cast<unsigned int *>(pmap + 4) & 0xffff0000) == 0x05010000) {
            *(slot + Path::pfstates) = static_cast<PATHFINDERSTATE *>(PATHI_memalloc(sizeof(PATHFINDERSTATE)));
            if (*(slot + Path::pfstates) != 0) {
                memset(*(slot + Path::pfstates), 0, sizeof(PATHFINDERSTATE));
                (*(slot + Path::pfstates))->pmap = reinterpret_cast<PATHFINDHEADER *>(pmap);
                (*(slot + Path::pfstates))->idflags =
                    (0x10000000 << (voiceID & 0x3f)) | (0x01000000 << (projectID & 0x3f));
                PATHI_switchproject(slot, (*(slot + Path::pfstates))->idflags);
                e = 0;
                result = 0;
                Path::pfstate->pnodeoffsets = reinterpret_cast<short *>(pmap + Path::pfstate->pmap->nodeoffsets);
                Path::pfstate->pnodes = reinterpret_cast<PATHFINDNODE *>(pmap + Path::pfstate->pmap->nodedata);
                Path::pfstate->peventoffsets = reinterpret_cast<short *>(pmap + Path::pfstate->pmap->eventoffsets);
                Path::pfstate->pevents = reinterpret_cast<PATHEVENT *>(pmap + Path::pfstate->pmap->eventdata);
                Path::pfstate->pnamedvars = reinterpret_cast<PATHNAMEDVAR *>(pmap + Path::pfstate->pmap->namedvars);
                Path::pfstate->prouters = reinterpret_cast<int *>(pmap + Path::pfstate->pmap->noderouters);
                Path::pfstate->ptrackoffsets = reinterpret_cast<int *>(pmap + Path::pfstate->pmap->trackoffsets);
                Path::pfstate->ptrackinfos = reinterpret_cast<PATHTRACKINFO *>(pmap + Path::pfstate->pmap->trackinfos);
                Path::pfstate->psampleoffsets =
                    reinterpret_cast<PATHFINDSAMPLE *>(pmap + Path::pfstate->pmap->sampleoffsets);
                for (e = 0; e < 16; e++) {
                    Path::pfstate->eventqueue[e] = 0;
                }
                Path::pfstate->timerinterval = 10;
                Path::pfstate->taskinterval = 0x32;
                result = projectID;
            }
        }
    abort:
        PATHI_unlock();
    }
    return result;
}

void PATH_callbacks(SongProgressCallback progresscb, EventReleaseCallback eventcb,
                    EventActionCallback actioncb) {
    Path::songprogress = progresscb;
    Path::eventrelease = eventcb;
    Path::eventaction = actioncb;
}

int PATH_destroy(int trackhandle) {
    PATHTRACK *track;
    int numdestroyed;

    if (Path::inited == 0) {
        return 0;
    }
    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    numdestroyed = 0;
    {
        for (int p = 0; p < PATH_MAX_PROJECTS; p++) {
            if (PATHI_switchproject(p & 0xff, trackhandle) != 0) {
                for (int t = 0; t < PATH_MAX_TRACKS; t++) {
                    track = Path::pfstate->track[t];
                    if (track != 0 && (((static_cast<unsigned int>(trackhandle) >> t) ^ 1) & 1) == 0) {
                        Path::pfstate->track[t] = 0;
                        if (track->trackimp != 0) {
                            delete track->trackimp;
                        }
                        track->trackimp = 0;
                        numdestroyed++;
                        PATHI_memfree(track);
                    }
                }
                int numtracks;

                numtracks = PATH_numtracks(0x1000000 << p);
                if (numtracks == 0) {
                    PATHI_memfree(Path::pfstate);
                    Path::pfstates[p] = 0;
                }
            }
        }
    }
    PATHI_sortprojects();
    PATHI_unlock();
    return numdestroyed;
}

int PATH_setnamedvalue(int projects, char *name, int value) {
    int result;
    char str[16];

    if (name == 0) {
        return PATHERR_INV_PARAM;
    }
    result = PATHERR_INV_PARAM;
    {
        int c;

        c = 0;
        do {
            str[c] = name[c];
            if (str[c] == 0) {
                break;
            }
            str[c] = str[c] | 0x20;
            c++;
            if (c > 15) {
                break;
            }
        } while (1);
    }
    projects &= PATH_ALL_PROJECTS;
    {
        for (int p = 0; p < PATH_MAX_PROJECTS; p++) {
            if (*(Path::pfstates + p) != 0 && (*(Path::pfstates + p))->pmap != 0 &&
                ((*(Path::pfstates + p))->idflags & projects) != 0) {
                int v;

                for (v = 0; v < (*(Path::pfstates + p))->pmap->numnamedvars; v++) {
                    if ((*(Path::pfstates + p))->pnamedvars[v].name[0] != 0 &&
                        strcmp(str, (*(Path::pfstates + p))->pnamedvars[v].name) == 0) {
                        result = 0;
                        (*(Path::pfstates + p))->pnamedvars[v].value = value;
                    }
                }
            }
        }
    }
    return result;
}

Path::IPathTrack *PATH_gettrackimp(int trackhandle) {
    PATHTRACK *track;

    track = PATHI_gettrackptr(trackhandle);
    if (track == 0) {
        return 0;
    }
    return track->trackimp;
}

int PATHI_bytesperms(int trackID) {
    PATHFINDSAMPLE *sample;
    PATHFINDSAMPLE *endsample = 0;
    float bytesperms;
    float byterate = 0.0f;
    PATHTRACKINFO *trackinfo;

    trackinfo = PATHI_gettrackinfo(trackID);
    if (trackinfo == 0) {
        return 0;
    }
    sample = Path::pfstate->psampleoffsets + trackinfo->startingsample;
    {
        PATHTRACKINFO *nexttrackinfo;

        nexttrackinfo = PATHI_gettrackinfo(trackID + 1);
        if (nexttrackinfo != 0) {
            endsample = Path::pfstate->psampleoffsets + nexttrackinfo->startingsample;
        }
        else {
            endsample = reinterpret_cast<PATHFINDSAMPLE *>(
                reinterpret_cast<char *>(Path::pfstate->pmap) + Path::pfstate->pmap->mapfilelen);
        }
    }
    if (sample < endsample - 1) {
        int length;

        for (; sample < endsample - 1; sample++) {
            length = (sample[1].offset - sample->offset) * 128;
            bytesperms = static_cast<float>(length) / static_cast<float>(sample->duration);
            if (byterate < bytesperms) {
                byterate = bytesperms;
            }
        }
    }
    return static_cast<int>(byterate + 0.5);
}
