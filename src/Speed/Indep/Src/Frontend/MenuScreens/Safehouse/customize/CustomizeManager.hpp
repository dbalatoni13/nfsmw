#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_CUSTOMIZEMANAGER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_CUSTOMIZEMANAGER_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include <types.h>
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"

struct FECarRecord;
struct CarPart;

bool CustomizeIsInBackRoom();
void CustomizeSetInBackRoom(bool b);
bool CustomizeIsInPerformance();
void CustomizeSetInPerformance(bool b);
bool CustomizeIsInParts();
void CustomizeSetInParts(bool b);

struct CarCustomizeManager {
    ShoppingCartItem *GetFirstCartItem() {
        return static_cast<ShoppingCartItem *>(ShoppingCart.GetHead());
    }
    ShoppingCartItem *GetLastCartItem() {
        return static_cast<ShoppingCartItem *>(ShoppingCart.GetTail());
    }
    int GetNumCartItems() {
        return NumPartsInCart;
    }
    ShoppingCartItem *GetCartItem(int index);
    void EmptyCart();
    SelectablePart *GetTempColoredPart() {
        return TheTempColoredPart;
    }
    void SetInBackRoom(bool in_back) {
        CustomizeSetInBackRoom(in_back);
    }
    bool IsInBackRoom() {
        return CustomizeIsInBackRoom();
    }
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
    const FECustomizationRecord *GetPreviewRecord() {
        return &PreviewRecord;
    }
    const FECarRecord *GetTuningCar() {
        return TuningCar;
    }

    void TakeControl(eCustomizeEntryPoint entry_point, FECarRecord *tuning_car);
    void RelinquishControl();
    bool CanTradeIn(SelectablePart *part);
    void AddToCart(SelectablePart *part);
    bool RemoveFromCart(ShoppingCartItem *item);
    void AddRemovalCarPart(unsigned int slot_id);
    ShoppingCartItem *IsPartTypeInCart(SelectablePart *to_find);
    ShoppingCartItem *IsPartTypeInCart(unsigned int slot_id);
    ShoppingCartItem *IsPartTypeInCart(Physics::Upgrades::Type type);
    ShoppingCartItem *IsPartInCart(SelectablePart *to_find);
    CarPart *GetActivePartFromSlot(unsigned int slot_id);
    int GetCartTotal(eCustomizeCartTotals type);
    void Checkout();
    bool DoesCartHaveActiveParts();
    int GetPartPrice(SelectablePart *part);
    void SetTempColoredPart(SelectablePart *part);
    void ClearTempColoredPart();
    CarPart *GetStockCarPart(unsigned int slot_id);
    void ResetToStockCarParts();
    void ResetToStockPerformance();
    void PreviewPerformanceRating(ePerformanceRatingType rating, float *base, float *preview);
    void UpdatePerformanceRating(ePerformanceRatingType rating, float *base, float *preview);
    bool IsVinylColorSupported(int slot);
    void ClearVinylColors();
    float GetHeatFromParts();
    CarPart *GetInstalledCarPart(int slot_id);
    void PreviewPart(int slot_id, CarPart *part);
    void InstallPart(int slot_id, CarPart *part);
    void ResetPreview();
    void PreviewPerfPkg(Physics::Upgrades::Type part_type, int level);
    void InstallPerfPkg(Physics::Upgrades::Type part_type, int level);
    bool IsJunkmanInstalled(Physics::Upgrades::Type type);
    int GetInstalledPerfPkg(Physics::Upgrades::Type type);
    int GetMaxPackages(Physics::Upgrades::Type type);
    int GetNumPackages(Physics::Upgrades::Type type);
    void MaxOutPerformance();
    unsigned int GetUnlockHash(eCustomizeCategory cat, int upgrade_lvl);
    float GetPerformanceRating(ePerformanceRatingType type, bool preview);
    void UpdateHeatOnVehicle(SelectablePart *part, FECareerRecord *record);
    eUnlockFilters GetUnlockFilter();
    bool IsPartInstalled(SelectablePart *part);
    bool IsPartLocked(SelectablePart *part, int perf_unlock_level);
    bool IsPartNew(SelectablePart *part, int perf_unlock_level);
    bool IsCategoryNew(unsigned int cat);
    bool IsCategoryLocked(unsigned int cat, bool backroom);
    bool IsRimCategoryLocked(unsigned int cat, bool backroom);
    bool IsVinylCategoryLocked(unsigned int cat, bool backroom);
    int GetOuterRadius();
    int GetMinInnerRadius();
    int GetMaxInnerRadius();
    void GetCarPartList(int car_slot, bTList<SelectablePart> &the_list, unsigned int param);
    void GetPerformancePartsList(Physics::Upgrades::Type type, bTList<SelectablePart> &the_list);
    bool CanInstallJunkman(Physics::Upgrades::Type type);
    bool IsCareerMode();
    bool IsTurbo();
    float GetActualHeat();
    float GetPreviewHeat(SelectablePart *part);
    int GetNumCustomizeMarkers();
    bool IsCastrolCar();
    bool IsRotaryCar();
    bool IsHeroCar();
    float GetCartHeat();

    CarCustomizeManager()
        : ThePVehicle(static_cast<const Attrib::Collection *>(nullptr), 0, nullptr) //
          ,
          PreviewRecord() {}

    eCustomizeEntryPoint EntryPoint;       // offset 0x0, size 0x4
    FECarRecord *TuningCar;                // offset 0x4, size 0x4
    Attrib::Gen::pvehicle ThePVehicle;     // offset 0x8, size 0x14
    FECustomizationRecord PreviewRecord;   // offset 0x1C, size 0x198
    bTList<ShoppingCartItem> ShoppingCart; // offset 0x1B4, size 0x8
    int NumPartsInCart;                    // offset 0x1BC, size 0x4
    SelectablePart *TheTempColoredPart;    // offset 0x1C0, size 0x4
};

extern CarCustomizeManager gCarCustomizeManager;

#endif
