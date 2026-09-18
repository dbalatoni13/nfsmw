# Ronda 27 — ventana: verificacion del hallazgo de las secciones

## Lo que verifique, y hay que partirlo en dos

El agente de `zFEng` cerro su encargo con una cifra que se sale de el:
**393.208 B de `.text` sin comparar en 184 unidades**, y seis SourceLists que
`report.json` da al **100 %** con hasta 18.908 B de mas.

**La reproduje y sale mas grande todavia** — `zPhysics` lleva **+43.960 B**, la
mayor del arbol. Pero la cifra **conflaciona dos cosas distintas**, y sin
separarlas no se puede repartir trabajo sobre ella:

### En bibliotecas el exceso es INOCUO, y esta probado

Las unidades del `dolphinsdk` salen con miles de bytes de mas y **cero
diferencia en `.rodata`, `.data` y `.bss`**: `mtx` +5.376, `dvd` +5.412,
`vi` +4.984, `mtx44` +3.784, `dvdfs` +3.404, `OSThread` +3.368.

**`mtx.c` esta `Matching` en `configure.py:1774`** —o sea promocionada— **con sus
+5.376 B, y el DOL es byte a byte identico**. El objeto extraido solo contiene lo
que quedo en el DOL; nosotros compilamos la unidad entera. Ese delta es
**esperado y no es credito falso**.

### En las SourceLists SI bloquea, y esta probado

`zLua` figura al **100 %** con **+18.908 B** de `.text`. `trypromo.py`:

    Speed/Indep/SourceLists/zLua    DOL ROTO (721388607032)

**Una unidad al 100 % que no puede promocionar, y el porcentaje no lo dice.**
Las seis del caso: `zLua` +18.908, `zSim` +18.312, `zAnim` +9.076,
`zMisc` +8.360, `zRender` +4.028, `zMiscSmall` +3.996.

Y la causa esta identificada en al menos un caso, con el remedio medido: en
`zFEng` eran **156 accesores triviales en clase**, que salen porque **la regla
«en clase = inline» NO vale si la clase es POLIMORFICA** (con virtuales y su key
method en el TU, GCC 2.95 exporta la clase y emite todos sus miembros fuera de
linea). **`-fno-implement-inlines` quita 2.240 de sus 2.488 B** y deja la
diferencia en **tres simbolos, ni uno mas**.

**Para la ronda 28:** el reparto util no son los 393.208 B, son **los ~303.000 de
las SourceLists**, y la primera pregunta de cada una es si su exceso es de la
misma familia que el de `zFEng` — porque entonces la cura es **una bandera de
biblioteca**, no fuente.

## La otra convergencia de la ronda: los cinco niveles del planificador

Dos agentes llegaron a la misma pieza sin saberlo:

- el de zBWare/zDynamics midio la anomalia — **`sched1` elige una insercion de
  PRE de prioridad 2 por delante de un `fsubs` de prioridad 10**, y `sched1` y
  `sched2` ordenan al reves — y concluyo que este `cc1plus` no sigue la regla de
  GCC 2.95;
- el de zCamera **encontro la regla**: `rank_for_schedule` tiene **cinco
  niveles** y el nivel 2 es **`INSN_REG_WEIGHT`, que solo actua en `sched1`**.

Y con ella se cae una veda del proyecto: la r24 decia que el desempate por
`INSN_LUID` lo decide el orden de la fuente. **No: el LUID que ve `sched2` es el
que dejo `sched1`.** En `TrackCop` el que rompe el orden es **`regmove` /
`optimize_reg_copy_2`**, que roba el `REG_DEAD` a una copia;
`-fno-expensive-optimizations` da la secuencia del objetivo **fila por fila**,
pero la bandera es obligatoria (sin ella zCamera cae de 83,34 a **53,81 %**).

## Herramientas

- **`triage.py` trunca los nombres a 42 caracteres**: `_bOutput` acaba en
  `__va_list_tag` y sale como `..._t`, que rompe cualquier herramienta a la que
  se le pase copiado.
- **`UTLVector.h` es CRLF** y `Geometry.cpp` tiene finales **mezclados** (cuerpo
  CRLF, comentarios LF): anclar con `
` falla en silencio.
- `c27fe_secs.py` (scratchpad) es el guion que compara secciones; conviene
  subirlo a `scripts/` con la distincion biblioteca/SourceList incorporada.

---

## CORRECCION a lo que escribi arriba: la causa dominante NO es la de `zFEng`

Dije que la primera pregunta de cada SourceList era si su exceso es de la familia
de `zFEng` (accesores en clase de clases polimorficas) y que la cura seria
`-fno-implement-inlines`. **Lo he medido simbolo a simbolo y no es asi.**

`jf27_extra.py` enfrenta los simbolos `FUNC` de nuestro `.o` contra los del
extraido:

| unidad | simbolos de mas | B | de ellos ≤28 B |
|---|---|---|---|
| `zPhysics` | 203 | 43.968 | 53 (476 B) |
| `zLua` | 167 | 18.908 | 17 (236 B) |
| `zSim` | 169 | 18.312 | 65 (792 B) |
| `zAnim` | 196 | 9.076 | 124 (724 B) |

**Los accesores triviales son calderilla.** Lo que pesa son **instanciaciones de
plantilla**: `_Storage<Sim::IEntity*>` (1.244 B), `Instanceable<HSIMABLE__,…>`
(1.172), `reserve__vector<WCollisionTri>` (444), `_Rb_tree::_M_insert` (356),
destructores como `_._6IModel` (532). Y **los mismos simbolos aparecen en varias
SourceLists** (`reserve__vector<WCollisionTri>` esta en `zLua` y en `zAnim`).

### Y la causa raiz, que es UNA y es de banderas

**Nuestro `zLua.o` tiene 24 secciones y CERO `.gnu.linkonce.*`**:
`reserve__…WCollisionTri` cae en `.text` plano, seccion 1.

Pero **el `ldscript` del proyecto —que sale del enlace del original— SI las
recoge**, en las tres secciones:

    16:  *(.gnu.linkonce.t*)
    60:  *(.rodata) *(.gnu.linkonce.r*) *(.rodata.*)
    67:  *(.gnu.linkonce.d*)

O sea: **el original emitia `.gnu.linkonce` y nosotros no**. En los cflags **no
hay `-fno-weak`** ni nada equivalente (las `-f` de una SourceList son
`cse-follow-jumps, cse-skip-blocks, expensive-optimizations, fast-math,
force-addr, force-mem, gcse, move-all-movables, no-static-dtors,
rerun-cse-after-loop, rerun-loop-opt, schedule-insns, schedule-insns2`).

Sintoma colateral que encaja: el enlace ya avisa
`L0019: Symbol 'eFrameCounter' multiply defined` y lo mismo con `PPCMtdec` —
`ngcld` tolera el duplicado y se queda uno. Con `linkonce` no habria aviso.

**Para la ronda 28, la pregunta es UNA y vale ~303.000 B:** por que este
`cc1plus` no emite `.gnu.linkonce.t.*` para las instanciaciones de plantilla, y
que bandera o forma de fuente lo activa. Es una pregunta de herramientas con una
medida clara detras, no de fuente.
