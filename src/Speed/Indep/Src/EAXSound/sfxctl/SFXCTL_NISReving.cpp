#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_NISReving.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/Generated/Messages/MAIEngineRev.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"


extern int GetCsisEventIndex(unsigned int anim_id);
extern const char lbl_803D7358[];
extern unsigned char uNIS_STRINGHASHMAP[];

NIS_RevManager *g_pNISRevMgr = nullptr;

EngRevDataPoint RevPat5[] = {
    EngRevDataPoint(0.0f, 1000, 0),
    EngRevDataPoint(0.3f, 1631, 83),
    EngRevDataPoint(0.6f, 2833, 100),
    EngRevDataPoint(0.9f, 4250, 100),
    EngRevDataPoint(1.2f, 5856, 100),
    EngRevDataPoint(1.5f, 6746, 70),
    EngRevDataPoint(1.8f, 6098, 0),
    EngRevDataPoint(2.1f, 5970, 30),
    EngRevDataPoint(2.4f, 7556, 100),
    EngRevDataPoint(2.7f, 9337, 100),
    EngRevDataPoint(3.0f, 9939, 100),
    EngRevDataPoint(3.3f, 9946, 100),
    EngRevDataPoint(3.6f, 9964, 100),
    EngRevDataPoint(3.9f, 9906, 100),
    EngRevDataPoint(4.2f, 9348, 16),
    EngRevDataPoint(4.5f, 8678, 0),
    EngRevDataPoint(4.8f, 7979, 0),
    EngRevDataPoint(5.1f, 7267, 0),
    EngRevDataPoint(5.4f, 6551, 0),
    EngRevDataPoint(5.7f, 5830, 0),
    EngRevDataPoint(6.0f, 5111, 0),
    EngRevDataPoint(6.3f, 4395, 0),
    EngRevDataPoint(6.6f, 3676, 0),
    EngRevDataPoint(6.9f, 2959, 0),
    EngRevDataPoint(7.2f, 2377, 0),
    EngRevDataPoint(7.5f, 2137, 0),
    EngRevDataPoint(7.8f, 1897, 0),
    EngRevDataPoint(8.1f, 1657, 0)
};

EngRevDataPoint RevPat6[] = {
    EngRevDataPoint(0.0f, 1000, 0),
    EngRevDataPoint(0.3f, 1052, 10),
    EngRevDataPoint(0.6f, 2010, 96),
    EngRevDataPoint(0.9f, 3285, 100),
    EngRevDataPoint(1.2f, 4770, 100),
    EngRevDataPoint(1.5f, 6427, 100),
    EngRevDataPoint(1.8f, 7965, 90),
    EngRevDataPoint(2.1f, 7571, 3),
    EngRevDataPoint(2.4f, 6864, 0),
    EngRevDataPoint(2.7f, 6219, 10),
    EngRevDataPoint(3.0f, 7544, 96),
    EngRevDataPoint(3.3f, 9154, 90),
    EngRevDataPoint(3.6f, 8831, 3),
    EngRevDataPoint(3.9f, 8154, 0),
    EngRevDataPoint(4.2f, 7443, 0),
    EngRevDataPoint(4.5f, 6727, 0),
    EngRevDataPoint(4.8f, 6008, 0),
    EngRevDataPoint(5.1f, 5291, 0),
    EngRevDataPoint(5.4f, 4572, 0),
    EngRevDataPoint(5.7f, 3853, 0),
    EngRevDataPoint(6.0f, 3137, 0),
    EngRevDataPoint(6.3f, 2433, 0),
    EngRevDataPoint(6.6f, 2193, 0)
};

EngRevDataPoint RevPat7[] = {
    EngRevDataPoint(0.0f, 1037, 47),
    EngRevDataPoint(0.3f, 1192, 54),
    EngRevDataPoint(0.6f, 1674, 72),
    EngRevDataPoint(0.9f, 2496, 85),
    EngRevDataPoint(1.2f, 3506, 87),
    EngRevDataPoint(1.5f, 4960, 100),
    EngRevDataPoint(1.8f, 6634, 100),
    EngRevDataPoint(2.1f, 8375, 97),
    EngRevDataPoint(2.4f, 8146, 12),
    EngRevDataPoint(2.7f, 7455, 0),
    EngRevDataPoint(3.0f, 6741, 0),
    EngRevDataPoint(3.3f, 6022, 0),
    EngRevDataPoint(3.6f, 5304, 0),
    EngRevDataPoint(3.9f, 4591, 0),
    EngRevDataPoint(4.2f, 3872, 0),
    EngRevDataPoint(4.5f, 3272, 20),
    EngRevDataPoint(4.8f, 2823, 6),
    EngRevDataPoint(5.1f, 2302, 0),
    EngRevDataPoint(5.4f, 2062, 0),
    EngRevDataPoint(5.7f, 1822, 0)
};

EngRevDataPoint RevPat8[] = {
    EngRevDataPoint(0.0f, 2476, 100),
    EngRevDataPoint(0.3f, 2714, 100),
    EngRevDataPoint(0.6f, 2763, 100),
    EngRevDataPoint(0.9f, 2747, 100),
    EngRevDataPoint(1.2f, 2944, 100),
    EngRevDataPoint(1.5f, 3093, 100),
    EngRevDataPoint(1.8f, 3240, 100),
    EngRevDataPoint(2.1f, 3393, 100),
    EngRevDataPoint(2.4f, 3550, 100),
    EngRevDataPoint(2.7f, 3707, 100),
    EngRevDataPoint(3.0f, 3868, 100),
    EngRevDataPoint(3.3f, 4031, 100),
    EngRevDataPoint(3.6f, 4197, 100),
    EngRevDataPoint(3.9f, 4365, 100),
    EngRevDataPoint(4.2f, 4535, 100),
    EngRevDataPoint(4.5f, 4707, 100),
    EngRevDataPoint(4.8f, 4882, 100),
    EngRevDataPoint(5.1f, 5058, 100),
    EngRevDataPoint(5.4f, 5230, 100),
    EngRevDataPoint(5.7f, 5409, 100),
    EngRevDataPoint(6.0f, 5591, 100),
    EngRevDataPoint(6.3f, 5773, 100),
    EngRevDataPoint(6.6f, 5958, 100),
    EngRevDataPoint(6.9f, 6144, 100),
    EngRevDataPoint(7.2f, 6330, 100),
    EngRevDataPoint(7.5f, 6518, 100),
    EngRevDataPoint(7.8f, 6542, 16),
    EngRevDataPoint(8.1f, 6430, 0),
    EngRevDataPoint(8.4f, 6328, 14),
    EngRevDataPoint(8.7f, 6359, 76),
    EngRevDataPoint(9.0f, 6538, 100),
    EngRevDataPoint(9.3f, 6727, 100),
    EngRevDataPoint(9.6f, 6916, 100),
    EngRevDataPoint(9.9f, 7105, 100),
    EngRevDataPoint(10.2f, 7295, 100),
    EngRevDataPoint(10.5f, 7485, 100),
    EngRevDataPoint(10.8f, 7675, 100),
    EngRevDataPoint(11.1f, 7866, 100),
    EngRevDataPoint(11.4f, 8058, 100),
    EngRevDataPoint(11.7f, 8234, 90),
    EngRevDataPoint(12.0f, 8048, 3),
    EngRevDataPoint(12.3f, 7614, 0),
    EngRevDataPoint(12.6f, 7136, 0),
    EngRevDataPoint(12.9f, 6689, 0),
    EngRevDataPoint(13.2f, 6246, 0),
    EngRevDataPoint(13.5f, 5909, 0),
    EngRevDataPoint(13.8f, 5722, 0),
    EngRevDataPoint(14.1f, 5582, 0),
    EngRevDataPoint(14.4f, 8915, 62),
    EngRevDataPoint(14.7f, 9083, 68),
    EngRevDataPoint(15.0f, 8884, 6),
    EngRevDataPoint(15.3f, 8618, 0),
    EngRevDataPoint(15.6f, 8182, 0),
    EngRevDataPoint(15.9f, 7646, 0),
    EngRevDataPoint(16.2f, 7014, 0),
    EngRevDataPoint(16.5f, 6275, 0),
    EngRevDataPoint(16.8f, 5443, 0),
    EngRevDataPoint(17.1f, 4696, 0),
    EngRevDataPoint(17.4f, 4412, 0),
    EngRevDataPoint(17.7f, 4134, 0),
    EngRevDataPoint(18.0f, 3862, 0),
    EngRevDataPoint(18.3f, 3595, 0),
    EngRevDataPoint(18.6f, 3335, 0),
    EngRevDataPoint(18.9f, 3079, 0),
    EngRevDataPoint(19.2f, 2829, 0),
    EngRevDataPoint(19.5f, 2582, 0),
    EngRevDataPoint(19.8f, 2342, 0),
    EngRevDataPoint(20.1f, 2106, 0)
};

EngRevDataPoint RevPat9[] = {
    EngRevDataPoint(0.0f, 1588, 0),
    EngRevDataPoint(0.3f, 1546, 49),
    EngRevDataPoint(0.6f, 1580, 50),
    EngRevDataPoint(0.9f, 1701, 76),
    EngRevDataPoint(1.2f, 1983, 94),
    EngRevDataPoint(1.5f, 2410, 100),
    EngRevDataPoint(1.8f, 2856, 100),
    EngRevDataPoint(2.1f, 3318, 100),
    EngRevDataPoint(2.4f, 3785, 100),
    EngRevDataPoint(2.7f, 4266, 100),
    EngRevDataPoint(3.0f, 4759, 100),
    EngRevDataPoint(3.3f, 5266, 100),
    EngRevDataPoint(3.6f, 5597, 70),
    EngRevDataPoint(3.9f, 5379, 0),
    EngRevDataPoint(4.2f, 5193, 28),
    EngRevDataPoint(4.5f, 5612, 100),
    EngRevDataPoint(4.8f, 6160, 100),
    EngRevDataPoint(5.1f, 6729, 100),
    EngRevDataPoint(5.4f, 7335, 100),
    EngRevDataPoint(5.7f, 7977, 94),
    EngRevDataPoint(6.0f, 7922, 7),
    EngRevDataPoint(6.3f, 7648, 0),
    EngRevDataPoint(6.6f, 7363, 0),
    EngRevDataPoint(6.9f, 7294, 30),
    EngRevDataPoint(7.2f, 7890, 100),
    EngRevDataPoint(7.5f, 8600, 100),
    EngRevDataPoint(7.8f, 9347, 100),
    EngRevDataPoint(8.1f, 9884, 90),
    EngRevDataPoint(8.4f, 9648, 3),
    EngRevDataPoint(8.7f, 9380, 0),
    EngRevDataPoint(9.0f, 8849, 0),
    EngRevDataPoint(9.3f, 8384, 0),
    EngRevDataPoint(9.6f, 7958, 0),
    EngRevDataPoint(9.9f, 7550, 0),
    EngRevDataPoint(10.2f, 7170, 0),
    EngRevDataPoint(10.5f, 6808, 0),
    EngRevDataPoint(10.8f, 6460, 0),
    EngRevDataPoint(11.1f, 6132, 0),
    EngRevDataPoint(11.4f, 5827, 0),
    EngRevDataPoint(11.7f, 5105, 0),
    EngRevDataPoint(12.0f, 4390, 0),
    EngRevDataPoint(12.3f, 3671, 0),
    EngRevDataPoint(12.6f, 3038, 0),
    EngRevDataPoint(12.9f, 3339, 0),
    EngRevDataPoint(13.2f, 3644, 0),
    EngRevDataPoint(13.5f, 3558, 0),
    EngRevDataPoint(13.8f, 3305, 0),
    EngRevDataPoint(14.1f, 3049, 0),
    EngRevDataPoint(14.4f, 2799, 0),
    EngRevDataPoint(14.7f, 2553, 0),
    EngRevDataPoint(15.0f, 2313, 0),
    EngRevDataPoint(15.3f, 2077, 0),
    EngRevDataPoint(15.6f, 1847, 0),
    EngRevDataPoint(15.9f, 1612, 0),
    EngRevDataPoint(16.2f, 1372, 0),
    EngRevDataPoint(16.5f, 1132, 0),
    EngRevDataPoint(16.8f, 1004, 0)
};

EngRevDataPoint RevPat10[] = {
    EngRevDataPoint(0.0f, 1864, 96),
    EngRevDataPoint(0.3f, 3111, 100),
    EngRevDataPoint(0.6f, 4570, 100),
    EngRevDataPoint(0.9f, 6211, 100),
    EngRevDataPoint(1.2f, 7707, 81),
    EngRevDataPoint(1.5f, 8127, 47),
    EngRevDataPoint(1.8f, 8455, 49),
    EngRevDataPoint(2.1f, 8571, 38),
    EngRevDataPoint(2.4f, 8611, 38),
    EngRevDataPoint(2.7f, 8650, 38),
    EngRevDataPoint(3.0f, 8689, 38),
    EngRevDataPoint(3.3f, 8731, 38),
    EngRevDataPoint(3.6f, 8629, 24),
    EngRevDataPoint(3.9f, 8023, 0),
    EngRevDataPoint(4.2f, 7315, 0),
    EngRevDataPoint(4.5f, 6590, 0),
    EngRevDataPoint(4.8f, 5879, 0),
    EngRevDataPoint(5.1f, 5290, 21),
    EngRevDataPoint(5.4f, 6618, 100),
    EngRevDataPoint(5.7f, 6547, 16),
    EngRevDataPoint(6.0f, 5839, 0),
    EngRevDataPoint(6.3f, 7064, 83),
    EngRevDataPoint(6.6f, 7489, 37),
    EngRevDataPoint(6.9f, 6814, 0),
    EngRevDataPoint(7.2f, 6098, 0),
    EngRevDataPoint(7.5f, 5376, 0),
    EngRevDataPoint(7.8f, 4663, 0),
    EngRevDataPoint(8.1f, 3947, 0),
    EngRevDataPoint(8.4f, 3227, 0),
    EngRevDataPoint(8.7f, 2525, 0),
    EngRevDataPoint(9.0f, 2187, 0),
    EngRevDataPoint(9.3f, 1947, 0),
    EngRevDataPoint(9.6f, 1707, 0),
    EngRevDataPoint(9.9f, 1467, 0),
    EngRevDataPoint(10.2f, 1227, 0)
};

EngRevDataPoint RevPat11[] = {
    EngRevDataPoint(0.0f, 1148, 30),
    EngRevDataPoint(0.3f, 2207, 100),
    EngRevDataPoint(0.6f, 3517, 100),
    EngRevDataPoint(0.9f, 5035, 100),
    EngRevDataPoint(1.2f, 6716, 100),
    EngRevDataPoint(1.5f, 8489, 100),
    EngRevDataPoint(1.8f, 9788, 100),
    EngRevDataPoint(2.1f, 9508, 16),
    EngRevDataPoint(2.4f, 8852, 0),
    EngRevDataPoint(2.7f, 8161, 0),
    EngRevDataPoint(3.0f, 7450, 0),
    EngRevDataPoint(3.3f, 6733, 0),
    EngRevDataPoint(3.6f, 6013, 0),
    EngRevDataPoint(3.9f, 6603, 60),
    EngRevDataPoint(4.2f, 8317, 100),
    EngRevDataPoint(4.5f, 8107, 16),
    EngRevDataPoint(4.8f, 7414, 0),
    EngRevDataPoint(5.1f, 7371, 30),
    EngRevDataPoint(5.4f, 9029, 100),
    EngRevDataPoint(5.7f, 9892, 100),
    EngRevDataPoint(6.0f, 9783, 70),
    EngRevDataPoint(6.3f, 9170, 0),
    EngRevDataPoint(6.6f, 8492, 0),
    EngRevDataPoint(6.9f, 7793, 0),
    EngRevDataPoint(7.2f, 7077, 0),
    EngRevDataPoint(7.5f, 6360, 0),
    EngRevDataPoint(7.8f, 5646, 0),
    EngRevDataPoint(8.1f, 4927, 0),
    EngRevDataPoint(8.4f, 4209, 0),
    EngRevDataPoint(8.7f, 3496, 0),
    EngRevDataPoint(9.0f, 2776, 0)
};

EngRevDataPoint RevPat12[] = {
    EngRevDataPoint(0.0f, 1220, 30),
    EngRevDataPoint(0.3f, 2300, 100),
    EngRevDataPoint(0.6f, 3634, 100),
    EngRevDataPoint(0.9f, 5163, 100),
    EngRevDataPoint(1.2f, 6849, 100),
    EngRevDataPoint(1.5f, 7537, 42),
    EngRevDataPoint(1.8f, 7288, 21),
    EngRevDataPoint(2.1f, 6711, 0),
    EngRevDataPoint(2.4f, 5991, 0),
    EngRevDataPoint(2.7f, 6775, 83),
    EngRevDataPoint(3.0f, 8390, 90),
    EngRevDataPoint(3.3f, 8085, 3),
    EngRevDataPoint(3.6f, 8402, 60),
    EngRevDataPoint(3.9f, 9433, 70),
    EngRevDataPoint(4.2f, 9375, 30),
    EngRevDataPoint(4.5f, 9824, 70),
    EngRevDataPoint(4.8f, 9844, 30),
    EngRevDataPoint(5.1f, 9837, 70),
    EngRevDataPoint(5.4f, 9919, 70),
    EngRevDataPoint(5.7f, 9644, 40),
    EngRevDataPoint(6.0f, 9840, 60),
    EngRevDataPoint(6.3f, 9784, 70),
    EngRevDataPoint(6.6f, 9172, 0),
    EngRevDataPoint(6.9f, 8492, 0),
    EngRevDataPoint(7.2f, 7795, 0),
    EngRevDataPoint(7.5f, 7079, 0),
    EngRevDataPoint(7.8f, 6360, 0),
    EngRevDataPoint(8.1f, 5649, 0),
    EngRevDataPoint(8.4f, 4927, 0),
    EngRevDataPoint(8.7f, 4210, 0)
};

void *NIS_RevManager::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}

void SFXCTL_Physics::MsgRevOff(const MAIEngineRev &message) {
    (void)message;
    eCurNisRevingState = NIS_OFF;
    pRevData = nullptr;
}


NIS_RevManager::NIS_RevManager() {
    g_pNISRevMgr = this;
    pRevData = nullptr;
    pBuffer = nullptr;
    RecordingCount = 0;
    IsInitialized = false;
}

NIS_RevManager::~NIS_RevManager() {
    g_pNISRevMgr = nullptr;
    delete pRevData;
    delete pBuffer;
    RecordingCount = 0;
}

void NIS_RevManager::OpenNISRevData(unsigned int anim_id) {
    int index;
    char *AnimName;
    char EngineRevBin[50];

    IsInitialized = false;
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
                    ::operator delete(pRevData);
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

                IsInitialized = true;
            }
        } else {
            IsInitialized = false;
        }
    }
}

void NIS_RevManager::StartNISReving() {}

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
    if (INIS::Get() && INIS::Get()->IsPlaying()) {
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
