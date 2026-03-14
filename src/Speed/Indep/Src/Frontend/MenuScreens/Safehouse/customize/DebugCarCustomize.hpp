// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_DEBUGCARCUSTOMIZE_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_DEBUGCARCUSTOMIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

// total size: 0x80
struct DebugCarCustomizeScreen : public MenuScreen {
    // total size: 0x4C
    struct DebugCarOption : public bTNode<DebugCarOption> {
        DebugCarOption(const char *name, int value)
            : Intval(value) {
            // strncpy String from name
        }

        int GetValue() { return Intval; }
        char *GetString() { return String; }
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
    void ApplyCurrentSelection();
    void ScrollParts(enum eScrollDir dir);
    void ScrollOptions(enum eScrollDir dir);

    int currentPart;                       // offset 0x2C, size 0x4
    int currentOption;                     // offset 0x30, size 0x4
    bTList<DebugCarOption> parts;          // offset 0x34, size 0x8
    bTList<DebugCarOption> options;        // offset 0x3C, size 0x8
    int numParts;                          // offset 0x44, size 0x4
    int numOptions;                        // offset 0x48, size 0x4
    DebugCarOption *currentPartNode;       // offset 0x4C, size 0x4
    DebugCarOption *currentOptionNode;     // offset 0x50, size 0x4
    FEString *partString;                  // offset 0x54, size 0x4
    FEString *optionString;                // offset 0x58, size 0x4
    FEString *slotString;                  // offset 0x5C, size 0x4
    int partSlotId;                        // offset 0x60, size 0x4
    unsigned int currentCarHandle;         // offset 0x64, size 0x4
    unsigned int originalCarHandle;        // offset 0x68, size 0x4
    int currentCarSlot;                    // offset 0x6C, size 0x4
    int numCars;                           // offset 0x70, size 0x4
    bool editingCar;                       // offset 0x74, size 0x1
    bool showStock;                        // offset 0x78, size 0x1
    bool showAllCars;                      // offset 0x7C, size 0x1
};

#endif
