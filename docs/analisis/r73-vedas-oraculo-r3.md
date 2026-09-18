# r73: vedas ronda 3 — el oráculo contra los cinco near-miss, anatomía completa de cuatro

Ronda de vedas con `dwarf1.py` + banco + fuente del compilador. **Ninguna cierra; cuatro
quedan con la divergencia explicada al insn y la palanca restante cuantificada.** GC
intacto en las cuatro unidades (zCamera 120412/125008, zWorld 159092/160956, zWorld2
135592/139388, zEagl4Anim 110208/113016). Herramientas: bench/measure1/dumpbench de
vedas_oraculo (el banco de RawStateChan necesita `EXTRA_INC=src/Speed/Indep/Src/EAGL4Anim`)
y commit.py propio en tools/scratch/vedas_r3.

## 1. HolePunchAvoidables (zWorld2, 2.980 B, 99,97%): el sandwich identificado

Las 4 filas siguen siendo el swap cr2/cr3. Leído el `.lreg` con mis propios ojos:
**334 (racer) 3 refs/494; 505 (traffic) 3/483; 901 (drag) 3/492** — pri
`int(30000/live)` = 60/62/60, empate 60/60 roto por nº de allocno (334<901), y el
objetivo reparte drag→cr3, racer→cr2.

La cadena del preencabezado (post-sched1):

```
[~14 pares high+lo_sum de literales] cmp334, lit0.2D(3341), cmp901, lit-6(3325),
addi r31+K ×5, high IHandle, high TRAILER, cmp505, ...
```

**El insn del sandwich NO es el literal de `extra_width`** (r50): el `(1.0f-0.8f)`
pliega a **0x3E4CCCCC** (LC558, carga en 3319, ANTES de cmp334). El del sandwich es
**el `0.2f` de `new_current_offset` (l.2263)**: pliega vía double a **0x3E4CCCCD**
(LC561, insn 3341, consumido por el insn 2727 `mult delta_offset(86)×882` con nota de
línea 2263). Dos constantes "0.2" distintas en la misma función — el objetivo tiene
las dos en el pool.

**El orden de nacimiento es irrelevante** (3 builds): base; producto de extra_width
fusionado y movido DETRÁS del ternario → 88 filas (floats rotos, gap relleno); fusionado
en el sitio → 76 (el fold cambia a ...CCC y rompe literales). El hueco siempre lo rellena
UN literal: en `rank_for_schedule` (haifa-sched.c:4158) los cmpwi y las cargas lo_sum
pesan `#sets−#deads = 0` y los `addi r31+K`/`high` pesan 1; cmp505 pesa 1 (su bool no
muere: 4 usos) y por eso cae al final del grupo peso-1 con LUID intacto.

El volteo exige int(30000/live901) > int(30000/live334): live901 ∈ {484..491} con
live334 ∈ {493..500}. O sea, EXTRAER exactamente 1 insn de [def334..fin de bucle]:
el hueco del sandwich, uno de los 8 movables, o la barrera. Los 8 movables existen en
el objetivo (filas casan), la barrera es el ancla de los floats (r48/r65/r67b: sin ella
46 filas). Veda con anatomía completa; comentario actualizado en WRoadNetwork.cpp.

## 2. EvalState FnRawStateChan (zEagl4Anim, 456 B): por qué ninguna forma de fuente basta

La d-form en el accesor (`unsigned short *d = &mDecodeData[mNumFields];` ANTES del if,
brazos `d`/`d+1`) es la forma que produce la estructura del objetivo en los bucles —
**y está medida por primera vez: 69 filas / 412 B**. Falla porque el FRONT-END unifica
al expandir: TAMBIÉN unifica la llamada del if-header (filas 7-17), que el objetivo
tiene RAMIFICADA (dos `slwi`, filas 10/14, con el `add` con operandos conmutados entre
ramas = dos árboles separados).

El DWARF zanja la estructura: los CINCO rangos inline de GetKeyData casan EXACTO en los
dos lados (52/12/36/12/56 B). Los de 12 B (bucles while y for) son SOLO los brazos
`addi +2`/`mr r4,r11`; la parte común `c+2nf+10` queda FUERA del DIE. En el objetivo la
misma fuente ramificada se unificó SOLO dentro de bucles (hoist + `combine_movables` de
loop.c casa los dos pseudos 2nf+10) y nunca en paso único. Nosotros: el brazo impar
RECALCULA `add r4,r30,r11` donde el objetivo COPIA `mr r4,r11` — r50/r62 en pie. Veda:
la unificación tiene que darla el optimizador, no la forma. Comentario en RawStateChan.cpp.

## 3. DefragmentPool (zWorld, 684 B, 99,27%): aparcada con el andamio confirmado

El diff DWARF (dw1ours) confirma la desviación estructural: el objetivo NO tiene las
locales `params`/`table`/`zero` (nuestras r26/r17/r18 dentro del inline bMalloc).
Quitarlas ya estaba medido (r46: 47 filas/692 B — el daño se va a `stmw r15` +8 B).
Los tres ciclos (r21↔r22, r16→r18→r17→r16, r25↔r27) siguen aparcados per r68/r68b
(CONST 2/7=2857 vs SYM/LIT; la batalla r25/r27 la pierde el contador contra el
Chunk-high de pri 4444). Solo reabrir con forma sin cantidad viva extra.

## 4. __static_init zCamera (3.604 B, 99,84%): local-alloc, ventanas falsas ±2, y el LIT bloqueado

Tres piezas nuevas sobre la veda r50/r64:

1. **Swap de definiciones medido** (NOSFov antes de Hydraulics): **7 filas** — empeora
   (la 535 se une). Revertido; el orden actual es el mejor de los dos.
2. **Reencuadre del asignador**: la función es UN bloque básico → las tres cantidades
   (CONST 858: 2/7=2857→r10; SYM 874: 2/10=2000→r11; LIT 965: 2/36=555→r9) son de
   **LOCAL-ALLOC**. `QTY_CMP_PRI` (local-alloc.c:1568) =
   `floor_log2(n_refs)*n_refs*size/(death−birth)*10000` — coincide con lo de r64 para
   n_refs=2/size=1, PERO `find_free_reg` prueba primero con **ventanas falsas ±2**
   (birth−2..death+2, activas con `-fschedule-insns-after-reload`): dos cantidades de
   vidas reales disjuntas pelean el registro si nacen/mueren a ±2. La batalla r10/r11/r9
   es de VECINDAD con las otras veintitantas qtys de pri 3333.
3. **live(LIT)=36 es estructural y está bloqueado**: el high de $LC1122 (12000.0f) lo
   comparten por CSE `vCopViewDistanceFovBand[0].y` (CopView.cpp:11) y
   `vCopViewDistanceFov[0].y` (:26), ~47 insns de distancia; las direcciones .bss
   anotadas (0x8045AE14/0x8045AE98) prueban que el objetivo tenía las definiciones en
   ESTE orden — reordenar CopView.cpp rompe el layout. Única vía viva: el fantasma de
   JoyE (r62) sobre r10 tapando la ventana falsa [~2837..~2848] del CONST para
   empujarlo a r9. Comentario en Cubic.cpp.

## 5. SetMemoryPoolSize (zWorld, 304 B): no reabierta

Veda r68/r68b en pie (la inversión addi/stw vive en las entradas de sched2). El brief
la lista pero no traía palanca nueva; el tiempo se fue a los cuatro de arriba.

## 6. Estado final y contabilidad

| unidad | antes | después | filas abiertas |
|---|---|---|---|
| zCamera | 120412/125008 | igual | static_init 6 |
| zWorld | 159092/160956 | igual | DefragmentPool 23, SetMemoryPoolSize ~2 |
| zWorld2 | 135592/139388 | igual | HolePunch 4, InitAtSegment 12 |
| zEagl4Anim | 110208/113016 | igual | EvalState 18, Initialize 8 |

Variantes medidas esta ronda: HolePunch V-A 88 / V-B 76; EvalState d-form 69/412;
static_init swap 7. Todas revertidas; árbol devuelto a la base (4 unidades recompiladas
y verificadas). Negativas añadidas como comentarios en las tres fuentes.

Lección transferible: cuando la función es un solo bloque básico, el reparto lo hace
local-alloc con ventanas falsas ±2 — el modelo de global.c (r72) no aplica y las
prioridades hay que leerlas del `.lreg` como `QTY_CMP_PRI`; y en bucles, el insn que
se cuela entre dos comparaciones izadas lo elige el peso `#sets−#deads` del
planificador, no el orden de nacimiento en la fuente.



## r74: HALLAZGO CRÍTICO — 4 versiones del compilador en el ELF original

El .debug del GC registra la versión del compilador en cada CU. Hay CUATRO:

| versión | CUs | unidades |
|---|---|---|
| v1.76 (3.9.3) | 260 C++ + 28 C | el estándar del juego |
| **v1.72** | **13 C++** | **rcmp completo**: madidct, maddec, maddeca, avplayer, audioplayer, rcmpbase, rcmp_mad/vp6_codec, bigswizzler |
| **v1.83** | **15 C++** | **path completo**: pathnode, pathserv, pathbank, pathreal, pathreal6, pathinit, pathaction, pathevent, pathcontrol, pathrand, pathdebug, author |
| v1.76 C | 28 C | las unidades .c del juego |

**Qué significa**: las vedas de reload en madidct y las dificultades en
path pueden ser diferencias de ASIGNADOR entre v1.72/v1.83 y v1.76. Estamos
compilando TODO con v1.76 (ProDG 3.9.3) — las 28 unidades con versiones
distintas producen registros distintos.

**Qué tenemos**:
- `build/compilers/ProDG/3.9.3` = v1.76 (instalado, operativo)
- `orig/prodg/NGC_GNU_SRC/NGC/gcc/` = **el fuente COMPLETO del compilador, v1.79** (entre las versiones del ELF)
- Instalados también: 3.5 (v?), 3.7 (v1.46), 3.8.1 (v1.55)

**La vía para eliminar los pins de rcmp y path**:
1. Modificar `sn_version.h` del fuente a v1.72 o v1.83
2. Compilar el cc1plus (Makefile presente, proyecto bootstrap GCC)
3. Añadir el compilador a build/compilers/ y configurar las 28 unidades

Esto también abre la pregunta: ¿hay MÁS diferencias entre v1.72/v1.83 y
v1.76 más allá del asignador? El diff del fuente entre la versión del
árbol (v1.79) y lo que produce v1.76 puede cuantificarlo.
