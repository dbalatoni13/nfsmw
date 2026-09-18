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



static u8 ucEXI2InputPending;
static u8 *pucEXI2InputPending = &ucEXI2InputPending;
static s32 fExi2Selected;
static EXICallback TRK_Callback;

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

static void EXI2_CallBack(s32 chan, void *context)
{
    *pucEXI2InputPending = 1;
    if (TRK_Callback) {
        TRK_Callback(0, context);
    }
}

/* Keep the initialization values as inline arguments and the transfer words
   in the caller's frame; both order and storage match the original sequence. */
static inline void EXI2InitCommands(u32 zero, u32 cmd, u32 *zeroWord, u32 *commandWord)
{
    BOOL ok;
    u32 pad[2];
    u32 head;

    *zeroWord = zero;
    *commandWord = cmd;
    ok = FALSE;
    while (ok == FALSE) {
        ok = AmcEXISelect(EXI2_DEV);
    }

    head = 0x80000000;
    AmcEXIImm(&head, 2, 1, 0);
    AmcEXISync();
    AmcEXIImm(commandWord, 4, 1, 0);
    AmcEXISync();
    AmcEXIImm(zeroWord, 4, 1, 0);
    AmcEXISync();
    AmcEXIDeselect();
}

void EXI2_Init(u8 **pointer, EXICallback callback)
{
    BOOL enabled;
    u32 zero;
    u32 cmd;
    u32 pad[2];

    *pointer = pucEXI2InputPending;
    enabled = OSDisableInterrupts();
    TRK_Callback = callback;
    OSRestoreInterrupts(enabled);

    AmcEXIInit();
    EXI2InitCommands(0, 0x01000003, &zero, &cmd);
}

void EXI2_EnableInterrupts(void)
{
    AmcEXISetExiCallback(EXI2_CallBack);
    AmcEXIEnableInterrupts();
}

/* UNSOLVED, 264 B al 99,8182%. Queda UNA fila: el orden de los dos lwz del
   bloque de salida, que es comun a los dos caminos.
       objetivo:  lwz r0, 0x24(r1)   <- LR primero
                  lwz r3, 0x18(r1)   <- luego el valor de retorno
       nuestro:   lwz r3, 0x18(r1)
                  lwz r0, 0x24(r1)
   El objetivo deja tres instrucciones entre el lwz del LR y su mtlr; nosotros
   dos. Barrido sin mover la fila: OCHO formas del bloque del &= (solo el orden
   invertido movio, y ya esta aplicado: c2 temporal 99,2121, c3 sin &= 99,2879,
   c4 99,2121, c5 puntero local 99,2121, c6 mascara al reves 99,2879, c7 99,0606),
   VEINTIUN #pragma por funcion (quince identicos; peephole off 89,06 con 276 B,
   scheduling off 79,09, scheduling 750 90,73, 604 93,76 -- o sea que -proc gekko
   es el planificador correcto, igual que en steering), y SIETE combinaciones de
   volatile en las locales (identicas salvo sel volatile, 91,94 con 284 B).
   La unidad no es promocionable de todas formas: splits.txt corta su .data justo
   antes de 0x8044F610 y la cadena de printf cae en auto_06. */
BOOL EXI2_Poll(void)
{
    u32 data;
    u32 status;
    u32 pad[2];
    u32 head;
    BOOL sel;

    if (!(__PIRegs[0] & 0x1000) && *pucEXI2InputPending == 0) {
        data = 0;
    } else {
        if (fExi2Selected == 0) {
            sel = FALSE;
            while (sel == FALSE) {
                sel = AmcEXISelect(EXI2_DEV);
            }
            if (sel == FALSE) {
                printf("Can't select EXI2 port!\n");
            } else {
                fExi2Selected = 1;
                head = 0;
                AmcEXIImm(&head, 2, 1, 0);
                AmcEXISync();
                AmcEXIImm(&status, 2, 0, 0);
                AmcEXISync();
            }
        }

        AmcEXIImm(&data, 4, 0, 0);
        AmcEXISync();

        /* El &= VA PRIMERO: 99,2879 -> 99,8182%. Con este orden el
         * planificador de mwcc rellena el hueco de latencia del lwz con el
         * li+stb del byte, que es justo lo que hace el objetivo:
         *     lwz r3,0x3000(r4); li r0,1; stb r0,0(r5); rlwinm r0,r3; stw
         * Con el orden contrario salen los mismos seis opcodes con los
         * registros permutados (r3/r0/r4 en vez de r4/r3/r0). */
        if (*pucEXI2InputPending == 0) {
            __PIRegs[0] &= 0x1000;
            *pucEXI2InputPending = 1;
        }
    }
    return data;
}

AmcExiError EXI2_ReadN(void *buffer, s32 length)
{
    BOOL sel;
    s32 i;
    s32 rest;
    s32 n;
    s32 pos;
    s32 nwords;
    u32 data;
    u32 status;
    u32 pad[1];
    u32 head;

    if (fExi2Selected != 0) {
        sel = TRUE;
    } else {
        BOOL selected = FALSE;
        while (!selected) {
            selected = AmcEXISelect(EXI2_DEV);
        }
        sel = selected;
        if (!sel) {
            printf("Can't select EXI2 port!\n");
            sel = FALSE;
        } else {
            fExi2Selected = 1;
            head = 0;
            AmcEXIImm(&head, 2, 1, 0);
            AmcEXISync();
            AmcEXIImm(&status, 2, 0, 0);
            AmcEXISync();
        }
    }

    if (!sel) {
        return AMC_EXI_UNSELECTED;
    }

    rest = length & 3;
    nwords = ((u32)length >> 2) + ((length & 3) ? 1 : 0);
    rest = rest + (length & 1);
    n = length % 4;
    pos = 0;
    i = 0;
    while ((u32)pos < (u32)length) {
        s32 len;

        if ((u32)i < (u32)(nwords - 1)) {
            len = 4;
        } else if ((u32)rest == 2) {
            len = 2;
        } else {
            len = 4;
        }
        AmcEXIImm(&data, len, 0, 0);
        AmcEXISync();
        {
            s32 k;
            s32 bytesToCopy;
            if ((u32)(length - pos) >= 4) {
                bytesToCopy = 4;
            } else {
                bytesToCopy = n;
            }
            for (k = 0; k < bytesToCopy; k++) {
                ((u8 *)((u32)buffer + pos))[k] = data >> (8 * (3 - k));
            }
        }
        i++;
        pos += 4;
    }

    AmcEXIDeselect();
    fExi2Selected = 0;
    *pucEXI2InputPending = 0;
    return AMC_EXI_NO_ERROR;
}

AmcExiError EXI2_WriteN(void *buffer, s32 length)
{
    s32 i;
    u8 *dataPtr = buffer;
    s32 nwords;
    s32 transferLength = length;
    u32 status;
    /* Keep the original distinct command/status words and stack slots. */
    u32 pad1[2];
    u32 head;
    u32 pad2[1];
    u32 statusHead;
    BOOL sel;

    sel = FALSE;
    while (!sel) {
        sel = AmcEXISelect(EXI2_DEV);
    }
    if (!sel) {
        return AMC_EXI_UNSELECTED;
    }

    head = 0x80000000;
    AmcEXIImm(&head, 2, 1, 0);
    AmcEXISync();
    AmcEXIImm(&transferLength, 4, 1, 0);
    AmcEXISync();

    nwords = ((u32)transferLength >> 2) + ((transferLength & 3) ? 1 : 0);
    for (i = 0; (u32)i < (u32)nwords; i++) {
        s32 len;

        if ((u32)i < (u32)(nwords - 1)) {
            len = 4;
        } else if ((u32)((transferLength & 3) + (transferLength & 1)) == 2) {
            len = 2;
        } else {
            len = 4;
        }
        {
            u8 *chunk = dataPtr;
            dataPtr += 4;
            AmcEXIImm(chunk, len, 1, 0);
        }
        AmcEXISync();
    }
    AmcEXIDeselect();

    sel = FALSE;
    while (!sel) {
        sel = AmcEXISelect(EXI2_DEV);
    }
    if (!sel) {
        return AMC_EXI_UNSELECTED;
    }

    statusHead = 0x00010000;
    AmcEXIImm(&statusHead, 2, 1, 0);
    AmcEXISync();
    AmcEXIImm(&status, 2, 0, 0);
    AmcEXISync();

    do {
        AmcEXIImm(&status, 2, 0, 0);
        AmcEXISync();
    } while (!((status >> 16) & 1));

    AmcEXIDeselect();
    return AMC_EXI_NO_ERROR;
}

void EXI2_Reserve(void)
{
}

void EXI2_Unreserve(void)
{
}
