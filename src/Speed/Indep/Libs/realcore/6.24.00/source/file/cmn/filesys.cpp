#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/file/driver.h"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/std/list.h"
#include "Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/system.h"

#include <string.h>

#define ALIGN8(x) ((char *)(((unsigned int)(x) + 7) & ~7))

void MEM_clear(void *ptr, int size);
void MEM_copy(void *dst, const void *src, int size);

// total size: 0x28
struct SIGNAL {
    int reserved[10]; // offset 0x0, size 0x28
};

// total size: 0x318
struct THREAD {
    int reserved[198]; // offset 0x0, size 0x318
};

bool SIGNAL_create(SIGNAL *s);
bool THREAD_create(THREAD *t, int (*func)(void *), void *param, void *stack, int stacksize,
                   int priority);
void SIGNAL_set(SIGNAL *s);
void SIGNAL_wait(SIGNAL *s);
void SIGNAL_destroy(SIGNAL *s);

// total size: 0xC
struct TagValuePair {
    unsigned int mTag; // offset 0x0, size 0x4
    union {
        int mInt;             // offset 0x0, size 0x4
        unsigned int mSize;   // offset 0x0, size 0x4
        float mFloat;         // offset 0x0, size 0x4
        const void *mPointer; // offset 0x0, size 0x4
    } mValue;                 // offset 0x4, size 0x4
    mutable const TagValuePair *mNext; // offset 0x8, size 0x4

    TagValuePair(unsigned int tag, int value) {
        mTag = tag;
        mValue.mInt = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, unsigned int value) {
        mTag = tag;
        mValue.mSize = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, float value) {
        mTag = tag;
        mValue.mFloat = value;
        mNext = 0;
    }
    TagValuePair(unsigned int tag, const void *value) {
        mTag = tag;
        mValue.mPointer = value;
        mNext = 0;
    }

    const TagValuePair &operator+(const TagValuePair &rhs) const {
        rhs.mNext = this;
        return rhs;
    }
};

// total size: 0x4
struct IAllocator {
    virtual void *Alloc(unsigned int size, const TagValuePair &flags) = 0;
    virtual void Free(void *pBlock, unsigned int size) = 0;
    virtual int AddRef() = 0;
    virtual int Release() = 0;

  protected:
    virtual ~IAllocator() {}
};

struct FILESYSOPTS {
    int size;                                // +0x00
    IAllocator *allocator;                   // +0x04
    int MaxOpenFiles;                        // +0x08
    int MaxFileOps;                          // +0x0C
    int nSearchLocs;                         // +0x10
    int nSearchPathLength;                   // +0x14
    int MaxDevices;                          // +0x18
    int ThreadStackSize;                     // +0x1C
    int (*decompresssize)(const void *);     // +0x20
    int (*decompress)(const void *, void *); // +0x24
    unsigned int LargeReadSliceSize;         // +0x28
    unsigned int AllocAlignBoundary;         // +0x2C
    int DiscType;                            // +0x30
    int mErrorRetryCount;                    // +0x34
};

extern FILESYSOPTS gFileSysOpts;

struct FILEDEVICE;
struct FILESYSHANDLE;

// Relleno: el cuerpo de ~FILEOPERATION cae en la linea 71 y los accesores
// GetId/GetPriority/GetStatus/GetCallback/IsCancelled en 86..90.
//
//
//
//
// total size: 0x48
struct FILEOPERATION : public ListNodeS {
    virtual ~FILEOPERATION() {
    }

    virtual void Exec(FILEDEVICE *device) = 0;
    virtual long long Complete() = 0;
    virtual void Cancel(FILEDEVICE *device);

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    int GetId() {
        return id;
    }
    int GetPriority() {
        return priority;
    }
    int GetStatus() {
        return status;
    }
    void (*GetCallback())(int, int, void *) {
        return callback;
    }
    bool IsCancelled() {
        return cancelled == true;
    }
    void *GetUserData() {
        return userdata;
    }
    void ClearId() {
        id = 0;
    }
    void SetPriority(unsigned char _priority) {
        priority = _priority;
    }
    void SetCallback(void (*_callback)(int, int, void *)) {
        callback = _callback;
    }

    static FILEDEVICE *GetDevice(int ophandle);
    int iGetNextOpId(FILEDEVICE *dev);
    FILEOPERATION(int _priority, void *_userdata, FILEDEVICE *device);
    static void *operator new(unsigned int);
    void SetName(const char *name);
    void AddToQueue();

    int id;                              // offset 0x4, size 0x4
    bool cancelled;                      // offset 0x8, size 0x1
    signed char status;                  // offset 0xC, size 0x1
    unsigned char priority;              // offset 0xD, size 0x1
    FILESYSHANDLE *filehandle;           // offset 0x10, size 0x4
    void *userdata;                      // offset 0x14, size 0x4
    void (*callback)(int, int, void *);  // offset 0x18, size 0x4
    long long value;                     // offset 0x20, size 0x8
    long long amount;                    // offset 0x28, size 0x8
    void *data;                          // offset 0x30, size 0x4
    unsigned long long totalbytes;       // offset 0x38, size 0x8
};

// total size: 0xC
struct FileOperationQueue : public ListSingle<FILEOPERATION> {
    FileOperationQueue() {
    }

    FILEOPERATION *Find(int ophandle, bool bRemoveFromQueue);
};

// total size: 0x20
struct FILESYSHANDLE : public ListNodeS {
    FILESYSHANDLE *pParentFile; // offset 0x4, size 0x4
    int hFile;                  // offset 0x8, size 0x4
    FILEDEVICE *dev;            // offset 0xC, size 0x4
    long long length;           // offset 0x10, size 0x8
    long long offset;           // offset 0x18, size 0x8
};

// total size: 0x3D4
struct FILEDEVICE : public ListNodeS {
    volatile int running;           // offset 0x4, size 0x4
    volatile int terminate;         // offset 0x8, size 0x4
    FILEOPERATION *current;         // offset 0xC, size 0x4
    FileOperationQueue pending;     // offset 0x10, size 0xC
    FileOperationQueue completed;   // offset 0x1C, size 0xC
    RealSystem::Mutex mutex;        // offset 0x28, size 0x1C
    SIGNAL signal;                  // offset 0x44, size 0x28
    RealSystem::Mutex atomic;       // offset 0x6C, size 0x1C
    THREAD deviceThread;            // offset 0x88, size 0x318
    void *pThreadStack;             // offset 0x3A0, size 0x4
    SIGNAL attention;               // offset 0x3A4, size 0x28
    int minpriority;                // offset 0x3CC, size 0x4
    RealFile::DeviceDriver *drv;    // offset 0x3D0, size 0x4
};

// total size: 0x9C
struct FILESYSINFO {
    FILESYSINFO() {
    }
    ~FILESYSINFO();

    ListSingle<FILEOPERATION> FreeOps;             // offset 0x0, size 0xC
    RealSystem::Mutex FreeOpsMutex;                // offset 0xC, size 0x1C
    ListSingle<FILESYSHANDLE> FreeFileSysHandles;  // offset 0x28, size 0xC
    RealSystem::Mutex FreeFileSysHandlesMutex;     // offset 0x34, size 0x1C
    int NextOp;                                    // offset 0x50, size 0x4
    RealSystem::Mutex NextOpMutex;                 // offset 0x54, size 0x1C
    ListSingle<FILEDEVICE> FreeDevices;            // offset 0x70, size 0xC
    ListSingle<FILEDEVICE> AllocatedDevices;       // offset 0x7C, size 0xC
    char **psearchpaths;                           // offset 0x88, size 0x4
    char *searchpath;                              // offset 0x8C, size 0x4
    FILEOPERATION *OpMemory;                       // offset 0x90, size 0x4
    FILESYSHANDLE *FileSysHandleMemory;            // offset 0x94, size 0x4
    FILEDEVICE *DeviceMemory;                      // offset 0x98, size 0x4
};

static FILESYSINFO *gpFileSysInfo;
static void *pDeviceMem = 0;

static void iStartDevice(FILEDEVICE *dev);
static FILESYSHANDLE *iAllocateFileSysHandle();
static void iFreeFileSysHandle(FILESYSHANDLE *fs_h);
static FILESYSHANDLE *iOpenFileSysHandle(const char *filename, int modeflags, FILEDEVICE *device);
static void iCloseFileSysHandle(FILESYSHANDLE *h);
static FILEOPERATION *iGetOpFromHandle(FILEDEVICE *device, int ophandle, int *ispending);
static int iDeviceCommandProcessorThreadFunc(void *param);

extern int bIsFileSysInitialized;

FILEDEVICE *FILE_nametodevice(const char *name);
int FILE_overhead();
void FILE_restore();
extern RealFile::DeviceDriver *libdevice[2];
void SYSTEM_addexit(void (*func)());

inline void *operator new(unsigned int, void *p, unsigned int) throw() {
    return p;
}
int THREAD_iscurrent(THREAD *t);
void THREAD_yield(int ms);
void SYNCTASK_run();

RealFile::DeviceDriver::DeviceDriver(const char *name) {
    strcpy(mDeviceName, name);
}

// total size: 0x14
class NullFileDriver : public RealFile::DeviceDriver {
  public:
    NullFileDriver(const char *pName) : DeviceDriver(pName) {}

    virtual EAFileHandle Open(const char *, int, int *) {
        return -1;
    }
    virtual void Close(EAFileHandle) {}
    virtual uint32_t Read(EAFileHandle, void *, unsigned int, DeviceDriver *, EAFileHandle) {
        return 0;
    }
    virtual uint64_t Seek(EAFileHandle, uint64_t, int, DeviceDriver *, EAFileHandle) {
        return 0;
    }
    virtual uint64_t Getsize(EAFileHandle) {
        return 0;
    }
};

static NullFileDriver nulldrv("null:");

static void iDefaultFilesysCallbackFunc(int, int, void *) {
}

FILEDEVICE *FILEOPERATION::GetDevice(int ophandle) {
    return &gpFileSysInfo->DeviceMemory[(unsigned char)ophandle];
}

FILEOPERATION *FileOperationQueue::Find(int ophandle, bool bRemoveFromQueue) {
    ListSingleIterator<FILEOPERATION> Itr;

    Itr = this->Begin();
    if (*Itr != 0) {
        do {
            if ((*Itr)->GetId() == ophandle) {
                if (bRemoveFromQueue) {
                    // Keep the queue's removal inline, including its empty-list case.
                    FILEOPERATION *node = *Itr;
                    FILEOPERATION *p;
                    bool foundit = false;
                    if (node == this->head) {
                        foundit = true;
                        this->nNodes--;
                        if (node == this->tail) {
                            this->head = 0;
                            this->tail = 0;
                        } else {
                            this->head = (FILEOPERATION *)node->next;
                        }
                    } else if (this->head != 0) {
                        p = this->head;
                        while (p->next != 0 && p->next != node) {
                            p = (FILEOPERATION *)p->next;
                        }
                        if (p->next != 0 && p->next == node) {
                            foundit = true;
                            this->nNodes--;
                            p->next = node->next;
                            if (node == this->tail) {
                                this->tail = p;
                            }
                        }
                    }
                    if (foundit) {
                        node->next = 0;
                    }
                }

                return *Itr;
            }
            Itr++;
        } while (*Itr != 0);
    }

    return 0;
}

static void iStartDevice(FILEDEVICE *dev) {
    gpFileSysInfo->FreeOpsMutex.Lock();

    dev->mutex.Create();

    dev->current = 0;

    SIGNAL_create(&dev->signal);

    //
    //
    SIGNAL_create(&dev->attention);

    dev->atomic.Create();
    dev->minpriority = 255;

    dev->drv->Init();

    //
    //
    //
    //
    //
    //
    //
    //
    //
    dev->pThreadStack = gFileSysOpts.allocator->Alloc(
        gFileSysOpts.ThreadStackSize,
        TagValuePair(4, 1) + TagValuePair(2, 32u) +
            TagValuePair(1, (const void *)"File System") +
            TagValuePair(5, (const void *)"d:/packages/realcore/6.24.00/source/file/cmn/filesys.cpp") +
            TagValuePair(6, 312));

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    THREAD_create(&dev->deviceThread, iDeviceCommandProcessorThreadFunc, dev, dev->pThreadStack,
                  gFileSysOpts.ThreadStackSize, 1);

    //
    //
    //
    //
    //
    //
    //
    SIGNAL_wait(&dev->attention);

    //
    //
    gpFileSysInfo->FreeOpsMutex.Unlock();
}

int FILEOPERATION::iGetNextOpId(FILEDEVICE *dev) {
    int op;

    gpFileSysInfo->NextOpMutex.Lock();

    op = gpFileSysInfo->NextOp;
    gpFileSysInfo->NextOp = op + 1;

    //
    //
    //
    //
    //
    if (gpFileSysInfo->NextOp == 0x10000) {
        gpFileSysInfo->NextOp = 1;
    }

    gpFileSysInfo->NextOpMutex.Unlock();

    return (op << 8) | (dev - gpFileSysInfo->DeviceMemory);
}

FILEOPERATION::FILEOPERATION(int _priority, void *_userdata, FILEDEVICE *device) {
    //
    //
    //
    //
    //
    //
    if (device->running == 0) {
        iStartDevice(device);
    }

    //
    priority = _priority;
    cancelled = false;
    status = 0;
    filehandle = (FILESYSHANDLE *)-1;
    userdata = _userdata;
    callback = iDefaultFilesysCallbackFunc;
    value = 0;
    amount = 0;
    data = 0;

    id = iGetNextOpId(device);
}

void *FILEOPERATION::operator new(unsigned int) {
    FILEOPERATION *op;

    gpFileSysInfo->FreeOpsMutex.Lock();

    op = gpFileSysInfo->FreeOps.Pop();

    gpFileSysInfo->FreeOpsMutex.Unlock();

    //
    //
    //
    //
    return op;
}

void FILEOPERATION::SetName(const char *name) {
    int len;

    //
    //
    //
    if (name == 0) {
        name = "";
    }
    len = strlen(name) + 1;

    //
    //
    //
    //
    //
    //
    data = gFileSysOpts.allocator->Alloc(
        len, TagValuePair(4, 1) + TagValuePair(1, (const void *)name) +
                 TagValuePair(5, (const void *)"d:/packages/realcore/6.24.00/source/file/cmn/filesys.cpp") +
                 TagValuePair(6, 432));
    MEM_copy(data, name, len);
}

void FILEOPERATION::Cancel(FILEDEVICE *device) {
    cancelled = true;

    //
    //
    if (device->pending.Remove(this, 0)) {

        device->completed.Push(this);

        //
        callback(id, -1, userdata);
    }
}

static FILESYSHANDLE *iAllocateFileSysHandle() {
    FILESYSHANDLE *fs_h;

    gpFileSysInfo->FreeFileSysHandlesMutex.Lock();
    fs_h = gpFileSysInfo->FreeFileSysHandles.Pop();
    gpFileSysInfo->FreeFileSysHandlesMutex.Unlock();

    //
    //
    //
    //
    fs_h->hFile = -1;

    return fs_h;
}

static void iFreeFileSysHandle(FILESYSHANDLE *fs_h) {
    //
    //
    MEM_clear(fs_h, sizeof(FILESYSHANDLE));
    //
    gpFileSysInfo->FreeFileSysHandlesMutex.Lock();
    gpFileSysInfo->FreeFileSysHandles.Push(fs_h);
    gpFileSysInfo->FreeFileSysHandlesMutex.Unlock();
}

static FILESYSHANDLE *iOpenFileSysHandle(const char *filename, int modeflags,
                                         FILEDEVICE *device) {
    int ret;
    int hParentFile;
    FILESYSHANDLE *h;
    char newname[256];
    int i;
    char *searchpath;

    ret = -1;
    hParentFile = -1;

    if (device == gpFileSysInfo->DeviceMemory) {

        //
        //
        //
        //
        for (i = 0; i < gFileSysOpts.nSearchLocs; i++) {

            searchpath = gpFileSysInfo->psearchpaths[i];

            //
            //
            //
            if (searchpath == 0) {
                break;
            }

            //
            //
            //
            //
            device = FILE_nametodevice(searchpath);

            //
            //
            //
            //
            //
            //
            if (device->running == 0) {
                iStartDevice(device);
            }

            //
            //
            //
            strcpy(newname, searchpath);
            strcat(newname, "/");
            strcat(newname, filename);

            //
            ret = device->drv->Open(newname, modeflags, &hParentFile);

            //
            //
            if (ret != -1) {
                break;
            }
        }

    } else {

        //
        //
        ret = device->drv->Open(filename, modeflags, &hParentFile);
    }

    //
    //
    if (ret != -1) {

        //
        //
        //
        if (modeflags & 0x10000) {

            //
            device->drv->Close(ret);

            //
            return (FILESYSHANDLE *)1;
        }

        //
        //
        h = iAllocateFileSysHandle();

        //
        h->hFile = ret;
        h->dev = device;
        h->length = device->drv->Getsize(ret);
        h->pParentFile = h;

        if (hParentFile != -1) {

            //
            //
            //
            //
            h->pParentFile = (FILESYSHANDLE *)hParentFile;
        }

        //
        //
        return h;
    }

    //
    //
    return (FILESYSHANDLE *)-1;
}

static void iCloseFileSysHandle(FILESYSHANDLE *h) {
    h->dev->drv->Close(h->hFile);

    iFreeFileSysHandle(h);
}

static FILEOPERATION *iGetOpFromHandle(FILEDEVICE *device, int ophandle, int *ispending) {
    FILEOPERATION *op = 0;

    *ispending = 0;

    if (device->running != 0) {

        op = device->current;

        if (op != 0 && op->GetId() == ophandle) {

            *ispending = -1;

        } else {

            op = device->pending.Find(ophandle, false);

            if (op != 0) {

                *ispending = 1;

            } else {

                op = device->completed.Find(ophandle, false);
            }
        }
    }

    return op;
}

void FILEOPERATION::AddToQueue() {
    FILEDEVICE *device = GetDevice(id);
    FILEOPERATION *current;
    FILEOPERATION *prev;
    unsigned int newoppriority;

    device->mutex.Lock();

    //
    prev = 0;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    // r65 IRREDUCIBLE, con DIAGNOSTICO. `regmap filesys "FILEOPERATION::AddToQueue"`
    // dice ESTRUCTURA: el original NO tiene la local `newid`, y su `curpriority`
    // (r9) vive en un BLOQUE ANONIMO que aqui no existe --es la prioridad del nodo
    // que recorre el bucle, no la del nuestro--. Locales del original: device r30,
    // Itr/current r11, prev r10, newoppriority r8, y b0{curpriority r9}.
    // MEDIDO r65: escrita la forma NATURAL --sin `newpriority`, sin `newid` y sin
    // ninguna de las tres barreras, `newoppriority = (GetPriority() << 24) |
    // (GetId() & 0x00FFFF00);`-- salen 252 B EXACTOS y 96,83 %, y queda UNA sola
    // fila: el `slwi r9,r9,24` dos ranuras antes que en el objetivo, que mete en
    // medio el `lwz` del Head(). Poner el Head() antes o despues de esa sentencia
    // da EL MISMO objeto (sello 0xe1474c34): el orden de sentencias no manda aqui.
    // NEGATIVO r65, sello ALLOC de cada intento (base 0x4a26cc84):
    //   las tres barreras, una a una      -> 0x549c4511 / 0xe1474c34 / 0xaede5c88
    //   forma natural (0 locales, 0 barreras) -> 0xe1474c34
    //   1 barrera sobre newpriority entre Head() y el desplazamiento -> 0xc0de70ba
    //   1 barrera "+m"(pending) con "r"(GetId()) sin local newid     -> 0x8c95d43d
    //   quitar SOLO la local newid (barreras intactas)               -> 0xaede5c88
    // O sea: las cinco construcciones son load-bearing y no se pueden reducir a
    // menos sin perder el objeto. Las dos del bucle (`current`) tambien: quitarlas
    // da 0xd8fa5d47 y 0x82c93e14 (esta ultima mueve hasta la tabla de simbolos).
    // r37: orden del objetivo, sin instrucciones adicionales ni registros fijos:
    // id -> Head() -> desplazar prioridad -> enmascarar id. Las dependencias
    // vacias conservan los valores reales y limitan solo esa planificacion.
    // r67 IRREDUCIBLE, PALANCA (a) MEDIDA Y NEGATIVA. El bloque anonimo con
    // `curpriority` que describe el DWARF (en la prueba del bucle, abajo) es
    // NEUTRO: con las cinco barreras da el objeto identico, y sobre la forma
    // natural de la r65 da exactamente el mismo objeto que sin el. Formas r67
    // (sello ALLOC, base de59e5dfaf2a9107):
    //   natural (0 locales, 0 barreras aqui)                            -> f1a1744d
    //   natural + bloque `if (current) { unsigned int curpriority = ...;
    //     if (newoppriority >= curpriority) goto ...; }`, con el bloque
    //     como `{ unsigned int curpriority; if (... = ...)}` o con la
    //     salida invertida (`< curpriority` -> goto done)                 -> f1a1744d
    //   el bloque sin la barrera del incremento / sin la del if          -> 6a34393e / 0c41f8cf
    //   operandos del `|` al reves `(GetId() & m) | (GetPriority() << 24)` -> 0ae531bf
    //   en dos sentencias `= prio << 24; |= id & m;` y al reves           -> 6b48d223 / 8b0bfa10
    //   con casts `(unsigned int)` en los dos operandos                   -> f1a1744d
    //   cada una con el Head() detras o con `prev = 0` detras del Head()  -> el mismo de su forma
    // El orden de las sentencias y el ambito de la prioridad no llegan al empate de
    // sched1 del `slwi`; lo unico que lo mueve siguen siendo las dependencias.
    // scaf-r1 VEDA, MECANISMO PROBADO DESDE EL COMPILADOR. Leido el scheduler
    // (orig/prodg/NGC_GNU_SRC/NGC/gcc/haifa-sched.c) y volcada la traza
    // (`rtldump filesys AddToQueue -dR`): priority() SALTA los dependientes de
    // otro bloque ("critical path is meaningful in block boundaries only"), y
    // el UNICO consumidor de la carga del Head() vive en el bloque siguiente
    // (la prueba del bucle). Con eso su INSN_PRIORITY es solo el coste del lwz
    // (2) y pierde SIEMPRE contra el `slwi` (3: slwi->or->b, rama anadida por
    // add_branch_dependences). Traza de la forma natural, bloque 0: el lwz del
    // Head esta listo en t=8 y aun asi pierde la lsu contra el lbz (t=8) y el
    // lwz del id (t=9); emite en t=10 por la lsu MIENTRAS el slwi toma la iu
    // DEL MISMO ciclo y se appenda PRIMERO en el flujo. El objetivo los tiene
    // al reves: solo una dependencia artificial (la cadena r37: id -> "+m"
    // pending -> Head) sube su prioridad por encima del slwi. Ninguna fuente
    // puede darle un consumidor DENTRO del bloque: la prueba vive en otro
    // bloque y cualquier uso de `current` aqui, o sobra (lo borran) o anade
    // instruccion. Es estructura del CFG, no forma de sentencia.
    // Negativos scaf-r1 (sello .text/.rodata del .o; objetivo 5fd57c16/74762f2b):
    //   natural r65 reproducida (sin locales, sin barreras de cabecera)   -> 31bb3e15, 96,83 %, 1 fila (el swap)
    //   con locales newpriority/newid, sin barreras                       -> 31bb3e15 (las locales no pesan)
    //   miembros directos `priority`/`id` (el DWARF original NO tiene los
    //     inlines GetPriority/GetId en la cabecera: accedia directo)      -> 31bb3e15
    //   `*(volatile unsigned char *)&priority << 24`                      -> 31bb3e15 (volatil de CARGA: efecto CERO,
    //                                                                        este scheduler solo obedece almacenes volatiles -- ver pathbank)
    //   `*(volatile int *)&id & 0x00FFFF00`                               -> 31bb3e15
    //   `*(FILEOPERATION * volatile *)&device->pending.head`              -> 31bb3e15
    //   `Itr = device->pending.Begin(); current = *Itr;` (la forma del
    //     DWARF: Itr/Begin/operator* en el arbol)                        -> 31bb3e15
    //   `(id & 0x00FFFF00) | (priority << 24)` (r67 midio 0ae531bf:
    //     ademas cambia el reparto, 91,67 %)                             -> a0ee3afa, 91,67 %
    // Con r65+r66+r67+scaf-r1 son ~23 formas, 24 flags y el mecanismo cerrado.
    unsigned int newpriority = GetPriority();
    int newid = GetId();
    current = device->pending.Head();
    newpriority <<= 24;
    newoppriority = newpriority | (newid & 0x00FFFF00);

    // El volcado DWARF del original dice `ListSingleIterator<FILEOPERATION> Itr; // r11`
    // con un bloque anonimo `{ unsigned int curpriority; // r9 }` y el `operator++`
    // FUERA de ese bloque, o sea un `for (Itr = Begin(); *Itr != 0; Itr++)`. Ese `for`
    // no cierra: g++ 2.9 no hace aqui el "roll" de la condicion de entrada al final
    // (83,65 %) y con `goto` el iterador se va a la pila (90,14 %). La forma de goto
    // con puntero pelado da la disposicion exacta del objetivo (95,16 %) y solo falta
    // el `mr.`+`beq` del `if (node != 0)` de `operator++`, que aqui GCC pliega porque
    // sabe que `current != 0` al entrar por `goto nextOperation`.
    // r32: leido el flujo del objetivo entero. Es un `for` sin rotacion:
    //   0x44  b TEST            <- entrada
    //   0x48  INCR: mr. r10,r11 <- prev = current, Y PRUEBA current
    //   0x4C        beq TEST
    //   0x50        lwz r11,0(r10)
    //   0x54  TEST: cmpwi r11,0 / beq SALIDA ... / bge INCR
    // O sea que el objetivo conserva una prueba de nulo REDUNDANTE: el TEST
    // del bucle repite exactamente lo que acaba de mirar el `beq` del
    // incremento. Es el `if (node != 0)` del `operator++` que el DWARF nombra.
    // Nosotros la plegamos porque al llegar a INCR por la arista de retorno
    // GCC sabe que current != 0. Son los 4 B (una instruccion) que faltan.
    //
    // OCHO formas barridas, ninguna la trae: barrera total detras y delante de
    // la copia (identicas), barrera selectiva sobre prev (256 B), expresion
    // condicional, probar current en vez de prev, while con la prueba abajo,
    // goto interno en vez de if (todas 95,1587 % / 248 B) y puntero volatil
    // (88,79 % / 268 B).
    //
    // Y EL EJE DEL ITERADOR, cerrado con cinco formas propias (r32). El arbol
    // ya trae `ListSingleIterator<T>` con justo la prueba que falta:
    //     void operator++(int) { if (node != 0) node = (T *)node->next; }
    // Escrita la forma que describe el DWARF --Itr en la funcion, curpriority
    // en un bloque anonimo, el ++ fuera-- y cuatro vecinas (prev fuera del
    // cuerpo, prioridad en la condicion, while con Itr++ al final, temporal
    // del nodo): las CINCO dan 83,65 % / 248 B, salvo la de prev fuera, 83,57
    // y 240 B. Reproduce la cifra que ya midio la ronda anterior. El iterador
    // trae la prueba pero pierde mucho mas por otro lado; la forma de goto con
    // puntero pelado sigue siendo la mejor con diferencia.
    //
    // Y la bandera VETADA CON CIFRA: `-fno-cse-follow-jumps` da 252 B --el
    // tamano exacto-- y 96,83 %, pero barrida en las 42 unidades de realcore
    // PIERDE 14 funciones y no gana ninguna, incluida esta unidad (73/74 ->
    // 69/74). El tamano exacto era un senuelo.
    //
    // r36e: TRAIDA. 248 -> 252 B, el tamano exacto, y de 4 filas a 2. Hacian
    // falta TRES cosas a la vez, y ninguna sirve sola --por eso las ocho
    // formas anteriores se quedaron cortas--:
    //   1. probar `current` y no `prev`, para que el objetivo pueda fundir la
    //      copia y la comparacion en el `mr.` con forma de registro;
    //   2. la barrera selectiva sobre `current` al FINAL del cuerpo, antes
    //      del `goto`, que es lo que impide plegar la prueba de nulo. Puesta
    //      detras de la copia trae la prueba pero rompe la fusion, y salen
    //      `mr` + `cmpwi` + `beq`: 256 B. Esa era la medida de la veda;
    //   3. una segunda barrera detras del `if`, que cierra la ultima arista:
    //      cuando `current == 0` el objetivo vuelve a la prueba del bucle y
    //      nosotros saltabamos directo a la salida (jump threading).
    //
    // En r36e quedaban DOS filas de planificacion: el `slwi r9,r9,24` salia
    // una ranura antes que en el objetivo, que mete en medio el `lwz` del
    // Head(). Barrido sin exito: mover la sentencia del Head arriba, barrera
    // sobre newoppriority, prioridad partida en dos sentencias, barrera de
    // ranura tras el Head (con y sin clobber de r0), el operando de entrada
    // de la r36d-ai, y sacar la prioridad y/o el id a temporales para cambiar
    // el orden inicial que ve rank_for_schedule. `-fno-thread-jumps` tampoco
    // mueve nada. r37 cierra esas dos filas con la cadena de dependencias de
    // arriba: 252 B exactos y las 74 funciones de filesys al 100 %. Ninguna
    // de las tres dependencias aislada bastaba; no cambia la logica de la cola.
    goto checkPriority;
nextOperation:
    prev = current;
    if (current != 0) {
        current = (FILEOPERATION *)current->next;
    }
checkPriority:
    if (current != 0 &&
        newoppriority >=
            (unsigned int)((current->GetPriority() << 24) | (current->GetId() & 0x00FFFF00))) {
        goto nextOperation;
    }

    device->pending.InsertAfter(prev, this);

    device->mutex.Unlock();
    SIGNAL_set(&device->signal);
}

static int iDeviceCommandProcessorThreadFunc(void *param) {
    FILEDEVICE *dev = (FILEDEVICE *)param;
    FILEOPERATION *op;

    dev->running = 1;

    SIGNAL_set(&dev->attention);

    while (dev->terminate == 0) {

        dev->mutex.Lock();

        dev->current = 0;

        op = dev->pending.Head();
        if (op != 0 && op->GetPriority() <= dev->minpriority) {

            dev->pending.Pop();

            dev->current = op;
        }

        dev->mutex.Unlock();

        if (dev->current == 0) {

            SIGNAL_wait(&dev->signal);

        } else {

            if (op->IsCancelled() != true) {

                op->Exec(dev);
            }

            if (op->GetStatus() != 0) {

                dev->mutex.Lock();

                dev->completed.Push(op);

                dev->current = 0;

                dev->mutex.Unlock();

                op->GetCallback()(op->GetId(),
                                  op->IsCancelled() ? -1 : op->GetStatus(),
                                  op->GetUserData());

                SIGNAL_set(&dev->attention);
            }
        }
    }

    dev->running = 0;

    return 0;
}

bool FILE_init(void *buf, int bufsize) {
    int i;
    int nDevices;
    FILEOPERATION *op;
    FILESYSHANDLE *h;
    FILEDEVICE *dev;
    FILEDEVICE *device;

    //
    //
    //
    //
    //
    //
    //
    if (buf == 0) {

        bufsize = FILE_overhead();

        pDeviceMem = gFileSysOpts.allocator->Alloc(bufsize,
                                            TagValuePair(1, (const void *)"File System"));
        buf = pDeviceMem;
    }

    //
    //
    //
    //
    //
    MEM_clear(buf, bufsize);

    gpFileSysInfo = new (buf, bufsize) FILESYSINFO;

    gpFileSysInfo->psearchpaths = (char **)(gpFileSysInfo + 1);

    gpFileSysInfo->searchpath =
        (char *)gpFileSysInfo->psearchpaths + gFileSysOpts.nSearchLocs * 4;

    gpFileSysInfo->OpMemory =
        (FILEOPERATION *)ALIGN8(gpFileSysInfo->searchpath + gFileSysOpts.nSearchPathLength);

    gpFileSysInfo->FileSysHandleMemory =
        (FILESYSHANDLE *)(gFileSysOpts.MaxFileOps * sizeof(FILEOPERATION) +
                          (unsigned int)gpFileSysInfo->OpMemory);

    gpFileSysInfo->DeviceMemory =
        (FILEDEVICE *)(gpFileSysInfo->FileSysHandleMemory + gFileSysOpts.MaxOpenFiles);

    //
    //
    gpFileSysInfo->FreeOpsMutex.Create();
    gpFileSysInfo->FreeFileSysHandlesMutex.Create();

    //
    op = gpFileSysInfo->OpMemory;
    //
    //
    for (i = 0; i < gFileSysOpts.MaxFileOps; i++) {

        gpFileSysInfo->FreeOps.PushTail(op);
        op++;
    }
    //
    //
    h = gpFileSysInfo->FileSysHandleMemory;
    for (i = 0; i < gFileSysOpts.MaxOpenFiles; i++) {

        gpFileSysInfo->FreeFileSysHandles.PushTail(h);
        h++;
    }
    //
    //
    dev = gpFileSysInfo->DeviceMemory;
    for (i = 0; i < gFileSysOpts.MaxDevices; i++) {

        gpFileSysInfo->FreeDevices.PushTail(dev);
        dev++;
    }
    //
    //
    gpFileSysInfo->NextOp = 1;
    gpFileSysInfo->NextOpMutex.Create();

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    RealFile::AddDevice(&nulldrv);

    //
    //
    //
    //
    //
    nDevices = sizeof(libdevice) / sizeof(libdevice[0]);
    for (i = 0; i < nDevices; i++) {
        RealFile::AddDevice(libdevice[i]);
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    RealFile::SetSearchPath(gFileSysOpts.DiscType == 1 ? "dvd:" : "hd:");

    //
    //
    //
    //
    iStartDevice(FILE_nametodevice(gpFileSysInfo->psearchpaths[0]));

    //
    SYSTEM_addexit(FILE_restore);

    //
    bIsFileSysInitialized = 1;

    //
    return true;
}

int FILE_overhead() {
    int operations = sizeof(FILESYSINFO) + gFileSysOpts.MaxFileOps * sizeof(FILEOPERATION);
    int handles = gFileSysOpts.MaxOpenFiles * sizeof(FILESYSHANDLE);
    handles += operations;
    handles += gFileSysOpts.MaxDevices * (sizeof(FILEDEVICE) + 1);
    handles += gFileSysOpts.nSearchLocs * 4;
    handles += gFileSysOpts.nSearchPathLength;
    return handles + 24;
}

void FILE_restore() {
    int i;

    //
    //
    //
    for (i = 0; i < gFileSysOpts.MaxDevices; i++) {

        RealFile::RemoveDevice(i);
    }

    gpFileSysInfo->NextOpMutex.Destroy();

    //
    //
    gpFileSysInfo->FreeFileSysHandlesMutex.Destroy();
    gpFileSysInfo->FreeOpsMutex.Destroy();

    //
    //
    //
    //
    //
    //
    //
    gpFileSysInfo->FILESYSINFO::~FILESYSINFO();
    gpFileSysInfo = 0;

    if (pDeviceMem != 0) {

        //
        gFileSysOpts.allocator->Free(pDeviceMem, 0);
        pDeviceMem = 0;
    }

    //
    //
    bIsFileSysInitialized = 0;
}

inline FILESYSINFO::~FILESYSINFO() {
}

// total size: 0x48
struct ExistOperation : public FILEOPERATION {
    ExistOperation(const char *name, int priority, void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        SetName(name);
    }

    virtual void Exec(FILEDEVICE *device) {
        value = (iOpenFileSysHandle((const char *)data, 0x30001, device) != (FILESYSHANDLE *)-1);
        status = 1;
    }

    virtual long long Complete() {
        gFileSysOpts.allocator->Free(data, 0);
        return value;
    }
};

// total size: 0x48
struct OpenOperation : public FILEOPERATION {
    OpenOperation(const char *name, unsigned int modeflags, int priority, void *userdata,
                  FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        SetName(name);
        value = modeflags;
    }

    virtual void Exec(FILEDEVICE *device) {
        filehandle = iOpenFileSysHandle((const char *)data, (int)value, device);

        if (filehandle != (FILESYSHANDLE *)-1) {
            status = 1;
        } else {
            status = -2;
        }
    }

    virtual long long Complete() {
        gFileSysOpts.allocator->Free(data, 0);
        if (IsCancelled() == true && filehandle != (FILESYSHANDLE *)-1) {

            iCloseFileSysHandle(filehandle);

        } else if (filehandle != (FILESYSHANDLE *)-1) {

            return (int)filehandle;
        }

        return 0;
    }
};

// total size: 0x48
struct CloseOperation : public FILEOPERATION {
    CloseOperation(FILESYSHANDLE *_filehandle, int priority, void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        filehandle = _filehandle;
    }

    virtual void Exec(FILEDEVICE *device) {
        status = 1;
    }

    virtual long long Complete() {
        iCloseFileSysHandle(filehandle);
        return 1;
    }

    virtual void Cancel(FILEDEVICE *device) {
    }
};

// total size: 0x48
struct ReadOperation : public FILEOPERATION {
    ReadOperation(FILESYSHANDLE *_filehandle, int bytes, int offset, void *buffer, int priority,
                  void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        filehandle = _filehandle;
        amount = bytes;
        value = offset;
        data = buffer;
        totalbytes = 0;
    }

    virtual void Exec(FILEDEVICE *device) {
        unsigned int readsize;
        unsigned int bytesread;
        unsigned int optimal;

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        filehandle->pParentFile->offset =
            device->drv->Seek(filehandle->hFile, value, 0, filehandle->pParentFile->dev->drv,
                              filehandle->pParentFile->hFile);

        readsize = amount;

        //
        //
        optimal = device->drv->GetOptimalReadSize();

        if (optimal != 0 && readsize > optimal) {

            readsize = optimal;
        }

        //
        //
        //
        bytesread =
            device->drv->Read(filehandle->hFile, data, readsize,
                              filehandle->pParentFile->dev->drv, filehandle->pParentFile->hFile);

        //
        //
        //
        amount -= bytesread;
        value += bytesread;
        totalbytes += bytesread;
        data = (char *)data + bytesread;

        //
        //
        //
        //
        //
        if (amount > 0 && bytesread == readsize) {
            AddToQueue();
        } else {
            status = 1;
        }
    }

    virtual long long Complete() {
        return totalbytes;
    }
};

// total size: 0x48
struct ReadLargeOperation : public FILEOPERATION {
    ReadLargeOperation(FILESYSHANDLE *_filehandle, unsigned long long bytes,
                       unsigned long long offset, void *buffer, int priority, void *userdata,
                       FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        filehandle = _filehandle;
        amount = bytes;
        value = offset;
        data = buffer;
        totalbytes = 0;
    }

    virtual void Exec(FILEDEVICE *device) {
        unsigned int readsize;
        unsigned int bytesread;
        unsigned int optimal;

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        filehandle->pParentFile->offset =
            device->drv->Seek(filehandle->hFile, value, 0, filehandle->pParentFile->dev->drv,
                              filehandle->pParentFile->hFile);

        readsize = gFileSysOpts.LargeReadSliceSize;
        if (readsize > (unsigned int)amount) {
            readsize = (unsigned int)amount;
        }

        optimal = device->drv->GetOptimalReadSize();

        if (optimal != 0 && readsize > optimal) {

            readsize = optimal;
        }

        //
        //
        //
        bytesread =
            device->drv->Read(filehandle->hFile, data, readsize,
                              filehandle->pParentFile->dev->drv, filehandle->pParentFile->hFile);

        //
        //
        //
        amount -= bytesread;
        value += bytesread;
        totalbytes += bytesread;
        data = (char *)data + bytesread;

        //
        //
        //
        //
        //
        if (amount > 0 && bytesread == readsize) {
            AddToQueue();
        } else {
            status = 1;
        }
    }

    virtual long long Complete() {
        return totalbytes;
    }
};

// total size: 0x48
struct WriteOperation : public FILEOPERATION {
    WriteOperation(FILESYSHANDLE *_filehandle, int bytes, int offset, void *buffer, int priority,
                   void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        filehandle = _filehandle;
        amount = bytes;
        value = offset;
        data = buffer;
    }

    virtual void Exec(FILEDEVICE *device) {
        unsigned int byteswritten;

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        if (value != filehandle->pParentFile->offset) {

            filehandle->pParentFile->offset =
                device->drv->Seek(filehandle->hFile, value, 0, 0, 0);
        }

        //
        //
        //
        //
        byteswritten = device->drv->Write(filehandle->hFile, data, amount, 0, 0);
        amount = byteswritten;
        filehandle->pParentFile->offset += byteswritten;
        status = 1;
    }

    virtual long long Complete() {
        return amount;
    }
};

// total size: 0x48
struct SizeOperation : public FILEOPERATION {
    SizeOperation(FILESYSHANDLE *_filehandle, int priority, void *userdata, FILEDEVICE *dev)
        : FILEOPERATION(priority, userdata, dev) {
        filehandle = _filehandle;
    }

    virtual void Exec(FILEDEVICE *device) {
        value = filehandle->length;
        status = 1;
    }

    virtual long long Complete() {
        return value;
    }
};

int FILESYS_opstatus(int ophandle) {
    int status;
    FILEDEVICE *dev;
    FILEOPERATION *op;
    int ispending;

    status = 0;
    dev = FILEOPERATION::GetDevice(ophandle);

    dev->mutex.Lock();

    op = iGetOpFromHandle(dev, ophandle, &ispending);

    if (ispending == 0) {

        if (op != 0) {

            if (op->IsCancelled() == true) {
                status = -1;
            } else {
                status = op->GetStatus();
            }

        } else {
            status = -3;
        }
    }

    dev->mutex.Unlock();
    return status;
}

int FILESYS_waitop(int ophandle) {
    FILEDEVICE *dev;
    FILEOPERATION *op;
    int ispending;

    if (ophandle == 0) {
        return -3;
    }

    dev = FILEOPERATION::GetDevice(ophandle);

    //
    if (dev->running == 0) {
        return -3;
    }

    do {
        //
        //
        dev->mutex.Lock();
        //
        op = iGetOpFromHandle(dev, ophandle, &ispending);
        ispending = (op != 0) ? (ispending & 1) : 0;
        //
        dev->mutex.Unlock();
        //
        if (ispending != 0) {
            //
            if (THREAD_iscurrent(0)) {
                //
                SYNCTASK_run();
                THREAD_yield(1);
            } else {
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                //
                SIGNAL_wait(&FILEOPERATION::GetDevice(ophandle)->attention);
            }
        }
        //
        //
    } while (ispending != 0);

    return FILESYS_opstatus(ophandle);
}

long long FILESYS_completeop64(int ophandle) {
    FILEDEVICE *dev;
    FILEOPERATION *op;
    long long ret;

    dev = FILEOPERATION::GetDevice(ophandle);

    dev->mutex.Lock();
    op = dev->completed.Find(ophandle, true);
    dev->mutex.Unlock();

    //
    //
    //
    //
    ret = op->Complete();

    op->ClearId();

    //
    //
    //
    //
    //
    gpFileSysInfo->FreeOpsMutex.Lock();
    gpFileSysInfo->FreeOps.PushTail(op);
    gpFileSysInfo->FreeOpsMutex.Unlock();

    return ret;
}

int FILESYS_completeop(int ophandle) {
    return FILESYS_completeop64(ophandle);
}

void FILESYS_callbackop(int ophandle, void (*func)(int, int, void *)) {
    FILEDEVICE *dev;
    FILEOPERATION *op;
    int ispending;

    dev = FILEOPERATION::GetDevice(ophandle);

    //
    //
    //
    dev->mutex.Lock();

    op = iGetOpFromHandle(dev, ophandle, &ispending);

    //
    //
    //
    if (ispending != 0) {

        op->SetCallback(func);

    } else {

        //
        //
        //
        func(op->GetId(), op->GetStatus(), op->GetUserData());
    }

    //
    //
    dev->mutex.Unlock();
}

void FILESYS_priorityop(int ophandle, int priority) {
    FILEDEVICE *dev;
    FILEOPERATION *op;

    //
    //
    //
    dev = FILEOPERATION::GetDevice(ophandle);

    if (dev != 0) {

        dev->mutex.Lock();
        op = dev->pending.Find(ophandle, true);
        dev->mutex.Unlock();
        if (op != 0) {

            op->SetPriority(priority);
            //
            op->AddToQueue();
        }
    }
}

int FILESYS_exists(const char *name, int priority, void *userdata) {
    FILEOPERATION *op = new ExistOperation(name, priority, userdata, FILE_nametodevice(name));

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_open(const char *name, unsigned int modeflags, int priority, void *userdata) {
    FILEOPERATION *op =
        new OpenOperation(name, modeflags, priority, userdata, FILE_nametodevice(name));

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_close(int filehandle, int priority, void *userdata) {
    FILEOPERATION *op = new CloseOperation((FILESYSHANDLE *)filehandle, priority, userdata,
                                          ((FILESYSHANDLE *)filehandle)->dev);

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_read(int filehandle, int offset, void *buffer, int bytes, int priority,
                 void *userdata) {
    FILEOPERATION *op = new ReadOperation((FILESYSHANDLE *)filehandle, bytes, offset, buffer,
                                          priority, userdata, ((FILESYSHANDLE *)filehandle)->dev);

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_readlarge(int filehandle, unsigned long long offset, void *buffer,
                      unsigned long long bytes, int priority, void *userdata) {
    FILEOPERATION *op =
        new ReadLargeOperation((FILESYSHANDLE *)filehandle, bytes, offset, buffer, priority,
                               userdata, ((FILESYSHANDLE *)filehandle)->dev);

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_write(int filehandle, int offset, void *buffer, int bytes, int priority,
                  void *userdata) {
    FILEOPERATION *op = new WriteOperation((FILESYSHANDLE *)filehandle, bytes, offset, buffer,
                                           priority, userdata, ((FILESYSHANDLE *)filehandle)->dev);

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_size(int filehandle, int priority, void *userdata) {
    FILEOPERATION *op = new SizeOperation((FILESYSHANDLE *)filehandle, priority, userdata,
                                          ((FILESYSHANDLE *)filehandle)->dev);

    op->AddToQueue();

    return op->GetId();
}

int FILESYS_atomic(int (*func)(int, void *), FILEDEVICE *device, int priority, void *userdata) {
    int result;
    int oldpriority;

    //
    //
    //
    //
    if (device->running == 0) {
        iStartDevice(device);
    }

    device->atomic.Lock();

    //
    //
    //
    //
    oldpriority = device->minpriority;
    device->minpriority = priority;

    //
    result = func(priority, userdata);

    //
    device->minpriority = oldpriority;
    SIGNAL_set(&device->signal);

    //
    device->atomic.Unlock();

    //
    return result;
}

namespace RealFile {

bool GetInfoFastByName(const char *name, const unsigned int modeflags, uint64_t &location,
                       uint64_t &size) {
    FILESYSHANDLE *h;

    h = iOpenFileSysHandle(name, modeflags | 0x20001, FILE_nametodevice(name));

    //
    //
    //
    if (h != (FILESYSHANDLE *)-1) {

        GetInfoFastByHandle((int)h, location, size);
        iCloseFileSysHandle(h);
        return true;
    }

    //
    //
    location = 0;
    size = 0;

    return false;
}

void GetInfoFastByHandle(int filehandle, uint64_t &location, uint64_t &size) {
    FILESYSHANDLE *h = (FILESYSHANDLE *)filehandle;

    //
    location = h->dev->drv->QueryLocation(h->hFile);
    size = h->length;
}

} // namespace RealFile

FILEDEVICE *FILE_nametodevice(const char *name) {
    char devname[16];
    ListSingleIterator<FILEDEVICE> Itr;
    bool found;

    found = false;

    //
    //
    //
    //
    //
    //
    //
    if (strchr(name, ':') != 0) {

        MEM_clear(devname, sizeof(devname));

        strncpy(devname, name, strchr(name, ':') - name + 1);

    } else if (name[0] == '/' || name[0] == '\\') {

        MEM_clear(devname, sizeof(devname));

        //
        //
        const char *end = strchr(gpFileSysInfo->psearchpaths[0], ':');
        strncpy(devname, gpFileSysInfo->psearchpaths[0],
                end - gpFileSysInfo->psearchpaths[0] + 1);

    } else {

        return gpFileSysInfo->DeviceMemory;
    }

    //
    //
    //
    //
    Itr = gpFileSysInfo->AllocatedDevices.Begin();
    if (*Itr != 0) {
        for (; !found;) {
            if (strcasecmp((*Itr)->drv->GetName(), devname) == 0) {
                return *Itr;
            }
            Itr++;
            if (*Itr == 0) {
                break;
            }
        }
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    return 0;
}

namespace RealFile {

void SetSearchPath(const char *path) {
    char *p;
    int n;

    //
    //
    //
    //
    //
    //
    //
    MEM_clear(gpFileSysInfo->searchpath, gFileSysOpts.nSearchPathLength);
    MEM_clear(gpFileSysInfo->psearchpaths, gFileSysOpts.nSearchLocs * 4);

    strcpy(gpFileSysInfo->searchpath, path);

    p = gpFileSysInfo->searchpath;
    gpFileSysInfo->psearchpaths[0] = p;
    n = 1;

    //
    //
    while ((p = strchr(p, ';')) != 0) {

        *p = 0;

        //
        if (p[-1] == '/' || p[-1] == '\\') {
            p[-1] = 0;
        }
        //
        p++;
        gpFileSysInfo->psearchpaths[n] = p;
        n++;
    }

    //
    //
    p = gpFileSysInfo->psearchpaths[n - 1];
    p += strlen(p);

    //
    if (p[-1] == '/' || p[-1] == '\\') {
        p[-1] = 0;
    }
}

void AddSearchLocation(const char *pszLoc, bool head) {
    char *p;
    char *newpath;

    p = gpFileSysInfo->searchpath;

    //
    while (p[1] != 0) {

        if (*p != 0) {

            //
            while (*++p != 0) {
            }
        }

        //
        //
        if (p[1] == 0 && p[2] != 0) {

            //
            *++p = '/';
        }

        //
        *p = ';';
    }

    //
    *p = 0;

    newpath = (char *)__builtin_alloca(gFileSysOpts.nSearchPathLength);
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

    //
    //
    SetSearchPath(newpath);
}

unsigned int AddDevice(DeviceDriver *drv) {
    FILEDEVICE *dev;

    dev = gpFileSysInfo->FreeDevices.Pop();

    //
    //
    //
    //
    dev->drv = drv;

    gpFileSysInfo->AllocatedDevices.PushTail(dev);

    //
    //
    //
    //
    //
    return dev - gpFileSysInfo->DeviceMemory;
}

void RemoveDevice(unsigned int DevId) {
    FILEDEVICE *dev;

    //
    //
    //
    dev = &gpFileSysInfo->DeviceMemory[DevId];

    //
    //
    //
    //
    if (gpFileSysInfo->AllocatedDevices.Remove(dev, 0)) {

        //
        //
        if (dev->running != 0) {

            //
            dev->terminate = 1;

            SIGNAL_set(&dev->signal);

            //
            while (dev->running != 0) {
                THREAD_yield(0);
            }

            SIGNAL_destroy(&dev->signal);

            //
            //
            //
            //
            //
            //
            //
            SIGNAL_set(&dev->attention);
            SIGNAL_destroy(&dev->attention);

            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            //
            gFileSysOpts.allocator->Free(dev->pThreadStack, 0);
            dev->pThreadStack = 0;

            //
            //
            dev->mutex.Destroy();
            dev->atomic.Destroy();

            //
            dev->drv->Restore();
        }

        //
        //
        MEM_clear(dev, 4);

        gpFileSysInfo->FreeDevices.PushTail(dev);
    }
}

} // namespace RealFile
