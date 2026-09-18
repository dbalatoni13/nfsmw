# R45 — cierre y ventana

2026-09-08. Base `590dc4a0` más las nueve rondas de fuente sin comprometer.
Esta ventana verifica todo lo acumulado, lo compromete, y aplica las dos
promociones que estaban demostradas y esperando autorización.

## Estado verificado

| medida | r44 | ahora |
|---|---:|---:|
| código exacto | 3.893.124 / 3.946.048 B — 98,65881 % | **igual** |
| funciones exactas | 18.373 / 18.432 | **igual** |
| código linked | 736.012 B — 18,65 % (478 / 618) | **746.188 B — 18,91 % (480 / 617)** |
| datos linked | 319.036 B | **319.056 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 4.541.888 B, verificado
después de cada paso. `lcfix.py --check` y `checksplits.py` limpios.

Quedan **52.924 B en 27 unidades**.

## Lo verificado y comprometido

Las nueve rondas de fuente estaban en el árbol sin comprometer. Verificadas con
un `ninja` completo y repartidas en nueve commits:

- **`__va_arg` no era GCC, era Metrowerks.** El troceador la metía en
  `ppc2D2.tmp` con los puentes de runtime; diez ensayos de fuente se habían
  quedado en el 58,49 % porque el compilador estaba equivocado. Rango partido
  en dos y objeto propio con `GC/1.2.5n`. `ppc2D2` pasa a Matching.
- **`AddToQueue`** (252 B) y **las dos de `spchpick`** (936 B).
- **`GetLoadingPriority` NO cerró.** El cambio de fuente es real --el mapa de
  líneas separa los tres productos y así `-ffast-math` ya no los reagrupa-- pero
  el delta es **0 B / 0 funciones** y el porcentaje **bajó** de 97,18 a 94,72 %.
  Se conserva por ser estructuralmente honesto, no por medida. El bloqueo real
  está en la primera instrucción: el objetivo abre marco `0x120` y nosotros
  `0x110`, o sea que le falta una local de 16 B.
- **`MsgPlayMiscSound`**: no era irresoluble por `Csis::FX_UVES`; los dos
  PursuitBreak llevan ancho `-1` y el constructor pone `SetID` antes que
  `SetVolume`.
- `steering` (dos accesos volátiles, que es lo único que queda en mwcc), `vm.c`,
  y la corrección del diagnóstico de `madidct`.

## Los literales huérfanos: tres rangos reclamados

`sserver`, `ssysinit` y `stagpat` emitían un `.rodata` que el objeto extraído no
tiene, porque el troceador dejó esos bytes sin dueña. Los tres rangos salen del
emparejamiento `$LC`/`lbl_` --no de buscar bytes-- y cumplen las dos reglas de
la ventana. El de `stagpat` cierra además el hueco de detrás del de `ssysinit`,
así que van juntos o no van.

`sserver` cortaba 4 B dentro de `lbl_80412880`, declarado de 8 B siendo dos
flotantes: partido en `symbols.txt` conservando la suma (regla B).

**`stagpat` promociona**: 2.340 B.

## `spchpick` promociona: 7.836 B

La única pega de `promote.py` son dos reubicaciones a símbolos con sufijo de
dirección, `gEventChoice_804CC900` y `multiple_80451F08`. Resuelven precisamente
porque el objeto hermano que las **define** sigue extraído, y la fuente ya las
nombra con el alias `__asm__`. `trypromo` enlaza y el DOL sale byte a byte.

## Veda nueva: el orden del `.bss` no se manda desde la fuente

Al quitarles el `.rodata` de más, `sserver` y `ssysinit` destaparon el bloqueo
que había detrás: el objetivo ordena su `.bss` lista/tick/periodo y nosotros
lista/periodo/tick. **No es cosmético** --`trypromo` rompe el DOL
(`8b1d7ab35729` y `0eb5a9d50601`)-- y no se puede cambiar.

Dieciocho formas medidas leyendo el orden de los `.lcomm` en el `-S`, que es más
fiable que mirar el `.o`:

- las **seis** permutaciones de las tres definiciones: idénticas;
- `extern` previo de uno o de los tres, segundo bloque `namespace Snd`, y la
  definición al final del fichero: idénticas;
- `volatile`, `-fno-common`, `-fcommon`, y tomar la dirección en un
  inicializador de fichero: idénticas;
- **renombrar la variable FUENTE** a `zz_period` conservando el nombre de
  ensamblador con `__asm__("_3Snd.gVariableTimerPeriod")`: idéntica.

Ese último cierra la puerta: la clave de orden es el **nombre de ensamblador**, y
ese lo fija el objetivo. La lista sale antes que las otras dos porque tiene
constructor y se emite junto a su init; las dos POD caen en la volcada final. Y
no es un inicializador dinámico: el
`__static_initialization_and_destruction_0` del original mide 40 B, los mismos
que el nuestro.

## Barrido de promoción: qué queda y por qué no

`trypromo` es la verdad --enlaza de verdad y compara el DOL--, así que se pasó a
las seis candidatas restantes. **Las seis rompen**:

| unidad | B | resultado |
|---|---:|---|
| `gc_interface` | 6.152 | ENLACE FALLA: `asd2` referencia `__static_initialization_and_destruction_0_803906C0` |
| `eathread_thread` | 84 | DOL ROTO `977db325db27` |
| `zGameModes` | 124 | DOL ROTO `c2cf9751dac3` |
| `realcore/input/interface` | 204 | DOL ROTO `648d01a3132d` |
| `sdspmix` | 2.120 | DOL ROTO `185355ce88be` |
| `libgcc2_4` | 176 | DOL ROTO `951091f0ce4b` |

Y `bigswizzler`, cuyo rango `0x8041010C..0x80410124` sí está verificado por
`claimlbl`, **no se reclamó**: tiene 52 B de hueco detrás y la regla A lo veta.
Para cobrarlo hay que reclamar también a su vecina de detrás.

## `gc_interface` + `asd2`: el racimo diagnosticado

Vale 6.240 B y es el mayor bloqueo de `linked` con causa conocida.

El troceador partió la unidad en dos. El objetivo deja en `gc_interface.o` el
`__static_initialization_and_destruction_0` **renombrado con sufijo de
dirección**, sin `.ctors` ni `.dtors`; y en `asd2.o` los 88 B de
`_GLOBAL_.I.` y `_GLOBAL_.D.` más sus dos entradas de `.ctors`/`.dtors`.

La diferencia de fondo está en el **nombre** de esos dos: el original los llama
`_GLOBAL_.I._6Realmc.ROOT_DIRECTORY_NAME` y nosotros
`_GLOBAL_.I._Q26Realmc11GCInterface.mpDriver`. GCC bautiza esa función según el
estático con constructor de la unidad, así que el original tenía un
`Realmc::ROOT_DIRECTORY_NAME` con constructor que nuestra fuente no tiene.
Nuestro `.text` mide además 6.424 B contra los 6.152+88 del objetivo.

Es trabajo de fuente en el territorio de `realmemcard`, no de ventana.
