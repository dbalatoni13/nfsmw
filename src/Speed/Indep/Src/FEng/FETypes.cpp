#include "Speed/Indep/Src/FEng/FETypes.h"

FEImageData::FEImageData()
    : UpperLeft(),
      LowerRight() {
    Col = FEColor();
    Pivot = FEVector3();
    Pos = FEVector3();
    Rot = FEQuaternion();
    Size = FEVector3();
}
