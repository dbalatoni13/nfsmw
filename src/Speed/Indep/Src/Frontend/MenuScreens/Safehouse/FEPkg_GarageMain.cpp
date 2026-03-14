#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"

extern MenuScreen *FEngFindScreen(const char *name);
extern RideInfo TopOrFullScreenRide;
extern eSetRideInfoReasons TopOrFullScreenLoadingReason;

static const char lbl_GarageMain[] = "GarageMain.fng";

// --- HaveAttributesChanged ---
bool HaveAttributesChanged(Attrib::Gen::frontend &) {
    return false;
}

// --- GarageMainScreen statics ---
GarageMainScreen *GarageMainScreen::sInstance;

GarageMainScreen *GarageMainScreen::GetInstance() {
    return static_cast<GarageMainScreen *>(FEngFindScreen(lbl_GarageMain));
}

void GarageMainScreen::EnableCarRendering() {
    if (!RenderingCar) {
        return;
    }
    RenderingCar->Visible = 1;
}

void GarageMainScreen::DisableCarRendering() {
    if (!RenderingCar) {
        return;
    }
    RenderingCar->Visible = 0;
}

bool GarageMainScreen::IsCarRendering() {
    if (RenderingCar && RenderingCar->Visible) {
        return true;
    }
    return false;
}

void GarageMainScreen::HandleHidePackage(unsigned int msg) {
    RenderingCar->Visible = 0;
}

// --- GarageCarLoader ---

RideInfo *GarageCarLoader::GetCurrentRideInfo() {
    if (!IsCurrentRide) {
        return nullptr;
    }
    return &CurrentRideInfo;
}

void GarageCarLoader::Switch() {
    IsDifferent = false;
}

// --- GarageCarLoader static access ---
static GarageCarLoader TheGarageCarLoader;

GarageCarLoader *GetGarageCarLoader() {
    return &TheGarageCarLoader;
}

void InitGarageCarLoaders() {
    GetGarageCarLoader()->Init();
}

void CleanUpGarageCarLoaders() {
    GetGarageCarLoader()->CleanUp();
}

void UpdateGarageCarLoaders() {
    GetGarageCarLoader()->Update();
}

// --- CarViewer ---

GarageMainScreen *CarViewer::FindWhichScreenToUpdate(eCarViewerWhichCar which_car) {
    if (cFEng::Get()->IsPackagePushed(lbl_GarageMain)) {
        return static_cast<GarageMainScreen *>(FEngFindScreen(lbl_GarageMain));
    }
    return nullptr;
}

void CarViewer::SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car) {
    GarageMainScreen *screen = FindWhichScreenToUpdate(which_car);
    TopOrFullScreenRide = *ride;
    TopOrFullScreenLoadingReason = reason;
    if (screen) {
        screen->SetRideInfo(&TopOrFullScreenRide, reason);
    }
}

void CarViewer::CancelCarLoad(eCarViewerWhichCar which_car) {
    GarageMainScreen *screen = FindWhichScreenToUpdate(which_car);
    if (screen) {
        screen->CancelCarLoad();
    }
}

RideInfo *CarViewer::GetRideInfo(eCarViewerWhichCar which_car) {
    return &TopOrFullScreenRide;
}

void CarViewer::HideAllCars() {
    cFEng::Get()->QueueGameMessage(0x0AD4BBDC, lbl_GarageMain, 0xFF);
}

void CarViewer::ShowAllCars() {
    cFEng::Get()->QueueGameMessage(0x18883F75, lbl_GarageMain, 0xFF);
}

void CarViewer::ShowCarScreen() {
    if (!cFEng::Get()->IsPackagePushed(lbl_GarageMain)) {
        cFEng::Get()->PushNoControlPackage(lbl_GarageMain, FE_PACKAGE_PRIORITY_FIFTH_CLOSEST);
    }
}
