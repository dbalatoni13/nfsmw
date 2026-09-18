# Iter4 — `spchsamp` / productores aritmeticos reales

**Resultado: negativo, 0 B retenidos.** Las cuatro variantes autorizadas se
compilaron en `scratchpad/codex_20260908_iter4_spchsamp`; produccion y el `c3`
de iter3 permanecen byte a byte intactos.

Partida: `c3`, 136 B al 93,52941 %. A diferencia del seguimiento anterior,
esta ronda no uso barreras para simular las operaciones: `add`, `mullw`,
`subf` y los stores fueron las instrucciones reales de bloques ASM locales,
alimentadas exclusivamente por valores C definidos.

## Resultados

| forma | productor real | resultado |
|---|---|---:|
| `c1` | `offset <<= 8` en C; `add/mullw` con output early-clobber; `subf` y ambos stores en un bloque | 87,94118 %, 140 B |
| `c2` | reutiliza `endOffset` como desplazado antes de sobrescribirlo en las dos ramas | 91,17647 %, 136 B |
| `c3` | ademas reutiliza el `numParms` real/r9 para el byte bajo y calcula `nextSampleData` antes del bloque | 92,50000 %, 136 B |
| `c4` | separa `add` y `mullw` en dos bloques de una instruccion; `add` usa output normal y `mullw` un read/write real | 91,17647 %, 136 B |

La otra funcion del TU, `iSPCH_GetSampleParmAddr` (48 B), permanece al 100 %
en las cuatro. No hay datos auxiliares ni reubicaciones externas; el candidato
fuente conserva unicamente sus tres reubicaciones de rama interna, con tipos y
addends validados por variante.

## Lo demostrado

Encerrar `subf`, `stw offset` y `stw bytes` en un unico productor resuelve
exactamente el orden de la cola. La dependencia de `li r0,1` sobre el
`endOffset` producido lo mantiene despues del bloque, tambien en el orden del
ELF. En `c1`, las instrucciones 30--35 son ya la cola objetivo exacta.

El bloqueo restante es el early-clobber necesario para que el destino de
`add` no pise los operandos que aun consume `mullw`. Con el mismo `offset` como
input y output, GCC crea un output temporal `r9` y despues `mr r8,r9`; con
`endOffset` como input separado, el output pasa a `r4` y rota las vidas de
`nextSampleData`/`endOffset`. Reutilizar `numParms` conserva el byte bajo en
`r9`, pero no recupera el destino `r8`.

La prueba adicional `c4` elimina ese early-clobber global de forma segura: el
`add` puede compartir registro con un input porque todos sus operandos se
consumen a la vez, y el `mullw` posterior usa `+r(offset)`. Desaparece el `mr`
de `c1`, pero el asignador rota el prefijo (`numSamples` a r10, base a r8 y el
offset final a r10). La cola conserva su orden aunque usa r10 donde el objetivo
usa r8. La puntuacion vuelve a 91,17647 %, sin mejora sobre `c3`.

Ninguna de las cuatro supera el `c3` de iter3 de partida, que ya llevaba el
productor real `li` del resultado. No se deben integrar bloques parciales.

## Gate

```text
python scratchpad/codex_20260908_iter4_spchsamp/audit_negative.py
python scripts/lcfix.py spchsamp --check
```

El auditor comprueba hashes de fuente/objetos/c3, CRLF, inventario de las dos
funciones, tamanos, ausencia de datos y perfil exacto de las tres ramas
internas. Quedan medidas tanto la forma
`add/mullw` multiinstruccion con early-clobber como la separacion segura en
dos productores reales; ambas rotan registros en vez de recuperar el r8 del
objetivo.
