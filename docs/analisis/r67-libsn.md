# r67 -- lote `libsn`: andamios de LibSN y del runtime SN

Encargo: dejar fuente legitima. Ficheros en exclusiva: `src/LibSN/metrotrk.c`,
`sndvd.c`, `FSasync.c`, `crt2D1.c`, `ppc2D2.c`. Test: sello por seccion de las ALLOC
(`.text`, `.rodata`, `.data`, `.bss` por tamano y sus `.rela`) mas la tabla de
simbolos restringida a ALLOC, compilando a `.o` PRIVADOS (nunca `build/GOWE69`).
Para `metrotrk.c`, que no se compila, el test es el oraculo funcion a funcion contra
`build/GOWE69/src/LibSN/metrotrk.o` (ensamblado del `.s` = bytes del original).

**Veredicto: 19 -> 16 andamios (3 retirados), 0 regresiones, 4/4 unidades con el sello
IDENTICO al `.o` committeado y `metrotrk.c` con el oraculo igual (7/8 funciones
identicas, como antes).** Los 16 que quedan llevan diagnostico r67 junto al andamio.

## 0. Resumen

| fichero | antes | despues | que ha pasado |
|---|---|---|---|
| `crt2D1.c` | 1 pin | **0** | es libgcc2 literal (`DO_GLOBAL_CTORS_BODY`) |
| `FSasync.c` | 1 pin + 1 barrera | 1 pin | barrera `16<<8` fuera: la constante nace en OTRO bloque basico |
| `metrotrk.c` | 12 pines | 11 | `fn_803104B4`: una variable, la copia del `return` la lleva a r3 |
| `ppc2D2.c` | 1 barrera | 1 | encontrada la fuente (cast a secas); sigue sin salir el `addi` |
| `sndvd.c` | 1 pin + 2 barreras | 1 + 2 | mecanismo del cross-jump leido en jump.c; sin forma de C |
| **total** | **19** | **16** | |

(Cuenta del encargo. Con la regex que tambien ve la grafia corta `asm(""`: metrotrk
13 -> 12 por su barrera de `fn_80311A28`, y FSasync 4 -> 3 por las dos de
`EXI2TCHandler`, que el encargo no cuenta.)

## 1. Metodo (`scratchpad/libsn67/`)

Herramientas de la r66 copiadas y reapuntadas (`cc.py`, `baseline.py`, `formsweep.py`,
`apply_tree.py`, `metrotrk_oracle.py`, `sweep.py`), mas:
* `keepvar.py` -- compila una variante, conserva el `.o`, lo desensambla (`KV_EXTRA`
  anade flags).
* `mtrk_forms.py`, `apply_mtrk.py` -- formas de `metrotrk.c` contra el oraculo; la
  segunda aplica al arbol y restaura si CUALQUIER funcion cambia de estado.
* `probe_gpr.c` -- sonda de la regla de reparto GPR (sec. 3).
* `annotate67.py` -- escribe las notas r67 respetando el final de linea de cada fichero.

**Al empezar**: `baseline.py` 4/4 IGUAL-AL-ARBOL; oraculo de metrotrk 7/8 identicas.
**Controles que fallan**: en cada spec va el andamio quitado a secas y cambia siempre
(crt2D1 `3fee1dbf48bb25a4`, FSasync `447b6814b9705ade`, ppc2D2 `be73ca08da03a5b3`,
sndvd `681f464e` / `0763fc3e` / `16cbc252`; metrotrk `fn_803104B4` 3 insn distintas).

## 2. Retirados

### 2.1 `crt2D1.c` `__do_global_ctors`: el oraculo es PUBLICO

El cuerpo del objetivo es exactamente `DO_GLOBAL_CTORS_BODY`
(`orig/prodg/NGC_GNU_SRC/NGC/gcc/gbl-ctors.h:103`) dentro de `__do_global_ctors`
(`libgcc2.c:2905`, con el `ON_EXIT` comentado por SN, que es por lo que no hay `atexit`).
El bucle `for (i = nptrs; i >= 1; i--) __CTOR_LIST__[i] ();` con `unsigned i` es lo que
loop.c reduce a `r31 = LIST + 4*n` contra `r30 = LIST`; el `mr. r0,r11` es el `i >= 1`
sin signo. La reconstruccion anterior (`do/while` con puntero y `remaining asm("r0")`)
necesitaba el pin; la del original no.

| forma | sello ALLOC / SYM |
|---|---|
| base (con pin) | `2b1667fc04134f98` / `d879e769d4b0ad9e` |
| base sin pin | `3fee1dbf48bb25a4` / igual |
| libgcc2 con `do { } while (0)` (la macro tal cual) | **`2b1667fc04134f98` / `d879e769d4b0ad9e`** |
| libgcc2 sin el `do/while` | igual |

Aplicada la de la macro. `__main` no se toca (su `__asm__("initialized.10_804B5550")` es
enlace de nombre).

### 2.2 `FSasync.c` `DoFSReadHeader`: la barrera del `16 << 8`

Doce formas barridas en rondas anteriores buscaban que el 16 "no fuera visible" con
tipos, globales, uniones o helpers. La condicion real es de **bloque basico**:

* declarada `size = 16` ARRIBA, antes del bucle de borrado, **cse1 olvida la
  equivalencia en la etiqueta de cabeza del bucle** (tiene dos predecesores);
* **gcse no puede propagar la constante dentro de un `ashift`**: `try_replace_reg` es
  `validate_replace_src` sin simplificar, y `(ashift (const_int 16) (const_int 8))` no
  es una insn reconocible;
* **combine no cruza bloques** (LOG_LINKS).

Sale el `li r0,16` + `slwi r0,r0,8` del objetivo. Sello `395b1828d28dc4ac` /
`dad1a80b0785ccef` IDENTICO en cuatro grafias (`unsigned short`, `unsigned int`, primera
declaracion, asignacion aparte antes del bucle). Quitar solo la barrera de la forma
anterior: `447b6814b9705ade`.

### 2.3 `metrotrk.c` `fn_803104B4` (`snIsSNTDEV`): pin r3

```c
int fn_803104B4(void) {
    unsigned int state;
    asm("lis %0,lbl_804AC6E0@h\nori %0,%0,lbl_804AC6E0@l" : "=r"(state));
    state = *(volatile unsigned int *)(state + 0x6D0);
    return state;
}
```
La misma variable lleva la direccion y luego el valor devuelto: la copia a r3 del
`return` es la sugerencia que usa el asignador. Oraculo: funcion IDENTICA (y las otras
siete sin cambio); con `int state` tambien. Pin quitado a secas: 3 insn distintas.

## 3. Regla nueva: reparto GPR de las salidas `"=r"` (hipotesis 2 del encargo)

Sonda `probe_gpr.c` con los cflags de libsn:

| caso | registro |
|---|---|
| una salida usada como base | **r9** |
| tres salidas de un mismo asm usadas como base | **r9, r10, r8** (r11 se lo lleva un temporal `li 1` del cuerpo) |
| salida usada solo como valor, temporal de un uso | **r0** |
| salida base con tres parametros vivos en r3..r5 | **r9** (no la desplazan) |
| salida base con `"=&r"` y parametro vivo | r9 (la `&` no cambia nada) |
| salida DEVUELTA | **r3** (`lis r3` directo) |

Es REG_ALLOC_ORDER de rs6000 (`rs6000.h:932`: `0, 9, 11, 10, 8, 7, 6, 5, 4, 3`) sin r0
cuando la salida es base, repartido por prioridad con los temporales del cuerpo.
**rs6000 no tiene letra de clase para un GPR suelto** (`REG_CLASS_FROM_LETTER`,
`rs6000.h:1260`: `f b h q c l x y z`), asi que un operando nunca puede pedir r4, r5 o r6;
la unica via medida hacia un registro de argumento es la copia del `return` a r3.

Contraste con la regla de FPR (UVectorMath): alli las salidas se reparten hacia atras
desde f13 porque SON muchas y vivas a la vez; en GPR una salida de base cae siempre en
la cabeza del orden.

**Hipotesis 1 del encargo, medida**: ningun pin de metrotrk esta sobre un parametro.
`value` de `fn_80311A10` es una COPIA de `flags`; usar el parametro directamente mete
`mr r0,r3` (24 -> 28 B). En `fn_80311A28` igual aun quitando la barrera (28 -> 32 B),
con `flags = ~flags;` y con `current &= (flags = ~flags);`.

## 4. Irreducibles (16), diagnostico junto al andamio

| fichero | andamio | diagnostico r67 |
|---|---|---|
| `metrotrk.c` | 11 pines + barrera de `fn_80311A28` | registros elegidos a mano; ninguno es parametro; los 11 estan en funciones `void` o que devuelven otro valor, donde la copia del `return` no aplica. Fichero muerto. |
| `ppc2D2.c` | `__asm__("" : "+r"(lim))` | **La fuente era `return (unsigned int)value;`**: la forma del objetivo es `optabs.c expand_fix` para `unsigned` sin patron `fixuns` en PowerPC (compara con 2^31, rama, `fsub`, `fctiwz`, `xoris 0x8000`), y `lbl_8040F1B8` es su constante de pool (0x8040F1B8..F1C0 no tiene dueno en splits; sndvd empieza en F1C0). Medido: mismo flujo, 68 B contra 72, porque GCC pliega el `@l` en el `lfd` (lo_sum DFmode legitimo con TARGET_HARD_FLOAT, `rs6000.h:2155`), y ademas `.rodata` propia de 8 B. `-fforce-addr` y `-fforce-addr -fforce-mem` sobre el cast: objeto identico al cast sin flags (`757f6af1856e73dc`). |
| `FSasync.c` | pin `block_high` r30 | global_alloc entre preservados con prioridades DISTINTAS (7 refs puestas por el `FS_BLOCK_*` contra 5), no empate; se va con el `FS_BLOCK_*`, que es de splits. Sin intento nuevo. |
| `sndvd.c` | pin `savedContext` r29 | prioridades distintas en global_alloc (r65); el orden de declaracion solo desempata. Sin intento nuevo. |
| `sndvd.c` | `__asm__ ("")` del default | **mecanismo leido en jump.c**: para dos saltos a la misma etiqueta `find_cross_jump` exige 2 insns iguales (`jump.c:2015`), pero baja el minimo a 1 si el recorrido topa con una CODE_LABEL (`jump.c:2891`). El rabo de `case 0xA800` tiene esa etiqueta delante del `bl ForceDvdDeIrq` (0x803129FC, destino del `bne` tras `CheckSeekOffset`), y un solo `bl` igual basta. En el original las dos CALL_INSN diferian (patron o `CALL_INSN_FUNCTION_USAGE`, `jump.c:2920`) o los `b` iban a etiquetas distintas; la ASM_INPUT (`jump.c:2968`) es lo que usa la barrera. |
| `sndvd.c` | `__asm__("" : : : "memory")` | adelanto de sched2 dentro de un bloque basico; sin intento nuevo. |

## 5. Reglas nuevas medidas

1. **Una constante que GCC pliega se deja sin plegar poniendola en OTRO bloque basico
   que su uso, si el uso no admite la constante en sitio** (un `ashift`, y en general
   cualquier operando que exija registro): cse1 la pierde en la etiqueta, gcse no
   simplifica al sustituir y combine no cruza bloques. Sustituye a la barrera selectiva
   que se usaba para "que la constante no sea visible".
2. **Reparto GPR de salidas `"=r"`** (sec. 3): base -> r9 (cabeza de REG_ALLOC_ORDER
   sin r0), valor de un uso -> r0, devuelta -> r3; ninguna letra alcanza r4-r6.
3. **El runtime SN es libgcc publico**: `crt2D1` es `libgcc2.c` literal y `ppc2D2` es
   la expansion de `expand_fix` de un cast. Antes de reconstruir una funcion de
   runtime, buscar su texto en `orig/prodg/NGC_GNU_SRC/NGC/gcc/` y en lo que GENERA
   el propio compilador (optabs), no solo en los `.c`.
4. **Cross-jumping de GCC 2.95, la asimetria**: el minimo de 2 baja a 1 cuando el rabo
   del salto examinado empieza en una etiqueta; si el objetivo NO funde dos rabos
   iguales con etiqueta delante, lo que difiere es la llamada o la etiqueta de salto.

## 6. Propuestas (no hechas)

* `git rm src/LibSN/metrotrk.c` -- retira 11 pines y 1 barrera sin tocar ningun arco
  de build (`build.ninja` ensambla `src/LibSN/metrotrk.s`; `objdiff.json` apunta al
  `.s`). Tercera ronda que se propone.
* Ninguna de config/flags. Para `ppc2D2` NO se propone darle `.rodata
  start:0x8040F1B8 end:0x8040F1C0` en splits: el `.text` del cast sigue 4 B corto, y
  asignar un rango huerfano mueve el enlace (ver nota "el rango no basta").

## 7. Verificacion final

* `baseline.py`, arbol final a `.o` privados contra el `.o` committeado:
  crt2D1 `2b1667fc04134f98` / `d879e769d4b0ad9e`, ppc2D2 `5dd104c13671c5af` /
  `75a2c2dca504e957`, FSasync `395b1828d28dc4ac` / `dad1a80b0785ccef`, sndvd
  `934fe3a63686312b` / `968a18b8211eb503` -- **4/4 IGUAL-AL-ARBOL**, los mismos
  sellos que al empezar. Por seccion: crt2D1 `.text cd1c1a0cff31:200`,
  `.rela.text 70435584c76c:180`; FSasync `.text 34e92584327a:2104`,
  `.rela.text baa35212f76f:2988`; el resto de ALLOC vacias en las dos.
* `metrotrk_oracle.py` sobre el fichero final: 7/8 identicas (fn_80310F94 con sus 5
  insn de prologo, sin pines), 11 pines + 1 barrera y cada uno sigue siendo necesario.
* UTF-8 valido en los cinco; finales de linea de la copia de trabajo conservados
  (metrotrk, FSasync y ppc2D2 CRLF; sndvd y crt2D1 LF). Los 4 bytes no ASCII de
  metrotrk son las comillas angulares que ya estaban en HEAD.
* `git diff --stat src/LibSN/`: 5 ficheros, +100 / -32.
* No he tocado `configure.py`, `config/GOWE69/*` ni cabeceras compartidas; no he corrido
  `lcfix.py` ni commiteado. ALLOC identicas en todo lo compilado: el DOL no puede
  moverse; el reenlace desde el arco de `main.elf` queda para el jefe.
