#include "../../../include/common/realmemcard/impl/interfaceimp.h"

namespace Realmc {

static int GetVersionNumber(const short *&pMsg) {
    int ver;
    bool found;

    ver = 0;
    while (*pMsg == ' ' || *pMsg == '.') {
        pMsg++;
    }

    while (*pMsg >= '0' && *pMsg <= '9') {
        ver = ver * 10;
        ver += *pMsg - '0';
        pMsg++;
    }

    return ver;
}

void InterfaceImp::CheckMessageCompatibility() {
    const short *pMsgVer;
    const short *pMinRealMCVer;
    int version;
    int major;
    int minor;
    int patch;

    pMsgVer = reinterpret_cast<const short *>(this->mISystem.mGetStrCallback(1));
    pMinRealMCVer = reinterpret_cast<const short *>(this->mISystem.mGetStrCallback(2));
    GetVersionNumber(pMsgVer);
    GetVersionNumber(pMsgVer);
    GetVersionNumber(pMsgVer);
    GetVersionNumber(pMinRealMCVer);
    GetVersionNumber(pMinRealMCVer);
    GetVersionNumber(pMinRealMCVer);
}

InterfaceImp::InterfaceImp(const SystemInterface &iSystem)
    : mISystem(iSystem) {
    this->mMutex = nullptr;
    this->mRefcount = 1;
    this->mISystem.mAllocator->AddRef();
    this->mISystem.mMutex->AddRef();
    this->mISystem.mThread->AddRef();
    Locale::SetLocaleGetStrCallback(this->mISystem.mGetStrCallback);
    this->mMutex = this->mISystem.mMutex->CreateInstance();
}

InterfaceImp::~InterfaceImp() {
    this->mMutex->Release();
    this->mISystem.mThread->Release();
    this->mISystem.mMutex->Release();
    this->mISystem.mAllocator->Release();
}

int InterfaceImp::AddRef() {
    this->mRefcount++;
    return this->mRefcount;
}

int InterfaceImp::Release() {
    this->ClearTask();
    this->mRefcount--;
    if (this->mRefcount == 0) {
        if (this == gInterface) {
            gInterface = nullptr;
        }
        if (this != nullptr) {
            delete this;
        }
    }
    return this->mRefcount;
}

Interface *gInterface = nullptr;

} // namespace Realmc
