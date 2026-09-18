# Por qué `linked` está en el 7,92 % — diagnóstico de las 143 unidades

Medido en la ronda 21, sobre `report.json` recién generado y con
`build_direct.py --all` pasado antes.

## El dato que nadie había mirado

**143 unidades tienen el código al 100 % y NO están marcadas `Matching`.** Entre
ellas **once SourceLists enteras**: `zAnim`, `zAttribSys`, `zDebug`,
`zFoundation`, `zGameModes`, `zLua`, `zMisc`, `zMiscSmall`, `zMission`,
`zRender`, `zSim`.

`matched_code` no las cuenta como pendientes porque su código casa. `linked`
—que es lo que la gente mira— sólo cuenta las unidades que el enlazador toma de
**nuestro** objeto, y ésas son las `Matching`. De ahí el 7,92 %.

## Por qué falla cada una (`promote.py` sobre las 137 diagnosticadas)

| casos | causa |
|---|---|
| **84** | **secciones que emitimos de más** |
| 34 | exportamos símbolos de más |
| 28 | símbolos en otro sitio de su sección (repatchan a quien los referencia) |
| 26 | `.text` de distinto tamaño |
| 22 | no definimos un símbolo que el extraído exporta |
| 19 | reubicaciones a símbolo con sufijo de dirección (racimo) |
| **11** | **LIMPIA** |
| 5 | VETADA: rompe el enlace |

## La causa dominante no es nuestro código: es `splits.txt`

De los 84 «secciones que emitimos de más», el caso testigo es `slib`:

```
NUESTRO     .text 360 B   .rodata 32 B
OBJETIVO    .text 360 B   (ninguna sección de datos)
```

Nuestra `.rodata` es el **pool de constantes del compilador**
(`4330000080000000 388206103c010204 42fe000000000000 3f00000000000000`). Esos 32
bytes **existen en el DOL**, en `0x80412DF8` — pero el split de `slib.c` declara
**sólo `.text`**:

```
Speed/Indep/Libs/snd/9/source/library/cmn/slib.c:
	.text       start:0x8036D3D4 end:0x8036D53C
```

Así que el pool cae en un `auto_*` sin dueño. Al promocionar, nuestro objeto
**añade** esos 32 B encima de la copia que ya está, y el DOL se desplaza.

Es exactamente el mismo bloqueo que el de `DebuggerDriver` (`splits.txt:911`
corta su `.data` justo antes de `0x8044F610` y la cadena `"Can't select EXI2
port!\n"` cae en `auto_06`).

**Conclusión: la mayor parte de `linked` no está bloqueada por la
decompilación, sino por cómo `splits.txt` atribuye los datos.** Arreglarlo es
editar `splits.txt`, que está vedado sin permiso explícito.

## Lo que SÍ se puede promocionar hoy — verificado contra el DOL

Diez unidades, **juntas, con el DOL byte a byte idéntico**:

```
libc/fseek
libc/memset
libc/strstr
libc/wcstring
egami/rcmp/dev/source/decoder/cmn/maddec
Packages/vp6/1.0.6/source/decode/cmn/borders
Packages/vp6/1.0.6/source/decode/cmn/decodemv
Packages/vp6/1.0.6/source/decode/cmn/FrameIni
Speed/Indep/Libs/snd/9/source/library/mix/sfsplit
Speed/Indep/Libs/path/5.01.04/source/cmn/pathserv
```

## Y un punto ciego de `promote.py`, medido

`Speed/Indep/Libs/realcore/6.24.00/source/system/gc/timerthread` sale **LIMPIA**
y **rompe el DOL** (`9384074e9578`), sola y en grupo. Es la undécima «limpia» y
la única que falla.

**`promote.py` compara secciones y símbolos; `trypromo.py` compara el DOL, que es
el juez.** Nunca promociones por el veredicto de `promote.py`.
