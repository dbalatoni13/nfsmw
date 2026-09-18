# r37 — ensayos complementarios de coordinacion

Base `590dc4a0`, 2026-09-07. Estos ensayos no deben confundirse con el cierre
real de 252 B documentado en `r37-filesys.md`.

## IconScroller, zFe2: tres variantes descartadas

Constructor `__12IconScrollerPCcN31f`: 384 B, 97,770836 %, ocho instrucciones
distintas. Se verificaron campos y ctor inline con DWARF y el historial de
vedas. Las ultimas diferencias son de orden de stores, en especial
`fCurFadeTime` y `fCurrentAddPos`, con reutilizacion de la constante cero.

| ensayo | resultado |
|---|---|
| entrada `m(fCurFadeTime)` inmediatamente tras asignarlo | 400 B, se rechaza |
| salida `+m(fCurFadeTime)` con entrada `f(fCurFadeTime)` | 384 B, 30 instrucciones distintas |
| `+m(fCurFadeTime):r(master)` y entrada `m(fCurrentAddPos)` tras su store | 380 B, se rechaza |

Todos retirados. `git diff` de la fuente vacio. Las instantaneas completas
`scratchpad/codex_r37_icon_before.json` y `codex_r37_icon_after.json` son
iguales como JSON, no solo en porcentaje: instrucciones, simbolos, datos y
reubicaciones restaurados. No se modificaron cabeceras compartidas.

## spchsamp: los pines no resuelven la rotacion

`iSPCH_GetSampleSizeData__FP10VOXBANKHDRiPUiT2`: base real 136 B, 85,73529 %.
Se leyeron r20-spch, r21-spch, r25-ventana y el historial de la fuente. Las
cinco asignaciones diferentes estan descritas por DWARF, pero fijarlas altera
tambien los temporales anonimos que comparten registros.

| ensayo | tamano / fuzzy | observacion |
|---|---:|---|
| pin de `result` a r0 | 136 / 85,588234 % | el resultado se fija, pero el producto final huye a r9 |
| ademas offset/r8, sampleSize/r7, blockSize/r11, nextSampleData/r4 | 136 / 84,55882 % | los temporales de las cargas iniciales cambian de banco |
| solo sampleSize/r7 sobre la base limpia | 136 / 85,29412 % | arrastra numSamples/result a r9/r10 |

Se retiran las tres. Fuente final igual a la copia inicial (normalizando solo
CRLF/LF) y los JSON completos anterior/posterior son iguales:
`scratchpad/codex_r37_spchsamp_before.json`, `codex_r37_spchsamp_after.json`.
No se retiene una asignacion de registro solo porque la sugiera DWARF cuando
el objeto real demuestra que empeora el conjunto.

## Continuacion de SetMemoryPoolSize

Los ensayos iniciales del agente estan en `r37-world.md`. Despues de liberar
ese territorio, coordinacion probo una combinacion no cubierta por las vedas:
dependencia de `pool_name` respecto al resultado real de
`bGetFreeMemoryPoolNum`, y dependencia de la memoria global respecto al nombre
ANTES del store. El registro global conservado ya no se pierde.

- R1: 304 B / 97,36842 %. Ahora `addi`/`stw` estan bien, pero el `lis` del
  nombre se adelanta a las dos cargas de argumentos.
- R2: expresar memoria/tamano en locales y usarlos como entradas de la primera
  dependencia deja las mismas dos diferencias de posicion del `lis`.
- R3: `+r(pool_mem):r(pool_size)` corrige el `lis`; quedan otras dos filas:
  la carga de tamano se adelanta al `mr r0,r3` del retorno. Tambien 304 B /
  97,36842 %. Instantanea `scratchpad/codex_r37_world_root_c3.json`.

Antes de explorar esa ultima dependencia se restauro y recompilo la base:
`scratchpad/codex_r37_world_root_after.json` resulto identico al JSON inicial
completo del agente. El veredicto final de la continuacion se anade abajo.

- R4: dependencia `+m(MemoryPoolSize):r(pool_num)` antes de cargar los
  argumentos. 304 B / 97,210526 %: corrige el adelanto del tamano, pero
  intercambia las cargas de tamano/memoria e intercala el `lis`.
- R5: extender esa dependencia tambien a `MemoryPoolMem`. 304 B / 94,73684 %:
  el par completo `lis/addi` se adelanta. Se descarta.

**Veredicto: ninguna variante retenida.** La cadena que cierra filesys no
transfiere directamente a esta preparacion de argumentos: el literal tiene
dos instrucciones y al limitar una se adelanta la otra. No repetir R1-R5 sin
una nueva explicacion del orden RTL o sus prioridades. Se conserva la base
de CarLoader, sin registros fijos ni dependencias anadidas. Las instantaneas
R3-R5 estan en `scratchpad/codex_r37_world_root_c{3,4,5}.json`.
