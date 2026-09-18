# Iter6, 2026-09-08: cierre y continuación

## Estado real integrado

| medida | antes | después |
|---|---:|---:|
| código matched | 3902288 / 3946048 B | 3903296 / 3946048 B |
| porcentaje matched | 98,891045 % | 98,91659 % |
| funciones exactas | 18387 / 18432 | 18388 / 18432 |
| código linked | 771180 B | 771264 B |
| porcentaje linked | 19,543098 % | 19,545225 % |
| datos matched | 444041 B | 444041 B |
| datos linked | 355633 B | 355637 B |
| unidades completas | 491 / 619 | 492 / 619 |

Quedan **42752 B / 44 entradas** en el reporte. Una es padding automático
`.init` de 20 B, no una función de juego; conservarla y no alterar el
denominador para simular progreso. El censo directo conserva su discrepancia
histórica: 3903440/3946204 B y 18394 funciones exactas tras sumar el mismo
avance de 1008 B / 1 función.

## Promoción aplicada: eathread

Dos funciones C locales, 40+44 B, y su entrada `.ctors` de 4 B pasan al enlace
desde fuente. La prueba inicial sin storage fallaba porque el linker quitaba
el constructor. Retener solamente el wrapper dejó cinco bytes distintos;
retener además la entrada `.ctors` produjo el DOL original. Se integra la
receta exacta, dos reglas keep calificadas por objeto, el Object Matching y
un mapping de nombres privados para preservar la medición de ambos cuerpos.

No se modifican splits, headers ni flags. El wrapper de plataforma y PS2
quedan intactos. Las omisiones de GC se apoyan en ausencia de DEF/UNDEF/usos
en los 619 objetos y en el ELF original; el bucle de inicialización permanece
en los mismos bytes. No se cuentan funciones nuevas: ya eran exactas.

[Detalle y prueba](iter6-20260908-eathread-linked.md). Las primeras pruebas
negativas de eathread son históricas y no invalidan el resultado keep2.

## Cierre integrado: SetupNextLoad

**1008 B / 100 %, integrado con autorización expresa del usuario.** La corrección de CFG vuelve a
buscar referencias del mismo SndBase después de eliminar una. Cuatro
dependencias reales de `this`, distribuidas en dos cadenas sobre endIndex y
bankIndex inicializados, cruzan el umbral de prioridad RTL sin alterar los
argumentos. No hay operandos indefinidos ni instrucciones PPC añadidas.

La prueba estricta aplica las 43 relocaciones del objeto REAL: los 1008 bytes
coinciden con el ELF original en 0x800AB3A0. Se compilaron sólo zEAXSound y
zEAXSound2. La auditoría cubre todos sus STT_FUNC definidos, no sólo los
emparejados: **997 + 1112 cuerpos**, con única diferencia SetupNextLoad.
zEAXSound conserva 11266 relocs runtime vecinas, sus 43 relocs interiores
se resuelven por separado, y zEAXSound2 conserva sus 12976 relocs y el
objeto completo byte-idéntico. Ambas unidades mantienen inventario, flags,
alineación y datos de sus cinco secciones allocadas, incluidas .ctors.

Las cadenas __FILE__ de la ruta real permanecen idénticas al baseline:
el efecto de los nombres privados del arnés no se integró. Se conserva LF
y todo el CPP fuera del cuerpo y del comentario de cinco líneas que
actualiza la antigua veda. Se restauró también el salto de línea vacío
final original que apply_patch había omitido, sin relajar el gate de fuente.

Delta real: **+1008 B matched, +1 función, +0 linked**. zEAXSound sigue
NonMatching: aún queda cStichWrapper::Play (384 B). El DOL intacto por sí
solo no probaría el cierre de una SourceList aún no enlazada; por eso el
gate reconstruye el cuerpo fuente contra el ELF original y audita vecinos.

[Receta y consumidores](iter6-20260908-setupload-refs.md) ·
[Auditoría de bytes privada](iter6-20260908-setupload-bytes.md) ·
[Auditoría real](iter6-20260908-setupload-integration-audit.md).

## Ensayos negativos preservados

- [SetMemoryPoolSize](iter6-20260908-carloader.md): tres productores de store
  y copia reales; ninguno cierra el orden de las dos instrucciones. 846
  cuerpos auditados, ninguna modificación de producción.
- [libgcc2_4](iter6-20260908-libgcc.md): las dos formas de slot C eliminan el
  pool privado pero cambian el lowering. Se descartan esas formas medidas,
  no se afirma imposibilidad general.
- [Triage linked](iter6-20260908-linked-triage.md): antes de esta promoción,
  28 unidades NonMatching tenían código mapeado al 100 %, pero ninguna
  conservaba literalmente la misma estructura de secciones runtime. No
  relanzar barridos de sserver/ssysinit ni racimos de pools ya descartados
  sin una hipótesis nueva de propiedad/layout.

## Gates y snapshots vigentes

```text
python scratchpad/codex_20260908_iter6_setupload_integration/audit_final.py
python scripts/lcfix.py --check
python scripts/checksplits.py
```

Ambas integraciones recibieron revisión independiente. El gate POST vigente
de Setup, revisado y ejecutado por coordinación, protege 32 rutas, 619
objetos enlazados y 534 especificaciones de compilación; sólo cambia uno
de los 534 objetos fuente. Config/keep/splits y el grafo se conservan desde
el cierre eathread. En reporte, censo y 15493 entradas de las 33 SourceLists
sólo mejora SetupNextLoad; las otras 618 unidades del reporte son idénticas.
El gate invoca también la auditoría vecinal estricta de ambos consumidores.

La revisión independiente ejecutó también el gate combinado final: PASS.
La recompilación de zEAXSound después de restaurar el EOF mantuvo el mismo
SHA-256 del objeto (`e57f1efd65a3b7d19a36c2bcc72787a83bab4a2aaf18e0820bee695cb9d2054e`),
por lo que no invalidó los snapshots ni la comparación vecinal.

checksplits: 1106 rangos, cero solapes/cortes. lcfix limpio. Sin git add ni
commit; índice vacío y HEAD 8853122edb6ebea56a6085dd88a4938a72a24be3.

Snapshots oficiales nuevos:
`scratchpad/codex_20260908_iter6_setupload_integration/{report,measure,pct}_after.json`.
El reporte live es idéntico a report_after. Los snapshots eathread conservan
el punto intermedio. Sus gates POST y los gates privados Setup que exigen
la antigua fuente/reporte ya son históricos, como iter5/anteriores. No
reescribir hashes para hacerlos pasar. El nuevo gate conserva el estado
eathread integrado y todos los otros cambios locales.

DOL real: **4541888 B**, byte-idéntico al original, SHA-1:
**9619ba57c9919f95f7f2ac951a2166a3517f91e3**.

El objetivo del 100 % sigue pendiente. No quedan autorizaciones pendientes
para estas dos integraciones. Para continuar, partir del reporte final y
de los 44 pendientes; no repetir las familias negativas anteriores sin una
hipótesis causal nueva. No borrar ni deshacer los cambios locales del usuario.
