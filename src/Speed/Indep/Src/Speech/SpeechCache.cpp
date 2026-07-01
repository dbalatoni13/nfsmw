#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern void *bMalloc(int size, int allocation_params);
extern void bFree(void *ptr);
extern int bGetFreeMemoryPoolNum();
extern void bInitMemoryPool(int pool, void *mem, int size, const char *name);
extern int bCountFreeMemory(int pool);
extern int bLargestMalloc(int pool);
extern Timer WorldTimer;
extern int SpeechMemoryPool;
extern int flushcount_lru;

namespace {
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
      mIndex(100) {}

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
    mIndex.Clear();
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

    SpeechSampleData *sample = mIndex.Find(CreateKey(data->bankNum, data->sampleOffset));
    if (sample && sample->cached) {
        if (!check_preparedness || sample->ready) {
            return true;
        }
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

    unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *sample = SpeechSampleData::Construct(data, static_cast<unsigned int>(key), false);
    if (!sample) {
        return 0;
    }

    mIndex.Add(key, sample);
    return sample;
}

SpeechSampleData *Cache::GetSample(Module *module, SPCHType_SampleRequestData *data) {
    if (!data) {
        return 0;
    }

    unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *sample = mIndex.Find(key);

    if (!sample) {
        if (IsCached(data, false)) {
            sample = LoadSample(module, data);
        } else {
            sample = GetUncached(module, data);
        }
    }

    if (sample) {
        sample->Lock();
    }
    return sample;
}

SpeechSampleData *Cache::LoadSample(Module *module, SPCHType_SampleRequestData *data) {
    (void)module;

    if (!data) {
        return 0;
    }

    unsigned long long key = CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *sample = mIndex.Find(key);
    if (sample) {
        return sample;
    }

    sample = SpeechSampleData::Construct(data, static_cast<unsigned int>(key), true);
    if (!sample) {
        return 0;
    }

    sample->ready = true;
    sample->t_load = WorldTimer;

    mIndex.Add(key, sample);
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
    if (mIndex.Size() < 100) {
        return 0;
    }

    FlushLRU();
    if (mIndex.Size() >= 100) {
        FlushAllUnlocked();
    }

    return 0;
}

void Cache::TossSample(SpeechSampleData *data) {
    if (!data) {
        return;
    }

    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtIndex(index);
        if (sample == data && !data->lock) {
            mIndex.DeleteIndex(index);
            return;
        }
        index = mIndex.GetNextValidIndex(index + 1);
    }
}

void Cache::FlushUncached() {
    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtIndex(index);
        if (sample && !sample->cached && !sample->lock) {
            mIndex.DeleteIndex(index);
        } else {
            index++;
        }
        index = mIndex.GetNextValidIndex(index);
    }
}

void Cache::FlushLRU() {
    bLargestMalloc(SpeechMemoryPool);

    unsigned int prelargest = 0;
    unsigned int index = mIndex.GetNextValidIndex(prelargest);
    unsigned int postlargest = 0;

    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtValidIndex(index);
        if (sample != 0 && !sample->lock) {
            if (sample->ready && sample->cached && sample->age < 2) {
                SpeechSampleData::Destruct(sample);
                mIndex.RemoveIndex(index);
            }
        }
        index = mIndex.GetNextValidIndex(index + 1);
    }

    bLargestMalloc(SpeechMemoryPool);
    ++flushcount_lru;
}

void Cache::FlushAllUnlocked() {
    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtIndex(index);
        if (sample && !sample->lock) {
            mIndex.DeleteIndex(index);
        } else {
            index++;
        }
        index = mIndex.GetNextValidIndex(index);
    }
}

void Cache::FlushInactiveSpeakers() {
    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtIndex(index);
        if (sample && !sample->lock && !IsSpeakerActive(mSpeakers, sample->speakerID)) {
            mIndex.DeleteIndex(index);
        } else {
            index++;
        }
        index = mIndex.GetNextValidIndex(index);
    }
}

void Cache::DebugPrintAllocations() {}

void Cache::Validate() {
    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtIndex(index);
        if (sample && (sample->size == 0)) {
            sample->ready = false;
        }
        index = mIndex.GetNextValidIndex(index + 1);
    }
}

void Cache::DebugPrints() {}

} // namespace Speech
