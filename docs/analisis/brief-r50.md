# Brief de la ronda 50

Sustituye a `brief-r49.md`. Leelo entero antes de tocar nada.

## Estado

`matched` **99,04542 %** (3.908.380 / 3.946.048 B, 18.399 / 18.432 funciones).
`linked` **497 / 618 unidades** (777.568 B). El **SDK entero al 100 %**.
Quedan **37.668 B en 32 funciones de 13 unidades**.
DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

**El 80,4 % de las unidades ya enlaza; el 19,6 % que no, guarda el 80,3 % del
codigo.** Ahi esta el trabajo: 1,64 MB de codigo que casa al 100 % y no
promociona, contra 37.668 B de codigo pendiente.

La r49 no cerro ni una funcion. No es mala suerte: las 32 que quedan llevan
entre tres y seis rondas y varias estan **demostradas irreducibles**. Antes de
pelear con una, lee su veda.

## Reglas duras

1. **Nunca lances un `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el arbol.
2. **No hagas commits ni `git add`.**
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Proponlo con
   la medida.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Hay ficheros CRLF enteros, LF enteros y alguno MIXTO. Comprueba antes; un
   parche con la forma equivocada casa 0 de N sin avisar.
7. **BORRA TUS VOLCADOS Y OBJETOS DE PRUEBA AL TERMINAR.** Cada volcado RTL son
   20-30 MB y el disco se ha llenado dos rondas seguidas; un disco lleno se ve
   como un `cc1plus: I/O error` que parece un fallo de compilador.
8. **Cero bytes con `asm` puesto es deuda.**

## LA LECCION DE LA R49, y es de metodo

**Comparar los OBJETOS para diagnosticar `linked` es un error.** El enlace lleva
`-strip-unused-data` y **se lleva todo lo que no se referencia, codigo
incluido**. `scripts/linkdelta.py` enlaza de verdad y compara las secciones del
ELF completo contra la base (2,6 s por unidad): **`.text` sale a delta CERO en
20 de 23 unidades**.

Por medir objetos, la r48 construyo un frente entero que no existe: «163.096 B
de simbolos de mas», «un simbolo que bloquea veinte unidades», y una bandera que
en la unica unidad donde es segura **da el mismo DOL**. Todo refutado.

**Dos reglas que valen para cualquier medida:**

- **Busca si la herramienta ya existe** antes de construir una. `linkdelta.py`
  llevaba dos dias en `scripts/` contestando bien la pregunta.
- **Exigele a tu metrica un control que TENGA que cambiar.** Dos metricas de la
  r49 salieron mentirosas y se cazaron asi: un `-I` puesto detras de los cflags
  hacia que 18 ensayos salieran «identicos» sin compilar nada distinto.

## Herramientas

    python scripts/fncmp.py <unidad> [<Nombre>]   # que funciones NO son identicas
    python scripts/fndiff.py <unidad> <simbolo>   # confirmalo ANTES de empezar
    python scripts/lmap.py <unidad> <simbolo>     # la linea de fuente de cada instruccion
    python scripts/linkdelta.py                   # la distancia REAL al enlace
    python scripts/permorden.py [unidad]          # la permutacion del orden de emision
    python scripts/dolwhere.py <unidad>           # DONDE difiere el DOL
    python scripts/rtldump.py <unidad> <fn> -dg -dl
    python scripts/schedtrace.py ...              # la traza del planificador
    python scripts/promote.py / trypromo.py       # promocion: previa y juez
    python scripts/build_direct.py <unidad>

**`lmap.py` SOBRE EL ORIGINAL es la herramienta que mas ha rendido**: cerro
`LoadSpeechBank` y `FindMatchTime`, y bajo `EvalState` de 52 filas a 18. Dice a
que sentencia atribuye el ORIGINAL cada instruccion; si una linea se lleva tres
comparaciones, o un `lfd` cae donde no hay conversion, ahi esta la respuesta.

**La TU reducida**: un `.cpp` con el preambulo de la SourceList y **un solo
`#include`** reproduce una funcion byte a byte y compila en 2-11 s contra 40.

**Avisos de herramienta, medidos:**
- **`regmap` da falsos positivos por TIPO**: empareja un `bool` con un `int` e
  inventa permutaciones que no existen. Confirma con el volcado DWARF.
- **El DWARF tambien se equivoca**: en `PATHI_calcwaitbeat` daba dos locales en
  f11 y solo una era cierta.
- El ldscript casa por **nombre de fichero**: un objeto de prueba tiene que
  conservar el basename exacto en otro directorio.

## Las palancas

### 1. Barrera selectiva y sus formas

`asm("" : "+r"(x))`, `"+f"`, `"+m"` --extendida y NO volatil, ata solo lo que
nombra--. `"+m"` para una local de PILA. **Un `asm` por argumento DESCOLOCADO**.
**La cadena de N**: una dependencia por eslabon, medidas solo juntas.

`asm("" : : "r"(x))` de solo entrada sube `n_refs` pero **no siempre es gratis**
(sin salidas es volatil): cero bytes en un caso, +1 instruccion en otro.
**Midelo.** Y `asm("")` a secas rompe empates EXACTOS de `allocno_compare`.

**Aviso r49: un `asm` de deuda puede TAPAR la causa real.** En `SetScreenNames`,
con el `asm` puesto la correccion correcta daba 15 filas; sin el, cero.

### 2. El reparto manda sobre el planificador mas veces de lo que parece

- **`set_preference` encadena locales**: de `(set A (mult B C))` el allocno de
  `B` hereda el registro de `A`. Hay que pinchar **la carga que le quita el
  registro**, no la variable del diff.
- **`regs_someone_prefers`** (`global.c:888`): un allocno de MENOR prioridad
  puede reservarte un registro que esta LIBRE.
- **Una diferencia de planificacion puede tener la raiz en el ASIGNADOR**: en
  `ProcessUpdate`, mover un valor cambio el registro, `sched2` vio una
  antidependencia y la prioridad salto de 2 a 17. **Mira el `.lreg` primero.**
- **Un pin puede ser ANDAMIO**: retirarlo gano tres filas en `InitAtSegment`.
  Pero midelo: en otras dos funciones retirarlo costo 37 filas.

`REG_ALLOC_ORDER` de rs6000: `0, 9, 11, 10, 8, 7, 6, 5, 4, 3, 31, 30…`; en FP
`f0, f13, f12, f11, f10…`. El «31 primero» solo sale en `global_alloc` para
allocnos que cruzan llamadas.

### 3. El planificador, con sus niveles

`rank_for_schedule`: prioridad, luego `INSN_REG_WEIGHT` **que SOLO existe en
`sched1`**, luego la **clase respecto al ultimo insn programado**, luego
`depend_count`, luego `INSN_LUID`. En `sched2` el nivel de peso no se consulta.

**NUEVO r49 --- «el `and` que se hunde»**: `INSN_REG_WEIGHT` da peso **2** a un
`and` reg-reg porque `andsi3` de rs6000 es siempre un `PARALLEL` (SET + clobber
del scratch de CC). Arreglo de cero bytes: mover la sentencia para que un
operando **muera** en ella.

**NUEVO r49 --- si el objetivo tiene una insn MAS ABAJO que nosotros, NO es el
planificador**: `compute_trg_info` solo trae insns de bloques *posteriores* al
destino. Es la fuente.

Lee la **tabla ciclo x unidad** de `-fsched-verbose-2`, no las lineas
`--> scheduling insn`: leerlas mal produjo un cierre falso en la r48.

### 4. Literales y datos

- **El «tamano mentido» generaliza a arrays**: `extern const int t[2]
  __asm__("t_8050042C")` conserva el `@sda21`, no mueve el `.text` y quita 920 B
  de `.sdata2` que el extraido no tiene.
- **Un `$LC` muerto se estripa y desplaza 4 B; un `asm()` de fichero no.**
- El inicializador decide `.data` contra `.bss` **aunque el valor sea cero**.
- **Nunca QUITES un literal** en una unidad con `$LC` en `keep.lst`.

### 5. Lo que se reserva al EXPANDIR y sobrevive a su codigo

El area X del marco y el `lfd` del sesgo `0x4330000080000000`. Se reproduce
multiplicando por una variable entera que siempre vale 1. **Frente barrido y
vacio**: no busques un tercer caso.

### 6. Otras

`asm()` de fichero para alias y talones; el `asm` que **emite la instruccion que
falta** cuando `combine` no puede plegar; las guardas de cabecera que nadie
enciende; el compilador equivocado y el objeto que son varias TU; el mapa de
lineas manda sobre `-ffast-math`; la forma del bucle decide el idioma de la
rama (`&&` da `blt`, `if/else`+`break` da `fcmpu+cror+bso`); y el DWARF trae los
TIPOS, no solo las locales.

**El `switch` como experimento de CONTROL** (r49): compilar una cadena de `if`
como `switch` separa reparto de equivalencia de CSE en una sola compilacion.

## El reparto

| # | clave | territorio | valor |
|---|---|---|---|
| 1 | `orden` | `zSim` (3 desplazadas) y `zMain` (32) | 256 kB de `linked` |
| 2 | `rodata` | el deficit de `.rodata` viva, y las 17 funciones que `zFe2` no emite | ~1 MB |
| 3 | `promo` | las unidades pequenas sin promocionar | ~15 kB |
| 4 | `cam` | `zCamera` | 10.812 B |
| 5 | `world` | `zWorld` + `zWorld2` + `zTrack` | 10.112 B |
| 6 | `snd` | `zEcstasy` + `zEagl4Anim` + `zSpeech` + `zEAXSound2` | 9.824 B |
| 7 | `plat` | `steering` + `zPlatform` + `zPhysicsBehaviors` + `madidct` + `criticalpath` | 6.920 B |

**Los tres primeros valen mucho mas que los otros cuatro juntos.**

## Que entregar

1. Informe en `docs/analisis/r50-<tu-clave>.md`: lo cerrado (con `fndiff` al
   100 % citado), lo negativo **con la cifra**, y las propuestas fuera de
   territorio.
2. Verificacion: `fndiff` = 100.0 en lo que cierres; `fncmp` antes/despues sobre
   TODAS tus unidades sin que **ninguna** empeore; `lcfix.py --check` limpio.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**.
4. **Borra tus volcados.**
