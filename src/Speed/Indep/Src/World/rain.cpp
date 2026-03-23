#include "Speed/Indep/Src/World/Rain.hpp"

float rainOverrideIntensity;
extern bool EnableRainIn2P;

void TempInits() {}

OnScreenRain::OnScreenRain() {}

void SetOverRideRainIntensity(float rov) {
    rainOverrideIntensity = rov;
}

void SetRainBase() {
    Rain *precipitation = eViews[1].Precipitation;

    *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(precipitation) + 0x2F4) = 0.0f;
    precipitation->Init(INACTIVE, 1.0f);

    if (EnableRainIn2P != 0) {
        precipitation = eViews[2].Precipitation;
        *reinterpret_cast<float *>(reinterpret_cast<unsigned char *>(precipitation) + 0x2F4) = 0.0f;
        precipitation->Init(INACTIVE, 1.0f);
    }
}

int AmIinATunnel(eView *view, int CheckOverPass) {
    Rain *precipitation = view->Precipitation;

    if (precipitation == 0) {
        return 0;
    }

    if (CheckOverPass != 0) {
        if (precipitation->inTunnel != 0) {
            return 1;
        }

        if (precipitation->inOverpass == 0) {
            return 0;
        }

        return 1;
    }

    return precipitation->inTunnel;
}

void Rain::AttachRainCurtain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3,
                             float z3) {}
