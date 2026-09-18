# R46 --- territorio mwcc: `steering`, `vm`, `vmbase`, `DebuggerDriver`, `FSasync`

Fecha: 2026-09-08. Base: `773fb3d4`.

## Resumen

**No se retiene ningun cambio de fuente**: las cinco unidades quedan byte por
byte como estaban (verificacion al final). El resultado de la ronda es un
**hallazgo estructural en `DebuggerDriver` que vale 804 B y 3 funciones**, y
que la coordinacion puede aplicar en un solo paso porque esta medido de punta a
punta: la unidad son **DOS unidades de traduccion del SDK de Dolphin metidas en
un objeto**, y cada mitad necesita una version distinta de MWCC.

Ademas quedan cerrados con cifra tres ejes que el brief pedia probar
(compilador equivocado en `Effect_Init` y `AmcEXISetExiCallback`, y el barrido
de `#pragma` sobre la fuente ACTUAL de `steering`).

| unidad | antes | despues | propuesta |
|---|---:|---:|---|
| `LibSN/steering` | 8 fn / 2.972 B | igual | --- |
| `LibSN/vm` | 1 fn / 168 B | igual | --- |
| `LibSN/vmbase` | 1 fn / 380 B | igual | --- |
| `DebuggerDriver` | 3 fn / 804 B | igual | **partir en dos objetos: 0 fn / 0 B** |
| `FSasync` | 1 fn / 388 B | igual | --- |

---

## 1. `DebuggerDriver`: son dos unidades de traduccion, no una

### 1.1 La medida que lo destapa

Barrido de **las 19 versiones de MWCC del arbol** sobre la fuente actual
(`scratchpad/r46_mwcc/sweep.py`):

```text
GC\1.0         3/31 distintas, 416 B
GC\1.1         3/31 distintas, 416 B
GC\1.1p1      26/31 distintas, 5908 B
GC\1.2.5       3/31 distintas, 416 B
GC\1.2.5n      3/31 distintas, 804 B     <- la actual
GC\1.3 .. 2.7 27/31 distintas, 5972 B
GC\3.0a3 .. 3.0a5.2  27/31 distintas, 5972 B
```

Lo decisivo no es el numero, es **QUE funciones fallan**:

| version | funciones con codigo distinto |
|---|---|
| `GC/1.2.5n` (actual) | `EXI2_WriteN` 432, `EXI2_Poll` 264, `AmcEXISetExiCallback` 108 |
| `GC/1.2.5` (y 1.0, 1.1) | `DBQueryData` 156, `DBRead` 140, `DBInitComm` 120 |

Los dos conjuntos son **disjuntos y estan separados por una direccion**. El
plano del objeto objetivo (`scratchpad/r46_mwcc/layout.py`):

```text
000000  DBClose ... DBGEXIImm            0x000000-0x000A80   14 funciones  2.688 B
000A80  EXI2_CallBack ... EXI2_Unreserve 0x000A80-0x001124    8 funciones
001124  AmcEXIImm ... AmcEXIInit         0x001124-0x001764    9 funciones  (3.300 B con el bloque anterior)
```

Las tres que falla `1.2.5n` estan **todas** despues de 0x000A80; las tres que
falla `1.2.5` estan **todas** antes. Ese corte es exactamente la frontera
`DebuggerDriver.c` / `AmcExi2.c` del SDK de Dolphin. En direcciones del DOL:
**0x803465D0** (`EXI2_CallBack`, confirmado en `symbols.txt`).

La diferencia entre las dos familias es **el planificador del epilogo**, nada
mas. Ejemplo (`AmcEXISetExiCallback`):

```text
objetivo y 1.2.5:  lwz r0,0x1c(r1) / mr r3,r31 / lwz r31 / lwz r30 / mtlr r0 / addi r1 / blr
1.2.5n:            mr r3,r31 / lwz r0,0x1c(r1) / lwz r31 / lwz r30 / addi r1 / mtlr r0 / blr
```

y al reves en `DBInitComm`:

```text
objetivo y 1.2.5n: lwz r31 / lwz r30 / lwz r29 / addi r1 / mtlr r0 / blr
1.2.5:             lwz r31 / lwz r30 / mtlr r0 / lwz r29 / addi r1 / blr
```

### 1.2 La prueba: el corte compila al 100 %

`scratchpad/r46_mwcc/split_clean.py` parte la fuente en las dos mitades **y
reparte tambien los estaticos** (el reparto sale limpio: ningun estatico se usa
a los dos lados; solo `__EXIRegs`/`__PIRegs`, que son declaraciones
`AT_ADDRESS` y no emiten datos). Resultado:

```text
=== parte A (GC\1.2.5n) rc=0 ===
       B  funcion                                        diferencia
                                              <- NINGUNA
   seccion .text        00000a80      (14 funciones, 2.688 B)
   seccion .sbss        00000016
=== parte B (GC\1.2.5) rc=0 ===
       B  funcion                                        diferencia
                                              <- NINGUNA
2 mas solo con nombres de simbolo distintos (964 B) -- casi siempre alias:
        700  EXI2_ReadN                                     2 reub
        264  EXI2_Poll                                      2 reub
   seccion .text        00000ce4      (17 funciones, 3.300 B)
   seccion .data        00000019
   seccion .sdata       00000004
   seccion .sbss        0000000c
```

2.688 + 3.300 = 5.988 = el `.text` entero de la unidad. **31 de 31 funciones
exactas.** Las dos filas de `EXI2_ReadN`/`EXI2_Poll` son el saco de alias de
siempre (mismas instrucciones, nombre de simbolo distinto), ya validadas al
100 % por `audit.py` en la r37.

### 1.3 La propuesta, con coordenadas exactas

`DebuggerDriver` es `linked False`, asi que **esto no puede mover el DOL**.

**Opcion A --- una linea, sin partir nada.** En `configure.py`, bloque
`"lib": "OdemuExi2"`, cambiar `"toolchain_version": "GC/1.2.5n"` por
`"GC/1.2.5"`: la unidad pasa de 804 B pendientes a 416 B. **+388 B matched**,
mismas 28 funciones exactas. Cero riesgo, cero trabajo de datos.

**Opcion B --- el corte, +804 B y +3 funciones, la unidad queda Matching.**

1. `config/GOWE69/splits.txt`, sustituir la entrada actual por dos:

```text
Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c:
	.text       start:0x80345B50 end:0x803465D0
	.sbss       start:0x804FFC48 end:0x804FFC5C     (ver §1.4: el reparto de datos)

Speed/GameCube/bWare/GameCube/OdemuExi2/src/AmcExi2.c:
	.text       start:0x803465D0 end:0x803472B4
	.data       start:0x8044F610 end:0x8044F629
	.sdata      start:0x804FF588 end:0x804FF594
	.sbss       start:0x804FFC5C end:0x804FFC6C
```

2. Partir la fuente por la linea 433 (`static void EXI2_CallBack(...)`),
   duplicando la cabecera de tipos/registros/externs (lineas 1..117) y
   repartiendo los estaticos:

   - se quedan en `DebuggerDriver.c`: `MTRCallback`, `DBGCallback`,
     `SendMailData`, `RecvDataLeng`, `pEXIInputFlag`, `EXIInputFlag`,
     `SendCount`;
   - pasan a `AmcExi2.c`: `exi_804FF598`, `fExi2Selected`, `TRK_Callback`,
     `pucEXI2InputPending` (y la cadena `"Can't select EXI2 port!\n"`, que es
     la `.data` de 0x19 B).

   Los dos ficheros ya escritos y compilados estan en
   `scratchpad/r46_mwcc/clean_A.c` y `clean_B.c`.

3. `configure.py`: el bloque `OdemuExi2` pasa a dos objetos, el segundo con
   `toolchain_version="GC/1.2.5"` (el override por objeto ya existe en el
   arbol, se usa igual en `Runtime.PPCEABI.H/__va_arg.c`).

### 1.4 Aviso de datos (vale para las dos opciones)

Comparando las secciones de datos del objeto extraido con las nuestras aparecen
**cuatro divergencias que no son del corte** y que ya existen hoy:

| objetivo | nuestro |
|---|---|
| `SendCount` en `.sdata` con valor **0x80** | en `.sbss`, sin inicializar |
| `ucEXI2InputPending` (u8) en `.sbss` +0x18 | **no existe** |
| `pucEXI2InputPending` en `.sdata` +8 inicializado a `&ucEXI2InputPending` | en `.sbss`, sin inicializar |
| `exi_804FF598` es **UND** (lo define otra TU) | estatico local `= &Ecb` en `.sdata` |

Medido (`scratchpad/r46_mwcc/data_parity.py`): con los cuatro cambios el
`.text` **no se mueve** (siguen 3/31 y 804 B con `1.2.5n`) y `.sdata`/`.sbss`
pasan de 4/0x28 a 8/0x20, contra 0x0c/0x24 del objetivo. Lo que falta son
huecos de relleno que el troceador ya marca como `gap` (7 B tras `SendCount` y
3+3 B alrededor de `ucEXI2InputPending`), o sea que el original alinea a 4/8 lo
que MWCC nos empaqueta.

**No lo he aplicado** por una razon concreta: `exi` vive en **0x804FF598**,
que en `symbols.txt` existe (`exi = .sdata:0x804FF598`) pero **ningun rango de
`splits.txt` lo cubre** --- es un huerfano. Pasarlo a `extern` sin darle dueña
deja un simbolo indefinido, y el precedente del proyecto («el rango no basta»)
dice que eso es trabajo de coordinacion, no de territorio.

### 1.5 Lo medido y negativo aqui

- **30 `#pragma` x 2 regiones x 2 versiones = 120 compilaciones.** Con
  `GC/1.2.5` acotando el pragma a la region DB (lineas 119..433): ninguno baja
  de 416 B; los que mueven algo empeoran (`scheduling off` 5.760 B,
  `peephole off` 2.680 B, `optimization_level 3` 1.024 B, `optimize_for_size on`
  1.080 B). Con `GC/1.2.5n` acotandolo a la region EXI2/Amc (433..EOF):
  ninguno baja de 804 B (`opt_propagation off` 1.992 B es lo mejor que no es
  la base). **El eje de pragma esta cerrado en las dos direcciones**: no hay
  forma de que una sola version sirva a las dos mitades.

---

## 2. `steering` --- 8 funciones, 2.972 B: nada nuevo, tres ejes cerrados con cifra

### 2.1 Version de compilador: cerrado

19 versiones sobre la fuente actual:

```text
GC\1.0 .. 1.2.5n   34/36, 8.668 B
GC\1.3              12/36, 4.864 B
GC\1.3.2, 1.3.2r     9/36, 3.780 B
GC\2.0 .. 2.7        8/36, 2.972 B   <- optimo, empate de cinco versiones
GC\3.0a3 .. 3.0a5.2 33/36, 8.544 B
```

`1.3.2` no arregla ninguna funcion nueva: su lista es la de `2.7` **mas**
`Effect_UpdateEffect` (808 B) y `HandleTriggers` con tamaño 620/588. Y las 8
pendientes estan **repartidas por todo el plano** del objeto (`HandleTriggers`
en 0x000224, `SimThread_Step` en 0x001E9C), asi que **`steering` no admite el
corte que si admite `DebuggerDriver`**: es una sola TU.

Las dos candidatas que pedia el brief al test del «compilador equivocado»:

- `Effect_Init` (276 B, 88,62 %): **ninguna de las 19 versiones la cierra**.
  1.0-1.2.5n dan 280 B, 1.3-2.7 dan los 26 insn de siempre, 3.0a* dan 284 B.
- `AmcEXISetExiCallback` (108 B, 85,19 %): **el test da POSITIVO** --- es la
  seccion 1. Era el compilador, no la fuente.

- `VDevice_RecalcGammaTable` (216 B, 99,80 %): la r20 dejo demostrado que toda
  la diferencia es el marco (0x20 objetivo contra 0x30 nuestro) porque MWCC
  nunca reutiliza una ranura de conversion, y dejo abierto «el camino es el
  compilador o las banderas». **Cierro el medio eje del compilador**: de las 19
  versiones, 1.3-2.7 dan marco 0x30, 3.0a* dan 0x80, y 1.0-1.2.5n generan un
  cuerpo distinto (salvan f27-f31 y el prologo cambia). **Ninguna da 0x20.**

### 2.2 `#pragma` por funcion sobre la fuente ACTUAL: cerrado

La r20 barrio 30 pragmas por funcion, pero eso fue **antes** de que r22/r39/r40
reescribieran `HandleTriggers`, `CookValues`, `SimThread_*` y
`Effect_PerformEnvelope`. Repetido sobre la fuente de hoy
(`scratchpad/r46_mwcc/pragall.py`, 8 funciones x 29 pragmas = 232
compilaciones): **ni un acierto**. Lo unico que merece quedar escrito:

| funcion | base | lo mejor que hace un pragma |
|---|---|---|
| `SimThread_Step` 924 B | 920/924 | los 29 dan el mismo objeto |
| `HandleTriggers` 588 B | 42 insn | `scheduling 604`/`7400` 42 insn; el resto peor |
| `SimThread_Init` 324 B | 8 insn | los 29 dan 8 insn |
| `Effect_Init` 276 B | 26 insn | los 29 dan 26 insn o peor (`scheduling 7400` 28) |
| `CookValues` 260 B | 13 insn | `scheduling 601` 21 insn; el resto peor |
| `VDevice_DownloadEffect` 188 B | 6 insn | `scheduling 604` **7 insn**, `opt_common_subs off` 9 |
| `Effect_PerformEnvelope` 196 B | 192 B | **`global_optimizer off` y `opt_common_subs off` dan el TAMAÑO EXACTO, 196 B, con 32 insn distintas** |

Ese ultimo es el unico dato con futuro: confirma que el `mr r5,r9` que le falta
a la funcion **lo produce apagar la CSE global**, pero a cambio reordena medio
cuerpo. No es un cierre; es la direccion.

### 2.3 Ensayo de fuente nuevo y negativo: `Effect_PerformEnvelope`

Hipotesis (no estaba en las 10 formas de la r22): el objetivo tiene `out` en
**r5, el registro del parametro `level`**, y hace `mr r5,r9`; eso pasaria si la
fuente original **reutilizara el propio parametro como variable de salida**, ya
que un pseudo precoloreado no coalesce con otro.

```c
mag = level < 0 ? -level : level;
neg = (u32)level >> 31;
level = mag;                 /* en vez de out = mag = ... */
...
return level;
```

**Objeto IDENTICO al base**, byte por byte (192 B, mismas 30 filas distintas).
MWCC 2.7 elimina la copia igualmente. **Veda nueva**: reutilizar el parametro
como salida no separa los pseudos en esta unidad; tampoco combinado con los 29
pragmas (mismo cuadro que arriba). Van **once** formas medidas en esta funcion.

### 2.4 Diagnostico que dejo escrito para quien siga

`SimThread_Step` (920 contra 924): la unica instruccion que nos falta es
`mr r31, r7` en el bloque de 0x1F34. Al objetivo `dt` le vive en un **volatil
(r7)** hasta el `stw r7,0xa8(r3)` y ahi lo copia a un preservado (r31) para el
`Effect_Update(e, dt)` de mucho despues; nosotros lo calculamos directamente en
r31 (`subf r31,r0,r3`) y no hay copia. Es una **particion de rango de vida** de
`dt`, no una variable de mas: en el objetivo r31 solo aparece dos veces, en el
`mr` y en el `mr r4,r31` de la llamada. Los tres ceros del mismo bloque tambien
salen distintos (el objetivo hace `li r8,0` + dos `mr`, nosotros tres `li`),
pero eso no cambia la cuenta.

---

## 3. `vm` y `vmbase`: el eje del compilador tambien cerrado

`__VMAllocVirtualToARAMLUT` (168 B) y `__VMBASESetupExceptionHandlers` (380 B)
llevan 8+24 y 21 formas medidas en rondas anteriores. Anado la que faltaba, las
19 versiones, funcion por funcion (`scratchpad/r46_mwcc/whichver.py`):

```text
__VMAllocVirtualToARAMLUT     1.0-1.2.5n 164/168   1.3-2.7 164/168   3.0a* 260/168
__VMBASESetupExceptionHandlers 1.0-1.2.5n 392/380  1.3 y 1.3.2-2.7 12 insn  3.0a* 368/380
```

**Ninguna version cierra ninguna de las dos**, y como en `steering` no hay
corte posible (son la unica funcion pendiente de su unidad). Total de la unidad:
`vm` 168 B con 1.3-2.7 (2.236 B con 1.0-1.2.5n, 2.092 con 3.0a*); `vmbase`
380 B con 1.3.2-2.7 (608 con 1.3, 2.244 con 1.0-1.2.5n, 1.576 con 3.0a*). Las
banderas actuales son las correctas.

Ensayo de fuente nuevo y negativo en `vm` --- meter la asignacion del global
**dentro** del argumento, para que el valor siga vivo mientras se calcula r3:

```c
OSSetArenaLo((void *)((u32)(g_baseVMtoARAM = base) + VM_VIRTUAL_LUT_ENTRIES * sizeof(u32)));
```

**164 B, objeto identico al base.** MWCC canonicaliza la expresion al mismo
orden. Con esta van **nueve** formas de fuente en la funcion. **Veda nueva.**

---

## 4. `FSasync`: OJO, no es Metrowerks

El encargo dice que el territorio es «todo compilado con Metrowerks». **No lo
es**: `build.ninja` compila `FSasync` con la regla **`prodg`** (ProDG 3.9.3,
`-O1 ... -O2 -G0 -x c`), o sea **GCC**. Las palancas de GCC si valen aqui; lo
que no valen son las de MWCC.

Dicho eso, los dos ejes que faltaban dan negativo:

- **5 versiones de ProDG** (3.5, 3.5b140, 3.7, 3.8.1, 3.9.3): **las cinco dan
  1/13 y 388 B**. El eje de version esta cerrado.
- **11 banderas nuevas** (las 8 de la r36e no se repiten):

```text
(base)                        1/13, 388 B
-fno-thread-jumps             1/13, 388 B      -fno-regmove          1/13, 388 B
-fno-caller-saves             1/13, 388 B      -fno-function-cse     1/13, 388 B
-fno-delayed-branch           1/13, 388 B      -fno-defer-pop        1/13, 388 B
-fno-optimize-register-move   1/13, 388 B      -fno-rerun-loop-opt   1/13, 388 B
-fno-omit-frame-pointer      13/13, 2.104 B    <- catastrofica
-fno-loop-optimize / -fno-peephole2   no existen en este cc1
```

Diagnostico confirmado (coincide con la veda r36e/r36f): nos sobran **tres**
`lis g_nBlockCnt@ha` y nos falta **uno** --- 4 materializaciones contra 2 del
objetivo, +8 B netos. En la primera region el objetivo mete el `@ha` en un
**preservado (r30) desde el predecremento** y lo reusa en el bucle; nosotros lo
ponemos en un volatil (r9) para el predecremento y `loop.c` crea otro en el
precabezal (r31). En la segunda region el objetivo **no iza** el `@ha` fuera del
bucle exterior (lo rehace tras cada `bl ReadSyncNext`) y nosotros si (r28) y
ademas creamos el del bucle interior (r31). Ese preservado de mas es el
`stmw r23` contra `stmw r24` y el marco 0x30 contra 0x28.

**Las palancas de `asm` que la r36f no toco: SIETE formas mas, todas negativas.**
La r36f solo probo `"+m"`/`"m"` sobre el global. Aqui van las que faltaban,
acotadas al cuerpo de `CompletePCreadAsync` (`scratchpad/r46_mwcc/fs_try2.py`,
para que no contaminen `ReadSyncNext`/`PCreadAsyncNext`, que llevan el mismo
bucle copiado):

```text
(base)         1/13 total 388 B   CompletePCreadAsync 396/388
r_remaining    1/13 total 388 B   396/388    asm("" : "+r"(remaining))
r_next         1/13 total 388 B   396/388    asm("" : "+r"(next))
aquien         1/13 total 388 B   396/388    asm("" : "+r"(remaining) : "r"(next))
cadena3        1/13 total 388 B   396/388    un eslabon por dependencia (3 asm)
ranura_pre     1/13 total 388 B   396/388    asm("") antes del predecremento
ranura_post    1/13 total 388 B   396/388    asm("") detras del predecremento
mem_lazo       1/13 total 388 B   396/388    asm("" : "+m"(g_nBlockCnt)) en el lazo
```

Las siete dan **el mismo tamaño y ninguna rompe otra funcion**. Y el motivo es
estructural, no de dosis: **las palancas de `asm` atan VALORES, y lo que sobra
aqui es una parte alta de DIRECCION (`lis sym@ha`), que no tiene nombre en C**
--- ningun operando de `asm` puede nombrarla. Un puntero si la nombraria, pero
entonces GCC materializa la direccion COMPLETA (`addi rN,rN,@l` y `0x0(rN)`),
que es la forma que la r36e ya retiro por ser la equivocada.

Con esto van **veinticuatro** formas medidas en esta funcion. Lo unico que
quedaria por probar es cambiar la estructura del bucle de `loop.c` (impedir el
izado del `@ha` fuera del bucle exterior), y para eso no hay bandera en este
cc1: `-fno-loop-optimize` no existe y `-fno-rerun-loop-opt` no toca nada.

---

## 5. Verificacion obligatoria

Fuentes tocadas: **ninguna**. Todo el trabajo se hizo sobre copias en
`scratchpad/r46_mwcc/`.

```text
git status --porcelain -- src/LibSN/steering.c src/LibSN/vm.c src/LibSN/vmbase.c \
    src/LibSN/FSasync.c \
    src/Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c
    -> vacio

python scripts/build_direct.py steering vm vmbase DebuggerDriver FSasync
    -> 5 ok, 0 fallidas
```

`fncmp` despues, identico al de antes en las cinco unidades:

```text
LibSN/steering   8 de 36,  2.972 B   (924/588/324/276/260/216/196/188)
LibSN/vm         1 de 18,    168 B
LibSN/vmbase     1 de 30,    380 B
DebuggerDriver   3 de 31,    804 B
FSasync          1 de 13,    388 B
```

`python scripts/lcfix.py --check` -> **«todas las entradas @lc estan al dia»**.

Ninguna funcion empeora porque ninguna fuente cambia.

## 6. Artefactos

Todo en `scratchpad/r46_mwcc/`:

- `tryver.py` --- compila una unidad con OTRA version/banderas/fuente **sin
  tocar el arbol** (lee la regla y los cflags de `build.ninja` como
  `build_direct.py`) y la compara con el objeto extraido. Una medida = 0,8 s.
- `fncmp2.py` --- `fncmp` con los dos objetos por variable de entorno.
- `sweep.py` (versiones), `flagsweep.py` (cflags), `pragfn.py`/`pragall.py`
  (`#pragma` por funcion o por region), `whichver.py`/`checkfn.py` (estado de
  UNA funcion en cada version), `layout.py` (plano del objeto objetivo),
  `var.py` (una variante de fuente), `mk.py` (parche anclado, respeta CRLF).
- `split_proof.py` y `split_clean.py` --- la prueba del corte de
  `DebuggerDriver`; dejan `clean_A.c` y `clean_B.c` listos para copiar.
- `data_parity.py` --- la medida de las cuatro divergencias de datos de §1.4.
- `fs_try2.py` --- las siete formas de `asm` de GCC sobre `CompletePCreadAsync`.

## 7. Propuestas fuera de territorio, ordenadas por valor

1. **`DebuggerDriver` en dos objetos** (§1.3 opcion B): **+804 B y +3
   funciones**, unidad Matching. Corte en `.text` 0x803465D0. `linked False`,
   el DOL no se puede mover. Ficheros y medida ya hechos.
2. **`DebuggerDriver` a `GC/1.2.5`** (§1.3 opcion A): **+388 B** cambiando una
   linea de `configure.py`, si no se quiere partir la unidad todavia.
3. **Dueña para `exi` (0x804FF598)** en `splits.txt` (§1.4): sin ella, la
   paridad de datos de `DebuggerDriver` no se puede cerrar.
4. Corregir el encargo: **`FSasync` es GCC/ProDG**, no Metrowerks (§4). Ya no
   es un frente abierto: las siete formas de `asm` que faltaban estan medidas y
   el motivo del muro es que la palanca no puede nombrar una parte alta de
   direccion.
