# iter2 2026-09-08 — zCamera / `__static_initialization_and_destruction_0`

**Resultado retenido: 0 B / 0 funciones. Produccion no se edito ni se
compilo.** Las tres pruebas se hicieron con un include sombra privado en
`scratchpad/codex_20260908_iter2_camera/`. Los cambios locales previos de
`ICEReplay.cpp` y `Cubic.cpp` no se tocaron. `CopView.cpp` continua sin diff y
`lcfix.py --check` da PASS.

## Estado de partida

El baseline reproduce el estado de r46:

| forma | match | RTX hasta `ReplayCategoryTable` | RTX bloque |
|---|---:|---:|---:|
| produccion | 97,112100 % | 933 | 1.107 |

La tabla ocupa 174 RTX. Para que `cse.c` no vacie su hash a las 1.001
instrucciones, debe empezar como maximo en 826. Faltan 107 RTX. En el RTL
inicial, `CopView.cpp:10` carga los literales con dos RTX (`high` y
`MEM(lo_sum)`), pero `CopView.cpp:17` usa tres (`high`, `lo_sum` a pseudo y
`MEM(pseudo)`). La transicion cae entre ambos arrays, justo en la construccion
de `tCopViewDistanceFovBand`.

## Las tres pruebas acotadas

| prueba | cambio fuente completo | match | tabla | bloque | lectura |
|---|---|---:|---:|---:|---|
| 1 | mover `tCopViewDistanceFovBand` despues de `vCopViewPoints` | 89,604880 % | 918 | 1.092 | `vCopViewPoints` entero vuelve a la forma barata; descarta valor, signo, longitud y pseudo 1.000, pero cambia orden de inicializacion y no es retenible |
| 2 | envolver cada elemento de `vCopViewPoints` en una factory inline de `bVector3` | 97,477250 % | 928 | 1.102 | ahorra un RTX por retorno agregado, pero conserva las 15 cargas caras |
| 3 | envolver solo la construccion real de `tCopViewDistanceFovBand` en una factory inline que retorna `tTable<bVector3>` | **98,369590 %** | **884** | **1.058** | conserva orden y globales; las 15 cargas posteriores pasan a `MEM(lo_sum)` y desaparecen 49 RTX antes de la tabla |

La tercera forma arregla dos de los cuatro `addi r27,r30,{48,72,96,120}` que
caracterizan el vaciado dentro de `ReplayCategoryTable`, pero aun queda 58 RTX
por encima de 826. No llega al 100 %, por lo que tambien se descarto.

## Condicion causal acotada

No existe un umbral en el numero de pseudo y `cse_not_expected` permanece en
cero durante la funcion. El discriminante es la **primera preparacion del
constructor inline de la especializacion `tTable<bVector3>` que usa el pool de
constantes**.

La evidencia del compilador es coherente con el resultado:

- `integrate.c::save_for_inline_copying`, cuando
  `current_function_uses_const_pool`, ejecuta `save_constants`, reinicia
  `const_rtx_hash_table` y llama a `clear_const_double_mem`;
- `varasm.c::force_const_mem` reutiliza `CONST_DOUBLE_MEM` si ya existe; de lo
  contrario crea una referencia nueva que `memory_address` materializa mediante
  `force_reg` por estar activos CSE y `-fforce-addr`;
- la factory de tabla adelanta la preparacion de esa especializacion fuera del
  punto medio del inicializador. En el RTL resultante, las cinco apariciones de
  `-2.0f` y los otros diez argumentos de `vCopViewPoints` son directamente
  `MEM(lo_sum)`. La factory de puntos no cambia ese estado y deja las cargas
  caras.

No se instrumento el ejecutable del compilador, asi que el enlace entre la
preparacion de la especializacion y `clear_const_double_mem` es una inferencia
de fuente + A/B, no una traza de llamada. Aun asi, las pruebas separan el evento
de los valores, el tamano del array, el numero de pseudo y el retorno agregado.

## Verificacion y siguiente accion

Reproducir sin compilar:

```text
python scratchpad/codex_20260908_iter2_camera/audit_final.py
```

Recompilar una sombra concreta:

```text
python scratchpad/codex_20260908_iter2_camera/probe.py table_factory
```

El siguiente intento, si se autoriza otra ronda, no debe repetir wrappers por
elemento ni mover globales. La unica direccion sustentada es localizar la forma
historica que preparaba la especializacion `tTable<bVector3>` antes del
inicializador sin introducir simbolos ni cambiar la secuencia de construccion.
La factory actual demuestra la palanca, pero por si sola solo recupera 49 de los
107 RTX y no es candidata a integracion.
