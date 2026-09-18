# r38 — `spchsamp`

Fecha: 2026-09-07. Base: `590dc4a0`.

## Alcance y estado de entrada

Se trabajo exclusivamente sobre
`src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c`; no se toco
`spchpick.c`, cabeceras, configuracion, flags ni `SourceLists`.

La unidad contiene dos funciones y 184 B de texto:

| funcion | tamano | estado final |
|---|---:|---:|
| `iSPCH_GetSampleParmAddr` | 48 B | 100 % |
| `iSPCH_GetSampleSizeData` | 136 B | 85,73529 % |

El diff inicial de `iSPCH_GetSampleSizeData` tiene 15 filas. No faltan
instrucciones semanticas: queda una rotacion de cinco valores entre registros.
El DWARF original asigna `sampleSize/r7`, `blockSize/r11`, `offset/r8`,
`nextSampleData/r4` y `result/r0`; el compilado actual usa respectivamente
`r8`, `r4`, `r11`, `r7` y `r9`. Se conserva el orden fiel de stores
`sampleOffset` antes de `dataBytes` aunque puntue peor, porque los punteros
pueden aliasar.

## Hipotesis nueva comprobada

La primera divergencia causal adicional a `sampleSize` aparece en la
expansion de `blockSize`: el objetivo conserva `r11` en `lbz/addi/slwi`,
mientras la fuente pasa de `r11` a `r4` en `slwi`. Se comprobo una familia
acotada para distinguir el temporal anonimo de la variable real, sin fijar
registros:

| variante | tamano / fuzzy | resultado |
|---|---:|---|
| asignar `hdr->blockSize + 1` y luego `<<= 8` | 136 B / 77,5 % | conserva `hdr` en `r7`, 33 filas distintas |
| materializar `blockSize` con restriccion vacia sobre su valor real | 136 B / 77,79412 % | el `slwi` va a `r3` y tambien prolonga `hdr` |
| cargar primero `hdr->blockSize` en `blockSize` y expandir despues | 136 B / 77,05882 % | no coalesiona; mueve `hdr` y el contador de muestras |
| dependencia vacia real `sampleSize <- blockSize` antes del indice | 136 B / 58,058823 % | altera el orden de cargas y agranda el diff |

Las cuatro son microformas de una sola causa propuesta; no se hizo barrido de
pines. El resultado descarta que la rotacion pendiente provenga solo de la
materializacion o el scope de `blockSize`. Los tres pines antiguos de r37 no
se repitieron.

## Restauracion y auditoria

Todas las variantes fueron retiradas. La fuente vuelve al blob de entrada
`d7f25cf824f9776950934019c475f81911e18f92`; `git diff --exit-code` devuelve
cero. El snapshot completo final es byte por byte identico al inicial
(`EF0217B8564EC612236127915F94B1E6D2B78EE1A0944AFA4FD0D81A559129A3` para
ambos JSON), por lo que no hay regresiones de instrucciones, simbolos,
secciones ni reubicaciones. La funcion exacta de 48 B pasa `scripts/audit.py`:
bytes correctos, cero ramas, cero relocs y cero literales.

Artefactos: `scratchpad/codex_r38_spchsamp/` (`before.json`, `after.json` y
los cuatro snapshots descartados). No se retiene ningun cambio de codigo.

