#ifndef EAXSOUND_CSIS_HPP
#define EAXSOUND_CSIS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Csis {
// total size: 0x8
struct InterfaceId {
    const char *pString; // offset 0x0, size 0x4
    short systemCrc;     // offset 0x4, size 0x2
    short interfaceCrc;  // offset 0x6, size 0x2
};

// total size: 0x8
struct ClassHandle {
    ClassHandle();
    int Set(const InterfaceId *pInterfaceId);
    int SetFast(const InterfaceId *pInterfaceId);
    int Valid();
    static void *operator new(unsigned int size);
    static void operator delete(void *ptr);

    void *mpPrivate; // offset 0x0, size 0x4
    int mKey;        // offset 0x4, size 0x4
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
