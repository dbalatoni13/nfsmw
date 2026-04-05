#include <types.h>

#include <algorithm>
#include <new>

#include "Speed/Indep/Src/EAXSound/EAXSoundTypes.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Speech/EAXCop.h"

extern void *NullPointer;
struct ISndAttachable;

#include "Speed/Indep/Src/Generated/AttribSys/Classes/speech.h"

namespace Speech {

struct History {
    Timer time;            // offset 0x0, size 0x4
    unsigned short count;  // offset 0x4, size 0x2
    unsigned short speakers; // offset 0x6, size 0x2
};

struct HistoryPair {
    SPCHType_1_EventID id; // offset 0x0, size 0x4
    History history;       // offset 0x4, size 0x8

    bool operator<(const HistoryPair &rhs) const {
        return id < rhs.id;
    }
};

struct SpeechEventPair {
    unsigned int hash;     // offset 0x0, size 0x4
    SPCHType_1_EventID id; // offset 0x4, size 0x4

    bool operator<(const SpeechEventPair &rhs) const {
        return id < rhs.id;
    }
};

struct SpeechHashIDMap : public UTL::FixedVector<SpeechEventPair, 264, 16>, public AudioMemBase {
  public:
    SpeechHashIDMap() {}
    void Add(unsigned int hash, SPCHType_1_EventID id);
    SPCHType_1_EventID GetID(unsigned int hash);
    unsigned int GetHash(SPCHType_1_EventID id);
};

struct EventHistory : public UTL::FixedVector<HistoryPair, 264, 16>, public AudioMemBase {
  public:
    EventHistory() {}
    void Init();
    History *Find(SPCHType_1_EventID id);
    int GetCount(SPCHType_1_EventID id);
    Timer GetTime(SPCHType_1_EventID id);
    History *Touch(SPCHType_1_EventID id, unsigned short speaker);
    void Reset();
};

short Manager::m_frameindex = 0;

Attrib::Key Attrib::Gen::speech::ClassKey() {
    return 0xc593dd47;
}

void copMap::Add(HSIMABLE__ *hsimable, EAXCop *cop) {
    copPair p;
    p.hsimable = hsimable;
    p.cop = cop;
    iterator iter = std::upper_bound(begin(), end(), p);
    insert(iter, p);
}

EAXCop *copMap::Remove(HSIMABLE__ *hsimable) {
    copPair pair;
    pair.hsimable = hsimable;
    pair.cop = nullptr;

    iterator it = std::lower_bound(begin(), end(), pair);
    if (it != end() && it->hsimable == hsimable) {
        EAXCop *cop = it->cop;
        erase(it);
        return cop;
    }

    return nullptr;
}

void copMap::ModifyHandle(HSIMABLE__ *hsimable, HSIMABLE__ *newhandle) {
    iterator iter;
    for (iter = begin(); iter != end(); ++iter) {
        if (iter->hsimable == hsimable) {
            iter->hsimable = newhandle;
            break;
        }
    }
    std::sort(begin(), end());
}

EAXCop *copMap::Find(HSIMABLE__ *hsimable) const {
    copPair p;
    p.hsimable = hsimable;
    p.cop = nullptr;

    const_iterator iter = std::lower_bound(begin(), end(), p);
    if (iter != end() && iter->hsimable == hsimable) {
        return iter->cop;
    }

    return nullptr;
}

void SpeechHashIDMap::Add(unsigned int hash, SPCHType_1_EventID id) {
    SpeechEventPair pair;
    pair.hash = hash;
    pair.id = id;
    insert(std::upper_bound(begin(), end(), pair), pair);
}

SPCHType_1_EventID SpeechHashIDMap::GetID(unsigned int hash) {
    for (const_iterator i = begin(); i != end(); ++i) {
        const SpeechEventPair &p = *i;
        if (p.hash == hash) {
            return p.id;
        }
    }

    return kSPCH1_EventID_MaxEventID;
}

unsigned int SpeechHashIDMap::GetHash(SPCHType_1_EventID id) {
    SpeechEventPair p;
    p.hash = 0;
    p.id = id;

    const_iterator iter = std::lower_bound(begin(), end(), p);
    if (iter != end() && iter->id == id) {
        return iter->hash;
    }

    return 0;
}

void EventHistory::Init() {
    Attrib::Class *speechClass = Attrib::Database::Get().GetClass(0xC593DD47);
    unsigned int collectionKey = speechClass->GetFirstCollection();
    while (collectionKey != 0) {
        HistoryPair pair;
        pair.id = kSPCH1_EventID_MaxEventID;
        pair.history.time = Timer(0);
        pair.history.count = 0;
        pair.history.speakers = 0;

        const Attrib::Collection *collection = Attrib::FindCollection(Attrib::Gen::speech::ClassKey(), collectionKey);
        Attrib::Gen::speech event_collection(collection, 0, nullptr);
        pair.id = event_collection.SpeechID();
        insert(std::upper_bound(begin(), end(), pair), pair);
        collectionKey = speechClass->GetNextCollection(collectionKey);
    }

    Reset();
}

History *EventHistory::Find(SPCHType_1_EventID id) {
    HistoryPair pair;
    pair.id = id;
    pair.history.time = Timer(0);
    pair.history.count = 0;
    pair.history.speakers = 0;

    iterator it = std::lower_bound(begin(), end(), pair);
    if (it != end() && it->id == id) {
        return &it->history;
    }

    return nullptr;
}

int EventHistory::GetCount(SPCHType_1_EventID id) {
    History *hist = Find(id);
    if (hist == nullptr) {
        return -1;
    }

    return hist->count;
}

Timer EventHistory::GetTime(SPCHType_1_EventID id) {
    History *hist = Find(id);
    if (hist == nullptr) {
        return Timer(0);
    }

    return hist->time;
}

History *EventHistory::Touch(SPCHType_1_EventID id, unsigned short speaker) {
    History *hist = Find(id);
    if (hist != nullptr) {
        hist->time = WorldTimer;
        hist->count = hist->count + 1;
        if (speaker < 10 && ((hist->speakers >> speaker) & 1) == 0) {
            hist->speakers = static_cast<unsigned short>(hist->speakers | (1 << speaker));
        }
    }

    return hist;
}

void EventHistory::Reset() {
    for (iterator i = begin(); i != end(); ++i) {
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
    (void)key;
    unsigned int total = 0x40;
    if (is_cached) {
        total = data->numBytes + 0x40;
    }

    SpeechSampleData *sample = static_cast<SpeechSampleData *>(::operator new(total));
    if (sample != nullptr) {
        sample->size = data->numBytes;
        sample->ready = false;
        sample->age = 0;
        sample->speakerID = data->subID;
        sample->eventID = static_cast<SPCHType_1_EventID>(data->eventSpec.eventID);
        sample->HSTRM = -1;
        sample->Lock();
        sample->cached = is_cached;
        sample->t_req = WorldTimer;
        sample->t_load = Timer(0);
        sample->t_play = Timer(0);
        sample->dataoffset = 0;
    }

    return sample;
}

ScheduledSpeechEvent::ScheduledSpeechEvent()
    : iid(nullptr), //
      fh(nullptr), //
      ID(kSPCH1_EventID_MaxEventID), //
      actor(nullptr), //
      entry_time(WorldTimer), //
      playback_time(WorldTimer), //
      finish_time(0), //
      assoc_samples_count(0), //
      assoc_samples_prep(0), //
      curndx(0), //
      priority(0), //
      frameindex(Manager::m_frameindex), //
      flags(0) {
    Manager::m_frameindex = static_cast<short>(Manager::m_frameindex + 1);

    for (short i = 0; i < 7; i = static_cast<short>(i + 1)) {
        assoc_samples[i] = nullptr;
    }
}

ScheduledSpeechEvent::~ScheduledSpeechEvent() {
    for (short i = 0; i < 7; ++i) {
        SpeechSampleData *stitch = assoc_samples[i];
        if (stitch != nullptr && stitch->lock == true) {
            stitch->Unlock();
        }
        assoc_samples[i] = nullptr;
    }

    assoc_samples_prep = 0;
    curndx = 0;
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

void *ScheduledSpeechEvent::operator new(unsigned int base_size, unsigned int xtra) {
    unsigned int total = base_size + xtra;
    (void)total;

    if (!gSpeechCache.GetEventPool()) {
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
        return lhs->entry_time < rhs->entry_time;
    }
    return rhs->priority < lhs->priority;
}

void ScheduledSpeechEvent::AddSample(SpeechSampleData *sample, unsigned char specific_index) {
    sample->Lock();
    if (specific_index != 0xFF) {
        assoc_samples[specific_index] = sample;
        return;
    }

    assoc_samples[curndx] = sample;
    curndx = static_cast<unsigned char>(curndx + 1);
}

void *ScheduledSpeechEvent::GetData(unsigned int *datasize) {
    unsigned int ptr = reinterpret_cast<unsigned int>(this + 1);
    if (datasize != nullptr) {
        *datasize = 0x40;
    }
    return reinterpret_cast<void *>(ptr);
}

unsigned char ScheduledSpeechEvent::ReserveSample() {
    unsigned char requested_index = curndx;
    curndx++;
    return requested_index;
}

} // namespace Speech

namespace _STL {
template Speech::SpeechEventPair *__lower_bound<Speech::SpeechEventPair *, Speech::SpeechEventPair,
                                                _STL::less<Speech::SpeechEventPair>, int>(
    Speech::SpeechEventPair *, Speech::SpeechEventPair *, Speech::SpeechEventPair const &, _STL::less<Speech::SpeechEventPair>, int *);

template _STL::_List_iterator<int, _STL::_Nonconst_traits<int> > find<_STL::_List_iterator<int, _STL::_Nonconst_traits<int> >, int>(
    _STL::_List_iterator<int, _STL::_Nonconst_traits<int> >, _STL::_List_iterator<int, _STL::_Nonconst_traits<int> >, int const &);

template ISndAttachable **find<ISndAttachable **, ISndAttachable *>(
    ISndAttachable **, ISndAttachable **, ISndAttachable * const &);
}
