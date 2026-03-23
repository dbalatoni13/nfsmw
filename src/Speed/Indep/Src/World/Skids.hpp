#ifndef WORLD_SKIDS_H
#define WORLD_SKIDS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Clans.hpp"
#include "Car.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct eView;
extern SlotPool *SkidSetSlotPool;

class SkidSegment {
  private:
    // total size: 0x10
    float Position[3];          // offset 0x0, size 0xC
    signed char DeltaPosition[3]; // offset 0xC, size 0x3
    unsigned char Intensity;    // offset 0xF, size 0x1

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
};

struct SkidMaker {
    // total size: 0x4
    struct SkidSet *pSkidSet; // offset 0x0, size 0x4

    void MakeSkid(Car *pCar, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity);
    void MakeNoSkid();
};

struct SkidSet : public bTNode<SkidSet> {
    // total size: 0xF0
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

    void *operator new(size_t size) {
        return bMalloc(SkidSetSlotPool);
    }
    void operator delete(void *ptr) {
        bFree(SkidSetSlotPool, ptr);
    }

    SkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity);
    ~SkidSet();

    int AddSegment(bVector3 *position, bVector3 *delta_position, bool skid_is_flaming, float intensity);
    void FinishedAddingSkids();
    void Render(eView *view, unsigned char alpha);
    int GetTerrainType() {
        return TheTerrainType;
    }
    bVector3 *GetBBoxMax() {
        return &BBoxMax;
    }
    bVector3 *GetBBoxMin() {
        return &BBoxMin;
    }
    bVector3 *GetBBoxCentre() {
        return &BBoxCentre;
    }
    void GetLastPoints(bVector3 *position, bVector3 *delta_position) {
        SkidSegments[NumSkidSegments - 1].GetPoints(position, delta_position);
    }
    float GetLastIntensity() {
        return static_cast<float>(SkidSegments[NumSkidSegments - 1].GetIntensity()) * (1.0f / 255.0f);
    }
};

SkidSet *CreateNewSkidSet(SkidMaker *skid_maker, bVector3 *position, bVector3 *delta_position, int terrain_type, float intensity);
void InitSkids(int max_skids);
void CloseSkids();
void DeleteThisSkid(SkidSet *skid_set);
void DeleteAllSkids();
void RenderSkids(eView *view, Clan *clan);

#endif
