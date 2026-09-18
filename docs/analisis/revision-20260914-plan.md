# Revisión del 14 de septiembre: C/C++ real, match y después linked

Continuación posterior: [segunda tanda ASM](20260914-tanda2-asm.md). Ese cierre
añade la limpieza de la referencia C de VP6 y establece el gate POST vigente;
esta revisión conserva sus cifras y pruebas como historial de la fase anterior.

## Prioridad acordada

El encargo actual cambia el orden: retirar los parches `asm()` mediante fuente
C/C++ justificada, alcanzar el 100 % de match y después completar el enlace desde
fuente. No se considera una limpieza sustituir ASM por opcodes en un array, otro
alias, un pin, un bloque de datos copiado o código muerto inventado para forzar el
compilador. Tampoco se quitarán barreras a costa de romper funciones ya exactas.

Hay que distinguir los parches del proyecto del ensamblador original del SDK,
arranque, ABI, registros especiales, caché y paired-single de Gekko. Estos últimos
requieren revisión específica: C estándar no expresa todas esas operaciones. No
son funciones de juego que se puedan reemplazar por un cuerpo C arbitrario.

## Estado comprobado, no heredado del historial

Base de esta revisión: HEAD `5691c2d4`, con los cambios locales de r68 preservados.
No se ha hecho `git add` ni commit. El reporte se ha regenerado desde los objetos
actuales usando los mismos argumentos del edge `report` de build.ninja.

| Medida | Estado final: integración r68, mapeo corregido y retirada del fantasma |
|---|---:|
| Código matched | 3.915.284 / 3.946.048 B, **99,22038 %** |
| Funciones exactas del reporte | 18.406 / 18.432 |
| Código linked | 1.264.380 B, **32,04168 %** |
| Unidades marcadas completas | 525 / 619 |
| Entradas pendientes | 26, con 30.764 B |
| Pendientes de código real | 25 funciones, 30.744 B |
| Relleno `.init` aparte | 20 B |

El porcentaje matched incluye funciones que aún usan andamios ASM. **No equivale
a porcentaje de C/C++ limpio.** El porcentaje de datos de objdiff tampoco prueba
identidad del contenido: nombres de pools, estáticos numerados y rellenos afectan
su emparejamiento.

Los 20 B de padding no se borran del inventario para simular un cierre. De las
unidades pendientes, 18 son SourceLists y dos bibliotecas (`steering`, `madidct`);
además hay 73 rangos `auto_*` y `prodg_fixes`. Completar el enlace exigirá resolver
la propiedad de los rangos automáticos, no simplemente cambiar todas las marcas
a Matching.

Dentro de las 83 unidades con código perfecto pero no completas hay 72 rangos
automáticos, `prodg_fixes` y sólo diez unidades fuente: zAI, zEAXSound, zFe, zFe2,
zGameplay, zLua, zMisc, zPhysics, zPlatform y zSpeech. Es un inventario para la
fase linked posterior, no autorización para promoverlas sin validar.

## Qué se ha avanzado

- Desde el cierre Iter6 del 08/09: 3.903.296 -> 3.915.284 B matched, 18.388 ->
  18.406 funciones exactas y 771.264 -> 1.264.380 B linked. La diferencia incluye
  la corrección de medición de 16 B descrita abajo; no son todos bytes de código
  escritos de nuevo. Unidades completas: 492 -> 525.
- Las rondas intermedias corrigieron propiedad y orden de emisión de inlines,
  plantillas, vtables y pools. No basta con que cada cuerpo aislado case: el
  enlazador puede elegir otro dueño o colocar un dato en otro sitio.
- r65-r67b se centraron en quitar andamios. La serie homogénea corregida publicada
  por r67b es **238 -> 209 -> 196 -> 176 -> 175**. Las cifras anteriores 362/190/166
  no son directamente comparables: algunos censos omitían `asm("")` y formas de
  declaración. El censo nuevo debe explicar su alcance y sus exclusiones.
- r68 clasificó 502 bloques de datos, proponiendo 131 inicialmente abordables.
  Las refutaciones posteriores importan: keep.lst afecta también a los objetos
  extraídos; el DOL no identifica por sí solo el nombre de un dato a cero; las
  direcciones DWARF de estáticos eliminados pueden estar caducadas.

Referencias de síntesis: `iter6-20260908-cierre.md`, `r64-cierre.md`,
`r66-cierre.md`, `r67-cierre.md`, `r67b-cierre.md`, `r68-plan-datos.md` y
`r68-refuta-1.md` / `r68-refuta-2.md`. Las refutaciones prevalecen sobre las
hipótesis anteriores, no se repiten barridos ya refutados sin evidencia nueva.

## Trabajo realizado en esta revisión

### La limpieza local de r68 estaba sin integrar en el enlace

Al empezar, el DOL real y un relink independiente dieron
`5ce7699bb80669427d43058bbc3483bd252fb04f`, 4.541.696 B: **no era el original**.
La `.rodata` estaba 160 B corta y la `.data` 32 B corta, desplazando referencias.

Se recompilaron en privado 22 unidades afectadas, incluidos los tres consumidores
conocidos de `CarCustomize.hpp`: zAnim, zFe2 y zFeOverlay. Todas reproducen las
secciones runtime, la tabla de símbolos y las relocaciones del objeto de build.
19 objetos son idénticos enteros; zWorld, zWorld2 y madidct sólo difieren en
depuración. Por tanto, el fallo no era una compilación atrasada de esas fuentes.

Faltaba integrar las recetas de keep correspondientes a las sustituciones ASM
por C/C++ que ya estaban en el árbol:

- zBWare: variables y punteros a función recuperados, sin retener el array muerto
  `bSinTable` entero; se conserva el ASM aún no identificado de FancyStomp.
- zAttribSys: `Attrib::gDefaultExportID`.
- zFeOverlay: `gInstallCarPartID`, `bIsCross`, `gCarTypeNameHash`; retirar la regla
  del padding que ahora genera el enlace.
- zAnim: cuatro literales de las llamadas reales y cinco renumeraciones por
  contenido. No se reconstruye el pool como un bloque de bytes.
- zSim: cuatro literales ya emitidos por las cabeceras y llamadas reales.

Se aplicó únicamente ese paquete acotado a keep.lst, conservando CRLF, después de
dos enlaces privados: con los objetos de build y con los recién compilados.
Ambos y el enlace real final producen **4.541.888 B exactos**, SHA-1
**9619ba57c9919f95f7f2ac951a2166a3517f91e3**. No se reintrodujo ASM ni se modificó
ninguna fuente para reparar el DOL. Esto completa una integración pendiente,
no es una promoción nueva ni un aumento de código linked.

**Límite de fidelidad conservado, no ocultado:** el cambio local de `bSinTable`
declara correctamente un array de 513 elementos, pero sólo su último elemento
sobrevive en el DOL. Los 512 anteriores se inicializan a cero en la reconstrucción
y son desconocidos en el original; la propia fuente lo advierte. El enlace los
elimina. La prueba certifica el ejecutable, no recupera esos datos perdidos ni
convierte ese inicializador provisional en fuente original demostrada. No he
reescrito ese cambio local. Es un punto pendiente de investigación con otros
binarios/oráculos, separado de los bytes ejecutables que cuentan para match.

### `Compare` era un falso pendiente

El comparador local de GManager conserva los mismos 16 B:
`80030004806400047c6300504e800020`. Target y fuente son LOCAL/STT_FUNC, están en
`.text+93036` y no tienen relocaciones en el cuerpo. Sólo cambió el contador del
nombre fuente: `.25190` -> `.25167`; el nombre target sigue en `.35326`.

Se corrigió el RHS del mapeo en configure.py y objdiff.json, sin ejecutar
configure.py ni modificar flags/splits/grafo. El reporte gana **16 B / una
función por medición**, no por recompilar código. Antes de esa corrección eran
27 entradas, 99,21998 %. Ahora zGameplay no tiene funciones pendientes.

### Censo nuevo y prueba de barrera

El censo léxico revisa 3.679 fuentes/cabeceras, no el código preprocesado. En
proyecto/terceros registra al cierre 460 bloques de datos, 8 alias/secciones,
124 barreras, 86 pines, 156 construcciones con instrucciones y 499 nombres de enlace. No son
«156 parches GC»: incluyen código de hardware y original; SDK y PS2 se clasifican
aparte. El detalle auditable está en
`scratchpad/codex_20260914_asm_instructions/report.md` y `census.json`.

Con el mismo parser en HEAD y las 33 fuentes modificadas, la limpieza local r68
retira 25 bloques de datos, 7 alias y 5 nombres de enlace. Al empezar había además
2 barreras y 1 pin nuevos en `IdctRow`, retirados en esta revisión. Es una
comparación homogénea, sin atribuir autoría de los cambios que ya estaban en el
árbol. La foto inicial tenía 213 andamios léxicos y la final 210. Los 35 de
`vfprintf` explican numéricamente la diferencia final con los 175 históricos;
no prueba por sí sola que el alcance histórico sea idéntico. Las dos fotos se
conservan en JSON, con sufijo `before_cleanup` para la inicial.

La prueba privada `csis`/`slinklist.h` queda cerrada como **negativa**: cuatro
formas C naturales producen el mismo cuerpo de 232 B / 99,01724 %, distinto del
cuerpo exacto por selección de registro y base de acceso. No se tocaron cabecera,
producción ni sus consumidores. Véase `20260914-asm-candidate.md`; no justifica
otro barrido cosmético de declaraciones ni demuestra que toda solución C sea
imposible.

### Retirada real de tres andamios en `IdctRow`

Se eliminó únicamente `register int g asm("r5")` y las dos barreras vacías que
definían/consumían ese fantasma. La matemática C y los datos se conservan; el
diagnóstico histórico r64 también, corregido para no indicar que el fantasma
sigue retenido. No se sustituye por otro pin, ASM ni operación artificial.

Prueba privada y recompilación real de **sólo madidct**: `IdctRow` pasa de
516 B / 44,36434 % a 496 B / 46,372093 %. Sigue pendiente: el objetivo mide
516 B. `IdctColumn` conserva 632 B / 50,449368 %; `idctcompute` sigue exacta
(356 B), con auditoría de sus 63 relocaciones contra el ELF original. La
compilación real reproduce todas las secciones runtime/símbolos/relocs de la
prueba privada. El gate verifica que los tokens C sólo cambian por las tres
construcciones retiradas.

Se regeneró el reporte: **cero pérdida o ganancia de bytes/funciones exactas**,
cero cambio de datos y linked. Sólo cambia la puntuación fuzzy de `IdctRow`.
madidct todavía usa su objeto extraído en el grafo de enlace. El relink final
mantiene el DOL original; esa prueba no se presenta como promoción de madidct.

## Continuación, por orden

1. **Censo de ASM con alcance explícito.** Separar instrucciones, datos/padding,
   barreras vacías, pines y nombres de enlace; distinguir SDK/PS2 de parches GC.
   Un grep bruto de `asm` mezcla categorías y cuenta comentarios.
2. **Datos con identidad demostrada.** Continuar las recetas C de r68 a partir de
   esta base ya coherente, con DWARF + ELF/DOL + referencias/orden independientes.
   No trasladar automáticamente un keep de un objeto fuente pendiente al objeto
   extraído que usa hoy el enlace. Probar cada paquete de compensación completo.
3. **Barreras y pines desde su causa.** Revisar tipos, alias, firmas, inlines,
   controles y compilador; no otro barrido de números de registro. Las cuatro
   formas medidas en `csis`/`slinklist.h` quedan descartadas. Dos hipótesis aún
   sin medir: inicialización C++ de `MPEGuse_MMX` en `lbmpeg.cpp` (84 B) y emisión
   fuera de línea del método real `_IHandle` de `IGenericMessage` (12 B). No son
   cierres prometidos; el segundo requiere auditar consumidores del header.
4. **Las 25 funciones de código.** Priorizar una hipótesis causal nueva por
   función, usando el primer desacuerdo real y el DWARF; validar también todas
   las vecinas. No confundir una mejor puntuación fuzzy con un cierre exacto.
5. **Linked después.** Con código y limpieza asentados, abordar orden de emisión,
   símbolos, pools, datos y propiedad de rangos. zLua era una candidata cercana
   según r67; hay que revalidarla sobre la base nueva. Ninguna marca Matching sin
   enlace privado y DOL original.

El inventario de funciones, trayectoria entre rondas y prioridades por unidad
está desarrollado en `revision-20260914-match-plan.md`. El primer candidato de
match es la inicialización estática de zCamera (3.604 B, seis filas distintas),
si aparece una hipótesis de ámbito/lifetime respaldada por evidencia; no más
pines ni valores ficticios. madidct, steering y los otros cuerpos resistentes
necesitan procedencia, tipos/ABI o evidencia del compilador, no repetir barridos
ya negativos. Estas son líneas de investigación, no una garantía de cierre.

## Gate y artefactos de esta integración

La primera fase pasó `scratchpad/codex_20260914_revision/final_gate.py` antes de
retirar el fantasma; su salida se conserva en `integration_final_gate.log`:

- 4.562 rutas protegidas, con los únicos cambios autorizados de keep, mapeo y
  derivados; fuentes locales intactas, 542 especificaciones de compilación y
  619 entradas de enlace iguales;
- 22 recompilaciones privadas y todos sus bytes runtime/símbolos/relocs;
- correspondencia exacta de los dos símbolos de `Compare`, cero relocaciones;
- ningún cambio de reporte fuera de `Compare`/sus medidas agregadas;
- DOL original real y dos enlaces privados originales;
- `checksplits`: 1.118 rangos, cero solapes y cortes;
- `lcfix --check`: cero CORRIGE y 53 avisos históricos de literal sin símbolo
  propio; ninguno es un fallo de `@lcsrc`;
- `keepchk`: 50 -> 49 referencias inexistentes, todas las restantes históricas,
  ninguna entrada añadida sin resolver. No se presenta como un chequeo sin avisos.

Snapshots anteriores no se reescriben. `report.json.before` y
`report_verified.json` conservan la medición anterior al arreglo del nombre;
`report_after.json` conserva la medición corregida anterior a retirar el fantasma.
`report_after_cleanup.json` y el reporte live conservan la medición final. El
gate POST vigente es **`cleanup_gate.py`**, con los argumentos de fuente y objeto
privados de `scratchpad/codex_20260914_asm_candidate/madidct_remove_ghost/`:
`madidct.cpp` y `no_ghost.o`. Compara contra un segundo snapshot inmutable de las
4.562 rutas, tomado después del PASS de la primera fase. Sólo permite cambiar
madidct.cpp, su objeto compilado y report.json, y comprueba tokens, secciones,
símbolos/relocs, medidas exactas, grafo y DOL. La auditoría específica de la
función vecina comprueba además las 63 relocaciones. El gate anterior queda como
prueba histórica de su fase y no debe ejecutarse como si madidct no hubiera
cambiado.

El objetivo de limpiar los parches y llegar a ambos 100 % **sigue pendiente**.
