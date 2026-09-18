# r21 — zAI + zWorld + zWorld2

Encargo: 6 near-miss, 16.324 B. **Cierres: 0.** Lo que traigo es un mecanismo
nuevo con nombre y sitio (`expand_end_loop`, en el expansor, no en un pase de
optimización), una función **cruzada de otra unidad** con la firma idéntica que
el mismo arreglo cerraría, y cuatro vedas medidas.

## 0. Estado verificado al empezar (no del papel)

`build_direct.py zAI zWorld zWorld2` + `triage.py`: **el encargo coincide exacto**.
Ninguna de las seis estaba ya cerrada.

    3 unidades  556816/573140 B  97,1518%  1958 funciones al 100%

| B | % | unidad | función | firma triage |
|---|---|---|---|---|
| 2908 | 99,601 | zAI | `UpdateAllAvoidables` | faltan 4, sobran 4, 4 SUST · blt-4 b+4 |
| 1684 | 99,667 | zAI | `AssignClosestOffsets` | faltan 2, sobran 1 · cmpwi+1 mr.-1 mr+1 |
| 2908 | 98,618 | zWorld | `RenderFlaresOnCar` | sobran 1, 1 SUST · lis-1 |
|  836 | 99,426 | zWorld | `CullParts` | faltan 1 · mr+1 |
|  876 | 99,384 | zWorld | `UpdateWheelYRenderOffset` | faltan 1 · lis+1 — **VEDADA en r20** |
| 1284 | 99,657 | zWorld2 | `CookieTrailCurvature` | faltan 1 · fmr+1 |

`measure.py --cmp antes_r21_ai.json despues_r21_ai.json`: **+0 B, +0 funciones,
0 unidades cambian**. El árbol queda como estaba: todos los ensayos se
revirtieron.

## 1. `audit.py` — lista de FALLA: **VACÍA**

Dos pasadas completas sobre las tres unidades (`Speed/Indep/SourceLists/zAI`,
`.../zWorld`, `.../zWorld2`): **1.958 funciones, 0 FALLA**, mismas cifras en
ambas pasadas. No hay fantasmas que confirmar.

## 2. `UpdateAllAvoidables` — el `blt` NACE EN EL EXPANSOR, y hay 1.488 B más en juego

### 2.1 La firma estaba al revés en el encargo

El encargo decía «el objetivo emite cuatro `blt` donde nosotros emitimos cuatro
`b`». Es al contrario: **el objetivo emite `b` incondicional y nosotros `blt`**.

Las cuatro son **la misma decisión**: el ctor `SAP::Grid<T>::Axis::Node::Node`
(SAP.h:18-37) se inlinea **cuatro veces** en `new Grid(...)` (mMin/mMax de los
ejes X y Z) y su bucle

```cpp
while (head != nullptr && head->mPosition < this->mPosition) {
    node = head;
    head = head->mTail;
}
```

sale así (índices del `fndiff`, izquierda objetivo / derecha nosotros):

    82..90  IDÉNTICOS: lfs f0,8(r9) · fcmpu cr0,f0,f13 · cror un,eq,gt · bso Lexit
    91      mr r6,r9        |  mr r6,r9
    92      lwz r9,0x4(r9)  |  lwz r9,0x4(r6)      <- cse coge otro miembro de la clase
    93,94   cmpwi r9,0 · beq Lexit
    95..97  lfs f13,8(r9) · lfs f0,8(r3) · fcmpu cr0,f13,f0
    98      b  +0x164       |  blt +0x16c          <- LA ÚNICA diferencia real

O sea: **la etiqueta del bucle**. El objetivo la pone en el `cror` (la rama del
test flotante, que se comparte entre el preencabezado y el retroceso) y cierra
con `b`; nosotros la ponemos en el cuerpo y cerramos con `blt`, duplicando la
rama. Mismo número de instrucciones (2908 B los dos), misma nube de registros.

### 2.2 Lo mismo pasa en **zPhysicsBehaviors** — 1.488 B más

`triage.py zPhysicsBehaviors`:

    1488 B  99,301%  Add__6RBGridUiR9RigidBodyRCQ25UMath7Vector   faltan 4, sobran 4, 4 SUST  blt-4, b+4

**Firma idéntica.** `RBGrid::Add` (RigidBody.cpp:167) construye un
`SAP::Grid<RigidBody>` y le inlinea los mismos cuatro ctores de `Node`. Un solo
arreglo en SAP.h cerraría **4.396 B en dos unidades**. SAP.h llega a zAI,
zPhysics y zPhysicsBehaviors, así que el A/B tendría que ser sobre las tres.

### 2.3 El mecanismo, con el volcado RTL delante

`aha_rtl.py AvoidableManager.cpp zAI rjsGLtJ`. En el volcado **`.rtl` — o sea
RECIÉN EXPANDIDO, antes de `jump`, `cse`, `gcse`, `loop` y `combine`** — el
bucle ya está rotado:

    (note 326) NOTE_INSN_LOOP_BEG
    (jump 327) goto L337                     <- salto al test
    (label 329) (label 330)                  <- newstart_label / cuerpo
      (insn 332) r168 = r170                 ; node = head        SAP.h:27
      (insn 334,335) r170 = *(r170+4)        ; head = head->mTail SAP.h:28
    (note 336) NOTE_INSN_LOOP_CONT
    (label 337)
      (insn 338,jump 339) if (r170==0) goto L348
      (insn 340..344)     cc177 = cmpfp(*(r170+8), *(r148+8))
      (jump 345) if (lt) goto L330           <- **EL `blt`, YA AQUÍ**
      (jump 346) goto L348

Es `expand_end_loop` (stmt.c): mueve `[start_label .. último salto condicional]`
al final del bucle y convierte el retroceso en condicional. Luego `loop.c`
llama a `duplicate_loop_exit_test` (jump.c) — el `NOTE_INSN_LOOP_BEG` va seguido
de un salto incondicional, que es justo su precondición — y **copia el test al
preencabezado**: de ahí el `cror;bso` de las filas 89-90.

**Consecuencia práctica**: ningún `-f...` puede deshacerlo, porque no lo hace
ningún pase. Medido, 19 banderas, ninguna mueve la cuenta de `b`/`blt` (4):

| bandera | % | diffs | b/blt |
|---|---|---|---|
| base | 99,6011 | 14 | 4 |
| `-fno-thread-jumps` · `-fno-rerun-loop-opt` · `-fno-expensive-optimizations` · `-fno-force-mem` · `-fno-strength-reduce` · `-fno-peephole` · `-fno-delayed-branch` · `-fno-defer-pop` · `-funroll-loops` | 99,6011 | 14 | 4 |
| `-fno-schedule-insns2` | 98,528 | 32 | 4 |
| `-fno-move-all-movables` | 95,648 | 73 | 4 |
| `-fno-omit-frame-pointer` | 98,345 | 128 | 4 |
| `-fno-cse-skip-blocks` | 93,088 | 153 | 5 |
| `-fno-force-addr` | 96,692 | 182 | 4 |
| `-fno-rerun-cse-after-loop` | 92,286 | 231 | 4 |
| `-fno-cse-follow-jumps` | 91,486 | 258 | 4 |
| `-fno-gcse` | 89,724 | 276 | 4 |
| `-O2` | 90,168 | 140 | 5 |

### 2.4 Ensayos de fuente (ciclo 10 s, sin tocar el árbol)

`c21ai_var.py` mete la variante en una COPIA de SAP.h en el scratchpad y usa el
guard `SAP_H` para que la real quede vacía; compila `AvoidableManager.cpp`
suelto con los cflags de zAI (reproduce el objeto de la SourceList **exacto**:
99,6011 %, 2908 B, 14 diffs).

| # | forma barrida | resultado |
|---|---|---|
| b0 | base (`while` con `&&`) | 99,6011 % · 14 diffs · 4 SUST |
| b1 | `for (head=mRoot; head && head->mPosition<mPosition; head=head->mTail) node=head;` | **IDÉNTICO** |
| b2 | `while (head->mPosition<mPos) { node=head; head=head->mTail; if(!head) break; }` | 94,417 % · 109 diffs · 2860 B |
| b3 | `while (head) { if (!(head->mPosition<mPos)) break; ... }` | **IDÉNTICO** |
| b4 | igual con `>=` en vez de `!(<)` | **IDÉNTICO** |
| b5 | `for (head=mRoot; head; head=head->mTail) { if (!(...)) break; node=head; }` | **IDÉNTICO** |
| b6 | `while (true) { if(!head) break; if(!(...)) break; ... }` | **IDÉNTICO** |
| b7 | bucle de `goto` explícito (test arriba, `goto loop` abajo) | 92,193 % · 171 diffs · 2880 B |

**b1, b3, b4, b5 y b6 dan bytes idénticos**: el front-end canonicaliza las cinco
al mismo árbol y `expand_end_loop` rota igual. **Veda: la forma del bucle no es
la palanca.**

**b7 es el dato que vale**: el `goto` explícito **sí** produce el retroceso
incondicional `b` (probado también en un caso mínimo de 2 s, `c21ai_probe3.cpp`,
función `h2`), porque sin construcción de bucle no hay `expand_end_loop`. Pero
pierde el pelado del preencabezado (que lo hace `duplicate_loop_exit_test`, y
ése **necesita** la forma rotada) y revienta el reparto de registros: 171 diffs.

### 2.5 Lo que hace falta y no he encontrado

Una forma que dé **las dos cosas a la vez**: bucle sin rotar (retroceso `b`) y
con el pelado del test en el preencabezado. En el objetivo el preencabezado
tiene sólo el **cómputo** (`lfs f0,8(r9); fcmpu`) y la **rama** es única, en la
cima del bucle; en nosotros están duplicadas las dos. La hipótesis que dejo
abierta es que en el objetivo `jump.c` hace **cross-jump** de la cola
`cror;bso Lexit` entre la copia del preencabezado y la del bucle (2 insns
comunes → una se borra y queda un `b`), y que en nosotros no llega a pasar
porque `jump.c` invierte antes `if lt goto Lbody; goto Lexit` en un `blt`.
Comprobarlo pide el volcado `.jump`/`.jump2` de las dos formas insn a insn:
está en `scratchpad/rtl/AvoidableManager/`.

## 3. `AssignClosestOffsets` — `mr.` contra `mr`+`cmpwi`

Diff real: **8 filas, y sólo dos son de código** (las otras 6 son la rotación
r28↔r29 que arrastran).

    388  objetivo: mr r9, r29      nosotros: mr. r9, r28
    389  objetivo: cmpwi r29, 0x0  nosotros: (nada)

Es el cierre del `do/while` de AIPursuit.cpp:1018,
`} while (--copsToAssignOffsets > 0);`. `combine` funde la copia con la puesta
de condición en el patrón `*movsi_internal2` (`or. rA,rS,rS`). El objetivo no
funde: compara **r29** (el origen) en vez del destino.

**`lmap.py` da el dato de fuente**: en el objetivo la copia es
**AIPursuit.cpp:1532** y la comparación **AIPursuit.cpp:1534** — *dos líneas de
fuente distintas*, con la 1533 sin código. Y los tres `continue` del cuerpo
(`b/blt/bge .L_80031D70`) saltan **a la copia**, o sea que el decremento está en
el punto de continuación del bucle.

| # | ensayo | resultado |
|---|---|---|
| c1 | decremento como sentencia suelta al final del cuerpo + etiqueta `next_cop:` y los tres `continue` a `goto next_cop` (semántica preservada; hubo que partir `float closestDistance = 100000.0f;` en declaración + asignación porque el `goto` cruzaba la inicialización) | **IDÉNTICO**: 99,66746 %, 1680 B, los mismos 8 diffs |
| c2 | igual que c1 pero con dos variables: `copsRemaining = copsToAssignOffsets-1; copsToAssignOffsets = copsRemaining;` y `while (copsRemaining > 0)` | **IDÉNTICO** |

**Veda**: partir el decremento de la prueba —ni como sentencia suelta ni con
variable auxiliar— no impide la fusión; `cse` colapsa las dos variables y
`combine` funde igual. Revertido (AIPursuit.cpp vuelve a su estado original).

Lo que NO he probado aquí: forzar que el valor comparado no sea el destino de la
copia con algo que `cse` no pueda colapsar (una lectura `volatile`, o que el
contador sobreviva al bucle y se use después).

## 4. `CullParts` — el `mr` que falta, y la barrera SÍ hace falta

Diff: 5 filas, la única de código es `mr r9, r0` (fila 102) que el objetivo
tiene y nosotros no (836 contra 832 B). El objetivo compara la guarda del bucle
contra `r9` (el `li r9,0` de `debug_print`) y nosotros reusamos el registro de
`Polarity`, que en esa rama vale 0 provablemente — o sea **nuestro código es más
listo y por eso le sobra el `mr`**.

`CarRender.cpp:307` lleva una barrera nuestra `asm("" : : : "r11")`. Aplicada la
regla de la ronda 20 (quitar el pin lo primero):

| # | ensayo | resultado |
|---|---|---|
| d1 | quitar `asm("" : : : "r11")` | **99,330 % · 9 diffs** (peor: base 99,4258 % · 5 diffs) |

**La barrera hace falta.** Restaurada. El fichero ya trae anotadas cuatro vedas
de rondas anteriores sobre `debug_print` (inicializarlo a `NumPlanes`: 824 B ·
asignarlo también antes del bucle: 828 B · moverlo al principio del cuerpo:
836 B pero 59 diffs · `for (n = 0; ...)`), así que ahí no queda forma obvia: lo
que decide es en qué registro cae `Polarity` (objetivo r0, nosotros r11).

## 5. `RenderFlaresOnCar` — auditoría de líneas, resultado parcial

Diff: 18 filas, una sola de código (`lis-1`). El objetivo crea **un** pseudo
`@ha` de `lbl_8040AD04` (el `0.0f`) en la fila 123, **CarRender.cpp:4061** —el
bloque de `GetCarTypeInfo`/`is_traffic_car`— y lo mantiene vivo 330
instrucciones para las tres cargas de 4225, 4310 y 4314. Nosotros
materializamos dos (uno local en la fila 452 y otro izado al preencabezado del
bucle de flares).

Lo que sí he hecho de la recomendación de r20 (auditar con `lmap.py`): el mapa
de líneas del objetivo cubre **167 líneas distintas entre 3976 y 4386**. Los
huecos > 3 líneas sin código son 4045→4057, 4057→4061, 4061→4065, 4066→4071,
4072→4077, 4105→4109, 4132→4136, 4139→4143, 4169→4176, **4185→4189**,
**4196→4213**, 4225→4229 y 4236→4240.

El desplazamiento fuente objetivo→nuestro no es constante: calibrando por
`is_traffic_car` sale **1486** (4061↔2575) y calibrando por la primera carga del
literal sale **1494** (4225↔2731). O sea que **el original tiene ~8 líneas más
que nosotros entre `is_traffic_car` y el bucle de flares**, y el hueco grande
(4196→4213, 17 líneas sin código) cae justo en esa zona (nuestro 2702-2727:
`IsLightBroken(COPWHITE)` … `preview_part` / `preview_part_id` / `FlareCount` /
`constFlicker` / cabecera del `for`). **Ahí es donde hay que seguir**: reconciliar
esas ~17 líneas es lo que puede mover el punto de inserción de PRE, que es lo
único que decide este `lis`. No me ha dado tiempo a cerrarlo.

## 6. `UpdateWheelYRenderOffset` y `CookieTrailCurvature`

No tocadas, por instrucción del encargo: la primera está vedada por
construcción desde r20 (PRE domina los tres usos del `0.0f`, orden de pases
fijo) y la segunda es muro reconfirmado en r18 y r20.

## 7. Qué NO he probado

- **SAP.h con el volcado `.jump`/`.jump2` insn a insn** para confirmar o
  descartar la hipótesis del cross-jump de la cola `cror;bso` (§2.5). Es lo que
  yo haría primero: los volcados ya están en
  `scratchpad/rtl/AvoidableManager/`.
- **`Add__6RBGrid`** (zPhysicsBehaviors, 1.488 B) directamente: sólo he medido
  su firma con `triage.py`. Es de otra unidad y no he compilado nada allí.
- **`permuter.py`** sobre ninguna de las seis.
- **`AssignClosestOffsets` con `dwbody.py`/`regmap.py`** filtrando por `low_pc`:
  he ido por `lmap.py` y por formas de fuente, no por las locales del DWARF.
- **`CullParts` con el DWARF**: las cuatro vedas anotadas en el fichero son de
  rondas anteriores; yo sólo he añadido d1 (quitar la barrera).
- **`RenderFlaresOnCar`**: la reconciliación línea a línea del hueco 4196→4213,
  que es lo que he identificado como el sitio (§5).
- **Ninguna variante con restricciones de registro** en ninguna de las seis.

## 8. Herramientas dejadas en el scratchpad (no en el árbol)

`c21ai_var.py` (variante de SAP.h sin tocar el árbol, 10 s/ciclo),
`c21ai_flag.py` (barrido de banderas sobre AvoidableManager.cpp),
`c21ai_s.py` (compila un `.cpp` suelto a `.s` con los cflags de zAI, 2 s),
`c21ai_probe*.cpp` (casos mínimos de forma de bucle),
`rtl/AvoidableManager/` (volcados `.rtl .jump .jump2 .cse .cse2 .gcse .loop`).
