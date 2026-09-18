# R45 — zSpeech / `RoadblockFlow::Setup`

## Resultado

No se retiene ningún cambio. `Setup__Q26Speech13RoadblockFlow` permanece en
596 B y 98,255035 %. Los dos ensayos iniciales, acotados a cambiar el origen de
la expansión inline, generaron el mismo código de la línea base para las 703
funciones de la unidad. Una comprobación adicional con puntero-a-miembro fue
negativa y quedó también restaurada. Fuente y objeto terminaron byte a byte en
la línea base.

## Diagnóstico actual

`fndiff` localiza toda la diferencia en la última llamada virtual,
`primary->PursuitApproaching()`:

```
objetivo: lwz r9,0(r31); lwz r0,0x2e4(r9); lha r3,0x2e0(r9);
          mtlr r0; add r3,r31,r3; blrl
nuestro:  lwz r9,0(r31); lha r3,0x2e0(r9); lwz r0,0x2e4(r9);
          add r3,r31,r3; mtlr r0; blrl
```

No hay una instrucción, rama, literal ni llamada ausente: son dos desempates de
planificación dentro del thunk, presentados por `fndiff` como cuatro filas. El
DWARF confirma `this=r28`, `ai=r29`, `should_interrupt_dispatch=r31` y el
`primary` del bloque final en `r31`; el rango y árbol de ámbitos coinciden. El
mapa de líneas atribuye además el `GetDispatch()` inline inmediatamente anterior,
pero no lista ningún inline alrededor de `PursuitApproaching()`.

Antes de ensayar se releyeron `brief-r36f.md`, `r28-mx.md`, `r31-ub.md`,
`r36b-snd.md`, `r36d-fe.md` y `r36f-snd.md`. Quedan por tanto vedadas y no se
repitieron: cambio de polaridad/llaves, copia de `primary`, cachear dispatch,
formas `(*primary)`, barreras vacías sobre puntero/memoria/vptr y el barrido de
sintaxis de la llamada.

## Ensayos nuevos

1. **V1 — helper privado inline sólo para la virtual.** Se trasladó
   `cop->PursuitApproaching()` a una función `static inline` del `.cpp`. La
   hipótesis era que un árbol inline distinto cambiara los UID del front-end sin
   añadir trabajo real. Resultado: 596 B, 98,255035 %, cuatro filas; inventario
   `fncmp` completo idéntico a la base.
2. **V2 — helper privado inline para el par dependiente.** Se trasladaron juntos
   `ai->GetDispatch()->RBUpdate(cop,1)` y la llamada virtual. Esto probaba si el
   límite de inline anterior al thunk era la causa del desempate. Resultado
   idéntico a V1 y a la base en todo `fncmp`.
3. **V3 — puntero-a-miembro constante al método real.** No aparecía en ninguna
   veda previa. La firma se comprobó tanto en `EAXCop.h` como en el DWARF:
   `virtual void EAXCop::PursuitApproaching()`. Se midió exactamente
   `void (EAXCop::*invoke)() = &EAXCop::PursuitApproaching;` seguido de
   `(primary->*invoke)()`. El compilador no redujo la representación PMF a la
   llamada virtual directa: materializó el descriptor de ocho bytes desde el
   pool y emitió las rutas generales para función virtual/no virtual y ajuste de
   `this`. `Setup` creció de 596 a **676 B** y cayó a **85,32886 %**. El resto
   del inventario `fncmp` no cambió. Variante descartada y restaurada.

Los JSON completos de V1/V2 no son byte a byte iguales al JSON base porque el
helper privado altera metadatos/símbolos locales del objeto aun desapareciendo
del código; por eso la afirmación de neutralidad se apoya en los inventarios
`fncmp` de las 703 funciones, no en normalizar esos JSON. Al restaurar, el JSON
completo y el objeto sí vuelven a ser byte a byte iguales a la línea base.

No se abrieron más variantes. La evidencia nueva acota el techo: ni el origen
inline ni el agrupamiento con la llamada anterior controlan esos UID; la forma
PMF, aunque distinta, activa una expansión ABI genérica de 80 B extra y no es
una vía para reordenar sólo los dos `load` del thunk.

## Gates y artefactos

- `build_direct.py Speed/Indep/SourceLists/zSpeech`: 1/1.
- `fncmp`: 701 funciones exactas, sólo `Setup` (596 B) y la ya vedada
  `LoadSpeechBank` (316 B) pendientes; 122 diferencias sólo de nombre/alias.
- `audit.py`: 701 `ok`, 0 `FALLA`; valida tamaño, ramas, relocs y literales por
  función exacta.
- `lcfix.py --check`: limpio.
- SHA-256 fuente restaurada:
  `f8bffab57634660e63c94c7b64a752615321fa998c1f7d551ec355b09d491251`.
- SHA-256 objeto restaurado:
  `e66d13a0766ed3c26d221bf37b67ccdf6d1be563b6f0e64e8c71b92be56bedd9`.
- `before.json == after.json == after_pmf.json` y
  `before_fncmp.txt == after_fncmp.txt == after_pmf_fncmp.txt`, byte a byte. Esto
  cubre también que datos, secciones, símbolos y relocs regresaron al estado
  estable inicial después del ensayo PMF.
- Gate reproducible: `python scratchpad/codex_r45_speech/final_gate.py`.

Artefactos completos en `scratchpad/codex_r45_speech/`: snapshots `before` y
`after`, diffs, inventarios de V1/V2, auditoría y salida de compilación final.
