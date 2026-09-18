#include "ESpawnExplosion.hpp"

#include "Speed/Indep/Src/Interfaces/SimModels/IModel.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Main/EventDynamicData.h"
#include "Speed/Indep/Src/Physics/Explosion.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

ESpawnExplosion::ESpawnExplosion(UCrc32 pPosition, float pSpeed, float pRadius, float pStartRadius, unsigned int pEffectThis,
                                 unsigned int pDoesDamage, unsigned int pIncludePlayers, unsigned int pExcludeVehicles,
                                 unsigned int pExcludeSmackables, unsigned int phModel)
    : Event(0x30), fPosition(pPosition), fSpeed(pSpeed), fRadius(pRadius), fStartRadius(pStartRadius), fEffectThis(pEffectThis),
      fDoesDamage(pDoesDamage), fIncludePlayers(pIncludePlayers), fExcludeVehicles(pExcludeVehicles), fExcludeSmackables(pExcludeSmackables),
      fhModel(phModel) {
}

ESpawnExplosion::~ESpawnExplosion() {
    IModel *iowner = IModel::FindInstance((HMODEL) fhModel);

    if (iowner) {

        if (fRadius <= 0.0f) {
            return;
        }

        if (fSpeed <= 0.0f) {
            return;
        }

        float exp_time = fRadius / fSpeed;

        if (exp_time > 30.0f) {
            return;
        }

        if (exp_time < Sim::GetTimeStep() * 2.0f) {
            return;
        }

        if (fStartRadius >= fRadius) {
            return;
        }

        UMath::Matrix4 matrix;

        iowner->GetTransform(matrix);

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

            node->GetPosition(arm);
            UMath::RotateTranslate(arm, matrix, position);
        }

        unsigned int targets = 3;

        if (fIncludePlayers) {
            targets = 7;
        }
        if (fExcludeVehicles) {
            targets &= ~2;
        }
        if (fExcludeSmackables) {
            targets &= ~1;
        }

        HMODEL hsource = (HMODEL) fhModel;

        ISimable::CreateInstance("Explosion", ExplosionParams(fSpeed, fStartRadius, fRadius, position, hsource, fEffectThis != 0,
                                                              fDoesDamage != 0, targets));
    }
}

const char *ESpawnExplosion::GetEventName() const {
    return "ESpawnExplosion";
}

void ESpawnExplosion_MakeEvent_Callback(const void *staticData) {
    new ESpawnExplosion(((ESpawnExplosion::StaticData *) staticData)->fPosition, ((ESpawnExplosion::StaticData *) staticData)->fSpeed,
                        ((ESpawnExplosion::StaticData *) staticData)->fRadius, ((ESpawnExplosion::StaticData *) staticData)->fStartRadius,
                        ((ESpawnExplosion::StaticData *) staticData)->fEffectThis, ((ESpawnExplosion::StaticData *) staticData)->fDoesDamage,
                        ((ESpawnExplosion::StaticData *) staticData)->fIncludePlayers,
                        ((ESpawnExplosion::StaticData *) staticData)->fExcludeVehicles,
                        ((ESpawnExplosion::StaticData *) staticData)->fExcludeSmackables, gEventDynamicData.fhModel);
}

int ESpawnExplosion_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 9) {
        new ESpawnExplosion(UCrc32(lua_tostring(L, 1)), lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4),
                            (unsigned int) lua_tonumber(L, 5), (unsigned int) lua_tonumber(L, 6), (unsigned int) lua_tonumber(L, 7),
                            (unsigned int) lua_tonumber(L, 8), (unsigned int) lua_tonumber(L, 9), gEventDynamicData.fhModel);
    }
    return 0;
}

void ESpawnExplosion_ResolveEvent_Callback(void *event, const UGroup *group) {
}
