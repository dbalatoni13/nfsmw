// RenderConn.cpp - top level render service entry points.
//
// These four functions are the render side pump called from the game loop
// (see GameFlow.cpp): they forward to the individual render connections and
// keep the global emitter system and animation player ticking.

#include "Speed/Indep/Src/Render/RenderConn.h"

#include "Speed/Indep/Src/Animation/AnimPlayer.hpp"

// Free hooks implemented by the render connections (SmackableRender.cpp,
// CarRender.cpp, EmitterSystem.cpp).
void SmackableRender_Init();
void SmackableRender_Shutdown();
void SmackableRender_Service(float dT);
void CarRender_Service(float dT);
void ClearXenonEmitters();

namespace RenderConn {

void InitServices() {
    SmackableRender_Init();
}

void RestoreServices() {
    SmackableRender_Shutdown();
    ClearXenonEmitters();
}

void UpdateLoading() {
    VehicleRenderConn::UpdateLoading();
}

void UpdateServices(float dT) {
    SmackableRender_Service(dT);
    CarRender_Service(dT);
    gEmitterSystem.Update(dT);
    TheAnimPlayer.UpdateTime(dT);
}

} // namespace RenderConn
