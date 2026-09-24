#ifndef _SPEECH_MODULE_H_
#define _SPEECH_MODULE_H_

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "spch/spch.h"

namespace Speech {

// total size: 0x30
// Decl: 31
struct SpeechSampleData {
    // Decl: 32
    SpeechSampleData(SPCHType_SampleRequestData *data, bool is_cached)
        : size(data->numBytes),                                              //
          ready(false),                                                      //
          age(0),                                                            //
          speakerID(data->subID),                                            //
          eventID(static_cast<SPCHType_1_EventID>(data->eventSpec.eventID)), //
          HSTRM(-1),                                                         //
          lock(true),                                                        //
          cached(is_cached),                                                 //
          t_req(WorldTimer),                                                 //
          t_load(0),                                                         //
          t_play(0),                                                         //
          dataoffset(0) {}

    ~SpeechSampleData() {} // Decl: 48

    // Decl: 52
    void *GetData() {
        // TODO
        uintptr_t ptr = reinterpret_cast<uintptr_t>(this);
        return reinterpret_cast<void *>(ptr + 0x40);
    }

    static void Destruct(SpeechSampleData *ptr);                                                                   // Decl: 59
    static Speech::SpeechSampleData *Construct(SPCHType_SampleRequestData *data, Attrib::Key key, bool is_cached); // Decl: 60

    // Decl: 63
    void Lock() {
        this->lock = true;
    }
    // Decl: 64
    void Unlock() {
        this->lock = false;
    }

    unsigned int size;          // offset 0x0, size 0x4, Decl: 66
    bool ready;                 // offset 0x4, size 0x1, Decl: 67
    int age;                    // offset 0x8, size 0x4, Decl: 68
    int speakerID;              // offset 0xC, size 0x4, Decl: 69
    SPCHType_1_EventID eventID; // offset 0x10, size 0x4, Decl: 70
    int HSTRM;                  // offset 0x14, size 0x4, Decl: 71
    bool lock;                  // offset 0x18, size 0x1, Decl: 72
    bool cached;                // offset 0x1C, size 0x1, Decl: 73
    Timer t_req;                // offset 0x20, size 0x4, Decl: 74
    Timer t_load;               // offset 0x24, size 0x4, Decl: 75
    Timer t_play;               // offset 0x28, size 0x4, Decl: 76
    unsigned int dataoffset;    // offset 0x2C, size 0x4, Decl: 77
};

#ifdef EA_BUILD_A124
DECLARE_CONTAINER_TYPE(SpeechSampleList);

// total size: 0x14
class SpeechSampleList : public UTL::Std::list<Speech::SpeechSampleData *, Speech::_type_SpeechSampleList>, public AudioMemBase {
  public:
    SpeechSampleList() {}

#else

DECLARE_CONTAINER_TYPE(SpeechSampleVec);

// total size: 0x14
// Decl: 81
class SpeechSampleVec : public UTL::Std::vector<Speech::SpeechSampleData *, Speech::_type_SpeechSampleVec>, public AudioMemBase {
  public:
    SpeechSampleVec() {}
#endif

    SpeechSampleData *Find(int handle) const {
        const_iterator i = this->begin();
        while (i != this->end()) {
            if ((*i)->HSTRM == handle) {
                return *i;
            }
            ++i;
        }

        return nullptr;
    }
};

// total size: 0x8
// Decl: 101
struct SpeechLoadCBData {
    class Module *object;   // offset 0x0, size 0x4, Decl: 102
    SpeechSampleData *data; // offset 0x4, size 0x4, Decl: 103
};

#define MAX_STITCH_SAMPLES_PER_EVENT 7 // Decl: 115

// total size: 0x40
// Decl: 124
struct ScheduledSpeechEvent {
    ScheduledSpeechEvent();  // Decl: 125
    ~ScheduledSpeechEvent(); // Decl: 126

    void *operator new(size_t base_size, size_t xtra); // Decl: 128
    void operator delete(void *ptr);                   // Decl: 129
    void operator delete(void *ptr, size_t xtra);

    static bool sort_nested_priority(const ScheduledSpeechEvent *lhs, const ScheduledSpeechEvent *rhs); // Decl: 131
    void *GetData(unsigned int *datasize);                                                              // Decl: 132
    void AddSample(Speech::SpeechSampleData *sample, uint8 specific_index);                             // Decl: 133
    uint8 ReserveSample();                                                                              // Decl: 134
    void SetData(void *data, uint32 size);                                                              // Decl: 135

    Csis::InterfaceId *iid;                                        // offset 0x0, size 0x4, Decl: 139
    Csis::FunctionHandle *fh;                                      // offset 0x4, size 0x4, Decl: 140
    SPCHType_1_EventID ID;                                         // offset 0x8, size 0x4, Decl: 141
    EAXCharacter *actor;                                           // offset 0xC, size 0x4, Decl: 142
    Timer entry_time;                                              // offset 0x10, size 0x4, Decl: 143
    Timer playback_time;                                           // offset 0x14, size 0x4, Decl: 144
    Timer finish_time;                                             // offset 0x18, size 0x4, Decl: 145
    SpeechSampleData *assoc_samples[MAX_STITCH_SAMPLES_PER_EVENT]; // offset 0x1C, size 0x1C, Decl: 146
    uint8 assoc_samples_count;                                     // offset 0x38, size 0x1, Decl: 147
    uint8 assoc_samples_prep;                                      // offset 0x39, size 0x1, Decl: 148
    uint8 curndx;                                                  // offset 0x3A, size 0x1, Decl: 149
    uint8 priority;                                                // offset 0x3B, size 0x1, Decl: 150
    short frameindex;                                              // offset 0x3C, size 0x2, Decl: 151
    short flags;                                                   // offset 0x3E, size 0x2, Decl: 152
};

typedef void (*SYNC_FUNC)(); // Decl: 155

// total size: 0x14
// Decl: 157
struct SyncAudioObject {
    // Decl: 159
    SyncAudioObject() {
        this->callback = nullptr;
        this->qsObject = nullptr;
        this->holdtime = 0;
        this->id = STRM_NONE;
        this->handle = -1;
    }

    ~SyncAudioObject() {} // Decl: 167

    SpeechSampleData *qsObject; // offset 0x0, size 0x4, Decl: 173
    SYNC_FUNC callback;         // offset 0x4, size 0x4, Decl: 174
    eNISSFX_TYPE id;            // offset 0x8, size 0x4, Decl: 175
    int handle;                 // offset 0xC, size 0x4, Decl: 176
    int16 holdtime;             // offset 0x10, size 0x2, Decl: 177
};

// total size: 0xC
// Decl: 181
struct SPEECH_BANK {
    char *mem;  // offset 0x0, size 0x4
    int bank;   // offset 0x4, size 0x4
    int offset; // offset 0x8, size 0x4
};

// total size: 0x8
// Decl: 188
struct SpeechEventPair {
    Attrib::Key hash;      // offset 0x0, size 0x4, Decl: 189
    SPCHType_1_EventID id; // offset 0x4, size 0x4, Decl: 190

    // Decl: 192
    bool operator<(const Speech::SpeechEventPair &from) const {
        return this->id < from.id;
    }
};

// total size: 0x2C
// Decl: 196
struct SPCHSampleRequest {
    SPCHType_SampleRequestData data; // offset 0x0, size 0x20, Decl: 197
    ScheduledSpeechEvent *owner;     // offset 0x20, size 0x4, Decl: 198
    unsigned int offset;             // offset 0x24, size 0x4, Decl: 199
    uint8 sample_index;              // offset 0x28, size 0x1, Decl: 200
    // Decl: 201
    bool operator<(const Speech::SPCHSampleRequest &from) const {
        return this->offset < from.offset;
    }
};

DECLARE_CONTAINER_TYPE(SchedSpchEvents);

// total size: 0x14
// Decl: 206
class SchedSpchEvents : public UTL::Std::vector<Speech::ScheduledSpeechEvent *, Speech::_type_SchedSpchEvents>, public AudioMemBase {
  public:
    SchedSpchEvents() {}
};

DECLARE_CONTAINER_TYPE(SampleReqList);

// total size: 0x14
// Decl: 214
class SampleReqList : public UTL::Std::vector<Speech::SPCHSampleRequest, Speech::_type_SampleReqList>, public AudioMemBase {
  private:
    bool ContainsEvent();
};

// total size: 0x58
// Decl: 222
class Module : public AudioMemBase {
  public:
    Module();
    ~Module() override;
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual void Init(int channel) = 0;
    virtual void LoadBanks() = 0;
    virtual int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) = 0;
    virtual int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) = 0;
    virtual SPCHType_EventRuleResult EventRuleCallback(int eventID) = 0;

    // Decl: 239
    virtual int GetNumBanks() {
        return this->m_numBanks;
    }

    virtual unsigned int GetBankOffset(int bnum);
    virtual void Update() = 0;

    // Decl: 242
    virtual const char *GetFilename() {
        return this->m_filename.GetString();
    }

    // Decl: 244
    virtual bool QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool trigger_play_after_callback) {
        return false;
    }

    // Decl: 246
    virtual unsigned int SampleRequestCallback(SPCHType_SampleRequestData *data) = 0;
    // Decl: 247
    virtual bool IsStreamQueued() {
        return this->m_bIsStreamQueued;
    }

    int GetDatID();
    int GetProjID();
    void Enable();
    void Disable();
    bool DonePlaying();
    void PurgeSpeech();
    bool PlayLastEvent(); // Decl: 263

    // Decl: 266
    Timer GetLastEventTimestamp() {
        return this->mLastEventTimestamp;
    }

#define NO_FILTER 0         // Decl: 268
#define SHORT_FILTER 1      // Decl: 269
#define VERY_SHORT_FILTER 2 // Decl: 270

    void SetFilter(int); // Decl: 273

    char GetFilter(); // Decl: 275

    virtual char *GetCSIptr() = 0;
    virtual int GetChannel() = 0;
    virtual char *GetEventDat() = 0;
    virtual bool IsDataLoaded() = 0;
    virtual bool PlayStream(int stream_id);

    void Pause();
    void UnPause();

    virtual void ReleaseResource();

    // Decl: 287
    EAXS_StreamChannel *GetStreamChannel() {
        return this->m_strm;
    }

    // Decl: 289
    // TODO figure out what the flags are
    void SetFlag(unsigned int flag) {
        this->m_flags |= flag;
    }
    // Decl: 290
    void ClearFlag(unsigned int flag) {
        this->m_flags &= ~flag;
    }
    // Decl: 291
    bool TestFlag(unsigned int flag) {
        return (this->m_flags & flag) != 0;
    }

    // Decl: 293
    SFX_Base *GetSFXOBJ_Speech() {
        return this->m_pSFXOBJ_Speech;
    }

  protected:
    bool m_enable;                     // offset 0x4, size 0x1, Decl: 296
    int m_datID;                       // offset 0x8, size 0x4
    int m_projID;                      // offset 0xC, size 0x4
    struct SPEECH_BANK *m_speechBanks; // offset 0x10, size 0x4 // TODO
    eMasterMixChannel m_mixChannel;    // offset 0x14, size 0x4
    int m_streamID;                    // offset 0x18, size 0x4
    int m_fileNum;                     // offset 0x1C, size 0x4
    char *m_bankHeaders;               // offset 0x20, size 0x4
    int m_numBanks;                    // offset 0x24, size 0x4
    unsigned int m_flags;              // offset 0x28, size 0x4, Decl: 305

    EAXS_StreamChannel *m_strm;   // offset 0x2C, size 0x4, Decl: 307
    Attrib::StringKey m_filename; // offset 0x30, size 0x10
    Timer mLastEventTimestamp;    // offset 0x40, size 0x4, Decl: 309

    SFX_Base *m_pSFXOBJ_Speech;    // offset 0x44, size 0x4, Decl: 312
    SFX_Base *m_pSFXOBJ_Moment;    // offset 0x48, size 0x4
    SFX_Base *m_pSFXOBJ_NISStream; // offset 0x4C, size 0x4
    bool m_bIsStreamQueued;        // offset 0x50, size 0x1, Decl: 318
};

}; // namespace Speech

#endif
