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

char *GetStichTypeName(STICH_TYPE CurType /* r3 */);

struct SampleQueueItem {
    // total size: 0x8
    struct cSampleWarpper *pSample; // offset 0x0, size 0x4
    struct cStichWrapper *pStitch;  // offset 0x4, size 0x4
};

struct cSTICH_PlayBack : public AudioMemBase {
  public:
    cSTICH_PlayBack();
    virtual ~cSTICH_PlayBack();

    bool AddStich(STICH_TYPE, SND_Stich &);
    SND_Stich &GetStich(STICH_TYPE StichType, int Index);
    void Update(float t);
    void DestroyAllStichs(void);

    bPList<SND_Stich> &GetStichList(STICH_TYPE StichType) {
        return StichList[StichType];
    }

  protected:
    static void QueueSampleRequest(struct SampleQueueItem &samplereq);
    static void RemoveFromList(struct SampleQueueItem sampleitem);
    static int Prune(STICH_TYPE type, int priority, int num_to_clear);

  private:
    // total size: 0x1C
    struct bPList<SND_Stich> StichList[3]; // offset 0x4, size 0x18

    static SlotPool *mSampleRefSlotPool;
    static SlotPool *mStitchSlotPool;
    static UTL::FixedVector<SampleQueueItem, 43, 16> mQueuedSampleList[3];
};

#endif
