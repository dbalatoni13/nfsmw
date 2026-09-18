#ifndef SPEECH_EAXDISPATCH_H
#define SPEECH_EAXDISPATCH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "EAXCharacter.h"
#include "Speed/Indep/Src/EAXSound/SND_GEN/copspeech.hpp"

class EAXCop;

// total size: 0x40
// Decl: 24
class EAXDispatch : public EAXCharacter {
  public:
    EAXDispatch(int sID) : EAXCharacter(sID, nullptr, 0, 0) {}

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
    // EVReply, VehicleVinyls y VehicleCustomPaint NO existen en el ELF (ni el
    // simbolo ni las tres instanciaciones de ScheduleSpeech que arrastraban).
    // Eran codigo anadido en la descompilacion; fuera.
    void SubRBReply();
};

#endif
