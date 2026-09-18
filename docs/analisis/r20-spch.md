# Ronda 20 — familia `spch` (spchpick, spchbank, spchrule, spchsamp)

Encargo: 2.628 B pendientes en cuatro unidades de
`Speed/Indep/Libs/spch/dev/source/library/cmn/`. `build_direct.py` antes de
**cada** medida (`ninja` no construye ninguna de las cuatro).

    base_r20_spch.json     8456 / 11084 B   76,2901 %   55 funciones al 100 %
    despues_r20_spch.json  8456 / 11084 B   76,2901 %   55 funciones al 100 %
    measure.py --cmp       +0 B, +0 funciones, 0 unidades cambian

**Cero bytes.** Lo que sí sale de la ronda: **un fallo real de `audit.py`
corregido**, **la veda de `spchi.h` confirmada con una sexta forma**, **la
hipótesis de banderas muerta con cifra**, y **dos diagnósticos nuevos** que
reducen las ocho funciones abiertas a dos mecanismos.

`audit.py` da **rc=0 en las cuatro** (dos pasadas). Las cuatro quedan congeladas
con `frozen.py cong`.

Ficheros tocados: **dos**, ninguna cabecera, ninguna bandera, ni `configure.py`.

* `src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchbank.c` (2 líneas)
* `src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c` (forma, 0 B)

---

## 0. El censo: ocho funciones, 2.628 B

`triage.py --muro` sobre las cuatro:

    848 B  82,217 %  spchpick  iSPCH_ChooseSentence            faltan 2  mr+2
    524 B  98,321 %  spchpick  iSPCH_MakeSampleRequests        reorden local, dmax 2
    412 B  95,534 %  spchpick  iSPCH_ChooseSamples             reorden local, dmax 3
    248 B  32,903 %  spchpick  iSPCH_SentenceIsContextMatch    faltan 1  mr+1
    168 B  86,500 %  spchrule  iSPCH_GetSentenceRuleSettings   reorden local, dmax 5
    156 B  98,333 %  spchbank  iSPCHBank_GetSampleTimeInQueue  12 registros
    136 B  95,294 %  spchbank  iSPCHBank_AddToQueue            faltan 1  lbz+1
    136 B  91,765 %  spchsamp  iSPCH_GetSampleSizeData         reorden local, dmax 4

**Siete de las ocho tienen el tamaño exacto del objetivo y el multiconjunto de
mnemónicos exacto.** La única que no es `iSPCHBank_AddToQueue` (132 B contra
136 B, le falta un `lbz`).

---

## 1. Lo único que cambia: `iSPCH_InitBanks` pasaba `audit.py` mintiendo

`audit.py` en la primera pasada de la ronda dio **una FALLA** en las cuatro
unidades:

    iSPCH_InitBanks__Fv       FALLA: reubicacion a otro simbolo:
                              gNumBanks contra gUniqueBankHandle

Confirmada con segunda pasada. Es el caso exacto de
`docs/memoria/nfsmw-comparador-reubicaciones.md`: **cuatro `stw r0` idénticos
carácter a carácter, objdiff los da al 100 %, y dos van al símbolo equivocado.**

`lmap.py` da el orden del original sin ambigüedad — el orden de los `lis` sigue
al de la fuente:

    8037D26C  li r0, 0x0                        spchbank.c:12  spchbank.c:13
    8037D270  lis r11, gVoxBanks@ha
    8037D274  lis r10, gNumBanks@ha             spchbank.c:14
    8037D278  lis r8,  gBankCount@ha            spchbank.c:15
    8037D27C  lis r9,  gUniqueBankHandle@ha     spchbank.c:16

Es decir: `gVoxBanks` (13), `gNumBanks` (14), `gBankCount` (15),
`gUniqueBankHandle` (16). Nosotros teníamos `gUniqueBankHandle` en segundo
lugar. Movido al final: **`audit.py` ok, 40 B, 8 relocs**, y el porcentaje no se
mueve (seguía y sigue al 100 %). **0 bytes, un bug real menos.**

---

## 2. `spchi.h`: la veda aguanta una sexta forma, y ahora con prueba del DWARF

La r19 dejó `iSPCH_GetContextDataAddr` vedada con cuatro formas medidas
idénticas (`14.404/17.384 B`). **No repetí ese barrido**, pero el DWARF trae un
dato que la r19 no citó y que obligaba a una medida más:

    // Range: 0x80380418 -> 0x80380468
    inline unsigned char * iSPCH_GetContextDataAddr(struct VoxEvent * event) {
        // Local variables
        unsigned char * data; // r0
    }

**El original tiene UNA sola local (`data`). La nuestra tiene tres** (`data`,
`numBytes`, `offset`). Las cuatro formas de la r19 iban en la dirección
contraria: *partían* el `return` en más sentencias. La forma del DWARF —
colapsar las tres sentencias en el `return`— no se había medido.

    s0  spchi.h de HEAD (tres locales)          14.404 / 17.384 B  82,8578 %  101 fn
    s5  una sola local, todo en el return       14.404 / 17.384 B  82,8578 %  101 fn

**Idéntico byte a byte en las once unidades de `Libs/spch/dev`**, y
`iSPCH_SentenceIsContextMatch` sigue en 32,9032 % e `iSPCH_ChooseSentence` en
82,2170 %. Revertido; `spchi.h` queda intacta.

**Conclusión con cifra: GCC 2.95 pliega este accesor igual escrito de cinco
maneras. La veda de la r19 es correcta y ahora cubre también la forma que dicta
el DWARF.** No volver aquí.

---

## 3. Las banderas de `spch` son el óptimo — hipótesis muerta

`iSPCHBank_AddToQueue` y `iSPCHBank_GetSampleTimeInQueue` fallan **por la misma
razón**: nuestro compilador propaga un valor a través del rombo que genera
`BANKHDR_GetSampleRepeatAddr` (`if (hdr->parmFlags & 0x80)`) y el original no.
El bloque de unión tiene dos predecesores, así que `cse` no debería llegar: sólo
`gcse` puede. Eso apuntaba a bandera.

Barrido de un flag por compilación sobre `spchbank` entero (11 funciones,
1.820 B), midiendo la unidad completa:

    (base)                        1528/1820 B   AddToQueue 95,29 | TimeInQueue 98,33
    -fno-gcse                      900/1820 B   + FindInsertPosition 81,41  AddBank 90,31
    -fno-cse-follow-jumps         1376/1820 B   + FindBank 74,61
    -fno-cse-skip-blocks          1132/1820 B   + FindInsertPosition 72,17
    -fno-rerun-cse-after-loop      752/1820 B   + InitBankMem 86,67  AddBank 85,14
    -fno-expensive-optimizations  1260/1820 B   + GetStartSample 85,45

**Las cinco regresan, y ninguna arregla las dos funciones.** Las banderas
actuales de `cflags_spch` son el óptimo del eje CSE. **No es bandera.**

---

## 4. `iSPCHBank_AddToQueue` (136 B) — la única a la que le falta código

Es la única de las ocho con tamaño distinto (132 B contra 136 B). La diferencia
es **una instrucción, y es una recarga**:

    objetivo                          nuestro
    19 lbzx r11, r8, r7               lbzx r11, r10, r7
    20 add  r9,  r8, r7               add  r0,  r10, r7
    21 lbz  r0,  0x6(r3)   <-- FALTA  (nada)
    22 cmplw r11, r0                  cmplw r11, r6

El objetivo lee `hdr->sampleRepeat` **tres veces** (líneas 644, 654, 668 del
mapa de líneas); nosotros dos: la de 654 la sustituye por el registro de la
guarda de entrada. La tercera se recarga en los dos, porque el `stb` de
`queue[index+1]` mata la CSE.

Entre la primera lectura y la segunda **no hay ningún almacenamiento**: el
DWARF confirma que la función tiene exactamente dos locales (`queue`, `index`),
sin ninguna sentencia intermedia que pudiera matar la expresión.

Siete formas, ninguna añade la recarga (todas siguen en **132 B**):

    f1  `if (!hdr->sampleRepeat) return;`                    -> 95,2941 %
    f2  `if (hdr->sampleRepeat < 1) return;`                 -> 95,2941 %
    f3  `if (queue[0] < hdr->sampleRepeat)`                  -> 95,2941 %
    f4  `if (hdr->sampleRepeat > index)`                     -> 95,0000 %
    f5  `if (hdr->sampleRepeat <= index) index = 0;`         -> 94,8529 %
    f6  `if (++index >= hdr->sampleRepeat)`                  -> 95,2941 %
    f7  la guarda con `goto done` en vez de `return`         -> 95,2941 %

**Veda: barridas las tres sentencias del cuerpo (guarda de entrada,
comparación central, incremento+comparación final) en siete formas.** Lo que
falta no es forma de fuente: es que nuestro `gcse` propaga a través del rombo y
el del original no, y el §3 demuestra que tampoco es bandera.

---

## 5. `iSPCHBank_GetSampleTimeInQueue` (156 B) — el mismo mecanismo, otro síntoma

Tamaño exacto, multiconjunto exacto. **Una sola diferencia real**, en la guarda
del bucle:

    objetivo:  cmpw r9,  r8      (i < numInQueue,   i recién puesto a 0)
    nuestro:   cmpw r6, r10      (age < numInQueue, age también vale 0)

Es la misma sustitución de literal 0 por un registro que ya vale 0 que la r19
documentó en `iSPCH_SentenceLength` — pero **al revés**: aquí el objetivo *no*
la hace y nosotros sí, y otra vez cruzando el rombo de
`BANKHDR_GetSampleRepeatAddr`. Todo el resto (r8↔r10, r7↔r8) cae de ahí.

`lmap.py` da el orden de sentencias del original y **coincide con el nuestro**:
694 `age = 0`, 709 `numInQueue = hdr->sampleRepeat`, 710 la guarda, 711 el
accesor, 718 `testIndex = queue[0]`, 719 `queue = queue + 1`, 722 `i = 0` + la
condición, 725/726 el decremento, 728 la comparación, 733/734, 736 el
incremento, 741 el `return`.

Ocho formas, ninguna mueve nada:

    k1  `for (i = 0; i < numInQueue;)` con el ++ en el cuerpo -> 98,3333 %
    k2  `for (i = 0; i < numInQueue; i++)`                    -> 98,3333 %
    k3  `numInQueue` antes de `age = 0`                       -> 98,3333 %
    k4  `age = 0` antes de `numInQueue` (= HEAD)              -> 98,3333 %
    k5  `while (numInQueue > i)`                              -> 95,2941 %  REGRESA
    k6  `i = 0` antes de `testIndex = queue[0]`               -> 98,3333 %
    k7  k3 + k1                                               -> 98,3333 %
    k8  `i = 0; if (i < n) do {...} while (i < n);`           -> 98,3333 %

**Veda: barrida la forma del bucle y el orden de las tres sentencias de
cabecera.** Mismo muro que §4.

---

## 6. `iSPCH_SentenceIsContextMatch` (248 B, 32,9 %) — abierto por primera vez,
   y es un muro de planificación, no de cabecera

El encargo decía que «huele a cabecera que miente». **No lo es, y está medido.**
El DWARF de la función casa con nuestra fuente **variable a variable, registro a
registro**:

    int i; // r6            int result; // r5       unsigned char * contextData; // r3
    struct SRule rule; // r1+0x8    int contextParmIndex;   (sin registro)
    int numContexts; // r12         int * sentenceContexts; // r4
    Labels: abort: // 0x803804E4

Los cinco registros son **los mismos que asigna nuestro compilador**. El árbol
de inlines (`VoxEvent_GetNumContexts`, `VoxSentence_GetContextFlagsAddr` →
`VoxSentence_GetNumPhrases`, `iSPCH_GetContextDataAddr` →
`iSPCH_GetSentenceRulesAddr` → `iSPCH_GetRuleDataAddr` + `VoxEvent_GetNumRules`
×2) también casa. `triage` dice `faltan 1, sobran 0`: **el multiconjunto es
exacto salvo un `mr`.**

El 32,9 % engaña: **las 41 instrucciones del bloque de direcciones son las
mismas, en otro orden**, y todo el desorden cuelga de **dos instrucciones**:

    objetivo:  lbz r8, 0x6(r3)   (numSentences)   luego  lbz r0, 0x7(r3)  (numRules)
    nuestro:   lbz r8, 0x7(r3)   (numRules)       luego  lbz r0, 0x6(r3)

El primer `lbz` se lleva r8 y el segundo r0 **en los dos**. Como el objetivo
mete `numRules` en r0 y r0 se reutiliza después para `data`, **necesita un
`mr r7, r0` que nosotros no necesitamos**: ése es el `mr` que falta. El resto
del bloque son consecuencias.

Las dos cargas son independientes y tienen la **misma longitud de camino
crítico** (7 en las dos direcciones), así que el planificador desempata por
LUID, es decir por orden de fuente — y nuestra fuente ya emite `numSentences`
primero (vía `iSPCH_GetRuleDataAddr`). El desempate lo rompe el planificador,
no la fuente.

Ensayos:

    c4  las dos asignaciones de dirección intercambiadas
        (`sentenceContexts` antes que `contextData`, que es el orden en
        que el DWARF lista los dos inlines)                  -> 32,5806 %  REGRESA
    c5  el `if` anidado convertido en uno solo con `&&`
        (el mapa de líneas pone las dos pruebas en la 2375)  -> 32,9032 %  idéntico
    c6  c5 sin c4                                            -> 32,9032 %  idéntico
    e1  las locales de `iSPCH_MakeValidParmsMask` en el
        orden del DWARF (que es el **inverso** del nuestro)  -> 32,9032 %  idéntico
    s5  §2, la forma del DWARF de GetContextDataAddr         -> 32,9032 %  idéntico

Todos revertidos. **`iSPCH_ChooseSentence` (848 B) tiene exactamente el mismo
mecanismo, dos veces** (dos expansiones del mismo accesor; en las dos el
objetivo carga 0x6 antes que 0x7 y en las dos nos falta el `mr`). **Los 1.096 B
de las dos funciones cuelgan de un único desempate del planificador.**

---

## 7. `iSPCH_ChooseSamples` (412 B) — el DWARF dice que `sampleTable` no existe

Leído `r19-libs4.md` §2.2 antes de tocar nada. La r19 dejó la función con una
local `sampleTable` añadida a mano para mantener vivo `bank + 0xC`, con tamaño
exacto, multiconjunto exacto y `argdiff = 0`, y **una** instrucción mal puesta.

**El DWARF del original no tiene ninguna `sampleTable`:**

    bank // r30      numSamples // r26   i // r29        numMatches // r27
    bankHandle       bankIndex // r9     sampleAge // r3 secondChoiceAge // r19
    secondChoice // r20   postMatchParms // r18   sampleSize // r24
    sampleData // r31     numParms // r9
    Labels: abort: // 0x8037F1E8

Trece locales, **ninguna para `bank + 0xC`**: el `addi r17, r30, 0xc` del
objetivo es un temporal del compilador que vive en un registro **salvado**
(`stmw r17` contra nuestro `stmw r18`) desde la precabecera del bucle hasta la
cola (línea 729).

Once ensayos más (los doce de la r19 no se repiten). Todos con `measure` de la
unidad entera, que no se mueve (5.804/7.836 B):

    HEAD (k3 de la r19)                                      -> 95,5340 %  412 B
    n1  `for (i = 0, sampleTable = bank + 0xC; ...)`          -> 96,1165 %  412 B
    n2  `for (sampleTable = bank + 0xC, i = 0; ...)`          -> 95,5340 %  412 B
    n3  `sampleTable = sampleData - 2;`                       -> 95,5340 %  412 B
    n4  `sampleTable` antes de `sampleData`                   -> 95,4757 %  412 B
    n5  la asignación DENTRO del bucle, primera sentencia     -> 97,8447 %  412 B  <- mejor
    q1  sin `sampleTable`; la cola en dos sentencias
        sobre `sampleData`                                    -> 98,0000 %  408 B
    q2  la asignación dentro del bucle, antes del `+=`        -> 97,7573 %  412 B
    q3  `i = 0; sampleTable = ...; while (...)`               -> 96,1165 %  412 B
    q4  igual que n5 pero con `while`                         -> 97,8447 %  412 B
    q5  la asignación al final del cuerpo del bucle           -> 97,7573 %  412 B
    q6  sin `sampleTable`; `bank + 0xC + sc*ss` en la cola     -> 97,9515 %  408 B

**`n5` es el estado más cercano medido**: con la asignación dentro del bucle,
GCC la saca él mismo por movimiento de invariantes y la coloca **una posición
después del `bge` de la guarda**; el objetivo la tiene **una posición antes**.
Es decir, ya no es una permutación dentro del bloque (lo que tenía HEAD), es el
lado de una rama.

**No lo he dejado en el árbol**: sube 2,3 pp y **cero bytes**, y para
conseguirlo hay que escribir una asignación dentro de un bucle cuyo valor sólo
se usa después del bucle — que no es lo que hace el original (el DWARF no tiene
la variable). Queda documentado y es el punto de partida para quien lo retome.

`lmap.py` sitúa `addi r17, r30, 0xc` en la línea **640**, la misma del `for`, y
`addi r31, r30, 0xe` en la 638: eso es lo que hizo pensar en la precabecera. La
atribución de una instrucción replanificada es la de la última nota de línea que
la precede en el orden final, así que **640 no prueba que la sentencia esté en
esa línea**; es exactamente la trampa que hay que tener en cuenta al leer
`lmap.py` sobre bloques replanificados.

---

## 8. `iSPCH_GetSampleSizeData` (136 B) y `iSPCH_GetSentenceRuleSettings` (168 B)

Las dos con tamaño exacto y multiconjunto exacto.

### `iSPCH_GetSampleSizeData` — sólo el registro de `result`

    objetivo:  li r0, 0x0 ... stw r8,(r5) ; stw r0,(r6) ; li r0,0x1 ; mr r3,r0
    nuestro:   li r9, 0x0 ... li r9,0x1 ; ... stw r0,(r6) ; stw r4,(r5) ; mr r3,r9

El objetivo pone `result` en **r0** (lo dice el DWARF: `int result; // r0`, y
`nextIndex` y `endOffset` también en r0), nosotros en r9. Con `result` en r0 el
`li r0, 0x1` **no puede** adelantarse, porque r0 está ocupado por `endOffset`
hasta el `stw`; con r9 sí. **El orden de los dos `stw` no es la causa** — es una
consecuencia:

    c0  HEAD                                                 -> 91,7647 %
    c1  `*sampleOffset` antes que `*dataBytes`               -> 85,7353 %  REGRESA
        (el `stw` de r5 se adelanta por delante del `subf`)

**Cambios aplicados, byte-neutros, con prueba del DWARF** (se quedan porque
corrigen la forma de la fuente, no porque muevan el porcentaje):

    c2  `goto abort` + etiqueta `abort:`  (el DWARF trae
        `Labels: abort: // 0x803814A0`, sobre el `mr r3,r0`) -> 91,7647 %  idéntico
    c3  `offset` declarada antes que `sampleSize`, que es
        el orden del DWARF                                   -> 91,7647 %  idéntico

### `iSPCH_GetSentenceRuleSettings` — orden de sentencias confirmado, sin efecto

`lmap.py` da el orden del original: 263 `*ioSettings = 0`, 264 `*ioFlags = 0`,
266 `bytesPerRule`, **267 `settingsSize`**, **271 `settingsAddr`**, 274 la
guarda. **Nosotros teníamos `settingsAddr` antes que `settingsSize`.**

    m1  `settingsSize` antes que `settingsAddr` (orden del
        mapa de líneas)                                      -> 86,5000 %  idéntico

**GCC reordena las dos sentencias independientes igual escritas en cualquier
orden**, así que el orden de fuente no es observable aquí. Revertido. El DWARF
confirma además las cinco locales y sus registros (`flagsAddr // r8`,
`settingsAddr // r3`, `i // r10`, `bytesPerRule // r12`, `settingsSize // r11`);
nosotros ponemos `bytesPerRule` en r7 y `settingsSize` en r10. Muro de
asignación.

---

## 9. Qué NO he probado

* **`permuter.py`**: ni un lanzamiento. `iSPCH_ChooseSamples` con `n5`
  (412 B exactos, multiconjunto exacto, una instrucción al otro lado de una
  rama) e `iSPCH_MakeSampleRequests` son los dos mejores candidatos que he visto
  para el permutador ciego; no lo lancé por la advertencia del brief.
* **Restricciones de registro**: ninguna. Los tres candidatos naturales
  (`iSPCH_SentenceIsContextMatch` con el `mr` que falta,
  `iSPCH_GetSampleSizeData` con `result` en r0, `iSPCH_GetSentenceRuleSettings`
  con `bytesPerRule` en r12) son de asignación, no de forma; la búsqueda de
  forma queda documentada arriba pero no las he aplicado.
* **`iSPCH_ChooseSentence` (848 B)**: **cero ensayos de forma propios**. El
  diagnóstico del §6 (mismo mecanismo, dos veces) sale de leer el diff y el
  DWARF, no de barrer formas. Es la pieza más grande que queda y la que menos
  se ha tocado — pero mientras el desempate del planificador de `0x6`/`0x7` no
  se rompa, no hay nada que barrer ahí que no sea lo mismo que ya falló en la
  de 248 B.
* **`iSPCH_MakeSampleRequests` (524 B)**: verificado contra el DWARF (locales,
  registros y **los dos bloques anónimos** con `sampleOffset`/`sampleBytes` y
  `dataOffset`/`bankBytes` casan exactamente con nuestra fuente) y no gasté
  ensayos: la r19 ya barrió seis formas y el diagnóstico sigue siendo el
  desempate de LUID entre `lwzx` y `add`.
* **Barrido de banderas más allá del eje CSE**: sólo medí las cinco de §3 y sólo
  sobre `spchbank`. No probé `-fno-schedule-insns2`, `-fno-move-all-movables` ni
  los ejes de `-O`.
* **La cadena completa**: ninguna de las cuatro unidades está enlazada
  (`linked False`), así que el DOL no puede cambiar; **no he ejecutado la
  construcción completa ni he verificado el DOL**.
* **No he ejecutado nada**: todas las afirmaciones son sobre bytes e
  instrucciones.
