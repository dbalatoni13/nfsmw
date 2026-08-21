#include "types.h"
#include "path/PathToSnd.h"
#include "pathi.h"
#include <string.h>

extern "C" {
int SNDSTRM_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize);
int SNDSTRM_overhead(int maxrequests, int maxchunks);
}

void PATH_vectortosnd() {
    if (Path::IPathToSnd::sndimp != 0) {
        delete Path::IPathToSnd::sndimp;
    }
    Path::IPathToSnd::sndimp = new Path::PathToSnd;
}

int Path::PathToSnd::CreateStreamTrack(Path::IPathTrack **ppPathTrack, int maxRequests,
                                       float bufferTime, int byteRate) {
    Path::PathTrackSndStream *streamimp;

    streamimp = new Path::PathTrackSndStream(maxRequests);
    if (bufferTime != 0.0f) {
        int maxchunks;
        int memsize;
        void *pmem;
        int shandle;

        if (byteRate == 0) {
            byteRate = 0x30;
        }
        maxchunks = static_cast<int>(bufferTime * 15.0f) + 1;
        memsize = static_cast<int>(static_cast<double>(bufferTime) * static_cast<double>(byteRate) * 1000.0);
        if (memsize < Path::IPathToReal::realimp->GetMinStreamBufferSize(maxRequests)) {
            memsize = Path::IPathToReal::realimp->GetMinStreamBufferSize(maxRequests);
        }
        memsize += SNDSTRM_overhead(maxRequests, maxchunks);
        pmem = PATHI_memalloc(memsize);
        shandle = PATHERR_FAILALLOC;
        if (pmem != 0) {
            shandle = SNDSTRM_create(static_cast<SNDPLAYOPTS *>(streamimp->GetPlayOpts()), maxRequests,
                                     maxchunks, pmem, memsize);
        }
        streamimp->AttachStreamInstance(shandle, static_cast<char *>(pmem));
    }
    *ppPathTrack = streamimp;
    return 0;
}

int Path::PathToSnd::CreateBankTrack(Path::IPathTrack **ppPathTrack, int maxsubbanks) {
    *ppPathTrack = new Path::PathTrackSndBank(maxsubbanks);
    return 0;
}

void *Path::PathToSnd::operator new(PATH_SIZE_T size) {
    return PATHI_memalloc(size);
}

void Path::PathToSnd::operator delete(void *ptr) {
    PATHI_memfree(ptr);
}

Path::PathToSnd::PathToSnd() {
    SNDSYS_add100hzclient(PATHI_servicetimer);
}

Path::PathToSnd::~PathToSnd() {
    SNDSYS_remove100hzclient(PATHI_servicetimer);
}

void Path::PathToSnd::GetDefaultPlayOpts(void *playopts) {
    SNDplaysetdef(static_cast<SNDPLAYOPTS *>(playopts));
}

Path::PathTrackSnd::PathTrackSnd() {
    this->mPath[0] = 0;
    this->mName[0] = 0;
    this->mSndPlayOpts = static_cast<SNDPLAYOPTS *>(PATHI_memalloc(sizeof(SNDPLAYOPTS)));
    this->mPlayOpts = this->mSndPlayOpts;
    Path::IPathToSnd::sndimp->GetDefaultPlayOpts(this->mSndPlayOpts);
    this->mHandle = -1;
    this->mPitchMult = this->mSndPlayOpts->pitchmult;
}

Path::PathTrackSnd::~PathTrackSnd() {
    PATHI_memfree(this->mSndPlayOpts);
}

int Path::PathTrackSnd::GetPitchMult() {
    return this->mSndPlayOpts->pitchmult;
}

int Path::PathTrackSnd::GetStretchMult() {
    return this->mSndPlayOpts->timemult;
}

int Path::PathTrackSnd::GetPathStatus(PATHSTATUS *psps) {
    int beatlen;
    int barlen;
    PATHFINDNODE *nodeinfo;
    int timeinnode;

    if (this->mUpdateStatus != 0) {
        this->CheckStatus();
    }
    beatlen = this->mStatus.nodeduration;
    barlen = beatlen;
    nodeinfo = PATHI_getnode(this->mStatus.playingnode);
    if (nodeinfo != 0 && nodeinfo->bars != 0) {
        this->mStatus.barduration = this->mStatus.nodeduration / nodeinfo->bars;
        barlen = this->mStatus.nodeduration / nodeinfo->bars;
        if (nodeinfo->beats != 0) {
            this->mStatus.beatduration =
                this->mStatus.nodeduration / (nodeinfo->beats * nodeinfo->bars);
            beatlen = beatlen / (nodeinfo->beats * nodeinfo->bars);
        }
    }
    timeinnode = this->mStatus.timeleftinnode;
    if (this->mStatus.beatduration == 0) {
        this->mStatus.playingbeat = 0;
    }
    else {
        this->mStatus.playingbeat =
            (this->mStatus.nodeduration - timeinnode) / this->mStatus.beatduration + 1;
    }
    if (barlen < 1) {
        this->mStatus.timeleftinbar = 0;
    }
    else {
        this->mStatus.timeleftinbar = timeinnode - timeinnode / barlen * barlen;
    }
    if (beatlen < 1) {
        this->mStatus.timeleftinbeat = 0;
    }
    else {
        this->mStatus.timeleftinbeat = timeinnode - timeinnode / beatlen * beatlen;
    }
    *psps = this->mStatus;
    return 1;
}

int Path::PathTrackSnd::ReadyForNewRequest() {
    if (this->mUpdateStatus != 0) {
        this->CheckStatus();
    }
    return this->mStatus.readyforrequest;
}

int Path::PathTrackSnd::TimeBuffered() {
    return this->mStatus.timebuffered;
}

int Path::PathTrackSnd::GetVolume() {
    return this->mVolume;
}

int Path::PathTrackSnd::SetVolume(int volume) {
    int current;

    if (static_cast<unsigned int>(volume) > 0x7f) {
        return PATHERR_INV_PARAM;
    }
    current = this->GetVolume();
    if (current != volume) {
        if (volume >= 0) {
            if (current >= 0) {
                return 0;
            }
        }
    }
    return -1;
}

int Path::PathTrackSnd::SetDryLevel(int level) {
    return 0;
}

int Path::PathTrackSnd::SetFXSendLevel(int bus, int level) {
    return 0;
}

int Path::PathTrackSnd::SetPitchMult(int pitchmult) {
    int current;

    if (static_cast<unsigned int>(pitchmult) > 0x4000) {
        return PATHERR_INV_PARAM;
    }
    current = this->GetPitchMult();
    if (current != pitchmult) {
        if (pitchmult >= 0) {
            if (current >= 0) {
                return 0;
            }
        }
    }
    return -1;
}

int Path::PathTrackSnd::SetStretchMult(int stretchmult) {
    int current;

    if (static_cast<unsigned int>(stretchmult - 0x800) > 0x1800) {
        return PATHERR_INV_PARAM;
    }
    current = this->GetStretchMult();
    if (current != stretchmult) {
        if (stretchmult >= 0) {
            if (current >= 0) {
                return 0;
            }
        }
    }
    return -1;
}

void Path::PathTrackSnd::SetName(const char *inName) {
    memcpy(this->mName, inName, sizeof(this->mName));
}

int Path::PathTrackSnd::Pause(int pause) {
    int pitchmult;
    int savepitchmult;

    pitchmult = 0;
    savepitchmult = 0;
    if (this->mPitchMult > 0) {
        savepitchmult = this->mPitchMult;
    }
    else {
        savepitchmult = 0;
        if (this->mSndPlayOpts->pitchmult != 0) {
            savepitchmult = this->mSndPlayOpts->pitchmult;
        }
    }
    if (pause == 0) {
        pitchmult = this->mPitchMult;
        savepitchmult = pitchmult;
    }
    this->SetPitchMult(pitchmult);
    this->mPitchMult = savepitchmult;
    return 0;
}

void *Path::PathTrackSndBank::operator new(PATH_SIZE_T size) {
    return PATHI_memalloc(size);
}

void Path::PathTrackSndBank::operator delete(void *ptr) {
    PATHI_memfree(ptr);
}

Path::PathTrackSndBank::PathTrackSndBank(int maxsubbanks) {
    int allocsize;

    allocsize = maxsubbanks * sizeof(PATHSUBBANKSTATUS);
    this->mRequest.duration = 0;
    this->mRequest.requesthandle = -1;
    this->mPatchNum = -1;
    this->mFXBus = 0;
    this->mRequest.timeremaining = 0;
    this->mSubBanks = static_cast<PATHSUBBANKSTATUS *>(PATHI_memalloc(allocsize));
    memset(this->mSubBanks, 0, allocsize);
    for (int i = 0; i < maxsubbanks; i++) {
        this->mSubBanks[i].subbanknum = -1;
    }
    this->mMaxSubBanks = maxsubbanks;
}

Path::PathTrackSndBank::~PathTrackSndBank() {
    for (int i = 0; i < this->GetNumSubBanks(); i++) {
        this->RemoveSubBank(i);
    }
    if (this->mSubBanks != 0) {
        PATHI_memfree(this->mSubBanks);
    }
}

int Path::PathTrackSndBank::AttachSubBank(int subbanknum, int bankhandle) {
    PATHSUBBANKSTATUS *subbank;

    if (this->GetSubBankPtr(subbanknum) != 0) {
        return -188;
    }
    subbank = this->GetAvailSubBankPtr();
    if (subbank != 0) {
        subbank->info.priority = 100;
        subbank->subbanknum = subbanknum;
        subbank->bankhandle = bankhandle;
        subbank->external = 1;
        subbank->ready = 1;
        return 0;
    }
    return -13;
}

int Path::PathTrackSndBank::DetachSubBank(int subbanknum) {
    PATHSUBBANKSTATUS *subbank;
    int bankhandle;

    subbank = this->GetSubBankPtr(subbanknum);
    if (subbank != 0) {
        bankhandle = subbank->bankhandle;
        this->RemoveSubBank(subbanknum);
        return bankhandle;
    }
    return PATHERR_INV_PARAM;
}

void Path::PathTrackSndBank::CheckStatus() {
    int timeleft;
    int status;
    int remainingbeats;

    memset(&this->mStatus, 0, sizeof(this->mStatus));
    this->mStatus.currentrequest = -1;
    timeleft = 0;
    this->mStatus.currentnode = -1;
    this->mStatus.playingnode = -1;
    status = PATHERR_INV_PARAM;
    this->mStatus.timeremaining = -1;
    remainingbeats = 0;
    this->mStatus.readyforrequest = 1;
    if (this->mHandle < 0) {
        return;
    }
    this->mUpdateStatus = 1;
    SNDSYS_entercritical();
    if (this->mPatchNum >= 0 && this->mRequest.requesthandle >= 0) {
        SNDSAMPLEFORMAT ssf;
        SNDSAMPLEATTR ssa;
        SNDSAMPLEDESC ssd;
        int duration;

        status = SNDBANK_patchinfo(this->mHandle, this->mPatchNum, &ssf, &ssa, &ssd);
        duration = 0;
        if (status < 0) {
            this->mRequest.requesthandle = -1;
            this->mHandle = -1;
            goto done;
        }
        timeleft = SNDtimeremaining(this->mRequest.requesthandle);
        if (timeleft < 0) {
            this->mRequest.requesthandle = -1;
            this->mHandle = -1;
            goto done;
        }
        {
            PATHFINDNODE *nodeinfo;
            int newvol;

            nodeinfo = 0;
            duration = static_cast<int>(
                static_cast<float>(ssd.totalframes) / static_cast<float>(ssf.samplerate) * 1000.0f);
            nodeinfo = PATHI_getnode(this->mRequest.node);
            if (nodeinfo->extra.beat.playbeats == 0) {
                this->mStatus.nodeduration = duration;
            }
            else {
                remainingbeats = nodeinfo->beats - 1 - this->mRequest.beat;
                this->mStatus.nodeduration = duration * nodeinfo->beats;
            }
            newvol = SNDCTRL_getprogvol(this->mRequest.requesthandle);
            if (newvol < 0) {
                this->mVolume = newvol;
            }
        }
        this->mStatus.playingbeat = this->mRequest.beat;
        this->mStatus.currentnode = this->mRequest.node;
        this->mStatus.timeleftinnode = duration * remainingbeats + timeleft;
        this->mStatus.timebuffered = duration;
        this->mStatus.currentrequest = this->mRequest.requesthandle;
        this->mRequest.duration = duration;
        this->mRequest.timeremaining = timeleft;
        this->mStatus.timeremaining = timeleft;
        this->mStatus.playingnode = this->mRequest.node;
    }
done:
    this->mStatusCode = status;
    this->mUpdateStatus = 0;
    SNDSYS_leavecritical();
}
