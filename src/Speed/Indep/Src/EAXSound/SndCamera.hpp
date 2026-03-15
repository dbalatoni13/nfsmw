#ifndef EAXSOUND_SNDCAMERA_H
#define EAXSOUND_SNDCAMERA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct Camera;

namespace SndCamera {
Camera *GetCam(int);

extern bVector2 m_AvergeCamDir[2];
extern bVector2 m_WorldCamPos[2];
extern bVector2 m_NormCamDir[2];
extern bVector2 m_AveragedCamPos[2];
extern bVector2 m_NormCarDir[2];
extern bVector2 m_WorldCarPos[2];
extern bVector2 m_CenteredCarPos[2];
extern bVector3 m_v3WorldCarVel[2];
extern bVector3 m_v3WorldCamVel[2];
extern bVector3 m_v3WorldCarPos[2];
extern bVector3 m_CurCamPos[2];
extern int m_CurCamState[2];

static inline bVector3 *GetV3WorldCarVel(int nPlayer);
static inline bVector3 *GetWorldCamVel(int nPlayer);
static inline bVector3 *GetV3WorldCarPos(int nPlayer);
static inline bVector3 *GetCamPos(int nPlayer);
} // namespace SndCamera

static inline bVector3 *SndCamera::GetV3WorldCarVel(int nPlayer) {
    return m_v3WorldCarVel + nPlayer;
}

static inline bVector3 *SndCamera::GetWorldCamVel(int nPlayer) {
    return m_v3WorldCamVel + nPlayer;
}

static inline bVector3 *SndCamera::GetV3WorldCarPos(int nPlayer) {
    return m_v3WorldCarPos + nPlayer;
}

static inline bVector3 *SndCamera::GetCamPos(int nPlayer) {
    return m_CurCamPos + nPlayer;
}



#endif
