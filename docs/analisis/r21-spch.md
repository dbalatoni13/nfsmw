# Ronda 21 — familia `spch` (spchpick, spchevnt, spchbank, spchrule, spchsamp)

Medida propia con `build_direct.py` antes de **cada** medida (`ninja` no
construye ninguna de las 11 unidades de `spch`).

    Las cinco del encargo
        antes:   11.828 / 14.808 B   79,8757 %   78 funciones al 100 %
        despues: 13.736 / 14.808 B   92,7607 %   84 funciones al 100 %

`spchi.h` es cabecera compartida, asi que la medida de control va sobre las
**11 unidades** de `dev/source/library/cmn` (las 9 que la incluyen mas `sptick`
y `sputil`, que no la incluyen y sirven de testigo):

    11 unidades  14.404 -> 16.312 / 17.384 B   101 -> 107 funciones al 100 %

    measure.py --cmp c21spch_antes9.json c21spch_despues.json
        +1.908 B, +6 funciones, 4 unidades cambian, **ninguna baja**
          +168 B  +1 fn  .../cmn/spchrule  1.076 -> 1.244   (100 %)
          +292 B  +2 fn  .../cmn/spchbank  1.528 -> 1.820   (100 %)
          +352 B  +1 fn  .../cmn/spchevnt  3.372 -> 3.724   (100 %)
        +1.096 B  +2 fn  .../cmn/spchpick  5.804 -> 6.900

**El encargo NO estaba caducado**: las cuatro firmas de `triage.py` del brief
coincidian exactamente con lo medido al empezar (848 B 82,217 %; 248 B 32,903 %;
352 B 86,818 %; 136 B 95,294 %), y las cinco de MURO tambien.

Cerradas: `iSPCH_ChooseEvent` (352 B), `iSPCHBank_AddToQueue` (136 B),
`iSPCHBank_GetSampleTimeInQueue` (156 B), `iSPCH_GetSentenceRuleSettings`
(168 B), `iSPCH_ChooseSentence` (848 B), `iSPCH_SentenceIsContextMatch` (248 B).
Mas `spchcsis` (360 B) que estaba al 100 % **mintiendo** (ver §5).

Ficheros tocados (todos dentro de `Libs/spch`, ninguna bandera, `configure.py`
sin tocar):

* `.../cmn/spchi.h`      (cabecera compartida — A/B sobre las 11 unidades)
* `.../cmn/spchevnt.c`
* `.../cmn/spchpick.c`
* `.../cmn/spchrule.c`
* `.../cmn/spchsamp.c`
* `.../cmn/spchcsis.cpp` (**fuera de mis cinco**: bug real de `audit.py`, §5)

`audit.py` da **cero FALLA en las 11 unidades** al terminar (dos pasadas).
Las **seis** unidades tocadas (`spchpick`, `spchevnt`, `spchbank`, `spchrule`,
`spchsamp`, `spchcsis`) quedan congeladas con `frozen.py cong` y `chk` responde
«identico al congelado» en las seis. Ojo: `spchpick` y `spchsamp` ya tenian
huella de una ronda anterior y **habia que refrescarla** (`chk` cantaba
«HA CAMBIADO»); `spchbank` y `spchrule` tambien.

---

## 1. El hallazgo de la ronda: `(x + 7) / 8` NO es `(x + 7) >> 3`

Los dos generan **el mismo `srawi`**. Pero `expand_divmod` construye la division
por potencia de dos con una forma de RTL distinta de la del desplazamiento, y
eso cambia el reparto de registros y la eliminacion de redundancias **de toda la
funcion que la contiene**.

En `spchi.h`, `iSPCH_GetContextDataAddr` calculaba los bytes por regla con
`>> 3`. Cambiarlo a `/ 8` (mas `data += ...; return data;`, §2) cerro **cuatro**
funciones en tres unidades:

| funcion | unidad | antes | despues |
|---|---|---:|---:|
| `iSPCH_ChooseEvent` (352 B) | spchevnt | 74,739 % † | **100 %** |
| `iSPCHBank_AddToQueue` (136 B) | spchbank | 95,294 % | **100 %** |
| `iSPCHBank_GetSampleTimeInQueue` (156 B) | spchbank | 98,333 % | **100 %** |
| `iSPCH_GetSentenceRuleSettings` (168 B) | spchrule | 86,500 % | **100 %** |

† tras quitar el ensamblador escrito a mano; ver §4.

Sitios cambiados, los tres medidos por separado:

    spchi.h  iSPCH_GetContextDataAddr   ((numRules + 7) >> 3)        -> / 8
    spchi.h  BANKHDR_GetSampleRepeatAddr ((numSamples + 7) >> 3) + 1 -> / 8
    spchrule.c iSPCH_GetSentenceRuleSettings (numRules + 7) >> 3     -> / 8

**Y no vale para cualquier desplazamiento.** Barridos y medidos, los tres
IDENTICOS o peores:

    k1  VoxSentence_GetNumPhrases   bitField >> 2      -> / 4    identico
    k2  VoxEvent_GetNumContexts     numRowContexts >>4 -> / 16   identico
    k3  k1 + k2                                                  identico
    k4  VoxEvent_GetExactParmMatchFlag (flags>>3)&1 -> flags&0x8  ChooseSentence 99,41 -> 97,24  DESCARTADO

La diferencia es que `(x + 7) / 8` es una division **con signo** de un valor
cuyo signo GCC tiene que razonar; `bitField >> 2` sobre un `unsigned char`
promocionado no pasa por `expand_divmod` de la misma forma.

### El sintoma que lo delata

`iSPCHBank_AddToQueue` tenia `faltan 1, sobran 0, lbz+1`: **al objetivo le
sobraba una carga** de `hdr->sampleRepeat` que nosotros cacheabamos. Es el mismo
sintoma que el brief atribuye a `true_dependence`, y **aqui no lo era** (§3).
Con `/ 8` en el inline que se expande justo antes, la carga sobrevive a CSE.

## 2. El segundo mecanismo: la variable con DOS sets

`triage.py` ya lo documenta para `UpdateCameraMovers` («con `view` asignada en
tres sitios en vez de redeclarada… `find_base_value` da la base por
desconocida»). En `spch` vale **1.264 B** y siempre con la misma forma:

    return  A + expr;      ->      A_var  = A;
                                   A_var += expr;
                                   return A_var;

| ensayo | sitio | efecto |
|---|---|---|
| c12 | `spchi.h` `iSPCH_GetContextDataAddr`: `return data + (...)` -> `data += (...); return data;` | `iSPCH_ChooseSentence` 99,410 -> **100 %** y `iSPCH_SentenceIsContextMatch` 83,790 -> **100 %** (+1.096 B) |
| c13 | `spchrule.c`: `settingsAddr = iSPCH_GetSentenceRulesAddr(event) + sentenceNum*bytesPerRule;` partido en dos sentencias | `iSPCH_GetSentenceRuleSettings` 98,571 -> **100 %** (+168 B) |

**El orden importa**: en el barrido h1-h7 la forma `data +=` con `>> 3` (h3) dio
**identico**; solo con `/ 8` ya puesto (h3b) cierra. Los dos mecanismos son
multiplicativos, no aditivos.

Y **no es universal**, medido:

    m1  iSPCH_GetSentenceRulesAddr con `data +=`              identico
    m2  BANKHDR_GetSampleRepeatAddr con `addr += cycleBytes;` **-292 B**, rompe spchbank  DESCARTADO

## 3. `true_dependence` en `spch`: NO vale — y esta medido

El brief pedia probar el mecanismo de la ronda 20 (vista `array[1]` de un global
para que `MEM_IN_STRUCT_P` lo marque como agregado). **No aplica en `spch`, y la
razon es estructural, no una veda de barrido:**

* El sintoma de `iSPCHBank_AddToQueue` (`lbz+1`) **no venia de un global**:
  `hdr->sampleRepeat` es un campo de struct por **puntero parametro**. La palanca
  de la ronda 20 solo existe para escalares en direccion FIJA (globales), y aqui
  no hay ninguno: `spchbank.c`, `spchsamp.c` y `spchrule.c` no leen un solo
  global en las funciones del encargo.
* **Y no era aliasing en absoluto.** Volcado RTL del propio `cc1plus` sobre un
  caso minimo con las banderas exactas de la unidad
  (`-O2 -G0 -fno-strength-reduce -fno-strict-aliasing -ffast-math -x c++`,
  `-dr -da`): entre las dos cargas **no hay ninguna escritura a memoria**, y la
  que desaparece la borra **el primer pase de CSE**:

      .jump  (antes de cse)   3 cargas de `(mem/s:QI (plus (reg 82) (const_int 6)))`
      .cse                    2   <- cse borra insn 96 (la de en medio)
      .gcse .loop .cse2       2

  O sea: redundancia pura dentro del bloque basico extendido, no invalidacion.
  Ninguna heuristica de `MEM_IN_STRUCT_P` puede afectarla porque **no se consulta**.
* Barrido de banderas sobre ese caso minimo, todas dan 2 cargas (el objetivo
  tiene 3): `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-expensive-optimizations`,
  `-fno-rerun-cse-after-loop`, `-fno-thread-jumps`, `-fno-schedule-insns`,
  `-fno-schedule-insns2`.
* Barrido de **diez formas de fuente** del cuerpo de `AddToQueue`, todas 2 cargas:
  comparacion invertida, cuerpo entero dentro del `if`, `int index`, orden de
  declaracion invertido, `*queue` en vez de `queue[0]`, `queue` calculada antes
  del `return` temprano, `index++`, `*(queue+index+1)`, `!hdr->sampleRepeat`.

**Lo que si lo cerro fue `/ 8` en `BANKHDR_GetSampleRepeatAddr`** (§1), que se
expande *entre* las dos cargas: cambia el grafo de pseudos del bloque y la carga
deja de ser redundante para CSE.

Conclusion para el proyecto: **el sintoma «al objetivo le sobra una carga» NO
implica `true_dependence`.** Antes de sacar la vista agregada hay que mirar (a)
si el simbolo es un global escalar en direccion fija, y (b) si hay una escritura
entre medias. Si no las hay, es CSE y la palanca es otra.

## 4. Ensamblador escrito a mano en `spchevnt.c` — retirado

`spchevnt.c` traia, de una ronda anterior ya commiteada (`4e8d4944`), dos
inlines falsificados:

    static inline unsigned char *iSPCH_GetContextDataAddrKnown(...)
        register unsigned int offset __asm__("r10");
        __asm__ volatile("add %0, 8, 8" : "=r"(offset));      <- instruccion escrita a mano
        __asm__ volatile("" : "+r"(rounded));                 <- barreras
    static inline int VoxEvent_GetFollowEventData(...)
        register int result __asm__("r5"); ... x4
        __asm__ volatile("lbz %0, 6(%1)" : "=r"(numSentences) : "r"(event));   <- otra

Con ellos la funcion daba **86,818 %**, o sea **cero bytes** (`matched_code` es
todo-o-nada), asi que retirarlos no costaba nada. El DWARF del original dice que
`VoxEvent_GetFollowEventData` tiene **dos locales** (`result // r5`,
`ptr // r10`) y llama al `iSPCH_GetContextDataAddr` normal — no existe ningun
`…Known`. Reescrito honesto (c1):

    c1  fuente honesta segun el DWARF   86,818 % -> 74,739 %
        pero `triage`: faltan 2 sobran 2 (mr+1, srawi+1, srwi-1, lbz-1)
                    -> faltan 1 sobran 0 (mr+1)

Bajo el porcentaje y **acerco la estructura**: es el caso de manual de «el
porcentaje no mide avance». Con c4 (`/ 8`) encima, la funcion cierra al 100 %.

## 5. `audit.py`: la pasada del principio y el bug que encontro

Primera pasada sobre las cinco del encargo: **cero FALLA**. Ampliada a las 11
unidades de la biblioteca, dos FALLA en **`spchcsis`** (unidad fuera de mi
encargo, cliente de `spchi.h`), **confirmadas con segunda pasada**:

    iSPCH_CsisCb__FPQ24Csis9ParameterPv  FALLA: reubicacion a otro simbolo:
        iSPCH_FindEvent__FP9EventSpec contra iSPCH_FindEvent
    iSPCH_InitCsis__FPv                  FALLA: reubicacion a otro simbolo:
        SPCH_MakeEventSpec__Fiii contra SPCH_MakeEventSpec

**FALLA real, no fantasma**, y verificado que **es anterior a mis cambios**
(reconstruido `spchcsis` con `spchi.h` en HEAD: los dos FALLA siguen). Causa:
`spchcsis.cpp` declaraba cinco funciones como `extern "C"`, y las cinco estan
**mangladas** en el original (`config/GOWE69/symbols.txt`):

    SPCH_MakeEventSpec__Fiii, iSPCH_FindEvent__FP9EventSpec,
    iSPCH_MemAlloc__FUi, iSPCH_MemFree__FPv, iSPCH_AddEvent__FPUi

O sea: la unidad estaba al 100 % por objdiff **llamando a cinco simbolos que no
existen**. Quitado el `extern "C"` de las cinco declaraciones: objdiff sigue
360/360 B (100 %) y `audit.py` pasa con **cero FALLA en dos pasadas**. Es el
patron «el 100 % que miente» y cuesta cero bytes, como dice el brief.

## 6. Ensayos numerados, con su cifra medida

    c1  spchevnt: fuera el ensamblador a mano; VoxEvent_GetFollowEventData
        segun el DWARF (2 locales, iSPCH_GetContextDataAddr normal)
                                       86,818 -> 74,739 %  (faltan 2/sobran 2 -> 1/0)  <- queda
    c2  spchi.h iSPCH_GetContextDataAddr con UN solo local `data`
        (el DWARF de spchpick/spchevnt/spchrule dice uno; el arbol tenia tres)
                                       +0 B, 0 unidades cambian                        <- queda
    c3  spchpick iSPCH_SentenceIsContextMatch: `sentenceContexts = ...` ANTES de
        `contextData = ...` (el mapa de lineas pone spchlib.h:479/465/466 antes
        de 928)                        32,903 -> 32,581 %                              <- queda
    c4  spchi.h iSPCH_GetContextDataAddr: `>> 3` -> `/ 8`
                                       **+352 B**, spchevnt 100 %                      <- queda
    c5  spchi.h BANKHDR_GetSampleRepeatAddr y spchrule.c: `>> 3` -> `/ 8`
                                       **+292 B**, spchbank 100 %                      <- queda
    c6  spchsamp: `*sampleOffset = offset;` ANTES de `*dataBytes = ...`
                                       91,765 -> 85,735 %  (ver aviso abajo)           <- queda
    c7  spchpick iSPCH_MakeValidParmsMask: `validParmsMask = 0;` antes de
        `parmTypesMask = ...` (lineas 2252/2253 del objetivo contra 2276)
                                       ChooseSentence 98,042 -> 99,410 %,
                                       y la clase pasa de «reorden local» a
                                       «14 registros» (cero filas movidas)             <- queda
    c8  spchi.h VoxEvent_GetSentenceParmMask: declarar `data` antes que
        `sentenceParmsMask` (el DWARF de la copia inline lista las locales al
        REVES: `sentenceParmsMask, data`)
                                       identico                                        <- queda
    c9  spchpick iSPCH_SentenceIsContextMatch: los dos `if` anidados a un `&&`
        (el objetivo no emite nota de linea para el segundo: los dos estan en
        la 2375)                       identico                                        <- queda
    c10 spchrule: `settingsSize = ...` antes de `settingsAddr = ...`
        (lineas 267 contra 271 del objetivo)
                                       identico                                        <- queda
    c11 spchi.h VoxEvent_GetSentenceParmMask: fundir `data = ...; data += ...`
        en una sentencia            identico, y el mapa de lineas dice que son
                                    DOS (spchlib.h:958 y :975)              DESCARTADO
    c12 spchi.h iSPCH_GetContextDataAddr: `data += (...); return data;`
                                       **+1.096 B**, ChooseSentence y
                                       SentenceIsContextMatch al 100 %                 <- queda
    c13 spchrule: `settingsAddr = iSPCH_GetSentenceRulesAddr(event);` y
        `settingsAddr += sentenceNum * bytesPerRule;` en dos sentencias
                                       **+168 B**, spchrule 100 %                      <- queda
    c14 spchcsis: fuera el `extern "C"` de las cinco declaraciones
                                       360/360 B igual, audit.py 2 FALLA -> 0          <- queda

**Aviso sobre c6.** Es una correccion estructural real —el objetivo guarda
primero en `r5` (`sampleOffset`) y luego en `r6` (`dataBytes`), y GCC **no
reordena dos escrituras que pueden solaparse**, asi que ese es el orden de la
fuente original— pero baja el porcentaje de 91,765 a 85,735 % y sube las filas
movidas de 2 a 4. **Cero bytes en las dos direcciones** (la funcion no cierra ni
antes ni despues; `spchsamp` sigue 48/184 B), y ninguna unidad baja en
`measure.py --cmp`. La dejo por la regla del brief, pero **queda anotado por si
el siguiente prefiere el numero al parecido**.

## 7. Vedas nuevas, con la sentencia barrida

* **`AddToQueue`, la carga que falta**: barridas **diez** formas de la sentencia
  `if (index < hdr->sampleRepeat)` y su entorno (lista en §3) mas **siete**
  banderas; todas 2 cargas contra las 3 del objetivo. Cerrada finalmente por
  `/ 8` en el inline vecino, no por ninguna de ellas.
* **`>>` -> `/` fuera de `(x + 7) / 8`**: barridas `bitField >> 2 -> / 4`,
  `numRowContexts >> 4 -> / 16` y las dos juntas: **identicas**. Y
  `(flags >> 3) & 1 -> flags & 0x8` **empeora** (ChooseSentence -2,17 pp).
* **`data +=` fuera de `iSPCH_GetContextDataAddr`**: barrida en
  `iSPCH_GetSentenceRulesAddr` (identico) y en `BANKHDR_GetSampleRepeatAddr`
  (`addr += cycleBytes;` -> **-292 B**, rompe las dos de `spchbank`).
* **`iSPCH_ChooseSamples`, la local `sampleTable`**: el DWARF del original lista
  **13 locales y `sampleTable` no esta**. Barrida la sentencia
  `sampleTable = (unsigned char *)bank + 0xC;` escribiendo la expresion en linea
  (f1): 95,534 -> 97,951 % **pero** aparece `faltan 1, addi+1` y el marco pasa de
  `stmw r17` a `stmw r18` — el objetivo **si** materializa `bank + 0xC` en un
  registro salvado (`add r31, r17, r0` y `addi r5, r31, 0x2`, contra nuestro
  `addi r5, r31, 0xe` con el `+0xC` plegado). **La local existe aunque el DWARF
  no la liste**; revertido.
* **`iSPCH_GetSampleSizeData`, la cadena de `offset`**: barrida
  `offset = ((offset << 8) + sampleData[1]) * blockSize;` partida en dos
  sentencias (dos sets): 85,735 -> **81,176 %** y aparece un `mr` de mas
  (140 B contra 136). Revertido.

## 8. Lo que queda: 936 B en dos funciones, las dos `faltan 0, sobran 0`

    524 B  98,321 %  spchpick  iSPCH_MakeSampleRequests   reorden local, dmax 2 (2 filas)
    412 B  95,534 %  spchpick  iSPCH_ChooseSamples        reorden local, dmax 3 (4 filas)
    136 B  85,735 %  spchsamp  iSPCH_GetSampleSizeData    reorden local, dmax 4 (4 filas)

* **`iSPCH_MakeSampleRequests`** — la UNICA diferencia es donde cae
  `add r30, r31, r9` (`phraseChoice = &gEventChoice[channel].phrases[i]`,
  spchpick.c:1921 del objetivo): el objetivo lo emite **detras** del
  `lwzx r3, r31, r9` de la linea 1924, nosotros delante. Multiconjunto exacto,
  mismos registros salvados, orden de sentencias identico al mapa de lineas.
  Es jitter del planificador.
* **`iSPCH_ChooseSamples`** — igual, con `addi r17, r30, 0xc` (`sampleTable`)
  emitido al principio en el nuestro y al final en el objetivo. Ver la veda f1.
* **`iSPCH_GetSampleSizeData`** — permutacion completa de registros
  (`sampleSize` r7/r8, `blockSize` r11/r4, `offset` r8/r11, `nextSampleData`
  r4/r7, `result` r0/r9) con las 36 instrucciones emparejadas 1:1. El orden de
  declaracion ya coincide con el DWARF. `spchsamp` **no tiene mapa de lineas**
  (todas las instrucciones salen sin linea).

## 9. Que NO he probado

* **`permuter.py`** y **`lreg.py`** sobre ninguna de las tres que quedan.
  `iSPCH_GetSampleSizeData` (136 B, 36 instrucciones emparejadas 1:1, solo
  permutacion de 5 registros) es el caso mas barato del arbol para intentarlo.
* **Restricciones de registro** en ninguna de las tres (el brief las deja como
  ultimo recurso y no he agotado la busqueda de la forma de fuente).
* **`(x + N) / M` en el resto del arbol.** El mecanismo de §1 es una propiedad de
  `expand_divmod` de GCC 2.9, no de `spch`: `grep -rn "+ 7) >> 3\|+ 3) >> 2\|+ 15) >> 4"`
  sobre `src/` deberia dar el frente. **No lo he contado** — y la nota
  «extrapolar frentes» dice que un hallazgo no es un frente hasta contarlo.
* **`sptick.c` y `sputil.c`**: no incluyen `spchi.h`, no las he tocado; las he
  recompilado y auditado como testigo (cero FALLA, cero cambio de bytes).
* **La cadena completa (link + DOL)**: ninguna unidad de `spch` esta enlazada
  (`linked False` en `build.ninja`), asi que el DOL no puede cambiar; no he
  ejecutado la construccion completa ni he tocado `configure.py`.
* **Las otras 4 unidades de `spch`** (`spchinit`, `spchrand`, `spchrslv`,
  `spchcsis`): ya al 100 %, solo recompiladas y auditadas por ser clientes de
  `spchi.h`. En `spchcsis` si he corregido el `extern "C"` (§5).
* **El resto de la biblioteca `spch` fuera de `cmn`**: no la he mirado.
