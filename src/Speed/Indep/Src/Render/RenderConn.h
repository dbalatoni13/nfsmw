#ifndef RENDER_SERVICE_H
#define RENDER_SERVICE_H

#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Physics/Bounds.h"
#include "Speed/Indep/Src/Sim/SimConn.h"
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

class HeliRenderConn;
class VehicleFragmentConn;

namespace RenderConn {

class Pkt_Heli_Open : public Sim::Packet {
  public:
    DECLARE_RENDERPACKET(Pkt_Heli_Open, HeliRenderConn);

    Pkt_Heli_Open(unsigned int model_hash, unsigned int world_id, bool spool_load)
        : mModelHash(model_hash), //
          mWorldID(world_id),     //
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
