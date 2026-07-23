#ifndef SFXCTL_3DOBJPOS_HPP
#define SFXCTL_3DOBJPOS_HPP

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

// Decl: 37
enum ePOSMIXTYPE {
    SINGLE_PLAYER = 0,
    TPMIX_AVE_CAM = 1,
    TPMIX_AVE_CAR = 2,
    TPMIX_AVE_CAM_CAR = 3,
};

// total size: 0x64
// Decl: 46
class SFXCTL_3DObjPos : public SFXCTL {
  public:
    DECLARE_CREATABLE();

    SFXCTL_3DObjPos();
    ~SFXCTL_3DObjPos() override;

    // Overrides: SndBase
    void UpdateParams(float t) override;

    virtual void Generate3DParams(int nplayer);
    virtual void AssignPositionVector(bVector3 *pV3ObjPos);
    virtual void AssignDirectionVector(const bVector3 *pV3ObjDir);
    virtual void AssignVelocityVector(const bVector3 *pV3ObjVel);

    // Overrides: SndBase
    void Detach() override;

    void SetPlayerRef(int i);

  private:
    void SetCameraAngle();
    void GenerateSinglePlayerMix();
    void GenerateTwoPlayerMix();
    void GenerateTwoPlayerAzim(eTP3DPosInputs azimtype);

  protected:
    void UpdateDoppler(int PlayerNum, float t);

    bVector3 *m_pV3ObjPos; // offset 0x28, size 0x4, Decl: 78
    bVector3 *m_pV3ObjDir; // offset 0x2C, size 0x4, Decl: 79
    bVector3 *m_pV3ObjVel; // offset 0x30, size 0x4, Decl: 80

    static bVector2 m_v2ObjPosCopy; // size: 0x8, address: 0x8045DD68
    static bVector2 m_v2ObjDirCopy; // size: 0x8, address: 0xFFFFFFFF

    float m_fDistToRef[2][2];            // offset 0x34, size 0x10, Decl: 86
    int m_PlayerRef;                     // offset 0x44, size 0x4, Decl: 97
    bool m_bIsInTwoPlayerTransitionZone; // offset 0x48, size 0x1, Decl: 101
    bool m_bDirectionClockwise;          // offset 0x4C, size 0x1, Decl: 102
    bool m_bIsOtherCamRightSide;         // offset 0x50, size 0x1, Decl: 103

    static bVector2 *m_pv2AzimRefDir;            // size: 0x4, address: 0x80417A94
    static bVector2 *m_pv2AzimRefPos;            // size: 0x4, address: 0x80417A98
    static unsigned short m_CameraAngle;         // size: 0x2, address: 0x80417A9C
    static unsigned short m_PlyrCarAzimToCam[2]; // size: 0x4, address: 0xFFFFFFFF
    static unsigned short m_PlyrCarLockAzim[2];  // size: 0x4, address: 0xFFFFFFFF
    static float m_fDistBetweenPlyrCars;         // size: 0x4, address: 0xFFFFFFFF
    static int m_CurLeadCar;                     // size: 0x4, address: 0xFFFFFFFF
    static int m_OutTempRefCar[16];              // size: 0x40, address: 0xFFFFFFFF
    static int m_OutTempOtherCar[16];            // size: 0x40, address: 0xFFFFFFFF
    static float fratio;                         // size: 0x4, address: 0xFFFFFFFF
    static float fratio_2;                       // size: 0x4, address: 0xFFFFFFFF

    float m_fdvelmag_car[2]; // offset 0x54, size 0x8, Decl: 118
    float m_fdvelmag_cam[2]; // offset 0x5C, size 0x8, Decl: 119
};

#endif
