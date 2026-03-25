#ifndef SIM_SIMCONN_H
#define SIM_SIMCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace Sim {

class Packet {
  public:
    template <typename T> static T *Cast(Packet *pkt) {
        return reinterpret_cast<T *>(pkt);
    }

    // Virtual functions
    virtual UCrc32 ConnectionClass() = 0;
    virtual unsigned int Compress(Packet *to) const;
    virtual unsigned int Decompress(Packet *to) const;
    virtual unsigned int Type() = 0;
    virtual unsigned int Size() = 0;

  protected:
    Packet() {}

    virtual ~Packet() {}
};

}; // namespace Sim

#endif
