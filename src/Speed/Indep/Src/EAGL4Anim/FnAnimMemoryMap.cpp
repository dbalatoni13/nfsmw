#include "FnAnimMemoryMap.h"

namespace EAGL4Anim {

FnAnimMemoryMap::FnAnimMemoryMap() : mpAnim(nullptr) {}

FnAnimMemoryMap::~FnAnimMemoryMap() {}

void FnAnimMemoryMap::SetAnimMemoryMap(AnimMemoryMap *anim) {
    mpAnim = anim;
}

AnimMemoryMap *FnAnimMemoryMap::GetAnimMemoryMap() {
    return mpAnim;
}

const AnimMemoryMap *FnAnimMemoryMap::GetAnimMemoryMap() const {
    return mpAnim;
}

unsigned short FnAnimMemoryMap::GetTargetCheckSum() const {
    return mpAnim->GetTargetCheckSum();
}

}; // namespace EAGL4Anim
