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



static EXIControl Ecb;
static EXIControl *exi = &Ecb;

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

#pragma push
/* Keep the zero index in its register for the original cmpw pretest. */
#pragma opt_propagation off
BOOL AmcEXIImm(void *buffer, s32 length, u32 mode, EXICallback callback)
{
    BOOL enabled;
    s32 chan;
    u32 data;
    s32 i;
    u32 pad[2];

    chan = 2;
    enabled = OSDisableInterrupts();
    if ((exi->state & EXI_STATE_BUSY) || !(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if (exi->tcCallback != 0) {
        AmcEXIClearInterrupts(FALSE, TRUE);
        __OSUnmaskInterrupts(0x00200000U >> (chan * 3));
    }

    exi->state |= EXI_STATE_IMM;

    if (mode != 0) {
        i = 0;
        data = 0;
        for (; i < length; i++) {
            data |= ((u8 *)buffer)[i] << (8 * (3 - i));
        }
        EXI2_DATA = data;
    }

    exi->immBuf = (u8 *)buffer;
    exi->immLen = (mode != 1) ? length : 0;
    EXI2_CR = ((mode << 2) | 1) | ((length - 1) << 4);

    OSRestoreInterrupts(enabled);
    return TRUE;
}
#pragma pop

void AmcEXISync(void)
{
    BOOL enabled;

    while (exi->state & EXI_STATE_SELECTED) {
        if (EXI2_CR & 1) {
            continue;
        }
        enabled = OSDisableInterrupts();
        if (exi->state & EXI_STATE_BUSY) {
            if (exi->state & EXI_STATE_IMM) {
                s32 len = exi->immLen;
                if (len != 0) {
                    s32 i;
                    u32 data;
                    u8 *buf;

                    buf = exi->immBuf;
                    data = EXI2_DATA;

                    for (i = 0; i < len; i++) {
                        *buf++ = data >> (8 * (3 - i));
                    }
                }
            }
            exi->state &= ~EXI_STATE_BUSY;
        }
        OSRestoreInterrupts(enabled);
        break;
    }
}

void AmcEXIClearInterrupts(BOOL exi, BOOL tc)
{
    if (exi) {
        __PIRegs[0] = (__PIRegs[0] & 0x1000) | 0x1000;
    }
    if (tc) {
        EXI2_CSR = (EXI2_CSR & 0x7F5) | 8;
    }
}

EXICallback AmcEXISetExiCallback(EXICallback callback)
{
    BOOL enabled;
    EXICallback prev;

    enabled = OSDisableInterrupts();
    if (callback) {
        __OSUnmaskInterrupts(0x40);
    } else {
        __OSMaskInterrupts(0x40);
    }
    prev = exi->exiCallback;
    exi->exiCallback = callback;
    OSRestoreInterrupts(enabled);
    return prev;
}

BOOL AmcEXISelect(u32 freq)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (exi->state & EXI_STATE_SELECTED) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state |= EXI_STATE_SELECTED;
    {
        u32 v = EXI2_CSR;
        v &= 0x405;
        v |= (freq << 4) | 0x80;
        EXI2_CSR = v;
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL AmcEXIDeselect(void)
{
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~EXI_STATE_SELECTED;
    EXI2_CSR = EXI2_CSR & 0x405;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

static void AmcDebugIntHandler(s32 interrupt, void *context)
{
    EXICallback callback;

    __PIRegs[0] = (__PIRegs[0] & 0x1000) | 0x1000;
    callback = exi->exiCallback;
    if (callback) {
        callback(2, context);
    }
}

void AmcEXIEnableInterrupts(void)
{
    __OSMaskInterrupts(0x00008000);
    __OSSetInterruptHandler(0x19, (void *)AmcDebugIntHandler);
    __OSUnmaskInterrupts(0x40);
}

void AmcEXIInit(void)
{
    memset(&Ecb, 0, sizeof(EXIControl));
    EXI2_CSR = 0;
    *(volatile u32 *)0xCC00643C = 0;
}
