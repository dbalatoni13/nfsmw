# Brief de la ronda 54 — ronda de DIAGNÓSTICO

Sustituye a `brief-r53.md`. Léelo entero antes de tocar nada.

**El entregable de esta ronda es un mecanismo, no bytes.** Si vuelves con cero bytes y el
nombre de lo que bloquea, la ronda ha salido bien. Si vuelves con bytes y sin mecanismo,
hemos vuelto a comprar una papeleta.

## Estado

    python scripts/estado.py

    HECHO    908.452 / 3.946.048 B   23,02 %   <- codigo que sale de NUESTRA fuente
    CODIGO   3.912.360 / 3.946.048 B  99,1463 %   faltan 28 funciones reales
    ENLACE   518 / 616, techo REAL 544

DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

### La cifra que importa es el 23 %, no el 99 %

`complete_code_percent` mide **el código que sale de nuestra fuente**. Las otras dos
engañan: `matched` cuenta funciones que casan aunque su unidad no enlace —y por tanto no
aporten un byte al binario— y `linked` cuenta unidades sin pesarlas. Ese 23 % es lo que
explica por qué las últimas catorce asignaciones de agente devolvieron tan poco.

Y dos correcciones más de la r53: el techo es **544** (no 545: `prodg_fixes` tampoco
enlaza), y de las 29 «funciones» abiertas **28 son funciones** — `pad_00_8000348C_init` son
20 B de **relleno de alineación del contenedor DOL**, ceros que nuestro DOL ya emite. O sea
que **`matched` al 99,99949 % es el 100 % real**.

## El frente, partido por lo que bloquea

| bloque | uds | código que guarda | funciones |
|---|---:|---:|---|
| **A — código YA al 100 %** | 13 | **1.652.576 B** | **ninguna** |
| **B — a UNA función** | 6 | 690.484 B | 6 fn, 5.728 B |
| **C — a 2-6 funciones** | 7 | 694.516 B | 22 fn, 27.940 B |

## EL CUELLO DE BOTELLA — y es el encargo principal

**No sabemos qué separa a una unidad con el código al 100 % y las nueve secciones a delta 0
de un `DOL OK`.**

`zMain` es el espécimen: `linkdelta` **IGUAL en las nueve secciones**, `fncmp` **0 de
1.380**, `permorden` **11 de 1.380** — todas las medidas que tenemos leen cero — y
`dolwhere` sigue dando **17.269 B** de contenido distinto.

Dato duro de la r53: `trypromo` se corrió por primera vez sobre **las trece del bloque A** y
salieron **trece `DOL ROTO`**. Ninguna estaba lista. No es «algunas ya pasan»: es que
ninguna medida de las que tenemos dice cuál está cerca.

**Pista de partida, medida por `cerca1`**: a zMain los símbolos descolocados le bajaron de
1.619 a 147 (138 de `.text`, 9 de `.rodata`, **0 de `.data`**), y
**`~UTL::Collections::Listable<ActionQueue,20>::_List` sale 9.204 B tarde y arrastra 97 de
esas 138**. O sea que el residuo no es difuso: cuelga en gran parte de un destructor.

## La caja de herramientas

    python scripts/estado.py                # el frente
    python scripts/movidos.py <ruta/unidad> # que simbolos caen en otra DIRECCION,
                                            # separando ARRASTRE de PERMUTACION
    python scripts/permorden.py <u>         # orden de las FUNCIONES
    python scripts/rodorden.py <u>          # orden de las CADENAS
    python scripts/vtord.py <u>             # orden de las VTABLES
    python scripts/dolwhere.py <u>          # los bytes del DOL (solo si los tamanos cuadran)
    python scripts/linkdelta.py <u>         # TAMANOS de seccion en el enlace
    python scripts/trypromo.py <ruta>       # el veredicto: DOL OK / ROTO. RUTA COMPLETA
    python scripts/prefijochk.py            # ningun rango con dos prefijos de TU
    python scripts/keepchk2.py              # entradas de keep.lst que nombran fantasmas

`docs/HERRAMIENTAS.md` (generado) tiene las 110. `docs/PLAYBOOK-MAPA.md` indexa el manual,
que pesa 442 kB: **no lo leas entero**.

## Las trampas, todas con su caso

- **`linkdelta` a cero NO significa cerca.** Compara TAMAÑOS. `zSpeech` daba `rodata−8` con
  83 de 703 funciones en otra dirección; `zAI` daba `−240` con 781 de 1.030. Usa `movidos`.
- **El DOL rellena cada sección a 32 B**, así que un `−24` puede estar ya bien y un `+8` no.
- **`dolrod` y `dolwhere` imputan al símbolo equivocado** si la sección está descolocada:
  nombran el siguiente que encuentran.
- **El `.data` casi nunca es un frente**: de 2.568 B que «faltaban» en siete unidades, 0 B
  eran dato con nombre. Y parte del diff es **la sombra del `.rodata`**. Excepción medida:
  `zEAXSound2`, cuyos 384 B sí son 26 huecos con contenido.
- **El fuzzy manda al sitio equivocado**: cuenta filas con `fndiff`.
- **Una línea en blanco no puede cambiar un byte**, y **el mapa de líneas dice DÓNDE mirar,
  nunca QUÉ poner** — hoy se revirtió una regresión de ocho rondas por ignorar esto.
- **Sombrear una cabecera con `-I` no funciona si el include es RELATIVO.**
- **Los andamios caducan — pero no todos.** Re-medir, no suponer.

## Palancas del catálogo, las recientes

- **El primer de pool**: una `static inline` MUERTA con `switch` de `return "literal"` emite
  los literales en ese punto del fichero por cero bytes. **Sólo mueve hacia ATRÁS**, y no
  vale para floats.
- **Dar nombre al operando intermedio para poder anclarlo**: `i / 2` son tres insns y el
  resultado del `add` no existe en C. Nombrarlo cerró una veda de ocho rondas.
- **La posición de la declaración**: `update_equiv_regs` sólo puede sustituir si el pseudo
  **cruza de bloque** (`REG_BASIC_BLOCK < 0`). Sacar un `const float` del `if` que lo usa
  cerró `TerrainVelocityNoise`.
- **Entrada `"r"(x)` en un `asm` que YA existe**, no el clobber: sube `n_refs` y con ello
  `QTY_CMP_PRI` sin gastar ranura. **Y es gratis sólo si el valor ya está vivo.**
- **`-fsched-verbose-5` sobre una mini-TU** convierte el planificador en una tabla. `-dS`/
  `-dR` no escriben fichero con `ngccc`: todo va a stderr.
- **`INSN_REG_WEIGHT` no actúa en `sched2`** (`haifa-sched.c:4175`, guardado con
  `!reload_completed`).
- **`REG_LIVE_LENGTH` lo recalcula `sched1`**: los umbrales de vida se atacan por el
  horario, no por la fuente.

## Reglas duras

1. **Nunca `ninja` completo ni `configure.py`.** Usa `build_direct.py`. Hay seis agentes más.
2. **No hagas commits ni `git add`.**
3. **No toques `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.** Proponlo con
   la medida, y **di si el paquete es atómico**.
4. **Cabecera compartida: se propone, no se aplica**, y se deja compilable en todo momento.
5. **Cero bytes con `asm` puesto es deuda.**
6. Borra tus volcados. **No borres con comodines en el scratchpad**: es compartido.
7. **Ninguna sonda de un solo uso en `scripts/`.**
8. **Exígele a tu métrica un control que TENGA que cambiar.**

## El reparto

| agente | encargo |
|---|---|
| `forense1` | los 17.269 B de `zMain`, por la vía del destructor desplazado |
| `forense2` | los mismos 17.269 B, por la vía de los bytes del DOL |
| `control` | ¿es el mismo residuo en `zLua`, `zAnim` y `zMisc`? |
| `flares` | `RenderFlaresOnCar` + `UpdateWheelY` — el volcado `.cse`, **escrito y nunca corrido** |
| `epcalc` | `epCalculate` — `-da` y «Reloads for insn», **escrito y nunca corrido** |
| `loaded` | `UpdateLoaded` — anticipabilidad de PRE, **escrita y nunca montada**. Cierra `zPhysicsBehaviors` |
| `frontera` | por qué mover la frontera de `splits.txt` rompe el DOL |

## Lo que hice yo hoy, para que nadie lo repita

- **Revertida la regresión de `GetLoadingPriority`**: 94,72 % → 97,18 %, 75 filas → 67.
  Llevaba viva desde la r45, y el commit que la metió **borró el comentario que decía que no
  se hiciera**.
- **`steering` gana sus tres rangos** (`.bss` 40 B, `.sdata2` 40 B, `.sbss` −4). DOL OK, y
  `promote.py` pasa de seis síntomas a tres.
- **Fuera las 14 entradas fósiles de `zEAXSound2`** en `keep.lst`, comprobado que eran
  inertes.
- **Las trece `trypromo` del bloque A**: trece ROTO.
