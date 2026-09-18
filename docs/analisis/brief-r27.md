# Ronda 27 — instrucciones comunes

Estado: **97,66 % matched**, 18.316 funciones, `linked` 10,8773 % (429 de 590),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).
Quedan **92.480 B en 39 unidades**.

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

---

## 1. LA PRUEBA QUE MÁS TRABAJO AHORRA, y es de esta ronda

**¿La función era C, o el original ya llevaba ensamblador?** Cuenta las entradas
de `symbols/debug_lines.txt` que caen en su rango:

```
con asm en el ORIGINAL    2-4 lineas en cientos de bytes
con C                     decenas
```

Es la prueba con la que ya se justificaba `src/LibSN/metrotrk.s`, pero **nadie la
usaba como test general**. Yo censé el ensamblador escrito a mano del árbol y di
**2.820 B de «crédito falso»**; la prueba dijo que **son 656**. `IDct64_GC`
(1.740 B) tiene **dos líneas** —`gcidct.inl` 100 y 101 cubriendo 1.684 B—: el
original también llevaba `asm` ahí.

Sirve para cualquier función atascada: **si el original tiene 2 líneas de DWARF
en 500 bytes, no hay fuente que escribir.**

## 2. LA REGLA DE ESTRUCTURA, con seis medidas en contra

**Mientras quede UNA diferencia de estructura, quitar las otras EMPEORA.**

| construcción nuestra que el DWARF no tiene | al quitarla |
|---|---|
| `calcFIRCoeffs` · local `halfTmpFloat` | 89,252 → 87,060 |
| `ActualReadJoystickData` · local `v` | 97,355 → 96,499 |
| `Convert32To16` · local `result` | 92,674 → 84,302 |
| `GTrigger` · local `triggerFlags` | 99,58 → 97,97 |
| `NotificationMessage` · referencia `mgr` | 98,886 → 97,599 |
| `LoadSpeechBank` · **etiqueta** `found` + 4 `goto` | 95,316 → 85,506 |

**El único caso a favor —`FindConditionBranchTarget`, 65 → 100 %— quitó las tres
cosas A LA VEZ** (dos locales, la etiqueta y el bloque anónimo), reescribiendo la
función entera contra el volcado. El diff de DWARF dice **si la función está a
una reescritura completa**; usar sus líneas de una en una falla seis de seis.

Y el corolario desde el otro lado: `srender` cerró **añadiendo** un `continue` y
**conservando** un `__asm__("")` declarado como deuda. A veces la construcción de
más hace falta — pero **si la tuya lo es, DILO**.

## 3. Herramientas: seis trampas nuevas, todas costaron medidas

1. **`lreg.py` está roto por tres sitios. Usa `scripts/alloc.py`.** Lista pseudos
   que nunca llegan a `global_alloc` (99 de 106 en un caso), omite el factor
   `size` y casa el nombre por subcadena. **Y el `.greg` trae el orden REAL
   literal** en `;; N regs to allocate:`.
2. **`-fsched-verbose-5`, CON GUION** (con `=N` da «Invalid option» y sale vacío).
   Y **con `-dS` el verbose de `sched1` va al `.sched` y el de `sched2` a
   stderr**; con `-dR`, la de sched2 va **dentro del `.sched2`**. Mirar sólo
   stderr hace creer que no hay verbose.
3. **La caché de `dwbody.py` se queda rancia después de CADA `build_direct.py`**
   y sigue imprimiendo resultados. Regenérala con
   `regmap.py <unidad> "<Clase::Func>" --ours` y **comprueba la fecha**.
4. **`triage.py` sin `--muro` sólo da el RECUENTO del muro, no los nombres**: una
   función que pasa de «faltan/sobran» a muro **desaparece y se lee como
   cerrada**. Y sin argumentos **sólo barre las 33 SourceLists**.
5. **Finales de línea MEZCLADOS dentro del mismo fichero** (`SFXCTL_Engine.cpp`,
   `SubTitle.cpp`, `vmbase.c`): decidir el terminador mirando el principio del
   fichero falla en silencio. Ancla **línea a línea** con `splitlines(True)`.
6. **`ngcas` falla de forma TRANSITORIA** (`Could not open output file`) y se lee
   como error de fuente. **Reintentar arregla.**

## 4. Lo que está CERRADO — no lo reabras

- **`_bOutput`**: lo decide el `INSN_LUID`, y PRE inserta **siempre** delante del
  salto. El `addi r23` del original **no puede ser una inserción de PRE**.
- **`VU0_quattom4` del original es el natural**: 25 posiciones y 36 parejas
  barridas, las dos condiciones son incompatibles.
- **El `blt`→`b`**: 14 apariciones en todo el DOL, 8 son las dos funciones
  conocidas. `expand_end_loop` es binario y el objetivo hace una rotación parcial
  que ninguna de 27 formas ni 30 banderas produce.
- **`epCalculate` no es eje de planificación**: el `psq_l` tiene prioridad 43 y
  sale en el ciclo 48; es una **antidependencia del registro que elige reload**.
- **`GenerateHorizonFog`**: el `andi.` no puede subir, su cadena hacia delante no
  existe.
- **`EvalSQT…FnStatelessQ`**: `combine` funde sólo si el pseudo es de un solo
  `set` y viene de memoria; el de los guardias lo crea PRE con cinco.
- **zGameplay (4.224 B) es negativo firme**: orden de sentencias ya casa, reparto
  idéntico, lo que queda son empates de `sched2`.
- **`#pragma interface` no es la veta** (−1.132 B en `interfaceimp`).

## 5. Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
2. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano**
   —un proceso murió sin ejecutar su `finally` y envenenó a dos agentes—, y el
   A/B va sobre los **`.ii` preprocesados**.
3. **Si tu palanca es un constructo que el original no tenía, DILO.**
4. **Antes de dar por bueno un 100 %, mide el efecto en los demás llamantes** —y
   comprueba que objdiff compara TODO: `zFEng` lee 100 % con **2.488 B de
   `.text` que no compara**.

## Método

- **Verifica tu encargo primero** (`build_direct.py`, luego `triage.py --muro`).
  Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** `c1`, `c2`… con su cifra. Si no cierra, **revierte** y
  anota la veda **diciendo qué sentencia barriste**.
- `audit.py` una pasada al empezar; **confirma los FALLA con una segunda**;
  congela al cerrar. Detalle en `docs/analisis/r27-<grupo>.md`. **No commits.**

## Prohibido

- **Escribir ensamblador.** Van **nueve** falsificaciones retiradas. Pero antes de
  llamar falsificación a un `asm`, **pasa la prueba del §1**.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`** salvo que tu encargo lo
  diga, y ahí sólo **proponer con verificación**.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- Pines de registro: último recurso. **Aviso medido: un pin nuestro sobre r31
  compilaba MAL** y quitarlo GANÓ 4,99 pp.

## Convivencia

Scratchpad con tu prefijo `c27<grupo>_`. **Vigila el disco** (15 GB libres) y
borra tus volcados al terminar.
