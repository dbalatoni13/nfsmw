#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

VehicleBehavior::VehicleBehavior(const BehaviorParams &bp, unsigned int num_interfaces) : Behavior(bp, num_interfaces), mVehicle(nullptr) {
    this->GetOwner()->QueryInterface(&this->mVehicle);
}
