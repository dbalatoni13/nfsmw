#ifndef _SPEECH_MANAGER_H_
#define _SPEECH_MANAGER_H_

#include "SpeechModule.hpp"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/EAXSound/Clump.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/speech.h"
#include "csis/csis.h"
#include "spch/spch.h"

namespace Speech {

// total size: 0x1
// Decl: 91
struct Manager {
    static void Destroy();                                              // Decl: 93
    static int SampleRequestCallback(SPCHType_SampleRequestData *data); // Decl: 94
    static void ToggleSpeech(bool status);                              // Decl: 95
    static void Init(SPEECH_MODE mode);                                 // Decl: 96
    static void Init2();                                                // Decl: 97

    static int LoadSpeechBank(CLUMP_IDX_FILE *index, int &type, int &number, SPEECH_BANK *sb);   // Decl: 100
    static int AddHeaders(char **dest, SPEECH_BANK *banks, int numBanks, Module *module);        // Decl: 101
    static void AttachSFXOBJ(SpeechModuleIndex idx, SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype); // Decl: 102
    static bool IsPlaying(SpeechModuleIndex idx);                                                // Decl: 103
    static void SetSpeechMode(SPEECH_MODE mode) {}                                               // Decl: 104
    static SPEECH_MODE GetSpeechMode() {}                                                        // Decl: 105
    static bool SpeechDisabled() {}                                                              // Decl: 106
    static int GetChannel(SpeechModuleIndex x) {}                                                // Decl: 107
    static struct Timer GetTimeSinceLastEvent(SpeechModuleIndex idx);                            // Decl: 108
    static float GetDeadAir() {}                                                                 // Decl: 109
    static void PurgeSpeech(int channel);                                                        // Decl: 110
    static void Update(float t);                                                                 // Decl: 111
    static void DoNothing();                                                                     // Decl: 112
    static float GetStep() {}                                                                    // Decl: 113
    static Module *GetSpeechModule(int nindex);                                                  // Decl: 114

    static void SpchLibAbort(const char *format); // Decl: 117
    static int GetTicker();                       // Decl: 118

    static int TestSentenceRuleCallback(EventSpec *eventInfo, int ruleID, int parmValue, int userNum); // Decl: 128
    static void SetSentenceRuleCallback(EventSpec *eventInfo, int ruleID, int parmValue, int userNum); // Decl: 129
    static SPCHType_EventRuleResult EventRuleCallback(EventSpec *eventInfo);                           // Decl: 130

    static int ReparmCallback(int ruleID, unsigned int *parms); // Decl: 131

    static void Deduce(); // Decl: 135

    // static SpeechValRtnType PreValidate(ScheduledSpeechEvent &evt); // Decl: 136
    // static SpeechValRtnType PostValidate(ScheduledSpeechEvent *evt, unsigned int mask); // Decl: 137
    // static  ScheduledSpeechEvent *GetNextEvent(); // Decl: 138
    // static float IsEventDead( ScheduledSpeechEvent *evt); // Decl: 139

    // static bool IsCacheable(SPCHType_1_EventID event_id); // Decl: 142
    // static ScheduledSpeechEvent *GetCurrentSpeechEvent() {} // Decl: 143

    // static ScheduledSpeechEvent *GetCorrelatedEvent(SpeechSampleData *sample); // Decl: 145
    // static SampleReqList &GetSampleRequests() {} // Decl: 146

    // static ScheduledSpeechEvent *ScheduleSpeechPartII(unsigned int size, void *data, const Csis::InterfaceId &iid, Csis::FunctionHandle &fh,
    //                                                   EAXCharacter *actor); // Decl: 150
    // static Csis::Result IndirectSpeechEvent(ScheduledSpeechEvent *evt, bool test_only); // Decl: 151
    // static void NotifyEventCompletion(ScheduledSpeechEvent *evt, bool playback_complete); // Decl: 152
    static bool HasBeenSaid(SPCHType_1_EventID event_id); // Decl: 153
    static bool IsCopSpeechBusy();                        // Decl: 154
    // static EventHistory &GetHistory() {}                  // Decl: 155
    // static SpeechHashIDMap &GetHashIDMap() {}             // Decl: 156
    static SPCHType_1_EventID GetLastEventID() {} // Decl: 157
    // static void Expire(ScheduledSpeechEvent *event);     // Decl: 158
    static void ClearPlayback();                                 // Decl: 159
    static void ResetGlobalHistory();                            // Decl: 160
    static bool IsQueued(SPCHType_1_EventID evtID, int indices); // Decl: 161
    static bool IsCopSpeechPlaying(SPCHType_1_EventID event_id); // Decl: 162
    static int FlushSpeechForActor(EAXCharacter *actor);         // Decl: 163
    static short GetLastSpeakerID() {}                           // Decl: 164

  private:
    static bool RecallSpeechEvent(SPCHType_1_EventID recall_id); // Decl: 165
    static short m_frameindex;                                   // size: 0x2, address: 0x804359B4, Decl: 166

    static void Speech_Done();     // Decl: 180
    static void PopulateHashMap(); // Decl: 181

    static void ServiceFilteredEvents();                         // Decl: 183
    static bool ServiceInterruptEvents();                        // Decl: 184
    static void CalcProbPlayback();                              // Decl: 185
    static bool CanPlayback(Attrib::Gen::speech &event_attribs); // Decl: 186

    template <typename T> static void ScheduleSpeech(T &data, const Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor);

    static Module *m_SpeechModule[2];                  // size: 0x8, address: 0x80435980, Decl: 188
    static enum SPEECH_MODE m_speechMode;              // size: 0x4, address: 0x80435988, Decl: 192
    static int m_numberSpeechBanks;                    // size: 0x4, address: 0x8043598C, Decl: 193
    static bool m_SPEECH_initted;                      // size: 0x1, address: 0x80435990, Decl: 194
    static char *m_SPEECH_bankPtrMem;                  // size: 0x4, address: 0x80435994, Decl: 195
    static bool m_speechDisable;                       // size: 0x1, address: 0x80435998, Decl: 196
    static float m_clock_in_ms;                        // size: 0x4, address: 0x804359A4, Decl: 197
    static int m_gameSpeechInitted;                    // size: 0x4, address: 0x8043599C, Decl: 198
    static int m_NISAudioInitted;                      // size: 0x4, address: 0x804359A0, Decl: 199
    static float m_timestep;                           // size: 0x4, address: 0x804359A8, Decl: 201
    static float m_deadair;                            // size: 0x4, address: 0x804359AC, Decl: 202
    static float mProbPlayback;                        // size: 0x4, address: 0x804359B8, Decl: 203
    static struct EventHistory mGlobalHistory;         // size: 0xC74, address: 0x80499484, Decl: 207 // TODO
    static struct SPCHEventList mEvtHistory;           // size: 0xC, address: 0x80498C24, Decl: 208 // TODO
    static struct SchedSpchEvents mEvents[4];          // size: 0x50, address: 0x80498BD4, Decl: 209 // TODO
    static struct SpeechHashIDMap mHashMap;            // size: 0x854, address: 0x80498C30, Decl: 210 // TODO
    static struct ScheduledSpeechEvent *mCurrentEvent; // size: 0x4, address: 0x804359B0, Decl: 211 // TODO
    static struct SampleReqList mSampleRequests;       // size: 0x14, address: 0x8049A0F8, Decl: 212 // TODO
    static Timer mSampleReqTimer;                      // size: 0x4, address: 0x8049A10C, Decl: 213
    static short mLastSpeakerID;                       // size: 0x2, address: 0x804359BC, Decl: 214
};

}; // namespace Speech

// Decl: 225
#define SCHEDULE_SPEECH(_EVENT_, _DATA_, _CALLER_)                                                                                                   \
    Manager::ScheduleSpeech<Csis::_EVENT_##Struct>(_DATA_, Csis::_EVENT_##Id, Csis::g##_EVENT_##Handle, _CALLER_)

#endif
