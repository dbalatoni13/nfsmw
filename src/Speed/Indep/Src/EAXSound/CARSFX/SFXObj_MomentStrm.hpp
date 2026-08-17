//
#ifndef SFXObj_MomentStrm_HPP
#define SFXObj_MomentStrm_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/Generated/Messages/MGamePlayMoment.h"
#include "Speed/Indep/Src/Generated/Messages/MPursuitBreaker.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

// total size: 0x8
// Decl: 12
struct stMomentMapping {
    stMomentMapping(int _MarkerType, Attrib::Key _MomentType)
        : MarkerType(_MarkerType), //
          MomentType(_MomentType) {} // Decl: 14

    int MarkerType;         // offset 0x0, size 0x4, Decl: 20
    Attrib::Key MomentType; // offset 0x4, size 0x4, Decl: 21
};

// total size: 0x590
// Decl: 26
class SFXObj_MomentStrm : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_MomentStrm();
    ~SFXObj_MomentStrm() override;

    // Overrides: SndBase
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;

    static void CBPlayMomentStream();
    bool ShouldStreamPlay(Attrib::Key key, bool IsQueueing, float dist_sqrd);
    void CommitStreamReq(UMath::Vector4 pos4, Attrib::Key collectionkey);
    void ReceiveMoment(const MGamePlayMoment &message);
    void ReceivePursuitBreaker(const MPursuitBreaker &message);

    Hermes::HHANDLER mMsgReceiveMoment;  // offset 0x28, size 0x4, Decl: 51
    Hermes::HHANDLER mMsgPursuitBreaker; // offset 0x2C, size 0x4, Decl: 55

    // Decl: 60
    struct stMomentDecription {
        stMomentDecription() {} // Decl: 61

        UMath::Vector4 vPos; // offset 0x0, size 0x10, Decl: 68
        Attrib::Key key;     // offset 0x10, size 0x4, Decl: 69
    };

    typedef UTL::FixedVector<SFXObj_MomentStrm::stMomentDecription, 64, 16> MomentList;

    MomentList mMomentPositonsList; // offset 0x30, size 0x510, Decl: 74

    SFXCTL_3DObjPos *m_p3DPos;          // offset 0x540, size 0x4, Decl: 77
    eVOL_MOMENT VolSlot;                // offset 0x544, size 0x4, Decl: 79
    bool m_IsPositioned;                // offset 0x548, size 0x1, Decl: 80
    static float m_TimeBeforeRetrigger; // size: 0x4, address: 0x8041836C

    bVector3 fPosition; // offset 0x54C, size 0x10, Decl: 83
    bVector3 fVector;   // offset 0x55C, size 0x10, Decl: 84
    bVector3 fVelocity; // offset 0x56C, size 0x10, Decl: 85

    Attrib::Key m_CurMoment;         // offset 0x57C, size 0x4, Decl: 87
    static bool bHoldStream;         // size: 0x1, address: 0x80418368
    stMomentDecription *mHeldMoment; // offset 0x580, size 0x4, Decl: 89
    bool UseUserPos;                 // offset 0x584, size 0x1, Decl: 90

    unsigned int mCarsID; // offset 0x588, size 0x4, Decl: 93
    bool mbUseTRafficsID; // offset 0x58C, size 0x1, Decl: 94
};

// total size: 0x64
// Decl: 101
class SFXCTL_3DMomentPos : SFXCTL_3DObjPos {
  public:
    DECLARE_CREATABLE();
};

#endif
