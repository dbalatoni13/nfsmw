#ifndef _SPEECH_MODULE_H_
#define _SPEECH_MODULE_H_

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSoundEnums.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribHash.h"

// TODO move to spch
enum SPCHType_EventRuleResult {
    kSPCH_EventRule_Delete = 0,
    kSPCH_EventRule_OK = 1,
};

#define MAX_STITCH_SAMPLES_PER_EVENT 7 // Decl: 115

namespace Speech {

// total size: 0x58
// Decl: 222
class Module : public AudioMemBase {
  public:
    virtual void Init(int channel);
    virtual void LoadBanks();
    virtual int TestSentenceRuleCallback(int eventID, int ruleID, int parmValue);
    virtual int SetSentenceRuleCallback(int eventID, int ruleID, int parmValue);
    virtual SPCHType_EventRuleResult EventRuleCallback(int eventID);

    virtual int GetNumBanks() {
        return 0; // TODO fix
    }

    virtual unsigned int GetBankOffset(int bnum);
    virtual void Update();
    virtual const char *GetFilename();
    virtual bool QueStream(eNISSFX_TYPE stream_type, void (*callback)(), bool trigger_play_after_callback);
    virtual unsigned int SampleRequestCallback(struct SPCHType_SampleRequestData *data); // TODO
    virtual bool IsStreamQueued();

    bool DonePlaying();

    virtual char *GetCSIptr();
    virtual int GetChannel();
    virtual char *GetEventDat();
    virtual bool IsDataLoaded();
    virtual bool PlayStream(int stream_id);
    virtual void ReleaseResource();

  protected:
    bool m_enable;                     // offset 0x4, size 0x1
    int m_datID;                       // offset 0x8, size 0x4
    int m_projID;                      // offset 0xC, size 0x4
    struct SPEECH_BANK *m_speechBanks; // offset 0x10, size 0x4 // TODO
    eMasterMixChannel m_mixChannel;    // offset 0x14, size 0x4
    int m_streamID;                    // offset 0x18, size 0x4
    int m_fileNum;                     // offset 0x1C, size 0x4
    char *m_bankHeaders;               // offset 0x20, size 0x4
    int m_numBanks;                    // offset 0x24, size 0x4
    unsigned int m_flags;              // offset 0x28, size 0x4
    struct EAXS_StreamChannel *m_strm; // offset 0x2C, size 0x4 // TODO
    Attrib::StringKey m_filename;      // offset 0x30, size 0x10
    Timer mLastEventTimestamp;         // offset 0x40, size 0x4
    SFX_Base *m_pSFXOBJ_Speech;        // offset 0x44, size 0x4
    SFX_Base *m_pSFXOBJ_Moment;        // offset 0x48, size 0x4
    SFX_Base *m_pSFXOBJ_NISStream;     // offset 0x4C, size 0x4
    bool m_bIsStreamQueued;            // offset 0x50, size 0x1
};

#define NO_FILTER 0         // Decl: 268
#define SHORT_FILTER 1      // Decl: 269
#define VERY_SHORT_FILTER 2 // Decl: 270

}; // namespace Speech

#endif
