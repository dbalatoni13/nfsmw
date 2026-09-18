# r39 — tres cierres y runtime integrado

2026-09-07, base `590dc4a0`, conservando los cambios locales de r37/r38.
Tanda de tres agentes mas coordinacion; todos los territorios estan liberados
y no queda ningun ensayo ni compilacion en marcha. **El 100 % sigue pendiente.**

## Progreso oficial final

| medida | antes r39 | despues r39 | delta |
|---|---:|---:|---:|
| codigo exacto | 3.890.852 / 3.946.048 B | **3.891.352 / 3.946.048 B** | **+500 B** |
| porcentaje codigo exacto | 98,601234 % | **98,61391 %** | +0,012676 pp |
| funciones exactas | 18.368 / 18.432 | **18.371 / 18.432** | **+3** |
| codigo linked | 735.668 B / 18,643158 % | **736.012 B / 18,651876 %** | **+344 B** |
| datos exactos | 443.749 / 1.285.741 B | igual | 0 |
| datos linked | 319.036 B | igual | 0 |
| unidades completas | 476 / 617 | **478 / 618** | +2 completas, +1 total |

Quedan **54.696 B en 61 entradas de funcion y 27 unidades** con codigo
pendiente. Incluye la entrada automatica de padding .init, 20 B; no todas
las entradas son funciones de juego. Fuzzy global: 99,95104 -> 99,95401 %.
El incremento de unidades procede exclusivamente del split aprobado.

## Cierres retenidos

1. **__va_arg, 244 B / 100 %**, implementacion C para MW 1.2.5n. Se encontro
   la declaracion ABI de un byte en la cabecera local del SDK y se recupero
   la forma de las sumas/alineacion. Tras autorizacion explicita, se separo
   de los ocho puentes GCC anteriores, que conservan exactamente sus 100 B.
   Las dos unidades se marcan Matching y se enlazan como objetos separados.
   Detalles: [r39-runtime.md](r39-runtime.md).
2. **VDevice_GetFreeEffect, 116 B / 100 %**, en `LibSN/steering.c`.
   Una lectura volatile localizada fija el acceso a vd->dev entre los
   stores volatile y la lectura del contador como en el original. Las 26
   exactas anteriores no cambian. Detalles: [r39-steering.md](r39-steering.md).
3. **CARSFX_PreColWoosh::MsgBarrier, 140 B / 100 %**, en zEAXSound2.
   Tres dependencias vacias sobre el bool, el receptor de la llamada y el
   argumento entero real reproducen el orden de la llamada Initialize.
   No hay cantidades indefinidas ni cuerpos ASM nuevos. Detalles:
   [r39-sound.md](r39-sound.md).

La ganancia de __va_arg es 244 B matched, no 344: los primeros 100 B ya
eran exactos antes. En cambio los 344 B si son nuevos para linked.
Sonido y steering siguen NonMatching como unidades completas.

## Verificacion del enlace y del alcance autorizado

El usuario aprobo SOLO dividir ppc2D2 en 100 B GCC + 244 B MW y los cambios
correspondientes de configure/splits para integrarlo y promocionarlo.

- Nuevo corte: **0x80311CB4**. No cambian extremos ni FSasync.
- `configure.py`: cambia la entrada ppc2D2 y añade la entrada MW con su
  override de compilador. No modifica flags compartidos ni otras unidades.
- `objdiff.json` regenerado: solo ppc2D2 y su nueva cola MW varian.
- `checksplits.py`: cero solapes y cero cortes de simbolos.
- El DOL PRINCIPAL se reconstruye con las dos fuentes reales compiladas y
  conserva **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. `dtk shasum` da OK.
- No se uso ninja completo, ni se cambiaron symbols.txt/keep.lst/ldscript.

El enlace parcial experimental `ngcld -r` producia codigo exacto pero
introducia una entrada .data de SN y un DOL distinto; se descarto. La
integracion usa objetos separados y no retoca bytes del original.

La promocion de **spchpick** de r38 sigue esperando su propia autorizacion.
Tiene 42/42 exactas y enlace aislado valido, pero su entrada sigue
NonMatching. **No incluir sus 7.836 B en el linked actual.**

## Auditorias sin regresiones

- `codex_r39_global_audit.py` compara las **18.432 entradas oficiales**,
  mapeando solo el traslado de __va_arg: mismas direcciones/tamanos objetivo,
  ninguna similitud baja y solo las tres funciones anteriores mejoran a 100.
- Runtime: auditadas las nueve funciones contra el ELF original. Las
  secciones objetivo nuevas concatenadas son exactamente los 344 B previos;
  el prefijo compilado de 100 B tampoco cambia. Ninguna TU emite datos.
- Sonido: auditor estandar, **925 funciones exactas OK**, incluidas las
  cuatro ramas/nueve reubicaciones/cuatro literales de MsgBarrier. Auditor de
  aislamiento: 2.808 simbolos / 23 secciones, otros 2.793 registros no-section
  identicos. No cambia ninguna funcion vecina ni ningun dato/literal runtime.
- Steering: auditor estandar valida las otras 26 exactas. En GetFreeEffect
  rechaza los sufijos locales MW distintos; el gate especifico resuelve las
  cinco SDA21 reales, addends cero y destinos individuales 0x804FF8C8 y
  0x804FF8CC. Tras aplicar SOLO esas relocaciones, **116/116 B** coinciden
  con NFSMWRELEASE.ELF. Normalizar el nombre sin esa comprobacion no bastaba.
- Steering NO queda promocionable: el orden de sus estaticos .sbss aun
  difiere. Certificar una funcion no certifica el layout de toda la unidad.
- `pctsnap --umbral 0`: 15.493 funciones / 33 SourceLists, una mejora
  (MsgBarrier), **cero regresiones**. Las otras dos nuevas son bibliotecas.
- `lcfix.py --check` y `git diff --check`: correctos.

## Censo directo y limitacion del comparador antiguo

`measure.py` completo pasa de **3.890.996 / 3.946.204 B**, 18.374 exactas y
617 unidades a **3.891.496 / 3.946.204 B**, 18.377 exactas y 618 unidades.
Son **+500 B / +3**, con denominador constante.

**Atencion:** `measure.py --cmp` omite las unidades que no existen en ambos
JSON. Por eso muestra solo **+256 B / +2** (steering y sonido); no suma
la unidad MW recien creada, **+244 B / +1**. No es una regresion ni una
ganancia artificial. El auditor global de r39 suma los cuatro contadores
de TODOS los registros y exige deltas `[500, 3, 0, 0]` (bytes exactos,
funciones exactas, bytes totales, funciones totales). No se modifico el
script compartido en esta tanda.

La discrepancia previa entre el censo directo y el oficial (144 B exactos,
156 B totales, seis funciones exactas y siete entradas totales) sigue siendo
la misma; no cuenta como avance.

## Frentes negativos retirados

- **zWorld2 / InitAtSegment**: referencia solo de entrada, argumentos ABI y
  seguimiento de seis variantes con segunda dependencia. Ninguna cierra;
  la segunda salida cambia fNodes antes de corregir el horario. Fuente
  restaurada, build final, 357 entradas sin variacion y 355 exactas auditadas.
  [r39-world2.md](r39-world2.md) recoge el seguimiento completo; ya no esta
  pendiente probar aquella familia de doble dependencia.
- **CARSFX_Turbo::ProcessUpdate**: alias y cinco variantes de lifetime/pin
  retiradas; baseline 672 B / 97,5 % preservada.
- **steering**: scopes en HandleTriggers y copias en SimThread_Step retirados;
  no se conserva ninguna mejora parcial ni se repiten barridos de flags.

WRoadNetwork.cpp y CARSFX_Turbo.cpp recuperaron diff cero; coordinacion
normalizo solo sus CRLF tras liberar a los agentes. Se conservan intactos
filesys.cpp, TrackStreamer.cpp y spchpick.c de r37/r38.

## Estado de ficheros y continuacion

Fuentes nuevas/modificadas de r39: `src/Runtime.PPCEABI.H/__va_arg.c`,
`src/LibSN/ppc2D2.c`, `src/LibSN/steering.c` y
`src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_PreColWoosh.cpp`.
Mas `configure.py` y `config/GOWE69/splits.txt`, expresamente autorizados.
No hay git add/commit; la nueva fuente runtime permanece untracked hasta
que el usuario decida incorporarla al indice.

Para continuar:

1. No volver a trabajar el codigo de spchpick o __va_arg: ambos cerrados.
   Resolver por separado la autorizacion de promocion de spchpick.
2. Revisar las otras nueve pendientes MW de steering con hipotesis nuevas:
   la carga localizada volatile es una palanca real de scheduling observada,
   no motivo para marcar campos/estructuras completos volatile.
3. En sonido quedan cinco pendientes de zEAXSound2; evitar repetir los
   pines que cruzan llamadas de ProcessUpdate o las vedas de Play/BindToData.
4. En zCamera, zWorld y las bibliotecas pendientes priorizar nueva evidencia
   de compilador/ABI/inline frente a nuevas permutaciones equivalentes.
5. La proxima comparacion debe incluir unidades añadidas/eliminadas: no
   confiar solo en --cmp cuando haya un split aprobado.

Instantaneas: `scratchpad/codex_20260907_r39_{before,after}_{report,measure,pct}.json`.
Informe vigente: `build/GOWE69/report.json`. Auditores principales:
`scratchpad/codex_r39_global_audit.py`, `codex_r39_runtime_audit.py`, y los
auditores dentro de `codex_r39_sound/` y `codex_r39_steering/`.
