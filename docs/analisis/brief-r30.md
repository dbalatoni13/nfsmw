# Ronda 30 — instrucciones comunes

Estado: **97,96 % matched**, 18.330 funciones, **`linked` 12,18 %** (443 de 599),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).
Quedan **80.380 B en 37 unidades**.

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

---

## 1. LA BARRERA: tres formas, y sirve una de cada tres veces

Tras tres correcciones en la ronda 29, así queda:

```c
__asm__("")                          barrera TOTAL
__asm__("" : : "r"(x))               TAMBIEN total  (sin salida => volatil)
__asm__("" : "=f"(x) : "0"(x), …)    selectiva: exige salida, VIVA y atada con "0"
__asm__("" : : "m"(x))               declara una LECTURA de la ranura
```

**GCC 2.95 pone `MEM_VOLATILE_P` a TODO asm sin operandos de SALIDA.** Con salida
muerta, `insn_dead_p` borra la insn entera.

Tres cosas medidas que hay que respetar:

- **Es DIRECCIONAL: va DETRÁS de la instrucción que llega tarde, no delante.**
  Doce ensayos poniéndola delante dan **objeto idéntico**.
- **Un asm no volátil cuenta como REFERENCIA sin emitir un byte.** Eso cerró
  `CheckCollideSRB`: `trig` de 28 a 29 refs, `pri` 5628 → **5800**. Es la palanca
  para un empate de `allocno_compare` que no se puede romper moviendo insns.
- **La forma exacta importa**: en `Smackable`, `"m"` corta la extensión de bloque
  de `cse` y `"=m"`, `"+m"` y `"m"`+`"memory"` dan **binario idéntico a la base**.

**Y la extrapolación, CONTADA: de las 8 funciones del árbol con «una insn en otra
ranura», cierra 2, mejora mucho 1, y no hace nada o EMPEORA en 5.** No es una
llave maestra. En `IconScroller` la destruye (48-85 %) porque rompe el CSE del
cero.

**Es siempre deuda declarada: el original no tenía asm. DILO, y di si el DWARF y
el mapa de líneas lo contradicen.**

## 2. EL FRENTE ESCRIBIBLE: `.data`, no `.rodata`

En `.rodata` **el 94 % del dato que falta está MUERTO** (3.319 B vivos contra
51.064) y **ninguna unidad está a tiro de promoción** (15 no enlazan, 17 dan DOL
roto, 0 OK). **No lo persigas.**

**En `.data` es al revés: faltan 7.584 B y 4.190 están VIVOS** — tablas y
globales inicializadas que **sí se escriben en C**.

| unidad | d.data | d.rodata |
|---|---|---|
| `zFe` | **−1.548** | −1.928 |
| `zEAXSound` | −912 | −944 |
| `zFeOverlay` | −748 | +432 |
| `zFe2` | −580 | −4.032 |
| `zLua` | −496 | −672 |
| `zTrack` | −480 | −1.176 |

Y la lista de trabajo del frente de enlace: **26 unidades con el `.text` YA
IDÉNTICO, 45.992 B**, bloqueadas sólo por rangos de datos (`c29lk_ready.py`).
`satospkr` (1.796 B) está **a una frontera**.

## 3. Reglas de codegen nuevas (r28-r29), todas extrapolables

1. **Los *setters* generados a los que les falta el clamp.**
   `expand_inline_function` **sólo materializa el argumento en un pseudo si el
   parámetro se MODIFICA dentro del inline**; un setter con clamp deja un
   `li rN,0` vivo que `cse` reutiliza y que obliga a un registro salvado. **Se
   detecta con `lmap.py` sin adivinar**: la distancia *línea de apertura del
   inline → línea del store* vale **7 con dos clamps, 6 con uno, 1-2 sin clamp**.
   Cerró `Play__cSampleWarpper` (2.152 B) tras varias rondas.
2. **Un `lis sym@ha` izado sobre un `bl` = FALTA UNA CONVERSIÓN**
   (`expand_assignment` sólo usa el atajo con un `CALL_EXPR` pelado). Cerró
   `DataLoadCB` cambiando `extern unsigned int` → `int`.
3. **El DWARF dice QUÉ LOCAL es el registro acumulador**: en `UpdateRPM` el
   objetivo acumula en f1 y el volcado da `Cur_RPM // f1` — el «temporal» era una
   local ya muerta.
4. **El ORDEN de los operandos de un `|`** decide el árbol de `fold`; y lo que
   suele faltar es **sacar `(C|r)` a un TEMPORAL que no sea el destino**.
5. **El ORDEN de declaración de los miembros DENTRO de la clase** decide si GCC
   hace inline (74,18 % contra 100 %).
6. **El orden de las sentencias de un bloque no lo conserva el planificador**:
   24 permutaciones son 2 min, y el orden «natural» puede ser **el peor**.

## 4. Vedas y trampas de lectura que cuestan rondas

- **En FLOTANTES `X > C` y `C < X` son byte a byte IDÉNTICOS** (`fold`
  canonicaliza antes de expandir). Cualquier barrido que los permute tira el
  presupuesto.
- **`stmw rN` NO dice cuántos valores hay**: arranca en el registro salvado más
  **BAJO USADO**. Yo repartí una ronda con «al objetivo le cabe un valor más» y
  era falso — contados, los dos lados tienen los mismos y al objetivo le queda un
  **hueco**.
- **Las `// Labels` de `dwbody` prueban que el DWARF no las NOMBRA, no que no
  existan.**
- **Mientras quede UNA diferencia de estructura, quitar las otras EMPEORA** (7
  contraejemplos; el único caso a favor quitó las tres a la vez).
- **Las vedas CADUCAN al mejorar el fuente debajo**, y **dos vedas ciertas por
  separado pueden ser falsas en combinación** (`Smackable`).

## 5. Herramientas: lo que falla

- **`lreg.py` roto por tres sitios → `scripts/alloc.py`**, y **comprueba en el
  `.greg` que el pseudo llega a `global_alloc`**: si lo asigna `local_alloc`,
  `alloc.py` no puede decir nada.
- **`-fsched-verbose-5` CON GUION**; con `-dR` la salida de sched2 va **dentro
  del `.sched2`**.
- **El volcado DWARF tiene 59 funciones SIN CUERPO**: «no aparece» ≠ «no existía».
- **La caché de `dwbody.py` se queda rancia tras CADA `build_direct.py`.**
- **`triage.py`**: sin `--muro` no da nombres; **trunca a 42 caracteres**; y su
  columna de multiconjunto **no es estable entre pasadas**.
- **`audit.py` da FALLA fantasma** con el `build/` compartido: **confírmalos
  siempre con una segunda pasada** (cuarta vez que pasa).
- **Comparar cuerpos de función en crudo MIENTE**: los campos de `bl` y
  reubicación los parchea el enlazador.
- **Finales de línea MEZCLADOS** dentro del mismo fichero; **`UTLVector.h` es
  CRLF**. Ancla línea a línea con `splitlines(True)`.

## 6. Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
2. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano.**
3. **Si tu palanca es un constructo que el original no tenía, DILO.**
4. **Antes de dar una unidad por terminada, compara el tamaño de sus secciones**
   (`scripts/seccdiff.py`), no sólo su porcentaje.

## Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra. Si no cierra, **revierte** y anota la veda
  **diciendo qué sentencia barriste**.
- `audit.py` una pasada al empezar, **FALLA confirmados con una segunda**;
  congela al cerrar. Detalle en `docs/analisis/r30-<grupo>.md`. **No commits.**
- **Si dejas un fichero modificado que rompe el enlace, DILO EN EL INFORME**: en
  la r29 un `maddeca.cpp` sin revertir tuvo el árbol sin enlazar media ronda.

## Prohibido

- **Escribir ensamblador de instrucciones.** Van diez falsificaciones retiradas.
  Antes de llamar falsificación a un `asm`, **cuenta las líneas de DWARF del
  original en su rango** (con `asm` salen 2-4 en cientos de bytes) — y recuerda
  que **emitir datos con `.long` es legítimo**: retirarlo rompió el enlace.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`**: sólo **proponer con
  verificación**, salvo que tu encargo diga otra cosa.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).

## Convivencia

Scratchpad con tu prefijo `c30<grupo>_`. **Vigila el disco** (15 GB libres).
