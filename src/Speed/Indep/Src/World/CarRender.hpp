#ifndef CARRENDER_H
#define CARRENDER_H

#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum CarRenderUsage {
    CarRenderUsage_Player = 0,
    CarRenderUsage_RemotePlayer = 1,
    CarRenderUsage_AIRacer = 2,
    CarRenderUsage_AICop = 3,
    CarRenderUsage_AITraffic = 4,
    CarRenderUsage_AIHeli = 5,
    carRenderUsage_NISCar = 6,
    CarRenderUsage_Invalid = 7,
};

enum CarRenderDoorID {
    CRD_LeftFront = 0,
    CRD_RightFront = 1,
    CRD_RightRear = 2,
    CRD_LeftRear = 3,
    CRD_Max = 4,
};

void InitCarRender();
void InitCarEffects();
void CloseCarEffects();
void InitStandardModels();

extern bMatrix4 CarScaleMatrix;

#endif
