# R41 - `LibSN/steering`

Fecha: 2026-09-08. Territorio exclusivo: `src/LibSN/steering.c`.

## Resultado

Las dos hipótesis preparadas al cerrar r40 fueron negativas. No se retiene
ningún cambio nuevo de fuente. El objeto final vuelve exactamente al estado
r40: 28/36 funciones exactas, ocho funciones / 2972 B pendientes.

Los snapshots JSON completos `before.json`, `after.json` y el `after.json` de
r40 son idénticos byte por byte, con SHA-256:

```text
f88fb3d9fe2273a03875f3a9b8b729742abc5e2fd69afac6bfdde6d63c5d31ee
```

Por tanto se preservan sin cambio los cierres de
`VDevice_GetFreeEffect` (116 B) y `Effect_Update` (992 B), además de los otros
26 símbolos exactos y todas las secciones de datos de la unidad.

## `CookValues`, 260 B

Baseline: 95,76923 %, 260 B, trece instrucciones distintas.

El target conserva el byte crudo de `raw->steering` y su promoción firmada en
pseudos distintos (`lbz r5` seguido de `extsb r6,r5`). La fuente actual funde
ambos papeles. Se probaron sólo formas dirigidas a expresar esas dos
representaciones, sin tocar el macro `AUTOCAL` compartido:

| Variante | Resultado | Diagnóstico |
|---|---:|---|
| `cook_bits_signed` | 95,00000 %, 260 B | `u8` crudo + `int` firmado; MWCC vuelve a usar el promovido en los stores y deja 18 diferencias |
| `cook_field_dual_use` | 95,76923 %, 264 B | usos directos repetidos del campo; añade una recarga por posible alias y 4 B |
| `cook_qi_promoted` | 94,46154 %, 260 B | local `s8` con promociones implícitas; conserva un QI real, pero cambia el reparto y deja 20 diferencias |
| `cook_typed_volatile` | 93,07692 %, 264 B | el mismo QI con carga localizada `volatile`; añade un `mr`, altera scheduling y no reproduce r5/r6 |

La hipótesis sí confirma el origen de `extsb` frente a `mr`, pero no proporciona
el reparto completo del original. Queda vedado repetir la separación simple
byte/promoción, tanto ordinaria como mediante una carga `volatile`. Todas las
variantes se retiraron.

## `VDevice_DownloadEffect`, 188 B

Baseline: 99,3617 %, 188 B, seis diferencias. Todas siguen siendo el mismo
intercambio limpio entre `ret` (r31 target / r30 fuente) e `idx` (r30 target /
r31 fuente).

Se materializó una local `effectId` inmediatamente después de
`VDevice_GetFreeEffect`, usando ese valor único para la comparación y las dos
derivaciones:

- lectura ordinaria: objeto idéntico al baseline;
- lectura `volatile` localizada de `*pid`: objeto idéntico al baseline.

MWCC elimina el pseudo adicional o le da exactamente el mismo color; no cambia
ninguna de las 47 instrucciones. Queda vedada esta familia de lectura única,
incluida su variante `volatile`. No se probó `e = &vd->pool[idx]` porque el
target calcula primero el desplazamiento de `e` desde el valor completo en r4
y sólo después materializa `idx`; esa forma altera la dependencia que ya es
correcta y además está cubierta por los ensayos históricos de expresión del
índice.

## Auditoría y artefactos

Artefactos de `scratchpad/codex_r41_steering/`:

- `before.json`, `after.json`;
- `cook_bits_signed.json`, `cook_field_dual_use.json`,
  `cook_qi_promoted.json` (`cook_typed_value.json` conserva la misma forma) y
  `cook_typed_volatile.json`;
- `download_effectid.json`, `download_effectid_volatile.json`;
- `audit.py`.

Comandos finales:

```text
python scripts/build_direct.py LibSN/steering
objdiff-cli-windows-x86_64.exe diff -1 build/GOWE69/obj/LibSN/steering.o -2 build/GOWE69/src/LibSN/steering.o -c function_reloc_diffs=none -c ppc.calculatePoolRelocations=false -o scratchpad/codex_r41_steering/after.json --format json
python scripts/fncmp.py LibSN/steering
python scripts/fndiff.py LibSN/steering VDevice_GetFreeEffect
python scripts/fndiff.py LibSN/steering Effect_Update
python scratchpad/codex_r41_steering/audit.py
python scripts/lcfix.py --check
git diff --check -- src/LibSN/steering.c
```

El auditor r41 exige igualdad binaria de los tres snapshots completos y vuelve
a ejecutar los gates estrictos heredados:

- r39: 116/116 B, cinco relocaciones SDA con addends/destinos/secciones ELF y
  bytes resueltos de `VDevice_GetFreeEffect`;
- r40: 992/992 B, 55 ramas, 27 relocaciones con addends/destinos, cero
  literales y bytes ELF de `Effect_Update`;
- los 36 símbolos fuente y todas las secciones, datos y campos de relocación
  permanecen idénticos entre el inicio y el final de r41.

Salida final:

```text
PASS steering r41 no-regression audit
  before == after == r40 snapshot
  all 36 symbols and every code/data/relocation field unchanged
  inherited GetFreeEffect and Effect_Update ELF gates pass
```

`lcfix.py --check` y `git diff --check` pasan. `steering.c` conserva sus 1443
terminadores CRLF, sin LF desnudos. SHA-1 final de la fuente:
`812ED175269E4A688DBB3BA3A8E3D395507BD36E`, idéntico a r40.

No se tocaron cabeceras, flags, configuración, splits ni otros territorios.
