
#ifndef REALSHAPE_H
#define REALSHAPE_H // Decl: 18

#define SHAPE_EXT ".fsh"           // Decl: 81
#define STREAMING_SHAPE_EXT ".fss" // Decl: 82

namespace RealShape {

// total size: 0x1
// Decl: 2326
struct Shape {
    static void Destroy(struct Shape *shape) {}

    // enum TexelType GetType() const {}

    int GetDepth() const {}

    // void (*GetMover(enum TexelType targetType))(unsigned char *, const unsigned char *, int) const {}

    bool GetConverterList(const struct ConvertStruct &convertStruct, struct Converter *converterList, int *converterListLength) const {}

    int GetWidth() const {}

    int GetHeight() const {}

    // enum CubeMapFlag GetCubeMapFlag() const {}

    // enum Dot3Flag GetDot3Flag() const {}

    // enum EmbmFlag GetEmbmFlag() const {}

    // enum TransparentFlag GetTransparentFlag() const {}

    // enum OpaqueFlag GetOpaqueFlag() const {}

    // enum CompressedFlag GetCompressedFlag() const {}

    // enum TransposedFlag GetTransposedFlag() const {}

    // enum MipmontFlag GetMipmontFlag() const {}

    // enum SwizzledFlag GetSwizzledFlag() const {}

    int GetShapeX() const {}

    int GetShapeY() const {}

    int GetNumMipmaps() const {}

    // enum MipmappedFlag GetMipmappedFlag() const {}

    void SetWidth(int width) {}

    void SetHeight(int height) {}

    // void SetCubeMapFlag(enum CubeMapFlag cubeMap) {}

    // void SetDot3Flag(enum Dot3Flag dot3) {}

    // void SetEmbmFlag(enum EmbmFlag embm) {}

    // void SetTransparentFlag(enum TransparentFlag transparent) {}

    // void SetOpaqueFlag(enum OpaqueFlag opaque) {}

    // void SetCompressedFlag(enum CompressedFlag compressed) {}

    // void SetTransposedFlag(enum TransposedFlag transposed) {}

    // void SetMipmontFlag(enum MipmontFlag mipmont) {}

    // void SetSwizzledFlag(enum SwizzledFlag swizzled) {}

    void SetShapeX(int shapeX) {}

    void SetShapeY(int shapeY) {}

    void SetNumMipmaps(int numMipmaps) {}

    int GetNumColours() const {}

    void SetNumColours(int numColours) {}

    void GetClipRect(int *x, int *y, int *width, int *height) const {}

    void SetClipRect(int x, int y, int width, int height) {}

    void *GetEaglBin() const {}

    int GetEaglBinSize() const {}

    void SetEaglBin(const void *eaglBin, int size) {}

    int GetNumHotSpots() const {}

    int GetDimension() const {}

    int GetCenterX() const {}

    int GetCenterY() const {}

    void SetCenterX(int centerX) const {}

    void SetCenterY(int centerY) const {}

    const char *GetCommentString() const {}

    void SetCommentString(const char *string) {}
};
} // namespace RealShape

#endif
