# Iter3 seguimiento — `spchsamp` / `iSPCH_GetSampleSizeData`

**Resultado: negativo, 0 B retenidos.** Se midieron las dos formas autorizadas
en `scratchpad/codex_20260908_iter3_spchsamp_followup`. Produccion, EOL CRLF,
objeto y los artefactos `c3` permanecen byte a byte intactos.

Partida: `c3`, 136 B al 93,52941 %. Solo faltaban encadenar `add` y `mullw` en
el offset de `r8`, y colocar `subf` antes de ambos stores.

| forma | cambio causal | resultado |
|---|---|---:|
| `c4` | descomponer el productor real de `offset` en `<<=`, `+=`, `*=`; dependencia vacia `+r(offset):r(endOffset)` antes del store | 90,00000 % |
| `c5` | expresion de suma original, barrera `+r(offset)` entre suma/producto y dependencia del **puntero real** `sampleOffset` sobre `endOffset` | 91,17647 % |

Ambas consiguen que `subf` preceda los dos stores. Sin embargo, prolongar la
dependencia hasta `sampleOffset` cambia el reparto global: `sampleData` pasa de
`r10` a `r8`, `offset` termina en `r10`, `endOffset` en `r3`, y el productor
`li r0,1` se agenda antes de los stores. `c5` si encadena suma y producto en un
solo registro, pero es `r10`, no el `r8` del ELF. `c4` produce la misma rotacion
general con una forma menos favorable.

La barrera selectiva demuestra que el orden de `subf` era una dependencia
alcanzable, pero no es independiente del reparto de los cinco valores. La vida
adicional del puntero desplaza precisamente el registro que se pretendia fijar.
No se anadieron inputs fantasma, pines nuevos ni instrucciones ASM del cuerpo;
no se repitieron `c1/c2/c3` ni las permutaciones agotadas.

Gate:

```text
python scratchpad/codex_20260908_iter3_spchsamp_followup/audit_negative.py
python scripts/lcfix.py spchsamp --check
```

No hay una tercera forma autorizada. El siguiente intento solo tendria sentido
si aparece una expresion C que haga depender la **direccion del store** de
`endOffset` sin prolongar la vida ABI de `sampleOffset`; las barreras directas
sobre el puntero y el valor ya quedan medidas como regresivas.
