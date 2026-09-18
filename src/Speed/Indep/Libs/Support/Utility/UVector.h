#ifndef SUPPORT_UTILITY_UVECTOR_H
#define SUPPORT_UTILITY_UVECTOR_H

#include "types.h"
#include "UMath.h"

// El orden de declaracion es el del volcado DWARF del original: en GCC 2.9 los
// cuerpos en clase se compilan al cerrar la clase y en orden de declaracion, de
// modo que un miembro solo puede expandirse dentro de otro declarado despues.
// Project/GetProjection/GetAngle/Normalize van definidos FUERA de la clase (con
// `inline`) y detras de los operadores libres, tal y como los lista el original.
// TODO class
struct ALIGN_16 UVector3 : public UMath::Vector3 {
    UVector3() {
        Clear(*this);
    }

    UVector3(const Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
    }

    UVector3(float fx, float fy, float fz) {
        x = fx;
        y = fy;
        z = fz;
    }

    UVector3(const UMath::Vector4 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
    }

    void Negate() {
        UMath::Negate(*this);
    }

    const UVector3 &operator=(const Vector3 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
        return *this;
    }

    const UVector3 &operator=(const UMath::Vector4 &From) {
        x = From.x;
        y = From.y;
        z = From.z;
        return *this;
    }

    UVector3 Normal() const {
        UVector3 n = *this;
        n.Normalize();
        return n;
    }

    void Abs() {
        x = UMath::Abs(x);
        y = UMath::Abs(y);
        z = UMath::Abs(z);
    }

    float Magnitude() const {
        return UMath::Length(*this);
    }

    float Dot(const UMath::Vector3 &a) const {
        return UMath::Dot(*this, a);
    }

    UVector3 Cross(const UMath::Vector3 &a) const {
        UVector3 r;
        UMath::Cross(*this, a, r);
        return r;
    }

    void operator*=(const UMath::Matrix4 &m) {
        UMath::Vector3 r;
        UMath::Rotate(*this, m, r);
        *this = r;
    }

    // unused
    void operator/=(const float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }

    void operator*=(float scalar) {
        UMath::Scale(*this, scalar, *this);
    }

    void operator+=(const UMath::Vector3 &v) {
        UMath::Add(*this, v, *this);
    }

    void operator-=(const UMath::Vector3 &v) {
        UMath::Sub(*this, v, *this);
    }

    void Project(const UVector3 &normal, const UVector3 &planept);
    float GetProjection(const UVector3 &normal, const UVector3 &planept) const;
    float GetAngle(const UVector3 &to) const;
    float Normalize();

    // No esta en el volcado del original; la conservamos al final para no
    // desplazar el orden de declaracion de los 21 miembros que si estan.
    UVector3(const float f) {
        x = f;
        y = f;
        z = f;
    }
};

inline UVector3 operator+(const UVector3 &v1, const UVector3 &v2) {
    UMath::Vector3 result;
    UMath::Add(v1, v2, result);
    return result;
}

inline UVector3 operator-(const UVector3 &v1, const UMath::Vector3 &v2) {
    UMath::Vector3 result;
    UMath::Sub(v1, v2, result);
    return result;
}

inline UVector3 operator*(float scalar, const UVector3 &v1) {
    UMath::Vector3 result;
    UMath::Scale(v1, scalar, result);
    return UVector3(result);
}

inline UVector3 operator*(const UVector3 &v1, float scalar) {
    UMath::Vector3 result;
    UMath::Scale(v1, scalar, result);
    return UVector3(result);
}

inline UVector3 operator/(const UVector3 &v1, float scalar) {
    UMath::Vector3 result;
    UMath::Scale(v1, 1.0f / scalar, result);
    return UVector3(result);
}

// El volcado lista aqui `operator*(const UVector3 &, const Matrix4 &)`, pero
// esa sobrecarga ya vive en Dynamics::Collision (Collision.h:14) y declararla
// tambien aqui la vuelve ambigua. Se deja anotada, no definida.

inline void UVector3::Project(const UVector3 &normal, const UVector3 &planept) {
    UMath::ScaleAdd(normal, -GetProjection(normal, planept), *this, *this);
}

inline float UVector3::GetProjection(const UVector3 &normal, const UVector3 &planept) const {
    return UMath::Dot(*this, normal) - UMath::Dot(planept, normal);
}

inline float UVector3::GetAngle(const UVector3 &to) const {
    UVector3 u0 = Normal();
    UVector3 u1 = to.Normal();
    const float dot = u0.Dot(u1);
    return acosf(dot);
}

inline float UVector3::Normalize() {
    float m = UMath::Length(*this);
    if (m != 0.0f) {
        UMath::Scale(*this, 1.0f / m, *this);
    }
    return m;
}

#endif
