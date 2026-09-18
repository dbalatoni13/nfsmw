# r38 — cierre de tanda: spchpick completa

2026-09-07. Repositorio `C:/Users/jferr/Desktop/nfsdecompiled`, base
`590dc4a0` mas los cambios locales conservados de r37. Tres agentes han
terminado sus territorios y no quedan escritores ni ensayos activos.
El objetivo de llegar al 100 % **sigue pendiente**.

## Progreso oficial

| medida | antes | despues | delta |
|---|---:|---:|---:|
| codigo exacto | 3.889.916 / 3.946.048 B | **3.890.852 / 3.946.048 B** | **+936 B** |
| porcentaje de codigo exacto | 98,577515 % | **98,601234 %** | +0,023719 pp |
| funciones exactas | 18.366 / 18.432 | **18.368 / 18.432** | **+2** |
| codigo linked | 735.668 B / 18,643158 % | igual | 0 |
| datos exactos | 443.749 / 1.285.741 B | igual | 0 |
| datos linked | 319.036 B | igual | 0 |
| unidades linked completas | 476 / 617 | igual | 0 |

Quedan **55.196 B, 64 funciones y 28 unidades** con codigo pendiente. Este
inventario oficial incluye el padding automatico de `.init` (20 B / 1);
no confundir sus entradas con funciones de juego escritas por el programador.
Fuzzy global: 99,95035 % -> 99,95104 %.

El censo independiente pasa de 3.890.060 / 3.946.204 B y 18.372 funciones
exactas a 3.890.996 / 3.946.204 B y 18.374. Confirma **+936 B / +2**.
La diferencia de denominador (156 B), bytes exactos (144 B) y seis funciones
ya existia; no es ganancia de esta ronda.

## Cambio nuevo retenido

**`src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchpick.c`**:

- `iSPCH_MakeSampleRequests`: **524 B / 100 %**. Dependencia real entre
  desplazamiento y handle cargado, handle de alcance corto y desplazamiento r9.
- `iSPCH_ChooseSamples`: **412 B / 100 %**. Orden exacto de inicializacion
  con dos dependencias sobre valores reales, sampleTable/r17 y alias real
  del argumento sentence/r25 para resolver dos intercambios independientes.

La unidad pasa de 40/42 a **42/42 funciones exactas**, 7.836/7.836 B.
No se incorporan cuerpos ASM ni valores sin inicializar. Las restricciones
vacias se conservan porque cierran ambos cuerpos por completo.
Pruebas, alternativas negativas y auditoria: [r38-spchpick.md](r38-spchpick.md).

Los cambios anteriores de `filesys.cpp` y `TrackStreamer.cpp` se mantienen
intactos. El descenso fuzzy estructural de zTrack pertenecia a r37, no a esta
tanda; aqui sus instrucciones y medidas no cambian.

## Promocion propuesta, todavia no aplicada

`trypromo.py` reconstruye un DOL **identico al original** al sustituir solo
el objeto de spchpick por el reconstruido. Se repitio al finalizar la fuente
y vuelve a dar **DOL OK**. Es una candidata real para sumar **7.836 B linked**.

Se ha propuesto al usuario cambiar exclusivamente la entrada de spchpick
de `NonMatching` a `Matching` en `configure.py`, como requiere el brief.
**La autorizacion esta pendiente y la configuracion NO se ha modificado.**
No atribuir esos 7.836 B al linked vigente. Tras autorizar, regenerar la
configuracion, enlazar el objetivo concreto sin ninja completo, comprobar
el SHA-1 del DOL principal y volver a generar el informe.

filesys tambien tiene todo el codigo exacto, pero NO comparte este estado:
su enlace aislado de r37 falla por disposicion de datos. No promocionarla
por analogia con spchpick.

## Frentes explorados sin cambios retenidos

- **zCamera**, [r38-camera.md](r38-camera.md): 90 expansiones inline en el
  static-init original y reconstruido, mismas firmas y orden. La diferencia
  CSE esta dentro de cuerpos equivalentes; no falta un constructor. Ensayos
  de constructores y dependencias retirados; 587 funciones emitidas y datos
  vuelven al baseline, incluidas las 448 exactas.
- **zEagl4Anim**, [r38-anim.md](r38-anim.md): pruebas causales y dos barridos
  acotados en EvalState/FindMatchTime sin cierre. El sesgo int->float puede
  moverse con una union, pero altera la ranura o duplica la constante.
  JSON completo y 318 entradas restaurados.
- **zEcstasy**, [r38-ecstasy.md](r38-ecstasy.md): una dependencia read/write
  nueva sobre envmap_power mejora parcialmente el orden de cargas, pero
  rota los FPR y empeora; retirada. 535 exactas conservadas, Render sigue
  exacta y las 539 parejas del censo quedan identicas.
- **spchsamp**, [r38-spchsamp.md](r38-spchsamp.md): cuatro variantes de
  materializacion de blockSize negativas. No se repitieron los tres pines
  r37. Fuente, objeto comparado y funcion exacta de 48 B restaurados.
- **__va_arg**, [r38-runtime.md](r38-runtime.md): solo scratchpad. Un backend
  MW antiguo reproduce mucho mejor la estructura que GCC: hasta 240 B /
  96,803276 %, aun no los 244 B exactos. Es una pista de procedencia y ABI,
  no un cierre ni autorizacion para cambiar el compilador de todo ppc2D2.

## Verificacion final

- `measure.py --cmp`: solo cambia spchpick, +936 B / +2 funciones.
- `pctsnap.py --cmp`: 15.493 funciones en 33 SourceLists, **0 mejoras y
  0 regresiones**; spchpick es una biblioteca fuera de ese censo.
- Informe oficial regenerado con REPORT directo: **solo cambian las medidas
  de spchpick**. No se ejecuto ninja completo.
- `fncmp.py` de spchpick: 0/42 distintas; `audit.py`: 42/42 pasan bytes,
  condiciones/destinos de ramas, referencias y literales.
- Auditor propio: 85 simbolos / 19 secciones; las otras 40 funciones,
  tamanos, direcciones, datos, alineaciones y referencias no cambian.
- `lcfix.py --check`: todas las entradas @lc al dia.
- `git diff --check`: correcto. `configure.py`, `config/`, `objdiff.json`
  y `src/LibSN/ppc2D2.c` siguen sin cambios.
- Solo tres fuentes tracked modificadas: filesys.cpp y TrackStreamer.cpp
  de r37, y spchpick.c de r38. Se restituyeron mecanicamente los CRLF de
  spchsamp.c e ICEMover.cpp despues de retirar los ensayos; sus diffs son cero.
- spchpick conserva su Latin-1/CRLF original. Se recompilo despues de
  restituir la codificacion y se repitieron comparacion y auditoria.
- DOL principal existente: SHA-1 **9619ba57c9919f95f7f2ac951a2166a3517f91e3**.
  No se reconstruyo el principal en r38: la comprobacion de spchpick es
  ademas un enlace aislado nuevo, no solo un hash del DOL previo.
- No hay staging ni commits y no se han borrado experimentos del usuario.

## Prioridades para continuar

1. Resolver la propuesta de promocion de spchpick; no necesita mas trabajo
   de codigo. Mantener filesys aparte hasta resolver sus datos.
2. Investigar procedencia/ABI de `__va_arg` y las ultimas sumas con MW antiguo,
   solo aislado hasta que exista una propuesta de split/config respaldada.
3. zCamera: comparar RTL inicial de los cuerpos inline equivalentes,
   especialmente bFill/constructores anteriores a las tablas. No repetir
   constructor/pines/barreras sin evidencia causal nueva.
4. Para otra tanda independiente, conservar las vedas de r38 y buscar nueva
   evidencia en zWorld (5.608 B / 5), steering (4.080 / 10), zWorld2
   (3.796 / 2) o zEAXSound2 (3.564 / 6). Leer sus notas previas antes de
   asignar territorios; el nombre y porcentaje de un comentario no es el
   estado vigente del objeto.

## Instantaneas

Prefijo `scratchpad/codex_20260907_r38_`:
`before_report.json`, `after_report.json`, `before_measure.json`,
`after_measure.json`, `before_pct.json`, `after_pct.json`.
`build/GOWE69/report.json` contiene el informe oficial actualizado.
Los informes de territorio detallan el resto de objetos, JSON y auditores.
