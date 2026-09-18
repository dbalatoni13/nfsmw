# r49 — zWorld / zWorld2 / zTrack

Territorio: 10.112 B en 8 funciones. **+0 B de `matched_code`** y **cero cambios
de codigo**: ninguna funcion baja de filas con un arbol defendible. Lo que deja
la ronda son **cuatro diagnosticos cerrados con la cifra** —tres de ellos
identifican por su nombre la funcion de GCC que decide— y **dos hipotesis
heredadas refutadas**.

Lo importante, en una linea cada uno:

- **`CullParts`**: las 5 filas son **UNA sola causa** —un `int debug_print = 0;`
  muerto que en el objetivo sobrevive porque CSE lo engancha— y las dos mitades
  se reproducen **por separado pero no juntas**: el `mr r9,r0` sale con la
  comparacion post-bucle contra `NumPlanes` (5 → **4 filas**), y el `li r9,0`
  con la cadena `if` convertida en `switch` (**tamano exacto 836/836**, filas
  95/96/97/101 EXACTAS, y solo sobra el reparto del switch).
- **`SetMemoryPoolSize`**: la r48 lo dio por cerrado leyendo mal la traza. Con
  la **visualizacion por ciclos** se ve que el `addi` **NO** sale en el mismo
  ciclo que el `stw`: sale **un ciclo despues**, y la causa es la regla de
  **clase respecto al ultimo insn programado** de `rank_for_schedule`.
- **`InitAtSegment`**: el swap r10↔r11 queda **cuantificado con `QTY_CMP_PRI`**
  (local-alloc.c:1568) y con los dos umbrales exactos que lo voltean.
- **`UpdateWheelYRenderOffset`**: el mecanismo tiene nombre —**`combine_movables`
  (loop.c:1470)**— y sus cuatro puertas estan leidas del fuente de GCC.
- **`RenderFlaresOnCar`**: **refutada** la hipotesis del inicializador muerto
  (cuatro variantes, objeto identico).
- **`GetLoadingPriority`**: el bloque muerto **mide cero instrucciones** en el
  DWARF del original, y con asignacion secuencial el reparto del marco solo
  cuadra de **una** manera: los 16 B que faltan son un **temporal sin nombre**,
  no una variable.

## Verificacion

`fncmp` antes / despues de las TRES unidades
(`scratchpad/r49_world/base_fncmp_*.txt` y `final_fncmp_*.txt`):

| unidad | antes | despues |
|---|---|---|
| zWorld | 5 funciones / 5.608 B | **`diff` vacio** |
| zWorld2 | 2 funciones / 3.796 B | **`diff` vacio** |
| zTrack | 1 funcion / 708 B | **`diff` vacio** |

Ninguna funcion empeora (ni mejora: no se ha cambiado una linea de codigo).
`lcfix.py --check`: «todas las entradas @lc estan al dia».
`audit.py` en las tres: **0 FALLA**.
`git status`: `CarRender.cpp`, `CarLoader.cpp`, `WRoadNetwork.cpp` y
`TrackStreamer.cpp`, **solo comentarios** (151 lineas insertadas, 0 borradas).
Los cuatro `.o` se reconstruyeron y las tres unidades siguen byte a byte.

**Aviso para el que herede el banco**: `scratchpad/r49_world/v.py` restaura la
fuente desde la copia `.HEAD` al terminar cada tanda. Si has editado la fuente a
mano **antes** de lanzar una tanda, refresca la copia `.HEAD` primero o el banco
te borra el trabajo (me paso una vez en esta ronda).

## 0. La TU reducida, extendida a todo el territorio

La r48 monto `wrn_only.cpp` para `WRoadNetwork.cpp`. Esta ronda hay tres mas
—`cr_only.cpp`, `cl_only.cpp`, `ts_only.cpp`— y **las ocho funciones del
territorio se reproducen byte a byte** con ellas:

| TU reducida | unidad | funciones | fidelidad |
|---|---|---|---|
| `cr_only.cpp` | zWorld | RenderFlaresOnCar 18 / UpdateWheelY 7 / CullParts 5 | exacta |
| `cl_only.cpp` | zWorld | DefragmentPool 23 / SetMemoryPoolSize 2 | exacta |
| `wrn_only.cpp` | zWorld2 | HolePunchAvoidables 4 / InitAtSegment 12 | exacta |
| `ts_only.cpp` | zTrack | GetLoadingPriority 75 | exacta |

El ciclo de medida completo (compilar + `objdiff` + contar filas) baja de **~40 s
a 2-6 s**, y con eso caben ~40 variantes en un cuarto de hora. La receta es
mecanica: **el preambulo de la SourceList hasta el primer `#include` de
`Speed/Indep/Src/`, y un solo `#include`**. En `zWorld` y `zTrack` el preambulo
son 1.097 bytes (el `asm` del prefijo de bWare/STL y los dos `#define` del
`BWARE_PREFIX`); en `zWorld2` hay ademas `BWARE_NO_INLINE_GLOBAL_NEW` y
`ATTRIB_NO_INLINE_CLASSKEY`. Herramienta: `scratchpad/r49_world/h.py`
(`h.py <tu.cpp> <unidad> <simbolo> [tag]`) y `v.py` (banco de variantes).

---

## 1. `CullParts` (zWorld, 836 B, 5 filas, −4 B) — las 5 filas son UNA causa

El objetivo:

```
95  lwz   r0, 0x8(r30)    ; Polarity -> r0        <- nosotros r11
96  li    r9, 0x0         ; debug_print = 0
97  cmpwi r0, 0x0                                  <- nosotros r11
99  lwz   r0, 0xc(r30)    ; NumPlanes
100 li    r31, 0x0        ; n = 0
101 cmpw  r9, r0          ; la guarda, con el CERO DE debug_print  <- nosotros r11
102 mr    r9, r0          ; <- el insn que falta
103 bge
...
144 cmpwi r0, 0x1                                  <- nosotros r11
```

### 1.1 El `mr r9,r0` es el valor de la comparacion post-bucle en el camino que salta el bucle

`bge` (fila 103) salta a la fila 140, que es `cmpw r31, r9` = `if (n == debug_print)`.
En el objetivo, por ese camino `debug_print` vale **NumPlanes**; en el nuestro
vale **0**. O sea que nuestro arbol es demostrablemente distinto del suyo en ese
camino (que ademas es inalcanzable, porque la guarda exterior ya garantiza
`NumPlanes > 0`).

Reproducirlo es una linea, y las filas 101 y 102 salen **EXACTAS**:

| ensayo (todo 832 B) | filas |
|---|---:|
| base | 5 |
| `if (n == plane_info->NumPlanes)` en vez de `== debug_print` | **4** |
| `debug_print = plane_info->NumPlanes;` movido DETRAS del bucle | **4** |
| … lo mismo con el bucle escrito como `while` + `n++` al final | **4** |
| mantener la escritura interna y comparar contra memoria | **4** |

Pero en las cuatro **desaparece el `li r9,0`** (fila 96, DELETE) porque
`debug_print = 0` queda muerto de verdad: se cambian tres filas por una.

### 1.2 El `li r9,0` vivo es lo que manda `Polarity` a r0

Convirtiendo la cadena `if / else if` en `switch (plane_info->Polarity)`:

```
switch: 11 filas, 836/836 B
   fila  95  lwz r0, 0x8(r30)   IGUAL
   fila  96  li  r9, 0x0        IGUAL
   fila  97  cmpwi r0, 0x0      IGUAL
   fila 104  cmpw r9, r0        IGUAL
   lo unico que sobra: el reparto del switch (beq/cmpwi/beq/b en vez de bne)
```

Con `default:` vacio o sin `default`, identico. O sea: **la cadena de `if` es la
forma buena** y el `switch` solo sirve de experimento de control, pero deja la
causa demostrada. `record_jump_equiv` (cse.c) mete `Polarity == 0` en la clase
de equivalencia del `bne`, y cuando CSE tiene que materializar el cero de `n`
para la guarda elige **ese** registro en vez del de `debug_print`; sin la
equivalencia (el `switch` no la crea) elige el de `debug_print`, que asi queda
vivo, y `Polarity` se va al volatil r0.

### 1.3 Lo que falta, y no esta en nuestra fuente

Las dos mitades no se pueden tener a la vez desde ninguna forma de fuente que
hayamos probado, porque son contradictorias: el `mr r9,r0` MATA el `li r9,0`.
La unica salida coherente es que en el original **`debug_print = 0` sea un
almacenamiento muerto** que sobrevive solo porque CSE lo engancha en la guarda
—exactamente lo que hace el `switch`— y que su uso este en el **bloque muerto de
las lineas 635-656**, del que `lmap` dice que no emite una sola instruccion.
Mientras no sepamos que hay ahi, la funcion no cierra.

Dos datos nuevos de `lmap` sobre el original que valen para la reconstruccion:

- **Los dos bucles del original son `while` con `n++` al final.** El incremento
  y la condicion del primero estan en las lineas **680/682** (no en la 661 del
  `for`) y los del segundo en la **716**, con `int n = 0` en la 691 y la
  condicion en la 694. En un `for`, GCC 2.95 imputa el incremento a la linea del
  `for`; que salga 19 lineas mas abajo solo puede venir de un `n++` suelto.
- `int debug_print = 0;` esta en la linea **634** y el `if` de la polaridad en la
  **657**: 22 lineas que no emiten nada.

MEDIDO Y NEGATIVO r49 (todo 832 B salvo donde se dice):

| ensayo | filas | tamano |
|---|---:|---|
| quitar el clobber `asm("":::"r11")` | 9 | 832 |
| `debug_print = NumPlanes` como PRIMERA sentencia del cuerpo | 63 | **836** |
| … tras `bVector3 normal = …` (mismo efecto) | 63 | **836** |
| … al final del cuerpo (`for`) | 66 | 832 |
| `while` + `n++` con la asignacion al final | 66 | 832 |
| `do { … } while (++n < NumPlanes)` | 13 | **816** |
| `debug_print = NumPlanes` DELANTE del bucle | 8 | **828** |
| … y ademas con la cota `n < debug_print` | 8 | **828** |
| `for (debug_print = NumPlanes; n < NumPlanes; n++)` | 8 | **828** |
| `int polarity = plane_info->Polarity;` local | 5 | 832 |
| `!plane_info->Polarity` / `static_cast<int>` de los dos lados | 5 | 832 |
| declarar `debug_print` dentro de la rama | 8 | 832 |
| … tras `int n = 0;` | 7 | 832 |
| `debug_print = 0;` repetido dentro de la rama | 5 | 832 |
| `switch` + comparar contra memoria | 14 | 836 |

**El clobber `asm("" : : : "r11")` es LOAD-BEARING** (5 → 9 filas): no es andamio
retirable, al contrario de lo que sugeria la leccion 3 de la r48.

---

## 2. `SetMemoryPoolSize` (zWorld, 304 B, 2 filas) — la r48 leyo mal la traza

La r48 concluyo que el `addi` y el `stw` «se emiten en el MISMO CICLO, en
unidades distintas» y cerro la funcion por construccion. Con la **visualizacion
por ciclos** que imprime `-fsched-verbose-2` (la traza de listas de listos que
leyo la r48 es la misma informacion, pero la tabla es inequivoca):

```
;;  13   lsu 266 (%5=[%31+0x64])   iu2 240 (%0=%3)
;;  14   lsu 264 (%4=[%31+0x60])   iu2 259 (%6=high($LC))
;;  15   lsu 244 (EL STW)          iu2 262 (%3=%0)
;;  16                             iu2 268 (EL ADDI)
```

El `addi` sale **un ciclo despues**, no en el mismo. Y hay **dos** cosas que lo
empujan, las dos nuevas:

1. **La regla de clase de `rank_for_schedule`** (haifa-sched.c). Cuando las
   prioridades empatan, desempata por la **clase respecto al ULTIMO insn
   programado**: 3 si es independiente de el, 2 si anti/output, 1 si depende por
   dato — **y gana la clase MAS ALTA**. En t=14 el ultimo programado es 259 (el
   `lis`), asi que en t=15 el `addi` (que depende de el) es clase **1** y el
   `stw` clase **3**. La lista de listos sale `268 244 262` (el mejor es el
   ultimo). Esto es un nivel del planificador que el catalogo del brief describe
   pero que aqui aparece **decidiendo solo**, sin que la prioridad ni el LUID
   intervengan.
2. **Una copia redundante que solo existe durante sched2.** El insn 262 es
   `mr r3, r0`, que reload fabrica porque el argumento de `bInitMemoryPool` es
   el mismo valor que se acaba de guardar, y que **desaparece despues de
   sched2** (los dos objetos tienen 304 B y ninguno lleva ese `mr`). Ocupa la
   segunda ranura de t=15 y empuja el `addi` a t=16.

Para invertirlo hace falta que **en t=14 el ultimo programado no sea el `lis`**,
y en t=14 la lista es `262 259 264`: gana 264 (lsu) y despues 259 (iu2). El
objetivo emite el mismo orden en t=13 y t=14 (`mr r0,r3`, `lwz r5`, `lwz r4`,
`lis`), asi que su diferencia esta en la prioridad o en las dependencias del
`stw`, no en el reparto de unidades.

MEDIDO Y NEGATIVO r49 (todo 304 B):

| ensayo | filas |
|---|---:|
| `int pool = bGetFreeMemoryPoolNum(); CarLoaderMemoryPoolNumber = pool;` y pasar `pool` | 2 (objeto IDENTICO) |
| la misma local dejando el global en la llamada | 2 (objeto IDENTICO) |
| asignar `MemoryPoolSize` antes que `MemoryPoolMem` | 4 |

Siguen vigentes las diez vedas de r36e y la de la barrera de ranura de r48.

---

## 3. `InitAtSegment` (zWorld2, 816 B, 12 filas) — el swap r10↔r11, con la formula

Las 12 filas son un ciclo limpio entre dos cantidades **locales** (`local_alloc`,
bloque 4 del `.lreg`):

| pseudo | que es | n_refs | live | `QTY_CMP_PRI` | nuestro | objetivo |
|---|---|---:|---:|---:|---|---|
| 176 | el VALOR de `fNodes` (insn 310, `lwz`) | 5 | 31 | **3225** | r11 | r10 |
| 212 | el `(high $LC472)` del 0,0f (insn 414) | 2 | 8 | **2500** | r10 | r11 |

`QTY_CMP_PRI` (local-alloc.c:1568) es
`int(floor_log2(n_refs) * n_refs * size / (death − birth) * 10000)`, la misma
expresion que `allocno_compare` pero con las cuentas de la cantidad. Gana 176,
se reparte primero y se lleva **r11** —en `REG_ALLOC_ORDER`, para `BASE_REGS`,
r9 va antes que r11 y r11 antes que r10, y r9 esta ocupado—; 212 se queda r10.
El objetivo reparte 212 primero.

**Los dos umbrales, calculados:**

- `live(212) <= 6` (con 7 sale 2857, todavia por debajo de 3225; con 6, 3333).
- `live(176) >= 41` (con 40 empata en 2500 y el desempate por numero de cantidad
  nos vuelve a dar 176 primero).

Y un dato que quita de en medio al planificador: el `.lreg` es **posterior a
sched1**, asi que esos rangos ya son los del codigo planificado; en el asm final
el `lis` y el `lfs` del literal salen a **3 instrucciones**, exactamente como en
el objetivo (`lmap`: 8030199C y 803019A8). **La planificacion es identica; lo
unico distinto es el reparto.**

**La veda del `guard` fantasma, remedida sobre la base actual** (la r47 citaba
«21 filas» contra una base de 15; la base son ahora 12):

| ensayo | filas |
|---|---:|
| base (`register int guard asm("r28")`) | **12** |
| sin `guard` (y sin el `asm` que lo consume) | 49 |
| `guard` en r23 | 49 |
| `guard` en r24 | 33 |
| `guard` en r26 | 18 |
| `guard` en r27 | 15 |

Todas 816/816. **El pin NO es andamio**: r28 es el unico valor que sirve, y es la
respuesta a la leccion 3 de la r48 para esta funcion.

---

## 4. `UpdateWheelYRenderOffset` (zWorld, 876 B, 7 filas, −4 B) — el mecanismo tiene nombre

Es **`combine_movables` (loop.c:1470)**, no CSE. Los tres `(high 0,0f)` del
bucle son **movables** y `loop.c` los iza al preencabezado; el objetivo **combina
dos y deja el tercero suelto**, y nosotros combinamos los tres. `lmap` sobre el
original lo dice literalmente:

```
802CA03C  lis r19, lbl_8040AA84@ha            <- movable A (izado)
802CA060  lfs f26, lbl_8040AA84@l(r19)        <- la carga de la linea 3248, izada
802CA07C  lis r16, lbl_8040AA84@ha            <- movable B (izado, NO combinado)
802CA25C  lfs f0,  lbl_8040AA84@l(r16)        <- linea 3278 (model_width  > 0.0f)
802CA2B4  lfs f0,  lbl_8040AA84@l(r19)        <- linea 3288 (model_radius > 0.0f)
```

Con `-fmove-all-movables` en los cflags de la unidad (esta), todos se mueven; lo
que hay que romper es el **emparejamiento**, cuyas cuatro puertas son, leidas del
fuente: `VARRAY_INT (n_times_set, m1->regno) == 1`, `!m1->global` («A reg used
outside the loop mustn't be eliminated»), `!m1->partial`, y
`rtx_equal_for_loop_p` de los `set_src` mas la igualdad de `dependencies`.
Ninguna se toca desde la fuente sin cambiar el codigo. Por eso el `lis` de
`TweakKitWheelOffsetRear` se queda con el preservado que al objetivo le ocupa el
segundo `@ha`, y de ahi los 4 B de menos.

**Vedas de r36e/r47 remedidas sobre la base actual (7 filas, 872 B), todas siguen
en pie:** `0.0f < x` en el width 7 filas; en el radius 7;
anidar el `&&` del width en dos `if` 27 filas;
`desired_width > 0.0f && model_width > 0.0f` 9 filas.

---

## 5. `RenderFlaresOnCar` (zWorld, 2.908 B, 18 filas, +4 B) — hipotesis refutada

`lmap` sobre el original situa el `lis r16, lbl_8040AD04@ha` que nos falta en
**CarRender.cpp:4061** —el **bloque de entrada**, entre el `lwz` de
`CarTypeInfoArray` y el `xori` de `is_traffic_car`— y **sin ninguna carga que lo
use ahi**: el pseudo nace en el **dominador comun de sus tres usos** (lineas
4225, 4310 y 4314) y vive ~480 instrucciones en un preservado. Todos los demas
usos de ese literal (4080, 4121, 4126, 4132, 4139, 4146, 4152, 4158, 4164, 4169,
4190, 4193, 4196, 4369) rematerializan su propio `lis r9` volatil. El objetivo
tiene **17** `lis` de `lbl_8040AD04` y nosotros **18**: dos para el racimo
4225 + 4310/4314 (uno volatil r9 y otro preservado r30) donde el objetivo tiene
uno solo.

La hipotesis nueva era que ese `lis` saliera de los inicializadores muertos
`float base_headlight_intensity = 0.0f; float base_brakelight_intensity = 0.0f;`
—el `lfs` se muere porque las dos ramas asignan, y el `lis` sobreviviria si CSE
lo hubiera compartido con los usos de abajo, exactamente el mecanismo que el
`switch` demostro en `CullParts`—.

**REFUTADA con la medida**: las cuatro variantes dan **18 filas y 2.912 B,
objeto identico a la base**: declararlos sin inicializar, moverlos detras de
`is_traffic_car`, y dejar solo uno de los dos (cualquiera de los dos). GCC los
borra antes de que CSE pueda engancharlos.

Siguen vigentes las vedas de r46/r47: la referencia sintetica a `lbl_8040AD04`
antes del bucle da el tamano exacto 2908/2908 pero **62 filas** (el `lis` cae en
el preencabezado del bucle, no en el bloque de entrada), y el pin de
`flashHeadlights` a r14 encima sube a 81.

---

## 6. `DefragmentPool` (zWorld, 684 B, 23 filas) — tres ciclos y CERO instrucciones

684/684 B y las 23 filas son **solo renombres**: tres ciclos independientes de
registro preservado. Con el `.greg` delante es puro `allocno_compare`:

| ciclo | pseudo | nuestro | objetivo | n_refs | live | pri |
|---|---:|---|---|---:|---:|---:|
| A | 82 | r22 | **r21** | 9 | 162 | 1666 |
| A | 222 | r21 | **r22** | 4 | 51 | 1568 |
| B | 223 (`ChunkMovementOffset@ha`) | r27 | **r25** | 8 | 54 | 4444 |
| B | 141 | r26 | r26 | 11 | 120 | 2750 |
| B | 139 (el contador) | r25 | **r27** | 14 | 184 | 2282 |
| C | 145 | r18 | **r16** | 4 | 118 | 677 |
| C | 144 | r17 | **r18** | 3 | 61 | 491 |
| C | 225 (`CarLoaderMemoryPoolNumber@ha`) | r16 | **r17** | 4 | 246 | 325 |

`REG_ALLOC_ORDER` reparte r31, r30, r29… asi que **el primero se lleva el numero
mas alto**. El **ciclo C esta exactamente al reves**: el objetivo reparte 144,
luego 225 y luego 145; nosotros 145, 144, 225.

Lo que hace falta, con la cifra:

- **Ciclo A**: bajar `live(222)` de 51 a **47**, o subir `live(82)` de 162 a
  **163**. Es el mas barato de los tres.
- **Ciclo B**: `live(139)` de 184 a **62**, o `live(223)` de 54 a **121**. Caro.
- **Ciclo C**: invertir el orden de los tres. Los tres pseudos son las tres
  locales que r46 llamaba «solo nuestras» (`params`, `table`, `zero`): **no hay
  que quitarlas** —eso costaba 47 filas y +8 B, remedido como vigente— sino
  **cambiarles el rango de vida**.

---

## 7. `GetLoadingPriority` (zTrack, 708 B, 75 filas) — el bloque mide CERO

Dato nuevo del DWARF del original que la r46/r47/r48 no dejaron escrito: el
bloque anonimo del ESP tiene **`Range: 0x802BC340 -> 0x802BC340`**, y lo mismo
**todos** sus inlines (`espCreateObjectAsync`, `espSetAttributeString`,
`GetNumPoints`, `espCreateUserMesh`, `espSetUserMeshFace`) y los dos bloques
anidados. O sea: **el bloque entero no emite un solo byte** y aun asi sus tres
locales estan en el marco. La reconstruccion actual es correcta en forma —el
arbol de bloques y la lista de inlines coinciden uno a uno con los nuestros—.

Con asignacion secuencial (`expand_decl` → `assign_stack_local`, sin reuso) el
reparto del original solo cuadra de **una** manera:

```
0x08  layer_name[32]                 0x20
0x28  pos                            0x10  (FloatVector 0xC + alineacion 8)
0x38  ???                            0x10  <-- LO QUE NOS FALTA
0x48  face[4] INTERIOR               0x30
0x78  face[4] EXTERIOR               0x30  <-- declarado DESPUES del bucle
0xA8  dest de la inline bScale
```

Es decir: **el volcado nombra `face` al objeto de 0x38, pero ese objeto solo
puede medir 0x10**; el `face[4]` exterior tiene que estar en 0x78, detras del
interior. Como el DWARF **no lista una cuarta local** en el bloque, los 0x10 de
0x38 son un **temporal sin nombre** (`assign_stack_temp`), no una variable — y
por eso ninguna declaracion inventada es la respuesta, y por eso **mover el
`face` exterior detras del bucle por si solo no cambia nada** (0x38+0x30+0x30
sigue dando 0x98). Lo que hay que encontrar es la **sentencia que crea ese
temporal de 16 B**.

`sizeof(FloatVector) > 12` sigue refutado (el DWARF dice `total size: 0xC`), y
sigue prohibido rellenar con una local inventada (precedente `ICEMover` /
`9de121ee`).

---

## 8. `HolePunchAvoidables` (zWorld2, 2.980 B, 4 filas) — cerrado, y por que

No se ha vuelto a tocar: la r48 lo dejo cerrado por construccion y el modelo se
sostiene. Anoto por que **no hay una salida por «poner la barrera fuera del rango
de vida»**, que era la unica idea que quedaba sin escribir: las tres
comparaciones CC estan izadas al preencabezado y se usan **en cada iteracion**,
asi que por la arista de retroceso estan vivas en **todos** los bloques del
bucle. Cualquier insn dentro del bucle —este donde este, incluso detras de su
ultimo uso textual— cuenta en `live_length`. Por eso el «+7» no tiene donde
esconderse y el unico camino sigue siendo **un insn menos**.

---

## 9. Propuestas fuera de territorio

1. **La regla de clase de `rank_for_schedule` merece una entrada del catalogo.**
   El brief describe los niveles de `sched1` y dice que en `sched2` deciden
   `depend_count` y `INSN_LUID`; falta el nivel de en medio, que es el que
   decide aqui: **clase respecto al ultimo insn programado, y gana la clase mas
   alta** (3 independiente > 2 anti/output > 1 dato). Consecuencia practica y
   contraintuitiva: **programar una instruccion penaliza en el ciclo siguiente a
   todas las que dependen de ella**, asi que un `lis` colocado tarde empuja su
   propio `addi` un ciclo mas alla. Es un patron que va a repetirse en cualquier
   near-miss de «dos instrucciones intercambiadas justo antes de una llamada».

2. **La visualizacion por ciclos de `-fsched-verbose-2`, no solo las listas de
   listos.** El volcado trae, despues de la traza, una tabla `clock × unidad` con
   una instruccion por celda. Leer esa tabla en vez de las lineas
   `--> scheduling insn` habria evitado el diagnostico erroneo de la r48 (que
   leyo dos `--> scheduling` seguidos como «mismo ciclo» cuando lo que hay entre
   ellos es un cambio de ciclo). Merece una linea en la descripcion de
   `scripts/schedtrace.py`.

3. **Una copia que no existe en el objeto final puede decidir el objeto final.**
   El `mr r3,r0` de `SetMemoryPoolSize` (insn 262) se borra despues de sched2 y
   no esta en ninguno de los dos binarios, pero ocupa una ranura de emision y
   mueve una instruccion de sitio. Cuando el `.greg` y el asm final no cuadren en
   numero de insns, hay que mirar el volcado de **sched2**, no el de reload.

4. **`combine_movables` como frente**, para el que tenga varias unidades con
   `@ha` duplicados. La pregunta «por que el objetivo materializa dos veces la
   misma direccion» tiene una respuesta unica y localizada en loop.c:1470, con
   cuatro puertas comprobables en el `.lreg`. En este territorio no se abre desde
   la fuente, pero el diagnostico es de 5 minutos con `lmap` sobre el original.

5. **El experimento de control con `switch`.** Cuando el diff se queje de un
   registro y no se sepa si la causa es el reparto o una equivalencia de CSE,
   compilar la cadena de `if` como `switch` **aunque se sepa que es la forma
   equivocada** separa las dos cosas en una compilacion: el `switch` no crea la
   equivalencia de `record_jump_equiv` que la cadena de `if` si crea. Aqui valio
   tres de las cinco filas de `CullParts`.

---

## 10. Artefactos

`scratchpad/r49_world/`:

- `base_fncmp_*.txt` / `final_fncmp_*.txt` — el antes/despues de las tres unidades.
- `d_cull_base.txt`, `d_uwy_base.txt`, `d_rfc_base.txt`, `d_ias_base.txt`,
  `d_smp_base.txt`, `d_dp_base.txt` — los `fndiff` citados.
- `lmap_cull.txt`, `lmap_uwy.txt`, `lmap_rfc.txt`, `lmap_ias.txt` — `lmap` sobre
  el ORIGINAL de las cuatro funciones donde ha dado evidencia nueva.
- `rtl/ias_base.lreg` — de donde salen los `n_refs`/`live` de `InitAtSegment`.
- `rtl/dp.greg`, `rtl/dp.lreg` — los tres ciclos de `DefragmentPool`.
- `rtl/smps.sched2` — la traza y la **visualizacion por ciclos** de
  `SetMemoryPoolSize`.
- `cr_only.cpp`, `cl_only.cpp`, `ts_only.cpp`, `wrn_only.cpp` — las TU reducidas.
- `h.py` (medir una variante), `v.py` (banco: parchea, mide y restaura),
  `d.py` (volcar el diff alineado de una variante ya construida), y los bancos
  `c1.py`…`c12.py`, `u1.py`, `r1.py`, `s1.py`, `w1.py`.
- `*.cpp.HEAD` — las copias de partida de las cuatro fuentes.

Volcados RTL grandes borrados (`scratchpad/rtl/{cr,cl,ts,wrn}_only_cpp.*` y
`scratchpad/schedtrace/cl_only_cpp.*`); solo quedan los recortes por funcion de
`scratchpad/r49_world/rtl` (304 kB en total).
