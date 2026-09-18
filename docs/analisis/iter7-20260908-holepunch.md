# Iteración 7 — `WRoadNav::HolePunchAvoidables`

## Resultado

No se conserva ningún cambio. Las tres formas nuevas y acotadas fueron
negativas o neutras. Producción permanece en 2.980 B, 99,919464 % y nueve
instrucciones distintas.

- Fuente: `src/Speed/Indep/Src/World/Common/WRoadNetwork.cpp`, SHA-256
  `f71f3100b05bc845639c975ae139ae475cc6194b3af3948e2cbe7254a955d633`.
- Objeto: `build/GOWE69/src/Speed/Indep/SourceLists/zWorld2.o`, SHA-256
  `bc6cf9867ac1c7a482db8f0096ca825fa61896208254291372b684a292ea5395`.
- `fncmp`: las únicas funciones con código distinto siguen siendo
  `HolePunchAvoidables` (9) e `InitAtSegment` (15), 2 de 357 mapeadas.
- `lcfix.py zWorld2 --check`: todas las entradas al día.

Artefactos reproducibles:

- `scratchpad/codex_20260908_iter7_holepunch/compile_shadow.py`;
- `scratchpad/codex_20260908_iter7_holepunch/audit.py`;
- `scratchpad/codex_20260908_iter7_holepunch/audit_result.json`;
- los cuatro pares `*.o`/`*.json` de control y variantes.

## Las nueve diferencias reales

La salida de objdiff y `fndiff` se contrastó con los ELF y con el RTL guardado
en `scratchpad/codex_20260908_resume_world/rtl`. Las nueve filas son
`DIFF_ARG_MISMATCH`: no hay inserciones, instrucciones ausentes ni destinos de
salto diferentes después de aplicar las relocs.

### CR: cuatro filas, un único intercambio

| Offset | Objetivo | Fuente |
|---:|---|---|
| `+0x2c0` | `cmpwi cr2,r29,0` | `cmpwi cr3,r29,0` |
| `+0x2c8` | `cmpwi cr3,r31,0` | `cmpwi cr2,r31,0` |
| `+0x3c4` | `beq cr2`, mismo destino interno | `beq cr3`, mismo destino interno |
| `+0xa14` | `beq cr3`, mismo destino interno | `beq cr2`, mismo destino interno |

El RTL confirma la causa:

| Local | pseudo | refs | vida | prioridad entera | asignación nuestra |
|---|---:|---:|---:|---:|---|
| `is_traffic` | 505 | 3 | 483 | 62 | `cr4` |
| `is_racer` | 334 | 3 | 494 | 60 | `cr3` |
| `is_drag` | 902 | 3 | 492 | 60 | `cr2` |

`is_racer` e `is_drag` empatan tras truncar la prioridad; el pseudo menor gana
el primer CR disponible. El objetivo sólo invierte esos dos CR. Esto confirma
que las ramas de objdiff no representan un cambio de CFG.

### FPR: cinco filas del productor `bCross`

| Offset | Objetivo | Fuente |
|---:|---|---|
| `+0x818` | `lfs f10,0x10(r30)` | `lfs f9,0x10(r30)` |
| `+0x83c` | `lfs f9,0x14(r30)` | `lfs f8,0x14(r30)` |
| `+0x860` | `fmuls f10,f10,f13` | `fmuls f9,f9,f13` |
| `+0x868` | `lfs f0,0x1b8(r1)` | `lfs f10,0x1b8(r1)` |
| `+0x86c` | `fmsubs f10,f0,f9,f10` | `fmsubs f10,f10,f8,f9` |

La base sin pin ya documentada da los temporales del objetivo, pero coloca el
resultado en f11. Su pseudo 689 tiene 4 refs, vida 22 y prioridad 3.636; recibe
la preferencia de `offset_change`. El pin actual fija el resultado en f10 y
mantiene exacta la cadena posterior, pero impide el coalescing del productor.

## Formas nuevas medidas

Todas se compilaron desde un `.i` privado con los flags reales. El control
preprocesado reproduce todas las secciones allocables, código y relocs de
producción; sólo cambia información debug fuera del runtime.

| Forma | Resultado | Evidencia causal |
|---|---:|---|
| V1: declarar `offset_change` y asignarlo en la sentencia siguiente | 9 filas, 99,919464 % | el front-end reconstruye el mismo árbol/preferencia; runtime byteidéntico al control |
| V2: `approach_time * avoidable_delta_offset` | **10 filas**, 99,906044 % | no rompe la preferencia y añade en el índice 561 `fmuls f10,f31,f10`; única función runtime modificada |
| V3: retirar `const` de la local real `is_drag` | 9 filas, 99,919464 % | el flag readonly no interviene en el empate; runtime byteidéntico |

No se repitieron permutaciones de declaraciones, pines, aliases, barreras ni
formas de las rondas r36/r46/resume.

## Gate de unidad

`audit.py` da `PASS_NEGATIVE_SHADOW_GATE` y verifica:

- inventario de 357 funciones objetivo mapeadas y las 500 `STT_FUNC` del
  objeto fuente;
- mismos nombres, secciones, offsets, tamaños y bindings en las sombras;
- igualdad de todas las secciones allocables y datos fuera del cuerpo bajo
  prueba;
- identidad de todas las relocaciones runtime de la unidad;
- V1 y V3 sin ningún byte runtime distinto; V2 cambia únicamente
  `HolePunchAvoidables`;
- fuente y objeto de producción conservados con los hashes anteriores.

## Siguiente palanca acotada

Estas tres pruebas no demuestran imposibilidad. Sí descartan que el empate CR
dependa del `const` y que la preferencia FPR dependa de la sintaxis básica de la
inicialización o del orden del producto. Una hipótesis posterior necesita una
dependencia fuente real que cambie la preferencia del pseudo 689 sin
precolorearlo, o que cambie la vida efectiva 494/492 sin materializar los bool
en GPR. Sin esa evidencia, otra permutación sería repetir un barrido agotado.
