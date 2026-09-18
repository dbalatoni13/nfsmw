# Reanudación 2026-09-08 — zWorld2 / HolePunchAvoidables

## Resultado

No se conserva ningún cambio de esta ronda: `HolePunchAvoidables` continúa en
2980 B y 99,919464 %, con las nueve filas conocidas. La fuente y el objeto de
producción se restauraron byte a byte a la base nueva (HEAD `8853122e`), sin
revertir las correcciones r46 de `bCross`, reasociación y `bMax`.

- Fuente final SHA-256:
  `F71F3100B05BC845639C975AE139AE475CC6194B3AF3948E2CBE7254A955D633`.
- Objeto final SHA-256:
  `BC6CF9867AC1C7A482DB8F0096CA825FA61896208254291372B684A292EA5395`.
- El JSON objdiff final y el JSON de entrada son idénticos:
  `scratchpad/codex_20260908_resume_world/{d_zWorld2.json,final/d_zWorld2.json}`.
- `fncmp` final: 357 funciones; sólo las dos pendientes previas tienen código
  distinto (`HolePunchAvoidables`, 9 filas, e `InitAtSegment`, 15). No apareció
  ninguna regresión nueva. `lcfix.py zWorld2 --check`: limpio.

## Primera grieta RTL real

El volcado nuevo, generado con los flags reales en
`scratchpad/codex_20260908_resume_world/rtl/`, corrige dos premisas históricas:

- `is_traffic`: pseudo 505, 3 refs, vida 483, prioridad entera 62, `cr4`.
- `is_racer`: pseudo 334, 3 refs, vida 494, prioridad entera 60, `cr3`.
- `is_drag`: pseudo 902, 3 refs, vida 492, prioridad entera 60, `cr2`.

`global.c:allocno_compare` trunca a entero la prioridad. Por eso racer y drag
empatan en 60 y el pseudo menor (334) gana: no tienen «igual rango», como decía
el comentario antiguo, pero sí igual prioridad efectiva. Para invertirlos sin
alterar el resto habría que llevar la vida de drag a 491 o menos, o la de racer
a 501 o más. Añadir instrucciones dentro del bucle no sirve: por el back-edge
alarga las dos vidas.

La segunda causa también queda localizada. Sin el pin de
`avoidable_delta_offset`, el pseudo 689 tiene 4 refs/vida 22 y el asignador
global lo coloca en f11 por la preferencia heredada de `offset_change` (pseudo
725, asignado localmente a f11). Así salen exactamente los operandos temporales
del objetivo, pero el resultado queda en f11. El pin actual fuerza el resultado
a f10 demasiado pronto y cambia los tres temporales del `bCross`, produciendo
las cinco filas 518/527/536/538/539.

## Formas medidas y descartadas

1. **Pin trasladado a `offset_change` + copia y multiplicación separadas.**
   La forma usa sólo las dos locales reales. En RTL hace que el primer
   `fmsubs` sea exactamente `fmsubs f10,f0,f9,f10`, pero el objeto real desplaza
   cantidades posteriores: 9 -> 39 filas, 99,919464 -> 98,88457 %. Revertida.
   Evidencia: `v1_pinoffsetsplit/`.

2. **Preferencia localizada mediante alias definido de `offset_change`.**
   Se probó un valor real ligado a f10 y una barrera vacía no volátil; no hay
   valor fantasma. Aunque el RTL aislado recupera los `fmsubs` buscados, el
   objeto real cambia planificación, GPR, FPR y colocación de pools desde antes
   del bloque: 95,010735 %. La versión en una sola línea física da el mismo
   resultado, por lo que no era un efecto de numeración posterior. Revertida.
   Evidencia: `v2_hintoffset/` y `v2b_hintoffset_oneline/`.

3. **Control algebraico `-bCross(Forward, velocity)`.**
   El `fold` vuelve a la misma forma RTL que la base y emite el mismo
   `fmsubs f10,f10,f8,f9`; no aporta una palanca de reparto. Sólo se midió en
   scratch y se descartó antes de tocar producción.

La conclusión útil es que el techo no está en otra permutación de expresiones:
hace falta una dependencia fuente legítima que cambie la preferencia de 689
sin precolorear el destino y, separadamente, que mueva uno de los umbrales de
vida CR. No se justifica conservar un alias artificial ni una mejora parcial.

## Reproducción

- RTL: `python scratchpad/codex_20260908_resume_world/rtl_dump.py` y variantes
  documentadas por las opciones del script.
- Tabla de allocnos: `python scripts/alloc.py
  scratchpad/codex_20260908_resume_world/rtl/zWorld2.i "void
  WRoadNav::HolePunchAvoidables(NavCookie *, int, float, float)"`.
- Gate final read-only: `python
  scratchpad/codex_20260908_resume_world/audit.py`.
