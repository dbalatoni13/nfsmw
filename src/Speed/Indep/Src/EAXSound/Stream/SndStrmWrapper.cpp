#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"

SndStrmWrapper::SndStrmWrapper() {
    this->m_handle = -1;
    this->m_StreamID = 0;
}

// STRIPPED
SndStrmWrapper::SndStrmWrapper(const int maxChunks, const int maxRequests, const int buffersize) {}

// STRIPPED
void SndStrmWrapper::SetStreamGreedyLevel(int nbytes) {}

int SndStrmWrapper::Create(const int maxChunks, const int maxRequests, const int buffersize) {
    SNDPLAYOPTS STRMopts;
    SNDplaysetdef(&STRMopts);
    STRMopts.vol = 100;
    STRMopts.fxlevel0 = 0;

    int overhead = SNDSTRM_overhead(maxRequests, maxChunks);
    this->m_RealStreamBuffer = reinterpret_cast<intptr_t>(this->m_buffer) + overhead;
    overhead += buffersize;
    this->m_BufferSize = overhead;
    this->m_buffer = gAudioMemoryManager.AllocateMemoryChar(overhead, "AUD:Stream buffer", false);
    return this->CreateStream(maxChunks, maxRequests, this->m_buffer, buffersize, &STRMopts);
}

int SndStrmWrapper::CreateStream(const int maxChunks, const int maxRequests, char *pmem, const int buffersize, void *pplayopts) {
    this->m_buffer = pmem;
    this->m_handle = SNDSTRM_create(static_cast<SNDPLAYOPTS *>(pplayopts), maxRequests, maxChunks, pmem, buffersize);
    this->m_BufferSize = buffersize;

    if (this->m_handle >= 0) {
        this->m_vol = 0;
        SNDSYS_entercritical();
        SNDSTRM_autovol(this->m_handle, 0, 0);
        SNDSYS_leavecritical();
    } else {
        gAudioMemoryManager.FreeMemory(this->m_buffer);
    }

    return this->m_handle;
}

bool SndStrmWrapper::IsPlaying() {
    SNDREQUESTSTATUS srs;
    SNDSTREAMSTATUS sss;
    SNDSYS_entercritical();
    SNDSTRM_status(this->m_handle, &sss);

    int reqHandle = sss.currentrequest;
    if (reqHandle < 0) {
        SNDSYS_leavecritical();
    } else {
        SNDSTRM_requeststatus(reqHandle, &srs);
        SNDSYS_leavecritical();
        if ((srs.state != 3) || (sss.outstandingrequests > 0)) {
            return true;
        }
    }
    return false;
}

// STRIPPED
int SndStrmWrapper::IsStrmQueued() {}

int SndStrmWrapper::GetCurrentTime() {
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;
    this->GetStatus(&sss);
    this->GetRequestStatus(sss.currentrequest, &srs);
    return srs.currenttime;
}

int SndStrmWrapper::GetTimeRemaining() {
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;
    this->GetStatus(&sss);
    this->GetRequestStatus(sss.currentrequest, &srs);
    return srs.timetoend;
}

bool SndStrmWrapper::AlmostDone() {
    int itemsinq;
    int timeremaining;
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;
    this->GetStatus(&sss);
    itemsinq = sss.outstandingrequests;
    if (itemsinq == 0) {
        return true;
    }

    if (itemsinq == 1) {
        SNDSYS_entercritical();
        SNDSTRM_requeststatus(sss.currentrequest, &srs);
        SNDSYS_leavecritical();
        if (srs.state == 3) {
            return true;
        }
        if (srs.state != 0) {
            if (srs.state != 1) {
                if (srs.timetoend < 100) {
                    return true;
                }
            }
        }
    }

    return false;
}

int SndStrmWrapper::Stop() {
    if (SNDSTRM_purge(this->m_handle) < 0) {
        return -3;
    }
    return 0;
}

// STRIPPED
int SndStrmWrapper::Play(const char *filename, long offset) {}

int SndStrmWrapper::AddToStream(const char *filename, long offset, int holdtime) {
    int request;
    if ((request = SNDSTRM_queuefile(this->m_handle, holdtime, const_cast<char *>(filename), offset)) == 0) {
        return request;
    }
    return request;
}

int SndStrmWrapper::AddToStream(int holdtime, void *paddr, int length, int offset) {
    int request = SNDSTRM_queuemem(this->m_handle, holdtime, paddr, offset << 7);
    return request;
}

int SndStrmWrapper::ModifyHold(int sndrequesthandle, int holdtime) {
    return SNDSTRM_modifyhold(sndrequesthandle, holdtime);
}

int SndStrmWrapper::SetVol(int vol, bool bramp) {
    if (bramp == true) {
        return this->RampVol(vol, 0xFA);
    }

    int result;
    this->m_vol = vol;
    SNDSYS_entercritical();
    result = SNDSTRM_setvol(this->m_handle, Snd::CHANNEL_ALL, static_cast<float>(this->m_vol) * 0.007874016f);
    SNDSYS_leavecritical();
    return result;
}

int SndStrmWrapper::SetAz(int Azimuth) {
    int result;
    SNDSYS_entercritical();
    result = SNDSTRM_setazimuth(this->m_handle, Snd::CHANNEL_ALL, static_cast<float>(Azimuth) * 0.005493248f);
    SNDSYS_leavecritical();
    return result;
}

int SndStrmWrapper::RampVol(int vol, int time) {
    if (vol < 0) {
        vol = 0;
    }
    if (vol > 100) {
        vol = 100;
    }
    if (time < 0) {
        return -5;
    }

    this->m_vol = vol;
    SNDSYS_entercritical();
    int result = SNDSTRM_autovol(this->m_handle, time, (this->m_vol * 127) / 100);
    SNDSYS_leavecritical();
    if (result < 0) {
        return -3;
    }
    return 0;
}

int SndStrmWrapper::GetVol() {}

// STRIPPED
int SndStrmWrapper::SetFXLevel(int bus, int level) {}

// STRIPPED
int SndStrmWrapper::SetDryLevel(int level) {}

int DEBUG_STREAMS; // size: 0x4, Decl: 447

// STRIPPED
int SndStrmWrapper::SetPitchMult(int pitchmult) {}

// STRIPPED
int SndStrmWrapper::SetTimeMult(int timemult) {}

int SndStrmWrapper::SetLowPass(int lowpass) {
    SNDSYS_entercritical();
    int ret = SNDSTRM_lowpass(this->m_handle, lowpass);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetStatus(SNDSTREAMSTATUS *sss) {
    SNDSYS_entercritical();
    int ret = SNDSTRM_status(this->m_handle, sss);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetRequestStatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs) {
    SNDSYS_entercritical();
    int ret = SNDSTRM_requeststatus(sndrequesthandle, psrs);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetTimeBuffered() {
    SNDSTREAMSTATUS sss;
    if (this->GetStatus(&sss) >= 0) {
        return sss.timebuffered;
    }
    return 0;
}

void SndStrmWrapper::Pause() {
    SNDSYS_entercritical();
    SNDSTRM_pitchmult(this->m_handle, 0);
    SNDSYS_leavecritical();
}

void SndStrmWrapper::Resume() {
    SNDSYS_entercritical();
    SNDSTRM_pitchmult(this->m_handle, 0x1000);
    SNDSYS_leavecritical();
}

SndStrmWrapper::~SndStrmWrapper() {
    if (this->m_handle >= 0) {
        this->DestroyStream();
        if (this->m_buffer != nullptr) {
            gAudioMemoryManager.FreeMemory(this->m_buffer);
        }
    }
}

void SndStrmWrapper::DestroyStream() {
    if (this->m_handle >= 0) {
        this->Stop();
        unsigned int time = bGetTicker() + 0x14;
        while (time > bGetTicker()) {
            bSyncTaskRun();
        }
        SNDSTRM_destroy(this->m_handle);
    }
}

int SndStrmWrapper::PurgeStream() {
    if (SNDSTRM_purge(this->m_handle) < 0) {
        return -3;
    }
    return 0;
}
