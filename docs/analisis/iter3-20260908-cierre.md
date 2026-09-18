# Continuacion iter3, 2026-09-08: +548 B, vm y vmbase exactas

Base HEAD `8853122e`, posterior a iter2-20260908-cierre. Las autorizaciones
antiguas de ppc2D2 y MAIN_AEMS/MsgPlayMiscSound ya estaban integradas; no se
reaplican ni se cuentan de nuevo. Se confirmo otra vez MsgPlayMiscSound780
exacta, con25 ramas y64 relocaciones correctas.

No hubo commits, staging, ninja completo, cambios de configuracion/splits ni
de cabeceras compartidas. Se conservaron los cambios locales del usuario,
los cierres previos de steering/FSasync y las notas historicas de fuente.

## Censo final

| medida oficial | entrada | salida |
|---|---:|---:|
| codigo exacto | 3.901.324 | **3.901.872 / 3.946.048 B** |
| porcentaje | 98,866615 % | **98,8805 %** |
| funciones exactas | 18.382 | **18.384 / 18.432** |
| codigo linked | 757.604 B | igual: **19,199057 %** |
| datos exactos | 443.781 / 1.285.733 B | igual |
| datos linked | 355.348 B | igual |
| unidades completas | 483 / 616 | igual |

Delta: **+548 B / +2 funciones**. Quedan **44.176 B en 48 entradas**, incluido
el padding automatico de20 B. No se ha alcanzado el100 %.

El censo independiente pasa de3.901.468 a3.902.016 B sobre3.946.204, y de
18.388 a18.390 funciones. La diferencia historica de inventario con el informe
oficial permanece; ambos miden el mismo incremento. Snapshot de33 SourceLists
y15.493 entradas byte-identico: ni una mejora parcial oculta una regresion.

## Integrado

- **vmbase / __VMBASESetupExceptionHandlers:380 B,95 instrucciones,100 %.**
  La unidad queda30/30 exacta,3180 B. Productores MW reales para la construccion
  de saltos, con puente mr que el compilador fusiona. No hay operandos
  indefinidos. Datos/raw relocs y todos los bytes fuera de esta funcion
  identicos al estado previo. [Detalle](iter3-20260908-vmbase.md).
- **vm / __VMAllocVirtualToARAMLUT:168 B,42 instrucciones,100 %.** La unidad
  queda18/18 exacta,2304 B. Tres instrucciones MW conservan la base vieja
  mientras se forma la nueva; sin fijar r4 para no reservarlo en el bucle.
  Los17 vecinos y los datos mantienen sus bytes. Se verifican todas las
  relocaciones compensando solo los desplazamientos documentados del cierre.
  [Detalle](iter3-20260908-vm.md).

Ambas integraciones fueron revisadas por otro agente contra el ELF original.
vm necesita un alias preexistente probado para auditar VMAlloc completo; no
es consecuencia del nuevo cierre ni una normalizacion arbitraria de nombres.
PLAYBOOK incorpora las dos palancas y sus variantes fallidas, no solo el exito.

## Trabajo paralelo no retenido

- Sonido: identificada instrumentalmente la cadena
  `&samplereq/pseudo141 -> full preference r31 -> regs_someone_prefers[116]`.
  El GCC original limpia la preferencia eliminable del conjunto normal/copy
  pero no del full. La identidad de la funcion depurada se valida mediante
  el mapa real de allocnos. Dos variantes:87,760414 % y99,6875 % neutra; fuente
  y objeto preservan sus SHA-256 iniciales. [Informe](iter3-20260908-stich.md).
- Camara: tres instanciaciones explicitas de constructores producen la misma
  sombra98,369590 %, tabla884/bloque1058: no eliminan los58 RTX restantes y
  ademas emiten simbolos extra. No conservar. [Informe](iter3-20260908-camera.md).
- spchsamp: tres pruebas del productor real de li result alcanzan93,52941 %;
  dos seguimientos con dependencias de calculo/store dan90 % y91,17647 %.
  Ninguna cierra, ninguna se integra. Fuente/objeto/CRLF y vecino48 B intactos.
  [Inicial](iter3-20260908-spchsamp.md),
  [seguimiento](iter3-20260908-spchsamp-followup.md).

## Linked: nueva prueba positiva, sin promocion real

vmbase fuente no enlaza directamente por siete nombres extern simples que no
exporta el objeto automatico. Sus28 B de datos pertenecen a VMBase.c segun
STT_FILE, pero moverlos a un monolito desplaza el layout y falla el DOL.

Una sombra mas acotada conserva los616 objetos y todos los datos tal cual:
renombra solo esas siete referencias a los aliases con direccion ya exportados
por el auto-objeto, y sustituye solo el objeto de codigo vmbase. Pasa30/30,
3180 B crudos, las150 relocaciones, auditor ELF sin overrides y **DOL completo
identico al original**. No se han integrado los aliases ni Matching.
[Prueba y limites](iter3-20260908-vmbase-link.md).

Se envio pregunta especifica para integrar esos siete aliases locales y marcar
unicamente LibSN/vmbase.c Matching, sin splits: **+3180 B linked** potenciales.
No interpretar una continuacion automatica como autorizacion.

Siguen sin respuesta las propuestas anteriores de FSasync Matching(+2104 B
linked), DebuggerDriver en3 TUs(+416 B matched/+5988 linked) y retirada de las
dos lineas P6 de ENVIRO_AEMS(recuperacion de444 B de una regresion previa).
La autorizacion vieja de MAIN_AEMS no cubre esos cambios.

## Verificacion final y artefactos

- `codex_20260908_iter3_global_gate.py`:18.432 entradas oficiales, solo las
  dos mejoras indicadas; censo independiente coherente;33 SourceLists iguales.
- Gates vigentes `iter3_vm/audit_shadow.py production` y
  `iter3_vmbase/audit_shadow.py production`: unidades enteras, fuente y ELF.
  No ejecutar el modo preintegracion esperando que preserve la fuente anterior.
- Gate alias-only vmbase repetido por coordinacion; SHA-1 comprobado de nuevo
  sobre ambos ficheros DOL, no solo sobre los valores guardados del informe.
- Gates negativos camara/spchsamp repetidos; hashes de Stich y traza de
  set_preference leidos y confirmados por coordinacion.
- Gate FSasync fuente reordenada repetido: se mantiene el cierre anterior y
  su prueba de DOL completa, sin promocion de configuracion.
- Enlace principal real repetido tras ambos cierres, sin ninja: main.dol OK.
  SHA-1 **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
- lcfix, checksplits1092 rangos sin solapes/cortes y git diff --check limpios.

Snapshots finales `scratchpad/codex_20260908_iter3_after_{report,measure,pct}.json`;
informe vigente actualizado `build/GOWE69/report.json`. El checkpoint posterior
a cerrar solo vmbase se guarda aparte; ningun baseline previo se sobrescribe.

Todos los agentes liberaron produccion. El objetivo sigue activo. La siguiente
tanda debe partir del estado98,8805 %, no de la base r46. Las nuevas pistas
MW estan documentadas; para sonido falta una forma historica/semantica que
cambie la formacion/interferencia de la direccion de pila, no otra permutacion
de las variables que ya se sabe no controla la preferencia residual.
