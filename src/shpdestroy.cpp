// librealshapez.a(shpdestroy.cpp)

#include "realshape.hpp"

#include "Speed/Indep/Libs/allocator/1.5.0/Allocator/iallocator.h"

namespace RealShape {

void ShapeElement::Destroy(ShapeElement *shapeElement) {
    ShapeElement *next = shapeElement->GetNext();
    if (next != 0) {
        Destroy(next);
    }
    if (shapeElement->GetOwnedDataFlag() == OWNED_DATA_ENABLED) {
        MemObject::sFree(shapeElement->GetData());
    }
    if (shapeElement->GetOwnedFlag() == OWNED_ENABLED) {
        MemObject::sFree(shapeElement);
    }
}

} // namespace RealShape
