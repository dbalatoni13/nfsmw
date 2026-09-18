# Ronda 32 — `pre`: las cinco SourceLists grandes a las que les falta `.rodata`

Encargo: `zMain`, `zLua`, `zFEng`, `zAttribSys`, `zAnim` — 416.568 B de `linked`,
`.text` al 100 %, cero funciones pendientes, y el DOL corto en `.rodata`/`.data`.

**Ninguna llega a `DOL OK`, y ahora se sabe por qué con medida**: el déficit **no
es dato que se pueda escribir en la SourceList**, salvo la cabecera de 92 B. Lo
demás cae **DENTRO del pool de una función concreta**, entre dos `$LC` del
compilador, y ahí un `asm()` de ámbito de fichero no puede llegar.

Lo que sí queda hecho, verificado y puesto en el árbol: **la cabecera de bWare/STL
de 92 B en las cinco**, byte a byte contra el objeto extraído. El prefijo común de
la `.rodata` pasa de **0 B a 92 B en las cinco** (152 en `zFEng`) — que era
exactamente el bloqueo que el §6 de `r31-lk` dejó apuntado y sin hacer.

---

## 0. Las cuatro frases

1. **`-strip-unused-data` no borra el símbolo muerto entero: le quita
   `size & ~7`** — el tamaño redondeado *hacia abajo* a múltiplo de 8 — y **deja
   los `size % 8` bytes finales**. Por eso una cadena muerta de 14 B sobrevive
   como `"Array\0"` y una de 16 B desaparece del todo. Verificado byte a byte en
   `zAttribSys` (§1). Es la razón de que el delta del DOL sea **hasta cuatro
   veces** el delta del objeto.
2. **De los 7.920 B que le faltan a las cinco en `.rodata`, 4.864 son daño de
   ese estripado**, no dato ausente: forzando `keep.lst` con todos los símbolos
   de datos de NUESTRO objeto, `zMain` pasa de −2.912 a −736 y `zLua` de −2.080 a
   −864 (§3). **`zFEng` no gana nada: a esa unidad no le estripan nada.**
3. **La cabecera de bWare/STL de 92 B es la única pieza colocable a mano**, y ya
   está puesta y verificada en las cinco (§2). Vale +64/+96 B de DOL por unidad y
   **el prefijo común de la `.rodata` pasa de 0 a 92 B**.
4. **El resto del pool que falta cae dentro del pool de UNA función**, no en las
   fronteras de `#include` (§4, medido en `zFEng` con `c32pre_owner.py`): son
   constantes de **código muerto que el original compiló y nosotros no tenemos**
   (`FEngine::Render()`, `Setup`, `Object traversal`, `[HEADER]`, `[PLATFORM]`,
   `d:/mw/speed/indep/src/feng/FEPackage.cpp`…). **No es trabajo de datos: es
   escribir las funciones muertas.**

---

## 1. El mecanismo: `-strip-unused-data` quita `size & ~7`

Esto explica de golpe las cinco unidades y estaba sin describir.

Enlazando `zAttribSys` con nuestro objeto, su `.rodata` en el ELF resultante
empieza así (`c32pre_hex.py`):

```
803D0980  63 00 00 00 00 00 00 00 00 00 00 00 70 00 00 00  c...........p...
803D0990  00 00 00 00 65 00 00 00 00 00 00 00 41 72 72 61  ....e.......Arra
803D09A0  79 00 00 00 65 00 00 00 00 00 00 00 6f 6e 00 00  y...e.......on..
803D09B0  43 6c 61 73 73 00 00 00 42 6c 6f 62 00 00 00 00  Class...Blob....
```

No es basura: son **las COLAS de nuestras cadenas**. Con la tabla de símbolos de
nuestro objeto delante, la regla sale exacta:

| símbolo | tamaño | `size & ~7` (se va) | queda | observado |
|---|---|---|---|---|
| `$LC3` `"bad_alloc\0"` | 10 | 8 | `"c\0"` | `c` en +0 |
| `$LC60` `"Attrib::CollectionHashMap\0"` | 26 | 24 | `"p\0"` | `p` en +12 |
| `$LC62` `"Attrib::TypeTable\0"` | 18 | 16 | `"e\0"` | `e` en +20 |
| `$LC64` `"Attrib::Array\0"` | 14 | 8 | `"Array\0"` | `Array` en +28 |
| `$LC69` `"Attrib::RefSpec\0"` | 16 | 16 | — | no aparece |
| `$LC79` `"STL\0"` (VIVO) | 4 | — | entero | `STL` en +104 |
| `$LC88` `"%s_%08x\0"` (VIVO) | 8 | — | entero | entero en +144 |

Las posiciones cuadran al byte acumulando lo borrado. Los símbolos **vivos**
(los 12 que `.rela.text` referencia) y los que `keep.lst` fuerza no se tocan.

**Consecuencias prácticas**

- Un símbolo de datos muerto de **menos de 8 B nunca se estripa**. Por eso los
  `gap_*` de 1-5 B del objeto extraído sobreviven sin estar en `keep.lst`.
- Un `.rodata` nuestro de 904 B puede aportar 448 B al DOL. **El delta del objeto
  no es el delta del DOL**, y la diferencia es exactamente este daño.
- Explica el negativo de `und` en la r31 (`zFoundation` sólo recupera 32 de
  1.248 B forzando `keep.lst`): allí casi todo el pool ya era de tamaño < 8.

---

## 2. Lo que queda puesto: la cabecera de bWare/STL, verificada

**Los 92 B que abren la `.rodata` de 31 de 33 SourceLists**:

```
"GAMECUBE\0\0\0\0"  "d:/mw/speed/indep/bware/inc/bware.hpp\0\0\0"
"bad_alloc\0\0\0"   "%f,%f,%f\0\0\0\0"  "%f,%f,%f,%f\0"  "STL\0"
```

La ruta es el `__FILE__` de la máquina de EA: no se puede generar. Se escribe con
un `asm()` **antes del primer `#include`** (GCC 2.9 emite el asm de ámbito de
fichero donde está escrito, así que cae en el desplazamiento 0), con **el nombre
de símbolo que trae el objeto extraído de esa unidad**, que ya está en `keep.lst`:

| unidad | símbolos escritos | ya en `keep.lst` |
|---|---|---|
| `zFEng` | `pad_05_803EA7E8_rodata` (92) | sí |
| `zLua` | `pad_05_803ED338_rodata` (92) | sí |
| `zMain` | `lbl_803EF580` (92) | sí |
| `zAnim` | `lbl_803CF348` (9) + gap + `lbl_803CF354` (38) + gap + `lbl_803CF37C` (10) + gap + `lbl_803CF388` (28) | sí (los 4) |
| `zAttribSys` | `$LC2151483776`/`…788`/`…828`/`…840`/`…852`/`…864` + 4 `gap_05_*` | sí (los 6) |

Los `gap_*` no hacen falta en `keep.lst`: miden 2-3 B y **el estripado no les
quita nada** (§1).

### Resultado medido

```
python scripts/build_direct.py zAttribSys zFEng zAnim zLua zMain     5 ok
python scripts/measure.py <las cinco>   416568/416568 B  100.0000%  2771 fns
python scripts/keepchk.py               758 ok, 19 RANCIAS, 0 sin objeto  (igual)
```

| unidad | prefijo común `.rodata` | delta DOL `.rodata` | delta DOL `.data` | `trypromo.py` |
|---|---|---|---|---|
| | antes → después | antes → después | | |
| `zAttribSys` | **0 → 92 B** | −704 → **−640** | −32 | DOL ROTO |
| `zFEng` | **0 → 152 B** | −736 → **−640** | −64 | DOL ROTO |
| `zAnim` | **0 → 92 B** | −1.568 → **−1.472** | −192 | DOL ROTO |
| `zLua` | **0 → 92 B** | −2.080 → **−1.984** | −288 | DOL ROTO |
| `zMain` | **0 → 92 B** | −2.912 → **−2.816** | −96 | DOL ROTO |

(`zFEng` gana 152 B de prefijo porque sus 60 B siguientes ya casaban; los otros
92 clavados. El delta mejora +96 en cuatro y +64 en `zAttribSys`, donde parte de
los 92 B cae en un hueco de alineación.)

**El `.text` no se mueve**: un `asm()` de datos no emite una sola instrucción, y
`measure.py` da los mismos 416.568 B al 100 % antes y después.

**`zMain` tiene además `.bss +64` — y NO es mío**: está en la base (medido
restaurando `zMain.cpp` del respaldo y volviendo a enlazar: `bss 698232` contra
`698168` también sin mi cambio). Es una pista aparte para quien coja `zMain`.

---

## 3. El otro 61 %: daño de estripado, recuperable con `keep.lst`

Forzando en `keep.lst` **todos** los símbolos de datos de nuestro objeto
(`c32pre_keepall.py`, sin tocar el árbol: el keep va al scratchpad y se pasa con
`--ldflags`), sobre la base **sin** la cabecera:

| unidad | `.rodata` hoy | con todo forzado | daño de estripado | efecto lateral |
|---|---|---|---|---|
| `zAttribSys` | −704 | **−256** | 448 B | — |
| `zFEng` | −736 | −736 | **0 B** | — |
| `zAnim` | −1.568 | **−544** | 1.024 B | `.text` **+64 B** |
| `zLua` | −2.080 | **−864** | 1.216 B | — |
| `zMain` | −2.912 | **−736** | 2.176 B | `.text` **+192 B** |

**4.864 B de los 7.920 son daño de estripado, no dato ausente.**

**Y el aviso**: en `zAnim` y `zMain` forzar nuestros datos **resucita código**
(`.text` +64/+192 B), porque nuestras tablas apuntan a funciones que el original
sí dejó morir. O sea: `keep.lst` a lo bruto **no** es la cura; lo es sólo cuando
nuestros datos son ya los del original. `zFEng` y `zAttribSys` no tienen ese
efecto lateral.

---

## 4. Por qué NO se puede escribir el resto: cae dentro del pool de una función

Éste es el resultado que cierra el encargo, y está medido en `zFEng`, que es la
unidad limpia (estripado 0, delta de objeto = delta de DOL).

`c32pre_ins.py` hace un diff de **bloques** entre la región del ELF original y la
nuestra ya enlazada, y dice qué trozo falta y en qué desplazamiento nuestro va.
`c32pre_owner.py` dice después **qué función** referencia los `$LC` que hay a cada
lado de ese punto:

| falta | va en nuestro | entre qué dos `$LC` | ¿frontera de `#include`? |
|---|---|---|---|
| 92 B (cabecera) | 0x000 | — (antes de todo) | **sí — HECHO** |
| 20 B | 0x098 | `$LC94`/`$LC95`, **las dos de `Update__13FECodeListBoxf`** | no |
| 132 B | 0x100 | `$LC111`/`$LC112`, **las dos de `__9FEListBox`** | no |
| 232 B | 0x130 | `RecalculateCummulative__9FEListBox` / `CompleteScroll__9FEListBox` | no (mismo fichero) |
| 84 B | 0x14A | `Identify__9FEMatrix4` / `ProcessMouseForPackage__7FEngine` | sí, pero con `$LC129`/`135`/`136` sin referencia en medio |
| 36 B | 0x196 | dentro de `FEngine.cpp` | no |
| 16/43/57 B | 0x1CC… | dentro de `FEObject.cpp`/`FEPackage.cpp` | no |

Y el contenido dice de qué son: `lbl_803EA9C4` (236 B) es
`[HEADER]`/`Languages`/`[LABEL]`/`[PLATFORM]`/`[COMMENT]`/`[FLAGS]`/`[MAX_SI…`;
`lbl_803EAAC0` (92 B) es `FEngine::Render()`, `Setup`, `Object traversal`,
`SortObjects()`; `lbl_803EAB50` es `FEngMalloc` y `FEngMalloc[somefile]`; y hay
43 B con `d:/mw/speed/indep/src/feng/FEPackage.cpp`.

**Son constantes de código que el original compiló, el enlazador tiró del `.text`
y `keep.lst` conserva en el pool.** GCC 2.9 emite la constante donde la ve, o sea
en medio del pool de la función que se estaba compilando. Un `asm()` de ámbito de
fichero sólo puede caer **entre dos `#include`**, así que **no hay forma de
colocar esos bloques desde la SourceList**.

El camino que queda para las cinco es **escribir las funciones muertas** (perfil,
depuración, lectores de paquete en texto), que no dan porcentaje en `objdiff`
pero sí colocan el pool. Es un frente nuevo y grande; lo dejo nombrado, no
empezado.

---

## 5. Vedas y trampas nuevas

1. **No se pueden poner marcas de posición en `.rodata`.** Para saber en qué
   desplazamiento cae cada frontera de `#include` probé a meter detrás de cada
   uno una etiqueta de tamaño cero. `ngcas` aborta con
   **`error: Required relocation can't be represented in ELF format`**, y aborta
   igual con `.globl`, **sin** `.globl` y con `.set c32mNN, . - <ancla>`: una
   etiqueta a la misma dirección que una constante le roba la base a su
   reubicación. **La alternativa que sí funciona es `c32pre_owner.py`** (§4), que
   lee las reubicaciones `.rela.text` y no compila nada.
2. **`keep.lst` a lo bruto resucita código** en `zAnim` (+64 B de `.text`) y
   `zMain` (+192 B) — §3.
3. **El `.data` de `zLua` no se arregla con un `asm()` al final**: el hueco
   (`gap_06_8041D664_data`, 108 B de ceros) va **entre** `accessorTable` y
   `flagMapping`, o sea dentro del `.data` que emite un `.cpp` incluido. La forma
   de fuente que lo daría es un `accessorTable` **más largo** (25 entradas de 12 B
   en vez de 16), pero eso es una conjetura sobre el original y no la he probado.
4. **Los objetos `.o` traen `SHT_RELA` (tipo 4), no `SHT_REL`.** Un guion que
   busque el tipo 9 lee **cero** reubicaciones y da «ningún símbolo está vivo».
   Me costó un diagnóstico entero.

---

## 6. Estado del árbol

- **Dejo modificados CINCO ficheros**, y sólo con datos añadidos:
  `src/Speed/Indep/SourceLists/{zAttribSys,zAnim,zFEng,zLua,zMain}.cpp` —
  un bloque `asm()` de 92 B antes del primer `#include`, marcado `// c32pre:`.
  Respaldos en `…/scratchpad/c32pre_backup/*.cpp.orig`.
- **No rompen nada**: las cinco siguen `NonMatching`, así que sus objetos no
  entran en el DOL; `measure.py` da 100 % igual; `keepchk.py` da los mismos
  758/19/0. Y si algún día entran, entran **menos cortas** que antes.
- **`config/GOWE69/*`, `splits.txt` y `configure.py`: intactos**
  (`git status --porcelain config/ configure.py` vacío al terminar). Todas las
  pruebas de `keep.lst` se hicieron con ficheros del scratchpad y `--ldflags`.
- **No he escrito ensamblador de instrucciones ni pines.** Sólo `.4byte` de datos.
- **Verificación final** (con el enlace ya sano): `c32pre_base.py` →
  `main.dol de la lista de enlace VIVA: OK`; `trypromo.py` de las cinco →
  `DOL ROTO` con los mismos sha1 de §2; `diff` contra los respaldos → **sólo
  líneas añadidas, ninguna borrada** en las cinco.
- **Aviso de convivencia**: a media ronda el enlace base estuvo roto **por otro
  agente** — `L0039: Reference to undefined symbol
  _LcGetSlotString__Q26Realmc9GCMessagei` en
  `build/GOWE69/src/Packages/realmemcard/.../trctasks.o` (objeto NUESTRO,
  promocionado en `build.ninja`, recompilado a media tarde). Se resolvió solo.
  **Si un enlace tuyo falla en `realmemcard`, no es tu cambio**: pasa
  `c32pre_base.py` antes de investigar nada.

## 7. Herramientas (scratchpad, prefijo `c32pre_`)

| guion | qué hace |
|---|---|
| **`c32pre_hex.py`** | enlaza promocionando unas unidades y vuelca **en hexadecimal** la región que le pidas del ELF resultante. Es con lo que salió la regla `size & ~7` |
| **`c32pre_live.py`** | por sección de datos, qué símbolos están **referenciados**, cuáles fuerza `keep.lst` y cuáles están muertos, con sus bytes. **Lee `SHT_RELA`** |
| **`c32pre_ins.py`** | diff de **bloques** entre la región del original y la nuestra ya enlazada: qué falta, cuánto y en qué desplazamiento nuestro va |
| **`c32pre_owner.py`** | para un desplazamiento de `.rodata`, **qué función** referencia los `$LC` de alrededor. Dice si el punto cae en una frontera de `#include` o dentro de una función |
| **`c32pre_keepall.py`** | genera un `keep.lst` con **todos** los símbolos de datos de nuestro objeto, para medir el daño de estripado sin tocar el árbol |
| **`c32pre_genasm.py`** | genera el bloque `asm()` con los bytes EXACTOS del objeto extraído en un rango, con los nombres de símbolo del original |
| **`c32pre_apply.py`** | lo mete en la SourceList antes del primer `#include`, respetando los finales de línea |
| **`c32pre_reg.py`** / `c32pre_dump.py` / `c32pre_pref.py` | la región enlazada de los dos lados como cadenas; el volcado de una sección de los dos objetos; el prefijo común |
| `c32pre_cc.py` | compila una SourceList con sus cflags exactos y **enseña toda** la salida (`build_direct.py` la recorta y esconde el error) |
| `c32pre_base.py` | enlaza la lista VIVA sin sustituir nada: dice si el `main.dol` base está bien **antes** de acusar a tu cambio |

## 8. Lo que NO he probado

1. **Escribir las funciones muertas** que colocan el pool (§4). Es el frente que
   queda para las cinco.
2. **`accessorTable` de `zLua` con 25 entradas** (§5.3).
3. **`zFEng` con `keep.lst` extendido y la cabecera juntas**: la cabecera no
   necesita `keep.lst` (ya estaba) y `zFEng` no tiene daño de estripado, así que
   no hay nada que sumar; en las otras cuatro sí habría que medirlo junto, y no
   lo he hecho porque en `zAnim`/`zMain` mueve `.text`.
4. **Las cinco juntas en un solo enlace.**
