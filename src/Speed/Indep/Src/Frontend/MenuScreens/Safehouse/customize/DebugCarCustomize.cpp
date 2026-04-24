#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/DebugCarCustomize.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

// CarPart is already defined in CustomizeManager.cpp (earlier in jumbo build)

extern cFrontendDatabase *FEDatabase;
extern CarCustomizeManager gCarCustomizeManager;
extern int gLookupCarSlotID;
extern const char *GetCarSlotNameFromID(int id);
extern CarPart *GetCarPartFromSlot(int slot_id);
extern const char *GetCarPartNameFromID(int id);
extern unsigned int bStringHash(const char *text);

int SortCarsByName(DebugCar *before, DebugCar *after) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    const char *before_name = stable->GetCarRecordByHandle(before->mHandle)->GetDebugName();
    const char *after_name = stable->GetCarRecordByHandle(after->mHandle)->GetDebugName();
    return bStrCmp(before_name, after_name) <= 0;
}

DebugCarCustomizeScreen::DebugCarOption::DebugCarOption(const char *name, int value) : Intval(value) {
    bStrNCpy(String, name, 0x40);
}

DebugCarCustomizeScreen::DebugCarCustomizeScreen(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      iFastScroll(1) {
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    for (int i = 0; i < 200; i++) {
        FECarRecord *car = stable->GetCarByIndex(i);
        if (car->Handle != 0xFFFFFFFF) {
            DebugCar *dc = new DebugCar(car->Handle);
            FilteredCarsList.AddTail(dc);
        }
    }
    FilteredCarsList.Sort(SortCarsByName);
    custom = stable->CreateNewCustomizationRecord();
    pDebugCar = static_cast<DebugCar *>(FilteredCarsList.GetHead());
    LoadCurrentCar();
    BuildOptionsLists();
    RebuildPartsList();
    Redraw();
}

DebugCarCustomizeScreen::~DebugCarCustomizeScreen() {
    custom->Handle = 0xFF;
}

DebugCarCustomizeScreen::DebugCarOption *DebugCarCustomizeScreen::FindElement(bTList<DebugCarOption> &list, int id) {
    for (DebugCarOption *node = list.GetHead(); node != reinterpret_cast<DebugCarOption *>(&list); node = node->GetNext()) {
        if (node->GetValue() == id) {
            return node;
        }
    }
    return nullptr;
}

void DebugCarCustomizeScreen::BuildOptionsLists() {
    DebugCarOption *opt = new DebugCarOption("CARTYPENAME_ANY", 0);
    CarTypeNameHashes.AddTail(opt);
    for (int i = 0; i < 0x54; i++) {
        CarTypeInfo *info = &CarTypeInfoArray[i];
        if (info) {
            DebugCarOption *node = new DebugCarOption(reinterpret_cast<const char *>(info), info->CarTypeNameHash);
            CarTypeNameHashes.AddTail(node);
        }
    }
    CurrentCarTypeNameHash = CarTypeNameHashes.GetHead();

    for (int i = 0; i < 0x8b; i++) {
        DebugCarOption *node = new DebugCarOption(GetCarSlotNameFromID(i), i);
        LookupCarSlotIDs.AddTail(node);
    }
    CurrentLookupSlotID = FindElement(LookupCarSlotIDs, gLookupCarSlotID);

    DebugCarOption *partOpt = new DebugCarOption("CARPARTNAME_ANY", 0);
    CurrentPartNameHash = CarPartNameHashes.GetHead();
    CarPartNameHashes.AddTail(partOpt);
}

void DebugCarCustomizeScreen::LoadCurrentCar() {
    if (pDebugCar) {
        FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
        if (car->Customization == 0xFF) {
            wasCarCustomized = false;
            car->Customization = custom->Handle;
            RideInfo ride;
            ride.Init(static_cast<CarType>(-1), static_cast<CarRenderUsage>(0), 0, 0);
            ride.Init(car->GetType(), static_cast<CarRenderUsage>(0), 0, 0);
            ride.SetRandomPaint();
            ride.SetStockParts();
            custom->WriteRideIntoRecord(&ride);
        } else {
            wasCarCustomized = true;
        }
        gCarCustomizeManager.RelinquishControl();
        gCarCustomizeManager.TakeControl(static_cast<eCustomizeEntryPoint>(1), car);
    }
}

void DebugCarCustomizeScreen::RebuildPartsList() {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->Customization != 0xFF) {
        InstallableParts.DeleteAllElements();
        int slotId = CurrentLookupSlotID->GetValue();
        GetCarPartFromSlot(slotId);
        bStringHash("CARPARTNAME_ANY");
        CarType type = car->GetType();
        for (CarPart *part = CarPartDB.NewGetFirstCarPart(type, slotId, 0, -1); part; part = CarPartDB.NewGetNextCarPart(part, type, slotId, 0, -1)) {
            InstallableParts.AddTail(part);
        }
        CurrentInstallablePart = InstallableParts.GetHead();
        bPNode *node = InstallableParts.GetHead();
        while (node != reinterpret_cast<bPNode *>(&InstallableParts)) {
            CarPart *cp = static_cast<CarPart *>(node->GetpObject());
            if (cp == gCarCustomizeManager.GetInstalledCarPart(slotId)) {
                CurrentInstallablePart = node;
                break;
            }
            node = node->GetNext();
        }
        gCarCustomizeManager.ResetPreview();
        NewPreviewPart();
    }
}

void DebugCarCustomizeScreen::NewPreviewPart() {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->Customization != 0xFF && !InstallableParts.IsEmpty()) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetpObject());
        part->GetCarTypeNameHash();
        GetCarTypeInfoFromHash(0);
        gCarCustomizeManager.PreviewPart(CurrentLookupSlotID->GetValue(), part);
    }
}

void DebugCarCustomizeScreen::InstallPreviewingPart() {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->Customization != 0xFF && !InstallableParts.IsEmpty()) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetpObject());
        part->GetCarTypeNameHash();
        GetCarTypeInfoFromHash(0);
        gCarCustomizeManager.InstallPart(CurrentLookupSlotID->GetValue(), part);
    }
}

void DebugCarCustomizeScreen::DumpPresetRide() {
    FECarRecord *car = const_cast<FECarRecord *>(gCarCustomizeManager.GetTuningCar());
    RideInfo ride;
    ride.Init(car->GetType(), static_cast<CarRenderUsage>(0), 0, 0);
    gCarCustomizeManager.GetPreviewRecord()->WriteRecordIntoRide(&ride);
    ride.DumpForPreset(const_cast<FECarRecord *>(car));
}

void DebugCarCustomizeScreen::Redraw() {
    FEPrintf(GetPackageName(), 0x36db742, "CarName");
    FEPrintf(GetPackageName(), 0x36db743, "LookupSlotID");
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car) {
        FEPrintf(GetPackageName(), 0x3e40712, car->GetDebugName());
    } else {
        FEPrintf(GetPackageName(), 0x3e40712, "NULL");
    }
    FEPrintf(GetPackageName(), 0x3e40713, CurrentLookupSlotID->GetString());
    if (CurrentInstallablePart == reinterpret_cast<bPNode *>(&InstallableParts) || !car) {
        goto dash_section;
    }
    {
        bool hasCustom = car->Customization != 0xFF;
        if (!hasCustom)
            goto dash_section;
    }
    {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetpObject());
        unsigned int typeHash = part->GetCarTypeNameHash();
        CarTypeInfo *typeInfo = GetCarTypeInfoFromHash(typeHash);
        FEPrintf(GetPackageName(), 0xd6d32016, "%s", reinterpret_cast<const char *>(typeInfo));
        const char *partName = GetCarPartNameFromID(part->GetPartID());
        FEPrintf(GetPackageName(), 0xeffe7224, "%s", partName);
        const char *name = part->GetName();
        FEPrintf(GetPackageName(), 0xb1027477, "%s", name);
        FEPrintf(GetPackageName(), 0x6a81554, "0x%x", part->GetPartNameHash());
        // TODO
        int idx; // = InstallableParts.TraversebList(CurrentInstallablePart);
        int total = InstallableParts.CountElements();
        FEPrintf(GetPackageName(), 0x36db746, "Part Info (%d/%d)", idx, total);
        goto end;
    }
dash_section:
    FEPrintf(GetPackageName(), 0xd6d32016, "----");
    FEPrintf(GetPackageName(), 0xeffe7224, "----");
    FEPrintf(GetPackageName(), 0xb1027477, "----");
    FEPrintf(GetPackageName(), 0x6a81554, "----");
    FEPrintf(GetPackageName(), 0x36db746, "Part Info (NONE)");
end:;
}

void DebugCarCustomizeScreen::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
        case 0xc519bfc2:
            iFastScroll = 10;
            return;
        case 0xe086d2e6:
            iFastScroll = 1;
            return;
        case 0xc519bfbf:
            if (InstallableParts.IsEmpty())
                return;
            gCarCustomizeManager.ResetToStockCarParts();
            NewPreviewPart();
            return;
        case 0xc519bfc0:
            DumpPresetRide();
            return;
        case 0x9120409e: {
            unsigned int hash = pobj->NameHash;
            switch (hash) {
                case 0x36db742: {
                    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
                    if (!wasCarCustomized) {
                        car->Customization = 0xFF;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        DebugCar *prev = pDebugCar->GetPrev();
                        if (prev == reinterpret_cast<DebugCar *>(&FilteredCarsList)) {
                            prev = FilteredCarsList.GetTail();
                        }
                        pDebugCar = prev;
                    }
                    CurrentCarTypeNameHash = CurrentCarTypeNameHash->GetPrev();
                    if (CurrentCarTypeNameHash == reinterpret_cast<DebugCarOption *>(&CarTypeNameHashes)) {
                        CurrentCarTypeNameHash = CarTypeNameHashes.GetTail();
                    }
                    LoadCurrentCar();
                    break;
                }
                case 0x36db743:
                    for (int i = 0; i < iFastScroll; i++) {
                        DebugCarOption *prev = CurrentLookupSlotID->GetPrev();
                        if (prev == reinterpret_cast<DebugCarOption *>(&LookupCarSlotIDs)) {
                            prev = LookupCarSlotIDs.GetTail();
                        }
                        CurrentLookupSlotID = prev;
                    }
                    break;
                case 0x36db746:
                    if (InstallableParts.IsEmpty())
                        goto done;
                    for (int i = 0; i < iFastScroll; i++) {
                        bPNode *prev = CurrentInstallablePart->GetPrev();
                        if (prev == reinterpret_cast<bPNode *>(&InstallableParts)) {
                            prev = InstallableParts.GetTail();
                        }
                        CurrentInstallablePart = prev;
                    }
                    NewPreviewPart();
                    goto done;
                default:
                    goto done;
            }
            RebuildPartsList();
            break;
        }
        case 0xb5971bf1: {
            unsigned int hash = pobj->NameHash;
            switch (hash) {
                case 0x36db742: {
                    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
                    if (!wasCarCustomized) {
                        car->Customization = 0xFF;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        DebugCar *next = pDebugCar->GetNext();
                        if (next == reinterpret_cast<DebugCar *>(&FilteredCarsList)) {
                            next = FilteredCarsList.GetHead();
                        }
                        pDebugCar = next;
                    }
                    CurrentCarTypeNameHash = CurrentCarTypeNameHash->GetNext();
                    if (CurrentCarTypeNameHash == reinterpret_cast<DebugCarOption *>(&CarTypeNameHashes)) {
                        CurrentCarTypeNameHash = CarTypeNameHashes.GetHead();
                    }
                    LoadCurrentCar();
                    break;
                }
                case 0x36db743:
                    for (int i = 0; i < iFastScroll; i++) {
                        DebugCarOption *next = CurrentLookupSlotID->GetNext();
                        if (next == reinterpret_cast<DebugCarOption *>(&LookupCarSlotIDs)) {
                            next = LookupCarSlotIDs.GetHead();
                        }
                        CurrentLookupSlotID = next;
                    }
                    break;
                case 0x36db746:
                    if (InstallableParts.IsEmpty())
                        goto done;
                    for (int i = 0; i < iFastScroll; i++) {
                        bPNode *next = CurrentInstallablePart->GetNext();
                        if (next == reinterpret_cast<bPNode *>(&InstallableParts)) {
                            next = InstallableParts.GetHead();
                        }
                        CurrentInstallablePart = next;
                    }
                    NewPreviewPart();
                    goto done;
                default:
                    goto done;
            }
            RebuildPartsList();
            break;
        }
        case 0x406415e3:
            InstallPreviewingPart();
            return;
        case 0x911ab364:
            gCarCustomizeManager.RelinquishControl();
            cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            return;
        default:
            return;
    }
done:
    Redraw();
}
