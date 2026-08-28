#ifndef CAMERA_CAMERANOISE_H
#define CAMERA_CAMERANOISE_H

#include "Speed/Indep/bWare/Inc/bMath.hpp"

static const bVector4 CameraNoiseHandheldAmplitude(0.01f, 0.01f, 0.03f, 0.03f);
static const bVector4 CameraNoiseHandheldFrequency(0.01f, 0.175f, 0.153f, 0.03f);
static const bVector4 CameraNoiseChopperFrequency(3.141f, 2.971f, 0.84234f, 0.92345f); // size: 0x10, address: 0x8045AB58
static const bVector4 CameraNoiseChopperAmplitude(0.01f, 0.05f, 1.1, 2.7f);            // size: 0x10, address: 0x8045AB68
static const bVector4 CameraNoiseSpeedFrequency;                                       // size: 0x10, address: 0x8045AB78
static const bVector4 CameraNoiseSpeedAmplitude;                                       // size: 0x10, address: 0x8045AB88
static const bVector4 CameraNoiseTerrainFrequency;                                     // size: 0x10, address: 0x8045AB98
static const bVector4 CameraNoiseTerrainAmplitude;                                     // size: 0x10, address: 0x8045ABA8

#endif
