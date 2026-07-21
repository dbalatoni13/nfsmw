#ifndef ONLINE_CONDITION_SIMULATOR_HPP
#define ONLINE_CONDITION_SIMULATOR_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum ConditionSimulatorEnum {
    SIMCONDITION_FIRST = 0,
    SIMCONDITION_NONE = 0,
    SIMCONDITION_UNABLETOCONNECTTOMASTER = 1,
    SIMCONDITION_INVALIDMASTERNAME = 2,
    SIMCONDITION_MASTERSERVERGOESDOWN = 3,
    SIMCONDITION_NTSCINEUROPE = 4,
    SIMCONDITION_NOAPPKEEPALIVES = 5,
    SIMCONDITION_FIREWALLPROBLEM = 6,
    SIMCONDITION_PATCHAVAILABLE = 7,
    SIMCONDITION_COUNT = 8
};

struct ConditionSimulator {
    static void SetSimCondition(int condition);
    static ConditionSimulatorEnum GetSimCondition() { return m_simCondition; }
    static void ListSimConditions() {}

  private:
    static ConditionSimulatorEnum m_simCondition;
};

#endif
