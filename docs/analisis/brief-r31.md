# Ronda 31 — instrucciones comunes

Estado: **97,974 % matched**, 18.337 funciones, **`linked` 12,56 %** (453 de 604),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

---

## 0. CAMBIA EL OBJETIVO: esta ronda se mide en `linked`, no en `matched`

Doce rondas optimizando `matched` lo han subido de 96,52 % a 97,97 %. Lo que
queda son **101 funciones / 79.932 B, y el 85 % de esos bytes está en funciones
que ya son correctas al 95 % o más**: orden de instrucciones y reparto de
registros, el trabajo más caro del proyecto. La r30 gastó dos agentes ahí y
cerró **cero**.

Mientras tanto **`linked` está al 12,56 %**. El DOL ya sale byte a byte, así que
lo que falta para que esto sea una decompilación terminada es que el juego se
construya *desde nuestro código* — y hoy sólo el 12,56 % lo hace. **Las 33
SourceLists son 3,33 MB, el 84 % del juego, y ninguna está enlazada.**

**Y quince de ellas tienen el `.text` al 100 % con CERO funciones pendientes:
657.524 B de código perfecto que no promociona.** Las once que enlazan,
promocionadas juntas, dejan el DOL a **−9.088 B**.

**Tu encargo se mide en bytes que pasan a `linked`.** Una función de 3.120 B que
desbloquea zPhysics vale **146.124 B**, no 3.120.

## 1. POR QUÉ no promocionan: el `lbl_` contra el `$LC`

Medido esta ronda con el enlazador delante, y es el hallazgo que ordena todo lo
demás. Al promocionar zFoundation salen **61 errores**, todos de la misma forma:

```
UMath.cpp(93) : error: L0039:Reference to undefined symbol lbl_803EB444
UVectorMath.h(99) : error: L0039:Reference to undefined symbol lbl_803EB450
```

**Los objetos EXTRAÍDOS de las otras unidades referencian los literales de la
nuestra POR SU NOMBRE (`lbl_<dirección>`), y nuestro objeto los llama `$LC12`.**
Mismos bytes, nombre distinto: el enlazador no los encuentra. `objdiff` no lo ve
—por eso la unidad figura al 100 %— porque ignora los nombres de reubicación.

Clasificados los símbolos de datos que «faltan» en las 15 unidades:

| clase | bytes |
|---|---|
| `lbl_` anónimos | **30.365** |
| relleno (`gap_`/`pad_`) | 2.606 |
| con NOMBRE | 892 |
| literales `$LC` | 812 |

y al mismo tiempo emitimos ~27.000 B de `$LC` «de más». **Es el mismo dato con
otro nombre en la inmensa mayoría de los casos. No hay 30 kB de datos que
escribir.**

**Dos curas, y la primera está probada:**

1. **Ceder el rango al comodín** (`splits.txt` + `keep.lst`). Si el dato no lo
   emite nuestro objeto, que lo emita un `auto_*` con el nombre `lbl_` intacto.
   Es exactamente lo que hizo el paquete de la r30 con `quantize`, `sstvol` y
   `svol`, y dio **DOL byte a byte**.
2. **Dar el nombre**: definir en C el dato con el nombre que el original usa
   (para los pocos «con NOMBRE», que son definiciones que faltan de verdad).

## 2. Las tres unidades que fallan por símbolo INDEFINIDO

No es «símbolo duplicado» —el `L0019: PPCMtdec multiply defined` es un **aviso**
y sale en las tres—. Son referencias sin definir:

| unidad | tamaño | lo que falta |
|---|---|---|
| `zSim` | 96.400 B | `bSawLoadingScreen` (lo referencian zFe2 y zMain) y `lbl_80404864` |
| `zFoundation` | 36.000 B | 61 × `lbl_803EBxxx` (§1) |
| `zMisc` | 78.008 B | `bin_globala_bun_marker` |

Recuerda la regla ya medida: **GCC 2.9 manda `int X = 0;` a `.data` y `int X;` a
COMMON**, y varios de estos «UND» son una palabra de diferencia.

## 3. Lo que NO bloquea, y está medido — no lo persigas

- **El `.text` de más NO llega al DOL.** 317.944 B de exceso en las SourceLists
  y el `.text` enlazado da exactamente el del original: `ngcld
  -strip-unused-data` descarta los cuerpos duplicados. Sustituir `zLua.o`
  (+18.908 B) da el `.text` del objetivo al byte.
- **`-fno-implement-inlines` NO va en las SourceLists**: de las 33, **13
  empeoran** y 12 de ellas introducen símbolos indefinidos. Ya está puesta en
  `realcore`, `path`, `realmemcard`, `rcmp` y (por objeto) `csis`.
- **En `.rodata` el 94 % del dato que falta está MUERTO.** Por eso la cura es
  cederlo al comodín, no escribirlo.

## 4. La receta de promoción, paso a paso (probada en la r30)

1. `python scripts/trypromo.py <unidad>` — **el juez es el DOL**, no `promote.py`.
2. Si sale `DOL ROTO`, mira **qué sección y cuánto** (tabla de secciones de la
   cabecera del DOL: 18 offsets, 18 direcciones, 18 tamaños).
3. Localiza el agujero por **rachas de bytes distintos** dentro de esa sección.
   **Las rachas de UN byte que aparecen antes son punteros corridos, no la
   causa.**
4. Carva el rango en `splits.txt`, parte el símbolo en `symbols.txt` si hace
   falta, y **añade la entrada de `keep.lst`**.
5. `python scripts/keepchk.py` — **obligatorio tras tocar una frontera**.

**Las dos trampas que costaron cuatro enlaces entre la r30 y la ventana:**

- **`dtk` renombra el relleno según dónde caiga**: `gap_` entre símbolos, `pad_`
  al principio o al final del rango de la unidad. Mover una frontera **renombra**
  el relleno que cambia de sitio, la entrada de `keep.lst` deja de casar y
  `-strip-unused-data` se lleva sus bytes: **DOL corto, enlace sin error**.
- **Si tu cambio «no surte efecto», mira la fecha de `build/GOWE69/main.elf`.**
  `keep.lst` y `splits.txt` ya son dependencias del grafo (arreglado), pero el
  reflejo vale para cualquier fichero de `config/`.

## 5. Herramientas: lo que falla

- **`lreg.py` roto por tres sitios → `scripts/alloc.py`**, y comprueba en el
  `.greg` que el pseudo llega a `global_alloc`.
- **`audit.py` y `frozen.py` quieren la RUTA COMPLETA SIN EXTENSIÓN**
  (`libc/itoa`, no `libc/itoa.c`); `triage.py` y `measure.py` aceptan el corto.
- **`audit.py` da FALLA fantasma** con el `build/` compartido: **confírmalos con
  una segunda pasada**.
- **La caché de `dwbody.py` se queda rancia tras CADA `build_direct.py`.**
- **`-fsched-verbose-5` CON GUION**; con `-dR` la salida de sched2 va dentro del
  `.sched2`.
- **Comparar cuerpos de función en crudo MIENTE**: los campos de `bl` y de
  reubicación los parchea el enlazador.
- **Finales de línea MEZCLADOS** dentro del mismo fichero; `UTLVector.h` es CRLF.
  Ancla línea a línea con `splitlines(True)`.
- **El volcado DWARF tiene 59 funciones SIN CUERPO**: «no aparece» ≠ «no existía».

## 6. Codegen: las reglas vivas (por si tu encargo lleva una función)

1. **La barrera tiene tres formas** y **cierra 2 de cada 8**: `__asm__("")` es
   total; `__asm__("" : : "r"(x))` **también** (sin salida ⇒ volátil);
   `__asm__("" : "=f"(x) : "0"(x), …)` es selectiva y **sube `n_refs` sin emitir
   un byte**. **Va DETRÁS de la instrucción que llega tarde.** Es deuda
   declarada: **dilo**.
2. **Un `lis sym@ha` izado sobre un `bl` = FALTA UNA CONVERSIÓN.**
3. **El ORDEN de los operandos de un `|`** decide el árbol de `fold`; lo que
   suele faltar es sacar `(C|r)` a un **temporal que no sea el destino**.
4. **El ORDEN de declaración de los miembros DENTRO de la clase** decide si GCC
   hace inline.
5. **`expand_inline_function` sólo materializa el argumento en un pseudo si el
   parámetro se MODIFICA dentro del inline** (los *setters* con clamp).
6. **Mover sentencias independientes dentro del MISMO bloque no cambia ningún
   `live_length`** (se recalcula tras `sched1`).
7. **Mientras quede UNA diferencia de estructura, quitar las otras EMPEORA** (7
   contraejemplos). Y **las vedas CADUCAN al mejorar el fuente debajo**.

## 7. Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
2. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano.**
3. **Si tu palanca es un constructo que el original no tenía, DILO.**
4. **El juez de una promoción es `trypromo.py`, no el porcentaje.**

## Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra. Si no cierra, **revierte** y anota la veda
  **diciendo qué sentencia barriste**.
- Detalle en `docs/analisis/r31-<grupo>.md`. **No commits.**
- **Si dejas un fichero modificado que rompe el enlace, DILO EN EL INFORME.**

## Prohibido

- **Escribir ensamblador de instrucciones.** Van diez falsificaciones retiradas.
  Antes de llamar falsificación a un `asm`, **cuenta las líneas de DWARF del
  original en su rango** (con `asm` salen 2-4 en cientos de bytes). Emitir
  **datos** con `.long` sí es legítimo.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`: PROPONER con verificación**
  (`trypromo.py` + DOL), no commitear. Puedes editarlos para medir **si los dejas
  como estaban**.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).

## Convivencia

Scratchpad con tu prefijo `c31<grupo>_`. **Vigila el disco** (15 GB libres). Hay
otros tres agentes en el mismo `build/`: mide con `build_direct.py`/`mn_repro.py`
(objeto suelto), nunca con un `ninja` completo.
