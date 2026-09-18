# r36f — zWorld / zWorld2 / zPhysicsBehaviors

**Resultado: NADA cerrado y CERO cambios en la fuente** (todo medido y revertido,
regla 4 del brief). Lo que se lleva la ronda son **tres vedas rotas**, **una veda
caducada**, **un frente unificado de 4.640 B en tres funciones y dos unidades**, y
**un peligro global de `$LC` que romperia el DOL en silencio**.

Verificacion final, con los tres objetos recien compilados desde fuentes
identicas a HEAD:

```
fncmp zWorld            5 funciones / 5.608 B   identico a la base
fncmp zWorld2           2 funciones / 3.796 B   identico a la base
fncmp zPhysicsBehaviors 2 funciones / 2.344 B   identico a la base
pctsnap --cmp base.json fin.json  (2.059 funciones, 3 unidades)
  EMPEORAN: ninguna
  MEJORAN:  0 funciones
git status src/  ->  ninguno de mis ficheros modificado
```

`lcfix.py --check` **NO esta limpio**, pero no por mi: ver §6.

---

## 0. Lo que se lleva la ronda

1. **`InitAtSegment`: DOS vedas rotas.** El racimo de doce filas que la r36d
   declaro inatacable («no se ataca con bloqueos ni con el permutador») **si se
   ataca**, con el eje «a quien» sobre una local que nombre `WRoadNetwork::fNodes`.
   Y el pin `asm("r25")` sobre `segment`, que puso la r36b, **ya no paga**:
   quitarlo baja otras tres filas. La receta completa deja la funcion en **3
   filas de 15**, pero se rechaza porque empeora las otras dos metricas. §3
2. **`SetMemoryPoolSize`: el mando que mueve el `addi` es el NUMERO DE
   DEPENDIENTES**, no la prioridad ni el LUID. `rank_for_schedule` desempata por
   dependientes ANTES que por LUID, y darle al `lo_sum` un segundo dependiente
   **si lo pone delante del `stw`** — con tamano exacto 304/304. Lo que sobra es
   que ese dependiente sube +1 la cadena de prioridad. Requisito exacto y por que
   no hay donde cumplirlo en esta funcion, en §1. Once ensayos nuevos.
3. **Un solo frente para `UpdateLoaded` (856 B), `RenderFlaresOnCar` (2.908 B) y
   `UpdateWheelYRenderOffset` (876 B): 4.640 B, dos unidades, UN mecanismo.** La
   r36c lo diagnostico dentro de `UpdateLoaded` y nadie lo habia conectado con
   las otras dos. Con la evidencia de reubicaciones de los tres objetos. §5
4. **El eje «a quien» NO vale en `HolePunchAvoidables`**: nueve colocaciones y
   multiplicidades, todas peores que la base. Y un dato reutilizable: **repetir
   el mismo operando de entrada en el MISMO `asm` no sube `n_refs`**. §2
5. **Veda caducada en `CullParts`**: «mover `debug_print = NumPlanes` al principio
   del cuerpo da 836 B pero 59 diffs» es FALSO — sale **byte a byte identico a la
   base**. §4
6. **344 entradas de `keep.lst` en 19 unidades estan desplazadas exactamente 4**,
   mis tres incluidas, por un cambio en marcha FUERA de mi territorio. Quien
   integre la ronda tiene que pasar `lcfix.py` (sin `--check`) antes del DOL. §6

---

## 1. `SetMemoryPoolSize` (304 B, 2 filas) — el mando es el numero de dependientes

La r36e cerro el diagnostico al ciclo: el `addi @l` es la ultima insn del bloque
en el RTL post-`sched1`, y en el ciclo 15 la lista de listos es
`{250 mr r3,r0, 232 stw, 256 addi}` con **prioridades iguales**, asi que decide el
`INSN_LUID`. Lo que la r36e no probo es el escalon anterior del desempate.

`rank_for_schedule` compara, en este orden: prioridad → (spec/prob) →
**numero de dependientes (`INSN_DEPEND`)** → `INSN_LUID`. Los tres candidatos
tienen **un** dependiente (el `bl`). **Darle al `addi` un segundo dependiente
gana el desempate sin tocar el LUID**, y funciona:

| ensayo | tamano | filas | que pasa con el `addi` |
|---|---|---|---|
| base | 304/304 | 2 | detras del `stw` |
| B1 `nm`+`pool_mem` locales, `asm("":"+r"(pool_mem):"r"(nm))` | 304/**308** | 5 | **DELANTE del `stw`** |
| **D = B1 + `register const char *nm asm("r6")`** | **304/304** | **4** | **DELANTE del `stw`** |

D es el mejor estado que ha tenido esta funcion: **tamano exacto** y el orden
`lis | addi | ... | stw` que pide el objetivo. Lo que falla es la colocacion:

```
objetivo : mr r0,r3 | lwz r5 | lwz r4 | lis r6 | addi r6 | stw | bl
D        : lis r6   | mr r0,r3 | addi r6 | lwz r5 | lwz r4 | stw | bl
```

El `asm` alarga la cadena `lis -> addi -> asm -> bl`, asi que
`prio(addi) = prio(bl)+2` en vez de `+1` y el par `lis/addi` sube tres ranuras.

**Requisito exacto para cerrarla** (deducido y comprobado): el segundo
dependiente del `addi` tiene que tener **prioridad <= `prio(bl)`**, o sea que su
salida se consuma **DESPUES** del `bl bInitMemoryPool`. En este bloque solo hay
dos cantidades asi: el `@ha` de `CarLoaderMemoryPoolNumber` (r29, que no se puede
nombrar desde C sin convertir el `stw @l(r29)` en `addi`+`stw 0(rN)`) y `this`
(r31). Medido con `this`:

| ensayo | tamano | filas |
|---|---|---|
| K1 `CarLoader *self = this;` + `asm("":"+r"(self):"r"(nm))`, `self` solo al final | 308 | 8 |
| K2 idem sin el pin de r6 | 312 | 9 |
| K3 idem usando `self` tambien en los argumentos | 308 | 10 |

Falla siempre por lo mismo: `this` sigue vivo (los dos `lwz` de los argumentos) y
`self` tras el `asm` es otro pseudo, asi que hacen falta dos registros y sale un
`mr`. **El frente queda con el mecanismo identificado y sin sitio donde
aplicarlo.**

### Las once vedas nuevas (todas con `fndiff` sobre la unidad completa)

| ensayo | tamano | filas |
|---|---|---|
| A `int pool_num=...; asm("":"+r"(pool_num)); CLMPN=pool_num` (receta del 6.o uso) | 308 | 2 + `mr r3,r0` real |
| C `asm("":"+r"(pool_num):"r"(nm))` | 308 | 7 |
| E `int stored=pool_num; asm("":"+r"(stored)); CLMPN=stored;` y `pool_num` de 1.er arg | 304 | **identica a la base** |
| F2 `asm("":"+m"(CarLoaderMemoryPoolNumber):"r"(nm))` tras el store | 304 | 8 (desaparece el `mr r0,r3`) |
| G `const char *nm="Cars"` declarado ENTRE la llamada y el store, sin `asm` | 304 | **identica a la base** (CSE lo hunde) |
| H `stored` + `nm` encadenados por el `asm` | 308 | 5 |
| J1 `asm("":"+m"(this->NumSpongeAllocations):::"r0")` tras el store | **300** | 7 |
| J2 idem sin clobber | 300 | 7 |
| J3 `+m(this->MemoryPoolMem)` tras el store | 300 | 7 |
| J4/J5 `+m(this->MemoryPoolSize)` tras el store, con y sin clobber de r0 | 300 | 6 |
| J6 `+m(TheTrackStreamer)` tras el store | 304 | 16 |
| J7 **solo** el pin `register const char *nm asm("r6")` | 304 | 4 |
| J8 J7 + `asm("":"+r"(nm))` antes del store | 304 | 4 |

Nota: G confirma que **no basta con adelantar la declaracion del literal**; y E
es otra caida en la trampa de DCE/coalescing que documenta la r36d (numero
EXACTAMENTE igual al de la base).

`regmap --all` sobre la funcion: **MISMO conjunto de locales, mismo arbol de
bloques y MISMO REPARTO** (4 iguales, 0 distintas). La fuente es la del original.

---

## 2. `HolePunchAvoidables` (2.980 B, 44 insn / 49 filas) — el eje «a quien» no llega

`regmap --all` sobre la base de la r36e da el veredicto mas limpio posible: **87
locales iguales, 2 con registro distinto** (`avoidable_half_width` f9->f8,
`hole_punch_safety_margin` f4->f6), 0 de ambito equivocado, 0 solo del original,
0 solo nuestras.

La r36e cerro el frente del pin en esas dos locales (lo rechazan sueltas y
acompanadas) y dejo escrito «ya NO se ataca con un pin». La palanca que quedaba
en el catalogo es el **eje «a quien»** (`asm("" : "+f"(x) : "f"(y))`, el unico
mando conocido sobre una permutacion entre pseudos). **Nueve medidas, todas
peores que la base (49 filas / 98,169 %, 2980/2980 en todas):**

| ensayo | filas | fuzzy |
|---|---|---|
| A1 `+f(gap_required) : f(hole_punch_safety_margin)` | 70 | 97,267 |
| A2 idem con el operando de entrada **repetido** | 70 | 97,267 |
| A3 `+f(gap_right) : f(avoidable_half_width)` | 58 | 97,626 |
| A4 `+f(gap_required) : f(avoidable_half_width)` | 66 | 97,334 |
| A5 `+f(gap_left) : f(hole_punch_safety_margin)` | 67 | 97,773 |
| A6 A1 + A3 juntas | 66 | 97,396 |
| A7 `+f(lateral_projection) : f(avoidable_half_width)` | 55 | 98,129 |
| A8 `+f(lateral_projection) : f(hole_punch_safety_margin)` | 59 | 98,062 |
| A9 idem con la entrada repetida **cuatro** veces | 61 | 98,042 |

**Dato reutilizable fuera de mi territorio**: A1 y A2 dan el MISMO numero, y en
`InitAtSegment` pasa igual (I2 = I3 = I4, objetos identicos). Repetir el mismo
operando de entrada dentro del mismo `asm` **no sube `n_refs`** — el brief dice
que sube «a saltos de `floor_log2`», y esta medida acota como NO se consigue el
salto: hay que repartir las referencias en `asm` distintos, no apilarlas en uno.

---

## 3. `InitAtSegment` (816 B, 15 insn) — dos vedas rotas y una receta de 3 filas

### 3.1 La veda rota

La r36d dejo escrito, sobre el racimo de doce filas (`fNodes` r10<->r11 contra el
`@ha` del 0,0f):

> «Los dos rangos se **solapan**, asi que cualquier registro que bloquees se lo
> quitas a los dos a la vez: hay que conseguir que `local_alloc` reparta `fNodes`
> primero. **No se ataca con bloqueos ni con el permutador.**»

El diagnostico es correcto y la conclusion no: no hay que **bloquear**, hay que
**subir `n_refs` de `fNodes`**, y para eso hace falta nombrarlo. Se puede:
`WRoadNav` es `friend` de `WRoadNetwork` (comentario en `WRoadNetwork.h:132`,
«it accesses fNodes without the getter»), asi que

```c
const WRoadNode *nodes = WRoadNetwork::fNodes;
__asm__("" : "+r"(lane) : "r"(nodes));
fStartPos = roadNetwork.GetNode(segment->fNodeIndex[fNodeInd == 0])->fPosition;
fEndPos   = roadNetwork.GetNode(segment->fNodeIndex[fNodeInd])->fPosition;
```

**hace desaparecer el racimo entero de las filas 87-120.** La local sola, sin el
`asm`, es neutra (15 filas, byte a byte identica): lo que decide es la referencia
de ENTRADA, tal y como dice el brief.

### 3.2 La segunda veda rota: el pin de r25 ya no paga

Las tres filas 12/17/20 (`mulli`/`lwz fSegments`/`add r25`) son el coste del pin
`register const WRoadSegment *segment asm("r25")` que puso la r36b. Con el racimo
grande cerrado, **quitar el pin baja de 12 a 9 filas** y no rompe nada.

### 3.3 Las medidas, y por que se revierte igual

| variante | filas `fndiff` | fuzzy | `fncmp` insn |
|---|---|---|---|
| base (pin r25, sin barreras) | 15 | **99,559** | **15** |
| I2 = base + `nodes` + eje «a quien» | 12 | 96,833 | 15 |
| N2 = I2 **sin** el pin de r25 | 9 | 96,931 | **12** |
| **L1 = N2 + `asm("":"+r"(segment))` antes de `GetSegmentNodes`** | **3** | 98,971 | 19 |

L1 deja **tres** filas: `mr r3,r30` dos ranuras tarde y `mr r4,r25` diecisiete
ranuras tarde — **una sola copia de argumento descolocada**, que es un
diagnostico muchisimo mas cerrado que la permutacion r10/r11 de partida.

**Se revierte**, y conviene que quede claro por que, porque es un caso de libro de
`nfsmw-medidas-que-enganan` en las dos direcciones:

- el `fuzzy` **baja** (99,559 -> 98,971) porque el `mr` desplazado mete un
  INSERT/DELETE, que puntua cero; `pctsnap` la daria por EMPEORADA;
- `fncmp` (byte a byte en el MISMO desplazamiento) **sube** de 15 a 19, porque el
  `mr` corrido desplaza una posicion a las 17 instrucciones siguientes;
- solo el recuento de filas de `fndiff` mejora, y `matched_code` es todo-o-nada:
  ninguna de las cuatro variantes llega al 100 %.

O sea: la regla 2 del brief («sin que ninguna empeore») la rompen L1 (fncmp) y N2
(fuzzy). Queda escrito para que quien la retome parta de L1 y ataque **solo** la
copia `mr r4, r25`.

### 3.4 Las colocaciones medidas (para no repetirlas)

Sobre la base **con** pin: I1 local `nodes` sola 15; I2 12; I3 (2 entradas) 12;
I4 (4 entradas) 12; I5 `nodes[...]` en vez de `GetNode` 16; I6 idem + barrera 23;
J1 `dst` local + barrera 11; J2 `dst` sin barrera 12; **J3 `+r(segment)` antes de
la llamada 6**; J4/J5 barrera de ranura `+m(fEndPos)` con y sin clobber de r0
**27**; J6 `+m(fStartPos)` tras el store 12; J7 `dst`+`rn` 13; K1 seg+dst 11;
K2 doble barrera sobre `segment` 16; K3 `rn`+seg 11; K4 `+r(segment):r(lane)` 6;
K5 barrera antes del array 6; K6 seg+lane 24; K7 seg con clobber de r0 18;
L2 pin en r26 17; L3 pin r25 + barrera 6; L4 `&WRoadNetwork::fSegments[segInd]` 3.

Sobre la base **sin** pin: M1 barrera sobre una copia `seg2` 7; M4 doble 13;
M5 `+r(segment):r(nodes)` **3**; M6 barrera antes de `guard` 13; M7 seg+nodes
**3**; N1 (solo J3, sin la barrera de `nodes`) 17; P3 `+r(nodes):r(segment)` 9;
P4 `+r(lane)` 33; P5 `+r(nodes)` 9; Q1 J3 con clobber r0 15; Q2 J3+dst 8;
Q3 J3+`rn` 8; Q4 `nodePtr` declarado antes de `guard` 3; Q5 barrera tambien tras
la llamada 13; R1 barrera antes de `nodePtr` 3; R2 barrera DESPUES de la llamada
9; R3 antes de `GetProfile` 9; R5 antes y despues 13; S1 argumento pinchado a r4
sin J3 9; S2 con J3 3; S3 `:r(guard)` 11; S4 J3 + ranura `+m(fSegTime)`+r0 3;
S5 J3 + `this` pinchado a r3 3; T1 copia previa para el argumento 9; T2 barrera
sobre la copia 9; T3/T4 `+r(lane):r(segment)` x1 y x4 **33**; U1 barrera solo
sobre `rn` 10; U2 barrera solo sobre `dst` 8.

**Tres filas es un suelo duro**: nueve variantes distintas caen exactamente ahi.

---

## 4. `CullParts` (836 B, 5 filas) — una veda caducada y siete medidas

La forma exacta del hueco, ahora leida entera: el objetivo mete `mr r9, r0` en el
preencabezado (`debug_print = plane_info->NumPlanes` ejecutado tambien en el
camino que se SALTA el bucle) y compara la guarda contra el cero de `debug_print`
(`cmpw r9, r0`); nosotros reusamos el cero que ya trae el registro de `Polarity`
(`cmpw r11, r0`) y no necesitamos la copia. Cuando el `bge` salta, en el objetivo
r9 vale `NumPlanes` y en el nuestro 0; las dos versiones son correctas para
`NumPlanes >= 0`.

| ensayo | tamano | filas | fuzzy |
|---|---|---|---|
| base | 832 | 5 | 99,4258 |
| C1 `debug_print = NumPlanes` tambien antes del bucle | 828 | 8 | 98,876 |
| C2 `asm("":"+r"(debug_print))` tras la declaracion | 832 | 9 | 99,330 |
| C3 idem justo antes del bucle | 832 | 7 | 98,469 |
| C4 eje «a quien» `+r(n):r(debug_print)` | 832 | 48 | 97,321 |
| C5 `+r(debug_print):r(NumPlanes)` | 832 | 7 | 98,469 |
| **C6 `debug_print = NumPlanes` al PRINCIPIO del cuerpo** | **832** | **5** | **99,4258** |
| C7 cantidad fantasma en r9 alrededor del bucle | 832 | 5 | 99,4258 |

**C6 caduca una veda**: el comentario de la r36d dice «moverlo al principio del
cuerpo (836 B pero 59 diffs)» y sale **byte a byte identica a la base**. C7 es
otra trampa de DCE de manual (`asm("":"=r"(guard))` sin consumidor vivo).

---

## 5. Un solo frente de 4.640 B: `UpdateLoaded` + `RenderFlaresOnCar` + `UpdateWheelYRenderOffset`

La r36c diagnostico el mecanismo **dentro de `UpdateLoaded`** y nadie lo habia
conectado con las otras dos. Es el mismo, y las tres se cierran con la misma
palanca:

> PRE (`gcse.c`) unifica los N calculos de `(high (*$LC))` en un pseudo e inserta
> copias; **`cse2` lo deshace** (las copias vuelven a ser `(set (regN) (high ...))`);
> y entonces `update_equiv_regs` de `local-alloc.c` **hunde** todo pseudo al que le
> quede UN solo uso hasta ese uso, donde ya no cruza ninguna llamada y
> `global_alloc` le da un VOLATIL en vez de un preservado.

La regla que sale de los tres objetos, y que se comprueba en el nuestro:
**un pseudo `high(literal)` que acaba con >=2 usos sobrevive a un registro
PRESERVADO; el que acaba con uno se hunde siempre a un volatil.**

| funcion | unidad | B | objetivo | nuestro | delta |
|---|---|---|---|---|---|
| `UpdateLoaded` | zPhysicsBehaviors | 856 | **un** `lis r30, lbl_803FB6B8@ha` (fila 86, antes de `bl VU0_Atan2`) con 2 usos, `stmw r30`/`lmw r30`, marco 0x30 | **dos** `lis r9, $LC917@ha` (filas 100 y 119), `stw r31`/`lwz r31`, marco 0x28 | +4 B |
| `RenderFlaresOnCar` | zWorld | 2.908 | **un** `lis r16, lbl_8040AD04@ha` (fila 123) con TRES usos (460, 600, 601), vivo 330 instrucciones | `lis r9` (1 uso, hundido) + `lis r30` (2 usos, conservado) | +4 B |
| `UpdateWheelYRenderOffset` | zWorld | 876 | **DOS** `@ha` de `lbl_8040AA84` (reubicaciones en +0x86 y +0xC6, **las dos con addend 0**) y por eso NO le queda sitio para izar `TweakKitWheelOffsetRear@ha`, que rematerializa en r9 dentro del bucle | UN solo `@ha` del literal y el `Tweak` izado a r16 | -4 B |

Las dos primeras necesitan **mas** unificacion, la tercera **menos**; es el mismo
grado de libertad. Y `regmap` confirma que en las tres **no falta ni sobra
ninguna local**: en `RenderFlaresOnCar` las tres permutaciones que senala
(`flashHeadlights` r14->r16, `renderFlareFlags` r17->r16, `force_light_state`
r30->r17) son consecuencia de que a nosotros nos falta el pseudo que ocupa r16,
no causa propia (40 iguales, 3 distintas, 0 sobrantes, 0 faltantes).

**Lo que hay que romper es la sustitucion de `cse2`**, y desde la fuente no se
llega: el objeto es un pseudo del compilador. Las palancas que si se pueden
nombrar dan **otra forma**, no la del objetivo (un FPR preservado con un solo
`lfs`, cuando el objetivo hace `lfs` dos o tres veces desde el mismo GPR).

**Recomendacion para quien reparta la proxima ronda: esto es UN encargo, no tres.**
Vale 4.640 B y cae de golpe si alguien encuentra como impedir que `cse2` vuelva a
expandir `(set (regN) (reg362))` a `(set (regN) (high (*$LC)))`.

---

## 6. FUERA DE MI TERRITORIO Y URGENTE: 344 `$LC` desplazados exactamente 4

```
python scripts/lcfix.py --check
  344 entradas CORRIGE, en 19 unidades:
  zAI zAnim zCamera zEAXSound zEAXSound2 zEcstasy zFe zFe2 zFeOverlay
  zGameplay zLua zMain zPhysics zPhysicsBehaviors zPlatform zSim zSpeech
  zWorld zWorld2
  desplazamiento: 4 en las 344 (histograma de un solo valor)
```

Mis tres unidades estan en la lista **con la fuente byte a byte igual a HEAD y
recien recompiladas**, asi que el desfase no viene de mi. Un desplazamiento
UNIFORME de 4 en 19 unidades solo lo puede producir una cabecera incluida por
todas. El unico fichero de cabecera modificado en el arbol de trabajo es
`src/Speed/Indep/Src/Misc/PackedDecimal.h` (+68/-41, otro agente de esta misma
ronda), y esta en el cierre de inclusiones de las 19 (aparece en la tabla de
ficheros DWARF de `zWorld.o`, `zWorld2.o`, `zPhysicsBehaviors.o`, `zLua.o`,
`zSpeech.o`, `zEAXSound.o`, `zPlatform.o` y `zAnim.o`).

Es exactamente el fallo que documenta `nfsmw-lc-se-desplaza`: **`-strip-unused-data`
se lleva la cadena que la entrada ya no nombra y el DOL rompe EN SILENCIO**
(`keepchk.py` la sigue dando por buena). **Antes de construir el DOL de esta
ronda hay que pasar `python scripts/lcfix.py` sin `--check`**, y conviene que lo
haga quien integre, no cada agente por su cuenta, porque el desplazamiento
depende del estado final de la cabecera.

---

## 7. Estado de las tres unidades (sin cambios)

```
                  pendiente                lo que queda
zWorld            5.608 B  5 funciones     RenderFlares 2908 (+4 B), UpdateWheelY 876 (-4 B),
                                           CullParts 836 (-4 B), DefragmentPool 684 (23 insn),
                                           SetMemoryPoolSize 304 (2 insn)
zWorld2           3.796 B  2 funciones     HolePunchAvoidables 2980 (44 insn), InitAtSegment 816 (15 insn)
zPhysicsBehaviors 2.344 B  2 funciones     RBGrid::Add 1488 (CERRADO r36e-ai), UpdateLoaded 856 (+4 B)
```

Las tres son `NonMatching` en `configure.py`: nada de esta ronda podia tocar el DOL.

`DefragmentPool` (684 B, 23 insn) no se ha tocado: las once vedas de la r36e mas
las nueve medidas negativas del eje «a quien» de §2 hacen muy improbable que la
misma palanca funcione en una permutacion de siete registros.

`RBGrid::Add` no se ha tocado (frente cerrado con prueba en la r36e-ai).

---

## 8. Para quien siga (fuera de mi territorio)

1. **`rank_for_schedule` desempata por NUMERO DE DEPENDIENTES antes que por
   `INSN_LUID`** (§1). Es un mando distinto de la prioridad y de la barrera de
   ranura, y se acciona con `asm("" : "+r"(otro_argumento) : "r"(el_descolocado))`.
   **Condicion para que no cueste**: la salida del `asm` tiene que consumirse
   DESPUES de la llamada; si la consume la propia llamada, la prioridad del
   productor sube +1 y el par `lis/addi` se va varias ranuras arriba.
2. **Repetir el mismo operando de entrada dentro de un `asm` NO sube `n_refs`**
   (§2 y §3.4): A1 = A2 y I2 = I3 = I4 dan objetos identicos. Para subirlo hay
   que repartir las referencias en `asm` distintos.
3. **Un pin puesto hace tres rondas puede ser hoy un coste neto** (§3.2). El pin
   de r25 de `InitAtSegment` valia cuando lo pusieron y hoy cuesta tres filas.
   Es una variante nueva de `nfsmw-vedas-caducan`: **caduca la palanca, no solo la
   veda**. Antes de dar una funcion por atascada, prueba a QUITAR los `asm` que
   ya tiene.
4. **Cuando el eje «a quien» apunta a un pseudo del compilador, busca si hay un
   simbolo de la fuente que lo nombre** (§3.1). Aqui era un miembro estatico
   privado al que la clase llamante ya tenia acceso por `friend`; una linea de
   `WRoadNetwork.h` lo decia. Con eso cayo un racimo de doce filas que llevaba
   tres rondas declarado inatacable.
5. **Las tres metricas se contradicen y hay que dar las tres** (§3.3). En
   `InitAtSegment` una misma variante mejora `fndiff` de 15 a 3 filas, empeora
   `fncmp` de 15 a 19 insn y baja el `fuzzy` 0,6 puntos: un `mr` corrido
   diecisiete ranuras desplaza todo lo que hay detras. Dar solo el recuento de
   filas habria colado un cambio que `pctsnap` marca como EMPEORA.
