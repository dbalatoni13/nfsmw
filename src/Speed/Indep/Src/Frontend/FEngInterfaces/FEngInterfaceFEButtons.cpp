#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/FEng/FEButtonMap.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"

// File: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEButtons.cpp
// total size: 0xC
// Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEButtons.cpp:93
struct ScreenButtonDatum {
    uint32 ScreenHash; // offset 0x0, size 0x4, Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEButtons.cpp:94
    uint8 LastButton;  // offset 0x4, size 0x1, Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEButtons.cpp:95
    uint32 GameMode;   // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEButtons.cpp:96
};

void FEngSetCurrentButton(const char *pkg_name, uint32 hash) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != nullptr) {
        FEButtonMap *map = pkg->GetButtonMap();
        FEObject *button = nullptr;
        for (unsigned long i = 0; i < map->GetCount(); i++) {
            FEObject *btn = map->GetButton(i);
            if (btn->NameHash == hash) {
                button = btn;
            }
        }
        if (button != nullptr) {
            pkg->SetCurrentButton(button, true);
        }
    }
}

FEObject *FEngGetCurrentButton(const char *pkg_name) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    FEObject *obj = nullptr;
    if (pkg != nullptr) {
        obj = pkg->GetCurrentButton();
    }
    return obj;
}

void FEngSetButtonState(const char *pkg_name, uint32 button_hash, bool enabled) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != nullptr) {
        FEButtonMap *map = pkg->GetButtonMap();
        FEObject *button = nullptr;
        for (unsigned long i = 0; i < map->GetCount(); i++) {
            FEObject *btn = map->GetButton(i);
            if (btn->NameHash == button_hash) {
                button = btn;
            }
        }
        if (button != nullptr) {
            if (enabled == true) {
                button->Flags &= ~0x4000000;
            } else {
                button->Flags |= 0x4000000;
            }
        }
    }
}
