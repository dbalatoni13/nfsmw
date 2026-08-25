#include <cstring>
#include <new>

#include "../../../include/common/realcore/file/driver.h"
#define REALCORE_FILESYS_IMPLEMENTATION
#include "../../../include/common/realcore/file/filesys.h"
#undef REALCORE_FILESYS_IMPLEMENTATION
#include "../../../include/common/realcore/std/list.h"
#include "../../../include/common/realcore/std.h"
#include "../../../include/common/realcore/system.h"
#include "../../../include/common/realcore/system/systask.h"
#include "../../../include/common/realcore/system/threads.h"

struct FILESYSHANDLE;
struct FILEDEVICE;

class FILEOPERATION : public ListNodeS {
  protected:
    int id;
    bool cancelled;
    signed char status;
    unsigned char priority;
    FILESYSHANDLE *filehandle;
    void *userdata;
    FILESYS_CALLBACK *callback;
    long long value;
    long long amount;
    void *data;
    unsigned long long totalbytes;

  public:
    FILEOPERATION(int _priority, void *_userdata, FILEDEVICE *device);
    virtual ~FILEOPERATION() {}
    static void *operator new(unsigned int size);
    virtual void Exec(FILEDEVICE *device) = 0;
    virtual long long Complete() = 0;
    virtual void Cancel(FILEDEVICE *device);
    void SetName(const char *name);
    void AddToQueue();

    int GetId() {
        return this->id;
    }

    int GetPriority() {
        return this->priority;
    }

    int GetStatus() {
        return this->status;
    }

    FILESYS_CALLBACK *GetCallback() {
        return this->callback;
    }

    bool IsCancelled() {
        return this->cancelled;
    }

    void *GetUserData() {
        return this->userdata;
    }

    void ClearId() {
        this->id = 0;
    }

    void SetPriority(unsigned char _priority) {
        this->priority = _priority;
    }

    void SetCallback(FILESYS_CALLBACK *_callback) {
        this->callback = _callback;
    }

    static FILEDEVICE *GetDevice(int ophandle);

  private:
    int iGetNextOpId(FILEDEVICE *dev);
};

struct FileOperationQueue : public ListSingle<FILEOPERATION> {
    FileOperationQueue() {}
    FILEOPERATION *Find(int ophandle, bool bRemoveFromQueue);
};

struct FILESYSHANDLE : public ListNodeS {
    FILESYSHANDLE *pParentFile;
    int hFile;
    FILEDEVICE *dev;
    long long length;
    long long offset;
};

struct FILEDEVICE : public ListNodeS {
    volatile int running;
    volatile int terminate;
    FILEOPERATION *current;
    FileOperationQueue pending;
    FileOperationQueue completed;
    RealSystem::Mutex mutex;
    SIGNAL signal;
    RealSystem::Mutex atomic;
    THREAD deviceThread;
    void *pThreadStack;
    SIGNAL attention;
    int minpriority;
    RealFile::DeviceDriver *drv;
};

struct FILESYSINFO {
    ListSingle<FILEOPERATION> FreeOps;
    RealSystem::Mutex FreeOpsMutex;
    ListSingle<FILESYSHANDLE> FreeFileSysHandles;
    RealSystem::Mutex FreeFileSysHandlesMutex;
    int NextOp;
    RealSystem::Mutex NextOpMutex;
    ListSingle<FILEDEVICE> FreeDevices;
    ListSingle<FILEDEVICE> AllocatedDevices;
    char **psearchpaths;
    char *searchpath;
    FILEOPERATION *OpMemory;
    FILESYSHANDLE *FileSysHandleMemory;
    FILEDEVICE *DeviceMemory;

    FILESYSINFO() {}
};

struct NullFileDriver : public RealFile::DeviceDriver {
    NullFileDriver(const char *pName) : RealFile::DeviceDriver(pName) {}
    virtual ~NullFileDriver() override {}

    virtual EAFileHandle Open(const char *, int, int *) override {
        return -1;
    }

    virtual void Close(EAFileHandle) override {}

    virtual uint32_t Read(EAFileHandle, void *, unsigned int, RealFile::DeviceDriver *,
                          EAFileHandle) override {
        return 0;
    }

    virtual uint64_t Seek(EAFileHandle, uint64_t, int, RealFile::DeviceDriver *,
                          EAFileHandle) override {
        return 0;
    }

    virtual uint64_t Getsize(EAFileHandle) override {
        return 0;
    }
};

static FILESYSINFO *gpFileSysInfo;
static void *pDeviceMem;
static NullFileDriver nulldrv("null:");
extern RealFile::DeviceDriver *libdevice[2];
static int iDeviceCommandProcessorThreadFunc(void *param);
FILEDEVICE *FILE_nametodevice(const char *name);

namespace RealFile {

DeviceDriver::DeviceDriver(const char *name) {
    strcpy(this->mDeviceName, name);
}

}

static void iDefaultFilesysCallbackFunc(int, int, void *) {}

FILEDEVICE *FILEOPERATION::GetDevice(int ophandle) {
    return &gpFileSysInfo->DeviceMemory[static_cast<unsigned char>(ophandle)];
}

FILEOPERATION *FileOperationQueue::Find(int ophandle, bool bRemoveFromQueue) {
    ListSingleIterator<FILEOPERATION> Itr;
    FILEOPERATION *prev = nullptr;
    for (Itr = this->Begin(); *Itr != nullptr; Itr++) {
        if ((*Itr)->GetId() == ophandle) {
            if (bRemoveFromQueue) {
                this->Remove(*Itr, prev);
            }
            return *Itr;
        }
        prev = *Itr;
    }
    return nullptr;
}

static void iStartDevice(FILEDEVICE *dev) {
    gpFileSysInfo->FreeOpsMutex.Lock();
    dev->mutex.Create();
    dev->current = nullptr;
    SIGNAL_create(&dev->signal);
    SIGNAL_create(&dev->attention);
    dev->atomic.Create();
    dev->minpriority = 255;
    dev->drv->Init();
    dev->pThreadStack = gFileSysOpts.allocator->Alloc(
        gFileSysOpts.ThreadStackSize,
        EA::TagValuePair(EA::Allocator::ATT_ALLOC_HIGH, 1) +
            EA::TagValuePair(EA::Allocator::ATT_ALIGNMENT, 32) +
            EA::TagValuePair(EA::Allocator::ATT_NAME, "File System") +
            EA::TagValuePair(EA::Allocator::ATT_FILE,
                             "d:/packages/realcore/6.24.00/source/file/cmn/filesys.cpp") +
            EA::TagValuePair(EA::Allocator::ATT_LINE, 312));
    THREAD_create(&dev->deviceThread, iDeviceCommandProcessorThreadFunc, dev, dev->pThreadStack,
                  gFileSysOpts.ThreadStackSize, 1);
    SIGNAL_wait(&dev->attention);
    gpFileSysInfo->FreeOpsMutex.Unlock();
}

int FILEOPERATION::iGetNextOpId(FILEDEVICE *dev) {
    int retval;
    int deviceid;
    gpFileSysInfo->NextOpMutex.Lock();
    retval = gpFileSysInfo->NextOp++;
    if (gpFileSysInfo->NextOp == 0x10000) {
        gpFileSysInfo->NextOp = 1;
    }
    gpFileSysInfo->NextOpMutex.Unlock();
    deviceid = dev - gpFileSysInfo->DeviceMemory;
    return (retval << 8) | deviceid;
}

FILEOPERATION::FILEOPERATION(int _priority, void *_userdata, FILEDEVICE *device) {
    if (device->running == 0) {
        iStartDevice(device);
    }
    this->priority = _priority;
    this->cancelled = false;
    this->status = 0;
    this->filehandle = reinterpret_cast<FILESYSHANDLE *>(-1);
    this->userdata = _userdata;
    this->callback = iDefaultFilesysCallbackFunc;
    this->value = 0;
    this->amount = 0;
    this->data = nullptr;
    this->id = this->iGetNextOpId(device);
}

void *FILEOPERATION::operator new(unsigned int) {
    FILEOPERATION *op;
    gpFileSysInfo->FreeOpsMutex.Lock();
    op = gpFileSysInfo->FreeOps.Pop();
    gpFileSysInfo->FreeOpsMutex.Unlock();
    return op;
}

void FILEOPERATION::SetName(const char *name) {
    int len;
    int allocamount;
    if (name == nullptr) {
        name = "";
    }
    len = strlen(name) + 1;
    this->data = gFileSysOpts.allocator->Alloc(
        len,
        EA::TagValuePair(EA::Allocator::ATT_ALLOC_HIGH, 1) +
            EA::TagValuePair(EA::Allocator::ATT_NAME, name) +
            EA::TagValuePair(EA::Allocator::ATT_FILE,
                             "d:/packages/realcore/6.24.00/source/file/cmn/filesys.cpp") +
            EA::TagValuePair(EA::Allocator::ATT_LINE, 432));
    MEM_copy(this->data, name, len);
}

void FILEOPERATION::Cancel(FILEDEVICE *device) {
    this->cancelled = true;
    if (device->pending.Remove(this, nullptr)) {
        device->completed.Push(this);
        this->callback(this->id, -1, this->userdata);
    }
}

static FILESYSHANDLE *iAllocateFileSysHandle() {
    FILESYSHANDLE *fs_h;
    gpFileSysInfo->FreeFileSysHandlesMutex.Lock();
    fs_h = gpFileSysInfo->FreeFileSysHandles.Pop();
    gpFileSysInfo->FreeFileSysHandlesMutex.Unlock();
    fs_h->hFile = -1;
    return fs_h;
}

static void iFreeFileSysHandle(FILESYSHANDLE *fs_h) {
    MEM_clear(fs_h, sizeof(FILESYSHANDLE));
    gpFileSysInfo->FreeFileSysHandlesMutex.Lock();
    gpFileSysInfo->FreeFileSysHandles.Push(fs_h);
    gpFileSysInfo->FreeFileSysHandlesMutex.Unlock();
}

static FILESYSHANDLE *iOpenFileSysHandle(const char *filename, int modeflags,
                                        FILEDEVICE *device) {
    int file_drv_handle;
    int ParentFileHandle;
    file_drv_handle = -1;
    ParentFileHandle = -1;
    if (device == gpFileSysInfo->DeviceMemory) {
        char *sp;
        char fullname[256];
        for (int i = 0;
             i < gFileSysOpts.nSearchLocs &&
             (sp = gpFileSysInfo->psearchpaths[i]) != nullptr;
             i++) {
            device = FILE_nametodevice(sp);
            if (device->running == 0) {
                iStartDevice(device);
            }
            strcpy(fullname, sp);
            strcat(fullname, "/");
            strcat(fullname, filename);
            file_drv_handle = device->drv->Open(fullname, modeflags, &ParentFileHandle);
            if (file_drv_handle != -1) {
                break;
            }
        }
    } else {
        file_drv_handle = device->drv->Open(filename, modeflags, &ParentFileHandle);
    }
    if (file_drv_handle != -1) {
        FILESYSHANDLE *fs_h;
        if ((modeflags & 0x10000) != 0) {
            device->drv->Close(file_drv_handle);
            return reinterpret_cast<FILESYSHANDLE *>(1);
        }
        fs_h = iAllocateFileSysHandle();
        fs_h->hFile = file_drv_handle;
        fs_h->dev = device;
        fs_h->length = device->drv->Getsize(file_drv_handle);
        fs_h->pParentFile = fs_h;
        if (ParentFileHandle != -1) {
            fs_h->pParentFile = reinterpret_cast<FILESYSHANDLE *>(ParentFileHandle);
        }
        return fs_h;
    }
    return reinterpret_cast<FILESYSHANDLE *>(-1);
}

static void iCloseFileSysHandle(FILESYSHANDLE *h) {
    h->dev->drv->Close(h->hFile);
    iFreeFileSysHandle(h);
}

static FILEOPERATION *iGetOpFromHandle(FILEDEVICE *device, int ophandle, int *ispending) {
    FILEOPERATION *result;
    result = nullptr;
    *ispending = 0;
    if (device->running != 0) {
        result = device->current;
        if (result != nullptr && result->GetId() == ophandle) {
            *ispending = -1;
        } else {
            result = device->pending.Find(ophandle, false);
            if (result != nullptr) {
                *ispending = 1;
            } else {
                result = device->completed.Find(ophandle, false);
            }
        }
    }
    return result;
}

void FILEOPERATION::AddToQueue() {
    FILEDEVICE *device;
    ListSingleIterator<FILEOPERATION> Itr;
    FILEOPERATION *prev;
    unsigned int newoppriority;
    device = this->GetDevice(this->id);
    device->mutex.Lock();
    prev = nullptr;
    newoppriority = (static_cast<unsigned int>(this->priority) << 24) |
                    (static_cast<unsigned int>(this->id) & 0xffff00);
    for (Itr = device->pending.Begin(); *Itr != nullptr; Itr++) {
        unsigned int curpriority;
        curpriority = (static_cast<unsigned int>((*Itr)->priority) << 24) |
                      (static_cast<unsigned int>((*Itr)->id) & 0xffff00);
        if (newoppriority < curpriority) {
            break;
        }
        prev = *Itr;
    }
    device->pending.InsertAfter(prev, this);
    device->mutex.Unlock();
    SIGNAL_set(&device->signal);
}

static int iDeviceCommandProcessorThreadFunc(void *param) {
    FILEDEVICE *device;
    FILEOPERATION *op;
    device = static_cast<FILEDEVICE *>(param);
    device->running = 1;
    SIGNAL_set(&device->attention);
    while (device->terminate == 0) {
        device->mutex.Lock();
        device->current = nullptr;
        op = device->pending.Head();
        if (op != nullptr && op->GetPriority() <= device->minpriority) {
            op = device->pending.Pop();
            device->current = op;
        }
        device->mutex.Unlock();
        if (device->current == nullptr) {
            SIGNAL_wait(&device->signal);
        } else {
            if (op->IsCancelled() != true) {
                op->Exec(device);
            }
            if (op->GetStatus() != 0) {
                device->mutex.Lock();
                device->completed.Push(op);
                device->current = nullptr;
                device->mutex.Unlock();
                op->GetCallback()(op->GetId(), op->IsCancelled() ? -1 : op->GetStatus(),
                                  op->GetUserData());
                SIGNAL_set(&device->attention);
            }
        }
    }
    device->running = 0;
    return 0;
}

bool FILE_init(void *buf, int bufsize) {
    int i;
    int nDevices;
    FILEOPERATION *op;
    FILESYSHANDLE *fs_h;
    FILEDEVICE *dev;
    FILEDEVICE *device;
    if (buf == nullptr) {
        bufsize = FILE_overhead();
        pDeviceMem = gFileSysOpts.allocator->Alloc(
            bufsize, EA::TagValuePair(EA::Allocator::ATT_NAME, "File System"));
        buf = pDeviceMem;
    }
    MEM_clear(buf, bufsize);
    gpFileSysInfo = new (buf) FILESYSINFO;
    gpFileSysInfo->psearchpaths = reinterpret_cast<char **>(gpFileSysInfo + 1);
    gpFileSysInfo->searchpath =
        reinterpret_cast<char *>(gpFileSysInfo->psearchpaths + gFileSysOpts.nSearchLocs);
    gpFileSysInfo->OpMemory = static_cast<FILEOPERATION *>(
        AlignAddrUp(gpFileSysInfo->searchpath + gFileSysOpts.nSearchPathLength, 8));
    gpFileSysInfo->FileSysHandleMemory = static_cast<FILESYSHANDLE *>(
        AlignAddrUp(gpFileSysInfo->OpMemory + gFileSysOpts.MaxFileOps, 8));
    gpFileSysInfo->DeviceMemory =
        reinterpret_cast<FILEDEVICE *>(gpFileSysInfo->FileSysHandleMemory +
                                       gFileSysOpts.MaxOpenFiles);
    gpFileSysInfo->FreeOpsMutex.Create();
    gpFileSysInfo->FreeFileSysHandlesMutex.Create();
    for (i = 0, op = gpFileSysInfo->OpMemory; i < gFileSysOpts.MaxFileOps; i++, op++) {
        gpFileSysInfo->FreeOps.PushTail(op);
    }
    for (i = 0, fs_h = gpFileSysInfo->FileSysHandleMemory; i < gFileSysOpts.MaxOpenFiles;
         i++, fs_h++) {
        gpFileSysInfo->FreeFileSysHandles.PushTail(fs_h);
    }
    for (i = 0, dev = gpFileSysInfo->DeviceMemory; i < gFileSysOpts.MaxDevices; i++, dev++) {
        gpFileSysInfo->FreeDevices.PushTail(dev);
    }
    gpFileSysInfo->NextOp = 1;
    gpFileSysInfo->NextOpMutex.Create();
    RealFile::AddDevice(&nulldrv);
    nDevices = sizeof(libdevice) / sizeof(RealFile::DeviceDriver *);
    for (i = 0; i < nDevices; i++) {
        RealFile::AddDevice(libdevice[i]);
    }
    RealFile::SetSearchPath(gFileSysOpts.DiscType == 1 ? "dvd:" : "hd:");
    device = FILE_nametodevice(*gpFileSysInfo->psearchpaths);
    iStartDevice(device);
    SYSTEM_addexit(FILE_restore);
    bIsFileSysInitialized = true;
    return true;
}

int FILE_overhead() {
    return gFileSysOpts.MaxOpenFiles * sizeof(FILESYSHANDLE) + sizeof(FILESYSINFO) +
           gFileSysOpts.MaxFileOps * sizeof(FILEOPERATION) +
           gFileSysOpts.MaxDevices * (sizeof(FILEDEVICE) + 1) +
           gFileSysOpts.nSearchLocs * sizeof(char *) + gFileSysOpts.nSearchPathLength + 24;
}

void FILE_restore() {
    {
        int i;
        for (i = 0; i < gFileSysOpts.MaxDevices; i++) {
            RealFile::RemoveDevice(i);
        }
    }
    gpFileSysInfo->NextOpMutex.Destroy();
    gpFileSysInfo->FreeFileSysHandlesMutex.Destroy();
    gpFileSysInfo->FreeOpsMutex.Destroy();
    gpFileSysInfo->~FILESYSINFO();
    gpFileSysInfo = nullptr;
    if (pDeviceMem != nullptr) {
        gFileSysOpts.allocator->Free(pDeviceMem, 0);
        pDeviceMem = nullptr;
    }
    bIsFileSysInitialized = false;
}

int FILESYS_opstatus(int ophandle) {
    int ispending;
    int result = 0;
    FILEDEVICE *device = FILEOPERATION::GetDevice(ophandle);
    device->mutex.Lock();
    FILEOPERATION *op = iGetOpFromHandle(device, ophandle, &ispending);
    if (ispending == 0) {
        if (op != nullptr) {
            if (op->IsCancelled() == true) {
                result = -1;
            } else {
                result = op->GetStatus();
            }
        } else {
            result = -3;
        }
    }
    device->mutex.Unlock();
    return result;
}

int FILESYS_waitop(int ophandle) {
    int ispending;
    if (ophandle == 0) {
        return -3;
    }
    FILEDEVICE *device = FILEOPERATION::GetDevice(ophandle);
    if (device->running == 0) {
        return -3;
    }
    FILEOPERATION *op;
    do {
        device->mutex.Lock();
        op = iGetOpFromHandle(device, ophandle, &ispending);
        if (op != nullptr) {
            ispending &= 1;
        } else {
            ispending = 0;
        }
        device->mutex.Unlock();
        if (ispending == 0) {
            break;
        }
        if (THREAD_iscurrent(nullptr)) {
            SYNCTASK_run();
            THREAD_yield(1);
        } else {
            FILEDEVICE *device = FILEOPERATION::GetDevice(ophandle);
            SIGNAL_wait(&device->attention);
        }
    } while (ispending != 0);
    return FILESYS_opstatus(ophandle);
}

long long FILESYS_completeop64(int ophandle) {
    long long result;
    FILEOPERATION *op;
    FILEDEVICE *device = FILEOPERATION::GetDevice(ophandle);
    device->mutex.Lock();
    op = device->completed.Find(ophandle, true);
    device->mutex.Unlock();
    result = op->Complete();
    op->ClearId();
    gpFileSysInfo->FreeOpsMutex.Lock();
    gpFileSysInfo->FreeOps.PushTail(op);
    gpFileSysInfo->FreeOpsMutex.Unlock();
    return result;
}

int FILESYS_completeop(int ophandle) {
    return FILESYS_completeop64(ophandle);
}

void FILESYS_callbackop(int ophandle, FILESYS_CALLBACK func) {
    int ispending;
    FILEDEVICE *device = FILEOPERATION::GetDevice(ophandle);
    device->mutex.Lock();
    FILEOPERATION *op = iGetOpFromHandle(device, ophandle, &ispending);
    if (ispending != 0) {
        op->SetCallback(func);
    } else {
        func(op->GetId(), op->GetStatus(), op->GetUserData());
    }
    device->mutex.Unlock();
}

void FILESYS_priorityop(int ophandle, int priority) {
    FILEDEVICE *device = FILEOPERATION::GetDevice(ophandle);
    if (device != nullptr) {
        FILEOPERATION *op;
        device->mutex.Lock();
        op = device->pending.Find(ophandle, true);
        device->mutex.Unlock();
        if (op != nullptr) {
            op->SetPriority(priority);
            op->AddToQueue();
        }
    }
}

struct ExistOperation : public FILEOPERATION {
    ExistOperation(const char *name, int priority, void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->SetName(name);
    }

    virtual ~ExistOperation() override {}
    virtual void Exec(FILEDEVICE *device) override {
        this->value = iOpenFileSysHandle(static_cast<char *>(this->data), 0x30001,
                                         device) !=
                              reinterpret_cast<FILESYSHANDLE *>(-1)
                          ? 1
                          : 0;
        this->status = 1;
    }

    virtual long long Complete() override {
        gFileSysOpts.allocator->Free(this->data, 0);
        return this->value;
    }
};

int FILESYS_exists(const char *name, int priority, void *userdata) {
    ExistOperation *op =
        new ExistOperation(name, priority, userdata, FILE_nametodevice(name));
    op->AddToQueue();
    return op->GetId();
}

struct OpenOperation : public FILEOPERATION {
    OpenOperation(const char *name, unsigned int modeflags, int priority, void *userdata,
                  FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->SetName(name);
        this->value = modeflags;
    }

    virtual ~OpenOperation() override {}
    virtual void Exec(FILEDEVICE *device) override {
        this->filehandle = iOpenFileSysHandle(static_cast<char *>(this->data),
                                              this->value, device);
        if (this->filehandle != reinterpret_cast<FILESYSHANDLE *>(-1)) {
            this->status = 1;
        } else {
            this->status = -2;
        }
    }

    virtual long long Complete() override {
        gFileSysOpts.allocator->Free(this->data, 0);
        if (this->IsCancelled() == true) {
            if (this->filehandle != reinterpret_cast<FILESYSHANDLE *>(-1)) {
                iCloseFileSysHandle(this->filehandle);
            }
            return 0;
        }
        return this->filehandle != reinterpret_cast<FILESYSHANDLE *>(-1)
                   ? reinterpret_cast<int>(this->filehandle)
                   : 0;
    }
};

int FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata) {
    OpenOperation *op =
        new OpenOperation(name, modeflags, priority, userdata, FILE_nametodevice(name));
    op->AddToQueue();
    return op->GetId();
}

struct CloseOperation : public FILEOPERATION {
    CloseOperation(FILESYSHANDLE *_filehandle, int priority, void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->filehandle = _filehandle;
    }

    virtual ~CloseOperation() override {}
    virtual void Exec(FILEDEVICE *) override {
        this->status = 1;
    }

    virtual long long Complete() override {
        iCloseFileSysHandle(this->filehandle);
        return 1;
    }

    virtual void Cancel(FILEDEVICE *) override {}
};

int FILESYS_close(int filehandle, int priority, void *userdata) {
    FILESYSHANDLE *hFile = reinterpret_cast<FILESYSHANDLE *>(filehandle);
    CloseOperation *op = new CloseOperation(hFile, priority, userdata, hFile->dev);
    op->AddToQueue();
    return op->GetId();
}

struct ReadOperation : public FILEOPERATION {
    ReadOperation(FILESYSHANDLE *_filehandle, int bytes, int offset, void *buffer, int priority,
                  void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->filehandle = _filehandle;
        this->amount = bytes;
        this->value = offset;
        this->data = buffer;
        this->totalbytes = 0;
    }

    virtual ~ReadOperation() override {}
    virtual void Exec(FILEDEVICE *device) override {
        this->filehandle->pParentFile->offset =
            device->drv->Seek(this->filehandle->hFile, this->value, 0,
                               this->filehandle->pParentFile->dev->drv,
                               this->filehandle->pParentFile->hFile);
        unsigned int NumBytesToRead = static_cast<unsigned int>(this->amount);
        unsigned int DeviceOptimalReadSize = device->drv->GetOptimalReadSize();
        if (DeviceOptimalReadSize != 0) {
            if (NumBytesToRead > DeviceOptimalReadSize) {
                NumBytesToRead = DeviceOptimalReadSize;
            }
        }
        unsigned int nread = device->drv->Read(
            this->filehandle->hFile, this->data, NumBytesToRead,
            this->filehandle->pParentFile->dev->drv, this->filehandle->pParentFile->hFile);
        this->amount -= nread;
        this->value += nread;
        this->totalbytes += nread;
        this->data = static_cast<char *>(this->data) + nread;
        if (this->amount > 0 && nread == NumBytesToRead) {
            this->AddToQueue();
        } else {
            this->status = 1;
        }
    }
    virtual long long Complete() override {
        return this->totalbytes;
    }
};

int FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                 void *userdata) {
    FILESYSHANDLE *hFile = reinterpret_cast<FILESYSHANDLE *>(filehandle);
    ReadOperation *op =
        new ReadOperation(hFile, bytes, offset, buffer, priority, userdata, hFile->dev);
    op->AddToQueue();
    return op->GetId();
}

struct ReadLargeOperation : public FILEOPERATION {
    ReadLargeOperation(FILESYSHANDLE *_filehandle, unsigned long long bytes,
                       unsigned long long offset, void *buffer, int priority, void *userdata,
                       FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->filehandle = _filehandle;
        this->amount = bytes;
        this->value = offset;
        this->data = buffer;
        this->totalbytes = 0;
    }

    virtual ~ReadLargeOperation() override {}
    virtual void Exec(FILEDEVICE *device) override {
        this->filehandle->pParentFile->offset =
            device->drv->Seek(this->filehandle->hFile, this->value, 0,
                               this->filehandle->pParentFile->dev->drv,
                               this->filehandle->pParentFile->hFile);
        unsigned int NumBytesToRead = gFileSysOpts.LargeReadSliceSize;
        if (NumBytesToRead > static_cast<unsigned int>(this->amount)) {
            NumBytesToRead = static_cast<unsigned int>(this->amount);
        }
        unsigned int DeviceOptimalReadSize = device->drv->GetOptimalReadSize();
        if (DeviceOptimalReadSize != 0) {
            if (NumBytesToRead > DeviceOptimalReadSize) {
                NumBytesToRead = DeviceOptimalReadSize;
            }
        }
        unsigned int nread = device->drv->Read(
            this->filehandle->hFile, this->data, NumBytesToRead,
            this->filehandle->pParentFile->dev->drv, this->filehandle->pParentFile->hFile);
        this->amount -= nread;
        this->value += nread;
        this->totalbytes += nread;
        this->data = static_cast<char *>(this->data) + nread;
        if (this->amount > 0 && nread == NumBytesToRead) {
            this->AddToQueue();
        } else {
            this->status = 1;
        }
    }
    virtual long long Complete() override {
        return this->totalbytes;
    }
};

int FILESYS_readlarge(int filehandle, unsigned long long offset, void *buffer,
                      unsigned long long bytes, int priority, void *userdata) {
    FILESYSHANDLE *hFile = reinterpret_cast<FILESYSHANDLE *>(filehandle);
    ReadLargeOperation *op =
        new ReadLargeOperation(hFile, bytes, offset, buffer, priority, userdata, hFile->dev);
    op->AddToQueue();
    return op->GetId();
}

struct WriteOperation : public FILEOPERATION {
    WriteOperation(FILESYSHANDLE *_filehandle, int bytes, int offset, void *buffer, int priority,
                   void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->filehandle = _filehandle;
        this->amount = bytes;
        this->value = offset;
        this->data = buffer;
    }

    virtual ~WriteOperation() override {}
    virtual void Exec(FILEDEVICE *device) override {
        if (this->value != this->filehandle->pParentFile->offset) {
            this->filehandle->pParentFile->offset =
                device->drv->Seek(this->filehandle->hFile, this->value, 0, nullptr, 0);
        }
        this->amount = device->drv->Write(
            this->filehandle->hFile, this->data, this->amount,
            nullptr, 0);
        this->filehandle->pParentFile->offset += this->amount;
        this->status = 1;
    }
    virtual long long Complete() override {
        return this->amount;
    }
};

int FILESYS_write(int filehandle, int offset, void *buffer, int bytes, int priority,
                  void *userdata) {
    FILESYSHANDLE *hFile = reinterpret_cast<FILESYSHANDLE *>(filehandle);
    WriteOperation *op =
        new WriteOperation(hFile, bytes, offset, buffer, priority, userdata, hFile->dev);
    op->AddToQueue();
    return op->GetId();
}

struct SizeOperation : public FILEOPERATION {
    SizeOperation(FILESYSHANDLE *_filehandle, int priority, void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        this->filehandle = _filehandle;
    }

    virtual ~SizeOperation() override {}
    virtual void Exec(FILEDEVICE *) override {
        this->value = this->filehandle->length;
        this->status = 1;
    }

    virtual long long Complete() override {
        return this->value;
    }
};

int FILESYS_size(int filehandle, int priority, void *userdata) {
    FILESYSHANDLE *hFile = reinterpret_cast<FILESYSHANDLE *>(filehandle);
    SizeOperation *op = new SizeOperation(hFile, priority, userdata, hFile->dev);
    op->AddToQueue();
    return op->GetId();
}

int FILESYS_atomic(FILESYS_ATOM func, FILEDEVICE *device, int priority, void *userdata) {
    int result;
    if (device->running == 0) {
        iStartDevice(device);
    }
    device->atomic.Lock();
    int oldminpriority = device->minpriority;
    device->minpriority = priority;
    result = func(priority, userdata);
    device->minpriority = oldminpriority;
    SIGNAL_set(&device->signal);
    device->atomic.Unlock();
    return result;
}

namespace RealFile {

bool GetInfoFastByName(const char *name, const unsigned int modeflags,
                       unsigned long long &location, unsigned long long &size) {
    FILESYSHANDLE *fs_h =
        iOpenFileSysHandle(name, modeflags | 0x20001, FILE_nametodevice(name));
    return fs_h == reinterpret_cast<FILESYSHANDLE *>(-1)
               ? (location = 0, size = 0, false)
               : (GetInfoFastByHandle(reinterpret_cast<int>(fs_h), location, size),
                  iCloseFileSysHandle(fs_h), true);
}

void GetInfoFastByHandle(int filehandle, unsigned long long &location,
                         unsigned long long &size) {
    FILESYSHANDLE *fs_h = reinterpret_cast<FILESYSHANDLE *>(filehandle);
    location = fs_h->dev->drv->QueryLocation(fs_h->hFile);
    size = fs_h->length;
}

} // namespace RealFile

FILEDEVICE *FILE_nametodevice(const char *name) {
    FILEDEVICE *device = nullptr;
    char devname[16];
    const char *fn;
    ListSingleIterator<FILEDEVICE> Itr;
    if (strchr(name, ':') != nullptr) {
        MEM_clear(devname, sizeof(devname));
        fn = strchr(name, ':');
        strncpy(devname, name, fn - name + 1);
    } else if (*name == '/' || *name == '\\') {
        MEM_clear(devname, sizeof(devname));
        fn = strchr(*gpFileSysInfo->psearchpaths, ':');
        strncpy(devname, *gpFileSysInfo->psearchpaths,
                fn - *gpFileSysInfo->psearchpaths + 1);
    } else {
        return gpFileSysInfo->DeviceMemory;
    }
    Itr = gpFileSysInfo->AllocatedDevices.Begin();
    while (*Itr != nullptr && device == nullptr) {
        if (strcasecmp((*Itr)->drv->GetName(), devname) == 0) {
            return device = *Itr;
        } else {
            Itr++;
        }
    }
    return device;
}

namespace RealFile {

void SetSearchPath(const char *path) {
    int i = 1;
    MEM_clear(gpFileSysInfo->searchpath, gFileSysOpts.nSearchPathLength);
    MEM_clear(gpFileSysInfo->psearchpaths,
              gFileSysOpts.nSearchLocs * sizeof(gpFileSysInfo->psearchpaths[0]));
    strcpy(gpFileSysInfo->searchpath, path);
    char *psp = gpFileSysInfo->searchpath;
    gpFileSysInfo->psearchpaths[0] = psp;
    while ((psp = strchr(psp, ';')) != nullptr) {
        *psp = '\0';
        if (psp[-1] == '/' || psp[-1] == '\\') {
            psp[-1] = '\0';
        }
        psp++;
        gpFileSysInfo->psearchpaths[i++] = psp;
    }
    psp = gpFileSysInfo->psearchpaths[i - 1];
    psp += strlen(psp);
    if (psp[-1] == '/' || psp[-1] == '\\') {
        psp[-1] = '\0';
    }
}

void AddSearchLocation(const char *pszLoc, bool head) {
    char *c = gpFileSysInfo->searchpath;
    while (c[1] != '\0') {
        while (*c != '\0') {
            c++;
        }
        if (c[1] == '\0') {
            if (c[2] != '\0') {
                *++c = '/';
            }
        }
        *c = ';';
    }
    *c = '\0';
    char *newpath = static_cast<char *>(__builtin_alloca(gFileSysOpts.nSearchPathLength));
    MEM_clear(newpath, gFileSysOpts.nSearchPathLength);
    if (head == true) {
        strcpy(newpath, pszLoc);
        strcat(newpath, ";");
        strcat(newpath, gpFileSysInfo->searchpath);
    } else {
        strcpy(newpath, gpFileSysInfo->searchpath);
        strcat(newpath, ";");
        strcat(newpath, pszLoc);
    }
    SetSearchPath(newpath);
}

unsigned int AddDevice(DeviceDriver *drv) {
    FILEDEVICE *dev = gpFileSysInfo->FreeDevices.Pop();
    dev->drv = drv;
    gpFileSysInfo->AllocatedDevices.PushTail(dev);
    return dev - gpFileSysInfo->DeviceMemory;
}

void RemoveDevice(unsigned int DevId) {
    FILEDEVICE *device;
    bool bFound;
    device = &gpFileSysInfo->DeviceMemory[DevId];
    bFound = gpFileSysInfo->AllocatedDevices.Remove(device, nullptr);
    if (bFound) {
        if (device->running != 0) {
            device->terminate = 1;
            SIGNAL_set(&device->signal);
            while (device->running != 0) {
                THREAD_yield(0);
            }
            SIGNAL_destroy(&device->signal);
            SIGNAL_set(&device->attention);
            SIGNAL_destroy(&device->attention);
            gFileSysOpts.allocator->Free(device->pThreadStack, 0);
            device->pThreadStack = nullptr;
            device->mutex.Destroy();
            device->atomic.Destroy();
            device->drv->Restore();
        }
        MEM_clear(device, sizeof(ListNodeS));
        gpFileSysInfo->FreeDevices.PushTail(device);
    }
}

} // namespace RealFile
