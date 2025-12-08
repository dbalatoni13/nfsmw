#include "Speed/Indep/Src/Sim/SimActivity.h"

namespace Sim {

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
