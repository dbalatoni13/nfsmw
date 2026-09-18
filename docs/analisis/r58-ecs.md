# r58 — zEcstasy: las nueve secciones EXACTAS, y el DOL cae de 410.239 a 14.323 B

Agente `ecs`. Unidad en propiedad: **zEcstasy**. **Un solo fichero tocado**:
`src/Speed/Indep/SourceLists/zEcstasy.cpp` (los tres bloques `asm()`, nada de C++).
Ninguna cabecera, ningún `config/`, ningún `keep.lst`, ningún commit.
Herramienta nueva: `scripts/desplaza.py`.

**Resumen en cinco líneas.**
1. `-strip-unused-data` se lleva `size & ~(alineación−1)` de cada símbolo sin referencia,
   **con la alineación de la SECCIÓN**: 8 en `.rodata`, 4 en `.data`/`.bss`. Partiendo un
   `lbl_` del bloque `asm` en trozos de ≤7 B se elige **al byte** cuánto sobrevive, con el
   contenido correcto y en orden. Eso cerró `rodata −424` de un tirón.
2. `.data +64` y `.bss +32` **no eran contenido**: son relleno de alineación. Se pagan
   quitando **ceros** del bloque `asm`.
3. **`linkdelta` decía `IGUAL` con la unidad todavía 12 B corta en `.data` y 16 en `.bss`.**
   El arranque de `.data`/`.bss`/`.sdata`/`.sdata2` se alinea a 32 y el redondeo se come
   hasta 31 B en cada una. **La medida buena es la dirección de los símbolos de DETRÁS.**
4. Con eso el DOL pasa de **410.239 a 14.323 bytes distintos (−395.916)** y **el tamaño del
   DOL coincide al byte** con el original.
5. **Y esa trampa está viva en las otras tres unidades `IGUAL`**: medido, a `zAnim` le
   faltan **24 B** de `.rodata` y a `zMain` **136 B**. Ver §5 — es lo más rentable de este
   informe.

---

## 1. La medida, antes y después

| | ANTES (HEAD `e5683085`) | DESPUÉS |
|---|---|---|
| `fncmp` | 3 de 539, 4.384 B | **3 de 539, 4.384 B** (sin regresión) |
| `linkdelta` | `.text +0 rodata−424 data+64 bss+32` | **`.text +0 IGUAL`** |
| `desplaza` DETRÁS mal | (no existía) | **0 en las cuatro secciones** |
| `dolwhere` | **no ejecutable** («LAS SECCIONES NO COINCIDEN») | **21.138 B** (infla: cuenta rangos) |
| `trypromo` | DOL ROTO `92b47d3c939e` | DOL ROTO `5e2a71311179` |
| **bytes distintos del DOL** | **410.239** | **14.323** |
| tamaño del DOL | 4.541.536 (original 4.541.888) | **4.541.888 = 4.541.888** |
| `sha1` del `.o` | `b26ee6b814ddd2cb90fded22773770a4c0137b3a` | `9bceb0308380d0bc4599787bad8dea90a2e87cfe`, **tres compilaciones iguales** |

Desglose por sección del DOL:

| | antes | después |
|---|---:|---:|
| `text0` | 20 | **0** |
| `text1` | 71.716 | 6.567 |
| `text2` | 2.073 | 14 |
| `data2` (`.rodata`) | 168.452 | 5.715 |
| `data3` (`.data`) | 162.984 | 2.027 |
| `data4` (`.sdata`) | 2.339 | **0** |
| `data5` (`.sdata2`) | 2.655 | **0** |

Y edición a edición:

| edición | DOL distinto | delta |
|---|---:|---:|
| base r57 | 410.239 | |
| `.rodata` −424 → 0 (§2) | 173.555 | **−236.684** |
| `.data` −64 y `.bss` −32 (§3) | 117.179 | −56.376 |
| corrección a −52 / −16 (§4) | **14.323** | **−102.856** |

La unidad **no puede promocionar**: `epCalculate` y `eProject` siguen VETADAS con prueba y
no las he tocado.

---

## 2. LA PALANCA DE LA RONDA: el troceado a 7 bytes

El bloque `asm()` de `.rodata` de `zEcstasy.cpp` es copia literal de la `.rodata` del
objetivo, con los nombres reales (`lbl_<dirección>`, `gap_<dirección>_rodata`). Está casi
todo **muerto** —nadie lo referencia—, así que el enlazador se lo lleva… **pero no del todo**.

    ngcld -strip-unused-data quita de cada símbolo sin referencia
        size & ~(sh_addralign − 1)
    con la alineación de la SECCIÓN DEL OBJETO:  .rodata = 8,  .data = 4,  .bss = 4.

O sea: **un símbolo de ≤7 B en `.rodata` sobrevive ENTERO**. Y la cuenta se comprueba sin
enlazar, con `ngcld -report-unused`, que escribe `statcov.txt` con dos columnas —`Strip` y
`pad`— por símbolo: `pad = size & 7` es exactamente lo que queda. Casos del propio fichero:
`lbl_803DD7B4` size 7 → strip 0; `lbl_803DD800` size 14 → strip 8; `$LC890` size 52 → strip 48.

Con eso el déficit de 424 B se paga **eligiendo qué sobrevive**, y como el bloque es copia
literal, lo que sobrevive es además **el contenido correcto y en su sitio**:

| | bytes |
|---|---:|
| el prefijo `803DD658..803DD6D8`, que **no emitíamos**, en trozos de 7 | +128 |
| `lbl_803DD800`, `810`, `820`, `864`, `87C`, `89C`, `8FC`, `90C`, `928`, `930`, `950` troceados | +272 |
| los **24 primeros** bytes de `lbl_803DD958` (los otros 24 quedan como un símbolo de 24 → se estripa entero) | +24 |
| | **424 EXACTOS** |

Resultado: `linkdelta` da `rodata` **0** y la cabeza de la `.rodata` de zEcstasy **casa al
byte durante 792 B seguidos** (antes casaban 296 y encima desplazados 128).

**Y no hace falta `keep.lst`.** El troceado es independiente de él, lo cual importa porque
`keep.lst` está vetado para los agentes. (De paso: `keep.lst` ya tenía la entrada
`zEcstasy.o:lbl_803DD658` **muerta** desde hace rondas, esperando a un símbolo que nadie
emitía. Ahora el símbolo existe; la entrada sobra pero no molesta.)

---

## 3. `.data +64` y `.bss +32` NO eran contenido

Emparejando los símbolos de los dos objetos por nombre base (quitando el sufijo `.NNNNN`
de los estáticos, que renumera):

* **`.data`**: nos sobran 13 símbolos que el objetivo no tiene (`LFrot`, `Rotty`, `LFangle`,
  `HeadlightClipTextureTestData`, `SmearBiasBASE/NOS`, `PrintSolidViewLocalWorldCached`,
  `cBfR`, `pPrevLightContext`, `ScreenTintScale`, `KColorWorldReflection`,
  `TestMWDesaturation/ColourTint`) y **12 de los 13 ya los estripa el enlazador**. El saldo
  de CONTENIDO es **+1 byte**. Lo que sobra de verdad es **relleno**: nuestra `.data` tiene
  **25 huecos de alineación, 54 B**, contra **1 hueco de 3 B** del objetivo.
* **`.bss`**: nos sobran 26 símbolos (828 B) y el enlazador estripa 824.

Como el exceso es relleno y el relleno son ceros, se paga quitando **ceros del bloque `asm`**,
que no cuesta un byte de información:

* `gap_06_8041B530_data` de `0x9C` (156 B de ceros) a `0x68` (104) → `.data` −52.
* fuera `gap_07_80467048_bss` (16 B de `.space`) → `.bss` −16.

---

## 4. LA TRAMPA QUE COSTÓ UNA VUELTA: `IGUAL` REDONDEA A 32

Quité 64 de `.data` y 32 de `.bss` porque eso decía `linkdelta`. `linkdelta` dijo entonces
**`IGUAL` en las nueve secciones**… y el DOL seguía con 117.179 bytes distintos.

La causa está en `ldscript.ld`: el arranque de `.data`, `.bss`, `.sdata` y `.sdata2` va con
`ALIGN(32)`, así que **el tamaño de sección que compara `linkdelta` redondea**. Con la unidad
12 B corta en `.data` y 16 en `.bss`, las nueve secciones daban delta 0 **y los símbolos de
zFe en adelante estaban 12 bytes por debajo de su dirección**.

Lo correcto era **−52** y **−16**, no −64 y −32. Corregido: el DOL bajó otros **102.856 B**.

> **`IGUAL` no significa que el tamaño esté bien. Significa que está bien ±31 por sección.**
> Y esos ±31 desplazan TODO lo que va detrás de la unidad en el enlace.

### La herramienta: `scripts/desplaza.py`

Enlaza dos veces —la base y la base con tu `.o`— y empareja los símbolos por nombre:

* **`DETRÁS`** = símbolos de **otras** unidades que cambian de dirección. **Tiene que ser 0.**
  Si no lo es, el desplazamiento que imprime es *exactamente* los bytes que te sobran o te
  faltan, y `linkdelta` no te los enseña.
* **`DENTRO`** = símbolos de la unidad en otra dirección: eso es ORDEN, y sale con `--detalle`.

```
    python scripts/desplaza.py zEcstasy
      seccion     DENTRO      mal |   DETRAS      mal
      .rodata         28        7 |    12446        0
      .data          325      317 |     2461        0
      .bss           152      151 |     1610        0
      .text          538       12 |    17479        0
      VEREDICTO: el tamano es EXACTO; lo que queda es ORDEN
```

---

## 5. LO QUE MÁS VALE DE ESTE INFORME: las otras tres `IGUAL` NO lo están

Pasé `desplaza.py` a las tres unidades que el encargo daba por `IGUAL` en las nueve
secciones. **Ninguna lo está de verdad**, y en dos el diagnóstico es de tres líneas:

| unidad | `.o` medido | qué dice `desplaza` | lectura |
|---|---|---|---|
| **`zAnim`** | `6317592ee8b9…` | `.rodata` DETRÁS: **11.403 símbolos ajenos a −24**, uniforme | le faltan **24 B de `.rodata`**. Nada más: `.data`, `.bss` y `.text` con DETRÁS 0 |
| **`zMain`** | `a3d57eb434a8…` | `.rodata` DETRÁS −136 (4.602 símbolos); `.data`/`.bss`/`.sdata`/`.sbss`/`.sdata2` −128 por arrastre | le faltan **136 B de `.rodata`**. Es la única causa: las demás secciones sólo heredan el `ALIGN(32)` |
| **`zPhysics`** | `b88fe7fed0b9…` | `.rodata` 143 símbolos movidos (+132/+64/+96/+56), `.data` 2 (±4), `.text` 355 con desplazamientos variados | **no** es un déficit uniforme: aquí hay cambio de dueño y/o orden entre objetos, no un simple «faltan N bytes» |

**Y la palanca de §2 sirve para las tres**: `zAnim` necesita 24 bytes, y eso son
**cuatro `lbl_` troceados a 7** en su bloque `asm` de `.rodata`, sin tocar `keep.lst` ni una
línea de C++. `zMain`, 136.

*Aviso de método*: esas tres medidas van contra el `.o` que había en `build/GOWE69/src/` en
mi ventana (sha1 en la tabla). Si su agente ha compilado desde entonces, hay que repetirlas
—cuestan dos enlaces.

---

## 6. Lo que le queda a zEcstasy: 14.323 B, y son ORDEN

`desplaza --detalle` los sitúa:

* **`.data`: 317 de 325 símbolos fuera de sitio** (2.027 B de DOL). La causa es una sola:
  los 59 símbolos del bloque `asm` de `.data` (28 `gap_06_*` + 31 `lbl_8041*`) se emiten
  **todos juntos al final** de nuestra `.data` en vez de intercalados. Se ve en el perfil:
  cada `gap_06_*` sale con desplazamiento **+1.000…+3.600** y los símbolos del compilador
  que deberían ir detrás de él salen con −100…−600, exactamente el tamaño acumulado de los
  huecos que se adelantaron.
  **Es arreglable**: una `.data` con inicializador la emite GCC 2.9 **en el punto del
  parseo**, así que un `asm(".section .data")` colocado entre dos definiciones de variable
  del `.cpp` correspondiente cae donde está escrito (comprobado: nuestro bloque, escrito en
  la línea 8696, cae detrás de la `.data` de los `#include` anteriores y delante de la de
  `EmitterSystem.cpp`). Son 59 fragmentos y la posición de cada uno se lee del objeto
  extraído: el símbolo que precede a la dirección del hueco.
* **`.bss`: 151 de 152 fuera de sitio**. Mismo patrón con los 8 `gap_07_*`. **Pero ojo**: la
  `.bss` del compilador se drena en `finish_file`, así que **no está comprobado** que un
  `asm(".section .bss")` intercalado sirva —nuestro bloque está escrito en la línea 9550, al
  final del fichero, y sus símbolos caen en el **offset 104** de la `.bss`, es decir *antes*
  de casi todo lo del compilador—. Hay que medirlo antes de invertir ahí.
* **`.rodata`: 5.715 B** (las 28 etiquetas con nombre: 7 mal). La cabeza casa 792 B y luego
  se rompe: los 5.443 B de literales del compilador salen **todos detrás** del bloque `asm`,
  y el objetivo los intercala. Se arregla partiendo el bloque `asm` en fragmentos entre los
  `#include` —los literales sí se emiten en el punto del parseo—, pero es trabajo de una
  ronda entera, no de una edición.
* **`.text`: 6.567 B**, de los que **4.384 son las tres funciones que no casan**
  (`epCalculateLocalDirectionalPOS16` 2.072, `UpdatePlatInfo` 2.044, `eProject` 268; las dos
  primeras VETADAS). El resto son reubicaciones a los símbolos de `.data`/`.bss`
  descolocados, o sea que **caen solas cuando caiga el orden de los datos**.

---

## 7. `Tweak_TuningAero` y `Tweak_UseTweakerTunings`: medido, y NO lo he hecho

Confirmado: los define `zMiscSmall.cpp` con un `asm()`, y `PVehicle.cpp:53-54` los declara
`extern` y los usa en `:2095` y `:2106`. Su sitio es la `.bss` de zEcstasy, `8045ED9C`
(el `float` primero) y `8045EDA0`.

**No lo he tocado, y con razón medida:**

1. La `.bss` de zEcstasy **ya está EXACTA** (`desplaza`: DETRÁS 0). Emitir esos 8 B la deja
   +8 y hay que quitar 8 en el mismo movimiento —trivial: `gap_07_8046BA0C_bss` es de 8—.
2. Pero el paquete **no es atómico dentro de mi unidad**: mientras `zMiscSmall.cpp` siga
   definiéndolos, el enlace queda con el símbolo duplicado. `ngcld` lo resuelve con un
   *warning* (`L0019:Symbol ... multiply defined`, ya pasa hoy con `PPCMtdec`) y se queda con
   el primero, así que el resultado depende del **orden de enlace** y, sobre todo, **le
   cambia el tamaño de `.bss` a zMiscSmall**, que es de otro agente (regla 4).

**Propuesta para el jefe**: encargo atómico de dos unidades — zEcstasy emite las dos
variables y recorta `gap_07_8046BA0C_bss` a 0; zMiscSmall borra las dos de su `asm()`. Con
`desplaza.py` en las dos antes y después se ve en dos enlaces si paga.

---

## 8. Trampas nuevas (todas con su caso)

* **`linkdelta`/`IGUAL` redondean a 32** (§4). Es la trampa cara de la ronda: 102.856 B de
  DOL escondidos detrás de un `IGUAL` en las nueve secciones.
* **La granularidad del estripado NO es 8: es la alineación de la sección.** La regla que
  circula («`size & ~7`») sólo vale en `.rodata`. En `.data` y `.bss` es `& ~3`, así que ahí
  **un `float` suelto de 4 B SÍ se estripa** (medido: `LFrot`, 4 B, `Strip 4`). Un `short` de
  2 B no.
* **`ngcld -report-unused` es la herramienta que faltaba**: escribe `statcov.txt` con
  `Strip`/`pad`/símbolo/sección/objeto para TODO el enlace. Con él se predice el resultado
  de una edición de datos **sin enlazar**. (`-M`/`-print-map` y `-sn-full-map`, en cambio,
  no producen nada en esta versión: probados, salida vacía.)
* **`dolwhere` no arranca si el tamaño de sección no coincide** («LAS SECCIONES NO
  COINCIDEN»), así que en una unidad con déficit de datos no hay «antes» con el que comparar.
  El «antes» hay que sacarlo con `agent_ecs_doldiff.py`.
* **Emparejar símbolos por nombre exige quitar el sufijo `.NNNNN`** de los estáticos: el
  objetivo numera en los 31.000-33.000 y nosotros en los 22.000-25.000. Sin eso salen 55
  «símbolos que sobran» donde de verdad hay 13.

## 9. Estado de coordinación

* **`lcfix.py --check`: 105 correcciones pendientes, y NINGUNA es de zEcstasy** (mis
  ediciones no añaden ni mueven literales del compilador, sólo `asm`, así que la numeración
  `$LC` de la unidad no se toca). Vienen de otros agentes/rondas; las aplica el jefe.
* No he tocado `config/GOWE69/*`, `splits.txt` ni `keep.lst`.
* `zEcstasy.cpp` no lo incluye ninguna otra SourceList: **cero regresiones posibles en
  unidades ajenas**, y ninguna cabecera modificada.
* Fichero en CRLF puro comprobado (10.106 CRLF / 10.106 LF / 0 CR sueltos).
