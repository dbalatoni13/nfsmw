# r76 — El DOL había dejado de reproducirse: forense y reparación

`python -m ninja build/GOWE69/ok` fallaba: el enlace daba
`002bb893e9a47d8887fd4578ad557654601ee936` en vez de
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`. Después de esta ronda **el check
oficial pasa**. Aquí está la cronología, la causa raíz de cada defecto con su
medida, lo que se cambió y lo que queda pendiente.

---

## 1. Resumen

Había **tres** defectos independientes, no uno:

| # | Defecto | Efecto en el DOL | Entró en |
|---|---------|------------------|----------|
| A | `zFoundation` interna una **segunda copia de la cadena `"GAMECUBE"`** | `.rodata` **+4 B** desde `0x803EB4F0` (y **+8** desde `0x803EB578` por realineación) → 300 kB del DOL corridos | `1561d64a` (15-sep 12:12) |
| B | Seis unidades **despineadas en la r75** dejaron de reproducir su objeto | contenido de `.text` distinto (6 símbolos, 405 B) | `359b1d2b` (16-sep, r75) |
| C | Degradar `filesys.cpp` a NonMatching **estripa el `"bad_alloc"` muerto** de `0x80413A70` | `.rodata` **−8 B** desde `0x80413A70` | esta misma sesión, al reparar B |

El último commit con el DOL bueno es **`6be044ab`** (15-sep 09:45). El primero
que lo rompe es **`1561d64a`** (15-sep 12:12).

---

## 2. Método: cómo se localizó cada defecto

Las comparaciones a mano de `.o` no valen (el enlazador estripa lo no
referenciado y `dtk` pone a cero los campos reubicados). Las tres herramientas
que sí valieron están en `scratchpad/r76_dol/`:

* **`doldiff.py ORIG NUESTRO`** — compara los dos DOL por cabecera, por sección
  y por rangos de bytes. Lo primero que dijo es que **todas las secciones tenían
  el mismo tamaño y la misma dirección**: no era un problema de tamaño, era de
  contenido.
* **`shift.py [ELF]`** — el perfil de **desplazamiento** del enlace. Los símbolos
  que `dtk` genera al extraer llevan su dirección original en el nombre
  (`lbl_XXXXXXXX`, `pad_NN_XXXXXXXX_sec`, `gap_…`, `auto_…`); comparando la
  dirección enlazada con la del nombre sale el desplazamiento acumulado en cada
  punto, y **donde cambia está la unidad culpable**. Sobre el enlace roto:

  ```
    a partir de 80003328  delta    +0   (lbl_80003328 -> 80003328)
    a partir de 803EB518  delta    +4   (lbl_803EB518 -> 803EB51C)
    a partir de 803EBB48  delta    +8   (pad_05_803EBB48_rodata -> 803EBB50)
    a partir de 80413A7C  delta    +0   (lbl_80413A7C -> 80413A7C)
  ```

  Eso separa de un golpe **lo que está corrido** de **lo que está mal escrito**:
  los 137 kB de `.text` y los 161 kB de `.rodata` que marcaba `doldiff` eran casi
  todos operandos de dirección desplazados, no código distinto.
* **`ngcld -Map`** (en `relink.py`/`try.py`) — el mapa del enlazador da la
  dirección y el tamaño **por objeto**, que es lo que convierte un rango de
  direcciones en un nombre de fichero. No hacía falta ninguna heurística.

`try.py TAG +unidad -unidad` enlaza en un temporal sustituyendo unidades sueltas
por su objeto extraído (`-`) o por el nuestro (`+`), y da el sha1. Con eso cada
hipótesis se confirma o se cae en 20 segundos y **sin tocar `build/`**.

---

## 3. Defecto A — la segunda copia de `"GAMECUBE"` en zFoundation

### Evidencia

El primer byte distinto del `.rodata` está en `0x803EB4F0`, justo delante de la
cadena `"Out of Fastmem memory\n"` (`FastMem::Alloc`):

```
orig …3dcccccd 00000000 3f800000 00000000 | 4f757420 6f662046 …   "Out of F…"
nues …3dcccccd 00000000 3f800000 00000000 | 00000000 4f757420 …   +4 B de ceros
```

En nuestro objeto el hueco tiene nombre:

```
build/GOWE69/src/Speed/Indep/SourceLists/zFoundation.o  .rodata size=2344 (0x928)
  +0214 size=4   $LC156  3b360b61          <- pool de BuildRotate
  +0218 size=4   $LC157  00000000
  +021C size=4   $LC158  3f800000
  +0220 size=4   $LC159  40c90fdb
  +02C0 size=9   $LC160  47414d454355424500   <- "GAMECUBE", 9 B + 3 de relleno
  +02CC size=23  $LC161  "Out of Fastmem memory\n"
```

`$LC160` no lo referencia nadie, así que `-strip-unused-data` se lleva sus 9 B
declarados y **deja 4 B de relleno**: `.rodata` de la unidad 2328 → 2336 B
enlazados, y todo el resto del `.rodata` corrido. Los `43300000 80000000` de
`0x803EB578` piden alineación a 8, así que a partir de ahí el desfase pasa a +8.

`"GAMECUBE"` ya está a mano en la unidad, en `+0x00` del prefijo de bWare/STL
(`pad_05_803EB230_rodata`). La inline `bGetPlatformName()` de `bWare.hpp` la
interna otra vez, y el árbol ya tiene el interruptor para eso desde la r59:
`BWARE_PREFIX_GAMECUBE` (lo usan `zAI`, `zAnim`, `zFe`, `zFEng`, `zFeOverlay`,
`zBWare`). zFoundation no lo tenía **porque hasta el 15-sep ninguna de sus TU
veía `bWare.hpp`**.

### Causa raíz (en `src/`, y por qué)

El commit `1561d64a` (15-sep 12:12, «CONSOLIDADO agente OTRO-112») añadió a
`src/Speed/Indep/Libs/Support/Utility/FastMem.cpp`:

```c
#include "Speed/Indep/bWare/Inc/bWare.hpp" // r71f: C3861 bMemSet (compilacion por fichero X360)
```

Era un arreglo de **portabilidad X360**, correcto en sí mismo, pero arrastró la
inline. Sonda de causalidad de dos factores
(`scratchpad/r76_dol/probe_fastmem.py`, compila las cuatro combinaciones y
restaura el árbol):

```
include bWare.hpp=True   define GAMECUBE=True   ->  .rodata 2328 B   2a copia: False
include bWare.hpp=True   define GAMECUBE=False  ->  .rodata 2344 B   2a copia: True
include bWare.hpp=False  define GAMECUBE=True   ->  .rodata 2328 B   2a copia: False
include bWare.hpp=False  define GAMECUBE=False  ->  .rodata 2328 B   2a copia: False
```

### Arreglo

Una línea en `src/Speed/Indep/SourceLists/zFoundation.cpp`, delante de los
`#include` (mismo idioma que zAI/zAnim/zFe), que **conserva el arreglo de
portabilidad**:

```c
#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)
```

`.rodata` de zFoundation: 2344 → **2328 B**, exactamente el del objeto extraído.

---

## 4. Defecto B — las seis unidades despineadas de la r75

El commit `359b1d2b` («DEPINEO MASIVO r75») quitó pines `asm` de nueve ficheros.
Seis de ellos **son unidades `Matching` de biblioteca**, y sin los pines su
código dejó de reproducir el objeto original. Con el defecto A ya corregido, el
DOL quedaba con **18 rangos y 405 B reales** en `.text`, en seis símbolos:

```
   125 B  add_separators                            libc/vfprintf.c
   100 B  VP6_PredictFilteredBlock                  Packages/vp6/…/criticalpath.c
    77 B  PATHI_calcwaitbeat__FiiiP12PATHBEATINFO   path/…/pathnode.cpp
    63 B  VP6_DecodeBlock                           Packages/vp6/…/criticalpath.c
    20 B  _vfprintf_r                               libc/vfprintf.c
    20 B  _vfiprintf_r                              libc/vfprintf_1.c
```

(`vfprintf_1.c` es la segunda compilación de `vfprintf.c` con `INTEGER_ONLY`; hay
que degradar las **dos**.)

Los otros dos ya los había detectado el usuario antes de esta sesión:
`iSPCH_ChooseSamples` (spchpick.c, 412 → 408 B) y `FILEOPERATION::AddToQueue`
(filesys.cpp, 252 → 248 B).

### Arreglo — política del usuario, sin reintroducir pines

Las seis pasan a `NonMatching` en `configure.py`; el enlace usa el objeto
extraído y el DOL vuelve a salir bien. **No se ha vuelto a meter ni un `asm`.**

| Unidad | `.text` original | `fuzzy` hoy | `matched_code` hoy |
|---|---:|---:|---:|
| `libc/vfprintf.c` | 6.948 B | 99,66 % | 6,16 % |
| `libc/vfprintf_1.c` | 5.972 B | 99,85 % | 3,55 % |
| `Packages/vp6/…/criticalpath.c` | 12.040 B | 99,76 % | 54,58 % |
| `path/…/pathnode.cpp` | 4.204 B | 99,88 % | 92,01 % |
| `spch/…/spchpick.c` | 7.836 B | 99,78 % | 88,06 % |
| `realcore/…/filesys.cpp` | 10.872 B | 99,89 % | 97,68 % |
| **total** | **47.872 B** | | |

Ese es el coste en `linked` de esta ronda: **47.872 B y 6 unidades**. Es el
precio de haber cambiado código falso por código real, y es reversible en cuanto
alguna de las seis vuelva a cerrar sin andamios.

---

## 5. Defecto C — el `"bad_alloc"` que se estripa al degradar `filesys`

Al degradar `filesys.cpp` aparece un defecto **nuevo**, que no existía mientras
la unidad era `Matching`:

```
splits.txt:  filesys.cpp  .rodata  start:0x80413A70 end:0x80413CE8   (632 B)
mapa:        filesys.o    .rodata  80413A70  size=270 (624 B)        <- −8 B
symbols.txt: lbl_80413A70 = .rodata:0x80413A70; // size:0xA data:string  ("bad_alloc")
```

`lbl_80413A70` es una cadena **muerta**: el original la tiene, nadie la
referencia. Mientras compilábamos `filesys.cpp` la emitía nuestro objeto; con el
objeto extraído, `-strip-unused-data` se la lleva y el `.rodata` enlazado sale
8 B corto a partir de ahí.

### Arreglo (configuración)

Entrada nueva al final de `config/GOWE69/keep.lst`, con el mismo formato que las
que ya hay para objetos extraídos (`auto_05_80413ED0_rodata.o:lbl_80413ED0`):

```
filesys.o:lbl_80413A70
```

> **Regla general que deja esto:** degradar una unidad a `NonMatching` **no es
> gratis**. Si la unidad tenía datos muertos que sólo emitía nuestro objeto, hay
> que nombrarlos en `keep.lst` o el `.rodata` sale corto. Se detecta en un
> segundo con `shift.py`.

---

## 6. Lo que NO era la causa (descartado con medida)

* **`keep.lst`** (commit `55ec445f`). El diff sólo borra **líneas de comentario**
  `# @lc …`; ninguna entrada activa. Confirmado además enlazando con el
  `keep.lst` anterior: mismo hash.
* **`splits.txt`**. Su contenido es **idéntico** al de `HEAD` (commit `32a9f473`,
  14-sep 13:06); sólo cambió la fecha del fichero (re-extracción del 15-sep
  14:05, con los mismos rangos). `checksplits.py`: `0` solapes, `0` cortes,
  `LIMPIO`. Ninguna frontera cambió de dueño.
* **Los commits de datos** `b6a9f951` (zMisc/zAI) — sólo tocan `keep.lst` y
  tampoco entran aquí.
* **Los seis commits de despineo de hoy** (`bf3d0dc0`, `6ca1495a`, `79383ec6`,
  `41825f22`, `15441ee9`, `7d55fafb`). Todos tocan ficheros de
  `src/Speed/Indep/Src/…` que pertenecen a SourceLists **NonMatching**
  (zEagl4Anim, zGameplay, zAI, zPhysics, zEAXSound, zFe/zFeOverlay, zWorld): el
  enlace usa el objeto extraído y no los ve. **Verificado empíricamente**: el DOL
  sale exacto con el árbol tal y como está. **No hay que revocar ninguno.**
* **Las 99 unidades `Matching` sin objeto extraído.** Son todas del Dolphin SDK
  (`dolphinsdk/src/os`, `mtx`, `card`, `db`, `base`). No es una anomalía y no
  afectan al enlace.

### Por qué `report.json` no lo delató

Dos motivos distintos:

1. **Para el defecto A no podía verse**: zFoundation da `matched_code 100 %`
   porque el problema es un símbolo **de más** en nuestro objeto (`$LC160`) que
   el objetivo no tiene; objdiff compara los símbolos que están en los dos.
   (Su `matched_data` sí baja —hoy 17,09 %— pero baja también cuando la unidad es
   byte-perfecta en el enlace, así que no sirve de alarma.)
2. **Para el defecto B sí se veía**, y el informe que se consultó estaba
   **rancio**: recién regenerado, las cuatro unidades salen con
   `matched_code_percent` entre **3,55 % y 92,01 %**, no al 100 %.

**La conclusión operativa es que ni `report.json` ni los porcentajes son un
chequeo del DOL.** El único chequeo del DOL es el sha1.

---

## 7. Qué se cambió

| Fichero | Cambio |
|---|---|
| `src/Speed/Indep/SourceLists/zFoundation.cpp` | `#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)` + la nota del mecanismo |
| `config/GOWE69/keep.lst` | entrada nueva `filesys.o:lbl_80413A70` (+3 líneas de comentario) |
| `configure.py` | `Matching` → `NonMatching` en `libc/vfprintf.c`, `libc/vfprintf_1.c`, `Packages/vp6/1.0.6/source/decode/gc/criticalpath.c`, `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp` (más `spchpick.c` y `filesys.cpp`, que ya venían del usuario) |

Herramientas nuevas, reutilizables, en `scratchpad/r76_dol/`: `doldiff.py`,
`shift.py`, `attrib.py`, `maprange.py`, `relink.py`, `try.py`, `ins.py`,
`osyms.py`, `censo.py`, `probe_fastmem.py`.

`src/Speed/Indep/Src/Speech/EAXDispatch.cpp` y `…/RoadblockFlow.cpp` también
aparecen modificados en el árbol: **no son míos**, son de otro agente de despineo
que trabajó en paralelo durante esta sesión. Son de zSpeech (NonMatching) y el
DOL sale exacto con ellos puestos.

---

## 8. Verificación oficial

```
$ cd /c/Users/jferr/Desktop/nfsdecompiled
$ python configure.py && python -m ninja build/GOWE69/ok
[516/519] LINK build\GOWE69\main.elf
[517/519] DOL build\GOWE69\main.dol
[518/519] CHECK config\GOWE69\build.sha1
build/GOWE69/main.dol: OK
$ echo $?
0

$ sha1sum build/GOWE69/main.dol
9619ba57c9919f95f7f2ac951a2166a3517f91e3 *build/GOWE69/main.dol

$ cat config/GOWE69/build.sha1
9619ba57c9919f95f7f2ac951a2166a3517f91e3  build/GOWE69/main.dol
```

Chequeos laterales, los dos limpios:

```
$ python scripts/lcfix.py --check
todas las entradas @lc estan al dia (y 1 @lcsrc de fuente)

$ python scripts/checksplits.py
rangos en splits.txt: 1119
SOLAPES: 0
RANGOS QUE CORTAN UN SIMBOLO: 0
LIMPIO: se puede re-extraer.
```

---

## 9. Pendiente

1. **Commitear** (lo dejo sin commitear, como se pidió): `configure.py`,
   `config/GOWE69/keep.lst`, `src/Speed/Indep/SourceLists/zFoundation.cpp` y este
   documento. Los ficheros de Speech son del otro agente y van en su commit.
2. **Recuperar las seis unidades degradadas** (47.872 B de `linked`) escribiendo
   la forma de fuente correcta, sin pines. Las más cercanas por tamaño de hueco:
   `filesys.cpp` (una función, 97,68 % de código casado) y `pathnode.cpp`
   (`PATHI_calcwaitbeat`, 92,01 %). Las dos `vfprintf` son las más lejanas
   (18 pines quitados, `matched_code` del 3–6 %).
3. **Meter el chequeo del sha1 en la cadencia de la ronda.** Los informes de la
   r75 decían «main.dol compila y enlaza» y «GC intacto»: eso eran porcentajes.
   Entre el 15-sep 12:12 y hoy nadie miró el hash, y se acumularon tres defectos
   de tres orígenes distintos. La comprobación cuesta segundos:
   `python -m ninja build/GOWE69/ok`.
4. **Regla para el despineo**: antes de quitar un pin de una unidad `Matching` de
   biblioteca, hay que decidir si se acepta degradarla. Y al degradar, correr
   `shift.py` para ver si se ha estripado algún dato muerto que haya que meter en
   `keep.lst`.
