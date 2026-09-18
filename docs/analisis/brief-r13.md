# Ronda 13 — instrucciones comunes

Estado: **92,54% matched, `main.dol: OK`**, 17.910 funciones al 100%, `linked`
7,39% (la ventana 12 promocionó 24 unidades). Faltan **189.124 B en 24
SourceLists, repartidos en sólo 202 funciones**, y 104.612 B en 87 unidades de
fuera. Serie de la tanda: 37.440 → 34.880 → 21.888 → **19.644 B** (ronda 12).

## Lo que queda, por dificultad — y lo que eso cambia en tu método

| tramo | bytes | funciones |
|---|---|---|
| 99-100% | 54.956 | 46 |
| 95-99% | 107.300 | 97 |
| 90-95% | 19.472 | 35 |
| 70-90% | 7.256 | 21 |
| <70% | 140 | 3 |

**El 86% está por encima del 95%** y `censo.py` da 0 funciones sin escribir en
casi todas las unidades. Consecuencias:

1. **No ordenes por tamaño; ordena por lo que `dwbody.py` y el mapa de líneas
   digan que es ESTRUCTURA.** Una función al 99% es moneda del asignador: va al
   final, y sólo con el permutador.
2. **El tamaño es el delator más barato.** Si estamos N bytes CORTOS, GCC no izó
   invariantes (bucle escrito con `while(true)`+`goto`) o falta una llamada
   duplicada; si estamos LARGOS, sobra un `goto`, una guarda o un temporal.
3. **La ronda buena cierra 8-12 funciones.** Elige bien las 4-5 primeras.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Lo primero, antes de tocar nada

1. `python scripts/measure.py -o antes_r13_<tu grupo>.json <tus unidades>`. Al
   terminar, **escribe en `despues_r13_<tu grupo>.json`** — NO en `despues.json`,
   tres agentes chocaron ahí en la 12b — y pásame el `--cmp`. **Ese delta es tu
   informe.**
2. `censo.py`, `dwbody.py` (árbol de expansiones inline del DWARF) y `fuse.py`
   sobre tus near-miss. **Las tres juntas dan el diagnóstico; `regmap` solo, no.**
3. **La hermana que ya casa al 100% es el manual de estilo del original.** Y
   **desmiente vedas**: la de la local `times` decía que quitarla empeora, y dos
   hermanas al 100% escriben el miembro directo.

## Las herramientas, y lo que ya sabemos que mienten

| script | aviso medido |
|---|---|
| `regmap` | «SOLO NUESTRA» tiene **dos lecturas** y sólo el asm las separa: si **otra local del original tiene el registro** que el asm usa para ese valor, esa era la que el original reusaba (+1.992 B); si nadie lo tiene, el valor existe y quitar la local **empeora** (`IsUnderVehicle` +56 B y −4,7 pp; `MaybeAddImpoundBox` 96,8→91,6; `eProject` 85,8→75,9). Coge `same[0]` si hay sobrecargas. Y su mapeo nombre→registro **contradice al asm en varias funciones**: verifica |
| `dwbody` | mismo mapeo que `regmap`; el volcado **invierte los declaradores de una misma sentencia** (`int a, b, c;` sale `c, b, a`) |
| `litpos` | caché `r4_c_*.json`; no ve el ORDEN del pool |
| `strseq` | «nosotros la tenemos y el objetivo no» es ruido |
| `fndiff` | imprime el tamaño del objetivo en los dos lados: el juez es el ELF |
| `fuse.py` | falso positivo con inlines anidados |
| `permuter` | `--file` y `--symbol`; **remide su ganador con objdiff** (su marcador dijo 0,947→0,961 y objdiff 97,98→97,96); **rechaza funciones definidas en cabecera**; **no valida semántica**; no sirve en middleware |
| `libdwarf.py cu` | coge la CU equivocada si dos ficheros acaban igual |
| `report.json` | no trae `functions` en 70 unidades: el barrido devuelve vacío, no error |
| el `%` | **puede moverse con `matched` idéntico** tras un build completo (alineamiento de objdiff): no compares porcentajes medidos sin build completo delante |

## Los patrones de la 12b — todos con cifra, todos reutilizables

**De lectura del DWARF:**
- **Un bloque anónimo con rango VACÍO que declara la misma local que su hermano = el original escribió el cuerpo DOS veces** y el cross-jumping borró el primero. `if (A||B){c}` → `if (A){c} else if (B){c}`: mismo código, más refs de `this`, deshace el intercambio. 876 B + 324 B. *(Pero en `ProcessPadsForPackage` el cross-jumping NO fundió: 3.632 contra 3.080. Mide.)*
- **El contador de expansiones del destructor dice cuántas salidas tenía el ámbito**: 7 contra 3 = los tres `goto` no existían. 1.704 B a la primera.
- **El árbol de inlines delata llamadas DUPLICADAS** (una por rama contra una detrás del if/else). 440 B a la primera.
- **`fuse.py` delata la lista de mem-init**: `stw` en la línea de la declaración del ctor. Valió en el ctor de una **struct compartida** (`tCubic1D`), no sólo en la clase. 2.276 B.
- **La inicialización de campos repetida en dos usuarios de una struct es el CTOR de esa struct — y hay que quitarla de LOS DOS** (dejar una copia tiró 100% → 74%).

**Del asm:**
- **N bytes CORTOS = invariantes no izados.** `while(true)`+`goto` no es un bucle para GCC; un `for`/`while` real recupera los `mr` del precabezal (52 → 98%).
- **`a = b = c = 0;` → tres sentencias**: la encadenada relee el lvalue y cambia el reparto.
- **Un `li rX,0` de fusión detrás de un `Search` es una guarda explícita que falta** (`if (handle)`): 83 → 100%.
- **Un `b` a un `beq` compartido = dos `if` con `return` propio, no un `?:`.**
- **Un `lwz` repetido de un puntero global detrás de un store a través de él = el original LEÍA el otro campo antes de escribir.** 340 B, dos líneas.
- **El objetivo carga un miembro en el bloque de ENTRADA = el original lo lee en una local arriba** (`lwz off(r3)` entre las 5 primeras).
- **Un `stw` contra la dirección BASE en vez del desplazamiento de marco = índice VARIABLE** que CSE pliega a 0 (`x[n++]`): vale una referencia más y gana el registro.
- **`&&` cuya rama falsa salta a la SALIDA de la función = `if (...) break;` anidado.** Y **asignación y test en líneas distintas del mapa = `x = f(); if (x != C)`**, no `if ((x = f()) != C)`.
- **Comparar `unsigned int` con `long long` promociona a 64 bits**; el original trunca con cast.
- **GCC 2.9 devirtualiza sobre `this` en un ctor; `T &self = *this;` lo impide.**
- **El orden de declaración en una tirada de asignaciones gemelas decide el reparto de FP** (12 órdenes: uno mejora).

**De método:**
- **Estructura correcta NO implica mismo código**: un cuerpo DWARF exacto salió 304 B peor. Y **sin ordenar cuesta**: un `switch` que necesita la posición 1 de 5.
- **La veda rota de la 12b** (`FastMem::AssignToFree`) cerró con **tres cambios juntos**: cada uno solo era peor. **Si el mapa de líneas pone dos pruebas en la MISMA línea, es un `&&`.**

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada por función.** Una al 99,99% aporta cero.
- **El DWARF dice lo que el original tenía, no lo que puedes borrar.**
- **Trabajo a medias REGRESA.** Si no cierra, revierte y anota la veda **diciendo qué sentencia barriste**, no sólo cuántas formas.
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`. Si una unidad queda LIMPIA en `promote.py`, dímelo: promociono yo en la ventana.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y `UVectorMath.hpp` (**este último debe conservar 2**).
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen**, con cabecera sombra en el scratchpad + `-I` delante si puedes. `UTLVector.h`, `UCollections.h`, `UCOM.h`, `AttribSys.h`: **no las toques** — `Vector<T>::assign` costó 12 B en otra unidad y se revirtió.
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
- **No solapes `build_direct.py` con el permutador.**
