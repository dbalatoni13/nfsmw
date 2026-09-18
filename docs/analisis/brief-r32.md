# Ronda 32 — instrucciones comunes

Estado: **97,97 % matched**, 18.337 funciones, **`linked` 12,57 %** (454 de 604),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).
**74 unidades sin promocionar, 3.450.152 B — y 37 tienen el `.text` YA PERFECTO
(701.652 B).**

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

---

## 0. EL OBJETIVO ES `linked`, y la r31 dejó el mapa hecho

La r31 promocionó **`zDebug`, la primera SourceList del proyecto**, y dejó tres
unidades más enlazando. Tu encargo se mide en **bytes que pasan a `linked`**, no
en porcentaje: una función de 3.120 B que desbloquea zPhysics vale **146.124 B**.

**Y la lección que costó cuatro agentes: cerrar la función NUNCA basta.** Antes
de gastar un ensayo en codegen, pasa `python scripts/trypromo.py <unidad>` y mira
qué más falta. Dos agentes de la r31 descubrieron a mitad de encargo que sus
unidades ni siquiera enlazaban.

## 1. Las CUATRO cosas que bloquean una promoción, y cómo se distinguen

`trypromo.py` las separa en un segundo:

| lo que dice | qué es | dónde mirar |
|---|---|---|
| `ENLACE FALLA` | símbolos indefinidos | el `L0039`, **no** el `L0019` de arriba, que es un AVISO |
| `DOL ROTO`, sección corta | falta dato | §2 |
| `DOL ROTO`, sección larga | sobra el pool `$LC` | §3 |
| `DOL ROTO`, mismo tamaño | **el ORDEN del `.text`** | §4 |

## 2. Dato que falta: los objetos del original NO TIENEN NI UN `$LC`

Contado en las 33 (`scripts/lcpool.py`): emitimos **142.460 B de `$LC`** contra
**7.616** del objetivo, y esos 7.616 son de una sola unidad. Todo el `.rodata`
del objetivo son `lbl_` —las etiquetas con que `dtk` bautiza el pool anónimo—,
nombres y relleno. **El pool existe en los dos lados; cambia el nombre.**

Y ahí las 33 se parten en dos:

- **26 van CORTAS** (`lbl_` nuestro 0, `$LC` menor que el `lbl_` del objetivo):
  `zAI` 7.795 contra 8.453, `zWorld` 10.854 contra 14.394, `zFe2` 13.275 contra
  17.011. **Falta pool de verdad**: forzando `keep.lst` con todos los símbolos de
  nuestro objeto, `zFoundation` sólo recupera 32 B de 1.248.
- **7 escriben su pool A MANO y el total casa EXACTO** (§3).

**Parte de ese déficit ya tiene nombre**: la `.rodata` de **31 de 33** empieza por
el mismo pool de bWare/STL de 92 B, con la ruta
`d:/mw/speed/indep/bware/inc/bware.hpp` — el `__FILE__` de la máquina de EA.
**No se puede generar: hay que escribirlo**, y sólo `zDebug`/`zOnline` lo hacen.

## 3. Pool `$LC` que SOBRA: 12.576 B en seis unidades

`zEcstasy` +5.632 · `zMisc` +4.768 · `zRender` +1.208 · `zMiscSmall` +648 ·
`zDynamics` +296 · `zMission` +24. En las seis, el `lbl_` escrito a mano **casa
byte a byte con el del objetivo**; lo que sobra es el pool que `cc1plus` emite
además, porque nuestro código escribe la constante donde el original
**referenciaba la etiqueta**.

**BORRAR LAS ETIQUETAS NO VALE, y está medido dos veces en `zDynamics`**: quitar
las 59 escalares baja el delta de +296 a +48 pero **rompe el enlace**, y quitar
sólo las 53 que la unidad no usa lo rompe igual — **las referencian los objetos
EXTRAÍDOS DE OTRAS UNIDADES, por nombre**. La veda está escrita en el propio
`zDynamics.cpp`.

**Lo que hay que hacer es que el CÓDIGO use las etiquetas**
(`extern const float lbl_XXXXXXXX;`) para que `cc1plus` no emita su pool. Y la
regla, medida por `und` en la r31: **la definición va AL FINAL de la unidad de
traducción**; con el inicializador visible en el punto de uso GCC pliega la carga
y rompe la función (`OnManageTime` cayó de 100 % a 97,61 %). `UMath.cpp` ya usa
la técnica.

**El emparejamiento NO es posicional.** En `zDynamics` cuadra en las primeras 24
y se desalinea porque nuestro pool mete un `4503601774854144.0` —la magia de
conversión int→double— que el original no tiene; los multiconjuntos difieren en
**7 constantes**. El mapa fiable sale de **comparar las reubicaciones insn a insn
contra el objeto extraído**, no de la posición.

## 4. EL FRENTE NUEVO, y es invisible para objdiff: el ORDEN del `.text`

`und` lo encontró en la r31: **una unidad puede tener el `.text` del tamaño
exacto, cero funciones ausentes y cero funciones por debajo del 100 %, y aun así
tener las funciones EN OTRO ORDEN**. `objdiff` empareja por nombre, así que no lo
ve. 169 funciones en zFoundation, 208 en zSim, 28 en zMisc.

**El mecanismo, medido: en GCC 2.9 un virtual con el cuerpo DENTRO de la clase se
emite en `finish_file`, al final de la unidad; fuera, donde está.** Con eso se
aislaron las cinco causas de zFoundation y se cerraron cuatro —18 funciones a su
sitio, de 169 a 120— sin perder un byte.

## 5. Herramientas (las cuatro nuevas de la r31 arriba del todo)

- **`promodist.py [--sl|--libs]`** — ordena las unidades por **lo que
  desbloquean**, con funciones pendientes y delta por sección. `fns=0` = el
  trabajo es de datos.
- **`lcpool.py`** — el pool `$LC` nuestro contra el `lbl_` del objetivo.
- **`dupdata.py`** — símbolos a mano cuyo gemelo emite el compilador.
  **Filtro, no lista de borrado**: dos constantes distintas pueden tener los
  mismos bytes.
- **`keepchk.py`** — entradas rancias de `keep.lst`. **Obligatorio tras tocar una
  frontera**: `dtk` renombra el relleno según dónde caiga (`gap_` entre símbolos,
  `pad_` al principio o al final del rango), la entrada deja de casar y
  `-strip-unused-data` se lleva los bytes: **DOL corto y el enlace no falla**.
- `trypromo.py` es el juez. **Lee el `build.ninja` VIVO**: si un barrido entero
  contesta `NO esta en la lista de enlace`, no son las unidades, es que otro
  agente acaba de regenerar el fichero.
- `audit.py`/`frozen.py` quieren la **ruta completa SIN extensión**.
- `alloc.py` (no `lreg.py`). `-fsched-verbose-5` **con guion**.
- **Finales de línea MEZCLADOS**: `spchpick.c` y `UTLVector.h` son CRLF enteros;
  un parche con `\n` casa **0 de N sin avisar**. Ancla con `splitlines(True)`.

## 6. Codegen: lo que sigue vivo

1. **La barrera cierra 2 de cada 8** y va **DETRÁS** de la instrucción que llega
   tarde. Novedad de la r31: **una barrera total delante de una sentencia arregla
   el `global_alloc` de la función entera**. Es deuda declarada: **dilo**.
2. **Un `lis sym@ha` izado sobre un `bl` = falta una CONVERSIÓN.**
3. **El orden de los operandos de un `|`** decide el árbol de `fold`.
4. **El orden de declaración de los miembros DENTRO de la clase** decide el
   inline… y ahora también **dónde se emite la función** (§4).
5. **Las vedas CADUCAN**: la r31 tumbó cuatro, incluida la receta de flags de la
   r27 (`-fno-cse-follow-jumps -fno-cse-skip-blocks` cuesta **75.596 B y 117
   funciones** en zPhysics).
6. **El marco de pila**: si el objetivo reserva más `vars_size` y el DWARF no
   nombra más locales, es un **temporal** que GCC reserva y no usa — la forma de
   la fuente no llega ahí (`PATH_createstreamimp`, seis formas).

## 7. Las cuatro reglas

1. **`trypromo.py` ANTES de gastar un ensayo en codegen.**
2. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
3. **Si tu palanca es un constructo que el original no tenía, DILO.**
4. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano.**

## Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra. Si no cierra, **revierte** y anota la veda
  **diciendo qué sentencia barriste**.
- Detalle en `docs/analisis/r32-<grupo>.md`. **No commits.**
- **Si dejas un fichero modificado que rompe el enlace, DILO EN EL INFORME.**

## Prohibido

- **Escribir ensamblador de instrucciones.** Van diez falsificaciones retiradas.
  Emitir **datos** con `.long`/`.float` sí es legítimo — de hecho es el frente.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación**
  (`trypromo.py` + DOL). Puedes editarlos para medir **si los dejas como
  estaban** (copia de seguridad ANTES).
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).

## Convivencia

Scratchpad con tu prefijo `c32<grupo>_`. Hay otros tres agentes en el mismo
`build/`: mide con objeto suelto (`build_direct.py`/`mn_repro.py`), nunca con un
`ninja` completo. Vigila el disco (19 GB libres).
