# r47 — sonido: `zEAXSound` promociona entera, y dos cierres por prioridad de allocno

Fecha: 2026-09-09. Territorio: `zEAXSound`, `zEAXSound2`, `zSpeech`, `zFeOverlay`.
Base: `1b6cd105` (brief de la r47).

## Resultado

| unidad | antes | despues |
|---|---|---|
| `zEAXSound` | 1 funcion / **384 B** | **0 funciones / 0 B — UNIDAD ENTERA AL 100 %** |
| `zEAXSound2` | 3 funciones / 2.304 B | **2 funciones / 1.912 B** |
| `zSpeech` | 2 funciones / 912 B | igual (912 B) |
| `zFeOverlay` | 1 funcion / 440 B | igual (440 B) |

**+776 B de `matched` y +2 funciones exactas.** Y, sobre todo, `zEAXSound`
queda **sin una sola funcion pendiente**: sus **151.092 B de `.text`** pasan a
poder promocionar como `linked`.

```
# Play__13cStichWrapperPC10SND_Params        target=100.0%  ours=100.0%  size=384/384
# Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi target=100.0%  ours=100.0%  size=392/392
fncmp zEAXSound    0 de 773 funciones con el CODIGO distinto -- 0 B
fncmp zEAXSound2   2 de 930 -- 1.912 B   (GenerateRoadNoise, ProcessUpdate)
fncmp zSpeech      2 de 703 --   912 B   (RoadblockFlow::Setup, LoadSpeechBank)
audit zEAXSound  exit 0  773 ok / 0 FALLA   (771 antes)
audit zEAXSound2 exit 0  928 ok / 0 FALLA   (927 antes)
audit zSpeech    exit 0  701 ok / 0 FALLA
lcfix.py --check  todas las entradas @lc estan al dia
```

Tres ficheros tocados, los tres de mi territorio:

- `src/Speed/Indep/Src/EAXSound/STICH_PlayBack.cpp` (cierre)
- `src/Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h` (cierre, gate en las dos SourceLists)
- `src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp` (regresion reparada)

`CARSFX_Turbo.cpp`, `SpeechManager.cpp`, `RoadblockFlow.cpp` y
`CarCustomize.cpp` terminan **byte a byte** iguales a la entrada (SHA-256
comprobados, §7).

---

## 1. CERRADA — `Play__13cStichWrapperPC10SND_Params` (384 B), y con ella `zEAXSound`

La veda de esta funcion tenia **cinco rondas** encima (r36e 14 formas, r36f 40,
r37 13, iter2/3/4/7 con sonda de depurador, r46 la retirada de `scale`). Cae con
una linea:

```cpp
            cSTICH_PlayBack::QueueSampleRequest(samplereq);
            __asm__("" : : "r"(this->ActiveSamplesRefs));   // <- DENTRO del if
        }
```

**Cero bytes emitidos, `fndiff` 100,0 %, 384/384 B, 0 filas.**

### El mecanismo, entero

Las cuatro filas eran el cruce `r30`/`r31` entre el desplazamiento `i*4`
(pseudo **115**) y la base `this+0x24` (pseudo **116**). La cadena causal, ya
completa, mezcla lo que dejaron iter2/iter3 con lo que faltaba:

1. `&samplereq` es la direccion de una local de pila, o sea
   `(set (reg 141) (plus (reg 31) (const_int 8)))` — y **el registro 31 de esa
   RTL es `FRAME_POINTER_REGNUM`, no r31**. `set_preference` no lo sabe y
   deposita r31 en `hard_reg_full_preferences[141]`.
2. `global_alloc` limpia `eliminable_regset` de `hard_reg_preferences` y de
   `hard_reg_copy_preferences` **pero no de `hard_reg_full_preferences`**
   (`global.c`, el bucle justo detras de `global_conflicts`). La preferencia
   fantasma sobrevive.
3. `prune_preferences` la hereda: 141 tiene menos prioridad que 115 y 116 y
   entra en conflicto con los dos, asi que `regs_someone_prefers[115]` y
   `regs_someone_prefers[116]` acaban con el bit de r31.
4. En `find_reg`, la **pasada 0** excluye `regs_someone_prefers` y ademas solo
   admite registros que ya esten en `regs_used_so_far`. El primero de la pareja
   que se reparta encuentra r30 (que ya usa el allocno 176) y se lo queda; el
   segundo falla la pasada 0 y en la pasada 1 —que ignora
   `regs_someone_prefers`— coge r31.

O sea: **el que se reparte ANTES se lleva r30 y el otro r31**. El objetivo
quiere `115 -> r30` y `116 -> r31`, luego hace falta repartir 115 antes que 116.

`allocno_compare` los ordena por `floor_log2(n_refs) * n_refs / live_length`:

| pseudo | que es | refs | len | prioridad |
|---|---|---:|---:|---:|
| 176 | `priority_to_prune` | 11 | 38 | 8.684 |
| **116** | `this+0x24` | 6 | 14 | **8.571** |
| **115** | `i*4` | 6 | 16 | **7.500** |
| 82 | `this` | 15 | 62 | 7.258 |
| 141 | `&samplereq` | 7 | 33 | 4.242 |

La ventana util es estrechisima: subir 115 por encima de 8.684 lo saca **delante
de 176**, y entonces r30 aun no esta en `regs_used_so_far`, falla la pasada 0 y
115 se lleva r31 — peor. Por eso ninguna de las ~70 formas anteriores servia.

**La salida es la contraria: subir 116 lo bastante** para que se reparta antes
de 176. Con un `asm` de solo entrada sobre la base, `REG_N_REFS(116)` pasa de 6
a **8** (peso 2 por estar en el bucle), `floor_log2` salta de 2 a 3 y la
prioridad de 8.571 a **11.428**; el rango crece de 14 a 21 insns porque el `asm`
va detras de su ultimo uso. Verificado en el `.greg`:

```
;; 21 regs to allocate: 127 113 133 181 182 183 155 116 175 83 177 115 82 106 141 ...
   reg 116  refs=8  len=21  pri=11428  -> r31
   reg 115  refs=6  len=16  pri=7500   -> r30
```

116 se reparte en la posicion 7, cuando `regs_used_so_far` solo tiene r31 (de
155) entre los no volatiles: pasada 0 falla, pasada 1 le da **r31**. Despues 177
coge r30 y 115 lo encuentra libre. Los demas repartos no se mueven.

### El sitio importa: DENTRO del `if`

| forma | posicion del `asm` | B | filas |
|---|---|---:|---:|
| P1 | tras `if (ActiveSamplesRefs[i])` | 396 | — |
| P2 | tras `Initialize()` | 392 | — |
| P3 | tras `SampleQueueItem samplereq;` | 392 | — |
| P4 | tras `samplereq.pSample = ...` | 384 | 11 |
| P5 | tras `samplereq.pStitch = this;` | 384 | 11 |
| **P6** | **tras `QueueSampleRequest(samplereq)`, dentro del `if`** | **384** | **0** |
| P7 | detras del `}` del `if` | 384 | 8 |

Un `asm` sin salidas es **volatil**: donde lo pongas, es barrera. P4/P5 lo meten
entre los cuatro `stw` de `samplereq` y los reordenan. P7, **fuera del `if`**,
alarga una ranura el rango del `li r26,0` del constructor de `samplereq`
(pseudo 142) y le da la vuelta a un empate de **3 unidades** con `stitch_type`
(pseudo 84): 1.612 contra 1.609 en la base, 1.562 contra 1.590 con el `asm`
fuera. De ahi las 8 filas de r25/r26. Dentro del `if` el `asm` cae detras del
ultimo uso de 142 y el empate no se toca.

Comprobado ademas que **comentarios y lineas en blanco no cambian nada** (base
+1 linea de comentario: identica, 4 filas): el fallo de P7 no es el numero de
linea, es el lado del `}`.

Con esto se retira el comentario `// VEDA r48` de `STICH_PlayBack.cpp`.

---

## 2. CERRADA — `Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi` (392 B)

Las siete filas eran, como dejo escrito r46, un intercambio de color entre el
cero persistente y `&this->mData`. El `.lreg` da el porque exacto, y es un
**EMPATE**:

| allocno | que es | refs | len | `QTY/allocno_compare` |
|---|---|---:|---:|---:|
| 154 | `&this->mData` | 3 | 27 | 1*3/27 = **1.111** |
| 156 | el cero persistente | 4 | 72 | 2*4/72 = **1.111** |

`allocno_compare` desempata por **numero de allocno** (`return v1 - v2`), asi que
gana 154, se reparte primero y coge r29 en la pasada 0; 156 se queda r27. El
objetivo quiere lo contrario.

Basta **una ranura mas** dentro del rango de vida de 154 para romper el empate:
154 pasa a 3/28 = 1.071 y 156 a 8/73 = 1.095. Y la ranura no cuesta un byte:

```cpp
// src/Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h, ctor de Csis::FX_ROADNOISE
        Result result = Class::CreateInstance(&gFX_ROADNOISEHandle, &this->mData, &this->mpClass);
        __asm__("");                       // <- cero bytes, una ranura
        if (result < RESULT_OK) {
```

`fndiff` = **100,0 %**, 392/392 B. Colocado dentro del `if` (H10) da lo mismo;
detras del segundo `CreateInstance` (H13) no cambia nada, porque cae fuera del
rango de 154.

**Gate cumplido**: `FX_ROADNOISE` solo lo consumen `CARSFX_Roadnoise.cpp` y
`ENVIRO_AEMS.cpp`, pero la cabecera entra en las dos SourceLists. Medidas las
dos: `zEAXSound` sigue en **0** funciones distintas y `zEAXSound2` baja de 3 a 2
(las otras expansiones inline del constructor —`InitSFX` y `ENVIRO_AEMS.cpp`—
no regresan; `audit` 928 ok / 0 FALLA).

---

## 3. REGRESION REPARADA — `GenerateRoadNoise` volvia a medir 1.232 B

r46 dejo medido que la local `float slipBoost = 0.1f` **vale 8 B reales** aunque
el DWARF del original no la liste. El commit `e66b11e2` (iteraciones 2-6) la
retiro y sustituyo por el literal, y la funcion llevaba desde entonces
**1.232 B contra los 1.240 del objetivo** — el comentario `// UNSOLVED` que ese
mismo commit anadio describe el estado *anterior* a su propio cambio («1240 B al
97,11 %»).

Restaurada. La funcion vuelve a **1.240/1.240 B exactos** y a las 33 filas de la
base de r46; ninguna vecina se mueve (`zEAXSound2` sigue con las mismas 2
pendientes). Es la tercera vez que la memoria del proyecto avisa de lo mismo:
**una local que el DWARF no lista no sobra por eso.**

Tres ordenes de sentencia nuevos medidos sobre esa base, todos negativos:

| # | forma | B | filas |
|---|---|---:|---:|
| GR0 | base restaurada | 1.240 | 33 |
| GR1 | la segunda acumulacion de `fRightVol` **antes** del `bLength` | 1.240 | 51 |
| GR2 | GR1 + `slipBoost` declarada al principio del grupo | 1.240 | 51 |
| GR3 | `slipBoost` declarada antes de la primera acumulacion | 1.240 | 33 |

GR1/GR2 reproducen el G3 de r46. **El orden de sentencias no es la palanca**: el
objetivo emite la segunda acumulacion (`fmadds f29,f29,f10,f29`) en la ranura
114, *dentro* del bloque previo al `bso` del `bLength`, y nosotros en la 134, ya
en el bloque de union. Es movimiento **interbloque** del planificador, no orden
de fuente.

---

## 4. `SetScreenNames__13CustomizeMain` (zFeOverlay, 440 B): diagnostico nuevo y firme

**Es la UNICA funcion pendiente de `zFeOverlay`** (467 funciones del objetivo,
una sola distinta), asi que cerrarla promociona los 141.224 B de la unidad.
`fncmp` y `triaje` no la ven porque el codigo va en `.over`; con `lee()`
parcheado para aceptar `.over`/`.rela.over` el inventario sale completo, y son
**6 filas**, no 23: el `asm("" : "+m"(g_pCustomizeSubPkg))` que dejaron las
iteraciones 2-6 ya arreglo el orden de los stores.

Las seis filas son dos cosas:

- el `@ha` de `g_pCustomizeSubPkg` va a **r30** y el de `g_pCustomizeSubTopPkg`
  a r9; el objetivo usa **r3** y r30;
- el `addi r3,r3,gCarCustomizeManager@l` sale dos ranuras mas tarde.

### La causa (esto es `local_alloc`, no `global_alloc`)

`global_alloc` solo reparte **2** pseudos en esta funcion; todo lo demas lo
resuelve `local_alloc`, con la MISMA formula (`QTY_CMP_PRI` = `floor_log2(refs)
* refs * size / (death − birth)`) y el mismo desempate por numero.

El `.lreg` senala el pseudo **130** = `high(g_pCustomizeSubPkg)`. El operando de
memoria del `asm "+m"` **reutiliza ese mismo pseudo** (insn 179 del volcado), asi
que le sube `REG_N_REFS` de 2 a **4** y el rango de ~24 a **54**:

```
Register 130 used 4 times across 54 insns in block 2   ->  pri 1481  ->  r30
Register 136 used 2 times across 18 insns in block 2   ->  pri 1111  ->  r9
Register 140 used 2 times across 20 insns in block 2   ->  pri 1000  ->  r29
```

Con 1.481 es la **mas prioritaria de las once direcciones del bloque**, se
reparte la primera y, sobre una ventana que cubre todo el grupo de stores, se
come los diez GPR que `REG_ALLOC_ORDER` prueba antes de r30.

**Y aqui hay un dato de metodo que corrige lo que estaba escrito**: el
`REG_ALLOC_ORDER` de rs6000 **no** empieza por r31. Es

```
{32, 45..33, 63..46, 75,74,69,68,72,71,70, 0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30, 29, ... 12, ...}
```

es decir **los volatiles primero** (r0, r9, r11, r10, r8, r7, r6, r5, r4, r3) y
solo despues r31, r30, r29… Lo que hace que en `global_alloc` parezca «31, 30,
29…» es que `find_reg` mete `call_used_reg_set` en `used1` para todo allocno que
cruce una llamada. En `local_alloc`, con cantidades que no cruzan llamadas, **el
orden real manda**: por eso el objetivo acaba en **r3**, el ultimo volatil de la
lista (r0 no vale, la clase es `BASE_REGS`).

O sea: al objetivo le sale r3 porque su direccion vive solo del `lis` a su `stw`
y **se reparte la ULTIMA**. Lo que hace falta es una forma que suba la prioridad
de ese primer store **sin anadir referencias a su pseudo de direccion**.

### Lo medido y negativo (18 formas, todas con cifra)

| # | forma | filas |
|---|---|---:|
| — | base actual (`"+m"` detras de las ocho asignaciones) | **6** |
| S2 | el mismo `"+m"` justo detras de la primera asignacion | 6 (identico) |
| S6 | el mismo `"+m"` detras de la segunda | 6 (identico) |
| S5 | `CarCustomizeManager *mgrp = &gCarCustomizeManager` | 6 (identico) |
| S3 | dos `"+m"` seguidos sobre `g_pCustomizeSubPkg` | 10 |
| T3 | `mgrp` + `asm("" : "+r"(mgrp))` | 19 |
| T4 | `mgrp` + `asm("" : "+r"(mgrp) : "m"(g_pCustomizeSubPkg))` | 22 |
| V2 | base + `asm("" ::: "memory")` tras la primera asignacion | 24 |
| S4 | un solo `asm` con `"+m"` sobre las dos primeras | 26 |
| T2 | `asm("" : "+m"(gCarCustomizeManager))` antes del `if` | 30 |
| U2 | `"m"` (solo lectura) tras la primera asignacion | 31 |
| V1 | solo `asm("" ::: "memory")`, sin el `"+m"` | 31 |
| S1 | `"+m"` sobre `SubPkg` y sobre `SubTopPkg` | 32 |
| T1 | `mgrp` + `asm("" : "+r"(mgrp) : "m"(g_pCustomizeHudColorPkg))` | 32 |
| V3/V4 | `"memory"` en otras posiciones | 32 |
| U1/U3 | `"m"` (solo lectura) en lugar del `"+m"` | 34 |
| U4 | `"m"` + `"+m"` | **444 B** |

`"m"` de solo lectura no vale: lo que adelanta el store es la
**antidependencia** del `"+m"`, no la referencia.

---

## 5. Vedas de territorio que NO he tocado, y por que

- **`ProcessUpdate__12CARSFX_Turbo`** (672 B). r46 la dejo cerrada por
  construccion y he releido `rank_for_schedule` de `haifa-sched.c` para
  comprobarlo: el nivel de `INSN_REG_WEIGHT` (palanca 10) **solo se consulta si
  `INSN_PRIORITY` empata**, y aqui no empata — la cadena de `lis r8,0x4330`
  (conversion int->float) llega hasta `lwz r3,0xc(r1)`, unas diez insns, y el
  `mr. r7,r3` muere en el limite del bloque. Sin subirle `INSN_PRIORITY` al
  `mr.` no hay forma; sigue vedada.
- **`Setup__Q26Speech13RoadblockFlow`** (596 B, 4 filas). No abro ensayos: r45
  midio 12 formas y la ABI de la llamada virtual. Dejo escrita **la hipotesis
  causal que falta por probar** (§6.3): con `INSN_PRIORITY` empatado entre
  `lwz r0,0x2e4(r9)` y `lha r3,0x2e0(r9)`, decide `INSN_REG_WEIGHT`, y pesa 0 el
  que **mata r9**, o sea el que sea el ULTIMO uso del vptr en la RTL. El objetivo
  mata r9 en el `lwz`; nosotros en el `lha`.
- **`LoadSpeechBank`** (316 B). Sin ensayos nuevos; el diagnostico de r46
  (`cse-follow-jumps`) sigue siendo el frente.

---

## 6. Propuestas y avisos fuera de mi territorio

1. **El eje «a quien» tiene una version sin coste que no estaba en el brief.**
   `__asm__("" : : "r"(x))` —solo ENTRADA, sin salidas— **no emite un byte** y
   sube `REG_N_REFS(x)` en `loop_depth` unidades. Con `floor_log2` en
   `allocno_compare` eso mueve un allocno varias posiciones de golpe. Es lo que
   cerro `cStichWrapper::Play` despues de cinco rondas. La veda de r36f
   («cualquier `asm` dentro de ese bucle cuesta 4 B») valia solo para los `asm`
   **con salida**: la salida no coalesce y aparece el `mr`; sin salida no hay
   nada que coalescer. **Conviene rehacer con esta forma los near-miss de
   reparto que se dieron por cerrados con `"+r"`.**
2. **`__asm__("")` a secas rompe empates exactos de `allocno_compare` /
   `QTY_CMP_PRI`.** Cero bytes, una ranura, +1 al `REG_LIVE_LENGTH` de todo lo
   vivo en ese punto. Cuando dos allocnos empatan y el desempate por numero cae
   del lado equivocado, es la palanca mas barata que hay. Cerro
   `Play__16CARSFX_RoadNoise`.
3. **Corregir el `REG_ALLOC_ORDER` que circula en los informes.** No es
   «31, 30, 29…»: es `0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30, 29, …, 12`
   (`config/rs6000/rs6000.h:932`). El «31 primero» solo se ve en `global_alloc`
   cuando el allocno cruza llamadas. En `local_alloc` la diferencia es
   determinante — es justo lo que separa `SetScreenNames` de cerrar.
4. **`fncmp.py` y `triaje.py` pueden ver `zFeOverlay` y `zOnline` con dos
   sustituciones de una linea** en `lee()`:
   `nom(s[0]) == '.text'` -> `nom(s[0]) in ('.text', '.over')` y lo mismo con
   `'.rela.text'`. Con eso salio que a `zFeOverlay` **le falta UNA sola funcion**
   de 467, cosa que hasta ahora nadie podia listar. Lo propongo como parche al
   script (no lo aplico: es herramienta compartida).
5. **`SetScreenNames` es el unico obstaculo de `zFeOverlay`** (141.224 B de
   `.text`). Con el diagnostico de §4 el frente es concreto: subir la prioridad
   del primer store sin tocar `REG_N_REFS` de su `high(...)`.
6. **Reproducciones minimas de 5 s.** `types.h` + el `.cpp` suelto reproduce
   `cStichWrapper::Play` **instruccion a instruccion** (solo cambian los addend
   de los saltos, porque la funcion vive en otro desplazamiento), y lo mismo con
   `CARSFX_Roadnoise.cpp`, `CARSFX_Turbo.cpp` y `CarCustomize.cpp`. 5-10 s por
   ensayo con volcado RTL incluido, contra los 20-60 s de la SourceList. Los
   arneses quedan en `scratchpad/r47_snd/` y son genericos por variables de
   entorno (`MUNIT`, `MCPP`, `MSYM`, `MFN`).

---

## 7. Verificacion

```
build_direct.py zEAXSound zEAXSound2 zSpeech zFeOverlay      4 ok, 0 fallidas
fndiff  Play__13cStichWrapperPC10SND_Params         100.0 %  384/384
fndiff  Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi 100.0 %  392/392
fndiff  GenerateRoadNoise__16CARSFX_RoadNoise        97.11 % 1240/1240 (tamano exacto)
fndiff  SetScreenNames__13CustomizeMain              98.0 %  440/440   (sin cambios)
fncmp   zEAXSound   0/773 distintas   (antes 1: cStichWrapper::Play)
fncmp   zEAXSound2  2/930 distintas   (antes 3)
fncmp   zSpeech     2/703 distintas   (igual)
audit   zEAXSound 773 ok/0 FALLA · zEAXSound2 928 ok/0 FALLA · zSpeech 701 ok/0 FALLA
lcfix.py --check    limpio
git diff --check    limpio en src/Speed/Indep/Src/EAXSound/
```

Ninguna funcion empeora en ninguna de las cuatro unidades.

SHA-256 (16 primeros) al terminar:

```
STICH_PlayBack.cpp    91faad4fea18d45f   (cierre)
ENVIRO_AEMS.h         9ce8382d1c380c2b   (cierre)
CARSFX_Roadnoise.cpp  e513d139636e6bd2   (slipBoost restaurada)
CARSFX_Turbo.cpp      37d3945dfb96e3cb   = base, sin tocar
SpeechManager.cpp     b24fbfd74aa4d0f3   = base, sin tocar
RoadblockFlow.cpp     d76d681f8d0df1ad   = base, sin tocar
CarCustomize.cpp      fff034faa23157c2   = base, sin tocar
```

`STICH_PlayBack.cpp` (470 lineas) y `ENVIRO_AEMS.h` (1.408) conservan **CRLF
puro**; `CARSFX_Roadnoise.cpp` (486) tambien. `CarCustomize.cpp` es LF puro y no
se toco. No queda ningun fichero `__r47*` bajo `src/`.

## 8. Artefactos

`scratchpad/r47_snd/`:

- `base/` — copias de entrada de los siete fuentes mas `ENVIRO_AEMS.h`;
- `m.py` — arnes de SourceList SOMBRA (no toca produccion) con `fncmp` cargado
  como modulo y parcheado para ver `.over`;
- `mini.py`, `mini2.py`, `mini3.py` — reproducciones minimas de 5-10 s para
  `cStichWrapper::Play`, `CARSFX_Roadnoise` y `CarCustomize` (con `--rtl`
  imprimen refs/len/prioridad y el reparto final de cada pseudo);
- `rtl.py` — volcados `.lreg`/`.greg` recortados a una funcion;
- `gen.py` — generador de variantes con CRLF respetado (P1..P7, E1/E2);
- `hdr.py` — ensayos sobre `ENVIRO_AEMS.h` con restauracion verificada por SHA;
- `v/` — las variantes medidas y sus objetos;
- `logs/` — `fncmp` antes/despues y las tres auditorias.
