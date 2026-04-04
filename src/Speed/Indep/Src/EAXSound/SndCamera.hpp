#ifndef EAXSOUND_SNDCAMERA_H
#define EAXSOUND_SNDCAMERA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

namespace Attrib {
class StringKey;
}

struct EAXCar;
struct Camera;

enum eCamStates {
    DMIX_DEFAULT_CAM = 0,
    DMIX_REAR_CAM = 0,
    DMIX_NFS_BUMPER_CAM = 1,
    DMIX_NFS_INCAR_CAM = 2,
    DMIX_NFS_JUMP_CAM = 3,
    DMIX_NFS_NIS_CAM = 4,
    DMIX_NFS_COLLISION_CAM = 5,
    MAX_DMIX_CAMS = 6,
};

struct SndCamera {
    static int NumPlayers;
    static Attrib::StringKey m_CamAction[2];
    static Attrib::StringKey m_NewCamAction[2];

  private:
    static Camera *m_pCams[2];
    static eCamStates m_CurCamState[2];
    static eCamStates m_PrevCamState[2];
    static bVector3 m_CurCamPos[2];
    static bVector3 m_CurCamDir[2];
    static bVector3 m_CurCamTarget[2];
    static bVector2 m_NormCamDir[2];
    static bVector2 m_AvergeCamDir[2];
    static bVector3 m_v3WorldCarPos[2];
    static bVector3 m_v3WorldCarDir[2];
    static bVector2 m_WorldCarPos[2];
    static bVector2 m_CenteredCarPos[2];
    static bVector2 m_AverageCarPos[2];
    static bVector2 m_WorldCamPos[2];
    static bVector2 m_CenteredCamPos[2];
    static bVector2 m_AveragedCamPos[2];
    static bVector2 m_NormCarDir[2];
    static bVector2 m_NormCarPos[2];
    static bVector3 m_v3WorldCarVel[2];
    static bVector3 m_v3WorldCamVel[2];
    static float m_WorldCarVel[2];
    static EAXCar *m_PLayerCars[2];

  public:
    static void InitializeCameras();
    static void UpdateCameras();
    static Camera *GetCam(int nPlayer);

    static bVector3 *GetCamPos(int nPlayer);
    static bVector3 *GetCamDir(int nPlayer);
    static bVector3 *GetCamTarget(int nPlayer);
    static bVector2 *GetNormCamDir(int nPlayer);
    static bVector2 *GetNormCarDir(int nPlayer);
    static bVector2 *GetWorldCamPos(int nPlayer);
    static bVector2 *GetAvgCamDir(int nPlayer);
    static bVector2 *GetWorldCarPos(int nPlayer);
    static bVector2 *GetAveragedCamPos(int nPlayer);
    static bVector2 *GetCenteredCarPos(int nPlayer);
    static bVector3 *GetV3WorldCarPos(int nPlayer);
    static bVector3 *GetV3WorldCarVel(int nPlayer);
    static bVector3 *GetWorldCamVel(int nPlayer);
    static eCamStates GetCurCamState(int nPlayer);
    static eCamStates GetPrevCamState(int nPlayer);
    static bVector3 *GetNormCarDir3(int nPlayer);
    static bVector3 *GetWorldCarPos3(int nPlayer);
    static float GetWorldCarVel(int nPlayer);
    static EAXCar *GetPlayerCar(int nPlayer);
    static bVector3 *GetCamPos3(int nPlayer);
};

inline bVector3 *SndCamera::GetCamPos(int nPlayer) { return m_CurCamPos + nPlayer; }

inline bVector3 *SndCamera::GetCamDir(int nPlayer) { return m_CurCamDir + nPlayer; }

inline bVector3 *SndCamera::GetCamTarget(int nPlayer) { return m_CurCamTarget + nPlayer; }

inline bVector2 *SndCamera::GetNormCamDir(int nPlayer) { return m_NormCamDir + nPlayer; }

inline bVector2 *SndCamera::GetNormCarDir(int nPlayer) { return m_NormCarDir + nPlayer; }

inline bVector2 *SndCamera::GetWorldCamPos(int nPlayer) { return m_WorldCamPos + nPlayer; }

inline bVector2 *SndCamera::GetAvgCamDir(int nPlayer) { return m_AvergeCamDir + nPlayer; }

inline bVector2 *SndCamera::GetWorldCarPos(int nPlayer) { return m_WorldCarPos + nPlayer; }

inline bVector2 *SndCamera::GetAveragedCamPos(int nPlayer) { return m_AveragedCamPos + nPlayer; }

inline bVector2 *SndCamera::GetCenteredCarPos(int nPlayer) { return m_CenteredCarPos + nPlayer; }

inline bVector3 *SndCamera::GetV3WorldCarPos(int nPlayer) { return m_v3WorldCarPos + nPlayer; }

inline bVector3 *SndCamera::GetV3WorldCarVel(int nPlayer) { return m_v3WorldCarVel + nPlayer; }

inline bVector3 *SndCamera::GetWorldCamVel(int nPlayer) { return m_v3WorldCamVel + nPlayer; }

inline eCamStates SndCamera::GetCurCamState(int nPlayer) { return m_CurCamState[nPlayer]; }

inline eCamStates SndCamera::GetPrevCamState(int nPlayer) { return m_PrevCamState[nPlayer]; }

inline bVector3 *SndCamera::GetNormCarDir3(int nPlayer) { return m_v3WorldCarDir + nPlayer; }

inline bVector3 *SndCamera::GetWorldCarPos3(int nPlayer) { return m_v3WorldCarPos + nPlayer; }

inline float SndCamera::GetWorldCarVel(int nPlayer) { return m_WorldCarVel[nPlayer]; }

inline EAXCar *SndCamera::GetPlayerCar(int nPlayer) { return m_PLayerCars[nPlayer]; }

inline bVector3 *SndCamera::GetCamPos3(int nPlayer) { return m_CurCamPos + nPlayer; }

#endif
