#include "Table.hpp"

// Credits: Brawltendo
// TODO variables aren't dwarf matching
float Table::GetValue(float arg) {
    const int entries = this->NumEntries;
    const float normarg = this->IndexMultiplier * (arg - this->MinArg);
    const int index = (int)normarg;

    if (index < 0 || normarg < 0.0f)
        return this->pTable[0];
    if (index >= (entries - 1))
        return this->pTable[entries - 1];

    float ind = index;
    if (ind > normarg)
        ind -= 1.0f;

    float delta = normarg - ind;
    return (1.0f - delta) * this->pTable[index] + delta * this->pTable[index + 1];
}

// STRIPPED
float Table::InverseLookup(float value) {}

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

template <> void tGraph<float>::Blend(float *dest, float *a, float *b, float blend_a) {
    *dest = *a * blend_a + *b * (1.0f - blend_a);
}

Graph::Graph(bVector2 *points, int num_points) {
    this->Points = points;
    this->NumPoints = num_points;
}

float Graph::GetValue(float x) {
    if (this->NumPoints > 1) {
        if (x <= this->Points[0].x) {
            return this->Points[0].y;
        }
        if (x >= this->Points[this->NumPoints - 1].x) {
            return this->Points[this->NumPoints - 1].y;
        }
        for (int i = 0; i < this->NumPoints - 1; i++) {
            if (x >= this->Points[i].x && x < this->Points[i + 1].x) {
                float delta_y = this->Points[i + 1].y - this->Points[i].y;
                float delta_x = this->Points[i + 1].x - this->Points[i].x;
                if (bAbs(delta_x) > 1e-06f) {
                    float u = (x - this->Points[i].x) / delta_x;
                    return u * delta_y + this->Points[i].y;
                }
                return delta_y * 0.5f + this->Points[i].y;
            }
        }
    }
    return this->Points[0].y;
}

// STRIPPED
float Graph::GetInverse(float y) {}

void *AverageBase::Allocate(unsigned int size, const char *name) {
    return gFastMem.Alloc(size, name);
}

void AverageBase::DeAllocate(void *ptr, unsigned int size, const char *name) {
    if (ptr != nullptr) {
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
      pData(nullptr),    //
      fTotal(0.0f) {}

Average::Average(int slots)
    : AverageBase(4, slots), //
      fAverage(0.0f),        //
      pData(nullptr),        //
      fTotal(0.0f) {
    this->Init(slots);
}

void Average::Init(int slots) {
    if ((this->pData != nullptr) && (this->pData != this->SmallDataBuffer)) {
        DeAllocate(this->pData, static_cast<unsigned int>(this->nSlots) << 2, "Average::pData");
        this->pData = nullptr;
    }
    this->pData = this->SmallDataBuffer;
    this->nSlots = slots;
    if (slots > 5) {
        this->pData = static_cast<float *>(this->Allocate(nSlots * sizeof(*this->pData), "Average::pData"));
    }
    bMemSet(this->pData, 0, slots << 2);
}

Average::~Average() {
    if (this->pData != this->SmallDataBuffer) {
        this->DeAllocate(this->pData, static_cast<unsigned int>(this->nSlots) << 2, "Average::pData");
    }
}

void Average::Recalculate() {
    this->fTotal = 0.0f;
    for (int i = 0; i < this->nSlots; i++) {
        this->fTotal += this->pData[i];
    }
    float fRecip = 1.0f / bMax(1, this->nSamples);
    this->fAverage = this->fTotal * fRecip;
}

void Average::Record(float fValue) {
    if (this->nSamples < this->nSlots) {
        this->nSamples++;
    }
    this->fTotal += fValue;
    this->fTotal -= this->pData[this->nCurrentSlot];
    this->pData[this->nCurrentSlot] = fValue;
    this->nCurrentSlot++;
    this->fAverage = this->fTotal / static_cast<int>(this->nSamples);
    if (this->nCurrentSlot >= this->nSlots) {
        this->nCurrentSlot = 0;
    }
}

void Average::Reset(float fValue) {
    for (int i = 0; i < this->nSlots; i++) {
        this->pData[i] = fValue;
    }
    this->nSamples = 0;
    this->fTotal = fValue * static_cast<int>(this->nSlots);
}

void Average::Flush(float fValue) {
    for (int i = 0; i < this->nSlots; i++) {
        this->pData[i] = fValue;
    }
    this->fTotal = fValue * static_cast<int>(this->nSlots);
    this->nSamples = this->nSlots;
    this->fAverage = fValue;
}

float Average::GetLastRecordedValue() const {
    if (this->nSamples != 0) {
        int last_slot = this->nCurrentSlot - 1;
        if (last_slot < 0) {
            last_slot = this->nSlots - 1;
        }
        return this->pData[last_slot];
    }
    return 0.0f;
}

AverageWindow::AverageWindow(float f_timewindow, float f_frequency)
    : Average(f_timewindow * f_frequency + 0.5f), //
      fTimeWindow(f_timewindow),                  //
      iOldestValue(0),                            //
      AllocSize(nSlots * sizeof(*pTimeData)) {
    this->pTimeData = static_cast<float *>(this->Allocate(this->AllocSize, "AverageWindow::TimeData"));
    bMemSet(this->pTimeData, 0, this->nSlots * sizeof(*this->pTimeData));
}

AverageWindow::~AverageWindow() {
    this->DeAllocate(this->pTimeData, this->AllocSize, "AverageWindow::TimeData");
}

void AverageWindow::Reset(float fValue) {
    for (int i = 0; i < this->nSlots; i++) {
        this->pData[i] = fValue;
        this->pTimeData[i] = 0.0f;
    }

    this->fTotal = fValue * static_cast<int>(this->nSlots);
    this->fAverage = 0.0f;
    this->nSamples = 0;
    this->iOldestValue = 0;
    this->nCurrentSlot = 0;
}

// STRIPPED
float AverageWindow::GetOldestValue() {
    return this->pData[iOldestValue];
}

// STRIPPED
float AverageWindow::GetOldestTime() {
    return this->pTimeData[iOldestValue];
}

void AverageWindow::Record(float fValue, float fTimeNow) {
    if (this->pData[this->nCurrentSlot] == 0.0f && this->pTimeData[this->nCurrentSlot] == 0.0f) {
        nSamples++;
    } else {
        this->fTotal -= this->pData[this->nCurrentSlot];
    }
    this->fTotal += fValue;
    this->pData[this->nCurrentSlot] = fValue;
    this->pTimeData[this->nCurrentSlot] = fTimeNow;
    while (fTimeNow - this->pTimeData[this->iOldestValue] > this->fTimeWindow) {
        if (this->pTimeData[this->iOldestValue] > 0.0f) {
            this->fTotal -= this->pData[this->iOldestValue];
            this->pData[this->iOldestValue] = 0.0f;
            this->pTimeData[this->iOldestValue] = 0.0f;
            this->nSamples--;
        }
        this->iOldestValue++;
        if (this->iOldestValue >= nSlots) {
            this->iOldestValue = 0;
        }
    }
    this->nCurrentSlot++;
    this->fAverage = this->fTotal / static_cast<int>(this->nSamples);
    if (this->nCurrentSlot >= this->nSlots) {
        this->nCurrentSlot = 0;
    }
}

void PidError::Record(float fError, float fTime, bool bZeroDerivative, bool bZeroIntegral) {
    this->fPreviousError = this->fCurrentError;
    this->fCurrentError = fError;

    float fDeltaError = this->fCurrentError - this->fPreviousError;
    float fIntegralTerm = bZeroIntegral ? 0.0f : (fTime * (fDeltaError * 0.5f + this->fPreviousError));
    float fDerivativeTerm = bZeroDerivative ? 0.0f : (fDeltaError / fTime);

    this->aTimes.Record(fTime);
    this->aIntegral.Record(fIntegralTerm);
    this->aDerivative.Record(fDerivativeTerm);
}

// STRIPPED
void PidError::DoSnapshot(ReplaySnapshot *snapshot) {}

// STRIPPED
void PidError::Reset(float fCalibrat) {}

// STRIPPED
void PidError::ResetIntegral(float fCalibrate) {}

// STRIPPED
void PidError::ResetDerivative(float fCalibrate) {}

// STRIPPED
void Linear::Init(float x0, float y0, float x1, float y1) {}

// STRIPPED
float Linear::GetValue(float x) {}

// STRIPPED
float Linear::GetInverse(float y) {}
