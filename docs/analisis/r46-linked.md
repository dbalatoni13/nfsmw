# r46 --- `linked`: el racimo `gc_interface` + `asd2`

**Resultado: DEMOSTRADO.** Con las cinco fuentes de este informe y **una linea de
`splits.txt`**, el enlace completo sale
`9619ba57c9919f95f7f2ac951a2166a3517f91e3` sustituyendo nuestro `gc_interface.o`
y quitando `asd2.o`. Son **6.240 B de `linked`** (6.152 de `gc_interface` + 88 de
`asd2`) pendientes solo de la autorizacion de configuracion.

`fncmp` sigue en **0 de 31** funciones distintas en `gc_interface` y en **0 de 6**
en `gc_blockcalculator`; ninguna de las 15 unidades de `realmemcard` empeora.
`lcfix.py --check` y `checksplits.py` limpios.

---

## 1. El diagnostico del brief estaba a medias: no era «el estatico con constructor»

El brief decia que GCC bautiza `_GLOBAL_.I.<X>` «segun el estatico con
constructor de la unidad». **Medido y NO es eso.** Con un fichero de cinco
lineas compilado con `ngccc -S`:

```cpp
struct S { S(); ~S(); int x; };
namespace NS { int zzz_first = 7; S aaa; }
```

sale `_GLOBAL_.I._2NS.zzz_first` --- una `int` **sin constructor**. Lo que fija el
nombre es `first_global_object_name`: **el nombre de ensamblador de la PRIMERA
variable global que la unidad ensambla**, tenga constructor o no. Un bloque
`asm()` de ambito de fichero **no** cuenta (por eso
`_6Realmc.FILENAME_ALL_FILES`, que sale de un `asm()`, no bautizaba nada y si lo
hacia `GCInterface::mpDriver`).

### Donde esta `Realmc::ROOT_DIRECTORY_NAME`: en ninguna parte, y por eso encaja

`orig/GOWE69/NFSMWRELEASE.ELF` tiene **exactamente dos** simbolos con ese texto,
y los dos son los envoltorios: `_GLOBAL_.I._6Realmc.ROOT_DIRECTORY_NAME`
(0x80391518) y `_GLOBAL_.D.` (0x80391544). **La variable en si no existe en el
ELF.** No es una contradiccion: es la prueba. Era una global de
`gc_interface.cpp`, declarada antes que `GCInterface::mpDriver`, que **no
referenciaba nadie**, y el enlazador se la llevo --el mismo `-strip-unused-data`
que documenta `linkdelta.py`--. Su tipo y su contenido no se pueden recuperar:
no quedan bytes.

Reproducido con el marcador minimo, en `.data` (seccion que el rango del
objetivo no reclama, justo porque se estripo):

```cpp
const char *ROOT_DIRECTORY_NAME __asm__("_6Realmc.ROOT_DIRECTORY_NAME")
    __attribute__((section(".data"))) = 0;
```

**Medido: cuesta 4 B en el objeto y CERO en el DOL** (enlace en sombra `DOL OK`).
Y el par pasa a llamarse `_GLOBAL_.I._6Realmc.ROOT_DIRECTORY_NAME` /
`_GLOBAL_.D....`, igual que el original. Sus 44+44 B son ademas **identicos byte
a byte** a los de `asd2.o` salvo el desplazamiento del `bl` (nosotros
reubicamos contra la seccion con addend 0x9B0, el objetivo contra
`__static_initialization_and_destruction_0_803906C0` con addend 0).

Quitarlo solo cambia el nombre del par: **el DOL sale byte a byte con las dos
formas**. Se conserva porque asi objdiff empareja los 88 B y cuentan como
`matched`.

---

## 2. Lo que de verdad retenia la unidad: el JUEGO y el ORDEN de emision

El codigo ya casaba al 100 %. Lo que sobraba eran **272 B de `.text` y 104 B de
`.rodata`** en simbolos que el objetivo tiene en OTRAS unidades, y --lo que no
se ve en `promote.py`-- el **orden** de lo que si es nuestro.

Dos reglas de GCC 2.9, medidas con `ngccc -S`:

- **Las vtables salen en orden INVERSO de declaracion de la clase.** Comprobado
  con un fichero de cinco lineas (tres clases A, B, C con destructor inline: las
  vtables salen C, B, A y los destructores fuera de linea salen A, B, C).
- **Las inline diferidas salen en el orden en que se DEFINEN**, repartidas en las
  pasadas de `finish_file` (lo que una pasada emite arrastra a la siguiente).

### Los cinco ensayos, con la cifra

Base: `.text` 6.424 / `.rodata` 712 / `.data` 0 / `.ctors` 4 / `.dtors` 4.
Objetivo (`gc_interface.o` + `asd2.o`): `.text` 6.152+88 / `.rodata` 608 /
`.ctors` 4 / `.dtors` 4.

| # | ensayo | efecto medido |
|---|---|---|
| E1 | apagar la pareja `#pragma interface`/`implementation` de `gc_blockcalculator.h` **solo en esta unidad** (`REALMC_BLOCKCALC_NO_INTERFACE`) | `.text` 6.424 -> **6.364** (-60: `_._BlockCalculatorImp` 52 + `GetResult` 8); `.rodata` 712 -> **664** (-48: `_vt.BlockCalculatorImp`) |
| E2 | `#define REALMC_LCFMT_BASE lbl_804144C0` (las cinco cadenas `LC_msg` estan MUERTAS aqui; su bloque de 28 B es `lbl_804144C0`, el comodin `auto_05_804144C0_rodata`) | `.rodata` 664 -> **632** (-32) |
| E3 | `#pragma interface` acotado a `GCMessage` dentro de `gc_interface_impl.h`, cerrado con un `#pragma implementation` a mitad de fichero | `.text` 6.364 -> **6.240**; `.rodata` 632 -> **608**. **Tamanos exactos** |
| E4 | `gc_blockcalculator.h` deja de incluir `gc_driver.h` (declaraciones adelantadas) | `_vt.BlockCalculator` pasa de la 2.a posicion a la ULTIMA de `.rodata`; `_._BlockCalculator` se coloca detras de `ResetAutosaveCardDetection` |
| E5 | mover la definicion de `IsBusy` DELANTE de la de `GetBlockSize` | las dos quedan en el orden del objetivo |
| E6 | sacar de la clase `TaskManager::UpdateCurrentTask` y `TaskTrc::Init` y definirlas en la unidad, detras de `EndTask` (`REALMC_TASK_INLINES_IN_TU`) | `.text` **ORDEN IDENTICO** al objetivo |
| E7 | mover `TaskStack` + `TaskManager` delante de `struct Task` en `memcard_interface_impl.h` | `_vt.TaskManager` pasa detras de `_vt.GcTask`: `.rodata` **ORDEN IDENTICO** |
| E8 | `ROOT_DIRECTORY_NAME` (seccion 1) | `.data` 0 -> **4**, y el par `_GLOBAL_` toma el nombre del original |

Detalles que costaron tiempo y conviene no repetir:

- **`REALMC_GC_MESSAGE_DTOR_OUTOFLINE` NO vale** para quitar `_vt.GCMessage`.
  El objetivo **no llama al destructor en ningun sitio**: su `__tcf_0` (20 B)
  solo repone el vptr, y en su tabla de reubicaciones no aparece
  `_._Q26Realmc9GCMessage`. Con el destructor fuera de linea, `__tcf_0` y el
  inicializador estatico cambiarian. Lo unico que deja `_vt.GCMessage`,
  `Init` y `Clear` como UNDEF **manteniendo el destructor inline** es
  `CLASSTYPE_INTERFACE_ONLY`, o sea `#pragma interface`.
- **Un `#pragma implementation` a mitad de cabecera SI restaura el estado.**
  `gc_interface_impl.h` declara `GCMessage` (que queremos externa) y
  `GCInterface` (cuya vtable hay que emitir). Poner `#pragma interface` antes de
  `GCMessage` y `#pragma implementation "..."` justo detras cierra la ventana:
  `_vt.GCInterface` se sigue emitiendo. Con el nombre del propio fichero GCC
  avisa (`appears after its #include`); **con un nombre cualquiera
  (`realmc_gcmsg_interface_end.h`) el efecto es identico y el aviso desaparece**
  --comprobado: mismas secciones y mismo orden--.
- **`.set` con referencia adelantada no crea el simbolo.** Un `asm()` de ambito
  de fichero con `.globl X` + `.set X, __static_initialization_and_destruction_0`
  sale en el `.s` (lineas 3-4) y **no llega al objeto**: `ngcas` lo descarta
  porque el destino todavia no existe, y `finish_file` emite esa funcion
  siempre despues de los `asm()` de la fuente. No hay forma de exportar el
  nombre con sufijo desde la fuente.

---

## 3. `ngcld` descarta los duplicados: WEAK pierde contra GLOBAL

Hallazgo que explica el ultimo bloqueo y que vale para todo el arbol.

`gc_blockcalculator.o` (nuestro, ya promocionado) emitia `.text` 364 B y
`.rodata` 96 B contra los 312/48 del objetivo, **y el DOL salia byte a byte
igual**. Los 100 B de mas son `_._Q26Realmc15BlockCalculator` (52) y
`_vt.Q26Realmc15BlockCalculator` (48), duplicados de los de `gc_interface.o`, y
el enlazador **se lleva la copia sobrante y sus bytes**: `_vt.BlockCalculatorImp`
sigue en 0x804149E8 sin desplazarse.

La regla, medida leyendo el `bind` de los simbolos:

- **entre una definicion GLOBAL y una WEAK gana la GLOBAL**, este donde este en
  el orden de enlace;
- **entre dos WEAK gana la del objeto que va ANTES.**

Los simbolos que GCC saca por `.gnu.linkonce.*` --que `ngcas` funde en `.text` /
`.rodata`-- son WEAK; los que salen bajo `#pragma implementation` son GLOBAL.
Eso rompia el primer ensayo del racimo: nuestro `_._BlockCalculator` (WEAK)
perdia contra el de `gc_blockcalculator.o` (GLOBAL, por su `#pragma
implementation`), el simbolo se resolvia a 0x80398DD0 y **la unidad entera se
quedaba 52 B corta**, arrastrando 68 simbolos. Apagando tambien alli la pareja
de pragmas los dos quedan WEAK, gana el primero del enlace (`gc_interface.o`,
posicion 522 contra 531) y encaja.

Efecto lateral bueno: `gc_blockcalculator.o` pasa a tener **el prefijo exacto
del objetivo** (ctor 68, Init 80, Clear 20, SetFileInfo 84, `_._Imp` 52,
`GetResult` 8) con los 100 B de duplicados **detras** en vez de intercalados.

---

## 4. Las medidas de enlace

Arnes: `scratchpad/r46_linked/shadowlink.py`, una variante de `trypromo.py` que
ademas puede **quitar** objetos de la lista de enlace (es lo que hace falta para
probar la fusion de `asd2` sin reextraer). No toca `configure.py`, `config/`,
`build/GOWE69/main.elf` ni el DOL real.

| enlace | resultado |
|---|---|
| **control** --- arbol tal cual, sin sustituir nada, con las cinco fuentes ya modificadas | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` **DOL OK** |
| **ensayo del racimo** --- nuestro `gc_interface.o` sustituido y `asd2.o` fuera | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` **DOL OK** |
| `trypromo.py` **con `asd2.o` dentro** (reparto vigente) | `ENLACE FALLA: asd2(1) : error: L0039: Reference to undefined symbol __static_initialization_and_destructi...` |

El tercero es esperado y **no se puede arreglar desde la fuente**: aunque se
exportara el nombre con sufijo, `asd2.o` seguiria aportando otros 88 B de
`.text` y una segunda entrada de `.ctors` y de `.dtors`. Nuestro par
`_GLOBAL_.I./.D.` es **LOCAL** (GCC no le pone `.globl`), asi que tampoco puede
deduplicarse contra el de `asd2`. **La fusion en `splits.txt` es obligatoria.**

Antes del ensayo, el mismo enlace en sombra con el reparto vigente daba
`DOL ROTO (61feb1033ca0cdf889ceeb366cd471ad25f508fe)` y **exactamente 79
simbolos desplazados**, todos explicables: `IsBusy`/`GetBlockSize` cruzados
(+36/-28), `_._BlockCalculator` fugado a 0x80398DD0, `UpdateCurrentTask` y
`Init__TaskTrcb` adelantados (-204/-312), y `_vt.TaskManager` y
`_vt.BlockCalculator` fuera de sitio (-200/-288) arrastrando 88 B a las ocho
vtables de la familia `TaskTrc`. Los ocho ensayos de la seccion 2 cierran los
cuatro grupos.

### Estado final del objeto

| seccion | nuestro | objetivo (`gc_interface.o` + `asd2.o`) |
|---|---:|---:|
| `.text` | 6.240 | 6.152 + 88 |
| `.rodata` | 608 | 608 |
| `.bss` | 2.108 | 2.108 |
| `.sdata` | 28 | 28 |
| `.sbss` | 4 | 4 |
| `.ctors` | 4 | 4 |
| `.dtors` | 4 | 4 |
| `.data` | 4 | --- (estripada; ver seccion 1) |

Orden de `.text` y de `.rodata`: **identico**, simbolo a simbolo, incluido el par
`_GLOBAL_` al final.

---

## 5. La propuesta de configuracion (NO aplicada)

### 5.1 `config/GOWE69/splits.txt`

Fundir `asd2` en su unidad. Los 88 B de 0x80391518 son el final de la TU de
`gc_interface.cpp` --- van justo delante del `gcc2_compiled.` de `public.cpp` en
0x80391570 --- y sus entradas de `.ctors`/`.dtors` ya caen en la posicion que le
toca a `gc_interface.o` por orden de enlace (0x803C8C18 va entre `hd_device` y
`eathread_thread`; `gc_interface.o` es el objeto 522 y `eathread_thread.o` el
606).

Sustituir estos dos bloques:

```text
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp:
	.text       start:0x8038FD10 end:0x80391518
	.rodata     start:0x80414530 end:0x80414790
	.bss        start:0x804D8DAC end:0x804D95E8
	.sdata      start:0x804FF68C end:0x804FF6A8
	.sbss       start:0x804FFD58 end:0x804FFD5C

asd2:
	.text       start:0x80391518 end:0x80391570
	.ctors      start:0x803C8C18 end:0x803C8C1C
	.dtors      start:0x803C8C58 end:0x803C8C5C
```

por este:

```text
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp:
	.text       start:0x8038FD10 end:0x80391570
	.ctors      start:0x803C8C18 end:0x803C8C1C
	.dtors      start:0x803C8C58 end:0x803C8C5C
	.rodata     start:0x80414530 end:0x80414790
	.bss        start:0x804D8DAC end:0x804D95E8
	.sdata      start:0x804FF68C end:0x804FF6A8
	.sbss       start:0x804FFD58 end:0x804FFD5C
```

No crea ni destruye comodines: `0x80391518` es exactamente el final del rango
anterior, y `.ctors`/`.dtors` solo cambian de duena. Validado con una copia de
`checksplits.py` apuntando al fichero fusionado
(`scratchpad/r46_linked/splits_merged.txt`): **SOLAPES 0, RANGOS QUE CORTAN UN
SIMBOLO 0, LIMPIO**.

### 5.2 `configure.py`

1. `Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp`:
   `NonMatching` -> **`Matching`** (linea ~1336).
2. Borrar las entradas de `main/asd2` de `OBJDIFF_SHARED_SOURCES` y
   `OBJDIFF_SHARED_SOURCE_MAPPINGS` (~2313-2323): esa unidad deja de existir.
   Sus mapeos de `_GLOBAL_.I./.D.` ya no hacen falta porque **nuestros simbolos
   se llaman igual que los del objetivo**.
3. Conservar `OBJDIFF_SYMBOL_MAPPINGS["main/.../gc_interface"]` con
   `__static_initialization_and_destruction_0_803906C0` ->
   `__static_initialization_and_destruction_0`: el troceador seguira poniendo el
   sufijo y ese si hay que mapearlo.

### 5.3 Guion de la ventana

```bash
python scripts/checksplits.py
rm build/GOWE69/config.json && python -m ninja      # reextrae con la fusion
sha1sum build/GOWE69/main.dol                        # 9619ba57... con el objeto EXTRAIDO
python scripts/promote.py Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface
python scripts/trypromo.py Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface
# marcar Matching y reconfigurar
python configure.py && rm -f build/GOWE69/ok && python -m ninja
sha1sum build/GOWE69/main.dol
```

Tras la reextraccion, `promote.py` deberia quedarse con una sola pega
(`secciones que emitimos de mas: .data(4B)`), que es la variable estripada de la
seccion 1 y que el enlace ya ha demostrado inocua. Si molesta, quitar
`ROOT_DIRECTORY_NAME` de la fuente: el DOL sale igual y lo unico que se pierde
es el emparejamiento por nombre de los 88 B del par `_GLOBAL_`.

---

## 6. Ficheros tocados (sin comprometer)

Los cinco estan dentro de `src/Packages/realmemcard/`:

| fichero | cambio |
|---|---|
| `source/lib/gc/gc_interface.cpp` | cuatro guardas nuevas, `REALMC_LCFMT_BASE`, `IsBusy` movida delante de `GetBlockSize`, `TaskManager::UpdateCurrentTask` y `TaskTrc::Init` definidas detras de `EndTask`, y `ROOT_DIRECTORY_NAME` |
| `include/common/realmemcard/gc_blockcalculator.h` | `#pragma interface` bajo `#ifndef REALMC_BLOCKCALC_NO_INTERFACE`; deja de incluir `gc_driver.h` y usa declaraciones adelantadas |
| `include/common/realmemcard/impl/gc_interface_impl.h` | `#pragma interface`/`implementation` acotados a `GCMessage` bajo `#ifdef REALMC_GCMSG_INTERFACE_ONLY` |
| `include/common/realmemcard/impl/memcard_interface_impl.h` | `TaskStack` y `TaskManager` delante de `struct Task` (+ `struct Task;` adelantada); cuerpos de `TaskTrc::Init` y `TaskManager::UpdateCurrentTask` bajo `#ifdef REALMC_TASK_INLINES_IN_TU` |
| `source/lib/gc/gc_blockcalculator.cpp` | `#pragma implementation` -> `#define REALMC_BLOCKCALC_NO_INTERFACE` + `#include gc_driver.h` |

**Radio comprobado.** `memcard_interface_impl.h` y `gc_blockcalculator.h` no
salen del paquete `realmemcard` (13 unidades lo incluyen, ninguna fuera).
Recompiladas las **15** unidades del paquete y comparado el sha1 de cada seccion
mas el orden de simbolos contra la foto previa: **solo cambian
`gc_interface.o` y `gc_blockcalculator.o`**, y este ultimo conserva sus tamanos
(364/96) mejorando el orden. `fncmp` de las 15 identico antes y despues (las
cuatro que tienen pegas --- `gc_driver`, `gc_memcard_interface_impl`, `tasks`,
`trctasks` --- las tenian ya y sus objetos son byte a byte los de antes).

---

## 7. Segundo frente: `bigswizzler` NO se puede cobrar. La vecina de detras es el mismo

`claimlbl.py` sigue dando `egami/rcmp/dev/source/vd/gc/bigswizzler .rodata
0x8041010C..0x80410124  24 B  BYTES OK`, y la regla A lo veta por los 52 B de
hueco de detras. **Esos 52 B no son de una vecina: 48 son de `bigswizzler`
mismo.** Leidos del ELF original, el bloque 0x8041010C..0x80410158 es:

| direccion | contenido | ¿lo emitimos? |
|---|---|---|
| 0x8041010C | `"VD::tBigSwizzler\0"`, 17 B, relleno a 20 | si, `$LC0` |
| 0x80410120 | `0.0f` | si, `$LC1` |
| 0x80410124 | `0.0f` | **no** |
| 0x80410128 | `0x4330000000000000` --- la constante de conversion int->float del pool | **no** |
| 0x80410130 | `0.0625f` | **no** |
| 0x80410134 | dos palabras a cero | **no** |
| 0x8041013C | `"Assert: %s:%i %s (%s)\0"`, 22 B, relleno a 24 | **no** |
| 0x80410154 | `0.0f` | es de `rcmpbase` |

Nuestro `.rodata` mide **24 B** y el bloque real **72**. Las reubicaciones del
objeto extraido lo confirman: `NEW_tBigSwizzlerTexture` solo referencia
`lbl_8041010C` y `lbl_80410120+0`; los 48 B restantes viven dentro del simbolo
`lbl_80410120` (`size:0x34`) porque el troceador lo fundio, y nadie los
referencia por separado.

O sea que **no es un problema de atribucion de rango sino la veda estructural
del pool** (`ventana-pendiente.md` §4) mas la cadena de `assert` que `NDEBUG`
elimina. `bigyuvswizzler` tiene el mismo patron: 80 B nuestros contra 104 del
bloque 0x804100A4..0x8041010C, con la misma pareja
`0x4330000000000000` + `0.0625f` + `"Assert: %s:%i %s (%s)"` al final.

Y anadirlos como datos muertos **tampoco vale**: esta misma ronda ha medido que
el enlazador se lleva lo que no se referencia (es lo que hace con
`ROOT_DIRECTORY_NAME`), asi que los 48 B no llegarian al DOL.

### Lo que si es un hallazgo aprovechable: `lbl_80410154` es de `rcmpbase`

Los ultimos 4 B del hueco **si** tienen duena identificada: el objeto extraido de
`rcmpbase.cpp` referencia `lbl_80410154` desde `GetFrameRate__Q24RCMP7DECODER`
(offset 0x020A), y nuestro `rcmpbase.o` emite ahi un `$LC1` de 4 B con el mismo
valor. Su rango deberia empezar en **0x80410154**, no en 0x80410158:

```text
	.rodata     start:0x80410154 end:0x80410188      # 52 B en vez de 48
```

**No lo propongo para aplicar**: nuestro `rcmpbase.o` emite ademas un `$LC0` de
12 B muerto (`"RCMP::CHUNK"`) que el original no tiene, con lo que la seccion
mide 64 y no 52; y su hueco de detras (0x80410188..0x80410230, 168 B) lo veta
igual por la regla A. Esos 168 B son, con el mismo patron, los literales de
`rcmp_vp6_codec.cpp`, cuya seccion nuestra mide 392 B contra los 184 del
extraido. Las cuatro unidades de `egami/rcmp` de esta zona
(`bigyuvswizzler`, `bigswizzler`, `rcmpbase`, `rcmp_vp6_codec`) enlazan hoy
desde `obj\`, asi que **la cadena entera es un solo trabajo de fuente**, no de
ventana, y el primer paso es el pool.

---

## 8. Verificacion

```text
python scripts/fncmp.py .../gc_interface        0 de 31 funciones con el CODIGO distinto
python scripts/fncmp.py .../gc_blockcalculator  0 de 6  funciones con el CODIGO distinto
(las 15 unidades de realmemcard: identicas antes y despues)
python scripts/lcfix.py --check                 todas las entradas @lc estan al dia
python scripts/checksplits.py                   LIMPIO: se puede re-extraer
enlace de control                               DOL OK  9619ba57c9919f95f7f2ac951a2166a3517f91e3
enlace del racimo (sin asd2)                    DOL OK  9619ba57c9919f95f7f2ac951a2166a3517f91e3
```

Arnes y ensayos en `scratchpad/r46_linked/`: `shadowlink.py` (enlace en sombra
con `--drop`), `mide.py` (secciones y orden de simbolos de una unidad),
`snap.py` (foto y comparacion de las 15 unidades del paquete), `elfdelta.py`
(que simbolos se mueven entre dos ELF enlazados), `dolbytes.py`,
`splits_merged.txt` y `checksplits_merged.py`.
