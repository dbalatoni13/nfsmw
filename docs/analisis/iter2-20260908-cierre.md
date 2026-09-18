# Continuacion iter2, 2026-09-08: +388 B exactos, FSasync preparada para linked

Base HEAD `8853122e`, posterior a resume-20260908-cierre. Se conservaron todos
los cambios locales anteriores, incluidos RecalcGammaTable, DownloadEffect y
SAP. Las autorizaciones antiguas ppc2D2 y MsgPlayMiscSound ya estaban integradas
y no se cuentan otra vez. No hubo commits, staging, ninja completo, cambios
de configuracion/splits ni de cabeceras compartidas.

## Censo final

| medida oficial | inicio | final |
|---|---:|---:|
| codigo exacto | 3.900.936 | **3.901.324 / 3.946.048 B** |
| porcentaje | 98,85678 % | **98,866615 %** |
| funciones exactas | 18.381 | **18.382 / 18.432** |
| codigo linked | 757.604 B | **igual: 19,199057 %** |
| datos exactos | 443.781 / 1.285.733 B | igual |
| datos linked | 355.348 B | igual |
| unidades completas | 483 / 616 | igual |

Delta: **+388 B / +1 funcion**, sin regresion en las 18.432 entradas oficiales.
Quedan **44.724 B en 50 entradas**, incluido el padding automatico de 20 B.
No se ha alcanzado el 100 %.

El censo directo independiente pasa de 3.901.080 a 3.901.468 B sobre 3.946.204,
y de 18.387 a 18.388 funciones. Solo cambia FSasync, de [1716,12,2104,13] a
[2104,13,2104,13]. La diferencia de inventario con el oficial es historica;
el delta coincide. Snapshot de 33 SourceLists/15.493 entradas byte-identico.

## Integrado y auditado

**FSasync / CompletePCreadAsync: 388 B, 97 instrucciones, 100 %.** La unidad
entera queda 13/13 exacta. La mitad alta del global estable se reconstruye con
dos productores lis y doce accesos lwz/stw, con restricciones de memoria y
valores definidos; se conserva el resto de la funcion en C. No es una funcion
reemplazada enteramente por ensamblador ni una comparacion que ignore destinos.
Las 14 relocaciones HA/LO se resuelven al global original; el auditor valida
10 ramas y 58 relocs de Complete, ademas de las doce funciones vecinas.

Tambien se integran el orden original de Complete/AsyncFSACK/SyncFSACK y un
prototipo static. El gate demuestra que los cuerpos de las dos definiciones
movidas no cambian ni un byte de fuente y que fuera del movimiento/prototipo
no cambia nada respecto al cierre. CRLF puro; macros limitados con undef.

Detalle: [iter2-20260908-fsasync.md](iter2-20260908-fsasync.md) y
[iter2-20260908-fs-link.md](iter2-20260908-fs-link.md).
El PLAYBOOK recoge la nueva palanca y excluye aplicarla con ordinales de pool
automaticos; UpdateLoaded no se ha tocado.

## Tanda paralela: sin otras integraciones

- JoyE/zPlatform: cuatro pruebas privadas de productores del mr/andi faltante.
  Dos dan tamano exacto, ninguna instrucciones exactas. Todas descartadas;
  fuente/objeto reales byte-preservados. [iter2-20260908-joy.md](iter2-20260908-joy.md).
- zCamera: tres sombras. Una factory de tabla conserva globales/orden y reduce
  49 RTX (tabla933->884, bloque1107->1058), pero necesita otros58; no se retiene.
  La relacion con clear_const_double_mem se documenta como inferencia, no traza
  instrumental. [iter2-20260908-camera.md](iter2-20260908-camera.md).
- zEAXSound/cStichWrapper::Play: depurador del compilador confirma el mecanismo
  inmediato: `regs_someone_prefers[116]` contiene solo el bit r31 y hace que
  find_reg elija r30. Falta demostrar que allocno inferior aporta esa
  preferencia; no se midieron variantes C ni hubo cambios de produccion.
  [iter2-20260908-stich.md](iter2-20260908-stich.md). Fuente/objeto conservan
  sus hashes, repetidos por coordinacion; se inspecciono la traza guardada.

## Verificaciones reproducidas por coordinacion

- `codex_20260908_iter2_global_gate.py`: inventario oficial, censo directo y
  snapshot SourceLists; unico cambio CompletePCreadAsync, +388/+1.
- `codex_20260908_iter2_fs_link/audit_final.py`: gate vigente de la fuente
  reordenada, repetido tambien por un segundo agente.
- `scripts/audit.py FSasync`: trece OK; fncmp cero pendientes.
- `MsgPlayMiscSound__10SFX_CommonRC10MMiscSound`: 780 B, 25 ramas, 64 relocs, OK.
- Gates privados de Joy y Camera repetidos, sin pruebas retenidas.
- Enlace principal desde lista generada real, sin ninja: `main.dol: OK`.
- Control y sustitucion privada de FSasync por objeto fuente reordenado:
  dos DOL completos iguales al original, 4.541.888 B.
- SHA-1 en los tres enlaces: **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
- checksplits: 1.092 rangos, cero solapes/cortes. lcfix y git diff --check OK.

Snapshots finales: `scratchpad/codex_20260908_iter2_after_{report,measure,pct}.json`.
`build/GOWE69/report.json` actualizado. Los manifiestos previos no se
sobrescriben para hacer pasar gates historicos; el informe fs-link indica
cual corresponde al estado actual.

## Pendientes de autorizacion, NO contabilizados

1. Nueva pregunta enviada: marcar solo la entrada **activa** FSasync de
   configure.py como Matching, sin splits. Prueba fuente+DOL ya pasa; aporta
   **2.104 B linked**, no mas matched. El reorden fuente ya esta hecho.
2. Propuesta de la tanda anterior: DebuggerDriver en tres TUs del ELF, con
   sus configuracion/splits precisos; **416 B matched y 5.988 B linked**.
3. Propuesta de la tanda anterior: quitar solo las dos lineas P6 de la cabecera
   ENVIRO_AEMS; **recupera 444 B** de InitSFX. La regresion era previa, no de
   esta tanda. No se retiran esas lineas sin respuesta explicita.

No interpretar la continuacion automatica del objetivo como aprobacion de
ninguna de estas tres operaciones. El objetivo sigue activo; la siguiente
tanda puede trabajar otras funciones sin tocar esas areas restringidas.

Todos los agentes terminaron y liberaron produccion. Dos continuaciones
causales quedan delimitadas: localizar el allocno que contribuye el bit r31
en sonido y adelantar la preparacion de tTable en camara sin mover globales
ni repetir las tres factories medidas. No volver a empezar esos diagnosticos
desde los porcentajes antiguos.
