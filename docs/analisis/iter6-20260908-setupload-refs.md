# Iter6 extensión — cierre privado de SetupNextLoad

## Estado actualizado: integrado

El usuario autorizó el cambio de cabecera y coordinación integró la receta
en las dos rutas reales, junto con un comentario actualizado de cinco líneas.
Ambas SourceLists fueron recompiladas y auditadas: **+1008 B matched y una
función exacta**, sin otros cambios de código/datos. Se revisaron los 997 y
1112 STT_FUNC definidos; sólo cambia SetupNextLoad, y zEAXSound2 conserva
incluso el hash del objeto completo. Los 1008 bytes del objeto real se
reconstruyen con sus 43 relocs y coinciden con el ELF original.

Gate vigente:
`scratchpad/codex_20260908_iter6_setupload_integration/audit_final.py`.
Ver `iter6-20260908-setupload-integration-audit.md` e
`iter6-20260908-cierre.md`. Las secciones siguientes conservan la historia
de la prueba privada: sus menciones de autorización pendiente y de fuente
intacta describen ese punto PRE, no el estado final. No ejecutar sus gates
de inmutabilidad como si fueran POST ni sobrescribir sus baselines.

La sustitución dirigida cierra `EAXAemsManager::SetupNextLoad`: **1008 B,
100 % y cero filas**. Sólo se compiló una forma nueva; no se modificó ni
recompiló producción.

La forma parte de la V1 de Iter5 (reinicio correcto de la segunda pasada de
`DeleteRefToAsset`) y mantiene cuatro entradas reales de `this`, pero reemplaza
las dos dependencias que perturbaban el código:

- dos `asm` vacíos separados y encadenados sobre `endIndex`;
- dos `asm` vacíos separados y encadenados sobre `bankIndex`;
- ninguna dependencia sobre `nextBankIndex` ni `currentLoad`.

Los dos valores son inicializados por el programa y consumidos después. Cada
`asm` expresa una dependencia sobre ese mismo valor y recibe `this` como
entrada; no hay datos indefinidos, operandos fantasma ni instrucciones PPC.

El RTL confirma exactamente la predicción:

- pseudo 82 (`this`): 16 referencias/245 instrucciones, asignado a `r29`;
- pseudo 370 (`gFastMem` de la segunda pasada): `r28`;
- pseudo 427 (sucesor): `r28`;
- pseudo 428 (desplazamiento): `r27`.

A diferencia de D4, esta colocación no intercambia `r9/r11` al formar
`g_SndAssetList` y no retrasa `li r26,0` ni `li r5,0x80`.

El gate reproducible
`scratchpad/codex_20260908_iter6_setupload_refs/audit.py` comprueba 773
funciones: `SetupNextLoad` es la única que cambia, quedan 772 exactas y sólo
`cStichWrapper::Play` continúa pendiente. El cuerpo exacto tiene 252
instrucciones, 38 ramas y 12 reubicaciones externas coincidentes por posición,
tipo, símbolo y addend; no usa literales de pool. Datos/BSS no cambian,
`lcfix.py --check` pasa y los hashes de fuente, cabecera y objeto de producción
siguen intactos.

La igualdad de secciones runtime se comprueba además contra `b0`, un control V1
compilado bajo una ruta privada de la misma longitud. Esto separa las cadenas
`__FILE__` propias del arnés (que explican la diferencia de `.rodata` frente al
objeto de producción) de cualquier cambio real de datos: `.rodata`, `.data`,
`.bss`, `.sdata`, `.sdata2` y `.sbss` son idénticas entre `b0` y la receta.

Receta privada entregable:

- `scratchpad/codex_20260908_iter6_setupload_refs/r1/EAXAemsManager.cpp`;
- `scratchpad/codex_20260908_iter6_setupload_refs/r1/Speed/Indep/Src/EAXSound/EAXAemsManager.h`;
- objeto y RTL en `scratchpad/codex_20260908_iter6_setupload_refs/`.

La cabecera real sigue sin tocarse: la integración requiere aplicar en conjunto
el cambio de CFG de V1 y el cuerpo exacto del `.cpp`, y después recompilar y
auditar todos sus consumidores reales.

## Comprobación independiente del segundo consumidor

Coordinación compiló una SourceList privada completa de `zEAXSound2`, con
los flags de producción, para comparar la cabecera actual y la propuesta.
El gate `scratchpad/codex_20260908_iter6_setupload_consumers/audit.py` pasa:
`ordered_base.o`, `ordered_trial.o` y el objeto real son idénticos en todas
las secciones runtime, sus bytes de reubicaciones, las reubicaciones
interpretadas y el inventario completo de funciones.

El orden de includes del arnés importa: preincluir Aems al comienzo de la
SourceList alteraba el orden de instanciación de plantillas y de rodata.
Los primeros `base.o`/`trial.o` son controles de arnés descartados, no pruebas
contra producción. Las variantes `ordered_*` insertan la cabecera justo
después de los tres includes que la preceden en `SndBase.cpp`, conservando
el resto de la SourceList y las rutas originales. Solo estas variantes
reproducen también el objeto real.

La búsqueda de consumidores sitúa las inclusiones directas de la cabecera
en `zEAXSound` y `zEAXSound2`. La sobrecarga StringKey modificada se usa en
`SetupNextLoad`; los demás sitios usan la sobrecarga por puntero, intacta.
Esto no sustituye la recompilación real y la comprobación de datos de
`zEAXSound` después de aplicar el cierre. El cambio de cabecera sigue
pendiente de la autorización específica solicitada al usuario; la
autorización general de promociones linked no se amplía a ese cambio.

## Prueba estricta contra el ELF original

Coordinación revisó y ejecutó también `audit_bytes.py` del mismo directorio:
PASS. Aplica las 43 reubicaciones ELF reales del cuerpo (24 REL14, 13 REL24,
3 HA y 3 LO), incluida la resolución de destinos contra símbolos de sección.
Los 1008 bytes reconstruidos coinciden exactamente con `0x800AB3A0` del ELF
original. Había 104 bytes distintos antes de relocar y cero después.

SHA-256 de ambos cuerpos:
`400f80d440bebe90653b74e5856666c451ccc6fe6655831270c193afdd4ce646`.
Las 12 reubicaciones externas resumidas por objdiff no eran el inventario
completo; esta prueba recorre directamente las 43 entradas del objeto.
Ver `docs/analisis/iter6-20260908-setupload-bytes.md`.
