#ifndef CAMERA_ICE_ICEDATA_H
#define CAMERA_ICE_ICEDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "ICEMath.hpp"

// total size: 0x84
struct ICEData {
    void PlatEndianSwap();
    void GetEye(int n, ICE::Vector3 *p);
    void GetLook(int n, ICE::Vector3 *p);

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
    unsigned char fSimSpeed[2];       // offset 0x80, size 0x2
};

// La clave de UN SOLO lado: ICEData de arriba guarda cada campo por pares (la
// tangente de entrada y la de salida) y esta es exactamente una de las dos
// mitades, campo por campo. 0x3C contra 0x84.
// total size: 0x3C
struct ICESingleKey {
    unsigned char nSpaceEye;  // offset 0x0, size 0x1
    unsigned char nSpaceLook; // offset 0x1, size 0x1
    float fTangentLength;     // offset 0x4, size 0x4
    float vEye[3];            // offset 0x8, size 0xC
    float vLook[3];           // offset 0x14, size 0xC
    float fDutch;             // offset 0x20, size 0x4
    float fLens;              // offset 0x24, size 0x4
    float fNearClip;          // offset 0x28, size 0x4
    float fNoiseAmplitude;    // offset 0x2C, size 0x4
    float fNoiseFrequency;    // offset 0x30, size 0x4
    float fFocalDistance;     // offset 0x34, size 0x4
    unsigned char fAperture;  // offset 0x38, size 0x1
    unsigned char fLetterbox; // offset 0x39, size 0x1
    unsigned char fSimSpeed;  // offset 0x3A, size 0x1
};

enum eICESpace { eDCE_CAR_SPACE = 0, eDCE_WORLD_SPACE = 1, eDCE_HYBRID_SPACE = 2, eDCE_SCENE_SPACE = 3, eDCE_NUM_SPACES = 4 };

enum eICEExitConfirmOptions { eDCE_SAVE_CONTINUE = 0, eDCE_NOSAVE_CONTINUE = 1, eDCE_CLEAR_DATA = 2, eDCE_NUM_EXIT_CONFIRM_OPTIONS = 3 };

enum eICEDeleteConfirmOptions { eDCE_DELETE_CONFIRM_NO = 0, eDCE_DELETE_CONFIRM_YES = 1, eDCE_NUM_DELETE_CONFIRM_OPTIONS = 2 };

enum eICEBrowseStatus { eDCR_OK = 0, eDCR_NONE = 1, eDCR_ERROR = 2 };

enum eICEBrowseOption { eDCB_CONTEXT = 0, eDCB_HANDLE = 1, eDCB_TRACK = 2, eDCB_DELETE = 3, eDCB_SAVE = 4, eDCB_MARKER = 5, eDCB_NUM_OPTIONS = 6 };

#endif
