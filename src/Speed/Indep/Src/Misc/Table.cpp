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

template <> void tTable<bVector2>::Blend(bVector2 *dest, bVector2 *a, bVector2 *b, float blend_a) {
    bScale(dest, a, blend_a);
    bScaleAdd(dest, dest, b, 1.0f - blend_a);
}

template <> void tTable<bVector4>::Blend(bVector4 *dest, bVector4 *a, bVector4 *b, float blend_a) {
    bScale(dest, a, blend_a);
    bScaleAdd(dest, dest, b, 1.0f - blend_a);
}

template <> void tTable<float>::Blend(float *dest, float *a, float *b, float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

template <> void tGraph<float>::Blend(float *dest, float *a, float *b, const float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

Graph::Graph(bVector2 *points, int num_points) {
    Points = points;
    NumPoints = num_points;
}

float Graph::GetValue(float x) {
    if (NumPoints > 1) {
        if (x <= Points[0].x) {
            return Points[0].y;
        }
        if (x >= Points[NumPoints - 1].x) {
            return Points[NumPoints - 1].y;
        }
        for (int i = 0; i < NumPoints - 1; i++) {
            if (x >= Points[i].x && x < Points[i + 1].x) {
                float delta_y = Points[i + 1].y - Points[i].y;
                float delta_x = Points[i + 1].x - Points[i].x;
                if (bAbs(delta_x) > 1e-06f) {
                    float u = (x - Points[i].x) / delta_x;
                    return u * delta_y + Points[i].y;
                }
                return delta_y * 0.5f + Points[i].y;
            }
        }
    }
    return Points[0].y;
}

void *AverageBase::Allocate(unsigned int size, const char *name) {
    return gFastMem.Alloc(size, name);
}

void AverageBase::DeAllocate(void *ptr, unsigned int size, const char *name) {
    if (ptr) {
        gFastMem.Free(ptr, size, name);
    }
}

AverageBase::AverageBase(int size, int slots)
    : nSize(size),   //
      nSlots(slots), //
      nSamples(0),   //
      nCurrentSlot(0) {}

Average::Average()
    : AverageBase(4, 0), //
      fAverage(0.0f),    //
      pData(NULL),       //
      fTotal(0.0f) {}

Average::Average(int slots)
    : AverageBase(4, slots), //
      fAverage(0.0f),        //
      pData(NULL),           //
      fTotal(0.0f) {
    Init(slots);
}

void Average::Init(int slots) {
    if (pData && pData != SmallDataBuffer) {
        DeAllocate(pData, static_cast<unsigned int>(nSlots) << 2, "Average::pData");
        pData = nullptr;
    }
    pData = SmallDataBuffer;
    nSlots = slots;
    if (slots > 5) {
        pData = static_cast<float *>(Allocate(nSlots * sizeof(*pData), "Average::pData"));
    }
    bMemSet(pData, 0, slots << 2);
}

Average::~Average() {
    if (pData != SmallDataBuffer) {
        DeAllocate(pData, static_cast<unsigned int>(nSlots) << 2, "Average::pData");
    }
}

void Average::Recalculate() {
    fTotal = 0.0f;
    for (int i = 0; i < nSlots; i++) {
        fTotal += pData[i];
    }
    float fRecip = 1.0f / bMax(1, nSamples);
    fAverage = fTotal * fRecip;
}

void Average::Record(float fValue) {
    if (nSamples < nSlots) {
        nSamples++;
    }
    fTotal += fValue;
    fTotal -= pData[nCurrentSlot];
    pData[nCurrentSlot] = fValue;
    nCurrentSlot++;
    fAverage = fTotal / static_cast<int>(nSamples);
    if (nCurrentSlot >= nSlots) {
        nCurrentSlot = 0;
    }
}

void Average::Reset(float fValue) {
    for (int i = 0; i < nSlots; i++) {
        pData[i] = fValue;
    }
    nSamples = 0;
    fTotal = fValue * static_cast<int>(nSlots);
}

void Average::Flush(float fValue) {
    for (int i = 0; i < nSlots; i++) {
        pData[i] = fValue;
    }
    fTotal = fValue * static_cast<int>(nSlots);
    nSamples = nSlots;
    fAverage = fValue;
}

float Average::GetLastRecordedValue() const {
    if (nSamples != 0) {
        int idx = nCurrentSlot - 1;
        if (idx < 0) {
            idx = nSlots - 1;
        }
        return pData[idx];
    }
    return 0.0f;
}

AverageWindow::AverageWindow(float f_timewindow, float f_frequency)
    : Average(f_timewindow * f_frequency + 0.5f), //
      fTimeWindow(f_timewindow),                  //
      iOldestValue(0),                            //
      AllocSize(nSlots * sizeof(*pTimeData)) {
    pTimeData = static_cast<float *>(Allocate(AllocSize, "AverageWindow::TimeData"));
    bMemSet(pTimeData, 0, nSlots * sizeof(*pTimeData));
}

AverageWindow::~AverageWindow() {
    DeAllocate(pTimeData, AllocSize, "AverageWindow::TimeData");
}

void AverageWindow::Reset(float fValue) {
    for (int i = 0; i < nSlots; i++) {
        pData[i] = fValue;
        pTimeData[i] = 0.0f;
    }
    nCurrentSlot = 0;
    fAverage = 0.0f;
    iOldestValue = 0;
    nSamples = 0;
    fTotal = fValue * static_cast<int>(nSlots);
}

void AverageWindow::Record(const float fValue, const float fTimeNow) {
    if (pData[nCurrentSlot] == 0.0f && pTimeData[nCurrentSlot] == 0.0f) {
        nSamples++;
    } else {
        fTotal -= pData[nCurrentSlot];
    }
    fTotal += fValue;
    pData[nCurrentSlot] = fValue;
    pTimeData[nCurrentSlot] = fTimeNow;
    while (fTimeNow - pTimeData[iOldestValue] > fTimeWindow) {
        if (pTimeData[iOldestValue] > 0.0f) {
            fTotal -= pData[iOldestValue];
            pData[iOldestValue] = 0.0f;
            pTimeData[iOldestValue] = 0.0f;
            nSamples--;
        }
        iOldestValue++;
        if (iOldestValue >= nSlots) {
            iOldestValue = 0;
        }
    }
    nCurrentSlot++;
    fAverage = fTotal / static_cast<int>(nSamples);
    if (nCurrentSlot >= nSlots) {
        nCurrentSlot = 0;
    }
}

void PidError::Record(float fError, float fTime, bool bZeroDerivative, bool bZeroIntegral) {
    fPreviousError = fCurrentError;
    fCurrentError = fError;

    float fDeltaError = fCurrentError - fPreviousError;
    float fIntegralTerm = bZeroIntegral ? 0.0f : (fTime * (fDeltaError * 0.5f + fPreviousError));
    float fDerivativeTerm = bZeroDerivative ? 0.0f : (fDeltaError / fTime);

    aTimes.Record(fTime);
    aIntegral.Record(fIntegralTerm);
    aDerivative.Record(fDerivativeTerm);
}
