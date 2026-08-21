#include "pathi.h"

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
                        return (*(p + Path::pfstates))->track[t];
                    }
                }
            }
        }
    }
    return 0;
}

int PATH_numtracks(unsigned int projects) {
    int numtracks;

    numtracks = 0;
    {
        int p;
        p = 0;
        do {
            if ((projects & (0x01000000 << p)) != 0 && *(p + Path::pfstates) != 0) {
                int t;
                t = 0;
                do {
                    if ((*(p + Path::pfstates))->track[t] != 0) {
                        numtracks++;
                    }
                } while (++t < PATH_MAX_TRACKS);
            }
            p++;
        } while (p < PATH_MAX_PROJECTS);
    }
    return numtracks;
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
