#include "Speed/Indep/Src/EAXSound/NFSMixMaster.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"

NFSMixMaster::NFSMixMaster() {
    int n;

    m_pMainMixMap = nullptr;
    m_pSecondaryMixMap = nullptr;
    m_pCurDynMMP = nullptr;
    m_pHeadDynMMP = nullptr;
    m_pTailDynMMP = nullptr;
    m_pSecDynamicMMP = nullptr;
    m_pSecDynMMPHead = nullptr;
    m_pSecDynMMPTail = nullptr;
    *reinterpret_cast<int *>(&m_bMapReady) = 0;
    n = 0;
    do {
        m_StateRefCount[n] = 0;
        n = n + 1;
    } while (n < 13);
}

NFSMixMaster::~NFSMixMaster() {
    if (m_pMainMixMap) {
        delete m_pMainMixMap;
        m_pMainMixMap = nullptr;
    }
}

void *NFSMixMaster::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}

void NFSMixMaster::DestroyMainMainMap() {
    if (m_pMainMixMap) {
        m_pMainMixMap->DestroyMainMixMap();
        if (m_pMainMixMap) {
            m_pMainMixMap->~NFSMixMap();
            ::operator delete(m_pMainMixMap);
        }
        m_pMainMixMap = nullptr;
    }
    if (m_pMainMixMapData) {
        gAudioMemoryManager.FreeMemory(m_pMainMixMapData);
        m_pMainMixMapData = nullptr;
    }
    *reinterpret_cast<int *>(&m_bMapReady) = 0;
}

void NFSMixMaster::DestroyMap() {
    m_pMainMixMap->DestroyMainMixMap();
}

void NFSMixMaster::InitMixMap(int param) {
    int i;
    int n;

    m_pMainMixMap->InitMixMap(m_pMainMixMapData, m_pMainMixMap);
    m_pMainMixMap->AllocateMixerMemory();

    i = 0;
    do {
        int m = i + 1;
        m_StateRefCount[i] = m_pMainMixMap->GetMapStateCopies(i);
        i = m;
    } while (i < 13);

    i = 0;
    do {
        n = 0;
        if (m_StateRefCount[i] > 0) {
            do {
                m_pMainMixMap->CreateMainMapState(static_cast<eMAINMAPSTATES>(i), m_StateRefCount[i], n);
                n = n + 1;
            } while (n < m_StateRefCount[i]);
        }
        i = i + 1;
    } while (i < 13);

    m_pMainMixMap->AllocateInputArrays();

    i = 0;
    do {
        n = 0;
        if (m_pMainMixMap->m_StateRefCount[i] > 0) {
            do {
                NFSMixMapState *pmmp = m_pMainMixMap->m_pStateProcs[i]->GetMixMapProc(n);
                pmmp->CreateSubMixChannels();
                pmmp->CreateMasterMixChannels();
                n = n + 1;
            } while (n < m_pMainMixMap->m_StateRefCount[i]);
        }
        i = i + 1;
    } while (i < 13);

    m_pMainMixMap->InitMainMapStates();
    *reinterpret_cast<int *>(&m_bMapReady) = 1;
    (*NFSMixMap::mMapReadyCB)();
}

void NFSMixMaster::ProcessMixMap(float dt, eCamStates ecam) {
    if (*reinterpret_cast<int *>(&m_bMapReady) != 0) {
        m_pMainMixMap->ProcessMixMap(dt, ecam);
    }
}

void NFSMixMaster::AssignSFXCallbacks(int *(*GetPointerCB)(int), void (*SetSFXOutCB)(int, int *),
                                      bool (*SetSFXInputCB)(int, int *), int (*GetStateRefCountCB)(int),
                                      void (*MixReadyCB)()) {
    NFSMixMap::AssignSFXCallbacks(GetPointerCB, SetSFXOutCB, SetSFXInputCB, GetStateRefCountCB, MixReadyCB);
}
