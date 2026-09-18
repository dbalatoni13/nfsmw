# Iteración 6 — triage reproducible de unidades linked

## Alcance y resultado

Esta revisión fue sólo de lectura. No compiló, extrajo ni enlazó objetos y no
modificó configuración ni fuentes de producción.

El resultado bajo el filtro deliberadamente estricto de esta revisión es
**cero candidatos simples**. Esto no demuestra que esas unidades sean
imposibles de promocionar: únicamente dice que ninguna de las unidades que el
reporte presenta como código mapeado al 100 % conserva hoy, por sí sola, la
misma estructura ELF allocable en el objeto fuente y el objeto extraído.

Artefactos reproducibles:

- `scratchpad/codex_20260908_iter6_linked_triage/audit.py`
- `scratchpad/codex_20260908_iter6_linked_triage/result.json`

Ejecución:

```text
python scratchpad/codex_20260908_iter6_linked_triage/audit.py \
  --output scratchpad/codex_20260908_iter6_linked_triage/triage_next.json
```

El result.json citado es una captura histórica anterior a integrar eathread
y SetupNextLoad. Una ejecución sobre el estado nuevo debe usar una ruta de
salida nueva; no sobrescribir esa evidencia ni esperar sus mismos conteos.

## Criterio literal

El script selecciona unidades no automáticas con
`metadata.complete == false` y `matched_code_percent == 100`. Después:

1. reconstruye directamente de `build.ninja` la lista `$in` del edge de
   `main.elf`;
2. exige que existan los objetos `build/GOWE69/obj/...` y
   `build/GOWE69/src/...`;
3. lee ambos ELF32 sin normalizar nombres ni destinos;
4. conserva, en orden de sección, toda sección no vacía con `SHF_ALLOC`;
5. compara literalmente cada tupla `(name, type, size, flags, align)`;
6. para considerar elegible una unidad, además exige que el grafo enlace en
   ese instante el objeto extraído y aplica las exclusiones del encargo.

No se equiparan pools, alias, rangos ni secciones sólo por tener el mismo
tamaño. Tampoco se interpreta un 100 % mapeado de objdiff como identidad de la
unidad completa.

Entradas fijadas en `result.json`:

- `report.json`: SHA-256
  `dc81b74666af8afff7e3a2aa4cf3c79dc4800c838617e3d078f5593b323323cf`;
- `build.ninja`: SHA-256
  `a2874dd4c62c8772f88167252f3eb7940067d4b6a3ff95c03077a4ed538d0125`;
- grafo: 619 entradas y 619 rutas únicas.

Conteo observado:

| Gate | Unidades |
|---|---:|
| NonMatching no automáticas con código mapeado al 100 % | 28 |
| pares de objetos existentes y comparados | 28 |
| pares con layout allocable literal idéntico | 0 |
| elegibles tras grafo y exclusiones | 0 |

## Rechazos cercanos, no candidatos

Estos ejemplos explican por qué no se propone una nueva prueba privada. Todos
tienen el mismo tamaño de `.text`, pero fallan antes de enlazar:

| Fuente | `.text` target/source | Diferencia allocable concreta | Historial |
|---|---:|---|---|
| `src/egami/rcmp/dev/source/decoder/cmn/rcmpbase.cpp` | 1.184/1.184 | `.rodata` 48/64 B; también difiere el orden de secciones | `r34-lib2.md`: reclamar el rango no evitó DOL ROTO |
| `src/egami/rcmp/dev/source/vd/gc/bigswizzler.cpp` | 1.208/1.208 | target sin `.rodata`, source con 24 B | `r34-lib2.md`: pool a 4 mod 8 y hueco previo ya diagnosticados |
| `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp` | 2.156/2.156 | `.rodata` 172/164 B | `r34-lib2.md`: GCC fusiona un literal que el original duplica |
| `src/Speed/Indep/Libs/snd/9/source/library/gc/sdspmix.c` | 2.120/2.120 | target `.data` 2.048 + `.bss` 5.656; source añade `.rodata` 4 y deja `.bss` 5.636 | `r45-cierre.md`: promoción aislada DOL ROTO |
| `src/Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp` | 472/472 | source añade `.rodata` 24 y cambia `.sdata` 8 por `.sdata` 4 + `.sbss` 4 | familia ya negativa en `r34-lib2.md` |
| `src/libc/ef_rem_pio2.c` | 848/848 | source añade `.sdata2` 920 B | familia de pools excluida |
| `src/libc/kf_rem_pio2.c` | 2.180/2.180 | source añade `.sdata2` 84 B | familia de pools excluida |
| `src/egami/rcmp/dev/source/av/cmn/avplayer.cpp` | 4.224/4.224 | `.rodata` 68/72 B | promoción aislada ya DOL ROTO en `r34-lib2.md` |
| `src/Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp` | 10.872/10.872 | `.rodata` 520/616 B y `.sdata` 8/4 B | alineación/layout ya bloqueados en el historial linked |

`zGameModes`, `input/cmn/interface` y `sdspmix` también constan como DOL ROTO
en `r45-cierre.md`. Las SourceLists que aparecen al 100 % mapeado tienen
excedentes grandes de texto o datos en el objeto fuente, de modo que tampoco
son unidades completas candidatas bajo este gate.

## Conclusión acotada

No se recomiendan candidatos ni barridos nuevos a partir de este reporte. Una
prueba futura necesitaría primero una evidencia nueva y específica de
propiedad/layout que explique la sección discrepante de una unidad concreta.
Resuelto ese bloqueo, habría que repetir una prueba de enlace privada y el gate
DOL; este triage no sustituye ninguna de las dos cosas.
