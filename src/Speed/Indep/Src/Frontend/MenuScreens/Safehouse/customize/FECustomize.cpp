#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/Customize/FECustomize.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

bool g_bCustomizeInBackRoom = false;
bool g_bCustomizeInPerformance = false;
bool g_bCustomizeInParts = false;
eCustomizeEntryPoint g_TheCustomizeEntryPoint = static_cast<eCustomizeEntryPoint>(1);
FECarRecord *g_pCustomizeCarRecordToUse = nullptr;

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

// .rodata:0x803E89B0 (4 B, scope:global, float) = 0.25f, leido del ELF original.
// La direccion cae en el rango .rodata de zFe2, no en el de zFeOverlay: quien lo
// usa (CarCustomize.cpp y CustomizeManager.cpp, los dos de zFeOverlay) solo lo
// declara, y el zFeOverlay.o extraido lo tiene UND.
// AL FINAL DEL FICHERO a proposito (regla de la r31): con el inicializador
// visible en el punto de uso GCC pliega la carga de un `const`.
extern const float gTradeInFactor;
extern const float gTradeInFactor = 0.25f;
