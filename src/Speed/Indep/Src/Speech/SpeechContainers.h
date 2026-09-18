#ifndef SPEECH_SPEECHCONTAINERS_H
#define SPEECH_SPEECHCONTAINERS_H

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class EAXCop;

namespace Speech {

// total size: 0x8
struct copPair {
    bool operator<(const copPair &rhs) const {
        return this->hsimable < rhs.hsimable;
    }

    HSIMABLE hsimable; // offset 0x0, size 0x4
    EAXCop *cop;       // offset 0x4, size 0x4
};

DECLARE_CONTAINER_TYPE(copMap);

class copMap : public UTL::Std::vector<copPair, _type_copMap> {
  public:
    copMap(int size) {
        this->reserve(size);
    }
    void Add(HSIMABLE__ *hsimable, EAXCop *cop);
    EAXCop *Remove(HSIMABLE__ *hsimable);
    void ModifyHandle(HSIMABLE__ *hsimable, HSIMABLE__ *newhandle);
    EAXCop *Find(HSIMABLE__ *hsimable) const;
};

} // namespace Speech

#endif
