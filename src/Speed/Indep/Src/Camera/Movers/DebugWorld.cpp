#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/World/Track.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"

extern float DebugCameraNearPlane;
extern bVector3 JumpToPosition;
extern bool gDebugCameraSetEye;
extern bool gDebugCameraSetLook;
extern bVector3 gDebugCameraTweakableEye;
extern bVector3 gDebugCameraTweakableLook;
extern tGraph<float> gDebugCameraInputGraph;

unsigned short bFixATan(int x);
unsigned short bFixATan(int x, int y);

bVector3 DebugWorldCameraMover::Eye;
bVector3 DebugWorldCameraMover::Look;
bVector3 DebugWorldCameraMover::Up;
float DebugWorldCameraMover::TurboSpeed = 3.06f;
float DebugWorldCameraMover::SuperTurboSpeed = 7.16f;
int DebugWorldCameraMover::TurboOn;
int DebugWorldCameraMover::SuperTurboOn;

DebugWorldCameraMover::DebugWorldCameraMover(int view_id, const bVector3 *start_position,
                                             const bVector3 *start_direction, JoystickPort jp)
    : CameraMover(view_id, CM_DEBUG_WORLD) {
    JoyPort = jp;
    TurnVInc = 0;
    TurnHInc = 0;
    StrafeInc = 0.0f;
    HeightInc = 0.0f;
    ForwardInc = 0.0f;
    ForwardAnalogInc = 0.0f;
    SuperTurboOn = 0;
    Eye = *start_position;
    TurboOn = 0;
    Look = *start_direction;
    PrevNearZ = GetCamera()->GetNearZ();
    GetCamera()->SetNearZ(DebugCameraNearPlane);
    mActionQ = new ActionQueue(JoyPort, 0x98c7a2f5, "DebugWorld", false);
}

DebugWorldCameraMover::~DebugWorldCameraMover() {
    if (mActionQ) {
        GetCamera()->SetNearZ(PrevNearZ);
        if (mActionQ) {
            delete mActionQ;
        }
        mActionQ = nullptr;
    }
}

void DebugWorldCameraMover::JoyHandler() {
    if (!mActionQ) {
        return;
    }

    while (!mActionQ->IsEmpty()) {
        ActionRef aRef = mActionQ->GetAction();
        float data = gDebugCameraInputGraph.GetValue(aRef.Data());

        switch (aRef.ID()) {
        case 0x35:
            HeightInc = data * 10.0f;
            break;
        case 0x36:
            HeightInc = data * -10.0f;
            break;
        case 0x37:
        case 0x3b:
            data = -data;
        case 0x38:
        case 0x3c:
            StrafeInc = data * -20.0f;
            break;
        case 0x3a:
            data = -data;
        case 0x39:
            ForwardAnalogInc = data * 20.0f;
            break;
        case 0x3e:
            data = -data;
        case 0x3d:
            ForwardInc = -data * 20.0f;
            break;
        case 0x40:
        case 0x44:
            data = -data;
        case 0x3f:
        case 0x43:
            TurnVInc = static_cast<short>(static_cast<int>(-data * 20000.0f));
            break;
        case 0x41:
        case 0x45:
            data = -data;
        case 0x42:
        case 0x46:
            TurnHInc = static_cast<short>(static_cast<int>(data * -20000.0f));
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
            eUnSwizzleWorldVector(*GetPosition(), simpos);
            {
                IPlayer *player = IPlayer::First(PLAYER_LOCAL);
                if (player) {
                    ISimable *sim = player->GetSimable();
                    if (sim) {
                        WCollisionMgr(0, 3).GetWorldHeightAtPoint(
                            reinterpret_cast<const UMath::Vector3 &>(simpos), simpos.y, nullptr);
                        simpos.y += 3.0f;
                        sim->GetRigidBody()->SetPosition(reinterpret_cast<const UMath::Vector3 &>(simpos));
                    }
                }
            }
            break;
        }
        default:
            break;
        }

        mActionQ->PopAction();
    }
}

void DebugWorldCameraMover::Update(float dT) {
    if (JumpToPosition.y != 0.0f) {
        TopologyCoordinate topology_coordinate;
        topology_coordinate.SetInterestBBox(&Eye, 0.0f, &Look);
        topology_coordinate.IsLoaded();
        JumpToPosition.z += 100.0f;
        bVector3 dir = Eye - Look;
        bNormalize(&dir, &dir, 1.0f);
        bVector3 newEye = JumpToPosition + dir;
        Eye = newEye;
        Look = JumpToPosition;
        bFill(&JumpToPosition, 0.0f, 0.0f, 0.0f);
        bRefreshTweaker();
    }

    if (gDebugCameraSetEye) {
        eSwizzleWorldVector(gDebugCameraTweakableEye, Eye);
        gDebugCameraSetEye = false;
    }

    if (gDebugCameraSetLook) {
        eSwizzleWorldVector(gDebugCameraTweakableLook, Look);
        gDebugCameraSetLook = false;
    }

    JoyHandler();

    bVector3 eyelook = Look - Eye;
    unsigned short hAngle = bFixATan(static_cast<int>(eyelook.x * 65536.0f),
                                     static_cast<int>(eyelook.y * 65536.0f));

    if (TurnHInc != 0 || TurnVInc != 0 || HeightInc != 0.0f) {
        hAngle = (hAngle + static_cast<int>(static_cast<float>(TurnHInc) * dT)) & 0xffff;
        bVector2 *horiz = reinterpret_cast<bVector2 *>(&eyelook);
        float xylen = bLength(horiz);

        unsigned short pitch = bFixATan(static_cast<int>(xylen * 65536.0f),
                                        static_cast<int>(eyelook.z * 65536.0f));
        pitch = (pitch + static_cast<int>(static_cast<float>(TurnVInc) * dT)) & 0xffff;

        if (static_cast<unsigned int>(pitch - 0x3ff7) < 0x4009u) {
            pitch = 0x3ff6;
        }
        if ((static_cast<unsigned int>(pitch - 0x8000) & 0xffffu) < 0x400au) {
            pitch = 0xc00a;
        }

        float fi;
        if (SuperTurboOn != 0) {
            fi = HeightInc * SuperTurboSpeed;
        } else if (TurboOn != 0) {
            fi = HeightInc * TurboSpeed;
        } else {
            fi = HeightInc;
        }

        float hi = fi * dT;
        Eye = Eye + bVector3(0.0f, 0.0f, hi);

        Look.x = bCos(pitch) * (bCos(hAngle) * 100.0f) + Eye.x;
        Look.y = bCos(pitch) * (bSin(hAngle) * 100.0f) + Eye.y;
        Look.z = bSin(pitch) * 100.0f + Eye.z;
    }

    if (ForwardInc != 0.0f) {
        float fi;
        if (SuperTurboOn) {
            fi = ForwardInc * SuperTurboSpeed * dT;
        } else if (TurboOn) {
            fi = ForwardInc * TurboSpeed * dT;
        } else {
            fi = ForwardInc * dT;
        }
        bVector3 forward = *GetDirection() * fi;
        Eye += forward;
        Look += forward;
    } else if (ForwardAnalogInc != 0.0f) {
        float fi;
        if (SuperTurboOn != 0) {
            fi = ForwardAnalogInc * SuperTurboSpeed;
        } else if (TurboOn != 0) {
            fi = ForwardAnalogInc * TurboSpeed;
        } else {
            fi = ForwardAnalogInc;
        }
        fi *= dT;
        bVector3 forward = *GetDirection() * fi;
        Eye += forward;
        Look += forward;
    }

    if (StrafeInc != 0.0f) {
        float si;
        if (SuperTurboOn != 0) {
            si = StrafeInc * SuperTurboSpeed;
        } else if (TurboOn != 0) {
            si = StrafeInc * TurboSpeed;
        } else {
            si = StrafeInc;
        }
        si *= dT;
        unsigned short sAngle = (hAngle + 0x4000) & 0xffff;
        float cval = bCos(sAngle) * si;
        float sval = bSin(sAngle) * si;
        bVector3 rl(cval, sval, 0.0f);
        Eye += rl;
        Look += rl;
    }

    bMatrix4 m;
    bVector3 up;
    unsigned short bank = 0;
    ComputeBankedUpVector(&up, &Eye, &Look, bank);
    eCreateLookAtMatrix(&m, Eye, Look, up);

    unsigned short fov = 0x32dc;
    GetCamera()->SetFieldOfView(fov);
    GetCamera()->SetCameraMatrix(m, dT);
}
