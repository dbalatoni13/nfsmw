# Iter4, 2026-09-08: +416 B exactos y +11.272 B linked

Punto de partida: cierre iter3, HEAD `8853122e`. El usuario autorizo durante
esta tanda **«haz las promociones a linked cuando quieras»**. Se integraron
FSasync, vmbase y las tres TUs originales de OdemuExi2, comprobando el DOL real
despues de cada etapa. No hubo staging, commits ni ninja completo.

## Censo oficial final

| medida | entrada iter4 | salida |
|---|---:|---:|
| codigo exacto | 3.901.872 B | **3.902.288 / 3.946.048 B** |
| porcentaje exacto | 98,8805 % | **98,891045 %** |
| funciones exactas | 18.384 | **18.387 / 18.432** |
| codigo linked | 757.604 B | **768.876 B / 19,484709 %** |
| datos exactos | 443.781 B | **443.865 / 1.285.733 B** |
| datos linked | 355.348 B | **355.457 B** |
| unidades completas | 483 | **488 / 616** |

Delta de toda la tanda: **+416 B matched, +3 funciones, +11.272 B linked,
+84 B matched data y +109 B linked data**. Quedan **43.760 B en 45 entradas**,
incluido el padding automatico de 20 B. El objetivo del 100 % sigue pendiente.

El censo independiente pasa de 3.902.016 a **3.902.432 / 3.946.204 B** y de
18.390 a **18.393 funciones**. La discrepancia historica de inventario con el
informe oficial no cambia; ambos confirman +416 B/+3 funciones. Las 33
SourceLists y sus **15.493 entradas** permanecen semanticamente identicas.

## Promociones integradas

1. **FSasync: +2104 B linked.** Solo la entrada activa con
   `source="LibSN/FSasync.c"` y `-G0` cambia a Matching. Se conserva el codigo
   exacto y orden original de iter2: 13/13 funciones. DOL original tras
   regenerar manifiestos y sustituir un objeto, sin recompilarlo.
2. **vmbase: +3180 B linked.** 30/30 funciones, con siete externs/macros a
   aliases estables que ya exporta `auto_09_804FFED8_sbss.o`. Sus 28 B de
   almacenamiento no se mueven ni se duplican. Solo cambia el bloque
   declarativo y su nota; runtime y raw relocs coinciden con la prueba iter3.
   Se compila un unico objeto, se promociona y se vuelve a obtener el DOL.
3. **OdemuExi2: +5988 B linked y +416 B matched.** Se divide el monolito
   DebuggerDriver en los tres propietarios STT_FILE demostrados por el ELF:
   DebuggerDriver (GC/1.2.5n), AmcExi2Comm y AmcExi (GC/1.2.5).
   Quedan **14/14, 8/8 y 9/9 funciones exactas**. Solo se cuentan como nuevas
   `DBRead` (140 B), `DBQueryData` (156 B) y `DBInitComm` (120 B).

El ultimo corte reemplaza un bloque de splits por tres y absorbe dos objetos
automaticos: `auto_07_804BC0C8_bss.o` y `auto_08_804FF594_sdata.o`. La lista
real sigue teniendo 616 objetos. Los ficheros auto obsoletos pueden continuar
en disco: lo comprobado es que ya no aparecen en el grafo ni en config.json.
No se borraron archivos manualmente.

El `Ecb` de AmcExi mantiene binding LOCAL y 24 B en 0x804BC0C8; no es el `Ecb`
de EXIBios, LOCAL/192 B en 0x804BBD68. El puntero fuente `exi_804FF598` tiene
identidad exacta con `exi` local original: .sdata+0, 4 B, 12 SDA21 y destino
Ecb correcto. Los estados inicializados SendCount=0x80 y los dos punteros
locales reconstruyen los datos reales, no aliases globalizados.

Las tres TUs nuevas suman 109 B de datos contando el acolchado original:
32+45+32. Antes eran 73 B asignados al monolito y 24+12 B automaticos. Solo
25 B estaban exactos: de ahi +84 B exactos y +109 B linked sin cambiar el
denominador. No es un incremento de 109 B matched.

Detalles: [FSasync/vmbase](iter4-20260908-link.md),
[ownership y prueba original](resume-20260908-debugger.md),
[revalidacion e integracion DebuggerDriver](iter4-20260908-debugger-review.md).

Las fuentes Debugger se integraron byte a byte desde los fragmentos probados,
con CRLF. Conservan comentarios de experimentos anteriores, incluido el
UNSOLVED antiguo de EXI2_Poll: no describe el estado actual de sus 31 funciones.
Para el estado vigente usar este cierre y el informe regenerado.

## Trabajo paralelo no retenido

- **steering / Effect_Init:** tres productores reales MW alcanzan 95 % como
  maximo, sin cerrar la asignacion de registros ni la segunda limpieza.
  **CookValues:** cuatro variantes; la mejor, `cook_lo`, da 260 B/99,07692 %
  y solo queda el intercambio v/r4 frente a min/r6. Cambiar el orden de sus
  declaraciones no cambia ni un byte. Ninguna se integra. Los 35 vecinos
  se conservan contra el baseline privado en las siete pruebas.
  [Detalle](iter4-20260908-steering.md).
- **Stich:** tres productores de direccion real `la` despues de inicializar
  samplereq. El constructor mantiene el productor previo y su preferencia
  residual de registro. Ninguna mejora el baseline, ninguna se retiene.
  [Detalle](iter4-20260908-stich.md).
- **eProject:** tres productores FP reales: 92,58 %, 92,84 % y 86,82 %,
  todos 268 B. Se conserva el baseline 93,970146 % y el resto de la TU.
  [Detalle](iter4-20260908-eproject.md).
- **spchsamp:** cuatro variantes de aritmetica/almacenamiento real. Ninguna
  cierra; el vecino de 48 B, los datos y la fuente de produccion no cambian.
  [Detalle](iter4-20260908-spchsamp.md).

No presentar estas mejoras parciales privadas como codigo ya decompilado.
Los agentes liberaron produccion; no hay compilaciones privadas pendientes.

## Gates vigentes y baselines historicos

El checkpoint FSasync/vmbase queda guardado inmutable en
`scratchpad/codex_20260908_iter4_link`, con ambos DOL originales. Su
`audit_final.py` se repitio antes de DebuggerDriver; ahora sus asserts sobre
configure/splits son historicos. No alterar hashes para volverlo a hacer pasar.

Para el estado actual:

```text
python scratchpad/codex_20260908_iter4_debugger_link/audit_scope.py
python scratchpad/codex_20260908_iter4_debugger_review/audit_post_integration.py
```

El primero comprueba el cambio exacto de configure, preservacion de splits
fuera del bloque, **529 especificaciones/reglas de compilacion ajenas** y
las tres versiones/flags correctas. Los **613 objetos ajenos** mantienen
orden, pertenencia y SHA-256. Los tres targets extraidos son byte-identicos
a los de la prueba; runtime/datos/raw relocs de las tres fuentes coinciden
con los objetos privados probados. Los 613 registros completos de unidades
ajenas en el informe son identicos, no solo sus porcentajes.

El segundo audita 31/31 funciones y relocaciones contra el ELF original,
bindings locales, splits exactos, grafo vivo, vecinos globales por simbolo y
DOL. Ambos gates se ejecutaron por coordinacion; otro agente reviso la
integracion de forma independiente. Tambien se repitieron los gates negativos
de steering, eProject y spchsamp tras la reextraccion, con hashes intactos.

`checksplits.py`: 1098 rangos, 0 solapes y 0 cortes de simbolo.
`lcfix.py --check` limpio. `git diff --check` limpio salvo avisos habituales
LF/CRLF; indice git vacio. El enlace real solo conserva el aviso historico
PPCMtdec duplicado. DOL real y copia final: 4.541.888 B y SHA-1 original:

**9619ba57c9919f95f7f2ac951a2166a3517f91e3**

Snapshot final autoritativo:
`scratchpad/codex_20260908_iter4_debugger_link/{report,measure,pct}_after.json`.
`build/GOWE69/report.json` contiene ese reporte final. Los ficheros llamados
`codex_20260908_iter4_after_*.json` son el checkpoint anterior a DebuggerDriver,
no el estado final de la tanda. Se guardan para reproducir ambos incrementos.

## Continuacion

No volver a pedir permiso para promociones linked verificadas: la autorizacion
general esta recibida. vm sigue NonMatching, aunque sus 18 funciones son
exactas; falta demostrar una promocion completa con su estado/aliases y DOL.
CookValues tiene una nueva base causal privada acotada, no una razon para
repetir barridos ciegos de versiones/declaraciones. Stich requiere resolver
el productor real y su interferencia, no mover una barrera vacia.

La retirada de P6 de ENVIRO_AEMS sigue siendo un cambio distinto de cabecera
compartida y de una regresion local previa del usuario; no se ha aplicado bajo
la autorizacion de promociones. Preservar tambien Wet=-1 y todos los cambios
locales ajenos. El siguiente punto de partida es **98,891045 % exacto y
19,484709 % linked**, con este gate y la lista de 45 pendientes actualizada.
