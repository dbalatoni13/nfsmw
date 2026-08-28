#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/Speech/EAXCop.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

// TODO where is this?
Sound::SongInfoList Songs;

namespace Speech {

void copMap::Add(HSIMABLE hsimable, EAXCop *cop) {
    copPair p;
    p.hsimable = hsimable;
    p.cop = cop;
    iterator iter = std::upper_bound(this->begin(), this->end(), p);
    this->insert(iter, p);
}

EAXCop *copMap::Remove(HSIMABLE hsimable) {
    EAXCop *result = nullptr;
    copPair p;
    p.hsimable = hsimable;
    p.cop = nullptr;

    iterator iter = std::lower_bound(this->begin(), this->end(), p);
    if (iter != this->end() && iter->hsimable == hsimable) {
        result = iter->cop;
        this->erase(iter);
    }

    return result;
}

void copMap::ModifyHandle(HSIMABLE hsimable, HSIMABLE newhandle) {
    iterator iter;
    for (iter = this->begin(); iter != this->end(); ++iter) {
        if (iter->hsimable == hsimable) {
            iter->hsimable = newhandle;
            break;
        }
    }
    std::sort(this->begin(), this->end());
}

EAXCop *copMap::Find(HSIMABLE hsimable) const {
    copPair p;
    p.hsimable = hsimable;
    p.cop = nullptr;

    const_iterator iter = std::lower_bound(this->begin(), this->end(), p);
    if (iter != this->end() && iter->hsimable == hsimable) {
        return iter->cop;
    }

    return nullptr;
}

void SpeechHashIDMap::Add(Attrib::Key hash, SPCHType_1_EventID id) {
    SpeechEventPair p;
    p.hash = hash;
    p.id = id;

    iterator iter = std::upper_bound(this->begin(), this->end(), p);
    this->insert(iter, p);
}

SPCHType_1_EventID SpeechHashIDMap::GetID(unsigned int hash) {
    for (const_iterator i = this->begin(); i != this->end(); ++i) {
        const SpeechEventPair &p = *i;
        if (p.hash == hash) {
            return p.id;
        }
    }

    return kSPCH1_EventID_MaxEventID;
}

Attrib::Key SpeechHashIDMap::GetHash(SPCHType_1_EventID id) {
    SpeechEventPair p;
    p.hash = 0;
    p.id = id;

    const_iterator iter = std::lower_bound(this->begin(), this->end(), p);
    if (iter != this->end() && iter->id == id) {
        return iter->hash;
    }

    return 0;
}

void EventHistory::Init() {
    const Attrib::Class *speechevents = Attrib::Database::Get().GetClass(Attrib::ClassName::speech);
    Attrib::Key eventkey = speechevents->GetFirstCollection();
    while (eventkey != 0) {
        HistoryPair p;
        Attrib::Gen::speech event_collection(eventkey, 0, nullptr);
        if (event_collection.SpeechID(p.id)) {
            iterator iter = std::upper_bound(this->begin(), this->end(), p);
            this->insert(iter, p);
        }
        eventkey = speechevents->GetNextCollection(eventkey);
    }

    this->Reset();
}

History *EventHistory::Find(SPCHType_1_EventID id) {
    HistoryPair p;
    p.id = id;

    iterator i = std::lower_bound(this->begin(), this->end(), p);
    if (i != this->end() && i->id == id) {
        return &i->history;
    }

    return nullptr;
}

int EventHistory::GetCount(SPCHType_1_EventID id) {
    History *hist = this->Find(id);
    if (hist == nullptr) {
        return -1;
    }

    return hist->count;
}

Timer EventHistory::GetTime(SPCHType_1_EventID id) {
    History *hist = this->Find(id);
    if (hist != nullptr) {
        return hist->time;
    }

    return Timer(0);
}

History *EventHistory::Touch(SPCHType_1_EventID id, unsigned short speaker) {
    History *hist = this->Find(id);
    if (hist == nullptr) {
        return nullptr;
    }

    hist->Touch(speaker);
    return hist;
}

void EventHistory::Reset() {
    for (iterator i = this->begin(); i != this->end(); ++i) {
        History &hist = i->history;
        hist.count = 0;
        hist.time = Timer(0);
        hist.speakers = 0;
    }
}

void SpeechSampleData::Destruct(SpeechSampleData *ptr) {
    ptr->~SpeechSampleData();
    gSpeechCache.Free(ptr);
}

SpeechSampleData *SpeechSampleData::Construct(SPCHType_SampleRequestData *data, unsigned int key, bool is_cached) {
    unsigned int total;
    if (is_cached) {
        total = data->numBytes + 0x40; // TODO magic
    } else {
        total = 0x40;
    }

    void *ptr = gSpeechCache.Alloc(total, key);
    if (ptr == nullptr) {
        return nullptr;
    }

    return new (ptr) SpeechSampleData(data, is_cached);
}

ScheduledSpeechEvent::ScheduledSpeechEvent()
    : iid(nullptr),                      //
      fh(nullptr),                       //
      ID(kSPCH1_EventID_MaxEventID),     //
      actor(nullptr),                    //
      entry_time(WorldTimer),            //
      playback_time(WorldTimer),         //
      finish_time(0),                    //
      assoc_samples_count(0),            //
      assoc_samples_prep(0),             //
      curndx(0),                         //
      priority(0),                       //
      frameindex(Manager::m_frameindex), //
      flags(0) {
    Manager::m_frameindex++;

    for (short i = 0; i < NUM_ELEMENTS(this->assoc_samples); i++) {
        this->assoc_samples[i] = nullptr;
    }
}

ScheduledSpeechEvent::~ScheduledSpeechEvent() {
    for (short i = 0; i < NUM_ELEMENTS(this->assoc_samples); ++i) {
        SpeechSampleData *stitch = this->assoc_samples[i];
        if (stitch != nullptr && stitch->lock == true) {
            stitch->Unlock();
        }
        this->assoc_samples[i] = nullptr;
    }

    this->assoc_samples_prep = 0;
    this->curndx = 0;
    SampleReqList &requests = Manager::GetSampleRequests();
    if (requests.size() != 0) {
        for (SPCHSampleRequest *i = requests.begin(); i != requests.end();) {
            if (i->owner == this) {
                requests.erase(i);
            } else {
                ++i;
            }
        }
    }
}

void *ScheduledSpeechEvent::operator new(size_t base_size, size_t xtra) {
    size_t total = base_size + xtra;

    if (gSpeechCache.GetEventPool() == nullptr) {
        return NullPointer;
    }
    if (gSpeechCache.GetEventPool() != nullptr && gSpeechCache.GetEventPool()->IsFull()) {
        return NullPointer;
    }
    return gSpeechCache.GetEventPool()->Malloc(1, nullptr);
}

void ScheduledSpeechEvent::operator delete(void *ptr) {
    gSpeechCache.GetEventPool()->Free(ptr);
}

bool ScheduledSpeechEvent::sort_nested_priority(const ScheduledSpeechEvent *lhs, const ScheduledSpeechEvent *rhs) {
    if (lhs->priority == rhs->priority) {
        if (lhs->entry_time == rhs->entry_time) {
            return lhs->frameindex < rhs->frameindex;
        }
        return (lhs->entry_time < rhs->entry_time) != 0;
    }
    return lhs->priority > rhs->priority;
}

void *ScheduledSpeechEvent::GetData(unsigned int *datasize) {
    size_t ptr = reinterpret_cast<size_t>(this + 1);
    if (datasize != nullptr) {
        *datasize = sizeof(ScheduledSpeechEvent);
    }
    return reinterpret_cast<void *>(ptr);
}

unsigned char ScheduledSpeechEvent::ReserveSample() {
    uint8 requested_index = this->curndx;
    this->curndx++;
    return requested_index;
}

void ScheduledSpeechEvent::AddSample(SpeechSampleData *sample, unsigned char specific_index) {
    sample->Lock();
    if (specific_index != 0xFF) {
        this->assoc_samples[specific_index] = sample;
        return;
    }

    this->assoc_samples[this->curndx] = sample;
    this->curndx = static_cast<unsigned char>(this->curndx + 1);
}

}; // namespace Speech
