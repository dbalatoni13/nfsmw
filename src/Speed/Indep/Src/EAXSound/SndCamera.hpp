#ifndef SND_CAMERA_HPP
#define SND_CAMERA_HPP

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

// total size: 0x1
// Decl: 21
class SndCamera {
  public:
    static void InitializeCameras();    // Decl: 23
    static Camera *GetCam(int nPlayer); // Decl: 24
    static bVector3 *GetCamPos(int nPlayer) {
        return &m_CurCamPos[nPlayer];
    } // Decl: 25
    static bVector3 *GetCamDir(int nPlayer) {
        return &m_CurCamDir[nPlayer];
    } // Decl: 26
    static bVector3 *GetCamTarget(int nPlayer) {
        return &m_CurCamTarget[nPlayer];
    } // Decl: 27
    static bVector2 *GetNormCamDir(int nPlayer) {
        return &m_NormCamDir[nPlayer];
    } // Decl: 28
    static bVector2 *GetNormCarDir(int nPlayer) {
        return &m_NormCarDir[nPlayer];
    } // Decl: 29
    static bVector2 *GetWorldCamPos(int nPlayer) {
        return &m_WorldCamPos[nPlayer];
    } // Decl: 30
    // Decl: 31
    static bVector2 *GetAvgCamDir(int nPlayer) {
        return &m_AvergeCamDir[nPlayer];
    }
    static bVector2 *GetWorldCarPos(int nPlayer) {
        return &m_WorldCarPos[nPlayer];
    } // Decl: 32
    static bVector2 *GetAveragedCamPos(int nPlayer) {
        return &m_AveragedCamPos[nPlayer];
    } // Decl: 33
    static bVector2 *GetCenteredCarPos(int nPlayer) {
        return &m_CenteredCarPos[nPlayer];
    } // Decl: 34

    static bVector3 *GetV3WorldCarPos(int nPlayer) {
        return &m_v3WorldCarPos[nPlayer];
    } // Decl: 36

    static bVector3 *GetV3WorldCarVel(int nPlayer) {
        return &m_v3WorldCarVel[nPlayer];
    } // Decl: 38
    // Decl: 39
    static bVector3 *GetWorldCamVel(int nPlayer) {
        return &m_v3WorldCamVel[nPlayer];
    }
    static eCamStates GetCurCamState(int nPlayer) {
        return m_CurCamState[nPlayer];
    } // Decl: 40
    static eCamStates GetPrevCamState(int nPlayer) {
        return m_PrevCamState[nPlayer];
    } // Decl: 41

    // Decl: 44
    static bVector3 *GetNormCarDir3(int nPlayer) {
        return &m_v3WorldCarDir[nPlayer];
    }
    static bVector3 *GetWorldCarPos3(int nPlayer) {
        return &m_v3WorldCarPos[nPlayer];
    } // Decl: 45
    static float GetWorldCarVel(int nPlayer) {
        return m_WorldCarVel[nPlayer];
    } // Decl: 46
    static EAXCar *GetPlayerCar(int nPlayer) {
        return m_PLayerCars[nPlayer];
    } // Decl: 47
    static bVector3 *GetCamPos3(int nPlayer) {
        return &m_CurCamPos[nPlayer];
    } // Decl: 48

    static void UpdateCameras(); // Decl: 52

    static int NumPlayers;                      // size: 0x4, address: 0x80417E68, Decl: 54
    static Attrib::StringKey m_CamAction[2];    // size: 0x20, address: 0x8045E070, Decl: 55
    static Attrib::StringKey m_NewCamAction[2]; // size: 0x20, address: 0x8045E090, Decl: 56

  private:
    static Camera *m_pCams[2];           // size: 0x8, address: 0x80417E4C, Decl: 59
    static eCamStates m_CurCamState[2];  // size: 0x8, address: 0x80417E54, Decl: 60
    static eCamStates m_PrevCamState[2]; // size: 0x8, address: 0x80417E5C, Decl: 61
    static bVector3 m_CurCamPos[2];      // size: 0x20, address: 0x8045DF00, Decl: 62
    static bVector3 m_CurCamDir[2];      // size: 0x20, address: 0x8045DF20, Decl: 63
    static bVector3 m_CurCamTarget[2];   // size: 0x20, address: 0x8045DF40, Decl: 64
    static bVector2 m_NormCamDir[2];     // size: 0x10, address: 0x8045DF60, Decl: 65
    static bVector2 m_AvergeCamDir[2];   // size: 0x10, address: 0x8045DF90, Decl: 66
    static bVector3 m_v3WorldCarPos[2];  // size: 0x20, address: 0x8045E030, Decl: 67
    static bVector3 m_v3WorldCarDir[2];  // size: 0x20, address: 0x8045E050, Decl: 68
    static bVector2 m_WorldCarPos[2];    // size: 0x10, address: 0x8045DF80, Decl: 69
    static bVector2 m_CenteredCarPos[2]; // size: 0x10, address: 0x8045DFC0, Decl: 70
    static bVector2 m_AverageCarPos[2];  // size: 0x10, address: 0x8045DFD0, Decl: 71
    static bVector2 m_WorldCamPos[2];    // size: 0x10, address: 0x8045DF70, Decl: 72
    static bVector2 m_CenteredCamPos[2]; // size: 0x10, address: 0x8045DFB0, Decl: 73
    static bVector2 m_AveragedCamPos[2]; // size: 0x10, address: 0x8045DFA0, Decl: 74
    static bVector2 m_NormCarDir[2];     // size: 0x10, address: 0x8045DFE0, Decl: 75
    static bVector2 m_NormCarPos[2];     // size: 0x10, address: 0xFFFFFFFF, Decl: 76
    static bVector3 m_v3WorldCarVel[2];  // size: 0x20, address: 0x8045DFF0, Decl: 79
    static bVector3 m_v3WorldCamVel[2];  // size: 0x20, address: 0x8045E010, Decl: 80
    static float m_WorldCarVel[2];       // size: 0x8, address: 0x80417E6C, Decl: 81
    static EAXCar *m_PLayerCars[2];      // size: 0x8, address: 0x80417E74, Decl: 83
};

#endif
