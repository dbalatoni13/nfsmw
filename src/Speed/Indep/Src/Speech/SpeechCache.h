#ifndef SPEECH_SPEECHCACHE_H
#define SPEECH_SPEECHCACHE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/VecHashMap64.h"

struct SlotPool;

namespace Speech {

struct SpeechSampleData;
class Module;

struct TablePolicy_FixedAudio {
    static void *Alloc(unsigned int bytes) {
        return gAudioMemoryManager.AllocateMemory(bytes, "SpeechSampleMap node", false);
    }
    static void Free(void *ptr, unsigned int bytes) {
        (void)bytes;
        gAudioMemoryManager.FreeMemory(ptr);
    }
    static unsigned int TableSize(unsigned int entries) {
        return entries;
    }
    static unsigned int GrowRequest(unsigned int currententries, bool collisionoverflow) {
        return currententries;
    }
    static unsigned int KeyIndex(unsigned long long k, unsigned int tableSize, unsigned int keyShift) {
        return Attrib::RotateNTo32(k, keyShift) % tableSize;
    }
    static unsigned int WrapIndex(unsigned int index, unsigned int tableSize, unsigned int keyShift) {
        return index % tableSize;
    }
};

struct SpeechSampleData {
    SpeechSampleData(SPCHType_SampleRequestData *data, bool is_cached)
        : size(data->numBytes),        //
          ready(false),               //
          age(0),                     //
          speakerID(data->subID),     //
          eventID(static_cast<SPCHType_1_EventID>(data->eventSpec.eventID)), //
          HSTRM(-1),                  //
          lock(true),                 //
          cached(is_cached),          //
          t_req(WorldTimer),          //
          t_load(0),                  //
          t_play(0),                  //
          dataoffset(0) {}

    unsigned int size;          // offset 0x0, size 0x4
    bool ready;                 // offset 0x4, size 0x1
    int age;                    // offset 0x8, size 0x4
    int speakerID;              // offset 0xC, size 0x4
    SPCHType_1_EventID eventID; // offset 0x10, size 0x4
    int HSTRM;                  // offset 0x14, size 0x4
    bool lock;                  // offset 0x18, size 0x1
    bool cached;                // offset 0x1C, size 0x1
    Timer t_req;                // offset 0x20, size 0x4
    Timer t_load;               // offset 0x24, size 0x4
    Timer t_play;               // offset 0x28, size 0x4
    unsigned int dataoffset;    // offset 0x2C, size 0x4

    ~SpeechSampleData() {}
    void Lock() { lock = true; }
    void Unlock() { lock = false; }
    void *GetData() {
        return reinterpret_cast<void *>(reinterpret_cast<unsigned int>(this) + 0x40);
    }

    static void Destruct(SpeechSampleData *ptr);
    static SpeechSampleData *Construct(SPCHType_SampleRequestData *data, unsigned int key, bool is_cached);
};

struct SpchSampleMap : public VecHashMap64<SpeechSampleData, TablePolicy_FixedAudio, false, 100>, public AudioMemBase {
  public:
    SpchSampleMap(unsigned int reserve) : VecHashMap64<SpeechSampleData, TablePolicy_FixedAudio, false, 100>(reserve) {}
    virtual ~SpchSampleMap() {}
};

class Cache : public AudioMemBase {
  public:
    typedef UTL::Std::list<int, _type_list> VoiceIDs;

    Cache();
    ~Cache();

    void AddSpeaker(int spkrID) {
        if (!mSpeakers) {
            return;
        }

        bool exists = false;
        VoiceIDs::iterator it = mSpeakers->begin();
        while (it != mSpeakers->end()) {
            if (*it == spkrID) {
                exists = true;
                break;
            }
            it++;
        }

        if (!exists) {
            mSpeakers->push_back(spkrID);
        }
    }

    void RemoveSpeaker(int spkrID) {
        if (!mSpeakers) {
            return;
        }

        VoiceIDs::iterator newend = remove(mSpeakers->begin(), mSpeakers->end(), spkrID);
        if (newend != mSpeakers->end()) {
            mSpeakers->erase(newend, mSpeakers->end());
        }
    }
    SlotPool *GetEventPool();
    bool IsCached(SPCHType_SampleRequestData *data, bool check_preparedness);
    unsigned long long CreateKey(int bank, int offset);
    SpeechSampleData *GetUncached(Module *module, SPCHType_SampleRequestData *data);
    SpeechSampleData *GetSample(Module *module, SPCHType_SampleRequestData *data);
    SpeechSampleData *LoadSample(Module *module, SPCHType_SampleRequestData *data);
    static void LoadedSampleDataCB(int param, int error_status);
    void *Alloc(int size, unsigned int key);
    void Free(void *ptr);
    SpeechSampleData *MakeSpaceFor(SPCHType_SampleRequestData *data, bool cached);
    void TossSample(SpeechSampleData *data);
    void FlushUncached();
    void FlushLRU();
    void FlushAllUnlocked();
    void FlushInactiveSpeakers();
    void DebugPrintAllocations();
    void Dump();
    void Init(int size);
    void Validate();
    void DebugPrints();

  private:
    VoiceIDs *mSpeakers;   // offset 0x4, size 0x4
    void *mCache;          // offset 0x8, size 0x4
    int mCacheSize;        // offset 0xC, size 0x4
    int mInitialMemFree;   // offset 0x10, size 0x4
    SlotPool *mEventPool;  // offset 0x14, size 0x4
    SpchSampleMap mIndex;  // offset 0x18, size 0x14
};

} // namespace Speech

extern Speech::Cache gSpeechCache;

#endif
