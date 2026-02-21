#include "eagl4AnimBank.h"
#include "MemoryPoolManager.h"
#include <cstring>

namespace EAGL4Anim {

void AnimBank::Constructor(void *ptr, EAGL4::DynamicLoader *loader, const char *pSymbolName) {
    AnimBank *bank = reinterpret_cast<AnimBank *>(ptr);
    for (int i = bank->GetNumAnims() - 1; i >= 0; i--) {
        MemoryPoolManager::InitAnimMemoryMap(bank->GetAnim(i));
    }
}

void AnimBank::Destructor(void *ptr) {}

int AnimBank::GetAnimIndex(const char *name) {
    int r;
    int m;
    int s = 0;
    int e = GetNumAnims() - 1;

    while (s <= e) {
        m = (s + e) >> 1;
        r = strcmp(name, mNames[m]);

        if (r > 0) {
            s = m + 1;
        } else if (r >= 0) {
            return m;
        } else {
            e = m - 1;
        }
    }

    return -1;
}

FnAnimMemoryMap *AnimBank::NewFnAnim(AnimMemoryMap *animMem) {}

}; // namespace EAGL4Anim
