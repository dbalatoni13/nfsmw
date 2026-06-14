#ifndef WORLD_HPP
#define WORLD_HPP

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Misc/Replay.hpp"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/Sim/OBB.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// STRIPPED, it's there in HP2
struct TrafficTeleporter {};
// STRIPPED, it's there in HP2
struct DriveTarget {};

class World;

extern World *pCurrentWorld;

// total size: 0xE8
class World {
  public:
    World();
    ~World();

    void DebugDisplay();

    void DoSnapshot(ReplaySnapshot *snapshot);

    void BeginPause();
    void EndPause();

    void BeginOnlinePause();
    void EndOnlinePause();

    void UpdateWorldPaused();
    int IsWorldPaused();
    float GetTimestep();
    void DoTimestep(float timestep);

    unsigned int Random(int range);
    float Random(float range);
    unsigned int ConstantRandom(int range, uint32 seed);
    float ConstantRandom(float range, uint32 seed);

    int GetNumCars() {}
    int GetNumRacingCars() {}
    int GetNumPlayerCars() {}
    int GetNumAICars() {}
    int GetNumCopCars() {}
    int GetNumTrafficCars() {}
    int GetNumParkedCars() {}
    Car *GetCar(int index) {}
    Car *GetRacingCar(int index) {}
    Car *GetPlayerCar(int index) {}
    Car *GetAICar(int index) {}
    Car *GetCopCar(int index) {}
    Car *GetTrafficCar(int index) {}
    Car *GetParkedCar(int index) {}
    void BuildCarTable();

    Car *GetCarByNumber(int driver_number);

    void DoPointerSnapshot(ReplaySnapshot *snapshot, Car **ppcar);
    void DoCarPointerSnapshot(Car **ppcar, ReplaySnapshot *snapshot) {}

    Timer &GetTimer() {}

    float GetElapsedTime(int32 start_time, int32 end_time) {}
    float GetElapsedTime(int32 start_time) {}

    void SetTimeScale(float t, float d, void (*func)(int), int param);
    void ResetTimeScale();
    float GetTimeScale() {}
    void SetWorldPaused(float t, void (*func)(int), int param);

    uint16 CalculateDebugChecksum();
    void VerifyDebugChecksums();

    float GetDistanceToClosestPlayer(const bVector2 *point);
    float GetDistanceToClosestPlayer(const bVector3 *point);

    int IsLocationVisibleToPlayer(const bVector2 *point) {}
    int IsDATVisibleToPlayer(float distance_around_track) {}

    TrafficTeleporter *GetTrafficTeleporter() {}

    bool AddTrafficCar(DriverInfo *d);

    bTList<Car> CarList; // offset 0x0, size 0x8

    void IncrementCollisionPredictionID();
    uint32 GetCurrentCollisionPredictionID() {}
    void ResetCollisionPredictionID() {
        this->CollisionPredictionID = 1;
    }
    void CollisionPredictionSetOBBs(OBB *my_obbs, OBB *other_car_obbs);
    void CollisionPredictionSetDriveTargets(DriveTarget *my_drive_target, DriveTarget *other_car_drive_target);
    void CollisionPredictionRecordIteration(int loop_counter, int8 my_index, int8 other_car_index, bVector2 *my_position,
                                            bVector2 *other_car_position);
    void IncrementPotentialDriveTargetID() {}
    uint32 GetCurrentPotentialDriveTargetID() {}
    void ResetPotentialDriveTargetID() {
        this->PotentialDriveTargetID = 1;
    }

    void RecordPotentialDriveTarget(DriveTarget *drive_target);

    void UpdateAIDebugRendering();

    void UpdateAIDebugStuff();

  private:
    void DoTimestepMove(float timestep);
    void DoTimestepCollisions(float timestep);

    int TotalNumCars;               // offset 0x8, size 0x4
    int AICarPos;                   // offset 0xC, size 0x4
    int CopCarPos;                  // offset 0x10, size 0x4
    int TrafficCarPos;              // offset 0x14, size 0x4
    int ParkedCarPos;               // offset 0x18, size 0x4
    Car *CarTable[10];              // offset 0x1C, size 0x28
    bVector3 PlayerPositions[6];    // offset 0x44, size 0x60
    float PlayerDATs[6];            // offset 0xA4, size 0x18
    uint32 WorldRandomSeed;         // offset 0xBC, size 0x4
    float fTimeToRemainPaused;      // offset 0xC0, size 0x4
    int OnlinePauseWorld;           // offset 0xC4, size 0x4
    float PreviousTimeToWaste;      // offset 0xC8, size 0x4
    float fTimeScale;               // offset 0xCC, size 0x4
    float fTimeScaleTimer;          // offset 0xD0, size 0x4
    void (*TimeScaleCallback)(int); // offset 0xD4, size 0x4
    int32 TimeScaleCallbackParam;   // offset 0xD8, size 0x4
    TrafficTeleporter *Teleporter;  // offset 0xDC, size 0x4
    uint32 CollisionPredictionID;   // offset 0xE0, size 0x4
    uint32 PotentialDriveTargetID;  // offset 0xE4, size 0x4
};

extern int g_tweakIsDriftRace; // TODO move?

void World_Service();
void World_DEBUGStartLocation(UMath::Vector3 &startLoc, UMath::Vector3 &initialVec);

#endif
