# r71: Auditoría de merge-readiness contra los 7 anti-patrones

Cifras medidas sobre el árbol el 2026-09-14, con el método de cada comprobación.
Veredicto por clase, honesto: qué tenemos, qué cubre la herramienta, y el plan.

## Resumen ejecutivo

| # | Anti-patrón | Estado | Cifra |
|---|---|---|---|
| 1 | Registro asignado a mano para forzar match | **VIOLACIÓN PRESENTE** | 124 pins en 45 ficheros |
| 2 | Acceso por offsets de puntero | **PARCIAL (matiz)** | 469 usos en 38 ficheros (324 alias asm + aritmética de pool) |
| 3 | Valores aleatorios | **CUBIERTO POR HERRAMIENTA** | audit.py valida cada literal y reloc contra el ELF |
| 4 | Renombres aleatorios | **PARCIALMENTE CUBIERTO** | regmap/dwbody comparan nombres; verificación total pendiente del parser DWARF-1 |
| 5 | Código reimplementado con guardas | **LIMPIO** | 20 cuerpos duplicados, todos paralelismo del ORIGINAL |
| 6 | Redeclaraciones de funciones | **BAJO RIESGO** | 452 prototipos, concentrados en libc/fdlibm/vp6 (estilo original) |
| 7 | Casts al propio tipo | **SIN CONFIRMAR** | 214 sospechosos heurísticos, 0 confirmados |

## 1. Pins de registro — 124 en 45 ficheros

`register X asm("rN")` / variables locales atadas a registro físico. Es
exactamente el anti-patrón 1. Los mayores: `libc/vfprintf.c` (15),
`WRoadNetwork.cpp` (9), `FEngInterfaceFEObjects.cpp` (7), `GManager.cpp` (7),
`criticalpath.c` (6), `CarRender.cpp` (6), `eLightE.cpp` (5), `pathnode.cpp` (5).

De los 64 andamios del inventario (INVENTARIO-ANDAMIOS.md), 52 eran pins; los
tres agentes de la ronda r69-r70 ya retiraron/vedaron su lote. **Quedan los de
funciones no cerradas** (los pins "diagnósticos" de near-miss) y los de libc.

**Plan**: continuar el protocolo del inventario (quitar → forma de fuente →
veda documentada con cifra). Para merge: ninguno puede quedar.

## 2. Aritmética de direcciones — 469 usos en 38 ficheros

Dos familias:
- **324 `extern` con `asm()` ligando datos a su dirección real** (`lbl_803F...`,
  `$LC...`, `_zms...`): es direccionamiento del DATO ORIGINAL en su dirección
  ORIGINAL — no structs inventados — pero un revisor upstream puede leerlo
  como anti-patrón 2. zMisc.cpp concentra 276.
- **Aritmética de pool** (`_bwarePrefix + off`, `ZMISC_POOL`): apunta usos de
  fuente a las copias del pool del objetivo en vez de literales orgánicos.

**El matiz honesto**: el bloque manual existe porque el pool del objetivo no
se reproduce orgánicamente todavía (nuestro cc1plus reinterna copias propias;
ver el +496 B de zMisc en r70). La solución LEGÍTIMA completa es que el pool
salga solo — eso exige que la deduplicación de literales del compilador se
comporte como la del original en TODA la unidad. La ronda de datos en vuelo
(misc/zAI) trabaja en esa dirección pero con la palanca de punteros: para
merge, o se cierra la vía orgánica o se negocia una exención documentada.
**Aviso**: la tanda r70 de zMisc (10 cadenas) EXTENDIÓ esta familia — es
deuda técnica consciente, anotada aquí.

## 3. Valores aleatorios — cubierto

`audit.py` resuelve cada reubicación contra el ELF original y compara el
VALOR de cada literal (float 4 B, cadena hasta NUL). Cualquier valor inventado
salta. Además `frozen.py` congela diffs completos. Estado: aplicado en cada
cierre de la ronda; cero casos conocidos vivos.

## 4. Renombres — parcialmente cubierto

`regmap.py`/`dwbody.py` comparan los nombres de locales del DWARF contra los
nuestros (así se cazarón los bTan/GetVelocity ausentes de TrackCar). Filtro
negativo verificado hoy: `interpolationTime` (nombre que INVENTAMOS en r65)
NO está en el .debug del GC; `halfVP2` (original) sí.
**Plan**: cuando `dwarf1.py` (agente en vuelo) esté, pasar el diff de nombres
a TODAS las unidades — es la verificación total de esta clase y de la 7.

## 5. Duplicación — limpio

Detector de cuerpos idénticos ≥6 líneas (hash normalizado): 20 pares, todos
paralelismo del código original o de terceros: maddec/maddeca (hermanos del
codec, como IdctColumn/IdctRow), rcmp_mad/rcmp_vp6, libgcc2_3/2_6 (variantes
del runtime), dolphinsdk include↔src (inlines del SDK), STLport, UVectorMath
(sobrecargas paralelas). **Ninguno nuestro.**

## 6. Redeclaraciones — 452, casi todas estilo original

Concentradas donde el original era C con declaraciones locales: libc/fdlibm
(`scalbnf`, `__kernel_rem_pio2`...), vp6 (doptsystemdependant 27,
vfwpbdll_if 25...), realcore, EAXSound/stream. Ese estilo ES el del código
fuente original de esas librerías. Muestra revisada: ninguna firma incorrecta
(compilan contra cabeceras reales donde existen y el código casa).
Riesgo residual: los prototipos que hayamos escrito NOSOTROS en código de
juego — revisión por caso si upstream lo pide.

## 7. Casts redundantes — sin confirmar

214 sospechosos por heurística (`static_cast<float>(x)` trivial). Sin
información de tipos no se confirma ninguno; con `dwarf1.py` (tipos originales
del .debug GC) se puede auditar con precisión. Pendiente.

## Lo que ya cumple el repo

- `audit.py`: verdad de literales y reubicaciones (clase 3).
- `frozen.py` + `measure --cmp`: congelación y detección de regresiones.
- `regmap.py`/`dwbody.py`: comparación de nombres/estructura (clase 4 parcial).
- La directiva del proyecto: CERO asm en funciones al 100% (clase 1, en curso).
- Disciplina de vedas: todo intento fallido queda documentado con su cifra y
  su mecanismo — nada se maquilla.

## Hoja de ruta para merge

1. Terminar la retirada de pins (inventario + libc) — clase 1 a cero.
2. Verificación total de nombres/tipos con dwarf1.py — clases 4 y 7.
3. La conversación difícil: la familia de bloques-pool. O se reproduce el pool
   orgánicamente (línea de trabajo abierta con el delta +496 B de zMisc como
   caso piloto), o se documenta como técnica puente con su porqué medido.
