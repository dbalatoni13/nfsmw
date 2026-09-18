#ifndef CAMERA_MOVERS_DEBUGWORLD_H
#define CAMERA_MOVERS_DEBUGWORLD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/Joystick.hpp"

// total size: 0xA8
class DebugWorldCameraMover : public CameraMover {
  public:
    DebugWorldCameraMover(int view_id, const bVector3 *start_position, const bVector3 *start_direction, JoystickPort jp);
    ~DebugWorldCameraMover() override;

    void JoyHandler();
    void Update(float dT) override;

    static unsigned short FOV;
    static bVector3 Eye;
    static bVector3 Look;
    static bVector3 Up;
    static int NeedsUpdateBeforeProceed;
    static float TurboSpeed;
    static float SuperTurboSpeed;
    static float SlowSpeed;
    static int TurboOn;
    static int SuperTurboOn;
    static int SlowOn;

    float HeightInc;         // offset 0x80, size 0x4
    float ForwardInc;        // offset 0x84, size 0x4
    float ForwardAnalogInc;  // offset 0x88, size 0x4
    float StrafeInc;         // offset 0x8C, size 0x4
    short TurnHInc;          // offset 0x90, size 0x2
    short TurnVInc;          // offset 0x92, size 0x2
    float RoadNetworkXInc;   // offset 0x94, size 0x4
    float RoadNetworkYInc;   // offset 0x98, size 0x4
    JoystickPort JoyPort;    // offset 0x9C, size 0x4
    float PrevNearZ;         // offset 0xA0, size 0x4
    ActionQueue *mActionQ;   // offset 0xA4, size 0x4
};
#endif
