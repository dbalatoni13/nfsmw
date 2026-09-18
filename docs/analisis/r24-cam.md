# Ronda 24 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** `measure.py --cmp` (base y medida con
`build_direct.py` delante, seguidas): **+0 B, +0 funciones, 0 unidades cambian**.
`audit.py`: **446/446 ok, cero FALLA**, dos pasadas byte a byte idénticas.

Lo que sí traigo:

1. **Seis símbolos de datos que el objetivo define en zCamera y a nosotros nos
   faltaban en TODO el árbol** — encontrados con `symtabdiff.py`, que nadie había
   pasado a esta unidad en cuatro rondas. Corregido: `.data` de zCamera pasa de
   **91,571 % a 92,131 %**, seis símbolos nuevos, **EMPEORAN: 0**. (§1)
2. **La causa raíz de `_Storage` está identificada al nivel del volcado RTL, y la
   de la r23 era INCORRECTA.** No es `reg_used_between_p` de `combine`: es que
   `flow` sólo pone LOG_LINK en el **PRIMER uso** de un registro. (§2)
3. **`TrackCop` deja de ser «un empate de `sched2`» y pasa a ser un número**: las
   dos instrucciones se emiten en **el MISMO ciclo (t=24)** en unidades distintas
   (LSU / IU2); lo que decide es el orden de extracción de la lista de listos.
   Y `mr r5,r3` ya gana a **cinco de las seis** `stfs`; sólo pierde con la
   primera. (§3)
4. **Un arnés de 1,1 s** para `_Storage` (caso mínimo de plantilla con la
   cabecera del árbol TAPADA por una copia del scratchpad) y **el volcado del
   planificador con `-fsched-verbose`**, que no estaba en `HERRAMIENTAS.md`. (§6)

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera` reproducen el brief clavado:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2...  faltan 1, sobran 2, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 3868 ICEMover::Update / 1192 TerrainVelocityNoise / 992 TrackCar / 948 TrackCop

`audit.py Speed/Indep/SourceLists/zCamera`: **446/446 ok, cero FALLA**, al empezar
y al terminar, con las dos pasadas de confirmación idénticas.
**Lista de FALLA: vacía.**

## 1. LO QUE SE QUEDA: seis símbolos de datos que zCamera no definía (ensayo v2)

`symtabdiff.py zCamera` — **no se había pasado nunca a esta unidad**:

    UND  OBJECT .data    28B  PovHandheldChopperScale
    UND  OBJECT .data    28B  PovHandheldNoiseScale
    UND  OBJECT .data    28B  PovTerrainNoiseScale
    UND  OBJECT .data    28B  PovVelocityNoiseScale
    UND  OBJECT .data     4B  Tweak_EnableICEAuthoring
    UND  OBJECT .data     4B  Tweak_ForceICEReplay
    UND  OBJECT .rodata  24B  _vt.Q33Sim9Collision9IListener   <- sigue faltando

Barrido de **todos** los `.o` de `build/GOWE69/src`: **ninguno los definía**. Los
resolvía el `.s` original en el enlace, así que no rompían nada y nadie los veía.
Los cuatro `Pov*Scale` estaban como `extern float x[]` en `Cubic.cpp` y los dos
`Tweak_*` como `extern int` en `CDActionDebug.cpp`, `CDActionIce.cpp` e
`ICEReplay.cpp`.

Valores leídos del `.data` del `zCamera.o` **objetivo** (no inventados):

    PovHandheldNoiseScale    7 x 0.0f
    PovHandheldChopperScale  7 x 1.0f
    PovVelocityNoiseScale    7 x 1.0f
    PovTerrainNoiseScale     7 x 1.0f
    Tweak_EnableICEAuthoring 0
    Tweak_ForceICEReplay     0

Definidos en `Cubic.cpp` (los arrays) y en `CDActionIce.cpp` (los dos `int`).
**La colocación es correcta**: en el objetivo los `Tweak_*` están en `.data+0xa8`
y `+0xac` y los `Pov*` en `+0xa90/0xaac/0xac8/0xae4`; con `CDActionIce.cpp` en la
línea 26 del SourceList y `Cubic.cpp` en la 30, los nuestros caen en
`+0x28/+0x2c` y `+0x30/0x4c/0x68/0x84` — **mismo orden relativo, mismo espaciado,
misma sección y mismo tamaño**.

**A/B con el arnés, base/v2/base/v2 (cada pareja seguida):**

    base   120821/145005 B  83,3220 %  609 fn100
    v2     120941/145125 B  83,3357 %  615 fn100
    base   120821/145005 B  83,3220 %  609 fn100   <- repetido, estable
    v2     120941/145125 B  83,3357 %  615 fn100

Por símbolo, sobre los 621 de zCamera: **EMPEORAN 0, MEJORAN 1** (`[.data-0]`
91,571 → 92,131 %), **6 nuevos**. Las **siete** funciones vigiladas
(`_Storage`, `LoadShakes`, static-init, `ICEMover`, `TrackCar`, `TrackCop`,
`TerrainVN`) quedan con el **porcentaje idéntico al dígito**.

`measure.py --cmp` da **+0 B**: `matched_code` sólo cuenta `.text`, así que estos
120 B no puntúan. **Se queda igualmente** por la regla del brief (corrección
estructural real, ninguna unidad baja) y porque es requisito para promocionar la
unidad algún día. **Sin efecto fuera de zCamera**: `Cubic.cpp` y `CDActionIce.cpp`
sólo los incluye `zCamera.cpp` (comprobado por grep en todos los SourceLists).

**Lo que NO he hecho: la vtable `_vt.Q33Sim9Collision9IListener` (24 B).** Es el
único símbolo que sigue faltando. Datos medidos: **de las 33 SourceLists del
objetivo, sólo zCamera la define**, y **ninguna define
`OnCollision__Q23Sim9Collision9IListenerRCQ23Sim9Collision4Info`**. Nuestro
`IListener` (en `src/Speed/Indep/Src/Interfaces/IListener.h`) declara
`virtual void OnCollision(const class Info &)` sin cuerpo, así que GCC la toma
como *key function* ausente y deja la vtable externa. Tocar esa cabecera afecta a
**14 clases derivadas repartidas por zAI, zPhysics, zSim, zSpeech y zCamera**; no
lo he intentado sin poder medir el A/B completo. Queda como encargo propio.

## 2. `_Storage` (1.156 B): la causa raíz, con el volcado RTL delante

La r23 dejó esto como «la vía es el volcado `.cse2` sobre un caso mínimo de
plantilla, que es justo lo que quedó sin hacer». **Hecho.** Y el diagnóstico que
sale corrige el de la r23.

### El arnés: caso mínimo que reproduce nuestra forma CLAVADA, en 1,1 s

`c24cam_stor.cpp` (20 líneas: incluye `UTLVector.h`, declara
`_Storage<Director*,2>` y fuerza el constructor de copia) compilado con los
cflags **exactos** de zCamera da:

    lwz 0,8(4) ; lwz 9,0(4) ; slwi 0,0,2 ; mr 30,9 ; add 24,9,0 ;
    subf 0,30,24 ; cmpwi 0,30,0 ; srawi 26,0,2

que es **nuestra forma instrucción a instrucción** (sólo cambia el número de
registro por la presión distinta). El objetivo quiere:

    lwz 9,8(4) ; lwz 0,0(4) ; slwi 9,9,2 ; add 24,0,9 ;
    mr. 27,0 ; subf 9,0,24 ; srawi 26,9,2

o sea **una sola instrucción menos**: `mr`+`cmpwi` fundidos en `mr.`, con el
`subf` intacto.

### Lo que dicen los volcados (`-da` sobre un `.ii` propio)

RTL de entrada (`.jump`), tras expandir `assign(src.begin(), src.end())`:

    62  reg100 = mem(reg83)          ; mBegin, de begin()
    63  reg101 = reg100
    72  reg103 = mem(reg83+8)        ; mSize
    74  reg105 = reg103 << 2
    75  reg106 = mem(reg83)          ; mBegin OTRA VEZ, de end()
    76  reg107 = reg106 + reg105     ; srcEnd
    83  reg109 = reg101              ; parametro srcBeg
    84  reg110 = reg108              ; parametro srcEnd
    87  reg111 = reg109              ; srcIt = srcBeg
    88  reg112 = reg110 - reg109     ; srcEnd - srcBeg   <- usa srcBeg
    93  cc116  = compare(reg111, 0)  ; srcIt == 0

Tras `cse`:

    87  reg111 = reg100              <- el ORIGEN (aun no es canonico)
    88  reg112 = reg110 - reg111     <- la COPIA (ya es canonico)

**`canon_reg` reescribió `reg109` a `reg111` entre las insns 87 y 88.** Quién lo
decide es `make_regs_eqv`, y ahora está medido, no deducido:

| pseudo | primer uso | último uso |
|---|---|---|
| `reg100` (mBegin, `begin()`) | 62 | **63** |
| `reg109` (parámetro `srcBeg`) | 83 | 88 |
| `reg111` (`srcIt`) | 87 | **965** |

y el `.cse` **imprime el bloque**: `;; Processing block from 2 to 669`. Insn 669
es el `code_label` del bucle de crecimiento de `reserve()`. La condición de
`make_regs_eqv` es

    la copia se hace canonica sii  (ultimo_uso(copia) > cse_basic_block_end
                                    || primer_uso(copia) < cse_basic_block_start)
                             y     ultimo_uso(copia) > ultimo_uso(firstr)

y aquí sale **965 > 669** y **965 > 63**: las dos ciertas. `firstr` es `reg100`,
cuyo último uso es la insn 63 — **no hay forma de fuente que lo mueva**, porque
`reg100` es el temporal que devuelve `begin()`.

### La corrección al diagnóstico de la r23

La r23 dijo: «con el `subf` usando `srcBeg`, nada usa r27 entre el `mr` y el
`cmpwi`, y `combine` funde los dos». **La puerta no es `reg_used_between_p`.** El
volcado `.combine` lo enseña:

    insn 87 (set reg111 reg100)   LOG_LINKS: (nil)
    insn 88 (set reg112 (minus reg107 reg111))  LOG_LINKS: (insn_list 76 (insn_list 87 (nil)))
    insn 93 (set cc116 (compare reg111 0))      LOG_LINKS: (nil)     <-- VACIO

**`flow` sólo crea LOG_LINK para el PRIMER uso** de un registro después de su
definición. Como la insn 88 usa `reg111` antes que la 93, la comparación se queda
**sin enlace** y `combine` **ni siquiera intenta** la fusión. No es que la
rechace: no la ve.

Verificado por el lado contrario, con la variante `b1` (= c14/c17 de la r22,
`minSize` declarada antes que `srcIt`): ahí el `subf` va delante de la copia, la
comparación **sí** es el primer uso, y el `.combine` produce literalmente

    insn 93 (parallel [ (set (reg:CC 116) (compare:CC (reg:SI 100) 0))
                        (set (reg/v:SI 115) (reg:SI 100)) ])      <- el `mr.`

…pero en la misma pasada `combine` funde la insn 76 (el `add`) con el `subf` y
`simplify_plus_minus` pliega `(reg100 + reg105) - reg100` a `reg105`: **el `subf`
desaparece**. 297 → 295 insns; el objetivo quiere 296.

### Por qué no se pueden tener las dos cosas

Para que el `subf` sobreviva en el orden `b1` haría falta que la insn del `subf`
**no** tuviera LOG_LINK con el `add`, o sea que **otro uso de `srcEnd` cayera
entre los dos**. El único candidato es la copia del parámetro
(`reg110 = reg107`), y CSE la canonicaliza y `flow` la borra por muerta en las
dos variantes. Barridas las dos direcciones, ninguna la salva (§4).

**Veda nueva, con el mecanismo:** los 4 B de `_Storage` **no son alcanzables
reordenando las tres primeras sentencias de `assign`**. Con las 7 formas de la
r22/r23 van **13**, y ahora se sabe por qué: cualquier orden que dé el `mr.`
pone el `subf` en la ventana donde `combine` lo pliega, y cualquier orden que
salve el `subf` deja la comparación sin LOG_LINK.

### Y NINGUNA bandera lo mueve

Barrido de **18 banderas** sobre el caso mínimo, en las dos variantes (`a0` y
`b1`), 36 compilaciones:

    a0: -fno-cse-follow-jumps, -fno-cse-skip-blocks, las dos juntas,
        -fno-rerun-cse-after-loop, -fno-gcse, -fno-expensive-optimizations,
        -fno-force-addr, -fno-force-mem, -fno-rerun-loop-opt,
        -fno-move-all-movables, -fno-schedule-insns, -fno-schedule-insns2,
        -fno-strength-reduce, -fno-thread-jumps, -fno-peephole,
        -fno-omit-frame-pointer, -fno-defer-pop
        --> LAS 18 dan  mr.=NO  subf=SI   (la forma nuestra)
    b1: las mismas 18                --> LAS 18 dan  mr.=SI  subf=NO

**Ninguna bandera produce la combinación del objetivo.** Esto cierra también la
hipótesis de la r23 de que lo decidía `cse_end_of_basic_block` vía
`-fcse-follow-jumps`/`-fcse-skip-blocks`.

## 3. `TrackCop::Update` (948 B): del «empate de sched2» al número exacto

`TrackCop.cpp` **compila suelto** con los cflags de zCamera (6,5 s) y **reproduce
nuestra forma clavada**. Con `-da -fsched-verbose=3` sale la visualización del
planificador, que **no estaba documentada en `HERRAMIENTAS.md`**:

    ;; -- basic block 15 from 490 to 795 -- after reload
    ;;  Ready list (t = 23):    627  629
    ;;  Ready list (t = 24):    623  616  609  585  582  646  579
    ;;  clock 23    629  %4=[%31+0x218]        (lsu)   627  %3=%1+0x68   (iu2)
    ;;  clock 24    579  [%1+0x58]=%30         (lsu)   646  %5=%3        (iu2)
    ;;  clock 25    582  [%1+0x5c]=%30         (lsu)   644  %4=%4+0x48   (iu2)

Traducción:

- las dos instrucciones en litigio **se emiten en el MISMO ciclo** (t=24), en
  unidades funcionales distintas (LSU y IU2). No es que una vaya «antes» en el
  tiempo: lo que las ordena en el flujo final es **el orden de extracción de la
  lista de listos**, que `schedule_block` recorre de `ready[n-1]` hacia abajo;
- la lista sale ordenada `623 616 609 585 582 646 579`, o sea la mejor es la
  `579` (`stfs f30,0x58(r1)`, la primera de `bScale`) y la segunda la `646`
  (`mr r5,r3`);
- **`646` ya gana a CINCO de las seis `stfs`** (582, 585 de `hcomp` y 609, 616,
  623 de `look_offset`): sólo pierde con la primera. Con desempate puro por
  `INSN_LUID` la `646` sería la PEOR de las siete (es la de LUID más alto), así
  que su prioridad **sí** es más alta que la de cinco de ellas; contra la `579`
  empata y pierde el desempate por LUID.

O sea: **falta un punto de prioridad, o que la `579` tenga LUID mayor que la
`646`.** Lo segundo exige que el montaje de argumentos de `eMulVector` se emita
en la fuente **antes** que `bScale` — y eso el objetivo lo desmiente, porque sus
tres `stfs` de `hcomp` (índices 164/165/167) van **delante** de los tres de
`look_offset` (168/169/170), que es justamente el orden de LUID que ya tenemos.

**Veda nueva, con la sentencia:** mover el `bScale`/`bFill` delante o detrás del
zurcido de `look_offset` **no puede** dar el orden del objetivo: cualquiera de las
dos posiciones mantiene el desempate por LUID a favor de la primera `stfs`, y la
posición contraria rompe además el orden `hcomp` → `look_offset` que el objetivo
sí tiene. Descartado **con la medida del planificador delante**, no por
razonamiento como en la r22.

## 4. Ensayos numerados, con su cifra

Todos los del caso mínimo (§2) se miden por la **forma** del prefijo
(`mr.` presente / `subf` presente / nº de instrucciones de la función), que es lo
que distingue objetivo (296) de nosotros (297) y de `b1` (295).

    a0   forma actual del arbol                       297  mr.=NO  subf=SI   (= nosotros)
    b1   `minSize` antes que `srcIt` (= c14/c17 r22)  295  mr.=SI  subf=NO   REVERTIDO
    b2   `minSize = srcEnd - srcIt`                   297  mr.=NO  subf=SI   sin efecto
    b3   `size_type minSize;` + asignacion aparte     297  mr.=NO  subf=SI   sin efecto
    b4   `(size_type)(srcEnd - srcBeg)` con cast      297  mr.=NO  subf=SI   sin efecto
    p1   uso muerto de `srcBeg` DETRAS del ultimo bucle
         (sonda de `ultimo_uso(firstr)`)              297  mr.=NO  subf=SI   sin efecto
                                                      (GCC borra la copia muerta antes de CSE)
    p2   igual con `srcEnd` (control)                 297  mr.=NO  subf=SI   sin efecto
    p3   uso muerto de `srcBeg` antes del `pop_back`  297  mr.=NO  subf=SI   sin efecto
    p4   el ultimo bucle recorre `srcBeg` en vez de `srcIt`
                                                      299  mr.=NO  subf=SI   PEOR (+2)
    g3   `end()` escrito como `begin() + mSize`       (a0) mr.=NO  subf=SI   sin efecto
    g2   g3 + b1                                      (b1) mr.=SI  subf=NO   sin efecto

    f1..f18  las 18 banderas de la §2 sobre a0        LAS 18 mr.=NO subf=SI
    f19..f36 las mismas 18 sobre b1                   LAS 18 mr.=SI subf=NO

    v1   definir los cuatro `Pov*Scale` en Cubic.cpp
         120821 -> 120933 B, 609 -> 613 fn100                                base de v2
    v2   v1 + `Tweak_EnableICEAuthoring`/`Tweak_ForceICEReplay` en CDActionIce.cpp
         120821 -> 120941 B, 609 -> 615 fn100, `.data` 91,571 -> 92,131 %   SE QUEDA
         (repetido base/v2/base/v2: las cuatro cifras estables)

**El árbol queda con `v2` aplicado y nada más.** Todo lo demás se probó sobre una
**copia de `UTLVector.h` en el scratchpad** tapada con un `-I` propio: el árbol
compartido **no se tocó ni una vez** durante los barridos.

## 5. Vedas nuevas (con la sentencia barrida)

1. **`_Storage`: el orden de las tres primeras sentencias de `assign` está
   agotado, y ahora con el mecanismo.** Barridas `a0`, `b1`, `b2`, `b3`, `b4`,
   `g2`, `g3` (más las 7 de la r22/r23 = **13 formas**). La puerta es
   **LOG_LINKS**: `flow` enlaza sólo el **primer uso**, así que o el `subf` usa
   `srcIt` (y la comparación se queda sin enlace: nuestro caso) o el `subf` usa
   `srcBeg` (y `combine` lo pliega contra el `add`: caso `b1`).
2. **Ninguna de las 18 banderas de optimización mueve `_Storage`**, en ninguna de
   las dos variantes (36 compilaciones). Incluye las cuatro de CSE, que era la
   pista que dejó la r23.
3. **`ultimo_uso(firstr)` no es palanca en `_Storage`.** `firstr` es el temporal
   que devuelve `begin()` y muere en la insn siguiente; los usos muertos de
   `srcBeg` que se añadan (p1, p3) los borra GCC **antes** de que `reg_scan`
   cuente, así que no alargan su vida (medido: 297 insns, idéntico).
4. **`TrackCop`: mover el `bScale` no puede dar el orden del objetivo.** Medido
   con la visualización del planificador: las dos instrucciones caen en el mismo
   ciclo y en unidades distintas, y el desempate por LUID favorece siempre a la
   primera `stfs` esté donde esté el bloque.

## 6. Herramientas nuevas (scratchpad, prefijo `c24cam_`)

    c24cam_min2.py      compila un .cpp con los cflags EXACTOS de zCamera
                        INCLUYENDO las -I (el c23cam_min.py las tiraba, asi que
                        solo servia para casos sin #include). Con `--rtl`
                        preprocesa y llama a cc1plus a mano con -da: deja los 16
                        volcados por pase. Con `-Ipre <dir>` antepone un -I, que
                        es lo que permite TAPAR una cabecera del arbol sin
                        tocarla.
    c24cam_stsweep.py   barrido de formas del cuerpo de `assign` sobre el caso
                        minimo (1,1 s por variante). El cuerpo base y las
                        variantes viven en c24cam_assign_base.txt y
                        c24cam_assign_vars.py.
    c24cam_stflags.py   barrido de banderas sobre el mismo caso minimo.
    c24cam_stor.cpp     el caso minimo de _Storage<Director*,2>.
    c24inc/             copia de UTLVector.h que tapa a la del arbol.
    c24cam_ab.py        el de la r23, renombrado (parchea, compila SOLO zCamera
                        al scratchpad, mide las 7 funciones y RESTAURA siempre).
    c24tc/              TrackCop.cpp compilado SUELTO (6,5 s) + su .sched2.

**Dos cosas que faltan en `HERRAMIENTAS.md`** y que han decidido esta ronda:

- **`-fsched-verbose=3` con `-da`** hace que el `.sched2` traiga las **listas de
  listos por ciclo** y la **visualización por unidad funcional**. Es lo que
  convierte «empate de `sched2`» en un dato. Verbosidad ≥4 no añade nada en esta
  versión (no llega a imprimir `debug_dependencies`).
- **`symtabdiff.py <unidad>` sobre una unidad near-miss**: aquí destapó seis
  símbolos de datos que faltaban en TODO el árbol y que nadie veía porque los
  resolvía el `.s`. **Vale la pena pasarlo a las demás unidades atascadas.**

## 7. Qué NO he probado

- **`__static_initialization_and_destruction_0` (3.604 B): nada nuevo.** Las
  vedas de la r21/r22/r23 siguen en pie (no falta ninguna sentencia; hacen falta
  25-31 insns RTL **muertas** en el prefijo; `-fno-force-addr` vetado a −82.744 B).
  **No he sacado el volcado RTL de la static-init**: el `.rtl` de un solo `.cpp`
  suelto ya pesa 48 MB y el de `zCamera.cpp` entero sería inmanejable con 13 GB
  libres. Lo que sí es nuevo y encaja con el modelo de la r21: el umbral
  posicional que describió es casi con seguridad el
  `if (code != NOTE && num_insns++ > 1000) flush_hash_table();` de
  `cse_basic_block` — que **excluye las NOTE a propósito** para que `-g` no
  cambie el código, lo cual **confirma la veda de la r24 sobre las NOTE de
  línea** y explica por qué las sondas de la r21 funcionan por cuenta de insns.
- **`ICEMover::Update` (3.868 B)**: he sacado el mapa de líneas del objetivo para
  la ventana 589-617 y he verificado que el `mr r11,r8` del objetivo en
  `ICEMath.hpp:136` es **la copia del parámetro `a` de `SignedMod` en la línea de
  apertura de la función**, no una sentencia que nos falte (offset constante de
  14 líneas entre su `SignedMod` y el nuestro, salvo el `}` del `while` sin
  llaves que la r21 ya anotó). **No hay diferencia de fuente**, y los 15 diffs
  son una permutación de registros con el mismo mecanismo de `canon_reg` de la
  §2 (el objetivo compara el ORIGEN `r8`, nosotros la COPIA `r11`). **No he
  compilado `ICEMover.cpp` suelto ni le he sacado el `.sched2`/`.cse`** — es lo
  primero que haría la ronda siguiente, y ahora el arnés existe.
- **`LoadCameraShakes` (168 B)**: no he gastado ensayos (van 22 formas en tres
  rondas). Pero el mecanismo de la §2 lo afina: para que `canon_reg` meta el cero
  de `warned_overflow` en la guarda del `for`, su pseudo tiene que ser el
  canónico de la cantidad «constante 0», y eso exige que **su último uso caiga
  MÁS ALLÁ del `code_label` del `for`** — es decir, un uso **dentro o después del
  bucle**, no antes. Las 22 formas barridas ponen el uso **antes**. Eso reduce el
  espacio de búsqueda de forma drástica y **no está probado**.
- **`TerrainVelocityNoise` (1.192 B)** y **`TrackCar` (992 B)**: sólo he releído
  sus diffs. Los dos son permutación de registros de coma flotante más un `lis`
  desplazado; misma familia que `TrackCop`. **No los he tocado.**
- **La vtable `_vt.Q33Sim9Collision9IListener`** (§1): medido que sólo zCamera la
  define en el objetivo y que nadie define su `OnCollision`. Tocar
  `IListener.h` afecta a 14 clases derivadas en cinco SourceLists y no he hecho
  ese A/B.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**. Sigue
  revertido: no he cerrado ninguna función, y el brief lo condicionaba a ganar
  bytes. La corrección de la §1 gana `.data`, no `.text`.
- **`permuter.py`** sobre ninguna de las siete. Sigue sin usarse en zCamera.

## 8. Verificación final

    build_direct.py zCamera                        ok
    triage.py zCamera                              identico al brief (3 near-miss, 4 muros)
    measure.py --cmp base/final (las dos con build) +0 B, +0 funciones, 0 unidades cambian
    por simbolo, 621 -> 627 simbolos de zCamera    EMPEORAN 0  MEJORAN 1 (.data)  NUEVOS 6
    audit.py Speed/Indep/SourceLists/zCamera        446/446 ok, CERO FALLA
      -> segunda pasada: los dos ficheros byte a byte IDENTICOS
    symtabdiff.py zCamera                          de 7 simbolos ausentes queda 1 (la vtable)
    pines.py                                        ningun pin de zCamera en funcion que no case

`frozen.py chk Speed/Indep/SourceLists/zCamera` da **«HA CAMBIADO»**: es esperado
(seis símbolos de datos nuevos). **No lo he vuelto a congelar**, porque no he
cerrado ninguna función y la huella congelada es la de la **r19**, ya desfasada
desde la r22. Decisión tuya si se recongela.

## 9. Estado del árbol y convivencia

Ficheros modificados (los dos **exclusivos de zCamera**, verificado por grep en
todos los SourceLists):

    src/Speed/Indep/Src/Camera/Movers/Cubic.cpp            (LF,   4 definiciones + comentario)
    src/Speed/Indep/Src/Camera/Actions/CDActionIce.cpp     (CRLF, 2 definiciones)

Finales de línea respetados por fichero y verificados tras editar.
**Sin commit**, como pedía el encargo.

El disco arrancó la ronda con **14 GB** libres y acaba con **13 GB**; se han
borrado los `.rtl` (48 MB cada uno), los `.o` del arnés y **diez `.json` de más de
12 MB de la r23** que ya no hacían falta.
