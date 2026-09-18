# DebuggerDriver: ownership ELF y prueba de enlace en sombra (2026-09-08)

## Alcance y estado real

Trabajo exclusivamente de lectura y sombra sobre `HEAD 8853122e`. No se ha
modificado `configure.py`, ningún fichero bajo `config/GOWE69`, ninguna fuente
de producción ni ningún objeto de `build/GOWE69/{obj,src}`.

El estado actual **no** es `Matching`: `configure.py` selecciona
`GC/1.2.5` para el único objeto monolítico `DebuggerDriver.c`, pero el objeto
sigue declarado `NonMatching`. La cifra 22/31 procede de una consulta directa
cruda y aislada del monolito; no es la medición oficial normalizada. El reporte
estable con los `symbol_mapping` vigentes, `functionRelocDiffs=none` y la
configuración de pools registra **28/31 funciones, 5572/5988 B**. Sus únicos
cuerpos pendientes son `DBRead` (140 B), `DBQueryData` (156 B) y `DBInitComm`
(120 B). Los demás aparentes pendientes de la consulta cruda no se deben sumar.
La discrepancia estructural no es solamente de compilador: el ELF original
demuestra que el monolito mezcla tres unidades de traducción.

## Prueba de procedencia y ownership

La tabla de símbolos de `orig/GOWE69/NFSMWRELEASE.ELF` contiene estos marcadores
`STT_FILE` consecutivos:

| índice ELF | fichero original | símbolos locales inmediatamente asociados |
|---:|---|---|
| 3257 | `EXIBios.c` | incluye `Ecb` 0x804BBD68, 192 B, índice 3260 |
| 3350 | `DebuggerDriver.c` | estado DB, `SendCount`, siete helpers DB |
| 3365 | `AmcExi2Comm.c` | `ucEXI2InputPending`, `pucEXI2InputPending`, `fExi2Selected`, `TRK_Callback`, `EXI2_CallBack`, literal de 25 B |
| 3372 | `AmcExi.c` | `Ecb` 0x804BC0C8, 24 B, `exi`, `AmcDebugIntHandler` |

Por tanto, los dos símbolos `Ecb` locales tienen dueños distintos e
inequívocos:

- `Ecb` 0x804BBD68, 0xC0/192 B pertenece por completo a `EXIBios.c`. El split
  actual ya le asigna `.bss 0x804BBD68..0x804BBE28` y el objeto está
  `Matching`. **No se toca ni se comparte con OdemuExi2.**
- `Ecb` 0x804BC0C8, 0x18/24 B pertenece a `AmcExi.c`. Es el `EXIControl`
  inicializado por `AmcEXIInit`; `exi` apunta a él y las funciones Amc lo usan
  a través de ese puntero. Debe definirse `static EXIControl Ecb;` en esa TU.
  Globalizarlo o declararlo `extern` contradice tanto el binding `LOCAL` como
  el marcador `STT_FILE` original.

## Corte exacto propuesto

El corte de dos mitades de r46 debe refinarse a estas tres TUs:

| TU | compilador | `.text` | estado propio |
|---|---|---|---|
| `DebuggerDriver.c` | `GC/1.2.5n` | 0x80345B50..0x803465D0 (0xA80, 14 funcs) | `.sdata` 0x804FF588..0x804FF590; `.sbss` 0x804FFC48..0x804FFC60 |
| `AmcExi2Comm.c` | `GC/1.2.5` | 0x803465D0..0x80346C74 (0x6A4, 8 funcs) | `.data` 0x8044F610..0x8044F629; `.sdata` 0x804FF590..0x804FF598; `.sbss` 0x804FFC60..0x804FFC6C |
| `AmcExi.c` | `GC/1.2.5` | 0x80346C74..0x803472B4 (0x640, 9 funcs) | `.bss` 0x804BC0C8..0x804BC0E0; `.sdata` 0x804FF598..0x804FF5A0 |

La distribución enlazada queda así:

- `SendCount` = `0x80` en 0x804FF588; el objeto fuente aporta 1 B y el enlace
  conserva los 7 B de alineación antes de la siguiente TU.
- `pucEXI2InputPending` en 0x804FF590 apunta al byte local
  `ucEXI2InputPending` de 0x804FFC60; quedan 4 B de alineación hasta la TU Amc.
- `exi` en 0x804FF598 apunta al `Ecb` local de 24 B en 0x804BC0C8; quedan 4 B
  de alineación hasta el siguiente propietario de `.sdata` en 0x804FF5A0.
- El primer `.sbss` contiene `MTRCallback`, `DBGCallback`, `SendMailData`,
  `RecvDataLeng`, `pEXIInputFlag` y `EXIInputFlag` (0x15 B efectivos más 3 B
  de alineación). El segundo empieza exactamente en 0x804FFC60 con el byte
  `ucEXI2InputPending`, seguido por `fExi2Selected` y `TRK_Callback`.

Esos huecos no son datos inventados: aparecen porque cada objeto tiene
alineación de sección 8. Los tamaños efectivos de fuente son, respectivamente,
`.sdata` 1/4/4 B, `.sbss` 0x15/0x0C B y `.bss` 0x18 B; el enlazador produce los
rangos acolchados exactos de arriba.

## Prueba aislada

Los tres fragmentos se generaron sólo en
`scratchpad/codex_20260908_resume_debugger` y se compilaron con los compiladores
indicados. Frente a los tres objetos objetivo extraídos con el split propuesto:

- `DebuggerDriver.c`: 0/14 funciones con código distinto.
- `AmcExi2Comm.c`: 0/8 funciones con código distinto. `fncmp2` lista en
  `EXI2_Poll` y `EXI2_ReadN` únicamente dos nombres de relocación del mismo
  literal local; el enlace resuelve ambos a 0x8044F610.
- `AmcExi.c`: 0/9 funciones con código distinto.

La prueba `shadow_link.py` hizo una extracción de control y otra con los tres
splits copiados. Para el ensayo sustituyó sólo el objeto monolítico y eliminó
los dos objetos automáticos que quedan absorbidos legítimamente:

- `auto_07_804BC0C8_bss.o`
- `auto_08_804FF594_sdata.o`

Resultados con 616 objetos enlazados en ambos casos:

| enlace | SHA-1 DOL | veredicto |
|---|---|---|
| control | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` | original |
| tres TUs fuente | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` | original |

El DOL de ensayo también es idéntico byte a byte a
`orig/GOWE69/sys/main.dol`. El único aviso de enlace es el duplicado histórico
`PPCMtdec`, presente igualmente en el control. El gate final:

```text
python scratchpad/codex_20260908_resume_debugger/audit.py
AUDIT_PASS
Functions: DebuggerDriver:14/0, AmcExi2Comm:8/0, AmcExi:9/0 (31/31 code-exact)
DOL SHA-1: 9619ba57c9919f95f7f2ac951a2166a3517f91e3 ORIGINAL
Production protected paths unchanged
```

Los comandos exactos de extracción y enlace están en
`scratchpad/codex_20260908_resume_debugger/proof/*.log`; las listas completas
de objetos están en los `.rsp` del mismo directorio y `result.json` conserva
los hashes SHA-256 de todas las rutas de producción protegidas.

## Plan de integración acotado (no aplicado)

1. Separar físicamente la reconstrucción actual en los tres nombres que ya
   constan en el ELF: `DebuggerDriver.c`, `AmcExi2Comm.c` y `AmcExi.c`.
2. Mantener `DebuggerDriver.c` en `GC/1.2.5n`; configurar las otras dos TUs en
   `GC/1.2.5`. Esto requiere dos entradas de librería/compilador, no un flag
   global compartido.
3. Aplicar exclusivamente los rangos del cuadro anterior. No cambiar símbolos
   globales ni el `Ecb` de `EXIBios.c`.
4. Marcar las tres TUs como enlazadas sólo después de repetir `audit.py`, el
   enlace completo y el SHA original en producción.

La evidencia de enlace demuestra que este plan elimina la barrera `Ecb` sin
falsear bindings ni trasladar estado entre propietarios.

Con la medición oficial anterior, la promoción acotada prevé **+416 B matched**
y **5988 B linked** para la unidad. Esa previsión no atribuye como ganancia los
falsos pendientes de la consulta cruda 22/31.
