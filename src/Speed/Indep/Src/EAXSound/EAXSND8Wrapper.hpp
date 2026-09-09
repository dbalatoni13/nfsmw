#ifndef EAX_SND8WRAPPER_HPP
#define EAX_SND8WRAPPER_HPP

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAudioParams.hpp"

// total size: 0x1C
class EAXSND8Wrapper : public AudioMemBase {
  public:
    EAXSND8Wrapper();
    ~EAXSND8Wrapper() override;

    bool Initialize();

    void DeleteStreamBuffer();

    void ReInit();

    void Destroy();

    void Update();

    void RestoreSoundDriver();

    eSndAudioMode SetAudioRenderMode(eSndAudioMode mode);

    eSndAudioMode GetDefaultPlatformAudioMode();

    eSndAudioMode SetAudioModeFromMemoryCard(eSndAudioMode mode);

    eSndAudioMode SetSnd8RenderMode(eSndAudioMode mode);

    void STUPID();

    bool HasAudioModeChanged();

  private:
    char *m_pSoundHeap;                // offset 0x4, size 0x4
    char *m_pStreamBuff;               // offset 0x8, size 0x4
    int m_nHeapSize;                   // offset 0xC, size 0x4
    int m_nStreamSize;                 // offset 0x10, size 0x4
    eSndAudioMode m_eCurrentAudioMode; // offset 0x14, size 0x4
    eSndAudioMode m_eLastAudioMode;    // offset 0x18, size 0x4
};

#endif
