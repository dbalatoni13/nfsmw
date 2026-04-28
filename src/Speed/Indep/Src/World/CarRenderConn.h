#ifndef WORLD_CARRENDERCONN_H
#define WORLD_CARRENDERCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UBitArray.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/tires.h"
#include "Speed/Indep/Src/Interfaces/IAttributeable.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct RoadNoiseRecord;
struct TireState;

typedef BitArray<unsigned int, 76> PartState;

namespace RenderConn {
class Pkt_Car_Open : public Sim::Packet {
  public:
    unsigned int mModelHash;                         // offset 0x4, size 0x4
    unsigned int mWorldID;                           // offset 0x8, size 0x4
    CarRenderUsage mUsage;                           // offset 0xC, size 0x4
    const FECustomizationRecord *mCustomizations;    // offset 0x10, size 0x4
    unsigned int mPhysicsKey;                        // offset 0x14, size 0x4
    bool mSpoolLoad;                                 // offset 0x18, size 0x1
    unsigned char _pad19[3];                         // offset 0x19, size 0x3
};

class Pkt_Car_Service : public Sim::Packet {
  public:
    Pkt_Car_Service(bool inview, float distancetoview) {
        unsigned int i;

        this->mDamageInfo = 0;

        *reinterpret_cast<unsigned int *>(&this->mFlashing) = 0;
        bMemSet(this->mCompressions, 0, 0x10);
        bMemSet(this->mSteering, 0, 8);
        bMemSet(this->mWheelSpeed, 0, 0x10);
        bMemSet(this->mTireSkid, 0, 0x10);
        bMemSet(this->mTireSlip, 0, 0x10);
        this->mAnimatedCarShake = 0.0f;
        *reinterpret_cast<unsigned int *>(&this->mInView) = inview;
        this->mDistanceToView = distancetoview;
        this->mShift = 1;
        this->mHealth = 1.0f;
        this->mBlowOuts = 0;
        this->mGroundState = 0;
        this->mLights = 0;
        this->mBrokenLights = 0;
        *reinterpret_cast<unsigned int *>(&this->mNos) = 0;
        *reinterpret_cast<unsigned int *>(&this->mEngineBlown) = 0;
        this->mGear = 1;
        this->mEnginePower = 0.0f;
        this->mExtraBodyRoll = 0.0f;
        this->mExtraBodyPitch = 0.0f;
        this->mEngineSpeed = 0.0f;
        this->mAnimatedCarPitch = 0.0f;
        this->mAnimatedCarRoll = 0.0f;
    }

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("CarRenderConn");
        return hash;
    }

    unsigned int Size() override {
        return 0xA8;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_Car_Service");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

    void HidePart(const UCrc32 &partname);

    float mCompressions[4];              // offset 0x4, size 0x10
    float mWheelSpeed[4];                // offset 0x14, size 0x10
    float mTireSkid[4];                  // offset 0x24, size 0x10
    float mTireSlip[4];                  // offset 0x34, size 0x10
    float mSteering[2];                  // offset 0x44, size 0x8
    int mGroundState;                    // offset 0x4C, size 0x4
    unsigned int mDamageInfo;            // offset 0x50, size 0x4
    PartState mPartState;                // offset 0x54, size 0xC
    unsigned int mLights;                // offset 0x60, size 0x4
    unsigned int mBrokenLights;          // offset 0x64, size 0x4
    bool mInView;                        // offset 0x68, size 0x4
    float mDistanceToView;               // offset 0x6C, size 0x4
    bool mFlashing;                      // offset 0x70, size 0x4
    bool mNos;                           // offset 0x74, size 0x4
    bool mEngineBlown;                   // offset 0x78, size 0x4
    int mShift;                          // offset 0x7C, size 0x4
    int mGear;                           // offset 0x80, size 0x4
    float mEnginePower;                  // offset 0x84, size 0x4
    float mEngineSpeed;                  // offset 0x88, size 0x4
    float mExtraBodyRoll;                // offset 0x8C, size 0x4
    float mExtraBodyPitch;               // offset 0x90, size 0x4
    int mBlowOuts;                       // offset 0x94, size 0x4
    float mHealth;                       // offset 0x98, size 0x4
    float mAnimatedCarPitch;             // offset 0x9C, size 0x4
    float mAnimatedCarRoll;              // offset 0xA0, size 0x4
    float mAnimatedCarShake;             // offset 0xA4, size 0x4
};
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
