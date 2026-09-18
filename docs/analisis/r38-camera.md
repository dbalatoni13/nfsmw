# Ronda 38 — zCamera

## Resultado

No se cerro ninguna funcion. Todos los ensayos quedaron restaurados y el delta
retenido es **0 B / 0 funciones**.

La unidad vuelve exactamente a su baseline: 114.196/125.008 B (91,3510 %) y
448 funciones exactas. El inventario comparado por objdiff contiene 587
funciones emitidas; las 587 vuelven a ser byte por byte identicas al snapshot
inicial, incluidas sus relocaciones.

## Estado real de las candidatas

- `__static_initialization_and_destruction_0`: objetivo 3.604 B, fuente
  3.620 B, 97,1121 %.
- `CameraMover::TerrainVelocityNoise`: 1.192/1.192 B, 99,07718 % y 13 filas.

El static-init conserva 905 instrucciones nuestras. Sus cuatro instrucciones
estructurales de mas siguen siendo:

| indice | instruccion |
|---:|---|
| 796 | `addi r27,r30,0x30` |
| 839 | `addi r27,r30,0x48` |
| 858 | `addi r27,r30,0x60` |
| 877 | `addi r27,r30,0x78` |

No se reintrodujo ningun relleno ni cantidad fantasma y no se conservaron
restricciones ASM parciales.

## Hallazgo nuevo: el arbol inline es identico

El DWARF del ELF original conserva dentro del cuerpo sintetico todas las
expansiones inline, incluso las de rango cero. Se extrajo especificamente el
cuerpo `0x8008095c..0x80081770` para evitar confundirlo con los otros 45
static-initializers del ejecutable y se comparo con el volcado de zCamera.

Resultado: **90 expansiones inline en cada lado, misma firma y mismo orden**.
El diff ordenado esta vacio y tampoco hay diferencias de cardinalidad. Esto
descarta que el presupuesto CSE divergente venga de un constructor/helper
ausente, duplicado o situado en otro punto de la secuencia. La diferencia esta
dentro del RTL producido por cuerpos inline equivalentes o en su estado de CSE,
no en el arbol de llamadas del inicializador.

El `regmap` del static-init tambien confirma los dos parametros, el arbol de
ambitos y el reparto declarable identicos. El excedente sigue siendo de
temporales internos del compilador.

Herramienta reproducible:
`scratchpad/codex_r38_camera/dwarf_inline_diff.py`.

## Ensayos nuevos y acotados

| ID | Hipotesis | Resultado |
|---|---|---|
| c1 | `ICE::Vector3(float,float,float)` con lista de inicializacion en vez de cuatro asignaciones | objeto y presupuesto identicos: 905 instrucciones, cuatro `addi` |
| c1b | un `bVector3` global con inicializacion por copia elidida | presupuesto identico: 905, cuatro `addi` |
| c2 | dependencia `+f` sobre el argumento real `0.02f` del ultimo `vIceAccelLagScale` | presupuesto identico: 905, cuatro `addi` |
| c3 | puntero `self=this` inicializado y dependencia `+r` dentro de `ICE::Vector3` | cambia CSE, pero elimina seis instrucciones finales anteriores: 899; los cuatro `addi` permanecen |
| c4 | dependencia `+r` sobre el parametro inicializado `p_car` antes del bloque de cinco literales de `TerrainVelocityNoise` | empeora a 1.200 B y 96,94631 % |
| c5 | dependencia de solo entrada sobre `p_car` en el mismo punto | binario identico a la baseline: 1.192 B y 99,07718 % |

El ensayo c3 demuestra que el puntero del constructor es una palanca real sobre
CSE, pero no sobre la causa buscada: cambia codigo final que en el objetivo ya
coincidia y no afecta al plegado de `ReplayCategoryTable`. Se retiro.

En `TerrainVelocityNoise`, `regmap` vuelve a dar 19/19 locales y parametros
identicos. La barrera read/write introduce dos instrucciones reales; la de solo
lectura no cambia el reparto de los cinco literales. No hay evidencia nueva para
repetir las cantidades fantasma o las barreras FP vedadas por r36f.

## Auditoria final

- `build_direct.py zCamera`: correcto.
- `fndiff` final: static-init 97,1121 %, 3.604/3.620 B;
  `TerrainVelocityNoise` 99,07718 %, 1.192/1.192 B.
- `fncmp`: los mismos cinco cuerpos pendientes y los mismos 10.812 B.
- `audit.py Speed/Indep/SourceLists/zCamera`: las 448 exactas correctas.
- `litpos.py`: 849 referencias emparejadas, 0 valores distintos, 0 sin
  resolver.
- Datos restaurados: `.rodata` 8.488 B, `.data` 2.884 B, `.bss` 1.928 B y
  `.ctors` 4 B, incluidas relocaciones.
- `lcfix.py --check`: limpio.
- `codex_r38_camera_audit.py`: PASS para 587 funciones, datos y medida.

La marca modificada que Windows puede mostrar en `ICEMover.cpp` es solo cache
de finales de linea: `git diff` esta vacio y `git hash-object` coincide con
`HEAD` (`35194cefe9308f074fb542fe38b86878cb92fbe5`). No queda cambio de fuente
Camera retenido.

## Artefactos

- `scratchpad/codex_r38_camera_before.json`
- `scratchpad/codex_r38_camera_after.json`
- `scratchpad/codex_r38_camera_c1.json`
- `scratchpad/codex_r38_camera_c2.json`
- `scratchpad/codex_r38_camera_c3.json`
- `scratchpad/codex_r38_camera_before_measure.json`
- `scratchpad/codex_r38_camera_after_measure.json`
- `scratchpad/codex_r38_camera_audit.py`
- `scratchpad/codex_r38_camera/diag.py`
- `scratchpad/codex_r38_camera/dwarf_inline_diff.py`

## Siguiente paso legitimo

La proxima tanda no deberia volver a cambiar la forma de
`ReplayCategoryTable`, el constructor de `bVector3`, el de `ICE::Vector3` ni
añadir barreras a sus argumentos. Hace falta comparar el RTL inicial de los
cuerpos inline que aparecen en las 90 expansiones, empezando por `bFill` y los
constructores de tablas anteriores, y localizar cual produce instrucciones
intermedias distintas aunque su ensamblador final coincida. Sin esa evidencia,
seguir desplazando el limite de CSE seria un barrido ciego.
