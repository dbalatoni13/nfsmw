#ifndef SPEECH_SPEECHCACHE_H
#define SPEECH_SPEECHCACHE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"

struct SlotPool;

namespace Speech {

struct SpeechSampleData;
class Module;

class Cache {
  public:
    typedef UTL::Std::list<int, _type_list> VoiceIDs;

    Cache();
    ~Cache();

    void AddSpeaker(int spkrID);
    void RemoveSpeaker(int spkrID);
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
    int mIndex;            // offset 0x18, size 0x4
};

} // namespace Speech

extern Speech::Cache gSpeechCache;

#endif
