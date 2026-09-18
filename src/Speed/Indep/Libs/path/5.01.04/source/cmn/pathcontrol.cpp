#include "pathi.h"

using namespace Path;

int PATH_control(int tracks, unsigned int controller)
{
    int result;

    if (!PATHI_lock())
        return PATHERR_INUSE;

    result = PATHERR_INV_PARAM;

    if (controller > 0x7F)
        goto abort;

    for (int p = 0; p < PATH_MAX_PROJECTS; p++)
    {
        if (PATHI_switchproject((unsigned char)p, tracks))
        {
            for (int t = 0; t < PATH_MAX_TRACKS; t++)
            {
                PATHTRACK *track = pfstate->track[t];
                if (track == 0)
                    continue;
                if (!(((unsigned int)tracks >> t) & 1))
                    continue;
                {
                    track->control = controller;
                    result = PATH_OK;
                }
            }
        }
    }

abort:
    PATHI_unlock();

    return result;
}

int PATH_pause(int tracks, unsigned char pause)
{
    int result;

    if (!PATHI_lock())
        return PATHERR_INUSE;

    result = PATHERR_INV_PARAM;

    if (tracks == PATH_ALL)
        paused = pause;

    for (int p = 0; p < PATH_MAX_PROJECTS; p++)
    {
        if (PATHI_switchproject((unsigned char)p, tracks))
        {
            for (int t = 0; t < PATH_MAX_TRACKS; t++)
            {
                PATHTRACK *track = pfstate->track[t];
                if (track == 0)
                    continue;
                if (!(((unsigned int)tracks >> t) & 1))
                    continue;
                {
                    result = PATH_OK;
                    track->trackimp->Pause(pause);
                    track->paused = pause;
                }
            }
        }
    }
    PATHI_unlock();

    return result;
}

PATHTRACKPLAYSTATUS PATH_trackstatus(unsigned int trackhandle)
{
    return PATHI_trackstatus(PATHI_gettrackptr(trackhandle));
}

int PATH_status(int trackhandle, PATHSTATUS *psps)
{
    int currentrequest;
    PATHTRACK *track;

    if (!PATHI_lock())
        return PATHERR_INUSE;

    currentrequest = PATHERR_INV_PARAM;

    track = PATHI_gettrackptr(trackhandle);
    if (!track)
        goto abort;

    if (!PATHI_switchvoice(trackhandle))
        goto abort;

    currentrequest = PATHI_status(track, psps);

abort:
    PATHI_unlock();

    return currentrequest;
}

int PATHI_status(PATHTRACK *track, PATHSTATUS *psps)
{
    if (track->trackimp == 0)
        return PATHERR_CANTOPEN;

    return track->trackimp->GetPathStatus(psps);
}

int PATH_stop(int tracks)
{
    int result;

    if (!PATHI_lock())
        return PATHERR_INUSE;

    result = PATHERR_INV_PARAM;

    for (int p = 0; p < PATH_MAX_PROJECTS; p++)
    {
        if (PATHI_switchproject(p, tracks))
        {
            for (int t = 0; t < PATH_MAX_TRACKS; t++)
            {
                PATHTRACK *track = pfstate->track[t];
                if (track == 0)
                    continue;
                if (!(((unsigned int)tracks >> t) & 1))
                    continue;
                {
                    result = PATHI_stop(track);
                }
            }
        }
    }
    PATHI_unlock();

    return result;
}

int PATHI_stop(PATHTRACK *track)
{
    track->trackimp->Stop();
    track->node = -1;
    track->nodebeat = -1;

    return PATH_OK;
}
