// librealshapez.a(shpcreate.cpp)
//
// El literal de fichero que lleva la unidad lo dice:
//   D:/env/egami/realgraph/6/source/shape/cmn/shpcreate.cpp
// y la llamada a sAlloc de Shape::Create lleva la linea 722 (0x2d2).

#include "realshape.hpp"

extern "C" {
void *memset(void *ptr, int value, unsigned int num);
int sprintf(char *buffer, const char *format, ...);
}


namespace RealShape {

// Bits por texel de cada TexelType (valores del DOL; el resto de la tabla es 0).
const unsigned char gTexelTypeToBpp[256] = {
    0, 4, 8, 16, 24, 32, 16, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    4, 8, 8, 16, 16, 16, 32, 0, 4, 8, 16, 0, 12, 0, 4, 0,
    16, 32, 24, 0, 0, 0, 0, 0, 0, 16, 32, 0, 32, 16, 32, 24,
    16, 16, 16, 32, 0, 0, 0, 0, 32, 16, 0, 0, 0, 0, 0, 0,
    32, 8, 16, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 16, 16, 16, 24, 32, 0,
    4, 8, 8, 0, 8, 16, 24, 16, 16, 0, 32, 0, 12, 16, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 16, 4, 4, 8, 0, 32, 16, 24,
    1, 1, 1, 1, 1, 1, 1, 1, 1,
};

void CreateStruct::Init() {
    mSignature = 0;
    mShapeElements = 0;
    mOwned = OWNED_DISABLED;
    mOwnedData = OWNED_DATA_DISABLED;
    mCompressed = COMPRESSED_DISABLED;
    mTextureType = TEXEL_TYPE_INVALID;
    mWidth = 0;
    mHeight = 0;
    mShapeX = 0;
    mShapeY = 0;
    mNumMipmaps = 0;
    mSwizzledTexture = SWIZZLED_DISABLED;
    mMipmont = MIPMONT_DISABLED;
    mCubeMap = CUBE_MAP_DISABLED;
    mDot3 = DOT3_DISABLED;
    mEmbm = EMBM_DISABLED;
    mHalfAlpha = HALF_ALPHA_DISABLED;
    mTransparent = TRANSPARENT_DISABLED;
    mOpaque = OPAQUE_DISABLED;
    mTransposed = TRANSPOSED_DISABLED;
    mTextureSharedData = SHARED_DATA_DISABLED;
    mClutType = TEXEL_TYPE_INVALID;
    mSwizzledClut = SWIZZLED_DISABLED;
    mNumClutColours = 0;
    mClutSharedData = SHARED_DATA_DISABLED;
    mClipX = 0;
    mClipY = 0;
    mClipWidth = 0;
    mClipHeight = 0;
    mCenterX = 0;
    mCenterY = 0;
    mDimension = 0;
    mNumHotSpots = 0;
    mEaglBinSize = 0;
    mUnicode = 0;
    mCommentLength = 0;
    mUserBinSize = 0;
    memset(mReserved, 0, sizeof(mReserved));
}

CreateStruct::CreateStruct() {
    Init();
}

static int GetNumberOfColours(int depth) {
    switch (depth) {
    case 8:
        return 256;
    case 4:
        return 16;
    }
    return 0;
}

int TextureElement::GetCreateSize(const CreateStruct &createStruct) {
    int size = 0;
    TexelType textureType = createStruct.mTextureType;
    int width = createStruct.mWidth;
    int height = createStruct.mHeight;
    int numMipmaps = createStruct.mNumMipmaps;
    if (createStruct.mTextureSharedData == SHARED_DATA_DISABLED) {
        int rawDepth = TexelTypeHelper::GetDepth(textureType);
        int depth = rawDepth == 15 ? 16 : rawDepth;
        if (width < 1 || width > 0x7fffffff || height < 1 || height > 0x7fffffff ||
            width * height > 0x7fffffff) {
            return 0;
        }
        int mipWidth = width;
        int mipHeight = height;
        for (int i = 0; i <= numMipmaps; i++) {
            int packed = textureType == TEXEL_TYPE_1E;
            int levelWidth;
            int levelHeight;
            if (packed) {
                levelWidth = (mipWidth + 7) & ~7;
                levelHeight = (mipHeight + 7) & ~7;
            } else {
                levelWidth = mipWidth;
                levelHeight = mipHeight;
            }
            if (size != 0) {
                size = (size + 31) & ~31;
            }
            size += ((levelWidth * depth + 7) >> 3) * levelHeight;
            if (mipWidth == 1 && mipHeight == 1) {
                break;
            }
            mipHeight >>= 1;
            mipWidth >>= 1;
            if (mipWidth <= 0) {
                mipWidth = 1;
            }
            if (mipHeight <= 0) {
                mipHeight = 1;
            }
        }
    }
    return size + 0x20;
}

int ClutElement::GetCreateSize(const CreateStruct &createStruct) {
    int size = 0;
    TexelType textureType = createStruct.mTextureType;
    int clutDepth = TexelTypeHelper::GetDepth(createStruct.mClutType);
    int textureDepth = TexelTypeHelper::GetDepth(textureType);
    int numColours = createStruct.mNumClutColours;
    if ((numColours == 0 && textureType == TEXEL_TYPE_18) || textureType == TEXEL_TYPE_19) {
        numColours = GetNumberOfColours(textureDepth);
    }
    if (createStruct.mClutSharedData == SHARED_DATA_DISABLED) {
        size = numColours * clutDepth / 8;
    }
    return size + 0x20;
}

int ClipElement::GetCreateSize(const CreateStruct &createStruct) {
    return 0x20;
}

int CommentElement::GetCreateSize(const CreateStruct &createStruct) {
    return createStruct.mCommentLength + 0x11;
}

int EaglElement::GetCreateSize(const CreateStruct &createStruct) {
    return createStruct.mEaglBinSize + 0x10;
}

int HotSpotsElement::GetCreateSize(const CreateStruct &createStruct) {
    return createStruct.mNumHotSpots * createStruct.mDimension * 4 + 0x20;
}

int Shape::GetCreateSize(const CreateStruct &createStruct) {
    int size = 0;
    if (createStruct.mShapeElements & ELEMENT_TYPE_TEXTURE) {
        size = TextureElement::GetCreateSize(createStruct);
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_CLUT) {
        if (size != 0 && createStruct.mClutSharedData == SHARED_DATA_DISABLED) {
            size = (size + 31) & ~31;
        }
        size += ClutElement::GetCreateSize(createStruct);
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_CLIP) {
        if (size != 0) {
            size = (size + 15) & ~15;
        }
        size += ClipElement::GetCreateSize(createStruct);
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_HOTSPOTS) {
        if (size != 0) {
            size = (size + 15) & ~15;
        }
        size += HotSpotsElement::GetCreateSize(createStruct);
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_EAGL) {
        if (size != 0) {
            size = ((size - 1) & ~15) + 16;
        }
        size += EaglElement::GetCreateSize(createStruct);
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_COMMENT) {
        if (size != 0) {
            size = ((size - 1) & ~15) + 16;
        }
        size += CommentElement::GetCreateSize(createStruct);
    }
    return size;
}

void ShapeElement::CreateAt(const CreateStruct &createStruct) {
    SetUsedFlag(1);
    SetOwnedFlag(createStruct.mOwned);
    SetOwnedDataFlag(createStruct.mOwnedData);
    SetCompressedFlag(createStruct.mCompressed);
    mDataOffset = 0;
    mDataSize = 0;
    mNextOffset = 0;
}

static inline int IsSwizzlableDepth(int depth) {
    switch (depth) {
    case 4:
    case 8:
    case 15:
    case 16:
    case 32:
        return 1;
    }
    return 0;
}

void TextureElement::CreateAt(const CreateStruct &createStruct) {
    ShapeElement::CreateAt(createStruct);
    SetType(createStruct.mTextureType);
    mWidth = (unsigned short)createStruct.mWidth;
    mHeight = (unsigned short)createStruct.mHeight;
    mShapeX = createStruct.mShapeX;
    mShapeY = createStruct.mShapeY;
    SetNumMipmaps(createStruct.mNumMipmaps);
    SetMipmontFlag(createStruct.mMipmont);
    if (IsSwizzlableDepth(TexelTypeHelper::GetDepth(createStruct.mTextureType))) {
        SetSwizzledFlag(createStruct.mSwizzledTexture);
    } else {
        SetSwizzledFlag(SWIZZLED_DISABLED);
    }
    SetCubeMapFlag(createStruct.mCubeMap);
    SetDot3Flag(createStruct.mDot3);
    SetEmbmFlag(createStruct.mEmbm);
    SetHalfAlphaFlag(createStruct.mHalfAlpha);
    SetTransparentFlag(createStruct.mTransparent);
    SetOpaqueFlag(createStruct.mOpaque);
    SetTransposedFlag(createStruct.mTransposed);
    SetData((char *)this + sizeof(TextureElement), GetCreateSize(createStruct) - sizeof(TextureElement));
}

void ClutElement::CreateAt(const CreateStruct &createStruct) {
    ShapeElement::CreateAt(createStruct);
    SetType(createStruct.mClutType);
    mWidth = (unsigned short)createStruct.mNumClutColours;
    mHeight = 1;
    SetSwizzledFlag(createStruct.mSwizzledClut);
    SetMipmontFlag(MIPMONT_DISABLED);
    SetNumMipmaps(0);
    SetData((char *)this + sizeof(ClutElement), GetCreateSize(createStruct) - sizeof(ClutElement));
}

void ClipElement::CreateAt(const CreateStruct &createStruct) {
    ShapeElement::CreateAt(createStruct);
    SetType(TEXEL_TYPE_CLIP);
    SetClipRect(createStruct.mClipX, createStruct.mClipY, createStruct.mClipWidth, createStruct.mClipHeight);
    SetData((char *)this + sizeof(ClipElement), GetCreateSize(createStruct) - sizeof(ClipElement));
}

void HotSpotsElement::CreateAt(const CreateStruct &createStruct) {
    ShapeElement::CreateAt(createStruct);
    SetType(TEXEL_TYPE_HOTSPOTS);
    mCenterX = createStruct.mCenterX;
    mCenterY = createStruct.mCenterY;
    mDimension = createStruct.mDimension;
    mNumHotSpots = createStruct.mNumHotSpots;
    SetData((char *)this + sizeof(HotSpotsElement), GetCreateSize(createStruct) - sizeof(HotSpotsElement));
}

void EaglElement::CreateAt(const CreateStruct &createStruct) {
    ShapeElement::CreateAt(createStruct);
    int dataSize = GetCreateSize(createStruct) - sizeof(EaglElement);
    SetType(TEXEL_TYPE_EAGL);
    SetData((char *)this + sizeof(EaglElement), dataSize);
    sprintf((char *)GetData(), "EAGL%d", dataSize);
}

void CommentElement::CreateAt(const CreateStruct &createStruct) {
    ShapeElement::CreateAt(createStruct);
    SetType(TEXEL_TYPE_COMMENT);
    SetData((char *)this + sizeof(CommentElement), GetCreateSize(createStruct) - sizeof(CommentElement));
}

Shape *Shape::Create(const CreateStruct &createStruct, int memoryDirection) {
    Shape *shape = (Shape *)MemObject::sAlloc("Created Shape", GetCreateSize(createStruct), 32, 32, memoryDirection,
                                             "D:/env/egami/realgraph/6/source/shape/cmn/shpcreate.cpp", 722);
    CreateAt(shape, createStruct);
    shape->SetOwnedFlag(OWNED_ENABLED);
    return shape;
}

void Shape::CreateAt(Shape *shape, const CreateStruct &createStruct) {
    ShapeElement *element = (ShapeElement *)shape;
    ShapeElement *previous = 0;
    if (createStruct.mShapeElements & ELEMENT_TYPE_TEXTURE) {
        if (createStruct.mTextureSharedData == SHARED_DATA_DISABLED) {
            element = (ShapeElement *)((((unsigned int)element + sizeof(TextureElement) + 31) & ~31) -
                                       sizeof(TextureElement));
        }
        ((TextureElement *)element)->CreateAt(createStruct);
        previous = element;
        element = (ShapeElement *)((char *)previous + TextureElement::GetCreateSize(createStruct));
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_CLUT) {
        if (createStruct.mClutSharedData == SHARED_DATA_DISABLED) {
            element =
                (ShapeElement *)((((unsigned int)element + sizeof(ClutElement) + 31) & ~31) - sizeof(ClutElement));
            if (previous != 0) {
                previous->SetNext(element);
            }
        }
        ((ClutElement *)element)->CreateAt(createStruct);
        previous = element;
        element = (ShapeElement *)((char *)previous + ClutElement::GetCreateSize(createStruct));
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_CLIP) {
        element = (ShapeElement *)((((unsigned int)element + sizeof(ClipElement) + 15) & ~15) - sizeof(ClipElement));
        if (previous != 0) {
            previous->SetNext(element);
        }
        ((ClipElement *)element)->CreateAt(createStruct);
        previous = element;
        element = (ShapeElement *)((char *)previous + ClipElement::GetCreateSize(createStruct));
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_HOTSPOTS) {
        element = (ShapeElement *)((((unsigned int)element + sizeof(HotSpotsElement) + 15) & ~15) -
                                   sizeof(HotSpotsElement));
        if (previous != 0) {
            previous->SetNext(element);
        }
        ((HotSpotsElement *)element)->CreateAt(createStruct);
        previous = element;
        element = (ShapeElement *)((char *)previous + HotSpotsElement::GetCreateSize(createStruct));
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_EAGL) {
        element = (ShapeElement *)((((unsigned int)element + sizeof(EaglElement) + 15) & ~15) - sizeof(EaglElement));
        if (previous != 0) {
            previous->SetNext(element);
        }
        ((EaglElement *)element)->CreateAt(createStruct);
        previous = element;
        element = (ShapeElement *)((char *)previous + EaglElement::GetCreateSize(createStruct));
    }
    if (createStruct.mShapeElements & ELEMENT_TYPE_COMMENT) {
        element =
            (ShapeElement *)((((unsigned int)element + sizeof(CommentElement) + 15) & ~15) - sizeof(CommentElement));
        if (previous != 0) {
            previous->SetNext(element);
        }
        ((CommentElement *)element)->CreateAt(createStruct);
        previous = element;
        element = (ShapeElement *)((char *)previous + CommentElement::GetCreateSize(createStruct));
    }
    if (previous != 0) {
        previous->SetNext(0);
    }
}

} // namespace RealShape
