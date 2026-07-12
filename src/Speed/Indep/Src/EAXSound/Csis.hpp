#ifndef EAXSOUND_CSIS_HPP
#define EAXSOUND_CSIS_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace Csis {
struct Class;

enum Type_location_region {
    Type_location_region_college_town = 1,
    Type_location_region_coastal = 2,
    Type_location_region_coastal_extra = 4,
    Type_location_region_city = 8,
};

enum Type_location {
    Type_location_Location_1 = 1,
    Type_location_Location_2 = 2,
    Type_location_Location_3 = 4,
    Type_location_Location_4 = 8,
    Type_location_Location_5 = 16,
    Type_location_Location_6 = 32,
    Type_location_Location_7 = 64,
    Type_location_Location_8 = 128,
    Type_location_Location_9 = 256,
    Type_location_Location_10 = 512,
    Type_location_Location_11 = 1024,
    Type_location_Location_12 = 2048,
    Type_location_Location_13 = 4096,
    Type_location_Location_14 = 8192,
    Type_location_Location_15 = 16384,
    Type_location_Location_16 = 32768,
    Type_location_Location_17 = 65536,
    Type_location_Location_18 = 131072,
    Type_location_Location_19 = 262144,
    Type_location_Location_20 = 524288,
    Type_location_Location_21 = 1048576,
    Type_location_Location_22 = 2097152,
    Type_location_Location_23 = 4194304,
    Type_location_Location_24 = 8388608,
    Type_location_Location_25 = 16777216,
    Type_location_Location_26 = 33554432,
    Type_location_Location_27 = 67108864,
    Type_location_Location_28 = 134217728,
    Type_location_Location_29 = 268435456,
    Type_location_Location_30 = 536870912,
    Type_location_Location_31 = 1073741824,
};

enum Type_direction {
    Type_direction_Northbound = 1,
    Type_direction_Southbound = 2,
    Type_direction_Eastbound = 4,
    Type_direction_Westbound = 8,
};

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
Result Unsubscribe(void *desc);
void *Alloc(int bytes);
void Free(void *memory);
} // namespace System

struct Function {
    static System::Result Call(FunctionHandle *pFunctionHandle, void *pParameters);
};
} // namespace Csis

#endif
