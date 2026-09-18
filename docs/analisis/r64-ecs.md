# r64 -- agente `ecs` (zEcstasy, zEAXSound, zEAXSound2)

## Resumen

**-9.518 B de DOL**, en dos unidades, con `linkdelta` en `IGUAL` en las tres y
sin tocar una sola instruccion de codigo.

| unidad | DOL antes | DOL despues | delta | `linkdelta` | `fncmp` |
|---|---:|---:|---:|---|---|
| **zEcstasy** | 9.449 | **3.424** | **-6.025** | `.text +0  IGUAL` | 3 de 539 (igual) |
| **zEAXSound2** | 37.784 | **34.291** | **-3.493** | `.text +0  IGUAL` | 1 de 930 (igual) |
| zEAXSound | 31.705 | 31.705 | 0 | `.text +0  IGUAL` | 0 de 773 (igual) |

Las tres cifras son `dolwhere` con el mismo banco y **con el control de base
delante y detras**: el enlace sin sustituir nada reproduce
`9619ba57c9919f95f7f2ac951a2166a3517f91e3` byte a byte, antes y despues de la
tanda de medidas.

`sha1` de los `.o`, **tres compilaciones consecutivas iguales** cada uno:

* `zEcstasy.o`   `ad85861fa95e11866ec8631a3c668d3da92ce516`
* `zEAXSound.o`  `be77f33b45332685312244980cf2f50b9ffa4d8a` (solo comentarios)
* `zEAXSound2.o` `dd212a0c0a07a791f6c54c83bbef0bc66375f51c`

**Correcciones de `lcfix` pendientes: CERO** (contadas con una copia de
`keep.lst` en el scratchpad, sin correr `lcfix.py` sobre el arbol; `keepchk.py`
da **0 entradas rancias** de las tres unidades). Cero propuestas para
`config/`, `splits.txt`, `symbols.txt` o `configure.py`.

---

## 0. La correccion al encargo, y es la que manda

El encargo daba por bueno el plan que la r60/r62/r63 dejaron escrito para la
`.rodata` de zEcstasy: *"el pool que emite cc1plus ya sale en el orden del
original (4 inversiones de 551) y lo unico descolocado es el bloque `asm()`;
partirlo en fragmentos y colocarlos"*.

**Las 4 inversiones son falsas, y el plan tal cual estaba escrito ES UNA
PERDIDA.** Medido con el simulador de esta ronda:

| variante | bytes de `.rodata` en su sitio |
|---|---:|
| lo que habia (bloque entero al frente) | 2.255 de 7.952 |
| el plan de la r63 (los 111 huecos, cada uno delante de "su" funcion) | **1.481** |
| lo aplicado esta ronda | **6.437** |

Las **4 inversiones de 551** salian de contar SOLO los 552 `$LC` que `lcmap`
empareja por reubicacion. Los otros 32 --que la r62 ya sabia que existian y
mapeaba por contenido-- son justo los que estan mal: **17 cadenas de AttribSys y
SMS (361 B) que emitimos 1,4 kB antes de su sitio**. Contando los 590 items
completos hay **12 inversiones**, no 4, y son las que fijan el techo.

La cuenta buena de la `.rodata` de zEcstasy, cuadrada al byte contra el ENLACE
(no contra el objeto):

```
ventana 803DD658..803DF568 = 7.952 B
  $LC del compilador ........ 4.419 B  (590 items, TODOS alineados a 4)
  simbolos de la fuente .....   116 B
  del bloque asm ............ 3.417 B  en 128 huecos   <- lo que hay que colocar
```

---

## 1. Lo que se ha hecho en zEcstasy: el bloque `asm()` REPARTIDO en 34 trozos

El bloque de `.rodata` de `zEcstasy.cpp` (9.086 lineas, 684 simbolos, 7.717 B
emitidos de los que sobrevivian 3.417) **ya no existe**. En su sitio hay 34
fragmentos colocados delante de la funcion cuyo pool de constantes los sigue en
el original, mas un marcador de alineacion de cero bytes.

### 1.1 Las cuatro cosas que hay que saber para repetirlo

**(a) GCC 2.9 emite el POOL DE CONSTANTES ANTES del cuerpo de la funcion.**
No despues. Se ve en 1 s con `ngccc -S` sobre tres funciones:

```
.section ".rodata" / .align 2 / $LC1: .long 0x3fa00000     <- el pool
.section ".text"   / .globl f1 / f1: ...                   <- el cuerpo
```

Por eso un `asm()` de fichero puesto **delante de la definicion de F** cae justo
delante del primer `$LC` del pool de F, que es donde hace falta.

**(b) `.previous`, no `.section ".text"`.** La regla de la r59 (`assemble_asm`
no toca el `in_section` de GCC, asi que hay que restaurar la seccion a mano)
sigue siendo cierta, pero `.section ".text"` **solo vale si GCC estaba en
`.text`**. Si el fragmento cae detras de una variable de `.sdata`, deja el
ensamblador y GCC desincronizados. `.previous` es agnostico y esta probado:
con `int gvar; asm(...); int gvar2;` la variable de detras vuelve sola a
`.sdata`.

**(c) Trozos de <=7 B.** `-strip-unused-data` se lleva `size & ~7` de cada
simbolo sin referencia, o sea CERO de un simbolo de 7 B o menos. Con eso NINGUN
fragmento necesita entrar en `keep.lst`.
**Y por eso hace falta el `.balign 8` del frente**: fija la alineacion de la
SECCION `.rodata` del objeto. Si baja a 4, el estripado pasa a `size & ~3` y
cada trozo de 7 B pierde 4 bytes. El marcador emite cero bytes (va en el
desplazamiento 0) y esta puesto a proposito.

**(d) Los 7 `lbl_` que la fuente referencia y las 16 lineas de `keep.lst`.**
`EcstasyE.cpp` declara `extern const char lbl_803DDF9C[]` y seis mas para
`EnvmapTargetNames`; y `keep.lst` nombra 16 `lbl_` de `.rodata`. El troceado
FUERZA un corte en esas 23 direcciones, asi que los simbolos siguen existiendo
con su nombre. Resultado: **0 entradas rancias** (`keepchk`). Sin ese cuidado el
enlace falla con `L0039: Reference to undefined symbol lbl_803DDF9C`.

### 1.2 Como se decidio el sitio de cada fragmento: NO se adivino

Suponer "el `$LC` que sigue al hueco lo emite la primera funcion que lo
referencia" **es falso** y cuesta la ronda: `$LC57`/`$LC58`/`$LC59` (el prefijo
de bWare) salen en el byte 0 de `.rodata`, mucho antes de la funcion que los
usa, porque los emite el preambulo del fichero.

El sitio se **midio**, con una sola compilacion:

> se mete un simbolo MARCADOR DE CERO BYTES (`.globl mrk_NNNN` / `mrk_NNNN:` /
> `.size mrk_NNNN, 0`) delante de cada uno de los 125 puntos candidatos, se
> compila una vez y se lee del `.o` el desplazamiento de `.rodata` en que cayo
> cada marcador.

Eso da el orden REAL de (puntos de insercion, items del compilador) sin modelar
nada. Un dato que salio de ahi y que no estaba en ningun informe: **de los 590
items de `.rodata`, 71 se emiten DESPUES del final de `zEcstasy.cpp`** --son la
cola de `finish_file`--, asi que ningun `asm()` de fichero puede ponerse delante
de ellos. El ultimo punto legal es el indice 519.

### 1.3 Y luego se AJUSTO por medida, no por modelo

Modelar el enlazador no sale: entre un fragmento y el `$LC` que le sigue el
enlace mete (i) relleno de `.align 2` o `.align 3` --51 de los 590 items son
`double` y alinean a 8, y eso NO estaba en el modelo-- y (ii) los restos
`size & 7` de los `$LC` MUERTOS, que ocupan sitio y son invisibles en la tabla
de simbolos del ELF enlazado. Dos modelos sucesivos fallaron por +232 y +144 B.

Lo que si funciona (`scratchpad/ecs64/iter.py`), y converge en 4 vueltas:

1. enlazar y leer **la direccion REAL de cada `$LC`** en el ELF enlazado;
2. `d = direccion_objetivo - direccion_real` para el item que sigue a cada
   fragmento;
3. corregir el tamano del fragmento **en DIFERENCIAL** (`adj = d - d_anterior`):
   arreglar un fragmento ya mueve todo lo que va detras, y aplicar los `d`
   crudos a la vez multiplica la correccion por el numero de fragmentos
   (probado: `rodata +272` y 1.248 B en su sitio);
4. el fragmento crece **por el FINAL** (`addr` se queda, `n` sube). Hacerlo al
   reves --anclar el final y crecer hacia atras-- topa con `addr < INI` en el
   fragmento del frente y anula la unica correccion que importaba.

**Trampa de nombres**: `$LC57` existe en TODOS los objetos. Leer
`{nombre: direccion}` del ELF enlazado sin filtrar por la ventana de la unidad
da deltas de +93.000 B. Hay que acotar al rango de `splits.txt`.

### 1.4 La convergencia, vuelta a vuelta

| vuelta | `linkdelta` | bytes de `.rodata` en su sitio | items en su direccion |
|---|---|---:|---:|
| base (lo que habia) | IGUAL | 2.255 / 7.952 | 83 / 590 |
| 1 | `rodata -8` | 1.918 | 83 |
| 2 | `rodata -8` | 4.514 | 204 |
| 3 | IGUAL | 2.507 | 223 |
| 4 | `rodata +8` | 5.930 | 286 |
| **5 (aplicada)** | **IGUAL** | **6.437** | **440** |
| 6 | -- | 0 correcciones propuestas: **punto fijo** | |

Se probo ademas una variante que ajusta cada fragmento al desplazamiento MAS
FRECUENTE de su tramo en vez de al de su primer item: da `rodata +8` y 3.940 B.
**Peor**; revertida.

### 1.5 El techo, y por que son 1.515 B y no cero

Lo que queda mal en la `.rodata` de zEcstasy son **inversiones del propio
compilador**, que ningun `asm()` puede arreglar:

| item | destino | lo emitimos en | desfase |
|---|---|---|---:|
| `$LC158` `Attrib::Gen::pvehicle` | `803DDE30` | `803DD770` | +1.720 |
| `$LC219` `Attrib::Gen::engine` | `803DDE48` | `803DD7C0` | +1.664 |
| `$LC245..248` (aivehicle, pursuit*) | `803DDDC0..` | `803DD820..` | +1.400 |
| `$LC337..340` `SMS_MESSAGE_%d...` | `803DDED0..` | `803DD890..` | +1.592 |
| `$LC596` | `803DDF6C` | `803DE2FC` | -920 |
| `$LC889` `Attrib::Gen::visuallook` | `803DDF38` | `803DF1A8` | -4.728 |

Son 17 items y 361 B mal colocados que arrastran ~1,4 kB de vecindad. La
palanca es el orden de parseo (el `#include` que los introduce), y **eso
renumera `$LC`**: es el mismo negativo que la r62 documento en zEAXSound.
Mientras no haya una ventana en exclusiva con `lcfix` en la misma mano, este es
el techo.

El reparto de los 3.424 B que quedan de DOL:

```
  902 B  803DD6DC  la cabeza, por las 17 inversiones de arriba
  400 B  _type_map              (la cola de finish_file, r63 seccion 5)
 ~700 B  epCalculateLocalDirectionalPOS16   (funcion abierta)
  321 B  $LC546 / 282 B $LC821 / 89 B $LC596  (mas orden de .rodata)
 ~140 B  UpdatePlatInfo + eProject          (funciones abiertas)
```

---

## 2. zEAXSound2: la misma enfermedad, en `.data`, y -3.493 B

`rodmap.py` (herramienta nueva, ver seccion 5) sobre la `.data` de zEAXSound2
daba una **escalera monotona** de 15 tramos:

```
80417E2C..80417E44   -132        80418198..80418286   -188
80417E4C..80417E84   -140        804182A4..8041833F   -208
80417EA8..80417EC9   -168        8041834C..804183DC   -212
...                              804183E0..80418802   -216
804180E0..8041817E   -184        80418980..8041A570   -280   <- 7.152 B seguidos
```

Una escalera monotona con un escalon inicial de exactamente **-132** y un bloque
`asm()` cuyo primer trozo es `pad_06_80417DA8_data` de exactamente **132 B**: el
bloque de `.data` esta DETRAS de los 68 `#include`, o sea al final de la `.data`,
y el original lo tiene intercalado. **Es el caso de zEcstasy, en otra seccion.**

Aplicado: los 15 trozos (300 B) repartidos entre los `#include`, con el sitio de
cada uno calculado con los desplazamientos MEDIDOS de 69 marcadores de `.data`
de cero bytes (una sola compilacion).

| | antes | despues |
|---|---:|---:|
| `.data` en su sitio | 4.524 / 10.236 | **6.491 / 10.236** |
| tramos de desplazamiento | 15 | **14** (y el mayor pasa de -280 a **-32**) |
| DOL | 37.784 | **34.291** |
| `linkdelta` | IGUAL | IGUAL |

**Y esto refuta a la r62 seccion 5.** Aquella midio dos recolocaciones de la
`.data` de zEAXSound2 (cada simbolo en su offset exacto; los 15 andamios
clavados) y las dos salieron **ocho y nueve veces peores**, y concluyo que *"no
se puede evaluar el orden de la `.data` sin mantener el tamano al byte, y eso
exige resolver antes los 19 simbolos que emitimos y el objetivo no"*. No hace
falta: **moviendo el bloque desde la FUENTE el tamano no se toca** (`linkdelta`
sigue en `IGUAL`) porque no se anade ni se quita un byte, solo cambia el punto
de emision. Lo que rompia era parchear el `.o`, no el orden.

### 2.1 El techo de zEAXSound2, medido

Quedan **-32 B** sobre un tramo de 7.204 B (el 70 % de la ventana). Para
cerrarlo los tres ultimos trozos (`gap_06_80418950` 24 B, `lbl_80418978` y
`lbl_8041897C` 4 B) tienen que caer en los desplazamientos de compilador **2.768
y 2.784**, y ahi dentro solo hay estaticos `_4Csis.<algo>Id` de **8 B**
alineados en 2.748+8k: **2.768 no es frontera de simbolo**. O sea que esos 4 B
de desfase no son de colocacion sino de que uno de nuestros estaticos no esta
donde el objetivo lo tiene. Quien lo retome: son 7.204 B a cambio de encontrar
ese estatico, y el observable es `rodmap.py zEAXSound2 .data`.

---

## 3. zEAXSound: NO la he tocado, y traigo el NEGATIVO que la r62 dejo abierto

La r62 seccion 9.3 cerraba pidiendo una ronda en exclusiva: *"los `#include`
tempranos de AttribSys mueven el orden de la `.rodata` desde el SourceList; con
`lcfix` en la misma mano, el objetivo son los 8.651 B de `.rodata` mas la parte
del `.text` que cuelga. Sin `lcfix` en la misma mano, no."*

**Lo he medido entero, con `lcfix` incluido y SIN tocar el arbol**, con una copia
de `keep.lst` en el scratchpad (`keepfix.py` + `keepprobe.py`):

| variante | `keep.lst` | secciones | DOL |
|---|---|---|---:|
| base (control) | repo | -- | **0 B** (`9619ba57c991`) |
| hoy | repo | exactas | **23.309 B** |
| + simsurface/ecar/camerainfo/effects/audioimpact/audioscrape arriba | repo (rancio) | **ROTAS** (`rodata -656`) | -- |
| + esos `#include` **y las 56 entradas de `keep.lst` re-resueltas** | copia | **exactas** | **23.258 B** |

**La ganancia real es de 51 bytes.** No 230 (la cifra de la r62 estaba tapando
la perdida con 656 B de ceros de compensacion, no midiendo la permutacion). Y
no hace falta relleno de compensacion: con `keep.lst` al dia las secciones
cuadran solas.

**51 B a cambio de 56 entradas de `keep.lst` renumeradas y una corrida de
`lcfix` en mitad de la ventana: NO COMPENSA, y ya no es "esta por medir".**
El eje de los `#include` de AttribSys en zEAXSound **esta cerrado**.

Lo que si es cierto es que su `.rodata` esta muy permutada, y ahora se sabe de
que forma. `rodmap.py zEAXSound`:

```
4.237 de 12.824 B en su sitio ; 73 bloques comunes de >=8 B (7.521 B)
60 tramos de desplazamiento constante: -1112, +16, -1004, +336, -104, -752,
-1096, +104, -1036, -1128, +32, +444, +1456, ...
```

**60 desplazamientos distintos y ninguno dominante**: no es un bloque en el
sitio equivocado (como zEcstasy o la `.data` de zEAXSound2), es el pool del
compilador genuinamente barajado. La palanca de esta unidad no es un `asm()`:
es el orden de emision de decenas de funciones. Anotado en `zEAXSound.cpp`.

---

## 4. Regresiones y comprobaciones

* `fncmp`: `zEcstasy` **3 de 539** (4.384 B) antes y despues; `zEAXSound`
  **0 de 773**; `zEAXSound2` **1 de 930** (`GenerateRoadNoise`, 17 insn). Sin
  cambio en ninguna.
* `textorder zEcstasy`: **6 descolocadas de 540, 5 saltos** antes y despues.
  Sin cambio (los seis siguen siendo la cola de `wrapup_global_declarations`).
* `linkdelta`: las tres en `.text +0  IGUAL`.
* `keepchk.py`: **0 entradas rancias** de `zEcstasy.o`, `zEAXSound.o` y
  `zEAXSound2.o` (las 81 del arbol son de otras unidades).
* `checksplits.py`: `0 solapes, 0 rangos que cortan un simbolo, LIMPIO`.
* `prefijochk.py`: `LIMPIO`.
* `gapchk.py` canta `zEcstasy 459 muerto(s) sin keep.lst, 3.070 B en juego`.
  **Es un falso positivo por diseno**: los 459 son los trozos de <=7 B, que
  `-strip-unused-data` no toca (`size & ~7 == 0`). La prueba es el enlace:
  `linkdelta` en `IGUAL` y 6.437 B de `.rodata` en su direccion exacta.
* Sellado (regla 6): tres compilaciones consecutivas identicas de cada `.o`.
* Control de base **delante y detras** de la tanda final de medidas:
  `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 0 bytes distintos.
* **Cero `asm` de andamio nuevos** (barreras, pines, `asm{}`): los 34+15
  fragmentos son DATO, no barreras. El censo de `pines.py` no se mueve.

### 4.1 Dos avisos de banco que costaron tiempo

**`import lcfix` EJECUTA `lcfix.py`.** El modulo lleva `main()` en el nivel
superior, asi que importarlo para reutilizar una funcion **reescribe
`config/GOWE69/keep.lst`** (me lo hizo dos veces; revertido las dos con
`git checkout`, comprobado con `git diff --stat`). Si alguien necesita
`rodata_y_simbolos`, que la copie. Merece una linea en `docs/TRAMPAS.md`.

**El enlace base se rompe SOLO, en mitad de la ventana.** Entre dos medidas mias
el control de base paso de 0 B a 10 B distintos y de vuelta a 0: son los demas
agentes recompilando objetos de unidades YA PROMOCIONADAS (`sn_malloc.o`,
`LibSN/sndvd.o`, `spchpick.o`, `libc/*.o`, con `mtime` de esos minutos), que
`objetos_del_enlace()` toma de `build/GOWE69/src`. Durante ese rato una medida
de `dolwhere` de zEAXSound2 dio **656.498 B** por un desplazamiento de 8 B en una
seccion que no era mia. **Toda medida de DOL en una ventana compartida necesita
el control de base delante Y detras**; sin el, un numero puede ser 20 veces el
real y no hay forma de saberlo.

---

## 5. Banco (en `scratchpad/ecs64/`, solo `.py` y `.json` pequenos)

Cinco de estos son genericos y merecen subir a `scripts/` si el jefe quiere:

* **`rodmap.py <unidad> [.rodata|.data]`** -- el mapa de DESPLAZAMIENTO de una
  seccion enlazada, en TRAMOS. Es lo que dice en 30 s si el desorden de una
  unidad es un bloque en el sitio equivocado (pocos tramos, escalera monotona)
  o un pool barajado (decenas de desplazamientos). Con el se encontro el caso de
  zEAXSound2 y se cerro el de zEAXSound.
* **`marks.py`** -- mete un simbolo marcador de CERO bytes en cada punto
  candidato y, con UNA compilacion, da el desplazamiento de seccion de cada uno.
  Es la unica forma fiable de saber donde cae un `asm()` de fichero.
* **`iter.py`** -- el ajuste por medida de la seccion 1.3 (diferencial, leyendo
  el ELF enlazado).
* **`keepprobe.py`** -- enlaza con una unidad sustituida y un `keep.lst`
  ALTERNATIVO, y trae el control de base.
* **`keepfix.py`** -- `lcfix` que escribe una COPIA. Para medir cuanto valdria
  una ronda con `lcfix` sin envenenar la ventana.

Y los especificos: `gaps.py`, `cover2.py`, `surv.py`, `perm.py`, `order.py`,
`plan5.py`, `calib.py`, `gen2.py`, `loc4.py`, `parse.py`, `link.py`, `loop.sh`
(zEcstasy) y `eax2mark.py`, `eax2gen.py` (zEAXSound2). Los volcados (`.bin`,
`.elf`, `.dol`, `.s`, las copias de seguridad) estan borrados.

---

## 6. Lo que dejo para la ronda siguiente

1. **zEcstasy: el techo es el orden del pool, no el bloque.** 902 B de la cabeza
   y ~400 B mas cuelgan de 17 cadenas de AttribSys que emitimos 1,4 kB pronto.
   Misma familia que zEAXSound, misma palanca (`#include`), mismo precio
   (`lcfix`). Ahora se sabe cuanto vale: en zEAXSound, 51 B. **Antes de gastar
   una ronda en zEcstasy, medirlo con `keepfix.py`/`keepprobe.py` igual que
   aqui**: son 20 minutos y evita una ronda entera.
2. **zEAXSound2: 7.204 B a un solo estatico de distancia.** Ver 2.1.
3. **La receta del bloque repartido vale para cualquier unidad** cuyo `rodmap`
   de una escalera monotona. Las candidatas se ven en un minuto: `rodmap.py` de
   `.rodata` y `.data` de las 20 unidades que quedan.
4. **zEcstasy sigue sin promocionar**: `epCalculateLocalDirectionalPOS16`
   (2.072 B), `UpdatePlatInfo` (2.044 B) y `eProject` (268 B). No las he tocado;
   los negativos de la r63 sobre las tres siguen en pie y estan en el arbol
   donde `previo.py` los encuentra.
