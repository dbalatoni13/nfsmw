#include "./STICH_Playback.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#include <algorithm>

namespace Csis {
extern ClassHandle gAEMS_StichCollisionHandle;
extern InterfaceId AEMS_StichCollisionId;
extern ClassHandle gAEMS_StichWooshHandle;
extern InterfaceId AEMS_StichWooshId;
extern ClassHandle gAEMS_StichStaticHandle;
extern InterfaceId AEMS_StichStaticId;
} // namespace Csis

void *cSTICH_PlayBack::operator new(unsigned int size, const char *debug_name) {
    return gAudioMemoryManager.AllocateMemory(size, debug_name, false);
}

struct AEMS_StichData {
    int type;
    int iD;
    int vol;
    int pitch;
    int az;
    int offset;
    int filter_DryFX;
    int filter_WetFX;
    int filter_LoPass;
    int filter_HiPass;
};

struct AEMS_StichCollision {
    Csis::Class *mpClass;
    AEMS_StichData mData;

    AEMS_StichCollision(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX,
                        int filter_LoPass, int filter_HiPass);
    ~AEMS_StichCollision();
    void SetAz(int x);
    void SetVol(int x);
    void SetPitch(int x);
    void SetFilter_WetFX(int x);
    void CommitMemberData();
    int GetRefCount();
};

struct AEMS_StichWoosh {
    Csis::Class *mpClass;
    AEMS_StichData mData;

    AEMS_StichWoosh(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX,
                    int filter_LoPass, int filter_HiPass);
    ~AEMS_StichWoosh();
    void SetAz(int x);
    void SetVol(int x);
    void SetPitch(int x);
    void SetFilter_WetFX(int x);
    void CommitMemberData();
    int GetRefCount();
};

struct AEMS_StichStatic {
    Csis::Class *mpClass;
    AEMS_StichData mData;

    AEMS_StichStatic(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX,
                     int filter_LoPass, int filter_HiPass);
    ~AEMS_StichStatic();
    void SetAz(int x);
    void SetVol(int x);
    void SetPitch(int x);
    void SetFilter_WetFX(int x);
    void CommitMemberData();
    int GetRefCount();
};

enum eSTITCH_PLAY_STATUS {
    eSTITCH_PLAY_STATUS_OFF = 0,
    eSTITCH_PLAY_STATUS_QUEUED = 1,
    eSTITCH_PLAY_STATUS_PLAYING = 2,
};

typedef UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE> cSampleListSet;

static int ClampStichValue(int value, int low, int high) {
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static int NormalizeStichAzimuth(int az) {
    int normalized = az % 0x10000;
    if (normalized < 0) {
        normalized = 0;
    } else if (normalized > 0x10000) {
        normalized = 0x10000;
    }
    return normalized;
}

static inline void AddToSampleList(cSampleWarpper *sample, STICH_TYPE to) {
    cSampleListSet::List &samplelist = const_cast<cSampleListSet::List &>(cSampleListSet::GetList(to));
    samplelist.push_back(sample);
}

static inline void RemoveFromSampleList(cSampleWarpper *sample, STICH_TYPE from) {
    cSampleListSet::List &samplelist = const_cast<cSampleListSet::List &>(cSampleListSet::GetList(from));
    cSampleListSet::List::iterator newend = std::remove(samplelist.begin(), samplelist.end(), sample);
    if (newend != samplelist.end()) {
        samplelist.erase(newend, samplelist.end());
    }
}

cSTICH_PlayBack::cSTICH_PlayBack() {
    mSampleRefSlotPool = bNewSlotPool(32, 129, "SampleWrapper SlotPool", AudioMemoryPool);
    mSampleRefSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    mSampleRefSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);

    mStitchSlotPool = bNewSlotPool(108, 45, "Stitch SlotPool", AudioMemoryPool);
    mStitchSlotPool->SetFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    mStitchSlotPool->SetFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
}

cSTICH_PlayBack::~cSTICH_PlayBack() {
    if (mSampleRefSlotPool) {
        bDeleteSlotPool(mSampleRefSlotPool);
    }
    if (mStitchSlotPool) {
        bDeleteSlotPool(mStitchSlotPool);
    }
}

void cSTICH_PlayBack::QueueSampleRequest(struct SampleQueueItem &samplereq) {
    STICH_TYPE type = static_cast<STICH_TYPE>(samplereq.pStitch->StichData->eStichType);
    GetQueueList(type).push_back(samplereq);
}

void cSTICH_PlayBack::RemoveFromList(struct SampleQueueItem sampleitem) {
    STICH_TYPE type = static_cast<STICH_TYPE>(sampleitem.pStitch->StichData->eStichType);
    for (SampleQueueItem *iter = mQueuedSampleList[type].begin();
         iter != mQueuedSampleList[type].end(); ++iter) {
        SampleQueueItem compareto = *iter;
        if (compareto == sampleitem) {
            GetQueueList(type).erase(iter);
            break;
        }
    }
    sampleitem.pSample->Destroy();
}

// UNSOLVED
int cSTICH_PlayBack::Prune(STICH_TYPE type, int priority, int num_to_clear) {
    if (num_to_clear == 0) {
        return 0;
    }

    int num_pruned = 0;
    UTL::Std::list<SampleQueueItem, _type_list> destroyed;

    for (SampleQueueItem *iter = GetQueueList(type).begin(); iter != GetQueueList(type).end();
         ++iter) {
        SampleQueueItem sampleitem = *iter;
        if (static_cast<int>(sampleitem.pSample->GetData().Priority) <= priority) {
            destroyed.push_back(sampleitem);
            num_pruned++;
        }
        if (num_to_clear == num_pruned) {
            break;
        }
    }

    std::for_each(destroyed.begin(), destroyed.end(), RemoveFromList);

    return num_pruned;
}

bool cSTICH_PlayBack::AddStich(STICH_TYPE StichType, SND_Stich &NewStichData) {
    StichList[StichType].AddTail(&NewStichData);
    return true;
}

bPList<SND_Stich> &cSTICH_PlayBack::GetStichList(STICH_TYPE StichType) {
    return StichList[StichType];
}

SND_Stich &cSTICH_PlayBack::GetStich(STICH_TYPE StichType, int Index) {
    return *static_cast<SND_Stich *>(GetStichList(StichType).GetNode(Index)->GetpObject());
}

void cSTICH_PlayBack::Update(float t) {
    for (int n = 0; n < MAX_NUM_STICH_TYPE; n++) {
        while (mQueuedSampleList[n].size() != 0) {
            SampleQueueItem samplereq = mQueuedSampleList[n][mQueuedSampleList[n].size() - 1];
            mQueuedSampleList[n].pop_back();
            samplereq.pSample->Play(&samplereq.pStitch->SndParams);
        }
    }
}

void cSTICH_PlayBack::DestroyAllStichs(void) {
    for (int i = 0; i < MAX_NUM_STICH_TYPE; i++) {
        GetQueueList(static_cast<STICH_TYPE>(i)).clear();
        while (!StichList[i].IsEmpty()) {
            StichList[i].RemoveHead();
        }
    }
}

char *GetStichTypeName(STICH_TYPE CurType) {
    return "";
}

void KillSample(cSampleWarpper *sampleref) {
    sampleref->Destroy();
}

cSampleWarpper::cSampleWarpper(SND_SampleRef &NewRef) {
    SampleRefData = &NewRef;
    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
    AEMS_ActiveSampleWsh = nullptr;
    AEMS_ActiveSampleCol = nullptr;
    AEMS_ActiveSampleStatic = nullptr;
    m_nLocalVolume = 0;
    m_nLocalPitch = 0;
}

void cSampleWarpper::Initialize() {
    m_eIsPlaying = eSTITCH_PLAY_STATUS_QUEUED;
}

void *cSampleWarpper::operator new(unsigned int obj_size) {
    if (cSTICH_PlayBack::mSampleRefSlotPool == nullptr || cSTICH_PlayBack::mSampleRefSlotPool->IsFull()) {
        return nullptr;
    }
    return cSTICH_PlayBack::mSampleRefSlotPool->Malloc(1, nullptr);
}

void cSampleWarpper::operator delete(void *ptr) {
    if (cSTICH_PlayBack::mSampleRefSlotPool != nullptr && ptr != nullptr) {
        cSTICH_PlayBack::mSampleRefSlotPool->Free(ptr);
    }
}

cSampleWarpper::~cSampleWarpper() {
    Destroy();
    for (int i = 0; i < MAX_NUM_STICH_TYPE; i++) {
        RemoveFromSampleList(this, static_cast<STICH_TYPE>(i));
    }
}

void cSampleWarpper::Destroy() {
    if (AEMS_ActiveSampleWsh != nullptr) {
        delete AEMS_ActiveSampleWsh;
    }
    AEMS_ActiveSampleWsh = nullptr;

    if (AEMS_ActiveSampleCol != nullptr) {
        delete AEMS_ActiveSampleCol;
    }
    AEMS_ActiveSampleCol = nullptr;

    if (AEMS_ActiveSampleStatic != nullptr) {
        delete AEMS_ActiveSampleStatic;
    }
    AEMS_ActiveSampleStatic = nullptr;

    if (m_eIsPlaying == eSTITCH_PLAY_STATUS_PLAYING) {
        RemoveFromSampleList(this, static_cast<STICH_TYPE>(SampleRefData->eStichType));
    }

    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
}

void cSampleWarpper::Update(const SND_Params *Params) {
    int TempPitch = m_nLocalPitch;
    int TempVol = m_nLocalVolume * Params->Vol >> 0xF;
    int refCountWsh;
    int refCountCol;
    int refCountStatic;

    if (AEMS_ActiveSampleWsh != nullptr) {
        int az = static_cast<unsigned short>(SampleRefData->Az) + 0x10000 + Params->Az;
        int azWrap = az;
        if (az < 0) {
            azWrap = az + 0xFFFF;
        }
        az += (azWrap >> 16) * -0x10000;
        if (az < 0) {
            az = 0;
        } else if (az > 0x10000) {
            az = 0x10000;
        }
        AEMS_ActiveSampleWsh->mData.az = az;

        int vol = TempVol;
        if (TempVol < 0) {
            vol = 0;
        } else if (TempVol > 0x7FFF) {
            vol = 0x7FFF;
        }
        AEMS_ActiveSampleWsh->mData.vol = vol;

        int pitch = TempPitch;
        if (TempPitch < 0) {
            pitch = 0;
        } else if (TempPitch > 0x2000) {
            pitch = 0x2000;
        }
        AEMS_ActiveSampleWsh->mData.pitch = pitch;

        int wetFX = Params->RVerb;
        if (wetFX < 0) {
            wetFX = 0;
        } else if (wetFX > 0x7FFF) {
            wetFX = 0x7FFF;
        }
        AEMS_ActiveSampleWsh->mData.filter_WetFX = wetFX;

        Csis::Class *cls = AEMS_ActiveSampleWsh->mpClass;
        if (cls != nullptr) {
            cls->SetMemberData(&AEMS_ActiveSampleWsh->mData);
        }
        refCountWsh = 0;
        cls = AEMS_ActiveSampleWsh->mpClass;
        if (cls != nullptr) {
            cls->GetRefCount(&refCountWsh);
        }
        if (refCountWsh < 2) {
            Destroy();
        }
    }

    if (AEMS_ActiveSampleCol != nullptr) {
        int az = static_cast<unsigned short>(SampleRefData->Az) + 0x10000 + Params->Az;
        int azWrap = az;
        if (az < 0) {
            azWrap = az + 0xFFFF;
        }
        az += (azWrap >> 16) * -0x10000;
        if (az < 0) {
            az = 0;
        } else if (az > 0x10000) {
            az = 0x10000;
        }
        AEMS_ActiveSampleCol->mData.az = az;

        int vol = TempVol;
        if (TempVol < 0) {
            vol = 0;
        } else if (TempVol > 0x7FFF) {
            vol = 0x7FFF;
        }
        AEMS_ActiveSampleCol->mData.vol = vol;

        int pitch = TempPitch;
        if (TempPitch < 0) {
            pitch = 0;
        } else if (TempPitch > 0x2000) {
            pitch = 0x2000;
        }
        AEMS_ActiveSampleCol->mData.pitch = pitch;

        int wetFX = Params->RVerb;
        if (wetFX < 0) {
            wetFX = 0;
        } else if (wetFX > 0x7FFF) {
            wetFX = 0x7FFF;
        }
        AEMS_ActiveSampleCol->mData.filter_WetFX = wetFX;

        Csis::Class *cls = AEMS_ActiveSampleCol->mpClass;
        if (cls != nullptr) {
            cls->SetMemberData(&AEMS_ActiveSampleCol->mData);
        }
        refCountCol = 0;
        cls = AEMS_ActiveSampleCol->mpClass;
        if (cls != nullptr) {
            cls->GetRefCount(&refCountCol);
        }
        if (refCountCol < 2) {
            Destroy();
        }
    }

    if (AEMS_ActiveSampleStatic != nullptr) {
        int az = static_cast<unsigned short>(SampleRefData->Az) + 0x10000 + Params->Az;
        int azWrap = az;
        if (az < 0) {
            azWrap = az + 0xFFFF;
        }
        az += (azWrap >> 16) * -0x10000;
        if (az < 0) {
            az = 0;
        } else if (az > 0x10000) {
            az = 0x10000;
        }
        AEMS_ActiveSampleStatic->mData.az = az;

        if (TempVol < 0) {
            TempVol = 0;
        } else if (TempVol > 0x7FFF) {
            TempVol = 0x7FFF;
        }
        AEMS_ActiveSampleStatic->mData.vol = TempVol;

        if (TempPitch < 0) {
            TempPitch = 0;
        } else if (TempPitch > 0x2000) {
            TempPitch = 0x2000;
        }
        AEMS_ActiveSampleStatic->mData.pitch = TempPitch;

        int wetFX = Params->RVerb;
        if (wetFX < 0) {
            wetFX = 0;
        } else if (wetFX > 0x7FFF) {
            wetFX = 0x7FFF;
        }
        AEMS_ActiveSampleStatic->mData.filter_WetFX = wetFX;

        Csis::Class *cls = AEMS_ActiveSampleStatic->mpClass;
        if (cls != nullptr) {
            cls->SetMemberData(&AEMS_ActiveSampleStatic->mData);
        }
        refCountStatic = 0;
        cls = AEMS_ActiveSampleStatic->mpClass;
        if (cls != nullptr) {
            cls->GetRefCount(&refCountStatic);
        }
        if (refCountStatic < 2) {
            Destroy();
        }
    }
}

void cSampleWarpper::Play(const SND_Params *Params) {
    if (AEMS_ActiveSampleCol != nullptr || AEMS_ActiveSampleWsh != nullptr || AEMS_ActiveSampleStatic != nullptr) {
        return;
    }

    STICH_TYPE stichType = static_cast<STICH_TYPE>(GetData().eStichType);
    AddToSampleList(this, stichType);

    if (GetData().RND_Vol != 0) {
        m_nLocalVolume = static_cast<int>(GetData().Volume - g_pEAXSound->Random(GetData().RND_Vol));
    } else {
        m_nLocalVolume = GetData().Volume;
    }

    int TempVol = m_nLocalVolume * Params->Vol >> 0xF;
    int randomPitchRange = GetData().RND_Pitch;
    if (randomPitchRange != 0) {
        if (randomPitchRange < 0) {
            randomPitchRange = -randomPitchRange;
        }
        m_nLocalPitch = static_cast<int>(GetData().Pitch - g_pEAXSound->Random(randomPitchRange));
    } else {
        m_nLocalPitch = GetData().Pitch;
    }

    float PitchScale = static_cast<float>(0x1000 - Params->Pitch) * (1.0f / 4096.0f);
    int TempPitch = static_cast<int>(static_cast<float>(m_nLocalPitch) * PitchScale);
    int TempAz = Params->Az;
    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;

    if (GetData().eStichType == STICH_TYPE_COLLISION) {
        g_pEAXSound->SetCsisName(GetStichTypeName(STICH_TYPE_COLLISION));
        AEMS_StichCollision **activeSample = &AEMS_ActiveSampleCol;
        AEMS_StichCollision *sample = static_cast<AEMS_StichCollision *>(Csis::System::Alloc(0x2C));

        SND_SampleRef *ref = SampleRefData;
        int sampleType = static_cast<int>(ref->eStichType);
        int sampleIndex = static_cast<int>(ref->SampleIndex);
        int sampleAz = (TempAz + static_cast<unsigned short>(ref->Az) + 0x10000) % 0x10000;
        int sampleOffset = static_cast<int>(ref->Offset);
        int wetFX = Params->RVerb;

        int type = sampleType;
        if (type < 0) {
            type = 0;
        } else if (type > 0x10) {
            type = 0x10;
        }
        sample->mData.type = type;

        int id = sampleIndex;
        if (id > 0x3FF) {
            id = 0x3FF;
        }
        sample->mData.iD = id;

        int vol = TempVol;
        if (vol < 0) {
            vol = 0;
        } else if (vol > 0x7FFF) {
            vol = 0x7FFF;
        }
        sample->mData.vol = vol;

        int pitch = TempPitch;
        if (pitch < 0) {
            pitch = 0;
        } else if (pitch > 0x2000) {
            pitch = 0x2000;
        }
        sample->mData.pitch = pitch;

        int az = sampleAz;
        if (az < 0) {
            az = 0;
        } else if (az > 0x10000) {
            az = 0x10000;
        }
        sample->mData.az = az;

        int offset = sampleOffset;
        if (offset > 4000) {
            offset = 4000;
        }
        sample->mData.offset = offset;

        sample->mData.filter_DryFX = 0x7FFF;

        int wet = wetFX;
        if (wet < 0) {
            wet = 0;
        } else if (wet > 0x7FFF) {
            wet = 0x7FFF;
        }

        sample->mData.filter_WetFX = wet;
        sample->mData.filter_LoPass = 25000;
        sample->mData.filter_HiPass = 0;

        int createResult = Csis::Class::CreateInstance(&Csis::gAEMS_StichCollisionHandle, &sample->mData, &sample->mpClass);
        if (createResult < 0) {
            Csis::gAEMS_StichCollisionHandle.Set(&Csis::AEMS_StichCollisionId);
            Csis::Class::CreateInstance(&Csis::gAEMS_StichCollisionHandle, &sample->mData, &sample->mpClass);
        }

        *activeSample = sample;
        if (*activeSample != nullptr) {
            int refCount = 0;
            Csis::Class *cls = (*activeSample)->mpClass;
            if (cls != nullptr) {
                cls->GetRefCount(&refCount);
            }
            if (refCount < 3) {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
            } else {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_PLAYING;
            }
        }
    }

    if (GetData().eStichType == STICH_TYPE_WOOSH) {
        g_pEAXSound->SetCsisName(GetStichTypeName(STICH_TYPE_WOOSH));
        AEMS_StichWoosh **activeSample = &AEMS_ActiveSampleWsh;
        AEMS_StichWoosh *sample = static_cast<AEMS_StichWoosh *>(Csis::System::Alloc(0x2C));

        SND_SampleRef *ref = SampleRefData;
        int sampleType = static_cast<int>(ref->eStichType);
        int sampleIndex = static_cast<int>(ref->SampleIndex);
        int sampleAz = (TempAz + static_cast<unsigned short>(ref->Az) + 0x10000) % 0x10000;
        int sampleOffset = static_cast<int>(ref->Offset);
        int wetFX = Params->RVerb;

        int type = sampleType;
        if (type < 0) {
            type = 0;
        } else if (type > 0x10) {
            type = 0x10;
        }
        sample->mData.type = type;

        int id = sampleIndex;
        if (id > 0x3FF) {
            id = 0x3FF;
        }
        sample->mData.iD = id;

        int vol = TempVol;
        if (vol < 0) {
            vol = 0;
        } else if (vol > 0x7FFF) {
            vol = 0x7FFF;
        }
        sample->mData.vol = vol;

        int pitch = TempPitch;
        if (pitch < 0) {
            pitch = 0;
        } else if (pitch > 0x2000) {
            pitch = 0x2000;
        }
        sample->mData.pitch = pitch;

        int az = sampleAz;
        if (az < 0) {
            az = 0;
        } else if (az > 0x10000) {
            az = 0x10000;
        }
        sample->mData.az = az;

        int offset = sampleOffset;
        if (offset > 4000) {
            offset = 4000;
        }
        sample->mData.offset = offset;

        sample->mData.filter_DryFX = 0x7FFF;

        int wet = wetFX;
        if (wet < 0) {
            wet = 0;
        } else if (wet > 0x7FFF) {
            wet = 0x7FFF;
        }

        sample->mData.filter_WetFX = wet;
        sample->mData.filter_LoPass = 25000;
        sample->mData.filter_HiPass = 0;

        int createResult = Csis::Class::CreateInstance(&Csis::gAEMS_StichWooshHandle, &sample->mData, &sample->mpClass);
        if (createResult < 0) {
            Csis::gAEMS_StichWooshHandle.Set(&Csis::AEMS_StichWooshId);
            Csis::Class::CreateInstance(&Csis::gAEMS_StichWooshHandle, &sample->mData, &sample->mpClass);
        }

        *activeSample = sample;
        if (*activeSample != nullptr) {
            int refCount = 0;
            Csis::Class *cls = (*activeSample)->mpClass;
            if (cls != nullptr) {
                cls->GetRefCount(&refCount);
            }
            if (refCount < 3) {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
            } else {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_PLAYING;
            }
        }
    }

    if (GetData().eStichType == STICH_TYPE_STATIC) {
        g_pEAXSound->SetCsisName(GetStichTypeName(STICH_TYPE_STATIC));
        AEMS_StichStatic **activeSample = &AEMS_ActiveSampleStatic;
        AEMS_StichStatic *sample = static_cast<AEMS_StichStatic *>(Csis::System::Alloc(0x2C));

        SND_SampleRef *ref = SampleRefData;
        int sampleType = static_cast<int>(ref->eStichType);
        int sampleIndex = static_cast<int>(ref->SampleIndex);
        int sampleAz = (TempAz + static_cast<unsigned short>(ref->Az) + 0x10000) % 0x10000;
        int sampleOffset = static_cast<int>(ref->Offset);
        int wetFX = Params->RVerb;

        int type = sampleType;
        if (type < 0) {
            type = 0;
        } else if (type > 0x10) {
            type = 0x10;
        }
        sample->mData.type = type;

        int id = sampleIndex;
        if (id > 0x3FF) {
            id = 0x3FF;
        }
        sample->mData.iD = id;

        int vol = TempVol;
        if (vol < 0) {
            vol = 0;
        } else if (vol > 0x7FFF) {
            vol = 0x7FFF;
        }
        sample->mData.vol = vol;

        int pitch = TempPitch;
        if (pitch < 0) {
            pitch = 0;
        } else if (pitch > 0x2000) {
            pitch = 0x2000;
        }
        sample->mData.pitch = pitch;

        int az = sampleAz;
        if (az < 0) {
            az = 0;
        } else if (az > 0x10000) {
            az = 0x10000;
        }
        sample->mData.az = az;

        int offset = sampleOffset;
        if (offset > 4000) {
            offset = 4000;
        }
        sample->mData.offset = offset;

        sample->mData.filter_DryFX = 0x7FFF;

        int wet = wetFX;
        if (wet < 0) {
            wet = 0;
        } else if (wet > 0x7FFF) {
            wet = 0x7FFF;
        }

        sample->mData.filter_WetFX = wet;
        sample->mData.filter_LoPass = 25000;
        sample->mData.filter_HiPass = 0;

        int createResult = Csis::Class::CreateInstance(&Csis::gAEMS_StichStaticHandle, &sample->mData, &sample->mpClass);
        if (createResult < 0) {
            Csis::gAEMS_StichStaticHandle.Set(&Csis::AEMS_StichStaticId);
            Csis::Class::CreateInstance(&Csis::gAEMS_StichStaticHandle, &sample->mData, &sample->mpClass);
        }

        *activeSample = sample;
        if (*activeSample != nullptr) {
            int refCount = 0;
            Csis::Class *cls = (*activeSample)->mpClass;
            if (cls != nullptr) {
                cls->GetRefCount(&refCount);
            }
            if (refCount < 3) {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
            } else {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_PLAYING;
            }
        }
    }
}

int cSampleWarpper::Prune(STICH_TYPE type, int priority, int num_to_clear) {
    if (num_to_clear == 0) {
        return 0;
    }

    int num_pruned = 0;
    typedef UTL::Std::list<cSampleWarpper *, _type_list> Destroyed;
    Destroyed destroyed;
    cSampleListSet::List &samplelist = const_cast<cSampleListSet::List &>(cSampleListSet::GetList(type));

    for (cSampleWarpper **iter = samplelist.begin(); iter != samplelist.end(); ++iter) {
        cSampleWarpper *sampleref = *iter;
        if ((sampleref->m_eIsPlaying != eSTITCH_PLAY_STATUS_OFF) &&
            (static_cast<int>(sampleref->GetData().Priority) <= priority)) {
            destroyed.push_back(sampleref);
            num_pruned++;
        }
        if (num_to_clear == num_pruned) {
            break;
        }
    }

    std::for_each(destroyed.begin(), destroyed.end(), KillSample);

    return num_pruned;
}

cStichWrapper::cStichWrapper(const SND_Stich &NewStichData)
    : SndParams() //
{
    StichData = const_cast<SND_Stich *>(&NewStichData);
    for (int i = 0; i < 18; i++) {
        ActiveSamplesRefs[i] = nullptr;
    }
}

cStichWrapper::~cStichWrapper() {
    Destroy();
}

void *cStichWrapper::operator new(unsigned int obj_size) {
    return cSTICH_PlayBack::mStitchSlotPool->Malloc(1, nullptr);
}

void cStichWrapper::operator delete(void *ptr) {
    if (cSTICH_PlayBack::mStitchSlotPool != nullptr && ptr != nullptr) {
        cSTICH_PlayBack::mStitchSlotPool->Free(ptr);
    }
}

void cStichWrapper::Play(int Vol, int Pitch, int Azimuth) {
    SndParams.Vol = Vol;
    SndParams.Az = Azimuth;
    SndParams.Pitch = Pitch;
    Play(&SndParams);
}

void cStichWrapper::Play(const SND_Params *Params) {
    STICH_TYPE stitch_type = static_cast<STICH_TYPE>(GetData().eStichType);

    if (Params != nullptr) {
        SndParams = *Params;
        SndParams.Vol = SndParams.Vol * 0x7FFF >> 0xF;
    }

    for (int i = 0; i < static_cast<int>(GetData().Num_SampleRefs); i++) {
        cSampleWarpper *sample = new cSampleWarpper(GetData().pSampleRefList[i]);
        ActiveSamplesRefs[i] = sample;

        if (sample != nullptr) {
            sample->Initialize();

            SampleQueueItem samplereq;
            samplereq.pSample = ActiveSamplesRefs[i];
            samplereq.pStitch = this;
            cSTICH_PlayBack::QueueSampleRequest(samplereq);
        }
    }

    bIsPlaying = true;

    int num_to_prune = cSTICH_PlayBack::mQueuedSampleList[stitch_type].size() + cSampleListSet::GetList(stitch_type).size() - 25;
    if (num_to_prune > 0) {
        bool continueloop = true;
        int priority_to_prune = 0;
        do {
            num_to_prune -= cSTICH_PlayBack::Prune(stitch_type, priority_to_prune, num_to_prune);
            num_to_prune -= cSampleWarpper::Prune(stitch_type, priority_to_prune, num_to_prune);
            if (num_to_prune != 0 && priority_to_prune != 10) {
                priority_to_prune++;
            } else {
                continueloop = false;
            }
        } while (continueloop);
    }
}

void cStichWrapper::Update(const SND_Params *Params) {
    if (!bIsPlaying) {
        return;
    }

    if (Params != nullptr) {
        SndParams = *Params;
    }

    bIsPlaying = false;
    for (int i = 0; ; i++) {
        if (i >= static_cast<int>(GetData().Num_SampleRefs)) break;
        if (ActiveSamplesRefs[i] != nullptr) {
            bool playing = true;
            ActiveSamplesRefs[i]->Update(&SndParams);
            playing = ActiveSamplesRefs[i]->IsPlaying();
            if (!playing) {
                delete ActiveSamplesRefs[i];
                ActiveSamplesRefs[i] = nullptr;
            } else {
                bIsPlaying = true;
            }
        }
    }
}

void cStichWrapper::Destroy() {
    for (int i = 0; i < 18; i++) {
        cSampleWarpper *sample = ActiveSamplesRefs[i];
        if (sample != nullptr) {
            if (sample->m_eIsPlaying == eSTITCH_PLAY_STATUS_QUEUED) {
                SampleQueueItem sampleitem;
                sampleitem.pSample = sample;
                sampleitem.pStitch = this;
                cSTICH_PlayBack::RemoveFromList(sampleitem);
            }

            if (ActiveSamplesRefs[i] != nullptr) {
                delete ActiveSamplesRefs[i];
            }
            ActiveSamplesRefs[i] = nullptr;
        }
    }

    bIsPlaying = false;
}

template <>
UTL::FixedVector<SampleQueueItem, 43, 16> cSTICH_PlayBack::mQueuedSampleList[3] =
    {UTL::FixedVector<SampleQueueItem, 43, 16>(), UTL::FixedVector<SampleQueueItem, 43, 16>(),
     UTL::FixedVector<SampleQueueItem, 43, 16>()};

template <>
UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_ListSet
    UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_mLists =
        UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_ListSet();

template class UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::List;
