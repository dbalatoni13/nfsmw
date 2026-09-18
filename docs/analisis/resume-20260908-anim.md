# Resume 2026-09-08 — `zEagl4Anim`

## Resultado

No se retuvo ningún cambio de producción. La única variante nueva de C++ fue
neutra y se retiró. La unidad vuelve **byte a byte** a la base autoritaria de
esta ronda:

| función | objetivo / fuente | estado final |
|---|---:|---:|
| `DynamicLoader::Initialize` | 2.352 / 2.356 B | 98,87585 %, pendiente |
| `FnRunBlender::FindMatchTime` | 720 / 720 B | 97,361115 %, pendiente |
| `FnStatelessQ::EvalSQT` | 1.480 / 1.480 B | **100 % protegido** |

El inventario final sigue siendo **3 de 318 funciones** con código distinto,
3.528 B no exactos. La tercera es `FnRawStateChan::EvalState` (456 B), fuera
del ámbito de esta ronda. `RawStateChan.cpp` no se editó.

## `FindMatchTime`: causa localizada en RTL

La primera diferencia no nominal continúa en la conversión de `n` a `float`:

```text
objetivo: lis r9,bias@ha     ... lfd f12,bias@l(r9) ... stfs f13,8(r1) ... bso
fuente:   stfs f13,8(r1)    ... bso ... lis r9,bias@ha; lfd f13,bias@l(r9)
```

El volcado completo de GCC (`rtl_dump.py`, opciones `-da`) acota mejor la
causa que las rondas anteriores:

- En `.sched`, el planificador previo a reload sí iza el pseudo 246, la parte
  alta de `$LC326`, al bloque 0 antes del primer salto.
- En `.lreg`, esa definición queda marcada como eliminada y aparece la
  rematerialización 555 en el bloque 1.
- En `.greg`, el pseudo 246 termina asignado a `r9`; la constante float vecina
  usa otro pseudo global (`248`, `r23`).
- En `.sched2` ya sólo queda la rematerialización tardía de `$LC326`.

Por tanto, el desfase no nace de la posición textual del `if`: scheduler intenta
la forma objetivo, pero local allocation/reload la deshace. Esto explica por
qué las variantes históricas de locales, inicializadores, uniones y barreras no
movieron la carga correctamente.

### Variante nueva medida

Se introdujo temporalmente un helper privado e inline:

```cpp
static inline float FindMatchTimeIntAsFloat(int value) { return value; }
```

y se usó sólo en la comparación con `mSearchLength`. Era una prueba acotada de
si un formal real separado cambiaba el nacimiento/uso del pseudo de conversión
antes de local allocation. El código de `FindMatchTime` fue **idéntico** a la
base: 720 B, 97,361115 %, las mismas 11 instrucciones y 14 relocaciones
nominales. Sólo cambiaron números internos `$LC` por la entidad DWARF adicional.
El helper se retiró de inmediato.

No se probaron de nuevo `float n`, uniones, barreras, inicialización en la
declaración ni reordenaciones globales: esas familias ya están vedadas en
`r38-anim.md` y `r43-anim.md`.

## `Initialize`: árbol del `switch` confirmado como familia agotada

La primera diferencia sigue siendo `add r30,r11,r10` contra
`add r30,r10,r11`; la diferencia estructural dominante posterior es el árbol
de `switch (sheader->sh_type)`:

```text
objetivo: cmp 8 / beq; cmplwi 8 / ble bajo; alto inline;
          bajo: cmp 3; cmplwi 3; cmplwi 1 / ble; caída a case 2
fuente:   cmp 8 / beq; cmplwi 8 / bgt alto;
          bajo: cmp 3; cmplwi 3; cmp 2 / beq; branch default
```

Antes de encontrar todas las notas antiguas se compilaron tres micro-sombras
aisladas (`switch_split_4`, `switch_split_7`, `switch_low_range`). Ninguna tocó
producción y ninguna produjo el árbol objetivo. La revisión posterior de
`r19-plat.md`, `r27-ae.md`, `r36c-snd.md` y `r46-ecs.md` confirmó que esta misma
familia ya acumulaba más de 80 formas completas/micro, incluidos los 16 patrones
break/continue, particiones 4..7, rangos bajos/altos y colocación de default.
No se abrió una variante real: habría repetido un barrido ya agotado.

La conclusión sigue siendo la de R46: el árbol que se observa no se obtiene de
las combinaciones C++ legítimas ya medidas con este `emit_case_nodes`. Hace falta
evidencia nueva del fuente original o una palanca demostrada en el pase del
compilador antes de volver a tocarlo.

## Gates y restauración

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEagl4Anim
  -> 1 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> 3 de 318 distintas, 3.528 B
python scripts/fndiff.py Speed/Indep/SourceLists/zEagl4Anim \
  EvalSQT__Q29EAGL4Anim12FnStatelessQfPfPCQ29EAGL4Anim8BoneMask
  -> target=100.0 %, ours=100.0 %, 1480/1480 B
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim
  -> sin FALLA; EvalSQT: 37 ramas, 51 relocs, 12 literales
python scratchpad/codex_20260908_resume_anim/audit_final.py
  -> PASS en los tres gates
python scripts/lcfix.py --check
  -> todas las entradas @lc están al día
```

`before.json` y `after.json`, generados con
`function_reloc_diffs=none` y `ppc.calculatePoolRelocations=false`, son
idénticos y tienen SHA-256
`965B8A112AFAA9E5F6F00F47C1E9F29FE318E10A71641E357E8B6D6B74A8220F`.

La igualdad completa del ELF fuente con su copia previa es el gate fuerte para
secciones, instrucciones, datos, tablas, literales, relocs y addends:

- `zEagl4Anim.o`:
  `175023E866A12E9462A16E7F4D8F0004177576AE92ED2AF4735B082EFB05FB93`
- `FnRunBlender.cpp`:
  `CF06286D23FC4475398BC39FB3691CC75441AD77E89A505AE2ABBFA29BDF5788`
- `eagl4supportdlopen.cpp`:
  `87A6A07CF46C69423154EC6FF903A293187C48A48B7BE7CF2B01E8A970B1EE34`
- `RawStateChan.cpp` intacto:
  `DCFC2B8FA1D31C32F241FE9A591A8F5671A9083FBA35640CB8E078EA8984235A`

Artefactos reproducibles en `scratchpad/codex_20260908_resume_anim/`. No se
modificaron cabeceras, configuración, splits ni flags; no se ejecutó `ninja` y
no se hizo staging ni commit.
