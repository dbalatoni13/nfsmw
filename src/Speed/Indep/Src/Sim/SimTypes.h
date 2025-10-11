#ifndef SIM_SIM_TYPES_H
#define SIM_SIM_TYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/IServiceable.h"
#include "Speed/Indep/Src/Interfaces/ITaskable.h"

namespace Sim {

class Param {
    // total size: 0x10
    UCrc32 mType;      // offset 0x0, size 0x4
    UCrc32 mName;      // offset 0x4, size 0x4
    const void *mData; // offset 0x8, size 0x4
    unsigned int pad;  // offset 0xC, size 0x4

  public:
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

class Packet {};

// total size: 0x2C
class Object : public UTL::COM::Object, public IServiceable, public ITaskable, public UTL::Collections::Countable<Object> {
    unsigned int mTaskCount;    // offset 0x24, size 0x4
    unsigned int mServiceCount; // offset 0x28, size 0x4

  protected:
    Object(unsigned int num_interfaces);
    HSIMTASK AddTask(const UCrc32 &schedule, float rate, float start_offset, TaskMode mode);
    void RemoveTask(HSIMTASK htask);

    virtual ~Object();

    virtual bool OnService(HSIMSERVICE hCon, Sim::Packet *pkt);

    virtual bool OnTask(HSIMTASK htask, float dT) {}
};

}; // namespace Sim

#endif
