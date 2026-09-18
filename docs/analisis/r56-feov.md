# r56 — agente `feov`: zFeOverlay y zAnim

Unidades: `Speed/Indep/SourceLists/{zFeOverlay,zAnim}`.
**Dos ediciones, las dos de `.bss`, las dos pagan. Ninguna promociona.**
Y un hallazgo que no es mio pero bloquea a toda la ronda: **`keep.lst` esta rancio en
10 unidades y cuesta 1.235 B de DOL solo en las dos mias** (§5).

    fncmp ANTES y DESPUES:  zFeOverlay 0 de 467 distintas, zAnim 0 de 315.  Cero regresiones.
    sha1 sellado en TRES compilaciones seguidas, identico las tres:
       zAnim.o      df394b1123330f2c0a664e1f7af185ece60fe1f5
       zFeOverlay.o 434f846e9640d90f4d17240e96ed88c934949a1c
    trypromo: DOL ROTO en las dos (0c94775e5355 / 6cf3b4282c52).

| unidad | linkdelta ANTES | linkdelta DESPUES |
|---|---|---|
| zFeOverlay | `.text +0  bss-4096  sbss+4  over-1288` | `.text +0  sbss+4  over-1288` |
| zAnim | `.text +0  rodata-1488  data-192` | igual — **y aun asi la edicion es correcta**, ver §2 |

El «antes» es el que da el ARBOL DE HOY recompilando la fuente de HEAD, no el del encargo:
con `keep.lst` corregido las dos filas son `over-1136` y `rodata-1336`, que son las cifras
del encargo. Los 152 B de diferencia son el `keep.lst` rancio, medido en §5.

**Bytes de DOL ganados: 378** (523.427 -> 523.049 con las dos unidades sustituidas y el
`keep.lst` corregido; 524.662 -> 524.279 con el `keep.lst` tal como esta hoy).

---

## 1. zFeOverlay: el hueco de `.bss` de 4.104 B — cerrado, `bss-4096` -> `0`

`zFeOverlay.cpp`, ultima linea:

    char _zFeOverlayBssTail[4104] asm("gap_07_804AB6B8_bss");

**La r55 lo dio por imposible por dos razones y las dos son falsas.**

1. Decia que hacia falta «una entrada en `keep.lst`». **Ya esta**: `keep.lst:809`
   `zFeOverlay.o:gap_07_804AB6B8_bss`. Lo mismo el de zAnim (`keep.lst:83`). Nadie lo
   habia mirado.
2. Decia «un estatico de ese tamano declarado el ultimo de la unidad». Lo es, pero **no
   vale un `asm()` de ambito de fichero**, que es lo que dice el brief para todo lo demas.

### La trampa, con la prueba en el `.s`

Lo probe primero con `asm(".section .bss ... .space 4104")` en la ultima linea del
`.cpp`. El hueco salio en el **offset 0x644**, o sea 4 kB ANTES de su sitio, delante de
`kFloatScaleUp`. Volcando el ensamblador (`ngccc -S`, 1,34 M de lineas):

    linea 1.284.928   el asm() de fichero            <- punto del parseo
    linea 1.308.214   .lcomm kFloatScaleUp,4,4       <- finish_file
    linea 1.339.705   .lcomm gPlayerNum,4,4          <- finish_file

**Las variables de `.bss` sin inicializador salen como `.lcomm` desde `finish_file`,
o sea DESPUES de cualquier `asm()` de fichero.** El «asm() se emite donde esta escrito»
del brief es cierto, y por eso mismo un `asm()` **no puede** ser el ultimo simbolo de
`.bss` de una unidad. La declaracion de C entra en la misma cola `.lcomm` y sale al
final. Patron ya usado en `bMemory.cpp:577` y nadie lo habia generalizado.

### Verificado en el enlace, simbolo a simbolo

Con el hueco puesto, el `.bss` de zFeOverlay mide **7.216 B, exactamente el del
objetivo**, y el reparto cuadra al byte:

    nuestro objeto            7.232 B
      - value.14919 + _.tmp_0     8 B  estripados (estatica de funcion sin referencia)
      - gCarTypeNameHash          4 B  estripado (DECLARADO Y NO USADO, CarCustomize.hpp:706)
      - g_pEAXSound               4 B  estripado (el zFeOverlay.o objetivo lo tiene INDEFINIDO)
      = 7.216 B  =  objetivo

## 2. zAnim: el hueco de 16 B — `linkdelta` es CIEGO a el, y aun asi hay que ponerlo

`zAnim.cpp`, ultima linea:

    char _zAnimBssTail[16] asm("gap_07_80457768_bss");

`linkdelta` da `.bss +0` **con el hueco y sin el**: el DOL rellena cada seccion a 32 B y
los 16 B se comian en el relleno del final de `.bss`. Es exactamente
`nfsmw-huecos-leer-el-dol` / «un -24 puede estar ya bien y un +8 no».

**Pero las direcciones si cambian, y son todas las de detras.** Enlazando base contra
nuestro y comparando el `.bss` simbolo a simbolo:

| simbolo | ANTES | DESPUES |
|---|---:|---:|
| `TheWorldAnimInstanceDirectory` (ultimo de zAnim) | -1.664 | -1.664 |
| `gap_07_80457768_bss` | AUSENTE | presente, -1.664 |
| `_6Attrib.gDatabaseType` | **-1.680** | **-1.664** |
| `gSharedStringPool`, `TheMemoryAllocator`, `bUnitVector3`... (todo el resto del `.bss` del juego) | **-1.680** | **-1.664** |

zAnim es el objeto **1** del enlace, asi que ese escalon de 16 B lo sufria **todo el
`.bss` de la imagen**. Ahora el desplazamiento es UNIFORME y sale entero de
`rodata-1336` + `data-192`, que es deuda de contenido de zAnim y de nadie mas.

El recuento tambien cuadra: objeto 508 B -> 492 + 16 = 508, menos 12 B estripados
(`k.19455`+`_.tmp_0`, 8 B, y `gCarTypeNameHash`, 4 B) = **496 = el objetivo**.

## 3. Lo que ya NO le queda a zFeOverlay: solo DOS funciones mal colocadas de 467

`permorden`: **465 de 467 en su sitio.** Las dos son:

| simbolo | tam | objetivo | nuestro | desplazamiento |
|---|---:|---|---|---:|
| `_._8CarDatum` | 52 B | puesto **#463** | puesto **#402** | **-5.540 B** |
| `TextureLoadedCallbackAccessor__24QRCarSelectBustedManagerUi` | 32 B | puesto **#431** | puesto **#462** | **+3.444 B** |

Y explican el histograma ENTERO del `.over`. Recorriendo la seccion enlazada:

    803a41b8 .. 803c524c   delta   +0     (135 kB, el 96 % del codigo)
    803c52e4 .. 803c5ab0   delta  +52     <- los 52 B de _._8CarDatum metidos aqui de mas
    803c5af4               delta +3444    <- el accesor, el solo
    803c5b14 .. 803c6854   delta  +20     (= 52 - 32)
    803c6888               delta -5540    <- CarDatum en su sitio del objetivo, vacio
    803c68bc .. fin        delta   +0

O sea: **dos simbolos que suman 84 B descolocan 5,6 kB de `.over`, y no hay nada mas.**

`_._8CarDatum` sale de `feArrayScrollerMenu.hpp:137` (`~CarDatum() override {}`, cuerpo en
clase = cola de `saved_inlines`, ordenada por CUANDO SE CIERRA LA CLASE). El objetivo la
cierra casi la ultima; nosotros en el puesto 402 porque la cabecera se parsea pronto. Y la
firma es la de la r55 §4: la vtable va al reves, **`_vt.8CarDatum` esta a +1.448** mientras
todas las demas estan a -1.280/-1.312.

**No lo he tocado: `feArrayScrollerMenu.hpp` es cabecera `fe`** y llega a zFe y zFEng.
Va en §7 como propuesta con la medida.

## 4. Lo que le queda a zFeOverlay de TAMANO: 1.136 B, y no son cadenas

`over-1136` (con `keep.lst` corregido). Descompuesto:

* **El codigo esta completo**: de los 468 simbolos de `.over` del objetivo, **los 468
  sobreviven en nuestro enlace**, 141.224 B, `secdiff` da 0 faltantes y 0 de otro tamano.
  Los 17 «de mas» que sobreviven (500 B: `_._8FEWidget`, `SetPos__8FEWidget`,
  `SetTime__5Timerf`...) resuelven a **zFe.o** en los dos enlaces: no cuestan nada.
* El deficit esta en la parte `rename:.rodata`: el objetivo conserva **8.169 B** y nosotros
  **7.465 B**. Los 327 simbolos que el encargo daba como «no emparejan por nombre» son el
  caso `nfsmw-el-cero-que-miente`: los del objetivo se llaman `lbl_803Cxxxx` y los nuestros
  `$LCnnn`. No es trabajo, es nomenclatura.
* `prefijotu zFeOverlay` da **22 cadenas / 91 B** y las 22 son basura de parseo:
  **zFeOverlay no tiene deuda de vocabulario**. Lo que falta es dato con nombre, y el mayor
  es `lbl_803C6A9C`, de **756 B**.
* `.data` esta LIMPIO: recorriendo la seccion enlazada, todos los simbolos van a -1.280
  uniforme y no hay ni un escalon dentro de la ventana.

## 5. El hallazgo que NO es mio y bloquea la ronda: `keep.lst` rancio en 10 unidades

`python scripts/lcfix.py --check` da **431 correcciones en 10 unidades**:

    zFe.o 101 | zPhysics.o 69 | zEAXSound.o 49 | zSpeech.o 49 | zEAXSound2.o 44
    zAI.o 30  | zGameplay.o 13 | zAnim.o 11 | zFeOverlay.o 10 | zEagl4Anim.o 3

**Todas son el mismo desplazamiento de -3**: una cabecera compartida perdio 3 literales
esta ronda (`PackedDecimal.h` es la candidata demostrada: sus estaticas `lower.`/`upper.`
desaparecieron de mis DOS objetos entre mi medida inicial y la siguiente) y eso renumero
los `$LC` de las diez unidades. Es `nfsmw-lc-se-desplaza` en vivo.

**Cuanto cuesta, medido** — con una copia privada de `keep.lst` con las 431 correcciones
aplicadas (salida de `scripts/lcfix.py --check`; no he tocado el fichero del repo):

| unidad | keep.lst de hoy | keep.lst corregido |
|---|---|---|
| zFeOverlay | `sbss+4 over-1288` | `sbss+4` **`over-1136`** |
| zAnim | `rodata-1488 data-192` | **`rodata-1336`** `data-192` |

**152 B por unidad, y en DOL 1.235 B solo en estas dos** (524.662 contra 523.427 con los
objetos de HEAD). Son diez unidades, asi que el coste real es mucho mayor.

La copia corregida esta en `<scratchpad>/keep_r56fix.lst` (431 aplicadas, 0 desajustes)
por si a la ventana le sirve. **Yo no puedo aplicarlo (regla 3), y ademas no se debe con
agentes midiendo.** Pero **ninguna medida de esta ronda vale sin correrlo**, y menos un
`trypromo`.

## 6. Trampa de medida nueva (me costo una hora y una conclusion falsa)

**`keep.lst` empareja por NOMBRE DE FICHERO OBJETO**, no por unidad: las entradas son
`zAnim.o:gap_07_80457768_bss`. Si copias un `.o` al scratchpad con otro nombre para
compararlo (`zanim_r56.o`), **el enlazador deja de encontrar sus entradas de `keep.lst` y
estripa todo lo que ellas protegian**, en silencio y sin error.

Me hizo medir «el hueco de 16 B no cambia ni un byte del DOL» —el hash salia identico— y
la explicacion era que en las dos ramas el hueco estaba estripado. Con el basename
correcto (`.../r56head/zAnim.o` y `.../r56new/zAnim.o`) el hueco aparece y las direcciones
cambian. **Cualquier probe que renombre objetos y toque unidades con `$LC`/`gap_`/`lbl_` en
`keep.lst` da resultados invalidos.**

## 7. Propuestas: medidas y no aplicadas por no poseer el fichero

1. **`feArrayScrollerMenu.hpp:137`, `struct CarDatum` — vale el 100 % del desorden que le
   queda al `.over` de zFeOverlay** (§3): 2 funciones de 467, 84 B de simbolo, 5,6 kB de
   seccion desplazada. La clase se cierra en el puesto 402 y el objetivo la cierra en el
   463. La palanca es la de la r55 §4: mover CUANDO se completa la clase (un `#include`),
   no el cuerpo. Cabecera `fe`, llega a zFe y zFEng.
2. **`TextureLoadedCallbackAccessor__24QRCarSelectBustedManagerUi`** (32 B,
   `uiQRCarSelect.hpp`) tiene que salir en el puesto 431, entre
   `_._20CustomizePerformance` y `_._9UIQRBrief`. **Arreglarlo SOLO no paga**: sin CarDatum
   el tramo 803c5b14..803c6854 pasa de +20 a +52. Van juntos o no van.
3. **zAnim `.data -192`**: son **9 huecos anonimos** que el objetivo tiene y nosotros no:
   `gap_06_804155F4_data` (124 B), `gap_06_804158B4_data` (36), `lbl_80415678` (8),
   `gap_06_8041577C_data` (8) —**los cuatro YA estan en `keep.lst`, lineas 79-82**— mas
   cinco de 4 y 2 B que no lo estan (18 B). No los he puesto porque van en **nueve puntos
   distintos** de la secuencia de `#include` y meterlos en un solo bloque arregla el tamano
   y estropea el orden interno.
4. **zAnim `.rodata -1.336`**: `prefijotu zAnim --asm` genera las **66 cadenas / 1.188 B**
   que faltan, en el orden del objetivo, y **un `asm()` de `.rodata` NO renumera los `$LC`**
   (el contador es de literales de C), asi que se puede pegar sin `lcfix`. **No lo he
   pegado y esta es la razon, con numeros**: 1.336 - 1.188 = **-148**, no cero, y un
   `.rodata` que sigue descuadrado desplaza los mismos 279 kB de `.rodata` posterior que
   ya desplaza — **cambia QUE bytes estan mal, no CUANTOS**. Es `matched_code` todo-o-nada
   aplicado a los datos. Solo paga si se llega a 0, y para eso hace falta ademas cerrar los
   148 B que no son cadenas y quitar las 2 vtables de mas (+48 B).

## 8. Negativos medidos (probados y revertidos)

* **`asm()` de fichero para el hueco de `.bss`**: cae 4 kB antes de su sitio (§1). Revertido.
* **Mover la definicion de `gPlayerNum`**: el objetivo la emite entre
  `TopOrFullScreenLoadingReason` y `CustomizeHUDTexPackResources`; nosotros la ultima de
  `.bss`. Probe **cuatro** sitios —`uiQRCarSelect.cpp` (original), `CarCustomize.cpp:62`,
  `FECarViewer.cpp:10`, y `static int _x asm("gPlayerNum")`— **y sale la ultima en las
  cuatro**. El sitio de la definicion NO manda en el orden de la cola `.lcomm` de
  `finish_file`, ni el `extern` previo, ni que sea `static`. Las cuatro revertidas.
  **Cuesta 0 B**: `gCarTypeNameHash` ocupa hoy su ranura y se estripa, asi que el `.bss`
  mide bien igual; lo unico mal son las direcciones de `gPlayerNum`,
  `CustomizeHUDTexPackResources` y `CustomizeHUDTexTextureResources`, 4 B.
  Queda como el unico desorden de `.bss` de la unidad, y sin palanca conocida.

## 9. Sorpresas

1. **Las dos entradas de `keep.lst` que la r55 pedia YA ESTABAN** (lineas 83 y 809). El
   bloqueo de la r55 sobre el hueco de 4.104 B era un fichero sin mirar.
2. **Un `asm()` de fichero no puede ser el ultimo simbolo de `.bss`**, nunca, en ninguna
   unidad: la cola `.lcomm` de `finish_file` va siempre detras. Contradice la lectura
   habitual del brief y afecta a todos los huecos de `.bss` del arbol.
3. **`linkdelta` es ciego a un hueco de 16 B** (relleno de 32 B del DOL) aunque ese hueco
   descoloque el `.bss` entero del juego. Una unidad puede dar `.bss +0` y tener el
   `.bss` mal.
4. **Copiar un `.o` con otro nombre invalida `keep.lst`** y con el la medida (§6).
5. **A zFeOverlay le quedan DOS funciones de 467**, y las dos estan en cabeceras que no son
   mias. El «orden» de esta unidad esta practicamente cerrado.
6. **El sitio de una definicion no decide el orden de `.bss`.** Cuatro colocaciones de
   `gPlayerNum`, el mismo puesto en las cuatro.
