#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
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

EAXSound *g_pEAXSound; // size: 0x4, Decl: speed/indep/src/frontend/menuscreens/FeGarageMain.cpp:57

float carPosX = 0.0f;
float carPosY = 0.0f;
float CarSelectTireSteerAngle = 21.6723f; // size: 0x4, address: 0x804390EC, Decl: speed/indep/src/frontend/menuscreens/FeGarageMain.cpp:94
static int sNumTicksSinceUserMovedCamera = 0;
static int sNumTicksBeforeCamMovesBackToScreenPosition = 300;
static float CarRotateSpeed = 0.5f;
static bool bPass1 = false;
static bool bAutoMovement = false;
static float cam_blur = 0.0f; // size: 0x4, address: 0x80439104, Decl: speed/indep/src/frontend/menuscreens/FeGarageMain.cpp:96

static Attrib::Key FindScreenInfo(const char *pkg_name, int category) {
    char name[128];
    char prefix[128];
    if (pkg_name) {
        bStrCpy(name, pkg_name);
    } else {
        bStrCpy(name, "");
    }
    int len = bStrLen(name);
    if (len > 3) {
        name[len - 4] = 0;
        bMemSet(prefix, 0, 128);
        unsigned int flags = FEDatabase->GetGameMode();
        if (flags & 0x20) {
            bStrCat(prefix, "customize_", name);
            if (category > -1) {
                bSPrintf(prefix, "%s_%d", prefix, category);
            }
            unsigned int key = Attrib::StringToLowerCaseKey(prefix);
            {
                Attrib::Gen::frontend inst(Attrib::FindCollection(0x85885722, key), 0, nullptr);
                bool hasCollection = inst.GetConstCollection() != 0;
                if (hasCollection) {
                    return key;
                }
                if (category > -1) {
                    return FindScreenInfo(pkg_name, -1);
                }
            }
        } else if (flags & 0x8000) {
            bStrCat(prefix, "carlot_", name);
        } else if (flags & 1) {
            bStrCat(prefix, "career_", name);
        } else if (flags & 4) {
            if (flags & 0x400) {
                bStrCat(prefix, "quickrace_", name);
            } else {
                bStrCat(prefix, "quickracemain_", name);
            }
        } else if (flags & 8) {
            bStrCat(prefix, "quickracemain_", name);
        } else if (flags & 0x40) {
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
            Attrib::Gen::frontend inst(Attrib::FindCollection(0x85885722, key), 0, nullptr);
            bool hasCollection = inst.GetConstCollection() != 0;
            if (hasCollection) {
                return key;
            }
        }
    }
    return 0x3b5aea62;
}

static const char *GetCurrentGarageName() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
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
            break;
    }
    const char *name;
    if (FEDatabase->IsCareerManagerMode()) {
        name = "career_manager";
    } else {
        name = "main_fe";
    }
    return name;
}

static Attrib::Key FindGarageCameraInfo(const char *prefix) {
    char garageCameraAngle[64];
    bStrCpy(garageCameraAngle, prefix);
    bStrCat(garageCameraAngle, garageCameraAngle, GetCurrentGarageName());
    Attrib::Key cameraKey = Attrib::StringToLowerCaseKey(garageCameraAngle);
    Attrib::Gen::frontend TheFrontend(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), cameraKey), 0, nullptr);
    bool hasCollection = TheFrontend.GetConstCollection() != 0;
    if (hasCollection) {
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

static Attrib::Key FindScreenCameraInfo(Attrib::Key screen_key) {
    Attrib::Gen::frontend TheFrontend(Attrib::FindCollection(0x85885722, screen_key), 0, nullptr);
    Attrib::Key collectionKey = 0xf907e767;
    if (TheFrontend.GetConstCollection()) {
        Attrib::Gen::frontend cam_inst(reinterpret_cast<Attrib::Gen::frontend::_LayoutStruct *>(TheFrontend.GetLayoutPointer())->cam_angle, 0,
                                       nullptr);
        collectionKey = cam_inst.GetCollection();
    }
    return collectionKey;
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
        mModels = new eModel[mNumModels];
        for (int i = 0; i < mNumModels; i++) {
            mModels[i].Init(SolidTable[i]->NameHash);
            if (bStrIStr(mModels[i].GetSolid()->GetName(), "CAST_SHADOW_MAP")) {
                mModelCastsShadowMapFlags |= 1 << i;
            }
            if (bStrIStr(mModels[i].GetSolid()->GetName(), "CURRGEN")) {
                mModelCurrGenOnly |= 1 << i;
            }
            if (bStrIStr(mModels[i].GetSolid()->GetName(), "NEXTGEN")) {
                mModelNextGenOnly |= 1 << i;
            }
        }
    }
}

void FEGeometryModels::UnInit() {
    if (mModels) {
        eModel *end = mModels + mNumModels;
        while (mModels != end) {
            end--;
            end->UnInit();
        }
        ::operator delete[](reinterpret_cast<char *>(mModels) - 8);
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
    : MenuScreen(sd) //
{
    HideEntireScreen = 1;
    ViewID = eview_id;
    bUserRotate = false;
    mZoom = 0.0f;
    mCustomizationCategory = -1;
    LoadingReason = static_cast<eSetRideInfoReasons>(1);
    RenderingCar = nullptr;
    mGeometryModels = FEGeometryModels();
    Player = player;
    CameraPushRequested = false;
    mScreenKeyCamIsSetTo = 0;
    mOrbitV = 0.0f;
    mOrbitH = 0.0f;

    int i = 0;
    do {
        mActionQ[i] = new ActionQueue(i, 0x82d21520, "GarageMainScreen", false);
        mActionQ[i]->Enable(true);
        i++;
    } while (i < 2);

    if (player == 0) {
        pCarName = FEngFindString(GetPackageName(), 0xdb8ccef6);
        pPlayerName = FEngFindString(GetPackageName(), 0x83003e0d);
        FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(0)->GetProfileName());
    } else if (player == 1) {
        pCarName = FEngFindString(GetPackageName(), 0xdb8ccef7);
        pPlayerName = FEngFindString(GetPackageName(), 0x83003e0e);
        FEPrintf(pPlayerName, "%s", FEDatabase->GetUserProfile(1)->GetProfileName());
    }

    TheGarageCarLoader = GetGarageCarLoader();
    SetRideInfo(start_ride, LoadingReason);
    CarState = 0;
    RenderingCar = new FrontEndRenderingCar(nullptr, ViewID);
    pCameraMover = new SelectCarCameraMover(ViewID);
    mGeometryModels.Init("BACKDROP");

    char sztemp[32];
    FEngSNPrintf(sztemp, 0x20, "CAR_NAME_%s", GetCarTypeInfo(start_ride->Type)->CarTypeName);
    FEngSetLanguageHash(pCarName, FEHashUpper(sztemp));
    SetSelectCarLighting(ViewID, 1.0f, 0);
    HandleTick(0);
}

GarageMainScreen::~GarageMainScreen() {
    if (pCameraMover) {
        delete pCameraMover;
    }
    if (RenderingCar) {
        delete RenderingCar;
    }
    mGeometryModels.UnInit();
    if (g_pEAXSound->GetFrontEnd()) {
        g_pEAXSound->GetFrontEnd()->DestroyAllDriveOnSnds();
    }
    for (int i = 0; i < 2; i++) {
        if (mActionQ[i]) {
            delete mActionQ[i];
            mActionQ[i] = nullptr;
        }
    }
}

GarageMainScreen *GarageMainScreen::GetInstance() {
    return static_cast<GarageMainScreen *>(FEngFindScreen("GarageMain.fng"));
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

void GarageMainScreen::HandleTick(u32 msg) {
    bool have_new_car = false;
    if (CarState == 0 && TheGarageCarLoader->HasSwitched()) {
        TheGarageCarLoader->Switch();
        have_new_car = true;
        CarState = 1;
    }
    if (have_new_car) {
        RideInfo *CurrentRideInfo = TheGarageCarLoader->GetCurrentRideInfo();
        if (CurrentRideInfo) {
            RenderingCar->ReInit(CurrentRideInfo);
            RenderingCar->Visible = 1;
            cFEng::Get()->QueuePackageMessage(0x913fa282, nullptr, nullptr);
        }
    }
    HandleJoyEvents();

    if (mOrbitV == 0.0f && mOrbitH == 0.0f && mZoom == 0.0f && sNumTicksSinceUserMovedCamera > 0 && CarGuysCamera == 0) {
        sNumTicksSinceUserMovedCamera--;
    }

    bool bTimeToRotate = false;
    if (sNumTicksSinceUserMovedCamera == 0 && bUserRotate) {
        bTimeToRotate = bAutoMovement == 0;
    }
    if (bTimeToRotate && bPass1) {
        bTimeToRotate = false;
        pCameraMover->SetHRotateSpeed(CarRotateSpeed);
        bAutoMovement = 1;
        bPass1 = 0;
    }

    FEPackage *currentControllingPackage = cFEng::Get()->FindPackageAtBase();
    if (!currentControllingPackage)
        goto after_camera;
    {
        const unsigned int screenKey = FindScreenInfo(currentControllingPackage->GetName(), mCustomizationCategory);
        const unsigned int attribKey = FindScreenCameraInfo(screenKey);
        Attrib::Gen::frontend camera(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), attribKey), 0, nullptr);
        if (!camera.GetLayoutPointer()) {
            camera.SetDefaultLayout(sizeof(Attrib::Gen::frontend::_LayoutStruct));
        }
        Attrib::Gen::frontend screen(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), screenKey), 0, nullptr);
        if (!screen.GetLayoutPointer()) {
            screen.SetDefaultLayout(sizeof(Attrib::Gen::frontend::_LayoutStruct));
        }

        if (screenKey != mScreenKeyCamIsSetTo) {
            float anim_speed = camera.cam_anim_speed();
            bAutoMovement = 0;
            bPass1 = 0;
            sNumTicksSinceUserMovedCamera = static_cast<int>(anim_speed * 60.0f);
            mScreenKeyCamIsSetTo = screenKey;
            bUserRotate = screen.cam_user_rotate();
            if (!CameraPushRequested) {
                bVector3 orbit(camera.cam_orbit_vertical(), camera.cam_orbit_horizontal(), camera.cam_orbit_radius());
                bVector3 lookat(camera.cam_lookat_x(), camera.cam_lookat_y(), camera.cam_lookat_z());
                pCameraMover->SetDesiredOrientation(orbit, camera.cam_roll_angle(), camera.cam_fov(), lookat, camera.cam_anim_speed(),
                                                    camera.cam_damping(), camera.cam_periods());
            }
        } else {
            if (bTimeToRotate) {
                float anim_speed = camera.cam_anim_speed();
                bPass1 = 1;
                sNumTicksSinceUserMovedCamera = static_cast<int>(anim_speed * 60.0f);
                mScreenKeyCamIsSetTo = screenKey;
                bUserRotate = screen.cam_user_rotate();
                if (!CameraPushRequested) {
                    bVector3 orbit(camera.cam_orbit_vertical(), camera.cam_orbit_horizontal(), camera.cam_orbit_radius());
                    bVector3 lookat(camera.cam_lookat_x(), camera.cam_lookat_y(), camera.cam_lookat_z());
                    pCameraMover->SetDesiredOrientation(orbit, camera.cam_roll_angle(), camera.cam_fov(), lookat, camera.cam_anim_speed(),
                                                        camera.cam_damping(), camera.cam_periods());
                }
            } else {
                if (HaveAttributesChanged(camera)) {
                    bVector3 orbit(camera.cam_orbit_vertical(), camera.cam_orbit_horizontal(), camera.cam_orbit_radius());
                    bVector3 lookat(camera.cam_lookat_x(), camera.cam_lookat_y(), camera.cam_lookat_z());
                    pCameraMover->SetCurrentOrientation(orbit, camera.cam_roll_angle(), camera.cam_fov(), lookat);
                }
            }
        }
    }
after_camera:
    if (eViews[1].ScreenEffects) {
        eViews[1].ScreenEffects->AddScreenEffect(SE_FE_BLUR, cam_blur, 0.0f, 0.0f, 0.0f);
    }
    UpdateRenderingCarParameters(RenderingCar);
    RefreshBackground();
}

void GarageMainScreen::SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason) {
    TheGarageCarLoader->LoadRideInfo(ride);
    CarState = 0;
    RideInfo *current = TheGarageCarLoader->GetCurrentRideInfo();
    if (current) {
        RideInfo *current2 = TheGarageCarLoader->GetCurrentRideInfo();
        if (current2->Type != ride->Type) {
            DisableCarRendering();
            cFEng::Get()->QueuePackageMessage(0xa05a328e, nullptr, nullptr);
        }
    }
    char sztemp[32];
    FEngSNPrintf(sztemp, 32, "CAR_NAME_%s", GetCarTypeInfo(ride->Type)->CarTypeName);
    FEngSetLanguageHash(pCarName, FEHashUpper(sztemp));
}

void GarageMainScreen::CancelCarLoad() {
    CarState = 1;
    TheGarageCarLoader->CancelCarLoad();
}

void GarageMainScreen::UpdateCurrentCameraView(bool bForce) {
    if (CameraPushRequested || bForce) {
        unsigned int entryKey = FindGarageEntryCameraInfo();
        Attrib::Gen::frontend entry(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), entryKey), 0, nullptr);
        if (!entry.GetLayoutPointer()) {
            entry.SetDefaultLayout(sizeof(Attrib::Gen::frontend::_LayoutStruct));
        }
        bVector3 orbit(entry.cam_orbit_vertical(), entry.cam_orbit_horizontal(), entry.cam_orbit_radius());
        bVector3 lookat(entry.cam_lookat_x(), entry.cam_lookat_y(), entry.cam_lookat_z());
        pCameraMover->SetCurrentOrientation(orbit, entry.cam_roll_angle(), entry.cam_fov(), lookat);

        unsigned int finalKey = FindGarageFinalCameraInfo();
        Attrib::Gen::frontend final_cam(Attrib::FindCollection(Attrib::Gen::frontend::ClassKey(), finalKey), 0, nullptr);
        if (!final_cam.GetLayoutPointer()) {
            final_cam.SetDefaultLayout(sizeof(Attrib::Gen::frontend::_LayoutStruct));
        }
        bVector3 orbit2(final_cam.cam_orbit_vertical(), final_cam.cam_orbit_horizontal(), final_cam.cam_orbit_radius());
        bVector3 lookat2(final_cam.cam_lookat_x(), final_cam.cam_lookat_y(), final_cam.cam_lookat_z());
        pCameraMover->SetDesiredOrientation(orbit2, final_cam.cam_roll_angle(), final_cam.cam_fov(), lookat2, final_cam.cam_anim_speed(),
                                            final_cam.cam_damping(), final_cam.cam_periods());

        CameraPushRequested = false;
    }
}

void GarageMainScreen::RefreshBackground() {
    const char *garageName = FEManager::Get()->GetGarageNameFromType();
    ResourceFile *bg = FEManager::Get()->GetGarageBackground();
    char name[128];
    bStrCpy(name, bg->GetFilename());
    char *dot = bStrIStr(name, ".");
    bStrCpy(dot, ".BIN");
    if (!bg || bStrCmp(name, garageName) != 0) {
        new EFadeScreenOn(false);
        eRemoveFEEnvMapPlat();
        eInitFEEnvMapPlat();
        UnloadResourceFile(bg);
        GameFlowLoadGarageScreen(BackgroundLoaded, 0);
    }
}

void GarageMainScreen::BackgroundLoaded(int param) {
    GarageMainScreen *inst = GetInstance();
    if (inst) {
        new EFadeScreenOff(0x14035fb);
        inst->mGeometryModels.UnInit();
        inst->mGeometryModels.Init("BACKDROP");
        inst->UpdateCurrentCameraView(true);
        inst->pCameraMover->Update(0.0f);
    }
}

float GarageMainScreen::GetCarRotationX() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAR_LOT:
            return -0.3796229958534241f;
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
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAR_LOT:
            return -0.00019299999985378236f;
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
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAR_LOT:
            return 340.0f;
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 304.96978759765625f;
        case GARAGETYPE_CAREER_SAFEHOUSE:
            return 304.96978759765625f;
        case GARAGETYPE_CUSTOMIZATION_SHOP:
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
        case GARAGETYPE_NONE:
        case GARAGETYPE_MAIN_FE:
        default:
            return 134.41250610351562f;
    }
}

float GarageMainScreen::GetGeometryXPos() {
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
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
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
        case GARAGETYPE_CAR_LOT:
            return 0.07500000298023224f;
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
    eGarageType type = FEManager::Get()->GetGarageType();
    switch (type) {
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
    if (reinterpret_cast<int>(fe_car) == -8 || fe_car->GetRideInfo()->Type == static_cast<CarType>(-1) || HideEntireScreen) {
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

    average_wheel_z *= 0.25f;
    fe_car->LightsOn = 0;
    float height = average_wheel_radius * 0.25f - average_wheel_z + (-0.025f);
    bVector3 position(carPosX, carPosY, height);
    fe_car->CopLightsOn = 0;

    bMatrix4 temp;
    eIdentity(&temp);
    eRotateZ(&temp, &temp, static_cast<int>(GetGeometryZAngle() * 65536.0f) / 360 & 0xffff);
    eMulVector(&position, &temp, &position);
    fe_car->SetPosition(&position);

    bMatrix4 body_matrix;
    eIdentity(&body_matrix);
    eRotateZ(&body_matrix, &body_matrix, static_cast<int>(GetCarRotationZ() * 65536.0f) / 360 & 0xffff);
    eRotateX(&body_matrix, &body_matrix, static_cast<int>(GetCarRotationX() * 65536.0f) / 360 & 0xffff);
    eRotateY(&body_matrix, &body_matrix, static_cast<int>(GetCarRotationY() * 65536.0f) / 360 & 0xffff);
    fe_car->SetBodyMatrix(&body_matrix);

    bMatrix4 tire_matrices[4];
    bMatrix4 brake_matrices[4];
    unsigned short front_tire_angle = static_cast<int>(CarSelectTireSteerAngle * 65536.0f) / 360 & 0xffff;

    for (int tire_num = 0; tire_num < 4; tire_num++) {
        eIdentity(&tire_matrices[tire_num]);
        eIdentity(&brake_matrices[tire_num]);
        if (tire_num < 2) {
            eRotateZ(&brake_matrices[tire_num], &brake_matrices[tire_num], front_tire_angle);
            eRotateZ(&tire_matrices[tire_num], &tire_matrices[tire_num], front_tire_angle);
        }
        tire_matrices[tire_num].v3 = wheel_positions[tire_num];
        brake_matrices[tire_num].v3 = wheel_positions[tire_num];
    }

    fe_car->SetTireMatrices(tire_matrices);
    fe_car->SetBrakeMatrices(brake_matrices);

    if (g_pEAXSound->GetFrontEnd()) {
        RideInfo *CurrentRideInfo = TheGarageCarLoader->GetCurrentRideInfo();
        if (CurrentRideInfo) {
            bVector3 car_velocity(0.0f, 0.0f, 0.0f);
            Camera *camera = eViews[0].GetCamera();
            g_pEAXSound->GetFrontEnd()->SetFEDrivingCarState(&position, &car_velocity, camera, ViewID);
        }
    }
}

void GarageMainScreen::HandleRender(unsigned int render_flags) {
    if (HideEntireScreen == 0) {
        eView *view = &eViews[ViewID];
        bMatrix4 *local = reinterpret_cast<bMatrix4 *>(CurrentBufferPos);
        if (CurrentBufferPos + 0x40 >= CurrentBufferEnd) {
            FrameMallocFailed = 1;
            FrameMallocFailAmount += 0x40;
            local = nullptr;
        } else {
            CurrentBufferPos += 0x40;
        }
        if (local) {
            eIdentity(local);
            float angle = GetGeometryZAngle();
            eRotateZ(local, local, static_cast<int>(angle * 65536.0f) / 360 & 0xffff);
            local->v3.x = GetGeometryXPos();
            local->v3.y = GetGeometryYPos();
            local->v3.z = GetGeometryZPos();
            mGeometryModels.Render(view, local, render_flags);
        }
        gEmitterSystem.Update(RealTimeElapsed);
    }
}

void GarageMainScreen::HandleShowPackage(unsigned int msg) {
    RenderingCar->Visible = 1;
    if (!(FEDatabase->GetGameMode() & 4)) {
        UpdateCurrentCameraView(true);
        pCameraMover->Update(0.0f);
        CameraPushRequested = true;
    }
}

void GarageMainScreen::HandleHidePackage(unsigned int msg) {
    RenderingCar->Visible = 0;
}

void GarageMainScreen::HandleJoyEvents() {
    static float zoomIn = 0.0f;
    static float zoomOut = 0.0f;
    int startPort = 0;
    int endPort = 2;
    bool isQR = false;
    if (FEDatabase->GetGameMode() & 4) {
        isQR = FEDatabase->iNumPlayers == 2;
    }
    if (isQR) {
        FEPackage *ctrl = cFEng::Get()->FindPackageWithControl();
        if (ctrl) {
            startPort = FEngMapJoyParamToJoyport(ctrl->GetControlMask());
            endPort = startPort + 1;
        }
    }
    for (int port = startPort; port < endPort; port++) {
        if (!mActionQ[port])
            continue;
        while (!mActionQ[port]->IsEmpty()) {
            if (bUserRotate || CarGuysCamera != 0) {
                ActionRef action = mActionQ[port]->GetAction();
                float dVar7;
                if (!mActionQ[port]->IsConnected()) {
                    dVar7 = 0.0f;
                } else if (action.ID() == 0) {
                    dVar7 = 0.0f;
                } else {
                    dVar7 = action.Data();
                }
                int id = action.ID();
                if (id == 0x20) {
                    mOrbitH = -dVar7;
                    pCameraMover->SetHRotateSpeed(-dVar7);
                    sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                } else if (id < 0x21) {
                    if (id == 0x1e) {
                        mOrbitV = -dVar7;
                        pCameraMover->SetVRotateSpeed(-dVar7);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                    } else if (id == 0x1d) {
                        mOrbitV = dVar7;
                        pCameraMover->SetVRotateSpeed(dVar7);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                    } else if (id == 0x1f) {
                        mOrbitH = dVar7;
                        pCameraMover->SetHRotateSpeed(dVar7);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                    }
                } else if (id > 0x2a) {
                    if (id < 0x2d) {
                        if (id == 0x2b) {
                            zoomOut = dVar7;
                        } else {
                            zoomIn = -dVar7;
                        }
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                        if (abs(zoomIn) > abs(zoomOut)) {
                            mZoom = zoomIn;
                        } else if (abs(zoomOut) > abs(zoomIn)) {
                            mZoom = zoomOut;
                        } else {
                            if (zoomOut == 0.0f && zoomIn == 0.0f) {
                                mZoom = 0.0f;
                            }
                        }
                        pCameraMover->SetZoomSpeed(mZoom);
                    } else if (id == 0x88) {
                        pCameraMover->SetVRotateSpeed(0.0f);
                        pCameraMover->SetHRotateSpeed(0.0f);
                        pCameraMover->SetZoomSpeed(0.0f);
                        sNumTicksSinceUserMovedCamera = sNumTicksBeforeCamMovesBackToScreenPosition;
                    }
                }
                if (sNumTicksSinceUserMovedCamera > 0) {
                    if (bAutoMovement) {
                        if (action.ID() != 0x1f && action.ID() != 0x20) {
                            pCameraMover->SetHRotateSpeed(0.0f);
                        }
                    }
                    bAutoMovement = 0;
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
    return new GarageMainScreen(sd, 1, &TopOrFullScreenRide, 0);
}

GarageCarLoader::GarageCarLoader() {
    LoadingRideInfo.Init(CARTYPE_NONE, CarRenderUsage_Player, 0, 0);
    CurrentRideInfo.Init(CARTYPE_NONE, CarRenderUsage_Player, 0, 0);
    IsLoadingRide = false;
    IsCurrentRide = false;
    LoadingCar = 0;
    CurrentCar = 0;
    IsDifferent = false;
    UseFirstDummyTexturesForNextLoad = true;
}

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
    if (UseFirstDummyTexturesForNextLoad == 0) {
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
            TheCarLoader.Unload(CurrentCar);
        }
        IsCurrentRide = true;
        CurrentCar = LoadingCar;
        CurrentRideInfo = LoadingRideInfo;
        IsDifferent = true;
        LoadingCar = 0;
        IsLoadingRide = false;
        UseFirstDummyTexturesForNextLoad = (UseFirstDummyTexturesForNextLoad != 1);
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
    cFEng *eng = cFEng::Get();
    const char *name = "GarageMain.fng";
    if (eng->IsPackagePushed(name)) {
        return static_cast<GarageMainScreen *>(FEngFindScreen(name));
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
    FindWhichScreenToUpdate(which_car)->CancelCarLoad();
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
        cFEng::Get()->PushNoControlPackage("GarageMain.fng", static_cast<FE_PACKAGE_PRIORITY>(100));
    }
}

bool CarViewer::haveLoadedOnce;
