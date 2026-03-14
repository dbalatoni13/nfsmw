#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"

extern bool g_bCustomizeInBackRoom;
extern bool g_bCustomizeInPerformance;
extern bool g_bCustomizeInParts;
extern eCustomizeEntryPoint g_TheCustomizeEntryPoint;
extern FECarRecord *g_pCustomizeCarRecordToUse;
extern const char lbl_803E52E4[];

bool CustomizeIsInBackRoom() {
    return g_bCustomizeInBackRoom;
}

void CustomizeSetInBackRoom(bool b) {
    g_bCustomizeInBackRoom = b;
}

bool CustomizeIsInPerformance() {
    return g_bCustomizeInPerformance;
}

void CustomizeSetInPerformance(bool b) {
    g_bCustomizeInPerformance = b;
}

bool CustomizeIsInParts() {
    return g_bCustomizeInParts;
}

void CustomizeSetInParts(bool b) {
    g_bCustomizeInParts = b;
}

void BeginCarCustomize(eCustomizeEntryPoint entry_point, FECarRecord *theCustomCar) {
    CustomizeSetInBackRoom(false);
    CustomizeSetInPerformance(false);
    CustomizeSetInParts(false);
    if (entry_point != CEP_GAMEPLAY) {
        cFEng::Get()->QueuePackageSwitch(lbl_803E52E4, 0, 0, false);
    }
    g_TheCustomizeEntryPoint = entry_point;
    g_pCustomizeCarRecordToUse = theCustomCar;
}
