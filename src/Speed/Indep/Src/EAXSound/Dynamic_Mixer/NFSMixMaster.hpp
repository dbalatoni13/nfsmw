//
//
//
//
//
//
//
//
#ifndef NFSMIXMASTER_HPP
#define NFSMIXMASTER_HPP // Decl: 10

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMap.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMapState.hpp"

struct NFSMixChannel;

// total size: 0xC
struct stDynMixMap {
    NFSMixMapState *pNext;   // offset 0x0, size 0x4
    NFSMixMapState *pPrev;   // offset 0x4, size 0x4
    NFSMixMapState *pMMProc; // offset 0x8, size 0x4
};

// total size: 0x74
// Decl: 39
class NFSMixMaster : public AudioMemBase {
  public:
    NFSMixMaster();           // Decl: 41
    ~NFSMixMaster() override; // Decl: 42

    void CreateMainMainMap(eRACETYPE eMapType); // Decl: 49
    void DestroyMainMainMap();                  // Decl: 50

    void SetMainMixMapPtr(int *pmem);
    void ConnectMap();
    void DestroyMap();
    void LoadMixMapFile(eMMTYPE etype, char *pfilename);

    static void LoadDataCallback(int param, int error_status);

    // NFSMixMap *GetMasterMixMap() {}

    void InitMixMap(int param);

    void TestSETAllocationParams();

    static void AssignSFXCallbacks(int *(*getptrcb)(int), void (*setsfxoutcb)(int, int *), bool (*setsfxincb)(int, int *), int (*getstaterefcnt)(int),
                                   void (*mapreadycb)());

    void ProcessMixMap(float dt, eCamStates ecam);

    // bool IsMixMapReady() {}

    void AssignMixMap(int *mapfile);

    int m_LoadMapID;               // offset 0x4, size 0x4
    stDynMixMap *m_pCurDynMMP;     // offset 0x8, size 0x4
    stDynMixMap *m_pHeadDynMMP;    // offset 0xC, size 0x4
    stDynMixMap *m_pTailDynMMP;    // offset 0x10, size 0x4
    stDynMixMap *m_pSecDynamicMMP; // offset 0x14, size 0x4
    stDynMixMap *m_pSecDynMMPHead; // offset 0x18, size 0x4
    stDynMixMap *m_pSecDynMMPTail; // offset 0x1C, size 0x4
    int *m_pMainMixMapData;        // offset 0x20, size 0x4
    int *m_pSecondaryMixMapData;   // offset 0x24, size 0x4
    NFSMixMap *m_pMainMixMap;      // offset 0x28, size 0x4
    NFSMixMap *m_pSecondaryMixMap; // offset 0x2C, size 0x4
    NFSMixChannel *m_pSFXMxCh;     // offset 0x30, size 0x4
    bool m_bMapReady;              // offset 0x34, size 0x1
    int m_StateRefCount[13];       // offset 0x38, size 0x34
    int m_nCurNumDMMP;             // offset 0x6C, size 0x4
    int m_nCurNumSecondaryDMMP;    // offset 0x70, size 0x4
};

#endif
