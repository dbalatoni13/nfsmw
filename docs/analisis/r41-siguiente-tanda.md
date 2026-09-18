# r41 - shortlist para la siguiente tanda

Fecha: 2026-09-08. Analisis solo lectura sobre el censo final de r41
(3.893.124/3.946.048 B, 18.373 funciones exactas oficiales, 52.924 B en 59 entradas de
27 unidades). Se excluyen `RBGrid::Add`, `Tire::UpdateLoaded`, `steering`, `vm`
y los techos de asignacion ya agotados. No se compilo ni se modificaron fuentes,
objetos, configuracion o flags.

## Prioridad 1: `CARSFX_RoadNoise::Play`

| campo | valor |
|---|---|
| unidad | `Speed/Indep/SourceLists/zEAXSound2` |
| TU | `Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp` |
| simbolo | `Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi` |
| estado real r41 | objetivo 392 B / fuente 388 B, 94,938774 %, 29 filas de `fndiff` |
| permiso compartido | no para la primera prueba; solo se cambia el argumento del sitio de llamada |

**Hipotesis nueva, no medida:** probar, uno por uno y con limite estricto, `-1`
en cada argumento actualmente cero del constructor `FX_ROADNOISE` cuyo setter
satura negativos a cero. El estado final del objeto sigue siendo cero, pero el
front-end ve una expresion distinta antes de plegar el setter y puede partir la
constante cero en los dos pseudos que exige el objetivo.

La evidencia es especialmente fuerte despues del cierre de r41:

- `MsgPlayMiscSound` tenia la misma firma de expansion de constructor y se cerro
  usando precisamente `width=-1`, que `SetWidth` satura a cero.
- Aqui el objetivo materializa dos ceros (`li r29,0` y `li r0,0`) y la fuente
  solo uno (`li r30,0`); esos 4 B explican el tamano 392/388.
- `SetVolume`, `SetAzimuth`, `SetSecondaryNoise`, `SetSpeed`,
  `SetFilter_Effects_HiPass` y `SetFilter_Effects_Wet_FX` aceptan `int`, prueban
  `x < 0` y escriben cero. Son candidatos semanticos reales; `id`, `pitch`,
  `type`, `LoPass` y `Dry_FX` no deben alterarse porque no son ceros en este
  sitio.
- Las 110 permutaciones antiguas del constructor y los intentos de forzar un
  cero propio no probaron sentinelas negativos en el sitio de llamada. Esta via
  no repite esos barridos y deja intacto el testigo exacto `InitSFX`.

Orden sugerido: `Wet_FX`, `HiPass`, `speed`, `secondaryNoise`, `azimuth`,
`volume`, deteniendose si el reparto de los dos `li` no cambia. Si una variante
mueve el corte, combinar como maximo dos posiciones guiadas por el orden de
stores del objetivo; no volver a permutar los once setters.

## Prioridad 2: copia de `_Storage<CameraAI::Director*,2>`

| campo | valor |
|---|---|
| unidad | `Speed/Indep/SourceLists/zCamera` |
| TU de instanciacion | `Speed/Indep/SourceLists/zCamera.cpp` / fuentes Camera |
| fuente causal | `Speed/Indep/Libs/Support/Utility/UTLVector.h`, `Vector::assign()` |
| simbolo | `__Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2RCQ33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2` |
| estado real r41 | 1.156/1.156 B, 98,69896 %, 22 filas de `fndiff` |
| permiso compartido | **si**; `UTLVector.h` afecta al menos 13 SourceLists y exige A/B de todos los consumidores |

**Hipotesis concreta aun no medida como tal:** dar a `size()` un valor C real de
vida larga en `Vector::assign()` (`size_type curSize = size()`) y usar ese valor
en la condicion del bucle de crecimiento, manteniendo el orden ya reconstruido
de `minSize`, `srcIt` y `destIt`. Primero debe probarse con una copia sombra del
header; no se propone retener una barrera ni un pin.

Evidencia causal:

- El objetivo conserva `mSize` en `r4` a traves del bucle y lo refresca con
  `mr r4,r9`; la fuente actual recarga `mSize` con `lwz r4,0x8(r31)`. Es una
  discrepancia de valor/expresion reutilizada, no un simple nombre de registro.
- Las seis filas de ese racimo son una permuta `mr` contra `lwz` de coste igual;
  por eso el tamano ya coincide. Nombrar y reutilizar el resultado de `size()`
  ataca exactamente esa diferencia.
- El informe r36c ya dejo esta forma como siguiente paso, pero no se aplico por
  estar en una cabecera compartida. Los ensayos posteriores se centraron en
  pines, orden de declaraciones y barreras, no en esta cache de `size()` con su
  alcance minimo.

Gate obligatorio: compilar en sombra las unidades consumidoras y rechazar la
variante ante cualquier regresion de porcentaje o tamano, no solo ante la
perdida de una funcion exacta. En particular hay que vigilar
`SetupNextLoad`, que historicamente se mueve 4 B con cambios de `assign()`.

## Descartes de la criba

No se recomiendan como siguiente tanda `ICEMover::Update`,
`TrackCarCameraMover::Update`, `TerrainVelocityNoise`, `InitAtSegment`,
`CullParts`, `DefragmentPool`, `FnStatelessQ::EvalSQT`, `cStichWrapper::Play`,
`LoadSpeechBank`, `CompletePCreadAsync`, `PATH_createstreamimp` ni
`VP6_PredictFilteredBlock`: sus diferencias actuales ya estan atribuidas a
reparto/planificacion o tienen baterias amplias de variantes negativas. Volver a
ellos sin una evidencia nueva seria un barrido de registros, justo lo que esta
shortlist evita.
