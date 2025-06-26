#pragma once

extern class Activity {};
extern class ITrafficMgr {};
extern class IVehicleCache {};
extern class Debugable {};
extern class _type_TrafficList {};
extern class IVehicle {};
template <class T, class U> extern struct list {};
extern enum eTrafficDensity {};
extern struct PatternMap {};
extern struct WRoadNav {};
extern struct trafficpattern {};
extern class Param {};

class AITrafficManager : public Activity, public ITrafficMgr, public IVehicleCache, public Debugable {
    // total size: 0x3C4
    struct HSIMTASK__ *mTask;                             // offset 0x68, size 0x4
    unsigned int mSpawnIdx;                               // offset 0x6C, size 0x4
    float mPatternTimer[10];                              // offset 0x70, size 0x28
    float mNewInstanceTimer;                              // offset 0x98, size 0x4
    struct list<IVehicle *, _type_TrafficList> mVehicles; // offset 0x9C, size 0x8
    struct ActionQueue *mActionQ;                         // offset 0xA4, size 0x4
    enum eTrafficDensity mDensity;                        // offset 0xA8, size 0x4
    struct PatternMap mPatternMap;                        // offset 0xAC, size 0x10
    struct WRoadNav mNav;                                 // offset 0xBC, size 0x2F0
    float mOncommingChance;                               // offset 0x3AC, size 0x4
    struct trafficpattern mPattern;                       // offset 0x3B0, size 0x14

    AITrafficManager(Param params);
};
