#ifndef __MEMORYCARDHELPER_HPP__
#define __MEMORYCARDHELPER_HPP__

#include <types.h>
#include <string.h>

#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

#include "realcore/system.h"
#include "realmemcard/memcard_interface.h"
#include "realmemcard/memcard_system.h"

class MyMutex : public Realmc::IMutex {
  private:
    MUTEX mMutex;  // offset 0x4, size 0x1C
    int mRefcount; // offset 0x20, size 0x4

  public:
    MyMutex() : mMutex(), mRefcount(1) {
#ifndef FIX_BUGS // unnecessary AND broken
        bMemSet(&mMutex, sizeof(MUTEX), 0);
#endif
        MUTEX_create(&mMutex);
    }
    virtual ~MyMutex() { // Decl: 49
        MUTEX_destroy(&mMutex);
    }
    IMutex *CreateInstance() override { // Decl: 50
        return new ("Realmc::IMutex", 0) MyMutex();
    };
    int AddRef() override { // Decl: 51
        return ++mRefcount;
    };

    int Release() override { // Decl: 54
        mRefcount--;
        if (mRefcount <= 0) {
            delete this;
            return 0;
        }
        return mRefcount;
    };
    void Lock() override { // Decl: 63
        MUTEX_lock(&mMutex);
    };
    void Unlock() override { // Decl: 64
        MUTEX_unlock(&mMutex);
    };
};

class MyThread : public Realmc::IThread {
  private:
    int mRefcount;                      // offset 0x4, size 0x4
    Realmc::ThreadEntryFunc mEntryFunc; // offset 0x8, size 0x4
    unsigned int mStackSize;            // offset 0xC, size 0x4
    void *mStackBuffer;                 // offset 0x10, size 0x4
    THREAD mThreadData;                 // offset 0x14, size 0x318
    int mPriority;                      // offset 0x32C, size 0x4
    bool mActive;                       // offset 0x330, size 0x1

  public:
    MyThread() : mRefcount(1), mStackSize(0x1000), mStackBuffer(nullptr), mThreadData(), mPriority(0), mActive(false) {}

    virtual ~MyThread() { // Decl: 80
        if (mActive) {
            WaitForEnd(0);
            THREAD_destroy(&mThreadData);
        }
    }

    IThread *CreateInstance() override { // Decl: 89
        return new ("Realmc::IThread", 0) MyThread();
    };
    int AddRef() override { // Decl: 94
        return ++mRefcount;
    };
    int Release() override { // Decl: 99
        mRefcount--;
        if (mRefcount <= 0) {
            delete this;
            return 0;
        }
        return mRefcount;
    };
    void SetStackSize(unsigned int stacksize) override { // Decl: 109
        mStackSize = stacksize;
    }
    static int EntryProc(void *pContext) { // Decl: 115
        MyThread *pThread = static_cast<MyThread *>(pContext);
        while (!pThread->MyThread::IsActive()) {
            THREAD_yield(1);
        }
        pThread->MyThread::GetEntryFunc()(pContext);
        return 0;
    };
    void Begin(Realmc::ThreadEntryFunc func) override { // Decl: 129
        mEntryFunc = func;
        mStackBuffer = new char[mStackSize];
        THREAD_create(&mThreadData, EntryProc, this, mStackBuffer, mStackSize, mPriority);
        mActive = true;
    };
    void WaitForEnd(int) override { // Decl: 137
        THREAD_waitexit(&mThreadData, 0);
        delete[] static_cast<char *>(mStackBuffer);
        mActive = false;
    };
    void Sleep(int ticks) override { // Decl: 143
        THREAD_yield(ticks);
    };
    void SetPriority(int priority) override { // Decl: 148
#ifdef EA_PLATFORM_WIN32
        mPriority = priority;
        THREAD_setpriority(&mThreadData, priority);
#else
        mPriority = 0;
        THREAD_setpriority(&mThreadData, 0);
#endif
    };
    virtual Realmc::ThreadEntryFunc GetEntryFunc() { // Decl: 153
        return mEntryFunc;
    };
    virtual bool IsActive() { // Decl: 154
        return mActive;
    };
};

// Decl: 22
enum MemoryCardJoyLoggableEvents {
    MJ_None = 0,
    MJ_ShowMesssage = 1,
    MJ_ClearMessage = 2,
    MJ_BootupCheckDone = 3,
    MJ_SaveCheckDone = 4,
    MJ_SaveDone = 5,
    MJ_CheckLoadedData = 6,
    MJ_LoadDone = 7,
    MJ_DeleteDone = 8,
    MJ_ClearEntries = 9,
    MJ_FoundEntry = 10,
    MJ_FindEntriesDone = 11,
    MJ_Retry = 12,
    MJ_Failed = 13,
    MJ_CardChecked = 14,
    MJ_CardRemoved = 15,
    MJ_SetAutosaveDone = 16,
    MJ_LoadReady = 17,
#ifndef EA_BUILD_A124
    MJ_SetMonitorDone = 18,
#endif
};

// total size: 0x1
// Decl: 47
class IJoyHelper {
  public:
    void JLog(const char *msg) { // Decl: 64
        Joylog::AddOrGetData(const_cast<char *>(msg), JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(bool &value) { // Decl: 86
        value = Joylog::AddOrGetData(static_cast<unsigned int>(value), 1, JOYLOG_CHANNEL_MEMORY_CARD) != 0;
    }

    void JLog(MemoryCardJoyLoggableEvents op) { // Decl: 108
        if (Joylog::IsCapturing())
            Joylog::AddData(static_cast<int>(op), 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(void *data, int data_size_bytes) { // Decl: 139
        Joylog::AddData(data, data_size_bytes, JOYLOG_CHANNEL_MEMORY_CARD);
    }

    static void EmulateMemoryCardLibrary(int aJoyOp); // Decl: 169

    void JLog(const wchar_t *msg) {
        Joylog::AddOrGetData(reinterpret_cast<uint16 *>(const_cast<wchar_t *>(msg)), JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(unsigned int &value) {
        value = Joylog::AddOrGetData(value, 0x20, JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(RealmcIface::CardId &id) {}

    void JLog(RealmcIface::CardStatus &status) {
        unsigned int value = static_cast<unsigned int>(status);
        status = static_cast<RealmcIface::CardStatus>(Joylog::AddOrGetData(value, 0x10, JOYLOG_CHANNEL_MEMORY_CARD));
    }

    void JLog(RealmcIface::MonitorState &state) {}

    void JLog(const RealmcIface::CardInfo *pInfo) {
        RealmcIface::CardInfo *pVal = const_cast<RealmcIface::CardInfo *>(pInfo);
        JLog(pVal->mCardId);
        JLog(const_cast<RealmcIface::CardStatus &>(pVal->mStatus));
        JLog(const_cast<unsigned int &>(pVal->mFreeSpace));
        JLog(const_cast<unsigned int &>(pVal->mFreeFiles));
        JLog(const_cast<unsigned int &>(pVal->mTotalSpace));
        JLog(const_cast<bool &>(pInfo->mFreeSpaceOverLimit));
        JLog(const_cast<bool &>(pInfo->mTotalSpaceOverLimit));
    }

    void JLog(RealmcIface::TaskResult &res) {
        res = static_cast<RealmcIface::TaskResult>(Joylog::AddOrGetData(static_cast<unsigned int>(res), 8, JOYLOG_CHANNEL_MEMORY_CARD));
    }

    void JLog(const RealmcIface::EntryInfo *info) {
        RealmcIface::EntryInfo *e = const_cast<RealmcIface::EntryInfo *>(info);
        JLog(e->mName);
        JLog(const_cast<RealmcIface::CardStatus &>(e->mStatus));
        JLog(const_cast<unsigned int &>(e->mEntryBlocks));
        JLog(const_cast<unsigned int &>(e->mUserDataSize));
        JLog(const_cast<unsigned int &>(e->mTime.mCreated));
        JLog(const_cast<unsigned int &>(e->mTime.mLastAccessed));
        JLog(const_cast<unsigned int &>(e->mTime.mLastModified));
        JLog(e->mCompanyCode);
        JLog(e->mGameCode);
    }
};

#endif
