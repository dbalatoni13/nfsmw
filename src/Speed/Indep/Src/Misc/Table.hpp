//
//
//
//
//
//
//
#ifndef TABLE_HPP
#define TABLE_HPP

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#define TABLE_TYPE(_TYPE_) USE_FASTALLOC(_TYPE_) // Decl: 16

// total size: 0x10
// Decl: 20
class TableBase {
  public:
    // Decl: 24
    TableBase(int32 num, float min, float max) {
        this->NumEntries = num;
        this->SetMinMax(min, max);
    }

    // Decl: 26
    void SetMinMax(float fMin, float fMax) {
        this->MinArg = fMin;
        this->MaxArg = fMax;
        this->CalcIndexMultiplier();
    }

  protected:
    // Decl: 36
    void CalcIndexMultiplier() {
        this->IndexMultiplier = (this->NumEntries - 1) / (this->MaxArg - this->MinArg);
    }
    int32 NumEntries;      // offset 0x0, size 0x4, Decl: 37
    float MinArg;          // offset 0x4, size 0x4, Decl: 38
    float MaxArg;          // offset 0x8, size 0x4, Decl: 38
    float IndexMultiplier; // offset 0xC, size 0x4, Decl: 39
};

// total size: 0x14
// Decl: 44
class Table : public TableBase {
  public:
    // Decl: 48
    Table(const float *table, int32 num, float min, float max) : TableBase(num, min, max), pTable(table) {}

    float GetValue(float arg);        // Decl: 50
    float InverseLookup(float value); // Decl: 51

    // Decl: 52
    const float *GetData() const {
        return this->pTable;
    }
    // Decl: 53
    void SetData(const float *data, int num) {
        // TODO
    }

  private:
    const float *pTable; // offset 0x10, size 0x4, Decl: 57
};

// total size: 0x14
// Decl: 70
template <typename T> class tTable : public TableBase {
  public:
    tTable() : TableBase(0, 0.0f, 1.0f) {}

    void Blend(T *dest, T *a, T *b, float blend_a); // Decl: 74

  private:
    T *pTable; // Decl: 88
};

// total size: 0x8
// Decl: 97
class Graph {
  public:
    USE_FASTALLOC(Graph);                    // Decl: 99
    Graph(bVector2 *points, int num_points); // Decl: 101

    float GetValue(float x); // Decl: 102
    float GetInverse(float y);

    bVector2 *GetData() {
        return this->Points;
    }

  private:
    bVector2 *Points; // offset 0x0, size 0x4, Decl: 108
    int NumPoints;    // offset 0x4, size 0x4, Decl: 109
};

// Decl: 123
template <typename T> struct GraphEntry {
    T x; // Decl: 124
    T y; // Decl: 125
};

// Decl: 130
template <typename T> class tGraph {
  public:
    // Decl: 133
    tGraph(GraphEntry<T> *data, int num) {
        this->GraphData = data;
        this->NumEntries = num;
    }
    void Blend(T *dest, T *a, T *b, T blend_a);

    // Decl: 137
    float GetValue(T x) {
        float ret;
        this->GetValue(&ret, x);
        return ret;
    }

    // Credits: Brawltendo
    // UNSOLVED
    // Decl: 146
    void GetValue(T *pValue, T x) {
        if (this->NumEntries > 1) {
            if (x <= this->GraphData[0].x) {
                bMemCpy(pValue, &this->GraphData[0].y, sizeof(T));
            } else if (x >= this->GraphData[this->NumEntries - 1].x) {
                bMemCpy(pValue, &this->GraphData[this->NumEntries - 1].y, sizeof(T));
            } else {
                for (int i = 0; i < this->NumEntries - 1; ++i) {
                    if (x >= this->GraphData[i].x && x < this->GraphData[i + 1].x) {
                        const T blend = (x - this->GraphData[i].x) / (this->GraphData[i + 1].x - this->GraphData[i].x);
                        this->Blend(pValue, &this->GraphData[i + 1].y, &this->GraphData[i].y, blend);
                        return;
                    }
                }
            }
        } else if (this->NumEntries > 0) {
            bMemCpy(pValue, &this->GraphData[0].y, sizeof(T));
        }
    }

  private:
    GraphEntry<T> *GraphData; // offset 0x0, size 0x4, Decl: 166
    int NumEntries;           // offset 0x4, size 0x4, Decl: 167
};

// total size: 0x8
// Decl: 172
class AverageBase {
  public:
    USE_FASTALLOC(AverageBase);       // Decl: 174
    AverageBase(int size, int slots); // Decl: 176
    virtual ~AverageBase() {}         // Decl: 177

    // bool FullySampled() {} // Decl: 179

    // Decl: 180
    uint8 GetNumSamples() {
        return this->nSamples;
    }

    virtual void Recalculate() {} // Decl: 181

  protected:
    static void *Allocate(unsigned int size, const char *name);             // Decl: 184
    static void DeAllocate(void *ptr, unsigned int size, const char *name); // Decl: 185

    uint8 nSize;        // Decl: 187
    uint8 nSlots;       // Decl: 188
    uint8 nSamples;     // Decl: 189
    uint8 nCurrentSlot; // Decl: 190
};

// total size: 0x28
// Decl: 195
class Average : public AverageBase {
  public:
    Average();
    Average(int slots);
    ~Average() override;

    void Init(int slots);
    void Reset(float fValue);
    void Record(float fValue);

    // Decl: 206
    float GetValue() const {
        return this->fAverage;
    }

    // Decl: 207
    float GetTotal() const {
        return this->fTotal;
    }

    void Flush(float fValue);
    void Recalculate() override;
    float GetLastRecordedValue() const;

  protected:
    float fTotal;   // Decl: 214
    float fAverage; // Decl: 215
    float *pData;   // Decl: 216

  private:
    float SmallDataBuffer[5]; // Decl: 219
};

// total size: 0x38
// Decl: 224
class AverageWindow : public Average {
  public:
    AverageWindow(float f_timewindow, float f_frequency);
    ~AverageWindow();

    void Record(float fValue, float fTimeNow);
    void Reset(float fValue);
    void DoSnapshot();
    float GetOldestValue();
    float GetOldestTime();

    float fTimeWindow; // Decl: 239
    int iOldestValue;  // Decl: 242
    float *pTimeData;  // Decl: 243
    uint32 AllocSize;  // Decl: 244
};

// total size: 0x8
// Decl: 334
class Linear {
  public:
    Linear() {}  // Decl: 337
    ~Linear() {} // Decl: 338

    void Init(float x0, float y0, float x1, float y1); // Decl: 340
    float GetValue(float x);                           // Decl: 341
    float GetInverse(float y);                         // Decl: 342
    float GetDerivative() {}                           // Decl: 343

  private:
    float m, b; // Decl: 347
};

// total size: 0x84
// Decl: 354
class PidError {
  public:
    USE_FASTALLOC(PidError);
    PidError(int nIntegralTerms, int nDerivativeTerms, float f_frequency)
        : aTimes(nIntegralTerms),        //
          aIntegral(nIntegralTerms),     //
          aDerivative(nDerivativeTerms), //
          fFrequency(f_frequency),       //
          fCurrentError(0.0f),           //
          fPreviousError(0.0f) {}

    ~PidError() {}

    float GetError() {
        return fCurrentError;
    }

    // Decl: 370
    float GetErrorIntegral() {
        int n_samples = this->aIntegral.GetNumSamples();
        if (n_samples != 0) {
            return (this->aIntegral.GetTotal() * (float)(int)this->aIntegral.GetNumSamples()) / (this->fFrequency * this->aTimes.GetTotal());
        } else {
            return 0.0f;
        }
    }

    // Decl: 371
    float GetErrorDerivative() {
        return aDerivative.GetValue();
    }

    // float GetErrorInstaneousDerivative() {}

    void Reset(float fCalibrate);
    void ResetIntegral(float fCalibrate);
    void ResetDerivative(float fCalibrate);
    void Record(float fError, float fTime, bool bZeroDerivative, bool bZeroIntegral);
    void DoSnapshot(ReplaySnapshot *snapshot);

  private:
    Average aTimes;      // offset 0x0, size 0x28, Decl: 381
    Average aIntegral;   // offset 0x28, size 0x28, Decl: 382
    Average aDerivative; // offset 0x50, size 0x28, Decl: 383

    float fFrequency;     // offset 0x78, size 0x4, Decl: 385
    float fCurrentError;  // offset 0x7C, size 0x4, Decl: 386
    float fPreviousError; // offset 0x80, size 0x4, Decl: 387
};

#endif
