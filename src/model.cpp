// librealshapez.a(shpelement.cpp)

#include "realshape.hpp"

namespace RealShape {

int ShapeElement::GetElementType() const {
    TexelType texelType = GetType();
    switch (texelType) {
    case TEXEL_TYPE_SHAPE:
        return ELEMENT_TYPE_SHAPE;
    case TEXEL_TYPE_HOTSPOTS:
        return ELEMENT_TYPE_HOTSPOTS;
    case TEXEL_TYPE_COMMENT:
        return ELEMENT_TYPE_COMMENT;
    case TEXEL_TYPE_EAGL:
        return ELEMENT_TYPE_EAGL;
    }
    if (TexelTypeHelper::IsClutType(texelType)) {
        return ELEMENT_TYPE_CLUT;
    }
    if (TexelTypeHelper::IsTextureType(texelType)) {
        return ELEMENT_TYPE_TEXTURE;
    }
    return 0;
}

ShapeElement *Shape::GetTexture() const {
    return GetElement(ELEMENT_TYPE_TEXTURE);
}

ShapeElement *Shape::GetElement(int elementType) const {
    ShapeElement *element = (ShapeElement *)this;
    while (element != 0) {
        if (elementType == element->GetElementType()) {
            return element;
        }
        element = element->GetNext();
    }
    return 0;
}

} // namespace RealShape
