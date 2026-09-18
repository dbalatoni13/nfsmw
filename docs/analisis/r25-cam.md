# Ronda 25 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** `measure.py --cmp` (base y medida con
`build_direct.py` delante, seguidas): **+0 B, +0 funciones, 0 unidades cambian**.
`audit.py`: **446/446 ok, cero FALLA**, y las dos pasadas dan ficheros **byte a
byte idénticos**. El árbol queda **byte a byte como lo encontré** (md5 verificado
de los cinco ficheros que se sondearon).

Lo que sí traigo:

1. **`LoadCameraShakes`: el mecanismo está resuelto y medido, y la función pasa
   de 96,667 % a 99,476 % con el TAMAÑO EXACTO (168/168) y el multiconjunto
   exacto.** Los 4 diffs que quedan son **un solo registro** y su causa está
   identificada al nivel del pase. Revertido porque 99,476 % son **cero bytes** y
   la palanca es una sentencia inventada. (§2)
2. **La static-init: la dirección DESCENDENTE, que nadie había medido en cuatro
   rondas, está medida y QUEDA DESCARTADA.** Es un gradiente monótono (un `addi`
   por cada ~8-12 insns) y llega a cero en **−49 instrucciones**; eso exigiría que
   el objetivo tuviera 49 instrucciones FINALES menos en el prefijo, lo que el
   casamiento 1:1 desmiente. **La única salida sigue siendo hacia arriba
   (+26..31 insns RTL muertas).** (§3)
3. **Corrección al diagnóstico de `_Storage` de la r24**: `qty_first_reg` no se
   queda en `reg100`; la cadena de copias lo va promocionando
   (`reg100→reg101→reg109→reg111`). El punto de parada real es **`srcBeg`**, no el
   temporal de `begin()`. (§4)
4. **Una trampa de herramienta que me costó un falso positivo y que hay que
   escribir**: `triage.py` **no imprime los nombres de la sección MURO**. Una
   función que sale de «FALTA O SOBRA CODIGO» y cae en MURO **desaparece del
   listado**, y eso se lee como «ha cerrado». (§6)

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera` reproducen el brief clavado:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2...  faltan 1, sobran 2, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 4 funciones, 7000 B

`audit.py Speed/Indep/SourceLists/zCamera`: **446/446 ok, cero FALLA**, al empezar
y al terminar. **Lista de FALLA: vacía.** `pines.py`: ningún pin de zCamera en
función que no case.

## 1. Lo que dice el objetivo, leído entero (dato nuevo)

`lmap.py` sobre `LoadCameraShakes` da la función completa del objetivo (42
instrucciones) y **el hermano que nadie había mirado**: `LoadCameraSet`
(0x8007EC8C, 344 B, **ya al 100 %** en nuestro árbol) tiene en el volcado DWARF
**las mismas tres locales** — `unsigned int id`, `bool warned_overflow`,
`bChunk *chunk` — y **la misma estructura interna**, línea por línea:

    LoadCameraShakes                     LoadCameraSet
    6812 if (group)                      6739 if (group != 0)
    6813 [nota SIN codigo]               6740 [nota SIN codigo]
    6814 int num_tracks                  6741 int num_tracks
    6815 track = ...                     6742 track = ...
    6816 for (i...)                      6743 for (i...)
    6817 [nada]                          6744 [nada]
    6818 PlatEndianSwap                  6745 PlatEndianSwap
    6819 AddTrack                        6746 AddTrack
    6820 track = ...                     6747 track = ...
    6821 }                               6748 }

O sea `LoadCameraShakes` es **un copia-pega de `LoadCameraSet` sin el bucle
exterior de chunks** (de ahí el `bChunk *chunk` muerto: en `LoadCameraSet` es la
variable del `for` exterior, y en el original se llama `chunk` porque el
parámetro se llama `set_chunk`). Y la línea 6813 / 6740 —que **no emite código**—
es `warned_overflow = false;`, **dentro del `if (group)`**, en las dos.

**Nota de fidelidad, no aplicada**: nuestro `LoadCameraSet` pone
`bool warned_overflow = false;` en la cabecera de la función (línea 228) y el
original lo pone dentro del `if (group)` (6740). Es byte-neutro allí (la función
está al 100 %) y **no lo he tocado** para no mover una función que ya casa.

**El presupuesto de líneas del original**, reconstruido con los dos anclajes
(6812↔962 y 6816↔966):

    3 lineas de mas   entre la cabecera y `group = pShakeGroup`  (6802..6810)
    1 linea  de mas   dentro del bucle (la `{` en linea aparte, o un blanco)
    2 lineas de mas   entre la `}` del for (6821) y la `}` de la funcion (6826)

**Esas DOS líneas del final son todo el sitio que hay** para lo que la §2
demuestra que hace falta.

## 2. `LoadCameraShakes` (168 B): 96,667 → 99,476 %, y el muro que queda es UNO

### 2.1 El mecanismo, confirmado con el volcado RTL

El objetivo materializa el cero **dos veces** y la guarda de entrada del `for`
compara **la que no es el contador**:

    objetivo:  lwz r28,0x8(r29) ; li r0,0 ; addi r30,r29,0xc ; li r27,0 ; cmpw r0,r28
    nuestro :  lwz r27,0x8(r29) ; li r28,0 ; addi r30,r29,0xc ;           cmpw r28,r27

El RTL de un caso mínimo (`c25cam_min.py`, 1 s por compilación, con los cflags
exactos de zCamera) lo explica entero:

- `.rtl` — `warned_overflow = false` es la **insn 27** (`set reg83, 0`), y el
  `i = 0` del `for` es la **insn 40** (`set reg90, 0`).
- `.jump` — **la insn 27 ya no existe**: `jump_optimize` la borra por muerta
  **antes** de `reg_scan`, así que CSE ni la ve. Ése es el motivo por el que las
  **22 formas** de la r21/r22/r23 dieron todas el mismo ensamblador: barrían la
  posición de una sentencia que se borra siempre.
- `.jump` — la guarda de entrada la crea `duplicate_loop_exit_test` (insns 95/96)
  y usa **`reg90` directamente**.
- `.cse` — `;; Processing block from 2 to 48`: el bloque básico de CSE **termina
  en la etiqueta del bucle**, así que la primera cláusula de `make_regs_eqv`
  (`último_uso(i) > cse_basic_block_end`) es **siempre cierta**. Lo único que
  decide es la segunda: `último_uso(i) > último_uso(warned_overflow)`.

**Conclusión operativa: `warned_overflow` necesita una REFERENCIA posterior al
último uso de `i`, o sea DESPUÉS del bucle, y que sobreviva a `jump_optimize`.**
Es exactamente lo que apuntaba el brief, y ahora está al nivel de la condición
del compilador.

### 2.2 Qué referencia sirve y cuál no (medido, 40 compilaciones del árbol)

| forma detrás del bucle | resultado |
|---|---|
| **`if (warned_overflow) { <store a una local viva> }`** | **99,476 %, 168/168 B, 4 diffs** |
| `if (warned_overflow) { }` (cuerpo vacío) | 96,667 % (base): `jump` borra el `if` entero |
| `bool unused = warned_overflow;` / `(void) w;` / `w = w;` | 96,667 % (base) |
| `num_tracks = warned_overflow;` (copia muerta) | 96,667 % (base) |
| `if (warned_overflow) return;` | 96,667 % (base) |
| `if (warned_overflow) break;` (dentro del bucle) | 68,786 % |
| `if (!w) w = true;` dentro del bucle | 83,881 % |
| `if (warned_overflow) <llamada>` | 96,667 %… y +6 insns |

**El cuerpo del `if` da exactamente igual**: `group = 0`, `track = 0`,
`num_tracks = 0`, `p_handle = 0`, `group = pShakeGroup`,
`num_tracks = group->GetNumTracks()`, `track = (ICEShakeTrack*)p_handle`,
`p_handle = (unsigned int*)track` — **las ocho dan el MISMO objeto**. Lo único que
cuenta es que sea **un almacenamiento a una local que tiene otros usos** (para que
`jump_optimize` no lo borre) **dentro de una rama** (para que la referencia a
`warned_overflow` llegue viva a `reg_scan`).

Con eso, el diff de la función pasa de `faltan 1` a **cuatro `ARG_MISMATCH`**:

    >>>  2  stmw r27, 0xc(r1)   | stmw r26, 0x8(r1)
    >>> 12  li   r0,  0x0       | li   r26, 0x0
    >>> 15  cmpw r0,  r28       | cmpw r26, r28
    >>> 39  lmw  r27, 0xc(r1)   | lmw  r26, 0x8(r1)

Las otras 38 instrucciones **casan**, el tamaño es **168/168** y el
multiconjunto es **exacto**.

### 2.3 El muro que queda, con nombre y apellidos

**`warned_overflow` se lleva `r26` (salvado por el llamado) y el objetivo le da
`r0` (volátil).** La causa es que en nuestra versión la referencia posterior al
bucle **sigue viva cuando corre `local-alloc`**: el `if` cruza el `bl
PlatEndianSwap`, así que el pseudo necesita un registro salvado, y eso ensancha
el `stmw`/`lmw` de r27 a r26.

En el objetivo el pseudo vive **sólo entre su `li` y el `cmpw`**. O sea: la
referencia posterior al bucle **existe en `reg_scan` (antes de `cse_main` #1) y ya
no existe en `local-alloc`**. La ventana en la que tendría que desaparecer es
`delete_trivially_dead_insns` + `jump_optimize` #2, justo detrás de CSE1; con un
almacenamiento muerto no ocurre, porque su destino conserva usos en todo el resto
de la función y no muere hasta `life_analysis`, que ya es tarde.

Dato que lo corrobora: **en el objetivo el `li r0,0` lleva la línea 6816, la del
`for`, no la 6813**, y la 6813 aparece como nota **huérfana** compartiendo
dirección con la 6814. Eso es la firma de un pseudo con `reg_equiv_constant`:
`local-alloc` borra la insn que lo define (deja la nota suelta) y `reload`
rematerializa la constante **delante del uso** (de ahí la línea del `for`, y de
ahí **r0**, que es el registro de recarga).

**Requisito exacto para cerrar los 168 B**, por si alguien encuentra la forma:

> una referencia a `warned_overflow` posterior al último uso de `i`, que
> sobreviva a `jump_optimize` #1, y que **desaparezca antes de `local-alloc`**,
> dejando el pseudo con exactamente dos referencias (su `set` y el `cmpw` de la
> guarda) para que `update_equiv_regs` lo haga `reg_equiv_constant`.

**Revertido**, por dos razones y las dos mandan: 99,476 % son **cero bytes**
(`matched_code` es todo-o-nada), y la sentencia es **un constructo que el
original no tenía** — el mapa de líneas dice que el original **no** ejecuta un
`if (warned_overflow)` detrás del bucle (su `li` está en la línea del `for`, no en
la de `warned_overflow`).

## 3. La static-init (3.604 B): la dirección DESCENDENTE, medida y descartada

Arnés nuevo, `c25cam_si.py`: compila **`zCamera.cpp` entero** a un `.s` del
scratchpad con los cflags reales (27 s) y cuenta los
`addi rX,r30,{0x30,0x48,0x60,0x78}` dentro de
`__static_initialization_and_destruction_0`. Línea base: **898 insns, 4 `addi` de
más** (reproduce el brief).

### 3.1 Veda: el constructor de `TheICEManager` NO es palanca

`ICEManager.cpp` aporta los índices 767-789 del objetivo, pero eso **no es su
constructor**: `lmap` enseña que el objetivo hace `lis`/`addi`/`bl __10ICEManager`
en `ICEManager.cpp:368`, o sea **3 instrucciones y la llamada fuera de línea**.
Comprobado quitando 2, 4, 6, 8, 10 y 13 sentencias del cuerpo del constructor:
**las seis sondas dan 898 insns y 4 `addi`**, idéntico. Lo que llena esa zona son
los globales vectoriales de `Still.cpp` e `ICEMover.cpp` (`bMath.hpp:905`,
`ICEMath.hpp:35`).

### 3.2 El gradiente descendente (nunca medido; la r21 sólo probó hacia arriba)

Sondas quitando la inicialización dinámica de globales del **prefijo**
(`bVector3 X(a,b,c);` → `bVector3 X;`, cuyo constructor por defecto está **vacío**
en `bMath.hpp:455`), sobre `Still.cpp`, `DebugWorld.cpp`, `CopView.cpp` y
`Cubic.cpp`:

| sonda | insns de la fn | `addi` de más |
|---|---|---|
| base | 898 | **4** |
| −1 global | 889 | 4 |
| −2 globales | 881 | 3 |
| −3 globales | 873/874 | 3 |
| −4 globales | 868 | **2** |
| −5 globales | 862 | **1** |
| −6 globales | 857 | 1 |
| −7 globales | 849 | **0** |
| −9 globales | 833 | 0 |

**Es monótono** (un `addi` por cada ~8-12 instrucciones), a diferencia de la
dirección ascendente de la r21, que primero **empeora** (4 → 5 → 5 → 5) y sólo se
arregla en el salto de N=8 a N=10.

### 3.3 Y por eso queda DESCARTADA

Llegar a cero por abajo exige **−49 instrucciones en el prefijo**. Esas 49 no
pueden ser insns RTL que mueren: la sonda las quita del **código final**. El
objetivo tiene **901** instrucciones y nosotros **905** (la diferencia son
exactamente los 4 `addi`), y la r21/r23 verificaron el casamiento 1:1 salvo seis
permutaciones en 549-564. Un prefijo 49 instrucciones más corto es incompatible
con eso.

**Veda nueva, con la medida:** la salida por abajo del pozo de
`cse_basic_block`/`flush_hash_table` **no es la del original**. La única
posibilidad sigue siendo la de la r21/r22: **+26..31 insns RTL en el prefijo que
mueran antes del código final**, o sea una **forma de expresión** distinta en
alguno de los globales de `Camera.cpp`/`Cubic.cpp`/`CopView.cpp`/`DebugWorld.cpp`/
`Still.cpp`. Con el árbol de inlines idéntico (veda 1 de la r22) eso no lo puede
señalar el DWARF.

## 4. Corrección al diagnóstico de `_Storage` de la r24

La r24 concluyó: «`firstr` es `reg100`, el temporal que devuelve `begin()`, cuyo
último uso es la insn 63 — **no hay forma de fuente que lo mueva**». Eso está
**incompleto**: `qty_first_reg` **no se queda en `reg100`**, porque
`make_regs_eqv` se vuelve a llamar en cada eslabón de la cadena de copias y va
promocionando al más longevo:

    62  reg100 = mem(reg83)     begin()        ultimo uso 63
    63  reg101 = reg100                        ultimo uso 83   -> reg101 canonico
    83  reg109 = reg101         param srcBeg   ultimo uso 88   -> reg109 canonico
    87  reg111 = reg109         srcIt          ultimo uso 965  -> reg111 canonico

**El punto de parada real es `reg109` = `srcBeg`, no `reg100`.** La condición
para que `srcIt` NO desplace a `srcBeg` es `último_uso(srcIt) ≤ último_uso(srcBeg)`,
o sea **un uso de `srcBeg` posterior al último bucle de copia** — que es la misma
familia de palanca que la §2.

La r24 probó eso (sus sondas `p1`/`p3`, «uso muerto de `srcBeg` detrás del último
bucle») y midió «sin efecto: GCC borra la copia muerta antes de CSE». **La §2
explica por qué y da la corrección**: un uso **muerto** se borra en
`jump_optimize` #1; **una rama sobre `srcBeg` no**. Eso **no está probado** y es
lo que yo haría primero si se retoma (§7).

**Aviso**: `srcBeg` es un parámetro de `assign()`, que vive en `UTLVector.h`, una
cabecera que ven **más de veinte unidades**. Cualquier ensayo ahí va sobre la
copia del scratchpad (`c24inc/`, con `-Ipre`), nunca sobre el árbol, y el A/B
final tendría que ser sobre los `.ii` preprocesados.

## 5. Ensayos numerados, con su cifra

Caso mínimo (`c25cam_shake.py`, 1 s por variante; métrica = nº de `li rX,0` y
operandos de la guarda):

    n0   base (warned_overflow declarada y asignada)      26 insns  li0=1  cmpw i,n
    n1   + `if (warned_overflow) { }` detras del bucle    26  li0=1   (jump borra el if)
    n2   igual, fuera del `if (group)`                    26  li0=1
    n3   igual, fuera del bloque anonimo                  26  li0=1
    n4   `bool unused = warned_overflow;`                 26  li0=1
    n5   `warned_overflow = warned_overflow;`             26  li0=1
    n6   `if (warned_overflow) Warn("x");`                32  li0=2  cmpw W,n   (+6 insns)
    n7   `if (!w) w = true;` dentro del bucle             30  li0=2  cmpw i,n
    n8   n7 + n1                                          30  li0=2  cmpw i,n
    n9   `(void) warned_overflow;`                        26  li0=1
    n10  `group->n += warned_overflow;`                   26  li0=1
    n11  `if (warned_overflow) group = 0;`                27  li0=2  cmpw W,n   <-- FORMA DEL OBJETIVO
    n12  `warned_overflow = !warned_overflow;`            26  li0=1

Sobre el árbol (`c25cam_sweepreal*.py`, parchea, compila SOLO zCamera, evalúa con
`fndiff.py` y **restaura siempre**; ~25 s por variante):

    e0/j0  base                                        96,667 %  164/168 B  faltan 1
    e1  `if (w) { group = 0; }`                        99,476 %  168/168 B  4 diffs  REVERTIDO
    e2  `if (w) { chunk = set_chunk; }`                96,667 %  (chunk no tiene otros usos)
    e3  `if (w) { track = 0; }`                        99,476 %  168/168 B  4 diffs
    e4  `if (w) { num_tracks = 0; }`                   99,476 %  168/168 B  4 diffs
    e5  `if (w) { }`                                   96,667 %
    e6  `if (w) { id = 0; }`                           91,667 %
    g1  `bool w = false;` en la declaracion (sin el interior)   96,667 %
    g2  g1 + la asignacion interior                    96,667 %
    g4  `if (!w) w = true;` dentro del bucle           83,881 %
    g5  `w = true;` dentro del bucle                   96,667 %
    g6  `if (w) num_tracks = group->GetNumTracks();`   99,476 %  4 diffs
    g7  `if (w) track = (ICEShakeTrack*)p_handle;`     99,476 %  4 diffs
    g8/g9 `if (w) group = 0;` con y sin llaves         99,476 %  4 diffs (identicos)
    h1  `if (w) return;`                               96,667 %
    h2  `if (w) break;` dentro del bucle               68,786 %
    h3  `if (w) p_handle = 0;`                         99,476 %  4 diffs
    h4  `if (w) num_tracks = 0;`                       99,476 %  4 diffs
    h5  `if (w) track = 0;`                            99,476 %  4 diffs
    i1  `num_tracks = warned_overflow;`                96,667 %
    i2  `track = track + warned_overflow;`             91,667 %
    i3  `p_handle = p_handle + warned_overflow;`       91,667 %
    i4  `num_tracks = num_tracks + warned_overflow;`   96,667 %
    i5  `group = group + warned_overflow;`             91,667 %
    k1  `w = false;` + `if (w) group = 0;`             91,667 %  164 B  17 diffs
    k2  `if (w) { w = false; group = 0; }`             99,476 %  4 diffs
    k3  `if (w) { group = 0; w = false; }`             99,476 %  4 diffs
    m1..m8  ocho cuerpos distintos del `if`            **los ocho: 99,476 %, 4 diffs IDENTICOS**

Static-init (`c25cam_si.py` / `c25cam_siprobe*.py`, 27 s por sonda):

    base                                          898 insns   4 addi
    q2,q4,q6,q8,q10,q13  (quitar sentencias del ctor de ICEManager)
                                                  898 insns   4 addi   LAS SEIS IDENTICAS
    neg1..neg3  (quitar globales de Still.cpp)     889/881/873  4/3/3 addi
    n3..n9      (quitar inits del prefijo)         874..833     3/2/1/1/0/0 addi

## 6. Trampa de herramienta (nueva, y me costó un falso positivo)

**`triage.py` no imprime los nombres de la sección `MURO`** — sólo el recuento.
Una función que deja de tener `faltan/sobran` y pasa a ser permutación de
registros **desaparece del listado**, y un detector que busque su nombre en la
salida lee «ha cerrado». Me pasó con `e1`: `triage` dejó de listar
`LoadCameraShakes` y `MURO` subió de **4 funciones / 7.000 B a 5 / 7.168 B**;
`measure.py --cmp` decía **+0 B** y ésa era la verdad.

**Regla**: para evaluar una función concreta usa
`fndiff.py <unidad> <simbolo>` (da `target=` / `ours=` / `size=x/y` y las filas
`>>>`) o `pctsnap.py`, **no la presencia en `triage.py`**. Y ojo con el JSON de
`pctsnap`: el valor es una **lista `[porcentaje, bytes_nuestros]`**, no un número.

Segunda nota: en `c25cam_si.py` mi contador de instrucciones da **898** donde la
r21 daba 905. Es la misma función; la diferencia viene de los flags del arnés
(sin `-gdwarf+`) y del filtro de líneas. **Las comparaciones de la §3 son todas
relativas dentro del mismo arnés**, así que no afecta a la conclusión — pero no
se deben mezclar con las cifras absolutas de la r21.

## 7. Qué NO he probado

- **`_Storage` (1.156 B): una RAMA sobre `srcBeg` detrás del último bucle de
  `assign`.** Es la corrección directa de la §4 a las sondas `p1`/`p3` de la r24
  (que eran usos muertos y se borran en `jump_optimize` #1). Es lo que yo haría
  primero. **No lo he probado** porque toca `UTLVector.h` y quería dejar el árbol
  intacto; el arnés de la r24 (`c24inc/` + `c25cam_min.py` con `-Ipre`) lo hace en
  1 s sin tocar el árbol compartido.
- **`_Storage`: el volcado `.cse2`** que pedía el brief. Saqué el `.rtl`/`.jump`/
  `.cse` del caso mínimo de `LoadCameraShakes`, no el de la plantilla.
  Y queda una **contradicción sin resolver** que hay que mirar ahí: en el objetivo
  el `subf` usa `srcBeg` (r0) **y** la comparación usa `srcIt` (r27) —
  **dos registros distintos para la misma cantidad**, que `canon_reg` no debería
  permitir si CSE los hubiera fundido. O CSE **no** los fundió (y entonces la
  §4 no aplica), o el `mr.` sale por otro camino de `combine`. **Sin ese volcado
  no se puede decidir**, y es la pregunta que decide los 1.156 B.
- **`ICEMover::Update` (3.868 B)**: he sacado el diff y el mapa de líneas de la
  ventana (589-617). Los 15 diffs son **la misma familia** que la §2 y la §4: el
  objetivo compara el **ORIGEN** (`cmpwi r8,0` tras `lwz r8,0x15c(r1)`) y nosotros
  la **COPIA** (`cmpwi r11,0` tras `mr r11,r9`), donde la copia es la del
  parámetro `a` de `SignedMod` (línea `ICEMath.hpp:136`, la de apertura). El
  origen es el resultado de `FloatToInt(...)` y **no tiene ningún otro uso en la
  fuente**, así que la palanca de la §2 (alargarle la vida al origen) no tiene
  dónde agarrarse sin inventar. **No he compilado `ICEMover.cpp` suelto ni le he
  sacado el `.cse`** — sigue siendo la pendiente que dejó la r24.
- **`TrackCop` (948 B), `TrackCar` (992 B), `TerrainVelocityNoise` (1.192 B)**: no
  las he tocado. Las vedas de la r22/r23/r24 siguen en pie.
- **La vtable `_vt.Q33Sim9Collision9IListener`** y el mecanismo del
  `#pragma interface`: **no lo he intentado**. Es `.rodata`, no `.text`, y la r24
  ya midió que ganar `.data` da **+0 B** en `matched_code`; con el disco al 98 % y
  14 GB libres no quise abrir un A/B de 5 SourceLists.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**. El
  brief lo condicionaba a cerrar algo que ganara bytes y no he ganado ninguno.
- **`permuter.py`**: sigue sin usarse en zCamera.
- **La corrección de fidelidad de `LoadCameraSet`** (mover
  `bool warned_overflow = false;` de la cabecera al interior del `if (group)`,
  §1): confirmada por el mapa de líneas del objetivo, **no aplicada** por no mover
  una función que ya está al 100 %.

## 8. Verificación final

    build_direct.py zCamera                          ok
    triage.py zCamera                                identico al brief (3 near-miss, 4 muros)
    measure.py --cmp base/final (las dos con build)  +0 B, +0 funciones, 0 unidades cambian
    audit.py Speed/Indep/SourceLists/zCamera         446/446 ok, CERO FALLA
      -> segunda pasada: los dos ficheros BYTE A BYTE IDENTICOS
    pines.py                                         ningun pin de zCamera en funcion que no case
    md5 de los 5 ficheros sondeados                  identicos a la copia de partida
    git status -- src/Speed/Indep/Src/Camera/        vacio

`frozen.py chk` **no lo he tocado**: no he cambiado nada, así que la huella
congelada (la de la r19, desfasada desde la r22) sigue como estaba.

## 9. Herramientas nuevas (scratchpad, prefijo `c25cam_`)

    c25cam_min.py         compila un .cpp con los cflags EXACTOS de zCamera y
                          devuelve SOLO las instrucciones del cuerpo (1 s).
                          Se importa como modulo (`from c25cam_min import body`).
    c25cam_shake.py       barrido de formas de LoadCameraShakes sobre el caso
                          minimo; mide nº de `li 0` y operandos de la guarda.
    c25cam_sweepreal2.py  barrido SOBRE EL ARBOL: reconstruye la funcion entera
                          desde una plantilla (`build(decl_init, inner_init,
                          body_extra, post_loop)`), compila zCamera, evalua con
                          fndiff.py y RESTAURA SIEMPRE. ~25 s por variante.
    c25cam_sweepreal3..6  tandas sucesivas sobre ese mismo motor.
    c25cam_si.py          compila zCamera.cpp ENTERO a un .s del scratchpad y
                          cuenta los `addi` de mas de la static-init (27 s).
    c25cam_siprobe.py     sonda: quita sentencias del ctor de ICEManager.
    c25cam_siprobe2.py    sonda: quita globales de Still.cpp.
    c25cam_siprobe4.py    sonda GRADUADA: sustituye `bVector3 X(a,b,c);` por
                          `bVector3 X;` en globales del prefijo (Still,
                          DebugWorld, CopView, Cubic). Es la que da la tabla §3.2.
    c25rtl/               volcados por pase del caso minimo (.rtl .jump .cse
                          .cse2 .gcse .loop .flow .combine .lreg .greg .sched2)

El disco arrancó con **14 GB** libres y acaba con **14 GB**: los `.s` de las
sondas se borran solos y los volcados RTL del caso mínimo pesan unos 100 kB en
total.
