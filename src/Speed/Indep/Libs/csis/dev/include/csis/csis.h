//
//
#ifndef CSIS_H
#define CSIS_H 1

#include <cstddef>
#include <Allocator/iallocator.h>

// TODO
// #define CSIS_VERSION_MAJOR 2 // Decl: 57
// #define CSIS_VERSION_MINOR 9 // Decl: 58
// #define CSIS_VERSION_PATCH 0 // Decl: 59
// #define CSIS_MAKE_VERSION(major,minor,patch) (((major) *10000) | ((minor) *100) | (patch)) // Decl: 60
// #define CSIS_VERSION CSIS_MAKE_VERSION(CSIS_VERSION_MAJOR, CSIS_VERSION_MINOR, CSIS_VERSION_PATCH) // Decl: 61
#define CSIS_SIZE_T size_t // Decl: 68

namespace Csis {

// Decl: 73
enum Result {
    RESULT_OK = 0,
    RESULT_ERR_MEM = -1,
    RESULT_ERR_ALLOCATE = -2,
    RESULT_ERR_HANDLEEXPIRED = -3,
    RESULT_ERR_UNUSED = -4,
    RESULT_ERR_NOTFOUND = -5,
    RESULT_ERR_UNINITIALIZED = -6,
    RESULT_ERR_ILLEGALOP = -7,
};

// Decl: 97
enum UnmatchedInterface {
    UNMATCHEDINTERFACE_FAIL = 0,
    UNMATCHEDINTERFACE_ABORT = 1,
};

// total size: 0x8
// Decl: 119
typedef struct {
    const char *pString; // offset 0x0, size 0x4
    short systemCrc;     // offset 0x4, size 0x2
    short interfaceCrc;  // offset 0x6, size 0x2
} InterfaceId;

// Decl: 161
typedef union {
    int iVal;   // offset 0x0, size 0x4
    float fVal; // offset 0x0, size 0x4
} Parameter;

// total size: 0x8
// Decl: 183
class FunctionHandle {
  public:
    FunctionHandle();

    Result Set(const InterfaceId *pInterfaceId);
    Result SetFast(const InterfaceId *pInterfaceId);
    Result Valid();

    void *operator new(size_t size);
    void operator delete(void *ptr);

  private:
    FunctionHandle(const InterfaceId *pInterfaceId) {}
    FunctionHandle &operator=(const FunctionHandle &);

    void *mpPrivate; // offset 0x0, size 0x4
    int mKey;        // offset 0x4, size 0x4
};

// total size: 0x8
// Decl: 355
class ClassHandle {
  public:
    ClassHandle();

    Result Set(const InterfaceId *pInterfaceId);
    Result SetFast(const InterfaceId *pInterfaceId);
    Result Valid();

    void *operator new(size_t size);
    void operator delete(void *ptr);

  private:
    ClassHandle(const InterfaceId *pInterfaceId) {}
    ClassHandle &operator=(const ClassHandle &);

    void *mpPrivate; // offset 0x0, size 0x4
    int mKey;        // offset 0x4, size 0x4
};

// total size: 0x8
// Decl: 443
class GlobalVariableHandle {
  public:
    GlobalVariableHandle(); // Decl: 445

    Result Set(const InterfaceId *pInterfaceId); // Decl: 470

    Result SetFast(const InterfaceId *pInterfaceId); // Decl: 471

    Result Valid(); // Decl: 482

    static void *operator new(size_t size); // Decl: 484
    static void operator delete(void *ptr); // Decl: 485

  private:
    GlobalVariableHandle(const InterfaceId *pInterfaceId) {}
    GlobalVariableHandle &operator=(const GlobalVariableHandle &);

    void *mpPrivate; // offset 0x0, size 0x4, Decl: 493
    int mKey;        // offset 0x4, size 0x4, Decl: 495
};

// total size: 0x10
// Decl: 519
typedef struct {
    void *reserved[2];                        // offset 0x0, size 0x8
    void (*pClientFunc)(Parameter *, void *); // offset 0x8, size 0x4
    void *pClientData;                        // offset 0xC, size 0x4
} FunctionClient;

typedef FunctionClient MemberDataClient;
typedef FunctionClient GlobalVariableClient;



// total size: 0x1
class Function {
public:
    static Result Call(FunctionHandle *pFunctionHandle, void *pParameters) ;

    static Result CallFast(FunctionHandle *pFunctionHandle, void *pParameters) ;

    static Result Subscribe(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) ;

    static Result SubscribeFast(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) ;

    static Result Unsubscribe(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) ;

    static Result UnsubscribeFast(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) ;
};

// total size: 0x1
class GlobalVariable {
public:
    static Result Set(GlobalVariableHandle *pHandle, void *pGlobalVariable) ;

    static Result SetFast(GlobalVariableHandle *pHandle, void *pGlobalVariable) ;

    static Result Subscribe(GlobalVariableHandle *pHandle, GlobalVariableClient *pClient) ;

    static Result SubscribeFast(GlobalVariableHandle *pHandle, GlobalVariableClient *pClient) ;

    static Result Unsubscribe(GlobalVariableHandle *pHandle, GlobalVariableClient *pClient) ;

    static Result UnsubscribeFast(GlobalVariableHandle *pHandle, GlobalVariableClient *pClient) ;
};

class Class;

// total size: 0x10
// Decl: 544
struct ClassConstructorClient {
    void *reserved[2];                                 // offset 0x0, size 0x8
    void (*pClientFunc)(Class *, Parameter *, void *); // offset 0x8, size 0x4
    void *pClientData;                                 // offset 0xC, size 0x4
};

// total size: 0x10
// Decl: 569
struct ClassDestructorClient {
    void *reserved[2];                    // offset 0x0, size 0x8
    void (*pClientFunc)(Class *, void *); // offset 0x8, size 0x4
    void *pClientData;                    // offset 0xC, size 0x4
};

// total size: 0x1
// Decl: 603
class System {
  public:
    static Result SetAllocator(EA::Allocator::IAllocator *pAllocator);
    static Result GetAllocator(EA::Allocator::IAllocator **ppAllocator);
    static Result SetAllocator(EA::Allocator::ICoreAllocator *pAllocator);
    static Result SetAbortMessage(void (*abortMessage)(const char *));
    static Result Init();
    static Result Restore();
    static bool IsInited();
    static Result Lock();
    static Result Unlock();
    static bool IsLocked();
    static Result Subscribe(void *pinterface);
    static Result Unsubscribe(void *pInterface);
    static unsigned char GetLibraryType();
    static void *Alloc(int size);
    static void Free(void *pMem);

  private:
    static void *AllocFast(int size);
    static void FreeFast(void *pMem);
    System();
    ~System();
    System(const System &);
    Csis::System &operator=(const Csis::System &);
};

// total size: 0x1
// Decl: 1029
class Class {
  public:
    static Result CreateInstance(ClassHandle *pClassHandle, void *pMemberData, Class **ppClass); // Decl: 1034

    static Result CreateInstanceFast(ClassHandle *pClassHandle, void *pMemberData, Class **ppClass); // Decl: 1036

    Result Release();     // Decl: 1040
    Result ReleaseFast(); // Decl: 1041

    Result SetMemberData(void *pMemberData);     // Decl: 1045
    Result SetMemberDataFast(void *pMemberData); // Decl: 1046

    Result GetRefCount(int *pRefCount); // Decl: 1058

    Result AddRef(); // Decl: 1060
    Result DecRef(); // Decl: 1061

    static Result SubscribeConstructor(ClassHandle *pClassHandle, ClassConstructorClient *pClient); // Decl: 1064

    static Result SubscribeConstructorFast(ClassHandle *pClassHandle, ClassConstructorClient *pClient); // Decl: 1066

    static Result UnsubscribeConstructor(ClassHandle *pClassHandle, ClassConstructorClient *pClient); // Decl: 1069

    static Result UnsubscribeConstructorFast(ClassHandle *pClassHandle, ClassConstructorClient *pClient); // Decl: 1071

    Result SubscribeDestructor(ClassDestructorClient *pClient);     // Decl: 1073
    Result SubscribeDestructorFast(ClassDestructorClient *pClient); // Decl: 1074

    Result UnsubscribeDestructor(ClassDestructorClient *pClient);     // Decl: 1076
    Result UnsubscribeDestructorFast(ClassDestructorClient *pClient); // Decl: 1077

    Result SubscribeMemberData(MemberDataClient *pClient);     // Decl: 1079
    Result SubscribeMemberDataFast(MemberDataClient *pClient); // Decl: 1080

    Result UnsubscribeMemberData(MemberDataClient *pClient);     // Decl: 1082
    Result UnsubscribeMemberDataFast(MemberDataClient *pClient); // Decl: 1083

  private:
    Class();  // Decl: 1124
    ~Class(); // Decl: 1125
};

}; // namespace Csis

#endif
