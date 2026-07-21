#include "ConditionSimulator.hpp"

ConditionSimulatorEnum ConditionSimulator::m_simCondition;

void ConditionSimulator::SetSimCondition(int condition) {
    m_simCondition = static_cast<ConditionSimulatorEnum>(condition);
}
