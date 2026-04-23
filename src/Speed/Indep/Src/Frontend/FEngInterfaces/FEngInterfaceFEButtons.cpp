#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"

void FEngSetCurrentButton(const char* pkg_name, unsigned int hash) {
    FEPackage* pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != nullptr) {
        FEButtonMap* map = pkg->GetButtonMap();
        FEObject* button = nullptr;
        for (unsigned long i = 0; i < map->GetCount(); i++) {
            FEObject* btn = map->GetButton(i);
            if (btn->NameHash == hash) {
                button = btn;
            }
        }
        if (button != nullptr) {
            pkg->SetCurrentButton(button, true);
        }
    }
}

FEObject* FEngGetCurrentButton(const char* pkg_name) {
    FEPackage* pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    FEObject* obj = nullptr;
    if (pkg != nullptr) {
        obj = pkg->GetCurrentButton();
    }
    return obj;
}

void FEngSetButtonState(const char* pkg_name, unsigned int button_hash, bool enabled) {
    FEPackage* pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != nullptr) {
        FEButtonMap* map = pkg->GetButtonMap();
        FEObject* button = nullptr;
        for (unsigned long i = 0; i < map->GetCount(); i++) {
            FEObject* btn = map->GetButton(i);
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