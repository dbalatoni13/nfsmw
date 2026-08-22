#include "pathi.h"
#include "path/IPathToReal.h"

#include <string.h>

int PATHI_addevent(unsigned int projectflags, PATHEVENT *event) {
    int project;
    int eventindex;
    int result;
    PATHACTION *action;
    unsigned int a;
    unsigned int indent;

    a = 0;
    result = PATHERR_INV_PARAM;
    action = reinterpret_cast<PATHACTION *>(event + 1);
    indent = 0;
    projectflags &= 0xff000000;
    for (a = 0; a < event->numactions; a++, action++) {
        if (action->type == 1) {
            if (action->assess > 3) {
                if (static_cast<int>(action->assess) == 4) {
                    indent--;
                    action->indent = indent;
                } else {
                    action->indent = indent;
                }
            } else {
                if (action->assess < 2) {
                    action->indent = indent;
                    indent++;
                } else {
                    action->indent = indent - 1;
                }
            }
        } else {
            action->indent = indent;
        }
    }
    for (project = 0; project < 4; project++) {
        if (Path::pfstates[project] != 0 &&
            (Path::pfstates[project]->idflags & projectflags & 0xff000000) ==
                Path::pfstates[project]->idflags) {
            eventindex = Path::pfstates[project]->eventindex;
            if (eventindex > 15) {
                return PATHERR_TOOMANY;
            }
            result = 0;
            event->lastact = Path::IPathToReal::realimp->GetMilliseconds();
            event->queued = event->lastact;
            Path::pfstates[project]->eventqueue[eventindex++] = event;
            Path::pfstates[project]->eventindex = eventindex;
        }
    }
    return result;
}

int PATH_event(int tracks, unsigned int eventID) {
    int result;
    unsigned int projectflags;
    PATHEVENT *copyeventp;

    if (PATHI_lock() == 0) {
        result = PATHERR_INUSE;
    } else {
        result = PATHERR_INV_PARAM;
        copyeventp = 0;
        projectflags = eventID & 0xf000000;
        eventID &= 0xffffff;
        {
            unsigned int p;

            for (p = 0; p < 4; p++) {
                if (PATHI_switchproject(static_cast<unsigned char>(p), tracks) != 0) {
                    PATHEVENT *eventp;

                    if ((projectflags & (0x1000000 << Path::pfstate->pmap->projectID)) != 0) {
                        eventp = PATHI_getevent(eventID, 0xffffffff);
                        if (eventp != 0) {
                            if (eventp->beingFiltered == 0) {
                                if (copyeventp != 0 || (copyeventp = PATHI_copyevent(eventp)) != 0) {
                                    result = PATHI_addevent(Path::pfstates[p]->idflags, copyeventp);
                                }
                            } else {
                                result = PATHERR_INV_PARAM;
                            }
                        }
                    }
                }
            }
        }
        PATHI_unlock();
    }
    return result;
}

int PATH_clearallevents(int projects) {
    int result;

    if (PATHI_lock() == 0) {
        result = PATHERR_INUSE;
    } else {
        result = PATHERR_INV_PARAM;
        {
            int p;

            for (p = 0; p < 4; p++) {
                if (PATHI_switchproject(static_cast<unsigned char>(p), projects) != 0) {
                    result = 0;
                    {
                        int i;

                        for (i = 0; i < 16; i++) {
                            if (Path::pfstate->eventqueue[i] != 0) {
                                PATHI_releaseevent(i, PATHEVENT_PURGED);
                                i--;
                            }
                        }
                    }
                }
            }
        }
        PATHI_unlock();
    }
    return result;
}

PATHEVENT *PATHI_copyevent(PATHEVENT *event) {
    PATHEVENT *minevent;
    PATHEVENT *maxevent;
    PATHEVENT *thisevent;
    unsigned int e;
    unsigned int eventsize;
    unsigned int thiseventsize;

    eventsize = sizeof(PATHEVENT) + event->numactions * 12;
    minevent = reinterpret_cast<PATHEVENT *>(Path::pfstate->eventheap);
    maxevent = reinterpret_cast<PATHEVENT *>(&Path::pfstate->eventindex);
    for (e = 0; e < 16; e++) {
        thisevent = Path::pfstate->eventqueue[e];
        if (thisevent >= minevent && thisevent < maxevent) {
            thiseventsize = sizeof(PATHEVENT) + thisevent->numactions * 12;
            minevent = reinterpret_cast<PATHEVENT *>(reinterpret_cast<char *>(thisevent) + thiseventsize);
        }
    }
    if (reinterpret_cast<char *>(minevent) + eventsize < reinterpret_cast<char *>(maxevent)) {
        memcpy(minevent, event, eventsize);
        minevent->project = Path::pfstate->idflags;
        return minevent;
    }
    return 0;
}

void PATHI_removeevent(PATHEVENT *event) {
    {
        int p;

        for (p = 0; p < 4; p++) {
            PATHFINDERSTATE *pfs;

            pfs = Path::pfstates[p];
            if (pfs != 0) {
                int e;

                for (e = 0; e <= 16; e++) {
                    if (pfs->eventqueue[e] == event) {
                        pfs->eventqueue[e] = 0;
                        e++;
                        pfs->eventindex--;
                        for (; e < 16; e++) {
                            pfs->eventqueue[e - 1] = pfs->eventqueue[e];
                        }
                    }
                }
            }
        }
    }
}

void PATHI_moveevent(PATHEVENT *event, PATHEVENT *newlocation) {
    {
        int p;

        for (p = 0; p <= 3; p++) {
            PATHFINDERSTATE *pfs;

            pfs = Path::pfstates[p];
            if (pfs != 0) {
                int e;

                for (e = 0; e <= 16; e++) {
                    if (pfs->eventqueue[e] == event) {
                        pfs->eventqueue[e] = newlocation;
                    }
                }
            }
        }
    }
}

void PATHI_releaseevent(int e, PATHEVENTRESULT result) {
    PATHEVENT *minevent;
    PATHEVENT *maxevent;
    PATHEVENT *event;
    PATHEVENT *thisevent;
    PATHEVENT copyevent;
    unsigned int z;
    unsigned int eventsize;
    unsigned int movesize;
    bool tidyheap;

    minevent = reinterpret_cast<PATHEVENT *>(Path::pfstate->eventheap);
    maxevent = reinterpret_cast<PATHEVENT *>(&Path::pfstate->eventindex);
    event = Path::pfstate->eventqueue[e];
    eventsize = sizeof(PATHEVENT) + event->numactions * 12;
    copyevent = *event;
    tidyheap = event >= minevent && event < maxevent;
    if (tidyheap) {
        movesize = reinterpret_cast<char *>(maxevent) - reinterpret_cast<char *>(event);
        memcpy(event, reinterpret_cast<char *>(event) + eventsize, movesize);
        for (z = 0; z <= 16; z++) {
            thisevent = Path::pfstate->eventqueue[z];
            if (thisevent > event && thisevent < maxevent) {
                thisevent = reinterpret_cast<PATHEVENT *>(reinterpret_cast<char *>(thisevent) - eventsize);
                PATHI_moveevent(Path::pfstate->eventqueue[z], thisevent);
                if (minevent <= thisevent) {
                    unsigned int thiseventsize;

                    thiseventsize = sizeof(PATHEVENT) + thisevent->numactions * 12;
                    minevent = reinterpret_cast<PATHEVENT *>(reinterpret_cast<char *>(thisevent) + thiseventsize);
                }
            }
        }
    } else if (Path::eventrelease != 0 && event->project < 0) {
        Path::eventrelease(event, result);
    }
    PATHI_removeevent(event);
}

void PATHI_seteventfilter(PATHEVENT *in_event, int onOff) {
    PATHEVENT *event;

    event = PATHI_getevent(in_event->eventID, 0xffffff);
    if (event == 0) {
        return;
    }
    event->beingFiltered = onOff;
}

void PATHI_clearalleventfilters() {
    int i;
    PATHEVENT *event;

    for (i = 0; i < Path::pfstate->pmap->numevents; i++) {
        event = reinterpret_cast<PATHEVENT *>(reinterpret_cast<char *>(Path::pfstate->pmap) +
                                              Path::pfstate->peventoffsets[i] * 4);
        if (event != 0) {
            event->beingFiltered = 0;
        }
    }
}

int PATHI_serviceevent(int eventindex) {
    PATHEVENT *event;
    PATHACTION *action;
    int actionstaken;
    unsigned int a;

    actionstaken = 0;
    event = Path::pfstate->eventqueue[eventindex];
    if (event->lastact > Path::milliseconds) {
        return 0;
    }
    a = event->currentaction;
    action = reinterpret_cast<PATHACTION *>(event + 1) + a;
    if (a < event->numactions) {
        do {
            if (action->done == 0 && PATHI_serviceaction(event, action) == 0) {
                break;
            }
            actionstaken++;
            event->currentaction++;
            event->lastact = Path::milliseconds;
            action++;
            a++;
        } while (a < event->numactions);
    }
    return actionstaken;
}

int PATHI_serviceeventqueue() {
    int e;
    int actionstaken;
    int endofqueue;
    PATHEVENT *event;
    PATHEVENTRESULT eventresult;

    actionstaken = 0;
    endofqueue = 0;
    for (e = 0; e < 16; e++) {
        if (e >= Path::pfstate->eventindex) {
            Path::pfstate->eventqueue[e] = 0;
        }
        event = Path::pfstate->eventqueue[e];
        if (event != 0) {
            eventresult = PATHEVENT_PENDING;
            if (event->currentaction >= event->numactions) {
                eventresult = PATHEVENT_FINISHED;
            }
            if (event->expiry != 0 && event->queued + event->expiry < Path::milliseconds) {
                eventresult = PATHEVENT_EXPIRED;
            }
            if (event->priority != 0 && PATHI_eventtakespriority(e) == 0) {
                eventresult = PATHEVENT_PURGED;
            }
            if (eventresult != PATHEVENT_PENDING) {
                PATHI_releaseevent(e, eventresult);
                e--;
            } else {
                actionstaken += PATHI_serviceevent(e);
            }
        }
    }
    return actionstaken;
}

int PATHI_eventtakespriority(int e) {
    PATHEVENT *event;
    int testpriority;
    int priority;
    int bumpsame;
    int takespriority;

    event = Path::pfstate->eventqueue[e];
    priority = event->priority;
    bumpsame = priority % 2;
    if (priority <= 0) {
        priority = -priority;
    }
    for (e++; e < 16; e++) {
        PATHEVENT *testevent;

        testevent = Path::pfstate->eventqueue[e];
        if (testevent != 0 && testevent->priority != 0) {
            int testbumpsame;

            testpriority = testevent->priority;
            if (testpriority < 0) {
                testpriority = -testpriority;
            }
            if (event->priority > 0 && testpriority < priority + bumpsame) {
                PATHI_releaseevent(e, PATHEVENT_PURGED);
                e--;
            } else if ((testevent->priority < 0 || testpriority > priority) &&
                       priority < testpriority + (testbumpsame = testpriority % 2)) {
                return 0;
            }
        }
    }
    return 1;
}
