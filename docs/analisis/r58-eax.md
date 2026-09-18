# r58 — `zEAXSound` + `zEAXSound2`: el `bss+32` no es nuestro, es la RAYA de `splits.txt`

Agente `eax`. Unidades: `Speed/Indep/SourceLists/zEAXSound` y `…/zEAXSound2`.

**Ficheros de fuente tocados: UNO, y sólo con un comentario** —
`src/Speed/Indep/Src/EAXSound/AudioMemoryManager.cpp`, el negativo de §5 anotado donde
`previo.py` lo encuentra—. Cero cabeceras (ni mías ni ajenas), cero `config/`, cero
`keep.lst`, cero `splits.txt`, cero `configure.py`. **Cero correcciones de `lcfix`
pendientes por mi culpa** (§7).

    ANTES == DESPUES en las dos unidades, y el DOL lo demuestra:
    trypromo(zEAXSound + zEAXSound2) = DOL ROTO 72487037a9d4  <- el MISMO hash con que cerro la r57
    zEAXSound    linkdelta .text +0   bss+32       dolwhere: LAS SECCIONES NO COINCIDEN
    zEAXSound2   linkdelta .text +0   IGUAL        dolwhere: 39.630 B
    fncmp  zEAXSound 0 de 773        zEAXSound2 1 de 930 (GenerateRoadNoise)

---

## 0. El encargo pedía quitar entre 32 y 63 B de `.bss`. La cifra buena es otra, y el
## dueño de esos bytes también

El encargo y la r57 §6.2 daban por hecho que **emitimos `.bss` de más** y que había que
**dejar de emitirlo** (los tres `_GetKind` de `Generated/Messages/M*.h`, 24 de los 32 B).

Medido esta ronda: **los 15 pares de estáticas que “sobran” SÍ están en el original**. No
sobran: están **fuera de nuestra ventana** porque la raya de `splits.txt` entre
`zEagl4Anim` y `zEAXSound` está **120 B tarde**.

La prueba es de dos líneas y no admite discusión:

    las 15 cadenas de esas 15 estaticas, en el .rodata del OBJETIVO de zEAXSound:
      WorldBodyConn 20   Pkt_Body_Open 21 ... Pkt_Effect_Service 29   (las 12 de WorldConn.h)
      MMiscSound 57      MControlPathfinder 75      MGamePlayMoment 196

Una `static UCrc32 k("…")` dentro de una función en clase **emite la cadena Y la estática
al PARSEAR la cabecera**, aunque nadie llame a la función (por eso las 12 de `WorldConn.h`
no tienen ni un uso en nuestro `.text` y aun así están). Si la cadena está en el `.rodata`
del objetivo, **la estática estaba en su `.bss`**. No está en la ventana de `zEAXSound`
porque cae en la de al lado:

    zEagl4Anim  .bss  0x8045B110..0x8045B268 (344 B) y sus ULTIMOS 140 B son
                gap_07_8045B1DC_bss  <- hueco ANONIMO de 140 B, 0x8045B1DC..0x8045B268
    nuestro zEagl4Anim .bss = 196 B: no tiene ese hueco (le faltan 148 B)
    nuestro zEAXSound  .bss: 120 B de CABEZA de mas, exactamente 15 pares de 8 B

**140 ≥ 120.** Los 120 B que a nosotros nos sobran en la cabeza de `zEAXSound` son la cola
de ese hueco de 140 B.

### La propuesta, con la raya al byte

    splits.txt:  zEagl4Anim  .bss end:   0x8045B268 -> 0x8045B1F0
                 zEAXSound   .bss start: 0x8045B268 -> 0x8045B1F0

Es una raya **entre dos objetos ADYACENTES en el mismo orden de enlace** (`bssalign`
confirma que `zEagl4Anim` va justo antes que `zEAXSound` en `.bss`), así que la imagen no
se mueve: sólo cambia quién emite esos 120 B. Es distinto del caso de
`nfsmw-rango-no-basta`, que era un rango **huérfano** cambiando de sitio de enlace.

Después de moverla:

| | antes | después |
|---|---|---|
| cabeza `.bss` de `zEAXSound`, objetivo | 136 B | **256 B = la nuestra, al byte** |
| ventana `.bss` de `zEAXSound` | **+16** | **−104** |
| `.bss` del objetivo de `zEagl4Anim` | 344 B (nuestro 196, −148) | 224 B (nuestro 196, −28) |

y los −104 los cierro **yo**, con `.bss` anónimo al final de `zEAXSound.cpp` (la receta de
la r56 §3 edición 10): al objetivo le faltan en la cola `gap_07_8045DDF4_bss` (108 B) y
`gap_07_8045DDC8_bss` (4 B) y a nosotros nos sobra `SND11MemoryPoolNum` (4 B, `address:
0xFFFFFFFF` en el DWARF, **sin una sola referencia en todo el árbol**).

**Es un paquete atómico de dos manos**: la raya es del jefe, el relleno es mío. Suelto, cada
mitad EMPEORA (la raya sola deja `.bss` a −120; el relleno solo, a +128).

---

## 1. La contabilidad completa del `bss+32`, que no existía

`linkdelta` dice `bss+32`. Eso **no** son 32 B de contenido nuestro: son dos poblaciones y
un redondeo.

    cabeza  (hasta gAudioMemoryManager)   nuestra 256 B   objetivo 136 B   +120
    cuerpo                                        10872           10872      0
    cola                                            400             248   +152
                                                                    ------
    objeto:                                       11528           11256   +272
    menos g_StreamTable (256 B, muerto: 256 & ~7 = 256, el enlazador lo recupera entero)
    VENTANA en el ENLACE:                         11272           11256    +16
    seccion .bss del ELF                                                   +32   <- cuantizada a 32

`agent_eax58_bssvivos.py` lo enseña sin modelo: el objetivo mete 78 de sus 79 símbolos de
`.bss` en el enlace y nosotros 72 de 119, **pero el espacio de los 47 “estripados” SIGUE
AHÍ** —una estática muerta de 4 B no se recupera nunca, `4 & ~7 == 0`— y por eso
`gAudioMemoryManager` cae 120 B más tarde en nuestro enlace que en el suyo.

Y la cola: nos faltan 104 B netos, todos huecos anónimos del objetivo.

### La función de transferencia, medida (no supuesta)

`agent_eax58_probe_bss.py` compila una COPIA de la SourceList en el scratchpad con N bytes
de `.bss` anónimo de más y la enlaza sustituida, **sin tocar el árbol compartido**:

    pad    +0   secciones: bss+32      <- control: reproduce linkdelta exactamente
    pad    +8   secciones: bss+32
    pad   +16   secciones: bss+32
    pad   +24   secciones: bss+64

**La sección va a saltos de 32** y hoy estamos a +16 dentro del escalón. De ahí sale el
intervalo bueno: **quitar entre 16 y 40 B de `.bss` cierra la sección** (el encargo decía
32–63, y los 24 B de la r57 §6.2 habrían funcionado — pero por la razón equivocada, y a
costa de dejar de emitir algo que el original SÍ emite).

**Un aviso de herramienta que costó una medida**: si el `.o` de la copia no se llama
`zEAXSound.o`, `keep.lst` no lo reconoce (indexa por nombre de objeto) y la sonda miente
`rodata-1088 data-384`. Está arreglado en el script y el control `pad +0 → bss+32` lo
verifica en cada corrida.

---

## 2. El `.bss` **no** vale ~0 bytes de DOL. Vale ~1.900

La r57 §6.2 lo dio por gratis («`.bss` no va en el DOL»). El contenido no, pero **las
DIRECCIONES sí**: cada `lis/addi` que apunta a un símbolo de `.bss` aguas abajo está en
`.text`, y `.text` sí va en el DOL. Medido con `agent_eax58_doldelta.py`, sustituyendo sólo
`zEAXSound`:

    pad  +0   bss+32   DOL 34.338 bytes distintos
    pad +16   bss+32   DOL 36.229           (+1.891 por 16 B de desplazamiento)
    pad +24   bss+64   DOL 36.578

Es decir: **16 B de corrimiento de `.bss` = 1.891 bytes de DOL**, casi todos aguas abajo, en
los 14.436 B que la r57 apuntó como «fuera de mis ventanas». Y los corrimientos parciales
no pagan nada: un `lis/addi` acierta o no acierta, así que **sólo paga el cierre completo**.
Por eso no toqué `SND11MemoryPoolNum` sólo (−4 B, no cierra nada por sí solo; entra en el
paquete de §0).

---

## 3. Las 32 parejas de la cabeza, una a una

`agent_eax58_bssmap.py` resuelve también las reubicaciones contra el símbolo de SECCIÓN con
addend (que es como `gas` escribe casi todas las referencias a estáticas locales), así que a
diferencia de `agent_eax57_duenos.py` no se deja ninguna.

| off | pares | de dónde | ¿en el objetivo? |
|---:|---|---|---|
| 0 | `_GetKind__18MControlPathfinder` | `Generated/Messages/MControlPathfinder.h` | **sí** (cadena en `.rodata` 75) |
| 8 | `_GetKind` de `MPerpBusted` | `Generated/Messages/MPerpBusted.h` | sí (70) |
| 16–111 | **12 pares de `WorldConn.h`** (`Pkt_Body_Open/Service/Send`, `Pkt_Effect_*`) | `World/WorldConn.h`, macro `DECLARE_WORLDPACKET` | sí (20–29) |
| 112–143 | 4 pares de `SoundConn.h` (`Pkt_Heli_Open`, `Pkt_Car_Open`) | **mío** | sí (80–85) |
| 144 | `_GetKind__10MMiscSound` | `MMiscSound.h` | **sí** (57) |
| 152 | `_GetKind__15MGamePlayMoment` | `MGamePlayMoment.h` | **sí** (196) |
| 160 | `value` de `"VehicleParams"` | `Physics/PVehicle.h:49` | sí (67) |
| 168–199 | 4 pares de `Pkt_Car_Service` / `Pkt_Heli_Service` | `SoundConn.cpp` (fuera de clase) | sí, y **en la ventana** |
| 200 | `_GetKind` de `MUnspawnCop` | `MUnspawnCop.h` | la cadena no está (estripada) |
| 208–255 | `prevbrakestate`, `MCountdownDone`, `MNotifyVehicleDestroyed`, `MAIEngineRev`, `MPursuitBreaker`, `MNotifyMusicFlow` | varios | sí, y **en la ventana** |

Las 10 últimas parejas son las 10 que el objetivo sí tiene con nombre en su ventana (20
símbolos, 80 B) más sus 7 pares de hueco (56 B) = 136 B. Las 15 primeras son los 120 B de
§0.

**Y el ORDEN sigue mal aunque el tamaño se arregle**: el objetivo pone
`MNotifyMusicFlow` **primero** y nosotros **último**, y mete dos pares muertos entre
`Pkt_Car_Service` y `Pkt_Heli_Service` (que en `SoundConn.cpp` son consecutivos). Eso es
orden de parseo, o sea el mismo frente de §4, y **no lo arregla la raya**.

---

## 4. El `.rodata`: las dos unidades están frenadas por lo MISMO, y una mitad no es mía

`rodorden` da la primera divergencia de las dos en el mismo sitio, el índice 5–6:

    zEAXSound    insert obj[6:6] | nue[6:8]  GAMECUBE  "Audio Memory Pool"
    zEAXSound2   insert obj[6:6] | nue[6:8]  bad_alloc GAMECUBE

* `GAMECUBE` y `bad_alloc` son los **duplicados de `keep.lst`** que la r57 §6.1 ya midió (12
  y 16 B): esas cadenas ya vienen dentro del blob de prefijo de bWare. Es la edición atómica
  del jefe (quitar tres entradas y añadir 8 B en `zEAXSound.cpp` y 16 en `zEAXSound2.cpp`).
* Lo segundo es **orden de parseo**, y la raíz está medida: nuestro TU abre
  `Generated/AttribSys/Classes/pvehicle.h` como cabecera **161** y `simsurface.h` como la
  **202**; el objetivo al revés (`simsurface` índice 13, `pvehicle` 31). La cadena de
  include la da `agent_eax58_cadena.py`:

      zEAXSound.cpp > EAXSound.cpp > EAXSOund.hpp > FEDatabase.hpp > RaceDB.hpp
        > IAI.h > AIAvoidable.h > IVehicle.h > pvehicle.h            (161)
                                             > ISimable.h > IModel.h > Bounds.h
                                               > SimSurface.h > simsurface.h    (202)

  O sea: **lo decide `Interfaces/Simables/IVehicle.h`, que incluye `pvehicle.h` antes que
  `ISimable.h`.** No es mío y lo incluye media docena de unidades, así que no lo he tocado:
  queda con su cadena y su cifra.

El orden del objetivo, para quien lo ataque, es reproducible con tres includes seguidos:
`Sim/SimSurface.h` (→ `simsurface`, `TAttrib`), `Camera/CameraMover.hpp` (→ `ecar`,
`camerainfo`) y `EAXSound/SoundConn.h` (→ `effects`, `audioimpact`, `audioscrape` y los diez
paquetes de `WorldConn`), y luego ya `FEDatabase.hpp`.

---

## 5. Un negativo MEDIDO y REVERTIDO, con su comentario en el fuente

`"Audio Memory Pool"` es la primera cadena de **cuerpo** de todo el TU y en el objetivo va
en el índice **40** del `.rodata`, detrás de las 35 de **parseo** que trae `EAXSOund.hpp`
(que termina justo en `Attrib::Gen::audiosystem`, índice 39). En el nuestro va en el **6**,
porque `AudioMemoryManager.cpp` es el primer fichero de la SourceList y no incluye
`EAXSOund.hpp`.

Probé la línea evidente —`#include "./EAXSOund.hpp"` la primera de
`AudioMemoryManager.cpp`, fichero **mío**—:

| | antes | después |
|---|---:|---:|
| `rodorden` primer `insert` | `GAMECUBE` + `Audio Memory Pool` | **sólo `GAMECUBE`** ✔ |
| `fncmp` | 0 de 773 | 0 de 773 ✔ |
| secciones (con `keep.lst` **privado y corregido**) | `bss+32` | `bss+32` ✔ |
| DOL, sólo `zEAXSound` sustituida | 34.338 | **34.342 (+4 PEOR)** |
| entradas `$LC` que renumera | 0 | **27** |

**Revertido**, y el negativo escrito en la cabecera de `AudioMemoryManager.cpp` con la
razón: mientras el bloque de parseo siga en el orden equivocado (§4) y delante sigan los 12
B del `GAMECUBE` duplicado, mover una sola cadena no alinea nada y sólo ensucia `keep.lst`
para todo el proyecto.

**Y una trampa de medida que casi me la cuela**: con el `keep.lst` compartido, `linkdelta`
daba `rodata-128` para esa edición. Era mentira —`keep.lst` rancio, 27 `$LC` renumerados—.
Con la copia privada corregida (`agent_eax58_privkeep.py`, control `BASE == DOL original` =
`True`) la sección salía `bss+32`, igual que antes. **Ninguna edición que mueva una cadena se
puede juzgar con el `keep.lst` compartido.**

---

## 6. `GenerateRoadNoise`: mirada, y NO tocada

`previo.py GenerateRoadNoise` da cuatro bloques (r27/r29/r46/r47/r49/r50) y un `UNSOLVED`
explícito: 1.240 B al 97,11 %, ~20 filas de **planificador y rotación de FPR**, con las dos
acumulaciones intercambiadas. La r57 volvió a medirlo y el diagnóstico coincidía sin aportar
nada nuevo. **No la he tocado: sigue sin haber palanca nueva y son cinco negativos.** Es la
única función de mis dos unidades que no casa (`fncmp` 1 de 930).

---

## 7. Sellos, controles y `lcfix`

    zEAXSound.o    f06ad54f… (inicio)  ->  ca5b0acf… (fin)
    zEAXSound2.o   40f74cce… (inicio)  ->  cec2c6a3… (fin)

**El `sha1` cambia y el contenido NO.** Las dos causas, las dos comprobadas:

* `zEAXSound.o`: el árbol lleva `-gdwarf+`, así que **21 líneas de comentario mueven
  `.line`/`.debug`**. Comparadas sección a sección contra el objeto compilado ANTES de tocar
  nada (guardado en el scratchpad), `.text`, `.rodata`, `.data`, `.bss` y `.ctors` salen
  **byte a byte idénticas** (`da8aa2352b99`, `2e6a384707d2`, `af5aaf35f574`, …).
* `zEAXSound2.o`: **no es mío**. No he tocado ni un fichero que incluya; cambió a mitad de
  ronda porque otro agente editó una cabecera compartida (mismo fenómeno que la r57 §8).

Tres compilaciones seguidas dan el mismo `sha1` en las dos. Y el sello que de verdad cierra:
**`trypromo` de las dos unidades da `DOL ROTO 72487037a9d4`, el mismo hash exacto con que
cerró la r57** — la contribución de mis dos unidades al DOL es bit a bit la que había.

`lcfix.py zEAXSound zEAXSound2 --check`: **todas las entradas al día**.
`lcfix.py --check` global: **166 pendientes, 94 de `zLua`, 71 de `zMain` y 1 de `zAnim`.
NINGUNA es mía.**

---

## 8. Herramientas nuevas

| script | qué hace |
|---|---|
| `agent_eax58_bssalign.py <u> [sec]` | alinea por NOMBRE los símbolos de una sección en el ENLACE base/promo y dice **dónde salta el delta**. |
| `agent_eax58_bssvivos.py <u>` | qué símbolos del objeto **sobreviven** al enlace y a qué dirección: acaba con el modelo de estripado a ojo. |
| `agent_eax58_bssmap.py <u>` | cada estática de `.bss` con la FUNCIÓN que la usa, resolviendo también los relocs contra el símbolo de SECCIÓN con addend. |
| `agent_eax58_probe_bss.py <u> N…` | **función de transferencia** ventana→sección: compila copias con N bytes de `.bss` de más en el scratchpad, sin tocar el árbol. |
| `agent_eax58_doldelta.py <u> obj…` | bytes distintos del DOL con un `.o` cualquiera sustituido: pone precio a un cambio de tamaño. |
| `agent_eax58_privkeep.py <u>…` | mide con una **copia privada de `keep.lst`** ya corregida: la única forma de juzgar una edición que mueve cadenas sin pisar a los demás. |
| `agent_eax58_attrord.py <f.i>` | orden de PARSEO de los `Attrib::*` con `-E`, y qué cabecera trae cada uno. |
| `agent_eax58_cadena.py <f.i> h…` | la CADENA DE INCLUDE completa con la que entra una cabecera. |
| `agent_eax58_edit.py` | pone/quita un bloque al principio de un fichero respetando CRLF/LF. |

---

## 9. Propiedad y regresiones

**Ninguna regresión.** El único fichero de fuente modificado es
`src/Speed/Indep/Src/EAXSound/AudioMemoryManager.cpp` y sólo lleva **un bloque de
comentario** (el negativo de §5); ese fichero **no lo incluye ninguna otra unidad**, así que
la regla 5 no aplica y no hay unidad ajena que medir. `linkdelta`, `fncmp`, `dolwhere` y
`trypromo` salen idénticos al principio y al final.

**Lo que le toca al jefe**, por orden de valor:

1. **La raya de `.bss` de `splits.txt`** (§0): `0x8045B268 → 0x8045B1F0`. Atómica con los
   ~104 B de `.bss` anónimo que pongo yo en `zEAXSound.cpp` en cuanto la raya se mueva.
   Cierra la única sección abierta de `zEAXSound` y vale del orden de **1.900 B de DOL**
   (§2). **Toca a `zEagl4Anim`** (pasaría de −148 a −28 de `.bss`), así que hay que avisar a
   `eagl`.
2. **Las tres entradas de `keep.lst`** de la r57 §6.1 (`GAMECUBE` ×2 y `bad_alloc`): son la
   PRIMERA divergencia del `.rodata` de **las dos** unidades (§4).
3. `Interfaces/Simables/IVehicle.h`: `pvehicle.h` antes que `ISimable.h` es lo que
   descoloca el bloque `Attrib::Gen::*` de las dos unidades (§4). No es mío y lo incluyen
   varias unidades.

**Y una corrección a la r57**: su §6.2 proponía dejar de emitir tres `_GetKind` por ser
«DUEÑO». **No lo son**: sus tres cadenas están en el `.rodata` del objetivo de `zEAXSound`
(índices 57, 75 y 196), así que el original las emitía. Quitarlas habría cerrado la sección
por casualidad y alejado el contenido.
