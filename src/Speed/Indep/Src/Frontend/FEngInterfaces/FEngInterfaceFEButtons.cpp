#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/FEng/FEButtonMap.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

void FEngSetCurrentButton(const char *pkg_name, uint32 hash) {
    FEPackage *pkg = FEPackageManager::Get()->FindPackage(pkg_name);
    if (pkg != nullptr) {
        FEButtonMap *map = pkg->GetButtonMap();
        FEObject *button = nullptr;
        for (unsigned long i = 0; i < map->GetCount(); i++) {
            if (map->GetButton(i)->NameHash == hash) {
                button = map->GetButton(i);
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
            if (map->GetButton(i)->NameHash == button_hash) {
                button = map->GetButton(i);
            }
        }
        if (button != nullptr) {
            if (enabled == true) {
                button->Flags &= ~FF_IgnoreButton;
            } else {
                button->Flags |= FF_IgnoreButton;
            }
        }
    }
}

bool FEngGetButtonState(const char *pkg_name, uint32 button_hash) {
    FEObject *obj = FEngFindObject(pkg_name, button_hash);
    if (obj != nullptr) {
        return (obj->Flags & FF_IgnoreButton) == 0;
    }
    return false;
}
