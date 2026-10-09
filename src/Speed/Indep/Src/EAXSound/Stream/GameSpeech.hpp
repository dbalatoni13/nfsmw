//
//
//
//
//
//
//
//
//
//
//
//
#ifndef _GAME_SPEECH_H_
#define _GAME_SPEECH_H_ // Decl: 14

#include "Speed/Indep/Src/EAXSound/Clump.h"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"

namespace Speech {

// total size: 0xD0
// Decl: 48
class GameSpeech : public Module {
  public:
    GameSpeech();
    ~GameSpeech() override;

    // Overrides: Module
    void Init(int channel) override;
    void LoadBanks() override;
    int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    SPCHType_EventRuleResult EventRuleCallback(int eventID) override;
    void Update() override;
    unsigned int SampleRequestCallback(SPCHType_SampleRequestData *data) override;

    void IssueSampleRequests();

    static void LoadingCallback(intptr_t param, int error_status);

    // bool IsInitted() {} // Decl: 62

    // Overrides: Module
    // Decl: 63
    char *GetCSIptr() override {
        return this->m_csisData;
    }
    // Decl: 64
    int GetChannel() override {
        return this->m_channel;
    }
    // Decl: 65
    char *GetEventDat() override {
        return this->m_eventDat;
    }
    // Decl: 66
    bool IsDataLoaded() override {
        return this->TestFlag(1);
    }

    // Overrides: Module
    void ReleaseResource() override;

    // Decl: 74
    bool IsBusy() {
        return this->TestFlag(8);
    }
    // Decl: 75
    bool IsForcingSilence() {
        return this->TestFlag(4);
    }
    // Decl: 76
    Speech::ScheduledSpeechEvent *GetCurrentEvent() {
        return this->m_currEvent;
    }

    std::deque<int> mLoadState; // offset 0x58, size 0x30, Decl: 79

  protected:
    void IssuePlayback(ScheduledSpeechEvent *nextevent);
    void ClearCompletedRequests();
    bool ShouldPause();
    int GetVolForSpeaker(int id);
    void RadioChirp(uint8 type);
    void UpdateChirps();
    void CheckNextEvent();

    int m_currentIntensity; // offset 0x88, size 0x4, Decl: 90
    static char *m_tempCharPtr;
    static CLUMP_IDX_FILEtag *m_clumpIdx;
    int m_speechCycle; // offset 0x8C, size 0x4, Decl: 93
    static char *m_csisData;
    static int m_channel;
    static char *m_eventDat;
#ifdef EA_BUILD_A124
    SpeechSampleList *m_pendingList;
#else
    SpeechSampleVec m_pendingList; // offset 0x90, size 0x14, Decl: 97
#endif
    ScheduledSpeechEvent *m_currEvent; // offset 0xA4, size 0x4, Decl: 100
    unsigned int m_currEventTime;      // offset 0xA8, size 0x4, Decl: 101
    int m_currEventClarity;            // offset 0xAC, size 0x4, Decl: 102
    short m_currEventSpeakerID;        // offset 0xB0, size 0x2, Decl: 103
    cStichWrapper *m_Chirper;          // offset 0xB4, size 0x4, Decl: 106
    SND_Params m_ChirpParams;          // offset 0xB8, size 0x18, Decl: 107
};

}; // namespace Speech

extern float TRACKSTREAMER_BACKLOG_THRESH; // TODO where is this declared?

#endif
