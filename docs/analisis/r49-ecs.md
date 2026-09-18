# R49 — `zEcstasy` + `zEagl4Anim`

## Resultado

**Ninguna función cerrada al 100 %, cero bytes.** Lo que sí sale de la ronda:

1. **`EvalState` pasa de 52 filas a 18** (93,50877 % → **98,070175 %**, 456 B
   antes y después, **sin un solo `asm`**). Es el mayor salto que ha tenido esa
   función en cinco rondas, y lo abrió `lmap.py` **sobre el original**.
2. **`UpdatePlatInfo` queda cerrada con la cuenta hecha**: no puede llegar al
   100 % (el bloque de literales son 6-7 filas irreducibles), así que el frente
   de `regs_someone_prefers` vale **cero bytes**. Aun así quedan medidas las dos
   vías del encargo y una **corrección al diagnóstico de la r48**.
3. **`eProject`: demostrado por el mapa de líneas que el `asm` que lleva desde la
   r36d NO está en la fuente original.**
4. **`GenerateHorizonFog`: el mapa de líneas descarta la última hipótesis viva.**

| unidad | entrada | salida |
|---|---|---|
| `zEcstasy` | 4 fn / 5.180 B | 4 fn / 5.180 B (**byte a byte idénticas**) |
| `zEagl4Anim` | 2 fn / 2.808 B (Initialize 2356/2352, **EvalState 52**) | 2 fn / 2.808 B (Initialize 2356/2352, **EvalState 18**) |

Cuatro ficheros tocados. **Tres son sólo comentarios** (`eLightE.cpp`,
`eMathE.cpp`, `EcstasyEx.cpp`: 70 líneas, todas `//`, cero supresiones). El
único cambio de código está en `src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp`.

---

## 1. `EvalState` (456 B) — 52 → 18 filas. El producto estaba en el LLAMANTE

### 1.1 El diagnóstico: `lmap.py` sobre el ORIGINAL

La r48 dejó escrito que el objetivo carga `mKeyIdx` en la fila 19 (el bloque de
unión del `if (mNumFields & 1)` de `GetKeyData`) y nosotros en la 7 (bloque de
entrada), y lo atribuyó a **movimiento interbloque de haifa**. **Eso es
imposible**: `compute_trg_info` sólo trae insns de bloques POSTERIORES al bloque
destino, o sea que haifa mueve hacia ARRIBA. Una insn que nace en el bloque de
entrada no puede acabar en el de unión. Luego en el original **nace ahí**.

`lmap.py` sobre el objetivo lo confirma y además dice de qué sentencia:

```text
800A30D4  lbz r0, 0x9(r30)      RawStateChan.h:162  RawStateChan.h:56
800A30D8  lwz r11, 0x10(r29)    RawStateChan.cpp:126     <- mKeyIdx
800A30DC  mr r3, r0
800A30E0  clrlwi r0, r0, 24     RawStateChan.h:56
800A30E4  mullw r9, r11, r0     RawStateChan.cpp:126     <- indice * GetKeySize()
800A30E8  lfsx f0, r10, r9
```

La carga **y el `mullw`** llevan la línea del **llamante** (`126` del original =
nuestra 117), no la del accesor (`h:162`), y están **detrás** del if/else de
`GetKeyData` (`h:158`/`h:159`/`h:161`). O sea: en el original el producto
`índice * GetKeySize()` **está escrito en el llamante**, no dentro de
`GetKeyData`, y por eso la lectura del miembro cae en el bloque de unión.

### 1.2 La palanca

```diff
-    int keyIdx = mKeyIdx;
-    if (time >= *reinterpret_cast<float *>(c->GetKeyData(keyIdx))) {
-        i = keyIdx;
+    if (time >= *reinterpret_cast<float *>(c->GetKeyData(0) + mKeyIdx * c->GetKeySize())) {
+        i = mKeyIdx;
...
-        for (i = keyIdx - 1; i >= 0; i--) {
+        for (i = mKeyIdx - 1; i >= 0; i--) {
```

`GetKeyData(0)` es **idéntico** (`0 * GetKeySize()` lo pliega `fold`), y la local
`keyIdx` —que `regmap` llevaba dos rondas señalando como «SOLO NUESTRA»—
desaparece. Con eso caen de golpe el `INSERT` de la fila 7, el `DELETE` de la 19
y todo el dominó de registros que colgaba de ellos.

La misma lectura vale para el segundo `GetKeyData` con índice (`c->GetNumKeys()
- 1`, línea 135) y para los dos `GetKeyData(0)` de los caminos agotados, donde el
objetivo mete el resultado **en el registro de `k` (r4)**: el original tenía la
local, no la expresión suelta.

| forma | % | filas | tamaño |
|---|---:|---:|---:|
| base (r48) | 93,50877 | 52 | 456 |
| producto en el llamante (línea 117) | — | **28** | 456 |
| + lo mismo en la línea 135 | 97,76316 | **21** | 456 |
| + `k = c->GetKeyData(0) ...; Decode(k + 4, ...)` en las dos salidas | **98,070175** | **18** | **456** |

`regmap` pasa de «1 local sólo nuestra» a **mismo conjunto de locales y mismo
árbol de bloques**.

### 1.3 Las 18 que quedan son UNA sola cosa, y está acotada

Dos filas son estructurales (39 y 78): el objetivo emite `mr r4,r11` y nosotros
`add r4,r30,r11`. En el objetivo, la rama impar del if/else de `GetKeyData`
**dentro del bucle** es una copia del invariante que `loop` (con
`-fmove-all-movables`) subió al preencabezado; en el nuestro sólo la rama par lo
usa (`addi r4,r8,2`) y la impar vuelve a sumar. Eso deja **vivo un pseudo de
más** (`2*mNumFields+10`, nuestro r11) durante todo el bucle.

Las otras 16 son consecuencia, y el `.greg` lo demuestra: el allocno del
`mr r3,r0` (el `GetKeySize()` crudo, 2 refs y vida larguísima → el **último** de
`allocno_order`) choca con r0, r3..r8, r10, r11, r29, r30, r31 y con r9 (duro).
En la pasada 0 de `find_reg` los callee-saved r13..r28 no están en
`regs_used_so_far` (no son `regs_ever_live`), así que **el único candidato que le
queda es r12**, el último GPR de `REG_ALLOC_ORDER`. Sin el pseudo de más, r11
queda libre y toda la cadena se desplaza al reparto del objetivo
(keySize crudo r3, numKeys r6, numKeys extendido r8, keySize extendido r7/r11).

**Medido y sin efecto (18 filas en todas):**

| barrido | n | resultado |
|---|---:|---|
| formas de `GetKeyData` en la cabecera | 9 | 18 las cuatro que conservan la estructura (`+2` en vez de `+1`, `&keyData[]`, `2 + (uchar*)`, `keyData +=` con `%2`); las que la cambian bajan a 103 insn o suben a 124 y dan **95-116** filas |
| producto manual también en las líneas 123 y 142 | 3 | 18 |
| `for (i = mKeyIdx; i < GetNumKeys(); i++)` en vez del `while` | 3 | 18 |
| `k + GetKeySize()` en vez de `&k[GetKeySize()]` | 1 | 18 |
| orden de declaración de `i` / `k` / `dest` | 4 | 18 |
| hoistar `GetKeyData(0)` a una local antes de cada bucle | 4 | **90 / 101 / 98 / 92** |
| `GetKeySize()` o `GetNumKeys()` en una local delante | 2 | **106 / 97** |
| parámetro `const int keyIdx` en la cabecera (`TREE_READONLY`) | 1 | 52 (sin efecto: `process_reg_param` copia igual el MEM a un pseudo) |

Banco reproducible: `scratchpad/r49_ecs/rsc.py` (TU reducida — prefijo de la
SourceList + un solo `#include` — y comparación canónica del `.s` contra el
ORIGINAL; **5 s por variante** contra 40 del build).

---

## 2. `UpdatePlatInfo` (2.044 B) — el frente vale CERO bytes, y por qué

**Lo primero, porque cambia el reparto de la próxima ronda**: las filas 333-339
del diff (el bloque de literales del `case 0x68E97F75`) son **6-7** y la r48 las
dejó demostradas irreducibles (dos unidades `iu2`, `prio(lis)=3 > prio(lfs)=2`,
las 8 permutaciones del cuerpo dan el objeto **idéntico**). Como `matched_code`
es todo-o-nada, **arreglar el ciclo de tres de `regs_someone_prefers` no aporta
un solo byte**. La medida de la r48 lo confirma por otra vía: con el producto
conmutado la función baja a **8 filas** y ahí se queda.

Dicho eso, el encargo mandaba probar dos vías. Están medidas, y de paso sale una
corrección:

### 2.1 CORRECCIÓN a la r48: el 115 **no** está vivo donde se creía

La r48 descartó la vía «hacer que 194 choque con f5 para que se le pode la
preferencia» con el argumento de que *«todo lo que alarga la escala alcanza
también al 115, que está vivo ahí»*. **No es cierto.** Los conflictos se
construyen sobre el flujo **ya planificado por `sched1`** (que corre ANTES de
`local_alloc`), y en ese flujo:

```text
insn 259  (envmap_min_b = escala * EnvmapMinB)        posicion 78
insn 272/279/286 (envmap_max_r/g/b)                   posiciones 80/81/82
insn  95  (diffuse_rng_a = DiffuseMaxA - min_a)       posicion 83
```

El 115 **nace después** de que muera f5 — por eso f5 no está en sus conflictos
duros (`0 1 8 9 10 11 32 41 42 43 44 45`). O sea que la vía **sí es viable en
teoría**: basta un uso de f5 en las posiciones 79-82. Lo que la mata es el coste,
no la vida del 115.

**Medido** — `asm("" : "+f"(envmap_max_r|_g|_b|_scale) : "f"(envmap_min_scale))`
en las cuatro colocaciones: **455, 457, 459 y 455 filas**, y además **se come dos
instrucciones** (509 contra 511), o sea que ni siquiera es neutro.

### 2.2 Retirar el pin `fr5` (la vía «un pin puede ser andamio»)

**59 filas** (base 27 en esta métrica). Sin el pin, `local_alloc` pone
`envmap_min_scale` en **f8** y `envmap_max_scale` en **f7**, la ponzoña cae en
f8/f7 y **f5 ni siquiera entra en juego**: `regs_used_so_far` arranca con todos
los `call_used` (global.c:374-402), pero la pasada 0 sigue sin poder darle f5 al
115 porque el candidato lo decide `REG_ALLOC_ORDER` y f8 va antes. El pin es
justamente lo que hace que f5 sea el registro envenenado **y** el que el objetivo
quiere.

### 2.3 Vía NUEVA (la otra salida de `prune_preferences`): que el 115 TAMBIÉN prefiera f5

`prune_preferences` no envenena un registro que el propio allocno prefiera
(`AND_COMPL_HARD_REG_SET(temp, hard_reg_full_preferences[A])`, que se aplica
porque `allocno_size` es igual). Y la SEGUNDA rama de `set_preference` da esa
preferencia cuando el **primer operando** del `src` es un registro duro:

```cpp
register float diffuse_max_a asm("fr5") = material_data->DiffuseMaxA;
float diffuse_rng_a = diffuse_max_a - diffuse_min_a;
```

El mecanismo es correcto —`(set 115 (minus (reg f5) …))` le da la preferencia—
pero el reparto se desmonta: **90 filas**. La razón está medida: `DiffuseMaxA` se
carga en la fila 51 del objetivo y el `fsubs` está en la 103, así que el pin
reserva f5 **durante todo el bloque envmap**. La misma local **sin** pin da 27
filas (objeto idéntico a la base), lo que confirma que la local no molesta y que
lo que rompe es la vida del pin.

### 2.4 Y no hay escape por las preferencias

Leído `find_reg`: los dos bloques que reajustan `best_reg` a un registro
preferido (`hard_reg_copy_preferences` y `hard_reg_preferences`) enmascaran con
`used`, y **`used` conserva el valor de la pasada 0 —que incluye
`regs_someone_prefers`— cuando la pasada 0 encuentra registro** (el bucle sale
por `i >= FIRST_PSEUDO_REGISTER` sin llegar a copiar `used1`). Así que ni por ahí.

**Veredicto: frente cerrado, y además sin premio.**

---

## 3. `eProject` (268 B, 13 filas) — el `asm` NO está en el original, demostrado

`lmap.py` sobre el objetivo da el mapa de sentencias completo. Correspondencia
línea del original → línea nuestra:

```text
225 cabecera | 231/232/233 local.x/y/z | 234 MTXMultVec | 236 el if
238 *sy=0;*sx=0  | 239 *sz  | 240 return | 244/245/246 clipX/clipY/clipZ
248 oneOverW | 251/252 halfVP2/halfVP3 | 254/255/256 *sx/*sy/*sz | 257 la llave
```

El desplazamiento vale **+31** desde nuestra 207 hasta la 221 y pasa a **+30** en
la 224. O sea: entre `halfVP3` y `*sx` el original tiene **UNA** línea (la 253,
sin código) y nosotros **DOS** (el `__asm__("" : "+f"(halfVP2))` y el blanco).
**No hay hueco para ninguna sentencia ahí: el `asm` es nuestro.**

No lo he retirado porque **empeora** (13 → 17 filas, y el brief prohíbe dejar
peor una función), pero es una decisión que hay que tomar arriba: es deuda en una
función que no cierra. Queda escrito en el fuente.

De paso, el mismo mapa **renombra las 13 filas**. No son «dos ciclos de dos»: son
**una transposición de dos cadenas independientes** más su dominó de registros.
El objetivo emite, por líneas:

```text
[254 lfs vp[0]] [251 fmuls halfVP2] [244 lfs eye.x] [254 fadds] [246 lfs pm[6]] [244 fmadds clipX]
```

y nosotros adelantamos la cadena de 244 y atrasamos la de 254. Los `f0<->f13` y
`f12<->f13` de las filas 49-53 son consecuencia. (Y el `fres` sale de
`eMathE.cpp:56`, `eRecip` inline, no de la 248.)

---

## 4. `GenerateHorizonFogDisplayList` (796 B, 2 filas) — última hipótesis descartada

```text
80106F48  add r0, r31, r0        EcstasyEx.cpp:766  EcstasyEx.cpp:764
80106F4C  fmr f30, f22           EcstasyEx.cpp:767
80106F50  andi. r11, r31, 0x1    (sin nota propia: hereda la 767)
80106F54  srawi r0, r0, 1        EcstasyEx.cpp:764
80106F58  xoris r0, r0, 0x8000   EcstasyEx.cpp:766
```

El `andi.` **no tiene línea propia**: es de la 767, la misma que el `fmr f30,f22`
que va justo delante. El `srawi` es de la 764 (el `/2`) y el `xoris` de la 766
(la conversión int→float). O sea que en el original las dos insns de la 767 salen
**juntas** y el `srawi` se cuela **detrás**; en el nuestro se cuela **delante**.
**No hay ninguna sentencia intermedia que mover** —la 765 no emite nada en el
original, igual que aquí—, con lo que se descarta la única hipótesis que quedaba
viva (que al objetivo le entrara ahí código que nosotros no tenemos) y el muro
numérico de la r30 (`prio(srawi)=12` contra `prio(andi.)=2`) queda confirmado por
una vía independiente.

---

## 5. `epCalculate` (2.072 B) y `Initialize` (2.352 B) — sin medidas nuevas

- **`epCalculate`**: el mapa de líneas del original abarca ~230 líneas distintas
  de `EcstasyEx.cpp` (4504-4957 más el bloque inline 2539-2713 y
  `GameFlow.hpp:53`). Con 164 filas y la única hipótesis viva siendo
  `secondary_memlocs_elim[]` de `reload`, no es abordable en una ronda con
  presupuesto compartido. Volcado guardado en `scratchpad/r49_ecs/lmap_ep.txt`.
- **`Initialize`**: las 28 filas siguen siendo tres problemas independientes
  (árbol del `switch` 14, orden `(offset,base)` 6, asociatividad de
  `&s[nameLength+1]` 8) y el primero está demostrado inalcanzable desde la r47
  (`jump.c:1831` exige `node_is_bounded({2})`, que con esta lista de `case`
  necesitaría `low == TYPE_MIN` o un antecesor con `high == 1`). No lo he tocado.

---

## 6. Propuestas fuera de territorio

1. **`lmap.py` sobre el ORIGINAL, aplicado a los ACCESORES EN LÍNEA.** Regla
   nueva y barrible en todo el árbol: **si una carga o un producto del objetivo
   lleva la línea del LLAMANTE pero está colocado DESPUÉS del `if/else` de un
   accesor inline, la aritmética estaba escrita en el llamante, no dentro del
   accesor.** GCC 2.95 evalúa los argumentos en el punto de llamada
   (`integrate.c:1540` + `process_reg_param`: un MEM siempre se copia a un
   pseudo ANTES del cuerpo), así que un `Get(x)` con `x` de memoria **nunca**
   puede poner esa carga detrás del cuerpo. La firma es exactamente la de
   `EvalState`: un `INSERT` en el bloque de entrada y un `DELETE` en el bloque de
   unión, con el tamaño total igual. Valió 34 filas y quitó la única local
   sobrante que `regmap` señalaba. Merece una línea en el brief junto a la regla
   de la r47.
2. **Corregir el brief sobre el movimiento interbloque de haifa.** «El
   planificador lo adelanta» sólo puede pasar **hacia arriba**: `compute_trg_info`
   toma candidatos de bloques posteriores al destino. Si el objetivo tiene una
   insn MÁS ABAJO que nosotros, no es el planificador —es la fuente—. Ese error
   costó una ronda entera en `EvalState`.
3. **`matched_code` todo-o-nada debería filtrar el REPARTO de territorios.**
   `UpdatePlatInfo` (2.044 B) lleva tres rondas siendo «la mejor apuesta» de esta
   clave y **no puede cerrar**: sus 6-7 filas del bloque de literales están
   demostradas irreducibles desde la r48. Antes de asignar una función como
   objetivo principal conviene comprobar si alguna de sus diferencias está ya
   marcada como cerrada-con-prueba; si lo está, el resto del trabajo vale cero
   bytes por definición.
4. **`eProject`: decisión pendiente arriba.** El `asm` de la r36d está
   **demostrado** ausente del original (mapa de líneas, § 3) y compra 4 filas en
   una función que no cierra. O se retira (13 → 17 filas, 0 bytes en los dos
   casos) o se acepta explícitamente como deuda conocida.
5. **Disco.** `scratchpad/rtl` sigue teniendo 22 MB de volcados ajenos y el disco
   está en **3,2 GB libres** con seis agentes compilando. Yo he borrado los míos
   (41 MB). Sigue siendo un fallo silencioso a la vuelta de la esquina.

---

## 7. Verificación

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEcstasy Speed/Indep/SourceLists/zEagl4Anim
  -> 2 ok, 0 fallidas

python scripts/fncmp.py Speed/Indep/SourceLists/zEcstasy
  ENTRADA y SALIDA IDENTICAS:
    4 de 539 distintas, 5.180 B  (epCalculate 164, UpdatePlatInfo 27,
                                  GenerateHorizonFog 2, eProject 13)
    131 mas solo con nombres distintos (76.252 B)

python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  entrada: 2 de 318 distintas, 2.808 B  (Initialize 2356/2352, EvalState 52)
  SALIDA : 2 de 318 distintas, 2.808 B  (Initialize 2356/2352, EvalState 18)
    113 mas solo con nombres distintos (87.768 B)   <- IGUAL que a la entrada

python scripts/fndiff.py zEagl4Anim EvalState__Q29EAGL4Anim14FnRawStateChanfPQ29EAGL4Anim5State
  -> target=98.070175%  ours=98.070175%  size=456/456   (entrada: 93.50877 %)

python scripts/regmap.py zEagl4Anim "FnRawStateChan::EvalState" --all
  -> MISMO conjunto de locales y mismo arbol de bloques (entrada: 1 local
     "SOLO NUESTRA")

python scripts/audit.py Speed/Indep/SourceLists/zEcstasy     -> 0 FALLA
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim   -> 0 FALLA
python scripts/lcfix.py --check   -> todas las entradas @lc estan al dia

git diff --stat (solo lo mio)
  src/Speed/GameCube/Src/Ecstasy/EcstasyEx.cpp     | 12 +++   (solo comentarios)
  src/Speed/GameCube/Src/Ecstasy/eLightE.cpp       | 39 +++   (solo comentarios)
  src/Speed/GameCube/Src/Ecstasy/eMathE.cpp        | 19 +++   (solo comentarios)
  src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp   | 57 +++-  (el cambio real)
CRLF conservado en los cuatro (236/236, 843/843, 566/566, 3894/3894).
```

**Ninguna función de las dos unidades empeora; ninguna cambia de tamaño.** Sin
commits, sin `ninja`, sin `configure.py`, sin `config/GOWE69/*`, sin
`splits.txt`, y **sin añadir un solo `asm`**. Volcados RTL borrados
(`scratchpad/r49_ecs/rtl`, 41 MB).

Bancos reproducibles en `scratchpad/r49_ecs/`: `rsc.py` (TU reducida de
`RawStateChan.cpp` + comparación canónica contra el original, 5 s), `ev.py` /
`ev2.py` / `sw.py` (variantes de `EvalState`), `probe.py` (mult:SF, preferencias
y `reg_renumber` del `.lreg`/`.greg`), `g.py` / `gold.py` / `sweep.py` (banco de
`UpdatePlatInfo` heredado de la r48), `lmap_ep.txt`.
