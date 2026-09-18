# Brief de la ronda 51

Sustituye a `brief-r50.md`. Léelo entero antes de tocar nada.

## Estado

    python scripts/estado.py

Eso imprime el frente y **es lo primero que tienes que correr**. No rehagas el
censo: la herramienta existe desde hoy y ya trae las métricas corregidas.

    CODIGO   3.909.216 / 3.946.048 B   99,0666 %   faltan 32 funciones, 36.832 B
    DATOS    0 B de trabajo real
    ENLACE   506 / 616 unidades, techo REAL 544 (71 comodines + prodg_fixes)
             QUEDAN 38 unidades: 23 SourceLists + 15 de biblioteca

DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

El plan completo está en **`docs/ESTRATEGIA.md`**. Léelo: explica por qué tu
encargo es el que es.

## Tres correcciones de encuadre que cambian el trabajo

1. **El `Data 40 %` no mide datos.** Mide cuántas unidades enlazan. Las 247
   enlazadas van al 97,9 % y lo que les «falta» es nomenclatura, porque una
   unidad enlazada da el DOL correcto por definición. **No justifiques nada con
   esa cifra.**
2. **El techo de `linked` es 544, no 616.** 71 de las que faltan son comodines
   `auto_*`: no son fuente y no pueden enlazar nunca.
3. **Las 32 funciones abiertas están TODAS en unidades que no enlazan.** Ninguna
   es trabajo que sólo mueva el porcentaje.

Y el corolario que decide el reparto: **13 SourceLists tienen el código al
100 % y 1.748.504 B parados**. En ésas **no hay codegen que hacer**. Si tu
encargo es una de ellas, tu frente es el orden de emisión o el `.rodata`, no las
funciones.

## Reglas duras

1. **Nunca lances un `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el árbol.
2. **No hagas commits ni `git add`.**
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Proponlo con
   la medida; los aplico yo en la ventana.
4. **Quédate en tu territorio.** Si la palanca está en una cabecera compartida,
   proponla; no la apliques.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. **BORRA TUS VOLCADOS Y OBJETOS DE PRUEBA AL TERMINAR.** Cada volcado RTL son
   20-30 MB y el disco se ha llenado dos rondas seguidas; un disco lleno se ve
   como un `cc1plus: I/O error` que parece un fallo de compilador.
7. **Cero bytes con `asm` puesto es deuda.**

## Reglas nuevas de la r51

8. **Ninguna sonda de un solo uso en `scripts/`.** 111 de los 217 ficheros de
   `scripts/` son sondas que un agente escribió para una ronda y nadie borró.
   Las tuyas van al scratchpad y se borran. Lo que merezca quedarse se queda
   **con docstring**, y sale solo en `docs/HERRAMIENTAS.md`.
9. **Antes de construir una herramienta, mira si ya existe:**
   `python scripts/indice.py` regenera el índice y `docs/HERRAMIENTAS.md` lista
   las 106. La r48 entera se construyó comparando OBJETOS mientras
   `scripts/linkdelta.py` llevaba dos días en el árbol contestando bien.
10. **Los andamios caducan con el árbol.** `CullParts` cerró quitando un
    `asm("" : : : "r11")` que **tres rondas distintas** (r36d, r48, r49) habían
    medido como imprescindible. **Cuando un cambio de árbol mejore una función,
    re-mide TODOS sus `asm` de andamio antes de dar el resultado por bueno.** Y
    una veda con fecha anterior al último cambio del árbol no es una veda.
11. **Los finales de línea se miden con Python, no con `grep`.** En este Git Bash
    `grep -c $'\r'` cuenta todas las líneas y siempre dice CRLF. Lo correcto:
    `python -c "d=open(f,'rb').read(); print(d.count(b'\r\n'))"`. `splits.txt`,
    `symbols.txt` y `configure.py` son **LF puros**; sólo `keep.lst` es CRLF.
12. **Exígele a tu métrica un control que TENGA que cambiar.** Si un ensayo sale
    «idéntico», demuestra primero que el ensayo llegó a compilarse distinto.

## Herramientas

    python scripts/estado.py                      # el frente entero
    python scripts/fncmp.py <unidad> [<Nombre>]   # que funciones NO son identicas
    python scripts/fndiff.py <unidad> <simbolo>   # el diff instruccion a instruccion
    python scripts/triaje.py <unidad>             # separa PERMUTADOR / local / estructura
    python scripts/linkdelta.py [<unidad>]        # la distancia REAL al enlace
    python scripts/permorden.py <unidad>          # la permutacion del orden de emision
    python scripts/promote.py <ruta/unidad>       # que le falta para promocionar
    python scripts/trypromo.py <ruta/unidad>      # prueba la promocion sin tocar el arbol
    python scripts/dolwhere.py <unidad>           # DONDE difiere el DOL
    python scripts/deadlink.py <unidad> [--keep]  # literales que el enlazador estripa
    python scripts/dupstr.py [<unidad>]           # cadenas que emitimos de mas
    python scripts/rtldump.py <unidad> <fn> [--extra "-DFOO"]   # volcados RTL
    python scripts/schedtrace.py                  # la traza del planificador

`promote.py` y `trypromo.py` quieren la **ruta completa** de la unidad
(`Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode`), no el basename: con el
basename responden «sin objeto construido» y parece otra cosa.

`docs/HERRAMIENTAS.md` tiene las 106 con una línea cada una. `docs/PLAYBOOK.md`
tiene el catálogo de palancas, pero pesa 442 kB: ve a la sección que te toca, no
lo leas entero.

## El reparto

| agente | encargo |
|---|---|
| `sinfuente` | las 6 unidades de biblioteca que no tienen `.c` |
| `datos6` | las 6 que están al 100 % de código y las bloquea ≤32 B de dato |
| `cam` | las 5 funciones de zCamera (10.812 B) |
| `ecs` | las 4 de zEcstasy (5.180 B) |
| `steer` | las 6 de `steering` y las 2 de `madidct` |
| `orden` | el orden de emisión de `zMain` (32 desplazadas) y `zLua` (68) |
| `zsim` | `zSim`: 4.688 B en el DOL, concentrados en ENTRADAS DE VTABLE |

Doce unidades en los dos primeros encargos. Si salen, `linked` pasa de 506 a
~526 sobre un techo de 544.

`zSim` entra de septimo con diagnostico nuevo: su codigo esta al 100 % (0 de
402 funciones distintas) y su permutacion de orden de emision a cero, y aun
asi el DOL difiere en 4.688 B **casi todos en entradas de vtable**. Lo que la
r49 dio por «secciones identicas» era sobre TAMANOS; el contenido difiere.

## Lo que me quedo yo

El **paquete del vocabulario compartido de `.rodata`** (`r50-rodata.md` §6.3,
**corregido en `r51-jf-prefijo.md`**). Ojo con la version de la r50: decia «un
solo mecanismo toca 21 unidades» y eso esta **refutado**. Lo medido es que el
vocabulario compartido existe y pesa 23.752 B, pero **el orden comun son solo
las 14 primeras cadenas (213 B)**: de las 43 que salen en 20+ unidades, 18
unidades llevan las 43 y entre ellas hay **catorce ordenes distintos**. O sea un
mecanismo de ~5.300 B mas 21 transcripciones, que `scripts/prefijotu.py` genera.
Bloqueado por `keep.lst`, que es coordinacion mia. **No lo toqueis.**

También son míos: la frontera `zGameModes`/`zGameplay` de `splits.txt`, y
cualquier cambio en `config/`.
