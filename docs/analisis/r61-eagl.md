# r61 — `eagl` (zEagl4Anim, zFe, zFe2): la escalera avanza en las TRES, y la veda de zFe2 estaba rota porque `mangfix.py` es trabajo de AGENTE

Agente `eagl`. Unidades: `zEagl4Anim`, `zFe`, `zFe2`.

**Tres ficheros tocados, los tres MIOS.** Cero cabeceras compartidas, cero
`config/`, cero `splits.txt`, cero `keep.lst`, cero `configure.py`, cero
`scripts/`.

    src/Speed/Indep/SourceLists/zFe.cpp
    src/Speed/Indep/SourceLists/zFe2.cpp        (incluye 16 alias reescritos por mangfix.py)
    src/Speed/Indep/SourceLists/zEagl4Anim.cpp

---

## 0. Titular

Metrica: **bytes distintos en los rangos de `splits.txt` de la unidad**, contra el
enlace base, con **copia privada de `keep.lst` ya pasada por `lcfix`** (la de la
r59/r60). Entre parentesis, la direccion de la **primera** diferencia de
`.rodata` (la metrica de escalera).

| unidad | antes | despues | gana | prefijo exacto de `.rodata` | secciones |
|---|---:|---:|---:|---|---|
| **`zFe`** | 27.181 | **26.521** | **−660** | 803DF93C → **803DFB70** (**+564 B**) | IGUAL → IGUAL |
| **`zFe2`** | 35.959 | **35.670** | **−289** | 803E473C → **803E4858** (**+284 B**) | IGUAL → IGUAL |
| **`zEagl4Anim`** | 93.573 | **93.279** | **−294** | 803D42C4 → **803D4390** (**+204 B**) | `text+4` → `text+4` |
| | | | **−1.243** | | |

Y una que nadie mide y esta ronda si se movio: la primera diferencia de la
**`.data`** de `zFe` pasa de `8041B5CF` a **`8041B612`** (+67 B) sin que yo haya
tocado un solo byte de `.data`. Es la escalera de `.rodata` arrastrando las
reubicaciones.

> **AVISO (§7): estas cifras son POST-`lcfix`.** Con el `keep.lst` de hoy sin
> tocar, `zFe` sale 31.935 y `zFe2` 41.807 — o sea PEOR que la base. Mis 153
> correcciones de `lcfix` no son cosmetica: sin ellas mis dos unidades de
> frontend son una regresion.

`fncmp` **IDENTICO antes y despues en las tres**:

    zFe          0 de 921      (0 B)
    zFe2         0 de 1307     (0 B)
    zEagl4Anim   2 de 318      (Initialize 2.352 + EvalState 456 = 2.808 B, las de siempre)

**Cero regresiones de codigo.** `checksplits`, `prefijochk`, `gapchk` de las tres
(`total 0`) y `mangfix --check` de las tres: limpios.

**Control obligatorio, hecho antes de creer ninguna cifra**: el enlace base con
la copia privada de `keep.lst` es **byte a byte igual al ELF ORIGINAL**
(`orig/GOWE69/NFSMWRELEASE.ELF`) dentro de los rangos de las tres unidades — 0 B
distintos en las tres.

Sello, **tres compilaciones seguidas del arbol final, las tres iguales**:

    zFe          ee144ad737c734ea49592799a316ee216a102390   (x3)
    zFe2         6cedec41e5a32c27ec7809747cfa2020bcc20a90   (x3)
    zEagl4Anim   aa541114a819e778282e46e4e77e04d0bb04bb06   (x3)

**Ninguna promociona**, y ninguna podia: son 26k/35k/93k B de colocacion.
`trypromo` con la ruta completa y el `keep.lst` privado:

    zFe + zFe2      DOL ROTO (6b8af8e0c5ee)
    zEagl4Anim      DOL ROTO (10bcc0acb886)

---

## 1. LA VEDA ROTA: `zFe2` no estaba bloqueada — `mangfix.py` solo edita `zFe2.cpp`

La r60 cerro `zFe2` con esto (`r60-resto.md §1.2`, y el mismo texto en el fuente):

> «**NO SE PUEDE EN zFe2**: quitar el `inline _AttribAllocTagOrder` corre el
> `DECL_UID` y los alias escritos a fuego en `zFe2.cpp` dejan de resolver — el
> ENLACE FALLA con tres L0039. Hay que pasar `mangfix.py` antes, y eso es
> **trabajo de ventana**.»

**Es trabajo de AGENTE.** `mangfix.py` reescribe los alias `.NNNNN` dentro del
propio `zFe2.cpp` — mi fichero — y no toca nada mas. El ciclo entero cabe en una
linea:

    python scratchpad/eagl61/bd.py zFe2 && python scripts/mangfix.py zFe2 && python scratchpad/eagl61/bd.py zFe2

Medido: **16 alias corregidos**, enlace correcto, `fncmp` 0/1307. La veda cae.

Con ella caida, la unidad se pudo trabajar por primera vez desde la r58, y de ahi
salen los −289 B del §3. **Pero el eje que la veda protegia es NEGATIVO**, y eso
es lo segundo que hay que escribir (§4).

---

## 2. `zFe`: el grupo de `uiMain.cpp`, adelantado entero (−660 B)

La r60 dejo la escalera en `0x803DF93C`, donde el objetivo quiere
`SMS_MESSAGE_%d`. Cuatro pasos, medidos uno a uno:

| paso | edicion | total | 1a dif |
|---|---|---:|---|
| base r60 | — | 27.181 | 803DF93C |
| EXP-1 | primer de `SMS_MESSAGE_%d{,_FROM,_VOICE,_SUBJECT}` + `GManager` | 27.086 | 803DF99C |
| EXP-2 | `#include speechtune.h` BAJADO detras de `uiSMSMessage.cpp` | 27.058 | 803DF99C |
| EXP-3 | dos primers con el grupo de `uiMain.cpp` + `asm()` de `UI_DebugCarCustomize.fng` en medio, y `.space 96 → 88` | **26.700** | 803DFAA4 |
| EXP-4+5 | primer del grupo `GAME_COMPLETED_DATA` + `.balign 8` de 4 B, y `.space 88 → 84` | **26.521** | **803DFB70** |

Tres cosas que valen para cualquier unidad:

**(a) Un `asm()` de fichero se puede meter ENTRE DOS PRIMERS.** El objetivo tiene
`UI_DebugCarCustomize.fng` en el cuarto puesto del grupo, y en `zFe` esa cadena
esta MUERTA (su tabla, `ScreenFactoryData`, vive en `FEPackageData.cpp`, que es de
`zFe2`). Partir el primer en dos y colar el `asm(".asciz ...")` en medio la coloca
**sin tocar `keep.lst`**: son bytes anonimos y `-strip-unused-data` va por
simbolo. Sale de su sitio en el bloque de cola de la r52.

**(b) Adelantar el grupo ENTERO es mas barato que retrasar lo que estorba.**
Delante del grupo se cuelan `ScrollerDatumNode`, `ScrollerSlotNode` y
`VehicleParams` (68 B con su relleno). No se pueden retrasar (§5). Pero adelantar
por delante de ellos los 264 B que el objetivo pone despues cuesta un primer.

**(c) La ventana de 32 muerde en las dos direcciones.** EXP-3 metio **+8 B** de
`.rodata` y el total SUBIO a 29.204 aunque el prefijo exacto habia avanzado 264 B:
`.data` se fue 32 B y fabrico 389 B de diferencias que no eran mias. Con
`.space 96 → 88` en el bloque de la r52, 26.700. **El aviso de la r60 («compensa
ANTES de leer nada») hay que leerlo tambien al reves: cuando METES bytes, quita
los mismos.**

---

## 3. `zFe2`: la escalera de versiones, replicada de `zFe` (−289 B)

El objetivo pone en `0x803E473C..0x803E4854`, justo detras del bloque VERBATIM de
la r58: `1.8.1`, `19.8.31`, `aivehicle`, `pursuitlevels`, `pursuitescalation`,
`pursuitsupport`, `GRaceStatus`, las cuatro `SMS_MESSAGE_*`, `MMiscSound`,
`SoundID`, `Radar_DirectionArrow`, `Radar_Icon`. Nosotros abriamos con `PAD4` +
`GAMECUBE` + las ocho `Attrib::*`.

Once de las doce son `$LC` **VIVOS** — comprobado contando las reubicaciones que
apuntan a su simbolo en el `.o`, no suponiendolo: `1.8.1` `$LC1164` 4 reub,
`19.8.31` `$LC269` 8, `Radar_Icon` `$LC373` 2, `MMiscSound` `$LC366` 2… — asi que
van con dos primers. `GRaceStatus` no tiene `$LC` (esta MUERTA) y sale del bloque
de cola con un `asm()`, exactamente como en `zFe`.

Ademas la receta arregla de paso una PERMUTACION: teniamos
`pursuitescalation` (803E49D8) **delante** de `pursuitlevels` (803E49F8) y el
objetivo los quiere al reves.

**La compensacion de tamano salio de un DUPLICADO, no de relleno.** La escalera
dejaba `.rodata` en **+8 B**. En vez de rellenar, `dupcola.py` (sonda mia) comparo
el multiconjunto de cadenas del bloque de cola contra el objetivo y encontro
siete cadenas que emitimos de mas; **`"Covered"` mide exactamente 8 B**. Borrada
del bloque de cola: `dtam +0` en las nueve secciones y el total a 35.670.

Las otras seis sobrantes que deja el censo, por si alguien las necesita para
compensar mas adelante: `MGeneric` (9 B), `MAudioReflection` (17), `PlayerNum`
(10), `%s%s` (5), `CustomizeCategory.fng` (22), `ScrollBar` (10, tenemos 3 copias
y el objetivo 2).

---

## 4. NEGATIVO MEDIDO, y cierra el eje que la r60 dejo abierto en `zFe2`

Con la veda del §1 rota, probe lo que la r60 queria probar. **No sirve.**

| variante | `.rodata` dtam | total | 1a dif |
|---|---:|---:|---|
| base r60 | 0 | 35.959 | 803E473C |
| `#define ATTRIB_TAG_ORDER_HAND_POOL 1` (+ `mangfix`) | 0 | **35.963** | 803E473C |
| escalera del §3 | 0 | **35.670** | 803E4858 |
| escalera + las DOS guardas `HAND_POOL`, compensados los 656 B | 0 | **35.852** | 803E4858 |

Dos conclusiones duras:

1. **`ATTRIB_TAG_ORDER_HAND_POOL` no quita las ocho `Attrib::*`: solo las
   PERMUTA** (pasan a `TypeDesc, Database, Attribute, Instance, Definition,
   Class, RefSpec, Blob`). O sea que el `inline _AttribAllocTagOrder` **no era su
   unica fuente**: las emite tambien el `ATTRIB_TAG` de `AttribSys.h`. El modelo
   de la r60 («las copias que cc1plus emite DETRAS del bloque») esta incompleto.
2. Anadiendo `ATTRIB_TAGS_HAND_POOL`, que si las mata, la unidad pierde **656 B**
   de `.rodata` — y se lleva por delante los `Attrib::Gen::*` que SI necesita. Con
   los 656 compensados al final, el total sube 182 B respecto a la escalera sola y
   **la primera diferencia no se mueve un byte**.

O sea: **las copias de `0x803E4858` NO son el bloqueo de `zFe2`**. Escrito en
`zFe2.cpp` con sus cuatro cifras, donde `previo.py` lo encuentra. Que nadie gaste
otra ronda ahi.

---

## 5. `zFe`: DONDE se atasca la escalera, y por que no lo puedo arreglar yo

La escalera de `zFe` se para en `0x803DFB70` por 68 B: `ScrollerDatumNode`,
`ScrollerSlotNode`, `VehicleParams` y una cadena de un caracter (`"p"`, 8 B con su
relleno). Medido, no supuesto:

* `ScrollerDatumNode` es `$LC245` con **7 reubicaciones** y `ScrollerSlotNode`
  `$LC247` con **3**: son literales VIVOS. Anularlos con `DEAD_STR` cambia el
  CODIGO y rompe `fncmp`. **No es via.**
* `VehicleParams` es `$LC329` con **CERO reubicaciones**: esta MUERTO y solo lo
  salva `keep.lst` (`@lc zFe "VehicleParams"`). Ese si se puede mover, pero solo
  vale 16 de los 68.
* El objetivo los tiene en `0x803E00DC`, `0x803E00F0` y `0x803E0908` — **1,6 kB
  mas abajo** — y justo DETRAS de los dos primeros pone `ARRAY_SCROLL_REGION`.
  Esa cadena la crean `feArrayScrollerMenu.cpp` y `uiEATraxJukebox.cpp`, o sea que
  **en el original `feScrollerina.hpp` se parseaba por PRIMERA VEZ en la zona de
  `uiEATraxJukebox` (zFe.cpp:~250), no en la linea 1 de `uiMain.cpp`.**

Aqui llega por `uiMain.hpp:6 → FEIconScrollerMenu.hpp:15`.

**PROPUESTA (cabecera compartida — NO la he tocado, regla 6):** quitar el
`#include ".../feScrollerina.hpp"` de
`src/Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp:15`.

Lo he medido **sin tocar el arbol**, con una copia sombra y `-I` delante en los
cflags de mi compilacion (`scratchpad/eagl61/bdshadow.py`), y con el control de
sintaxis obligatorio de `nfsmw-sombrear-cabecera` (metido un `@@@` en la copia:
la compilacion falla y el chorro de errores nombra la ruta del sombreado, asi que
el sombreado SI se usa). Resultado:

* **el include NO es necesario para `eScrollerAlignment`** — esa la trae
  `feWidget.hpp:17`, que ya se incluye en la linea 9;
* **lo unico que se rompe es `FEScrollBar *GetScrollBar()` en la linea 280**
  (`ScrollBar` es miembro POR VALOR, o sea que hace falta el tipo completo).

Asi que la propuesta concreta es partir `feScrollerina.hpp` o mover
`FEScrollBar`, no borrar el include a secas. **La cabecera la incluyen ~20
ficheros de varias unidades**, asi que hay que medirlo con el metodo del `.o`
privado antes de aplicarlo. Vale los 68 B de aqui **y toda la escalera de
`0x803DFB70` a `0x803E00DC`, 1.592 B.**

**Segundo hallazgo de la misma zona, sin dueno todavia**: a partir de
`0x803DFB74` el objetivo tiene **CINCO** grupos `1.0f / 0.1f 160.0f` y nosotros
**TRES**, con el `3a83126f` de cierre en los dos. Faltan 32 B de pool de funcion
con `fncmp` a 0/921 y `.text` al mismo tamano — o sea que esos dos grupos existen
en otro sitio de nuestra `.rodata`. Es orden de pool por funcion, no codigo.

---

## 6. `zEagl4Anim`: el bloque VERBATIM acaba EXACTAMENTE en la primera diferencia

El `asm()` de cabecera (`lbl_803D3FE8`, `0x2DC`) termina en `0x803D42C4`, que es
**exactamente** la primera diferencia de `.rodata` de la unidad. El objetivo pone
ahi once cosas y nosotros abriamos con la ultima de las once
(`EAGL4::SymbolEntry`).

    OBJETIVO   AnimId | 4 B de cero | @EAGL4::dynamic destructor list | .symtab |
               .strtab | ::: | EAGL4::dynamic symbols | EAGL4::dynamic symbols 2 |
               DynamicLoader::AddPatchAddress | EAGL4::SymbolPool::mpSymbolTable |
               EAGL4::SymbolEntry
    NOSOTROS   EAGL4::SymbolEntry | 43300000 80000000 x N

Tres de las once (`AnimId`, `@EAGL4::dynamic destructor list`,
`DynamicLoader::AddPatchAddress`) estaban en el bloque de cola de la r52, 0x700 B
tarde, y salen con `asm()`; el resto son `$LC` vivos y salen con dos primers.
**`EAGL4::SymbolEntry` se coloca solo**: como es lo primero que emitia cc1plus, al
meterle delante los otros diez cae en su sitio sin tocarlo.

Detalle que costaria una compilacion si no se dice: **el objetivo tiene ahi dos
cadenas SIN alinear**. `AnimId` acaba en `0x803D42CA`, hay **cuatro** bytes de
cero, y `@EAGL4::dynamic destructor list` empieza en `0x803D42CF`, que es IMPAR.
La forma que lo reproduce es `.asciz` + `.space 4` + `.asciz` + `.balign 4`, leida
del volcado hex y no del listado de cadenas (el listado dice `PAD3` porque agrupa
los ceros al alinear). Resultado: `dtam +0` sin ninguna compensacion.

Ganancia: `.rodata` 3.113 → **2.860 B** distintos, prefijo exacto +204 B, total
−294 B. `fncmp` sigue en 2/318 y `mangfix --check` limpio (esta unidad no tiene
alias mangled, asi que el primer no cuesta nada ahi).

**Donde se para**: `0x803D4390`. El objetivo tiene una tirada de `3f800000` y
nosotros una de `43300000 80000000` (el doble magico de la conversion
`int → float`). Es pool por funcion; misma familia que el atasco de `zFe`.

Y el aviso de higiene: **`zEagl4Anim.cpp` es CRLF** (`zFe.cpp` y `zFe2.cpp` son
LF). Un script de edicion que escriba `\n` a pelo deja el fichero MIXTO. El
chequeo bueno es contar `\r\n` contra `\n` totales — `grep` no vale
(`crlf-grep-miente`).

---

## 7. `lcfix`: **153 correcciones pendientes, las 153 MIAS. SIN APLICARLAS MIS DOS UNIDADES DE FRONTEND EMPEORAN**

**Esto es lo mas importante del informe despues del §1, y hay que leerlo antes de
commitear nada.** Todas mis cifras son el estado POST-`lcfix`. Medido con el
`keep.lst` REAL de hoy, que esta rancio respecto a mis fuentes:

| unidad | base r60 | mio, con `lcfix` | mio, SIN `lcfix` |
|---|---:|---:|---:|
| `zFe` | 27.181 | **26.521** | **31.935** (`rodata−432`) |
| `zFe2` | 35.959 | **35.670** | **41.807** (`rodata−624`) |
| `zEagl4Anim` | 93.573 | **93.279** | **93.279** (identico: sus 2 correcciones no mueven nada) |

`linkdelta.py` lo canta igual (`zFe rodata-432`, `zFe2 rodata-624`): mover un
literal renumera su `$LC`, las entradas de `keep.lst` pasan a nombrar otra cosa y
`-strip-unused-data` se lleva cadenas muertas que el objetivo SI tiene. **Mis
ediciones de `zFe` y `zFe2` son una REGRESION de 4.754 y 5.848 B mientras
`lcfix.py` no se pase.** No es un riesgo teorico: es la cifra medida.


    zFe   88     zFe2   63     zEagl4Anim   2

Al empezar la ronda `lcfix --check` daba **0 CORRIGE** (solo 54 `FALLO`
cosmeticos de la familia «no tiene simbolo `$LC` propio», que ya venian). Las 153
las he creado yo moviendo literales, y **todas mis cifras son ya el estado
POST-`lcfix`** (mido con una copia privada de `keep.lst` resuelta por contenido).

**No son venenosas para nadie**: `keep.lst` indexa por OBJETO (`zFe.o:$LCn`), asi
que una correccion de `zFe` no puede tocar a otra unidad. Y el control de que la
base sigue siendo el original (§0) da 0 B distintos en las tres.

**No son mias** y las he visto pasar mientras median otros: `zLua` 69–81 y
`zSpeech` 33. Van y vienen segun recompilan sus duenos.

`FALLO` que sigue abierto y es de una unidad mia: `zFe: 'GAMECUBE' no tiene
simbolo $LC propio` — de la familia cosmetica que la r58/r59 ya documento (la
entrada apunta al `$LC` vivo de su vecina). Es limpieza, no arreglo.

---

## 8. Lo que pido, con la cifra

| propuesta | vale | quien |
|---|---|---|
| **`python scripts/lcfix.py`** — 153 correcciones mias (zFe 88, zFe2 63, zEagl4Anim 2). Sin esto `linkdelta`, `dolwhere` y `trypromo` mienten sobre mis tres unidades | obligatorio | ventana |
| **Partir `feScrollerina.hpp`** (o mover `FEScrollBar`) para que `FEIconScrollerMenu.hpp:15` no la arrastre — §5, con el sombreado ya medido | 68 B ya, y la escalera de `zFe` de 803DFB70 a 803E00DC (**1.592 B**) | ventana + duenos de las ~20 unidades que la incluyen |
| **Borrar de `docs/analisis/r60b-auditoria-vedas.md` y del DOSSIER la etiqueta «zFe2 bloqueada, trabajo de ventana»** — `mangfix.py` es de agente (§1) | desbloquea 250 kB de unidad | ventana |
| **`dupcola.py` para TODAS las unidades con bloque `.asciz` de cola** — el censo de multiconjunto encuentra la compensacion de tamano exacta sin inventar relleno; en `zFe2` valio los 8 B justos | evita `.space` a ojo | los duenos |

Nada que proponer para `splits.txt`, `keep.lst` ni `configure.py`: no he movido
una raya ni he creado un simbolo nuevo.

---

## 9. Herramientas (todas en `scratchpad/eagl61/`, ninguna en `scripts/`)

| script | que hace |
|---|---|
| `bd.py <u>…` | `build_direct` con filtro por nombre **EXACTO** de unidad, y sella el `.o` con `sha1`. Es el arreglo del §7 de la r60: `build_direct.py zFe` recompila TAMBIEN `zFe2` y `zFeOverlay`, que es de otro agente. |
| `lcpriv.py` | `keep.lst` privado resuelto por CONTENIDO, sin escribir en `config/`. Imprime cuantas correcciones quedarian pendientes, por unidad. |
| `rangos.py <u>…` | la metrica de la r59/r60: bytes distintos en los rangos de `splits.txt`, primera diferencia por seccion y delta de TAMANO de seccion. |
| `control.py <u>…` | el enlace base contra `orig/GOWE69/NFSMWRELEASE.ELF`. Si no da 0, ninguna cifra de la ronda vale. |
| `cmpstr.py <u> <dir>` | objetivo contra nuestro enlace, cadena a cadena, desde una direccion. |
| `objlist.py` / `hexat.py` | el objetivo solo, y el volcado hex de los dos (el que hizo falta para el relleno impar de `zEagl4Anim`). |
| `findstr.py <u> <cad>…` | donde vive una cadena en los dos enlaces, marcando las que caen en el rango de la unidad. |
| `reftext.py <o> <cad>` | **si un `$LC` esta VIVO**: cuenta las reubicaciones que apuntan a el. Es lo que separa «va con primer» de «va con `asm()`». |
| `dupcola.py <u>` | cadenas del bloque `.asciz` de cola que emitimos MAS VECES que el objetivo: bytes borrables sin perder contenido. |
| `bdshadow.py <u>` | compila la unidad con `-I` a una copia sombra de una cabecera: mide un cambio en cabecera compartida **sin tocar el arbol** ni pisar a otros agentes. |
| `meas.py <u>` | compila + `lcpriv` + `rangos` + `fncmp` de una tacada. |

Los volcados (`.i`, `.elf`, `.gcse`) estan borrados; queda solo el codigo (~30 kB)
y la copia de `keep.lst`.

---

## 10. Sorpresas

1. **Una veda de «trabajo de ventana» puede ser trabajo de agente.** `mangfix.py`
   solo edita la SourceList de la unidad. Nadie lo comprobo en dos rondas.
2. **Un `asm()` de fichero se intercala ENTRE dos primers** y coloca una cadena
   muerta en medio de un grupo vivo, sin tocar `keep.lst`.
3. **La compensacion de tamano puede salir de un DUPLICADO.** No hace falta
   `.space`: el bloque de cola de `zFe2` emitia `"Covered"` dos veces y median
   exactamente los 8 B que sobraban.
4. **La ventana de 32 muerde tambien cuando METES bytes.** +8 B de `.rodata`
   convirtieron un avance de 264 B de prefijo en un retroceso de 2.023 B en el
   total. La regla de la r60 hay que leerla en los dos sentidos.
5. **El bloque VERBATIM de cabecera de `zEagl4Anim` acaba EXACTAMENTE en su
   primera diferencia.** Merece la pena comprobar `direccion + .size` contra la
   primera diferencia en todas las unidades que tengan uno: dice si el bloque esta
   bien cortado y donde hay que escribir.
6. **El objetivo tiene cadenas SIN alinear.** `@EAGL4::dynamic destructor list`
   empieza en `0x803D42CF`. El listado de cadenas lo esconde (agrupa los ceros);
   solo el volcado hex lo dice. Un `.balign 4` de mas ahi habria costado la ronda.
7. **`reftext.py` decide la herramienta en un segundo.** `VehicleParams` parecia
   un literal vivo intocable y tiene CERO reubicaciones: esta muerto y solo lo
   salva `keep.lst`. Suponerlo vivo me habria costado un barrido entero.
8. **El sombreado con `-I` SI funciona cuando el `#include` lleva ruta de
   proyecto.** La nota `nfsmw-sombrear-cabecera` («con `-I` no funciona si el
   include es RELATIVO») no cubre este caso: `FEIconScrollerMenu.hpp` se incluye
   con `"Speed/Indep/Src/..."` y basta con copiar UN fichero. Es la forma de medir
   una cabecera compartida sin pisar a nadie.
