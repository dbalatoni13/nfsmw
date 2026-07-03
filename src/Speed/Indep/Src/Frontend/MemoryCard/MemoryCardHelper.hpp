#ifndef __MEMORYCARDHELPER_HPP__
#define __MEMORYCARDHELPER_HPP__

#include <types.h>
#include <string.h>

#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"
#include "Packages/realmemcard/3.04.01-layer2/include/common/realmemcard/memcard_interface.h"

#include "Speed/Indep/Src/Misc/Joylog.hpp"

struct IAllocator;

typedef enum {
    IDLE_PRIORITY = -3,
    LOW_PRIORITY = -2,
    BELOW_PRIORITY = -1,
    NORM_PRIORITY = 0,
    ABOVE_PRIORITY = 1,
    HIGH_PRIORITY = 2,
    CRIT_PRIORITY = 3
} Priority;

typedef int (*ThreadEntryFunc)(void *);

class IThread {
  public:
    virtual ~IThread() {}
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual IThread *CreateInstance() = 0;
    virtual void SetStackSize(unsigned int stacksize) = 0;
    virtual void Begin(ThreadEntryFunc func) = 0;
    virtual void WaitForEnd(int) = 0;
    virtual void Sleep(int ticks) = 0;
    virtual ThreadEntryFunc GetEntryFunc() = 0;
    virtual bool IsActive() = 0;
};

class IMutex {
  public:
    virtual ~IMutex() {}
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual IMutex *CreateInstance() = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

struct THREAD {
    int reserved[198]; // offset 0x0, size 0x318
};

void THREAD_sleep(int ticks);
void THREAD_create(THREAD *thread, int (*func)(void *), void *param, void *stack, int stackSize, int priority);
void THREAD_waitexit(THREAD *thread, int status);
void THREAD_setpriority(THREAD *thread, int priority);
void THREAD_yield(int ticks);
void THREAD_destroy(THREAD *thread);

class MyMutex : public IMutex {
  private:
    MUTEX mMutex;  // offset 0x4, size 0x1C
    int mRefcount; // offset 0x20, size 0x4

  public:
    MyMutex() {
        memset(&mMutex, 0, sizeof(MUTEX));
        mRefcount = 1;
        MUTEX_create(&mMutex);
    }
    ~MyMutex() { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:49
        MUTEX_destroy(&mMutex);
    }
    IMutex *CreateInstance() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:50
        return new MyMutex();
    };
    int AddRef() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:51
        return ++mRefcount;
    };

    int Release() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:54
        int ref = --mRefcount;
        if (ref > 0) {
            return ref;
        }
        if (this != nullptr) {
            delete this;
        }
        return 0;
    };
    void Lock() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:63
        MUTEX_lock(&mMutex);
    };
    void Unlock() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:64
        MUTEX_unlock(&mMutex);
    };
};

class MyThread : public IThread {
  private:
    int mRefcount;              // offset 0x4, size 0x4
    ThreadEntryFunc mEntryFunc; // offset 0x8, size 0x4
    unsigned int mStackSize;    // offset 0xC, size 0x4
    void *mStackBuffer;         // offset 0x10, size 0x4
    THREAD mThreadData;         // offset 0x14, size 0x318
    int mPriority;              // offset 0x32C, size 0x4
    bool mActive;               // offset 0x330, size 0x1

  public:
    MyThread() {
        mRefcount = 1;
        mStackSize = 0x1000;
        mStackBuffer = nullptr;
        memset(&mThreadData, 0, sizeof(THREAD));
        mPriority = 0;
        mActive = false;
    }

    ~MyThread() { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:80
        if (mActive) {
            WaitForEnd(0);
            THREAD_destroy(&mThreadData);
        }
    }

    IThread *CreateInstance() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:89
        return new MyThread();
    };
    int AddRef() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:94
        return ++mRefcount;
    };
    int Release() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:99
        int ref = --mRefcount;
        if (ref > 0) {
            return ref;
        }
        if (this != nullptr) {
            delete this;
        }
        return 0;
    };
    void SetStackSize(unsigned int stacksize) override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:109
        mStackSize = stacksize;
    }
    static int EntryProc(void *pContext) { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:115
        MyThread *pThread = static_cast<MyThread *>(pContext);
        while (!pThread->MyThread::IsActive()) {
            THREAD_yield(1);
        }
        pThread->MyThread::GetEntryFunc()(pContext);
        return 0;
    };
    void Begin(ThreadEntryFunc func) override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:129
        mEntryFunc = func;
        mStackBuffer = new char[mStackSize];
        THREAD_create(&mThreadData, EntryProc, this, mStackBuffer, mStackSize, mPriority);
        mActive = true;
    };
    void WaitForEnd(int) override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:137
        THREAD_waitexit(&mThreadData, 0);
        if (mStackBuffer != nullptr) {
            delete[] static_cast<char *>(mStackBuffer);
        }
        mActive = false;
    };
    void Sleep(int ticks) override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:143
        THREAD_sleep(ticks);
    };
    void SetPriority(int priority) override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:148
        mPriority = 0;
        THREAD_setpriority(&mThreadData, 0);
    };
    ThreadEntryFunc GetEntryFunc() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:153
        return mEntryFunc;
    };
    bool IsActive() override { // Decl: speed/indep/src/frontend/MemoryCard/MemoryCardHelper.hpp:154
        return mActive;
    };
};

// TODO belongs to C:/packages/realmemcard/3.04.01-layer2
namespace Realmc {

// total size: 0x10
struct SystemInterface {
    IAllocator *mAllocator;              // offset 0x0, size 0x4
    IThread *mThread;                    // offset 0x4, size 0x4
    IMutex *mMutex;                      // offset 0x8, size 0x4
    const char *(*mGetStrCallback)(int); // offset 0xC, size 0x4

    void Clear();
};

} // namespace Realmc

struct MemoryCard;
struct UIMemcardBase;

// TODO belongs to C:/packages/realmemcard/3.04.01-layer2
struct IGameInterface {
    virtual void ShowMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t **options) = 0;
    virtual void ClearMessage() = 0;
    virtual void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults res) = 0;
    virtual void SaveCheckDone(RealmcIface::TaskResult result, RealmcIface::CardStatus status) = 0;
    virtual void SaveDone(const char *filename) = 0;
    virtual RealmcIface::DataStatus CheckLoadedData(const char *data) = 0;
    virtual void LoadDone(const char *filename) = 0;
    virtual void DeleteDone(const char *filename) = 0;
    virtual void ClearEntries() = 0;
    virtual void FoundEntry(const RealmcIface::EntryInfo *info) = 0;
    virtual void FindEntriesDone(RealmcIface::CardStatus status) = 0;
    virtual void Retry(RealmcIface::CardStatus status) = 0;
    virtual void Failed(RealmcIface::TaskResult result, RealmcIface::CardStatus status) = 0;
    virtual void CardChanged(RealmcIface::TaskResult result, RealmcIface::CardStatus status) = 0;
    virtual void CardChecked(const RealmcIface::CardInfo *info) = 0;
    virtual void CardRemoved() = 0;
    virtual void SetAutosaveDone(RealmcIface::TaskResult res, RealmcIface::CardStatus status, RealmcIface::AutosaveState flag) = 0;
    virtual void SetMonitorDone(RealmcIface::CardStatus status, RealmcIface::MonitorState state) = 0;
    virtual RealmcIface::TaskStatus LoadReady(const char *entryName, unsigned int headerSize, unsigned int bodySize, char *&headerData,
                                              char *&bodyData) = 0;
};

// File: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp
// Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:22
typedef enum {
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
    MJ_LoadReady = 17
} MemoryCardJoyLoggableEvents;

// total size: 0x1
// Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:47
class IJoyHelper {
  public:
    void JLog(const char *msg) {} // Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:64

    void JLog(bool &value) {} // Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:86

    void JLog(MemoryCardJoyLoggableEvents op) { // Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:108
        if (Joylog::IsCapturing())
            Joylog::AddData(static_cast<int>(op), 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(void *data, int data_size_bytes) {} // Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:139

    static void EmulateMemoryCardLibrary(int aJoyOp); // Decl: speed/indep/src/frontend/memorycard/MemoryCardHelper.hpp:169

    void JLog(const wchar_t *msg) {}

    void JLog(unsigned int &value) {}

    void JLog(RealmcIface::CardId &id) {}

    void JLog(RealmcIface::CardStatus &status) {
        status = static_cast<RealmcIface::CardStatus>(Joylog::AddOrGetData(static_cast<unsigned int>(status), 0x10, JOYLOG_CHANNEL_MEMORY_CARD));
    }

    void JLog(RealmcIface::MonitorState &state) {}

    void JLog(const RealmcIface::CardInfo *pInfo) {}

    void JLog(RealmcIface::TaskResult &res) {
        res = static_cast<RealmcIface::TaskResult>(Joylog::AddOrGetData(static_cast<unsigned int>(res), 8, JOYLOG_CHANNEL_MEMORY_CARD));
    }

    void JLog(const RealmcIface::EntryInfo *info) {}
};

#endif
