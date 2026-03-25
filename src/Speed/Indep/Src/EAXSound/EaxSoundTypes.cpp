#include <types.h>

#include <algorithm>
#include <new>

#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

enum SPCHType_1_EventID {
    kSPCH1_EventID_MaxEventID = 0x108,
};

struct SPCHType_1_EventSpec {
    unsigned short eventID;
    unsigned short _pad;
};

struct SPCHType_SampleRequestData {
    int bankNum;
    int sampleOffset;
    unsigned int numBytes;
    SPCHType_1_EventSpec eventSpec;
    int channel;
    int subID;
    int datID;
    int interruptFlag;
};

extern void *NullPointer;

struct EAXCop;
struct ISndAttachable;

#include "Speed/Indep/Src/Generated/AttribSys/Classes/speech.h"

namespace Speech {

struct ScheduledSpeechEvent;

struct SPCHSampleRequest {
    SPCHType_SampleRequestData data;
    ScheduledSpeechEvent *owner;
    unsigned int offset;
    unsigned char sample_index;

    bool operator<(const SPCHSampleRequest &from) const;
};

struct History {
    Timer time;
    unsigned short count;
    unsigned short speakers;
};

struct HistoryPair {
    SPCHType_1_EventID id;
    History history;

    bool operator<(const HistoryPair &rhs) const {
        return id < rhs.id;
    }
};

struct SpeechEventPair {
    unsigned int hash;
    SPCHType_1_EventID id;

    bool operator<(const SpeechEventPair &rhs) const {
        if (id != rhs.id) {
            return id < rhs.id;
        }
        return hash < rhs.hash;
    }
};

struct copPair {
    HSIMABLE hsimable;
    EAXCop *cop;

    bool operator<(const copPair &rhs) const {
        return hsimable < rhs.hsimable;
    }
};

DECLARE_CONTAINER_TYPE(copMap);

class copMap : public UTL::Std::vector<copPair, _type_copMap> {
  public:
    copMap(int size) {}
    void Add(HSIMABLE__ *hsimable, EAXCop *cop);
    EAXCop *Remove(HSIMABLE__ *hsimable);
    void ModifyHandle(HSIMABLE__ *hsimable, HSIMABLE__ *newhandle);
    EAXCop *Find(HSIMABLE__ *hsimable) const;
};

class SpeechHashIDMap : public UTL::FixedVector<SpeechEventPair, 264, 16>, public AudioMemBase {
  public:
    SpeechHashIDMap() {}
    void Add(unsigned int hash, SPCHType_1_EventID id);
    SPCHType_1_EventID GetID(unsigned int hash);
    unsigned int GetHash(SPCHType_1_EventID id);
};

class EventHistory : public UTL::FixedVector<HistoryPair, 264, 16>, public AudioMemBase {
  public:
    EventHistory() {}
    void Init();
    History *Find(SPCHType_1_EventID id);
    int GetCount(SPCHType_1_EventID id);
    Timer GetTime(SPCHType_1_EventID id);
    History *Touch(SPCHType_1_EventID id, unsigned short speaker);
    void Reset();
};

struct SpeechSampleData {
    unsigned int size;
    bool ready;
    int age;
    int speakerID;
    SPCHType_1_EventID eventID;
    int HSTRM;
    bool lock;
    bool cached;
    Timer t_req;
    Timer t_load;
    Timer t_play;
    unsigned int dataoffset;

    void Lock();
    void Unlock();

    static void Destruct(SpeechSampleData *ptr);
    static SpeechSampleData *Construct(SPCHType_SampleRequestData *data, unsigned int key, bool is_cached);
};

DECLARE_CONTAINER_TYPE(SampleReqList);

struct SampleReqList : public UTL::Std::vector<SPCHSampleRequest, _type_SampleReqList>, public AudioMemBase {};

struct ScheduledSpeechEvent {
    void *iid;
    void *fh;
    SPCHType_1_EventID ID;
    void *actor;
    Timer entry_time;
    Timer playback_time;
    Timer finish_time;
    SpeechSampleData *assoc_samples[7];
    unsigned char assoc_samples_count;
    unsigned char assoc_samples_prep;
    unsigned char curndx;
    unsigned char priority;
    short frameindex;
    short flags;

    ScheduledSpeechEvent();
    ~ScheduledSpeechEvent();

    static void *operator new(unsigned int base_size, unsigned int xtra);
    static void operator delete(void *ptr);

    static bool sort_nested_priority(const ScheduledSpeechEvent *lhs, const ScheduledSpeechEvent *rhs);
    void AddSample(SpeechSampleData *sample, unsigned char specific_index);
    void *GetData(unsigned int *datasize);
    unsigned char ReserveSample();
};

static short sSpeechFrameIndex = 0;

Attrib::Key Attrib::Gen::speech::ClassKey() {
    return 0xc593dd47;
}

void copMap::Add(HSIMABLE__ *hsimable, EAXCop *cop) {
    copPair pair;
    pair.hsimable = hsimable;
    pair.cop = cop;
    insert(std::upper_bound(begin(), end(), pair), pair);
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
    for (iterator it = begin(); it != end(); ++it) {
        if (it->hsimable == hsimable) {
            it->hsimable = newhandle;
            break;
        }
    }
    std::sort(begin(), end());
}

EAXCop *copMap::Find(HSIMABLE__ *hsimable) const {
    copPair pair;
    pair.hsimable = hsimable;
    pair.cop = nullptr;

    const_iterator it = std::lower_bound(begin(), end(), pair);
    if (it != end() && it->hsimable == hsimable) {
        return it->cop;
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
    for (iterator it = begin(); it != end(); ++it) {
        if (it->hash == hash) {
            return it->id;
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
    for (iterator it = begin(); it != end(); ++it) {
        it->history.count = 0;
        it->history.time = Timer(0);
        it->history.speakers = 0;
    }
}

void SpeechSampleData::Destruct(SpeechSampleData *ptr) {
    gSpeechCache.Free(ptr);
}

inline void SpeechSampleData::Lock() { *reinterpret_cast<int *>(&lock) = 1; }

inline void SpeechSampleData::Unlock() { *reinterpret_cast<int *>(&lock) = 0; }

inline SampleReqList &Manager::GetSampleRequests() { return mSampleRequests; }

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

ScheduledSpeechEvent::ScheduledSpeechEvent() {
    ID = kSPCH1_EventID_MaxEventID;
    iid = nullptr;
    fh = nullptr;
    actor = nullptr;
    entry_time = WorldTimer;
    assoc_samples_count = 0;
    assoc_samples_prep = 0;
    curndx = 0;
    priority = 0;
    playback_time = WorldTimer;
    finish_time = Timer(0);
    flags = 0;
    frameindex = sSpeechFrameIndex;
    sSpeechFrameIndex = static_cast<short>(sSpeechFrameIndex + 1);

    for (int i = 0; i < 7; ++i) {
        assoc_samples[i] = nullptr;
    }
}

ScheduledSpeechEvent::~ScheduledSpeechEvent() {
    for (short i = 0; i < 7; ++i) {
        SpeechSampleData *stitch = assoc_samples[i];
        if (stitch != nullptr && *reinterpret_cast<int *>(&stitch->lock) == 1) {
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
    (void)base_size;
    (void)xtra;
    SlotPool *pool = gSpeechCache.GetEventPool();

    if (pool != nullptr && ((pool = gSpeechCache.GetEventPool()) == nullptr ||
                            !gSpeechCache.GetEventPool()->IsFull())) {
        return gSpeechCache.GetEventPool()->Malloc(1, nullptr);
    }

    return NullPointer;
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
    void *data = static_cast<void *>(this + 1);
    if (datasize != nullptr) {
        *datasize = 0x40;
    }
    return data;
}

unsigned char ScheduledSpeechEvent::ReserveSample() {
    return curndx++;
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
