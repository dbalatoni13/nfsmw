#include <stdio.h>
#include <string.h>

#include <dolphin/card.h>

#include "../../../include/common/realmemcard/gc_blockcalculator.h"
#include "../../../include/common/realmemcard/gc_driver.h"
#include "../../../include/common/realmemcard/impl/interfaceimp.h"

namespace Realmc {

extern "C" char lbl_804144E0[];

IThread *gInterfaceThread = nullptr;
IMutex *gInterfaceMutex = nullptr;

MessageTimer GCInterface::mMsgTimer;
BlockCalculatorImp GCInterface::mBlockCalculator;
TaskManager GCInterface::mTaskManager;
FindResult GCInterface::mFindResult;
GCMessage GCInterface::mTaskMsg;
TaskTrcStartGame GCInterface::mTaskTrcStartGame(&GCInterface::mTaskManager);
TaskTrcCardExists GCInterface::mTaskTrcCardExists(&GCInterface::mTaskManager);
TaskTrcGetCardInfo GCInterface::mTaskTrcGetCardInfo(&GCInterface::mTaskManager);
TaskTrcSaveFile GCInterface::mTaskTrcSaveFile(&GCInterface::mTaskManager);
TaskTrcLoadFile GCInterface::mTaskTrcLoadFile(&GCInterface::mTaskManager);
TaskTrcDeleteFile GCInterface::mTaskTrcDeleteFile(&GCInterface::mTaskManager);
TaskTrcListFiles GCInterface::mTaskTrcListFiles(&GCInterface::mTaskManager);
TaskCardExists GCInterface::mTaskCardExists(&GCInterface::mTaskManager);
TaskGetCardInfo GCInterface::mTaskGetCardInfo(&GCInterface::mTaskManager);
TaskMount GCInterface::mTaskMount(&GCInterface::mTaskManager);
TaskUnmount GCInterface::mTaskUnmount(&GCInterface::mTaskManager);
TaskOpen GCInterface::mTaskOpen(&GCInterface::mTaskManager);
TaskClose GCInterface::mTaskClose(&GCInterface::mTaskManager);
TaskRead GCInterface::mTaskRead(&GCInterface::mTaskManager);
TaskWrite GCInterface::mTaskWrite(&GCInterface::mTaskManager);
TaskSeek GCInterface::mTaskSeek(&GCInterface::mTaskManager);
TaskFlush GCInterface::mTaskFlush(&GCInterface::mTaskManager);
TaskDelete GCInterface::mTaskDelete(&GCInterface::mTaskManager);
TaskSetAttribute GCInterface::mTaskSetAttribute(&GCInterface::mTaskManager);
TaskFind GCInterface::mTaskFind(&GCInterface::mTaskManager);
TaskTrcMount GCInterface::mTaskTrcMount(&GCInterface::mTaskManager);
TaskTrcFormat GCInterface::mTaskTrcFormat(&GCInterface::mTaskManager);
TaskShowCardStatusMsg GCInterface::mTaskShowCardStatusMsg(&GCInterface::mTaskManager);
TaskTrcCheckSpace GCInterface::mTaskTrcCheckSpace(&GCInterface::mTaskManager);

GCDriver *GCInterface::mpDriver = nullptr;
UserMessage GCInterface::mUserMsg = UMSG_NONE;
volatile bool GCInterface::mExitThread = false;
unsigned short GCInterface::mCardName[48];
volatile GCMessage *GCInterface::mpNewTaskMsg = nullptr;

Interface *Interface::CreateInstance(const SystemInterface &iSystem) {
    Interface *interfaceInstance;

    SetMemAllocator(iSystem.mAllocator);
    interfaceInstance = gInterface;
    if (interfaceInstance == nullptr) {
        interfaceInstance = new GCInterface(iSystem);
        gInterface = interfaceInstance;
    } else {
        interfaceInstance->AddRef();
    }
    return gInterface;
}

GCInterface::GCInterface(const SystemInterface &iSystem)
    : InterfaceImp(iSystem) {
    memset(GCInterface::mCardName, 0, sizeof(GCInterface::mCardName));
    GCInterface::mMsgTimer.mNumSecondsDefaultDelay = 3;
    GCInterface::mpDriver = nullptr;
    GCInterface::mUserMsg = UMSG_NONE;
    GCInterface::mExitThread = false;
    GCInterface::mTaskMsg.Clear();
    memset(&GCInterface::mFindResult, 0, sizeof(GCInterface::mFindResult));
    GCInterface::mFindResult.msg.Clear();

    GCInterface::mpDriver = new GCDriver(&iSystem);
    gInterfaceMutex = this->mMutex->CreateInstance();
    gInterfaceThread = this->mISystem.mThread->CreateInstance();
    gInterfaceThread->SetStackSize(0x1000);
    gInterfaceThread->Begin(GCInterface::TaskThread, nullptr);
    gInterfaceThread->SetPriority(-2);

    CardID cardID(0, 0);
    GCInterface::mBlockCalculator.Init(GCInterface::mpDriver, GCInterface::mpDriver->GetSectorSize(cardID));
}

GCInterface::~GCInterface() {
    gInterfaceMutex->Lock();
    GCInterface::mExitThread = true;
    gInterfaceMutex->Unlock();
    gInterfaceThread->WaitForEnd();
    gInterfaceThread->Release();
    gInterfaceMutex->Release();
    if (GCInterface::mpDriver != nullptr) {
        delete GCInterface::mpDriver;
    }
}

int GCInterface::TaskThread(void *) {
    if (GCInterface::mExitThread) {
        return 0;
    }

    do {
        if (GCInterface::mpNewTaskMsg == nullptr && GCInterface::mTaskManager.GetCurrentTask() != nullptr) {
            gInterfaceMutex->Lock();
            if (GCInterface::mTaskManager.GetCurrentTask() != nullptr) {
                memset(&GCInterface::mTaskMsg, 0, sizeof(GCInterface::mTaskMsg));
                GCInterface::mpNewTaskMsg = nullptr;
                switch (GCInterface::mTaskManager.GetCurrentTask()->GetID()) {
                case TID_TRC_STARTGAME:
                    GCInterface::UpdateTaskTrcStartGame();
                    break;
                case TID_TRC_CARDEXISTS:
                    GCInterface::UpdateTaskTrcCardExists();
                    break;
                case TID_TRC_GETCARDINFO:
                    GCInterface::UpdateTaskTrcGetCardInfo();
                    break;
                case TID_TRC_SAVECHECK:
                    GCInterface::UpdateTaskTrcSaveCheck();
                    break;
                case TID_TRC_SAVEFILE:
                    GCInterface::UpdateTaskTrcSaveFile();
                    break;
                case TID_TRC_LOADFILE:
                    GCInterface::UpdateTaskTrcLoadFile();
                    break;
                case TID_TRC_DELETEFILE:
                    GCInterface::UpdateTaskTrcDeleteFile();
                    break;
                case TID_TRC_LISTFILES:
                    GCInterface::UpdateTaskTrcListFiles();
                    break;
                case TID_TRC_MOUNT:
                    GCInterface::UpdateTaskTrcMount();
                    break;
                case TID_TRC_FORMAT:
                    GCInterface::UpdateTaskTrcFormat();
                    break;
                case TID_SHOW_CARD_STATUS_MSG:
                    GCInterface::UpdateTaskShowCardStatusMessage();
                    break;
                case TID_CHECK_SPACE:
                    GCInterface::UpdateTaskTrcCheckSpace();
                    break;
                case TID_CARD_EXISTS:
                    GCInterface::UpdateTaskCardExists();
                    break;
                case TID_GET_CARD_INFO:
                    GCInterface::UpdateTaskGetCardInfo();
                    break;
                case TID_MOUNT:
                    GCInterface::UpdateTaskMount();
                    break;
                case TID_UNMOUNT:
                    GCInterface::UpdateTaskUnmount();
                    break;
                case TID_OPEN:
                    GCInterface::UpdateTaskOpen();
                    break;
                case TID_CLOSE:
                    GCInterface::UpdateTaskClose();
                    break;
                case TID_READ:
                    GCInterface::UpdateTaskRead();
                    break;
                case TID_WRITE:
                    GCInterface::UpdateTaskWrite();
                    break;
                case TID_SEEK:
                    GCInterface::UpdateTaskSeek();
                    break;
                case TID_FLUSH:
                    GCInterface::UpdateTaskFlush();
                    break;
                case TID_DELETE:
                    GCInterface::UpdateTaskDelete();
                    break;
                case TID_SET_ATTRIBUTES:
                    GCInterface::UpdateTaskSetAttribute();
                    break;
                case TID_FIND:
                    GCInterface::UpdateTaskFindFile();
                    break;
                default:
                    break;
                }
            }
            if (GCInterface::mTaskMsg.mMsg != LMSG_NONE) {
                GCInterface::mpNewTaskMsg = &GCInterface::mTaskMsg;
            }
            gInterfaceMutex->Unlock();
        }
        gInterfaceThread->Sleep(5);
    } while (!GCInterface::mExitThread);

    return 0;
}

const Message *GCInterface::GetMessage(int elapsedTime) {
    GCMessage *pMsg;
    GCInterface::mMsgTimer.AddElapsedTime(elapsedTime);
    pMsg = nullptr;
    static GCMessage sMsg;
    memset(&sMsg, 0, sizeof(GCMessage));
    if (GCInterface::mpNewTaskMsg != nullptr) {
        gInterfaceMutex->Lock();
        sMsg = *const_cast<GCMessage *>(GCInterface::mpNewTaskMsg);
        pMsg = &sMsg;
        GCInterface::mpNewTaskMsg = nullptr;
        gInterfaceMutex->Unlock();
    }
    return pMsg;
}

void GCInterface::SendMessage(UserMessage msg, int) {
    gInterfaceMutex->Lock();
    GCInterface::mUserMsg = msg;
    if (msg == UMSG_EXPIRE_DELAY) {
        GCInterface::mMsgTimer.Stop();
    }
    gInterfaceMutex->Unlock();
}

bool GCInterface::IsBusy() {
    return GCInterface::mTaskManager.GetCurrentTask() != nullptr;
}

unsigned int GCInterface::GetBlockSize(const CardID &cardID) {
    return GCInterface::mpDriver->GetSectorSize(cardID);
}

BlockCalculator *GCInterface::GetBlockCalculator() {
    return &GCInterface::mBlockCalculator;
}

bool GCInterface::CheckForAutosaveCardRemoval() {
    bool removed;

    removed = false;
    if (!GCInterface::mpDriver->IsCardPresent() && GCInterface::mpDriver->WasCardPresent()) {
        removed = true;
    }
    return removed;
}

void GCInterface::ResetAutosaveCardDetection() {
    GCInterface::mpDriver->ResetWasCardPresent();
}

InputOptions GCInterface::ConvertUmsgToOption(UserMessage umsg, int options, TaskID taskID) {
    int numOptions;
    int cOption;
    InputOptions ioptions[4];
    char errorMsg[128];

    if (taskID != TID_TRC_STARTGAME) {
        options &= 0x0fffffff;
    }

    numOptions = 0;
    cOption = 1;
    if (options != 0) {
        do {
            if ((options & 1) != 0) {
                ioptions[numOptions++] = static_cast<InputOptions>(cOption);
            }
            cOption <<= 1;
            options >>= 1;
        } while (options != 0);
    }

    if (umsg == UMSG_OPTION1 && numOptions > 0) {
        return static_cast<InputOptions>(ioptions[0]);
    }
    if (umsg == UMSG_OPTION4 && numOptions > 1) {
        return static_cast<InputOptions>(ioptions[1]);
    }
    if (umsg == UMSG_OPTION2 && numOptions > 2) {
        return static_cast<InputOptions>(ioptions[2]);
    }
    if (umsg == UMSG_OPTION3 && numOptions > 3) {
        return static_cast<InputOptions>(ioptions[3]);
    }

    sprintf(errorMsg, lbl_804144E0, umsg);
    return IO_NONE;
}

CardStatus GCInterface::CheckCard(const CardID &) {
    return GCInterface::mpDriver->IsCardPresent() ? STATUS_OK : STATUS_NO_CARD;
}

void GCInterface::ClearTask() {
    TaskTrc *curTask;

    gInterfaceMutex->Lock();
    curTask = static_cast<TaskTrc *>(GCInterface::mTaskManager.GetCurrentTask());
    if (curTask != nullptr) {
        GCInterface::mpDriver->Unmount(curTask->mCardID);
    }
    GCInterface::mpNewTaskMsg = nullptr;
    memset(&GCInterface::mTaskMsg, 0, sizeof(GCInterface::mTaskMsg));
    GCInterface::mTaskManager.ClearTask();
    gInterfaceMutex->Unlock();
}

inline void TaskManager::StartTask(Task *newTask) {
    this->TrcSingletonAssert(newTask);
    if (this->mCurrentTask != nullptr) {
        this->mTaskStack.Push(this->mCurrentTask);
    }
    newTask->SetState(TS_START, TS_START);
    this->mCurrentTask = newTask;
    if (this->IsPublicTrcTask(newTask)) {
        this->mPublicTrcFunctionRefcount++;
    }
}

inline void TaskManager::EndTask(Task *task) {
    this->mTaskStack.Pop(&this->mCurrentTask);
    if (this->IsPublicTrcTask(task)) {
        this->mPublicTrcFunctionRefcount--;
    }
}

inline bool TaskManager::IsPublicTrcTask(Task *task) {
    if (task != nullptr) {
        switch (task->mID) {
        case TID_TRC_STARTGAME:
        case TID_TRC_CARDEXISTS:
        case TID_TRC_GETCARDINFO:
        case TID_TRC_LOADFILE:
        case TID_TRC_SAVECHECK:
        case TID_TRC_SAVEFILE:
        case TID_TRC_DELETEFILE:
        case TID_TRC_LISTFILES:
            return true;
        default:
            break;
        }
    }
    return false;
}

inline void TaskManager::TrcSingletonAssert(Task *newTask) {
    if (newTask != nullptr) {
        if (newTask->mID < TID_TRC_STARTGAME) {
            return;
        }
    }
}

} // namespace Realmc
