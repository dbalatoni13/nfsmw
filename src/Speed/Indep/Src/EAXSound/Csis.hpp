#ifndef EAXSOUND_CSIS_HPP
#define EAXSOUND_CSIS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Csis {
struct InterfaceId;

struct ClassHandle {
    int Set(const InterfaceId *pInterfaceId);
};

struct FunctionHandle {
    int Set(const InterfaceId *pInterfaceId);
};

struct Class {
    static int CreateInstance(ClassHandle *handle, void *memberData, Class **outClass);
    int Release();
    void SetMemberData(void *data);
    void GetRefCount(int *refcount);
};

namespace System {
enum Result {
    kResult_Ok = 0,
};

Result Subscribe(void *desc);
void *Alloc(int bytes);
void Free(void *memory);
} // namespace System

struct Function {
    static System::Result Call(FunctionHandle *pFunctionHandle, void *pParameters);
};
} // namespace Csis

#endif
