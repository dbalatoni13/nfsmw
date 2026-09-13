#ifndef __CUSTOMIZEMANAGER_HPP__
#define __CUSTOMIZEMANAGER_HPP__

#include <types.h>
#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"

enum ePerformanceRatingType {
    PRT_TOP_SPEED = 0,
    PRT_HANDLING = 1,
    PRT_ACCELERATION = 2,
};

// total size: 0x1C4
// Decl: 218
class CarCustomizeManager {
  public:
    CarCustomizeManager() : ThePVehicle(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr), PreviewRecord() {}

    void TakeControl(eCustomizeEntryPoint entry_point, FECarRecord *tuning_car);

    void RelinquishControl();

    bool CanTradeIn(SelectablePart *part);

    void AddToCart(SelectablePart *part);

    bool RemoveFromCart(ShoppingCartItem *item); // Decl: 230

    void AddRemovalCarPart(CAR_SLOT_ID slot_id); // Decl: 229

    ShoppingCartItem *IsPartTypeInCart(SelectablePart *to_find);

    ShoppingCartItem *IsPartTypeInCart(uint32 slot_id);

    ShoppingCartItem *IsPartTypeInCart(Physics::Upgrades::Type type);

    ShoppingCartItem *IsPartInCart(SelectablePart *to_find);

    CarPart *GetActivePartFromSlot(uint32 slot_id);

    ShoppingCartItem *GetFirstCartItem() {
        return static_cast<ShoppingCartItem *>(ShoppingCart.GetHead());
    }

    ShoppingCartItem *GetLastCartItem() {
        return static_cast<ShoppingCartItem *>(ShoppingCart.GetTail());
    }

    int GetNumCartItems() { // Decl: 237
        return ShoppingCart.CountElements();
    }

    ShoppingCartItem *GetCartItem(int index) { // Decl: 236
        return ShoppingCart.GetNode(index);
    }

    int GetCartTotal(eCustomizeCartTotals type); // Decl: 248

    void Checkout(); // Decl: 249

    void EmptyCart() { // Decl: 250
        ShoppingCart.DeleteAllElements();
        ResetPreview();
    }

    bool DoesCartHaveActiveParts();

    int GetPartPrice(SelectablePart *part);

    SelectablePart *GetTempColoredPart() {
        return TheTempColoredPart;
    }

    void SetTempColoredPart(SelectablePart *part);

    void ClearTempColoredPart();

    CarPart *GetStockCarPart(uint32 slot_id);

    void ResetToStockCarParts();

    void ResetToStockPerformance();

    void ResetPreview(); // Decl: 255

    void PreviewPart(int slot, CarPart *part);

    void InstallPart(int slot, CarPart *part);

    CarPart *GetInstalledCarPart(int slot_id);

    void PreviewPerfPkg(Physics::Upgrades::Type part_type, int level);

    void InstallPerfPkg(Physics::Upgrades::Type part_type, int level);

    int GetInstalledPerfPkg(Physics::Upgrades::Type type); // Decl: 287

    bool IsJunkmanInstalled(Physics::Upgrades::Type type);

    int GetMaxPackages(Physics::Upgrades::Type type);

    int GetNumPackages(Physics::Upgrades::Type type);

    void MaxOutPerformance(); // Decl: 290

    float GetPerformanceRating(ePerformanceRatingType type, bool preview);

    void UpdateHeatOnVehicle(SelectablePart *part, FECareerRecord *record);

    eUnlockFilters GetUnlockFilter();

    uint32 GetUnlockHash(eCustomizeCategory cat, int upgrade_lvl);

    bool IsPartInstalled(SelectablePart *part); // Decl: 292

    bool IsPartLocked(SelectablePart *part, int perf_unlock_level);

    bool IsPartNew(SelectablePart *part, int perf_unlock_level);

    bool IsCategoryNew(uint32 cat);

    bool IsCategoryLocked(uint32 cat, bool backroom);

    bool IsRimCategoryLocked(unsigned int cat, bool backroom);

    bool IsVinylCategoryLocked(unsigned int cat, bool backroom);

    int GetOuterRadius();

    int GetMinInnerRadius();

    int GetMaxInnerRadius();

    void GetCarPartList(int car_slot, bTList<SelectablePart> &the_list, unsigned int param);

    void GetPerformancePartsList(Physics::Upgrades::Type type, bTList<SelectablePart> &the_list);

    CarType GetTuningCarType() {} // Decl: 325

    void SetInBackRoom(bool in_back) {
        CustomizeSetInBackRoom(in_back);
    }

    bool IsInBackRoom() {
        return CustomizeIsInBackRoom();
    }

    bool CanInstallJunkman(Physics::Upgrades::Type type);

    bool IsCareerMode(); // Decl: 334

    float GetMaxRPM() {
        return Physics::Info::Redline(ThePVehicle);
    }

    bool IsTurbo();

    void SetInPerformance(bool b) {
        CustomizeSetInPerformance(b);
    }

    bool IsInPerformance() {
        return CustomizeIsInPerformance();
    }

    void SetInParts(bool b) {
        CustomizeSetInParts(b);
    }

    bool IsInParts() {
        return CustomizeIsInParts();
    }

    float GetActualHeat(); // Decl: 306

    float GetActualRep() {}

    float GetPreviewHeat(SelectablePart *part); // Decl: 307

    float GetPreviewRep() {}

    float GetCartHeat();

    const FECustomizationRecord *GetPreviewRecord() { // Decl: 285
        return &PreviewRecord;
    }

    const FECarRecord *GetTuningCar() { // Decl: 328
        return TuningCar;
    }

    int GetNumCustomizeMarkers();

    bool IsCastrolCar();

    bool IsRotaryCar();

    bool IsHeroCar();

  private:
    float GetCartRep() {}

  private:
    eCustomizeEntryPoint EntryPoint;       // offset 0x0, size 0x4, Decl: 371
    FECarRecord *TuningCar;                // offset 0x4, size 0x4, Decl: 373
    Attrib::Gen::pvehicle ThePVehicle;     // offset 0x8, size 0x14, Decl: 374
    FECustomizationRecord PreviewRecord;   // offset 0x1C, size 0x198, Decl: 375
    bTList<ShoppingCartItem> ShoppingCart; // offset 0x1B4, size 0x8, Decl: 378
    int NumPartsInCart;                    // offset 0x1BC, size 0x4, Decl: 379
    SelectablePart *TheTempColoredPart;    // offset 0x1C0, size 0x4
};

extern CarCustomizeManager gCarCustomizeManager;

#endif
