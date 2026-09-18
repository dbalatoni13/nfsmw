#ifndef FRONTEND_MEMORYCARD_MEMORYCARDHELPER_H
#define FRONTEND_MEMORYCARD_MEMORYCARDHELPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include <string.h>
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardImp.hpp"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

// realcore: la parte de la API de hilos que MyThread envuelve y que
// system/threads.h no declara.
struct THREAD {
    int reserved[198]; // total size: 0x318
};

bool THREAD_create(THREAD *t, int (*func)(void *), void *param, void *stack, int stacksize, int priority);
void THREAD_destroy(THREAD *t);
bool THREAD_waitexit(THREAD *t, int timeout);
void THREAD_yield(int dur);
bool THREAD_setpriority(THREAD *t, int priority);

extern int IsMemcardEnabled;
extern int IsAutoSaveEnabled;

// Sobrecarga ancha de bWare que Strings.hpp no declara.
unsigned short *bStrNCpy(unsigned short *to, const char *from, int m);

enum LOCALESTATE {
    LOCALE_LANGUAGEID = 0,
    LOCALE_LANGUAGECOUNT = 1,
    LOCALE_STRINGCOUNT = 2,
    LOCALE_HASINDEX = 3,
};

void *LOCALE_create(void *buffer, int flag);
void LOCALE_setstate(void *locale, LOCALESTATE state, int flag);
const char *LOCALE_getstrA(const void *locale, int strID);

// total size: 0x1804
struct MemoryCardMessage {
    MemoryCardMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t **options);

    unsigned short mMsg[2048];        // offset 0x0
    unsigned int mnOptions;           // offset 0x1000
    unsigned short mOptions[4][256];  // offset 0x1004
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

// total size: 0x24
class MyMutex : public Realmc::IMutex {
  public:
    MyMutex() {
        memset(&mMutex, 0, sizeof(mMutex));
        mRefcount = 1;
        bMemSet(&mMutex, sizeof(mMutex), 0);
        MUTEX_create(&mMutex);
    }

    virtual ~MyMutex() {
        MUTEX_destroy(&mMutex);
    }

    Realmc::IMutex *CreateInstance() override { return BNEW MyMutex; }
    int AddRef() override { return ++mRefcount; }

    int Release() override {
        if (--mRefcount <= 0) {
            delete this;
            return 0;
        }
        return mRefcount;
    }

    void Lock() override { MUTEX_lock(&mMutex); }
    void Unlock() override { MUTEX_unlock(&mMutex); }

    MUTEX mMutex;  // offset 0x4
    int mRefcount; // offset 0x20
};

// total size: 0x334
class MyThread : public Realmc::IThread {
  public:
    MyThread() {
        mRefcount = 1;
        mStackSize = 4096;
        mStackBuffer = 0;
        memset(&mThreadData, 0, sizeof(mThreadData));
        mPriority = 0;
        mActive = false;
    }

    virtual ~MyThread() {
        if (mActive) {
            WaitForEnd(0);
            THREAD_destroy(&mThreadData);
        }
    }

    Realmc::IThread *CreateInstance() override {
        return BNEW MyThread;
    }

    int AddRef() override { return ++mRefcount; }

    int Release() override {
        if (--mRefcount <= 0) {
            delete this;
            return 0;
        }
        return mRefcount;
    }

    void SetStackSize(unsigned int stacksize) override { mStackSize = stacksize; }

    static int EntryProc(void *pContext) {
        MyThread *pThread = static_cast<MyThread *>(pContext);

        while (!pThread->IsActive()) {
            THREAD_yield(1);
        }
        pThread->GetEntryFunc()(pThread);
        return 0;
    }

    void Begin(int (*func)(void *)) override {
        mEntryFunc = func;
        mStackBuffer = BNEW char[mStackSize];
        THREAD_create(&mThreadData, EntryProc, this, mStackBuffer, mStackSize, mPriority);
        mActive = true;
    }

    void WaitForEnd(int) override {
        THREAD_waitexit(&mThreadData, 0);
        delete[] static_cast<char *>(mStackBuffer);
        mActive = false;
    }

    void Sleep(int ticks) override { THREAD_yield(ticks); }

    void SetPriority(int priority) override {
        mPriority = 0;
        THREAD_setpriority(&mThreadData, 0);
    }

    int (*GetEntryFunc())(void *) { return mEntryFunc; }
    bool IsActive() { return mActive; }

    int mRefcount;             // offset 0x4
    int (*mEntryFunc)(void *); // offset 0x8
    unsigned int mStackSize;   // offset 0xC
    void *mStackBuffer;        // offset 0x10
    THREAD mThreadData;        // offset 0x14
    int mPriority;             // offset 0x32C
    bool mActive;              // offset 0x330
};

// total size: 0x1
class IJoyHelper {
  public:
    void JLog(const wchar_t *msg) {
        Joylog::AddOrGetData(reinterpret_cast<unsigned short *>(const_cast<wchar_t *>(msg)), JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(unsigned int &value) {
        value = Joylog::AddOrGetData(value, 32, JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(const char *msg) {
        Joylog::AddOrGetData(const_cast<char *>(msg), JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(RealmcIface::CardId &id) {
        id = static_cast<RealmcIface::CardId>(Joylog::AddOrGetData(id, 32, JOYLOG_CHANNEL_MEMORY_CARD));
    }

    void JLog(RealmcIface::CardStatus &status) {
        unsigned int value = status;
        value = Joylog::AddOrGetData(value, 16, JOYLOG_CHANNEL_MEMORY_CARD);
        status = static_cast<RealmcIface::CardStatus>(value);
    }

    void JLog(RealmcIface::MonitorState &state) {
        unsigned int value = state;
        value = Joylog::AddOrGetData(value, 16, JOYLOG_CHANNEL_MEMORY_CARD);
        state = static_cast<RealmcIface::MonitorState>(value);
    }

    void JLog(bool &value) {
        value = Joylog::AddOrGetData(value, 1, JOYLOG_CHANNEL_MEMORY_CARD);
    }

    void JLog(const RealmcIface::CardInfo *pInfo) {
        RealmcIface::CardInfo *pVal = const_cast<RealmcIface::CardInfo *>(pInfo);

        JLog(pVal->mCardId);
        JLog(pVal->mStatus);
        JLog(pVal->mFreeSpace);
        JLog(pVal->mFreeFiles);
        JLog(pVal->mTotalSpace);
        JLog(pVal->mFreeSpaceOverLimit);
        JLog(pVal->mTotalSpaceOverLimit);
    }

    void JLog(RealmcIface::TaskResult &res) {
        res = static_cast<RealmcIface::TaskResult>(Joylog::AddOrGetData(res, 8, JOYLOG_CHANNEL_MEMORY_CARD));
    }

    void JLog(MemoryCardJoyLoggableEvents op) {
        if (Joylog::IsCapturing()) {
            Joylog::AddData(op, 8, JOYLOG_CHANNEL_MEMORY_CARD);
        }
    }

    void JLog(const RealmcIface::EntryInfo *info) {
        RealmcIface::EntryInfo *e = const_cast<RealmcIface::EntryInfo *>(info);

        JLog(e->mName);
        JLog(e->mStatus);
        JLog(e->mEntryBlocks);
        JLog(e->mUserDataSize);
        JLog(e->mTime.mCreated);
        JLog(e->mTime.mLastAccessed);
        JLog(e->mTime.mLastModified);
        JLog(e->mCompanyCode);
        JLog(e->mGameCode);
    }

    void JLog(void *data, int data_size_bytes) {
        if (Joylog::IsReplaying()) {
            Joylog::GetData(data, data_size_bytes, JOYLOG_CHANNEL_MEMORY_CARD);
        }
        if (Joylog::IsCapturing()) {
            Joylog::AddData(data, data_size_bytes, JOYLOG_CHANNEL_MEMORY_CARD);
        }
    }

    static void EmulateMemoryCardLibrary(int aJoyOp);
};

void CaptureJoyOp(MemoryCardJoyLoggableEvents aJoyOp) {
    Joylog::AddData(aJoyOp, 8, JOYLOG_CHANNEL_MEMORY_CARD);
}

int ReplayJoyOp() {
    int aJoyOp;

    aJoyOp = Joylog::GetData(8, JOYLOG_CHANNEL_MEMORY_CARD);
    IJoyHelper::EmulateMemoryCardLibrary(aJoyOp);
    return aJoyOp;
}

#endif
