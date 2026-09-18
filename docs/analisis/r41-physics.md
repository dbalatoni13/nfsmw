# Ronda 41 - zPhysicsBehaviors

## Alcance y baseline

Unidad exclusiva: `Speed/Indep/SourceLists/zPhysicsBehaviors`. Se recompilo el
objeto real antes de modificar y se guardaron las 1.120 funciones en
`scratchpad/codex_r41_physics/before_pct.json`.

Quedan exactamente dos cuerpos distintos:

| Funcion | Tamano target/fuente | Estado |
|---|---:|---:|
| `RBGrid::Add(unsigned int, RigidBody &, UMath::Vector3 const &, float)` | 1488/1488 B | 99,301080 %, 8 instrucciones |
| `SuspensionTraffic::Tire::UpdateLoaded(float,float,float,float)` | 856/860 B | 97,210280 % |

No se alteraron cabeceras, flags, configuracion ni splits.

## `SuspensionTraffic::Tire::UpdateLoaded`

### Causa vigente

Las 21 filas visibles siguen siendo una sola causa. El target mantiene el
`high(<literal 1.0f>)` en r30 a traves de `VU0_Atan2` y `VU0_sqrt`, y lo usa en
dos `lfs`. Nuestro GCC vuelve a materializar dos `lis r9`. Eso explica tambien
el segundo GPR preservado, `stmw/lmw r30` y el marco target de 0x30 frente a
0x28. Las otras apariciones de 1.0f se rematerializan en ambos objetos.

La evidencia previa de `.gcse/.cse2/.lreg` sigue consistente: PRE crea el
pseudo compartido, cse2 deshace las copias, y `update_equiv_regs` hunde cada
pseudo de un solo uso. El DWARF original no contiene una local `one`; sus tres
locales y el arbol de inlines ya casan con la fuente.

### Hipotesis nueva: lectura volatile localizada

El cierre r40 de steering justificaba probar una lectura volatile real en vez
del `+f` ya vedado. Se nombro temporalmente un objeto constante 1.0f y se
cambiaron **solo** las dos comparaciones dominadas por el bloque de Atan2/Sqrt.

| Ensayo | Resultado | Evidencia |
|---|---:|---|
| `static const float one`, dos usos ordinarios | 97,210280 %, 860 B | objeto de la funcion identico; GCC vuelve a plegarlo en `$LC917` |
| mismo objeto, dos lecturas por lvalue `volatile const float` | 97,140190 %, 860 B | conserva dos `lis r9`, cambia `fcmpu/cror` y agrega 8 B de rodata |
| `static const volatile float one` + input-only `m(one)` antes de Atan2 | 96,182240 %, 868 B | agrega `addi ... one@l`, no comparte los dos `high`, y agrega 4 B de data |

La variante localizada alcanza exactamente las dos cargas relevantes, pero no
crea el pseudo `high(symbol)` comun: volatile impide el plegado de las cargas,
no la reconstruccion independiente de sus direcciones. La dependencia `m`
obliga ademas a formar una direccion completa, ausente en el target.

Por tanto no queda un valor de fuente legitimo al que aplicar la palanca: un
float nombrable va a un FPR, un objeto volatile introduce dato/relocacion, y el
valor que debe vivir es solo la mitad alta generada por GCC. Todos los ensayos
fueron retirados.

## `RBGrid::Add`

No se abrio un nuevo barrido porque la veda relevante sigue demostrada a nivel
de RTL. Las ocho diferencias son cuatro copias del mismo par inline de `SAP.h`:

```
target: lwz r9,4(r9)   ... fcmpu ... b -> cror compartido
fuente: lwz r9,4(prev) ... fcmpu ... blt -> cuerpo
```

La barrera conocida sobre `head` arregla solo los cuatro `lwz`, pero aporta cero
bytes exactos y no se retiene. Las otras cuatro filas nacen de
`expand_end_loop` y de la inversion de `jump.c` antes de que el cross-jump pueda
fundir las colas. Las formas que bloquean esa transformacion pierden la copia
del test y bajan a 1440--1456 B, mientras el target exige 1488 B. Los ensayos
previos cubren 24 formas en unidad real, unas 45 en el minimo y 34 flags.

Una dependencia de lifetime o argumentos puede cambiar el registro del `lwz`,
pero no puede convertir ese `blt` en el latch incondicional requerido. Sin una
causa nueva anterior a `jump.c`, repetir permutaciones contradice la evidencia.

## Restauracion y auditoria final

Se retiraron todas las pruebas y se recompilo de forma directa:

```
python scripts/build_direct.py zPhysicsBehaviors
1 ok, 0 fallidas
```

- `before_pct.json` y `after_pct.json`: iguales para las 1.120 funciones.
- `fncmp.py`: siguen siendo los unicos dos cuerpos distintos; 2.344 B
  pendientes, ninguna regresion.
- `audit.py`: 1.118 funciones exactas, 0 fallos de bytes, ramas, relocs o
  literales.
- Datos en el baseline restaurado: `.bss` 28256/25820, `.data` 1412/1808 con
  386 bytes iguales, `.rodata` 31064/30936 con 22489 bytes iguales.
- `lcfix.py --check`: limpio. `git diff --check`: limpio.
- `RigidBody.cpp` y `SuspensionTraffic.cpp`: sin diff; este ultimo conserva su
  formato LF original (754/754 finales LF, cero CRLF).

Resultado: **0 bytes y 0 funciones nuevas**, sin deuda parcial ni regresiones.
La unidad y el objeto quedan liberados.
