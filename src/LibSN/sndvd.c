/*
 * LibSN - sndvd.c
 *
 * Emulacion del lector de DVD del ProDG Target Manager: intercepta con el
 * DABR los accesos a los registros del DI (0xCC006000..) y los sirve por el
 * canal del depurador.
 *
 * ESTA unidad SI es SN ProDG (GCC 2.9): su .s lleva `gcc2_compiled.` y usa
 * lis/@l (o sea -G0) para los estaticos.  Las de al lado (vm, vmbase,
 * metrotrk) son Metrowerks.
 *
 * El orden de las funciones es el del .s del troceador.
 */

typedef unsigned long u32;
typedef signed long s32;
typedef unsigned char u8;

#define DI_SR (*(volatile u32 *)0xCC006000)
#define DI_CVR (*(volatile u32 *)0xCC006004)
#define DI_CMDBUF0 (*(volatile u32 *)0xCC006008)
#define DI_CMDBUF1 (*(volatile u32 *)0xCC00600C)
#define DI_CMDBUF2 (*(volatile u32 *)0xCC006010)
#define DI_MAR (*(volatile u32 *)0xCC006014)
#define DI_LENGTH (*(volatile u32 *)0xCC006018)
#define DI_CR (*(volatile u32 *)0xCC00601C)
#define DI_IMMBUF (*(volatile u32 *)0xCC006020)

#define DI_REGS_BASE 0x0C006000
#define DI_REGS_COUNT 10

typedef struct OSContext {
    u32 gpr[32];    /* 0x000 */
    u32 cr;         /* 0x080 */
    u32 lr;         /* 0x084 */
    u32 ctr;        /* 0x088 */
    u32 xer;        /* 0x08C */
    double fpr[32]; /* 0x090 */
    u32 fpscr_pad;  /* 0x190 */
    u32 fpscr;      /* 0x194 */
    u32 srr0;       /* 0x198 */
    u32 srr1;       /* 0x19C */
    unsigned short mode;  /* 0x1A0 */
    unsigned short state; /* 0x1A2 */
    u32 gqr[8];     /* 0x1A4 */
    u32 psf_pad;    /* 0x1C4 */
    double psf[32]; /* 0x1C8 */
} OSContext;        /* 0x2C8 */

extern void OSReport(const char *msg, ...);
extern void OSLoadContext(OSContext *context);
extern void __OSSetExceptionHandler(int exception, void *handler);
extern int PCreadAsyncInit(int handle, u32 dst, u32 length,
                           void (*callback)(int), u32 offset, int flags);
extern void DSIentry(void);

/* Los datos de la unidad, en el orden del DOL: .data 0x804397E0 (a cero) y
 * .bss 0x804B4F80.  */
static int g_hDVD = 0;

static int g_nDvdAudioCfg;
static u8 g_aBuffer[0x20];
static int g_nDvdError;
static int g_nDvdCurrOffset;
static int g_nDvdReadLength;
static int g_nBaseOffset;
static u8 g_nEmuState;
static u8 g_nLidState;

extern u32 SN_DSI;

static u32 DisDvdBP(void);
static u32 EnaDvdBP(void);
static void ForceDvdTcIrq(u32 addr);
static void ForceDvdDeIrq(void);
static void DvdCallback(int length);
static int CheckSeekOffset(int offset);
extern void DSIExcHandler(void);
static void DSIHandler(int exception, OSContext *context, u32 dsisr, u32 dar);

void SNDVDEmuInit(int handle);
void SNDVDEmuInitDSIHandler(void);
u32 SNDVDEmuControl(int cmd);

static u32 DisDvdBP(void)
{
    volatile u32 dabr = 0;
    u32 v = dabr;

    __asm__ __volatile__ ("mtspr 1013, %0\n\tisync" : : "r" (v));
    return v;
}

static u32 EnaDvdBP(void)
{
    volatile u32 dabr = 0xCC00601E;
    u32 v = dabr;

    __asm__ __volatile__ ("mtspr 1013, %0\n\tisync" : : "r" (v));
    return v;
}

static void ForceDvdTcIrq(u32 addr)
{
    DisDvdBP();
    DI_CMDBUF0 = 0x12000000;
    DI_CMDBUF1 = 0;
    DI_CMDBUF2 = 0x20;
    DI_LENGTH = 0x20;
    DI_MAR = addr;
    DI_CR = 3;
    EnaDvdBP();
}

static void ForceDvdDeIrq(void)
{
    DisDvdBP();
    DI_CMDBUF0 = 0;
    DI_CR = 1;
    EnaDvdBP();
}

static void DvdCallback(int length)
{
    int done = (length + 0x1F) & ~0x1F;

    if (done == g_nDvdReadLength) {
        g_nDvdError = 0;
        g_nDvdCurrOffset += done;
        ForceDvdTcIrq((u32)g_aBuffer);
    } else {
        g_nDvdCurrOffset = -1;
        g_nDvdError = 0x00031100;
        ForceDvdDeIrq();
    }
}

static int CheckSeekOffset(int offset)
{
    if (offset > 0x5705FFFF) {
        return 0x00052100;
    }
    return 0;
}

/* NO TIENE FORMA EN C, Y ESTA MEDIDO (ronda 24).
 *
 * Esto es el VECTOR de excepcion del DSI, no una funcion: el despachador de
 * OSError entra aqui con r4 apuntando al OSContext y con r0, r1, r2 y r6..r31
 * todavia cargados con los valores DEL PROGRAMA INTERRUMPIDO, que es
 * justamente lo que guarda en el contexto (`stw 0,0(4)`, `stw 1,4(4)`,
 * `stmw 6,0x18(4)`).  Las tres cosas que hace son inexpresables:
 *
 *  1. leer r1 y r2 como VALORES.  Son el puntero de pila y la base de datos
 *     pequenos: GCC los ha usado ya antes de la primera sentencia, y un
 *     `register u32 x __asm__("r1")` no lee el r1 del programa interrumpido.
 *  2. los SPR de supervisor: `mfdsisr`, `mfdar`, `mtsrr0/1`, `mtcrf 255`,
 *     `mfspr GQR1..GQR7`.  No hay intrinsecos en este compilador.
 *  3. las DOS ramas de cola, `b DSIHandler` y `b DSIentry`.  Medido con el
 *     propio ngccc y los cflags de esta unidad: `void h(void) {
 *     DSIHandler(0,0,0,0); }` sale de DOCE palabras --`stwu r1,-8(r1)`,
 *     `mflr r0`, `stw r0,12(r1)`, los cuatro `li`, el `bl` y cuatro de
 *     epilogo-- contra las CINCO del objetivo, y ponerle `for(;;)` detras
 *     deja el prologo igual y cambia el `blr` por `b .`: GCC 2.9 no emite
 *     llamadas finales, que es la misma razon por la que los siete alias de
 *     4 B de ppc2D2 se quedaron como estan.
 *
 * Asi que se queda como `__asm__` de nivel de fichero con etiqueta --por eso
 * `NotDvdDsi` sale como simbolo local suelto en el .s del troceador--, pero
 * ANOTADO: esta funcion NO esta decompilada, esta transcrita. */
__asm__ (
        "	.type	DSIExcHandler,@function\n"
        "DSIExcHandler:\n"
        "	mfdsisr	5\n"
        "	extrwi.	5, 5, 1, 9\n"
        "	beq	NotDvdDsi\n"
        "	stw	0, 0(4)\n"
        "	stw	1, 4(4)\n"
        "	stw	2, 8(4)\n"
        "	stmw	6, 0x18(4)\n"
        "	mfspr	0, 913\n"
        "	stw	0, 0x1a8(4)\n"
        "	mfspr	0, 914\n"
        "	stw	0, 0x1ac(4)\n"
        "	mfspr	0, 915\n"
        "	stw	0, 0x1b0(4)\n"
        "	mfspr	0, 916\n"
        "	stw	0, 0x1b4(4)\n"
        "	mfspr	0, 917\n"
        "	stw	0, 0x1b8(4)\n"
        "	mfspr	0, 918\n"
        "	stw	0, 0x1bc(4)\n"
        "	mfspr	0, 919\n"
        "	stw	0, 0x1c0(4)\n"
        "	mfdsisr	5\n"
        "	mfdar	6\n"
        "	b	DSIHandler\n"
        "NotDvdDsi:\n"
        "	lwz	3, 0x80(4)\n"
        "	mtcrf	255, 3\n"
        "	lwz	3, 0x84(4)\n"
        "	mtlr	3\n"
        "	lwz	3, 0x88(4)\n"
        "	mtctr	3\n"
        "	lwz	3, 0x8c(4)\n"
        "	mtxer	3\n"
        "	lwz	3, 0x198(4)\n"
        "	mtsrr0	3\n"
        "	lwz	3, 0x19c(4)\n"
        "	mtsrr1	3\n"
        "	lwz	3, 0xc(4)\n"
        "	lwz	5, 0x14(4)\n"
        "	lwz	4, 0x10(4)\n"
        "	b	DSIentry\n"
        "	li	3, 0\n"
        "	li	4, 0\n"
        "	li	5, 0\n"
        "	li	6, 0\n"
        "	bl	DSIHandler\n"
        "	.size	DSIExcHandler,.-DSIExcHandler\n");

static void DSIHandler(int exception, OSContext *context, u32 dsisr, u32 dar)
{
    u32 regs[DI_REGS_COUNT];
    int i;
    u32 insn;
    u32 ea;
    int cmd;
    u32 offset;

    insn = *(u32 *)context->srr0;
    ea = insn & 0xFFFF;
    if (insn & 0x8000) {
        ea |= 0xFFFF0000;
    }
    ea += context->gpr[(insn >> 16) & 0x1F];

    if (dar != 0xCC00601C) {
        goto emulate;
    }

    for (i = 0; i < DI_REGS_COUNT; i++) {
        regs[i] = *(volatile u32 *)((DI_REGS_BASE + i * 4) | 0xC0000000);
    }

    cmd = regs[2] >> 16;
    offset = g_nBaseOffset + (regs[3] << 2);

    switch (cmd) {
    case 0x1200:
        ForceDvdTcIrq(regs[5]);
        break;

    case 0xA800:
        if (regs[6] == 0) {
            ForceDvdTcIrq((u32)g_aBuffer);
        } else {
            g_nDvdError = CheckSeekOffset(offset);
            if (g_nDvdError != 0) {
                ForceDvdDeIrq();
                break;
            }
            g_nDvdReadLength = regs[6];
            if (PCreadAsyncInit(g_hDVD, regs[5], regs[6], DvdCallback,
                                offset, 1) == -1) {
                g_nDvdError = 0x00031100;
            }
        }
        if (g_nDvdError != 0) {
            ForceDvdDeIrq();
        }
        break;

    case 0xE000:
        DI_IMMBUF = g_nDvdError;
        g_nDvdError &= 0xFF000000;
        ForceDvdTcIrq((u32)g_aBuffer);
        break;

    case 0xE100:
    case 0xE200:
    case 0xE401:
        if (g_nDvdAudioCfg != 0) {
            goto emulate;
        }
        /* fallthrough */
    case 0xAB00:
    case 0xE300:
        ForceDvdTcIrq((u32)g_aBuffer);
        break;

    default:
        OSReport("Unknown DVD cmd (%08X)\n", regs[2]);
        /* El objetivo lleva aqui una PALABRA DE DATOS dentro del .text,
         * 0x00000001 (opcode ilegal: la trampa del Target Manager de ProDG).
         * No hay construccion de C que la emita --`__builtin_trap()` da
         * `tw 31,0,0` y es noreturn--, asi que no se reproduce: son 4 de los
         * 8 B que le faltan a la funcion. */
        g_nDvdError = 0x00052000;
        ForceDvdDeIrq();
        break;
    }
    goto done;

emulate:
    DisDvdBP();
    *(u32 *)ea = context->gpr[(insn >> 21) & 0x1F];
    EnaDvdBP();

done:
    context->srr0 += 4;
    OSLoadContext(context);
}

void SNDVDEmuInit(int handle)
{
    if (handle == -0x8000) {
        g_hDVD = handle;
        SN_DSI = 0;
    }
}

void SNDVDEmuInitDSIHandler(void)
{
    if (g_hDVD == -0x8000) {
        g_nDvdAudioCfg = 0;
        g_nDvdError = 0;
        g_nDvdCurrOffset = -1;
        g_nBaseOffset = 0;
        g_nLidState = 1;
        g_nEmuState = 1;
        EnaDvdBP();
        __OSSetExceptionHandler(2, (void *)DSIExcHandler);
    }
}

u32 SNDVDEmuControl(int cmd)
{
    u32 result = 0;

    if (g_hDVD != -0x8000) {
        return 0;
    }

    switch (cmd) {
    case 0:
        g_nLidState = 0;
        break;
    case 1:
        g_nLidState = 1;
        break;
    case 2:
        g_nEmuState = 0;
        DisDvdBP();
        result = 0x80000000;
        break;
    case 3:
        g_nEmuState = 1;
        EnaDvdBP();
        result = 0x80000000;
        break;
    }

    return result | (g_nEmuState << 8) | g_nLidState;
}
