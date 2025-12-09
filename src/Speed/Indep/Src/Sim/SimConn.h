#ifndef SIM_SIMCONN_H
#define SIM_SIMCONN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Sim {

class Packet {
  public:
    template <typename T> static T *Cast(Packet *pkt) {
        return reinterpret_cast<T *>(pkt);
    }

    // Virtual functions
    virtual unsigned int Compress(Packet *to);
    virtual unsigned int Decompress(Packet *to);
    virtual unsigned int Type();
    virtual unsigned int Size();

  protected:
    Packet() {}

    virtual ~Packet() {}
};

}; // namespace Sim

#endif
