#ifndef SPEECH_MISCSPEECH_H
#define SPEECH_MISCSPEECH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/CsisSpeechEvents.h"
#include "Speed/Indep/Src/Speech/MWRoadNames.h"

// Decl: 30
class MiscSpeech {
  public:
    static void SMSCellCall(int SMS_ID);
    static bool IsVehicleTypeOK();
    static bool IsSMSValid(int SMS_ID);
    static bool MapSMSToSPCHEnums(int SMS_ID, Csis::CellCallStruct &data);
    static void ForcedRhinoArrival(int speakerid);
    static void CrossDialog(int ID);
    static void PlayE3Specific();
    static bool GetSPAMLocation(int SPAMID, Csis::Type_offroad_moment_id &id);
    static bool GetLocation(RoadNames id, Csis::Type_location_region &region, Csis::Type_location &location);
    static int Bailout(int spkrID);
    static int LostSuspect(int spkrID);
    static int Unit911Reply(int spkrID);
    static int MoreDetails(int spkrID);
    static void RBWarning();
    static void RBPosition(int pos);
    static void RBEngaged(bool spikes_hit);
    static void RBAverted();
    static void SwarmingReply();
    static void SuperPursuitReply();
    static void SwarmingReplyFollow();
    static void QuadrantForming();
    static void SuspectPossiblyGone();
    static void QuadrantMoving();
    static void OtherLead();
    static void PossibleSuspect();
    static void WrongSuspect();
    static void SuspectGone();
    static void D_Day();
    static void DispIntroRace();
};

enum SPCH_Rules {};

enum Type_eta {
    Type_eta_No_eta_available = 1,
    Type_eta_eta_1_minute = 2,
    Type_eta_eta_2_minutes = 4,
    Type_eta_eta_3_minutes = 8,
    Type_eta_eta_4_minutes = 16,
    Type_eta_eta_5_minutes = 32,
    Type_eta_eta_6_minutes = 64,
    Type_eta_ets_7_minutes = 128,
    Type_eta_eta_8_minutes = 256,
    Type_eta_eta_9_minutes = 512,
    Type_eta_eta_10_minutes = 1024,
};

enum Type_roadcondition_type {
    Type_roadcondition_type_accident = 1,
    Type_roadcondition_type_construction = 2,
    Type_roadcondition_type_roadblock = 4,
    Type_roadcondition_type_bridge_out = 8,
    Type_roadcondition_type_overturned_semi = 16,
    Type_roadcondition_type_bad_roads = 32,
    Type_roadcondition_type_heavy_traffic = 64,
};

#endif
