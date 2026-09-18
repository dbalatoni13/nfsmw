# r67b — cierre

Las barreras con la palabra clave corta `asm("")` que el censo de las r65–r66 no veía.
Cuatro lotes con propiedad exclusiva de ficheros (32 ficheros, 67 barreras; la de
`metrotrk.c` se fue con el fichero). En la ventana, además, la propuesta de cabecera del
lote audio-plat y un barrido mecánico de las barreras cuyos operandos ya están pineados.

## Progreso oficial verificado

| medida | antes r67b (HEAD `ae29e9dc`) | después r67b + ventana |
|---|---:|---:|
| unidades completas | 525 / 619 | 525 / 619 |
| andamios de reparto (pin + `__asm__("")` + `asm("")`) | 196 | **175** |
| de ellos: pines / `__asm__("")` / `asm("")` | 86 / 43 / 67 | 86 / 42 / 47 |
| bytes de DOL distintos | — | 0 (ronda de fidelidad) |
| unidades rancias (`censorancios`) | — | 0 |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, relinkado desde el edge de
`main.elf` tras cada paso; el último, después del barrido y de `stBankSlot`.

Serie de la limpieza con el censo corregido (tres formas): **238 (HEAD r66) → 209 (r67) →
196 (ventana r67: `metrotrk.c` y EPlayRaceNIS) → 176 (r67b) → 175 (ventana r67b)**.

## Por lote (censo HEAD/árbol fichero a fichero)

| lote | retiradas | dónde y con qué |
|---|---:|---|
| libs | 7 de 22 | `memset.c` y `strstr.c` con la forma literal de newlib (lo muerto incluido); `syscalls.c`: el prototipo de `PCwrite` llevaba argumentos de menos; `pathserv.cpp` lecturas escalares `*(T *)&p->campo`; `pathsnd.cpp` vista agregada en TODOS los accesos; `FSasync.c` las dos con el almacén por lvalue agregada |
| fe-world | 7 de 16 | **5 muertas detrás de un pin** (TrackPath, TrackStreamer, FEngInterface ×2, WRoadNetwork InitAtSegment); FEPackage con el bucle natural del DWARF; QuickGame con una asignación pisada `camera_time = 0.0f;` en vez del cebador de pool por asm |
| audio-plat | 5 de 20 | `EAXAemsManager.cpp` RegisterSlots sin la local que el DWARF no tiene; `SFXCTL_3DObjPos.cpp` cuatro barreras que sobraban junto a clobbers que sí sostienen |
| ecs-cam-cabecera | 1 de 9 | ICEManager: `do { } while (0);` en lugar de la barrera |
| **total ronda** | **20** | pines y `__asm__("")` sin cambio: ninguna barrera convertida en otra forma |
| ventana: barrido tras pin | 1 de 29 | EmitterSystem: la definición `"=r"(e36guard)` del fantasma sobraba |

## Reglas nuevas medidas

1. **Un bucle vacío es la barrera de sched2 escrita en C**: `haifa-sched.c` trata
   `NOTE_INSN_LOOP_BEG/END` como `schedule_barrier_found`, igual que `ASM_INPUT`. Sustituye a
   una barrera cuyo único efecto es el ORDEN; no a una que esconde un valor a cse.
2. **Una barrera cuyo operando es una variable pineada puede estar muerta**: la variable ya
   es registro duro, no allocno. Hay que medirla SOLA: «sin el pin y la barrera» no dice nada
   de la barrera. 5 de 16 en fe-world; el barrido del resto del árbol, 1 de 29.
3. **La palanca de alias va en las dos direcciones y en TODOS los accesos**: vista agregada
   en el almacén de un global contra otro global; lecturas escalares `*(T *)&p->campo` cuando
   el almacén es el escalar fijo.
4. **Los prototipos extern de la casa pueden llevar argumentos de menos**: antes de una
   barrera `"+r"` sobre parámetros, comprobar la firma real (`PCwrite` de SN).
5. **La forma literal de newlib incluye lo muerto**: la rama de `LBLOCKSIZE` y las locales sin
   usar forman parte de la forma que casa.
6. **El cebador de pool flotante sí existe en C**: una asignación redundante pisada en seguida
   (`x = 0.0f; x = f();`) crea la entrada del pool en ese punto, en un bloque que no domine las
   cargas posteriores.
7. **Un puntero local que el DWARF no lista puede ser el `this` de un constructor inline que
   en el original hacía trabajo** (`pNewSlot` con `stBankSlot() { Clear(); }`).
8. **«Cuatro referencias de más» es el escalón de `floor_log2` de 12 a 16**: si el hueco sólo
   se cierra cruzándolo y no hay cuatro usos reales, no hay C.

## Propuestas: decisión de cada una

| propuesta | decisión |
|---|---|
| audio-plat: `SndDataParams.hpp` `stBankSlot() { Clear(); }` + fuera `pNewSlot` en RegisterSlots | **APLICADA** (`6e8407f2`). Remedida en copias estáticas de HEAD `e4c719f5`: las **14 unidades** que incluyen la cabecera IDÉNTICAS en ALLOC y symtab; control con sólo la cabecera (doble `Clear`): zEAXSound DISTINTO, `.text` +12 B. No es un andamio asm: quita una local que el DWARF no tiene y pone el trabajo donde lo tenía el original |
| fe-world: barrido de las barreras cuyos operandos ya están pineados, una a una | **HECHO** (`barrerapin.py` + `barrido_pin.py`, copias de HEAD, ALLOC+symtab por unidad, control de determinismo por unidad): **1 IDÉNTICA de 29** → aplicada `963f75aa`; 28 DISTINTAS, que sostienen el objeto |
| libs: `slinklist.h` para csis (de la r67) | aplazada: encargo propio con barrido de todo snd |
| ecs-cam-cabecera | ninguna |

## Verificación de la ventana

1. **Propiedad**: 33 ficheros modificados; 32 en su lote y **uno fuera de todos los lotes,
   `src/LibSN/steering.c`** (ver abajo).
2. **Censo HEAD/árbol** con las tres formas: `asm("")` 67 → 47, pines y `__asm__("")` sin
   cambio.
3. **`cmphead.py`** contra HEAD `ae29e9dc`: **37 de 37 unidades IDÉNTICAS**, incluidas las 24
   que incluyen `UTLVector.h`. `steering` DISTINTA (sólo por el cambio de fuera de lote).
4. Reconstrucción de los 37 objetos (0 fallidas), `censorancios.py`: `RANCIO 0`, `ok 523`,
   `ok_debug 2`; relink del DOL: **DOL OK**; `lcfix --check`: 0 CORRIGE, 53 FALLO inertes.
5. UTF-8 en los 32; tres ficheros con finales mixtos normalizados. Sello sha1 de las 32
   fuentes tomado tras la verificación y comprobado antes de commitear: 32 de 32 iguales.
6. Tras la propuesta de cabecera y el barrido: zEcstasy del árbol contra HEAD `e4c719f5`
   **IDÉNTICO**; reconstrucción de las 14 unidades de la cabecera, relink, `lcfix` y `censorancios`
   finales 15 ok / 0 fallidas, **DOL OK**, `lcfix` 0 CORRIGE / 53 FALLO inertes, `censorancios` **RANCIO 0** (`ok 523`, `ok_debug 2`).

Commits: `6ed7f908` libs, `e374f7ae` fe-world, `aea50ba3` ecs-cam-cabecera, `e4c719f5`
audio-plat, `6e8407f2` stBankSlot, `963f75aa` EmitterSystem.

### `steering.c`: un cambio que no era de la ronda

A las 13:37:57 `src/LibSN/steering.c` apareció modificado (en `Effect_Init`, `i = 0` y
`off = 0` reordenados), fuera de todos los lotes. Ningún agente de la r67b lo nombra en su
transcripción (ni el fichero, ni `Effect_Init`, ni `g_bWaveTablesInitialized`), y no había
otra sesión de Claude activa. A las 13:44:17 volvió solo al contenido de HEAD. No lo toqué,
no entró en ningún commit, y `steering` es NonMatching (el DOL enlaza el objeto extraído),
así que no afecta al DOL.

## Lo que queda

- **175 andamios de reparto**: 86 pines, 42 `__asm__("")`, 47 `asm("")`. Todos con su
  diagnóstico junto al andamio; las barreras tras pin ya están barridas.
- 502 parches de datos y 62 bloques de instrucciones: frente propio.
- `slinklist.h` (csis) y la compensación de `.rodata` de zWorld (CreateCarLightFlares):
  encargos propios.
