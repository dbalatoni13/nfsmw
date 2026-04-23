#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_DEBUGCARCUSTOMIZE_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_DEBUGCARCUSTOMIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

struct CarPart;
struct FECustomizationRecord;

// total size: 0xC
struct DebugCar : public bTNode<DebugCar> {
    DebugCar(unsigned int handle) : mHandle(handle) {}
    ~DebugCar() {}

    unsigned int mHandle; // offset 0x8, size 0x4
};

// total size: 0x80
struct DebugCarCustomizeScreen : public MenuScreen {
    // total size: 0x4C
    struct DebugCarOption : public bTNode<DebugCarOption> {
        DebugCarOption(const char *name, int value);

        int GetValue() {
            return Intval;
        }
        char *GetString() {
            return String;
        }
        ~DebugCarOption() {}

        char String[64]; // offset 0x8, size 0x40
        int Intval;      // offset 0x48, size 0x4
    };

    DebugCarCustomizeScreen(ScreenConstructorData *sd);
    ~DebugCarCustomizeScreen() override;

    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    DebugCarOption *FindElement(bTList<DebugCarOption> &list, int id);
    void BuildOptionsLists();
    void LoadCurrentCar();
    void RebuildPartsList();
    void NewPreviewPart();
    void InstallPreviewingPart();
    void DumpPresetRide();
    void Redraw();

    bTList<DebugCar> FilteredCarsList;        // offset 0x2C, size 0x8
    DebugCar *pDebugCar;                      // offset 0x34, size 0x4
    bTList<DebugCarOption> CarTypeNameHashes; // offset 0x38, size 0x8
    DebugCarOption *CurrentCarTypeNameHash;   // offset 0x40, size 0x4
    bTList<DebugCarOption> LookupCarSlotIDs;  // offset 0x44, size 0x8
    DebugCarOption *CurrentLookupSlotID;      // offset 0x4C, size 0x4
    bTList<DebugCarOption> CarPartNameHashes; // offset 0x50, size 0x8
    DebugCarOption *CurrentPartNameHash;      // offset 0x58, size 0x4
    bTList<DebugCarOption> InstallCarPartIDs; // offset 0x5C, size 0x8
    DebugCarOption *CurrentInstallPartID;     // offset 0x64, size 0x4
    bPList<CarPart> InstallableParts;         // offset 0x68, size 0x8
    bPNode *CurrentInstallablePart;           // offset 0x70, size 0x4
    FECustomizationRecord *custom;            // offset 0x74, size 0x4
    bool wasCarCustomized;                    // offset 0x78, size 0x1
    int iFastScroll;                          // offset 0x7C, size 0x4
};

#endif
