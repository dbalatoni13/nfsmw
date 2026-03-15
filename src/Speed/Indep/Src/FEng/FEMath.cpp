#include "Speed/Indep/Src/FEng/FETypes.h"

FEVector2& FEVector2::operator=(const FEVector2& v) {
    x = v.x;
    y = v.y;
    return *this;
}
