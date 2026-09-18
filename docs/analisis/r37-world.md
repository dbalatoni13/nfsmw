# r37 — `zWorld` / `CarLoader.cpp`

Fecha: 2026-09-07. Base de la ronda: `590dc4a0` y censo global
`scratchpad/codex_20260907_r37_before_measure.json` (3.889.808 / 3.946.204 B,
18.371 funciones exactas en la medida directa). Propiedad exclusiva de esta
subtarea: `src/Speed/Indep/Src/World/CarLoader.cpp`.

## Resultado

No se conserva ningun cambio en `CarLoader.cpp`: el fichero final tiene el
mismo SHA-256 que la copia anterior a los ensayos y `zWorld` produce el mismo
objeto fuente que la base. Las dos pendientes estudiadas quedan asi:

| funcion | tamano | estado final | diferencia real |
|---|---:|---:|---|
| `SetMemoryPoolSize__9CarLoaderi` | 304 B | 97,36842 % | dos filas: `stw` / `addi` intercambiados |
| `DefragmentPool__9CarLoader` | 684 B | 99,269005 % | 23 operandos, tres ciclos de reparto de registros |

El inventario completo permanece en cinco funciones / 5.608 B pendientes. No
hay cierre nuevo en este frente.

## `SetMemoryPoolSize`: hipotesis nuevas medidas

El DWARF, el mapa de lineas y el RTL vuelven a confirmar que la forma C base es
la correcta. El objetivo necesita dar al `addi r6, literal@l` un segundo
dependiente de prioridad no mayor que la llamada, para que el planificador lo
coloque delante del `stw` sin adelantar el `lis`. Los ensayos r36e/r36f con una
salida `+r` no cumplen simultaneamente esas dos condiciones.

Se probo el eje nuevo de una dependencia vacia **solo de entrada**:

| variante | tamano / fuzzy | resultado |
|---|---:|---|
| local `pool_name`, `asm("" : : "r"(pool_name))` | 308 B / 87,039474 % | pierde la base global conservada y rematerializa argumentos |
| entrada directa `asm("" : : "r"("Cars"))` | 316 B / 89,73684 % | duplica el literal y añade tres instrucciones |
| local ABI fijada en `r6`, entrada `"r"` | 308 B / 87,82895 % | evita parte de la duplicacion, pero sigue perdiendo la base global |

Las tres variantes son semanticamente legitimas y usan un valor real, pero
ninguna aisla el desempate buscado. Todas se retiraron. El diagnostico nuevo es
que el consumidor solo de entrada cambia el CSE/vida del literal antes de
afectar el desempate de las dos instrucciones; fijarlo al registro ABI tampoco
evita la perturbacion de `CarLoaderMemoryPoolNumber@ha`. No repetir este eje
con otro nombre local.

## `DefragmentPool`: ensayo acotado

La unica hipotesis nueva razonable era aumentar `n_refs` solo para
`num_hole_filling_allocations`, que el objetivo asigna a `r27` y la fuente a
`r25`, sin crear una salida ni prolongar otro allocno:

```cpp
asm("" : : "r"(num_hole_filling_allocations));
```

Se coloco inmediatamente antes del bucle final de liberacion. El objeto fue
identico byte a byte a la base: 684 B, 99,269005 % y los mismos 23 operandos.
Por tanto esa referencia terminal se elimina antes de influir en el reparto de
`local_alloc`. Se retiro. Los otros ciclos (`this`/temporal y
`zero`/tabla/base global) siguen sin un valor fuente nombrable que permita una
hipotesis localizada distinta de los barridos ya descartados.

## Auditoria final

- `build_direct.py` recompilo solo
  `build/GOWE69/src/Speed/Indep/SourceLists/zWorld.o` en las medidas finales.
  La primera invocacion abreviada de inspeccion tambien recompilo `zWorld2` por
  coincidencia de prefijo; no se modifico ninguna fuente de esa unidad.
- `fncmp.py Speed/Indep/SourceLists/zWorld`: 582 pares, las mismas cinco
  pendientes y los mismos 5.608 B.
- `pctsnap.py zWorld`: los 582 registros de `before_pct.json` y
  `after_pct.json` son identicos, cero regresiones.
- `scripts/audit.py`: 577 funciones exactas verificadas contra el ELF, con
  ramas, llamadas, tipos/addends de reubicacion y literales resueltos.
- `audit_restoration.py`: 2.883 simbolos, 846 funciones, 42.785 posiciones de
  instruccion, 3.876 ramas, 11.265 referencias y 23 secciones identicas a la
  instantanea inicial; tamanos, alineaciones, datos y reubicaciones incluidos.
- `python scripts/lcfix.py zWorld --check`: limpio.
- `CarLoader.cpp`: contenido y SHA-256 restaurados exactamente; `git diff`
  vacio.

Artefactos: `scratchpad/codex_r37_world/`, en particular
`before_zWorld_setmem.json`, `after_zWorld.json`, `before_pct.json`,
`after_pct.json`, `final_fncmp.txt`, `final_audit.txt`,
`restoration_audit.txt` y los tres informes de ensayo.

