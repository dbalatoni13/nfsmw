#ifndef SIM_SIMPROFILE_H
#define SIM_SIMPROFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct HSIMPROFILE__ {
    // total size: 0x4
    int unused; // offset 0x0, size 0x4
};

typedef HSIMPROFILE__ *HSIMPROFILE;

namespace Sim {

namespace Profile {

inline HSIMPROFILE Create() { return 0; }
inline void Release(HSIMPROFILE) {}

}; // namespace Profile

}; // namespace Sim

#endif
