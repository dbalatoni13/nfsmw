# R48 — `zEcstasy` + `zEagl4Anim`

## Resultado

**Cero funciones cerradas, cero bytes.** Las dos unidades entran y salen byte a
byte iguales (4 fn / 5.180 B y 2 fn / 2.808 B). Lo que sí sale de esta ronda es
**el mecanismo exacto de las 27 filas de `UpdatePlatInfo`, leído en `global.c`**,
y una palanca de una línea que baja la función **de 28 filas a 8** y le da **el
reparto de registros del original** — no aplicada, y con la prueba de por qué.

| unidad | entrada | salida |
|---|---|---|
| `zEcstasy` | 4 fn / 5.180 B | 4 fn / 5.180 B (idénticas) |
| `zEagl4Anim` | 2 fn / 2.808 B | 2 fn / 2.808 B (idénticas) |

Tres ficheros tocados, **sólo comentarios** (80 líneas, todas `//`, 0
supresiones): `eLightE.cpp`, `eMathE.cpp`, `RawStateChan.cpp`.

---

## 1. `UpdatePlatInfo` (2.044 B) — el ciclo de tres, resuelto como diagnóstico

### 1.1 No es prioridad, ni `call_used_regs`, ni el marco: es `regs_someone_prefers`

La r47 dejó el `.greg` sacado y el ciclo acotado a tres pseudos. Lo que faltaba
era **por qué** el 115 se lleva f1. Está leído en `global.c` y comprobado
instrucción a instrucción:

```text
pseudo 115 = diffuse_rng_a   pos 99   n_refs 10 / len 358  prio 837  -> f1   (objetivo f5)
pseudo 191 = envmap_min_g    pos 108  n_refs  7 / len 317  prio 441  -> f31  (objetivo f1)
pseudo 202 = envmap_max_g    pos 109  n_refs  7 / len 328  prio 426  -> f5   (objetivo f31)
```

Conflictos DUROS del 115 (cola numérica de su fila de `;; N conflicts:`):
`0 1 8 9 10 11 32 41 42 43 44 45` = r0, r1, r8..r11, **f0, f9, f10, f11, f12,
f13**. Cuando le toca el turno (posición 99 de 131), los conflictos acumulados
por los allocnos ya asignados son f7 (99), f6 (101 y 114), f4 (106), f3 (110) y
f2 (165) — **f8 y f5 están libres**. Con `REG_ALLOC_ORDER` de rs6000
(`f0, f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3, f2, f1, f31, …`) la
**pasada 1** de `find_reg` le daría **f8**.

Le da f1 porque gana la **pasada 0**, que además de los conflictos excluye
`regs_someone_prefers[115]`. Y el `.greg` imprime quién prefiere qué:

```text
;; 194 preferences: 37      <- envmap_min_b prefiere f5
;; 205 preferences: 40      <- envmap_max_b prefiere f8
```

`prune_preferences` (global.c:888) mete en `regs_someone_prefers[A]` las
preferencias de **todo allocno de MENOR prioridad que choque con A** —194 (prio
314) y 205 (prio 303) contra los 837 del 115— **salvo las que A también
prefiera** (`AND_COMPL_HARD_REG_SET(temp, hard_reg_full_preferences[A])`, que
sólo se aplica porque `allocno_size` es igual). El 115 no prefiere nada, así que
pierde f8 **y f5**, cae en f1, y de ahí sale el ciclo entero: el 191 encuentra
f1 ocupado y se va a f31, y el 202 a f5.

### 1.2 De dónde sale la preferencia del 194 (la palanca)

`set_preference` (global.c:1551) hace `src = XEXP (src, 0)` para un `src` de
formato `'e'`. De `(set (reg 194) (mult (reg f5) (reg X)))` el que manda es el
**PRIMER operando**; como `envmap_min_scale` está pinchado en `fr5`, el 194
hereda la preferencia por f5. El 191 y el 202 tienen la misma preferencia pero
**PODADA**, porque sí chocan con f5 / f8 (no son el último uso de la escala).

Esto es la lección de la r47 («`set_preference` encadena locales y manda la
SEGUNDA») en su forma más pura: **el pseudo que envenena al 115 no es ninguno de
los tres del ciclo**.

### 1.3 La palanca de una línea, medida y NO aplicada

```diff
-    float envmap_min_b = envmap_min_scale * material_data->EnvmapMinB;
+    float envmap_min_b = material_data->EnvmapMinB * envmap_min_scale;
```

| | % | filas (objdiff) | tamaño | 115 | 191 | 202 |
|---|---:|---:|---:|---|---|---|
| base | 99,33464 | 28 | 2.044 | f1 | f31 | f5 |
| **minb conmutada** | **99,540115** | **8** | 2.044 | **f5** | **f1** | **f31** |

Con ella `regmap` da IDÉNTICO también en el objeto (las 54 locales con su
registro, `diffuse_rng_a f5`, `envmap_min_g f1`, `envmap_max_g f31`, que es lo
que dice el DWARF del original).

**No aplicada.** El objetivo emite `fmuls f26,f5,f10` y la forma conmutada emite
`fmuls f26,f10,f5`: está **demostrado que no es la fuente original** (esa es una
de las 8 filas que quedan). Dejarla sería escribir a sabiendas una forma falsa a
cambio de 0 bytes.

### 1.4 Barrido para matar la preferencia SIN conmutar — 30 medidas, todas negativas

Banco nuevo de **1,4 s por variante** (`scratchpad/r48_ecs/g.py` + `gold.py` +
`sweep.py`: compila `eLightE.cpp` suelto con `cc1plus -dg`, lee `reg_renumber`
del `.greg` y cuenta filas contra el ORIGINAL canonizando las dos sintaxis).
Base 27 filas en esa métrica, `minb conmutada` 7.

| forma | filas | 115 / 191 / 202 |
|---|---:|---|
| pin `envmap_min_b asm("fr26")` | 67 | f5 / f1 / **f8** |
| ídem + `maxscale asm("fr8")` / `maxb fr8` / `maxg fr31` / `maxr fr25` | 452-453 | pierde 2 insn |
| emitir el `fmuls` desde un `asm` (min_b) | 54 | f5 / f1 / **f8** |
| ídem sobre min_g | 18 | **f5 / f1 / f31** (¡correctos!) |
| `asm("" : "+f"(envmap_min_b) : "f"(envmap_min_scale))` | 83 | f7 / f5 / f1 |
| `asm("" : : "f"(envmap_min_scale))` tras min_b / tras max_scale | 493 / 493 | f6 / f3 / f5 |
| `asm("" : "+f"(envmap_min_scale))` tras min_b | 27 | idéntico (lo borra `flow`: salida muerta) |
| sin el pin `fr5` | — | 115 f1, 191 f12, 202 f9 |
| pin del min_scale en fr3 / fr4 / fr6 | 27 | el 202 sigue el pin |
| min_r o min_g conmutadas (sueltas) | 27 | sin efecto (no son el último uso) |
| las tres conmutadas | 10 | correctos, pero 2 filas de más |
| temporal intermedio / `*=` / `(float)` / doble / doble negación | 27-511 | — |
| `envmap_min_b` antes de `envmap_min_g` | 29 | mueve el problema al 191 |

Lo que cierra el frente teórico: **cualquier forma en la que `envmap_min_scale`
sea el primer operando y esté asignada por `local_alloc` produce la preferencia**
(`set_preference` la crea siempre). Para el original sólo quedan dos salidas, y
las dos son de fuente: que el 115 **también** prefiera f5 (haría falta que uno
de sus 8 `set` tuviera como primer operando algo que viva en f5 — sólo
`envmap_min_scale`, imposible sin cambiar la semántica), o que el 194 **choque**
con f5 (haría falta un uso de la escala POSTERIOR a `envmap_min_b`, y todo lo que
lo alarga alcanza también al 115, que está vivo ahí).

### 1.5 Las 6 filas restantes: el bloque de literales, cerrado con el volcado del planificador

`case 0x68E97F75 … 0xDD90E320` (`envmap_power = 6.0f; diffuse_min_a = 0.22f;
diffuse_rng_a = 0.86f;`). Con `-dS -fsched-verbose-5` (bloque **b73**):

```text
;;      insn  code  bb  dep  prio  cost  units
;;      508   506   0    0     3     1   iu2      : 525 511      (lis 6.0f)
;;      511   523   0    1     2     2   lsu      : 525          (lfs 6.0f)
;;      514   506   0    0     3     1   iu2      : 525 517      (lis 0.22f)
;;      517   523   0    1     2     2   lsu      : 525
;;      520   506   0    0     3     1   iu2      : 525 523      (lis 0.86f)
;;      523   523   0    1     2     2   lsu      : 525
;;      525   767   0    6     1     1   bpu
;;  Ready list (t = 1):  520 514 508  --> 508 en iu2, 514 en iu2   <-- DOS unidades iu2
;;  Ready list (t = 2):  517 511 520  --> 520 en iu2, 511 en lsu
```

Sale `lis,lis,lis,lfs,lfs,lfs` (3 GPR de dirección: r9, r11, r10). El objetivo
emite `lis,lfs,lis,lis,lfs,lfs` y **reutiliza r11**, o sea que en su `sched1`
sólo entró UNA `lis` en t=1. Con `prio(lis)=3 > prio(lfs)=2` y **dos ranuras
`iu2`**, ninguna forma de fuente lo toca: las **8 variantes** medidas (las 6
permutaciones de las tres sentencias, 9 y 0 líneas en blanco, y meter la primera
en un bloque `{}`) dan el **objeto IDÉNTICO**. El mismo compilador sí produce la
forma del objetivo en el otro bloque de tres literales (`case 0x2388DD82`,
`.L_8010A108`), que tiene más insns y llena las ranuras de t=1.

---

## 2. `eProject` (268 B, 13 filas) — los números de `local_alloc`, por fin

`QTY_CMP_PRI` (local-alloc.c:1568) es
`floor_log2(n_refs) * n_refs * size / (death - birth) * 10000`. Del `.lreg` de
`eMathE.cpp` compilado suelto:

```text
107 clipX     2 refs /  3 insns  ->  6666   -> f0    (objetivo f13)
137 halfVP2   5 refs /  8 insns  -> 12500   -> f13   (objetivo f0)
143 halfVP3   3 refs /  5 insns  ->  6000   -> f12   (objetivo f13)
```

**La prioridad no explica el ciclo (a)**: `halfVP2` ya la tiene más alta que
`clipX` y aun así se lleva f13. Lo que sí lo mueve es el número de barreras
(`__asm__("" : "+f"(halfVP2))`, la que ya está en el fuente desde la r36d):

| barreras | filas | 137 (refs/len, prio) | 107 | 137 | 143 |
|---|---:|---|---|---|---|
| 0 | 17 | 3/6, 5000 | f0 | f13 | f12 |
| **1 (hoy)** | **13** | 5/8, 12500 | f0 | f13 | f12 |
| 2 | 25 | 7/11, 12727 | **f13** | **f0** | f12 |
| 3 | 31 | 9/13, 20769 | f13 | f0 | f12 |
| 5 | 20 | 13/15, 26000 | f13 | f0 | f12 |

Con **dos** barreras el ciclo (a) casa exactamente (`clipX` f13, `halfVP2` f0,
que es el objetivo) pero el resto del reparto se desmonta y sube a 25 filas. Ése
es el único eje vivo que le queda a esta función, y es nuevo: las 200+ medidas de
r30/r36d/r36f atacaban el reparto sin este número.

**Negativo nuevo y útil para todo el árbol**: la forma
`__asm__("" : : "f"(x))` de **sólo entrada** del brief de la r47 **aquí SÍ emite**
(67 → 68 insns), porque *un `asm` sin operandos de salida es volátil por
definición*. Las cinco colocaciones probadas dan 47-51 filas. La regla del brief
(«no emite un byte») vale cuando GCC puede absorberlo en una copia que ya
existía, no en general.

---

## 3. `EvalState` (456 B, 52 filas) — `regmap` le pone nombre

```text
python scripts/regmap.py zEagl4Anim "FnRawStateChan::EvalState" --all
  k        original r4   nuestro -     <-- DISTINTO
  keyIdx   original -    nuestro r10   <-- SOLO NUESTRA
  VEREDICTO: NO es una permutacion: 1 local solo nuestra.
```

El original **no tiene** la local `keyIdx`, y la primera divergencia es de
**sitio**: el objetivo carga `mKeyIdx` en la fila 19 —el bloque que hay después
del `if (mNumFields & 1)` de `GetKeyData`, que domina las dos ramas y por eso le
sirve al `mr r31,r11` del `then` (fila 29) y al `subic. r31,r11,1` del `else`
(fila 69)— y nosotros en la fila 7, en el bloque de entrada.

Medido esta ronda (base 93,50877 % / 43 filas / 456 B):

| forma | % | filas | tamaño |
|---|---:|---:|---:|
| `i = mKeyIdx;` antes del `if`, `else i-1` | 83,41228 | 43 | **452** |
| ídem con `for (i--; …)` | 83,41228 | 43 | 452 |
| `mKeyIdx` directo en las tres lecturas | 89,86842 | 46 | **460** (TRES `lwz`) |
| `i = mKeyIdx` antes / cond `i` / `else mKeyIdx-1` | 82,14035 | 51 | 456 |
| `k = c->GetKeyData(mKeyIdx)` fuera del `if` | 89,86842 | 46 | 460 |
| `keyIdx` asignado DENTRO del argumento (`GetKeyData(keyIdx = mKeyIdx)`) | 93,50877 | 43 | 456 (objeto IDÉNTICO) |
| `asm("")`, `asm("" : "+r"(keyIdx))`, `asm("" : : "r"(keyIdx))` en 5 sitios | 93,50877 | 43 | 456 (IDÉNTICOS) |

La carga se sube de bloque **aunque la lectura esté escrita dentro del
argumento**: es movimiento INTERBLOQUE de haifa (el bloque del multiplicando
post-domina al de entrada, así que no es especulativo y el planificador lo
adelanta sin coste). Y los `asm` no lo frenan porque GCC los absorbe en la copia
que ya había. Quitar la local sin más pierde una instrucción (452 B); dejar las
tres lecturas del miembro gana una (460 B). **El original está en medio: una sola
carga, colocada en el bloque intermedio.**

---

## 4. Frentes que siguen cerrados (remedidos o respetados)

- **`DynamicLoader::Initialize`** (2.352 B, 4 B): la veda de la r47
  (`jump.c:1831`, `JUMP_LABEL(range1end) == label2` inalcanzable con esta lista
  de `case`) no se ha reabierto. Sin medidas nuevas.
- **`epCalculateLocalDirectionalPOS16`** (2.072 B): sin medidas nuevas; la única
  hipótesis viva sigue siendo `secondary_memlocs_elim[]` de `reload`.
- **`GenerateHorizonFogDisplayList`** (796 B, 2 filas): confirmado desde el
  fuente de haifa que `rank_for_schedule` (haifa-sched.c:4168) mira
  `INSN_PRIORITY` antes que nada y que `INSN_REG_WEIGHT` está detrás **y además
  se salta con `reload_completed`** —o sea que en `sched2`, que es donde se
  decide esta transposición, ese nivel **no existe**—. Con prio(`srawi`)=12 y
  prio(`andi.`)=2 no hay desempate que ganar. Sin medidas nuevas.

---

## 5. Propuestas fuera de territorio

1. **`regs_someone_prefers` como diagnóstico de barrido.** Cualquier near-miss
   cuyo `.greg` diga «el registro que quiere el objetivo estaba LIBRE cuando le
   tocó el turno» es esto, y se lee en dos líneas del volcado: buscar
   `;; N preferences: R` con `R` = el registro del objetivo y `N` un allocno de
   **menor prioridad** que choque con el nuestro. El culpable **nunca aparece en
   el diff**. Merece una columna en `triaje.py` o un script propio
   (`scripts/greg.py`, hoy en `scratchpad/r48_ecs/greg.py`).
2. **Corregir el brief sobre `asm("" : : "r"(x))`.** «De sólo entrada no emite un
   byte» **no es general**: un `asm` sin salidas es volátil, y en `eProject` sube
   de 67 a 68 insns. Lo que no emite es el `asm` que GCC puede absorber en una
   copia preexistente (variable de un solo uso), sea de entrada o de salida.
3. **`-dS -fsched-verbose-5` como herramienta de primera línea.** Imprime, por
   bloque, la tabla `insn/code/dep/prio/cost/units`, las listas de preparados de
   cada ciclo y **un diagrama de ocupación de unidades funcionales**. En el
   bloque de literales de `UpdatePlatInfo` convirtió «¿por qué reordena?» en «hay
   dos unidades `iu2` y entran dos `lis` en t=1» en un comando. Debería estar en
   la lista de herramientas del brief junto a `rtldump.py`.
4. **`scratchpad/rtl` y `%TEMP%` están reventando el disco.** Durante esta ronda
   `build_direct.py` falló con `No space left on device` (quedaban 333 MB, y
   bajando mientras otros agentes compilaban). `%TEMP%` tiene **3,4 GB en 3.221
   ficheros**, de los cuales **1,46 GB son `*.tmp` de compilaciones de hace más
   de 12 h**; `scratchpad/rtl` tiene 222 MB de volcados de otros agentes. Yo sólo
   he borrado lo mío (~90 MB) porque borrar ficheros ajenos no me corresponde,
   pero **alguien tiene que barrer eso o las tandas van a empezar a fallar en
   silencio** (el fallo aparece como `cc1plus: … I/O error`, no como disco
   lleno).

---

## 6. Verificación

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEcstasy Speed/Indep/SourceLists/zEagl4Anim
  -> 2 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEcstasy
  -> 4 de 539 distintas, 5.180 B    (epCalculate 164, UpdatePlatInfo 27,
                                     GenerateHorizonFog 2, eProject 13)
     131 mas solo con nombres distintos (76.252 B)   <- IGUAL que a la entrada
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> 2 de 318 distintas, 2.808 B    (Initialize 2356/2352, EvalState 52)
     113 mas solo con nombres distintos (87.768 B)   <- IGUAL que a la entrada
python scripts/audit.py Speed/Indep/SourceLists/zEcstasy     -> 0 FALLA
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim   -> 0 FALLA
python scripts/lcfix.py --check   -> todas las entradas @lc estan al dia
git diff --stat  -> 3 ficheros, 80 inserciones, 0 supresiones, TODAS `//`
```

Ninguna función de las dos unidades empeora; ninguna cambia de bytes. Sin
commits, sin `ninja`, sin `configure.py`, sin `config/GOWE69/*`, sin
`splits.txt`.

Bancos reproducibles en `scratchpad/r48_ecs/`: `g.py` (compila un `.cpp` suelto y
lee el `.greg`), `greg.py` (orden de asignación, conflictos y preferencias),
`gold.py` / `gold2.py` (comparación canónica del `.s` contra el ORIGINAL),
`sweep.py`, `ep.py`, `bench.py`, `ver.py`.
