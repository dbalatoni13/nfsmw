#include "GIcon.h"

void GIcon::SetFlag(unsigned int mask) {
    mFlags |= mask;
}

void GIcon::ClearFlag(unsigned int mask) {
    mFlags &= ~mask;
}
