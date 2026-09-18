# iter3 2026-09-08 — zCamera / preparacion de `tTable`

**Resultado retenido: 0 B / 0 funciones.** No se edito ni recompilo el objeto
de produccion. Los `cpp` reales `Camera.cpp` y `CopView.cpp` quedan sin diff y
se preservaron los cambios de usuario en `ICEReplay.cpp` y `Cubic.cpp`.

## De donde tenian que salir los 58 RTX

La mejor sombra de iter2 empezaba `ReplayCategoryTable` en 884 y dejaba el
bloque en 1.058. El limite exacto era 826 / 1.000: faltaban 58 RTX.

El mapa de `lo_sum` de `poolmap.py` encontro 85 cargas caras aun activas. Las
58 que habia que convertir no eran una estimacion:

- 55 pertenecen al prefijo: una sin linea, dos de `Camera.cpp:221`, 32 de
  `CameraMover.cpp:15-22` y 20 de `CameraMover.cpp:24`;
- tres son los casos mixtos de `Cubic.cpp:355`, `:361` y `:362`;
- las 27 posteriores (`Rearview`, `Showcase`, `Still`, `ICEMover`) podian
  permanecer caras: `884 - 58 = 826` y `1.058 - 58 = 1.000`.

Esto justificaba probar preparacion, no mas permutaciones de arrays.

## Tres variantes nuevas

Ninguna mueve globales ni elimina una construccion. Todas usan
**instanciacion explicita de constructor**, no las factories de iter2.

| # | preparacion | match | tamano objetivo/fuente | tabla | bloque |
|---|---|---:|---:|---:|---:|
| 1 | `tTable<bVector3>` junto a su uso en `CopView.cpp` | 98,369590 % | 3.604/3.612 | 884 | 1.058 |
| 2 | la misma peticion antes de `Camera.cpp`, antes del primer global | 98,369590 % | 3.604/3.612 | 884 | 1.058 |
| 3 | anterior + `tTable<bVector4>` de `TerrainVelocityNoise` | 98,369590 % | 3.604/3.612 | 884 | 1.058 |

Las tres generan **exactamente la misma secuencia de 908 instrucciones** para
el inicializador (SHA-256 de texto normalizado
`73dbe12b3cbd37237ca68cefa05392945833f613612935dd9b0153bf82fffa73`).
Tambien reproducen exactamente la mejor factory de tabla de iter2.

La posicion textual de la instanciacion explicita no adelanta su efecto: GCC la
difiere y la procesa en el mismo momento. Preparar tambien la especializacion
`bVector4` no altera un solo RTX. Ademas, la instanciacion explicita emite el
simbolo de constructor `__t6tTable1Z8bVector3P8bVector3iff`; la tercera emite
tambien `__t6tTable1Z8bVector4P8bVector4iff`. Aunque hubieran cerrado el
inicializador, no pasarian el inventario de la unidad.

## Condicion mejor delimitada

La relacion de iter2 con `save_for_inline_copying` / `clear_const_double_mem`
sigue siendo coherente: una factory usada y una instanciacion explicita
producen el mismo cambio de pool y el mismo cuerpo. Lo que queda descartado es
que se pueda **adelantar** el evento cambiando la posicion de una solicitud de
instanciacion en el fuente. El frontend la encola, y la especializacion
adicional no fuerza el punto buscado.

La unica prueba mecanica que podria mover el evento antes de los 55 literales
seria introducir/mover una definicion de funcion real, porque
`toplev.c::rest_of_compilation` llama a `clear_const_double_mem` al terminar
funciones. No se hizo: crear un helper fantasma anadiria un simbolo, y mover una
funcion existente cambiaria el orden `.text` y regresaria la unidad. No es una
forma historica sustentada ni una candidata valida.

## Reproduccion

```text
python scratchpad/codex_20260908_iter3_camera/audit_final.py
python scratchpad/codex_20260908_iter3_camera/poolmap.py scratchpad/codex_20260908_iter2_camera/table_factory/zCamera.i.jump
```

El gate comprueba las tres metricas, que los cuerpos son identicos, los
simbolos adicionales y que los dos fuentes reales permanecen intactos. La
familia de preparacion explicita queda agotada; repetirla en otra posicion o
con otra especializacion no tiene base nueva.
