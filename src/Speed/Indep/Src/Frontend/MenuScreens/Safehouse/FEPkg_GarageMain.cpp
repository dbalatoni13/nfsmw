// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct FrontEndRenderingCar {
    char _pad[0x574];
    int Visible;
};

extern MenuScreen *FEngFindScreen(const char *name);

extern RideInfo TopOrFullScreenRide;
extern eSetRideInfoReasons TopOrFullScreenLoadingReason;

extern CarLoader TheCarLoader;

static const char lbl_GarageMain[] = "GarageMain.fng";

// --- Free functions ---

bool HaveAttributesChanged(Attrib::Gen::frontend &) {
    return false;
}

const char *GetCurrentGarageName() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return "customization_shop";
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return "career_safehouse";
        case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM:
            return "backroom";
        case GARAGETYPE_CAR_LOT:
            return "car_lot";
        default:
            break;
    }
    if (FEDatabase->IsCareerManagerMode()) {
        return "career_manager";
    }
    return "main_fe";
}

// --- GarageMainScreen ---

GarageMainScreen *GarageMainScreen::sInstance;

GarageMainScreen *GarageMainScreen::GetInstance() {
    return static_cast<GarageMainScreen *>(FEngFindScreen(lbl_GarageMain));
}

void GarageMainScreen::EnableCarRendering() {
    if (RenderingCar) {
        RenderingCar->Visible = 1;
    }
}

void GarageMainScreen::DisableCarRendering() {
    if (RenderingCar) {
        RenderingCar->Visible = 0;
    }
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

void GarageMainScreen::CancelCarLoad() {
    CarState = 1;
    TheGarageCarLoader->CancelCarLoad();
}

void GarageMainScreen::NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) {
    switch (Message) {
        case 0x18883F75:
            HideEntireScreen = 0;
            HandleShowPackage(0x18883F75);
            break;
        case 0x0AD4BBDC:
            HideEntireScreen = 1;
            HandleHidePackage(0x0AD4BBDC);
            break;
        case 0xC98356BA:
            HandleTick(0xC98356BA);
            break;
        case 0xD0678849:
            HandleJoyEvents();
            break;
    }
}

float GarageMainScreen::GetCarRotationX() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return -0.3796229958534241f;
        default:
            return 0.0f;
    }
}

float GarageMainScreen::GetCarRotationY() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return -0.00019299999985378236f;
        default:
            return 0.0f;
    }
}

float GarageMainScreen::GetCarRotationZ() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 304.96978759765625f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 304.96978759765625f;
        case GARAGETYPE_CAR_LOT:
            return 340.0f;
        default:
            return 304.96978759765625f;
    }
}

float GarageMainScreen::GetGeometryZAngle() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 302.85308837890625f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
        case GARAGETYPE_CAR_LOT:
            return 0.0f;
        default:
            return 134.41250610351562f;
    }
}

float GarageMainScreen::GetGeometryXPos() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return 0.0f;
        default:
            return 0.0f;
    }
}

float GarageMainScreen::GetGeometryYPos() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return 0.07500000298023224f;
        default:
            return 0.0f;
    }
}

float GarageMainScreen::GetGeometryZPos() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return 0.0f;
        default:
            return 0.0f;
    }
}

// --- CarViewer ---

GarageMainScreen *CarViewer::FindWhichScreenToUpdate(eCarViewerWhichCar which_car) {
    const char *name = lbl_GarageMain;
    if (cFEng::mInstance->IsPackagePushed(name)) {
        return static_cast<GarageMainScreen *>(FEngFindScreen(name));
    }
    return nullptr;
}

void CarViewer::SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car) {
    GarageMainScreen *screen = FindWhichScreenToUpdate(which_car);
    TopOrFullScreenRide = *ride;
    TopOrFullScreenLoadingReason = reason;
    if (screen) {
        screen->SetRideInfo(ride, reason);
    }
}

void CarViewer::CancelCarLoad(eCarViewerWhichCar which_car) {
    FindWhichScreenToUpdate(which_car)->CancelCarLoad();
}

RideInfo *CarViewer::GetRideInfo(eCarViewerWhichCar which_car) {
    return &TopOrFullScreenRide;
}

void CarViewer::HideAllCars() {
    cFEng::mInstance->QueueGameMessage(0x0AD4BBDC, lbl_GarageMain, 0xFF);
}

void CarViewer::ShowAllCars() {
    cFEng::mInstance->QueueGameMessage(0x18883F75, lbl_GarageMain, 0xFF);
}

void CarViewer::ShowCarScreen() {
    if (!cFEng::mInstance->IsPackagePushed(lbl_GarageMain)) {
        cFEng::mInstance->PushNoControlPackage(lbl_GarageMain, static_cast<FE_PACKAGE_PRIORITY>(100));
    }
}

bool CarViewer::haveLoadedOnce;

// --- Camera Info functions ---

// FindGarageCameraInfo is defined later in this TU
unsigned int FindGarageCameraInfo(const char *prefix);

static unsigned int FindGarageEntryCameraInfo() {
    return FindGarageCameraInfo("angle_entry_");
}

static unsigned int FindGarageFinalCameraInfo() {
    return FindGarageCameraInfo("angle_final_");
}

// --- GarageCarLoader ---

GarageCarLoader *GetGarageCarLoader() {
    static GarageCarLoader TheGarageCarLoader;
    return &TheGarageCarLoader;
}

void GarageCarLoader::Init() {
    IsCurrentRide = false;
    LoadingCar = 0;
    CurrentCar = 0;
    IsLoadingRide = false;
}

void GarageCarLoader::Switch() {
    IsDifferent = false;
}

RideInfo *GarageCarLoader::GetCurrentRideInfo() {
    if (IsCurrentRide) {
        return reinterpret_cast<RideInfo *>(&_pad_ride1[0]);
    }
    return nullptr;
}

void GarageCarLoader::CancelCarLoad() {
    if (IsLoadingRide) {
        TheCarLoader.Unload(LoadingCar);
    }
}

void GarageCarLoader::CleanUp() {
    if (IsLoadingRide && LoadingCar) {
        TheCarLoader.Unload(LoadingCar);
    }
    if (IsCurrentRide && CurrentCar) {
        TheCarLoader.Unload(CurrentCar);
    }
    IsCurrentRide = false;
    LoadingCar = 0;
    CurrentCar = 0;
    IsLoadingRide = false;
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

MenuScreen *CreateGarageMainScreen(ScreenConstructorData *sd) {
    return new GarageMainScreen(sd, 1, &TopOrFullScreenRide, 0);
}
