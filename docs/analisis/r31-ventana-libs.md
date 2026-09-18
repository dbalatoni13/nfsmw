# Ronda 31 / ventana — el frente de BIBLIOTECAS, censado y sin tocar

Trabajo de ventana hecho mientras corrían los cuatro agentes de la r31, sobre el
terreno que **ninguno de ellos toca**: las unidades que no son SourceLists.

## La foto completa, con `scripts/promodist.py`

    75 unidades sin promocionar, 3.450.276 B de codigo
       de esas, 38 con el .text YA PERFECTO (701.776 B): el trabajo es de DATOS

De esos 701.776 B, **657.400 son las 15 SourceLists** (encargo de `lk` y `und`) y
**44.376 B son 24 unidades de biblioteca** que nadie está mirando.

## Las 24 candidatas, con lo que las bloquea

`fns` = funciones pendientes; el delta es **nuestro objeto menos el extraído**.

| desbloquea | fns | B fns | delta de datos | unidad |
|---|---|---|---|---|
| 12.040 | 1 | 740 | `rodata−4` | `vp6/…/gc/criticalpath` |
| 10.872 | 1 | 252 | `rodata+616 sdata−8 sbss+4` | `realcore/…/file/cmn/filesys` |
| 8.760 | 10 | 4.080 | `data+40 bss+40 sdata2+40 sbss+20` | `LibSN/steering` |
| **7.836** | **2** | **936** | **ninguno** | **`spch/…/cmn/spchpick`** |
| 6.948 | **0** | 0 | `bss+128 sdata+8` | `libc/vfprintf` |
| 6.152 | **0** | 0 | `rodata+712` | `realmemcard/…/gc/gc_interface` |
| 5.988 | 3 | 804 | `data+25 sdata+4 sbss+40` | `OdemuExi2/…/DebuggerDriver` |
| 4.224 | 1 | 656 | `rodata+4` | `rcmp/…/av/cmn/avplayer` |
| 4.204 | 2 | 644 | `rodata+32` | `path/…/cmn/pathnode` |
| **3.180** | **1** | **380** | **ninguno** | **`LibSN/vmbase`** |
| 2.828 | **0** | 0 | `rodata+360` | `rcmp/…/rcmp_vp6_codec` |
| 2.704 | 1 | 440 | `bss−28` | `realcore/…/file/gc/dvd_device` |
| 2.592 | **0** | 0 | `rodata+288` | `rcmp/…/rcmp_mad_codec` |
| 2.472 | 1 | 424 | ninguno | `path/…/cmn/pathtrack` |
| 2.448 | 12 | 1.188 | ninguno | `ppc2D2` |
| 2.340 | **0** | 0 | `rodata+16 data−4 bss+4` | `snd/…/cmn/stagpat` |
| 2.324 | **0** | 0 | `rodata+224 sdata2+152` | `libc/math_support` |
| 2.304 | 1 | 168 | `data+40 sbss+4` | `LibSN/vm` |
| 2.212 | **0** | 0 | `rodata+80` | `rcmp/…/gc/bigyuvswizzler` |
| 2.184 | **0** | 0 | `sdata+64 sdata2+136` | `libc/k_rem_pio2` |
| 2.180 | **0** | 0 | `sdata+40 sdata2+84` | `libc/kf_rem_pio2` |
| 2.156 | 1 | 396 | `rodata+164` | `path/…/cmn/pathbank` |
| 2.120 | **0** | 0 | `rodata+4 bss−20` | `snd/…/gc/sdspmix` |
| 2.000 | **0** | 0 | `sdata+272 sdata2+48` | `libc/e_pow` |

## Lo que sale de clasificar los símbolos de datos uno a uno

**Aquí el problema es el CONTRARIO que en las SourceLists.** Allí faltan `lbl_`;
aquí **sobran símbolos con NOMBRE**, y casi nunca falta nada:

    spchpick, vmbase, ppc2D2 ......  FALTAN 0  /  SOBRAN 0    <- puro codegen
    e_pow ........................  FALTAN 0  /  SOBRAN 312 B en 36 NOMBRE
    math_support .................  FALTAN 0  /  SOBRAN 168 B en 20 NOMBRE
    k_rem_pio2 ...................  FALTAN 0  /  SOBRAN 136 B en  9 NOMBRE
    rcmp_vp6_codec ...............  FALTAN 0  /  SOBRAN 353 B (240 NOMBRE, 113 $LC)
    gc_interface .................  FALTAN 124 B / SOBRAN 823 B (804 NOMBRE)

Es decir: **emitimos tablas y estáticos con nombre que el objeto del original no
tiene en esa unidad**, y eso corre las direcciones. La cura no es escribir dato,
es lo contrario — o el dato está en otra unidad en el original, o sobra.

Y **`dvd_device` merece mirada aparte**: FALTAN 32.892 B y SOBRAN 32.864 B en
**un solo símbolo con nombre**. Un símbolo de 32 kB que existe en los dos lados y
no casa por nombre o por sitio: eso es una línea de `symbols.txt`, no trabajo.

## Las tres que valen la pena y son PURO CODEGEN

Sin ningún problema de datos — cerrar la función es cerrar la unidad:

| unidad | desbloquea | qué falta |
|---|---|---|
| `spchpick` | **7.836 B** | 2 funciones, 936 B |
| `vmbase` | **3.180 B** | 1 función, 380 B |
| `pathtrack` | **2.472 B** | 1 función, 424 B |

## Una trampa nueva de herramienta, ya anotada en `HERRAMIENTAS.md`

**`trypromo.py` lee el `build.ninja` VIVO, y los agentes lo regeneran cada vez
que prueban un `configure.py`.** Un barrido mío de 23 unidades contestó
**`NO esta en la lista de enlace` a las 23**, y las 23 estaban: media hora
después, las mismas dos unidades dieron `DOL ROTO` normalmente. Si un barrido
entero sale con el mismo mensaje raro, no son las unidades: es el fichero.

`promodist.py` se apoya en `complete_code` de `report.json` y en los tamaños de
sección de los dos objetos, así que **no depende de `build.ninja`** y es fiable
con agentes trabajando.

---

# Segunda pasada: QUÉ las bloquea, unidad por unidad

## El bloqueo sistemático: emiten datos en secciones que `splits.txt` no les declara

**17 de las 24** emiten `.rodata`/`.data`/`.sdata`/`.sdata2`/`.bss` en secciones
para las que su entrada de `splits.txt` **no tiene ningún rango**. Así no pueden
promocionar nunca: al enlazar meten bytes donde el original no les dio sitio.

Y **el tamaño de lo huérfano cuadra al byte con el delta del DOL**:
`gc_interface` emite 712 B de `.rodata` sin rango → su delta es `rodata+712`;
`math_support` 224+152 → `rodata+224 sdata2+152`; `e_pow` 272+48 → `sdata+272
sdata2+48`. No hay nada más.

**Es el problema al revés del paquete de la r30**: allí había que CEDER rango al
comodín; aquí hay que RECLAMARLO.

## Dónde están esos bytes en el DOL (buscados por contenido)

Los once localizados están **todos en territorio libre** (hoy los sirve un
comodín), así que reclamarlos no le quita nada a nadie.

**Siete EXACTOS, listos para reclamar:**

| unidad | sección | B | rango en el DOL |
|---|---|---|---|
| `steering` | `.data` | 40 | `0x80439F18..0x80439F40` |
| `DebuggerDriver` | `.data` | 25 | `0x8044F610..0x8044F629` |
| `k_rem_pio2` | `.sdata` | 64 | `0x804FF838..0x804FF878` |
| `kf_rem_pio2` | `.sdata` | 40 | `0x804FF388..0x804FF3B0` |
| `pathbank` | `.rodata` | 164 | `0x8041399C..0x80413A40` |
| `e_pow` | `.sdata2` | 48 | `0x80500340..0x80500370` |
| `math_support` | `.rodata` | 224 | `0x8040FD30..0x8040FE10` |

**`k_rem_pio2` es además una simple frontera**: ya tiene `.sdata
start:0x804FF878`, y el rango que le falta es el inmediatamente anterior — o sea
`start:0x804FF878 -> start:0x804FF838`.

**Cuatro con el anclaje bueno pero contenido que aún no casa:**

- `filesys .rodata` @`0x80413A7C`: **46 de las 49 palabras distintas son
  punteros** (los parchea el enlazador), así que el anclaje es correcto; las
  otras 3 son un desplazamiento de 4 B en el bloque de cadenas (`"null:"` nos
  sale una palabra antes).
- `e_pow .sdata` (27 palabras), `k_rem_pio2 .sdata2` (13),
  `kf_rem_pio2 .sdata2` (6): **son las mismas constantes en OTRO ORDEN**. Ahí el
  trabajo es de orden de declaración en la fuente, no de `splits.txt`.

El resto (`gc_interface`, `rcmp_vp6_codec`, `rcmp_mad_codec`, `bigyuvswizzler`,
`pathnode`, `stagpat`, `sdspmix`, `vfprintf`, `vm`, `steering .sdata2`) no da
anclaje único: o su dato no está en el DOL, o es una cadena que aparece muchas
veces.

## Y un bug real que salió de mirar los bytes

`libc/math_support.c:298` tenía **la constante mal tecleada**, con el hex bueno
en su propio comentario:

    log10_2lo = 3.69423907715893278825e-13;   /* 0x3D59FEF3, 0x11F12B36 */
                                                            salia ...2B3A

Caso de libro del «100 % que miente»: `objdiff` compara `lfd f1, lbl@l(r3)` y no
mira el VALOR del literal. Corregido, el `.rodata` de `math_support` es **byte a
byte** el del DOL.

**Lo importante para el método: `audit.py` SÍ lo detecta** —`FALLA: literal
distinto (8 B)`— **pero nunca se había pasado por las bibliotecas.** Barridas
ahora las 41 unidades no-SourceList con fuente: **0 FALLA**, así que era el
único. Y de los 27 literales del árbol que llevan su hex de referencia en el
comentario, éste era el único que no casaba.

---

# Tercera pasada (r32): los rangos, localizados POR NOMBRE

Buscar por contenido fallaba en cuanto había una reubicación. Buscar por
**nombre** en `config/GOWE69/symbols.txt` da la dirección exacta y no falla:
`scripts/claimrange.py`, alimentado por `promodist.py --libs --listas`.

Y sale que **casi todo el dato huérfano son vtables**.

## Rangos LIBRES, listos para reclamar

| unidad | sección | rango | B |
|---|---|---|---|
| `filesys` | `.rodata` | `0x80413AE0..0x80413CE8` | 520 |
| `gc_interface` | `.rodata` | `0x80414530..0x80414790` | 608 |
| `gc_interface` | `.rodata` | `0x804149E8..0x80414A18` | 48 |
| `rcmp_vp6_codec` | `.rodata` | `0x80410230..0x804102E8` | 184 |
| `rcmp_mad_codec` | `.rodata` | `0x80410270..0x804102B0` | 64 |
| `rcmp_mad_codec` | `.rodata` | `0x80410400..0x80410440` | 64 |
| `DebuggerDriver` | `.sbss` | `0x804FFC48..0x804FFC6C` | 36 |
| `DebuggerDriver` | `.sdata` | `0x804FF588..0x804FF594` | 12 |
| `steering` | `.sbss` | `0x804FF8C4..0x804FF8D4` | 16 |
| `k_rem_pio2` | `.sdata2` | `0x80500BD0..0x80500C20` | 80 |
| `math_support` | `.sdata2` | `0x80500388..0x80500390` | 8 |
| `e_pow` | `.sdata2` | `0x80500390..0x805003A8` | 24 |

Más los siete que ya salieron por contenido en la segunda pasada (`steering
.data`, `DebuggerDriver .data`, `k_rem_pio2 .sdata`, `kf_rem_pio2 .sdata`,
`pathbank .rodata`, `e_pow .sdata2`, `math_support .rodata`).

## Y lo que NO es un rango que reclamar: la VTABLE EMITIDA DOS VECES

Cuatro filas salen `CHOCA`, y ninguna es un problema de `splits.txt`: **el
símbolo lo emitimos en dos unidades y el original una sola vez.**

```
rcmp_vp6_codec  _vt.Q32EA9Allocator10IAllocator  0x803D18F0  <- tambien en zBWare
rcmp_mad_codec  _vt.Q24RCMP5CODEC                0x80410270  <- tambien en rcmp_vp6_codec
gc_interface    _vt.Q26Realmc9GCMessage          0x804143E0  <- tambien en memcard_interface_impl
steering        @457 / @458                      .data/.sdata en el original, .sdata2 en el nuestro
```

Eso explica de una vez el patrón que la segunda pasada dejó sin nombre —
«**SOBRAN símbolos con NOMBRE**»—: son vtables duplicadas entre unidades
hermanas. **La cura es que sólo una de las dos la emita** (en GCC 2.9 la vtable
se emite en el TU que define su *key method*), no tocar `splits.txt`.

Y las dos últimas filas de `steering` dicen otra cosa útil: sus `@457`/`@458`
están en `.data`/`.sdata` en el original y nosotros los mandamos a `.sdata2`.
Eso es una bandera `-G`/`const`, no un rango.

## Lo que sigue sin localizar

Los estáticos de fdlibm (`zero`, `one`, `two24`, `eighth`…) **repiten nombre
entre ficheros**, así que `symbols.txt` no los desambigua: `k_rem_pio2` y
`kf_rem_pio2` proponen el **mismo** rango. Ésos hay que localizarlos por
contenido, y ahí `e_pow .sdata` ya se sabe que tiene **las mismas constantes en
otro orden** (27 palabras), que es trabajo de orden de declaración.
