# r43 - cuatro frentes, restauracion completa y nuevas causas acotadas

2026-09-08. Base `590dc4a0` mas los cambios locales verificados al cerrar r42.
Se trabajo con tres agentes en paralelo y coordinacion. El turno anterior se
clasifica como progreso: integro los 780 B exactos de r41 y, en r42, dejo la
receta estructural de RoadNoise. **En r43 no hay nuevos bytes matched ni cambios retenidos.**
Los quince ensayos de esta tanda aportan evidencia para descartar formas
concretas; no convierten mejoras parciales en funciones exactas.

## Estado oficial, sin variacion

| medida | resultado |
|---|---:|
| codigo exacto | **3.893.124 / 3.946.048 B, 98,65881 %** |
| funciones exactas | **18.373 / 18.432** |
| codigo linked | **736.012 B, 18,651876 %** |
| datos exactos | 443.749 / 1.285.741 B |
| datos linked | 319.036 B |
| unidades completas | 478 / 618 |

Pendiente: **52.924 B / 59 entradas / 27 unidades**, incluidos los 20 B de
padding automatico .init. El objetivo del 100 % sigue incompleto y activo.

El censo directo independiente conserva 3.893.268 / 3.946.204 B y
18.379 / 18.439 funciones, en 618 unidades. Su diferencia historica respecto
al informe oficial no se cuenta como progreso.

## Evidencia nueva y ensayos retirados

### Sonido: el cero no pertenece a refcnt exterior

Sobre Wet_FX=-1, las formas locales de declaracion/inicializacion de refcnt
mantienen 392 B / 99,64286 % y los siete operandos r27/r29. Usar directamente
el resultado de la asignacion elimina una recarga requerida y baja a
380 B / 92,05102 %, con un marco menor: no es el codigo objetivo.

El lmap y DWARF sitúan el store final de cero en la local INTERNA de
GetRefCount, cuyo resultado externo no tiene registro y muere antes de RTL.
Se probaron en sombra tanto declaracion+asignacion internas como escritura
volatile localizada; ambas generan el mismo objeto. La ranura ya escapaba por
direccion a una llamada opaca y esa escritura ya estaba materializada.
Las cinco formas estan medidas, sin repetir sentinelas ni el pin rechazado
de r42. Produccion nunca se toco. [r43-sound.md](r43-sound.md).

### Animacion: separar base y desplazamiento mejora, sin cierre

En EvalState, `GetKeyData(0) + index * GetKeySize()` reproduce mejor la frontera
de inline que indica el DWARF: **456 B / 93,50877 %, 52 diferencias ->
456 B / 97,76316 %, 21 diferencias**. El prologo se alinea, pero persisten
copias/base/temporales distintos. No se retiene.

Nombrar keySize o keyOffset empeora a 436 B / 88,24561 % y
448 B / 87,10526 %. El wrapper inline privado con un formal idx no usado
es identico a la primera receta. En FindMatchTime, adelantar la inicializacion
de n en su declaracion es neutro: 720 B / 97,361115 %.
Las cinco pruebas se retiraron. [r43-anim.md](r43-anim.md).

### Renderizado: nombrar el cero no reproduce solo su mitad alta

Tres pruebas sobre CarRenderInfo: puntero para cinco usos de cero en flares,
puntero limitado a width en ruedas y referencia para las tres cargas que
comparten @ha en el objetivo. Resultados respectivos: 2924 B / 97,31087 %,
880 B / 94,79452 % y 2944 B / 94,801926 %. Todas empeoran y se retiran.
Las referencias conservan una direccion o un valor completo, no el pseudo
high aislado que necesita el objetivo. [r43-world.md](r43-world.md).

### sfir: la extraccion inline pendiente ya esta medida

Extraer individualmente los cases 2 y 4 a helpers static inline mantiene
940 B frente a 936 objetivo y baja de 89,25214 % a 88,11966 % y 87,73505 %.
En ambos casos solo se emite la funcion principal: no falla la expansion
inline, sino que la nueva forma no reproduce las vidas de sus temporales.
Ambas retiradas. [r43-sfir.md](r43-sfir.md).

## Verificacion final

- `codex_r43_global_audit.py`: 18.432 entradas oficiales con mismas direcciones
  y tamanos, cero regresiones y delta **0 B / 0 funciones**. Datos, linked
  y denominadores identicos.
- Snapshot de las 33 SourceLists: las 15.493 entradas coinciden en porcentaje
  y tamano con r42.
- Animacion: JSON completo, objeto y dos fuentes restaurados al byte;
  314 exactas auditadas sin fallos.
- Sonido: fuente, header y objeto de produccion mantienen sus hashes;
  926/930 comparables exactas. MsgPlayMiscSound y MsgBarrier pasan el auditor
  ELF. Gates de los cinco ensayos sombra y sus datos: PASS.
- World: fuente CarRender.cpp byte-identica, CRLF puro, 577/582 exactas.
  Coordinacion genero ademas un JSON COMPLETO del objeto restaurado:
  ambos lados coinciden exactamente con el ultimo snapshot completo validado
  de r40, incluidos codigo, datos, simbolos y relocaciones. Gate:
  `codex_r43_world_full_audit.py`.
- sfir: fuente de 4.598 B / 109 CRLF, objeto entero y JSON completo restaurados
  byte a byte. `codex_r43_sfir/audit.py`: PASS.
- Se volvio a enlazar el DOL principal con los objetos de la lista generada,
  sin ninja completo. `dtk shasum`: OK,
  **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. Solo advertencias duplicadas
  ya conocidas.
- Runtime r39: nueve funciones, particion GCC100 + MW244, datos y hash: PASS.
- `checksplits.py`: 1.090 rangos, cero solapes y cero cortes de simbolos.
- `lcfix.py --check` y `git diff --check`: limpios.

No hay cambios nuevos de produccion, headers reales, flags, configuracion,
splits, staging ni commits. Todos los agentes liberaron sus territorios;
no quedan compilaciones ni ensayos activos.

## Proximas pruebas que todavia no estan medidas

Estas son hipotesis, no ganancias ni autorizaciones para headers compartidos:

1. En la receta RoadNoise de 392 B, contrastar en SOMBRA el pin anterior
   de mData con una restriccion **solo de entrada**. El ensayo r42 uso +r,
   que invalida para el compilador la relacion conocida data=this+4; no se
   midio el control que la conserva. Un unico control, con InitSFX como
   testigo obligatorio; rechazar cualquier regresion. No repetir las cinco
   formas de refCount de r43.
2. En EvalState, el wrapper con idx no usado ya esta medido. Si se retoma la
   frontera de GetKeyData, implementar directamente su seleccion de base
   con accesores publicos en un helper privado, no otro wrapper de
   GetKeyData(0). Primero confirmar contra el DWARF el alcance exacto; no
   volver a nombrar keySize ni keyOffset.

Instantaneas:
`scratchpad/codex_20260908_r43_{before,after}_{report,measure,pct}.json`.
Informe oficial vigente: `build/GOWE69/report.json`.
