#ifndef _NISSFX_SPEECH_H_
#define _NISSFX_SPEECH_H_

#include "Speed/Indep/Src/EAXSound/Clump.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechModule.hpp"

namespace Speech {

// total size: 0xB0
// Decl: 32
class SED_NISSFX : public Module {
  public:
    SED_NISSFX();
    ~SED_NISSFX() override;

    // Overrides: Module
    void Init(int channel) override;
    void LoadBanks() override;
    int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) override;
    SPCHType_EventRuleResult EventRuleCallback(int eventID) override;
    void Update() override;
    unsigned int SampleRequestCallback(SPCHType_SampleRequestData *data) override;
    bool QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool trigger_play_after_callback) override;
    bool PlayStream(int stream_id) override;

    void ClearStream();

    static void LoadingCallback(int32 param, int error_status);

    bool IsInitted() {
        return this->m_moduleIsInitted;
    } // Decl: 58
    // Overrides: Module
    char *GetCSIptr() override {
        return this->m_csisData;
    } // Decl: 59
    int GetChannel() override {
        return this->m_channel;
    } // Decl: 60
    char *GetEventDat() override {
        return this->m_eventDat;
    } // Decl: 61
    bool IsDataLoaded() override {
        return this->m_dataIsLoaded;
    } // Decl: 62

    eNISSFX_TYPE GetStreamType() {
        return this->m_SyncObject.id;
    } // Decl: 64

    std::deque<int> mLoadState; // offset 0x58, size 0x30, Decl: 68

  protected:
    int m_currentIntensity;            // offset 0x88, size 0x4, Decl: 75
    static char *m_tempCharPtr;        // size: 0x4, address: 0x804359EC
    static CLUMP_IDX_FILE *m_clumpIdx; // size: 0x4, address: 0x804359F0
    bool m_moduleIsInitted;            // offset 0x8C, size 0x1, Decl: 78
    int m_speechCycle;                 // offset 0x90, size 0x4, Decl: 79
    bool m_paused;                     // offset 0x94, size 0x1, Decl: 80
    static char *m_csisData;           // size: 0x4, address: 0x804359F4
    static int m_channel;              // size: 0x4, address: 0x804359F8
    static char *m_eventDat;           // size: 0x4, address: 0x804359FC
    static bool m_dataIsLoaded;        // size: 0x1, address: 0x80435A00
    SyncAudioObject m_SyncObject;      // offset 0x98, size 0x14, Decl: 85
    EAXS_StreamChannel *m_backupstrm;  // offset 0xAC, size 0x4, Decl: 89
};

}; // namespace Speech

#endif
