# r76b — zMain: los tres andamios fuera

Lote: los 3 andamios de `zMain` (unidad **Matching**).

| # | fichero:linea | andamio |
|---|---|---|
| 1 | `SteeringWheelDevice.cpp:317` | `register float originalVal asm("fr0")` |
| 2 | `SteeringWheelDevice.cpp:327` | `register float linearScale asm("fr13")` |
| 3 | `InputDeviceGC.cpp:528` | `__asm__("")` |

**Los tres estan retirados.** `censoasm.py --clase PIN` y `--clase BARRERA` ya no
devuelven nada para estos dos ficheros. No se ha anadido asm de ninguna clase.

## Tabla

| funcion | andamio | oraculo | % antes | % despues | nuestro/objetivo | formas descartadas | cruce `.line` |
|---|---|---|---|---|---|---|---|
| `ConvertWheelRotation__19SteeringWheelDevicei` | 2 pines FPR | `val` f0 y `originalVal` f0 existen; **`linearScale` NO existe** | **100,0 %** | **98,83721 %** (8 filas) | 172/172 | W1, W3, W5, W6, W8, R4, R6, R7 (abajo) | **W2 CASA; la forma con pines NO casaba** |
| `PollDevice__10GameDevice` | 1 barrera | sin locales ni inlines en el tramo; `di` r31, `value` r30 | **100,0 %** | **99,934715 %** (2 filas) | 1348/1348 | B, F3, F4, F5 (abajo) | con barrera habia nota espuria; sin ella no, pero las 2 filas siguen |

## 1-2. ConvertWheelRotation — los dos pines

El oraculo es tajante: el DWARF trae `val` (f0) y `originalVal` (f0), y
**`linearScale` no existe**. Era invencion nuestra; la forma real es el 0.8f en
linea. El pin de `originalVal` no inventaba la variable, pero sigue siendo
asignacion manual de registro, prohibida por el repo oficial y error duro en PS2
y Xbox 360.

### Lo que queda sin los pines: una rotacion de 3 FPR, 8 filas

    objetivo:  originalVal=f0   0.8f=f13  0.2f=f12
    nuestro:   originalVal=f12  0.8f=f0   0.2f=f13

`REG_ALLOC_ORDER` es `f0, f13, f12, ...`. `local-alloc` corre **antes** que
`global-alloc` y se lleva f0 y f13 para las dos constantes, que nacen y mueren en
el bloque de la cola. `originalVal` se escribe en los **dos** bloques del `Clamp`
(`VU0_floatmin` / `VU0_floatmax` tienen ramas), asi que es de `global-alloc` y se
queda con lo que sobra, f12. En el objetivo los tres se reparten f0/f13/f12 en el
orden `[originalVal, 0.8f, 0.2f]`, que es lo que sale si **un solo** asignador ve
los tres. La fuente no llega a `local-alloc` (`local-alloc.c`, `qty_compare_1`),
y por eso el andamio era un pin: no hay forma de C que lo mueva.

### Formas probadas, con cifra

| forma | % | filas |
|---|---|---|
| W1 return de la expresion directa | 98,83721 | 8 |
| **W2 asignar a `val` y devolverla — ELEGIDA** | **98,83721** | **8** |
| W3 el 0.2f delante de su producto | 94,18604 | 9 |
| W5 las dos constantes delante | 94,18604 | 9 |
| W6 el cubo en variable aparte | 89,76744 | 10 |
| W8 sin la local `val` (expresion directa) | 98,83721 | 8 |
| R4 `originalVal` declarada antes que `val` | 98,83721 | 8 |
| R6 las dos declaradas arriba, sin inicializador | 98,83721 | 8 |
| R7 `originalVal` reusado como acumulador | 98,83721 | 8 |

El orden de declaracion **no** mueve el reparto (R4/R6/R7 = W1). W8 empata pero
tira la local `val` que el DWARF si tiene, asi que no vale.

### El cruce `.line` es lo que decide, y descalifica la forma con pines

Rango del original `0x801F0FB8..0x801F1064`. Cola:

    ORIGINAL          +84 -> 659 (una sola nota)     +a0 -> 668  (el fmadds)
    con los 2 PINES   +84 -> 328, +88 -> 327, +8c -> 328, +94 -> 327, +98 -> 328
                      ...y NINGUNA nota en +a0
    W1 (sin pines)    +84 -> 282 (una nota)          ...y NINGUNA nota en +a0
    W2 (sin pines)    +84 -> 290 (una nota)          +a0 -> 291   <-- CASA

O sea: **la forma con pines daba 100,0 % de .text con un mapa de lineas falso**
(el pin de `linearScale` metia cuatro notas que el original no tiene). W1 tampoco
casa: devolviendo la expresion directa el `fmadds` no recibe nota propia. Solo
asignar a `val` y devolverla pone la suma final bajo la nota del `return`, como
el original. Entre las cinco formas que empatan a 8 filas, el `.line` deja **una**.

De paso fija el fuente original: la asignacion en la 659 y el `return val;` en la
**668** — nueve lineas mas abajo, con la funcion abierta en la 625. Ese hueco
(625-656 y 660-667) solo puede ser codigo condicional de otra plataforma o
comentario: no emite una sola instruccion.

## 3. PollDevice — la barrera

`barreramuerta.py` la da **VIVA** (`3837a3bb8d32 -> f90b3ee4961f`), confirmando
r65/r66/r67. Retirada igualmente: `__asm__("")` es error de compilacion en Xbox
360 (C3861).

Sin ella, `sched1` intercambia los dos `addi` del pie del bucle:

    objetivo:  addi r30,r30,0x4 (value++)  ;  addi r31,r31,0x14 (di++)
    nuestro:   addi r31,r31,0x14           ;  addi r30,r30,0x4

El bloque basico del pie es `[addi r30, addi r31, lwz r0,4(r31), cmpwi, bne]`.
`addi r31` encabeza la cadena que alimenta el test, asi que su `INSN_PRIORITY` es
3 y la de `addi r30` es 0: el nivel 1 de `rank_for_schedule` lo decide y no hay
empate que la fuente pueda romper.

| forma | % | filas | nota |
|---|---|---|---|
| **F1 los dos ++ sueltos, en orden fuente — ELEGIDA** | **99,934715** | **2** | |
| F3 el `di++` en la clausula de avance de un `for` | 99,934715 | 2 | ademas rompe el `.line`: en el original 796 y 797 son sentencias propias, no la linea del `for` |
| F4 preincremento en los dos | 99,934715 | 2 | |
| F5 suma explicita en los dos | 99,934715 | 2 | |
| B el bucle como `do ... while` | 98,272995 | 8 | **−12 B**: suprime el test de entrada que el original SI tiene |

**B refuta la hipotesis del `do-while`** que sugeria el mapa de lineas (el test
del bucle lleva la linea del `}`, la 798). Medido: GCC 2.9 le pone la linea del
`}` al test tambien en un `while` normal, asi que esa nota no discrimina, y el
original conserva el test de entrada. Es un `while`.

`NOTE_INSN_LOOP_CONT` **no** es barrera de sched: en `haifa-sched.c` solo lo son
`LOOP_BEG`, `LOOP_END`, `EH_REGION_BEG/END` y `SETJMP`. Por eso el `for` no gana
nada. Y el bucle vacio como barrera no se ha usado: aqui iria **dentro** de un
bucle, que es justo donde `nfsmw-bucle-vacio-barrera-sched2` lo tiene medido como
no valido, y ademas no es un constructo plausible en esta fuente.

### Cruce `.line`: la barrera tambien mentia

    ORIGINAL       +514 -> 796 (value++)   +518 -> 797 (di++)
    con BARRERA    +514 -> 507             +518 -> 529 (la linea del asm) y 530
    sin barrera    +514 -> 512 y 513       +518 -> 512

La barrera daba 100,0 % de `.text` **metiendo una nota de linea en +518 que el
original no tiene**. Sin ella no hay notas espurias, pero las dos filas siguen.
Ninguna de las dos reproduce el original: la diferencia es de planificador.

## Hallazgo suelto, sin resolver (no es un andamio)

En `PollDevice` el original emite **dos notas de linea que nosotros no**, en +474
y +4b0, exactamente donde el DWARF abre los bloques lexicos de `newval`
(`0x801EF78C`) y `range` (`0x801EF7C8`):

    ORIGINAL  +474 -> 779, 780      +4b0 -> 782, 783, 784
    NUESTRO   +474 -> 488           +4b0 -> 491, 492

Probadas y descartadas, las dos **sin mover el 100 %** y **sin emitir la nota**:

* declarar y asignar por separado (`float newval;` y luego la asignacion)
* partir la sentencia en dos lineas (GCC 2.9 usa la linea de **inicio**)

Queda abierto que emite esa nota. Afecta solo al `.line`, no al codigo.

## Estado de zMain

**Sigue reproduciendo su objeto y el enlace no se mueve**, comprobado enfrentando
el `.o` con andamios contra el `.o` sin ellos:

* **todas las secciones ALLOC, identicas en tamano** (`.text` 175.204 B incluido)
* **tabla de simbolos identica** (`nm`, diff vacio)
* solo cambian `.debug` (−44 B) y `.line` (−30 B), que no van al DOL
* `.o` sellado: `sha1 4e4cc2cf3e674d02`; `build_direct.py zMain` -> `1 ok, 0 fallidas`

**Pero ya no es byte-exacta.** Difieren **10 instrucciones** (8 + 2), todas
`ARG_MISMATCH` de numero de registro, sin un solo byte de mas ni de menos. Como
`matched_code` es todo-o-nada, las dos funciones dejan de contar enteras:

    1.520 B de 175.204 = 0,868 % de la unidad

**Hay que degradar zMain de Matching a NonMatching**, y eso lo decide el jefe. Lo
digo sin adorno: si el flag se queda como esta, el informe estara diciendo que la
unidad casa cuando no casa.

## Lo que se gana a cambio

Los tres andamios eran los tres no portables (`register ... asm("frN")` es error
duro en PS2 y Xbox 360; el asm vacio lo es en Xbox 360). Y dos de los tres
**estaban sosteniendo un 100 % que el mapa de lineas desmentia**: el pin de
`linearScale` inventaba una variable que el DWARF no tiene, y la barrera metia
una nota de linea propia. La forma que queda en `ConvertWheelRotation` es la
primera que **casa el `.line` del original**, cosa que la version al 100 % nunca
hizo.
