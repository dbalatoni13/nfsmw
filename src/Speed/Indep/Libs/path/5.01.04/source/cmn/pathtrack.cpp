#include <stdio.h>
#include <string.h>
#include "pathi.h"
#include "path/IPathToSnd.h"

int PATH_createstreamtrack(int trackhandle, char *musfilename, int latency) {
    int result;

    if (PATHI_lock() == 0) {
        return PATHERR_INUSE;
    }
    result = PATHI_createtrack(trackhandle, musfilename);
    if (result >= 0) {
        PATHTRACK *track;

        track = PATHI_gettrackptr(trackhandle);
        if (track != 0) {
            track->latency = latency & 0xffff;
        }
    }
    PATHI_unlock();
    return result;
}

Path::IPathTrack *PATH_createstreamimp(int trackhandle, int maxrequests, float buffertime) {
    Path::IPathTrack *trackimp;

    if (PATHI_lock() == 0) {
        return 0;
    }
    trackimp = 0;
    {
        {
            PATHTRACK *track;

            if (PATHI_switchvoice(trackhandle) != 0) {
                track = PATHI_gettrackptr(trackhandle);
                if (track != 0) {
                    {
                        int byterate;
                        int result;

                        byterate = PATHI_bytesperms(track->trackID);
                        result = Path::IPathToSnd::sndimp->CreateStreamTrack(
                            &trackimp, maxrequests, buffertime, byterate);
                        if (trackimp != 0 && result >= 0) {
                            track->trackimp = trackimp;
                            trackimp->SetName(track->trackname);
                            trackimp->SetFilePath(track->musicfilename);
                            trackimp->SetTrackInfo(PATHI_gettrackinfo(track->trackID));
                            trackimp->SetVolume(track->volume * track->volscale / 100);
                        }
                    }
                }
            }
        }
    }
    PATHI_unlock();
    return trackimp;
}

int PATHI_createtrack(int trackhandle, char *musfilename) {
    PATHTRACK *track;
    int result;

    if (PATHI_switchvoice(trackhandle) == 0) {
        return PATHERR_INV_PARAM;
    }
    result = PATHI_inittrack(trackhandle, musfilename);
    if (result < 0) {
        return result;
    }
    track = PATHI_gettrackptr(trackhandle);
    result = 0;
    Path::pfstate->track[track->trackID] = track;
    return result;
}

int PATHI_inittrack(int trackhandle, char *musfilename) {
    int i;
    int trackID;
    int voiceID;
    PATHTRACK *track;
    char *p;
    char c;

    trackID = 0;
    if ((trackhandle & PATH_ALL_TRACKS) == 0 ||
        (trackhandle & PATH_ALL_PROJECTS) == 0 ||
        (trackhandle & PATH_ALL_VOICES) == 0) {
        return PATHERR_INV_PARAM;
    }
    if (((trackhandle ^ 1) & 1) != 0) {
        while (++trackID <= PATH_MAX_TRACKS - 1 &&
               (((trackhandle >> trackID) ^ 1) & 1)) {}
    }
    track = Path::pfstate->track[trackID];
    voiceID = trackhandle & (PATH_ALL_VOICES | PATH_ALL_PROJECTS);
    if (track != 0) {
        return PATHERR_TOOMANY;
    }
    track = static_cast<PATHTRACK *>(PATHI_memalloc(sizeof(PATHTRACK)));
    if (track == 0) {
        return PATHERR_FAILALLOC;
    }
    p = track->trackname + 1;
    memset(track, 0, sizeof(PATHTRACK));
    track->trackID = trackID;
    track->nobranch = 0;
    track->volume = 0x7f;
    track->volscale = Path::volscale;
    track->node = -1;
    track->repeatnode = -1;
    track->volumefade.fadeto = -1;
    track->volumefade.fadefrom = -1;
    track->volumefade.fadenum = -1;
    track->sfxsendfade.fadeto = -1;
    track->sfxsendfade.fadefrom = -1;
    track->sfxsendfade.fadenum = -1;
    track->drylevelfade.fadefrom = -1;
    track->drylevelfade.fadenum = -1;
    track->pitchfade.fadefrom = -1;
    track->pitchfade.fadenum = -1;
    track->stretchfade.fadefrom = -1;
    track->stretchfade.fadenum = -1;
    track->status = PATHTRACK_STOPPED;
    track->loadingsubbank = -1;
    track->fileop = PATH_UNLIKELY_VALUE;
    track->newestrequesthandle = -1;
    track->nodebeat = -1;
    track->mainvoice = (voiceID & PATH_ALL_VOICES) == 0x10000000;
    track->sfxbus = Path::defaultfxbus;
    strcpy(track->musicfilename, musfilename);
    p = musfilename + strlen(musfilename);
    while (p >= musfilename) {
        if (*p == '\\' || *p == '/' || *p == ':') {
            break;
        }
        p--;
    }
    p++;
    voiceID >>= 28;
    i = 0;
    while ((voiceID & 1) == 0 && ++i < PATH_MAX_VOICES) {
        voiceID = (trackhandle & (PATH_ALL_VOICES | PATH_ALL_PROJECTS)) >> (28 + i);
    }
    sprintf(track->trackname, "%x%s", i + 1, p);
    p = track->trackname + 1;
    c = 'a';
    if (p < track->trackname + sizeof(track->trackname)) {
        do {
            if (c == 0 || *p == 0) {
                c = 0;
            }
            else {
                c = static_cast<char>(*p | 0x20);
            }
            if ((c - 'a' > 25) && (c - '0' > 9)) {
                c = 0;
                if (p >= track->trackname + 12) {
                    *p = 0;
                    break;
                }
                *p = ' ';
            }
            p++;
        } while (p < track->trackname + sizeof(track->trackname));
    }
    track->trackname[11] = 0;
    Path::pfstate->track[trackID] = track;
    return trackID;
}

PATHTRACK *PATHI_gettrackptr(unsigned int trackhandle) {
    unsigned int p;
    unsigned int t;
    unsigned int idflags;
    unsigned int voices;
    unsigned int projects;
    unsigned int tracks;

    voices = trackhandle & PATH_ALL_VOICES;
    projects = trackhandle & PATH_ALL_PROJECTS;
    tracks = trackhandle & PATH_ALL_TRACKS;
    for (p = 0; p < PATH_MAX_PROJECTS; p++) {
        if (*(p + Path::pfstates) != 0) {
            idflags = (*(p + Path::pfstates))->idflags;
            if ((idflags & voices) != 0 && (idflags & projects) != 0) {
                for (t = 0; t < PATH_MAX_TRACKS; t++) {
                    if (((tracks >> t) & 1) != 0) {
                        return Path::pfstates[p]->track[t];
                    }
                }
            }
        }
    }
    return 0;
}

void PATHI_getmastertrack() {
    PATHTRACK *track;
    int t;
    char trackplaying;

    Path::pfstate->masterlatency = 0;
    Path::pfstate->mastertrack = -1;
    Path::pfstate->mastersection = 0;
    for (t = 0; t < PATH_MAX_TRACKS; t++) {
        track = Path::pfstate->track[t];
        if (track != 0) {
            trackplaying = 0;
            if (track->paused == 0) {
                trackplaying = track->node >= 0 && track->entryinfo != 0;
                if (trackplaying != 0 && Path::pfstate->masterlatency < track->latency) {
                    Path::pfstate->masterlatency = track->latency;
                    Path::pfstate->mastertrack = t;
                    Path::pfstate->mastersection = track->entryinfo->sectionID;
                }
            }
        }
    }
}

int PATH_numtracks(unsigned int projects) {
    int numtracks;

    numtracks = 0;
    {
        int p;
        p = 0;
        do {
            if ((projects & (0x01000000 << p)) != 0 && Path::pfstates[p] != 0) {
                int t;
                t = 0;
                do {
                    if (Path::pfstates[p]->track[t] != 0) {
                        numtracks++;
                    }
                } while (++t < PATH_MAX_TRACKS);
            }
            p++;
        } while (p < PATH_MAX_PROJECTS);
    }
    return numtracks;
}

void PATHI_mainvoice(PATHTRACK *track, int mainvoice) {
    int trackID;
    int foundmain;

    trackID = track->trackID;
    foundmain = mainvoice > 0;
    track->mainvoice = mainvoice != 0;
    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++) {
            if (Path::pfstates[p] != 0) {
                PATHTRACK *thistrack;

                thistrack = Path::pfstates[p]->track[trackID];
                if (thistrack != 0 && thistrack != track) {
                    thistrack->mainvoice = foundmain ^ 1;
                    foundmain = 1;
                }
            }
        }
    }
    if (foundmain == 0) {
        track->mainvoice = 1;
    }
}

void PATHI_statusall(int clear) {
    {
        int p;

        for (p = 0; p < PATH_MAX_PROJECTS; p++) {
            if (Path::pfstates[p] != 0) {
                if (PATHI_switchproject(p, -1) != 0) {
                    {
                        int t;

                        for (t = 0; t < PATH_MAX_TRACKS; t++) {
                            {
                                PATHTRACK *track;

                                track = Path::pfstate->track[t];
                                if (track != 0) {
                                    if (track->trackimp != 0) {
                                        if (clear != 0) {
                                            track->trackimp->UpdateStatus();
                                        }
                                        else {
                                            track->trackimp->CheckStatus();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int Path::IPathTrack::GetNumSubBanks() {
    return this->mTrackInfo != 0 ? this->mTrackInfo->numsubbanks : 0;
}

int Path::IPathTrack::GetMaxSubBanks() {
    return this->mMaxSubBanks;
}

PATHSUBBANKSTATUS *Path::IPathTrack::GetSubBankPtr(int) {
    return 0;
}

PATHSUBBANKSTATUS *Path::IPathTrack::GetAvailSubBankPtr() {
    return 0;
}

int Path::IPathTrack::AddSubBank(int, void *) {
    return PATHERR_INV_PARAM;
}

int Path::IPathTrack::AddSubBankDone(int) {
    return PATHERR_INV_PARAM;
}

int Path::IPathTrack::DetachSubBankHeader(int, int) {
    return PATHERR_INV_PARAM;
}

int Path::IPathTrack::RemoveSubBank(int) {
    return PATHERR_INV_PARAM;
}

Path::IPathTrack::IPathTrack() {
    this->mVolume = 0x7f;
    this->mMaxSubBanks = 0;
    this->mUpdateStatus = 1;
    this->mStatus.timeremaining = -1;
    this->mHandle = 0;
    this->mStatusCode = 0;
    this->mFXSendLevel = 0;
    this->mDryLevel = 0x7f;
    this->mMaxRequests = 1;
    this->mActiveRequests = 0;
    this->mTrackInfo = 0;
    this->mPlayOpts = 0;
    this->mSubBanks = 0;
}

Path::IPathTrack::~IPathTrack() {}
