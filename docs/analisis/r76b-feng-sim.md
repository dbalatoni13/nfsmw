# r76b — zFEng y zSim sin andamios

Lote: los 5 andamios REALES (2 PIN + 3 BARRERA) de `FEngine.cpp` y `QuickGame.cpp`.
**Los cinco estan retirados.** `censoasm.py --clase PIN` y `--clase BARRERA` ya no
listan ninguno de mis dos ficheros: 2 PIN -> 0 y 3 BARRERA -> 0.
(El total del censo en el arbol baja mas porque hay otros agentes de la r76b
trabajando en paralelo sobre los otros 8 andamios; esa cifra no es mia.)

Medida con `build_direct.py` por unidad y `fndiff.py` por funcion, con
`ppc.calculatePoolRelocations=false`. `.rodata`, `.data` y `.bss` de zFEng son
byte a byte los del objetivo antes y despues: no se ha movido ni un literal.

## Tabla

| funcion | andamio retirado | oraculo (DWARF) | % antes | % despues | nuestro/objetivo | formas descartadas | cruce `.line` |
|---|---|---|---|---|---|---|---|
| `FEngine::ProcessPadsForPackage` | `asm volatile("" : : "r"(Held), "r"(JoyMask))` :479 **y** `register u32 padMask asm("r30")` :618 | `Pressed` r18, `Released` r15, `Held` r19, `Mask` r27, `JoyMask` r20, `PadIndex` r7/r25, `i` r26; el bloque 0x80186954-0x80186A40 solo declara `PadIndex` — **`padMask` no existe** | 100 % | **99,909 %** (14 filas) | 3080 / 3080 (**0 B**) | `Pressed = Released = Held = 0` y las otras 3 cadenas con `Held` antes que `Released` (33 filas); `if (WasReleased(Mask)) {}` con cuerpo vacio (identico byte a byte) | **REPRODUCE** la region tocada nota por nota (idx 66-81 y 453-512); 757/770 indices con nota coinciden |
| `FEngine::Update` | `asm("" : "+r"(iIterationTicks), "+r"(iTicksRemaining))` :341 | `iTicksRemaining` r28, `iIterationTicks` **SIN registro**; bloques [0x80186118,0x801861C0) y [0x80186128,0x801861B4) | 100 % | **98,247 %** (9 filas) | `int iIterationTicks = 0;` con la asignacion posterior intacta (dos sets contra cprop): **EMPEORA** a 97,72 %, 768 B, 10 filas | 51/51 hasta el corte salvo el cluster 745-748 (ver abajo) |
| `QuickGame::CreateCars` | `register const UMath::Vector3 *collisionPos asm("r4")` :304 **y** su `asm("" : "+r"(collisionPos))` :308 | `initialPos` r1+8, `initialVec` r1+0x18, `initialVelocity` (fuera), `height` r1+0x548, `Heat` f29, `player_1` r1+0x28, `player_2` r1+0x388, `icar` r29, `ip` (fuera) — **`collisionPos` no existe** | 100 % | **99,748 %** (4 filas) | — | **51/51 indices** del prefijo, una sola diferencia y es de numeracion de cabecera |

## Veredicto por unidad

**zFEng: YA NO REPRODUCE. Hay que degradarla a NonMatching.**
`Update` sale con 764 B contra 776 (faltan 3 instrucciones) y
`ProcessPadsForPackage` tiene 14 instrucciones con registro distinto a igual
tamano. El resto de la unidad no se ha tocado.

**zSim: YA NO REPRODUCE. Hay que degradarla a NonMatching.**
`CreateCars` sale con 1912 B contra 1908: una instruccion `mr r3,r11` de mas.

## Lo que se ha ganado (codigo real, no porcentaje)

### 1. La sentencia que faltaba valia 21 filas — y NO era ninguna barrera

El mayor hallazgo. La barrera de `:479` decia estar sujetando el reparto
r18/r19 de `Pressed`/`Held`. Era falso: lo que faltaba era una **sentencia**.

El mapa de lineas del original pone **tres `li rN,0` en UNA sola linea** (la 879
emite nota en los indices 74 y 79; el tercer cero, idx 80, no tiene nota propia
porque es continuacion de la misma sentencia), y esa linea va **detras** de
`Mask` y `JoyMask`. Nuestra fuente tenia `Pressed = 0;` suelto y ADELANTE, y
`Held = 0; Released = 0;` detras — tres sentencias donde el original tiene una.

    // ANTES                                  // AHORA
    Pressed = 0;                              Mask = 1 << i;
    Mask = 1 << i;                            JoyMask = pPackage->GetControlMask();
    JoyMask = pPackage->GetControlMask();     Pressed = Held = Released = 0;
    Held = 0;
    Released = 0;

Con la forma nueva el cruce `.line` casa **nota por nota** con el original
(idx 66, 70, 71, 73, 74, 75, 76, 77, 78, 79, 81), y la permutacion r18/r19
desaparece sola: de 33 filas a 14, **sin un solo `asm`**.

El orden de la cadena SI importa y esta medido: lo unico que decide es que
`Released` se evalue antes que `Held`.

| cadena | orden de evaluacion | filas |
|---|---|---|
| `Pressed = Held = Released = 0` | R, H, P | **14** |
| `Held = Pressed = Released = 0` | R, P, H | **14** |
| `Held = Released = Pressed = 0` | P, R, H | **14** |
| `Pressed = Released = Held = 0` | H, R, P | 33 |
| `Released = Pressed = Held = 0` | H, P, R | 33 |
| `Released = Held = Pressed = 0` | P, H, R | 33 |

Las tres buenas dan el MISMO objeto. Elegida `Pressed = Held = Released = 0;`.

### 2. El pin r30 costaba 0 bytes

`padMask` no existe en el DWARF. Con `(JoyMask & (1 << PadIndex))` en linea, el
mapa de lineas pasa a casar (el original tiene slw+and.+beq en UNA sentencia,
la 1071; nuestro sitio del pin tenia nota propia y el original no). Lo que
queda es una **permutacion r30<->r31, 12 filas, 3080/3080 B**: cero bytes de
diferencia, cero instrucciones de mas o de menos. Es exactamente el caso que
la consigna del usuario pide aceptar.

Confirmada de paso la afirmacion de r67 que estaba sin medir: poner
`if (WasReleased(Mask)) {}` con cuerpo vacio da **el mismo `.o` byte a byte**
que la llamada suelta (jump1 y cse lo borran antes de que cuente).

### 3. La estructura de bloques de `FEngine::Update`, corregida

El DWARF da el ambito de `iIterationTicks` como [0x80186128, 0x801861B4): un
bloque **interno** que acaba ANTES de `bRenderedRecently = false` (:797) y de la
condicion (:798). Y la 798 emite `mr.` + `bne` + `b` de una sola sentencia, que
es la firma de un `do/while`, no de un `for(;;)` con `break`. Reescrito asi:

    do {
        {
            int iIterationTicks;
            ...
            iTicksRemaining = iIterationTicks;
        }
        bRenderedRecently = false;
    } while (iTicksRemaining != 0);

No mueve un byte (mismo resultado que la forma anterior), pero es la estructura
del original.

## Lo que queda, con el dedo puesto encima

### `FEngine::Update` — las cuatro sentencias de 745-748

Es lo unico que bloquea la unidad, y el cruce `.line` lo localiza al indice
exacto. En el **indice 124** (la instruccion `lwz r0,0x5260(r29)`, la carga de
`bRenderedRecently`) el original acumula OCHO notas de cero bytes:

    745, 746, 747, FEKeyTrack.h:51, 748, FEKeyTrack.h:51, 748, 753

y nosotros tenemos una sola (`if (bRenderedRecently)`). Las dos expansiones de
`FEFramesToTicks` del original estan **antes** del `if`, con cero bytes; las
nuestras caen dentro de las ramas (indices 127 y 129). O sea: hay cuatro
sentencias en 745-748 que no emiten codigo y que expanden `FEFramesToTicks` dos
veces, una por sentencia (la secuencia `747, KT, 748, KT, 748` dice que 747 y
748 son sentencias distintas con una expansion cada una).

Restricciones duras para quien lo retome:

* **No pueden ser declaraciones.** El bloque del DWARF lista exactamente una
  variable, `int iIterationTicks`, y el volcado SI lista las locales sin
  localizacion (`initialVelocity` y `ip` de `CreateCars` lo estan). Cualquier
  `u32 uDirtyOn = ...` extra aparaceria.
* **La hipotesis "dos sets contra cprop" de r67b queda REFUTADA**, medida:
  `int iIterationTicks = 0;` conservando la asignacion de :760 da 97,72 %,
  768 B y 10 filas — GCC iza el inicializador fuera del bucle y sigue plegando
  la salida.
* Lo que falta produce exactamente `li r27,0` (:760), `mr. r28,r27` y su `bne`
  (:798): 3 instrucciones, 12 B.

### `QuickGame::CreateCars` — una ranura de sched1

Las 4 filas son una sola instruccion de mas. El objetivo calcula la direccion
del temporal `WCollisionMgr` ya en r3 (`addi r3,r1,0x540` ... `stw r0,4(r3)`);
nosotros la dejamos en r11 y hace falta `mr r3,r11`. Mecanismo ya diagnosticado
en r67 con el volcado `.lreg`: sched1 adelanta la copia a r3 por delante del
segundo store del constructor y, como el pseudo sigue vivo, local-alloc no
puede fundir la copia. Es orden dentro de `expand_call`, no reparto.

Comprobado en r76b que el constructor de `WCollisionMgr` ya tiene la forma
buena (cuerpo con dos asignaciones, no lista de inicializacion): sus notas son
`.h:85/86/87` en el original y `.h:49/50/51` aqui, el mismo patron relativo.

## CORRECCION a la nota del lote

El aviso de que `SkipFEPlayerPerformance`, `SkipFEPlayerCar` y `SkipFE` serian
locales del original «sin localizacion» de `CreateCars` es **FALSO**. No
aparecen en el DWARF de la funcion; son externs de fichero declarados en
`QuickGame.cpp:114`. No hay ninguna local que escribir por ahi.

## Nota de metodo: el desplazamiento del `.line`

`rdline.py` toma los offsets del `.line` tal cual, y NO coinciden con el valor
que da `nm` para el simbolo: en `ProcessPadsForPackage` van **3 instrucciones
(12 B) por delante**, en `Update` y `CreateCars` van a cero. Alinear a ojo lleva
a conclusiones falsas. `scratchpad/r76b_feng_sim/cruce.py` busca el
desplazamiento que mas indices hace casar y luego lista los que difieren; es lo
que destapo la sentencia de los tres ceros.

Ojo tambien al leer el cruce de una funcion que ya no casa en tamano: a partir
de la instruccion que falta (o sobra) TODOS los indices se desplazan y el
informe se llena de ruido. Hay que acotarlo al prefijo anterior al corte.
