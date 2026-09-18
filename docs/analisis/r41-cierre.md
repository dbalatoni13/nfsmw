# r41 - MsgPlayMiscSound cerrado y literal VM corregido

2026-09-07/08. Base `590dc4a0` mas trabajo local r37-r40 preservado.
Tanda de tres agentes y coordinacion. Integracion de sonido autorizada
explicitamente por el usuario el 8 de septiembre. El objetivo del 100 %
continua pendiente; no confundir similitud, codigo exacto y codigo linked.

## Progreso oficial verificado

| medida | antes r41 | despues r41 | delta |
|---|---:|---:|---:|
| codigo exacto | 3.892.344 / 3.946.048 B | **3.893.124 / 3.946.048 B** | **+780 B** |
| porcentaje exacto | 98,63904 % | **98,65881 %** | +0,01977 pp |
| funciones exactas | 18.372 / 18.432 | **18.373 / 18.432** | **+1** |
| codigo linked | 736.012 B / 18,651876 % | igual | 0 |
| datos exactos | 443.749 / 1.285.741 B | igual | 0 |
| datos linked | 319.036 B | igual | 0 |
| unidades completas | 478 / 618 | igual | 0 |

Quedan **52.924 B / 59 entradas de funcion / 27 unidades**, incluidos 20 B
de padding automatico `.init`, que no son una funcion del juego.

El censo directo independiente pasa de 3.892.488 B / 18.378 funciones exactas
a **3.893.268 / 3.946.204 B, 18.379 / 18.439 funciones**, en 618 unidades.
La diferencia historica de inventarios respecto al informe oficial permanece
igual y no se cuenta como progreso: ambos censos confirman +780 B / +1.

## Cambios retenidos

### Sonido: 780 B nuevos exactos

`SFX_Common::MsgPlayMiscSound`, 195/195 instrucciones, pasa de 99,40513 %
a **100 %**. El cambio aprobado tiene un alcance preciso:

- `MAIN_AEMS.h`: constructor `FX_UVES`, ejecutar `SetID` antes de `SetVolume`.
- `SFX_Common.cpp`: usar `width = -1` en los casos 3/4, conservando el caso 1.
  `SetWidth` satura los argumentos negativos a cero. La receta reproduce el
  cuerpo original sin afirmar recuperar el literal negativo concreto de su
  fuente perdida.
- Retirar el comentario `UNSOLVED` de la funcion ya cerrada.

No hay ASM nuevo, valores indefinidos, cambios de flags ni promocion de unidad.
Se recompilaron directamente ambos consumidores reales de la cabecera,
`zEAXSound` y `zEAXSound2`. El primero mantiene 771 exactas; el segundo pasa
de 925 a 926. `MsgBarrier` de r39 sigue exacta, 140 B.

El gate de produccion **no normaliza nombres locales**: compara las tablas
ELF crudas de 2.674 y 2.807 simbolos, las cuatro secciones de relocaciones
runtime de cada objeto y todos sus bytes alloc. Solo cambia el intervalo
`.text` de MsgPlayMiscSound. El auditor contra el ELF confirma 25 ramas,
64 relocaciones y cero literales. Detalles en [r41-sound.md](r41-sound.md).

### VM: correccion de fidelidad, sin inflar matched

`src/LibSN/vm.c`: reemplazado el mensaje inventado de `__VMMappingErrorAlert`
por los **103 B incluido NUL** que contiene el ELF original en `0x80452860`.
Se conserva el formato `%x` y la ausencia de salto de linea originales.

La funcion ya tenia sus 56 B al 100 % nominal; su literal era incorrecto.
La auditoria ahora pasa y todos los demas simbolos, instrucciones y
relocaciones de vm permanecen iguales. No se suman 56 B nuevos ni 103 B de
datos matched: el split de vm solo asigna codigo y sigue NonMatching.
Vease [r41-root.md](r41-root.md).

Tambien queda certificado el alias local de `VMAlloc` mediante ELF, no por
parecido de nombres: estatico `.sbss` de 4 B en `0x804FFD9C`, 15 relocaciones
con tipos/addends correctos, 256/256 B completamente relocalizados iguales
al original y tres destinos vecinos rechazados como controles negativos.
Esto certifica un cierre previo, no genera otro incremento del contador.
Vease [r41-vm-alloc-audit.md](r41-vm-alloc-audit.md).

## Ensayos retirados

- **Steering:** separar byte crudo/promocion de CookValues no reproduce el
  reparto completo; materializar una lectura unica de effectId en
  VDevice_DownloadEffect resulta neutro, tambien con acceso volatile.
  Fuente y JSON completos identicos a r40; los cierres de 116 y 992 B siguen
  certificados. [r41-steering.md](r41-steering.md).
- **Fisica:** nombrar o cargar volatile el literal 1.0 no conserva su mitad
  alta a traves de las llamadas de UpdateLoaded y puede aumentar datos/codigo.
  Retirado. RBGrid::Add mantiene la causa RTL conocida, sin repetir barridos.
  Las 1.120 funciones conservan porcentajes/tamanos; 1.118 exactas auditadas.
  [r41-physics.md](r41-physics.md).
- **vmbase/vm/dvd:** accesos volatile localizados y dependencia del puntero
  alineado no cierran sus funciones pendientes. Restauracion byte por byte,
  incluidos los finales de linea mixtos de vmbase y dvd; en vm solo queda
  la sustitucion independiente del literal. [r41-root.md](r41-root.md).

## Verificacion final

- `codex_r41_global_audit.py --sound-integrated`: 18.432 entradas oficiales,
  mismas direcciones/tamanos objetivo y ninguna regresion. Solo mejora
  MsgPlayMiscSound; delta exacto +780 B / +1 funcion.
- Censo de las 33 SourceLists: 15.493 entradas, solo cambia MsgPlayMiscSound;
  no cambia ningun otro porcentaje ni tamano de fuente.
- `codex_r41_sound/production_audit.py` y auditor individual repetidos por
  coordinacion: PASS. Auditorias completas de ambos consumidores: PASS.
- `codex_r41_root_audit.py` y `codex_r41_vm_alloc_audit.py`: PASS.
- Gates estrictos r39/r40 de steering y r39 del runtime: PASS.
- Reconstruccion del **DOL principal** mediante la lista generada de objetos,
  sin ninja completo: `dtk shasum` OK. SHA-1 confirmado independientemente:
  **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. Persisten solo las advertencias
  de simbolos duplicados ya conocidas en el enlace.
- `checksplits.py`: 1.090 rangos, cero solapes y cero cortes de simbolos.
- `lcfix.py --check` y `git diff --check`: limpios. Se conservan los finales
  originales de las fuentes; los avisos previos LF/CRLF no son regresiones.

La division autorizada de **ppc2D2: 100 B GCC + 244 B MW** ya estaba integrada
en r39; vuelve a pasar sus nueve funciones y el hash final. No contabilizar
otra vez esos 344 B linked por la reiteracion de su autorizacion.

No se modificaron configure.py, splits, flags ni otras cabeceras en r41.
Sus cambios locales previos siguen intactos. La unica cabecera editada ahora
es MAIN_AEMS.h, con permiso explicito. **spchpick sigue NonMatching:** su
promocion de 7.836 B requiere la respuesta a su pregunta independiente.
No inferir ese permiso de las autorizaciones del runtime o de sonido.

No hay git add ni commits. Los agentes de implementacion han liberado todas
las fuentes y objetos. Las propuestas para la siguiente tanda son solo
lectura y no modifican el baseline verificado; estan en
[r41-siguiente-tanda.md](r41-siguiente-tanda.md). Primero se propone trasladar
la hipotesis de saturacion de argumentos a CARSFX_RoadNoise::Play; despues,
un ensayo sombra de Vector::assign(), sin autorizacion implicita para editar
su cabecera compartida real.

Instantaneas finales:
`scratchpad/codex_20260907_r41_{before,after}_{report,measure,pct}.json`.
Informe oficial vigente: `build/GOWE69/report.json`.
