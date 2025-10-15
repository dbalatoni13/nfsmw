#ifndef SUPPORT_UTILITY_UVECTOR_H
#define SUPPORT_UTILITY_UVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "cstddef"
#include "./UTypes.h"

class UBezierLite {
public:
    UBezierLite() {}
    ~UBezierLite() {}

    static void Evaluate(const UMath::Matrix4 &fSplineMat, const float t, UMath::Vector4 &position);
    
    static void *operator new(std::size_t size, void *ptr) {}
    static void operator delete(void *mem, void *ptr) {}
    static void *operator new(std::size_t size) {}
    static void operator delete(void *mem, std::size_t size) {}
    static void *operator new(std::size_t size, const char *name) {}
    static void operator delete(void *mem, const char *name) {}
    static void operator delete(void *mem, std::size_t size, const char *name) {}
};

#endif
