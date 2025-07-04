#ifndef WORLD_WCOLLISION_MGR_H
#define WORLD_WCOLLISION_MGR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"
namespace WCollisionMgr {

class ICollisionHandler {
    ICollisionHandler() {}

  public:
    virtual bool OnWCollide(const struct WorldCollisionInfo &cInfo, const struct bVector3 &cPoint, void *userdata);
};

}; // namespace WCollisionMgr

#endif
