#include "pathi.h"
#include "path/IPathToReal.h"

int PATH_milliseconds() {
    unsigned int ms;

    ms = 0;
    if (Path::IPathToReal::realimp != 0) {
        ms = Path::IPathToReal::realimp->GetMilliseconds();
    }
    return ms;
}

int PATHI_switchproject(int p, int idflags) {
    if (p >= 0 && p < PATH_MAX_PROJECTS && Path::pfstates[p] != 0 &&
        Path::pfstates[p]->pmap != 0) {
        {
            int voiceflags;
            voiceflags = idflags & PATH_ALL_VOICES;
            if (voiceflags != 0 && (Path::pfstates[p]->idflags & voiceflags) == 0) {
                return 0;
            }
            {
                int projectflags;
                projectflags = idflags & PATH_ALL_PROJECTS;
                if ((Path::pfstates[p]->idflags & projectflags) != 0) {
                    Path::pfstate = Path::pfstates[p];
                    return Path::pfstate->idflags;
                }
            }
        }
    }
    return 0;
}

int PATHI_switchvoice(unsigned int voiceflags) {
    int p;
    int hasproject;

    p = 0;
    while (1) {
        {
            int projectflag;
            projectflag = 0;
            if ((voiceflags & PATH_ALL_PROJECTS) == 0) {
                projectflag = 0x01000000 << p;
            }
            hasproject = PATHI_switchproject(p, voiceflags | projectflag);
        }
        if (hasproject != 0) {
            break;
        }
        p++;
        if (p > PATH_MAX_PROJECTS - 1) {
            return 0;
        }
    }
    return 1;
}

void PATHI_sortprojects() {
    int p1;
    int p2;
    PATHFINDERSTATE pfstate1;

    p1 = 0;
    do {
        p2 = p1 + 1;
        if (Path::pfstates[p1] != 0) {
            while (p2 < PATH_MAX_PROJECTS) {
                if (Path::pfstates[p2] != 0 &&
                    Path::pfstates[p2]->idflags < Path::pfstates[p1]->idflags) {
                    pfstate1 = *Path::pfstates[p1];
                    *Path::pfstates[p1] = *Path::pfstates[p2];
                    *Path::pfstates[p2] = pfstate1;
                }
                p2++;
            }
        }
        p1++;
    } while (p1 < PATH_MAX_PROJECTS);
}

void PATHI_serviceproject() {
    PATHTRACK *track;
    int timeremaining;
    unsigned int t, interval;
    char trackplaying;
    char intimer;

    intimer = Path::bankservice == 'B';
    if (intimer != 0) {
        interval = Path::pfstate->timerinterval;
    } else {
        interval = Path::pfstate->taskinterval;
    }
    if (intimer == 0) {
        PATHI_serviceeventqueue();
    }
    t = 0;
    do {
        track = Path::pfstate->track[t];
        if (track != 0 && track->trackimp != 0) {
            trackplaying = 0;
            if (track->node >= 0 && track->entryinfo != 0) {
                trackplaying = 1;
            }
            if (trackplaying != 0) {
                if (track->volumefade.fadenum >= 0) {
                    PATHI_setfadevolume(track);
                }
                if (track->sfxsendfade.fadenum >= 0) {
                    PATHI_setsfxfadevolume(track);
                }
                if (track->drylevelfade.fadenum >= 0) {
                    PATHI_setdrylevelfadevolume(track);
                }
                if (track->pitchfade.fadenum >= 0) {
                    PATHI_setpitchfadevolume(track);
                }
                if (track->stretchfade.fadenum >= 0) {
                    PATHI_setstretchfadevolume(track);
                }
            }
            if (track->pauseat != 0 &&
                track->pauseat <= Path::milliseconds + (interval >> 1)) {
                track->trackimp->Pause(1);
                track->pauseat = 0;
                track->paused = 1;
            }
            if (track->resumeat != 0 &&
                track->resumeat <= Path::milliseconds + (interval >> 1)) {
                track->trackimp->Pause(0);
                track->resumeat = 0;
                track->paused = 0;
            }
            if (intimer == 0 && track->loadingsubbank >= 0) {
                PATHI_subbankready(track, track->loadingsubbank);
            }
            if (track->ramtrack == intimer && trackplaying != 0 && track->paused == 0) {
                timeremaining = 0;
                if (track->loadingsubbank < 0) {
                    timeremaining = PATHI_readyfornewrequest(track);
                }
                if (timeremaining != 0) {
                    timeremaining = PATHI_timeremaining(track);
                    if (track->nextbeattime == 0) {
                        if (timeremaining >= track->latency) {
                            continue;
                        }
                    } else if (track->nextbeattime > Path::milliseconds) {
                        continue;
                    }
                    if (track->volumefade.fadeto == 0 && track->trackimp->GetVolume() == 0) {
                        track->volumefade.fadeto = -1;
                    }
                    PATHI_seeknextnode(t);
                }
            }
        }
        t++;
    } while (t < PATH_MAX_TRACKS);
    if (intimer == 0) {
        while (PATHI_serviceeventqueue() != 0) {
        }
    }
}

void PATHI_service(char isbankservice) {
    unsigned char p;

    if (Path::paused != 0) {
        return;
    }
    if (PATHI_lock() == 0) {
        return;
    }
    Path::bankservice = isbankservice;
    Path::milliseconds = PATH_milliseconds();
    p = 0;
    PATHI_statusall(0);
    do {
        if (PATHI_switchproject(p, -1) != 0) {
            PATHI_getmastertrack();
            PATHI_serviceproject();
        }
        p++;
    } while (p <= PATH_MAX_PROJECTS - 1);
    PATHI_statusall(1);
    PATHI_unlock();
}

void PATHI_servicetask() {
    PATHI_service(' ');
}

void PATHI_servicetimer() {
    int timercb;

    timercb = PATH_milliseconds();
    Path::lasttimercb = (timercb / 10) * 10;
    PATHI_service('B');
}
