//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef __SPEECHCACHE_H
#define __SPEECHCACHE_H

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"

// TODO decrease for MW
#define SPEECH_CACHE_SIZE 4 * 27 * 1024 // Decl: 43

#define SPEECH_EVENT_SLOTS 200 // Decl: 48

#define SPEECH_CACHE_MAX_SAMPLES 500 // Decl: 53

namespace Speech {

// total size: 0x1
// Decl: 66
struct TablePolicy_FixedAudio {
    static void *Alloc(size_t bytes) {
        return gAudioMemoryManager.AllocateMemory(bytes, "SpeechSampleMap node", false);
    }

    static void Free(void *ptr, size_t bytes) {
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

// total size: 0x14
// Decl: 77
class SpchSampleMap : public VecHashMap64<Speech::SpeechSampleData, Speech::TablePolicy_FixedAudio, false, 100>, public AudioMemBase {
  public:
    SpchSampleMap(unsigned int reserve) : VecHashMap64<Speech::SpeechSampleData, Speech::TablePolicy_FixedAudio, false, 100>(reserve) {}
    ~SpchSampleMap() override {}
};

// total size: 0x2C
// Decl: 85
class Cache : public AudioMemBase {
  public:
    typedef UTL::Std::list<int, _type_list> VoiceIDs;

    Cache();
    // Overrides: AudioMemBase
    ~Cache() override;

    static void LoadedSampleDataCB(int32 param, int error_status);

    void Init(int memsize);
    void Dump();

    SpeechSampleData *GetSample(Module *module, SPCHType_SampleRequestData *data);
    SpeechSampleData *LoadSample(Module *module, SPCHType_SampleRequestData *data);
    void TossSample(SpeechSampleData *data);

    SpeechSampleData *GetUncached(Module *module, SPCHType_SampleRequestData *data);
    bool IsCached(SPCHType_SampleRequestData *data, bool check_preparedness);

    void DebugPrints();
    void DebugPrintAllocations();

    void *Alloc(int size, Attrib::Key key);
    void Free(void *mem);

    SlotPool *GetEventPool();
    SpeechSampleData *MakeSpaceFor(SPCHType_SampleRequestData *data, bool cached);

    uint64_t CreateKey(int bank, int offset);

    // Decl: 146
    void AddSpeaker(int spkrID) {
        if (mSpeakers == nullptr) {
            return;
        }

        bool exists = false;
        for (VoiceIDs::iterator i = mSpeakers->begin(); i != mSpeakers->end(); i++) {
            if (*i == spkrID) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            mSpeakers->push_back(spkrID);
        }
    }

    // Decl: 167
    void RemoveSpeaker(int spkrID) {
        if (mSpeakers == nullptr) {
            return;
        }

        VoiceIDs::iterator newend = remove(mSpeakers->begin(), mSpeakers->end(), spkrID);
        if (newend != mSpeakers->end()) {
            mSpeakers->erase(newend, mSpeakers->end());
        }
    }

    void Validate();

  private:
    void FlushLRU();
    void FlushAllUnlocked();
    void FlushInactiveSpeakers();
    void FlushUncached();

    VoiceIDs *mSpeakers; // offset 0x4, size 0x4, Decl: 133
    void *mCache;        // offset 0x8, size 0x4, Decl: 134
    int mCacheSize;      // offset 0xC, size 0x4, Decl: 135

    int mInitialMemFree;  // offset 0x10, size 0x4, Decl: 138
    SlotPool *mEventPool; // offset 0x14, size 0x4, Decl: 139

    SpchSampleMap mIndex; // offset 0x18, size 0x14, Decl: 141
};

}; // namespace Speech

extern Speech::Cache gSpeechCache; // Decl: 178

#endif
