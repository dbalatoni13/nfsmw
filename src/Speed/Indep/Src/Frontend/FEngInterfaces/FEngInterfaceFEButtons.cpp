#include "FEngInterfaceFEButtons.hpp"

#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"

void FEngSetCurrentButton(const char *pkg_name, unsigned int hash) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != 0) {
        FEButtonMap *map = pkg->GetButtonMap();
        FEObject *button = 0;
        for (u32 i = 0; i < map->GetCount(); i++) {
            FEObject *obj = map->GetButton(i);
            if (obj->NameHash == hash) {
                button = obj;
            }
        }
        if (button != 0) {
            pkg->SetCurrentButton(button, true);
        }
    }
}

FEObject *FEngGetCurrentButton(const char *pkg_name) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    FEObject *obj = 0;
    if (pkg != 0) {
        obj = pkg->GetCurrentButton();
    }
    return obj;
}

void FEngSetButtonState(const char *pkg_name, unsigned int button_hash, bool enabled) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != 0) {
        FEButtonMap *map = pkg->GetButtonMap();
        FEObject *button = 0;
        for (u32 i = 0; i < map->GetCount(); i++) {
            FEObject *obj = map->GetButton(i);
            if (obj->NameHash == button_hash) {
                button = obj;
            }
        }
        if (button != 0) {
            if (enabled == true) {
                button->Flags &= ~0x04000000;
            } else {
                button->Flags |= 0x04000000;
            }
        }
    }
}
