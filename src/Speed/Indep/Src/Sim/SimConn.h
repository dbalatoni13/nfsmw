#ifndef SIM_CONN_H
#define SIM_CONN_H

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

// I added override to this macro so clangd doesn't yell at us
#define DECLARE_SIMPACKET(_PKT_, _NAME_)                                                                                                             \
    unsigned Size() override {                                                                                                                       \
        return sizeof(_PKT_);                                                                                                                        \
    }                                                                                                                                                \
    static unsigned SType() {                                                                                                                        \
        static UCrc32 hash(_NAME_);                                                                                                                  \
        return hash.GetValue();                                                                                                                      \
    }                                                                                                                                                \
    unsigned Type() override {                                                                                                                       \
        return SType();                                                                                                                              \
    }

namespace Sim {

class Packet {
  public:
    template <typename T> static T *Cast(Packet *pkt) {
        return reinterpret_cast<T *>(pkt);
    }

    // Virtual functions
    virtual UCrc32 ConnectionClass();
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
