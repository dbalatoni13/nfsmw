# r36e --- EAGL4Anim y Platform

Territorio: `src/Speed/Indep/Src/EAGL4Anim/**` y lo que compila
`Speed/Indep/SourceLists/zPlatform`.

**Cerrado: nada.** Pero la cola era mas corta de lo que decia el brief y las
tres formas de atacarla estan ahora acotadas con medida. Todo revertido, arbol
limpio.

## Verificacion

    python scripts/build_direct.py zEagl4Anim zPlatform    -> 2 ok, 0 fallidas
    pctsnap --cmp antes/despues                            -> EMPEORAN: ninguna
    python scripts/lcfix.py --check                        -> todas al dia
    git status en mi territorio                            -> vacio

Estado final, identico al de partida (las cinco funciones al mismo % y tamano):

| B (obj) | % | nuestro | funcion | unidad |
|---|---|---|---|---|
| 2352 | 98,8758 | 2356 | `Initialize__Q25EAGL413DynamicLoader...` | zEagl4Anim |
| 1588 | 99,2443 | 1580 | `ActualReadJoystickData__Fv` | zPlatform |
| 1480 | 99,4595 | 1488 | `EvalSQT__...FnStatelessQ...` | zEagl4Anim |
| 720 | 97,3611 | 720 | `FindMatchTime__...FnRunBlender...` | zEagl4Anim |
| 456 | 93,5088 | 456 | `EvalState__...FnRawStateChan...` | zEagl4Anim |

## 1. La cola del brief tenia 6 funciones; son 5

**`DVDErrorTask__FPvi` (1.904 B) YA ESTA AL 100 %.** No habia nada que hacer, y
era la primera de la cola. Lo dicen a la vez `objdiff` (`target=100.0%
ours=100.0%`) y el `report.json` fresco (que para zPlatform solo lista
`ActualReadJoystickData`).

Es un **falso positivo nuevo de `fncmp.py`**, y conviene apuntarlo porque la
rama que lo produce no imprime nada ni con el argumento de detalle:

    +170 RELOC obj 3E408042 None | nue 3E400000 ('g_discErrorOccured', 6, 0)

En `+0x170` el objeto extraido lleva `lis r18,0x8042` **sin reubicacion** y
nosotros `lis r18, g_discErrorOccured@ha`. `g_discErrorOccured = 0x8041E5A0`
(`config/GOWE69/symbols.txt`), cuyo `@ha` es 0x8042: **los bytes enlazados son
identicos**. dtk simplemente no emparejo ese `lis` (si emparejo los otros dos de
la misma funcion, en `+0x360` y `+0x43C`). `fncmp` cuenta como diferencia
cualquier instruccion donde un lado tiene reubicacion y el otro no --y esa rama
es la unica de las cinco que **no** tiene `print`, por eso el detalle salia vacio
y parecia un fallo de la herramienta--.

Segundo falso positivo, mismo dia: `Insert__Q25EAGL410SymbolPoolPCcPv` (452 B).
Es una funcion **recursiva**; el `bl` a si misma lo resuelve `destino()` como
`('.', 0)` en el objeto extraido y como `('Insert__...', 0)` en el nuestro.
`objdiff` la da al 100 %.

**Propuesta fuera de territorio** (`scripts/fncmp.py`, herramienta compartida):

1. anadir el `print` que falta en la rama `(pa is None) != (pb is None)`;
2. antes de contar esa rama como diferencia, resolver el inmediato: si el lado
   sin reubicacion tiene el valor que produciria `@ha`/`@l`/`@sda21` del simbolo
   del otro lado, es la misma instruccion;
3. normalizar el simbolo de seccion `.` al de la propia funcion cuando el
   desplazamiento cae dentro de ella (recursion).

Sin (2) y (3) la cola de zPlatform se lee como 3.492 B cuando son **1.588**.

**Aviso de metodo**: los dos falsos positivos aparecen y desaparecen segun quien
construyo el `.o` (ninja o `build_direct.py`) porque cambia la forma de la
reubicacion, no el codigo. Antes de creerse una fila de `fncmp`, contrastarla con
`fndiff`/`pctsnap`, que es lo que hice para fijar la linea base de arriba.

## 2. `Initialize` (2.352 B, sobran 4 B): el arbol del `switch`

Los 4 B son **una sola instruccion**: un `b` de mas al final del arbol de
decision del `switch (sheader->sh_type)`. El resto de las diferencias son
permutaciones de registro del mismo tamano (`add r30,r11,r10` contra
`add r30,r10,r11`, `lwzx r4,r11,r29` contra `lwzx r4,r29,r11`, r29<->r30).

Los dos arboles, decodificados instruccion a instruccion (direcciones reales del
objeto extraido, base 0x48E8):

```
OBJETIVO                              NUESTRO
cmpwi  r0,8   ; beq  FIN              cmpwi  r0,8   ; beq  FIN
cmplwi r0,8   ; ble  L_BAJO   <---    cmplwi r0,8   ; bgt  L_ALTO   <---
cmplw  r0,R6  ; bgt  FIN              cmpwi  r0,3   ; beq  STRTAB
cmplw  r0,R5  ; bge  FIN              cmplwi r0,3   ; bgt  FIN
cmpwi  r0,9   ; beq  REL              cmpwi  r0,2   ; beq  SYMTAB
b FIN                                 b FIN
L_BAJO:                               L_ALTO:
cmpwi  r0,3   ; beq  STRTAB           cmplw  r0,R6  ; bgt  FIN
cmplwi r0,3   ; bgt  FIN              cmplw  r0,R5  ; bge  FIN
cmplwi r0,1   ; ble  FIN              cmpwi  r0,9   ; beq  REL
(cae) SYMTAB                          b FIN          <-- LOS 4 B
                                      (cae) SYMTAB
```

El objetivo emite el grupo ALTO en linea y manda el BAJO fuera; nosotros al
reves. Y su ultima hoja **cae** en el cuerpo de `SHT_SYMTAB` en vez de saltar,
que es lo que le ahorra el `b`.

### Lo que emite cada forma de nodo, medido con `ngccc -S`

Reconstrui `emit_case_nodes` de GCC 2.9 a base de repros de un segundo. Vale
para cualquier `switch` del proyecto:

| construccion | codigo emitido |
|---|---|
| `node_is_bounded(node->left)` --> `LT node->high` | `cmplwi C ; blt` (**no** se canonicaliza a `ble C-1`) |
| nodo rango, test de entrada --> `GE node->low` | `cmplwi C-1 ; bgt` (si se canonicaliza) |
| nodo rango con hijo derecho y sin izquierdo --> `LE node->high` | `cmplwi C ; ble` |
| hoja de valor unico | `do_jump_if_equal` + `b default`, **sin** tests de limite |
| hijo derecho acotado --> `GT node->high` | `cmplwi C ; bgt` |

Con esa tabla, las dos instrucciones raras del objetivo se leen solas:

- `cmplwi r0,8 ; ble L_BAJO` es un **LE**, no un LT (`bc 4,1` en crudo). Solo lo
  emite un **nodo rango con hijo derecho y sin izquierdo** cuyo `node->high`
  es 8.
- `cmplwi r0,1 ; ble FIN` + caida es la misma forma con `node->high == 1`: un
  rango `[0,1]` cuyo cuerpo es el `break`, seguido de un `[2,2]` **acotado**
  (`emit_jump` al cuerpo que jump.c borra por ser el siguiente).

Es decir: al original le sobra, respecto a nosotros, un `case SHT_NULL ...
SHT_PROGBITS:`. Pero **anadirlo no basta**, porque `balance_case_nodes` mueve
entonces la raiz a `[4,7]`.

### 622 variantes barridas, cero aciertos

- **599** listas de `case` distintas sobre un repro suelto (particiones de
  {0,1} y de {4..8}, nodos extra en 10, 10..11, 0x70000000..4, 1000, corte del
  rango alto en HIPROC/LOUSER, `default` delante, sin `default`). Ninguna produce
  `cmplwi 8 ble`.
- **23** variantes sobre la funcion **real** (que es el oraculo bueno): las
  medidas van de 97,38 % a 98,88 %, ninguna mejora, ninguna cambia el tamano a
  2.352.

### Regla nueva: GCC **funde** los `case` contiguos con la MISMA etiqueta

Comprobado comparando el **SHA de los bytes de `.text` de la funcion** (no del
`.o`: ver el aviso de abajo). Las seis variantes dan `.text` **identico byte a
byte**, 2.356 B, `sha=c0b08d51...`:

- base;
- partir `case SHT_RELA ... SHT_NOTE` en `4...5` + `6...7`;
- partir `case SHT_LOPROC+5 ... +6` en dos `case` sueltos;
- `case SHT_NOBITS ... SHT_NOBITS` contra `case SHT_NOBITS`;
- `default: break;` cambiado a `default: continue;`;
- quitar el `default:` entero.

**Aviso de metodo**: el SHA del `.o` completo **si** cambia en cuatro de las seis,
porque `-gdwarf+` mete la numeracion de lineas y anadir o quitar una linea de
fuente la desplaza. Comparar `.o` contra `.o` para decidir si un cambio de fuente
es neutro da falsos «ha cambiado»; hay que comparar los bytes de la funcion.

Lo ultimo tambien dice que aqui **`break` y `continue` son la misma etiqueta**
(el `switch` es la ultima sentencia del cuerpo del `for`), asi que un `case` con
`continue` pegado a otro con `break` **se funden** --y por eso
`case SHT_RELA ... SHT_NOBITS: continue;` encoge la funcion a 2.304 B--.

Esto **matiza** [[nfsmw-arbol-de-switch]]: alli se dice que un `case` vacio
mueve el pivote aunque no emita codigo. Cierto solo si su **etiqueta difiere**
de la de sus vecinos. Si es contigua y comparte etiqueta, `add_case_node` la
funde antes de `balance_case_nodes` y el arbol no se entera. Para mover el
pivote hace falta un `case` **no contiguo** o con cuerpo propio.

**Veda**: el arbol de `Initialize` no sale de editar la lista de `case`. Si
alguien vuelve, que ataque el conjunto efectivo de nodos por otro lado (el tipo
del indice, o codigo entre el `switch` y el fin del bucle que separe `break` de
`continue`), no por anadir `case`.

## 3. `EvalSQT` (1.480 B, sobran 8 B): dos `clrlwi` que no se van

Los 8 B son dos `clrlwi r0, r0, 16` detras de `subi r0, r5, 0x1`, en las dos
copias de

```c
if (floorKey >= static_cast<unsigned short>(statelessQ->mNumKeys - 1))
```

El objetivo hace `subi r0, r5, 0x1 ; cmpw r30, r0` **sin extender**, con `r5`
copia cruda del `lhz`. Nosotros extendemos siempre: con el cast, detras de la
resta; sin el cast, delante. **El resto de la funcion casa entera**: son las dos
unicas diferencias.

**19 formas medidas, todas neutras o peores** (la ronda anterior ya habia hecho
siete; estas son las mias, con cifra):

| forma | % | tamano |
|---|---|---|
| base (con el cast) | 99,4595 | 1488 |
| sin `static_cast` | 98,0513 | 1488 |
| `static_cast<int>(mNumKeys) - 1` | 98,0513 | 1488 |
| `static_cast<int>(mNumKeys - 1)` | 98,0513 | 1488 |
| `(int)(unsigned short)(mNumKeys - 1)` | 99,4595 | 1488 |
| `static_cast<short>(mNumKeys - 1)` | 99,4595 | 1488 |
| `floorKey > mNumKeys - 2` | 98,0324 | 1488 |
| `(unsigned short)floorKey >= mNumKeys - 1` | 97,3081 | 1496 |
| cast en los dos lados | 97,3216 | 1496 |
| local `unsigned short numKeys` | 98,0513 | 1488 |
| local `int numKeys = mNumKeys - 1` | 98,0513 | 1488 |
| `floorKey + 1 >= mNumKeys` | 97,3108 | 1492 |
| `!(floorKey < mNumKeys - 1)` | 98,0513 | 1488 |
| `slerpReqd = slerpReqd && floorKey < mNumKeys - 1` | 92,0135 | 1492 |
| `if (slerpReqd && floorKey >= mNumKeys - 1)` | 94,8784 | 1464 |

**La palanca 1 (barrera selectiva) tambien es negativa aqui.** Cuatro formas:

| forma | % | tamano |
|---|---|---|
| `int numKeys = mNumKeys; asm("" : "+r"(numKeys));` | 98,4162 | 1488 |
| `int nk1 = mNumKeys - 1; asm("" : "+r"(nk1));` | 97,8892 | 1488 |
| `unsigned short nk = mNumKeys; asm("" : "+r"(nk));` | 98,4162 | 1488 |
| barrera + cast | 97,4973 | 1496 |
| `volatile int numKeys` | 97,4784 | 1504 |

La barrera no sirve porque **la extension la emite la propia asignacion**
`int x = mNumKeys`, no la comparacion: el `asm` llega tarde. El tamano se queda
en 1.488 en todas.

Diagnostico: el objetivo usa el resultado crudo del `lhz` como si fuera SImode
(su `combine` borro el `zero_extend` por `nonzero_bits`), y el nuestro no lo
borra en ninguna de las 19 formas. La palanca, si existe, no esta en esta
sentencia.

## 4. `FindMatchTime` (720 B, mismo tamano): la constante magica cambia de bloque

**No es un problema de tamano** (720/720). Las 12 instrucciones distintas son
todas consecuencia de UNA cosa: donde se carga la constante 0x4330000080000000
de la conversion `int -> float`.

```
                                  OBJETIVO            NUESTRO
bloque A  (antes del `bso` de     lis r9, K@ha        --
           input.mSearchLength    lfd f12, K@l(r9)    --
           > 0.0f)
bloque B  (la conversion de n)    --                  lis r9, K@ha
                                  --                  lfd f13, K@l(r9)
```

El objetivo la sube al bloque que se ejecuta siempre; nosotros la dejamos dentro
del `if`. De ahi salen en cascada los `lis r9/r10, 0x4330` y `f12/f13`
cambiados. Es el caso «hundimiento entre bloques» de la palanca 1, pero la
cantidad que se hunde es una **constante generada por el compilador**, sin
variable C a la que atar la barrera.

Seis formas de la condicion medidas:

| forma | % | tamano |
|---|---|---|
| base | 97,3611 | 720 |
| `(float)n` explicito | 97,3611 | 720 |
| dos `if` anidados | 97,3611 | 720 |
| `0.0f < mSearchLength && ...` | 97,3611 | 720 |
| `float fn = n;` antes del `if` | 90,4889 | 724 |
| `n > input.mSearchLength` | 97,2778 | 720 |
| `(double)` en los dos lados | 95,5833 | 716 |

Sacar la conversion a una local (`fn_local`) mueve **toda** la conversion al
bloque A, no solo la constante: por eso se desploma. Lo que hace falta es mover
**solo** el `lfd`, y eso no tiene mando desde la fuente. Candidata a permutador
o a un `asm` de ranura entre el `fcmpu` y el `cror`.

## 5. `EvalState` (456 B, mismo tamano): permutacion pura, y una pista de `regmap` que MIENTE

456/456 y 52 instrucciones distintas, todas del mismo tamano: reparto de
registros y planificacion. Los dos «INSERT» que marca objdiff no son codigo de
mas, son el alineador: nosotros adelantamos `lwz r10,0x10(r29)` a la instruccion
7 y el objetivo lo hace en la 19; igual con `subi r0,r3,1`.

`regmap` dice:

```
  fn   k        original r4   nuestro -      <-- DISTINTO
  fn   keyIdx   -             r10            <-- SOLO NUESTRA (int)
  VEREDICTO: NO es una permutacion: 1 local solo nuestra. ESO VA PRIMERO.
```

Esa `keyIdx` la anadio a proposito una ronda anterior (hay comentario en
`RawStateChan.cpp`). **Quitarla empeora**, y bastante:

| forma | % | tamano |
|---|---|---|
| base (con `int keyIdx = mKeyIdx;`) | 93,5088 | 456 |
| `mKeyIdx` directo en los tres usos | 89,8684 | 460 |
| reutilizar `i` como indice | 83,4123 | 452 |
| `const int keyIdx` | 93,5088 | 456 |

O sea: **el aviso «local de mas» del DWARF es aqui un falso positivo**. La local
existe en nuestro fuente porque sin ella GCC emite dos `lwz` y un `add`; el
original la resuelve sin local porque su `cse` la comparte, no porque su fuente
no la tenga. Anotado para no volver a caer: la regla de
[[nfsmw-local-de-mas-roba-registro]] no se aplica cuando la local sirve para
forzar UNA lectura de un miembro que se escribe dentro del mismo ambito.

## 6. `ActualReadJoystickData` (1.588 B, **faltan** 8 B)

El comentario que ya hay en `JoyE.cpp` (rondas 36 y 36b, cinco formas barridas)
sigue siendo correcto. Aporto dos datos nuevos:

**Que consume el valor que falta.** Las dos instrucciones ausentes son
`mr r9, r0` y `andi. r11, r9, 0x8000`, alrededor del `stb r0, 0x4(r29)` de
`stickX`. Lo importante es lo que viene detras, identico en los dos:

```
   mr r9, r11 / mr r10, r11 / mr r8, r11 / mr r6, r11 / mr r7, r11
```

Cinco copias del mismo registro. En el **objetivo** `r11` vale
`v & 0x8000` (lo acaba de calcular el `andi.`); en el **nuestro** vale
`lwz r11, 0x14(r1)`, o sea el entero crudo de la conversion, de mucho antes.
Las cinco copias existen en los dos, asi que no son codigo de mas: son copias de
union de un pseudo vivo. **La diferencia es que valor tiene ese pseudo**, y en
el objetivo es el `& 0x8000`. Eso solo pasa si `v` sigue VIVA despues del
`stb`.

**Intentos de mantenerla viva, medidos:**

| forma | % | tamano |
|---|---|---|
| base | 99,2443 | 1580 |
| `int v` subida al ambito del bucle `for (port...)` | 99,2443 | 1580 |
| ... y quitada la local interna que la tapa | 99,2443 | 1580 |
| `v = v & 0x8000;` en vez del `if` | 98,4761 | **1576** |
| ambas cosas | 98,4761 | 1576 |

Subir `v` al ambito del bucle no la mantiene viva porque en cada vuelta se
**escribe antes de leerse**: DCE se lleva el `& 0x8000` igual. Y cambiar el `if`
por la asignacion quita todavia una instruccion mas (1.576 B). La palanca es que
`v` se **lea** despues del bloque de `stickX`, cosa que nuestro fuente no hace
en ningun sitio.

Siguiente paso para quien vuelva: buscar donde lee el original esos cinco
pseudos --si son las variables del bloque `else` de `pad_state == 0` o de la
siguiente vuelta-- y escribir el fuente de forma que `v` llegue viva hasta ahi.
No hace falta tocar mas el bloque de `stickX`: esta agotado (10 formas entre las
dos rondas).

## Resumen de palancas del brief probadas en este territorio

| palanca | donde | resultado |
|---|---|---|
| 1 barrera selectiva `"+r"` | `EvalSQT` (4 formas) | negativo, tamano sin mover |
| 1 barrera, eje «a quien» | --- | no llegue a probarla |
| 2 cantidad fantasma | --- | no aplica: en `ActualRead` faltan 8 B **reales**, y la fantasma emite cero bytes |
| 3 pin de registro | --- | no aplica: ningun near-miss del territorio esta bloqueado por reparto puro salvo `EvalState`, y ahi manda antes la primera diferencia que no es de registro |
| 4 barrera de ranura | --- | candidata para `FindMatchTime` (entre el `fcmpu` y el `cror` del bloque A), sin probar |
| 5 `asm()` de fichero | --- | no aplica: no hay nada en el objetivo que no salga de C |

Ninguna de las cinco estaba probada aqui; ahora la 1 esta medida y descartada
para `EvalSQT`, y la 4 queda como la unica con pinta razonable (`FindMatchTime`).
