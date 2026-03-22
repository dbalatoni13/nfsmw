#include "VehicleSystem.h"

#include "Speed/Indep/Src/Main/stubs.h"
#include "Speed/Indep/Src/Physics/Behaviors/ResetCar.h"
#include "Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.h"
#include "Speed/Indep/Src/Physics/Behaviors/SpikeStrip.h"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"

class SuspensionRacer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SuspensionTraffic {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SuspensionSimple {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SuspensionTrailer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SuspensionSpline {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class EngineRacer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class EngineDragster {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class EngineSpline {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SimpleChopper {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class EngineTraffic {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DrawHeli {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DrawTraffic {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DrawNISCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DrawCopCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DrawRaceCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SoundTraffic {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SoundCop {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SoundRacer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class SoundHeli {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
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

static void Init() {
    InitializeGlobals();
    InitializeVehicleGlobals();
}

static void Shutdown() {}

}; // namespace VehicleSystem

static Sim::SubSystem _Physics_System_VehicleSystem("VehicleSystem", VehicleSystem::Init, VehicleSystem::Shutdown);

UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SuspensionRacer("SuspensionRacer", SuspensionRacer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SuspensionTraffic("SuspensionTraffic", SuspensionTraffic::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SuspensionSimple("SuspensionSimple", SuspensionSimple::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SuspensionTrailer("SuspensionTrailer", SuspensionTrailer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SuspensionSpline("SuspensionSpline", SuspensionSpline::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EngineRacer("EngineRacer", EngineRacer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EngineDragster("EngineDragster", EngineDragster::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EngineSpline("EngineSpline", EngineSpline::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SimpleChopper("SimpleChopper", SimpleChopper::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EngineTraffic("EngineTraffic", EngineTraffic::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DrawHeli("DrawHeli", DrawHeli::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DrawTraffic("DrawTraffic", DrawTraffic::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DrawNISCar("DrawNISCar", DrawNISCar::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DrawCopCar("DrawCopCar", DrawCopCar::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DrawRaceCar("DrawRaceCar", DrawRaceCar::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SoundTraffic("SoundTraffic", SoundTraffic::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SoundCop("SoundCop", SoundCop::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SoundRacer("SoundRacer", SoundRacer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __ResetCar("ResetCar", ResetCar::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SoundHeli("SoundHeli", SoundHeli::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __SpikeStrip("SpikeStrip", SpikeStrip::Construct);
