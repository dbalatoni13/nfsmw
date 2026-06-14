#ifndef VEHICLE_FRAGMENT_CONN
#define VEHICLE_FRAGMENT_CONN

#include "Speed/Indep/Src/Sim/SimServer.h"
#include "Speed/Indep/Src/World/CarPartID.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/Src/World/WorldModel.hpp"

class VehicleFragmentConn : public Sim::Connection, public bTNode<VehicleFragmentConn> {
  public:
    VehicleFragmentConn(const Sim::ConnectionData &data);
    ~VehicleFragmentConn() override;

    void OnClose() override {
        delete this;
    }

    Sim::ConnStatus OnStatusCheck() override {
        return Sim::CONNSTATUS_READY;
    }

    static Sim::Connection *Construct(const Sim::ConnectionData &data) {
        return new VehicleFragmentConn(data);
    }

    static void FetchData(float dT);

  private:
    void Update(float dT);
    void UpdateModel();

    WorldConn::Reference mTarget;                       // offset 0x18, size 0x10
    WUID mVehicleWorldID;                               // offset 0x28, size 0x4
    CAR_PART_ID mPartSlot;                              // offset 0x2C, size 0x4
    UCrc32 mColName;                                    // offset 0x30, size 0x4
    int mModelHash;                                     // offset 0x34, size 0x4
    WorldModel *mModel;                                 // offset 0x38, size 0x4
    bMatrix4 mModelOffset;                              // offset 0x3C, size 0x40
    bMatrix4 mRenderMatrix;                             // offset 0x7C, size 0x40
    eReplacementTextureTable *mReplacementTextureTable; // offset 0xBC, size 0x4
    eLightMaterial *mLightMaterial;                     // offset 0xC0, size 0x4
};

#endif
