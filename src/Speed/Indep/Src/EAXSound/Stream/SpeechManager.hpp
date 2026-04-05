#ifndef EAXSOUND_STREAM_SPEECHMANAGER_H
#define EAXSOUND_STREAM_SPEECHMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "SpeechModule.hpp"

struct SlotPool;

enum SpeechModuleIndex {
    NISSFX_MODULE = 0,
    COPSPEECH_MODULE = 1,
    NUM_SPEECH_MODULES = 2,
};

namespace Speech {

class SampleReqList;

class Cache {
  public:
    SlotPool *GetEventPool();
    void Free(void *ptr);
    void Dump();
    void Init(int size);
    void Validate();
};

class Manager {
  public:
    static void ClearPlayback();
    static void Init(int mode);
    static void Destroy();
    static void Deduce();
    static void Update(float dt);
    static Module *GetSpeechModule(int id);
    static void AttachSFXOBJ(SpeechModuleIndex module, SFX_Base *psb, eSFXOBJ_MAIN_TYPES type);
    static SampleReqList &GetSampleRequests() { return mSampleRequests; }

    static short m_frameindex;
    static SampleReqList mSampleRequests;
};

}; // namespace Speech

extern Speech::Cache gSpeechCache;

#endif
