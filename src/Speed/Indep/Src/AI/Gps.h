#ifndef AI_GPS_H
#define AI_GPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

struct eView;
struct eModel;

// total size: 0x388
struct Gps : Sim::Activity, UTL::Collections::Singleton<Gps> {
    enum eGPSState {
        GPS_DOWN = 0,
        GPS_SEARCHING = 1,
        GPS_TRACKING = 2,
    };

    static Sim::IActivity *Construct(Sim::Param params);

    Gps();
    ~Gps() override;

    bool OnTask(HSIMTASK htask, float dT) override;
    void Update(float dT);
    bool Engage(const UMath::Vector3 &target, float maxDeviation);
    void Render(eView *view);

    bool IsEngaged() const {
        return mState != GPS_DOWN;
    }

    void Disengage() {
        mState = GPS_DOWN;
    }

private:
    UMath::Vector3 mTarget;         // offset 0x50, size 0xC
    HSIMTASK mTask;                 // offset 0x5C, size 0x4
    UMath::Vector3 mPosition;       // offset 0x60, size 0xC
    eModel *mArrowModel;            // offset 0x6C, size 0x4
    UMath::Vector3 mDirection;      // offset 0x70, size 0xC
    eGPSState mState;               // offset 0x7C, size 0x4
    WRoadNav mRoadNav;              // offset 0x80, size 0x2F0
    float mPathDist;                // offset 0x370, size 0x4
    bool mDrawn;                    // offset 0x374, size 0x1
    float mAngle;                   // offset 0x378, size 0x4
    float mScale;                   // offset 0x37C, size 0x4
    float mMaxDeviation;            // offset 0x380, size 0x4
    float mDeviation;               // offset 0x384, size 0x4
};

void GPS_Disengage();
bool GPS_Engage(const UMath::Vector3 &target, float maxDeviation);
bool GPS_IsEngaged();
void RenderGpsArrows(eView *view);

#endif
