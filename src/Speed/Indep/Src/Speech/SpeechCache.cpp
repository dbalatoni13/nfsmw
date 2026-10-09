#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"

Speech::Cache gSpeechCache;

int SpeechMemoryPool = 3; // Decl: 26

bool SPEECH_CACHE_STATS = false;         // Decl: 29
static const bool DUMP_BANK_MAP = false; // Decl: 30
extern int IsSpeechEnabled;              // Decl: 31
int PRINT_SPEECH_CACHE_IO = 0;           // Decl: 32

static int flushcount_uncached = 0;       // Decl: 34
static int flushcount_lru = 0;            // Decl: 35
static int flushcount_inactive_spkrs = 0; // Decl: 36
static int flushcount_all = 0;            // Decl: 37

namespace Speech {

Cache::Cache()
    : mSpeakers(nullptr), //
      mCache(nullptr),    //
      mCacheSize(0),      //
      mInitialMemFree(0), //
      mIndex(0) {}

void Cache::Init(int memsize) {
    if (!IsSpeechEnabled || (this->mCache != nullptr)) {
        return;
    }

    this->mCacheSize = memsize;
    this->mCache = bMalloc(memsize, "Speech Cache Memory Pool", 0, 0);

    SpeechMemoryPool = bGetFreeMemoryPoolNum();
    bInitMemoryPool(SpeechMemoryPool, this->mCache, memsize, "Speech Cache Memory Pool");

    this->mSpeakers = new ("Speech Cache Memory Pool", 0) VoiceIDs;

    this->mEventPool = bNewSlotPool(0x80, 0xA0, "CACHE: Speech Event slotpool", SpeechMemoryPool);
    this->mEventPool->ClearFlag(SLOTPOOL_FLAG_OVERFLOW_IF_FULL);

    this->mInitialMemFree = bCountFreeMemory(SpeechMemoryPool);
    this->mIndex.Reserve(100);
}

void Cache::Dump() {
    if (this->mEventPool != nullptr) {
        bDeleteSlotPool(this->mEventPool);
    }

    if (this->mCache != nullptr) {
        bFree(this->mCache);
        bCloseMemoryPool(SpeechMemoryPool);
        this->mCache = nullptr;
        this->mCacheSize = 0;
    }

    unsigned int index = this->mIndex.GetNextValidIndex(0);
    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        SpeechSampleData::Destruct(sample);
        index = this->mIndex.GetNextValidIndex(index + 1);
    }

    this->mIndex.Clear();

    if (this->mSpeakers != nullptr) {
        this->mSpeakers->clear();
        delete this->mSpeakers;
    }
    this->mSpeakers = nullptr;
}

SlotPool *Cache::GetEventPool() {
    return this->mEventPool;
}

Cache::~Cache() {
    this->Dump();
}

bool Cache::IsCached(SPCHType_SampleRequestData *data, bool check_preparedness) {
    if (!IsSpeechEnabled) {
        return false;
    }

    uint64_t indexKey = this->CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *sample = this->mIndex.Find(indexKey);
    if (sample == nullptr) {
        return false;
    }

    if (!check_preparedness) {
        return true;
    }

    return sample->ready;
}

uint64_t Cache::CreateKey(int bank, int offset) {
    uint64_t x = static_cast<uint64_t>(bank);
    uint64_t y = static_cast<uint64_t>(offset);

    y = (y | y << 16) & 0xffff0000ffffULL;
    x = (x | x << 16) & 0xffff0000ffffULL;
    y = (y | y << 8) & 0xff00ff00ff00ffULL;
    x = (x | x << 8) & 0xff00ff00ff00ffULL;
    y = (y | y << 4) & 0xf0f0f0f0f0f0f0fULL;
    x = (x | x << 4) & 0xf0f0f0f0f0f0f0fULL;
    y = (y | y << 2) & 0x3333333333333333ULL;
    x = (x | x << 2) & 0x3333333333333333ULL;
    x = (x | x << 1) & 0x5555555555555555ULL;
    y = (y | y << 1) & 0x5555555555555555ULL;
    uint64_t result = x | y << 1;
    return result;
}

SpeechSampleData *Cache::GetUncached(Module *module, SPCHType_SampleRequestData *data) {
    uint64_t indexKey = this->CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *rval = this->mIndex.Find(indexKey);
    if (rval == nullptr) {
        rval = this->MakeSpaceFor(data, false);
        rval->cached = false;
        rval->ready = true;
        rval->t_req = WorldTimer;
        rval->t_play = Timer(0);
        rval->Unlock();
        rval->dataoffset = module->GetBankOffset(data->bankNum) + data->sampleOffset;
        this->mIndex.Add(indexKey, rval);
    }

    return rval;
}

SpeechSampleData *Cache::GetSample(Module *module, SPCHType_SampleRequestData *data) {
    uint64_t indexKey = this->CreateKey(data->bankNum, data->sampleOffset);
    SpeechSampleData *rval = this->mIndex.Find(indexKey);
    if (rval != nullptr && rval->size == data->numBytes && rval->ready) {
        ++rval->age;
        rval->t_req = WorldTimer;
        rval->t_play = Timer(0);
        rval->Lock();
    }
    return rval;
}

SpeechSampleData *Cache::LoadSample(Module *module, SPCHType_SampleRequestData *data) {
    if (static_cast<float>(data->numBytes) > 5529.6f) {
        return nullptr;
    }

    SpeechSampleData *sample = this->MakeSpaceFor(data, true);
    if (sample == nullptr) {
        if (SPEECH_CACHE_STATS) {
            this->DebugPrints();
        }

        return nullptr;
    }

    uint64_t key = this->CreateKey(data->bankNum, data->sampleOffset);
    this->mIndex.Add(key, sample);

    unsigned int baseoffset = module->GetBankOffset(data->bankNum);
    sample->dataoffset = baseoffset + data->sampleOffset;

    SpeechLoadCBData *CBdata = new ("SpeechSampleMap node", 0) SpeechLoadCBData;
    CBdata->data = sample;
    CBdata->object = module;

    AddQueuedFile(sample->GetData(), module->GetFilename(), sample->dataoffset, sample->size, Cache::LoadedSampleDataCB,
                  reinterpret_cast<intptr_t>(CBdata), nullptr);
    return sample;
}

void Cache::LoadedSampleDataCB(int param, int error_status) {
    SpeechLoadCBData *CBdata = reinterpret_cast<SpeechLoadCBData *>(static_cast<unsigned int>(param));
    CBdata->data->ready = true;
    ++CBdata->data->age;
    CBdata->data->t_load = WorldTimer;
    delete CBdata;
}

void *Cache::Alloc(int size, Attrib::Key key) {
    void *memptr = nullptr;
    if (size <= bLargestMalloc(SpeechMemoryPool)) {
        const char *allocname = "Speech::Cache::Alloc";
        memptr = bMalloc(size, allocname, 0, (SpeechMemoryPool & 0xf) | 0x1000);
    } else if (SPEECH_CACHE_STATS) {
        this->DebugPrintAllocations();
    }
    return memptr;
}

void Cache::Free(void *mem) {
    if (mem != nullptr) {
        bFree(mem);
    }
}

SpeechSampleData *Cache::MakeSpaceFor(SPCHType_SampleRequestData *data, bool cached) {
    short attempt = 0;
    Attrib::Key key = Manager::GetHashIDMap().GetHash(static_cast<SPCHType_1_EventID>(data->eventSpec.eventID));
    SpeechSampleData *sample = nullptr;

    unsigned int highwater_idx = static_cast<unsigned int>(static_cast<float>(this->mIndex.Capacity()) * 0.98f);
    if (this->mIndex.Size() <= highwater_idx) {
        sample = SpeechSampleData::Construct(data, key, cached);
    }

    while (sample == nullptr && attempt >= 0) {
        switch (attempt) {
            case 0:
                this->FlushUncached();
                break;
            case 1:
                this->FlushInactiveSpeakers();
                break;
            case 2:
                this->FlushLRU();
                break;
            case 3:
                this->FlushAllUnlocked();
                break;
            default:
                attempt = -1;
                break;
        }

        if (attempt >= 0) {
            ++attempt;
            highwater_idx = static_cast<unsigned int>(static_cast<float>(this->mIndex.Capacity()) * 0.98f);
            if (this->mIndex.Size() <= highwater_idx) {
                sample = SpeechSampleData::Construct(data, key, cached);
            }
        }
    }

    return sample;
}

void Cache::TossSample(SpeechSampleData *data) {
    if (SPEECH_CACHE_STATS) {
        Attrib::Gen::speech speech(Manager::GetHashIDMap().GetHash(data->eventID), 0, nullptr);
    }

    unsigned int index = this->mIndex.GetNextValidIndex(0);
    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        if (sample == data && !sample->lock) {
            SpeechSampleData::Destruct(sample);
            this->mIndex.DeleteIndex(index);
            if (SPEECH_CACHE_STATS) {
                Attrib::Gen::speech speech(Manager::GetHashIDMap().GetHash(data->eventID), 0, nullptr);
            }
            return;
        }
        index = this->mIndex.GetNextValidIndex(index + 1);
    }
}

void Cache::FlushUncached() {
    unsigned int index = this->mIndex.GetNextValidIndex(0);
    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        if (!sample->lock && sample->ready && !sample->cached) {
            if (PRINT_SPEECH_CACHE_IO) {
                Attrib::Gen::speech speech(Manager::GetHashIDMap().GetHash(sample->eventID), 0, nullptr);
            }
            SpeechSampleData::Destruct(sample);
            this->mIndex.DeleteIndex(index);
        }
        index = this->mIndex.GetNextValidIndex(index + 1);
    }

    ++flushcount_uncached;
}

void Cache::FlushLRU() {
    bLargestMalloc(SpeechMemoryPool);

    unsigned int prelargest = 0;
    unsigned int index = this->mIndex.GetNextValidIndex(prelargest);
    unsigned int postlargest = 0;

    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        if (!sample->lock) {
            if (sample->ready && sample->cached && sample->age < 2) {
                SpeechSampleData::Destruct(sample);
                this->mIndex.DeleteIndex(index);
            }
        }
        index = this->mIndex.GetNextValidIndex(index + 1);
    }

    bLargestMalloc(SpeechMemoryPool);
    ++flushcount_lru;
}

void Cache::FlushAllUnlocked() {
    bLargestMalloc(SpeechMemoryPool);

    unsigned int prelargest = 0;
    unsigned int index = this->mIndex.GetNextValidIndex(prelargest);
    unsigned int postlargest = 0;

    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        if (!sample->lock && sample->ready && sample->cached) {
            SpeechSampleData::Destruct(sample);
            this->mIndex.DeleteIndex(index);
        }

        index++;
        index = this->mIndex.GetNextValidIndex(index);
    }

    bLargestMalloc(SpeechMemoryPool);
    ++flushcount_all;
}

void Cache::FlushInactiveSpeakers() {
    bLargestMalloc(SpeechMemoryPool);

    unsigned int prelargest = 0;
    unsigned int index = this->mIndex.GetNextValidIndex(prelargest);
    unsigned int postlargest = 0;

    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        if (!sample->lock && sample->ready && sample->cached) {
            bool inactive = true;
            for (VoiceIDs::iterator i = mSpeakers->begin(); i != this->mSpeakers->end(); i++) {
                if ((*i == sample->speakerID) || (sample->speakerID == 0xffff)) {
                    inactive = false;
                }
            }

            if (inactive) {
                SpeechSampleData::Destruct(sample);
                this->mIndex.DeleteIndex(index);
            }
        }

        index++;
        index = this->mIndex.GetNextValidIndex(index);
    }

    bLargestMalloc(SpeechMemoryPool);
    ++flushcount_inactive_spkrs;
}

void Cache::DebugPrintAllocations() {
    bMemoryPrintAllocations(SpeechMemoryPool, 0, 0x7fffffff);

    unsigned int index = this->mIndex.GetNextValidIndex(0);
    while (this->mIndex.ValidIndex(index)) {
        SpeechSampleData *sample = this->mIndex.GetPtrAtIndex(index);
        Attrib::Gen::speech speech(Manager::GetHashIDMap().GetHash(sample->eventID), 0, nullptr);
        index = this->mIndex.GetNextValidIndex(index + 1);
    }
}

void Cache::Validate() {}

void Cache::DebugPrints() {}

}; // namespace Speech
