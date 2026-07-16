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
    int type;           // offset 0x0, size 0x4
    int iD;             // offset 0x4, size 0x4
    int vol;            // offset 0x8, size 0x4
    int pitch;          // offset 0xC, size 0x4
    int az;             // offset 0x10, size 0x4
    int offset;         // offset 0x14, size 0x4
    int filter_DryFX;   // offset 0x18, size 0x4
    int filter_WetFX;   // offset 0x1C, size 0x4
    int filter_LoPass;  // offset 0x20, size 0x4
    int filter_HiPass;  // offset 0x24, size 0x4
};

struct AEMS_StichCollision {
    Csis::Class *mpClass;   // offset 0x0, size 0x4
    AEMS_StichData mData;   // offset 0x4, size 0x28

    void SetType(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10) {
            x = 0x10;
        }
        mData.type = x;
    }

    void SetID(int x) {
        if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.iD = x;
    }

    void SetVol(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vol = x;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        mData.pitch = x;
    }

    void SetAz(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        mData.az = x;
    }

    void SetOffset(int x) {
        if (x > 4000) {
            x = 4000;
        }
        mData.offset = x;
    }

    void SetFilter_DryFX(int x) {
        mData.filter_DryFX = x;
    }

    void SetFilter_WetFX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.filter_WetFX = x;
    }

    void SetFilter_LoPass(int x) {
        mData.filter_LoPass = x;
    }

    void SetFilter_HiPass(int x) {
        mData.filter_HiPass = x;
    }

    int GetRefCount() {
        int refCount = 0;

        if (mpClass) {
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    AEMS_StichCollision(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX,
                        int filter_LoPass, int filter_HiPass) {
        SetType(type);
        SetID(iD);
        SetVol(vol);
        SetPitch(pitch);
        SetAz(az);
        SetOffset(offset);
        SetFilter_DryFX(filter_DryFX);
        SetFilter_WetFX(filter_WetFX);
        SetFilter_LoPass(filter_LoPass);
        SetFilter_HiPass(filter_HiPass);

        Csis::System::Result result = static_cast<Csis::System::Result>(
            Csis::Class::CreateInstance(&Csis::gAEMS_StichCollisionHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gAEMS_StichCollisionHandle.Set(&Csis::AEMS_StichCollisionId);
            Csis::Class::CreateInstance(&Csis::gAEMS_StichCollisionHandle, &mData, &mpClass);
        }
    }

    ~AEMS_StichCollision() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }
};

struct AEMS_StichWoosh {
    Csis::Class *mpClass;   // offset 0x0, size 0x4
    AEMS_StichData mData;   // offset 0x4, size 0x28

    void SetType(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10) {
            x = 0x10;
        }
        mData.type = x;
    }

    void SetID(int x) {
        if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.iD = x;
    }

    void SetVol(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vol = x;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        mData.pitch = x;
    }

    void SetAz(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        mData.az = x;
    }

    void SetOffset(int x) {
        if (x > 4000) {
            x = 4000;
        }
        mData.offset = x;
    }

    void SetFilter_DryFX(int x) {
        mData.filter_DryFX = x;
    }

    void SetFilter_WetFX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.filter_WetFX = x;
    }

    void SetFilter_LoPass(int x) {
        mData.filter_LoPass = x;
    }

    void SetFilter_HiPass(int x) {
        mData.filter_HiPass = x;
    }

    int GetRefCount() {
        int refCount = 0;

        if (mpClass) {
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    AEMS_StichWoosh(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX,
                    int filter_LoPass, int filter_HiPass) {
        SetType(type);
        SetID(iD);
        SetVol(vol);
        SetPitch(pitch);
        SetAz(az);
        SetOffset(offset);
        SetFilter_DryFX(filter_DryFX);
        SetFilter_WetFX(filter_WetFX);
        SetFilter_LoPass(filter_LoPass);
        SetFilter_HiPass(filter_HiPass);

        Csis::System::Result result = static_cast<Csis::System::Result>(
            Csis::Class::CreateInstance(&Csis::gAEMS_StichWooshHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gAEMS_StichWooshHandle.Set(&Csis::AEMS_StichWooshId);
            Csis::Class::CreateInstance(&Csis::gAEMS_StichWooshHandle, &mData, &mpClass);
        }
    }

    ~AEMS_StichWoosh() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }
};

struct AEMS_StichStatic {
    Csis::Class *mpClass;   // offset 0x0, size 0x4
    AEMS_StichData mData;   // offset 0x4, size 0x28

    void SetType(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10) {
            x = 0x10;
        }
        mData.type = x;
    }

    void SetID(int x) {
        if (x > 0x3FF) {
            x = 0x3FF;
        }
        mData.iD = x;
    }

    void SetVol(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.vol = x;
    }

    void SetPitch(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x2000) {
            x = 0x2000;
        }
        mData.pitch = x;
    }

    void SetAz(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        mData.az = x;
    }

    void SetOffset(int x) {
        if (x > 4000) {
            x = 4000;
        }
        mData.offset = x;
    }

    void SetFilter_DryFX(int x) {
        mData.filter_DryFX = x;
    }

    void SetFilter_WetFX(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.filter_WetFX = x;
    }

    void SetFilter_LoPass(int x) {
        mData.filter_LoPass = x;
    }

    void SetFilter_HiPass(int x) {
        mData.filter_HiPass = x;
    }

    int GetRefCount() {
        int refCount = 0;

        if (mpClass) {
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    AEMS_StichStatic(int type, int iD, int vol, int pitch, int az, int offset, int filter_DryFX, int filter_WetFX,
                     int filter_LoPass, int filter_HiPass) {
        SetType(type);
        SetID(iD);
        SetVol(vol);
        SetPitch(pitch);
        SetAz(az);
        SetOffset(offset);
        SetFilter_DryFX(filter_DryFX);
        SetFilter_WetFX(filter_WetFX);
        SetFilter_LoPass(filter_LoPass);
        SetFilter_HiPass(filter_HiPass);

        Csis::System::Result result = static_cast<Csis::System::Result>(
            Csis::Class::CreateInstance(&Csis::gAEMS_StichStaticHandle, &mData, &mpClass));
        if (result < Csis::System::kResult_Ok) {
            Csis::gAEMS_StichStaticHandle.Set(&Csis::AEMS_StichStaticId);
            Csis::Class::CreateInstance(&Csis::gAEMS_StichStaticHandle, &mData, &mpClass);
        }
    }

    ~AEMS_StichStatic() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }
};

static inline void AddToSampleList(cSampleWarpper *sample, STICH_TYPE to) {
    sample->AddToList(to);
}

static inline void RemoveFromSampleList(cSampleWarpper *sample, STICH_TYPE from) {
    sample->UnList(from);
}

char *GetStichTypeName(STICH_TYPE CurType) {
    return "";
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

void cSTICH_PlayBack::QueueSampleRequest(SampleQueueItem &samplereq) {
    STICH_TYPE type = static_cast<STICH_TYPE>(samplereq.pStitch->GetData().eStichType);
    GetQueueList(type).push_back(samplereq);
}

// UNSOLVED
static void KillSample(cSampleWarpper *sampleref) {
    sampleref->Destroy();
}

void cSTICH_PlayBack::RemoveFromList(SampleQueueItem sampleitem) {
    STICH_TYPE type = static_cast<STICH_TYPE>(sampleitem.pStitch->GetData().eStichType);
    for (SampleQueueItem *iter = GetQueueList(type).begin(); iter != GetQueueList(type).end(); ++iter) {
        SampleQueueItem compareto = *iter;
        if (compareto == sampleitem) {
            GetQueueList(type).erase(iter);
            break;
        }
    }
    sampleitem.pSample->Destroy();
}

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

SND_Stich &cSTICH_PlayBack::GetStich(STICH_TYPE StichType, int Index) {
    return *static_cast<SND_Stich *>(GetStichList(StichType).GetNode(Index)->GetpObject());
}

inline bPList<SND_Stich> &cSTICH_PlayBack::GetStichList(STICH_TYPE StichType) { return StichList[StichType]; }

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
    if (cSTICH_PlayBack::mStitchSlotPool && ptr) {
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

    if (Params) {
        SndParams = *Params;
        SndParams.Vol = SndParams.Vol * 0x7FFF >> 0xF;
    }

    for (int i = 0; i < static_cast<int>(GetData().Num_SampleRefs); i++) {
        cSampleWarpper *sample = new cSampleWarpper(GetData().pSampleRefList[i]);
        ActiveSamplesRefs[i] = sample;

        if (sample) {
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

    if (Params) {
        SndParams = *Params;
    }

    bIsPlaying = false;
    for (int i = 0; ; i++) {
        if (i >= static_cast<int>(GetData().Num_SampleRefs)) break;
        if (ActiveSamplesRefs[i]) {
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
        if (ActiveSamplesRefs[i]) {
            if (ActiveSamplesRefs[i]->m_eIsPlaying == eSTITCH_PLAY_STATUS_QUEUED) {
                SampleQueueItem sampleitem;
                sampleitem.pSample = ActiveSamplesRefs[i];
                sampleitem.pStitch = this;
                cSTICH_PlayBack::RemoveFromList(sampleitem);
            }

            if (ActiveSamplesRefs[i]) {
                delete ActiveSamplesRefs[i];
            }
            ActiveSamplesRefs[i] = nullptr;
        }
    }

    bIsPlaying = false;
}

cSampleWarpper::cSampleWarpper(SND_SampleRef &NewRef) {
    SampleRefData = &NewRef;
    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
    AEMS_ActiveSampleWsh = nullptr;
    AEMS_ActiveSampleCol = nullptr;
    AEMS_ActiveSampleStatic = nullptr;
}

cSampleWarpper::~cSampleWarpper() {
    Destroy();
}

void *cSampleWarpper::operator new(unsigned int obj_size) {
    if (!cSTICH_PlayBack::mSampleRefSlotPool || cSTICH_PlayBack::mSampleRefSlotPool->IsFull()) {
        return nullptr;
    }
    return cSTICH_PlayBack::mSampleRefSlotPool->Malloc(1, nullptr);
}

void cSampleWarpper::operator delete(void *ptr) {
    if (cSTICH_PlayBack::mSampleRefSlotPool && ptr) {
        cSTICH_PlayBack::mSampleRefSlotPool->Free(ptr);
    }
}

void cSampleWarpper::Destroy() {
    if (AEMS_ActiveSampleWsh) {
        delete AEMS_ActiveSampleWsh;
    }
    AEMS_ActiveSampleWsh = nullptr;

    if (AEMS_ActiveSampleCol) {
        delete AEMS_ActiveSampleCol;
    }
    AEMS_ActiveSampleCol = nullptr;

    if (AEMS_ActiveSampleStatic) {
        delete AEMS_ActiveSampleStatic;
    }
    AEMS_ActiveSampleStatic = nullptr;

    if (m_eIsPlaying == eSTITCH_PLAY_STATUS_PLAYING) {
        RemoveFromSampleList(this, static_cast<STICH_TYPE>(SampleRefData->eStichType));
    }

    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
}

void cSampleWarpper::Initialize() {
    m_eIsPlaying = eSTITCH_PLAY_STATUS_QUEUED;
}

void cSampleWarpper::Update(const SND_Params *Params) {
    int TempVol;
    int TempPitch;

    TempVol = m_nLocalVolume * Params->Vol >> 0xF;
    TempPitch = m_nLocalPitch;

    if (AEMS_ActiveSampleWsh) {
        AEMS_ActiveSampleWsh->SetAz(
            (static_cast<unsigned short>(GetData().Az) + 0x10000 + Params->Az) % 0x10000);
        AEMS_ActiveSampleWsh->SetVol(TempVol);
        AEMS_ActiveSampleWsh->SetPitch(TempPitch);
        AEMS_ActiveSampleWsh->SetFilter_WetFX(Params->RVerb);
        AEMS_ActiveSampleWsh->CommitMemberData();
        if (AEMS_ActiveSampleWsh->GetRefCount() < 2) {
            Destroy();
        }
    }

    if (AEMS_ActiveSampleCol) {
        AEMS_ActiveSampleCol->SetAz(
            (static_cast<unsigned short>(GetData().Az) + 0x10000 + Params->Az) % 0x10000);
        AEMS_ActiveSampleCol->SetVol(TempVol);
        AEMS_ActiveSampleCol->SetPitch(TempPitch);
        AEMS_ActiveSampleCol->SetFilter_WetFX(Params->RVerb);
        AEMS_ActiveSampleCol->CommitMemberData();
        if (AEMS_ActiveSampleCol->GetRefCount() < 2) {
            Destroy();
        }
    }

    if (AEMS_ActiveSampleStatic) {
        AEMS_ActiveSampleStatic->SetAz(
            (static_cast<unsigned short>(GetData().Az) + 0x10000 + Params->Az) % 0x10000);
        AEMS_ActiveSampleStatic->SetVol(TempVol);
        AEMS_ActiveSampleStatic->SetPitch(TempPitch);
        AEMS_ActiveSampleStatic->SetFilter_WetFX(Params->RVerb);
        AEMS_ActiveSampleStatic->CommitMemberData();
        if (AEMS_ActiveSampleStatic->GetRefCount() < 2) {
            Destroy();
        }
    }
}

void cSampleWarpper::Play(const SND_Params *Params) {
    if (AEMS_ActiveSampleCol || AEMS_ActiveSampleWsh || AEMS_ActiveSampleStatic) {
        return;
    }

    int TempVol;
    int TempPitch;
    int TempAz;
    float PitchScale;
    int RefCount;

    AddToList(static_cast<STICH_TYPE>(GetData().eStichType));

    if (GetData().RND_Vol != 0) {
        m_nLocalVolume = static_cast<int>(GetData().Volume - g_pEAXSound->Random(bAbs(GetData().RND_Vol)));
    } else {
        m_nLocalVolume = GetData().Volume;
    }

    TempVol = m_nLocalVolume * Params->Vol >> 0xF;
    if (GetData().RND_Pitch != 0) {
        m_nLocalPitch = static_cast<int>(GetData().Pitch - g_pEAXSound->Random(bAbs(GetData().RND_Pitch)));
    } else {
        m_nLocalPitch = GetData().Pitch;
    }

    PitchScale = static_cast<float>(0x1000 - Params->Pitch) * (1.0f / 4096.0f);
    TempPitch = static_cast<int>(static_cast<float>(m_nLocalPitch) * PitchScale);
    TempAz = Params->Az;
    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;

    if (GetData().eStichType == STICH_TYPE_COLLISION) {
        g_pEAXSound->SetCsisName(GetStichTypeName(static_cast<STICH_TYPE>(GetData().eStichType)));
        AEMS_ActiveSampleCol = new AEMS_StichCollision(
            GetData().eStichType, GetData().SampleIndex, TempVol, TempPitch,
            (TempAz + static_cast<unsigned short>(GetData().Az) + 0x10000) % 0x10000, GetData().Offset, 0x7FFF,
            Params->RVerb, 25000, 0);
        if (AEMS_ActiveSampleCol != nullptr) {
            RefCount = AEMS_ActiveSampleCol->GetRefCount();
            if (RefCount < 3) {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
            } else {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_PLAYING;
            }
        }
    }

    if (GetData().eStichType == STICH_TYPE_WOOSH) {
        g_pEAXSound->SetCsisName(GetStichTypeName(static_cast<STICH_TYPE>(GetData().eStichType)));
        AEMS_ActiveSampleWsh = new AEMS_StichWoosh(
            GetData().eStichType, GetData().SampleIndex, TempVol, TempPitch,
            (TempAz + static_cast<unsigned short>(GetData().Az) + 0x10000) % 0x10000, GetData().Offset, 0x7FFF,
            Params->RVerb, 25000, 0);
        if (AEMS_ActiveSampleWsh != nullptr) {
            RefCount = AEMS_ActiveSampleWsh->GetRefCount();
            if (RefCount < 3) {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;
            } else {
                m_eIsPlaying = eSTITCH_PLAY_STATUS_PLAYING;
            }
        }
    }

    if (GetData().eStichType == STICH_TYPE_STATIC) {
        g_pEAXSound->SetCsisName(GetStichTypeName(static_cast<STICH_TYPE>(GetData().eStichType)));
        AEMS_ActiveSampleStatic = new AEMS_StichStatic(
            GetData().eStichType, GetData().SampleIndex, TempVol, TempPitch,
            (TempAz + static_cast<unsigned short>(GetData().Az) + 0x10000) % 0x10000, GetData().Offset, 0x7FFF,
            Params->RVerb, 25000, 0);
        if (AEMS_ActiveSampleStatic != nullptr) {
            RefCount = AEMS_ActiveSampleStatic->GetRefCount();
            if (RefCount < 3) {
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
    const cSampleListSet::List &samplelist = cSampleListSet::GetList(type);
    Destroyed destroyed;

    for (cSampleWarpper *const *iter = samplelist.begin(); iter != samplelist.end(); ++iter) {
        cSampleWarpper *sampleref = *iter;
        if (sampleref->IsPlaying() && static_cast<int>(sampleref->GetData().Priority) <= priority) {
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

template <>
UTL::FixedVector<SampleQueueItem, 43, 16> cSTICH_PlayBack::mQueuedSampleList[3] =
    {UTL::FixedVector<SampleQueueItem, 43, 16>(), UTL::FixedVector<SampleQueueItem, 43, 16>(),
     UTL::FixedVector<SampleQueueItem, 43, 16>()};

template <>
UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_ListSet
    UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_mLists =
        UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_ListSet();

template class UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::List;
