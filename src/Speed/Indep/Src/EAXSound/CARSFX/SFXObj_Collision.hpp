#ifndef EAXSOUND_CARSFX_SFXOBJ_COLLISION_H
#define EAXSOUND_CARSFX_SFXOBJ_COLLISION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SoundCollision.hpp"

struct CSTATE_Collision;
struct FX_Scrape;
struct cStichWrapper;
struct SFXCTL_3DObjPos;

enum eVOL_COLLISION {
    eAZI_COLLISION_COLLISION_AZI = 0,
    eAZI_COLLISION_SCRAPE_AZI = 1,
    eVOL_COLLISION_SMACKABLE_AZI = 2,
    eVOL_COLLISION_CAR_FRONT = 3,
    eVOL_COLLISION_CAR_SIDE = 4,
    eVOL_COLLISION_WORLD_ROLLOVER = 5,
    eVOL_COLLISION_WORLD_BOTTEMOUT = 6,
    eVOL_COLLISION_WORLD_WALL_FRONT = 7,
    eVOL_COLLISION_WORLD_WALL_SIDE = 8,
    eVOL_COLLISION_WORLD_CHAINFENCE = 9,
    eVOL_COLLISION_SMOKABLE_CAR = 10,
    eVOL_COLLISION_SMOKABLE_WORLD = 11,
    eVOL_COLLISION_SCRAPE = 12,
    eVRB_COLLISION_RVRB_COL = 13,
    eVRB_COLLISION_RVRB_SMAK = 14,
    eVRB_COLLISION_RVRB_SCRP = 15,
    ePCH_COLLISION_COL_PITCH = 16,
    ePCH_COLLISION_SMAK_PITCH = 17,
    ePCH_COLLISION_SCRAPE_PITCH = 18,
};

struct SFXObj_Collision : public CARSFX {
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

    SFXObj_Collision();
    ~SFXObj_Collision() override;

    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void Detach() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void Destroy() override;

    CSTATE_Collision *m_pCollisionState;      // offset 0x28, size 0x4
    Sound::CollisionEvent *m_pCollisionEvent; // offset 0x2C, size 0x4
    cStichWrapper *m_pCollisionStich;         // offset 0x30, size 0x4
    float m_fScrapeFade;                      // offset 0x34, size 0x4
    FX_Scrape *m_pcsisScrape;                 // offset 0x38, size 0x4
    SFXCTL_3DObjPos *m_p3DColPos;             // offset 0x3C, size 0x4
    SFXCTL_3DObjPos *m_p3DScrapePos;          // offset 0x40, size 0x4
    eVOL_COLLISION VolSlot;                   // offset 0x44, size 0x4
    eVOL_COLLISION AzimSlot;                  // offset 0x48, size 0x4
    eVOL_COLLISION PitchSlot;                 // offset 0x4C, size 0x4
    eVOL_COLLISION ReverbSlot;                // offset 0x50, size 0x4
    bool FirstUpdate;                         // offset 0x54, size 0x1
    int InitialAz;                            // offset 0x58, size 0x4
    bVector3 vCollisionPos;                   // offset 0x5C, size 0x10
    bVector3 vScrapePos;                      // offset 0x6C, size 0x10
    bVector3 vVelocity;                       // offset 0x7C, size 0x10
};

#endif
