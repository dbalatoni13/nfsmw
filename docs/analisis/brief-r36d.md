# Ronda 36d — el empujón sobre lo que ya está diagnosticado

Estado: **98,33 % matched** (18.338 / 18.432 funciones), **18,64 % linked**
(476 / 617), `main.dol` `9619ba57c991`.

**Lee `brief-r36c.md` primero** (las cuatro palancas y las dos medidas que
engañan) y `brief-r36b.md`. Esto sólo añade lo de la r36c.

## Las CINCO palancas, y con qué va cada una

1. **Pin de registro** `register float x asm("fr7")` — contra el REPARTO. Lo
   único que llega a `local_alloc`.
2. **Barrera selectiva** `asm("" : "+f"(x))` — **cuatro usos y DOS ejes**:
   adelanto de sched, coalescing, hundimiento interbloque, y plegado de CSE.
   Los ejes son *dónde/cuántas* y **a quién** — el destinatario sube `n_refs`,
   que es el numerador de `allocno_compare`, y eso nadie lo tocó en seis rondas.
3. **Barrera de ranura** `asm("" : "+m"(v) : : "r0")` — ocupa una ranura de
   emisión. Ojo: en `ToggleCapsLock` cerró con `"+r"` y **sin** el clobber, y lo
   que decidió fue **de qué lado del store** va.
4. **Etiqueta de tipo** `enum { _tag };` — la única construcción que abre un
   `lexical_block` vacío sin emitir una instrucción. 90 en el original.
5. **`DECL_COMDAT`** — interfaz explícita acotada a la clase, para la inline que
   sale una vuelta tarde.

## Reglas de aplicación que costaron una ronda cada una

- **Si el pin EMPEORA, el reparto es SÍNTOMA**: arregla antes la primera
  diferencia que NO sea de registro. Así cerró `RenderString` (61 filas de
  registro caían solas al corregir un `(a+C)+b`).
- **Un `regs=N` alto del triaje no siempre es reparto**: en `MsgPlayMiscSound` 8
  de 28 eran desplazamiento; en `epCalculate` los «124» eran tres locales.
- **Repasa `regmap` DESPUÉS DE CADA PIEZA**, no sólo al principio: el censo no ve
  una variable que el original aloja y nosotros no. Así cerró `Smackable`.
- **El pin no FIJA el registro, lo sugiere** — puede miscompilar.
- **Un pin sobre una local con la dirección tomada se ignora en silencio.**
- **El radio del pin es el rango de vida entero, desde la declaración.**
- **No vale en unidades de Metrowerks** (`LibSN/*`): error de sintaxis.
- **El marco y el reparto son el mismo problema**: con un preservado de menos el
  asignador reserva 8 B que no usa.

## Y la trampa de verificación que me colé yo

**El censo de «0 funciones ausentes, 0 con tamaño distinto» NO ve un cambio de
CONTENIDO a igual tamaño.** Reordené dos asignaciones en `OnManageTime` para
arreglar el pool: seguía midiendo 564 B, mi censo decía «0 y 0», y había caído
de 100 % a 96,809 %. Lo cazó el `ninja` completo dos horas después.

**Pasa `fndiff <unidad> <símbolo>` a las funciones que toques, antes de dar nada
por bueno.** Y el `%` de `triaje.py` sale de un `report.json` que
`build_direct.py` deja rancio: sólo el `difs` es fresco.

## Método

- Verifica tu encargo primero; si no reproduce, dilo antes de tocar nada.
- Ensayos numerados con su cifra; si no cierra, revierte y anota la veda.
- `build_direct.py <unidad>`; **nunca `ninja` completo**. `lcfix.py --check` al
  terminar si tocas literales.
- **Sólo los ficheros de tu lista.** Informe en `docs/analisis/r36d-<grupo>.md`.
  **No commits.** `configure.py`/`config/`/`splits.txt`: proponer, no aplicar.
- No rompas los `#if defined(__ANDROID__)` de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp`.
