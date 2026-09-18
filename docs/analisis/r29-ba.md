# r29 — el eje SELECTIVO barrido en las cinco candidatas

Encargo: reabrir con la **barrera selectiva** (asm extendido NO volátil) las vedas
de `CheckCollideSRB`, `UpdatePlatInfo`, `SetMemoryPoolSize`,
`TrackCopCameraMover::Update` y `_bOutput`.

## 0. Verificación del encargo — reproduce EXACTO

`build_direct.py zWorld2 zEcstasy zWorld zCamera` + `triage.py --muro` da las
cinco líneas al dígito:

    1044 B  99.138%  zWorld2   CheckCollideSRB            45 registros
    2044 B  98.659%  zEcstasy  UpdatePlatInfo             dmax 5
     948 B  99.156%  zCamera   Update__19TrackCopCamera…  dmax 2
     304 B  97.368%  zWorld    SetMemoryPoolSize          dmax 2
    5180 B  99.983%  zBWare    _bOutput                   2 registros

`measure.py zWorld2` de partida: **133.264/139.388 B, 95,6065 %, 353 funciones**.

## 1. Resultado: UNA cerrada, 1.044 B, y **el hallazgo corregido**

| función | antes | después |
|---|---|---|
| **`WTriggerManager::CheckCollideSRB`** | 99,13793 % · 45 filas | **100 % · 0 filas · 1044/1044 B** |
| `UpdatePlatInfo` | 98,659 % | sin cambio (8 formas, todas peores) |
| `SetMemoryPoolSize` | 97,368 % · 2 filas | sin cambio — **pero el orden del objetivo SÍ es alcanzable a coste cero** (§4) |
| `TrackCop::Update` | 99,156 % · 2 filas | sin cambio (5 formas) |
| `_bOutput` | 99,983 % | sin cambio — **el ángulo nuevo también cae** (§6) |

`measure.py --cmp` (base y medida con `build_direct.py` delante de cada una):
**+1.044 B, +1 función, 1 unidad**. `zWorld2` 133.264 → **134.308 B**,
95,6065 → **96,3555 %**, 353 → **354 funciones**.

`audit.py Speed/Indep/SourceLists/zWorld2`, **cinco pasadas**: **354 ok, 0 FALLA**
(353 ok antes). `frozen.py cong` → `23c85e35916b96e2`, `chk` = idéntico.
`triage --muro` al cerrar: `CheckCollideSRB` **desaparece del muro** y las otras
doce líneas quedan en su cifra exacta de partida. **Ninguna regresión.**

**Un solo fichero mío, y una sola línea de código:**
`src/Speed/Indep/Src/World/Common/WTrigger.cpp` (+1 línea de código, +26 de
comentario). Los demás ficheros de `git status` son de otros agentes de la ronda.
**Sin commit.**

---

## 2. CORRECCIÓN AL HALLAZGO DE LA r28 — y es importante para todo el árbol

El brief dice: «`__asm__("")` es una barrera TOTAL; un asm extendido NO VOLÁTIL
no lo es». **La mitad de esa frase está mal, y lo he medido con el RTL delante.**

> **GCC 2.95 pone `MEM_VOLATILE_P` a TODO asm que no tenga operandos de SALIDA,
> aunque no lleve la palabra `volatile`.**

Volcado literal (`-dR`), la insn de `__asm__("" : : "r"(trig));`:

    (insn:TI 17 9 23 (asm_operands/v ("") ("") 0 [(reg/v:SI 31 r31)]
                                        [(asm_input:SI ("r"))]) -1
      (insn_list 11 (insn_list 7 (insn_list 9 (nil)))))

El **`/v`** es `MEM_VOLATILE_P`. Esa insn depende de las TRES copias de prólogo y
**46 insns dependen de ella**: es barrera total. O sea, la regla real es:

| forma | ¿barrera total? | ¿sobrevive a `flow.c`? |
|---|---|---|
| `__asm__("")` (sin operandos, `ASM_INPUT`) | **SÍ** | sí |
| `__asm__ __volatile__(…)` | **SÍ** | sí |
| **`__asm__("" : : "r"(x))` — sin salida** | **SÍ** (`/v` implícito) | sí |
| **`__asm__("" : "=r"(muerta) : …)` — salida MUERTA** | no | **NO: la borra `insn_dead_p`** |
| **`__asm__("" : "=r"(viva) : "0"(viva), "r"(x))`** | **no** | **sí** ← ésta es la palanca |

**La barrera selectiva exige las tres cosas a la vez**: salida, salida **viva**, y
la salida **atada con `"0"`** a su propio valor (si no, hace falta una copia).
Lo de la r28 (`"+r"(a_lp)`) cumple las tres por casualidad, porque `a_lp` se usa
después. **Seis formas con salida muerta dan el objeto base EXACTO** (45 filas):
`"=r"(dummy)` con `unsigned int`, con `int`, con `"memory"`, con puntero, con
`= 0`, y `"+r"(dummy)` inicializada a `trig`. Todas se borran.

---

## 3. `CheckCollideSRB` (1.044 B) — CERRADA, y la ruta no era ninguna de las cuatro

### 3.1 El cambio, una línea

```c
 float srRadiusPlusVel = srBody->GetSpeed() * dT + srRadius;
+__asm__("" : "=f"(srRadiusPlusVel) : "0"(srRadiusPlusVel), "r"(trig));
 UMath::Vector3 dVdT;
```

### 3.2 La ruta que faltaba: **subir `n_refs`, no mover insns**

La r28 dejó el problema reducido a `44·S + 112·a − 68·b > 0` con cuatro salidas,
las cuatro sobre **insns** (mover una, quitar dos, añadir tres). **Falta una
quinta variable en esa ecuación: `n_refs`.** `allocno_compare` es

    pri = floor_log2(n_refs) · n_refs / live_length · 10000 · size

y el numerador de `trig` sube de **112 a 116** con **una sola referencia más**
(28 → 29 refs; `floor_log2` sigue valiendo 4, así que no hay salto de escalón):

    srBody  17 refs / live 121 -> pri 5619   (era 17/120 -> 5666)
    trig    29 refs / live 200 -> pri 5800   (era 28/199 -> 5628)

**Un asm extendido no volátil CUENTA como referencia** (`flow.c` cuenta los
operandos de `ASM_OPERANDS` en `REG_N_REFS`) **y no emite un byte**. Salen los
1.044 B exactos y el reparto del objetivo: `mr r27,r4` / `mr r31,r5`.

Medido con `c28w2_lsweep.py` (el modelo exacto de la r28) sobre 8 formas:

| forma | refs/live srBody | refs/live trig | margen | reparto |
|---|---|---|---|---|
| base | 17/120 → 5666 | 28/199 → 5628 | −38 | 31 / 27 (mal) |
| **asm 1ª sentencia** | 17/121 → 5619 | **29/199 → 5829** | **+210** | **27 / 31 (bien)** |
| asm tras `radsSq` | 17/122 → 5573 | 29/201 → 5771 | +198 | 27 / 31 |
| asm tras `MakeMatrix` | 17/120 → 5666 | 29/204 → 5686 | +20 | 27 / 31 |
| asm al final | 17/120 → 5666 | 29/236 → 4915 | −751 | 31 / 27 |

### 3.3 Los ensayos, con su cifra (banco `c29ba_run.py`, 4 s por variante)

Base 99,13793 % / 45 filas / 1.044 B.

| # | forma | % | filas | B |
|---|---|---|---|---|
| **t2** | **`"=f"(srRadiusPlusVel)` + `"0"` + `"r"(trig)`** | **100** | **0** | **1044** ← APLICADA |
| t1 | idem atado a `srRadius` | **100** | 0 | 1044 |
| t5 | idem atado a `dT`, tras `rPos` | **100** | 0 | 1044 |
| t6 | atado a `srRadius`, con `"m"(*trig)` | **100** | 0 | 1044 |
| t3 | atado a `radsSq` | 97,586 | 11 | 1044 |
| t7 | atado a `dT` **sin** `"r"(trig)` | 97,625 | 48 | 1044 |
| t8 | atado a `srRadius` **sin** `"r"(trig)` | 99,138 | 45 | 1044 (= base) |
| a2/q1..q5 | asm **sin salida** (`"r"(trig)`, ×2, ×3, `"g"`, `+memory`, `+"f"(dT)`) | 99,2337 | **2** | 1044 |
| a1/a7 | asm sin salida tras `radsSq` / tras `cp` | 96,383 / 93,234 | 18 / 23 | 1048 |
| a3/a5 | asm sin salida al final / dos asm | 95,410 / 91,812 | 77 / 92 | 1044/1048 |
| a4 | **`"+r"(trig)`** (redefine `trig`) | 73,636 | 115 | **1080** |
| a6 | asm sin salida tras `MakeMatrix` | 85,008 | 63 | 1052 |
| p2..p8 | asm sin salida en 7 posiciones más | 94,1–96,8 | 14–23 | 1040–1048 |
| s1..s6 | asm con salida **muerta**, 6 formas | 99,138 | 45 | 1044 (= base) |

Dos lecturas que valen para todo el árbol:

- **`t8` prueba que la palanca es el `"r"(trig)`, no la barrera**: el mismo asm
  sin nombrar `trig` da el objeto base exacto.
- **`a4` prueba por qué no se puede escribir el pseudo que interesa**: `"+r"(trig)`
  lo redefine, **corta el CSE de todas las cargas `trig->…` posteriores** y sube
  `refs` de 28 a **39** — 36 B de más.

### 3.4 Deuda declarada (regla 3 del brief), y el mapa de líneas la deja

**El original no tenía un `asm`.** Lo que hace falta es **una referencia más a
`trig` que no emita código**, y no la he encontrado en C++.

**El mapa de líneas NO la contradice, y por eso se elige esta posición de las
cuatro que dan 100 %.** `lmap.py zWorld2 CheckCollideSRB…`:

    WTrigger.cpp:887  cabecera (prologo + fmr f30,f1)
    WTrigger.cpp:889  rPos = srBody->GetPosition()
    WTrigger.cpp:890  srRadius = srBody->GetRadius()
    WTrigger.cpp:891  srRadiusPlusVel = GetSpeed()*dT + srRadius
    WTrigger.cpp:894  UMath::Scale(...)

Entre la 891 y la 894 **el objetivo tiene DOS líneas sin instrucción (892 y
893)**, y nosotros necesitamos exactamente dos ahí: el `asm` y
`UMath::Vector3 dVdT;`. Las otras tres posiciones que cierran (`t1` entre 890 y
891, `t5` entre 889 y 890, `t6` = `t1`) **no tienen hueco**: ahí las líneas del
objetivo son consecutivas.

---

## 4. `SetMemoryPoolSize` (304 B) — el «negativo firme» de la r28 se CORRIGE a medias

La r28 lo cerró como negativo firme: «con esta fuente y estos cflags la ranura no
se puede mover». **Eso es falso: el orden del objetivo SÍ se alcanza, y a coste
cero de bytes.** Lo que no se alcanza es el match completo.

Diff de partida (2 filas): el objetivo hace `lis r6@ha · addi r6@l · stw r0,
CarLoaderMemoryPoolNumber@l(r29) · bl`; nosotros ponemos el `stw` **entre** el
`lis` y el `addi`.

### 4.1 La forma que pone el `stw` en su sitio (x2, 304/304 B)

```c
int poolNum = bGetFreeMemoryPoolNum();
const char *poolName = "Cars";
__asm__("" : "+r"(poolName) : : "memory");
CarLoaderMemoryPoolNumber = poolNum;
bInitMemoryPool(CarLoaderMemoryPoolNumber, this->MemoryPoolMem, this->MemoryPoolSize, poolName);
```

El clobber `"memory"` hace que el `stw` (escritura de memoria) dependa del asm, y
el asm depende del `addi` porque nombra `poolName`. **El `stw` sale detrás del
`addi`, como el objetivo, y sin un byte de más.** Coste: el mismo clobber empuja
también los **dos `lwz`** de los argumentos por debajo del asm.

### 4.2 Y la que arregla también los `lwz` (x4, 304/304 B, **3 filas**)

Izando los dos `lwz` a locales **antes** del asm:

```c
int poolNum = bGetFreeMemoryPoolNum();
void *poolMem = this->MemoryPoolMem;
int poolSize = this->MemoryPoolSize;
const char *poolName = "Cars";
__asm__("" : "+r"(poolName) : : "memory");
CarLoaderMemoryPoolNumber = poolNum;
bInitMemoryPool(CarLoaderMemoryPoolNumber, poolMem, poolSize, poolName);
```

sale `bl · lwz r5 · lwz r4 · lis · addi · stw · bl` — **todo en su sitio menos
`mr r0,r3`**, que se queda pegado al `stw` en vez de ir justo detrás del `bl`.
**No lo he aplicado**: 3 filas es peor que las 2 de la base y `matched_code` es
todo-o-nada.

### 4.3 Ensayos numerados (banco 4 s). Base 97,36842 % / 2 filas / 304 B

| # | forma | % | filas | B |
|---|---|---|---|---|
| w3/x6 | **control: los locales `poolNum`/`poolMem`/`poolSize`/`poolName` SIN asm** | 97,368 | 2 | **304** (= base) |
| xa | `"+r"(poolName)` + `"r"(poolNum)` **sin** `"memory"` | 97,368 | 2 | 304 (= base) |
| **x4/x5/xc/xd** | **loads izados + `"+r"(poolName)` + `"memory"`** | 96,579 | **3** | **304** |
| xf | `"+r"(poolNum)` + `"m"(*poolName)` + `"memory"` | 97,039 | 4 | 308 |
| x2 | `"+r"(poolName)` + `"memory"`, sin izar loads | 94,737 | 4 | **304** |
| x8/x9/xe | idem + `"r"(poolNum)` (1, 2 y 3 entradas) | 94,737 | 4 | 304 |
| w1 | `"+r"(poolNum)` + `"r"(poolName)` | 93,947 | 5 | **312** |
| w2 | idem + `"memory"` | 91,974 | 7 | 312 |
| w5 | `"+r"(poolNum)` + `"memory"` | 93,421 | 5 | 308 |
| x1 | `"+r"(poolNum)` + `"m"(*poolName)` | 95,263 | 4 | 308 |
| xb | `"+r"(poolName)` **y** `"+r"(poolNum)` + `"memory"` | 95,263 | 4 | 308 |
| w4 | `"+r"(poolName)`+`"memory"` **detrás** del store | 90,263 | 11 | 308 |
| w6 | asm detrás del store | 95,908 | 5 | **300** |
| x3 | x2 sin sacar `poolNum` | 93,000 | 9 | 308 |

**Veda con su sentencia**: barrí **21 formas** del asm alrededor de
`CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum();` y de
`bInitMemoryPool(…)`. La que da el orden bueno cuesta 0 B pero deja
**`mr r0,r3` una ranura tarde**, y **`"+r"(poolNum)` siempre cuesta 4 B**
(rompe el reuso de `r3` para el primer argumento y obliga a `mr r3,r0`). El
`"memory"` sin izar los loads cuesta otras 2 filas.

---

## 5. `UpdatePlatInfo` (2.044 B) y `TrackCop::Update` (948 B) — negativos con el eje nuevo

### 5.1 `UpdatePlatInfo`: la «ruta ancha» de la r28, barrida — **8 formas, todas peores**

La r28 dejó la ventana `live(envmap_min_b) ∈ [316, 318]` (hoy 315) como la ruta
que nadie había atacado. **El asm selectivo la ataca —añade +1 insn al `.sched`
sin emitir un byte— y no sirve, y ahora se sabe por qué**: `envmap_min_b` muere
en la línea de `envmap_min_ib = (int)(envmap_min_b*128.0f)`, y `diffuse_min_r`
sigue viva hasta `plat_info->DiffuseMinR`, **veinte líneas más abajo**. O sea,
**no existe ningún punto donde `envmap_min_b` esté viva y `diffuse_min_r` no**:
toda insn que alarga a una alarga a la otra, y `live(diffuse_min_r)` se va de 442
a 443 justo al revés de lo que hace falta (441).

Medido igual (banco 12 s, base 98,65949 % / 75 filas / 2.044 B):

| # | posición (asm `"=f"(x) : "0"(x)`) | % | filas |
|---|---|---|---|
| u6 | tras las conversiones a `int`, atado a `envmap_power` | 98,571 | 84 |
| u7 | tras los `bClamp`, atado a `envmap_power` | 98,072 | 94 |
| u1 | antes del `switch`, atado a `envmap_max_b` | 95,205 | 109 |
| u4 | en medio de las declaraciones, atado a `diffuse_rng_a` | 95,493 | 102 |
| u3 | antes del `switch`, atado a `envmap_power` | 94,438 | 125 |
| u2 | antes del `switch`, atado a `specular_power` | 94,669 | 172 |
| u5 | tras el `switch`, atado a `envmap_min_r` | 95,705 | 171 (**2.052 B**) |
| u8 | **dos** asm antes del `switch` | 93,663 | 157 |

**La base sigue siendo la mejor de las nueve.** Y encaja con la regla de
estructura del brief: aquí bailan **once** registros, así que empujar uno solo
rompe los otros diez. Dato nuevo del diff: además de la permutación de FPR hay
un **desorden real de cargas del pool** en las filas 333-339 (nos sobra un
`lfs f2, $LC71@l(r10)` donde el objetivo tiene `lfs f2, lbl_803DECFC@l(r11)`),
o sea **no es sólo renombrar registros**.

### 5.2 `TrackCop::Update`: el eje selectivo no puede, y es estructural

La r27 dejó demostrado que quien rompe el orden es `optimize_reg_copy_2` de
`regmove`, que le roba el `REG_DEAD` a la copia `%5 = <pseudo>`. **El asm
selectivo no lo evita**, y la razón es general:

> **Una barrera —selectiva o total— sólo puede AÑADIR restricciones.** Cierra los
> casos donde el objetivo está MÁS atado que nosotros (`SphereVsBox`: le falta
> una dependencia) o donde una prioridad necesita un empujón (`CheckCollideSRB`:
> le falta una referencia). **No puede cerrar un caso donde el objetivo IZA una
> insn por encima de una nuestra**, que es exactamente lo que pasa aquí
> (`mr r5,r3` sube por encima del primer `stfs`).

Ensayos (banco 5 s, base 99,15612 % / 2 filas / 948 B):

| # | forma | % | filas |
|---|---|---|---|
| y1 | `bVector3 *lo1; __asm__("" : "=r"(lo1) : "0"(&look_offset));` y `lo1` de 1.er argumento | 99,156 | 2 (**idéntica**) |
| y2 | igual pero `lo2` de 3.er argumento | 99,105 | 4 |
| y4 | sólo `bScale` detrás de los tres ceros | 99,135 | 7 |
| y3 | `bCross`+`bScale` detrás de los tres ceros | 96,616 | 10 |

Ninguna emite un byte de más (948 B las cuatro). **`y1` es informativa**: el
segundo pseudo se crea sin coste, pero `reload` sigue dándole el mismo registro y
`optimize_reg_copy_2` vuelve a disparar.

---

## 6. `_bOutput` (5.180 B) — traje un ángulo nuevo, y también cae

El brief pedía no tocarlo «salvo que traigas un ángulo nuevo». El ángulo era
éste, y es distinto del que agotó la r28 (que atacaba **la dependencia**):

> La r28 midió que las dos insns empatan en prioridad, en `INSN_REG_WEIGHT` y en
> **número de dependientes (1 cada una)**, y que decide el LUID.
> **Con `INSN_DEPEND` no hacía falta tocar dependencias: bastaba con darle un
> dependiente más a la inserción de PRE (`addi r23,r1,0x10`)**, que es el nivel 4
> de `rank_for_schedule` y va antes que el LUID.

En fuente eso es **nombrar `cvtbuf` en un asm** con la salida atada a `p` (que
está viva). Medido (banco 5 s, base 99,98301 % / 2 filas / 5.180 B):

| # | forma | % | filas | B |
|---|---|---|---|---|
| b1 | `"=r"(p) : "0"(p), "r"(cvtbuf)` tras `desiredPrecision = precision + 1` | 99,402 | 21 | **5.192** |
| b2 | idem delante | 99,402 | 21 | 5.192 |
| b3 | `"r"(&cvtbuf[0])` en vez de `"r"(cvtbuf)` | 99,402 | 21 | 5.192 |
| b5 | idem tras `p -= (precision - 1)` | 99,402 | 21 | 5.192 |
| b6 | dos referencias a `cvtbuf` en el mismo asm | 99,402 | 21 | 5.192 |
| b4 | atado a `desiredPrecision` en vez de a `p` | 99,404 | 29 | 5.192 |

**Veda con su sentencia, y la razón es concreta y nueva**: la dirección de
`cvtbuf` que se nombra desde la fuente **NO es el mismo pseudo que la inserción
de PRE**. GCC materializa una segunda y cuesta **+12 B** en las seis formas.
El `addi r23,r1,0x10` del objetivo aparece **dos veces** (filas 910 y 913), o sea
PRE ya lo rematerializa por su cuenta y no hay forma de engancharse a él desde C.
**Van 93 formas medidas entre las r24…r29.**

---

## 7. Cuántas vedas he reabierto, y con qué resultado

**Cinco vedas de barrera reabiertas** (una por candidata) — todas ellas decían
«probada la barrera en la posición X» refiriéndose a la barrera TOTAL:

| veda | resultado |
|---|---|
| `CheckCollideSRB` (r27: barrera en 4 posiciones; r28: 25 formas) | **ROTA — 100 %** |
| `SetMemoryPoolSize` (r28: «negativo firme, la ranura no se puede mover») | **PARCIALMENTE ROTA**: el orden sí se mueve y a 0 B, pero deja otra insn desplazada (§4) |
| `_bOutput` (r28: 87 formas, techo por LUID) | **CONFIRMADA** con un ángulo nuevo (`INSN_DEPEND` de la inserción de PRE): +12 B |
| `UpdatePlatInfo` (r28: la ventana [316,318] sin atacar) | **CONFIRMADA**, y ahora con la razón: no hay punto donde vivan la una sin la otra |
| `TrackCop::Update` (r27: 10 formas, `optimize_reg_copy_2`) | **CONFIRMADA**, y con una regla general (§5.2) |

**73 variantes medidas**, todas con su cifra: **34** en `CheckCollideSRB`, **21**
en `SetMemoryPoolSize`, **8** en `UpdatePlatInfo`, **4** en `TrackCop` (una quinta
no compila) y **6** en `_bOutput`.

## 8. Lo que NO he probado

- **`CheckCollideSRB`**: no he buscado el constructo de C++ que dé la referencia
  de más a `trig` sin emitir código (es la deuda). No he tocado `HolePunchAvoidables`
  ni `InitAtSegment`, que siguen siendo el muro de zWorld2.
- **`UpdatePlatInfo`**: nada sobre el **desorden real de cargas del pool** de las
  filas 333-339, que no es permutación de registros y que nadie ha mirado; ni
  nada sobre `envmap_max_g` en f31; ni el ciclo de cuatro volátiles.
- **`SetMemoryPoolSize`**: no he encontrado cómo subir `mr r0,r3` una ranura en
  la forma `x4` (haría falta darle prioridad, y una barrera sólo puede retrasar).
  No he mirado si un cambio en `bWare.h`/`CarLoader.hpp` mueve algo.
- **`TrackCop`**: no he probado el permutador ni restricciones de registro; no he
  tocado `TrackCar::Update`, que la r28 dejó con el mismo diagnóstico.
- **`_bOutput`**: no he tocado las cuatro etiquetas de más, ni `int textLen`, ni
  los dos bloques anónimos, ni `GENERIC_FLOAT` (deudas de la r24).
- **El eje selectivo en el resto del árbol**: sólo lo he barrido en las cinco
  candidatas del encargo. Con la regla corregida de §2 (**salida VIVA y atada con
  `"0"`**) quedan sin barrer todas las demás vedas de barrera del proyecto — y la
  palanca `n_refs` de §3.2 **es aplicable a cualquier empate de `allocno_compare`**,
  que es el diagnóstico de media docena de muros del proyecto.
- **El frente §2 del brief (`.rodata` que falta)**: fuera del encargo, sin tocar.

## 9. Convivencia

Scratchpad con prefijo `c29ba_`; arneses nuevos: `c29ba_run.py` (parchea con el
ancla del json, compila con `mn_repro`, da `%`/filas/bytes; 4-12 s por variante),
`c29ba_rows.py` (copia de `c26wag_rows.py` **con el json de salida por PID**: el
original escribe siempre en `c26wag_rows.json` y **con dos agentes a la vez
revienta con `JSONDecodeError`** — mordió una vez), `c29ba_dumpvar.py` (volcado
RTL de una variante) y los `c29ba_*_forms*.json`.

Disco: **16 GB al empezar, 14 GB al acabar**; borrados los 275 MB de volcados RTL
míos (`dump_sw*`, `dump_p1.*`). No he tocado `configure.py`, `config/GOWE69/*` ni
`splits.txt`, no he escrito ensamblador de instrucciones, no he tocado los
`#if defined(__ANDROID__)` y **no he hecho commit**.

`seccdiff.py` sobre zWorld2 al cerrar: `.text +10.404`, `.rodata −272`,
`.data −256`, `.bss +160` — la unidad **no es promocionable** todavía, y lo que
la bloquea es el frente §2 del brief, no el código.
