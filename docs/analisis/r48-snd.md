# r48 — sonido: dos vedas de varias rondas caen, y las dos por el REPARTO

Fecha: 2026-09-09. Territorio: `zEAXSound2` + `zSpeech` (y `zEAXSound` como gate).
Base: brief de la r48 (`matched` 98,98 %, DOL en `9619ba57`).

## Resultado

| unidad | antes | despues |
|---|---|---|
| `zEAXSound` | 0 funciones / 0 B | **igual — sigue en 0 de 773** |
| `zEAXSound2` | 2 funciones / 2.304... **1.912 B** | **1 funcion / 1.240 B** |
| `zSpeech` | 2 funciones / 912 B | **1 funcion / 596 B** |

**+988 B de `matched` y +2 funciones exactas.**

```
# LoadSpeechBank__Q26Speech7ManagerP17CLUMP_IDX_FILEtagRiT2PQ26Speech11SPEECH_BANK
#                                       target=100.0%  ours=100.0%  size=316/316
# ProcessUpdate__12CARSFX_Turbo         target=100.0%  ours=100.0%  size=672/672
fncmp zEAXSound   0 de 773   (igual)
fncmp zEAXSound2  1 de 930 -- 1.240 B   (antes 2 -- 1.912 B)
fncmp zSpeech     1 de 703 --   596 B   (antes 2 --   912 B)
audit zEAXSound  exit 0  773 ok / 0 FALLA   (773 antes)
audit zEAXSound2 exit 0  929 ok / 0 FALLA   (928 antes)
audit zSpeech    exit 0  702 ok / 0 FALLA   (701 antes)
lcfix.py --check  todas las entradas @lc estan al dia
```

Dos ficheros tocados, los dos de mi territorio:

- `src/Speed/Indep/Src/EAXSound/Stream/SpeechManager.cpp` (cierre, LF puro)
- `src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Turbo.cpp` (cierre, CRLF puro)

`CARSFX_Roadnoise.cpp`, `RoadblockFlow.cpp`, `STICH_PlayBack.cpp` y
`ENVIRO_AEMS.h` terminan **byte a byte** iguales a la entrada (SHA-256 en §6).
**`slipBoost` sigue en su sitio.**

---

## 1. CERRADA — `LoadSpeechBank` (316 B). La condicion del `while` tenia TRES clausulas

La veda de r46/r47 decia «no es `loop.c` ni `gcse`, es `cse-follow-jumps`». Es
verdad como sintoma y es un callejon como frente: `-fno-cse-follow-jumps` da los
316 B exactos pero con **60 instrucciones distintas**, y `-fno-gcse`,
`-fno-cse-skip-blocks`, `-fno-force-addr`, `-fno-move-all-movables` y
`-fno-rerun-cse-after-loop` no mueven el tamano.

**Lo resolvio `lmap.py` sobre el ORIGINAL en una lectura.** La linea 981 del
fuente original se lleva TRES comparaciones seguidas:

```
800D5..  mr r4, r11        SpeechManager.cpp:981
         slwi r9, r10, 4   SpeechManager.cpp:981
         lwzx r0, r11, r9            (item[i].key)
         cmpw r0, r7                 (== key ?)   -> found
         beq .L_8028C670
         cmpw r10, r5                (i != lower)
   .L:   beq .L_8028C618
         cmpw r10, r8                (i != upper)
         bne .L_8028C5D0             (vuelta al cuerpo)
```

y la 982, **otra** comparacion de clave (la de despues del bucle). O sea: la
prueba de la clave **esta dentro de la condicion del `while`, primera de tres**,
y el cuerpo no la lleva. Nuestro fuente la tenia como un `if (…) goto found;` al
final del cuerpo.

```c
-        while ((i != lower) && (i != upper)) {
+        while ((index->item[i].key != key) && (i != lower) && (i != upper)) {
             if (index->item[i].key < key) { lower = i; } else { upper = i; }
             i = (lower + upper) >> 1;
-            if (index->item[i].key == key) { goto found; }
         }
```

**`fndiff` = 100,0 %, 316/316 B, a la primera compilacion.**

Con esa forma, `cse-follow-jumps` deja de ser un problema y pasa a ser la
explicacion del preambulo: CSE sigue el `b` que entra al bloque de condicion,
sabe por el `if` que envuelve al bucle que `item[i].key != key` ya es cierto,
**se salta la prueba de la clave** y, como `lower` vale 0, pliega
`cmpw r10,r5` en `cmpwi r10,0`. De ahi el `cmpwi r10,0x0` + `b` que salta al
`beq` de en medio del bloque de condicion. Con nuestra forma antigua ese mismo
mecanismo producia una instruccion menos (312 B).

Medido, sobre la misma base:

| # | forma | B |
|---|---|---:|
| base | prueba de clave en el cuerpo | 312 |
| **L1** | **prueba de clave 1.ª clausula del `while`** | **316 — IDENTICA** |
| L2 | `do { … } while (clave && i!=lower && i!=upper);` | 288 |
| L3 | prueba de clave ULTIMA clausula | 320 |

Se retiran las seis lineas de `// VEDA r47` de `SpeechManager.cpp`.

---

## 2. CERRADA — `ProcessUpdate__12CARSFX_Turbo` (672 B). Era el REPARTO, no la planificacion

r46 la dio por «cerrada por construccion» y r47 lo confirmo releyendo
`rank_for_schedule`: al `mr. r7,r3` (la prueba `Az < 0` del `SetAzimuth` inline)
le falta prioridad para llegar a su ranura. La tabla de dependencias de sched2
(`-dR -fsched-verbose-5`) lo dice con numeros:

```
;;   insn  code  bb  dep prio cost  units          : dependientes
;;    89   512   0   0   23    2    lsu            : 137 91      lwz r11,0x60(r31)
;;    91    94   0   1   21    4    [iu2 imuldiv]  : 137 93      mullw
;;    93   230   0   1   17    1    iu2            : 137 575     srawi
;;   113   512   0   0   14    1    iu2            : 137 578     lis r8,0x4330
;;   114   506   0   0   12    1    iu2            : 137 583 117 lis r9,@ha
;;   136   514   0   0    2    1    [iu iu2]       : 137         <-- el `mr.`
```

**Prioridad 2 contra 14: el `mr.` no puede subir por planificacion.** Su unico
dependiente es la rama final del bloque, y una comparacion que alimenta la rama
final de su propio bloque vale siempre `1 + prio(rama)` = 2.

La palanca esta un paso mas atras. El objetivo pone el resultado de
`(BlowoffVol * nDMixOut) >> 15` en **r3** (`srawi r3,r11,15`) y nosotros en r30
(`srawi r30,r11,15`). El `.lreg` explica por que no puede ser el mismo pseudo:

```
Register 86 used 5 times across 20 insns; set 3 times; user var;
            dies in 2 places; CROSSES 2 CALLS;  ->  r30
Register 87 used 4 times across 14 insns; set 2 times; user var;  ->  r3   (Az)
```

`nDMixOut` **cruza dos llamadas**, asi que vive en r30 de principio a fin, y al
reasignarlo el desplazamiento aterriza tambien en r30. Que el objetivo use dos
registros duros distintos significa que el original usa **dos pseudos**. Y el
corte correcto no es inventar una variable nueva aguas arriba —eso es T1 y
empeora— sino **asignar el desplazamiento a la variable de aguas abajo**, que la
conversion a flotante reasigna acto seguido:

```c
-        nDMixOut = (this->BlowoffVol * nDMixOut) >> 15;
-        int TmpBlowoffVol = static_cast<int>(static_cast<float>(nDMixOut) * this->m_BlowoffRampDown.GetValue());
+        int TmpBlowoffVol = (this->BlowoffVol * nDMixOut) >> 15;
+        TmpBlowoffVol = static_cast<int>(static_cast<float>(TmpBlowoffVol) * this->m_BlowoffRampDown.GetValue());
```

Un pseudo corto que no cruza ninguna llamada -> **r3**, igual que el objetivo
(`srawi r3` en la linea 263 y `lwz r3,0xc(r1)` en la 265; nosotros teniamos r30
y r9). **`fndiff` = 100,0 %, 672/672 B, 0 filas.**

### Y ese cambio de registro es lo que mueve al planificador

Con el desplazamiento en r3, la insn 93 **escribe** el registro que la 136
**lee**, asi que sched2 ve una **antidependencia 136 -> 93**. El coste de una
antidependencia es 0, luego

```
prio(136) = max(1 + prio(137), 0 + prio(93)) = max(2, 17) = 17
```

y con 17 el `mr.` sale por delante del `lis r8,0x4330` (14) y se coloca en la
segunda ranura del bloque, exactamente donde lo tiene el objetivo. Las tres
permutas de registro que venian «de propina» (r3/r30, r0/r9) desaparecen con el.

**Regla nueva y general: una diferencia de PLANIFICACION dentro de un bloque
puede tener su raiz en el REPARTO.** Antes de escribir una veda de planificacion,
mira los `dependientes` de la tabla de `-dR -fsched-verbose-5`: si a la insn
rezagada le falta un dependiente respecto al objetivo, lo que hay que mover es el
registro, no la prioridad.

### Lo medido y negativo (con cifra)

| # | forma | B | filas |
|---|---|---:|---:|
| base | `nDMixOut` reasignado | 672 | 11 |
| T1/T2/T3 | variable nueva `nDMix` para el desplazamiento | 672 | 19 |
| T4/T5 | `register int nDMix asm("r3")` | **652** | — |
| T6 | `asm("" : : "r"(Az))` tras el desplazamiento | 672 | 21 |
| T7/T8 | `asm` tras la conversion | 672 | 25 |
| T9 | `register int Az2 asm("r7") = Az` | 672 | 11 |
| TA / TB | pin de `nDMix` a r0 / r9 | 672 | 19 / 20 |
| **TC/TD/TE** | **el desplazamiento a `TmpBlowoffVol`** | **672** | **0** |

El pin duro a r3 (T4) hace justo lo contrario de lo que hace falta: obliga a
sacar `Az` de r3 nada mas volver de la llamada (`mr r10,r3` de mas).

---

## 3. VEDA con numeros — `Setup__Q26Speech13RoadblockFlow` (596 B, 4 filas)

Las cuatro filas son **una sola decision** repetida (cada consumidor sigue a su
productor) dentro del talon de la llamada virtual:

```
objetivo: lwz r9,0(r31); lwz r0,0x2e4(r9); lha r3,0x2e0(r9); mtlr r0; add r3,r31,r3; blrl
nuestro:  lwz r9,0(r31); lha r3,0x2e0(r9); lwz r0,0x2e4(r9); add r3,r31,r3; mtlr r0; blrl
```

`-dR -fsched-verbose-5` sobre el bloque 23:

```
;;   insn  code  bb  dep prio cost  units : dependientes
;;    469  512   0   1   10    2    lsu   : 484 480 476     lwz r9,0(r31)   (vptr)
;;    476   48   0   3    8     2    lsu   : 484 482        lha r3,0x2e0(r9) (delta)
;;    480  512   0   2    8     2    lsu   : 484 573        lwz r0,0x2e4(r9) (pfn)
;;    482   52   0   3    6     1    iu2   : 484            add r3,r31,r3
;;    573  512   0   2    6     3    bpu   : 484            mtlr r0
;;    484  605   0   9    2     1    bpu   : 486
;;   Ready list (t = 6):    480  476        -> escoge 476
```

**El empate es EXACTO en todos los niveles de `rank_for_schedule`**
(`haifa-sched.c:4158`):

1. `INSN_PRIORITY`: **8 = 8**.
2. `INSN_REG_WEIGHT`: **no se consulta** — la linea es
   `if (!reload_completed && (weight_val = …))`, y sched2 va con
   `reload_completed`. (Confirma por fuente la nota «el nivel 2 solo actua en
   sched1».)
3. Interbloque: no aplica, la region es `b 23 bb 0` (un bloque).
4. Clase respecto al `last_scheduled_insn` (469): dependencia de datos con coste
   2 en los dos -> clase **1 = 1**.
5. `depend_count`: **2 = 2**.
6. `INSN_LUID`: gana el menor, que es 476.

Y el LUID lo fija el orden que dejo **sched1**, donde la asimetria SI es real:
alli `prio(476) = 8` y `prio(480) = 6`, porque antes de reload el `mtlr` no
existe y el pfn va directo a la llamada. Es decir, la asimetria de sched1 es
estructural del modelo de costes de rs6000 y sched2 hereda su orden.

`-fno-schedule-insns2` -> 6 filas. `-fno-schedule-insns` -> 21 filas.

### Ensayado ahora: el `asm` de SOLO ENTRADA (lo que pedia el encargo)

Las ocho formas dan **exactamente el codigo base** (596 B, mismas filas):

| # | forma | filas |
|---|---|---:|
| A1 | `asm("" : : "r"(primary))` antes de la llamada virtual | 4 (= base) |
| A2 | `asm("")` antes | 4 |
| A3 | `asm("" : : "r"(primary))` despues | 4 |
| A4 | `asm("")` despues | 4 |
| A5 | `asm("" : : "r"(ai))` antes | 4 |
| A6 | cachear el dispatch en una local | 4 |
| A7 | `asm("" : : "r"(primary) : "r0")` | 4 |
| A8 | `asm("" : : "m"(*primary))` | 4 |

**Y la razon de que sea asi es la misma que cierra la veda**: el empate esta
DENTRO de la secuencia de seis instrucciones que el front-end emite para la
llamada virtual, y ningun `asm` de fuente puede meterse entre ellas. La veda
queda cerrada con causa medida, no con «no se me ocurre otra forma»: para abrirla
haria falta que el pfn tuviese un dependiente mas o el delta uno menos, y ninguno
de los dos es expresable desde C.

---

## 4. REVERTIDA por la regla 9 — `GenerateRoadNoise` (1.240 B): de 33 filas a 16 con cero bytes

**No se retiene nada** (la funcion sigue en 33 filas / 97,1129 %), pero el frente
queda partido en dos mitades y una de ellas resuelta.

La mejor forma medida, **16 filas** y 97,2419 %:

```c
    float slipBoost = 0.1f;
    fRightVol = fRightVol + fRightVol * slipBoost;   // la acumulacion ANTES del bLength
    __asm__("");                                      // barrera de ranura, 0 B
    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip);
```

Arregla **la mitad del problema**: la primera acumulacion
(`fmadds f29,f31,f11,f31`) cae en la ranura del objetivo y `ftemp` conserva f11
—en la base la temporal del `bLength` se lleva f11 y la acumulacion se va 12
ranuras mas tarde—. Lo que NO arregla: la segunda acumulacion
(`fmadds f29,f29,f10,f29`) tiene que ser la **ultima** instruccion del bloque
previo al `bso` (fila 114/115 del objetivo), y la barrera la clava en la
**primera** (fila 102). Sin barrera, la funcion entera permuta f11<->f10.

Como el tamano ya era exacto y no desaparece marco ni derrame, es deuda: se
revierte y se deja la medida.

### El barrido de posicion de la barrera (lo unico que decide)

Orden `ACC1; slipBoost; ACC2; bLength; *0.01f` — barrera en cada hueco:

| hueco | `asm("")` | `asm("" : : "f"(slipBoost))` |
|---|---:|---:|
| 0 (sin barrera / antes de ACC1) | 51 | — |
| 1 (tras ACC1) | 20 | — |
| 2 (tras la declaracion) | 21 | 21 |
| **3 (tras ACC2, antes del `bLength`)** | **16** | **16** |
| 4 (tras el `bLength`) | 51 | 1.244 B |
| 5 (tras el `*0,01f`) | 54 | 24 |

Orden base (`ACC2` DESPUES del `bLength`): 33, 34, 31, 33, 30, 37. La barrera
sobre `ftemp` o sobre `fLeftVol` en el hueco 3 da tambien 16 (Y5): **manda la
POSICION, no la variable que nombra.**

### El resto, todo negativo y con cifra

| # | forma | resultado |
|---|---|---|
| N1/N2/N3/N5/N7 | `slipBoost` declarada **despues** del `bLength` | **1.232 B** (CSE funde el 0,1f: se pierden los 8 B) |
| N4/N6 | reordenes que la dejan antes | 1.240 B, 33 filas |
| P1/P2/Q1/Q2 | `register float slipBoost asm("fr10"/"fr8")` | 33 / 31 filas |
| P3/P4/P5 | `asm` de solo entrada tras la declaracion, orden base | 31 filas |
| P9 | `volatile float slipBoost` | 1.248 B |
| PA | `const float slipBoost` | 1.232 B |
| W1..W9 | barrera **selectiva** `asm("" : "+f"(…))` | mejor 22 filas |
| X1..X6 | `register float ftemp asm("fr11")` | **109-116 filas** |

El pin de `ftemp` es catastrofico porque `ftemp` se reutiliza en toda la funcion
(pseudo 92: 34 usos, 17 asignaciones): **un pin sobre una variable reciclada no
es una palanca, es una granada.** Y la barrera selectiva no vale aqui: lo que
arregla el reparto de f11 es la volatilidad de `asm("")`, no la atadura.

Unas 80 formas medidas en total. El frente que queda es de una sola frase:
**meter la segunda acumulacion en el bloque del `bLength` y que se planifique la
ultima**, sin barrera delante del `bLength`.

---

## 5. Propuestas y avisos fuera de mi territorio

1. **`lmap.py` sobre el ORIGINAL es el primer paso, no el ultimo.** `LoadSpeechBank`
   llevaba dos rondas de arqueologia de banderas de compilador y la respuesta
   estaba en que la linea 981 se lleva tres comparaciones. **Cuando un near-miss
   tiene un bucle, cuenta cuantas comparaciones cuelgan de la linea de la
   condicion antes de tocar nada.**
2. **Palanca nueva: «la variable que cruza la llamada no puede ser la del
   resultado».** Si el objetivo usa dos registros duros distintos para lo que
   nuestro fuente guarda en una sola variable, el original tenia dos, y el corte
   correcto suele ser **reusar la variable de aguas abajo**, no inventar una de
   aguas arriba. Se detecta a un grep: la linea `crosses N calls` del `.lreg`
   nombra al culpable.
3. **Palanca nueva: la antidependencia que fabrica el reparto sube
   `INSN_PRIORITY` de 2 a la del escritor** (coste 0 en las antidependencias).
   Un rezagado de planificacion dentro de un bloque puede tener la raiz en el
   asignador. Mirar la columna `dependientes` de `-dR -fsched-verbose-5` antes de
   escribir una veda de planificacion.
4. **`rank_for_schedule`, desglosado y comprobado por fuente**: en sched2
   (`reload_completed`) el nivel de `INSN_REG_WEIGHT` **se salta**; despues van
   los niveles interbloque (solo si `INSN_BB` difiere), la clase respecto al
   `last_scheduled_insn` (3 independiente > 2 anti/salida > 1 datos), el numero
   de dependientes y, por ultimo, `INSN_LUID` —y el LUID que ve sched2 es el
   orden que dejo sched1, no el de la fuente—.
5. **`-fsched-verbose-5` existe y es lo que hace falta.** Se escribe asi
   (`fix_sched_param`, `toplev.c:5372`) e imprime la tabla
   `insn code bb dep prio cost blockage units` mas los dependientes de cada insn.
   **`rtldump.py` NO lo pasa**: filtra por `KEEP` sobre los cflags de la unidad,
   no sobre `argv`. Un `--extra` de una linea lo arreglaria y convertiria
   cualquier veda de planificacion en una tabla de numeros.
6. **`-dS`/`-dR` provocan un ICE de GCC 2.95 en algunas funciones**
   (`_STL::_Rb_tree<…>::_M_erase` y `Speech::Manager::ServiceInterruptEvents`),
   pero **el volcado ya esta escrito** para todo lo compilado antes del ICE: si
   la funcion que te interesa va antes, sirve igual. Conviene anotarlo en
   `rtldump.py` para que nadie lo de por imposible.
7. **Aviso de infraestructura: el disco C: llego a 0 B libres durante la ronda.**
   Un volcado `-dS`/`-dR`/`-dg` de una SourceList grande ocupa 20-30 MB y el `.s`
   otros 20. Conviene que cada agente borre `scratchpad/rtl/*.s` y
   `scratchpad/rtl/*.i.*` al terminar; yo he dejado solo mis 2,9 MB.
8. **Reproducciones minimas**: `CARSFX_Turbo.cpp` y `CARSFX_Roadnoise.cpp`
   compilan SUELTOS con los cflags de `zEAXSound2` y reproducen las funciones
   **fila a fila** (6-10 s contra 20-25 s de la SourceList). `RoadblockFlow.cpp`
   no compila suelto (choque `SpeechManagerLite.h` / `SpeechManager.hpp`), pero
   una SourceList RECORTADA (cabeceras + `SpeechManager.hpp` + el `.cpp`)
   reproduce el talon exacto en 8 s.

---

## 6. Verificacion

```
build_direct.py zEAXSound zEAXSound2 zSpeech            3 ok, 0 fallidas
fndiff  LoadSpeechBank__…SPEECH_BANK      100.0 %   316/316
fndiff  ProcessUpdate__12CARSFX_Turbo     100.0 %   672/672
fndiff  GenerateRoadNoise__16CARSFX_RoadNoise  97,1129 %  1240/1240 (sin cambios)
fndiff  Setup__Q26Speech13RoadblockFlow    98,255035 %  596/596   (sin cambios)
fncmp   zEAXSound   0/773   (antes 0)   -- gate cumplido
fncmp   zEAXSound2  1/930   (antes 2)
fncmp   zSpeech     1/703   (antes 2)
audit   zEAXSound 773 ok/0 FALLA · zEAXSound2 929 ok/0 FALLA · zSpeech 702 ok/0 FALLA
lcfix.py --check    limpio
git diff --check    limpio
git status          solo SpeechManager.cpp y CARSFX_Turbo.cpp modificados
```

Ninguna funcion empeora en ninguna de las tres unidades. `zEAXSound` no se ha
tocado (ni `ENVIRO_AEMS.h` ni `STICH_PlayBack.cpp`) y sigue en cero.

SHA-256 (16 primeros) al terminar:

```
SpeechManager.cpp     7c5b42e90defae69   (cierre, LF puro, 2.428 lineas)
CARSFX_Turbo.cpp      c08ea3da2966efc0   (cierre, CRLF puro, 301 lineas)
CARSFX_Roadnoise.cpp  e513d139636e6bd2   = base, sin tocar
RoadblockFlow.cpp     d76d681f8d0df1ad   = base, sin tocar
STICH_PlayBack.cpp    91faad4fea18d45f   = base, sin tocar
ENVIRO_AEMS.h         9ce8382d1c380c2b   = base, sin tocar
```

No queda ningun fichero `__r48*` ni `__r47*` bajo `src/`.

## 7. Artefactos

`scratchpad/r48_snd/` (2,9 MB):

- `base/` — copias de entrada de los seis fuentes;
- `m.py` — arnes de SourceList SOMBRA (no toca produccion), con `fncmp` cargado
  como modulo;
- `mini.py` — reproduccion minima de UN `.cpp` con los cflags de la unidad
  (`CARSFX_Turbo`, `CARSFX_Roadnoise`, `SpeechManager`);
- `mini2.py` — SourceList RECORTADA para `RoadblockFlow.cpp` (8 s);
- `mini3.py` — sombra completa de `zSpeech` con `ServiceInterruptEvents` vaciada,
  para esquivar el ICE de `-dS`/`-dR`;
- `mrtl.py`, `rtl.py` — volcados RTL recortados a una funcion, con flags extra;
- `fd.py` — `fndiff` contra un objeto arbitrario;
- `gen_lsb.py`, `gen_tb.py`, `gen_rn.py`, `gen_rn2.py`, `gen_rn3.py`,
  `gen_rn4.py`, `gen_rn5.py`, `gen_rbf.py` — generadores de variantes con CRLF
  respetado;
- `run_*.py` — medidores;
- `logs/` — `fncmp` antes/despues y las tres auditorias.
