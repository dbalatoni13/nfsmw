# Ronda 23 — zEcstasy: dos funciones cerradas, 3.668 B

Estado medido (`build_direct.py zEcstasy` antes de cada medida):

    antes    136.340 / 145.884 B   93,4578 %   532 funciones al 100 %
    despues  140.008 / 145.884 B   95,9721 %   534 funciones al 100 %
    measure.py --cmp c23ecs_antes.json c23ecs_despues.json
      -> +3.668 B, +2 funciones, 1 unidad cambia, NINGUNA baja

    audit.py  534 ok, CERO FALLA  (dos pasadas)
    frozen.py cong  Speed/Indep/SourceLists/zEcstasy = 9c8312a359c83949

**Cerradas: `eProgressiveScan_EURGB60DialogBox` (1.848 B) y `UpdateParticles`
(1.820 B).** Las dos al 100,00000 %, 0 diffs, `audit.py` limpio.

Y de paso **desaparece la `FALLA` de `__static_initialization_and_destruction_0`**
(§5): no la he tocado, se resolvio sola al renumerarse el pool.

## 0. El encargo, verificado

`triage.py --muro zEcstasy` al empezar reprodujo el brief exacto (696 + 8.848 =
9.544 B). Al acabar:

    696 B   98,075 %  Render__13EmitterSystemP5eView       faltan 2 (stw+1, lwz+1)
    MURO: 4 funciones, 5.180 B
    2072 B  93,305 %  epCalculateLocalDirectionalPOS16
    2044 B  98,280 %  UpdatePlatInfo
     796 B  98,995 %  GenerateHorizonFogDisplayList
     268 B  88,851 %  eProject

## 1. `eProgressiveScan_EURGB60DialogBox` (1.848 B) — CERRADA. Mecanismo nuevo

### 1.1 El mecanismo: `fold` pliega `(A ^ B) & B` cuando B es EL MISMO ARBOL

La ronda 22 dejo el diagnostico correcto («el objetivo hace el `and.` contra el
valor enmascarado y nosotros contra el boton crudo») pero atribuido a `combine`.
**No es `combine`, es el front end**, y se demuestra con un caso minimo de 18
lineas a **0,6 s por compilacion** (`scratchpad/mini/`):

    fuente                                                          and. emitido
    ((now&M) ^ (last&M)) & (last&M)                                 CRUDO   (mal)
    (last&M) & ((now&M) ^ (last&M))            v1                   CRUDO
    ((last&M) ^ (now&M)) & (last&M)            v20                  CRUDO
    ((now&M) ^ (last&M)) & (0x4 & last)        v21                  CRUDO
    ((now&M) ^ (last&M)) & ((int)last & 0x4)   v22                  CRUDO
    (((now&M) ^ (last&M)) & last) & M          v12                  CRUDO
    !!(...)                                    v18                  CRUDO
    ---------------------------------------------------------------------------
    int lb = last&M; int nb = now&M; (nb^lb)&lb            v2       ENMASCARADO
    int lb = last&M; ((now&M) ^ lb) & lb                   v11      ENMASCARADO
    int lb = last&M; ((now&M) ^ (last&M)) & lb             v14      ENMASCARADO
    int lb = last&M; ((now&M) ^ lb) & (last&M)             v17      ENMASCARADO
    ({ int m_=last&M; ((now&M)^m_)&m_; })                  v16      ENMASCARADO
    funcion inline con el temporal dentro                  v15      ENMASCARADO

**La regla, generalizable a todo el arbol:** en GCC 2.9, `X & Y` donde `Y` es
**textualmente el mismo subarbol** que un operando de `X` se pliega en `fold`
(quita la mascara redundante). En cuanto uno de los dos deja de ser el mismo
arbol —una variable, un parametro, una expresion-sentencia— **el plegado no
ocurre**. Da igual el orden de los operandos, los parentesis, el `!= 0` o el
cast: **lo unico que lo rompe es que dejen de ser el mismo arbol.**

O sea: **si el objetivo NO pliega una redundancia que a nosotros se nos pliega,
la fuente original usaba una variable o un helper, no la expresion repetida.**

### 1.2 La forma que casa (q1, la que queda en el arbol)

    static inline int eProgressiveScanButtonReleased(int now_masked, int last_masked) {
        return (now_masked ^ last_masked) & last_masked;
    }

llamada en los **seis** sitios (DOWN/UP/A x mando/volante) como

    eProgressiveScanButtonReleased(g_InitPad[channel].button & PAD_BUTTON_DOWN,
                                   g_LastInitPad[channel].button & PAD_BUTTON_DOWN)

Es `static inline` y **no emite simbolo** (comprobado en el `.o`: 3.000 simbolos,
ninguno se llama asi; `audit.py` cuenta 534 funciones, una mas que antes, que es
`eProgressiveScan` pasando a 100 %).

### 1.3 Ensayos numerados

    p1  helper inline (int now, int last, int mask), la mascara DENTRO
        98,766 -> 99,41558 %  (94 -> 20 diffs)  size 1852/1848  <- 4 B DE MAS
    p2  helper que lee los globales por canal (int channel, int mask)
        99,12337 %  (34 diffs)  1852 B.  Revertido, peor que p1.
    p3  = p1 con parametros `unsigned short`
        99,41558 %  (20 diffs)  1852 B.  Identico a p1.
    p4  macro con expresion-sentencia GNU `({ int m_ = ...; ... })`
        99,70779 %  (14 diffs)  1848/1848
    p5  helper (int now_masked, int last, int mask), el `now & mask` en el
        SITIO DE LLAMADA
        **100,00000 %  1848/1848  0 diffs**
    q1  helper de dos argumentos (now_masked, last_masked), las dos mascaras
        en el sitio de llamada
        **100,00000 %  1848/1848  0 diffs**   <- ES LA QUE SE QUEDA (mas limpia)

Los 4 B de mas de p1/p3 eran **un `clrlwi rX, r3, 16`**: pasar
`g_InitPad[channel].button` (u16) por un parametro materializa la conversion en
una insn propia con dos usos, y ahi `combine` ya no la puede plegar contra la
mascara. Con la mascara aplicada en el sitio de llamada la conversion y el `and`
salen en la MISMA insn y se pliegan solas. **Aviso para quien reutilice el
patron: el temporal tiene que ser el operando ENMASCARADO, no el crudo.**

### 1.4 Lo que se llevo por delante

Las **94** filas eran dos cosas, y **las 68 del scratch del `elf_high` (objetivo
r8, nosotros r9) eran CONSECUENCIA de las otras 26**, no un problema aparte: al
mantener vivo el valor enmascarado el objetivo ocupa un GPR volatil mas, y el
scratch del `lis @ha` baja en `REG_ALLOC_ORDER` de r9 a r8. **Corrige el §5 de
`r22-ecs.md`, que las daba por independientes.**

## 2. `UpdateParticles` (1.820 B) — CERRADA. Y el modelo del corte, cerrado

### 2.1 La palanca la dio el DWARF, no el asm

`dwbody.py … both`, contando expansiones inline a cada lado:

    bTNode<EmitterParticle>::GetNext    original 4    nuestro 3
    bNode::GetNext                      original 12   nuestro 11

**El original avanza `particle` una vez mas que nosotros**, y las tres del bloque
`mLife > time_step` tienen en el DWARF del objetivo el **mismo rango**
(`0x8011222C`): es decir, el objetivo las tiene en fuente y **`jump2` las funde
por cross-jump**. Esa era exactamente la insn fantasma que pedia el modelo:
existe cuando `flow` calcula `reg_live_length` y ya no existe en el codigo final.

### 2.2 El cambio (u1)

Partir el `||` de la prueba de alfa en dos `if`:

    if (ignore_programmer_badness) { particle = particle->GetNext(); continue; }
    if (a > alpha_value_to_kill_at) { particle = particle->GetNext(); continue; }
    { EmitterParticle *to_kill = particle; particle = particle->GetNext();
      this->KillParticle(em, to_kill); }
    continue;

Resultado medido:

    live_length  428 (257.0f)  632 -> 636      d = +4
                 439 (1/255)   630 -> 634
    prioridad    221 / 222     ->  220 / 220   EMPATE
    desempate    allocno 428 < 439  ->  428 primero  ->  f21 = EL OBJETIVO
    UpdateParticles  99,95605 % (4 diffs)  ->  **100,00000 %  1820/1820  0 diffs**

**`d = +4` estaba en la ventana** que la ronda 22 calculo (`-2 | +1 +4 +7 +10 …`),
y las dos insns extra (dos bloques `lwz` + `b` identicos) desaparecen en el
cross-jump: el tamano NO se mueve. Es la primera vez en el proyecto que se cierra
un corte de `allocno_compare` con una sentencia de fuente.

### 2.3 Diagnostico que NO se queda pero vale la ronda

Antes del DWARF probe **c1**: partir el `(uint32)` del `pangle` en un temporal y
calcular `rot_scale` entre medias, para invertir el orden de izado de las dos
constantes:

    float ia = static_cast<uint32>(particle->mInitialAngle); float rot_scale = extra_params.y * (1.0f / 255.0f);
    pangle = ia * 257.0f; adelta = extra_params.y + rot_scale * ...;

    99,95605 -> 99,97802 %  (4 -> 2 diffs)  1820/1820

**Los dos registros salian YA como el objetivo por valor** (257.0f en f21,
1/255 en f20). Los 2 diffs que quedaban eran que **se invertia el orden del
POOL**: `$LC939` pasaba a valer `0x3b808081` (1/255) y `$LC940` `0x43808000`
(257,0), mientras el objetivo tiene `lbl_803DF358 = 257` y `lbl_803DF35C = 1/255`.
**Revertido** (c1 dejaba la `.rodata` distinta del original). Sirvio para
demostrar dos cosas:

- **el orden de izado de `loop.c` conserva el orden de la fuente** (dos medidas),
- y **va pegado al orden del pool**, que es el orden de expansion: no se pueden
  separar cambiando el orden de las sentencias.

Tambien medido: **d3** (mover solo `rot_scale` delante de `pangle`) da 99,69231 %
(6 diffs) porque la constante DF magica del `(uint32)` se cuela entre las dos y
el hueco pasa de 2 a 4.

## 3. `Render__13EmitterSystemP5eView` (696 B) — cerrada por construccion, y ahora con prueba

**Nuestro asm y nuestro pin NO sobran** (el brief pedia medirlo):

    r1  quitar la barrera `asm("" : : "f"(world_size))`  -> 82,12069 % (82 diffs)
    r2  quitar solo el pin `register float ... asm("fr6")` -> 97,03449 % (56 diffs)

Los dos revertidos; el estado con las dos cosas es 98,07471 % (34 diffs). Es el
tercer contraejemplo del proyecto a «quita el pin».

Y el DWARF cierra la via de fuente: **`dwbody.py` da 47 expansiones inline
IDENTICAS** y `regmap` da **las mismas locales con los mismos registros** en los
dos lados (`num_textures` r20, `grp` r25, `em` r28, `plist` r26,
`sprite_hack_flags` r24, `last_emitter_data` interior r22, `particle` r30,
`this_emitter_data` r9, y las cinco `Vector3` en las mismas ranuras de pila).

Lo unico que cambia es el reparto de los TEMPORALES del compilador, y esta
cuantificado: **usamos los 18 salvados r14-r31, el objetivo usa 17 y derrama el
centinela** `&this->mEmitterGroups` a `0xc8(r1)` (de ahi el `stw`+`lwz` y los
8 B de marco). Todo su reparto va corrido un registro (r14/r15, r15/r16, r16/r17,
r17/r18, r18/r19). **Le falta al objetivo un allocno mas de larga vida que no es
ninguna variable de la fuente.** Anotado en el propio `EmitterSystem.cpp`.

Unica diferencia de nombres en el DWARF (no cambia codigo): el original **repite
el nombre** `last_emitter_data`/`_atr` en el bloque interior (sombra), nosotros
los llamamos `inner_last_emitter_data`.

## 4. `epCalculateLocalDirectionalPOS16` (2.072 B) — el marco +8, ya con la aritmetica

**Correccion a la r21/r22: NO es un `assign_stack_temp` de la expansion.** El
volcado `-dr` de la funcion **no tiene ni una sola `mem` relativa al marco**:
todas las ranuras las crea `reload`. La aritmetica del marco, sacada de los dos
`.s`, es esta (el `.greg` de GCC 2.9 se imprime DESPUES de `reload`, por eso trae
`r1` y registros duros):

    ranuras referenciadas, IDENTICAS en los dos lados:
      SF 0x8 0xc | PS 0x10 0x18 0x20 0x28 0x30 0x38 0x40 | SF 0x4c
      PS 0x50 0x58 0x60 | SF 0x6c 0x74 0x7c
    objetivo: get_frame_size = 128 (0x80)  -> fpmem 0x88, CR 0x94, stmw 0x98, marco 0x170
    nuestro : get_frame_size = 136 (0x88)  -> fpmem 0x90, CR 0x9c, stmw 0xa0, marco 0x178

La ranura de 8 B de la conversion entero/flotante **no es un temporal de pila
normal: es el registro falso `fpmem` de rs6000** (reg 76, patrones
`*floatsidf2_store1/2`, `*floatsidf2_load`, `*fix_truncdfsi2_store/load`), y hay
**una sola** en los dos lados. Los `SF` en 0x4c/0x6c/0x74/0x7c son las mitades
bajas de ranuras de 8 B en 0x48/0x68/0x70/0x78. **Lo que sobra es una ranura de
8 B de `reload` que ninguna insn referencia.**

Ensayos (todos revertidos, ninguno mueve el marco ni el %):

    e1  quitar las locales muertas `envmap_power`/`envmap_bias` -> marco 0x178, IGUAL
    x1  declarar `ib8` antes que `ib16` (orden del DWARF)       -> 93,30502 %, 155 diffs, IGUAL
    x2  mover `dsrg` delante de `dsba` (orden del DWARF)        -> 92,33784 % (181 diffs). PEOR
    x3  `sn_ps dsrg;` partida + asignacion en su sitio          -> 93,30502 %, IGUAL
    x4  `ib8` partida y calculada desde `ib16`                  -> 93,30502 %, IGUAL

**Lo que si trae `regmap.py` y es la mejor pista para el que siga:**

    local   original   nuestro
    ib8     -          r5        <-- el original NO le da registro a ib8
    ib16    r5         -         <-- se lo da a ib16
    dcrg    f3         f5
    dcba    f2         f4
    scba    f4         f13

Que `ib8` no tenga registro en el original y `ib16` si, con las dos declaradas,
significa que **el original indexa el switch por `ib16` (con casts), no por
`ib8`**, y que `ib8` esta declarada y practicamente muerta. Reescribir los nueve
`case` sobre `ib16` es el ensayo que no me ha dado tiempo a hacer.

Aviso que sigue en pie: aunque se cierren los 8 B quedan **106 filas** de
permutacion de `f2..f13` en el bucle interno.

## 5. La `FALLA` de `__static_initialization_and_destruction_0`: resuelta sola

Al empezar la ronda estaba (`lis r11, 0x803e | lis r11, $LC980@ha`), confirmada
en segunda pasada. **Despues de q1 ya no aparece**: `audit.py` da
`__static_initialization_and_destruction_0  ok  4544 B, 17 ramas, 429 relocs,
118 literales`. La causa es que el helper de q1 **renumera el pool de literales
de la unidad** y el `@ha` que el objetivo aparca en LR pasa a emparejarse. Las
seis lineas de `config/GOWE69/symbols.txt` que proponia la r22 **ya no hacen
falta**. `audit.py`: **534 ok, cero FALLA, dos pasadas.**

## 6. Herramientas

- **`scratchpad/mini/`** (`m.cpp`, `go.sh`, `sweep.py`): 25 variantes del flanco
  de boton a **0,6 s cada una** con los cflags reales. Es lo que abrio la funcion:
  ninguna de las 25 se podia haber adivinado desde la unidad entera.
- **`c23ecs_cc1.py`** — `cc1plus` a mano sobre el `.i` de zEcstasy con banderas
  extra (`-dr`, `-fno-gcse`, lo que sea) y sin tocar el `build/` compartido.
  Reproduce el reparto y el marco **exactos** del build.
- **`c23ecs_show.py` / `c23ecs_len.py`** — tabla `pseudo/n_refs/live_len/size/
  prioridad/registro`. **`scripts/lreg.py` sigue mintiendo por omision**: su
  `RE_REG` exige `pref (\w+)` y las lineas de coma flotante dicen `FLOAT_REGS or
  none`, asi que **se come TODOS los pseudos de FPR** — que son justo los que
  deciden zEcstasy. En `UpdateParticles` enseña 152 de 240 y **ni uno solo de los
  de f**. Ademas no aplica el factor `size` (los `8 bytes` cuentan doble).
- **`c23ecs_inlcnt.py`** — cuenta expansiones inline y locales del DWARF a los
  dos lados de golpe, para una lista de funciones. Es lo que cerro
  `UpdateParticles` y lo que cerro por construccion `Render`.
- **`regmap.py --list` NO regenera nuestro volcado** (solo lista las funciones
  del original y sale con codigo 2). El `our_<unidad>_<tam>_<mtime>.nothpp` solo
  lo escribe una invocacion normal `regmap.py <unidad> <Funcion>`. Con el `.o`
  recien compilado hay que borrar el cache o `dwbody.py` compara contra un
  volcado de hace dias **sin avisar**. Me costo dos lecturas falsas.

## 7. Que NO he probado

- **`UpdatePlatInfo` (2.044 B)**: ni un ensayo. Lo que si tengo medido es su
  `regmap`: los seis registros del muro siguen en el objetivo (f20/f19/f18 y
  f16/f15/f14 — el `c5` de la r22 aguanta) y **solo quedan tres locales con
  registro distinto**: `diffuse_max_scale` (obj f6, nuestro f5), `diffuse_rng_a`
  (obj f5, nuestro f2) y `envmap_power` (obj f2, nuestro f1). Son FPR volatiles,
  o sea `local_alloc`. Los tramos `d = -5..-4, +2..+5, +8..+12` de la ventana de
  la r22 **siguen sin tocar**.
- **`GenerateHorizonFogDisplayList` (796 B, 2 diffs)**: sin ensayos. El unico
  hallazgo es que **la unica diferencia de DWARF es la cabecera**: el original
  declara `GXPosition1x8/GXColor1x8/GXTexCoord1x8` con el parametro
  **`const unsigned char x`** y nosotros sin `const`. Es una «cabecera que
  miente» a un grep, y **no la he probado** (cabecera compartida: hay que hacer
  A/B por objetos sobre el cierre transitivo).
- **`eProject` (268 B)**: sin ensayos nuevos; la r21 dejo veda de orden.
- **`epCalculate` reescrito sobre `ib16`** (§4), que es la pista fuerte.
- **`Render`**: el allocno extra que le falta al objetivo. Descartadas por medida
  las vias de fuente (DWARF identico) y las de pila muerta (r21/r22).
- **El permutador**, ni guiado ni ciego, en ninguna funcion.
- **Extrapolar el mecanismo del §1 al resto del arbol**: he barrido `src/` con
  grep buscando `((x&M) ^ (y&M)) & (y&M)` y **no aparece en ningun otro sitio**
  (los `^`+`&` de `steering.c`, `RigidBody.cpp`, `Observer.cpp`, `RoadblockFlow`,
  `StrategyFlow` y los dos de `EcstasyE.cpp:2781/2937` ya usan una variable o no
  repiten el subarbol). **No es un frente, es un caso** — contado, no supuesto.

## 8. Lo que queda en el arbol

Dos ficheros, los dos solo dentro de `zEcstasy` (ningun otro SourceList los
incluye):

- `src/Speed/GameCube/Src/Ecstasy/EcstasyE.cpp` — el helper `static inline
  eProgressiveScanButtonReleased` (4 lineas) y los seis sitios sustituidos.
- `src/Speed/Indep/Src/Ecstasy/EmitterSystem.cpp` — el `||` partido en dos `if`
  en `UpdateParticles` (+4 lineas) y la anotacion de `Render` ampliada con las
  dos vedas nuevas.
