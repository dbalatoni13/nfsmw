//
#ifndef SFXOBJ_COLLISION_HPP
#define SFXOBJ_COLLISION_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"

// total size: 0x8C
// Decl: 14
class SFXObj_Collision : public CARSFX {
  public:
    DECLARE_CREATABLE();

    SFXObj_Collision();
    ~SFXObj_Collision() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Destroy() override;
    void Detach() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    CSTATE_Collision *m_pCollisionState;      // offset 0x28, size 0x4, Decl: 33
    Sound::CollisionEvent *m_pCollisionEvent; // offset 0x2C, size 0x4, Decl: 34

    cStichWrapper *m_pCollisionStich; // offset 0x30, size 0x4, Decl: 36

    float m_fScrapeFade;            // offset 0x34, size 0x4, Decl: 38
    Csis::FX_Scrape *m_pcsisScrape; // offset 0x38, size 0x4, Decl: 39

    SFXCTL_3DObjPos *m_p3DColPos;    // offset 0x3C, size 0x4, Decl: 41
    SFXCTL_3DObjPos *m_p3DScrapePos; // offset 0x40, size 0x4, Decl: 42

    eVOL_COLLISION VolSlot;    // offset 0x44, size 0x4, Decl: 44
    eVOL_COLLISION AzimSlot;   // offset 0x48, size 0x4, Decl: 45
    eVOL_COLLISION PitchSlot;  // offset 0x4C, size 0x4, Decl: 46
    eVOL_COLLISION ReverbSlot; // offset 0x50, size 0x4, Decl: 47

    bool FirstUpdate; // offset 0x54, size 0x1, Decl: 49
    int InitialAz;    // offset 0x58, size 0x4, Decl: 50

    bVector3 vCollisionPos; // offset 0x5C, size 0x10, Decl: 52
    bVector3 vScrapePos;    // offset 0x6C, size 0x10, Decl: 53
    bVector3 vVelocity;     // offset 0x7C, size 0x10, Decl: 54
};

#endif
