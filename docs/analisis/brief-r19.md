# Ronda 19 — instrucciones comunes

Estado: **96,52% matched**, 18.261 funciones, `linked` 7,89% (340/589),
`main.dol: OK`. En las SourceLists quedan **44.232 B en 32 funciones**
accionables y 57.712 B de muro.

## Lo que la ronda 18 enseñó, y cambia por dónde empezar

**El valor no estuvo en los bytes sino en lo que se corrigió.** `audit.py`
encontró **más de treinta funciones que objdiff daba al 100% y estaban mal**, y
varias eran fallos de comportamiento:

- `RightToLeftMatrix4` **cruzada** entre dos funciones de cámara — cada una
  llamaba a la instanciación de la otra, **con 100% en ambas**.
- `EventsQueued()` devolvía **el predicado invertido**.
- `AIVehicleHuman::Update` llamaba **dos veces a la misma función** donde el
  original llama a dos distintas.
- Los dos cocientes de PAL **intercambiados**: FOV y aspecto al revés en 50 Hz.
- `UIProfileManager` construía **`PMPopDelete`, una clase que no existe**.
- El título del juego en `MemoryCard` **en UTF-8 en vez de Latin-1**.
- Dos símbolos **indefinidos** en `zEAXSound.o` que no existen ni en el ELF.

**Empieza pasando `python scripts/audit.py <unidad>` por las tuyas** y tráeme la
lista. Es trabajo que ninguna herramienta anterior veía.

## Las tres herramientas, y cómo NO usarlas mal

| | |
|---|---|
| `triage.py <unidades>` | tu lista: `faltan N, sobran M, de ellas K SUST`. **`SUST` son sustituciones**: `4/4 de ellas 4 SUST` es **una** diferencia repetida, no ocho |
| `audit.py <ruta completa>` | **prueba** que casa: resuelve reubicaciones contra el ELF, compara destino relativo y condición de cada rama, y el **valor** de cada literal |
| `frozen.py cong/chk` | congela el diff entero (guarda su huella) y avisa si cambia |

**Las dos últimas quieren la RUTA COMPLETA** (`Speed/Indep/SourceLists/zWorld`),
no el nombre corto que aceptan `triage.py` y `measure.py`.

**Y leen el `build/` compartido, así que con varios agentes dan fantasmas.**
Medido: una pasada dio **8 FALLA que la segunda no reproduce** porque otro agente
recompiló en medio; y `frozen.py chk` gritó en dos unidades limpias porque otro
movió una cabecera y con ella los sufijos de los estáticos. **Confirma todo fallo
con una segunda pasada.**

## Método: preparación, ensayos numerados, prueba

1. **Antes de editar**, escribe en `docs/analisis/r19-<grupo>.md`: tamaño e
   instrucciones de la función, su dirección, el censo de llamantes (`bl`
   directas — dilo así, no es prueba de que no haya punteros a función), los
   llamados y su contrato, **y qué NO has probado**.
2. **Ensayos numerados** `c1`, `c2`… **cada uno con su cifra medida** y una línea
   de por qué se descarta. Los descartados no quedan en el árbol.
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Y congélalo después.

## Trampas medidas — todas han costado dinero

- **`measure.py` NO COMPILA y `ninja` NO CONSTRUYE EL MIDDLEWARE** (cero reglas
  para `libc/`, `LibSN/`, `Libs/path/`). Medir sin `build_direct.py` antes lee
  objetos rancios. **Me ha mordido dos veces esta sesión**, la segunda casi
  reportando una regresión de 632 B que no existía.
- **El tamaño exacto miente**: siete casos medidos donde la variante da el tamaño
  del objetivo y **empeora** `faltan/sobran`.
- **El porcentaje tampoco mide avance**: dos correcciones reales hicieron
  coincidir *todas* las ranuras de pila y objdiff **bajó** 2,2 pp.
- **Disco lleno**: `No space left on device` se lee como «no cambia nada».
- **Finales de línea mixtos POR LÍNEA**. Usa `\r?\n`.
- `bench.py` puede dar un 100% falso · `regmap` empareja mal con sobrecargas ·
  `litcheck` da falsos positivos en el lado del objetivo · `zcmp2` miente con
  permutación global de registros · el permutador lleva seis rondas sin cerrar.

## Patrones nuevos de la 18

- **Un sondeo `while (cond) ;` LO ROTA GCC** y emite `lwz`+`andi.`+`beq` de prueba
  antes de entrar; **`do { } while (cond);` no**. 88,34 → 93,70% en una función.
- **Con N asignaciones seguidas a globales, el orden de los `lis` del objetivo ES
  el orden de las sentencias en la fuente** (los `stw` salen permutados). Cerró 5
  funciones de golpe.
- **`case 0:` sin `default:`**: el árbol `cmplwi 1; bgt` sólo sale si el nodo 1
  tiene dos hijos y el `b` de «ningún caso» va al final del switch.
- **El mismo inline se expande de dos formas en la misma unidad**, y lo resuelve
  el mapa de líneas: si el `blt` está atribuido al **llamante**, la guarda está en
  el llamante.

## Restricciones de registro

Se permiten **como último recurso**: sólo tras agotar y **documentar** la búsqueda
de la forma de fuente, sólo restricciones vacías de entrada, y **tienen que pasar
`audit.py`**. En la 18 un agente las probó en `CookieTrailCurvature`, **empeoran**,
y cerró como muro medido en vez de forzarlo — eso es lo correcto.

**Escribir ensamblador sigue prohibido.** En la 18 se retiró un `asm("fmr %0,%1")`
que estaba commiteado y **ni siquiera cerraba** su función.

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada.** Una función al 99,99% aporta cero.
- **Si no cierra, revierte y anota la veda diciendo QUÉ SENTENCIA barriste.**
- **No toques** `config/GOWE69/*` ni `splits.txt`. `configure.py` **sólo** si te lo
  digo en tu encargo.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Cabecera compartida = A/B por objetos sobre TODAS las unidades que la
  incluyen**, y luego la construcción completa.
- **El scratchpad es compartido**: nombre único a tus helpers y a tus `.json`.
