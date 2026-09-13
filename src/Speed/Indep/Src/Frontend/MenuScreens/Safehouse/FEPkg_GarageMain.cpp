#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/DemoDisc.hpp"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/PSX2/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

// TODO: esolidplat
extern bTList<eSolid> SolidList;

float carPosX = 0.0f;
float carPosY = 0.0f;
float CarSelectTireSteerAngle = 21.6723f; // size: 0x4, address: 0x804390EC, Decl: 94
static int sNumTicksSinceUserMovedCamera = 0;
static int sNumTicksBeforeCamMovesBackToScreenPosition = 300;
static float CarRotateSpeed = 0.5f;
static bool bPass1 = false;
static bool bAutoMovement = false;
static float cam_blur = 0.0f; // size: 0x4, address: 0x80439104, Decl: 96

static Attrib::Key FindScreenInfo(const char *screenName, int customizationCategory) {
    char screenNameNoExt[128];
    if (screenName != nullptr) {
        bStrCpy(screenNameNoExt, screenName);
    } else {
        bStrCpy(screenNameNoExt, "");
    }
    int index = bStrLen(screenNameNoExt);
    if (index > 3) {
        screenNameNoExt[index - 4] = 0;
        char specialCase[128];
        bMemSet(specialCase, 0, 128);
        if (FEDatabase->IsCustomizeMode()) {
            bStrCat(specialCase, "customize_", screenNameNoExt);
            if (customizationCategory > -1) {
                bSPrintf(specialCase, "%s_%d", specialCase, customizationCategory);
            }
            Attrib::Key cameraKey = Attrib::StringToLowerCaseKey(specialCase);
            Attrib::Gen::frontend TheFrontend(cameraKey, 0, nullptr);
            if (TheFrontend.IsValid()) {
                return cameraKey;
            }
            if (customizationCategory > -1) {
                return FindScreenInfo(screenName, -1);
            }

        } else if (FEDatabase->IsCarLotMode()) {
            bStrCat(specialCase, "carlot_", screenNameNoExt);
        } else if (FEDatabase->IsCareerMode()) {
            bStrCat(specialCase, "career_", screenNameNoExt);
        } else if (FEDatabase->IsQuickRaceMode() && FEDatabase->IsModeSelectMode()) {
            bStrCat(specialCase, "quickrace_", screenNameNoExt);
        } else if (FEDatabase->IsQuickRaceMode()) {
            bStrCat(specialCase, "quickracemain_", screenNameNoExt);
        } else if (FEDatabase->IsOnlineMode() || FEDatabase->IsLANMode()) {
            bStrCat(specialCase, "online_", screenNameNoExt);
        } else if (FEDatabase->IsOptionsMode()) {
            bStrCat(specialCase, "options_", screenNameNoExt);
        } else if (FEDatabase->IsCareerManagerMode()) {
            bStrCat(specialCase, "career_", "manager");
        } else {
            bStrCat(specialCase, "", screenNameNoExt);
        }

        Attrib::Key cameraKey = Attrib::StringToLowerCaseKey(specialCase);
        Attrib::Gen::frontend TheFrontend(cameraKey, 0, nullptr);
        if (TheFrontend.IsValid()) {
            return cameraKey;
        }
    }
    return 0x3b5aea62;
}

static const char *GetCurrentGarageName() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM:
            return "backroom";
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return "career_safehouse";
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return "customization_shop";
        case GARAGETYPE_CAR_LOT:
            return "car_lot";
        case GARAGETYPE_MAIN_FE:
        default:
            return FEDatabase->IsCareerManagerMode() ? "career_manager" : "main_fe";
    }
}

static Attrib::Key FindGarageCameraInfo(const char *prefix) {
    char garageCameraAngle[64];
    bStrCpy(garageCameraAngle, prefix);
    bStrCat(garageCameraAngle, garageCameraAngle, GetCurrentGarageName());
    Attrib::Key cameraKey = Attrib::StringToLowerCaseKey(garageCameraAngle);
    Attrib::Gen::frontend TheFrontend(cameraKey, 0, nullptr);
    if (TheFrontend.IsValid()) {
        return cameraKey;
    }
    return 0xf907e767;
}

static Attrib::Key FindGarageEntryCameraInfo() {
    return FindGarageCameraInfo("angle_entry_");
}

static Attrib::Key FindGarageFinalCameraInfo() {
    return FindGarageCameraInfo("angle_final_");
}

static Attrib::Key FindScreenCameraInfo(Attrib::Key screenKey) {
    Attrib::Gen::frontend TheFrontend(screenKey, 0, nullptr);
    if (TheFrontend.IsValid()) {
        return Attrib::Gen::frontend(TheFrontend.cam_angle(), 0, nullptr).GetCollection();
    } else {
        return 0xf907e767;
    }
}

static bool HaveAttributesChanged(Attrib::Gen::frontend &attribSet) {
    return false;
}

void FEGeometryModels::Init(char *filterPrefix) {
    const int kMaxModels = 32;
    eSolid *SolidTable[kMaxModels];
    int filterPrefixSize;

    mNumModels = 0;
    filterPrefixSize = bStrLen(filterPrefix);
    for (eSolid *solid = SolidList.GetHead(); solid != SolidList.EndOfList(); solid = solid->GetNext()) {
        if (bStrNICmp(solid->GetName(), filterPrefix, filterPrefixSize) == 0) {
            SolidTable[mNumModels++] = solid;
        }
    }

    if (mNumModels != 0) {
        mModels = new ("FEGeometryModels", 0) eModel[mNumModels];
        for (int i = 0; i < mNumModels; i++) {
            mModels[i].Init(SolidTable[i]->NameHash);
            if (bStrIStr(mModels[i].GetSolid()->GetName(), "CAST_SHADOW_MAP") != nullptr) {
                mModelCastsShadowMapFlags |= 1 << i;
            }
            if (bStrIStr(mModels[i].GetSolid()->GetName(), "CURRGEN") != nullptr) {
                mModelCurrGenOnly |= 1 << i;
            }
            if (bStrIStr(mModels[i].GetSolid()->GetName(), "NEXTGEN") != nullptr) {
                mModelNextGenOnly |= 1 << i;
            }
        }
    }
}

void FEGeometryModels::UnInit() {
    if (mModels != nullptr) {
        delete[] mModels;
    }
    mModels = nullptr;
    mModelCastsShadowMapFlags = 0;
    mModelCurrGenOnly = 0;
    mModelNextGenOnly = 0;
}

void FEGeometryModels::Render(eView *view, bMatrix4 *local, uint32 render_flags) {
    for (int i = 0; i < mNumModels; i++) {
        bool renderModel = true;
        if ((render_flags & 4) != 0) {
            if ((1 << i & mModelCastsShadowMapFlags) == 0) {
                renderModel = false;
            }
        }
        if ((render_flags & 1) != 0) {
            if ((1 << i & mModelNextGenOnly) != 0) {
                renderModel = false;
            }
        }
        if ((render_flags & 2) != 0 && (1 << i & mModelCurrGenOnly) != 0) {
            renderModel = false;
        }
        if (renderModel) {
            reinterpret_cast<eViewPlatInterface *>(view)->Render(&mModels[i], local, nullptr, 4, nullptr);
        }
    }
}

GarageMainScreen::GarageMainScreen(ScreenConstructorData *sd, int eview_id, RideInfo *start_ride, int player)
    : MenuScreen(sd), HideEntireScreen(1), ViewID(eview_id), bUserRotate(false), mZoom(0.0f), mCustomizationCategory(-1),
      LoadingReason(SET_RIDE_INFO_REASON_LOAD_CAR), RenderingCar(nullptr), mGeometryModels(), Player(player), CameraPushRequested(false),
      mScreenKeyCamIsSetTo(0), mOrbitV(0.0f), mOrbitH(0.0f) {

    for (int i = 0; i < 2; i++) {
        mActionQ[i] = new ActionQueue(i, 0x82d21520, "GarageMainScreen", false);
        mActionQ[i]->Enable(true);
    }

    if (player == 0) {
        const u32 FEObj_CarNameP1 = 0xdb8ccef6;
        const u32 FEObj_PlayerNameP1 = 0x83003e0d;
        pCarName = FEngFindString(GetPackageName(), FEObj_CarNameP1);
        pPlayerName = FEngFindString(GetPackageName(), FEObj_PlayerNameP1);
        FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName());
        TheDemoDiscManager.IsActive(); // Unknown usage
    } else if (player == 1) {
        const u32 FEObj_CarNameP2 = 0xdb8ccef7;
        const u32 FEObj_PlayerNameP2 = 0x83003e0e;
        pCarName = FEngFindString(GetPackageName(), FEObj_CarNameP2);
        pPlayerName = FEngFindString(GetPackageName(), FEObj_PlayerNameP2);
        FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(1)->GetProfileName());
    }

    TheGarageCarLoader = GetGarageCarLoader();
    SetRideInfo(start_ride, LoadingReason);
    CarState = 0;
    RenderingCar = new ("FrontendRenderingCar", 0) FrontEndRenderingCar(nullptr, ViewID);
    pCameraMover = new ("SelectCarCameraMover", 0) SelectCarCameraMover(ViewID);
    mGeometryModels.Init("BACKDROP");

    CarTypeInfo *cti = GetCarTypeInfo(start_ride->Type);
    char sztemp[32];
    FEngSNPrintf(sztemp, sizeof(sztemp), "CAR_NAME_%s", cti->GetName());
    FEngSetLanguageHash(pCarName, FEHashUpper(sztemp));
    SetSelectCarLighting(ViewID, 1.0f, 0);
    HandleTick(0);
}

GarageMainScreen::~GarageMainScreen() {
    if (pCameraMover != nullptr) {
        delete pCameraMover;
    }
    if (RenderingCar != nullptr) {
        delete RenderingCar;
    }
    mGeometryModels.UnInit();
    if (g_pEAXSound->GetFrontEnd() != nullptr) {
        g_pEAXSound->GetFrontEnd()->DestroyAllDriveOnSnds();
    }
    for (int i = 0; i < 2; i++) {
        if (mActionQ[i] != nullptr) {
            delete mActionQ[i];
            mActionQ[i] = nullptr;
        }
    }
}

GarageMainScreen *GarageMainScreen::GetInstance() {
    return static_cast<GarageMainScreen *>(FEngFindScreen("GarageMain.fng"));
}

void GarageMainScreen::EnableCarRendering() {
    if (RenderingCar != nullptr) {
        RenderingCar->Visible = 1;
    }
}

void GarageMainScreen::DisableCarRendering() {
    if (RenderingCar != nullptr) {
        RenderingCar->Visible = 0;
    }
}

bool GarageMainScreen::IsCarRendering() {
    if ((RenderingCar != nullptr) && RenderingCar->Visible) {
        return true;
    }
    return false;
}

// UNSOLVED
void GarageMainScreen::HandleTick(u32 msg) {
    bool have_new_car = false;
    if (CarState == 0 && TheGarageCarLoader->HasSwitched()) {
        TheGarageCarLoader->Switch();
        CarState = 1;
        have_new_car = true;
    }
    if (have_new_car) {
        RideInfo *CurrentRideInfo = TheGarageCarLoader->GetCurrentRideInfo();
        if (CurrentRideInfo != nullptr) {
            RenderingCar->ReInit(CurrentRideInfo);
            RenderingCar->Visible = 1;
            const u32 FEObj_LOADERLEAVE = 0x913fa282;
            cFEng::Get()->QueuePackageMessage(FEObj_LOADERLEAVE, nullptr, nullptr);
        }
    }
    HandleJoyEvents();

    if (mOrbitV == 0.0f && mOrbitH == 0.0f && mZoom == 0.0f && sNumTicksSinceUserMovedCamera > 0 && CarGuysCamera == false) {
        sNumTicksSinceUserMovedCamera--;
    }

    bool bTimeToRotate = false;
    if (sNumTicksSinceUserMovedCamera == 0 && bUserRotate) {
        bTimeToRotate = bAutoMovement == false;
    }
    if (bTimeToRotate && bPass1) {
        pCameraMover->SetHRotateSpeed(CarRotateSpeed);
        bPass1 = false;
        bAutoMovement = true;
        bTimeToRotate = false;
    }

    FEPackage *currentControllingPackage = cFEng::Get()->FindPackageAtBase();
    if (currentControllingPackage != nullptr) {
        const Attrib::Key screenKey = FindScreenInfo(currentControllingPackage->GetName(), mCustomizationCategory);
        const Attrib::Key attribKey = FindScreenCameraInfo(screenKey);
        Attrib::Gen::frontend camera(attribKey, 0, nullptr);
        Attrib::Gen::frontend screen(screenKey, 0, nullptr);
        if (screenKey != mScreenKeyCamIsSetTo) {
            sNumTicksSinceUserMovedCamera = static_cast<int>(camera.cam_anim_speed() * 60.0f);
            bPass1 = false;
            bAutoMovement = false;
        } else if (bTimeToRotate) {
            sNumTicksSinceUserMovedCamera = static_cast<int>(camera.cam_anim_speed() * 60.0f);
            bPass1 = true;
        }

        mScreenKeyCamIsSetTo = screenKey;
        bUserRotate = screen.cam_user_rotate();
        if (!CameraPushRequested) {
            bVector3 orbit(camera.cam_orbit_vertical(), camera.cam_orbit_horizontal(), camera.cam_orbit_radius());
            bVector3 lookAt(camera.cam_lookat_x(), camera.cam_lookat_y(), camera.cam_lookat_z());
            pCameraMover->SetDesiredOrientation(orbit, camera.cam_roll_angle(), camera.cam_fov(), lookAt, camera.cam_anim_speed(),
                                                camera.cam_damping(), camera.cam_periods());

        } else if (HaveAttributesChanged(camera)) {
            bVector3 orbit(camera.cam_orbit_vertical(), camera.cam_orbit_horizontal(), camera.cam_orbit_radius());
            bVector3 lookAt(camera.cam_lookat_x(), camera.cam_lookat_y(), camera.cam_lookat_z());
            pCameraMover->SetCurrentOrientation(orbit, camera.cam_roll_angle(), camera.cam_fov(), lookAt);
        }
    }

    eView *view = eGetView(1, false);
    if (view != nullptr && view->ScreenEffects != nullptr) {
        view->ScreenEffects->AddScreenEffect(SE_FE_BLUR, cam_blur, 0.0f, 0.0f, 0.0f);
    }
    UpdateRenderingCarParameters(RenderingCar);
    RefreshBackground();
}

void GarageMainScreen::SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason) {
    TheGarageCarLoader->LoadRideInfo(ride);
    CarState = 0;
    if (TheGarageCarLoader->GetCurrentRideInfo() != nullptr) {
        if (TheGarageCarLoader->GetCurrentRideInfo()->Type != ride->Type) {
            DisableCarRendering();
            const u32 FEObj_LOADERAPPEAR = 0xa05a328e;
            cFEng::Get()->QueuePackageMessage(FEObj_LOADERAPPEAR, nullptr, nullptr);
        }
    }
    CarTypeInfo *cti = GetCarTypeInfo(ride->Type);
    char sztemp[32];
    FEngSNPrintf(sztemp, 32, "CAR_NAME_%s", cti->GetName());
    FEngSetLanguageHash(pCarName, FEHashUpper(sztemp));
}

void GarageMainScreen::CancelCarLoad() {
    CarState = 1;
    TheGarageCarLoader->CancelCarLoad();
}

void GarageMainScreen::UpdateCurrentCameraView(bool bForce) {
    if (!CameraPushRequested && !bForce) {
        return;
    }

    const Attrib::Key currentAttribKey = FindGarageEntryCameraInfo();
    Attrib::Gen::frontend currentCamera(currentAttribKey, 0, nullptr);

    bVector3 currentOrbit(currentCamera.cam_orbit_vertical(), currentCamera.cam_orbit_horizontal(), currentCamera.cam_orbit_radius());
    bVector3 currentLookAt(currentCamera.cam_lookat_x(), currentCamera.cam_lookat_y(), currentCamera.cam_lookat_z());
    pCameraMover->SetCurrentOrientation(currentOrbit, currentCamera.cam_roll_angle(), currentCamera.cam_fov(), currentLookAt);

    const Attrib::Key desiredAttribKey = FindGarageFinalCameraInfo();
    Attrib::Gen::frontend desiredCamera(desiredAttribKey, 0, nullptr);

    bVector3 desiredOrbit(desiredCamera.cam_orbit_vertical(), desiredCamera.cam_orbit_horizontal(), desiredCamera.cam_orbit_radius());
    bVector3 desiredLookAt(desiredCamera.cam_lookat_x(), desiredCamera.cam_lookat_y(), desiredCamera.cam_lookat_z());
    pCameraMover->SetDesiredOrientation(desiredOrbit, desiredCamera.cam_roll_angle(), desiredCamera.cam_fov(), desiredLookAt,
                                        desiredCamera.cam_anim_speed(), desiredCamera.cam_damping(), desiredCamera.cam_periods());

    CameraPushRequested = false;
}

void GarageMainScreen::RefreshBackground() {
    const char *filename = FEManager::Get()->GetGarageNameFromType();
    ResourceFile *pGarageBackground = FEManager::Get()->GetGarageBackground();
    char resource_filename[128];
    bStrCpy(resource_filename, pGarageBackground->GetFilename());
    char *extension = bStrIStr(resource_filename, ".");
    bStrCpy(extension, ".BIN");
    if ((pGarageBackground == nullptr) || bStrCmp(resource_filename, filename) != 0) {
        new EFadeScreenOn(false);
        eRemoveFEEnvMapPlat();
        eInitFEEnvMapPlat();
        UnloadResourceFile(pGarageBackground);
        GameFlowLoadGarageScreen(BackgroundLoaded, 0);
    }
}

void GarageMainScreen::BackgroundLoaded(int param) {
    GarageMainScreen *pGarage = GetInstance();
    if (pGarage != nullptr) {
        new EFadeScreenOff(FEHASH_15_IN);
        pGarage->mGeometryModels.UnInit();
        pGarage->mGeometryModels.Init("BACKDROP");
        pGarage->UpdateCurrentCameraView(true);
        pGarage->pCameraMover->Update(0.0f);
    }
}

float GarageMainScreen::GetCarRotationX() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_CAR_LOT:
            return -0.379623f;
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 0.0f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
    }
}

float GarageMainScreen::GetCarRotationY() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_CAR_LOT:
            return -0.000193f;
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 0.0f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
    }
}

float GarageMainScreen::GetCarRotationZ() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_CAR_LOT:
            return 340.0f;
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 304.96979f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 304.96979f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 304.96979f;
    }
}

float GarageMainScreen::GetGeometryZAngle() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 302.85309f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
        case GARAGETYPE_CAR_LOT:
            return 0.0f;
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 134.4125f;
    }
}

float GarageMainScreen::GetGeometryXPos() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 0.0f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return 0.0f;
    }
}

float GarageMainScreen::GetGeometryYPos() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_CAR_LOT:
            return 0.075f;
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 0.0f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
    }
}

float GarageMainScreen::GetGeometryZPos() {
    switch (FEManager::Get()->GetGarageType()) {
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 0.0f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 0.0f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
            return 0.0f;
        case GARAGETYPE_CAR_LOT:
            return 0.0f;
    }
}

void GarageMainScreen::UpdateRenderingCarParameters(FrontEndRenderingCar *fe_car) {
    RideInfo *loaded_ride_info = fe_car->GetRideInfo();
    if (reinterpret_cast<int>(fe_car) == -8 || loaded_ride_info->Type == CARTYPE_NONE || HideEntireScreen) {
        fe_car->Visible = 0;
        return;
    }

    if (CarTypeInfoArrayUpdated) {
        CarTypeInfoArrayUpdated = 0;
    }

    bVector4 wheel_positions[4];
    float wheel_radius[4];
    float average_wheel_radius = 0.0f;
    float average_wheel_z = 0.0f;

    for (unsigned int i = 0; i <= 3; i++) {
        if (!fe_car->LookupWheelPosition(i, &wheel_positions[i])) {
            wheel_positions[i] = bVector4(0.0f, 0.0f, 0.0f, 1.0f);
        }
        if (!fe_car->LookupWheelRadius(i, wheel_radius[i])) {
            wheel_radius[i] = 1.5f;
        }
        average_wheel_radius += wheel_radius[i];
        average_wheel_z += wheel_positions[i].z;
        wheel_positions[i].w = 1.0f;
    }

    average_wheel_radius /= 4;
    average_wheel_z /= 4;

    fe_car->LightsOn = 0;
    fe_car->CopLightsOn = 0;
    float height = average_wheel_radius - average_wheel_z + (-0.025f);
    bVector3 position(carPosX, carPosY, height);

    bMatrix4 temp;
    eIdentity(&temp);
    eRotateZ(&temp, &temp, bDegToAng(GetGeometryZAngle()));
    eMulVector(&position, &temp, &position);
    fe_car->SetPosition(&position);

    bMatrix4 body_matrix;
    eIdentity(&body_matrix);
    eRotateZ(&body_matrix, &body_matrix, bDegToAng(GetCarRotationZ()));
    eRotateX(&body_matrix, &body_matrix, bDegToAng(GetCarRotationX()));
    eRotateY(&body_matrix, &body_matrix, bDegToAng(GetCarRotationY()));
    fe_car->SetBodyMatrix(&body_matrix);

    bMatrix4 tire_matrices[4];
    bMatrix4 brake_matrices[4];
    unsigned short front_tire_angle = bDegToAng(CarSelectTireSteerAngle);

    for (int tire_num = 0; tire_num < 4; tire_num++) {
        eIdentity(&tire_matrices[tire_num]);
        eIdentity(&brake_matrices[tire_num]);
        if (tire_num < 2) {
            eRotateZ(&brake_matrices[tire_num], &brake_matrices[tire_num], front_tire_angle);
            eRotateZ(&tire_matrices[tire_num], &tire_matrices[tire_num], front_tire_angle);
        }
        bCopy(&tire_matrices[tire_num].v3, &wheel_positions[tire_num]);
        bCopy(&brake_matrices[tire_num].v3, &wheel_positions[tire_num]);
    }

    fe_car->SetTireMatrices(tire_matrices);
    fe_car->SetBrakeMatrices(brake_matrices);

    if (g_pEAXSound->GetFrontEnd() != nullptr) {
        RideInfo *CurrentRideInfo = TheGarageCarLoader->GetCurrentRideInfo();
        if (CurrentRideInfo != nullptr) {
            bVector3 car_velocity(0.0f, 0.0f, 0.0f);
            eView *view = eGetView(0, false);
            Camera *camera = view->GetCamera();
            g_pEAXSound->GetFrontEnd()->SetFEDrivingCarState(&position, &car_velocity, camera, ViewID);
        }
    }
}

void GarageMainScreen::HandleRender(uint32 render_flags) {
    if (HideEntireScreen != 0) {
        return;
    }
    eView *view = eGetView(ViewID, false);
    {
        bMatrix4 *local = eFrameMallocMatrix(1);

        if (local != nullptr) {
            eIdentity(local);
            eRotateZ(local, local, bDegToAng(GetGeometryZAngle()));
            local->v3.x = GetGeometryXPos();
            local->v3.y = GetGeometryYPos();
            local->v3.z = GetGeometryZPos();
            mGeometryModels.Render(view, local, render_flags);
        }
        gEmitterSystem.Update(RealTimeElapsed);
    }
}

void GarageMainScreen::HandleShowPackage(uint32 msg) {
    RenderingCar->Visible = 1;
    if (!FEDatabase->IsQuickRaceMode()) {
        UpdateCurrentCameraView(true);
        pCameraMover->Update(0.0f);
        GarageMainScreen::RequestCameraPush();
    }
}

void GarageMainScreen::HandleHidePackage(uint32 msg) {
    RenderingCar->Visible = 0;
}

// UNSOLVED
void GarageMainScreen::HandleJoyEvents() {
    int firstPortToCheck = 0;
    int lastPortToCheck = 2;

    if (FEDatabase->IsSplitScreenMode()) {
        FEPackage *packageWithCtrl = cFEng::Get()->FindPackageWithControl();
        if (packageWithCtrl != nullptr) {
            firstPortToCheck = FEngMapJoyParamToJoyport(packageWithCtrl->GetControlMask());
            lastPortToCheck = firstPortToCheck + 1;
        }
    }
    for (int port = firstPortToCheck; port < lastPortToCheck; port++) {
        if (mActionQ[port] == nullptr)
            continue;
        while (!mActionQ[port]->IsEmpty()) {
            if (bUserRotate || CarGuysCamera) {
                ActionRef aRef = mActionQ[port]->GetAction();
                float controllerValue;
                if (mActionQ[port]->IsConnected()) {
                    controllerValue = aRef.Data();
                } else {
                    controllerValue = 0.0f;
                }
                int msg = aRef.ID();
                switch (msg) {
                    case 0x88:
                        pCameraMover->SetVRotateSpeed(0.0f);
                        pCameraMover->SetHRotateSpeed(0.0f);
                        pCameraMover->SetZoomSpeed(0.0f);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        break;
                    case 0x1d:
                        mOrbitV = controllerValue;
                        pCameraMover->SetVRotateSpeed(controllerValue);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        break;
                    case 0x1e:
                        mOrbitV = -controllerValue;
                        pCameraMover->SetVRotateSpeed(-controllerValue);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        break;
                    case 0x1f:
                        mOrbitH = controllerValue;
                        pCameraMover->SetHRotateSpeed(controllerValue);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        break;
                    case 0x20:
                        mOrbitH = -controllerValue;
                        pCameraMover->SetHRotateSpeed(-controllerValue);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        break;
                    case 0x2b:
                    case 0x2c: {
                        static float zoomIn = 0.0f;
                        static float zoomOut = 0.0f;

                        if (aRef.ID() == 0x2b) {
                            zoomOut = controllerValue;
                            sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        } else {
                            zoomIn = -controllerValue;
                            sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        }

                        if (bAbs(zoomIn) > bAbs(zoomOut)) {
                            mZoom = zoomIn;
                        } else if (bAbs(zoomOut) > bAbs(zoomIn)) {
                            mZoom = zoomOut;
                        } else {
                            if (zoomOut == 0.0f && zoomIn == 0.0f) {
                                mZoom = 0.0f;
                            }
                        }
                        pCameraMover->SetZoomSpeed(mZoom);

                        break;
                    }
                }

                if (sNumTicksSinceUserMovedCamera > 0) {
                    if (bAutoMovement) {
                        if (aRef.ID() != 0x1f && aRef.ID() != 0x20) {
                            pCameraMover->SetHRotateSpeed(0.0f);
                        }
                    }
                    bAutoMovement = false;
                }
            }
            mActionQ[port]->PopAction();
        }
    }
}

void GarageMainScreen::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    switch (Message) {
        case FEMSG_HIDE_PACKAGE:
            HideEntireScreen = 1;
            HandleHidePackage(FEMSG_HIDE_PACKAGE);
            break;
        case FEMSG_SHOW_PACKAGE:
            HideEntireScreen = 0;
            HandleShowPackage(FEMSG_SHOW_PACKAGE);
            break;
        case FEMSG_ERROR_STATE:
            HandleJoyEvents();
            break;
        case FEMSG_SCREEN_TICK:
            HandleTick(FEMSG_SCREEN_TICK);
            break;
    }
}

MenuScreen *CreateGarageMainScreen(ScreenConstructorData *sd) {
    return new ("GarageMainScreen", 0) GarageMainScreen(sd, 1, &TopOrFullScreenRide, 0);
}

GarageCarLoader::GarageCarLoader()
    : LoadingRideInfo(), CurrentRideInfo(), IsLoadingRide(false), IsCurrentRide(false), LoadingCar(0), CurrentCar(0), IsDifferent(false),
      UseFirstDummyTexturesForNextLoad(true) {}

GarageCarLoader::~GarageCarLoader() {
    CleanUp();
}

void GarageCarLoader::Init() {
    LoadingCar = 0;
    CurrentCar = 0;
    IsLoadingRide = false;
    IsCurrentRide = false;
}

void GarageCarLoader::CleanUp() {
    if (IsLoadingRide && LoadingCar) {
        TheCarLoader.Unload(LoadingCar);
    }
    if (IsCurrentRide && CurrentCar) {
        TheCarLoader.Unload(CurrentCar);
    }
    LoadingCar = 0;
    CurrentCar = 0;
    IsLoadingRide = false;
    IsCurrentRide = false;
}

void GarageCarLoader::CancelCarLoad() {
    if (IsLoadingRide) {
        TheCarLoader.Unload(LoadingCar);
    }
}

void GarageCarLoader::LoadRideInfo(RideInfo *ride_info) {
    if (IsLoadingRide) {
        TheCarLoader.Unload(LoadingCar);
    }
    int dummy_texture_number = 1;
    if (!UseFirstDummyTexturesForNextLoad) {
        dummy_texture_number = 2;
    }
    ride_info->SetCompositeNameHash(dummy_texture_number);
    LoadingCar = TheCarLoader.Load(ride_info);
    TheCarLoader.BeginLoading(nullptr, 0);
    IsLoadingRide = true;
    LoadingRideInfo = *ride_info;
    IsDifferent = false;
}

RideInfo *GarageCarLoader::GetLoadingRideInfo() {
    if (IsLoadingRide) {
        return &LoadingRideInfo;
    }
    return nullptr;
}

RideInfo *GarageCarLoader::GetCurrentRideInfo() {
    if (IsCurrentRide) {
        return &CurrentRideInfo;
    }
    return nullptr;
}

void GarageCarLoader::Switch() {
    IsDifferent = false;
}

void GarageCarLoader::Update() {
    if (IsLoadingRide && TheCarLoader.IsLoaded(LoadingCar)) {
        if (IsCurrentRide) {
            CarTypeInfo *cti = GetCarTypeInfo(CurrentRideInfo.Type); // unknown
            TheCarLoader.Unload(CurrentCar);
        } else {
            CarTypeInfo *cti = GetCarTypeInfo(LoadingRideInfo.Type); // unknown
        }
        IsCurrentRide = true;
        CurrentCar = LoadingCar;
        CurrentRideInfo = LoadingRideInfo;
        IsDifferent = true;
        LoadingCar = 0;
        IsLoadingRide = false;
        UseFirstDummyTexturesForNextLoad = UseFirstDummyTexturesForNextLoad != true;
    }
}

GarageCarLoader *GetGarageCarLoader() {
    static GarageCarLoader TheGarageCarLoader;
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

GarageMainScreen *CarViewer::FindWhichScreenToUpdate(eCarViewerWhichCar which_car) {
    if (cFEng::Get()->IsPackagePushed("GarageMain.fng")) {
        return static_cast<GarageMainScreen *>(FEngFindScreen("GarageMain.fng"));
    }
    return nullptr;
}

// UNSOLVED
void CarViewer::SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car) {
    GarageMainScreen *screen = FindWhichScreenToUpdate(which_car);
    RideInfo *update_this_ride = &TopOrFullScreenRide;
    *update_this_ride = *ride;
    TopOrFullScreenLoadingReason = reason;
    if (screen != nullptr) {
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

bool CarViewer::haveLoadedOnce = false;
