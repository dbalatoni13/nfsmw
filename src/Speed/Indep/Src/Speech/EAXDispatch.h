#ifndef SPEECH_EAXDISPATCH_H
#define SPEECH_EAXDISPATCH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXCharacter.h"
#include "EAXCop.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
enum Type_pursuit_type {
    Type_pursuit_type_Generic_Speeder = 1,
    Type_pursuit_type_Possible_Wanted = 2,
    Type_pursuit_type_Hit_and_Run = 4,
    Type_pursuit_type_Reckless = 8,
    Type_pursuit_type_Unit_Rammed = 16,
};

enum Type_jurisdiction {
    Type_jurisdiction_state = 1,
    Type_jurisdiction_federal = 2,
};
}

struct EAXDispatch : public EAXCharacter {
    EAXDispatch(int sID) : EAXCharacter(sID, 0, 0, 0) {}

    ~EAXDispatch() override;
    void Update() override;
    void BackupReply(EAXCop *cop, int yes, int type);
    void ArrestReply();
    void PursuitUpdate(EAXCop *cop);
    void PursuitEscalationGeneric();
    void PursuitEscalation();
    void BackupUpdate(EAXCop *cop, int yes);
    void BreakAway();
    void GoAhead();
    void TimeExpired();
    void Report911(Csis::Type_pursuit_type infraction);
    void RBUpdate(EAXCop *cop, signed char true_false);
    void RBReply(EAXCop *cop, signed char true_false, unsigned int type);
    void JurisShift(Csis::Type_jurisdiction jurisdiction);
    void BackupETA();
    void VehicleDescription();
    void NoVehicleDescription();
    void SubRBReply();
};

#endif
