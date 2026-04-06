#ifndef EAXSOUND_CARSFX_SFXOBJ_PFEATRAX_H
#define EAXSOUND_CARSFX_SFXOBJ_PFEATRAX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct stEATraxParms {
    unsigned int TraxMask;       // offset 0x0, size 0x4
    unsigned int PlayBits;       // offset 0x4, size 0x4
    unsigned int NumEnabledSongs; // offset 0x8, size 0x4
    int LastPlaylistSong;        // offset 0xC, size 0x4
    int PlayTrackIndex;          // offset 0x10, size 0x4
    int PBMode;                  // offset 0x14, size 0x4

    stEATraxParms() {}
};

enum eEATRAXSTATES {
    EATRAX_UNINIT = -2,
    EATRAX_OFF = -1,
    EATRAX_FE = 0,
    EATRAX_IG = 1,
};

enum ePATHFINDER_PROJ {
    PF_PROJECTRESET = -1,
    PF_LICENSED_MUSIC = 0,
    PF_INTERACTIVE_MUSIC = 1,
};

enum eINTERACTIVE_PROJ_ID {
    PF_INTERACTIVE_00 = 0,
    PF_INTERACTIVE_01 = 1,
    PF_INTERACTIVE_02 = 2,
    PF_INTERACTIVE_03 = 3,
};

enum eMUSIC_TYPE {
    eMUSIC_TYPE_LICENCED = 0,
    eMUSIC_TYPE_INTERACTIVE = 1,
    eMUSIC_TYPE_AMBIENCE = 2,
    eMUSIC_TYPE_SPLASH = 3,
};

// TODO: PS2 alpha type info omits several later members; use the GC DWARF layout here.
class SFXObj_PFEATrax : public SFXObj_Pathfinder {
  protected:
    static TypeInfo s_TypeInfo;
    static stEATraxParms m_EATrax[2];

  public:
    // total size: 0x19C
    bool m_bSkipUpdate;                        // offset 0x120, size 0x1
    bool m_bClearSkipUpdate;                   // offset 0x124, size 0x1
    bool m_bPathFAILED;                        // offset 0x128, size 0x1
    int m_nAmbientZone;                        // offset 0x12C, size 0x4
    int m_AmbZoneVol;                          // offset 0x130, size 0x4
    int m_PrevAmbientZone;                     // offset 0x134, size 0x4
    unsigned int m_CurPathEvent;              // offset 0x138, size 0x4
    unsigned int m_PrevPathEvent;             // offset 0x13C, size 0x4
    unsigned int m_FEPreviewEvent;            // offset 0x140, size 0x4
    int m_FEPreviewIndex;                      // offset 0x144, size 0x4
    char m_CurIntensity;                       // offset 0x148, size 0x1
    char m_PrevIntensity;                      // offset 0x149, size 0x1

  private:
    ePATHFINDER_PROJ m_ActiveProject;          // offset 0x14C, size 0x4
    ePATHFINDER_PROJ m_PrevActiveProject;      // offset 0x150, size 0x4
    eINTERACTIVE_PROJ_ID m_InteractiveProj;    // offset 0x154, size 0x4
    eMUSIC_TYPE m_MusicType;                   // offset 0x158, size 0x4
    eMUSIC_TYPE m_PrevMusicType;               // offset 0x15C, size 0x4
    eEATRAXSTATES m_EATraxState;               // offset 0x160, size 0x4
    cPathLine *m_FilterFade;                   // offset 0x164, size 0x4
    int m_FilterFreq;                          // offset 0x168, size 0x4
    int m_Volume;                              // offset 0x16C, size 0x4
    int m_CurPart;                             // offset 0x170, size 0x4
    Timer m_PartStart;                         // offset 0x174, size 0x4
    Timer mT_ambienceStart;                    // offset 0x178, size 0x4
    Hermes::HHANDLER mMsgControlPF5;           // offset 0x17C, size 0x4
    Hermes::HHANDLER mMsgControlInitSongslist; // offset 0x180, size 0x4
    Hermes::HHANDLER mMsgSendPathEvent;        // offset 0x184, size 0x4
    Hermes::HHANDLER mMsgSendPathControl;      // offset 0x188, size 0x4
    Hermes::HHANDLER mMsgNotifyPartUpdate;     // offset 0x18C, size 0x4
    Hermes::HHANDLER mMsgPerpBusted;           // offset 0x190, size 0x4
    Hermes::HHANDLER mMsgInteractiveDone;      // offset 0x194, size 0x4
    Hermes::HHANDLER mMsgSwapInteractive;      // offset 0x198, size 0x4

  public:
    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

    SFXObj_PFEATrax();
    ~SFXObj_PFEATrax() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    void SetupSFX(CSTATE_Base *_StateBase) override;
    void SetupLoadData() override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    void SwapInteractiveProjects();
    virtual void Stop();
    void SendPathEvent();
    void RestartRace();
    eEATRAXSTATES GenEATraxState();
    eMUSIC_TYPE GenMusicType();

    eMUSIC_TYPE GetMusicType() {
        return m_MusicType;
    }

    void UpdateInGame(float t);
    void UpdateFrontEnd(float t);
    void UpdatePursuitBreaker(float t);
    bool TestToPursuit();
    bool TestToLicensed(bool bstart);
    bool TestToAmbience();
    void Pause();
    void UnPause();
    void UpdateAmbience(float t);
    virtual void StartAmbience(unsigned int PathEvent);
    virtual void StartInteractiveMusic(unsigned int PathEvent);
    virtual void StartLicensedMusic(unsigned int PathEvent);
    void GenNextMusicTrackID();
    void NotifyChyron();

  private:
    bool ShouldPause();
    void MessageStartPathfinder(const MControlPathfinder &message);
    void MessageInitSongsList(const MControlPathfinder &message);
    void MessageSendPathEvent(const MControlPathfinder &message);
    void MessageSendPathControl(const MControlPathfinder &message);
    void MessagePartUpdate(const MControlPathfinder &message);
    void MessagePerpBusted(const MPerpBusted &message);
    void MessageInteractiveDone(const MControlPathfinder &message);
    void MessageSwapInteractive(const MControlPathfinder &message);
};

#endif
