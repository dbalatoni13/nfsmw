#include <string.h>

#include <dolphin/os/OSMutex.h>

#include <csis/csis.h>
#include "csisi.h"

namespace Csis {

bool gIsAllocSet = false;
EA::Allocator::IAllocator *gpAllocator = NULL;
EA::Allocator::ICoreAllocator *gpCoreAllocator = NULL;
bool gIsAllocatorCreated = false;
bool gIsCoreAllocatorCreated = false;
bool gIsInited = false;
static short gUniqueKeyId = 0;
CListDStack gSystems;
OSMutex gMutexHandle;

struct IAllocatorToICoreAdaptor : public EA::Allocator::IAllocator {
    static void *operator new(size_t size, void *ptr) {
        return ptr;
    }

    IAllocatorToICoreAdaptor() {}
    static IAllocatorToICoreAdaptor *CreateInstance(EA::Allocator::ICoreAllocator *pCoreAllocator) {
        IAllocatorToICoreAdaptor *ptemp;
        ptemp = new (pCoreAllocator->Alloc(4, "CsisAlloc", 1)) IAllocatorToICoreAdaptor;
        return ptemp;
    }
    virtual int Release() {
        this->~IAllocatorToICoreAdaptor();
        gpCoreAllocator->Free(this, 0);
        return 0;
    }
    void *Alloc(size_t size, const char *name, unsigned int flags) {
        return gpCoreAllocator->Alloc(size, name, flags);
    }
    virtual void *Alloc(size_t size, const EA::TagValuePair &flags) {
        return gpCoreAllocator->Alloc(size, "CsisAlloc", 1);
    }
    virtual void Free(void *block, size_t size) {
        gpCoreAllocator->Free(block, 0);
    }
    virtual int AddRef() {
        return 0;
    }
};

struct ICoreToIAllocatorAdaptor : public EA::Allocator::ICoreAllocator {
    static void *operator new(size_t size, void *ptr) {
        return ptr;
    }

    ICoreToIAllocatorAdaptor() {}

    static ICoreToIAllocatorAdaptor *CreateInstance(EA::Allocator::IAllocator *pAllocator) {
        ICoreToIAllocatorAdaptor *ptemp = new (pAllocator->Alloc(sizeof(ICoreToIAllocatorAdaptor), NULLALLOCTVP)) ICoreToIAllocatorAdaptor;
        return ptemp;
    }

    void Release() {}

    virtual void *Alloc(size_t size, const char *name, unsigned int flags) {
        return gpAllocator->Alloc(size, NULLALLOCTVP);
    }
    virtual void *Alloc(size_t size, const char *name, unsigned int flags,
                        unsigned int align, unsigned int alignOffset) {
        return gpAllocator->Alloc(size, NULLALLOCTVP);
    }
    virtual void Free(void *block, size_t size) {
        gpAllocator->Free(block, 0);
    }
};

struct InterfaceHandleData {
    void *mpPrivate;
    int mKey;
};

inline Result ValidHandle(FunctionHandle *pHandle, FunctionDesc *pInterfaceDesc) {
    InterfaceHandleData *pInterfaceHandleData = reinterpret_cast<InterfaceHandleData *>(pHandle);

    if (pInterfaceHandleData->mpPrivate == NULL) {
        return RESULT_ERR_UNINITIALIZED;
    }
    if (pInterfaceHandleData->mKey != pInterfaceDesc->u.key) {
        pInterfaceHandleData->mKey = RESULT_ERR_HANDLEEXPIRED;
        pInterfaceHandleData->mpPrivate = NULL;
        return RESULT_ERR_HANDLEEXPIRED;
    } else {
        return RESULT_OK;
    }
}

inline Result ValidHandle(ClassHandle *pHandle, FunctionDesc *pInterfaceDesc) {
    InterfaceHandleData *pInterfaceHandleData = reinterpret_cast<InterfaceHandleData *>(pHandle);

    if (pInterfaceHandleData->mpPrivate == NULL) {
        return RESULT_ERR_UNINITIALIZED;
    }
    if (pInterfaceHandleData->mKey != pInterfaceDesc->u.key) {
        pInterfaceHandleData->mKey = RESULT_ERR_HANDLEEXPIRED;
        pInterfaceHandleData->mpPrivate = NULL;
        return RESULT_ERR_HANDLEEXPIRED;
    } else {
        return RESULT_OK;
    }
}

inline Result ValidHandle(GlobalVariableHandle *pHandle, GlobalVariableDesc *pInterfaceDesc) {
    InterfaceHandleData *pInterfaceHandleData = reinterpret_cast<InterfaceHandleData *>(pHandle);

    if (pInterfaceHandleData->mpPrivate == NULL) {
        return RESULT_ERR_UNINITIALIZED;
    }
    if (pInterfaceHandleData->mKey != pInterfaceDesc->u.key) {
        pInterfaceHandleData->mKey = RESULT_ERR_HANDLEEXPIRED;
        pInterfaceHandleData->mpPrivate = NULL;
        return RESULT_ERR_HANDLEEXPIRED;
    } else {
        return RESULT_OK;
    }
}

FunctionHandle::FunctionHandle() {
    this->mKey = RESULT_ERR_HANDLEEXPIRED;
}

Result FunctionHandle::Set(const InterfaceId *pInterfaceId) {
    Result result;

    System::Lock();
    result = this->SetFast(pInterfaceId);
    System::Unlock();
    return result;
}

template <typename Handle, typename Id, typename Desc>
inline Result SetHandle(Handle *pHandle, const Id *pInterfaceId, Desc *pInterfaceDesc,
                              unsigned int interfaceTypeOffset, unsigned int numInterfacesOffset) {
    bool tryUnmatched = false;
    char *pNode;
    SystemDesc *pSystemDesc;
    int nodeOffset = offsetof(SystemDesc, linkNode);

TryUnmatched:
    pNode = reinterpret_cast<char *>(gSystems.GetHead());
    while (pNode != NULL) {
        pSystemDesc = reinterpret_cast<SystemDesc *>(pNode - nodeOffset);
        pInterfaceDesc = *reinterpret_cast<Desc **>(reinterpret_cast<char *>(pSystemDesc) + interfaceTypeOffset);
        int numInterfaces = *reinterpret_cast<unsigned short *>(reinterpret_cast<char *>(pSystemDesc) + numInterfacesOffset);
        if (pSystemDesc->crc == pInterfaceId->systemCrc || tryUnmatched) {
            int i = 0;
            while (i < numInterfaces) {
                if (pInterfaceId->interfaceCrc == (pInterfaceDesc + i)->u.crcAndKey.crc &&
                    strcmp(pInterfaceId->pString, (pInterfaceDesc + i)->pStringId) == 0) {
                    InterfaceHandleData *pInterfaceHandleData = reinterpret_cast<InterfaceHandleData *>(pHandle);
                    pInterfaceHandleData->mpPrivate = pInterfaceDesc + i;
                    pInterfaceHandleData->mKey = (pInterfaceDesc + i)->u.key;
                    return RESULT_OK;
                }
                i++;
            }
        }
        pNode = reinterpret_cast<char *>(pSystemDesc->linkNode.GetNext());
    }
    if (tryUnmatched) {
        return RESULT_ERR_NOTFOUND;
    }
    tryUnmatched = true;
    goto TryUnmatched;
}

Result FunctionHandle::SetFast(const InterfaceId *pInterfaceId) {
    return SetHandle(this, pInterfaceId, static_cast<FunctionDesc *>(NULL), 0x14, 0xA);
}

Result FunctionHandle::Valid() {
    if (this->mKey < 0) {
        return static_cast<Result>(this->mKey);
    }
    FunctionDesc *pFunctionDesc;
    pFunctionDesc = reinterpret_cast<FunctionDesc *>(this->mpPrivate);
    return ValidHandle(this, pFunctionDesc);
}

ClassHandle::ClassHandle() {
    this->mKey = RESULT_ERR_HANDLEEXPIRED;
}

Result ClassHandle::Set(const InterfaceId *pInterfaceId) {
    Result result;

    System::Lock();
    result = this->SetFast(pInterfaceId);
    System::Unlock();
    return result;
}

Result ClassHandle::SetFast(const InterfaceId *pInterfaceId) {
    return SetHandle(this, pInterfaceId, static_cast<FunctionDesc *>(NULL), 0x18, 0xC);
}

Result ClassHandle::Valid() {
    if (this->mKey < 0) {
        return static_cast<Result>(this->mKey);
    }
    FunctionDesc *pClassDesc;
    pClassDesc = reinterpret_cast<FunctionDesc *>(this->mpPrivate);
    return ValidHandle(this, pClassDesc);
}

Result GlobalVariableHandle::SetFast(const InterfaceId *pInterfaceId) {
    return SetHandle(this, pInterfaceId, static_cast<GlobalVariableDesc *>(NULL), 0x1C, 0xE);
}

Result GlobalVariableHandle::Valid() {
    if (this->mKey < 0) {
        return static_cast<Result>(this->mKey);
    }
    GlobalVariableDesc *pGlobalVariableDesc;
    pGlobalVariableDesc = reinterpret_cast<GlobalVariableDesc *>(this->mpPrivate);
    return ValidHandle(this, pGlobalVariableDesc);
}

void ClassData::SendParameters(Parameter *pParameters) {
    FunctionClient *pClient = reinterpret_cast<FunctionClient *>(this->mUpdateClients.GetHead());

    while (pClient != NULL) {
        pClient->pClientFunc(pParameters, pClient->pClientData);
        CListDNode *pNode = reinterpret_cast<CListDNode *>(pClient)->GetNext();
        pClient = reinterpret_cast<FunctionClient *>(pNode);
    }
}

inline short GenerateUniqueKeyId() {
    gUniqueKeyId++;
    if (gUniqueKeyId & 0x8000) {
        gUniqueKeyId = 1;
    }
    return gUniqueKeyId;
}

Result System::SetAllocator(EA::Allocator::ICoreAllocator *pAllocator) {
    if (!gIsInited) {
        if (gpAllocator != NULL) {
            gpAllocator = NULL;
        }
        gpCoreAllocator = pAllocator;
        gIsAllocSet = true;
        return RESULT_OK;
    }
    return RESULT_ERR_ILLEGALOP;
}

void *System::Alloc(int size) {
    void *pMem;

    System::Lock();
    pMem = System::AllocFast(size);
    System::Unlock();
    return pMem;
}

void System::Free(void *pMem) {
    System::Lock();
    System::FreeFast(pMem);
    System::Unlock();
}

void *System::AllocFast(int size) {
    return gpCoreAllocator->Alloc(size, "CsisAlloc", 1);
}

void System::FreeFast(void *pMem) {
    gpCoreAllocator->Free(pMem, 0);
}

Result System::Init() {
    if (gpCoreAllocator == NULL) {
        gpCoreAllocator = ICoreToIAllocatorAdaptor::CreateInstance(gpAllocator);
        gIsCoreAllocatorCreated = true;
    } else if (gpAllocator == NULL) {
        gpAllocator = IAllocatorToICoreAdaptor::CreateInstance(gpCoreAllocator);
        gIsAllocatorCreated = true;
    }
    OSInitMutex(&gMutexHandle);
    gSystems.Reset();
    gIsInited = true;
    return RESULT_OK;
}

Result System::Lock() {
    OSLockMutex(&gMutexHandle);
    return RESULT_OK;
}

Result System::Unlock() {
    OSUnlockMutex(&gMutexHandle);
    return RESULT_OK;
}

Result System::Subscribe(void *pinterface) {
    SystemDesc *pSystemDesc = reinterpret_cast<SystemDesc *>(pinterface);
    int i;
    unsigned int ptr;

    pSystemDesc->pFunctionDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<char *>(pSystemDesc) + 0x28);
    pSystemDesc->pClassDesc = pSystemDesc->pFunctionDesc + pSystemDesc->numFunctions;
    pSystemDesc->pGlobalVariableDesc = reinterpret_cast<GlobalVariableDesc *>(pSystemDesc->pClassDesc + pSystemDesc->numClasses);

    for (i = 0; i < pSystemDesc->numFunctions; i++) {
        ptr = reinterpret_cast<unsigned int>(pSystemDesc->pFunctionDesc[i].pStringId);
        pSystemDesc->pFunctionDesc[i].pStringId = reinterpret_cast<const char *>(
            ptr + reinterpret_cast<unsigned int>(pSystemDesc));
        pSystemDesc->pFunctionDesc[i].u.crcAndKey.key = GenerateUniqueKeyId();
    }
    for (i = 0; i < pSystemDesc->numClasses; i++) {
        ptr = reinterpret_cast<unsigned int>(pSystemDesc->pClassDesc[i].pStringId);
        pSystemDesc->pClassDesc[i].pStringId = reinterpret_cast<const char *>(
            ptr + reinterpret_cast<unsigned int>(pSystemDesc));
        pSystemDesc->pClassDesc[i].u.crcAndKey.key = GenerateUniqueKeyId();
    }
    for (i = 0; i < pSystemDesc->numGlobalVariables; i++) {
        ptr = reinterpret_cast<unsigned int>(pSystemDesc->pGlobalVariableDesc[i].pStringId);
        pSystemDesc->pGlobalVariableDesc[i].pStringId = reinterpret_cast<const char *>(
            ptr + reinterpret_cast<unsigned int>(pSystemDesc));
        pSystemDesc->pGlobalVariableDesc[i].u.crcAndKey.key = GenerateUniqueKeyId();
    }

    gSystems.Push(&pSystemDesc->linkNode);
    return RESULT_OK;
}

Result System::Unsubscribe(void *pInterface) {
    SystemDesc *pSystemDesc = reinterpret_cast<SystemDesc *>(pInterface);
    int i;

    for (i = 0; i < pSystemDesc->numFunctions; i++) {
        pSystemDesc->pFunctionDesc[i].u.key = -1;
    }
    for (i = 0; i < pSystemDesc->numClasses; i++) {
        pSystemDesc->pClassDesc[i].u.key = -1;
    }
    for (i = 0; i < pSystemDesc->numGlobalVariables; i++) {
        pSystemDesc->pGlobalVariableDesc[i].u.key = -1;
    }

    gSystems.Remove(&pSystemDesc->linkNode);
    return RESULT_OK;
}

Result Function::Call(FunctionHandle *pFunctionHandle, void *pParameters) {
    Result result;

    System::Lock();
    result = Function::CallFast(pFunctionHandle, pParameters);
    System::Unlock();
    return result;
}

Result Function::CallFast(FunctionHandle *pFunctionHandle, void *pParameters) {
    Result result = pFunctionHandle->Valid();
    FunctionDesc *pFunctionDesc;
    FunctionClient *pClient;

    if (result < 0) {
        return result;
    }
    pFunctionDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<InterfaceHandleData *>(pFunctionHandle)->mpPrivate);
    pClient = reinterpret_cast<FunctionClient *>(reinterpret_cast<CListDStack *>(pFunctionDesc)->GetHead());
    if (pClient == NULL) {
        return RESULT_ERR_UNUSED;
    }
    while (pClient != NULL) {
        pClient->pClientFunc(reinterpret_cast<Parameter *>(pParameters), pClient->pClientData);
        CListDNode *pNode = reinterpret_cast<CListDNode *>(pClient)->GetNext();
        pClient = reinterpret_cast<FunctionClient *>(pNode);
    }
    return result;
}

Result Function::Subscribe(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) {
    Result result;

    System::Lock();
    result = Function::SubscribeFast(pFunctionHandle, pFunctionClient);
    System::Unlock();
    return result;
}

Result Function::SubscribeFast(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) {
    Result result = pFunctionHandle->Valid();
    FunctionDesc *pFunctionDesc;

    if (result >= 0) {
        pFunctionDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<InterfaceHandleData *>(pFunctionHandle)->mpPrivate);
        reinterpret_cast<CListDStack *>(pFunctionDesc)->Push(reinterpret_cast<CListDNode *>(pFunctionClient));
        result = RESULT_OK;
    }
    return result;
}

Result Function::UnsubscribeFast(FunctionHandle *pFunctionHandle, FunctionClient *pFunctionClient) {
    Result result = pFunctionHandle->Valid();
    FunctionDesc *pFunctionDesc;

    if (result >= 0) {
        pFunctionDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<InterfaceHandleData *>(pFunctionHandle)->mpPrivate);
        reinterpret_cast<CListDStack *>(pFunctionDesc)->Remove(reinterpret_cast<CListDNode *>(pFunctionClient));
        result = RESULT_OK;
    }
    return result;
}

Result Class::CreateInstance(ClassHandle *pClassHandle, void *pMemberData, Class **ppClass) {
    Result result;

    System::Lock();
    result = Class::CreateInstanceFast(pClassHandle, pMemberData, ppClass);
    System::Unlock();
    return result;
}

Result Class::CreateInstanceFast(ClassHandle *pClassHandle, void *pMemberData, Class **ppClass) {
    ClassConstructorClient *pClient;
    ClassData *pClassData;
    Result result;

    *ppClass = NULL;
    result = pClassHandle->Valid();
    if (result < 0) {
        return result;
    }
    pClassData = reinterpret_cast<ClassData *>(System::AllocFast(0x10));
    if (pClassData == NULL) {
        return RESULT_ERR_MEM;
    }
    pClassData->mUpdateClients.Reset();
    pClassData->mRefCount = 1;
    pClassData->mDestroyClients.Reset();
    pClassData->mpClassDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<InterfaceHandleData *>(pClassHandle)->mpPrivate);
    pClient = reinterpret_cast<ClassConstructorClient *>(reinterpret_cast<CListDStack *>(pClassData->mpClassDesc)->GetHead());
    while (pClient != NULL) {
        pClient->pClientFunc(reinterpret_cast<Class *>(pClassData), reinterpret_cast<Parameter *>(pMemberData), pClient->pClientData);
        CListDNode *pNode = reinterpret_cast<CListDNode *>(pClient)->GetNext();
        pClient = reinterpret_cast<ClassConstructorClient *>(pNode);
    }
    pClassData->SendParameters(reinterpret_cast<Parameter *>(pMemberData));
    *ppClass = reinterpret_cast<Class *>(pClassData);
    return RESULT_OK;
}

Result Class::Release() {
    Result result;

    System::Lock();
    result = this->ReleaseFast();
    System::Unlock();
    return result;
}

Result Class::ReleaseFast() {
    ClassData *pClass = reinterpret_cast<ClassData *>(this);
    ClassDestructorClient *pClient = reinterpret_cast<ClassDestructorClient *>(pClass->mDestroyClients.GetHead());

    while (pClient != NULL) {
        CListDNode *pNode = reinterpret_cast<CListDNode *>(pClient)->GetNext();
        pClient->pClientFunc(this, pClient->pClientData);
        pClient = reinterpret_cast<ClassDestructorClient *>(pNode);
    }
    pClass->DecRefInline();
    return RESULT_OK;
}

Result Class::GetRefCount(int *pRefCount) {
    ClassData *pClass = reinterpret_cast<ClassData *>(this);
    *pRefCount = pClass->mRefCount;
    return RESULT_OK;
}

Result Class::SetMemberData(void *pMemberData) {
    Result result;

    System::Lock();
    result = this->SetMemberDataFast(pMemberData);
    System::Unlock();
    return result;
}

Result Class::SetMemberDataFast(void *pMemberData) {
    reinterpret_cast<ClassData *>(this)->SendParameters(reinterpret_cast<Parameter *>(pMemberData));
    return RESULT_OK;
}

Result Class::SubscribeConstructorFast(ClassHandle *pClassHandle, ClassConstructorClient *pClient) {
    Result result = pClassHandle->Valid();
    FunctionDesc *pClassDesc;

    if (result >= 0) {
        pClassDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<InterfaceHandleData *>(pClassHandle)->mpPrivate);
        reinterpret_cast<CListDStack *>(pClassDesc)->Push(reinterpret_cast<CListDNode *>(pClient));
        result = RESULT_OK;
    }
    return result;
}

Result Class::UnsubscribeConstructor(ClassHandle *pClassHandle, ClassConstructorClient *pClient) {
    Result result;

    System::Lock();
    result = Class::UnsubscribeConstructorFast(pClassHandle, pClient);
    System::Unlock();
    return result;
}

Result Class::UnsubscribeConstructorFast(ClassHandle *pClassHandle, ClassConstructorClient *pClient) {
    Result result = pClassHandle->Valid();
    FunctionDesc *pClassDesc;

    if (result >= 0) {
        pClassDesc = reinterpret_cast<FunctionDesc *>(reinterpret_cast<InterfaceHandleData *>(pClassHandle)->mpPrivate);
        reinterpret_cast<CListDStack *>(pClassDesc)->Remove(reinterpret_cast<CListDNode *>(pClient));
        result = RESULT_OK;
    }
    return result;
}

Result Class::SubscribeDestructor(ClassDestructorClient *pClient) {
    Result result;

    System::Lock();
    result = this->SubscribeDestructorFast(pClient);
    System::Unlock();
    return result;
}

Result Class::SubscribeDestructorFast(ClassDestructorClient *pClient) {
    ClassData *pClass = reinterpret_cast<ClassData *>(this);
    pClass->mDestroyClients.Push(reinterpret_cast<CListDNode *>(pClient));
    pClass->AddRefInline();
    return RESULT_OK;
}

Result Class::UnsubscribeDestructor(ClassDestructorClient *pClient) {
    Result result;

    System::Lock();
    result = this->UnsubscribeDestructorFast(pClient);
    System::Unlock();
    return result;
}

Result Class::UnsubscribeDestructorFast(ClassDestructorClient *pClient) {
    ClassData *pClass = reinterpret_cast<ClassData *>(this);
    pClass->mDestroyClients.Remove(reinterpret_cast<CListDNode *>(pClient));
    pClass->DecRefInline();
    return RESULT_OK;
}

Result Class::SubscribeMemberData(MemberDataClient *pClient) {
    Result result;

    System::Lock();
    result = this->SubscribeMemberDataFast(pClient);
    System::Unlock();
    return result;
}

Result Class::SubscribeMemberDataFast(MemberDataClient *pClient) {
    ClassData *pClass = reinterpret_cast<ClassData *>(this);
    pClass->mUpdateClients.Push(reinterpret_cast<CListDNode *>(pClient));
    pClass->AddRefInline();
    return RESULT_OK;
}

Result Class::UnsubscribeMemberData(MemberDataClient *pClient) {
    Result result;

    System::Lock();
    result = this->UnsubscribeMemberDataFast(pClient);
    System::Unlock();
    return result;
}

Result Class::UnsubscribeMemberDataFast(MemberDataClient *pClient) {
    ClassData *pClass = reinterpret_cast<ClassData *>(this);
    pClass->mUpdateClients.Remove(reinterpret_cast<CListDNode *>(pClient));
    pClass->DecRefInline();
    return RESULT_OK;
}

Result GlobalVariable::SetFast(GlobalVariableHandle *pHandle, void *pGlobalVariable) {
    Result result = pHandle->Valid();
    GlobalVariableDesc *pGlobalVariableDesc;
    FunctionClient *pClient;

    if (result < 0) {
        return result;
    }
    pGlobalVariableDesc = reinterpret_cast<GlobalVariableDesc *>(reinterpret_cast<InterfaceHandleData *>(pHandle)->mpPrivate);
    if (*reinterpret_cast<int *>(pGlobalVariable) == pGlobalVariableDesc->curVal.iVal) {
        return RESULT_OK;
    }
    pGlobalVariableDesc->curVal.iVal = *reinterpret_cast<int *>(pGlobalVariable);
    pClient = reinterpret_cast<FunctionClient *>(reinterpret_cast<CListDStack *>(pGlobalVariableDesc)->GetHead());
    while (pClient != NULL) {
        pClient->pClientFunc(&pGlobalVariableDesc->curVal, pClient->pClientData);
        CListDNode *pNode = reinterpret_cast<CListDNode *>(pClient)->GetNext();
        pClient = reinterpret_cast<FunctionClient *>(pNode);
    }
    return result;
}

Result GlobalVariable::SubscribeFast(GlobalVariableHandle *pHandle, GlobalVariableClient *pClient) {
    Result result = pHandle->Valid();
    GlobalVariableDesc *pGlobalVariableDesc;

    if (result >= 0) {
        pGlobalVariableDesc = reinterpret_cast<GlobalVariableDesc *>(reinterpret_cast<InterfaceHandleData *>(pHandle)->mpPrivate);
        reinterpret_cast<CListDStack *>(pGlobalVariableDesc)->Push(reinterpret_cast<CListDNode *>(pClient));
        pClient->pClientFunc(&pGlobalVariableDesc->curVal, pClient->pClientData);
        result = RESULT_OK;
    }
    return result;
}

Result GlobalVariable::UnsubscribeFast(GlobalVariableHandle *pHandle, GlobalVariableClient *pClient) {
    Result result = pHandle->Valid();
    GlobalVariableDesc *pGlobalVariableDesc;

    if (result >= 0) {
        pGlobalVariableDesc = reinterpret_cast<GlobalVariableDesc *>(reinterpret_cast<InterfaceHandleData *>(pHandle)->mpPrivate);
        reinterpret_cast<CListDStack *>(pGlobalVariableDesc)->Remove(reinterpret_cast<CListDNode *>(pClient));
        result = RESULT_OK;
    }
    return result;
}

}
