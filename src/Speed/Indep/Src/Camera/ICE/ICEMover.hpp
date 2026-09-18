#ifndef CAMERA_ICE_ICEMOVER_H
#define CAMERA_ICE_ICEMOVER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnchor.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEData.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEPoint.hpp"

namespace ICE {

const char *GetOverlayName(unsigned char n);
void HideOverlay();
void ShowOverlay(unsigned char n);

}; // namespace ICE

// total size: 0x12C
class ICEMover : public CameraMover {
  public:
    bool IsViolatingTopology() { return bViolatesTopology; }

    bool IsSmooth() { return pICEData ? pICEData->bSmooth : false; }

    ICEAnchor *GetICEAnchor() { return pCar; }

    ICEMover(int nView, ICEAnchor *pCar);
    ~ICEMover() override;

    void EyeCubicInit(ICE::Cubic3D *pEye, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity);
    void LookCubicInit(ICE::Cubic3D *pLook, ICE::Matrix4 *pMatrix, ICE::Vector3 *pVelocity);
    void DutchCubicInit(ICE::Cubic1D *pDutch);
    void FovCubicInit(ICE::Cubic1D *pFov);
    void SetDesired(bool b_snap, bool b_refresh);
    void GetEye(ICE::Vector3 *vEye, float f_param);
    void GetLook(ICE::Vector3 *vLook, float f_param);
    float GetDutch(float f_param);
    unsigned short GetFOV(float f_param);

    float GetNearClip(float f_param) {
        float v0 = pNearClip->GetVal();
        float v1 = pNearClip->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    float GetNoiseAmplitude(float f_param) {
        float v0 = pNoiseAmplitude->GetVal();
        float v1 = pNoiseAmplitude->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    float GetNoiseFrequency(float f_param) {
        float v0 = pNoiseFrequency->GetVal();
        float v1 = pNoiseFrequency->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    float GetFocalDistance(float f_param) {
        float v0 = pFocalDistance->GetVal();
        float v1 = pFocalDistance->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    float GetAperture(float f_param) {
        float v0 = pAperture->GetVal();
        float v1 = pAperture->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    float GetLetterbox(float f_param) {
        float v0 = pLetterbox->GetVal();
        float v1 = pLetterbox->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    float GetSimSpeed(float f_param) {
        float v0 = pSimSpeed->GetVal();
        float v1 = pSimSpeed->GetValDesired();

        return v0 * (1.0f - f_param) + v1 * f_param;
    }

    void Update(float dT) override;

  protected:
    ICEAnchor *pCar;               // offset 0x80, size 0x4
    ICE::Cubic3D *pEye;            // offset 0x84, size 0x4
    ICE::Cubic3D *pLook;           // offset 0x88, size 0x4
    ICE::Cubic1D *pDutch;          // offset 0x8C, size 0x4
    ICE::Cubic1D *pFov;            // offset 0x90, size 0x4
    ICE::Cubic1D *pNearClip;       // offset 0x94, size 0x4
    ICE::Cubic1D *pNoiseAmplitude; // offset 0x98, size 0x4
    ICE::Cubic1D *pNoiseFrequency; // offset 0x9C, size 0x4
    ICE::Cubic1D *pFocalDistance;  // offset 0xA0, size 0x4
    ICE::Cubic1D *pAperture;       // offset 0xA4, size 0x4
    ICE::Cubic1D *pLetterbox;      // offset 0xA8, size 0x4
    ICE::Cubic1D *pSimSpeed;       // offset 0xAC, size 0x4
    ICE::Cubic3D *pAccelOffset;    // offset 0xB0, size 0x4
    float fParameter0;             // offset 0xB4, size 0x4
    float fParameter1;             // offset 0xB8, size 0x4
    int nSpaceEye;                 // offset 0xBC, size 0x4
    int nSpaceLook;                // offset 0xC0, size 0x4
    ICEData *pICEData;             // offset 0xC4, size 0x4
    bool bViolatesTopology;        // offset 0xC8, size 0x1
    ICE::Matrix4 mHybridToWorld;   // offset 0xCC, size 0x40
    ICE::Vector3 vSmoothCarPos;    // offset 0x10C, size 0x10
    ICE::Vector3 vSmoothCarFwd;    // offset 0x11C, size 0x10
};

#endif
