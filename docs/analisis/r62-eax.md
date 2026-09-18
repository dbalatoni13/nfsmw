# r62 — `zEAXSound` + `zEAXSound2`

Agente `eax`. Unidades en exclusiva: `Speed/Indep/SourceLists/zEAXSound` y `…/zEAXSound2`.

**Ninguna de las dos promociona.** Lo que sí traigo, todo aplicado y sellado:

1. **`zEAXSound` −8.431 B de DOL.** El «16 B» del encargo está resuelto **al byte y con
   causa**: el objetivo emite **34** parejas `(guarda, _.tmp_N)` de 8 B en la cabeza de su
   `.bss` y nosotros **32**. 16 B de relleno lo cierran, y `gAudioMemoryManager` cae por
   primera vez en su `0x8045B2F8`.
2. **`zEAXSound2` −9.931 B de DOL.** Su `bss+32` era el andamio de la r56 **descalibrado por
   la raya nueva de aranges**: 64 B → **56 B**. `linkdelta` pasa de `bss+32` a **`IGUAL`**.
3. **Refutada la línea de salida de la r61 §8.4**: el orden de la `.rodata` de `zEAXSound`
   **NO** exige tocar `IVehicle.h` (cabecera compartida). Se mueve desde el propio
   SourceList, y está medido — pero **no compensa hoy**, porque renumera 35 `$LC`. §4.
4. Dos negativos más, medidos, sobre el orden de la `.data` de `zEAXSound2`. §5.

Ficheros de fuente tocados: **dos, los dos míos**. Cero cabeceras, cero `config/`, cero
`keep.lst`, cero `splits.txt`, cero `configure.py`. **Cero correcciones de `lcfix`
pendientes**, y no es una suposición: `keepchk.py` da **0** entradas rancias de
`zEAXSound.o` y `zEAXSound2.o` (las 51 rancias del árbol son de otros).

---

## 0. Estado, antes y después

| | antes | después |
|---|---|---|
| `zEAXSound` `.o` sha1 | `bca3254…` (r61) → hoy el de la ventana | **`c50d57d3cced7feea2cafbd7c216479209d4637d`** (3 compilaciones) |
| `zEAXSound` `fncmp` | 0 de 773 | **0 de 773** |
| `zEAXSound` `linkdelta` | `.text +0 IGUAL` | `.text +0 **IGUAL**` |
| `zEAXSound` DOL | `7791fd2316f1` — **31.728 B**, 12.429 rangos | `fddec57f7c90` — **23.297 B**, 4.423 rangos |
| `zEAXSound2` `.o` sha1 | `2a3ee786…` (r61) | **`874895abb764e1a011a964f3428200867f9f524a`** (3 compilaciones) |
| `zEAXSound2` `fncmp` | 1 de 930 (`17 insn`) | 1 de 930 (`17 insn`) — sin tocar |
| `zEAXSound2` `linkdelta` | `.text +0 **bss+32**` | `.text +0 **IGUAL**` |
| `zEAXSound2` DOL | `29f3826ee3d8` — **33.502 B**, 15.376 rangos | `f94a9c201716` — **23.571 B**, 5.854 rangos |
| las dos JUNTAS | — | `829803752e33` — **51.041 B** |

**Total: 18.362 B de DOL menos**, medidos unidad a unidad.

**Control en cada corrida, y podía fallar**: el enlace base (sin sustituir nada) reproduce
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`, el DOL de referencia, **byte a byte**.

> **Nota de medida.** Mis cifras cuentan **bytes que difieren**, no rangos: `dolwhere.py`
> extiende un rango mientras difiera alguno de los 12 siguientes y por eso el encargo hablaba
> de 50.323 B donde hay 31.728. Las dos medidas son monótonas; la mía es la que se puede
> sumar.

---

## 1. `zEAXSound`: qué había en la cabeza de su `.bss`. Respuesta completa

Con la raya de aranges aplicada (`0x8045B1E0`), `movidos.py` canta **un escalón de −16 B que
comparten 1.246 símbolos**. El primer símbolo que se mueve es `gAudioMemoryManager`
(`0x8045B2F8` en el objetivo, `0x8045B2E8` nuestro). Todo lo que va detrás — el resto de la
`.bss`, y por arrastre cada `@ha/@l` del `.text` de **todo el DOL** — sale desplazado.

Volcando la `.bss` de los dos objetos ordenada por offset:

```
OBJETIVO (extraido)              NUESTRO
0x000 pad_07_8045B1E0_bss (136)  0x000 k.21792 / _.tmp_0
0x088 k.30900 / _.tmp_17         ...
0x090 gap_07_8045B270_bss (24)   0x0fc _.tmp_31.32558
...                              0x100 kFloatScaleUp
0x110 kFloatScaleUp              0x104 kFloatScaleDown
0x114 kFloatScaleDown            0x108 gAudioMemoryManager
0x118 gAudioMemoryManager
```

La cabeza es una serie **perfectamente regular** de parejas de 8 B: guarda de 4 B +
`_.tmp_N` de 4 B. El DWARF del ELF original lo confirma sin un hueco:
`_.tmp_0 @0x8045B1E4`, `_.tmp_1 @0x8045B1EC` … `_.tmp_33 @0x8045B2EC`, es decir
`0x8045B1E4 + 8N` para N=0..33. **34 parejas = 0x110**, y `0x8045B1E0 + 0x110 = 0x8045B2F0`
= `kFloatScaleUp`. Cierra al byte.

Nosotros emitimos **32**. Ésos son los 16 B.

### Las 34 ranuras del objetivo, identificadas

Sacadas del `.debug` del ELF original (el DIE de la guarda cuelga de su `TAG_subroutine`, y
de ahí saco el `AT_low_pc`) y **confirmadas por un segundo camino independiente**: un escáner
de parejas `lis`+D-form sobre el `.text` **enlazado**. Las dos fuentes dan la misma lista.

| ranura | dirección | función que la referencia |
|---:|---|---|
| 17 | `0x8045B268` | `_GetKind__16MNotifyMusicFlow` |
| 21 | `0x8045B288` | `ConnectionClass__Q29SoundConn15Pkt_Car_Service` |
| 22 | `0x8045B290` | `SType__Q29SoundConn15Pkt_Car_Service` |
| 25 | `0x8045B2A8` | `ConnectionClass__Q29SoundConn16Pkt_Heli_Service` |
| 26 | `0x8045B2B0` | `SType__Q29SoundConn16Pkt_Heli_Service` |
| 29 | `0x8045B2C8` | `_GetKind__12MAIEngineRev` |
| 30 | `0x8045B2D0` | `prevbrakestate`, en `UpdateGearShiftState__15SFXCTL_Shiftingf` |
| 31 | `0x8045B2D8` | `_GetKind__23MNotifyVehicleDestroyed` |
| 32 | `0x8045B2E0` | `_GetKind__14MCountdownDone` |
| 33 | `0x8045B2E8` | `_GetKind__15MPursuitBreaker` |

**Las otras 24 están MUERTAS**: ni una reubicación las nombra en el `.text` enlazado. Y las
ranuras **23, 24, 27 y 28** no tienen **ni siquiera DIE** en el DWARF — su función no llegó a
la imagen. Son las que nos faltan.

Nuestras 32 parejas llevan **las mismas 10 vivas**, una muerta de más (`k.30103`, sin una
sola reubicación) y **en otro orden**: `MNotifyMusicFlow` nos sale la 31 y al objetivo la 17;
`MCountdownDone`/`MNotifyVehicleDestroyed` salen intercambiadas.

### Y el orden NO importa. Medido

Parcheando el `.o` para clavar las **10 parejas vivas en su ranura exacta** del objetivo:

    32 parejas, sin relleno                                31.728 B
    + 16 B de relleno (lo aplicado)                        23.297 B   <- -8.431 B
    + 16 B de relleno Y las 10 vivas en su ranura exacta   23.292 B   <- el orden: 5 B

**Manda el TAMAÑO de la cabeza, no su orden ni su contenido.** Es la misma lección que
`nfsmw-bss-y-huecos-estripados`, ahora con la cifra al lado.

### Lo aplicado

Al final de `src/Speed/Indep/SourceLists/zEAXSound.cpp`, tras los `asm()` de `.rodata`:

```c
asm(".section .bss\n"
    "  .space 16\n"
    ".previous\n");
```

Y **el `asm .space 104` de la r61 §8.2 queda formalmente muerto**: la r61c ya lo había
refutado sobre el papel; aquí está refutado sobre el enlace. El valor bueno es 16.

Control de equivalencia, que podía fallar: los 16 B por parche de ELF (creciendo un símbolo)
y los 16 B por `asm()` de fuente dan **el mismo sha1 de DOL**, `fddec57f7c90`. Y un tercer
parche (hueco anónimo en vez de símbolo crecido) da también ése: `-strip-unused-data` **no**
se lleva el hueco anónimo.

---

## 2. `zEAXSound2`: el andamio de la r56 estaba descalibrado por la raya nueva

La r56 puso `asm(".section .bss / .space 64")` con la raya vieja (`0x8045DE60`). La r61c la
movió a `0x8045DDF8` y el óptimo se desplazó 8 B. **Barrido completo**, parcheando el `.o` y
enlazando entero (bytes del DOL que difieren):

| `.space` | DOL |
|---:|---:|
| 40 | 31.474 |
| 48 | 31.274 |
| 52 | 24.002 |
| 54 | 24.002 |
| **56** | **23.571** ← mínimo |
| 58 | 33.501 |
| 60 | 33.498 |
| 64 (lo de antes) | 33.502 |
| 72 | 33.677 |
| 80 | 33.773 |
| 88 | 35.533 |
| 96 | 35.842 |

Con 56, `kFloatScaleUp` cae en `0x100` desde el inicio de la ventana, que es donde lo tiene
el objetivo (32 parejas guarda/`_.tmp_N` delante, contra nuestras 25 parejas + este relleno),
y `linkdelta` pasa de `bss+32` a **`IGUAL`**.

**La lectura que vale para el resto del proyecto**: la familia D sigue muerta —los andamios
NO envenenan, sostienen— **pero un andamio de tamaño SÍ se recalibra cuando se mueve la raya
de `splits.txt` que lo justificaba**. No es «caduca»: es que la ventana cambió de sitio. Con
las 4 rayas de P1 aplicadas en esta ventana, **cualquier `.space` de la r56/r57 de las cuatro
unidades vecinas es sospechoso de estar 8 o 16 B fuera**, y el barrido cuesta un minuto por
unidad con el guion de §7.

---

## 3. Lo que queda, por sección

Con las dos `.bss` cerradas:

| | `.text` | `.rodata` | `.data` | `.over` | total |
|---|---:|---:|---:|---:|---:|
| `zEAXSound` | 13.381 | 8.651 | 1.259 | 6 | **23.297** |
| `zEAXSound2` | 6.943 | 10.901 | 5.719 | 8 | **23.571** |

En las dos, el `.text` **no es código**: `fncmp` da 0 de 773 y 1 de 930. Son los `@ha/@l`
que apuntan a una `.rodata`/`.data` permutada. El frente real de las dos unidades es el
**ORDEN de los datos**.

`rodorden zEAXSound`: **169 de 277 cadenas en secuencia, 108 fuera.**

---

## 4. EL HALLAZGO QUE REFUTA A LA r61, con su negativo

La r61 §8.4 cerraba diciendo: la palanca del orden de `.rodata` vive en
`Interfaces/Simables/IVehicle.h`, **cabecera compartida, no mía, no se toca**.

**Es falso que haya que tocarla.** El orden de parseo se mueve desde el propio SourceList:
basta con abrir las cabeceras de AttribSys **antes** del primer `#include` de `zEAXSound.cpp`.
El orden del objetivo lo da su propia `.rodata`:

```
obj[6..13]  Attrib::Attribute Instance Definition Class Database TypeDesc RefSpec Blob
obj[14]     Attrib::Gen::simsurface      <- la PRIMERA cabecera AttribSys del objetivo
obj[15]     Attrib::TAttrib
obj[16..20] Attrib::Gen::ecar camerainfo effects audioimpact audioscrape
```

y el nuestro abre `pvehicle` la primera (nue[16]) y `simsurface` la cuarta (nue[19]).

**Medido, con `rodorden` y con el DOL:**

| variante | en secuencia | `linkdelta` | DOL |
|---|---:|---|---:|
| base | 169/277 | IGUAL | 23.297 |
| `#include simsurface.h` arriba | 170/277 | `rodata−128` | 242.661 |
| … + 128 B de ceros de compensación | 170/277 | IGUAL | **23.165** (−132 B) |
| … + `ecar/camerainfo/effects/audioimpact/audioscrape` | 174/277 | `rodata−656` | 242.563 |
| … + 656 B de compensación | 174/277 | IGUAL | **23.067** (−230 B) |

`fncmp` se queda en **0 de 773** en las cinco variantes: no toca una sola instrucción.

**POR QUÉ LO REVIERTO, y es el negativo que hay que leer**: `keepchk.py` pasa de **0**
entradas rancias de `zEAXSound.o` a **35** —`$LC147..154`, `$LC229/230`, `$LC245..248`,
`$LC337..340`, `$LC361..363`, `$LC392..394`, `$LC396`, `$LC399`, `$LC414`, `$LC531`,
`$LC153`, `$LC167`, `$LC219`, `$LC231/232`, `$LC347`, `$LC360`, `$LC567`. Las cabeceras
nuevas meten literales delante y **renumeran los `$LC`**: es `nfsmw-lc-se-desplaza` al pie de
la letra. **Los 656 B de relleno no compensaban una permutación: tapaban una pérdida.**

230 B a cambio de una corrida de `lcfix.py` en mitad de la ventana —que envenena a los demás
agentes— **no compensa hoy**. Queda anotado junto a la función para `previo.py`.

**Para quien lo retome**: hay que hacer las dos cosas a la vez (includes + `lcfix`), en una
ventana en la que `zEAXSound` sea el único que toca `.rodata`. El techo es los **8.651 B** de
`.rodata` más la parte del `.text` que cuelga de ellos (13.381 B, casi todo `@ha/@l`).

---

## 5. Los negativos de `zEAXSound2`: el ORDEN de la `.data` no es el frente

Su `.data` sale muy permutada: los 15 andamios `gap_`/`pad_`/`lbl_` del final del SourceList
se colocan **+8.000..+9.900** respecto de su sitio, y los 201 símbolos reales salen
**−132..−280**. La hipótesis obvia —mover los `asm()` entre los `#include`— se prueba
recolocando la `.data` del `.o` a mano:

| variante | tamaño de `.data` | DOL |
|---|---|---:|
| base | 0x283C | 23.571 |
| cada símbolo en el offset EXACTO del `.o` extraído | 0x28CC (+144) | **191.914** |
| sólo los 15 andamios clavados, el resto empaquetado | 0x29C0 (+388) | **213.201** |

Las dos son **ocho y nueve veces peores**, y por la misma razón que manda en `.bss`: en
cuanto el **tamaño** de la ventana cambia, el DOL explota y la medida del orden queda
contaminada. No se puede evaluar el orden de la `.data` sin mantener el tamaño **al byte**, y
eso exige resolver antes los **19 símbolos que emitimos y el objetivo no**.

---

## 6. La trampa nueva de la ventana (para `docs/TRAMPAS.md`)

**Un `.o` sustituido con OTRO NOMBRE DE FICHERO no es el mismo experimento.** `keep.lst`
indexa por **nombre de objeto** (`zEAXSound.o:$LC61`), así que enlazar
`scratchpad/…/eax_q0.o` en lugar de `…/zEAXSound.o` **pierde las 90 entradas de keep de esa
unidad en silencio** y `-strip-unused-data` se lleva el bloque.

Medido, y el control era un fichero **byte a byte idéntico** al original:

    build/GOWE69/src/…/zEAXSound.o          31.728 B
    scratchpad/eax62/eax_q0.o (idéntico)   277.488 B     <- 8,7x, sin cambiar un byte

Me costó tres experimentos dar tres resultados catastróficos que no eran del experimento.
**Cualquier sonda que sustituya un objeto tiene que conservar el basename.**

---

## 7. Banco, si alguien retoma estas dos

En `scratchpad/eax62/` (sólo `.py`, los volcados borrados):

* `probe.py` — enlaza con sustituciones arbitrarias y lee tablas de símbolos y secciones de
  los ELF enlazados.
* `meas.py` — `measure({unidad: ruta_o_None})` → `(bytes distintos, rangos, sha1, por sección)`.
  Es la métrica sumable de todo el informe. **Conserva el basename**.
* `bsspatch.py` / `bsspatch2.py` — inserta o quita N bytes en `.bss` de un `.o` a un offset
  (hueco anónimo, o dándoselos al símbolo que termina ahí). Es lo que hace el barrido de §2
  sin recompilar: **1 s por punto contra los 47 s de `build_direct`**.
* `reorder_bss.py` — clava las parejas vivas en su ranura del objetivo (la medida de §1).
* `reorder_data.py` / `reorder_gaps.py` — las dos recolocaciones de `.data` de §5.
* `dwstat.py` — recorre el `.debug` (DWARF-1) del ELF original con **anidamiento por
  `AT_sibling`** y da, para cada variable con dirección en un rango, **la función que la
  contiene**. Es lo que `dwarfmap.py --dir` no sabe hacer.
* `scanref.py` — escanea el `.text` **enlazado** buscando parejas `lis`+D-form que apunten a
  un rango, y dice desde qué función. La confirmación independiente de `dwstat`.
* `whoref.py` — para un `.o` nuestro, qué función referencia cada símbolo local.
* `rostr.py` — imprime la secuencia de cadenas de `rodorden` (`obj` o `nue`) por índice.

---

## 8. Regresiones

* `fncmp zEAXSound`: **0 de 773** antes y después. `fncmp zEAXSound2`: **1 de 930**
  (`GenerateRoadNoise`, `17 insn`) antes y después — no la he tocado.
* `linkdelta`: `zEAXSound` `IGUAL` (igual que antes); `zEAXSound2` **`bss+32` → `IGUAL`**.
* `checksplits.py` → `0 solapes, 0 rangos que cortan un símbolo, LIMPIO`.
* `prefijochk.py` → `LIMPIO`.
* `keepchk.py` → **0** rancias de `zEAXSound.o` y `zEAXSound2.o` (51 en el árbol, ninguna
  mía). **`lcfix` pendientes: 0.** Ninguna entrada venenosa.
* `gapchk` sin entradas nuevas de mis unidades.
* Sellado (regla 6): `zEAXSound.o` = `c50d57d3cced7feea2cafbd7c216479209d4637d` y
  `zEAXSound2.o` = `874895abb764e1a011a964f3428200867f9f524a`, **tres compilaciones
  consecutivas cada uno** sobre la fuente final.
* Control de base en cada enlace: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

---

## 9. Propuestas para el jefe

1. **Nada de `config/`, nada de `splits.txt`, nada de `configure.py`, nada de `keep.lst`.**
   Las cuatro rayas de P1 ya están y son correctas: `zEAXSound` cierra con ellas por
   aritmética exacta.
2. **Recalibrar los andamios `.space` de las vecinas de P1.** `zEAXSound2` estaba 8 B fuera
   sólo por el movimiento de raya. `zEagl4Anim` y `zEcstasy` movieron raya en el mismo
   paquete: merecen el mismo barrido, que con `bsspatch.py` cuesta un minuto por unidad y no
   toca la fuente.
3. **Para otra ronda: `zEAXSound` + `lcfix`, en exclusiva.** Los `#include` tempranos de
   AttribSys (§4) mueven el orden de la `.rodata` **desde el SourceList**, sin tocar ninguna
   cabecera compartida. Con `lcfix` en la misma mano, el objetivo son los 8.651 B de
   `.rodata` y la parte del `.text` que cuelga. Sin `lcfix` en la misma mano, no.
4. **`zEAXSound` no necesita escribir ni una línea de código.** `fncmp` 0 de 773 y las 34
   ranuras de `.bss` identificadas: lo único que la separa del DOL es el **orden de los
   datos**, y ahora se sabe con qué palanca se mueve.
