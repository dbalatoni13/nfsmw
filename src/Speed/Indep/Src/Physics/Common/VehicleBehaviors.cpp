#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"

#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

class RBVehicle {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class RBTractor {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class RBTrailer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class RBCop {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DamageRacer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DamageDragster {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DamageHeli {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class DamageCopCar {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class PInput {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class InputPlayer {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class InputPlayerDrag {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};
class InputNIS {
  public:
    static Behavior *Construct(const BehaviorParams &params);
};

UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __RBVehicle("RBVehicle", RBVehicle::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __RBTractor("RBTractor", RBTractor::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __RBTrailer("RBTrailer", RBTrailer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __RBCop("RBCop", RBCop::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EffectsVehicle("EffectsVehicle", EffectsVehicle::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EffectsCar("EffectsCar", EffectsCar::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EffectsPlayer("EffectsPlayer", EffectsPlayer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EffectsSmackable("EffectsSmackable", EffectsSmackable::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __EffectsFragment("EffectsFragment", EffectsFragment::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DamageVehicle("DamageVehicle", DamageVehicle::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DamageRacer("DamageRacer", DamageRacer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DamageDragster("DamageDragster", DamageDragster::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DamageHeli("DamageHeli", DamageHeli::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __DamageCopCar("DamageCopCar", DamageCopCar::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __PInput("PInput", PInput::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __InputPlayer("InputPlayer", InputPlayer::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __InputPlayerDrag("InputPlayerDrag", InputPlayerDrag::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype __InputNIS("InputNIS", InputNIS::Construct);

VehicleBehavior::VehicleBehavior(const BehaviorParams &bp, unsigned int num_interfaces)
    : Behavior(bp, num_interfaces)
    , mVehicle(nullptr) {
    GetOwner()->QueryInterface(&mVehicle);
}