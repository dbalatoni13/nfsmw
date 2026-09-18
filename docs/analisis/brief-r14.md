# Ronda 14 — instrucciones comunes

Estado: **92,90% matched**, 17.944 funciones, `main.dol: OK`, `linked` 7,56%.
Faltan **178.660 B en 33 SourceLists**, repartidos en **163 funciones**.

## LO QUE CAMBIA EN ESTA RONDA: no todas las near-miss son trabajo

`scripts/auditecho.py` compara el **multiconjunto de mnemónicos** de cada función
contra el objetivo. El resultado parte lo que queda en dos mitades que **no se
atacan igual**:

| | bytes | funciones | qué es |
|---|---|---|---|
| **DELTA de mnemónicos** | **99.048** | **72** | **falta o sobra código**, y el opcode dice cuál |
| Mismo multiconjunto | 79.612 | 91 | mismas instrucciones, otros registros u otro orden |

**Todos los cierres identificables de la ronda 13 salieron del grupo con delta.**
Y los dos agentes que dieron **0 B** gastaron su presupuesto en el otro.

Por eso tu encargo trae **dos listas**: las funciones **con delta, ordenadas por
cuántas instrucciones bailan**, y las de **MURO**, que son las que no debes tocar.

**La regla, y es dura**: si una función está en la lista de MURO, **no gastes
presupuesto en ella** — ni permutador, ni barridos de forma. Si crees que una
está mal clasificada, **demuéstralo con el asm y dímelo**: eso vale más que los
bytes. Y si cierras las tuyas con delta y te sobra tiempo, **entonces** baja al
muro, empezando por las de menos bytes.

**El delta también dice qué buscar.** `mr±N` = una copia de registro de más o de
menos (¿una local que no existe? ¿un inline que falta?). `lis±N` = un `@ha` izado
o no izado. `b/blt/beq/bne` cruzados = el cross-jumping colocó un bloque en otro
sitio, o un `&&` es un `if` anidado. `lwz±N` = se relee o se cachea un miembro.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Lo primero, antes de tocar nada

1. `python scripts/measure.py -o antes_r14_<grupo>.json <tus unidades>`. Al
   terminar, **escribe en `despues_r14_<grupo>.json`** — NO en `despues.json`.
2. `python scripts/auditecho.py <tus unidades>` para ver tu delta actualizado.
3. **`scripts/lmap.py <unidad> <símbolo>`** (nuevo): funde `debug_lines.txt` con
   el `.s` del troceador y da, instrucción a instrucción, **la línea de fuente del
   original**. Tres de los cinco cierres de zPlatform salieron de ahí.
4. **`scripts/dwblocks.py`** (nuevo): rangos DWARF **normalizados**. `dwbody`
   imprime absolutos y por eso no se podían comparar. Delata un inline con rango
   no vacío en el original y vacío en el nuestro (= plegamos algo que él emite),
   una llave que abre N instrucciones antes, y bloques anidados contra hermanos.
5. **`scripts/pctsnap.py`** (nuevo): instantánea del **porcentaje por función**.
   `measure.py` no ve una regresión de near-miss: una función puede caer del
   95,5% al 75,7% **sin mover un byte**. Ya dejó código malo en HEAD una ronda.

## Herramientas: lo que ya sabemos que mienten

| script | aviso medido |
|---|---|
| **`bench.py`** | **puede dar un 100% FALSO**: su verificación compara sólo TAMAÑOS. Un ctor midió 384 B / 100,0000% / 0 diffs en su banco y **95,48% en la unidad real**. Verifica cada cierre contra el `.o` de la unidad |
| barridos propios | **si no compruebas el código de salida del compilador, un fallo de compilación se lee como «IGUAL»** (libdiff no imprime nada). Ya coló un ganador falso. Y `str.replace` sin ancla única parchea la ocurrencia equivocada |
| `regmap` | «SOLO NUESTRA» tiene **dos lecturas**: si otra local del original tiene el registro que el asm usa, la nuestra no existe (+1.992 B); si no, quitarla **empeora** (−4,7 pp y +56 B). Coge `same[0]` si hay sobrecargas, y **`--list` NO regenera el volcado**: invócalo con función |
| `dwbody` | coge `cands[-1]` del glob → volcado rancio tras recompilar |
| `permuter` | **cerró CERO funciones en toda la ronda 13.** Remide su ganador con objdiff siempre; rechaza funciones definidas en cabecera; no valida semántica; no sirve en middleware |
| el `%` | se mueve con `matched` idéntico tras un build completo (alineamiento de objdiff) |

## Los patrones de la ronda 13 — todos con cifra

**De lectura del asm:**
- **El signo del producto vectorial delata el ORDEN de los operandos**: un `fmsubs` con los factores intercambiados dice que `this` y el argumento están al revés. Una línea, 95 → 99,99%.
- **`while(true)`+`continue` impide izar invariantes, y ahí el TAMAÑO NO lo delata** (964/964). Lo delata la **dirección de la cabecera del bucle** en `fndiff`.
- **El literal partido**: si una función mezcla `0.0f` con el hack `lbl_XXXXXXXX` del mismo valor, GCC ve **dos símbolos** y no iza el `@ha` de ninguno. Unificar **hacia el símbolo externo** hizo desaparecer a la vez el `lis` de más y el `stw`/`lwz` que faltaban.
- **El orden del pool lo decide qué sumando va a la IZQUIERDA**, y es el mismo operando que GCC funde en el `fmadds`: las dos pistas se mueven juntas.
- **Dos `addi` idénticos en las dos ramas de un if/else + UN `mr` en la junta** = el incremento está en la **cabecera del `for`**; GCC copia el latch dentro de cada rama y **el mapa de líneas miente** sobre dónde se escribe el `i++`.
- **Un `beq` que salta POR ENCIMA del bloque `else`** = el `else` cuelga del `if` **exterior**.
- **`li 0/1` en registro salvado + DOS `cmpwi` contra él** = es `f = (a && b)`, no un `if`.
- **`--p; *p` se funde en `lwzu`; `p[-1]` no.**

**De lectura del DWARF:**
- **La lista de locales es especificación**: una de más = la forma del bucle es otra; una de menos = falta una variable que comparte registro. +668 B en tres funciones.
- **Un inline que no emite código** es un test que GCC borró porque ambas ramas devuelven lo mismo — **devolverlo cambia la planificación**. +92 B.
- **El TIPO decide el orden de `expand_assignment`**: con tipos iguales GCC evalúa la llamada primero (`@ha` en volátil); con una conversión de por medio calcula la dirección antes y **el `@ha` cruza la llamada a un callee-saved**.
- **Un bloque anónimo de rango VACÍO que declara la misma local que su hermano** = el original escribió el cuerpo **dos veces** y el cross-jumping borró el primero.
- **Las locales muertas cuentan**: dos `float = 1.0f` sin usar metían la dirección del literal en un salvado antes de `bSin`/`bCos`.

**De la lista de mem-init, que va en los dos sentidos:**
- Stores en la **línea de declaración** del ctor = mem-init (+156 B, +2.276 B).
- Mem-init cuyo store el objetivo pone **después de la primera llamada** = asignación en el cuerpo (+384 B).
- **Y el ORDEN escrito manda**: GCC 2.9 emite en orden de escritura, no de declaración.

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada por función.** Una al 99,99% aporta cero.
- **Trabajo a medias REGRESA.** Si no cierra, revierte y anota la veda **diciendo qué sentencia barriste**, no cuántas formas.
- **NO escribas ensamblador a mano ni claves registros** (`register x __asm__("r27")`, `__asm__ volatile("li ...")`). Eso no cierra una función, la falsifica. Se revirtió un caso de 456 B en la ronda 13 y el árbol quedó auditado y limpio: no lo reabras.
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`. Si una unidad queda LIMPIA en `promote.py`, **dímelo y pásale `scripts/trypromo.py`**, que prueba la promoción enlazando de verdad sin tocar el árbol.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y `UVectorMath.hpp` (**este último debe conservar 2**).
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen.** `UTLVector.h` y `FEDatabase.hpp` se tocaron en la 13 **con control medido de 10 y 5 unidades**: ése es el listón.
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
