#ifndef CAMERA_CAMERAAI_H
#define CAMERA_CAMERAAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

namespace CameraAI {

void Update(float dT);
void Reset();
void MaybeDoTotaledCam(IPlayer *iplayer);
void MaybeDoPursuitCam(IVehicle *ivehicle);

void StartCinematicSlowdown(EVIEW_ID viewID, float seconds);
}; // namespace CameraAI

#endif
