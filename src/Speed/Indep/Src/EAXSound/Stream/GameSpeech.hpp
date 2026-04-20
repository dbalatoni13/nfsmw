#ifndef EAXSOUND_STREAM_GAMESPEECH_H
#define EAXSOUND_STREAM_GAMESPEECH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "SpeechModule.hpp"

struct CLUMP_IDX_FILEtag;

namespace Speech {

struct SpeechSampleData;
struct ScheduledSpeechEvent;

DECLARE_CONTAINER_TYPE(SpeechSampleVec);

struct SpeechSampleVec : public UTL::Std::vector<SpeechSampleData *, _type_SpeechSampleVec>, public AudioMemBase {};

// total size: 0xD0
struct GameSpeech : public Module {
    static char *m_tempCharPtr;
    static CLUMP_IDX_FILEtag *m_clumpIdx;
    static char *m_csisData;
    static int m_channel;
    static char *m_eventDat;

    std::deque<int> mLoadState;            // offset 0x58, size 0x30
    int m_currentIntensity;                // offset 0x88, size 0x4
    int m_speechCycle;                     // offset 0x8C, size 0x4
    SpeechSampleVec m_pendingList;         // offset 0x90, size 0x14
    ScheduledSpeechEvent *m_currEvent;     // offset 0xA4, size 0x4
    unsigned int m_currEventTime;          // offset 0xA8, size 0x4
    int m_currEventClarity;                // offset 0xAC, size 0x4
    short m_currEventSpeakerID;            // offset 0xB0, size 0x2
    cStichWrapper *m_Chirper;              // offset 0xB4, size 0x4
    SND_Params m_ChirpParams;              // offset 0xB8, size 0x18

    GameSpeech();
    ~GameSpeech() override;
    void Init(int channel) override;
    static void LoadingCallback(int param, int error_status);
    void LoadBanks() override;
    int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    SPCHType_EventRuleResult EventRuleCallback(int eventID) override;
    void Update() override;
    void CheckNextEvent();
    void IssuePlayback(ScheduledSpeechEvent *nextevent);
    void ClearCompletedRequests();
    void ReleaseResource() override;
    unsigned int SampleRequestCallback(SPCHType_SampleRequestData *data) override;
    void IssueSampleRequests();
    bool ShouldPause();
    void RadioChirp(unsigned char type);
    void UpdateChirps();
    int GetVolForSpeaker(int id);
    char *GetCSIptr() override;
    int GetChannel() override;
    char *GetEventDat() override;
    bool IsDataLoaded() override;
};

} // namespace Speech

#endif
