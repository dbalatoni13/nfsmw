#include <types.h>
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemoryManager.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

class PF_Allocator : public EA::Allocator::IAllocator {
  public:
    virtual ~PF_Allocator() {}
    virtual void *Alloc(unsigned int size, const EA::TagValuePair &flags);
    virtual void Free(void *pBlock, unsigned int size);
    virtual int AddRef();
    virtual int Release();

    int mRefcount; // offset 0x4, size 0x4
};

#include "Speed/Indep/Src/EAXSound/PF_iallocatorimpl.h"

namespace EA {
namespace Allocator {
struct ICoreAllocator {
  public:
    ICoreAllocator() {}
    virtual void *Alloc(unsigned int size, const char *name, unsigned int flags) {
        (void)size;
        (void)name;
        (void)flags;
        return nullptr;
    }
    virtual void *Alloc(unsigned int size, const char *name, unsigned int flags, unsigned int alignment, unsigned int offset) {
        (void)size;
        (void)name;
        (void)flags;
        (void)alignment;
        (void)offset;
        return nullptr;
    }
    virtual void Free(void *pBlock, unsigned int size) {
        (void)pBlock;
        (void)size;
    }
};
} // namespace Allocator
} // namespace EA

struct CSISCoreAllocator : public EA::Allocator::ICoreAllocator {
    void *Alloc(unsigned int size, const char *name, unsigned int flags) override;
    void *Alloc(unsigned int size, const char *name, unsigned int flags, unsigned int alignment, unsigned int offset) override;
    void Free(void *pBlock, unsigned int size) override;
};

extern SlotPool *pCsisSlotPools[];
extern int nCsisSlotPoolSizes[];
extern void bDeleteSlotPool(SlotPool *slotpool);
extern SlotPool *bNewSlotPool(int slot_size, int reserve_count, const char *name, int pool);
extern int AudioMemoryPool;
extern int MAIN_SAMPLERATE;
extern CSISCoreAllocator g_CSISCoreAllocator;
extern AudioMemoryManager gAudioMemoryManager;
extern bool IsAudioStreamingEnabled;
extern bool IsNISAudioEnabled;
extern bool IsSpeechEnabled;
extern EAXSound *g_pEAXSound;
extern void SNDSYS_service();

CSISCoreAllocator g_CSISCoreAllocator;

namespace Csis {
namespace System {
void SetAllocator(EA::Allocator::ICoreAllocator *allocator);
void Init();
}
} // namespace Csis

namespace Snd {
enum Device {
    DEVICE_MAIN = 0,
    DEVICE_IOP = 1,
};

enum OutputMode {
    OUTPUTMODE_MONO = 0,
    OUTPUTMODE_STEREO = 1,
    OUTPUTMODE_PROLOGIC2 = 8,
};

namespace System {
void ReInit();
void Init(int memsize);
void SetOutputMode(OutputMode mode);
void SetOutputSampleRate(Device device, int rate);
void SetVoices(Device device, int voices);
void SetSndInitsAram(bool enabled);
void SetMaxBanks(int maxBanks);
void VectorToCsisMutex();
void VectorToReal6();
} // namespace System

namespace Memory {
void SetHeap(Device device, void *heap, int size);
void SetHeapThreshold(Device device, float threshold);
} // namespace Memory
} // namespace Snd

EAXSND8Wrapper::EAXSND8Wrapper() {
    m_pSoundHeap = nullptr;
    m_pStreamBuff = nullptr;
    pCsisSlotPools[0] = nullptr;
    nCsisSlotPoolSizes[0] = 0x88;
}

EAXSND8Wrapper::~EAXSND8Wrapper() {
    bDeleteSlotPool(pCsisSlotPools[0]);
    pCsisSlotPools[0] = nullptr;
}

bool EAXSND8Wrapper::Initialize() {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        return false;
    }

    pCsisSlotPools[0] = static_cast<SlotPool *>(bNewSlotPool(nCsisSlotPoolSizes[0], 0x100, "AUD:Csis SlotPools", AudioMemoryPool));
    Csis::System::SetAllocator(static_cast<EA::Allocator::ICoreAllocator *>(static_cast<void *>(&g_CSISCoreAllocator)));
    Csis::System::Init();
    Snd::System::VectorToCsisMutex();
    Snd::System::VectorToReal6();
    Snd::System::SetMaxBanks(0x20);

    m_nHeapSize = 0x33000;
    m_pSoundHeap = gAudioMemoryManager.AllocateMemoryChar(m_nHeapSize, "SND Heap", false);
    Snd::System::SetOutputSampleRate(Snd::DEVICE_MAIN, MAIN_SAMPLERATE);

    eSndAudioMode mode = GetDefaultPlatformAudioMode();
    m_eCurrentAudioMode = mode;
    m_eLastAudioMode = mode;
    SetAudioRenderMode(mode);

    Snd::System::SetVoices(Snd::DEVICE_MAIN, 8);
    Snd::System::SetOutputSampleRate(Snd::DEVICE_IOP, 32000);
    Snd::System::SetOutputSampleRate(Snd::DEVICE_MAIN, MAIN_SAMPLERATE);
    Snd::System::SetSndInitsAram(true);
    Snd::Memory::SetHeap(Snd::DEVICE_MAIN, m_pSoundHeap, m_nHeapSize);
    Snd::Memory::SetHeapThreshold(Snd::DEVICE_MAIN, 1.0f);
    Snd::System::Init(0x90600);
    return true;
}

void EAXSND8Wrapper::ReInit() {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
    } else {
        AudioSettings *settings = g_pEAXSound->GetCurrentAudioSettings();
        eSndAudioMode mode = static_cast<eSndAudioMode>(settings->AudioMode);
        m_eCurrentAudioMode = mode;
        SetSnd8RenderMode(mode);
        Snd::System::ReInit();
    }
}

void EAXSND8Wrapper::STUPID() {}

void EAXSND8Wrapper::Update() {
    STUPID();
}

eSndAudioMode EAXSND8Wrapper::SetAudioModeFromMemoryCard(eSndAudioMode mode) {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return mode;
    }

    m_eCurrentAudioMode = GetDefaultPlatformAudioMode();
    switch (m_eCurrentAudioMode) {
    case AUDIO_MODE_MIN:
        mode = AUDIO_MODE_MIN;
        break;
    case AUDIO_MODE_STEREO:
        if (mode == AUDIO_MODE_MIN) {
            mode = AUDIO_MODE_STEREO;
        }
        break;
    default:
        mode = m_eCurrentAudioMode;
        break;
    }

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

    m_eCurrentAudioMode = mode;
    return mode;
}

eSndAudioMode EAXSND8Wrapper::SetAudioRenderMode(eSndAudioMode mode) {
    if (IsSoundEnabled == 0) {
        IsAudioStreamingEnabled = IsSoundEnabled;
        IsSpeechEnabled = IsSoundEnabled;
        IsNISAudioEnabled = IsSoundEnabled;
        return mode;
    }

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

    m_eCurrentAudioMode = mode;
    SetSnd8RenderMode(mode);
    return m_eCurrentAudioMode;
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
    if (!IsSoundEnabled) {
        return AUDIO_MODE_STEREO;
    }
    eSndAudioMode mode = AUDIO_MODE_STEREO;
    if (OSGetSoundMode() == 0) {
        mode = static_cast<eSndAudioMode>(0);
    }
    return mode;
}

void *CSISCoreAllocator::Alloc(unsigned int size, const char *name, unsigned int flags) {
    (void)flags;
    return bOMalloc(pCsisSlotPools[0]);
}


void *CSISCoreAllocator::Alloc(unsigned int size, const char *name, unsigned int flags, unsigned int alignment, unsigned int offset) {
    (void)alignment;
    (void)offset;
    return Alloc(size, g_pcsCSISAllocString, 0);
}

void CSISCoreAllocator::Free(void *pBlock, unsigned int size) {
    (void)size;
    bFree(pCsisSlotPools[0], pBlock);
}

void *PF_Allocator::Alloc(unsigned int size, const EA::TagValuePair &flags) {
    (void)flags;
    return gAudioMemoryManager.AllocateMemory(size, "AUD: Pathfinder alloc", true);
}

PF_Allocator gPF_MemoryAllocator;
