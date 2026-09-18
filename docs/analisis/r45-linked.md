# r45 - auditoría sombra de promoción a linked

## Resultado

**`stagpat` queda demostrada como promoción válida en sombra: 2.340 B de código
linked potenciales.** Se reclamó únicamente su `.rodata` verificada en una
copia de `splits.txt`, se extrajeron los 618 objetos a `scratchpad` y se
ejecutaron dos enlaces completos aislados:

| enlace | sustitución | SHA-1 del DOL |
|---|---|---|
| control | ninguna; reparto vigente íntegramente reextraído en sombra | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` |
| ensayo | rango `0x804129E0..0x804129F0` + objeto fuente de `stagpat` | `9619ba57c9919f95f7f2ac951a2166a3517f91e3` |

Ambos DOL miden 4.541.888 B y son byte por byte el original. La comprobación de
`promote.py` redirigida al árbol extraído del ensayo responde **`LIMPIA: se
puede marcar Matching`**, sin `undef` con sufijo de dirección. `audit.py` pasa
las tres funciones.

Nada se aplicó al árbol real: `stagpat` sigue `NonMatching` y `splits.txt` sigue
sin su rango. La promoción necesita autorización antes de tocar configuración.

El censo de partida sigue siendo relevante: `build/GOWE69/report.json` enumera
**16** unidades `NonMatching` que parecen completas en código y datos,
**31.808 B** en total, pero `scripts/promote.py` clasificaba **0** como limpias
con el reparto vigente. Se excluyó `spchpick` porque su promoción ya está
demostrada y pendiente de autorización separada. Las otras catorce entradas,
fuera de esas dos, conservan diferencias reales o fallos documentados; no se
repitieron enlaces sin una causa modificada.

Esto vuelve a demostrar que `matched_code == total_code` y
`matched_data == total_data` no implican que el objeto sea enlazable: las
secciones no atribuidas al objetivo no entran en el denominador de datos.

## Obstrucción mínima accionable: `stagpat`

Se eligió la unidad
`Speed/Indep/Libs/snd/9/source/library/cmn/stagpat` como diagnóstico de datos,
no como promoción ya conseguida.

- `fncmp`: **0/3 cuerpos de código distintos**, 2.340 B. La función de 1.600 B
  sólo conserva diez relocaciones con nombres de literal distintos.
- `audit.py`: las tres funciones pasan; `SNDBANKI_findfreekey` 132 B,
  `SNDBANKI_playpatch` 608 B y `SNDBANKI_playtimbre` 1.600 B, con todas sus
  ramas, relocaciones y diez literales auditados.
- `promote.py`: una sola pega actual, «secciones que emitimos de más:
  `.rodata(16B)`».
- El objeto extraído tiene `.text` 2.340 B y `.data` 4 B. El nuestro tiene los
  mismos tamaños más una `.rodata` de 16 B. `freekey` ya está correctamente en
  los 4 B de `.data`; no es el bloqueo vigente.

Los 16 B no son datos inventados por la fuente. Son exactamente los bytes del
DOL original en `0x804129E0..0x804129F0`:

| símbolo objetivo | tipo/tamaño | bytes |
|---|---:|---|
| `lbl_804129E0` | `double`, 8 B | `4330000080000000` |
| `lbl_804129E8` | `float`, 4 B | `3C010204` |
| `lbl_804129EC` | `float`, 4 B | `00000000` |

El objeto fuente emite la concatenación idéntica como `$LC11`, `$LC12` y
`$LC13`; el objeto objetivo referencia los tres `lbl_` desde las mismas cargas.
`symbols.txt` confirma sus direcciones, tamaños, tipos y alineaciones. El split
de `stagpat.c` no reclama `.rodata`, mientras el siguiente rango ya atribuido,
el de `svol.c`, empieza exactamente en `0x804129F0`. Por tanto el rango propuesto
tiene el mismo tamaño que nuestra sección y **cero bytes de hueco detrás**, lo
que satisface las dos reglas de `ventana-pendiente.md`.

## Cambio demostrado que requiere autorización

Añadir provisionalmente a `stagpat.c`:

```text
.rodata     start:0x804129E0 end:0x804129F0
```

La prueba sombra confirma que, junto a esa línea, se puede cambiar solamente la
entrada de `stagpat.c` de `NonMatching` a `Matching`. En una ventana central
autorizada aún deben repetirse `checksplits`, extracción, enlace principal y el
SHA del DOL real. La ganancia no se cuenta hasta aplicar y medir esa ventana.

No se modificó `splits.txt`, `configure.py`, fuentes, objetos ni el DOL real. El
`DOL ROTO` de r34 describía correctamente el reparto anterior; el cambio causal
nuevo es atribuir los 16 B a su dueña antes de sustituir el objeto.

## Reproducción exacta de la sombra

El arnés usado es:

```text
python scratchpad/codex_r45_link_shadow_stagpat.py --out scratchpad/codex_r45_link_shadow_stagpat_repro
```

El directorio de salida debe no existir y ser hijo directo de `scratchpad`.
Dentro de esa raíz, los comandos efectivos son:

```text
dtk.exe dol split --no-update -j 4 config/GOWE69/config_control.yml obj_control
ngcld.exe -strip-unused-data -keep <repo>/config/GOWE69/keep.lst -T <repo>/config/GOWE69/ldscript.ld -o control.elf @control.rsp
dtk.exe elf2dol control.elf control.dol

dtk.exe dol split --no-update -j 4 config/GOWE69/config_trial.yml obj_trial
ngcld.exe -strip-unused-data -keep <repo>/config/GOWE69/keep.lst -T <repo>/config/GOWE69/ldscript.ld -o trial_stagpat.elf @trial_stagpat.rsp
dtk.exe elf2dol trial_stagpat.elf trial_stagpat.dol
```

El arnés usa `cwd` en su raíz espejo porque esta versión de `dtk` mutila las
rutas absolutas `C:/...` dentro del YAML. Los `.rsp` contienen exactamente los
618 objetos del enlace vivo; los objetos extraídos apuntan a la copia sombra y
sólo `stagpat.o` se sustituye por `build/GOWE69/src/.../stagpat.o` en el ensayo.
Los hashes SHA-256 de `configure.py`, config, splits, symbols, keep, ambos objetos
de `stagpat` y el DOL original se toman antes y se exigen idénticos después.

## Artefactos y gate

- `scratchpad/codex_r45_link_inventory.json`: censo, descarte y rango propuesto.
- `scratchpad/codex_r45_link_stagpat.json`: diff completo del objeto.
- `scratchpad/codex_r45_link_stagpat_audit.txt`: auditoría de las tres funciones.
- `scratchpad/codex_r45_link_stagpat_target_relocs.txt`: usos de los tres
  símbolos objetivo.
- `scratchpad/codex_r45_link_shadow_stagpat.py`: extracción, control y ensayo
  íntegramente en sombra.
- `scratchpad/codex_r45_link_shadow_stagpat_v4/result.json`: resultado y hashes
  de protección; `control.dol` y `trial_stagpat.dol` conservan el SHA original.
- `python scratchpad/codex_r45_link_audit.py`: verifica secciones, bytes del
  objeto y del DOL, símbolos, split actual, rango siguiente, ambos enlaces y
  los hashes de protección sin modificar producción.
