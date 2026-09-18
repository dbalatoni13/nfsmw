# Veintiocho unidades del original se compilaron con un compilador que no tenemos

Verificado por mí, no inferido.

## ACTUALIZACION 2026-09-15: la teoria del compilador ausente esta REFUTADA

Sonda empirica (sonda_versiones_madidct.py / sonda_versiones_pathnode.py,
SHA-1 de .text y .data comparados):

- `madidct` baseline r68 compilado con ProDG **3.9.3 (v1.76), 3.8.1 (v1.54) y
  3.7 (v1.46): .text y .data BYTE-IDENTICOS** (mismo SHA-1 3df2d512784e).
  IdctRow 46,37%, IdctColumn 50,45%, idctcompute 100% en las tres.
- `pathnode` compilado con las tres versiones: **100,00% en las tres**
  (10.252 B). La sospecha sobre PATHI_nextnode ya estaba cerrada.

Conclusion: el codegen de SN para GC esta congelado en v1.46..v1.76 para este
codigo. Conseguir v1.72 (encajonado entre versiones identicas) o v1.83 (path
ya matchea al 100% con v1.76) **no cambiaria nada**. Las vedas restantes son
de forma de fuente, y la informacion para resolverlas esta en el DWARF-1 del
ELF (mapa de registros por local, tabla de lineas, arbol de inlines), no en
un compilador perdido. No perseguir mas esta linea.

## Lo que declara el volcado DWARF del original

```
288 unidades   SN BUILD v1.76
 15 unidades   SN BUILD v1.83     <- no lo tenemos
 13 unidades   SN BUILD v1.72     <- no lo tenemos
```

## Lo que tenemos

Preguntado a cada compilador del árbol (`echo "" | cc1.exe` imprime su `.ident`):

| directorio | versión |
|---|---|
| `ProDG/3.5b140` | SN BUILD v1.40 |
| `ProDG/3.7` | SN BUILD v1.46 |
| `ProDG/3.8.1` | SN BUILD v1.54 |
| `ProDG/3.9.3` | **SN BUILD v1.76** |
| `ProDG/3.5` | no responde |

**Ni v1.72 ni v1.83.**

## Qué unidades son

**`SN BUILD v1.83` — las 15 de `path/5.01.04/source/cmn/`:**
`pathcontrol`, `pathevent`, `pathinit`, `pathreal`, `pathreal6`, `pathserv`,
`pathsnd`, `pathtrack`, `pathvol`, `author`, `pathaction`, `pathbank`,
`pathdebug`, `pathnode`, `pathrand`.

**`SN BUILD v1.72` — las 13 de `egami/rcmp/dev/source/`:**
`av/cmn/avplayer`, `av/cmn/avsubtitle`, `av/cmn/audioplayer`,
`vd/gc/bigyuvswizzler`, `vd/gc/bigswizzler`, `decoder/cmn/rcmpbase`,
`decoder/cmn/rcmp_vp6_codec`, `decoder/cmn/rcmp_vp6_codec_chunk_types`,
`decoder/cmn/rcmp_mad_codec`, `decoder/cmn/rcmp_mad_codec_chunk_types`,
`decoder/cmn/maddec`, `decoder/cmn/maddeca`, `decoder/cmn/madidct`.

## Qué significa, y qué NO significa

**No significa que sean inalcanzables.** `path` está al 88 % con varias unidades
al 100 %, así que v1.83 y v1.76 generan el mismo código en la mayoría de los
casos. Lo que significa es que **cuando una de estas 28 se atasca en una o dos
instrucciones, la causa puede no estar en la fuente**, y conviene saberlo antes
de gastar una ronda entera barriendo formas.

Sospechosos concretos, por ser de esta lista y llevar rondas resistiéndose:

- **`madidct`** (1.148 B al 23,67 %) — v1.72. Siete rondas de permutador sin
  cerrar nada.
- **`pathnode`** (644 B) — v1.83. Su `PATHI_nextnode` va en dirección contraria
  a la del resto de `path`.
- **`avplayer`** — v1.72, con un pin de registro y `.rodata` de más.

## Lo que habría que hacer

Conseguir `ProDG` con `SN BUILD v1.72` y `v1.83`. No es trabajo de
decompilación: es de herramientas. Mientras tanto, **al repartir, marcar estas 28
como «la fuente puede no ser la causa»**.
