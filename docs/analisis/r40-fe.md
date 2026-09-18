# Ronda 40 - zFe2 / zFeOverlay

## Alcance y baseline

Se trabajaron exclusivamente los dos cuerpos asignados, sobre los objetos
reales de `Speed/Indep/SourceLists/zFe2` y `zFeOverlay`:

- `IconScroller::IconScroller(const char *, const char *, const char *,
  const char *, float)`: 384 B, 97,770836 %, 8 instrucciones distintas.
- `CustomizeMain::SetScreenNames()`: 440 B, 91,727270 %.

`pctsnap.py` guardo las 1.774 funciones de ambas unidades en
`scratchpad/codex_r40_fe/before_pct.json`. Antes de probar, `audit.py` no
encontro fallos en ninguna funcion exacta. En zFe2 `fncmp.py` confirmo que el
constructor era el unico cuerpo con codigo distinto (los otros 308 casos eran
solo nombres/reubicaciones de alias).

## `CustomizeMain::SetScreenNames`

La rama `BackRoom` ya era exacta. En la rama normal el target hace el primer
store tras materializar las dos primeras cadenas. El conocido `+m` selectivo
reproduce ese orden y llega a 98 %, pero deja seis filas de asignacion:

- target: base del primer global en r3 y del segundo en r30;
- ProDG/GCC actual: bases en r30 y r9;
- como consecuencia, el `addi` de `gCarCustomizeManager` queda tres ranuras
  mas tarde.

Se probaron solamente extensiones causales de ese punto:

| Ensayo | Resultado | Diagnostico |
|---|---:|---|
| store mediante lvalue `volatile` local | 91,727270 % | binario identico al baseline; un unico acceso volatile no ordena los demas stores |
| `+m(first)` + input de la segunda cadena real | 98,000000 % | conserva exactamente las seis diferencias; el valor no cambia la vida de las bases |
| `+m(first)` + input de memoria del segundo global | 95,090910 % | rota todas las bases posteriores |
| `+m(first)` sobre un puntero local al primer slot | 98,000000 % | el compilador recupera la misma RTL que con el global directo |
| `+m(first)` + `+r` de la primera cadena real | 94,409090 % | alarga el valor y rota cadenas y bases |
| `+m(first)` + input `r` de la primera cadena real | 94,772730 % | adelanta el store demasiado y rota registros posteriores |
| input-only `m(first)` | 94,500000 % | fuerza el store antes de la segunda cadena y reproduce la barrera antigua, no el target |

No se retiene el 98 %: aporta cero bytes exactos y deja una barrera artificial.
El resto es asignacion/coalescing de registros, no una dependencia de fuente
que estas variantes puedan expresar.

## `IconScroller::IconScroller`

El baseline final tiene solo ocho diferencias de planificacion. El target
emite los stores en el orden semantico de la fuente: `fCurFadeTime` antes de
`fMaxFadeTime`, y `fXCenter`, `fYCenter`, `fCurrentAddPos` al final. GCC retrasa
`fCurFadeTime` y adelanta `fCurrentAddPos` aunque la fuente ya esta en el orden
correcto.

Ensayos acotados (el primero revalida una variante ya medida en r37; los
cuatro restantes prueban dependencias distintas):

| Ensayo | Resultado | Diagnostico |
|---|---:|---|
| input-only `m(fCurFadeTime)` (revalidacion r37) | 45,458332 %, 400 B | amplifica el alias de memoria y cambia marco/prologo |
| cero real con `+f`, usado por los cinco stores | 93,541664 %, 388 B | separa el CSE en dos registros FP y agrega un `fmr` |
| copia real de `this` con `+r` entre fade actual/maximo | 70,625000 %, 380 B | divide la base del objeto y cambia marco y registros guardados |
| `+m(fCurFadeTime)` + input FP del mismo cero real | 87,854164 %, 384 B | identico al `+m` ya vedado: mantener el cero como input no repara el CSE |
| stores `volatile` solo en los dos grupos float | 83,281250 %, 384 B | conserva el orden relativo, pero los adelanta antes de la preparacion de enteros/literales |

Esto refuerza la causa ya observada: el source tiene el orden correcto y el
desfase restante pertenece al scheduler. Las dependencias suficientemente
fuertes para mover esos stores tambien destruyen el CSE o el reparto global de
registros. No se conserva ninguna.

## Estado final y auditoria

Todos los ensayos fueron retirados con parches inversos. Los dos fuentes quedan
sin diff respecto al baseline de la ronda y se recompilaron de forma directa:

```
python scripts/build_direct.py zFe2 zFeOverlay
2 ok, 0 fallidas
```

`scratchpad/codex_r40_fe/after_pct.json` es semanticamente identico al snapshot
`before_pct.json` para las 1.774 funciones. Auditoria final:

- zFe2: 1.306 funciones exactas auditadas, 0 fallos; el constructor sigue en
  384 B / 97,770836 %, 8 instrucciones distintas.
- zFeOverlay: 466 funciones exactas auditadas, 0 fallos; `SetScreenNames`
  vuelve a 440 B / 91,727270 %.
- `lcfix.py --check`: todas las entradas `@lc` al dia.
- `git diff --check` y diff de los dos fuentes: limpios.
- datos/literales no cambiaron: zFe2 conserva `.bss` 3340/3520, `.data`
  5088/4988 (3387 bytes iguales) y `.rodata` 25704/22600 (5641 iguales);
  zFeOverlay conserva `.bss` 7216/3144, `.data` 940/924 (297 iguales) y
  `.rodata` 8472/8776 (3401 iguales).

Resultado de la ronda en estas unidades: **0 bytes y 0 funciones nuevas**, sin
regresiones ni deuda experimental. Ambos objetos quedan liberados.
