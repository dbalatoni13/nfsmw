#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"

#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

SndStrmWrapper::SndStrmWrapper() {
    m_handle = -1;
    m_StreamID = 0;
}

int SndStrmWrapper::Create(const int maxChunks, const int maxRequests, const int buffersize) {
    SNDPLAYOPTS STRMopts;

    SNDplaysetdef(&STRMopts);
    STRMopts.vol = 100;
    STRMopts.fxlevel0 = 0;

    int overhead = SNDSTRM_overhead(maxRequests, maxChunks);
    m_RealStreamBuffer = (int) m_buffer + overhead;
    overhead += buffersize;
    m_BufferSize = overhead;
    m_buffer = gAudioMemoryManager.AllocateMemoryChar(m_BufferSize, "AUD:Stream buffer", false);

    return CreateStream(maxChunks, maxRequests, m_buffer, buffersize, &STRMopts);
}

int SndStrmWrapper::CreateStream(const int maxChunks, const int maxRequests, char *pmem, const int buffersize,
                                 void *pplayopts) {
    m_buffer = pmem;
    m_handle = SNDSTRM_create((SNDPLAYOPTS *) pplayopts, maxRequests, maxChunks, pmem, buffersize);
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

bool SndStrmWrapper::IsPlaying() {
    SNDREQUESTSTATUS srs;
    SNDSTREAMSTATUS sss;

    SNDSYS_entercritical();
    SNDSTRM_status(m_handle, &sss);

    int reqHandle = sss.currentrequest;
    if (reqHandle < 0) {
        SNDSYS_leavecritical();
    } else {
        SNDSTRM_requeststatus(reqHandle, &srs);
        SNDSYS_leavecritical();

        if (srs.state != 3 || sss.outstandingrequests > 0) {
            return true;
        }
    }
    return false;
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
    int itemsinq;
    SNDSTREAMSTATUS sss;
    SNDREQUESTSTATUS srs;

    GetStatus(&sss);
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
        if (srs.state == 0) {
            return false;
        }
        if (srs.state == 1) {
            return false;
        }
        if (srs.timetoend < STRM_ALMOST_DONE_TIME) {
            return true;
        }
    }
    return false;
}

int SndStrmWrapper::Stop() {
    if (SNDSTRM_purge(m_handle) < 0) {
        return STRM_ERR_UNKNOWN;
    }
    return STRM_OK;
}

int SndStrmWrapper::AddToStream(const char *filename, long offset, int holdtime) {
    int ret = SNDSTRM_queuefile(m_handle, holdtime, filename, offset);
    if (ret < 0) {
        return ret;
    }
    return ret;
}

int SndStrmWrapper::AddToStream(int holdtime, void *paddr, int length, int offset) {
    return SNDSTRM_queuemem(m_handle, holdtime, paddr, offset * 128);
}

int SndStrmWrapper::ModifyHold(int sndrequesthandle, int holdtime) {
    return SNDSTRM_modifyhold(sndrequesthandle, holdtime);
}

int SndStrmWrapper::SetVol(int vol, bool bramp) {
    int result;

    if (bramp == true) {
        return RampVol(vol, 250);
    }

    m_vol = vol;
    SNDSYS_entercritical();
    result = SNDSTRM_setvol(m_handle, Snd::CHANNEL_ALL, m_vol / 127.0f);
    SNDSYS_leavecritical();
    return result;
}

int SndStrmWrapper::SetAz(int Azimuth) {
    int result;

    SNDSYS_entercritical();
    result = SNDSTRM_setazimuth(m_handle, -1, Azimuth * (360.0f / 65535.0f));
    SNDSYS_leavecritical();
    return result;
}

int SndStrmWrapper::RampVol(int vol, int time) {
    int result;

    if (vol < 0) {
        vol = 0;
    }
    if (vol > 100) {
        vol = 100;
    }
    if (time < 0) {
        return STRM_ERR_BAD_PARM;
    }

    m_vol = vol;
    SNDSYS_entercritical();
    result = SNDSTRM_autovol(m_handle, time, m_vol * 127 / 100);
    SNDSYS_leavecritical();

    if (result < 0) {
        return STRM_ERR_UNKNOWN;
    }
    return STRM_OK;
}

int SndStrmWrapper::SetLowPass(int lowpass) {
    int ret;

    SNDSYS_entercritical();
    ret = SNDSTRM_lowpass(m_handle, lowpass);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetStatus(SNDSTREAMSTATUS *sss) {
    int ret;

    SNDSYS_entercritical();
    ret = SNDSTRM_status(m_handle, sss);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetRequestStatus(int sndrequesthandle, SNDREQUESTSTATUS *psrs) {
    int ret;

    SNDSYS_entercritical();
    ret = SNDSTRM_requeststatus(sndrequesthandle, psrs);
    SNDSYS_leavecritical();
    return ret;
}

int SndStrmWrapper::GetTimeBuffered() {
    SNDSTREAMSTATUS sss;

    if (GetStatus(&sss) < 0) {
        return 0;
    }
    return sss.timebuffered;
}

void SndStrmWrapper::Pause() {
    SNDSYS_entercritical();
    SNDSTRM_pitchmult(m_handle, 0);
    SNDSYS_leavecritical();
}

void SndStrmWrapper::Resume() {
    SNDSYS_entercritical();
    SNDSTRM_pitchmult(m_handle, 4096);
    SNDSYS_leavecritical();
}

SndStrmWrapper::~SndStrmWrapper() {
    if (m_handle >= 0) {
        DestroyStream();
        if (m_buffer != NULL) {
            gAudioMemoryManager.FreeMemory(m_buffer);
        }
    }
}

void SndStrmWrapper::DestroyStream() {
    if (m_handle >= 0) {
        Stop();

        unsigned int time = bGetTicker() + 20;
        while (time > bGetTicker()) {
            bSyncTaskRun();
        }

        SNDSTRM_destroy(m_handle);
    }
}

int SndStrmWrapper::PurgeStream() {
    if (SNDSTRM_purge(m_handle) < 0) {
        return STRM_ERR_UNKNOWN;
    }
    return STRM_OK;
}
