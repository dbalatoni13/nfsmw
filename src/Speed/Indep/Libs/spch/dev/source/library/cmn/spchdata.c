#include "spch/spch.h"

// La unidad de datos puros de SPCH: no tiene ni una instruccion (`.text` de
// longitud cero en `.debug_aranges` del original) y define las 20 globales que
// las otras once unidades de la libreria declaran `extern`.
//
// DOS COSAS MANDAN AQUI, y las dos estan medidas (r63, informe r63-spch.md):
//
//  1. `.data` contra `.bss`: con `-x c++` una definicion SIN inicializador va a
//     `.bss`, y una CON inicializador --aunque valga cero-- va a `.data`.  Las
//     diez de arriba viven en `.data 0x80451ED0..0x80451F00` en el original, asi
//     que llevan `= 0` explicito; las diez de abajo viven en
//     `.bss 0x804CC6F8..0x804CC8FE` y no lo llevan.  Quitar un `= 0` mueve el
//     simbolo de seccion y descuadra las dos.
//
//  2. El ORDEN de `.bss` NO es el de definicion, es el de PRIMERA DECLARACION.
//     GCC 2.95 emite `.data` en el momento de la definicion pero difiere `.bss`
//     a `finish_file`, recorriendo la cadena de declaraciones globales: un
//     `extern` previo (por ejemplo el de una cabecera incluida antes) ADELANTA
//     el simbolo al principio de `.bss`.  Medido con `extern int e1; extern int
//     e2; int d1; int e2; int d2; int e1; int d3;` -> `.bss` sale
//     `e1 e2 d1 d2 d3`.  Por eso esta unidad NO incluye `spchi.h`: sus dos
//     `extern` (`gExtVecs` y `gEventDats`) colocaban `gEventDats` en el offset 0
//     de `.bss` y `gCallbacks` detras, al reves que el original.  Los tipos que
//     hacen falta se definen aqui.

struct VoxData;
struct VoxEvent;
struct VOXBANKHDR;

// total size: 0x8
struct EventDatInfo {
    VoxData *data;        // offset 0x0, size 0x4
    unsigned int channel; // offset 0x4, size 0x4
};

// total size: 0x14
struct SPCH_Callbacks {
    int (*request)(SPCHType_SampleRequestData *);       // offset 0x0, size 0x4
    int (*testRule)(EventSpec *, int, int, int);        // offset 0x4, size 0x4
    void (*setRule)(EventSpec *, int, int, int);        // offset 0x8, size 0x4
    SPCHType_EventRuleResult (*eventRule)(EventSpec *); // offset 0xC, size 0x4
    int (*reparm)(int, unsigned int *);                 // offset 0x10, size 0x4
};

// total size: 0x10
struct VoxEventItem {
    unsigned int entryTime;  // offset 0x0, size 0x4
    unsigned short subTicks; // offset 0x4, size 0x2
    unsigned char pending;   // offset 0x6, size 0x1
    unsigned char channel;   // offset 0x7, size 0x1
    VoxEvent *event;         // offset 0x8, size 0x4
    unsigned int *memParms;  // offset 0xC, size 0x4
};

// total size: 0x140
struct VoxPendingEvents {
    int numPending[8];       // offset 0x0, size 0x20
    int lastAddedEvent[8];   // offset 0x20, size 0x20
    VoxEventItem events[16]; // offset 0x40, size 0x100
};

// total size: 0x8
struct VOXINGAME {
    EventSpec lastEventSpec; // offset 0x0, size 0x4
    int numEventTimes;       // offset 0x4, size 0x4
};

// total size: 0x8
struct VoxBankInfo {
    int bankHandle;     // offset 0x0, size 0x4
    VOXBANKHDR *voxHdr; // offset 0x4, size 0x4
};

// .data 0x80451ED0..0x80451F00 (48 B): todo a cero salvo gClearCycle.
void *(*gMemAlloc)(unsigned int) = 0;   // 0x80451ED0
void (*gMemFree)(void *) = 0;           // 0x80451ED4
SPCHType_ExtVecs gExtVecs = {0, 0, 0};  // 0x80451ED8
int gSPCH_Initialized = 0;              // 0x80451EE4
VoxBankInfo *gVoxBanks = 0;             // 0x80451EE8
int gUniqueBankHandle = 0;              // 0x80451EEC
int gNumBanks = 0;                      // 0x80451EF0
int gBankCount = 0;                     // 0x80451EF4
int (*gSPCH_AddEvent)(int, int, ...) = 0; // 0x80451EF8
int gClearCycle = 1;                    // 0x80451EFC

// .bss 0x804CC6F8..0x804CC8FE (518 B) + 2 B de relleno hasta 0x804CC900.
SPCH_Callbacks gCallbacks;      // 0x804CC6F8
EventDatInfo gEventDats[8];     // 0x804CC70C
VoxPendingEvents gVoxEvents;    // 0x804CC74C
VOXINGAME gVoxInGame[8];        // 0x804CC88C
unsigned int gGameNum;          // 0x804CC8CC
int gDataRate;                  // 0x804CC8D0
int gFilterSetting[8];          // 0x804CC8D4
int gPreLoadTicks;              // 0x804CC8F4
unsigned int gLastTick;         // 0x804CC8F8
unsigned short gLastSubTick;    // 0x804CC8FC
