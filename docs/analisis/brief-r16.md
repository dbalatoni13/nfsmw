# Ronda 16 — instrucciones comunes

Estado: **93,22% matched**, 17.960 funciones, `main.dol: OK`, `linked` 7,56%.

## LO QUE CAMBIA: el mapa de lo que queda estaba mal, y ya está corregido

Las rondas 14 y 15 repartieron el trabajo con el **multiconjunto de mnemónicos**
(`auditecho.py`). Tiene un punto ciego medido: **netea los bloques movidos**. De
las 92 funciones que daba por muro, **51 tenían movimiento estructural** que el
multiconjunto cancelaba (`Play__14cSampleWarpper` 22+22, `HolePunchAvoidables`
13+13), y al revés `CompositeSkin(RideInfo*)` salía con delta 2 escondiendo un
bloque de 5 instrucciones movido — que era justo lo que la cerró.

**`scripts/triage.py` mira ahora tres señales** y el mapa queda así:

| | antes | ahora |
|---|---|---|
| Accionable | 89.632 B | **126.192 B** (116 funciones) |
| Muro | 79.116 B | **34.488 B** (43) |

**Salen del muro, tras rondas vetadas**: `Play__14cSampleWarpper` (2.152 B, 44
filas movidas), `HolePunchAvoidables` (2.980, 26), `CompositeSkin(SkinComposite‑
Params*)` (2.128, 20), `RenderString` (1.572, 18), `ProcessPadsForPackage`
(3.080, 10). **Ésas son la veta de esta ronda.**

Cómo leerlo: **`INSERT`/`DELETE` = código que falta o que está en otro sitio**;
`ARG_MISMATCH` = mismo código, otro registro. Un delta simétrico (`n+k` con `n−k`
del mismo mnemónico) es ruido del planificador; **filas asimétricas son código
movido**. Y la regla medida (20 funciones, 11/11): **delta compuesto sólo de
copias (`mr`, `fmr`, `mr.`) = muro**.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Antes de tocar nada

1. `measure.py -o antes_r16_<grupo>.json <unidades>`, y al terminar `despues_r16_<grupo>.json`.
2. **`pctsnap.py` antes y después.** `measure.py` no ve una regresión de near-miss.
3. **`triage.py <tus unidades>`** para tu lista al día con las tres señales.
4. **Compila tu `.o` al scratchpad, no al `build/` compartido.** En la 15 un agente
   borró los `.o` de las SourceLists a mitad de ronda y otro dejó un `.cpp` sin
   compilar un rato: las dos cosas tumban las herramientas de los demás en silencio.

## LA TÉCNICA NUEVA, y es la más potente de la 15

**Cambiar UN cflag sobre el `.cpp` real identifica el pase culpable en una sola
compilación.** No para cambiar los flags —son correctos y está medido— sino para
saber **quién te hace el daño** y buscar la forma de fuente que lo evita:

- `Update__7FEnginelUi` → **`-fgcse`** (su cprop propaga el 0 porque la variable tiene un único set constante que domina el uso).
- `MaybeDoJumpCam` → **`-fcse-skip-blocks`**, y con `-fno-` la función sale **del tamaño exacto del objetivo**.
- `UpdateCameraMovers` → aliasing: quitar el puntero que **sombrea** deja las cargas recargando; tamaño exacto, y una variante queda **a una instrucción**.

Un repro de 40 líneas con los cflags exactos itera después en ~1 s. **Ojo: que el
flag arregle la función no significa que el original no lo llevara** — `FEngine.cpp`
entero con `-fno-gcse` pierde 6.464 B.

## Patrones nuevos de la ronda 15 — todos con cifra

**De control de flujo:**
- **El bloque fundido del epílogo CUENTA LAS SENTENCIAS**: `lmap` imprime una línea por cada `return` que el cross-jumping metió ahí. Si el objetivo lleva N y tu fuente tiene N−1, **falta una sentencia**. Valió **2.200 B** en una ronda. Generaliza a cualquier bloque fundido.
- **`if (A || B) { cuerpo }` con un solo cuerpo se emite en línea y con la segunda condición invertida; con el cuerpo DUPLICADO en dos `if`, el cross-jumping los funde fuera de línea.** Firma: **`bne-1, beq+1` con el mismo tamaño**. Cerró 1.232 B.
- **GCC 2.9 borra los `case` de los EXTREMOS de la lista cuyo destino es el del `default`; uno en medio sobrevive.** Delator: una constante izada que nadie compara.
- **`cmplwi K; ble DEFAULT` prueba que NO hay un `case` en K** — sirve para descartar «falta el case K» sin compilar.

**De C++, y por eso valen en cualquier sitio:**
- **`i++` vs `++i` sólo muerde en iteradores de CLASE** (el postfijo materializa una copia en pila); en punteros crudos da asm idéntico.
- **`iterator i = begin();` emite UN store; `i = begin();` emite DOS.**
- **Un `T *p = this;` local ROMPE el CSE de los miembros** (+2,86 pp).
- **Un temporal flotante intermedio cuesta un `fmr` por uso**; el primero y el último coalescen solos.
- **`a = b = f();` vs `a = f(); b = a;`** — un `fmr` de más detrás de un `bl` es una asignación encadenada.
- **Una variable de resultado sin inicializador asignada en las dos ramas NO es lo mismo que dos `return`**: con `return`, GCC siempre iza el `li r3,0`.

## Trampas — todas medidas, todas han costado dinero

| | |
|---|---|
| **`bench.py`** | puede dar un **100% FALSO** y su columna «bytes» es la del OBJETIVO |
| **`measure.py`** | **no compila**: pasa `build_direct.py` antes |
| **el `.o` rancio** | con varios agentes, un `.o` puede venir de otro árbol. **Bórralo antes de medir** |
| **finales de línea** | son mixtos **POR LÍNEA**, no por fichero. Usa `\r?\n` o trabaja por líneas; con `str.replace` las variantes se leen como «sin cambio» |
| **la medida vacía** | si tu barrido dice «no cambia nada», comprueba que la medida **no está vacía** |
| **el repro** | puede ser representativo y aun así llevarte al sitio equivocado: **mira si produce el código del objetivo, no si se parece**. Una «grieta» verificada en repro costó −11 pp al aplicarla de verdad |
| **«está rancio»** | remídelo. «Ese comentario ya no se cumple» resultó falso y habría costado 1,25 pp |
| `regmap` | empareja mal con sobrecargas. Y **«SOLO NUESTRA» no es «sobra»**: quitar una local así empeoró tres funciones |
| `litcheck` | falsos positivos en el lado del OBJETIVO |
| `permuter` | **cero cierres en las rondas 13, 14 y 15** |

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada.** Una función al 99,99% aporta cero.
- **Si no cierra, revierte y anota la veda diciendo QUÉ SENTENCIA barriste.**
- **NO inventes locales ni claves registros para cuadrar un número.** Se han revertido dos casos (456 B de asm a mano y un `pad16` de marco). Los seis `__asm__("")` del árbol están auditados y **los seis sostienen código**: no los toques sin medir.
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`. Si una unidad queda LIMPIA en `promote.py`, **pásale `scripts/trypromo.py`**.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y `UVectorMath.hpp` (**dos bloques**). Hay un port vivo que compila código real del árbol.
- **Cabecera compartida = A/B por objetos sobre TODAS las unidades que la incluyen.**
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
