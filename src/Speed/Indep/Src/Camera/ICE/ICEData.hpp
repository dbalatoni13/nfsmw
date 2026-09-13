#ifndef ICEDATA_HPP_
#define ICEDATA_HPP_

#include "Speed/Indep/Src/Camera/ICE/ICEMath.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "types.h"
#include "Speed/Indep/Src/Misc/attribuserinclude.h"

class ICEGroup;
class ICEShakeGroup;

#define ICE_ALIGN(value, alignment) (((value) + ((alignment) - 1)) & ~((alignment) - 1)) //  :239

enum ICEContext { eDCE_NIS = 0, eDCE_FMV = 1, eDCE_REPLAY = 2, eDCE_GENERIC = 3, eDCE_NUM_CONTEXTS = 4, eDCE_NOCONTEXT = 4 };

enum eICESpace { eDCE_CAR_SPACE = 0, eDCE_WORLD_SPACE = 1, eDCE_HYBRID_SPACE = 2, eDCE_SCENE_SPACE = 3, eDCE_NUM_SPACES = 4 };

// total size: 0x84
// total size: 0x84
class ICEData {
  public:
    uint8 nType;              // offset 0x0, size 0x1
    uint8 bSmooth;            // offset 0x1, size 0x1
    uint8 bCubicEye;          // offset 0x2, size 0x1
    uint8 bCubicLook;         // offset 0x3, size 0x1
    uint8 nSpaceEye;          // offset 0x4, size 0x1
    uint8 nSpaceLook;         // offset 0x5, size 0x1
    uint8 bConstrainToCars;   // offset 0x6, size 0x1
    uint8 bConstrainToWorld;  // offset 0x7, size 0x1
    uint8 bIgnoreOrientation; // offset 0x8, size 0x1
    uint8 bCarSpaceLag;       // offset 0x9, size 0x1
    uint8 nOverlay;           // offset 0xA, size 0x1
    uint32 nShakeType;        // offset 0xC, size 0x4
    float fParameter;         // offset 0x10, size 0x4
    float fTangentLength[2];  // offset 0x14, size 0x8
    float vEye[3][2];         // offset 0x1C, size 0x18
    float vLook[3][2];        // offset 0x34, size 0x18
    float fDutch[2];          // offset 0x4C, size 0x8
    float fLens[2];           // offset 0x54, size 0x8
    float fNearClip[2];       // offset 0x5C, size 0x8
    float fNoiseAmplitude[2]; // offset 0x64, size 0x8
    float fNoiseFrequency[2]; // offset 0x6C, size 0x8
    float fFocalDistance[2];  // offset 0x74, size 0x8
    uint8 fAperture[2];       // offset 0x7C, size 0x2
    uint8 fLetterbox[2];      // offset 0x7E, size 0x2
    uint8 fSimSpeed[2];       // offset 0x80, size 0x2

    void InitData();

    void PlatEndianSwap();

    void GetEye(int i, ICE::Vector3 *p);

    void GetLook(int i, ICE::Vector3 *p);
};

// total size: 0x3C
struct ICESingleKey {
    uint8 nSpaceEye;       // offset 0x0, size 0x1
    uint8 nSpaceLook;      // offset 0x1, size 0x1
    float fTangentLength;  // offset 0x4, size 0x4
    float vEye[3];         // offset 0x8, size 0xC
    float vLook[3];        // offset 0x14, size 0xC
    float fDutch;          // offset 0x20, size 0x4
    float fLens;           // offset 0x24, size 0x4
    float fNearClip;       // offset 0x28, size 0x4
    float fNoiseAmplitude; // offset 0x2C, size 0x4
    float fNoiseFrequency; // offset 0x30, size 0x4
    float fFocalDistance;  // offset 0x34, size 0x4
    uint8 fAperture;       // offset 0x38, size 0x1
    uint8 fLetterbox;      // offset 0x39, size 0x1
    uint8 fSimSpeed;       // offset 0x3A, size 0x1
};

// total size: 0x19F0
class ICETrack : public bTNode<ICETrack> {
  public:
    ICETrack(ICEGroup *group, char *name) {}
    ICETrack(ICEGroup *group, char *name, ICEData *p_data, int n_keys, float length) {}
    ~ICETrack() {}

    bool IsAllocated() {}

    char *GetName() {}

    void SetName(char *s) {}

    int GetNumKeys() {}
    void SetNumKeys(int n) {}

    ICEData *GetKey(int n) {}

    void SetData(ICEData *p_data, int n_keys, float length) {}

    int MemoryImageSize() {}

    float GetParameter();
    float GetParameter(int n) {}

    void SetGroup(ICEGroup *group) {}

    ICEGroup *GetGroup() {}

    void SetStart(float start) {}

    void SetLength(float length) {}

    float GetStart() {}

    float GetLength() {}

    int GetKeyNumber(ICEData *data) {}
    int GetKeyNumber(float f_param);
    ICEData *GetKey(float f_param) {}

    void PlatEndianSwap();

    int GetContext();

    ICEData *GetCameraData(float *p_start, float *p_end, float *p_current);

  private:
    ICEGroup *Group;  // offset 0x8, size 0x4
    float Start;      // offset 0xC, size 0x4
    float Length;     // offset 0x10, size 0x4
    int16 NumKeys;    // offset 0x14, size 0x2
    int8 Allocated;   // offset 0x16, size 0x1
    char Name[14];    // offset 0x17, size 0xE
    ICEData Keys[50]; // offset 0x28, size 0x19C8
};

// total size: 0x14
// Decl: 732
class ICEGroup {
  public:
    static void *operator new(size_t size, const char *debug_name) {}

    static void operator delete(void *ptr) {}

    ICEGroup() {}  // Decl: 740
    ~ICEGroup() {} // Decl: 741

    uint32 GetHandle() {} // Decl: 744

    void SetContext(int context) {} // Decl: 747

    int GetContext() {} // Decl: 745

    void SetHandle(uint32 n) {} // Decl: 746

    int GetNumTracks() {}

    ICETrack *GetTrack(int n);

    ICETrack *GetTrack(char *s);

    void AddTrack(ICETrack *track) {}

    void FlushTrack(ICETrack *track) {}

    void FlushTracks() {}

    void FlushAllocatedTracks();

    bTList<ICETrack> *GetTrackList() {}

  private:
    uint32 Handle;              // offset 0x0, size 0x4, Decl: 784
    int Context;                // offset 0x4, size 0x4, Decl: 785
    int NumTracks;              // offset 0x8, size 0x4
    bTList<ICETrack> TrackList; // offset 0xC, size 0x8
};

// total size: 0x18
struct ICEShakeData {
    ICEShakeData();
    void InitData();
    void PlatEndianSwap();

    float q[3]; // offset 0x0, size 0xC
    float p[3]; // offset 0xC, size 0xC
};

// total size: 0xB60
class ICEShakeTrack : public bTNode<ICEShakeTrack> {
  public:
    ~ICEShakeTrack() {}

    bool IsAllocated() {}

    char *GetName() {}

    void SetName(char *s) {}

    int GetNumKeys() {}

    ICEShakeData *GetKey(int n) {}

    void SetData(ICEShakeData *p_data, int n_keys) {}

    int MemoryImageSize() {}

    void SetGroup(ICEShakeGroup *group) {}

    ICEShakeGroup *GetGroup() {}

    int GetKeyNumber(ICEShakeData *data) {}

    void PlatEndianSwap();

    ICEShakeTrack(ICEShakeGroup *group, char *name) {}

    ICEShakeTrack(ICEShakeGroup *group, char *name, ICEShakeData *p_data, int n_keys) {}

    void SetNumKeys(int n) {}

    ICEShakeGroup *Group;   // offset 0x8, size 0x4
    int16 NumKeys;          // offset 0xC, size 0x2
    int8 Allocated;         // offset 0xE, size 0x1
    char Name[14];          // offset 0xF, size 0xE
    ICEShakeData Keys[120]; // offset 0x20, size 0xB40
};

// total size: 0xC
class ICEShakeGroup {
  public:
    ICEShakeGroup() {}
    ~ICEShakeGroup() {}

    int GetNumTracks() {}

    ICEShakeTrack *GetTrack(int n);

    void AddTrack(ICEShakeTrack *track) {}

    void FlushTracks() {}

    void FlushAllocatedTracks();

    bTList<ICEShakeTrack> *GetTrackList() {
        return &TrackList;
    }

  private:
    int NumTracks;                   // offset 0x0, size 0x4
    bTList<ICEShakeTrack> TrackList; // offset 0x4, size 0x8
};

enum eICEEditState {
    eDCE_OFF = 0,
    eDCE_BROWSER = 1,
    eDCE_EXIT_CONFIRM = 2,
    eDCE_DELETE_CONFIRM = 3,
    eDCE_LOADING_ANIMATION = 4,
    eDCE_PLAYING_ANIMATION = 5,
    eDCE_SCREENSHOT = 6,
    eDCE_DISTANCE = 7,
    eDCE_CAMERA_SETTINGS = 8,
    eDCE_START_DISTANCE = 9,
    eDCE_END_DISTANCE = 10,
    eDCE_START_KEY = 11,
    eDCE_END_KEY = 12,
    eDCE_START_KEY_AUX = 13,
    eDCE_END_KEY_AUX = 14,
    eDCE_SELECT_COPY_MODE = 15,
    eDCE_NUM_OF_EDIT_STATES = 16
};

enum eICEEditAction {
    eDCE_ADD = 0,
    eDCE_REMOVE = 1,
    eDCE_SET_TYPE = 2,
    eDCE_SET_DATA = 3,
    eDCE_SET_TANGENT_LENGTH = 4,
    eDCE_SET_DISTANCE = 5,
    eDCE_SET_SPACE_EYE = 6,
    eDCE_SET_SPACE_LOOK = 7,
    eDCE_TOGGLE_SMOOTH_ENTRY = 8,
    eDCE_TOGGLE_SMOOTH_EXIT = 9,
    eDCE_TOGGLE_CUBIC_EYE = 10,
    eDCE_TOGGLE_CUBIC_LOOK = 11,
    eDCE_TOGGLE_CONSTRAIN_CARS = 12,
    eDCE_TOGGLE_CONSTRAIN_WORLD = 13,
    eDCE_TOGGLE_IGNORE_ORIENTATION = 14,
    eDCE_TOGGLE_CAR_SPACE_LAG = 15,
    eDCE_SET_SPLINE_DISTANCE = 16,
    eDCE_SET_OVERLAY = 17,
    eDCE_SET_SHAKE_TYPE = 18,
    eDCE_SET_PARAMETER_LENGTH = 19
};

enum eICEExitConfirmOptions { eDCE_SAVE_CONTINUE = 0, eDCE_NOSAVE_CONTINUE = 1, eDCE_CLEAR_DATA = 2, eDCE_NUM_EXIT_CONFIRM_OPTIONS = 3 };

enum eICEDeleteConfirmOptions { eDCE_DELETE_CONFIRM_NO = 0, eDCE_DELETE_CONFIRM_YES = 1, eDCE_NUM_DELETE_CONFIRM_OPTIONS = 2 };

enum eICEBrowseStatus { eDCR_OK = 0, eDCR_NONE = 1, eDCR_ERROR = 2 };

enum eICEBrowseOption { eDCB_CONTEXT = 0, eDCB_HANDLE = 1, eDCB_TRACK = 2, eDCB_DELETE = 3, eDCB_SAVE = 4, eDCB_MARKER = 5, eDCB_NUM_OPTIONS = 6 };

enum eICECopyMode {
    eDCM_COPY_ALL = 0,
    eDCM_COPY_SPACE = 1,
    eDCM_COPY_TARGET = 2,
    eDCM_COPY_LENS = 3,
    eDCM_COPY_DOF = 4,
    eDCM_COPY_SHAKE = 5,
    eDCM_COPY_FX = 6,
    eDCM_NUM_COPY_MODES = 7
};

enum eICESetting {
    eDCS_TYPE = 0,
    eDCS_SMOOTH_ENTRY = 1,
    eDCS_SMOOTH_EXIT = 2,
    eDCS_SPACE_EYE = 3,
    eDCS_CUBIC_EYE = 4,
    eDCS_SPACE_LOOK = 5,
    eDCS_CUBIC_LOOK = 6,
    eDCS_CONSTRAIN_CARS = 7,
    eDCS_CONSTRAIN_WORLD = 8,
    eDCS_IGNORE_ORIENTATION = 9,
    eDCS_CAR_SPACE_LAG = 10,
    eDCS_OVERLAY = 11,
    eDCS_SHAKE_TYPE = 12,
    eDCS_NUM_SETTINGS = 13
};

#endif
