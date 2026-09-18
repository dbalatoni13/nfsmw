# r61 · `phys` — zMisc: `rodata +2840` → **+1048**, y `UpdateLoaded` pasa a veda dura

Dos resultados, los dos medidos hoy y los dos con su control:

1. **zMisc**: las 150 cadenas duplicadas bajan a **22**, `2.186 B → 254 B` en el objeto y
   **`rodata +2840 → +1048` en el ENLACE (−1.792 B)**, con `.text +0` y `fncmp 0 de 450`
   antes y después. La `.rodata` de cadenas queda **exacta**: `rodorden` da *307 de 307 en
   secuencia, fuera 0*, y lo único que sobra son 22 cadenas pegadas al final.
2. **`UpdateLoaded`** (el único bloqueo de zPhysicsBehaviors, que está a `.text +4`)
   deja de ser DUDOSA y pasa a **VEDA DURA con la desigualdad exacta y su línea de
   `cse.c`**. Con dos formas nuevas medidas y un control que tenía que cambiar y cambió.

```
sellos (TRES compilaciones cada uno, regla 7)
  zMisc              5e7ed0fcaf3289328f8f92268d1cae503100b573   x3   (base: da892bbf6d8dced6050df7d698a8ac60dea20e2e x3)
  zPhysicsBehaviors  729dc6bd28ef6b061099e8d4ee5b998d5e7496e4   x3   (base: e4fceb82377624a0ba2bda0923b00b0648c44f7b, solo cambia .debug_line)
  zPhysics           0ab50993ec35d55cf8949e3b6689f1d240be3059        (sin tocar, = r60)
  zGameplay          54c66cf43b6293b4018e32648ef59c6e3f1b6fca        (sin tocar, = r60)

linkdelta   zMisc              rodata+2840 data+32  ->  rodata+1048 data+32   (.text +0 en los dos)
            zPhysicsBehaviors  .text +4  resto IGUAL                          (antes y despues)
            zPhysics           .text +0  resto IGUAL
            zGameplay          .text +0  resto IGUAL
fncmp       zMisc              0 de 450   antes y despues
            zPhysicsBehaviors  1 de 1120 (UpdateLoaded, 860/856)  antes y despues
dupstr      zMisc              150 cadenas / 2186 B  ->  22 cadenas / 254 B
rodorden    zMisc              307/307 en secuencia, fuera 0; sobran nue[307:329]
trypromo    zMisc              DOL ROTO (0db797b03f15)
lcfix --check  127 pendientes (88 zFe, 39 zSpeech).  CERO mias y NINGUNA venenosa.
keep.lst / splits.txt / configure.py:  NO TOCADOS.  Cero propuestas.
```

---

## 1. zMisc: la palanca, y por qué la primera versión regresaba

El bloque escrito a mano de `zMisc.cpp` (450 símbolos, 8.152 B) trae **150 cadenas que el
objetivo tiene UNA sola vez**, y `dupstr` demuestra que las tiene **en el offset exacto** en
que nuestro bloque las pone:

```
  29 B x2/t1  ours@['0x7e4', '0x1e90'] ['-', '$LC437']  tgt@['0x7e4']  'RenderConn::UpdateServices()'
```

O sea: la copia buena es la del bloque (offset 0x7e4, sin nombre) y la que sobra es el `$LC`
que `cc1plus` interna en cuanto un `.cpp` de la unidad escribe el literal. Verificado que
`lbl_803F4878` está en el **byte 0** de nuestra `.rodata` (tabla de símbolos del ELF), así
que el offset del volcado es directamente el desplazamiento desde ese símbolo.

**Primer intento (NEGATIVO, y hay que dejarlo escrito):** una sola macro

```cpp
#define ZMISC_POOL(off) (_bwarePrefix + (off))
```

aplicada a las 113 cadenas con sitio único **baja 1.681 B pero ROMPE NUEVE FUNCIONES**
(`fncmp` 0 → 9, `linkdelta .text +0 → −28`). La razón es que todas comparten el **mismo
symbol_ref**: cuando dos `ZMISC_POOL` caen en la misma función, GCC comparte el `@ha` y la
función sale **más corta** que el objetivo (`LoadGlobalChunks` 952 contra 964, −12 B). El
original no podía compilarse así porque allí cada cadena es un `$LC` **distinto**.

**Versión que sí vale:** el propio bloque a mano ya declara `.globl` en 387 sitios, y
**91 de las 150 cadenas caen EXACTAMENTE sobre uno de esos símbolos** (delta 0). Usando el
símbolo propio de cada cadena en vez de un offset sobre una base común, los `high` son rtx
distintos y **no hay nada que compartir**:

```cpp
extern const char _zmsE5C[] asm("$LC2151634644");   // 'Global\InGameSplitScreen.bun'
...
LoadResourceFile(_zmsE5C, RESOURCE_FILE_INGAME, 9, ...);
```

Con las 91 por símbolo propio y el resto por `ZMISC_POOL(off)`, **130 cadenas (1.962 B)** y
**una sola función regresa** (`LoadFrontEndVault`, 2 insn). Devolviéndole el literal a esas
dos apariciones: **`fncmp` 0 de 450** y 128 cadenas fuera.

### Lo que queda de zMisc, y por qué no lo he tocado

| resto | B | por qué |
|---|---:|---|
| 22 cadenas duplicadas | **254** | **nacen de CABECERAS COMPARTIDAS**, no de los 28 `.cpp` de la unidad |
| `.rodata` que no es cadena | ~794 | `lcpool` da `+1232` de objeto contra `254` de cadena |
| `.data` | +32 | sin diagnosticar, viene de antes |

Las 22 son `SMS_MESSAGE_%d{,_FROM,_VOICE,_SUBJECT}`, `GManager`, `DebugVehicleSelection`,
`HeliSoundConn`/`Pkt_Heli_Open`/`CarSoundConn`/`Pkt_Car_Open`, `19.8.31`/`16.2.1`/`1.2.3`,
`GLOBAL\FE_ATTRIB.BIN` y sueltas. Las he rastreado: salen de `SoundConn.h`,
`GRaceDatabase.h` y `uiSMSMessage`, es decir **de cabeceras que comparten zWorld, zSim, zFe
y zAnim** — y esas unidades ya han resuelto lo mismo con la función `case N: return "…";`
dentro de SU SourceList (`zFe.cpp:134-156`, `zAnim.cpp:64`). **Regla 6: no las toco.**
Propuesta en §4.

---

## 2. `UpdateLoaded`: la desigualdad, con su línea

zPhysicsBehaviors está a **`.text +4`** y `dolwhere` ni siquiera puede medirlo — dice
`LAS SECCIONES NO COINCIDEN: obj a=803A41B8 s=249A0 / nue a=803A41C0 s=249A0`: mismo
tamaño, **8 B de desplazamiento**, que son los 4 B de `UpdateLoaded` alineados a 8. Es
literalmente **una función y cuatro bytes** de la promoción.

Arnés nuevo (2,5 s por prueba, reproduce la función al dígito: 860 B, 97,21028 %, 21 filas):
`scratchpad/phys61/{mini2.cpp,cc.py,dif.py,rtl.py}` — prefijo de `zPhysicsBehaviors.cpp`
más una copia editable de `SuspensionTraffic.cpp` en el scratchpad, nunca sobre `src/`.

### Lo que dicen los volcados FRESCOS (`-dG -ds -dt -dl -dg`)

* **PRE lo hace bien**: `PRE: redundant insn 528/694/751/860/920 (expression 40) …
  reaching reg is 362`, e inserta `(set (reg 362) (high *$LC251))` como **insn 988**, al
  final del bloque del `bl VU0_Atan2` — **la ranura exacta del `lis r30` del objetivo**
  (fila 86). Cero líneas `COPY-PROP`, igual que en la r54.
* **DATO NUEVO que la r54 no vio**: `cse2` **no deshace las cinco**. La del `if` de la línea
  372 sobrevive: `insn 468` usa `(lo_sum (reg 362) $LC251)` y el `(set (reg 218) (high))` de
  `insn 465` queda muerto, porque los dos caen en el **mismo bloque extendido `955..480`**.
  `.lreg` lo confirma: `Register 362 used 2 times … in block 17`. **Nos falta UNA
  referencia, no cinco** — y por eso `update_equiv_regs` lo hunde (insn 1049) y le toca r9.
* **La que falta** es la de la línea 379 (`insn 528`, bloque extendido `965..609`). Allí
  `cse2` rematerializa, y es una desigualdad de tabla de costes:

  > `cse.c:7191` elige `src` (el pseudo) sólo si `src_cost <= src_eqv_cost`.
  > `COST` de un pseudo es **1** (`cse.c:519-524`, rama `REGNO >= FIRST_PSEUDO_REGISTER`);
  > `notreg_cost` del `(high …)` de la nota `REG_EQUAL` es **0**
  > (`rs6000.h:2513`, `CONST_COSTS`, `case HIGH: return 0`).
  > **`1 <= 0` es falso para TODO pseudo y TODA fuente.**

* **La única puerta del código** es `cse.c:6837`
  (`if (elt && src_eqv_here && src_eqv_elt) src_eqv_here = 0;`): exige que el reg 362 **y**
  el `(high $LC251)` ya estén en la tabla de `cse2` en ese insn, o sea que el uso caiga en
  el **mismo bloque extendido** que la inserción de PRE. `cse_end_of_basic_block`
  (`cse.c:8508`) corta en la primera `CODE_LABEL` y sólo sigue un salto cuyo
  `LABEL_NUSES == 1`. El bloque de la línea 379 es una **UNIÓN** de dos caminos (el `&&` de
  la 372), luego abre bloque extendido nuevo **siempre**. Y **el objetivo tiene esa misma
  unión** (sus dos `bso` a `0x19a50`). No hay fuente que lo cambie.

### Formas nuevas (ninguna está en la tabla de 30 de la r54)

| forma | B | filas | `lis r30` |
|---|---:|---:|:--:|
| base | 860 | 21 | no |
| ifs anidados **con el `else` duplicado** (parte la etiqueta de unión en dos, `NUSES==1` cada una) | 864 | 52 | **no** |
| `if (!(0<mEBrake)) … else if (!(1<Abs)) … else …` | **856** | 42 | no |

La primera mata una hipótesis que valía la pena matar: *«las dos `bso` al mismo destino del
objetivo podrían venir de un cross-jump FINAL, y en `cse2` haber sido dos etiquetas con
`NUSES==1`»*. Se parten, y **sigue sin haber `lis r30`**: la unión de caminos no la arregla
duplicar el `else`.
La segunda es **el tamaño correcto por el motivo equivocado** — el `!` cambia la comparación
a `bgt` y pierde el `cror un,eq,lt; bso` del objetivo. Es la trampa de «una diferencia de
tamaño no es cercanía», en su versión más cara.

### El control que TENÍA que cambiar, y cambió

`-fno-rerun-cse-after-loop`: 848 B / 84,67 % **y emite `lis r29, $LC249@ha` en un preservado
con cuatro usos**. O sea: la forma del objetivo **la fabrica PRE y la borra `cse2`**,
confirmado en las dos direcciones con una sola compilación.

**Veredicto**: `UpdateLoaded` **no se cierra desde la fuente con estas banderas**. Pasa de
DUDOSA a VEDA DURA. El día que se abra será por una palanca de banderas por unidad, no por
una sentencia. Todo esto queda escrito **junto a la función**
(`SuspensionTraffic.cpp:339-387`), donde `previo.py` lo encuentra.

---

## 3. Negativos y regresiones

* **NEGATIVO 1 (zMisc)**: la macro única `(_bwarePrefix + off)` para las 150 cadenas
  **regresa 9 funciones y −28 B de `.text`**. Cifra: `fncmp` 0 → 9, `linkdelta .text +0 →
  −28`, `LoadGlobalChunks` 952/964. La causa es la base compartida. **Quien repita esta
  palanca en otra unidad tiene que usar el símbolo propio de cada cadena**, no un offset
  sobre una base común, y comprobar `fncmp` DESPUÉS — la reducción de `.rodata` sale igual
  de bien en los dos casos y `dupstr` no ve la regresión.
* **NEGATIVO 2 (zMisc)**: `deadstr.py zMisc` propone hoy **17 cadenas / 184 B** (eran 37/512
  antes de mi cambio) y **el signo está al revés**: la unidad ya emite de más
  (`rodata +1048`), así que meter esas 17 en `keep.lst` la aleja. Confirmado el aviso del
  encargo, con la cifra de hoy.
* **NEGATIVO 3 (`UpdateLoaded`)**: las tres formas de §2, con su cifra.
* **Regresiones medidas: NINGUNA.** `fncmp` 0/450 (zMisc), 1/1120 (zPhysicsBehaviors, la
  misma de siempre), `linkdelta` `.text +0` en zMisc/zPhysics/zGameplay y `+4` en
  zPhysicsBehaviors — idénticos antes y después. `gapchk`/`prefijochk`/`checksplits`/
  `mangfix` no aplican: no he tocado `splits.txt` ni ningún `asm()` de datos ni ningún
  alias.
* **El sello de zPhysicsBehaviors cambia** (`e4fceb82` → `729dc6bd`) **sólo por
  `.debug_line`**: el bloque de comentario que añadí delante de la función corre sus líneas.
  `.text`, `fncmp` y `linkdelta` idénticos, verificado.

---

## 4. Propuestas (no las aplico)

1. **Las 22 cadenas que quedan en zMisc (254 B) necesitan una guarda en cabecera
   compartida.** El patrón ya existe en el árbol (`ATTRIB_TAGS_HAND_POOL`,
   `SIM_PACKET_NAMES_HAND_POOL`, `HAND_POOL_TAG`). Las tres familias:
   `SoundConn.h` (`HeliSoundConn`, `Pkt_Heli_Open`, `CarSoundConn`, `Pkt_Car_Open` — 54 B),
   `GRaceDatabase.h` (`19.8.31`, `16.2.1`, `1.2.3` — 21 B) y las `SMS_MESSAGE_*` + `GManager`
   (88 B). **Toca a zWorld, zSim, zFe y zAnim a la vez**, así que es trabajo de una ventana
   con esos agentes presentes, no mío.
2. **`dupstr.py` debería sacar el offset y el símbolo del bloque a mano**, no sólo el
   nombre. Con eso la palanca de este informe es mecánica en cualquier unidad que escriba su
   pool a mano (zAI, zAnim, zCamera, zFe, zFe2, zPhysics, zPhysicsBehaviors, zFoundation
   todas declaran `_bwarePrefix`). Mi sonda `scratchpad/phys61/bases.py` lo hace en 2 s:
   lee la tabla de símbolos `.rodata` del `.o` y busca el símbolo con offset exacto.
3. **`dolwhere.py` debería decir CUÁNTO desplazamiento hay** cuando las secciones no
   coinciden, en vez de sólo `LAS SECCIONES NO COINCIDEN`. En zPhysicsBehaviors la respuesta
   («8 B, mismo tamaño») es exactamente el diagnóstico, y hay que sacarla a mano de las dos
   direcciones que imprime.
4. **`fncmp` es el control obligatorio de cualquier palanca de `.rodata`.** `dupstr`,
   `rodorden` y `linkdelta .rodata` dieron todos «mejor» en la versión que rompía nueve
   funciones; la única que lo vio fue `fncmp`, y `linkdelta` sólo lo delató en la columna
   `.text`.

---

## 5. Sorpresas

1. **Una macro que sólo cambia el operando de un `lis/addi` puede acortar la función.**
   `(_bwarePrefix + a)` y `(_bwarePrefix + b)` comparten el `@ha` y GCC funde; dos `$LC`
   distintos no. Cambiar un literal por un puntero **no es** una operación neutra de código.
2. **El bloque a mano de zMisc ya tenía nombre para 91 de las 150 cadenas.** El trabajo de
   rondas anteriores dejó los `.globl` puestos y nadie los había usado desde C.
3. **`dolwhere` no puede medir zPhysicsBehaviors**, y esa incapacidad es el mejor
   diagnóstico que ha dado: 8 B de desplazamiento con tamaño idéntico.
4. **`cse2` sí conserva UNA de las cinco copias de PRE en `UpdateLoaded`.** La r54 lo contó
   como cinco rematerializaciones; son cuatro. La diferencia importa: no faltan cinco usos,
   falta uno.
5. **`build_direct.py Speed/Indep/SourceLists/zMisc` alterna entre `2 ok, 0 fallidas` y
   `1 ok, 1 fallidas`** en la misma ventana sin que cambie nada mío: el que falla es
   `zMiscSmall`, que otro agente tiene abierto. El `.o` de zMisc sale bien las dos veces
   (sello idéntico), pero el resumen asusta.
