#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"

namespace Sim {

// UNSOLVED stack too small and two regswaps
// https://decomp.me/scratch/FaYvA
Activity::Activity(unsigned int num_interfaces)
    : Object(num_interfaces + 3), //
      IActivity(this),            //
      IAttachable(this),          //
      mAttachments(new Attachments(this)) {}

Activity::~Activity() {
    if (mAttachments) {
        delete mAttachments;
        mAttachments = nullptr;
    }
}

void Activity::DetachAll() {
    if (mAttachments) {
        delete mAttachments;
        mAttachments = nullptr;
    }
}

void Activity::Release() {
    ReleaseGC();
}

}; // namespace Sim
