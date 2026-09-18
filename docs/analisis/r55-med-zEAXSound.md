# r55 — medida de `zEAXSound` (151.092 B): **ORDEN PURO**, y la palanca es al revés que en zMain

Unidad: `Speed/Indep/SourceLists/zEAXSound`. Reconstruida con
`python scripts/build_direct.py Speed/Indep/SourceLists/zEAXSound` → `2 ok, 0 fallidas`
al primer intento. Nada de fuente tocado: ronda de medida.

## Veredicto en una línea

**No queda ni una palabra de contenido que escribir.** Los 134.216 B de `.text` que caen en
otra dirección los explica un solo mecanismo: **19 funciones que suman EXACTAMENTE 2.192 B
que el objetivo emite en la COLA de la unidad (la cola de `saved_inlines` que drena
`finish_file`) y que nosotros emitimos PRONTO, en el punto de su definición fuera de clase.**

Y el «cambio de dueño» que `movidos.py` le atribuye (−22.524 B de `.text`) **no existe**:
está verificado símbolo a símbolo y vale **6 B**.

---

## 1. `reorden.py zEAXSound` — ¿orden o contenido?

```
seccion     simbs palabras   crudas  CONTENIDO  sin par
.ctors          0        0        0          0        0
.data         118      370       85          1       28
.rodata       672     1444      361         25      595
.text         772    35452     3557          0        0
```

* **`.text`: CONTENIDO 0 y `sin par` 0.** 772 de 772 símbolos emparejados, 35.452 palabras,
  3.557 diferencias crudas y **cero** sobreviven al enmascarado. El código está escrito.
* `.rodata` 25 + `.data` 1 = **26 palabras que reorden llama CONTENIDO. Son falsos
  positivos, y está comprobado**: ver «Sorpresa 1».
* `.rodata` `sin par` 595 = los `lbl_xxxxxxxx` anónimos del troceado contra nuestros `$LC`
  (6.759 B). Es el aviso que ya trae el propio `reorden.py`, no un fallo.
* `.data` `sin par` 28 = 426 B: 9 `lbl_` anónimos (214 B), 17 `gap_06_*_data` y **dos
  estáticas de función renumeradas** (`PlayerUpgrade.14976`, `counter.33705`). Es todo lo
  que hay detrás del `data-384` de `linkdelta`.

**VEREDICTO: ORDEN_PURO** (con la salvedad literal de que `reorden` imprime 26, no 0).

---

## 2. `movidos.py Speed/Indep/SourceLists/zEAXSound`

```
zEAXSound: 39624 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
   15434 cambian de direccion, pero 15031 es ARRASTRE (tres terminos: donde
   empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
   cambia de dueno).
   .bss: base-480 delta+288, .data: base-96 delta-400, .rodata: base+0 delta+432,
   .sbss: base-448 delta+0, .sbss2: base-448 delta+0, .sdata: base-448 delta+0,
   .sdata2: base-448 delta+0, .text: base+0 delta+22524

   CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
      .bss       nuestro .o    +288, seccion enlazada     +32  ->     -256 de otro
      .data      nuestro .o    -400, seccion enlazada    -384  ->      +16 de otro
      .rodata    nuestro .o    +432, seccion enlazada    -104  ->     -536 de otro
      .text      nuestro .o  +22524, seccion enlazada      +0  ->   -22524 de otro

   ESCALONES SIN EXPLICAR (8): un desplazamiento que comparten muchos
   simbolos es una TRASLACION -- su orden relativo no cambia --, asi que
   NO se cuenta como permutacion. Que exista uno es la firma de que un
   simbolo cambio de DUENO: otro objeto tambien cambio de tamano.
      .data           -504   2300 simbolos
      .text          +2192    282 simbolos
      .text          +1356     88 simbolos
      .text          +1760     47 simbolos
      .bss            -344     40 simbolos
      .text           +516     38 simbolos
      .text          +1308     33 simbolos
      .text          +2316     20 simbolos

**403 simbolos DE VERDAD permutados** (desplazados respecto a su seccion)

seccion       movidos      bytes
.text             221     29,396
.rodata            70      4,868
.data              99      1,640
.bss               10        132
?                   3          0
```

**Permutaciones reales: 403.** Pero los «escalones sin explicar» de `.text` **no** son firma
de cambio de dueño aquí: son las sumas parciales acumuladas de las 19 funciones de la cola
(ver §4). `+1308` = el constructor de `EAX_CarState` solo; `+1356` = ése más
`Clear__10stBankSlot`; `+2192` = los 19 enteros.

### El M4 medido de verdad: 6 B, no 22.524

De los **229 símbolos que nuestro `.o` define y el objeto original no** (24.650 B, de los
que 24.316 B son `.text`):

| | símbolos | bytes |
|---|---:|---:|
| definidos también por otro objeto del enlace | 58 | 6.990 |
| **presentes en el ELF enlazado** | **36** | **7.054** |
| **estripados por el enlazador (nadie los referencia)** | **193** | **≈17.600** |
| presentes en ambos enlaces y que **cambian de dirección** | **5** | 2.094 |
| **que cambian de DUEÑO de verdad** | **2** | **6 B** |

Los dos únicos cambios de dueño reales:

```
   2 B  _Q26Speech7Manager.m_frameindex            804359B4 -> 80417768   (de zSpeech a nosotros)
   4 B  _Q43UTL3COMt7Factory3ZRCQ23Sim14Connect... 8043589C -> 80417764
```

Los otros 3 (`restartstream`/`parsechunks`/`startnextrequest`, 2.088 B, +2192 B) **no**
cambian de dueño: caen dentro de nuestra propia ventana `.text` en los dos enlaces. Lo que
cambia es la **ligadura**: el objeto original los define como símbolos LOCALES (eran
`static`) y nosotros como globales.

Y en sentido inverso: los 718 símbolos que el objetivo define y nosotros no (7.525 B) son
**todos** `lbl_*` y `gap_*` del troceado — cero `.text`. **No hay M4 en ninguna dirección.**

---

## 3. `linkdelta`, `permorden`, `trypromo`

```
Speed/Indep/SourceLists/zEAXSound                  +0   rodata-104 data-384 bss+32
```

```
zEAXSound: objetivo 773 funciones, nuestro 997, comunes 773
solo nuestras (las estripa el enlazador): 224
en su sitio (subsecuencia creciente mas larga): 676 de 773
DESPLAZADAS: 97
ciclos no triviales: 7  (longitudes: 259, 172, 145, 134, 7, 3, 2)
```

```
Speed/Indep/SourceLists/zEAXSound            DOL ROTO (9b8ece0832d5)
```

El histograma de **salto de índice** es la firma más limpia de todo esto:

```
     +19   290 funciones      <- casi el 40 % del .text desplazado exactamente 19 posiciones
      +2    89
      +0    51
     +10    51
     +17    49
```

**290 funciones desplazadas +19 posiciones**: son 19 funciones que se adelantan y empujan a
todo lo que queda detrás.

Bytes de `.text` en otra dirección: **134.216 B de 772 símbolos comunes** (sólo 43 en su
sitio). Reparto por escalón:

| desplazamiento | símbolos | bytes |
|---:|---:|---:|
| **+2192** | **282** | **52.704** |
| +1356 | 88 | 16.440 |
| +1760 | 47 | 11.896 |
| +1308 | 33 | 10.444 |
| +516 | 38 | 9.336 |

---

## 4. El símbolo dominante: **19 funciones que suman 2.192 B exactos**

El objetivo las emite en la cola de la unidad (`800C86A8`–`800CA744`, el último 6 % de la
ventana `.text` `800A6128`–`800CAF5C`). Nosotros las emitimos repartidas por delante.

```
   obj 800C86A8  nue 800B5878      8 B  Compress__CQ23Sim6PacketPQ23Sim6Packet
   obj 800C86B0  nue 800B5880      8 B  Decompress__CQ23Sim6PacketPQ23Sim6Packet
   obj 800C8788  nue 800AEF10     80 B  Update__Q25Sound10AudioEventRC8bVector3N21f
   obj 800C87D8  nue 800AF7B0      4 B  Pause__Q25Sound14CollisionEventb
   obj 800C87E8  nue 800B5888     84 B  _._Q25Sound5Wheel
   obj 800C899C  nue 800A7EA4   1308 B  __12EAX_CarStatePCQ26Attrib10CollectionQ25Sound7ContextUiP10HSIMABLE__
   obj 800C9098  nue 800AAC8C     48 B  Clear__10stBankSlot
   obj 800C94D0  nue 800B4388    100 B  ConnectionClass__Q29SoundConn15Pkt_Car_Service
   obj 800C9534  nue 800B43EC      8 B  Size__Q29SoundConn15Pkt_Car_Service
   obj 800C953C  nue 800B43F4     32 B  Type__Q29SoundConn15Pkt_Car_Service
   obj 800C9590  nue 800B446C    100 B  ConnectionClass__Q29SoundConn16Pkt_Heli_Service
   obj 800C95F4  nue 800B44D0      8 B  Size__Q29SoundConn16Pkt_Heli_Service
   obj 800C95FC  nue 800B44D8     32 B  Type__Q29SoundConn16Pkt_Heli_Service
   obj 800CA3A4  nue 800B1108     40 B  Alloc__17CSISCoreAllocatorUiPCcUi
   obj 800CA3CC  nue 800B1130     64 B  Alloc__17CSISCoreAllocatorUiPCcUiUiUi
   obj 800CA40C  nue 800B1170     40 B  Free__17CSISCoreAllocatorPvUi
   obj 800CA59C  nue 800B4414     88 B  SType__Q29SoundConn15Pkt_Car_Service
   obj 800CA5F4  nue 800B44F8     88 B  SType__Q29SoundConn16Pkt_Heli_Service
   obj 800CA744  nue 800B1198     52 B  Alloc__12PF_AllocatorUiRCQ22EA12TagValuePair
                                 -----
                                 2.192 B
```

**El dominante individual es `EAX_CarState::EAX_CarState`, 1.308 B — el 60 % de los
2.192 B** —, y además el primero de la cadena: se desplaza **−133.880 B** (objetivo
`800C899C` → nuestro `800A7EA4`, índice 646 → 42).

**Cuánto explica:** el escalón `+2192` afecta a 282 de 772 funciones = **52.704 B**; con sus
dos sumas parciales (`+1308`, 10.444 B y `+1356`, 16.440 B) la cadena explica **79.588 de
los 134.216 B** desplazados = **59 %**. Sólo el constructor (los 1.308 B que están dentro de
cada uno de esos tres escalones) responde de **79.588 × 1308/2192 ≈ 47.500 B**.

### Por qué se adelantan: no son plantillas, son **inlines**

Once de los diecinueve están en una vtable de esta misma unidad
(`Compress`/`Decompress` en `_vt.Q23Sim6Packet`, `Alloc`/`Free` en
`_vt.17CSISCoreAllocator` y `_vt.12PF_Allocator`, `ConnectionClass`/`Size`/`Type` en
`_vt.Q29SoundConn15Pkt_Car_Service` y `..16Pkt_Heli_Service`, `Update` en
`_vt.Q25Sound10AudioEvent`, `Pause` en `_vt.Q25Sound14CollisionEvent`). O sea: **el
original las tenía DENTRO de la clase** — `inline` — y GCC 2.9 las manda a `saved_inlines`
por `mark_inline_for_output` (`cp/decl2.c:2140`), que `wrapup_global_declarations`
(`cp/decl2.c:3795`) vuelca al final desde `finish_file`. Se emiten porque la vtable las
referencia, pero **al final**.

En nuestro árbol están **fuera de clase, en el `.cpp`**, y por eso salen en el punto de
parseo:

| símbolo | dónde lo definimos |
|---|---|
| `EAX_CarState::EAX_CarState` (1.308 B) | `src/Speed/Indep/Src/EAXSound/EAXSound.cpp:862` (declarado en clase en `EAXCarState.hpp:30`) |
| `Sim::Packet::Compress` / `Decompress` | `src/Speed/Indep/Src/EAXSound/SoundConn.cpp:270` / `:272` |
| `Pkt_Car_Service::ConnectionClass/Size/Type/SType` | `src/Speed/Indep/Src/EAXSound/SoundConn.cpp:53,58,62,66` |
| `Pkt_Heli_Service::ConnectionClass/Size/Type/SType` | `src/Speed/Indep/Src/EAXSound/SoundConn.cpp:71,76,80,84` |
| `Sound::AudioEvent::Update` | `src/Speed/Indep/Src/EAXSound/SoundCollision.cpp:23` |
| `Sound::CollisionEvent::Pause` | `src/Speed/Indep/Src/EAXSound/SoundCollision.cpp:159` |
| `Sound::Wheel::~Wheel` | `src/Speed/Indep/Src/EAXSound/EaxSoundTypes.cpp:20` |
| `stBankSlot::Clear` | `src/Speed/Indep/Src/EAXSound/EAXAemsManager.cpp:288` |
| `CSISCoreAllocator::Alloc` ×2 / `Free` | `src/Speed/Indep/Src/EAXSound/EAXSND8Wrapper.cpp:232,238,244` |
| `PF_Allocator::Alloc` | `src/Speed/Indep/Src/EAXSound/EAXSND8Wrapper.cpp:249` |

La prueba de que la cola es de `inline` y no de plantillas: la cola del objetivo mezcla estos
diecinueve con `ClassKey__Q36Attrib3Gen6speech`, `Clear__18stAssetDescription`,
`Clear__19stSndDataLoadParams` y los `push_back__Q23UTLt6Vector2Z...` — **y ésos ya salen en
la cola en nuestro build**, en el orden correcto. Sólo faltan los 19.

**Esto es la palanca de la r54 AL REVÉS.** En `zMain` había que ADELANTAR con un primer
`static void _prime(...)`. Aquí hay que **RETRASAR**, y para eso no sirve un primer: hay que
devolver la definición al cuerpo de la clase (o marcarla `inline`), que es lo que la mete en
`saved_inlines`.

---

## 5. Lo que NO domina

* **`.rodata`**: 76 de 77 símbolos con nombre en otra dirección, 5.684 B, en escalones
  pequeños y repartidos (−104 ×6, −168 ×6, +348 ×4, +320 ×4…). **Difuso.** El
  `linkdelta rodata-104` no tiene un dueño único.
* **`.data`**: 90 de 90 en otra dirección, 1.482 B; `linkdelta data-384` y 426 B de símbolos
  sin par, todos `lbl_`/`gap_`/estáticas renumeradas. Difuso también.
* **Cambio de dueño**: 6 B. Cero peso.

---

## Sorpresas (contradicen el brief)

### 1. `reorden.py` NO sabe enmascarar reubicaciones de DATOS: sus 26 «palabras de CONTENIDO» son falsos positivos

`mascara()` sólo entiende codificaciones de INSTRUCCIÓN (formas D, `b`, `bc`). Una palabra
de `.rodata`/`.data` que es un puntero de 32 bits (`R_PPC_ADDR32`) cae en el `else` y se
compara ENTERA — o peor: una dirección `0x800BFADC` tiene `op = v>>26 = 32`, que **está en
`DFORM`**, así que se compara sólo su mitad alta. Dos direcciones con la misma mitad alta se
ocultan y las demás se cuentan como contenido.

Verificado a mano sobre los 9 símbolos que `reorden` señala (`_vt.14SFXCTL_Physics`,
`_vt.Q29SoundConn16Pkt_Heli_Service`, `_vt.Q29SoundConn15Pkt_Car_Service`,
`_vt.Q25Sound14CollisionEvent`, `_vt.Q25Sound10AudioEvent`, `_vt.Q23Sim6Packet`,
`_vt.12PF_Allocator`, `_vt.17CSISCoreAllocator`, `_14SFXCTL_Physics.s_TypeInfo`): **40
palabras crudas distintas, 39 apuntan al MISMO símbolo en los dos enlaces.** La 40.ª
(`_14SFXCTL_Physics.s_TypeInfo+4`) apunta a `lbl_803D74B0` en el objetivo y a `$LC861` en el
nuestro, y **las dos cadenas son `"SFXCTL_Physics"`**.

**Contenido real de `zEAXSound`: 0 palabras en las tres secciones.** El criterio literal del
brief («0 en las tres») habría marcado esta unidad como MIXTO y habría repartido trabajo de
fuente que no existe. **Es un fallo de herramienta, no de esta unidad: afecta a las trece.**
Arreglo: en `mascara()`, decidir por la SECCIÓN, y para `.rodata`/`.data` resolver la palabra
a `(símbolo, desplazamiento)` en cada enlace y comparar eso.

### 2. La sección `CAMBIO DE DUEÑO (M4)` de `movidos.py` **no descuenta lo que el enlazador estripa**

Anuncia `−22.524 B de otro` en `.text`. La realidad medida: de los 229 símbolos que sólo
define nuestro objeto, **193 (≈17.600 B) no llegan al enlace** —nadie los referencia y
`ngcld` los tira— y de los 36 que llegan **sólo 2 cambian de dueño, 6 B**. La resta
«tamaño de nuestro `.o`» − «tamaño de la sección enlazada» supone implícitamente que todo lo
que aporta el objeto sobrevive, y con `-strip-unused-data` eso es falso en cuanto la unidad
instancia plantillas de más (aquí 224 funciones sobrantes según `permorden`).

El M4 **verdadero** se mide comparando el DUEÑO (la ventana en la que cae el símbolo) en los
dos enlaces, no restando tamaños. Cuidado con esto en el reparto: `zMisc` tenía 439.304 B
atribuidos a M4 por la misma resta.

### 3. `build_direct.py Speed/Indep/SourceLists/zEAXSound` compila **también `zEAXSound2`**

Empareja por prefijo. Con trece agentes en paralelo eso pisa el `.o` de quien lleve
`zEAXSound2`. Avisado.

### 4. `permorden` dice «solo nuestras (las estripa el enlazador): 224» y acierta

224 funciones sobrantes, y el enlace confirma que 193 de los 229 símbolos extra
desaparecen. Esa línea es una medida buena y `movidos` no la está usando.

---

## Siguiente paso concreto

**Mover el cuerpo de `EAX_CarState::EAX_CarState` de
`src/Speed/Indep/Src/EAXSound/EAXSound.cpp:862` al interior de la clase, en
`src/Speed/Indep/Src/EAXSound/EAXCarState.hpp`, sustituyendo la declaración de la línea 30**
(`EAX_CarState(const Attrib::Collection *atr, Sound::Context context, WUID wuid, HSIMABLE handle);`).
El resto de la clase ya está definido en clase (`Find`, `~EAX_CarState`, `GetState`,
`GetMaxRPM`…), así que es consistente con lo que hacía el original.

**Evidencia de que ahí va:** el objetivo lo emite en `800C899C`, entre
`push_back__Q23UTLt6Vector2ZP12EAX_CarStatei16RCP12EAX_CarState` (`800C8848`) y
`push_back__Q23UTLt6Vector2ZP13EAX_HeliStatei16RCP13EAX_HeliState` (`800C8EB8`) — dentro de
la cola de `finish_file`, rodeado de `ClassKey__Q36Attrib3Gen*` y `Clear__18stAssetDescription`,
que en nuestro build **ya** salen ahí. Nosotros lo emitimos en `800A7EA4`, entre
`PlayFEMusic__8EAXSoundi` y `Update__8EAXSoundf`, o sea exactamente donde está su definición
fuera de clase dentro de `EAXSound.cpp`. Una posición de definición, no de uso.

Predicción falsable, para que la siguiente ronda pueda comprobar el mecanismo con una sola
compilación: **si el mecanismo es el que digo, los escalones `+2192`, `+1356` y `+1308` bajan
en 1.308 B cada uno** (a `+884`, `+48` y `0`), las funciones «en su sitio» suben de 43 a ≥76
y el residuo de `.text` cae en el orden de 45–50 kB. Si no bajan exactamente 1.308, el
mecanismo no es `saved_inlines` y hay que volver a mirar.

Después, los 18 restantes en un solo commit (cinco ficheros: `SoundConn.cpp`,
`SoundCollision.cpp`, `EAXSND8Wrapper.cpp`, `EaxSoundTypes.cpp` y `EAXAemsManager.cpp`), y
sólo entonces mirar `.rodata`/`.data`, que están difusos y sólo valen 7.166 B entre las dos.

**Coste estimado: una ronda.** El diagnóstico está cerrado; lo que queda es mover diecinueve
cuerpos y medir el orden DENTRO de la cola, que puede no salir a la primera.
