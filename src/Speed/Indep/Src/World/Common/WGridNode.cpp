#include "Speed/Indep/Src/World/Common/WGridNode.h"

unsigned int WGridNode::TotalSize() const {
    unsigned int size = sizeof(WGridNode);
    for (int i = 0; i < 4; i++) {
        size += this->fElemCounts[i] * 4;
    }
    return size;
}
