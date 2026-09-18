# Iter3 — `spchsamp` (`iSPCH_GetSampleSizeData`)

## Alcance y referencia

Se trabajo exclusivamente en sombras bajo
`scratchpad/codex_20260908_iter3_spchsamp`. La fuente de produccion
`src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp.c`, su objeto, la
configuracion y los splits no se modificaron. El control recompilado desde una
copia byte a byte de la fuente conserva las dos funciones y los 184 B de
`.text` al 100 % respecto al objeto de produccion; `.rodata`, `.data` y `.bss`
tambien son identicos. La fuente sigue en CRLF puro.

La referencia real de la funcion pendiente es 136/136 B al 85,73529 %. La
otra funcion del TU, `iSPCH_GetSampleParmAddr` (48 B), sigue al 100 % en todos
los ensayos.

## Hipotesis acotada

El resultado de exito comparte `r0` con `endOffset` en el ELF. Se probo un
productor real de ese resultado:

```c
register int result asm("r0");
/* ... calculo y stores del tamano real ... */
__asm__("li %0,1" : "=r"(result) : "r"(endOffset));
```

El operando de entrada es el tamano final definido y ya escrito, no una lectura
indefinida ni un valor fantasma. El `li` es la instruccion real que produce el
valor semantico retornado.

| variante | cambio adicional | similitud |
|---|---|---:|
| control | fuente vigente | 85,73529 % |
| `c1` | productor `li`, `result` en `r0`, tamano final vivo | 92,64706 % |
| `c2` | `c1` + `sampleSize` en `r7`, segun DWARF | 92,50000 % |
| `c3` | `c2` + `numParms` en `r9`, segun DWARF | **93,52941 %** |

La primera generacion accidental de `c3` fijo el `numParms` del vecino, por
una coincidencia duplicada del texto. Se corrigio el arnes para aplicar el
cambio dentro de `iSPCH_GetSampleSizeData`; la tabla contiene exclusivamente
la medida corregida y el vecino permanecio exacto.

## Diferencia restante y veredicto

`c3` reproduce las instrucciones 0--17 y 20--27, el `li r0,0`, el bloque de
salida `li r0,1; mr r3,r0`, `sampleSize=r7`, `numParms=r9`, `blockSize=r11` y
`nextSampleData=r4`. Quedan dos problemas conectados:

1. El ELF acumula el offset directamente en `r8`
   (`add r8,r0,r9; mullw r8,r8,r11`); `c3` usa `r0` como intermedio y deja el
   producto en `r8`.
2. El ELF emite `subf r0,r8,r0; stw r8,0(r5); stw r0,0(r6)`. GCC agenda en
   `c3` `stw r8,0(r5); subf r0,r8,r0; stw r0,0(r6)`.

No se abrio una cuarta variante ni se repitieron las 48 ordenaciones o los
pines agotados de r46. La mejora queda solo como evidencia de que el productor
real resuelve la vida de `result`; no se retiene codigo parcial porque no hay
cierre al 100 %.

Gate reproducible:

```text
python scratchpad/codex_20260908_iter3_spchsamp/prepare.py
python scratchpad/codex_20260908_iter3_spchsamp/build.py
python scratchpad/codex_20260908_iter3_spchsamp/audit_negative.py
python scripts/lcfix.py spchsamp --check
python scripts/fncmp.py Speed/Indep/Libs/spch/dev/source/library/cmn/spchsamp
```

Resultado esperado: el auditor da dos `PASS`, `lcfix` queda limpio y `fncmp`
mantiene 1/2 funciones exactas, con los 136 B pendientes y 15 instrucciones
diferentes en produccion.
