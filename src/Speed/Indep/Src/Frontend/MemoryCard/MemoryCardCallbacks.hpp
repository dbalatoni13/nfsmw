#ifndef MEMORYCARDCALLBACKS_H
#define MEMORYCARDCALLBACKS_H

#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

class MemcardCallbacks : public IGameInterface, public IJoyHelper {
  public:
    MemoryCard *GetMemcard();
    UIMemcardBase *GetScreen();

    void ShowMessage(const wchar_t *msg, uint32_t nOptions, const wchar_t **options) override;
    void ClearMessage() override;
    void BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults res) override;
    void SaveCheckDone(RealmcIface::TaskResult result, RealmcIface::CardStatus status) override;
    void SaveDone(const char *filename) override;
    RealmcIface::DataStatus CheckLoadedData(const char *data) override;
    void LoadDone(const char *filename) override;
    void DeleteDone(const char *filename) override;
    void ClearEntries() override;
    void FoundEntry(const RealmcIface::EntryInfo *info) override;
    void FindEntriesDone(RealmcIface::CardStatus status) override;
    void Retry(RealmcIface::CardStatus status) override;
    void Failed(RealmcIface::TaskResult result, RealmcIface::CardStatus status) override;
    void CardChanged(RealmcIface::TaskResult result, RealmcIface::CardStatus status) override;
    void CardChecked(const RealmcIface::CardInfo *info) override;
    void CardRemoved() override;
    void SetAutosaveDone(RealmcIface::TaskResult res, RealmcIface::CardStatus status, RealmcIface::AutosaveState flag) override;
    void SetMonitorDone(RealmcIface::CardStatus status, RealmcIface::MonitorState state) override;
    RealmcIface::TaskStatus LoadReady(const char *entryName, uint32_t headerSize, uint32_t bodySize, char *&headerData, char *&bodyData) override;
};

#endif
