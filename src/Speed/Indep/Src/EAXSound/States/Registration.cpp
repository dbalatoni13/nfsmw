#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_BottomOut.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Nitrous.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Rain.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_RoadNoise.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Skids.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Siren.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_SparkChatter.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_TrafficFX.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Turbo.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_WindNoise.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAITunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTrafficCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_MomentStrm.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_PFEATrax.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Reverb.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_TruckFX.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Woosh.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_WorldObject.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_Common.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_DriveBy.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Main.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Music.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DObjPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_HybridMotor.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Pathfinder5.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Physics.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Wheel.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/sfxctl_mastervol.hpp"

struct CARSFX_TruckWoosh : public CARSFX_TrafficWoosh {
  protected:
    static TypeInfo s_TypeInfo;
};

struct CARSFX_WindWeather : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXObj_Ambience : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXObj_Speech : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXObj_FEHUD : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
};

struct CARSFX_TrafficSkids : public CARSFX_Skids {
  protected:
    static TypeInfo s_TypeInfo;
};

struct CARSFX_TruckHorn : public CARSFX_TrafficHorn {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXCTL_3DRearPos : public SFXCTL_3DCarPos {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXCTL_3DFountainPos : public SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXCTL_3DTrailerPos : private SFXCTL_3DCarPos {
  protected:
    static TypeInfo s_TypeInfo;
};

struct SFXCTL_3DVoiceActorPos : private SFXCTL_3DObjPos {
  protected:
    static TypeInfo s_TypeInfo;
};

#define REGISTER_TYPEINFO(RegisterMethod, Type)                     \
    do {                                                            \
        struct Type##RegistrationAccessor : public Type {           \
            static SndBase::TypeInfo *GetStaticTypeInfo() {         \
                return &Type::s_TypeInfo;                           \
            }                                                       \
        };                                                          \
        CSTATEMGR_Base::RegisterMethod(Type##RegistrationAccessor::GetStaticTypeInfo()); \
    } while (0)

void RegisterSFX() {
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_SingleGinsuEng);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_DualGinsuEng);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_AEMSEngine);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_TruckFX);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_TruckWoosh);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_PreColWoosh);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_Skids);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_RoadNoise);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_Shift);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_Turbo);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_Nitrous);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_SparkChatter);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_WindNoise);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_WindWeather);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_BottomOut);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_Rain);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_Ambience);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_Collision);
    REGISTER_TYPEINFO(RegisterSFX, SFX_Common);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_Woosh);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_Speech);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_FEHUD);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_Siren);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_PFEATrax);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_TrafficEngine);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_TrafficHorn);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_TrafficWoosh);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_TrafficSkids);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_Helicopter);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_NISStream);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_MomentStrm);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_Reverb);
    REGISTER_TYPEINFO(RegisterSFX, SFXObj_WorldObject);
    REGISTER_TYPEINFO(RegisterSFX, CARSFX_TruckHorn);

    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Pathfinder);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_AccelTrans);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Engine);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_HybridMotor);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Physics);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_TruckPhysics);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Shifting);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Wheel);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Tunnel);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_MasterVol);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_GameState);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DMomentPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_Helicopter);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DCarPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DRearPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DColPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DScrapePos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DTrafficPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DFountainPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DWooshPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DRightWheelPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DLeftWheelPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DRightWindPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DLeftWindPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DTrailerPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DHeliPos);
    REGISTER_TYPEINFO(RegisterSFXCTL, SFXCTL_3DVoiceActorPos);
}

#undef REGISTER_TYPEINFO

void RegisterStates() {
    CSTATEMGR_Base::RegisterSTATE(EAXAITunerCar::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(EAXTrafficCar::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(EAXTunerCar::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(EAXTruck::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(EAXCopCar::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(CSTATE_Collision::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(CSTATE_Main::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(CSTATE_DriveBy::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(CSTATE_Music::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(CSTATE_Helicopter::GetStaticStateInfo());
    CSTATEMGR_Base::RegisterSTATE(CSTATE_WorldObject::GetStaticStateInfo());
}
