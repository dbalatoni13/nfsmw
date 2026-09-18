# r62 - zCamera: el bloqueo NO era el codigo, era la COLOCACION. dolwhere 333.533 -> 30.671 B

**Resultado**: `dolwhere zCamera` baja de **333.533 B a 30.671 B (-90,8 %)**, con
`linkdelta` en `IGUAL` antes y despues, `fncmp` **identico** (las mismas 3
funciones y los mismos 8.464 B) y el `.o` sellado tres veces.
`trypromo` sigue dando `DOL ROTO (7c1cba0adc72)`: **no promociona**, pero la
distancia real cae un orden de magnitud y queda medida.

```
                          antes (r61)     despues (r62)
fncmp                     3 / 453         3 / 453        (8.464 B, sin tocar)
linkdelta                 .text +0 IGUAL  .text +0 IGUAL
permorden DESPLAZADAS     79 de 453       28 de 453
ciclos no triviales       19              11
dolwhere                  333.533 B       30.671 B
reorden CONTENIDO         35 palabras     30 palabras
sello del .o (x3)         a9b29ca4...     094bd7d95f993d504e7f13baec9097964f142720
```

---

## 1. El diagnostico que faltaba: zCamera no tenia un problema de codigo

El encargo describia zCamera como "el lote de codigo mas gordo que queda"
(3 funciones, 8.464 B). Es cierto y es irrelevante: **el DOL rompia por 333 kB,
y de esos 333 kB las tres funciones abiertas no explicaban ni el 1 %.**

Lo que lo dijo, en dos medidas de solo lectura:

1. **`reorden.py`**: recolocando NUESTROS bytes en el orden del objetivo quedaban
   **35 palabras** de contenido real en toda la unidad -- 15 en
   `Update__19TrackCarCameraMoverf`, 15 en `Update__8ICEMoverf`, 5 palabras de
   vtable y 2 discrepancias de TAMANO. Todo lo demas era posicion.
2. **Un diff de las TABLAS DE SIMBOLOS de los dos ELF ENLAZADOS**
   (`scratchpad/cam62/symmove.py`, la tecnica de `nfsmw-text-cero-dos-errores`):
   **2.305 simbolos de `.data` a -24 B, 142 mas a -28 B y 1.165 de `.bss` a -8 B**.
   Un desplazamiento global de seccion, invisible para `linkdelta` (que compara
   TAMANOS, y el relleno de alineacion se traga 24 B) y que `dolwhere` reparte
   entre cientos de simbolos ajenos a zCamera.

**Esos 28 B de `.data` y 8 B de `.bss` valian 302.845 B de DOL.** Eran el 91 %
del problema y estaban a dos ediciones de un `asm()` que ya existia en el arbol.

---

## 2. Lo aplicado, por orden de rendimiento

Todo en `src/`; **nada en `config/`, `splits.txt`, `symbols.txt` ni `configure.py`**.

| # | Cambio | Fichero | dolwhere despues |
|---|---|---|---:|
| 0 | base | - | 333.533 |
| 1 | `Reset() override {}` sacado FUERA de la clase (x4) | CDActionTrackCar/TrackCop/Showcase/DebugWatchCar.cpp | - |
| 2 | `ATTRIB_NO_INLINE_CLASSKEY` APAGADO | zCamera.cpp | - |
| 3 | `CDActionDrive::SetSpecial` metido DENTRO de la clase | CDActionDrive.cpp | - |
| 4 | `~CameraAnchor()` pasado a IMPLICITO (guardado por macro) | CameraMover.hpp + .cpp | 271.187 |
| 5 | reorden de 15 funciones | CameraMover.cpp | - |
| 6 | reorden de 32 funciones | CameraAI.cpp | 254.220 |
| 7 | reorden de 39 funciones | ICEManager.cpp | 254.215 |
| 8 | **`gap_06_80417085_data` 0x28 -> 0x44** | zCamera.cpp | **65.552** |
| 9 | reorden de 17 funciones (Cubic) + `WasRecentlyUsed`/`ClearRecentlyUsed` movidas a ICEManager.cpp | Cubic.cpp, ICEReplay.cpp, ICEManager.cpp | 50.307 |
| 10 | **`gap_07_8045A99C_bss` nuevo, 8 B** | zCamera.cpp | **30.688** |
| 11 | `Pov*Scale` tras los `Cubic*`; `bUseOldDutch` antes de `GenericCategoryNames`; `gCinematicMomementCamera` antes de `gGameBreakerCamera` | Cubic.cpp, ICEManager.cpp, CDActionDrive.cpp | **30.671** |

### 2.1 Las dos ediciones que valen 302.845 B

**`.data` (-28 B).** El comentario de la r59 que vive en `zCamera.cpp` decia la
verdad y nadie volvio: *"gap_06_80417085_data mide 67 B en el objetivo y aqui va
RECORTADO a 40 ... el dia que se escriban las estaticas de funcion que faltan hay
que ENCOGERLO lo mismo"*. Las estaticas no se han escrito, pero el recorte estaba
**28 B por debajo** de lo que la seccion necesita: objetivo `.data` = 3.136 B,
nuestro = 3.108 B. Subiendo el hueco de `0x28` a `0x44` la seccion mide
**3.136 B exactos** y los 2.447 simbolos de `.data` de las unidades de abajo
vuelven a su direccion. `keep.lst` protege por NOMBRE: **no hace falta tocar
config**.

**`.bss` (-8 B).** Mismo mecanismo y misma via: `keep.lst` nombra desde la r58
ocho huecos `gap_07_8045A99C_bss` .. `gap_07_8045AAC4_bss` y **nuestro objeto no
emitia ninguno**. Un `asm()` con `.space 8` y el primero de esos nombres cierra
el desfase. `.bss` no viaja en el DOL: lo que se cobra es que las unidades de
abajo recuperan su direccion.

**Aviso**: los dos son ANDAMIOS declarados, igual que el de la r59. Cuando se
escriban las estaticas que faltan hay que encoger el hueco de `.data` lo mismo
que crezca el codigo, y quitar los 8 B de `.bss` si aparece el simbolo real. Van
comentados en el fuente.

### 2.2 El mecanismo de las cuatro `Reset()` (y su reciproca)

Ocho funciones estaban desplazadas ~300 posiciones, cuatro en cada sentido, y las
ocho por la MISMA decision: **cuerpo dentro de la clase = inline = sale en la COLA
de `finish_file`; cuerpo fuera = sale en su sitio, en orden de fuente**
(`nfsmw-en-clase-es-inline`).

- `Reset__16CDActionTrackCar/TrackCop/Showcase/DebugWatchCar` (4 B cada una):
  nosotros `void Reset() override {}` en clase -> cola (obj#87 -> nue#380).
  El objetivo las tiene FUERA, justo detras del destructor. Confirmado por los
  vecinos: `CDActionDebug` y `CDActionIce` ya las tenian fuera y **no estaban
  desplazadas**.
- La reciproca, tres casos: `SetSpecial__13CDActionDrivef`,
  `ClassKey__Q36Attrib3Gen4ecar/camerainfo` y `_._12CameraAnchor` salian
  DEMASIADO PRONTO porque las teniamos fuera de la clase. El objetivo las tiene
  en la cola.

**Premio colateral medido**: las **5 palabras de `_vt.*CDAction*`** que
`reorden.py` listaba como contenido real en la r61 desaparecieron solas. Eran la
ranura de `Reset` en cinco vtables apuntando a la funcion equivocada. Ahora
`reorden` deja 30 palabras y **las 30 son las dos `Update`**.

### 2.3 `ATTRIB_NO_INLINE_CLASSKEY`: la guarda sobraba en zCamera

`zCamera.cpp` definia la macro y `CameraAI.cpp` suministraba
`Gen::ecar::ClassKey()` / `Gen::camerainfo::ClassKey()` fuera de linea. El
objetivo las tiene en la COLA (obj#344/345), o sea que usa la `inline` de la
cabecera generada. Apagando la macro casan de posicion. **La medida vieja del
PLAYBOOK ("zFe2 pierde 1.376 B sin la macro") NO se extrapola: en zCamera
apagarla es positivo y `linkdelta` no se mueve.**

### 2.4 Los reordenes de fuente

Cuatro ficheros reordenados poniendo las definiciones en el orden que dicta el
`.o` extraido (`CameraMover.cpp`, `CameraAI.cpp`, `ICEManager.cpp`, `Cubic.cpp`).
Detalles que costaron tiempo y hay que saber:

- **En `CameraAI.cpp` ninguna `static` necesito declaracion adelantada**: el
  orden del objetivo respeta por si solo las cuatro dependencias
  (`FindPlayer`, `FindDirector` x2, `AreMomentCamerasEnabled`, `AverageAir`).
  Se comprobo una por una antes de mover.
- **Los datos definidos ENTRE funciones tienen que viajar con la funcion que los
  precede.** `ICEManager.cpp` tiene tres (`bUseOldDutch`, `GenericCategoryNames`,
  `TheICEManager`); moverlos sueltos cambia el orden de `.data` y de `$LC`.
- `WasRecentlyUsed__9ICEReplay` y `ClearRecentlyUsed__9ICEReplay` estan en el
  objetivo en obj#250/251, o sea **entre la ultima funcion de `ICEMover.cpp` y la
  primera de `ICEManager.cpp`**, no al principio de `ICEReplay.cpp` (que es donde
  las teniamos, y donde el resto de `ICEReplay` si casa, obj#318/319). Movidas a
  la cabeza de `ICEManager.cpp`, que ya incluye `ICEReplay.hpp`.

---

## 3. NEGATIVOS Y TRAMPAS (lo que no funciona, con cifra)

### 3.1 `reorden.py` da FALSOS CEROS en los nombres repetidos -- TRAMPA NUEVA

`reorden.py` dice **0 palabras de contenido** para
`__static_initialization_and_destruction_0`, y `fndiff` sigue dando las **6 filas
de siempre** (549/557/558/561/562/564; 3.604/3.604 B; 99,839066 %), tres de ellas
diferencias de REGISTRO puro (`sth r0, NOSFovWidening@l(r11)` contra `(r9)`), que
la mascara de `reorden` NO oculta (para forma D guarda `v >> 16`, o sea
opcode + rD + rA).

Causa medida: **`reorden` empareja POR NOMBRE**, y en el ELF enlazado
`__static_initialization_and_destruction_0` **aparece 47 veces**; hay **312
nombres repetidos** en total. Compara contra el static-init de otra unidad.

**Regla**: `reorden` NO cierra una funcion cuyo nombre se repita en el enlace.
Para esas el juez es `fndiff`. (Vale para `docs/TRAMPAS.md`.)

### 3.2 Reordenar la unidad entera no mueve las 6 filas del static-init

Con 51 funciones recolocadas y `dolwhere` a la decima parte, las seis filas del
racimo `LIT/CONST/SYM` de `Cubic.cpp:358-399` estan **byte a byte igual**. Es
evidencia independiente de que la cerradura es del ASIGNADOR y no del entorno de
la unidad. Anotado junto a la veda.

### 3.3 La receta 1 de `r60b-auditoria-vedas.md` YA estaba medida y es negativa

El encargo decia "nadie volvio". Si volvieron: el comentario de `Cubic.cpp` trae
la medida de **r61-plat**: con el `_cam_pin` el tamano aguanta en 3.604 B (o sea
el `asm` no gasta ranura y el control del informe se cumple) pero el porcentaje
cae a 98,234184 % y las filas suben de 6 a **58**, y falla su propio observable:
la fila 549 no pasa a `li r9,0x38e`. **Esa via esta cerrada para esta funcion.**

### 3.4 Lo que NO se ha atacado

`Update__8ICEMoverf` (3.868 B) y `Update__19TrackCarCameraMoverf` (992 B) no se
han tocado. El dato nuevo, anotado junto a las dos, es que **son lo unico que le
queda a zCamera aparte del orden**: `reorden` deja 30 palabras y las 30 son
15 + 15 en ellas.

---

## 4. Lo que queda en zCamera, en orden de tamano

`dolwhere` 30.671 B, repartidos asi (por simbolo que contiene la direccion):

```
  7.573  _._11IAttachable                    <- la COLA de .text, obj#338..#380
  4.786  MessageJumpCut__13CDActionDrive...  <- racimo de accesores de CDAction*
  3.127  $LC316                              <- orden del pool de .rodata
  2.124  CubicHood                           <- orden de .data
  1.240  _vt.19TrackCopCameraMover
  1.141  IsAnyCopNear__FP12CameraAnchor
  1.068  _vt.21CDActionDebugWatchCar
    ...
    429  __static_initialization_and_destruction_0   (las 6 filas)
    225  Update__8ICEMoverf
    152  Update__19TrackCarCameraMoverf
```

### 4.1 Frente A: la COLA de `.text` (28 descolocadas, ~12 kB)

Las 28 que quedan estan TODAS en la cola de `finish_file` (obj#338..#453). El
orden DENTRO de cada grupo casa; lo que no casa es el entrelazado de los grupos:

| simbolo | obj# | nue# |
|---|---:|---:|
| `_IHandle__14IDebugWatchCar` + `_._14IDebugWatchCar` + `push_back__...IDebugWatchCar` | 341-343 | 426-428 |
| `_._Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2` | 439 | 367 |
| `GetAnchor__19TrackCopCameraMover` + `RenderCarPOV__19TrackCopCameraMover` | 359-360 | 393-394 |
| `_._Q43UTL11Collectionst8Listable2ZQ28CameraAI8Directori2_4List` | 339 | 368 |
| `_._11IAttachable` | 366 | 338 |
| `GetAnchor__19TrackCarCameraMover` | 358 | 383 |
| `_._12CameraAnchor` | 361 | 343 |
| `GetAnchor__25RearViewMirrorCameraMover` | 357 | 369 |

El patron es limpio y es la pista para el siguiente: **el objetivo agrupa los
`GetAnchor` de los tres Movers (RearView, TrackCar, TrackCop) INMEDIATAMENTE
detras de los accesores de `CameraMover` y DELANTE de los de
`CubicCameraMover`**; nosotros los sacamos cada uno pegado a su `CDAction`
usuario, decenas de posiciones despues. Eso es la cola `saved_inlines`, y la
regla que la ordena esta en `cp/decl2.c:2140` + `cp/pt.c:9439-9461`
(`nfsmw-orden-de-emision`, seccion r54): **decide el PUNTO DE USO dentro de una
funcion**. La palanca es la `static` MUERTA de una linea colocada en el punto del
fichero donde el objetivo lo emite.

### 4.2 Frente B: el orden de `.data` (12 descolocados, ~2,5 kB)

`.data` mide ya 3.136 B EXACTOS y le quedan 12 simbolos descolocados. Cinco
casos, con el diagnostico hecho:

1. `cameralink` (estatica de funcion de `Camera.cpp`) va 2a en el objetivo y 7a
   en el nuestro.
2. `CameraDebugWatchCar` + `Tweak_EnableICEAuthoring` + `Tweak_ForceICEReplay`
   van DELANTE de `TheAvoidables` en el objetivo. **Y eso es cross-fichero**:
   `CameraAI.cpp` (TheAvoidables) se incluye ANTES que `CDActionIce.cpp` y
   `CDActionDebugWatchCar.cpp`. Si el orden de `.data` es orden de definicion,
   esto solo se explica con **otro orden de `#include` en el SourceList
   original**, o con que esos huecos `gap_06_*` no sean lo que dtk cree. **No
   verificado: es la primera pregunta para el que siga.**
3. `gCamCloseToRoadBlock` (TrackCar.cpp) va justo tras `gGameBreakerCamera`.
4. `old_pov` es en el objetivo `old_pov.29797`, o sea una **estatica DE FUNCION**;
   la nuestra es de fichero (`CDActionDrive.cpp:45`).
5. Los seis `gap_06_*` y los tres `lbl_*` estan en el objetivo INTERCALADOS entre
   simbolos reales y en el nuestro TODOS AL FINAL (el `asm()` de `zCamera.cpp`).
   `lbl_804164C8` = 0x3ECCCCCD, `lbl_80416FF4` = 2 y `lbl_80417008` = 0x3D4CCCCD
   llevan VALOR, asi que su posicion se cobra en bytes del DOL. Para cerrarlo hay
   que **partir el `asm()` en nueve trozos** y colgar cada uno del punto del TU
   que le toca.

---

## 5. Ficheros tocados

```
src/Speed/Indep/SourceLists/zCamera.cpp
src/Speed/Indep/Src/Camera/CameraMover.hpp          <- CABECERA COMPARTIDA (ver aviso)
src/Speed/Indep/Src/Camera/CameraMover.cpp
src/Speed/Indep/Src/Camera/CameraAI.cpp
src/Speed/Indep/Src/Camera/Actions/CDActionDrive.cpp
src/Speed/Indep/Src/Camera/Actions/CDActionTrackCar.cpp
src/Speed/Indep/Src/Camera/Actions/CDActionTrackCop.cpp
src/Speed/Indep/Src/Camera/Actions/CDActionShowcase.cpp
src/Speed/Indep/Src/Camera/Actions/CDActionDebugWatchCar.cpp
src/Speed/Indep/Src/Camera/Movers/Cubic.cpp
src/Speed/Indep/Src/Camera/Movers/TrackCar.cpp      <- solo comentario
src/Speed/Indep/Src/Camera/ICE/ICEMover.cpp         <- solo comentario
src/Speed/Indep/Src/Camera/ICE/ICEManager.cpp
src/Speed/Indep/Src/Camera/ICE/ICEReplay.cpp
```

### AVISO DE REGLA 5 -- cabecera compartida

`CameraMover.hpp` la ven **39 ficheros** (World, Ecstasy, Frontend, AI,
EAXSound, Sim, Physics). El cambio es de tres lineas y esta **guardado**:

```c
    CameraAnchor(int model);
#ifndef ZCAM_ANCHOR_IMPLICIT_DTOR
    ~CameraAnchor();
#endif
```

`ZCAM_ANCHOR_IMPLICIT_DTOR` **solo lo define `zCamera.cpp`** (comprobado con grep
en todo el arbol). Para cualquier otra unidad el preprocesado es LITERALMENTE el
de antes, asi que el cambio es inerte por construccion -- **pero eso es un
razonamiento, no una medida**: no he compilado ninguna otra unidad para no pisar
el `.o` de otro agente. Si el jefe prefiere no tocar la cabecera, la reversion
cuesta una linea y solo devuelve `_._12CameraAnchor` a su sitio malo.

## 6. Config

**CERO propuestas para `keep.lst`, `splits.txt`, `symbols.txt` o `configure.py`.**
Los dos huecos cambiados (`gap_06_80417085_data`, `gap_07_8045A99C_bss`) YA
estaban nombrados en `config/GOWE69/keep.lst` desde la r58; se ha usado el nombre
que ya estaba y solo se ha cambiado el TAMANO / se ha emitido el simbolo.

`lcfix.py --check zCamera`: **"todas las entradas @lc estan al dia (y 0 @lcsrc de
fuente)"** -- CERO correcciones pendientes, ninguna venenosa. `lcfix.py` NO se ha
ejecutado en modo escritura.
