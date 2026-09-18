#ifndef REALMEMCARD_IMPL_GC_INTERFACE_IMPL_H
#define REALMEMCARD_IMPL_GC_INTERFACE_IMPL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "memcard_interface_impl.h"

/* Las cinco cadenas de formato de los `LC_msg`. GCC 2.9 las emite en .rodata
   aunque la TU no llame nunca a esa sobrecarga: salen al escribir el .debug del
   inline (se ve en el .s, entre un `.section .debug` y el `.previous`). El
   troceador dejo esos bloques de 28 B en el comodin auto_05_804130A0_rodata en
   vez de asignarselos a su unidad, asi que una TU en la que las cinco esten
   MUERTAS puede apuntar al bloque que le corresponde con REALMC_LCFMT_BASE y no
   emitir una copia mas. Mismo patron que LIBGCC2_CLZ_TAB_SYM en libgcc2. */
#ifdef REALMC_LCFMT_BASE
#define REALMC_LCFMT_S (REALMC_LCFMT_BASE)
#define REALMC_LCFMT_SS (REALMC_LCFMT_BASE + 4)
#define REALMC_LCFMT_SSS (REALMC_LCFMT_BASE + 8)
#define REALMC_LCFMT_SSSS (REALMC_LCFMT_BASE + 12)
#define REALMC_LCFMT_SSDD (REALMC_LCFMT_BASE + 20)
#else
#define REALMC_LCFMT_S "s"
#define REALMC_LCFMT_SS "ss"
#define REALMC_LCFMT_SSS "sss"
#define REALMC_LCFMT_SSSS "ssss"
#define REALMC_LCFMT_SSDD "ssdd"
#endif

#ifdef REALMC_GCMSG_INTERFACE_ONLY
#pragma interface
#endif

namespace Realmc {

struct GCMessage : public Message {
    GCMessage() {
        this->Init();
    }
#ifdef REALMC_GC_MESSAGE_DTOR_OUTOFLINE
    virtual ~GCMessage();
#else
    virtual ~GCMessage() {}
#endif

#ifdef REALMC_GC_MESSAGE_INIT_INLINE
    inline void Init() {
        this->Clear();
    }
#else
    void Init();
#endif

    void _SetMsgOptions(int options);
    short *_LcGetSlotString(int slotnum);

#ifdef REALMC_GC_MESSAGE_INLINE
    inline void Clear() {
        memset(this, 0, 0x78);
    }
#else
    void Clear();
#endif

    void Set(LibMessage msg) {
        memset(this, 0, 0x78);
        this->mMsg = msg;
    }

    void Set(LibMessage msg, TaskResult result, CardStatus status) {
        this->Clear();
        this->mMsg = msg;
        this->mTaskResult = result;
        this->mCardStatus = status;
    }

    void Set(Message *msg) {
        memcpy(this, msg, sizeof(*this));
    }

    void LC_msg(int msgId, int options) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, nullptr);
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, int nSlot) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, REALMC_LCFMT_S, this->_LcGetSlotString(nSlot));
        this->_SetMsgOptions(options);
    }

    static int PackMsgOptions(int option1, int option2, int option3, int option4) {
        return option1 | option2 << 8 | option3 << 16 | option4 << 24;
    }

    void LC_msg(int msgId, int options, int nSlot, wchar_t *name) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, REALMC_LCFMT_SS, this->_LcGetSlotString(nSlot), name);
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, wchar_t *type, wchar_t *content) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, REALMC_LCFMT_SS, type, content);
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, wchar_t *name, int nSlot) {
        this->LC_msg(msgId, options, nSlot, name);
    }

    void LC_msg(int msgId, int options, wchar_t *gameTitle, wchar_t *fileName, int nSlot) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, REALMC_LCFMT_SSS, gameTitle, fileName, this->_LcGetSlotString(nSlot));
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, wchar_t *gameTitle, wchar_t *saveGameContent, wchar_t *fileName, int nSlot) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, REALMC_LCFMT_SSSS, gameTitle, saveGameContent, fileName, this->_LcGetSlotString(nSlot));
        this->_SetMsgOptions(options);
    }

    void LC_msg(int msgId, int options, int nSlot, wchar_t *name, unsigned int space, unsigned int files) {
        this->Set(LMSG_TRC);
        this->info.trc.mMsgId = msgId;
        this->info.trc.mMsg = Locale::GetString(msgId, REALMC_LCFMT_SSDD, this->_LcGetSlotString(nSlot), name, space, files);
        this->_SetMsgOptions(options);
    }
};

#ifdef REALMC_GCMSG_INTERFACE_ONLY
#pragma implementation "realmc_gcmsg_interface_end.h"
#endif

struct FindResult {
    FindResult() {}
    ~FindResult() {}

    GCMessage msg;
    char fileName[36];
};

struct GCInterface : public InterfaceImp {
    static inline void *operator new(unsigned int size) {
        return AllocateMemSize(0, size, 0, 0, 0);
    }
    static inline void operator delete(void *ptr, unsigned int size) {
        FreeMemSize(ptr, size);
    }
    static inline void *operator new(unsigned int, void *ptr) {
        return ptr;
    }
    static inline void operator delete(void *, void *) {}

    static GCMessage mTaskMsg;
    static volatile GCMessage *mpNewTaskMsg;

    static TaskManager mTaskManager;
    static TaskTrcStartGame mTaskTrcStartGame;
    static TaskTrcCardExists mTaskTrcCardExists;
    static TaskTrcGetCardInfo mTaskTrcGetCardInfo;
    static TaskTrcSaveFile mTaskTrcSaveFile;
    static TaskTrcLoadFile mTaskTrcLoadFile;
    static TaskTrcDeleteFile mTaskTrcDeleteFile;
    static TaskTrcListFiles mTaskTrcListFiles;
    static TaskTrcMount mTaskTrcMount;
    static TaskTrcFormat mTaskTrcFormat;
    static TaskShowCardStatusMsg mTaskShowCardStatusMsg;
    static TaskTrcCheckSpace mTaskTrcCheckSpace;
    static TaskCardExists mTaskCardExists;
    static TaskGetCardInfo mTaskGetCardInfo;
    static TaskMount mTaskMount;
    static TaskUnmount mTaskUnmount;
    static TaskOpen mTaskOpen;
    static TaskClose mTaskClose;
    static TaskRead mTaskRead;
    static TaskWrite mTaskWrite;
    static TaskSeek mTaskSeek;
    static TaskFlush mTaskFlush;
    static TaskDelete mTaskDelete;
    static TaskSetAttribute mTaskSetAttribute;
    static TaskFind mTaskFind;
    static FindResult mFindResult;
    static GCDriver *mpDriver;
    static UserMessage mUserMsg;
    static volatile bool mExitThread;
    static MessageTimer mMsgTimer;
    static BlockCalculatorImp mBlockCalculator;
    static unsigned short mCardName[48];

    GCInterface(const SystemInterface &iSystem);
    virtual ~GCInterface();

    static int TaskThread(void *);
    static InputOptions ConvertUmsgToOption(UserMessage, int, TaskID);
    static CardStatus CheckCard(const CardID &);
    static const unsigned short *GetCardName(const CardID &, wchar_t *, int);

    const Message *GetMessage(int) override;
    void SendMessage(UserMessage, int) override;
    bool IsBusy() override;
    unsigned int GetBlockSize(const CardID &) override;
    BlockCalculator *GetBlockCalculator() override;
    bool CheckForAutosaveCardRemoval() override;
    void ResetAutosaveCardDetection() override;

    void TrcStartGame(const StartGameInfo &) override;
    void TrcCardExists(const CardID &) override;
    void TrcGetCardInfo(const CardID &) override;
    void TrcLoadFile(const CardID &, const FileInfo &) override;
    void TrcSaveFile(const CardID &, const FileInfo &, SaveTaskType, unsigned int, unsigned int) override;
    void TrcListFiles(const CardID &, const FileInfo &, ListTaskType) override;
    void TrcDeleteFile(const CardID &, const FileInfo &) override;
    const unsigned short *GetCardName(const CardID &) override;
    void CardExists(const CardID &) override;
    void GetCardInfo(const CardID &) override;
    void Mount(const CardID &) override;
    void Unmount(const CardID &) override;
    void OpenFile(const CardID &, const FileInfo &, FileOpenMode) override;
    void CloseFile(OpenFileDescriptor *) override;
    void Read(OpenFileDescriptor *, void *, int) override;
    void Write(OpenFileDescriptor *, void *, int) override;
    void Seek(OpenFileDescriptor *, int, SeekFrom) override;
    void Flush(OpenFileDescriptor *) override;
    void DeleteFile(const CardID &, const char *, const char *) override;
    void SetFileAttribute(const CardID &, const char *, const char *, FileAttribute) override;
    void FindFile(const CardID &, const char *, const char *) override;
    void ClearTask() override;

    static void UpdateTaskCardExists();
    static void UpdateTaskGetCardInfo();
    static void UpdateTaskMount();
    static void UpdateTaskUnmount();
    static void UpdateTaskOpen();
    static void UpdateTaskClose();
    static void UpdateTaskRead();
    static void UpdateTaskWrite();
    static void UpdateTaskSeek();
    static void UpdateTaskFlush();
    static void UpdateTaskDelete();
    static void UpdateTaskSetAttribute();
    static void UpdateTaskFindFile();
    static FindResult *Find(const CardID &, const char *, bool);
    static void UpdateTaskTrcStartGame();
    static void UpdateTaskTrcCardExists();
    static void UpdateTaskTrcGetCardInfo();
    static void UpdateTaskTrcLoadFile();
    static void UpdateTaskTrcSaveCheck();
    static void UpdateTaskTrcSaveFile();
    static void UpdateTaskTrcDeleteFile();
    static void UpdateTaskTrcListFiles();
    static void UpdateTaskTrcMount();
    static void UpdateTaskTrcFormat();
    static void UpdateTaskShowCardStatusMessage();
    static void UpdateTaskTrcCheckSpace();

    inline void SetDummyMessage() {
        mTaskMsg.mMsg = LMSG_NONE;
        mpNewTaskMsg = &mTaskMsg;
    }
};

} // namespace Realmc

#endif
