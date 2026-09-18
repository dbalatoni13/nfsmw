# r44 - pathnode

## Resultado

No hay cierre nuevo: **+0 B / +0 funciones**. El archivo real es
`src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp`, no `.c`. Se
midieron tres hipótesis causales acotadas; dos empeoran y una se canoniza al
objeto de partida. Todas se retiraron.

Baseline reproducido con el objeto real:

| función | objetivo | fuente | fuzzy de código |
|---|---:|---:|---:|
| `PATHI_nextnode` | 308 B | 300 B | 93,05195 % |
| `PATHI_calcwaitbeat` | 336 B | 336 B | 98,86905 % |

La unidad conserva **8/11 funciones estrictamente exactas** en el JSON completo.
`PATHI_choosesynchtime` tiene código idéntico, pero cuatro relocaciones sólo
difieren por el nombre del símbolo; por eso `fncmp` informa únicamente dos
cuerpos de código distintos mientras `objdiff` estricto deja tres entradas por
debajo del 100 %.

## Evidencia del original

El DWARF de `pathnode.cpp` confirma para `PATHI_calcwaitbeat` los seis locales
originales `scalar` (`f11`), `fevery` (`f11`), `foffset` (`f0`), `timeinbar`
(`r8`), `firstsynchtime` (`r7`) y `nextsynchtime` (`r3`). No existe `notes` en
el original: el `register float notes asm("fr11")` actual es un auxiliar sólo
de fuente que mantiene la mejor emisión conocida.

Para `PATHI_nextnode`, el DWARF confirma el parámetro `forreal` en `r31` y los
locales `nodeinfo` en `r7`, `branches` en `r5`, `track`/`i` en `r10` y
`nextnode` en `r12`. El objetivo guarda `r31`, conserva `forreal` allí y emite
`mr r6,r9` para pasar `pfstate` al inline de `PATHI_getnode`. La compilación
actual mantiene `forreal` en `r12`, usa `r6` para `nextnode` y recarga
`pfstate` después. El orden de sentencias e inlines del mapa de líneas ya casa.

La procedencia es un posible factor, no una causa demostrada: el objeto objetivo
registra SN BUILD v1.83; ProDG 3.9.3 corresponde a SN BUILD v1.76 según
`compiladores-que-faltan.md`. Otras unidades path ya casan con v1.76. Las
notas r20-r36f ya agotaron pines, referencias extra, barreras y cambios de
expresión destinados sólo a permutar registros. No se repitieron esos barridos.

## Tres hipótesis nuevas medidas

1. **Vista local real de `pfstate` en `PATHI_nextnode`.** Se dio una sola
   identidad C al estado usado tanto por `PATHI_getnode` como por `track`, para
   comprobar si el compilador conservaba el argumento en `r6`. Resultado:
   **91,844154 %, 296 B**. Elimina la recarga, pero también el `mr r6,r9` y
   adelanta la carga de `pfstate`; se aleja del flujo objetivo. Revertida.
2. **División explícita en dos sentencias de `scalar`.** `scalar = notes;` y
   después `scalar /= (float)note` buscaba separar la vida de ambos pseudos de
   punto flotante sin añadir presión fantasma. Resultado: **98,5119 %, 336 B**,
   con 17 diferencias FPR frente a 14. Revertida.
3. **División in-place de `notes`.** `notes /= (float)note; scalar = notes;`
   probaba una dependencia real de escritura antes de copiar el valor.
   Resultado: exactamente el baseline, **98,86905 %, 336 B**. El optimizador la
   canoniza; revertida.

No se añadieron instrucciones asm, valores fantasma, cambios de flags, pines de
GPR ni cabeceras compartidas. Estas tres formas no justifican repetir un barrido
ciego. Una comparación con SN BUILD v1.83, si se dispone de él, o evidencia
nueva del front-end podría orientar la siguiente hipótesis; no se ha probado
que ese compilador sea imprescindible ni que no exista otra forma C válida.

## Verificación final

- Compilación exacta de
  `build/GOWE69/src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.o`: PASS.
- Los JSON completos `codex_r44_pathnode_before.json` y
  `codex_r44_pathnode_after.json` son byte por byte idénticos; esto incluye
  código, tamaños, datos, relocaciones y literales.
- `fncmp`: mismo inventario de 11 funciones; sólo los dos cuerpos objetivo
  siguen distintos.
- `audit.py Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode`: PASS, código de
  salida 0.
- `lcfix.py pathnode --check`: todos los `@lc` están actualizados.
- `pathnode.cpp`: SHA-256
  `237791968259642E42191C27C91B2994C430FC486CA8977FCE6236F0864F5FE1`,
  restaurado byte por byte y con LF puro.

Gate reproducible: `python scratchpad/codex_r44_pathnode_audit.py`.
Artefactos adicionales: `scratchpad/codex_r44_pathnode_trials.json` y los dos
snapshots completos mencionados arriba.
