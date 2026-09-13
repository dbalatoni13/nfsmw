#ifndef FENITROUSGAUGE_H
#define FENITROUSGAUGE_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

// total size: 0x40
// Decl: 12
class NitrousGauge : public HudElement, public INos {
  public:
    NitrousGauge(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~NitrousGauge() override {} // Decl: 15

    void Update(IPlayer *player) override;
    void SetNos(float nos) override;

  private:
#ifdef EA_BUILD_A124
    bool mNosChanged;
#endif
    float mNos;                   // offset 0x30, size 0x4, Decl: 22
    FEObject *mpDataNosMeterIcon; // offset 0x34, size 0x4, Decl: 26
    FEMultiImage *mpNosMeterBar;  // offset 0x38, size 0x4, Decl: 27
};

#endif
