# Ronda 23 — zMain (+ zGameplay y zBWare de propina)

## 0. Verificación del encargo

Tras `build_direct.py zMain` + `triage.py zMain`:

    zMain  155436/159776 B  97.2837%  1378 funciones al 100%
    MURO: 2 funciones, 4340 B
        2176 B  99.485%  __12EPlayRaceNISP7GMarkerPCcT2iiT2T2   29 registros
        2164 B  98.869%  _._14ESpawnFragment                    reorden local, dmax 3

**Coincide con el encargo.** `audit.py Speed/Indep/SourceLists/zMain` al empezar:
**1378 ok, 0 FALLA**. Repetido al terminar: **1378 ok, 0 FALLA**. `zGameplay`
765 ok / 0 FALLA y `zBWare` 238 ok / 0 FALLA, también al terminar.

## 1. Resultado

    measure.py --cmp antes.json final.json  ->  +0 B, +0 funciones, 0 unidades cambian

| función | antes | después |
|---|---|---|
| `__12EPlayRaceNISP7GMarkerPCcT2iiT2T2` (2.176 B) | 99,48529 % · **31 filas** | **99,97243 % · 3 filas** |
| `_._14ESpawnFragment` (2.164 B) | 98,86876 % · 23 filas | sin cambio (revertido, §3) |
| `__8GTriggerRCUi` (2.544 B) | 99,583336 % · 4 filas | sin cambio |
| `_bOutput` (5.180 B) | 99,84016 % · 37 filas | sin cambio |

**Cero bytes** (`matched_code` es todo-o-nada). Fichero tocado: **sólo**
`src/Speed/Indep/Src/Generated/Events/EPlayRaceNIS.cpp`, y sólo el bucle de
`car%d`. Ninguna cabecera se queda tocada. Sin commit.

`pctsnap.py --cmp` sobre zMain, con el «antes» **reconstruido** desde HEAD:

    EMPEORAN: ninguna
    MEJORAN:  1 funciones
        +0.487 pp  __12EPlayRaceNISP7GMarkerPCcT2iiT2T2  99,4853 -> 99,9724  2176 B

---

## 2. `__12EPlayRaceNIS…` — 31 filas → 3. El `for` con `i++`

### El cambio

```cpp
for (int i = 0; i < numRacers; i++) {
    bSPrintf(channelName, "car%d", i + 1);
    nis->AddCar(UCrc32(channelName), racerVehicle);
    racerVehicle = racerVehicle->Next(VEHICLE_RACERS);
}
```

en vez de `for (int i = 0; i < numRacers;)` con `++i` dentro del `bSPrintf`.
**Cierra las 28 filas de permutación de GPR de golpe.** Quedan las 3 del racimo
de FPR.

### Por qué: el modelo de `global_alloc`, verificado al dígito

Los cinco registros del bucle los reparte `global_alloc`, y su orden es la lista
`;; N regs to allocate` del volcado `.greg`, que ya sale ordenada por
`allocno_compare`:

    prioridad = int( floor_log2(n_refs) * n_refs * size / live_length * 10000 )

empates a **allocno más bajo primero**. Dentro del conjunto que entra en
conflicto, el reparto sigue `reg_alloc_order` de rs6000 (`9,10,8,7,6,5,4,3,
31,30,…,11`), o sea r30, r29, r28… en orden de prioridad **descendente**.

Con `++i` dentro del `bSPrintf` (base), el `.lreg` da:

| pseudo | qué | refs/len | prioridad | nuestro | objetivo |
|---|---|---|---|---|---|
| 510 | `racerVehicle` | 11/34 | **9705** | r30 | r29 |
| 534 | `&…ListableSet::_mLists` | 9/84 | **3214** | r29 | r28 |
| 511 | `i` | 10/100 | **3000** | r28 | r30 |
| 521 | copia de `nis` en el bucle | 5/41 | 2439 | r27 | r27 |
| 507 | `numRacers` | 5/54 | **1851** | r26 | r25 |
| 500 | `nis` | 10/165 | **1818** | r25 | r26 |
| 737 / 736 / 514 | `channelName`, `r1+0x18`, `$LC "car%d"` | | 1125/978/348 | r24/r23/r22 | igual |

La lista impresa por el compilador es **exactamente** ese orden, así que el
modelo está verificado. El objetivo necesita `i > racerVehicle > _mLists` y
`nis > numRacers`; con `i++` en la cláusula del `for` eso sale solo.

### Ensayos numerados (arnés `c23main_sw.py`, 14 s por variante)

    base                             99,48529   31 filas
    n0  quitar el __asm__("")        98,35478   39   2.168 B   PEOR -> el asm SE QUEDA
    n1  Next() sin GetList()         99,48529   31   idéntico
    n2  un `{ }` de más (DWARF)      99,48529   31   idéntico
    n3  n1+n2                        99,48529   31   idéntico
    q1  numRacers con `if (>8)`      98,47059   37   2.160 B
    q2  numRacers con UMath::Min     98,47059   37   2.160 B
    q3  UCrc32 chan en un temporal   99,42647   22   2.180 B
    r1  `for (…; i++)` + `i + 1`     **99,97243    3   APLICADO**
    r2  Next() al principio del cuerpo 91,39889  95   2.180 B

**El `__asm__("")` de la línea 79 es nuestro pero HACE FALTA**: quitarlo son
8 filas más. Queda medido y anotado; no volver a tocarlo.

### Las 3 filas que quedan: el racimo de FPR, y por qué es muro

Rows 301-303. Las cuatro constantes de `(dot * 0.75f + minTresh) * distTresh`
son invariantes que **`loop.c` iza al preencabezado del bucle de polis**, y el
registro de cada una lo decide su `live_length`: la izada **más pronto** vive
más y se lleva el registro **más bajo**.

| constante | `.lreg` refs/len | prioridad | nuestro | objetivo |
|---|---|---|---|---|
| `0.25` | 3/280 | 107 | f29 | f26 |
| `0.75` | 3/284 | 105 | f28 | f28 |
| `0.5` | 3/288 | 104 | f27 | f29 |
| `1.0` | 3/294 | 102 | f26 | f27 |

O sea: **nuestro orden de izada es 1,0 · 0,5 · 0,75 · 0,25 y el del objetivo
tiene que ser 0,25 · 1,0 · 0,75 · 0,5.**

Comprobado en los volcados por pase que **el orden de izada es el orden de las
cargas supervivientes DENTRO del bucle**:

    .cse   ->  1.0  0.5  0.25(decl)  0.75  0.25(uso)
    .loop  ->  1.0  0.5  0.75  0.25          (la copia de la decl muere)

**Y ahí está el muro, con una medida dura**: el orden del *pool* del objetivo es

    lbl_803F0600 2.0 · 0604 80.0 · 0608 1.0 · 060C 0.5 · 0610 0.25 · 0614 0.75

(leído del `.s`, no inferido). El pool se crea en orden de primera
materialización, así que **el original declara `minTresh` DESPUÉS de la
sentencia de `dot` y antes de la de `distTresh`, exactamente como nosotros.**
Con esa restricción la carga de `0.25` no puede nacer antes que la de `1.0`, y
el reparto de FPR sale forzado. Para que 0,25 se ice primero haría falta que su
`store` de la declaración sobreviviera, y eso sólo pasa si `minTresh` no es
`const` — que contradice el DWARF (`const float minTresh;`) y aun así deja 2
filas, no 0.

    s1  `float minTresh = 0.25f;` (NO const) declarado antes de `dot`
                                     99,98162   2 filas   <- la mejor, pero contradice el DWARF
    s2  igual pero `const`           99,97243   3   idéntico a s0
    s3  0.25 en línea (sin minTresh) 99,97243   3
    s4  temporal `scaled = dot*0.75` 99,97243   3
    s5  `0.5f * (Dot + 1.0f)`        99,97243   3
    s6  `dot` partido en dos         99,97243   3
    s7  `minTresh + dot * 0.75f`     99,97243   3
    s8  `const float distFrac=0.75f` 99,97243   3
    v1  minTresh entre las dos mitades de `dot`  99,97243  3
    v2  `const float dotScale=0.75f`             99,97243  3
    v4  `dot * (1.0f - minTresh) + minTresh`     99,97243  3
    v5  `/ 2.0f` en vez de `* 0.5f`              99,97243  3
    v6  `((dot+1)*0.5f*0.75f + minTresh)`        98,94486  23

**Veda**: barridas **catorce** formas de `(dot * 0.75f + minTresh) * distTresh`
y de la sentencia de `dot`. Ninguna mueve el orden de izada, y el orden del pool
del objetivo demuestra que la fuente es la que tenemos.

### Lo que el DWARF decía y NO era accionable

- **El nivel de bloque de más alrededor de `parms`**: aplicado como `{ }`
  explícito (n2) → **objeto idéntico**. Las notas `NOTE_INSN_BLOCK_BEG/END` no
  cuentan como insn ni en `local_alloc` (`block_alloc` salta los NOTE) ni en
  `sched`. Es una diferencia de DWARF sin efecto en el código.
- **La expansión de `ListableSet::GetList` que nos sobra dentro de `Next`**: es
  REAL (el original expande `begin/end/end/end` y nosotros `GetList/begin/end/
  end/end`), y se corrige escribiendo `const List &list = _mLists._buckets[idx];`
  en `UListable.h`. **Medido: objeto idéntico** (n1). No la he dejado puesta
  porque `UListable.h` es cabecera compartida y no paga nada; queda anotada aquí
  para quien haga una limpieza de DWARF.

---

## 3. `_._14ESpawnFragment` — el 100 % encontrado, y **medido que cuesta 14.972 B**

### El mecanismo, cerrado

Las tres constantes (`1/32767` de `_Q4c::Decompress`, y `1.0f`/`0.0f` de
`VU0_quattom4`) las reparte **`local-alloc`** con la misma fórmula, y los FPR
volátiles se dan en orden `f0, f13…f1`, así que las tres de vida más larga se
llevan f3, f2 y f1 por ese orden.

    nuestro:  1.0 -> f3   0.0 -> f2   1/32767 -> f1
    objetivo: 0.0 -> f3   1/32767 -> f2   1.0 -> f1

La causa raíz está localizada al insn: **`sched1` coloca el
`stfs 1.0f -> result[3][3]` en la 3.ª posición del bloque** (volcado `.sched`:
`0x88, 0x98, 0x9c, 0x60, …`), mientras que antes de `sched1` (`.regmove`) los 16
`stfs` están en orden de fuente. Con el store tan pronto, `1.0` muere 15 ranuras
antes, su `live_length` baja de 128 a 114 y adelanta a `1/32767`.

### La forma que da 100 % — y por qué NO se queda

Barridas **las 25 posiciones** de `result[3][3] = 1.0f;` dentro de `quattom4`
(`c23main_q4sweep.py`, 4 s por posición):

    posiciones 0-12  (antes de `result[0][3]`)      99,890945   13 filas
    posiciones 13-16                                99,97597     3
    posiciones 17-19                                99,97782     2
    posiciones 20-24 (tras `result[2][2]`)          98,86876    23  <- el acantilado

y luego **las 25 posiciones de `result[2][3] = 0.0f;`** sobre la mejor base
(`c23main_q4sweep2.py`):

    posición 18 (justo detrás de `result[1][3]`)    **100,0 %  0 filas**

O sea, `VU0_quattom4` con este orden cierra la función:

```
    result[1][3] = 0.0f;
    result[2][3] = 0.0f;      // movidas aquí
    result[3][3] = 1.0f;      //
    result[2][0] = (xz + yw);
    result[2][1] = (yz - xw);
    result[2][2] = 1.0f - (xx + yy);
    result[3][0] = 0.0f; …
```

**Pero `VU0_quattom4` es inline en `UVectorMathGC.hpp` y tiene 9 llamantes.**
A/B por objetos sobre las cinco unidades afectadas (zMain, zPhysics,
zPhysicsBehaviors, zWorld, zWorld2):

    measure.py --cmp   ->  -12.808 B netos,  3 unidades BAJAN
        +2.164 B  +1 fn   zMain            (ESpawnFragment CIERRA)
        -7.996 B  -6 fns  zPhysicsBehaviors
        -5.784 B  -3 fns  zPhysics
        -1.192 B  -1 fn   zWorld

`pctsnap.py --cmp` (con el «antes» tomado **después** de reconstruir con la
cabecera natural — la primera pasada usó objetos rancios y dijo «ninguna
empeora», que era mentira) da las ocho que se rompen:

    -31,193 pp  RecalcOrientMat__C15SimpleRigidBody…     100 -> 68,807   228 B
    -13,236 pp  GetSceneryTransform__C12SceneryModel…    100 -> 86,764   492 B
     -4,686 pp  SetCollision__CQ29RigidBody9Primitive…   100 -> 95,314   560 B
     -0,923 pp  DoRBCollisions__15SimpleRigidBodyf       100 -> 99,077  1508 B
     -0,849 pp  UpdateModel__19VehicleFragmentConn       100 -> 99,151  1192 B
     -0,764 pp  DoSRBCollisions__15SimpleRigidBody…      100 -> 99,236  1508 B
     -0,492 pp  AddNode__CQ217CollisionGeometry10Coll…   100 -> 99,508  1584 B
     -0,479 pp  DoIntegration__9RigidBodyf               100 -> 99,521  1988 B

**Conclusión medida: el `quattom4` del original ES el natural** (esas ocho lo
demuestran), así que la diferencia de `ESpawnFragment` está **aguas arriba, en
su propio bloque**, no en `quattom4`. **REVERTIDO.**

### Aguas arriba: barrido y descartado

    d0-d5  `_Q4c::Decompress`: orden w/x/y/z, recíproco en local, temporal
           Vector4, sin `this->`, w primero          23 / 98 / 23 / 239 / 23 / 97 filas
    g1     `Bounds::GetTransform` con `Vector3 &pos` temporal      23 filas
    g2     `VU0_quattom4` directo en vez de `QuaternionToMatrix4`  23 filas
    g3     `bounds->GetTransform` antes de `model->GetTransform`  141 filas
    g4     declarar `m_inner` antes que `m_outer`                  39 filas
    w1-w4, x1-x3, y1-y3, N1-N8  (formas de quattom4)   ver arriba

`dwbody.py` sobre `ESpawnFragment::~ESpawnFragment`: el cuerpo casa **entero**
salvo dos NOMBRES (`ib_fragment`/`ib_owner` contra `iboundable_frag`/
`iboundable_parent`), y los 25+25 `Matrix4::operator[]`/`Vector4::operator[]`
coinciden uno a uno.

**Lo que queda por probar aquí**: la única asimetría de este llamante frente a
los otros ocho es que `Bounds::GetTransform` **escribe después en
`matrix.v3`** (`GetPosition(Vector4To3(matrix.v3))`), lo que crea dependencia de
salida con los `stfs 0.0 -> [3][0..2]` y cambia las prioridades de `sched1`. No
he probado nada sobre `_V3c::Decompress` ni sobre `UMath::Vector4To3`.

---

## 4. `__8GTriggerRCUi` (zGameplay, 2.544 B) — 7 formas más, todas peores

Sigue siendo la permutación de `{fneg, mr r3, mr r4, fmuls, li r22, bl}`:

    objetivo: fneg · mr r3 · fmuls · mr r4 · bl · li r22
    nuestro : mr r3 · fneg · mr r4 · fmuls · li r22 · bl

    t1  `fType = 1` antes de MultYRot          97,63994   23 filas
    t2  `fType = 1` tras `fFingerprint = 0`    98,89937   15
    t3  ángulo en un temporal                  99,583336   4   idéntico
    t4  `fHeight` antes del bloque             97,56761   33
    t5  `Init`/`MultYRot` tras las asignaciones 93,3522   74
    t6  `fFingerprint` antes de `fType`        98,80346   16
    t7  `(0.0f - Rotation()) / 360.0f`         99,583336   4   idéntico

Con las 9 de la r21 y las 6 de la r22 van **22 formas**. `r22` es el 1 CSE-ado
entre `mWorldTrigger.fType` (fila 314, `rlwimi`) y `mEventList.fNumEvents`
(fila 384, `stw`). **Veda: el orden de las sentencias alrededor de
`UMath::MultYRot` no lo mueve.**

Nota: la corrección de `UMath::MultYRot` que la r22 dejó como hallazgo **ya está
aplicada en el árbol** (commit `f22abc39`); `UMath.h` ya no lleva el `r = m;`.

## 5. `_bOutput` (zBWare, 5.180 B) — el censo de los cuatro racimos

37 filas, ninguna tocada esta ronda. Con `lmap.py` quedan situadas:

| filas | líneas del original | qué |
|---|---|---|
| 550-560 | 749-756 (`bPrintf.cpp:412-418`, el `if (flags & FL_FORCEOCTAL)`) | `size` va a r9 en el objetivo y a r10 en el nuestro; el `'0'` va a **r11** (el ÚLTIMO de `reg_alloc_order`) en el objetivo y a **r9** (el primero) en el nuestro |
| 592-614 | cascada de `_nan_table` | rotación r8/r9/r11 |
| 619-678 | pool de `%f` (`lbl_803D1028/1038/1040`) | rotación r0/r8/r9/r11 en los `lis @ha` |
| 913/915 | — | `addi r23,r1,0x10` y `addi r5,r22,0x1` permutados |

**Diagnóstico nuevo del racimo A**: el `'0'` del objetivo en **r11** no huele a
pseudo repartido por `local-alloc` (un pseudo de 2 refs y vida 2 tiene prioridad
~10000 y se llevaría r9), sino a **registro de recarga de `reload`**: el
objetivo llega a `reload` con `(set (mem:QI …) (const_int 48))` sin pseudo, y
nosotros con la constante ya metida en un pseudo. Eso apunta a `cse`/`combine`,
no a la forma de la sentencia. **Sin ensayos.**

## 6. Herramientas

- **`c23main_sw.py`** (scratchpad): arnés multi-parche que compila el `.cpp`
  suelto con los cflags de la unidad, diffea y **restaura siempre**. 14 s por
  variante para `EPlayRaceNIS`, 4 s para `ESpawnFragment`/`GTrigger`. Verificado
  que los tres stubs reproducen el `.o` real fila a fila antes de fiarme.
- **`c23main_pri.py`**: la tabla de allocnos del propio compilador (refs, vida,
  prioridad de `allocno_compare`) **con las constantes flotantes identificadas
  por valor** (`REG_EQUIV (const_double …)`), 6 s. Pide sólo `-dl -dg`, no `-da`:
  2 volcados en vez de 16 y sin el `.rtl` de 45 MB.
- **`c23main_q4sweep.py` / `q4sweep2.py`**: barren la POSICIÓN de una sentencia
  dentro de una función y miden cada una. 25 posiciones en 4 min 47 s. Es lo que
  encontró el 100 % de `ESpawnFragment`.
- **Aviso**: `cc1plus` a mano con `-da` **peta con un ICE** al final
  (`_tree.c:639`, la instanciación de `_Rb_tree::_M_erase`), pero **eso pasa
  después de nuestras funciones y los 16 volcados ya están escritos**. No es un
  problema; sólo hay que ignorar el error y borrar el `.rtl` (45 MB).
- **`pctsnap.py --cmp` miente si el «antes» se toma sin reconstruir.** Me dio
  «EMPEORAN: ninguna» sobre un cambio que `measure.py` medía en −14.972 B,
  porque el snapshot «antes» leía los objetos ya modificados. Igual que la regla
  de `measure.py`: **`build_direct.py` antes de CADA snapshot, no sólo del
  último.**

## 7. Aviso de convivencia

`frozen.py chk Speed/Indep/SourceLists/zGameplay` dice **`*** HA CAMBIADO ***`**
y **no es mío**: el congelado es de **hoy a las 17:18**, antes de que empezara
esta sesión, y con `zGameplay` reconstruido y mis ficheros intactos sigue
diciéndolo; además `measure.py --cmp` contra mi propia línea base da
**+0 B y 0 unidades cambian** para las tres unidades. `zBWare` sí es idéntico a
su congelado.

## 8. Qué NO he probado

- `EPlayRaceNIS`: nada que cambie el orden del **pool** (`2.0, 80.0, 1.0, 0.5,
  0.25, 0.75`), que es la restricción que cierra el racimo de FPR; nada sobre
  `UMath::Dot` / `UMath::Direction` / `body->GetForwardVector`; nada sobre el
  bucle de polis (`GetList(...).begin()/end()`).
- `ESpawnFragment`: nada sobre `_V3c::Decompress` ni sobre `UMath::Vector4To3`,
  que es **la única asimetría** de este llamante de `quattom4` frente a los otros
  ocho; nada sobre `VU0_MATRIX4_mult`; ninguna búsqueda conjunta de un orden de
  `quattom4` que satisfaga a la vez a las ocho funciones de zPhysics/
  zPhysicsBehaviors/zWorld y a ésta.
- `GTrigger`: nada sobre `StaticData * pTriggerData` (el DWARF dice que el
  original lo tiene y nosotros no); nada sobre el CSE del `1` entre `fType` y
  `fNumEvents`.
- `_bOutput`: **cero ensayos**. Ni los racimos 592-614, 619-678 y 913/915, ni las
  cuatro etiquetas de más (`OUTPUT`/`positioned`/`Z_INT`/`VECT_OUTPUT`), ni el
  `int * __ptr`, ni el `int shift` que el original tiene en r27, ni la hipótesis
  nueva del `reload` del racimo A.
- `GenerateIndex`: no la he tocado (cerrada como muro en la r22).
