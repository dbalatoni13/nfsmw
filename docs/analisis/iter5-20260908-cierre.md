# Iter5, 2026-09-08: cierre y punto de continuacion

## Estado real integrado

| medida | antes | despues |
|---|---:|---:|
| codigo matched | 3902288 / 3946048 B | 3902288 / 3946048 B |
| porcentaje matched | 98,891045 % | 98,891045 % |
| funciones exactas | 18387 / 18432 | 18387 / 18432 |
| codigo linked | 768876 B | 771180 B |
| porcentaje linked | 19,484709 % | 19,543098 % |
| datos matched | 443865 B | 444041 B |
| datos linked | 355457 B | 355633 B |
| unidades completas | 488 / 616 | 491 / 619 |

Quedan **43760 B / 45 entradas** del reporte. Una de esas entradas es el
acolchado .init de 20 B introducido por la conversion DOL, no una funcion de
juego; no borrarlo del inventario para simular decompilacion. El denominador
oficial no ha cambiado. El censo directo sigue dando 3902432/3946204 B y
18393 funciones exactas: diferencia historica de inventarios, no progreso.

## Promocion integrada

**VM: +2304 B linked, +176 B de datos matched/linked.** La unidad monolitica
se separa en vm.c, VMPageReplacement.c y VMMapping.c, siguiendo los tres
propietarios STT_FILE originales y sus funciones/estados. Son 6+4+8 funciones
que ya estaban al 100 %. Se recompilaron exclusivamente esos tres objetos,
con la misma version GC/1.2.5n y flags, y se enlazaron desde fuente.

Los tres autos vecinos se recortan, no desaparecen: conservan PRINTdevicelist
y estados RealInput. Se crea la cola auto_06_804528C8_data de 24 B. El grafo
real queda en 619 objetos, con los otros 612 objetos byte-identicos. No hubo
limpieza destructiva de autos viejos en disco.

La autorizacion general de promociones linked del usuario esta recibida.
No pedirla otra vez para una promocion acotada y completamente verificada.
No extiende el alcance a la retirada P6 de ENVIRO_AEMS ni a cambios ajenos.

Detalle y limitacion del ambito lexico de los estaticos:
[iter5-20260908-vm.md](iter5-20260908-vm.md).

## Trabajo paralelo no integrado

- **CookValues:** cuatro productores MW y dos pruebas coordinadas con
  registros fisicos sobre cook_lo de iter4. Ninguna cierra ni mejora la base
  privada 260 B/99,07692 %/9 filas. Colocar v en r6 rota bits/min y los
  argumentos; el minimo fisico empeora. Vecinos, datos y produccion intactos.
  [Detalle](iter5-20260908-cook.md).
- **CalcWait:** tres productores reales fdivs. Poner la division en el
  registro objetivo cambia la planificacion y obliga a preservar GPRs; las
  sombras crecen 336 -> 340 B y bajan a 63,02381/59,08333 %. No se retiene
  ninguna. [Detalle](iter5-20260908-libs.md).
- **SetupNextLoad:** hallazgo causal de CFG. Tras borrar una referencia debe
  volver a revisar el mismo SndBase antes de decrementar, para eliminar todas
  sus referencias. La sombra V1 pasa de 23 a 17 filas, 1008 B/99,66270 %;
  solo queda un ciclo de tres registros. Se analizaron sus prioridades RTL
  y dos aliases C: uno se elimina y otro rompe el CSE. No se integra el
  parcial ni se toca la cabecera compartida. El siguiente ensayo debe partir
  de V1 y una dependencia real, no repetir los viejos pines/CFG.
  [Detalle](iter5-20260908-setupload.md).

Los gates negativos se repitieron por coordinacion, tambien tras VM. No
presentar ninguna sombra como codigo integrado ni como funciones cerradas.

## Verificacion y baselines

Gates vigentes:

```text
python scratchpad/codex_20260908_iter5_vm/audit_post.py
python scratchpad/codex_20260908_iter5_vm/integration/audit_snapshots.py
```

Ambos PASS, ejecutados por coordinacion; el primero tambien por un agente
independiente. Verifican las 18 funciones y relocaciones contra el ELF
original, tres layouts/estados, fuentes y targets contra la prueba privada,
24 paths protegidos, 612 hashes de objetos y 532 specs/reglas previos sin
cambios (534 tras anadir las dos rutas). Configure/splits se comparan byte a
byte contra su baseline mas el cambio exacto; mantienen LF. Solo las tres
fuentes C se normalizan a CRLF. Una conversion accidental de EOL de los dos
ficheros de configuracion se corrigio, no se relajo el gate para aceptarla.

Las 612 unidades ajenas tienen el registro completo del reporte identico.
Las **15493 entradas de las 33 SourceLists** conservan exactamente porcentaje
y tamano, no solo los bytes de funciones al 100 %.

checksplits: **1106 rangos, 0 solapes, 0 cortes de simbolo**. lcfix limpio.
git diff --check limpio salvo avisos habituales de EOL; indice vacio, sin
commits. HEAD sigue 8853122edb6ebea56a6085dd88a4938a72a24be3.

DOL real: **4541888 B**, byte-identico al original, SHA-1:

**9619ba57c9919f95f7f2ac951a2166a3517f91e3**

Snapshots finales autoritativos:
`scratchpad/codex_20260908_iter5_vm/integration/{report,measure,pct}_after.json`.
`build/GOWE69/report.json` es identico a ese report_after.

Los gates iter4 y el gate privado PRE de VM conservan sus baselines inmutables:
ya son historicos respecto de configure/splits o del monolito. No actualizar
sus hashes para hacerlos pasar en el grafo nuevo. Los DOL control/trial y los
fragmentos privados revisados de VM siguen disponibles e inmutables.

El objetivo del 100 % sigue pendiente; este cierre no lo marca como logrado.

## Triage preliminar para la siguiente tanda linked

El agente de revision deja dos candidatos, aun SIN prueba de promocion ni
gate independiente de coordinacion. No contabilizarlos como cerrados:

1. `Packages/eathread/1.1.0/source/eathread_thread` (84 B): su fuente emite
   una funcion vacia adicional y 26756 B de BSS ausentes del target. Investigar
   en sombra las definiciones AllocateThreadDynamicData, gThreadDynamicData
   y gThreadDynamicDataAllocated. Antes de omitirlas, comprobar referencias
   de TODOS los objetos del grafo, no solo usos C. Preservar el ctor original.
2. `libc/libgcc2_4` (176 B): .rodata fuente de 24 B frente a pool externo
   original lbl_8040FE28/30/38. Probar, como maximo, una forma causal de usar
   esos tres valores originales sin duplicar almacenamiento. Dos constantes
   nacen del lowering interno de conversion; puede no ser viable sin cambiar
   instrucciones. Reabsorber ese rango en splits ya fallo por orden de enlace.

No iniciar nuevos barridos de sserver/ssysinit (orden BSS ya medido en 18
formas) ni promociones de racimos de pools/vtables sin prueba conjunta.
Los agentes han terminado; no quedan compilaciones privadas en ejecucion.
