#include "Speed/Indep/Src/Frontend/HUD/FeGetawayMeter.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"

GetAwayMeter::GetAwayMeter(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x200), IGetAwayMeter(pOutter)

{
    mGetawayDistance = 0;
    reinterpret_cast<HudElement *>(this)->RegisterGroup(FEHashUpper("GetawayDistanceMeter"));
    mpDataDistanceBar = FEngFindObject(pkg_name, FEHashUpper("Getaway_Distance_Bar"));
    mpDataDistanceString = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("Distance_Number")));
}

void GetAwayMeter::Update(IPlayer *player) {}
