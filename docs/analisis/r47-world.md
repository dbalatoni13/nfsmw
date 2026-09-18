# r47 — zWorld / zWorld2 / zTrack

Territorio: 10.112 B en 8 funciones. **+0 B de `matched_code`** (ninguna llega al
100 %), pero `HolePunchAvoidables` pasa de **9 a 4 instrucciones distintas** —de
99,919464 % a **99,97315 %**, tamaño 2980/2980— y las 4 que quedan son
**exclusivamente** el empate `cr2`/`cr3`, que esta ronda deja **cuantificado con
una fórmula y con la predicción validada**. Además `InitAtSegment` pierde una
local que no existe en el original **sin mover un byte**.

`fncmp` antes / después, las tres unidades:

| unidad | antes | después |
|---|---|---|
| zWorld | 5 funciones / 5.608 B | **idéntico** (fuente sin tocar, md5 igual a HEAD) |
| zWorld2 | `HolePunch` **9** insn, `InitAtSegment` 15 | **`HolePunch` 4 insn**, `InitAtSegment` 15 |
| zTrack | `GetLoadingPriority` 72 insn | **idéntico** (fuente sin tocar) |

`diff` de los `fncmp` completos: la **única** línea que cambia en las tres
unidades es la de `HolePunchAvoidables` (9 → 4). Ninguna función empeora.
`lcfix.py --check`: «todas las entradas @lc estan al dia». `audit.py` en las
tres: **0 FALLA**.

Ficheros tocados en mi territorio: `WRoadNetwork.cpp` (código: el pin cambia de
sitio y se retira la local `lane`) y `CarRender.cpp` (**sólo comentarios** con la
evidencia nueva de las secciones 6 y 7; `fncmp` de `zWorld` idéntico antes y
después). `CarLoader.cpp` y `TrackStreamer.cpp` siguen byte a byte como en HEAD
(md5 comprobado).

---

## 1. `HolePunchAvoidables` — 9 → 4 filas. **El pin estaba en la variable equivocada**

La r46 dejó un `register float avoidable_delta_offset asm("fr10")` que costaba 5
filas y ahorraba 10. Era un síntoma: la variable que decide es **`offset_change`**,
y quien la decide a ella es **la carga de `cut_to_position.x`**.

Lo aplicado, y es un cambio de UNA línea de fuente (más el pin que se retira):

```cpp
float avoidable_delta_offset = bCross(&avoidable_velocity, ...);   // sin pin
...
register float wld_cutx asm("fr11") = cut_to_position.x;
cut_to_position.x = wld_cutx + offset_change * 0.8f * cookie.Forward.y;
```

Tamaño 2980/2980, y `regmap` pasa de «89 de 89 con el mismo registro» a **89 de
89 iguales y 0 con registro distinto**; la única entrada que sobra es
`wld_cutx`, que es el andamio.

### 1.1 La cadena causal, leída del `.greg` y del fuente de GCC

No es adivinanza: sale de `orig/prodg/NGC_GNU_SRC/NGC/gcc/global.c` y del volcado.

1. `set_preference()` (global.c) trata `(set oc (mult adelta approach))` como
   preferencia: para un `src` cuyo formato empieza por `'e'` hace
   `src = XEXP (src, 0)`, o sea que del `mult` se queda con `adelta`. Si
   `reg_renumber[oc] >= 0` (y lo está, porque `offset_change` es una cantidad
   **local**), marca `hard_reg_preferences[allocno(adelta)] |= reg(oc)`.
   **`avoidable_delta_offset` SIGUE a `offset_change`**: pinchar la primera era
   tratar el síntoma, y por eso el pin costaba las cinco filas del `bCross`.
2. `offset_change` la reparte `local_alloc`, y `find_free_reg` recorre
   `REG_ALLOC_ORDER`, que en la parte FP de `rs6000.h:932` es
   **f0, f13, f12, f11, f10, f9 …**: coge f11 salvo que f11 esté ocupado en su
   rango. En el objetivo lo ocupa la carga de `cut_to_position.x`
   (fila 567, `lfs f11,0x188(r1)`), cuya cantidad se reparte antes por tener más
   prioridad en `qty_compare_1` (local-alloc.c:1568). Fijándola a `fr11` se
   reproduce eso, `offset_change` cae en **f10** y `avoidable_delta_offset` cae
   detrás en el mismo f10 — coalescida con el operando moribundo, igual que el
   objetivo: `fmsubs f10,f0,f9,f10`.

Sin el pin de r46 los cinco temporales del `bCross` ya casaban EXACTOS
(f10/f9/f0); lo único que fallaba era el destino, y el destino lo arrastraba
`offset_change`.

### 1.2 MEDIDO Y NEGATIVO (todo 2980/2980 salvo donde se dice)

| ensayo | filas |
|---|---:|
| base de r46 (pin `fr10` en `avoidable_delta_offset`) | 9 |
| sin ningún pin | 15 |
| **`fr11` en `cut_to_position.x`, sin el pin `fr10`** | **4** |
| `fr11` en `cut_to_position.x` **más** el pin `fr10` | 9 |
| pin `fr10` en `offset_change` (con el `fr10` de r46) | 44 |
| pin `fr10` en `offset_change` sin nada más, sin barrera | 49 |
| pin `fr10` en `offset_change` además del `fr11` | 41 |
| cantidad fantasma en `fr11` alrededor de `offset_change` | **228** |
| pin de `extra_width` a `fr2` (volátil) sin barrera | 45 |
| pin de `extra_width` a `fr2` con barrera | 19 |

La cantidad fantasma en un FPR de trabajo es catastrófica: f11 se queda bloqueado
en todo el rango que abarca la pareja de `asm` y arrastra 224 filas. **La receta
de [[nfsmw-cantidad-fantasma]] no vale para los FPR de trabajo (f0..f13)**; el
equivalente correcto ahí es el pin de una carga que ya existe, como este `fr11`.

---

## 2. Las 4 filas que quedan: el empate `cr2`/`cr3`, con fórmula

`allocno_compare` (global.c:626) ordena por

    pri = (int) (floor_log2(n_refs) * n_refs / live_length * 10000 * size)

y **desempata por número de allocno**, que es el orden de pseudo, que es el orden
en que aparece el test en el cuerpo del bucle. `is_racer` se prueba antes que
`is_drag` en la fuente, así que el desempate nunca se puede invertir desde ahí.

Del `.greg` de la base actual (n_refs = 3 y size = 1 en las tres, o sea
`pri = int(30000/live_length)`):

| pseudo | variable | live | pri | rango | nuestro | objetivo |
|---|---|---:|---:|---:|---|---|
| 505 | `is_traffic` | 483 | **62** | 166 | cr4 | cr4 |
| 334 | `is_racer` | 494 | **60** | 171 | cr3 | **cr2** |
| 901 | `is_drag` | 492 | **60** | 172 | cr2 | **cr3** |

30000/492 = 60,97 y 30000/494 = 60,72: **los dos truncan a 60**, empatan, y gana
el pseudo menor. El objetivo necesita el orden de reparto 505, 901, 334 (el
`REG_ALLOC_ORDER` de los CR preservados es cr4, cr3, cr2).

`live_length` se cuenta en flow.c:3405 y flow.c:2908, **un incremento por cada
insn de clase `'i'`**: un `asm` de cero bytes cuenta igual que una instrucción.
Y las tres comparaciones están izadas al preencabezado con `is_racer` **2 insns
por delante** de `is_drag` (entre las dos sólo está la carga del literal 0,2f),
así que `live(334) = live(901) + 2` siempre.

**Predicción validada**: si se quita `asm("" : "+f"(offset_change));` (un insn de
cero bytes dentro del bucle, o sea dentro del rango de vida de las tres), sale
`live(505)=482 → 62`, `live(901)=491 → 61`, `live(334)=493 → 60`, y el `.greg`
da **505→cr4, 901→cr3, 334→cr2, EXACTO**. Las cuatro filas desaparecen. Pero esa
barrera es la que impide que `sched` adelante el `fmuls` de `extra_width`, y sin
ella `regmap` da tres locales mal (`avoidable_delta_offset` f12, `offset_change`
f13, `extra_width` f30 contra f10/f10/f2): **45 filas**.

**Las únicas longitudes que voltean el empate** (con L334 = L901 + 2) son
L901 ∈ {482, 483, 490, 491, 499, 500}. Con la barrera puesta L901 = 492, así que
sólo valen **+7 u +8 insns más de cero bytes** dentro del bucle. Medido:

| ensayo | resultado |
|---|---|
| la barrera repetida ×7 / ×8 / ×9 | 62 filas / **2976 B** / 95 filas |
| 7 barreras `+f` sobre 7 flotantes distintos (avoidable_d, my_d, avoidable_ahead, avoidable_offset, right_projection, left_projection, extra_width) | 40 filas |
| 1 barrera `+f(dist_to_tail)` en el bucle interior | 10 filas |
| mover `extra_width` detrás de `cookie_to_avoidable` / `cookie_to_me` / `my_d` / `avoidable_ahead` | objeto **IDÉNTICO** en las cuatro |
| mover `hole_punch_safety_margin` detrás de `close_factor` / `avoidable_half_width` / delante de `approach_time *=` / detrás de `gap_right` | 48 / **+8 B** / **+8 B** / 56 |
| `asm("fmuls %0,%1,%2" : "=f"(zp), "+f"(offset_change) : "f"(zk))` en vez de `cut_to_position.z -=`, **con** barrera | **4 filas, objeto idéntico** |
| … la misma, **sin** barrera (para que el `asm` con cuerpo haga de ancla) | 53 filas |
| `asm("fsubs …")` emitiendo el `fsubs` de `cut_to_position.z`, con / sin barrera | 55 / 59 filas |

**Lo que falta es UN insn menos en el cuerpo del bucle sin perder el ancla del
`fmuls`.** Un `asm` **con cuerpo** que emita el `fmuls` sí es byte-neutro (la
variante «con barrera» da objeto idéntico), pero su `"+f"(offset_change)` **no
ancla**: hay que dejar la barrera aparte, y entonces el insn sigue ahí.

Regla generalizable, y es nueva: **mover sentencias dentro del mismo bloque
básico es neutro, pero mover una sentencia con un ternario NO lo es** —parte el
bloque— y aquí cuesta entre 44 filas y 8 bytes.

---

## 3. `InitAtSegment` (zWorld2, 816 B) — fuera la local que no existe

`regmap` daba `laneInd` en r5 contra r24 del original y **dos locales sólo
nuestras** (`lane` en r24 y el `guard` fantasma de r36d). La `lane` no existe en
el original: su `laneInd` vive directamente en r24. Quitándola y usando `laneInd`
a pelo el objeto sale **IDÉNTICO** (15 filas, 816/816) y `regmap` pasa a **10 de
10 iguales**; sólo queda `endOffset` (al que el original no da registro) y el
`guard`, que sigue siendo load-bearing (**quitarlo: 21 filas**, remedido esta
ronda).

Aplicado: cuesta 0 bytes, borra un falso positivo del triaje y deja el volcado
DWARF alineado con el original para la próxima ronda. Las 15 filas que quedan son
un ciclo r10↔r11 (más r9↔r10 y r25 en el prólogo), no una local.

---

## 4. Auditoría de `bCross` — mi territorio está LIMPIO, y hay un atajo

Las **18** llamadas a `bCross(` de mi territorio (17 en `WRoadNetwork.cpp`, 1 en
`CarRender.cpp`) están **todas** con el orden correcto:

| función | llamadas | prueba |
|---|---:|---|
| `WRoadNav::UpdateCookieTrail` | 2 | byte-idéntica |
| `WRoadNav::CookieCutter` | 1 | byte-idéntica |
| `WRoadNav::UpdateOccludedPosition` | 8 | byte-idéntica |
| `WRoadNav::IsPointInCookieTrail` | 1 | byte-idéntica |
| `WRoadNav::HolePunchAvoidables` | 5 | 4 filas, y las 4 son `cmpwi`/`bc` |
| `CarRenderInfo::CarRenderInfo(RideInfo*)` | 1 | byte-idéntica |

**El atajo**: una llamada a `bCross` dentro de una función byte-idéntica es
correcta **por construcción**, y una dentro de un near-miss cuyas diferencias no
tocan ningún `fmuls`/`fmsubs` también. Sólo hay que leer el asm de las llamadas
que están en funciones con diferencias FP. De las 35 del árbol eso deja muy
pocas: **las de `zCamera` (`CameraMover`, `Cubic`, `Showcase`, `TrackCar`,
`TrackCop`), `eMathE.cpp` de `zEcstasy` y `Gps.cpp`**. Es un `fncmp` por unidad,
no 35 lecturas.

---

## 5. `RenderFlaresOnCar` (zWorld, 2.908 B) — diagnóstico afinado, sin ensayos nuevos

18 filas y **+4 B**, sin ensayos nuevos: el diagnóstico de r17/r46 escrito en la
fuente se confirma instrucción a instrucción y no ha aparecido evidencia nueva.
El objetivo materializa **un solo** `@ha` de `lbl_8040AD04` (`const float` = 0,0f,
`extern "C"`), en el **bloque de entrada** (fila 123, `CarRender.cpp:4061` del
original según `lmap`), lo guarda en r16 y lo mantiene vivo hasta la fila 601.
Nosotros no creamos ninguno ahí y materializamos **dos** más abajo (fila 452 en
el volátil r9 y fila 461 en r30): de ahí los 4 B, y de ahí el corrimiento
`flashHeadlights` r14→r16, `renderFlareFlags` r17→r16 y `force_light_state`
r30→r17 que señala `regmap` (40 de 43 locales iguales, 0 de más y 0 de menos).

Las 18 filas son exactamente eso: 1 `lis` que falta arriba, 3 que sobran abajo
(452/454/455 INSERT, 458/461 DELETE) y 12 de renombrado de esos tres registros.
Vetas de r46 remedidas como vigentes: la referencia sintética a `lbl_8040AD04`
antes del bucle da el tamaño exacto 2908/2908 pero sube a **62 filas**, en
`headlight` o en `brakelight` da el MISMO objeto, y el pin de `flashHeadlights`
a r14 encima sube a **81**. La palanca no es cuántos `lis` hay sino **dónde nace
el pseudo**, y el sitio es el bloque de entrada — no el preencabezado del bucle,
que es a donde va la referencia sintética.

---

## 6. `UpdateWheelYRenderOffset` (zWorld, 876 B) — la causa, con el censo

7 filas y **−4 B**. El censo de registros de las dos columnas del `fndiff` cierra
el diagnóstico: **el objetivo tiene DOS pseudos `@ha` del literal 0,0f
(`lbl_8040AA84`) izados al preencabezado, r19 y r16**, para tres cargas
(filas 42, 169 y 191); nosotros creamos **uno solo** (r19, compartido por CSE) y
con el preservado que sobra izamos `TweakKitWheelOffsetRear@ha` a r16 — que es
justo el `lis` de menos: el objetivo lo rematerializa DENTRO del bucle, en el
volátil r9 (filas 97/98).

Uso de registros, `fndiff` columna a columna: idéntico en r0..r31 salvo
**r9 (obj 26 / nue 24)** y **r19 (obj 3 / nue 4)**. O sea que no falta presión
—el banco r14..r31 está entero en los dos— sino que **le sobra a CSE una
fusión**: hay que impedir que comparta el `@ha` de 0,0f entre la carga de antes
del bucle y la primera de dentro. La veda de r36e («la cantidad fantasma no tiene
dónde ponerse») sigue vigente y ahora se entiende: el frente no es la presión,
es la fusión de CSE.

---

## 7. `CullParts` (zWorld, 836 B) — leído entero, 5 filas y −4 B

El objetivo:

```
95  lwz   r0, 0x8(r30)     ; Polarity en r0 (volátil, muere por el camino corto)
96  li    r9, 0x0          ; debug_print = 0
97  cmpwi r0, 0x0
99  lwz   r0, 0xc(r30)     ; NumPlanes
101 cmpw  r9, r0           ; la guarda compara el 0 de debug_print
102 mr    r9, r0           ; <-- el insn de más: debug_print = NumPlanes, IZADO
103 bge
```

Nosotros dejamos `Polarity` en r11, reusamos ese r11 como fuente del 0 en la
guarda (CSE sabe que vale 0 tras el `beq`) y no izamos el `mr`. Son dos cosas
distintas y las dos las decide CSE, no el asignador: `record_jump_equiv` mete
`r11 == 0` en la clase de equivalencia y `fold_rtx` elige r11 donde el objetivo
elige r9. Las vedas de r36d (pin de `polarity` a r0 → 9 difs; fantasma en r11 →
9 difs; `debug_print` inicializado a `NumPlanes` → 824 B; asignado también antes
del bucle → 828 B) siguen vigentes; no se han repetido.

---

## 8. `GetLoadingPriority` (zTrack, 708 B) — EVIDENCIA NUEVA del volcado del original

Se queda la base de HEAD, como manda el brief. Pero al releer el DWARF del
original (`symbols/mw_dwarfdump.nothpp:2177094`) sale un dato que r46 no dejó
escrito **literal**, y que no cuadra con nuestras cabeceras:

```
/* anonymous block */ {
    char layer_name[32];         // r1+0x8
    struct FloatVector pos;      // r1+0x28
    struct FloatVector face[4];  // r1+0x38     <-- exterior
    ...
        /* anonymous block */ {
            struct bVector2 * point1;
            struct bVector2 * point2;
            struct FloatVector face[4];  // r1+0x48   <-- interior
```

Entre el `face[4]` exterior y el interior el original deja **0x10 B**. Con
nuestro `FloatVector` de 12 B un `FloatVector[4]` ocupa 0x30, así que los dos
arrays **se solaparían**, cosa que GCC 2.95 no hace con variables declaradas de
bloque (`expand_decl` → `assign_stack_local`, sin reúso por vida). Las dos
lecturas posibles son:

1. **`sizeof(FloatVector)` no es 12 en el original** (con 4 B cuadran los dos
   arrays: 0x38..0x48 y 0x48..0x58; `pos` en 0x28 ocuparía 0x10 por alineación), o
2. el bloque tiene, además de las cuatro variables, **0x50 B de temporales** que
   el DWARF no nombra, y el volcado de una de las dos entradas está desalineado.

El bloque entero le ocupa al original 0xA0 (0x8..0xA8, donde arranca el `dest` de
la inline `bScale`) y a nosotros 0x90 = 32+16+48+48 exacto. Faltan **16 B**, y de
las 72 filas **37 son sólo el desplazamiento mecánico de +0x10 del marco**. Sigue
en pie la prohibición de rellenarlo con una local inventada (precedente
`9de121ee`, y r46 midió que un `pos2` fabricado baja de 71 a 28 filas). Pero
**`sizeof(FloatVector)` es una hipótesis que se comprueba en un `grep`, no una
local inventada** — ver la propuesta 3.

---

## 9. `DefragmentPool` y `SetMemoryPoolSize` (zWorld) — sin ensayos nuevos

`DefragmentPool` (684 B, 23 filas): `regmap` sigue dando tres locales sólo
nuestras (`params`, `table`, `zero`) y r46 midió que retirarlas cuesta 47 filas y
+8 B (sueltas 29 / 23 / 43). Son andamio deliberado. Lo que queda son tres ciclos
de registro (r21↔r22, r16→r18→r17→r16, r25↔r27) y **`loop_number`, que el
original tiene en `.data` (@0x80438904) y nosotros en otra dirección**: eso es
dato, no código.

`SetMemoryPoolSize` (304 B, 2 filas): las dos filas son **un solo intercambio de
orden** entre `stw r0, CarLoaderMemoryPoolNumber@l(r29)` y
`addi r6, r6, "Cars"@l` justo antes de `bl bInitMemoryPool`. Es `sched2`:
`rank_for_schedule` (sched.c) desempata por prioridad, luego por clase respecto
al último insn programado y por último por `INSN_LUID` (orden original). El
`addi` depende del `lis` inmediatamente anterior (clase 1) y el store no (clase
3), y la clase más alta va primero — de ahí nuestro orden. Para invertirlo hace
falta que el `addi` tenga MÁS prioridad que el store, o que su LUID sea menor
(que la dirección de «Cars» se materialice antes de la asignación: medido en
r36e, **308 B**). Las diez vedas de r36e siguen vigentes.

---

## 10. Propuestas fuera de territorio

1. **`bCross` — auditar por unidad, no por llamada.** Sólo hay que leer las que
   caen en funciones con diferencias FP. En mi territorio las 18 salen limpias
   sin leer una sola: 12 están en funciones byte-idénticas y 5 en un near-miss
   cuyas 4 diferencias son `cmpwi`/`bc`. Para el resto del árbol basta cruzar la
   lista de `grep -n "bCross("` con el `fncmp` de cada unidad. Quedan de verdad
   por leer las de `zCamera` (`CameraMover`, `Cubic`, `Showcase`, `TrackCar`,
   `TrackCop`), `eMathE.cpp` y `Gps.cpp`.

2. **Regla nueva del catálogo — `set_preference` encadena las locales.** Cuando
   `regmap` señale dos locales seguidas con el registro cambiado y una alimente a
   la otra por un `mult`/`plus`, **la que manda es la SEGUNDA**: `set_preference`
   (global.c) hace `src = XEXP(src,0)` para cualquier `src` de formato `'e'`, así
   que el allocno de la primera hereda la preferencia del registro que
   `local_alloc` le dio a la segunda. Pinchar la primera es tratar el síntoma
   (aquí costaba 5 filas). Y **para forzar el registro de una cantidad LOCAL no
   se usa la cantidad fantasma sino el pin de una carga que ya existe**: hay que
   ocupar el registro que `REG_ALLOC_ORDER` daría antes (en FP el orden es f0,
   f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, f31…f14).

3. **Comprobar `sizeof(FloatVector)`** (`src/Speed/Indep/bWare/Inc/…`) contra el
   volcado DWARF del original: el `face[4]` exterior de `GetLoadingPriority` y el
   interior están a 0x10 B en el original y a 0x30 en el nuestro. Si el tipo mide
   4 B (o si lleva alineación) cuadran los dos arrays **y** los 16 B que le
   faltan al marco, y caen las 37 filas mecánicas de la función sin inventar
   ninguna local. Es cabecera compartida: no la he tocado. Entra directamente en
   [[nfsmw-cabeceras-que-mienten]].

4. **La cantidad fantasma NO vale para los FPR de trabajo.** Medido aquí: la
   pareja `register float g asm("fr11"); asm("":"=f"(g)); … asm("":"+f"(x):"f"(g));`
   sube `HolePunchAvoidables` de 15 a **228 filas**. Bloquea f11 en todo el rango
   entre los dos `asm` y el rango lo fija `sched`, no la fuente. En los enteros
   preservados (r19 en `InitAtSegment`) sigue funcionando.

5. **Un `asm` con cuerpo NO sirve de barrera de programación.** El
   `asm("fmuls %0,%1,%2" : "=f"(t), "+f"(x) : "f"(k))` emite exactamente la
   instrucción del objetivo (objeto idéntico con la barrera puesta) pero su
   `"+f"` no impide que `sched` adelante un consumidor posterior de `x`: hacen
   falta las dos cosas, y entonces el `asm` de cero bytes sigue contando como
   insn. Útil para saberlo antes de intentar «fundir la barrera en una
   instrucción real».

---

## 11. Artefactos

`scratchpad/r47_world/`:

- `base_fncmp_zWorld*.txt`, `base_fncmp_zTrack.txt` y `final_fncmp_*.txt` (el
  antes/después de la verificación).
- `hpa_base.txt` (9 filas), `hpa_nopin.txt` (15), `hpa_x5_final.txt` (4),
  `ias_base.txt`, `cull_base.txt`, `uwy_base.txt`, `rf_base.txt`, `rf_lmap.txt`,
  `smps_base.txt`.
- `rtl/hpa_x5.greg`, `rtl/hpa_x5.lreg`, `rtl/hpa_x7.greg`, `rtl/hpa_nopin.*`
  (los volcados de los que salen las tablas de `live_length` y de reparto).
- `WRoadNetwork.cpp.HEAD` / `.x5` / `.r47` y las copias de las otras tres
  fuentes, todas sin tocar.
- `try.py`, `tryias.py`, `tryhps.py`, `tryew.py`, `tryasm.py`, `cc.py`, `ed.py`
  (los bancos de variantes de cada eje).

Herramientas nuevas, en `scripts/`:

- **`agent_wld_rtl.py`** — saca `.greg`/`.lreg`/`.combine`… de **cualquier**
  unidad o de un `.cpp` suelto, con los cflags reales sacados de `build.ninja`:
  `python scripts/agent_wld_rtl.py <unidad> <filtro> -dg -dl`.
- **`agent_wld_m.py`** / **`agent_wld_m2.py`** — compilan y sacan las filas
  distintas de una función o de la unidad entera en una sola línea.
