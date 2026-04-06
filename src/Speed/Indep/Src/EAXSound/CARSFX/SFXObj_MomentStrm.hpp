#ifndef SPEED_INDEP_SRC_EAXSOUND_CARSFX_SFXOBJ_MOMENTSTRM_HPP
#define SPEED_INDEP_SRC_EAXSOUND_CARSFX_SFXOBJ_MOMENTSTRM_HPP

#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/aud_moment_strm.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class MGamePlayMoment;
class MPursuitBreaker;

class SFXObj_MomentStrm : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    struct stMomentDecription {
        UMath::Vector4 vPos; // offset 0x0, size 0x10
        unsigned int key;    // offset 0x10, size 0x4

        stMomentDecription() {}
    };

    typedef UTL::FixedVector<stMomentDecription, 64, 16> MomentList;

    static float m_TimeBeforeRetrigger;
    static bool bHoldStream;

    HHANDLER mMsgReceiveMoment;   // offset 0x28, size 0x4
    HHANDLER mMsgPursuitBreaker;  // offset 0x2C, size 0x4
    MomentList mMomentPositonsList;   // offset 0x30, size 0x510
    SFXCTL_3DObjPos *m_p3DPos;        // offset 0x540, size 0x4
    eVOL_MOMENT VolSlot;              // offset 0x544, size 0x4
    bool m_IsPositioned;              // offset 0x548, size 0x1
    bVector3 fPosition;               // offset 0x54C, size 0x10
    bVector3 fVector;                 // offset 0x55C, size 0x10
    bVector3 fVelocity;               // offset 0x56C, size 0x10
    unsigned int m_CurMoment;         // offset 0x57C, size 0x4
    stMomentDecription *mHeldMoment;  // offset 0x580, size 0x4
    bool UseUserPos;                  // offset 0x584, size 0x1
    unsigned int mCarsID;             // offset 0x588, size 0x4
    bool mbUseTRafficsID;             // offset 0x58C, size 0x1

    SFXObj_MomentStrm();
    virtual ~SFXObj_MomentStrm();

    virtual TypeInfo *GetTypeInfo() const;
    virtual const char *GetTypeName() const;

    virtual int GetController(int Index);
    virtual void AttachController(SFXCTL *psfxctl);
    virtual void SetupSFX(CSTATE_Base *_StateBase);
    virtual void InitSFX();
    virtual void Destroy();
    virtual void UpdateParams(float t);
    virtual void ProcessUpdate();

    static SndBase *CreateObject(unsigned int allocator);

    void ReceiveMoment(const MGamePlayMoment &message);
    void ReceivePursuitBreaker(const MPursuitBreaker &message);
    bool ShouldStreamPlay(unsigned int collectionkey, bool holdstream, float timebeforetrigger);
    void CommitStreamReq(UMath::Vector4 pos4, unsigned int collectionkey);
    static void CBPlayMomentStream();
};

class SFXCTL_3DMomentPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    SFXCTL_3DMomentPos() {}
    virtual ~SFXCTL_3DMomentPos() {}

    virtual TypeInfo *GetTypeInfo() const;
    virtual const char *GetTypeName() const;

    static SndBase *CreateObject(unsigned int allocator);
};

#endif
