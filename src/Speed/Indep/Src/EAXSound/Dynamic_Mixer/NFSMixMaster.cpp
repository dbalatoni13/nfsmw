#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMaster.hpp"

NFSMixMaster::NFSMixMaster() {
    this->m_pMainMixMap = nullptr;
    this->m_pSecondaryMixMap = nullptr;
    this->m_pCurDynMMP = nullptr;
    this->m_pHeadDynMMP = nullptr;
    this->m_pTailDynMMP = nullptr;
    this->m_pSecDynamicMMP = nullptr;
    this->m_pSecDynMMPHead = nullptr;
    this->m_pSecDynMMPTail = nullptr;
    this->m_bMapReady = false;

    for (int n = 0; n < 13; n++) {
        this->m_StateRefCount[n] = 0;
    }
}

NFSMixMaster::~NFSMixMaster() {
    if (this->m_pMainMixMap != nullptr) {
        delete this->m_pMainMixMap;
        this->m_pMainMixMap = nullptr;
    }
}

void NFSMixMaster::DestroyMainMainMap() {
    if (this->m_pMainMixMap != nullptr) {
        this->m_pMainMixMap->DestroyMainMixMap();

        ::delete this->m_pMainMixMap;
        this->m_pMainMixMap = nullptr;
    }

    if (this->m_pMainMixMapData != nullptr) {
        gAudioMemoryManager.FreeMemory(this->m_pMainMixMapData);
        this->m_pMainMixMapData = nullptr;
    }

    this->m_bMapReady = false;
}

#ifdef CLANGD_DAMNIT
extern char *szMixMapFiles[4];
#endif

void NFSMixMaster::CreateMainMainMap(eRACETYPE eMapType) {
    this->m_bMapReady = false;
    this->m_pMainMixMap = ::new ("Dynamic MIx Map", 0) NFSMixMap();
    this->LoadMixMapFile(MMTYPE_MAIN, szMixMapFiles[eMapType]);
}

void NFSMixMaster::LoadMixMapFile(eMMTYPE etype, char *pfilename) {
    int nfilesize = bFileSize(pfilename);
    char *memptr = nullptr;
    int n;
    int dynindex = 0;
    int size;

    switch (etype) {
        case MMTYPE_MAIN:
            this->m_pMainMixMapData = reinterpret_cast<int *>(gAudioMemoryManager.AllocateMemory(nfilesize, "Main Dyn MixMap", false));
            memptr = reinterpret_cast<char *>(this->m_pMainMixMapData);
            break;
        case MMTYPE_SECONDARY:
            this->m_pSecondaryMixMapData = reinterpret_cast<int *>(new ("Event MixMap", 0) char[nfilesize]);
            memptr = reinterpret_cast<char *>(this->m_pSecondaryMixMapData);
            break;
        case MMTYPE_DYNAMIC:
            break;
        default:
            return;
    }

    this->m_LoadMapID = etype | (dynindex << 8);

    bFile *file = bOpen(pfilename, 1, 1);
    if (file != nullptr) {
        bRead(file, memptr, nfilesize);
        bClose(file);
    }

    NFSMixMaster::LoadDataCallback(reinterpret_cast<intptr_t>(this), 0);
}

void NFSMixMaster::LoadDataCallback(intptr_t param, int error_status) {}

// STRIPPED
void NFSMixMaster::AssignMixMap(int *mapfile) {}

// STRIPPED
void NFSMixMaster::TestSETAllocationParams() {}

void NFSMixMaster::DestroyMap() {
    this->m_pMainMixMap->DestroyMainMixMap();
}

void NFSMixMaster::InitMixMap(int param) {
    this->m_pMainMixMap->InitMixMap(this->m_pMainMixMapData, this->m_pMainMixMap);
    this->m_pMainMixMap->AllocateMixerMemory();

    for (int m = 0; m < 13; m++) {
        this->m_StateRefCount[m] = this->m_pMainMixMap->GetMapStateCopies(m);
    }

    int i;
    int n;
    for (i = 0; i < 13; i++) {
        for (n = 0; n < this->m_StateRefCount[i]; n++) {
            this->m_pMainMixMap->CreateMainMapState(static_cast<eMAINMAPSTATES>(i), this->m_StateRefCount[i], n);
        }
    }

    this->m_pMainMixMap->AllocateInputArrays();

    for (i = 0; i < 13; i++) {
        for (n = 0; n < this->m_pMainMixMap->m_StateRefCount[i]; n++) {
            NFSMixMapState *pmmp = this->m_pMainMixMap->m_pStateProcs[i]->GetMixMapProc(n);
            pmmp->CreateSubMixChannels();
            pmmp->CreateMasterMixChannels();
        }
    }

    this->m_pMainMixMap->InitMainMapStates();
    this->m_bMapReady = true;
    (*NFSMixMap::mMapReadyCB)();
}

void NFSMixMaster::AssignSFXCallbacks(int *(*getptrcb)(int), void (*setsfxoutcb)(int, int *), bool (*setsfxincb)(int, int *),
                                      int (*getstaterefcnt)(int), void (*mapreadycb)()) {
    NFSMixMap::AssignSFXCallbacks(getptrcb, setsfxoutcb, setsfxincb, getstaterefcnt, mapreadycb);
}

void NFSMixMaster::ProcessMixMap(float dt, eCamStates ecam) {
    if (this->m_bMapReady) {
        this->m_pMainMixMap->ProcessMixMap(dt, ecam);
    }
}
