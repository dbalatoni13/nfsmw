#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

int SndCamera::NumPlayers = 0;
Attrib::StringKey SndCamera::m_CamAction[2] = {Attrib::StringKey("CDActionDrive"), Attrib::StringKey("CDActionDrive")};
Attrib::StringKey SndCamera::m_NewCamAction[2] = {Attrib::StringKey("CDActionDrive"), Attrib::StringKey("CDActionDrive")};
Camera *SndCamera::m_pCams[2] = {nullptr, nullptr};
eCamStates SndCamera::m_CurCamState[2] = {DMIX_DEFAULT_CAM, DMIX_DEFAULT_CAM};
eCamStates SndCamera::m_PrevCamState[2] = {DMIX_DEFAULT_CAM, DMIX_DEFAULT_CAM};
bVector3 SndCamera::m_CurCamPos[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
bVector3 SndCamera::m_CurCamDir[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
bVector3 SndCamera::m_CurCamTarget[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
bVector2 SndCamera::m_NormCamDir[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_AvergeCamDir[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector3 SndCamera::m_v3WorldCarPos[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
bVector3 SndCamera::m_v3WorldCarDir[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
bVector2 SndCamera::m_WorldCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_CenteredCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_AverageCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_WorldCamPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_CenteredCamPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_AveragedCamPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_NormCarDir[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector2 SndCamera::m_NormCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};
bVector3 SndCamera::m_v3WorldCarVel[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
bVector3 SndCamera::m_v3WorldCamVel[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};
float SndCamera::m_WorldCarVel[2] = {0.0f, 0.0f};
EAXCar *SndCamera::m_PLayerCars[2] = {nullptr, nullptr};

void SetNewSndCamAction(Attrib::StringKey camtype, EVIEW_ID viewID) {
    if (viewID == EVIEW_FIRST_PLAYER) {
        SndCamera::m_NewCamAction[0] = camtype;
        return;
    }

    if (viewID == EVIEW_PLAYER2) {
        SndCamera::m_NewCamAction[1] = camtype;
    }
}

void SndCamera::UpdateCameras() {
    int k;

    NumPlayers = 0;
    for (k = 0; k < 2; k++) {
        EAXCar *pEaxCar;

        pEaxCar = g_pEAXSound->GetPlayerTunerCar(k);
        if (pEaxCar) {
            Camera *pcam;
            EAX_CarState *pcar;
            EVIEW_ID vwID;
            bVector2 v2CamDir;
            bVector2 v2CarDir;

            pcar = nullptr;
            pEaxCar = g_pEAXSound->GetPlayerTunerCar(k);
            m_PLayerCars[k] = pEaxCar;
            if (pEaxCar) {
                pcar = pEaxCar->GetPhysCar();
            }

            if (!pcar) {
                continue;
            }

            vwID = EVIEW_PLAYER2;
            NumPlayers++;
            if (k == 0) {
                vwID = EVIEW_FIRST_PLAYER;
            }

            pcam = eViews[vwID].pCamera;
            if (!pcam) {
                continue;
            }

            m_pCams[k] = pcam;
            m_CurCamPos[k] = *pcam->GetPosition();
            m_CurCamDir[k] = *pcam->GetDirection();
            v2CamDir.x = m_CurCamDir[k].x;
            v2CamDir.y = m_CurCamDir[k].y;
            m_CurCamTarget[k] = *pcam->GetTarget();
            bNormalize(m_NormCamDir + k, &v2CamDir);
            m_WorldCamPos[k].x = m_CurCamPos[k].x;
            m_WorldCamPos[k].y = m_CurCamPos[k].y;
            m_AvergeCamDir[k].x = m_NormCamDir[k].x;
            m_AvergeCamDir[k].y = m_NormCamDir[k].y;
            m_AveragedCamPos[k].x = m_CurCamPos[k].x;
            m_AveragedCamPos[k].y = m_WorldCamPos[k].y;
            m_WorldCarPos[k] = *pcar->GetPosition2D();
            m_v3WorldCarPos[k] = *pcar->GetPosition();
            m_CenteredCarPos[k].x = m_WorldCarPos[k].x - m_CurCamPos[k].x;
            m_CenteredCarPos[k].y = m_WorldCarPos[k].y - m_WorldCamPos[k].y;
            m_v3WorldCamVel[k] = *pcam->GetVelocityPosition();
            m_v3WorldCarDir[k] = *pcar->GetForwardVector();
            m_v3WorldCarPos[k] = *pcar->GetPosition();
            m_v3WorldCarVel[k] = *pcar->GetVelocity();
            m_WorldCarVel[k] = bLength(m_v3WorldCarVel + k);
            v2CarDir.x = pcar->GetForwardVector()->x;
            v2CarDir.y = pcar->GetForwardVector()->y;
            bNormalize(m_NormCarDir + k, &v2CarDir);

            m_PrevCamState[k] = m_CurCamState[k];
            if (m_CamAction[k] != m_NewCamAction[k]) {
                m_CamAction[k] = m_NewCamAction[k];
                {
                    Attrib::StringKey drive("CDActionDrive");
                    if (m_CamAction[k] == drive) {
                        int pov;

                        pEaxCar = g_pEAXSound->GetPlayerTunerCar(k);
                        pov = pEaxCar->GetPOV();
                        if (pov == 0) {
                            m_CurCamState[k] = DMIX_NFS_BUMPER_CAM;
                        } else if (pov == 1) {
                            m_CurCamState[k] = DMIX_NFS_INCAR_CAM;
                        } else {
                            m_CurCamState[k] = DMIX_DEFAULT_CAM;
                        }
                    } else {
                        Attrib::StringKey track("CDActionTrackCar");
                        if (m_CamAction[k] == track) {
                            if (pcar->mHealth == 0.0f) {
                                m_CurCamState[k] = DMIX_NFS_COLLISION_CAM;
                            } else {
                                m_CurCamState[k] = DMIX_NFS_JUMP_CAM;
                            }
                        } else {
                            Attrib::StringKey ice("CDActionIce");
                            if (m_CamAction[k] == ice) {
                                m_CurCamState[k] = DMIX_NFS_NIS_CAM;
                            }
                        }
                    }
                }
            }
        }
    }
}

Camera *SndCamera::GetCam(int nPlayer) {
    if (m_pCams[nPlayer]) {
        return m_pCams[nPlayer];
    }

    if (m_pCams[0]) {
        return m_pCams[0];
    }

    return nullptr;
}
