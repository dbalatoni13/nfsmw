#ifndef SIM_SIM_TYPES_H
#define SIM_SIM_TYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"

namespace Sim {

class Param {
    // total size: 0x10
    UCrc32 mType;      // offset 0x0, size 0x4
    UCrc32 mName;      // offset 0x4, size 0x4
    const void *mData; // offset 0x8, size 0x4
    unsigned int pad;  // offset 0xC, size 0x4

  public:
    Param() : mType(UCrc32()), mName(UCrc32()), mData(nullptr) {}

    Param(const Param &from) : mType(UCrc32(from.mType)), mName(UCrc32(from.mName)), mData(from.mData) {}

    template <typename T> Param(UCrc32 name, T *addr) : mName(name), mData(addr) {}

    template <typename T> const Param *Find(UCrc32 name) const {
        UCrc32 type = T::TypeName();
        if (mName == name) {
            if (mType == type) {
                return this;
            } else {
                bBreak();
            }
        }
        return nullptr;
    }

    template <typename T> const T &Fetch(UCrc32 name) const {
        const Param *p = Find<T>(UCrc32(name));
        return *reinterpret_cast<const T *>(p->mData);
    }
};

enum State {
    STATE_NONE = 0,
    STATE_INITIALIZING = 1,
    STATE_ACTIVE = 3,
    STATE_IDLE = 4,
};

enum eUserMode {
    USER_SINGLE = 0,
    USER_SPLIT_SCREEN = 1,
    USER_ONLINE = 2,
};

}; // namespace Sim

// total size: 0x18
class SimCollisionMap {
  public:
    class IRigidBody *GetRB(int rbIndex) const;
    class IRigidBody *GetSRB(int srbIndex) const;
    class IRigidBody *GetOrderedBody(int index) const;

    void Clear() {
        for (unsigned int i = 0; i < 3; ++i) {
            fBitMap[i] = 0;
        }
    }

  private:
    unsigned long long fBitMap[3]; // offset 0x0, size 0x18
};

#endif
