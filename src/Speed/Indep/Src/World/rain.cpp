#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern float RAINRadiusX;
extern float RAINRadiusY;
extern float RAINRadiusZ;
extern float RAINwindEffect;
extern float RAINX;
extern float RAINY;
extern float RAINZ;
extern float RAINZconstant;
extern float twkCloudsMinAmount;

float rainOverrideIntensity;
extern bool EnableRainIn2P;

void TempInits() {}

OnScreenRain::OnScreenRain() {}

namespace Attrib {
namespace Gen {

const tires &tires::operator=(const Instance &rhs) {
    GetBase() = rhs;
    return *this;
}

} // namespace Gen
} // namespace Attrib

void Rain::Init(RainType type, float percent) {
    int j;

    TempInits();

    this->texture_info[RAIN] = GetTextureInfo(bStringHash("RAINDROP"), 0, 0);
    this->NumRainPoints = 350;

    for (j = 0; j < 2; j++) {
        this->NumOfType[j] = 0;
        this->DesiredNumOfType[j] = 0;
    }

    this->NewSwapBuffer = 0;
    this->OldSwapBuffer = 1;
    this->NumOfType[type] = this->NumRainPoints;
    this->CloudIntensity = twkCloudsMinAmount;
    this->precipWindEffect[RAIN][1] = RAINwindEffect * 0.5f;
    this->precipWindEffect[INACTIVE][1] = 1.0f;
    this->precipRadius[RAIN] = bVector3(RAINRadiusX, RAINRadiusY, RAINRadiusZ);
    this->precipWindEffect[INACTIVE][0] = 1.0f;
    this->precipWindEffect[RAIN][0] = RAINwindEffect;
    this->precipSpeedRange[RAIN] = bVector3(RAINX, RAINY, RAINZ);
    this->precipZconstant[RAIN] = RAINZconstant;
    this->windType[RAIN] = VECTOR_WIND;
    this->windSpeed = bVector3(0.0f, 0.0f, 0.0f);
    this->windTime = 0.0f;
}

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

void OnlineManager::InitQuantizers() {}
