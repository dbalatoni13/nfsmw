# Ronda 32 / pool — **`zDynamics` da `DOL OK`**, y `zMission` también

**Lo primero, que es lo que pedía el encargo: `python scripts/trypromo.py
zDynamics` → `DOL OK`.** Y con él, `zMission`. Las dos juntas también:

```
python scripts/trypromo.py --ldflags "-strip-unused-data -keep <keep+3 lineas>" zDynamics zMission
  zDynamics + zMission                         DOL OK
```

| unidad | `.rodata` antes | después | veredicto | pasa a `linked` |
|---|---|---|---|---|
| `zDynamics` | 840 (objetivo 544, **+296**) | **544, byte a byte** | **DOL OK** | **28.976 B** |
| `zMission` | 120 (objetivo 96, **+24**) | **96, byte a byte** | **DOL OK** | **124 B** |

`.text` 28.976 = 28.976 con las **41 funciones en el orden exacto del objetivo**,
`measure` 100 % en las dos, `audit.py` 0 fallos en las dos.

**No hace falta cambiar los cflags.** El paquete es fuente (7 ficheros) + **3
líneas de `keep.lst`** + los dos `Matching` de `configure.py`. `configure.py`,
`splits.txt`, `symbols.txt`, `config.yml` y `keep.lst` los dejo **byte a byte
como estaban** (md5 comprobado al terminar).

---

## 0. La frase que abre la ronda: **la veda de la r31 estaba CADUCADA**

La r31 midió dos veces que borrar las etiquetas de `zDynamics.cpp` rompe el
enlace, y escribió la veda dentro del propio fichero: *«las referencian los
objetos EXTRAÍDOS DE OTRAS UNIDADES, por nombre»*.

**Contado hoy sobre los 604 objetos extraídos y los 523 nuestros: CERO
referencias externas a las 67 etiquetas de `zDynamics`.** Y en todo `src/` sólo
las nombran `zDynamics.cpp` y los tres `.cpp` de `Dynamics/` (más un comentario
en `zDebug.cpp`). O sea: **el pool escrito a mano se puede borrar entero**, y
ésa es la vía buena — no la del encargo.

Porque la vía del encargo (*que el código use `extern const float lbl_…`*)
**está cerrada por construcción, y también lo he medido**: el pool de GCC 2.9 es
**por función** (`force_const_mem` limpia su tabla al acabar cada una), así que
la MISMA constante tiene una etiqueta distinta en cada función. Las 1e-6 de
`zDynamics` (`lbl_803D3E44`, `E58`, `E68`, `E80`) salen todas de **una sola
línea de `Inertia.h`** inlineada en cuatro funciones, y las 0.0/1.0 de
`UVector.h::Normalize`. **Una cabecera compartida no puede referenciar una
etiqueta distinta por llamante**: el frente del encargo, tal cual, no escala.

Lo que sí funciona es lo simétrico: **dejar que `cc1plus` emita su pool donde ya
lo emite —que es exactamente donde el original lo tiene— y escribir a mano sólo
lo que el compilador no puede generar.**

---

## 1. La herramienta que pedía el encargo: `scripts/lcmap.py`

```
python scripts/lcmap.py zDynamics            # el mapa $LC -> lbl_
python scripts/lcmap.py zDynamics --insn     # + funcion e indice de instruccion
python scripts/lcmap.py zEcstasy --conflict  # solo lo dudoso
```

Empareja comparando **las reubicaciones instrucción a instrucción** contra el
objeto extraído (por función, no por posición de sección: así aguanta que el
`.text` esté reordenado), y **coteja además los bytes del dato apuntado en los
dos lados**. Marca `CONFLICTO` cuando un `$LC` cae contra dos `lbl_` distintos y
`!!BYTES` cuando el valor no coincide.

Las seis unidades, de una pasada:

| unidad | pares | conflictos | bytes distintos |
|---|---|---|---|
| `zDynamics` | 58 | 0 | 0 |
| `zMisc` | 248 | 0 | 0 |
| `zEcstasy` | 551 | **2** | **2** |
| `zMission`, `zMiscSmall`, `zRender` | 1 | 0 | 0 |

Y confirma el aviso del encargo: en `zDynamics` **el mapa NO es posicional**.
Cuadra en las primeras 24 y se desalinea porque siete etiquetas
(`EA0`, `EA4`, `EC0`, `ED4`, `ED8`, `EDC`, `EE0`) no tienen `$LC` —el código ya
las referenciaba— y porque el objetivo lleva 8 B muertos dentro de
`lbl_803D3E90`. **En `zEcstasy` hay dos filas que un mapa posicional daría por
buenas y son falsas** (`$LC845` contra `lbl_803DED04`/`lbl_803DED00`).

---

## 2. La receta de `zDynamics`, en el orden en que hay que aplicarla

El objetivo tiene la `.rodata` así, y **cada tramo lo pone un mecanismo
distinto**:

```
  0   lbl_803D3DC8   92 B  cadenas bWare/STL   -> asm, ANTES de los #include
 92   pool FP       300 B  por funcion         -> lo emite cc1plus
392   algos.3467     36 B  estatico local      -> asm en Geometry.cpp
428   F74/F78        12 B  pool de FindIntersection + 4 de relleno
440   vtables        72 B  finish_file
512   cola           32 B  $LC del static-init + 28 B -> ESTATICO DE PLANTILLA
```

### 2.1 El `.text`: tres virtuales y un intercambio (el frente §4 del brief)

`objdiff` no lo ve, pero 23 de las 41 funciones estaban desplazadas. Dos causas,
las dos de fuente:

- **los tres `virtual void OnDebugDraw() {}` con el cuerpo DENTRO de la clase**
  (`Articulation.h`) los emite GCC en `finish_file`, al final del TU; el
  original los tiene **en su sitio**. Declaración en la clase + definición fuera
  de línea en `Articulation.cpp`, en el hueco exacto que les da el objetivo;
- **`Joint::AddConstraint` iba DELANTE de `Lever::SetFulcrum`** y en el original
  va detrás.

Con eso: **41 de 41 funciones en el offset exacto**.

**Efecto colateral medido, y la trampa**: al dejar de ser inline, `OnDebugDraw`
pasa a ser la *key function* de las tres clases, `CLASSTYPE_INTERFACE_KNOWN` se
enciende y —con `-fimplement-inlines`, que es el defecto— GCC **emite fuera de
línea los 9 miembros inline restantes** (`GetArm`, `GetEntity`×2, `IsImmobile`,
`Lever::Lever`, `operator new`/`delete` de `Joint` y `Constraint`): `.text`
29.268 en vez de 28.976.

- Con **`-fno-implement-inlines`**: `.text` **28.976 clavados**, 41 funciones,
  orden exacto, 100 %.
- **Pero la bandera NO hace falta**: los 9 sobrantes no los referencia nadie y
  el enlazador se los lleva. **`trypromo` da `DOL OK` con el objeto que produce
  el árbol tal cual.** Lo dejo sin bandera para no tocar `configure.py`.
  (La bandera sigue siendo la respuesta *limpia* si alguien quiere el objeto
  idéntico; ya hay precedente en `configure.py`: `csis.cpp` y `path`.)

### 2.2 El pool: borrarlo a mano y devolvérselo al compilador

- `zDynamics.cpp`: del `asm()` sólo sobrevive **`lbl_803D3DC8`**, y **sube antes
  de los `#include`** (tiene que caer en el desplazamiento 0).
- Las 19 + 25 + 18 declaraciones `extern const float lbl_…` de
  `Articulation/Collision/Geometry.cpp` se borran, y sus 11 usos vuelven a ser
  literales con **`scripts/pool2lit.py --apply`**. **`.text` byte a byte
  idéntico** (0 diferencias): el aviso del docstring de `pool2lit` sobre el
  reparto de registros **no se cumple aquí**.
- `algos.3467` se muda de `zDynamics.cpp` a **`Geometry.cpp`, justo delante de
  `FindIntersection`**: el `asm()` de ámbito de fichero se emite al parsearlo,
  o sea **entre el pool de la función anterior y el de la siguiente**. Ése es el
  mando fino de colocación que hacía falta, y sirve para cualquier unidad.
- 8 B muertos (`0x803D3E98`) con un `asm()` detrás de
  `Moment::React(const Joint&, int)`, y 4 B de relleno (`0x803D3F7C`) detrás de
  `FindIntersection`.

### 2.3 La cola de 32 B: el estático de plantilla

Los 32 B que van **DETRÁS de las vtables** eran el muro. Un `asm()` de ámbito de
fichero se emite al **parsearlo**, siempre delante de todo lo de `finish_file`;
`.subsection` no existe en `ngcas` (veda r31) y `.rodata.*` va detrás de *todos*
los `.rodata` del enlace.

**Lo que sí llega ahí** (medido, tres formas):

| forma | dónde cae |
|---|---|
| `asm()` de ámbito de fichero | siempre **delante** de las vtables |
| instanciación **explícita** `template struct X<0>;` | delante de las vtables |
| estático de plantilla **instanciado por un puntero `static` sin usar** | **detrás del pool de `__static_initialization_and_destruction_0`** |

O sea:

```cpp
template <int N> struct zDynTailA { static const unsigned int d[3]; };
template <int N> const unsigned int zDynTailA<N>::d[3] = { 0x00000000u, 0x40800000u, 0x3F000000u };
static const unsigned int *const zDynTailRefA = zDynTailA<0>::d;   // no se emite
```

El puntero `static` **no cuesta ni un byte** (no se usa, GCC no lo emite) y el
array cae exactamente detrás del `$LC` del inicializador estático — que es el
1.0f de `0x803D3FC8`. `.text`, `.data`, `.bss` sin tocar.

### 2.4 El `"bad_alloc"` que emite `cc1plus` — y por qué NO va en `keep.lst`

Las 92 B de cadenas del original son **seis literales del compilador** en orden
de primer uso: `"GAMECUBE"`, el `__FILE__` de EA, `"bad_alloc"`, `"%f,%f,%f"`,
`"%f,%f,%f,%f"`, `"STL"`. **De los seis, nuestro árbol genera uno: el
`"bad_alloc"` de STL** (`$LC162`, 10 B, sin referenciar), y lo emite **el
primero de todo**.

Escribir el bloque entero a mano deja el `$LC` de más: +12 B, y encima descuadra
la alineación a 8 del primer doble del pool (+4 más). Ésa es la veda `zMission`
de la r31 (*«el enlazador tira el `$LC60` de en medio»*).

**La cura**: partir el bloque en **52 B antes de los `#include`** y **28 B al
principio del primer `.cpp` incluido**, dejándole el hueco al literal del
compilador. Y —esto es lo que evita nombrar un `$LC` en `keep.lst`— **el `.size`
de 0x5C del símbolo escrito a mano cubre las tres partes**: `-strip-unused-data`
respeta los bytes de dentro de un símbolo que se conserva. Medido: con
`zDynamics.o:$LC162` en `keep.lst` y sin él, **el DOL sale igual de bueno**.

---

## 3. `zMission`: la misma receta, más el orden del `.bss`

`.rodata` byte a byte con el mismo corte 52 + 28 (bloque B al principio de
`MissionEdit.cpp`) y **borrando el `lbl_803F68EC` escrito a mano**: el 1.0f lo
pone el pool del inicializador estático, que es lo último del TU.

Con eso quedaban **2 bytes** de diferencia en todo el DOL, y eran del `.bss`:
nuestro `gap_07_804801B4_bss` (36 B, escrito en un `asm()`) caía **delante** de
`kFloatScaleUp`/`kFloatScaleDown`, y en el original va detrás — `stfs f13,
0x1d0(r9)` contra `0x1ac(r9)`, exactamente 0x24.

- un estático de plantilla **sin inicializar** se va a **COMMON** (`SHN_COMMON`)
  y lo coloca el enlazador: no vale;
- con `__attribute__((section(".bss")))` sí entra en `.bss`… **pero delante**,
  porque `kFloatScaleUp`/`Down` salen por `.lcomm` y el ensamblador los coloca
  al final;
- **lo que funciona es un `static` normal, diferido, declarado después**, con
  `__asm__("gap_07_804801B4_bss")` para conservar el nombre de `keep.lst`, y un
  puntero `static const` sin usar que lo marque como usado:

```cpp
static unsigned char zMisTailBss[0x24] __asm__("gap_07_804801B4_bss");
static const unsigned char *const zMisTailRef = zMisTailBss;
```

`.bss` queda `[kFloatScaleUp][kFloatScaleDown][gap_07_804801B4_bss]` = 44 B,
igual que el extraído. **`DOL OK`.**

(El `__asm__("…")` de renombrado **no funciona sobre un miembro estático de
plantilla** —el símbolo sale manglado igual— pero sí sobre un `static` normal.
Por eso `zDynamics` necesita las dos entradas mangladas de `keep.lst` y
`zMission` no.)

---

## 4. El paquete: lo que hay que aplicar

**Fuente (ya está puesta en el árbol, 7 ficheros):**

- `src/Speed/Indep/SourceLists/zDynamics.cpp` (−380 líneas)
- `src/Speed/Indep/SourceLists/zMission.cpp`
- `src/Speed/Indep/Src/Misc/MissionEdit.cpp`
- `src/Speed/Indep/Src/Physics/Dynamics/Articulation.h` / `Articulation.cpp`
- `src/Speed/Indep/Src/Physics/Dynamics/Collision.cpp`
- `src/Speed/Indep/Src/Physics/Dynamics/Geometry.cpp`

**`config/GOWE69/keep.lst` — 3 líneas (PROPUESTA, no aplicada):**

```
zDynamics.o:_t9zDynTailA1i0.d
zDynamics.o:_t9zDynTailB1i0.d
zMission.o:pad_05_803F6890_rodata
```

Las dos primeras nombran símbolos que sólo define **nuestro** objeto, así que
`keepchk.py` las dará por `RANCIAS` (valida contra `build/GOWE69/obj`): es un
falso positivo conocido, igual que el `zDebug.o:lbl_803D3DC4` de la r31. Sin
tocar `keep.lst`, `keepchk` sigue en **758 ok / 19 RANCIAS**, como antes.
La entrada vieja `zDynamics.o:lbl_803D3FD8` queda inerte (nuestro objeto ya no
define ese símbolo) y **se puede dejar**.

**`configure.py` — dos líneas (PROPUESTA, no aplicada):**

```diff
-            Object(NonMatching, "Speed/Indep/SourceLists/zDynamics.cpp"),   # linea 648
+            Object(Matching,    "Speed/Indep/SourceLists/zDynamics.cpp"),
-            Object(NonMatching, "Speed/Indep/SourceLists/zMission.cpp"),    # linea 679
+            Object(Matching,    "Speed/Indep/SourceLists/zMission.cpp"),
```

**Verificación (reproducible):**

```
python scripts/build_direct.py zDynamics zMission          2 ok, 0 fallidas
python scripts/trypromo.py --ldflags "-strip-unused-data -keep <keep+3>" zDynamics zMission
                                                           DOL OK
python scripts/measure.py SourceLists/zDynamics            28976/28976 B  100.0000%  41 fns
python scripts/measure.py SourceLists/zMission             124/124 B      100.0000%   2 fns
python scripts/audit.py  Speed/Indep/SourceLists/zDynamics 0 FALLA
python scripts/audit.py  Speed/Indep/SourceLists/zMission  0 FALLA
```

**Cabecera compartida**: `Articulation.h` la incluyen `RigidBody.cpp`
(zPhysicsBehaviors) y `Bounds.cpp`/`Smackable.cpp` (zPhysics). A/B por objetos,
antes y después, **byte a byte idénticos** en las dos unidades
(`.text`/`.rodata`/`.data`/`.bss`, 0 bytes distintos, 921 y 1.304 funciones).
Además, ninguna otra unidad define ni referencia los tres `_vt.` ni los
`OnDebugDraw`.

---

## 5. Deuda declarada (regla 3 del brief)

**Un constructo que el original no tenía**, en `Geometry.cpp::BoxVsBox`:

```cpp
float best_interval = -100000.0f;
...
best_interval = 0.0f;          // <-- almacenamiento muerto ANADIDO
best_interval = -100000.0f;
```

El objetivo fuerza a memoria `-100000` **antes** que `0.0` (`lbl_803D3F1C`
delante de `lbl_803D3F20`) y nosotros al revés. Barrido de cinco formas:

| | forma | `.text` vs la base al 100 % | orden del pool |
|---|---|---|---|
| c1 | el árbol tal cual (`= 0.0f` primero) | **0 bytes distintos** | MAL |
| c2 | `= -100000.0f` en la declaración, conservando la asignación | 12 distintos | OK |
| c3 | `= -100000.0f` y fuera la asignación | 12 distintos | OK |
| **c4** | `= -100000.0f` + `= 0.0f` muerto + `= -100000.0f` | **0 distintos** | **OK** |
| c5 | `= -100000.0f` + `best_interval += 0.0f;` | 12 distintos | OK |

c4 es la única que da las dos cosas. Los 12 B que mueven c2/c3/c5 dicen que
**nuestro `BoxVsBox` difiere del original en algo más**, y quien lo encuentre
podrá quitar el almacenamiento muerto.

---

## 6. Vedas y trampas nuevas (todas medidas)

1. **`.text` al final de un `asm()` DESINCRONIZA a GCC.** GCC lleva su propio
   `in_section`; si el `asm` cambia de sección por debajo, la siguiente
   constante de pool se emite **sin** `.section .rodata` y cae en `.text`.
   Síntoma: `.text` +4 B y **falta la primera entrada del pool**. Se ve en el
   `.s`: `$LC169: .long 0x0` justo detrás de un `.text` de un `asm`.
   **Usa `.previous`, no `.text`.** (Los `asm()` viejos del árbol funcionan
   porque detrás de ellos siempre viene una función, y las funciones sí
   redeclaran `.section ".text"`.)
2. **La instanciación explícita de plantilla emite PRONTO**; sólo la disparada
   por un uso en un inicializador de ámbito de fichero emite **tarde**. Con
   `template struct X<0>;` el dato cae delante de las vtables y no sirve.
3. **`__asm__("nombre")` no renombra un miembro estático de plantilla.**
   Sobre un `static` normal sí.
4. **Un estático de plantilla sin inicializar es COMMON**, no `.bss`.
5. **`-strip-unused-data` respeta lo que cae dentro del `.size` de un símbolo
   conservado.** Es lo que salva al `$LC` del compilador metido entre las dos
   mitades del bloque de cadenas.
6. **`pool2lit.py` no cambia el reparto de registros** en `zDynamics`: 11 usos
   convertidos, `.text` byte a byte igual. Su docstring dice lo contrario;
   sigue siendo cierto que hay que medir fichero a fichero.
7. **La veda de la r31 sobre borrar las etiquetas (§3 del brief) está
   caducada** — con medida: 0 referencias externas en 1.127 objetos.
8. **La vía del encargo (`extern const float lbl_…` en el código) no escala** a
   las constantes que vienen de cabeceras compartidas: el pool de GCC 2.9 es
   por función y una cabecera no puede referenciar una etiqueta distinta por
   llamante. Sólo vale para las constantes escritas en el `.cpp` de la unidad.

---

## 7. Las otras cuatro: diagnóstico, no barrido

Las cuatro fallan por **lo mismo** que fallaban `zDynamics` y `zMission` —el
literal del compilador delante del bloque escrito a mano— y la receta de §2.4
debería aplicarse igual. Lo que ya está contado:

| unidad | `.rodata` nuestro/obj | primer desajuste | `.data` | `.bss` | `.text` de más |
|---|---|---|---|---|---|
| `zMiscSmall` | 1.000 / 352 | `$LC60` (72 B: `"bad_alloc"` **y** `"GAMECUBE"`) delante de `lbl_803F6730` | 684 / 620 | 8 = 8 | 3.996 |
| `zRender` | 2.240 / 1.032 | `$LC62` (`"bad_alloc"`) delante de `pad_05_80403B40_rodata` | 172 = 172 | 232 / 8 | 4.028 |
| `zMisc` | 12.632 / 7.864 | `$LC57` delante de `lbl_803F4878` | 2.184 / 2.164 | 2.560 / 2.272 | 8.360 |
| `zEcstasy` | 13.456 / 7.824 | `$LC58` delante de `lbl_803DD6DC` | 4.240 / 4.136 | 82.064 / 81.200 | 8.956 |

Avisos para quien las coja:

- **`zMiscSmall` necesita un corte en TRES**, no en dos: su `$LC60` de 72 B trae
  `"bad_alloc"` y `"GAMECUBE"` juntos.
- **`zRender` y `zMisc`/`zEcstasy` traen además `.bss`/`.data` de más**; el
  `.bss` es el problema del §3 (orden contra los `.lcomm`), y ahí la palanca es
  el `static` diferido, no el `asm()`.
- **`zEcstasy` tiene 2 filas con `CONFLICTO` en `lcmap`** (`$LC845` contra
  `lbl_803DED04` y `lbl_803DED00`) y 2 `$LC` sin pareja en su inicializador
  estático: ahí un mapa posicional sería el error silencioso del que avisa el
  encargo.
- El `.text` de más de las cuatro es código muerto que el enlazador tira (lo
  mismo que los 9 inlines de `zDynamics`): **no es trabajo**, salvo que
  desplace el orden de lo vivo.

## 8. Estado del árbol

- **Ficheros dejados modificados: los 7 de §4.** Ninguno rompe el enlace: las
  dos unidades siguen siendo `NonMatching`, y cuando se promocionen dan
  `DOL OK`. Copias de los originales en
  `…/scratchpad/c32pool_backup/*.orig`.
- **`configure.py` y `config/GOWE69/*`: intactos** (md5 idénticos al arrancar y
  al terminar). `keep.lst` medido siempre sobre una copia del scratchpad, con
  `trypromo --ldflags`.
- **Herramienta nueva en el árbol: `scripts/lcmap.py`.** Guiones de trabajo en
  el scratchpad con prefijo `c32pool_` (`_cc.py` compila una unidad con sus
  cflags a donde yo diga; `_rod.py` compara una sección de datos contra la
  extraída; `_dol.py` enlaza y dice **dónde** se rompe el DOL; `_drift.py` dice
  dónde empieza a desplazarse el enlace contra el ELF original).
- **Ni una instrucción de ensamblador escrita.** Todo lo emitido a mano son
  datos (`.byte`, `.4byte`, `.skip`).
