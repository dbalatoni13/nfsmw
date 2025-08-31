#include "Table.hpp"

// Credits: Brawltendo
// TODO variables aren't dwarf matching
float Table::GetValue(float input) {
    const int entries = NumEntries;
    const float normarg = IndexMultiplier * (input - MinArg);
    const int index = (int)normarg;

    if (index < 0 || normarg < 0.0f)
        return pTable[0];
    if (index >= (entries - 1))
        return pTable[entries - 1];

    float ind = index;
    if (ind > normarg)
        ind -= 1.0f;

    float delta = normarg - ind;
    return (1.0f - delta) * pTable[index] + delta * pTable[index + 1];
}
