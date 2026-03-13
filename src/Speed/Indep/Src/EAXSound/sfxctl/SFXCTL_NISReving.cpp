#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"

NIS_RevManager *g_pNISRevMgr = nullptr;

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
    (void)t;
}

void NIS_RevManager::StartNISReving() {
    Start321Reving();
}
