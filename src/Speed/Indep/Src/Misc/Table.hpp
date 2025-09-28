#ifndef MISC_TABLE_H
#define MISC_TABLE_H

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

    virtual ~AverageBase() {}

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
    float GetOldestValue() {
        return pData[iOldestValue];
    }

    float GetOldestTimeValue() {
        return pTimeData[iOldestValue];
    }

    void Record(const float fValue, const float fTimeNow);

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
        const int length = NumEntries;

        if (length > 1) {
            if (x <= GraphData[0].x) {
                *pValue = GraphData[0].y;
            } else if (x >= GraphData[length - 1].x) {
                *pValue = GraphData[length - 1].y;
            } else {
                int i = 0;
                const int maxInd = length - 1;
                if (maxInd > 0) {
                    GraphEntry<T> *p = GraphData;
                    while (!(x >= p->x) || !(x < p[1].x)) {
                        ++i;
                        ++p;
                        if (i >= maxInd) {
                            return;
                        }
                    }
                    const T blend = (x - GraphData[i].x) / (GraphData[i + 1].x - GraphData[i].x);
                    Blend(pValue, &GraphData[i + 1].y, &GraphData[i].y, blend);
                }
            }
        } else if (length > 0) {
            *pValue = GraphData[0].y;
        }
    }

  private:
    GraphEntry<T> *GraphData; // offset 0x0, size 0x4
    int NumEntries;           // offset 0x4, size 0x4
};

#endif
