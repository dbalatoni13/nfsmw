#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_CUSTOMIZEMANAGER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CUSTOMIZE_CUSTOMIZEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#include <types.h>

struct FECarRecord;
struct pvehicle;

// total size: 0x1C4
struct CarCustomizeManager {
    ShoppingCartItem *GetFirstCartItem() { return static_cast<ShoppingCartItem *>(ShoppingCart.GetHead()); }
    ShoppingCartItem *GetLastCartItem() { return static_cast<ShoppingCartItem *>(ShoppingCart.GetTail()); }
    int GetNumCartItems() { return NumPartsInCart; }
    ShoppingCartItem *GetCartItem(int index) { return nullptr; }
    void EmptyCart() {}
    SelectablePart *GetTempColoredPart() { return TheTempColoredPart; }
    CarType GetTuningCarType() { return static_cast<CarType>(0); }
    void SetInBackRoom(bool in_back) {}
    bool IsInBackRoom() { return false; }
    float GetMaxRPM() { return 0.0f; }
    void SetInPerformance(bool b) {}
    bool IsInPerformance() { return false; }
    void SetInParts(bool b) {}
    bool IsInParts() { return false; }
    float GetActualRep() { return 0.0f; }
    float GetPreviewRep() { return 0.0f; }
    const FECustomizationRecord *GetPreviewRecord() { return &PreviewRecord; }
    const FECarRecord *GetTuningCar() { return TuningCar; }
    float GetCartRep() { return 0.0f; }

    void TakeControl(eCustomizeEntryPoint entry_point, FECarRecord *tuning_car);
    void RelinquishControl();
    bool CanTradeIn(SelectablePart *part);
    void AddToCart(SelectablePart *part);
    bool RemoveFromCart(ShoppingCartItem *item);
    void AddRemovalCarPart(unsigned int slot_id);
    ShoppingCartItem *IsPartTypeInCart(SelectablePart *to_find);
    ShoppingCartItem *IsPartTypeInCart(unsigned int slot_id);
    ShoppingCartItem *IsPartTypeInCart(enum Type type);
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

    eCustomizeEntryPoint EntryPoint;              // offset 0x0, size 0x4
    FECarRecord *TuningCar;                       // offset 0x4, size 0x4
    char ThePVehicle[0x14];                       // offset 0x8, size 0x14 (pvehicle)
    FECustomizationRecord PreviewRecord;          // offset 0x1C, size 0x198
    bTList<ShoppingCartItem> ShoppingCart;         // offset 0x1B4, size 0x8
    int NumPartsInCart;                            // offset 0x1BC, size 0x4
    SelectablePart *TheTempColoredPart;            // offset 0x1C0, size 0x4
};

extern CarCustomizeManager *TheCustomizeManager;

#endif
