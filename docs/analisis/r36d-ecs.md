# r36d — zEcstasy: la ranura huérfana es de `sched1`, y una veda de la r30 que caducó

Encargo: `epCalculateLocalDirectionalPOS16` (2.072 B), `UpdatePlatInfo` (2.044 B)
y `GenerateHorizonFogDisplayList` (796 B), con el eje del **destinatario** de la
barrera (mi hallazgo de la r36c) sin barrer en `epCalculate`.

**Resultado en una línea: la unidad mejora por `eProject` —que NO estaba en el
encargo—, de 85,82089 % / 25 filas a 93,970146 % / 14 filas y de 46 a 54
instrucciones exactas, sin mover un byte (268/268)**, porque la veda que la r30
le puso («es un asm») **ha caducado**: la barrera selectiva es hoy una de las
cinco palancas legítimas del brief. Y del encargo sale un **diagnóstico cerrado
con número** de por qué `epCalculate` tiene 8 B de marco de más, más el eje del
destinatario **agotado** allí con 178 medidas.

Estado de la unidad, `pctsnap --cmp` sobre las 539 funciones de `zEcstasy`:

    MEJORAN: 1   +8,149 pp  eProject__FfffPA3_fPfN44   85,8209 -> 93,9701   268 B
    EMPEORAN: ninguna

| función | antes | ahora |
|---|---|---|
| `eProject` | 85,82089 % / 25 filas / eq 46 | **93,970146 % / 14 filas / eq 54** |
| `epCalculateLocalDirectionalPOS16` | 93,305016 % / 155 | igual (objeto byte a byte) |
| `UpdatePlatInfo` | 99,33464 % / 28 | igual |
| `GenerateHorizonFogDisplayList` | 98,99497 % / 2 | igual |
| `EmitterSystem::Render` | 98,074715 % / 34 | igual |

`lcfix.py zEcstasy --check`: «todas las entradas `@lc` están al día». Tres
ficheros tocados, todos dentro de mi territorio: `eMathE.cpp` (el cambio real +
comentario), `EcstasyEx.cpp` y `EmitterSystem.cpp` (**sólo comentarios**, objeto
idéntico — verificado con `pctsnap`, que sí ve un cambio de contenido a igual
tamaño). **No he hecho commit.** No he tocado `configure.py`, `config/GOWE69/*`
ni `splits.txt`.

**Dato de contabilidad para quien reparta: `zEcstasy` tiene 539 funciones y sólo
CINCO por debajo del 100 %** — las cuatro del encargo más `eProject`. Las cinco
están en este informe.

---

## 1. Lo que se lleva la ronda: `-fno-schedule-insns` da el marco del objetivo

`epCalculate` llevaba dos rondas atascada en «nos sobran 8 B de marco (0x178
contra 0x170) y eso son 49 de las 155 filas». La r36c cerró el *tamaño* de la
ranura (una de 8 B, sin referencias, por encima de la última local) y gastó 296
medidas contra ella sin moverla. **Faltaba preguntarle al compilador qué pasada
la pone.** Barrido de 25 flags sobre el banco suelto (1,5 s cada uno, leyendo el
`stwu`):

| flag | marco | insns |
|---|---|---|
| base | 0x178 | 518 |
| **`-fno-schedule-insns`** | **0x170** ← el del objetivo | 521 |
| `-fno-expensive-optimizations` | 0x180 | 520 |
| `-fno-move-all-movables` | 0x160 | 504 |
| `-fno-inline` | 0x190 | 499 |
| los otros 20 | 0x178 | 517–518 |

**Es el ÚNICO que lo consigue.** O sea: los 8 B los crea el planificador
**pre-reload** (`sched1`) al alargar rangos de vida, no `alter_reg` ni una local.

### Y NO es un fallo de cflags — medido en la unidad entera

La tentación evidente («entonces zEcstasy no lleva `-fschedule-insns`») está
**descartada con medida**. Recompilada la SourceList completa con
`-fno-schedule-insns` y comparada con `pctsnap`:

    MEJORAN: 0 funciones
    EMPEORAN: 39 funciones   (ps_Model -0,63 pp · psGouraud -0,40 · eWaitDrawDone
    -0,39 · psMWScreenContrast -0,05 · y 35 que estaban al 100 % y caen)

Los cflags de `zEcstasy` son correctos. Lo mismo confirmado por función sobre
`UpdatePlatInfo` (base 99,33 %, el mejor flag alternativo 99,28 %) y
`GenerateHorizonFog` (base 98,99 %, ninguno mejora): **para las tres funciones
del encargo, los 16 flags barridos empeoran o son neutros.**

### Qué hace `sched1` exactamente, leído en los volcados

Con `cc1plus -dg` sobre el preprocesado (la receta de `c36c-wld`), comparando el
`.greg` con y sin `sched1` — los números de pseudo son los mismos, porque
`sched1` sólo reordena:

    con sched1 (= lo nuestro)   global_alloc derrama  166=ez, 197=ld2y, 718=specular_a
                                y salva               293=lit_vertex_description, 517
    sin sched1                  justo al reves

Y aquí está la corrección importante: **el DWARF dice que el ORIGINAL derrama los
mismos que nosotros** (`ez` en `r1+0x58`, `ld2y` en `r1+0x60`, `specular_a` en
`r1+0x7C`, y las 89 locales restantes idénticas). El objetivo corrió `sched1` y
tomó **nuestras** decisiones de reparto. La diferencia no está en qué se derrama.

### La ranura huérfana no es de ninguna pseudo

Censadas las 24 pseudos sin registro duro del `.greg`, quitadas las 8 que llevan
`REG_EQUIV` (las seis direcciones `lo_sum` de las tablas y las dos constantes),
quedan **16 pseudos con ranura**, que suman **exactamente 120 B** — y los 16
offsets (0x8, 0xC, y catorce de 8 B de 0x10 a 0x78) **se referencian todos y son
idénticos offset a offset a los del objetivo**, extraídos del asm final de los
dos lados. `get_frame_size()` vale 120 en el objetivo y 128 en nosotros.

O sea: **los 8 B que sobran son un `assign_stack_local` SIN DUEÑO**, por encima
de la última local. Descartada la hipótesis natural (el temporal `(subreg:PS)` de
`specular_a`, pseudo 801): tiene registro duro en los dos volcados (f6 con
`sched1`, f0 sin él).

---

## 2. La cota de `epCalculate`, y por qué es un falso positivo

`register float specular_a asm("fr2")` —o `fr0..fr13`/`fr31`, dan todos lo
mismo— pone el marco en **0x170** y la función en:

| | fuzzy | filas | insns exactas | tamaño |
|---|---|---|---|---|
| base | 93,305016 | 155 | 376 | 2072/2072 |
| **pin `specular_a` fr2** | 92,324326 | **102** | **430** | 2072/**2060** |

**+54 instrucciones exactas y −53 filas.** Es el caso más extremo de «las dos
medidas que engañan» que ha dado el proyecto: el fuzzy baja 0,98 pp mientras
casan 54 instrucciones más.

**No lo aplico, y la razón no es el fuzzy: es que el objetivo SÍ tiene esa
ranura.** El DWARF nombra `specular_a` en `r1+0x7C`, y el asm del objetivo trae
el `stfs f8, 0x7c(r1)` + `lfs f13, 0x7c(r1)` + `ps_mr f0, f13` que el pin borra
(de ahí los 12 B de menos). El pin acierta el marco quitando **una ranura buena**
en vez de la huérfana — exactamente el falso positivo que la r36c documentó con
`hack_scale asm("fr6")`, pero mucho más grande. Con el pin la huérfana **sigue
ahí**, sólo que corrida a 0x78.

El motivo técnico de que el pin borre el `stfs`/`lfs`: `CLASS_CANNOT_CHANGE_SIZE`
es `FLOAT_OR_FPMEM_REGS`, así que un `(subreg:PS (reg:SF pseudo))` obliga a
`reload` a pasar por memoria; pero sobre un **registro duro** el SUBREG se
resuelve sin más. Regla nueva y reutilizable: **pinchar un `float` del que se
toma un cast a `sn_ps` elimina el viaje a memoria y produce código mejor que el
original.** En esta función eso pasa con los tres `float` con cast: `specular_a`
(0x170, 102 filas, 2060 B), `alpha_rng` (0x170, 127 filas, 2064 B) y `hack_scale`
(0x170, 131 filas, 2040 B). **Los tres son falsos positivos por la misma razón.**

Queda como **cota**: si alguien mata la huérfana de verdad, esta función cae a
~102 filas y sube a ~430 instrucciones exactas manteniendo los 2072 B.

---

## 3. El eje del DESTINATARIO en `epCalculate`: agotado, con 178 medidas

El encargo pedía esto explícitamente. La r36c ya había barrido 165 barreras
`×1/×2/×3` sobre las 55 locales **midiendo filas**; lo que faltaba —y es donde
está la palanca, según el §1— era medirlas **contra el marco**. Hecho:

| barrido | medidas | resultado |
|---|---|---|
| `asm("" : "+f"(x))` ×1/×2/×3 sobre las 59 locales FP, midiendo el marco | 178 | **ninguna baja de 0x178**; 18 lo suben a 0x180 |
| pines `register T X asm("fr2")`, uno por local FP | 59 | sólo los 3 `float` con cast dan 0x170 (§2) |
| pines de `specular_a` a fr0..fr31 | 32 | fr0-fr13 y fr31 → 0x170/515 insns; fr14-fr30 → 0x178/517 |
| pines de `alpha_rng` a fr0..fr13, fr31 | 14 | los 14 dan 0x170/516: **el registro no importa, importa sacarla del reparto** |
| barreras de PLANIFICACIÓN `__asm__ __volatile__("")`, una por sentencia (86 en el bloque interno + 68 en el externo) | 154 | mueven el código (516–522 insns) y **ninguna quita la ranura** |
| permutaciones del bloque `specular_a`/`scrg`/`scba`/`ssrg`/`ssb` | 10 | 0x178 las diez |

**Total de la ronda sobre `epCalculate`: 447 medidas nuevas.** Sumadas a las 296
de la r36c y las de la r30, el frente está cerrado por las cinco palancas.

Lo que sí sale de las 154 barreras de planificación, y vale para todo el árbol:
**un corte local del planificador NO reproduce lo que hace `-fno-schedule-insns`
global.** El `asm` volátil vacío es una barrera de verdad (el conteo de insns se
mueve en 100 de las 154), pero la ranura la decide el efecto acumulado de
`sched1` sobre TODOS los bloques, no un adelanto concreto.

---

## 4. `eProject` (268 B): la veda de la r30 caducó y valía 8,15 pp

La r30 midió en esta función que `__asm__("" : "+f"(halfVP2))` la llevaba de
85,82 % a 93,67 % y **la revirtió** con esta sentencia escrita en la fuente:

> «REVERTIDO: es un asm y el DWARF de eProject no lo tiene.»

Eso era la política de entonces. **Hoy la barrera selectiva es la palanca nº 2
del brief**, no emite un byte, y la r36b/r36c ya dejaron tres en `eLightE.cpp`
commiteadas. Reproducido el número exacto de la r30 y mejorado:

| variante | fuzzy | filas | eq | tamaño |
|---|---|---|---|---|
| base | 85,82089 | 25 | 46 | 268/268 |
| sólo la barrera de la r30 | 93,82089 | 15 | 53 | 268/268 |
| sólo `*sx` reasociado | 86,1194 | 23 | 48 | 268/268 |
| **barrera + `*sx` + `*sy`** | **93,970146** | **14** | **54** | 268/268 |

Las tres piezas son inseparables (la reasociación sola casi no vale). Lo
aplicado:

```c
    float halfVP2 = vp[2] * 0.5f;
    float halfVP3 = vp[3] * 0.5f;
    __asm__("" : "+f"(halfVP2));

    *sx = vp[0] + halfVP2 + clipX * halfVP2 * oneOverW;
    *sy = vp[1] + halfVP3 + -clipY * halfVP3 * oneOverW;
```

Lo que queda son **dos ciclos de dos**, los dos de `local_alloc`:

    (a) en *sx:  halfVP2 <-> clipX     objetivo f0/f13,  nosotros f13/f0
    (b) en *sy:  halfVP3 <-> clipY-tmp objetivo f13/f12, nosotros f12/f13

**`register float halfVP3 asm("fr13")` cierra (b)** y deja **12 filas con eq=57**
(+3 exactas), pero baja el fuzzy a 92,955 % porque rompe el registro del
resultado de `*sy`: el pin reserva f13 durante **toda** la vida de `halfVP3` y el
objetivo **reutiliza** f13 para el producto. No aplicado; anotado en la fuente
por si alguien mide por instrucciones exactas.

Vedas nuevas de `eProject`, todas sobre la base de 14 filas: **40 pines**
(halfVP2/halfVP3/clipX/clipY/clipZ × fr0..fr13), **72 destinatarios de barrera**
(6 variables × N=1..3 × 2 sitios × 2 bases), **48 formas** de `*sx`/`*sy`
(reasociaciones, paréntesis y orden de `clipX`), **36 combinaciones** pin × forma
× barrera y **26 de dos destinatarios a la vez**. Ninguna baja de 12, y ninguna
que baje de 14 sube también el fuzzy.

Confirmación de la regla del §3 por otra vía: barreras sobre `halfVP3` son
**absorbidas** (objeto idéntico con y sin ellas), como la r30 ya había visto —
«el asm sólo añade una insn sobre una variable de vida larga».

---

## 5. `EmitterSystem::Render`: la quinta palanca, también negativa

La r30 dejó el diagnóstico redondo: el objetivo **derrama** el centinela
`this->mEmitterGroups.EndOfList()` a `0xc8(r1)` (de ahí sus 8 B de marco de más,
0x128 contra 0x120) porque le queda un **hueco en r19**; nuestro reparto es el
suyo corrido uno. «Hay que conseguir que un pseudo NO QUEPA.» La r36b probó las
doce colocaciones de la barrera selectiva: ninguna.

Faltaba **el pin**, que en `UpdatePlatInfo` cerró 40 filas. Diez medidas:

| pin | fuzzy | filas | tamaño |
|---|---|---|---|
| base | 98,074715 | 34 | 696/688 |
| `grp` → r14 / r15 / r19 / r20 | 95,545975 (las cuatro) | 58 | 692 |
| `em` → r14 / r19 | 96,206894 | 69 | 692 |
| `plist` → r14 / r19 | 97,155174 | 59 | 688 |
| `num_textures` → r14 / r19 | 97,95977 | 37 | 688 |

**Ninguno mejora, y el REGISTRO NO IMPORTA**: los cuatro pines de `grp` dan el
mismo objeto. Confirma la regla de la r36c por el lado contrario: lo que hace un
pin es **sacar el pseudo del reparto**, y a dónde lo manda es secundario.

Con esto **las cinco palancas del brief están medidas contra esta función** y
ninguna abre el hueco de r19. Sigue siendo el mismo muro que
`WRoadNav::InitAtSegment`.

---

## 6. Método y herramientas

- **Banco suelto, tres ficheros ya montados** (`bench.py` + `vary.py` del
  scratchpad, prefijo `c36d_`): `EcstasyEx.cpp` (2,8 s con objdiff, 1,5 s si sólo
  hace falta el marco), `eLightE.cpp` (2,7 s), **`eMathE.cpp` (nuevo, sin
  prefacio, reproduce `eProject` exacto)** y **`EmitterSystem.cpp` (nuevo: hace
  falta `-I src/Speed/Indep/Src/Ecstasy` además del prefacio de la r30, si no
  fallan `EmitterSystem.h` y `eMath.hpp`)**.
- Arneses nuevos: `c36d_flag.py` (barrido de flags por el marco), `c36d_pins.py`
  (pin de cada local × registro), `c36d_barall.py` (barreras ×N por el marco),
  `c36d_schedbar.py` (barrera de planificación tras cada sentencia),
  `c36d_perm.py`/`c36d_eptpl.py` (permutaciones estructurales),
  `c36d_es.py` (banco de `EmitterSystem`), `c36d_unit.py` (recompilar una
  SourceList con flags extra, para medir un flag en las 539 funciones a la vez),
  `c36d_rtl.py` (volcados `.lreg`/`.greg` con flags extra).
- **Aviso que se hereda y confirmo por tercera vez**: los `.cpp` de Ecstasy son
  **CRLF** y un fichero de variantes con `\n` falla en silencio los cambios de
  varias líneas.
- **La receta de flags como diagnóstico**, que es lo que abrió la ronda: cuando
  una función esté atascada en algo estructural (marco, ranura, un derrame),
  **barre los `-fno-*` primero**. Cuesta 25 × 1,5 s y dice qué pasada lo hace. Y
  si un flag mejora, hay que medirlo en la **unidad entera** antes de creérselo:
  aquí el que arreglaba `epCalculate` empeoraba otras 39.

## 7. Lo que NO he tocado

No he repetido ninguna de las vedas que el encargo prohíbe (los 64 pines fr2/3/4,
las 56+165 barreras de la r36c, el permutador ciego y las 10 barreras de ranura
de `GenerateHorizonFog`, ni `rank_for_schedule`). `UpdatePlatInfo` sigue en
99,33464 % / 28 filas: el eje del destinatario allí estaba agotado en la r36c
(350 medidas) y lo único que he añadido es el barrido de flags del §1, que
también sale negativo.
