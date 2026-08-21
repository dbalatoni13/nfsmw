#include "pathi.h"
#include "path/IPathToReal.h"
#include "path/IPathToSnd.h"

Path::IPathToReal *Path::IPathToReal::realimp = 0;
Path::IPathToSnd *Path::IPathToSnd::sndimp = 0;

int PATH_control(int tracks, unsigned int controller) {
    int result;

    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    result = PATHERR_INV_PARAM;
    if (controller >= 0x80) {
        goto abort;
    }

    {
        int p;
        p = 0;
        do {
            int t;
            if (PATHI_switchproject(p & 0xff, tracks) != 0) {
                t = 0;
                do {
                    PATHTRACK *track;
                    track = Path::pfstate->track[t];
                    if (track != 0 &&
                        ((((static_cast<unsigned int>(tracks) >> t) ^ 1) & 1) == 0)) {
                        track->control = controller;
                        result = PATH_OK;
                    }
                } while (++t < PATH_MAX_TRACKS);
            }
            p++;
        } while (p < PATH_MAX_PROJECTS);
    }

abort:
    PATHI_unlock();
    return result;
}

int PATH_pause(int tracks, unsigned char pause) {
    int result;

    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    result = PATHERR_INV_PARAM;
    if (tracks == -1) {
        Path::paused = pause;
    }

    {
        int p;
        p = 0;
        do {
            int t;
            if (PATHI_switchproject(p & 0xff, tracks) != 0) {
                for (t = 0; t < PATH_MAX_TRACKS; t++) {
                    PATHTRACK *track;
                    track = Path::pfstate->track[t];
                    if (track != 0 &&
                        ((((static_cast<unsigned int>(tracks) >> t) ^ 1) & 1) == 0)) {
                        track->trackimp->Pause(pause);
                        track->paused = pause;
                        result = PATH_OK;
                    }
                }
            }
            p++;
        } while (p < PATH_MAX_PROJECTS);
    }
    PATHI_unlock();
    return result;
}

PATHTRACKPLAYSTATUS PATH_trackstatus(unsigned int trackhandle) {
    PATHTRACK *track;

    track = PATHI_gettrackptr(trackhandle);
    return PATHI_trackstatus(track);
}

int PATH_status(int trackhandle, PATHSTATUS *psps) {
    int currentrequest;
    PATHTRACK *track;

    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    currentrequest = PATHERR_INV_PARAM;
    track = PATHI_gettrackptr(trackhandle);
    if (track != 0 && PATHI_switchvoice(trackhandle) != 0) {
        currentrequest = PATHI_status(track, psps);
    }

abort:
    PATHI_unlock();
    return currentrequest;
}

int PATHI_status(PATHTRACK *track, PATHSTATUS *psps) {
    if (track->trackimp == 0) {
        return PATHERR_CANTOPEN;
    }
    return track->trackimp->GetPathStatus(psps);
}

int PATH_stop(int tracks) {
    int result;

    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    result = PATHERR_INV_PARAM;
    {
        int p;
        p = 0;
        do {
            int t;
            if (PATHI_switchproject(p, tracks) != 0) {
                for (t = 0; t < PATH_MAX_TRACKS; t++) {
                    PATHTRACK *track;
                    track = Path::pfstate->track[t];
                    if (track != 0 &&
                        ((((static_cast<unsigned int>(tracks) >> t) ^ 1) & 1) == 0)) {
                        result = PATHI_stop(track);
                    }
                }
            }
            p++;
        } while (p < PATH_MAX_PROJECTS);
    }
    PATHI_unlock();
    return result;
}

int PATHI_stop(PATHTRACK *track) {
    track->trackimp->Stop();
    track->node = -1;
    track->nodebeat = -1;
    return PATH_OK;
}
