# Iter4 2026-09-08 — revisión de promoción `DebuggerDriver`

## Veredicto sobre el estado actual

La promoción en tres TUs sigue siendo válida después de integrar `FSasync` y
`vmbase`. No se modificó ningún fichero de producción durante esta revisión.

El control y el ensayo se extrajeron y enlazaron de nuevo con la lista viva de
**616 objetos**. Esa lista contiene expresamente
`build/GOWE69/src/FSasync.o` y `build/GOWE69/src/LibSN/vmbase.o`, no sus objetos
extraídos. Ambos enlaces producen byte a byte el DOL original:

| enlace actual | objetos | SHA-1 |
|---|---:|---|
| control monolítico | 616 | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` |
| tres TUs fuente | 616 | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` |

Los fragmentos se regeneraron desde la fuente de producción actual, cuyo
SHA-256 sigue siendo `9fa0b8978e5e8847a7d5bfe7dbae610ae95fc374dbf9e4fde27ccfa56c0c3a45`,
y se recompilaron de cero. La auditoría compara **31/31 funciones exactas**:
14 de `DebuggerDriver`, 8 de `AmcExi2Comm` y 9 de `AmcExi`.

La situación de producción todavía es la monolítica: `configure.py` mantiene
un único objeto `NonMatching` bajo `GC/1.2.5` y `splits.txt` conserva el bloque
0x80345B50..0x803472B4. Por tanto esta prueba prepara una integración; no la
da por aplicada.

## Mapa de ownership que debe integrarse

Los marcadores `STT_FILE` y símbolos `LOCAL` del ELF fijan tres TUs y dos
`Ecb` distintos. El de 192 B en 0x804BBD68 ya pertenece a `EXIBios.c` y no se
toca. Sólo el `Ecb` de 24 B en 0x804BC0C8 se define `static` en `AmcExi.c`.

| TU nueva | compilador | `.text` | secciones de datos reclamadas |
|---|---|---|---|
| `OdemuExi2/src/DebuggerDriver.c` | `GC/1.2.5n` | 0x80345B50..0x803465D0 | `.sdata` 0x804FF588..0x804FF590; `.sbss` 0x804FFC48..0x804FFC60 |
| `OdemuExi2/src/AmcExi2Comm.c` | `GC/1.2.5` | 0x803465D0..0x80346C74 | `.data` 0x8044F610..0x8044F629; `.sdata` 0x804FF590..0x804FF598; `.sbss` 0x804FFC60..0x804FFC6C |
| `OdemuExi2/src/AmcExi.c` | `GC/1.2.5` | 0x80346C74..0x803472B4 | `.bss` 0x804BC0C8..0x804BC0E0; `.sdata` 0x804FF598..0x804FF5A0 |

Las secciones fuente recién compiladas confirman los tamaños efectivos y la
alineación 8 que produce los huecos enlazados:

- `DebuggerDriver`: `.text` 0xA80, `.sdata` 1, `.sbss` 0x15;
- `AmcExi2Comm`: `.text` 0x6A4, `.data` 0x19, `.sdata` 4, `.sbss` 0x0C;
- `AmcExi`: `.text` 0x640, `.bss` 0x18, `.sdata` 4.

El nuevo reparto absorbe legítimamente los actuales automáticos
`auto_07_804BC0C8_bss.o` y `auto_08_804FF594_sdata.o`. No absorbe ni modifica
el rango de 192 B de `EXIBios.c`. El número de objetos enlazados permanece en
616: salen el monolito y esos dos automáticos; entran tres TUs fuente.

## Cambios de producción exactos propuestos

1. Sustituir el contenido monolítico actual por el fragmento probado
   `scratchpad/codex_20260908_iter4_debugger_review/DebuggerDriver.c` y añadir,
   en la misma carpeta de `OdemuExi2/src`, los fragmentos probados
   `AmcExi2Comm.c` y `AmcExi.c`. Los tres deben conservar CRLF.
2. Reemplazar el bloque único de `splits.txt` por exactamente los tres rangos
   de la tabla anterior. No tocar `symbols.txt`, `keep.lst` ni el bloque de
   `EXIBios.c`.
3. Partir la entrada de `configure.py` en dos entradas contiguas que conserven
   `cflags_dolphin` y `progress_category = "sdk"`:
   - `DebuggerDriver.c`, `Matching`, `toolchain_version = "GC/1.2.5n"`;
   - `AmcExi2Comm.c` y `AmcExi.c`, en ese orden, ambos `Matching` y
     `toolchain_version = "GC/1.2.5"`.
4. No cambiar las dos entradas homónimas de `amcstubs`: tienen rutas y rangos
   distintos. No introducir `extern Ecb`, aliases ni símbolos globales nuevos.

El orden de las tres entradas es contractual: reproduce el acolchado de
`.sdata`/`.sbss` y la posición de `.data`/`.bss` probados por el enlace.

## Protocolo de integración acotado, sin Ninja global

Con todos los escritores pausados:

1. Guardar hashes de `configure.py`, `splits.txt`, `symbols.txt`, `keep.lst`,
   `build.ninja`, el monolito fuente/objetivo y los objetos fuente de
   `FSasync`/`vmbase`; comprobar primero que `build/GOWE69/main.dol` conserva
   el SHA original.
2. Aplicar únicamente los tres cambios de la sección anterior y ejecutar
   `python scripts/checksplits.py`.
3. Reextraer explícitamente con
   `build/tools/dtk.exe dol split config/GOWE69/config.yml build/GOWE69`.
   Verificar en `config.json` —no por la mera existencia de ficheros obsoletos
   en disco— que aparecen los tres objetivos nuevos y que ya no se enlazan los
   dos automáticos absorbidos.
4. Ejecutar `python configure.py -v GOWE69` y comprobar el diff generado de
   `objdiff.json`/`build.ninja`: sólo cambia esta unidad en tres TUs; `FSasync`
   y `vmbase` continúan enlazando desde `build/GOWE69/src`.
5. Compilar sólo estos targets:

   ```text
   python -m ninja build/GOWE69/src/Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.o build/GOWE69/src/Speed/GameCube/bWare/GameCube/OdemuExi2/src/AmcExi2Comm.o build/GOWE69/src/Speed/GameCube/bWare/GameCube/OdemuExi2/src/AmcExi.o
   ```

6. Auditar los tres pares target/fuente: 14/14, 8/8 y 9/9 funciones, tamaños de
   sección, datos, bindings locales, relocaciones y destinos resueltos. En
   `AmcExi2Comm`, dos nombres locales de literal pueden diferir; la dirección
   resuelta debe seguir siendo 0x8044F610 y el DOL decide.
7. Construir sólo el gate enlazado, no el target global por defecto:

   ```text
   python -m ninja build/GOWE69/ok
   ```

   Exigir SHA-1 `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 616 objetos y ausencia de
   errores `L0039`. El aviso histórico de `PPCMtdec` puede seguir presente, pero
   no se debe silenciar ni confundir con un PASS.
8. Ejecutar `audit.py`, `checksplits.py`, `lcfix.py --check`, medición oficial y
   comparación before/after. El delta esperado es **+416 B matched** y
   **+5988 B linked**, sin alterar el denominador ni volver a contar las 28
   funciones ya exactas del monolito.

## Riesgos y gates obligatorios

- **Baseline vieja:** los hashes de configuración del proof anterior ya no son
  válidos por las promociones de `FSasync`/`vmbase`. Sólo vale el proof actual
  de este informe.
- **Binding de `Ecb`:** ambos `Ecb` son `LOCAL`. Globalizar o compartir el de
  Amc con `EXIBios` invalida la procedencia aunque enlazase.
- **Orden/alineación:** cambiar el orden de los objetos o agruparlos de nuevo
  en un monolito puede mover los huecos de 7/4 B y romper el DOL.
- **Automáticos obsoletos:** la extracción debe demostrar que los dos objetos
  auto dejan de formar parte de la lista; borrarlos manualmente no es prueba.
- **Homónimos de `amcstubs`:** no deben editarse ni promocionarse como parte de
  este cambio.
- **Medición:** la consulta cruda 22/31 no es la cifra oficial. El estado
  normalizado previo es 28/31, 5572/5988 B; sólo 416 B son ganancia matched.

## Artefactos reproducibles

- `scratchpad/codex_20260908_iter4_debugger_review/review_current.py` regenera,
  compila, extrae y enlaza la prueba contra el grafo vivo.
- `scratchpad/codex_20260908_iter4_debugger_review/audit_current.py` verifica
  ownership ELF, 31/31 funciones, DOL, lista de 616 objetos, sustituciones y
  presencia de los objetos fuente actuales de `FSasync`/`vmbase`.
- `proof_root_recheck/result.json`, los dos `.rsp`, ELF/DOL y logs contienen los
  comandos y hashes de esta repetición.

Resultado del gate: `CURRENT_GRAPH_AUDIT_PASS`.

## Gate posterior a la integración

Tras la integración coordinada se ejecutó
`scratchpad/codex_20260908_iter4_debugger_review/audit_post_integration.py`.
Resultado: **`POST_INTEGRATION_AUDIT_PASS`**.

El gate posterior añade a la prueba previa:

- fuentes reales byte a byte iguales a los tres fragmentos CRLF revisados;
- configuración `Matching`, versiones `GC/1.2.5n` / `GC/1.2.5` y splits
  exactos, manteniendo intactos los homónimos de `amcstubs`;
- grafo vivo de 616 objetos con los tres objetos fuente nuevos, sin los dos
  automáticos absorbidos y con `FSasync`/`vmbase` todavía enlazados;
- los otros **613 objetos** conservan orden, pertenencia y SHA-256 frente al
  manifiesto coordinado anterior;
- auditoría original-ELF de las 31 funciones, datos y relocaciones;
- `Ecb` sigue siendo `LOCAL` y distinto en `EXIBios` (192 B) y `AmcExi`
  (24 B). La única diferencia de nombre local se verificó de forma acotada:
  `exi` en el objetivo y `exi_804FF598` en fuente son el mismo objeto de 4 B a
  offset 0 de `.sdata`, con las mismas 12 referencias SDA21 resueltas;
- reporte oficial comparado por `(unidad, símbolo)`, no por posición: las 31
  quedan exactas y todas las funciones vecinas globales permanecen iguales;
- deltas exactos: **+416 B matched, +3 funciones exactas, +5988 B linked,
  +84 B de datos matched, +109 B de datos linked y +3 unidades completas**;
- DOL real y ambos DOL del proof conservan SHA-1 original.

Dependencias del gate: el `report_before.json` y `manifest_before.json` de
`scratchpad/codex_20260908_iter4_debugger_link`, el reporte vivo regenerado,
los tres pares target/source y el proof inmutable de esta revisión. El script
no extrae, no compila y no enlaza; sólo genera tres JSON de comparación local
con `objdiff` y lee los artefactos de producción.
