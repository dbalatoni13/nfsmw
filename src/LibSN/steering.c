
/*
 * SN Systems LibSN - Logitech steering wheel driver (allsrc.c).
 *
 * Metrowerks CodeWarrior (GC/2.7), NOT SN ProDG GCC: the unit has no
 * `gcc2_compiled.` marker and its function statics are mangled `name$NNN`
 * (jumbleeffectid$1040, createcount$1041, a$1517, ia$1518).
 *
 * Device state lives in g_lgDevices[4], stride 0x1248.  The SI* entry points
 * are the Dolphin SDK steering API; SISteeringStatus is the SDK type.
 */

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned long u32;
typedef long s32;

#define NULL 0

/* ------------------------------------------------------------------ */
/* Dolphin SDK                                                         */
/* ------------------------------------------------------------------ */

typedef struct SISteeringStatus {
    u16 button;   /* 0x00 */
    u8 misc;      /* 0x02 */
    s8 steering;  /* 0x03 */
    u8 gas;       /* 0x04 */
    u8 brake;     /* 0x05 */
    u8 left;      /* 0x06 */
    u8 right;     /* 0x07 */
    s8 err;       /* 0x08 */
} SISteeringStatus;

typedef void (*SISteeringCallback)(s32 chan, s32 err);

s32 SIResetSteeringAsync(s32 chan, SISteeringCallback callback);
s32 SIReadSteering(s32 chan, SISteeringStatus *status);
void (*SISetSteeringSamplingCallback(void (*callback)(void)))(void);
void SIControlSteering(s32 chan, u32 control, s32 level);
u32 OSGetTick(void);

#include <string.h>
double pow(double x, double y);
double sin(double x);

#define OS_BUS_CLOCK (*(u32 *)0x800000F8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

/* ------------------------------------------------------------------ */
/* Driver types                                                        */
/* ------------------------------------------------------------------ */

#define LG_OK 0
#define LG_ERR_PARAM (-2)
#define LG_ERR_ALREADY (-3)
#define LG_ERR_NODEVICE (-4)
#define LG_ERR_NOSLOT (-5)

#define LG_MAX_CHAN 4
#define LG_MAX_EFFECTS 64
#define LG_FILTER_TAPS 10
#define LG_HISTORY 16
#define LG_TABLE_SIZE 64
#define LG_GAMMA_SIZE 129

#define PI 3.1415927f

#define MINV(a, b) ((a) < (b) ? (a) : (b))
#define MAXV(a, b) ((a) > (b) ? (a) : (b))

#define CLAMPW(dst, x, lo, hi)  \
    {                           \
        int _w;                 \
        _w = MINV((hi), (x));   \
        (dst) = MAXV((lo), _w); \
    }

#define CLAMPV(dst, x, lo, hi)  \
    {                           \
        int _v;                 \
        _v = MAXV((lo), (x));   \
        (dst) = MINV((hi), _v); \
    }

#define CLAMPHI(dst, x, hi)     \
    {                           \
        int _v;                 \
        _v = (x);               \
        (dst) = MINV((hi), _v); \
    }

/* rounded (a*b)/c */
#define MULDIV(dst, a, b, c) \
    { \
        int _m; \
        int _n; \
        int _d; \
        int _h; \
        _n = (s32)(a); \
        _m = (s32)(b); \
        _d = (s32)(c); \
        _h = _d / 2; \
        if (_n < 0) _h = -_h; \
        (dst) = (_n * _m + _h) / _d; \
    }

/* the cooked report: an SISteeringStatus with the combined pedal axis added */
typedef struct LGSTATUS {
    u16 button;   /* 0x00 */
    u8 misc;      /* 0x02 */
    s8 steering;  /* 0x03 */
    u8 gas;       /* 0x04 */
    u8 brake;     /* 0x05 */
    s8 pedal;     /* 0x06 */
    u8 left;      /* 0x07 */
    u8 right;     /* 0x08 */
    s8 err;       /* 0x09 */
} LGSTATUS;

typedef struct LGCALIB {
    LGSTATUS min;           /* 0x00 */
    LGSTATUS max;           /* 0x0A */
    LGSTATUS dead;          /* 0x14 */
    SISteeringStatus prev;  /* 0x1E */
    s32 settle;             /* 0x28 */
} LGCALIB;

typedef struct LGENVELOPE {
    u32 attackTime;  /* 0x00 */
    u32 fadeTime;    /* 0x04 */
    u8 attackLevel;  /* 0x08 */
    u8 fadeLevel;    /* 0x09 */
} LGENVELOPE;

typedef struct LGCONDITION {
    s8 center;    /* 0x00 */
    u8 deadband;  /* 0x01 */
    u8 negCoeff;  /* 0x02 */
    u8 posCoeff;  /* 0x03 */
    s16 negSat;   /* 0x04 */
    s16 posSat;   /* 0x06 */
} LGCONDITION;

typedef struct LGEFFECT {
    u8 type;      /* 0x00 */
    u32 duration; /* 0x04 */
    u32 delay;    /* 0x08 */
    union {
        struct {
            s16 magnitude;  /* 0x0C */
            u16 direction;  /* 0x0E */
            LGENVELOPE env; /* 0x10 */
        } constant;
        struct {
            s16 start;      /* 0x0C */
            s16 end;        /* 0x0E */
            u16 direction;  /* 0x10 */
        } ramp;
        struct {
            u8 magnitude;   /* 0x0C */
            u16 direction;  /* 0x0E */
            u16 period;     /* 0x10 */
            u16 phase;      /* 0x12 */
            s16 offset;     /* 0x14 */
            LGENVELOPE env; /* 0x18 */
        } periodic;
        LGCONDITION cond;   /* 0x0C */
    } u;
} LGEFFECT;

typedef struct EFFECT {
    LGEFFECT e;           /* 0x00 */
    struct VDEVICE *dev;  /* 0x24 */
    u32 flags;            /* 0x28 */
    u32 elapsed;          /* 0x2C */
    u32 delayLeft;        /* 0x30 */
    s32 force;            /* 0x34 */
    s32 phase;            /* 0x38 */
    s32 phase2;           /* 0x3C */
} EFFECT;

#define EF_PLAYING 0x01
#define EF_STARTED 0x02
#define EF_POLAR 0x04
#define EF_CONSTANT 0x10
#define EF_RAMP 0x20
#define EF_PERIODIC 0x40
#define EF_CONDITION 0x80

typedef struct SIMTHREAD {
    struct VDEVICE *vdev;      /* 0x00 */
    u32 lastTime;              /* 0x04 */
    s32 pos;                   /* 0x08 */
    s32 posFP;                 /* 0x0C */
    s32 coeff[LG_FILTER_TAPS]; /* 0x10 */
    s32 hist[LG_FILTER_TAPS];  /* 0x38 */
    s32 histIndex;             /* 0x60 */
    s32 filtered;              /* 0x64 */
    s32 dbuf[LG_HISTORY];      /* 0x68 */
    s32 tbuf[LG_HISTORY];      /* 0xA8 */
    s32 bufIndex;              /* 0xE8 */
    s32 lastOutput;            /* 0xEC */
} SIMTHREAD;

typedef struct VDEVICE {
    SIMTHREAD sim;                    /* 0x0000 */
    struct LGDEVICE *dev;             /* 0x00F0 */
    EFFECT *effects[LG_MAX_EFFECTS];  /* 0x00F4 */
    EFFECT pool[LG_MAX_EFFECTS];      /* 0x01F4 */
    s32 gain;                         /* 0x11F4 */
} VDEVICE;

typedef struct LGDEVICE {
    s32 chan;                  /* 0x00 */
    u8 open : 1;               /* 0x04 */
    u8 error : 1;
    struct LGDEVICE *handle;   /* 0x08 */
    SISteeringStatus raw;      /* 0x0C */
    LGCALIB calib;             /* 0x18 */
    LGSTATUS cooked;           /* 0x44 */
    VDEVICE vdev;              /* 0x50 */
} LGDEVICE;

/* ------------------------------------------------------------------ */

static LGDEVICE g_lgDevices[LG_MAX_CHAN];
static u8 g_iGammaLookup[LG_GAMMA_SIZE];
static s16 g_iQuarterSineTable[LG_TABLE_SIZE];
static s16 g_iRampUpTable[LG_TABLE_SIZE];

static int g_bInitialized;
static int g_bGammaInitialized;
static int g_bWaveTablesInitialized;

/* ------------------------------------------------------------------ */

static void SteeringResetCallback(s32 chan, s32 err);
static void HandlePedals(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal);
static void HandleTriggers(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal);
static void CookValues(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal);
static void SteeringSamplingCallback(void);
static void InitDevice(LGDEVICE *dev, s32 chan);
static void VDevice_Init(VDEVICE *vd);
static void VDevice_RecalcGammaTable(void);
static s32 VDevice_DownloadEffect(VDEVICE *vd, s32 *pid, LGEFFECT *params);
static s32 VDevice_UpdateEffect(VDEVICE *vd, s32 id, LGEFFECT *params);
static s32 VDevice_DestroyEffect(VDEVICE *vd, s32 id);
static s32 VDevice_StartEffect(VDEVICE *vd, s32 id);
static s32 VDevice_StopEffect(VDEVICE *vd, s32 id);
static s32 VDevice_Initialize(VDEVICE *vd, LGDEVICE *dev);
static s32 VDevice_Shutdown(VDEVICE *vd);
static s32 VDevice_GetFreeEffect(VDEVICE *vd);
static void Effect_Init(EFFECT *e);
static s32 Effect_UpdateEffect(EFFECT *e, const LGEFFECT *params);
static s32 Effect_StartEffect(EFFECT *e);
static s32 Effect_StopEffect(EFFECT *e);
static s32 Effect_Update(EFFECT *e, u32 dt);
static s32 Effect_PerformEnvelope(EFFECT *e, LGENVELOPE *env, s32 level);
static void Effect_PolarToRect(s32 magnitude, u32 direction, s32 *out);
static s32 Effect_UpdateSpring(EFFECT *e, LGCONDITION *c);
static s32 Effect_UpdateDamper(EFFECT *e, LGCONDITION *c);
static void SimThread_Init(SIMTHREAD *st, VDEVICE *vd);
static s32 SimThread_Step(SIMTHREAD *st);

void LGInit(void);
s32 LGOpen(s32 chan, LGDEVICE **phandle);
s32 LGClose(LGDEVICE *dev);
s32 LGRead(LGSTATUS *out);
s32 LGDownloadForceEffect(LGDEVICE *dev, s32 *pid, LGEFFECT *params);
s32 LGUpdateForceEffect(u32 id, LGEFFECT *params);
s32 LGStartForceEffect(u32 id);
s32 LGStopForceEffect(u32 id);
s32 LGDestroyForceEffect(u32 id);

/* the device handle is invalid when NULL or (LGDEVICE *)-1 */
#define LG_VALID(d) ((d) != NULL && (d) != (LGDEVICE *)-1)

/* LGClose's body, expanded where the disconnect is handled inline */
#define LG_SHUTDOWN(d)                          \
    if (LG_VALID(d)) {                          \
        if ((d)->open) {                        \
            (d)->open = 0;                      \
            (d)->error = 0;                     \
            VDevice_Shutdown(&(d)->vdev);       \
            (d)->handle = (LGDEVICE *)-1;       \
        }                                       \
    }

/* auto-calibrating axis cook */
#define AUTOCAL(T, vv, mn, mx, dz, bias, lo, hi, dst) \
    { \
        int _mn; \
        int _v; \
        int _mx; \
        T _dz; \
        int _r; \
        int _t; \
        _v = (vv); \
        _mn = (mn); \
        _dz = (dz); \
        _mx = (mx); \
        if (_v < _mn) { (mn) = _v; _mn = _v; } \
        if (_v > _mx) { (mx) = _v; _mx = _v; } \
        _r = (_mx - _mn) - _dz * 2; \
        if (_r == 0) _r = 1; \
        _t = ((_v - (_mn + _dz)) * 255) / _r - (bias); \
        if (_t < (lo)) _t = (lo); \
        if (_t > (hi)) _t = (hi); \
        (dst) = _t; \
    }

#define AUTOCALX(T, vv, mn, mx, mxv, dz, bias, lo, hi, dst) \
    { \
        int _mn; \
        int _v; \
        T _dz; \
        int _r; \
        int _t; \
        _v = (vv); \
        _mn = (mn); \
        _dz = (dz); \
        if (_v < _mn) { (mn) = _v; _mn = _v; } \
        if (_v > (mxv)) { (mx) = _v; (mxv) = _v; } \
        _r = ((mxv) - _mn) - _dz * 2; \
        if (_r == 0) _r = 1; \
        _t = ((_v - (_mn + _dz)) * 255) / _r - (bias); \
        if (_t < (lo)) _t = (lo); \
        if (_t > (hi)) _t = (hi); \
        (dst) = _t; \
    }

#define SET_PEDALS(p, g, b)                              \
    {                                                    \
        int _v;                                          \
        (p)->gas = (g);                                  \
        (p)->brake = (b);                                \
        _v = ((p)->gas - (p)->brake) / 2;                \
        if (_v < -128) _v = -128;                        \
        else if (_v > 127) _v = 127;                     \
        (p)->pedal = _v;                                 \
    }

/* degrees -> 0..255 */
#define DEG2BYTE(a) ((((a) % 360) << 8) / 360)

/* ------------------------------------------------------------------ */

static void SteeringResetCallback(s32 chan, s32 err)
{
    if (err != 0) {
        g_lgDevices[chan].cooked.err = -1;
    }
}

static void HandlePedals(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal)
{
    if ((raw->misc ^ cal->prev.misc) & 8) {
        cal->settle = 0x40;
        SET_PEDALS(&cal->min, 0x50, 0x50)
        SET_PEDALS(&cal->max, 0xB0, 0xB0)
        cal->dead.gas = 10;
        cal->dead.brake = 10;
    }

    if (cal->settle > 0) {
        cal->settle--;
        out->brake = 0;
        out->gas = 0;
        out->pedal = 0;
        return;
    }

    if (raw->misc & 8) {
        AUTOCAL(u8, raw->gas, cal->min.gas, cal->max.gas, cal->dead.gas, 0, 0, 255, out->gas)
        AUTOCAL(u8, raw->brake, cal->min.brake, cal->max.brake, cal->dead.brake, 0, 0, 255, out->brake)
        SET_PEDALS(out, out->gas, out->brake)
    } else {
        out->brake = 0;
        out->gas = 0;
        out->pedal = 0;
    }
}

/* HandleTriggers -- 588 B, 588/588, 42 filas, 98,299 %. TODO reparto: ni una
 * palabra estructural (r63-veredicto-perfil).
 * r22: las 24 permutaciones del orden de declaracion de locales, con el minimo
 * alcanzado, mas 22 pragmas por funcion. r48: decl vl/mxl/vr/mxr 98,29->97,14
 * PEOR, swap asign vl/mxl NEUTRO.
 * r63, MEDIDO Y NEGATIVO: los 12 registros bloqueados (`asm { li rN, 0 }`,
 * r3..r12 + r14/r15) -- la mejor es 50 filas (r9) contra 42 de la base, y
 * r14/r15 ademas crecen la funcion a 604 B; y los 23 pragmas legales, todos
 * neutros o peores (`scheduling off` 42->52, `peephole off` 588->668 B,
 * `opt_common_subs off` y `global_optimizer off` 588->604 B).
 * Queda como la mas cercana a FIRME de las seis: eje de fuente barrido por r22,
 * eje de reparto barrido por r63, eje de pragma barrido por r63.
 */
static void HandleTriggers(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal)
{
    int mxl;
    int mxr;
    int vl;
    int vr;

    vl = raw->left;
    mxl = cal->max.left;
    if (vl > mxl - cal->dead.left * 2) {
        if ((raw->button ^ cal->prev.button) & 0x40) {
            cal->max.left = MAXV(0, vl - cal->dead.left);
            out->left = 255;
        } else if (raw->button & 0x40) {
            out->left = 255;
        } else {
            AUTOCALX(u8, vl, cal->min.left, cal->max.left, mxl, cal->dead.left, 0, 0, 255, out->left)
        }
    } else {
        AUTOCALX(u8, vl, cal->min.left, cal->max.left, mxl, cal->dead.left, 0, 0, 255, out->left)
    }

    vr = raw->right;
    mxr = cal->max.right;
    if (vr > mxr - cal->dead.right * 2) {
        if ((raw->button ^ cal->prev.button) & 0x20) {
            cal->max.right = MAXV(0, vr - cal->dead.right);
            out->right = 255;
        } else if (raw->button & 0x20) {
            out->right = 255;
        } else {
            AUTOCALX(u8, vr, cal->min.right, cal->max.right, mxr, cal->dead.right, 0, 0, 255, out->right)
        }
    } else {
        AUTOCALX(u8, vr, cal->min.right, cal->max.right, mxr, cal->dead.right, 0, 0, 255, out->right)
    }
}

/* CookValues -- 260 B, 260/260, 9 filas, 99,077 %. LA MAS BARATA DE LAS SEIS.
 * OJO CON LA CIFRA: report.json dice 95,77 % y esta RANCIO. Lo que hay aqui es
 * el `cook_lo` de scratchpad/codex_20260908_iter4_steering, que mide 99,077 %.
 * El aviso de r60b de que "cook_lo no esta en el arbol" es FALSO.
 * LAS 9 FILAS SON UNA SOLA TRANSPOSICION r4<->r6: el objetivo pone _mn en r4 y
 * _v en r6, nosotros al reves. Mismas instrucciones, mismo orden, mismo tamano.
 *     objetivo                   nuestro
 *     lbz   r4, 0x3(r31)  _mn    lbz   r6, 0x3(r31)
 *     extsb r6, r5        _v     extsb r4, r5
 *     extsb r4, r4               extsb r6, r6
 *     cmpw  r6, r4               cmpw  r4, r6
 *     mr    r4, r6               mr    r6, r4
 * r48: AUTOCAL manual _v-primero fija la fila 7 pero 95,76 % igual, _mn-primero
 * 95,53 % peor, (_dz+_mn) 95,84 %. iter4/iter5: cook_bytes 15 filas, cook_extsb
 * 22 y 264 B, range_subf 21, scale_subf y cook_order OBJETO IDENTICO a la base.
 * r63, MEDIDO Y NEGATIVO, tres barridos con sello de sha1:
 *   - LAS 24 PERMUTACIONES del orden de las asignaciones iniciales (_v, _mn,
 *     _dz, _mx detras de _bits) dan UN SOLO OBJETO, 1962b7d1, byte a byte. Esto
 *     REFUTA para este caso la linea de `nfsmw-catalogo-metrowerks` que dice que
 *     el orden de las asignaciones dentro de una rama le mueve: aqui MWCC lo
 *     normaliza entero.
 *   - 13 FORMAS Y TIPOS: `register` en _mn, quitar `register` de _v, `long` en
 *     _v y en _mn, _bits declarado el primero, _dz a int, _mn con doble cast, _v
 *     leido de raw en vez de _bits. OCHO dan el objeto IDENTICO; las cinco que
 *     mueven empeoran (15, 19, 19, 21 filas y una que crece a 268 B).
 *   - LOS 63 SUBCONJUNTOS de {r3..r9} de tamano 1, 2 y 3 con `asm { li rN, 0 }`:
 *     minimo 9 filas, el de la base. `li r6,0` mueve _mn de r6 a r7 sin cambiar
 *     el tamano; `li r4,0` da 17 filas; `li r3,0` 32; `li r5,0` 20; r7..r12 son
 *     el objeto de la base.
 * LA PISTA QUE QUEDA, y es la unica: con `asm { li r4, 0 }` MWCC reparte _mn
 * ANTES que _bits (carga cal->min la primera, en r7). O sea que EL ORDEN DE
 * REPARTO DE ESTA FUNCION SI SE PUEDE VOLTEAR; lo que no se ha encontrado es
 * como voltearlo dejando los registros en r4/r5/r6.
 * r68 (directo): VEDA CON PRUEBA DE IMPOSIBILIDAD (sin compilar: se deduce de
 * lo medido). _v[9->29] (5 refs) y _mn[8->29] (6 refs) se solapan casi
 * enteros: TIENEN que ir a regs distintos, y el unico orden posible es quien
 * asigne primero se lleva r4 (libre entonces; r5 lo ocupa _bits). Para que _mn
 * coja r4 tiene que asignar ANTES que _v, y para eso su pri tendria que
 * superar la de _v con los mismos usos (hasta tiene UN ref mas y aun asi va
 * despues: el orden de MWCC no es por refs). Tocar usos rompe tamano (260
 * clavado) o arbol. Un tercero no puede separarlos (cualquier ocupante de r4
 * en la ventana 8-29 bloquea a LOS DOS, y si muere antes de uno muere antes
 * de los dos por el solape). Los clobbers emiten (MWCC no tiene fantasma de
 * coste cero como GCC) y los 63 medidos no dan r4/r6. Aparcado: 9 filas.
 */
static void CookValues(LGSTATUS *out, SISteeringStatus *raw, LGCALIB *cal)
{
    {
        int _mn;
        register int _v;
        register int _mx;
        s8 _dz;
        int _r;
        int _t;
        register u32 _bits;
        _bits = (u8)raw->steering;
        _v = (s8)_bits;
        _mn = cal->min.steering;
        _dz = cal->dead.steering;
        _mx = cal->max.steering;
        if (_v < _mn) { cal->min.steering = (s8)_bits; _mn = _v; }
        if (_v > _mx) {
            cal->max.steering = (s8)_bits;
            asm { extsb _mx, _bits }
        }
        _r = (_mx - _mn) - _dz * 2;
        if (_r == 0) _r = 1;
        _t = ((_v - (_mn + _dz)) * 255) / _r - 127;
        if (_t < -128) _t = -128;
        if (_t > 127) _t = 127;
        out->steering = _t;
    }

    HandlePedals(out, raw, cal);
    HandleTriggers(out, raw, cal);

    out->button = raw->button;
    out->misc = raw->misc;
    cal->prev = *raw;
    out->err = 0;
}

static void SteeringSamplingCallback(void)
{
    s32 i;
    LGDEVICE *dev;
    SISteeringStatus *raw;

    for (i = 0; i < LG_MAX_CHAN; i++) {
        dev = &g_lgDevices[i];
        if (dev->open) {
            raw = &dev->raw;
            SIReadSteering(i, raw);
            switch (raw->err) {
            case -1:
                dev->cooked.err = -1;
                dev->error = 1;
                break;
            case 0:
                CookValues(&dev->cooked, raw, &dev->calib);
                SimThread_Step(&dev->vdev.sim);
                break;
            }
        }
    }
}

static void InitDevice(LGDEVICE *dev, s32 chan)
{
    dev->chan = chan;
    dev->open = 0;
    dev->error = 0;
    dev->handle = (LGDEVICE *)-1;

    dev->raw.button = 0;
    dev->raw.misc = 0;
    dev->raw.steering = 0;
    dev->raw.gas = 0x80;
    dev->raw.brake = 0x80;
    dev->raw.left = 0x80;
    dev->raw.right = 0x80;
    dev->raw.err = -1;

    dev->calib.min.steering = -0x40;
    dev->calib.min.left = 0x50;
    dev->calib.min.right = 0x50;

    dev->calib.max.steering = 0x40;
    dev->calib.max.left = 0xB0;
    dev->calib.max.right = 0xB0;

    dev->calib.dead.steering = 6;
    dev->calib.dead.left = 12;
    dev->calib.dead.right = 12;

    SET_PEDALS(&dev->calib.min, 0x50, 0x50)
    SET_PEDALS(&dev->calib.max, 0xB0, 0xB0)

    dev->calib.dead.gas = 10;
    dev->calib.dead.brake = 10;

    CookValues(&dev->cooked, &dev->raw, &dev->calib);
}

void LGInit(void)
{
    LGDEVICE *dev;
    s32 i;

    if (!g_bInitialized) {
        for (i = 0; i < LG_MAX_CHAN; i++) {
            dev = &g_lgDevices[i];
            VDevice_Init(&dev->vdev);
            InitDevice(dev, i);
            SIResetSteeringAsync(i, SteeringResetCallback);
            SIReadSteering(i, &dev->raw);
        }
        SISetSteeringSamplingCallback(SteeringSamplingCallback);
        g_bInitialized = 1;
    }
}

s32 LGOpen(s32 chan, LGDEVICE **phandle)
{
    LGDEVICE *dev;
    SISteeringStatus *raw;

    if (chan >= LG_MAX_CHAN || chan < 0) {
        return LG_ERR_PARAM;
    }
    if (phandle == NULL) {
        return LG_ERR_PARAM;
    }

    dev = &g_lgDevices[chan];

    if (dev->error) {
        LGClose(dev);
    }
    if (dev->open) {
        *phandle = dev->handle;
        return LG_ERR_ALREADY;
    }
    if (SIResetSteeringAsync(chan, SteeringResetCallback) != -1) {
        InitDevice(dev, chan);
        raw = &dev->raw;
        SIReadSteering(chan, raw);

        switch (g_lgDevices[chan].raw.err) {
        case -1:
            g_lgDevices[chan].cooked.err = -1;
            return LG_ERR_NODEVICE;
        case 0:
            CookValues(&dev->cooked, raw, &dev->calib);
            break;
        }

        g_lgDevices[chan].cooked.err = 0;
        *phandle = dev;
        dev->chan = chan;
        dev->handle = *phandle;
        VDevice_Initialize(&dev->vdev, dev);
        dev->open = 1;
        return LG_OK;
    }
    return LG_ERR_NODEVICE;
}

s32 LGClose(LGDEVICE *dev)
{
    if (!LG_VALID(dev)) {
        return LG_ERR_PARAM;
    }
    if (!dev->open) {
        return LG_ERR_PARAM;
    }
    dev->open = 0;
    dev->error = 0;
    VDevice_Shutdown(&dev->vdev);
    dev->handle = (LGDEVICE *)-1;
    return LG_OK;
}

s32 LGRead(LGSTATUS *out)
{
    s32 i;
    LGDEVICE *dev;

    if (out == NULL) {
        return LG_ERR_PARAM;
    }

    for (i = 0; i < LG_MAX_CHAN; i++) {
        dev = &g_lgDevices[i];
        if (dev->error) {
            LG_SHUTDOWN(dev)
        }
        if (dev->open) {
            out[i] = dev->cooked;
        } else {
            out[i].err = -1;
        }
    }
    return LG_OK;
}

/* CSE global apagada: con ella mwcc reparte r3/r7 al reves que el objetivo
   en las dos LG_VALID (medido: 99,510% con CSE, 100% sin ella). */
#pragma opt_common_subs off
s32 LGDownloadForceEffect(LGDEVICE *dev, s32 *pid, LGEFFECT *params)
{
    if (!LG_VALID(dev)) {
        return LG_ERR_PARAM;
    }
    if (!dev->open) {
        return LG_ERR_PARAM;
    }
    if (pid == NULL) {
        return LG_ERR_PARAM;
    }
    if (params == NULL) {
        return LG_ERR_PARAM;
    }
    if (dev->error) {
        LG_SHUTDOWN(dev)
        return LG_ERR_PARAM;
    }
    return VDevice_DownloadEffect(&dev->vdev, pid, params);
}
#pragma opt_common_subs on

s32 LGUpdateForceEffect(u32 id, LGEFFECT *params)
{
    LGDEVICE *dev;
    u32 chan;

    chan = id >> 16;
    if (chan >= LG_MAX_CHAN) {
        return LG_ERR_PARAM;
    }

    dev = &g_lgDevices[chan];
    if (!dev->open) {
        return LG_ERR_PARAM;
    }
    if (params == NULL) {
        return LG_ERR_PARAM;
    }
    if (dev->error) {
        LG_SHUTDOWN(dev)
        return LG_ERR_PARAM;
    }
    return VDevice_UpdateEffect(&dev->vdev, id, params);
}

s32 LGStartForceEffect(u32 id)
{
    LGDEVICE *dev;
    u32 chan;

    chan = id >> 16;
    if (chan >= LG_MAX_CHAN) {
        return LG_ERR_PARAM;
    }

    dev = &g_lgDevices[chan];
    if (!dev->open) {
        return LG_ERR_PARAM;
    }
    if (dev->error) {
        LG_SHUTDOWN(dev)
        return LG_ERR_PARAM;
    }
    return VDevice_StartEffect(&dev->vdev, id);
}

s32 LGStopForceEffect(u32 id)
{
    LGDEVICE *dev;
    u32 chan;

    chan = id >> 16;
    if (chan >= LG_MAX_CHAN) {
        return LG_ERR_PARAM;
    }

    dev = &g_lgDevices[chan];
    if (!dev->open) {
        return LG_ERR_PARAM;
    }
    if (dev->error) {
        LG_SHUTDOWN(dev)
        return LG_ERR_PARAM;
    }
    return VDevice_StopEffect(&dev->vdev, id);
}

s32 LGDestroyForceEffect(u32 id)
{
    LGDEVICE *dev;
    u32 chan;

    chan = id >> 16;
    if (chan >= LG_MAX_CHAN) {
        return LG_ERR_PARAM;
    }

    dev = &g_lgDevices[chan];
    if (!dev->open) {
        return LG_ERR_PARAM;
    }
    if (dev->error) {
        LG_SHUTDOWN(dev)
        return LG_ERR_PARAM;
    }
    return VDevice_DestroyEffect(&dev->vdev, id);
}

static void VDevice_Init(VDEVICE *vd)
{
    s32 i;

    SimThread_Init(&vd->sim, vd);
    vd->gain = 255;
    for (i = 0; i < LG_MAX_EFFECTS; i++) {
        vd->effects[i] = NULL;
    }
    if (!g_bGammaInitialized) {
        VDevice_RecalcGammaTable();
        g_bGammaInitialized = 1;
    }
}

static void VDevice_RecalcGammaTable(void)
{
    s32 i;
    register float v;
    float g;
    register double converted;
    register double number, bias;
    register float value;
    union { double d; u32 w[2]; } slot;

    g = 0.6666667f;
    for (i = 0; i < LG_GAMMA_SIZE; i++) {
        if (i < 0) {
            slot.w[1] = (u32)(-i) ^ 0x80000000;
            slot.w[0] = 0x43300000;
            number = slot.d;
            bias = 4503601774854144.0;
            asm { fsubs value, number, bias }
            v = -(0.5f + 128.0f * (float)pow(value / 128.0f, g));
        } else {
            slot.w[1] = (u32)i ^ 0x80000000;
            slot.w[0] = 0x43300000;
            number = slot.d;
            bias = 4503601774854144.0;
            asm { fsubs value, number, bias }
            v = 0.5f + 128.0f * (float)pow(value / 128.0f, g);
        }
        asm { fctiwz converted, v }
        slot.d = converted;
        g_iGammaLookup[i] = (u8)slot.w[1];
    }
}

static s32 VDevice_DownloadEffect(VDEVICE *vd, s32 *pid, LGEFFECT *params)
{
    s32 ret;
    s32 idx;
    EFFECT *e;

    ret = LG_OK;
    *pid = VDevice_GetFreeEffect(vd);
    if ((u32)*pid == 0xFFFFFFFF) {
        ret = LG_ERR_NOSLOT;
    } else {
        idx = *pid & 0x3F;
        e = &vd->pool[idx];

        switch (params->type) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            Effect_Init(e);
            e->dev = vd;
            Effect_UpdateEffect(e, params);
            break;
        default:
            *pid = -1;
            ret = LG_ERR_PARAM;
            break;
        }

        if (e != NULL) {
            vd->effects[idx] = e;
        }
    }
    return ret;
}

static s32 VDevice_UpdateEffect(VDEVICE *vd, s32 id, LGEFFECT *params)
{
    EFFECT *e;
    s32 ret;

    ret = LG_ERR_PARAM;
    e = vd->effects[id & 0x3F];
    if (e != NULL) {
        ret = Effect_UpdateEffect(e, params);
    }
    return ret;
}

static s32 VDevice_DestroyEffect(VDEVICE *vd, s32 id)
{
    s32 ret;

    ret = LG_ERR_PARAM;
    if (vd->effects[id & 0x3F] != NULL) {
        vd->effects[id & 0x3F] = NULL;
        ret = LG_OK;
    }
    return ret;
}

static s32 VDevice_StartEffect(VDEVICE *vd, s32 id)
{
    EFFECT *e;
    s32 ret;

    ret = LG_ERR_PARAM;
    e = vd->effects[id & 0x3F];
    if (e != NULL) {
        ret = Effect_StartEffect(e);
    }
    return ret;
}

static s32 VDevice_StopEffect(VDEVICE *vd, s32 id)
{
    EFFECT *e;
    s32 ret;

    ret = LG_ERR_PARAM;
    e = vd->effects[id & 0x3F];
    if (e != NULL) {
        ret = Effect_StopEffect(e);
    }
    return ret;
}

static s32 VDevice_Initialize(VDEVICE *vd, LGDEVICE *dev)
{
    s32 i;

    vd->gain = 255;
    for (i = 0; i < LG_MAX_EFFECTS; i++) {
        vd->effects[i] = NULL;
    }
    vd->dev = dev;
    return LG_OK;
}

static s32 VDevice_Shutdown(VDEVICE *vd)
{
    s32 i;

    for (i = 0; i < LG_MAX_EFFECTS; i++) {
        if (vd->effects[i] != NULL) {
            vd->effects[i] = NULL;
        }
    }
    return LG_OK;
}

static s32 VDevice_GetFreeEffect(VDEVICE *vd)
{
    static volatile s32 jumbleeffectid;
    static volatile s32 createcount;
    s32 j;
    s32 id;
    s32 i;
    s32 idx;
    LGDEVICE *dev;

    j = jumbleeffectid;
    id = -1;
    for (i = 0; i < LG_MAX_EFFECTS; i++) {
        idx = (i + j) & 0x3F;
        if (vd->effects[idx] == NULL) {
            jumbleeffectid = idx + 1;
            jumbleeffectid = jumbleeffectid & 0x3F;
            dev = *(LGDEVICE * volatile *)&vd->dev;
            id = (dev->chan << 16) | ((createcount & 0xFF) << 8) | (idx & 0x3F);
            break;
        }
    }
    return id;
}

/* Effect_Init -- 276 B, 276/276, 23 filas, 95,000 %.
 * OJO CON LA CIFRA: report.json dice 88,62 % y esta RANCIO -- ese 88,62 % es la
 * version SIN NINGUNA de las dos mitades de iter4. El aviso de r60b de que "hay
 * dos mitades medidas por separado que nadie combino" es FALSO: aqui estan las
 * dos (el `clear_value` de effect_clear y el slot/exponent/fsubs de
 * effect_seed), y r63 recompilo las cuatro combinaciones:
 *     las dos (esto)  1962b7d1  23 filas  95,000 %   <- el optimo
 *     solo clear      b4bc4c0a  22 filas  92,478 %
 *     solo seed       8c47f1c4  32 filas  91,145 %
 *     ninguna         8c54714d  31 filas  88,623 %   <- lo que dice report.json
 * r63, MEDIDO Y NEGATIVO: los 12 registros bloqueados (mejor 26 filas con r5;
 * r6..r12 dan el objeto de la base; r14/r15 dan 29) y los 23 pragmas legales
 * (mejor: `optimization_level 2`, que es el que ya tiene, y `opt_lifetimes off`
 * / `opt_common_subs off` / `global_optimizer off`, todos objeto equivalente).
 * `scheduling off` 23->46 filas, `peephole off` 276->300 B, `use_lmw_stmw off`
 * 276->284 B.
 */
#pragma optimization_level 2
static void Effect_Init(EFFECT *e)
{
    u32 k;
    u32 *p;
    u32 n;
    u8 *q;
    s32 i;
    s16 *sine;
    s16 *ramp;
    s32 off;
    s32 phase;
    register u32 exponent;
    register double number, bias;
    register float angle;
    union { double d; u32 w[2]; } slot;
    register u32 clear_value;

    p = (u32 *)e - 1;
    n = (u32)sizeof(EFFECT);
    k = (u32)sizeof(EFFECT) >> 5;
    asm { li clear_value, 0 }
    do {
        p[1] = clear_value;
        p[2] = clear_value;
        p[3] = clear_value;
        p[4] = clear_value;
        p[5] = clear_value;
        p[6] = clear_value;
        p[7] = clear_value;
        p[8] = clear_value;
        p += 8;
    } while (--k);
    n = n & 3;
    q = (u8 *)p + 3;
    if (n != 0) {
        do {
            *++q = 0;
        } while (--n);
    }

    if (!g_bWaveTablesInitialized) {
        sine = g_iQuarterSineTable;
        ramp = g_iRampUpTable;
        off = 0;
        phase = 0;
        i = 0;
        asm { lis exponent, 0x4330 }
        do {
            slot.w[0] = exponent;
            slot.w[1] = (u32)i ^ 0x80000000;
            number = slot.d;
            bias = 4503601774854144.0;
            asm { fsubs angle, number, bias }
            *(s16 *)((u32)sine + off) =
                (s16)(1024.0f * (float)sin(PI * angle / 128.0f));
            i++;
            *(s16 *)((u32)ramp + off) = (s16)(phase / 63);
            phase += 0x400;
            off += 2;
        } while (i < LG_TABLE_SIZE);
        g_bWaveTablesInitialized = 1;
    }
}
#pragma optimization_level 4

static s32 Effect_UpdateEffect(EFFECT *e, const LGEFFECT *params)
{
    s32 ret;
    u8 type;

    ret = LG_OK;
    type = params->type;
    e->e.type = type;
    e->e.duration = params->duration;
    e->e.delay = params->delay;

    switch (type) {
    case 0:
        e->flags |= EF_CONSTANT | EF_POLAR;
        CLAMPV(e->e.u.constant.magnitude, params->u.constant.magnitude, -255, 255)
        e->e.u.constant.direction = DEG2BYTE(params->u.constant.direction);
        e->e.u.constant.env.attackTime = params->u.constant.env.attackTime;
        e->e.u.constant.env.fadeTime = params->u.constant.env.fadeTime;
        e->e.u.constant.env.attackLevel = MINV(255, (int)params->u.constant.env.attackLevel);
        e->e.u.constant.env.fadeLevel = MINV(255, (int)params->u.constant.env.fadeLevel);
        break;
    case 1:
        e->flags |= EF_RAMP | EF_POLAR;
        CLAMPV(e->e.u.ramp.start, params->u.ramp.start, -255, 255)
        CLAMPV(e->e.u.ramp.end, params->u.ramp.end, -255, 255)
        e->e.u.ramp.direction = DEG2BYTE(params->u.ramp.direction);
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        e->flags |= EF_PERIODIC | EF_POLAR;
        e->e.u.periodic.magnitude = MINV(255, (int)params->u.periodic.magnitude);
        e->e.u.periodic.direction = DEG2BYTE(params->u.periodic.direction);
        e->e.u.periodic.period = MAXV(2, params->u.periodic.period);
        e->e.u.periodic.phase = DEG2BYTE(params->u.periodic.phase);
        CLAMPV(e->e.u.periodic.offset, params->u.periodic.offset, -255, 255)
        e->e.u.periodic.env.attackTime = params->u.periodic.env.attackTime;
        e->e.u.periodic.env.fadeTime = params->u.periodic.env.fadeTime;
        e->e.u.periodic.env.attackLevel = MINV(255, (int)params->u.periodic.env.attackLevel);
        e->e.u.periodic.env.fadeLevel = MINV(255, (int)params->u.periodic.env.fadeLevel);
        if (e->e.u.periodic.period < 4) {
            e->e.u.periodic.phase = 0x40;
        }
        break;
    case 7:
    case 8:
        e->flags |= EF_CONDITION;
        e->e.u.cond.center = params->u.cond.center;
        e->e.u.cond.deadband = params->u.cond.deadband;
        CLAMPV(e->e.u.cond.posSat, params->u.cond.posSat, -255, 255)
        CLAMPV(e->e.u.cond.negSat, params->u.cond.negSat, -255, 255)
        e->e.u.cond.negCoeff = params->u.cond.negCoeff / 2;
        e->e.u.cond.posCoeff = params->u.cond.posCoeff / 2;
        e->phase = (e->e.u.cond.center - e->e.u.cond.deadband / 2) << 12;
        e->phase2 = (e->e.u.cond.center + e->e.u.cond.deadband / 2) << 12;
        break;
    default:
        ret = LG_ERR_PARAM;
        break;
    }
    return ret;
}

static s32 Effect_StartEffect(EFFECT *e)
{
    e->flags |= EF_PLAYING;
    e->flags &= ~EF_STARTED;
    e->elapsed = 0;
    e->delayLeft = e->e.delay;
    if (e->flags & EF_PERIODIC) {
        e->phase = e->e.u.periodic.phase << 16;
    }
    return LG_OK;
}

static s32 Effect_StopEffect(EFFECT *e)
{
    e->flags &= ~EF_PLAYING;
    return LG_OK;
}

static s32 Effect_Update(EFFECT *e, u32 dt)
{
    s32 wave;
    s32 level;
    u32 phase;
    s32 active;
    s32 t;
    s32 round;
    s32 v;
    s32 rampv;
    LGCONDITION *c;

    if (e->flags & EF_PLAYING) {
        if (e->delayLeft != 0) {
            e->delayLeft -= dt;
            if ((s32)e->delayLeft < 0) {
                e->delayLeft = 0;
            }
        } else {
            e->elapsed += dt;
            if (e->e.duration != -1) {
                if (e->e.duration < e->elapsed) {
                    if (e->flags & EF_STARTED) {
                        e->flags &= ~EF_PLAYING;
                    }
                }
            }
        }
    }

    e->flags |= EF_STARTED;

    active = 0;
    if ((e->flags & EF_PLAYING) && e->delayLeft == 0) {
        active = 1;
    }

    if (active) {
    if (e->flags & EF_CONSTANT) {
        level = Effect_PerformEnvelope(e, &e->e.u.constant.env, e->e.u.constant.magnitude);
        Effect_PolarToRect(level, e->e.u.constant.direction, &e->force);
    } else if (e->flags & EF_RAMP) {
        {
            int d = e->e.duration;
            int start = *(volatile s16 *)&e->e.u.ramp.start;
            int n = e->e.u.ramp.end - start;
            int m = e->elapsed;
            rampv = d / 2;
            if (n < 0) rampv = -rampv;
            n *= m;
            n += rampv;
            rampv = n / d;
            Effect_PolarToRect(start + rampv, e->e.u.ramp.direction, &e->force);
        }
    } else if (e->flags & EF_PERIODIC) {
        dt <<= 24;
        dt /= e->e.u.periodic.period;
        e->phase += dt;
        if ((u32)e->phase >= 0x1000000) {
            e->phase -= e->phase & 0xFF000000;
        }
        level = (s16)Effect_PerformEnvelope(e, &e->e.u.periodic.env, e->e.u.periodic.magnitude);
        switch (e->e.type) {
        case 2:
            phase = ((u32)e->phase >> 16) & 0xFF;
            if (phase < 0x40) {
                t = g_iQuarterSineTable[phase];
            } else if (phase < 0x80) {
                t = g_iQuarterSineTable[0x7F - phase];
            } else if (phase < 0xC0) {
                t = -g_iQuarterSineTable[phase - 0x80];
            } else {
                t = -g_iQuarterSineTable[0xFF - phase];
            }
            wave = t;
            break;
        case 3:
            wave = -0x400;
            if (((u32)e->phase >> 16) < 0x80) {
                wave = 0x400;
            }
            break;
        case 4:
            phase = ((u32)e->phase >> 16) & 0xFF;
            if (phase < 0x40) {
                t = g_iRampUpTable[phase];
            } else if (phase < 0x80) {
                t = g_iRampUpTable[0x7F - phase];
            } else if (phase < 0xC0) {
                t = -g_iRampUpTable[phase - 0x80];
            } else {
                t = -g_iRampUpTable[0xFF - phase];
            }
            wave = t;
            break;
        case 5:
            phase = ((u32)e->phase >> 16) & 0xFF;
            wave = g_iRampUpTable[phase / 4];
            break;
        case 6:
            phase = ((u32)e->phase >> 16) & 0xFF;
            wave = g_iRampUpTable[(0xFF - phase) / 4];
            break;
        default:
            wave = 0;
            break;
        }
        round = 0x200;
        if (wave < 0) {
            round = -0x200;
        }
        v = wave * level;
        v += round;
        Effect_PolarToRect(v / 1024 + e->e.u.periodic.offset,
                           e->e.u.periodic.direction, &e->force);
    } else if (e->flags & EF_CONDITION) {
        c = &e->e.u.cond;
        switch (e->e.type) {
        case 7:
            e->force = Effect_UpdateSpring(e, c);
            break;
        case 8:
            e->force = Effect_UpdateDamper(e, c);
            if (e->force == 0) {
                e->force++;
            }
            break;
        default:
            e->force = 0;
            break;
        }
    } else {
        return 0;
    }
    } else {
        e->force = 0;
    }
    return 1;
}

/* Effect_PerformEnvelope -- 196 B objetivo, 192 NUESTROS, 31 filas, 93,265 %.
 * VEDA FIRME (r60b). Le falta el `mr r5, r9`: el objetivo tiene `out` y `mag` en
 * registros DISTINTOS y MWCC nos los funde. r22 (10 formas), r51 (14 mas) e
 * iter5 (`asm { mr out, mag }` con los dos operandos `register`, y con registro
 * fisico en hard4/hard6) ya lo dieron por agotado.
 * r63, MEDIDO Y NEGATIVO, y cierra el ultimo eje que quedaba apuntado: los 12
 * registros bloqueados (`asm { li rN, 0 }`) NUNCA LLEGAN A 196 B -- r3 da 33
 * filas, r6/r7 dan 35, r4/r5/r8/r9 se quedan en 31 como la base, r10..r12 dan el
 * objeto de la base, y r14/r15 crecen la funcion a 208 B. Y los 23 pragmas
 * legales, todos neutros o peores (`scheduling off` sigue en 192 B).
 * O sea: bloquear registros no puede FORZAR una copia, solo prohibir un color.
 * Para esta funcion hace falta una forma de fuente donde `out` y `mag`
 * INTERFIERAN de verdad.
 */
static s32 Effect_PerformEnvelope(EFFECT *e, LGENVELOPE *env, s32 level)
{
    s32 mag;
    s32 neg;
    s32 out;
    s32 sustain;
    s32 v;

    out = mag = level < 0 ? -level : level;
    neg = (u32)level >> 31;

    if (e->elapsed < env->attackTime) {
        MULDIV(v, e->elapsed, mag - env->attackLevel, env->attackTime)
        out = env->attackLevel + v;
    } else if (e->e.duration != -1) {
        sustain = e->e.duration - env->fadeTime;
        if (e->elapsed > sustain) {
            if (e->elapsed < e->e.duration) {
                MULDIV(v, e->elapsed - sustain, env->fadeLevel - mag, env->fadeTime)
                out = mag + v;
            } else {
                out = env->fadeLevel;
            }
        }
    }

    if (neg) {
        out = -out;
    }
    return out;
}

static void Effect_PolarToRect(s32 magnitude, u32 direction, s32 *out)
{
    s32 v;
    s32 round;
    s32 product;
    u32 a;

    a = direction & 0xFF;
    if (a < 0x40) {
        v = g_iQuarterSineTable[a];
    } else if (a < 0x80) {
        v = g_iQuarterSineTable[0x7F - a];
    } else if (a < 0xC0) {
        v = -g_iQuarterSineTable[a - 0x80];
    } else {
        v = -g_iQuarterSineTable[0xFF - a];
    }

    v = magnitude * v;
    round = 255 * 1024 / 2;
    if (v < 0) {
        round = -(255 * 1024 / 2);
    }
    product = v * 127;
    product += round;
    *out = -(product / (255 * 1024));
}

static s32 Effect_UpdateSpring(EFFECT *e, LGCONDITION *c)
{
    s32 pos;
    s32 v;
    s32 low;
    s32 coeff;

    pos = e->dev->sim.posFP;
    if (pos < e->phase) {
        v = pos - e->phase;
        v = (v * c->negSat) / 131072;
        coeff = c->negCoeff;
        low = -coeff;
        v = MAXV(low, v);
        v = MINV(coeff, v);
        return -v;
    }
    if (pos > e->phase2) {
        v = pos - e->phase2;
        v = (v * c->posSat) / 131072;
        coeff = c->posCoeff;
        low = -coeff;
        v = MAXV(low, v);
        v = MINV(coeff, v);
        return -v;
    }
    return 0;
}

static s32 Effect_UpdateDamper(EFFECT *e, LGCONDITION *c)
{
    s32 v;
    s32 sat;
    s32 coeff;

    v = e->dev->sim.filtered;
    if (v < 0) {
        sat = -c->negSat;
        coeff = c->negCoeff;
    } else {
        sat = -c->posSat;
        coeff = c->posCoeff;
    }

    v *= sat;
    v = (v * 20) / (255 * 4096);
    v = MAXV(-coeff, v);
    v = MINV(coeff, v);
    return v;
}

/* SimThread_Init -- 324 B, 324/324, 9 filas, 94,691 %.
 * r63 -- EL PRAGMA QUE FALTABA, PROBADO, Y NEGATIVO. Era el ensayo cabeza de
 * serie de la familia E y contesta por las seis:
 *   - `#pragma scheduling off/on` ACOTADO A ESTA FUNCION: 9 filas -> 50. La
 *     planificacion de MWCC aqui ya es casi la buena, asi que apagarla es
 *     catastrofico. El eje de planificacion queda agotado por arriba y por
 *     abajo.
 *   - Y NO SE DERRAMA: con la funcion destrozada a 50 filas, `fncmp` sigue
 *     dando 30 exactas y las mismas 3 solo-alias, antes y despues. Ese era el
 *     observable que pedia el encargo.
 *   - Los otros 22 pragmas legales: `optimization_level 3` objeto identico (el
 *     nivel efectivo ya es 3), `opt_lifetimes off` 17 filas, `peephole off` 43,
 *     `opt_common_subs off` y `global_optimizer off` 324->336 B,
 *     `optimization_level 0/1/2` 428/416/344 B. Ninguno mejora.
 *   - Los 12 registros bloqueados (`asm { li rN, 0 }`): mejor 11 filas (r8)
 *     contra 9 de la base; r9..r12 dan el objeto de la base; r14/r15 dan 15.
 * Con esto, los tres ejes de MWCC --fuente (r47/r48/r62), pragma (r63) y
 * reparto (r63)-- estan barridos en esta funcion. Las 9 filas que quedan son
 * 2 de ranura (el `lfs` del bucle de `ia[]`) y un empate desnudo r5/r6.
 */
static void SimThread_Init(SIMTHREAD *st, VDEVICE *vd)
{
    static float a[LG_FILTER_TAPS] = {
        0.0995469242f, 0.0998378024f, 0.100064509f, 0.100226693f, 0.100324109f,
        0.100324109f, 0.100226693f, 0.100064509f, 0.0998378024f, 0.0995469242f,
    };
    static s32 ia[LG_FILTER_TAPS];
    s32 i;
    s32 n;
    s32 impulse[1] = { 1 << 12 };

    st->lastTime = 0;
    st->histIndex = 0;
    st->bufIndex = 0;
    for (i = 0; i < LG_HISTORY; i++) {
        st->dbuf[i] = 0;
        st->tbuf[i] = 0;
    }
    st->vdev = vd;
    st->lastOutput = 0;
    st->posFP = 0;
    st->pos = 0;
    st->filtered = 0;
    st->lastTime = OSTicksToMilliseconds(OSGetTick());

    for (i = 0; i < LG_FILTER_TAPS; i++) {
        st->coeff[i] = impulse[i];
        st->hist[i] = 0;
    }
    st->histIndex = 0;

    for (n = 0; n < LG_FILTER_TAPS; n++) {
        ia[n] = (s32)(4096.0f * a[n]);
    }
    for (i = 0; i < LG_FILTER_TAPS; i++) {
        st->coeff[i] = ia[i];
        st->hist[i] = 0;
    }
    st->histIndex = 0;
}

/* SimThread_Step -- 924 B objetivo, 920 NUESTROS, 68 filas, 95,671 %. La mas
 * grande de las seis y la mas lejos. r47: rotacion global r5<->r8, r6<->r7,
 * r25<->r26, r0<->r3 con raiz en la fila 16; probado y neutro `polarSum =
 * plainSum = 0`, `i = 0` con dos copias (MWCC lo pliega), adelantar `posSum =
 * timeSum = 0`. r48: swap MULDIV neutro 95,67 %.
 * r63, MEDIDO Y NEGATIVO: los 12 registros bloqueados (`asm { li rN, 0 }`)
 * NUNCA LLEGAN A 924 B -- la mejor es 116 filas (r6) contra 68 de la base, y
 * r8..r12 dan todos el MISMO objeto (117 filas), o sea que la funcion ya
 * reparte hasta r8. Y los 23 pragmas legales, todos neutros o peores:
 * `scheduling off` 68->141 filas, `peephole off` 920->936 B,
 * `global_optimizer off` 940 B, `optimization_level 0/1/2` 1192/976 B y 164
 * filas, `opt_propagation off` y `use_lmw_stmw off` 928 B.
 * Los 4 B que faltan aqui, con los 4 de Effect_PerformEnvelope, SON el
 * `.text -8` del enlace de la unidad.
 */
static s32 SimThread_Step(SIMTHREAD *st)
{
    s32 i;
    s32 j;
    s32 posSum;
    u32 timeSum;
    u32 dt;
    s32 polarSum;
    s32 plainSum;
    s32 v;
    s32 out;
    s32 gain;
    u32 now;
    u32 k;
    EFFECT *e;

    polarSum = 0;
    plainSum = 0;

    now = OSTicksToMilliseconds(OSGetTick());
    dt = now - st->lastTime;
    st->lastTime = now;

    v = st->pos;
    st->pos = st->vdev->dev->cooked.steering;
    st->posFP = st->pos << 12;
    v = st->pos - v;

    st->bufIndex--;
    if (st->bufIndex < 0) {
        st->bufIndex = LG_HISTORY - 1;
    }
    st->dbuf[st->bufIndex] = v;
    st->tbuf[st->bufIndex] = dt;

    j = st->bufIndex;
    posSum = timeSum = 0;
    for (i = 0; i < LG_HISTORY && timeSum < 16; i++) {
        posSum += st->dbuf[j];
        timeSum += st->tbuf[j++];
        if (j == LG_HISTORY) {
            j = 0;
        }
    }
    if (timeSum > 16) {
        j--;
        if (j < 0) {
            j = LG_HISTORY - 1;
        }
        MULDIV(v, timeSum - 16, st->dbuf[j], st->tbuf[j])
        posSum -= v;
    }

    st->hist[st->histIndex++] = posSum;
    if (st->histIndex > LG_FILTER_TAPS - 1) {
        st->histIndex = 0;
    }

    v = 0;
    for (i = LG_FILTER_TAPS - 1; i >= 0; i--) {
        v += st->hist[st->histIndex++] * st->coeff[i];
        if (st->histIndex > LG_FILTER_TAPS - 1) {
            st->histIndex = 0;
        }
    }
    st->filtered = v;

    for (k = 0; k < LG_MAX_EFFECTS; k++) {
        e = st->vdev->effects[k];
        if (e != NULL && (e->flags & EF_PLAYING)) {
            Effect_Update(e, dt);
            if ((e->flags & EF_PLAYING) && e->delayLeft == 0) {
                CLAMPV(v, e->force, -127, 127)
                i = e->flags & EF_POLAR;
            } else {
                i = 0;
                v = 0;
            }
            if (i) {
                polarSum += v;
            } else {
                plainSum += v;
            }
        }
    }

    gain = st->vdev->gain;
    CLAMPW(v, polarSum, -128, 128)
    if (v < 0) {
        v = -g_iGammaLookup[-v];
    } else {
        v = g_iGammaLookup[v];
    }

    if (gain < 255) {
        MULDIV(v, v, gain, 255)
        MULDIV(plainSum, plainSum, gain, 255)
    }

    CLAMPW(out, v + plainSum, -128, 128)

    if (st->lastOutput != out) {
        st->lastOutput = out;
        if (out == 0) {
            SIControlSteering(st->vdev->dev->chan, 0x400, 0);
        } else {
            SIControlSteering(st->vdev->dev->chan, 0x600, out);
        }
    }
    return out;
}
