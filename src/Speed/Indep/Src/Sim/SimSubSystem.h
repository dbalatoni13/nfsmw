#ifndef SIM_SUBSYTEM
#define SIM_SUBSYTEM

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"

namespace Sim {

// total size: 0x14
class SubSystem {
  public:
    typedef void (*InitCallback)();
    typedef void (*RestoreCallback)();

#if EA_BUILD_A124 // TODO is this necessary?
    SubSystem(const char *name, InitCallback initcb, RestoreCallback restorecb) {

#else
    SubSystem(const char *name, InitCallback initcb, RestoreCallback restorecb)
        : mInit(initcb),       //
          mRestore(restorecb), //
          mSig(name),          //
          mNext(mHead),        //
          mName(name) {
#endif
    }

    static void Init(const UCrc32 &sig) {
        SubSystem *s;
        for (s = mHead; s != nullptr; s = s->mNext) {
            if (s->mSig == sig && s->mInit) {
                s->ValidateHeap(true, true);
                s->mInit();
                s->ValidateHeap(false, true);
                break;
            }
        }
    }

    static void Shutdown(const UCrc32 &sig) {
        SubSystem *s;
        for (s = mHead; s != nullptr; s = s->mNext) {
            if (s->mSig == sig && s->mRestore) {
                s->ValidateHeap(true, false);
                s->mRestore();
                s->ValidateHeap(false, false);
                break;
            }
        }
    }

  private:
    void ValidateHeap(bool before, bool initializing);

    InitCallback mInit;       // offset 0x0, size 0x4
    RestoreCallback mRestore; // offset 0x4, size 0x4
    SubSystem *mNext;         // offset 0x8, size 0x4
    UCrc32 mSig;              // offset 0xC, size 0x4
    const char *mName;        // offset 0x10, size 0x4
    static SubSystem *mHead;
};

}; // namespace Sim

#define BIND_SIM_SUBSYSTEM(_CLASSNAME_, _INITFUNCTION_, _RESTOREFUNCTION_)                                                                           \
    Sim::SubSystem _Physics_System_##_CLASSNAME_(#_CLASSNAME_, _INITFUNCTION_, _RESTOREFUNCTION_);

#endif
