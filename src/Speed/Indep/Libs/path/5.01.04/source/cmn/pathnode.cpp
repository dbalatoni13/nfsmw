#include "pathi.h"

unsigned int PATHI_sampleoffset(int node) {
    unsigned int offset;
    int sampleindex;
    PATHFINDNODE *nodeinfo;

    offset = 0;
    if (node < 0 || node >= Path::pfstate->pmap->numnodes) {
        return 0;
    }
    nodeinfo = PATHI_getnode(node);
    sampleindex = nodeinfo->index;
    if (sampleindex > 0) {
        offset = Path::pfstate->psampleoffsets[sampleindex - 1].offset;
    }
    return offset;
}

int PATHI_beatinfo(PATHTRACK *track, PATHBEATINFO *beatinfo) {
    PATHFINDNODE *nodeinfo;
    PATHSTATUS status;

    if (track->node < 0) {
        return -1;
    }
    nodeinfo = PATHI_getnode(track->node);
    PATHI_status(track, &status);
    if (beatinfo != 0) {
        beatinfo->beats = nodeinfo->beats;
        beatinfo->notes = nodeinfo->notes;
        beatinfo->playingbeat = status.playingbeat;
        beatinfo->beatduration = status.beatduration;
        beatinfo->barduration = status.barduration;
        beatinfo->nodeduration = status.nodeduration;
        beatinfo->timetonextbar = status.timeleftinbar;
        beatinfo->timetonextbeat = status.timeleftinbeat;
        beatinfo->timetonextnode = status.timeleftinnode;
    }
    return status.playingbeat;
}

int PATHI_calcwaitbeat(int every, int note, int offset, PATHBEATINFO *beatinfo) {
    float scalar = static_cast<float>(beatinfo->beats) / static_cast<float>(note);
    float fevery = static_cast<float>(every);
    float foffset = static_cast<float>(offset);
    int timeinbar = beatinfo->barduration - beatinfo->timetonextbar;
    int firstsynchtime = static_cast<int>(scalar * foffset * static_cast<float>(beatinfo->beatduration));
    int nextsynchtime = firstsynchtime;
    if (nextsynchtime < timeinbar) {
        do {
            if (nextsynchtime >= static_cast<int>(beatinfo->barduration)) {
                break;
            }
            nextsynchtime += static_cast<int>(scalar * fevery * static_cast<float>(beatinfo->beatduration));
        } while (nextsynchtime <= timeinbar);
    }
    if (nextsynchtime > static_cast<int>(beatinfo->barduration)) {
        nextsynchtime = beatinfo->barduration + firstsynchtime;
    }
    return nextsynchtime - timeinbar;
}

int PATHI_choosesynchtime(int node, const PATHFINDNODE &entryinfo, const PATHBEATINFO &masterinfo,
                          unsigned int &waitms) {
    if (static_cast<short>(entryinfo.partID) < 0 || node < 0) {
        return 1;
    }
    PATHFINDNODE *nodeinfo = PATHI_getnode(node);
    PATHFINDSAMPLE oversample = Path::pfstate->psampleoffsets[nodeinfo->index - 1];
    bool forcesynch;
    float beatlen = static_cast<float>(oversample.duration);
    if (nodeinfo->beats != 0) {
        beatlen /= static_cast<float>(nodeinfo->beats * nodeinfo->bars);
    }
    forcesynch = nodeinfo->extra.beat.forcesynch;
    if (forcesynch) {
        beatlen = static_cast<float>(masterinfo.beatduration);
    }
    int nodebeat = -1;
    int elapsedtime = masterinfo.nodeduration - masterinfo.timetonextnode;
    int overbeatsleft = static_cast<int>(static_cast<float>(masterinfo.timetonextnode) / beatlen);
    int overbeatsdone = static_cast<int>(static_cast<float>(elapsedtime) / beatlen);
    switch (entryinfo.synch) {
    case 2:
        waitms = masterinfo.timetonextnode % static_cast<int>(overbeatsleft * beatlen);
        nodebeat = nodeinfo->beats - (overbeatsleft % nodeinfo->beats) + 1;
        break;
    case 1:
        overbeatsdone++;
        waitms = static_cast<int>(overbeatsdone * beatlen) - elapsedtime;
        nodebeat = (overbeatsdone % nodeinfo->beats) + 1;
        break;
    case 3:
        nodebeat = 1;
        waitms = static_cast<int>((overbeatsdone + 1) * beatlen) - elapsedtime;
        break;
    }
    if (waitms != 0) {
        waitms += Path::milliseconds;
    }
    return nodebeat;
}

int PATHI_timeremaining(PATHTRACK *track) {
    int timeremaining;
    int activerequests;

    timeremaining = 0;
    for (int i = 0;;) {
        int requesttime = track->trackimp->TimeRemaining(i);
        i++;

        if (requesttime < 0) {
            break;
        }
        timeremaining += requesttime;
    }
    return timeremaining;
}

int PATHI_pickclosestbranch(int numBranches, int control, PATHFINDBRANCH *branch) {
    int i;
    int below;
    int above;
    int distance;
    int closest;
    PATHFINDBRANCH *bestBranch;

    if (numBranches <= 0) {
        return -1;
    }
    closest = 0x7f;
    bestBranch = branch;
    for (i = 0; i < numBranches; i++) {
        below = branch[i].controlmin - control;
        above = branch[i].controlmax - control;
        if (below < 0) {
            below = -below;
        }
        if (above < 0) {
            above = -above;
        }
        distance = above;
        if (above > below) {
            distance = below;
        }
        if (closest > distance) {
            bestBranch = &branch[i];
            closest = distance;
        }
    }
    return bestBranch->dstnode;
}

int PATHI_nextnode(int node, int control, int forreal) {
    PATHFINDNODE *nodeinfo;
    PATHFINDBRANCH *branches;
    PATHTRACK *track;
    unsigned int i;
    int nextnode;

    nextnode = -1;
    if (node < 0) {
        return nextnode;
    }
    nodeinfo = PATHI_getnode(node);
    if (nodeinfo == 0) {
        return nextnode;
    }
    branches = reinterpret_cast<PATHFINDBRANCH *>(nodeinfo + 1);
    track = Path::pfstate->track[nodeinfo->trackID];
    if (track->repeatnode == node) {
        control = track->control;
        if (nodeinfo->repeat > 0 && forreal != 0) {
            track->repeat--;
            if (track->repeat == -1) {
                track->repeatnode = -1;
            }
        }
    }
    for (i = 0; i < nodeinfo->numbranches; i++) {
        if (control >= branches[i].controlmin && control <= branches[i].controlmax) {
            nextnode = static_cast<short>(branches[i].dstnode);
            break;
        }
    }
    if (nextnode < 0) {
        nextnode = PATHI_pickclosestbranch(nodeinfo->numbranches, control, branches);
    }
    return nextnode;
}

int PATHI_enternode(int origin, int node, int control, int forreal) {
    PATHFINDNODE *nodeinfo;
    PATHTRACK *track;
    PATHEVENT *eventp;
    int savecontrol;

    if (node < 0) {
        return -1;
    }
    node = static_cast<short>(PATHI_routenode(origin, node));
    if (node < 0) {
        return -1;
    }
    nodeinfo = PATHI_getnode(node);
    if (nodeinfo == 0) {
        return -1;
    }
    track = Path::pfstate->track[nodeinfo->trackID];
    while (nodeinfo->index < 1) {
        savecontrol = control;
        if (forreal != 0) {
            if (nodeinfo->index == 0) {
                track->entryinfo = nodeinfo;
                if (nodeinfo->controller == 1) {
                    savecontrol = PATHI_random() & 0x7f;
                }
                if (Path::songprogress != 0) {
                    Path::songprogress(Path::pfstate->idflags, node);
                }
            } else if (nodeinfo->index == -1) {
                if (nodeinfo->repeat != 0 && node != track->repeatnode) {
                    track->repeatnode = node;
                    track->repeat = nodeinfo->repeat;
                }
            } else if (nodeinfo->index == -2) {
                savecontrol = PATHI_random() & 0x7f;
            } else if (nodeinfo->index == -3) {
                eventp = PATHI_getevent(nodeinfo->extra.sendevent.eventID, 0xffffff);
                if (eventp != 0) {
                    eventp = PATHI_copyevent(eventp);
                    if (eventp != 0) {
                        PATHI_addevent(Path::pfstate->idflags | 0xf0000000, eventp);
                    }
                }
            }
        }
        node = PATHI_nextnode(node, savecontrol, forreal);
        if (node < 0) {
            if (Path::songprogress != 0 && forreal != 0 && origin >= 0) {
                Path::songprogress(Path::pfstate->idflags, node);
            }
            return -1;
        }
        node = static_cast<short>(PATHI_routenode(origin, node));
        if (node < 0) {
            if (Path::songprogress != 0 && forreal != 0 && origin >= 0) {
                Path::songprogress(Path::pfstate->idflags, node);
            }
            return -1;
        }
        nodeinfo = PATHI_getnode(node);
        if (nodeinfo == 0) {
            return -1;
        }
    }
    return static_cast<short>(PATHI_routenode(origin, node));
}

int PATHI_routenode(int fromnode, int destnode) {
    int *router;
    int numpairs;
    int i;
    unsigned int routerID;

    if (fromnode < 0) {
        return destnode;
    }
    routerID = PATHI_getnode(fromnode)->routerID;
    if (routerID != 0) {
        numpairs = Path::pfstate->prouters[routerID] - Path::pfstate->prouters[routerID - 1];
        router = reinterpret_cast<int *>(Path::pfstate->pmap) + Path::pfstate->prouters[routerID - 1];
        for (i = 0; i < numpairs; i++) {
            if (destnode == router[i] >> 16) {
                destnode = router[i] & 0xffff;
            }
        }
    }
    return destnode;
}

void PATHI_seeknextnode(int trackindex) {
    PATHTRACK *track;
    PATHTRACK savetrack;
    PATHFINDNODE *nodeinfo;
    int nodebeat;
    int nextnode;
    int savenumevents;
    int result;

    nextnode = -1;
    savenumevents = Path::pfstate->eventindex;
    track = Path::pfstate->track[trackindex];
    if (track->node < 0) {
        return;
    }
    savetrack = *track;
    nodeinfo = PATHI_getnode(track->node);
    nodebeat = track->ramtrack != 0 ? 1 : -1;
    if (track->nodebeat < 1 || nodeinfo->extra.beat.playbeats == 0 ||
        nodeinfo->beats * nodeinfo->bars < track->nodebeat) {
        if (track->node >= 0) {
            nextnode = PATHI_nextnode(track->node, track->control, 1);
            if (Path::songprogress != 0 && nextnode < 0) {
                Path::songprogress(Path::pfstate->idflags, nextnode);
            }
        }
    } else {
        nextnode = track->node;
        nodebeat = track->nodebeat;
    }
    if (nextnode >= 0) {
        nextnode = PATHI_enternode(track->node, nextnode, track->control, 1);
    }
    track->node = nextnode;
    if (nextnode > -1) {
        track->nodebeat = nodebeat;
    } else {
        track->nodebeat = -1;
    }
    result = PATHI_queuenode(track);
    if (result < 0 && result != -9999) {
        Path::pfstate->eventindex = savenumevents;
        *track = savetrack;
    }
}

int PATHI_queuenode(PATHTRACK *track) {
    PATHTRACK *mastertrack;
    PATHFINDNODE *nodeinfo;
    PATHBEATINFO masterinfo;
    int playbeat;
    int duration;
    int holdtime;
    int sampleoffset;

    mastertrack = 0;
    playbeat = 0;
    holdtime = 0;
    if (Path::pfstate->mastertrack > -1) {
        mastertrack = Path::pfstate->track[Path::pfstate->mastertrack];
    }
    if (track->node < 0) {
        track->entryinfo = 0;
        track->newestrequesthandle = -1;
        track->nodebeat = -1;
        track->node = -1;
        return 0;
    }
    nodeinfo = PATHI_getnode(track->node);
    track->entryinfo = PATHI_getnode(nodeinfo->partID);
    duration = Path::pfstate->psampleoffsets[nodeinfo->index - 1].duration;
    track->bartime = duration;
    track->beattime = duration;
    if (nodeinfo->bars != 0) {
        track->bartime /= nodeinfo->bars;
    }
    if (nodeinfo->beats != 0) {
        track->beattime /= nodeinfo->beats * nodeinfo->bars;
    }
    if (track->ramtrack == 0) {
        track->nodebeat = -1;
    } else {
        bool havemastertrack;
        bool forcesynch;
        bool playbeats;

        havemastertrack = mastertrack != 0 && mastertrack != track;
        forcesynch = track->entryinfo->extra.beat.forcesynch != 0 && havemastertrack;
        playbeats = track->entryinfo->extra.beat.playbeats != 0;
        masterinfo.beatduration = 0;
        masterinfo.timetonextbeat = 0;
        if (havemastertrack) {
            PATHI_beatinfo(mastertrack, &masterinfo);
        }
        if (track->nodebeat < 0) {
            if (track->entryinfo->synch != 0 && havemastertrack) {
                track->nodebeat = PATHI_choosesynchtime(track->node, *track->entryinfo, masterinfo,
                                                        track->nextbeattime);
                track->nextbeattime -= Path::pfstate->timerinterval * 5;
                return 0;
            }
        } else if (forcesynch && Path::pfstate->timerinterval <= masterinfo.timetonextbeat &&
                   masterinfo.timetonextbeat < masterinfo.beatduration / 2) {
            return -9999;
        }
        if (track->nodebeat < 1) {
            track->nodebeat = 1;
        }
        track->nextbeattime = 0;
        playbeat = track->nodebeat;
        if (forcesynch && masterinfo.beatduration != 0) {
            if (masterinfo.timetonextbeat < Path::pfstate->timerinterval) {
                masterinfo.timetonextbeat += masterinfo.beatduration;
            }
            track->nextbeattime = masterinfo.timetonextbeat + Path::milliseconds;
            track->nextbeattime -= Path::pfstate->timerinterval * 5;
        }
        if (playbeats) {
            track->nodebeat++;
            if (nodeinfo->beats * nodeinfo->bars <= playbeat) {
                track->nodebeat = -1;
            }
            duration /= nodeinfo->beats * nodeinfo->bars;
        } else {
            track->nodebeat = -1;
        }
    }
    sampleoffset = PATHI_sampleoffset(track->node);
    track->newestrequesthandle =
        track->trackimp->Play(track->node, sampleoffset, playbeat, holdtime, duration);
    return track->newestrequesthandle;
}
