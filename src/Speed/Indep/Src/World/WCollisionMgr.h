#pragma once

#include "Speed/Indep/bWare/Inc/bVector.hpp"
namespace WCollisionMgr {

class ICollisionHandler {
  ICollisionHandler() {}

public:
  virtual bool OnWCollide(const struct WorldCollisionInfo &cInfo, const struct bVector3 &cPoint, void *userdata);
};

}; // namespace WCollisionMgr
