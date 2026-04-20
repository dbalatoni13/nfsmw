#ifndef EAXSOUND_STREAM_SPEECHMANAGER_H
#define EAXSOUND_STREAM_SPEECHMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/speech.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SpeechCache.h"
#include "SpeechModule.hpp"

struct InterfaceId;
struct FunctionHandle;
struct SlotPool;
namespace Csis {
struct InterfaceId;
struct FunctionHandle;
}

enum SpeechModuleIndex {
    NISSFX_MODULE = 0,
    COPSPEECH_MODULE = 1,
    NUM_SPEECH_MODULES = 2,
};

enum SPEECH_MODE {
    SPEECH_FRONTEND_MODE = 1,
    SPEECH_GAME_MODE = 2,
    SPEECH_SPLITSCREEN_MODE = 3,
};

struct CLUMP_IDX_FILEtag;

namespace Speech {

struct ScheduledSpeechEvent;

struct SPCHSampleRequest {
    SPCHType_SampleRequestData data; // offset 0x0, size 0x20
    ScheduledSpeechEvent *owner;     // offset 0x20, size 0x4
    unsigned int offset;             // offset 0x24, size 0x4
    unsigned char sample_index;      // offset 0x28, size 0x1

    bool operator<(const SPCHSampleRequest &from) const;
};

DECLARE_CONTAINER_TYPE(SampleReqList);

class SampleReqList : public UTL::Std::vector<SPCHSampleRequest, _type_SampleReqList>, public AudioMemBase {
  public:
};

struct SpeechSampleData {
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

    static void Destruct(SpeechSampleData *ptr);
    static SpeechSampleData *Construct(SPCHType_SampleRequestData *data, unsigned int key, bool is_cached);
};

struct ScheduledSpeechEvent {
    InterfaceId *iid;                   // offset 0x0, size 0x4
    FunctionHandle *fh;                 // offset 0x4, size 0x4
    SPCHType_1_EventID ID;              // offset 0x8, size 0x4
    EAXCharacter *actor;                // offset 0xC, size 0x4
    Timer entry_time;                   // offset 0x10, size 0x4
    Timer playback_time;                // offset 0x14, size 0x4
    Timer finish_time;                  // offset 0x18, size 0x4
    SpeechSampleData *assoc_samples[7]; // offset 0x1C, size 0x1C
    unsigned char assoc_samples_count;  // offset 0x38, size 0x1
    unsigned char assoc_samples_prep;   // offset 0x39, size 0x1
    unsigned char curndx;               // offset 0x3A, size 0x1
    unsigned char priority;             // offset 0x3B, size 0x1
    short frameindex;                   // offset 0x3C, size 0x2
    short flags;                        // offset 0x3E, size 0x2

    ScheduledSpeechEvent();
    ~ScheduledSpeechEvent();

    static void *operator new(unsigned int base_size, unsigned int xtra);
    static void operator delete(void *ptr);

    static bool sort_nested_priority(const ScheduledSpeechEvent *lhs, const ScheduledSpeechEvent *rhs);
    void AddSample(SpeechSampleData *sample, unsigned char specific_index);
    void *GetData(unsigned int *datasize);
    unsigned char ReserveSample();
};

class Manager {
  public:
    template <typename T>
    static void ScheduleSpeech(T &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) {
        ScheduleSpeechPartII(sizeof(T), &data, iid, fh, actor);
    }

    static void FlushSpeechForActor(EAXCharacter *actor);
    static int GetGlobalHistoryCount(SPCHType_1_EventID id);
    static void ScheduleSpeechPartII(unsigned int sample_size, void *sample_data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor);
    static int IndirectSpeechEvent(ScheduledSpeechEvent *evt, bool test_only);
    static int TestSentenceRuleCallback(EventSpec *event_info, int rule_id, int parm_value, int user_num);
    static int ReparmCallback(int rule_id, unsigned int *parms);
    static void SetSentenceRuleCallback(EventSpec *event_info, int rule_id, int parm_value, int user_num);
    static SPCHType_EventRuleResult EventRuleCallback(EventSpec *event_info);
    static void ClearPlayback();
    static void Init(SPEECH_MODE mode);
    static void Init(int mode) { Init(static_cast<SPEECH_MODE>(mode)); }
    static void Init2();
    static void Destroy();
    static void Deduce();
    static void Update(float dt);
    static Module *GetSpeechModule(int id);
    static void AttachSFXOBJ(SpeechModuleIndex module, SFX_Base *psb, eSFXOBJ_MAIN_TYPES type);
    static bool IsPlaying(SpeechModuleIndex module);
    static bool IsCopSpeechPlaying(SPCHType_1_EventID event);
    static bool IsCopSpeechBusy();
    static Timer GetTimeSinceLastEvent(SpeechModuleIndex module);
    static void SpchLibAbort(const char *format, ...);
    static int SampleRequestCallback(SPCHType_SampleRequestData *data);
    static int LoadSpeechBank(CLUMP_IDX_FILEtag *index, int &type, int &number, SPEECH_BANK *sb);
    static int AddHeaders(char **dest, SPEECH_BANK *banks, int numBanks, Module *module);
    static int GetTicker();
    static void PopulateHashMap();
    static bool IsCacheable(SPCHType_1_EventID event_id);
    static bool HasBeenSaid(SPCHType_1_EventID event_id);
    static bool ServiceInterruptEvents();
    static void ServiceFilteredEvents();
    static bool RecallSpeechEvent(SPCHType_1_EventID recall_id);
    static void ResetGlobalHistory();
    static void Expire(ScheduledSpeechEvent *event);
    static bool IsQueued(SPCHType_1_EventID evtID, int indices);
    static float IsEventDead(ScheduledSpeechEvent *evt);
    static void NotifyEventCompletion(ScheduledSpeechEvent *evt, bool playback_complete);
    static ScheduledSpeechEvent *GetNextEvent();
    static int PostValidate(ScheduledSpeechEvent *evt, unsigned int mask);
    static int PreValidate(ScheduledSpeechEvent &evt);
    static bool CanPlayback(Attrib::Gen::speech &event_attribs);
    static void CalcProbPlayback();
    static SampleReqList &GetSampleRequests() { return mSampleRequests; }

    static Module *m_SpeechModule[NUM_SPEECH_MODULES];
    static short m_frameindex;
    static short mLastSpeakerID;
    static SampleReqList mSampleRequests;
};

}; // namespace Speech

#endif
