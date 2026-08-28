#include <string.h>

#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"

namespace RealmcIface {

TaskManager::TaskManager(MemcardInterfaceImpl *impl, IGameInterface *iGame)
    : mMemcardImpl(impl)
    , mIGame(iGame)
    , mCurTask(0)
    , mMainTask(TASK_NONE)
    , mMonitorState(MONITOR_OFF)
    , mCheckingMsgShown(false)
    , mWarningMsgShown(false)
    , mLastMessageId(0)
    , mSetMonitorDoneCalled(false)
    , mCancelledCardChangedCalled(false) {
    this->mRecentCardInfo.Clear();
    this->_InitTaskList();
}

void TaskManager::BootupCheck(const BootupCheckParams *params, unsigned int nEntries,
                              const char **entryNames, wchar_t *content) {
    this->mMainTask = TASK_BOOTUPCHECK;
    this->_ClearTaskList();
    this->mTaskList[0].mTask = TASK_BOOTUPCHECK;
    this->mTaskList[0].InitBootupCheck(params);

    if (nEntries > 0) {
        const char *allEntries = ALL_ENTRIES;

        this->mTaskList[1].mTask = TASK_FINDENTRIES;
        this->mTaskList[1].InitFindEntries(allEntries, nullptr);
        this->mTaskList[2].mTask = TASK_LOAD;
        this->mTaskList[2].InitLoad(nEntries, entryNames, content, nullptr, nullptr);
    }

    this->_StartTask();
}

void TaskManager::Load(const char *entryName, char *header, char *body,
                       const wchar_t *contentName, const wchar_t *typeName,
                       const TitleInfo *titleInfo) {
    if (this->mMainTask == TASK_NONE) {
        const char **names = &entryName;

        this->_ClearTaskList();
        this->mMainTask = TASK_LOAD;
        this->mTaskList[0].mTask = TASK_LOAD;
        this->mTaskList[0].InitLoad(1, names, contentName, typeName, titleInfo);
        this->mTaskList[0].mDetails.mLoad.mLoadInfos[0].mHeader = header;
        this->mTaskList[0].mDetails.mLoad.mLoadInfos[0].mBody = body;
        this->mTaskList[0].mDetails.mLoad.mLoadInfos[0].mTryLoad = true;
        this->_StartTask();
    } else {
        const char **names = &entryName;

        this->mTaskList[1].mTask = TASK_LOAD;
        this->mTaskList[1].InitLoad(1, names, contentName, typeName, titleInfo);
        this->mTaskList[1].mDetails.mLoad.mLoadInfos[0].mHeader = header;
        this->mTaskList[1].mDetails.mLoad.mLoadInfos[0].mBody = body;
        this->mTaskList[1].mDetails.mLoad.mLoadInfos[0].mTryLoad = true;
        if (this->mMemcardImpl->IsActiveTaskNone()) {
            this->mCurTask--;
        }
    }
}

void TaskManager::FindEntries(const char *entryNamePattern, const TitleInfo *titleInfo) {
    if (this->mMainTask == TASK_NONE) {
        this->_ClearTaskList();
        this->mMainTask = TASK_FINDENTRIES;
        this->mTaskList[0].mTask = TASK_FINDENTRIES;
        this->mTaskList[0].InitFindEntries(entryNamePattern, titleInfo);
        this->_StartTask();
    } else {
        this->mTaskList[1].mTask = TASK_FINDENTRIES;
        this->mTaskList[1].InitFindEntries(entryNamePattern, titleInfo);
        if (this->mMemcardImpl->IsActiveTaskNone()) {
            this->mCurTask--;
        }
    }
}

void TaskManager::Save(const char *entryName, const char *header, const char *body,
                       const SaveInfo *saveInfo, const TitleInfo *titleInfo) {
    if (this->mMainTask == TASK_NONE) {
        this->_ClearTaskList();
        this->mMainTask = TASK_SAVE;
        this->mTaskList[0].mTask = TASK_SAVE;
        this->mTaskList[0].InitSave(entryName, header, body, saveInfo, titleInfo);
        this->_StartTask();
    } else {
        this->mTaskList[1].mTask = TASK_SAVE;
        this->mTaskList[1].InitSave(entryName, header, body, saveInfo, titleInfo);
        if (this->mMemcardImpl->IsActiveTaskNone()) {
            this->mCurTask--;
        }
    }
}

void TaskManager::Delete(unsigned int nEntries, const char **entryNames, const wchar_t *contentName) {
    if (this->mMainTask == TASK_NONE) {
        this->_ClearTaskList();
        this->mMainTask = TASK_DELETE;
        this->mTaskList[0].mTask = TASK_DELETE;
        this->mTaskList[0].InitDelete(nEntries, entryNames, contentName);
        this->_StartTask();
    } else {
        this->mTaskList[1].mTask = TASK_DELETE;
        this->mTaskList[1].InitDelete(nEntries, entryNames, contentName);
        if (this->mMemcardImpl->IsActiveTaskNone()) {
            this->mCurTask--;
        }
    }
}

void TaskManager::SetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs,
                              const char *entryName, CardId cardId) {
    if (this->mMainTask == TASK_NONE) {
        this->_ClearTaskList();
        this->mMainTask = TASK_SETAUTOSAVE;
        this->mTaskList[0].mTask = TASK_SETAUTOSAVE;
        this->mTaskList[0].InitSetAutosave(state, nSaveReqs, saveReqs, entryName, cardId);
        this->_StartTask();
    } else {
        this->mTaskList[1].mTask = TASK_SETAUTOSAVE;
        this->mTaskList[1].InitSetAutosave(state, nSaveReqs, saveReqs, entryName, cardId);
        if (this->mMemcardImpl->IsActiveTaskNone()) {
            this->mCurTask--;
        }
    }
}

void TaskManager::CheckCard(CardId cardId) {
    if (this->mMainTask == TASK_NONE) {
        this->_ClearTaskList();
        this->mMainTask = TASK_CHECKCARD;
        this->mTaskList[0].InitCheckCard(cardId);
        this->mTaskList[0].mTask = TASK_CHECKCARD;
        this->_StartTask();
    } else {
        this->mTaskList[1].mTask = TASK_NONE;
        this->mIGame->CardChecked(&this->mRecentCardInfo);
    }
}

void TaskManager::SetMonitor(MonitorState state) {
    this->mMonitorState = state;
    if (this->mMainTask == TASK_NONE) {
        if (state == MONITOR_OFF) {
            this->mIGame->SetMonitorDone(this->mRecentCardInfo.mStatus, state);
        } else {
            this->_ClearOldMsgs();
            this->_ClearTaskList();
            this->mSetMonitorDoneCalled = false;
            this->mMainTask = TASK_MONITOR;
            this->mTaskList[0].mTask = TASK_CHECKCARD;
            this->mTaskList[0].InitMonitor();
            this->_StartTask();
        }
    } else if (this->mMainTask == TASK_MONITOR) {
        if (state == MONITOR_OFF) {
            this->mMemcardImpl->ClearTask();
            this->mMainTask = TASK_NONE;
            this->_ClearOldMsgs();
        }
        this->mIGame->SetMonitorDone(this->mRecentCardInfo.mStatus, this->mMonitorState);
    }
}

void TaskManager::FoundEntry(EntryInfo *entryInfo) {
    if (this->mMainTask == TASK_BOOTUPCHECK) {
        unsigned int iEntry;

        for (iEntry = 0; iEntry < this->mTaskList[2].mDetails.mLoad.mNumEntries; iEntry++) {
            LoadInfo *loadInfo = &this->mTaskList[2].mDetails.mLoad.mLoadInfos[iEntry];
            if (strncasecmp(entryInfo->mName, loadInfo->mEntryName, 31) == 0) {
                loadInfo->mTryLoad = true;
            }
        }
    } else {
        this->mIGame->FoundEntry(entryInfo);
    }
}

void TaskManager::ClearEntries() {
    if (this->mMainTask == TASK_FINDENTRIES) {
        this->mIGame->ClearEntries();
    }
}

void TaskManager::_StartTask() {
    register McTask *curTask;
    curTask = &this->mTaskList[this->mCurTask];

    switch (curTask->mTask) {
    case TASK_NONE:
        break;
    case TASK_BOOTUPCHECK:
        this->_ClearOldMsgs();
        this->mMemcardImpl->BootupCheck(curTask->mDetails.mBootupCheck.mBootupParams);
        break;
    case TASK_LOAD: {
        LoadInfo *info;

        this->_ClearOldMsgs();
        info = &curTask->mDetails.mLoad.mLoadInfos[curTask->mDetails.mLoad.mCurEntry];
        curTask->mDetails.mLoad.mCurEntry++;
        if (info->mTryLoad) {
            if (info->mTitleInfo.mDataFormat == FORMAT_RAW) {
                this->mMemcardImpl->LoadAlternate(info->mEntryName, info->mHeader, info->mBody,
                                                  info->mContentName, info->mTypeName, &info->mTitleInfo);
            } else if (info->mTitleInfo.mTitleType == TITLE_DEFAULT) {
                this->mMemcardImpl->Load(info->mEntryName, info->mHeader, info->mBody,
                                         info->mContentName, info->mTypeName);
            } else {
                this->mMemcardImpl->LoadAlternate(info->mEntryName, info->mHeader, info->mBody,
                                                  info->mContentName, info->mTypeName, &info->mTitleInfo);
            }
        } else {
            this->CompleteTask(RESULT_FAILED, STATUS_ENTRY_NOT_FOUND, nullptr);
        }
        break;
    }
    case TASK_SAVECHECK: {
        this->_ClearOldMsgs();

        SaveReq reqs;
        SaveReq *saveReqs;

        reqs.mNumSaves = 1;
        reqs.mSaveInfo = const_cast<SaveInfo *>(curTask->mDetails.mSaveCheck.mSaveInfo);
        saveReqs = &reqs;
        if (curTask->mDetails.mSaveCheck.mTitleInfo == nullptr ||
            curTask->mDetails.mSaveCheck.mTitleInfo->mDataFormat == FORMAT_LAYER2) {
            this->mMemcardImpl->SaveCheck(curTask->mDetails.mSaveCheck.mEntryName, 1, &saveReqs);
        }
        break;
    }
    case TASK_SAVE:
        this->_ClearOldMsgs();
        if (curTask->mDetails.mSave.mTitleInfo == nullptr ||
            curTask->mDetails.mSave.mTitleInfo->mDataFormat == FORMAT_LAYER2) {
            this->mMemcardImpl->Save(curTask->mDetails.mSave.mEntryName,
                                     curTask->mDetails.mSave.mHeader,
                                     curTask->mDetails.mSave.mBody,
                                     curTask->mDetails.mSave.mSaveInfo);
        }
        break;
    case TASK_DELETE:
        this->_ClearOldMsgs();
        if (curTask->mDetails.mDelete.mNumEntries == 1) {
            this->mMemcardImpl->Delete(curTask->mDetails.mDelete.mEntryName,
                                       curTask->mDetails.mDelete.mContentName);
        } else {
            this->mMemcardImpl->DeleteMultiple(curTask->mDetails.mDelete.mNumEntries,
                                                curTask->mDetails.mDelete.mEntryNames,
                                                curTask->mDetails.mDelete.mContentName);
        }
        break;
    case TASK_FINDENTRIES:
        if (this->mMainTask != TASK_BOOTUPCHECK) {
            this->_ClearOldMsgs();
        }
        {
            FindEntriesInfo *info = &curTask->mDetails.mFindEntries;

            if (info->mTitleInfo.mDataFormat == FORMAT_RAW) {
                this->mMemcardImpl->FindEntriesAlternate(info->mEntryNamePattern, &info->mTitleInfo);
            } else if (info->mTitleInfo.mTitleType == TITLE_DEFAULT) {
                this->mMemcardImpl->FindEntries(curTask->mDetails.mFindEntries.mEntryNamePattern);
            } else {
                this->mMemcardImpl->FindEntriesAlternate(info->mEntryNamePattern, &info->mTitleInfo);
            }
        }
        break;
    case TASK_CHECKCARD:
        if (this->mMainTask == TASK_CHECKCARD) {
            this->_ClearOldMsgs();
            this->mMemcardImpl->CheckCard(curTask->mDetails.mCheckCardId);
        } else {
            this->mMemcardImpl->CheckCard(CARD_UNKNOWN);
        }
        break;
    case TASK_SETAUTOSAVE:
        this->_ClearOldMsgs();
        this->mMemcardImpl->SetAutosave(curTask->mDetails.mSetAutosave.mState,
                                         curTask->mDetails.mSetAutosave.mNumSaveReqs,
                                         curTask->mDetails.mSetAutosave.mSaveReqs,
                                         curTask->mDetails.mSetAutosave.mEntryName,
                                         curTask->mDetails.mSetAutosave.mCardId);
        break;
    }
}

void TaskManager::CompleteTask(TaskResult result, CardStatus status, void *other) {
    McTask *curTask = &this->mTaskList[this->mCurTask];
    asm volatile("" : : : "r28");

    switch (curTask->mTask) {
    case TASK_NONE:
        break;
    case TASK_BOOTUPCHECK:
        curTask->mDetails.mBootupCheck.mCardStatus = status;
        curTask->mDetails.mBootupCheck.mBootupResults = *static_cast<BootupCheckResults *>(other);
        this->mMemcardImpl->SetActiveCard(curTask->mDetails.mBootupCheck.mBootupResults.mFirstGoodCard);
        if (status != STATUS_OK && status != STATUS_INSUFFICIENT_SPACE &&
            this->mTaskList[1].mTask != TASK_NONE &&
            this->mTaskList[2].mDetails.mLoad.mNumEntries != 0) {
            if (this->mTaskList[2].mDetails.mLoad.mLoadInfos != nullptr) {
                delete[] this->mTaskList[2].mDetails.mLoad.mLoadInfos;
                this->mTaskList[2].mDetails.mLoad.mLoadInfos = nullptr;
            }
            this->_ClearTaskList();
        }
        this->mCurTask++;
        break;
    case TASK_LOAD:
        if (result == RESULT_SUCCESS) {
            if (!((curTask->mDetails.mLoad.mLastEntryFound > curTask->mDetails.mLoad.mCurEntry ||
                   curTask->mDetails.mLoad.mNumEntries > curTask->mDetails.mLoad.mLastEntryFound) &&
                  curTask->mDetails.mLoad.mNumEntries > curTask->mDetails.mLoad.mCurEntry)) {
                if (curTask->mDetails.mLoad.mLoadInfos != nullptr) {
                    delete[] curTask->mDetails.mLoad.mLoadInfos;
                }
                curTask->mDetails.mLoad.mLoadInfos = nullptr;
                this->mCurTask++;
            }
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->LoadDone(reinterpret_cast<const char *>(other));
        } else if (result == RESULT_RETRY) {
            if (this->mMainTask == TASK_BOOTUPCHECK) {
                unsigned int iEntry;

                curTask->mDetails.mLoad.mLastEntryFound = 0;
                this->mCurTask = 0;
                for (iEntry = 0; iEntry < curTask->mDetails.mLoad.mNumEntries; iEntry++) {
                    curTask->mDetails.mLoad.mLoadInfos[iEntry].mTryLoad = false;
                }
            }
            curTask->mDetails.mLoad.mCurEntry = 0;
            this->mIGame->Retry(status);
            this->_ClearOldMsgs();
        } else {
            if (this->mMainTask == TASK_BOOTUPCHECK) {
                if (status != STATUS_NO_CARD && status != STATUS_CARD_REMOVED &&
                    status != STATUS_CARD_UNFORMATTED && status != STATUS_WRONG_DEVICE) {
                    if (result == RESULT_CANCELLED && status == STATUS_CARD_CHANGED) {
                        if (curTask->mDetails.mLoad.mLoadInfos != nullptr) {
                            delete[] curTask->mDetails.mLoad.mLoadInfos;
                        }
                        curTask->mDetails.mLoad.mLoadInfos = nullptr;
                        this->mCurTask++;
                    } else if (status == STATUS_CARD_CHANGED) {
                        this->_ClearOldMsgs();
                        this->mCurTask--;
                        curTask->mDetails.mLoad.mCurEntry = 0;
                        {
                            unsigned int iEntry;

                            for (iEntry = 0; iEntry < curTask->mDetails.mLoad.mNumEntries; iEntry++) {
                                curTask->mDetails.mLoad.mLoadInfos[iEntry].mTryLoad = false;
                            }
                        }
                    } else {
                        this->_ClearOldMsgs();
                        if (!((curTask->mDetails.mLoad.mLastEntryFound > curTask->mDetails.mLoad.mCurEntry ||
                               curTask->mDetails.mLoad.mNumEntries > curTask->mDetails.mLoad.mLastEntryFound) &&
                              curTask->mDetails.mLoad.mNumEntries > curTask->mDetails.mLoad.mCurEntry)) {
                            if (curTask->mDetails.mLoad.mLoadInfos != nullptr) {
                                delete[] curTask->mDetails.mLoad.mLoadInfos;
                            }
                            curTask->mDetails.mLoad.mLoadInfos = nullptr;
                            this->mCurTask++;
                        }
                    }
                } else {
                    if (curTask->mDetails.mLoad.mLoadInfos != nullptr) {
                        delete[] curTask->mDetails.mLoad.mLoadInfos;
                    }
                    curTask->mDetails.mLoad.mLoadInfos = nullptr;
                    this->mCurTask++;
                }
                this->mTaskList[0].mDetails.mBootupCheck.mCardStatus = status;
            } else {
                if (curTask->mDetails.mLoad.mLoadInfos != nullptr) {
                    delete[] curTask->mDetails.mLoad.mLoadInfos;
                }
                curTask->mDetails.mLoad.mLoadInfos = nullptr;
                this->mCurTask++;
            }
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->Failed(result, status);
        }
        if (status >= STATUS_ENTRY_CORRUPTED && status <= STATUS_ENTRY_DELETED) {
            status = STATUS_OK;
        }
        if (this->mMainTask == TASK_MONITOR) {
            if (this->_HasStatusChanged(status)) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
                this->mIGame->CardChanged(result, status);
            }
        }
        break;
    case TASK_SAVECHECK:
        if (result == RESULT_RETRY) {
            this->mIGame->Retry(status);
            this->_ClearOldMsgs();
        } else {
            this->mCurTask++;
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->SaveCheckDone(result, status);
        }
        if (this->mMainTask == TASK_MONITOR) {
            if (this->_HasStatusChanged(status)) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
                this->mIGame->CardChanged(result, status);
            }
        }
        break;
    case TASK_SAVE:
        if (result == RESULT_SUCCESS) {
            this->mCurTask++;
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->SaveDone(reinterpret_cast<const char *>(other));
        } else if (result == RESULT_RETRY) {
            this->mIGame->Retry(status);
            this->_ClearOldMsgs();
        } else {
            this->mCurTask++;
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->Failed(result, status);
        }
        if (this->mMainTask == TASK_MONITOR) {
            if (this->_HasStatusChanged(status)) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
                this->mIGame->CardChanged(result, status);
            }
        }
        break;
    case TASK_DELETE:
        if (result == RESULT_SUCCESS) {
            this->mCurTask++;
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->DeleteDone(reinterpret_cast<const char *>(other));
        } else if (result == RESULT_RETRY) {
            this->mIGame->Retry(status);
            this->_ClearOldMsgs();
        } else {
            if (result == RESULT_CANCELLED) {
                this->mWarningMsgShown = true;
            }
            this->mCurTask++;
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->Failed(result, status);
        }
        if (status == STATUS_ENTRY_NOT_FOUND) {
            status = STATUS_OK;
        }
        if (this->mMainTask == TASK_MONITOR) {
            if (this->_HasStatusChanged(status)) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
                this->mIGame->CardChanged(result, status);
            }
        }
        break;
    case TASK_FINDENTRIES:
        if (result == RESULT_SUCCESS) {
            this->mCurTask++;
            if (this->mMainTask == TASK_BOOTUPCHECK) {
                {
                    unsigned int iEntry;

                    this->mTaskList[2].mDetails.mLoad.mLastEntryFound = 0;
                    for (iEntry = 0; iEntry < this->mTaskList[2].mDetails.mLoad.mNumEntries; iEntry++) {
                        if (this->mTaskList[2].mDetails.mLoad.mLoadInfos[iEntry].mTryLoad) {
                            this->mTaskList[2].mDetails.mLoad.mLastEntryFound = iEntry + 1;
                        }
                    }
                }
            }
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->FindEntriesDone(status);
        } else if (result == RESULT_RETRY) {
            if (this->mCurTask != 0 && this->mMainTask == TASK_BOOTUPCHECK) {
                this->mCurTask = 0;
            }
            this->mIGame->Retry(status);
            this->_ClearOldMsgs();
        } else {
            if (this->mMainTask == TASK_FINDENTRIES ||
                (this->mMainTask == TASK_MONITOR &&
                 this->mTaskList[this->mCurTask].mTask == TASK_FINDENTRIES)) {
                if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                    this->_ClearMainTask();
                }
                this->mIGame->Failed(result, status);
            } else if (this->mMainTask == TASK_BOOTUPCHECK) {
                this->mTaskList[0].mDetails.mBootupCheck.mCardStatus = status;
            }
            this->mCurTask++;
        }
        if (status == STATUS_ENTRY_NOT_FOUND) {
            status = STATUS_OK;
        }
        if (this->mMainTask == TASK_MONITOR) {
            if (this->_HasStatusChanged(status)) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
                this->mIGame->CardChanged(result, status);
            }
        }
        break;
    case TASK_CHECKCARD:
        this->mRecentCardInfo = *static_cast<CardInfo *>(other);
        if (this->mMainTask == TASK_CHECKCARD) {
            if (result == RESULT_RETRY) {
                this->mIGame->Retry(status);
            } else {
                this->mCurTask++;
                this->_ClearMainTask();
                this->mIGame->CardChecked(&this->mRecentCardInfo);
            }
        } else {
            if (this->mSetMonitorDoneCalled) {
                if (this->_HasStatusChanged(status)) {
                    curTask->mDetails.mMonitorStatus = status;
                    this->mIGame->CardChanged(result, status);
                    this->mCancelledCardChangedCalled = false;
                } else if (result == RESULT_CANCELLED && !this->mCancelledCardChangedCalled) {
                    this->mIGame->CardChanged(RESULT_CANCELLED, status);
                    this->mCancelledCardChangedCalled = true;
                }
            } else {
                if (result != RESULT_RETRY) {
                    curTask->mDetails.mMonitorStatus = status == STATUS_CARD_CHANGED ? STATUS_OK : status;
                    if (result == RESULT_CANCELLED) {
                        this->mMonitorState = static_cast<MonitorState>(static_cast<int>(result));
                    }
                }
                if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                    this->_ClearMainTask();
                }
                this->mIGame->SetMonitorDone(curTask->mDetails.mMonitorStatus, this->mMonitorState);
                this->mSetMonitorDoneCalled = true;
            }

            if (result == RESULT_RETRY) {
                this->mCancelledCardChangedCalled = true;
                this->_ClearOldMsgs();
            } else {
                this->mWarningMsgShown = false;
                this->mLastMessageId = 0;
                if (this->mTaskList[this->mCurTask + 1].mTask != TASK_NONE) {
                    this->mCurTask++;
                }
            }
        }
        break;
    case TASK_SETAUTOSAVE:
        if (result == RESULT_RETRY) {
            this->mIGame->Retry(status);
            this->_ClearOldMsgs();
        } else {
            this->mCurTask++;
            if (this->mMainTask != TASK_MONITOR && this->mMainTask != TASK_BOOTUPCHECK) {
                this->_ClearMainTask();
            }
            this->mIGame->SetAutosaveDone(result, status, *static_cast<AutosaveState *>(other));
        }
        if (this->mMainTask == TASK_MONITOR) {
            if (this->_HasStatusChanged(status)) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
                this->mIGame->CardChanged(result, status);
            }
        }
        break;
    default:
        break;
    }

    if (this->mCurTask <= 2 && this->mTaskList[this->mCurTask].mTask != TASK_NONE) {
        if (this->mMemcardImpl->IsActiveTaskNone()) {
            this->_StartTask();
        }
        return;
    }

    if (this->mMainTask == TASK_MONITOR) {
        while (this->mCurTask != 0) {
            this->mTaskList[this->mCurTask].Clear();
            this->mCurTask--;
        }
        this->_StartTask();
        return;
    }

    if (this->mMainTask == TASK_BOOTUPCHECK) {
        this->_ClearMainTask();
        this->mIGame->BootupCheckDone(this->mTaskList[0].mDetails.mBootupCheck.mCardStatus,
                                      this->mTaskList[0].mDetails.mBootupCheck.mBootupResults);
    } else {
        this->_ClearMainTask();
    }
}

void TaskManager::_ClearOldMsgs() {
    this->mCheckingMsgShown = false;
    this->mWarningMsgShown = false;
    this->mLastMessageId = 0;
}

void TaskManager::_ClearTaskList() {
    {
        unsigned int iTask;

        for (iTask = 0; iTask <= 2; iTask++) {
            if (this->mTaskList[iTask].mTask == TASK_LOAD &&
                this->mTaskList[iTask].mDetails.mLoad.mLoadInfos != nullptr) {
                delete[] this->mTaskList[iTask].mDetails.mLoad.mLoadInfos;
                this->mTaskList[iTask].mDetails.mLoad.mLoadInfos = nullptr;
            }
            this->mTaskList[iTask].Clear();
        }
    }
    this->mCurTask = 0;
}

void TaskManager::_InitTaskList() {
    {
        unsigned int iTask;

        for (iTask = 0; iTask < 3; iTask++) {
            this->mTaskList[iTask].Clear();
        }
    }
}

bool TaskManager::_HasStatusChanged(CardStatus status) {
    bool hasStatusChanged = false;

    switch (this->mTaskList[0].mDetails.mMonitorStatus) {
    case STATUS_CARD_REMOVED:
        if (status == STATUS_NO_CARD) {
            this->mTaskList[0].mDetails.mMonitorStatus = status;
        } else {
            hasStatusChanged = true;
        }
        break;
    case STATUS_CARD_CHANGED:
        if (status == STATUS_OK) {
            this->mTaskList[0].mDetails.mMonitorStatus = STATUS_OK;
        } else {
            hasStatusChanged = true;
        }
        break;
    default:
        hasStatusChanged = true;
        if (this->mTaskList[0].mDetails.mMonitorStatus == status) {
            hasStatusChanged = false;
        }
        break;
    }
    /*
        if (this->mTaskList[0].mDetails.mMonitorStatus == STATUS_CARD_REMOVED) {
            if (status == STATUS_NO_CARD) {
                this->mTaskList[0].mDetails.mMonitorStatus = status;
            } else {
                hasStatusChanged = true;
            }
        } else {
            hasStatusChanged = true;
            if (this->mTaskList[0].mDetails.mMonitorStatus == status) {
                hasStatusChanged = false;
            }
        }
    }
    */
    return hasStatusChanged;
}

} // namespace RealmcIface
