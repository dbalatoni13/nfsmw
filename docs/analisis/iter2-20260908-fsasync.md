# Iter2 FSasync: `CompletePCreadAsync` (2026-09-08)

## Resultado en sombra

`CompletePCreadAsync` queda cerrada en la variante `c4`:

| estado | tamaño | objdiff | funciones exactas de la unidad |
|---|---:|---:|---:|
| fuente a la entrada de esta tanda | 396/388 B | 93,701035 % | 12/13 |
| `c4` | **388/388 B** | **100 %** | **13/13** |

El cierre se probó primero exclusivamente en
`scratchpad/codex_20260908_iter2_fsasync` y, después de una revisión
independiente de los dos auditores, se integró sólo en `src/LibSN/FSasync.c`.

## Causa

El objetivo materializa la parte alta de la dirección estable
`g_nBlockCnt_804B4F48` exactamente dos veces:

1. antes del predecremento de la primera región, conservada en r30 durante el
   bucle interior;
2. inmediatamente después de cada `ReadSyncNext` de la segunda región, otra
   vez en r30, sin izarla al precabezal del bucle exterior.

La fuente actual produce cuatro `lis`: uno para cada predecremento y otro para
cada bucle. Un puntero C completo no es equivalente porque introduce
`addi @l` y accesos `0(base)`, ausentes del ELF.

La variante cerrada da nombre real a la mitad alta:

```c
#define FS_BLOCK_HIGH(base) \
    __asm__ volatile("lis %0,g_nBlockCnt_804B4F48@ha" : "=r"(base))
#define FS_BLOCK_LOAD(value, base) \
    __asm__ volatile("lwz %0,g_nBlockCnt_804B4F48@l(%1)" \
                     : "=r"(value) : "r"(base) : "memory")
#define FS_BLOCK_STORE(value, base) \
    __asm__ volatile("stw %0,g_nBlockCnt_804B4F48@l(%1)" \
                     : : "r"(value), "r"(base) : "memory")
```

Cada región declara un `block_high` real en r30. La salida del `lis` lo
inicializa antes de todo acceso. El valor aritmético se carga en una local
normal y permanece en r9; una local distinta `condition` en r0 recibe cada
recarga posterior al store antes de compararse. No existe lectura indefinida,
cantidad fantasma ni cuerpo implementado en ensamblador. `lis`, `lwz` y `stw`
no modifican CR, por lo que no corresponde un clobber `cc`; los accesos sí
llevan el clobber `memory` necesario para impedir que el compilador reordene
otras lecturas o escrituras alrededor del acceso volatile real.

El nombre del ensamblador no es un ordinal de pool: es el símbolo global
estable que ya declara la TU. El objeto objetivo y `c4` contienen exactamente
**2 `R_PPC_ADDR16_HA` y 12 `R_PPC_ADDR16_LO`** hacia
`g_nBlockCnt_804B4F48` dentro de la función.

## Ensayos acotados

Se hicieron tres variantes antes de reevaluar, según el límite del encargo:

| variante | modelado de memoria | resultado | explicación |
|---|---|---:|---|
| `c1` | operando `m` | 404 B / 93,051544 % | reload crea dos bases adicionales por región |
| `c2` | operando genérico `X` sobre la misma lvalue | 404 B / 93,051544 % | retiene la misma legalización de dirección |
| `c3` | acceso real con clobber `memory` | **388 B / 98,453606 %** | forma/CFG exactos; sólo quedan dos rangos de registro |

La reevaluación de `c3` muestra 97 instrucciones en ambos lados. Las únicas
diferencias causales son:

- la mitad alta sale en r31 y desplaza la base del buffer a r30, frente a
  r30/r31 en el objetivo;
- las dos recargas de condición de cada región se fusionan con el temporal r9,
  mientras el objetivo les reserva r0.

Con autorización posterior se probó solamente la combinación demostrada:
`block_high asm("r30")` y la local real separada
`condition asm("r0")`. Esa es `c4`, y cierra; no hubo barrido de registros.
`c3` no se propone ni se retiene por su tamaño superficial.

## Auditoría

El gate sombra `audit.py` probó antes de integrar:

- inventario idéntico de 13 funciones;
- sólo cambia la métrica de `CompletePCreadAsync`;
- todas las funciones pasan a 100 %;
- `.rodata`, `.data`, `.bss`, `.sdata`, `.sbss`, constructores/destructores y
  sus reubicaciones permanecen byte a byte como en el objeto fuente anterior;
- que los hashes de `FSasync.c`, ambos objetos de producción, `configure.py` y
  `splits.txt` no cambiaron durante la prueba sombra.

El auditor común contra el ELF original da:

```text
CompletePCreadAsync  ok  388 B, 10 ramas, 58 relocs, 0 literales
```

También audita las otras doce funciones, todas `ok`; por ejemplo se preservan
`DoFSReadHeader` 140 B, `EXI2TCHandler` 316 B y `SNInitEXI2TCHandler` 112 B.

La compilación real posterior usa el target exacto `FSasync`. El nuevo
`audit_production.py` conserva sin sobrescribir el manifiesto y las copias
anteriores a la edición. Confirma que los cambios de fuente se limitan a la
veda/cuerpo de `CompletePCreadAsync`, que el fichero continúa en CRLF puro, que
las otras doce funciones no cambian su métrica, tamaño o reubicaciones
normalizadas, y que datos/reubicaciones de datos son iguales.

Reproducción de producción:

```text
python scripts/build_direct.py FSasync
python scratchpad/codex_20260908_iter2_fsasync/audit_production.py
python scripts/audit.py FSasync
python scripts/fncmp.py FSasync
python scripts/lcfix.py --check
```

Resultado: `PRODUCTION_AUDIT_PASS`, trece funciones `ok`, cero funciones con
código distinto y LC limpio. No se cambió configuración ni splits y no se
promovió la unidad: el orden enlazado de tres definiciones se evalúa en una
prueba separada de coordinación. El censo y SHA completos quedan igualmente a
cargo de la coordinación para no solapar builds globales.

Actualizacion de coordinacion: el reorden de las dos definiciones FSACK ya esta
integrado y auditado por separado. El gate historico de esta carpeta corresponde
al estado inmediatamente anterior al reorden. Para el estado vigente usar
`scratchpad/codex_20260908_iter2_fs_link/audit_final.py`; ver el informe
`iter2-20260908-fs-link.md`. El censo final confirma +388 B matched, cero
regresiones, y el DOL conserva el SHA original. La promocion Matching sigue
pendiente de autorizacion.
