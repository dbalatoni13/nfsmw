#include "Speed/Indep/Src/Camera/Movers/Cubic.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/CubicTweaks.hpp"
#include "Speed/Indep/Src/Misc/Table.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/RaceParameters.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/Misc/Rumble.hpp"

bVector3 *bClamp(bVector3 *dest, const bVector3 *minimum, const bVector3 *maximum);

bVector3 *bClamp(bVector3 *dest, const bVector3 *minimum, const bVector3 *maximum) {

    float x = bClamp(dest->x, minimum->x, maximum->x);
    float y = bClamp(dest->y, minimum->y, maximum->y);
    float z = bClamp(dest->z, minimum->z, maximum->z);

    dest->x = x;
    dest->y = y;
    dest->z = z;

    return dest;
}



// zWorld / zMisc helpers.


// Inlined tCubic1D::Snap / tCubic3D::Snap as seen in this TU's Point.hpp
// (unlike the zMisc variant, it does NOT write time).
static inline void CubicSnap(tCubic1D *cubic) {
    cubic->Val = cubic->ValDesired;
    cubic->dVal = cubic->dValDesired;
    cubic->state = 0;
}

static inline void CubicSnap(tCubic3D *cubic) {
    CubicSnap(&cubic->x);
    CubicSnap(&cubic->y);
    CubicSnap(&cubic->z);
}

// Inlined: set the duration on all three components of a tCubic3D.
static inline void CubicSetDuration(tCubic3D *cubic, float dur) {
    cubic->x.duration = dur;
    cubic->y.duration = dur;
    cubic->z.duration = dur;
}

// Inlined: forward durations get extra reaction time from collisions and drift.
static inline void CubicSetForwardDuration(tCubic3D *cubic, const float *dur, float fDamping, float fDrift) {
    float dur_x = dur[0] + fDamping + fDrift;
    float dur_y = dur[1] + fDamping + fDrift;
    float dur_z = dur[2] + fDamping + fDrift;

    cubic->x.duration = dur_x;
    cubic->y.duration = dur_y;
    cubic->z.duration = dur_z;
}


bVector2 aDriftData[2] = {bVector2(8.0f, 0.0f), bVector2(13.0f, 1.0f)}; // size: 0x10

Graph gDriftSpeed(aDriftData, 2);

CubicPovData CubicBumper[2] = {
    {
        0.0f,
        0.0f,
        1.0f,
        0.0f,
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
    },
    {
        0.0f,
        0.0f,
        0.5f,
        0.0f,
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
    },
};

CubicPovData CubicHood[2] = {
    {
        0.7f,
        0.7f,
        1.0f,
        0.7f,
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {-0.01f, -0.02f, 0.1f, 0.0f},
        {-0.1f, -0.1f, -0.1f, 0.0f},
        {0.3f, 0.1f, 0.1f, 0.0f},
        {-0.1f, -0.02f, 0.0f, 0.0f},
        {-0.05f, -0.1f, 0.0f, 0.0f},
        {0.2f, 0.1f, 0.0f, 0.0f},
        {0.4f, 0.4f, 0.4f, 0.0f},
    },
    {
        0.5f,
        0.5f,
        0.5f,
        0.5f,
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {-0.1f, -0.1f, -0.1f, 0.0f},
        {0.3f, 0.1f, 0.1f, 0.0f},
        {0.0f, 0.0f, 0.0f, 0.0f},
        {-0.05f, -0.1f, 0.0f, 0.0f},
        {0.2f, 0.1f, 0.0f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
    },
};

CubicPovData CubicOutsideNear[2] = {
    {
        1.25f,
        0.4f,
        1.0f,
        0.8f,
        {0.004f, 0.004f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 0.0f},
        {-1.25f, -1.0f, -1.0f, 0.0f},
        {1.5f, 1.0f, 1.0f, 0.0f},
        {-0.5f, 0.0f, 0.0f, 0.0f},
        {-2.0f, 0.0f, -1.0f, 0.0f},
        {2.0f, 0.0f, 1.0f, 0.0f},
        {0.4f, 0.4f, 0.4f, 0.0f},
    },
    {
        0.75f,
        0.3f,
        0.5f,
        0.4f,
        {0.002f, 0.002f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.75f, 0.0f, 0.0f, 0.0f},
        {-1.0f, -1.0f, -1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.2f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 0.0f, -1.0f, 0.0f},
        {0.5f, 0.0f, 1.0f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
    },
};

CubicPovData CubicOutsideFar[2] = {
    {
        1.25f,
        0.4f,
        1.0f,
        0.8f,
        {0.005f, 0.005f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f, 0.0f},
        {-1.0f, -1.0f, -1.0f, 0.0f},
        {2.0f, 1.0f, 1.0f, 0.0f},
        {-0.1f, 0.0f, 0.0f, 0.0f},
        {-2.0f, 0.0f, -1.0f, 0.0f},
        {2.0f, 0.0f, 1.0f, 0.0f},
        {0.4f, 0.4f, 0.4f, 0.0f},
    },
    {
        0.75f,
        0.3f,
        0.5f,
        0.4f,
        {0.003f, 0.003f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.5f, 0.0f, 0.0f, 0.0f},
        {-1.0f, -1.0f, -1.0f, 0.0f},
        {2.0f, 1.0f, 1.0f, 0.0f},
        {-0.05f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 0.0f, -1.0f, 0.0f},
        {0.5f, 0.0f, 1.0f, 0.0f},
        {0.2f, 0.2f, 0.2f, 0.0f},
    },
};

CubicPovData CubicSuperFar[2] = {
    {
        1.5f,
        0.5f,
        1.0f,
        1.0f,
        {0.006f, 0.006f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.25f, 0.0f, 0.0f, 0.0f},
        {-2.5f, -1.0f, -1.0f, 0.0f},
        {2.0f, 1.0f, 1.0f, 0.0f},
        {-0.1f, 0.0f, 0.0f, 0.0f},
        {-2.0f, 0.0f, -1.0f, 0.0f},
        {2.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.5f, 0.0f},
    },
    {
        0.5f,
        0.2f,
        0.5f,
        0.5f,
        {0.002f, 0.002f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.05f, 0.0f, 0.0f, 0.0f},
        {-0.5f, -1.0f, -1.0f, 0.0f},
        {0.5f, 1.0f, 1.0f, 0.0f},
        {-0.02f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 0.0f, -1.0f, 0.0f},
        {0.5f, 0.0f, 1.0f, 0.0f},
        {0.1f, 0.1f, 0.1f, 0.0f},
    },
};

CubicPovData CubicDrift[2] = {
    {
        1.5f,
        0.5f,
        1.0f,
        1.0f,
        {0.006f, 0.006f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.25f, 0.0f, 0.0f, 0.0f},
        {-2.5f, -1.0f, -1.0f, 0.0f},
        {2.0f, 1.0f, 1.0f, 0.0f},
        {-0.1f, 0.0f, 0.0f, 0.0f},
        {-2.0f, 0.0f, -1.0f, 0.0f},
        {2.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.5f, 0.0f},
    },
    {
        0.5f,
        0.2f,
        0.5f,
        0.5f,
        {0.002f, 0.002f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.05f, 0.0f, 0.0f, 0.0f},
        {-0.5f, -1.0f, -1.0f, 0.0f},
        {0.5f, 1.0f, 1.0f, 0.0f},
        {-0.02f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 0.0f, -1.0f, 0.0f},
        {0.5f, 0.0f, 1.0f, 0.0f},
        {0.1f, 0.1f, 0.1f, 0.0f},
    },
};

CubicPovData CubicPursuit[2] = {
    {
        1.5f,
        0.5f,
        1.0f,
        1.0f,
        {0.006f, 0.006f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.25f, 0.0f, 0.0f, 0.0f},
        {-2.5f, -1.0f, -1.0f, 0.0f},
        {2.0f, 1.0f, 1.0f, 0.0f},
        {-0.1f, 0.0f, 0.0f, 0.0f},
        {-2.0f, 0.0f, -1.0f, 0.0f},
        {2.0f, 0.0f, 1.0f, 0.0f},
        {0.5f, 0.5f, 0.5f, 0.0f},
    },
    {
        0.5f,
        0.2f,
        0.5f,
        0.5f,
        {0.002f, 0.002f, 0.0f, 0.0f},
        {-1.0f, -1.0f, 1.0f, 0.0f},
        {1.0f, 1.0f, 1.0f, 0.0f},
        {-0.05f, 0.0f, 0.0f, 0.0f},
        {-0.5f, -1.0f, -1.0f, 0.0f},
        {0.5f, 1.0f, 1.0f, 0.0f},
        {-0.02f, 0.0f, 0.0f, 0.0f},
        {-0.5f, 0.0f, -1.0f, 0.0f},
        {0.5f, 0.0f, 1.0f, 0.0f},
        {0.1f, 0.1f, 0.1f, 0.0f},
    },
};
tTable<CubicPovData> aCubicPovTables[7] = {
    tTable<CubicPovData>(CubicBumper, 2, 0.0f, 1.0f),
    tTable<CubicPovData>(CubicHood, 2, 0.0f, 1.0f),
    tTable<CubicPovData>(CubicOutsideNear, 2, 0.0f, 1.0f),
    tTable<CubicPovData>(CubicOutsideFar, 2, 0.0f, 1.0f),
    tTable<CubicPovData>(CubicSuperFar, 2, 0.0f, 1.0f),
    tTable<CubicPovData>(CubicDrift, 2, 0.0f, 1.0f),
    tTable<CubicPovData>(CubicPursuit, 2, 0.0f, 1.0f),
}; // size: 0x8C

// Escalas de ruido por POV: el objetivo las DEFINE en zCamera (.data, 4x28 B).
float PovHandheldNoiseScale[7] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
float PovHandheldChopperScale[7] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
float PovVelocityNoiseScale[7] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
float PovTerrainNoiseScale[7] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

bVector2 CameraSpeedHugData[5] = {
    bVector2(1.0f, 1.0f),   //
    bVector2(1.0f, 1.0f),   //
    bVector2(1.02f, 1.0f),  //
    bVector2(1.125f, 1.0f), //
    bVector2(1.2f, 1.0f),   //
}; // size: 0x28

bVector3 vCubicBirdsEyeOffset[2] = {bVector3(-17.2f, 0.0f, 6.0f), bVector3(-15.8f, 0.0f, 4.4f)}; // size: 0x20

bVector3 SmokeShowEyeOffset(-10.0f, 0.0f, 5.0f); // size: 0x10
bAngle SmokeShowLookAngle = bDegToAng(16.5f);
bVector3 HydraulicsEyeOffset(-5.0f, 0.0f, 1.5f); // size: 0x10
// VEDA r47, ampliada en la r50. static-init 97,11 %: el `li 0x38e` sale en la
// fila 549 en el objetivo y en la 557 en el nuestro; constantes verificadas
// bit a bit con litpos (849 refs, 0 distintas). Es orden del planificador.
//
// r50 --- ESTE RACIMO ES LA CERRADURA QUE MANDA. Son tres insns
// independientes que ROTAN entre tres ranuras. Los registros que salen por
// RANURA son los mismos en los dos lados (r11/r9/r10); lo que difiere es que
// valor lleva cada uno.
//   objetivo: [lis LITERAL] [li 0x38e] [lis HydraulicsLookAngle@ha]
//   nuestro:  [lis HydraulicsLookAngle@ha] [lis LITERAL] [li 0x38e]
// El literal es lbl_803D3A80 = 12000.0f, y va a f24 (preservado) porque se
// consume 50 y 97 instrucciones despues, en vCopViewDistanceFovBand[0].y y
// vCopViewDistanceFov[0].y (Movers/CopView.cpp): su cadena es la mas larga de
// las tres y los dos lados le dan la prioridad mas alta.
//
// La forma de la fuente esta CONFIRMADA (r50): con `= 910` el objeto pasa a
// .data y la funcion ENCOGE a 3.592 B; el objetivo tiene `li 0x38e` + `sth`,
// o sea init dinamica, que es lo que da bDegToAng(). Y NINGUN relleno de
// codigo muerto mueve estas filas, ni puesto delante: `flow` borra el codigo
// muerto ANTES de `sched1`.
//
// Y ES `sched2`, NO LA FUENTE (r50, una compilacion por bandera):
//   base                    SYM(r11) LIT(r9) CONST(r10)
//   -fno-schedule-insns2    LIT(r9) CONST(r10) SYM(r11)  <- el orden del objetivo
//   objetivo                LIT(r11) CONST(r9) SYM(r10)
// Nuestro orden POST-reload ya es el bueno; lo rota `sched2`, donde
// INSN_REG_WEIGHT no existe y decide `depend_count` (r48 seccion 1.2). Y
// apagar sched2 no mueve un registro, asi que lo que difiere del objetivo es
// el MAPA valor->registro (el nuestro SYM=r11/LIT=r9/CONST=r10, el suyo
// SYM=r10/LIT=r11/CONST=r9): la palanca esta en el ASIGNADOR, no aqui.
// Ver docs/analisis/r50-cam.md, seccion 2.
// r61-plat: LA RECETA 1 DEL INFORME DE VEDAS r60b, MEDIDA Y NEGATIVA.
// Se probo tal cual la escribe `docs/analisis/r60b-auditoria-vedas.md`:
//     static inline bAngle _cam_pin(bAngle a) { asm("" : "+r"(a)); return a; }
//     bAngle HydraulicsLookAngle = _cam_pin(bDegToAng(5.0f));
// Base 3.604/3.604 B, 99,839066 %, SEIS filas (549/557/558/561/562/564).
// Con la receta: 3.604/3.604 B --- el tamano AGUANTA, o sea que el asm no
// gasta ranura y el control del informe se cumple --- pero 98,234184 % y
// CINCUENTA Y OCHO filas. Y falla su propio observable: la fila 549 NO pasa
// a `li r9,0x38e`; el `li 0x38e` se va a la fila 533 y arrastra todo el
// racimo de literales de Demo1/Demo2/PreviousEye (521..614).
// Sello: zCamera.o 38a0b9fb358bcb4215634b68c7d811d48a5959da; base
// e74ba2da8c97e6b172d671c2b4916fd348af2d4b (arbol devuelto a la base).
//
// r62: DOS AVISOS MEDIDOS, NINGUNA PALANCA NUEVA.
// (1) `reorden.py` MIENTE sobre esta funcion: dice CERO palabras de contenido
//     real, y `fndiff` sigue dando las SEIS filas de siempre (549/557/558/
//     561/562/564, 3.604/3.604 B, 99,839066 %).  La causa esta medida: el
//     nombre `__static_initialization_and_destruction_0` aparece 47 VECES en
//     el ELF enlazado (312 nombres repetidos en total) y `reorden` empareja
//     POR NOMBRE, asi que compara contra el static-init de otra unidad.  No
//     uses `reorden` para dar por cerrada una funcion con nombre repetido.
// (2) Reordenar la unidad entera (r62: 79 -> 28 funciones descolocadas,
//     dolwhere 333.533 -> 30.671 B) NO mueve ni una de las seis filas.  Es
//     una prueba mas de que la cerradura es del asignador y no del entorno.
// e74ba2da8c97e6b172d671c2b4916fd348af2d4b (arbol devuelto a la base).
//
// POR QUE FALLA, con la medida que lo explica (`lreg.py zCamera
// __static_initialization_and_destruction_0`): esta funcion tiene 437
// pseudos y r9/r10/r11 los comparten DECENAS de allocnos de vida corta
// (solo con prioridad 3333 hay veintitantos). Meter un insn de asm en medio
// no sube n_refs de la cantidad que importa: le cambia el live_length a
// todos los vecinos a la vez y re-baraja el racimo entero. La palanca que
// SI funciona en un caso de este tipo esta medida esta misma ronda en
// `JoyE.cpp` (ActualReadJoystickData, de 9 filas a 0): fantasma sobre el
// registro que sobra + relleno de rango CALIBRADO con lreg escalon a
// escalon. Aqui hace falta primero identificar los TRES allocnos (SYM/LIT/
// CONST) en el `.greg`, que en una funcion de 437 pseudos no se hace de
// oido: sin esa identificacion cualquier ensayo es a ciegas.
// ==== r64-diferido-cam: LOS TRES ALLOCNOS, IDENTIFICADOS ====
// La veda de arriba se cerraba con "hace falta primero identificar los TRES
// allocnos (SYM/LIT/CONST) en el .greg, que en una funcion de 437 pseudos no
// se hace de oido".  YA ESTAN.  Receta, 40 s:
//     python scripts/rtldump.py zCamera static_initialization -dl
//     grep -n "const_int 910" scratchpad/rtl/zCamera_cpp.i.lreg   -> insn 2839
// y alrededor de esa insn estan las tres, con el .lreg todavia en pseudos:
//
//   cantidad                       insn   pseudo  n_refs live_len prio   reg
//   CONST  (const_int 910)         2839    858      2       7     2857   r10
//   SYM    (high HydraulicsLook..) 2847    874      2      10     2000   r11
//   LIT    (high *$LC1122 =12000f) 3165    965      2      36      555   r9
//
// (`python scripts/lreg.py zCamera __static_initialization_and_destruction_0`
//  da esas cuatro columnas; las tres filas son las de pseudo 858/874/965.)
//
// EL OBJETIVO REPARTE LIT=r11, CONST=r9, SYM=r10.  Y las TRES RANURAS son las
// mismas en los dos lados (541, 549, 557): lo unico que rota es que valor cae
// en cada (ranura,registro).  Con el orden de proceso de global_alloc que se
// observa hoy --por prioridad: 858, 874, 965, y les toca r10, r11, r9-- para
// llegar al reparto del objetivo hace falta el orden SYM, LIT, CONST, o sea
// subir la prioridad de 874 por encima de 2857 (n_refs 3 -> 3000) y la de 965
// entre medias.  El limite esta MEDIDO: 965 tiene live_len 36, asi que
// n_refs=10 da 2777 y n_refs=11 da 3055 -- no hay valor entero que caiga entre
// 2857 y 3000.  O sea que con n_refs SOLO no se llega: hace falta mover
// tambien live_length.
//
// NEGATIVO NUEVO Y MEDIDO (r64): la variante "m" de la palanca de la r53
// (colgar una entrada de un asm de cero bytes para subir n_refs) NO ES GRATIS
// AQUI.  Probado:
//     extern bAngle HydraulicsLookAngle;
//     static inline bAngle _cam64_m(bAngle a) { asm("" : : "m"(HydraulicsLookAngle)); return a; }
//     bAngle HydraulicsLookAngle = _cam64_m(bDegToAng(5.0f));
//   base   3.604/3.604 B, 99,839066 %, SEIS filas (549/557/558/561/562/564)
//   con la 3.644/3.604 B (+40), 89,59379 %, 236 filas
// El +40 dice donde falla: el operando "m" gasta ranura, el marco encoge de
// 0xa8 a 0xa0 y el bloque de psq_st se corre entero (se preserva un FPR menos).
// FALLA EL CONTROL DURO DEL ENCARGO (tamano clavado en 3.604) -> descartada.
// Con esto quedan medidas las dos formas del pin en esta funcion: "+r" sobre
// el valor (r61, 58 filas) y "m" sobre el simbolo (r64, 236 filas).
// Lo que NO se ha probado todavia: tocar live_length (rango de vida) en vez de
// n_refs -- la palanca de la "cantidad fantasma" sobre los pseudos 874 y 965.
//
// r73 (vedas-oraculo-r3): TRES piezas nuevas, ninguna palanca.
// (1) MEDIDO el swap de definiciones (NOSFov antes que Hydraulics):
//     7 filas (empeora; la 535 se une al diff). El orden actual es el mejor.
// (2) REENCUADRE DEL ASIGNADOR: esta funcion es UN bloque basico -> las tres
//     cantidades son de LOCAL-ALLOC, no global. La formula es QTY_CMP_PRI
//     (local-alloc.c:1568) = floor_log2(n_refs)*n_refs*size/(death-birth)*10000
//     — con n_refs=2 y size=1 coincide con la de r64, PERO find_free_reg
//     prueba PRIMERO con ventanas FALSAS +-2 (birth-2..death+2) porque la
//     unidad lleva -fschedule-insns-after-reload: dos cantidades con vidas
//     REALES disjuntas pueden pelearse el registro si nacen/mueren a +-2.
//     La batalla por r10/r11/r9 es de vecindad con las otras veintitantas de
//     prioridad 3333, ventana a ventana.
// (3) EL live=36 del LIT (965) ES ESTRUCTURAL Y ESTA BLOQUEADO: el high de
//     $LC1122 lo comparten por CSE vCopViewDistanceFovBand[0].y (CopView.cpp:11)
//     y vCopViewDistanceFov[0].y (:26), ~47 insns de distancia, y las
//     direcciones .bss anotadas (0x8045AE14 / 0x8045AE98) prueban que el
//     objetivo tenia las definiciones en ESTE mismo orden — reordenar
//     CopView.cpp romperia el layout. La palanca fantasma de JoyE (r62)
//     queda como unica via: un fantasma sobre r10 tapando la ventana falsa
//     [~2837..~2848] del CONST (858) para empujarlo a r9. Calibracion lreg
//     escalon a escalon; los pines "n_refs" ya estan medidos y fallan.
bAngle HydraulicsLookAngle = bDegToAng(5.0f);
bAngle NOSFovWidening = bDegToAng(9.0f);

bVector3 Demo1EyeOffset(0.78035f, -3.09249f, 0.89595f); // size: 0x10
bVector3 Demo1LookOffset(0.20231f, 0.0f, 0.0f);         // size: 0x10
bVector3 Demo2EyeOffset(1.35838f, -1.35838f, 0.20231f); // size: 0x10
bVector3 Demo2LookOffset(0.43353f, 1.76301f, 0.0f);     // size: 0x10

float CameraAccelerationCurve[5] = {-1.0f, -0.1f, 0.0f, 0.1f, 1.0f};
float CameraGearChangingCurve[9] = {0.0f, 0.2f, 0.13f, -0.08f, -0.2f, -0.13f, -0.07f, -0.03f, 0.0f};
float CameraImpcatCurveH[5] = {0.0f, 0.02f, 0.25f, 0.99f, 0.0f};
float CameraImpcatCurveV[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

bool OutsidePovType(int nType) {
    return (nType >= POV_OUTSIDE_CLOSE && nType <= POV_SUPER_FAR) || nType == POV_PURSUIT || nType == POV_DRIFT;
}

bool RenderCarPovType(int nType, bool bLookBack) {
    return (nType >= POV_OUTSIDE_CLOSE && nType <= POV_SUPER_FAR) || (nType == POV_HOOD && !bLookBack) || nType == POV_PURSUIT ||
           nType == POV_DRIFT;
}

bool CubicCameraMover::IsHoodCamera() {
    return nPovTypeUsed == POV_HOOD && !bLookBack;
}

CubicCameraMover::CubicCameraMover(int nView, CameraAnchor *p_car, int pov_type, bool smooth, bool disable_lag, bool look_back,
                                   bool perfect_focus)
    : CameraMover(nView, CM_DRIVE_CUBIC), pCar(p_car), nPovType(pov_type), nPovTypeUsed(pov_type),
      bAccelLag(!disable_lag), bLookBack(look_back), bSnapNext(false), bPerfectFocus(perfect_focus),
      tLastGrounded(WorldTimer - 8000), tLastUnderVehicle(WorldTimer - 6400), tLastGearChange(WorldTimer - 6000) {
    fIgnoreSetSnapNextTimer = 0.0f;
    bFirstTime = true;

    POV *pov = pCar->GetPov(pov_type);

    CubicPovData pov_data;
    aCubicPovTables[nPovType].GetValue(&pov_data, 0.0f);

    pFov = new ("tCubic1D", 0) tCubic1D(1, pov_data.fFovDuration);

    pEye = new ("tCubic3D", 0) tCubic3D(1, pov_data.fEyeDuration);

    pLook = new ("tCubic3D", 0) tCubic3D(1, pov_data.fLookDuration);

    pForward = new ("tCubic3D", 0) tCubic3D(1, pov_data.GetForwardDuration());

    pUp = new ("tCubic3D", 0) tCubic3D(1, pov_data.GetForwardDuration());

    pAvgAccel = new tAverage<bVector3>(5);

    bMatrix4 car_to_world;
    SetDesired(&car_to_world, pov, &pov_data, true);

    bMatrix4 world_to_camera;
    eInvertTransformationMatrix(&world_to_camera, &car_to_world);

    if (bLookBack) {
        bScale((bVector2 *)&world_to_camera[0], (const bVector2 *)&world_to_camera[0], -1.0f);
        bScale((bVector2 *)&world_to_camera[1], (const bVector2 *)&world_to_camera[1], -1.0f);
        bScale((bVector2 *)&world_to_camera[2], (const bVector2 *)&world_to_camera[2], -1.0f);
        bScale((bVector2 *)&world_to_camera[3], (const bVector2 *)&world_to_camera[3], -1.0f);
    }

    SetEyeLook(pEye, pLook, pFov, &world_to_camera, (bVector3 *)pCar);

    bVector3 eye_val;
    bVector3 eye_des;
    bVector3 look_val;
    bVector3 look_des;
    pEye->GetVal(&eye_val);
    pLook->GetVal(&look_val);
    pEye->GetValDesired(&eye_des);
    pLook->GetValDesired(&look_des);

    bVector3 vEyeDelta = eye_des - eye_val;
    bVector3 vLookDir = look_val - eye_val;
    bVector3 vLookDirDesired = look_des - eye_des;

    bNormalize(&vLookDir, &vLookDir);
    bNormalize(&vLookDirDesired, &vLookDirDesired);

    vSavedEye.z = 0.0f;
    vSavedEye.y = 0.0f;
    vSavedEye.x = 0.0f;
    vCameraImpcat.y = 0.0f;
    vCameraImpcat.x = 0.0f;
    vCameraImpcatTimer.y = 0.0f;
    vCameraImpcatTimer.x = 0.0f;

    if (!smooth || bLength(&vEyeDelta) > 50.0f || bDot(&vLookDir, &vLookDirDesired) < -0.9f) {
        CubicSnap(pUp);
        CubicSnap(pFov);
        CubicSnap(pEye);
        CubicSnap(pLook);
    } else {
        fIgnoreSetSnapNextTimer = 1.0f;
    }
}

CubicCameraMover::~CubicCameraMover() {

    delete pUp;
    delete pFov;
    delete pEye;
    delete pLook;
    delete pForward;
    delete pAvgAccel;
}

float CubicCameraMover::MinDistToWall() {
    return 0.7f;
}

bool CubicCameraMover::OutsidePOV() {
    return OutsidePovType(nPovTypeUsed);
}

int CubicCameraMover::RenderCarPOV() {
    return RenderCarPovType(nPovTypeUsed, bLookBack);
}

bool CubicCameraMover::HighliteMode() {
    return false;
}

void CubicCameraMover::SetSnapNext() {
    if (fIgnoreSetSnapNextTimer > 0.0f) {
        return;
    }
    bSnapNext = true;
}

void CubicCameraMover::SetPovType(int pov_type) {
    if (pov_type != nPovTypeUsed) {

        bool old_outside = OutsidePovType(nPovTypeUsed);
        bool new_outside = OutsidePovType(pov_type);

        bSnapNext |= !(new_outside && old_outside);
        nPovType = pov_type;
    }
}

void CubicCameraMover::ResetState() {

    CubicSnap(pUp);
    CubicSnap(pFov);
    CubicSnap(pEye);
    CubicSnap(pLook);
    CubicSnap(pForward);

    pCamera->ClearVelocity();

    vCameraImpcat.y = 0.0f;
    vCameraImpcat.x = 0.0f;
    vCameraImpcatTimer.y = 0.0f;
    vCameraImpcatTimer.x = 0.0f;
}

bool CubicCameraMover::IsUnderVehicle() {

    const IVehicle::List &vehicles = IVehicle::GetList(VEHICLE_TRAILERS);

    for (IVehicle::List::const_iterator iter = vehicles.begin(); iter != vehicles.end(); iter++) {
        IVehicle *ivehicle = *iter;

        if (!ivehicle->IsActive()) {
            continue;
        }

        if (ivehicle->IsLoading()) {
            continue;
        }

        ISimable *isimable = ivehicle->GetSimable();
        if (isimable == NULL) {
            continue;
        }

        if (isimable->GetWorldID() == pCar->GetWorldID()) {
            continue;
        }

        IRigidBody *irb = isimable->GetRigidBody();
        if (irb == NULL) {
            continue;
        }

        const UMath::Vector3 &vehicle_pos = ivehicle->GetPosition();

        UMath::Vector4 vehiclePos = UMath::Vector4Make(vehicle_pos, 1.0f);
        UMath::Vector4 testPos;
        UMath::Vector4 test2vehicle;
        UMath::Vector4 test2vehicleLocal;

        UMath::Vector3 dim = irb->GetDimension();

        UMath::Matrix4 mat;
        irb->GetMatrix4(mat);

        UMath::Matrix4 world2local;
        UMath::Transpose(mat, world2local);

        UMath::Vector3 forward;
        irb->GetForwardVector(forward);

        bVector3 carFwd;
        eSwizzleWorldVector((const bVector3 &)forward, carFwd);

        bVector3 *camFwd = pCamera->GetDirection();

        float dot = bDot(camFwd, &carFwd);

        if (bAbs(dot) >= 0.707f) {
            continue;
        }

        {
            bVector3 predictedPos = *pCar->GetGeometryPosition() + *pCar->GetVelocity() * 0.05f;

            eUnSwizzleWorldVector(predictedPos, (bVector3 &)testPos);
            testPos.w = 1.0f;

            UMath::Sub(testPos, vehiclePos, test2vehicle);

            if (UMath::Length(test2vehicle) >= irb->GetRadius()) {
                continue;
            }

            UMath::Rotate(test2vehicle, world2local, test2vehicleLocal);

            if (UMath::Abs(test2vehicleLocal.x) >= dim.x || UMath::Abs(test2vehicleLocal.z) >= dim.z) {
                continue;
            }

            return true;
        }
    }

    return false;
}

void CubicCameraMover::SetForward(POV *pov, bool bSnap) {

    if (pov != NULL && OutsidePovType(pov->Type)) {

        if (!bSnap && HighliteMode()) {
            return;
        }

        bVector3 v(*pCar->GetVelocity());

        const bVector3 *pFwd = pCar->GetForwardVector();
        float fDot = bDot(&v, pFwd);

        if (fDot < 0.0f) {
            bScaleAdd(&v, &v, pFwd, fDot * -2.0f);
        }

        float fDrift = gDriftSpeed.GetValue(pCar->GetVelocityMagnitude());

        bNormalize(&v, &v);

        bScaleAdd(&v, bScale(&v, &v, fDrift), pFwd, 1.0f - fDrift);

        float fSeconds = (WorldTimer - tLastGrounded).GetSeconds();
        v.z *= bClamp(1.0f - fSeconds * 0.5f, 0.0f, 1.0f);

        float z = pCar->GetForwardVector()->z;
        v.z *= 1.0f - z * z;

        pForward->SetValDesired(&v);

        if (!bSnap) {
            return;
        }
    } else {
        if (pCar == NULL) {
            return;
        }
        pForward->SetValDesired(pCar->GetForwardVector());
    }

    CubicSnap(pForward);
}

void CubicCameraMover::MakeSpace(bMatrix4 *pMatrix) {

    if (OutsidePOV()) {

        bIdentity(pMatrix);

        bVector3 vForward(pForward->x.Val, pForward->y.Val, pForward->z.Val);

        bNormalize((bVector3 *)&pMatrix->v0, &vForward);
        bCross((bVector3 *)&pMatrix->v1, (bVector3 *)&pMatrix->v2, (bVector3 *)&pMatrix->v0);
        bCross((bVector3 *)&pMatrix->v2, (bVector3 *)&pMatrix->v0, (bVector3 *)&pMatrix->v1);
        bCopy(&pMatrix->v3, pCar->GetGeometryPosition(), 1.0f);
    } else {

        bCopy(pMatrix, pCar->GetGeometryOrientation(), pCar->GetGeometryPosition());

        if (pCar->IsUpsideDown()) {


            bScale(&pMatrix->v1, &pMatrix->v1, -1.0f);
            bScale(&pMatrix->v2, &pMatrix->v2, -1.0f);
        }
    }
}

void CubicCameraMover::CameraAccelCurve(bVector3 *pAccel) {

    tTable<float> accel_table(CameraAccelerationCurve, 5, -30.0f, 30.0f);

    accel_table.GetValue(&pAccel->x, pCar->GetAcceleration()->x);
    accel_table.GetValue(&pAccel->y, pCar->GetAcceleration()->y);
    accel_table.GetValue(&pAccel->z, pCar->GetAcceleration()->z);

    bScale(pAccel, pAccel, 30.0f);
}

void CubicCameraMover::CameraSpeedHug(bVector3 *pEyeOffset) {

    if (pCar->GetTopSpeed() > 0.0f) {

        bVector2 v_hug;
        tTable<bVector2> speed_hug_table(CameraSpeedHugData, 5, 0.0f, pCar->GetTopSpeed());

        speed_hug_table.GetValue(&v_hug, pCar->GetVelocityMagnitude());

        pEyeOffset->x *= v_hug.x;
        pEyeOffset->z *= v_hug.y;
    }
}

void CubicCameraMover::SetDesired(bMatrix4 *pCarToWorld, POV *pov, CubicPovData *pov_data, bool bSnapForward) {

    bool b_outside = OutsidePovType(pov->Type);

    if (pCar->IsTouchingGround()) {
        tLastGrounded = WorldTimer;
    }

    if (IsUnderVehicle()) {
        tLastUnderVehicle = WorldTimer;
    }

    SetForward(pov, bSnapForward);

    MakeSpace(pCarToWorld);

    bMatrix4 world_to_camera;
    eInvertTransformationMatrix(&world_to_camera, pCarToWorld);

    bVector4 vAccel(0.0f, 0.0f, 0.0f, 0.0f);

    bVector3 vEyeOffset;
    bVector3 vLookOffset;
    bVector3 vEyeAccel;
    bVector3 vLookAccel;

    if (bAccelLag) {
        bCopy(&vAccel, pAvgAccel->GetValue(), 0.0f);
        bMulMatrix(&vAccel, &world_to_camera, &vAccel);
    }

    bAngle nLookAngle;

    if (TheRaceParameters.IsBurnout()) {
        vEyeOffset = SmokeShowEyeOffset;
        nLookAngle = SmokeShowLookAngle;
    } else if (HighliteMode()) {
        vEyeOffset = HydraulicsEyeOffset;
        nLookAngle = HydraulicsLookAngle;
    } else {
        vEyeOffset.x = pov->Lag;
        vEyeOffset.y = pov->LatOffset;
        vEyeOffset.z = pov->Height;
        CameraSpeedHug(&vEyeOffset);
        nLookAngle = pov->Angle;
    }

    float fTan = bTan(nLookAngle);

    float fLag;
    float fHeight;

    if (b_outside) {
        fLag = 0.0f;
    } else {
        fLag = vEyeOffset.x + 1.0f;
    }

    if (b_outside) {
        fHeight = fTan * vEyeOffset.x + vEyeOffset.z;
    } else {
        fHeight = vEyeOffset.z - fTan;
    }

    vLookOffset.x = fLag;
    vLookOffset.y = 0.0f;
    vLookOffset.z = fHeight;

    vEyeAccel.x = pov_data->vEyeAccel[0] * vAccel.x;
    vEyeAccel.y = pov_data->vEyeAccel[1] * vAccel.y;
    vEyeAccel.z = pov_data->vEyeAccel[2] * vAccel.z;
    bClamp(&vEyeAccel, pov_data->GetEyeAccelMin(), pov_data->GetEyeAccelMax());

    if (HighliteMode()) {
        bScale(&vEyeOffset, &vEyeOffset, 1.3f);
    }

    bAdd(&vEyeAccel, &vEyeAccel, &vEyeOffset);
    pEye->SetValDesired(&vEyeAccel);

    vLookAccel.x = pov_data->vLookAccel[0] * vAccel.x;
    vLookAccel.y = pov_data->vLookAccel[1] * vAccel.y;
    vLookAccel.z = pov_data->vLookAccel[2] * vAccel.z;
    bClamp(&vLookAccel, pov_data->GetLookAccelMin(), pov_data->GetLookAccelMax());

    bAdd(&vLookAccel, &vLookAccel, &vLookOffset);
    pLook->SetValDesired(&vLookAccel);

    bVector3 vUp(0.0f, 0.0f, 1.0f);

    if (pov->AllowTilting) {
        vUp = vUp + bScale(*pov_data->GetUpAccel(), *pAvgAccel->GetValue());
        bClamp(&vUp, pov_data->GetUpAccelMin(), pov_data->GetUpAccelMax());
    }

    if (pCar->GetVelocityMagnitude() > 5.0f) {
        bVector3 *p_dutch = DutchAroundCar(pCar->GetGeometryPosition(), pCar->GetVelocity());
        bAdd(&vUp, &vUp, p_dutch);
    }

    bNormalize(&vUp, &vUp);
    pUp->SetValDesired(&vUp);

    bAngle nFov = pov->Fov;

    if (b_outside) {
        if (pCar->IsNosEngaged()) {
            if (pCar->GetVelocityMagnitude() > 0.0f) {
                nFov = nFov + NOSFovWidening;
            }
        }
    }

    pFov->SetValDesired((float)nFov);

    bool not_outside = !b_outside;

    if (bSnapNext || not_outside) {
        CubicSnap(pUp);
        CubicSnap(pFov);
        CubicSnap(pEye);
        CubicSnap(pLook);
    }

    if (bSnapNext) {
        FlushAccumulationBuffer();
    }
}

void CubicCameraMover::Update(float dT) {

    if (TheGameFlowManager.IsPaused() && !bFirstTime) {
        return;
    }

    bFirstTime = false;

    if (fIgnoreSetSnapNextTimer > 0.0f) {
        fIgnoreSetSnapNextTimer -= dT;
    }

    int pov_type = nPovType;

    nPovTypeUsed = pov_type;

    POV *pov = pCar->GetPov(pov_type);

    float fCollisionDamping = pCar->GetCollisionDamping();
    float fDrift = pCar->GetDrift();

    CubicPovData pov_data;
    aCubicPovTables[nPovTypeUsed].GetValue(&pov_data, bClamp(pCar->GetVelocityMagnitude() * 0.014084507f, 0.0f, 1.0f));

    if (TheRaceParameters.IsDriftRace() || TheRaceParameters.IsBurnout()) {
        *pov_data.GetEyeAccel() = bVector3(0.0f, 0.0f, 0.0f);
    }

    float stiffness;

    bVector3 vAccel(0.0f, 0.0f, 0.0f);

    if (fIgnoreSetSnapNextTimer <= 0.0f) {
        CameraAccelCurve(&vAccel);
    }

    pAvgAccel->Record(&vAccel);

    float fAccelH = bMax(bAbs(vAccel.x), bAbs(vAccel.y));

    if (fAccelH >= 24.0f) {
        if (pCar->GetCollisionDamping() > 0.0f && pCar->GetCollisionDamping() > vCameraImpcat.x) {
            vCameraImpcat.x = pCar->GetCollisionDamping();
            vCameraImpcatTimer.x = 1.0f;
        }
    }

    float fAccelV = vAccel.z;

    if (fAccelV >= 24.0f) {
        float fImpact = bClamp((fAccelV - 24.0f) * 0.16666667f, 0.0f, 1.0f);

        if (fImpact > vCameraImpcat.y) {
            vCameraImpcat.y = fImpact;
            vCameraImpcatTimer.y = 1.0f;
        }
    }

    pUp->SetDuration(pov_data.fUpDuration);

    pFov->SetDuration(pov_data.fFovDuration);

    pLook->SetDuration(pov_data.fLookDuration);

    bVector3 *foward_duration = pov_data.GetForwardDuration();
    pForward->SetDuration(foward_duration->x + fCollisionDamping + fDrift, foward_duration->y + fCollisionDamping + fDrift,
                          foward_duration->z + fCollisionDamping + fDrift);


    float fEyeDuration = pov_data.fEyeDuration;

    if (HighliteMode()) {
        fEyeDuration = pov_data.fEyeDuration * 0.7692307829856873f;
    }

    pEye->SetDuration(fEyeDuration);


    pUp->Update(dT, 0.0f, 0.0f);
    pFov->Update(dT, 0.0f, 0.0f);
    pEye->Update(dT, 0.0f, 0.0f);
    pLook->Update(dT, 0.0f, 0.0f);
    pForward->Update(dT, 0.0f, 0.0f);

    bool b_outside = OutsidePovType(pov->Type);

    pCamera->SetTargetDistance(bDistBetween(pCamera->GetPosition(), pCar->GetGeometryPosition()));

    if (!bPerfectFocus) {
        pCamera->SetFocalDistance(15.0f);
        pCamera->SetDepthOfField(65.0f);
    } else {
        pCamera->SetFocalDistance(0.0f);
        pCamera->SetDepthOfField(0.0f);
    }

    bMatrix4 car_to_world;

    SetDesired(&car_to_world, pov, &pov_data, bSnapNext);
    bSnapNext = false;

    float fLookbackSign = bLookBack ? -1.0f : 1.0f;

    bVector3 vUp(pUp->x.Val, pUp->y.Val, 1.0f);

    if (!b_outside) {
        vUp = *pCar->GetUpVector();
    }

    bVector3 vEye(fLookbackSign * pEye->x.Val, fLookbackSign * pEye->y.Val, pEye->z.Val);
    bVector3 vLook(fLookbackSign * pLook->x.Val, fLookbackSign * pLook->y.Val, pLook->z.Val);

    if (vCameraImpcat.x > 0.0f && vCameraImpcatTimer.x > 0.0f) {

        tTable<float> impact_table(CameraImpcatCurveH, 5, 0.0f, 1.0f);

        float fCurve;
        impact_table.GetValue(&fCurve, vCameraImpcatTimer.x);

        vEye.x *= 1.0f - fCurve * 0.25f * vCameraImpcat.x;
        vLook.x *= 1.0f + fCurve * 0.25f * vCameraImpcat.x;
    }

    if (vCameraImpcatTimer.x > 0.0f) {
        vCameraImpcatTimer.x -= dT;
    }

    if (vCameraImpcatTimer.x <= 0.0f) {
        vCameraImpcat.x = 0.0f;
    }

    if (vCameraImpcat.y > 0.0f && vCameraImpcatTimer.y > 0.0f) {

        tTable<float> impact_table(CameraImpcatCurveV, 5, 0.0f, 1.0f);

        float fCurve;
        impact_table.GetValue(&fCurve, vCameraImpcatTimer.y);

        vEye.z *= 1.0f - fCurve * 0.25f * vCameraImpcat.y;
    }

    if (vCameraImpcatTimer.y > 0.0f) {
        vCameraImpcatTimer.y -= dT;
    }

    if (vCameraImpcatTimer.y <= 0.0f) {
        vCameraImpcat.y = 0.0f;
    }

    if (b_outside) {

        float fSeconds = (WorldTimer - tLastGearChange).GetSeconds();

        if (fSeconds < 1.5f) {

            tTable<float> gear_table(CameraGearChangingCurve, 9, 0.0f, 1.5f);

            float fCurve = 0.0f;
            gear_table.GetValue(&fCurve, fSeconds);

            float fSpeed = 1.0f - bClamp(pCar->GetVelocityMagnitude() * 0.01f, 0.0f, 1.0f);

            float fMag = bAbs(vEye.x) * 0.2f;
            vEye.x += fCurve * fMag * fSpeed;
        } else if (pCar->GetVelocityMagnitude() > 10.0f) {

            if (pCar->IsGearChanging()) {
                tLastGearChange = WorldTimer;
            }
        }
    }

    bool bUnderVehicle = false;

    if (b_outside) {

        float fSeconds = (WorldTimer - tLastUnderVehicle).GetSeconds();
        const float fOvershoot = 1.2f;

        float fCurve = bClamp(fOvershoot - fSeconds * 1.25f, 0.0f, fOvershoot);

        if (fCurve > 1.0f) {
            fCurve = 2.0f - fCurve;
        }

        vEye.z += fCurve * -0.9f;

        if (fCurve > 0.0f) {
            bUnderVehicle = true;
        }
    }

    bMulMatrix(&vEye, &car_to_world, &vEye);
    bMulMatrix(&vLook, &car_to_world, &vLook);

    if (0) {
        float seconds = 0.0f;
        vEye = vLook;
        {
            float t = seconds;
            vEye = vLook * t + vUp * t;
        }
    }

    bVector3 vEyeDirection(vEye - vLook);
    vEyeDirection *= 1.0f;
    vEye = vLook + vEyeDirection;

    bAngle a_fov = (bAngle)pFov->Val;
    float f_tan_fov = bTan(a_fov >> 1);
    bAngle a_new_fov = 2 * bATan(1.0f, f_tan_fov);
    pCamera->SetFieldOfView(a_new_fov);

    bMatrix4 world_to_camera;

    eCreateLookAtMatrix(&world_to_camera, vEye, vLook, vUp);

    ApplyCameraShake(ViewID, &world_to_camera);

    float f_handheld_scale = PovHandheldNoiseScale[nPovTypeUsed];
    HandheldNoise(&world_to_camera, f_handheld_scale, true);

    if (!AmIinATunnel(&eViews[ViewID], 1)) {
        float f_chopper_scale = PovHandheldChopperScale[nPovTypeUsed];
        float speed_attenuation = bClamp(pCar->GetVelocityMagnitude() * 0.033333335f, 0.0f, 1.0f);

        ChopperNoise(&world_to_camera, f_chopper_scale * (1.0f - speed_attenuation * 0.8f), true);
    }

    float f_speed_scale = PovVelocityNoiseScale[nPovTypeUsed];
    float f_terrain_scale = PovTerrainNoiseScale[nPovTypeUsed];
    TerrainVelocityNoise(&world_to_camera, pCar, f_speed_scale, f_terrain_scale);

    if (b_outside) {

        MinGapTopology(&world_to_camera, pCar->GetGeometryPosition());

        if (!bUnderVehicle) {
            MinGapCars(&world_to_camera, pCar->GetGeometryPosition(), pCar->GetVelocity());
        }
    }

    pCamera->SetCameraMatrix(world_to_camera, dT);
}
