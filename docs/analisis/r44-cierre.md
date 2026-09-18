# R44 — diagnóstico corregido, sin cierres nuevos

2026-09-08. Base `590dc4a0` más cambios locales verificados al cerrar r43.
Tres agentes y coordinación trabajaron sobre cuatro unidades independientes.
**Delta: 0 B / 0 funciones.** Se retiraron todos los cambios experimentales.
El único cambio nuevo de fuente retenido es el comentario UNSOLVED de madidct;
su objeto reconstruido es byte por byte idéntico al anterior.

## Estado verificado

| medida | resultado |
|---|---:|
| código exacto | 3.893.124 / 3.946.048 B — **98,65881 %** |
| funciones exactas | 18.373 / 18.432 |
| código linked | 736.012 B — **18,651876 %** |
| datos exactos | 443.749 / 1.285.741 B |
| datos linked | 319.036 B |
| unidades completas | 478 / 618 |

Quedan 52.924 B / 59 entradas / 27 unidades, incluidos 20 B de padding .init.
El objetivo del 100 % sigue incompleto. El censo directo conserva su diferencia
histórica: 3.893.268 / 3.946.204 B y 18.379 / 18.439 funciones.

Las autorizaciones de ppc2D2 y FX_UVES ya estaban integradas en r39 y r41.
Se revalidaron; sus 344 B linked y 780 B matched no se cuentan de nuevo.
La promoción separada de spchpick sigue pendiente de autorización y no se tocó.

## Trabajo y negativos medidos

- **RoadNoise, sólo sombra:** pin del puntero real mData en r27 con restricción
  sólo de entrada, sobre Wet=-1. Play queda en 392 B / 83,061226 %, pero
  InitSFX regresa a 448 B / 90,4955 %. Rechazado. Los dos cierres protegidos
  MsgPlayMiscSound y MsgBarrier permanecen exactos. [r44-sound.md](r44-sound.md).
- **Animación:** helper cpp privado que calcula la base mediante accesores
  públicos, con offset del índice fuera. EvalState pasa a 492 B / 66,89474 %:
  materializa c+10 como pseudo adicional y salva r28. Retirado; fuente, objeto
  y JSON completos restaurados. [r44-anim.md](r44-anim.md).
- **pathnode:** vista local de pfstate: nextnode 296 B / 91,844154 %; división
  de scalar en dos sentencias: 336 B / 98,5119 %; división in-place de notes:
  idéntica a la base, 336 B / 98,86905 %. Las tres retiradas. La diferencia
  de versión SN v1.83/v1.76 es un posible factor, no prueba de imposibilidad.
  [r44-pathnode.md](r44-pathnode.md).
- **madidct, sombras:** valor real src[6] fijado directamente a CTR provoca
  error interno del backend, sin objeto. La restricción de entrada c sobre
  una local ordinaria compila a 504 B / 39,87597 %; +c da 512 B / 44,279068 %.
  Sólo un mfctr: ninguna reproduce las dos recuperaciones del original.
  Column, idctcompute y datos no cambian. No se retuvo código experimental.

## Corrección importante del diagnóstico de madidct

La auditoría independiente confirma r19/r20 y refuta el comentario r36f:
IdctRow no carece de cinco copias high-word. Ambas compilaciones tienen cinco.
El delta real de 20 B es **mtctr +1, mfctr +2, mr +2**. El objetivo conserva
src[6] en CTR y un acarreo en LR; nuestra base conserva src[6] en LR y el
acarreo en un GPR. Column tiene igual tamaño, pero sustituye dos recargas por
dos copias: tampoco es únicamente una permutación de nombres de registro.

Se corrigieron las 17 líneas del comentario conservando CRLF y número de
líneas. Tras recompilar, el objeto entero mantiene SHA-256
`40c640b0d720575017ef168acee2edea77ee9d02e7c7aa1bb51b5ab526368608`.
El gate demuestra que fuera del comentario la fuente es byte-idéntica.
[Auditoría detallada](r44-madidct-audit.md).

## Verificación final

- `codex_r44_global_audit.py`: los tres JSON completos del censo coinciden con
  la base, no sólo sus totales. 18.432 entradas oficiales y 15.493 entradas
  SourceList, sin cambios de código, datos, tamaños, direcciones ni linked.
- Gates de sonido, animación, pathnode y madidct: PASS. Animación: 314 exactas
  auditadas; idctcompute: 356 B y 63 relocaciones contra ELF original.
- Auditoría estricta de FX_UVES/MsgPlayMiscSound r41 y runtime r39: PASS.
- Enlace principal actual, sin ninja; `dtk shasum`: OK. SHA-1 del DOL:
  **9619ba57c9919f95f7f2ac951a2166a3517f91e3**. Advertencias duplicadas conocidas.
- `checksplits.py`: 1.090 rangos, cero solapes/cortes. `lcfix --check` y
  `git diff --check`: limpios.

Todos los agentes liberaron fuentes y objetos; no quedan compilaciones activas.
Sin nuevos cambios de headers, configuración, splits, flags, staging ni commits.
Instantáneas: `scratchpad/codex_20260908_r44_{before,after}_{report,measure,pct}.json`.

## Continuación

No repetir los controles r43/r44 de RoadNoise ni el helper público de animación.
Para madidct, la siguiente investigación debe reconstruir las vidas intermedias
de las cinco operaciones de 64 bits, no añadir copias supuestamente ausentes.
La ausencia de inlines MULT en el DWARF original es una pista, no demostración
de que fuese macro; la forma macro simple ya está medida y no casa. Cualquier
nueva prueba debe distinguir una dependencia real del efecto de forzar CTR.
