#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

Camera *SndCamera::m_pCams[2] = {};                                                                // size: 0x8, address: 0x80417E4C, Decl: 13
eCamStates SndCamera::m_CurCamState[2] = {};                                                       // size: 0x8, Decl: 14
eCamStates SndCamera::m_PrevCamState[2] = {};                                                      // size: 0x8, Decl: 15
bVector3 SndCamera::m_CurCamPos[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};     // size: 0x20, Decl: 16
bVector3 SndCamera::m_CurCamDir[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};     // size: 0x20, Decl: 17
bVector3 SndCamera::m_CurCamTarget[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)};  // size: 0x20, address: 0x8045DF40, Decl: 18
bVector2 SndCamera::m_NormCamDir[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};                // size: 0x10, address: 0x8045DF60
bVector2 SndCamera::m_WorldCamPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};               // size: 0x10, address: 0x8045DF70, Decl: 20
bVector2 SndCamera::m_WorldCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};               // size: 0x10, address: 0x8045DF80, Decl: 21
bVector2 SndCamera::m_AvergeCamDir[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};              // size: 0x10, address: 0x8045DF90, Decl: 22
bVector2 SndCamera::m_AveragedCamPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};            // size: 0x10, address: 0x8045DFA0, Decl: 23
bVector2 SndCamera::m_CenteredCamPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};            // size: 0x10, address: 0x8045DFB0, Decl: 24
bVector2 SndCamera::m_CenteredCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};            // size: 0x10, address: 0x8045DFC0, Decl: 25
bVector2 SndCamera::m_AverageCarPos[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};             // size: 0x10, address: 0x8045DFD0, Decl: 26
bVector2 SndCamera::m_NormCarDir[2] = {bVector2(0.0f, 0.0f), bVector2(0.0f, 0.0f)};                // size: 0x10, address: 0x8045DFE0, Decl: 27
bVector3 SndCamera::m_v3WorldCarVel[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)}; // size: 0x20, address: 0x8045DFF0, Decl: 28
bVector3 SndCamera::m_v3WorldCamVel[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)}; // size: 0x20, address: 0x8045E010, Decl: 29
bVector3 SndCamera::m_v3WorldCarPos[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)}; // size: 0x20, address: 0x8045E030, Decl: 30
bVector3 SndCamera::m_v3WorldCarDir[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(0.0f, 0.0f, 0.0f)}; // size: 0x20, address: 0x8045E050, Decl: 31

// size: 0x20, address: 0x8045E070, Decl: 32
Attrib::StringKey SndCamera::m_CamAction[2] = {Attrib::StringKey("CDActionDrive"), Attrib::StringKey("CDActionDrive")};
// size: 0x20, address: 0x8045E090, Decl: 33
Attrib::StringKey SndCamera::m_NewCamAction[2] = {Attrib::StringKey("CDActionDrive"), Attrib::StringKey("CDActionDrive")};

int SND_PRINT_CAMERA_STATE = 0;          // size: 0x4, address: 0xFFFFFFFF, Decl: 35
int SndCamera::NumPlayers = 1;           // size: 0x4, address: 0x80417E68, Decl: 36
float SndCamera::m_WorldCarVel[2] = {};  // size: 0x8, address: 0x80417E6C, Decl: 37
EAXCar *SndCamera::m_PLayerCars[2] = {}; // size: 0x8, address: 0x80417E74, Decl: 38

float TWO_PLYR_XFADE_AZIM_MAX_DIST = 20.0f; // size: 0x4, address: 0xFFFFFFFF, Decl: 42
float TWO_PLYR_XFADE_AZIM_MIN_DIST = 5.0f;  // size: 0x4, address: 0xFFFFFFFF, Decl: 43

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
    NumPlayers = 0;
    for (int k = 0; k < 2; k++) {
        if (g_pEAXSound->GetPlayerTunerCar(k) != nullptr) {
            EAX_CarState *pcar = nullptr;

            m_PLayerCars[k] = g_pEAXSound->GetPlayerTunerCar(k);
            if (m_PLayerCars[k] != nullptr) {
                pcar = m_PLayerCars[k]->GetPhysCar();
            }

            if (pcar != nullptr) {
                NumPlayers++;

                EVIEW_ID vwID = k == 0 ? EVIEW_FIRST_PLAYER : EVIEW_PLAYER2;
                Camera *pcam = eGetView(vwID, false)->GetCamera();
                if (pcam != nullptr) {
                    m_pCams[k] = pcam;

                    bCopy(m_CurCamPos + k, pcam->GetPosition());
                    bCopy(m_CurCamDir + k, pcam->GetDirection());
                    bCopy(m_CurCamTarget + k, pcam->GetTarget());

                    bVector2 v2Temp;
                    v2Temp.x = m_CurCamDir[k].x;
                    v2Temp.y = m_CurCamDir[k].y;
                    bNormalize(m_NormCamDir + k, &v2Temp);

                    m_WorldCamPos[k].x = m_CurCamPos[k].x;
                    m_WorldCamPos[k].y = m_CurCamPos[k].y;

                    bCopy(m_AvergeCamDir + k, m_NormCamDir + k);
                    bCopy(m_AveragedCamPos + k, m_WorldCamPos + k);

                    bCopy(m_WorldCarPos + k, pcar->GetPosition2D());
                    bCopy(m_v3WorldCarPos + k, pcar->GetPosition());

                    bSub(m_CenteredCarPos + k, m_WorldCarPos + k, m_WorldCamPos + k);

                    m_v3WorldCamVel[k] = *pcam->GetVelocityPosition();
                    m_v3WorldCarDir[k] = *pcar->GetForwardVector();
                    m_v3WorldCarPos[k] = *pcar->GetPosition();
                    m_v3WorldCarVel[k] = *pcar->GetVelocity();
                    m_WorldCarVel[k] = pcar->GetVelocityMagnitude();

                    v2Temp.x = pcar->GetForwardVector()->x;
                    v2Temp.y = pcar->GetForwardVector()->y;
                    bNormalize(m_NormCarDir + k, &v2Temp);

                    m_PrevCamState[k] = m_CurCamState[k];

                    if (m_CamAction[k] != m_NewCamAction[k]) {
                        m_CamAction[k] = m_NewCamAction[k];

                        if (m_CamAction[k] == Attrib::StringKey("CDActionDrive")) {
                            int pov = g_pEAXSound->GetPlayerTunerCar(k)->GetPOV();

                            switch (pov) {
                                case 0:
                                    m_CurCamState[k] = DMIX_NFS_BUMPER_CAM;
                                    break;
                                case 1:
                                    m_CurCamState[k] = DMIX_NFS_INCAR_CAM;
                                    break;
                                default:
                                    m_CurCamState[k] = DMIX_DEFAULT_CAM;
                                    break;
                            }
                        } else if (m_CamAction[k] == Attrib::StringKey("CDActionTrackCar")) {
                            if (pcar->GetHeath() == 0.0f) {
                                m_CurCamState[k] = DMIX_NFS_COLLISION_CAM;
                            } else {
                                m_CurCamState[k] = DMIX_NFS_JUMP_CAM;
                            }
                        } else if (m_CamAction[k] == Attrib::StringKey("CDActionIce")) {
                            m_CurCamState[k] = DMIX_NFS_NIS_CAM;
                        }
                    }
                }
            }
        }
    }
}

Camera *SndCamera::GetCam(int nPlayer) {
    if (m_pCams[nPlayer] != nullptr) {
        return m_pCams[nPlayer];
    }

    if (m_pCams[0] != nullptr) {
        return m_pCams[0];
    }

    return nullptr;
}
