# r60 — `resto` (zFe, zFe2, zAI, zSpeech, zLua): el bloque de cabecera VERBATIM se paga con el pool, y las cadenas muertas se colocan con un `asm()` de fichero

Agente `resto`. **Cinco ficheros tocados, los cinco mios**, ninguna cabecera compartida,
nada de `config/`, nada de `scripts/`:

    src/Speed/Indep/SourceLists/zFe.cpp        <- edicion
    src/Speed/Indep/SourceLists/zSpeech.cpp    <- edicion
    src/Speed/Indep/SourceLists/zLua.cpp       <- edicion
    src/Speed/Indep/SourceLists/zAI.cpp        <- edicion
    src/Speed/Indep/SourceLists/zFe2.cpp       <- SOLO COMENTARIOS (dos negativos, uno es veda)

---

## 0. Titular

Metrica: **bytes distintos en los rangos de `splits.txt` de la unidad**, contra el enlace
base, con una **copia privada de `keep.lst` ya pasada por `lcfix`** (§6). Entre parentesis, la
direccion de la **primera** diferencia de `.rodata` — la metrica de escalera.

| unidad | antes | despues | gana | prefijo exacto de `.rodata` | secciones |
|---|---:|---:|---:|---|---|
| **`zFe`** | 27.861 | **27.181** | **−680** | 803DF5F4 → **803DF93C** (**+840 B**) | IGUAL → IGUAL |
| **`zLua`** | 10.240 | **9.533** | **−707** | 803ED42C → **803ED5B5** (**+393 B**) | rodata−728 → **rodata−480** |
| **`zAI`** | 40.284 | **40.215** | **−69** | 803C8D89 → **803C8E51** (**+200 B**) | rodata−256 → rodata−272 |
| **`zSpeech`** | 11.765 | **11.753** | **−12** | 80406220 → **804062E4** (**+196 B**) | IGUAL → IGUAL |
| `zFe2` | 35.959 | 35.959 | 0 | — | IGUAL |

`fncmp` **IDENTICO antes y despues en las cinco**: zFe 0/921, zFe2 0/1307, zAI 0/1030,
zSpeech 1/703 (`Setup`, la vetada de siempre), zLua 0/537. **Cero regresiones de codigo.**

Sello, **tres compilaciones seguidas del arbol final, las tres iguales**:

    zFe      1cdb405a2eda9fceba7a0289dabef87f47c446f2   (x3)
    zFe2     4b40d3ef01154ce3df198375eeeda857d3ac0624   (x3, = el de partida: solo comentarios)
    zAI      30d0b19cfd6f4dac57ff731f086807ef79b3d655   (x3)
    zSpeech  3b9138141bc33cef6e64440e6b73ab6e9820d847   (x3)
    zLua     650611dfa9b26b3011d25ede22a9f0ab5c29e8de   (x3)

**Ninguna promociona.** `trypromo` con la ruta completa, las cuatro editadas:

    Speed/Indep/SourceLists/zFe        DOL ROTO (63f5620fbce5)
    Speed/Indep/SourceLists/zSpeech    DOL ROTO (8e25b6c7c23b)
    Speed/Indep/SourceLists/zAI        DOL ROTO (359ad3eadfb9)
    Speed/Indep/SourceLists/zLua       DOL ROTO (cca8853188bd)

**Las cuatro dependen de que se aplique `lcfix`** (311 correcciones, §6).

---

## 1. EL HALLAZGO: un bloque de cabecera VERBATIM puede ser el ARRANQUE DEL POOL

La receta de la r57/r58 —leer N bytes del objetivo en la direccion del `lbl_` y pegarlos como
`asm()`— **valio 1,35 MB y no se discute**. Pero tiene un limite que nadie habia nombrado, y
lo he medido en `zFe`:

**si los bytes que pegas son literales que cc1plus emite igual, no estas escribiendo cabecera:
estas DUPLICANDO el arranque del pool, y el pool entero de la unidad sale N bytes tarde.**

En `zFe` el bloque `lbl_803DF568` medía `0x8C`. Los ultimos 48 B (803DF5C4..803DF5F4) los pego
la r58 «VERBATIM del objetivo» para cuadrar `rodata−48 → IGUAL`. Son, uno a uno:

    803DF5C4  00 00 00 00                          <- el $LC "" 
    803DF5C8  "Attrib::Attribute"                  <- $LC de _AttribAllocTagOrder
    803DF5DC  "Attrib::Instance"                   <- idem
    803DF5F0  "Attr"                               <- los 4 primeros B de "Attrib::Definition"

Los tres los emite cc1plus **ademas**, detras del bloque. Sumados a la `"GAMECUBE"` duplicada
(`bGetPlatformName`, 12 B), nuestro `"Attrib::Attribute"` caia en **0x803DF604** y el objetivo
lo tiene en **0x803DF5C8**: **60 B de corrimiento**, y con el todo el pool.

La cura son dos lineas:

    .size lbl_803DF568, 0x8C   ->   0x5C          (el bloque vuelve a ser SOLO cabecera)
    #define BWARE_PREFIX_GAMECUBE (_bwarePrefix)  (la GAMECUBE ya esta en +0)

y el pool arranca en 0x803DF5C4 **exactamente donde el objetivo**. Medido: las ocho etiquetas
de AttribSys casan byte a byte, `Attribute / Instance / Definition / Class / Database /
TypeDesc / RefSpec / Blob`, y la primera diferencia salta de **803DF5F4 a 803DF669**.

### 1.1 El deficit que la r58 media NO estaba donde lo tapo

Al acortar el bloque, `.rodata` cae a −64. **Eso NO significa que el bloque hiciera falta**:
significa que a la unidad le faltan 64 B de contenido **en otro sitio**. La prueba es que
poniendo esos 64 B de relleno **al final** de la `.rodata` de la unidad —donde no desplazan
nada— la seccion vuelve a `IGUAL`, `.bss` vuelve a 0 diferencias y el prefijo exacto se
QUEDA en 803DF669. O sea: **el bloque estaba pagando el tamano correcto con los bytes en el
sitio equivocado.**

Es la misma trampa que el DOSSIER llama «la ventana de 32 bytes», vista por el otro lado: un
`rodata−64` mueve el arranque de `.data` y `.bss` y **dispara miles de diferencias que no son
tuyas**. En zSpeech me costo media hora creer que apagar cinco etiquetas «rompia la `.bss`»:
no rompia nada, la `.bss` entera se habia ido 32 B. **Con `linkdelta` en IGUAL, `.data` y
`.bss` volvian a cero solas.**

> Regla operativa: **cada vez que quites bytes del pool, compensa el mismo numero al FINAL de
> la `.rodata` de la unidad ANTES de leer ninguna otra cifra.** Sin eso, todas las demas mienten.

### 1.2 Y en `zFe2` la misma receta es NEGATIVA — con veda

`zFe2` tiene el mismo bloque, con **864 B** pegados por la r58 (803E43DC..803E473C). Acortarlo
a `0x5C` + `BWARE_PREFIX_GAMECUBE` + 880 B de relleno al final: la `.rodata` vuelve a IGUAL,
pero **el prefijo exacto EMPEORA** —la primera diferencia RETROCEDE de 803E473C a 803E4481— y
el total sube de **35.959 a 42.147**. Ahi el bloque verbatim esta ganando 864 B de prefijo
gratis; lo que sobra son las copias que cc1plus emite DETRAS.

La cura de esas copias existe y tiene nombre: `#define ATTRIB_TAG_ORDER_HAND_POOL 1`, la guarda
que `AttribSys.h` reserva textualmente para «las unidades cuyo bloque ya las trae».
**NO SE PUEDE EN zFe2, y es una veda de fuente**: quitar el `inline _AttribAllocTagOrder` corre
el `DECL_UID` y los alias escritos a fuego en `zFe2.cpp` (`....32013`, `....32014`, `....32030`
de `FEPlayerCarDB`) dejan de resolver — **el ENLACE FALLA con tres L0039**. Es exactamente el
aviso de la r59 §6 («el sufijo `.NNNNN` es `DECL_UID`»), cobrado. Hay que pasar `mangfix.py`
antes, y eso es trabajo de ventana.

Los dos negativos estan escritos en `zFe2.cpp`, donde `previo.py` los encuentra.

---

## 2. LA PALANCA NUEVA: `asm()` de fichero para colocar una cadena MUERTA en su direccion

Las rondas r52/r53 dejaron en `zFe`, `zFe2` y `zSpeech` bloques `asm` de `.asciz` con **las
cadenas que el objetivo tiene y nuestro arbol no referencia** —117 cadenas / 1.846 B en zFe,
13 / 239 B en zSpeech—, y los pusieron **AL FINAL a proposito**, «asi no desplazan ningun `$LC`
y no hay que tocar `keep.lst`».

Eso cuadra el tamano y **deja el contenido a kilobytes de su sitio**. Pero el mecanismo 1 del
DOSSIER dice que **un `asm()` de fichero se emite DONDE ESTA ESCRITO**, asi que esas mismas
cadenas se pueden PARTIR y colocar una a una en su direccion exacta, **sin coste y sin tocar
`keep.lst`** (son bytes anonimos: `-strip-unused-data` va por simbolo y no los ve).

Es la tercera via, y hacia falta porque las otras dos no llegan:

| la cadena esta... | herramienta |
|---|---|
| VIVA en la unidad, pero mas adelante | **primer de pool** (`static inline` muerta); solo mueve hacia ATRAS |
| MUERTA pero emitida (la trae un `#include`) | no vale: `-strip-unused-data` deja `size & ~7` |
| MUERTA y no emitida | **`asm(".asciz ...")` escrito en el punto exacto** |

Medido, cadena a cadena:

    zSpeech  "1.8.1"                        cola -> su sitio    1a dif. 80406220 -> 80406228
    zSpeech  "GRaceStatus"                  cola -> su sitio    1a dif. 80406284 -> 804062E4
    zFe      MGeneric MAudioReflection
             PlayerNum Dist Covered         cola -> su sitio    1a dif. 803DF80C -> 803DF898
    zFe      "16.1.0" "1.8.1" "GRaceStatus" cola -> su sitio    1a dif. 803DF898 -> 803DF93C
    zLua     "MGeneric"                     NUEVA               1a dif. 803ED42C -> 803ED460
    zLua     "LuaPostOffice"                NUEVA               1a dif. 803ED598 -> 803ED5B5

**Dos detalles que cuestan una compilacion cada uno si no los sabes:**

1. **`.balign 4` SIEMPRE.** Sin el, `"1.8.1"` cayo en 0x8040621E (justo detras de `"1.2.3"`,
   que acaba en byte impar) y la primera diferencia RETROCEDIO 2 B. Con el, cae en 0x80406220
   clavado.
2. **Un `#include` de clase NO sustituye al `asm`.** En zLua metí
   `Generated/AttribSys/Classes/milestonetypes.h` para colocar `"Attrib::Gen::milestonetypes"`:
   como en zLua nadie la referencia, el `$LC` es MUERTO y el enlace dejo el cadaver `"pes"`
   (4 B) donde el objetivo tiene los 28 enteros. Cambiado a `asm(".asciz ...")`, entra entera.

### 2.1 Y para bloques grandes, VERBATIM en hexadecimal

En zLua los 148 B de 0x803ED4C4..0x803ED558 —`"LuaRuntime temp compression buffer"`,
`"LuaRuntime VM reset snapshot"` y los nombres de tipo de Lua (`NIL BOOL LUD NUMBER STRING
TABLE FUNC UD THREAD (?)`)— son todos muertos. Pegados como `.4byte` **en el punto exacto**
valen de golpe: `.rodata −632 → −488`, primera diferencia **803ED4C4 → 803ED598**, y 164 B
menos de `.rodata` distinta. **Es la receta del prefijo de la r57, pero aplicada EN MEDIO del
pool en vez de en el byte 0.**

---

## 3. El censo de PARASITOS, y por que apagarlos no es gratis

`parasitos.py` (r59) sobre mis cinco unidades: cadenas `$LC` que emitimos y el objetivo **no
tiene en el rango de la unidad**, con el cadaver que deja `size & ~7`:

| unidad | cadenas | B | cadaver |
|---|---:|---:|---:|
| zFe | 25 | 807 | **132** |
| zSpeech | 35 | 494 | 60 |
| zLua | 34 | 421 | 96 |
| zFe2 | 18 | 432 | 96 |
| zAI | 17 | 274 | 56 |

Guardas aplicadas esta ronda (todas ya existian en el arbol, todas inertes por defecto):

| guarda | mata | puesta en |
|---|---|---|
| `HAND_POOL_TAG(s) ((const char *)0)` | `EventSequencerSystems`, `EAGL4::SymbolEntry` | zFe, zSpeech, zLua, zAI |
| `DEAD_STR_DONE(s) ((const char *)0)` | `"done"` | zFe, zLua, zAI |
| `WORLDCONN_NO_EMITTERSYSTEM` | `Attrib::Gen::emitterdata`/`emittergroup` | zFe, zLua |

**Comprobacion obligatoria antes de `HAND_POOL_TAG`**: la macro mata DOS literales, y uno de
ellos puede ser del objetivo. En zSpeech y zLua verifique en el ELF **enlazado** que
`EAGL4::SymbolEntry` no aparece **ni en el objetivo ni en el nuestro** (0 copias en los dos
lados) antes de apagarla.

### 3.1 La supresion SELECTIVA de una etiqueta de asignador, sin tocar cabeceras ajenas

`chassis / induction / nos / tires / transmission` entran por `Physics/PhysicsInfo.hpp`, que es
cabecera de otro, y sus `USE_ATTRIB_ALLOC` internan cinco literales parasitos (28 B de cadaver
en zSpeech, 32 en zFe). `ATTRIB_TAGS_HAND_POOL` los mata TODOS y no sirve: la unidad necesita
casi todos los demas.

El truco, que no cuesta ni una edicion en cabecera ajena y **es reutilizable en cualquier
unidad**:

```c
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"   // ya parseada: no-op
#undef  ATTRIB_TAG
#define ATTRIB_TAG(s) ((const char *)0)
#include ".../Classes/chassis.h"        // guarda de inclusion: la
#include ".../Classes/induction.h"      // inclusion transitiva de
#include ".../Classes/nos.h"            // despues es un no-op
#include ".../Classes/tires.h"
#include ".../Classes/transmission.h"
#undef  ATTRIB_TAG
#define ATTRIB_TAG(s) s
```

**Dos controles que hice y merecen quedar escritos:**

* con las cinco cabeceras pero **sin** apagar la etiqueta, la medida es **identica al byte**
  (11.757 antes y despues, `.bss` 0): o sea que **adelantar el parseo de esas cinco clases no
  cuesta nada**, y lo que se mide despues es la supresion y nada mas;
* apagarlas da `rodata −32`, y **todo el estropicio aparente de `.data` (553 B) y `.bss`
  (6.361 B) es la ventana de 32**; con el relleno puesto, `.bss` vuelve a **0**.

Da igual poner `((const char *)0)` que `(_bwarePrefix + 0x34)`: **medida identica**, luego no
es la forma del codigo, es que la cadena desaparece.

---

## 4. Lo que quedo en cada unidad, y cual es el paso siguiente

### `zFe` — la de mas recorrido, y la que mas pago

Cinco ediciones, todas medidas por separado:

    bloque 0x8C -> 0x5C  +  BWARE_PREFIX_GAMECUBE           803DF5F4 -> 803DF669
    orden del objetivo en las Classes/*.h                   803DF669 -> 803DF7D1
      (simsurface, ecar, camerainfo, effects, audioimpact, audioscrape, WorldConn,
       y milestonetypes/speechtune/speech BAJAN detras; ecar.h y camerainfo.h
       no estaban en el arbol de la unidad y sus dos Attrib::Gen SI son del objetivo)
    pvehicle.h + engine.h + engineaudio.h                   803DF7D1 -> 803DF80C
    MGeneric/MAudioReflection/PlayerNum/Dist/Covered (asm)  803DF80C -> 803DF898
    escalera de versiones (asm + dos primer) + guardas      803DF898 -> 803DF93C

Relleno de compensacion: **96 B** al final del bloque de la r52.
**Siguiente**: 0x803DF93C, donde el objetivo quiere `SMS_MESSAGE_%d`; delante se cuelan
`GManager` y `VehicleParams`.

### `zLua` — la que mas contenido gano

Estaba en `rodata−728` y **le faltan 55 cadenas (633 B)**. Cuatro ediciones la dejan en −480,
o sea **248 B de contenido nuevo y correcto**, y +393 B de prefijo. **Siguiente**: 0x803ED5B5;
el objetivo quiere `Attrib::Gen::simsurface` y `16.1.0`, y delante nos sobra
`Attrib::Gen::pvehicle`, que **hay que retrasar** — y un primer solo mueve hacia atras.

### `zSpeech` — cerrada la cabeza, abierta la cintura

Sigue `IGUAL` en las nueve secciones. Lo que ahora bloquea esta en **0x804062E4**: entre
`SMS_MESSAGE_%d_SUBJECT` y `AUD:GameSpeech` se cuelan **160 B** (el cadaver `"bosfx"` de
`Attrib::Gen::turbosfx` mas `MGamePlayMoment/Position/Vector/Velocity/hSimable/AttribKey/
MPursuitBreaker/StartBreaker/MUnspawnCop/CopHandle/ISimable/Param`) que el objetivo tiene mucho
mas adelante. **Un tercer primer con las ocho de detras NO las mueve — MEDIDO Y NEGATIVO**, y
esta anotado en el fuente: la primera diferencia no se movio y `.rodata` empeoro de 7.572 a
7.581. Hay que sacar esas doce por otra via.

Y una medida que sirve para todo el arbol: la comparacion de MULTICONJUNTOS de cadenas (no de
conjuntos) delato que el bloque de la r52 de zSpeech emitia **cuatro DUPLICADOS** (`16.1.0`,
`AUD:Relocated speech headers`, `AUD: SED_NISSFX events`, `SoundAI CarCustomization`) y le
faltaban **dos cadenas de verdad** (`Attrib::Gen::aud_moment_strm`, `Speech::Cache::Alloc`).
`prefijotu.py` decia «nos faltan 0». **`prefijotu` no ve duplicados.**

### `zAI` — el negativo de la r59 ROTO, aunque por poco

La r59 dejo escrito que el primer de la cabeza «NO PAGA» (total 40.284 → 41.108-41.136).
**Con las cadenas muertas fuera (`HAND_POOL_TAG` + `DEAD_STR_DONE`), el mismo primer pasa a
pagar**: 40.284 → **40.215**, y el prefijo exacto +200 B. Sigue costando 16 B de `.rodata`
(−256 → −272), que es el precio conocido de quitarle a `keep.lst` la copia que salvaba.

Es un caso de manual de `nfsmw-vedas-caducan`: **el negativo era correcto y la conclusion no**;
lo que fallaba era el orden en que se probaron las dos cosas.

### `zFe2` — bloqueada, y ahora con la razon exacta

Ver §1.2. Su bloqueo NO es el bloque de la r58: es que **no se le puede quitar
`_AttribAllocTagOrder` sin `mangfix.py`**.

---

## 5. Como se mide esto (y por que `linkdelta`/`dolwhere` no valen tal cual)

Igual que la r59, con las cuatro sondas que aquel informe pidio subir al arbol y siguen sin
subir (`lcpriv.py`, `link2.py`, `rangos.py`, `parasitos.py`). Anado dos mias, en mi
scratchpad, `cmpstr.py` (cadena a cadena, objetivo contra enlace, desde una direccion) y
`multiset.py` (el §4 de zSpeech: cuenta COPIAS, no cadenas distintas).

El control de siempre: **el enlace base con la copia privada de `keep.lst` es identico al ELF
original en los rangos de la unidad**. Comprobado antes de creer ninguna cifra.

---

## 6. `lcfix`: **311 pendientes, las 311 mias, y DOS que hay que BORRAR**

    zFe 142   zLua 84   zSpeech 43   zAI 42

Antes de empezar, `lcfix --check` daba **0 correcciones** (la ventana de la r59 lo aplico), asi
que **las 311 son mias y ninguna es de otro agente**.

**Ninguna correccion romperia mis unidades**: todas mis medidas son ya el estado POST-`lcfix`.
Pero hay **dos entradas que `lcfix` no puede arreglar y hay que BORRAR**, porque nombran una
cadena que sus unidades ya no emiten:

    config/GOWE69/keep.lst:3063   # @lc zLua "done"     <- FALLO duro: «la cadena no esta en su .rodata»
    config/GOWE69/keep.lst:2374   # @lc zFe  "done"

Y cuatro cosmeticas mas de la familia que la r58/r59 ya documento (`'GAMECUBE' no tiene simbolo
`$LC` propio`, lineas 1332 zLua, 1666 zSpeech, 1957 zAI, 2368 zFe): la entrada se queda
apuntando al `$LC` vivo de su vecina. **Borrarlas es limpieza, no arreglo.**

---

## 7. Un aviso de coordinacion que me toco cobrar a mi

**`build_direct.py zFe` compila TAMBIEN `zFe2` y `zFeOverlay`**: el filtro es por SUBCADENA, no
por nombre exacto. Mi pasada de sello (`build_direct.py zFe zFe2 zAI zSpeech zLua`) dijo
**«6 ok»** para cinco unidades, y la sexta era `zFeOverlay`, **que es de otro agente**.

No he tocado su fuente ni su `.o` a mano, y **no contamina mis medidas** (el enlace base usa el
objeto EXTRAIDO de zFeOverlay, no el nuestro), pero **si le he reescrito su `.o` en
`build/GOWE69/src`**, que es justo lo que la regla 5 prohibe. Se nota en `lcfix --check`: han
aparecido **28 FALLO de `zFeOverlay`** que no existian al empezar la ronda, todos de la familia
`GAMECUBE`/`Attrib::*` — o sea que el agente `feov` esta haciendo exactamente el mismo trabajo
del §1 y su fuente ya no coincide con el `.o` que el habia dejado.

**Y dar la ruta completa NO basta**: `build_direct.py Speed/Indep/SourceLists/zFe` sigue
diciendo «6 ok». El filtro compara subcadenas de la ruta, asi que `zFe` casa con `zFe2` y con
`zFeOverlay` de las dos maneras. **Que se arregle comparando el nombre de unidad EXACTO.**

(En una de mis tres pasadas de sello salio «5 ok, 1 fallidas»: la que fallo era `zFeOverlay`,
compilandose mientras su dueno editaba el fuente. **Mis cinco `sha1` salieron identicos en las
tres pasadas de todos modos**, que es justo lo que el aviso 1 del encargo pide comprobar.)

---

## 8. Lo que pido, con la cifra

| propuesta | vale | quien |
|---|---|---|
| **`python scripts/lcfix.py`** — 311 correcciones, todas mias. Sin esto `linkdelta` y `dolwhere` mienten sobre mis cuatro unidades | obligatorio | ventana |
| **borrar `keep.lst:3063` y `:2374`** (`@lc zLua/zFe "done"`) — `lcfix` NO puede arreglarlas | evita un literal muerto salvado en el sitio equivocado | ventana |
| **auditar los bloques `asm` VERBATIM de cabecera de TODAS las unidades**: si los bytes pegados son literales que cc1plus emite igual, estan duplicando el pool. Sospechosos por el mismo patron: `zFeOverlay`, `zCamera`, `zTrack`, `zWorld`, `zPhysicsBehaviors` | en `zFe` valieron **−680 B y +840 de prefijo** | los duenos |
| **partir los bloques `.asciz` de cola de la r52/r53 y colocar cada cadena en su direccion** con `.balign 4` | seis colocaciones medidas esta ronda, todas positivas | los duenos |
| **`mangfix.py` sobre `zFe2`** y luego `ATTRIB_TAG_ORDER_HAND_POOL` | desbloquea la unidad de 250 kB que hoy no se puede tocar | ventana |
| arreglar el filtro por subcadena de `build_direct.py` (§7) | evita que un agente recompile la unidad de otro sin enterarse | ventana |
| subir al arbol `lcpriv/link2/rangos/parasitos` (la r59 ya lo pidio) **+ `cmpstr` y `multiset`** | son las que hacen medible una unidad con `keep.lst` sucio | ventana |

---

## 9. Sorpresas

1. **Un bloque VERBATIM del objetivo puede ser un ERROR.** Si los bytes son literales que
   cc1plus emite igual, no son cabecera: son el arranque del pool, y lo duplicas. En `zFe` eran
   48 B y corrian 60 B de pool; en `zFe2` son 864 B y ahi **la misma cura es negativa**. La
   receta no es «pegar bytes», es «pegar bytes QUE EL COMPILADOR NO VAYA A EMITIR».
2. **El deficit de una seccion no esta donde lo tapas.** La r58 cuadro `zFe` con 48 B en la
   CABEZA; los 64 B que de verdad faltan van al FINAL. El tamano sale igual y el contenido no.
3. **Compensa ANTES de leer nada.** Un `rodata−32` mueve `.data` y `.bss` enteras por la
   ventana de 32 y fabrica 6.361 B de diferencias que no son tuyas. Dos veces me convencio de
   que una edicion buena era un desastre.
4. **La tercera via para colocar una cadena**: ni primer ni `#include`, un `asm()` de fichero.
   Es lo unico que sirve cuando la cadena esta MUERTA — y **`.balign 4` no es opcional**: sin
   el, la primera diferencia RETROCEDE.
5. **`prefijotu.py` no ve duplicados.** Decia «a zSpeech no le falta ninguna cadena» y le
   faltaban dos mientras emitia cuatro por duplicado. La comparacion buena es de MULTICONJUNTO.
6. **Una etiqueta de asignador se puede apagar UNA A UNA sin tocar cabeceras ajenas**, con
   `#undef ATTRIB_TAG` alrededor de los `#include` de las clases. Y **adelantar ese parseo
   cuesta exactamente cero** (medido al byte con el control de la §3.1).
7. **Un negativo de la ronda anterior puede caer sin herramienta nueva, solo cambiando el
   ORDEN de las pruebas.** El primer de `zAI` era negativo con los cadaveres puestos y positivo
   sin ellos.
8. **`build_direct.py` filtra por SUBCADENA.** «zFe» son tres unidades, y una es de otro
   agente.
