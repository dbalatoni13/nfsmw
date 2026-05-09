#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETVD_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETVD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

class RapSheetVDDatum : public ArrayDatum {
  public:
    RapSheetVDDatum(uint32 carHash, uint32 statusHash, int bounty, int fines, int unserved, int evaded, int busted)
        : ArrayDatum(0, 0), CarHash(carHash), StatusHash(statusHash), Bounty(bounty), Fines(fines), Unserved(unserved), Evaded(evaded),
          Busted(busted) {}
    ~RapSheetVDDatum() override {}
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override {};

    uint32 getCarHash() {
        return CarHash;
    }
    uint32 getStatusHash() {
        return StatusHash;
    }
    int getBounty() {
        return Bounty;
    }
    int getFines() {
        return Fines;
    }
    int getUnserved() {
        return Unserved;
    }
    int getEvaded() {
        return Evaded;
    }
    int getBusted() {
        return Busted;
    }

  private:
    uint32 CarHash;    // offset 0x24, size 0x4
    uint32 StatusHash; // offset 0x28, size 0x4
    int Bounty;        // offset 0x2C, size 0x4
    int Fines;         // offset 0x30, size 0x4
    int Unserved;      // offset 0x34, size 0x4
    int Evaded;        // offset 0x38, size 0x4
    int Busted;        // offset 0x3C, size 0x4
};

class RapSheetVDArraySlot : public ArraySlot {
  public:
    RapSheetVDArraySlot(FEString *CarName, FEString *Bounty, FEString *Fines, FEString *Unserved, FEString *ToDrive, FEString *Evaded,
                        FEString *Busted)
        : ArraySlot(reinterpret_cast<FEObject *>(ToDrive)), pCar(CarName), pBounty(Bounty), pFines(Fines), pUnserved(Unserved), pToDrive(ToDrive),
          pEvaded(Evaded), pBusted(Busted) {}
    ~RapSheetVDArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

  private:
    FEString *pCar;      // offset 0x14, size 0x4
    FEString *pBounty;   // offset 0x18, size 0x4
    FEString *pFines;    // offset 0x1C, size 0x4
    FEString *pUnserved; // offset 0x20, size 0x4
    FEString *pToDrive;  // offset 0x24, size 0x4
    FEString *pEvaded;   // offset 0x28, size 0x4
    FEString *pBusted;   // offset 0x2C, size 0x4
};

class uiRapSheetVD : public ArrayScrollerMenu {
  public:
    uiRapSheetVD(ScreenConstructorData *sd);
    ~uiRapSheetVD() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader() override;

  private:
    void Setup();
};

#endif
