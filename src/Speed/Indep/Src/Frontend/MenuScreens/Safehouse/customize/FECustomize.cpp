#include <types.h>
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

bool g_bCustomizeInBackRoom;
bool g_bCustomizeInPerformance;
bool g_bCustomizeInParts;
eCustomizeEntryPoint g_TheCustomizeEntryPoint;
FECarRecord *g_pCustomizeCarRecordToUse;

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 30)
void BeginCarCustomize(eCustomizeEntryPoint entry_point, FECarRecord *theCustomCar) {
    CustomizeSetInBackRoom(false);
    CustomizeSetInPerformance(false);
    CustomizeSetInParts(false);
    if (entry_point) {
        cFEng::Get()->QueuePackageSwitch("CustomizeMain.fng", 0, 0, false);
    }
    g_TheCustomizeEntryPoint = entry_point;
    g_pCustomizeCarRecordToUse = theCustomCar;
}

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 45)
bool CustomizeIsInBackRoom() {
    return g_bCustomizeInBackRoom;
}

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 50)
void CustomizeSetInBackRoom(bool b) {
    g_bCustomizeInBackRoom = b;
}

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 55)
bool CustomizeIsInPerformance() {
    return g_bCustomizeInPerformance;
}

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 60)
void CustomizeSetInPerformance(bool b) {
    g_bCustomizeInPerformance = b;
}

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 65)
bool CustomizeIsInParts() {
    return g_bCustomizeInParts;
}

// Decl: speed/indep/src/frontend/menuscreens/safehouse/customize/FECustomize.cpp (line 70)
void CustomizeSetInParts(bool b) {
    g_bCustomizeInParts = b;
}
