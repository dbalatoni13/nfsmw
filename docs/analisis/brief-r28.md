# Ronda 28 — instrucciones comunes

Estado: **97,67 % matched**, 18.317 funciones, **`linked` 11,28 %** (432 de 595),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).
Quedan **91.992 B en 39 unidades** de near-miss… **y ~303.000 B de `.text` que
nadie compara** (§1).

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

---

## 1. EL FRENTE NUEVO: 303.000 B que objdiff no compara

`objdiff` compara los símbolos que **empareja**. Lo que emitimos de más no sale
en ningún diff, así que **una unidad puede leer 100 % con miles de bytes que el
original no tiene**. Medido unidad por unidad contra el objeto extraído:

```
zPhysics +43.960   zEAXSound +24.080   zMain +20.188   zLua +18.908
zSim     +18.312   zPhysBeh  +14.996   zFe   +14.572   zEAXSound2 +13.752
```

**Y no es cosmético: `zLua` figura al 100 % y `trypromo.py` dice `DOL ROTO`.**

**Dos matices, los dos medidos, y sin ellos la cifra engaña:**

- **En BIBLIOTECAS el exceso es inocuo.** `mtx.c` está `Matching` con **+5.376 B**
  y el DOL casa: el objeto extraído sólo trae lo que quedó en el DOL, nosotros
  compilamos la unidad entera. **No lo persigas ahí.**
- **En SourceLists SÍ bloquea**, y la causa dominante **no** son accesores:
  son **instanciaciones de plantilla** (`_Storage<Sim::IEntity*>` 1.244 B,
  `Instanceable<…>` 1.172, `reserve__vector<WCollisionTri>` 444), y **los mismos
  símbolos aparecen en varias SourceLists**.

**La pista raíz, sin explotar:** nuestro `zLua.o` tiene 24 secciones y **CERO
`.gnu.linkonce.*`** — las plantillas caen en `.text` plano. Pero el `ldscript`,
que sale del enlace del original, **sí las recoge** (`*(.gnu.linkonce.t*)`,
`.r*`, `.d*`). En los cflags **no hay `-fno-weak`**. Y el enlace ya avisa
`L0019: Symbol 'eFrameCounter' multiply defined`, que con `linkonce` no pasaría.

**La otra causa, con la cura ya medida:** en `zFEng` eran **156 accesores en
clase**, y salen porque **«en clase = inline» NO vale si la clase es
POLIMÓRFICA** — con virtuales y su *key method* en el TU, GCC 2.95 exporta la
clase y emite todos sus miembros fuera de línea. **`-fno-implement-inlines` quita
2.240 de sus 2.488 B** y deja la diferencia en **TRES símbolos**.

## 2. Tres reglas nuevas, las tres extrapolables

1. **El ORDEN de los operandos de un `|` decide el árbol de `fold`, y el árbol
   decide qué registro acumula.** Barrer las 24 permutaciones cuesta 2 min y
   valió **+6,7 pp** en `UnlockPalette` (92,674 → 99,419). Y
   `((a >> 5) << 12)` → `((a & 0xE0) << 7)` **bloquea la fusión de `combine`**
   que mantenía vivo el operando: +2,7 pp por sí sola.
2. **El ORDEN de declaración de los miembros DENTRO de la clase decide si GCC 2.9
   hace inline.** Con `EvalFrame` declarada *después* de `Eval`, la emite fuera
   de línea con un `bl`: **74,18 % contra 100 %**. El volcado da el orden real.
3. **`rank_for_schedule` tiene CINCO niveles y el nivel 2 —`INSN_REG_WEIGHT`—
   sólo actúa en `sched1`.** De ahí que `sched1` y `sched2` ordenen al revés. Y
   **el LUID que ve `sched2` es el que dejó `sched1`, no el orden de la fuente**
   (esto tumba una veda de la r24). En `TrackCop` quien rompe el orden es
   **`regmove`/`optimize_reg_copy_2`**, que roba el `REG_DEAD` a una copia.

## 3. La regla de estructura: SIETE contraejemplos, y cómo leerla bien

**Mientras quede UNA diferencia de estructura, quitar las otras EMPEORA.** Van
siete: `halfTmpFloat` −2,2 · `v` −0,9 · `result` −2,7 · `triggerFlags` −1,6 ·
`mgr` −1,3 · la etiqueta de `LoadSpeechBank` −9,8 · los tres `goto` de
`AssignClosestOffsets` **−1,24**. El único caso a favor
(`FindConditionBranchTarget`, 65 → 100 %) quitó **las tres cosas a la vez**.

**Y una corrección de método que costó un ensayo:** las `// Labels` de `dwbody`
prueban que el DWARF del original **no las NOMBRA**, no que no existan. En
`AssignClosestOffsets` el objetivo **sí** tiene el bloque fuera de línea que
produce el `goto`.

**Las vedas CADUCAN al mejorar el fuente debajo**: «quitar `result` da 84,30 %»
era cierto sobre la forma vieja; con los `|` reordenados da **96,74**.

## 4. Herramientas: lo que falla y cómo

- **`lreg.py` está roto por tres sitios. Usa `scripts/alloc.py`.**
- **`-fsched-verbose-5`, CON GUION.** Con `-dS` el verbose de `sched1` va al
  `.sched` y el de `sched2` a stderr; con `-dR`, el de sched2 va **dentro del
  `.sched2`**. Mirar sólo stderr hace creer que no hay verbose.
- **La caché de `dwbody.py` se queda rancia tras CADA `build_direct.py`** y
  **sigue imprimiendo**. Regenérala con `regmap.py <u> "<C::F>" --ours` y mira la
  fecha.
- **`triage.py`**: sin `--muro` sólo da el recuento del muro; sin argumentos sólo
  barre las 33 SourceLists; **trunca los nombres a 42 caracteres**; y **su
  columna de multiconjunto no es estable entre pasadas** con el objeto sin tocar
  (`lwz-1` ↔ `stw-1`) — se lee como regresión y no lo es.
- **`pines.py` no ve las barreras con clobber** (su regex sólo casa `asm("rN")`):
  hay **6** en el árbol.
- **`audit.py` compara VALORES de literal**, así que no ve un literal duplicado.
- **Finales de línea MEZCLADOS dentro del mismo fichero** (`SFXCTL_Engine.cpp`,
  `SubTitle.cpp`, `Geometry.cpp`, `vmbase.c`) y **`UTLVector.h` es CRLF**: ancla
  **línea a línea** con `splitlines(True)`.
- **`ngcas` falla de forma TRANSITORIA**; reintentar arregla.
- **El grafo ya está arreglado**: `splits.txt` y `symbols.txt` son ahora
  dependencias del troceado. Antes, cambiarlos no volvía a trocear y **el DOL
  salía roto en silencio**; eso hizo devolver un paquete correcto.

## 5. Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
2. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano**,
   y el A/B va sobre los **`.ii` preprocesados**.
3. **Si tu palanca es un constructo que el original no tenía, DILO.**
4. **Antes de dar una unidad por terminada, compara el TAMAÑO DE SUS SECCIONES**
   contra `build/GOWE69/obj/<ruta>.o`, no sólo su porcentaje.

## Método

- **Verifica tu encargo primero** (`build_direct.py`, luego `triage.py --muro`).
  Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra. Si no cierra, **revierte** y anota la veda
  **diciendo qué sentencia barriste**.
- `audit.py` una pasada al empezar; **confirma los FALLA con una segunda**;
  congela al cerrar. Detalle en `docs/analisis/r28-<grupo>.md`. **No commits.**

## Prohibido

- **Escribir ensamblador.** Van nueve falsificaciones retiradas. Pero antes de
  llamar falsificación a un `asm`, **cuenta las líneas de DWARF del original en
  su rango**: con `asm` en el original salen 2-4 en cientos de bytes, con C
  decenas. Esa prueba evitó retirar 2.164 B legítimos.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`** salvo que tu encargo lo
  diga, y ahí sólo **proponer con verificación**.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- Pines de registro: último recurso. **Un pin nuestro sobre r31 compilaba MAL** y
  quitarlo GANÓ 4,99 pp.

## Convivencia

Scratchpad con tu prefijo `c28<grupo>_`. **Vigila el disco** (16 GB libres).
