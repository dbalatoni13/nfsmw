#include "Speed/Indep/Src/EAXSound/Stream/SndStrmWrapper.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"

SndStrmWrapper::SndStrmWrapper() {
    m_handle = -1;
    m_StreamID = 0;
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
    int result = SNDSTRM_purge(m_handle);
    int ret = -3;
    if (result >= 0) {
        ret = 0;
    }
    return ret;
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
    int result = SNDSTRM_modifyhold(sndrequesthandle, holdtime);
    return result;
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
    int result = SNDSTRM_purge(m_handle);
    int ret = -3;
    if (result >= 0) {
        ret = 0;
    }
    return ret;
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
