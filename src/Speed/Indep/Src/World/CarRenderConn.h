#ifndef WORLD_CARRENDERCONN_H
#define WORLD_CARRENDERCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"

struct RoadNoiseRecord;
struct TireState;

typedef unsigned int PartState[3];

namespace RenderConn {
class Pkt_Car_Open;
class Pkt_Car_Service;
}

class CarRenderConn : public VehicleRenderConn, public IAttributeable {
  public:
    enum eFlag {
        CF_HIDDEN = 1,
        CF_MISSSHIFT = 2,
        CF_ISPLAYER = 4,
        CF_ISRAINING = 8,
        CF_BLOWOFF = 16,
    };

    typedef bTList<VehicleRenderConn::Effect> PipeEffects;
    typedef bTList<VehicleRenderConn::Effect> EngineEffects;

    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    CarRenderConn(const Sim::ConnectionData &data, CarType ct, RenderConn::Pkt_Car_Open *oc);
    ~CarRenderConn() override;

    void OnAttributeChange(const Attrib::Collection *collection, unsigned int attribkey) override;
    bool TestVisibility(float distance);
    void OnEvent(EventID id) override;
    void OnFetch(float dT) override;
    void OnLoaded(CarRenderInfo *carrender_info) override;
    void GetRenderMatrix(bMatrix4 *matrix) override;
    void OnRender(eView *view, int reflection) override;

  private:
    void UpdateSteering(float dT, const RenderConn::Pkt_Car_Service &data);
    void UpdateParts(float dT, const RenderConn::Pkt_Car_Service &data);
    void AddRoadNoise(float speed, unsigned int tires, const RoadNoiseRecord &noise);
    void UpdateRoadNoise(float dT, float carspeed, const RenderConn::Pkt_Car_Service &data);
    void UpdateEngineAnimation(float dT, const RenderConn::Pkt_Car_Service &data);
    void UpdateBodyAnimation(float dT, const RenderConn::Pkt_Car_Service &data);
    void UpdateContrails(const RenderConn::Pkt_Car_Service &data, float dT);
    void UpdateTires(float dT, float carspeed, const RenderConn::Pkt_Car_Service &data);
    void UpdateEffects(const RenderConn::Pkt_Car_Service &data, float dT);
    void Update(const RenderConn::Pkt_Car_Service &data, float dT);
    void BuildRenderMatrix(float dT);
    void UpdateRenderMatrix(float dT);
    void Hide(bool b);

    bool GetFlag(eFlag flag) const {
        return (this->mFlags & flag) != 0;
    }

    void SetFlag(eFlag flag, bool on) {
        if (on) {
            this->mFlags |= flag;
        } else {
            this->mFlags &= ~flag;
        }
    }

    TireState *mTireState[4];          // offset 0x68, size 0x10
    bVector4 mTirePositions[4];        // offset 0x78, size 0x40
    float mTireRadius[4];              // offset 0xB8, size 0x10
    float mPhysicsRadius[4];           // offset 0xC8, size 0x10
    Attrib::Gen::ecar mAttributes;     // offset 0xD8, size 0x14
    Attrib::Gen::pvehicle mPhysics;    // offset 0xEC, size 0x14
    Attrib::Gen::tires mTirePhysics;   // offset 0x100, size 0x14
    bMatrix4 mTireMatrices[4];         // offset 0x114, size 0x100
    bMatrix4 mBrakeMatrices[4];        // offset 0x214, size 0x100
    bMatrix4 mRenderMatrix;            // offset 0x314, size 0x40
    UMath::Vector2 mExtraBodyAngle;    // offset 0x354, size 0x8
    UMath::Vector3 mFlatTireAngle;     // offset 0x35C, size 0xC
    UMath::Vector3 mWheelHop;          // offset 0x368, size 0xC
    UMath::Vector3 mRoadNoise;         // offset 0x374, size 0xC
    float mEnginePower;                // offset 0x380, size 0x4
    float mAnimTime;                   // offset 0x384, size 0x4
    float mShiftPitchAngle;            // offset 0x388, size 0x4
    float mEngineTorqueAngle;          // offset 0x38C, size 0x4
    float mEngineVibrationAngle;       // offset 0x390, size 0x4
    float mEnginePitchAngle;           // offset 0x394, size 0x4
    float mPerfectLaunchTimer;         // offset 0x398, size 0x4
    float mMaxWheelRenderDeltaAngle;   // offset 0x39C, size 0x4
    PartState mPartState;              // offset 0x3A0, size 0xC
    unsigned int mLastRenderFrame;     // offset 0x3AC, size 0x4
    unsigned int mLastVisibleFrame;    // offset 0x3B0, size 0x4
    float mDistanceToView;             // offset 0x3B4, size 0x4
    float mFlashTime;                  // offset 0x3B8, size 0x4
    float mShifting;                   // offset 0x3BC, size 0x4
    float mSteering[2];                // offset 0x3C0, size 0x8
    PipeEffects mPipeEffects;          // offset 0x3C8, size 0x8
    EngineEffects mEngineEffects;      // offset 0x3D0, size 0x8
    bool mDoContrailEffect;            // offset 0x3D8, size 0x1
    CarRenderUsage mUsage;             // offset 0x3DC, size 0x4
    unsigned int mFlags;               // offset 0x3E0, size 0x4
};

#endif
