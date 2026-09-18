# Ronda 27 — zEagl4Anim + zEAXSound

Encargo: cerrar `Initialize…DynamicLoader` (28 filas), `SetupNextLoad` (2 filas) y
los muros de las dos unidades.

## 0. Línea base VERIFICADA

`build_direct.py zEagl4Anim zEAXSound` + `triage.py --muro`, medidos por mí:

    2352 B  98.876%  zEagl4Anim  Initialize__…DynamicLoader   faltan 3, sobran 4, 7 SUST
    2152 B  91.816%  zEAXSound   Play__14cSampleWarpper       muro, dmax 7
    1480 B  99.459%  zEagl4Anim  EvalSQT__…FnStatelessQ       sobran 2 clrlwi
    1008 B  98.671%  zEAXSound   SetupNextLoad                muro, dmax 2
     896 B  98.170%  zEAXSound   UpdateParams__16SFXCTL_MasterVolf   49 registros
     720 B  97.361%  zEagl4Anim  FindMatchTime                muro, dmax 7
     716 B  99.804%  zEAXSound   UpdateRPM                    4 registros
     488 B  98.852%  zEagl4Anim  Eval__…FnRawLinearChannel    22 registros
     456 B  93.509%  zEagl4Anim  EvalState__…FnRawStateChan   faltan 2, sobran 2
     384 B  99.688%  zEAXSound   Play__13cStichWrapper        4 registros
     356 B  94.303%  zEAXSound   startnextrequest             muro, dmax 2
     320 B  94.487%  zEAXSound   DataLoadCB                   muro, dmax 2
     268 B  96.761%  zEAXSound   MsgPlayMiscSound             muro, dmax 6
     168 B  90.238%  zEAXSound   ResolveCurrentDataMemory     muro, dmax 2

**Coincide con el encargo instrucción a instrucción.** `audit.py` al empezar:
**zEagl4Anim 313 ok / 0 FALLA**, **zEAXSound 764 ok / 0 FALLA**.

---

## 1. RESULTADO

| | antes | después |
|---|---|---|
| `Eval__Q29EAGL4Anim18FnRawLinearChannelffPf` (488 B) | 98,85246 % | **100 % — CERRADA** |

`measure.py --cmp` (base y medida SEGUIDAS, `build_direct.py` antes de cada una):

    +488 B, +1 funciones, 1 unidades cambian
       +488 B  +1 fns  Speed/Indep/SourceLists/zEagl4Anim   107520 -> 108008

`pctsnap.py --cmp`: **EMPEORAN: ninguna en mis unidades**; +1,147 pp en `Eval`.
(Las otras dos filas que salen —zAI y zDynamics— son de otros agentes: mis dos
ficheros sólo los incluye zEagl4Anim.)

`audit.py` al cerrar, **dos pasadas**: **zEagl4Anim 314 ok / 0 FALLA**,
**zEAXSound 764 ok / 0 FALLA**. `Eval` audita entera: **488 B, 15 ramas,
19 relocs, 4 literales**. `frozen.py`: **zEagl4Anim re-congelada**
(`54b6788f9055615a`). **zEAXSound NO la he re-congelado**: dice «HA CAMBIADO»
pero mide exactamente lo mismo que dejó la r26 (144.824 B / 764 fns) y no la he
tocado — es el ruido documentado de agentes en paralelo.

**Ficheros tocados, dos:** `src/Speed/Indep/Src/EAGL4Anim/RawLinearChannel.h` y
`src/Speed/Indep/Src/EAGL4Anim/FnRawLinearChannel.h`. Ningún ensamblador.
Sin commit.

---

## 2. `Eval__…FnRawLinearChannel` — CERRADA (488 B), y es el patrón de `QueueFileLoad`

La r26 dejó esta función «sin diagnóstico» porque `regmap` emparejaba mal el
homónimo. **Filtrando por `low_pc` el volcado original lo dice todo**
(`symbols/mw_dwarfdump.nothpp:414442`, rango `0x800A54FC..0x800A56E4`):

    inline void FnRawLinearChannel::Eval(float, float currentTime, float * output) {
        inline RawLinearChannel * FnRawLinearChannel::GetRawLinearChannel() {}
        inline void RawLinearChannel::Eval(float frameTime, float * output, bool interp) {
            int frame;
            inline int FloatToInt(float f) {}
            inline void RawLinearChannel::EvalFrame(int frame, float * output) {
                unsigned short * dofIdx; // r8
                float * frameData;
                /* anonymous block */ { int i; /* r10 */ }
            }
            ... (EvalFrame otras dos veces, EvalInterpFrame una)
            /* anonymous block */ { float t; // f2 ... }
        }
    }

Es decir: **el original NO escribía el cuerpo en `FnRawLinearChannel::Eval`;
delegaba en `RawLinearChannel::Eval`, que a su vez llamaba a `EvalFrame` y
`EvalInterpFrame`** — y las tres estaban **en nuestro árbol como cuerpos vacíos**
(`RawLinearChannel.h`, `{}`), igual que le pasó a `QueueFileLoad` en la r26 con
`DeleteRefToAsset`. `lmap.py` lo confirma: las 488 B llevan líneas de
`RawLinearChannel.h:63,72,73,81,88,103,170-178,186-195,203-226` y
`AnimUtil.h:93`, no del `.cpp`.

Y `RawPoseChannel.cpp` ya tenía escrito **el hermano exacto** de
`RawLinearChannel::Eval` (mismo `FloatToInt`, mismo `if/else if/else`, mismas
cuatro llamadas), así que era transcribir.

### Los cuatro ensayos, con su cifra

| | cambio | % | tamaño |
|---|---|---|---|
| base | cuerpo abierto en `FnRawLinearChannel::Eval` | 98,85246 | 488 |
| **c1** | delegar + `Eval(…, bool interp)` + `for (int i…)` | **0,00000** | 308 |
| **c2** | igual pero `int interp` | **1,64286** | 280 |
| **c3** | igual + **`EvalFrame` declarada ANTES que `Eval`** | **74,18104** | 464 |
| **c4** | igual + los tres bucles como `int i=0; if(…) do{…}while(…);` | **100,00000** | **488** |

**Los tres hallazgos que la cierran, y ninguno es de reparto:**

1. **`bool interp` cuesta 6 instrucciones.** El volcado dice `bool interp`, pero
   nuestro `mInterp` es `int` y GCC 2.9 normaliza `int`→`bool`
   (`li r3,1 / cmpwi r0,0 / bne / li r3,0`). El objetivo hace `lwz r5,0x10(r3)` y
   lo pasa tal cual: **el parámetro tiene que ser `int`**. (El volcado del
   original dice `bool mInterp; // offset 0x10, size 0x1` y aun así emite `lwz`;
   la anotación de tamaño del volcado no es fiable para `bool`.)
2. **El ORDEN de declaración dentro de la clase decide si GCC hace inline.**
   Con `EvalFrame` declarada DESPUÉS de `Eval` (como estaba nuestra cabecera),
   GCC 2.9 **emite `EvalFrame` fuera de línea y pone un `bl`**
   (`bl EvalFrame__Q29EAGL4Anim16RawLinearChanneliPf`, 280 B). El volcado da el
   orden del original —`EvalFrame`, `EvalInterpFrame`, `Eval`— y con ese orden
   se expande todo: **de 1,6 % a 74,2 %**. *Regla nueva: en GCC 2.9 los cuerpos
   en clase se «terminan» en orden de declaración; un inline sólo entra en quien
   se compila DESPUÉS. El orden de miembros del volcado no es cosmético.*
3. **`for (int i = 0; i < n; i++)` no es el bucle del objetivo.** Da prueba
   arriba y salto al fondo; el objetivo tiene **guarda + `do/while`**
   (`li r10,0 / cmpw / bge FIN / … / lhz / cmpw / blt`). La forma
   `int i = 0; if (i < GetNumDOFs()) { do { … i++; } while (i < GetNumDOFs()); }`
   —la que ya tenía nuestro cuerpo abierto— cierra las 24 B que faltaban.
   El bloque anónimo de `int i` que pide el volcado sale igual con las dos formas
   (los rangos coinciden).

---

## 3. `Initialize` (2.352 B): el árbol del `switch` es VEDA, ahora con el mecanismo al dígito

Las 28 filas siguen en los tres grupos de la r26 (A: 6 filas de orden de
operandos; **B: 15 filas del árbol del `switch`**; C: 7 filas de `t`/`nameLength`).
**El grupo B es la puerta**: como `matched_code` es todo-o-nada, mientras no caiga,
A y C no valen un byte. Y no cae.

### 3.1 El caso mínimo reproduce nuestro árbol en 2 s

`c27ae_m0.cpp` (17 líneas, mismo juego de `case`, mismos cflags) da **exactamente**
nuestra forma. Con eso he barrido **34 formas** en vez de recompilar la unidad.

### 3.2 Lo que el objetivo hace y nosotros no

    OBJETIVO                          NUESTRO
    cmpwi 8 ; beq BREAK               cmpwi 8 ; beq BREAK
    cmplwi 8 ; ble IZQUIERDO          cmplwi 8 ; bgt DERECHO
    [DERECHO en línea] ; b BREAK      [IZQUIERDO en línea] ; b BREAK
    IZQUIERDO:                        DERECHO:
      cmpwi 3 ; beq STRTAB              cmplw r21 ; bgt BREAK
      cmplwi 3 ; bgt BREAK              cmplw r22 ; bge BREAK
      cmplwi 1 ; ble BREAK              cmpwi 9 ; beq REL
      (CAE en SYMTAB)                   b BREAK              <- la instrucción de más

Volcando la RTL de `expand` (`cc1plus -dr`) se ve que **los dos emiten el mismo
tercer brazo de `emit_case_nodes`** (`bgtu test_label`, IZQUIERDO en línea,
`b default`, `test_label:` DERECHO). La diferencia la hace **`jump.c`**: cuando el
bloque IZQUIERDO termina en `b <etiqueta de case>` en vez de `b <default>`,
invierte el `bgtu` en `ble` y **intercambia los dos bloques**. Reproducido:
`case 0 ... 2` y `case 1 ... 2` para SYMTAB dan ese intercambio exacto.

Y la última fila del subárbol izquierdo (`cmplwi 1 ; ble BREAK` + caída) sólo
sale de un nodo `[?,1]` cuyo hijo derecho `[2,2]` está **acotado por los dos
lados**: el padre emite `GT node->high(1) -> [2,2].code_label` y `jump.c` lo
invierte. O sea: **el original tenía un `case` que cubre el valor 1** (por
ejemplo `case SHT_NULL ... SHT_PROGBITS`).

### 3.3 Por qué es imposible: la aritmética de `balance_case_nodes`

Verificada al dígito contra el compilador (predice el nodo raíz de 4 juegos de
`case` distintos, incluido el `[6,8]` que sale al fusionarse `6..7` con `8`):

    i = (n_nodos + n_rangos + 1) / 2 ;  se avanza restando 1 por nodo y 2 por rango

Para que la raíz siga siendo `[8,8]` **y** el subárbol izquierdo tenga raíz
`[3,3]` con `[0,1] -> [2,2]` colgando a su izquierda hacen falta a la vez:

* coste acumulado hasta `[3,3]` = 4 → `i_izq = 4` → `n_izq + rangos_izq ∈ {7,8}`,
* y el conjunto izquierdo es exactamente `{[0,1], [2,2], [3,3], [4,7]}` →
  `n_izq = 4` y **como máximo 2 rangos** (`[2,2]` y `[3,3]` son de valor único).

**6 ≠ 7.** No hay lista de `case` que lo produzca: partir `[4,7]` para subir el
coste rompe que `[4,7]` sea el hijo derecho *acotado* que exige el
`cmplwi 3 ; bgt BREAK`. **El árbol del objetivo no sale de un `switch` con esos
límites.**

### 3.4 Veda del grupo B: 34 formas medidas esta ronda (todas idénticas o peores)

Sentencia barrida: **la lista de `case` del `switch (sheader->sh_type)`**.

`default:` fuera · `default:` primero · `case 8` sin rango · `case 4…7` con
`break` · `case 0x70000007…` con `break` · `case 8` con `continue` ·
`case 0x70000005…6` con `continue` · orden de cuerpos invertido · SYMTAB el
último · `SHT_HIUSER` = `0x8FFFFFFF` · sin el rango alto · sin `case 4…7` ·
`4…7` y `8` con la misma etiqueta · `case 0…1` con `break` y con `continue` ·
`case 9` como rango · `case 2…2`, `3…3`, `9…9` como rangos · `case 0…2`,
`1…2`, `2…3` para SYMTAB · `case 1:` y `case 0:` sueltos · `case 10…0x70000004`
en cinco combinaciones · partir `4…7` en `4…5`/`6…7` y en `4`/`5…7` · un tercer
rango alto · un `if (x==8) break;` delante del `switch`.

**Sumadas a los 88 árboles y 512 listas de la r20/r21 y a las 3 de la r26: el
grupo B está cerrado.** Los grupos A (8 formas vedadas en la r26) y C (6 formas)
siguen vedados y **no los he vuelto a tocar: no valen bytes mientras B esté vivo.**

---

## 4. `SetupNextLoad` (1.008 B): la prueba de `lmap.py` sale NEGATIVA

Era el encargo explícito («mira con `lmap.py` de qué fichero son las líneas de las
dos filas atascadas»). Hecho:

    800AB4E8  addi r28, r26, 0x1     _list.h:89
    800AB4EC  slwi r27, r26, 2
    800AB4F0  lwz r0, 0x0(r9)        _list.h:120      <- la fila que se mueve
    800AB4F4  lis r26, gFastMem@ha   _list.h:470      <- y ésta

Histograma completo del rango: `_list.h` (19×89, 13×120, 6×470…),
`EAXAemsManager.h:66-88`, `attribhash.h`, `UStandard.h`, `SndDataParams.hpp`, y
del `.cpp` sólo las líneas 911-925. **O sea: el original tampoco escribía el
bucle en el `.cpp` — llamaba al inline de la cabecera, y eso es exactamente lo que
ya hace nuestro `SetupNextLoad` desde la r26** (`mWaitForResolve.DeleteRefToAsset`).
**No falta ninguna sentencia.**

Lo que queda son 23 filas y son **una rotación de registros** (objetivo→nuestro:
`this` r29→r27, contador r26→r31, índice r27→r28, temporal r28→r29/r7) **más un
único intercambio de planificación** (el objetivo pone `lwz r0,0(r9)` antes de
`lis r26,gFastMem@ha` y nosotros después). `alloc.py` sobre el `.greg` da la tabla
de allocnos (54 pseudos llegan a `global_alloc`; la fórmula CON `size` reproduce
el orden del compilador) y **el objetivo comparte r26 entre el contador y
`gFastMem`**, que en el nuestro son r31 y r26.

Diagnóstico de banderas (sólo diagnóstico): `-fno-schedule-insns` 92,63095 ·
`-fno-schedule-insns2` 97,39286 · `-fno-gcse` 97,67461. **Ninguna acerca.**
Cero ensayos de fuente: no hay sentencia que mover.

---

## 5. `DataLoadCB` (320 B): la causa localizada, y NO es el planificador

Las 12 filas son una sola cosa: el objetivo iza `lis r29, StartBankLoadTicks@ha`
**por encima de `bl bGetTicker__Fv`** (y por eso abre `stwu r1,-0x18` + `stmw r29`,
un registro salvado más que nosotros).

Medido esta ronda: **con `-fno-schedule-insns` nuestro `lis` sigue DESPUÉS de la
llamada** (92,42500 %). O sea **`sched1` no lo está bajando: nunca estuvo arriba**.
La RTL de `expand` lo pone después porque `expand_assignment` deja el
`(high (symbol_ref))` dentro del `emit_move_insn` final. **Queda por saber qué
forma de fuente hace que la dirección se legitime ANTES de evaluar la llamada.**
`regmap` dice IDÉNTICO (mismas locales, mismo reparto), así que no es estructura.

---

## 6. `EvalState` (456 B): tercera forma medida, tercer negativo

`regmap` insiste: la única diferencia es que nos **sobra** `int keyIdx` (r10) y al
original le falta el registro de `k` (r4 en el original). El volcado
(`mw_dwarfdump.nothpp:411398`) da las locales exactas: `c` r30, `i` r31, `k` r4,
`dest` sin registro. **Nada más.**

**Ensayo e1** (nuevo, no estaba en la r26): quitar `keyIdx` **y** reusar `i` desde
el principio —

    i = mKeyIdx;
    if (time >= *(float *)c->GetKeyData(i)) { while (i < c->GetNumKeys()) …
    } else { for (i = i - 1; i >= 0; i--) … }

**83,45132 % (452 B) — PEOR.** Revertido. Con las dos de la r26 (usar `mKeyIdx`
directo: 89,868; reusar `i` de otra forma: 83,412) van **tres formas** y es el
**octavo caso de la regla del §2 del brief**: la local de más está haciendo
trabajo real.

Lo que sí se ve en el `lmap` y explica las 2 filas: el objetivo **iza fuera del
bucle** el `andi. mNumFields,1` (lo guarda en `cr7` con `mcrf cr7, cr0`) y la base
`add r11, r30, r9`, ambos de dentro del inline `GetKeyData`; nosotros los
recalculamos en cada vuelta. Es movimiento de invariantes de `loop.c`, no fuente.

---

## 7. `UpdateParams` (896 B): aviso — hay una falsificación de NOMBRE en nuestro árbol

`regmap --scan` la marca **ESTRUCTURA, «1 local sólo nuestra»**, y esa local es:

    // SFXCTL_MasterVol.cpp, final de UpdateParams
    __typeof__(RandarRange) __ct8 = static_cast<int>(RadarDetector::mStaticRange * 32767.0f);
    RandarRange = __ct8;
    this->SetDMIX_Input(10, __ct8);

**`__ct8` es un nombre que genera GCC para temporales, escrito a mano por una
ronda anterior.** No está en el volcado del original. **Lo declaro como
construcción nuestra (regla §3 del brief)**; no lo he tocado porque el encargo
marca la función como negativo firme y no quería gastar la medida, pero **es el
único desajuste estructural que queda en zEAXSound** y quien la retome debería
empezar por ahí (el otro desplazamiento es `fMasterVol` f6→f11, que arrastra las
49 filas).

---

## 8. `Play__14cSampleWarpper` (2.152 B): diagnóstico, sin ensayos

133 filas. El volcado (`mw_dwarfdump.nothpp:495118`) da **las mismas cinco
locales** (`TempVol` r26, `TempPitch` r28, `TempAz` r23, `PitchScale` f13,
`RefCount` r0) y **el mismo árbol de inlines**: `AddToList` → `_add` →
`push_back` → `reserve` (con su bloque anónimo `oldBuffer/oldSize/oldCapacity` y
el `int ii`), los tres constructores `AEMS_Stich*` con sus diez `SetX` y su
`Result result`, y `GetRefCount` con `int refCount`. **Nuestro árbol ya es ése:
no hay hueco estructural.**

Lo que queda es reparto: el objetivo abre `stwu r1,-0x58` + `stmw r22` (diez
salvados) y nosotros `stwu r1,-0x48` + `stmw r23` (nueve). El registro de más es
**una constante 0 que el objetivo mantiene viva en un registro salvado por bloque**
(`li r25,0` / `li r28,0`) y usa para dos cosas a la vez: el último argumento del
constructor (`filter_HiPass`) **y** `m_eIsPlaying = eSTITCH_PLAY_STATUS_OFF`.
Nosotros rematerializamos `li r0,0` cada vez. Es un izado de constante de
`gcse`/PRE. **Cero ensayos de fuente: no he encontrado la forma que lo provoque.**

---

## 9. `FindMatchTime` (720 B): confirmado IDÉNTICO, y las 8 filas son una pareja replanificada

`regmap`: **15 locales, mismo árbol, MISMO REPARTO**. Las 8 filas son
`lis $LC326@ha` + `lfd f13` (la constante mágica `0x4330…` de la conversión
entera→float) que el objetivo carga **12 ranuras antes** que nosotros, más el
f13↔f12 que arrastra. Según el catálogo eso es **permutador CIEGO**; no lo he
lanzado (seis rondas sin cerrar nada y el presupuesto se fue en el switch).

Detalle curioso del objetivo, por si sirve a quien la retome: en `0x8009E188` hay
un `mr r9, r11` **muerto** (r9 se pisa dos instrucciones después con
`lwz r9, 0x1c(r1)`).

---

## 10. Herramientas dejadas en el scratchpad (prefijo `c27ae_`)

* **`c27ae_d.py <UNIT> <SYM>`** — diff del `.o` YA CONSTRUIDO contra el objetivo,
  con `ALL=1` y `ROWS=lo-hi` para volcar el rango entero. **Izquierda = NUESTRO,
  derecha = OBJETIVO** (al revés que `c26ae_v.py`, que pone el objetivo a la
  izquierda; comprobado contra `lmap`).
* **`c27ae_v.py`** — clon de `c26ae_v.py` con **`EXTRAPRE=`** (banderas y `-I`
  DELANTE de los cflags, que es lo que hace falta para sombrear una cabecera;
  `EXTRA=` sigue detrás).
* **`c27ae_rtl.py <UNIT> <src> <tag> <letras> [args]`** — volcados RTL por pase
  llamando a `cc1plus` a mano, **con `SL=`** para preprocesar la SourceList
  entera con un `.cpp` sustituido.
* **`c27ae_min.py`** / **`c27ae_sw.py`** — caso mínimo con los cflags exactos de
  la unidad (2 s por variante) y extractor de la secuencia del árbol de un
  `switch`. Son los que han cerrado el grupo B.
* `c27sw/` — las 34 variantes del `switch`. `c27ae_*_lmap.txt` — los mapas de
  línea de `SetupNextLoad`, `Initialize`, `Eval`, `EvalState`, `FindMatchTime`.
* Copias de seguridad: `c27ae_RawLinearChannel.h.bak`,
  `c27ae_FnRawLinearChannel.h.bak`, `c27ae_RawStateChan.cpp.bak`.
* Borrados los tres directorios de volcados RTL (34 MB cada uno) y los `.json`/`.o`
  intermedios; el scratchpad queda en **642 MB** y el disco en **16 GB libres**.

### Trampas que han vuelto a morder

* **`RawStateChan.cpp` es CRLF entero**: un `replace` con `\n` falla en silencio
  y la variante sale idéntica (una medida tirada). `splitlines(True)`.
* **`regmap.py --scan --min N` devuelve vacío en zEagl4Anim**; con `--max 99.99`
  tampoco lista sus funciones atascadas, aunque `regmap.py <unidad> "<Clase::Func>"`
  las resuelve bien una a una. En zEAXSound el `--scan` sí funciona.
* **`regmap` empareja por firma, no por `low_pc`**: en `FnRawLinearChannel::Eval`
  cogía un homónimo de 4 parámetros y daba «17 locales sólo nuestras», que es
  ruido. Hay que ir al volcado por `Range: 0x…` (el `low_pc` sale de `lmap.py`).

---

## 11. Qué NO he probado

* **`Initialize`**: no he tocado los grupos A y C (vedados en la r26 y sin valor
  mientras B esté vivo); no he lanzado el permutador; **no he probado a partir la
  función** (la única salida que quedaría para el grupo B sería que el original
  no usara `switch` ahí, y eso es reescribir 2.352 B a ciegas).
* **`SetupNextLoad`**: cero ensayos de fuente (justificado en §4). No he probado
  a mover sentencias fuera del bucle para bajar `n_refs` del contador, que es lo
  que dice el catálogo para romper un empate de `allocno_compare`.
* **`Play__14cSampleWarpper`**: cero ensayos. No he probado formas que fuercen la
  constante 0 a vivir (una local `const int off = 0;`, o pasar
  `eSTITCH_PLAY_STATUS_OFF` como último argumento).
* **`UpdateParams`**: no la he tocado (negativo firme del encargo). El `__ct8`
  queda declarado, no retirado.
* **`FindMatchTime`**: permutador ciego no lanzado.
* **`EvalSQT…FnStatelessQ`**, **`UpdateRPM`**, **`Play__13cStichWrapper`**,
  **`MsgPlayMiscSound`**, **`startnextrequest`**, **`ResolveCurrentDataMemory`**:
  no tocadas (negativos previos). `regmap` las clasifica IDÉNTICO/REPARTO, o sea
  **no queda ni un hueco estructural en zEAXSound salvo el `__ct8` del §7**.
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas.
* **El DOL**: no lo he construido (va en la verificación de la tanda).
* **`configure.py`, `config/GOWE69/*`, `splits.txt`**: intactos. Ningún
  ensamblador. Ningún `#if defined(__ANDROID__)` tocado. Ningún pin nuevo.
  **Ninguna cabecera compartida fuera de las dos de EAGL4Anim**, que sólo incluye
  zEagl4Anim (`FnRawLinearChannel.h` ← `MemoryPoolManager.cpp`;
  `RawLinearChannel.h` ← esas dos y nada más).
