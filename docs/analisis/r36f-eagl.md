# r36f --- EAGL4Anim, Gameplay y Platform

Territorio: `src/Speed/Indep/Src/EAGL4Anim/**`,
`src/Speed/Indep/Src/Gameplay/**` y lo que compila
`Speed/Indep/SourceLists/zPlatform`.

**Cerrado: `GenerateIndex__15GRaceParametersP14GRaceIndexData`, 1.680 B.**
Con ella **zGameplay se queda sin una sola funcion pendiente** (0 de 768).
Las otras cinco de la cola siguen abiertas, con 24 medidas negativas nuevas y
--lo que mas vale-- **dos hallazgos del volcado DWARF que no se habian mirado**:
uno cerro `GenerateIndex` y el otro reescribe el diagnostico de
`ActualReadJoystickData`.

## Verificacion

    python scripts/fndiff.py zGameplay GenerateIndex__15GRaceParametersP14GRaceIndexData
      -> # GenerateIndex__15GRaceParametersP14GRaceIndexData  target=100.0%  ours=100.0%  size=1680/1680

    python scripts/fncmp.py Speed/Indep/SourceLists/zGameplay
      -> 0 de 768 funciones con el CODIGO distinto -- 0 B que no cuentan
    python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
      -> 4 de 318 (identico a la entrada: Initialize, EvalSQT, FindMatchTime, EvalState)
    python scripts/fncmp.py Speed/Indep/SourceLists/zPlatform
      -> 1 de 136 (identico a la entrada: ActualReadJoystickData)

    python scripts/build_direct.py <las 33 SourceLists>   -> 33 ok, 0 fallidas
    python scripts/pctsnap.py --cmp antes.json despues.json
      -> EMPEORAN: ninguna
      -> MEJORAN: 1 funciones
         +0.964 pp  zGameplay  GenerateIndex...  99.0357 -> 100.0000  1680 B

    python scripts/lcfix.py --check   -> todas las entradas @lc estan al dia (0 correcciones)

Las instantaneas se tomaron con el arbol limpio (`git checkout` de mis cinco
ficheros) y con mis cambios, reconstruyendo **las 33 SourceLists** en ambos
casos, porque `PackedDecimal.h` entra en 19 unidades por `GRaceStatus.h`.

Ficheros tocados:

| fichero | que |
|---|---|
| `src/Speed/Indep/Src/Misc/PackedDecimal.h` | **funcional** |
| `src/Speed/Indep/Src/Gameplay/GRaceDatabase.cpp` | **funcional** |
| `src/Speed/GameCube/Src/JoyE.cpp` | solo comentario |
| `src/Speed/Indep/Src/EAGL4Anim/FnStatelessQ.cpp` | solo comentario |
| `src/Speed/Indep/Src/EAGL4Anim/FnRunBlender.cpp` | solo comentario |

(En el arbol hay ademas cuatro ficheros modificados por otro agente
--`uiMemcardBase.cpp`, `ENISVisualLook.cpp`, `EPursuitBreaker.cpp`,
`SteeringWheelDevice.cpp`-- que no son mios.)

## 1. `GenerateIndex` (1.680 B): CERRADA

Entraba al 99,0357 % con cinco filas: cuatro por la colocacion de dos
`li r31,0` y una por `ori r0,r30,0x200` contra `ori r0,r0,0x200`. Las dos
mitades son independientes y cada una necesito su palanca.

### 1.1 El tipo del DWARF NO es cosmetico: los campos de bits cambian la PLANIFICACION

El brief de la r36d ya decia que el tipo original es
`FloatingPoint<short int,10,3,5,11>` con `mExp:5` / `mMan:11` en vez de una
clase con `unsigned short mPacked`, pero se habia leido como un detalle de
nombres. **No lo es.**

Escribir el empaquetado como **dos asignaciones a campos de bits**

```c
mExp = exponent;
mMan = mantissa;
```

emite **exactamente las mismas cuatro instrucciones** que
`mPacked = ((exponent & 0x1F) << 11) | (mantissa & 0x7FF);`
(`rlwinm` / `rlwinm` / `or` / `sth`: GCC funde las dos lecturas-modificaciones),
pero **cambia el orden en que el planificador emite el bloque**:

```
              objetivo                          nuestro (con mPacked)
sth r0,0(r22)   ; copia al temporal      sth r0,0(r22)
li r27,0        ; exp = 0 del SIGUIENTE  li r27,0
lhz r0,8(r1)    ; copia del ANTERIOR     li r31,0
li r31,0        ; man = 0                lhz r0,8(r1)
sth r0,0(r21)                            sth r0,0(r21)
```

Las cuatro filas del `li r31,0` (dos copias del ctor inline) cayeron a cero a la
primera compilacion. Tres rondas las habian dado por «desempate de
`rank_for_schedule` sin mando desde la fuente»; el mando era el TIPO.

**Como se encontro y como repetirlo.** Monte un repro de un segundo (un `.cpp`
suelto con la clase, un `IDX` con el mismo layout y un `gen()` con tres
conversiones) compilado con los cflags reales de zGameplay. Reproducia el
patron `li | li | lhz | sth` exacto, asi que servia de oraculo. Barrido en el
repro, todo con el mismo numero de instrucciones salvo donde se indica:

| forma del empaquetado | orden `li`/`lhz` |
|---|---|
| `mPacked = ((exp & 0x1F) << 11) \| (man & 0x7FF)` | `li li lhz` (base) |
| `mPacked = (man & 0x7FF) \| ((exp & 0x1F) << 11)` | `li li lhz` |
| dos sentencias: `mPacked = ...<<11; mPacked \|= ...;` | `li li lhz` |
| `unsigned short mPacked : 16` (campo unico) | `li li lhz` |
| **`short mExp:5; short mMan:11;` + `mExp=exp; mMan=man;`** | **`li lhz li`** |
| lo mismo con las dos asignaciones al reves | `li lhz li` |
| lo mismo en la misma linea (`mExp = e, mMan = m;`) | `li lhz li` |
| campos de bits pero guardando con `*(unsigned short*)this = ...` | `li li lhz` |

O sea: **lo que decide es que la sentencia sea una asignacion a un CAMPO DE
BITS**, no el layout ni el numero de stores.

### 1.2 `operator float()` tambien tiene que leer los campos

Con los campos declarados, `operator float()` no puede seguir leyendo
`mPacked`. Dos formas medidas sobre `GetReputation` / `GetCashValue` /
`GetChallengeGoal` (que estaban al 100 % y hay que no romper):

| forma | GetReputation |
|---|---|
| local `unsigned short mPacked = *(const unsigned short*)this;` | 340 B (86,96 %) --- el temporal comparte el `slwi` entre las dos ramas y el objetivo lo repite |
| `int exponent = mExp; ... if (exponent < 0)` | 344 B (93,81 %) --- reaprovecha el CR del `srawi.` con `mcrf`+`bge` |
| **`int exponent = mExp; ... if (mExp < 0)`** | **336 B, 100 %** |

La clave es la **condicion**: leyendo el campo otra vez, `combine` la vuelve a
convertir en el `andi. r11, 0x8000` del objetivo; leyendo la local, se queda con
el CR que ya tiene. Y `slwi r0,r11,21 ; srawi r0,r0,21` (la lectura de `mMan`
con signo) sale identico al objetivo sin escribirlo a mano.

### 1.3 El derrame de campo de CR y la barrera de cero bytes

Con los campos de bits el tamano subia a 1.692 B: en la **tercera** copia inline
del ctor, el asignador deja de darle cr2/cr3/cr4 al `negative` y reload lo saca
por un GPR (`cmpwi cr7` + `mfcr r26` + `slwi r26,r26,28` + `mtcrf 128,r26`).
Solo pasa en la tercera; las dos primeras usan cr4 como el objetivo.

**Ocho formas naturales, todas 1.692 B** (`if (negative)` sin llaves,
`0 - mantissa`, `0.0f - value`, comparacion invertida, `negative != false`,
`int negative`, `negative = value < 0.0f;` con el `if` aparte, e intercambiar
las dos sentencias del primer `if`). **Cuatro barreras selectivas lo quitan**
--todas de cero bytes y todas dejan el mismo resultado (1.680 B, 1 fila)--:

- `__asm__("" : "+r"(negative))` detras de `negative = true;`   <- la que dejo puesta
- `__asm__("" : "+r"(exponent))` entre las dos asignaciones de campo
- `__asm__("" : "+f"(value))` delante del `if (negative)`
- `__asm__("" : "+r"(negative) : "r"(mantissa))` detras del `if (negative)`

Negativos del mismo barrido: `"+r"(mantissa)` delante del empaquetado deshace la
ganancia de los campos (1.680 B pero 11 filas), `"+r"(mantissa)` ENTRE las dos
asignaciones se desmadra (1.752 B, 64 filas) y `"+r"(mantissa) : "r"(negative)`
da 85 filas.

### 1.4 La quinta fila: `ori r0,r0` contra `ori r0,r30`

`cse` canonicaliza el temporal al registro de `flags` porque `make_regs_eqv`
deja como canonico el pseudo que **vive mas**, y `flags` vive hasta el final de
la funcion. Hacen falta **dos cosas a la vez** y cada una sola es peor que la
base:

```c
bool rank = GetRankPlayersByPoints();
unsigned int f = flags;
if (rank) {
    __asm__("" : "+r"(f));
    f |= GRaceIndexData::kRaceFlag_RankPlayersByPoints;
}
*pflags = f;
```

- el temporal `rank` **para la llamada**: sin el, la barrera coalesce `f` con
  `flags` y desaparece el `mr r0,r30` (1.676 B, 38 filas);
- la barrera **dentro** del `if`, delante del `|=`: fuera del `if` adelanta el
  `mr` una ranura (1.680 B, 2 filas) y sobre el ternario no cambia nada.

Barrido de la ronda sobre esa sentencia (todos con `filas` = instrucciones
distintas, no reubicaciones):

| forma | % | tamano | filas |
|---|---|---|---|
| ternario (base de la r36d) | 99,988 | 1.680 | 1 |
| `bool rank` + `f = index->mFlags` + `if` (carga tras la llamada) | 99,857 | 1.680 | 1 (sale `lwz` donde el objetivo tiene `mr`) |
| `bool rank` + `f = flags` + `if` | 99,988 | 1.680 | 1 |
| `f = index->mFlags` + `if` (sin `rank`) | 99,488 | 1.680 | 4 |
| `f = flags` + `asm("+r")` + `if` (sin `rank`) | 99,726 | 1.676 | 3 |
| `bool rank` + `f = flags` + `asm("+r")` ANTES del `if` | 99,524 | 1.680 | 2 |
| `bool rank` + `asm("+r"(f) : "r"(rank))` antes del `if` | 99,524 | 1.680 | 2 |
| `if/else` explicito con `f = flags` en las dos ramas | 99,262 | 1.684 | 5 |
| `*pflags \|= K` dentro del `if` | 97,233 | 1.688 | 62 |
| `index->mFlags \|= K` dentro del `if` | 97,233 | 1.688 | 62 |
| ternario sobre `*pflags` | 98,952 | 1.688 | 12 |
| renombrar el resto de `flags` a `f` | 99,726 | 1.676 | 3 |
| ... + `asm("" : : "r"(flags))` detras del store | 98,690 | 1.680 | 25 |
| **`bool rank` + `f = flags` + `asm("+r")` DENTRO del `if`** | **100,0** | **1.680** | **0** |

### 1.5 TRAMPA: la plantilla desplaza los `$LC` en 19 unidades

El DWARF pone el ctor y el `operator float()` **dentro** de la plantilla
`FloatingPoint<short,10,3,5,11>` (con `PackedDecimal` como `typedef`). Lo
escribi asi primero y **da exactamente el mismo codigo** (100 %, 1.680 B, y
`fncmp` de zGameplay a cero igual).

Pero `lcfix.py --check` pasa de **0 a 344 correcciones en 19 unidades**
(zAI, zAnim, zCamera, zEAXSound, zEAXSound2, zEcstasy, zFe, zFe2, zFeOverlay,
zGameplay, zLua, zMain, zPhysics, zPhysicsBehaviors, zPlatform, zSim, zSpeech,
zWorld, zWorld2): la plantilla consume **cuatro numeros `$LC` menos** en cada TU
que incluye `GRaceStatus.h`, y el contenido de `.rodata` es identico byte a byte
--solo cambia la NUMERACION--, asi que `keep.lst` se queda apuntando a otra
cadena y el DOL se rompe en silencio.

Bisecado con `lcfix.py --check zPlatform` (0 = bien):

| version de `PackedDecimal.h` | correcciones |
|---|---|
| campos de bits + guardado por cast + `operator float()` con local | 0 |
| campos de bits + `mExp=`/`mMan=` + `operator float()` con `mExp < 0` (clase) | 0 |
| lo mismo movido DENTRO de la plantilla + `typedef` | 10 |
| lo mismo + comentarios (version final de plantilla) | 10 |

**Dejo la version con `class PackedDecimal` suelta** (campos de bits dentro de
la clase, plantilla `FloatingPoint` solo con los dos estaticos, que es como
estaba). Codigo identico, `lcfix` limpio. Queda anotado en la propia cabecera
para que nadie la «arregle» convirtiendola en plantilla.

**Regla nueva para el proyecto**: un cambio de cabecera puede mover los `$LC` de
unidades que ni siquiera estas mirando. Si tocas una cabecera compartida,
`lcfix.py --check` **entero** (no solo tu unidad) forma parte de la
verificacion, y hay que reconstruir todas las SourceLists antes, porque con
objetos rancios da 0 falsamente.

## 2. `EvalSQT` (1.480 B, sobran 8 B): por que GCC no sabe que `r5` viene extendido

Las dos unicas diferencias siguen siendo dos `clrlwi r0, r0, 16`. Diagnostico
nuevo, y esta vez con el mecanismo:

El objetivo hace `subi r0, r5, 0x1 ; cmpw r30, r0` con `r5` crudo del `lhz`.
**Nosotros tambien lo hacemos donde el pseudo tiene una sola definicion**: unas
lineas mas abajo, `mNumKeys - 2` sale `lhz r9 ; subi r8, r9, 0x2` sin extension
y casa. Lo que falla es el pseudo de ESTA comparacion, que tiene **DOS
definiciones**: el `lhz r5` que PRE mete en la rama `floorTime < 0` y el
`mr r5, r0` de la otra. `set_nonzero_bits_and_sign_copies` corre con
`nonzero_sign_valid == 0`, asi que al procesar una definicion por COPIA
`nonzero_bits` devuelve la mascara entera y el pseudo pierde la propiedad de
estar extendido. Con el cast la extension va detras de la resta (2 filas); sin
el cast va delante y salen 17.

Nueve formas nuevas, encima de las 19 de la r36e:

| forma | % | tamano | filas |
|---|---|---|---|
| base (con `static_cast<unsigned short>`) | 99,459 | 1.488 | 2 |
| `(int)mNumKeys - 1` | 98,051 | 1.488 | 17 |
| `mNumKeys - 1 <= floorKey` | 98,011 | 1.488 | 18 |
| sin cast solo en la primera copia | 98,132 | 1.488 | 12 |
| local `unsigned short numKeys` justo antes | 98,132 | 1.488 | 12 |
| local `int numKeys` justo antes | 98,132 | 1.488 | 12 |
| `numKeys` izado al principio de `if (!mTimes)` | 96,443 | **1.476** | 20 |
| `numKeys` izado FUERA de `if (!mTimes)` | 93,208 | **1.480** | 46 |

El izado completo **da el tamano exacto** (quita las dos extensiones) pero
rompe otras 46 filas, porque el objetivo **RECARGA** `statelessQ->mNumKeys` con
`lhz` en cinco sitios distintos y el izado deja una sola carga. La palanca, si
existe, es quitarle la definicion por copia al pseudo **sin** quitar las
recargas; ninguna forma de la sentencia lo consigue.

## 3. `FindMatchTime` (720 B, mismo tamano): sigue siendo el `lfd` que no sube

Confirmado el diagnostico de la r36e y acotado con el mapa de lineas: el `lfd`
de la constante magica 0x4330000080000000 esta, en el objetivo, en
`0x8009E138`/`0x8009E144`, que el mapa imputa a la **linea 432** del original
--o sea a la sentencia `n = FloatToInt(...)`, el PRIMER bloque--, y nosotros lo
emitimos dentro del `if`, detras del `bso`. Las otras 7 filas cascadean de ahi
(f12 contra f13, r9 contra r10).

Ocho formas nuevas, encima de las seis de la r36e:

| forma | % | tamano | filas |
|---|---|---|---|
| base | 97,361 | 720 | 8 |
| `input.mSearchLength < (float)n` | 97,361 | 720 | 8 |
| sin llaves (la forma del original, por el mapa de lineas) | 97,361 | 720 | 8 |
| `float sl = input.mSearchLength;` local | 97,361 | 720 | 8 |
| `__asm__("" : "+f"(angle))` delante del `if` | 97,361 | 720 | 8 |
| `angle`/`dAngle` DESPUES del `if` | 92,244 | 724 | 43 |
| `__asm__("" : "+r"(n))` delante del `if` | 96,833 | 720 | 27 |
| `n > input.mSearchLength` | 97,278 | 720 | 10 |
| `0.0f < ... && (float)n > ...` | 97,278 | 720 | 10 |

Las locales del DWARF (`phase`, `a`, `da`, `na`, `n`, `s`, `i`, `minIdx`,
`diffAngle`, `minAngle`, `angle`, `dAngle`, `nonConstThis`) coinciden una a una
con las nuestras, y el mapa de lineas dice que el original tiene el `if` sin
llaves y sin linea en blanco delante --formato, no codigo--. **La veda queda
donde estaba**: es un hundimiento entre bloques de una cantidad que genera el
compilador, sin variable C a la que atar la barrera.

## 4. `ActualReadJoystickData` (1.588 B, faltan 8 B): el DWARF reescribe el diagnostico

Esto es lo que dejo mas avanzado para la proxima ronda.

**El volcado DWARF acota la forma original mucho mas de lo que se creia.** El
bloque `if (pad_state == 0)` es `0x8026C6B4..0x8026C878` y **no declara `v`**:
su unica local es

    short data;   // r0

**una sola para las cuatro palancas**, no las cuatro `int data` que tenemos ni
el `int v` que las envuelve. Y el mapa de lineas dice que las dos
instrucciones que faltan --`mr r9,r0` (0x8026C86C) y `andi. r11,r9,0x8000`
(0x8026C874)-- son la **linea 307** del original, justo detras del `stb` de
`stickX` (linea 306) y **ultima del ambito de `data`**.

Lo importante: **detras del `andi.` no hay ningun salto**. Como en PPC `andi.`
es la unica and-inmediata, eso significa que la sentencia **no es un `if`**: es
una **asignacion** (`data = data & 0x8000;`) cuyo resultado es el pseudo que
alimenta los cinco `mr rX,r11` de despues. Nuestro `if (v & 0x8000) v = 0;` lo
borra DCE porque `v` esta muerto.

Medidas de la ronda (todo revertido):

| forma | % | tamano |
|---|---|---|
| base | 99,244 | 1.580 |
| **una sola `short data`** para las cuatro palancas | 96,383 | **1.564** |
| `v = v & 0x8000;` + `asm("" : : "r"(v))` al final del bloque | 97,594 | 1.584 |
| `v = v & 0x8000;` + `asm("" : "+r"(v))` detras | 98,476 | 1.576 |
| `if (v & 0x8000) v = 0;` + `asm("" : "+r"(v))` detras | 99,244 | 1.580 (IDENTICO a la base) |

La ultima da el mismo numero exacto que la base: por el aviso del brief, eso es
**DCE llevandose la pareja**, no «no funciona».

Y la `short data` unica no vale tal cual: mete un `extsh` por conversion y se
lleva los `mr rX,r0` + `extsh r0,rX` que el objetivo **si** tiene. O sea que
`data` (short) **convive con un temporal entero sin nombre**: en
`addi r0,r8,0x80 ; andi. r9,r0,0x8000` el valor probado es el INT (no hay
`extsh` delante), y solo en `mr r8,r0 ; extsh r0,r8 ; cmpwi r0,0xff` aparece el
short. La forma que hay que encontrar es una que declare **una sola `short
data`** y deje el int como subexpresion --lo mas probable es una macro, dado
que las cuatro palancas son identicas--.

Siguiente paso concreto: encontrar el USO real de `data & 0x8000` en el
original (los cinco `mr` estan en los dos lados, solo cambia que pseudo copian),
que es lo unico que impide que DCE se lo lleve sin `asm`.

## 5. `EvalState` (456 B) y `Initialize` (2.352 B)

- **`EvalState`**: no la toque. El DWARF confirma las cuatro locales del
  original (`c` r30, `i` r31, `k` r4, `dest`) y **no** trae la `keyIdx` que
  anadio una ronda anterior, pero la r36e ya midio que quitarla empeora
  (89,87 % / 460 B) y que reutilizar `i` como indice es peor todavia
  (83,41 % / 452 B). Es permutacion pura; toca permutador o pin, no fuente.
- **`Initialize`**: frente cerrado con 622 medidas en la r36e. **No lo barri.**

## 6. Palancas del brief usadas en esta ronda

| palanca | donde | resultado |
|---|---|---|
| 1 barrera selectiva `"+r"` | `PackedDecimal.h` (derrame de CR) | **CIERRA**, 4 sitios equivalentes |
| 1 barrera selectiva `"+r"` | `GenerateIndex` (`ori`), dentro del `if` | **CIERRA**, y solo dentro del `if` |
| 1 barrera, eje «a quien» | `GenerateIndex`, `FnStatelessQ` | negativo (2 y 85 filas) |
| 2 cantidad fantasma | --- | no aplica |
| 3 pin de registro | --- | no aplica |
| 4 barrera de ranura | `FindMatchTime` | negativo (`"+f"(angle)` neutro, `"+r"(n)` 27 filas) |
| 5 `asm()` de fichero | --- | no aplica |
| 6 guardas de cabecera | mirado en `PackedDecimal.h`/`StatelessQ.h`: no hay `#ifdef` sin encender |

## 7. Propuestas fuera de territorio

1. **`config/GOWE69/keep.lst` --- NADA que hacer.** Lo dejo escrito porque
   estuvo a punto de hacer falta: la version de `PackedDecimal.h` con plantilla
   habria necesitado 344 correcciones de `lcfix.py`. La version que dejo NO las
   necesita (`lcfix --check` da 0). Si alguien vuelve y convierte
   `PackedDecimal` en `typedef` de la plantilla --que es lo que dice el
   DWARF--, hay que correr `python scripts/lcfix.py` a continuacion y
   comprometer `keep.lst`.

2. **`scripts/pctsnap.py` y `scripts/lcfix.py`, aviso de objetos rancios.**
   `lcfix.py --check` da 0 con objetos viejos y 344 despues de reconstruir; no
   avisa de que el `.o` es mas antiguo que la fuente. Un chequeo de mtime
   (`.o` mas viejo que su `.cpp` o que `build.ninja` -> avisar) ahorraria
   exactamente el susto que me lleve.

3. **`src/Speed/Indep/Src/EAGL4Anim/StatelessQ.h`** tiene un metodo
   `Uncompress2Bits` que el volcado DWARF **no** trae (el original solo lista
   `GetAttributeBlock`, `GetNumFrames`, `GetData`, `GetFrameData`,
   `GetConstData`, `GetConstBoneIdx`, `ComputeSize`, `Compress2UShort`,
   `Uncompress2Float`, `GetFnOffset`, `GetFnLocation`, `InitAnimMemoryMap`).
   Es inline y sin llamantes, asi que no emite nada, pero es una cabecera que
   miente y conviene apuntarlo.

## 8. Lo que aprendi que vale para otras unidades

1. **El TIPO del DWARF es codigo, no documentacion.** Un campo de bits contra
   un entero del mismo tamano da las mismas instrucciones y **otra
   planificacion**. Antes de dar por «desempate del planificador sin mando» un
   near-miss, comprobar en `mw_dwarfdump.nothpp` que los tipos de los miembros
   que toca la funcion son los nuestros.
2. **El mapa de lineas dice a que SENTENCIA pertenece cada instruccion que
   falta.** En `JoyE.cpp` dijo que la sentencia ausente es la linea 307 y que
   detras del `andi.` no hay salto --o sea que no es un `if`--; eso solo se ve
   ahi.
3. **El repro de un segundo funciona tambien para la planificacion.** Un `.cpp`
   suelto con la clase, el struct destino con el mismo layout y una funcion que
   repita las llamadas reproducia el orden `li | li | lhz | sth` exacto. Ocho
   formas barridas en menos de un minuto contra los 20 s por medida de la
   unidad entera.
4. **Un cambio de cabecera compartida puede mover los `$LC` de 19 unidades sin
   cambiar un solo byte de `.rodata`.** `lcfix.py --check` ENTERO, y con todas
   las SourceLists reconstruidas, o no vale.
