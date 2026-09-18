# Las 27 funciones que están FUERA de las SourceLists

`triaje` y los repartos de todas las rondas miran sólo las 33 SourceLists.
Cruzando `report.json` con el resto salen **27 funciones de ≥200 B y 12.444 B**
en unidades que **nadie ha trabajado nunca**:

    992 B  99,839 %  steering    Effect_Update          <- SEIS diferencias
    936 B  89,252 %  sfir        calcFIRCoeffs
    924 B  95,671 %  steering    SimThread_Step
    740 B  94,395 %  criticalpath VP6_PredictFilteredBlock
    656 B  98,780 %  avplayer    GetFirstFrame
    632 B  50,449 %  madidct     IdctColumn
    588 B  98,299 %  steering    HandleTriggers
    524 B  98,321 %  spchpick    iSPCH_MakeSampleRequests
    516 B  46,372 %  madidct     IdctRow
    440 B  93,591 %  dvd_device  StartNonAlignedAyncRead
    432 B  94,444 %  DebuggerDriver EXI2_WriteN
    424 B  99,925 %  pathtrack   PATH_createstreamimp    <- SOLO el marco
    412 B  95,534 %  spchpick    iSPCH_ChooseSamples
    396 B  87,778 %  pathbank    PATHI_loadbankdata
    388 B  93,907 %  FSasync     CompletePCreadAsync
    380 B  99,158 %  vmbase      __VMBASESetupExceptionHandlers
    336 B  98,869 %  pathnode    PATHI_calcwaitbeat
    324 B  94,691 %  steering    SimThread_Init

De paso, **`fndiff.py` no las podía diffear**: tenía cableado
`Speed/Indep/SourceLists/`. Ya acepta rutas (`LibSN/steering`, `libc/…`).

## `steering::Effect_Update` (992 B, 99,839 %) — VEDA, ocho formas

Sus **seis** diferencias son **un solo intercambio de r6/r7** en la rama del
ramp: el objetivo tiene `e->e.u.ramp.start` en r7 y `e->e.duration` en r6, y
nosotros al revés. Es reparto puro.

**El pin de registro no existe aquí**: `steering.c` lo compila `mwcceppc` y
`register int d asm("r6")` es error de sintaxis. **Las cuatro palancas nuevas de
la r36b son todas específicas de GCC**, así que todas las unidades MWCC
(`LibSN/*`) se quedan sin ellas.

Barridas ocho formas, todas al mismo porcentaje o peor:

| forma | % | difs |
|---|---|---|
| base | 99,839 | 6 |
| `d,n,m` / `d,m,n` / `n,d,m` / `d` con `h` pegado | 99,839 | 6 |
| `start` en local | 99,839 | 6 |
| `d` en línea | 98,972 | 8 |
| `start` en local + `d` en línea | 98,931 | 9 |
| `h` calculado al final | 97,492 | 14 |

MWCC canonicaliza el orden de declaración igual que GCC. **Frente cerrado hasta
que aparezca una palanca para MWCC.**

## `pathtrack::PATH_createstreamimp` (424 B, 99,925 %) — diagnosticado al byte

Las **ocho** diferencias son **sólo el tamaño del marco**: el objetivo usa `0x28`
y nosotros `0x20`. Las 95 instrucciones de en medio son idénticas.

`regmap`: **mismas locales, mismo árbol de bloques y mismo reparto** (7 de 7).
O sea que los 8 B son un **temporal de marco**, no una local — y no salen en el
DWARF.

**Medido qué tipo de temporal es**: metiendo un `double` en el cálculo del
volumen el marco sale **exacto (`0x28`)**, pero cuesta 25 diferencias porque
cambia la aritmética. Así que el hueco es de 8 B alineado a 8 —un `double` o un
`long long`— y está en un sitio que **no toca estas instrucciones**. Descartados:
`SetVolume` toma `int`, `volume`/`volscale` son bitfields `int:8`,
`PATHI_gettrackinfo` devuelve puntero y `PATHI_bytesperms` devuelve `int`.

Formas barridas: `trackinfo` en local (87,3 %), volumen en local (99,925 %, sin
cambio), volumen en `double` (84,5 % con el marco bueno).

### Aplicado el hallazgo de `c36c-wld` — y no cierra, pero acota

`c36c-wld` demostró que **el marco muerto y el reparto son el mismo problema**:
con un preservado de menos el asignador reserva 8 B que luego no usa. Este es
justo ese caso, así que barrí presión de registro:

| forma | % | difs | marco |
|---|---|---|---|
| base | 99,925 | 8 | 0x20 |
| barrera en `byterate` | 98,038 | 10 | 0x20 |
| barrera ×1 y ×2 en `buffertime` | 99,925 | 8 | 0x20 |
| `byterate` pin r6 | 99,642 | 13 | 0x20 |
| **`byterate` pin r29** | 98,896 | **4** | **0x28 (exacto)** |

El pin a r29 **acierta el marco y baja a cuatro diferencias**, pero las cuatro
son estructurales: fuerza a salvar r29 (`stmw r29` contra `stmw r30`) y el
tamaño sube a 428 B. El objetivo salva sólo r30/r31 **y aun así reserva los 8 B**
— o sea que es una `assign_stack_local` que `reload` pide y luego no referencia,
igual que la ranura huérfana que `c36c-ecs` midió en `epCalculate`.

**Frente acotado**: hacen falta los 8 B **sin** el tercer preservado. Barridas
siete formas (tres estructurales, dos barreras, dos pines). Lo que queda por
mirar es de dónde sale la presión en el original sin cambiar el censo de
salvados — con los volcados de RTL que `c36c-wld` acaba de habilitar
(`cc1plus.exe` a mano) se vería el grafo de interferencias directamente.
