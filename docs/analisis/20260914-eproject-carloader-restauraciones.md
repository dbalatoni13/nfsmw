# eProject y CarLoader: restauraciones de fuente, 2026-09-14

## Resultado integrado

Se continúa desde `20260914-integracion-fengine-sonido-defrag.md`.
Dos restauraciones de `CarLoader.cpp` están integradas y verificadas:

1. `SetMemoryPoolSize`: recuperar el bucle de asignaciones sponge eliminado
   por optimización, poner su `n` en el ámbito correcto y emitir
   `"SpongeAllocation"` como argumento C++ de `bMalloc`. Se retira únicamente
   la correspondiente línea `.asciz` (17 bytes) del bloque ASM mixto.
2. `DefragmentPool`: sustituir el `enum` artificial `kHoleProbeSize` por
   `extern int ChunkMovementOffset;`, declaración encontrada en el DWARF
   original. Se corrige el comentario que describía ese bloque como vacío.

**No se cierra ninguna función nueva ni se promociona zWorld.** Se mejora
la fidelidad de la fuente y se reduce la deuda artificial, sin regresión.

| Medida global | Antes | Después |
| --- | ---: | ---: |
| Código exacto | 3.915.284 / 3.946.048 B | Sin cambios |
| Código match | 99,22038 % | 99,22038 % |
| Funciones exactas | 18.406 / 18.432 | Sin cambios |
| Código linked | 1.264.380 B; 32,04168 % | Sin cambios |
| Unidades linked | 525 / 619 | Sin cambios |
| Datos match | 565.009 B | Sin cambios |

El informe global regenerado es idéntico al anterior, incluidos todos sus
registros de funciones. El DOL de un enlace nuevo del grafo real de 619
objetos es idéntico byte a byte al original (4.541.888 bytes):
`9619ba57c9919f95f7f2ac951a2166a3517f91e3` (SHA1).

## Evidencia y límites de CarLoader

### Sponge

El original contiene un segundo bloque con `n` sin ubicación, DIE
`0x4434192`, y otro inline de `bMalloc` dentro de él. La fuente previa
declaraba ese `n` en el bloque exterior y carecía del segundo inline.
Las constantes existentes son `CarLoaderMemorySpongeNumAllocs = 0` y
`CarLoaderMemorySpongeAllocSize = 102400`: el bucle recuperado no añade
instrucciones en este build. El literal original se sitúa después de `Cars`.
El DIE no conserva los valores de los argumentos muertos: se reutilizan las
constantes existentes y la estructura conocida, no se afirma haber recuperado
el texto original exacto de esos argumentos.

La restauración recupera la topología de bloques/inlines, **no un DWARF
exacto completo**. Su bloque muerto se ancla en +284, frente a +164 en el
original. Los rangos muertos no se usan para fingir exactitud del fuente.

La `.rodata` conserva 17.320 bytes y alineación 8. Sólo se reordena la
ventana `[11496,11580)`:

- `SpongeAllocation` pasa de 11521 a 11496.
- La constante de cuatro bytes `40200000` pasa de 11496 a 11516.
- `CARS\\%s\\TEXTURES.BIN` pasa de 11500 a 11520.
- Las cadenas ASM siguientes se desplazan tres bytes; tres NUL de padding
  pasan al final del nuevo literal. No se añade ni pierde contenido.

De las 11.581 relocaciones runtime, seis actualizan los destinos de la
constante y del nombre de archivo anteriores; se verifican por separado
sus tipos, posiciones y contenidos apuntados. Las otras 11.575 no cambian
tras normalizar la identidad de símbolos locales por sección/dirección.
`.text`, las demás secciones ALLOC y los símbolos públicos son idénticos.

### Declaración extern

El bloque original `0x443f719` contiene el DIE `0x443f731`:
`global_variable`, nombre `ChunkMovementOffset`, tipo `int`, sin location.
La lectura anterior de r36b había omitido este registro y lo reemplazado
por un `enum` para forzar el bloque. La declaración real produce el mismo
runtime completo; ya no hace falta ese sustituto artificial.

Persisten los otros andamios documentados en `DefragmentPool` (`params`,
`table`, `zero`) y los desacuerdos de registro; no se declaran resueltos.

### Funciones pendientes de zWorld

| Función | Bytes | Match |
| --- | ---: | ---: |
| SetMemoryPoolSize | 304 | 97,36842 % |
| DefragmentPool | 684 | 99,269005 % |
| UpdateWheelYRenderOffset | 876 | 98,92694 % |
| RenderFlaresOnCar | 2.908 | 98,05364 % |

Las otras 578 funciones de la unidad siguen exactas.

## eProject: una hipótesis nueva descartada

Se comprueban tipos originales y una hipótesis acotada: ligar la entrada
al resultado en el ASM hardware existente de `eRecip`, retirando a la vez
la barrera vacía de `eProject`. Se usan una base y un control sin barrera.

| Variante privada | Bytes | Match | Filas distintas |
| --- | ---: | ---: | ---: |
| Fuente actual | 268 | 93,970146 % | 14 |
| Sin barrera | 268 | 86,1194 % | 23 |
| Sin barrera + entrada ligada de fres | 268 | 86,1194 % | 23 |

Los dos controles sin barrera generan idéntico runtime. La restricción del
operando de `fres` no explica el desacuerdo. No se integra esta variante y
no se añade ASM. La barrera actual continúa pendiente de sustitución.

El DIE de matriz `0x153210a` remite a un array de cuatro floats
(`0x1350e8c`): no hay error de dimensión en el Mtx 3x4 actual. Los tipos de
`eRecip`, `x`, `val` y `recip` también se verifican como float.
La base aislada reproduce la función del objeto completo, y las otras 22
funciones del fichero, datos y destinos de relocación quedan intactos.
Detalle reproducible: `scratchpad/codex_20260914_eproject_next/report.md`.

## Enlace y keep: no confundir prueba privada con promoción

El grafo real sigue seleccionando el objeto extraído
`build/GOWE69/obj/Speed/Indep/SourceLists/zWorld.o`, no el objeto de comparación
recompilado. **No se toca `configure.py`, `splits.txt`, `keep.lst`, headers,
flags ni ningún otro objeto.** Tampoco se modifica HEAD ni el índice.

El ensayo privado sustituyendo sólo zWorld por su fuente usa un keep
condicionado y resuelto por contenido. No reproduce todavía el original:

- Base fuente: SHA1 `2494248b4abaf6e53f3944bae9cd055382342b4d`.
- Fuente integrada: SHA1 `9f1dda869dfca888b2eb9d92141ec7ce2cab0706`.
- Mismo tamaño; 85 bytes distintos entre ambas pruebas. La distancia bruta
  al original pasa de 17.623 a 17.622 bytes. Esto **no** cuenta como nuevo
  código match ni permite promocionar la unidad.

Para un futuro enlace desde fuente, consultar
`scratchpad/codex_20260914_world_next/combined/keep.source-only.lst` y
`runtime-proof.json`: se remapean las entradas existentes por contenido y
se retienen `LC1091` (SpongeAllocation) y `LC1107` (literal largo de Defrag).
Ambos tienen cero referencias runtime. **No copiar este keep sobre el
actual ni ejecutar una corrección automática de LC en producción.**
La receta fuente anterior del informe de Defrag queda superada por ésta.

## Gate y artefactos

`scratchpad/codex_20260914_world_integration/phase.py` crea un snapshot PRE
de 4.644 archivos, compila únicamente zWorld con sus flags reales, verifica
el runtime contra el candidato privado, enlaza privadamente los 619 inputs
reales y regenera el informe global. Sólo cambian `CarLoader.cpp` y su
objeto de comparación. Se conserva CRLF.

Estado POST autoritativo de esta tanda:
`scratchpad/codex_20260914_world_integration/post.json`.
Comprobación repetible: `python scratchpad/codex_20260914_world_integration/phase.py check`.
Los guards PRE de tandas anteriores son fotografías históricas y fallarán
legítimamente al comparar esta nueva fuente con la anterior.

Artefactos privados adicionales: `codex_20260914_world_next` (tres variantes,
base, DWARF y enlaces con inputs congelados), y `codex_20260914_eproject_next`
(tipos originales, tres compilaciones, auditoría del experimento negativo).

## Por dónde continuar

1. Investigar una diferencia concreta adicional en `DefragmentPool`: el
   constructor original de `ProfileNode` registra `section_name` y `categories`,
   mientras que la fuente actual llama al constructor sin argumentos. Primero
   hay que identificar los argumentos genuinos; no inventar nombres ni asumir
   que `CarLoaderDefragment` le pertenece (ya se usa en otra asignación).
   Esta diferencia de DWARF no demuestra por sí sola una mejora de código.
2. Seguir retirando las cadenas ASM restantes de CarLoader sólo cuando se
   encuentre su productor C++ real; `QUEUED`, `LOADING`, `LOADED` y
   `CARLOADER: %d` siguen en el bloque mixto, no se han dado por resueltas.
3. Para las funciones pendientes, usar los árboles DWARF completos
   (incluidos global_variable y bloques optimizados) antes de nuevas pruebas.
   No repetir restricciones de fres, permutaciones de registros ni enum
   artificiales descartados en las tandas anteriores.
4. Promocionar zWorld únicamente cuando estén cerradas sus cuatro funciones
   y su layout de datos, con un enlace desde fuente idéntico al original.
