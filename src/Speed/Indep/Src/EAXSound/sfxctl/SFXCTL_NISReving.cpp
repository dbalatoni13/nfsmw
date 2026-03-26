#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern "C" void __builtin_delete(void *ptr);
extern int GetCsisEventIndex(unsigned int anim_id);
extern const char lbl_803D7358[];
extern unsigned char uNIS_STRINGHASHMAP[];

NIS_RevManager *g_pNISRevMgr = nullptr;

void *NIS_RevManager::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}

NIS_RevManager::NIS_RevManager() {
    g_pNISRevMgr = this;
    pRevData = nullptr;
    pBuffer = nullptr;
    RecordingCount = 0;
    *reinterpret_cast<int *>(&IsInitialized) = 0;
}


void SFXCTL_Physics::MsgRevOff(const MAIEngineRev &message) {
    (void)message;
    eCurNisRevingState = NIS_OFF;
    pRevData = nullptr;
}

void NIS_RevManager::OpenNISRevData(unsigned int anim_id) {
    int index;
    char *AnimName;
    char EngineRevBin[50];

    *reinterpret_cast<int *>(&IsInitialized) = 0;
    index = GetCsisEventIndex(anim_id);
    if (index != -1) {
        AnimName = reinterpret_cast<char **>(uNIS_STRINGHASHMAP + 8)[index * 3];
        bSPrintf(EngineRevBin, lbl_803D7358, AnimName);
        index = bFileExists(EngineRevBin);
        if (index) {
            int nfilesize;
            bFile *file;
            int *pTemp;

            nfilesize = bFileSize(EngineRevBin);
            file = bOpen(EngineRevBin, 1, 1);
            if (file) {
                if (pRevData) {
                    __builtin_delete(pRevData);
                    pRevData = nullptr;
                }

                pRevData = static_cast<int *>(bMalloc(nfilesize, EngineRevBin, 0, 0x1040));
                bRead(file, pRevData, nfilesize);
                bClose(file);

                {
                    int *ptmpSTart;
                    int nloopsize;

                    ptmpSTart = pRevData;
                    nloopsize = nfilesize;
                    if (nfilesize < 0) {
                        nloopsize += 3;
                    }
                    nloopsize >>= 2;

                    for (int n = 0; n < nloopsize; ++n) {
                        bPlatEndianSwap(ptmpSTart);
                        ++ptmpSTart;
                    }
                }

                {
                    pTemp = pRevData;
                    for (int n = 0; n < 16; ++n) {
                        m_EngineDataSet[n].NumPoints = *pTemp++;
                        if (m_EngineDataSet[n].NumPoints != 0) {
                            m_EngineDataSet[n].DataPoints = reinterpret_cast<EngRevDataPoint *>(pTemp);
                            pTemp += m_EngineDataSet[n].NumPoints * 3;
                        }
                    }
                }

                *reinterpret_cast<int *>(&IsInitialized) = 1;
            }
        } else {
            *reinterpret_cast<int *>(&IsInitialized) = index;
        }
    }
}

void NIS_RevManager::Start321Reving() {
    CloseNIS();
    MAIEngineRev(0, 0, nullptr, 0).Send(UCrc32("QRev"));
}

void NIS_RevManager::CloseNIS() {
    bFree(pRevData);
    pRevData = nullptr;
    MAIEngineRev(0, 0, nullptr, 0).Send(UCrc32("RevOFF"));
    IsInitialized = false;
}

void NIS_RevManager::Update(float t) {
    if (INIS::Get() != nullptr && INIS::Get()->IsPlaying()) {
        {
            for (int n = 0; n < EAX_CarState::Count(); ++n) {
                EAX_CarState *pcar = EAX_CarState::GetList()[n];
                pcar->SetNISCarID(-1);
            }
        }

        {
            for (int i = 0; i < 16; ++i) {
                char channelName[32];
                int id;
                IVehicle *NISCar;
                ISimable *isim;
                EAX_CarState *state;

                if (i <= 7) {
                    bSPrintf(channelName, "car%d", i + 1);
                } else {
                    bSPrintf(channelName, "cop%d", i - 7);
                }

                NISCar = INIS::Get()->GetCar(UCrc32(channelName));
                if (NISCar) {
                    isim = NISCar->GetSimable();
                    if (isim) {
                        state = EAX_CarState::Find(isim->GetWorldID());
                        if (state) {
                            state->SetNISCarID(i);
                        }
                    }
                }
            }
        }
    }

    (void)t;
}

void NIS_RevManager::StartNISReving() {}

NIS_RevManager::~NIS_RevManager() {
    g_pNISRevMgr = nullptr;
    delete pRevData;
    delete pBuffer;
    RecordingCount = 0;
}
