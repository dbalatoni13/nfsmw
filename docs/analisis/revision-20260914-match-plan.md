# Revisión de match y linked — 2026-09-14

## Estado fiable

Referencia: `build/GOWE69/report.json` regenerado después de corregir la asociación local de `Compare`.

- Código exacto: **3.915.284 / 3.946.048 B = 99,22038 %**.
- Funciones exactas: **18.406 / 18.432 = 99,85894 %**.
- Código `linked`: **1.264.380 / 3.946.048 B = 32,04168 %**.
- Unidades completas: **525 / 619**.
- El informe enumera 26 entradas y 30.764 B: **25 cuerpos reales / 30.744 B**, más `pad_00_8000348C_init` / 20 B, que es alineación del contenedor DOL y no una función a decompilar.

La cifra oficial manda sólo cuando el informe se ha regenerado con objetos actuales. Entre r55 y r59 los propios cierres marcaban el informe como obsoleto; no se deben sumar sus cifras intermedias. Tampoco se deben acumular “bytes distintos del DOL eliminados” entre rondas: los rangos se solapan.

## Trayectoria comprobable

| Hito | Match / exactas | Linked / unidades | Lectura correcta |
|---|---:|---:|---|
| r48 | 3.908.380 B / 18.399 | 494 / 618 | Punto de partida comparable de la cola resistente. |
| r49 | sin cambio de match | 777.568 B / 497 | Demostró que objeto exacto no implica DOL exacto; hay que enlazar. |
| r54 | 18.403 exactas | 518 / 616 | Último match fresco antes del tramo con report obsoleto. |
| r55–r60 | report no comparable en parte del tramo | 522 / 616 al final | Promociones reales de zGameModes, zMain, zAnim y zFeOverlay. |
| r61–r63 | — | 525 / 619 | inittmr, criticalpath y spchdata; cambió también el denominador. |
| r64–r67b | estable | 525 / 619 | Trabajo de orden, enlace, fidelidad y limpieza de asm, no cierres ficticios. |
| estado actual | 3.915.284 B / 18.406 | 1.264.380 B / 525 | +6.904 B y +7 exactas frente a r48; quedan dos problemas distintos: match y promoción. |

El censo histórico r67b dejó 175 andamios de distribución de registros (86 pines, 42 barreras vacías largas y 47 cortas), además de 502 parches de datos y 62 bloques con instrucciones. No se compara directamente con el parser nuevo. El censo léxico final de esta revisión, después de retirar el fantasma de IdctRow, da 124 barreras y 86 pines en proyecto/terceros; incluye 35 de vfprintf y no demuestra alcanzabilidad GC. Alcance, categorías y delta homogéneo están en `revision-20260914-plan.md` y sus artefactos.

## Corrección de medición `Compare`

El cuerpo de 16 B ya era exacto. El sufijo local de fuente había cambiado y el mapping apuntaba a `.25190`; el símbolo vigente es `.25167`.

- Target: `Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.35326`.
- Fuente: `Compare__Q38GManager48GatherVisibleIcons__8GManagerPP5GIconP7IPlayer.0_8IconSortPCvT1.25167`.
- Ambos: `.text+93036`, 16 B, `STB_LOCAL`, `STT_FUNC`.
- Bytes: `80030004806400047c6300504e800020`; cero relocaciones en el intervalo.
- Objetos SHA-256: target `084e269c...968957`; fuente `bfc68095...cae2eb`.

El mapping ya fue corregido por coordinación y elevó el informe en **+16 B/+1 función** sin cambiar código ni `linked`. El auditor reproducible es `scratchpad/codex_20260914_match_plan/audit_compare.py` y pasa contra el estado actual.

## Las tres prioridades concretas

### 1. Cerrar la inicialización estática de zCamera con una causa de C real

`__static_initialization_and_destruction_0`, 3.604 B, 99,839066 %, sólo seis filas distintas. La investigación ya identificó los tres valores reales cuya asignación rota entre `r9/r10/r11`: una constante, `HydraulicsLookAngle` y el literal `12000.0`. Aumentar referencias, fijar registros o añadir barreras ya dio resultados negativos.

La única vía nueva defendible es expresar en C un ámbito/tiempo de vida real distinto para los pseudos de `HydraulicsLookAngle` y `12000.0`, respaldado por fuente o DWARF. Debe predecir antes del ensayo la rotación completa y cerrar las seis filas. Si requiere un valor fantasma, un pin o una barrera sin semántica, se descarta y se conserva el baseline.

### 2. Fantasma de `IdctRow` retirado; buscar procedencia de madidct

El local r68 de `IdctRow` introducía `register int g asm("r5")` y dos asm vacíos. Sólo igualaba el tamaño 516 B, pero bajaba a 44,36434 % sin ganar ningún byte exacto. Se retiraron en esta revisión: fuente C pura, 496 B / 46,372093 %, sin cambiar matemática ni datos y sin perder funciones exactas. El diagnóstico histórico se conserva. Véase `revision-20260914-plan.md` y su gate de segunda fase.

`IdctRow` (objetivo 516 B, fuente 496 B / 46,372093 %) y `IdctColumn` (632 B, 50,449368 %) deben pasar a búsqueda de procedencia: versión original del IDCT, macros o compilador/flags primarios. La primera divergencia de `IdctColumn` es la herencia de `src[4]` en registro frente a recarga de pila; los pines, hoists y locales ya están agotados. Hasta aparecer evidencia primaria, no es rentable otro barrido de registros.

### 3. Cerrar los 25 cuerpos y resolver explícitamente el padding

`pad_00_8000348C_init` son 20 B de alineación introducidos al convertir a DOL; no pertenecen a una sección del ELF original. Es una incidencia de contabilidad todavía pendiente dentro del denominador actual: no debe eliminarse silenciosamente del denominador, crear una función falsa ni mapearse a otro símbolo. Hasta que exista una corrección formal, el estado debe informarse como “25 cuerpos reales pendientes + 20 B de padding contabilizado”.

La promoción no es la prioridad actual: se abordará **después** de sustituir asm por C legítimo y alcanzar 100 % match. La diferencia bruta `608 − 525 = 83` no representa 83 fuentes promocionables: contiene 72 unidades `auto_*` y `main/prodg_fixes`. Las diez unidades fuente code-perfect pero aún no completas son `zAI`, `zEAXSound`, `zFe`, `zFe2`, `zGameplay`, `zLua`, `zMisc`, `zPhysics`, `zPlatform` y `zSpeech`. Ése es el inventario fuente inicial para la fase linked posterior; automáticas y `prodg_fixes` exigen tratamiento separado.

## Inventario restante y decisión

| Unidad | Cuerpos pendientes | Decisión inmediata |
|---|---|---|
| zCamera | `ICEMover::Update`, static init, `TrackCarCameraMover::Update` (8.464 B) | Abrir sólo static init; ICEMover ya agotó combinaciones y necesita evidencia primaria nueva. |
| zWorld | `RenderFlaresOnCar`, `UpdateWheelYRenderOffset`, `DefragmentPool`, `SetMemoryPoolSize` (4.772 B) | Aparcar: hay pruebas de estructura/pases y rutas causales negativas. |
| zEcstasy | `epCalculate`, `UpdatePlatInfo`, `eProject` (4.384 B) | Conservar mejores baselines; no repetir pines/productores ya medidos. |
| zWorld2 | `HolePunchAvoidables`, `InitAtSegment` (3.796 B) | Aparcar hasta nueva evidencia ABI/DWARF. |
| zEagl4Anim | `DynamicLoader::Initialize`, `EvalState` (2.808 B) | No repetir cientos de variantes; buscar procedencia/compilador. |
| steering | seis cuerpos, de `SimThread_Step` a `Effect_PerformEnvelope` (2.568 B) | No repetir CookValues ni barridos MW; priorizar fuente/compilador original. |
| zEAXSound2 | `GenerateRoadNoise` (1.240 B) | Datos ya resueltos; el resto necesita evidencia de emisión, no más pines. |
| madidct | `IdctColumn`, `IdctRow` (1.148 B objetivo) | Ghost retirado; buscar implementación original. |
| zPhysicsBehaviors | `UpdateLoaded` (856 B) | Literal automático sin símbolo estable; no forzar ordinales o asm. |
| zTrack | `GetLoadingPriority` (708 B) | Aparcar hasta evidencia DWARF distinta. |
| auto | padding `.init` (20 B) | Corregir clasificación, nunca maquillarlo como función. |

## Secuencia hasta 100 %

1. Congelar el gate r68 y repetir el censo exacto de asm sobre el árbol definitivo.
2. Sustituir asm por C legítimo. `IdctRow` ya está sin ghost; continuar con otros andamios y convertir datos/alias sólo con equivalencia ELF y DOL demostrada.
3. Alcanzar 100 % match: ejecutar una tanda acotada de static init zCamera y, para el resto, cambiar a recuperación de fuente/procedencia, DWARF y compiladores faltantes. Todo negativo vuelve byte a byte al baseline; no se repiten permutaciones vedadas.
4. Resolver transparentemente la contabilidad del padding de 20 B sin sacarlo unilateralmente del denominador.
5. Sólo entonces abrir la fase linked: comenzar por las diez unidades fuente code-perfect identificadas; tratar aparte las 72 automáticas y `prodg_fixes`, y promover después cada TU cerrada.
6. Gate de cada tanda: objetos frescos, `fncmp` de toda la TU, bytes/datos/literales/relocaciones, `lcfix --check`, report regenerado y enlace con SHA original. Una mejora fuzzy o de tamaño nunca cuenta como cierre.

Con esta disciplina, “100 % match” significa cuerpos reales exactos sin asm fantasma; “100 % linked” significa además que la composición completa reproduce el DOL original. El orden acordado es estricto: primero ASM → C real, después 100 % match y finalmente 100 % linked.
