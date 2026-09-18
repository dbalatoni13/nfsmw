#include "ESpawnFragment.hpp"

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Physics/Smackable.h"
#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/WorldConn.h"

ESpawnFragment::ESpawnFragment(UCrc32 pPartName, UCrc32 pCollisionName, UCrc32 pAttributeName, UCrc32 pJointName, unsigned int pAllowDupes,
                               float pInheritVelocity, unsigned int pHideParent, unsigned int pHidePart, unsigned int pAllowHiddenSpawn,
                               unsigned int pNoPhysics, unsigned int pSimplePhysics, unsigned int phModel, UMath::Vector4 pVelocity,
                               UMath::Vector4 pAngularVelocity)
    : Event(0x60), fPartName(pPartName), fCollisionName(pCollisionName), fAttributeName(pAttributeName), fJointName(pJointName),
      fAllowDupes(pAllowDupes), fInheritVelocity(pInheritVelocity), fHideParent(pHideParent), fHidePart(pHidePart),
      fAllowHiddenSpawn(pAllowHiddenSpawn), fNoPhysics(pNoPhysics), fSimplePhysics(pSimplePhysics), fhModel(phModel), fVelocity(pVelocity),
      fAngularVelocity(pAngularVelocity) {
}

ESpawnFragment::~ESpawnFragment() {
    IModel *model = IModel::FindInstance((HMODEL) fhModel);

    if (!model) {
        return;
    }

    ISimable *ifragment = NULL;

    if (!fAllowHiddenSpawn && !model->IsPartVisible(fPartName)) {
        return;
    }

    if (!fAllowDupes && model->GetChildModel(fPartName)) {
        return;
    }

    IModel *newmodel = model->SpawnModel(fPartName, fCollisionName, fAttributeName);

    if (!newmodel) {
        return;
    }

    const CollisionGeometry::Bounds *bounds = newmodel->GetCollisionGeometry();

    if (!bounds) {

        newmodel->ReleaseModel();
        return;
    }

    UMath::Matrix4 m_outer;
    UMath::Matrix4 m_inner;
    UMath::Matrix4 m_final;

    model->GetTransform(m_outer);
    bounds->GetTransform(m_inner);
    m_inner[3][3] = 1.0f;
    UMath::Mult(m_inner, m_outer, m_final);

    if (!fNoPhysics) {

        bool simple_physics = fSimplePhysics != 0;

        ifragment = ISimable::CreateInstance("Smackable", SmackableParams(m_final, false, newmodel, simple_physics));

        if (!ifragment) {

            newmodel->ReleaseModel();
            return;
        }
    } else {

        WorldConn::Pkt_Body_Send pkt(newmodel->GetWorldID(), m_final);
        SendService(UCrc32(0x998c21c0), &pkt);
    }

    if (fHidePart) {

        model->HidePart(fPartName);
    }

    if (fHideParent) {

        model->HideModel();
    }

    if (!ifragment) {
        return;
    }

    if (fInheritVelocity > 0.0f) {

        UMath::Vector3 v = UMath::Vector4To3(fVelocity);
        UMath::Vector3 a = UMath::Vector4To3(fAngularVelocity);

        UMath::Scale(v, fInheritVelocity);
        UMath::Scale(a, fInheritVelocity);

        ifragment->GetRigidBody()->SetLinearVelocity(v);
        ifragment->GetRigidBody()->SetAngularVelocity(a);
    }

    ISimable *iowner = model->GetSimable();

    bool can_hinge = true;

    if (iowner) {
        can_hinge = (iowner->IsPlayer() == false);
    }

    if (can_hinge && fJointName != UCrc32::kNull && iowner) {

        CollisionGeometry::IBoundable *ib_fragment;
        CollisionGeometry::IBoundable *ib_owner;

        if (ifragment->QueryInterface(&ib_fragment) && iowner->QueryInterface(&ib_owner)) {

            CollisionGeometry::CreateJoint(ib_owner, fJointName, ib_fragment, fJointName, NULL, NULL, 2);
        }
    }
}

const char *ESpawnFragment::GetEventName() const {
    return "ESpawnFragment";
}

void ESpawnFragment_MakeEvent_Callback(const void *staticData) {
    new ESpawnFragment(((ESpawnFragment::StaticData *) staticData)->fPartName, ((ESpawnFragment::StaticData *) staticData)->fCollisionName,
                       ((ESpawnFragment::StaticData *) staticData)->fAttributeName, ((ESpawnFragment::StaticData *) staticData)->fJointName,
                       ((ESpawnFragment::StaticData *) staticData)->fAllowDupes, ((ESpawnFragment::StaticData *) staticData)->fInheritVelocity,
                       ((ESpawnFragment::StaticData *) staticData)->fHideParent, ((ESpawnFragment::StaticData *) staticData)->fHidePart,
                       ((ESpawnFragment::StaticData *) staticData)->fAllowHiddenSpawn, ((ESpawnFragment::StaticData *) staticData)->fNoPhysics,
                       ((ESpawnFragment::StaticData *) staticData)->fSimplePhysics, gEventDynamicData.fhModel, gEventDynamicData.fVelocity,
                       gEventDynamicData.fAngularVelocity);
}

int ESpawnFragment_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 11) {
        new ESpawnFragment(UCrc32(lua_tostring(L, 1)), UCrc32(lua_tostring(L, 2)), UCrc32(lua_tostring(L, 3)), UCrc32(lua_tostring(L, 4)),
                           (unsigned int) lua_tonumber(L, 5), lua_tonumber(L, 6), (unsigned int) lua_tonumber(L, 7),
                           (unsigned int) lua_tonumber(L, 8), (unsigned int) lua_tonumber(L, 9), (unsigned int) lua_tonumber(L, 10),
                           (unsigned int) lua_tonumber(L, 11), gEventDynamicData.fhModel, gEventDynamicData.fVelocity,
                           gEventDynamicData.fAngularVelocity);
    }
    return 0;
}

void ESpawnFragment_ResolveEvent_Callback(void *event, const UGroup *group) {
}
