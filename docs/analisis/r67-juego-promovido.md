# r67 — lote `juego-promovido`: 18 andamios de código de juego en unidades promocionadas

**Encargo**: dejar fuente legítima. Test único: digests por sección de las ALLOC
(`.text`/`.over`, `.rodata`, `.data`, `.bss`, `.ctors` y sus `.rela.*`), ignorando
`.line`, `.debug*`, `.comment`, `.stab*`, compilando a `.o` **privados**
(`scratchpad/jprom67/o/`), nunca a `build/GOWE69`.

## 1. Cifras

| | |
|---|---|
| andamios del lote (pines + `__asm__("")`) | **18 → 11** |
| retirados | **7** (5 pines de `Strings.cpp`, 2 pines de `QuickGame::OnManageTime`) |
| fuera del censo, retirado de paso | 1 `asm("" : "+r"(c2))` de `bStrNICmp` |
| locales inventadas retiradas con ellos | `c1`, `c2`, `c1wide` (Strings); `game_speed`, `initial_speed`, `release_speed` y una expresión-sentencia GNU (OnManageTime) |
| irreducibles con diagnóstico r67 junto al andamio | **11** (8 funciones) |
| variantes medidas | 22 compilaciones de unidad (+ 6 volcados RTL); todas revertidas salvo las dos formas aceptadas |
| regresiones | **0**: las seis unidades dan su digest base exacto |
| propuestas de `configure.py` / `config/GOWE69` | ninguna |

Controles que pueden fallar y fallan: las dos formas sin andamio de la r66 se
reprodujeron al byte antes de medir nada (`ConvertWheelRotation` sin pines
`10b8f74f747da06a`, `OnManageTime` V0 `bd8ab33e5d64c04f`), y cada retirada se
comprobó además en el **DWARF del `.o` nuevo** (las locales del andamio ya no
están), porque un «IDÉNTICO» contra un `.o` rancio no prueba nada.

## 2. Herramientas (`scratchpad/jprom67/`)

* `cc.py`, `tool.py`, `d.py`, `sym.py` — los de `resto66` con la salida a mi
  directorio (`d.py` con JSON por proceso, para poder correr unidades en paralelo).
* `v.py <spec>` — **variante**: aplica reemplazos exactos a UN fichero del lote,
  compila la unidad a `.o` privado, compara digest con la base, saca las filas de la
  función contra el `.o` original y **revierte siempre** (byte a byte). Opcional:
  volcados RTL (`DUMP`, `DUMPFLAGS`, `EXTRA`).
* `rtldump.py` — copia del de `scripts/` con los volcados en `jprom67/rtl/`.
* `rtlfn.py <volcado> <función>` — cada insn de una función en una línea.

Bases (medidas al empezar, iguales a las de la r66 donde había):

| unidad | ficheros del lote | ALLOC base |
|---|---|---|
| zBWare | `Strings.cpp` | `51df0949941a0339` |
| zSim | `QuickGame.cpp` | `6913d4cebd1c34a7` |
| zFeOverlay | `CarCustomize.cpp` | `9e4d806d60a97306` |
| zMain | `SteeringWheelDevice.cpp`, `EPlayRaceNIS.cpp`, `InputDeviceGC.cpp` | `cc5a538171d2ac53` |
| zFEng | `FEngine.cpp` | `3869f721cf92634e` |
| zDynamics | `Geometry.cpp` | `fd74052ef8a3c7c5` |

## 3. Retirados

### 3.1 `bStrNCmp` y `bStrNICmp` (zBWare) — 5 pines + 1 barrera → 0

**La pista estaba en el mapa de líneas, no en el DWARF.** El DWARF del original no
tiene ni una local en ninguna de las dos (por eso `regmap` no decía nada), pero
`lmap` pone **toda la cola** de `bStrNCmp` (`cmpwi r5,0` … `subf`) en una sola
línea, `Strings.cpp:116`, y la de `bStrNICmp` en la 143. Una línea es una
sentencia: el original era **un `return` con ternarios**.

```c
return n >= 0 ? (*s1 == '\0' ? (*s2 == '\0' ? s1[-1] - s2[-1] : -1) : 1) : 0;
```

Por qué casa sin pines (volcado `.lreg` de la forma llana): `expand_return`
(stmt.c) calcula un `return` con valor en un **pseudo** y lo copia a r3 al final. Con
if/else y un `return` por rama, la resta se escribe directamente en r3 y local-alloc
le sugiere r3 al operando que muere (`lbz r3,-1(r4)`). Con el ternario el destino es
un pseudo global, no hay sugerencia, y el reparto por prioridad da s2[-1]→r0 y
s1[-1]→r9, que es el objetivo. En `bStrNICmp` el mismo cambio hace lo contrario: las
dos expansiones de `bToUpper` dejan de competir por r3 con la resta, y reaparece el
`mr r11,r3` que le faltaba a la forma llana (+4 B).

| medida | zBWare ALLOC | filas |
|---|---|---|
| base (5 pines + barrera) | `51df0949941a0339` | 0 / 0 |
| `return s1[-1] - s2[-1]` dentro del if/else, sin pines (control) | `a0827f47e006fd88` | 5 (r0/r3) |
| **ternario en bStrNCmp** | **`51df0949941a0339`** | **0** |
| **ternario en bStrNICmp** | **`51df0949941a0339`** | **0** |
| **las dos en el árbol** | **`51df0949941a0339`** | **0 / 0**, sin locales en el DWARF del `.o` |

### 3.2 `QuickGame::OnManageTime` (zSim) — 2 pines → 0

La r66 lo dejó en «registros buenos u orden bueno, no las dos cosas» (V2, 2 filas).
Los volcados de la r67 dan las tres causas, y las tres se separan con **una sola
decisión: literal en la primera comparación, alias en las recargas.**

1. **Reparto.** Sin pines (V0), `allocno_compare` pone la carga del 1.0 (pseudo 88,
   3 refs / 6 insns → 5000) delante de la de Tweak (85, 3 / 8 → 3750), y se lleva
   f0. La carga de un 1.0f **de pool** trae `REG_EQUAL` constante:
   `local-alloc.c:881` la promociona a `REG_EQUIV` y la **931 le duplica
   `REG_LIVE_LENGTH`** → Tweak gana. El alias `lbl_80404864` es una MEM sin nota y no
   se duplica.
2. **Orden.** `sched1` (`-fsched-verbose-5`) saca los dos `lis` en t=1 y las dos
   cargas por **LUID**: el nivel de «clase respecto al último insn» de
   `rank_for_schedule` no actúa porque `insn_cost == 1`. La carga que va primero en
   el RTL sale primero en el `.s` y, a la vez, vive más. Con el alias no hay forma de
   tener las dos: V6 (1.0 declarado antes) 2 filas; V7 (asignación dentro de la
   comparación, que da el RTL `[hiT, hiL, L, T]`) 4 filas, `ab4e709ba5277166`.
3. **GCSE.** Con el literal en *todas* partes (la forma natural; r51/V5, 28 filas)
   GCSE comparte el `(high $LC523)` de este bloque, que domina las recargas. Con el
   alias en las cinco recargas la expresión ya no es la misma.

| variante | zSim ALLOC | filas |
|---|---|---|
| V0 sin pines (control r66) | `bd8ab33e5d64c04f` | 5 |
| V6 1.0 cargado antes, sin pines | `188a48e4b5451a77` | 2 (= V2 de la r66) |
| V7 `Tweak != (initial_speed = lbl)` | `ab4e709ba5277166` | 4 |
| V8 literal en la comparación + alias abajo | **`6913d4cebd1c34a7`** | **0** |
| **V9 = V8 sin `release_speed` ni la expresión-sentencia** | **`6913d4cebd1c34a7`** | **0** |

Queda V9 en el árbol. El DWARF del `.o` nuevo lista exactamente las locales del
original (`target_speed f31`, `delta_speed f13`, `player1 r31`, `view`,
`camera r11`, `camera_time`).

**Aviso para el revisor.** El encargo decía que en `OnManageTime` tocara «solo esas
dos declaraciones» si probaba la palanca (a). Esta retirada no es la palanca (a) y
toca más: la primera comparación (literal), las cinco recargas del 1.0 (`1.0f` →
`lbl_80404864`) y la expresión-sentencia de `release_speed`. **El bloque del `.set
lbl_80404864, $LC523` no se ha tocado** (no aparece en el diff), y `.rodata` y las
`.rela.*` salen idénticas, así que el alias sigue resolviendo al mismo `$LC523`.
Deuda que queda, y está escrita junto al código: el alias en cinco sitios en vez de uno.

## 4. Irreducibles (11), con el diagnóstico escrito junto a cada andamio (marca r67)

| sitio | función (unidad) | clase | lo nuevo de la r67 |
|---|---|---|---|
| `QuickGame.cpp` pin r4 + `asm` | `CreateCars` (zSim) | sched1 / expand_call | sin los dos `c194a1d2cfdc9749`, 4 filas (+4 B). `.lreg`: sched1 deja `r3 = this` (insn 153) delante del 2.º store del constructor (138); el `this` sigue vivo tras la copia y local-alloc no lo ata a r3 → `addi r11` + `mr r3,r11`. El DWARF no da local para el temporal. |
| `CarCustomize.cpp` pin r21 | `CustomizeParts::Setup` (zFeOverlay) | reparto | `.greg`: vinyl 12/1388 → 259, installed_index 4/318 → 251. Margen: quitar ≥ 13 insns pre-combine del bucle (`8/(318-x) > 36/(1388-x)`) o añadir ≥ 44 antes de `installed_index = 0`. Sin forma legítima. |
| `CarCustomize.cpp` 3 barreras | `CustomizeMain::NotificationMessage` (zFeOverlay) | reparto + expand_call | sin la de `mgrp`: `dbe03f26e7d01c07`, 5 filas (r31↔r28, lo de la r65). El DWARF del bloque no trae nada muerto que dé la cuarta referencia. |
| `SteeringWheelDevice.cpp` 2 pines FPR | `ConvertWheelRotation` (zMain) | reparto local/global | `.lreg`: las cargas de 0.8f/0.2f son de local-alloc y se llevan f0/f13 antes de que global vea el Clamp (f12). En el objetivo f0 no estaba libre en la cola. Mapa de líneas: todo en la 659 salvo el `fmadds` (668). W1 `val = …; return val;` → `10b8f74f` (= sin pines); W2 copia local → `fe805bbc23b707a5`, 13 filas. |
| `FEngine.cpp` pin r30 | `ProcessPadsForPackage` (zFEng) | reparto | `.greg`: 409 (`1<<PadIndex`) 9/13 → 20769; 414 (`PadIndex*0x88`) 18/33 → 21818. Condición necesaria: −1 insn en la vida de 409 o +2 en la exclusiva de 414. El DWARF del bloque solo tiene `PadIndex`. |
| `Geometry.cpp` barrera selectiva | `SphereVsBox` (zDynamics) | sched2 | sin ella `739f073ea479bef2`. El bloque del bucle en el DWARF no tiene contenido muerto. |
| `EPlayRaceNIS.cpp` barrera | ctor (zMain) | jump1 | sin ella `382fa5d392005b35`, 8 filas; el `.jump` ya trae `bgt`. Ver §5. |
| `InputDeviceGC.cpp` barrera | `PollDevice` (zMain) | sched | sin ella `fc5f8b525f46df68`, 2 filas (= r66). `-fsched-verbose-5` da ICE en `EventSequencer.cpp:135` antes de la función. |

## 5. Propuesta (no aplicada): `EPlayRaceNIS::EPlayRaceNIS`

Mecanismo medido. jump1 pliega `bso L; b ret` en `bgt ret` si no hay etiqueta entre
los dos saltos y el `b` es el siguiente insn **activo** (`jump.c`). En el original
había algo activo en medio al pasar jump1, que desapareció después. Una vez sin
pliegue, loop coloca el bloque `li r19,1; b …` (líneas 132-133) detrás del `b`. El
mapa de líneas deja el hueco: test en la 74, `b` en la 80 y nada en la 75-79.

**Medido**: sustituir la barrera por un store muerto a una local que el DWARF sí
tiene,

```c
                nisType = CAnimChooser::Intro;

                return;
```

da **zMain `cc5a538171d2ac53`, IDÉNTICO, la función al 100 %**: flow lo borra después
de jump1 y de loop. **No se aplica** porque la sentencia concreta de las líneas
75-79 no deja ni bytes ni DWARF: sería invento, y la palanca (c) exige que el DWARF
describa el contenido. Queda para que decida el usuario.

## 6. Reglas nuevas medidas

1. **Un `return` es un pseudo; un `return` por rama escribe directo en r3.**
   `expand_return` calcula el valor en un pseudo y lo copia a r3, pero con if/else la
   resta de la rama va directa a r3 y local-alloc le sugiere r3 al operando que muere.
   Cuando el mapa de líneas pone varias ramas en **una línea**, prueba el ternario.
   Retiró 5 pines y 1 barrera.
2. **Una constante de pool tiene la vida doble para global-alloc; un alias no.**
   `REG_EQUAL` constante → `REG_EQUIV` (`local-alloc.c:881`) → `REG_LIVE_LENGTH *= 2`
   (931). Sustituir un literal por un alias MEM (el truco de `lbl_...`) cambia el
   reparto aunque el código sea el mismo. Y al revés: el literal donde hace falta la
   prioridad, el alias donde GCSE compartiría.
3. **sched1 ordena por LUID las cargas independientes**: el nivel de clase de
   `rank_for_schedule` (sched.c del árbol, que tiene tres niveles: prioridad, clase y
   LUID) no separa dependencias de latencia 1. Así, «qué carga sale primero» y «cuál
   vive más» son la misma decisión de fuente.
4. **El pliegue salto-sobre-salto se decide en jump1**, antes de loop y de flow. Un
   insn que flow borra después basta para impedirlo (medido). Con eso, un
   `__asm__("")` delante de un `return` es sospechoso de tapar contenido muerto.
5. **`-fsched-verbose-5` da ICE** en zMain (`EventSequencer.cpp:135`) y en zSim
   (`NISActivity.cpp:576`). El volcado sirve para las funciones anteriores en la
   SourceList (`OnManageTime` sí salió).
6. **Operandos `"=r"` para GPR**: no se han usado; ninguna de las 18 era un asm
   legítimo con operandos. Regla sin medir.

## 7. Sellos: ALLOC por sección, antes = después

Las seis unidades recompiladas desde el árbol final, a `.o` privado. Digest de unidad
igual a la base en las seis, así que antes y después coinciden sección a sección:

| unidad | digest | secciones |
|---|---|---|
| zBWare | `51df0949941a0339` | `.text 4f4119743db1:38580` `.rodata dff26c83dc7c:2928` `.data cac6dd49bd3d:2896` `.bss nobits:12756` `.ctors 9069ca78e745:4` `.rela.text fe6a4c2b775b:25212` `.rela.rodata 993e0c479bbd:216` `.rela.data d3788ec93b33:72` `.rela.ctors c98c0eae009f:12` |
| zSim | `6913d4cebd1c34a7` | `.text 43193f3b6672:109324` `.rodata ccd9b60ffe97:8384` `.data 4c8f8036143b:348` `.bss nobits:8164` `.ctors 9069ca78e745:4` `.rela.text 02cc67571158:77952` `.rela.rodata 176b0cc3a681:5604` `.rela.data 82e53f430d98:12` `.rela.ctors 768ffafc4592:12` |
| zFeOverlay | `9e4d806d60a97306` | `.over d51074b0a6c1:142528` `.rodata bd90f5af414a:8688` `.data f2da92f1a4e9:944` `.bss nobits:7232` `.ctors 9069ca78e745:4` `.rela.over f17bb016a2b1:135084` `.rela.rodata 59e80879cc06:3492` `.rela.data 756063e2f2e2:336` `.rela.ctors 26a171b62f8d:12` |
| zMain | `cc5a538171d2ac53` | `.text 911d8ef5f6fa:175204` `.rodata 7a5d89ea1345:21320` `.data 123df8ce3e71:3356` `.bss nobits:47800` `.ctors 9069ca78e745:4` `.rela.text e39c05fb1cc2:147420` `.rela.rodata 1ae856d295ab:6504` `.rela.data ee834034952b:6456` `.rela.ctors 3c6b158a23f2:12` |
| zFEng | `3869f721cf92634e` | `.text fbd60c5d2253:73756` `.rodata 1d4a592b6d77:2632` `.data 82101053fe83:384` `.bss nobits:332` `.ctors 9069ca78e745:4` `.rela.text 7dcf5b1adb36:43416` `.rela.rodata 32939450b90c:648` `.rela.data 5a80c32f1024:72` `.rela.ctors d4a140d381cc:12` |
| zDynamics | `fd74052ef8a3c7c5` | `.text aea377736bfa:29268` `.rodata d0620c1a9629:544` `.data 3636c642d1c2:44` `.bss nobits:16` `.ctors 9069ca78e745:4` `.rela.text 74985e389e14:13224` `.rela.rodata 9d80e29ae24d:84` `.rela.ctors e1a643a7ddc5:12` |

`git diff` de los ocho ficheros: seis solo con líneas `+` de comentario; `Strings.cpp`
y `QuickGame.cpp` con exactamente los cambios de §3. Ni cabeceras compartidas, ni
`#if defined(__ANDROID__)`, ni `configure.py`, ni `config/GOWE69/*`. **El DOL no se ha
relinkado** (exige escribir en `build/GOWE69`, compartido): queda para el cierre de
la tanda, con `.o` recompilados y sellados.

## 8. Veredicto

**PARCIAL con cifra: 18 → 11, siete retirados, y los once que quedan con su
diagnóstico r67 y su margen calculado donde es reparto.** Las dos retiradas no salen
de más formas, sino de leer por qué fallaban las anteriores: el mapa de líneas en
Strings y tres volcados en `OnManageTime`. De los once, cuatro son de planificador o
jump (fuera de las palancas de asignación), cuatro de reparto con el margen en insns
escrito, y uno (`EPlayRaceNIS`) tiene la forma medida y queda como propuesta a falta
de evidencia en el DWARF.
