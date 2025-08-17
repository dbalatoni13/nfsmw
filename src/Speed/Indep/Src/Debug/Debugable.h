#ifndef DEBUG_DEBUGABLE_H
#define DEBUG_DEBUGABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum eDebugableClass {
    DBG_RIGIDBODY = 0,
    DBG_AI = 1,
    DBG_WROADNETWORK = 2,
    DBG_WRACELINE = 3,
    DBG_PHYSICS_DIMENSIONS = 4,
    DBG_PHYSICS_RACERS = 5,
    DBG_PHYSICS_AERODYNAMICS = 6,
    DBG_ARTICULATION = 7,
    DBG_SKELETON = 8,
    DBG_ONLINE = 9,
    DBG_PLAYER = 10,
    DBG_ROAD_EDITOR = 11,
    DGB_TRAFFIC_MANAGER = 12,
    DBG_AIAVOIDABLE = 13,
    DBG_CREW = 14,
    DBG_RACE = 15,
    DBG_DRIFT = 16,
    DBG_RULER = 17,
    DBG_DRAFT_ZONE = 18,
    DBG_MAX = 19,
};

class Debugable {
  protected:
    Debugable() {}
    void MakeDebugable(eDebugableClass) {}

  public:
    // static void SetState(eDebugableClass value) {}

    // static void DoDebugDraws() {}

    // static bool GetDebugState(eDebugableClass) {}
};

#endif
