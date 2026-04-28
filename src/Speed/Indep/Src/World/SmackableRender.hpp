#ifndef WORLD_SMACKABLERENDER_H
#define WORLD_SMACKABLERENDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Sim/SimServer.H"
#include "Speed/Indep/Src/World/SmackableRender.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "WorldConn.h"
#include "WorldModel.hpp"

struct SmackableRenderConn : public Sim::Connection, public bTNode<SmackableRenderConn> {
public:
    static Sim::Connection *Construct(const Sim::ConnectionData &data);

    SmackableRenderConn(const Sim::ConnectionData &data);
    virtual ~SmackableRenderConn();
    void OnClose() override;
    Sim::ConnStatus OnStatusCheck() override;
    void Update(float dT);
    static void UpdateAll(float dT);

    void *operator new(unsigned int size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, unsigned int size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }

    static bTList<SmackableRenderConn> mList;

private:
    // total size: 0x8C
    bHash32 mModelHash; // offset 0x18, size 0x4
    const ModelHeirarchy *mHeirarchy; // offset 0x1C, size 0x4
    WorldConn::Reference mTarget; // offset 0x20, size 0x10
    WorldModel *mModel; // offset 0x30, size 0x4
    unsigned int mRenderNode; // offset 0x34, size 0x4
    bVector4 mModelOffset; // offset 0x38, size 0x10
    bMatrix4 mRenderMatrix; // offset 0x48, size 0x40
    int mLOD; // offset 0x88, size 0x4
};

#endif
