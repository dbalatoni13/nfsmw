/*
 * Dolphin SDK - OdemuExi2/DebuggerDriver.c
 *
 * COMPILADOR: Metrowerks CodeWarrior, NO SN ProDG.  Prueba medida: el .s no
 * lleva `gcc2_compiled.` y usa `addi rD,rS,0` como copia de registro **34
 * veces**; micro-banco con las doce versiones del arbol: GC/1.0..1.2.5n
 * emiten `addi rD,rS,0` y GC/1.3..2.7 emiten `mr`.  O sea la misma familia
 * que el resto del Dolphin SDK (GC/1.2.5n, cflags_dolphin).
 *
 * El orden de las funciones es el del .s del troceador.
 */

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef int BOOL;
typedef enum {
    AMC_EXI_NO_ERROR = 0,
    AMC_EXI_UNSELECTED = 1
} AmcExiError;
typedef void (*EXICallback)(s32 chan, void *context);
typedef void (*DBInterruptCallback)(s16 cause, void *context);

#define FALSE 0
#define TRUE 1

#define EXI2_DEV 5

/* Registros del EXI: 3 canales x 5 palabras desde 0xCC006800.  El canal 2 es
   el del depurador: CSR = [10], MAR = [11], LEN = [12], CR = [13], DATA = [14]. */
/* La sintaxis `objeto : direccion` de MWCC (AT_ADDRESS del Dolphin SDK) es lo
   que reproduce el `addi rN, rBase, 0x6800` + `stwu/lwzu` con desplazamiento
   corto del objetivo; con `((vu32*)0xCC006800)[10]` MWCC pliega la direccion
   entera y emite `stw r0, 0x6828(rBase)`, que es otro codigo. */
volatile u32 __EXIRegs[15] : 0xCC006800;
#define EXI2_CSR __EXIRegs[10]
#define EXI2_CR __EXIRegs[13]
/* El objetivo lee/escribe SIEMPRE el registro de datos con la direccion
   plegada (`lwz r6, 0x6838(rBase)`) y en cambio CSR y CR con el puntero
   al array (`stwu r0, 0x28(rN)`): son dos expresiones distintas. */
#define EXI2_DATA (*(volatile u32 *)0xCC006838)

volatile u32 __PIRegs[16] : 0xCC003000;

/* Estado del EXI que comparte con el runtime del depurador. */
typedef struct EXIControl {
    EXICallback exiCallback;                     /* 0x00 */
    EXICallback tcCallback;                      /* 0x04 */
    EXICallback extCallback;                     /* 0x08 */
    volatile s32 state;                           /* 0x0C */
    s32 immLen;                                   /* 0x10 */
    u8 *immBuf;                                   /* 0x14 */
} EXIControl;                                     /* 0x18 */

#define EXI_STATE_DMA 0x01
#define EXI_STATE_IMM 0x02
#define EXI_STATE_BUSY (EXI_STATE_DMA | EXI_STATE_IMM)
#define EXI_STATE_SELECTED 0x04

extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern u32 __OSMaskInterrupts(u32 mask);
extern u32 __OSUnmaskInterrupts(u32 mask);
extern void *__OSSetInterruptHandler(s32 interrupt, void *handler);
extern int printf(const char *fmt, ...);
extern void *memset(void *dst, int val, u32 n);



static DBInterruptCallback MTRCallback;
static DBInterruptCallback DBGCallback;
static u32 SendMailData;
static s32 RecvDataLeng;
static u8 *pEXIInputFlag;
static u8 EXIInputFlag;
static u8 SendCount = 0x80;

void DBClose(void);
void DBOpen(void);
BOOL DBWrite(u32 *buffer, s32 length);
BOOL DBRead(u32 *buffer, s32 length);
s32 DBQueryData(void);
void DBInitInterrupts(void);
void DBInitComm(u8 **pointer, DBInterruptCallback callback);

static void DBGHandler(s32 cause, void *context);
static void MWCallback(s16 cause, void *context);
static BOOL DBGReadStatus(u32 *status);
static BOOL DBGWrite(u32 addr, u32 *buffer, s32 length);
static BOOL DBGRead(u32 addr, u32 *buffer, s32 length);
static BOOL DBGReadMailbox(u32 *mail);
static BOOL DBGEXIImm(void *buffer, s32 length, u32 mode);
static void EXI2_CallBack(s32 chan, void *context);

void EXI2_Init(u8 **pointer, EXICallback callback);
void EXI2_EnableInterrupts(void);
BOOL EXI2_Poll(void);
AmcExiError EXI2_ReadN(void *buffer, s32 length);
AmcExiError EXI2_WriteN(void *buffer, s32 length);
void EXI2_Reserve(void);
void EXI2_Unreserve(void);

BOOL AmcEXIImm(void *buffer, s32 length, u32 mode, EXICallback callback);
void AmcEXISync(void);
void AmcEXIClearInterrupts(BOOL exi, BOOL tc);
EXICallback AmcEXISetExiCallback(EXICallback callback);
BOOL AmcEXISelect(u32 freq);
BOOL AmcEXIDeselect(void);
static void AmcDebugIntHandler(s32 interrupt, void *context);
void AmcEXIEnableInterrupts(void);
void AmcEXIInit(void);

void DBClose(void)
{
}

void DBOpen(void)
{
}

/* These transfers are expanded in DBWrite, but not in DBQueryData. The caller
   owns their command words so each expansion retains its original stack slot. */
static inline BOOL DBReadStatusInline(u32 *status, u32 *cmd)
{
    BOOL err;
    EXI2_CSR = (EXI2_CSR & 0x405) | 0xC0;
    *cmd = 0x40000000;
    err = !DBGEXIImm(cmd, 2, 1);
    while (EXI2_CR & 1) {
    }
    err |= !DBGEXIImm(status, 4, 0);
    while (EXI2_CR & 1) {
    }
    EXI2_CSR = EXI2_CSR & 0x405;
    return !err;
}

static inline BOOL DBWriteMailboxInline(u32 value, u32 *mail)
{
    BOOL err;
    EXI2_CSR = (EXI2_CSR & 0x405) | 0xC0;
    *mail = value;
    err = !DBGEXIImm(mail, 4, 1);
    while (EXI2_CR & 1) {
    }
    EXI2_CSR = EXI2_CSR & 0x405;
    return !err;
}

BOOL DBWrite(u32 *buffer, s32 length)
{
    u32 status;
    u32 commandBeforeWrite;
    u32 commandAfterWrite;
    u32 commandGap1;
    u32 mailCommand;
    u32 commandGap2;
    u32 commandAfterMail;
    u32 mail;
    BOOL enabled;
    s32 alignedLength;
    u32 addr;
    u32 offset;
    BOOL ok;
    /* Original caller frame is 0x78 bytes, with distinct inline command slots. */
    u32 pad[8];

    enabled = OSDisableInterrupts();

    do {
        DBReadStatusInline(&status, &commandBeforeWrite);
    } while (status & 2);

    SendCount++;
    offset = (SendCount & 1) ? 0x1000 : 0;
    alignedLength = (length + 3) & ~3;
    addr = offset | 0x0001C000;

    do {
        ok = DBGWrite(addr, buffer, alignedLength);
    } while (ok == FALSE);

    do {
        DBReadStatusInline(&status, &commandAfterWrite);
    } while (status & 2);

    mail = (((SendCount << 16) | 0x1F000000 | length) & 0x1FFFFFFF) | 0xC0000000;

    do {
        ok = DBWriteMailboxInline(mail, &mailCommand);
    } while (ok == FALSE);

    do {
        ok = DBReadStatusInline(&status, &commandAfterMail);
    } while (ok == FALSE || (status & 2));

    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL DBRead(u32 *buffer, s32 length)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    DBGRead((((SendMailData & 0x10000) ? 0x1000 : 0) + 0x20000) - 0x2000,
            buffer, (length + 3) & ~3);
    RecvDataLeng = 0;
    EXIInputFlag = 0;
    OSRestoreInterrupts(enabled);
    return FALSE;
}

s32 DBQueryData(void)
{
    BOOL enabled;
    u32 mail;

    EXIInputFlag = 0;
    if (RecvDataLeng == 0) {
        enabled = OSDisableInterrupts();
        DBGReadStatus(&mail);
        if (mail & 1) {
            DBGReadMailbox(&mail);
            mail = mail & 0x1FFFFFFF;
            if ((mail & 0x1F000000) == 0x1F000000) {
                SendMailData = mail;
                RecvDataLeng = mail & 0x7FFF;
                EXIInputFlag = 1;
            }
        }
        OSRestoreInterrupts(enabled);
    }
    return RecvDataLeng;
}

void DBInitInterrupts(void)
{
    __OSMaskInterrupts(0x18000);
    __OSMaskInterrupts(0x40);
    DBGCallback = MWCallback;
    __OSSetInterruptHandler(0x19, (void *)DBGHandler);
    __OSUnmaskInterrupts(0x40);
}

void DBInitComm(u8 **pointer, DBInterruptCallback callback)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    pEXIInputFlag = &EXIInputFlag;
    *pointer = pEXIInputFlag;
    MTRCallback = callback;
    __OSMaskInterrupts(0x18000);
    EXI2_CSR = 0;
    OSRestoreInterrupts(enabled);
}

static void DBGHandler(s32 cause, void *context)
{
    __PIRegs[0] = 0x1000;
    if (DBGCallback) {
        DBGCallback(cause, context);
    }
}

static void MWCallback(s16 cause, void *context)
{
    EXIInputFlag = 1;
    if (MTRCallback) {
        MTRCallback(0, context);
    }
}

static BOOL DBGReadStatus(u32 *status)
{
    BOOL err;
    u32 cmd;
    /* Preserve the original command slot at sp+0x18 in the 0x38-byte frame. */
    u32 pad[3];

    {
        u32 csr = EXI2_CSR;
        csr &= 0x405;
        csr |= 0xC0;
        EXI2_CSR = csr;
    }
    cmd = 0x40000000;
    err = !DBGEXIImm(&cmd, 2, 1);
    while (EXI2_CR & 1) {
    }
    err |= !DBGEXIImm(status, 4, 0);
    while (EXI2_CR & 1) {
    }
    EXI2_CSR = EXI2_CSR & 0x405;
    return !err;
}

static BOOL DBGWrite(u32 addr, u32 *buffer, s32 length)
{
    BOOL err;
    u32 cmd;
    u32 data;
    /* Original temporary area below the command and data words. */
    u32 pad[3];
    u32 *dataPtr = buffer;

    {
        u32 csr = EXI2_CSR;
        csr &= 0x405;
        csr |= 0xC0;
        EXI2_CSR = csr;
    }
    cmd = 0xA0000000 | ((addr << 8) & 0x01FFFC00);
    err = !DBGEXIImm(&cmd, 4, 1);
    while (EXI2_CR & 1) {
    }
    while (length != 0) {
        data = *dataPtr;
        dataPtr++;
        err |= !DBGEXIImm(&data, 4, 1);
        while (EXI2_CR & 1) {
        }
        length -= 4;
        if (length < 0) {
            length = 0;
        }
    }
    EXI2_CSR = EXI2_CSR & 0x405;
    return !err;
}

static BOOL DBGRead(u32 addr, u32 *buffer, s32 length)
{
    BOOL err;
    u32 cmd;
    u32 data;
    /* Original temporary area below the command and data words. */
    u32 pad[3];
    u32 *dataPtr = buffer;

    {
        u32 csr = EXI2_CSR;
        csr &= 0x405;
        csr |= 0xC0;
        EXI2_CSR = csr;
    }
    cmd = 0x20000000 | ((addr << 8) & 0x01FFFC00);
    err = !DBGEXIImm(&cmd, 4, 1);
    while (EXI2_CR & 1) {
    }
    while (length != 0) {
        err |= !DBGEXIImm(&data, 4, 0);
        while (EXI2_CR & 1) {
        }
        *dataPtr = data;
        dataPtr++;
        length -= 4;
        if (length < 0) {
            length = 0;
        }
    }
    EXI2_CSR = EXI2_CSR & 0x405;
    return !err;
}

static BOOL DBGReadMailbox(u32 *mail)
{
    BOOL err;
    u32 cmd;
    /* Preserve the original command slot at sp+0x18 in the 0x38-byte frame. */
    u32 pad[3];

    {
        u32 csr = EXI2_CSR;
        csr &= 0x405;
        csr |= 0xC0;
        EXI2_CSR = csr;
    }
    cmd = 0x60000000;
    err = !DBGEXIImm(&cmd, 2, 1);
    while (EXI2_CR & 1) {
    }
    err |= !DBGEXIImm(mail, 4, 0);
    while (EXI2_CR & 1) {
    }
    EXI2_CSR = EXI2_CSR & 0x405;
    return !err;
}

#pragma push
/* Preserve the original register comparisons at the two loop entries. */
#pragma opt_propagation off
static BOOL DBGEXIImm(void *buffer, s32 length, u32 mode)
{
    u32 writeVal;
    u32 readVal;
    s32 i;
    /* Original saved-register area starts at sp+0x20. */
    u32 pad[3];

    if (mode != 0) {
        i = 0;
        writeVal = 0;
        for (; i < length; i++) {
            writeVal |= ((u8 *)buffer)[i] << ((3 - i) << 3);
        }
        EXI2_DATA = writeVal;
    }

    EXI2_CR = ((mode << 2) | 1) | ((length - 1) << 4);
    while (EXI2_CR & 1) {
    }

    if (mode == 0) {
        u8 *dataPtr = (u8 *)buffer;
        i = 0;
        readVal = EXI2_DATA;
        for (; i < length; i++) {
            *dataPtr++ = readVal >> ((3 - i) << 3);
        }
    }
    return TRUE;
}
#pragma pop
