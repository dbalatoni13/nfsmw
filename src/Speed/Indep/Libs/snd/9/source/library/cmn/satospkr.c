#include "./sndcmn.h"

// total size: 0x8
typedef struct SNDGAINPAIR {
    float gain1; // offset 0x0, size 0x4
    float gain2; // offset 0x4, size 0x4
} SNDGAINPAIR;

namespace Snd {

extern float gSpeakerPositions[5];
extern unsigned char gTotalOutputChannels;

}; // namespace Snd

// SNDI_sin.c / SNDI_cos.c
float SNDI_sin(float x);
float SNDI_cos(float x);

unsigned char *sndaztospkr_buf;
unsigned char *sndaztospkr[6];

static inline float Az65536To360(unsigned short azimuth) {
    return (float)azimuth * (360.0f / 65536.0f);
}

void SNDI_equalpower(SNDGAINPAIR *pair, float theta1, float theta2, float azimuth) {
    float tm;

    tm = (azimuth * 0.017453294f - theta2 * 0.017453294f) /
         (theta1 * 0.017453294f - theta2 * 0.017453294f) * 1.5707964f;

    pair->gain1 = SNDI_sin(tm);
    pair->gain2 = SNDI_cos(tm);
}

void SNDI_precalcaztospkrvol() {
    int az;
    int azshift;
    SNDGAINPAIR spkrpair;
    int i;

    SNDSYS_entercritical();
    sndaztospkr_buf = (unsigned char *)SNDMEMI_allocz(sndgs.sso.set.outputchannels * 256);
    SNDSYS_leavecritical();

    for (i = 0; i < sndgs.sso.set.outputchannels; i++) {
        sndaztospkr[i] = &sndaztospkr_buf[i * 256];
    }

    for (az = 0; az < 256; az++) {
        float az360;

        azshift = az << 8;
        az360 = Az65536To360(azshift);

        if (sndgs.sso.set.outputchannels == 1) {
            sndaztospkr[0][az] = 127;
            continue;
        }

        for (i = 0; i < sndgs.sso.set.outputchannels; i++) {
            sndaztospkr[i][az] = 0;
        }

        if (az360 <= Snd::gSpeakerPositions[0]) {
            if (sndgs.sso.set.outputchannels > 4) {
                SNDI_equalpower(&spkrpair, Snd::gSpeakerPositions[0], Snd::gSpeakerPositions[1], az360);
                sndaztospkr[0][az] = SNDI_ftoiround(spkrpair.gain1 * 255.0f);
                sndaztospkr[1][az] = SNDI_ftoiround(spkrpair.gain2 * 255.0f);
            } else {
                SNDI_equalpower(&spkrpair, Snd::gSpeakerPositions[0],
                                Snd::gSpeakerPositions[sndgs.sso.set.outputchannels - 1] - 360.0f, az360);
                sndaztospkr[0][az] = SNDI_ftoiround(spkrpair.gain1 * 255.0f);
                sndaztospkr[sndgs.sso.set.outputchannels - 1][az] = SNDI_ftoiround(spkrpair.gain2 * 255.0f);
            }
        } else if (az360 >= Snd::gSpeakerPositions[sndgs.sso.set.outputchannels - 1]) {
            SNDI_equalpower(&spkrpair, Snd::gSpeakerPositions[0],
                            Snd::gSpeakerPositions[sndgs.sso.set.outputchannels - 1] - 360.0f, az360 - 360.0f);
            sndaztospkr[0][az] = SNDI_ftoiround(spkrpair.gain1 * 255.0f);
            sndaztospkr[sndgs.sso.set.outputchannels - 1][az] = SNDI_ftoiround(spkrpair.gain2 * 255.0f);
        } else {
            for (i = 0; i < sndgs.sso.set.outputchannels; i++) {
                if (az360 >= Snd::gSpeakerPositions[i] && az360 <= Snd::gSpeakerPositions[i + 1]) {
                    SNDI_equalpower(&spkrpair, Snd::gSpeakerPositions[i], Snd::gSpeakerPositions[i + 1], az360);
                    sndaztospkr[i][az] = SNDI_ftoiround(spkrpair.gain1 * 255.0f);
                    sndaztospkr[i + 1][az] = SNDI_ftoiround(spkrpair.gain2 * 255.0f);
                }
            }
        }
    }
}

void SNDI_aztospkrvol(int azimuth, float *balance) {
    int azshift;
    int i;

    azshift = (unsigned short)azimuth >> 8;

    for (i = 0; i < sndgs.sso.set.outputchannels; i++) {
        balance[i] = (float)sndaztospkr[i][azshift] * (1.0f / 255.0f);
    }

    if (Snd::gFoldDownTarget == Snd::FOLDDOWNTARGET_5POINT1) {
        balance[5] = 0.0f;
    }
}

void SNDI_freespkrtable() {
    SNDSYS_entercritical();

    if (sndaztospkr) {
        SNDMEMI_free(sndaztospkr_buf);
    }

    SNDSYS_leavecritical();
}

void SNDI_spkrconfig() {
    if (Snd::gTotalOutputChannels == 1) {
        Snd::gFoldDownTarget = Snd::FOLDDOWNTARGET_MONO;
        Snd::gSpeakerPositions[0] = 0.0f;
    } else if (Snd::gTotalOutputChannels == 2) {
        Snd::gFoldDownTarget = Snd::FOLDDOWNTARGET_STEREO;
        Snd::gSpeakerPositions[0] = 90.0f;
        Snd::gSpeakerPositions[1] = 270.0f;
    } else if (Snd::gTotalOutputChannels == 4) {
        Snd::gFoldDownTarget = Snd::FOLDDOWNTARGET_QUAD;
        Snd::gSpeakerPositions[0] = 45.0f;
        Snd::gSpeakerPositions[1] = 135.0f;
        Snd::gSpeakerPositions[2] = 225.0f;
        Snd::gSpeakerPositions[3] = 315.0f;
    } else if (Snd::gTotalOutputChannels == 6) {
        Snd::gFoldDownTarget = Snd::FOLDDOWNTARGET_5POINT1;
        Snd::gSpeakerPositions[0] = 0.0f;
        Snd::gSpeakerPositions[1] = 45.0f;
        Snd::gSpeakerPositions[2] = 135.0f;
        Snd::gSpeakerPositions[3] = 225.0f;
        Snd::gSpeakerPositions[4] = 315.0f;
    }

    if (sndgs.sso.set.outputchannels == 2) {
        sndgs.srcchancfg3d[1][0] = -0x4000;
        sndgs.srcchancfg3d[1][1] = 0x4000;
    } else {
        sndgs.srcchancfg3d[1][0] = -0x2000;
        sndgs.srcchancfg3d[1][1] = 0x2000;
    }

    sndgs.srcchancfg3d[2][0] = sndgs.sso.set.virtualspkrcfg3d[2][2];
    sndgs.srcchancfg3d[2][1] = sndgs.sso.set.virtualspkrcfg3d[2][0];
    sndgs.srcchancfg3d[2][2] = sndgs.sso.set.virtualspkrcfg3d[2][1];

    sndgs.srcchancfg3d[3][0] = -0x2000;
    sndgs.srcchancfg3d[3][1] = 0x2000;
    sndgs.srcchancfg3d[3][2] = -0x6000;
    sndgs.srcchancfg3d[3][3] = 0x6000;

    sndgs.srcchancfg3d[4][0] = -0x2000;
    sndgs.srcchancfg3d[4][1] = 0;
    sndgs.srcchancfg3d[4][2] = 0x2000;
    sndgs.srcchancfg3d[4][3] = -0x6000;
    sndgs.srcchancfg3d[4][4] = 0x6000;

    sndgs.srcchancfg3d[5][0] = -0x2000;
    sndgs.srcchancfg3d[5][1] = 0;
    sndgs.srcchancfg3d[5][2] = 0x2000;
    sndgs.srcchancfg3d[5][3] = -0x6000;
    sndgs.srcchancfg3d[5][4] = 0x6000;
    sndgs.srcchancfg3d[5][5] = 0;
}
