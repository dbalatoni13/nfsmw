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
extern float windAng;
extern float swayMax;
extern bVector3 windAxis;

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

Rain::Rain(eView *view, RainType StartType) {
    this->CloudIntensity = twkCloudsMinAmount;
    this->RoadDampness = 0.0f;
    this->intensity = 0.0f;
    this->percentPrecip[RAIN] = 0.0f;
    this->percentPrecip[INACTIVE] = 0.0f;
    this->percentPrecip[StartType] = 1.0f;
    this->NumRainPoints = -1;
    this->MyView = view;
    this->NoRain = 0;
    this->NoRainAhead = 0;
    this->PRECIPpoly[0].UVs[0][0] = 0.0f;
    this->PRECIPpoly[0].UVs[0][1] = 1.0f;
    this->PRECIPpoly[0].UVs[0][2] = 0.1f;
    this->PRECIPpoly[0].UVs[0][3] = 1.0f;
    this->PRECIPpoly[0].UVs[1][0] = 0.1f;
    this->PRECIPpoly[0].UVs[1][1] = 0.0f;
    this->PRECIPpoly[0].UVs[1][2] = 0.0f;
    this->PRECIPpoly[0].UVs[1][3] = 0.0f;
    this->PRECIPpoly[0].Colours[0][0] = 0x80;
    this->PRECIPpoly[0].Colours[0][1] = 0x80;
    this->PRECIPpoly[0].Colours[0][2] = 0x80;
    this->PRECIPpoly[0].Colours[0][3] = 0x80;
    this->PRECIPpoly[0].Colours[1][0] = 0x80;
    this->PRECIPpoly[0].Colours[1][1] = 0x80;
    this->PRECIPpoly[0].Colours[1][2] = 0x80;
    this->PRECIPpoly[0].Colours[1][3] = 0x80;
    this->PRECIPpoly[0].Colours[2][0] = 0x80;
    this->PRECIPpoly[0].Colours[2][1] = 0x80;
    this->PRECIPpoly[0].Colours[2][2] = 0x80;
    this->PRECIPpoly[0].Colours[2][3] = 0x80;
    this->PRECIPpoly[0].Colours[3][0] = 0x80;
    this->PRECIPpoly[0].Colours[3][1] = 0x80;
    this->PRECIPpoly[0].Colours[3][2] = 0x80;
    this->PRECIPpoly[0].Colours[3][3] = 0x80;
    this->PRECIPpoly[1].UVs[0][0] = 0.0f;
    this->PRECIPpoly[1].UVs[0][1] = 1.0f;
    this->PRECIPpoly[1].UVs[0][2] = 0.1f;
    this->PRECIPpoly[1].UVs[0][3] = 1.0f;
    this->PRECIPpoly[1].UVs[1][0] = 0.1f;
    this->PRECIPpoly[1].UVs[1][1] = 0.0f;
    this->PRECIPpoly[1].UVs[1][2] = 0.0f;
    this->PRECIPpoly[1].UVs[1][3] = 0.0f;
    this->PRECIPpoly[1].Colours[0][0] = 100;
    this->PRECIPpoly[1].Colours[0][1] = 100;
    this->PRECIPpoly[1].Colours[0][2] = 100;
    this->PRECIPpoly[1].Colours[0][3] = 0x1C;
    this->PRECIPpoly[1].Colours[1][0] = 100;
    this->PRECIPpoly[1].Colours[1][1] = 100;
    this->PRECIPpoly[1].Colours[1][2] = 100;
    this->PRECIPpoly[1].Colours[1][3] = 0x1C;
    this->PRECIPpoly[1].Colours[2][0] = 100;
    this->PRECIPpoly[1].Colours[2][1] = 100;
    this->PRECIPpoly[1].Colours[2][2] = 100;
    this->PRECIPpoly[1].Colours[2][3] = 0x1C;
    this->PRECIPpoly[1].Colours[3][0] = 100;
    this->PRECIPpoly[1].Colours[3][1] = 100;
    this->PRECIPpoly[1].Colours[3][2] = 100;
    this->PRECIPpoly[1].Colours[3][3] = 0x1C;
    this->fogR = 0;
    this->fogG = 0;
    this->fogB = 0;
    this->inTunnel = 0;
    this->inOverpass = 0;
    this->IsValidRainCurtainPos = CT_INACTIVE;
}

void CreateWindRotMatrix(eView *view, bMatrix4 *windrot, int offset, bMatrix4 *l2w) {
    int index = offset;
    bMatrix4 local2world(*l2w);
    float sway = bSin(bDegToAng(windAng + static_cast<float>(index))) * swayMax;

    bIdentity(windrot);
    windAxis = bVector3(1.0f, 0.0f, 0.0f);

    if (view->Precipitation != 0) {
        bNormalize(&windAxis, view->Precipitation->GetWind());
    }

    local2world.v1.x = -local2world.v1.x;
    local2world.v0.y = -local2world.v0.y;
    local2world.v3.x = 0.0f;
    local2world.v3.y = 0.0f;
    local2world.v3.z = 0.0f;
    local2world.v3.w = 1.0f;
    eMulVector(&windAxis, &local2world, &windAxis);
    eCreateAxisRotationMatrix(windrot, windAxis, bDegToAng(sway));
    eRotateZ(windrot, windrot, bDegToAng(sway));
}

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
    if (view->Precipitation == 0) {
        return 0;
    }

    if (CheckOverPass != 0) {
        return view->Precipitation->inTunnel != 0 || view->Precipitation->inOverpass != 0;
    }

    return view->Precipitation->inTunnel;
}

void Rain::AttachRainCurtain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3,
                             float z3) {}

void OnlineManager::InitQuantizers() {}
