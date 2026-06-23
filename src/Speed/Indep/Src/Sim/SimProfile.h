#ifndef _SIM_PROFILE_H
#define _SIM_PROFILE_H

#include "Speed/Indep/Src/Sim/SimTypes.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

DECLARE_SIMHANDLE(HSIMPROFILE);

#define SIM_PROFILING 0

namespace Sim {

namespace Profile {

inline HSIMPROFILE Create() {
    return nullptr;
}

inline void Capture() {}

inline void Release(HSIMPROFILE) {}

inline void Init() {}

inline void Shutdown() {}

inline void Begin() {}

inline void End() {}

// total size: 0xC
class Scope : public bTNode<Scope> {
  public:
    void operator delete(void *ptr) {}

    Scope(HSIMPROFILE prof) {}

    ~Scope() {}

  private:
    void *_mPrivate; // offset 0x8, size 0x4
};

}; // namespace Profile

}; // namespace Sim

#endif
