#ifndef __DEBUGCARCUSTOMIZE_HPP__
#define __DEBUGCARCUSTOMIZE_HPP__

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

// total size: 0xC
class DebugCar : public bTNode<DebugCar> {
  public:
    DebugCar(FECarHandle handle) : mHandle(handle) {}
    ~DebugCar() {}

    FECarHandle mHandle; // offset 0x8, size 0x4
};

// total size: 0x80
class DebugCarCustomizeScreen : public MenuScreen {
  public:
    // total size: 0x4C
    class DebugCarOption : public bTNode<DebugCarOption> {
      public:
        DebugCarOption(const char *name, int value) : Intval(value) {
            bStrNCpy(String, name, 0x40);
        }

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

  private:
    void LoadCurrentCar();
    void BuildOptionsLists();
    void RebuildPartsList();
    void Redraw();
    void NewPreviewPart();
    void InstallPreviewingPart();
    void DumpPresetRide();
    DebugCarOption *FindElement(bTList<DebugCarOption> &list, int id);

  public:
    DebugCarCustomizeScreen(ScreenConstructorData *sd);
    ~DebugCarCustomizeScreen() override;

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
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
