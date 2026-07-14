#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/speech.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern void *bMalloc(int size, int allocation_params);
extern void bFree(void *ptr);
extern int bGetFreeMemoryPoolNum();
extern void bInitMemoryPool(int pool, void *mem, int size, const char *name);
extern void bCloseMemoryPool(int pool);
extern int bCountFreeMemory(int pool);
extern int bLargestMalloc(int pool);
extern void bMemoryPrintAllocations(int pool_num, int from_allocation, int to_allocation);
extern Timer WorldTimer;
extern int SpeechMemoryPool;
extern int PRINT_SPEECH_CACHE_IO;
extern int flushcount_uncached;
extern int flushcount_lru;
extern int flushcount_inactive_spkrs;
extern int flushcount_all;
extern bool SPEECH_CACHE_STATS;

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

struct SpeechLoadCBData {
    Module *object;
    SpeechSampleData *data;
};

Cache::Cache()
    : mSpeakers(0), //
      mCache(0), //
      mCacheSize(0), //
      mInitialMemFree(0), //
      mIndex(0) {}

Cache::~Cache() {
    Dump();
}

void Cache::Init(int memsize) {
    if (!IsSpeechEnabled || (mCache != 0)) {
        return;
    }

    mCacheSize = memsize;
    mCache = bMalloc(memsize, 0);

    int speech_pool = bGetFreeMemoryPoolNum();
    SpeechMemoryPool = speech_pool;
    bInitMemoryPool(speech_pool, mCache, memsize, "Speech Cache Memory Pool");

    mSpeakers = new VoiceIDs;

    mEventPool = bNewSlotPool(0x80, 0xA0, "CACHE: Speech Event slotpool", SpeechMemoryPool);
    mEventPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);

    mInitialMemFree = bCountFreeMemory(SpeechMemoryPool);
    mIndex.Reserve(100);
}

void Cache::Dump() {
    if (mEventPool) {
        bDeleteSlotPool(mEventPool);
    }

    if (mCache) {
        bFree(mCache);
        bCloseMemoryPool(SpeechMemoryPool);
        mCache = 0;
        mCacheSize = 0;
    }

    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtValidIndex(index);
        SpeechSampleData::Destruct(sample);
        index = mIndex.GetNextValidIndex(index + 1);
    }

    mIndex.Clear();

    if (mSpeakers) {
        mSpeakers->clear();
        delete mSpeakers;
    }
    mSpeakers = 0;
}

SlotPool *Cache::GetEventPool() {
    return mEventPool;
}

bool Cache::IsCached(SPCHType_SampleRequestData *data, bool check_preparedness) {
    if (!IsSpeechEnabled) {
        return false;
    }

    unsigned long long indexKey = CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *sample = mIndex.Find(indexKey);
    if (!sample) {
        return false;
    }

    if (!check_preparedness) {
        return true;
    }

    return sample->ready;
}

unsigned long long Cache::CreateKey(int bank, int offset) {
    unsigned long long x = static_cast<unsigned long long>(offset);
    unsigned long long y = static_cast<unsigned long long>(bank);
    unsigned long long result;

    x = (x | x << 16) & 0xffff0000ffffULL;
    y = (y | y << 16) & 0xffff0000ffffULL;
    x = (x | x << 8) & 0xff00ff00ff00ffULL;
    y = (y | y << 8) & 0xff00ff00ff00ffULL;
    x = (x | x << 4) & 0xf0f0f0f0f0f0f0fULL;
    y = (y | y << 4) & 0xf0f0f0f0f0f0f0fULL;
    x = (x | x << 2) & 0x3333333333333333ULL;
    y = (y | y << 2) & 0x3333333333333333ULL;
    y = (y | y << 1) & 0x5555555555555555ULL;
    x = (x | x << 1) & 0x5555555555555555ULL;

    result = y | x << 1;
    return result;
}

SpeechSampleData *Cache::GetUncached(Module *module, SPCHType_SampleRequestData *data) {
    unsigned long long indexKey;
    SpeechSampleData *rval;

    indexKey = CreateKey(data->bankNum, data->sampleOffset);
    rval = mIndex.Find(indexKey);
    if (!rval) {
        rval = MakeSpaceFor(data, false);
        rval->cached = false;
        rval->ready = true;
        rval->t_req = WorldTimer;
        rval->t_play = Timer(0);
        rval->Unlock();
        rval->dataoffset = module->GetBankOffset(data->bankNum) + data->sampleOffset;
        mIndex.Add(indexKey, rval);
    }

    return rval;
}

SpeechSampleData *Cache::GetSample(Module *, SPCHType_SampleRequestData *data) {
    unsigned long long indexKey;
    SpeechSampleData *rval;

    indexKey = CreateKey(data->bankNum, data->sampleOffset);
    rval = mIndex.Find(indexKey);
    if (rval && rval->size == data->numBytes && rval->ready) {
        ++rval->age;
        rval->t_req = WorldTimer;
        rval->t_play = Timer(0);
        rval->Lock();
    }
    return rval;
}

SpeechSampleData *Cache::LoadSample(Module *module, SPCHType_SampleRequestData *data) {
    SpeechSampleData *sample;
    unsigned long long key;
    unsigned int baseoffset;
    SpeechLoadCBData *CBdata;

    if (static_cast<float>(data->numBytes) > 5529.6f) {
        return 0;
    }

    sample = MakeSpaceFor(data, true);
    if (!sample) {
        if (SPEECH_CACHE_STATS) {
            DebugPrints();
        }

        return 0;
    }

    key = CreateKey(data->bankNum, data->sampleOffset);
    mIndex.Add(key, sample);

    baseoffset = module->GetBankOffset(data->bankNum);
    sample->dataoffset = baseoffset + data->sampleOffset;

    CBdata = new SpeechLoadCBData[1];
    CBdata->data = sample;
    CBdata->object = module;

    AddQueuedFile(sample->GetData(), module->GetFilename(), sample->dataoffset, sample->size, Cache::LoadedSampleDataCB,
                  reinterpret_cast<int>(CBdata), 0);
    return sample;
}

void Cache::LoadedSampleDataCB(int param, int error_status) {
    SpeechLoadCBData *CBdata = reinterpret_cast<SpeechLoadCBData *>(static_cast<unsigned int>(param));
    CBdata->data->ready = true;
    ++CBdata->data->age;
    CBdata->data->t_load = WorldTimer;
    delete CBdata;
}

void *Cache::Alloc(int size, unsigned int key) {
    void *memptr = nullptr;
    if (size <= bLargestMalloc(SpeechMemoryPool)) {
        const char *allocname;
        memptr = bMalloc(size, allocname, 0, (SpeechMemoryPool & 0xf) | 0x1000);
    } else if (SPEECH_CACHE_STATS) {
        DebugPrintAllocations();
    }
    return memptr;
}

void Cache::Free(void *mem) {
    if (mem) {
        bFree(mem);
    }
}

SpeechSampleData *Cache::MakeSpaceFor(SPCHType_SampleRequestData *data, bool cached) {
    short attempt;
    unsigned int key;
    SpeechSampleData *sample;
    unsigned int highwater_idx;

    key = Manager::mHashMap.GetHash(static_cast<SPCHType_1_EventID>(data->eventSpec.eventID));
    attempt = 0;
    sample = 0;

    highwater_idx = static_cast<unsigned int>(static_cast<float>(mIndex.Capacity()) * 0.98f);
    if (mIndex.Size() <= highwater_idx) {
        sample = SpeechSampleData::Construct(data, key, cached);
    }

    while (!sample && attempt >= 0) {
        switch (attempt) {
        case 0:
            FlushUncached();
            break;
        case 1:
            FlushInactiveSpeakers();
            break;
        case 2:
            FlushLRU();
            break;
        case 3:
            FlushAllUnlocked();
            break;
        default:
            attempt = -1;
            break;
        }

        if (attempt >= 0) {
            ++attempt;
            highwater_idx = static_cast<unsigned int>(static_cast<float>(mIndex.Capacity()) * 0.98f);
            if (mIndex.Size() <= highwater_idx) {
                sample = SpeechSampleData::Construct(data, key, cached);
            }
        }
    }

    return sample;
}

void Cache::TossSample(SpeechSampleData *data) {
    if (SPEECH_CACHE_STATS) {
        Attrib::Gen::speech speech(Manager::mHashMap.GetHash(data->eventID), 0, 0);
    }

    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtValidIndex(index);
        if (sample == data && !data->lock) {
            SpeechSampleData::Destruct(data);
            mIndex.RemoveIndex(index);
            if (SPEECH_CACHE_STATS) {
                Attrib::Gen::speech speech(Manager::mHashMap.GetHash(data->eventID), 0, 0);
            }
            return;
        }
        index = mIndex.GetNextValidIndex(index + 1);
    }
}

void Cache::FlushUncached() {
    unsigned int index = mIndex.GetNextValidIndex(0);
    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtIndex(index);
        if (!sample->lock && sample->ready && !sample->cached) {
            if (PRINT_SPEECH_CACHE_IO) {
                Attrib::Gen::speech speech(Manager::mHashMap.GetHash(sample->eventID), 0, 0);
            }
            SpeechSampleData::Destruct(sample);
            mIndex.RemoveIndex(index);
        }
        index = mIndex.GetNextValidIndex(index + 1);
    }

    ++flushcount_uncached;
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
    bLargestMalloc(SpeechMemoryPool);

    unsigned int prelargest = 0;
    unsigned int index = mIndex.GetNextValidIndex(prelargest);
    unsigned int postlargest = 0;

    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtValidIndex(index);
        if (!sample->lock && sample->ready && sample->cached) {
            SpeechSampleData::Destruct(sample);
            mIndex.DeleteIndex(index);
        }

        index++;
        index = mIndex.GetNextValidIndex(index);
    }

    bLargestMalloc(SpeechMemoryPool);
    ++flushcount_all;
}

void Cache::FlushInactiveSpeakers() {
    bLargestMalloc(SpeechMemoryPool);

    unsigned int prelargest = 0;
    unsigned int index = mIndex.GetNextValidIndex(prelargest);
    unsigned int postlargest = 0;

    while (mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = mIndex.GetPtrAtValidIndex(index);
        if (!sample->lock && sample->ready && sample->cached) {
            bool inactive = true;
            for (VoiceIDs::iterator i = mSpeakers->begin(); i != mSpeakers->end(); i++) {
                if ((*i == sample->speakerID) || (sample->speakerID == 0xffff)) {
                    inactive = false;
                }
            }

            if (inactive) {
                SpeechSampleData::Destruct(sample);
                mIndex.DeleteIndex(index);
            }
        }

        index++;
        index = mIndex.GetNextValidIndex(index);
    }

    bLargestMalloc(SpeechMemoryPool);
    ++flushcount_inactive_spkrs;
}

void Cache::DebugPrintAllocations() {
    bMemoryPrintAllocations(SpeechMemoryPool, 0, 0x7fffffff);

    unsigned int index = mIndex.GetNextValidIndex(0);
    for (;;) {
        int valid = mIndex.ValidIndex(index);
        if (valid == 0) {
            break;
        }
        SpchSampleMap *map = &mIndex;
        SpeechSampleData *sample = map->GetPtrAtValidIndex(index);
        Attrib::Gen::speech speech(Manager::mHashMap.GetHash(sample->eventID), 0, 0);
        index = map->GetNextValidIndex(index + 1);
    }
}

void Cache::Validate() {
}

void Cache::DebugPrints() {}

} // namespace Speech
