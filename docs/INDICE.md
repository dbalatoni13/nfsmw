# ÍNDICE — la puerta de entrada

Decompilación de **NFS Most Wanted**, en **cinco versiones**: GameCube
(`GOWE69`, SN ProDG / GCC 2.95.3), PlayStation 2 (`SLES-53558-A124` prototipo y
`SLUS-21351` de venta, EE-GCC 2.9), Xbox 360 (`EUROPEGERMILESTONE`, MSVC PPC) y
PC (`SPEED_EXE_1_3`, MSVC 7.1).

GameCube es la más avanzada con diferencia, pero **ya no es la única que se
mide**, y esa es la diferencia con todo lo escrito antes del 2026-09-17.

---

## 1. Antes de tocar nada: cinco documentos

| # | documento | por qué |
|---|---|---|
| 1 | **`docs/METODO.md`** | **Cómo se mide y se decide hoy.** Cero `asm`, el flag `Matching` se ignora, un informe por versión, y la regla: un 80 % real y portable vale más que un 100 % que solo compila en GameCube. Si un informe viejo dice otra cosa, manda éste. |
| 1b | **`docs/PROCEDIMIENTO.md`** | **Cómo se trabaja para subir la cifra.** Buscar la causa común, no la función; qué patrones del diff valen y cuáles son ruido; confirmar con un oráculo (la sonda de layout, el volcado del original) antes de tocar nada. |
| 2 | **`docs/TRAMPAS.md`** | Dónde **miente** cada herramienta. 31 entradas, todas medidas. Su regla madre: *exígele a tu medida un control que TENGA que cambiar*. |
| 3 | **`docs/DATOS-DEBUG.md`** | Qué sabemos del fuente original, dónde vive y **qué herramienta lo lee**. |
| 4 | **`docs/PLAYBOOK.md`** | El manual (432 kB, 6.242 líneas). **No lo abras entero**: busca la sección en `docs/PLAYBOOK-MAPA.md` y lee sólo su rango con `sed -n '<a>,<b>p'`. |
| 5 | **`docs/HERRAMIENTAS.md`** | Qué hace cada una de las 128. Generado por `scripts/indice.py`; regenéralo si dudas. |

Y una orden que cuesta un segundo y ahorra rondas enteras:

```bash
python scripts/previo.py <funcion>     # QUÉ SE HA PROBADO YA en esa función
```

El trabajo previo no vive en `docs/`: vive en los **comentarios del fuente**,
junto a la función. Son 226 bloques.

---

## 2. Dónde está cada dato del original

| dato | fichero | lo lee |
|---|---|---|
| **El objetivo** (ELF final, 105 MB) | `orig/GOWE69/NFSMWRELEASE.ELF` | todo |
| **DWARF-1**: locales con nombre+tipo+**registro**, bloques léxicos, árbol de inlines | `.debug` (92 MB) del ELF | **`scripts/dwarf1.py`**, y `scripts/orac.py` para el mismo dato en un grep |
| **Fronteras de unidad** autoritativas | `.debug_aranges` (18 kB) | `scripts/aranges.py` |
| **Símbolo → unidad de compilación** | `.debug_pubnames` + `.debug` | `scripts/pubnames.py` |
| **Dueño de cada dato** | DWARF | `scripts/dwarfmap.py` |
| **Línea de fuente de cada instrucción** | `symbols/debug_lines.txt` (40 MB) | `scripts/lmap.py`, `scripts/fuse.py`, `scripts/plan.py` |
| **Locales con registro, inlines y orden — en PS2** | `orig/SLES-53558-A124/NFS.ELF` (`.mdebug`, tabla SYMR) | `scripts/mdebug.py` |
| **Orden de emisión** de referencia | `orig/SLES-53558-A124/NFS.MAP` | `scripts/ps2map.py`, `scripts/ps2fn.py` |
| **Nombres** del prototipo de Xbox | Xbox A138 · `orig/EUROPEGERMILESTONE/` | `scripts/x360ref.py` |
| **Nombres** sacados de nuestro propio objeto (X360 y PC) | — | `tools/nombra_por_codigo.py` |
| **Troceado** en unidades | `config/<V>/splits.txt` | `checksplits.py`, `rangechk.py`, `claimrange.py` |
| **Símbolos** | `config/<V>/symbols.txt` | — |
| **Qué salva el enlazador** (solo GameCube) | `config/GOWE69/keep.lst` | **`lcfix.py`**, `keepchk.py`, `gapchk.py` |
| **Mapa del enlace** (solo GameCube) | `config/GOWE69/ldscript.ld` | `dolwhere.py`, `stripmap.py` |
| **Candados anti-regresión** (95 unidades) | `docs/congelado/*.json` | `scripts/frozen.py` |
| El fuente de **GCC 2.95.3 SN** | `orig/prodg/NGC_GNU_SRC/` | lectura directa (verifica los formatos) |

---

## 3. El ciclo de trabajo

```bash
# 0. DÓNDE ESTAMOS -- las cinco versiones, con "0 objetos no compilan" como control
python scripts/pctall.py                   # la medida que manda
python scripts/pctall.py --solo-informe    # sin recompilar

# 1. ELEGIR -- qué falta por escribir, por bytes
python scripts/censo.py
python scripts/unitscore.py

# 2. DIAGNOSTICAR una función -- por TIPO, nunca por porcentaje
python scripts/previo.py   <fn>            # ¿ya está probado?  <-- SIEMPRE PRIMERO
python scripts/triaje.py   <unidad>        # permutador / falta una local / estructura
python scripts/fncmp.py    <unidad>        # qué funciones NO son idénticas
python scripts/dwarf1.py fn <fn>           # el ORÁCULO: locales, registros, inlines
python scripts/mdebug.py  fn <fn>          # el mismo oráculo, desde el lado de PS2
python scripts/plan.py     <unidad> <fn>   # asm + línea de fuente + árbol de inlines

# 3. ESCRIBIR y medir  (~1,4 s por variante)
python scripts/bench.py  <unidad>
python scripts/fndiff.py <unidad> <fn>     # la fila a fila; el fuzzy manda al sitio malo

# 4. CERRAR
python scripts/audit.py  <unidad> <fn>     # PRUEBA que casa, no te creas el 100 %
python scripts/auditaportable.py           # que no quede nada que solo compile en GC
python scripts/pctall.py                   # y que las CINCO sigan compilando
python scripts/frozen.py cong <unidad>     # congela el resultado
```

**El control de GameCube:** el DOL de referencia es
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`. Si se mueve sin que lo hayas
querido, `scripts/lcfix.py` primero (y lee sus líneas `FALLO`, no solo las
`CORRIGE`) y `scripts/dolwhere.py` después.

---

## 4. Las cinco trampas que más cuestan

1. **`matched_code` es todo-o-nada.** Una función al 99,99 % aporta **cero**
   bytes. Los near-misses valen su tamaño íntegro.
2. **Una cifra sin «0 objetos no compilan» no vale.** objdiff cuenta al 100 %
   una unidad marcada `complete` que no tiene objeto base, y un troceado que
   falla deja el `objdiff.json` de la versión anterior: se acaba midiendo una
   versión como si fuera otra. Ver `docs/METODO.md`.
3. **Un `.o` rancio da `DOL OK` durante rondas.** `build_direct.py` puede decir
   «1 ok» y dejar el objeto viejo. Sella con `sha1` y corre `censorancios.py`.
   Y **`build_direct.py --help` no imprime ayuda: compila las 33 unidades.**
4. **objdiff empareja por NOMBRE y por SECCIÓN.** Mismo tamaño y 0 % casi
   siempre es el nombre, no el código; y una unidad cuyo objeto lleva otra
   sección que la del objetivo se lee entera como no casada.
5. **Las vedas caducan.** 12 rotas de 55 barridas. Una veda vieja es una
   oportunidad, no un muro.

---

## 5. Cómo está organizada la documentación

| dónde | qué | vigencia |
|---|---|---|
| `docs/METODO.md` | cómo se mide y se decide | **PERMANENTE** |
| `docs/PLAYBOOK.md`, `TRAMPAS.md`, `DATOS-DEBUG.md`, `GHIDRA_NFSMW.md` | reglas del compilador, trampas, oráculos | **PERMANENTE** |
| `docs/HERRAMIENTAS.md`, `PLAYBOOK-MAPA.md` | catálogos | **GENERADOS** — no editar a mano |
| `docs/X360_EXTRACTION.md`, `github_actions.md`, `splits.md`, `symbols.md` | procedimientos y formatos | PERMANENTE |
| `docs/ESTRATEGIA.md` | el plan de la era `linked` | **HISTÓRICO** — el método sirve, los números no |
| `docs/analisis/` | 656 informes de ronda y volcados | diario; se consulta, **no se cree** |
| `docs/congelado/` | 95 huellas anti-regresión | dato, lo escribe `frozen.py` |
| `docs/comment_section.md`, `common_bss.md`, `getting_started.md`, `dependencies.md` | plantilla dtk (hablan de **mwcc**, no de SN) | ajeno; `dependencies.md` además **contradice al README** |

Dos avisos de lectura sobre `docs/analisis/`, medidos:

- **`r48-jf-censo-linked.md`**: sus primeras ~400 líneas están refutadas al final
  del propio fichero.
- **`r32-upstream.md`**: refutado entero; es peligroso leerlo hoy.

Para cualquier número: `scripts/pctall.py`, nunca un documento.

---

## 6. Estado

La cifra de hoy sale de `python scripts/pctall.py`, que deja el detalle en
`build/versiones/<V>.json`. Este documento **no lleva cifras**: han caducado
cuatro veces y han mandado trabajo al sitio equivocado. Corre la orden.

Lo que sí es estable y conviene saber de entrada:

- Las **cinco** versiones compilan con **0 objetos fallidos**.
- GameCube está por encima del 97 % de código casado; el resto está entre el 1 %
  y el 27 %, así que **el trabajo con más recorrido hoy no está en GameCube**.
- Xbox 360 y PC tenían techo de **nombres**, no de código:
  `tools/nombra_por_codigo.py` los saca de nuestro propio objeto compilado.
