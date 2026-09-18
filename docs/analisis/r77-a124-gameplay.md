# r77 — La cascada de `EA_BUILD_A124`: quién usa los miembros que la alpha no tiene

Lote de Gameplay + Sim. No es trabajo de match: es que `ee-gcc` acepte el código.
Todo lo de aquí está medido con el compilador real de PS2
(`build/compilers/PS2/ee-gcc2.9-991111`), sin tocar `build.ninja` ni `build/`.

## 1. Qué se guardó y por qué

El patrón ya estaba a medias: varios **miembros** de `GRaceStatus.h` y
`WorldModel.hpp` estaban bajo `#ifndef EA_BUILD_A124`, y en la r77 se guardaron
sus **accesores**. Faltaba la tercera capa, **quién los usa**. Eso es todo lo de
abajo.

### `src/Speed/Indep/Src/Gameplay/GRaceStatus.cpp` — 12 sitios

Nota de cabecera puesta una vez, en el primero (el constructor).

| Sitio | Qué se guarda | Por qué sigue teniendo sentido sin ello |
|---|---|---|
| constructor | las tres puestas a cero de `mRefreshBinAfterRace`, `mWarpWhenInFreeRoam` y `mCaluclatedAdaptiveGain` | miembros que no existen |
| `Update` | el `if` entero del refresco de bin | sin el flag la condición nunca se cumple |
| `Update` | el `if` entero del warp al volver a free roam | ídem |
| `Update` | el `if (isChallenge)` completo **con su local** `bool isChallenge` | es sólo la pausa/reanudación del reloj maestro durante el cooldown; el reloj se arranca y se para en `GRaceStatus::StartMasterTimer`/`StopMasterTimer`, así que no queda muerto |
| `SetRacing` | `mCaluclatedAdaptiveGain = false;` | — |
| `UpdateAdaptiveDifficulty` | el `if (mCaluclatedAdaptiveGain) return;` y, aparte, el `mCaluclatedAdaptiveGain = true;` de dentro del `if (update)` | son la guarda y la marca de "esto ya se calculó una vez"; sin el flag, la alpha simplemente no tiene ese "una sola vez". El resto del `if (update)` (fijar la dificultad) se conserva |
| `GRacerInfo::IsBehind` | el `if (mDNF && other.mDNF)` entero | es un desempate más de la cadena; los demás y el `return` final siguen |
| `GRacerInfo::UpdateSplits` | **el cuerpo entero**, la función queda vacía | sin parciales no hay nada que actualizar |
| `FinalizeRaceStats` | sólo `mDNF = true;` dentro del `if` del drag | el `adjustedTime = 0.0f;` que lo acompaña es un efecto aparte y se conserva |
| `FinalizeRaceStats` | el `if (mGameCharacter != NULL)` que rellena `mSplitTimes`/`mSplitRankings`, con sus locales `comparePct`, `thresholds` e `i` | todo el bloque existe sólo para los parciales |
| `FinalizeRaceStats` | el `if (!mDNF)` final, **con `#else`** que deja `SetTime` + `FinishRace()` sin condición | ver la duda 1 |

### `src/Speed/Indep/Src/Gameplay/GManager.cpp` — 3 llamadas

- `UpdatePursuit`: la llamada a `SetPlayerPursuitInCooldown(inCooldown)`. La local
  `inCooldown` **no** queda huérfana: se sigue usando 17 líneas más abajo para
  `mHidingSpotIconsShown` y `mPursuitBreakerIconsShown`.
- `StartCareerActivities`: el `if (previousBinRace && raceContext == kRaceContext_Career)`
  entero, porque su único cuerpo era el `SetRefreshBinAfterRace(true)`.
  `previousBinRace` sigue usada después.
- `StartRaceFromInGame`: sólo la llamada; el `SuspendAllBinActivities()` que la
  precede se queda.

### `src/Speed/Indep/Src/Gameplay/GIcon.cpp` — 1 llamada

`CreateGeometry`: `model->SetCastsShadow(0)` (miembro `mCastsShadow` de
`WorldModel.hpp`). El `SetEnabledFlag(false)` de al lado se queda.

### `src/Speed/Indep/Src/Sim/Activities/NISActivity.cpp` — 3 sitios

Entra por la regla del lote: el mismo patrón, miembro guardado usado sin guardar.
`mBlackListNIS` y `mNonSkipableNIS` ya estaban bajo `#ifndef EA_BUILD_A124` en la
declaración **y en la lista de inicialización**; faltaban los usos.

- `Load`: los dos `if` que marcan el NIS como no saltable y como de blacklist,
  enteros.
- El árbol de `SetSoundControlState`: se guarda la cadena completa y se repite en
  el `#else` la misma cadena **sin la rama de blacklist**. Es el idioma que ese
  mismo fichero ya usa unas líneas más abajo, en `SkipOverNIS`
  (`#else / if (true) { // TODO PS2`).

### `src/Speed/Indep/Src/Sim/Entities/LocalPlayer.cpp` — 1 sitio

El `if` que resetea el coste al cambiar de persecución, porque usa `mLastPursuit`.
**Aquí la guarda no es `EA_BUILD_A124`**: el miembro está declarado en
`LocalPlayer.hpp:155` bajo `#ifndef EA_PLATFORM_PLAYSTATION2`, y se ha usado la
misma que su declaración. Ver la duda 4.

## 2. Cuántas unidades de PS2 pasan a compilar

**Aviso sobre la medida.** Durante esta tanda hubo otro trabajo tocando el árbol
en paralelo (`UVectorMath.cpp`, `AIVehicle.cpp`, `EAXSound.cpp`, `SoundConn.cpp`,
`LuaGameHooks.cpp`, `FEPkg_PostRace.hpp` y luego todo el Frontend). El barrido
global mezcla las dos aportaciones, así que va por separado.

| | unidades que compilan |
|---|---|
| barrido al empezar el lote | **20 de 34** |
| barrido al cerrarlo | **25 de 34** |

Y **mi aportación, aislada y medida**: se compiló cada unidad dos veces, la
segunda con un directorio sombra (`-I`) que repone la versión anterior de mis
cinco ficheros, sin tocar el árbol:

| unidad | sin mis guardas | con mis guardas |
|---|---|---|
| `zGameplay` | FALLA (13 errores) | **COMPILA** |
| `zSim` | FALLA (6 errores) | **COMPILA** |
| `zAI` | COMPILA | COMPILA |
| `zFoundation` | COMPILA | COMPILA |

Es decir: **este lote desbloquea 2 unidades, `zGameplay` y `zSim`**. Las otras
tres del salto global (`zAI`, `zFoundation`, `zFEng`) son del trabajo paralelo.
Mis cinco ficheros sólo entran en esos dos SourceLists, así que 2 es el techo.

## 3. GameCube no se mueve — y está probado dos veces

```
python -m ninja build/GOWE69/ok   ->  build/GOWE69/main.dol: OK
sha1sum build/GOWE69/main.dol     ->  9619ba57c9919f95f7f2ac951a2166a3517f91e3
```

Esa comprobación, por sí sola, **no prueba nada de este lote**: `zGameplay` y
`zSim` enlazan desde `build/GOWE69/obj/...` (los `.o` extraídos del original), no
desde `src/`, así que el DOL no llega a ver estos fuentes. Y `ninja` decía "no
work to do" con los `.o` de `src/` obsoletos: es otra cara de *el grafo de build
es mudo*.

La prueba de verdad es esta: se guardaron los `.o` de GameCube anteriores al
cambio, se forzó la recompilación de los dos, y se compararon **sección a
sección** ignorando las de depuración:

```
zGameplay.o: 0 secciones sin-debug distintas (de 13)
zSim.o:      0 secciones sin-debug distintas (de 13)
```

`.text`, `.data`, `.rodata`, `.bss`, símbolos y reubicaciones **idénticos byte a
byte**. Lo único que cambia en el `.o` es la información de líneas, porque las
directivas del preprocesador desplazan el fuente. Para GameCube este lote es
demostrablemente neutro.

## 4. Lo que me hizo dudar (no lo decido yo)

1. **`FinalizeRaceStats`, el `if (!mDNF)` final.** Guardarlo entero dejaría la
   función sin llamar nunca a `FinishRace()`: nadie terminaría una carrera. Por
   eso se usó `#ifndef/#else`, y en la alpha `SetTime` + `FinishRace()` van sin
   condición. Es lo único con sentido, pero **no sé si la alpha 124 llamaba a
   `FinishRace` incondicionalmente** o si esa función era otra cosa allí.
2. **`GRacerInfo::UpdateSplits`.** Se guardó el cuerpo y la función queda vacía en
   la alpha. Lo coherente con "la alpha no tiene parciales" sería que la función
   no existiese, pero eso obliga a guardar también su declaración en
   `GRaceStatus.h:95` y su llamada en `GRaceStatus.cpp:311`, y la cabecera no era
   de este lote. **Si alguien decide que la función no existía, es un cambio de
   tres sitios y está localizado.**
3. **El reloj maestro y el cooldown.** Se guardó el `if (isChallenge)` completo.
   Tengo comprobado que `mRaceMasterTimer` se arranca y para en
   `StartMasterTimer`/`StopMasterTimer`, así que no queda muerto, pero **no sé si
   la alpha tenía ahí alguna otra lógica** en lugar de nada.
4. **Incoherencia en `LocalPlayer` que no he tocado.** El miembro `mLastPursuit`
   está bajo `#ifndef EA_PLATFORM_PLAYSTATION2` (cabecera) y su inicialización en
   el constructor, bajo `#ifndef EA_BUILD_A124` (`LocalPlayer.cpp:68`). Hoy da
   igual porque PS2 define los dos, pero un build de PS2 que no fuese la alpha 124
   no compilaría. **Alguien debería unificarlas**; para el uso nuevo se eligió la
   guarda del miembro.

## 5. Lo que NO entra, y por qué

**`EmitterSystem.cpp` no es este patrón.** Falla en `HandleFXTriggers`
(línea 1890) con `ePlayerList` y `PLAYER_LOCAL` sin declarar y `struct IPlayer`
incompleto. Ese enum **existe y no está guardado** (`IPlayer.h:37-42`): el
problema es que la unidad no ve `IPlayer.h` y sólo tiene la declaración adelantada
de `ISimable.h:41`. Es un include que falta, no un miembro guardado. Se deja.
`zEcstasy` además falla por `zEcstasy.cpp:430` (syntax error) y por
`eDynamicLightPack` incompleto en `eLight.cpp:244`: tres causas distintas, ninguna
de ellas la de este lote.

## 6. Nota de procedimiento

No hice commit (la instrucción del lote lo prohibía), pero **otro proceso
commiteó mis cinco ficheros** mientras trabajaba: los tres de Gameplay en
`09840c38` y los dos de Sim en `d67041f1`, ambos mezclados con trabajo ajeno. Por
eso un `git checkout --` de "reversión de control" no revirtió nada y estuvo a
punto de dar una medida falsa; la medida buena es la del directorio sombra de la
sección 2. **Si alguien mide el efecto de un lote comparando contra el árbol,
conviene que compruebe antes que sus cambios siguen sin commitear.**
