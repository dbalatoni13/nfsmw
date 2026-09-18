# R39 - `LibSN/steering`

Fecha: 2026-09-07. Fuente exclusiva: `src/LibSN/steering.c`.

## Resultado

Se cerró `VDevice_GetFreeEffect`: **116/116 B, 100 %, 29/29 instrucciones**.
La unidad pasó de 10 a 9 funciones con código distinto y de 4080 a 3964 B
pendientes. El inventario final de objdiff tiene 27 funciones al 100 %; no se
regresó ninguna de las 26 que ya eran exactas.

El cambio C es local y semánticamente conservador. La lectura de `vd->dev` se
materializa después de las dos escrituras a `jumbleeffectid`, aplicando
`volatile` sólo a esa lectura:

```c
jumbleeffectid = idx + 1;
jumbleeffectid = jumbleeffectid & 0x3F;
dev = *(LGDEVICE * volatile *)&vd->dev;
```

Esto expresa la dependencia observable que MWCC necesitaba para programar
`lwz r3, 0xf0(r3)` en la posición original. No añade instrucciones ASM, valores
indefinidos, símbolos fantasma ni cambios de configuración. El acceso conserva
el tipo efectivo del campo y sólo añade la calificación `volatile` a una carga.

## Diagnóstico y ensayos acotados

El diff inicial confirmó los diez pendientes documentados. Se ensayaron y se
restauraron por no cerrar:

- `HandleTriggers`: separar los temporales izquierdo/derecho en scopes léxicos;
  objeto idéntico al baseline.
- `SimThread_Step`: introducir un `tickDelta` separado; MWCC lo coalesció. La
  variante firmada produjo el `mr` buscado y tamaño 924 B, pero empeoró el mapa
  de registros (79 filas), por lo que también se restauró.
- Reordenar la copia respecto a los ceros y reutilizar la cadena de cero;
  ninguna mejora adicional.

Los snapshots de esos negativos están en
`scratchpad/codex_r39_steering/handle_split_scopes.json` y los cuatro
`step_split_*.json`. No se repitieron los barridos de flags, pines o formas C
vedados por las rondas r18-r23/r36.

## Auditoría final

Artefactos reproducibles:

- baseline: `scratchpad/codex_r39_steering/before.json`
- resultado fresco tras la integración central:
  `scratchpad/codex_r39_steering/after.json`
- auditor estricto: `scratchpad/codex_r39_steering/audit.py`

Comandos finales:

```text
python scripts/build_direct.py LibSN/steering
objdiff-cli-windows-x86_64.exe diff -1 build/GOWE69/obj/LibSN/steering.o -2 build/GOWE69/src/LibSN/steering.o -c function_reloc_diffs=none -c ppc.calculatePoolRelocations=false -o scratchpad/codex_r39_steering/after.json --format json
python scripts/fncmp.py LibSN/steering
python scripts/fndiff.py LibSN/steering VDevice_GetFreeEffect 100
python scratchpad/codex_r39_steering/audit.py
python scripts/audit.py LibSN/steering
python scripts/lcfix.py --check
git diff --check
```

El auditor estricto comprueba:

- 116/116 B y 29 instrucciones idénticas;
- todos los destinos relativos de rama;
- las cinco reubicaciones `R_PPC_EMB_SDA21` (cuatro a
  `jumbleeffectid`, una a `createcount`), incluido addend cero, tamaño y
  sección `.sbss` del símbolo;
- el enlace explícito de objdiff desde cada símbolo fuente al símbolo objetivo
  concreto y su dirección en `symbols.txt`: `jumbleeffectid$426` ->
  `jumbleeffectid$1040` (`0x804FF8C8`) y `createcount$427` ->
  `createcount$1041` (`0x804FF8CC`);
- los 116 bytes después de aplicar únicamente esas cinco reubicaciones con
  `_SDA_BASE_=0x80506DC0`, cotejados contra `NFSMWRELEASE.ELF`;
- ausencia de llamadas y literales en la función;
- target extraído intacto;
- todas las secciones/datos y los restantes símbolos fuente idénticos al
  baseline.

`scripts/audit.py` da OK en las otras 26 funciones exactas y sólo informa una
falsa diferencia nominal para ésta: los estáticos locales son
`jumbleeffectid$1040`/`createcount$1041` en el objeto extraído y
`jumbleeffectid$426`/`createcount$427` en el recompilado. El auditor r39
normaliza exclusivamente ese contador local, pero no acepta el nombre por sí
solo: exige el mapeo individual al símbolo objetivo, addend, sección, dirección
original y bytes finales. Las direcciones originales son distintas por cuatro
bytes, de modo que intercambiar ambos destinos hace fallar el gate.

Limitación explícita: el layout del objeto fuente completo todavía no coincide
con el extraído. `jumbleeffectid` vive en `.sbss+4` en ambos, pero
`createcount` está en `.sbss+0` en el fuente y `.sbss+8` en el extraído, debido
al orden de otros estáticos de esta TU aún no matching. Por eso este resultado
certifica la función y su resolución hacia los símbolos originales; **no**
declara la TU completa promocionable ni confunde el offset local del objeto
parcial con la dirección final original.

Salida final del auditor propio:

```text
PASS steering r39 strict audit
  VDevice_GetFreeEffect: 116/116 bytes, 29/29 instructions
  branches: identical relative destinations
  relocations: 5/5 R_PPC_EMB_SDA21, addends 0/0, .sbss targets resolved
  resolved originals: jumbleeffectid=0x804FF8C8, createcount=0x804FF8CC
  relocated bytes: 116/116 equal to NFSMWRELEASE.ELF
  literals/calls: none
  all other source symbols and every section/data diff unchanged
  functions at objdiff 100%: 27
```

`lcfix.py --check` y `git diff --check` pasan. `steering.c` conserva CRLF puro
(1443 terminadores, cero LF desnudos) y su diff final es 1 línea añadida / 1
eliminada. SHA-1 final de la fuente:
`DBA1D5B08AB1769717A0B99C59DB63F3A16D664A`.

## Pendientes conservados

Quedan nueve funciones / 3964 B con código distinto: `Effect_Update`,
`SimThread_Step`, `HandleTriggers`, `SimThread_Init`, `Effect_Init`,
`CookValues`, `VDevice_RecalcGammaTable`, `Effect_PerformEnvelope` y
`VDevice_DownloadEffect`. `LGInit` y `VDevice_GetFreeEffect` aparecen en
`fncmp` bajo la lista separada de alias locales, no como diferencias de código.
