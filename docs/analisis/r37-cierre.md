# r37 — cierre de tanda y punto de continuacion

2026-09-07. Repositorio `C:/Users/jferr/Desktop/nfsdecompiled`, base
`590dc4a0`. Trabajo coordinado con tres agentes en territorios exclusivos.
Todos han terminado; no queda una compilacion ni un ensayo temporal activo.
El objetivo del 100 % **sigue pendiente**.

## Progreso oficial actualizado

| medida | antes | despues | delta |
|---|---:|---:|---:|
| codigo exacto | 3.889.664 / 3.946.048 B | 3.889.916 / 3.946.048 B | **+252 B** |
| porcentaje de codigo exacto | 98,57113 % | **98,577515 %** | +0,006385 pp |
| funciones exactas | 18.365 / 18.432 | **18.366 / 18.432** | **+1** |
| codigo linked | 735.668 B / 18,643158 % | igual | 0 |
| datos exactos | 443.749 / 1.285.741 B | igual | 0 |
| datos linked | 319.036 B | igual | 0 |

Quedan **56.132 B, 66 funciones y 29 unidades** con codigo pendiente.
El informe oficial se regenero con el comando REPORT de `build.ninja`, sin
invocar ninja completo ni cambiar configuracion.

El censo independiente `measure.py` usa otro denominador: pasa de
3.889.808 / 3.946.204 B y 18.371 funciones exactas a
3.890.060 / 3.946.204 B y 18.372. Confirma el mismo delta **+252 B / +1**.
La diferencia de 144 B exactos, 156 B totales y seis funciones respecto al
informe oficial ya existia antes y NO es ganancia de esta ronda.

## Cambios de fuente retenidos

1. **`filesys.cpp`: AddToQueue cerrado**, 252 B. Las 74 funciones y 10.872 B
   de la unidad ya coinciden. Tres dependencias vacias sobre valores
   inicializados fijan el orden de cargas y calculo; sin instrucciones ASM
   nuevas ni registros fijos. Las otras funciones y datos no cambian.
   Detalles y auditoria: `r37-filesys.md`.
2. **`TrackStreamer.cpp`: asociacion FP corregida con evidencia del original**.
   El mapa de lineas separa las tres multiplicaciones que la forma anterior
   permitia reagrupar a `-ffast-math`. No cierra GetLoadingPriority y no suma
   bytes: su similitud baja de 97,18079 % a 94,72317 % por el reparto de
   registros, con los mismos 708 B. Se conserva por fidelidad del calculo,
   no para mejorar una metrica. Ninguna de las otras 290 funciones emitidas
   cambia y las 258 exactas se mantienen. Detalles: `r37-track.md`.

**No afirmar «ninguna similitud baja»**: `pctsnap.py` detecta precisamente
esa diferencia en zTrack. Lo comprobado es que no se pierde ninguna funcion
exacta ni hay cambios colaterales de codigo, datos, literales o referencias.
Fuzzy global: 99,95061 % -> 99,95035 %.

## Frentes probados y restaurados

- `CarLoader::SetMemoryPoolSize` y `DefragmentPool`: `r37-world.md` y la
  continuacion de coordinacion en `r37-root-ensayos.md`. Las dependencias
  nuevas desplazan las ultimas diferencias, pero no cierran. Restaurados.
- `DebuggerDriver` (Poll, WriteN, callback): `r37-debugger.md`. Tres
  pendientes / 804 B; las 28 exactas permanecen. Restaurado.
- `cStichWrapper::Play` y `GinsuSynthData::BindToData`: `r37-sound.md`.
  Ningun cierre; incluso la mejora parcial del contador fue retirada.
- Constructor `IconScroller` y `iSPCH_GetSampleSizeData`:
  `r37-root-ensayos.md`. Todas las variantes retiradas.

Las instantaneas completas finales de los frentes restaurados coinciden con
las iniciales. Se normalizaron solo finales de linea CRLF de cuatro fuentes
restauradas; `git status --untracked-files=no` muestra unicamente los dos
ficheros retenidos de arriba. No se borraron los experimentos previos del
usuario, ni se hizo staging o commit.

## Verificacion global

- `measure.py --cmp`: una sola unidad gana, filesys, +252 B / +1 funcion.
- `pctsnap.py`: 15.493 funciones en 33 SourceLists, una unica variacion,
  GetLoadingPriority, explicitada arriba.
- Informe oficial: solo cambian medidas de filesys y fuzzy de zTrack.
- `audit.py` de filesys: 74/74 verificadas contra bytes del ELF, ramas,
  referencias y literales. Auditor propio: 150 simbolos y 26 secciones.
- Auditoria zTrack: resto de funciones identico, datos/BSS y referencias
  intactos; 178 referencias literales comprobadas.
- `lcfix.py --check`: todas las entradas `@lc` al dia.
- `git diff --check`: correcto; `configure.py`, `config/` y `objdiff.json`
  no tienen cambios.
- DOL existente: SHA-1 **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
  No se reconstruyo el DOL principal; esta comprobacion no implica que las
  unidades NonMatching modificadas ya esten enlazadas.
- Enlace aislado de filesys: **DOL ROTO (21c5daf704f3...)** por disposicion
  de datos pendiente. No se promociono la unidad.

## Proxima tanda: evitar repetir las vedas

Priorizar nueva evidencia sobre variantes cosmeticas de las funciones de
dos filas ya barridas. Los mayores bloques son zCamera (10.812 B / 5),
zWorld (5.608 / 5), zEcstasy (5.180 / 4), zEagl4Anim (5.008 / 4), steering
(4.080 / 10), zWorld2 (3.796 / 2) y zEAXSound2 (3.564 / 6).

- En zTrack falta explicar honestamente el marco original `0x120` y la
  asignacion de registros. No restaurar el relleno `pos2` eliminado por
  `9de121ee`; haria falta un local respaldado por DWARF o evidencia del
  compilador original.
- En filesys, el codigo ya esta cerrado. Para linked investigar los 96 B
  adicionales de `.rodata`, los 4 B ausentes de `.sdata` y los nueve simbolos
  desplazados. No marcar Matching antes de que el enlace aislado produzca
  el SHA-1 original.
- Los cambios de splits/config y de cabeceras compartidas siguen sujetos a
  la regla de propuesta del `brief-r36f.md`; no aplicarlos en paralelo.
- Leer este cierre y los informes de territorio antes de relanzar pruebas.
  El brief r36f y comentarios antiguos no sustituyen al HEAD real.

## Instantaneas

En `scratchpad/`, prefijo `codex_20260907_r37_`:
`before_report.json`, `before_measure.json`, `after_measure.json`,
`before_pct.json`, `after_pct.json`. Informe actualizado:
`build/GOWE69/report.json`. Los informes de territorio enlazan sus JSON y
auditores especificos para reproducir cada comprobacion.
