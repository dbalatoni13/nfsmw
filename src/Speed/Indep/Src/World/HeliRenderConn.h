#ifndef WORLD_HELIRENDERCONN_H
#define WORLD_HELIRENDERCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/VehicleRenderConn.h"

namespace RenderConn {

class Pkt_Heli_Open : public Sim::Packet {
  public:
    Pkt_Heli_Open(unsigned int model_hash, unsigned int world_id, bool spool_load)
        : mModelHash(model_hash), //
          mWorldID(world_id), //
          mSpoolLoad(spool_load) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("HeliRenderConn");
        return hash;
    }

    unsigned int Size() override {
        return 0x10;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_Heli_Open");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

    ~Pkt_Heli_Open() override {}

    unsigned int mModelHash; // offset 0x4, size 0x4
    unsigned int mWorldID;   // offset 0x8, size 0x4
    bool mSpoolLoad;         // offset 0xC, size 0x1
};

class Pkt_Heli_Service : public Sim::Packet {
  public:
    Pkt_Heli_Service(bool inview, float distancetoview)
        : mInView(inview), //
          mDistanceToView(distancetoview) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("HeliRenderConn");
        return hash;
    }

    unsigned int Size() override {
        return 0x14;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_Heli_Service");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

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

    ~Pkt_Heli_Service() override {}

    bool mInView;          // offset 0x4, size 0x1
    float mDistanceToView; // offset 0x8, size 0x4
    float mShadowScale;    // offset 0xC, size 0x4
    float mDustStorm;      // offset 0x10, size 0x4
};

} // namespace RenderConn

class HeliRenderConn : public VehicleRenderConn {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    HeliRenderConn(const Sim::ConnectionData &data, CarType type, RenderConn::Pkt_Heli_Open *open);
    ~HeliRenderConn() override;

    void OnRender(eView *view, int reflection) override;
    void OnFetch(float dT) override;

  private:
    void Update(const RenderConn::Pkt_Heli_Service &data, float dT);

    bMatrix4 mMatrices[4];         // offset 0x64, size 0x100
    bMatrix4 mGeomMatrix;          // offset 0x164, size 0x40
    unsigned int mLastRenderFrame; // offset 0x1A4, size 0x4
    unsigned int mLastVisibleFrame; // offset 0x1A8, size 0x4
    float mDistanceToView;         // offset 0x1AC, size 0x4
    float mShadowScale;            // offset 0x1B0, size 0x4
};

#endif
