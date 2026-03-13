#include "Table.hpp"

void *AverageBase::Allocate(unsigned int size, const char *name) {
    return gFastMem.Alloc(size, name);
}

void AverageBase::DeAllocate(void *ptr, unsigned int size, const char *name) {
    if (ptr != nullptr) {
        gFastMem.Free(ptr, size, name);
    }
}

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

template <> void tGraph<float>::Blend(float *dest, float *a, float *b, const float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

template <> void tTable<float>::Blend(float *dest, float *a, float *b, float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

template <> void tTable<bVector2>::Blend(bVector2 *dest, bVector2 *a, bVector2 *b, float blend_a) {
    float ax = a->x;
    float ay = a->y;
    dest->x = ax * blend_a;
    dest->y = ay * blend_a;
    bScaleAdd(dest, dest, b, 1.0f - blend_a);
}

template <> void tTable<bVector4>::Blend(bVector4 *dest, bVector4 *a, bVector4 *b, float blend_a) {
    float x = a->x;
    float y = a->y;
    float z = a->z;
    float w = a->w;
    dest->x = x * blend_a;
    dest->y = y * blend_a;
    dest->z = z * blend_a;
    dest->w = w * blend_a;
    bScaleAdd(dest, dest, b, 1.0f - blend_a);
}

float Graph::GetValue(float x) {
    int num = NumPoints;
    if (num > 1) {
        bVector2 *pts = Points;
        if (x <= pts[0].x) {
            return pts[0].y;
        }
        if (x >= pts[num - 1].x) {
            return pts[num - 1].y;
        }
        int upper = num - 1;
        int i = 0;
        if (i < upper) {
            do {
                float px = pts[i].x;
                if (x >= px && x < pts[i + 1].x) {
                    float y0 = pts[i].y;
                    float dx = pts[i + 1].x - px;
                    float dy = pts[i + 1].y - y0;
                    if (bAbs(dx) > 1e-06f) {
                        return ((x - px) / dx) * dy + y0;
                    }
                    return dy * 0.5f + y0;
                }
                i = i + 1;
            } while (i < upper);
        }
    }
    return Points[0].y;
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
    if (pData != nullptr && pData != SmallDataBuffer) {
        DeAllocate(pData, static_cast<unsigned int>(nSlots) << 2, "Average::Data");
        pData = nullptr;
    }
    pData = SmallDataBuffer;
    nSlots = slots;
    if (static_cast<int>(slots) > 5) {
        pData = static_cast<float *>(Allocate(static_cast<unsigned int>(static_cast<unsigned char>(slots)) << 2, "Average::Data"));
    }
    bMemSet(pData, 0, slots << 2);
}

Average::~Average() {
    if (pData != SmallDataBuffer) {
        DeAllocate(pData, static_cast<unsigned int>(nSlots) << 2, "Average::Data");
    }
}

void Average::Recalculate() {
    fTotal = 0.0f;
    for (int i = 0; i < static_cast<int>(static_cast<unsigned int>(nSlots)); i++) {
        fTotal = fTotal + pData[i];
    }
    unsigned int num = static_cast<unsigned int>(nSamples);
    if (num == 0) {
        num = 1;
    }
    fAverage = fTotal * (1.0f / static_cast<float>(static_cast<int>(num)));
}

void Average::Record(float fValue) {
    if (nSamples < nSlots) {
        nSamples = nSamples + 1;
    }
    fTotal = fTotal + fValue;
    fTotal = fTotal - pData[nCurrentSlot];
    pData[nCurrentSlot] = fValue;
    unsigned char slot = nCurrentSlot + 1;
    nCurrentSlot = slot;
    fAverage = fTotal / static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSamples)));
    if (slot >= nSlots) {
        nCurrentSlot = 0;
    }
}

void Average::Reset(float fValue) {
    for (int i = 0; i < nSlots; i++) {
        pData[i] = fValue;
    }
    nSamples = 0;
    fTotal = fValue * static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSlots)));
}

void Average::Flush(float fValue) {
    for (int i = 0; i < nSlots; i++) {
        pData[i] = fValue;
    }
    fTotal = fValue * static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSlots)));
    fAverage = fValue;
    nSamples = nSlots;
}

AverageWindow::AverageWindow(float f_timewindow, float f_frequency)
    : Average(f_timewindow * f_frequency + 0.5f), //
      fTimeWindow(f_timewindow),                  //
      iOldestValue(0),                            //
      AllocSize(4 * nSlots) {
    pTimeData = (float *)Allocate(AllocSize, "AverageWindow::TimeData");
    bMemSet(pTimeData, 0, nSlots * 4);
}

AverageWindow::~AverageWindow() {
    DeAllocate(pTimeData, AllocSize, "AverageWindow::TimeData");
}

void AverageWindow::Reset(float fValue) {
    for (int i = 0; i < nSlots; i++) {
        pData[i] = fValue;
        pTimeData[i] = 0.0f;
    }
    nSamples = 0;
    fAverage = 0.0f;
    nCurrentSlot = 0;
    iOldestValue = 0;
    fTotal = fValue * static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSlots)));
}

void AverageWindow::Record(const float fValue, const float fTimeNow) {
    if (pData[nCurrentSlot] == 0.0f && pTimeData[nCurrentSlot] == 0.0f) {
        nSamples = nSamples + 1;
    } else {
        fTotal = fTotal - pData[nCurrentSlot];
    }
    fTotal = fTotal + fValue;
    pData[nCurrentSlot] = fValue;
    pTimeData[nCurrentSlot] = fTimeNow;
    if (fTimeNow - pTimeData[iOldestValue] > fTimeWindow) {
        float sentinel = 0.0f;
        do {
            int oldest_offset = iOldestValue * 4;
            if (*(reinterpret_cast<float *>(reinterpret_cast<int>(pTimeData) + oldest_offset)) > sentinel) {
                fTotal = fTotal - pData[iOldestValue];
                pData[iOldestValue] = sentinel;
                pTimeData[iOldestValue] = sentinel;
                nSamples = nSamples - 1;
            }
            iOldestValue = iOldestValue + 1;
            if (iOldestValue >= static_cast<int>(static_cast<unsigned int>(nSlots))) {
                iOldestValue = 0;
            }
        } while (fTimeNow - pTimeData[iOldestValue] > fTimeWindow);
    }
    unsigned char slot = nCurrentSlot + 1;
    nCurrentSlot = slot;
    fAverage = fTotal / static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSamples)));
    if (nSlots <= slot) {
        nCurrentSlot = 0;
    }
}

void PidError::Record(float fError, float fTime, bool bZeroDerivative, bool bZeroIntegral) {
    float diff = fError - fCurrentError;
    fPreviousError = fCurrentError;
    fCurrentError = fError;
    float integral_value;
    if (bZeroIntegral) {
        integral_value = 0.0f;
    } else {
        integral_value = fTime * fError;
    }
    float derivative_value;
    if (bZeroDerivative) {
        derivative_value = 0.0f;
    } else {
        derivative_value = diff / fTime;
    }
    aTimes.Record(fTime);
    aIntegral.Record(integral_value);
    aDerivative.Record(derivative_value);
}
