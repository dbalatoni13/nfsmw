# Ronda 10 — instrucciones comunes

Estado al empezar: **90,61% matched, 7,12% linked (301/590), `main.dol: OK`**.
Faltan **258.144 B** en las SourceLists y ~112 kB fuera. La ronda 9 subió
**+37.440 B y +94 funciones sin que bajara ninguna unidad**; esto va bien.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Lo primero, antes de tocar nada

1. **Lee `docs/PLAYBOOK.md`** — **777 patrones medidos**, y la mayoría de los
   cierres de la ronda 9 salieron de ahí. Si crees que algo está bloqueado,
   recorre su sección antes de darlo por perdido.
2. **`python scripts/measure.py -o antes.json <tus unidades>`**. Al terminar,
   `--cmp antes.json despues.json`. **Ese delta es tu informe.**

## Las herramientas

| script | para qué |
|---|---|
| `bench.py <unidad> <fichero> --medir` | el ciclo en 1,4 s en vez de 34 |
| `litpos.py` | valores del pool. **Borra su caché `r4_c_<unidad>.json` o miente**: dio 494 falsos en zFe2 |
| `strseq.py` | cadenas. objdiff **no compara** a qué apunta un puntero |
| `mnemo.py` / `auditecho.py` | multiconjunto de mnemónicos (ya no cuentan ranuras vacías) |
| `arboljump.py` | árbol de saltos: ve el `switch` que `mnemo` no ve |
| `globalini.py` | el símbolo `_GLOBAL_.I.`, 44 B por unidad |
| `censo.py` | qué está **sin escribir** |
| `regmap`, `fuse.py`, `plan.py` | el DWARF fundido con el asm |

## Lo que más rindió en la ronda 9 — repítelo

- **Transcribir desde `plan.py`**: FEngFont pasó de 5 near-miss a **5 cierres
  seguidos**. Es el frente más productivo del proyecto.
- **Los `__static_initialization_and_destruction_0`**: cuatro cerrados. Un
  simulador de registros sobre el `.s` del troceador sacó **~140 inicializadores
  de zCamera en una pasada** (26,40 → 97,11%).
- **Las cabeceras que mienten**: `tEnvelope(p,n)` hacía `p = pPoints;` en vez de
  `pPoints = p;` y el compilador borraba quince stores. zMisc entera al 100%.
- **Los literales y las cadenas**: en una sola ronda salieron ocho cadenas mal
  —una en una función que marcaba 100%—, `UCrc32("smackable")` en minúscula, y
  `MPH2MPS` con el divisor cambiado. **NFSMW hashea en runtime: eso no da error,
  da un `Find` que falla en silencio.**

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada por función.** Una al 99,99% aporta **cero**.
- **El DWARF NO lista los temporales que GCC coalesce**: «esta local no está en
  el volcado» **no** autoriza a borrarla (medido: 99,27 → 92,47%).
- **«SOLO NUESTRA» de `regmap` es una hipótesis, no un veredicto**: de tres
  casos, uno mejoró y dos empeoraron.
- **Las locales muertas NO reordenan el pool**; la regla del `float zero = 0.0f`
  sólo vale cuando la local **se usa**.
- **Trabajo a medias REGRESA.** Si una vía no cierra, revierte y anota la veda
  **con el porcentaje medido**.
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`: es trabajo de
  ventana y regenera `obj/` y `asm/` para todos.
- **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
  `bMath.hpp` y `UVectorMath.hpp` (**este último debe conservar 2**).
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen.** Si
  rompes la compilación de otro, le cuestas media ronda: `UTLVector.h` tumbó
  zSim/zPhysics/zMain **dos veces** en la ronda 9.
- **El disco es el recurso escaso.** Borra cada volcado JSON en el acto.
