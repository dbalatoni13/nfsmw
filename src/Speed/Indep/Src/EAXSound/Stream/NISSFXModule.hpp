#ifndef EAXSOUND_STREAM_NISSFXMODULE_H
#define EAXSOUND_STREAM_NISSFXMODULE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "SpeechModule.hpp"

struct CLUMP_IDX_FILEtag;

namespace Speech {
struct SpeechSampleData;
}

// total size: 0x14
struct SyncAudioObject {
    Speech::SpeechSampleData *qsObject; // offset 0x0, size 0x4
    void (*callback)();                 // offset 0x4, size 0x4
    eNISSFX_TYPE id;                    // offset 0x8, size 0x4
    int handle;                         // offset 0xC, size 0x4
    short holdtime;                     // offset 0x10, size 0x2

    SyncAudioObject() {}
    ~SyncAudioObject() {}
};

namespace Speech {

// total size: 0xB0
struct SED_NISSFX : public Module {
    static char *m_tempCharPtr;
    static CLUMP_IDX_FILEtag *m_clumpIdx;
    static char *m_csisData;
    static int m_channel;
    static char *m_eventDat;
    static bool m_dataIsLoaded;

    std::deque<int> mLoadState;         // offset 0x58, size 0x30
    int m_currentIntensity;             // offset 0x88, size 0x4
    bool m_moduleIsInitted;             // offset 0x8C, size 0x1
    int m_speechCycle;                  // offset 0x90, size 0x4
    bool m_paused;                      // offset 0x94, size 0x1
    SyncAudioObject m_SyncObject;       // offset 0x98, size 0x14
    EAXS_StreamChannel *m_backupstrm;   // offset 0xAC, size 0x4

    bool IsInitted() {
        return m_moduleIsInitted;
    }

    char *GetCSIptr() override {
        return m_csisData;
    }

    int GetChannel() override {
        return m_channel;
    }

    char *GetEventDat() override {
        return m_eventDat;
    }

    bool IsDataLoaded() override {
        return m_dataIsLoaded;
    }

    eNISSFX_TYPE GetStreamType() {
        return m_SyncObject.id;
    }

    SED_NISSFX();
    ~SED_NISSFX() override;
    void Init(int channel) override;
    static void LoadingCallback(int param, int error_status);
    void LoadBanks() override;
    int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    SPCHType_EventRuleResult EventRuleCallback(int eventID) override;
    bool QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool trigger_play_after_callback) override;
    unsigned int SampleRequestCallback(SPCHType_SampleRequestData *data) override;
    bool PlayStream(int stream_id) override;
    void ClearStream();
    void Update() override;
};

} // namespace Speech



#endif
