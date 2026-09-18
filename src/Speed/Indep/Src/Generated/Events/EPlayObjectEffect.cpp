#include "EPlayObjectEffect.hpp"

#include <new>

#include "Speed/Indep/Src/Generated/AttribSys/Classes/effects.h"
#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"

// El DWARF del original lista `inline void OrthoInverse(Matrix4&)` expandido en
// este destructor, y el mapa de lineas atribuye el `addi r3,r1,0x88`+`bl` a
// UMath.h:680: el original llamaba a un envoltorio inline, no al global.
static inline void UMathOrthoInverse(UMath::Matrix4 &m) {
    OrthoInverse(m);
}

EPlayObjectEffect::EPlayObjectEffect(const char *pEffect, UCrc32 pPosition, float pIntensity, UCrc32 pID, unsigned int pTracking,
                                     unsigned int pAllowHiddenSpawn, unsigned int phModel, UMath::Vector4 pVelocity)
    : Event(0x40), fEffect(pEffect), fPosition(pPosition), fIntensity(pIntensity), fID(pID), fTracking(pTracking),
      fAllowHiddenSpawn(pAllowHiddenSpawn), fhModel(phModel), fVelocity(pVelocity) {
}

EPlayObjectEffect::~EPlayObjectEffect() {
    if (fEffect) {

        const Attrib::Collection *effect = Attrib::FindCollection(Attrib::Gen::effects::ClassKey(), Attrib::StringToKey(fEffect));

        if (effect) {
            IModel *iowner = IModel::FindInstance((HMODEL) fhModel);

            if (iowner) {

                if (!fAllowHiddenSpawn && iowner->IsHidden()) {
                    return;
                }

                float intensity = UMath::Clamp(fIntensity, 0.0f, 1.0f);

                UMath::Matrix4 matrix;

                iowner->GetTransform(matrix);

                UMath::Vector3 magnitude = UMath::Vector3Make(0.0f, intensity, 0.0f);
                UMath::Vector3 position = UMath::Vector4To3(matrix.v3);

                if (fPosition != UCrc32::kNull) {

                    const CollisionGeometry::Bounds *bnds = iowner->GetCollisionGeometry();

                    if (!bnds) {
                        return;
                    }

                    const CollisionGeometry::Bounds *node = bnds->GetChild(fPosition);

                    if (!node) {
                        return;
                    }

                    UMath::Vector3 arm;
                    UMath::Vector4 orientation;

                    node->GetPosition(arm);
                    UMath::RotateTranslate(arm, matrix, position);

                    node->GetOrientation(orientation);
                    UMath::ExtractYAxis(orientation, magnitude);
                    UMath::Rotate(magnitude, matrix, magnitude);
                    UMath::Scale(magnitude, intensity, magnitude);
                }

                if (fID != UCrc32::kNull) {

                    if (fTracking) {
                        UMath::Matrix4 world_to_local;

                        UMath::Copy(matrix, world_to_local);
                        UMathOrthoInverse(world_to_local);
                        UMath::RotateTranslate(position, world_to_local, position);
                        UMath::Rotate(magnitude, world_to_local, magnitude);
                    }

                    iowner->PlayEffect(fID, effect, position, magnitude, fTracking != 0);
                } else {
                    Sim::Effect::Fire(effect, position, magnitude, iowner->GetWorldID(),
                                      iowner->GetAttributes().GetConstCollection(), NULL, 0);
                }
            }
        }
    }
}

const char *EPlayObjectEffect::GetEventName() const {
    return "EPlayObjectEffect";
}

void EPlayObjectEffect_MakeEvent_Callback(const void *staticData) {
    new EPlayObjectEffect(((EPlayObjectEffect::StaticData *) staticData)->fEffect, ((EPlayObjectEffect::StaticData *) staticData)->fPosition,
                          ((EPlayObjectEffect::StaticData *) staticData)->fIntensity, ((EPlayObjectEffect::StaticData *) staticData)->fID,
                          ((EPlayObjectEffect::StaticData *) staticData)->fTracking,
                          ((EPlayObjectEffect::StaticData *) staticData)->fAllowHiddenSpawn, gEventDynamicData.fhModel,
                          gEventDynamicData.fVelocity);
}

int EPlayObjectEffect_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 6) {
        new EPlayObjectEffect(lua_tostring(L, 1), UCrc32(lua_tostring(L, 2)), lua_tonumber(L, 3), UCrc32(lua_tostring(L, 4)),
                              (unsigned int) lua_tonumber(L, 5), (unsigned int) lua_tonumber(L, 6), gEventDynamicData.fhModel,
                              gEventDynamicData.fVelocity);
    }
    return 0;
}

void EPlayObjectEffect_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EPlayObjectEffect::StaticData *) event)->fEffect) CARP::TagReference(group);
}
