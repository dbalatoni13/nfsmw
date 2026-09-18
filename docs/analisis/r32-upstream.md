# Verificación de upstream (dbalatoni13/nfsmw) — 06-sep-2026

Barridas **las 11 ramas** de `https://github.com/dbalatoni13/nfsmw` contra nuestro
árbol. Conclusión corta: **no hay nada que importar, y fusionar costaría caro.**

## Dónde está cada rama

| rama | nos faltan | les faltan | último commit |
|---|---|---|---|
| `eaxsound-clean` | 1.169 | 838 | 2026-08-10 Apply this-> to zEAXSound |
| `bit_of_ecstasyy` | 566 | 777 | 2026-08-07 |
| `ps2_fixes` | 209 | 726 | 2026-08-18 |
| `csis` | 50 | 714 | 2026-09-05 some rcmp |
| `thingies` | 26 | 713 | 2026-09-06 Fix A124 region |
| `main` | **19** | **713** | 2026-09-06 |
| `pr-117` | 15 | 723 | · `physicsdev` 14 · `zFE-dev` 14 · `xbox_stuff` 11 · `dev` 2 |

**Estamos entre 713 y 838 commits por delante de todas.**

## La medida que decide: objetos `Matching`

```
HEAD                    275 de 415        (1 SourceList)
upstream/csis            28 de 352
upstream/main             1 de 288        (0 SourceLists)
upstream/thingies         1 de 288
upstream/eaxsound-clean   0 de 347
```

**Y de los 27 objetos `Matching` de `csis`, los 27 ya lo son aquí. Cero
diferencia.**

## Qué son sus 19 commits nuevos de `main`

Nada de match de GameCube en nuestro terreno:

- **Otras plataformas**: PC 1.3 + símbolos de Xenon, splits de Xbox, la versión
  de PS2 partida bien, arreglos de compilación de PS2/Xbox.
- **Estilo y refactor**: «Apply `this->` to zWorld/zWorld2/zTrack», «Apply
  `!= nullptr` to zAI», «use `this->` in zAI».
- **Herramientas**: `dwarf-compare` más rápido, hashes tomados de Carbon.
- Tres que sí suenan a match — «Match most of zAI», «Match all of zEAXSound
  except functions that rely on FE», «zWorld2 match improvements»— pero ver
  abajo.

## Comprobado función a función, no de oídas

De nuestras **101 funciones pendientes**, 20 están en ficheros que difieren de
`upstream/main`. Miradas por dentro, las diferencias son **nuestras**: en
`AvoidableManager.cpp` (zAI) el diff entero es que **nosotros** sacamos la clase
a un `.hpp` y ellos la tienen aún dentro del `.cpp`.

Y los dos ficheros de SourceList con más diferencia son `zEcstasy.cpp`
(**+9.599**) y `zMisc.cpp` (**+4.187**): son los pools de datos que **nosotros**
escribimos a mano y ellos no tienen.

## Los 12 ficheros que sólo existen en upstream

Todos esbozos, y ninguno aporta:

```
EAXEnvironment.cpp 15 lineas · EAXEnvironment.hpp 6 · UTLSequencer.h 21
filesys_cc.h 35 · realcore/std/memory.h 7 · endian/big/endian.h 60
NFSLiveLink.hpp 41 (ya lo tenemos)
RealStream/src/stream.cpp 1.217 (lo tenemos como realstream/, es el commit
                                 "Fix path capitalization")
```
más cuatro de PC/Xenon (`xSparks.h`, `eLightPlat.hpp`, `TextureInfoPlat.hpp`).

## Coste de fusionar, y por qué no ahora

`upstream/main` toca **346 ficheros de `src/`** en los commits que no tenemos,
de los que **322 difieren** del nuestro, y además `config/GOWE69/symbols.txt`
(**2.595 líneas**), `config.yml` y `keep.lst` — justo los tres ficheros sobre
los que se apoya todo nuestro trabajo de promoción.

**Una fusión sería un conflicto enorme a cambio de cero ganancia en GameCube.**
Lo que sí conviene es **volver a mirar cuando queramos aportar hacia arriba**:
nuestro trabajo de `linked` (275 objetos contra 1) es lo que le falta a upstream,
no al revés.
