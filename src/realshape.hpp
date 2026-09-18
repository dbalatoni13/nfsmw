// Cabecera reconstruida de librealshapez.a (RealShape 6.x, egami/realgraph).
//
// No existe fuente en el arbol: los tipos salen del desasamblado del objetivo
// (`build/GOWE69/asm/{creates,base,model,cluttype}.s`), de los nombres
// manglados y del stub que ya habia en
// `src/egami/rcmp/dev/source/decoder/cmn/rcmp_mad_codec.cpp`.
//
// Solo la usan src/creates.cpp, src/base.cpp y src/model.cpp. `cluttype.cpp`
// va aparte porque alli TexelTypeHelper se define fuera de linea.

#ifndef REALSHAPE_HPP
#define REALSHAPE_HPP

namespace EA {
namespace Allocator {
class IAllocator;
}
} // namespace EA

namespace RealShape {

enum OwnedFlag { OWNED_DISABLED = 0, OWNED_ENABLED = 1 };
enum OwnedDataFlag { OWNED_DATA_DISABLED = 0, OWNED_DATA_ENABLED = 1 };
enum CompressedFlag { COMPRESSED_DISABLED = 0, COMPRESSED_ENABLED = 1 };
enum SwizzledFlag { SWIZZLED_DISABLED = 0, SWIZZLED_ENABLED = 1 };
enum MipmontFlag { MIPMONT_DISABLED = 0, MIPMONT_ENABLED = 1 };
enum CubeMapFlag { CUBE_MAP_DISABLED = 0, CUBE_MAP_ENABLED = 1 };
enum Dot3Flag { DOT3_DISABLED = 0, DOT3_ENABLED = 1 };
enum EmbmFlag { EMBM_DISABLED = 0, EMBM_ENABLED = 1 };
enum HalfAlphaFlag { HALF_ALPHA_DISABLED = 0, HALF_ALPHA_ENABLED = 1 };
enum TransparentFlag { TRANSPARENT_DISABLED = 0, TRANSPARENT_ENABLED = 1 };
enum OpaqueFlag { OPAQUE_DISABLED = 0, OPAQUE_ENABLED = 1 };
enum TransposedFlag { TRANSPOSED_DISABLED = 0, TRANSPOSED_ENABLED = 1 };
enum SharedDataFlag { SHARED_DATA_DISABLED = 0, SHARED_DATA_ENABLED = 1 };

// Los identificadores que lleva el byte 0 de cada elemento.
enum TexelType {
    TEXEL_TYPE_INVALID = 0,
    TEXEL_TYPE_18 = 0x18,
    TEXEL_TYPE_19 = 0x19,
    TEXEL_TYPE_1E = 0x1e,
    TEXEL_TYPE_CLUT_FIRST = 0x30,
    TEXEL_TYPE_CLUT_LAST = 0x33,
    TEXEL_TYPE_EAGL = 0x69,
    TEXEL_TYPE_GC_YUV = 0x6b,
    TEXEL_TYPE_GC_R8G8B8A8 = 0x6c,
    TEXEL_TYPE_COMMENT = 0x6f,
    TEXEL_TYPE_CLIP = 0x7a,
    TEXEL_TYPE_HOTSPOTS = 0x7c,
    TEXEL_TYPE_SHAPE = 0x81
};

// Lo que devuelve ShapeElement::GetElementType() y lo que lleva
// CreateStruct::mShapeElements.
enum ElementType {
    ELEMENT_TYPE_SHAPE = 0x1,
    ELEMENT_TYPE_TEXTURE = 0x2,
    ELEMENT_TYPE_CLUT = 0x4,
    ELEMENT_TYPE_HOTSPOTS = 0x10,
    ELEMENT_TYPE_COMMENT = 0x20,
    ELEMENT_TYPE_EAGL = 0x80,
    ELEMENT_TYPE_CLIP = 0x100
};

// Las banderas del campo mFlags de 24 bits de ElementInfo.
enum ElementFlag {
    ELEMENT_FLAG_USED = 0x000001,
    ELEMENT_FLAG_COMPRESSED = 0x000002,
    ELEMENT_FLAG_OWNED = 0x000004,
    ELEMENT_FLAG_OWNED_DATA = 0x000008,
    ELEMENT_FLAG_CUBE_MAP = 0x000100,
    ELEMENT_FLAG_DOT3 = 0x000200,
    ELEMENT_FLAG_EMBM = 0x000400,
    ELEMENT_FLAG_HALF_ALPHA = 0x000800,
    ELEMENT_FLAG_TRANSPARENT = 0x001000,
    ELEMENT_FLAG_OPAQUE = 0x002000,
    ELEMENT_FLAG_SWIZZLED = 0x004000,
    ELEMENT_FLAG_TRANSPOSED = 0x008000,
    ELEMENT_FLAG_MIPMONT = 0x010000,
    ELEMENT_FLAG_NUM_MIPMAPS = 0xf00000
};

// La tabla de bits-por-texel (la define creates.cpp).
extern const unsigned char gTexelTypeToBpp[256];

// El ternario NO puede escribirse en el `|=`: GCC 2.9 lo convierte en rama.
// Pasando por este envoltorio el valor se materializa como mascara
// (`xori`/`subic`/`subfe`/`rlwinm`), que es lo que emite el objetivo.
static inline unsigned int ElementFlagBit(int condition, unsigned int mask) {
    return condition ? mask : 0;
}

class TexelTypeHelper {
  public:
    // IsTextureType va DELANTE a proposito: GCC 2.9 compila los cuerpos en
    // clase en orden de declaracion, asi que aqui IsClutType todavia no tiene
    // cuerpo y sale `bl`. Es lo que hace el objetivo (model.cpp inlina la
    // PRIMERA llamada a IsClutType y llama a la de dentro de IsTextureType).
    static int IsClutType(TexelType texelType);
    static int GetDepth(TexelType texelType);

    static int IsTextureType(TexelType texelType) {
        return GetDepth(texelType) != 0 && !IsClutType(texelType);
    }
};

// Los dos cuerpos van FUERA de la clase para que IsTextureType, que se compila
// antes, no pueda inlinarlos: en el objetivo IsTextureType llama a los dos.
// Desde una funcion normal (creates.cpp, model.cpp) si se inlinan.
inline int TexelTypeHelper::IsClutType(TexelType texelType) {
    int first = TEXEL_TYPE_CLUT_FIRST;
    int last = TEXEL_TYPE_CLUT_LAST;
    return texelType <= last && texelType >= first;
}

inline int TexelTypeHelper::GetDepth(TexelType texelType) {
    return gTexelTypeToBpp[(unsigned char)texelType];
}

// total size: 0x4
struct ElementInfo {
    unsigned int mType : 8;   // bits 0-7
    unsigned int mFlags : 24; // bits 8-31
};

// total size: 0xC0
struct CreateStruct {
    int mSignature;                    // offset 0x0
    int mShapeElements;                // offset 0x4
    OwnedFlag mOwned;                  // offset 0x8
    OwnedDataFlag mOwnedData;          // offset 0xC
    CompressedFlag mCompressed;        // offset 0x10
    TexelType mTextureType;            // offset 0x14
    int mWidth;                        // offset 0x18
    int mHeight;                       // offset 0x1C
    int mShapeX;                       // offset 0x20
    int mShapeY;                       // offset 0x24
    int mNumMipmaps;                   // offset 0x28
    SwizzledFlag mSwizzledTexture;     // offset 0x2C
    MipmontFlag mMipmont;              // offset 0x30
    CubeMapFlag mCubeMap;              // offset 0x34
    Dot3Flag mDot3;                    // offset 0x38
    EmbmFlag mEmbm;                    // offset 0x3C
    HalfAlphaFlag mHalfAlpha;          // offset 0x40
    TransparentFlag mTransparent;      // offset 0x44
    OpaqueFlag mOpaque;                // offset 0x48
    TransposedFlag mTransposed;        // offset 0x4C
    SharedDataFlag mTextureSharedData; // offset 0x50
    TexelType mClutType;               // offset 0x54
    SwizzledFlag mSwizzledClut;        // offset 0x58
    int mNumClutColours;               // offset 0x5C
    SharedDataFlag mClutSharedData;    // offset 0x60
    int mClipX;                        // offset 0x64
    int mClipY;                        // offset 0x68
    int mClipWidth;                    // offset 0x6C
    int mClipHeight;                   // offset 0x70
    int mCenterX;                      // offset 0x74
    int mCenterY;                      // offset 0x78
    int mDimension;                    // offset 0x7C
    int mNumHotSpots;                  // offset 0x80
    int mEaglBinSize;                  // offset 0x84
    int mUserBinSize;                  // offset 0x88
    int mUnicode;                      // offset 0x8C
    int mCommentLength;                // offset 0x90
    int mReserved[11];                 // offset 0x94

    CreateStruct();
    void Init();
};

class MemObject {
  public:
    static void *sAlloc(const char *name, unsigned int size, int alignment, int offset, int flags, char *file, int line);
    static void sFree(void *ptr);

    static EA::Allocator::IAllocator *sAllocator;
};

class GraphObject {
  public:
    static void SetAllocator(EA::Allocator::IAllocator *allocator);
};

// total size: 0x10
class ShapeElement {
  public:
    int GetElementType() const;
    void CreateAt(const CreateStruct &createStruct);
    static void Destroy(ShapeElement *shapeElement);

    TexelType GetType() const {
        return (TexelType)mElementInfo.mType;
    }

    ShapeElement *GetNext() const {
        if (mNextOffset != 0) {
            return (ShapeElement *)((char *)this + mNextOffset);
        }
        return 0;
    }

    void *GetData() const {
        if (mDataOffset == 0) {
            return 0;
        }
        return (char *)this + mDataOffset;
    }

    void SetNext(ShapeElement *next) {
        if (next == 0) {
            mNextOffset = 0;
        } else {
            mNextOffset = (char *)next - (char *)this;
        }
    }

    void SetData(void *data, int size) {
        if (data == 0) {
            mDataOffset = 0;
            mDataSize = 0;
        } else {
            mDataOffset = (char *)data - (char *)this;
            mDataSize = size;
        }
    }

    // El ternario NO vale: `flag == ENABLED` se pliega a un `andi.`/`beq` y el
    // objetivo materializa el 0/1 (`li 1`/`andi.`/`bne`/`li 0`/`cmpwi 1`).
    // Cada bandera va por su SETTER, y el argumento se pasa por VALOR: eso
    // mete el `lwz` del campo de CreateStruct DELANTE del `&= ~mask`, y es lo
    // que impide que muera el store del `|=` anterior. Con las ocho sentencias
    // escritas en linea en CreateAt, GCC borra el store equivocado.
    void SetType(TexelType texelType) {
        mElementInfo.mType = texelType;
    }

    void SetUsedFlag(int used) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_USED;
        mElementInfo.mFlags |= ElementFlagBit(used, ELEMENT_FLAG_USED);
    }

    void SetOwnedFlag(OwnedFlag owned) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_OWNED;
        mElementInfo.mFlags |= ElementFlagBit(owned == OWNED_ENABLED, ELEMENT_FLAG_OWNED);
    }

    void SetOwnedDataFlag(OwnedDataFlag ownedData) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_OWNED_DATA;
        mElementInfo.mFlags |= ElementFlagBit(ownedData == OWNED_DATA_ENABLED, ELEMENT_FLAG_OWNED_DATA);
    }

    void SetCompressedFlag(CompressedFlag compressed) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_COMPRESSED;
        mElementInfo.mFlags |= ElementFlagBit(compressed == COMPRESSED_ENABLED, ELEMENT_FLAG_COMPRESSED);
    }

    void SetNumMipmaps(int numMipmaps) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_NUM_MIPMAPS;
        mElementInfo.mFlags |= numMipmaps << 20;
    }

    void SetMipmontFlag(MipmontFlag mipmont) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_MIPMONT;
        mElementInfo.mFlags |= ElementFlagBit(mipmont == MIPMONT_ENABLED, ELEMENT_FLAG_MIPMONT);
    }

    void SetSwizzledFlag(SwizzledFlag swizzled) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_SWIZZLED;
        mElementInfo.mFlags |= ElementFlagBit(swizzled == SWIZZLED_ENABLED, ELEMENT_FLAG_SWIZZLED);
    }

    void SetCubeMapFlag(CubeMapFlag cubeMap) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_CUBE_MAP;
        mElementInfo.mFlags |= ElementFlagBit(cubeMap == CUBE_MAP_ENABLED, ELEMENT_FLAG_CUBE_MAP);
    }

    void SetDot3Flag(Dot3Flag dot3) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_DOT3;
        mElementInfo.mFlags |= ElementFlagBit(dot3 == DOT3_ENABLED, ELEMENT_FLAG_DOT3);
    }

    void SetEmbmFlag(EmbmFlag embm) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_EMBM;
        mElementInfo.mFlags |= ElementFlagBit(embm == EMBM_ENABLED, ELEMENT_FLAG_EMBM);
    }

    void SetHalfAlphaFlag(HalfAlphaFlag halfAlpha) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_HALF_ALPHA;
        mElementInfo.mFlags |= ElementFlagBit(halfAlpha == HALF_ALPHA_ENABLED, ELEMENT_FLAG_HALF_ALPHA);
    }

    void SetTransparentFlag(TransparentFlag transparent) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_TRANSPARENT;
        mElementInfo.mFlags |= ElementFlagBit(transparent == TRANSPARENT_ENABLED, ELEMENT_FLAG_TRANSPARENT);
    }

    void SetOpaqueFlag(OpaqueFlag opaque) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_OPAQUE;
        mElementInfo.mFlags |= ElementFlagBit(opaque == OPAQUE_ENABLED, ELEMENT_FLAG_OPAQUE);
    }

    void SetTransposedFlag(TransposedFlag transposed) {
        mElementInfo.mFlags &= ~ELEMENT_FLAG_TRANSPOSED;
        mElementInfo.mFlags |= ElementFlagBit(transposed == TRANSPOSED_ENABLED, ELEMENT_FLAG_TRANSPOSED);
    }

    OwnedFlag GetOwnedFlag() const {
        OwnedFlag flag = OWNED_ENABLED;
        if (!(mElementInfo.mFlags & ELEMENT_FLAG_OWNED)) {
            flag = OWNED_DISABLED;
        }
        return flag;
    }

    OwnedDataFlag GetOwnedDataFlag() const {
        OwnedDataFlag flag = OWNED_DATA_ENABLED;
        if (!(mElementInfo.mFlags & ELEMENT_FLAG_OWNED_DATA)) {
            flag = OWNED_DATA_DISABLED;
        }
        return flag;
    }

    ElementInfo mElementInfo; // offset 0x0
    int mNextOffset;          // offset 0x4
    int mDataOffset;          // offset 0x8
    int mDataSize;            // offset 0xC
};

// total size: 0x20
class TextureElement : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    void CreateAt(const CreateStruct &createStruct);

    int mShapeX;          // offset 0x10
    int mShapeY;          // offset 0x14
    unsigned int mWidth;  // offset 0x18
    unsigned int mHeight; // offset 0x1C
};

// total size: 0x20
class ClutElement : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    void CreateAt(const CreateStruct &createStruct);

    int mShapeX;          // offset 0x10
    int mShapeY;          // offset 0x14
    unsigned int mWidth;  // offset 0x18
    unsigned int mHeight; // offset 0x1C
};

// total size: 0x20
class ClipElement : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    void CreateAt(const CreateStruct &createStruct);

    void SetClipRect(int x, int y, int width, int height) {
        mClipX = x;
        mClipY = y;
        mClipWidth = width;
        mClipHeight = height;
    }

    int mClipX;      // offset 0x10
    int mClipY;      // offset 0x14
    int mClipWidth;  // offset 0x18
    int mClipHeight; // offset 0x1C
};

// total size: 0x20
class HotSpotsElement : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    void CreateAt(const CreateStruct &createStruct);

    int mCenterX;     // offset 0x10
    int mCenterY;     // offset 0x14
    int mDimension;   // offset 0x18
    int mNumHotSpots; // offset 0x1C
};

// total size: 0x10
class EaglElement : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    void CreateAt(const CreateStruct &createStruct);
};

// total size: 0x10
class CommentElement : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    void CreateAt(const CreateStruct &createStruct);
};

class Shape : public ShapeElement {
  public:
    static int GetCreateSize(const CreateStruct &createStruct);
    static Shape *Create(const CreateStruct &createStruct, int memoryDirection);
    static void CreateAt(Shape *shape, const CreateStruct &createStruct);

    ShapeElement *GetElement(int elementType) const;
    ShapeElement *GetTexture() const;
};

} // namespace RealShape

#endif
