#include "Speed/Indep/Src/EAXSound/EAXSND8Wrapper.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "snd/sndo.h"

int MAIN_SAMPLERATE = 32000; // Decl: 48

extern int IsAudioStreamingEnabled; // Decl: 52
extern int IsSpeechEnabled;         // Decl: 53
extern int IsNISAudioEnabled;       // Decl: 54
CSISCoreAllocator g_CSISCoreAllocator;
SlotPool *pCsisSlotPools[1];   // Decl: 56
uint32 nCsisSlotPoolSizes[1];  // Decl: 57
uint32 nLargestAllocation = 0; // Decl: 58
int CsisCnt = 0;               // Decl: 59

float g_PC_UpdateRate = 50.0f;

int g_PC_SampleRate = 44100;
int g_PC_SampRate_Override = 0;

int gn_PS2TestStreamBufferSize = 0x86; // Decl: 102
int gn_GCTestStreamBufferSize = 0x6E;  // Decl: 103

int WRITEAIFF = 0;
int SAVEAIFF = 0;

EAXSND8Wrapper::EAXSND8Wrapper() {
    this->m_pSoundHeap = nullptr;
    this->m_pStreamBuff = nullptr;
    pCsisSlotPools[0] = nullptr;
    nCsisSlotPoolSizes[0] = 0x88;
}

EAXSND8Wrapper::~EAXSND8Wrapper() {
    bDeleteSlotPool(pCsisSlotPools[0]);
    pCsisSlotPools[0] = nullptr;
}

// STRIPPED
void EAXSND8Wrapper::RestoreSoundDriver() {}

bool EAXSND8Wrapper::Initialize() {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return false;
    }

    pCsisSlotPools[0] = static_cast<SlotPool *>(bNewSlotPool(nCsisSlotPoolSizes[0], 256, "AUD:Csis SlotPools", AudioMemoryPool));
    Csis::System::SetAllocator(&g_CSISCoreAllocator);
    Csis::System::Init();
    Snd::System::VectorToCsisMutex();
    Snd::System::VectorToReal6();
    Snd::System::SetMaxBanks(32);

    this->m_nHeapSize = 204 * 1024;
    this->m_pSoundHeap = gAudioMemoryManager.AllocateMemoryChar(this->m_nHeapSize, "SND Heap", false);
    Snd::System::SetOutputSampleRate(Snd::DEVICE_MAIN, MAIN_SAMPLERATE);

    this->SetAudioRenderMode(this->m_eLastAudioMode = this->m_eCurrentAudioMode = this->GetDefaultPlatformAudioMode());

    Snd::System::SetVoices(Snd::DEVICE_MAIN, 8);
    Snd::System::SetOutputSampleRate(Snd::DEVICE_IOP, 32000);
    Snd::System::SetOutputSampleRate(Snd::DEVICE_MAIN, MAIN_SAMPLERATE);
    Snd::System::SetSndInitsAram(true);
    Snd::Memory::SetHeap(Snd::DEVICE_MAIN, this->m_pSoundHeap, this->m_nHeapSize);
    Snd::Memory::SetHeapThreshold(Snd::DEVICE_MAIN, 1.0f);
    Snd::System::Init(0x90600);
    return true;
}

// STRIPPED
void EAXSND8Wrapper::DeleteStreamBuffer() {}

void EAXSND8Wrapper::ReInit() {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return;
    }

    this->m_eCurrentAudioMode = static_cast<eSndAudioMode>(g_pEAXSound->m_pCurAudioSettings->AudioMode);
    this->SetSnd8RenderMode(this->m_eCurrentAudioMode);
    Snd::System::ReInit();
}

void EAXSND8Wrapper::STUPID() {}

void EAXSND8Wrapper::Update() {
    this->STUPID();
}

eSndAudioMode EAXSND8Wrapper::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return mode;
    }

    this->m_eCurrentAudioMode = this->GetDefaultPlatformAudioMode();
    switch (this->m_eCurrentAudioMode) {
        case AUDIO_MODE_MIN:
            mode = AUDIO_MODE_MIN;
            break;
        case AUDIO_MODE_STEREO:
            if (mode == AUDIO_MODE_MIN) {
                mode = AUDIO_MODE_STEREO;
            }
            break;
        default:
            mode = this->m_eCurrentAudioMode;
            break;
    }

#ifdef EA_PLATFORM_GAMECUBE
    switch (mode) {
        case AUDIO_MODE_MIN:
            OSSetSoundMode(0);
            break;
        case AUDIO_MODE_STEREO:
            OSSetSoundMode(1);
            break;
        case AUDIO_MODE_MAX:
            OSSetSoundMode(1);
            break;
        default:
            break;
    }
#endif

    this->m_eCurrentAudioMode = mode;
    return mode;
}

eSndAudioMode EAXSND8Wrapper::SetAudioRenderMode(eSndAudioMode mode) {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return mode;
    }

#ifdef EA_PLATFORM_GAMECUBE
    switch (mode) {
        case AUDIO_MODE_MIN:
            OSSetSoundMode(0);
            break;
        case AUDIO_MODE_STEREO:
            OSSetSoundMode(1);
            break;
        case AUDIO_MODE_MAX:
            OSSetSoundMode(1);
            break;
        default:
            break;
    }
#endif

    this->m_eCurrentAudioMode = mode;
    this->SetSnd8RenderMode(mode);
    return this->m_eCurrentAudioMode;
}

eSndAudioMode EAXSND8Wrapper::SetSnd8RenderMode(eSndAudioMode mode) {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return mode;
    }

    switch (mode) {
        case AUDIO_MODE_MIN:
            Snd::System::SetOutputMode(Snd::OUTPUTMODE_STEREO);
            SNDSYS_service();
            Snd::System::SetOutputMode(Snd::OUTPUTMODE_MONO);
            break;
        case AUDIO_MODE_STEREO:
            Snd::System::SetOutputMode(Snd::OUTPUTMODE_PROLOGIC2);
            break;
        case AUDIO_MODE_MAX:
            Snd::System::SetOutputMode(Snd::OUTPUTMODE_PROLOGIC2);
            break;
        default:
            break;
    }

    return mode;
}

eSndAudioMode EAXSND8Wrapper::GetDefaultPlatformAudioMode() {
    if (IsSoundEnabled == 0) {
        return AUDIO_MODE_STEREO;
    }
#ifdef EA_PLATFORM_GAMECUBE
    eSndAudioMode mode = AUDIO_MODE_STEREO;
    if (OSGetSoundMode() == 0) {
        mode = AUDIO_MODE_MIN;
    }
#endif
    return mode;
}
