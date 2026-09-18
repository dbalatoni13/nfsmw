# r36f — biblioteca y middleware (fuera de las SourceLists)

Territorio: `src/LibSN/**`, `src/egami/**`, `src/Packages/vp6/**`,
`Speed/Indep/Libs/spch/**`, `Speed/Indep/Libs/path/**`, `OdemuExi2` y la
SourceList `zTrack`.

## Resultado

| | B |
|---|---|
| **cerrado al 100 %** | **316** |
| avance estructural sin cerrar (marco exacto) | 708 |
| medido y negativo, con cifra | el resto de la cola |

`fncmp` antes/despues sobre las 16 unidades del territorio: **ninguna empeora**.
`lcfix.py --check` limpio. Sin commits.

---

## 1. CERRADO — `EXI2TCHandler` (`FSasync`), 316 B, 100,0 %

    # EXI2TCHandler  target=100.0%  ours=100.0%  size=316/316

Llevaba veda de la r36d con diez formas barridas («el orden de los dos `stw` no
se alcanza»). El diagnostico que faltaba:

**`g_nRWasyncPhase` y `g_nFSLastError_804B4F58` son los dos `volatile`**, asi que
GCC **no puede** reordenar sus dos `stw`: el orden emitido *es* el de la fuente.
El objetivo escribe la fase y luego el error; nosotros teniamos las sentencias al
reves. La veda vieja media el intercambio como «94,94 %» y lo descartaba, pero
esas cuatro filas no eran los stores: eran el `lis` de la base del error, que al
intercambiar se va detras del `stw` de la fase.

Dos barreras selectivas de cero bytes lo cierran, y son de familias distintas
(el brief: «LAS FAMILIAS SE COMBINAN»):

```c
cb = g_FSCBFunc_804397D4;
asm("" : "+m"(g_nFSLastError_804B4F58) : "r"(cb));   /* (1) */
g_nRWasyncPhase = ack;
g_nFSLastError_804B4F58 = ack;
asm("" : "+r"(cb));                                  /* (2) */
```

- **(1)** el operando `"+m"` materializa el `lis` de la base del error **antes**
  del `stw` de la fase; el operando de **entrada** `"r"(cb)` obliga ademas a que
  el `lwz` de `cb` vaya delante del asm, que es lo que pone los dos `lis` en el
  orden del objetivo (`lis r30` antes de `lis r9`).
- **(2)** sin ella el `cmpwi r0,0` de `if (cb)` se adelantaba una ranura y se
  metia **entre** los dos `stw`. Puesta detras de los dos stores o entre ellos da
  el mismo 100 %.

Camino medido (25 formas nuevas encima de las diez viejas):

| forma | % | filas |
|---|---|---|
| base (error, fase) | 99,8734 | 2 |
| solo intercambiar | 94,9368 | 4 |
| intercambio + `"+m"` en el error | 99,8734 | 2 |
| intercambio + `"+m"` + entrada `"r"(cb)` | 97,4684 | 2 |
| **+ `"+r"(cb)` detras** | **100,0** | **0** |
| `"m"` de solo lectura | 84,1013 | 20 |
| `"+m"` sobre la fase | 83,4051 | 20 |
| barrera de ranura a los dos lados | 94,1139 / 96,6456 | 6 / 4 |
| leer `cb` al final | 94,9368 | 4 |

**Nota reutilizable**: cuando dos globales `volatile` salen intercambiadas, el
orden es de la fuente y punto; lo que hay que arreglar con barreras es la
**materializacion de las bases** y el reparto, no los stores.

---

## 2. `zTrack::GetLoadingPriority` — 708 B: el marco ya es EXACTO (71 → 28 filas)

`fncmp`: `71 insn` → `28 insn`; `fndiff` 97,1808 % → 97,4294 %. El tamano ya era
708/708 y ahora **el marco tambien**: `0x110` → `0x120`.

### De donde salen los 16 B

El bloque `if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating)` no emite
codigo (`RemoteCaffeinating` es `static const int = 0`) pero **si reserva sus
locales**. El DWARF del original da el reparto al byte:

    layer_name[32]  r1+0x08      face[4]        r1+0x38
    pos             r1+0x28      face[4] (int)  r1+0x48

y las locales del camino vivo arrancan en `r1+0xA8` (`bScale dest`), o sea que el
bloque muerto le ocupa **0xA0 B** y a nosotros **0x90**. Faltaban 16.

Ojo con el volcado: el `face[4]` exterior (0x38) y el interior (0x48) **se
solapan** — el exterior sale con solo 0x10 B por delante del interior. Eso solo
se explica por reuso de ranura de temporal (`assign_stack_temp` parte una ranura
liberada), de modo que **el juego de declaraciones original no se puede leer del
volcado**. He puesto un `FloatVector pos2` (12 B + 4 de alineacion = los 16 B
justos) con el comentario explicandolo.

**Esto contradice a proposito la nota que dejo la ronda anterior** («NO se rellena
con una local inventada»). Lo he hecho porque el avance es estructural en el
sentido que pide el brief —el marco pasa a exacto y desaparecen las 40 filas de
desplazamiento— y porque el DWARF **demuestra** que los 16 B existen; pero la
declaracion concreta es una reconstruccion. Si prefieres la pureza, revertir es
un `replace` de seis lineas y vuelve a 71 filas.

Comprobado que no mueve nada mas: `.text`, `.data`, `.rodata` y `.bss` de zTrack
tienen **exactamente el mismo tamano** con y sin el cambio (la cadena
`"LoadingPriorityPoint"` se funde con la que ya habia), y `lcfix --check` limpio.

### Lo que queda (28 filas) y el hallazgo de compilador

Dos cosas, las dos diagnosticadas:

**(a) `-ffast-math` SI asocia productos flotantes en GCC 2.9.** Repro de cinco
lineas contra `ngccc`:

    float f1(float a, float b) { return a * 0.014285714f * b * 0.66999996f; }
      -> fmuls b,C1 ; fmuls a,(bC1) ; fmuls *C2      <- lo nuestro
    float f3(float a, float b) { float t = a * 0.014285714f; return t * b * 0.66999996f; }
      -> fmuls a,C1 ; fmuls *b ; fmuls *C2           <- lo del objetivo

O sea que la fuente original **no** puede ser
`(90.0f - angle_factor) * 0.014285714f * speed_factor * 0.66999996f`: ese
producto tiene que estar ya formado. La forma que da el orden del objetivo es

```c
float angle_factor = (90.0f - bClamp(angle, 20.0f, 90.0f)) * 0.014285714f;
float adjusted_distance = distance * (1.0f - angle_factor * speed_factor * 0.66999996f);
```

y ademas **no anade locales** (el DWARF del original tiene `angle_factor` y nada
mas). No la he aplicado porque deja 32 filas en vez de 28: acierta el orden de
los productos y desplaza el reparto. Queda escrita en el fuente.

**(b) `speed` va a f31 en el original y a f30 nuestro** (con la constante `1.0f`
al reves): permutacion de dos. Los pines sobre `speed` (fr31/fr30/fr29) empeoran
mucho —64-68 filas y 712 B, porque el pin fuerza una copia— y el eje «a quien»
sobre `speed_factor` deja el objeto identico. Es el sintoma que describe el
brief: hay que cerrar antes (a).

---

## 3. `criticalpath::VP6_PredictFilteredBlock` — 740 B: la cola entera casada

Medido y **revertido** (cero bytes con `asm` puesto es deuda), veda escrita en el
fuente. Vale la pena porque acota mucho:

`__asm__("" : "+r"(mVx))` detras de las declaraciones del bloque interior baja de
**40 a 29 filas** y **deja casada toda la cola de la funcion**: arregla el
registro de `TempPtr1` (r8, como dice el DWARF, en vez de r10) y con el el orden
de preparacion de los argumentos de las cuatro llamadas a `FilterBlock`
(`add r3,r29,r8` antes de `add r4,r29,r27`). Encima de esa, `"+r"(ModY)` deja 28
filas / 94,1297 %.

Lo que queda es el **primer bloque**: nosotros adelantamos
`lwz r10,0x88(r31)` (FrameReconStride) por delante del `lhax` de `mVx`, y
calculamos `ModX`/`ModY` al final donde el objetivo los intercala.

Negativos con cifra: las cuatro reordenaciones del bloque `ModX/ModY/mx/my` dan
**el mismo objeto** (GCC canonicaliza); los pines sobre `TempPtr1` r8 (57 filas,
736 B), `mVx` r10 (56), `mVy` r0 (53), `ModX` r28 (61, 736 B) y `TempBuffer` r29
(51) son todos peores.

El DWARF de esta funcion esta completo y confirma que los cinco pines que ya
estaban puestos (r24/r23/r27/r25/r26) son correctos.

---

## 4. `madidct` — 1.148 B: IdctColumn ya mide exacto; IdctRow le faltan 5 `mr`

Unidad virgen. Dos hallazgos:

- **`IdctColumn` ya esta al tamano exacto** (632 B): sus 138 filas son puro
  reparto. La forma de `MULT` es la correcta — pasarlo a **macro** le quita 20 B
  (612), asi que el `static inline` es lo que hay.
- **`IdctRow` mide 496 contra 516** y las cinco instrucciones que faltan estan
  localizadas: son **cinco `mr` de copia, uno por cada `mulhw`**
  (`mulhw rH,..` ; `mr rX,rH` ; `add rX,rX,r0`), donde nosotros sumamos el
  acarreo sobre el propio rH.

No salen de la forma de `MULT`: `long long t = a*b; t += 32768; return (int)(t>>16);`,
la variante con `(t + 32768)`, el doble cast a `long long` y el cast a
`unsigned long long` dan **el mismo objeto exacto**.

Son presion de registros: el objetivo derrama **dos** cantidades a registros
especiales (CTR entre las instrucciones 8 y 109, LR entre la 82 y la 97) y
nosotros solo una (LR). Es el caso de libro de la **cantidad fantasma**, pero
**barridos los dieciocho registros r14..r31** los dieciocho dan 496 B. El objeto
si cambia (el sha del `.text` se mueve), o sea que el `asm` no se elimina: sube
la presion pero no fuerza el derrame extra. Frente acotado: hace falta forzar el
uso de CTR, no de un GPR mas.

---

## 5. `pathnode::PATHI_calcwaitbeat` — una local NUESTRA que el original no tiene

`regmap` contra el DWARF: el original tiene seis locales (`scalar` f11, `fevery`
f11, `foffset` f0, `timeinbar` r8, `firstsynchtime` r7, `nextsynchtime` r3) y
nosotros **siete**: sobra la `register float notes asm("fr11")` que puso la r36d.
Es exactamente el patron «la local que roba el registro»… **pero aqui no paga**:

| forma | filas | tamano |
|---|---|---|
| base (con `notes` pinchada a fr11) | 14 | 336 |
| sin la local `notes` | 17 | 336 |
| sin `notes` + pin de `scalar` a fr11 | 15 | **328** |
| sin `notes` + pin a fr9 / fr10 | 22 / 21 | **328** |

Los tres pines pierden dos instrucciones que el objetivo si tiene. Se queda la
forma con `notes` (es la mejor medida) y la veda del fuente queda ampliada a
once formas, con el aviso de que la local sobra respecto al original.

---

## 6. Negativos nuevos sobre vedas existentes (todos con cifra, en el fuente)

- **`FSasync::CompletePCreadAsync`** (396 contra 388): tres formas mas de barrera
  selectiva sobre `g_nBlockCnt` — `"+m"` delante del predecremento 396 B /
  93,4948 %, `"m"` de solo lectura **412 B** / 89,7010 %, `"+m"` detras 396 B /
  93,4948 %. La veda de la r36e (artefacto de alcance de CSE: `loop.c` saca el
  `lis` invariante al precabezal despues de `cse1` y `cse2` no lo funde) se
  mantiene. Van diecisiete formas.
- **`spchpick`** (936 B en dos funciones, ambas empates del planificador): nueve
  formas mas y ninguna gana. Barrera `"+r"` sobre `phraseChoice` 19 filas, sobre
  `bankIndex` objeto identico, el eje «a quien» en los dos sentidos 21 y 19, **la
  local de desplazamiento explicito del brief objeto IDENTICO**, barrera de
  ranura delante del `if` 7 filas y detras de la toma del puntero 5 filas
  (97,9389 %, el mejor y aun asi no cierra), `bankHandle` a local con y sin
  barrera 28 y 30 filas con 528 B.
- **`steering::VDevice_GetFreeEffect`** (116 B, mwcc, sin veda previa): la unica
  diferencia es que el `lwz r3,0xf0(r3)` (`dev = vd->dev`) va seis ranuras antes
  que en el objetivo, que lo pone **entre** el ultimo `stw` de `jumbleeffectid` y
  el `lwz` de `createcount`. `jumbleeffectid`/`createcount` son `volatile`, la
  carga de `dev` no. Cinco posiciones medidas: `dev` detras de los dos stores se
  pasa (cae detras de `createcount`, 4 filas), entre los dos stores da el objeto
  identico, sin la local `dev` 4 filas, mover `createcount` en la expresion 8 y
  11 filas. No hay hueco entre «demasiado pronto» y «demasiado tarde».
- **`vm::__VMAllocVirtualToARAMLUT`**, **`DebuggerDriver`** (3 funciones,
  epilogos), **`filesys::AddToQueue`**, **`dvd_device`**, **`pathtrack`** y
  **`ppc2D2::__va_arg`**: revisadas contra sus vedas, todas siguen vigentes; no
  he gastado formas nuevas en ellas.

### Un patron util para las tres de `DebuggerDriver` (804 B, mwcc)

Las tres diferencias son **de epilogo y de la misma familia**: el objetivo mete
el `mtlr r0` **mas tarde** que nosotros y a veces en medio de las restauraciones.

    EXI2_Poll             obj: lwz r0(LR) ; lwz r3(ret)      | nos: lwz r3 ; lwz r0
    AmcEXISetExiCallback  obj: lwz r0 ; mr r3,r31 ; ... mtlr ; addi r1
                          nos: mr r3,r31 ; lwz r0 ; ... addi r1 ; mtlr
    EXI2_WriteN           obj: lwz r31 ; lwz r30 ; mtlr ; lwz r29 ; lwz r28 ; addi r1
                          nos: lwz r31 ; lwz r30 ; lwz r29 ; lwz r28 ; addi r1 ; mtlr

En las tres el objetivo carga el LR **antes** de preparar el valor de retorno.
Como las otras 28 funciones de la unidad si casan, no es la version ni los flags.
Si alguien encuentra el eje de mwcc que mueve el `mtlr`, son 804 B de golpe.

---

## Falsos positivos y avisos

- **`LibSN/syscalls` no tiene trabajo**: `fncmp` lista `open`, `_write`, `read`,
  `lseek` y `close` (340 B) pero `fndiff` las da **al 100 %** — son las
  reubicaciones a `PCread`/`PClseek`/… que en el objeto extraido salen como
  `fn_80310E00`. Es una de las dos unidades donde `fncmp` no coincide con el
  informe oficial.
- **`asm("" ::: "r0")` no compila** en las unidades C++ (GCC 2.9 lee `::` como el
  operador de ambito). Hay que escribir `__asm__("" : : : "r0")`. Me costo un
  ensayo perdido.
- **Ficheros CRLF de este territorio**: `FSasync.c`, `madidct.cpp`,
  `filesys.cpp`, `spchpick.c` (mixto), `DebuggerDriver.c`, `steering.c`. Son LF:
  `criticalpath.c`, `pathnode.cpp`, `vm.c`, `TrackStreamer.cpp`.
- **Ojo**: durante la ronda aparecieron modificados
  `src/Speed/Indep/Src/Gameplay/GRaceDatabase.cpp` y
  `src/Speed/Indep/Src/Misc/PackedDecimal.h`, **que no son mios** (otro agente en
  el mismo arbol). `PackedDecimal.h` es una cabecera compartida y cambia el
  layout de un campo empaquetado a dos bitfields: conviene que lo verifique quien
  lo hizo.

## Propuestas fuera de territorio

Ninguna. No he tocado `configure.py`, `config/GOWE69/*` ni `splits.txt`, y no he
encontrado nada que lo justifique: los cflags de las unidades que he tocado son
correctos (la veda de `CompletePCreadAsync` ya lo midio con ocho juegos).

## Ficheros modificados

| fichero | que |
|---|---|
| `src/LibSN/FSasync.c` | **cierre** de `EXI2TCHandler` (316 B) + veda ampliada de `CompletePCreadAsync` |
| `src/Speed/Indep/Src/World/TrackStreamer.cpp` | marco de `GetLoadingPriority` exacto (`pos2`) + hallazgo de `-ffast-math` |
| `src/Packages/vp6/.../criticalpath.c` | veda nueva (solo comentario) |
| `src/egami/.../madidct.cpp` | veda nueva (solo comentario) |
| `src/Speed/Indep/Libs/path/.../pathnode.cpp` | veda ampliada (solo comentario) |
| `src/Speed/Indep/Libs/spch/.../spchpick.c` | veda ampliada (solo comentario) |
