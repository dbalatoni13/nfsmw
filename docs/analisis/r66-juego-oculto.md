# r66 — lote «juego-oculto»: el andamio que tapa código que falta

**Encargo**: cazar, entre los andamios del código de juego (`src/Speed/Indep/Src`,
`src/Speed/Indep/bWare`, `src/Speed/GameCube`), los que esconden decompilación
pendiente, y escribir ese código. Test: secciones ALLOC idénticas.

## 1. Resultado

| | |
|---|---|
| andamios de juego, expresión del brief (`__asm__("")` + pin) | **97** (35 barreras + 62 pines) |
| más las barreras escritas `asm("")` (r65 §4.1) | 45 → **142 sitios en 71 funciones** |
| **andamios que tapan código que falta, entre los 97** | **1** (`CDActionDrive.cpp:422`) — y 1 entre los 142 |
| funciones con código que falta DEMOSTRADO por el DWARF | **3**: `CDActionDrive::Update`, `QuickGame::CreateCars`, `TrackCopCameraMover::Update` |
| de ellas, donde el andamio es INDEPENDIENTE (medido) | 2 (`CreateCars`, `TrackCop`) |
| código escrito | **2**: `CDActionDrive` en el árbol; `CreateCars` como parche sellado (ver §9) |
| no escrito | 1: `TrackCop` (contenido no recuperable, §5) |
| andamios | **97 → 96** (142 → 141); retirado 1 |
| regresiones | 0 (zCamera y zSim sellados sección a sección, §8) |

**La cifra principal es 1.** La clase existe, pero es rara: de 71 funciones con
andamio, sólo en tres el original tiene código que nosotros no, y sólo en una ese
código era lo que el andamio sustituía. La r65 la describió bien en su caso, pero con
un diagnóstico que hay que corregir: **no es que el original recorra la lista de
jugadores para decidir `isBeingPursued`. Es CÓDIGO MUERTO.** Y lo que sostiene el
reparto es el contenido muerto, no la segunda asignación (§3, control B).

## 2. Método (scripts en `scratchpad/juego66/`)

Tres detectores, porque cada uno se equivoca de una manera distinta:

1. **`regmap` por función** (`rmall.py`, `clasif.py`), sobre `.o` compilados en
   scratch (`cc.py`, con `REGMAP_CACHE` propio). Se busca «N sólo en el ORIGINAL» y
   «el bloque X FALTA en el nuestro».
2. **Multiconjunto de inlines, original menos nuestro** (`inl.py`, `inl2.py`,
   `seq.py`). Ve el código que falta **sin locales**, que `regmap` no puede ver
   (TrackCop), y además dice DÓNDE falta, por orden de expansión.
3. **Rangos cero**: un bloque o un inline con `Range: X -> X` y **llamadas reales
   dentro** es código borrado. **Sin llamadas no prueba nada** (§6.1).

Y dos comprobaciones cruzadas: el volcado de PS2 (otro compilador; si allí la local
tampoco existe, se plegó) y `lmap` (las líneas de las sentencias borradas se amontonan
en la primera instrucción viva).

Cada cambio se midió con digests ALLOC por sección (`tool.py`), con `fnd.py`
(objdiff contra el `.o` original, `calculatePoolRelocations=false`) y **con un
control que tuviera que fallar**.

## 3. `CDActionDrive::Update` — el caso: la barrera TAPABA código muerto

**Evidencia** (`symbols/mw_dwarfdump.nothpp:326101`): el original abre un bloque
`{IPerpetrator *iperp; const List &playerList;}` con, anidados, `iter`, `ip`
(`IPlayer*`) y `simable` (`ISimable*`), más las expansiones de
`ListableSet<IPlayer,…>::GetList`, `begin`, `end`, `IUnknown::QueryInterface` e
`IPerpetrator::_IHandle`. **Todos los rangos valen `0x8006C740 -> 0x8006C740`.** El
volcado de PS2 no conserva ni `isBeingPursued` (la propagó y plegó el bloque). `lmap`:
la línea 439/442 del original está en `li r20,0` (0x8006C740) y la siguiente viva es
la 472: treinta líneas sin un byte.

**Escrito** (en el árbol, `CDActionDrive.cpp`):

```c
    isBeingPursued = false;
    if (false) {
        IPerpetrator *iperp;
        const IPlayer::List &playerList = IPlayer::GetList(PLAYER_LOCAL);
        for (IPlayer::List::const_iterator iter = playerList.begin(); iter != playerList.end(); ++iter) {
            IPlayer *ip = *iter;
            if (ip) {
                ISimable *simable = ip->GetSimable();
                if (simable && simable->QueryInterface(&iperp)) {
                    isBeingPursued = iperp->IsBeingPursued();
                }
            }
        }
    }
    mAnchor->SetCloseToRoadBlock(isBeingPursued);   // la barrera __asm__("") ya no esta
```

**Medido** (zCamera, base `da733f5538b6df67`):

| variante | digest ALLOC | filas `Update` |
|---|---|---|
| base: barrera, sin bloque | `da733f5538b6df67` | 0 |
| **bloque, sin barrera** | **`da733f5538b6df67`** | **0** |
| control A: sin barrera, sin bloque | `29daa6755ac67b96` (`.text`) | 4 |
| control C: sin barrera, `if (false) {}` vacío | `29daa6755ac67b96` | 4 |
| control B: bloque SIN `isBeingPursued = …` dentro | `da733f5538b6df67` | 0 |

El control A es el que tiene que fallar, y falla. El B corrige a la r65: **no hace
falta que `isBeingPursued` tenga una segunda asignación**; basta el contenido muerto
(las expansiones inline y los pseudos que crean antes de que `jump` borre el
bloque). El C dice que tampoco basta la estructura vacía del `if`.

`regmap` pasa de «46 iguales, **6 sólo del original**, 4 desajustes de bloques» a
«**51 iguales**»; lo único que queda es `old_pov`, un `static` de bloque que no es
andamio. La condición real del original no es recuperable (no deja bytes ni líneas):
`false` es la forma mínima que reproduce el árbol de bloques. Diagnóstico escrito
junto al sitio.

## 4. `QuickGame::CreateCars` — código muerto escrito, pero NO es lo que tapa el pin

**Evidencia** (`mw_dwarfdump.nothpp:1983834`): bloque `{Performance
maximum_performance; Performance pp1;}` con `ComputePerformance` inline y, anidado,
`{Performance pp2;}` con `ComputePerformance`, dos `Performance::Maximize` y dos
`CarBuilder::Match(const Performance&)`. **Todo en `0x80281D40 -> 0x80281D40`.** `lmap`
lo confirma de forma espectacular: en `0x80281D40` se amontonan las líneas 429, 430,
434, 435, 437, 438 y 443 de `QuickGame.cpp` junto a las de `pvehicle.h`,
`physicsinfo.hpp:161-166` (Maximize) y `QuickGame.cpp:257-264` y `275-289` (los
cuerpos de `ComputePerformance` y `Match`). PS2 lista `maximum_performance`, `pp1`,
`pp2`, dos `pvehicle vehicle` y dos `pvehicle test`. `CarBuilder::ComputePerformance`
no existía en nuestra fuente: se reconstruye de su DWARF (una `pvehicle vehicle` local,
líneas 257-264) con `Physics::Info::ComputePerformance`, que ya está declarada en
`PhysicsInfo.hpp:96`.

**Medido**, en compilación en sombra (§8), con `QuickGame.cpp` en su estado actual:

| variante | digest ALLOC zSim | filas `CreateCars` |
|---|---|---|
| actual (pin + barrera) | `6913d4cebd1c34a7` | 0 |
| **+ bloque muerto** | **`6913d4cebd1c34a7`** | **0** |
| + bloque, sin pin | `1a1bd1b50dca4c67` | 4 |
| sin bloque, sin pin (control) | `1a1bd1b50dca4c67` | 4 |

Las dos últimas dan **el mismo digest**: el bloque muerto es real y ahora está en el
DWARF (`seq.py` sólo echa en falta el bloque vacío del `else` de pantalla partida),
pero **el pin `collisionPos asm("r4")` es un problema aparte**: el objetivo pone la
dirección del temporal `WCollisionMgr(0,3)` en r3 y nosotros en r11 (`addi r11,r1,0x540`
/ `mr r3,r11`). Irreducible con su diagnóstico; texto propuesto en §9.

Quedan dos cosas de estructura en esta función que no son andamio: el bloque vacío
de 16 B del `else` (0x80281D30) y la local `UserProfile *prof` del bloque de carrera.

## 5. `TrackCopCameraMover::Update` — código muerto que no se puede escribir

**Evidencia** (`seq.py`): el original tiene inlines que nosotros no, todos de rango
cero en `0x80077018`: **dos `bTan`** entre `bLength` y `displacement /= distance`, y
**`GetGeometryPosition` ×2 + `GetVelocity`** justo antes de `SetTargetDistance`.
`bTan` es `bSin/bCos`, dos llamadas: sólo desaparecen si el código es inalcanzable.

**Medido** (zCamera; el bloque escrito como `if (false) { vert_comp = distance *
bTan(0); horiz_comp = distance * bTan(0); }` detrás del clamp de `distance`):

| variante | digest | filas |
|---|---|---|
| bloque, con las dos barreras | `da733f5538b6df67` (idéntico) | 0 |
| sin barreras, sin bloque | `51f6ec46a581c9c1` | 2 |
| sin barreras, con bloque | `51f6ec46a581c9c1` (el mismo) | 2 |
| con bloque, sin la de `hcomp` | `2d2f816393002c80` | 4 |
| con bloque, sin la de `dst` | `c51fb84c5d506117` | 2 |

**Las barreras no tapan el código que falta.** Y el código no se deja escrito: los
argumentos de `bTan` y el uso de `GetVelocity` no dejan locales, ni bytes, ni líneas,
así que cualquier forma sería inventada. Fuente restaurada byte a byte; diagnóstico
escrito junto a las barreras. Las dos son `asm("")`, no cuentan en los 97.

## 6. Lo que parecía la clase y no lo es

### 6.1 Rango cero SIN llamadas dentro no es código muerto: `FEngSetScaleX/Y`

El original declara `{TextureInfo *pTex;}` con `Range: 0x80131AE4 -> 0x80131AE4`, y
el detector 3 lo marcó. Es un bloque **vivo** (el `GetTextureInfo` está en el objeto);
GCC 2.95 deja el rango de bloque a cero cuando el `switch` recoloca los cuerpos. La
r65-fe ya midió que la forma limpia es una PERMUTACIÓN r30/r31. **Hace falta una
llamada real dentro del rango cero para hablar de código muerto.**

### 6.2 «Sólo en el original» que es ámbito

- `EPlayRaceNIS::EPlayRaceNIS` (3 sólo del original, 5 sólo nuestras, 8 de ámbito):
  el multiconjunto de inlines coincide salvo un `GetList` **de más** en el nuestro.
  Es emparejado por posición de `regmap` sobre bloques colocados a otra profundidad.
- `TrackStreamer::HandleLoading` (1 y 1): `n` vive en su propio bloque con `section`
  anidada, o sea un `for (int n…)`. Nosotros, `do/while` con los dos pines.
- `LockPalette`, `LoadCameraShakes`, `IssueScriptMessages`, `eSolid::GetPostionMarker`
  y `SFXObj_Collision::InitSFX`: la local existe en los dos, a otra profundidad, o hay
  un bloque vacío de más. Cero inlines de menos.

### 6.3 Inlines de menos que son FORMA, no código

Accesor que nosotros escribimos como acceso directo, o nombre distinto, en funciones
al 100 %: `CWorldAnimCtrl::GetAllocated`, `CustomizationScreen::GetCategory` ×3,
`Elements::Elements`, `SndBase::GetPhysCar`, `Timer::Timer`/`operator!=`,
`CarPartModel::GetModel` (los tres pines de `CreateCarLightFlares` reproducen su
`& ~3`), `operator new[](size, file, line)` (el `new` de depuración con fichero y
línea), `bSqrt`/`bDistBetween` sustituidos por `bSqrtNoCJ`/`bDistBetweenNoCJ`,
`FloatingPoint` contra `PackedDecimal` y `MPH2MPS` contra `TrackMPH2MPS`.

### 6.4 Sin oráculo DWARF (15 sitios)

`SetupNextLoad` no está en el volcado de GameCube (sí en el de PS2); su mapa de líneas
(911-925) va seguido con nuestras sentencias. `ResolveCurrentDataMemory`,
`ToggleCapsLock` y `bStrNCmp` están en el volcado **sin ninguna local**, así que las
nuestras sobran: es lo contrario de la clase. `ScratchPtr<…>::_Alloc` sólo resuelve
con el nombre de plantilla y en `zPhysicsBehaviors`: PERMUTACIÓN (`spbuffer`).
`FX_ROADNOISE` (`ENVIRO_AEMS.h`) sólo existe como copia inline; la r47 ya lo explicó
como empate de allocnos.

## 7. Clasificación de los 142 sitios

| clase | de los 97 | de los 142 | funciones |
|---|---|---|---|
| **A** tapa código que falta | **1** | **1** | 1 |
| **B** falta código, andamio independiente (medido) | 1 | 4 | 2 |
| **C** ámbito / árbol de bloques | 6 | 10 | 7 |
| **D** sobra una local nuestra / reparto | 59 | 83 | 34 |
| **E** mismo conjunto de locales (IDÉNTICO/PERMUTACIÓN) | 21 | 29 | 19 |
| **F** sin oráculo DWARF | 9 | 15 | 8 |

Estado de las funciones: **58 de las 64 que tienen oráculo están al 100 %** en su
objeto. Los seis near-miss (`eProject` 93,97 %, `GenerateRoadNoise` 96,80 %,
`ConvertWheelRotation` 98,84 %, `InitAtSegment` 99,66 %, `UpdatePlatInfo` 99,96 %,
`HolePunchAvoidables` 99,97 %) **no tienen ni una local ni un inline de menos**. O
sea: no hay código pendiente escondido en la parte que todavía no casa.

## 8. Sellos

**zCamera**, compilación NORMAL, espalda con espalda: A = `CDActionDrive.cpp` y
`TrackCop.cpp` como estaban antes de la r66; B = los dos finales (bloque, barrera fuera
y las dos notas). Los ficheros finales se comprobaron con `cmp` al volver.

| sección | A (pre-r66) | B (final) |
|---|---|---|
| `.text` | `c9e4271e2942:136020` | `c9e4271e2942:136020` |
| `.rela.text` | `ebfe4a759be8:93384` | `ebfe4a759be8:93384` |
| `.rodata` | `af8ea7774ffc:9872` | `af8ea7774ffc:9872` |
| `.rela.rodata` | `a51ab1a78166:3996` | `a51ab1a78166:3996` |
| `.data` | `0f230206151e:3136` | `0f230206151e:3136` |
| `.rela.data` | `424dae5f3e45:84` | `424dae5f3e45:84` |
| `.ctors` / `.rela.ctors` | `9069ca78e745:4` / `7d1d38f48e39:12` | iguales |
| `.bss` | `nobits:2000` | `nobits:2000` |
| **digest** | **`da733f5538b6df67`** | **`da733f5538b6df67`** |

`fnd`: `Update__13CDActionDrivef` 4564/4564 B, 0 filas; `Update__19TrackCopCameraMoverf`
948/948 B, 0 filas.

**zSim**, compilación EN SOMBRA (`cc2.py`: la SourceList copiada a `priv/` con el
`#include` de `QuickGame.cpp` apuntando a una copia privada, más `-I` del directorio
original **al final**), sobre el `QuickGame.cpp` ACTUAL, por la MISMA ruta privada:

| sección | actual | + parche |
|---|---|---|
| `.text` | `43193f3b6672:109324` | `43193f3b6672:109324` |
| `.rela.text` | `02cc67571158:77952` | `02cc67571158:77952` |
| `.rodata` | `ccd9b60ffe97:8384` | `ccd9b60ffe97:8384` |
| `.rela.rodata` | `176b0cc3a681:5604` | `176b0cc3a681:5604` |
| `.data` / `.rela.data` | `4c8f8036143b:348` / `82e53f430d98:12` | iguales |
| `.ctors` / `.rela.ctors` | `9069ca78e745:4` / `768ffafc4592:12` | iguales |
| `.bss` | `nobits:8164` | `nobits:8164` |
| **digest** | **`6913d4cebd1c34a7`** | **`6913d4cebd1c34a7`** |

Control de la sombra: sombra del fichero sin tocar = compilación normal
(`65bc470dfc01463a` = `65bc470dfc01463a`, medido sobre el estado que tenía entonces
el fichero).

## 9. Incidente de concurrencia, y lo que queda para el jefe

**`QuickGame.cpp` lo reescribía a la vez el agente `resto66`**, con un script que
**vuelca el fichero ENTERO desde su propia copia** (`scratchpad/resto66/omt.py`).
Consecuencias, medidas por fecha y tamaño:

- Mi primera edición del bloque (10:03:25) desapareció antes de compilarse: las
  medidas q1/q2/q3 de esa hora **no valen** y se repitieron en sombra. Se detectó
  porque el DWARF de q1 no tenía `maximum_performance`, pese a dar «IDÉNTICO».
- Mi «restaurar» de las 10:14 escribió **su** estado de las 10:04:49. Su último `.o`
  es de las 10:04:50 y a las 10:32 volvió a escribir el fichero con sus notas, así que
  no consta que se perdiera nada suyo. **Pero no lo puedo garantizar: conviene que lo
  confirme `resto66`.**
- Desde entonces **no he tocado `QuickGame.cpp`**. El bloque de `CreateCars` se
  entrega como **parche**: `scratchpad/juego66/r66_createcars_bloque_muerto.patch`
  (LF; el fichero es CRLF). Se aplica limpio sobre el fichero actual (`patch
  --dry-run`) y está sellado en §8. Hay que aplicarlo **cuando `resto66` cierre**.
- Nota propuesta para escribir junto al pin `collisionPos` (`QuickGame.cpp:294`):

```
// r66 (juego-oculto): este pin NO tapa codigo que falta. La funcion SI tenia codigo
// muerto sin escribir (bloque maximum_performance/pp1/pp2, rango 0x80281D40 -> 0x80281D40,
// lineas 429-443 del original amontonadas en esa instruccion): escrito, zSim identico
// CON el pin; SIN el pin da 1a1bd1b50dca4c67 con o sin el bloque (4 filas). Lo que
// hace el pin es otra cosa: el objetivo pone la direccion del temporal WCollisionMgr
// en r3 (addi r3,r1,0x540) y nosotros en r11 (addi r11 / mr r3,r11).
```

## 10. Trampas nuevas (propuestas para `docs/TRAMPAS.md`)

1. **Una compilación en sombra con otro NOMBRE de fichero cambia `.rodata`.** zCamera
   usa `__FILE__` (el `new` de depuración): `CDActionDrive.barrera.cpp` contra
   `CDActionDrive.e1.cpp` dio `.rodata` 9904 B contra 9872 y un «DISTINTO» con `.text`
   idéntico. Sombra siempre por la MISMA ruta, una variante detrás de otra.
2. **Un rango de bloque cero no es código muerto** si no hay una llamada dentro
   (§6.1).
3. **Un agente que restaura desde su copia de fichero entero borra en silencio las
   ediciones de otro.** El «IDÉNTICO» que sale después es verdad, pero de otro
   fichero. Control barato: después de medir, busca en el DWARF del `.o` algo que
   sólo exista en tu edición.

## 11. Propuestas de configuración

Ninguna. No se ha tocado `configure.py` ni `config/GOWE69/*`.

## 12. Veredicto

**CUMPLIDO en lo que pedía el lote, y la cifra es pequeña.** Entre los 97 andamios
de juego hay **un** caso de andamio que tapa código que falta. Está escrito y el
andamio retirado, con zCamera idéntico sección a sección y el control negativo
fallando como debe. La búsqueda extendida (142 sitios, tres detectores) encontró
**dos funciones más con código muerto sin escribir**, pero ahí está MEDIDO que el
andamio es independiente. Una se entrega escrita y sellada como parche; la otra no se
puede escribir sin inventar. La conclusión útil para las siguientes rondas: **el
código que falta no se esconde detrás de los andamios**. Los 190 andamios que quedan
son reparto, ámbito o locales de más, y no decompilación pendiente disfrazada.

## Apéndice — los 142 sitios

Clase (§7) · sitio · tipo · función · unidad · veredicto de `regmap` sobre `.o` de scratch.

| clase | sitio | tipo | función | unidad | regmap |
|---|---|---|---|---|---|
| E | `EcstasyE.cpp:3384` | bar__asm__ | `eViewPlatInterface::Render` | zEcstasy | IDENTICO |
| D | `EcstasyEx.cpp:949` | bar__asm__ | `GenerateHorizonFogDisplayList` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| C | `TextureInfoPlat.cpp:103` | bar_asm | `TextureInfoPlatInterface::LockPalette` | zPlatform | ESTRUCTURA ambito=2 FALTA=b0/b0[j] |
| E | `eLightE.cpp:609` | pin | `eLightMaterialPlatInterface::UpdatePlatInfo` | zEcstasy | IDENTICO |
| E | `eLightE.cpp:643` | bar__asm__ | `eLightMaterialPlatInterface::UpdatePlatInfo` | zEcstasy | IDENTICO |
| E | `eLightE.cpp:644` | bar__asm__ | `eLightMaterialPlatInterface::UpdatePlatInfo` | zEcstasy | IDENTICO |
| E | `eLightE.cpp:645` | bar__asm__ | `eLightMaterialPlatInterface::UpdatePlatInfo` | zEcstasy | IDENTICO |
| E | `eLightE.cpp:732` | bar__asm__ | `eLightMaterialPlatInterface::UpdatePlatInfo` | zEcstasy | IDENTICO |
| E | `eMathE.cpp:265` | bar__asm__ | `eProject` | zEcstasy | PERMUTACION/REPARTO |
| D | `JoyE.cpp:303` | pin | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `JoyE.cpp:304` | bar_asm | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `JoyE.cpp:325` | bar_asm | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `JoyE.cpp:326` | bar_asm | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `JoyE.cpp:327` | bar_asm | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `JoyE.cpp:328` | bar_asm | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `JoyE.cpp:329` | bar_asm | `ActualReadJoystickData` | zPlatform | ESTRUCTURA solo_nuestras=5 SOBRA=b0/b0/b0/b0/b0[data];b0/b0/b0/b0/b1[data];b0/b0/b0/b0/b2[data];b0/b0/b0/b0/b3[data] |
| D | `LGWheels.cpp:427` | pin | `LGWheels::PlayFrontalCollisionForce` | zPlatform | ESTRUCTURA solo_nuestras=3 SOBRA=b0[playing, periodic] |
| D | `AIPursuit.cpp:1072` | bar__asm__ | `AIPursuit::AssignClosestOffsets` | zAI | ESTRUCTURA solo_nuestras=1 SOBRA=b2[next] |
| E | `AnimEntity_WorldEntity.cpp:169` | bar_asm | `CWorldAnimEntity::Init` | zAnim | IDENTICO |
| A | `CDActionDrive.cpp:422` | bar__asm__ | `CDActionDrive::Update` | zCamera | ESTRUCTURA solo_orig=6 FALTA=b2[iperp, playerList];b6/b0[old_pov] |
| C | `ICEManager.cpp:1008` | bar_asm | `ICEManager::LoadCameraShakes` | zCamera | ESTRUCTURA solo_nuestras=1 ambito=1 FALTA=b0/b0/b0[i] |
| C | `ICEManager.cpp:1010` | bar_asm | `ICEManager::LoadCameraShakes` | zCamera | ESTRUCTURA solo_nuestras=1 ambito=1 FALTA=b0/b0/b0[i] |
| B | `TrackCop.cpp:345` | bar_asm | `TrackCopCameraMover::Update` | zCamera | ESTRUCTURA solo_nuestras=1 |
| B | `TrackCop.cpp:353` | bar_asm | `TrackCopCameraMover::Update` | zCamera | ESTRUCTURA solo_nuestras=1 |
| D | `MemoryPoolManager.cpp:103` | pin | `MemoryPoolManager::NewBlockAux` | zEagl4Anim | ESTRUCTURA solo_nuestras=3 SOBRA=b0[blockStorage, poolFree, nextPoolFree] |
| D | `MemoryPoolManager.cpp:114` | pin | `MemoryPoolManager::NewBlockAux` | zEagl4Anim | ESTRUCTURA solo_nuestras=3 SOBRA=b0[blockStorage, poolFree, nextPoolFree] |
| D | `MemoryPoolManager.cpp:125` | pin | `MemoryPoolManager::NewBlockAux` | zEagl4Anim | ESTRUCTURA solo_nuestras=3 SOBRA=b0[blockStorage, poolFree, nextPoolFree] |
| D | `RawStateChan.cpp:297` | pin | `FnRawStateChan::FindTime` | zEagl4Anim | ESTRUCTURA solo_nuestras=1 SOBRA=b0[keyOffset] |
| D | `CARSFX_PreColWoosh.cpp:95` | bar__asm__ | `CARSFX_PreColWoosh::MsgBarrier` | zEAXSound2 | ESTRUCTURA solo_nuestras=2 SOBRA=b0[fadeOut, interpolationTime] |
| D | `CARSFX_PreColWoosh.cpp:97` | bar__asm__ | `CARSFX_PreColWoosh::MsgBarrier` | zEAXSound2 | ESTRUCTURA solo_nuestras=2 SOBRA=b0[fadeOut, interpolationTime] |
| D | `CARSFX_PreColWoosh.cpp:98` | pin | `CARSFX_PreColWoosh::MsgBarrier` | zEAXSound2 | ESTRUCTURA solo_nuestras=2 SOBRA=b0[fadeOut, interpolationTime] |
| D | `CARSFX_PreColWoosh.cpp:99` | bar__asm__ | `CARSFX_PreColWoosh::MsgBarrier` | zEAXSound2 | ESTRUCTURA solo_nuestras=2 SOBRA=b0[fadeOut, interpolationTime] |
| D | `CARSFX_Roadnoise.cpp:489` | pin | `CARSFX_RoadNoise::GenerateRoadNoise` | zEAXSound2 | ESTRUCTURA solo_nuestras=1 |
| C | `SFXObj_Collision.cpp:59` | pin | `SFXObj_Collision::InitSFX` | zEAXSound2 | ESTRUCTURA solo_nuestras=1 FALTA=b1[nada] |
| D | `SFXObj_Pathfinder.cpp:593` | pin | `SFXObj_PFEATrax::TestToLicensed` | zEAXSound2 | ESTRUCTURA solo_nuestras=1 SOBRA=b3[mode] |
| D | `NFSMixMapState.cpp:173` | pin | `NFSMixMapState::CreateSubMixChannels` | zEAXSound2 | ESTRUCTURA solo_nuestras=4 SOBRA=b0/b0/b0/b0[nOffset] |
| D | `NFSMixMapState.cpp:185` | pin | `NFSMixMapState::CreateSubMixChannels` | zEAXSound2 | ESTRUCTURA solo_nuestras=4 SOBRA=b0/b0/b0/b0[nOffset] |
| D | `NFSMixMapState.cpp:202` | pin | `NFSMixMapState::CreateSubMixChannels` | zEAXSound2 | ESTRUCTURA solo_nuestras=4 SOBRA=b0/b0/b0/b0[nOffset] |
| F | `EAXAemsManager.cpp:455` | bar_asm | `EAXAemsManager::SetupNextLoad` | zEAXSound | NO-ORIGINAL no encuentro "EAXAemsManager::SetupNextLoad" en el volcado original. |
| F | `EAXAemsManager.cpp:456` | bar_asm | `EAXAemsManager::SetupNextLoad` | zEAXSound | NO-ORIGINAL no encuentro "EAXAemsManager::SetupNextLoad" en el volcado original. |
| F | `EAXAemsManager.cpp:466` | bar_asm | `EAXAemsManager::SetupNextLoad` | zEAXSound | NO-ORIGINAL no encuentro "EAXAemsManager::SetupNextLoad" en el volcado original. |
| F | `EAXAemsManager.cpp:467` | bar_asm | `EAXAemsManager::SetupNextLoad` | zEAXSound | NO-ORIGINAL no encuentro "EAXAemsManager::SetupNextLoad" en el volcado original. |
| F | `EAXAemsManager.cpp:890` | bar__asm__ | `EAXAemsManager::ResolveCurrentDataMemory` | zEAXSound | NO-ORIGINAL no encuentro "EAXAemsManager::ResolveCurrentDataMemory" en el volcado original. |
| F | `EAXAemsManager.cpp:906` | bar__asm__ | `EAXAemsManager::ResolveCurrentDataMemory` | zEAXSound | NO-ORIGINAL no encuentro "EAXAemsManager::ResolveCurrentDataMemory" en el volcado original. |
| D | `EAXAemsManager.cpp:1025` | bar_asm | `EAXAemsManager::RegisterSlots` | zEAXSound | ESTRUCTURA solo_nuestras=2 |
| D | `EaxSoundTypes.cpp:263` | pin | `ScheduledSpeechEvent::sort_nested_priority` | zEAXSound | ESTRUCTURA solo_nuestras=2 |
| D | `EaxSoundTypes.cpp:264` | bar_asm | `ScheduledSpeechEvent::sort_nested_priority` | zEAXSound | ESTRUCTURA solo_nuestras=2 |
| D | `ginsudata.cpp:156` | pin | `GinsuSynthData::BindToData` | zEAXSound2 | ESTRUCTURA solo_nuestras=1 |
| F | `ENVIRO_AEMS.h:333` | bar__asm__ | `None` | zEAXSound2 | SIN-REGMAP |
| D | `STICH_PlayBack.cpp:191` | bar__asm__ | `cStichWrapper::Play` | zEAXSound | ESTRUCTURA solo_nuestras=1 |
| E | `stream.cpp:474` | pin | `startnextrequest` | zEAXSound | IDENTICO |
| F | `SFXCTL_3DObjPos.cpp:37` | bar_asm | `bSqrtNoCJ` | zEAXSound | NO-ORIGINAL no encuentro "bSqrtNoCJ" en el volcado original. |
| F | `SFXCTL_3DObjPos.cpp:40` | bar_asm | `bSqrtNoCJ` | zEAXSound | NO-ORIGINAL no encuentro "bSqrtNoCJ" en el volcado original. |
| E | `SFXCTL_3DObjPos.cpp:134` | bar_asm | `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | zEAXSound | PERMUTACION/REPARTO |
| E | `SFXCTL_3DObjPos.cpp:135` | bar_asm | `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | zEAXSound | PERMUTACION/REPARTO |
| E | `SFXCTL_3DObjPos.cpp:155` | bar_asm | `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | zEAXSound | PERMUTACION/REPARTO |
| E | `SFXCTL_3DObjPos.cpp:156` | bar_asm | `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | zEAXSound | PERMUTACION/REPARTO |
| D | `SFXCTL_AccelTrans.cpp:90` | pin | `SFXCTL_AccelTrans::UpdateParams` | zEAXSound | ESTRUCTURA solo_nuestras=2 |
| D | `SFXCTL_AccelTrans.cpp:91` | bar_asm | `SFXCTL_AccelTrans::UpdateParams` | zEAXSound | ESTRUCTURA solo_nuestras=2 |
| D | `SFXCTL_NISReving.cpp:346` | bar_asm | `SFXCTL_Physics::UpdateNIS` | zEAXSound | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1347` | pin | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1348` | bar__asm__ | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1487` | pin | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1515` | pin | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1517` | bar_asm | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1551` | bar_asm | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| D | `EmitterSystem.cpp:1564` | bar__asm__ | `EmitterSystem::Render` | zEcstasy | ESTRUCTURA solo_nuestras=1 |
| C | `eSolid.cpp:352` | pin | `eSolid::GetPostionMarker` | zEcstasy | ESTRUCTURA ambito=1 SOBRA=b0[last_marker] |
| C | `eSolid.cpp:376` | pin | `eSolid::GetPostionMarker` | zEcstasy | ESTRUCTURA ambito=1 SOBRA=b0[last_marker] |
| C | `FEPackage.cpp:235` | bar_asm | `FEPackage::IssueScriptMessages` | zFEng | ESTRUCTURA solo_nuestras=4 ambito=1 FALTA=b0[pTargetPtr] SOBRA=b1/b1[savedEvent, pTargetPtr] |
| E | `FEngine.cpp:327` | bar_asm | `FEngine::Update` | zFEng | PERMUTACION/REPARTO |
| D | `FEngine.cpp:576` | pin | `FEngine::ProcessPadsForPackage` | zFEng | ESTRUCTURA solo_nuestras=1 SOBRA=b1/b0[padMask] |
| D | `FEngInterface.cpp:53` | pin | `cFEng::PushErrorPackage` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0/b0[wasPaused];b1/b0[wasPaused] |
| D | `FEngInterface.cpp:54` | bar_asm | `cFEng::PushErrorPackage` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0/b0[wasPaused];b1/b0[wasPaused] |
| D | `FEngInterface.cpp:74` | pin | `cFEng::PushErrorPackage` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0/b0[wasPaused];b1/b0[wasPaused] |
| D | `FEngInterface.cpp:75` | bar_asm | `cFEng::PushErrorPackage` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0/b0[wasPaused];b1/b0[wasPaused] |
| D | `FEngInterfaceFEObjects.cpp:525` | pin | `FEngSetScaleX` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:530` | pin | `FEngSetScaleX` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:535` | pin | `FEngSetScaleX` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:536` | bar_asm | `FEngSetScaleX` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:566` | pin | `FEngSetScaleY` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:570` | pin | `FEngSetScaleY` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:574` | pin | `FEngSetScaleY` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `FEngInterfaceFEObjects.cpp:575` | bar_asm | `FEngSetScaleY` | zFe | ESTRUCTURA solo_nuestras=2 SOBRA=b0[initialScale] |
| D | `MemoryCardCallbacks.cpp:331` | bar__asm__ | `MemcardCallbacks::FoundEntry` | zFe | ESTRUCTURA solo_nuestras=2 |
| D | `MemoryCardCallbacks.cpp:332` | bar__asm__ | `MemcardCallbacks::FoundEntry` | zFe | ESTRUCTURA solo_nuestras=2 |
| D | `MemoryCardCallbacks.cpp:333` | bar__asm__ | `MemcardCallbacks::FoundEntry` | zFe | ESTRUCTURA solo_nuestras=2 |
| D | `feArrayScrollerMenu.cpp:209` | pin | `ArrayScroller::ScrollVer` | zFe2 | ESTRUCTURA solo_nuestras=1 |
| F | `FEpkg_MU_Keyboard.cpp:518` | pin | `FEKeyboard::ToggleCapsLock` | zFe2 | NO-ORIGINAL no encuentro "FEKeyboard::ToggleCapsLock" en el volcado original. |
| F | `FEpkg_MU_Keyboard.cpp:524` | bar__asm__ | `FEKeyboard::ToggleCapsLock` | zFe2 | NO-ORIGINAL no encuentro "FEKeyboard::ToggleCapsLock" en el volcado original. |
| D | `CarCustomize.cpp:2024` | bar__asm__ | `CustomizeMain::NotificationMessage` | zFeOverlay | ESTRUCTURA solo_nuestras=3 SOBRA=b2/b0[fe, pkg] |
| D | `CarCustomize.cpp:2052` | bar__asm__ | `CustomizeMain::NotificationMessage` | zFeOverlay | ESTRUCTURA solo_nuestras=3 SOBRA=b2/b0[fe, pkg] |
| D | `CarCustomize.cpp:2054` | bar__asm__ | `CustomizeMain::NotificationMessage` | zFeOverlay | ESTRUCTURA solo_nuestras=3 SOBRA=b2/b0[fe, pkg] |
| E | `CarCustomize.cpp:2727` | pin | `CustomizeParts::Setup` | zFeOverlay | IDENTICO |
| D | `SubTitle.cpp:105` | pin | `SubTitler::GetElapsedTime` | zFe | ESTRUCTURA solo_nuestras=1 SOBRA=b0[t] |
| D | `SubTitle.cpp:119` | bar__asm__ | `SubTitler::GetElapsedTime` | zFe | ESTRUCTURA solo_nuestras=1 SOBRA=b0[t] |
| E | `GManager.cpp:997` | pin | `GManager::AllocateInstanceMap` | zGameplay | IDENTICO |
| E | `GManager.cpp:1774` | pin | `GManager::GetStrippedNameKey` | zGameplay | IDENTICO |
| F | `GRaceDatabase.cpp:55` | bar__asm__ | `PackedDecimal::PackedDecimal` | zGameplay | NO-ORIGINAL no encuentro "PackedDecimal::PackedDecimal" en el volcado original. |
| D | `GRaceDatabase.cpp:1067` | bar__asm__ | `GRaceParameters::GenerateIndex` | zGameplay | ESTRUCTURA solo_nuestras=4 |
| D | `GTrigger.cpp:87` | bar_asm | `GTrigger::GTrigger` | zGameplay | ESTRUCTURA solo_nuestras=5 SOBRA=b1[halfWidth, halfLength];b2[halfWidth] |
| C | `EPlayRaceNIS.cpp:76` | bar__asm__ | `EPlayRaceNIS::EPlayRaceNIS` | zMain | ESTRUCTURA solo_orig=3 solo_nuestras=5 ambito=8 FALTA=b0/b0/b0/b1/b0[ipursuit];b0/b0/b0/b1/b1/b0/b0/b0/b0/b0/b0[car_fwd, cop_dir, dot, minTresh] SOBRA=b0/b0/b0/b0[ipursuit] |
| E | `SteeringWheelDevice.cpp:290` | pin | `SteeringWheelDevice::ConvertWheelRotation` | zMain | PERMUTACION/REPARTO |
| E | `SteeringWheelDevice.cpp:300` | pin | `SteeringWheelDevice::ConvertWheelRotation` | zMain | PERMUTACION/REPARTO |
| E | `InputDeviceGC.cpp:514` | bar__asm__ | `GameDevice::PollDevice` | zMain | IDENTICO |
| F | `ScratchPtr.h:56` | bar__asm__ | `ScratchPtr::_Alloc` |  | SIN-REGMAP |
| E | `PhysicsObject.cpp:69` | pin | `PhysicsObject::PhysicsObject` | zPhysics | IDENTICO |
| E | `Geometry.cpp:425` | bar__asm__ | `Geometry::SphereVsBox` | zDynamics | IDENTICO |
| B | `QuickGame.cpp:294` | pin | `QuickGame::CreateCars` | zSim | ESTRUCTURA solo_orig=4 solo_nuestras=1 ambito=1 FALTA=b1[nada];b2[maximum_performance, pp1] |
| B | `QuickGame.cpp:295` | bar_asm | `QuickGame::CreateCars` | zSim | ESTRUCTURA solo_orig=4 solo_nuestras=1 ambito=1 FALTA=b1[nada];b2[maximum_performance, pp1] |
| D | `QuickGame.cpp:422` | pin | `QuickGame::OnManageTime` | zSim | ESTRUCTURA solo_nuestras=4 SOBRA=b0[phantom] |
| D | `QuickGame.cpp:438` | pin | `QuickGame::OnManageTime` | zSim | ESTRUCTURA solo_nuestras=4 SOBRA=b0[phantom] |
| D | `QuickGame.cpp:450` | bar_asm | `QuickGame::OnManageTime` | zSim | ESTRUCTURA solo_nuestras=4 SOBRA=b0[phantom] |
| E | `EAXDispatch.cpp:141` | bar__asm__ | `EAXDispatch::PursuitEscalation` | zSpeech | IDENTICO |
| E | `EAXDispatch.cpp:182` | bar__asm__ | `EAXDispatch::BreakAway` | zSpeech | IDENTICO |
| D | `CarRender.cpp:2371` | pin | `CarRenderInfo::CreateCarLightFlares` | zWorld | ESTRUCTURA solo_nuestras=2 |
| D | `CarRender.cpp:2382` | pin | `CarRenderInfo::CreateCarLightFlares` | zWorld | ESTRUCTURA solo_nuestras=2 |
| D | `CarRender.cpp:2393` | pin | `CarRenderInfo::CreateCarLightFlares` | zWorld | ESTRUCTURA solo_nuestras=2 |
| D | `WRoadNetwork.cpp:1982` | pin | `WRoadNav::HolePunchAvoidables` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:2178` | bar_asm | `WRoadNav::HolePunchAvoidables` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:2957` | pin | `WRoadNav::InitAtSegment` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:2958` | bar_asm | `WRoadNav::InitAtSegment` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:2968` | bar_asm | `WRoadNav::InitAtSegment` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:3811` | pin | `WRoadNav::CookieTrailCurvature` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:3812` | bar_asm | `WRoadNav::CookieTrailCurvature` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| D | `WRoadNetwork.cpp:3813` | pin | `WRoadNav::CookieTrailCurvature` | zWorld2 | ESTRUCTURA solo_nuestras=1 |
| E | `WTrigger.cpp:429` | bar__asm__ | `WTriggerManager::CheckCollideSRB` | zWorld2 | IDENTICO |
| E | `TrackPath.cpp:193` | pin | `TrackPathManager::FindZone` | zTrack | IDENTICO |
| E | `TrackPath.cpp:227` | bar_asm | `TrackPathManager::FindZone` | zTrack | IDENTICO |
| E | `TrackStreamer.cpp:1526` | pin | `TrackStreamer::GetPredictedZone` | zTrack | IDENTICO |
| E | `TrackStreamer.cpp:1572` | bar_asm | `TrackStreamer::GetPredictedZone` | zTrack | IDENTICO |
| C | `TrackStreamer.cpp:2180` | pin | `TrackStreamer::HandleLoading` | zTrack | ESTRUCTURA solo_orig=1 solo_nuestras=1 FALTA=b0/b0/b0[section] |
| C | `TrackStreamer.cpp:2190` | pin | `TrackStreamer::HandleLoading` | zTrack | ESTRUCTURA solo_orig=1 solo_nuestras=1 FALTA=b0/b0/b0[section] |
| D | `WeatherMan.cpp:177` | pin | `RegionQuery::CalculateRegionInfo` | zTrack | ESTRUCTURA solo_nuestras=2 |
| D | `WeatherMan.cpp:179` | bar_asm | `RegionQuery::CalculateRegionInfo` | zTrack | ESTRUCTURA solo_nuestras=2 |
| D | `WeatherMan.cpp:182` | pin | `RegionQuery::CalculateRegionInfo` | zTrack | ESTRUCTURA solo_nuestras=2 |
| F | `Strings.cpp:161` | pin | `bStrNCmp` | zBWare | NO-ORIGINAL no encuentro "bStrNCmp" en el volcado original. |
| F | `Strings.cpp:162` | pin | `bStrNCmp` | zBWare | NO-ORIGINAL no encuentro "bStrNCmp" en el volcado original. |
| D | `Strings.cpp:258` | pin | `bStrNICmp` | zBWare | ESTRUCTURA solo_nuestras=3 SOBRA=b0[c1, c2, c1wide] |
| D | `Strings.cpp:259` | pin | `bStrNICmp` | zBWare | ESTRUCTURA solo_nuestras=3 SOBRA=b0[c1, c2, c1wide] |
| D | `Strings.cpp:260` | bar_asm | `bStrNICmp` | zBWare | ESTRUCTURA solo_nuestras=3 SOBRA=b0[c1, c2, c1wide] |
| D | `Strings.cpp:261` | pin | `bStrNICmp` | zBWare | ESTRUCTURA solo_nuestras=3 SOBRA=b0[c1, c2, c1wide] |
