#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"

#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"

RideInfo TopOrFullScreenRide;
eSetRideInfoReasons TopOrFullScreenLoadingReason;

bool CarViewer::haveLoadedOnce = false;

GarageMainScreen *CarViewer::FindWhichScreenToUpdate(eCarViewerWhichCar which_car) {
    if (cFEng::Get()->IsPackagePushed("GarageMain.fng")) {
        return (GarageMainScreen *)FEngFindScreen("GarageMain.fng");
    }
    return nullptr;
}

void CarViewer::SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car) {
    GarageMainScreen *screen = FindWhichScreenToUpdate(which_car);
    RideInfo *update_this_ride = &TopOrFullScreenRide;

    *update_this_ride = *ride;
    if (update_this_ride == &TopOrFullScreenRide) {
        TopOrFullScreenLoadingReason = reason;
    }
    if (screen) {
        screen->SetRideInfo(update_this_ride, reason);
    }
}

void CarViewer::CancelCarLoad(eCarViewerWhichCar which_car) {
    GarageMainScreen *screen = FindWhichScreenToUpdate(which_car);
    screen->CancelCarLoad();
}

RideInfo *CarViewer::GetRideInfo(eCarViewerWhichCar which_car) {
    return &TopOrFullScreenRide;
}

void CarViewer::HideAllCars() {
    cFEng::Get()->QueueGameMessage(0x0AD4BBDC, "GarageMain.fng", 0xFF);
}

void CarViewer::ShowAllCars() {
    cFEng::Get()->QueueGameMessage(0x18883F75, "GarageMain.fng", 0xFF);
}

void CarViewer::ShowCarScreen() {
    if (!cFEng::Get()->IsPackagePushed("GarageMain.fng")) {
        cFEng::Get()->PushNoControlPackage("GarageMain.fng", FE_PACKAGE_PRIORITY_FIFTH_CLOSEST);
    }
}
