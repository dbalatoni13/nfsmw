//
#ifndef NFSMIXERDEFINES_HPP
#define NFSMIXERDEFINES_HPP
#define MAX_NUM_NFSMIX_EVENTS 5              // Decl: 8
#define MAXMAPSTATES 4                       // Decl: 9
#define MAXMAPID 1                           // Decl: 10
#define MAXNUMINSTANCE 32                    // Decl: 11
#define SFXBASE_CONTROL_OFFSET 15            // Decl: 14
#define MASK_OBJID 0xe0000000                // Decl: 15
#define MASK_MAIN_MIXCHIN 0x20000000         // Decl: 16
#define MASK_MAIN_SUBMIXCHIN 0x30000000      // Decl: 17
#define MASK_SECONDARY_MIXCHIN 0x20020000    // Decl: 18
#define MASK_SECONDARY_SUBMIXCHIN 0x20030000 // Decl: 19
#define STATESHIFT 16                        // Decl: 21
#define MIXCTL_CURVESHIFT 24                 // Decl: 22

// Decl: 87
enum DMX_PRESET_TYPE {
    DMX_VOL = 0,
    DMX_PITCH = 1,
    DMX_FREQ = 2,
    DMX_AZIM = 3,
    DMX_DEPTH = 4,
};

// Decl: 165
enum eTP3DPosInputs {
    DIST_TO_PLYR_CAR = 0,
    DIST_TO_PLYR_CAM = 1,
    AZIM_TO_PLYR_CAR = 2,
    AZIM_TO_PLYR_CAM = 3,
    AZIM_TO_CAMDIR_CARPOS = 4,
    AZIM_CAM_TO_OBJ = 5,
    AZIM_CAR_TO_OBJ = 6,
    PREV_AZIM_TO_PLYR_CAR = 7,
    PREV_AZIM_TO_PLYR_CAM = 8,
    PREV_AZIM_TO_CAMDIR_CARPOS = 9,
    ROT_TO_CAMERA = 10,
    PLAYER_CAR_REF = 11,
    MAX_ROLLOFF_DIST = 12,
    VEL_DELTA_CAR = 13,
    VEL_DELTA_CAM = 14,
    RELATIVE_PARMS = 15,
};

// Decl: 208
enum eDistTargets {
    DIST_OBJ_NONE = -1,
    DIST_OBJ_TO_CAM = 0,
    DIST_OBJ_TO_PLAYERCAR = 1,
    DIST_OBJ_TO_CAM_TARGET = 2,
};

// total size: 0x10
// Decl: 281
struct stMixMapHeader {
    int MixMapID;         // offset 0x0, size 0x4
    int NumStates;        // offset 0x4, size 0x4
    int StateTableOffset; // offset 0x8, size 0x4
    int DynamicMapOffset; // offset 0xC, size 0x4
};

// total size: 0x20
// Decl: 296
struct stMixMapStateHdr {
    int StateIndex;          // offset 0x0, size 0x4
    int OffsetMixCtlData;    // offset 0x4, size 0x4
    int Offset3DMixCtlData;  // offset 0x8, size 0x4
    int OffsetSubMixData;    // offset 0xC, size 0x4
    int OffsetMasterMixData; // offset 0x10, size 0x4
    int OffsetPresetData;    // offset 0x14, size 0x4
    int OffsetEventCtlData;  // offset 0x18, size 0x4
    int Offset_Reserved_07;  // offset 0x1C, size 0x4
};

// total size: 0x10
// Decl: 323
struct stCurveDataProc {
    int nINPUTID;     // offset 0x0, size 0x4
    int *pInputParam; // offset 0x4, size 0x4
    int dBOutput;     // offset 0x8, size 0x4
    int Q15Output;    // offset 0xC, size 0x4
};

// total size: 0x8
// Decl: 331
struct stMixCtlParams {
    int nINPUTID;        // offset 0x0, size 0x4
    int nUScaleCntSwing; // offset 0x4, size 0x4
};

// total size: 0x10
// Decl: 341
struct stMixCtlSharedData {
    stMixCtlParams *pstMixCtlParms; // offset 0x0, size 0x4
    int MIXCTLOBJID;                // offset 0x4, size 0x4
    int nOffset;                    // offset 0x8, size 0x4
    int nRatio;                     // offset 0xC, size 0x4
};

// total size: 0xC
// Decl: 349
struct stMixCtlUniqueData {
    stCurveDataProc *pstCurveData; // offset 0x0, size 0x4
    int **ppScaleRatios;           // offset 0x4, size 0x4
    int CmpdBOut;                  // offset 0x8, size 0x4
};

// total size: 0x8
// Decl: 357
struct stMixCtlProc {
    stMixCtlSharedData *psdata; // offset 0x0, size 0x4
    stMixCtlUniqueData *pudata; // offset 0x4, size 0x4
};

// total size: 0x18
// Decl: 362
struct st3DStateParams {
    int n3DSTATEINFOID;   // offset 0x0, size 0x4
    int nCURVEID_DOPPLER; // offset 0x4, size 0x4
    int nQ0MinMax;        // offset 0x8, size 0x4
    int nQ1MinMax;        // offset 0xC, size 0x4
    int nQ2MinMax;        // offset 0x10, size 0x4
    int nQ3MinMax;        // offset 0x14, size 0x4
};

typedef st3DStateParams ST3DSTATEPARAMS; // Decl: 371

// total size: 0x1C
// Decl: 374
struct st3DMixCtlParams {
    int nINPUTID;                // offset 0x0, size 0x4
    ST3DSTATEPARAMS StateParams; // offset 0x4, size 0x18
};

// total size: 0x14
// Decl: 380
struct st3DMixCtlSharedData {
    st3DMixCtlParams *pMapParams;     // offset 0x0, size 0x4
    ST3DSTATEPARAMS *pCurStateParams; // offset 0x4, size 0x4
    int CurCamState;                  // offset 0x8, size 0x4
    int PrevCamState;                 // offset 0xC, size 0x4
    int msSinceCamTrans;              // offset 0x10, size 0x4
};

// total size: 0x20
// Decl: 398
struct st3DMixCtlUniqueData {
    int nINPUTID;         // offset 0x0, size 0x4
    int *pInputs;         // offset 0x4, size 0x4
    int azimuth;          // offset 0x8, size 0x4
    int dBRolloff;        // offset 0xC, size 0x4
    int q15Rolloff;       // offset 0x10, size 0x4
    int DopplerCents;     // offset 0x14, size 0x4
    float fPrevDist;      // offset 0x18, size 0x4
    float fPrevDeltaDist; // offset 0x1C, size 0x4
};

// total size: 0x8
// Decl: 409
struct st3DMixCtlProc {
    st3DMixCtlSharedData *p3DMixCtlData_S; // offset 0x0, size 0x4
    st3DMixCtlUniqueData *p3DMixCtlData_U; // offset 0x4, size 0x4
};

// total size: 0xC
// Decl: 417
struct stMasterMixChParams {
    int MIXCHID;  // offset 0x0, size 0x4
    int MixData;  // offset 0x4, size 0x4
    int SFXOBJID; // offset 0x8, size 0x4
};

// total size: 0x8
// Decl: 424
struct stSubMixChParams {
    int MIXCHID;         // offset 0x0, size 0x4
    int UpperLowerSwing; // offset 0x4, size 0x4
};

// total size: 0x10
// Decl: 434
struct stMasterMixChSharedData {
    stMasterMixChParams *pMapParams; // offset 0x0, size 0x4
    int MIXCHINID;                   // offset 0x4, size 0x4
    int NumInputs;                   // offset 0x8, size 0x4
    int *pPRESETS;                   // offset 0xC, size 0x4
};

// total size: 0x14
// Decl: 442
struct stMasterMixChUniqueData {
    int outputID;             // offset 0x0, size 0x4
    int *pInputs;             // offset 0x4, size 0x4
    int Output;               // offset 0x8, size 0x4
    st3DMixCtlProc **p3DData; // offset 0xC, size 0x4
    int *pOutputs;            // offset 0x10, size 0x4
};

// total size: 0xC
// Decl: 450
struct stMixChSharedData {
    stSubMixChParams *pMapParams; // offset 0x0, size 0x4
    int MIXCHINID;                // offset 0x4, size 0x4
    int NumInputs;                // offset 0x8, size 0x4
};

// total size: 0x8
// Decl: 457
struct stMixChUniqueData {
    int *pInputs; // offset 0x0, size 0x4
    int Output;   // offset 0x4, size 0x4
};

// total size: 0x8
// Decl: 462
struct stSubMixChProc {
    stMixChSharedData *pMixChData_S; // offset 0x0, size 0x4
    stMixChUniqueData *pMixChData_U; // offset 0x4, size 0x4
};

// total size: 0x8
// Decl: 467
struct stMasterMixChProc {
    stMasterMixChSharedData *pMixChData_S; // offset 0x0, size 0x4
    stMasterMixChUniqueData *pMixChData_U; // offset 0x4, size 0x4
};

// total size: 0x10
// Decl: 549
struct st3DMixCtlHdr {
    int Num3DMixCtls;        // offset 0x0, size 0x4
    int NumMainMap3DMixCtls; // offset 0x4, size 0x4
    int Reserved_02;         // offset 0x8, size 0x4
    int Reserved_03;         // offset 0xC, size 0x4
};

// total size: 0x10
// Decl: 556
struct stMixCtlHdr {
    int NumMixCtls;          // offset 0x0, size 0x4
    int NumNewMixDataProcs;  // offset 0x4, size 0x4
    int NumMainMixDataProcs; // offset 0x8, size 0x4
    int NumMainMixCtlOut;    // offset 0xC, size 0x4
};

// total size: 0x10
// Decl: 573
struct stMixChHdr {
    int NumMixChannels;   // offset 0x0, size 0x4
    int NumUniqueSFXOBJs; // offset 0x4, size 0x4
    int NumMainIn;        // offset 0x8, size 0x4
    int NumSecIn;         // offset 0xC, size 0x4
};

// total size: 0x10
// Decl: 582
struct stMixEventHdr {
    int NumEvents;   // offset 0x0, size 0x4
    int Reserved_01; // offset 0x4, size 0x4
    int Reserved_02; // offset 0x8, size 0x4
    int Reserved_03; // offset 0xC, size 0x4
};

// total size: 0x18
// Decl: 610
struct stMixEvtParams {
    int nEVTCTLID;       // offset 0x0, size 0x4
    int nUScaleCntSwing; // offset 0x4, size 0x4
    int nTriggerID;      // offset 0x8, size 0x4
    int nParam_00;       // offset 0xC, size 0x4
    int nParam_01;       // offset 0x10, size 0x4
    int nParam_02;       // offset 0x14, size 0x4
};

// total size: 0x4
// Decl: 622
struct stEvtMixCtlSharedData {
    stMixEvtParams *pMapParms; // offset 0x0, size 0x4
};

// total size: 0x20
// Decl: 641
struct stEvtMixCtlUniqueData {
    float msTimeElapsed; // offset 0x0, size 0x4
    float msResetTime;   // offset 0x4, size 0x4
    int reset;           // offset 0x8, size 0x4
    int reset_level;     // offset 0xC, size 0x4
    int *pTriggerPtr;    // offset 0x10, size 0x4
    int **ppScaleRatios; // offset 0x14, size 0x4
    int output;          // offset 0x18, size 0x4
    int qoutput;         // offset 0x1C, size 0x4
};

// total size: 0x8
// Decl: 672
struct stEvtMixCtlProc {
    stEvtMixCtlSharedData *pData_S; // offset 0x0, size 0x4
    stEvtMixCtlUniqueData *pData_U; // offset 0x4, size 0x4
};

// Decl: 677
enum eMIXTABLEID {
    SHAPE_DWN_EQPWR = 0,
    SHAPE_UP_EQPWR = 1,
    SHAPE_DWN_EQPWR_SQ = 2,
    SHAPE_UP_EQPWR_SQ = 3,
    SHAPE_DWN_ONE_MIN_EQPWR = 4,
    SHAPE_UP_ONE_MIN_EQPWR = 5,
    SHAPE_DWN_ONE_MIN_EQPWR_SQ = 6,
    SHAPE_UP_ONE_MIN_EQPWR_SQ = 7,
    SHAPE_DWN_LINEAR = 8,
    SHAPE_UP_LINEAR = 9,
    MAX_SHAPE_TYPES = 10,
};

#define SFXCTL_NONE -1 // Decl: 804
// #define intptr unsigned int  // Decl: 806 // TODO
#define MAX_NUM_MIXSTATES 25 // Decl: 808
#define DEFAULT_CAM 0        // Decl: 809

// Decl: 838
enum eDMIXENVELOPS {
    DMENV_AR = 0,
    DMENV_ASR = 1,
    DMENV_ATR = 2,
    DMENV_LFO = 3,
};

#endif
