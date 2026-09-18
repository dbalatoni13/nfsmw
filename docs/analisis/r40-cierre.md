# r40 - Effect_Update cerrado, +992 bytes exactos

2026-09-07. Base `590dc4a0`, preservando todo el trabajo local de r37-r39.
Tanda de tres agentes y coordinacion. Todos los territorios estan liberados;
no quedan pruebas, ediciones de fuentes ni compilaciones en marcha.
**El objetivo del 100 % sigue pendiente.**

## Progreso oficial verificado

| medida | antes r40 | despues r40 | delta |
|---|---:|---:|---:|
| codigo exacto | 3.891.352 / 3.946.048 B | **3.892.344 / 3.946.048 B** | **+992 B** |
| porcentaje exacto | 98,61391 % | **98,63904 %** | +0,02513 pp |
| funciones exactas | 18.371 / 18.432 | **18.372 / 18.432** | **+1** |
| codigo linked | 736.012 B / 18,651876 % | igual | 0 |
| datos exactos | 443.749 / 1.285.741 B | igual | 0 |
| datos linked | 319.036 B | igual | 0 |
| unidades completas | 478 / 618 | igual | 0 |

Quedan **53.704 B / 60 entradas de funcion / 27 unidades**. La cuenta incluye
20 B de padding automatico `.init`, que no son una funcion del juego.

El censo directo independiente suma **3.892.488 / 3.946.204 B**, 18.378
funciones exactas y 618 unidades, frente a 3.891.496 B / 18.377 antes.
Confirma exactamente +992 B / +1. La discrepancia historica con el informe
oficial sigue intacta y no se cuenta como progreso.

## Cambio retenido

`src/LibSN/steering.c`: **Effect_Update, 992/992 B y 248 instrucciones al
100 %**, desde 99,83871 %. La carga volatile localizada del campo real
`ramp.start`, combinada con un acumulador de rampa de alcance de funcion
separado del pseudo periodico `v`, reproduce las vidas de registro del
original. No hay cuerpos ASM, valores indefinidos ni nuevas lecturas runtime.

El agente y coordinacion ejecutaron el auditor estricto: **55 ramas, 27
reubicaciones con destinos/tipos/addends correctos y 992 bytes resueltos
contra el ELF original**. No cambia ningun otro simbolo ni ninguna seccion de
datos. El cierre r39 de GetFreeEffect (116 B) permanece intacto.

Steering pasa de 27/36 a **28/36 exactas** y de 4.796 a **5.788 bytes
exactos**; quedan ocho funciones / 2.972 B. Sigue NonMatching: cerrar esta
funcion no equivale a incorporar toda la unidad al enlace, y su layout de
datos pendiente ya esta documentado en r39. Detalles en
[r40-steering.md](r40-steering.md).

## Ensayos retirados y aislamiento

- **CarLoader / zWorld**: siete combinaciones nuevas de retorno ABI real,
  copia para el store y preparacion del nombre/argumentos. Ninguna cierra.
  C1 arregla el orden pero elimina 4 B; la cadena final devuelve el tamano y
  desordena cargas. Fuente restaurada byte por byte, CRLF incluido; JSON
  completo anterior/posterior identico: 2.883 simbolos, 23 secciones y 582
  funciones medidas, las mismas 577 exactas. [r40-world.md](r40-world.md).
- **JoyE / zPlatform**: comprobado mecanicamente que las cinco copias que
  siguen al `andi.` sobrante del objetivo no tienen consumidores reales.
  Cinco ensayos retirados, fuente y ambos lados del JSON final identicos al
  baseline; 135/136 exactas auditadas. [r40-platform.md](r40-platform.md).
- **Frontend / zFe2 y zFeOverlay**: ni la combinacion de dependencias de
  SetScreenNames ni los ensayos de stores/CSE de IconScroller cierran. Fuentes
  restauradas, 1.774 funciones con mismos tamanos/porcentajes; pasan las
  1.306 y 466 exactas respectivas. feIconScroller conserva CRLF puro.
  [r40-fe.md](r40-fe.md) identifica tambien la revalidacion de una prueba de
  r37; no debe venderse como hipotesis nueva.

## Verificacion global y del enlace

- `codex_r40_global_audit.py`: **18.432 entradas oficiales**, mismas
  direcciones/tamanos objetivo, ninguna regresion y solo Effect_Update mejora.
- Censo completo `measure.py`: +992 B/+1, mismos inventarios y denominadores.
- `pctsnap.py --umbral 0`: las **15.493 entradas de las 33 SourceLists son
  exactamente iguales**, incluso los tamanos fuente. El cierre es una
  biblioteca MW, por eso no aparece en este subconjunto.
- Se reconstruyo el **DOL principal** con el listado de objetos generado,
  sin ninja completo: `dtk shasum` OK,
  **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. Las advertencias de simbolos
  duplicados existentes no impidieron obtener el DOL exacto.
- Auditor runtime r39 repetido: los 100 B GCC + 244 B MW siguen integrados
  exactamente como se autorizo; nueve funciones, sin datos ni padding nuevos.
- `checksplits.py`: 1.090 rangos, cero solapes y cero cortes de simbolos.
- `lcfix.py --check` y `git diff --check`: correctos. Los avisos de conversion
  CRLF de ficheros anteriores no son cambios nuevos de esta tanda.

**No se modificaron configure.py, splits, flags ni cabeceras en r40.** Sus
cambios locales siguen siendo los aprobados e integrados en r39. spchpick
continua NonMatching: su promocion de 7.836 B aun requiere la respuesta a su
pregunta separada. No inferir esa autorizacion de la aprobacion de __va_arg.
No hay git add ni commits. La fuente nueva __va_arg.c de r39 sigue untracked.

## Siguiente tanda preparada: propuestas aun no medidas

La revision final de steering fue SOLO LECTURA; no altera este baseline.

1. **CookValues, 260 B / 95,76923 %**. Primera causa: target conserva el byte
   crudo en r5 (`lbz`) y su promocion firmada en r6 (`extsb r6,r5`); la fuente
   fusiona ambas representaciones en r6. Probar separacion local del byte real
   y su valor firmado solo para el eje steering, conservando signedness de
   stores/comparaciones. Primero lectura ordinaria; solo si MW fusiona,
   control con lectura volatile localizada. No tocar AUTOCAL compartido ni
   repetir las variantes p1-p3 de cache de maximo.
2. **VDevice_DownloadEffect, 188 B / 99,3617 %**. Seis operandos forman un
   intercambio r30/r31 entre ret e idx. Probar una unica lectura real de
   `*pid` despues de la llamada, materializada en `effectId`, manteniendo
   ret/idx separados. El target ya tiene una unica carga. Rechazar si la
   lectura localizada o las derivaciones añaden/reordenan instrucciones.

Estas son hipotesis, no mejoras conseguidas. La aceptacion sigue exigiendo
100 %, auditoria de destinos/datos y cero regresiones. Los otros seis frentes
de steering requieren evidencia nueva antes de reabrir las familias vedadas.

Instantaneas completas:
`scratchpad/codex_20260907_r40_{before,after}_{report,measure,pct}.json`.
Informe vigente: `build/GOWE69/report.json`. Auditores principales:
`scratchpad/codex_r40_global_audit.py`, `codex_r40_world_audit.py` y los
auditores de `codex_r40_steering/` y `codex_r40_platform/`.
