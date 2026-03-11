#include "VehicleSystem.h"

#include "Speed/Indep/Src/Sim/SimSubSystem.h"

namespace VehicleSystem {

float ENABLE_ROLL_STOPS_THRESHOLD = 0.2f;
float PAD_DEAD_ZONE = 0.05f;

static void InitializeVehicleGlobals() {}
static void InitializeGlobals() {}

static void Init() {
    InitializeGlobals();
    InitializeVehicleGlobals();
}

static void Shutdown() {}

}; // namespace VehicleSystem

static Sim::SubSystem _Physics_System_VehicleSystem("VehicleSystem", VehicleSystem::Init, VehicleSystem::Shutdown);
