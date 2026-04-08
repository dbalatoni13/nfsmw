#include "Speed/Indep/Src/EAXSound/NFSMixMaster.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"

char *szMixMapFiles[] = {
    "SOUND\\MixMaps\\MAPOUTPUT.mxb",
    "SOUND\\MixMaps\\MAPOUTPUTDRG.mxb",
    "SOUND\\MixMaps\\MAPOUTPUT2CR.mxb",
    "SOUND\\MixMaps\\MAPOUTPUT2DR.mxb",
};

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

void NFSMixMaster::CreateMainMainMap(eRACETYPE eMapType) {
    *reinterpret_cast<int *>(&m_bMapReady) = 0;
    m_pMainMixMap = new ("Dynamic MIx Map", 0) NFSMixMap();
    LoadMixMapFile(MMTYPE_MAIN, szMixMapFiles[eMapType]);
}

void NFSMixMaster::LoadMixMapFile(eMMTYPE etype, char *pfilename) {
    int nfilesize;
    char *memptr;
    int size;
    bFile *file;

    nfilesize = bFileSize(pfilename);
    memptr = nullptr;

    if (etype == MMTYPE_SECONDARY) {
        memptr = new ("Event MixMap", 0) char[nfilesize];
        m_pSecondaryMixMapData = reinterpret_cast<int *>(memptr);
    } else if (etype < 2) {
        if (etype != MMTYPE_MAIN) {
            return;
        }
        memptr = static_cast<char *>(gAudioMemoryManager.AllocateMemory(nfilesize, "Main Dyn MixMap", false));
        m_pMainMixMapData = reinterpret_cast<int *>(memptr);
    } else if (etype != MMTYPE_DYNAMIC) {
        return;
    }

    m_LoadMapID = etype;
    file = bOpen(pfilename, 1, 1);
    if (file) {
        size = nfilesize;
        bRead(file, memptr, size);
        bClose(file);
    }

    LoadDataCallback(reinterpret_cast<int>(this), 0);
}

void NFSMixMaster::LoadDataCallback(int, int) {}
