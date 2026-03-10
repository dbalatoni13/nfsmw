#include "Speed/Indep/Src/Physics/Smackable.h"

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IPlaceableScenery.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ISceneryModel.h"
#include "Speed/Indep/Src/Interfaces/SimModels/ITriggerableModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.h"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Physics/HeirarchyModel.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/DamageZones.h"
#include "Speed/Indep/Src/World/WWorldPos.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"

namespace Sim {
bool CanSpawnRigidBody(const UMath::Vector3 &position, bool highPriority);
bool CanSpawnSimpleRigidBody(const UMath::Vector3 &position, bool highPriority);
} // namespace Sim

namespace DamageZone {
void GetImpactStimulus(unsigned int &stimulus);
} // namespace DamageZone

namespace Dynamics {
namespace Articulation {
bool IsJoined(const IEntity *a, const IEntity *b);
} // namespace Articulation
} // namespace Dynamics

UTL::COM::Factory<Sim::Param, ISimable, UCrc32>::Prototype _Smackable("Smackable",
                                                                       Smackable::Construct);
UTL::COM::Factory<const BehaviorParams &, Behavior, UCrc32>::Prototype _RBSmackable(
    "RBSmackable", RBSmackable::Construct);

int Smackable_RigidCount;
static float Smackable_ManagementRate = 0.125f;
