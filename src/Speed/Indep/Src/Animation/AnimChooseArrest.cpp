#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiQRCarSelect.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Messages/MPerpBusted.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/Hermes.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

float g_TriggerMomentNISTime = 0;
bool Tweak_TriggerMomentAlways = 0;

int lbl_80415678[2] = { 0, 0x3DA3D70A };

void ChooseArrestAnimation(int *cameraTrack, char *sceneName, int strLen) {
    IVehicle *playerVehicle = IVehicle::First(VEHICLE_PLAYERS);
    int heatLevel;
    heatLevel = 0;
    if (playerVehicle) {
        IPerpetrator *player;
        if (playerVehicle->QueryInterface(&player)) {
            heatLevel = static_cast<int>(player->GetHeat());
        }
    }

    float fVar = bRandom(0.99f);
    int randPick = static_cast<int>(fVar * 8.0f);

    switch (heatLevel) {
    case 0:
    case 1:
        switch (randPick) {
        case 0:
            bStrNCpy(sceneName, "ArrestM01", strLen);
            break;
        case 1:
            bStrNCpy(sceneName, "ArrestM16", strLen);
            break;
        case 2:
            bStrNCpy(sceneName, "ArrestF02", strLen);
            break;
        case 3:
            bStrNCpy(sceneName, "ArrestF18", strLen);
            break;
        case 4:
            bStrNCpy(sceneName, "ArrestM01b", strLen);
            break;
        case 5:
            bStrNCpy(sceneName, "ArrestM16b", strLen);
            break;
        case 6:
            bStrNCpy(sceneName, "ArrestF02b", strLen);
            break;
        default:
            bStrNCpy(sceneName, "ArrestF18b", strLen);
            break;
        }
        goto done;
    case 2:
        switch (randPick) {
        case 0:
        case 1:
            bStrNCpy(sceneName, "ArrestM04", strLen);
            break;
        case 2:
        case 3:
            bStrNCpy(sceneName, "ArrestF23", strLen);
            break;
        case 4:
        case 5:
            bStrNCpy(sceneName, "ArrestM04b", strLen);
            break;
        case 6:
        default:
            bStrNCpy(sceneName, "ArrestF23b", strLen);
            break;
        }
        break;
    case 3:
        break;
    default:
        goto third_section;
    }

    switch (randPick) {
    case 0:
        bStrNCpy(sceneName, "ArrestM07", strLen);
        break;
    case 1:
        bStrNCpy(sceneName, "ArrestM14", strLen);
        break;
    case 2:
    case 3:
        bStrNCpy(sceneName, "ArrestF14", strLen);
        break;
    case 4:
        bStrNCpy(sceneName, "ArrestM07b", strLen);
        break;
    case 5:
        bStrNCpy(sceneName, "ArrestM14b", strLen);
        break;
    case 6:
    default:
        bStrNCpy(sceneName, "ArrestF14b", strLen);
        break;
    }

third_section:
    switch (randPick) {
    case 0:
        bStrNCpy(sceneName, "ArrestM06", strLen);
        break;
    case 1:
        bStrNCpy(sceneName, "ArrestM19", strLen);
        break;
    case 2:
        bStrNCpy(sceneName, "ArrestF06", strLen);
        break;
    case 3:
        bStrNCpy(sceneName, "ArrestF07", strLen);
        break;
    case 4:
        bStrNCpy(sceneName, "ArrestM06b", strLen);
        break;
    case 5:
        bStrNCpy(sceneName, "ArrestM19b", strLen);
        break;
    case 6:
        bStrNCpy(sceneName, "ArrestF06b", strLen);
        break;
    default:
        bStrNCpy(sceneName, "ArrestF07b", strLen);
        break;
    }

done:
    TheICEManager.GetNumSceneCameraTrack(bStringHash(sceneName));
    *cameraTrack = 0;
}

bool ChooseArrestLocation(UMath::Vector3 &position, float &angle) {
    IVehicle *playerVehicle = IVehicle::First(VEHICLE_PLAYERS);
    ISimable *simable = playerVehicle->GetSimable();
    if (simable) {
        UMath::Matrix4 xform;
        simable->GetTransform(xform);
        UMath::Vector3 markerDir = UMath::Vector3::kZero;
        markerDir = UMath::Vector4To3(xform.v0);

        angle = (1.0f - UMath::Atan2a(markerDir.x, markerDir.z)) + 0.25f;
        position = UMath::Vector4To3(xform.v3);

        WRoadNav roadBlockNav;
        roadBlockNav.SetCookieTrail(true);
        roadBlockNav.SetPathType(WRoadNav::kPathCop);
        roadBlockNav.SetNavType(WRoadNav::kTypeDirection);
        roadBlockNav.InitAtPoint(position, markerDir, true, 0.0f);

        if (!roadBlockNav.IsValid()) {
            return false;
        }

        UMath::Vector3 rightPos = roadBlockNav.GetRightPosition();
        UMath::Vector3 leftPos = roadBlockNav.GetLeftPosition();
        UMath::Vector3 centreOfRoad = (UVector3(rightPos) + UVector3(leftPos)) * 0.5f;
        float road_width = UMath::Distance(leftPos, rightPos);

        position = centreOfRoad;
        return true;
    } else {
        return false;
    }
}

// TODO where to put this?
// total size: 0x58
struct NISListenerActivity : public Sim::Activity, public INISLISTENER {
    static IActivity *Construct(Sim::Param params) {
        return new NISListenerActivity();
    }

    NISListenerActivity();

    // Overrides: IUnknown
    ~NISListenerActivity() override;

    // Overrides: INISLISTENER
    void ArrestLevel(int level) override;

    void MessageBusted(const MPerpBusted &message);

    Hermes::HHANDLER mMessageBusted; // offset 0x54, size 0x4
};

static UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::Prototype
    _NISListenerActivity(UCrc32("NISActivity"), NISListenerActivity::Construct);

NISListenerActivity::NISListenerActivity() : Sim::Activity(1), INISLISTENER(this) {
    // TODO magic
    mMessageBusted =
        Hermes::Handler::Create<MPerpBusted, NISListenerActivity, NISListenerActivity>(this, &NISListenerActivity::MessageBusted, 0xfea34c0a, 0);
}

NISListenerActivity::~NISListenerActivity() {
    if (mMessageBusted) {
        Hermes::Handler::Destroy(mMessageBusted);
    }
}

void NISListenerActivity::ArrestLevel(int level) {}

void NISListenerActivity::MessageBusted(const MPerpBusted &message) {
    if (INIS::Get()) {
        return;
    }
    GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
    if (!parms) {
        QRCarSelectBustedManager::SetPlayerBusted();
    }
    int cameraTrack = 0;
    char sceneName[32];
    ChooseArrestAnimation(&cameraTrack, sceneName, sizeof(sceneName));

    IVehicle *playerVehicle = IVehicle::First(VEHICLE_PLAYERS);
    UMath::Vector3 markerPos;
    float markerAngle;
    if (ChooseArrestLocation(markerPos, markerAngle)) {
        Sim::IActivity *activity = UTL::COM::Factory<Sim::Param, Sim::IActivity, UCrc32>::CreateInstance("NISActivity", Sim::Param());
        INIS *nis;
        if (activity && activity->QueryInterface(&nis)) {
            nis->AddCar("car1", playerVehicle);
            nis->StartLocation(markerPos, markerAngle);
            nis->Load(CAnimChooser::Arrest, sceneName, cameraTrack, true);
        }
    } else {
        GRaceParameters *parms = GRaceStatus::Get().GetRaceParameters();
        if (!parms) {
            new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "Infractions.fng");
        } else {
            new EPause(0, 1, 0);
        }
    }
}
