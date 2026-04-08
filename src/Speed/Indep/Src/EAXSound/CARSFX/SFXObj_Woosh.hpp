#ifndef EAXSOUND_CARSFX_SFXOBJ_WOOSH_H
#define EAXSOUND_CARSFX_SFXOBJ_WOOSH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/STICH_Playback.h"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

enum STICH_WHOOSH_TYPE {
    WHSH_Bridge_Slow_01 = 0,
    WHSH_Bridge_Slow_02 = 1,
    WHSH_Bridge_Slow_03 = 2,
    WHSH_Bridge_Med_01 = 3,
    WHSH_Bridge_Med_02 = 4,
    WHSH_Bridge_Med_03 = 5,
    WHSH_Bridge_Fast_01 = 6,
    WHSH_Bridge_Fast_02 = 7,
    WHSH_Bridge_Fast_03 = 8,
    WHSH_Post_Slow_01 = 9,
    WHSH_Post_Slow_02 = 10,
    WHSH_Post_Slow_03 = 11,
    WHSH_Post_Slow_04 = 12,
    WHSH_Post_Slow_05 = 13,
    WHSH_Post_Slow_06 = 14,
    WHSH_Post_Med_01 = 15,
    WHSH_Post_Med_02 = 16,
    WHSH_Post_Med_03 = 17,
    WHSH_Post_Med_04 = 18,
    WHSH_Post_Med_05 = 19,
    WHSH_Post_Med_06 = 20,
    WHSH_Post_Fast_01 = 21,
    WHSH_Post_Fast_02 = 22,
    WHSH_Post_Fast_03 = 23,
    WHSH_Post_Fast_04 = 24,
    WHSH_Post_Fast_05 = 25,
    WHSH_Post_Fast_06 = 26,
    WHSH_Pre_Med_01 = 27,
    WHSH_Pre_Med_02 = 28,
    WHSH_Pre_Med_03 = 29,
    WHSH_Pre_Big_01 = 30,
    WHSH_Pre_Big_02 = 31,
    WHSH_Pre_Big_03 = 32,
    WHSH_Smack_Med_01 = 33,
    WHSH_Smack_Med_02 = 34,
    WHSH_Smack_Med_03 = 35,
    WHSH_Smack_Med_04 = 36,
    WHSH_Smack_Med_05 = 37,
    WHSH_Smack_Med_06 = 38,
    WHSH_Smack_Fast_01 = 39,
    WHSH_Smack_Fast_02 = 40,
    WHSH_Smack_Fast_03 = 41,
    WHSH_Smack_Fast_04 = 42,
    WHSH_Smack_Fast_05 = 43,
    WHSH_Smack_Fast_06 = 44,
    WHSH_Traf_Med_01 = 45,
    WHSH_Traf_Med_02 = 46,
    WHSH_Traf_Med_03 = 47,
    WHSH_Traf_Med_04 = 48,
    WHSH_Traf_Med_05 = 49,
    WHSH_Traf_Med_06 = 50,
    WHSH_Traf_Fst_01 = 51,
    WHSH_Traf_Fst_02 = 52,
    WHSH_Traf_Fst_03 = 53,
    WHSH_Traf_Fst_04 = 54,
    WHSH_Traf_Fst_05 = 55,
    WHSH_Traf_Fst_06 = 56,
    WHSH_Tree_Slow_01 = 57,
    WHSH_Tree_Slow_02 = 58,
    WHSH_Tree_Slow_03 = 59,
    WHSH_Tree_Slow_04 = 60,
    WHSH_Tree_Slow_05 = 61,
    WHSH_Tree_Slow_06 = 62,
    WHSH_Tree_Med_01 = 63,
    WHSH_Tree_Med_02 = 64,
    WHSH_Tree_Med_03 = 65,
    WHSH_Tree_Med_04 = 66,
    WHSH_Tree_Med_05 = 67,
    WHSH_Tree_Med_06 = 68,
    WHSH_Tree_Fast_01 = 69,
    WHSH_Tree_Fast_02 = 70,
    WHSH_Tree_Fast_03 = 71,
    WHSH_Tree_Fast_04 = 72,
    WHSH_Tree_Fast_05 = 73,
    WHSH_Tree_Fast_06 = 74,
    WHSH_Tunnel_Slow_01 = 75,
    WHSH_Tunnel_Slow_02 = 76,
    WHSH_Tunnel_Slow_03 = 77,
    WHSH_Tunnel_Med_01 = 78,
    WHSH_Tunnel_Med_02 = 79,
    WHSH_Tunnel_Med_03 = 80,
    WHSH_Tunnel_Fast_01 = 81,
    WHSH_Tunnel_Fast_02 = 82,
    WHSH_Tunnel_Fast_03 = 83,
    WHSH_TunOut_Med_01 = 84,
    WHSH_TunOut_Med_02 = 85,
    WHSH_TunOut_Med_03 = 86,
    WHSH_TunOut_Fast_01 = 87,
    WHSH_TunOut_Fast_02 = 88,
    WHSH_TunOut_Fast_03 = 89,
};

void GetWooshBlockSizeParams(eDRIVE_BY_TYPE type, STICH_WHOOSH_TYPE &base, int &numblocks, int &sizeperblock);

struct SFXObj_Woosh : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    // total size: 0x50
    SND_Stich *m_pStitchData;        // offset 0x28, size 0x4
    SND_Params m_SndParams;          // offset 0x2C, size 0x18
    cStichWrapper *m_pWooshStich;    // offset 0x44, size 0x4
    CSTATE_DriveBy *m_pDriveByState; // offset 0x48, size 0x4
    SFXCTL_3DObjPos *m_p3DPos;       // offset 0x4C, size 0x4

    SFXObj_Woosh();
    ~SFXObj_Woosh() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    void Detach() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void Destroy() override;
};

// total size: 0x64
struct SFXCTL_3DWooshPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DWooshPos() {}
    ~SFXCTL_3DWooshPos() override {}

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);
};

#endif
