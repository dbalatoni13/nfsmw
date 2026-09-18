# r64 - track (zTrack, zLua)

## Resumen

| unidad | antes | despues | ganancia |
|---|---:|---:|---:|
| **zLua** | 8.157 B | **1.445 B** | **-6.712 B** |
| zTrack | 3.188 B | 3.195 B | 0 (es ruido de la tanda, no lo he tocado) |

Metrica: **bytes distintos del DOL, contados uno a uno** (no la agrupacion de
`dolwhere`, que da 9.152 -> 1.735 para los mismos dos enlaces). Las dos cifras
de zLua estan tomadas **en el mismo estado del arbol**: la de "antes" se midio
HOY, con `git stash` de mis cinco ficheros y la `keep.lst` de `config/`, porque
otros agentes han tocado cabeceras compartidas durante la ronda y el 9.152 del
encargo ya no era comparable.

`.data` de zLua: **0 B distintos** (venia con 245).

**Ruido de agentes en paralelo, cuantificado:** durante la ronda otro agente ha
recompilado alguna unidad YA PROMOCIONADA (el enlace usa su `.o`, no el
extraido), y eso mueve la cifra ABSOLUTA de cualquiera. Con `.o` identico
(sha1 sellado) y `keep.lst` identica, zLua marco 2.041 y luego 2.075, y zTrack
3.188 y luego 3.222: **+34 B los dos, el mismo desplazamiento**. Por eso doy
SIEMPRE zTrack al lado como testigo: la ultima medida es zLua 1.445 con zTrack
en 3.195, contra la base de zLua 8.157 con zTrack en 3.188, o sea 7 B de ruido
sobre 6.712 de ganancia. La cifra ABSOLUTA hay que volver a tomarla cuando la
tanda cierre.

**NO se promociona nada.** zLua sigue a 1.445 B y zTrack a 3.195 B.

---

## AVISO QUE MANDA SOBRE TODO LO DEMAS

**El arbol tal como lo dejo NO enlaza bien zLua hasta que se toque
`config/GOWE69/keep.lst`.** Sin ese paso las secciones salen descuadradas
(`.rodata` -512 B) y zLua queda PEOR que al empezar. Es exactamente el
"trabajo a medias regresa" del dossier, y por eso lo pongo el primero.

Son **tres cosas**, en este orden:

1. `python scripts/build_direct.py Speed/Indep/SourceLists/zLua`
2. `python scripts/lcfix.py zLua` -- corrige **63** entradas `$LC` sola (la
   renumeracion; se resuelven por contenido, no hay que mirarlas). Ademas
   imprimira **DOS FALLOS**, que son las dos entradas caducadas a **borrar**:

   ```
   # @lc zLua "GAMECUBE"          (linea 1332)   +  zLua.o:$LC58
   # @lc zLua "%d"                (linea 3157)   +  zLua.o:$LC635
   ```

   La de `GAMECUBE` ya estaba caduca ANTES de esta ronda (la mata
   `BWARE_PREFIX_GAMECUBE`, definido desde la r58) y es inerte: quitarla no
   cambia un byte, medido. La de `%d` la mata esta ronda,
   `ZLUA_NO_FEPKG_POSTRACE`.

3. **Anadir estas siete lineas al final del bloque de zLua** (los `$LCnnn` son
   los del `.o` con sha1 `0d2b58c1921b`; si el `.o` cambia, `lcfix` los vuelve
   a resolver por contenido, asi que lo unico que importa es que las
   directivas `# @lc` esten puestas):

   ```
   # zLua r64-track: cadenas muertas nuevas que el objetivo SI tiene, colocadas
   # en su sitio por los primer/#include de esta ronda.
   # @lc zLua "MAudioReflection"
   zLua.o:$LC472
   # @lc zLua "PlayerNum"
   zLua.o:$LC473
   # @lc zLua "VehicleParams"
   zLua.o:$LC478
   # @lc zLua "Attrib::Gen::speechtune"
   zLua.o:$LC479
   # @lc zLua "Attrib::Gen::world"
   zLua.o:$LC481
   # @lc zLua "MLoadingComplete"
   zLua.o:$LC624
   # @lc zLua "MEnterFreeRoam"
   zLua.o:$LC642
   ```

Hay una `keep.lst` completa ya montada en
`scratchpad/track64/keep.lst.r64track` y el diff linea a linea en
`scratchpad/track64/keep-diff.txt`, pero **NO la copies encima**: otros agentes
estan editando `keep.lst` en paralelo y la copia se los llevaria por delante.
Los pasos 1-3 son seguros porque solo tocan lineas de zLua.

Sello: `.o` de zLua = `0d2b58c1921b5dd5f830686df660575dda6c5710`, TRES
compilaciones seguidas con el mismo sha1.

**Aviso de sello, medido:** anadir SOLO COMENTARIOS a `LuaGameHooks.cpp` cambia
el sha1 del `.o` (`f4abbed967e1` -> `0d2b58c1921b`) y no cambia un byte del DOL
(1.448 -> 1.445, y esos 3 B son el ruido de la tanda: zTrack se movio igual).
O sea que el `.o` lleva numeros de linea. **El sha1 sella una compilacion, no un
resultado**: para comparar dos variantes hay que mirar el DOL, no el sha.

---

## zLua: que se ha hecho

El frente de zLua era **el ORDEN de la `.rodata`**, no el codigo: `fncmp` daba
0 de 537 antes y da 0 de 537 despues. La unidad tenia ademas un `.space 488` al
final --relleno de compensacion que la r61 dejo puesto para cuadrar la seccion--
y ese relleno era **la cuenta exacta de lo que faltaba**: cada byte de contenido
de verdad colocado se descuenta de ahi. Al acabar la ronda vale **16**.

La herramienta que ha hecho posible medir esto es `scratchpad/track64/m.py`:
compila, arregla una **copia privada** de `keep.lst` (config/ no se toca),
enlaza y da (a) bytes distintos por zona, (b) la direccion de la primera
diferencia y (c) `--shift`, que es la clave.

### `--shift`: la ventana de 32 hay que rodearla, no sufrirla

`.rodata` del DOL se redondea, asi que "seccion +32" puede ser cualquier delta
entre 1 y 32 y **el tamano no sirve para cuadrar la unidad**. Lo que si sirve es
**buscar el desplazamiento de lo que va DETRAS**: se coge la ventana de 512 B
que empieza en `0x803EF580` (donde arranca zMain) en el DOL original y se busca
en el nuestro con desplazamiento -256..+256. El numero que sale es el error
EXACTO en bytes de la unidad. Con eso el ciclo es: edito, leo el shift, ajusto
el `.space` por ese shift, y vuelvo a medir con la seccion cuadrada.

### Los nueve pasos, en orden, con lo que valio cada uno

| # | que | primera dif. queda en | zona `.rodata` |
|---|---|---|---:|
| 0 | base | 0x803ED5C0 | 8.006 |
| 1 | primer `_zLuaPoolVer` (simsurface / 16.1.0 / 16.2.1 / 1.2.3 / 1.8.1 / 19.8.31 / GRaceStatus) | 0x803ED5F4 | 7.437 |
| 2 | bloque VERBATIM de 160 B: CHAR..INVALID entre `lmem.c` y `lobject.c` | " | " |
| 3 | matar `emitterdata`/`emittergroup` con la guarda `ATTRIB_TAG` | 0x803ED614 | 7.205 |
| 4 | `asm` con `"LuaBindery"` dentro de `LuaBindery.cpp` | 0x803EE150 | 4.766 |
| 5 | `asm` con `"LuaAttributes"` dentro de `LuaAttributes.cpp` | 0x803EE458 | 4.041 |
| 6 | `keep.lst`: `MAudioReflection`, `PlayerNum` | 0x803EE496 | 3.969 |
| 7 | primer `_zLuaPoolMsg` (audiosystem / VehicleParams / speechtune / pursuitlevels / world / visuallook / GManager / done) | 0x803EE5D1 | 3.658 |
| 8 | **reordenar los 18 `#include` de `Generated/Messages/M*.h`** | 0x803EE750 | 3.316 |
| 9 | `SmackableParams` + 136 B VERBATIM de `SoundConn.h` | 0x803EE8F5 | 1.544 |
| 10 | `ZLUA_NO_FEPKG_POSTRACE` (mata cinco formatos que el objetivo no tiene) | 0x803EE8F4 | 2.396 |
| 11 | los dos mensajes de corrutina VERBATIM + `LUA_RESUME_MSG` | " | 1.814 |
| 12 | `DEAD_STR` nulo **a partir de** `MFlowReadyForOutro.h` | **0x803EEA48** | **1.186** |
| 13 | `IEngineDamage.h` DELANTE del bloque de mensajes | " | " (pero `.text` 822 -> **252**) |

La primera diferencia de `.rodata` ha pasado de `0x803ED5C0` a `0x803EEA48`:
**5.768 B de `.rodata` que antes estaban descolocados y ahora casan byte a byte.**

### Los cuatro hallazgos que valen para otras unidades

**1. `size & ~7` regala las cadenas de 7 bytes o menos.** `"16.1.0"` (7 B con
el NUL) y `"1.8.1"` (6 B) son MUERTAS en nuestro arbol y aun asi
`-strip-unused-data` **no se lleva un solo byte** de ellas: `7 & ~7 = 0`. O sea
que un primer las trae por cero bytes y **sin entrada en `keep.lst`**. Lo mismo
con `"done"` (5 B). Es el reverso de la "ventana de 32" y hay que mirarlo SIEMPRE
antes de escribir un `asm()` o pedir una linea de `keep.lst`.

**2. El ORDEN DE LOS `#include` DE `Generated/Messages/M*.h` ES el orden del
pool.** En `LuaGameHooks.cpp` estaban en orden alfabetico (el de la extraccion)
y el objetivo los quiere en otro. Reordenarlos --y solo eso-- adelanto la
primera diferencia 383 B de golpe. El orden del original, medido cadena a
cadena en el DOL, es:

```
MNISComplete, MNotifySpeedTrap, MNotifyRacePlacement, MNotifyKnockedOut,
MForcePursuitStart, MSetCopsEnabled, MNotifyPlayerRep, MJackKnife,
MSetTrafficSpeed, MAIEngineRev, MLoadingComplete, MNotifyChallengePassed,
MNotifyFinished, MGamePlayMoment, MControlPathfinder, MNotifyMovieFinished,
MEnterFreeRoam, MFlowReadyForOutro
```

y **el bloque tiene que ir detras de `class EAX_CarState;`**, porque
`MAIEngineRev.h` lo usa (ponerlo antes no compila). `MLoadingComplete.h` y
`MEnterFreeRoam.h` no estaban incluidos y hay que anadirlos.
Vale la pena mirar esto en zFe, zFe2, zMain, zPhysics, zSim y zSpeech, que
definen `ZMAIN_MESSAGES_LUA_INLINE` igual.

**3. NEGATIVO MEDIDO Y CARO: `keep.lst` sobre un `$LC` puede resucitar `.text`.**
`-keep zLua.o:$LC760` (`"cannot resume dead coroutine"`) mete **+192 B de
`.text`** en el enlace. Medido con el MISMO `.o` (sha1 `248a0a2aacf5`), solo
cambiando la lista: 7 entradas -> `.text` +0; 8 entradas -> `.text` +192; 9
entradas -> `.text` +192. El objeto extraido **no trae ni `lua_resume` ni
`resume_error`** (el enlazador original los tiro) pero **si trae sus dos
cadenas**: dato vivo con codigo muerto. La cura que si vale es apagar el literal
con una macro (`LUA_RESUME_MSG`, guarda `#ifndef` en `ldo.c`) y escribir los 72 B
en un `asm()` ANONIMO, que el estripado no puede tocar porque no tiene simbolo.
**Antes de anadir una entrada `$LC` a `keep.lst`, comprueba el tamano de
`.text`.**

**4. Una macro de "literal muerto" hay que apagarla en el SITIO, no en el
`.cpp` de la unidad.** `DEAD_STR` en `zLua.cpp` mata ademas las cuatro
`SMS_MESSAGE_%d*`, que el objetivo SI tiene, y deja `.rodata` -96 B (medido).
Puesta **dentro de `LuaGameHooks.cpp`, justo detras de `MFlowReadyForOutro.h`**
--o sea despues de que se internen las SMS y antes de que se parsee
`feScrollerina.hpp`-- solo mata lo que sobra. La misma idea sirve para
`ATTRIB_TAG` y para `HAND_POOL_TAG`.

**5. Reordenar `#include` mueve TAMBIEN la cola diferida, y hay que mirar los
vecinos.** El bloque de `M*.h` reordenado dejo `IEngineDamage.h` detras de todos
ellos (antes estaba en medio). Resultado medido: `.rodata` igual de bien, pero
**`.text` de 252 a 822 B distintos** y `textorder` de 2 descolocadas de 538 a 9,
porque cambia el PRIMER PUNTO DE USO de `_IHandle__13IEngineDamage` y con el el
orden de la cola de `finish_file`. Devolverlo delante del bloque lo arregla del
todo: **-570 B de `.text` por mover una linea**. Cuando muevas un bloque de
`#include`, mira siempre los que quedan a su lado.

### Ficheros tocados (los cinco son EXCLUSIVOS de zLua, comprobado)

- `src/Speed/Indep/SourceLists/zLua.cpp`
- `src/Speed/Indep/Src/Lua/LuaBindery.cpp`
- `src/Speed/Indep/Src/Lua/LuaAttributes.cpp`
- `src/Speed/Indep/Src/Lua/LuaGameHooks.cpp`
- `src/Speed/Indep/Src/Lua/source/ldo.c`

`grep -rln` sobre `src/Speed/*/SourceLists/` confirma que solo `zLua.cpp` los
incluye. No se ha tocado ninguna cabecera compartida.
`ZLUA_NO_FEPKG_POSTRACE` y `LUA_RESUME_MSG` son guardas: fuera de zLua el
comportamiento es identico.

### Lo que le queda a zLua: 1.445 B, y donde estan

```
803EEA48               14 B   falta un bf800000 (-1.0f) en el pool
803EEA64            1.231 B   arrastre del anterior: todo va 8 B corrido
803EEF3F/5F/9F, 803EF01F  18 B  cuatro huecos de 4-6 B
803EF510               67 B   el .space 16 final
801BEECA..801D3B44    252 B   mitades @l sueltas, consecuencia de lo de arriba
```

`.text` esta en **252 B**, `.data` en **0**, `textorder` en **2 descolocadas de
538 y 3 saltos de delta** (exactamente lo que habia al empezar: cero regresion
de orden).

**El siguiente paso es el `bf800000` de `0x803EEA48`**: es UNA constante de
coma flotante que el objetivo tiene en medio de un volcado de pool y nosotros no.
Como los flotantes se vuelcan por funcion, eso significa que ahi hay una funcion
del original con una constante mas -- casi seguro otra funcion MUERTA cuyo pool
el enlazador original conservo, como `lua_resume`. Los 1.231 B de detras son
puro arrastre de esos 8 B: se caen solos. Con eso zLua quedaria en unos
**330 B**, y esos 330 serian ya los `@l` que dependen del ultimo hueco.

**zLua es la unidad mas cerca de promocionar de las veinte que quedan.**

---

## zTrack: NO se ha tocado, y por que

zTrack esta **igual que al empezar** (3.188 B byte a byte, 3.694 con la
agrupacion de `dolwhere`; `.o` limpio, sin diff en git). El encargo decia "esta
a UN SOLO BLOQUE, 348 B de cadenas duplicadas". **Los 348 B son ciertos y estan
medidos, pero no son un bloque suelto: quitarlos SOLOS empeora la unidad.**

### Lo que hay, medido

La `.rodata` de zTrack (`0x80408FB8..0x80409D58`, 3.488 B) casa hasta el final
del prefijo escrito a mano (`lbl_80408FB8`, 0x21C). A partir de ahi:

- Nosotros soltamos **348 B** en `0x804091D4..0x80409330`: `GAMECUBE`, las ocho
  `Attrib::*`, `Attrib::Gen::pvehicle`, `Attrib::TAttrib`,
  `Attrib::Gen::simsurface`, `Attrib::Gen::engine`, `Attrib::Gen::gameplay`,
  `19.8.31`, `16.2.1`, `1.2.3` y siete cadaveres (`stems`, `ssis`, `uction`,
  `es`, `n`, `done`, `ry`).
- El objetivo, en ese mismo sitio, tiene el pool de `Skids.cpp`
  (`SkidPoints`/`SkidPoint`/`Dot`...). O sea que nuestro pool va **+344 B**.
- Pero **1.036 B mas adelante nuestro pool va -588 B**: `CODEINE` esta en
  `0x80409434` y el objetivo lo tiene en `0x80409680`. Entre medias el objetivo
  tiene dos bloques que nosotros soltamos AL FINAL del TU (el `asm` de 68
  cadenas de la r52):
  - `0x80409310..0x804093FC`: `RESET`, `RESET_TO_POINT`, ... `PURSUIT_START`,
    `High`, `Medium`, `Low`, `Reflection`;
  - `0x8040944C..0x80409678`: `Attrib::Gen::milestonetypes`, `16.1.0`, `16.2.1`,
    `1.2.3`, `1.8.1`, `19.8.31`, `speech`, `aivehicle`, `pursuitlevels`,
    `pursuitescalation`, `pursuitsupport`, `ecar`, `camerainfo`, `pvehicle`,
    `engine`, `engineaudio`, `MGeneric`, `MAudioReflection`, `PlayerNum`,
    `Dist`, `Covered`, `audiosystem`, `GRaceStatus`, las cuatro
    `SMS_MESSAGE_%d*`, `TrackStreamingLoadedZonePoint`, `Zone`, `LoadTime`,
    `Time`.

**Quitar los 348 B de la cabeza sin colocar antes esos dos bloques deja
`CODEINE` 936 B por delante en vez de 588: es PEOR.** Los dos movimientos son
un solo paquete, igual que en zLua.

### Y hay un problema que zLua no tenia: faltan FLOTANTES

Entre `ClanSlotPool` y `CODEINE` el objetivo tiene **156 B de constantes de coma
flotante** (`0x804092B0..0x8040930F`, `0x804093D8`, `0x804093FC..0x80409434`) y
nosotros solo **44 B**. Faltan ~112 B de pool de flotantes, con la pareja
`43300000 80000000` (conversion int->float), `+-2048.0` y dos `2e5be6ff`. Como
el pool de flotantes se vuelca **por funcion**, eso es codigo, no cadenas: o hay
funciones del original que nuestro enlace estripa, o hay una diferencia de forma.
Es el mismo sintoma que el `bf800000` que le queda a zLua, y probablemente la
misma causa (pool de funcion muerta que el enlazador original conservo).

**Ese es el trabajo que zTrack necesita, y no cabia en esta ronda sin dejar la
unidad a medias.** El mapa completo esta arriba; con `m.py --shift` y el
`.space` de compensacion (zTrack no lo tiene: hay que anadirlo) es la misma
receta que acabo de aplicar a zLua, paso a paso.

`GetLoadingPriority` (708 B, la unica funcion abierta de zTrack) aporta ademas
~150 B propios al DOL --`fc1f`/`fc1e`, `c1a7`/`c12b`: es reparto de registros--
y la r37 ya la dejo con veinte ensayos y un "hace falta evidencia nueva del
asignador". No la he tocado.

### NEGATIVO MEDIDO en zTrack (y una veda que se rompe a medias)

`docs/analisis` de la r62 dejo escrito que en zTrack "la guarda es UNA sola
para las dos cosas (AttribSys.h:43 y :88)". **Eso es falso**: `AttribSys.h` usa
`ATTRIB_TAGS_HAND_POOL` para `ATTRIB_TAG(s)` y `ATTRIB_TAG_ORDER_HAND_POOL`
para `_AttribAllocTagOrder`, y son independientes.

Pero definir **solo** `ATTRIB_TAG_ORDER_HAND_POOL` en zTrack **no sirve para
nada**, medido: `.rodata` +0 B, DOL 3.708 B antes y despues, y **0 entradas de
`keep.lst` caducadas** (las ocho `Attrib::*` siguen emitiendose). O sea que en
zTrack esas ocho cadenas **no salen del primer de `AttribSys.h`**, salen de
llamadas de verdad. La conclusion de la r62 era correcta por el motivo
equivocado. Anotado tambien en `zTrack.cpp` para que `previo.py` lo encuentre.

---

## Propuestas para el jefe

1. **`config/GOWE69/keep.lst`** -- los tres pasos del aviso de arriba. Sin ellos
   zLua no enlaza.
2. Nada mas: no hace falta tocar `configure.py`, ni `splits.txt`, ni
   `symbols.txt`, ni re-extraer.

## Herramientas que dejo en `scratchpad/track64/`

| fichero | que hace |
|---|---|
| `m.py` | compila + `keep.lst` privada + enlaza + mide: `--zona`, `--shift`, `--win`, `--dump`, `--show`, `--lc` |
| `drift.py` | alinea las cadenas del objetivo con las nuestras y dice DONDE cambia el desfase |
| `gen.py` | genera un `asm()` VERBATIM de un rango del DOL |
| `apply.py` | inserta un bloque en un `.cpp` respetando su final de linea |
| `lc.py` | los simbolos de `.rodata` del `.o` nuestro, en orden, con sus bytes |
| `genkeep.py` | monta la `keep.lst` propuesta y su diff |
| `verif.py` | enlaza con una `keep.lst` dada y cuenta bytes distintos, uno a uno |

`m.py --shift` y `drift.py` son las dos que merecen subir a `scripts/` si el
jefe quiere: con ellas un reorden de `.rodata` deja de ser adivinanza.
