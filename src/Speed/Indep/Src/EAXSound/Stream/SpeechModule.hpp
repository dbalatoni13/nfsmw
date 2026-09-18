#ifndef EAXSOUND_STREAM_SPEECHMODULE_H
#define EAXSOUND_STREAM_SPEECHMODULE_H

#define _SPEECH_MODULE_H_

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "spch/spch.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

// El ELF mangla AddHeaders/LoadSpeechBank con PQ26Speech11SPEECH_BANK:
// SPEECH_BANK vive DENTRO del namespace Speech, no en el global.
namespace Speech {
struct SPEECH_BANK {
    char *mem;
    int bank;
    int offset;
};
}
struct EAXS_StreamChannel;
struct SFX_Base;






namespace Speech {

// total size: 0x58
class Module : public AudioMemBase {
  public:
    Module();
    virtual ~Module();
    virtual void Init(int channel) = 0;
    virtual void LoadBanks() = 0;
    virtual int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue) = 0;
    virtual int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue) = 0;
    virtual SPCHType_EventRuleResult EventRuleCallback(int eventID) = 0;

    virtual int GetNumBanks() {
        return m_numBanks;
    }

    virtual unsigned int GetBankOffset(int bnum);
    virtual void Update() = 0;
    virtual const char *GetFilename() {
        return m_filename.GetString();
    }

    virtual bool QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool trigger_play_after_callback) {
        return false;
    }

    virtual unsigned int SampleRequestCallback(SPCHType_SampleRequestData *data) = 0; // TODO
    virtual bool IsStreamQueued() {
        return m_bIsStreamQueued;
    }

    virtual char *GetCSIptr() = 0;
    virtual int GetChannel() = 0;
    virtual char *GetEventDat() = 0;
    virtual bool IsDataLoaded() = 0;
    virtual bool PlayStream(int stream_id);
    bool DonePlaying();
    void AttachSFXOBJ(SFX_Base *psfx, eSFXOBJ_MAIN_TYPES sfxtype);
    virtual void ReleaseResource();
    EAXS_StreamChannel *GetStreamChannel() { return m_strm; }
    bool TestFlag(unsigned int flag) { return (m_flags & flag) != 0; }
    void SetFlag(unsigned int flag) { m_flags |= flag; }
    void ClearFlag(unsigned int flag) { m_flags &= ~flag; }
    void PurgeSpeech();
    void Pause();
    void UnPause();
    SFX_Base *GetSFXOBJ_Speech() { return m_pSFXOBJ_Speech; }
    Timer GetLastEventTimestamp() { return mLastEventTimestamp; }

  protected:
    bool m_enable;                        // offset 0x4, size 0x1
    int m_datID;                          // offset 0x8, size 0x4
    int m_projID;                         // offset 0xC, size 0x4
    SPEECH_BANK *m_speechBanks;    // offset 0x10, size 0x4 // TODO
    eMasterMixChannel m_mixChannel;       // offset 0x14, size 0x4
    int m_streamID;                       // offset 0x18, size 0x4
    int m_fileNum;                        // offset 0x1C, size 0x4
    char *m_bankHeaders;                  // offset 0x20, size 0x4
    int m_numBanks;                       // offset 0x24, size 0x4
    unsigned int m_flags;                 // offset 0x28, size 0x4
    EAXS_StreamChannel *m_strm;    // offset 0x2C, size 0x4 // TODO
    Attrib::StringKey m_filename;         // offset 0x30, size 0x10
    Timer mLastEventTimestamp;            // offset 0x40, size 0x4
    SFX_Base *m_pSFXOBJ_Speech;    // offset 0x44, size 0x4
    SFX_Base *m_pSFXOBJ_Moment;    // offset 0x48, size 0x4
    SFX_Base *m_pSFXOBJ_NISStream; // offset 0x4C, size 0x4
    bool m_bIsStreamQueued;               // offset 0x50, size 0x1
};

}; // namespace Speech

#endif
