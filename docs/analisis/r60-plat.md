# r60 - agente `plat`: las TRES unidades con las nueve secciones a IGUAL

    zPlatform           .text -8   IGUAL   ->   .text +0  IGUAL
    zCamera             .text +16  IGUAL   ->   .text +0  IGUAL
    zPhysicsBehaviors   .text +4   bss+4   ->   .text +4  IGUAL   (el +4 es la funcion VETADA)

Cero promociones: `trypromo` da `DOL ROTO` en las tres. Pero las tres pasan de tener una
seccion descuadrada a no tener ninguna, y `ActualReadJoystickData` --- once rondas
bloqueada --- baja de **16 filas a NUEVE**, con el tamano EXACTO y en un solo grupo.

Ni un commit, ni un `git add`, ni una linea de `config/GOWE69/*`, `splits.txt` o `keep.lst`.

| unidad | `linkdelta` antes -> despues | `dolwhere` antes -> despues | `trypromo` | sha1 del `.o` (3 compilados iguales) |
|---|---|---|---|---|
| zPlatform | `.text -8` IGUAL -> **`.text +0` IGUAL** | no medible (secciones descuadradas) -> **95.754 B** | DOL ROTO | `f840e5a03e2c54e9e822e4f72f84507e6bfd74fc` |
| zCamera | `.text +16` IGUAL -> **`.text +0` IGUAL** | no medible -> **333.533 B** | DOL ROTO | `e74ba2da8c97e6b172d671c2b4916fd348af2d4b` |
| zPhysicsBehaviors | `.text +4` `bss+4` -> **`.text +4` IGUAL** | no medible (el `.text +4` la deja descuadrada) | DOL ROTO | `e4fceb82377624a0ba2bda0923b00b0648c44f7b` |

sha1 de partida, para poder deshacer: zPlatform `03457ae6c7d1cc81ce4ad0bf07f5c0c4221e81db`,
zCamera `eb51e40f2add8bf155325d761e927ff8f470244d`, zPhysicsBehaviors
`bb75afca1c7c3873b922be8e6897ac6982de9f36`.

`fncmp` ANTES, DESPUES y tras cada fichero:

    zPlatform            1 de 136    ActualReadJoystickData  1580/1588 16 filas -> 1588/1588 NUEVE
    zCamera              3 de 453    __static_init 3620/3604 54 filas -> 3604/3604 SEIS; los dos Update igual
    zPhysicsBehaviors    1 de 1120   UpdateLoaded 860/856 (VETADA) -- sin cambio

`lcfix.py --check`: **311 correcciones pendientes y 56 FALLO, y NINGUNA es de mis tres
unidades** (zFe 142, zLua 84, zSpeech 43, zAI 42; los FALLO en zFeOverlay 28, zAnim 22, zFe 2,
zLua 2, zAI 1, zSpeech 1). Ninguna nombra un simbolo de zPlatform, zCamera ni
zPhysicsBehaviors, asi que **ninguna puede romperlas** y **nada de lo de aqui depende de que
se aplique `lcfix`**.

---

## 1. `ActualReadJoystickData`: 16 filas -> 9, y el tamano EXACTO

Once rondas (r0, r9, r11, r18, r19, r29, r36, r36b, r36f, r46, r47, r48, r50) la habian
dejado en 1.580 B / 99,3199 % / 16 filas, con el veredicto escrito de la r48: *"lo que queda
por probar y no he sabido montar: meter esas dos instrucciones sin barrera"*.

### 1.1 Lo que estaba mal en el diagnostico: el `stb` va EN MEDIO

El objetivo emite, en la linea 307 de su fuente, dos instrucciones **muertas**:

    188 li r11, 0xff
    189 extsh r0, r11          <- destino del `ble` de arriba
    190 mr r9, r0              <- linea 307
    191 stb r0, 0x4(r29)       <- linea 306, AnalogLeftX
    192 andi. r11, r9, 0x8000  <- linea 307

**El `stb` esta ENTRE las dos.** Por eso el `asm volatile` unico de la r48 (X2, las dos
instrucciones juntas) no podia acertar nunca: es **un solo insn de RTL**, y el planificador
solo puede poner el `stb` delante o detras del bloque entero. La r48 midio ese sintoma
("mueve el `stb` dos ranuras") pero lo apunto como coste de la barrera, no como imposibilidad.

**La palanca es DOS `asm volatile` que ENVUELVEN la sentencia de C**: cada uno es su propio
insn, la sentencia real queda en medio y el orden sale exacto. No es que la barrera estorbe:
son barreras las dos, y precisamente por serlo fijan el orden que hace falta.

### 1.2 Y el `extsh` va DENTRO del asm, no devuelto a la fuente

El `extsh r0, r11` del objetivo existe porque **su linea 307 LEE `v`**, asi que
`v = (short)data` sobrevive. En nuestra fuente el destino es un `unsigned char` y GCC se come
la conversion. Devolverle el uso (el `if (v & 0x8000) v = 0;` muerto) **si** emite el `extsh`
y da el tamano exacto... y mete **1 insn de RTL de mas**, con la que se rompen los DOS grupos
de empate de `allocno_compare` que en ese punto salian bien (`r18/r19` y `f25/f26/f27`):
**20 filas en vez de 9**. Es el mecanismo que la r48 documento en su seccion 1.4 --- el estado
de cada grupo depende de la longitud exacta del cuerpo del bucle --- visto desde el otro lado.

Metiendo el `extsh` **dentro del primer `asm`** el objeto crece 4 B sin crecer un insn de RTL,
y los dos grupos se quedan como estaban.

### 1.3 La escalera, toda medida sobre la unidad real

| # | forma | tamano | fuzzy | filas |
|---|---|---:|---:|---:|
| 0 | base | 1.580 | 99,3199 % | 16 |
| P1 | `asm` **NO volatil** `"+r"(v)` con salida viva (la *barrera selectiva*, que es lo que la r48 no supo montar) | 1.588 | 97,4937 % | 41 |
| X2 | un solo `asm volatile` con las dos instrucciones (r48, reproducido al byte) | 1.588 | 99,2569 % | 21 |
| Y1 | **DOS `asm volatile` envolviendo el store** + el `if` muerto | 1.588 | 99,7481 % | 20 |
| Y1a | igual, **sin** el `if` muerto | 1.584 | 99,6348 % | 10 |
| **T6** | **Y1a + `extsh 0,8` dentro del primer asm** | **1.588** | **99,88665 %** | **9** |
| | T6 con los dos asm SEPARADOS en dos sentencias | 1.588 | 99,8111 % | 15 |
| | T6 + pin `register int data asm("r10"/"r11")` | 1.580 | 99,2443 % | 19 |
| | T6 + `short data` en el bloque de LeftX | 1.584 | 99,5088 % | 20 |
| | T6 + pines de `fr25/fr27/fr26` en 7.0f/5.0f/0.0f | 1.588 | 94,5340 % | 51 |
| | Y3 `asm volatile("mr 9,%0" : : "r"(v))` | 1.604 | 91,0227 % | 132 |
| | Y1a + `asm volatile("" : : "r"(v))` para revivir `v` | 1.608 | 88,3627 % | 147 |

Todos los negativos estan escritos **junto a la funcion** en `JoyE.cpp`, donde los encuentra
`previo.py`.

Aviso de metodo: el nombre de registro que acepta `register ... asm(...)` en este cc1 es
**`fr25`, no `f25`** (`invalid register name`), y **un `asm("a; b")` no es lo mismo que dos
`asm("a"); asm("b")`**: 9 filas contra 15.

### 1.4 Lo que queda: UN grupo de nueve filas

    158 mr r10, r0      | mr r8, r0         bloque AnalogRightY
    159 extsh r0, r10   | extsh r0, r8
    162 li r10, 0xff    | li r8, 0xff
    165 stb r10,0x7(r29)| stb r8,0x7(r29)
    184 mr r11, r0      | mr r8, r0         bloque AnalogLeftX
    185 extsh r0, r11   | extsh r0, r8
    188 li r11, 0xff    | li r8, 0xff
    189 extsh r0, r11   | extsh r0, r8
    191 stb r0,0x4(r29) | stb r8,0x4(r29)

El objetivo da un registro **distinto** al `data` de cada bloque de eje --- RightX `r8`,
RightY `r10`, LeftX `r11` --- y nosotros reutilizamos `r8` en los tres. El bloque de RightX
**casa**, asi que no es una regla general: es que en el objetivo `r8` sigue ocupado cuando
nacen los otros dos.

La novena fila es su consecuencia: el objetivo guarda el resultado del `extsh` (`stb r0`) y
nosotros guardamos `data` (`stb r8`). **El pin directo EMPEORA**, que segun el catalogo
(`nfsmw-si-el-pin-empeora`) es sintoma de que queda una diferencia que **no** es de registro;
la candidata es justamente ese `stb r0`.

---

## 2. `zPlatform` NO estaba "a una funcion": tiene 48 funciones descolocadas

El encargo decia que `ActualReadJoystickData` era *"su UNICO bloqueo"*. **Es falso, y es un
agujero de la metrica, no del encargo**: `linkdelta` mide TAMANOS de seccion y **no ve el
orden**.

    permorden zPlatform: 88 de 136 en su sitio, 48 DESPLAZADAS,
                         10 ciclos no triviales (93, 8, 4, 3, 3, 3, 2, 2, 2, 2)

El ciclo de 93 es una **rotacion**: nuestro #1 es el #4 del objetivo, nuestro #4 el #10,
nuestro #10 el #133. Por eso `dolwhere` da 95.754 B con la funcion casi casada.

Lo mismo en zCamera: `permorden` da **79 desplazadas y 19 ciclos** (50, 37, 35, 35, 31, 24...),
y por eso su `dolwhere` son 333.533 B.

**Regla nueva para el dossier**: *"nueve secciones a IGUAL" + "una funcion en `fncmp`" NO es
"a una funcion"*. Hay que pasar `permorden` antes de llamar candidata a una unidad. Las dos
que cerraron en la r59 (zMain, zAnim) tenian el ORDEN cerrado ademas de las secciones a
IGUAL --- eso era lo que las hacia cerrables, y no estaba dicho.

---

## 3. `zCamera`: los 16 B ya estaban medidos desde la r50, y nadie los habia aplicado

`__static_initialization_and_destruction_0` emitia cuatro punteros de fila
(`addi r27, r30, 0x30/0x48/0x60/0x78`) que el objetivo no tiene: 16 B, los del `.text +16`.

La causa la dejo cerrada la r50 (`docs/analisis/r50-cam.md`, seccion 1.1): `cse_basic_block`
purga su tabla hash cada 1.000 insn (`cse.c:8942`) y a nosotros la purga nos cae **en mitad**
de la inicializacion de `ReplayCategoryTable`, asi que la equivalencia `base == r30 + K` no
sobrevive. Faltaban **43 insn de RTL** en el bloque extendido, y `expand_vec_init`
(`cp/init.c:2857`) las da gratis con un array de 20 elementos de constructor VACIO.

**La r50 lo midio y NO lo aplico** porque con la meta en `matched` los 16 B valian 0 B. Con la
meta en `linked` valen el `.text` entero, asi que va aplicado: `ICEReplay.cpp`, delante de la
tabla.

    3.620 B / 97,1121 % / 54 filas  ->  3.604 B EXACTOS / 99,8391 % / SEIS filas
    .text +16  ->  .text +0,  resto IGUAL,  y el resto de la unidad sin mover un byte

El array es un estatico sin referencias: `-strip-unused-data` se lleva `size & ~7` entero y
**no aporta ni un byte al enlace** (`linkdelta` lo confirma: `.bss` IGUAL).

Es un ANDAMIO declarado. La forma honesta, segun la propia r50, serian **~22 referencias
duplicadas** a un global o a una constante del pool dentro de la ventana
`Movers/Cubic.cpp` .. `ICEReplay.cpp`; nadie la ha encontrado.

Quedan las **seis** filas de `__static_init` (la permutacion de `HydraulicsLookAngle`,
`Movers/Cubic.cpp:355-362`, sin palanca segun la r50) y los dos `Update` con 15 insn cada uno.

---

## 4. `zPhysicsBehaviors`: el `bss +4` eran DOS errores que se anulaban

### 4.1 El diagnostico

El `+4` NO era un exceso de 4 B. Era:

    - 2.304 B  `_15SimpleRigidBody.mCollisionMap` NO se emitia en su sitio (era COMMON)
    +     4 B  el cadaver de `sSimpleRigidBodyInit`
    + 2.304 B  el COMMON, que el enlazador coloca al FINAL del .bss (0x804FE9F0)
    ---------
    +     4 B

Los dos hallazgos, cada uno con su cifra:

* **Un estatico de 1 B deja 4 B de cadaver.** `-strip-unused-data` se lleva `size & ~7`, que
  para 1 B es **cero**: el simbolo desaparece de la tabla y sus 1 + 3 B de relleno se quedan.
  `sSimpleRigidBodyInit` es el objeto ficticio de 1 B con el que disparabamos a mano el bucle
  que limpia el mapa de colisiones, y estaba justo donde el objetivo pone `mCollisionMap`.
* **El reciproco del `= {}` de la r55.** Aquella quito un inicializador para **dejar de
  definir** un simbolo y devolverselo al COMMON del original. Aqui hace falta lo contrario: el
  original define `mCollisionMap` de VERDAD, en su `.bss`, detras de `TheSimpleBodies`; el
  nuestro lo dejaba en definicion TENTATIVA (COMMON) y el enlazador lo mandaba al final.

### 4.2 La causa, y la prueba esta en el DWARF

`symbols/mw_dwarfdump.nothpp:8531` declara **`inline SimCollisionMap()`** con un bloque anonimo
de `unsigned int i` --- o sea, el mismo bucle que `Clear()`. Con constructor, el array **se
CONSTRUYE** (`expand_vec_init`), deja de ser tentativo y el objeto ficticio sobra.

Que nuestra fuente tuviera el bucle escrito a mano con la forma
`for (i = Max-1; i != -1; --i, ++p)` --- que es exactamente la que emite `expand_vec_init` ---
ya era la pista: alguien transcribio el asm sin darse cuenta de que venia de un constructor.

Detalle que cuesta 40 B: **el cuerpo del constructor hay que REPETIRLO, no llamar a
`Clear()`**. Con `Clear()` el constructor si se pliega, pero `Clear` sale **fuera de linea** y
el bucle queda en `bl Clear__15SimCollisionMap`: 4.424 B contra 4.464 y 14 filas.

### 4.3 El `.space` del relleno cambia de clase de resto

`zPhysicsBehaviors.cpp` lleva desde la r59 un `asm(".section .bss ... .space 376")` anonimo,
con la nota *"aqui el tamano esta CUANTIZADO en 32 --348 da bss-28 y 376 da bss+4--, asi que el
0 no es alcanzable"*. Es cierto **con el COMMON puesto**. Al meter los 2.304 B reales en su
sitio la clase de resto se desplaza 220 mod 32 = 28, y el 0 pasa a ser alcanzable:

    .space 376  ->  bss +224      (contenido real ya colocado)
    .space 152  ->  bss  IGUAL

**Leccion**: un `.space` de ajuste **caduca en cuanto cambia el contenido real de la seccion**,
y su nota de "el 0 no es alcanzable" caduca con el. Es la familia de `nfsmw-andamios-caducan`.

### 4.4 Lo que queda de verdad en su `.bss`

Con el COMMON arreglado, el `.bss` de nuestro objeto mide 28.480 B contra 28.256 del objetivo:
**+336 en la cabeza** (emitimos 39 parejas `hash.N`/`_.tmp_N` de estatico local donde el
objetivo tiene 10) y **-112 en la cola**. El `.space` los tapa. Ese es el frente real de datos
de la unidad y no estaba escrito en ningun sitio.

---

## 5. El `#line`: una cabecera compartida con `__LINE__` no se puede alargar

`SimTypes.h` la incluye medio juego por `ISimable.h`. El constructor va bajo
`#ifdef SIMCOLLISIONMAP_CTOR`, definido **en un solo sitio** (`zPhysicsBehaviors.cpp:14`), asi
que fuera de la guarda no se compila nada nuevo... y aun asi el `.o` de zPhysics **cambiaba
106 bytes**.

Los 106 son **inmediatos de numero de linea**, todos exactamente **+28** --- las 28 lineas que
anadi ---, porque `SimTypes.h` tiene macros con `__LINE__` mas abajo. La cura es una linea:

    #endif
    #line 134
        void Clear() {

Control con `.o` PRIVADOS (nunca en `build/GOWE69/src` de otro), compilando cada unidad con la
cabecera PRISTINA y con la editada, y comparando el sha1:

| unidad | sin la edicion | con la edicion + `#line` |
|---|---|---|
| zPhysics | `0ab50993ec35d55cf8949e3b6689f1d240be3059` | **identico** |
| zAI | `3dd4df6b3974997befe9dcd77b6edaf3bbaa3f2c` | **identico** |
| zGameplay | `b26b73d007b19d9fcda37469181a8bd7e7f431ff` | **identico** |
| zWorld | `fe6b45890f1e8b277c423af93c24d7e3a2bb1d82` | **identico** |
| zMisc | `da892bbf6d8dced6050df7d698a8ac60dea20e2e` | **identico** |
| zWorld2 | `3ff5ce83804d0f8185bdeeee8188e82e3b9c1fbd` | **identico** |
| zSpeech | `3b9138141bc33cef6e64440e6b73ab6e9820d847` | **identico** |

Control de determinismo hecho antes de creerselo: dos compilados privados del mismo arbol a
nombres distintos dan el **mismo** sha1, asi que la diferencia de 106 B no era ruido.

**Sin el `#line`, la guarda `#ifdef` NO basta**: es una regresion silenciosa en toda unidad que
incluya la cabecera. Y esto pone en duda el atajo de *"el macro solo se define en un sitio,
luego es inerte"* que usa la r59 con `IModel.h` --- **habria que comprobar si `IModel.h` tiene
`__LINE__` por debajo del punto de insercion**.

---

## 6. Lo que cambia del manual

1. **DOS `asm volatile` que ENVUELVEN una sentencia** colocan instrucciones del objetivo a los
   dos lados de ella. Un `asm` unico con las dos nunca puede: es un insn de RTL y el
   planificador no puede meter nada en medio. Reabre cualquier veda escrita como *"la
   instruccion que falta esta pegada al store"*.
2. **Meter una instruccion DENTRO del `asm` cuesta 4 B y CERO insn de RTL**; devolverla a la
   fuente cuesta 4 B **y un insn de RTL**, que es lo que rompe los empates de
   `allocno_compare`. Cuando quedan grupos de reparto en su sitio, el asm es **mas barato que
   la fuente**.
3. **Un estatico de 1 B deja 4 B en el enlace** (`size & ~7` = 0). Cualquier objeto ficticio de
   disparo de constructor esta pagando eso.
4. **Constructor contra COMMON**: darle constructor al elemento de un array estatico convierte
   una definicion tentativa en real **y en su sitio**. Es el reciproco exacto del `= {}` de la
   r55, y el DWARF dice cual de los dos toca.
5. **Un constructor que llama a `Clear()` deja `Clear` fuera de linea.** Repetir el cuerpo.
6. **`#line` tras un bloque guardado en cabecera compartida**, si la cabecera usa `__LINE__`
   por debajo.
7. **Un `.space` de ajuste caduca con el contenido**, y su clase de resto mod 32 se desplaza:
   "el 0 no es alcanzable" solo vale para el arbol de ese dia.
8. **`linkdelta` IGUAL no es "a una funcion"**: no ve el orden. `permorden` antes de prometer.
9. `register ... asm("frN")`, no `asm("fN")`. Y `asm("a; b")` no es `asm("a"); asm("b")`.

---

## 7. Ficheros tocados (sin commit)

    src/Speed/GameCube/Src/JoyE.cpp                              solo zPlatform
    src/Speed/Indep/Src/Camera/ICE/ICEReplay.cpp                 solo zCamera
    src/Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.cpp    solo zPhysicsBehaviors
    src/Speed/Indep/SourceLists/zPhysicsBehaviors.cpp            mia
    src/Speed/Indep/Src/Sim/SimTypes.h                           COMPARTIDA: guarda
        SIMCOLLISIONMAP_CTOR + #line 134; siete unidades ajenas medidas con .o privado,
        las siete IDENTICAS byte a byte.

Comprobado con `grep` sobre `src/` que los cuatro primeros los compila **una sola** unidad.

## 8. Lo siguiente

1. **zPlatform**: nueve filas, un solo grupo. El siguiente paso NO es un pin (empeora): es el
   `stb r0` contra `stb r8`, o sea encontrar la forma de fuente que deja `v = (short)data` vivo
   **sin** anadir un insn de RTL. Y despues, su frente de ORDEN (48 desplazadas).
2. **zCamera**: `HydraulicsLookAngle` (6 filas) y los dos `Update` (15 insn cada uno), mas 79
   funciones desplazadas.
3. **zPhysicsBehaviors**: el `.text +4` sigue VETADO. Su deuda real de datos son **+336 B de
   parejas `hash`/`_.tmp` de mas en la cabeza del `.bss` y -112 en la cola**, hoy tapados por
   el `.space 152`.
4. **Revisar `IModel.h`** (r59): si tiene `__LINE__` por debajo del
   `#ifdef IMODEL_ZMAIN_AQVECTOR_PRIMER`, la prueba de "cero regresiones" de aquella ronda esta
   incompleta por el mismo motivo que la mia lo estaba.
