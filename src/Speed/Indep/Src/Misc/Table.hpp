#ifndef MISC_TABLE_H
#define MISC_TABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstddef>

// total size: 0x10
class TableBase {
  public:
    TableBase(int num, float min, float max) {
        NumEntries = num;
        SetMinMax(min, max);
    }

    void SetMinMax(float fMin, float fMax) {
        MinArg = fMin;
        MaxArg = fMax;
        CalcIndexMultiplier();
    }

    void CalcIndexMultiplier() {
        IndexMultiplier = (NumEntries - 1) / (MaxArg - MinArg);
    }

  protected:
    int NumEntries;        // offset 0x0, size 0x4
    float MinArg;          // offset 0x4, size 0x4
    float MaxArg;          // offset 0x8, size 0x4
    float IndexMultiplier; // offset 0xC, size 0x4
};

class Table : public TableBase {
  public:
    float GetValue(float arg);
    float InverseLookup(float value);

    Table(const float *table, int num, float min, float max) : TableBase(num, min, max), pTable(table) {}

    const float *GetData() const {}

    void SetData(const float *data, int num) {}

  private:
    // total size: 0x14
    const float *pTable; // offset 0x10, size 0x4
};

template <typename T> class tTable : public TableBase {
    T *pTable;

  public:
};

class AverageBase {
  public:
    static void *operator new(std::size_t size, void *ptr) {}

    static void operator delete(void *mem, void *ptr) {}

    static void *operator new(std::size_t size) {}

    static void operator delete(void *mem, std::size_t size) {}

    static void *operator new(std::size_t size, const char *name) {}

    static void operator delete(void *mem, const char *name) {}

    static void operator delete(void *mem, std::size_t size, const char *name) {}

    AverageBase(int size, int slots);
    virtual ~AverageBase() {}

    void *Allocate(unsigned int size, const char *name);
    void DeAllocate(void *ptr, unsigned int size, const char *name);

    // bool FullySampled() {}

    unsigned char GetNumSamples() {
        return nSamples;
    }

    virtual void Recalculate() {}

  protected:
    unsigned char nSize;
    unsigned char nSlots;
    unsigned char nSamples;
    unsigned char nCurrentSlot;
};

class Average : public AverageBase {
  public:
    Average();
    Average(int slots);
    ~Average();

    void Init(int slots);

    float GetValue() {
        return fAverage;
    }

    float GetTotal() {
        return fTotal;
    }

  protected:
    float fTotal;
    float fAverage;
    float *pData;

  private:
    float SmallDataBuffer[5];
};

class AverageWindow : public Average {
  public:
    AverageWindow(float f_timewindow, float f_frequency);
    ~AverageWindow();

    float GetOldestValue() {
        return pData[iOldestValue];
    }

    float GetOldestTimeValue() {
        return pTimeData[iOldestValue];
    }

    void Record(const float fValue, const float fTimeNow);
    void Reset(float fValue);

    float fTimeWindow;
    int iOldestValue;
    float *pTimeData;
    unsigned int AllocSize;
};

class Graph {
  public:
    static void *operator new(std::size_t size, void *ptr) {}

    static void *operator new(std::size_t size) {}

    static void operator delete(void *mem, void *ptr) {}

    static void operator delete(void *mem, std::size_t size) {}

    Graph(bVector2 *points, int num_points) {
        Points = points;
        NumPoints = num_points;
    }

    float GetValue(float x);

  private:
    bVector2 *Points; // offset 0x0, size 0x4
    int NumPoints;    // offset 0x4, size 0x4
};

template <typename T> struct GraphEntry {
    T x;
    T y;
};

template <typename T> class tGraph {
  public:
    tGraph(GraphEntry<T> *data, int num) {
        GraphData = data;
        NumEntries = num;
    }
    void Blend(T *dest, T *a, T *b, const T blend_a);

    // Credits: Brawltendo
    // UNSOLVED
    void GetValue(T *pValue, T x) {
        if (NumEntries > 1) {
            if (x <= GraphData[0].x) {
                bMemCpy(pValue, &GraphData[0].y, sizeof(float));
            } else if (x >= GraphData[NumEntries - 1].x) {
                bMemCpy(pValue, &GraphData[NumEntries - 1].y, sizeof(float));
            } else {
                for (int i = 0; i < NumEntries - 1; ++i) {
                    if (x >= GraphData[i].x && x < GraphData[i + 1].x) {
                        const T blend = (x - GraphData[i].x) / (GraphData[i + 1].x - GraphData[i].x);
                        Blend(pValue, &GraphData[i + 1].y, &GraphData[i].y, blend);
                        return;
                    }
                }
            }
        } else if (NumEntries > 0) {
            bMemCpy(pValue, &GraphData[0].y, sizeof(float));
        }
    }

    float GetValue(T x) {
        float ret;
        GetValue(&ret, x);
        return ret;
    }

  private:
    GraphEntry<T> *GraphData; // offset 0x0, size 0x4
    int NumEntries;           // offset 0x4, size 0x4
};

// total size: 0x84
struct PidError {
    // void *operator new(unsigned int size, void *ptr) {}

    // void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    void *operator new(size_t size, const char *name) {
        return gFastMem.Alloc(size, name);
    }

    // void operator delete(void *mem, const char *name) {}

    // void operator delete(void *mem, unsigned int size, const char *name) {}

    PidError(int nIntegralTerms, int nDerivativeTerms, float f_frequency)
        : aTimes(nIntegralTerms),        //
          aIntegral(nIntegralTerms),     //
          aDerivative(nDerivativeTerms), //
          fFrequency(f_frequency),       //
          fCurrentError(0.0f),           //
          fPreviousError(0.0f) {}

    ~PidError() {}

    void Record(float fError, float fTime, bool bZeroDerivative, bool bZeroIntegral);
    void DoSnapshot(ReplaySnapshot *snapshot);
    void Reset(float fCalibrate);
    void ResetIntegral(float fCalibrate);
    void ResetDerivative(float fCalibrate);

    // float GetError() {}

    float GetErrorIntegral() {
        int n_samples = aIntegral.GetNumSamples();
        if (n_samples != 0) {
            return (aIntegral.GetTotal() * (float)(int)aIntegral.GetNumSamples()) / (fFrequency * aTimes.GetTotal());
        } else {
            return 0.0f;
        }
    }

    float GetErrorDerivative() {
        return aDerivative.GetValue();
    }

    // float GetErrorInstaneousDerivative() {}

  private:
    Average aTimes;       // offset 0x0, size 0x28
    Average aIntegral;    // offset 0x28, size 0x28
    Average aDerivative;  // offset 0x50, size 0x28
    float fFrequency;     // offset 0x78, size 0x4
    float fCurrentError;  // offset 0x7C, size 0x4
    float fPreviousError; // offset 0x80, size 0x4
};

#endif
