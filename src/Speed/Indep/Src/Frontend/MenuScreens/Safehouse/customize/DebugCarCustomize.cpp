#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/DebugCarCustomize.hpp"

#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Generated/CarTypes.hpp"
#include "Speed/Indep/Src/Generated/FEngHash/FEHash_UI_DebugCarCustomize.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

// TODO: 0x802DC758: d:/mw/speed/indep/src/world/CarPartNames.cpp (line 449)
const char *GetCarPartNameFromID(int32 car_part_id);
// TODO: 0x802DC898: d:/mw/speed/indep/src/world/CarPartNames.cpp (line 524)
const char *GetCarSlotNameFromID(int32 car_slot_id);

extern int gLookupCarSlotID;

int SortCarsByName(DebugCar *before, DebugCar *after) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    const char *before_name = stable->GetCarRecordByHandle(before->mHandle)->GetDebugName();
    const char *after_name = stable->GetCarRecordByHandle(after->mHandle)->GetDebugName();
    return static_cast<int>(bStrCmp(before_name, after_name) <= 0);
}

DebugCarCustomizeScreen::DebugCarCustomizeScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), //
      iFastScroll(1) {
    int player_num = 0;
    uint32 player_car = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(player_num);
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(player_num);
    for (int i = 0; i < 200; i++) {
        FECarRecord *fe_car = stable->GetCarByIndex(i);
        if (fe_car->IsValid()) {
            FilteredCarsList.AddTail(new ("DebugCar", 0) DebugCar(fe_car->Handle));
        }
    }
    FilteredCarsList.Sort(SortCarsByName);
    custom = stable->CreateNewCustomizationRecord();
    pDebugCar = FilteredCarsList.GetHead();
    LoadCurrentCar();
    BuildOptionsLists();
    RebuildPartsList();
    Redraw();
}

DebugCarCustomizeScreen::~DebugCarCustomizeScreen() {
    custom->Handle = INVALID_CUSTOMIZATION_HANDLE;
}

DebugCarCustomizeScreen::DebugCarOption *DebugCarCustomizeScreen::FindElement(bTList<DebugCarOption> &list, int id) {
    for (DebugCarOption *d = list.GetHead(); d != list.EndOfList(); d = d->GetNext()) {
        if (d->GetValue() == id) {
            return d;
        }
    }
    return nullptr;
}

void DebugCarCustomizeScreen::BuildOptionsLists() {
    CarTypeNameHashes.AddTail(new DebugCarOption("CARTYPENAME_ANY", 0));

    for (int i = 0; i < NUM_CARTYPES; i++) {
        CarTypeInfo *ti = GetCarTypeInfo(static_cast<CarType>(i));
        if (ti != nullptr) {
            CarTypeNameHashes.AddTail(new ("DebugCarOption", 0) DebugCarOption(ti->CarTypeName, ti->CarTypeNameHash));
        }
    }

    CurrentCarTypeNameHash = CarTypeNameHashes.GetHead();

    for (int i = 0; i < CARSLOTID_NUM; i++) {
        LookupCarSlotIDs.AddTail(new ("DebugCarOption", 0) DebugCarOption(GetCarSlotNameFromID(i), i));
    }

    CurrentLookupSlotID = FindElement(LookupCarSlotIDs, gLookupCarSlotID);

    CarPartNameHashes.AddTail(new ("DebugCarOption", 0) DebugCarOption("CARPARTNAME_ANY", 0));
    CurrentPartNameHash = CarPartNameHashes.GetHead();
}

void DebugCarCustomizeScreen::LoadCurrentCar() {
    if (pDebugCar != nullptr) {
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
        if (car->IsCustomized()) {
            wasCarCustomized = true;
        } else {
            wasCarCustomized = false;
            car->Customization = custom->Handle;
            RideInfo ride;
            ride.Init(car->GetType(), CarRenderUsage_Player, 0, 0);
            ride.SetRandomPaint();
            ride.SetStockParts();
            custom->WriteRideIntoRecord(&ride);
        }
        gCarCustomizeManager.RelinquishControl();
        gCarCustomizeManager.TakeControl(CEP_MAIN_MENU, car);
    }
}

void DebugCarCustomizeScreen::RebuildPartsList() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
    if (!car->IsCustomized()) {
        return;
    }
    while (!InstallableParts.IsEmpty()) {
        InstallableParts.RemoveHead();
    }
    CAR_SLOT_ID part_slot = static_cast<CAR_SLOT_ID>(CurrentLookupSlotID->GetValue());
    int part_id_for_slot = GetCarPartFromSlot(part_slot);
    uint32 partname_hash = bStringHash("CARPARTNAME_ANY");
    CarType car_type = car->GetType();
    CarPart *part = CarPartDB.NewGetFirstCarPart(car_type, part_slot, 0, -1);
    while (part != nullptr) {
        InstallableParts.AddTail(part);
        part = CarPartDB.NewGetNextCarPart(part, car_type, part_slot, 0, -1);
    }
    CurrentInstallablePart = InstallableParts.GetHead();
    for (bPNode *node = InstallableParts.GetHead(); node != InstallableParts.EndOfList(); node = node->GetNext()) {
        if (node->GetObj() == gCarCustomizeManager.GetInstalledCarPart(part_slot)) {
            CurrentInstallablePart = node;
            break;
        }
    }
    gCarCustomizeManager.ResetPreview();
    NewPreviewPart();
}

void DebugCarCustomizeScreen::NewPreviewPart() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->IsCustomized() && !InstallableParts.IsEmpty()) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetObject());
        CarTypeInfo *cti = GetCarTypeInfo(part->GetCarTypeNameHash());
        int part_slot = CurrentLookupSlotID->GetValue();
        gCarCustomizeManager.PreviewPart(part_slot, part);
    }
}

void DebugCarCustomizeScreen::InstallPreviewingPart() {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
    if (car->IsCustomized() && !InstallableParts.IsEmpty()) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetObject());
        CarTypeInfo *cti = GetCarTypeInfo(part->GetCarTypeNameHash());
        int part_slot = CurrentLookupSlotID->GetValue();
        gCarCustomizeManager.PreviewPart(part_slot, part);
    }
}

void DebugCarCustomizeScreen::DumpPresetRide() {
    const FECustomizationRecord *cust = gCarCustomizeManager.GetPreviewRecord();
    FECarRecord *rec = const_cast<FECarRecord *>(gCarCustomizeManager.GetTuningCar());
    RideInfo bla(rec->GetType(), CarRenderUsage_Player, 0, 0);
    cust->WriteRecordIntoRide(&bla);
    bla.DumpForPreset(rec);
}

void DebugCarCustomizeScreen::Redraw() {
    FEPrintf(GetPackageName(), 0x36db742, "CarName");
    FEPrintf(GetPackageName(), 0x36db743, "LookupSlotID");

    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
    FEPrintf(GetPackageName(), 0x3e40712, car != nullptr ? car->GetDebugName() : "NULL");
    FEPrintf(GetPackageName(), 0x3e40713, CurrentLookupSlotID->GetString());
    if (CurrentInstallablePart != InstallableParts.EndOfList() && (car != nullptr && car->IsCustomized())) {
        CarPart *part = static_cast<CarPart *>(CurrentInstallablePart->GetObject());
        CarTypeInfo *cti = GetCarTypeInfo(part->GetCarTypeNameHash());
        FEPrintf(GetPackageName(), 0xd6d32016, "%s", cti->CarTypeName);
        FEPrintf(GetPackageName(), 0xeffe7224, "%s", GetCarPartNameFromID(part->PartID));
        FEPrintf(GetPackageName(), 0xb1027477, "%s", part->GetName());
        FEPrintf(GetPackageName(), 0x6a81554, "0x%x", part->GetPartNameHash());
        FEPrintf(GetPackageName(), 0x36db746, "Part Info (%d/%d)", InstallableParts.GetNodeNumber(CurrentInstallablePart),
                 InstallableParts.CountElements());
    } else {
        FEPrintf(GetPackageName(), 0xd6d32016, "----");
        FEPrintf(GetPackageName(), 0xeffe7224, "----");
        FEPrintf(GetPackageName(), 0xb1027477, "----");
        FEPrintf(GetPackageName(), 0x6a81554, "----");
        FEPrintf(GetPackageName(), 0x36db746, "Part Info (NONE)");
    }
}

// UNSOLVED
void DebugCarCustomizeScreen::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case __PAD_BUTTON3__:
            iFastScroll = 10;
            break;
        case __PAD_BUTTON3_RELEASED__:
            iFastScroll = 1;
            break;
        case __PAD_BUTTON0__:
            if (!InstallableParts.IsEmpty()) {
                gCarCustomizeManager.ResetToStockCarParts();
                NewPreviewPart();
            }
            break;
        case __PAD_BUTTON1__:
            DumpPresetRide();
            break;
        case __PAD_LEFT__:
            switch (pobj->NameHash) {
                case __OPT_1__: {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
                    if (!wasCarCustomized) {
                        car->Customization = INVALID_CUSTOMIZATION_HANDLE;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        pDebugCar = FilteredCarsList.GetPrevCircular(pDebugCar);
                    }
                    CurrentCarTypeNameHash = CarTypeNameHashes.GetPrevCircular(CurrentCarTypeNameHash);
                    LoadCurrentCar();
                    RebuildPartsList();
                    break;
                }
                case __OPT_2__:
                    for (int i = 0; i < iFastScroll; i++) {
                        CurrentLookupSlotID = LookupCarSlotIDs.GetPrevCircular(CurrentLookupSlotID);
                    }
                    RebuildPartsList();
                    break;
                case __OPT_5__:
                    if (!InstallableParts.IsEmpty()) {
                        for (int i = 0; i < iFastScroll; i++) {
                            CurrentInstallablePart = InstallableParts.GetPrevCircular(CurrentInstallablePart);
                        }
                        NewPreviewPart();
                    }
                    break;
            }
            Redraw();
            break;

        case __PAD_RIGHT__:
            switch (pobj->NameHash) {
                case __OPT_1__: {
                    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                    FECarRecord *car = stable->GetCarRecordByHandle(pDebugCar->mHandle);
                    if (!wasCarCustomized) {
                        car->Customization = INVALID_CUSTOMIZATION_HANDLE;
                    }
                    for (int i = 0; i < iFastScroll; i++) {
                        pDebugCar = FilteredCarsList.GetNextCircular(pDebugCar);
                    }
                    CurrentCarTypeNameHash = CarTypeNameHashes.GetNextCircular(CurrentCarTypeNameHash);
                    LoadCurrentCar();
                    RebuildPartsList();
                    break;
                }
                case __OPT_2__:
                    for (int i = 0; i < iFastScroll; i++) {
                        CurrentLookupSlotID = LookupCarSlotIDs.GetNextCircular(CurrentLookupSlotID);
                    }
                    RebuildPartsList();
                    break;
                case __OPT_5__:
                    if (!InstallableParts.IsEmpty()) {
                        for (int i = 0; i < iFastScroll; i++) {
                            CurrentInstallablePart = InstallableParts.GetNextCircular(CurrentInstallablePart);
                        }
                        NewPreviewPart();
                    }
                    break;
            }
            Redraw();
            break;

        case __PAD_ACCEPT__:
            InstallPreviewingPart();
            break;
        case __PAD_BACK__:
            gCarCustomizeManager.RelinquishControl();
            cFEng::Get()->QueuePackageSwitch("MainMenu.fng", 0, 0, false);
            break;
    }
}
