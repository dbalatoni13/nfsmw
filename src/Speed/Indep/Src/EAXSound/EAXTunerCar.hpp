//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef EAX_TUNERCAR_HPP
#define EAX_TUNERCAR_HPP // Decl: 16

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

#define UP_SHIFTING_ENGAGING_REATTACH_MAX_T 800.0f // Decl: 30

// total size: 0x134
// Decl: 35
class EAXTunerCar : public EAXCar {
  public:
    DECLARE_STATETYPE();

    EAXTunerCar();
    ~EAXTunerCar() override;

    // Overrides: CSTATE_Base
    void PreLoadAssets() override;
    void UpdateParams(float t) override;

    virtual int Play(void *peventst) {} // Decl: 58

    // Overrides: EAXCar
    void ProcessSoundSphere(uint32 unamehash, int nparamid, bVector3 *pv3pos, float fradius) override;
    int SFXMessage(eSFXMessageType SFXMessageType, uint32 param1, uint32 param2) override;

    int m_playerID; // offset 0x114, size 0x4, Decl: 64

    float m_fCarVolume; // offset 0x118, size 0x4, Decl: 66

    bool BottomOutPlay;     // offset 0x11C, size 0x1, Decl: 70
    int BottomOutIntensity; // offset 0x120, size 0x4, Decl: 71

    bool TrunkBouncePlay;        // offset 0x124, size 0x1, Decl: 75
    float TrunkBounceInstensity; // offset 0x128, size 0x4, Decl: 76

    bool PlayBackFire; // offset 0x12C, size 0x1, Decl: 78

  protected:
    bool bFirstUpdate; // offset 0x130, size 0x1, Decl: 82

    void FirstUpdate(float t);
    void UpdateAccel();

    // Overrides: EAXCar
    void UpdatePov() override;
    int UpdateRotation() override;
};

#endif
