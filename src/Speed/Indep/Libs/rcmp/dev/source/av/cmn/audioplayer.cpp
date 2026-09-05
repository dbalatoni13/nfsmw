#include "rcmp/rcmp.h"
#include "snd/sndo.h"

extern "C" void SNDSYS_entercritical();
extern "C" void SNDSYS_leavecritical();
extern "C" int SNDSTRM_pitchmult(int streamhandle, int pitchmult);
extern "C" int SNDSTRM_modifyhold(int requesthandle, int holdtime);
extern "C" int SNDSTRM_setvol(int streamhandle, int channel, float volume);
extern "C" int SNDSTRM_status(int streamhandle, SNDSTREAMSTATUS *status);
extern "C" int SNDSTRM_requeststatus(int requesthandle, SNDREQUESTSTATUS *status);
extern "C" int SNDSTRM_overheadtap(int maxrequests, int maxchunks);
extern "C" int SNDSTRM_createtap(int audiotap, SNDPLAYOPTS *playopts, int maxrequests,
                                  int maxchunks, void *mem, int memsize);
extern "C" int SNDSTRM_queuerequestid(int streamhandle, int holdtime, int streamrequestid);
extern "C" int SNDSTRM_destroy(int streamhandle);

extern char lbl_80410064[];
extern "C" const float lbl_80410074;
void DEBUG_break();

namespace Snd {

struct System {
    static bool IsInited();
};

} // namespace Snd

namespace RCMP {

struct AUDIO_PLAYER {
    float m_Volume;
    unsigned char *m_audiobuff;
    int m_audiotap;
    int m_sndstreamhandle;
    int m_sndrequesthandle;

    inline static void operator delete(void *ptr) {
        RCMP::rcmp_sys.FreeMem(ptr);
    }

    int SetSpeed(unsigned int speed);
    int StartSound();
    int SetVol(unsigned int volume);
    bool IsAudioFinished();
    inline int GetStreamHandle() {
        return this->m_sndstreamhandle;
    }

    AUDIO_PLAYER(int audiotap, int streamrequestid);
    ~AUDIO_PLAYER();
};

AUDIO_PLAYER::AUDIO_PLAYER(int audiotap, int streamrequestid) {
    SNDPLAYOPTS playopts;
    int overhead;
    const int MAX_SOUND_TAP_REQ = 1;
    const int MAX_SOUND_CHUNKS_OUTSTANDING = 30;
    const int SOUND_HOLD_TIME = -1;

    if (!Snd::System::IsInited()) {
        DEBUG_break();
    } else {
        this->m_audiobuff = 0;
        this->m_sndrequesthandle = -1;
        this->m_sndstreamhandle = -1;
        SNDplaysetdef(&playopts);
        overhead = SNDSTRM_overheadtap(MAX_SOUND_TAP_REQ, MAX_SOUND_CHUNKS_OUTSTANDING);
        this->m_audiobuff = static_cast<unsigned char *>(RCMP::rcmp_sys.AllocMem(
            lbl_80410064, overhead, 0, 0, RCMP::rcmp_sys.m_DefaultMemDir));
        this->m_sndstreamhandle = SNDSTRM_createtap(
            audiotap, &playopts, MAX_SOUND_TAP_REQ, MAX_SOUND_CHUNKS_OUTSTANDING,
            this->m_audiobuff, overhead);
        this->m_sndrequesthandle = SNDSTRM_queuerequestid(
            this->m_sndstreamhandle, SOUND_HOLD_TIME, streamrequestid);
        this->m_Volume = lbl_80410074;
    }
}

AUDIO_PLAYER::~AUDIO_PLAYER() {
    if (this->m_audiobuff != 0) {
        SNDSTRM_destroy(this->m_sndstreamhandle);
        RCMP::rcmp_sys.FreeMem(this->m_audiobuff);
        this->m_audiobuff = 0;
    }
}

int AUDIO_PLAYER::SetSpeed(unsigned int Speed) {
    if (this->m_sndstreamhandle != -1) {
        int snderr;

        SNDSYS_entercritical();
        snderr = SNDSTRM_pitchmult(this->m_sndstreamhandle, Speed);
        SNDSYS_leavecritical();
        return snderr;
    }
    return 0;
}

int AUDIO_PLAYER::StartSound() {
    if (this->m_sndrequesthandle != -1) {
        int snderr;

        SNDSYS_entercritical();
        snderr = SNDSTRM_modifyhold(this->m_sndrequesthandle, 0);
        SNDSYS_leavecritical();
        return snderr;
    }
    return 0;
}

int AUDIO_PLAYER::SetVol(unsigned int Vol) {
    if (this->m_sndstreamhandle != -1) {
        int snderr;

        SNDSYS_entercritical();
        snderr = SNDSTRM_setvol(this->m_sndstreamhandle, -1, static_cast<float>(Vol) / 127.0f);
        SNDSYS_leavecritical();
        return snderr;
    }
    return 0;
}

bool AUDIO_PLAYER::IsAudioFinished() {
    SNDREQUESTSTATUS status;
    SNDSTREAMSTATUS sndstrmsstatus;

    SNDSYS_entercritical();
    if (SNDSTRM_status(this->m_sndstreamhandle, &sndstrmsstatus) < 0) {
        SNDSYS_leavecritical();
        return false;
    }
    (void)this->GetStreamHandle();
    if (SNDSTRM_requeststatus(sndstrmsstatus.currentrequest, &status) < 0) {
        SNDSYS_leavecritical();
        return false;
    }
    SNDSYS_leavecritical();
    return status.timetoend == 0;
}

}
