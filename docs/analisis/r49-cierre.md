# R49 — cierre

2026-09-09. Seis agentes en paralelo, más el trabajo de coordinación sobre el
frente de `linked`. DOL verificado en
`9619ba57c9919f95f7f2ac951a2166a3517f91e3` después de cada paso.

## Estado

| medida | r48 | ahora |
|---|---:|---:|
| código exacto | 3.908.380 B — 99,04542 % | **igual** |
| funciones exactas | 18.399 / 18.432 | **igual** |
| unidades linked | 494 / 618 | **497 / 618** |
| código linked | 772.384 B | **777.568 B** |

**Ninguno de los seis agentes cerró una función.** Es la primera ronda así, y
conviene decirlo claro: lo que quedan son 32 funciones que llevan entre tres y
seis rondas resistiéndose, y varias están demostradas irreducibles. El avance de
la ronda fue **+5.184 B de `linked`** y, sobre todo, **diagnóstico**.

Y una forma útil de ver dónde estamos: **el 80,4 % de las unidades ya enlaza; el
19,6 % que no, guarda el 80,3 % del código.**

## Lo aplicado

- **Tres promociones**: `ef_rem_pio2` (848 B), `kf_rem_pio2` (2.180) y
  `pathbank` (2.156). `trypromo` da DOL OK por separado y las tres juntas.
- **`zFe2` vuelve a enlazar.** No enlazaba **en absoluto** —16 alias mangled
  caducados, desfasados exactamente en 3—, y nadie lo sabía. Con `mangfix.py`
  pasa de `ENLACE FALLA` a `.text +0`.

## El vuelco del frente de `linked`

La r48 dejó un mapa con predicado y una palanca. **La r49 lo refuta entero**, y
el error era metodológico: **yo comparé los OBJETOS y el frente está en el
ENLACE**. La herramienta que lo dice —`scripts/linkdelta.py`— llevaba dos días
en el árbol, y su docstring abre advirtiendo del error exacto que cometí.

Lo que cae:

- **El frente de los «símbolos de más» no existe**: `.text` a delta **cero en 20
  de 23 unidades**. El enlazador estripa lo que no se referencia.
- **`_M_erase` no bloquea veinte unidades**: no está en el ELF original **ni en
  el nuestro**.
- **`-fno-implicit-templates` no es la palanca**: falla en 12 de 13 unidades, y
  en la única segura los dos objetos dan **el mismo DOL**.
- **`promopred.py` mide algo que no es el bloqueo**: la prueba de «referenciado»
  no es transitiva desde raíces vivas.

Lo que queda en pie son las medidas de **datos**, que van sobre símbolos con
nombre: el déficit era nomenclatura, y lo que faltaba de verdad eran 4 B en
`.data` y **0 B en `.bss`** (medido esta ronda; ver el censo).

## El frente de verdad, y su unidad más cercana

`linkdelta.py` deja **22 unidades con déficit de `.rodata` viva en el enlace**,
de 288 a 4.216 B. Y `zSim` es la más cercana: secciones enlazadas **idénticas** a
la base y el código a 0 de 402.

**Su permutación es de tres funciones.** De 402 comunes, **399 ya están en el
orden correcto**; los tres destructores de `FixedVector` salen veinte posiciones
antes de donde el objetivo los pone. El objetivo emite cada `~FixedVector`
inmediatamente después del `~Vector` de su mismo tipo; nosotros sacamos los tres
juntos delante de los accesores, y eso corre doce funciones `+3`.

Y la misma medida (`scripts/permorden.py`) ordena el frente entero:

| unidad | comunes | desplazadas |
|---|---:|---:|
| **`zSim`** | 402 | **3** |
| `zMain` | 1.380 | 32 |
| `zLua` | 537 | 68 |
| `zFe2` | 1.290 | 144 |
| `zAI` | 1.030 | 182 |
| `zGameplay` | 767 | 270 |

De paso: **`zFe2` emite 1.290 de las 1.307 funciones del objetivo**, o sea que
hay **17 que no emitimos** — un problema distinto del orden y que nadie había
visto.

## Palancas nuevas de la ronda

1. **«El `and` que se hunde»** (`haifa-sched.c`): `INSN_REG_WEIGHT` da peso 2 a
   un `and` reg-reg porque `andsi3` de rs6000 es siempre un `PARALLEL`, y eso lo
   hunde bajo cualquier `li`/`mr`/`lwz`. Arreglo de cero bytes: mover la
   sentencia para que un operando muera en ella.
2. **El «tamaño mentido» generaliza a arrays grandes**: `extern const int t[2]
   __asm__("t_8050042C")` conserva el `@sda21`, no mueve el `.text` y quita
   920 B de `.sdata2`.
3. **Un `$LC` muerto se estripa y desplaza 4 B; un `asm()` de fichero no.**
4. **Si el objetivo tiene una insn más abajo que nosotros, no es el
   planificador, es la fuente** — `compute_trg_info` sólo trae insns de bloques
   *posteriores* al destino.

## Diagnósticos que dejan de ser vagos

- **`EvalState`**: 52 → 18 filas sin un `asm`. `lmap.py` sobre el original
  enseñó que el producto `índice * GetKeySize()` estaba escrito en el
  **llamante**, no en el accesor.
- **`zCamera/__static_init`**: no es presupuesto de `cse`. Son cuatro
  `addi rN,r30,K` de una init de tabla, y **la fuente es correcta**: en una TU
  que sólo la contiene sale igual que el objetivo. Es efecto de **tamaño de
  unidad**. Con 22 cargas muertas delante, la función pasa a **3.604 B exactos y
  6 filas**.
- **`CullParts`**: compilar la cadena de `if` como `switch` da el tamaño exacto y
  demuestra la causa (`record_jump_equiv`). El `switch` como **experimento de
  control** merece entrar en el catálogo.
- **`criticalpath`**: 40 → 26 filas sin `asm`, revertido porque el mapa de
  líneas lo contradice; y lo que queda está **demostrado inalcanzable**.

## Correcciones a rondas anteriores

1. El cierre de `SetMemoryPoolSize` de la r48 **era erróneo**: se leyeron las
   líneas `--> scheduling insn` en vez de la tabla ciclo × unidad.
2. El diagnóstico de `TerrainVelocityNoise` estaba en el nivel equivocado (es la
   prioridad, no el LUID).
3. **`regmap` da falsos positivos por TIPO**: empareja un `bool` con un `int` e
   inventa permutaciones que no existen.
4. **`promote.py` daba falsos positivos** en «nos falta un símbolo»: recorría
   todo `build/GOWE69/obj/` en vez de la lista de enlace, contando objetos
   rancios. Tres de las cuatro unidades que el censo de la r48 puso en esa
   familia estaban mal diagnosticadas. Arreglado.
5. `zMisc`: `lbl_8041F8C3` **no es un racimo** —lo define nuestro `SunE.cpp`—, y
   el `.bss +64` de `zAI` **no es su bloqueo** (tras enlazar, delta 0).

## Infraestructura

El disco volvió a llenarse. Limpiados **3,9 GB en 19.604 binarios regenerables**
de `scratchpad` (`.o`, `.s`, `.i`, `.elf`, `.dol`), conservando los `.json`,
`.py` y `.md`. De 3,6 GB libres a 6,4. Los agentes ya borran sus volcados.

Aviso de método: `find … -prune -o … -delete` **no funciona** —`-delete` implica
`-depth`, que anula el `-prune`—; hay que usar `-not -path`.
