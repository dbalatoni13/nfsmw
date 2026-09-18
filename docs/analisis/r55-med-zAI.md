# r55 - medida de `zAI` (272.796 B de codigo)

Unidad: `Speed/Indep/SourceLists/zAI`. Solo se ha medido esta. Ningun fuente tocado.
Reconstruida con `python scripts/build_direct.py Speed/Indep/SourceLists/zAI` -> `1 ok, 0 fallidas`
a la primera.

## Resumen en una linea

**zAI no roba nada y casi no le queda contenido: el `.text` es ORDEN PURO y dos simbolos
(52 B y 104 B) explican 83.340 de los 126.180 B de `.text` descolocados. Lo unico que si es
contenido son 388 B de `.data` que no emitimos y 240 B de `.rodata` muertos.**

---

## 1. `reorden.py zAI` - orden contra contenido

```
seccion     simbs palabras   crudas  CONTENIDO  sin par
.ctors          0        0        0          0        0
.data          68      101       14          0       19
.rodata      1466     4362      933          1     1283
.text        1029    65854     4664          0        0

quedan 1 palabras de CONTENIDO real:
   .rodata     1 pal de   48 B   _vt.21AIGoalStaticRoadBlock
```

`.text` **0 de 65.854 palabras** y `.data` **0 de 101**: no queda una sola instruccion que
escribir. La unica palabra de `.rodata` **no es contenido**: es la ranura +0x0C de la vtable,
y apunta al destructor que se ha movido:

```
_vt.21AIGoalStaticRoadBlock   A 803CAF28   B 803CACE0
 +0C  A=8002CFC4 _._21AIGoalStaticRoadBlock   B=800457C4 _._21AIGoalStaticRoadBlock   <<<
```

La mascara de `reorden` deja **la mitad alta** de una forma D / un `ADDR32`, y el destructor
salta de `0x8002CFC4` a `0x800457C4`: cruza el limite de 64 kB, asi que la mitad alta cambia
(`0x8002` -> `0x8004`) y se cuela por la mascara. **Es orden, no contenido**, y ademas es el
mismo simbolo que domina el `.text`: las dos medidas apuntan al mismo sitio.

Aviso de metodo: la columna **`sin par` hay que leerla siempre**. `reorden` solo compara los
simbolos que emparejan por nombre, asi que un `CONTENIDO 0` puede convivir con un deficit
entero (aqui, 19 simbolos de `.data`, ver el punto 4).

## 2. `movidos.py Speed/Indep/SourceLists/zAI`

```
zAI: 38933 simbolos comunes (755 `$LC` fuera: su nombre colisiona entre objetos)
   17376 cambian de direccion, pero 16909 es ARRASTRE (tres terminos: donde
   empieza la seccion, cuanto mide nuestra aportacion, y cuanto bulto
   cambia de dueno).
   .bss: base-608 delta+64, .data: base-224 delta-388, .sbss: base-608 delta+0,
   .sbss2: base-608 delta+0, .sdata: base-608 delta+0, .sdata2: base-608 delta+0,
   .text: base+0 delta+5660
```

**467 simbolos DE VERDAD permutados**: `.text` 272 / 31.212 B, `.rodata` 143 / 15.080 B,
`.data` 49 / 404 B, `?` 3 / 0 B.

### CAMBIO DE DUENO (M4), literal

```
   CAMBIO DE DUENO (M4): esta unidad le quita bulto a OTROS objetos
      .bss       nuestro .o     +64, seccion enlazada      +0  ->      -64 de otro
      .data      nuestro .o    -388, seccion enlazada    -384  ->       +4 de otro
      .text      nuestro .o   +5660, seccion enlazada      +0  ->    -5660 de otro
```

```
   ESCALONES SIN EXPLICAR (9)
      .data           -616   2623 simbolos
      .text            +52    113 simbolos
      .text            -52     94 simbolos
      .text          +1532     68 simbolos
      .text          +1564     57 simbolos
      .rodata        -2648     38 simbolos
      .text           +264     33 simbolos
      .text           -232     26 simbolos
      .text          +3104     24 simbolos
```

### ...y por que ese -5660 NO es un cambio de dueno (medido)

`movidos` calcula M4 como `enlazado - delta`, y eso **no distingue "me lo han robado" de
"el enlazador me lo ha tirado"**. En zAI es lo segundo, y cuadra al byte.

Censo directo (leyendo las tablas de simbolos de los dos `.o` y de los dos ELF enlazados):
de los **120** simbolos globales/weak que nuestro `.o` define y `obj/zAI.o` no,

| que les pasa | n | bytes |
|---|---:|---:|
| `.text` sin referencia -> los estripa el enlazador | 109 | **4.820** |
| `.text` duplicados (los define tambien otro objeto) | 5 | **840** |
| `.rodata` estripado (`_vt.14AIGoalPullOver`) | 1 | 48 |
| `.rodata` nuevo y vivo (`lbl_803C8C60`, el prefijo de bWare) | 1 | 92 |
| `.data` estripado | 2 | 8 |
| `.bss` nuevo y vivo (`Tweak_ForceGPSArrowTo`) | 1 | 8 |

**4.820 + 840 = 5.660**, exactamente el `delta+5660` de `.text`.

Y los cinco duplicados **no cambian de dueno**: tienen **la misma direccion en los dos
enlaces**, o sea que el suministrador no cambia.

```
reserve__Q24_STLt6vector2ZP12EAX_CarState...   A=800C4D74  B=800C4D74   (zEAXSound.o, zSim.o)
reserve__Q24_STLt6vector2ZUi...                salto +0                 (zEAXSound.o, zSim.o)
find__H2ZQ24_STLt14_List_iterator2ZiZ...       A=800C4F40  B=800C4F40   (zEAXSound.o, zSim.o)
clear__Q24_STLt10_List_base2Z18SPCHType_1...   A=802AE948  B=802AE948   (zSim.o, zSpeech.o)
RaiseToPower__H1i10_i_i                        A=801429D8  B=801429D8   (zFe.o, zSim.o)
_14AISpawnManager.mSpawnSegment (.bss)         A=804F4040  B=804F3DE0   (-608 = base)
```

Ninguna de esas direcciones cae en la ventana de zAI (`.text` `800034A0..80045E3C`).

**CAMBIO DE DUENO REAL DE zAI: 0 bytes.**

## 3. El residuo del `.text`, simbolo a simbolo

Recorriendo la ventana del objetivo en orden y midiendo el desplazamiento de cada simbolo:

```
.text  ventana 272796 B  1029 simbolos  ->  EN SITIO 342 / 137.236 B   FUERA 687 / 126.180 B
   desplaz     +0   137.236 B     <- la primera mitad del .text esta PERFECTA
   desplaz    -52    49.204 B
   desplaz    +52    34.136 B
   desplaz   -232     6.576 B
   desplaz   +352     4.196 B
   desplaz   -408     3.244 B
   ... 145 transiciones mas, difusas
```

150 transiciones. Las tres primeras lo cuentan todo:

```
800034A0        +0   1732 B  __16AITrafficManagerGQ23Sim5Param            x323 simbolos
80024A6C       +52    128 B  __15AIVehicleCopCarRC14BehaviorParams        x113
8002CFC4   +100352    104 B  _._21AIGoalStaticRoadBlock                   x1
8002D02C       -52    140 B  __17AIGoalFleePursuitP8ISimable              x91
800388C0      +792    ...     (a partir de aqui, la cola de finish_file: churn)
```

### El simbolo dominante: `GetBits__C9WRoadLaneii` (+ su hermano)

```
80042A9C   -122928     24 B  GetBits__C9WRoadLaneii            x2 (con GetBitsSigned, 28 B)
```

**52 B que emitimos 122.928 B ANTES de su sitio.** `permorden` lo dice igual:
`obj#707 -> nue#323` y `obj#708 -> nue#324`, y son la cabeza de los dos ciclos mayores
(219 y 222 simbolos).

Los dos estan definidos **fuera de linea** al final de
`src/Speed/Indep/Src/AI/Common/AIVehicle.cpp:2369` y `:2373`, y GCC emite una funcion
no-inline **en su punto de definicion**: por eso salen justo antes de que empiece
`AIVehicleCopCar.cpp` (`0x80024A6C`). El objetivo los emite en `0x80042A9C`, **dentro de la
cola de `finish_file`** (entre `TypeName__13VehicleParams` y `_._19WRoadNavWithCookies`, los
dos artefactos de inline). Un cuerpo no-inline **no puede** caer ahi: en el original esas dos
funciones eran **inline**. La cabecera ya lo sospechaba:
`src/Speed/Indep/Src/World/WRoadElem.h:97` dice literalmente `// TODO inline`.

**Cuanto explica:** el `+52` que introducen arrastra **los dos** tramos --el de `+52` y el de
`-52`, que es `+52 - 104`--: **83.340 B de los 126.180 descolocados (66 %)**. Y son los
unicos que pagan solos: arreglar solo GetBits pone 34.136 B en su sitio; arreglar solo el
destructor no pone ninguno (el tramo `-52` pasaria a `+52`, igual de mal).

### El segundo: `_._21AIGoalStaticRoadBlock`, 104 B, +100.352 B tarde

El objetivo lo emite **fuera de linea, pegado a su constructor**:

```
8002CF68    92  __21AIGoalStaticRoadBlockP8ISimable
8002CFC4   104  _._21AIGoalStaticRoadBlock          <- aqui
8002D02C   140  __17AIGoalFleePursuitP8ISimable
```

Nosotros lo mandamos a `0x800457C4`, a la cola. Causa exacta, y esta a un editor:
`src/Speed/Indep/Src/AI/Common/AIGoal.cpp:305` lo define **dentro de la clase**
(`~AIGoalStaticRoadBlock() override {}`), o sea inline, o sea diferido.

Es el UNICO destructor de AIGoal* que el objetivo saca fuera de linea: `_._9AIGoalPit`,
`_._15AIGoalHeadOnRam`, `_._17AIGoalFleePursuit`, `_._10AIGoalNone`... viven todos en
`0x80044F60` y arriba, o sea en la cola. Esos hay que dejarlos como estan.

### Lo que queda despues de esos dos: difuso

`126.180 - 83.340 = 42.840 B` repartidos en ~145 transiciones, **todas dentro de la cola de
`finish_file`** (de `0x800388C0` al final). Eso es el orden de `pending_templates` /
`saved_inlines`, el mecanismo de `r54-forense1/2`. No hay un tercer simbolo dominante.

## 4. `.rodata`, `.data` y `.bss`

```
.rodata  ventana 26.344 B  1466 simbolos (1283 sin par: son los literales)  EN SITIO 0 B
.data    ventana    832 B    68 simbolos (19 sin par)                       EN SITIO 0 B
.bss     ventana  4.136 B   118 simbolos (38 sin par)   EN SITIO 80 / 3.976 B  FUERA 0
```

* **`.bss` esta perfecto** (todo a `+0` una vez descontada la base).
* **`.rodata`**: nuestro `.o` mide **26.344 B, exactamente lo mismo que el objetivo**, y aun
  asi el enlace sale a **-240**. O sea: emitimos 240 B de `.rodata` **muerta** (48 de ellos son
  `_vt.14AIGoalPullOver`, una vtable que el objetivo no tiene) y nos faltan 240 B de `.rodata`
  **viva**. Ningun simbolo con nombre esta en su sitio: el orden del bloque de literales y
  vtables esta entero por hacer.
* **`.data`: aqui SI hay contenido que falta.** Nuestro `.o` emite **444 B** contra los
  **832 B** del objetivo. Los 388 B que faltan son 14 bloques anonimos que el troceado no supo
  nombrar y que nuestro fuente no produce:

```
  -172  pad_06_80415180_data      -68  lbl_80415438        -12  lbl_80415348
   -44  gap_06_80415230_data      -40  gap_06_80415498     -16  gap_06_804153F8
    -8  lbl_804152DC    -8  lbl_8041541C    -8  gap_06_80415370
    -4  lbl_804152C0    -4  gap_06_80415260 -4  gap_06_804152A8
    -4  gap_06_804152C8 -4  gap_06_80415414
  (+8 nuestros de mas: _16AITrafficManager.mTrafficMaxSpawnDist y ...MinSpawnDist)
```

## 5. `linkdelta`, `permorden`, `trypromo`

```
linkdelta   Speed/Indep/SourceLists/zAI     .text +0    rodata-240 data-384
permorden   objetivo 1030 funciones, nuestro 1144, comunes 1030
            solo nuestras (las estripa el enlazador): 114
            en su sitio (subsecuencia creciente mas larga): 861 de 1030
            DESPLAZADAS: 169
            ciclos no triviales: 9  (longitudes: 222, 219, 61, 55, 35, 35, 22, 17, 7)
trypromo    Speed/Indep/SourceLists/zAI     DOL ROTO (416b87e30704)
```

Los dos ciclos mayores (222 y 219) los encabezan `GetBitsSigned__C9WRoadLaneii` (`obj#708 ->
nue#324`) y `GetBits__C9WRoadLaneii` (`obj#707 -> nue#323`): son **el mismo par**.

## 6. Siguiente paso, en orden de rentabilidad

1. **`src/Speed/Indep/Src/AI/Common/AIGoal.cpp:305`** - sacar el destructor de la clase:
   dejar `~AIGoalStaticRoadBlock() override;` dentro y poner
   `AIGoalStaticRoadBlock::~AIGoalStaticRoadBlock() {}` **justo detras del cuerpo del
   constructor** (despues de la linea 313), que es donde el objetivo lo emite
   (`0x8002CFC4`, entre `__21AIGoalStaticRoadBlockP8ISimable` en `0x8002CF68` y
   `__17AIGoalFleePursuitP8ISimable` en `0x8002D02C`). Cambio local, sin cabecera, y
   verificable con `permorden` y con la ranura +0x0C de `_vt.21AIGoalStaticRoadBlock`.
   **No tocar los otros destructores de AIGoal*: esos si van en la cola.**
2. **`WRoadLane::GetBits` / `GetBitsSigned`** - tienen que ser **inline** para caer en la cola
   de `finish_file` donde el objetivo los pone (`0x80042A9C`). El sitio faithful es el cuerpo
   dentro de la clase en `src/Speed/Indep/Src/World/WRoadElem.h:97-98` (donde ya hay un
   `// TODO inline`), pero **es cabecera compartida**: el ensayo barato y acotado a la unidad
   es anteponer `inline` a las dos definiciones de `AIVehicle.cpp:2369` y `:2373`. Vale
   83.340 B de `.text`, pero la posicion exacta dentro de la cola la decide el primer uso, asi
   que hay que medirla, no darla por hecha.
3. **Los 388 B de `.data`** son lo unico que es contenido de verdad. Empezar por
   `pad_06_80415180_data` (172 B, al principio de la ventana) y `lbl_80415438` (68 B).
4. **Quitar `_vt.14AIGoalPullOver`** (48 B de `.rodata` muerta) y buscar los otros ~192 B.

Coste estimado: los puntos 1 y 2 son **una tarde**; el 3 y el 4, una ronda.

## Sorpresas

1. **`movidos.py` da un falso positivo de M4 en zAI.** Dice `-5.660 de otro` en `.text`, y son
   **0**: 4.820 B que el enlazador estripa por no referenciados mas 840 B de definiciones
   duplicadas que **pierden** contra el objeto que ya las suministraba (misma direccion en los
   dos enlaces). `enlazado - delta` no distingue robo de estripado, y con
   `-strip-unused-data` el segundo caso es el normal (lo dice el propio docstring de
   `linkdelta`). **Antes de creerse un M4 hay que comprobar la direccion del simbolo en los dos
   ELF enlazados**; si no cambia, no hay robo.
2. **El censo del bloque A de `r54-control` da a zAI `dueno = 7` con
   `GetBits__C9WRoadLaneii -122.928`, y es un artefacto de su umbral.** Ese censo llamaba
   cambio de dueno a todo salto mayor de 20 kB, pero **la ventana de `.text` de zAI mide
   272.796 B**: una permutacion interna se salta los 20 kB sin despeinarse. `GetBits` es
   `obj#707 -> nue#323` **dentro de zAI**. El umbral tiene que ser relativo al tamano de la
   ventana, no absoluto. Conviene revisar con este criterio las otras cifras de `dueno` de esa
   tabla, sobre todo las de `zFe` (47) y `zFe2` (49).
3. **Un `CONTENIDO: 1` de `reorden` puede no ser contenido.** La mascara conserva la mitad
   alta de un `ADDR32`, asi que cualquier simbolo que se mueva mas de 64 kB delata su
   desplazamiento como si fuera contenido. Aqui la unica palabra "de contenido" era la ranura
   de vtable del destructor que ya sabiamos descolocado.
4. **Y al reves: `CONTENIDO 0` puede esconder un deficit.** `reorden` solo compara lo que
   empareja por nombre; los 19 `sin par` de `.data` son **388 B que no emitimos**.
