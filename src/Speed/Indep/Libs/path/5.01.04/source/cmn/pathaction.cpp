#include "pathi.h"

using namespace Path;


int PATHI_serviceaction(PATHEVENT *event, PATHACTION *action) {
    PATHTRACK *track;
    PATHTRACK savetrack;
    PATHFINDNODE *nodeinfo;
    PATHEVENT *neweventp;
    PATHACT act = action->act;
    PATHBEATINFO beatinfo;
    PATHTRACKPLAYSTATUS trackstatus;
    int n;
    int fadetime;
    int currentnode;
    int nodeID;
    int sectionID;
    int savenumevents;
    int value;
    int compareValue;
    int setwhat;
    int towhat;
    int ms;
    int result;
    static unsigned int lastwhile;
    static unsigned int lastendif;
    int fadebits;
    int pitchbits;

    savenumevents = Path::pfstate->eventindex;
    fadebits = *reinterpret_cast<int *>(&action->act) << 8;
    pitchbits = *reinterpret_cast<int *>(&action->act) << 14;
    {
        int t;
        {
            unsigned int trackhandle;
            int trackFlag;
            unsigned int voice;
            bool mainvoice;
            bool auxvoice;
            for (t = 0; t < PATH_MAX_TRACKS; t++) {
                trackFlag = 1 << t;
                if ((static_cast<unsigned int>(action->track) & trackFlag) == 0) {
                    continue;
                }
                trackhandle = ((static_cast<unsigned int>(action->track) & 0x0f000000) | 0xf0000000) &
                              Path::pfstate->idflags;
                trackhandle |= trackFlag;
                track = PATHI_gettrackptr(trackhandle);
                if (track == 0) {
                    continue;
                }
                if (track->trackimp == 0) {
                    continue;
                }
                mainvoice = ((static_cast<unsigned int>(action->track) >> 28) & 1) != 0;
                voice = (static_cast<unsigned int>(action->track) & 0xe0000000) != 0;
                if (mainvoice && track->mainvoice == 0) {
                    continue;
                }
                if (voice != 0 && mainvoice == 0 && track->mainvoice != 0) {
                    continue;
                }

                switch (action->type) {
                case 2:
                    value = PATHI_getvalue(act.waittime.millisecs, action->rightvaluetype, track, event);
                    if (act.waittime.lowest > 0 && act.waittime.lowest < value) {
                        value = 0;
                    }
                    if (action->rightvaluetype == PATH_VALUE_SPECIAL) switch (act.waittime.millisecs) {
                    case 0x14:
                    case 0x15:
                    case 0x16:
                        if (action->act.waittime.lowest == 0 ||
                            action->act.waittime.lowest > value) {
                            action->act.waittime.lowest = value;
                        }
                        if (value > 0) {
                            value = Path::milliseconds + 0xbb8;
                        }
                        break;
                    }
                    ms = Path::IPathToReal::realimp->GetMilliseconds();
                    if (value > static_cast<int>(ms - event->lastact)) {
                        break;
                    }
                    action->act.waittime.lowest = 0;
                    action->done = 1;
                    break;
                case 3:
                    if (act.waitbeat.millisecs == 0) {
                        track->trackimp->UpdateStatus();
                        if (PATHI_beatinfo(track, &beatinfo) >= 0) {
                            ms = PATHI_calcwaitbeat(act.waitbeat.every, 1 << act.waitbeat.note,
                                                     act.waitbeat.offset, &beatinfo);
                            action->act.waitbeat.millisecs = ms;
                        } else {
                            action->done = 1;
                        }
                    } else if (act.waitbeat.millisecs <=
                               static_cast<int>(Path::IPathToReal::realimp->GetMilliseconds() - event->lastact)) {
                        action->act.waitbeat.millisecs = -1;
                        action->done = 1;
                    }
                    break;
                case 4:
                    result = PATHERR_PENDING;
                    savetrack = *track;
                    nodeID = PATHI_getvalue(act.branch.node, action->leftvaluetype, track, event);
                    sectionID = PATHI_getvalue(act.branch.ofsection, action->rightvaluetype, track, event);
                    if (nodeID < -1 || nodeID > Path::pfstate->pmap->numnodes) {
                        if ((Path::debugchannels & kEventDebugChannel) != 0) {
                            PATHI_printf("( %.8s )  PATHACTION_BRANCHTO bad node ID = %d, \n",
                                         track->trackname, nodeID);
                        }
                        nodeID = -1;
                    }
                    nodeinfo = 0;
                    if (nodeID >= 0) {
                        if (sectionID < 0) {
                            nodeinfo = PATHI_getnode(nodeID);
                        } else {
                            for (n = 0; n < Path::pfstate->pmap->numnodes; n++) {
                                nodeinfo = PATHI_getnode(n);
                                if (nodeinfo->sectionID == sectionID &&
                                    nodeinfo->index == 0) {
                                    nodeID--;
                                    if (nodeID == 0) {
                                        nodeID = n;
                                        break;
                                    }
                                }
                            }
                            if (n >= Path::pfstate->pmap->numnodes) {
                                nodeID = -1;
                                nodeinfo = 0;
                            }
                        }
                        if (nodeinfo == 0 || nodeinfo->trackID != track->trackID) {
                            continue;
                        }
                    }
                    if (track->nobranch != 0) {
                        continue;
                    }
                    currentnode = track->node;
                    if (act.branch.immediate != 0) {
                        PATHI_stop(track);
                    }
                    trackstatus = static_cast<PATHTRACKPLAYSTATUS>(PATHI_readyfornewrequest(track));
                    if (trackstatus != PATHTRACK_INVALID) {
                        track->nodebeat = -1;
                        result = PATHI_enternode(currentnode, nodeID, track->control, 1);
                        track->node = static_cast<short>(result);
                        result = PATHI_queuenode(track);
                    }
                    if (result >= 0) {
                        action->done = 1;
                    } else {
                        Path::pfstate->eventindex = savenumevents;
                        *track = savetrack;
                        continue;
                    }
                    break;
                case 5:
                    {
                        int fadevalue = PATHI_getvalue(act.fade.ms, action->rightvaluetype, track, event);
                        fadetime = fadebits >> 25;
                        PATHI_fade(track, act.fade.tovol, fadevalue, fadetime);
                        action->done = 1;
                        break;
                    }
                case 7:
                    {
                        int fadevalue = PATHI_getvalue(act.sfxfade.ms, action->rightvaluetype, track, event);
                        fadetime = fadebits >> 25;
                        PATHI_customsfxfade(track, act.sfxfade.tovol, fadevalue, fadetime);
                        action->done = 1;
                        break;
                    }
                case 6:
                    {
                        int fadevalue = PATHI_getvalue(act.dryfade.ms, action->rightvaluetype, track, event);
                        fadetime = fadebits >> 25;
                        PATHI_customdrylevelfade(track, act.dryfade.tovol, fadevalue, fadetime);
                        action->done = 1;
                        break;
                    }
                case 17:
                    {
                        int fadevalue = PATHI_getvalue(act.pitchfade.ms, action->rightvaluetype, track, event);
                        fadetime = pitchbits >> 29;
                        PATHI_custompitchfade(track, act.pitchfade.tovol, fadevalue, fadetime);
                        action->done = 1;
                        break;
                    }
                case 18:
                    {
                        int fadevalue = PATHI_getvalue(act.stretchfade.ms, action->rightvaluetype, track, event);
                        fadetime = pitchbits >> 29;
                        PATHI_customstretchfade(track, act.stretchfade.tovol, fadevalue, fadetime);
                        action->done = 1;
                        break;
                    }
                case 9:
                    {
                        int eventID;
                        int eventIDMask;
                        eventID = act.event.eventid;
                        int eventValue = PATHI_getvalue(eventID, action->leftvaluetype, track, event);
                        eventIDMask = -1;
                        if (eventValue != eventID) {
                            eventIDMask = 0xffff;
                        }
                        neweventp = PATHI_getevent(eventValue, eventIDMask);
                        if (neweventp != 0) {
                            neweventp = PATHI_copyevent(neweventp);
                            if (neweventp != 0) {
                                PATHI_addevent(Path::pfstate->idflags | 0xf0000000, neweventp);
                            }
                        }
                    }
                    action->done = 1;
                    break;
                case 13:
                    {
                        int cbID;
                        cbID = PATHI_getvalue(act.callback.id, action->rightvaluetype, track, event);
                        value = PATHI_getvalue(act.callback.value, action->leftvaluetype, track, event);
                        if (Path::eventaction != 0) {
                            Path::eventaction(trackhandle, cbID, value);
                        }
                    }
                    action->done = 1;
                    break;
                case 10:
                case 11:
                case 12:
                    if (act.filter.eventid == 0) {
                        PATHI_clearalleventfilters();
                    } else {
                        neweventp = PATHI_getevent(act.filter.eventid, 0xffffffff);
                        if (neweventp != 0) {
                            PATHI_seteventfilter(neweventp, act.filter.beingFiltered);
                        }
                    }
                    action->done = 1;
                    break;
                case 8:
                    towhat = PATHI_getvalue(act.setval.towhat, action->rightvaluetype, track, event);
                    action->done = 1;
                    setwhat = act.setval.setwhat;
                    PATHI_setvalue(setwhat, action->leftvaluetype, towhat, track, event);
                    break;
                case 14:
                    {
                        value = PATHI_getvalue(act.calc.value, action->leftvaluetype, track, event);
                        int calcby = PATHI_getvalue(act.calc.by, action->rightvaluetype, track, event);
                        action->done = 1;
                        switch (act.calc.op) {
                        case 1:
                            value += calcby;
                            break;
                        case 2:
                            value -= calcby;
                            break;
                        case 3:
                            value *= calcby;
                            break;
                        case 4:
                            value /= calcby;
                            break;
                        case 5:
                            value %= calcby;
                            break;
                        default:
                            break;
                        }
                        PATHI_setvalue(act.calc.value, action->leftvaluetype, value, track, event);
                        break;
                    }
                case 15:
                    value = PATHI_getvalue(act.pause.when, action->rightvaluetype, track, event);
                    action->done = 1;
                    if (value < 1) {
                        track->paused = act.pause.on;
                        track->trackimp->Pause(act.pause.on);
                    } else if (act.pause.on != 0) {
                        track->pauseat = Path::milliseconds + value;
                    } else {
                        track->resumeat = Path::milliseconds + value;
                    }
                    break;
                case 16:
                    value = PATHI_getvalue(act.loadbank.subbanknum, action->rightvaluetype, track, event);
                    action->done = 1;
                    if (act.loadbank.unload != 0) {
                        PATHI_unloadbank(track, value);
                    } else {
                        PATHI_loadbank(track, value);
                    }
                    break;
                case 1:
                    if (action->assess == 3) {
                        action->done = 1;
                        return 1;
                    }
                    if (action->assess == 4) {
                        action->done = 1;
                        if (PATHI_restoretolastwhile(action, event) != 0) {
                            if (event->lastact == Path::milliseconds) {
                                event->lastact++;
                            }
                            action->done = 0;
                            lastendif = Path::milliseconds;
                        }
                        return action->done;
                    }
                    value = PATHI_getvalue(act.only.value, action->leftvaluetype, track, event);
                    compareValue = PATHI_getvalue(act.only.compareValue,
                                                  action->rightvaluetype, track, event);
                    if (action->leftvaluetype == PATH_VALUE_SPECIAL &&
                        act.only.value == PATH_PLAYSTATUS) {
                        trackstatus = PATHI_trackstatus(track);
                        if (compareValue == PATHTRACK_READYFORNEXT &&
                            trackstatus == PATHTRACK_STOPPED) {
                            action->done = 1;
                        }
                    }
                    switch (action->comparison) {
                    case 1:
                        action->done = value == compareValue;
                        break;
                    case 2:
                        action->done = value != compareValue;
                        break;
                    case 3:
                        action->done = value > compareValue;
                        break;
                    case 4:
                        action->done = value < compareValue;
                        break;
                    case 5:
                        action->done = value >= compareValue;
                        break;
                    case 6:
                        action->done = value <= compareValue;
                        break;
                    case 0:
                        action->done = 1;
                        break;
                    }
                    if (action->assess == 0) {
                        if (action->done == 0) {
                            PATHI_conditiondone(action, 0,
                                                 reinterpret_cast<PATHACTION *>(reinterpret_cast<char *>(event) + 8) +
                                                     event->numactions);
                        }
                        lastwhile = Path::milliseconds;
                        action->done = !action->done;
                        return 1;
                    }
                    PATHI_conditiondone(action, action->done,
                                         reinterpret_cast<PATHACTION *>(reinterpret_cast<char *>(event) + 8) +
                                             event->numactions);
                    action->done = 1;
                    break;
                default:
                    action->done = 1;
                    break;
                }
            }
        }
    }
    return action->done;
}

void PATHI_conditiondone(PATHACTION *action, int skiptonextelse, PATHACTION *endaction)
{
    unsigned int startindent = action->indent;
    int waitforendif = skiptonextelse;

    for (action++; action <= endaction && waitforendif >= 0; action++)
    {
        if (action->indent == startindent)
        {
            if (action->type == PATHACTION_CONDITION && action->assess == 4)
            {
                action->done = 1;
                return;
            }

            if (!skiptonextelse)
                return;

            waitforendif = 0;
        }

        if (!waitforendif)
            action->done = 1;
    }
}

int PATHI_restoretolastwhile(PATHACTION *action, PATHEVENT *event)
{
    PATHACTION *origaction = action;
    PATHACTION *whileaction = 0;
    PATHACTION *firstaction = (PATHACTION *)(event + 1);
    int indent = action->indent;

    for (;;)
    {
        action--;

        if (action < firstaction)
            break;

        if (action->indent != indent)
            continue;

        if (action->type != PATHACTION_CONDITION)
            continue;

        if (action->assess)
            break;

        whileaction = action;
        break;
    }

    if (!whileaction)
        return 0;

    event->currentaction = whileaction - firstaction;

    for (; action <= origaction; action++)
        action->done = 0;

    return 1;
}

PATHTRACKPLAYSTATUS PATHI_trackstatus(PATHTRACK *track)
{
    PATHTRACKPLAYSTATUS trackstatus;

    if (!track)
        return PATHTRACK_INVALID;

    track->status = PATHTRACK_STOPPED;

    if (track->paused)
    {
        track->status = PATHTRACK_PAUSED;
    }
    else if (track->node >= 0)
    {
        track->status = PATHTRACK_PLAYING;

        if (PATHI_readyfornewrequest(track) &&
            PATHI_timeremaining(track) <= (int)track->latency)
        {
            track->status = PATHTRACK_READYFORNEXT;
        }
        else if (track->trackimp->TimeBuffered() < track->trackimp->TimeRemaining(-1))
        {
            track->status = PATHTRACK_QUEUEING;
        }

        if (track->volumefade.fadeto >= 0)
            track->status = PATHTRACK_FADING;
    }

    return track->status;
}

int PATHI_getvalue(int val, int inValType, PATHTRACK *track, PATHEVENT *event)
{
    PATHSTATUS trackstatus;
    PATHFINDNODE *nodeinfo;
    PATHVALUETYPE valType = (PATHVALUETYPE)inValType;

    if (valType == PATH_VALUE_VARIABLE)
    {
        val = pfstate->pnamedvars[val].value;
    }
    else if (valType == PATH_VALUE_SPECIAL)
    {
        switch (val)
        {
        case PATH_CONTROLLER:
            val = track->control;
            break;

        case PATH_CURRENTNODE:
            val = track->node;
            break;

        case PATH_CURRENTPART:
            nodeinfo = track->node >= 0 ? PATHI_getnode(track->node) : 0;
            val = -1;
            if (nodeinfo)
                val = nodeinfo->partID;
            break;

        case PATH_CURRENTSECTION:
            nodeinfo = track->node >= 0 ? PATHI_getnode(track->node) : 0;
            if (nodeinfo)
                val = nodeinfo->sectionID;
            else
                val = -1;
            break;

        case PATH_EVENTEXPIRY:
            val = event->expiry;
            break;

        case PATH_EVENTPRIORITY:
            val = event->priority;
            break;

        case PATH_FXBUS:
            val = track->sfxbus;
            break;

        case PATH_FXDRYLEVEL:
            val = track->drylevel;
            break;

        case PATH_FXSENDLEVEL:
            val = track->sfxlevel;
            break;

        case PATH_MAINVOICE:
            val = track->mainvoice;
            break;

        case PATH_NEXTNODE:
            val = PATHI_nextnode(track->node, track->control, 0);
            val = PATHI_enternode(track->node, val, track->control, 0);
            break;

        case PATH_NOBRANCHING:
            val = track->nobranch;
            break;

        case PATH_NODEDURATION:
            PATHI_status(track, &trackstatus);
            val = trackstatus.nodeduration;
            break;

        case PATH_PAUSE:
            val = track->paused;
            break;

        case PATH_PITCHMULT:
            if (track->trackimp)
                val = track->trackimp->GetPitchMult();
            else
                val = 1;
            break;

        case PATH_PLAYINGNODE:
            PATHI_status(track, &trackstatus);
            val = trackstatus.playingnode;
            break;

        case PATH_PLAYSTATUS:
            val = PATHI_trackstatus(track);
            break;

        case PATH_RANDOMSHORT:
            val = (unsigned short)(PATHI_random() / 23);
            break;

        case PATH_TIMENOW:
            val = IPathToReal::realimp->GetMilliseconds();
            break;

        case PATH_TIMETONEXTBEAT:
            PATHI_status(track, &trackstatus);
            val = trackstatus.timeleftinbeat;
            break;

        case PATH_TIMETONEXTBAR:
            PATHI_status(track, &trackstatus);
            val = trackstatus.timeleftinbar;
            break;

        case PATH_TIMETONEXTNODE:
            PATHI_status(track, &trackstatus);
            val = trackstatus.timeleftinnode;
            break;

        case PATH_VOLUME:
            val = track->volume;
            if (track->volumefade.fadeto >= 0)
                val = track->trackimp->GetVolume();
            break;

        case PATH_TIMESTRETCH:
            if (track->trackimp)
                val = track->trackimp->GetStretchMult();
            else
                val = 1;
            break;

        case PATH_BARDURATION:
            PATHI_status(track, &trackstatus);
            val = trackstatus.barduration;
            break;

        case PATH_BEATDURATION:
            PATHI_status(track, &trackstatus);
            val = trackstatus.beatduration;
            break;
        }
    }

    return val;
}

void PATHI_setvalue(int setwhat, int inValType, int towhat, PATHTRACK *track, PATHEVENT *event)
{
    PATHVALUETYPE valType = (PATHVALUETYPE)inValType;

    if (valType == PATH_VALUE_VARIABLE)
    {
        pfstate->pnamedvars[setwhat].value = towhat;
    }
    else if (valType == PATH_VALUE_SPECIAL)
    {
        switch (setwhat)
        {
        case PATH_CONTROLLER:
            track->control = towhat;
            if (towhat < 0)
                towhat = 0;
            if (towhat > 127)
                towhat = 127;
            break;

        case PATH_EVENTEXPIRY:
            event->expiry = towhat;
            break;

        case PATH_EVENTPRIORITY:
            event->priority = towhat;
            if (towhat < -7)
                event->priority = -7;
            if (towhat > 7)
                event->priority = 7;
            break;

        case PATH_FXBUS:
            track->sfxbus = towhat;
            break;

        case PATH_FXDRYLEVEL:
            if (towhat > 127)
                towhat = 127;
            if (towhat < 0)
                towhat = 0;

            track->drylevel = towhat;

            if (track->trackimp)
                track->trackimp->SetDryLevel(towhat);
            break;

        case PATH_FXSENDLEVEL:
            if (towhat > 127)
                towhat = 127;
            if (towhat < 0)
                towhat = 0;

            if (track->trackimp)
                track->trackimp->SetFXSendLevel(track->sfxbus, towhat);

            track->sfxlevel = towhat;
            break;

        case PATH_MAINVOICE:
            PATHI_mainvoice(track, towhat);
            break;

        case PATH_NOBRANCHING:
            track->nobranch = (towhat != 0);
            break;

        case PATH_PAUSE:
            towhat = (towhat != 0);
            track->paused = towhat;

            if (track->trackimp)
                track->trackimp->Pause(towhat);
            break;

        case PATH_PITCHMULT:
            if (track->trackimp)
                track->trackimp->SetPitchMult(towhat);
            break;

        case PATH_VOLUME:
            if (towhat > 127)
                towhat = 127;
            if (towhat < 0)
                towhat = 0;

            PATHI_volume(track, towhat);
            break;

        case PATH_TIMESTRETCH:
            if (track->trackimp)
                track->trackimp->SetStretchMult(towhat);
            break;
        }
    }
}
