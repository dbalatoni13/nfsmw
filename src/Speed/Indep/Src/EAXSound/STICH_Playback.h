//
//
//
//
//
#ifndef STICH_PLAYBACK_H
#define STICH_PLAYBACK_H

#include "SND_GEN/STITCH_AEMS.h"
#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Struct.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

#define MaxNumStitchSamples 42
#define MaxStitchPriority 10

class cStichWrapper;

// total size: 0x14

class cStitchLoop {
  public:
    cStitchLoop(Attrib::Key _attrib);
    ~cStitchLoop();

    void Update(const SND_Params *Params, float dt);

  private:
    Attrib::Key m_StitchAttribKey; // offset 0x0, size 0x4
    const SND_Stich *m_StichData;  // offset 0x4, size 0x4
    cStichWrapper *m_Stitch[2];    // offset 0x8, size 0x8
    int16 m_tOverlap;              // offset 0x10, size 0x2
    int16 m_tTimeBeforeRepeat;     // offset 0x12, size 0x2
};

// total size: 0x20

class cSampleWarpper : public UTL::Collections::ListableSet<cSampleWarpper, 25, STICH_TYPE, MAX_NUM_STICH_TYPE> {
  public:
    enum eSTITCH_PLAY_STATUS {
        eSTITCH_PLAY_STATUS_OFF = 0,
        eSTITCH_PLAY_STATUS_QUEUED = 1,
        eSTITCH_PLAY_STATUS_PLAYING = 2,
    };

    cSampleWarpper(SND_SampleRef &NewRef);
    ~cSampleWarpper();

    void *operator new(size_t obj_size);
    void operator delete(void *ptr);

    void Initialize();

    void Update(const SND_Params *Params);
    void Play(const SND_Params *Params);

    void Destroy();

    bool IsPlaying() {
        return this->m_eIsPlaying != eSTITCH_PLAY_STATUS_OFF;
    }
    const SND_SampleRef &GetData() {
        return *this->SampleRefData;
    }

  protected:
    eSTITCH_PLAY_STATUS m_eIsPlaying; // offset 0x4, size 0x4

    static int Prune(enum STICH_TYPE type, int priority, int num_to_clear);
    static bool SortSamplePriority(SND_SampleRef *FirstSample, SND_SampleRef *SecondSample);

  private:
    const SND_SampleRef *SampleRefData;        // offset 0x8, size 0x4
    AEMS_StichCollision *AEMS_ActiveSampleCol; // offset 0xC, size 0x4
    AEMS_StichWoosh *AEMS_ActiveSampleWsh;     // offset 0x10, size 0x4
    AEMS_StichStatic *AEMS_ActiveSampleStatic; // offset 0x14, size 0x4

    int m_nLocalVolume; // offset 0x18, size 0x4
    int m_nLocalPitch;  // offset 0x1C, size 0x4
};

// total size: 0x6C

class cStichWrapper : public AudioMemBase {
  public:
    cStichWrapper(const SND_Stich &NewStichData);
    ~cStichWrapper() override;

    static void *operator new(size_t obj_size);

    static void operator delete(void *ptr);

    void Update(const SND_Params *Params);
    void Play(const SND_Params *Params);
    void Play(int Vol, int Pitch, int Azimuth);
    void Pause();
    void Destroy();

    bool IsPlaying() {
        return this->bIsPlaying;
    }

    const SND_Stich &GetData() {
        return *this->StichData;
    }

    int GetNumVoicesPlaying();

    SND_Params SndParams; // offset 0x4, size 0x18

  private:
    const SND_Stich *StichData;            // offset 0x1C, size 0x4
    bool bIsPlaying;                       // offset 0x20, size 0x1
    cSampleWarpper *ActiveSamplesRefs[18]; // offset 0x24, size 0x48
};

// total size: 0x8

struct SampleQueueItem {
    cSampleWarpper *pSample; // offset 0x0, size 0x4
    cStichWrapper *pStitch;  // offset 0x4, size 0x4
};

// total size: 0x1C

class cSTICH_PlayBack : public AudioMemBase {
  public:
    cSTICH_PlayBack();
    ~cSTICH_PlayBack() override;

    bool AddStich(STICH_TYPE StichType, SND_Stich &NewStichData);
    void DestroyAllStichs();
    SND_Stich &GetStich(STICH_TYPE StichType, int Index);

    bPList<SND_Stich> &GetStichList(STICH_TYPE StichType) {
        return StichList[StichType];
    }

    void Update(float t);

  protected:
    static void RemoveFromList(SampleQueueItem sampleitem);
    static void QueueSampleRequest(SampleQueueItem &samplereq);
    static int Prune(STICH_TYPE type, int priority, int num_to_clear);

    static SlotPool *mSampleRefSlotPool;
    static SlotPool *mStitchSlotPool;

  private:
    static UTL::FixedVector<SampleQueueItem, 43> mQueuedSampleList[3];
    bPList<SND_Stich> StichList[3]; // offset 0x4, size 0x18
};

char *GetStichTypeName(STICH_TYPE CurType);

#endif
