# r49 — `linked`: la bandera no era la palanca, y el frente cambia de seccion

Territorio: las 30 unidades con el codigo al 100 % que no promocionan
(1.788.644 B), el barrido de `-fno-implicit-templates` y el predicado de
promocion.

## Resultado en una linea

**El barrido esta hecho y sale negativo por los dos lados**: la bandera es
insegura en 12 de las 13 SourceLists (pierde simbolos y el enlace se cae), y en
la unica donde es segura **no cambia ni un byte del DOL**. Medido:
`zFeOverlay` con y sin la bandera —5.880 B y 47 simbolos de diferencia en el
objeto— produce **el mismo DOL, sha1 `d2e208d4dd88`**, y las mismas secciones
enlazadas.

Con eso cae el modelo entero de la r48: los simbolos de mas **no bloquean nada**,
el `_M_erase` que «bloqueaba veinte unidades» **no llega al ELF enlazado**, y
`zBWare` deja de ser una anomalia del predicado. El frente de `linked` no esta en
`.text`: esta en `.rodata` (y en `.data`/`.bss`).

**Dos promociones demostradas** (`libc/kf_rem_pio2` + `libc/ef_rem_pio2`,
3.028 B, DOL `9619ba57c9919f95f7f2ac951a2166a3517f91e3`) y **`zFe2` vuelve a
enlazar** tras corregir 16 alias mangled caducados.

---

## 1. El barrido de `-fno-implicit-templates`, unidad por unidad

Compilado a mano con los cflags reales de cada unidad sacados de `build.ninja`
(arnes `scratchpad/r49_linked/hh.py` + `sweep.py`), a un `.o` del scratchpad: no
se ha tocado `configure.py` ni se ha lanzado ningun `ninja`. Base y variante
compiladas las dos por mi, para que el «antes» y el «despues» sean el mismo
arbol.

| unidad | sec | antes | despues | objetivo | fn con CODIGO distinto | simbolos que FALTAN | simbolos de mas |
|---|---|---:|---:|---:|---|---|---|
| `zAI` | .text | 278.456 | 256.496 | 272.796 | 0 → **119** | 0 → **119** | 114/5.660 B → 83/2.716 B |
| `zFe2` | .text | 262.808 | 255.528 | 250.732 | 0 → **2** | 1 → **3** | 301/12.076 B → 244/5.140 B |
| `zFe` | .text | 187.568 | 180.888 | 174.200 | 0 → **1** | 0 → **1** | 185/13.368 B → 131/6.744 B |
| `zPhysics` | .text | 188.664 | 155.308 | 146.124 | 0 → **248** | 0 → **248** | 200/42.540 B → 102/31.192 B |
| `zMain` | .text | 177.620 | 165.164 | 159.776 | 0 → **34** | 0 → **34** | 165/17.844 B → 98/9.000 B |
| `zEAXSound` | .text | 173.572 | 158.548 | 151.092 | 0 → **68** | 0 → **68** | 224/22.480 B → 158/14.496 B |
| `zGameplay` | .text | 153.616 | 122.468 | 141.472 | 0 → **103** | 1 → **104** | 196/12.160 B → 133/4.204 B |
| `zFeOverlay` | .over | 142.528 | 142.528 | 141.224 | **0 → 0** | **0 → 0** | 49/1.304 B → 49/1.304 B |
| `zSim` | .text | 112.980 | 97.044 | 96.400 | 0 → **55** | 0 → **55** | 158/16.580 B → 94/7.972 B |
| `zLua` | .text | 111.700 | 88.076 | 94.264 | 0 → **114** | 0 → **114** | 159/17.436 B → 107/10.964 B |
| `zMisc` | .text | 84.896 | 76.008 | 78.008 | 0 → **7** | 0 → **7** | 91/6.888 B → 41/536 B |
| `zAnim` | .text | 49.896 | 44.508 | 42.292 | 0 → **1** | 0 → **1** | 188/7.604 B → 144/2.360 B |
| `zGameModes` | .text | 620 | 620 | 124 | 0 → 0 | 0 → 0 | 18/496 B → 18/496 B |

Lecturas:

- **La columna que decide es «FALTAN»**, y sale distinta de cero en **12 de 13**.
  En esta tabla «fn con codigo distinto» y «faltan» coinciden porque la unica
  diferencia que introduce la bandera es que la funcion deja de emitirse: no
  cambia ni una instruccion de ninguna funcion que siga estando.
- **No es un aviso teorico: el enlace se cae.** Enlazando de verdad con el objeto
  de la variante:
  ```
  zSim  = zSim_nit.o   ENLACE FALLA: _algo.h(289): L0039: Reference to undefined
                       symbol find_if__H2ZPQ53UTL11Collectionst11GarbageNode2...
  zAnim = zAnim_nit.o  ENLACE FALLA: AnimEntity_WorldEntity.cpp(328): L0039:
                       Reference to undefined symbol AddSorted__t6bTList1Z6bPNode...
  ```
- **`zFeOverlay` es la unica segura, y ya la lleva puesta** desde la r48
  (`configure.py:707`). Por eso «antes» y «despues» son identicos: la base ya
  incluye la bandera.
- **`zGameModes` no tiene ni una instanciacion implicita**: la bandera no le
  cambia un byte.
- Dos unidades pierden simbolos **ya en la base**, sin bandera ninguna:
  `zGameplay` (1) y `zFe2` (1). En `zGameplay` faltan ademas, con la bandera,
  las seis `CreateObjects__H1Z...GObjectBlock` de 412 B y sus
  `DeleteObjects__H1Z...` de 168 B, que el objetivo **si tiene**: son
  instanciaciones que el original emite en esta unidad.

**Conclusion del barrido: no hay ninguna unidad nueva a la que aplicarle la
bandera.** Para las otras doce haria falta instanciar explicitamente entre 1 y
248 plantillas por unidad, y el resto del informe explica por que eso no
compraria nada.

---

## 2. La bandera no cambia el DOL — la medida que cierra el frente

Es la medida que faltaba en la r48: alli se comparo el **objeto**, no el enlace.

`zFeOverlay`, dos objetos compilados por mi con los cflags reales, uno con la
bandera y otro con ella filtrada, cada uno colocado en su propio directorio con
el **basename exacto** (el `ldscript` empareja por nombre de fichero:
`*zFEOverlay.o(.over .rodata)`; con el `.o` renombrado la `.rodata` se va a otra
seccion de salida y la medida sale falsa):

| objeto | `.over` | funciones | DOL del enlace | secciones del ELF contra la base |
|---|---:|---:|---|---|
| sin `-fno-implicit-templates` | 148.408 | 563 | `d2e208d4dd88` | `.over-1136 .bss-4096 .sbss+4` |
| con `-fno-implicit-templates` | 142.528 | 516 | **`d2e208d4dd88`** | **`.over-1136 .bss-4096 .sbss+4`** |

**5.880 B y 47 simbolos de diferencia en el objeto, cero bytes de diferencia en
el DOL.** El enlazador (`ngcld -strip-unused-data`) estripa los huerfanos a
nivel de simbolo, y lo hace igual de bien con la bandera que sin ella.

Lo mismo dice `linkdelta.py` sobre las 23 unidades sin promocionar: **`.text` a
delta 0 en 20 de ellas** (§4). Todo el exceso de codigo —287.372 B segun el
censo de la r48— se estripa entero.

---

## 3. El `_M_erase` no bloquea nada, y `zBWare` deja de ser una anomalia

El «objetivo individual mas valioso del frente» era
`_M_erase__Q24_STLt8_Rb_tree5ZPQ26Hermes13_h_HHANDLER__...` (104 B, 19-20
unidades). Comprobado en el ELF **enlazado**:

```
en el ELF de la base (el que casa byte a byte):   NO esta
en el enlace con nuestro zSim sustituido:          NO esta
en el objeto build/GOWE69/src/.../zSim.o:          SI, 104 B
```

El enlazador se lo lleva. **No bloquea ninguna unidad.**

Y lo mismo pasa con el fallo del predicado. `zBWare` enlaza teniendo 8 simbolos
«vivos» de 236 B; el predicado los llama vivos porque una reubicacion **de
nuestro propio objeto** apunta a ellos. Comprobado uno a uno en el ELF enlazado:

```
GetCodeineString__FPcii                  100 B  interno   en el ELF: NO
_._11bFunkServer                          68 B  interno   en el ELF: NO
_._19bFunkServerPlatform                  48 B  interno   en el ELF: NO
Service__19bFunkServerPlatform             4 B  interno   en el ELF: NO
Service__11bFunkServer                     4 B  interno   en el ELF: NO
DeliverPacket__19bFunkServerPlatform...    4 B  interno   en el ELF: NO
DeliverPacket__11bFunkServer...            4 B  interno   en el ELF: NO
CanDeliverPacket__11bFunkServerUi          4 B  interno   en el ELF: NO
```

**Los ocho se estripan.** El predicado no falla en `zBWare`: falla en el modelo.
Su test de «referenciado desde dentro» **no es transitivo desde raices vivas**:
una vtable muerta que apunta a un destructor muerto los marca a los dos como
vivos, y el enlazador tira el grupo entero. Por eso el predicado acertaba 20 de
21: **acertaba por correlacion** (las unidades con muchos simbolos de mas suelen
tener tambien mas descuadre de datos), no por causa.

> **La conclusion, para el brief: `promopred.py` mide algo que no es el bloqueo.**
> El juez es `trypromo.py`, y el diagnostico barato es `linkdelta.py`, que ya
> estaba escrito y nadie estaba mirando.

---

## 4. Donde esta el bloqueo de verdad

`python scripts/linkdelta.py` (2,6 s por unidad; enlaza el proyecto entero
sustituyendo una unidad y compara las secciones del ELF contra la base):

```
unidad                .text   resto
zSim                     +0   IGUAL
zGameModes               +0   rodata+104
zEcstasy                 +0   rodata-288 data+64 bss+32
zSpeech                  +0   rodata-624
zPhysics                 +0   rodata-784 bss-32
zEagl4Anim               +4   rodata-864 data-96 bss-128
zWorld2                  +0   rodata-808 data-256 bss+64
zAnim                    +0   rodata-1240 data-192 over-16
zEAXSound2               +0   rodata-1184 data-288 bss-32
zPlatform                -8   rodata-1408 data-64
zAI                      +0   rodata-1344 data-384
zMain                    +0   rodata-1720 data-32
zCamera                 +16   rodata-1384 data-224 bss+32
zEAXSound                +0   rodata-1544 data-384 bss+32
zLua                     +0   rodata-1648 data-320
zTrack                   +0   rodata-1784 data-416 bss-160
zPhysicsBehaviors        +4   rodata-936 data+416 bss-988
zFe                      +0   rodata-2456 data-32
zGameplay                +0   rodata-2656 data-128
zMisc                    +0   rodata+2840 data+32
zWorld                   -4   rodata-4136 data-128
zFe2                     +0   rodata-4216 data-160 over-16
zFeOverlay               +0   bss-4096 sbss+4 over-1136
```

**Veinte de veintitres tienen el `.text` a cero.** Lo que descuadra es `.rodata`
en las veintidos, y casi siempre **en negativo**: nuestro enlace produce MENOS
`.rodata` que la base. El objetivo tiene mas, no menos. Y no son los `gap_`/`pad_`
del troceador —medidos aparte: entre 0 y 908 B por unidad contra deficits de
hasta 4.216—.

Ademas del tamano hay un eje que `linkdelta` no ve: el **orden**. Arnes
`scratchpad/r49_linked/mapa.py` (enlaza, y diffea la tabla de simbolos del ELF
contra `build/GOWE69/main.elf`, filtrando los literales, que solo cambian de
nombre: `$LC145` contra `lbl_80403FA8`):

| unidad | simbolos que cambian de direccion | primero de cada seccion |
|---|---:|---|
| `zSim` | **93** | `.rodata` 80404B70 **+8** `_vt.16GameplayActivity.11IAttachable`; `.text` 80289DC0 +540 `AllocVectorSpace__Q23UTLt11FixedVector3Z...` |
| `zGameModes` | 5.644 | `.data` +128, `.bss` +128, `.rodata` +104 |
| `zMisc` | 5.399 | `.rodata` +2.904 `_vt.16DisculatorDriver` |
| `zAnim` | 6.362 | `.rodata` −1.240 `_vt.25GenericNISControlScenario` |
| `zEAXSound` | 6.869 | `.rodata` −980 `_14EAXAemsManager.m_SlotSizes`; `.text` +1.308 `Update__8EAXSoundf` |
| `zAI` | 7.215 | `.rodata` −1.352 `_vt.3Gps.11IAttachable`; `.text` +48 `SetTrafficPattern__16AITrafficManagerUi` |
| `zFe2` | 7.123 | `.rodata` −861 `_10FEKeyboard.mLetterMap`; `.text` +88 `SetMapHeader__21ChoppedMiniMapManagerPc` |

Los millares de simbolos movidos de todas menos `zSim` son **consecuencia**: al
descuadrar el tamano de `.rodata`/`.data`, todo lo que va detras en la imagen se
corre. La causa es una sola por unidad.

**Respuesta directa a `zEAXSound`** (la unidad que el encargo marcaba como la mas
valiosa): la bandera **no la arregla** —pierde 68 simbolos— y **aunque la
arreglara no compraria nada**, porque su `.text` ya esta a delta 0 en el enlace.
Su bloqueo son `rodata-1544 data-384 bss+32` y el orden dentro de `.text`
(`Update__8EAXSoundf` a +1.308).

---

## 5. `zSim` es la unidad mas cerca de todo el censo, y su bloqueo tiene nombre

`zSim` (96.400 B) es la **unica** con las secciones del enlace **IGUALES** a la
base. Sus 93 simbolos movidos son todos suyos y se parten en dos:

**a) `.rodata`: el pool de literales.** El primer byte que difiere en el enlace
es `0x80404868`, y es una permutacion:

```
BASE  00 00 00 00 | 3c 23 d7 0a        (0.0f y luego 0.01f)
NUES  3c 23 d7 0a | 00 00 00 00        (al reves)
```

De ahi hasta el final del `.rodata` de la unidad hay **2.042 bytes distintos de
7.864**, todos de la misma familia: los mismos literales en otro orden. El
arrastre acumulado son los **+8** con los que empiezan a moverse las 72 vtables
a partir de `_vt.16GameplayActivity.11IAttachable`. Los literales estan **todos**
(comprobado por contenido: `SCENERY_GROUP_DOOR`, `42960000`, `43b40000`,
`47800000`, `3983126f` aparecen el mismo numero de veces en los dos objetos).

Ojo con un detalle: `zSim` ya lleva un pool escrito a mano
(`#define ZSIM_HAND_POOL` en `zSim.cpp:79`, y los `asm` de `.rodata` de
`LocalPlayer.cpp:118` y `zSim.cpp:230`). Hoy **`"FEngHUD"` sale dos veces** en
nuestro `.rodata` (el `$LC481` de GCC en `+0x6D0` **y** los 8 B del `asm` en
`+0x6D8`) donde el objetivo lo tiene una: el comentario del `asm` asume que el
`$LC` se estripa con `SetHud` y no se estripa. Ahi hay 8 B de mas que son el
origen del desfase.

**b) `.text`: el orden de instanciacion.** 21 cuerpos de plantilla
(`_._Q23UTLt11FixedVector3Z...`, `_._Q23UTLt6Vector2Z...`, `GetGrowSize__C...`)
salen intercalados de otra forma: el objetivo pone `FixedVector<...>` antes que
`Vector<...>` y nosotros al reves. Es el frente de `nfsmw-inline-es-posicion.md`
(el orden de `finish_file`), no un problema de codigo.

Y `.rodata` emite ademas 5 vtables de interfaz que el objetivo no tiene
(`_vt.6IModel` 272 B, `_vt.11IAttachable` 72, `_vt.13IVehicleCache` 48,
`_vt.Q214EventSequencer8IContext` 32, `_vt.Q33UTL3COM8IUnknown` 24 — 448 B) —
la familia de `nfsmw-la-virtual-que-era-pura.md`, esta vez por el lado
contrario—. Se estripan, pero conviene saber que estan.

---

## 6. `zFe2` vuelve a enlazar — lo unico que he tocado

`zFe2` **no enlazaba en absoluto**: la sustitucion daba `ENLACE FALLA` con 16
referencias indefinidas.

```
L0039: Reference to undefined symbol
  _._Q313FEPlayerCarDB46GetNumCareerCarsWithARecord__13FEPlayerCarDB.0_7NumCars.32166
  Callback__CQ313FEPlayerCarDB46GetNumCareerCarsWithARecord__...32165
  _._Q313FEPlayerCarDB33GetTotalFines__13FEPlayerCarDBb.0_5Fines.32149
  ...
```

Es exactamente lo que documenta `scripts/mangfix.py`: los alias mangled de las
clases locales de `FEPlayerCarDB` estaban **caducados por 3** (`.32168` cuando el
compilador pide `.32165`, y asi los dieciseis). `python scripts/mangfix.py zFe2`
los corrige.

| | antes | despues |
|---|---|---|
| `fncmp Speed/Indep/SourceLists/zFe2` | 0 de 1.307 con el codigo distinto | **0 de 1.307** |
| `trypromo zFe2` | **ENLACE FALLA** | `DOL ROTO (ff0979ce9e5d)` |
| `linkdelta zFe2` | (no enlazaba) | `.text +0  rodata-4216 data-160 over-16` |

Fichero modificado: `src/Speed/Indep/Src/Frontend/Database/VehicleDB.cpp`, 16
lineas, solo numeros dentro de los `asm()` de alias. **Finales de linea
intactos** (fichero MIXTO: 1.418 CRLF y 282 LF antes y despues, y el mismo
tamano: 58.676 B). Solo lo incluye `zFe2.cpp`. Copia de entrada en
`scratchpad/r49_linked/VehicleDB.cpp.bak`.

---

## 7. Promociones demostradas

`trypromo.py` sobre **las 30** unidades con el codigo al 100 % que no
promocionan (`scratchpad/r49_linked/trypromo_all.txt`). Dos dan **`DOL OK`**:

```
  libc/kf_rem_pio2                             DOL OK      2.180 B
  libc/ef_rem_pio2                             DOL OK        848 B
  libc/kf_rem_pio2 + libc/ef_rem_pio2          DOL OK      3.028 B
      sha1 del DOL: 9619ba57c9919f95f7f2ac951a2166a3517f91e3
```

Reproducido despues de recompilar las dos con `build_direct.py`, o sea que sale
de la fuente actual y no de un `.o` rancio. `fncmp` en las dos: **0 de 1
funciones con el codigo distinto**.

**Propuesta**: marcar `Matching` en `configure.py`
`libc/kf_rem_pio2.c` y `libc/ef_rem_pio2.c`. **Van juntas** (por separado
tambien salen OK, pero juntas es como se promocionan).

**Credito**: las dos son territorio del agente `promo` de esta misma ronda y
llevan cambios suyos sin commitear en el arbol (`src/libc/ef_rem_pio2.c`,
`src/libc/kf_rem_pio2.c`). Yo solo he medido que ya promocionan; el trabajo es
suyo. Lo dejo escrito aqui para que la promocion no se pierda entre las dos
tandas.

El resto de las 30, con su sha de DOL roto, esta en
`scratchpad/r49_linked/trypromo_all.txt`. `libc/libgcc2_4` (176 B) rompe el DOL
tambien acompanando a las dos de arriba: `951091f0ce4b`.

---

## 8. Lo que hay que hacer ahora, en orden

1. **Retirar `-fno-implicit-templates` de la cola de trabajo.** No es la palanca:
   §1 dice que es insegura en 12 de 13 y §2 que en la 13.ª no cambia el DOL. La
   que ya esta puesta en `zFeOverlay` puede quedarse (es inocua y hace el objeto
   mas pequeno) o irse; da igual al byte.
2. **El frente es `.rodata`, y esta sin diagnosticar.** Veintidos unidades con
   deficit de entre 288 y 4.216 B de `.rodata` VIVA en el enlace. No son los
   `gap_`/`pad_` (medido). Es el frente que la r48 dejo abierto en una linea y
   ahora es el unico que queda.
3. **`zSim` primero**: es la unica con las secciones IGUALES, son 96.400 B, y el
   bloqueo esta acotado a (a) el orden del pool de literales, con el `"FEngHUD"`
   duplicado como origen medido, y (b) el orden de 21 cuerpos de plantilla en
   `.text`. Las dos cosas son de posicion, no de codigo.
4. **`zGameModes` sigue siendo la mas barata** (124 B de codigo). Su objeto
   objetivo tiene **exactamente cinco simbolos**
   (`__static_initialization_and_destruction_0` 80 B,
   `_GLOBAL_.I.aEmotionalSummaryTypeStrings` 44 B, `kFloatScaleUp`,
   `kFloatScaleDown`, y un UNDEF a `lbl_803EBE90`) y **ni `.data` ni `.rodata`**.
   Nosotros emitimos la clase `EmotionManager` entera; de sus 248 B de `.rodata`
   sobreviven al estripado **104** porque el `.ctors` los mantiene vivos. Es un
   problema de a que unidad le asigna `splits.txt` las tablas de cadenas, no de
   codigo.
5. **`zGameplay` tiene un `pad_05_803EBB48_rodata` de 840 B sin reclamar**, la
   misma familia que el `.rodata 0x80413A48..0x80413AE0` de `pathnode` de la r48.
   Es trabajo de `splits.txt`, o sea de coordinacion.

## 9. Verificacion

```
fncmp Speed/Indep/SourceLists/zFe2   0 de 1307 antes  ->  0 de 1307 despues
fncmp libc/ef_rem_pio2               0 de 1
fncmp libc/kf_rem_pio2               0 de 1
lcfix.py --check                     todas las entradas @lc estan al dia
trypromo libc/kf_rem_pio2 + libc/ef_rem_pio2   DOL OK
   sha1 = 9619ba57c9919f95f7f2ac951a2166a3517f91e3
linkdelta.py (23 unidades)            .text +0 en 20; ninguna empeora
```

Ninguna funcion empeora en ninguna unidad. **No se ha lanzado ningun `ninja` ni
`configure.py`**; no hay commits ni `git add`; no se ha tocado `configure.py`,
`config/GOWE69/*` ni `splits.txt`. Un solo fichero de `src/` modificado
(`VehicleDB.cpp`, §6). **Todos los objetos y volcados de prueba borrados**
(`scratchpad/r49_linked/v/`, ~160 MB, y el indice cacheado).

### Un aviso de metodo, medido a mi costa

El `ldscript` empareja por **nombre de fichero**
(`*zFEOverlay.o(.over .rodata)`). Un objeto de prueba llamado
`zFeOverlay_base.o` deja su `.rodata` fuera de `.over` y el enlace sale con
`over-8476 rodata+6856`: un diagnostico falso completo. **Los objetos alternos
tienen que llamarse igual y vivir en otro directorio.**

Y el arbol se mueve debajo: entre el principio y el final de la ronda cambiaron
el HEAD y la lista de ficheros modificados. Compilar la base uno mismo, no
fiarse del `.o` que ya esta en `build/GOWE69/src`.

## 10. Artefactos

`scratchpad/r49_linked/`:

- `hh.py` — compila una unidad con banderas extra a un `.o` del scratchpad y saca
  las tres cifras (`.text`, funciones con codigo distinto, simbolos que faltan)
  mas el predicado de vivos;
- `sweep.py` — el barrido en paralelo (base + variante por unidad);
  `sweep_nit.json` / `sweep_log.txt`, la tabla de §1;
- `tp2.py` — `trypromo` con objetos ALTERNOS, y ademas el delta de secciones del
  ELF;
- `lsym.py` — enlaza y diffea la TABLA DE SIMBOLOS del ELF contra la base: dice
  que simbolos cambian de direccion;
- `mapa.py` — lo anterior por unidad, filtrando los literales, con el primer
  desplazado de cada seccion (la tabla de §4);
- `secsym.py` — diff de simbolos POR SECCION entre el `.o` extraido y el nuestro,
  emparejando por nombre base, con el ORDEN de los comunes;
- `idx.py` — indice cacheado de UNDEF/definidos de los 618 objetos del enlace;
- `trypromo_all.txt`, `linkdelta_final.txt`, `promopred_base.txt` — las medidas;
- `VehicleDB.cpp.bak` — copia de entrada del unico fichero tocado.
