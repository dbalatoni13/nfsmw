#ifndef EAXSOUND_STICH_PLAYBACK_H
#define EAXSOUND_STICH_PLAYBACK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

enum STICH_TYPE {
    MAX_NUM_STICH_TYPE = 3,
    STICH_TYPE_STATIC = 2,
    STICH_TYPE_WOOSH = 1,
    STICH_TYPE_COLLISION = 0,
    STICH_TYPE_UNKNOWN = -1,
};

struct SND_SampleRef {
    // total size: 0x10
    unsigned char SampleIndex;  // offset 0x0, size 0x1
    char eStichType;            // offset 0x1, size 0x1
    unsigned short Volume;      // offset 0x2, size 0x2
    unsigned short Pitch;       // offset 0x4, size 0x2
    unsigned short Offset;      // offset 0x6, size 0x2
    short Az;                   // offset 0x8, size 0x2
    unsigned short RND_Vol;     // offset 0xA, size 0x2
    short RND_Pitch;            // offset 0xC, size 0x2
    unsigned char Priority;     // offset 0xE, size 0x1
    unsigned char eRollOffType; // offset 0xF, size 0x1
};

struct SND_Stich {
    // total size: 0x14
    unsigned int NameHash;                // offset 0x0, size 0x4
    unsigned short Volume;                // offset 0x4, size 0x2
    unsigned short StichIndex;            // offset 0x6, size 0x2
    char eStichType;                      // offset 0x8, size 0x1
    unsigned char Num_SampleRefs;         // offset 0x9, size 0x1
    short RND_Pitch;                      // offset 0xA, size 0x2
    unsigned short RND_Vol;               // offset 0xC, size 0x2
    struct SND_SampleRef *pSampleRefList; // offset 0x10, size 0x4
};

struct SND_Params {
    // total size: 0x18
    int ID;    // offset 0x0, size 0x4
    int Vol;   // offset 0x4, size 0x4
    int Pitch; // offset 0x8, size 0x4
    int Az;    // offset 0xC, size 0x4
    int Mag;   // offset 0x10, size 0x4
    int RVerb; // offset 0x14, size 0x4
};

struct cSampleWarpper : public AudioMemBase {
    int m_eIsPlaying;             // offset 0x4, size 0x4
    SND_SampleRef *SampleRefData; // offset 0x8, size 0x4

    const SND_SampleRef &GetData() const {
        return *SampleRefData;
    }

    void Play(const SND_Params *params);
    void Destroy();
};

// total size: 0x6C
struct cStichWrapper : public AudioMemBase {
    SND_Params SndParams;        // offset 0x4, size 0x18
    SND_Stich *StichData;        // offset 0x1C, size 0x4
    bool bIsPlaying;             // offset 0x20, size 0x1
    cSampleWarpper *ActiveSamplesRefs[18]; // offset 0x24, size 0x48

    const SND_Stich &GetData() const {
        return *StichData;
    }

    void Destroy();
};

char *GetStichTypeName(STICH_TYPE CurType /* r3 */);

struct SampleQueueItem {
    // total size: 0x8
    cSampleWarpper *pSample; // offset 0x0, size 0x4
    cStichWrapper *pStitch;  // offset 0x4, size 0x4

    bool operator==(const SampleQueueItem &compareto) const {
        return pSample == compareto.pSample && pStitch == compareto.pStitch;
    }
};

// total size: 0x1C
class cSTICH_PlayBack : public AudioMemBase {
  public:
    cSTICH_PlayBack();
    ~cSTICH_PlayBack() override;

    bool AddStich(STICH_TYPE, SND_Stich &);
    SND_Stich &GetStich(STICH_TYPE StichType, int Index);
    void Update(float t);
    void DestroyAllStichs(void);

    bPList<SND_Stich> &GetStichList(STICH_TYPE StichType);

    static UTL::FixedVector<SampleQueueItem, 43, 16> &GetQueueList(STICH_TYPE type) {
        return mQueuedSampleList[type];
    }

  protected:
    static void QueueSampleRequest(struct SampleQueueItem &samplereq);
    static void RemoveFromList(struct SampleQueueItem sampleitem);
    static int Prune(STICH_TYPE type, int priority, int num_to_clear);

  private:
    struct bPList<SND_Stich> StichList[3]; // offset 0x4, size 0x18

    static SlotPool *mSampleRefSlotPool;
    static SlotPool *mStitchSlotPool;
    static UTL::FixedVector<SampleQueueItem, 43, 16> mQueuedSampleList[3];
};

#endif
