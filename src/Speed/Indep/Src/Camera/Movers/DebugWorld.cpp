#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/DebugWorld.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/World/Track.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

extern bAngle bFixATan(int x, int y);



// Orden de .bss/.data del original (config/GOWE69/symbols.txt 0x8045AED0.. / 0x80416FF8..).
bVector3 gDebugCameraTweakableEye(0.0f, 0.0f, 0.0f); // .bss: 0x8045AED0
bVector3 gDebugCameraTweakableLook(0.0f, 0.0f, 0.0f); // .bss: 0x8045AEE0
int gDebugCameraSetEye = 0;                          // .data: 0x80416FF8
int gDebugCameraSetLook = 0;                         // .data: 0x80416FFC

bVector3 DebugWorldCameraMover::Eye(20.0f, 20.0f, 100.0f); // .bss: 0x8045AEF0
bVector3 DebugWorldCameraMover::Look(0.0f, 0.0f, 0.0f);    // .bss: 0x8045AF00
bVector3 DebugWorldCameraMover::Up(0.0f, 0.0f, 1.0f);      // .bss: 0x8045AF10
float DebugWorldCameraMover::TurboSpeed = 3.06f;
float DebugWorldCameraMover::SuperTurboSpeed = 7.16f;
int DebugWorldCameraMover::TurboOn = 0;
int DebugWorldCameraMover::SuperTurboOn = 0;

bVector3 JumpToPosition(0.0f, 0.0f, 0.0f); // .bss: 0x8045AF20
bVector3 spline_points[2] = {bVector3(0.0f, 0.0f, 0.0f), bVector3(1000.0f, 0.0f, 0.0f)}; // .bss: 0x8045AF30

GraphEntry<float> fDebugCameraInputData[5] = {
    {0.0f, 0.0f},
    {0.3f, 0.03f},
    {0.6f, 0.15f},
    {0.9f, 0.4f},
    {1.0f, 1.0f},
};
tGraph<float> gDebugCameraInputGraph(fDebugCameraInputData, 5);

extern float DebugCameraNearPlane;

DebugWorldCameraMover::DebugWorldCameraMover(int view_id, const bVector3 *start_position, const bVector3 *start_direction,
                                             JoystickPort jp)
    : CameraMover(view_id, CM_DEBUG_WORLD) {
    this->JoyPort = jp;
    this->HeightInc = 0.0f;
    this->ForwardInc = 0.0f;
    this->ForwardAnalogInc = 0.0f;
    this->StrafeInc = 0.0f;
    this->TurnHInc = 0;
    this->TurnVInc = 0;
    TurboOn = 0;
    SuperTurboOn = 0;
    Eye = *start_position;
    Look = *start_direction;

    this->PrevNearZ = this->pCamera->GetNearZ();
    this->pCamera->SetNearZ(DebugCameraNearPlane);

    this->mActionQ = new ActionQueue(this->JoyPort, 0x98C7A2F5, "DebugWorld", false);
}

DebugWorldCameraMover::~DebugWorldCameraMover() {
    if (this->mActionQ != 0) {
        this->pCamera->SetNearZ(this->PrevNearZ);
        delete this->mActionQ;
        this->mActionQ = 0;
    }
}

void DebugWorldCameraMover::JoyHandler() {
    if (this->mActionQ == 0) {
        return;
    }

    while (!this->mActionQ->IsEmpty()) {
        ActionRef aRef = this->mActionQ->GetAction();
        float data = gDebugCameraInputGraph.GetValue(aRef.Data());

        switch (aRef.ID()) {
        case 0x35:
            this->HeightInc = data * 10.0f;
            break;
        case 0x36:
            this->HeightInc = data * -10.0f;
            break;
        // Los cuerpos del switch del original ENCADENAN por caida: el caso
        // negado solo hace `data = -data;` y cae en el que multiplica
        // (zCamera.s .L_800775C4/.L_800775C8, .L_800775D4/.L_800775D8,
        // .L_800775E4/.L_800775E8, .L_800775F8/.L_800775FC,
        // .L_80077618/.L_8007761C). El orden de los cuerpos es el de aqui.
        // Ojo: 0x39/0x3A escriben ForwardAnalogInc (0x88) y 0x3D/0x3E
        // ForwardInc (0x84) -- estaban cambiados.
        case 0x37:
        case 0x3B:
            data = -data;
            // fallthrough
        case 0x38:
        case 0x3C:
            this->StrafeInc = data * -20.0f;
            break;
        case 0x3A:
            data = -data;
            // fallthrough
        case 0x39:
            this->ForwardAnalogInc = data * 20.0f;
            break;
        case 0x3E:
            data = -data;
            // fallthrough
        case 0x3D:
            data = -data;
            this->ForwardInc = data * 20.0f;
            break;
        case 0x40:
        case 0x44:
            data = -data;
            // fallthrough
        case 0x3F:
        case 0x43:
            data = -data;
            this->TurnVInc = (short)(data * 20000.0f);
            break;
        case 0x41:
        case 0x45:
            data = -data;
            // fallthrough
        case 0x42:
        case 0x46:
            this->TurnHInc = (short)(data * -20000.0f);
            break;
        case 0x47:
            if (data != 0.0f) {
                TurboOn = 1;
            } else {
                TurboOn = 0;
            }
            break;
        case 0x48:
            if (data != 0.0f) {
                SuperTurboOn = 1;
            } else {
                SuperTurboOn = 0;
            }
            break;
        case 0x49: {
            bVector3 simpos;
            eUnSwizzleWorldVector(*this->pCamera->GetPosition(), simpos);

            {
                IPlayer *player = IPlayer::First(PLAYER_LOCAL);
                if (player != 0) {
                    ISimable *sim = player->GetSimable();
                    if (sim != 0) {
                        WCollisionMgr(0, 3).GetWorldHeightAtPoint((const UMath::Vector3 &)simpos, simpos.y, (UMath::Vector3 *)0);
                        simpos.y += 3.0f;
                        sim->GetRigidBody()->SetPosition((const UMath::Vector3 &)simpos);
                    }
                }
            }
            break;
        }
        }

        this->mActionQ->PopAction();
    }
}

void DebugWorldCameraMover::Update(float dT) {
    if (JumpToPosition.y != 0.0f) {
        TopologyCoordinate topology_coordinate;
        JumpToPosition.z += 3.0f;
        bVector3 dir(Eye - Look);
        bNormalize(&dir, &dir, 10.0f);
        Eye = JumpToPosition + dir;
        Look = JumpToPosition;
        bFill(&JumpToPosition, 0.0f, 0.0f, 0.0f);
        bRefreshTweaker();
    }

    bMatrix4 m;
    if (gDebugCameraSetEye) {
        eSwizzleWorldVector(gDebugCameraTweakableEye, Eye);
        gDebugCameraSetEye = 0;
    }
    if (gDebugCameraSetLook) {
        eSwizzleWorldVector(gDebugCameraTweakableLook, Look);
        gDebugCameraSetLook = 0;
    }

    JoyHandler();

    bVector3 eyelook(Look - Eye);
    unsigned short hAngle = bFixATan(static_cast<int>(eyelook.x * 65536.0f), static_cast<int>(eyelook.y * 65536.0f));

    if (*reinterpret_cast<int *>(&this->TurnHInc) != 0 || this->HeightInc != 0.0f) {
        hAngle += static_cast<unsigned short>(static_cast<float>(this->TurnHInc) * dT);

        bVector2 *horiz = reinterpret_cast<bVector2 *>(&eyelook);
        float xylen = bLength(horiz);
        unsigned short pitch = bFixATan(static_cast<int>(xylen * 65536.0f), static_cast<int>(eyelook.z * 65536.0f));
        pitch += static_cast<unsigned short>(static_cast<float>(this->TurnVInc) * dT);

        if (static_cast<unsigned short>(pitch - 0x3ff7) <= 0x4008) {
            pitch = 0x3ff6;
        }
        if (static_cast<unsigned short>(pitch + 0x8000) <= 0x4009) {
            pitch = 0xc00a;
        }

        float hi;
        if (SuperTurboOn) {
            hi = this->HeightInc * SuperTurboSpeed * dT;
        } else if (TurboOn) {
            hi = this->HeightInc * TurboSpeed * dT;
        } else {
            hi = this->HeightInc * dT;
        }
        Eye = Eye + bVector3(0.0f, 0.0f, hi);

        Look.x = Eye.x + bCos(pitch) * (bCos(hAngle) * 20.0f);
        Look.y = Eye.y + bCos(pitch) * (bSin(hAngle) * 20.0f);
        Look.z = Eye.z + bSin(pitch) * 20.0f;
    }

    if (this->ForwardInc != 0.0f) {
        float fi;
        if (SuperTurboOn) {
            fi = this->ForwardInc * SuperTurboSpeed * dT;
        } else if (TurboOn) {
            fi = this->ForwardInc * TurboSpeed * dT;
        } else {
            fi = this->ForwardInc * dT;
        }
        bVector3 forward = *pCamera->GetDirection() * fi;
        forward.z = 0.0f;
        Eye += forward;
        Look += forward;
    } else if (this->ForwardAnalogInc != 0.0f) {
        float fi;
        if (SuperTurboOn) {
            fi = this->ForwardAnalogInc * SuperTurboSpeed * dT;
        } else if (TurboOn) {
            fi = this->ForwardAnalogInc * TurboSpeed * dT;
        } else {
            fi = this->ForwardAnalogInc * dT;
        }
        bVector3 forward = *pCamera->GetDirection() * fi;
        Eye += forward;
        Look += forward;
    }

    if (this->StrafeInc != 0.0f) {
        float si;
        if (SuperTurboOn) {
            si = this->StrafeInc * SuperTurboSpeed * dT;
        } else if (TurboOn) {
            si = this->StrafeInc * TurboSpeed * dT;
        } else {
            si = this->StrafeInc * dT;
        }
        bVector3 rl(bCos(static_cast<unsigned short>(hAngle + 0x4000)) * si, bSin(static_cast<unsigned short>(hAngle + 0x4000)) * si, 0.0f);
        Eye += rl;
        Look += rl;
    }

    bVector3 up;
    ComputeBankedUpVector(&up, &Eye, &Look, 0);
    eCreateLookAtMatrix(&m, Eye, Look, up);
    Camera *cam = pCamera;
    if (!Camera::StopUpdating) {
        cam->SetFieldOfView(0x32dc);
    }
    pCamera->SetCameraMatrix(m, dT);
}
