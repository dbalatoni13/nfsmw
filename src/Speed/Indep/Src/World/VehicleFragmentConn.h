#ifndef WORLD_VEHICLEFRAGMENTCONN_H
#define WORLD_VEHICLEFRAGMENTCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/VehicleRenderConn.h"

class WorldModel;
struct eLightMaterial;
struct eReplacementTextureTable;

namespace RenderConn {

class Pkt_VehicleFragment_Open : public Sim::Packet {
  public:
    Pkt_VehicleFragment_Open(unsigned int worldid_part, unsigned int worldid_vehicle, UCrc32 partname, UCrc32 collision_node)
        : mVehicleWorldID(worldid_vehicle), //
          mWorldID(worldid_part), //
          mPartName(partname), //
          mColName(collision_node) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("VehicleFragmentConn");
        return hash;
    }

    unsigned int Size() override {
        return 0x14;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_VehicleFragment_Open");
        return hash.GetValue();
    }

    unsigned int Type() override {
        return SType();
    }

    ~Pkt_VehicleFragment_Open() override {}

    unsigned int mVehicleWorldID; // offset 0x4, size 0x4
    unsigned int mWorldID;        // offset 0x8, size 0x4
    UCrc32 mPartName;             // offset 0xC, size 0x4
    UCrc32 mColName;              // offset 0x10, size 0x4
};

class Pkt_VehicleFragment_Service : public Sim::Packet {
  public:
    Pkt_VehicleFragment_Service(bool inview, float disttoview)
        : mInView(inview), //
          mDistanceToView(disttoview) {}

    UCrc32 ConnectionClass() override {
        static UCrc32 hash("VehicleFragmentConn");
        return hash;
    }

    unsigned int Size() override {
        return 0xC;
    }

    static unsigned int SType() {
        static UCrc32 hash("Pkt_VehicleFragment_Service");
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

    ~Pkt_VehicleFragment_Service() override {}

    bool mInView;          // offset 0x4, size 0x1
    float mDistanceToView; // offset 0x8, size 0x4
};

} // namespace RenderConn

class VehicleFragmentConn : public Sim::Connection, public bTNode<VehicleFragmentConn> {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    VehicleFragmentConn(const Sim::ConnectionData &data);
    ~VehicleFragmentConn() override;

    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override;

    void Update(float dT);
    static void FetchData(float dT);
    void UpdateModel();

    static bTList<VehicleFragmentConn> mList;

  private:
    Reference mTarget;                              // offset 0x18, size 0x10
    unsigned int mVehicleWorldID;                   // offset 0x28, size 0x4
    CAR_PART_ID mPartSlot;                          // offset 0x2C, size 0x4
    UCrc32 mColName;                                // offset 0x30, size 0x4
    int mModelHash;                                 // offset 0x34, size 0x4
    WorldModel *mModel;                             // offset 0x38, size 0x4
    bMatrix4 mModelOffset;                          // offset 0x3C, size 0x40
    bMatrix4 mRenderMatrix;                         // offset 0x7C, size 0x40
    eReplacementTextureTable *mReplacementTextureTable; // offset 0xBC, size 0x4
    eLightMaterial *mLightMaterial;                 // offset 0xC0, size 0x4
};

#endif
