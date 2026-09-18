# Brief de la ronda 53

Sustituye a `brief-r52.md`. Léelo entero antes de tocar nada.

## Estado

    python scripts/estado.py        # el frente
    python scripts/linkdelta.py     # la distancia REAL de cada unidad al enlace

    CODIGO   3.910.372 / 3.946.048 B   99,0959 %   faltan 31 funciones, 35.676 B
    DATOS    0 B de trabajo real (ver §«el .data no es un frente»)
    ENLACE   518 / 616, techo REAL 544
             QUEDAN 27 unidades: 22 SourceLists + 5 de biblioteca (una imposible)

DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## La r52 cambió el frente: SEIS unidades están a menos de 300 B

```
zSpeech             rodata-8                      <- y UNA funcion (596 B)
zMain               data-32                       <- codigo al 100 %
zPhysics            rodata-40   bss-32            <- codigo al 100 %
zPhysicsBehaviors   rodata-40   data+416 bss-988
zEAXSound           rodata-104  data-384          <- codigo al 100 %
zGameModes          rodata+104                    <- codigo al 100 %, SUPERAVIT
zEAXSound2          rodata-176  data-288
zFe                 rodata-192  data-32           <- codigo al 100 %
zAI                 rodata-240  data-384          <- codigo al 100 %
zEcstasy            rodata-288  data+64  bss+32
```

En la r52 el déficit de `.rodata` del frente pasó de **−24.416 a −6.080 B**
(18.336 cerrados, cero regresiones). **Éste es el trabajo de la r53: convertir
esos restos en promociones.** `zMain` son 159 kB de `linked`, `zAI` 272 kB,
`zFe` 174 kB, `zEAXSound` 151 kB, `zPhysics` 146 kB.

## La caja de herramientas que salió de la r52

### La receta del vocabulario (cierra el CONTENIDO de `.rodata`)

    python scripts/prefijotu.py <u> --asm    # las cadenas que faltan, EN ORDEN DEL OBJETIVO
    python scripts/deadlink.py  <u> --keep   # las nuestras que el estripado se lleva
    python scripts/lcfix.py <u>              # SIEMPRE despues de deadlink

El bloque va **al final del `.cpp`**: ahí no desplaza ningún `$LC`. Probada en
trece unidades. **`lcfix` es obligatorio después de `deadlink`**: sus números
salen de otra compilación y en tres de siete unidades estaban mal.

### El primer de pool (cierra el ORDEN)

Una **`static inline` MUERTA** con un `switch` de `return "literal"`: cc1plus
**no emite la función** pero **sí los literales, en ese punto del fichero**, y la
dedup de la TU hace que cualquier uso posterior comparta ese `$LC`. Mueve
cualquier cadena a un punto **ANTERIOR** por cero bytes.

**Sólo mueve hacia atrás**, y no vale para floats (su pool se reinicia por
función; ahí va un `.4byte` crudo). Es estrictamente mejor que un `asm()`
escrito a mano — la r52 cambió ocho andamios por primers.

    python scripts/rodorden.py [<u>]    # que cadenas NO estan en su sitio

Y el mapa: **el pool se vuelca justo antes de cada función**, cadenas de parseo
primero. Cada tramo pertenece al hueco entre dos `#include`.

### Reparto de registros — la palanca de enteros CORREGIDA

El brief de la r52 daba `asm("" : : : "rN")`. **Es la mala**: empeora en los ocho
registros probados (14 → 89 filas). La buena es **añadir la variable como entrada
`"r"(x)` a un `asm` que ya existe** — sube `n_refs` y con ello `QTY_CMP_PRI` sin
gastar ranura. Y **cuál de dos variables lleva la salida `"+r"` decide el reparto
de los preservados**. Así cerró `_Storage`.

### Leer el planificador sin adivinar

**`-fsched-verbose-5` sobre una mini-TU** da `prio` por insn y el log de la lista
de listos por ciclo: convierte el reparto en una tabla. Una mini-TU de cuatro
líneas reproduce el símbolo mangled exacto en 3,8 s. Ojo: `-dS`/`-dR` **no
escriben fichero** con `ngccc`, todo va a stderr.

Cuatro correcciones al catálogo, de la r52:

- **`REG_LIVE_LENGTH` lo RECALCULA `sched1`** (`haifa-sched.c:5584`). Refuta la
  frase de la r50 sobre `InitAtSegment`: **todo umbral de `live` se ataca por el
  HORARIO, no por la fuente.**
- **`rank_for_schedule` tiene un escalón que faltaba**: `INSN_REG_WEIGHT`
  (nº de SET − nº de `REG_DEAD`, gana el menor) va tras la prioridad y **antes**
  de la clase. O sea que cuántas veces usa la fuente una variable decide dónde
  cae su comparación izada.
- `INSN_PRIORITY` de un insn sin dependientes es su propio `insn_cost`.
- `REG_N_REFS` se pondera por `loop_depth` (`flow.c:3399`).
- **Un `asm` con `unit none` SÍ gasta ranura** (`ISSUE_RATE`=2).

## El `.data` NO es un frente

Medido en siete unidades (`r52-jf-data.md`): de 2.568 B que «faltan», 844 son
renombres del contador de declaración, 1.724 son nombres del troceador
(`pad_`/`gap_`/`lbl_`, y casi todos CEROS: relleno de alineación) y **0 B son
dato con nombre sin escribir**.

**Y parte del diff de `.data` es la SOMBRA del `.rodata`**: en zAI una docena de
tablas de `const char *` salían como dato ausente y sólo eran punteros a cadenas
que están 752 B más allá. **Cierra el `.rodata` antes de mirar el `.data`.**

## Reglas duras

1. **Nunca lances `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el árbol.
2. **No hagas commits ni `git add`.**
3. **No toques `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.**
   Proponlo con la medida. Para `keep.lst`, deja las líneas exactas y **di si el
   paquete es atómico**.
4. **Una cabecera compartida se propone, no se aplica**, y si la tocas para
   medir, **déjala compilable en todo momento**.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. **Borra tus volcados**, y **no borres con comodines en el scratchpad**.
7. **Cero bytes con `asm` puesto es deuda.**
8. **Ninguna sonda de un solo uso en `scripts/`.**
9. **Mira si la herramienta existe**: `docs/HERRAMIENTAS.md` (generado) y
   `docs/PLAYBOOK-MAPA.md` para el manual, que pesa 442 kB.
10. **Exígele a tu métrica un control que TENGA que cambiar.**

## Trampas medidas, todas con su caso

- **`dolrod` y `dolwhere` imputan al símbolo EQUIVOCADO** si la sección está
  descolocada: nombran el siguiente símbolo que encuentran.
- **`dolwhere` sólo funciona si los tamaños ya coinciden**; si no, para en
  `LAS SECCIONES NO COINCIDEN`.
- **Sombrear una cabecera con `-I` no funciona si el include es RELATIVO**:
  copia el directorio entero y mete un error de sintaxis como control.
- **El fuzzy manda al sitio equivocado**: cuenta filas con `fndiff`.
- **El número de `$LC` NO es el orden de la `.rodata`.**
- **Una línea en blanco no puede cambiar un byte** (`block_alloc` salta los NOTE,
  `rank_for_schedule` sólo usa el signo del LUID). El mapa de líneas dice DÓNDE
  mirar, nunca QUÉ poner.
- **Los andamios caducan — pero no todos.** Re-medir, no suponer.

## El reparto

| agente | encargo |
|---|---|
| `speech` | **zSpeech**: `rodata−8` y la función `Setup` (596 B). Una función para 178.888 B de `linked` |
| `cerca1` | **zMain** (`data−32`) y **zPhysics** (`rodata−40 bss−32`), las dos con el código al 100 % |
| `cerca2` | **zFe** (`−192 data−32`) y **zEAXSound** (`−104 data−384`), código al 100 % |
| `cerca3` | **zAI** (`−240 data−384`) y **zGameModes** (`+104`, superávit) |
| `ecs` | **zEcstasy**: 4 funciones y `rodata−288` |
| `cam` | zCamera: 4 funciones, 9.656 B |
| `world` | zWorld (4 fn) y zWorld2 (2 fn, `rodata−808`) |

## Lo que me quedo yo

- `keep.lst`, `splits.txt` y `configure.py`.
- El rango de `steering` (`.bss` 40 B, `.sdata2` 40 B, `.sbss` −4), medido y
  probado con `STT_FILE` en `r52-jf-steering.md`, pendiente de re-extraer.
- Las 150 cadenas duplicadas de `zMisc` (`r52-jf-superavit.md`): 2.186 B con la
  lista y el método hechos.
