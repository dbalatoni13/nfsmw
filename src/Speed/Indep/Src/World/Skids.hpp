#ifndef WORLD_SKIDS_H
#define WORLD_SKIDS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Car.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

class Clan;
extern SlotPool *SkidSetSlotPool;

// total size: 0x10
class SkidSegment {
  public:
    bVector3 *GetPosition() {
        return reinterpret_cast<bVector3 *>(Position);
    }

    void SetIntensity(unsigned char intensity) {
        Intensity = intensity;
    }

    unsigned char GetIntensity() {
        return Intensity;
    }

    void SetPoints(bVector3 *position, bVector3 *delta_position);
    void GetPoints(bVector3 *position, bVector3 *delta_position);
    void GetEndPoints(bVector3 *left_point, bVector3 *right_point);

  private:
    float Position[3];            // offset 0x0, size 0xC
    signed char DeltaPosition[3]; // offset 0xC, size 0x3
    unsigned char Intensity;      // offset 0xF, size 0x1
};

class SkidSet;

// total size: 0x4
class SkidMaker {
  public:
    void MakeSkid(Car *pCar, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity);
    void MakeNoSkid();

    friend class SkidSet;

  private:
    SkidSet *pSkidSet; // offset 0x0, size 0x4
};

// total size: 0xF0
class SkidSet : public bTNode<SkidSet> {
  public:
    void *operator new(size_t size) {
        return bMalloc(SkidSetSlotPool);
    }

    void operator delete(void *ptr) {
        bFree(SkidSetSlotPool, ptr);
    }

    SkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity);
    ~SkidSet();

    void FinishedAddingSkids();
    void Render(eView *view, unsigned char alpha);

    bVector3 *GetBBoxMax() {
        return &BBoxMax;
    }

    bVector3 *GetBBoxMin() {
        return &BBoxMin;
    }

    bVector3 *GetBBoxCentre() {
        return &BBoxCentre;
    }

  private:
    int AddSegment(bVector3 *position, bVector3 *delta_position, bool skid_is_flaming, float intensity);

    int GetTerrainType() {
        return TheTerrainType;
    }

    void GetLastPoints(bVector3 *position, bVector3 *delta_position) {
        SkidSegments[NumSkidSegments - 1].GetPoints(position, delta_position);
    }

    float GetLastIntensity() {
        return static_cast<float>(SkidSegments[NumSkidSegments - 1].GetIntensity()) * (1.0f / 255.0f);
    }

    friend class SkidMaker;

    bVector3 LastNormal;         // offset 0x8, size 0x10
    float LastSegmentLength;     // offset 0x18, size 0x4
    Clan *pClan;                 // offset 0x1C, size 0x4
    bPNode *pClanNode;           // offset 0x20, size 0x4
    SkidMaker *pSkidMaker;       // offset 0x24, size 0x4
    int TheTerrainType;          // offset 0x28, size 0x4
    bVector3 Position;           // offset 0x2C, size 0x10
    bVector3 BBoxMax;            // offset 0x3C, size 0x10
    bVector3 BBoxMin;            // offset 0x4C, size 0x10
    SkidSegment SkidSegments[8]; // offset 0x5C, size 0x80
    int NumSkidSegments;         // offset 0xDC, size 0x4
    bVector3 BBoxCentre;         // offset 0xE0, size 0x10
};

SkidSet *CreateNewSkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity);
void InitSkids(int max_skids);
void CloseSkids();
void DeleteThisSkid(SkidSet *skid_set);
void DeleteAllSkids();
void RenderSkids(eView *view, Clan *clan);

#endif
