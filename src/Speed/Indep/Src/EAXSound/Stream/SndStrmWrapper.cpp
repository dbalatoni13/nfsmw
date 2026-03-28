#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

SndStrmWrapper::SndStrmWrapper() {
    m_handle = -1;
    m_StreamID = 0;
}

int SndStrmWrapper::Create(int maxChunks, int maxRequests, int buffersize) {
    SNDPLAYOPTS STRMopts;
    SNDplaysetdef(&STRMopts);
    STRMopts.vol = 100;
    STRMopts.fxlevel0 = 0;

    int overhead = SNDSTRM_overhead(maxRequests, maxChunks);
    int total = overhead + buffersize;
    m_BufferSize = total;
    m_RealStreamBuffer = reinterpret_cast<int>(m_buffer + overhead);
    char *pmem = gAudioMemoryManager.AllocateMemoryChar(total, "AUD:Stream buffer", false);
    m_buffer = pmem;
    return CreateStream(maxChunks, maxRequests, pmem, buffersize, &STRMopts);
}

int SndStrmWrapper::CreateStream(int maxChunks, int maxRequests, char *pmem, int buffersize, void *pplayopts) {
    m_buffer = pmem;
    m_handle = SNDSTRM_create(static_cast<SNDPLAYOPTS *>(pplayopts), maxRequests, maxChunks, pmem, buffersize);
    m_BufferSize = buffersize;

    if (m_handle >= 0) {
        m_vol = 0;
        SNDSYS_entercritical();
        SNDSTRM_autovol(m_handle, 0, 0);
        SNDSYS_leavecritical();
    } else {
        gAudioMemoryManager.FreeMemory(m_buffer);
    }

    return m_handle;
}

SndStrmWrapper::~SndStrmWrapper() {
    if (m_handle >= 0) {
        DestroyStream();
        if (m_buffer != nullptr) {
            gAudioMemoryManager.FreeMemory(m_buffer);
        }
    }
}

int SndStrmWrapper::Stop() {
    if (SNDSTRM_purge(m_handle) >= 0) {
        return 0;
    }
    return -3;
}

int SndStrmWrapper::AddToStream(const char *filename, long offset, int holdtime) {
    int request = SNDSTRM_queuefile(m_handle, holdtime, filename, offset);
    register int ret asm("r0");
    asm volatile("mr. %0, %1" : "=r"(ret) : "r"(request));
    return ret;
}

int SndStrmWrapper::AddToStream(int holdtime, void *paddr, int length, int offset) {
    int request = SNDSTRM_queuemem(m_handle, holdtime, paddr, offset << 7);
    return request;
}

int SndStrmWrapper::ModifyHold(int sndrequesthandle, int holdtime) {
    return SNDSTRM_modifyhold(sndrequesthandle, holdtime);
}

bool SndStrmWrapper::IsPlaying() {
    SNDREQUESTSTATUS srs;
    SNDSTREAMSTATUS sss;
    SNDSYS_entercritical();
    SNDSTRM_status(m_handle, &sss);
    if (sss.currentrequest < 0) {
        SNDSYS_leavecritical();
    } else {
        SNDSTRM_requeststatus(sss.currentrequest, &srs);
        SNDSYS_leavecritical();
        if ((srs.state != 3) || (sss.outstandingrequests > 0)) {
            return true;
        }
    }
    return false;
}

int SndStrmWrapper::SetVol(int vol, bool bramp) {
    if (bramp == true) {
        return RampVol(vol, 0xFA);
    }

    int result;
    m_vol = vol;
    SNDSYS_entercritical();
    result = SNDSTRM_setvol(m_handle, -1, static_cast<float>(m_vol) * 0.007874016f);
    SNDSYS_leavecritical();
    return result;
}

int SndStrmWrapper::SetAz(int Azimuth) {
    int result;
    SNDSYS_entercritical();
    result = SNDSTRM_setazimuth(m_handle, -1, static_cast<float>(Azimuth) * 0.005493248f);
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

    m_vol = vol;
    SNDSYS_entercritical();
    int result = SNDSTRM_autovol(m_handle, time, (m_vol * 0x7F) / 100);
    SNDSYS_leavecritical();
    if (result < 0) {
        return -3;
    }
    return 0;
}

int SndStrmWrapper::SetLowPass(int lowpass) {
    SNDSYS_entercritical();
    int ret = SNDSTRM_lowpass(m_handle, lowpass);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetStatus(SNDSTREAMSTATUS *sss) {
    SNDSYS_entercritical();
    int ret = SNDSTRM_status(m_handle, sss);
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
    int result = GetStatus(&sss);
    if (result >= 0) {
        return sss.timebuffered;
    }
    return 0;
}

int SndStrmWrapper::GetCurrentTime() {
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;
    GetStatus(&sss);
    GetRequestStatus(sss.currentrequest, &srs);
    return srs.currenttime;
}

int SndStrmWrapper::GetTimeRemaining() {
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;
    GetStatus(&sss);
    GetRequestStatus(sss.currentrequest, &srs);
    return srs.timetoend;
}

bool SndStrmWrapper::AlmostDone() {
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;
    GetStatus(&sss);
    if (sss.outstandingrequests == 0) {
        return true;
    }

    if (sss.outstandingrequests == 1) {
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

void SndStrmWrapper::Pause() {
    SNDSYS_entercritical();
    SNDSTRM_pitchmult(m_handle, 0);
    SNDSYS_leavecritical();
}

void SndStrmWrapper::Resume() {
    SNDSYS_entercritical();
    SNDSTRM_pitchmult(m_handle, 0x1000);
    SNDSYS_leavecritical();
}

int SndStrmWrapper::PurgeStream() {
    if (SNDSTRM_purge(m_handle) >= 0) {
        return 0;
    }
    return -3;
}

void SndStrmWrapper::DestroyStream() {
    if (m_handle >= 0) {
        Stop();
        unsigned int time = bGetTicker();
        while (time + 0x14 > bGetTicker()) {
            bSyncTaskRun();
        }
        SNDSTRM_destroy(m_handle);
    }
}
