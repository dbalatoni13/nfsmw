# r52 · rodata1 — zPhysics de `rodata−784` a `rodata−40`, y la palanca que lo hizo

**Ninguna de las dos promociona.** `trypromo Speed/Indep/SourceLists/zPhysics`
sigue dando **DOL ROTO** (`67dc0681044a`). El negativo va con su cifra abajo,
y con la causa exacta de los 40 B que faltan, medida y vetada por regla.

Lo que sí sale de aquí es una palanca nueva y general —**el primer de pool**— que
mueve CUALQUIER literal a CUALQUIER punto anterior del fichero por cero bytes de
`.text`, y que es lo que le faltaba a la receta de `zSim` para escalar.

    unidad      antes                              despues
    zPhysics    .text+0  rodata-784  bss-32        .text+0  rodata-40   bss-32
    zAnim       .text+0  rodata-1240 data-192      .text+0  rodata-1112 data-192
                         over-16                            over-16

`fncmp` **0 de 718** en zPhysics y **0 de 315** en zAnim, antes y después. No se
ha tocado una sola función.

**EL PAQUETE ES ATÓMICO.** Con las fuentes puestas y el `keep.lst` del árbol tal
cual, las dos EMPEORAN: `zPhysics rodata−1120` y `zAnim rodata−1312`. Medido.

---

## 1. La palanca: el primer de pool

`docs/analisis/r51-zsim.md` §2 dejó la regla (`output_constant_pool` escribe en
orden de CREACIÓN) pero no una forma de **elegir** ese orden. Aquí está, medida
con `ngccc -S` sobre un fichero de cinco líneas:

```c
static inline const char *zphys_pool_a(int i)
{
    switch (i) {
    case 0: return "MGeneric";
    case 1: return "High";
    ...
    }
    return 0;
}
```

* La función es `static inline` y nadie la llama: **cc1plus no la emite**. Cero
  bytes de `.text`, comprobado en el `.s`.
* Los literales **sí** se emiten, en el orden en que están escritos, en el punto
  del fichero donde está el primer.
* La dedup de cadenas de cc1plus es **de toda la TU** (`const_hash_table`), así
  que cualquier uso posterior —y cualquier cabecera que la vuelva a nombrar—
  **reutiliza ese mismo `$LC`**. O sea: el primer **adelanta** la cadena.
* Las constantes de coma flotante **no** comparten esa dedup (el pool de
  `force_const_mem` se reinicia por función), por eso un primer sólo sirve para
  cadenas. Para un float suelto va un `asm()` de fichero con `.4byte` crudo, sin
  símbolo, que `-strip-unused-data` no puede tocar.

**El límite, y es duro: un primer sólo mueve HACIA ATRÁS.** Quien crea la cadena
primero gana. Si una cabecera incluida arriba ya la interna, no hay primer que la
retrase; hay que mover la cabecera. Es exactamente donde se atasca zPhysics (§4).

Forma que NO vale, medida: `const char *t[] = {...}` dentro de la inline muerta
emite además la **tabla de punteros** (`$LC3`, 12 B en la prueba). El `switch` no.

## 2. El otro hallazgo: dónde caen los volcados de pool

En el `.s`, el pool se vuelca **justo antes de cada función**, y contiene todo lo
creado desde el volcado anterior: primero las cadenas de parseo, después las
constantes de la función que viene. Comprobado con `t3.cpp`:

    .rodata: cadenas parseadas + floats de f1   ->  f1
    .rodata: cadenas parseadas + floats de f2   ->  f2

Eso convierte el `.rodata` de una SourceList en un mapa legible: cada tramo de
cadenas del objetivo pertenece al hueco entre dos `#include` de `.cpp`. En
zPhysics los cinco tramos del prefijo son:

| tramo del objetivo | punto del fichero |
|---|---|
| `simsurface`…`Reflection`, 0.5, `SmackableParams`, `SmackTrigger` | antes de `SmackableTrigger.cpp` |
| 1.0, 0.0 | pool de `GetObjectMatrix` |
| `pvehicle`…`pursuitsupport`, `ExplosionParams`…`DamageParams` | entre `SmackableTrigger.cpp` y `Explosion.cpp` |
| `Attrib::Gen::explosion`, `explosion`, `default`, `SimpleRigidBody` | dentro de `Explosion.cpp`, tras `Construct` |
| `gameplay`…`AnchorWorldID` (36 cadenas) | entre `Explosion.cpp` y `PVehicle.cpp` |
| los 15 `PAD_DEAD_ZONE`…`POST_BRAKE_ACCEL_MIN` | entre `Wheel.cpp` y `VehicleBehaviors.cpp` |

## 3. Lo que he escrito, y por qué

### 3.1 `zPhysics.cpp` · `BWARE_PREFIX_GAMECUBE` (−12 B duplicados, cabeza al byte)

La deuda que `r50-rodata.md` §6.2 dejó apuntada. `zPhysics` escribe a mano el
prefijo de bWare (`pad_05_803F68F0_rodata`, "GAMECUBE" en +0x00) y no definía la
guarda, así que `bGetPlatformName()` internaba una **segunda** copia y `keep.lst`
la mantenía VIVA. Con la guarda, `dupstr` baja de 6 cadenas de más a 5 y los
15 primeros items de la ventana casan.

Lo mismo en `zAnim.cpp` (§3.6). Las dos exigen quitar su par de `keep.lst`; el
propio `lcfix` lo dice: `FALLO zPhysics: 'GAMECUBE' no tiene simbolo $LC propio`.

### 3.2 `zPhysics.cpp` · las nueve cabeceras de AttribSys, disueltas

Estaban las nueve juntas arriba (`milestonetypes, audioimpact, audioscrape,
engineaudio, audiosystem, camerainfo, explosion, speech, world`) y el objetivo
las suelta en **cinco sitios distintos**. Queda una sola, `simsurface.h`, porque
arrastra attribsys (los ocho `Attrib::*`) y su cadena de clase es la primera del
objetivo. Las demás las ponen los primers `a`…`e` en su tramo.

Comprobación de que se podían mover sin riesgo: **las 31 cadenas de ese prefijo
tienen 0 referencias** en el `.s` (`grep -c '\$LCn@'`). No las usa nadie; sólo
las mantiene vivas `keep.lst`.

### 3.3 `zPhysics.cpp` · el 0.5 suelto (asm crudo, 4 B)

Entre `"Reflection"` y `"SmackableParams"` el objetivo tiene un `3F000000` que
ninguna de nuestras funciones crea ahí. Va como `.4byte` crudo en un `asm()` de
fichero: sin `.globl`/`.type`/`.size` no es un símbolo y el estripado no lo ve.

### 3.4 `zPhysics.cpp` · el primer `e`: los 15 nombres de tuning (+308 B)

**La palanca grande.** `PAD_DEAD_ZONE`, `SKID_AUDIO_SCALE`, `ROLLING_RESISTANCE`,
`TYRE_DAMAGE_RADIUS`, `ENABLE_ROLL_STOPS_THRESHOLD`, `BASE_FRICTION_MASS`,
`SHRED_DRAG`, `MIN_BUTTON_VALUE`, `WHEEL_SPIN_EXTRA_RPM`,
`DAMAGE_SCALE_COLLISION`, `EMP_LIFETIME`, `MAX_WHEEL_SPIN_RATE_AI`,
`POST_BRAKE_ACCEL_COUNT/SCALE/MIN`. El objetivo los tiene en
`0x803F7120..0x803F7259` y **nuestro objeto no los emitía en ninguna sección**
(los daba `prefijotu.py`). Puestos detrás de `Wheel.cpp` caen exactos: `rodata`
pasó de −96 con ellos y desaparecieron del diff.

### 3.5 `zPhysics.cpp` · el bloque `lbl_803F748C..E4`, movido (+84 B en su sitio)

Estaba **al final del fichero** por la regla de la r31 (si el inicializador es
visible en el punto de uso, GCC pliega la carga y rompe la función) y sus 84 B
caían detrás de todo. Su único usuario es `PhysicsUpgrades.cpp`
(`grep -rl lbl_803F74` da dos ficheros: ése y `zPhysics.cpp`), así que ponerlo
**justo detrás de ese `#include`** respeta la regla y lo lleva a
`0x803F748C..0x803F74E8`. Dentro va el `"<unknown>"` de `0x803F74C0` como asm
crudo, porque una cadena de primer se volcaría con el pool y no entre dos
definiciones de dato.

### 3.6 `Explosion.cpp` · primer de `Attrib::Gen::explosion` (1 línea)

Al quitar `#include explosion.h` de arriba la cadena dejaba de existir y `lcfix`
daba `FALLO ... NO esta en su .rodata` —o sea, una entrada de `keep.lst` colgando
y apuntando a otro literal—. El objetivo la tiene en `0x803F6B80`, que es el
volcado **entre `Construct` y el constructor**, y a ese punto no se llega desde
`zPhysics.cpp`. Va un primer de una línea en `Explosion.cpp`, detrás de
`BIND_PHYSICS_FACTORY(Explosion)`. **Ese `.cpp` sólo lo compila `zPhysics.cpp`**
(comprobado con grep), así que no hace falta guarda.

Igual con `Attrib::Gen::world`, que necesita un primer antes de `Smackable.cpp`.

### 3.7 `zAnim.cpp` · `BWARE_PREFIX_GAMECUBE` (−128 B con sus keeps)

Mismo caso que §3.1: escribe el prefijo a mano en `lbl_803CF348` y no definía la
guarda. `zAnim` pasa de `rodata−1240` a `−1112` con esto más las 9 entradas de
`deadlink`.

## 4. El negativo, con su cifra: los 40 B que quedan

Lo que sigue mal en zPhysics es **una permutación, no un tamaño**: 23 cadenas
—`pvehicle`, `chassis`, `engine`, `induction`, `nos`, `tires`, `transmission`,
`gameplay`, `19.8.31`, `16.2.1`, `1.2.3`, `aivehicle`, `pursuit*`, `presetride`,
los cuatro `SMS_MESSAGE_%d`— salen en el **primer** volcado y el objetivo las
suelta repartidas entre el tercero y el último.

Las crea el bloque de `#include "Speed/Indep/Src/Interfaces/..."` +
`IMPLEMENT_LISTABLE` de `zPhysics.cpp:79..151`, que se parsea **antes** de
`SmackableTrigger.cpp`. Y por §1 un primer no puede retrasarlas.

**Moverlo funciona para el orden y cuesta una función.** Probado: bloque entero
detrás de `Explosion.cpp` →

    fncmp: 1 de 718 -- __static_initialization_and_destruction_0  7996 -> 8000 B

o sea 4 B de más en la lista de construcción. **Revertido**: la regla dice que
ninguna función puede empeorar. Es el trabajo que queda, y es de esa función:
hay que encontrar la posición del bloque que deja el orden de construcción
intacto (o partirlo, dejando las definiciones donde están y adelantando sólo
las cabeceras que no las necesitan).

El resto del residuo, ya menor:

| qué | B | por qué |
|---|---:|---|
| `SceneryOverrideConn` + `Pkt_SceneryOverride_Open` | 48 | van EN MEDIO del bloque `CarRenderConn`…`Pkt_Smackable_Service`, que sale entero del parseo de una sola cabecera: no hay punto de fichero donde meterlas |
| `GManager` | 12 | lo creamos antes de las funciones de `Bounds.cpp` y el objetivo lo suelta después: hace falta RETRASARLO |
| cuatro palabras a cero muertas (`0x803F7100` ×3, `0x803F725C`) | 16 | entradas de pool de 4 B de funciones que el enlazador tiró; sobreviven porque `4 & ~7 = 0` |
| `.bss` | 32 | frente aparte, no lo he tocado |

## 5. Las líneas de `keep.lst` — la receta, no la lista

Como cada primer desplaza los `$LC`, dar las líneas a pelo no sirve: hay que
resolverlas contra el objeto recién compilado. La receta, **verificada de una
sola pasada** (produce un fichero idéntico al que fui construyendo por etapas):

```
1. python scripts/build_direct.py zPhysics zAnim
2. quitar de keep.lst los DOS pares:
       # @lc zPhysics "GAMECUBE"   /  zPhysics.o:$LC58
       # @lc zAnim    "GAMECUBE"   /  zAnim.o:$LC58
   (lcfix ya los denuncia solo: «'GAMECUBE' no tiene simbolo $LC propio»)
3. python scripts/lcfix.py                      -> 82 CORRIGE, todas de estas dos
4. python scripts/deadlink.py zPhysics --keep >> config/GOWE69/keep.lst
   python scripts/deadlink.py zAnim    --keep >> config/GOWE69/keep.lst
5. python scripts/lcfix.py --check              -> tiene que salir LIMPIO
```

`lcfix.py --check` sobre el árbol AHORA MISMO da **82 `CORRIGE` y 2 `FALLO`**, y
los dos `FALLO` son exactamente los `GAMECUBE` del paso 2. No hay ninguno de
otra unidad: mis cambios no desplazan `$LC` fuera de zPhysics y zAnim.

El resultado del paso 4 son **47 cadenas nuevas** en zPhysics y **9** en zAnim,
y una borrada (`GAMECUBE`) en cada una. Las 47:

```
Attrib::Instance   Attrib::RefSpec   Attrib::Gen::simsurface  Attrib::Gen::pvehicle
Attrib::Gen::chassis  Attrib::Gen::engine  Attrib::Gen::induction  Attrib::Gen::nos
Attrib::Gen::tires  Attrib::Gen::transmission  Attrib::Gen::rigidbodyspecs
Attrib::Gen::smackable  Attrib::Gen::junkman  Attrib::Gen::brakes
16.1.0  16.2.1  1.2.3  1.8.1   ExplosionParams  VehicleParams  SmackableParams
MJumpCut  MGeneric  High  Medium  Low  Reflection  MAudioReflection
PlayerNum  Dist  Covered  GRaceStatus
PAD_DEAD_ZONE  SKID_AUDIO_SCALE  ROLLING_RESISTANCE  TYRE_DAMAGE_RADIUS
ENABLE_ROLL_STOPS_THRESHOLD  BASE_FRICTION_MASS  SHRED_DRAG  MIN_BUTTON_VALUE
WHEEL_SPIN_EXTRA_RPM  DAMAGE_SCALE_COLLISION  EMP_LIFETIME  MAX_WHEEL_SPIN_RATE_AI
POST_BRAKE_ACCEL_COUNT  POST_BRAKE_ACCEL_SCALE  POST_BRAKE_ACCEL_MIN
```

Queda un `keep.lst` completo y ya resuelto en
`…/scratchpad/r52_rodata1/keep_r52_rodata1.lst` por si se prefiere copiar el
bloque en vez de regenerarlo.

## 6. Verificación

    python scripts/fncmp.py Speed/Indep/SourceLists/zPhysics -> 0 de 718
    python scripts/fncmp.py Speed/Indep/SourceLists/zAnim    -> 0 de 315

    linkdelta con el keep.lst propuesto
      zPhysics  .text +0  rodata-40    bss-32
      zAnim     .text +0  rodata-1112  data-192  over-16

    linkdelta con el keep.lst del arbol (SIN el paso 2-5)   <- NO APLICAR ASI
      zPhysics  .text +0  rodata-1120  bss-32
      zAnim     .text +0  rodata-1312  data-192  over-16

    trypromo --ldflags "-strip-unused-data -keep <keep propuesto>" zPhysics
      -> DOL ROTO (67dc0681044a)

`.text` a `+0` en las dos, y su contenido enlazado sólo difiere en mitades `@l`
de direcciones: los pares que quedan son todos `orig-nuestro = ±0x1E0`/`±0x260`,
o sea el desplazamiento de la propia `.rodata`. No hay código distinto.

## 7. Herramienta que dejo ofrecida

En `…/scratchpad/r52_rodata1/` van cuatro sondas. Tres son de un solo uso, pero
`tokdiff.py` **no**: alinea la sección ENLAZADA de una unidad contra el DOL
original **por ITEMS** (cadena / palabra) en vez de por bytes, y es lo único con
lo que este frente se lee. `dolrod.py` hace el diff de bytes y sobre una `.rodata`
permutada saca 120 líneas de trozos de cadena partidos; `tokdiff` saca
`SOBRA Attrib::Gen::milestonetypes` / `FALTA Attrib::Gen::simsurface`. Si te
parece, va a `scripts/`.

    python tokdiff.py <unidad> [.rodata|.text|.data] --dol <dol nuestro>
    python link.py    <unidad> <salida> [--keep F]     # enlaza y deja .elf/.dol

## 8. Lo que he tocado

    src/Speed/Indep/SourceLists/zPhysics.cpp
    src/Speed/Indep/SourceLists/zAnim.cpp
    src/Speed/Indep/Src/Physics/Common/Explosion.cpp   (1 linea; TU exclusiva de zPhysics)

Nada más. No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt` ni
`keep.lst`; no hay commits ni `git add`; no se ha lanzado `ninja` ni
`configure.py`. Los `.elf`/`.dol`/`.rsp` de los ensayos están borrados.
