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
    Param(const Param &from) : mType(from.mType), mName(from.mName) {
        this->mData = from.mData;
    }
};

class Packet {};

class Object : public UTL::COM::Object, public IServiceable, public ITaskable, public UTL::Collections::Countable<Object> {
    // total size: 0x2C
    unsigned int mTaskCount;    // offset 0x24, size 0x4
    unsigned int mServiceCount; // offset 0x28, size 0x4

  protected:
    Object(unsigned int num_interfaces);

    virtual bool OnService(HSIMSERVICE, Sim::Packet *);

    virtual bool OnTask(HSIMTASK htask, float dT);
};

}; // namespace Sim

#endif
