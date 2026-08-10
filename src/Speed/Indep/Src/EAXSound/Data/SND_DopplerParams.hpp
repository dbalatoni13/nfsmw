//
//
//
//
//
//
#ifndef SND_DOPPLERPARAMS_HPP
#define SND_DOPPLERPARAMS_HPP

#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixerDefines.hpp"

// total size: 0x8
// Decl: 42
struct stDOPPLER_PARAMS {
    stDOPPLER_PARAMS() {} // Decl: 43

    stDOPPLER_PARAMS(float _PitchScale, eDistTargets _ReferanceObj)
        : PitchScale(_PitchScale),       //
          ReferanceObj(_ReferanceObj) {} // Decl: 49

    float PitchScale;          // offset 0x0, size 0x4, Decl: 55
    eDistTargets ReferanceObj; // offset 0x4, size 0x4, Decl: 56
};

#endif
