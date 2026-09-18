// Las tablas estaticas de las interfaces de simulacion estan en la .bss de esta
// unidad en el original, delante de todo lo demas, pero ningun .cpp con codigo
// las contiene (debug_lines no da su fichero): se definen aqui, en el orden en
// que el original las construye.
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IEntity.h"
#include "Speed/Indep/Src/Interfaces/Simables/IExplosion.h"
#include "Speed/Indep/Src/Interfaces/Simables/IINput.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IVehicleCache.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimpleBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/Simables/IDisposable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IRecordablePlayer.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISpikeable.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/Simables/ICause.h"

IMPLEMENT_LISTABLE(IExplosion)
IMPLEMENT_LISTABLE(IDisposable)
template <>
UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, VEHICLE_MAX>::_ListSet
    UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, VEHICLE_MAX>::_mLists =
        UTL::Collections::ListableSet<IVehicle, 10, eVehicleList, VEHICLE_MAX>::_ListSet();
IMPLEMENT_LISTABLE(IRigidBody)
IMPLEMENT_LISTABLE(ICollisionBody)
IMPLEMENT_LISTABLE(ISimpleBody)
template <>
UTL::Collections::ListableSet<Sim::IEntity, 8, eEntityList, ENTITY_MAX>::_ListSet
    UTL::Collections::ListableSet<Sim::IEntity, 8, eEntityList, ENTITY_MAX>::_mLists =
        UTL::Collections::ListableSet<Sim::IEntity, 8, eEntityList, ENTITY_MAX>::_ListSet();
template <>
UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::_ListSet
    UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::_mLists =
        UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX>::_ListSet();
IMPLEMENT_LISTABLE(IRecordablePlayer)
IMPLEMENT_LISTABLE(IInputPlayer)
IMPLEMENT_LISTABLE(IModel)
IMPLEMENT_LISTABLE(IPursuit)
IMPLEMENT_LISTABLE(IRoadBlock)
IMPLEMENT_LISTABLE(IHud)
IMPLEMENT_LISTABLE(IVehicleCache)
IMPLEMENT_LISTABLE(ITrafficCenter)
IMPLEMENT_LISTABLE(ISpikeable)

template <>
UTL::Collections::Instanceable<HSIMABLE, ISimable, 160>::_List UTL::Collections::Instanceable<HSIMABLE, ISimable, 160>::_mList =
    UTL::Collections::Instanceable<HSIMABLE, ISimable, 160>::_List();
template <>
UTL::Collections::Instanceable<HACTIVITY, Sim::IActivity, 40>::_List UTL::Collections::Instanceable<HACTIVITY, Sim::IActivity, 40>::_mList =
    UTL::Collections::Instanceable<HACTIVITY, Sim::IActivity, 40>::_List();
template <>
UTL::Collections::Instanceable<HMODEL, IModel, 434>::_List UTL::Collections::Instanceable<HMODEL, IModel, 434>::_mList =
    UTL::Collections::Instanceable<HMODEL, IModel, 434>::_List();
template <>
UTL::Collections::Instanceable<HCAUSE, ICause, 10>::_List UTL::Collections::Instanceable<HCAUSE, ICause, 10>::_mList =
    UTL::Collections::Instanceable<HCAUSE, ICause, 10>::_List();

#include "Speed/Indep/Src/Physics/SmackableTrigger.cpp"

#include "Speed/Indep/Src/Physics/Common/Explosion.cpp"

#include "Speed/Indep/Src/Physics/Common/PVehicle.cpp"

#include "Speed/Indep/Src/Physics/Common/PhysicsObject.cpp"

#include "Speed/Indep/Src/Physics/Common/Smackable.cpp"

#include "Speed/Indep/Src/Physics/Common/Wheel.cpp"

#include "Speed/Indep/Src/Physics/Common/VehicleBehaviors.cpp"

#include "Speed/Indep/Src/Physics/Common/VehicleSystem.cpp"

#include "Speed/Indep/Src/Physics/Common/Behavior.cpp"

#include "Speed/Indep/Src/Physics/Common/Bounds.cpp"

#include "Speed/Indep/Src/Physics/Common/SmokeableInfo.cpp"

#include "Speed/Indep/Src/Physics/PhysicsUpgrades.cpp"

#include "Speed/Indep/Src/Physics/PhysicsInfo.cpp"

#include "Speed/Indep/Src/Physics/PhysicsTunings.cpp"
