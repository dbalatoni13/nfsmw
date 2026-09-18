/* FSasync.c: la segunda mitad del troceado ppc2D2, que el ELF atribuye a
 * FSasync.c (marca `gcc2_compiled.` propia en 0x80311DA8).
 */

/* The original write-initialization entry is genuinely unsupported. */
int PCwriteAsyncInit(void) {
    return -1;
}

/* Los datos de la unidad, en el orden del DOL: .data 0x804397D0 (los cuatro a
 * cero) y .bss 0x804B4F40.  El manejador de EXI2TC los toca desde la interrupcion,
 * de ahi los `volatile`; el error no lo es (nunca se relee tras escribirlo).  */
volatile int g_nRWasyncPhase = 0;
static void (*g_FSCBFunc)(int error) = 0;
static volatile unsigned int g_nEXI2TCCnt = 0;
volatile unsigned int g_nDbgFsAsyncCnt = 0;

static int g_hHandle;
static char *g_pBuffer;
static volatile int g_nBlockCnt;
static volatile int g_nRemainderCnt;
static volatile unsigned int g_nTotalBytesRemaining;
static int g_bDoFSACK;
static int g_nFSLastError;

typedef int BOOL;
struct OSContext;
typedef void (*SNInterruptHandler)(short interrupt, struct OSContext *context);
BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);
unsigned int __OSUnmaskInterrupts(unsigned int mask);
SNInterruptHandler __OSSetInterruptHandler(short interrupt, SNInterruptHandler handler);

extern void SNDVDWrite_init(unsigned int bytes);
extern void SNDVDWriteNoDMA_next(const void *buffer, unsigned int bytes);
int CompleteAsync(void);

typedef struct {
    unsigned char command, reserved0;
    unsigned short size_le;
    unsigned int reserved1;
    unsigned char operation, reserved2[3];
    int handle;
    unsigned int offset, length;
} SNFSReadRequest;


static void DoFSReadHeader(int handle, unsigned long destination,
                           unsigned long length, unsigned long offset) {
    SNFSReadRequest request;
    unsigned char *clear = (unsigned char *)&request;
    unsigned int i;
    unsigned short size = 16;
    for (i = 0; i < sizeof(request); ++i)
        *clear++ = 0;
    /* Solo el tamano de la carga va en little-endian; las tres palabras son
       nativas. El objetivo NO pliega el `16 << 8`: tiene `li r0,0x10` y luego
       `slwi r0,r0,8`, dos instrucciones donde nosotros teniamos `li r0,0x1000`.
       O sea que en el original ese 16 no era una constante visible en el punto
       del desplazamiento. Barridas sin exito once formas: local `unsigned
       short`, `unsigned int` y `unsigned char`, `sizeof(request)`, el literal
       desplegado, una global `const`, la union de dos bytes y el helper inline
       --este ultimo ademas descarta que hubiera helper, porque truncar el valor
       de retorno a `unsigned short` obliga a GCC a enmascarar (`rlwinm
       0,0,8,16,23`) y el objetivo tiene un `slwi` limpio: el original computaba
       dentro del campo, que ya trunca solo al hacer `sth`. La barrera selectiva
       es el unico freno que queda al pliegue de CSE y da la forma exacta.

       REMEDIDO r65 Y SIGUE VIVO: quitando la barrera, .text 2104 -> 2100 B y
       `DoFSReadHeader` de 140 a 136 --el `li r0,16` + `slwi r0,r0,8` colapsa en
       un `li r0,0x1000`-. La condicion que hay que cumplir esta clara: que el
       16 NO sea una constante visible en el punto del desplazamiento. Eso solo
       pasa si viene de un objeto que el compilador no puede rastrear, y las dos
       familias que lo consiguen en C llano cuestan instrucciones: `volatile`
       local mete stw+lhz, y una tabla en `.rodata` mueve la seccion. Doce
       formas barridas en total. */
    /* CORRECCION r67: SALE EN C, SIN BARRERA.  Lo que pedia la veda --que el 16
       no sea una constante visible en el punto del desplazamiento-- lo da el
       BLOQUE BASICO, no el tipo: con `size` declarada arriba, antes del bucle de
       borrado, cse1 olvida la equivalencia en la etiqueta de cabeza del bucle;
       gcse no puede meter la constante dentro de un `ashift`
       (gcse.c try_replace_reg -> validate_replace_src sin simplificar:
       `(ashift (const_int 16) (const_int 8))` no es una insn reconocible), y
       combine no cruza bloques (LOG_LINKS).  Sale el `li r0,16` + `slwi r0,r0,8`
       del objetivo.  Sello ALLOC/SYM identico (395b1828d28dc4ac /
       dad1a80b0785ccef) en cuatro grafias: `unsigned short`, `unsigned int`,
       como primera declaracion y con la asignacion aparte antes del bucle.
       Quitar solo la barrera de la forma anterior: 447b6814b9705ade. */
    request.size_le = (unsigned short)(size << 8);
    request.command = 9;
    if (g_bDoFSACK)
        request.operation = 9;
    else
        request.operation = 11;
    request.handle = handle;
    request.offset = offset;
    request.length = length;
    SNDVDWrite_init(sizeof(request));
    SNDVDWriteNoDMA_next(&request, sizeof(request));
}

static void InitReadCounts(void) {
    unsigned int bytes = g_nTotalBytesRemaining > 0x1FC00u
                             ? 0x1FC00u : g_nTotalBytesRemaining;
    g_nTotalBytesRemaining -= bytes;
    g_nRWasyncPhase = 2;
    g_nBlockCnt = bytes >> 10;
    g_nRemainderCnt = bytes & 0x3FF;
}

/* These original SNDVD entry points accept normal C arguments in r3/r4.
 * Their assembly implementations save r28-r31 around the private EXI helpers.
 */
extern void SNDVDRead_init(void);
extern void SNDVDReadAsync_next(void *buffer, unsigned int bytes);
extern void SNDVDReadSync_next(void *buffer, unsigned int bytes);
extern unsigned int SNQueryData(void);

static void PCreadAsyncNext(void) {
    char *buffer = g_pBuffer;
    unsigned int bytes;
    *(volatile unsigned int *)0xCC003000 = 0x1000;
    InitReadCounts();
    if (g_nBlockCnt != 0) {
        char *next = g_pBuffer;
        int remaining = g_nBlockCnt;
        bytes = 1024;
        next += 1024;
        --remaining;
        g_pBuffer = next;
        g_nBlockCnt = remaining;
    } else {
        bytes = g_nRemainderCnt;
        g_nRemainderCnt = 0;
    }
    SNDVDRead_init();
    SNDVDReadAsync_next(buffer, bytes);
}

int PCreadAsyncInit(int handle, unsigned long destination,
                    unsigned long length, void (*callback)(int),
                    unsigned long offset, int do_ack) {
    if (!(destination & 31) && !(length & 31)) {
        CompleteAsync();
        g_bDoFSACK = do_ack;
        g_FSCBFunc = callback;
        DoFSReadHeader(handle, destination, length, offset);
        g_nTotalBytesRemaining = length;
        g_hHandle = handle;
        g_pBuffer = (char *)destination;
        g_nRWasyncPhase = 1;
        return 0;
    }
    return -1;
}

static void ReadSyncNext(void) {
    char *buffer = g_pBuffer;
    unsigned int bytes;
    while (SNQueryData() == 0)
        ;
    *(volatile unsigned int *)0xCC003000 = 0x1000;
    InitReadCounts();
    if (g_nBlockCnt != 0) {
        int remaining = g_nBlockCnt;
        char *next = g_pBuffer;
        bytes = 1024;
        --remaining;
        next += 1024;
        g_nBlockCnt = remaining;
        g_pBuffer = next;
    } else {
        bytes = g_nRemainderCnt;
        g_nRemainderCnt = 0;
    }
    SNDVDRead_init();
    SNDVDReadSync_next(buffer, bytes);
}

typedef struct {
    unsigned char command;
    unsigned char reserved;
    unsigned short size;
    unsigned int id;
    int error;
} SNFSResult;

static SNFSResult g_FsResult;
extern int SNRead(void *buffer, unsigned int bytes);
extern int SNWrite(const void *buffer, unsigned int bytes);

static void PCrwSyncFSACK(void);

/* El bucle de bloques es un post-decremento en la condicion: fold-const.c
 * convierte `g_nBlockCnt-- != 0` en `--g_nBlockCnt != -1`, y como la global es
 * volatile se relee tras el almacen (el `lwz` + `cmpwi -1` del objetivo).  La
 * copia de la prueba que jump.c pone delante del bucle comparte el `lis` con el
 * invariante que loop.c saca del cuerpo.  */
static void CompletePCreadAsync(void) {
    if (g_nRWasyncPhase == 1)
        ReadSyncNext();

    do {
    } while (*(volatile unsigned int *)0xCC006834 & 1);

    while (g_nBlockCnt--) {
        SNDVDReadSync_next(g_pBuffer, 1024);
        g_pBuffer += 1024;
    }
    if (g_nRemainderCnt) {
        SNDVDReadSync_next(g_pBuffer, g_nRemainderCnt);
        g_nRemainderCnt = 0;
    }
    *(volatile unsigned int *)0xCC006828 = 0;

    while (g_nTotalBytesRemaining) {
        ReadSyncNext();
        while (g_nBlockCnt--) {
            SNDVDReadSync_next(g_pBuffer, 1024);
            g_pBuffer += 1024;
        }
        if (g_nRemainderCnt) {
            SNDVDReadSync_next(g_pBuffer, g_nRemainderCnt);
            g_nRemainderCnt = 0;
        }
        *(volatile unsigned int *)0xCC006828 = 0;
    }
    PCrwSyncFSACK();
}

static void PCrwAsyncFSACK(void) {
    SNFSResult *result = &g_FsResult;
    unsigned int bytes;
    while ((bytes = SNQueryData()) == 0)
        ;
    SNRead(result, bytes);
    result->size = 0;
    g_nFSLastError = result->error;
    SNWrite(result, 8);
    g_nRWasyncPhase = 0;
    if (g_FSCBFunc) {
        g_FSCBFunc(g_nFSLastError);
        g_FSCBFunc = 0;
    }
    *(volatile unsigned int *)0xCC003000 = 0x1000;
}

static void PCrwSyncFSACK(void) {
    SNFSResult *result = &g_FsResult;
    if (g_bDoFSACK) {
        unsigned int bytes;
        while ((bytes = SNQueryData()) == 0)
            ;
        SNRead(result, bytes);
        g_nFSLastError = result->error;
        result->size = 0;
        SNWrite(result, 8);
        g_nRWasyncPhase = 0;
        if (g_FSCBFunc) {
            g_FSCBFunc(g_nFSLastError);
            g_FSCBFunc = 0;
        }
    }
    g_FSCBFunc = 0;
    *(volatile unsigned int *)0xCC006828 = 8;
    *(volatile unsigned int *)0xCC003000 = 0x1000;
    g_nRWasyncPhase = 0;
}

int CompleteAsync(void) {
    if (g_nRWasyncPhase == 0)
        return 0;
    if (g_nRWasyncPhase == 5)
        PCrwSyncFSACK();
    else
        CompletePCreadAsync();
    return g_nFSLastError;
}

void PCrwAsyncNextPh(unsigned int phase) {
    switch (phase) {
    case 1:
        PCreadAsyncNext();
        break;
    case 2:
    case 4:
        ++g_nDbgFsAsyncCnt;
        break;
    case 3:
        break;
    case 5:
        PCrwAsyncFSACK();
        break;
    }
}

static void EXI2TCHandler(short interrupt, struct OSContext *context) {
    char *buffer = g_pBuffer;
    volatile unsigned int *csr = (volatile unsigned int *)0xCC006828;
    unsigned int bytes;
    ++g_nEXI2TCCnt;
    *csr = 136;
    if (g_nRWasyncPhase == 0)
        return;
    if (g_nBlockCnt != 0) {
        if (g_nRWasyncPhase == 4) {
            g_pBuffer = buffer + 512;
            bytes = 512;
        } else {
            g_pBuffer = buffer + 1024;
            bytes = 1024;
        }
        --g_nBlockCnt;
    } else if (g_nRemainderCnt != 0) {
        bytes = g_nRemainderCnt;
        g_nRemainderCnt = 0;
    } else {
        void (*cb)(int);
        int ack;
        *csr = 0;
        if (g_nTotalBytesRemaining != 0) {
            if (g_nRWasyncPhase != 4)
                g_nRWasyncPhase = 1;
            return;
        }
        ack = g_bDoFSACK;
        if (ack) {
            g_nRWasyncPhase = 5;
            return;
        }
        cb = g_FSCBFunc;
        g_nRWasyncPhase = 0;
        g_nFSLastError = 0;
        if (cb) {
            cb(0);
            g_FSCBFunc = 0;
        }
        return;
    }
    SNDVDReadAsync_next(buffer, bytes);
}

void SNInitEXI2TCHandler(void) {
    BOOL enabled = OSDisableInterrupts();
    __OSSetInterruptHandler(16, EXI2TCHandler);
    __OSUnmaskInterrupts(0x8000u);
    *(volatile unsigned int *)0xCC006828 |= 8;
    *(volatile unsigned int *)0xCC006828 &= ~4u;
    OSRestoreInterrupts(enabled);
}
