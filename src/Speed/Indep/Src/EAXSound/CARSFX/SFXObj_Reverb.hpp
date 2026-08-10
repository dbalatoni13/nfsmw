//
#ifndef SFXOBJ_REVERB_HPP
#define SFXOBJ_REVERB_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

#define NUMBER_REVERB_ALLOCS 4         // Decl: 15
#define LARGEST_REVERB_SIZE 128 * 1024 // Decl: 16

// TODO remove
namespace Snd {
struct GlobalFxProcessor;
};

// total size: 0x2C
// Decl: 23
class SFXObj_Reverb : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_Reverb();
    ~SFXObj_Reverb() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void SetupLoadData() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    static void *AllocReverbBuffer(char *name, int size);
    static bool FreeReverbBuffer(void *ptr);

    SFXCTL_Tunnel *m_pTunnelCtl; // offset 0x28, size 0x4, Decl: 46

    // Decl: 51
    struct ReverbStructure {
        // Decl: 52
        ReverbStructure() {
            this->Clear();
        }

        // Decl: 57
        void Clear() {
            this->Alloc = nullptr;
            this->Size = 0;
        }

        void *Alloc; // offset 0x0, size 0x4, Decl: 63
        int Size;    // offset 0x4, size 0x4, Decl: 64
    };

    static Snd::GlobalFxProcessor *m_pFXEditModule[2];         // size: 0x8, address: 0x804FFF28
    static char *m_pFXEditPatch[12];                           // size: 0x30, address: 0x804FE710
    static void *m_EchoBuffer;                                 // size: 0x4, address: 0x80418274
    static void *m_UnusedBuffer;                               // size: 0x4, address: 0xFFFFFFFF
    static bool bUnavailable;                                  // size: 0x1, address: 0xFFFFFFFF
    static ReverbStructure m_EchoAllocs[NUMBER_REVERB_ALLOCS]; // size: 0x20, address: 0x8045E580
};

#endif
