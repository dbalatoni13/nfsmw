#include "types.h"
#define PATH_SND_EMIT_METHODS
#include "path/PathToSnd.h"
#undef PATH_SND_EMIT_METHODS
#include "pathi.h"
#include <string.h>

extern "C" {
int SNDSTRM_create(SNDPLAYOPTS *pspo, int maxrequests, int maxchunks, void *pmem, int memsize);
int SNDSTRM_overhead(int maxrequests, int maxchunks);
int SNDSTRM_destroy(int streamhandle);
int SNDSTRM_setvol(int streamhandle, int channel, float volume);
int SNDSTRM_drylevel(int streamhandle, int level);
int SNDSTRM_fxlevel(int streamhandle, int bus, int level);
int SNDSTRM_modifyhold(int requesthandle, int holdtime);
int SNDSTRM_pitchmult(int streamhandle, int pitchmult);
int SNDSTRM_timemult(int streamhandle, int timemult);
int SNDSTRM_queuemem(int streamhandle, int holdtime, char *buffer, int offset);
int SNDSTRM_queuefile(int streamhandle, int holdtime, char *filename, int offset);
int SNDSTRM_purge(int streamhandle);
int SNDSTRM_status(int streamhandle, SNDSTREAMSTATUS *status);
int SNDSTRM_requeststatus(int requesthandle, SNDREQUESTSTATUS *status);
int SNDCTRL_timemult(int shandle, int timemult);
int SNDstop(int shandle);
}

Path::IPathToSnd::~IPathToSnd() {}

void PATH_vectortosnd() {
    if (Path::IPathToSnd::sndimp != 0) {
        delete Path::IPathToSnd::sndimp;
    }
    Path::IPathToSnd::sndimp = new Path::PathToSnd;
}

int Path::PathToSnd::CreateStreamTrack(Path::IPathTrack **ppPathTrack, int maxRequests,
                                       float bufferTime, int byteRate) {
    int result;
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
        memsize = static_cast<int>(bufferTime * byteRate * 1000.0f);
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
    result = 0;
    return result;
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
    this->mName[0] = 0;
    this->mPath[0] = 0;
    this->mSndPlayOpts = static_cast<SNDPLAYOPTS *>(PATHI_memalloc(sizeof(SNDPLAYOPTS)));
    this->mPlayOpts = this->mSndPlayOpts;
    Path::IPathToSnd::sndimp->GetDefaultPlayOpts(this->mSndPlayOpts);
    this->mPitchMult = this->mSndPlayOpts->pitchmult;
    this->mHandle = -1;
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
        barlen = beatlen / nodeinfo->bars;
        if (nodeinfo->beats != 0) {
            this->mStatus.beatduration =
                this->mStatus.nodeduration / (nodeinfo->beats * nodeinfo->bars);
            beatlen = beatlen / (nodeinfo->beats * nodeinfo->bars);
        }
    }
    timeinnode = this->mStatus.nodeduration - this->mStatus.timeleftinnode;
    if (this->mStatus.beatduration != 0) {
        timeinnode = timeinnode / this->mStatus.beatduration + 1;
    }
    else {
        timeinnode = 0;
    }
    this->mStatus.playingbeat = timeinnode;
    if (barlen >= 1) {
        this->mStatus.timeleftinbar = this->mStatus.timeleftinnode -
                                      this->mStatus.timeleftinnode / barlen * barlen;
    }
    else {
        this->mStatus.timeleftinbar = 0;
    }
    if (beatlen >= 1) {
        this->mStatus.timeleftinbeat = this->mStatus.timeleftinnode -
                                       this->mStatus.timeleftinnode / beatlen * beatlen;
    }
    else {
        this->mStatus.timeleftinbeat = 0;
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
    this->mPatchNum = -1;
    this->mFXBus = 0;
    this->mRequest.duration = this->mRequest.timeremaining = 0;
    this->mRequest.requesthandle = -1;
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
    if (subbank == 0) {
        return -13;
    }
    subbank->subbanknum = subbanknum;
    subbank->bankhandle = bankhandle;
    subbank->ready = 1;
    subbank->external = 1;
    subbank->info.priority = 100;
    return 0;
}

int Path::PathTrackSndBank::DetachSubBank(int subbanknum) {
    PATHSUBBANKSTATUS *subbank;
    int bankhandle;

    subbank = this->GetSubBankPtr(subbanknum);
    if (subbank == 0) {
        return PATHERR_INV_PARAM;
    }
    bankhandle = subbank->bankhandle;
    this->RemoveSubBank(subbanknum);
    return bankhandle;
}

void Path::PathTrackSndBank::CheckStatus() {
    int timeleft;
    int status;
    int remainingbeats;

    memset(&this->mStatus, 0, sizeof(this->mStatus));
    this->mStatus.playingnode = this->mStatus.currentnode = this->mStatus.currentrequest = -1;
    timeleft = 0;
    this->mStatus.timeremaining = -1;
    status = PATHERR_INV_PARAM;
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

        duration = 0;
        status = SNDBANK_patchinfo(this->mHandle, this->mPatchNum, &ssf, &ssa, &ssd);
        if (status >= 0) {
            timeleft = SNDtimeremaining(this->mRequest.requesthandle);
            if (timeleft < 0) {
                this->mHandle = -1;
                this->mRequest.requesthandle = -1;
                goto done;
            }
            else {
                PATHFINDNODE *nodeinfo;
                int newvol;

                timeleft *= 10;
                nodeinfo = 0;
                duration = static_cast<int>(
                    static_cast<float>(ssd.totalframes) /
                    static_cast<float>(static_cast<int>(ssf.samplerate)) * 1000.0f);
                nodeinfo = PATHI_getnode(this->mRequest.node);
                if (nodeinfo->extra.beat.playbeats != 0) {
                    remainingbeats = nodeinfo->beats - 1 - this->mRequest.beat;
                    this->mStatus.nodeduration = duration * nodeinfo->beats;
                }
                else {
                    this->mStatus.nodeduration = duration;
                }
                newvol = SNDCTRL_getprogvol(this->mRequest.requesthandle);
                if (newvol < 0) {
                    this->mVolume = newvol;
                }
            }
        }
        else {
            this->mRequest.node = -1;
            this->mHandle = -1;
            this->mRequest.requesthandle = -1;
        }
        this->mRequest.duration = duration;
        this->mRequest.timeremaining = timeleft;
        this->mStatus.timeremaining = timeleft;
        this->mStatus.timeleftinnode = duration * remainingbeats + timeleft;
        this->mStatus.playingnode = this->mRequest.node;
        this->mStatus.playingbeat = this->mRequest.beat;
        this->mStatus.currentnode = this->mRequest.node;
        this->mStatus.timebuffered = duration;
        this->mStatus.currentrequest = this->mRequest.requesthandle;
    }
done:
    this->mStatusCode = status;
    this->mUpdateStatus = 0;
    SNDSYS_leavecritical();
}

int Path::PathTrackSndBank::SetVolume(int volume) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetVolume(volume) == 0) {
        SNDSYS_entercritical();
        result = SNDvol(this->mRequest.requesthandle, volume);
        SNDSYS_leavecritical();
        this->mVolume = volume;
        this->mSndPlayOpts->vol = volume;
    }
    return result;
}

int Path::PathTrackSndBank::SetDryLevel(int drylevel) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetDryLevel(drylevel) == 0) {
        SNDSYS_entercritical();
        result = SNDCTRL_drylevel(this->mRequest.requesthandle, drylevel);
        SNDSYS_leavecritical();
        this->mDryLevel = drylevel;
        this->mSndPlayOpts->drylevel = drylevel;
    }
    return result;
}

int Path::PathTrackSndBank::SetFXSendLevel(int bus, int level) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetFXSendLevel(bus, level) == 0) {
        SNDSYS_entercritical();
        result = SNDfxlevel(this->mRequest.requesthandle, bus, level);
        SNDSYS_leavecritical();
        this->mFXSendLevel = level;
        this->mFXBus = bus;
    }
    return result;
}

int Path::PathTrackSndBank::SetPitchMult(int pitchmult) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetPitchMult(pitchmult) == 0) {
        SNDSYS_entercritical();
        result = SNDpitchmult(this->mRequest.requesthandle, pitchmult);
        SNDSYS_leavecritical();
        this->mPitchMult = pitchmult;
        this->mSndPlayOpts->pitchmult = pitchmult;
    }
    return result;
}

int Path::PathTrackSndBank::SetStretchMult(int stretchmult) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetStretchMult(stretchmult) == 0) {
        SNDSYS_entercritical();
        result = SNDCTRL_timemult(this->mRequest.requesthandle, stretchmult);
        SNDSYS_leavecritical();
        this->mSndPlayOpts->timemult = stretchmult;
    }
    return result;
}

int Path::PathTrackSndBank::TimeRemaining(int request) {
    if (request == 0) {
        return this->mStatus.timeremaining;
    }
    return -1;
}

int Path::PathTrackSndBank::Play(int node, unsigned int offset, int beat, int, unsigned int duration) {
    int subbanknum;
    subbanknum = static_cast<int>(offset >> 16);
    PATHSUBBANKSTATUS *subbank;
    subbank = this->GetSubBankPtr(subbanknum);
    if (subbank == 0 || subbank->bankhandle < 0) {
        return -1;
    }
    this->mPatchNum = (offset & 0xffff) + beat - 1;
    SNDSYS_entercritical();
    int subbankhandle;
    subbankhandle = subbank->bankhandle;
    if (this->mRequest.requesthandle >= 0) {
        SNDstop(this->mRequest.requesthandle);
    }
    int returnhandle;
    returnhandle = SNDBANK_play(subbank->bankhandle, this->mPatchNum, this->mSndPlayOpts);
    if (returnhandle >= 0) {
        SNDfxlevel(returnhandle, this->mFXBus, this->mFXSendLevel);
    }
    this->mHandle = subbankhandle;
    SNDSYS_leavecritical();
    if (returnhandle < 0) {
        this->mRequest.requesthandle = -1;
    }
    else {
        this->mRequest.node = node;
        this->mRequest.beat = beat - 1;
        this->mRequest.requesthandle = returnhandle;
        this->mRequest.duration = duration;
        subbank->lastplaytime = Path::milliseconds;
        this->mUpdateStatus = 1;
    }
    return returnhandle;
}

int Path::PathTrackSndBank::Stop() {
    int result;

    SNDSYS_entercritical();
    result = SNDstop(this->mRequest.requesthandle);
    SNDSYS_leavecritical();
    this->mStatus.readyforrequest = 1;
    this->mRequest.requesthandle = -1;
    this->mHandle = -1;
    this->UpdateStatus();
    return result;
}

int Path::PathTrackSndBank::AddSubBank(int subbanknum, void *pbank) {
    int status;
    PATHSUBBANKSTATUS *subbank;
    char *bankdata;

    status = -8;
    subbank = this->GetSubBankPtr(subbanknum);
    bankdata = static_cast<char *>(pbank) + 0x100;
    if (subbank != 0) {
        status = SNDBANK_asyncloadmem(&subbank->bankhandle, bankdata);
    }
    return status;
}

int Path::PathTrackSndBank::AddSubBankDone(int subbanknum) {
    int status;
    PATHSUBBANKSTATUS *subbank;

    status = 0;
    subbank = this->GetSubBankPtr(subbanknum);
    if (subbank != 0) {
        status = SNDBANK_asyncloadmemdone();
    }
    return status;
}

PATHSUBBANKSTATUS *Path::PathTrackSndBank::GetSubBankPtr(int subbanknum) {
    for (int i = 0; i < this->GetMaxSubBanks(); i++) {
        if (this->mSubBanks[i].subbanknum == subbanknum) {
            return &this->mSubBanks[i];
        }
    }
    return 0;
}

PATHSUBBANKSTATUS *Path::PathTrackSndBank::GetAvailSubBankPtr() {
    for (int i = 0; i < this->GetMaxSubBanks(); i++) {
        if (this->mSubBanks[i].subbanknum < 0) {
            return &this->mSubBanks[i];
        }
    }
    return 0;
}

int Path::PathTrackSndBank::RemoveSubBank(int subbanknum) {
    PATHSUBBANKSTATUS *subbank;
    int result;

    result = 0;
    subbank = this->GetSubBankPtr(subbanknum);
    if (subbank != 0 && subbank->bankhandle != -1) {
        if (subbank->external == 0) {
            result = SNDbankremove(subbank->bankhandle);
            PATHI_memfree(subbank->filedata);
        }
        memset(subbank, 0, sizeof(PATHSUBBANKSTATUS));
        subbank->bankhandle = -1;
        subbank->subbanknum = -1;
    }
    return result;
}

int Path::PathTrackSndBank::DetachSubBankHeader(int subbanknum, int status) {
    PATHSUBBANKSTATUS *subbank;
    int result;

    result = -8;
    subbank = this->GetSubBankPtr(subbanknum);
    if (subbank != 0 && subbank->bankhandle != -1 && status == 7) {
        int headersize;
        char *newbankhdr;

        headersize = SNDbankheadersize(subbank->bankhandle);
        newbankhdr = static_cast<char *>(PATHI_memalloc(headersize));
        if (newbankhdr != 0) {
            result = 0;
            SNDbankheadercopy(newbankhdr, subbank->bankhandle);
            PATHI_memfree(subbank->filedata);
            subbank->filedata = newbankhdr;
        }
    }
    return result;
}

void *Path::PathTrackSndStream::operator new(size_t size) {
    return PATHI_memalloc(size);
}

void Path::PathTrackSndStream::operator delete(void *ptr) {
    PATHI_memfree(ptr);
}

Path::PathTrackSndStream::PathTrackSndStream(int maxrequests) {
    this->mMaxRequests = maxrequests;
    if (this->mMaxRequests < 1) {
        this->mMaxRequests = 1;
    }
    this->mRequests = static_cast<PATHQUEUEINFO *>(
        PATHI_memalloc(this->mMaxRequests * sizeof(PATHQUEUEINFO)));
    if (this->mRequests == 0) {
        this->mMaxRequests = 0;
    }
    for (int i = 0; i < this->mMaxRequests; i++) {
        this->mRequests[i].duration = this->mRequests[i].timeremaining = 0;
        this->mRequests[i].requesthandle = -1;
    }
    this->mCacheMem = 0;
    this->mCacheSize = 0;
    this->mBuffer = 0;
}

Path::PathTrackSndStream::~PathTrackSndStream() {
    int sndstreamhandle;

    sndstreamhandle = this->mHandle;
    if (sndstreamhandle >= 0) {
        SNDSTRM_destroy(sndstreamhandle);
        if (this->mBuffer != 0) {
            PATHI_memfree(this->mBuffer);
        }
    }
    PATHI_memfree(this->mRequests);
}

int Path::PathTrackSndStream::AttachStreamInstance(int streamhandle, char *buffer) {
    if (this->mHandle >= 0) {
        return -188;
    }
    this->mHandle = streamhandle;
    this->mUpdateStatus = 1;
    this->mBuffer = buffer;
    return 0;
}

int Path::PathTrackSndStream::DetachStreamInstance(char *&buffer) {
    int streamhandle;

    streamhandle = this->mHandle;
    if (streamhandle < 0) {
        return -43;
    }
    this->mHandle = -1;
    buffer = this->mBuffer;
    this->mBuffer = 0;
    return streamhandle;
}

int Path::PathTrackSndStream::GetPathStatus(PATHSTATUS *psps) {
    int pitchmult;
    int timestretch;

    this->PathTrackSnd::GetPathStatus(psps);
    pitchmult = this->mSndPlayOpts->pitchmult;
    timestretch = this->mSndPlayOpts->timemult;
    if (pitchmult != 0x1000 || timestretch != 0x1000) {
        double fudge = pitchmult != 0 ? 4096.0 / pitchmult : 0.0;

        if (timestretch != 0) {
            fudge *= (1.0 / 4096.0) * timestretch;
        } else {
            fudge = 0.0;
        }
        if (fudge == 0.0) {
            this->mStatus.timeremaining = 0x7fffffff;
            this->mStatus.timebuffered = 0x7fffffff;
            this->mStatus.timeleftinbeat = 0x7fffffff;
            this->mStatus.timeleftinbar = 0x7fffffff;
            this->mStatus.timeleftinnode = 0x7fffffff;
            this->mStatus.beatduration = 0x7fffffff;
            this->mStatus.barduration = 0x7fffffff;
            this->mStatus.nodeduration = 0x7fffffff;
        } else {
            this->mStatus.nodeduration = static_cast<unsigned int>(
                fudge * static_cast<double>(this->mStatus.nodeduration));
            this->mStatus.barduration = static_cast<unsigned int>(
                fudge * static_cast<double>(this->mStatus.barduration));
            this->mStatus.beatduration = static_cast<unsigned int>(
                fudge * static_cast<double>(this->mStatus.beatduration));
            this->mStatus.timeleftinnode = static_cast<unsigned int>(
                fudge * static_cast<double>(this->mStatus.timeleftinnode));
            this->mStatus.timeleftinbar = static_cast<unsigned int>(
                fudge * static_cast<double>(this->mStatus.timeleftinbar));
            this->mStatus.timeleftinbeat = static_cast<unsigned int>(
                fudge * static_cast<double>(this->mStatus.timeleftinbeat));
            this->mStatus.timebuffered = static_cast<int>(
                fudge * static_cast<double>(this->mStatus.timebuffered));
            this->mStatus.timeremaining = static_cast<int>(
                fudge * static_cast<double>(this->mStatus.timeremaining));
        }
        *psps = this->mStatus;
    }
    return 1;
}

void Path::PathTrackSndStream::CheckStatus() {
    SNDSTREAMSTATUS sss;

    memset(&this->mStatus, 0, sizeof(PATHSTATUS));
    this->mStatus.currentrequest = -1;
    this->mStatus.currentnode = -1;
    this->mStatus.playingnode = -1;
    if (this->mHandle >= 0) {
        this->mUpdateStatus = 1;
        this->mStatus.readyforrequest = 1;
        SNDSYS_entercritical();
        SNDSTRM_status(this->mHandle, &sss);
        this->mStatus.timeremaining = 0;
        this->mStatus.timebuffered = 0;
        if (sss.outstandingrequests >= this->mMaxRequests) {
            this->mStatus.readyforrequest = 0;
        }
        {
            int q;

            for (q = 0; q < this->mMaxRequests; q++) {
                this->mRequests[q].timeremaining = 0;
                if (this->mRequests[q].requesthandle >= 0) {
                    SNDREQUESTSTATUS srs;

                    SNDSTRM_requeststatus(this->mRequests[q].requesthandle, &srs);
                    switch (srs.state) {
                    case 1:
                        this->mRequests[q].timeremaining = this->mRequests[q].duration;
                        this->mStatus.timeremaining += this->mRequests[q].duration;
                        this->mStatus.readyforrequest = 0;
                        this->mStatus.timebuffered += srs.timebuffered;
                        break;
                    case 0:
                    case 2:
                            this->mRequests[q].timeremaining = srs.timetoend;
                            if (srs.currenttime == 0 && srs.timetoend == 0) {
                                this->mRequests[q].timeremaining = this->mRequests[q].duration;
                            }
                            this->mStatus.timeleftinnode = this->mRequests[q].timeremaining;
                            this->mStatus.timeremaining += this->mStatus.timeleftinnode;
                            this->mStatus.timebuffered += srs.timebuffered;
                            if (srs.timebuffered < srs.timetoend) {
                                this->mStatus.readyforrequest = 0;
                            }
                        break;
                    default:
                        this->mRequests[q].requesthandle = -1;
                        this->mRequests[q].node = -1;
                        break;
                    }
                    if (this->mRequests[q].node >= 0) {
                        this->mStatus.playingnode = this->mRequests[q].node;
                        this->mStatus.playingbeat = this->mRequests[q].beat;
                    }
                }
            }
        }
        this->mStatus.currentnode = this->mRequests[0].node;
        this->mStatus.currentrequest = this->mRequests[0].requesthandle;
        this->mStatus.nodeduration = this->mRequests[0].duration;
        this->mVolume = SNDSTRM_getprogvol(this->mHandle);
        this->mUpdateStatus = 0;
        if (Path::timercallsinarow > 0 && this->mStatus.timeremaining > 0) {
            int correction;

            correction = Path::timercallsinarow * Path::pfstate->timerinterval;
            this->mStatus.timebuffered -= correction;
            this->mStatus.timeremaining -= correction;
            this->mStatus.timeleftinnode -= correction;
            if (this->mStatus.timeremaining < 0) {
                this->mStatus.timeremaining = 0;
            }
            if (this->mStatus.timebuffered < 0) {
                this->mStatus.timebuffered = 0;
            }
            if (static_cast<int>(this->mStatus.timeleftinnode) < 0) {
                this->mStatus.timeleftinnode = 0;
            }
        }
        SNDSYS_leavecritical();
    }
}

int Path::PathTrackSndStream::SetVolume(int volume) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetVolume(volume) == 0) {
        SNDSYS_entercritical();
        result = SNDSTRM_setvol(this->mHandle, -1, static_cast<float>(volume) / 127.0f);
        SNDSYS_leavecritical();
        this->mVolume = volume;
        this->mSndPlayOpts->vol = static_cast<signed char>(volume);
    }
    return result;
}

int Path::PathTrackSndStream::SetDryLevel(int drylevel) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetDryLevel(drylevel) == 0) {
        SNDSYS_entercritical();
        result = SNDSTRM_drylevel(this->mHandle, drylevel);
        SNDSYS_leavecritical();
        this->mDryLevel = drylevel;
    }
    return result;
}

int Path::PathTrackSndStream::SetFXSendLevel(int bus, int level) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetFXSendLevel(bus, level) == 0) {
        SNDSYS_entercritical();
        result = SNDSTRM_fxlevel(this->mHandle, bus, level);
        SNDSYS_leavecritical();
        this->mFXSendLevel = level;
    }
    return result;
}

int Path::PathTrackSndStream::ModifyHold(int holdtime) {
    return SNDSTRM_modifyhold(this->mRequests->requesthandle, holdtime);
}

int Path::PathTrackSndStream::SetPitchMult(int pitchmult) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetPitchMult(pitchmult) == 0) {
        SNDSYS_entercritical();
        result = SNDSTRM_pitchmult(this->mHandle, pitchmult);
        SNDSYS_leavecritical();
        this->mSndPlayOpts->pitchmult = static_cast<unsigned short>(pitchmult);
        this->mPitchMult = pitchmult;
    }
    return result;
}

int Path::PathTrackSndStream::SetStretchMult(int stretchmult) {
    int result;

    result = 0;
    if (this->PathTrackSnd::SetStretchMult(stretchmult) == 0) {
        SNDSYS_entercritical();
        result = SNDSTRM_timemult(this->mHandle, stretchmult);
        SNDSYS_leavecritical();
        this->mSndPlayOpts->timemult = static_cast<unsigned short>(stretchmult);
    }
    return result;
}

void Path::PathTrackSndStream::StreamCache(char *pcache, int cachesize) {
    this->mCacheMem = pcache;
    this->mCacheSize = cachesize;
}

int Path::PathTrackSndStream::TimeRemaining(int request) {
    if (request < 0) {
        return this->mStatus.timeremaining;
    }
    if (request < this->mMaxRequests && this->mRequests[request].requesthandle >= 0) {
        return this->mRequests[request].timeremaining;
    }
    return -8;
}

int Path::PathTrackSndStream::Play(int node, unsigned int offset, int, int holdtime,
                                   unsigned int duration) {
    int requesthandle;

    if (this->mCacheMem != 0 && static_cast<int>(offset << 7) < this->mCacheSize) {
        requesthandle = SNDSTRM_queuemem(this->mHandle, holdtime, this->mCacheMem,
                                         offset << 7);
    } else {
        requesthandle = SNDSTRM_queuefile(this->mHandle, holdtime, this->mPath, offset << 7);
    }
    if (requesthandle >= 0) {
        this->mActiveRequests = this->mMaxRequests;
        {
            int q;

            for (q = this->mMaxRequests - 1; q > 0; q--) {
                this->mRequests[q] = this->mRequests[q - 1];
                if (this->mRequests[q].requesthandle < 0) {
                    this->mActiveRequests = q - 1;
                }
            }
        }
        this->mRequests[0].requesthandle = requesthandle;
        this->mRequests[0].duration = duration;
        this->mRequests[0].node = node;
        this->mRequests[0].beat = -1;
        this->mStatus.readyforrequest = 0;
    }
    this->mUpdateStatus = 1;
    return requesthandle;
}

int Path::PathTrackSndStream::Stop() {
    int result;

    SNDSYS_entercritical();
    result = SNDSTRM_purge(this->mHandle);
    SNDSYS_leavecritical();
    this->mStatus.readyforrequest = 1;
    this->UpdateStatus();
    return result;
}

void Path::PathTrackSndStream::SetFilePath(char *path) {
    memcpy(this->mPath, path, 128);
}
