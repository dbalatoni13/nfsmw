// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/eModel.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/World/CarInfo.hpp"

extern MenuScreen *FEngFindScreen(const char *name);
extern FEString *FEngFindString(const char *pkg_name, int hash);
extern void FEngSetLanguageHash(FEString *text, unsigned int hash);
extern int FEPrintf(FEString *text, const char *fmt, ...);
extern int FEngSNPrintf(char *, int, const char *, ...);
extern unsigned int FEHashUpper(const char *str);
extern int FEngMapJoyParamToJoyport(int feng_param);

extern void SetSelectCarLighting(int view_id, float f, int);
extern void eRotateZ(bMatrix4 *, bMatrix4 *, unsigned short);
extern void eRotateX(bMatrix4 *, bMatrix4 *, unsigned short);
extern void eRotateY(bMatrix4 *, bMatrix4 *, unsigned short);
extern void eMulVector(bVector3 *, const bMatrix4 *, const bVector3 *);
extern void PSMTX44Identity(void *mtx);
extern void PSMTX44Copy(const void *src, void *dst);
extern void eInitFEEnvMapPlat();
extern void eRemoveFEEnvMapPlat();
extern void GameFlowLoadGarageScreen(void (*callback)(int), int param);
extern void AddScreenEffect(ScreenEffectDB *db, ScreenEffectType type, float a, float b, float c, float d);

extern RideInfo TopOrFullScreenRide;
extern eSetRideInfoReasons TopOrFullScreenLoadingReason;

extern CarLoader TheCarLoader;

extern EmitterSystem gEmitterSystem;
extern float RealTimeElapsed;
extern unsigned int FrameMallocFailed;
extern unsigned int FrameMallocFailAmount;

extern float carPosX;
extern float carPosY;
extern float CarSelectTireSteerAngle;
extern int CarTypeInfoArrayUpdated;

struct SelectCarCameraMover;
extern void SetHRotateSpeed(SelectCarCameraMover *mover, float speed);
extern void SetVRotateSpeed(SelectCarCameraMover *mover, float speed);
extern void SetZoomSpeed(SelectCarCameraMover *mover, float speed);
extern void SetCurrentOrientation(SelectCarCameraMover *mover, bVector3 *target, float roll, float fov, bVector3 *lookat);
extern void SetDesiredOrientation(SelectCarCameraMover *mover, bVector3 *target, float roll, float fov, float anim_speed, float damping, bVector3 *lookat, int periods);
extern SelectCarCameraMover *NewSelectCarCameraMover(int view_id);
extern void SelectCarCameraMover_SetTime(SelectCarCameraMover *mover, float time);

struct EAXFrontEnd;
extern void DestroyAllDriveOnSnds(EAXFrontEnd *fe_snd);
extern void SetFEDrivingCarState(EAXFrontEnd *fe_snd, bVector3 *pos, bVector3 *vel, void *camera, int view_id);

extern eSolidListHeader *SolidList;

extern float cam_blur;
extern int CarGuysCamera;
extern float CarRotateSpeed;

static int sNumTicksSinceUserMovedCamera;
static int sNumTicksBeforeCamMovesBackToScreenPosition;
static int bAutoMovement;
static int bPass1;
static float zoomIn;
static float zoomOut;

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

// --- Free functions ---

unsigned int FindScreenInfo(const char *pkg_name, int category) {
    char name[128];
    char prefix[128];
    if (!pkg_name) {
        bStrCpy(name, "");
    } else {
        bStrCpy(name, pkg_name);
    }
    int len = bStrLen(name);
    if (len > 3) {
        name[len - 4] = 0;
        bMemSet(prefix, 0, 128);
        unsigned int flags = FEDatabase->mUserFlags;
        if (flags & 0x20) {
            bStrCat(prefix, "customize_", name);
            if (category > -1) {
                bSPrintf(prefix, "%s_%d", prefix, category);
            }
            unsigned int key = Attrib::StringToLowerCaseKey(prefix);
            Attrib::Gen::frontend inst(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
            if (inst.GetLayoutPointer()) {
                inst.~frontend();
                return key;
            }
            if (category > -1) {
                unsigned int fallback = FindScreenInfo(pkg_name, -1);
                inst.~frontend();
                return fallback;
            }
            inst.~frontend();
        } else if (flags & 0x8000) {
            bStrCat(prefix, "carlot_", name);
        } else if (flags & 1) {
            bStrCat(prefix, "career_", name);
        } else if ((flags & 4) && (flags & 0x400)) {
            bStrCat(prefix, "quickrace_", name);
        } else if (flags & 4) {
            bStrCat(prefix, "quickracemain_", name);
        } else if ((flags & 8) || (flags & 0x40)) {
            bStrCat(prefix, "quickracemain_", name);
        } else if (flags & 0x10) {
            bStrCat(prefix, "options_", name);
        } else if (flags & 0x100) {
            bStrCat(prefix, "career_", "manager");
        } else {
            bStrCat(prefix, "", name);
        }
        unsigned int key = Attrib::StringToLowerCaseKey(prefix);
        {
            Attrib::Gen::frontend inst(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
            if (inst.GetLayoutPointer()) {
                inst.~frontend();
                return key;
            }
            inst.~frontend();
        }
    }
    return 0x3b5aea62;
}

unsigned int FindGarageCameraInfo(const char *prefix) {
    char buf[64];
    bStrCpy(buf, prefix);
    const char *garage_name = GetCurrentGarageName();
    bStrCat(buf, buf, garage_name);
    unsigned int key = Attrib::StringToLowerCaseKey(buf);
    {
        Attrib::Gen::frontend inst(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), key), 0, nullptr);
        if (!inst.GetLayoutPointer()) {
            inst.~frontend();
            return 0xf907e767;
        }
        inst.~frontend();
    }
    return key;
}

unsigned int FindScreenCameraInfo(unsigned int screen_key) {
    {
        Attrib::Gen::frontend inst(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), screen_key), 0, nullptr);
        if (!inst.GetLayoutPointer()) {
            inst.~frontend();
            return 0xf907e767;
        }
        Attrib::Gen::frontend cam_inst(reinterpret_cast<Attrib::Gen::frontend::_LayoutStruct *>(inst.GetLayoutPointer())->cam_angle, 0, nullptr);
        unsigned int result = cam_inst.GetCollection();
        cam_inst.~frontend();
        inst.~frontend();
        return result;
    }
}

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
