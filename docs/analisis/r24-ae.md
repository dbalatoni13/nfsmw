# Ronda 24 — zEAXSound + zEagl4Anim

Encargo: `Play__9EAXCommon18eMenuSoundTriggers`, `EvalSQT…FnStatelessQ`,
`EvalState…FnRawStateChan`, `Initialize…DynamicLoader` y los muros de las dos
unidades.

## 0. Línea base VERIFICADA — coincide con el encargo instrucción a instrucción

`build_direct.py zEAXSound zEagl4Anim` + `triage.py`:

    2352 B  91.337%  zEagl4Anim  Initialize__…DynamicLoader   faltan 14, sobran 5, 10 SUST
     492 B  96.829%  zEAXSound   Play__9EAXCommon18eMenuSound faltan  3, sobran 2,  1 SUST
     456 B  93.509%  zEagl4Anim  EvalState__…FnRawStateChan   faltan  2, sobran 2,  2 SUST
    1480 B  99.459%  zEagl4Anim  EvalSQT__…FnStatelessQ       faltan  0, sobran 2

`audit.py` al empezar: **zEAXSound 762 ok / 0 FALLA**, **zEagl4Anim 313 ok /
0 FALLA**. Ningún FALLA que confirmar con segunda pasada.

## 1. RESULTADO

| | antes | después |
|---|---|---|
| `Play__9EAXCommon18eMenuSoundTriggers` (492 B) | 96,82927 % | **100 % — CERRADA** |
| `Play__14cSampleWarpperPC10SND_Params` (2.152 B) | 90,10780 % | 91,81599 % (+1,708 pp) |
| `EvalSQT…FnStatelessQ` (1.480 B) | 99,45946 % | 99,45946 % (revertido) |
| `EvalState…FnRawStateChan` (456 B) | 93,50877 % | 93,50877 % (revertido) |
| `Initialize…DynamicLoader` (2.352 B) | 91,33673 % | 91,33673 % (no tocada) |
| `Play__13cStichWrapperPC10SND_Params` (384 B) | 99,68750 % | 99,68750 % (revertido) |

`measure.py --cmp` (base y medida seguidas, `build_direct.py` antes de cada una):

    +492 B, +1 funciones, 1 unidades cambian
       +492 B  +1 fns  Speed/Indep/SourceLists/zEAXSound   143596 -> 144088

`pctsnap.py --cmp` del cambio de `cSampleWarpper::Play`:
**EMPEORAN: ninguna · MEJORAN: 1** (+1,708 pp).

`audit.py` al cerrar: **zEAXSound 763 ok / 0 FALLA** (una más, la nueva),
**zEagl4Anim 313 ok / 0 FALLA**. `Play__9EAXCommon18eMenuSoundTriggers` audita
entera: **492 B, 19 ramas, 44 relocs, 4 literales**.

`frozen.py`: `zEagl4Anim` **idéntico al congelado** (no la he tocado);
`zEAXSound` **re-congelado** (`95f7386646971af7`) porque es mi unidad y el
congelado anterior era el de la r23, tomado con `EAXAemsManager.cpp` modificado
sin commitear por otro agente. Ese fichero está hoy limpio en `git status`.

**Ficheros tocados: `EAXFrontEnd.cpp` (+1 línea) y `STICH_PlayBack.cpp`
(3 líneas).** Ninguna cabecera. `git diff` completo:

```
+    nvol = 0;
-  (TempAz + static_cast<unsigned short>(this->GetData().Az) + 0x10000) % 0x10000   (x3)
+  (TempAz + 0x10000 + static_cast<unsigned short>(this->GetData().Az)) % 0x10000   (x3)
```

---

## 2. EL MECANISMO DE LA RONDA: quién decide el izado del guardia

Tres rondas (25 + 13 + 18 = 56 formas de fuente) buscaron esto en la forma de
los cuatro guardias. **No estaba ahí, y ahora está medido al insn.**

### 2.1 La transformación

`jump.c` (el `jump_optimize` que corre **después de `cse_main`**) convierte

    if (cond) goto L;  rX = K;  goto EPI;  barrier;  L: ...

en

    rX = K;  if (!cond) goto EPI;  L: ...

es decir **iza el `set` por encima del salto e invierte la condición**. En
`Play(eMenuSoundTriggers)` eso gasta el izado en el **cuarto** guardia
(`li r3,0`), r3 queda vivo dentro del rango del pseudo del puntero
(`.greg`: `96 conflicts: … 3`), el puntero cae en r9 y hace falta `mr r3, r9`.

### 2.2 LA CONDICIÓN, medida con caso mínimo

> **La transformación se aplica al guardia cuya ETIQUETA DE DESTINO está seguida
> INMEDIATAMENTE por un `set` del mismo registro.**

Un caso mínimo de 20 líneas (`c24ae_min/h.cpp`, 1 s por compilación) lo
reproduce exacto y lo demuestra en cuatro configuraciones:

| caso | quién tiene `set r3` justo tras su etiqueta | quién se lleva el izado |
|---|---|---|
| `h.cpp` (4 guardias, la fuente actual) | guardia 4 (el cuerpo abre con `r3 = ptr` para `GetDMixOutput`) | **guardia 4** |
| `g2.cpp` (sin guardia 4) | guardia 3 | **guardia 3** |
| `g5.cpp` (un 5.º guardia detrás) | guardia 5 | **guardia 5** |
| `v1.cpp` (cuerpo dentro de `if(GetOutputBlockPtr())`) | ninguno | **guardia 2** (lo hace `jump2`, no cse) |
| `t1.cpp` (una sentencia cualquiera entre el guardia 4 y la llamada) | ninguno | **guardia 2** ✔ el objetivo |

No es «uno por función» ni «el último»: es **la adyacencia**. Y explica por qué
`p5`/`bb1` de las rondas 22-23 movían el izado al 2.º guardia (metían el cuerpo
dentro de un `if`, así que la etiqueta dejaba de estar seguida del `set r3`) y
por qué rompían la cola (el `return 0` pasaba a ser punto de unión).

Pase confirmado con banderas sobre el caso mínimo: **`-fno-rerun-cse-after-loop`
produce la cabecera del objetivo exacta**; `-fno-cse-follow-jumps`,
`-fno-cse-skip-blocks`, `-fno-gcse`, `-fno-expensive-optimizations`,
`-fno-thread-jumps`, `-fno-force-mem`, `-fno-force-addr`, `-fno-schedule-insns`,
`-fno-schedule-insns2`, `-fno-rerun-loop-opt`, `-fno-move-all-movables`,
`-fno-delayed-branch`, `-fno-peephole` no cambian nada. (`-fno-if-conversion` y
`-fno-optimize-sibling-calls` no existen en este compilador.)

### 2.3 El cierre: una sentencia muerta con coste CERO

Basta con que el cuerpo **no empiece** por el `set r3`. `nvol = 0;` delante de la
llamada lo consigue y **no cuesta ni una instrucción** (el store muerto lo borra
`flow`, pero llega vivo al `jump_optimize` que decide):

    a1_nvol0    `nvol = 0;` detrás del cuarto guardia   **100,00000 %  492 B  0 diffs**
    a4_dblneg   `nvol = 0; nvol |= …;`                  **100,00000 %  492 B  0 diffs**
    b1_declabajo `int nvol = 0;` bajada al cuerpo       **100,00000 %  492 B  0 diffs**
    b2_slidervol `int CurSliderVol = 0;` izada del if   **100,00000 %  492 B  0 diffs**
    b4_tmp      = b2 sin la línea en blanco             **100,00000 %  492 B  0 diffs**
    a2_declhere `int nvol = 0` arriba + `nvol +=`        96,82927 %  488 B  9 diffs
    a3_slider1st `GetDMixOutput(...) + 0`                96,82927 %  488 B  9 diffs
    b5_arriba   `int nvol = 0;` en la declaración        96,82927 %  488 B  9 diffs  (control)

**Aplicada `b3`/`a1` (`nvol = 0;`)** porque es la única de las cinco que respeta
el DWARF del original: `dwbody.py` dice que sus locales son
`int nvol` (r30) y `int CurSliderVol` (r0) **dentro de un bloque anónimo**, así
que `b2`/`b4` (que sacan `CurSliderVol` del `if`) contradicen el volcado y `b1`
mueve la declaración de `nvol`.

### 2.4 Regla general que deja la ronda

> **Si sobra un `mr` a un registro duro de retorno/argumento y falta un `li`
> izado, no barras los guardias: mira qué hay justo detrás de la etiqueta a la
> que salta cada uno.** El izado se lo lleva el guardia cuya etiqueta abre con un
> `set` del mismo registro; una sentencia muerta delante del cuerpo lo mueve sin
> coste.

---

## 3. `Play__14cSampleWarpperPC10SND_Params` (2.152 B) — 90,108 → 91,816 %

`dwbody.py` da **una sola diferencia**: `int TempAz` está en **r23** en el
original y en **r25** en el nuestro. Locales, árbol de inlines y bloques,
idénticos.

Y el diff señalaba la causa concreta en las filas 176/178 (y 307/311):

    objetivo:  addis r9, r9, 0x1     ; +0x10000 PRIMERO
               add   r11, r23, r9    ; luego TempAz
    nuestro:   add   r9, r25, r9     ; TempAz primero
               addis r11, r9, 0x1

O sea el original agrupa **`TempAz + 0x10000`** y no `TempAz + Az`. Barrido de
las 9 asociaciones útiles de `(TempAz + Az + 0x10000) % 0x10000`:

    base   (a+b)+c                                     90,10780  141 diffs
    w1     (b+c)+a                                     89,90334  162
    w2     a+(b+c)                                     89,47212  185
    w3     (c+b)+a                                     89,90334  162
    x3     (b+a)+c                                     90,10780  141
    x2     b+(a+c)                                     91,78810  133
    **w4   (a+c)+b   `TempAz + 0x10000 + Az`**         **91,81599  133**
    **x1   (c+a)+b   `0x10000 + TempAz + Az`**         **91,81599  133**
    **x4   ((a+c))+b con paréntesis explícitos**       **91,81599  133**
    x5     `& 0xFFFF` en vez de `% 0x10000`            82,58550  2.032 B  PEOR

Aplicada `w4`. **Es una corrección estructural real**: con ella las filas 176/178
casan y el orden de operandos coincide con el objetivo. `pctsnap --cmp` con base
y medida seguidas: **EMPEORAN ninguna, MEJORAN 1**.

Barrido adicional (sobre `w4`, todas en meseta o peor): `y1` TempAz asignada
antes de PitchScale (91,816 = igual), `y2` TempAz asignada la última (90,227
PEOR), `y3` TempAz asignada en el bloque de declaraciones (91,816 = igual).

**Lo que queda son 133 diffs y todos cuelgan de UNA cosa**: el objetivo abre el
marco con `stwu r1,-0x58` y `stmw r22` y nosotros con `stwu r1,-0x48` y
`stmw r23` — **el original usa un registro salvado más (r22) y 12 B más de
locales**. En concreto guarda la constante `0` del último argumento
(`filter_HiPass`) en **r25** a lo largo de las tres construcciones
(`li r25,0` … `stw r25,0x28(r31)`), mientras nosotros la rematerializamos por
bloque (`li r0,0`). **Mientras no aparezca ese valor vivo de más, no cierra.**

---

## 4. `EvalSQT__…FnStatelessQ` (1.480 B) — la veda aguanta, pero cambia el porqué

`faltan 0, sobran 2 · clrlwi-2`. El objetivo hace `subi r0,r5,1 ; cmpw r30,r0`
sobre el pseudo **HImode** de `mNumKeys`; nosotros añadimos
`clrlwi r0,r0,16` (el `static_cast<unsigned short>`).

### 4.1 Caso mínimo: sin el cast SÍ funde

`c24ae_min/q1.cpp` y `q2.cpp` (40-55 líneas) reproducen el `clrlwi` exacto. En
ellos, **quitar el cast da `addi 0,5,-1 ; cmpw` = el objetivo**:

    `>= static_cast<unsigned short>(mNumKeys - 1)`   addi; rlwinm; cmpw   (= nuestro)
    `>= mNumKeys - 1`                                addi; cmpw           (= objetivo)
    `>= (int)mNumKeys - 1`                           addi; cmpw
    `> mNumKeys - 2`                                 addi -2; cmpw
    `!(< mNumKeys - 1)`                              addi; cmpw

O sea el problema **no es la sentencia**: en un contexto pequeño la fusión de
`combine` funciona. En el fichero real, no:

    q1_nc_both  sin cast en las dos líneas   98,05135  1.488 B  17 diffs
    q2_nc_74    sin cast sólo en la 74       98,13243  1.488 B  12 diffs
    q3_nc_132   sin cast sólo en la 132      99,37838  1.488 B   7 diffs
    q4_int_both `(int)mNumKeys - 1`          98,05135  1.488 B  17 diffs

y el diff de `q2_nc_74` enseña `clrlwi r9,r5,16 ; subi r9,r9,1` — combine extiende
la **entrada** en vez de fundir.

### 4.2 Hipótesis propia, REFUTADA con medida

Razoné que sin el cast la fusión sólo es legal si `combine` sabe
`nonzero_bits(P) ≤ 0xFFFF`, y que eso falla porque PRE crea un segundo `set` del
pseudo (`lhz r5` en una rama y `mr r5,r0` en la otra, filas 37/42), lo que anula
`reg_nonzero_bits` (que sólo se calcula con `REG_N_SETS == 1`). **Falso:**

    r1_localus  `unsigned short numKeys = …;` de un solo set, sin cast   98,13243  PEOR
    r2_localint `int numKeys = …;` de un solo set, sin cast              98,13243  PEOR
    r3_localus_cast  el mismo local conservando el cast                  IDÉNTICO al base

cse funde el local con el pseudo existente y el resultado no cambia. **La veda de
las 40 formas de la sentencia sigue en pie**; lo que hay que averiguar es qué
tiene el fichero real que no tiene el caso mínimo, y **el caso mínimo ya está
escrito** (`q1.cpp`/`q2.cpp`): el camino es crecerlo hasta que la fusión se
rompa.

---

## 5. `EvalState__…FnRawStateChan` (456 B) — diagnóstico nuevo, cero avance

Los `mr+2 / add-2` son las filas 40 y 80: el objetivo hace `mr r4, r11`
reutilizando la base y nosotros `add r4, r30, r10` recalculándola. Además la
fila 49/59: nosotros izamos el `subi r0,·,1` de `GetNumKeys()-1` y el objetivo lo
emite tarde.

**Dato nuevo, del volcado RTL** (`c24ae_rtl/ev.ii.gcse`):

    CPROP of bool EAGL4Anim::FnRawStateChan::EvalState(float, State *), pass 1: 0 const props, **6 copy props**
    CPROP of bool EAGL4Anim::FnRawStateChan::EvalState(float, State *), pass 2: 0 const props, **9 copy props**

Es exactamente el patrón de la regla de la r22 (`UpdateRPM`): **sobra/falta un
`mr` y `gcse` imprime N copy props → hay que matar la DISPONIBILIDAD de la copia
volviendo a escribir uno de los dos registros en el camino**. La copia es
`keyData = base` dentro de `RawStateChan::GetKeyData`, y ahí no hay dónde
escribir: es una cabecera de 8 líneas con la veda de 10 formas de la r21/r22.

Ensayos nuevos (8, ninguno mueve nada):

    ev1_kdead        `k = 0;` muerta delante del primer guardia   IDÉNTICO
    ev2_idead        `i = 0;` muerta delante                      IDÉNTICO
    ev3_declorder    `dest` declarada la segunda                  IDÉNTICO
    ev4_kfirst       `k` declarada la primera                     IDÉNTICO
    ev5_kdead_final  `k = GetKeyData(GetNumKeys()-1);` explícito  IDÉNTICO
    ev6_numkeys_dead `i = GetNumKeys();` muerta delante           IDÉNTICO
    ev7_final_i      `i = GetNumKeys()-1;` + Decode(GetKeyData(i)) 89,82456  PEOR
    ev8_final_swap   `mKeyIdx = …` antes del `Decode`              69,90351  PEOR

**El truco de la sentencia muerta que cerró `Play` NO sirve aquí**: allí decidía
un CFG (y la insn llega viva al `jump_optimize`); aquí decide `local-alloc` y las
sentencias muertas se borran antes de contar. Veda ampliada a **37 formas**.

---

## 6. `Initialize__…DynamicLoader` (2.352 B) — localizado al byte, no tocada

Ni un ensayo de fuente (tres rondas de vedas: 88 árboles, 512 listas de `case`,
14 del DWARF). Sí he leído el diff entero y **la diferencia principal ya está
localizada**, y confirma lo que dejó escrito la r21:

Las filas 133-156 son el `ELFAddr` inline. El objetivo **cruza por cross-jump las
dos copias de `return &mpData[offset];`**: su primer camino hace
`lwz r0,0x10(r8) ; b L154` y cae **dentro** del bloque de la segunda copia
(`add r30,r0,r30 ; stw r30,0x98(r1)`). Nosotros emitimos las dos copias enteras y
**nos sobran ahí dos instrucciones** (`add r9,r0,r31 ; stw r9,0x98(r1)`).

**Por qué no se cruzan**: el cross-jump de `jump2` corre **después** del reparto
de registros y exige instrucciones idénticas. El objetivo escribe el resultado en
**r30 en las dos copias** (y r30 es además el `offset`, que muere en ese `add`);
nosotros escribimos en **r9 y r6**. No es la fuente —que ya tiene las dos
sentencias `return &mpData[offset];` idénticas, comprobado en
`eagl4supportdlopen.h`— es el registro de recarga que elige `reload`.

Y el resto de la firma es lo que decía el encargo: al objetivo le sobra
`mtctr r8` (guarda `i*0x28` en CTR) y `lwz r11,0x98(r1)` — **más presión**.

---

## 7. `Play__13cStichWrapperPC10SND_Params` (384 B, 99,688 %) — a 4 filas

El diff son **cuatro filas y una sola transposición r30↔r31**:

    37 slwi r30, r28, 2      |  slwi r31, r28, 2       ; el índice i*4
    39 addi r31, r29, 0x24   |  addi r30, r29, 0x24    ; la base &ActiveSamplesRefs[0]
    45 stwx r3, r31, r30     |  stwx r3, r30, r31
    49 lwzx r0, r31, r30     |  lwzx r0, r30, r31

Planificación idéntica, `n_refs` y `live_length` idénticos → **empate de
`allocno_compare` roto por número de allocno**, o sea por orden de creación de
los dos pseudos. 12 formas barridas, ninguna lo invierte:

    t1 `!= nullptr` en el if              IDÉNTICO
    t3 `++i`                              IDÉNTICO
    t4 el `for` como `while`              IDÉNTICO
    t2 samplereq: pStitch antes de pSample 98,12500  380 B  PEOR
    t5 `SampleQueueItem` declarada antes de Initialize  94,89584  PEOR
    v1 `*(refs + i)` en los cuatro usos   88,58334  PEOR
    v2 `*(refs + i)` sólo en la asignación 94,30209  392 B  PEOR
    v3 `*(refs + i)` sólo en los usos     90,80209  392 B  PEOR
    v4 local `cSampleWarpper **slot`      93,42709  PEOR
    s1 sin la local `scale`               97,44791  388 B  PEOR
    s2 `int StitchSpecificVol = … >> 0xF;` 97,44791  388 B  PEOR
    s3 `32767 >> 15` en decimal            97,44791  388 B  PEOR

**Y ojo con `dwbody` aquí, que es un caso de «la diferencia no es accionable»**:
el volcado dice que al original le **falta** nuestra local `int scale` — pero
quitarla cuesta 2,24 pp y **cuatro bytes de más**. La local está haciendo trabajo
real.

---

## 8. `MsgPlayMiscSound__9EAXCommon` (268 B, 96,761 %) — hallazgo de cabecera, neutro

Seis diffs, todos el mismo empate: el objetivo materializa el `0` en **r0** para
las cuatro escrituras del constructor de `FX_Radar` y nosotros lo repartimos
entre **r11** (campos 0x4 y 0x10) y **r0** (0x14 y 0xc).

`dwbody.py` da una diferencia estructural REAL, y es de cabecera:

| | orden de los inline del constructor `FX_Radar` |
|---|---|
| **original** | SetID, SetVolume, **SetPitch_Offset, SetIntensity, SetStop** |
| nuestro (`MAIN_AEMS.h:1726`) | SetID, SetVolume, **SetStop**, SetPitch_Offset, SetIntensity |

O sea **el original llama a `SetStop` la ÚLTIMA**. Lo he probado con un arnés que
**sombrea la cabecera desde el scratchpad** (`-I <scratchpad>/c24ae_inc` delante
de `-I src`, verificado con un `#error` que el sombreado está activo): el objeto
sale **IDÉNTICO** (96,76119 %, 6 diffs). Es una corrección de fidelidad sin efecto
en el código.

**NO la he aplicado**: `MAIN_AEMS.h` la incluyen **9 ficheros** y el encargo
prohíbe tocar cabeceras compartidas sin decirlo. Queda aquí para quien quiera
hacer el A/B. (Segunda diferencia menor del mismo volcado: el parámetro se llama
`message` en el original y `msg` en el nuestro.)

---

## 9. Herramientas y trampas

### 9.1 La receta del caso mínimo para ProDG (`c24ae_min/`)

Es lo que ha cerrado la ronda. **1 s por compilación** y `-da` completo:

    # mk.sh  -> el .s
    SN_NGC_PATH=<...>/build/compilers/ProDG/3.9.3
    ngccc.exe -O1 -mps-nodf -G0 -fno-static-dtors -ffast-math -fforce-addr \
      -fcse-follow-jumps -fcse-skip-blocks -fforce-mem -fgcse -frerun-cse-after-loop \
      -fschedule-insns -fschedule-insns2 -fexpensive-optimizations -frerun-loop-opt \
      -fmove-all-movables $EXTRA -x c++ -S -o x.s x.cpp
    # rtl.sh -> los 16 volcados por pase (ngccc -E a un .ii propio + cc1plus -da)

`run.sh <f.cpp>` imprime el cuerpo sin ruido; `EXTRA=-fno-…` barre banderas.

### 9.2 Arnés con sombreado de CABECERAS (`c24ae_eax2.py`)

Copia de `c23st_eax.py` con **`EXTRA` delante de `CFLAGS`** en la línea de
compilación, para que un `-I <scratchpad>/c24ae_inc` gane a `-I src`. Con eso se
prueba un cambio de cabecera compartida **sin tocar el árbol**. Comprobado con un
`#error` que el sombreado está activo (si no se comprueba, un `-I` que llega
tarde da «el cambio no hace nada» y es mentira).

### 9.3 Trampa nueva: la caché de `regmap.py` puede estar rancia y no se refresca

`build/regmap/our_<unidad>_<tam>_<mtime>.nothpp` está indexada por tamaño y mtime
del objeto. El objeto de `zEagl4Anim` ya era `1179764` bytes y la caché seguía
siendo la de `1179584` (4-sep), y **ni `--list` ni `--build` la regeneraron**. Me
dio una local `unsigned short * times` en `FnStatelessQ::EvalSQT` que **no existe
en la fuente**: era el volcado de antes del arreglo de la r21. Los `dwbody` de
esa unidad hay que leerlos con esa reserva (los de `zEAXSound` sí estaban
frescos: el hallazgo de `FX_Radar` y el de `TempAz` se corresponden con el código
actual).

### 9.4 Recordatorios que han vuelto a morder

* Los `.h` de `SND_GEN` son **CRLF**: un `str.replace` con `\n` da 0 ocurrencias
  y parece que el bloque no existe. `\r?\n` por línea, siempre.
* `objdiff-cli` escribe en la ruta que le des, pero el shell de Git Bash traduce
  `/tmp` a `C:/Users/…/Temp`: pásale rutas absolutas.
* `triage.py` imprime la lista de opcodes **desordenada y truncada a 4**; dos
  pasadas seguidas dan firmas distintas de la misma función.

### 9.5 Ficheros dejados en el scratchpad (prefijo `c24ae_`)

* `c24ae_eax.py` — arnés multi-parche sobre una copia del `.cpp` (copia de
  `c23st_eax.py` con nombres propios para no pisar a otros agentes).
* **`c24ae_eax2.py`** — el mismo con `EXTRA` antes de `CFLAGS` (§9.2).
* `c24ae_inc/…/MAIN_AEMS.h` — la cabecera sombreada con el constructor reordenado.
* **`c24ae_min/`** — `mk.sh`, `run.sh`, `rtl.sh`, `hdr.inc`, y los casos
  `g.cpp h.cpp v1.cpp g2.cpp g5.cpp t1.cpp q1.cpp q2.cpp` con sus `rtl_*/`.
* `c24ae_rtl/ev.ii*` — los 16 volcados por pase de `RawStateChan.cpp`.
* `c24ae_spec_{base,a,b,q,q2,q3,ev,ev2,dl,mp,sw,sw2,sw3,sw4,st,st2,st3,st4}.py` —
  los ~60 ensayos, reproducibles.
* `c24ae_{antes,base2,post1,post2,final}.json`, `c24ae_pct_{base2,post2}.json`.

---

## 10. Qué NO he probado

* **`Initialize__…DynamicLoader`**: cero ensayos de fuente. No he probado a
  reconstruir el cross-jump de `ELFAddr` (§6) ni a subir la presión para que
  `reload` elija el mismo registro en las dos copias; ni el permutador.
* **`FnStatelessQ`**: no he crecido el caso mínimo hasta romper la fusión de
  `combine` — **es el trabajo obvio para la siguiente ronda y las herramientas
  están escritas** (`q1.cpp`/`q2.cpp`). No he instrumentado `try_combine`.
* **`EvalState`**: no he tocado `RawStateChan.h` (cabecera compartida con
  `MemoryPoolManager.cpp`, las dos en zEagl4Anim: el A/B es barato). No he
  probado `lreg.py`/`.greg` sobre esta función ni el permutador.
* **`cSampleWarpper::Play`**: no he encontrado cómo hacer vivir la constante `0`
  del último argumento en un registro salvado (§3). No he probado el permutador
  ni pines.
* **`cStichWrapper::Play`**: no he probado el permutador ni un pin sobre los dos
  pseudos del empate (§7). El encargo deja los pines para el último recurso y
  `pines.py` no señala nada aquí.
* **`MAIN_AEMS.h`**: el reorden del constructor de `FX_Radar` (§8) está medido
  como neutro en `MsgPlayMiscSound` pero **no** he hecho el A/B de las otras 8
  unidades que la incluyen.
* **Restricciones de registro / `permuter.py`**: ninguna, en ninguna función.
* **Banderas de unidad**: sólo como DIAGNÓSTICO sobre el caso mínimo (§2.2),
  nunca aplicadas. `configure.py`, `config/GOWE69/*` y `splits.txt` intactos.
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas.
* **El DOL**: no he ejecutado la construcción completa (la promoción y el DOL van
  en la verificación de la tanda, según el protocolo).
* **Los muros que no menciono** (`UpdateParams__16SFXCTL_MasterVolf`,
  `SetupNextLoad`, `QueueFileLoad`, `DataLoadCB`, `ResolveCurrentDataMemory`,
  `startnextrequest`, `Eval__…FnRawLinearChannel`, `FindMatchTime`): no tocados.
  Los cuatro de `EAXAemsManager.cpp` y el de `SFXCTL_MasterVol.cpp` son del otro
  agente. `FindMatchTime` sí lo he mirado con `dwbody`: **cero diferencias
  estructurales**.
