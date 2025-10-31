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

template <>
void tGraph<float>::Blend(float *dest, float *a, float *b, const float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

AverageBase::AverageBase(int size, int slots)
: nSize(size)
, nSlots(slots) 
, nSamples(0) 
, nCurrentSlot(0) {
}

Average::Average()
: AverageBase(4, 0)
, fAverage(0.0f)
, pData(NULL) 
, fTotal(0.0f) {
}

Average::Average(int slots)
: AverageBase(4, slots)
, fAverage(0.0f)
, pData(NULL) 
, fTotal(0.0f) {
    Init(slots);
}

AverageWindow::AverageWindow(float f_timewindow, float f_frequency)
: Average(f_timewindow * f_frequency + 0.5f)
, fTimeWindow(f_timewindow)
, iOldestValue(0)
, AllocSize(4 * nSize) {
    pTimeData = (float *)Allocate(AllocSize, "AverageWindow::TimeData");
    bMemSet(pTimeData, 0, AllocSize);
}
