# r42 - continuacion sobre el cierre verificado de r41

2026-09-08. Se traslado inmediatamente el hallazgo de saturacion de argumentos
de MsgPlayMiscSound a RoadNoise y se probo la cache de size() propuesta para
camara. **No hay cierre exacto nuevo ni cambios de produccion retenidos en r42.**

## Estado final

- Codigo exacto: **3.893.124 / 3.946.048 B, 98,65881 %**.
- Funciones exactas oficiales: **18.373 / 18.432**.
- Codigo linked: **736.012 B, 18,651876 %**, sin cambios.
- Pendiente: **52.924 B / 59 entradas / 27 unidades**, incluyendo 20 B de
  padding automatico .init.

La ganancia integrada de esta continuacion sigue siendo la de r41:
**+780 B / +1 funcion**, con MsgPlayMiscSound al 100 %, y la correccion
independiente del literal VM sin inflar el contador. El 100 % del proyecto
sigue pendiente. Vease [r41-cierre.md](r41-cierre.md).

## RoadNoise: recuperados los 4 B ausentes solo en ensayo

Cuatro argumentos negativos saturados a cero reproducen el tamano y todos los
stores del constructor en `CARSFX_RoadNoise::Play`: **388/392 B y 94,938774 %
-> 392/392 B y 99,64286 %**. La receta mas simple es Wet_FX=-1. Solo quedan
siete operandos, un intercambio coherente r27/r29 entre el cero persistente
y el puntero a mData. Una combinacion de dos argumentos no cambia ese empate.

Se probo despues, SOLO EN SOMBRA, una local inicializada con el puntero real a
mData ligada a r27. No cierra Play (95,5102 %) y hace retroceder InitSFX de
100 % a 97,324326 %. Se rechaza; no se propone editar ENVIRO_AEMS.h.

La mejora parcial sin pin se conserva como receta aislada reproducible, no
como fuente integrada ni como bytes matched. El header real, Roadnoise.cpp
y zEAXSound2.o permanecen byte por byte iguales al baseline r41. Los cierres
protegidos MsgPlayMiscSound (780 B) y MsgBarrier (140 B) siguen exactos.
Detalles y cifras en [r42-sound.md](r42-sound.md).

## Camara: cache ordinaria neutra

Una copia privada de UTLVector.h nombra `curSize = size()` en la rama nula de
Vector::assign y refresca el valor despues de push_back. El compilador elimina
la distincion: siguen 1.156 B / 98,69896 %. Las 587 funciones y todos los bytes
runtime, relocaciones y simbolos son identicos al control y al objeto real.
La cabecera y el objeto de produccion nunca se tocaron. No procede ampliar la
prueba a otros consumidores ni pedir autorizacion para integrar esta variante.
Detalles: [r42-camera.md](r42-camera.md).

## Verificacion y entrega

- Nuevo informe oficial, censo directo completo de 618 unidades y snapshot
  de las 33 SourceLists.
- `codex_r42_global_audit.py`: 18.432 entradas oficiales con las mismas
  direcciones/tamanos; 15.493 entradas SourceList iguales en porcentaje y
  tamano; **delta cero y ninguna regresion**.
- Directo independiente: 3.893.268 / 3.946.204 B, 18.379 / 18.439 funciones;
  su diferencia historica con el informe oficial permanece intacta.
- Repetidos `codex_r41_sound/production_audit.py`,
  `codex_r42_camera/audit.py` y `codex_r42_sound/pin_shadow_gate.py`: PASS.
- DOL principal reconstruido al integrar r41 y hash final vuelto a comprobar:
  **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. Ningun objeto de produccion
  cambia en r42 respecto a aquel enlace.
- `lcfix.py --check` y `git diff --check`: limpios.
- Sin commits, ninja completo, promociones, cambios de splits/configuracion,
  flags compartidos ni cabeceras reales en r42. Todos los territorios liberados.

Instantaneas:
`scratchpad/codex_20260908_r42_{before,after}_{report,measure,pct}.json`.
Los ensayos sombra no cuentan como integrados ni autorizan cambios compartidos.

La proxima hipotesis sobre RoadNoise debe atacar el empate de los dos pseudos
**sobre la receta de 392 B**, sin volver a buscar el cero ausente ni repetir el
pin de mData medido aqui. La cache ordinaria de size() de la shortlist r41 ya
esta medida y no debe presentarse de nuevo como una propuesta sin probar.
