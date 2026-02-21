#ifndef EAGL4ANIM_EAGL4RUNTIMETRANSFORM_H
#define EAGL4ANIM_EAGL4RUNTIMETRANSFORM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

#include <types.h>

// total size: 0x40
struct Transform {
    enum Axis {
        X_AXIS = 0,
        Y_AXIS = 4,
        Z_AXIS = 8,
    };

    // Functions
    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    void PostMult(const Transform &second, Transform *pOutput) const;

    void PostMult(const Transform &second);

    void BuildScale(float x, float y, float z, float w);

    void BuildTranslate(float x, float y, float z);

    void BuildIdentity();

    void BuildZero();

    void BuildRotTrans(const UMath::Matrix3 *pRot, const UMath::Vector4 *pTrans);

    void BuildRotTrans(const UMath::Matrix3 *pRot, const UMath::Vector3 *pTrans);

    void BuildSRT(float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz);

    void ExtractRotTrans(UMath::Matrix3 *pRot, UMath::Vector3 *pTrans) const;

    void BuildMatrix(const UMath::Matrix4 *pMat);

    void BuildQuatTrans(const UMath::Vector4 *pQuat, const UMath::Vector4 *pTrans);

    void ExtractQuatTrans(UMath::Vector4 *retQuat, UMath::Vector4 *retTrans) const;

    void TransformPoints(unsigned int num, const UMath::Vector3 *source, unsigned int sourceStride, UMath::Vector3 *dest,
                         unsigned int destStride) const;

    void TransformPoints(unsigned int num, const UMath::Vector3 *source, UMath::Vector3 *dest) const;

    void TransformPoints(unsigned int num, const UMath::Vector4 *source, unsigned int sourceStride, UMath::Vector4 *dest,
                         unsigned int destStride) const;

    void TransformPoints(unsigned int num, const UMath::Vector4 *source, UMath::Vector4 *dest) const;

    void TransformPoints(unsigned int num, const UMath::Vector3 *source, unsigned int sourceStride, UMath::Vector4 *dest,
                         unsigned int destStride) const;

    void TransformPoints(unsigned int num, const UMath::Vector3 *source, UMath::Vector4 *dest) const;

    void TransformPoints(unsigned int num, const UMath::Vector4 *source, unsigned int sourceStride, UMath::Vector3 *dest,
                         unsigned int destStride) const;

    void TransformPoints(unsigned int num, const UMath::Vector4 *source, UMath::Vector3 *dest) const;

    void TransformVector(const UMath::Vector3 &src, UMath::Vector3 &dest) const;

    void OrthoInverse(Transform *retOutput) const;

    void ReplaceRotate(float angle360, float vectorx, float vectory, float vectorz);

    void TransformPoint(const UMath::Vector3 &src, UMath::Vector3 &dest) const;

    void Inverse(Transform *retOutput) const;

    void Inverse();

    void Transpose();

    static void Transpose(const Transform &a, Transform &at);

    static float Invert(const Transform &a, Transform &b);

    void AddWeightedSRT(float weight, const Transform &srt);

    void AppendScale(float x, float y, float z, float w);

    void AppendRotate(float angle360, float vectorx, float vectory, float vectorz);

    void AppendTranslate(float x, float y, float z);

    void AppendRotTrans(const UMath::Matrix3 *pRot, const UMath::Vector4 *pTrans);

    void AppendRotTrans(const UMath::Matrix3 *pRot, const UMath::Vector3 *pTrans);

    void AppendMatrix(const UMath::Matrix4 *pMat);

    void AppendQuatTrans(const UMath::Vector4 *pQuat, const UMath::Vector4 *pTrans);

    void PrependScale(float x, float y, float z, float w);

    void PrependRotate(float angle360, float vectorx, float vectory, float vectorz);

    void PrependTranslate(float x, float y, float z);

    void PrependRotTrans(const UMath::Matrix3 *pRot, const UMath::Vector4 *pTrans);

    void PrependMatrix(const UMath::Matrix4 *pMat);

    void PrependQuatTrans(const UMath::Vector4 *pQuat, const UMath::Vector4 *pTrans);

    void BuildSQT(float sx, float sy, float sz, float qx, float qy, float qz, float qw, float tx, float ty, float tz);

    void BuildQT(float qx, float qy, float qz, float qw, float tx, float ty, float tz);

    void BuildRotate(float angle360, float vectorx, float vectory, float vectorz);

    void PreMult(const Transform &first);

    UMath::Matrix4 m; // offset 0x0, size 0x40
};

#endif
