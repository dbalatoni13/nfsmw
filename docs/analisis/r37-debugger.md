# R37 — `DebuggerDriver.c`

## Alcance y resultado

Fuente exclusiva examinada:

- `src/Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c`
- objeto real `build/GOWE69/src/Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.o`

No se conserva ningún cambio de fuente. El fichero y el objeto final son idénticos byte a
byte al baseline de esta subtanda. Continúan pendientes las mismas tres funciones (804 B):

| función | tamaño | similitud | diferencia restante |
|---|---:|---:|---|
| `EXI2_WriteN` | 432 B | 94.44444 % | 4 posiciones del epílogo |
| `EXI2_Poll` | 264 B | 99.818184 % | 2 instrucciones y sus 2 relocaciones, sólo orden de cargas del epílogo |
| `AmcEXISetExiCallback` | 108 B | 85.18519 % | 4 posiciones del epílogo |

Las otras 28 funciones permanecen exactas. `EXI2_ReadN` aparece como diferencia nominal de
alias en `fncmp`, pero la auditoría simbólica genérica la valida al 100 %.

## Estado exacto de los epílogos

`EXI2_Poll` ya tiene el cuerpo exacto gracias a `91629d16`; sólo difiere:

```text
target: lwz r0,0x24(r1); lwz r3,0x18(r1); addi r1,...; mtlr r0; blr
source: lwz r3,0x18(r1); lwz r0,0x24(r1); addi r1,...; mtlr r0; blr
```

En `EXI2_WriteN`, el objetivo restaura `r31/r30`, hace `mtlr`, restaura `r29/r28` y después
libera el frame; GC/1.2.5n restaura los cuatro registros y libera el frame antes de `mtlr`.
En `AmcEXISetExiCallback`, el objetivo carga LR antes del `mr` de retorno y ejecuta `mtlr`
antes de liberar el frame; el compilador actual intercambia ambos pares.

## Evidencia histórica y ensayos

Se revisaron completos `r36f-lib.md`, `r36e-jf-ventana.md`, las notas de fases 7/8, los
comentarios de la fuente y el historial `91629d16`/`8ef3035a`. Las rondas anteriores ya
agotaron ocho formas de latch, veintiún pragmas, siete combinaciones `volatile`, helpers,
retorno común, temporales PI, permutaciones de argumentos y modelos de procesador.

Ensayos nuevos y acotados:

1. Barrera GNU `+m` sobre el valor de retorno de `EXI2_Poll`: MWCC no acepta esta sintaxis de
   constraints. El build falló antes de producir un objeto; la fuente fue restaurada. El
   archivo `trial_poll_plusm.txt` corresponde por tanto al objeto anterior, no a una variante.
2. Representar el retorno de `EXI2_Poll` como `u32 data[1]` y pasar el array a `DBGReadStatus`:
   compiló correctamente, pero produjo exactamente el mismo objeto y el mismo epílogo. El
   compilador lo canonicaliza al escalar original. Se restauró la fuente.

No apareció una hipótesis C nueva para `WriteN` o el callback distinta de las familias ya
documentadas, así que no se repitieron barridos.

Como control diagnóstico, `compiler125.json` muestra que GC/1.2.5 coloca correctamente los
tres epílogos, pero ese objeto antiguo (anterior al arreglo actual del latch de `Poll`) regresa
`DBRead`, `DBQueryData`, `DBInitComm` y también difiere en `EXI2_Init`. Confirma que el cuello
es de versión/scheduling del compilador, pero cambiar compilador o flags queda fuera del
alcance y no sería una mejora sin regresiones.

## Verificación final

- build real aislado con `scripts/build_direct.py`: correcto;
- `fncmp` final: exactamente 3/31 pendientes, 804 B, idéntico al baseline;
- `scripts/audit.py`: 28 funciones exactas, llamadas, ramas, relocaciones y alias validados;
- `scripts/lcfix.py DebuggerDriver --check`: todos los `@lc` actuales;
- auditoría propia: 57 símbolos, 31 funciones, 1497 posiciones de instrucción, 147 ramas,
  158 referencias y 10 secciones idénticas; bytes, alineaciones y relocaciones sin cambios;
- SHA-256 fuente final/baseline:
  `9FA0B8978E5E8847A7D5BFE7DBAE610AE95FC374DBF9E4FDE27CCFA56C0C3A45`.

Artefactos: `scratchpad/codex_r37_debugger/`. Los principales son `before.json`,
`after.json`, `before_fncmp.txt`, `after_fncmp.txt`, `epilogues.txt`, `final_audit.txt`,
`lcfix_check.txt`, `audit_restoration.py` y `restoration_audit.txt`.
