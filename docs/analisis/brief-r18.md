# Ronda 18 — instrucciones comunes

Estado: **94,62% matched**, 18.042 funciones, `main.dol: OK`. Quedan **212.588 B**:
120.956 en las SourceLists y **91.632 en middleware**, que hasta ahora nunca se
había repartido.

## LO QUE CAMBIA: el método

Esta ronda adopta el método con el que jferr ha estado trabajando en paralelo, y
que rinde bastante más que el nuestro: en su última tanda cerró **+32.424 B y 66
funciones** con tres agentes. No es que barra más formas — es que **prueba** lo
que cierra, en vez de creerse el porcentaje. Cuatro cosas, en orden:

### 1. Fase de preparación, sin tocar nada

Antes de la primera edición, escribe en `docs/analisis/r18-<tu grupo>.md`:

- **La identidad**: tamaño de la función en bytes e instrucciones, su dirección
  en el ELF original, y el SHA256 del objeto objetivo si vas a apoyarte en él.
- **El censo de llamantes**: `bl` directos al símbolo en el `.text` del ELF. Di
  explícitamente que es un censo de llamadas **directas**, no una prueba de que
  no haya punteros a función.
- **Los llamados y su contrato**: qué registros consume cada uno, qué devuelve.
- **Qué NO has probado.** Esto es parte del entregable, no una disculpa.

Suena a burocracia y no lo es: es lo que evita gastar la ronda en una hipótesis
que el asm ya desmentía.

### 2. Ensayos acotados y numerados

Lleva la cuenta como `c1`, `c2`, `c3`… cada uno con **su cifra medida** y una
línea de por qué se descarta. Al final, una línea diciendo cuál se queda. Los
descartados son instantáneas, no quedan en el árbol. Ejemplo real:

    c1: solo el orden del reset -> 99,697% (intercambia r27/r28 globalmente)
    c2: ligar los dos valores, restricción de lectura/escritura -> 100% y 99,636%
    c3: restricciones solo de entrada -> mismo extsh no deseado
    c5: int normal, solo la bandera ligada -> las dos al 100%   <- se queda

### 3. La prueba es `scripts/audit.py`, no el porcentaje

**`python scripts/audit.py <unidad> [Simbolo]`** demuestra que una función casa:
resuelve las reubicaciones contra el ELF original, comprueba que cada rama tiene
el mismo destino **relativo** y la misma condición/CR, y compara **el valor** de
cada literal (4 bytes si es flotante, hasta el NUL si es cadena), no que "haya un
literal". Ya ha encontrado una función al 100% de objdiff que referencia
`LightTable` donde el original referencia `MaxSceneryLightContexts`.

**Un cierre no está cerrado hasta que pasa `audit.py`.**

### 4. Congela lo que cierres

**`python scripts/frozen.py cong <unidad>`** guarda el diff entero; `chk`
comprueba que sigue igual. Quita `line_number` antes de comparar, así que añadir
un comentario no da falso positivo. Caza cualquier cambio, no sólo una caída de
porcentaje — que es lo que `pctsnap` no veía.

## Sobre las restricciones de registro

El árbol ya las usa: 10 ficheros de `src/libc/` y `TrackStreamer.cpp`
(`register bool predict_position_used asm("r28")`), y esas funciones están al
100%. **Se permiten, pero como último recurso y con condiciones:**

- Sólo después de agotar la búsqueda de la forma de fuente, **y habiéndola
  documentado** en tus ensayos numerados.
- Sólo restricciones vacías de **entrada**: no emiten ninguna instrucción.
- **Tiene que pasar `audit.py`.**
- Con un comentario diciendo qué forma de fuente se buscó y no se encontró.

Lo que sigue prohibido es **escribir ensamblador** (`__asm__ volatile("li r3,0")`):
eso no cierra una función, la falsifica. Se revirtieron 456 B por eso.

La razón de la cautela: un registro clavado **esconde la forma de fuente real**, y
la forma real es la que hace caer también a las funciones vecinas. Esta ronda ha
cerrado nueve funciones encontrando la forma, no clavando registros.

## Lo demás sigue igual

- `python scripts/triage.py <unidades>` para tu lista: `faltan N, sobran M, de
  ellas K SUST`. **`SUST` son sustituciones**: `faltan 4, sobran 4, de ellas 4
  SUST` es **una** diferencia repetida cuatro veces, no ocho.
- `measure.py -o antes_r18_<grupo>.json` y `despues_…`, más `pctsnap` antes y
  después. `measure.py` **no compila**: pasa `build_direct.py` antes.
- **Compila tu `.o` al scratchpad**, no al `build/` compartido.
- `scripts/mn_repro.py` y hermanos iteran en 4-6 s sobre un `.cpp` suelto. Ya
  gritan cuando objdiff no da porcentaje, pero **no compilan las SourceLists** y
  no todas las unidades reproducen el objeto de ninja: **verifica el stub contra
  el `.o` de la unidad antes de fiarte**.

## Trampas medidas — todas han costado dinero

| | |
|---|---|
| **el tamaño exacto miente** | seis casos esta sesión: la variante da el tamaño del objetivo y **empeora** `faltan/sobran`. **El tamaño no es la medida** |
| **el porcentaje tampoco mide avance** | dos correcciones reales a la vez hicieron coincidir *todas* las ranuras de pila y objdiff **bajó** 2,2 pp |
| **`bench.py`** | puede dar un **100% FALSO** |
| **disco lleno** | `No space left on device` se lee como «la variante no cambia nada» |
| **finales de línea** | mixtos **por línea**. Usa `\r?\n` |
| `regmap` | empareja mal con sobrecargas |
| `litcheck` | falsos positivos en el lado del OBJETIVO |
| `zcmp2` | miente con permutación global de registros |
| `permuter` | cinco rondas sin cerrar nada |

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada.** Una función al 99,99% aporta cero.
- **Si no cierra, revierte y anota la veda diciendo QUÉ SENTENCIA barriste.**
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Cabecera compartida = A/B por objetos sobre TODAS las unidades que la
  incluyen**, y luego la construcción completa.
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
- **`ppc2D2` NO se toca**: jferr tiene una preparación en vuelo
  (`scratchpad/phase14_ppc2d2_plan.md`).
