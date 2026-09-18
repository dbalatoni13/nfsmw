# r50 — `.rodata`: los bytes que faltan son literales MUERTOS, y hay dos palancas nuevas

Territorio: el deficit de `.rodata` viva de las 22 unidades que `linkdelta.py`
deja con el `.text` a cero, y las «17 funciones que `zFe2` no emite».

## Resultado en una linea

**Los bytes que faltan son literales de cadena MUERTOS**, y se parten en dos
familias que se arreglan al reves:

| familia | B (23 SourceLists) | herramienta |
|---|---:|---|
| el enlazador se los lleva (los emitimos y `keep.lst` no los nombra) | **20.464** de los que **8.440** son cadenas que el objetivo SI tiene | `keep.lst` (`scripts/deadlink.py`, nuevo) |
| **cc1plus no los emite nunca**: el objetivo los tiene y nuestro `.o` no | **22,2 kB** en 992 cadenas distintas | la guarda de cabecera, o el `asm()` anonimo |

`deadstr.py` --la herramienta que existia para la primera familia-- **infra-
reporta**: su prueba de vida no es transitiva y por eso propone **cero** entradas
nuevas en `zSpeech`, `zGameModes` y `zPhysics` mientras el enlazador se lleva 54,
20 y 46 simbolos respectivamente. Lo he arreglado en `scripts/deadlink.py`.

Y las «17 funciones de `zFe2`» **no existen**: son un artefacto del filtro
`s['size']` de `permorden.py`. Las 17 estan definidas, 16 por los `asm()` de
alias de `VehicleDB.cpp` y una por el de `FeGenericMessage.cpp`, y esos `asm()`
no llevan `.size`.

**Cero promociones**: ninguna de las 22 llega a `DOL OK`, y explico por que el
tamano de `.rodata` es un mal indicador de lo cerca que estan.

**El arbol queda como lo encontre.** Todos los ensayos estan medidos con
`keep.lst` temporales del scratchpad y las fuentes revertidas (§7).

---

## 1. El mecanismo, y por que `deadstr.py` no lo veia

El enlace lleva `-strip-unused-data`, que quita `size & ~7` bytes de cada
**simbolo** de dato muerto y deja el resto. Muerto para `ngcld` significa **no
alcanzable desde una raiz viva**, y eso es TRANSITIVO: una cadena que solo
referencia una funcion que el propio enlazador tira se va con ella.

`deadstr.py` decide lo contrario: `if k in vivos: continue`, donde `vivos` son
los indices que nombra **cualquier reubicacion del objeto**. Una cadena
referenciada desde una funcion muerta le parece viva y no la propone.

La diferencia, medida enlazando dos veces (nuestro `.o`, y nuestro `.o` con un
`keep.lst` que nombra todos nuestros simbolos de `.rodata`):

| unidad | `deadstr.py` propone | el enlazador se lleva | de eso, cadenas que el objetivo tiene |
|---|---:|---:|---:|
| `zSpeech` | **0** | 54 simbolos / 688 B | 36 / 408 B |
| `zPhysics` | **0** | 46 / 1.528 B | 20 / 296 B |
| `zGameModes` | **0** | 20 / 128 B | 0 |

Comprobado ademas que **las 691 entradas `@lc` que ya tiene `keep.lst` son
exactamente las que `deadstr.py` sabe encontrar**: pasarle `--keep` a las 23
unidades no produce ni una linea que no este ya.

### Y por que el original si las conserva

Porque en el objeto EXTRAIDO esas cadenas **no tienen simbolo propio**. `dtk`
solo pone un `lbl_` donde algo apunta, asi que un literal muerto viaja DENTRO
del `lbl_` vivo que lo precede y sobrevive entero. Ejemplo al byte, `zSpeech`:

```
lbl_80406184  44 B  "SpeechSampleMap node\0" + relleno + "MNotifySpeechStatus\0"
                    (solo el primero esta referenciado)
lbl_804061B0 308 B  "Event\0..Speech::ScheduledSpeechEvent\0..MNotifyCellCallComplete\0
                     MNotifyCellCallStarted\0.."
```

Nosotros emitimos cada una como su propio `$LCnn` y el estripado las quita una a
una. **Esa es la asimetria entera del frente.**

## 2. `zSpeech`, al byte

Deficit `rodata-624`. Ventana `0x80405E10..0x80408FB8` (12.712 B en la base).

| concepto | B |
|---|---:|
| deficit de partida | **−624** |
| cadenas muertas que emitimos y `keep.lst` no nombra (33 entradas nuevas) | **+384** → `rodata-240` |
| cadenas que el objetivo tiene y **nuestro `.o` no tiene en ninguna seccion** (10) | −162 |
| trozos nuestros que el objetivo no tiene (colas de `size & 7`) | +36 |
| resto sin explicar (constantes de coma flotante y alineacion) | −114 |

Las diez que faltan de verdad, comprobadas ausentes de **todas** las secciones de
nuestro `zSpeech.o` (solo aparecen en `.debug`):

```
AUD_tmp: Game speech index   AUD_tmp: SED_NISSFX index   Speech Cache speaker list
MForcePursuitStart           SpeechLoadCBData            MinHeatLevel
GRaceStatus                  GManager                    16.1.0        1.8.1
```

Y los seis trozos que sobran son la firma del estripado: `'uction'` (cola de
`Attrib::Gen::induction`), `'stems'` (de `EventSequencerSystems`), `'bosfx'`,
`'ssis'`, `'done'`.

## 3. La misma cuenta en las 23, medida

`scratchpad`: `lcsweep.py` (tres enlaces por unidad). `hoy` es el delta de
`.rodata` del ELF completo contra la base; `estr` los simbolos de `.rodata`
nuestros que el estripado se lleva; `cad` los que ademas son cadenas del
objetivo; `nuev` las entradas de `keep.lst` que no existen todavia.

| unidad | hoy | estr | B | cad | B | nuev | B | con las nuevas |
|---|---:|---:|---:|---:|---:|---:|---:|---|
| zAI | −1344 | 34 | 576 | 22 | 336 | 22 | 336 | rodata−1008 |
| zAnim | −1240 | 31 | 512 | 9 | 136 | 9 | 136 | rodata−1104 |
| zCamera | −1384 | 42 | 808 | 10 | 112 | 10 | 112 | rodata−1272 |
| zEAXSound | −1544 | 49 | 920 | 20 | 288 | 20 | 288 | rodata−1256 |
| zEAXSound2 | −1184 | 41 | 672 | 26 | 392 | 26 | 392 | rodata−792 |
| zEagl4Anim | −864 | 16 | 184 | 8 | 40 | 3 | 8 | rodata−856 |
| **zEcstasy** | −288 | 631 | 5688 | 205 | 2320 | 109 | 1896 | **rodata+1608** ← empeora |
| zFe | −2456 | 91 | 2040 | 51 | 424 | 51 | 424 | rodata−2032 |
| zFe2 | −4216 | 50 | 872 | 25 | 320 | 25 | 320 | rodata−3896 |
| zFeOverlay | (over) | 33 | 1224 | 7 | 120 | 7 | 120 | over−1016 |
| zGameModes | +104 | 20 | 128 | 0 | 0 | 0 | 0 | (sin cambio) |
| zGameplay | −2656 | 27 | 488 | 11 | 200 | 10 | 192 | rodata−2464 |
| **zLua** | −1648 | 52 | 616 | 32 | 392 | 31 | 384 | rodata−1264 **y text+196** |
| zMain | −1720 | 59 | 824 | 56 | 704 | 56 | 704 | rodata−1016 |
| **zMisc** | +2840 | 54 | 520 | 45 | 512 | 45 | 512 | **rodata+3352** ← empeora |
| zPhysics | −784 | 46 | 1528 | 20 | 296 | 20 | 296 | rodata−488 |
| zPhysicsBehaviors | −936 | 48 | 512 | 27 | 400 | 27 | 400 | rodata−536 |
| zPlatform | −1408 | 17 | 216 | 6 | 64 | 6 | 64 | rodata−1344 |
| **zSim** | +0 | 15 | 64 | 12 | 56 | 12 | 56 | **rodata+56** ← empeora |
| zSpeech | −624 | 54 | 688 | 36 | 408 | 33 | 384 | rodata−240 |
| zTrack | −1784 | 36 | 440 | 25 | 304 | 25 | 304 | rodata−1480 |
| zWorld | −4136 | 37 | 656 | 25 | 448 | 25 | 448 | rodata−3688 |
| zWorld2 | −808 | 22 | 288 | 15 | 168 | 15 | 168 | rodata−640 |

**No es gratis en las cuatro marcadas.** `zSim`, `zMisc` y `zEcstasy` tienen
SUPERAVIT de `.rodata`: salvarles mas dato las aleja. Y en `zLua` una de las
salvadas es una vtable que mantiene vivas sus virtuales y **resucita 196 B de
`.text`** que hoy esta a cero. Cualquiera que aplique esto tiene que pasar
`deadlink.py --medir` y mirar las tres cifras, no solo `.rodata`.

Composicion de lo que queda **despues** de aplicar esas entradas (`faltasweep.py`;
`falt` son cadenas del objetivo ausentes de nuestra ventana enlazada, `sobr` las
nuestras que el objetivo no tiene):

| unidad | tras keep | falt | B | sobr | B | resto |
|---|---:|---:|---:|---:|---:|---:|
| zAI | −1008 | 57 | 740 | 5 | 28 | −296 |
| zAnim | −1104 | 66 | 1188 | 18 | 233 | −149 |
| zCamera | −1272 | 71 | 939 | 8 | 43 | −376 |
| zEAXSound | −1256 | 71 | 1133 | 4 | 23 | −146 |
| zEAXSound2 | −792 | 49 | 596 | 6 | 32 | −228 |
| zEagl4Anim | −856 | 17 | 217 | 0 | 0 | −639 |
| zFe | −2032 | 123 | 1930 | 16 | 113 | −215 |
| zFe2 | −3896 | 260 | 3163 | 8 | 44 | −777 |
| zGameplay | −2464 | 111 | 1794 | 4 | 23 | −693 |
| zLua | −1264 | 105 | 1286 | 8 | 42 | **−20** |
| zMain | −1016 | 48 | 1146 | 1 | 6 | +124 |
| zPhysics | −488 | 30 | 447 | 2 | 11 | **−52** |
| zPhysicsBehaviors | −536 | 36 | 503 | 2 | 11 | **−44** |
| zPlatform | −1344 | 37 | 598 | 3 | 18 | −764 |
| zSpeech | −240 | 10 | 162 | 6 | 36 | −114 |
| zTrack | −1480 | 68 | 1130 | 5 | 29 | −379 |
| zWorld | −3688 | 141 | 3236 | 5 | 30 | −482 |
| zWorld2 | −640 | 15 | 215 | 4 | 22 | −447 |

**En 21 unidades, `22.230 B` de cadenas que el objetivo tiene y nuestro objeto no
emite.** En `zLua`, `zPhysics` y `zPhysicsBehaviors` esas cadenas explican el
deficit entero a menos de 52 B.

Y no son cadenas de cada unidad: **992 distintas para 1.394 apariciones**. Las
que faltan en mas unidades a la vez:

```
15 uds  'Dist' 'PlayerNum' 'MGeneric' '1.8.1' 'Covered'
14 uds  'MAudioReflection' '16.1.0' 'GRaceStatus'
13 uds  'Attrib::Gen::audioscrape'      11 uds  'Attrib::Gen::audioimpact' 'Attrib::Gen::milestonetypes'
 7 uds  'WorldBodyConn' 'Pkt_Body_Open' 'Pkt_Body_Send' 'Pkt_Body_Service'
        'Pkt_Effect_Open' 'Pkt_Effect_Send' 'Pkt_Effect_Service'
        'World_UpdateBody' 'World_OneShotEffect' 'WorldEffectConn'
```

Eso **no es contenido de cada unidad: es el prefijo estandar de una TU de este
proyecto**, el mismo que `zAI.cpp`, `zFEng.cpp`, `zFoundation.cpp` y compania ya
escriben a mano en 92 B. La prueba esta en `zGameModes` (§4): sus 840 B de
`.rodata` original son exactamente esa lista.

## 4. `zGameModes` explicada al byte — y es de `splits.txt`

Es la unica del frente con SUPERAVIT pequeno (`rodata+104`) y se explica entera.

El objeto extraido tiene **cinco simbolos y ninguna seccion de datos**:
`.text` 124 B (`__static_initialization_and_destruction_0` 80 +
`_GLOBAL_.I.aEmotionalSummaryTypeStrings` 44), `.ctors` 4 B, `.bss` 8 B
(`kFloatScaleUp`/`Down`) y un UNDEF a `lbl_803EBE90`.

Nosotros emitimos `.rodata` 232 B y `.data` 64 B. El estripado:

- `.data` 64 B = tres arrays de 24+24+16 B, **todos multiplos de 8** → se van
  enteros, delta 0. Correcto.
- `.rodata` 232 B = 20 simbolos muertos (`$LC60..$LC78` y `_vt.14EmotionManager`)
  mas `$LC79` vivo. `size & ~7` suma **128 B**. Los **104 B** que quedan son las
  colas `size & 7` (72 B: `$LC62` 5, `$LC71` 7, `$LC74` 6, los cuatro `$LC75..78`
  de 4 B cada uno...) mas 32 B de relleno de alineacion, **que no pertenece a
  ningun simbolo y por eso sobrevive intacto**.

**232 − 128 = 104 = el `rodata+104` exacto.** No hay nada que escribir ni que
quitar del codigo: el superavit es la granularidad del estripador.

Y el sitio de esos bytes en el original esta localizado. Las cadenas
`Embarrassed/Anxious/Smug/Ecstatic/PursuitBegins/.../RampDown/Final`, mas
`EmotionManagerImpl` y `EmotionalResponse`, estan en
`0x803EBD64..0x803EBE90`, **dentro de `pad_05_803EBB48_rodata`** — los 840 B sin
reclamar que `splits.txt` le da a `zGameplay` y que la r48 ya habia visto sin
identificar. El volcado del rango completo es el prefijo estandar de la TU:

```
803EBB48  "GAMECUBE" "d:/mw/speed/indep/bware/inc/bware.hpp" "bad_alloc"
          "%f,%f,%f" "%f,%f,%f,%f" "STL"
803EBBA8  "Attrib::Attribute" .. "Attrib::Blob" "Attrib::Gen::effects" "Attrib::TAttrib"
          "Attrib::Gen::audioimpact" "Attrib::Gen::audioscrape" "Attrib::Gen::simsurface"
803EBCB0  "WorldBodyConn" "Pkt_Body_Open" "Pkt_Body_Service" "World_UpdateBody"
          "Pkt_Body_Send" "World_OneShotEffect" "Pkt_Effect_Send" "WorldEffectConn"
          "Pkt_Effect_Open" "Pkt_Effect_Service"
803EBD64  "Embarrassed" .. "Final"   <- lo unico que hoy emitimos
803EBE20  "EmotionManagerImpl"  constantes  "EmotionalResponse"
803EBE90  lbl_803EBE90 = 1.0f   <- el UNDEF que pide zGameModes
```

**Propuesta para `splits.txt`** (no la aplico, regla 3):

```
Speed/Indep/SourceLists/zGameModes.cpp:   .rodata start:0x803EBB48 end:0x803EBE90
Speed/Indep/SourceLists/zGameplay.cpp:    .rodata start:0x803EBE90 end:0x803ED338
```

Es una frontera entre dos objetos **contiguos en el orden de enlace** —sus
`.text` (`0x8019A1F0..0x8019A26C` y `0x8019A26C..0x801BCB0C`) y sus `.ctors`
(`0x803C8BA0` y `0x803C8BA4`) van pegados—, asi que **ningun byte cambia de
posicion de enlace** y no aplica el aviso de `nfsmw-rango-no-basta.md`. Despues
del cambio `zGameModes` deja de ser un caso raro y pasa a ser el caso normal del
frente: le faltaran los ~600 B del prefijo estandar que hoy no emite.

## 5. La receta del `asm()` de fichero: donde generaliza y donde no

La pregunta del encargo. La respuesta es que **generaliza a una de las dos
familias y a la otra no**, y el criterio es si cc1plus emite tambien esa cadena.

- **Si cc1plus NO la emite** (los 22,2 kB de §3): el `asm()` anonimo es la
  herramienta correcta y ya existe —`genrodata.py --anonimo`, con la medida de la
  r36 detras: 16 B crudos sobreviven 16 de 16; los mismos 16 B como dos simbolos,
  8 de 16—. Un rango sin `.globl`/`.type`/`.size` no es un simbolo y
  `-strip-unused-data` no lo puede tocar.
- **Si cc1plus SI la emite** (los 20.464 B de §3): el `asm()` **duplica**. La
  copia del compilador no desaparece: deja su cola `size & 7` mas el relleno, que
  es justo lo que mide `zGameModes` (104 B de 232). Ahi la herramienta es
  `keep.lst`, que es lo que hace `deadlink.py`.

Y hay una tercera consecuencia, que es el limite real del frente: **el tamano de
`.rodata` no dice lo cerca que esta una unidad**. Nuestro pool esta ademas
PERMUTADO. `zAI`, comparando el objetivo con nuestro `.o` justo detras del
prefijo de 92 B:

```
objetivo  ..Blob, simsurface, TAttrib, speech, aivehicle, pursuitlevels,
          pursuitescalation, pursuitsupport, trafficpattern, pvehicle,
          VehicleParams, world, MGeneric, MSpawnTraffic, InitialPos, ...
nuestro   ..Blob, trafficpattern, TAttrib, pvehicle, EventSequencerSystems,
          simsurface, chassis, engine, induction, nos, tires, transmission,
          AITrafficManager, gameplay, 19.8.31, ...
```

Mismo idioma, otro orden: es el frente de `nfsmw-inline-es-posicion.md` visto en
`.rodata`. Cerrar el tamano es condicion necesaria y **no** suficiente.

## 6. Dos palancas nuevas, medidas

### 6.1 `ZMAIN_MESSAGES_LUA_INLINE`: la guarda que nadie enciende

`src/Speed/Indep/Src/Generated/Messages/*.h` (65 cabeceras) definen
`BuildMessageTable` y `HandleMessage_LuaBinding` **en la cabecera** bajo
`#ifdef ZMAIN_MESSAGES_LUA_INLINE`, con un comentario de la r35 que dice que ahi
es donde los pone el original (`debug_lines`). Hoy solo lo definen **seis**
SourceLists: `zMain`, `zFe`, `zFe2`, `zPhysics`, `zSim`, `zSpeech`.

En el original, esos cuerpos se emiten en TODA TU que incluya la cabecera; el
enlazador se lleva las funciones y **el pool de cadenas se queda** (§1). De ahi
salen `MGeneric`, `MinHeatLevel`, `Perpetrator`, `CopHandle`, `BackupType`,
`PathEvent`, `MAudioReflection`, `Dist`, `PlayerNum`, `Covered`... que es la
cabeza de la lista de §3.

Ensayo: `#define ZMAIN_MESSAGES_LUA_INLINE` en cinco SourceLists de mi
territorio, `build_direct.py`, `lcfix` sobre una copia del `keep.lst` y las
entradas de `deadlink.py`:

| unidad | hoy | +guarda (con lcfix) | +guarda +keeps | **ganancia** | `fncmp` | `.text` |
|---|---:|---:|---:|---:|---|---|
| `zAI` | −1344 | −1216 | **−744** | **600 B** | 0 de 1030 | +0 |
| `zAnim` | −1240 | −1224 | **−1072** | 168 B | 0 de 315 | +0 |
| `zEAXSound` | −1544 | −1424 | **−1024** | 520 B | 0 de 773 | +0 |
| `zGameplay` | −2656 | −2584 | **−2312** | 344 B | 0 de 768 | +0 |
| `zLua` | −1648 | −1504 | **−952** | 696 B | 0 de 537 | **+196** ← ojo |
| `zMisc` | +2840 | +2840 | +3352 | — (no cambia nada) | 0 de 450 | +0 |

**2.328 B de deficit cerrados en cinco unidades sin mover una instruccion.** Y
quedan once mas sin la guarda: `zCamera`, `zEAXSound2`, `zEcstasy`, `zTrack`,
`zWorld`, `zWorld2`, `zPlatform`, `zPhysicsBehaviors`, `zEagl4Anim`,
`zFeOverlay`, `zGameModes`.

### 6.2 `BWARE_PREFIX_GAMECUBE`: el `"GAMECUBE"` duplicado, en tres unidades

`bWare.hpp:164 bGetPlatformName()` devuelve `"GAMECUBE"` salvo que la SourceList
defina `BWARE_PREFIX_GAMECUBE` apuntando al `"GAMECUBE"` que su prefijo escrito a
mano ya trae en `+0x00`. Lo definen siete unidades. **`zAI`, `zPhysics` y
`zPhysicsBehaviors` escriben el prefijo pero NO lo definen**, asi que cc1plus
interna una segunda copia — y `keep.lst` la mantiene VIVA:

```
unidad             objetivo  nuestro   donde estan las dos copias
zAI                   x1       x2      +0x00 lbl_803C8C60          +0x60 $LC58   (en keep.lst)
zPhysics              x1       x2      +0x00 pad_05_803F68F0_rodata +0x60 $LC58  (en keep.lst)
zPhysicsBehaviors     x1       x2      +0x00 lbl_803FA7B0          +0x60 $LC145  (en keep.lst)
zSim                  x1       x1      +0x00 pad_05_80403F48_rodata          <- correcta
```

Ensayo en `zAI`: anadiendo `#define BWARE_PREFIX_GAMECUBE (_bwarePrefix)` justo
detras del `BWARE_PREFIX_BADALLOC` que ya tiene, el duplicado desaparece
(`dupstr` pasa de 14 cadenas duplicadas a 13) con **`fncmp` 0 de 1030**. Es
contenido correcto, no tamano: la `.rodata` sale 8 B mas corta (−744 → −752),
porque los 12 B que se quitan estaban FORZADOS vivos.

El paquete tiene que quitar de `keep.lst` el par
`# @lc zAI "GAMECUBE"` / `zAI.o:$LC58` (`lcfix` lo dice solo:
`FALLO zAI: 'GAMECUBE' no tiene simbolo $LC propio`).

De la misma familia y sin resolver, la lista completa de duplicados forzados
(`scripts/dupstr.py`, nuevo): `zSim` `"FEngHUD"` x2 —el de la r49, sigue ahi— y
**`"SimModel"` x2, que la r49 no vio**; `zMain` `"SuspensionParams"` y
`"RBComplexParams"` (`$LC523`, `$LC521`, los dos en `keep.lst`); `zTrack`
`"Attrib::Gen::gameplay"` (`$LC237`); `zEcstasy` cinco cadenas de UI alemanas
duplicadas entre su `asm()` a mano y su `$LC`.

### 6.3 El bloqueo de coordinacion, que es lo que impide aplicarlo esta ronda

**Cualquier cambio de fuente que anada o quite un literal desplaza todos los
`$LC` posteriores de la unidad y deja rancias sus entradas de `keep.lst`.**
Medido: con la guarda puesta en cinco unidades, `lcfix.py --check` pasa de limpio
a **37 lineas `CORRIGE`** (`zEAXSound` 18, `zAI` 8, `zGameplay` 3, ...).

O sea que las dos palancas de §6 **no se pueden aplicar sin escribir en
`config/GOWE69/keep.lst`**, que la regla 3 me prohibe. Por eso he revertido todo
(§7) y lo dejo como paquete atomico:

```
1. #define ZMAIN_MESSAGES_LUA_INLINE   en zAI, zAnim, zEAXSound, zGameplay
                                       (y zLua solo si se acepta el text+196)
2. #define BWARE_PREFIX_GAMECUBE (_bwarePrefix)   en zAI
   (y el equivalente en zPhysics y zPhysicsBehaviors, que son de otro agente)
3. python scripts/build_direct.py <esas unidades>
4. quitar de keep.lst el par  # @lc zAI "GAMECUBE" / zAI.o:$LC58
5. python scripts/lcfix.py
6. python scripts/deadlink.py <esas unidades> --keep >> config/GOWE69/keep.lst
7. python scripts/lcfix.py --check   (tiene que salir limpio)
```

Resultado medido del paquete completo sobre `zAI`: `rodata −1344 → −752`,
`.text +0`, `fncmp` 0 de 1030, DOL `ad01f353ad45` (sigue ROTO: le quedan los
~600 B de prefijo estandar de §3 y la permutacion de §5).

## 7. Lo que he tocado y como lo he dejado

**Nada.** `git status src/` no lista ni uno de mis ficheros. Lo que hubo y se
revirtio, con su copia de entrada en el scratchpad:

- `zAI.cpp`, `zAnim.cpp`, `zEAXSound.cpp`, `zLua.cpp`, `zGameplay.cpp`,
  `zMisc.cpp` — el `#define` de §6.1 (y en `zAI` tambien el de §6.2). Revertidos
  y **recompilados** con `build_direct.py`.
- `config/GOWE69/keep.lst` — **no se ha tocado**. Los ensayos usan copias en
  `scratchpad/r50_rodata/keep_lcfix*.lst`.
- **No se ha lanzado ningun `ninja` ni `configure.py`**, ni hay commits ni
  `git add`.

Aviso para el siguiente: `build_direct.py zEAXSound` recompila tambien
`zEAXSound2` por prefijo, y `zMisc` arrastra `zMiscSmall`. No cambie su fuente,
pero sus `.o` se han regenerado.

## 8. `zFe2`: las 17 funciones no existen

`permorden.py` dice «1.290 de 1.307». Emparejando **sin** el filtro `s['size']`:

```
objetivo 1307 funciones con tamano; nuestro objeto define 1625 nombres en codigo
NO DEFINIMOS NI CON ALIAS: 0 (0 B)
DEFINIDOS SOLO POR ALIAS (size 0 en nuestro objeto): 17
```

Las 17 son las mismas 16 clases locales de `FEPlayerCarDB`
(`_._Q313FEPlayerCarDB...` y `Callback__CQ313FEPlayerCarDB...`, sufijos `.35649`
a `.35774`) que arreglo `mangfix.py` en la r49, definidas por los cuatro `asm()`
de `src/Speed/Indep/Src/Frontend/Database/VehicleDB.cpp:416,426,458,514`, mas
`_IHandle__15IGenericMessage` (12 B) del `asm()` de
`src/Speed/Indep/Src/Frontend/HUD/FeGenericMessage.cpp:87`.

Esos `asm()` emiten `.globl` y `.type ...,@function` pero **no `.size`**, asi que
`st_size` vale 0 y el filtro de `permorden.py` (`if s['type']==2 and ... and
s['size']`) no las cuenta. **No falta codigo en `zFe2`**: `linkdelta` da su
`.text` a `+0` y `fncmp` 0 de 1.307.

Arreglo de la herramienta, si se quiere: `permorden.py` deberia aceptar los
simbolos de tamano 0 del lado NUESTRO (el objetivo siempre los trae con tamano).
No lo he tocado para no cambiarle la salida a otro agente a mitad de ronda.

## 9. Negativos, con la cifra

- **`deadstr.py --keep` sobre las 23 SourceLists: 0 entradas nuevas.** Lo que ya
  esta en `keep.lst` es exactamente lo que esa herramienta sabe ver (§1).
- **El relleno `gap_`/`pad_` no explica nada**, confirmado: `zGameModes` tiene 0 B
  de `gap_` y le sobran 104.
- **Salvar mas dato no siempre acerca.** Cuatro de 23 empeoran (§3), una de ellas
  (`zLua`) devolviendo 196 B de `.text` que hoy estan a cero.
- **`zGameModes` no se puede cerrar desde la fuente.** Sus 104 B son colas
  `size & 7` mas relleno de alineacion; ni `keep.lst` (que anadiria bytes) ni un
  `asm()` (idem) los quitan. Es `splits.txt` (§4).
- **Ninguna unidad llega a `DOL OK`.** El mejor resultado del frente, `zSpeech`
  con las 33 entradas nuevas, se queda en `rodata-240` y DOL `2d99ad828bbf`.
- **La `.rodata` esta ademas permutada** (§5): el tamano es condicion necesaria y
  no suficiente, y ese es el trabajo de la ronda siguiente.

## 10. Verificacion

```
fncmp  zAI 0/1030   zAnim 0/315   zEAXSound 0/773   zLua 0/537   zGameplay 0/768
       zMisc 0/450  zFe 0/921     zFe2 0/1307       zPhysics 0/718  zGameModes 0/2
       (identicos antes y despues; ninguna funcion empeora en ninguna unidad)
lcfix.py --check          todas las entradas @lc estan al dia
linkdelta.py              identico al de partida en las 23 unidades
enlace base               DOL 9619ba57c9919f95f7f2ac951a2166a3517f91e3  OK
```

Volcados y objetos de prueba borrados; `scratchpad/r50_rodata/` vacio.

## 11. Herramientas nuevas

**`scripts/deadlink.py`** — `deadstr.py` con la prueba de vida medida contra el
enlazador en vez de deducida de las reubicaciones (§1).

```
python scripts/deadlink.py                 # censo de las 23
python scripts/deadlink.py zSpeech --keep  # las lineas @lc para keep.lst
python scripts/deadlink.py zSpeech --medir # el delta de secciones antes/despues
```

Cuesta ~8 s por unidad (tres enlaces del proyecto entero) y lleva en el docstring
los dos avisos de §3 y §6.3.

**`scripts/dupstr.py`** — cadenas que emitimos mas veces que el objetivo,
contadas por CONTENIDO (no por simbolo, que es lo que se le escapa a
`dupdata.py` cuando la copia a mano va dentro de un bloque sin simbolo propio), y
marcando cuales estan FORZADAS vivas por `keep.lst`. Es lo que saco §6.2 y el
`"SimModel"` de `zSim` que la r49 no vio. Instantaneo, no enlaza.

```
python scripts/dupstr.py            # todas
python scripts/dupstr.py zSim zAI   # solo esas
```
