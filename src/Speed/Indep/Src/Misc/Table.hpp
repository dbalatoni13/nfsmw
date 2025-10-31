#ifndef MISC_TABLE_H
#define MISC_TABLE_H

#include "Speed/Indep/bWare/Inc/bWare.hpp"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <cstddef>

class TableBase {
  protected:
    // total size: 0x10
    int NumEntries;        // offset 0x0, size 0x4
    float MinArg;          // offset 0x4, size 0x4
    float MaxArg;          // offset 0x8, size 0x4
    float IndexMultiplier; // offset 0xC, size 0x4
};

class Table : public TableBase {

  public:
    float GetValue(float arg);
    float InverseLookup(float value);

    Table() {}

    Table(const float *table, int num, float min, float max) {}

    const float *GetData() const {}

    void SetData(const float *data, int num) {}

  protected:
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

    bool FullySampled() {}

    unsigned char GetNumSamples() {}

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
    struct bVector2 *Points; // offset 0x0, size 0x4
    int NumPoints;           // offset 0x4, size 0x4
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

#endif
