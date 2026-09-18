#include "snd/sndo.h"


void DEBUG_break();


namespace RCMP {

class RCMP_SYSTEM {
  public:
    void *(*AllocMemFunc)(const char *, int, int, int, int);
    void (*FreeMemFunc)(void *);
    int m_DefaultMemDir;

    void *AllocMem(const char *name, unsigned int size, int alignment, int headersize, int type) {
        return AllocMemFunc(name, size, alignment, headersize, type);
    }
    void FreeMem(void *memadr) {
        FreeMemFunc(memadr);
    }
};

extern RCMP_SYSTEM rcmp_sys;

class AUDIO_PLAYER {
  public:
    float mSpeed;
    void *mMemBlock;
    int mPad;
    int mTap;
    int mRequestId;

    static void operator delete(void *p) {
        rcmp_sys.FreeMem(p);
    }

    AUDIO_PLAYER(int channel, int param);
    ~AUDIO_PLAYER();
    int SetSpeed(unsigned int speed);
    int StartSound();
    int SetVol(unsigned int vol);
    int IsAudioFinished();
};

AUDIO_PLAYER::AUDIO_PLAYER(int channel, int param) {
    SNDPLAYOPTS playdef;

    if (!Snd::System::IsInited()) {
        DEBUG_break();
        return;
    }

    mMemBlock = 0;
    mRequestId = -1;
    mTap = -1;

    SNDplaysetdef(&playdef);
    int overhead = SNDSTRM_overheadtap(1, 30);
    mMemBlock = rcmp_sys.AllocMem("AV::audiobuff", overhead, 0, 0, rcmp_sys.m_DefaultMemDir);
    mTap = SNDSTRM_createtap(channel, &playdef, 1, 30, mMemBlock, overhead);
    mRequestId = SNDSTRM_queuerequestid(mTap, -1, param);
    mSpeed = 1.0f;
}

AUDIO_PLAYER::~AUDIO_PLAYER() {
    if (mMemBlock != 0) {
        SNDSTRM_destroy(mTap);
        rcmp_sys.FreeMem(mMemBlock);
        mMemBlock = 0;
    }
}

int AUDIO_PLAYER::SetSpeed(unsigned int speed) {
    int result;

    if (mTap != -1) {
        SNDSYS_entercritical();
        result = SNDSTRM_pitchmult(mTap, speed);
        SNDSYS_leavecritical();
        return result;
    }
    return 0;
}

int AUDIO_PLAYER::StartSound() {
    int result;

    if (mRequestId != -1) {
        SNDSYS_entercritical();
        result = SNDSTRM_modifyhold(mRequestId, 0);
        SNDSYS_leavecritical();
        return result;
    }
    return 0;
}

int AUDIO_PLAYER::SetVol(unsigned int vol) {
    int result;

    if (mTap != -1) {
        SNDSYS_entercritical();
        result = SNDSTRM_setvol(mTap, Snd::CHANNEL_ALL, (float)vol * (1.0f / 127.0f));
        SNDSYS_leavecritical();
        return result;
    }
    return 0;
}

int AUDIO_PLAYER::IsAudioFinished() {
    SNDREQUESTSTATUS reqstatus;
    SNDSTREAMSTATUS status;

    SNDSYS_entercritical();
    if (SNDSTRM_status(mTap, &status) < 0) {
        SNDSYS_leavecritical();
        return 0;
    }
    if (SNDSTRM_requeststatus(status.currentrequest, &reqstatus) < 0) {
        SNDSYS_leavecritical();
        return 0;
    }
    SNDSYS_leavecritical();
    return reqstatus.timetoend == 0;
}

} // namespace RCMP
