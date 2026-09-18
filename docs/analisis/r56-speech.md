# r56 — zSpeech: el `.rodata +8` era `keep.lst`, y el `.data` estaba 8 B corta

Agente `speech`. Propiedad: `zSpeech`, ficheros de `Speech/` y `Stream/`.
Dos ficheros tocados, los dos incluidos **sólo** por `zSpeech.cpp` (comprobado):
`Speech/SoundAI.cpp` y `EAXSound/Stream/SpeechManager.cpp`. Ni `config/`, ni
`splits.txt`, ni `keep.lst`, ni `configure.py`.

---

## 0. Titular

**De 269.714 B de DOL distinto a 19.383.** Y ninguno de los tres bloqueos que
traía el encargo era lo que decía ser:

| paso | bytes de DOL distintos | quién lo aplica |
|---|---:|---|
| árbol tal como está al empezar la ronda | **269.714** | — |
| + `python scripts/lcfix.py` (379 entradas desfasadas) | 215.151 | **la ventana** |
| + borrar `zSpeech.o:$LC58` de `keep.lst` | 68.596 | **la ventana** (propuesta) |
| + mis dos ediciones de fuente | **19.383** | **hecho** |

Con las dos correcciones de `keep.lst` puestas, **las siete secciones del ELF
enlazado dan delta 0** (`.text`, `.rodata`, `.data`, `.sdata`, `.sdata2`, `.bss`,
`.sbss`) y el DOL sale del tamaño exacto. Lo que queda es orden de emisión puro.

`fncmp` antes y después: **idéntico**, 1 de 703 funciones (`Setup`, 596 B).
Cero regresiones.

Sellos, con tres compilaciones cada uno:

| | `sha1` del `.o` | `linkdelta` | `trypromo` |
|---|---|---|---|
| HEAD | `fc8bbcd7151985723cdc6d666f836e101fbd092d` | `rodata −344` (keep rancio) | `436a6a7313ca` |
| después | `bc5b120e241efabbe4bd00ae7e90f1d1d522b313` | `rodata −344` (keep rancio) | `436a6a7313ca` |
| después, con el `keep` corregido y sin `$LC58` | mismo `.o` | **+0 en las siete secciones** | `c6fa3ea45dd4`, 19.383 B |

(El `.o` que había en `build/` al empezar la ronda era el de la r55,
`d0b1ff71aaa9`, y **no correspondía a la fuente**: ver §7.1.)

---

## 1. El `.rodata +8`: era una entrada de `keep.lst` caducada

`keep.lst:1668-1669`

```
# @lc zSpeech "GAMECUBE"
zSpeech.o:$LC58
```

`$LC58` es la cadena `"GAMECUBE\0"` (9 B) que `cc1plus` interna al parsear
`bware.hpp`. Está **muerta**: nadie la referencia. La entrada la salva entera.

Pero desde la r33 `zSpeech.cpp` abre con un `asm()` de ámbito de fichero que
escribe a mano el prefijo de bWare/STL, y ese bloque **ya empieza por
`"GAMECUBE"`**:

```asm
lbl_80405E10:
  .4byte 0x47414D45, 0x43554245, ...   ; "GAMECUBE"
```

Resultado: en `[0x80405E10, 0x80408FB8)` el objetivo tiene **una** `"GAMECUBE"`
y nosotros **dos**. Sin la entrada, `-strip-unused-data` se lleva `9 & ~7 = 8`
bytes y deja 1 de residuo — que es justo lo que el objetivo tiene ahí.

Medido, A/B limpio, mismo `.o` (`fc8bbcd7`), sólo cambia el fichero `keep`:

| `keep.lst` | `.rodata` | bytes de DOL distintos | tamaño del DOL |
|---|---:|---:|---:|
| con `lcfix` aplicado | **+8** | 215.151 | 4.541.920 (32 B de más) |
| con `lcfix` y **sin** `zSpeech.o:$LC58` | **+0** | **68.596** | 4.541.888 ✔ |

**−146.555 B por borrar una línea.** El `+8` del `.rodata` que llevaba dos
rondas sin localizar y que el informe de la r55 daba por «pool o relleno, sin
emparejamiento posible» no estaba en el pool: estaba en la lista de `-keep`.

### Extrapolación: 13 unidades más con el mismo patrón — MEDIDA EN UNA, CONTADA EN 14

`keep.lst` tiene **14** entradas `# @lc <unidad> "GAMECUBE"`, y **13 de esas 14
unidades tienen además un `asm()` de prefijo bWare en su SourceList que ya emite
`"GAMECUBE"`** (la excepción es `zLua`, que no tiene bloque). Contando las
apariciones en el rango `.rodata` de cada unidad en el ELF objetivo:

```
zCamera            [803D1928,803D3BF8) x1   zCamera.o:$LC58            keep.lst:1091
zEAXSound          [803D5E38,803D8D90) x1   zEAXSound.o:$LC61          keep.lst:1503
zEAXSound2         [803D9050,803DD468) x1   zEAXSound2.o:$LC146        keep.lst:1588
zFe                [803DF568,803E4290) x1   zFe.o:$LC58                keep.lst:2371
zFe2               [803E4380,803EA7E8) x1   zFe2.o:$LC58               keep.lst:1006
zGameplay          [803EBE98,803ED338) x1   zGameplay.o:$LC59          keep.lst:2023
zPhysicsBehaviors  [803FA7B0,80402108) x1   zPhysicsBehaviors.o:$LC145 keep.lst:1883
zSpeech            [80405E10,80408DD8) x1   zSpeech.o:$LC58            keep.lst:1669  <- MEDIDA
zWorld             [80409DB4,8040DCC4) x1   zWorld.o:$LC58             keep.lst:2115
zWorld2            [8040DFE8,8040F0B8) x1   zWorld2.o:$LC163           keep.lst:2194
zAI / zLua / zPlatform / zFeOverlay          rango sin acotar bien, sin contar
```

**Esto es un indicio contado, no un frente medido**: sólo se puede confirmar
promocionando cada unidad (la entrada `<u>.o:$LCn` es inerte mientras el enlace
use el objeto extraído). La comprobación por unidad son tres líneas: ¿su
SourceList tiene el bloque `0x47414D45`? ¿su rango del objetivo tiene una sola
`"GAMECUBE\0"`? Si las dos son que sí, la entrada sobra y son 8 B.

Vale **+8 B de `.rodata` por unidad**, o sea el DOL roto entero.

---

## 2. `keep.lst` está RANCIO: 379 entradas, y eso invalida medidas de esta ronda

`python scripts/lcfix.py --check` da **379 `CORRIGE`**, 30 de ellas de zSpeech
(sus `$LC` se han corrido **−3**). Con el árbol tal cual, `linkdelta` de zSpeech
dice `rodata −344` y `trypromo` da `436a6a7313ca`; con `lcfix` aplicado dice
`rodata +8` y `fac385ac7273`. **Son 54.563 B de DOL de diferencia que no tienen
nada que ver con la unidad.**

Dos consecuencias:

1. **La medida oficial de cualquier agente de esta ronda que toque literales
   está sesgada** hasta que se corra `lcfix`. No es un problema mío: el commit
   `4770b306` lo corrigió al cerrar la r55 y las ediciones de la r56 lo han
   vuelto a desfasar.
2. Yo he medido **siempre contra una copia corregida** en mi scratchpad
   (`keep_fixed.lst`, generada aplicando la salida de `lcfix --check` a una
   copia), nunca tocando el fichero del árbol.

---

## 3. El `.data` de zSpeech estaba **8 B corta**, y eso valía 47.900 B

Con el `.rodata` ya cuadrado, el reparto del deficit era:

    .text 10.607   .rodata 8.571   .data 49.126   .sdata 9      (68.313)

Los 49.126 del `.data` **no eran contenido**: la sección medía lo mismo, pero
la aportación de zSpeech era 8 B más corta, así que **todo el `.data` posterior
iba desplazado −8** hasta que la alineación lo reabsorbía en `CardData`
(`0x8044E620`). Comparando por zonas con el desplazamiento compensado:

```
[80415180,80435900) desp +0 :     2 de 132.992 B distintos
[80435900,80435F48) zona de zSpeech
[80435F48,8044E620) desp -8 :   146 de 100.056 B distintos
[8044E620,8045655C) desp +0 :     0 de  32.572 B distintos
```

O sea: **el 97 % del deficit de `.data` era el corrimiento**. Los 8 B son dos
huecos de 4 que el objetivo tiene y nosotros no:

### 3.1 `gap_06_80435EA4_data` — la entrada de `keep.lst` que nadie había emitido

`keep.lst:568` nombra `zSpeech.o:gap_06_80435EA4_data` desde hace rondas, pero
**el símbolo no existía en el árbol**: de los 8 `gap_06_*` de zSpeech sólo dos
(`80435E78` y `80435F24`) estaban escritos, en `zSpeech.cpp`. Los otros seis eran
entradas muertas.

Escrito en `SoundAI.cpp`, justo detrás de
`Singleton<SoundAI>::mInstance` (que es donde cae en el objetivo), con el mismo
idioma que los de `zSpeech.cpp`:

```cpp
asm(".section .data\n.balign 4\n.globl gap_06_80435EA4_data\n"
    ".type gap_06_80435EA4_data, @object\ngap_06_80435EA4_data:\n"
    "  .4byte 0x00000000\n.size gap_06_80435EA4_data, 0x4\n.previous\n");
```

Verificado en el enlace: el símbolo existe y `keep.lst` lo salva.

### 3.2 `TRACKSTREAMER_BACKLOG_THRESH` ocupaba un relleno

`SpeechManager.cpp:68` la definía junto a `SPEECH_DISPLAY_HISTORY`, y por eso
caía en `.data:0x8043597C`, que **en el objetivo es un hueco de 4 B**
(`gap_06_8043597C_data`). En el objetivo la variable está en `0x804359D0`,
detrás de `max_samplerequests` y del hueco de 12 B, o sea **después de todos los
estáticos de esa TU**.

Arreglo: `extern float` arriba y la **definición al final del fichero**.

### 3.3 Lo que valieron las dos juntas

    68.596 -> 19.410 bytes de DOL distintos    (-49.186)
    .data  49.126 -> 1.233

---

## 4. Las dos ediciones de contenido puro (0 bytes)

* **`SoundAI.cpp:1638`**: `static float prev_heat = 0.0f;` → **`1.0f`**. El
  objetivo tiene `3f800000` en `.data:0x80435EB0`. Es un bug de valor, no de
  tamaño.
* **`SoundAI.cpp`**: `DESTROY_COPS_ON_INACTIVITY` movida **delante** de
  `FORCE_VOICE_RANDOMIZATION` (el objetivo las tiene en `0x80435EA8` y
  `0x80435EAC`), no detrás del factory `_SoundAI`.

    19.410 -> 19.383   (-27)

Poco, pero es contenido correcto y no cuesta un byte.

---

## 5. Lo que queda, con cifra y con causa

    .text    9.605 B    orden de emisión: cinco bloques
    .rodata  8.571 B    orden del pool (reorden.py: CONTENIDO = 0)
    .data    1.207 B    orden de los estáticos
    -----------------
             19.383 B

`reorden.py` da **`.rodata` CONTENIDO 0 y `.data` CONTENIDO 0**: no falta ni un
byte de dato por escribir. Todo es *dónde*.

### 5.1 Los cinco bloques del `.text` (`permorden`: 26 desplazadas, 703 comunes)

```
1) reserve__..vector<SpeechSampleData*>..   284 B   obj#449 -> nue#415   (34 huecos antes)
2) bloque de 17 destructores               1.096 B   obj[604:621] -> nue[624:645]
     _._SpeechSampleVec, _._VoiceUsage, _._SchedSpchEvents,
     _._copList, _._observations, GetNumBanks__Module, ...
3) GetCSIptr/GetChannel/GetEventDat/IsDataLoaded__SED_NISSFX
   + _._VecHashMap644 + _._SpchSampleMap + RebuildTable   920 B   obj[660:667]
4) _._Q26Speech13SampleReqList              176 B   obj#683 -> nue#680
5) el ciclo de 4 destructores: el objetivo emite
     SpeechHashIDMap, EventHistory, SPCHEventList, SampleReqList
   y nosotros
     SampleReqList, SpeechHashIDMap, EventHistory, SPCHEventList
   (una rotación: SampleReqList del último al primero)
```

(5) tiene palanca evidente: en `SpeechManager.hpp` las clases se declaran
`SampleReqList`(67), `SpeechHashIDMap`(111), `EventHistory`(120),
`SchedSpchEvents`(136), `SPCHEventList`(142) — y nuestro orden de emisión **es
exactamente el de declaración**. Mover `SampleReqList` detrás de `SPCHEventList`
debería rotarlo. **NO lo he probado**: `SpeechManager.hpp` la incluyen 17 `.cpp`
de seis unidades más y no toco cabeceras compartidas con agentes midiendo en
paralelo. Es lo primero de la r57.

### 5.2 El pool: cuatro errores de contenido que se COMPENSAN entre sí

El `.rodata` mide exacto **por casualidad**: sobran 77 B de cadena y faltan 50,
y el resto lo tapa el relleno. Los cuatro, medidos por multiconjunto de cadenas
sobre el rango `[80405E10,80408FB8)`:

| qué | bytes | dónde |
|---|---:|---|
| `"AUD:Relocated speech headers"` **duplicada** | 29 | `zSpeech.cpp` la mete en el `asm()` de la r52 **y** `SpeechManager.cpp:667` la referencia viva |
| `"AUD: SED_NISSFX events"` **duplicada** | 23 | ídem, `SpeechManager.cpp:2161` |
| `"SoundAI CarCustomization"` **duplicada** | 25 | ídem, `SoundAI.cpp:1635` — y **además** tiene entrada en `keep.lst:2879` |
| `"Speech::Cache::Alloc"` **falta** | 21 | ver abajo |
| `"Attrib::Gen::aud_moment_strm"` **falta** | 29 | falta el `#include` del `.h` generado en el bloque `c34dat2` de `zSpeech.cpp` |

Las tres duplicadas son **andamio caducado**: el bloque `asm()` de la r52 se
escribió como «cadenas que el objetivo tiene y `cc1plus` no emite», y desde
entonces el árbol ha ganado el código que las referencia. Hay que quitarlas del
`asm()`, no del código.

**`"Speech::Cache::Alloc"` tiene causa exacta.** `SpeechCache.cpp:238`:

```cpp
const char *allocname;                     // local SIN INICIALIZAR
memptr = bMalloc(size, allocname, 0, (SpeechMemoryPool & 0xf) | 0x1000);
```

`bMalloc` se resuelve a `bMalloc__Fii` (dos enteros: el nombre y el 0 se tiran),
así que **poner el literal no cambia ni una instrucción** — la función ya casa —
pero hace que `cc1plus` interne la cadena, que es de donde sale en el objetivo.
Es el mismo mecanismo de todas las cadenas muertas de `.rodata`: un argumento de
depuración que el macro descarta. Necesita además su entrada `# @lc` en
`keep.lst`, porque muerta se estriparía a 5 B.

**AVISO**: los cinco arreglos hay que hacerlos **juntos**, y aun así el balance
sale `−76 + 24 + 32 ≈ −20`. Falta identificar ~20-50 B más de pool (no son
cadenas: el multiconjunto de cadenas ya está cerrado, así que son flotantes o
relleno). Hacer sólo una parte **rompe el tamaño de `.rodata` y con él el
DOL entero**: por eso no he tocado nada de esto.

### 5.3 Los residuos que sobran

Cinco colas de cadena muerta que el objetivo no tiene: `uction`(7), `stems`(6),
`bosfx`(6), `ssis`(5), `done`(5). Vienen de los `#include` del bloque `c34dat2`
de `zSpeech.cpp`. Cuentan en el balance de §5.2.

---

## 6. `Setup__Q26Speech13RoadblockFlow`: la veda sigue, y `previo.py` NO la encuentra

La veda está escrita en el fuente, encima de la función
(`RoadblockFlow.cpp:236`): **r36d, r36f, r45, r48 barrieron 15 formas**, más las
8 de la r55 = 23 negativos. No la he vuelto a atacar.

**TRAMPA NUEVA, y es de herramienta.** El dossier manda correr `previo.py` antes
de tocar nada, y `fncmp` (y el propio encargo) dan el nombre **manglado**:

```
python scripts/previo.py Setup__Q26Speech13RoadblockFlow   ->  0 bloques
python scripts/previo.py RoadblockFlow::Setup              ->  1 bloque, r36d/r36f/r45/r48
```

Con el nombre que imprimen las demás herramientas, `previo.py` dice **que no hay
trabajo previo** de una función con cuatro rondas de negativos encima. Arreglo:
desmanglar `X__QnN...` (o buscar también el último componente) antes de filtrar.

---

## 7. Sorpresas

1. **El `.o` de `build/` estaba rancio.** El primero que medí daba
   `d0b1ff71aaa9` (el de la r55) y `rodata +8`; recompilando la misma fuente sale
   `fc8bbcd7...` y `rodata −344`. Los dos enlazan al **mismo DOL** si a cada uno
   se le da el `keep.lst` de su época: la diferencia entera era la numeración
   `$LC`. Confirma el aviso del encargo y añade el porqué.
2. **El `+8` no estaba en el pool.** Dos rondas buscando 8 bytes de dato dentro
   de 12.712, y estaban en una lista de `-keep`.
3. **`movidos.py` no sirvió para nada aquí** (mezcla robo y estripado, como dice
   el dossier); lo que resolvió el `.data` fue comparar el ELF enlazado por zonas
   con el desplazamiento compensado. Herramienta de un solo uso, en mi scratchpad.
4. **La `.data` puede estar 8 B corta con la sección midiendo EXACTO.** La
   alineación de un símbolo posterior reabsorbe el corrimiento y `linkdelta` da
   `+0` en todas las secciones mientras 100 kB van desplazados. `linkdelta` no
   ve esto; hay que comparar posiciones de símbolo.
5. **`Cache::Alloc` pasa una local sin inicializar** donde el original pasaba un
   literal, y **casa igual** porque el macro tira el argumento. Es un patrón que
   hay que buscar en todo el árbol: cada uno de esos vale su cadena de `.rodata`.
6. **`sed -i` convierte a LF un fichero que estaba en CRLF** y deja un diff de
   fichero entero. `SoundAI.cpp` está en CRLF en `HEAD` (2.674) y una sola
   sustitución con `sed -i` lo dejó en 2.690 líneas cambiadas. Rehecho en
   binario: el diff final son **22 líneas**. En el árbol de esta ronda hay al
   menos dos ficheros de otros agentes con el mismo síntoma
   (`GRaceStatus.cpp` 2.935 líneas, `PVehicle.cpp` 1.065): conviene mirarlo antes
   de commitear, porque `nfsmw-pr-upstream-workflow` ya avisa de que el CRLF
   rompe `git apply`. Y ojo con el heredoc de bash: `\\n` dentro de un literal C
   se le puede escapar y meter un salto de línea REAL dentro de la cadena.

---

## 8. Lo que pido, con la cifra

| propuesta | vale | quién |
|---|---:|---|
| correr `python scripts/lcfix.py` (379 entradas) | 54.563 B de DOL, y desbloquea la medida de todos | ventana |
| borrar `keep.lst:1668-1669` (`# @lc zSpeech "GAMECUBE"` + `zSpeech.o:$LC58`) | **146.555 B** | ventana |
| revisar las otras 13 entradas `# @lc <u> "GAMECUBE"` | 8 B de `.rodata` por unidad = DOL roto | los dueños |
| arreglar `previo.py` para nombres manglados | media ronda por agente | ventana |

Y para la r57 de zSpeech, en este orden:
1. `SampleReqList` detrás de `SPCHEventList` en `SpeechManager.hpp` (§5.1.5).
2. Los cinco arreglos del pool **juntos** + los ~20 B que faltan por identificar (§5.2).
3. Los bloques 2 y 3 del `.text` (§5.1).
