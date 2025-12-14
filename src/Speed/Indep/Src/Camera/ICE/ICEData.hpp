#ifndef CAMERA_ICE_ICEDATA_H
#define CAMERA_ICE_ICEDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ICEMath.hpp"

// total size: 0x84
struct ICEData {
    void GetEye(int i, ICE::Vector3 *p);

    unsigned char nType;              // offset 0x0, size 0x1
    unsigned char bSmooth;            // offset 0x1, size 0x1
    unsigned char bCubicEye;          // offset 0x2, size 0x1
    unsigned char bCubicLook;         // offset 0x3, size 0x1
    unsigned char nSpaceEye;          // offset 0x4, size 0x1
    unsigned char nSpaceLook;         // offset 0x5, size 0x1
    unsigned char bConstrainToCars;   // offset 0x6, size 0x1
    unsigned char bConstrainToWorld;  // offset 0x7, size 0x1
    unsigned char bIgnoreOrientation; // offset 0x8, size 0x1
    unsigned char bCarSpaceLag;       // offset 0x9, size 0x1
    unsigned char nOverlay;           // offset 0xA, size 0x1
    unsigned int nShakeType;          // offset 0xC, size 0x4
    float fParameter;                 // offset 0x10, size 0x4
    float fTangentLength[2];          // offset 0x14, size 0x8
    float vEye[2][3];                 // offset 0x1C, size 0x18
    float vLook[2][3];                // offset 0x34, size 0x18
    float fDutch[2];                  // offset 0x4C, size 0x8
    float fLens[2];                   // offset 0x54, size 0x8
    float fNearClip[2];               // offset 0x5C, size 0x8
    float fNoiseAmplitude[2];         // offset 0x64, size 0x8
    float fNoiseFrequency[2];         // offset 0x6C, size 0x8
    float fFocalDistance[2];          // offset 0x74, size 0x8
    unsigned char fAperture[2];       // offset 0x7C, size 0x2
    unsigned char fLetterbox[2];      // offset 0x7E, size 0x2
};

#endif
