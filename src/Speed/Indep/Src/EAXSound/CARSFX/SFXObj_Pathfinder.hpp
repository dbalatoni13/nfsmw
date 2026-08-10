//
//
#ifndef SFXOBJ_PATHFINDER_H
#define SFXOBJ_PATHFINDER_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"

// Decl: 26
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

// total size: 0x18
// Decl: 49
struct stEATraxParms {
    stEATraxParms()
        : TraxMask(0),          //
          PlayBits(0xFFFFFFFF), //
          NumEnabledSongs(0),   //
          LastPlaylistSong(-1), //
          PlayTrackIndex(-1),   //
          PBMode(-1) {}         // Decl: 50

    unsigned int TraxMask;        // offset 0x0, size 0x4, Decl: 59
    unsigned int PlayBits;        // offset 0x4, size 0x4, Decl: 60
    unsigned int NumEnabledSongs; // offset 0x8, size 0x4, Decl: 61
    int LastPlaylistSong;         // offset 0xC, size 0x4, Decl: 62
    int PlayTrackIndex;           // offset 0x10, size 0x4, Decl: 63
    int PBMode;                   // offset 0x14, size 0x4, Decl: 64
};

// Decl: 67
enum eEATRAXSTATES {
    EATRAX_UNINIT = -2,
    EATRAX_OFF = -1,
    EATRAX_FE = 0,
    EATRAX_IG = 1,
};

// total size: 0x120
// Decl: 104
class SFXObj_Pathfinder : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_Pathfinder();
    ~SFXObj_Pathfinder() override {} // Decl: 108

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    void SetSplitScreen(bool bIsSplitScreen) {
        if (bIsSplitScreen) {
            this->m_Flags |= 2;
        } else {
            this->m_Flags &= ~2u;
        }
    } // Decl: 113

    bool GetSplitScreen() {
        return (this->m_Flags & 2) != 0;
    } // Decl: 114

    void SetNISPlaying(bool bon) {
        if (bon) {
            this->m_Flags |= 0x200;
        } else {
            this->m_Flags &= ~0x200;
        }
    } // Decl: 119

    void Set321(bool bon) {} // Decl: 120

    stPFParms m_PFParms[2];                  // offset 0x28, size 0xF0, Decl: 117
    SFXCTL_Pathfinder *m_pSFXCTL_Pathfinder; // offset 0x118, size 0x4, Decl: 118

    unsigned int m_Flags; // offset 0x11C, size 0x4, Decl: 121
};

// total size: 0x19C
// Decl: 125
class SFXObj_PFEATrax : public SFXObj_Pathfinder {
  public:
    DECLARE_CREATABLE();
    SFXObj_PFEATrax();
    ~SFXObj_PFEATrax() override;

    // Overrides: SndBase
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
        return this->GenMusicType();
    } // Decl: 149

    void UpdateInGame(float t);

    void UpdateFrontEnd(float t) {} // Decl: 154

    void UpdatePursuitBreaker(float t);
    bool TestToPursuit();
    bool TestToLicensed(bool bstart);
    bool TestToAmbience();
    void Pause();
    void UnPause();
    void UpdateAmbience(float t);

    virtual void StartAmbience(uint32 PathEvent);

    virtual void StartInteractiveMusic(uint32 PathEvent);

    virtual void StartLicensedMusic(uint32 PathEvent);

    void GenNextMusicTrackID();

    void NotifyChyron();

    bool m_bSkipUpdate;               // offset 0x120, size 0x1, Decl: 166
    bool m_bClearSkipUpdate;          // offset 0x124, size 0x1, Decl: 167
    bool m_bPathFAILED;               // offset 0x128, size 0x1, Decl: 168
    int m_nAmbientZone;               // offset 0x12C, size 0x4, Decl: 173
    int m_AmbZoneVol;                 // offset 0x130, size 0x4, Decl: 174
    int m_PrevAmbientZone;            // offset 0x134, size 0x4, Decl: 175
    uint32 m_CurPathEvent;            // offset 0x138, size 0x4, Decl: 183
    uint32 m_PrevPathEvent;           // offset 0x13C, size 0x4, Decl: 184
    unsigned int m_FEPreviewEvent;    // offset 0x140, size 0x4, Decl: 185
    int m_FEPreviewIndex;             // offset 0x144, size 0x4, Decl: 186
    int8 m_CurIntensity;              // offset 0x148, size 0x1, Decl: 189
    int8 m_PrevIntensity;             // offset 0x149, size 0x1, Decl: 190
    static stEATraxParms m_EATrax[2]; // size: 0x30, address: 0x8045E5BC

  private:
    void MessageStartPathfinder(const MControlPathfinder &message);
    void MessageInitSongsList(const MControlPathfinder &message);
    void MessageSendPathEvent(const MControlPathfinder &message);
    void MessageSendPathControl(const MControlPathfinder &message);
    void MessagePartUpdate(const MControlPathfinder &message);
    void MessagePerpBusted(const MPerpBusted &message);
    void MessageInteractiveDone(const MControlPathfinder &message);
    void MessageSwapInteractive(const MControlPathfinder &message);

    ePATHFINDER_PROJ m_ActiveProject;          // offset 0x14C, size 0x4, Decl: 201
    ePATHFINDER_PROJ m_PrevActiveProject;      // offset 0x150, size 0x4, Decl: 202
    eINTERACTIVE_PROJ_ID m_InteractiveProj;    // offset 0x154, size 0x4
    eMUSIC_TYPE m_MusicType;                   // offset 0x158, size 0x4, Decl: 212
    eMUSIC_TYPE m_PrevMusicType;               // offset 0x15C, size 0x4, Decl: 213
    eEATRAXSTATES m_EATraxState;               // offset 0x160, size 0x4, Decl: 214
    cPathLine *m_FilterFade;                   // offset 0x164, size 0x4, Decl: 216
    int m_FilterFreq;                          // offset 0x168, size 0x4, Decl: 217
    int m_Volume;                              // offset 0x16C, size 0x4, Decl: 218
    int m_CurPart;                             // offset 0x170, size 0x4, Decl: 220
    Timer m_PartStart;                         // offset 0x174, size 0x4, Decl: 222
    Timer mT_ambienceStart;                    // offset 0x178, size 0x4, Decl: 223
    Hermes::HHANDLER mMsgControlPF5;           // offset 0x17C, size 0x4, Decl: 228
    Hermes::HHANDLER mMsgControlInitSongslist; // offset 0x180, size 0x4, Decl: 231
    Hermes::HHANDLER mMsgSendPathEvent;        // offset 0x184, size 0x4, Decl: 234
    Hermes::HHANDLER mMsgSendPathControl;      // offset 0x188, size 0x4, Decl: 237
    Hermes::HHANDLER mMsgNotifyPartUpdate;     // offset 0x18C, size 0x4, Decl: 240
    Hermes::HHANDLER mMsgPerpBusted;           // offset 0x190, size 0x4
    Hermes::HHANDLER mMsgInteractiveDone;      // offset 0x194, size 0x4, Decl: 243
    Hermes::HHANDLER mMsgSwapInteractive;      // offset 0x198, size 0x4
};

#endif
