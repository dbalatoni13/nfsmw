# Ronda 9 — instrucciones comunes

Estado al empezar: **89,41% matched, 6,97% linked (293/583), `main.dol: OK`**.
Faltan **304.424 B** en las SourceLists. Tu unidad tiene su parte.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Lo primero, antes de tocar nada

1. **Lee `docs/PLAYBOOK.md`** — 720 patrones medidos. La mayoría de los cierres de
   la ronda anterior salieron de ahí. Si crees que una función está bloqueada,
   recorre la sección que le toca antes de darla por perdida.
2. **`python scripts/measure.py -o antes.json <tus unidades>`** para tu línea
   base. Al terminar, `--cmp antes.json despues.json`. **Ese delta es tu
   informe**: sin él no puedo verificar nada.

## Las herramientas, por orden de rentabilidad

| script | para qué |
|---|---|
| `bench.py <unidad> <fichero.cpp> --medir` | **el ciclo en 1,4 s en vez de 34.** Úsalo para iterar; construye la unidad entera sólo para confirmar |
| `litpos.py` / `litcheck.py` | **OBLIGATORIO.** objdiff **no compara** qué constante carga un `lfs`, a qué símbolo salta un `bl`, ni qué cadena apunta un puntero |
| `mnemo.py` | multiconjunto de mnemónicos: mismo = asignador, delta = falta código |
| `arboljump.py` | árbol de saltos — ve el `switch` que `mnemo` no ve |
| `auditecho.py` | audita techos documentados: cuáles son falsos |
| `regmap` | decide permutador guiado (si señala algo) o **CIEGO con el catálogo entero** (si dice IDÉNTICO) |
| `fuse.py`, `plan.py` | el DWARF y el mapa de líneas fundidos con el asm: escribir se convierte en transcribir |

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada por función.** Una al 99,99% aporta **cero
  bytes**. Prefiere cerrar dos pequeñas a dejar tres al 99%.
- **Trabajo a medias REGRESA**: baja a las vecinas. Si una vía no cierra,
  revierte y anota la veda con el porcentaje medido.
- **objdiff puede puntuar MÁS ALTO la versión EQUIVOCADA.** El porcentaje no es
  la verdad; el DOL sí.
- **La comparación POSICIONAL de literales miente** cuando hay una instrucción de
  más (desplaza todas las ranuras). La de **SECUENCIA** —lista ordenada por
  función— acertó 4 de 4 sin falsos.
- **No toques** `configure.py`, `config/GOWE69/*`, `splits.txt` ni el troceado:
  eso es trabajo de ventana y regenera `obj/` y `asm/` para todos.
- **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
  `bMath.hpp` y `UVectorMath.hpp` (**este último debe conservar 2**).
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen**, antes y
  después. Un `#ifdef` encendido sólo donde toca es una solución legítima.
- **El disco es el recurso escaso.** Borra cada volcado JSON en el acto.

## Lo que más ha rendido últimamente

- **Los static-init.** Son las funciones más grandes del árbol y las más fáciles:
  es una lista de inicializadores, no un muro. Tres se cerraron en la ronda
  anterior (4.464 B, 1.956 B, 1.304 B) y uno pasó de 46,86 a 100%.
- **Escribir desde cero** con `fuse.py` + el DWARF: la mayoría casa a la primera.
- **Las cabeceras que mienten**: accesores vacíos, enums cambiados, arrays con
  stride mal, virtuales que no son `= 0`. Está a un `grep`.
- **Los literales.** En la ronda anterior aparecieron **cuatro cadenas mal, una de
  ellas en una función que marcaba 100%**, y cinco parejas de coordenadas
  cruzadas que `matched_code` no movió ni una centésima.
