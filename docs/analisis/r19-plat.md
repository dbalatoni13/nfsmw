# Ronda 19 — grupo zEagl4Anim / zBWare / zPlatform / zSim / zAnim / zEcstasy

Línea base `base_r19_plat.json`: 438.012/462.744 B, **94,6554%**, 1.932 funciones.
Cierre `despues_r19_plat.json`: **idéntico en bytes**. `measure.py --cmp` -> `+0 B,
+0 funciones, 0 unidades cambian`.

Lo que sí sale de esta ronda:

- El **mecanismo real** por el que GCC borra el nodo de rango del switch de
  `DynamicLoader::Initialize`, con el árbol del objetivo reproducido en las dos
  ramas: **90,000% -> 91,337%, `faltan 23, sobran 4` -> `faltan 14, sobran 5`**,
  +40 B de estructura correcta.
- Dos **falsos positivos nuevos y medidos** de `audit.py` y de `litcheck.py`.
- El frente `__builtin_new` de la ronda 18 **ya está cerrado** (por otro agente).
- Cinco vedas nuevas, todas con cifra.

## 1. La auditoría: las seis unidades están limpias

`audit.py` sobre las seis, dos pasadas separadas ~50 min, con
`build_direct.py` inmediatamente antes de cada una:

| unidad | ok | pasada 1 | pasada 2 |
|---|---|---|---|
| zEagl4Anim | 204 | — | — |
| zBWare | 236 | — | — |
| zPlatform | 121 | — | — |
| zSim | 313 | `Load__11NISActivity…` (`__builtin_new`) | — |
| zAnim | 286 | — | — |
| zEcstasy | 446 | `__static_init…_0`, `epInitViews__Fv` | `__static_init…_0` |

### El frente `__builtin_new` está cerrado, y no por mí

Los dos fallos de `__builtin_new` contra `__builtin_vec_new` que la ronda 18 dejó
anotados (sección 4 de `r18-plat.md`) **desaparecen entre la pasada 1 y la 2**.
No es un fantasma: **otro agente añadió `#define BWARE_NO_INLINE_GLOBAL_NEW` a
`SourceLists/zSim.cpp` y `SourceLists/zEcstasy.cpp`** durante la sesión (las dos
salen como `M` en `git status`). Verificado: `Load__11NISActivity…` y
`epInitViews__Fv` pasan a `ok` y `measure --cmp` no mueve ni un byte.
**Ese frente de la ronda 18 se puede tachar.**

### FALSO POSITIVO NUEVO: literales con `@l` NEGATIVO

El único fallo que **sí** reproduce en las dos pasadas es
`__static_initialization_and_destruction_0` de zEcstasy:

    FALLA: reubicacion en un solo lado: lis r11, 0x803e | lis r11, $LC980@ha

**No es un fallo.** El objetivo carga tres literales con desplazamiento
**negativo**:

    169  lfs f12, -0xb8c(r7)   -> 0x803E0000 - 0xb8c = 0x803DF474 = 80.0f
    341  lfs f9,  -0xb74(r15)  -> 0x803DF48C = 0.98f
    392  lfs f1,  -0xb4c(r10)  -> 0x803DF4B4 = -27.0f

Comprobados los tres leyendo el ELF. Como el troceador **no emite un `lbl_` para
una dirección alcanzada por `@l` negativo**, el `lis` del objetivo sale sin
reubicación y `audit.py` lo lee como «reubicación en un solo lado».

**Y `litcheck.py` tiene el MISMO agujero por la misma causa**: dice
`0.98 tgt=0 ours=1`, `80 tgt=0 ours=1`, `-27 tgt=0 ours=1` para esa misma
función, y los tres valores **sí están** en el objetivo. Es toda una clase:
cualquier función que cargue del pool con `@ha` redondeado hacia arriba dará
falso positivo en las dos herramientas.

Arreglo sugerido (no lo he tocado): al resolver el par `lis rN, 0xHHHH` +
`lfs/lwz fN, -0xLLL(rN)`, calcular `0xHHHH0000 + (int16)(-0xLLL)` y comparar ese
valor, en vez de exigir que exista un `lbl_`.

## 2. `DynamicLoader::Initialize` — el árbol de switch, resuelto de verdad

**90,000% -> 91,337%; 2.276 -> 2.316 B (objetivo 2.352); `faltan 23, sobran 4` ->
`faltan 14, sobran 5`.** `pctsnap --cmp`: **una sola función se mueve, ninguna
empeora**.

La pista de la ronda 18 («la única forma que conserva las comparaciones del rango
es que el cuerpo del case emita código») era el síntoma. El mecanismo es otro y
son **dos** pases de `stmt.c`:

### a) `balance_case_nodes` sólo parte una lista de MÁS DE DOS nodos

    if (i > 2) { npp = head; i /= 2; while (i-- > 0) npp = &(*npp)->right; ... }
    else       { /* la deja como cadena: nodo->right->right */ }

Con `{2,3}` en el subárbol bajo y `{9,[R]}` en el alto, **ninguno se parte**: el
2 queda con hijo DERECHO 3, y el 9 con hijo derecho el rango. Un nodo hoja cuyo
cuerpo es el `default` se emite entonces como *«prueba de rango + `b default`»*, y
`jump.c` **borra la condicional porque las dos salidas son la misma etiqueta**.
Ése es el borrado que la ronda 18 midió nueve veces sin explicar.

El objetivo tiene la forma partida en las dos ramas: raíz 3 con hijo IZQUIERDO 2,
y raíz el rango con hijo izquierdo 9. Eso **exige tres nodos por subárbol**.

### b) Los nodos que sobran no cuestan ni una instrucción si son ADYACENTES

`node_has_high_bound` / `node_has_low_bound` devuelven 1 cuando
`node->high + 1` coincide con el `low` de un ancestro (o `node->low` es el mínimo
del tipo). Un nodo con **las dos cotas conocidas** se emite como `emit_jump(label)`
a secas, y si su etiqueta es la del `default`, `jump.c` lo funde con el `bgt` del
padre: **cero instrucciones**.

### c) Y `group_case_nodes` los vuelve a fusionar si comparten salida

    while (np = np->right)  ...  simplejump_p(lb) && simplejump_p(lb2)
                                 && rtx_equal_p (SET_SRC(lb), SET_SRC(lb2))
                                 && np->low == node->high + 1  ->  node->high = np->high

Dos `case` vacíos **adyacentes** con el mismo `break` se funden en uno solo y el
conteo vuelve a caer. **Se rompe dándoles salidas distintas: `continue` en uno y
`break` en el otro** (el `continue` del `for` es otra etiqueta, aunque acabe en la
misma dirección tras `jump.c`).

### El cambio que queda en el árbol (`eagl4supportdlopen.cpp`)

    case SHT_RELA ... SHT_NOTE:            //  4..7, entre 3 y 8, con las dos cotas
        continue;                          //  `continue` para no fundirse con el 8
    ...
    case SHT_LOPROC + 7 ... SHT_HIUSER:    //  por encima del rango, high == TYPE_MAX
        continue;

Es **inerte**: los dos hacen exactamente lo que hace `default: break;`. Con ellos
la lista pasa a 7 nodos, la raíz sigue siendo el 8, y los dos subárboles se
parten. El árbol emitido pasa de

    cmpwi 8,beq / cmplwi 8,bgt ALTO / cmpwi 2,beq / cmpwi 3,beq / b default
                                   / cmplw r21,bgt / cmplw r22,bge / cmpwi 9,beq / b

a **casi** el del objetivo:

    objetivo:  cmpwi 8,beq | cmplwi 8,**ble BAJO** | ALTO... | BAJO: cmpwi 3,beq;
               cmplwi 3,bgt; **cmplwi 1,ble**; (cae) case 2
    nuestro:   cmpwi 8,beq | cmplwi 8,**bgt ALTO** | BAJO: cmpwi 3,beq;
               cmplwi 3,bgt; **cmpwi 2,beq; b**   | ALTO...

Quedan **dos** diferencias en el árbol, las dos entendidas y ninguna cerrada:

1. **La orientación de la raíz.** El objetivo emite el subárbol ALTO primero y
   salta al BAJO con `ble`; nosotros emitimos el BAJO primero y saltamos al ALTO
   con `bgt` (la rama «ninguno de los dos hijos acotado» de `emit_case_nodes`).
   Para que GCC emita el derecho primero hace falta `node_is_bounded(node->left)`,
   y eso exige que el subárbol bajo sea **una hoja sin hijos** — imposible con
   los `case 2` y `case 3` dentro. **No he encontrado la forma. Sin explicar.**
2. **El `case 2` necesita cota INFERIOR.** El objetivo lo emite como
   `cmplwi r0,1; ble default` + caída, que es la forma de un nodo con las dos
   cotas: hace falta un ancestro con `high == 1`, o sea **un `case [0..1]`**
   (`SHT_NULL ... SHT_PROGBITS`) como padre en la cadena.

**Ensayos numerados (todos medidos):**

    h1  case [4..7] break + case [LOPROC+7..HIUSER] break   -> 89,888%  (2276 B)
        `group_case_nodes` funde [4..7] con el 8 en [4..8] y el rango con
        [LOPROC+7..] : la raíz pasa a ser un rango. Descartado.
    h2  los mismos dos, con `continue`                      -> **91,337%** (2316 B)
        SE QUEDA. Árbol correcto en las dos ramas.
    i1  h2 + `case SHT_NULL ... SHT_PROGBITS: break;` arriba -> 90,422% (2316 B)
    j1  igual pero con `continue`                            -> 90,422%
    k2  igual, colocado justo antes de `default:`            -> 90,422%
        Los tres pierden UN nodo (la raíz pasa a [4..7], que es el 4.º de 7, no
        el 5.º de 8) y no he averiguado cuál se funde ni por qué. **Ahí está el
        siguiente paso**: si [0..1] sobrevive, la lista queda en 8 nodos, la raíz
        vuelve al 8, el subárbol bajo pasa a {[0..1],2,3,[4..7]} con raíz 3, y el
        `case 2` gana su cota inferior. Es la mitad de lo que falta.

### Lo que queda (`faltan 14`, `lwz+7`) NO es el switch

Con `lmap` sobre el objetivo, las 14 caen en el bucle de bytes de las secciones:

    8008F344  mtctr r8      (i*0x28)   eagl4supportdlopen.cpp:234
    8008F578  mfctr r10                eagl4supportdlopen.cpp:1102

El original **guarda `i * 0x28` en el registro CTR** durante todo el cuerpo del
bucle y lo recupera con `mfctr`, y recarga `0x98(r1)` donde nosotros conservamos
el valor en r29. Es decir: **el original tiene MÁS presión de registros que
nosotros en ese bucle**, no código distinto. Los `lwz+7` son recargas.
También cruzamos menos: en `ELFAddr` (el inline de las líneas 234-255) el
objetivo comparte la cola `add r0+off; stw 0x98(r1)` entre el camino de la
línea 236 y el de la 251 con un `b`, y nosotros la duplicamos.

## 3. `_bOutput` (zBWare, 5.180 B, 99,593%) — cinco racimos, todos de registro

Confirmado lo de la ronda 18 (1.296 contra 1.296 instrucciones) y **localizados
los cinco racimos independientes**; cerrar la función exige los cinco:

| filas | qué |
|---|---|
| 550-560 | `subf r9/r10` en el relleno de ceros |
| 592-615 | el `lis 0x7ff0` y la cascada `_nan_table` |
| 620-681 | rotación r8/r9/r11 en los `lfd` del pool de `%f` |
| 797-819 | el bucle de redondeo (`q`) |
| 914-916 | dos `addi` permutados |

### El racimo del `0x7ff0`: es REMATERIALIZACIÓN de reload, no orden de fuente

    objetivo:  lis r11,0x7ff0 ; lwz r8,0xac(r1) ; li r27,0 ; stfd ; lwz r0,4(r8)
    nuestro:   li r27,0 ; lwz r11,0xac(r1) ; stfd ; lwz r0,4(r11) ; lis r8,0x7ff0

`0xac(r1)` guarda `&number` (`addi r8,r1,0x98` + `stw r8,0xac(r1)` en el prólogo,
en los dos lados): es un invariante que `loop.c` izó y que se derramó. El `lis`
del objetivo coge **r11**, que está al final de `REG_ALLOC_ORDER` de rs6000 —
firma de registro de derrame— mientras el nuestro coge r8. Y las dos constantes
están **intercambiadas**: objetivo `0x7ff0->r11, 0xfff0->r8`; nuestro
`0x7ff0->r8, 0xfff0->r11`.

    d1  quitar el `__asm__("" : : "r"(&number))`   -> 99,434% (mismo tamaño)
        Sin el asm los dos `lwz`/`lis` se van a otro bloque: 2 INSERT + 2 DELETE.
        Descartado. **El asm reproduce el derrame del original**, no lo inventa.
    e1  mover el asm delante de `stringOut = nullptr;` -> 99,485%. Descartado.

### El racimo de `q`: el DWARF da el reparto y el porqué

El DWARF del original dice, para el bloque 0x8005F324->0x8005F388:
`char * q; // r11`. Nosotros ponemos `q` en r9 y el `*q` en r11; el objetivo hace
lo contrario y además mete el `'0'` en **r0**, que es el ÚLTIMO registro de
`REG_ALLOC_ORDER`. Con la prioridad de `allocno_compare`
(`floor_log2(n_refs)*freq/live_length`), el valor de `*q` (4 refs, rango corto)
gana a `q` (6 refs, rango largo) y por eso el objetivo le da el registro bueno.
En nuestra compilación gana `q`. **La estructura, las ramas y el `cmpwi cr7`
izado del `group_flag` coinciden fila a fila**: sólo cambia el reparto. No
probado: nada que fuerce esa prioridad sin `asm`.

## 4. `UpdateParticles` (zEcstasy, 1.820 B, 99,956%) — dos `lfs` y nada más

Sólo cuatro filas, y son **una** diferencia: el objetivo mete `257.0f` en f21 y
`1/255` en f20; nosotros al revés. Los valores son correctos (`litcheck` limpio),
el orden de carga en el precabezal es el mismo, y las dos constantes tienen
`n_refs = 2` en el mismo par de bloques.

**El DWARF prueba que la local `rot_scale` NO existe en el original** (el bloque
lista `pvel, pacc, ppos, tlife, t, extra_params, pangle, adelta, col, r, g, b, a,
ignore_programmer_badness, alpha_value_to_kill_at` y nada más) — y aun así:

    c1  declarar `rot_scale` antes de la línea de `pangle`   -> 99,692%
    c2  sin `rot_scale`, expresión entera en una sentencia   -> 99,385%
        GCC reasocia y mueve el `fmuls` de `y*(1/255)` DETRÁS de la conversión
        de `mRotOffset`; el objetivo lo tiene delante. Descartado.
    c3  igual pero con el `+ extra_params.y` al final        -> 99,385%. Igual.

O sea: **la forma que el DWARF dice que NO tiene el original es la que mejor
compila**, y la permutación f20/f21 sobrevive a las tres. Contradicción medida,
sin resolver. No probado: el permutador ciego.

## 5. `EmitterSystem::Render` (696 B, 98,075%) — dos racimos, uno explicado entero

1. **La ranura de 0xc8(r1)** (ya en `r18-plat.md`): el objetivo derrama
   `&mEmitterGroups` y lo recarga. Nada nuevo; sigue sin forma de fuente.
2. **NUEVO: la permutación f7/f8/f9 sale de `bScale`**, y el DWARF da el reparto
   exacto. La primera expansión de `bVector4::operator*=` -> `bScale` tiene
   `x//f9, y//f8, z//f7, w//f10` y la segunda `x//f11, y//f12, z//f13, w//f0`.
   **La segunda casa perfecta; la primera va rotada** (nosotros f8, f7, f9).
   La fuente de `bMath.hpp:bScale(bVector4*)` coincide una a una con el DWARF
   (cuatro locales declaradas y luego los cuatro productos), así que la forma es
   la correcta. Es cabecera compartida: no la toqué.

## 6. La familia `Eval*` — dos vedas nuevas con cifra

`FnStatelessQ::EvalSQT` (1.480 B, 98,811%, `clrlwi-2, mr+2`). El DWARF confirma
otra vez que **no hay local `times` ni local para `mNumKeys`**; las locales son
`statelessQ//r31, floorTime//r9, floorKey//r30, scale//f30, slerpReqd//r29`.

    n1  quitar los DOS `static_cast<unsigned short>(mNumKeys - 1)` -> 97,403%
    o1  quitar sólo el primero (rama `!mTimes`)                    -> 97,484%
    o2  quitar sólo el segundo (rama `else`)                       -> 98,730%

Los tres empeoran. **Veda: barridas las cuatro combinaciones del cast en
`floorKey >= (unsigned short)(statelessQ->mNumKeys - 1)`.** El objetivo no emite
`clrlwi` ahí (`subi r0,r5,1 ; cmpw r30,r0` y punto); con el cast sale detrás del
`subi`, sin él delante. Lo que falta es que `mNumKeys` llegue al `subi` sin que
GCC crea que hay que extenderla, y con `r5` alimentada desde DOS `lhz` distintos
no lo consigue ninguna de las formas probadas.

`FnRawStateChan::EvalState` (456 B, 93,509%, `add-2, mr+2`):

    m1  usar `mKeyIdx` directamente en vez de la local `keyIdx`  -> 89,868%
        y el tamaño sube a 460 B. Descartado; la local ES la del original.

Lo que le queda son tres racimos: la posición del `lwz 0x10(r29)`, la del
`subi numKeys-1`, y el par `mr r4,r11` (objetivo) contra `add r4,r30,r10`
(nuestro) en la rama impar de `GetKeyData`. El objetivo iza `this + 0xa + nf*2`
del bucle **y las dos ramas lo comparten**; nosotros izamos el desplazamiento y
una rama recalcula. La cabecera `RawStateChan.h` está bien: las dos ramas del
objetivo tienen asociaciones DISTINTAS (`add r10,r30,r9` contra
`add r9,r9,r30 ; addi r10,r9,0xc`), que es exactamente lo que genera
`&mDecodeData[nf]` contra `&mDecodeData[nf] + 1`.

## 7. Qué NO he probado

- `epCalculateLocalDirectionalPOS16` (2.072 B), `UpdatePlatInfo` (2.044),
  `eProgressiveScan_EURGB60DialogBox` (1.848), `GenerateHorizonFogDisplayList`
  (796), `FindMatchTime` (720), `Eval__FnRawLinearChannel` (488), `eProject`
  (268), `UnlockPalette` (172): **cero ensayos esta ronda**.
- `ActualReadJoystickData` (1.588 B, `mr+1, andi.+1`): cero ensayos.
- `FnPoseAnim::EvalPose` y `FnStatelessF3::EvalSQT`: cero ensayos esta ronda
  (misma familia `mr+N` que Q; la ronda 18 barrió seis formas de `times`).
- El permutador, ni guiado ni ciego, en ninguna función.
- Los dos `asm` que ya estaban commiteados (`bPrintf.cpp` con
  `__asm__("" : : "r"(&number))`, `EmitterSystem.cpp` con
  `register float world_size asm("fr6")`, `eagl4supportdlopen.cpp` con
  `register HashPointer *pHP asm("r31")`, `RawStateChan.cpp` con
  `register int keyOffset asm("r9")`): **medido que el primero es necesario**
  (quitarlo cuesta 0,16 pp), los otros tres sin tocar.

## 8. Concurrencia

Otro agente tiene en vuelo `Libs/Support/Utility/UTLVector.h` y los
`SourceLists/*.cpp` (`zEcstasy`, `zSim`, `zMain`, `zFeOverlay`,
`zPhysicsBehaviors`). Efecto medido en mis unidades: `frozen.py chk` grita en
zSim y zEcstasy, **`measure --cmp` da +0 B y `pctsnap --cmp` no marca ninguna
función a la baja**. Es su arreglo del `__builtin_new`, es correcto, y he vuelto
a congelar las seis unidades sobre el estado verificado.

## 9. Herramientas dejadas en el scratchpad

`pl19_it.sh <unidad> [símbolos]` (compila y saca el % de esos símbolos; zBWare
3 s, zEagl4Anim 30 s, zEcstasy 17 s) y `pl19_sw.sh` (lo anterior más las 25 filas
del árbol de switch de `Initialize`). Nombres únicos, como pide el brief.
