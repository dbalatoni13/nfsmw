#include "./STICH_Playback.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#include <algorithm>

struct Attrib__Class;

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
    Attrib__Class *mpClass;
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
    Attrib__Class *mpClass;
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
    Attrib__Class *mpClass;
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
    return *reinterpret_cast<SND_Stich *>(GetStichList(StichType).GetNode(Index)->GetpObject());
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

    if (AEMS_ActiveSampleWsh != nullptr) {
        AEMS_ActiveSampleWsh->SetAz(NormalizeStichAzimuth(Params->Az + GetData().Az + 0x10000));
        AEMS_ActiveSampleWsh->SetVol(ClampStichValue(TempVol, 0, 0x7FFF));
        AEMS_ActiveSampleWsh->SetPitch(ClampStichValue(TempPitch, 0, 0x2000));
        AEMS_ActiveSampleWsh->SetFilter_WetFX(ClampStichValue(Params->RVerb, 0, 0x7FFF));
        AEMS_ActiveSampleWsh->CommitMemberData();
        if (AEMS_ActiveSampleWsh->GetRefCount() < 2) {
            Destroy();
        }
    }

    if (AEMS_ActiveSampleCol != nullptr) {
        AEMS_ActiveSampleCol->SetAz(NormalizeStichAzimuth(Params->Az + GetData().Az + 0x10000));
        AEMS_ActiveSampleCol->SetVol(ClampStichValue(TempVol, 0, 0x7FFF));
        AEMS_ActiveSampleCol->SetPitch(ClampStichValue(TempPitch, 0, 0x2000));
        AEMS_ActiveSampleCol->SetFilter_WetFX(ClampStichValue(Params->RVerb, 0, 0x7FFF));
        AEMS_ActiveSampleCol->CommitMemberData();
        if (AEMS_ActiveSampleCol->GetRefCount() < 2) {
            Destroy();
        }
    }

    if (AEMS_ActiveSampleStatic != nullptr) {
        int clampedVol = ClampStichValue(TempVol, 0, 0x7FFF);
        int clampedPitch = ClampStichValue(TempPitch, 0, 0x2000);
        AEMS_ActiveSampleStatic->SetAz(NormalizeStichAzimuth(Params->Az + GetData().Az + 0x10000));
        AEMS_ActiveSampleStatic->SetVol(clampedVol);
        AEMS_ActiveSampleStatic->SetPitch(clampedPitch);
        AEMS_ActiveSampleStatic->SetFilter_WetFX(ClampStichValue(Params->RVerb, 0, 0x7FFF));
        AEMS_ActiveSampleStatic->CommitMemberData();
        if (AEMS_ActiveSampleStatic->GetRefCount() < 2) {
            Destroy();
        }
    }
}

void cSampleWarpper::Play(const SND_Params *Params) {
    if (AEMS_ActiveSampleCol != nullptr || AEMS_ActiveSampleWsh != nullptr || AEMS_ActiveSampleStatic != nullptr) {
        return;
    }

    SND_SampleRef *sampleData = SampleRefData;
    AddToSampleList(this, static_cast<STICH_TYPE>(sampleData->eStichType));

    if (sampleData->RND_Vol == 0) {
        m_nLocalVolume = sampleData->Volume;
    } else {
        m_nLocalVolume = static_cast<int>(sampleData->Volume - g_pEAXSound->Random(sampleData->RND_Vol));
    }

    int TempVol = m_nLocalVolume * Params->Vol >> 0xF;
    int randomPitchRange = sampleData->RND_Pitch;
    if (randomPitchRange == 0) {
        m_nLocalPitch = sampleData->Pitch;
    } else {
        if (randomPitchRange < 0) {
            randomPitchRange = -randomPitchRange;
        }
        m_nLocalPitch = static_cast<int>(sampleData->Pitch - g_pEAXSound->Random(randomPitchRange));
    }

    float PitchScale = static_cast<float>(0x1000 - Params->Pitch) * (1.0f / 4096.0f);
    int TempPitch = static_cast<int>(static_cast<float>(m_nLocalPitch) * PitchScale);
    int TempAz = Params->Az;
    m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF;

    int stichDataType = static_cast<int>(sampleData->eStichType);
    if (stichDataType < 0) {
        stichDataType = 0;
    } else if (stichDataType > 0x10) {
        stichDataType = 0x10;
    }

    int sampleIndex = static_cast<int>(sampleData->SampleIndex);
    if (sampleIndex < 0) {
        sampleIndex = 0;
    } else if (sampleIndex > 0x3FF) {
        sampleIndex = 0x3FF;
    }

    int sampleVol = TempVol;
    if (sampleVol < 0) {
        sampleVol = 0;
    } else if (sampleVol > 0x7FFF) {
        sampleVol = 0x7FFF;
    }

    int samplePitch = TempPitch;
    if (samplePitch < 0) {
        samplePitch = 0;
    } else if (samplePitch > 0x2000) {
        samplePitch = 0x2000;
    }

    int sampleAzValue = TempAz + sampleData->Az + 0x10000;
    int sampleAzWrap = sampleAzValue;
    if (sampleAzValue < 0) {
        sampleAzWrap = sampleAzValue + 0xFFFF;
    }
    int sampleAz = sampleAzValue + ((sampleAzWrap >> 16) * -0x10000);
    if (sampleAz < 0) {
        sampleAz = 0;
    } else if (sampleAz > 0x10000) {
        sampleAz = 0x10000;
    }

    int sampleOffset = static_cast<int>(sampleData->Offset);
    if (sampleOffset > 4000) {
        sampleOffset = 4000;
    }

    int wetFX = Params->RVerb;
    if (wetFX < 0) {
        wetFX = 0;
    } else if (wetFX > 0x7FFF) {
        wetFX = 0x7FFF;
    }

    if (sampleData->eStichType == STICH_TYPE_COLLISION) {
        g_pEAXSound->SetCsisName(GetStichTypeName(STICH_TYPE_COLLISION));
        AEMS_ActiveSampleCol =
            new AEMS_StichCollision(stichDataType, sampleIndex, sampleVol, samplePitch, sampleAz, sampleOffset, 0x7FFF, wetFX,
                                    25000, 0);
        if (AEMS_ActiveSampleCol != nullptr) {
            int RefCount = AEMS_ActiveSampleCol->GetRefCount();
            m_eIsPlaying = (RefCount < 3) ? eSTITCH_PLAY_STATUS_OFF : eSTITCH_PLAY_STATUS_PLAYING;
        }
    }

    if (sampleData->eStichType == STICH_TYPE_WOOSH) {
        g_pEAXSound->SetCsisName(GetStichTypeName(STICH_TYPE_WOOSH));
        AEMS_ActiveSampleWsh =
            new AEMS_StichWoosh(stichDataType, sampleIndex, sampleVol, samplePitch, sampleAz, sampleOffset, 0x7FFF, wetFX,
                                25000, 0);
        if (AEMS_ActiveSampleWsh != nullptr) {
            int RefCount = AEMS_ActiveSampleWsh->GetRefCount();
            m_eIsPlaying = (RefCount < 3) ? eSTITCH_PLAY_STATUS_OFF : eSTITCH_PLAY_STATUS_PLAYING;
        }
    }

    if (sampleData->eStichType == STICH_TYPE_STATIC) {
        g_pEAXSound->SetCsisName(GetStichTypeName(STICH_TYPE_STATIC));
        AEMS_ActiveSampleStatic =
            new AEMS_StichStatic(stichDataType, sampleIndex, sampleVol, samplePitch, sampleAz, sampleOffset, 0x7FFF, wetFX,
                                 25000, 0);
        if (AEMS_ActiveSampleStatic != nullptr) {
            int RefCount = AEMS_ActiveSampleStatic->GetRefCount();
            m_eIsPlaying = (RefCount < 3) ? eSTITCH_PLAY_STATUS_OFF : eSTITCH_PLAY_STATUS_PLAYING;
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
    STICH_TYPE stitchType = static_cast<STICH_TYPE>(StichData->eStichType);

    if (Params != nullptr) {
        SndParams.ID = Params->ID;
        SndParams.Vol = Params->Vol;
        SndParams.Pitch = Params->Pitch;
        SndParams.Az = Params->Az;
        SndParams.Mag = Params->Mag;
        SndParams.RVerb = Params->RVerb;
        SndParams.Vol = SndParams.Vol * 0x7FFF >> 0xF;
    }

    for (int i = 0; i < static_cast<int>(StichData->Num_SampleRefs); i++) {
        cSampleWarpper *sample = new cSampleWarpper(StichData->pSampleRefList[i]);
        ActiveSamplesRefs[i] = sample;

        if (sample != nullptr) {
            sample->Initialize();

            SampleQueueItem samplereq;
            samplereq.pSample = sample;
            samplereq.pStitch = this;
            cSTICH_PlayBack::QueueSampleRequest(samplereq);
        }
    }

    bIsPlaying = true;

    int numToClear = cSTICH_PlayBack::mQueuedSampleList[stitchType].size() + cSampleListSet::GetList(stitchType).size() - 25;
    if (numToClear > 0) {
        int priority = 0;
        bool keepPruning = true;
        while (keepPruning) {
            int numQueuePruned = cSTICH_PlayBack::Prune(stitchType, priority, numToClear);
            int numListPruned = cSampleWarpper::Prune(stitchType, priority, numToClear - numQueuePruned);
            numToClear = (numToClear - numQueuePruned) - numListPruned;
            if (numToClear == 0 || priority == 10) {
                keepPruning = false;
            } else {
                priority++;
            }
        }
    }
}

void cStichWrapper::Update(const SND_Params *Params) {
    if (!bIsPlaying) {
        return;
    }

    if (Params != nullptr) {
        SndParams.ID = Params->ID;
        SndParams.Vol = Params->Vol;
        SndParams.Pitch = Params->Pitch;
        SndParams.Az = Params->Az;
        SndParams.Mag = Params->Mag;
        SndParams.RVerb = Params->RVerb;
    }

    bIsPlaying = false;
    for (int i = 0; i < static_cast<int>(StichData->Num_SampleRefs); i++) {
        if (ActiveSamplesRefs[i] != nullptr) {
            ActiveSamplesRefs[i]->Update(&SndParams);
            if (ActiveSamplesRefs[i]->m_eIsPlaying == eSTITCH_PLAY_STATUS_OFF) {
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
UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_ListSet
    UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_mLists =
        UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::_ListSet();

template class UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE>::List;
