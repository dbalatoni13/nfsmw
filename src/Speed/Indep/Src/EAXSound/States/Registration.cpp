
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_BottomOut.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Engine.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Nitrous.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Rain.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_RoadNoise.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Siren.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Skids.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_SparkChatter.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Turbo.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX_WindNoise.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Ambience.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_FEHUD.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_MomentStrm.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Reverb.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Speech.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_TruckFX.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Woosh.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_WorldObject.hpp"
#include "Speed/Indep/Src/EAXSound/EAXAITunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTrafficCar.hpp"
#include "Speed/Indep/Src/EAXSound/EAXTunerCar.hpp"
#include "Speed/Indep/Src/EAXSound/SFX_Common.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Helicopter.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Main.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Music.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Enviro.hpp"
#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_PlayerCar.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_AccelTrans.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Collision.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_MasterVol.hpp"

void RegisterSFX() {
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_SingleGinsuEng));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_DualGinsuEng));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_AEMSEngine));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_TruckFX));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_TruckWoosh));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_PreColWoosh));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_Skids));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_RoadNoise));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_Shift));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_Turbo));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_Nitrous));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_SparkChatter));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_WindNoise));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_WindWeather));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_BottomOut));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_Rain));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_Ambience));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_Collision));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFX_Common));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_Woosh));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_Speech));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_FEHUD));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_Siren));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_PFEATrax));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_TrafficEngine));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_TrafficHorn));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_TrafficWoosh));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_TrafficSkids));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_Helicopter));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_NISStream));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_MomentStrm));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_Reverb));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(SFXObj_WorldObject));
    CSTATEMGR_Base::RegisterSFX(TYPEINFO(CARSFX_TruckHorn));

    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Pathfinder));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_AccelTrans));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Engine));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_HybridMotor));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Physics));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_TruckPhysics));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Shifting));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Wheel));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Tunnel));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_MasterVol));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_GameState));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DMomentPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_Helicopter));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DCarPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DRearPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DColPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DScrapePos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DTrafficPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DFountainPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DWooshPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DRightWheelPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DLeftWheelPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DRightWindPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DLeftWindPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DTrailerPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DHeliPos));
    CSTATEMGR_Base::RegisterSFXCTL(TYPEINFO(SFXCTL_3DVoiceActorPos));
}

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
