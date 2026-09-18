# Ronda 26 — zEAXSound + zEagl4Anim

Encargo: `Initialize…DynamicLoader`, los muros de las dos unidades, la deuda de
la `j` y **la falsificación viva del pin `asm("r31")`**.

## 0. Línea base VERIFICADA

`build_direct.py zEagl4Anim zEAXSound` + `triage.py --muro`, medidos por mí:

    2352 B  93.88776%  zEagl4Anim  Initialize__…DynamicLoader        faltan 8, sobran 6, 11 SUST
    2152 B  91.81599%  zEAXSound   Play__14cSampleWarpper            reorden local, dmax 7
    1480 B  99.45946%  zEagl4Anim  EvalSQT__…FnStatelessQ            sobran 2 clrlwi
    1008 B  98.67063%  zEAXSound   SetupNextLoad                     2 filas
     896 B  98.16964%  zEAXSound   UpdateParams__16SFXCTL_MasterVolf 49 registros
     736 B  99.86413%  zEAXSound   QueueFileLoad                     5 registros
     720 B  97.36100%  zEagl4Anim  FindMatchTime                     4 filas
     716 B  99.80447%  zEAXSound   UpdateRPM__13SFXCTL_Enginef       4 registros
     488 B  98.85200%  zEagl4Anim  Eval__…FnRawLinearChannel         22 registros
     456 B  93.50877%  zEagl4Anim  EvalState__…FnRawStateChan        2 SUST
     384 B  99.68750%  zEAXSound   Play__13cStichWrapper             4 registros
     356 B  94.30337%  zEAXSound   startnextrequest                  4 filas
     320 B  94.48750%  zEAXSound   DataLoadCB                        4 filas
     268 B  96.76100%  zEAXSound   MsgPlayMiscSound                  2 filas
     168 B  90.23810%  zEAXSound   ResolveCurrentDataMemory          4 filas

**Coincide con el encargo instrucción a instrucción** (incluida la corrección de
la r25: `QueueFileLoad` mide 736 B).

`audit.py` al empezar: **zEagl4Anim 313 ok / 0 FALLA**, **zEAXSound 763 ok /
0 FALLA**. Ningún FALLA que confirmar con segunda pasada.

---

## 1. RESULTADO

| | antes | después |
|---|---|---|
| `Initialize__Q25EAGL413DynamicLoader…` (2.352 B) | 93,88776 % · 227 diffs · 586 ins | **98,87585 % · 28 diffs · 589 ins** (+4,988 pp) |
| `QueueFileLoad__14EAXAemsManager…` (736 B) | 99,86413 % · 5 diffs | **100 % — CERRADA** |
| `DataLoadCB__14EAXAemsManagerii` (320 B) | 94,48750 % | 94,48750 % (reescritura **neutra**, sólo fidelidad DWARF) |

`measure.py --cmp` (base y medida SEGUIDAS, `build_direct.py` antes de cada una):

    +736 B, +1 funciones, 1 unidades cambian
       +736 B  +1 fns  Speed/Indep/SourceLists/zEAXSound   144088 -> 144824

`pctsnap.py --cmp`: **EMPEORAN: ninguna · MEJORAN: 2** (+4,988 pp y +0,136 pp).

`audit.py` al cerrar: **zEagl4Anim 313 ok / 0 FALLA**, **zEAXSound 764 ok /
0 FALLA** (una más, la nueva). `QueueFileLoad` audita entera: **736 B, 22 ramas,
35 relocs, 0 literales**. `frozen.py`: las dos unidades **re-congeladas**
(`zEagl4Anim 85c5e05574673387`, `zEAXSound 6c622d4559c33b11`) porque son mías y
las he cambiado.

**Ficheros tocados, dos:** `src/Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.cpp`
y `src/Speed/Indep/Src/EAXSound/EAXAemsManager.cpp`. Ninguna cabecera. Ningún
ensamblador. Sin commit.

---

## 2. LA FALSIFICACIÓN DEL PIN r31: RESUELTA, y sin pin

### 2.1 Confirmada al insn, y es código incorrecto de verdad

El aviso de la r25 se confirma leyendo las filas 379-381 del diff:

    fila  OBJETIVO                    NUESTRO (base)
    379   lwz r0, 0x10(r30)           lwz r0, 0x10(r31)     ; sheader->sh_voffset
    380   stw r0, 0x14(r31)           stw r0, 0x14(r31)     ; pHP->symtab
    381   lwz r9, 0x14(r30)           mr  r9, r0            ; sheader->sh_size

En el objetivo `sheader` vive en **r30** y `pHP` en **r31**; en el nuestro
GCC metía **las dos en r31** (`add 31,8,11` en la fila 347 pisa `pHP`). El
`stw r0,0x14(r31)` escribía `pHP->symtab` **encima de `sheader->sh_size`**, y el
`mr r9,r0` de la 381 releía lo recién escrito. `mr 31,3 / or 31,7,30 /
add 31,29,8 / add 31,8,11 / lwz 31,156(1)`: **cinco escrituras a r31** contra
**una** del objetivo.

### 2.2 La causa real NO es sólo que r31 sea el frame pointer: es que `sheader` ganaba r31

`scripts/alloc.py` sobre el `.greg` de la variante sin pin lo dice al dígito:

    #   pseudo n_refs live_len pri     confl  reg   qué es
    31  89     64     190      20210   190    r31   sheader   <- gana r31
    49  85     45     522       4310   395    -     pHP       <- SPILLED

`sheader` es UN solo pseudo que cubre los dos bucles (64 refs / 190 insns) y su
prioridad es 5× la de `pHP`. Con el pin puesto, GCC respeta `pHP` en r31 sólo
donde lo ve vivo y **mete `sheader` encima**: de ahí el código incorrecto.

### 2.3 La forma de fuente que lo arregla — y NO cuesta 4,9 pp, GANA 5

`dwbody.py`/`regmap.py --ours` dan la pista: en el original **`sheader` está en
ámbito de función pero el bucle 1 no lo usa**. Escribiendo el primer bucle con
sintaxis de array (`reinterpret_cast<ELFSectionHeader *>(p)[i].campo`) y sin
tocar `sheader`, el pseudo del bucle 1 se separa, `local/global-alloc` le da un
volátil (r12, y en el objetivo r7) y **`pHP` gana r31 solo**:

    variante                                        %          ins   escrituras a r31
    base r25 (pin r31, INCORRECTO)                  93,88776   586   5 (pHP + sheader)
    sin pin, sin más cambios                        79,54762   602   13 (pHP a la pila)
    sin pin + `sheader` declarada dentro del bucle  94,55102   585   1
    pin `sheader asm("r30")`, pHP sin pin           94,68707   585   1
    pin `sheader asm("r29")`, pHP sin pin           96,26871   585   1
    **sin NINGÚN pin + bucle 1 con sintaxis array** **98,87585 589   1 (sólo `mr 31,3`)**

**Aplicada la última.** r31 se escribe **una sola vez en toda la función**, igual
que el objetivo, y el reparto es correcto. La decisión de proyecto que la r25
dejó abierta se cierra en la dirección buena: **no hay que pagar 4,9 pp por
quitar el pin; quitándolo bien se ganan 5.**

### 2.4 Y de paso el DWARF cuadra

`regmap.py --ours` antes y después:

    antes:  "NO es una permutacion: 1 local solo del original (h), pHP r31, sheader r31"
    ahora:  "MISMO conjunto de locales y mismo arbol de bloques; solo cambia el reparto"

Los tres cambios de fidelidad que lo consiguen son **neutros en código**
(medidos: 79,54762 los tres, idénticos a «sin pin» pelado):

* `HashPointer *pHP = new HashPointer(this);` — el DWARF del original expande
  `HashPointer::operator new` y `HashPointer::HashPointer(DynamicLoader*)`, que
  nosotros no teníamos; sustituye al `EAGL4Malloc` explícito y a
  `pHP->mpDynamicLoader = this;`.
* `HashPointer &h = *pHP;` — la local `struct HashPointer & h` que el original
  lista entre `pHP` y `sheader` y nosotros no teníamos.
* `int j;` en vez de `long unsigned int hv;`.

---

## 3. La deuda de la `j`: RESUELTA, con el volcado delante

`dwbody.py zEagl4Anim "DynamicLoader::Initialize" both`, bloque anónimo final:

    ORIGINAL                          NUESTRO (antes)
    struct ELF32_Sym * sym; // r4     struct ELF32_Sym * sym; // r30
    int j;                            unsigned long hv;

**El bloque del original tiene EXACTAMENTE dos locales**, y la segunda se llama
`j` y es `int`. No hay ninguna `hv` ni ningún contador extra: la `j` del original
**es nuestro `hv`** (el valor de `elfhash`, que además devuelve `h & 0xFF`, así
que `int` es correcto). O sea: el cambio de la r25 —reusar `i` como contador del
último bucle— **era fiel también al DWARF**, y la «deuda declarada» no existía.
Renombrado `hv` → `j` y cambiado el tipo a `int`: **medido idéntico** (93,88776 %
sobre la base vieja), o sea fidelidad gratis.

---

## 4. `QueueFileLoad` — CERRADA (736 B), y con ella se retira otro invento nuestro

`regmap.py` no encuentra la función en el volcado original **y ahora se sabe por
qué**: `symbols/mw_dwarfdump.nothpp` trae las declaraciones de clase
(`void SetupNextLoad();`, `void QueueFileLoad(stSndAssetQueue&, eBANK_SLOT_TYPE);`
en las líneas 474152 y 474182) pero **ningún `DW_TAG_subprogram` con cuerpo**
para ninguna de las dos. No es un problema de nombre ni de homónimos: son un
hueco del volcado. **`lmap.py` sí las tiene** (`EAXAemsManager.cpp:911` y
siguientes) y es la herramienta que hay que usar en ellas.

Y `lmap.py` es lo que la cierra. Las filas 70-74, que la r25 leyó como «empate de
`local-alloc`», llevan **líneas de cabecera**:

    800ACC14  lwz r11, 0x28(r8)     EAXAemsManager.h:75
    800ACC18  stw r11, 0x48(r1)
    800ACC1C  cmpw r11, r27         EAXAemsManager.h:111
    800ACC20  lwz r0, 0x2c(r8)      EAXAemsManager.h:75
    800ACC28  bne .L_800ACD00       EAXAemsManager.h:111
    800ACC2C  lwz r9, 0x4(r30)      EAXAemsManager.h:112

O sea **el original no escribía ese bucle en el `.cpp`: llamaba a un inline de la
cabecera.** Y ese inline ya está escrito en nuestro árbol:
`SndAssetQueue::DeleteRefToAsset(SndBase *pSfxObj)` (`EAXAemsManager.h`), que hace
literalmente lo mismo (recorre, compara `currequst.pThis`, `remove`, reinicia).

    q1  el bucle abierto -> `this->mWaitForResolve.DeleteRefToAsset(owner); return;`
        **100,00000 %  736 B  0 diffs**
    q2  igual pero conservando el `char framePad[16]` + `asm("")`   100 %  (idéntico)
    q3  el bucle abierto SIN el framePad (control)                   99,67391 %  38 diffs

Con la llamada, el `char framePad[16]; asm("" : : "m"(framePad[0]));` —**un
invento nuestro de una ronda anterior para falsear el tamaño de marco**— sobra:
**retirado**. El cambio son 19 líneas menos y 2 más.

---

## 5. `DataLoadCB` (320 B) — DWARF cuadrado gratis, y el diff localizado

`regmap.py` daba diferencias estructurales reales: al original le faltaban
nuestras `eSNDDATATYPE eDataType` y `stBankSlot *pBankSlot`, y le sobraban
`int Result` e `int nhandle` en el bloque anónimo. Reescrito conforme al volcado
(switch directamente sobre `gAEMSMgr.m_pCurLoadSDLP->AssetDescription.eDataType`,
bloque con `Result`/`nhandle`, sin `pBankSlot`): **el objeto no cambia**
(94,48750 %, 320 B, 12 diffs, idéntico) y `regmap` pasa a decir **«MISMO conjunto
de locales, mismo árbol de bloques y MISMO REPARTO»**. Aplicado por fidelidad.

**Lo que queda son 12 filas y son UNA sola cosa**: el objetivo abre el marco con
`stwu r1,-0x18` y `stmw r29` (tres salvados) y nosotros con `stwu r1,-0x10` y
`stmw r30` (dos). El registro de más es el `lis r29, StartBankLoadTicks@ha`, que
el objetivo **iza por encima del `bl bGetTicker__Fv`** y por eso necesita un
registro salvado; nosotros lo materializamos después de la llamada, en r9.
El cuerpo entero (filas 15-76) casa al 100 %.

Diagnóstico de banderas (sólo diagnóstico, nunca aplicadas): `-fno-schedule-insns`
92,425 · `-fno-schedule-insns2` 91,175 (304 B) · `-fno-gcse` 94,4875 (idéntico).
**Ninguna reproduce el izado**, así que no es el planificador ni PRE: queda por
saber quién mueve ese `elf_high`.

---

## 6. `Initialize`: lo que queda son 28 filas en tres grupos

### 6.1 Grupo A (6 filas): el orden de operandos del direccionamiento indexado

    fila 343  objetivo `add r30, r11, r10`   nuestro `add r30, r10, r11`
    filas 370/500/501/512/542  lo mismo en `lwzx`/`stwx`

r11/r5 es el índice (`i*0x28`, `i*16`) y r10/r4 la base (`p`, `pHP->symtab`): el
objetivo pone **el índice primero** y nosotros la base. En la fila 367
(`p[sheader->sh_link]`) **los dos ponen el índice primero**, así que no es una
regla general del compilador. **Veda: 8 formas** — `&p[i]`, `p + i`, `&i[p]`,
`(ELFSectionHeader*)((char*)p + i*sizeof(...))`, `&p[(unsigned)i]`, y las mismas
cuatro para `sym = &pHP->symtab[i]` — **todas dan el objeto idéntico**
(98,87585 %). No se decide por fuente.

### 6.2 Grupo B (15 filas): el árbol del `switch` — la veda de la r20/r21, ahora con el mecanismo exacto

La lista de nodos es **la misma** que la del objetivo (verificado: r21 = 0x70000006
y r22 = 0x70000005 en los dos, y los tres tests del subárbol derecho son
idénticos). Lo que cambia es **cuál de los dos subárboles se emite en línea**:

    OBJETIVO                       NUESTRO
    cmpwi 8 ; beq  -> break        cmpwi 8 ; beq  -> break
    cmplwi 8 ; ble -> IZQUIERDO    cmplwi 8 ; bgt -> DERECHO
    [derecho en línea]             [izquierdo en línea]
    [izquierdo al final]           [derecho al final]
      cmpwi 3 ; beq  STRTAB          cmpwi 3 ; beq  STRTAB
      cmplwi 3 ; bgt break           cmplwi 3 ; bgt break
      cmplwi 1 ; ble break           cmpwi 2 ; beq  SYMTAB
      (CAE en SYMTAB)                b break                <- la instrucción de más

`expand_end_case` mueve el árbol delante del cuerpo, así que **el que cae por el
final es el PRIMER `case` en orden de fuente**, que es `SHT_SYMTAB` en los dos.
El objetivo aprovecha esa caída (nodo [2] tratado con cota inferior,
`cmplwi 1 ; ble`) y por eso se ahorra el `b`; nosotros emitimos el izquierdo
primero y el nodo [2] como test de igualdad. En `stmt.c` eso es la rama
`node_is_bounded (node->left)` de `emit_case_nodes` contra el `else`.

**Veda nueva, 3 formas**: `case SHT_NULL ... SHT_PROGBITS: break;` explícito
(97,96088, PEOR), `default:` movido arriba (idéntico), `default:` eliminado
(idéntico). Ninguna cambia la rama.

### 6.3 Grupo C (7 filas): `t` / `nameLength` y `&s[nameLength] + 1`

`regmap` lo dice: el original asigna `t`→r29 y `nameLength`→r30, nosotros al
revés. La causa está localizada: en `t = type_separator + strlen(TYPE_SEPARATOR);`
`local-alloc` nos **ata** el destino a `type_separator` (r30, que muere ahí) y al
objetivo no; el objetivo reusa r29, que era `&s[2]` y murió tres insns antes.

**Veda, 6 formas**: `t = &type_separator[strlen(...)]` (idéntico),
`t = type_separator; t += strlen(...)` (idéntico),
`t = strlen(...) + type_separator` (idéntico), `int nameLength` en vez de
`unsigned int` (idéntico), `unsigned` a secas (idéntico), y las dos
reordenaciones de `*type_separator = 0` / `strcpy(typebuf,…)` respecto del
`t = …` (97,58334 y 98,17007, **peores**).
Y `strcpy(&s[nameLength] + 1, typebuf)` (la forma que el objetivo emite,
`add`+`addi` en vez de `addi`+`add`): **98,70238, peor** — confirma el negativo
`g2`/`h1` de la r25 también sobre la base nueva.

---

## 7. Los negativos medidos de esta ronda

* **`UpdateRPM` (716 B, 99,804 %)**: cuatro filas y son **un par de registros de
  coma flotante** — el objetivo carga `m_RPM_LFO` en **f0** y acumula en **f1**;
  nosotros cargamos en f11 y acumulamos en f0. **Veda, 7 formas** de
  `NormalRPM = VisualRPM + m_RPM_LFO + m_ComppressionRPM.GetValue() + m_RPM_LFO;`
  — partida en tres sentencias, con paréntesis explícitos, con `Cur_RPM` en vez
  de `VisualRPM`, con el LFO delante (99,77653, peor), con el segundo par entre
  paréntesis (98,37989, peor), partida en dos, y el orden de declaración de
  `NormalRPM`/`VisualRPM` intercambiado: **todas 99,80447 con las mismas 4 filas**
  salvo las dos peores. No se decide por fuente.
* **`ResolveCurrentDataMemory` (168 B, 90,238 %)**: seis filas y **sólo dos son
  reales**: el objetivo emite `lwz r4, 0x28(r9)` (el argumento) **antes** de
  `lis r3, TheTrackStreamer@ha` y nosotros al revés; las otras cuatro son
  destinos de rama que se desalinean por eso. **Veda, 7 formas**: temporal para
  el argumento (con y sin bloque), referencia local al objeto global,
  `(&TheTrackStreamer)->FreeUserMemory(...)`, `mgr->m_pCurLoadSDLP->pmem` en vez
  de `curLoad->pmem`, y un `void *pmem` izado al principio de la función
  (91,19048 pero **164 B**: el tamaño que miente). Ninguna invierte el orden.
* **`EvalState…FnRawStateChan` (456 B)**: `regmap` señala algo nuevo y real —
  nos **sobra** la local `int keyIdx` (r10, con comentario propio en el fuente
  que la justifica) y al original le falta el registro de `k` (**r4** en el
  original, sin localización en el nuestro). Pero quitar `keyIdx` **empeora**:
  usarla directamente como `mKeyIdx` da **89,86842 (460 B)** y reusar `i` como
  índice inicial da **83,41228 (452 B)**. Igual que el `scale` de
  `cStichWrapper::Play` en la r24: **la local de más está haciendo trabajo real.**
  Veda ampliada a **39 formas**.
* **`EvalSQT…FnStatelessQ`**: no la he tocado, tal como pedía el encargo.
* **`UpdateParams__16SFXCTL_MasterVolf`**: no la he tocado (negativo de la r25).

---

## 8. Herramientas dejadas en el scratchpad (prefijo `c26ae_`)

* **`c26ae_v.py <UNIT> <SYM> <variante.cpp>…`** — el caballo de batalla de la
  ronda, y **sirve para cualquier unidad**: saca los cflags exactos de
  `build.ninja`, y con `SL=<ruta rel del .cpp dentro de la SourceList>` compila
  **la SourceList entera** con ese fichero sustituido por una copia del
  scratchpad (23 s por variante; sin `SL` compila el .cpp suelto, 10 s).
  `EXTRA=…` añade banderas o `-I`. `DUMP=<tag>` vuelca las filas con diff y
  `ROWS=lo-hi` (con `DUMP`) vuelca **todas** las filas del rango.
  Verificado que reproduce el `.o` real al bit en las cuatro funciones medidas.
* **`c26ae_rows.py <json> <lo> <hi> [SYM]`** — imprime todas las filas de un
  rango de un volcado de objdiff, casen o no. Es lo que localizó el miscompile
  de r31 (las filas que casan son la mitad de la prueba).
* `c26ae/` — `dl0.cpp` (base), `v_*`/`x*`/`c*`/`d*`/`e*`/`f*`/`g*`/`k*`/`m*`/
  `n*`/`p*`/`s*` (los ~45 ensayos de `Initialize`), `FINAL_dlopen.cpp`,
  `dw_init.txt` (el `dwbody` both), `mkvar.py`.
* `c26eax/` — `a0/a1.cpp` (EAXAemsManager antes y después), `q1_dra.cpp` (el
  cierre), `y2_result.cpp` (DataLoadCB fiel), `w*`/`x*` (UpdateRPM y
  ResolveCurrentDataMemory), `s0.cpp`, `st0.cpp`, `e0.cpp`.
* `c26anim/` — `r0.cpp`, `ev1`, `ev3` (EvalState).
* `c26ae_{B,P,B2,P2}.json`, `c26ae_pct{B,P,B2,P2}.json`, `c26ae_aud{1,2,3}_*.txt`.
* Borrados todos los volcados JSON de objdiff > 5 MB y los `.o` intermedios:
  el scratchpad bajó de **1,5 GB a 477 MB** (quedan 16 GB libres).

### Trampas que han vuelto a morder

* **Finales de línea MIXTOS dentro del mismo fichero.** `SFXCTL_Engine.cpp`
  tiene `\r\n` en unas líneas y `\n` en otras: `'\r\n' in texto` da `True`,
  `texto.split('\r\n')` parte mal y **las variantes salen idénticas al original
  sin avisar** (cinco medidas tiradas). Lo correcto es
  `splitlines(True)` y respetar el terminador **de cada línea**.
* `regmap.py` **no reconstruye la caché de `dwbody`** hasta que cambia el tamaño
  o el mtime del `.o`; tras `build_direct.py` sí la refresca.
* Compilar un `.cpp` suelto no vale en estas dos unidades: `RawStateChan.cpp`
  incluye `"RawStateChan.h"` por ruta relativa (hace falta
  `-I src/Speed/Indep/Src/EAGL4Anim`) y `EAXAemsManager.cpp` depende de los
  `.cpp` que la SourceList incluye antes. Por eso `c26ae_v.py` trae `SL=`.

---

## 9. Qué NO he probado

* **`Initialize`**: no he tocado el árbol del `switch` más allá de las 3 formas
  de §6.2 (la veda de la r20/r21 sigue: 88 árboles y 512 listas de `case`); no
  he probado el permutador; no he intentado forzar el orden de operandos del
  §6.1 desde el backend. **No he mirado si el cross-jump de `ELFAddr` volvería
  con otra forma** — desapareció solo con el cambio aplicado.
* **`Play__14cSampleWarpper` (2.152 B)**: cero ensayos de fuente. Diagnóstico
  reproducido y afinado: el objetivo abre `stwu r1,-0x58` + `stmw r22` y
  nosotros `stwu r1,-0x48` + `stmw r23` — **un registro salvado más y 12 B más
  de locales**; la fila 250 enseña el `li r25, 0x0` del último argumento
  (`filter_HiPass`) que el objetivo mantiene vivo por las tres construcciones y
  nosotros rematerializamos. No he encontrado la forma de fuente que lo haga
  vivir. `regmap`: sólo `TempAz` r23→r25.
* **`SetupNextLoad` (1.008 B)**: 23 filas, y son **una rotación**
  (`this` r29→r27, contador r26→r31, índice r27→r28, temporal r28→r29) más un
  `lis gFastMem@ha` desplazado dos ranuras. Sin DWARF del original (§4). Cero
  ensayos.
* **`startnextrequest` (356 B)**: 22 filas, permutación pura r27↔r28 / r29↔r30
  más dos parejas replanificadas. Cero ensayos.
* **`Eval__…FnRawLinearChannel` (488 B)**: sigue sin diagnóstico. Está definida
  **dentro de la clase** (`FnRawLinearChannel.h:59`), así que probarla exige
  sombrear una cabecera compartida (la receta está en `c24ae_eax2.py`: `EXTRA`
  **delante** de `CFLAGS`); `c26ae_v.py` pone `EXTRA` detrás y **no sirve para
  sombrear**. `regmap` sigue emparejando el homónimo de 4 parámetros: hay que
  filtrar por `low_pc` antes de leer nada.
* **`FindMatchTime` (720 B)**: `regmap` dice **IDÉNTICO** (15 locales, mismo
  árbol, mismo reparto) → según el catálogo, permutador CIEGO. No lo he lanzado.
* **`MsgPlayMiscSound`, `Play__13cStichWrapper`, `UpdateParams`**: no tocadas
  (las tres con negativo previo medido en la r24/r25).
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas.
* **El DOL**: no lo he construido (va en la verificación de la tanda).
* **`configure.py`, `config/GOWE69/*`, `splits.txt`**: intactos. Ningún
  ensamblador escrito. Ningún `#if defined(__ANDROID__)` tocado. Ninguna
  cabecera modificada. **Ningún pin nuevo aplicado** (los de §2.3 quedan
  medidos, no puestos).
