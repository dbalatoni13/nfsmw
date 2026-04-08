#ifndef EAXSOUND_CSIS_HPP
#define EAXSOUND_CSIS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Csis {
struct Class;

union Parameter {
    int iVal;
    float fVal;
};

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
    FunctionHandle();
    int Set(const InterfaceId *pInterfaceId);
    int SetFast(const InterfaceId *pInterfaceId);
    int Valid();
    static void *operator new(unsigned int size);
    static void operator delete(void *ptr);

    void *mpPrivate; // offset 0x0, size 0x4
    int mKey;        // offset 0x4, size 0x4
};

struct FunctionClient {
    void *reserved[2];                               // offset 0x0, size 0x8
    void (*pClientFunc)(Parameter *pParameters, void *pClientData); // offset 0x8, size 0x4
    void *pClientData;                               // offset 0xC, size 0x4
};

typedef FunctionClient MemberDataClient;
typedef FunctionClient GlobalVariableClient;

struct ClassConstructorClient {
    void *reserved[2];                                              // offset 0x0, size 0x8
    void (*pClientFunc)(Class *pClass, Parameter *pParameters, void *pClientData); // offset 0x8, size 0x4
    void *pClientData;                                              // offset 0xC, size 0x4
};

struct ClassDestructorClient {
    void *reserved[2];                                 // offset 0x0, size 0x8
    void (*pClientFunc)(Class *pClass, void *pClientData); // offset 0x8, size 0x4
    void *pClientData;                                 // offset 0xC, size 0x4
};

struct Class {
    static int CreateInstance(ClassHandle *handle, void *memberData, Class **outClass);
    int Release();
    void SetMemberData(void *data);
    void GetRefCount(int *refcount);
    static int SubscribeConstructor(ClassHandle *pClassHandle, ClassConstructorClient *pClient);
    static int UnsubscribeConstructor(ClassHandle *pClassHandle, ClassConstructorClient *pClient);
    int SubscribeDestructor(ClassDestructorClient *pClient);
    int UnsubscribeDestructor(ClassDestructorClient *pClient);
    int SubscribeMemberData(FunctionClient *pClient);
    int UnsubscribeMemberData(FunctionClient *pClient);
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
