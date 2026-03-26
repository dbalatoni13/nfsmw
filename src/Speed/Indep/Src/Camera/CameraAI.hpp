#ifndef CAMERA_CAMERAAI_H
#define CAMERA_CAMERAAI_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class IBody;

namespace CameraAI {

void Update(float dT);
void Reset();
void MaybeDoTotaledCam(IPlayer *iplayer);
void MaybeDoPursuitCam(IVehicle *ivehicle);
void AddAvoidable(IBody *body);
void RemoveAvoidable(IBody *body);

}; // namespace CameraAI

#endif
