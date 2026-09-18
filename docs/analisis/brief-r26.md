# Ronda 26 — instrucciones comunes

Estado: **97,40 % matched**, 18.313 funciones, **`linked` 10,8773 %** (429 de
590), `main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).
Quedan **102.420 B en 39 unidades**.

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

Está todo ahí. **No repitas este documento en tu cabeza: consúltalo.**

---

## 1. HERRAMIENTAS: tres cosas que cambiaron y te ahorran la tarde

### `lreg.py` está roto de TRES formas. Usa `scripts/alloc.py`.

Lo destapó la ronda 25 y es el tercer arreglo de la misma herramienta:

1. **Lista pseudos que nunca llegan a `global_alloc`** (41 de 69 en una función,
   **99 de 106** en otra). A `global_alloc` sólo llegan los que `local_alloc` no
   asignó.
2. **Omite el factor `size`** de `allocno_compare`: `pri = floor_log2(n_refs) *
   n_refs / live_length * 10000 * size`, y un `double` vale 2, o sea **dobla su
   prioridad**.
3. **Casa el nombre por SUBCADENA**: `"Smackable::Simplify"` también casa
   `SimplifySort`.

**Y no hace falta calcular nada: el `.greg` trae el orden REAL literal** en la
línea `;; N regs to allocate: ...`, que es `allocno_order` ya ordenado, justo
antes de `find_reg`. `scripts/alloc.py` lo lee y además verifica que la fórmula
reproduce ese orden. **Cualquier tabla de prioridades anterior al 5-sep hay que
rehacerla.**

### `-fsched-verbose=3` NO EXISTE. Es `-fsched-verbose-5`, CON GUION.

Con `=N` da «Invalid option» y `cc1plus` **sigue sin verbose**: la salida sale
vacía y se lee como «aquí no hay nada que ver». Con guion imprime
`INSN_PRIORITY(...) = N` y el ciclo en que se emite cada instrucción, que es lo
que cerró dos diagnósticos de planificación en la ronda 25.

Recordatorio: `ngccc` pasa `-da` a `cc1plus` pero **borra el temporal**;
preprocesa a tu propio `.ii` y llama a `cc1plus` a mano.

### `triage.py` es ciego a dos cosas

- **Sin argumentos sólo barre las 33 SourceLists.** Las 18 unidades de middleware
  con pérdida —13.268 B en 38 funciones— no salen, y por eso llevaban rondas sin
  agente.
- **Sin `--muro` sólo imprime el RECUENTO de la sección MURO, no los nombres.**
  Una función que pasa de «faltan/sobran» a muro **desaparece del listado y se lee
  como cerrada**. Le costó un falso positivo a un agente de la r25.

Y **`regmap.py` no acepta rutas para el middleware**: la llave es el nombre pelado
(`srender`, `sfir`, `inittmr`), y con una ruta dice «unidad desconocida».

---

## 2. EL DIAGNÓSTICO MÁS RENTABLE, y sus tres límites

**Una local nuestra que el DWARF del original no tiene le roba el registro duro
a la que sí.** `dwbody.py <unidad> "<Clase::Func>" both`, una pasada, antes de
gastar un ensayo.

Caso testigo de la r25: `FindConditionBranchTarget` (144 B) pasó de **65,278 % a
100 % en la PRIMERA compilación**, y con ella `zFEng` entera. El fichero llevaba
tres formas ya medidas en un comentario y **ninguna tenía la estructura del
original**: dos locales a nivel de función y nada más, sin bloque anónimo, con
`Result` compartiendo r4 con el parámetro. Nuestra `count` de más se llevaba
**r3** —el registro de retorno— y el `return` de salida costaba `bne`+`mr`+`blr`
donde el objetivo hace un solo `beqlr`.

**Los tres límites, todos medidos, respétalos:**

1. **Disolver la local que sobra NO siempre paga**: `calcFIRCoeffs` 89,25 →
   **87,06 %**; `ActualReadJoystickData` 97,36 → **96,50 %**.
2. **Los temporales del compilador no salen en DWARF.** En `calcFIRCoeffs` el
   objetivo **calcula igualmente** el valor «ausente» y lo reutiliza en f30.
3. **Un tipo del DWARF no es evidencia de la firma**: `dvd_device` da `int` donde
   el nombre decorado dice `...Pvll`. DWARF codifica `int` y `long` igual.

Corolario útil: **el bloque anónimo de más suele costar cero**, pero subirlo deja
el fuente de acuerdo con el volcado para el siguiente.

---

## 3. Lo que la ronda 25 dejó CERRADO — no lo reabras

- **`#pragma interface` NO es la veta.** Medido en cuatro candidatas: las vtables
  **sí son de la unidad**; lo que falla es que `splits.txt` no le asigna
  `.rodata`. El pragma cuesta **−1.132 B** en `interfaceimp` y **504 B de `.text`**
  en `csis`.
- **El `blt`→`b` compartido: misma causa, NO es frente.** El patrón sale **14
  veces en todo el DOL** y **8 son las dos funciones conocidas**. `expand_end_loop`
  es binario y el objetivo hace una rotación parcial que ninguna de 27 formas ni
  30 banderas produce.
- **La deuda del `while+break` de `ResolveCarBanks`: no hay ruta sin bucle**,
  demostrado con cuatro medidas.
- **`VU0_quattom4` del original es el natural**: las 25 posiciones y 36 parejas
  están barridas, y las dos condiciones son incompatibles.
- **`_bOutput`**: lo decide el `INSN_LUID`, no una antidependencia (la r24 se
  equivocaba); PRE va siempre al final del bloque y por eso pierde la ranura.

---

## 4. Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
   Con siete agentes a la vez, `--cmp` contra una base de hace una hora **no mide
   tu cambio**: en la r25 dos agentes cantaron −14.828 B y −18.244 B que eran de
   un tercero.
2. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano.**
   En la r25 un proceso murió sin ejecutar su `finally` y dejó
   `UVectorMathGC.hpp` tocada; envenenó las medidas de dos agentes. Restaura en
   **cada punto**, no sólo al final, y corre por tramos en primer plano.
3. **Si tu palanca es un constructo que el original no tenía, DILO.** Dos deudas
   así se declararon en la r25 y por eso se pudieron resolver.
4. **Antes de dar por bueno un 100 %, mide el efecto en los demás llamantes.**

## Método

- **Verifica tu encargo primero** (`build_direct.py`, luego `triage.py --muro`).
  Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** `c1`, `c2`… **cada uno con su cifra**. Si no cierra,
  **revierte** y anota la veda **diciendo qué sentencia barriste**.
- `audit.py` una pasada al empezar; **confirma los FALLA con una segunda**;
  congela al cerrar con `frozen.py`.
- Detalle en `docs/analisis/r26-<grupo>.md`. **No hagas commit.**

## Prohibido

- **Escribir ensamblador.** Van **seis** falsificaciones retiradas; la última
  fabricaba 128 B de los que sólo 96 eran de la unidad.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`**, salvo que tu encargo diga
  otra cosa — y ahí sólo **proponer**, con su verificación.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- Restricciones de registro: último recurso, y **tienen que pasar `audit.py`**.
  Aviso de la r25: **`asm("r31")` compila MAL** — r31 es `FRAME_POINTER_REGNUM`
  en rs6000 y GCC mete otra variable en el mismo registro.

## Convivencia

Scratchpad con tu prefijo `c26<grupo>_`. **Vigila el disco** (14 GB libres) y
borra tus volcados al terminar.
