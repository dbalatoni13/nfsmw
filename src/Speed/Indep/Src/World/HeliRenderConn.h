#ifndef HELI_RENDER_CONN_H
#define HELI_RENDER_CONN_H

#include "Speed/Indep/Src/Render/RenderConn.h"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"

class HeliRenderConn : public VehicleRenderConn {
  public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    HeliRenderConn(const Sim::ConnectionData &data, CarType ct, RenderConn::Pkt_Heli_Open *oc);
    ~HeliRenderConn() override;

    void OnRender(eView *view, int reflection) override;
    void OnFetch(float dT) override;

  private:
    void Update(const RenderConn::Pkt_Heli_Service &data, float dT);

    bMatrix4 mMatrices[4];    // offset 0x64, size 0x100
    bMatrix4 mGeomMatrix;     // offset 0x164, size 0x40
    uint32 mLastRenderFrame;  // offset 0x1A4, size 0x4
    uint32 mLastVisibleFrame; // offset 0x1A8, size 0x4
    float mDistanceToView;    // offset 0x1AC, size 0x4
    float mShadowScale;       // offset 0x1B0, size 0x4
};

#endif
