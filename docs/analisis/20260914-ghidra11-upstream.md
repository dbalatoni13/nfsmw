# Ghidra 11.4 completo y revisión upstream — 14/09/2026

## Resultado

Se ha generado una **exportación nueva de las 18.442 funciones reconocidas** por
el proyecto Gekko + DWARF1 de Ghidra 11.4. Todas tienen salida de pseudocódigo
tras reintentar una función que agotó el primer límite de tiempo. Esto no
significa 18.442 funciones reconstruidas en C/C++ compilable ni una prueba de
match. Los límites y advertencias se conservan en los índices.

Se ha revisado también el rango upstream completo y los dos commits indicados.
De esa revisión salen dos pruebas privadas positivas de limpieza de ASM:
**84 B de cadenas de FEngine con DOL exacto**, y **52 B de una cadena de
DefragmentPool con objeto equivalente, aún sin cierre de enlace**.

No se han integrado estos candidatos: el encargo de esta fase era generar y
verificar recursos. Fuentes, objetos de build, configuración, keep, splits,
reporte, proyectos originales y extensión instalada permanecen intactos.
No hay nuevos bytes de match o linked contabilizados.

## Exportación que se puede usar

Directorio: `scratchpad/codex_20260914_ghidra_export/`.

- **`README.md`**: entrada y enlaces por unidad.
- `all_functions.c`: pseudocódigo agregado; **no es fuente recompilable**.
- `by_unit/`: el mismo material agrupado por unidad del reporte.
- `effective_index.tsv` / `effective_audit.json`: resultado efectivo, incluido
  el reintento, con ruta exacta del pseudocódigo por dirección.
- `pending.c` / `pending.asm.txt`: las 25 funciones realmente pendientes.
- `out/functions/<prefijo>/<dirección>.{c,asm.txt,vars.tsv}`: primera pasada
  con pseudocódigo, bytes/instrucciones y parámetros/locales conocidos por Ghidra.
- `dwarf_original/`: metadatos originales que el importador no incorpora.

Se copió el proyecto compatible de
`C:/Users/jferr/AppData/Local/Temp/ghidra_r63both` a un directorio privado. La
exportación usa **11.4**, `PowerPC:BE:32:Gekko_Broadway`, GQR0=0 y los tipos y
firmas ya importados, con `-readOnly -noanalysis`. No se reimportó ni convirtió
el `ghidra_project` incompatible con 11.4. La copia y el original conservan
exactamente los mismos diez archivos y hashes después del trabajo.

La primera pasada empleó cuatro decompiladores paralelos y 60 segundos por
función. Tardó aproximadamente nueve minutos y terminó 18.441 pseudocódigos;
el inicializador zWorld de `802E9284`, 11.168 B, agotó ese límite. Un reintento
con límite de 600 segundos terminó en unos 62 segundos. Sus **2.792
instrucciones coinciden con las de la primera pasada ya auditadas contra el
original**. La prueba fallida se conserva en `out/`; la recuperada se incorpora
al agregado y al índice efectivo desde `recovered/802e9284.c`.

### Cobertura comprobada y límites

| Control | Resultado |
|---|---:|
| Funciones Ghidra procesadas | 18.442 |
| Pseudocódigos tras el reintento | 18.442 |
| Firmas IMPORTED / DEFAULT | 13.824 / 4.618 |
| Cuerpos iguales al rango del reporte | 18.372 |
| Pendientes reales completas | **25 / 25, 30.744 B** |
| Instrucciones comprobadas contra bytes del ELF original | **985.909** |
| Bytes de esas instrucciones | **3.943.636** |

El contador de instrucciones corresponde a lo listado, no a una afirmación de
que todo el código original esté representado una sola vez o tenga semántica C
correcta. La auditoría conserva:

- **39 entradas del reporte sin función Ghidra en su entrada exacta**, incluyendo
  padding y numerosos rangos de MetroTRK.
- **49 funciones adicionales de Ghidra** no presentes como entradas del reporte.
- **21 discrepancias de límites/cobertura**, mayoritariamente SDK, vectores y
  rutinas de registros. `DSIHandler` contiene `halt_baddata`: que el decompilador
  entregue texto no convierte ese cuerpo en una reconstrucción completa.
- Un error concreto de límites de juego: Ghidra atribuye 456 B a
  `IconSort::Compare` en `801B0DD8`, cuando el reporte/original separan sus
  16 B y los 440 B de `GatherVisibleIcons` en `801B0DE8`. Se ha identificado
  mediante inspección de solapamientos, **no se han corregido límites a ciegas**.

Las 25 pendientes nuevas se compararon por **dirección**, no por nombre, con
`r63-ghidra-27-decomp.c`: **25/25 producen el mismo pseudocódigo**. La novedad
es el corpus completo, los controles de cobertura y el suplemento DWARF; no se
anuncia un cierre por volver a generar el mismo texto.

## Información que la extensión está dejando fuera

Se leyeron las fuentes incluidas en el ZIP de la extensión instalada:

- `DWARF1VariableImporter` sólo acepta una ubicación formada por un único
  `OP_ADDR`. Omite las locales en registros y pila.
- `DWARF1FunctionImporter` toma los parámetros hijos directos y asigna storage
  dinámico; no recupera por ello el reparto original durante el cuerpo.
- `DWARF1ProgramAnalyzer` no procesa `TAG_inlined_subroutine`.
- `DWARF1ImportUtils.extractName` sólo consulta `AT_name`: no sigue
  `AT_abstract_origin` para encontrar el nombre heredado.

Esto se ha suplido **sin modificar ni reinstalar la extensión**. El lector
`dwarf_sidecars.py` recorre las CU originales, respeta los padres/siblings de los
DIE y exporta atributos, referencias, locales, bloques e inlines por dirección.
Hay datos para **22 de los 28 objetivos**: las 19 pendientes GCC, los dos
inicializadores de lbmpeg y `_IHandle`. Los seis cuerpos de steering no tienen
un DIE de subrutina vivo en esta lectura DWARF1; no se inventan sus locales.
Los árboles incluyen inlines y bloques de rango cero: no equivalen a llamadas
runtime ni autorizan resucitar código eliminado.

Dos ejemplos útiles:

1. `_IHandle` tiene DIE concreto `01C954E4`, `[8017CCF4,8017CD00)`, con
   `abstract_origin=01A9EBB8`; el origen aporta nombre `_IHandle`, atributo
   inline, clase y tipo de retorno. **Firma DEFAULT en Ghidra no significa que
   falte DWARF original.** Esto explica una limitación del importador, aunque no
   resuelve por sí solo la emisión fuera de línea de GCC ni sus consumidores.
2. `SetMemoryPoolSize` conserva en DWARF un bloque de rango cero en `802DD404`
   con local `n` y un segundo inline de bMalloc. Es evidencia del bucle
   optimizado que interesa para reconstruir la emisión de `SpongeAllocation`,
   y que no aparece como código ejecutado en el pseudocódigo.

Las ubicaciones `REG(n)` del suplemento son **registros DWARF del cuerpo**, no
una receta para cambiar los parámetros ABI de entrada de Ghidra. Los atributos
de pila y tipos se mantienen en bruto para evitar interpretar offsets o tipos
sin su contexto.

## Commits revisados

| Referencia | Alcance real | Utilidad comprobada |
|---|---|---|
| [917dc4c](https://github.com/dbalatoni13/nfsmw/commit/917dc4cf78012496e48970cf8c48df9a82d6f3b1) | 4 archivos, +80 líneas; devcontainer y documentación | Entorno de desarrollo; no trae cierres de funciones ni cambios ASM |
| [917dc…800816](https://github.com/dbalatoni13/nfsmw/compare/917dc4cf78012496e48970cf8c48df9a82d6f3b1...80081647102eebdde6da2b65462b85ea4e807393) | 31 commits, principalmente frontend; head `80081647102eebdde6da2b65462b85ea4e807393` | Instrumentación C de FEngine probada; no cierra directamente las 25 pendientes |
| [3bb4ab24 correcto](https://github.com/dbalatoni13/nfsmw/commit/3bb4ab24df54840a290310fe6d249bc13abe8235) | 21 archivos, 102 hunks del patch, +525/-549 | Cadena C de DefragmentPool probada; pista adicional de sponge |

El tercer enlace del usuario tenía una `a` final extra: 41 dígitos en vez de
40. Se verificó el identificador correcto siguiendo el padre de 917dc y luego
contra los objetos Git públicos. No se eligió otro commit por semejanza.

La API del compare truncaba su lista a 300 archivos. Se obtuvo el inventario
completo en un **bare privado**, sin fetch ni cambios de refs en el repositorio
principal: 522 archivos con detección de renombres, **523 rutas sin esa
heurística**, +85.120/-5.367 y +85.127/-5.374 respectivamente. Los commits
incluyen ramas con trabajo desde abril: no son 31 novedades escritas el día
del merge. Todos los hashes, fechas, rutas y categorías están en
`scratchpad/codex_20260914_upstream_audit/{commits.csv,files.csv,inventory.json}`.

Los informes detallados están en ese directorio y en
`scratchpad/codex_20260914_upstream_cleanup/report.md`. Entre los descartes:

- ScratchPtr unsigned + stores por rama ya está incorporado y limpio aquí.
- La función modificada por #128 es `SuspensionSimple::Tire::UpdateLoaded`, ya
  exacta; no la pendiente `SuspensionTraffic::Tire::UpdateLoaded` de 856 B.
- Las cuatro SourceLists frontend siguen NonMatching upstream. Sus títulos
  «solve most» y la ausencia de ASM no prueban match ni DOL exacto.
- No importar los arrays ICEData con dimensiones intercambiadas, cadenas
  `TODO` o `CarLoaderSponge` sin contrastarlos con el original.

## Dos pruebas privadas y orden de continuación

### 1. FEngine: candidato listo para integración acotada

En una copia privada de FEngine.cpp se incluyó Profiler.hpp original y se
restituyeron las dos variables ProfileNode y cuatro llamadas Begin documentadas
en upstream y DWARF. Se retiró el bloque de cinco cadenas ASM, **84 B**.
El resto de Render y los flags se conservan.

- **343/343 funciones exactas**, todas las secciones ALLOC idénticas y las
  **3.679 relocaciones runtime equivalentes**.
- Render conserva **464 B y 18 relocaciones**.
- Con keep intacto el linker elimina cuatro nuevas cadenas con símbolo: el
  DOL pierde 32 B finales por alineación. La causa está medida, no supuesta.
- Con exactamente cuatro retenciones por contenido en una **copia privada** de
  keep (`$LC130`, `$LC132`, `$LC133`, `$LC134`), sustituyendo sólo zFEng en el
  grafo real de 619 entradas, el **DOL es idéntico al original**, 4.541.888 B,
  SHA-1 `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

Root revalidó ALLOC, relocaciones, diff exacto de esas cuatro retenciones,
hashes de las 619 entradas y comparación binaria del DOL. Fuentes/candidato,
comandos y pruebas están en `scratchpad/codex_20260914_upstream_audit/profile/`.
Éste es el primer cierre a integrar en la siguiente fase: ya hay receta
completa. La unidad ya estaba source-linked, por lo que será limpieza de
fuente, no ganancia nueva de match o linked.

### 2. DefragmentPool: objeto equivalente, integración de datos aún pendiente

Una sola variante privada restaura el literal largo en el bMalloc real y
elimina su `.asciz` de **52 B**. Se conservan cinco secciones ALLOC y **11.581
destinos de relocaciones**, sin regresiones en las 582 funciones. Quedan las
mismas 578 exactas y DefragmentPool sigue **684 B / 99,269005 %**.

El nuevo `$LC1106` carece de referencias runtime y renumera seis entradas
usadas por keep. **zWorld sigue NonMatching y el DOL usa su objeto extraído**:
no se puede aplicar ese mapa al keep compartido sin coordinar el objeto al que
se aplica. No se hizo enlace ni se afirma DOL exacto para este candidato.
Informe y correspondencias: `scratchpad/codex_20260914_defrag_literal/report.md`.

Después: estudiar la emisión del bucle sponge con el suplemento DWARF y
retomar lbmpeg con su frontera de datos/BSS, sin rellenos inventados. La
prioridad sigue siendo ASM legítimamente sustituido por C/C++, match y luego
linked; cada promoción necesita además la prueba del DOL.

## Estado y reproducción

Producción conservada: **99,22038 % matched**, **32,04168 % linked**,
18.406/18.432 funciones exactas, 525/619 unidades completas. No git add,
commit, merge, cherry-pick, instalación ni actualización de Ghidra.

Los scripts nuevos sólo generan artefactos privados. El gate de esta fase
incluye los controles de la fase anterior y comprueba los proyectos originales,
la copia Ghidra y la prueba privada FEngine:

```text
python scratchpad/codex_20260914_ghidra_export/verify_export.py
python scratchpad/codex_20260914_ghidra_export/package_retry.py
python scratchpad/codex_20260914_ghidra_export/dwarf_sidecars.py
python scratchpad/codex_20260914_ghidra_export/final_gate.py
```

El primer comando reconstruye el paquete desde la pasada inicial; el segundo
superpone el reintento validado sin sobrescribir esa evidencia inicial. No
renovar snapshots ni volver a ejecutar scripts de compilación de una fase
anterior sobre directorios existentes para eludir sus comprobaciones.
