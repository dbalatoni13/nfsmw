#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

NIS_RevManager *g_pNISRevMgr = nullptr;

void *NIS_RevManager::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}

NIS_RevManager::NIS_RevManager()
    : pRevData(nullptr) //
    , IsInitialized(false) //
    , pBuffer(nullptr) //
    , pCurEntry(nullptr) //
    , RecordingCount(0) {
    for (int i = 0; i < 16; ++i) {
        m_EngineDataSet[i].NumPoints = 0;
        m_EngineDataSet[i].DataPoints = nullptr;
    }
    g_pNISRevMgr = this;
}

void SFXCTL_Physics::MsgRevOff(const MAIEngineRev &message) {
    (void)message;
    eCurNisRevingState = NIS_OFF;
    pRevData = nullptr;
}

void NIS_RevManager::OpenNISRevData(unsigned int anim_id) {
    (void)anim_id;
    IsInitialized = false;
    pBuffer = nullptr;
    pCurEntry = nullptr;
    RecordingCount = 0;
}

void NIS_RevManager::Start321Reving() {
    CloseNIS();
    MAIEngineRev(0, 0, nullptr, 0).Send(UCrc32("QRev"));
}

void NIS_RevManager::CloseNIS() {
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

void NIS_RevManager::StartNISReving() {
    Start321Reving();
}
