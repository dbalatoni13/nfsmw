# Ronda 22 — zEAXSound2: los tres near-miss

**Resultado: `ResolveCarBanks` 98,44433 % → 99,12360 % (+0,679 pp), y de
`faltan 1, sobran 2` a `faltan 1, sobran 1, de ellas 1 SUST`: cero instrucciones
de más o de menos, sólo UN opcode mal (`ble` contra `bgt`).** Aplicado al árbol.
`pctsnap --cmp`: **1 función mejora, ninguna empeora.**

Unidad: 158.400/170.256 B = **93,0364 %, 921 funciones al 100 %** (igual que
antes: la función sigue sin cerrar, y `matched_code` es todo-o-nada).

`audit.py Speed/Indep/SourceLists/zEAXSound2`: **921 ok / 0 FALLA**, dos pasadas
(una al empezar, otra tras el cambio, más una tercera de confirmación). **Lista
de FALLA: vacía.** Congelada de nuevo con `frozen.py cong` (huella
`4925fdd5353dc660`).

Encargo verificado con `build_direct.py` + `triage.py`: coincidía al dígito.

---

## 1. La herramienta que lo abrió: `dwbody.py`

Ninguna de las 43 formas de fuente de las rondas 20 y 21 movió un byte de
`ResolveCarBanks`. **`dwbody.py` la abrió en una sola pasada**, y con dos datos
que ni `triage`, ni `lmap`, ni `regmap` ven:

| | ORIGINAL | NUESTRO |
|---|---|---|
| `inline engineaudio::~engineaudio` | **21** | 20 |
| `Vector<unsigned int,16>::size` | **97** | 96 |
| `Vector<unsigned int,16>::end` | 21 | **29** |

La expansión de destructor que falta está **localizada al byte**: el original
tiene **tres** al final de BeginRule4 (dos coladas en `0x800D2764` y una de rango
cero en `0x800D2770`, o sea borrada por el *cross-jump*) y nosotros **dos**. Las
8 `end()` de más son de `Vector::erase` en `UTLVector.h`, que es de otro agente.

Ese censo es el oráculo que faltaba: **da una diana comprobable a la que apuntar
sin depender del porcentaje.** Con él se ve que `c4` (abajo) reproduce el censo
del original y `c5`/`c6`/`c8` no.

## 2. Lo que cerró los 4 B: invertir las DOS colas interiores

    if (FinalEngines.size() + AIEnginesWeWantToLoad.size() > 4) {
        goto BeginRule3;              // antes: if (<= 4) goto LoadRemainingEngines;
    }
    goto LoadRemainingEngines;        // antes: goto BeginRule3;

y lo mismo en el `for` interior de BeginRule4 (`goto BeginRule4`). **Las dos a la
vez**, en las líneas 250-253 y 280-283 de `STATEMGR_CarState.cpp`.

| ensayo | qué | insns | B | palabras distintas | % |
|---|---|---|---|---|---|
| base | | 1878 | 7512 | 906 | 98,44433 |
| e0 | sólo cola de BeginRule3 invertida | 1877 | **7508** | 612 | 98,61748 |
| f2 | sólo cola interior de BeginRule4 invertida | 1878 | 7512 | 903 | 98,65211 |
| **f1** | **las dos** | **1877** | **7508** | **609** | **99,12360** |

**Esto rompe la veda `r09` de la ronda 21** («cola interior invertida: mueve un
`li r4,0` de sitio y nada más»). Estaba medida con md5 de los bytes sobre la
base, donde efectivamente casi no mueve; por porcentaje son +0,21 pp, y
**combinada con la de BeginRule3 vale +0,68 pp**. Decimotercera veda caída por
haber medido la sentencia equivocada.

Barridas además, todas **idénticas a `f1`** (meseta): `else` explícito en
cualquiera de las dos colas, cola de BeginRule2 invertida o con `else`, `c4`, y
las nueve combinaciones de la tanda G. **La meseta es exactamente 609.**

## 3. El mecanismo, ahora entendido y medido

La diferencia era **colocación de bloque**, y ya sé de qué depende. La
«pastilla» compartida (`mr r3,r20 ; b ; mr r3,r31 ; li ; bl ; mr r3,r17 ; li ;
bl ; b LoadRemainingEngines`, 9 instrucciones) vive:

- **objetivo**: en `0x800D2098` = **+0x110C**, entre `b BeginRule2` y la cabecera
  de BeginRule3;
- **nosotros, base**: en **+0x17B8**, entre el `goto BeginRule4` de la línea 300 y
  la limpieza de cierre de bloque de la 302.

Todo lo anterior a `+0x110C` es **byte a byte idéntico**; el desfase de 9
instrucciones arrastraba el resto de la función. Con `f1` el desfase baja a 3 y
**el final de la función queda alineado al byte** (`LoadRemainingEngines` en
+0x17E4 en los dos), que es de donde salen los 294 palabras de mejora.

Comprobado con volcados RTL (`aha_rtl.py … rj`, y `-dJ` para jump2): las
limpiezas de un `goto` hacia delante que sale de un ámbito con destructores se
generan al final de la función y `reorder_insns` las mete **en el sitio del
goto** (visto en la cadena: uid 6656-6733 empalmados detrás del uid 6331 de la
línea 251). O sea, **el que sobrevive al *cross-jump* es el sitio del goto que no
se procesa**, y el orden de proceso lo decide `jump.c` recorriendo hacia delante.
Invertir la cola cambia cuál de las dos limpiezas es el «entonces» y cuál el
paso-a-través, y con eso cambia el superviviente.

## 4. Diagnóstico de pases: NINGUNO decide

24 banderas medidas sobre `ResolveCarBanks` (`c22eax2_flags.py`), todas
**idénticas** salvo `-fno-omit-frame-pointer` (1891 insns, obviamente peor):
`schedule-insns`, `schedule-insns2`, `gcse`, `cse-follow-jumps`,
`cse-skip-blocks`, `expensive-optimizations`, `force-mem`, `force-addr`,
`rerun-cse-after-loop`, `rerun-loop-opt`, `move-all-movables`, `thread-jumps`,
`peephole`, `function-cse`, `defer-pop`, `strength-reduce`, `caller-saves`,
`delayed-branch`, `regmove`, `branch-count-reg`, `inline-functions`,
`keep-inline-functions`. **La diferencia era 100 % de fuente.**
(`-fno-crossjumping`, `-fno-if-conversion`, `-fno-optimize-sibling-calls` y
`-fno-guess-branch-probability` no existen en GCC 2.95: error de compilación.)

## 5. Lo que queda en `ResolveCarBanks`

- **Una sustitución**: fila 1324, objetivo `ble <pastilla>` contra nuestro
  `bgt <limpieza de goto BeginRule3>`. El objetivo conserva la polaridad de la
  base en esa cola **y aun así coloca la pastilla en +0x110C**; nosotros sólo
  conseguimos moverla invirtiéndola. Falta el dato que explique eso.
- **30 filas `ARG_MISMATCH`**: los dos racimos de registro que la ronda 20 dejó
  anotados (filas 1418-1443, `srawi r5/r6` contra `r4/r5`; filas 1672-1700,
  `r22`/`r24`, `r20`/`r22`, `r24`/`r21`). Sin tocar.
- El `size()` que le falta a nuestro censo DWARF: el original tiene **tres** a
  nivel de bloque en el `if (CopsCanBeInGame && FinalCopV8Engines.size() == 0)`
  (`0x800D17F8`, `0x800D1804` ×2) y nosotros **uno** ahí más otro un nivel más
  adentro. Los tres son de rango cero, así que no emiten código, pero es la única
  diferencia estructural que queda documentada y sin explotar.

---

## 6. `BindToData` (340 B, 95,88236 %) — muro confirmado, 28 formas más

`dwbody` **sí** da un dato nuevo: en el original `ptr` vive en **r30** y
`memdata` **no tiene registro**; en el nuestro `ptr` está en r4 y `memdata` en
r30. O sea, a la coalescencia de `memdata = ptr` sobrevive el **parámetro** allí
y la **local** aquí. **No es accionable**: nueve formas de esa copia (magic sobre
`ptr`, los dos términos sobre `ptr`, declaración y asignación separadas, la
declaración detrás de los ceros, detrás del magic, `const`, cast en C,
`reinterpret_cast`) dan **las nueve el mismo objeto**.

El diff completo son **tres cosas y una sola causa**: al objetivo el `-1` de
`mCurrentBlock` le nace **tarde y en r0** (insn 51, justo detrás de `mr r7,r0`,
que es lo que libera r0), así que la base del bucle nace ya en **r8**; a nosotros
le nace en la insn 33 y ocupa r8, la base cae en r5 y hace falta un `mr r8,r5`
en el precabezal. **El orden de los `stw` ya coincide fila a fila.**

Vedas nuevas (todas revertidas, ninguna mueve un byte):

1. **10 grafías del `-1`**: `~0`, `static_cast<int>(0xffffffff)`, `0 - 1`, un
   local `int nb = -1;` (declarado arriba y declarado justo antes), y las
   posiciones detrás de `int minperiod`. **Idénticas.** Se confirma la meseta de
   la ronda 21: 55 palabras distintas con `mCurrentBlock` después de
   `mSampleCount`, 32 antes.
2. **9 formas del bucle** (r21 no lo tocó): resta partida en dos sentencias,
   `minperiod > period`, `++i`, `while` con el incremento al final, puntero local
   `int *cyclepos`, `mCyclePos + mCycleCount + 1` sin paréntesis, `mFreqPos +
   mSegCount + 1` sin paréntesis, resta invertida con negación. **Idénticas**
   salvo la resta partida (87 insns, peor).
3. **22 banderas de pase**: todas idénticas.

Y queda **remedida y ratificada** la trampa de la ronda 21: con `mCurrentBlock`
delante de `mSampleCount` salen los **340 B clavados** y **32** palabras
distintas contra las 55 de la base… y objdiff da **93,61176 %**. Mirado fila a
fila es peor de verdad: el `li r0,-1` se va a la insn 31 (el objetivo lo tiene en
la 52) y el `stw …,0xac` a la 49 (objetivo: 56). **Undécimo caso de «el tamaño
exacto miente», y el primero en el que también miente el recuento de palabras.**

## 7. `Play__16CARSFX_RoadNoise` (392 B, 94,93877 %) — muro, 16 formas más

`dwbody`: **el árbol de inlines es idéntico** (la única línea distinta es la
dirección del estático `LastRandom`). No falta ni sobra ninguna sentencia: es
puro reparto.

El corte, releído fila a fila: el objetivo agrupa los ceros en **dos** pseudos —
**r29** para `volume` (0x8), `azimuth` (0x10) y el `refCount = 0` de
`GetRefCount` (que vive a través de las dos llamadas a `CreateInstance`), y
**r0** para `type` (0x14), `secondaryNoise` (0x18), `speed` (0x1c), `hiPass`
(0x24) y `wetFX` (0x2c). Nosotros usamos **uno solo** (r30) para los siete.

Vedas nuevas:

1. **9 formas del sitio de nacimiento de los ceros** en `Play`: `int refcnt = 0;`
   declarada antes del `new` y reutilizada para `volume`/`azimuth`; `refcnt = 0`
   antes pero con los once literales; un local `int zero = 0;` para
   `volume`/`azimuth`, para los siete, y sólo para `type..wetFX`; puntero
   temporal para el `new` con `GetRefCount` sobre él; el bloque interior sin
   llaves con `refcnt` arriba. **Todas iguales o mucho peores** (el local `zero`
   dispara a 108-131 instrucciones: GCC deja de plegar).
2. **7 formas del clamp de `SetType`** en `ENVIRO_AEMS.h`, esta vez **midiendo a
   la vez `Play` e `InitSFX`** con un árbol sombra (`c22eax2_hsw.py`), que es lo
   que la ronda 21 no hizo: ramas intercambiadas, cotas como literales,
   comparaciones en `int`, `<=`/`>=`, parámetro `int` con cast en el `store`,
   temporal de salida. **Play sale idéntico en las seis** (la de ramas
   intercambiadas, peor: 59 palabras contra 55) **e `InitSFX` no se mueve.**
   Sirve como aviso: el barrido de cabecera **tiene que llevar `InitSFX`
   enganchado**, o se repite la trampa de `h15`.

## 8. Lo que NO he probado

- **`ResolveCarBanks`**: los dos racimos de registro (filas 1418-1443 y
  1672-1700); reconstruir el `size()` que falta en el bloque de
  `CopsCanBeInGame`; el permutador; y el paso final, que es conseguir la
  pastilla en +0x110C **conservando** la polaridad `ble` de la cola de
  BeginRule3.
- **`BindToData`**: el permutador; `lreg.py` sobre los pseudos del `-1` y de la
  base del bucle (que es lo que decidiría si el empate se puede romper por
  `n_refs`); restricciones de registro (último recurso, no las he tocado).
- **`Play`**: el permutador (ni guiado ni ciego); `lreg.py`; cualquier cosa que
  parta la cadena de CSE entre `azimuth` y `type` sin cambiar el clamp.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt`, los bloques
  `__ANDROID__`, ni ningún fichero del agente de zEAXSound.

## 9. Aviso sobre cabecera compartida

A mitad de sesión `UTLVector.h` (otro agente) estuvo en un estado en que
`ResolveCarBanks` pasaba de **1878 insns / 7512 B a 1909 / 7636** (+31
instrucciones). Al cerrar la sesión ese estado ya no está y la función mide
igual con la cabecera del árbol y con la de `HEAD`. Lo dejo anotado porque el
cambio que lo provocaba (`FixedVector(const FixedVector&)` llamando a
`Vector::Init()`, más `Init()` con cuerpo) **castiga fuerte a esta unidad** si
vuelve. Mientras duró, todas mis medidas se hicieron con la cabecera de `HEAD`
fijada en un árbol sombra.

## 10. Herramientas dejadas en el scratchpad (prefijo `c22eax2_`)

| | |
|---|---|
| `c22eax2_rcb.py <variantes.py>` | oráculo genérico: compila en paralelo variantes de un `.cpp` con los cflags de la unidad y da insns / bytes / **palabras distintas contra el objeto objetivo** / offset de la pastilla. Las variantes son listas de `(viejo, nuevo)`, así que se combinan |
| `c22eax2_helper.py` | extrae líneas exactas del fuente respetando los finales de línea **mixtos** (este fichero tiene 394 CRLF y 4 LF) |
| `c22eax2_hsw.py` | barrido de **cabecera** con árbol sombra midiendo **varios símbolos a la vez** — es lo que evita la trampa de `InitSFX` |
| `c22eax2_dwcount.py <obj.o>` | censo de expansiones inline de `ResolveCarBanks` en un `.o` cualquiera (`dtk dwarf dump`): es el oráculo estructural del punto 1 |
| `c22eax2_flags.py` / `c22eax2_flags2.py` | diagnóstico de pases (24 y 22 banderas) |
| `c22eax2_pct.py` / `c22eax2_rows2.py` / `c22eax2_lst.py` | % de objdiff, filas del diff y listado con offset y línea de un `.o` suelto, sin contexto de proyecto |
| `c22eax2_list2.py` | listado con offset y **número de línea** de los dos lados desde `rows_zEAXSound2.json` |
| `c22eax2_rtlsum.py` | resume un volcado RTL de una función a una línea por insn (notas de línea, etiquetas, saltos, llamadas): es lo que deja ver dónde se empalman las limpiezas de un `goto` |
