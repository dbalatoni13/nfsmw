#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/CarRenderConn.h"
#include "Speed/Indep/Src/World/DamageZones.h"
#include "Speed/Indep/Src/World/Scenery.hpp"
#include "Speed/Indep/Src/World/SmackableRender.hpp"
#include "Speed/Indep/Src/World/WorldTypes.h"

#define RENDER_SERVICE UCrc32(UCRC32_ECSTASY)
#define DECLARE_RENDERPACKET(_PKT_, _HANDLER_)                                                                                                       \
    friend class ::_HANDLER_;                                                                                                                        \
    UCrc32 ConnectionClass() override {                                                                                                              \
        static UCrc32 hash(#_HANDLER_);                                                                                                              \
        return hash;                                                                                                                                 \
    }                                                                                                                                                \
    DECLARE_SIMPACKET(_PKT_, #_PKT_)

class CarRenderConn;
class HeliRenderConn;
class VehicleFragmentConn;

namespace RenderConn {

enum eTireIdx {
    // The front left wheel
    TIRE_FL,
    // The front right wheel
    TIRE_FR,
    // The number of wheels on the front axle
    TIRE_MAX_FRONT = 2,
    // The rear right wheel
    TIRE_RR = 2,
    // The rear left wheel
    TIRE_RL,
    // The total number of wheels
    TIRE_MAX,
};

class Pkt_Car_Open : public Sim::Packet {
  public:
    DECLARE_RENDERPACKET(Pkt_Car_Open, CarRenderConn);

    Pkt_Car_Open(const char *modelname, WUID worldid, CarRenderUsage usage, const FECustomizationRecord *customizations, unsigned int physicskey,
                 bool spool_load)
        : mModelHash(bStringHash(modelname)), //
          mWorldID(worldid), mUsage(usage),   //
          mCustomizations(customizations),    //
          mPhysicsKey(physicskey),            //
          mSpoolLoad(spool_load) {}

    uint32 mModelHash;                            // offset 0x4, size 0x4
    WUID mWorldID;                                // offset 0x8, size 0x4
    CarRenderUsage mUsage;                        // offset 0xC, size 0x4
    const FECustomizationRecord *mCustomizations; // offset 0x10, size 0x4
    Attrib::Key mPhysicsKey;                      // offset 0x14, size 0x4
    bool mSpoolLoad;                              // offset 0x18, size 0x1
};

class Pkt_Car_Service : public Sim::Packet {
  public:
    typedef UTL::Std::list<UCrc32, _type_list> PartList;

    Pkt_Car_Service(bool inview, float distancetoview) {
        this->mFlashing = false;

        bMemSet(this->mCompressions, 0, sizeof(this->mCompressions));
        bMemSet(this->mSteering, 0, sizeof(this->mSteering));
        bMemSet(this->mWheelSpeed, 0, sizeof(this->mWheelSpeed));
        bMemSet(this->mTireSkid, 0, sizeof(this->mTireSkid));
        bMemSet(this->mTireSlip, 0, sizeof(this->mTireSlip));

        this->mGroundState = 0;
        this->mLights = 0;
        this->mBrokenLights = 0;
        this->mInView = inview;
        this->mDistanceToView = distancetoview;
        this->mNos = false;
        this->mEngineBlown = false;
        this->mGear = G_NEUTRAL;
        this->mEnginePower = 0.0f;
        this->mShift = SHIFT_STATUS_NORMAL;
        this->mExtraBodyRoll = 0.0f;
        this->mHealth = 1.0f;
        this->mBlowOuts = 0;
        this->mExtraBodyPitch = 0.0f;
        this->mEngineSpeed = 0.0f;
        this->mAnimatedCarPitch = 0.0f;
        this->mAnimatedCarRoll = 0.0f;
        this->mAnimatedCarShake = 0.0f;
    }

    DECLARE_RENDERPACKET(Pkt_Car_Service, CarRenderConn);

    void SetNOS(bool nos) {
        this->mNos = nos;
    }

    void SetEngineBlown(bool b) {
        this->mEngineBlown = b;
    }

    void SetGear(GearID gear) {
        this->mGear = gear;
    }

    void SetShiftStatus(ShiftStatus shift) {
        this->mShift = shift;
    }

    void SetSteering(eTireIdx idx, float angle) {
        this->mSteering[idx] = angle;
    }

    void SetWheelCompression(eTireIdx idx, float compression) {
        this->mCompressions[idx] = compression;
    }

    void SetWheelSpeed(eTireIdx idx, float speed) {
        this->mWheelSpeed[idx] = speed;
    }

    void SetWheelSlip(eTireIdx idx, float slip) {
        this->mTireSlip[idx] = slip;
    }

    void SetWheelSkid(eTireIdx idx, float skid) {
        this->mTireSkid[idx] = skid;
    }

    void SetWheelOnGround(eTireIdx idx, bool b) {
        if (b) {
            this->mGroundState |= 1 << idx;
        } else {
            this->mGroundState &= ~(1 << idx);
        }
    }

    void SetDamage(const DamageZone::Info &dmg) {
        this->mDamageInfo = dmg;
    }

    void SetHealth(float amount) {
        this->mHealth = amount;
    }

    void HidePart(const UCrc32 &partname);

    void SetLightState(VehicleFX::ID idx, bool on) {
        if (on) {
            this->mLights |= idx;
        } else {
            this->mLights &= ~idx;
        }
    }

    void SetBrokenLightState(VehicleFX::ID idx, bool on) {
        if (on) {
            this->mBrokenLights |= idx;
        } else {
            this->mBrokenLights &= ~idx;
        }
    }

    void SetFlashing(bool b) {
        this->mFlashing = b;
    }

    void SetTireBlowOut(eTireIdx idx) {
        this->mBlowOuts |= 1 << idx;
    }

    void SetEnginePower(float rev) {
        this->mEnginePower = rev;
    }

    void SetEngineSpeed(float vib) {
        this->mEngineSpeed = vib;
    }

    void SetExtraBodyRoll(float amount) {
        this->mExtraBodyRoll = amount;
    }

    void SetExtraBodyPitch(float amount) {
        this->mExtraBodyPitch = amount;
    }

    void SetAnimatedRoll(float amount) {
        this->mAnimatedCarRoll = amount;
    }

    void SetAnimatedPitch(float amount) {
        this->mAnimatedCarPitch = amount;
    }

    void SetAnimatedShake(float amount) {
        this->mAnimatedCarShake = amount;
    }

    bool InView() const {
        return this->mInView;
    }

    float DistanceToView() const {
        return this->mDistanceToView;
    }

    float mCompressions[4];       // offset 0x4, size 0x10
    float mWheelSpeed[4];         // offset 0x14, size 0x10
    float mTireSkid[4];           // offset 0x24, size 0x10
    float mTireSlip[4];           // offset 0x34, size 0x10
    float mSteering[2];           // offset 0x44, size 0x8
    int mGroundState;             // offset 0x4C, size 0x4
    DamageZone::Info mDamageInfo; // offset 0x50, size 0x4
    PartState mPartState;         // offset 0x54, size 0xC
    unsigned int mLights;         // offset 0x60, size 0x4
    unsigned int mBrokenLights;   // offset 0x64, size 0x4
    bool mInView;                 // offset 0x68, size 0x4
    float mDistanceToView;        // offset 0x6C, size 0x4
    bool mFlashing;               // offset 0x70, size 0x4
    bool mNos;                    // offset 0x74, size 0x4
    bool mEngineBlown;            // offset 0x78, size 0x4
    ShiftStatus mShift;           // offset 0x7C, size 0x4
    GearID mGear;                 // offset 0x80, size 0x4
    float mEnginePower;           // offset 0x84, size 0x4
    float mEngineSpeed;           // offset 0x88, size 0x4
    float mExtraBodyRoll;         // offset 0x8C, size 0x4
    float mExtraBodyPitch;        // offset 0x90, size 0x4
    int mBlowOuts;                // offset 0x94, size 0x4
    float mHealth;                // offset 0x98, size 0x4
    float mAnimatedCarPitch;      // offset 0x9C, size 0x4
    float mAnimatedCarRoll;       // offset 0xA0, size 0x4
    float mAnimatedCarShake;      // offset 0xA4, size 0x4
};

class Pkt_Heli_Open : public Sim::Packet {
  public:
    DECLARE_RENDERPACKET(Pkt_Heli_Open, HeliRenderConn);

    Pkt_Heli_Open(const char *modelname, WUID worldid, bool spool_load)
        : mModelHash(bStringHash(modelname)), //
          mWorldID(worldid),                  //
          mSpoolLoad(spool_load) {}

    ~Pkt_Heli_Open() override {}

    uint32 mModelHash; // offset 0x4, size 0x4
    WUID mWorldID;     // offset 0x8, size 0x4
    bool mSpoolLoad;   // offset 0xC, size 0x1
};

class Pkt_Heli_Service : public Sim::Packet {
  public:
    ~Pkt_Heli_Service() override {}

    DECLARE_RENDERPACKET(Pkt_Heli_Service, HeliRenderConn);

    Pkt_Heli_Service(bool inview, float distancetoview)
        : mInView(inview), //
          mDistanceToView(distancetoview) {}

    bool InView() const {
        return this->mInView;
    }

    float DistanceToView() const {
        return this->mDistanceToView;
    }

    void SetShadowScale(float scale) {
        this->mShadowScale = scale;
    }

    void SetDustStormIntensity(float intensity) {
        this->mDustStorm = intensity;
    }

  private:
    bool mInView;          // offset 0x4, size 0x1
    float mDistanceToView; // offset 0x8, size 0x4
    float mShadowScale;    // offset 0xC, size 0x4
    float mDustStorm;      // offset 0x10, size 0x4
};

class Pkt_VehicleFragment_Open : public Sim::Packet {
  public:
    Pkt_VehicleFragment_Open(unsigned int worldid_part, unsigned int worldid_vehicle, UCrc32 partname, UCrc32 collision_node)
        : mVehicleWorldID(worldid_vehicle), //
          mWorldID(worldid_part),           //
          mPartName(partname),              //
          mColName(collision_node) {}

    DECLARE_RENDERPACKET(Pkt_VehicleFragment_Open, VehicleFragmentConn);

    ~Pkt_VehicleFragment_Open() override {}

    WUID mVehicleWorldID; // offset 0x4, size 0x4
    WUID mWorldID;        // offset 0x8, size 0x4
    UCrc32 mPartName;     // offset 0xC, size 0x4
    UCrc32 mColName;      // offset 0x10, size 0x4
};

class Pkt_VehicleFragment_Service : public Sim::Packet {
  public:
    Pkt_VehicleFragment_Service(bool inview, float disttoview)
        : mInView(inview), //
          mDistanceToView(disttoview) {}

    DECLARE_RENDERPACKET(Pkt_VehicleFragment_Service, VehicleFragmentConn);

    bool InView() const {
        return this->mInView;
    }

    float DistanceToView() const {
        return this->mDistanceToView;
    }

    ~Pkt_VehicleFragment_Service() override {}

  private:
    bool mInView;          // offset 0x4, size 0x1
    float mDistanceToView; // offset 0x8, size 0x4
};

// total size: 0x18
class Pkt_Smackable_Open : public Sim::Packet {
  public:
    DECLARE_RENDERPACKET(Pkt_Smackable_Open, SmackableRenderConn);

  private:
    bHash32 mModelHash;                              // offset 0x4, size 0x4
    WUID mObjectWUID;                                // offset 0x8, size 0x4
    const CollisionGeometry::Bounds *mCollisionNode; // offset 0xC, size 0x4
    const ModelHeirarchy *mHeirarchy;                // offset 0x10, size 0x4
    uint32 mRenderNode;                              // offset 0x14, size 0x4
};

// total size: 0x10
class Pkt_Smackable_Service : public Sim::Packet {
  public:
    Pkt_Smackable_Service(bool visible, float distancetoview) {
        this->mVisible = visible;
        this->mDistanceToView = distancetoview;
        this->mChildVisibility = 0xFFFFFF;
    }

    DECLARE_RENDERPACKET(Pkt_Smackable_Service, SmackableRenderConn);

  private:
    bool mVisible;           // offset 0x4, size 0x1
    float mDistanceToView;   // offset 0x8, size 0x4
    uint32 mChildVisibility; // offset 0xC, size 0x4
};

void UpdateServices(float dT);
void UpdateLoading();
void InitServices();
void RestoreServices();

} // namespace RenderConn

#endif
