#ifndef SIM_SIMPROFILE_H
#define SIM_SIMPROFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct HSIMPROFILE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HSIMPROFILE__ *HSIMPROFILE;

namespace Sim {

namespace Profile {

inline HSIMPROFILE Create() {
    return nullptr;
}

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
