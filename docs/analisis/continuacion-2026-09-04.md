# Continuacion del 4 de septiembre de 2026

## Punto de partida verificado

Se recompilaron las 33 SourceLists antes de tomar la referencia. El resumen de la sesion anterior estaba desactualizado: `Render(ePoly*)`, `TimeUntilBusted` y `UpdateASREvent` ya estaban cerradas y no se cuentan como avances nuevos.

- Medicion directa inicial: 3.737.088 / 3.946.204 B, 18.050 funciones exactas.
- Referencia: `scratchpad/resume_20260904_before.json`.
- Se conservaron los cambios locales existentes; sin commits, staging ni cambios de configuracion.

## Primera tanda integrada

| Funcion | Unidad | Bytes nuevos exactos |
| --- | --- | ---: |
| AIActionPursuitOffRoad::UpdateRoadAffinity | zAI | 600 |
| CustomizeParts::Setup | zFeOverlay | 1.708 |
| SFXObj_PFEATrax::TestToLicensed | zEAXSound2 | 876 |
| ScheduledSpeechEvent::sort_nested_priority | zEAXSound | 104 |
| MemoryPoolManager::NewBlockAux | zEagl4Anim | 84 |
| Total | | 3.372 |

Mejoras parciales conservadas (no suman bytes exactos):

- FEngine::ProcessPadsForPackage: 99,9091 -> 99,9870 %. Quedan dos inicializaciones `li` intercambiadas. Alias r30 de la mascara del pad en el segundo bucle resuelve el intercambio r30/r31. Las pruebas de reordenar Held/Released o fijarlos durante toda la funcion empeoraban; descartadas.
- IconScroller constructor: 93,4479 -> 95,4792 %. Restaurar la inicializacion mediante `AnimateCubic(1, 1.0f)` mejora el experimento manual heredado; no esta cerrado.
- GManager::PreloadTransientVaults: 93,4000 -> 94,6000 %. Alias locales de vault/base/desplazamiento. Pendiente el izado de la parte alta del literal de nombre del pool. Declarar el literal en una local adelantada emitia una instruccion extra; descartado.
- RegionQuery::CalculateRegionInfo: 98,5464 -> 99,9205 %. Quedan cuatro escrituras rotadas en la rama de override.
- EmitterSystem::Render: 82,1207 -> 98,0747 %. 688 B frente a 696 del objetivo; pendientes derrame y marco.
- eProject: 85,8209 -> 88,8507 %.

Otras pruebas descartadas: SubTitler::GetElapsedTime (fr0 y barreras desplazaban el retorno compartido), EmitterSystem::UpdateParticles (constantes fr20/fr21), QueueFileLoad (conserva 99,86413 % y cambios locales anteriores).

## Verificacion del primer hito

- `ninja`: termina correctamente.
- Informe oficial: 3.740.460 / 3.946.048 B, **94,7900 % matched**, 18.055 / 18.432 funciones.
- Medicion directa: 3.740.460 / 3.946.204 B, **94,7863 %**, 18.055 funciones exactas. El denominador de la herramienta directa incluye simbolos adicionales; no mezclar sus porcentajes con los oficiales.
- Delta directo: **+3.372 B, +5 funciones**; comparaciones por funcion sin regresiones.
- SHA-1 de `build/GOWE69/main.dol`: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.
- Datos oficiales: 28,76 %; enlazado: 7,56 %. El hash correcto no significa que toda la decompilacion este terminada: la compilacion aun utiliza los objetos originales donde corresponde.
- Evidencias: `scratchpad/resume_20260904_phase1_measure.json` y `scratchpad/resume_20260904_phase1_all_pct.json`.

## Segunda tanda

- Animacion: unico pendiente de zAnim, `CWorldAnimEntity::Init` (912 B).
- Mundo: `WTriggerManager::CheckCollideSRB`, seguido de `WRoadNav::InitAtSegment` si resulta accionable; preservar WRoadNetwork.cpp local.
- Simulacion: `SimTask::Link` y `QuickGame::OnManageTime`; evitar repetir el constructor de Sim::Entity sin una hipotesis nueva.
- Principal: pendientes de CustomizeMain y control de integracion.

### Segunda tanda integrada

| Funcion | Unidad | Bytes nuevos exactos |
| --- | --- | ---: |
| CWorldAnimEntity::Init | zAnim | 912 |
| PhysicsObject constructor | zPhysics | 1.436 |
| SimTask::Link | zSim | 144 |
| FEngSetScaleX | zFe | 256 |
| FEngSetScaleY | zFe | 256 |
| Total | | 3.004 |

zAnim queda completamente exacta: 42.292 B, 315 funciones. Esto no implica que todo el proyecto este terminado.

Detalles reproducibles:

- AnimEntity: barrera vacia entre SetLocalDelayTime y GetAnimPart.
- PhysicsObject: binding local del argumento de IBody a r3 en la lista de inicializacion.
- SimTask::Link: cola append antes del bloque de busqueda; conserva la logica y las 36 instrucciones.
- FEngSetScaleX/Y: una carga explicita desde object_reg/r30 con clobber de memoria; solo la copia inicial de escala se fija a fr13 en un ambito corto. Fijar toda la variable scale empeora la aritmetica posterior y fue descartado.

Mejoras parciales: QuickGame::OnManageTime 94,01418 -> 97,432625 %; WRoadNav::InitAtSegment 99,004906 -> 99,15196 %; CustomizeMain::NotificationMessage 97,89604 -> 98,88614 %. NotificationMessage conserva una referencia normal a gCarCustomizeManager; las pruebas que fijaban esa referencia a r31 no son seguras y fueron revertidas.

### Verificacion del segundo hito

- ninja: termina correctamente, 10/10 pasos.
- Informe oficial: 3.743.464 / 3.946.048 B, **94,8661 % matched**, 18.060 / 18.432 funciones.
- Medicion directa: 3.743.464 / 3.946.204 B, **94,8624 %**, 18.060 funciones exactas.
- Delta fase 2: **+3.004 B, +5 funciones**. Acumulado desde la referencia inicial: **+6.376 B, +10 funciones**.
- SHA-1 sigue siendo `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.
- Evidencias: `scratchpad/resume_20260904_phase2_measure.json`, `scratchpad/resume_20260904_phase2_all_pct.json`, `scratchpad/resume_20260904_phase2_build.log`.

## Tercera tanda

Reparto independiente: libc/fseek e itoa; path/pathsnd y pathnode; gc_driver y avplayer; principal LibSN/steering y control de integracion. Sin cambiar flags, headers compartidos, splits ni estado de Git. Verificar las bibliotecas con objetos reales y comparar todas sus funciones, no solo el candidato.

### Tercera tanda integrada

| Funcion | Bytes nuevos exactos |
| --- | ---: |
| fseek | 996 |
| fftoa | 1.256 |
| PathTrackSndStream::CheckStatus | 672 |
| PathToSnd::CreateStreamTrack | 404 |
| PATHI_queuenode | 896 |
| GCDriver::OpenFile | 692 |
| AV_PLAYER::GetFirstFrame | 656 |
| Effect_PolarToRect | 180 |
| Effect_UpdateDamper | 96 |
| VP6_DecodeBlock | 4.728 |
| FILE_init | 824 |
| __VMBASESetupVMRegisters | 84 |
| RegionQuery::CalculateRegionInfo | 1.208 |
| Total: 13 funciones | 12.692 |

Detalles y descartes para continuar sin repetir trabajo:

- fseek: ptr/r30, blockOffset/r29 y blockMask/r0. fftoa: digit compartido entre los dos bucles, lo/r11 y dos barreras vacias. Las dos unidades completas quedan al 100 % (5 funciones, 2.544 B).
- pathsnd: barrera de memoria despues de mUpdateStatus; CreateStreamTrack recupera el marco mediante framePad de 8 B no leido y sin instrucciones emitidas. pathnode: dos barreras sobre pfstate. Auditadas 77 funciones y 35 referencias a literales, sin regresiones ni discrepancias.
- GCDriver::OpenFile: corregida ademas la ruta de fallo de ReadFile: el original retorna sin CARDClose, no pasa por la limpieza comun. Resultado r25. AV_PLAYER: conserva la copia inicial de latencia en r29. 63 funciones auditadas sin regresiones.
- LibSN/steering (MW GC/2.7): PolarToRect cierra separando product = v * 127 y product += round; Damper separa v *= sat. NO modificar MULDIV global: cambiarlo afecta negativamente a Envelope y SimThread_Step. Parciales conservadas: VDevice_DownloadEffect 98,510635 -> 99,3617 % (ret declarado primero), Effect_Update 99,435486 -> 99,83871 % (calculos por etapas, quedan registros en rampa). Auditadas 36 funciones, sin regresiones.
- VP6_DecodeBlock: orden byteoffset + base y registros finalIndex/r27 y finalOffset/r0 al final del inline VP6_ReadTokensPredictB. 21 funciones auditadas sin regresiones. PredictFilteredBlock continua pendiente; FindNearestandNextNearest ya era exacta.
- FILE_init: placement new de tres argumentos con throw(), asignacion de pDeviceMem antes de buf, nDevices y registros de los pools. 78 funciones auditadas sin regresiones.
- VMBASE: una sola variable sdr1 actualizada despues de guardar el original. SetupExceptionHandlers mejora 90,47369 -> 98,73684 % (380 B); vm.c queda restaurado a su referencia. 48 funciones auditadas sin regresiones.
- RegionQuery: barrera especifica +m(DistFogStart), falloffX/fr13 y falloffY/fr0; comparar oldDistFogStart con fogStart. Mantener la asignacion final oldDistFogStart = DistFogStart: cambiarla a fogStart rompe la cola compartida y agrega 36 B. Barreras globales, basicas o solo entrada adelantaban el bloque completo y se descartaron.

### Verificacion del tercer hito

- ninja: 20/20 pasos, incluyendo LINK, DOL y CHECK; main.dol OK.
- Informe oficial: **3.756.156 / 3.946.048 B, 95,1878 % matched**, 18.073 / 18.432 funciones.
- Medicion directa: 3.756.156 / 3.946.204 B, 95,1840 %, 18.073 funciones exactas.
- Delta frente a fase 2: **+12.692 B, +13 funciones**. Acumulado desde inicio de esta continuacion: **+19.068 B, +23 funciones**.
- Comparacion de 15.493 simbolos SourceLists con umbral 0: una mejora (RegionQuery), ninguna regresion. Auditorias de bibliotecas detalladas arriba.
- SHA-1 conservado: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. git diff --check sin errores (avisos normales de CRLF).
- Evidencias: `scratchpad/resume_20260904_phase3_measure.json`, `scratchpad/resume_20260904_phase3_all_pct.json`, `scratchpad/resume_20260904_phase3_build.log`. Datos 28,76 %, enlazado 7,56 %: el proyecto NO esta al 100 %.

## Cuarta tanda

Reparto: LibSN/steering; snddrv y sst; libc/vfprintf y vfprintf_1; principal csis. La nueva referencia global ya esta tomada antes de compilar estas unidades.

### Cuarta tanda integrada

| Funcion | Bytes nuevos exactos |
| --- | ---: |
| Effect_UpdateSpring | 160 |
| SNDDRV_audiocallback | 400 |
| SNDSTRMI_parsedata | 280 |
| Csis::System::Init | 264 |
| Csis::System::Subscribe | 392 |
| Csis::System::Unsubscribe | 232 |
| Csis::ICoreToIAllocatorAdaptor::Alloc, dos sobrecargas | 168 |
| SFILTER_splitter | 256 |
| SNDSTRM_fxlevel | 252 |
| add_separators | 392 |
| GCDriver::ReadFile | 464 |
| Wildcard | 576 |
| Total: 13 funciones | 3.836 |

Detalles y verificaciones:

- Steering: Spring cierra separando la diferencia y materializando coeff/low. 36 funciones auditadas, unica mejora Spring. Las nuevas pruebas de DownloadEffect (inicializador, tipos, register, ambitos y expresion del indice) y Update (start explicito, divisor, quotient) no mejoraron; todas revertidas.
- snddrv/sst: 70/70 funciones exactas; 38 referencias de literales verificadas. Se corrigieron ademas el enlace C local de MIX_getwetbuffer y un error heredado de SetPan: el original usa 1/32767 (0x38000100), no 1/32768 (0x38000000). Este ultimo era invisible a matched_code.
- sfsplit/sstfxlev: 4/4 funciones exactas; dos literales verificados. Fxlevel no llama a SNDfxlevel cuando falta voz valida, como el original; ademas cambia el orden de suma de direccion.
- CSIS: unidad completa, 57/57 funciones y 5.728/5.728 B. NullAllocatorFlags conserva tag/next/value y la base de la ultima escritura mediante un stw local con operando de memoria. No se modifica el header compartido. Subscribe/Unsubscribe expanden las operaciones de lista manteniendo los registros y las bases originales. Auditoria independiente: 46 reubicaciones simbolicas, 22 destinos de salto, cadena CsisAlloc y nueve referencias de gUniqueKeyId correctas; layout y semantica de la estructura temporal y listas equivalentes.
- printf: add_separators exacta; _vfprintf_r 95,00196 -> 98,588776 %, _vfiprintf_r 96,423615 -> 98,579865 %. Quedan 6.156/6.128 B y 5.788/5.760 B respectivamente: 28 B de mas en cada cuerpo, prefijos PRINT sin fusion y registros. Se recuperaron CR4 y los marcos originales. Corregido %s(NULL): respeta precision mediante el camino comun memchr/strlen. Seis funciones auditadas sin regresiones; solo vfprintf.c cambia, la variante INTEGER_ONLY lo incluye.
- memcard: ReadFile recupera el control inicial de rango al pie del bucle y nBytesRead/r25. Wildcard recupera bloques compartidos de negacion y fallo. Utilities queda completa (4 funciones, 1.012 B). Auditoria: 58 simbolos emitidos, unicamente dos cierres; OpenFile preservado. Hashes de .rodata/.data/.sdata identicos, incluida tabla CRC.
- Parcial principal: PATHI_calcwaitbeat 98,5119 -> 98,86905 %, mismo tamano (336 B); solo fija el numerador inicial en fr11. Las 11 funciones de pathnode no regresan y los 35 literales de pathsnd/pathnode siguen correctos. NO conservar las pruebas de PATHI_nextnode fijando forreal a r31: el compilador no reponia su salvado en el prologo, incluso con operandos/clobber; todas revertidas. Fijar scalar durante toda la funcion quitaba dos fmr y las barreras de float alteraban el marco; descartado.

### Verificacion del cuarto hito

- ninja: 35/35 pasos, LINK/DOL/CHECK correctos, main.dol OK.
- Informe oficial: **3.759.992 / 3.946.048 B, 95,2850 % matched**, 18.086 / 18.432 funciones.
- Medicion directa: 3.759.992 / 3.946.204 B, 95,2812 %, 18.086 funciones exactas. Comparacion por unidad: ninguna pierde bytes exactos; 15.493 simbolos de SourceLists sin cambios frente al tercer hito. Auditorias de funciones de bibliotecas sin regresiones detalladas arriba.
- Acumulado desde la referencia inicial: **+22.904 B, +36 funciones**. Fase 4: **+3.836 B, +13 funciones**.
- SHA-1 conservado: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`; git diff --check sin errores.
- Evidencias: `scratchpad/resume_20260904_phase4_build.log`, `scratchpad/resume_20260904_phase4_measure.json`, `scratchpad/resume_20260904_phase4_all_pct.json`; snapshots individuales de cada agente y `scratchpad/csis_resume_20260904_after.json`, `scratchpad/pathnode_root_round4_after.json`.

## Quinta tanda

Referencia: 3.759.992 B y 18.086 funciones exactas. Reparto independiente: gc_driver y DVD; pathsnd, interfaz abstracta y libc pequena; printf; principal DebuggerDriver e integracion. Se coordino expresamente la unica modificacion de header compartido (IPathToSnd.h), con baseline de sus 13 consumidores. No cambios de flags, splits, mappings ni commits.

### Cierres de la quinta tanda

| Funcion | Bytes nuevos exactos |
| --- | ---: |
| GCDriver::WriteHeaderData | 424 |
| GCDriver::RecordIplDataChecksum | 264 |
| GCDriver::VerifyIplDataChecksum | 220 |
| PathTrackSnd::PathTrackSnd | 140 |
| PathTrackSnd::GetPathStatus | 456 |
| PathTrackSndBank::Play | 276 |
| IPathToSnd::~IPathToSnd | 52 |
| DBGHandler | 64 |
| DBGReadStatus | 172 |
| DBGWrite | 220 |
| DBGRead | 220 |
| DBGReadMailbox | 172 |
| AmcEXISync | 408 |
| AmcEXISelect | 128 |
| AmcDebugIntHandler | 80 |
| memset | 148 |
| strstr | 104 |
| Total: 17 funciones | 3.548 |

Detalles para retomar sin repetir ensayos:

- gc_driver completo: 8.952/8.952 B, 46/46 funciones objetivo exactas. WriteHeader usa sizeof(comments); RecordIpl conserva sizeofcomment en la condicion; VerifyIpl inicializa readBlockSize antes de readStartPosition y declara readEndPosition al nivel de funcion. Auditoria de los 54 simbolos emitidos: solo tres cierres, ninguna regresion, .rodata/.data/.sdata identicas. OpenFile y ReadFile preservados. Evidencias: `ai_memcard_round5_driver_{antes,despues_real}.json` y `ai_memcard_round5_measure.json` en scratchpad.
- pathsnd: constructor conserva opciones en r4; Play conserva flag en r9; ambos con restricciones vacias. GetPathStatus recupera el orden de bloques del lookup. Tres cierres, 32 literales correctos. El destructor de 52 B NO se forzo con mapping: la vtable original en 0x80413738 tiene __pure_virtual en las tres ranuras. Anadir `= 0` a CreateStreamTrack, CreateBankTrack y GetDefaultPlayOpts restaura la interfaz abstracta y emite destructor y vtable reales. Los 13 consumidores conservan 184 funciones existentes y 70 simbolos de datos; 87 literales correctos. Vtable nueva de 48 B identica al original tras resolver sus cuatro relocs. pathsnd: 67/67 funciones exactas. Evidencias: `pathsnd5_audio_resume_20260904_*` y `pathheader_audio_resume_20260904_*`.
- DebuggerDriver usa el MW GC/1.2.5n configurado, no ProDG; eliminado aviso obsoleto que decia lo contrario. Lectura/escritura: acumulador CSR por etapas antes de inicializar comando, cursor local dataPtr, padding de pila que recupera las posiciones originales de comando/datos. AmcEXISync: recorrido con `*buf++`, declaraciones i/data/buf en ese orden. AmcEXISelect: acumulador CSR leido antes de combinar freq.
- Los handlers no eran meros swaps de registros: los prototipos de callback habian perdido el segundo argumento. El SDK local declara EXICallback(s32, OSContext*), y el target mantiene r4 hasta blrl. Restaurar context cierra ambos handlers; se propaga tambien por los adaptadores MTR/TRK sin cambiar sus instrucciones. Los tipos de callback son locales, sin modificar headers SDK. Auditoria de 31 funciones: ocho cierres, cero regresiones; 154 referencias simbolicas, cuatro referencias de cadena y 31 destinos de salto de funciones cerradas verificados; .data/.sdata/.sbss y relocs preservadas. Script reproducible: `scratchpad/phase5_debugger_audit.py`.
- Parciales SDK: EXI2_Init 94,80851 -> 97,085106 % con tamano ya 188/188 B (zero y ok comparten asignacion); EXI2_Poll 99,181816 -> 99,28788 % con slots originales; AmcEXIImm 94,06896 -> 98,586205 % con mascara unsigned, orden data/i y ternario mode!=1. Quedan registros/orden y comparacion inicial del bucle. Descartados: temporal PI en Poll (regresaba), register del indice y bucle con goto (sin mejora). No dejar el ensayo del callback temporal de DBGHandler: no resolvia la ausencia de context.
- libc: memset conserva puntero de palabras separado, mascara solo en camino alineado y patron en r0; restriccion vacia sobre patron/longitud para ordenar la comparacion. strstr comparte retorno mediante etiqueta y restriccion vacia del indice para impedir duplicar la primera iteracion. Dos unidades al 100 %, sin llamadas externas ni literales. Evidencias `libc_audio_resume_20260904_{before,final}.json` y `_notes.md`.
- printf: _vfprintf_r 98,588776 -> 99,010445 %, _vfiprintf_r 98,579865 -> 99,32708 %. Ahora ambos tamanos son exactos (6.128 y 5.760 B); NO son cierres al 100 %. Colas compartidas de prefijos recuperadas y cuatro auxiliares exactas preservadas. Seis funciones auditadas sin regresiones: 14 literales, cuatro tablas, 195 referencias simbolicas y 712 destinos de salto correctos. Script `scratchpad/vfprintf_resume5_audit.py`; faltan principalmente registros y scheduling.
- DVD: corregido `BytesRead += DVDReadAsyncPrio(...)` en rama PreBytes de StartNonAlignedAyncRead, acorde al original; antes sobrescribia el acumulado. 92,454544 -> 93,59091 %, sin cierre. 19 simbolos emitidos sin regresiones, datos/relocs externas identicos. Ensayos de registros descartados; solo queda el cambio semantico respaldado. Evidencias `ai_dvd_round5_*`.
- Cuidado con el censo de gc_interface de realmemcard: ya es 6.152/6.152 B y 31/31 funciones usando el symbol_mapping preexistente del static initializer. El informe oficial y measure directo siguen dejando sus 2.456 B fuera, mientras `objdiff-cli 3.8 diff -p . -u ...` aplica el mapping y verifica el cuerpo. No se altero configuracion ni se conto como ganancia nueva. Evidencia `ai_gcinterface_round5_mapped.json`; revisar esta limitacion de medicion separadamente.

### Verificacion del quinto hito

- ninja: 27/27 pasos, LINK/DOL/CHECK correctos; main.dol OK.
- Informe oficial: **3.763.540 / 3.946.048 B, 95,374916 % matched**, 18.103 / 18.432 funciones.
- Medicion directa: 3.763.540 / 3.946.204 B, 95,3711 %, 18.103 funciones exactas. Las 589 unidades conservan o aumentan bytes exactos; los 15.493 simbolos de las 33 SourceLists permanecen iguales frente al cuarto hito. Auditorias por funcion de todas las bibliotecas modificadas sin regresiones, detalladas arriba.
- Delta de esta tanda: **+3.548 B, +17 funciones**. Acumulado de las cinco tandas: **+26.452 B, +53 funciones**.
- SHA-1 conservado: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`; git diff --check sin errores (avisos CRLF habituales).
- Datos siguen en 28,760845 % y codigo enlazado en 7,556523 %. El SHA correcto no demuestra que todo el juego este reconstruido: el build sigue utilizando objetos originales para unidades no enlazadas desde fuente.
- Evidencias globales: `scratchpad/resume_20260904_phase5_build.log`, `_before.json`, `_measure.json` y `_all_pct.json`; snapshots y auditorias individuales arriba.

## Sexta tanda

Referencia revalidada en el worktree (HEAD ef92ca4e): 3.763.540 B y 18.103 funciones exactas. Reparto: filesys; Path pequeno y wcstring/syscalls; printf; principal DebuggerDriver. Sin commits ni cambios de headers, flags globales, compiladores configurados o splits. Tras integrar las fuentes se corrige por separado la version/invocacion del medidor, como se detalla abajo.

### Cierres verificados por unidad

| Funcion | Bytes nuevos exactos |
| --- | ---: |
| FileOperationQueue::Find | 252 |
| FILEOPERATION::SetName | 232 |
| FILE_overhead | 72 |
| PATHI_switchproject | 96 |
| PATHI_clearalleventfilters | 92 |
| PATHI_serviceevent | 184 |
| wcscat | 104 |
| _write | 80 |
| DBGEXIImm | 664 |
| EXI2_ReadN | 700 |
| AmcEXIImm | 580 |
| Total: 11 funciones | 3.056 |

Detalles de reconstruccion y auditoria:

- filesys: Find expande localmente Remove para compartir el cero de cabeza/cola sin tocar el header; SetName debe pasar name como TagValuePair(1), no la cadena "FILEOP NAME"; FILE_overhead recupera su orden de calculo. Unidad: 10.304/10.872 B, 72/74 funciones objetivo. 83 simbolos emitidos sin regresiones. Parciales: AddToQueue 83,6508 -> 95,07937 %, FILE_nametodevice 83,8481 -> 98,73418 %. Faltan scheduling/mr./destino equivalente en el primero y una rama inicial redundante en el segundo.
- Auditoria filesys adicional: corregidos los rotulos "FILE THREAD STACK"/"FILE SYSTEM" a "File System" y las dos expansiones de __FILE__ a "d:/packages/realcore/6.24.00/source/file/cmn/filesys.cpp", respaldado por ELF original. Estos fallos preexistentes eran invisibles al match de instrucciones. FILE_init sigue siendo exacta. Verificadas 22 referencias de cadenas en seis funciones, nueve vtables completas y sus 46 relocs. Script `scratchpad/ai_filesys_round6_audit.py`; snapshots `ai_filesys_round6_{antes,despues_real,measure}.json`.
- Path: switchproject fuerza recarga de idflags tras pfstate; clearalleventfilters mantiene la barrera de memoria dentro del if; serviceevent usa cuerpo positivo/else-break y termina el offset de accion antes de sumar base. 33 funciones emparejadas sin regresiones y 34 simbolos emitidos no modificados intactos; nueve relocs y 15 ramas de los cierres correctas. PATH_setallocator no cerro y todos sus ensayos fueron revertidos: pathreal.cpp queda sin diff. Evidencias `pathevents6_audio_resume_20260904_*`.
- wcscat: suma de offset de bytes mas base; _write: restriccion vacia de buffer/numBytes conserva preparacion de argumentos dentro de la rama fd==1/2. Once funciones de las dos unidades exactas, sin regresiones; cinco relocs y tres ramas verificadas. PCwrite y fn_80310E08 ya eran aliases de la misma direccion en symbols.txt; no se cambiaron mappings. Evidencias `wcsys6_audio_resume_20260904_*` y auditoria `pathevents6_audio_resume_20260904_audit.py --wcsys6`.
- EXI2_ReadN: separar selected local del resultado sel; orden de declaracion rest/n/pos; separar len de transferencia y bytesToCopy del bucle de copia. Recupera las duraciones de las variables y registros sin asm ni flags.
- DBGEXIImm y AmcEXIImm: directivas locales `#pragma push / opt_propagation off / pop` conservan los cmpw originales del indice cero, en vez de cmpwi contra longitud. DBG elimina el alias nextWordPtr por iteracion y usa indice directo; el alias con propagacion desactivada producia siete sumas sobrantes en el bucle desplegado. Areas de pila pad[3] y pad[2] recuperan marcos/slots originales. Las directivas no afectan otras funciones ni configuracion global. 31 funciones de la unidad auditadas: tres cierres, ninguna regresion ni funcion extra emitida; 154 refs simbolicas, cuatro refs de cadena y 61 ramas de los cierres correctas; .data/.sdata/.sbss y sus relocs conservadas. Script `phase5_debugger_audit.py --phase6` y snapshots `phase6_debugger_{before,after}.json`.
- Parciales SDK: DBWrite 91,0921 -> 98,17105 %, ya 608/608 B. Dos auxiliares static inline solo para sus transferencias reproducen los resultados booleanos residuales que faltaban en la expansion manual; no se emiten cuerpos extra ni cambian otras funciones. Quedan slots de los comandos inlined (desfase de 0x2c) y seleccion de direccion. EXI2_WriteN 89,68519 -> 94,44444 %, ya identica salvo scheduling del epilogo: longitud local transferLength, comandos head/statusHead separados, cursor y puntero chunk, bit de status desplazado antes de compararlo. Se restituye AmcExiError como tipo de retorno de ReadN/WriteN, conforme al header SDK; no altera las instrucciones.
- Diagnostico aislado de compiladores: GC/1.2.5 reproduce los epilogos de EXI2_WriteN y AmcEXISetExiCallback, pero empeora DBRead/DBQueryData/DBInitComm; NO se aplica. GC/1.2.5n se conserva. Pruebas de GC1.2.5n antiguo, proc750, nofunctions, nopeephole y noschedule producen regresiones. -O3,p no cambia resultados; nopropagation global perjudica otras rutinas, por eso se limita a las dos funciones verificadas. Evidencias `phase6_debugger_compilers/results.json` (experimentos aislados, no objetos oficiales). El DWARF y .debug_sfnames originales no aportan estos nombres SDK; no afirmar divisiones de unidades originales no demostradas.
- Ensayos descartados SDK: array setup y variantes volatile en EXI2_Init no mejoran el estado previo; temporal PI a nivel de funcion en EXI2_Poll regresa y se revierte; DBMailboxOffset inline y temporales de offset no resuelven el registro. Mantener el padding/auxiliares de DBWrite actuales como parcial, no contarlo como cierre.

- printf: _vfprintf_r 99,010445 -> 99,51566 %, 6.128/6.128 B; _vfiprintf_r 99,32708 -> 99,90972 %, 5.760/5.760 B. Ambos siguen pendientes, no son cierres. La subtanda 6b declara muertos los registros del parser despues de PRINT del literal, antes de reinicializarlos, y recupera m/r31 sin fijarlo globalmente. En la variante entera quedan 23 diferencias de operandos, ninguna insercion/eliminacion: dos mr del prologo y registros de las tablas de espacios de PAD. Seis funciones sin regresiones, cuatro auxiliares exactas preservadas; 14 literales, cuatro tablas, 195 referencias simbolicas y 712 destinos de salto verificados. Auditoria `python scratchpad/vfprintf_resume5_audit.py 6b`; notas y snapshots `vfprintf_resume6b_20260904_*` y `vfiprintf_resume6b_20260904_*`.

### Verificacion del sexto hito antes de corregir la medicion

- ninja: 11/11 pasos correctos, nueve objetos compilados y REPORT/PROGRESS. Esta tanda NO ejecuto LINK/DOL: las unidades afectadas todavia usan objetos originales para el enlace. Se comprobo el hash del DOL existente, sin atribuirle una reconstruccion desde estas fuentes.
- Informe oficial con objdiff 3.7.0: **3.766.596 / 3.946.048 B, 95,45236 % matched**, 18.114 / 18.432 funciones.
- Medicion directa historica: 3.766.596 / 3.946.204 B, 95,4486 %, 18.114 funciones exactas. Las 589 unidades conservan o aumentan bytes exactos; los 15.493 simbolos de las 33 SourceLists permanecen iguales frente al quinto hito. Las auditorias por funcion de las bibliotecas modificadas tambien pasan sin regresiones.
- Ganancia real de codigo de esta tanda: **+3.056 B y +11 funciones** en seis unidades. Acumulado de las seis tandas: **+29.508 B y +64 funciones nuevas exactas**.
- Evidencias: `scratchpad/resume_20260904_phase6_build.log`, `_before.json`, `_measure.json`, `_all_pct.json` y `_report_legacy.json`. Estos snapshots conservan la semantica anterior; no compararlos directamente con una medicion nueva sin separar el cambio siguiente.

### Correccion de medicion, sin atribuirla a decompilacion nueva

El inicializador estatico de gc_interface (2.456 B) ya era exacto. Su asociacion de nombres estaba configurada antes de la tanda. Pruebas aisladas con los mismos objetos muestran que objdiff 3.7.0 ignora esa asociacion; 3.8.0 la aplica al cargar el proyecto. Abrir solo los dos objetos tampoco proporciona el contexto de nombres, incluso con 3.8.0. Diagnostico reproducible: `scratchpad/ai_gcinterface_report_diag/DIAGNOSIS.md`.

- Se actualiza unicamente `config.objdiff_tag` de 3.7.0 a 3.8.0; ninja regenera y descarga la herramienta por su procedimiento normal. No se cambian compiladores, splits, asociaciones ni opciones de comparacion.
- `scripts/measure.py` usa `diff -p ROOT -u NAME` solo para unidades con asociaciones preexistentes. Conserva la via directa y los fallbacks para las demas. La opcion `--raw` reproduce la semantica anterior para comparaciones historicas. py_compile y pruebas filtradas pasan.
- Comparacion global de informes: solo gc_interface cambia sus bytes/funciones exactas, de 3.696/6.152 B y 30/31 a 6.152/6.152 B y 31/31. **+2.456 B y +1 funcion son exclusivamente correccion contable**, no nuevos cierres.
- Repeticion completa de las 589 unidades con `--raw`: snapshot identico al posterior a las fuentes. Repeticion con el contexto corregido: exactamente una unidad cambia, gc_interface, con el delta previsto; ninguna perdida.
- Nuevo informe oficial: **3.769.052 / 3.946.048 B, 95,5146 % matched**, **18.115 / 18.432 funciones**. Medicion independiente corregida: 3.769.052 / 3.946.204 B, 95,5108 %, 18.115 funciones. La diferencia de denominadores preexistente sigue siendo 156 B.
- Nueva referencia para la siguiente tanda: `scratchpad/resume_20260904_phase6_mapped_measure.json`. Control historico: `_raw_check.json`; log de migracion: `_measurement_build.log`. Ambas herramientas verificadas en 3.8.0.
- SHA-1 conservado: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. HEAD sigue en ef92ca4e; git diff --check sin errores, solo avisos CRLF. Datos 28,760845 % y codigo enlazado 7,556523 %: no confundir match de instrucciones con reconstruccion/enlace completos.

## Septima tanda

Referencia revalidada: informe oficial 3.769.052 B / 18.115 funciones exactas;
snapshot independiente `scratchpad/resume_20260904_phase6_mapped_measure.json`.
La correccion contable de la sexta tanda ya esta incorporada, no es avance de
esta. Reparto: filesys/DVD y despues zSim; LibSN y log10f; printf; principal
DebuggerDriver y auditoria de integracion. HEAD ef92ca4e, sin commits ni cambios
de flags, compiladores configurados, splits o asociaciones.

### Reconstrucciones y auditorias

- DebuggerDriver: DBWrite (608 B) y EXI2_Init (188 B) alcanzan el 100 %. DBWrite
  conserva sus cuatro palabras de comando en el caller y pasa sus direcciones
  a los auxiliares inline: slots 0x50/0x4c/0x44/0x3c, status0x54, frame0x78.
  Separar offset, longitud alineada y direccion recupera r3 y el orden; declarar
  la longitud antes de addr restituye r24/r25. EXI2_Init conserva zero/cmd como
  argumentos de valor del helper inline y sus palabras en el caller; recupera
  lis r4/li r3/addi r0 y slots0x20/0x1c/0x10, frame0x30. Sin cuerpos auxiliares
  extra ni instrucciones ensamblador. 31 funciones, cero regresiones; datos y
  relocs intactos; 154 referencias, cuatro literales y 17 ramas de los cierres
  verificados. Unidad: 5.184/5.988 B, 28/31 funciones. Evidencias
  `scratchpad/phase7_debugger_notes.md`, `_before.json`, `_after.json` y
  `python scratchpad/phase5_debugger_audit.py --phase7`.
- SDK, descartes nuevos: cambiar modelos de scheduling no resuelve los
  epilogos; varios regresan 13/21/25 funciones. Un pragma scheduling-off al
  final afecta toda la funcion, no solo el retorno; asm vacio MW no cambia
  nada. GC/1.2.5 sigue cerrando dos epilogos, pero regresa tres del grupo DB;
  no se aplica ni se inventa un split. El helper latch de Poll y el data
  volatile se revierten. Quedan Poll264 B, WriteN432 B y SetExiCallback108 B.
- LibSN: DSIHandler (628 B) y SNDVDEmuInitDSIHandler (124 B) cierran sndvd.
  Contexto local r29 y restriccion vacia tras srr0; orden de inicializacion
  audioCfg/error/currOffset/baseOffset/lidState/emuState. Una variante inicial
  marcaba 100 % pero intercambiaba dos destinos de stores de cero: la auditoria
  de relocs lo detecto y se corrigio. Resultado final: 59 relocs y 29 ramas
  correctas; los otros 93 cuerpos de las cuatro unidades y 18 objetos de datos
  intactos; cadena DVD original verificada. Ensayos VM/VMBASE/steering
  revertidos al estado local previo. `scratchpad/libsn7_audio_resume_20260904_notes.md`
  y `python scratchpad/pathevents6_audio_resume_20260904_audit.py --libsn7`.
- zSim: Sim::Entity::Entity (924 B) cierra con un binding corto de owner/r3
  dentro del initializer IEntity, sin tocar headers. 571 funciones emitidas:
  otras 570 identicas; 334 datos y 6.735 referencias intactos; las 34 referencias
  externas del constructor coinciden con target. zSim: 95.836/96.400 B,
  401 funciones exactas; queda OnManageTime564 B, sin cambios. Snapshots/notas
  `scratchpad/ai_zsim_round7_*`, auditoria `_audit.py`. Control de las 33
  SourceLists: 15.493 simbolos, solo Entity mejora y ninguno retrocede
  (`scratchpad/resume_20260904_phase7_all_pct.json`).
- log10f (268 B): resuelto el antiguo conflicto entre sesgo SDA, orden de
  stores y posiciones de pila. Un frame explicito agrupa xword/sp+8 y la union
  double volatile/sp+0x10; temporales fr0/r0/fr13 y restricciones vacias
  recuperan GET/SET, registros y dependencias. Conversion por palabras low/high
  y resta del sesgo 0x4330000080000000, sin instrucciones ensamblador escritas
  a mano. Objeto real 100 %, ocho relocs, siete ramas y los 36 B originales de
  constantes/sesgo/relleno verificados. Siete datos nombrados intactos; solo
  desaparece el pool privado de sesgo que sobraba. La receta requiere adaptar
  el frame de cada funcion, no copiarla a ciegas a otras libm. Se corrige tambien
  el comentario obsoleto de fdlibm.h, sin cambiar macros ni tipos. Evidencias
  `scratchpad/sf_log10_audio_resume_20260904_*` y auditoria anterior `--log10`.
- filesys y DVD: no hay nuevos cambios retenidos. Objetos recompilados identicos
  a sus baselines, excepto metadata de linea; preservadas las correcciones de
  la tanda anterior. Descartes en `scratchpad/ai_filesys_round7_notes.md` y
  `ai_dvd_round7_notes.md`.

- printf: **_vfiprintf_r (5.760 B) cerrado al 100 %**. Los keepalives de memoria
  de PAD diferencian blanks (bucle y cola) de zeroes (solo bucle), recuperando
  prioridades de direcciones y registros. Los alias fp/ap, las lecturas de byte
  r0 y las vidas del parser completan el cuerpo. _vfprintf_r sube de 99,51566 %
  a **99,986946 %**, 6.128/6.128 B: solo estan intercambiados `mr r29,r6` y
  `mr r22,r5` del prologo. NO contar esos 6.128 B como cerrados. Inicializar ap
  antes arregla esos mr, pero cambia la preparacion del frame tras localeconv;
  K&R y las barreras ensayadas no resuelven ambas cosas. Se conservan seis
  funciones sin regresiones, 14 literales, 195 referencias con tipo/addend,
  cuatro tablas, 712 ramas y seis objetos estaticos, incluida la relocacion
  de pch_pointer a pch. Evidencias `scratchpad/vfprintf_resume7_20260904_notes.md`,
  los snapshots resume7 de ambas variantes y auditorias
  `python scratchpad/vfprintf_resume5_audit.py 7` / `vfprintf_resume7_strict.py`.

### Verificacion del septimo hito

| Funcion nueva exacta | Bytes |
| --- | ---: |
| DBWrite | 608 |
| EXI2_Init | 188 |
| DSIHandler | 628 |
| SNDVDEmuInitDSIHandler | 124 |
| Sim::Entity::Entity | 924 |
| log10f | 268 |
| _vfiprintf_r | 5.760 |
| Total: siete funciones | 8.500 |

- ninja: 42/42 pasos correctos, incluidos LINK, DOL y CHECK; main.dol OK.
  El comentario de fdlibm.h recompila sus consumidores: se verifico que sus
  macros/tipos/codigo son identicos al quitar comentarios. Persisten avisos
  preexistentes de prototipos VM, headers del juego y simbolo PPCMtdec duplicado.
- Informe oficial: **3.777.552 / 3.946.048 B, 95,73 % matched**,
  **18.122 / 18.432 funciones exactas**. Datos 369.792/1.285.748 B (28,760845 %)
  y codigo enlazado 298.184/3.946.048 B (7,556523 %) sin cambios. El hash correcto
  no implica que las unidades NonMatching esten enlazadas desde estas fuentes.
- Medicion independiente: 3.777.552/3.946.204 B, **95,7262 %**, 18.122 exactas.
  Las 589 unidades mantienen inventario y denominadores; cinco aumentan codigo
  exacto, ninguna pierde bytes ni funciones. Diferencia exacta contra la sexta
  tanda corregida: **+8.500 B y +7 funciones**. No hay cambios contables nuevos.
- Control por funcion repetido despues de ninja: 15.493 simbolos de 33
  SourceLists, solo Entity mejora, ninguna regresion. Auditorias individuales
  de las bibliotecas y SDK detalladas arriba, repetidas por el principal.
- Acumulado de siete tandas: **+38.008 B y +71 funciones nuevas exactas**;
  la correccion de medicion de la sexta tanda (+2.456 B/+1) sigue separada.
- SHA-1: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. HEAD ef92ca4e, sin
  commits/staging. git diff --check correcto, con los avisos CRLF habituales.
- Evidencias globales: `scratchpad/resume_20260904_phase7_build.log`,
  `_measure.json` y `_all_pct.json`. Referencia anterior comparable:
  `scratchpad/resume_20260904_phase6_mapped_measure.json`. Usar phase7_measure
  como baseline corregido de la proxima tanda, con objdiff 3.8.0.

### Diagnostico adicional: alcance real de MetroTRK

`scratchpad/phase7_metrotrk_diagnosis.md` prepara las cinco funciones publicas
de 108 B con ABI identificada y separa los despachadores/contextos que requieren
reconstruccion ensamblador por sus registros implicitos. Sus doce entradas PC*
son ranuras autoparcheables de ocho bytes: preservar su primer word y el blr
de continuacion; no contarlo como doce funciones vacias adicionales.

Auditoria independiente `phase7_metrotrk_census.py`: el ELF original tiene
63 nombres NOTYPE sin tamanos; el split reconstruye 107 FUNC/12.136 B, mientras
report usa 101/11.992 B. Excluye siete gap_* (656 B), pero incorpora
lbl_8030FFDC (512 B). Tres de esos gaps contienen 608 B de codigo real. No se
cambia el censo ni se suman bytes por reclasificacion. El futuro 100 % requiere
cubrir ese codigo y las convenciones reales, no limitarse al porcentaje.

## Octava tanda

Referencia: septimo hito revalidado, 3.777.552 B / 18.122 funciones exactas.
Tres agentes reutilizados: printf y despues SDK; Game Code; tan/rem-pio2f.
Principal: rem-pio2 double, reconstruccion de logf y auditoria global.

### Cierres nuevos y parciales conservados

- **_vfprintf_r: 6.128 B, 100 %.** Inicializar ap temprano y asignar fmt despues
  de state=0 recupera ambos mr y mantiene las preparaciones exactas tras
  localeconv. El RTL explica por que importa cual de los argumentos sigue vivo
  antes de asignar su alias. No se anaden barreras ni instrucciones ensamblador.
  _vfiprintf_r y los cuatro auxiliares siguen exactos. Auditorias: seis funciones,
  14 literales, 195 referencias con tipo/addend, 712 ramas, cuatro tablas y seis
  objetos estaticos; sin regresiones. Notas `scratchpad/vfprintf_resume8_20260904_notes.md`,
  snapshots resume8 y comandos `vfprintf_resume5_audit.py 8` / `vfprintf_resume7_strict.py 8`.
- **LGWheels::PlayFrontalCollisionForce: 440 B, 100 %.** Alias local de la
  magnitud promovida a unsigned int en r27; conserva el parametro uchar y su
  rango. Sin barreras ni ASM de instrucciones. Las 188 funciones restantes,
  191 datos y 2.058 referencias de zPlatform permanecen identicos; las nueve
  referencias externas del cierre y el literal OSReport coinciden con ELF.
  zPlatform: 27.620/29.380 B, 134 funciones exactas. Evidencias
  `scratchpad/ai_platform_round8_*`, incluida auditoria completa.
- **e_rem_pio2: 84,6129 -> 93,57143 %.** Recuperada llamada original a fabs
  mediante alias extern local que evita el builtin; restaura GPR/CR y frame0x48.
  Tablas en .sdata2 y escalares SDA inmutables. 868/868 B, todavia parcial.
- **k_rem_pio2: 93,46154 -> 95,02747 %.** Tablas originales .sdata2, escalares
  inmutables y operandos multiply en orden original. 2.184/2.208 B; pendiente
  sesgo de conversion nativa y reparto de registros. Retirado comentario que
  declaraba imposible superar el porcentaje antiguo.
- **logf: fuente antes vacia, ahora 90,09375 %.** C reconstruido desde target:
  normalizacion, casos especiales y dos caminos del polinomio. 640/652 B;
  tres lis extra por sesgo .rodata frente a .sdata. Las 53 operaciones FP son
  identicas en orden y operandos; los 20 destinos de control coinciden y el
  frame0x18 es correcto. Revision independiente de las 160/163 instrucciones,
  constantes, signed zero, negativos, subnormales e Inf/NaN, sin errores hallados.
  Las pruebas de frame manual y bindings quedaban en 74--86 %, revertidas;
  se conserva C simple, macros fdlibm y casts nativos, sin ASM adicional.
- Auditoria conjunta de los tres anteriores: 37 objetos/672 B y sus secciones,
  83 referencias/tipos/addends y los tres sesgos originales verificados. Solo
  se normaliza la colocacion pendiente de esos sesgos, que sigue documentada
  como diferencia. `scratchpad/phase8_libm_{notes.md,audit.py,diff.py}` y
  snapshots `phase8_{e_rem_pio2,k_rem_pio2,sf_log}_{before,after}.json`.
- **kf_rem_pio2: 93,51376 -> 94,02752 %.** Corregido error semantico: q0==0
  requiere >>8, no >>7; instruccion original verificada. init_jk en .sdata2 y
  operandos multiply exactos. Nueve objetos/84 B, 31 referencias y 80 destinos
  de control auditados. PIo2 SDA regresaba CR/frame: revertida, sigue pendiente
  junto al sesgo. kf_tan vuelve integramente al baseline97,706764 %; se descarto
  la prueba de fctiwz aislado y no se conserva ASM nuevo. Evidencias
  `scratchpad/kernels8_audio_resume_20260904_{notes.md,audit.py}` y snapshots.
- SDK: nuevos helpers inline y vidas de retorno no resuelven los tres casos;
  31 funciones, datos, 154 refs y cuatro literales identicos al baseline.
  Game Code: ensayos STICH_PlayBack, GTrigger y UnlockPalette revertidos;
  ActualReadJoystickData solo leido. Notas SDK `phase8_debugger_notes.md` y
  notas Game Code `ai_platform_round8_notes.md`. Ningun parcial experimental
  de esos ensayos queda mezclado con el cierre validado.

### Verificacion del octavo hito

- ninja **14/14** correcto, incluido REPORT/PROGRESS. Esta tanda no necesitaba
  volver a enlazar el DOL: los objetos NonMatching siguen fuera del enlace de
  fuentes. SHA-1 del main.dol comprobado directamente y sin cambios:
  `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. Persisten avisos preexistentes.
- Informe oficial: **3.784.120 / 3.946.048 B, 95,896454 % matched**,
  **18.124 / 18.432 funciones exactas**. Datos369.792/1.285.748 B y
  codigo enlazado298.184/3.946.048 B permanecen iguales. No confundir el hash
  correcto ni los cierres de instrucciones con un enlace completo desde C.
- Medicion independiente: **3.784.120 / 3.946.204 B, 95,8927 %**, 18.124 exactas.
  589 unidades, inventarios y denominadores identicos a phase7; solo zPlatform
  y vfprintf ganan codigo exacto; ninguna pierde bytes ni funciones.
- Control de las 33 SourceLists: 15.493 simbolos, solo PlayFrontalCollisionForce
  mejora, cero regresiones. Snapshots root libm renovados despues de ninja sin
  recompilar; auditorias de todas las areas repetidas por el principal.
- Delta real: **+6.568 B / +2 funciones**. Las cuatro mejoras parciales no
  se suman a este delta. Acumulado ocho tandas: **+44.576 B / +73 funciones**;
  la correccion contable de fase6 (+2.456 B/+1) sigue separada, no se recuenta.
- HEAD ef92ca4e, sin commits/staging, flags, headers, splits ni configuracion
  cambiados en esta tanda. git diff --check de los seis fuentes correcto.
- Nueva referencia: `scratchpad/resume_20260904_phase8_measure.json`;
  evidencia global `_build.log` y `_all_pct.json`. Mantener objdiff3.8.0 y la
  semantica corregida de medicion ya incorporada en fase6.

## Novena tanda

Referencia revalidada: octavo hito, 3.784.120 B y 18.124 funciones exactas.
Reparto: cinco hojas publicas MetroTRK; logf/expm1f; exp/expf; principal
ef_rem_pio2, revisiones cruzadas y validacion global. El turno anterior fue
avance real, no bloqueo. La novena tanda tampoco tiene bloqueos externos.

### Cierres nuevos: cinco hojas, no todo MetroTRK

| Funcion publica | Simbolo tecnico existente | Bytes exactos |
| --- | --- | ---: |
| snIsSNTDEV | fn_803104B4 | 16 |
| snProfSetRange | fn_803119E8 | 24 |
| snProfSetFlagValue | fn_80311A00 | 16 |
| snProfSetFlags | fn_80311A10 | 24 |
| snProfClrFlags | fn_80311A28 | 28 |
| Total | Cinco funciones | 108 |

El cuerpo original de estas hojas fue escrito en ensamblador: sus direcciones
usan HI/LO+ori, no HA/LO+addi de C. Se conservan explicitamente solo esas dos
instrucciones de carga de direccion por hoja; campos, operaciones y retornos
siguen en C. Los nombres publicos son alias NOTYPE originales, colocados en la
misma direccion que los fn_* del split, sin duplicar cuerpos ni crear mappings.
Preservados retornos no intuitivos: SetFlags devuelve el argumento y ClrFlags
su complemento, no el valor final de flags. El header externo no emite datos.

Auditoria `scratchpad/ai_metrotrk_round9_audit.py`: inventario107 conservado,
cero regresiones, diez referencias con tipo/objetivo/addend/offset, cinco alias,
los108 bytes con relocaciones resueltas identicos al ELF,150 ejecuciones de
hojas con retornos/campos/clobbers verificados. Sin datos ni padding emitidos.
Notas y snapshots `scratchpad/ai_metrotrk_round9_*`. Sigue pendiente el resto
de MetroTRK, incluidos ABI privados, vectores y ranuras PC* autoparcheables.

### Cuatro cuerpos matematicos antes vacios

| Funcion | Match parcial | Bytes target/fuente |
| --- | ---: | ---: |
| exp | 96,57233 % | 636/640 |
| expf | 93,52518 % | 556/560 |
| expm1f | 91,56716 % | 804/808 |
| __ieee754_rem_pio2f | 90,3066 % | 848/844 |

Total2.844 B originales con cuerpo reconstruido, pero **cero cierres exactos**
en estas cuatro funciones. Sesgos de conversion nativa en .rodata frente a
.sdata y su efecto en registros/scheduling siguen pendientes. Marcos correctos.

- exp/expf reconstruidos desde ASM y ELF, sin copiar fuente externa: clasificacion
  NaN/Inf, overflow/underflow, reduccion ln2, polinomio y escalado subnormal.
  Preservadas cargas de constantes Inf/zero, sin introducir operaciones que
  levantasen excepciones adicionales. Cast unsigned antes de exponent shifts
  negativos. 34 objetos/240 B,48 refs tipadas y42 ramas auditados;4288 entradas
  diferenciales, resultados y50.329 operaciones FP coincidentes. Principal
  reviso ambos fuentes y las159/139 instrucciones originales de forma independiente.
  `scratchpad/phase9_exp_{notes.md,audit.py,trace.py}` y snapshots phase9.
- expm1f:22 objetos/88 B,31 referencias,25.566 entradas diferenciales, todos los
  bits no-NaN y trazas de operandos/operaciones iguales. Cobertura201/201 y202/202
  instrucciones y52 resultados de rama por lado. Separar t=x*invln2 de las dos
  ramas conserva el redondeo original; escribir productos completos en cada
  rama introducia una FMA incorrecta y se revirtio. Retenido solo binding k/r10
  comprobado por ejecucion, sin ASM nuevo. logf queda integramente igual al
  baseline90,09375 %, incluidos todos sus simbolos/datos/instrucciones.
  `scratchpad/sf9_audio_resume_20260904_{notes.md,audit.py}` y snapshots.
- ef_rem_pio2f: adaptacion de C double comprobada contra212 instrucciones, no
  mera reduccion de tipos. Dos tablas originales de chunks8bit, mascaras de
  mantisa, shifts23, cancelaciones>8/>25, e0=ilogb(|x|)-7 y two8=256. Corregido
  inmediatamente un shift20 residual de la primera transformacion; pruebas de
  bindings y/r31 y t/fr9 descartadas por registros destructivos. C ordinario
  conservado.12 objetos/960 B,25 refs y24 ramas auditadas, incluidas dos colas
  compartidas que aun deben escribir y[1]. Revision independiente sin defectos;
 100.004 descomposiciones grandes verifican identidad y limites de conversion.
  `scratchpad/phase9_libm_notes.md`, snapshots phase9_ef_rem_pio2 y
  `python scratchpad/phase8_libm_audit.py --phase9`.
- Los interpretes diferenciales son diagnosticos acotados, no emulacion completa
  de GameCube ni pruebas exhaustivas de todos los bits/FPSCR/payloads NaN.
  Se complementan con lectura independiente del ASM y de las operaciones.

### Diagnostico contable nuevo, sin sumar progreso

`scratchpad/phase9_uncategorized_census.py` identifica las tres funciones del
informe fuera de categorias. `asd2` contiene dos wrappers ctor/dtor de44 B que
**ya genera** gc_interface.cpp: sus88 bytes, llamadas al helper original y
punteros .ctors/.dtors coinciden al resolver relocaciones. Pero asd2 no tiene
base_path ni fuente configurada, y no se asocia con ese objeto para comparar.
Esto permite una futura correccion de asociacion/limite de unidad, no afirmar
que se han decompilado88 B nuevos. No se cambia configuracion ni metricas ahora.

La tercera entrada es pad_00_8000348C_init,20 bytes cero: no pertenece a ninguna
seccion original del ELF, sino al relleno de alineacion de la conversion DOL.
Su ubicacion y ceros se verifican en el DOL con SHA-1 correcto. Preservar esos
bytes; no presentarlos como una funcion nueva decompilada. Este diagnostico se
suma al censo de MetroTRK y no lo sustituye ni reduce el alcance pendiente.

### Verificacion del noveno hito

- ninja8/8, seguido de4/4 tras los comentarios finales de exp/expf; todos los
  scopes ya liberados y ultima medicion repetida despues de la compilacion
  final. Los objetos NonMatching siguen fuera del enlace desde fuentes: no se
  requirio relink del DOL. Hash comprobado:9619ba57c9919f95f7f2ac951a2166a3517f91e3.
- Informe oficial:**3.784.228/3.946.048 B,95,899185 %**, **18.129/18.432 funciones
  exactas**. Fuzzy99,30844 %. Datos369.792/1.285.748 B y enlazado298.184 B iguales.
- Independiente:**3.784.228/3.946.204 B,95,8954 %**,18.129 exactas.589 unidades con
  mismo inventario/denominadores, solo MetroTRK gana108 B y5 funciones; ninguna
  regresion.33 SourceLists/15.493 simbolos, cero cambios por funcion.
- Todas las auditorias se repiten por el principal; diffs libm renovados contra
  los objetos reales despues de ninja, sin recompilar. git diff --check correcto.
- Acumulado nueve tandas:**+44.684 B/+78 funciones realmente decompiladas**.
  La correccion contable de fase6 sigue separada; la de asd2 aun no se aplica.
- HEAD ef92ca4e, sin commits/staging, flags, headers, splits ni configuracion
  cambiados. Nueva referencia `scratchpad/resume_20260904_phase9_measure.json`;
  evidencias `_build.log`, `_build_final.log` y `_all_pct.json`.

## Frentes previstos tras la novena tanda

El objetivo del 100 % sigue pendiente. El informe oficial deja **161.820 B y
303 funciones**. Ademas hay que resolver las clasificaciones de MetroTRK
documentadas arriba y cubrir el codigo ejecutable excluido de ese resumen.
No confundir instrucciones exactas con datos/enlazado completos.

Prioridades con evidencia nueva:

- STD deja 13.028 B y once funciones. Nueve ya tienen C parcial: kf_tan.c
  (532 B, 97,706764 %), kf_rem_pio2.c (2.180 B, 94,02752 %), e_rem_pio2.c
  (868 B, 93,57143 %), k_rem_pio2.c (2.184 B, 95,02747 %) y sf_log.c
  (640 B, 90,09375 %), junto con las cuatro reconstrucciones de la novena tanda,
  total9.248 B. Logf no tiene el conflicto fctiwz de tan
  y rem: sus tres conversiones son int->float; continuar desde el C sencillo
  auditado y revisar la nueva planificacion antes de repetir bindings viejos.
  La receta de log10f requiere adaptar cada frame, no copiarla ciegamente.
- Los dos fuentes restantes de STD siguen vacios: e_pow y ef_pow, total3.780 B.
  Reconstruirlos tambien; no
  limitar el trabajo a ultimos operandos de las funciones casi exactas.
- MetroTRK: conservar las cinco hojas publicas ya exactas; continuar con otros
  limites/callers confirmados y reconstruir ensamblador
  de contexto y ranuras parcheables, sin reducir el alcance a funciones faciles
  o falsas fronteras del censo. Diagnostico reproducible arriba.
- Contabilidad: resolver la asociacion de los dos wrappers de asd2 ya emitidos
  por gc_interface, preservando sus88 B y tablas originales y separando esa
  futura correccion de nuevos cierres. Revisar tambien el padding .init20 B,
  sin eliminar codigo ejecutable ni fabricar funciones para alcanzar el100 %.
- Resto: filesys dos funciones/568 B; DebuggerDriver tres/804 B (Poll,
  WriteN, SetExiCallback); OnManageTime564 B para completar zSim; steering,
  Path y DVD. Los dos ultimos SDK difieren solo en epilogos, y cambiar toda
  la unidad a GC/1.2.5 regresa funciones del grupo DB.

gc_driver, pathsnd, sndvd, log10f y ambas variantes de printf estan cerrados.
Revisar snapshots actuales antes de usar censos antiguos. Mantener auditoria de
literales, vtables y relocs aun cuando el porcentaje de instrucciones sea 100 %.

Estado: nueve tandas integradas, fuentes y medicion verificadas, sin commits;
objdiff 3.8.0, sin nuevos cambios de configuracion. Objetivo global aun no
alcanzado; todos los agentes y compilaciones de esta tanda han terminado.

## Decima tanda integrada

### Seis cierres nuevos, separados de la correccion de medicion

| Funcion | Unidad | Bytes nuevos exactos |
| --- | --- | ---: |
| FEngine::ProcessPadsForPackage | zFEng | 3.080 |
| SNDebugBoot | MetroTRK | 20 |
| Handler de desconexion fn_8030F460 | MetroTRK | 20 |
| SNDI_getb | sgetdata | 128 |
| SNDAEMSI_timerupdate | saemstimupdt | 120 |
| atexit | atexit | 152 |
| Total realmente decompilado | Seis funciones | 3.520 |

- zFEng: el analisis RTL explica por que intercambiar Held/Released tambien
  intercambiaba registros en toda la funcion. Adelantar Held y mantener vivos
  Held/JoyMask despues de acumularlos conserva sus prioridades sin instrucciones
  nuevas. Solo cambian los dos li pendientes. Auditoria499 funciones emitidas,
  343 parejas, cero regresiones;101 ramas,72 referencias y224 B de tablas
  originales verificados. Datos y .ctors originales preservados.
- MetroTRK: dos hojas mas con40 B exactos; total148 B y siete funciones. Se
  conservan alias NOTYPE y ABI real, sin fabricar funciones ni gaps. Ademas
  SNInitInterrupts queda reconstruida al77,72222 %: cuerpo44 B exacto y prologo/
  epilogo aun diferentes. Auditoria107funciones,20relocs,320casos de registros,
  memoria, LR y llamadas SDK. La entrada de desconexion se confirma en la tabla
  original de comandos, no se deduce solo de un nombre.
- sgetdata: i=numbytes con if(i--!=0) reproduce el test del contador antes de
  formar el resto. Aritmetica unsigned conserva el wrapping32bit de campos4B;
  extension de signo1/2/3B y limites de lectura intactos.128 B originales,
  diez ramas locales resueltas,87.005casos y32/32instrucciones verificados.
- saemstimupdt: no era una llamada C ordinaria. saems instala codigo cargado
  del banco AEMS; el puente original preserva r31, LR y el siguiente nodo en
  r0. Solo ese puente40B se expresa en ASM con todas las restricciones y
  clobbers; bucle, contador y frame siguen C++.120 B resueltos identicos al ELF,
  tres referencias/cuatro ramas y2.400callbacks adversariales, incluidos nodos
  que se eliminan a si mismos. Revision independiente sin hallazgos.
- atexit: fuente antes inexistente, ahora C ordinario con sys/reent.h existente.
  Registra32callbacks por tabla y enlaza una nueva de0x88B al llenarse. Preserva
  fallo de malloc y recarga de _impure_ptr despues de malloc, incluso si este
  cambia el registro de reentrancia.152 B originales, tres referencias, cuatro
  ramas y1.680casos de estado/ABI. Su entrada de configuracion pasa del fallback
  cflags_game al conjunto existente cflags_libc: el primero fuerza una direccion
  SDA a r31 y emite156B, el segundo reproduce los152B. No se cambian conjuntos
  compartidos de flags ni headers. No se copia fuente externa.

Evidencias: phase10_feng_*, ai_metrotrk_round10_*, phase10_sgetdata_*,
phase10_saemstimupdt_*, phase10_atexit_after.json y phase10_atexit_audit*.

### Dos potencias reconstruidas, todavia parciales

| Funcion | Match parcial | Bytes target/fuente |
| --- | ---: | ---: |
| pow | 98,25 % | 2.000/2.000 |
| powf | 96,89663 % | 1.780/1.784 |

Ambos fuentes estaban vacios. Ahora las once funciones pendientes de STD tienen
cuerpo C; las dos potencias aportan3.780B originales reconstruidos, pero **cero
bytes/funciones nuevos exactos**. No se confunde esta mejora con los seis cierres.

Reconstruidos desde instrucciones/ELF: NaN y exponente cero en orden original,
bases negativas y paridad completa de exponente, reciprocos y raices, ±0/Inf,
subnormales, cortes de mantisa, polinomios y escalado. Preservado incluso el
inusual x-1 con signo en la rama de exponente enorme cerca de |x|=1. Arrays
.sdata2 y constantes originales verificadas. Los sesgos nativos aun van a
.rodata y afectan a registros/planificacion; no se parchea el objeto generado.

- pow:36objetos/312B, sesgo8B,53referencias,73aristas y una inversion de rama
  justificada por carga inmutable.19.180pares de entradas,500/500instrucciones
  en ambos lados y126/125resultados de rama. Sin bindings retenidos.
- powf:35objetos/152B, sesgo8B,49referencias,64aristas.32.814pares,445/445 y
  446/446instrucciones,105resultados de rama por lado. Solo binding local del
  signo a r9; aliases declarativos conservan llamadas fabsf/sqrtf.
- Resultados bit a bit (NaN por clase) y multiconjuntos de operaciones FP y
  entradas de llamadas coinciden. Son interpretes acotados, no emuladores
  completos ni pruebas de FPSCR, payloads NaN o todas las combinaciones.
- Principal reviso fuentes, operaciones y casos particulares y repitio ambas
  auditorias. Notas ai_pow_round10_notes.md y pow10_audio_resume_20260904_notes.md.

srender tuvo una subtanda acotada sin cambios retenidos: fuente y JSON completo
iguales al baseline69,46809 %. Sus lecturas eliminadas por CSE siguen pendientes.
DebuggerDriver tambien queda integramente igual a la novena tanda.

### Contabilidad y enlace real

La asociacion de asd2 con el objeto ya compilado de gc_interface recupera
**88B/+2funciones y8B de datos existentes**, no decompilacion nueva. El hook
persistente de configure es especifico de GOWE69, comparte solo la fuente de
comparacion y conserva target, inventario, categorias y enlace. Los wrappers
son100 % al resolver sus llamadas y punteros .ctors/.dtors. El helper2456B es
solo una referencia indefinida en asd2; no se duplica su conteo. Configuracion
idempotente, con revision independiente. El padding20B permanece sin cambios.

Tras preflight completo de secciones/aliases/relocs, atexit, sgetdata y
saemstimupdt pasan a Matching. Sus **400B ya se enlazan desde los objetos fuente**,
sustituyendo los tres originales, no duplicandolos. El SHA-1 completo del DOL
sigue siendo9619ba57c9919f95f7f2ac951a2166a3517f91e3. Esto valida esas tres
promociones, no implica que todo el juego este enlazado desde C.

### Verificacion del decimo hito

- ninja correcto: compilacion, enlace, conversion DOL y CHECK SHA. Persiste el
  aviso de simbolo duplicado PPCMtdec entre unidades ajenas a esta tanda; tambien
  los avisos previos de FEng por -1 a unsigned. No errores. ninja -n solo PROGRESS.
- Oficial:**3.787.836/3.946.048B,95,99062 %**, **18.137/18.432 funciones exactas**.
  Fuzzy99,411804 %. Datos369.800/1.285.748B. Enlazado298.584B (7,566659 %).
- Independiente:**3.787.836/3.946.204B,95,9868 %**,18.137funciones exactas;
  589unidades, mismos inventarios y denominadores. Delta total+3.608B/+8:
  **+3.520B/+6 codigo nuevo** y **+88B/+2 reconocimiento de codigo previo**.
- 33SourceLists/15.493simbolos: solo mejora ProcessPadsForPackage, ninguna
  regresion. Las diferencias completas post-ninja se cotejan con los objetos
  auditados, omitiendo solo numeros de linea DWARF desplazados por comentarios
  finales (powf,+6lineas). Censos, datos y referencias no se validan solo por
  porcentaje.
- Acumulado diez tandas:**+48.204B/+84funciones realmente decompiladas**;
  correcciones contables fase6+fase10 separadas:+2.544B/+3funciones.
- HEAD ef92ca4e, sin commits ni staging. Configuracion cambiada solo para asd2,
  flags de la nueva atexit y las tres promociones verificadas; sin cambios de
  flags compartidos, headers o splits.
- Referencia siguiente: scratchpad/resume_20260904_phase10_measure.json y
  _all_pct.json; log _build.log. Reconciliacion: phase10_refresh_and_check.py.

## Proximos frentes despues de la decima tanda

El objetivo100 % permanece activo e incompleto: quedan **158.212B y295funciones**
en el informe oficial. Sigue siendo necesario corregir/documentar el censo de
MetroTRK y su codigo ejecutable excluido; no eliminarlo ni convertir padding
en funciones para aparentar el100 %.

- STD: once parciales/13.028B, ahora todas con cuerpo. Pow/powf abren nuevos
  casos del sesgo SDA y lifetimes; conservar sus casos diferenciales y la rama
  inusual de exponente enorme. No repetir bindings globales destructivos.
- MetroTRK: siete hojas cerradas y SNInit parcial; pasar a mas handlers con ABI
  demostrado y a sus bloques de contexto. No aplicar C normal a ranuras PC*
  que se autoparchean ni suponer preservacion de registros no demostrada.
- Sonido: srender188B conserva pendiente la recarga entre dos filtros. La
  evidencia del puente AEMS puede servir para otros callbacks de bancos, pero
  cada caller necesita su auditoria propia. No copiar el ASM por similitud.
- Runtime ausente: sn_malloc120B/2funciones, ppc2D2, crt2D1 y pro2D4 siguen como
  frentes reales; antes de asignarlos confirmar sus ABI y flags originales.
  El caso atexit demuestra que el fallback de misc no es prueba de sus flags.
- Game:93funciones/105.456B; zFEng queda en70144/71460B,341/343exactas.
  SDK: tres funciones/804B sin cambio. Resto de Library:187funciones/38.904B,
  incluyendo los bloques runtime citados; repartir unidades sin solapamiento.

Estado: diez tandas integradas; seis cierres nuevos, tres promociones con SHA
correcto, dos potencias parciales y contabilidad separada. Agentes y builds de
esta tanda terminados. Objetivo global todavia no alcanzado.

## Undecima tanda: runtime ausente y mejora de exp

Se partio de la referencia real de fase10: 3.787.836 B y 18.137 funciones
exactas. Se conservaron todas las modificaciones locales. Los tres agentes
trabajaron en unidades separadas; root reconstruyo sn_malloc y centralizo
configuracion, promociones y enlace. No se alteraron headers, flags compartidos,
splits ni el censo para reducir pendientes.

### Diez cierres nuevos, 772 bytes exactos

| Unidad | Funciones cerradas | Bytes | Naturaleza |
| --- | --- | ---: | --- |
| crt2D1 | __do_global_ctors, __main | 200 | C, nueva fuente |
| pro2D4 | __save_gpr, __restore_gpr | 152 | Primitivas ABI en ensamblador |
| sn_malloc | malloc, free | 120 | C y lectura del LR del llamador |
| ppc2D2 | PCwriteAsyncInit, InitReadCounts, CompleteAsync, PCrwAsyncNextPh | 300 | C, unidad todavia parcial |

Son **620 B en cuerpos C y 152 B de primitivas ABI**; estas ultimas no se
presentan como funciones decompiladas a C. No hay correccion contable en esta
tanda. Los cuatro objetos originales no tenian fuente asociada en la baseline;
los snapshots target-vs-target no se usaron como medida de progreso.

**crt2D1** conserva tabla y flag externos, cuenta explicita o sentinel -1,
recorrido inverso y flag escrito antes de llamar constructores. Se validaron
200 bytes reubicados contra split y ELF, ocho referencias/tipos/addends y
1.560 casos con reentrada, llamadas repetidas y clobbers de registros volatiles.
La tabla original tiene 47 constructores. No se duplican datos. ProDG requiere
la receta libc existente con -G0; el fallback de misc no era evidencia de flags
originales. El override conserva nombre de unidad, target y salida crt2D1.o.

**pro2D4** conserva las dos funciones y los 36 labels globales internos
_savegpr_14..31 / _restgpr_14..31. Las cuatro llamadas originales desde
__VMSwapPageIn y __VMPageReplacementLRU entran a +0x24/+0x28, no al inicio.
Auditoria de 152 bytes, inventario/tipos/offsets y 2.432 casos de todas las
entradas, incluida la instruccion blr final. Sin relocaciones, datos, padding,
prologo C o perdida de aliases. Usa la regla AS y fixup existentes.

**sn_malloc** contiene realmente trampas diagnosticas de ProDG, no el allocator
del juego. Ambas llaman OSPanic(0,0,mensaje,LR-4); los literales originales dicen
que una llamada externa a malloc/free es un error de biblioteca. OSPanic acaba
en PPCHalt, cuyo bucle infinito esta en el SDK local. El C conserva los epilogos
y el r3 opaco si un hook retornase. Solo mflr es una instruccion ASM; restricciones
vacias conservan lifetimes de argumentos y capturan r3, sin copiar el cuerpo
en ASM. Sin cambio de flags. Auditoria independiente: 120 bytes exactos, seis
referencias/tipos/addends, strings de 126/124 B y 4.096 casos con argumentos,
CR1eq de varargs, retorno hipotetico, wrap de LR-4 y preservacion de ABI.

**ppc2D2** mantiene las 22 funciones/2.448 B originales; cuatro exactas y dos
parciales emitidas, dieciseis aun pendientes. Los trece cuerpos FSasync tienen
proveniencia C/ProDG demostrada por STT_FILE y gcc2_compiled; los nueve primeros
simbolos runtime originales son NOTYPE/size0, no se presupuso el mismo origen.
El nuevo source override usa libc + -G0. InitReadCounts conserva min(total,0x1FC00),
resta, fase2, bloques de 1024 y resto; CompleteAsync conserva fases0/5/otras y
lectura post-call del error; el switch conserva fases1,2/4,5 y no-op restantes.
Los accesos volatile se justifican por el handler IRQ y los callbacks originales.
PCwriteAsyncInit es realmente return -1 en el original, no un stub inventado.

La auditoria resuelve las 49 relocaciones del objeto y coteja los 300 bytes
exactos con el ELF. __cvt_fp2unsigned queda en 94,44444 % (72/68 B): mismas
operaciones FP, comparacion, conversion y pila; falta un addi de direccion.
__va_arg queda en 58,491802 % (244/232 B), con lista EABI de 12 B reconstruida.
58.368 pares de estados cubren las 61/58 instrucciones, contadores/tipos de
8 bits y alineaciones; retorno y memoria coinciden. Las precondiciones de
buffers validos/no solapados y el modelo PPC32 se documentan en las notas.
No se cuenta ninguno de esos dos parciales como cierre. Los externs de helpers
locales aun ausentes impiden tratar esta unidad parcial como enlazable sola.

### exp: mejora parcial conservada

exp pasa de **96,57233 % a 97,98742 %**, siempre 636 B target / 640 B source.
Un bloque local al converger la reduccion mantiene hi/lo en fr10/fr7, calcula
x=hi-lo y retiene los tres valores con una restriccion vacia. No se cambia la
aritmetica, no queda fctiwz manual ni frame explicito, y sf_exp no se modifica.

Se demostro en RTL que el pseudo fpmem se asigna despues de todos los locales C:
una union adicional desplazaba los bitcasts a sp+24/frame32. Reemplazar todos
los bitcasts perdia las copias DI del backend y empeoraba a 82-88 %. Esos ensayos
se descartaron. El bias nativo sigue en .rodata, no .sdata, con un lis adicional.
No volver a aplicar la receta de log10f sin resolver primero ese frame compartido.

Auditoria final contra original Y baseline: 17 objetos/160 B y bias8 B identicos,
24 referencias, 21 ramas, frame24 y slots8/16; 14.744 entradas y 276.692
operaciones FP identicas. 13.621 conversiones verifican truncacion hacia cero.
El interprete no modela FPSCR ni payload NaN de hardware; se mantienen las
operaciones nativas y las rutas excepcionales. No es elegible para Matching.

### Integracion y evidencias de fase11

- crt2D1, pro2D4 y sn_malloc pasan a Matching tras revision independiente:
  **472 B adicionales enlazados desde fuentes**, tres unidades completas.
  ppc2D2 y exp permanecen NonMatching; no se cambian fronteras o categorias.
- ninja recompila los cinco objetos, enlaza y supera CHECK SHA. Persiste solo
  el aviso previo PPCMtdec duplicado; Missing configuration for asd2 corresponde
  a la asociacion compartida ya documentada, no a un nuevo fallo.
- SHA-1 del DOL: **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. ninja -n solo PROGRESS.
- Oficial: **3.788.608/3.946.048 B = 96,010185 %**, **18.147/18.432 funciones**.
  Fuzzy99,43694 %. Datos369.800/1.285.748 B, sin cambios. Enlazado299.056 B
  (7,5786204 %), 338/589 unidades completas; no significa que todo se enlace
  desde las reconstrucciones de funciones individuales.
- Medicion independiente: **3.788.608/3.946.204 B = 96,0064 %**, 18.147 exactas,
  589 unidades. La diferencia previa de 156 B entre denominadores se conserva.
- Acumulado once tandas: **+48.976 B/+94 cuerpos realmente reconstruidos**;
  ajustes contables fase6+fase10 separados: +2.544 B/+3 funciones.
- Baseline siguiente: scratchpad/resume_20260904_phase11_measure.json y
  _all_pct.json. Log completo: _build.log. Reconciliacion de los objetos
  integrados con snapshots auditados: scratchpad/phase11_refresh_and_check.py.
- Auditorias: phase11_crt2D1_audit.py, phase11_pro2D4_audit.py --real,
  phase11_sn_malloc_audit.py, ai_ppc2d2_round11_audit.py y phase11_exp_audit.py.
  Notas detalladas de cada frente en scratchpad con los mismos prefijos.
- Sin commits ni staging, HEAD ef92ca4e, cambios locales anteriores preservados.

## Plan de continuidad tras fase11

Quedan **157.440 B y 285 funciones** oficiales: Game105.456 B/93,
SDK804 B/3, STD13.028 B/11, Library38.132 B/177 y padding20 B/1.
El 100 % sigue siendo un objetivo pendiente, no un resultado de esta tanda.

1. Agente runtime: continuar ppc2D2.c por PCreadAsyncNext/ReadSyncNext y despues
   ACK/completion. Primero demostrar convenios de fn_803115AC/11684/117C0.
   Conservar los cuatro cierres de esta tanda y auditar estados IRQ/callback.
   Los siete puentes de 4 B requieren comprobar operandos de 64 bits y destinos;
   no convertirlos automaticamente en wrappers C ni excluirlos del denominador.
2. Agente bibliotecas: inspeccionar los pequenos pendientes de eathread_thread
   (40 B), pathreal (64 B) y SYSTEM_addexit (108 B), revisando intentos previos
   antes de editar. Elegir un frente con hipotesis concreta y sin tocar headers
   compartidos. El camino de callbacks AEMS exige evidencia propia por caller.
3. Agente FEng: aplicar el analisis de lifetimes a los dos pendientes restantes,
   FEngine::Update (776 B,99,45876 %) e IssueScriptMessages (540 B,98,77778 %).
   Preservar ProcessPadsForPackage y las otras 341 funciones exactas; no cambiar
   de forma global flags ni bindings de registros.
4. Root: coordinar fuentes no solapadas, configuracion y verificaciones globales;
   preservar snapshots de fase11 y revisar los parciales de libm con evidencia
   del backend. No repetir barridos de flags ni recetas de frame ya descartadas.

Mantener el censo de MetroTRK y el padding fuera de cualquier simulacion del
100 %. Cada integracion requiere build, medicion independiente, comparacion
por funcion y SHA; el SHA correcto por si solo no prueba la decompilacion completa.

## Duodecima tanda: cinco cierres y dos mejoras parciales

La tanda siguiente se lanzo tras completar la integracion de fase11. Baseline:
3.788.608 B y 18.147 funciones exactas. Agentes independientes tomaron FSasync,
bibliotecas pequenas y FEng; root trabajo en QuickGame, config/verificacion y
revision cruzada. Se mantuvieron las fuentes locales y no se hicieron commits.

### Codigo nuevo exacto: 1.248 B / 5 funciones

| Funcion | Unidad | Bytes |
| --- | --- | ---: |
| PCreadAsyncNext | ppc2D2 | 140 |
| ReadSyncNext | ppc2D2 | 160 |
| PCrwAsyncFSACK | ppc2D2 | 168 |
| PCrwSyncFSACK | ppc2D2 | 216 |
| QuickGame::OnManageTime | zSim | 564 |

**FSasync:** ppc2D2 pasa a 984/2.448 B, 8/22 funciones exactas. Prototipos
SNDVD/SNRead/SNWrite confirmados contra aliases, cuerpos y callers originales:
argumentos r3/r4 y preservacion de r28-r31/pila/LR en la frontera publica; no
se trasladan esas garantias a sus auxiliares EXI privados. Las lecturas conservan
captura del buffer antes de polling, limites/bloques, MMIO y orden distinto de
los stores sync/async. g_pBuffer es normal, no volatile artificial; contadores
y fase conservan accesos volatile demostrados en la tanda anterior.

Los ACK conservan el paquete existente de 12 B y respuesta de 8 B, error,
callback y fase. El camino sync limpia de nuevo fase/callback despues de un
callback; el async no vuelve a borrar una fase que el callback haya cambiado.
No se inventan timeouts o validaciones del tamaño recibido. Auditoria:
125 relocaciones, los 984 bytes exactos contra ELF, 9.556 casos por cada cuerpo
target/source frente a modelo, 194 instrucciones cubiertas, polling y callbacks
que cambian globals. Dos parciales y los otros doce cuerpos pendientes siguen
en el denominador; no hay datos, padding ni helpers nuevos.

**QuickGame:** 97,432625 %/560 B pasa a 100 %/564 B. La primera lectura de 1.0f
se expresa mediante el label original lbl_80404864, separando su identidad de
los literales numericos posteriores. Desaparece la direccion retenida en r29
y el marco vuelve de 48 a 40 B. Leer Tweak_GameSpeed antes recupera el orden de
dos lis; un valor de liberacion local en fr0, definido despues del callback,
conserva tres operandos originales. No se añaden instrucciones ASM.

Los **402 cuerpos target de zSim ya son exactos**, pero **NO se promociona la
unidad**. El label .rodata original pertenece a su propio pool y la fuente aun
lo importa: reconstruir/nombrar esos datos es una deuda real, declarada en el
codigo. El pool anonimo existente no cambia ni crece. No se añade alias a un
label $LC generado ni se cuenta progreso de enlace. Revision independiente
acepta el fragmento NonMatching, no el reemplazo del objeto entero.

La auditoria resuelve 59 relocaciones y compara los 564 bytes completos con
el ELF; 25 ramas y 34 referencias, constantes de cuatro bytes verificadas.
Las otras 570 funciones emitidas son identicas normalizando solo movimiento
de seccion, indices de simbolos y lineas de depuracion. .rodata7.240/.data92/
.bss8.180/.ctors4 y referencias no-texto permanecen iguales. Incluye marco,
registros guardados, llamadas directas/virtuales y todas las operaciones FP.

### Parciales: no suman funciones exactas

- IssueScriptMessages (zFEng): **98,77778 -> 99,77778 %**, siempre 540 B.
  Separar el offset efimero del que sobrevive a FindObjectByGUID recupera todo
  el cuerpo y latch. Solo quedan tres mr independientes del prologo fuera de
  orden; los 32 registros simbolicos coinciden despues de ellos, sin alterar
  CR/LR/CTR/memoria. Se verifican 135 opcodes, 28 ramas y seis referencias;
  otras 498 funciones intactas, 343 parejas sin regresion, datos/.ctors/tablas
  originales sin cambios. El lwz ASM inicial es heredado y no se amplio.
  FEngine.cpp y Update se restauran exactamente a baseline; permanece la
  diferencia mr+cmpwi frente a mr. del target. ProcessPads sigue exacta.
- SYSTEM_addexit: **68,33334 -> 80,37037 %**, 116 -> 112 B frente a target108.
  Indices separados y una dependencia vacia conservan el indice de elemento
  del segundo bucle. Un inicializador explicito corrige exitfunctions de
  BSS/NOTYPE a **.data/OBJECT: 256 bytes cero**, como el original. Auditoria de
  6.245 casos, 27/28 instrucciones, diez desenlaces y secuencia exacta de
  lecturas/escrituras. Conserva deduplicacion antes de buscar hueco, null,
  tabla llena, bounds y ABI. Queda una referencia LO adicional; no se promociona.
- eathread_thread y pathreal se conservan sin cambios, con snapshots completos
  source antes/despues identicos. No se fuerzan constructores vacios con ASM ni
  copias de agregado que creen un marco ausente en el original.

### Verificacion de fase12

- ninja correcto; recompila las unidades tocadas y actualiza REPORT/PROGRESS.
  No hay nuevas promociones ni cambios de entradas del enlace: esta vez ninja
  reutiliza el DOL de fase11, no ejecuta LINK/DOL de nuevo. Se comprueba su SHA-1
  explicitamente: **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
- Persisten avisos previos de overloads ocultos y conversion -1 a unsigned en
  zSim/FEng; no errores. ninja -n solo PROGRESS.
- Oficial: **3.789.856/3.946.048 B = 96,04181 %**, **18.152/18.432 funciones**.
  Fuzzy99,45511 %. Datos **370.056/1.285.748 B (28,781378 %)**: +256 B reales,
  no contabilidad. Enlazado299.056 B/7,5786204 %, 338/589 unidades, sin aumento.
- Acumulado doce tandas: **+50.224 B/+99 cuerpos reconstruidos**; ajustes
  contables previos separados: +2.544 B/+3 funciones. Fases11+12 juntas:
  **+2.020 B/+15 funciones**, sin contar sus mejoras parciales como cierres.
- Baseline siguiente: scratchpad/resume_20260904_phase12_measure.json y
  _all_pct.json. Log: _build.log. Reconciliador: phase12_refresh_and_check.py.
- Censo independiente final: **3.789.856/3.946.204 B = 96,0380 %**, 18.152
  funciones. Reconciliacion PASS en las 589 unidades, mismos denominadores,
  cero regresiones. Entre los 15.493 simbolos de 33 SourceLists solo cambian
  QuickGame (cierre) e IssueScriptMessages (mejora parcial). Los seis objetos
  revisados post-ninja coinciden con snapshots auditados, omitiendo solo lineas
  DWARF desplazadas por comentarios finales.
- Auditorias repetidas por root: ai_ppc2d2_round12_audit.py,
  phase12_zsim_audit.py, phase12_feng_audit.py y
  small12_audio_resume_20260904_audit.py. Revision cruzada QuickGame:
  phase12_zsim_independent_review.md. Notas de cada frente conservan ensayos.
- Sin staging/commits; HEAD ef92ca4e y modificaciones previas conservados.

## Continuidad preparada despues de fase12

Siguen pendientes **156.192 B y 280 funciones oficiales**: Game104.892 B/92,
SDK804 B/3, STD13.028 B/11, Library37.448 B/173 y padding20 B/1.
El objetivo 100 % permanece activo; no es equivalente a disponer de un DOL
reconstruido entero desde fuentes ni permite ignorar los problemas de censo.

El proximo bloque runtime ya tiene analisis solo-lectura en
**scratchpad/phase13_ppc2d2_plan.md**: DoFSReadHeader140 B y PCreadAsyncInit164 B.
Confirma paquete24 B, tamaño de payload16 codificado little-endian (corroborado
por sthbrx del dispatcher), payload words big-endian, segundo argumento
del header deliberadamente no usado y los seis argumentos reales del inicio.
La operacion antigua debe completarse antes de publicar callback/flag nuevos;
rechazo por alineacion ocurre antes de cualquier efecto. No se implementaron
estos dos cuerpos durante la preparacion, ni se cambio la baseline fase12.

Para los otros agentes, elegir nuevas hipotesis sobre pendientes reales; no
volver a barrer el prologo de IssueScriptMessages ni el latch de Update sin
evidencia distinta. Conservar las mejoras parciales de exit/exp/pow y todos los
casos de auditoria. Root mantiene configuracion, limites de propiedad entre
agentes, seguimiento de deuda de datos y verificacion global en cada tanda.

Estado al cerrar fase12: todas las compilaciones, medidas y auditorias de esta
tanda terminadas; tres agentes libres. Fase13 solo planificada, no implementada.
Objetivo global activo e incompleto, sin bloqueo nuevo que requiera al usuario.

## Decimotercera tanda integrada: zonas, inicio de lectura y sonido

Resultado real: **+1.944 B / +7 funciones exactas**, sin regresiones. La
referencia anterior fue fase12; no se vuelven a contar sus cierres.

| Funcion | Unidad | Nuevos bytes exactos |
| --- | --- | ---: |
| TrackPathManager::FindZone | zTrack | 664 |
| TrackStreamer::GetPredictedZone | zTrack | 660 |
| PCreadAsyncInit | ppc2D2 | 164 |
| Snd::Hal::SetCustomFx | ssdfx | 80 |
| Snd::Hal::Reset | ssdfx | 68 |
| SNDPLATFORM_setfxlevel | ssdfx | 196 |
| Snd::Util::SetDefaultAzimuths | sdownmix | 112 |
| Total | | 1.944 |

### zTrack: dos cierres, sin promocion de la unidad

- FindZone664 B: resultado local en r26 con restriccion vacia de entrada;
  el literal de la ruta de cache queda separado en r25. No cambia el valor del
  puntero, las condiciones ni los accesos.
- GetPredictedZone660 B: el original limpia primero flag r28 y luego zona r27.
  Se conserva ese orden y se fija solo el bool a r28 mediante entrada vacia.
  El numero de zona sigue siendo int ordinario. Los ensayos que fijaban ambos
  valores provocaban extsh extra y fueron retirados.
- No se agregan instrucciones ASM ni barreras de memoria; son restricciones
  locales del compilador verificadas con su regla real. Ningun flag modificado.
- Auditoria phase13_ztrack_audit.py: **1.324 bytes originales**, 93 relocaciones,
  46 ramas y 47 referencias, incluidas 28 referencias a floats verificadas por
  posicion y bits. Otros297 cuerpos de299 emitidos y datos/relo sin cambios.
- Revision independiente aprobada: phase13_ztrack_independent_review.md.
  Notas y ensayos: phase13_ztrack_notes.md. Target258/259 funciones exactas;
  GetLoadingPriority708 B sigue97,18079 %, y permanece deuda de datos/pools.
- En GetLoadingPriority solo se actualiza un comentario obsoleto: marco actual
  0x110 frente a0x120 target; quedan16 B sin identificar. No se inserta padding
  ni local muerta inventada para forzar ese marco.

### ppc2D2: inicializador exacto, header parcial legitimo

- PCreadAsyncInit164 B al100 %. Verifica alineacion antes de cualquier efecto;
  completa la operacion vieja antes de publicar nuevo flag/callback; conserva
  los seis argumentos, manda header y publica fase1 al final. No agrega guards.
- DoFSReadHeader se reconstruye como C completo, pero queda **136/140 B,
  97,11429 %**, sin contarlo como cierre. Su unica diferencia es li4096 frente
  al li16+slwi8 original para el tamano little-endian del paquete24 B. Ambos
  producen10 00; los tres words del payload siguen big-endian. No se fuerza
  una lectura volatile ni ASM para impedir ese plegado constante.
- Total unidad **1.148/2.448 B,9/22 exactas**, doce cuerpos emitidos. Las ocho
  exactas previas y los dos parciales anteriores permanecen intactos.
- ai_ppc2d2_round13_audit.py:149 relocaciones,1.148 bytes exactos contra ELF,
  **19.557 casos por target/source/modelo**,18.414 rechazos sin efectos,
  76/75 instrucciones de los cuerpos nuevos cubiertas. Prueba24 bytes de cada
  paquete, orden global, ABI y callbacks viejos que modifican el estado.
- Transporte y CompleteAsync viejo se modelan con hooks documentados, no se
  afirma ejecucion de hardware. Auditor repetido por root, PASS. Notas y
  snapshots ai_ppc2d2_round13_*; NonMatching se mantiene.

### Sonido: dos fuentes nuevas y una unidad enlazada desde fuente

- ssdfx.c (C++ con los flags snd existentes):3/3 funciones,344 B exactos. Tipos
  privados del driver duplicados identicamente a snddrv.c, sin tocar headers.
  DSP bit0x200 tiene precedencia sobre software bit4; se conserva la conversion
  float->signed32->low16, el bus y el orden de las secciones criticas.
- Auditor ssdfx:344 bytes originales/20 relocaciones/16 referencias/4 ramas,
  **6.912 casos pareados**,86 instrucciones y ABI con clobbers adversarios.
- **ssdfx sigue NonMatching**: su float32767.0f en80412AE8 esta asignado al
  split mpegl3base, aunque solo lo consume ssdfx. Los duplicados80412AEC y
  80412AF8 son privados de sdspmix y snddrv. Eso indica posibles limites rodata
  incorrectos, no un dato compartido demostrado. No se sustituye el literal C
  por un extern artificial. DWARF llama a esta CU gc/sdfx.c, no ssdfx.c.
- sdownmix.cpp:112 B exactos y tablas globales144+36 B reconstruidas. Target
  lbl80412CC2 equivale a tabla80412CC8 con addend-6 natural. Se verifican HA/LO
  despues del addend, incluidos cruces del bit de signo bajo, sin crear alias.
- Auditor sdownmix:112 bytes y180 bytes de tablas iguales al ELF,7 relocaciones,
  exports/tipos/orden correctos y **1.120 casos pareados**. Contrato de metadatos
  fold0..3/canales1..6; canal0 se prueba solo como comportamiento maquina sin
  accesos, no como garantia de C portable fuera de contrato.
- El target extraido declara alineacion8 y C emite4. Forzar aligned8 agregaba
  cuatro ceros que pisarian datos reales en80412D7C: ensayo retirado. La seccion
  previa acaba en80412CC8, valido para ambas alineaciones, por lo que180 bytes
  se colocan correctamente sin ese atributo. Diferencia de metadata explicita.
- Root cambia **solo sdownmix a Matching** y valida LINK/DOL/CHECK. main.elf
  coloca funcion8036D1D8, tablas80412CC8/80412D58 y siguiente objeto80412D7C;
  el DOL vuelve a dar SHA-1 original. Promocion retenida: +112 B source-linked,
  +1 unidad, +180 B datos reales. No es un ajuste contable del censo.
- Notas/auditores completos: phase13_ssdfx_{notes.md,audit.py} y
  phase13_sdownmix_{notes.md,audit.py}. Sus before confirman fuente ausente;
  no se usa target-vs-target como baseline de progreso.

### Particulas: ensayos limitados, restauracion completa

- UpdateParticles sigue99,95605 %,1820 B. Separar initial_angle mejoraba el
  cuerpo pero invertia las dos cargas/referencias de constantes iniciales;
 99,97802 % no era cierre. La pista se guarda en phase13_emitter_body_exact.cpp
  con advertencia expresa; no queda en el fuente.
- Render sigue98,074715 %,696 target/688 source. Se identifica sentinel
  derramado en sp+0xc8 y marco target0x128 frente a0x120 source. La dependencia
  local de memoria reproducia store/reload pero dejaba otros registros y marco
  diferentes; el segundo ensayo tampoco mejoraba. Ambos retirados.
- phase13_ecstasy_audit.py confirma restauracion:2999 simbolos/654 funciones,
 539 parejas,38.710 instrucciones y10.267 referencias identicas. Datos reales,
  incluyendo cuatro ceros finales omitidos por objdiff, preservados. Root lo
  repite, PASS. Notas phase13_ecstasy_notes.md; ningun progreso ficticio.

### Verificacion global y nueva referencia autoritativa

- ninja termina correctamente y esta tanda SI ejecuta LINK/DOL/CHECK, debido
  a sdownmix Matching. SHA-1 **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
  Persisten avisos previos PPCMtdec duplicado y conversiones numericas; no
  errores. ninja -n solo PROGRESS. Log resume_20260904_phase13_build.log.
- Oficial **3.791.800/3.946.048 B =96,09107 %**, **18.159/18.432 funciones**;
  fuzzy99,4743 %. Datos370.236/1.285.748 B=28,79538 %.
- Source-linked **299.168 B/7,581459 %**,339/589 unidades. Esta medida no es
  equivalente al porcentaje de funciones/codigo matched.
- Censo independiente **3.791.800/3.946.204 B=96,0873 %**,18.159 exactas. Se
  mantiene la diferencia conocida de156 B en denominadores, sin ocultarla.
- **phase13_refresh_and_check.py PASS**:589 unidades con igual inventario y
  denominadores, deltas exactos1944 B/+7 en cuatro unidades, cero regresiones.
  Los15.493 simbolos de33 SourceLists solo cambian en los dos cierres zTrack.
  Cinco diffs post-ninja identicos a snapshots auditados, exceptuando solo
  line_number. Verifica ademas colocacion enlazada sdownmix y hash original.
- Referencias para continuar: **scratchpad/resume_20260904_phase13_measure.json**
  y **scratchpad/resume_20260904_phase13_all_pct.json**. Sin staging/commits;
  HEAD ef92ca4e. Cambios locales anteriores preservados.
- Acumulado trece tandas: **+52.168 B/+106 cuerpos reconstruidos**; ajustes
  contables anteriores separados e invariables: +2.544 B/+3 funciones.

## Continuidad despues de fase13

Pendientes oficiales: **154.248 B y273 funciones**: Game103.568 B/90,
SDK804 B/3, STD13.028 B/11, Library36.828 B/168 y padding20 B/1.
El objetivo100 % continua activo; ningun bloqueo nuevo requiere al usuario.

- Runtime: **phase14_ppc2d2_plan.md**, leido por root: CompletePCreadAsync388 B,
  ABI ordinaria comprobada, propuesta C con dos drenajes separados, contador
  final-1, tail sin avance de buffer, pollbit0 y callback solo via ACK. Incluye
  matriz de estados/MMIO/callbacks y prueba de las97 instrucciones originales.
  Es preparacion, no implementacion ni nuevo cierre. No abrir IRQ handlers aun.
- Sonido: SNDI_root1x144 B como proximo candidato. **La fuente ya existe y esta
  tracked/limpia**: matched_code0 no significa ausencia. La preparacion de
  phase14_sndi_root1x_plan.md debe partir de ese fuente, no recrearlo; conservar
  sus datos originales y separar deuda de limites rodata de avance de codigo.
- Juego: **phase14_queuefileload_plan.md** y snapshot de lectura de zEAXSound.
  QueueFileLoad736 B tiene cinco operandos distintos al copiar pThis/pCar;
  probar frontera entre copia agregada y puntero tipado local. El RTL antiguo
  no corresponde al fuente actual con framePad/goto: obtener RTL vigente si
  hace falta. No volver a barrer registros globalmente sin esa evidencia.
- No se han implementado esos cuerpos durante la preparacion. No hay
  compilaciones ni escrituras pendientes sobre fuentes/objetos de fase13.
