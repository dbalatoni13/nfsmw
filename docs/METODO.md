# El método

Desde el 2026-09-17 este proyecto mide y decide como upstream. Es un cambio de
criterio, no de herramientas, y contradice buena parte de lo que hay escrito en
`docs/analisis/` de rondas anteriores: si un informe viejo dice otra cosa, manda
este documento.

## La regla

> Un 80 % real y portable vale más que un 100 % con pines que solo compila en
> GameCube.

El porcentaje no es la métrica. La métrica es **código real que compila en las
cinco versiones**. De ahí salen las tres consecuencias que gobiernan el día a
día:

1. **Cero `asm`.** Ni pines de registro (`register T x asm("rN")`), ni barreras
   (`asm("")`), ni bloques de `.rodata` escritos a mano, ni alias
   `__asm__("lbl_…")` para cuadrar un nombre. No es una preferencia estética:
   el pin es **error duro** en el compilador de PS2 y en el de Xbox 360, así que
   un archivo con pines no es decompilación portable, es una función que solo
   existe en una plataforma.
2. **El flag `Matching` se ignora.** Es un estado de `configure.py`, no una
   medida, y sobrevive al cambio de fuente que lo justificaba. Lo que cuenta es
   lo que dice objdiff hoy sobre el objeto de hoy.
3. **Un informe por versión.** No basta con GameCube.

## Lo que NO se hace

Copiado de la lista de upstream, porque describe exactamente los atajos que
inflan un porcentaje sin decompilar nada:

- asignar registros a mano para forzar un match;
- acceder a los miembros de una struct por desplazamiento de puntero;
- sustituir cadenas y flotantes por valores inventados;
- renombrar parámetros y variables al azar;
- reimplementar el mismo código varias veces y arreglar la compilación con
  guardas de include;
- redeclarar al principio de cada fichero todas las funciones que usa, a menudo
  mal;
- convertir una variable al tipo que ya tiene.

Y una regla propia, para cuando hay que sustituir un andamio por C: **solo
constructos plausibles**. Nada de `#line`, `__attribute__((aligned))` ni
`section`; lo que se recorta se anota; y un nombre dudoso solo entra con
evidencia de orden.

## Cómo se mide

```bash
python scripts/pctall.py
```

Configura, compila y genera un informe de objdiff por cada versión, y termina
dejando GameCube configurada otra vez. Con `--solo-informe` no compila: mide los
objetos que ya haya.

**Una cifra no vale si la línea no dice «0 objetos no compilan».** Hay tres
formas medidas de medir mal, y las tres han pasado en este proyecto:

| trampa | qué pasa |
|---|---|
| objdiff cuenta al **100 %** una unidad marcada `complete` que no tiene objeto base | una versión donde medio árbol no compila daba un porcentaje altísimo |
| si falla el **troceado**, `configure.py` no reescribe `objdiff.json` y queda el de la versión anterior | se mide una versión como si fuera otra |
| `configure.py --version V` puede terminar **OK y no emitir ni una regla** de compilación | un 0 % que en realidad es «no se ha compilado nada» |

`pctall.py` tapa las tres. Cualquier otra forma de medir hay que comprobarla
contra él.

## Las cinco versiones

| versión | plataforma | compilador |
|---|---|---|
| `GOWE69` | GameCube | ProDG / GCC 2.95.3 (`ngccc`) |
| `SLES-53558-A124` | PlayStation 2, prototipo alpha 124 | EE-GCC 2.9-991111 (el middleware, con el de SN 2.95.3) |
| `SLUS-21351` | PlayStation 2, versión de venta | ídem |
| `EUROPEGERMILESTONE` | Xbox 360, prototipo | MSVC PPC 14.00.2110 |
| `SPEED_EXE_1_3` | PC | MSVC 7.1 (13.10.3077) |

Para compilar una sola: `python configure.py --version <V>` y después
`python -m ninja`.

Aviso vivo: la rama de PS2 de `configure.py` es **una sola para las dos
versiones**, y define `-DEA_BUILD_A124` y `-DMILESTONE_BUILD`. Eso es correcto
para el prototipo y **falso para `SLUS-21351`**, que es la versión de venta: su
ELF no tiene el `bMalloc` de cuatro argumentos (tamaño, fichero, línea, params),
que solo existe en builds milestone, y el de `SLES` sí. Mientras no se separen,
la cifra de SLUS está medida contra un build que no es el suyo.

## Qué cuenta como terminado

Una unidad está hecha cuando **su objeto casa y la misma fuente compila en las
cinco versiones**. En ese orden: primero que compile en todas, después el
porcentaje. Una función al 99,99 % aporta cero bytes a `matched_code`, así que
los near-misses valen su tamaño íntegro y conviene cerrarlos; pero un 100 %
apoyado en un andamio no vale nada, porque hay que quitarlo después y al
quitarlo se cae.

## Dónde está el resto

- `docs/PROCEDIMIENTO.md` — cómo se trabaja: del censo del hueco a la causa
  común, y cómo se confirma con el binario antes de tocar el código.
- `docs/PLAYBOOK.md` — el manual de codegen de GCC 2.9 para este target. Se lee
  por rangos con `docs/PLAYBOOK-MAPA.md`, nunca entero.
- `docs/TRAMPAS.md` — dónde miente cada herramienta, con el caso que lo destapó.
- `docs/HERRAMIENTAS.md` — qué hace cada una. Generado con `scripts/indice.py`.
- `docs/DATOS-DEBUG.md` — qué sabemos del fuente original y qué herramienta lee
  cada cosa.
- `README.md` — cómo se monta el entorno y se compila cada versión.
