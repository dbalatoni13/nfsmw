# Brief de la ronda 49

Sustituye a `brief-r48.md`. Leelo entero antes de tocar nada.

## Estado

`matched` **99,04542 %** (3.908.380 / 3.946.048 B, 18.399 / 18.432 funciones).
`linked` **19,57 %** (494 / 618 unidades). El **SDK entero al 100 %**.
Quedan **37.668 B en 32 funciones de 13 unidades**.
DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

**Y hay un segundo frente, que ahora es el mayor**: 1,64 MB de codigo que casa al
100 % y no promociona. Ya no es un misterio: la r48 lo dejo mapeado y con
predicado. Ver `docs/analisis/r48-jf-censo-linked.md`.

## Reglas duras

1. **Nunca lances un `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el arbol.
2. **No hagas commits ni `git add`.** Deja los ficheros modificados.
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Proponlo con
   la medida que lo respalda.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques --salvo que el encargo te la asigne, y entonces el
   gate son TODAS las unidades que la incluyen.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Ancla cada parche con **dos o tres lineas de contexto**. Hay ficheros CRLF
   enteros, LF enteros y alguno MIXTO (`promote.py` tiene 255 CRLF y 99 LF): un
   parche con una sola forma casa cero sin avisar.
7. **Si retiras un ensayo, comprueba que la fuente vuelve de verdad**, y pasa
   `fncmp` al final.
8. **BORRA TUS VOLCADOS RTL AL TERMINAR** (`scratchpad/rtl`, `*.i.*`, `*.s`).
   En la r48 el disco llego a **0 B libres** a mitad de ronda, y eso se ve como
   un `cc1plus: I/O error` que parece un fallo de compilador. Cada volcado ocupa
   20-30 MB.
9. **Cero bytes con `asm` puesto es deuda**: si una funcion no llega al 100 %,
   revierte y deja la veda escrita, salvo que el avance sea estructural y lo
   digas claro.

## Herramientas

    python scripts/fncmp.py <unidad> [<Nombre>]   # que funciones NO son identicas
    python scripts/fndiff.py <unidad> <simbolo>   # confirmalo ANTES de empezar
    python scripts/triaje.py <unidad>             # PERMUTADOR / falta local / estructura
    python scripts/lmap.py <unidad> <simbolo>     # la linea de fuente de cada instruccion
    python scripts/rtldump.py <unidad> <fnfiltro> -dg -dl   # .greg / .lreg / .combine
    python scripts/schedtrace.py ...              # la traza del planificador, ciclo a ciclo
    python scripts/promopred.py                   # el predicado de promocion
    python scripts/promomap.py                    # el mapa de linked, en paralelo
    python scripts/build_direct.py <unidad>

`fncmp` ya imprime el detalle **aunque el tamano difiera** (antes se quedaba
mudo justo en las funciones de +-4 B, que son el caso mas barato), y ya lee la
seccion `.over`.

**La TU reducida** (r48): un `.cpp` con el preambulo de la SourceList y **un
solo `#include`** reproduce una funcion byte a byte y compila en **11 s** contra
40. Vale para todo el arbol y es lo primero que deberias montar.

## LA LECCION DE LA RONDA 48

**En cinco de los seis cierres, la causa NO estaba donde senalaba el diff.**

- **`SetScreenNames`**: seis filas de reparto, y la causa era que el original
  expandia `IsInBackRoom()` **inline** y nosotros llamabamos a la funcion libre.
  Una linea, cero bytes.
- **`ProcessUpdate`**: dos rondas la dieron por «cerrada por construccion» como
  problema de planificacion. **No era planificacion, era reparto**: el `.lreg`
  decia que un valor cruzaba dos llamadas y vivia en r30 entero. Al asignar el
  desplazamiento a la variable de aguas abajo, el `srawi` escribe otro registro,
  `sched2` ve una antidependencia y su `INSN_PRIORITY` salta de 2 a 17.
  **Una diferencia de planificacion dentro de un bloque puede tener la raiz en
  el asignador.**
- **`LoadSpeechBank`**: el diagnostico heredado era el sintoma. `lmap.py`
  **sobre el original** enseno que una linea se llevaba TRES comparaciones.
- **`UpdatePlatInfo`**: las 27 filas son `regs_someone_prefers`. Cuando le toca
  el turno al pseudo, sus registros estan **libres**, y los pierde porque dos
  allocnos de MENOR prioridad los prefieren --por el pin de OTRA variable--.

**Corolarios que valen como metodo:**

- **Un `asm` de deuda puede TAPAR la causa real.** En `SetScreenNames`, con el
  `asm "+m"` puesto la correccion correcta daba 15 filas; sin el, cero.
- **Un pin puede ser andamio.** En `InitAtSegment`, RETIRARLO gano tres filas:
  el DWARF ya daba ese registro y el pin forzaba un coalescing de mas.
- Antes de escribir la primera variante: `lmap.py` sobre el original, y el
  `.greg`/`.lreg`.

## Las palancas

Todas son de GCC. **En `LibSN/steering` no funciona ninguna** (es mwcc); ahi lo
unico medido es **leer a traves de un puntero `volatile`**.

### 1. Barrera selectiva

`asm("" : "+r"(x))`, `"+f"`, `"+m"`. Extendida y NO volatil: ata solo lo que
nombra. `"+m"` para una local de PILA (un `"+r"` sobre ella **se ignora en
silencio**). **Un `asm` por argumento DESCOLOCADO**, no por argumento, y sin una
local propia el resultado es 12 veces peor. **La cadena de N**: una dependencia
por eslabon, medidas solo juntas.

**`asm("" : : "r"(x))` de solo entrada sube `n_refs`, pero NO siempre es
gratis**: un `asm` sin salidas es volatil, asi que emite salvo que GCC lo
absorba en una copia preexistente. Medido en las dos direcciones: cero bytes en
`cStichWrapper::Play`, 67 -> 68 instrucciones en `eProject`. **Midelo.**

**`asm("")` a secas rompe empates EXACTOS** de `allocno_compare`.

### 2. `set_preference` encadena locales, y manda la SEGUNDA

`global.c` hace `src = XEXP(src, 0)`: de un `(set A (mult B C))` el allocno de
`B` **hereda** el registro de `A`. Por eso a veces hay que pinchar **la carga
que le quita el registro**, no la variable del diff. Y `regs_someone_prefers`
es la otra cara: un allocno de menor prioridad puede reservarte el registro.

### 3. Pin de registro

`register float x asm("fr7")`. Lo unico que llega a `local_alloc`. **Si el pin
empeora, el reparto es un SINTOMA**, y **si el DWARF ya da ese registro, el pin
sobra**. `REG_ALLOC_ORDER` de rs6000: `0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30…`
y en FP `f0, f13, f12, f11, f10…`. El «31 primero» solo sale en `global_alloc`
para allocnos que cruzan llamadas.

### 4. La cantidad fantasma

Con `register int guard asm("rN")` mas un `asm` de cierre que toque un valor
VIVO. **No vale en FPR de trabajo** (f0..f13), y en `madidct` esta cerrada con
24 registros x 8 combinaciones.

### 5. El `asm` que emite la instruccion que falta

Cuando el `.combine` ensena que la instruccion del objetivo ya esta y solo sobra
una conversion que `combine` no puede tirar. **No es deuda si el tamano pasa a
exacto y el diff a cero.** Aviso: un `asm` con cuerpo **no ancla** --emite la
instruccion pero su `"+f"` no sirve de barrera--.

### 6. Lo que se reserva al EXPANDIR y sobrevive a su codigo

El area X del marco (temporal de conversion entero<->flotante) y el `lfd` del
sesgo `0x4330000080000000`. Se reproduce multiplicando por una variable entera
que siempre vale 1. **Frente barrido y VACIO en las 42 pendientes de la r47**:
no busques un tercer caso.

### 7. Los dos grupos de peso de `sched1` --- y OJO con `sched2`

`rank_for_schedule` desempata por `INSN_REG_WEIGHT`... **pero ese nivel SOLO
existe en `sched1`** (`if (!reload_completed …)`). En `sched2` decide
`depend_count` y luego `INSN_LUID`. Mirar la traza equivocada produjo un
diagnostico falso en la r47. `schedtrace.py` da la traza ciclo a ciclo; ojo,
`cc1plus` da **ICE** con esa bandera en unidades grandes.

### 8. El DWARF trae los TIPOS

Lista las struct del original **con el offset de cada miembro**. Regla barrible:
si el objetivo pone la **BASE primero** en un `add` o un store indexado y
nosotros el indice, la fuente original **indexa un array MIEMBRO**. Y **el DWARF
tambien se equivoca**: en `PATHI_calcwaitbeat` daba dos locales en f11 y solo
una era cierta.

### 9. La forma del bucle decide el idioma de la rama

Con `&&` sale `blt`; con `if/else` + `break` sale `fcmpu + cror + bso`. Cerro
4.396 B a la primera compilacion.

## Avisos

- **La reciproca de «falta una local» es FALSA**: cuatro contraejemplos medidos.
- **`matched_code == total_code` NO implica enlazable.**
- **Las vedas caducan**: seis han caido esta temporada, casi todas por haber
  probado N formas de **la sentencia equivocada**. Si una veda cita un
  porcentaje, **remidelo sobre la base actual**.
- **El fuzzy engana**: puede SUBIR bajando las filas exactas. **Cuenta filas.**

## El reparto

| # | clave | territorio | B |
|---|---|---|---:|
| 1 | `linked` | la bandera de plantillas en las unidades sin enlazar, y el `_M_erase` | 1,6 MB de `linked` |
| 2 | `promo` | `zGameModes`, `zAI`, `pathbank`, `ef_rem_pio2`, `kf_rem_pio2`, `zMisc` | ~300 kB de `linked` |
| 3 | `cam` | `zCamera` | 10.812 |
| 4 | `world` | `zWorld` + `zWorld2` + `zTrack` | 10.112 |
| 5 | `ecs` | `zEcstasy` + `zEagl4Anim` | 7.988 |
| 6 | `resto` | `steering`, `zPlatform`, `zPhysicsBehaviors`, `zEAXSound2`, `zSpeech`, `madidct`, `criticalpath` | 8.756 |

**Los dos primeros valen mas que los otros cuatro juntos.**

## Que entregar

1. Informe en `docs/analisis/r49-<tu-clave>.md`: lo cerrado (con `fndiff` al
   100 % citado), lo medido y negativo **con la cifra**, y las propuestas fuera
   de territorio.
2. Verificacion: `fndiff` = 100.0 en lo que cierres; `fncmp` antes/despues sobre
   TODAS tus unidades sin que **ninguna** empeore; `lcfix.py --check` limpio.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**.
4. **Borra tus volcados RTL.**
