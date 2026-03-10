#ifndef FRONTEND_MEMORYCARD_MEMORYCARDHELPER_H
#define FRONTEND_MEMORYCARD_MEMORYCARDHELPER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "RealmcIface.hpp"

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
