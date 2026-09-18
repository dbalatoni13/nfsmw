# Ronda 33 — bibliotecas: reclamar rango de `splits.txt`

Base de la ronda verificada al empezar y al acabar: **`main.dol` OK con los 604
objetos del enlace**, `keepchk` **21 rancias** (no 19: las 21 ya estaban antes de
tocar nada, son de `zEAXSound2`/`sndfxbus`/`asd2`/`zDynamics`/`GXTexture`).
`config/` y `configure.py` quedan **exactamente como me los encontré**
(`git status` limpio, `build.ninja` idéntico byte a byte al de partida).

---

## LO PRIMERO: TRES UNIDADES DAN `DOL OK` — 4.820 B

**Dos de ellas no necesitan ni un rango: ya promocionan hoy, tal cual está el
árbol.** Salieron de pasar `trypromo.py` a las **43** unidades de biblioteca sin
promocionar antes de tocar nada — el barrido que el encargo no pedía y que valió
la ronda.

| unidad | qué hace falta | `trypromo` |
|---|---|---|
| `libc/math_support` (2.324 B) | **nada** | **DOL OK** |
| `Packages/realmemcard/…/gc/gc_blockcalculator` (312 B) | **nada** | **DOL OK** |
| `libc/k_rem_pio2` (2.184 B) | **un rango** (`.sdata2 0x80500BD0..0x80500C20`) | **DOL OK** |

Las tres **juntas** también: `DOL OK`. Y con el paquete completo aplicado
(17 rangos + los tres `Matching` en `configure.py`) el **enlace base de los 605
objetos sale `DOL OK`** y `keepchk` sigue en 21. Verificado cuatro veces.

### El paquete exacto, ya medido

**`configure.py`** — tres `NonMatching` → `Matching`:

```
libc/math_support.c                                                (linea 2052)
libc/k_rem_pio2.c    (conserva extra_cflags=["-msafe-sda","-mstrict-align"])
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_blockcalculator.cpp  (1316)
```

**`config/GOWE69/splits.txt`** — 17 líneas nuevas. Las tres primeras marcadas
**[P]** son las que hacen falta para la promoción; el resto son rangos correctos
y **medidos como inocuos** (enlace base `DOL OK`, `keepchk` sin mover) que dejan
listo el terreno de las rondas siguientes:

```
Speed/Indep/Libs/path/5.01.04/source/cmn/pathbank.cpp:
	.rodata     start:0x8041399C end:0x80413A48
Speed/Indep/Libs/realcore/6.24.00/source/file/cmn/filesys.cpp:
	.rodata     start:0x80413AE0 end:0x80413CE8
Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_interface.cpp:
	.rodata     start:0x80414530 end:0x80414790
	.rodata     start:0x804149E8 end:0x80414A18
egami/rcmp/dev/source/decoder/cmn/rcmp_vp6_codec.cpp:
	.rodata     start:0x80410230 end:0x804102E8
egami/rcmp/dev/source/decoder/cmn/rcmp_mad_codec.cpp:
	.rodata     start:0x80410400 end:0x80410440
libc/math_support.c:
	.rodata     start:0x8040FD30 end:0x8040FE10
libc/e_pow.c:
	.sdata      start:0x804FEEF0 end:0x804FF000
	.sdata2     start:0x80500340 end:0x80500370
libc/k_rem_pio2.c:
	.sdata      start:0x804FF838 end:0x804FF878          [P, opcional: ver abajo]
	.sdata2     start:0x80500BD0 end:0x80500C20          [P]
libc/kf_rem_pio2.c:
	.sdata      start:0x804FF388 end:0x804FF3B0
Speed/GameCube/bWare/GameCube/OdemuExi2/src/DebuggerDriver.c:
	.data       start:0x8044F610 end:0x8044F629
	.sdata      start:0x804FF588 end:0x804FF594
	.sbss       start:0x804FFC48 end:0x804FFC6C
LibSN/steering.c:
	.data       start:0x80439F18 end:0x80439F40
	.sbss       start:0x804FF8C4 end:0x804FF8D4
```

`keep.lst`: **sin cambios**. Ninguno de los 17 rangos mueve una frontera que
renombre un relleno — `keepchk` se quedó en 21 rancias en **cada uno** de los 18
ensayos.

**Aviso honrado sobre `promote.py`**: ninguna de las tres sale limpia
(`math_support` emite `.rodata`+`.sdata2` de más, `k_rem_pio2` `.sdata`+`.sdata2`,
y `gc_blockcalculator` **52 B de `.text` de más** y dos símbolos desplazados).
El DOL sale idéntico igual porque el enlazador tira lo que no está vivo. El DOL
es el juez, pero `gc_blockcalculator` conviene mirarlo dos veces al promocionar.

---

## Método: 18 ensayos, uno por rango

Ciclo por ensayo (unos 35 s): restaurar `splits.txt` desde copia → añadir **un**
rango → `dtk dol split` → `configure.py` → `keepchk` → **enlace base** →
`trypromo`. Herramientas propias en el scratchpad (`c33lib_claim.py`,
`c33lib_base.py`, `c33lib_chk.py`, `c33lib_cmp.py`).

**El enlace base es el filtro que faltaba.** Un rango puede trocear sin queja y
romper el DOL de los 605 objetos: pasó dos veces (ver vedas). Comprobarlo cuesta
2,6 s y separa «rango correcto» de «rango expresable».

| # | unidad | sección | rango | split | keepchk | enlace base | `trypromo` |
|---|---|---|---|---|---|---|---|
| 1 | `pathbank` | `.rodata` | `0x8041399C..0x80413A48` | ok | 21 | **DOL OK** | DOL ROTO |
| 2 | `filesys` | `.rodata` | `0x80413A7C..0x80413CE8` | ok | 21 | **DOL ROTO** | DOL ROTO |
| 2b | `filesys` | `.rodata` | `0x80413AE0..0x80413CE8` | ok | 21 | **DOL OK** | DOL ROTO |
| 2c | `filesys` | `.rodata` | `0x80413A7C..0x80413CE8 align:4` | ok | 21 | **DOL ROTO** (mismo hash que 2) | — |
| 3 | `gc_interface` | `.rodata` | `0x80414530..0x80414790` | ok | 21 | **DOL OK** | (blockcalc sigue OK) |
| 4 | `gc_interface` | `.rodata` | `0x804149E8..0x80414A18` | ok | 21 | **DOL OK** | (blockcalc sigue OK) |
| 5 | `rcmp_vp6_codec` | `.rodata` | `0x80410230..0x804102E8` | ok | 21 | **DOL OK** | ENLACE FALLA |
| 6 | `rcmp_mad_codec` | `.rodata` | `0x80410400..0x80410440` | ok | 21 | **DOL OK** | ENLACE FALLA |
| 7 | `math_support` | `.rodata` | `0x8040FD30..0x8040FE10` | ok | 21 | **DOL OK** | **DOL OK** |
| 8 | `e_pow` | `.sdata2` | `0x80500340..0x80500370` | ok | 21 | **DOL OK** | DOL ROTO |
| 9 | `k_rem_pio2` | `.sdata2` | `0x80500BD0..0x80500C20` | ok | 21 | **DOL OK** | **DOL OK** |
| 10 | `kf_rem_pio2` | `.sdata` | `0x804FF388..0x804FF3B0` | ok | 21 | **DOL OK** | DOL ROTO |
| 11 | `DebuggerDriver` | `.sbss` | `0x804FFC48..0x804FFC6C` | ok | 21 | **DOL OK** | ENLACE FALLA |
| 12 | `DebuggerDriver` | `.sdata` | `0x804FF588..0x804FF594` | ok | 21 | **DOL OK** | ENLACE FALLA |
| 13 | `DebuggerDriver` | `.data` | `0x8044F610..0x8044F629` | ok | 21 | **DOL OK** | ENLACE FALLA |
| 14 | `steering` | `.data` | `0x80439F18..0x80439F40` | ok | 21 | **DOL OK** | DOL ROTO |
| 15 | `steering` | `.sbss` | `0x804FF8C4..0x804FF8D4` | ok | 21 | **DOL OK** | DOL ROTO |
| 16 | `e_pow` | `.sdata` | `0x804FEEF0..0x804FF000` (**localizado aquí**) | ok | 21 | **DOL OK** | DOL ROTO |
| 17 | `kf_rem_pio2` | `.sdata2` | `0x805007C4..0x80500818` (**localizado aquí**) | ok | 21 | **DOL ROTO** | ENLACE FALLA |
| 18 | `k_rem_pio2` | `.sdata`+`.sdata2` | `0x804FF838..0x804FF878` + `0x80500BD0..0x80500C20` | ok | 21 | **DOL OK** | **DOL OK** |

**Ningún ciclo de dependencia y ningún «ends within symbol»** en toda la tanda:
las correcciones del encargo (`pathbank` a `0x80413A48`) y la comprobación previa
de cada extremo contra `config/GOWE69/symbols.txt` bastaron.

---

## Veda nueva y medida: el rango que empieza en 4 mod 8 no se puede expresar

Dos unidades tienen su `.rodata`/`.sdata2` **en una dirección que no es múltiplo
de 8** en el DOL, y ahí **no hay rango que reclamar**:

| unidad | sección | dirección real | qué pasa |
|---|---|---|---|
| `filesys` | `.rodata` | `0x80413A7C` | enlace base **DOL ROTO** (`f7de435c3e84`) |
| `kf_rem_pio2` | `.sdata2` | `0x805007C4` | enlace base **DOL ROTO** (`326e25a8b7b7`) |

La cabecera `Sections:` de `splits.txt` declara `.rodata align:8` y `.sdata2
align:8`, así que el objeto que emite el troceador se alinea a 8 y el enlazador
lo empuja al siguiente múltiplo. **Y el `align:4` por rango — que existe, lo usa
la línea 151 para un `.bss common` — NO sirve aquí**: el ensayo 2c da
**exactamente el mismo hash roto** que sin él. Comprobado además que **nuestros
propios objetos traen `.rodata align=8`**, o sea que el problema no es sólo del
troceador.

Del de `kf_rem_pio2` hay además una segunda razón: el rango se lleva
`@961_80500810`, y `steering.c` lo referencia (`L0039`).

## Los cinco bloqueos reales, con su mensaje

Cuando `trypromo` no dice `DOL ROTO` sino `ENLACE FALLA`, el rango no es el
problema. Los cuatro que quedan:

1. **`gc_interface`**: `asd2.o` referencia
   `__static_initialization_and_destruction_0_803906C0` y nuestro objeto emite el
   nombre pelado `__static_initialization_and_destruction_0`. Es exactamente el
   par que `configure.py` ya mapea **para objdiff** (`OBJDIFF_SYMBOL_MAPPINGS`),
   pero el enlazador no lee ese mapeo. Emite además los dos cuerpos inline de
   `gc_blockcalculator.h` (`_._Q26Realmc18BlockCalculatorImp` y
   `GetResult__…`), que salen como `multiply defined`.
2. **`DebuggerDriver`**: `Reference to undefined symbol Ecb`.
3. **`rcmp_vp6_codec` / `rcmp_mad_codec`**: **RESUELTO** (ver abajo).

## `rcmp_vp6_codec` y `rcmp_mad_codec`: dos correcciones de fuente aplicadas

Las dejo aplicadas en `src/` porque son correcciones demostrables, no apuestas:
**`libdiff.py` sigue dando 100,00 % de código en las dos** (2.828 y 2.592 B) y
`audit.py` no da un solo `FALLA`.

1. **El `extern "C" { … }` de las líneas 26-32 era el `ENLACE FALLA`.** Envolvía
   `REAL_abortmessage`, `MAD_initdecode` y `MAD_decodemacroblock`, y el original
   los tiene **manglados a C++** en `symbols.txt`
   (`REAL_abortmessage__FPCce`, `MAD_initdecode__FPCUsii`,
   `MAD_decodemacroblock__FPCUcN20PUcN23i`). Quitando el `extern "C"` el enlace
   pasa de `L0039` a enlazar.
2. **Tres mensajes de aborto estaban truncados.** El DOL dice:
   - `0x804101C8` (103 B) `VP6_CODEC_INTERNAL::GetFrameFromList() out of frames did you call CODEC::ReleaseFrame( FRAME *Frame )\n`
   - `0x80410338` (97 B) `MAD_CODEC_INTERNAL::CreateIorP() out of frames did you call CODEC::ReleaseFrame( FRAME *Frame )\n`
   - `0x8041039C` (94 B) `MAD_CODEC_INTERNAL::CreateB() out of frames did you call CODEC::ReleaseFrame( FRAME *Frame )\n`

   y nosotros teníamos `… out of frames did not release` (69/63/60 B). Corregido,
   **el bloque entero de `.rodata` casa byte a byte con el DOL salvo 11 bytes**
   (ver el punto siguiente).

### Lo que aún les falta a los dos códecs (localizado al byte)

Con el rango completo —no el de las vtables solas— la comparación byte a byte
contra el DOL deja **11 bytes** de diferencia en cada uno, y son los mismos:

- **`"RCMP::CHUNK\0"` (12 B) NO EXISTE EN NINGÚN SITIO DEL DOL.** Lo emiten tres
  TUs nuestras (`rcmpbase.cpp:68`, `rcmp_mad_codec.cpp:194`,
  `rcmp_vp6_codec.cpp:228`), siempre desde el `static void *operator new` de
  `RCMP::CHUNK` definido **dentro de la clase** en el propio `.cpp`. En vp6 y mad
  es **dato muerto**: `$LC0` no tiene **ni una** reubicación en `.text`. GCC 2.9
  lo emite igual porque la constante nace al parsear.
- **Un literal de 4 B de más** entre la cadena de 19 B y el doble de 8 B: el DOL
  tiene `lbl_804101A4`(19) → `lbl_804101B8`(4) → `lbl_804101C0`(8), y nosotros
  `$LC1`(19) → `$LC2`(1) → `$LC3`(4) → `$LC4`(8).

Quitando esos 16 B, `rcmp_vp6_codec .rodata` sería **`0x804101A4..0x804102E8`**
(324 B) — pero **`0x804101A4` es 4 mod 8**, así que cae en la veda de arriba.
Lo mismo `rcmp_mad_codec`, cuya base sería `0x80410324`. **Los dos códecs están
cerrados por la alineación, no por el dato.**

---

## Vtables duplicadas: el frente NO bloquea, medido

De las 27 de `vtdup.py` me tocaban cuatro. La medida es contundente:

**`gc_blockcalculator` promociona con `DOL OK` emitiendo sus 96 B de vtables
duplicadas** (`_vt.Q26Realmc15BlockCalculator` y
`_vt.Q26Realmc18BlockCalculatorImp`, que también emite `gc_interface`), más 52 B
de `.text` de más. El enlazador se lleva lo que no está vivo y el DOL sale
idéntico. **No hace falta ni *key method* ni tocar la cabecera.**

Eso cierra por medida lo que la r32 dejó abierto con dos curas descartadas: en
las unidades donde la copia duplicada no la referencia nadie vivo, **la vtable
duplicada es gratis**. Sigue sin ser gratis donde la unidad sí la usa
(`rcmp_vp6_codec` con `_vt.Q32EA9Allocator10IAllocator`, 6 reubicaciones
internas; `rcmp_mad_codec` con `_vt.Q24RCMP5CODEC`, 4), pero ahí el bloqueo real
es la alineación, no la vtable.

---

## Dos rangos localizados que nadie tenía

La r31 los dio por «sin localizar» porque los estáticos de fdlibm repiten nombre
entre ficheros. Buscando por **contenido** (nuestro `.o` contra el DOL) salen:

| unidad | sección | rango | casa |
|---|---|---|---|
| `e_pow` | `.sdata` | `0x804FEEF0..0x804FF000` (272 B) | **160 B de prefijo**, 174/272 |
| `kf_rem_pio2` | `.sdata2` | `0x805007C4..0x80500818` (84 B) | **60 B de prefijo**, 72/84 |
| `k_rem_pio2` | `.sdata` | `0x804FF838..0x804FF878` (64 B) | **64/64 byte a byte** |

Los dos primeros confirman el diagnóstico de la r31: **son las mismas constantes
en otro orden**, y el trabajo que queda ahí es de orden de declaración en la
fuente, no de `splits.txt`.

**Y la frontera de `k_rem_pio2` que rompió la r32 tiene arreglo**: no había que
mover hacia atrás el `.sdata start:0x804FF878` (que hoy es de `s_floor.c`, no de
`k_rem_pio2`), sino **añadirle a `k_rem_pio2` un rango nuevo
`0x804FF838..0x804FF878`**. Así troceado, enlace base `DOL OK` y `keepchk` en 21.

---

## `filesys`: el diagnóstico completo, aunque no se pueda cobrar

Su `.rodata` del original es **`0x80413A7C..0x80413CE8` (620 B)**, y nuestro
objeto lo reproduce entero salvo dos cosas:

- **4 B de relleno** entre la última cadena (`lbl_80413AD8`, acaba en
  `0x80413ADA`) y la primera vtable (`0x80413AE0`). Nosotros ponemos la vtable en
  `+0x60` y el objetivo en `+0x64`.
- (Ya no hace falta) los 128 B de `_vt.Q28RealFile12DeviceDriver` duplicada: **a
  mitad de mi tanda otro agente los quitó** — `filesys.o` pasó de `.rodata` 744 B
  a **616 B** y de `.text` 10.984 a 10.868 tocando
  `realcore/include/common/realcore/file/driver.h`. **616 + 4 = 620.**

O sea: a `filesys` le falta **exactamente ese relleno de 4 B** y una instrucción
en `AddToQueue`. Los dos bloques de vtables y cadenas casan **byte a byte** con
el DOL (`0 bytes distintos` en los 384 B de las ocho vtables y en los 136 B de
`$LC10`+`NullFileDriver`).

## `pathbank`: le falta una cadena repetida

Su `.rodata` casa **byte a byte los 164 B** contra `0x8041399C`. El objetivo
tiene 172: el original emite **`"%s  %s\0"` DOS VECES**
(`0x80413A38` y `0x80413A40`) y nuestro GCC funde los dos literales idénticos en
uno. Ese es el hueco de 8 B; la función pendiente (`PATHI_…`) es lo otro.

---

## Lo que NO toqué y por qué

- **`math_support .sdata2 0x80500388..0x80500390`**: el «anclaje NO único» del
  encargo. Confirmado que `Zero` de `math_support` resuelve a esa dirección por
  homonimia; y **no hace falta**, la unidad ya promociona.
- **`ppc2D2`** (2.448 B): le faltan 7 funciones de 64 bits de LibSN
  (`__div2i`, `__mod2i`, `__shr2u`, `__shl2i`, `__shr2i`, `__div2u`…). El
  enlazador lo dice al intentarlo: `auto_06_804394D0_data(1) : L0039: Reference
  to undefined symbol __mod2i`. Es escribir código.

## Trampas de convivencia que costaron tiempo

- **`promodist.py --libs --listas` a un fichero y leer con `while read`**: la
  salida lleva **CRLF**, y `trypromo.py` contestó **`NO esta en la lista de
  enlace` a las 43 unidades**. No era el `build.ninja` vivo (la trampa conocida
  de la r31): era el `\r` pegado al nombre. `tr -d '\r'` y las 43 contestaron.
- **`build_direct.py <filtro>` compila más de lo que pides**: `rcmp_vp6_codec`
  arrastró también los `*_chunk_types`.
