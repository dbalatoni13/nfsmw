# Continuacion 2026-09-08: +216 B exactos y Debugger preparado

Base autoritativa `8853122e`, con todo el trabajo local R47/R48 preservado.
Se leyeron el brief R46 y los informes posteriores antes de elegir frentes.
No se reutilizaron los baselines anteriores a esa revision ni se repitieron
las integraciones de ppc2D2, MsgPlayMiscSound, stagpat o spchpick.

## Censo final verificado

| medida oficial | inicio estable | final | delta |
|---|---:|---:|---:|
| codigo exacto | 3.900.720 / 3.946.048 B | **3.900.936 / 3.946.048 B** | **+216 B** |
| porcentaje exacto | 98,85130 % | **98,85678 %** | +0,00548 pp |
| funciones exactas | 18.380 / 18.432 | **18.381 / 18.432** | +1 |
| codigo linked | 757.604 B / 19,199057 % | igual | 0 |
| datos exactos | 443.781 / 1.285.733 B | igual | 0 |
| datos linked | 355.348 B | igual | 0 |
| unidades completas | 483 / 616 | igual | 0 |

Quedan **45.112 B en 51 entradas**, incluido el padding automatico de 20 B.
El objetivo del 100 % sigue pendiente.

El censo directo independiente pasa de 3.900.864 a 3.901.080 B sobre
3.946.204 B; de 18.386 a 18.387 funciones exactas. Confirma **+216 B / +1**.
Su inventario difiere historicamente del oficial; esa diferencia no se cobra.
El snapshot de las 33 SourceLists (15.493 entradas) permanece byte-identico.

## Integrado: VDevice_RecalcGammaTable

`src/LibSN/steering.c`: **216/216 B, 54/54 instrucciones al 100 %**.
Se reconstruye la ranura unica de conversion de 8 B y el marco original 0x20
mediante almacenamiento explicito y tres instrucciones MW inline ASM
(dos fsubs y fctiwz); el resto de la funcion sigue en C. Todos los operandos
se definen antes de usarse y no hay pines de registro fisico ni cambios de flags.

Las otras funciones mantienen codigo y referencias; se conserva expresamente
el cierre local anterior de DownloadEffect. Se audita la renumeracion/orden de
los literales, no se equiparan nombres a ciegas. La fuente mantiene CRLF y todo
el contenido local fuera de esta funcion queda byte-preservado.

Detalle y controles negativos: [resume-20260908-steering.md](resume-20260908-steering.md).
La nota nueva de PLAYBOOK documenta la tecnica y su limite: MWCC tambien
coalesce un `asm { mr out, mag }`; esa prueba de Envelope fue neutra y no se
retuvo.

## Listo para proponer: DebuggerDriver en tres TUs

El ELF STT_FILE demuestra los propietarios originales `DebuggerDriver.c`,
`AmcExi2Comm.c` y `AmcExi.c`. El Ecb de 192 B ya pertenece a EXIBios y no se
toca; el de 24 B es local del bloque Amc. Se reparte el estado real y los
rangos precisos, sin globalizar estaticos ni inventar relleno.

Dos pruebas completas e independientes de extraccion/enlace en sombra
(`proof` del agente y `proof_root_recheck` de coordinacion) producen, tanto en
control como con las tres TUs compiladas, el DOL original byte a byte:
**SHA-1 9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
Las 31 funciones quedan exactas: 14/14 + 8/8 + 9/9.

El inventario oficial vigente tiene 28/31 exactas, 5.572/5.988 B. La propuesta
aporta **416 B matched y 5.988 B linked**. No confundirlo con la consulta
aislada sin asociaciones, que mostraba 22/31. No se aplican configuracion,
splits, nuevas fuentes ni promociones sin autorizacion especifica.
Plan: [resume-20260908-debugger.md](resume-20260908-debugger.md).

## Recuperacion propuesta, NO integrada: barrera P6 de ENVIRO_AEMS

La cabecera compartida real conserva un cambio local previo de R47-c25:
comentario P6 y `asm volatile` de memoria tras Dry_FX en FX_ROADNOISE.
Un control privado que elimina **solo esas dos lineas**, conservando Wet_FX=-1,
demuestra su efecto:

- `InitSFX`: 444 B / 92,34234 % -> **100 %**.
- `Play`: 392 B / 87,45918 % -> 99,64286 %, aun pendiente.
- No cambia ninguna otra metrica de las 930 funciones comparables.
  MsgPlayMiscSound780, MsgBarrier140 y BindToData340 siguen exactas.

Se verifican las secciones/relocs crudas del control contra produccion y la
identidad precisa de sus dos alias locales renumerados por el wrapper (7 usos,
indices 355/356, .bss+0x10/+0x14, 4 B, LOCAL/STT_NOTYPE). Eliminar la barrera
solo cambia 117 posiciones de byte y 4 registros de relocacion dentro de
InitSFX/Play. Datos y referencias de datos son identicos. Los otros 23
registros JSON distintos solo cambian la linea DWARF en -2.

Los gates `codex_20260908_resume_enviro/audit.py` y `audit_variant.py` se
repiten por coordinacion: PASS. El segundo audita InitSFX y las tres funciones
protegidas contra el ELF original. Ambas instancias de FX_ROADNOISE viven en
Roadnoise.cpp/zEAXSound2; zEAXSound no emite ese constructor.

Son **444 B recuperables de un cierre anterior**, no una nueva decompilacion.
La regresion local ya estaba presente en el censo inicial estable de esta
tanda. No se ha retirado la barrera real ni actualizado ningun fichero
congelado: requiere autorizacion acotada de cabecera compartida, independiente
de la autorizacion antigua de MAIN_AEMS/MsgPlayMiscSound.
Detalle: [resume-20260908-enviro.md](resume-20260908-enviro.md).

## Ensayos retirados

- `madidct`: exponer el acarreo real y combinar LR/CTR no reproduce su vida;
  tres sombras negativas. Produccion completamente intacta.
  [resume-20260908-madidct.md](resume-20260908-madidct.md).
- `zWorld2`: se cuantifican los umbrales de prioridad CR y la preferencia FPR.
  Corregir las cinco filas FP altera cantidades posteriores; no hay cierre.
  Fuente/objeto/JSON restaurados byte a byte, preservando bCross de R46.
  [resume-20260908-world.md](resume-20260908-world.md).
- `zEagl4Anim`: el helper de conversion es neutro; local allocation/reload
  vuelve a materializar tarde el bias que sched habia izado. No se reabre el
  barrido historico de Initialize. Fuente/objeto/JSON restaurados, EvalSQT
  sigue exacta y RawStateChan local no se toca.
  [resume-20260908-anim.md](resume-20260908-anim.md).

## Verificacion y disciplina

- `codex_20260908_resume_global_gate.py`: las 18.432 entradas oficiales
  conservan direccion/tamano; solo mejora RecalcGammaTable, ninguna regresion.
- `audit_shadow.py production`: inventario entero de steering, todos los
  destinos/tipos/addends runtime y las 30 funciones exactas contra el ELF.
- Gates read-only de animacion y mundo repetidos por coordinacion: PASS.
- `MsgPlayMiscSound` autorizado previamente sigue exacto: 780 B, 25 ramas,
  64 relocs. No se contabiliza una segunda vez.
- Enlace principal desde la lista real generada, sin ninja: DOL SHA original
  comprobado despues de integrar steering. Solo aviso historico PPCMtdec.
- `checksplits.py`: 1.092 rangos, cero solapes, cero cortes de simbolo.
- `lcfix.py --check` y `git diff --check`: PASS; avisos LF/CRLF no son errores.
- Sin staging, commits, cambios de configuracion/splits ni de cabeceras reales.

Incidente registrado: un agente invoco por error `build_direct.py --help`;
ese script no procesa ayuda y empezo a compilar SourceLists. Se interrumpio el
arbol, se pausaron escritores y se repitio el censo. Los tres snapshots
report/measure/pct repetidos resultaron SHA-256 identicos a los anteriores;
el DOL y lcfix tambien pasan. No hubo ediciones de fuente por ese incidente.
No ejecutar esa opcion como consulta: leer el script y usar un target exacto.

Snapshots: `scratchpad/codex_20260908_resume_{stable,after}_{report,measure,pct}.json`.
Informe vigente actualizado: `build/GOWE69/report.json`.

Todas las fuentes y objetos de produccion estan liberados por los agentes.
La siguiente integracion requiere responder a las dos propuestas acotadas:
configuracion/splits y tres fuentes de Debugger; retirar solo las dos lineas
P6 de ENVIRO_AEMS. Sus pruebas de sombra ya pasan, pero no se han aplicado.
