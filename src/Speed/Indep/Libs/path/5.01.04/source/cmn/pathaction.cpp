#include "pathi.h"
#include "path/IPathToReal.h"
void PATHI_conditiondone(PATHACTION *action, int skiptonextelse, PATHACTION *endaction);
int PATHI_restoretolastwhile(PATHACTION *action, PATHEVENT *event);
int PATHI_getvalue(int val, int inValType, PATHTRACK *track, PATHEVENT *event);
void PATHI_setvalue(int setwhat, int inValType, int towhat, PATHTRACK *track, PATHEVENT *event);

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
    int by;
    int setwhat;
    int towhat;
    int ms;
    int result;
    static unsigned int lastwhile;
    static unsigned int lastendif;

    savenumevents = Path::pfstate->eventindex;
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
                trackhandle = (static_cast<unsigned int>(action->track) & 0x0f000000) | 0xf0000000;
                trackhandle = (trackhandle & Path::pfstate->idflags) | trackFlag;
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
                if (voice == 0 && mainvoice == 0 && track->mainvoice != 0) {
                    continue;
                }

                switch (action->type) {
                case 2:
                    value = PATHI_getvalue(act.waittime.millisecs, action->rightvaluetype, track, event);
                    compareValue = act.waittime.millisecs;
                    if (act.waittime.lowest > 0 && act.waittime.lowest < value) {
                        value = 0;
                    }
                    if (action->rightvaluetype == PATH_VALUE_SPECIAL &&
                        compareValue >= 0x14 && compareValue <= 0x16) {
                        if (event->lastact == 0 || event->lastact > value) {
                            event->lastact = value;
                        }
                        if (value > 0) {
                            value = Path::milliseconds + 0xbb8;
                        }
                    }
                    ms = Path::IPathToReal::realimp->GetMilliseconds();
                    if (value <= static_cast<int>(ms - event->lastact)) {
                        action->act.waittime.lowest = 0;
                        action->done = 1;
                    }
                    break;
                case 3:
                    if (act.waitbeat.millisecs == 0) {
                        track->trackimp->UpdateStatus();
                        if (PATHI_beatinfo(track, &beatinfo) < 0) {
                            action->done = 1;
                            break;
                        }
                        ms = PATHI_calcwaitbeat(act.waitbeat.every, 1 << act.waitbeat.note,
                                                 act.waitbeat.offset, &beatinfo);
                        action->act.waitbeat.millisecs = ms;
                    } else if (act.waitbeat.millisecs <=
                               static_cast<int>(Path::IPathToReal::realimp->GetMilliseconds() - event->lastact)) {
                        action->act.waitbeat.millisecs = -1;
                        action->done = 1;
                    }
                    break;
                case 4:
                    savetrack = *track;
                    nodeID = PATHI_getvalue(act.branch.node, action->leftvaluetype, track, event);
                    sectionID = PATHI_getvalue(act.branch.ofsection, action->rightvaluetype, track, event);
                    nodeinfo = 0;
                    if (nodeID >= 0) {
                        if (sectionID < 0) {
                            nodeinfo = PATHI_getnode(nodeID);
                        } else {
                            for (n = 0; n <= Path::pfstate->pmap->numnodes; n++) {
                                nodeinfo = PATHI_getnode(n);
                                if (nodeinfo != 0 && nodeinfo->sectionID == sectionID &&
                                    nodeinfo->index == 0) {
                                    nodeID--;
                                    if (nodeID == 0) {
                                        break;
                                    }
                                }
                            }
                            if (n > Path::pfstate->pmap->numnodes) {
                                nodeID = -1;
                                nodeinfo = 0;
                            }
                        }
                    }
                    if (nodeID < -1 || nodeID > Path::pfstate->pmap->numnodes ||
                        (nodeinfo != 0 && nodeinfo->trackID != track->trackID)) {
                        if ((Path::debugchannels & kEventDebugChannel) != 0) {
                            PATHI_printf("( %.8s )  PATHACTION_BRANCHTO bad node ID = %d, \n");
                        }
                        nodeID = -1;
                    }
                    if (track->nobranch != 0) {
                        continue;
                    }
                    currentnode = track->node;
                    if (act.branch.immediate != 0) {
                        PATHI_stop(track);
                    }
                    trackstatus = PATHTRACK_INVALID;
                    if (track->loadingsubbank < 0) {
                        trackstatus = static_cast<PATHTRACKPLAYSTATUS>(PATHI_readyfornewrequest(track));
                    } else {
                        trackstatus = PATHTRACK_INVALID;
                    }
                    if (trackstatus != PATHTRACK_INVALID) {
                        track->paused = 0;
                        result = PATHI_enternode(currentnode, nodeID, track->control, 1);
                        track->node = static_cast<short>(result);
                        result = PATHI_queuenode(track);
                    }
                    if (result >= 0) {
                        action->done = 1;
                    } else {
                        *track = savetrack;
                        Path::pfstate->eventindex = savenumevents;
                        continue;
                    }
                    break;
                case 5:
                    value = PATHI_getvalue(act.fade.ms, action->rightvaluetype, track, event);
                    fadetime = act.fade.id;
                    PATHI_fade(track, act.fade.tovol, value, fadetime);
                    action->done = 1;
                    break;
                case 7:
                    value = PATHI_getvalue(act.sfxfade.ms, action->rightvaluetype, track, event);
                    fadetime = act.sfxfade.id;
                    PATHI_customsfxfade(track, act.sfxfade.tovol, value, fadetime);
                    action->done = 1;
                    break;
                case 6:
                    value = PATHI_getvalue(act.dryfade.ms, action->rightvaluetype, track, event);
                    fadetime = act.dryfade.id;
                    PATHI_customdrylevelfade(track, act.dryfade.tovol, value, fadetime);
                    action->done = 1;
                    break;
                case 17:
                    value = PATHI_getvalue(act.pitchfade.ms, action->rightvaluetype, track, event);
                    fadetime = act.pitchfade.id;
                    PATHI_custompitchfade(track, act.pitchfade.tovol, value, fadetime);
                    action->done = 1;
                    break;
                case 18:
                    value = PATHI_getvalue(act.stretchfade.ms, action->rightvaluetype, track, event);
                    fadetime = act.stretchfade.id;
                    PATHI_customstretchfade(track, act.stretchfade.tovol, value, fadetime);
                    action->done = 1;
                    break;
                case 9:
                    {
                        int eventID;
                        int eventIDMask;
                        eventID = act.event.eventid;
                        value = PATHI_getvalue(eventID, action->leftvaluetype, track, event);
                        eventIDMask = -1;
                        if (value != eventID) {
                            eventIDMask = 0xffff;
                        }
                        neweventp = PATHI_getevent(value, eventIDMask);
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
                    value = PATHI_getvalue(act.calc.value, action->leftvaluetype, track, event);
                    compareValue = PATHI_getvalue(act.calc.by, action->rightvaluetype, track, event);
                    by = act.calc.op;
                    switch (by) {
                    case 1:
                        value += compareValue;
                        break;
                    case 2:
                        value -= compareValue;
                        break;
                    case 3:
                        value *= compareValue;
                        break;
                    case 4:
                        value /= compareValue;
                        break;
                    case 5:
                        value %= compareValue;
                        break;
                    default:
                        break;
                    }
                    PATHI_setvalue(act.calc.value, action->leftvaluetype, value, track, event);
                    action->done = 1;
                    break;
                case 15:
                    value = PATHI_getvalue(act.pause.when, action->leftvaluetype, track, event);
                    action->done = 1;
                    if (value < 1) {
                        track->paused = act.pause.on;
                        track->trackimp->Pause(track->paused);
                    } else if (act.pause.on == 0) {
                        track->resumeat = Path::milliseconds + value;
                    } else {
                        track->pauseat = Path::milliseconds + value;
                    }
                    break;
                case 16:
                    value = PATHI_getvalue(act.loadbank.subbanknum, action->rightvaluetype, track, event);
                    action->done = 1;
                    if (act.loadbank.unload == 0) {
                        PATHI_loadbank(track, value);
                    } else {
                        PATHI_unloadbank(track, value);
                    }
                    break;
                case 1:
                    if (action->assess == 3) {
                        action->done = 1;
                        return 1;
                    }
                    if (action->assess != 4) {
                        value = PATHI_getvalue(act.only.value, action->leftvaluetype, track, event);
                        compareValue = PATHI_getvalue(act.only.compareValue,
                                                      action->rightvaluetype, track, event);
                        switch (action->comparison) {
                        case 1:
                            action->done = value == compareValue;
                            break;
                        case 2:
                            action->done = value != compareValue;
                            break;
                        case 3:
                            action->done = value < compareValue;
                            break;
                        case 4:
                            action->done = compareValue < value;
                            break;
                        case 5:
                            action->done = compareValue <= value;
                            break;
                        case 6:
                            action->done = value <= compareValue;
                            break;
                        default:
                            action->done = 1;
                            break;
                        }
                        if (action->indent == 0) {
                            if (action->done == 0) {
                                PATHI_conditiondone(action, 0,
                                                     reinterpret_cast<PATHACTION *>(event + 1) + event->numactions);
                            }
                            lastwhile = Path::milliseconds;
                            action->done = action->done == 0;
                            return 1;
                        }
                        PATHI_conditiondone(action, action->done == 0,
                                             reinterpret_cast<PATHACTION *>(event + 1) + event->numactions);
                        action->done = 1;
                    } else {
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

void PATHI_conditiondone(PATHACTION *action, int skiptonextelse, PATHACTION *endaction) {
    unsigned int startindent;
    int waitforendif;

    startindent = action->indent;
    action++;
    if (action > endaction) {
        return;
    }
    if (skiptonextelse < 0) {
        return;
    }
    waitforendif = skiptonextelse;
    do {
        if (action->indent == startindent) {
            if (action->type == 1 && action->assess == 4) {
                action->done = 1;
                return;
            }
            if (waitforendif == 0) {
                return;
            }
            skiptonextelse = 0;
        }
        if (skiptonextelse == 0) {
            action->done = 1;
        }
        action++;
        if (action > endaction) {
            return;
        }
        if (skiptonextelse < 0) {
            return;
        }
    } while (1);
}

int PATHI_restoretolastwhile(PATHACTION *action, PATHEVENT *event) {
    PATHACTION *origaction;
    PATHACTION *whileaction;
    PATHACTION *firstaction;
    int indent;

    origaction = action;
    whileaction = 0;
    firstaction = reinterpret_cast<PATHACTION *>(event + 1);
    indent = action->indent;
    do {
        action--;
        if (action < firstaction) {
            break;
        }
        if (action->indent == indent && action->type == 1) {
            if (action->assess == 0) {
                whileaction = action;
            }
            break;
        }
    } while (1);
    if (whileaction == 0) {
        return 0;
    }
    event->currentaction = whileaction - firstaction;
    for (; action <= origaction; action++) {
        action->done = 0;
    }
    return 1;
}

PATHTRACKPLAYSTATUS PATHI_trackstatus(PATHTRACK *track) {
    PATHTRACKPLAYSTATUS trackstatus;

    if (track == 0) {
        return PATHTRACK_INVALID;
    }
    track->status = PATHTRACK_STOPPED;
    if (track->paused != 0) {
        track->status = PATHTRACK_PAUSED;
    } else if (track->node >= 0) {
        track->status = PATHTRACK_PLAYING;
        if (track->loadingsubbank >= 0) {
            trackstatus = PATHTRACK_INVALID;
        } else {
            trackstatus = static_cast<PATHTRACKPLAYSTATUS>(PATHI_readyfornewrequest(track));
        }
        if (trackstatus != PATHTRACK_INVALID && PATHI_timeremaining(track) <= static_cast<int>(track->latency)) {
            track->status = PATHTRACK_READYFORNEXT;
        } else if (track->trackimp->TimeBuffered() < track->trackimp->TimeRemaining(-1)) {
            track->status = PATHTRACK_QUEUEING;
        }
        if (track->volumefade.fadeto < 0) {
            return track->status;
        }
        track->status = PATHTRACK_FADING;
    }
    return track->status;
}

int PATHI_getvalue(int val, int inValType, PATHTRACK *track, PATHEVENT *event) {
    PATHSTATUS trackstatus;
    PATHFINDNODE *nodeinfo;
    PATHVALUETYPE valType = static_cast<PATHVALUETYPE>(inValType);

    if (valType == PATH_VALUE_VARIABLE) {
        val = Path::pfstate->pnamedvars[val].value;
    } else if (valType == PATH_VALUE_SPECIAL) {
        switch (val) {
        case 1:
            val = track->control;
            break;
        case 2:
            val = track->node;
            break;
        case 3:
            nodeinfo = PATHI_getnode(track->node);
            val = -1;
            if (nodeinfo != 0) {
                val = nodeinfo->partID;
            }
            break;
        case 4:
            nodeinfo = PATHI_getnode(track->node);
            if (nodeinfo != 0) {
                val = nodeinfo->sectionID;
                break;
            }
            val = -1;
            break;
        case 5:
            val = event->expiry;
            break;
        case 6:
            val = event->priority;
            break;
        case 7:
            val = track->sfxbus;
            break;
        case 8:
            val = static_cast<unsigned short>(track->drylevel);
            break;
        case 9:
            val = static_cast<unsigned short>(track->sfxlevel);
            break;
        case 0xa:
            val = track->mainvoice;
            break;
        case 0xb:
            val = PATHI_nextnode(track->node, track->control, 0);
            val = PATHI_enternode(track->node, val, track->control, 0);
            break;
        case 0xc:
            val = track->nobranch;
            break;
        case 0xd:
            PATHI_status(track, &trackstatus);
            val = trackstatus.nodeduration;
            break;
        case 0xe:
            val = track->paused;
            break;
        case 0xf:
            if (track->trackimp != 0) {
                val = track->trackimp->GetPitchMult();
            } else {
                val = 1;
            }
            break;
        case 0x10:
            PATHI_status(track, &trackstatus);
            val = trackstatus.playingnode;
            break;
        case 0x11:
            val = PATHI_trackstatus(track);
            break;
        case 0x12:
            val = (PATHI_random() / 0x17) & 0xffff;
            break;
        case 0x13:
            val = Path::IPathToReal::realimp->GetMilliseconds();
            break;
        case 0x14:
            PATHI_status(track, &trackstatus);
            val = trackstatus.timeleftinbeat;
            break;
        case 0x15:
            PATHI_status(track, &trackstatus);
            val = trackstatus.timeleftinbar;
            break;
        case 0x16:
            PATHI_status(track, &trackstatus);
            val = trackstatus.timeleftinnode;
            break;
        case 0x17:
            val = static_cast<char>(track->volume);
            if (track->volumefade.fadeto >= 0) {
                val = track->trackimp->GetVolume();
            }
            break;
        case 0x18:
            if (track->trackimp != 0) {
                val = track->trackimp->GetStretchMult();
            } else {
                val = 1;
            }
            break;
        case 0x19:
            PATHI_status(track, &trackstatus);
            val = trackstatus.barduration;
            break;
        case 0x1a:
            PATHI_status(track, &trackstatus);
            val = trackstatus.beatduration;
            break;
        default:
            break;
        }
    }
    return val;
}

void PATHI_setvalue(int setwhat, int inValType, int towhat, PATHTRACK *track, PATHEVENT *event) {
    PATHVALUETYPE valType = static_cast<PATHVALUETYPE>(inValType);

    if (valType == PATH_VALUE_VARIABLE) {
        Path::pfstate->pnamedvars[setwhat].value = towhat;
    } else if (valType == PATH_VALUE_SPECIAL) {
        switch (setwhat) {
        case 1:
            track->control = towhat;
            if (towhat < 0) {
                towhat = 0;
            }
            if (towhat > 0x7f) {
                towhat = 0x7f;
            }
            break;
        case 5:
            event->expiry = towhat;
            break;
        case 6:
            event->priority = towhat;
            if (towhat < -7) {
                event->priority = -7;
            }
            if (towhat >= 8) {
                event->priority = 7;
            }
            break;
        case 7:
            track->sfxbus = towhat;
            break;
        case 8:
            if (towhat > 0x7f) {
                towhat = 0x7f;
            }
            if (towhat < 0) {
                towhat = 0;
            }
            track->drylevel = towhat;
            if (track->trackimp != 0) {
                track->trackimp->SetDryLevel(towhat);
            }
            break;
        case 9:
            if (towhat > 0x7f) {
                towhat = 0x7f;
            }
            if (towhat < 0) {
                towhat = 0;
            }
            if (track->trackimp != 0) {
                track->trackimp->SetFXSendLevel(track->sfxbus, towhat);
            }
            track->sfxlevel = towhat;
            break;
        case 10:
            PATHI_mainvoice(track, towhat);
            break;
        case 12:
            track->nobranch = towhat != 0;
            break;
        case 14:
            towhat = towhat != 0;
            track->paused = towhat;
            if (track->trackimp != 0) {
                track->trackimp->Pause(towhat);
            }
            break;
        case 15:
            if (track->trackimp != 0) {
                track->trackimp->SetPitchMult(towhat);
            }
            break;
        case 23:
            if (towhat > 0x7f) {
                towhat = 0x7f;
            }
            if (towhat < 0) {
                towhat = 0;
            }
            PATHI_volume(track, static_cast<signed char>(towhat));
            break;
        case 24:
            if (track->trackimp != 0) {
                track->trackimp->SetStretchMult(towhat);
            }
            break;
        default:
            break;
        }
    }
}
