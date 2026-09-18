# r39 — __va_arg cerrado e integrado: +244 B matched, +344 B linked

2026-09-07, base `590dc4a0` con r37/r38 conservados. El usuario autorizo
explicitamente dividir solo ppc2D2 y actualizar su configuracion/split.

## Resultado integrado

| unidad | rango .text | compilador | resultado |
|---|---|---|---|
| ppc2D2.tmp | 0x80311C50..0x80311CB4 | ProDG 3.9.3, flags previos | **100 B / 8 funciones exactas** |
| Runtime.PPCEABI.H/__va_arg.c | 0x80311CB4..0x80311DA8 | MW GC/1.2.5n | **244 B / 1 funcion exacta** |

Ambas entradas son ahora `Matching`. Los ocho cuerpos previos no cambian;
la ganancia matched es **244 B / 1 funcion**, no 344 B. La ganancia linked
si es **344 B**, porque antes se enlazaba todo el objeto extraido.

El DOL PRINCIPAL se reconstruyo con la lista de objetos generada actual y
verifico SHA-1 **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. No se uso ninja.
Este resultado ya no es solo una propuesta ni una comprobacion aislada.

## Evidencia del compilador y de la ABI

r38 encontro que MW antiguo reproduce la estructura de __va_arg mucho mejor
que GCC. En r39 se encontro ademas la declaracion `unsigned char` del segundo
parametro en la cabecera LOCAL
`src/Speed/GameCube/bWare/GameCube/dolphinsdk/include/libc/stdarg.h`.
La otra cabecera, `include/Runtime.PPCEABI.H/__va_arg.h`, usa int; no era
la unica evidencia de firma disponible y no fue modificada.

La representacion conserva dos contadores de un byte, dos bytes reservados
y punteros en +4/+8: 12 B EABI. Se declaran contadores explicitamente signed
porque el original usa lbz/extsb. El argumento type se enmascara al byte
bajo, exactamente como el original; se mantienen todos sus casos, no solo
los de llamada mas frecuentes.

Como referencia estructural se consulto la reconstruccion primaria de
[__va_arg en doldecomp/sms](https://github.com/doldecomp/sms/blob/main/src/PowerPC_EABI_Support/Runtime/__va_arg.c),
que separa la lectura del puntero de overflow de su alineacion. La decision
de usar esta forma en NFS se basa en los bytes del ELF de NFS y en el DOL
reconstruido, no en asumir que dos juegos comparten exactamente el runtime.

## Cierre causal y controles negativos

La fuente C aislada C1 alcanza **244/244 B y 100,0 %** sin ASM. Usa result
declarado primero, contador antes del indice, suma de puntero/base/producto
asociada a la izquierda, y lectura de overflow en result antes de alinearlo.

Dos controles sobre la solucion exacta delimitan las causas:

| control | resultado | diferencia |
|---|---:|---|
| C2: volver a saved + (base + index*stride) | 244 B / 97,78689 % | cambia las sumas, el coalescing y el reparto desde el primer lbz |
| C3: volver a alineacion directa desde list->overflow | 240 B / 97,70492 % | pliega size-1 con la suma y pierde el subi del objetivo |

Ambos se retiraron. Estos controles no prueban que cada cambio de
declaracion sea necesario por separado; prueban que las dos expresiones
anteriores impiden el cierre aun sobre el resto de la receta correcta.

La nueva TU usa `cflags_runtime` ya existente mas `-char unsigned -lang=c`,
con override de compilador SOLO para su objeto. No se cambian flags
compartidos, cabeceras ni compiladores de otras unidades. Los pragmas comunes
del proyecto (`cats off`, etc.) mantienen el cierre real; no se dependen de
los flags reducidos del experimento inicial.

## El falso atajo del enlace parcial

Primero se compilaron en scratchpad los puentes GCC previos y el cuerpo MW
cerrado. Sus nueve funciones pasan la auditoria estricta tambien al juntar
los objetos con `ngcld -r`, pero el DOL de esa forma NO coincide:

- combinado con -r: **5f20cad403ac17d7d08e6f9881d18e078e0ad893**;
- dos objetos separados: **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.

El objeto combinado introduce `__sn__bss__tag__address__` y una entrada de
4 B en .data con reubicacion, ausentes del objetivo. El DOL resulta 32 B
mayor. La integracion aprobada usa dos objetos separados, NO ese artefacto
combinado. No se copian bytes del original al objeto compilado ni se
retocan objetos para hacerlos coincidir.

## Cambios aplicados con autorizacion

1. `src/LibSN/ppc2D2.c`: se retira la reconstruccion GCC pendiente de
   __va_arg; se conservan conversion y siete puentes previos intactos.
   Se actualiza el comentario historico sobre FSasync.
2. Nueva `src/Runtime.PPCEABI.H/__va_arg.c`: cuerpo C exacto de 244 B.
3. `config/GOWE69/splits.txt`: corte unico en **0x80311CB4**. El extremo
   anterior (0x80311C50) y posterior (0x80311DA8) no cambian; FSasync intacta.
4. `configure.py`: ppc2D2 a Matching y nueva entrada MW Matching contigua.
   La entrada de spchpick NO cambia: su autorizacion sigue pendiente.

Se pauso a los tres agentes durante la regeneracion. Se ejecuto directamente
`dtk dol split`, `python configure.py`, builds de esas dos fuentes y enlace,
conversion a DOL y comprobacion SHA. El aviso de configuracion ausente `asd2`
no impidio generar los dos objetos requeridos. Los avisos de alineacion del
troceador y duplicados PPCMtdec/BlockCalculator del linker no se ocultaron;
el hash exacto del resultado es el criterio de aceptacion.

## Auditoria integrada

- `fndiff Runtime.PPCEABI.H/__va_arg __va_arg`: **100,0 % / 244 B**.
- `fncmp ppc2D2`: **0/8 distintas**.
- `audit.py` en las dos unidades: **9/9 exactas**; __va_arg tiene 7 ramas,
  ninguna reubicacion ni literal. Puentes y conversion conservan sus refs.
- `codex_r39_runtime_audit.py`: concatenar las dos secciones originales
  nuevas reproduce byte por byte los 344 B extraidos antes del corte.
  Los 100 B compilados de puentes son identicos al prefijo del objeto previo.
- Ninguna TU nueva emite datos, BSS, ctors/dtors ni relleno entre los cuerpos.
- Configuracion objdiff: solo cambia ppc2D2 y se añade la unidad MW;
  no se pierde ninguna unidad ni se cambian asociaciones de vecinos.
- `checksplits.py`: cero solapes y cero cortes dentro de simbolos.
- `lcfix.py --check`: limpio.
- DOL principal: comprobacion `dtk shasum -c ... -o build/GOWE69/ok`, **OK**.
- Fuentes runtime conservan CRLF y se recompilaron despues de normalizarlo.

## Artefactos reproducibles

En `scratchpad/`, prefijo `codex_r39_`:

- `va_arg.c`, `va_compile.py`, `va_c1/c2/c3.o/.json`: cierre y controles.
- `ppc_bridges.c`, `ppc_split_probe.py`: prueba de dos layouts sin modificar
  objetos reales; reconoce tanto el estado anterior como el split integrado.
- `ppc_target_before_split.o`, `ppc_source_before_split.o`: objetos previos.
- `ppc_combined` y `ppc_separate`, con .o/.json cuando corresponde y
  .rsp/.elf/.dol: resultados de enlaces aislados.
- `objdiff_before_split.json`: configuracion de comparacion previa.
- `runtime_audit.py`: auditoria de particion, prefijo, unidades y DOL real.
- `link_main.py`: ejecuta solo linker/elf2dol/shasum con los inputs actuales.
- `main_before_runtime.elf/.dol`: copias del principal anterior para comparar.

Metricas globales y auditorias de los otros territorios: `r39-cierre.md`.
