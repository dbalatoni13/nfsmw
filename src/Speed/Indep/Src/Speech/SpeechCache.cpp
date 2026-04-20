#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern void *bMalloc(int size, int allocation_params);
extern void bFree(void *ptr);
extern int bGetFreeMemoryPoolNum();
extern void bInitMemoryPool(int pool, void *mem, int size, const char *name);
extern int bCountFreeMemory(int pool);
extern Timer WorldTimer;

namespace {
struct CacheEntry {
    unsigned long long key;
    Speech::SpeechSampleData *sample;
};

DECLARE_CONTAINER_TYPE(SpeechCacheEntries);

class SpeechCacheEntries : public UTL::Std::vector<CacheEntry, _type_SpeechCacheEntries> {};

SpeechCacheEntries gCacheEntries;

bool IsSpeakerActive(const Speech::Cache::VoiceIDs *speakers, int speaker_id) {
    if (!speakers) {
        return false;
    }

    Speech::Cache::VoiceIDs::const_iterator it = speakers->begin();
    while (it != speakers->end()) {
        if (*it == speaker_id) {
            return true;
        }
        ++it;
    }

    return false;
}
} // namespace

Speech::Cache gSpeechCache;

namespace Speech {

Cache::Cache()
    : mSpeakers(0), //
      mCache(0), //
      mCacheSize(0), //
      mInitialMemFree(0), //
      mEventPool(0), //
      mIndex(0) {}

Cache::~Cache() {
    FlushAllUnlocked();

    if (mEventPool) {
        bDeleteSlotPool(mEventPool);
        mEventPool = 0;
    }

    if (mSpeakers) {
        delete mSpeakers;
        mSpeakers = 0;
    }

    if (mCache) {
        bFree(mCache);
        mCache = 0;
    }
}

void Cache::AddSpeaker(int spkrID) {
    if (!mSpeakers) {
        mSpeakers = new VoiceIDs();
    }

    VoiceIDs::iterator it = mSpeakers->begin();
    while (it != mSpeakers->end()) {
        if (*it == spkrID) {
            return;
        }
        ++it;
    }

    mSpeakers->push_back(spkrID);
}

void Cache::RemoveSpeaker(int spkrID) {
    if (!mSpeakers) {
        return;
    }

    for (VoiceIDs::iterator it = mSpeakers->begin(); it != mSpeakers->end(); ++it) {
        if (*it == spkrID) {
            mSpeakers->erase(it);
            return;
        }
    }
}

void Cache::Init(int memsize) {
    if (!IsSpeechEnabled || (mCache != 0)) {
        return;
    }

    mCacheSize = memsize;
    mCache = bMalloc(memsize, 0);
    if (!mCache) {
        return;
    }

    int speech_pool = bGetFreeMemoryPoolNum();
    bInitMemoryPool(speech_pool, mCache, memsize, "Speech Cache Memory Pool");

    if (!mSpeakers) {
        mSpeakers = new VoiceIDs();
    }

    mEventPool = bNewSlotPool(0x80, 0xA0, "CACHE: Speech Event slotpool", speech_pool);
    if (mEventPool) {
        mEventPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);
    }

    mInitialMemFree = bCountFreeMemory(speech_pool);
    mIndex = 0;
    gCacheEntries.clear();
}

void Cache::Dump() {
    DebugPrintAllocations();
}

SlotPool *Cache::GetEventPool() {
    return mEventPool;
}

bool Cache::IsCached(SPCHType_SampleRequestData *data, bool check_preparedness) {
    if (!data) {
        return false;
    }

    const unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechCacheEntries::iterator it = gCacheEntries.begin();
    while (it != gCacheEntries.end()) {
        SpeechSampleData *sample = it->sample;
        if (sample && sample->cached && (it->key == key)) {
            if (!check_preparedness || sample->ready) {
                return true;
            }
        }
        ++it;
    }

    return false;
}

unsigned long long Cache::CreateKey(int bank, int offset) {
    return (static_cast<unsigned long long>(static_cast<unsigned int>(bank)) << 32) | static_cast<unsigned int>(offset);
}

SpeechSampleData *Cache::GetUncached(Module *, SPCHType_SampleRequestData *data) {
    if (!data) {
        return 0;
    }

    const unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *sample = SpeechSampleData::Construct(data, static_cast<unsigned int>(key), false);
    if (!sample) {
        return 0;
    }

    CacheEntry entry;
    entry.key = key;
    entry.sample = sample;
    gCacheEntries.push_back(entry);
    return sample;
}

SpeechSampleData *Cache::GetSample(Module *module, SPCHType_SampleRequestData *data) {
    if (!data) {
        return 0;
    }

    const unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechCacheEntries::iterator it = gCacheEntries.begin();
    while (it != gCacheEntries.end()) {
        if (it->sample && (it->key == key)) {
            return it->sample;
        }
        ++it;
    }

    if (IsCached(data, false)) {
        return LoadSample(module, data);
    }

    return GetUncached(module, data);
}

SpeechSampleData *Cache::LoadSample(Module *module, SPCHType_SampleRequestData *data) {
    (void)module;

    if (!data) {
        return 0;
    }

    const unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechCacheEntries::iterator it = gCacheEntries.begin();
    while (it != gCacheEntries.end()) {
        if (it->sample && (it->key == key)) {
            return it->sample;
        }
        ++it;
    }

    SpeechSampleData *sample = SpeechSampleData::Construct(data, static_cast<unsigned int>(key), true);
    if (!sample) {
        return 0;
    }

    sample->ready = true;
    sample->t_load = WorldTimer;

    CacheEntry entry;
    entry.key = key;
    entry.sample = sample;
    gCacheEntries.push_back(entry);
    return sample;
}

void Cache::LoadedSampleDataCB(int param, int error_status) {
    SpeechSampleData *sample = reinterpret_cast<SpeechSampleData *>(static_cast<unsigned int>(param));
    if (!sample) {
        return;
    }

    sample->ready = (error_status == 0);
    sample->t_load = WorldTimer;
}

void *Cache::Alloc(int size, unsigned int) {
    return bMalloc(size, 0);
}

void Cache::Free(void *mem) {
    if (mem) {
        bFree(mem);
    }
}

SpeechSampleData *Cache::MakeSpaceFor(SPCHType_SampleRequestData *, bool) {
    if (gCacheEntries.size() < 100) {
        return 0;
    }

    FlushLRU();
    if (gCacheEntries.size() >= 100) {
        FlushAllUnlocked();
    }

    return 0;
}

void Cache::TossSample(SpeechSampleData *data) {
    if (!data) {
        return;
    }

    for (SpeechCacheEntries::iterator it = gCacheEntries.begin(); it != gCacheEntries.end(); ++it) {
        if (it->sample == data) {
            gCacheEntries.erase(it);
            break;
        }
    }

    if (data->lock) {
        data->Unlock();
    }
    SpeechSampleData::Destruct(data);
}

void Cache::FlushUncached() {
    for (SpeechCacheEntries::iterator it = gCacheEntries.begin(); it != gCacheEntries.end();) {
        SpeechSampleData *sample = it->sample;
        if (sample && !sample->cached && !sample->lock) {
            SpeechSampleData::Destruct(sample);
            it = gCacheEntries.erase(it);
        } else {
            ++it;
        }
    }
}

void Cache::FlushLRU() {
    SpeechCacheEntries::iterator victim = gCacheEntries.end();
    int max_age = -1;

    for (SpeechCacheEntries::iterator it = gCacheEntries.begin(); it != gCacheEntries.end(); ++it) {
        SpeechSampleData *sample = it->sample;
        if (sample && !sample->lock && (sample->age > max_age)) {
            max_age = sample->age;
            victim = it;
        }
    }

    if (victim != gCacheEntries.end() && victim->sample) {
        SpeechSampleData::Destruct(victim->sample);
        gCacheEntries.erase(victim);
    }
}

void Cache::FlushAllUnlocked() {
    for (SpeechCacheEntries::iterator it = gCacheEntries.begin(); it != gCacheEntries.end();) {
        SpeechSampleData *sample = it->sample;
        if (sample && !sample->lock) {
            SpeechSampleData::Destruct(sample);
            it = gCacheEntries.erase(it);
        } else {
            ++it;
        }
    }
}

void Cache::FlushInactiveSpeakers() {
    for (SpeechCacheEntries::iterator it = gCacheEntries.begin(); it != gCacheEntries.end();) {
        SpeechSampleData *sample = it->sample;
        if (sample && !sample->lock && !IsSpeakerActive(mSpeakers, sample->speakerID)) {
            SpeechSampleData::Destruct(sample);
            it = gCacheEntries.erase(it);
        } else {
            ++it;
        }
    }
}

void Cache::DebugPrintAllocations() {}

void Cache::Validate() {
    for (SpeechCacheEntries::iterator it = gCacheEntries.begin(); it != gCacheEntries.end(); ++it) {
        if (it->sample && (it->sample->size == 0)) {
            it->sample->ready = false;
        }
    }
}

void Cache::DebugPrints() {}

} // namespace Speech
