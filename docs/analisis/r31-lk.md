# Ronda 31 — enlace: **zDebug promociona (DOL OK)** y el pool de bWare, que es la causa de las once

Encargo: las once SourceLists con el `.text` al 100 % y cero funciones pendientes
que no promocionan (446.992 B de `linked`).

Resultado: **`zDebug` promociona con `trypromo.py` → `DOL OK`, y el paquete es un
cambio de FUENTE de 40 líneas: cero cambios en `splits.txt`, `symbols.txt`,
`keep.lst` y `config.yml`.** Además, **74 entradas nuevas de `keep.lst`
verificadas (`DOL OK` sin promocionar nada)** que desbloquean el dato muerto de
`zRender`, `zMission`, `zMiscSmall` y `zDynamics`, y el diagnóstico medido de por
qué las otras diez siguen sin cerrar.

`config/GOWE69/*` y `configure.py` quedan **byte a byte como estaban** (md5
comprobado al terminar). El único fichero del árbol que dejo modificado es
`src/Speed/Indep/SourceLists/zDebug.cpp` (§2).

---

## 0. Las cinco frases

1. **No hay 30 kB de datos que escribir: hay datos escritos DOS veces.** El
   `asm()` de la SourceList escribe a mano un literal que el compilador emite
   *también* por su cuenta. Las dos copias sobreviven al enlace y ese exceso
   **es exactamente el delta positivo del DOL**. En `zDebug` eran 12 B (un
   `.float 1` y un `.4byte 0`) y quitarlos da **DOL byte a byte**.
   `zOnline.cpp` ya trae ese hallazgo escrito en un comentario: es la plantilla.
2. **Y el simétrico: al promocionar, el dato MUERTO escrito a mano se lo lleva
   `-strip-unused-data`.** Sobrevive en el objeto extraído pero no en el nuestro.
   Se arregla con una entrada `<unidad>.o:<símbolo>` en `keep.lst`, y eso solo
   sube `zRender` de **−704 a +320** y `zMission` de **−64 a +32**.
3. **La `.rodata` de 31 de las 33 SourceLists empieza por el MISMO pool de
   bWare/STL** (`"GAMECUBE"`, `"d:/mw/speed/indep/bware/inc/bware.hpp"`,
   `"bad_alloc"`, `"%f,%f,%f"`, `"%f,%f,%f,%f"`, `"STL"`, 92 B). **Sólo UNA de
   las nuestras lo reproduce (`zOnline`), y sólo 7 contienen siquiera la ruta.**
   Por eso el prefijo común de la `.rodata` de zAttribSys, zAnim, zFEng, zLua y
   zMain contra la original es **cero bytes**. La ruta es el `__FILE__` de la
   máquina de EA: **no se puede generar, hay que escribirla**.
4. **`keep.lst` nombra símbolos que nuestros objetos NO DEFINEN** en las cinco
   unidades gordas: `zAttribSys` 47 (796 B `.rodata` + 48 `.data` + 4 `.bss`),
   `zAnim` 9 (85 + 176 + 16), `zFEng` 6 (160 + 84), `zLua` 5 (97 + 304),
   `zMain` 6 (92 + 56). Los `.data` que faltan **coinciden con el delta `.data`
   del DOL** (zLua −288 contra 304 B, zAnim −192 contra 176, zFEng −64 contra
   84, zMain −96 contra 56). Mientras esos símbolos no existan en nuestro objeto,
   esas cinco no pueden promocionar por mucho que el `.text` esté al 100 %.
5. **`dtk` sólo fabrica un comodín `auto_*` de `.rodata` en frontera de 8.** Un
   corte en dirección múltiplo de 4 no da error: **los bytes desaparecen y el DOL
   sale corto**. Medido: corte en `0x803D3DC4` → sin comodín, **DOL −320 B**;
   corte en `0x803D3DC0` → nace `auto_05_803D3DC0_rodata` y **DOL OK**.

---

## 1. El mecanismo, con el enlazador delante

`zDebug` es el caso mínimo: 124 B de `.text` (sólo el par de inicialización
estática) y `.rodata` 0x803D3C78..0x803D3DC8 (336 B).

| | nuestro `.o` | extraído |
|---|---|---|
| `.text` | 860 B | 124 B |
| `.rodata` | 352 B | 336 B |
| `.bss` | 16 B | 8 B |

El `.text` de más (736 B, la clase `DebugGraph`, que **no existe en ninguna parte
del original**) lo tira el enlazador y no molesta. Lo que rompe el DOL son 12 B
de datos, en dos sitios distintos y por dos razones distintas:

### 1.1 El `.float 1` escrito a mano contra el `$LC` del compilador

El `__static_initialization_and_destruction_0` del original hace

```
/* 80083624 */  lis  r9, lbl_803D3DC0@ha
/* 80083630 */  lfs  f0, lbl_803D3DC0@l(r9)
```

y el nuestro, **la misma instrucción con otro nombre**:

```
/* 00000300 */  lis  r9, $LC61@ha
/* 0000030C */  lfs  f0, $LC61@l(r9)
```

`objdiff` lee 100 % y `audit.py` pasa (compara el VALOR del literal, y 1.0f es
1.0f). Pero `zDebug.cpp` **escribía a mano** `lbl_803D3DC0: .float 1`, así que el
objeto llevaba **dos** copias del 1.0f: la del `asm` en el sitio bueno y la del
compilador detrás. Resultado: `.rodata` 352 en vez de 336 y el DOL **+32 B**
(los 8 de más se redondean a 32 por el `align:32` de `.data`).

`lbl_803D3DC0` **es** el `$LC` del compilador original: nadie fuera de zDebug lo
referencia (`c31lk_who.py`: definido en `zDebug.o`, 0 referencias externas).

### 1.2 `lbl_803D3DC4` no es un dato: es el relleno de alineación

Los 4 B de 0x803D3DC4 son ceros que nadie referencia (por eso `keep.lst` tiene
que forzarlos). Son el relleno que lleva el final real de la `.rodata` de zDebug
(0x803D3DC4) hasta el `align 8` con el que empieza zDynamics (0x803D3DC8). El
ensamblador los pone solo con el `.balign 8` que ya tiene el bloque: **escribirlos
a mano sólo servía para duplicarlos**.

### 1.3 El `.bss` que era del vecino

`zDebug.cpp` escribía a mano `kFloatScaleUp_8045B100` y `kFloatScaleDown_8045B104`
"pooled .bss float scale copies owned by this unit". **No son de esta unidad**:
0x8045B100 es el arranque del `.bss` de **zDynamics** (zDebug es
0x8045B0F8..0x8045B100), y `zDynamics.cpp` ya los define con esos nombres vía
`#define`. Nuestro `zDebug.o` emitía 16 B de `.bss` (los 8 del vecino + los 8
suyos, que el compilador emite solo) contra los 8 del original, y esos 8 B de más
corrían la `.sdata` **32 B**.

---

## 2. El paquete de `zDebug` — **`trypromo.py zDebug` → `DOL OK`**

### 2.1 `src/Speed/Indep/SourceLists/zDebug.cpp` — 40 líneas BORRADAS, ninguna añadida

```diff
@@ -9,26 +9,6 @@
 #include "Speed/Indep/Src/Debug/Common/DebugGraph.cpp"

-// Pooled .bss float scale copies owned by this unit in the shipped binary
-// (the DOL splitter named them after their final addresses).
-asm(
-    ".globl kFloatScaleUp_8045B100\n"
-    ".globl kFloatScaleDown_8045B104\n"
-    ".section .bss\n"
-    ".balign 4\n"
-    "kFloatScaleUp_8045B100:\n"
-    "  .skip 4\n"
-    "kFloatScaleDown_8045B104:\n"
-    "  .skip 4\n"
-);
-
 // .rodata pool owned by this unit in the shipped binary, emitted verbatim
@@ -61,24 +41,4 @@ asm(
     ".size lbl_803D3CE8, 0xD8\n"
-    ".globl lbl_803D3DC0\n"
-    ".type lbl_803D3DC0, @object\n"
-    "lbl_803D3DC0:\n"
-    "  .float 1\n"
-    ".size lbl_803D3DC0, 0x4\n"
-    ".globl lbl_803D3DC4\n"
-    ".type lbl_803D3DC4, @object\n"
-    "lbl_803D3DC4:\n"
-    "  .4byte 0x00000000\n"
-    ".size lbl_803D3DC4, 0x4\n"
 );
```

(El fichero real lleva los saltos de línea DENTRO de la cadena y **finales de
línea MEZCLADOS**: la cabecera es LF y el bloque `asm` CRLF. Anclar con `\r?\n`.)

### 2.2 `configure.py` — una línea

```diff
-            Object(NonMatching, "Speed/Indep/SourceLists/zDebug.cpp"),
+            Object(Matching, "Speed/Indep/SourceLists/zDebug.cpp"),
```

### 2.3 `splits.txt`, `symbols.txt`, `keep.lst`, `config.yml`: **NADA**

La entrada `zDebug.o:lbl_803D3DC4` de `keep.lst` **se puede dejar tal cual**: con
la unidad promocionada nuestro objeto no define ese símbolo, la entrada no casa
con nada y el enlazador la ignora — los 4 B siguen ahí porque son el relleno del
`.balign 8` (§1.2). `keepchk.py` la sigue dando por buena porque valida contra
`build/GOWE69/obj`, donde el objeto extraído sí la tiene.

### 2.4 Verificación

```
python scripts/build_direct.py zDebug        1 ok, 0 fallidas
python scripts/trypromo.py zDebug            zDebug   DOL OK
python scripts/keepchk.py                    758 ok, 19 RANCIAS, 0 sin objeto   (igual que antes)
python scripts/measure.py SourceLists/zDebug 124/124 B  100.0000%  2 funciones al 100%
python scripts/audit.py Speed/Indep/SourceLists/zDebug   0 FALLA (2 funciones)
```

Y el `.text` del objeto es **byte a byte el mismo** que antes del cambio
(`.rodata` 352 → 344, `.bss` 16 → 8, `.text` 860 = 860 IGUAL): el cambio no toca
una sola instrucción, así que no puede mover `matched`.

Comprobado además que **ningún objeto del árbol** (523 nuestros + 604 extraídos)
referencia `kFloatScaleUp_8045B100`, `kFloatScaleDown_8045B104`, `lbl_803D3DC0`
ni `lbl_803D3DC4` como indefinido: borrarlos no deja un UND en ninguna parte.

---

## 3. Las 74 entradas de `keep.lst` — **verificadas `DOL OK` sin promocionar nada**

Cuando la unidad se promociona, el dato MUERTO que su `.cpp` escribe a mano
(`pad_*`, `gap_*`, `lbl_*` que nadie referencia) **se lo lleva
`-strip-unused-data`**, cosa que no pasa con el objeto extraído. El DOL sale
corto y el enlace no falla: es la trampa del brief, vista desde el otro lado.

`c31lk_mkkeep.py` genera las entradas que faltan (símbolo que nuestro objeto
define, con el mismo nombre que el extraído, y que aún no está en `keep.lst`):

| unidad | entradas nuevas | B forzados |
|---|---|---|
| `zRender` | 3 | 1.032 (`pad_05_80403B40_rodata`, `lbl_80403F40`, `gap_05_80403F44_rodata`) |
| `zMiscSmall` | 5 | 227 |
| `zDynamics` | 65 | 328 |
| `zMission` | 1 | 92 (`pad_05_803F6890_rodata`) |

Las 74 juntas, **sin promocionar ninguna unidad: `DOL OK`** (banco `c31lk_go.py
--tag K1`). Son seguras de aplicar por sí solas.

Efecto sobre el delta del DOL de cada promoción:

| unidad | hoy | con las 74 entradas |
|---|---|---|
| `zRender` | −704 | **+320** |
| `zMission` | −64 | **+32** |
| `zMiscSmall` | +192 | +192 |
| `zDynamics` | +192 | **+288** (empeora, ver abajo) |

**Aviso**: en `zDynamics` las entradas EMPEORAN mientras el objeto siga llevando
los `$LC` duplicados — se fuerza el dato muerto escrito a mano *y* sigue la copia
del compilador. Las de `zDynamics` sólo deben aplicarse junto con la limpieza de
duplicados de §4. Las de `zRender`, `zMission` y `zMiscSmall` son ganancia neta.

---

## 4. Lo que queda en las cuatro de delta POSITIVO: literales duplicados

Con `keep.lst` arreglado, las cuatro fallan por lo mismo y sólo por lo mismo:
**nuestro objeto emite `$LC` vivos que el original no tiene aparte**, porque el
`asm()` ya escribe esos bytes.

| unidad | sobra | qué es |
|---|---|---|
| `zMission` | 16 B | `$LC60` = `"bad_alloc"`, que ya está dentro de `pad_05_803F6890_rodata` |
| `zRender` | 320 B | 288 B de `$LC` delante del pad + 4 detrás |
| `zMiscSmall` | 192 B | idem |
| `zDynamics` | 288 B | 59 `$LC` (`$LC162`…`$LC235`) delante del bloque `asm` |

La cura es la de §1.1 (borrar del `asm` lo que el compilador ya emite), pero aquí
choca con el **orden de emisión**, que está medido y es rígido:

> GCC 2.9 emite, por este orden: (1) las constantes de cada función según se
> compila —o sea, durante los `#include`—, (2) el `asm()` de ámbito de fichero
> donde esté escrito, (3) el pool de la función de inicialización estática, que
> es SIEMPRE lo último del TU. **Nada escrito a mano puede quedar detrás del pool
> del `_GLOBAL_.I.`.**

Eso es exactamente lo que hace funcionar a `zDebug` y `zOnline` (el 1.0f de
cierre lo pone el compilador) y lo que bloquea a las otras: sus `$LC` vivos caen
**delante** del bloque escrito a mano, y en el original van **dentro**.

**Ensayo numerado sobre `zMission`** (124 B), con las cuatro formas medidas:

| | forma | `.rodata` del objeto | DOL |
|---|---|---|---|
| c1 | árbol tal cual | 120 B | −64 |
| c2 | quitado `lbl_803F68EC: .float 1` | 112 B | −64 |
| c3 | c2 + `keep.lst` | 112 B | **+32** |
| c4 | pool partido en dos `asm`, uno ANTES del `#include` y otro después, dejando el hueco de `"bad_alloc"` para el compilador | **96 B, byte a byte idéntica a la extraída** | +0 de tamaño, **contenido mal**: el enlazador tira el `$LC60` de en medio y todo lo que va detrás se corre 12 B |
| c5 | pool entero ANTES del `#include` | 112 B | +32 |

**Veda `zMission`**: barrida la colocación del bloque `asm` (antes/después del
`#include`, partido en dos, con y sin `.size`, con y sin `.type @object`) y la
entrada de `keep.lst`. Se llega a tener la `.rodata` del objeto **idéntica byte a
byte** (c4) y aun así el enlazador decide tirar el `$LC` de en medio. Lo que
queda es que el compilador **no emita** su `"bad_alloc"` — o sea, fuente de STL,
no de reparto.

---

## 5. Las cinco gordas: `keep.lst` nombra símbolos que no existen en nuestro objeto

Ésta es la razón de fondo de los deltas negativos, y se mide en un grep:

| unidad | entradas de `keep.lst` | de ellas, ausentes en nuestro `.o` | B que fuerzan |
|---|---|---|---|
| `zAttribSys` | 47 | **47** | 796 `.rodata` + 48 `.data` + 4 `.bss` |
| `zAnim` | 9 | **9** | 85 + 176 + 16 |
| `zFEng` | 6 | **6** | 160 + 84 |
| `zLua` | 5 | **5** | 97 + 304 |
| `zMain` | 6 | **6** | 92 + 56 |
| (`zDebug`, `zMission`, `zRender`, `zMiscSmall`, `zDynamics`) | 27 | **0** | — |

Y el `.data` que fuerzan **es** el delta `.data` del DOL: zLua 304 B contra −288,
zAnim 176 contra −192, zFEng 84 contra −64, zMain 56 contra −96, zAttribSys 48
contra −32. O sea: **el agujero de `.data` de las cinco gordas es, entero, el
dato muerto que `keep.lst` fuerza y que nuestro objeto no escribe.**

Los nombres dicen de qué son: en `zAnim`/`zFEng`/`zLua`/`zMain` son `pad_*`,
`gap_*` y `lbl_*` (bloques anónimos); en `zAttribSys` son **`$LC2151483776`,
`$LC2151483788`…** — el nombre `$LC` del compilador ORIGINAL, sacado del DWARF y
desambiguado por `dtk` con la dirección en decimal (2151483776 = 0x803D0980).
Están en `symbols.txt`. Son literalmente el pool de literales del TU original.

**El trabajo que falta en las cinco es escribir esos bloques como `asm()` en la
SourceList** (dato con `.long`, que el brief permite), igual que ya lo hacen
`zDebug`, `zMission`, `zDynamics`, `zMiscSmall`, `zRender` y `zOnline`. No lo he
hecho: son 1.230 B de `.rodata` y 668 de `.data` repartidos en 73 símbolos, y
antes hay que resolver el §6, que decide dónde van.

---

## 6. El hallazgo que ordena las 33: el pool de cabecera de bWare

Medido sobre las 33 SourceLists, objeto extraído contra el nuestro:

- **31 de 33 objetos extraídos empiezan su `.rodata` con este bloque de 92 B**:

  ```
  "GAMECUBE\0\0\0\0"
  "d:/mw/speed/indep/bware/inc/bware.hpp\0\0\0"
  "bad_alloc\0\0\0"
  "%f,%f,%f\0\0\0\0"
  "%f,%f,%f,%f\0"
  "STL\0"
  ```
  (las dos excepciones son `zEcstasy` y `zGameModes`.)

- **De los nuestros lo reproduce UNO: `zOnline`** — la única SourceList ya
  promocionada. Y **sólo 7 contienen siquiera la ruta** (`zDebug`, `zDynamics`,
  `zMisc`, `zMiscSmall`, `zMission`, `zOnline`, `zRender`): exactamente los que
  la escriben a mano en un `asm()`.

- En las otras 24, incluidas las cinco gordas, el prefijo común de la `.rodata`
  contra la original es **0 bytes**:

  | unidad | nuestro empieza por | el original empieza por |
  |---|---|---|
  | `zAttribSys`, `zLua`, `zMain` | `"bad_alloc"`, luego `"GAMECUBE"`, luego `"Attrib::…"` | `"GAMECUBE"`, `"d:/mw/…/bware.hpp"`, `"bad_alloc"` |
  | `zAnim` | `\0\0\0\0"GAMECUBE"`, `"EAGL4::SymbolEntry"` | idem |
  | `zFEng` | flotantes | idem |

De dónde sale cada cadena en el árbol: `"GAMECUBE"` de
`bGetPlatformName()` (inline en `src/Speed/Indep/bWare/Inc/bWare.hpp:166`),
`"bad_alloc"` de `__THROW_BAD_ALLOC` de STL, `"STL"` de
`stlgc/stl/_alloc.h:236`. **La ruta `d:/mw/speed/indep/bware/inc/bware.hpp` es el
`__FILE__` de bWare.hpp EN LA MÁQUINA DE EA**: nuestro árbol no la puede
generar, hay que escribirla — y no está en ninguno de los 24.

Y el orden importa: el original emite primero `bGetPlatformName`, después lo que
usa `__FILE__`, y sólo entonces el `bad_alloc` de STL; nosotros emitimos el
`bad_alloc` primero. Eso es **orden de emisión de inlines fuera de línea**, que
en GCC 2.9 sigue el orden de primer uso.

**Esto es el frente**: mientras la cabecera de la `.rodata` no case, ninguna de
las 24 puede promocionar aunque el `.text` esté al 100 %, porque el desfase
empieza en el byte 0 del rango.

---

## 7. Vedas (con lo que se barrió)

1. **`zGameModes` (124 B) — NO promocionable como está.** Su
   `__static_initialization_and_destruction_0` carga el 1.0f del pool con
   `lis r9, lbl_803EBE90@ha` / `lfs f0, lbl_803EBE90@l(r9)`, y **0x803EBE90 está
   dentro de la `.rodata` de `zGameplay`** (0x803EBB48..0x803ED338). Comprobado:
   `lbl_803EBE90` lo DEFINE `zGameplay.o` y lo referencia **sólo** `zGameModes.o`.
   El original tiene ese TU con `.rodata` y `.data` vacías (no hay rango en
   `splits.txt`), o sea que su pool entero se atribuyó a otra unidad. GCC siempre
   emite su propio literal: medido **+128 B** de `.rodata` viva que el rango no
   tiene. Barrido: nada — no hay construcción de C que haga que GCC referencie la
   entrada de pool de otro TU. **Sospecha para la próxima ronda: la frontera
   zGameModes/zGameplay está mal puesta**, igual que las dos del §5 de `r30-da`.
2. **`zMission`** — §4, ensayos c1..c5. Barridas las cinco colocaciones del
   bloque `asm` y la entrada de `keep.lst`. Se queda en **+32 B**: el
   `"bad_alloc"` que emite nuestro STL y el original no tiene aparte.
3. **Comodín de 4 B en `.rodata`: imposible.** `dtk` sólo crea `auto_*` en
   frontera múltiplo de 8 (`.rodata align:8` de la cabecera de `splits.txt`).
   Cortar `zDebug .rodata end:0x803D3DC8 -> 0x803D3DC4` **no da error y no crea
   comodín**: los 4 B se pierden y el DOL sale **−320 B**. Con
   `end:0x803D3DC0` sí nace `auto_05_803D3DC0_rodata` y el reparto solo da
   `DOL OK`. Mover la frontera a la unidad siguiente (`zDynamics .rodata
   start:0x803D3DC4`) también da `DOL OK` solo, pero con `zDebug` promocionado
   añade 4 B de relleno de alineación (`+32`); la solución de §2 lo evita.
4. **`.subsection` NO existe en `ngcas`** (`error: Unrecognised opcode
   ".subsection"`). No se puede colocar dato escrito a mano DETRÁS del pool de
   constantes del compilador dentro del mismo objeto. Y `.rodata.*` tampoco vale:
   el `ldscript` los pone con `*(.rodata.*)` **después de TODOS** los `*(.rodata)`
   del enlace, no detrás de los del propio objeto.
5. **`.type @object` no cambia el criterio de `-strip-unused-data`.** Probado en
   `zMission`: el símbolo escrito a mano y el extraído son idénticos en binding
   (LOCAL), tipo (`STT_OBJECT`), tamaño y flags de sección, y aun así el nuestro
   se estripa y el suyo no. El criterio del enlazador no lo he reducido a una
   regla; **la palanca que sí funciona es `keep.lst`** (§3).

---

## 8. Herramientas (scratchpad, prefijo `c31lk_`)

| guion | qué hace |
|---|---|
| **`c31lk_go.py`** | el banco de la r30 con edición **LIBRE** de `splits.txt` por JSON (`--edits`), `addkeep`/`delkeep` de `keep.lst`, re-troceado con `dtk` dentro del scratchpad (no toca `build/GOWE69/obj`), inserción de los comodines nuevos en el sitio del orden de enlace, y `--promo`/`--use`. Un ciclo completo = **11 s** |
| **`c31lk_dol.py`** | enlaza y, si rompe, imprime la tabla de 18 secciones del DOL y las **rachas** de bytes distintos con el símbolo de `symbols.txt` en que caen |
| **`c31lk_where.py`** | dice **entre qué dos direcciones** está el bloque que sobra o falta (prefijo/sufijo comunes) y lista los símbolos del ELF enlazado alrededor |
| **`c31lk_drift.py`** | compara la dirección de cada símbolo del ELF enlazado con la del `NFSMWRELEASE.ELF` original e imprime cada punto donde el desplazamiento CAMBIA |
| **`c31lk_sum.py`** | el resumen que decide: por sección, tamaño nuestro/extraído, símbolos que sobran y que faltan **con sus bytes**, y si el ORDEN de los comunes coincide |
| **`c31lk_rod.py`** | el mapa de una sección de datos, símbolo a símbolo, los dos lados alineados |
| **`c31lk_mkkeep.py`** | genera las entradas de `keep.lst` que le faltan a una unidad para que su dato muerto sobreviva a la promoción (§3) |
| **`c31lk_cc.py`** | compila **un fuente cualquiera** con los cflags exactos de una unidad a **donde yo diga**: permite probar variantes de una SourceList sin tocar el árbol (verificado: reproduce `.text`/`.rodata`/`.data`/`.bss` del objeto de ninja byte a byte) |
| `c31lk_obj.py`, `c31lk_who.py`, `c31lk_cmp.py` | secciones/símbolos de un `.o`; qué objetos referencian un símbolo; tabla de secciones de un DOL contra el original |

**Trampa nueva y cara**: `--use` con un `.o` cuyo **basename no sea el de la
unidad** invalida todas las entradas `<unidad>.o:<símbolo>` de `keep.lst` —el
enlazador casa por nombre de fichero— y el DOL sale **320 B corto** por un
motivo que no tiene nada que ver con lo que estás midiendo. Los objetos de prueba
tienen que llamarse `zDebug.o`, `zMission.o`… en un directorio aparte.

---

## 9. Estado del árbol y deuda

- **`config/GOWE69/splits.txt`, `symbols.txt`, `keep.lst`, `config.yml` y
  `configure.py`: intactos** (md5 idénticos al arrancar y al terminar). Todo lo
  medido se hizo sobre copias en el scratchpad con `c31lk_go.py`.
- **Dejo modificado UN fichero: `src/Speed/Indep/SourceLists/zDebug.cpp`**
  (40 líneas borradas, ninguna añadida). **No rompe el enlace**: `zDebug` sigue
  siendo `NonMatching`, así que no entra en el DOL; y cuando entre, da `DOL OK`.
  `measure.py` y `audit.py` sin cambio. Copia del original en
  `…/scratchpad/c31lk_backup/zDebug.cpp.orig`.
  (`git status` saca ~90 ficheros más bajo `src/`: **ninguno es mío**, son de la
  r30-da ya aplicada y de los otros agentes de esta ronda.)
- **Ninguna deuda de fuente**: no he escrito ensamblador de instrucciones ni
  pines. Lo único que he tocado es **borrar** datos duplicados.
- Liberados ~330 MB de `.json`/`.elf` de rondas cerradas del scratchpad
  (15 → 20 GB libres).

## 10. Lo que NO he probado

1. **Escribir los 73 símbolos de dato muerto de las cinco gordas** (§5). Es el
   trabajo grande y hay que hacerlo junto con §6.
2. **El pool de cabecera de bWare en las 24 unidades que no lo tienen** (§6). No
   he intentado ni escribirlo a mano ni reproducirlo por fuente.
3. **`zRender` / `zMiscSmall` / `zDynamics` con la limpieza de duplicados de §4.**
   Sólo he medido el efecto de `keep.lst`; no he tocado sus `asm()`.
4. **Las cinco gordas con `trypromo` tras aplicar nada**: sus deltas de hoy son
   los del encargo, reproducidos (zAttribSys −704/−32, zAnim −1.568/−192,
   zFEng −736/−64, zLua −2.080/−288, zMain −2.912/−96).
5. **El criterio exacto de `-strip-unused-data`** (§7.5). Tengo cuatro medidas
   que no encajan en una sola regla; `keep.lst` lo esquiva, pero entenderlo
   ahorraría barridos.
6. **Las once juntas**: sólo he enlazado `zDebug` con las otras diez sin
   promocionar. La suma de deltas del encargo sigue sin verificarse en un solo
   enlace.
