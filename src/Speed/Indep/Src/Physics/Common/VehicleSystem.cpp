#include "VehicleSystem.h"
#include "Speed/Indep/Src/Main/stubs.h"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"

namespace VehicleSystem {

float ENABLE_ROLL_STOPS_THRESHOLD = 0.2f;
float PAD_DEAD_ZONE = 0.05f;

static void InitializeVehicleGlobals() {
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0, 0, 0, 0, 0, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0, 0, 0, 0, 0, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0, 0, 0, 0, 0, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
    dbattrib(0.0f, 0.0f, 0, 0.0f, 0.0f, 0);
}

static void InitializeGlobals() {}

void Init() {
    InitializeGlobals();
    InitializeVehicleGlobals();
}

void Shutdown() {}

}; // namespace VehicleSystem

BIND_SIM_SUBSYSTEM(VehicleSystem, VehicleSystem::Init, VehicleSystem::Shutdown)
