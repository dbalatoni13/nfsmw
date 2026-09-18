#include "ENISVisualLook.hpp"

#include "Speed/Indep/Src/Ecstasy/eLight.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/VisualTreatment.h"

extern eLightMaterial *TweakLightMaterial;

void SetMiddleGrayValue(float val);

ENISVisualLook::ENISVisualLook(float pDesaturation, float pColourBloom, float pBleachFactor, int pUseHDR, float pFacingScale, float pGrazingScale)
    : Event(0x20), fDesaturation(pDesaturation), fColourBloom(pColourBloom), fBleachFactor(pBleachFactor), fUseHDR(pUseHDR),
      fFacingScale(pFacingScale), fGrazingScale(pGrazingScale) {

    if (IVisualTreatment::Get()) {

        IVisualTreatment::Get()->SetDesaturationTarget(fDesaturation);
        IVisualTreatment::Get()->SetColourBloomIntensityTarget(fColourBloom);
    }

    SetMiddleGrayValue(fBleachFactor);

    TweakLightMaterial = elGetLightMaterial(0x93c0eaf6);

    if (TweakLightMaterial) {
        static bool fOverwroteDefaultsFacingScale = false;
        static float fDefaultFacingScale = 0.0f;

        if (!fOverwroteDefaultsFacingScale) {

            fDefaultFacingScale = TweakLightMaterial->feData.EnvmapMaxScale;
        }

        if (fFacingScale >= 0.0f && fFacingScale <= 2.0f) {

            TweakLightMaterial->feData.EnvmapMaxScale = fFacingScale;
            fOverwroteDefaultsFacingScale = true;
        } else if (fOverwroteDefaultsFacingScale) {

            TweakLightMaterial->feData.EnvmapMaxScale = fDefaultFacingScale;
            fOverwroteDefaultsFacingScale = false;
        }

        static bool fOverwroteDefaultsGrazingScale = false;
        static float fDefaultGrazingScale = 0.0f;

        if (!fOverwroteDefaultsGrazingScale) {

            fDefaultGrazingScale = TweakLightMaterial->feData.EnvmapMinScale;
        }

        if (fGrazingScale >= 0.0f && fGrazingScale <= 2.0f) {

            TweakLightMaterial->feData.EnvmapMinScale = fGrazingScale;
            fOverwroteDefaultsGrazingScale = true;
        } else if (fOverwroteDefaultsGrazingScale) {

            TweakLightMaterial->feData.EnvmapMinScale = fDefaultGrazingScale;
            fOverwroteDefaultsGrazingScale = false;
        }
    }
}

ENISVisualLook::~ENISVisualLook() {
}

const char *ENISVisualLook::GetEventName() const {
    return "ENISVisualLook";
}

void ENISVisualLook_MakeEvent_Callback(const void *staticData) {
    new ENISVisualLook(((ENISVisualLook::StaticData *) staticData)->fDesaturation, ((ENISVisualLook::StaticData *) staticData)->fColourBloom,
                       ((ENISVisualLook::StaticData *) staticData)->fBleachFactor, ((ENISVisualLook::StaticData *) staticData)->fUseHDR,
                       ((ENISVisualLook::StaticData *) staticData)->fFacingScale, ((ENISVisualLook::StaticData *) staticData)->fGrazingScale);
}

int ENISVisualLook_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 6) {
        new ENISVisualLook(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3), (int) lua_tonumber(L, 4), lua_tonumber(L, 5),
                           lua_tonumber(L, 6));
    }
    return 0;
}

void ENISVisualLook_ResolveEvent_Callback(void *event, const UGroup *group) {
}
