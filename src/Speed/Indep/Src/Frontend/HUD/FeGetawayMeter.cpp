#include "Speed/Indep/Src/Frontend/HUD/FeGetawayMeter.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"

unsigned long FEHashUpper(const char *name);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);

extern const char lbl_803E46AC[];
extern const char lbl_803E46C4[];
extern const char lbl_803E46DC[];
extern const float lbl_803E46EC;

GetAwayMeter::GetAwayMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x200) //
    , IGetAwayMeter(pOutter) //
    , mGetawayDistance(lbl_803E46EC) //
{
    RegisterGroup(FEHashUpper(lbl_803E46AC));
    mpDataDistanceBar = FEngFindObject(pkg_name, FEHashUpper(lbl_803E46C4));
    mpDataDistanceString = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E46DC)));
}

void GetAwayMeter::Update(IPlayer *player) {}

void GetAwayMeter::SetGetAwayDistance(float distance) {
    mGetawayDistance = distance;
}
