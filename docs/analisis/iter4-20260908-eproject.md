# Iter4 2026-09-08 — `eProject`

## Resultado

No hay cierre nuevo ni cambio de producción. `eProject__FfffPA3_fPfN44`
permanece en **268 B / 93,970146 %** y la fuente y el objeto reales de
`zEcstasy` quedaron intactos. Las tres pruebas permitidas se hicieron sobre
copias privadas en `scratchpad/codex_20260908_iter4_eproject`.

## Base y causa comprobada

Se leyeron completos `brief-r46.md`, `r46-ecs.md` y las vedas pertinentes de
r21, r24, r25, r30, r36d, r36f y r38. No se repitieron los barridos ya agotados
de pines, barreras, destinatarios ni reasociaciones.

El control recompilado reproduce el objeto actual: 268 B, 93,970146 % y 14
filas alineadas distintas. La primera diferencia causal sigue en el horario de
la proyección X:

- el ELF objetivo carga `vp[0]`, calcula `halfVP2`, carga `eye.x` y emite el
  `fadds` de `vp[0] + halfVP2` antes del `fmadds` de `clipX`;
- ProDG carga `eye.x`, calcula `halfVP2`, carga `vp[0]` y emite primero el
  `fmadds` de `clipX`, dejando el `fadds` después.

La hipótesis nueva fue expresar esas operaciones como productores aritméticos
reales de inline ASM, siempre con entradas inicializadas y salidas consumidas
por la expresión C. No se usaron registros fijos, entradas fantasma ni
barreras vacías nuevas.

## Tres variantes acotadas

| variante | cambio causal | tamaño | similitud | filas distintas | veredicto |
|---|---|---:|---:|---:|---|
| control | fuente vigente | 268 B | 93,970146 % | 14 | base exacta |
| `c1` | productor real `fadds` para `screenXBase` | 268 B | 92,582090 % | 15 | negativo |
| `c2` | productor real `fmadds` para `clipX` | 268 B | 92,835820 % | 14 | negativo |
| `c3` | ambos productores reales en orden `fadds` → `fmadds` | 268 B | 86,820890 % | 35 | negativo |

`c1` mueve el `fadds` una ranura, pero no lo adelanta al productor de `clipX`:
el `fmadds` queda en la fila 36 y el `fadds` en la 37. `c2` conserva el mismo
orden no deseado. `c3` demuestra que imponer el orden de ambas operaciones sí
es posible, pero altera de forma amplia el reparto FPR, incluso desde la
comparación con cero; además difiere la inicialización fuente de `clipX`
confirmada por DWARF. Por tanto no es una reconstrucción retenible.

La evidencia descarta que un productor aislado baste para resolver esta
función. Otra prueba necesitaría una causa nueva en el reparto global de FPR o
evidencia primaria distinta; combinar más dependencias equivaldría a reabrir
los barridos vedados.

## Gate de restauración

`python scratchpad/codex_20260908_iter4_eproject/audit_negative.py` pasa y
comprueba:

- hashes de la fuente, objeto fuente y objeto objetivo de producción sin
  cambios, fuente CRLF pura y control byte a byte idéntico;
- inventario completo de 23 funciones del TU: sólo cambia `eProject` en las
  sombras y se preservan las 19 vecinas exactas;
- datos y literales runtime idénticos; `.text` y relocaciones fuera del cuerpo
  de 268 B idénticos;
- los mismos nueve destinos, tipos y addends de relocación dentro de
  `eProject` en las tres variantes;
- ninguna variante alcanza 100 %, por lo que ninguna es integrable.

`python scripts/lcfix.py --check` también queda limpio. No se ejecutó Ninja
global ni se tocaron cabeceras, flags, configuración, splits o commits.
