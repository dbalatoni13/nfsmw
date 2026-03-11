#ifndef FRONTEND_MEMORYCARD_MEMORYCARDHELPER_H
#define FRONTEND_MEMORYCARD_MEMORYCARDHELPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "RealmcIface.hpp"

struct IAllocator;

struct IThread {
    virtual ~IThread() {}
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual IThread* CreateInstance() = 0;
    virtual void SetStackSize(unsigned int stacksize) = 0;
    virtual void Begin(int (*func)(void*)) = 0;
    virtual void WaitForEnd(int) = 0;
    virtual void Sleep(int ticks) = 0;
    virtual int (*GetEntryFunc())(void*) = 0;
    virtual bool IsActive() = 0;
};

struct IMutex {
    virtual ~IMutex() {}
    virtual int AddRef() = 0;
    virtual int Release() = 0;
    virtual IMutex* CreateInstance() = 0;
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

struct THREAD {
    int reserved[198]; // offset 0x0, size 0x318
};

struct MyThread : public IThread {
    int mRefcount;                   // offset 0x4, size 0x4
    int (*mEntryFunc)(void*);        // offset 0x8, size 0x4
    unsigned int mStackSize;         // offset 0xC, size 0x4
    void* mStackBuffer;              // offset 0x10, size 0x4
    THREAD mThreadData;              // offset 0x14, size 0x318
    int mPriority;                   // offset 0x32C, size 0x4
    bool mActive;                    // offset 0x330, size 0x1

    int AddRef() override;
    int Release() override;
    IThread* CreateInstance() override;
    void SetStackSize(unsigned int stacksize) override { mStackSize = stacksize; }
    void Begin(int (*func)(void*)) override;
    void WaitForEnd(int) override;
    void Sleep(int ticks) override;
    int (*GetEntryFunc())(void*) override { return mEntryFunc; }
    bool IsActive() override { return mActive; }
};

struct MyMutex : public IMutex {
    MUTEX mMutex;  // offset 0x4, size 0x1C
    int mRefcount; // offset 0x20, size 0x4

    int AddRef() override;
    int Release() override;
    IMutex* CreateInstance() override;
    void Lock() override;
    void Unlock() override;
};

namespace Realmc {

// total size: 0x10
struct SystemInterface {
    IAllocator *mAllocator;              // offset 0x0, size 0x4
    IThread *mThread;                    // offset 0x4, size 0x4
    IMutex *mMutex;                      // offset 0x8, size 0x4
    const char *(*mGetStrCallback)(int); // offset 0xC, size 0x4

    void Clear() {
        mAllocator = nullptr;
        mThread = nullptr;
        mMutex = nullptr;
        mGetStrCallback = nullptr;
    }
};

} // namespace Realmc

struct MemoryCard;
struct UIMemcardBase;

struct IGameInterface {
    virtual void ShowMessage(const wchar_t *msg, unsigned int nOptions,
                             const wchar_t **options) = 0;
    virtual void ClearMessage() = 0;
    virtual void BootupCheckDone(RealmcIface::CardStatus status,
                                 RealmcIface::BootupCheckResults res) = 0;
    virtual void SaveCheckDone(RealmcIface::TaskResult result,
                               RealmcIface::CardStatus status) = 0;
    virtual void SaveDone(const char *filename) = 0;
    virtual RealmcIface::DataStatus CheckLoadedData(const char *data) = 0;
    virtual void LoadDone(const char *filename) = 0;
    virtual void DeleteDone(const char *filename) = 0;
    virtual void ClearEntries() = 0;
    virtual void FoundEntry(const RealmcIface::EntryInfo *info) = 0;
    virtual void FindEntriesDone(RealmcIface::CardStatus status) = 0;
    virtual void Retry(RealmcIface::CardStatus status) = 0;
    virtual void Failed(RealmcIface::TaskResult result,
                        RealmcIface::CardStatus status) = 0;
    virtual void CardChanged(RealmcIface::TaskResult result,
                             RealmcIface::CardStatus status) = 0;
    virtual void CardChecked(const RealmcIface::CardInfo *info) = 0;
    virtual void CardRemoved() = 0;
    virtual void SetAutosaveDone(RealmcIface::TaskResult res,
                                 RealmcIface::CardStatus status,
                                 RealmcIface::AutosaveState flag) = 0;
    virtual void SetMonitorDone(RealmcIface::CardStatus status,
                                RealmcIface::MonitorState state) = 0;
    virtual RealmcIface::TaskStatus LoadReady(const char *entryName,
                                              unsigned int headerSize,
                                              unsigned int bodySize,
                                              char *&headerData,
                                              char *&bodyData) = 0;
};

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
    MJ_SetMonitorDone = 18,
};

struct IJoyHelper {
    static void EmulateMemoryCardLibrary(int aJoyOp);

    inline void JLog(MemoryCardJoyLoggableEvents op) {
        if (Joylog::IsCapturing())
            Joylog::AddData(static_cast< int >(op), 8, JOYLOG_CHANNEL_MEMORY_CARD);
    }
};

struct MemcardCallbacks : public IGameInterface, public IJoyHelper {
    MemoryCard *GetMemcard();
    UIMemcardBase *GetScreen();

    void ShowMessage(const wchar_t *msg, unsigned int nOptions,
                     const wchar_t **options) override;
    void ClearMessage() override;
    void BootupCheckDone(RealmcIface::CardStatus status,
                         RealmcIface::BootupCheckResults res) override;
    void SaveCheckDone(RealmcIface::TaskResult result,
                       RealmcIface::CardStatus status) override;
    void SaveDone(const char *filename) override;
    RealmcIface::DataStatus CheckLoadedData(const char *data) override;
    void LoadDone(const char *filename) override;
    void DeleteDone(const char *filename) override;
    void ClearEntries() override;
    void FoundEntry(const RealmcIface::EntryInfo *info) override;
    void FindEntriesDone(RealmcIface::CardStatus status) override;
    void Retry(RealmcIface::CardStatus status) override;
    void Failed(RealmcIface::TaskResult result,
                RealmcIface::CardStatus status) override;
    void CardChanged(RealmcIface::TaskResult result,
                     RealmcIface::CardStatus status) override;
    void CardChecked(const RealmcIface::CardInfo *info) override;
    void CardRemoved() override;
    void SetAutosaveDone(RealmcIface::TaskResult res,
                         RealmcIface::CardStatus status,
                         RealmcIface::AutosaveState flag) override;
    void SetMonitorDone(RealmcIface::CardStatus status,
                        RealmcIface::MonitorState state) override;
    RealmcIface::TaskStatus LoadReady(const char *entryName,
                                      unsigned int headerSize,
                                      unsigned int bodySize, char *&headerData,
                                      char *&bodyData) override;

    inline MemcardCallbacks() {}
};

#endif
