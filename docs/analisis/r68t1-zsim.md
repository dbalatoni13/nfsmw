# r68 tanda 1 — lote zSim: cuatro cabezas de literal en asm → `@lc`

Receta S1 del plan (medida antes por lote3). Cuatro bloques `asm()` anónimos de `.rodata`, bajo
`#ifdef ZSIM_HAND_POOL`, escribían a mano la CABEZA de un literal que nuestro `zSim.o` ya emite y
que el enlace estripaba (`size & ~7` por delante, sin referencias vivas). El original tiene los
cuatro literales ENTEROS en el DOL. Se quita el asm y keep.lst conserva el literal de GCC por su `$LC`.

**Resultado: 4 de 4 retirados.** El DOL es 9619ba57 y hay un control por bloque que rompe justo en la dirección de su cadena.

## Tabla bloque a bloque

| fichero:línea (HEAD) | bytes | qué era | literal y `$LC` (en nuestro objeto) | dirección en el DOL (cadena entera) | veredicto |
|---|---|---|---|---|---|
| `Sim/Activities/GameplayActivity.cpp:11` | 8 | cabeza `"MNotifyS"` (0x4D4E6F74 0x69667953) | `"MNotifySimTick"`, 15 B, `$LC626` (@0xC08), nombre del mensaje en `MNotifySimTick.h` | 0x80404B40 | RETIRADO |
| `Sim/Activities/NISActivity.cpp:34` | 8 | cabeza `"MNISComp"` (0x4D4E4953 0x436F6D70) | `"MNISComplete"`, 13 B, `$LC550` (@0x9CC), `MNISComplete.h` | 0x80404904 | RETIRADO |
| `Sim/Activities/NISActivity.cpp:70` | 16 | cabeza `"MNotifyMovieFini"` | `"MNotifyMovieFinished"`, 21 B, `$LC555` (@0xA20), `MNotifyMovieFinished.h` | 0x80404958 | RETIRADO |
| `Sim/Entities/LocalPlayer.cpp:119` | 8 | `"FEngHUD\0"` entero (8 & ~7 = 8) | `"FEngHUD"`, 8 B, `$LC478` (@0x6D0), tag de `::new ("FEngHUD", __LINE__)` en `LocalPlayer::SetHud` | 0x80404618 | RETIRADO |

Evidencia del nombre: aquí no hay variable ni nombre dudoso (criterio 3). El «dato» es el literal
que GCC ya emite desde la cabecera del mensaje o desde la llamada `::new` real. Lo he comprobado en
el objeto (tabla de símbolos: `$LC` de `.rodata` con esa cadena) y en el DOL original: cada cadena
aparece entera, seguida de su vecina (`"SimTime"`, `"NISName"`, `"High"`, `"LocalPlayer"`).
Constructo del criterio 1: el literal de la cabecera o de la llamada viva. No se usa primer, `#line`, `aligned` ni `section`.

## Cambio en la fuente

Cada bloque `#ifdef ZSIM_HAND_POOL … asm(…) … #endif` se sustituye por una nota r68t1 con el mismo
número de líneas (15, 14, 11 y 12). Así se evita la trampa de `__LINE__`: `LocalPlayer.cpp` la
usa en `SetHud`, justo encima, y se conserva la cuenta en los tres ficheros por seguridad. Comprobado:

- `wc -l`: 93 / 1466 / 1052, igual que en HEAD.
- `LocalPlayer.cpp` sigue en CRLF (1052 CRLF de 1052 LF); los otros dos siguen en LF.
- `.text` 109324 → 109324 y `.rodata` 8384 → 8344 (−40 = 8+8+16+8).
- Los 205 `$LC` de `.rodata` tienen el mismo número y el mismo contenido que en la base.
- En particular, `$LC523` (el `.set lbl_80404864, $LC523` de QuickGame.cpp) no se mueve. S1 no renumera nada.

## Medidas (relink.py de lote5_68, keep en copia; nunca build/ ni config/)

Arnés: `scratchpad/zsim68t1/comp.py` (la `compila()` de `jefe/cmphead.py`, cwd = raíz). La compilación
de HEAD reproduce el objeto del build: `fbc6763b193439a8555fa0dce75ae7e98eff0efb`, igual que
`build/GOWE69/.../zSim.o`.

keep.lst de partida: `config/GOWE69/keep.lst` sha1 `b0646b0412ddf53bbd857b95deadc18ba9f4120c` (sin cambios durante el trabajo).

| variante | objeto zSim.o | keep | DOL |
|---|---|---|---|
| base, sin sustituciones | (build) | real | 9619ba57 OK |
| base compilada por mí | fbc6763b… | real | 9619ba57 OK |
| sin asm, SIN keep (control global) | 158d405d… | real | ROTO: `.rodata` 312608 → 312576 |
| **final: sin asm + 4 `@lc`** | **158d405d…** | keep_final.lst | **9619ba57 OK** |
| final, las 4 entradas al final del fichero (lcres.py --add) | 158d405d… | keep_lcres_v1.lst | 9619ba57 OK |
| control sin `FEngHUD` | 158d405d… | keep_final − 1 | ROTO, 1ª dif `.rodata` en 0x80404618 |
| control sin `MNISComplete` | 158d405d… | keep_final − 1 | ROTO, 1ª dif en 0x80404904 |
| control sin `MNotifyMovieFinished` | 158d405d… | keep_final − 1 | ROTO, 1ª dif en 0x80404958 |
| control sin `MNotifySimTick` | 158d405d… | keep_final − 1 | ROTO, 1ª dif en 0x80404B40 |

Cada control rompe exactamente en la dirección de su cadena, así que cada una de las cuatro entradas es necesaria.
El orden de las entradas dentro de keep.lst no importa: la variante con las cuatro al final del
fichero da el mismo DOL.

Objeto final: `C:\Users\jferr\Desktop\nfsdecompiled\scratchpad\zsim68t1\final\zSim.o`, sha1
`158d405dd90fd3d9c09b0db6878aafe4bc88591c`.
Keep usado: `C:\Users\jferr\Desktop\nfsdecompiled\scratchpad\zsim68t1\keep_final.lst`.
Repetir: `python scratchpad\lote5_68\relink.py build/GOWE69/src/Speed/Indep/SourceLists/zSim.o=scratchpad/zsim68t1/final/zSim.o keep=scratchpad/zsim68t1/keep_final.lst`

## keep.diff (`scratchpad\zsim68t1\keep.diff`)

No se quita ninguna línea: los cuatro asm eran anónimos y no tenían entrada. Se añaden 8 líneas
detrás de `zSim.o:$LC628` (el par de `"TimeStep"`, líneas 2320-2321 del keep.lst actual) y delante de `zSim.o:lbl_80404940`:

```
  # @lc zSim "TimeStep"
  zSim.o:$LC628
+ # @lc zSim "FEngHUD"
+ zSim.o:$LC478
+ # @lc zSim "MNISComplete"
+ zSim.o:$LC550
+ # @lc zSim "MNotifyMovieFinished"
+ zSim.o:$LC555
+ # @lc zSim "MNotifySimTick"
+ zSim.o:$LC626
  zSim.o:lbl_80404940
```

keep.lst es CRLF: las líneas nuevas van con CRLF. Los números están resueltos por contenido contra
el objeto final (`lcres.py`: 0 renumeradas, 0 sin resolver) y coinciden con los del plan.

## Fuera del lote (sin tocar)

`SourceLists/zSim.cpp:211` (mezcla `@lc` con primer), `NISActivity.cpp:54` y `:84` (primer / R4) y el prefijo de 92 B.
El `#define ZSIM_HAND_POOL` de `zSim.cpp` sigue haciendo falta para los otros bloques guardados.
