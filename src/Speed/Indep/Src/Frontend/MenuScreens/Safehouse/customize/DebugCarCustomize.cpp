#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/DebugCarCustomize.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"

// TODO: 0x802DC758: d:/mw/speed/indep/src/world/CarPartNames.cpp (line 449)
const char *GetCarPartNameFromID(int32 car_part_id);
// TODO: 0x802DC898: d:/mw/speed/indep/src/world/CarPartNames.cpp (line 524)
const char *GetCarSlotNameFromID(int32 car_slot_id);










static uint32 gCarTypeNameHash = 0;
//
//
//

int gLookupCarSlotID = 0;





static int gInstallCarPartID = -1;
//
//
//
//
//
int SortCarsByName(DebugCar *before, DebugCar *after) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    const char *before_name = stable->GetCarRecordByHandle(before->mHandle)->GetDebugName();
    const char *after_name = stable->GetCarRecordByHandle(after->mHandle)->GetDebugName();
    return bStrCmp(before_name, after_name) <= 0;
}

DebugCarCustomizeScreen::DebugCarCustomizeScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), //
      iFastScroll(1) {
    FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(0xb));
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    for (int i = 0; i < 200; i++) {
        FECarRecord *car = stable->GetCarByIndex(i);
        if (car->IsValid()) {
            DebugCar *dc = new ("DebugCar", 0) DebugCar(car->Handle);
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
    for (DebugCarOption *node = list.GetHead(); node != list.EndOfList(); node = node->GetNext()) {
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
        CarTypeInfo *ti = GetCarTypeInfo(static_cast<CarType>(i));
        if (ti) {
            DebugCarOption *node = new ("DebugCarOption", 0) DebugCarOption(reinterpret_cast<const char *>(ti), ti->CarTypeNameHash);
            CarTypeNameHashes.AddTail(node);
        }
    }
    CurrentCarTypeNameHash = CarTypeNameHashes.GetHead();

    for (int i = 0; i < 0x8b; i++) {
        DebugCarOption *node = new ("DebugCarOption", 0) DebugCarOption(GetCarSlotNameFromID(i), i);
        LookupCarSlotIDs.AddTail(node);
    }
    CurrentLookupSlotID = FindElement(LookupCarSlotIDs, gLookupCarSlotID);

    DebugCarOption *partOpt = new ("DebugCarOption", 0) DebugCarOption("CARPARTNAME_ANY", 0);
    CarPartNameHashes.AddTail(partOpt);
    CurrentPartNameHash = CarPartNameHashes.GetHead();
}

void DebugCarCustomizeScreen::LoadCurrentCar() {
    if (pDebugCar) {
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
        bool customized = car->Customization != 0xFF;
        if (customized) {
            wasCarCustomized = true;
        } else {
            wasCarCustomized = false;
            car->Customization = custom->Handle;
            RideInfo ride;
            ride.Init(car->GetType(), static_cast<CarRenderUsage>(0), 0, 0);
            ride.SetRandomPaint();
            ride.SetStockParts();
            custom->WriteRideIntoRecord(&ride);
        }
        gCarCustomizeManager.RelinquishControl();
        gCarCustomizeManager.TakeControl(CEP_MAIN_MENU, car);
    }
}

void DebugCarCustomizeScreen::RebuildPartsList() {
    FEPlayerCarDB *stable;
    FECarRecord *car;
    CAR_SLOT_ID part_slot;
    int part_id_for_slot;
    unsigned int partname_hash;
    CarType car_type;
    CarPart *part;

    stable = FEDatabase->GetPlayerCarStable(0);
    car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->IsCustomized()) {
        while (!InstallableParts.IsEmpty()) {
            InstallableParts.RemoveHead();
        }
        part_slot = static_cast<CAR_SLOT_ID>(CurrentLookupSlotID->GetValue());
        part_id_for_slot = GetCarPartFromSlot(part_slot);
        partname_hash = bStringHash("CARPARTNAME_ANY");
        car_type = car->GetType();
        for (part = CarPartDB.NewGetFirstCarPart(car_type, part_slot, 0, -1); part;
             part = CarPartDB.NewGetNextCarPart(part, car_type, part_slot, 0, -1)) {
            InstallableParts.AddTail(part);
        }
        CurrentInstallablePart = InstallableParts.GetHead();
        for (bPNode *node = InstallableParts.GetHead(); node != InstallableParts.EndOfList(); node = node->GetNext()) {
            if (node->GetpObject() == gCarCustomizeManager.GetInstalledCarPart(part_slot)) {
                CurrentInstallablePart = node;
                break;
            }
        }
        gCarCustomizeManager.ResetPreview();
        NewPreviewPart();
    }
}

void DebugCarCustomizeScreen::NewPreviewPart() {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->IsCustomized() && !InstallableParts.IsEmpty()) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetpObject());
        GetCarTypeInfoFromHash(part->GetCarTypeNameHash());
        gCarCustomizeManager.PreviewPart(CurrentLookupSlotID->GetValue(), part);
    }
}

void DebugCarCustomizeScreen::InstallPreviewingPart() {
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->IsCustomized() && !InstallableParts.IsEmpty()) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetpObject());
        GetCarTypeInfoFromHash(part->GetCarTypeNameHash());
        gCarCustomizeManager.InstallPart(CurrentLookupSlotID->GetValue(), part);
    }
}

void DebugCarCustomizeScreen::DumpPresetRide() {
    FECarRecord *car = const_cast<FECarRecord *>(gCarCustomizeManager.GetTuningCar());
    FECustomizationRecord *rec = gCarCustomizeManager.GetPreviewRecord();
    RideInfo ride(car->GetType(), 0, 0, 0);
    rec->WriteRecordIntoRide(&ride);
    ride.DumpForPreset(const_cast<FECarRecord *>(car));
}

void DebugCarCustomizeScreen::Redraw() {
    FEPrintf(GetPackageName(), 0x36db742, "CarName");
    FEPrintf(GetPackageName(), 0x36db743, "LookupSlotID");
    FECarRecord *car = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(pDebugCar->mHandle);
    FEPrintf(GetPackageName(), 0x3e40712, car ? car->GetDebugName() : "NULL");
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
        FEPrintf(GetPackageName(), __CARNAME__, "%s", reinterpret_cast<const char *>(typeInfo));
        const char *pkg1 = GetPackageName();
        const char *partName = GetCarPartNameFromID(part->GetPartID());
        FEPrintf(pkg1, __PARTIDNAME__, "%s", partName);
        const char *pkg2 = GetPackageName();
        const char *name = part->GetName();
        FEPrintf(pkg2, __PARTNAME__, "%s", name);
        FEPrintf(GetPackageName(), 0x6a81554, "0x%x", part->GetPartNameHash());
        const char *fmt = "Part Info (%d/%d)";
        const char *pkg3 = GetPackageName();
        int idx = InstallableParts.GetNodeNumber(CurrentInstallablePart);
        int total = InstallableParts.CountElements();
        FEPrintf(pkg3, 0x36db746, fmt, idx, total);
        goto end;
    }
dash_section:
    FEPrintf(GetPackageName(), __CARNAME__, "----");
    FEPrintf(GetPackageName(), __PARTIDNAME__, "----");
    FEPrintf(GetPackageName(), __PARTNAME__, "----");
    FEPrintf(GetPackageName(), 0x6a81554, "----");
    FEPrintf(GetPackageName(), 0x36db746, "Part Info (NONE)");
end:;
}

void DebugCarCustomizeScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case 0xc519bfc2:
            iFastScroll = 10;
            break;
        case 0xe086d2e6:
            iFastScroll = 1;
            break;
        case 0xc519bfbf:
            if (!InstallableParts.IsEmpty()) {
                gCarCustomizeManager.ResetToStockCarParts();
                NewPreviewPart();
            }
            break;
        case 0xc519bfc0:
            DumpPresetRide();
            break;
        case 0x9120409e:
            switch (pobj->NameHash) {
                case 0x36db742: {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
                    if (!wasCarCustomized) {
                        car->Customization = 0xFF;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        pDebugCar = FilteredCarsList.GetPrevCircular(pDebugCar);
                    }
                    CurrentCarTypeNameHash = CarTypeNameHashes.GetPrevCircular(CurrentCarTypeNameHash);
                    LoadCurrentCar();
                    RebuildPartsList();
                    break;
                }
                case 0x36db743:
                    for (int i = 0; i < iFastScroll; i++) {
                        CurrentLookupSlotID = LookupCarSlotIDs.GetPrevCircular(CurrentLookupSlotID);
                    }
                    RebuildPartsList();
                    break;
                case 0x36db746:
                    if (InstallableParts.IsEmpty()) {
                        break;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        CurrentInstallablePart = InstallableParts.GetPrevCircular(CurrentInstallablePart);
                    }
                    NewPreviewPart();
                    break;
                default:
                    break;
            }
            Redraw();
            break;
        case 0xb5971bf1:
            switch (pobj->NameHash) {
                case 0x36db742: {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
                    if (!wasCarCustomized) {
                        car->Customization = 0xFF;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        pDebugCar = FilteredCarsList.GetNextCircular(pDebugCar);
                    }
                    CurrentCarTypeNameHash = CarTypeNameHashes.GetNextCircular(CurrentCarTypeNameHash);
                    LoadCurrentCar();
                    RebuildPartsList();
                    break;
                }
                case 0x36db743:
                    for (int i = 0; i < iFastScroll; i++) {
                        CurrentLookupSlotID = LookupCarSlotIDs.GetNextCircular(CurrentLookupSlotID);
                    }
                    RebuildPartsList();
                    break;
                case 0x36db746:
                    if (InstallableParts.IsEmpty()) {
                        break;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        CurrentInstallablePart = InstallableParts.GetNextCircular(CurrentInstallablePart);
                    }
                    NewPreviewPart();
                    break;
                default:
                    break;
            }
            Redraw();
            break;
        case 0x406415e3:
            InstallPreviewingPart();
            break;
        case 0x911ab364:
            gCarCustomizeManager.RelinquishControl();
            cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            break;
        default:
            break;
    }
}
