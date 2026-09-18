# R48 — cierre

2026-09-09. Siete agentes en paralelo, más el mapa del frente de `linked` hecho
en coordinación mientras trabajaban. DOL verificado en
`9619ba57c9919f95f7f2ac951a2166a3517f91e3` después de cada paso.

## Estado

| medida | r47 | ahora |
|---|---:|---:|
| código exacto | 3.905.708 / 3.946.048 B — 98,98 % | **3.908.380 — 99,04542 %** |
| funciones exactas | 18.394 / 18.432 | **18.399** |
| unidades linked | 493 / 619 | **494 / 618** |

**`matched` cruza el 99 %.** Quedan **37.668 B en 32 funciones de 13 unidades**
—de 40.340 B, 37 funciones y 17 unidades—. `zFeOverlay`, `pathnode` y `sfir`
quedan sin ninguna función pendiente.

## Cerrado: seis funciones, 2.672 B

| unidad | función | B |
|---|---|---:|
| `sfir` | `calcFIRCoeffs` | 936 |
| `zEAXSound2` | `ProcessUpdate__12CARSFX_Turbo` | 672 |
| `zFeOverlay` | `SetScreenNames` | 440 |
| `zSpeech` | `LoadSpeechBank` | 316 |
| `pathnode` | `PATHI_nextnode` | 308 |

Más `InitAtSegment` de 15 filas a 12 **quitando** un pin, y
`ActualReadJoystickData` de 19 a 16.

## El patrón de la ronda: la causa casi nunca está donde señala el diff

Cinco de los seis cierres vinieron de mirar a otro sitio:

- **`SetScreenNames`**: seis filas de reparto, y la causa era que el original
  expandía `IsInBackRoom()` inline y nosotros llamábamos a la función libre. Una
  línea, cero bytes. **Y con el `asm "+m"` de deuda puesto, la misma corrección
  daba 15 filas; sin él, cero.** Un `asm` puede tapar la causa real.
- **`ProcessUpdate`**: r46 y r47 la dieron por «cerrada por construcción» como
  problema de planificación. **No era planificación, era reparto**: el `.lreg`
  dice que `nDMixOut` cruza dos llamadas y vive en r30 entero. Asignando el
  desplazamiento a la variable de aguas abajo, el `srawi` escribe r3, `sched2`
  ve una antidependencia y su `INSN_PRIORITY` salta de 2 a 17 — la ranura del
  objetivo. **Una diferencia de planificación dentro de un bloque puede tener la
  raíz en el asignador.**
- **`LoadSpeechBank`**: el diagnóstico heredado (`cse-follow-jumps`) era el
  síntoma. `lmap.py` **sobre el original** enseña que su línea se lleva tres
  comparaciones: la condición del `while` incluía la prueba de la clave.
- **`sfir`**: era PRE, pero **no hacía falta ningún flag**. La condición está en
  `oprs_unchanged_p`, y `reg_first_set` **se reinicia por bloque**.
- **`UpdatePlatInfo`** (no cerrada): las 27 filas son `regs_someone_prefers` —
  cuando le toca el turno al pseudo, sus registros están **libres**, y los pierde
  porque dos allocnos de *menor* prioridad los prefieren, por el pin de otra
  variable. El culpable no aparece en el diff, igual que en
  `HolePunchAvoidables`.

## Frentes cerrados por construcción

- **`HolePunchAvoidables`**: el «+7 insns» no existe. El modelo
  `live(x) = T + s(x)` cuadra al insn contra el `.lreg`; con 7 `asm("")` el
  reparto CR sale exacto pero destapa un **segundo** empate de truncamiento, y
  las longitudes válidas de los dos —`{−10,−9,−2,−1,+7,+8}` y `{4,5,10,11}`— **no
  se cortan salvo en −1**, forma que da 170 filas.
- **`Initialize`** (zEagl4Anim): el `ble` lo pone `jump.c:1831`, y su condición
  exige que el nodo `{2}` sea acotado o un rango: ninguna se da sin cambiar la
  semántica.
- **`Setup__RoadblockFlow`**: empate exacto en **todos** los niveles de
  `rank_for_schedule`; decide `INSN_LUID`, heredado de `sched1`, y el empate está
  *dentro* de la secuencia que el front-end emite para la virtual.

## El mapa del frente de `linked` (coordinación)

Ver `r48-jf-censo-linked.md`. Cuatro medidas, y el frente pasa de «1,64 MB de
código perfecto que no promociona, no sabemos por qué» a un predicado:

> una unidad promociona si y sólo si **todos** sus símbolos de más son huérfanos
> por los dos lados **y** ningún otro objeto del enlace los define.

**20 aciertos y 1 fallo** contra la realidad (`scripts/promopred.py`). Con él,
los cientos de símbolos de más se quedan en **uno o dos por unidad** — y **un
solo símbolo, `_Rb_tree::_M_erase` de 104 B, bloquea VEINTE unidades**.

Y dos correcciones grandes:

- **El déficit de datos no existe.** De los 85.349 B que «faltan», 78.364 son
  literales con otro nombre, 5.217 el relleno del troceador y 1.764 el mismo
  dato con otro contador de sufijo. **Lo que faltaba de verdad eran 4 bytes**, y
  eran un `static int counter;` sin inicializador.
- **«Sección descuadrada» y «símbolo desplazado» son síntomas, no causas.**
  Nuestro `.text` mide más porque emitimos símbolos de más; las tres familias
  grandes del mapa (22, 20 y 16 unidades) son la misma cosa vista en tres sitios.

## La ventana

| paso | resultado |
|---|---|
| `static int counter = 0;` | los 4 B del censo de datos, `zEAXSound` sigue 0/773 |
| `sfir`: rango `.rodata` + promoción | **+936 B linked**, DOL OK |
| `pathnode`: rango `.rodata` | **retirado con medida** — ver abajo |
| `-fno-implicit-templates` en `zFeOverlay` | **−5.880 B de exceso**, 0 de 467 funciones |

**El rango de `pathnode` no vale.** La propuesta era reclamar
`0x80413A48..0x80413AE0`, pero ahí el objetivo tiene 152 B donde nosotros
emitimos 32, y once de sus `lbl_` los referencia el comodín
`auto_01_8037C0F8_text`. Reclamarlo entero obligaría a definir once símbolos que
no tenemos.

## Correcciones al catálogo

1. **`asm("" : : "r"(x))` de sólo entrada NO siempre es gratis.** Un `asm` sin
   salidas es volátil: emite salvo que GCC lo absorba en una copia preexistente.
   Cero bytes en `cStichWrapper::Play`, 67 → 68 instrucciones en `eProject`.
2. **El racimo de `TrackCar` lo decide `sched2`, no `sched1`**, y en `sched2` el
   nivel de `INSN_REG_WEIGHT` **no existe** (`if (!reload_completed …)`): decide
   `depend_count`. El diagnóstico de la r47 salió de mirar la traza equivocada.
3. **La «veta viva» del presupuesto de `cse` de zCamera no existe** — las cargas
   «baratas» y «caras» son construcciones distintas, no dos formas de la misma.
   La pasé yo en el brief sin verificarla.
4. **`sizeof(FloatVector)` es 0xC**, y los 16 B de `GetLoadingPriority` no son un
   hueco: el original ahorra 32 B solapando dos `face[4]` y gasta 48 donde
   nosotros no dejamos nada.
5. **Un pin puede ser andamio**: en `InitAtSegment` retirarlo ganó tres filas.

## Infraestructura

**El disco llegó a 0 B libres a mitad de ronda**, y eso se ve como un
`cc1plus: I/O error` que parece un fallo de compilador. Limpiados 3,5 GB de
`.tmp` viejos, cachés y restos de rondas antiguas, más 225 MB de volcados RTL.
**Hay que barrer esto entre tandas**: cada volcado ocupa 20-30 MB.

## Herramientas nuevas

`scripts/rtldump.py` (volcados RTL, con la ruta del repo quitada de a fuego),
`scripts/refs.py` (índice de referencias), `scripts/promopred.py` (el
predicado), `scripts/promomap.py` (el mapa, en paralelo),
`scripts/schedtrace.py` (la traza del planificador ciclo a ciclo). Más:
`fncmp.py` ya imprime el detalle cuando el tamaño difiere —antes se quedaba mudo
justo en las funciones de ±4 B— y `promote.py` cachea en disco el índice de
UNDEF.
