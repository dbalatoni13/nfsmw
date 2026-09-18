# Ronda 20 — `realcore`, `LibSN/vmbase` y el driver de depuración

Línea base `base_r20_core.json`: 11.572/13.968 B, 84 funciones al 100 %.
Cierre `despues_r20_core.json`: **12.076/13.968 B (86,45 %), 86 funciones**.
**+504 B, +2 funciones, cero regresiones** (`measure.py --cmp`).

Toda medida lleva `build_direct.py` delante. Objetos comparados con
`objdiff-cli` y `function_reloc_diffs=none`, `ppc.calculatePoolRelocations=false`.

---

## 0. Lo primero: el encargo venía con dos cifras muertas

| unidad | el encargo decía | lo que mide el árbol |
|---|---|---|
| `system/cmn/systask` | 55,9 %, faltan 224 B | **100 %** (508/508, 3/3 funciones). Nada que hacer |
| `system/cmn/exit` | 0 %, «escribir desde cero» | **90 %**: la función ya estaba escrita, era un near-miss de 108 B |

El frente real eran **seis funciones near-miss**, no unidades a cero:

| unidad | función | B | antes | después |
|---|---|---:|---:|---|
| DebuggerDriver | `AmcEXISetExiCallback` | 108 | 85,19 % | — |
| DebuggerDriver | `EXI2_WriteN` | 432 | 94,44 % | — |
| DebuggerDriver | `EXI2_Poll` | 264 | 99,29 % | — |
| dvd_device | `StartNonAlignedAyncRead` | 440 | 93,59 % | — |
| input/gc/gc_interface | `EnumerateDevices` | 396 | 87,56 % | **100 %** |
| system/gc/inittmr | `TIMER_init` | 268 | 97,02 % | — |
| system/cmn/exit | `SYSTEM_addexit` | 108 | 90,00 % | **100 %** |
| LibSN/vmbase | `__VMBASESetupExceptionHandlers` | 380 | 98,74 % | — |

---

## 1. El hallazgo de la ronda: **un bucle con `goto` no lleva notas de bucle**

Los cflags de todo `realcore` traen **`-fmove-all-movables`** (y `-O1`, que ya
activa `loop_optimize`). GCC 2.9 saca los invariantes del bucle a un
*preencabezado* — pero **sólo dentro de bucles que el front-end marcó** con
`NOTE_INSN_LOOP_BEG` / `LOOP_END`, es decir `for`, `while` y `do…while`.
**Un bucle hecho con `goto` hacia atrás no lleva esas notas: `loop_optimize` ni
lo ve, y el invariante se rematerializa dentro.**

Ese era el mecanismo de las **dos** funciones que han cerrado, y valió 504 B.
Se reconoce en el diff sin ambigüedad: **un `lis …@ha` nuestro colocado justo
antes de la cabecera del bucle** que en el objetivo aparece repetido dentro del
cuerpo, normalmente con registros callee-saved de más y un marco más grande.

---

## 2. `SYSTEM_addexit` — 108 B, 90 % → **100 %** (cerrada)

`.text:0x8038682C`, tamaño 0x6C, 27 instrucciones.
Censo de llamantes (`bl` directas sobre el `.text` del ELF original): **3** —
`TIMER_init__Fi` (0x8038616C), `FILE_init__FPvi` (0x80382AA0) y 0x803660AC.
Llamados: **ninguno**. Referencias: sólo `exitfunctions` (HA/LO, addend 0).
Sin literales, sin datos nuevos.

| ensayo | forma | medida |
|---|---|---|
| — | estado de partida (`for (;;)` + `*(exitfunctions + j)`) | 90,0000 % |
| **c1** | el `for (;;)` pasa a etiqueta + `goto again` | **99,2593 %** |
| **c2** | además `*(exitfunctions + j)` → `exitfunctions[j]` | **100 %** ✅ |
| c3 | c2 sin el pin `register int j __asm__("r11")` | 98,5185 % — revertido |
| c4 | c3 con `int j` declarada antes del primer bucle | 91,2963 % — revertido |

- **c1** es el mecanismo del apartado 1: con `for(;;)` GCC sacaba
  `lis r8, exitfunctions@ha` al preencabezado; con `goto` lo rematerializa
  dentro, como el objetivo.
- **c2**: `*(exitfunctions + j)` da `lwzx r0, r10, r9` (índice primero) porque
  `fold` de GCC manda las constantes al segundo operando y `&exitfunctions` es
  `TREE_CONSTANT`; `exitfunctions[j]` da `lwzx r0, r9, r10` (base primero),
  que es lo del objetivo. Misma diferencia en el `stwx`.
- **c3/c4**: el pin de `j` a r11 **ya estaba en el árbol** antes de esta ronda.
  Sin él GCC intercambia r10/r11 entre el contador y el índice desplazado. Se
  intentó quitarlo por forma de fuente (c4) y salió peor; se conserva tal cual.

`audit.py`: ok, 108 B, 3 ramas, 10 reubicaciones, 0 literales.
`frozen.py cong`: `3f592d4f255aff8b`.
**`trypromo.py`: DOL OK** — esta unidad se puede promocionar.

---

## 3. `EnumerateDevices` — 396 B, 87,56 % → **100 %** (cerrada)

`.text:0x8039A23C`, tamaño 0x18C, 101 instrucciones.
Censo de llamantes: **1** — el constructor
`GcInterface::GcInterface(const ConfigOptions&)` en 0x8039A114.
Llamados y contrato:

| llamado | sitios | contrato |
|---|---|---|
| `RealInput::AllocateMemSize(const char*,int,int,int,int)` | 4 | r3=0, r4=0x154, r5=0, r6=0, r7=0. Es el `operator new` de `GcPad` |
| `GcPad::GcPad(GcInterface*, unsigned, volatile PADStatus*)` | 4 | r3=objeto, r4=`this`, r5=índice, r6=`&gPadstat[i]` |
| callback de enumeración (indirecto, `blrl`) | 1 | r3=Device*, r4=userdata, r5=interface |
| `Device::~Device` (indirecto, `blrl`) | 1 | vtable+8/+0xc |

| ensayo | forma | medida |
|---|---|---|
| — | estado de partida (`do { … } while (dvindex <= 3)`) | 87,5556 % |
| **g1** | el `do…while` pasa a etiqueta + `goto again` | **96,9697 %** |
| **g2** | además `gPadstat + dvindex` → `gPadstat + 0/1/2/3` literal en cada `case` | **100 %** ✅ |

- **g1**: con `do…while`, `-fmove-all-movables` sacaba
  `lis r27, gPadstat@ha` + `addi r28, r27, gPadstat@l` al preencabezado y
  **eso obligaba a salvar r27 y r28**: `stmw r27` en vez de `stmw r29` y marco
  0x20 en vez de 0x18. Con `goto` desaparecen los dos registros salvados, el
  marco vuelve a 0x18 y la dirección se rematerializa en cada `case`.
- **g2**: con `dvindex` variable, el desplazamiento salía como
  `lis @ha; addi @l; addi r6,r6,0xc` (tres instrucciones). Con el índice
  literal, la constante entra en el **addend de la reubicación**:
  `lis r6, gPadstat+0xc@ha; addi r6, r6, gPadstat+0xc@l` (dos), que es el
  objetivo. El orden de los `li` de argumentos se corrigió solo.

`audit.py`: ok, 396 B, 16 ramas, 29 reubicaciones, 0 literales (y las otras
cuatro funciones de la unidad siguen ok).
`frozen.py cong`: `859533207b166549`.
**`trypromo.py`: DOL ROTO (`16dcd648aa71`)** — al 100 % pero **no se puede
promocionar**; queda como las cinco de libm.

---

## 4. `TIMER_init` — 268 B, 97,02 % (no cierra). Veda

`.text:0x8038607C`, 67 instrucciones. Una sola diferencia, y es de
**planificación**, en las dos últimas sentencias del cuerpo:

    objetivo:  li r0,1 · lis r3,TIMER_restore@ha · addi r3,r3,@l · stb r0,bIsTimerInited@sda21 · bl
    nuestro:   li r0,1 · lis r3,TIMER_restore@ha · stb r0,…                · addi r3,r3,@l    · bl

El `addi` y el `stb` empatan en prioridad en la lista de listos de
`haifa-sched` (los dos a un paso del `bl`); el desempate es **INSN_LUID**, o
sea el orden en que se emitió el RTL. Para que gane el `addi`, la
materialización de la dirección tiene que estar **antes** del `stb` en el RTL.

DWARF (`libdwarf.py cu inittmr.cpp`): `TIMER_init(int hz /* r27 */)` con
locales `long long now /* r27 */`, `long long Period /* r29 */`, `int tmp /* r9 */`.
Nuestra fuente ya casa uno a uno con esos tres.

| ensayo | forma | medida |
|---|---|---|
| **i1** | `void (*restorefunc)() = TIMER_restore;` antes de `bIsTimerInited = 1;` | 97,0149 % — **sin ningún cambio de instrucción**. Revertido |

**Veda:** barrida la sentencia `bIsTimerInited = 1; SYSTEM_addexit(TIMER_restore);`
en la forma «temporal local con la dirección». No sirve porque *cse* propaga la
dirección constante al montaje del argumento y borra el `lis/addi` original,
devolviéndolos detrás del `stb`. Haría falta una forma en la que la dirección
**no** sea propagable (usarla dos veces, o materializarla en memoria), y ninguna
de ésas se puede escribir sin cambiar el código emitido.

**No probado:** cambiar el orden de las sentencias (imposible: el `stb` no puede
cruzar el `bl` en ninguna de las dos direcciones), ni pinchar r0.

---

## 5. `StartNonAlignedAyncRead` — 440 B, 93,59 % (no cierra). Veda

Muro del asignador, ya documentado en `ai_dvd_round5_*` y
`ai_dvd_round7_notes.md`. `triage.py`: `faltan 15, sobran 9 … mr+6` — al
objetivo le sobran exactamente **seis `mr`**: `mr r28,r3`, `mr r7,r4` y cuatro
`mr r3,r28` delante de cada `DVDReadAsyncPrio`. Emitimos 416 B contra 440.

**Aportación nueva de esta ronda: el mapa de registros del DWARF.**
`libdwarf.py cu dvd_device.cpp` da la firma con registro por parámetro, que las
rondas 5 y 7 no citan:

    StartNonAlignedAyncRead(DVDFileInfo *FileInfo /*r28*/, void *MemPointer /*r7*/,
                            int FileBase /*r12*/, int Size /*r6*/)
        int readSize; // r5
        bloque anónimo { int sizealigned; // r5   rango 0x8038554C }

Es decir: el original mete `FileInfo` en un registro **callee-saved** y copia
`MemPointer` a r7 aunque r7 sea volátil. Y hay un **bloque anidado** con su
propia local en la rama `Size <= 31` (0x8038554C = instrucción 43 = primera de
esa rama).

| ensayo | forma | medida |
|---|---|---|
| — | partida | 93,5909 %, 416 B |
| **d1** | bloque anidado con `int sizealigned` en la rama `Size <= 31`, tal como el DWARF | 93,5909 % — **instrucción a instrucción idéntico**. Revertido |
| **dd1** | los cuatro parámetros pinchados a r28/r7/r12/r6 (declaración `register … __asm__`) | 95,1909 %, 432 B — pero mete r25/r26 y el marco pasa a 0x28. Revertido |
| **dd2** | sólo `FileInfo` → r28 | 95,1636 %, 424 B — un callee-saved de más (`stmw r26`). Revertido |
| **dd3/dd4/dd5** | `FileInfo`+`FileBase`, `FileInfo`+`Size`, los tres | 95,1636 % las tres. Revertidas |
| **dd6** | los cuatro pines **declarados detrás** del `if (Size == 0) { QEndOp(); return; }` | 89,5364 %. Revertido |

Diagnóstico: el pin de `FileInfo` a r28 no basta porque GCC **propaga la copia**
(`r28 = r3`) y sigue usando r3 en los cuatro montajes de argumento; para que
aparezcan los `mr r3,r28` hace falta que r3 esté ocupado, y en el objetivo lo
ocupa `PreBytes` (`subf r3,r30,r12`). Es circular. El pin de `MemPointer` a r7
—que es volátil— obliga a GCC a guardarlo en r25 porque su rango vivo empieza
antes del `bl QEndOp`; declararlo después (dd6) rompe la entrada.

**Veda:** barridas la declaración del bloque anidado del DWARF y **seis**
combinaciones de pines de parámetro. **No probado:** pinchar `PreBytes` a r3 a
la vez que `FileInfo` a r28 (la ronda 5 probó `preBytes/r3` **sola** y da
derrames); ni tocar los cflags.

---

## 6. `__VMBASESetupExceptionHandlers` — 380 B, 98,74 % (no cierra). Veda

`src/LibSN/vmbase.c` es **Metrowerks** (regla `mwcc`), no ProDG: aquí no aplica
nada de lo del apartado 1, y LibSN no tiene unidades DWARF. Diferencia única,
repetida cinco veces: el objetivo calcula **el valor que va a la memoria en r0**
y nosotros en r5/r6.

    objetivo:  oris r0,r6,0x4800  ·  stw r0,0x300(r5)
    nuestro:   oris r6,r6,0x4800  ·  stw r6,0x300(r5)
    objetivo:  neg r0,r5 · clrlwi r0,r0,6 · oris r0,r0,0x4800 · stw r0,0(r31)
    nuestro:   neg r5,r6 · clrlwi r5,r5,6 · oris r6,r5,0x4800 · stw r6,0(r31)

| ensayo | forma | medida |
|---|---|---|
| — | partida | 98,7368 % |
| **v1** | quitar la local `instruction` y meter la expresión en el `store` | 98,3158 % — la cadena pasa a un solo registro (bien) pero intercambia r5/r6 en la base. Revertido |
| **v2 (C)** | `register u32 instruction;` | 98,7368 % |
| **v3 (D)** | `register u32 original;` | 98,7368 % |
| **v4 (F)** | `instruction` declarada antes que `branch` | 98,7368 % |
| **v5 (G)** | `branch` sin `register` | 98,7368 % |
| **v6 (H)** | los cuatro destinos como `volatile u32 *` | 98,7368 % |
| **v7 (I)** | sin la local `branch` en los dos primeros bloques | 94,8947 % |

Las cinco variantes de declaración dan **exactamente la misma cifra**: el
asignador de MW no se mueve con eso. Coincide con lo que ya anotó
`libsn7_audio_resume_20260904_notes.md` («scoped expression versus local
result/register hints did not improve the initial 98.73684 %»).

**Veda:** barridas las sentencias `instruction = 0x48000000 | …;` y `*p = instruction;`
en siete formas (inline, `register`, orden de declaración, `volatile`, sin
`branch`). **No probado:** ninguna restricción de registro sobre r0 en MW, ni
`#pragma` de scheduling, ni cambio de versión de compilador.
**No he tocado nada de MetroTRK.**

---

## 7. `DebuggerDriver` — 804 B, no tocado (por encargo)

`AmcEXISetExiCallback` (108 B, 85,19 %), `EXI2_WriteN` (432 B, 94,44 %) y
`EXI2_Poll` (264 B, 99,29 %) siguen igual: las fases 7 y 8 los barrieron con
doce ensayos y el encargo dice no repetirlos. Sólo miré el más pequeño para
confirmar el diagnóstico ajeno, y lo confirmo:

    objetivo:  bl OSRestoreInterrupts · lwz r0,0x1c(r1) · mr r3,r31 · lwz r31 · lwz r30 · mtlr r0 · addi r1 · blr
    nuestro:   bl OSRestoreInterrupts · mr r3,r31 · lwz r0,0x1c(r1) · lwz r31 · lwz r30 · addi r1 · mtlr r0 · blr

Sitio del `mr r3,r31` dentro del epílogo y orden `mtlr`/`addi r1`. Es forma de
epílogo de MW, no forma de fuente — consistente con la nota de la fase 7 de que
GC/1.2.5 cierra ésta y `EXI2_WriteN` pero rompe otras tres.

---

## 8. Hallazgo aparte: `audit.py` falla en `EXI2_ReadN` y es de `splits.txt`

`audit.py` sobre `DebuggerDriver` da, **confirmado en dos pasadas con
recompilación en medio**:

    EXI2_ReadN   FALLA: reubicacion a otro simbolo: @28_8044F610 contra @226

`EXI2_ReadN` son 700 B que objdiff da al **100 %**. Comprobado a mano:

- en el objeto **objetivo**, `@28_8044F610` es **UNDEF** (shndx 0): la cadena
  vive fuera de la unidad;
- en el **nuestro**, `@226` es una cadena **local** de 25 B,
  `"Can't select EXI2 port!\n"`;
- en `orig/GOWE69/NFSMWRELEASE.ELF`, la dirección 0x8044F610 contiene esos
  mismos 25 bytes exactos;
- `config/GOWE69/symbols.txt:35835` la declara `@28 = .data:0x8044F610`, y
  `config/GOWE69/splits.txt:911` corta el `.data` de esta unidad **justo antes**:
  `start:0x8044F600 end:0x8044F610`. La cadena se la queda el auto-split
  `auto_06_8044F610_data.s`.

O sea: **no es un bug del código, es un límite de `splits.txt`**. El contenido
es idéntico, pero nuestra unidad emite su propia copia en vez de referenciar la
del original. No he tocado `splits.txt` (no entra en mi encargo); lo dejo
anotado porque afectaría a cualquier intento de promocionar `DebuggerDriver`.

---

## 9. Qué NO he probado en toda la ronda

- **Nada de `configure.py`**, ni de `splits.txt`, ni de cflags, ni de versión de
  compilador, ni `#pragma`.
- **Ningún cuerpo en ensamblador** y ninguna restricción de registro **nueva
  retenida**: el único pin del árbol es el `r11` de `exit.cpp`, que **ya estaba**
  antes de esta ronda (intentado quitar en c3/c4, sale peor).
- `permuter.py`: no lanzado en ninguna de las seis.
- `DebuggerDriver`: no he ensayado ninguna variante de fuente (encargo).
- `EXI2_Poll` y `EXI2_WriteN`: ni siquiera abiertos.
