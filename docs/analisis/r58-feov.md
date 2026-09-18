# r58 — agente `feov`: zFeOverlay, zFe y zFe2

**Las TRES unidades del encargo quedan con las NUEVE secciones a `IGUAL`.** El frente pasa
de tres unidades sin deficit de seccion (zAnim, zMain, zPhysics) a **seis**.

    linkdelta       ANTES                DESPUES
    zFeOverlay      .text +0  over-8     .text +0  IGUAL
    zFe             .text +0  rodata-48  .text +0  IGUAL
    zFe2            .text +0  rodata-872 .text +0  IGUAL

    fncmp ANTES y DESPUES:  zFe 0 de 921, zFe2 0 de 1307, zFeOverlay 0 de 467.  CERO regresiones.
    sha1 sellado en TRES compilaciones seguidas, identico las tres:
       zFe.o         0ade8b9ba8de76aff5d2be4ece6ad7606b018ae1
       zFe2.o        328dec2240d89d7e6cebbfbbd1e2ba7fbf5c943f
       zFeOverlay.o  888d5c6c7f93d6dc513432285328b9e8e8626212
    trypromo: DOL ROTO (zFe 5afaa8201fb4, zFe2 8df18fee0d00, zFeOverlay a3a1d3536a97;
              los tres juntos 1533e9240128).
    lcfix.py --check: 166 pendientes al cerrar y NINGUNA es mia (1 zAnim, 94 zLua, 71 zMain).
    mangfix --check, gapchk (las tres), prefijochk y checksplits: limpios.
    config/GOWE69/*, splits.txt, keep.lst y configure.py SIN TOCAR.

| unidad | ELF enlazado distinto ANTES | DESPUES | ganado | `dolwhere` DESPUES |
|---|---:|---:|---:|---:|
| zFe | 338.704 | **29.130** | **−309.574** | 37.472 B |
| zFe2 | 409.176 | **37.129** | **−372.047** | 47.147 B |
| zFeOverlay | 5.266 | **5.042** | −224 | 6.211 B |
| **total** | **753.146** | **71.301** | **−681.845 B** | 90.900 B (los tres juntos) |

(«ELF enlazado distinto» = bytes que difieren, POR DIRECCION, entre el enlace con la unidad
sustituida y el enlace base, todas las secciones. `scripts/agent_feov58_metric.py`. El ANTES
se midio recompilando la version de HEAD de cada fuente en el arbol de HOY, no copiando
numeros de la r57: el arbol se ha movido bajo los pies durante la ronda — ver §6.1.)

**Y `dolwhere` funciona por primera vez en las tres.** Antes contestaba
`LAS SECCIONES NO COINCIDEN` en las tres, porque el deficit de seccion descolocaba las
SECCIONES DEL DOL (`.ctors`/`.dtors` en zFeOverlay; `.data`/`.sdata`/`.sdata2` en zFe y
zFe2). Eso no es «unos bytes»: mientras la tabla de secciones del DOL no case, la unidad no
puede promocionar **por construccion** y ninguna medida de contenido sirve.

---

## 1. zFeOverlay `over-8`: SI hay palanca para emitir detras de las vtables

El encargo decia «ocho bytes». Son exactamente estos, y ahora estan medidos byte a byte:

    objetivo   803C8B40  1.0f | 803C8B44  4000.0f | 803C8B48  0.5f | 803C8B4C  0.0f
               803C8B50  _overlay_end (8 B a cero)   -> .over acaba en 803C8B58
    nuestro    803C8B40  1.0f | 803C8B44  0.0f    | 803C8B48  4000.0f | 803C8B4C  0.5f
               (nada)                              -> .over acababa en 803C8B50

La r57 dejo `_overlay_end` **dentro del bloque de cabecera** con esta veda escrita:

> «No hay palanca de fuente para emitir detras de las vtables: ni `asm()` de fichero, ni
> definicion de C++, ni `static` con `__asm__()` (esta ni se emite).»

**La veda cae.** La palanca es la MISMA que la del hueco de `.bss` que la propia r57 uso tres
lineas mas abajo (`char _zFeOverlayBssTail[4104] asm("gap_07_804AB6B8_bss");`), con un
atributo encima:

```cpp
char _zFeOverlayOverlayEnd[8] __asm__("_overlay_end") __attribute__((section(".rodata")));
```

Por que funciona, con el fuente de GCC delante:

* una definicion **TENTATIVA** (sin inicializador) a nivel de fichero **no se emite en el
  punto del parseo**: `rest_of_decl_compilation` la difiere y sale por
  `wrapup_globals_for_namespace` (`cp/decl2.c:3798`), que dentro de `finish_file` corre
  **DESPUES del drenaje de `saved_inlines` de la vuelta y ANTES de las vtables de la vuelta
  siguiente**;
* sin `section` sale por `.lcomm` y cae en `.bss` (eso es lo que hace el hueco de la r57);
  **con `section` `varasm` no puede usar common** y la vuelca como `.space` en la seccion
  que le digas;
* el orden de los atributos importa: `asm()` va **antes** de `__attribute__`. Al reves es
  `parse error before 'asm'`.

Comprobado en 4 s con una TU minima (dos clases con vtable y una inline con literales):
el simbolo cae en `.rodata` **detras de las dos `_vt.`**, en la MISMA seccion — el objeto
tiene una sola `.rodata`, `SHF_ALLOC`; el `,"aw"` que escribe el compilador en el `.s` no
abre otra. La TU esta en el scratchpad (`t1.cpp`/`t2.cpp`).

Contrapartida: los 8 B que `_overlay_end` ocupaba en el bloque de cabecera hay que
rellenarlos para que NADA se mueva. En el objetivo son el final de
`"Attrib::Gen::emitterdata"` (803C6EAC = `0x64617461`) mas su relleno, asi que
`lbl_803c6a40` pasa de `0x46C` a `0x474` con esos bytes verbatim: mide lo mismo (1.140 B),
tapa 4 B mas de diferencia, y `keep.lst` lo protege por nombre igual que antes.

Resultado: `.over` 0x24998 -> **0x249A0**, `.ctors` de 803C8B50 a **803C8B60** y `.dtors` de
803C8C30 a **803C8C40**, que son las del objetivo.

### 1.1 De paso, medido: la ventana de `.over` NO es de 32 B, es de 16

El encargo decia «aplica la ventana de 32 si la `.over` esta alineada igual — compruebalo en
`ldscript.ld`». Comprobado: **no lo esta**. `.over` va entre `. = ALIGN(8)` y `. = ALIGN(16)`,
asi que la ventana de esa seccion es de **16 B**, no de 32. Con `.over` acabando en
803C8B50 (ya multiplo de 16) el `ALIGN(16)` no rescata nada y `.ctors` cae 16 B antes.

### 1.2 Un literal de coma flotante de 4 B sobrevive al estripado, y aqui se ve

`$LC770`/`$LC771`/`$LC772` **no estaban estripados**, al reves de lo que decia la r57: los
tres siguen en la imagen enlazada (4000.0f y 0.5f se leen en 803C8B48/4C). Lo que
`-strip-unused-data` se lleva de un simbolo muerto es `size & ~7`, y para 4 B eso es CERO;
lo unico que desaparece es el SIMBOLO de la tabla, no los bytes. Por eso `.over` medi­a
0x24998 y no menos.

Corolario util para todo el proyecto: **cualquier literal muerto de 4 B llega entero al
enlace sin pasar por `keep.lst`**; los de 8 B o mas, no. Y el resto de un simbolo muerto de
tamano N son sus **N & 7 bytes finales** (por eso el pool de zFe tiene «cadenas» sueltas como
`'ox.cpp'` o `'uction'`: son colas de cadenas muertas).

---

## 2. zFe `rodata-48` y zFe2 `rodata-872`: la jugada del bloque de cabecera, otra vez

`prefijotu` decia **0 cadenas que faltan** en las dos, asi que el deficit no era vocabulario.
Y hacia falta cerrarlo igualmente porque **`.data` va detras de `.rodata` con `ALIGN(32)`**:

    zFe    rodata-48   ->  .data en 80415160 (objetivo 80415180)   -32
    zFe2   rodata-872  ->  .data en 80414E20 (objetivo 80415180)  -864

Los dos ficheros ya tenian el prefijo de bWare/STL escrito a mano
(`lbl_803DF568` de 0x5C en zFe, `lbl_803E4380` de 0x5C en zFe2) y **los dos estan en
`keep.lst`, que protege por NOMBRE y no por TAMANO** (regla de la r57). Alargarlos con los
bytes verbatim del objetivo cierra el deficit sin tocar `keep.lst`, sin anadir ni quitar un
solo `$LC` y sin mover una instruccion:

| unidad | bloque | bytes anadidos | de donde | resultado |
|---|---|---:|---|---|
| zFe | `lbl_803DF568` 0x5C -> **0x8C** | 48 | 803DF5C4..803DF5F4 | `rodata-48` -> **IGUAL** |
| zFe2 | `lbl_803E4380` 0x5C -> **0x3BC** | 864 | 803E43DC..803E473C | `rodata-872` -> **IGUAL** |

No es contenido nuevo: son duplicados de cadenas que ya emitimos (`Attrib::Attribute`,
`Attrib::Instance`, ...). Es exactamente lo que la r57 hizo con la cabecera de zFeOverlay y
por lo que gano 439.477 B: **la seccion mide lo que debe y todo lo que va detras vuelve a su
direccion**. `fncmp` a 0 despues de cada uno.

**El −872 de zFe2 se habia quedado en −864 antes de esto** por las tres ediciones de §3, asi
que el bloque son 864 y no 872.

---

## 3. EL HALLAZGO: las cadenas de «nombres de clase» son el NOMBRE DE LA RESERVA, y el codigo es el mismo

La r57 dejo esto abierto en las tres unidades:

> «La familia de **nombres de clase** (`SelectablePart 1`..`14`, `QuickPlay`, `MSOption`, ...)
> es un mecanismo del original que no he identificado; si aparece, cierra la unidad casi
> entera.» (`r57-feov.md` §6.3)
>
> «entre las que faltan hay nombres de clase (`PostPursuitInfractionsScreen`,
> `RaceResultStat`, `LapStat`, `ConfirmDialog`, `InfoStat`, `BootFlowManager`...) que el
> arbol no genera.» (`r57-fe.md` §7)

**Es `BNEW`.** El macro de `bWare.hpp:64` es `new (__FILE__, __LINE__)`, y la sobrecarga que
llama es

```cpp
inline void *operator new(size_t size, const char *file, int line) { return new char[size]; }
```

que **descarta los dos argumentos**. O sea: la cadena es DATO MUERTO — se emite en la
`.rodata` y no la referencia ni una instruccion. Por eso el objetivo puede tener ahi una
cadena COMPLETAMENTE DISTINTA con **el mismo codigo**, y por eso `fncmp` daba 0 mientras el
pool no casaba.

En el original esos sitios llevan **el nombre del tipo**, no `__FILE__`. La prueba es
posicional y es 1 a 1:

| fuente | nuestra cadena | la del objetivo | direccion |
|---|---|---|---|
| `uiInfractions.cpp:173` `new(...) PostPursuitInfractionsScreen(sd)` | `src/.../uiInfractions.cpp` | `PostPursuitInfractionsScreen` | 803E58D0 |
| `FeMinimapStreamer.cpp:16` `new(...) ChoppedMiniMapManager(9)` | `src/.../FeMinimapStreamer.cpp` | `ChoppedMiniMapManager` | 803E4908 |
| `FEPKg_PostRace.cpp:1243` `new(...) PursuitResultsDatum(...)` | `src/.../FEPKg_PostRace.cpp` | `PursuitResultsDatum` | 803E5F70 |
| `FEDatabase.cpp:694` `new(...) Sound::stSongInfo` | `src/.../FEDatabase.cpp` | `Sound::stSongInfo` | 803E6EA8 |
| `feIconScrollerMenu.cpp:345` `new(...) FEScrollyBookEnd(...)` | `src/.../feIconScrollerMenu.cpp` | `FEScrollyBookEnd` | 803E86DC |

`Sound::stSongInfo` cierra la discusion: la cadena es el TIPO **tal y como esta escrito en el
fuente**, con su `::` incluido. Y `FEPKg_PostRace.cpp:1101` ya tenia en el arbol
`AddSlot(new ("", 0) PursuitResultsArraySlot(...))` — la forma correcta con el nombre
en blanco.

Y no todas son nombres de tipo: el objetivo tambien tiene `"JukeboxEntry[] - backup copy"`
(donde `uiEATraxJukebox.cpp:57` reserva `JukeboxEntry[NumSongs]`), `"Unnamed shape mem"`,
`"cFEngJoyInput::ActionQueue"`, `"Movie filename"`, `"MiniMap Chop"`. Son nombres de
reserva escritos a mano por el programador.

**Que se ha aplicado**, con `fncmp` a 0 y `lcfix --check` limpio despues:

    uiInfractions.cpp        new ("PostPursuitInfractionsScreen", __LINE__)   +8 B de linkdelta
    FeMinimapStreamer.cpp    new ("ChoppedMiniMapManager", __LINE__)          0 B, contenido correcto
    FEPKg_PostRace.cpp (x7)  new ("PursuitResultsDatum", __LINE__)            0 B, contenido correcto

Las tres juntas: **954 B** del ELF enlazado (409.176 -> 408.222 antes de §2).

**Que NO se ha aplicado, y por que** (regla 8; el negativo esta anotado junto a cada funcion,
que es donde `previo.py` lo encuentra):

    FEDatabase.cpp          new ("Sound::stSongInfo", __LINE__)   NEGATIVO
    feIconScrollerMenu.cpp  new ("FEScrollyBookEnd", __LINE__)    NEGATIVO

Las dos juntas costaban 24 B de `linkdelta` **y renumeraban los `$LC` de zFe2** (`lcfix`
abria 4 correcciones), que es lo que no se puede hacer con agentes midiendo en paralelo.
Revertidas.

### 3.1 La regla practica, para el dossier

* **Sustituir** `__FILE__` por un nombre es GRATIS en `$LC` (mismo numero de literales) y el
  codigo no cambia: `fncmp` a 0. Es la unica forma segura fuera de ventana.
* **Anadir** un nombre donde hoy no hay cadena (o hay `""`) crea un literal y **renumera**:
  eso es trabajo de ventana, con `lcfix` detras.
* GCC 2.95 **no deduplica** estas cadenas: en zFe2 `"PostPursuitInfractionsScreen"` quedo
  DOS veces (0x13CC y 0x3D55) sin que el numero de literales cambiase.
* La cadena es MUERTA, asi que del bloque solo sobreviven al enlace sus `N & 7` bytes
  finales. Por eso una sustitucion de 74 B por 29 B puede **subir** el tamano enlazado.

---

## 4. Lo que queda, medido

`dolwhere` con las tres sustituidas a la vez: **90.900 B**. Ya no hay ni un byte de deficit de
seccion; todo es **DONDE**.

| unidad | quedan | que es |
|---|---:|---|
| zFeOverlay | 6.211 B | **4.150 B de pool** (44 cadenas muertas que el objetivo tiene y nosotros no, §4.1) + ~2.000 B de punteros de `.data`/`.text` que apuntan a ellas + **9 B de orden de floats** (§4.2) |
| zFe | 37.472 B | orden de la `.rodata` dentro de su ventana: **130 de 157 vtables descolocadas** (`vtord`) y el pool de cadenas permutado; y las tablas de punteros de `.data` (`sMovieNameMap`, `MovieVolumeArray`, `FEAnyTutorialScreenName`) que los siguen |
| zFe2 | 47.147 B | lo mismo: `rodorden` da 940 cadenas del objetivo contra 948 nuestras con **648 en secuencia** |

### 4.1 zFeOverlay: las 44 cadenas, contadas y con su cuenta de bytes

`.over` mide exacto **por compensacion**: al objetivo le faltan de nuestro pool 44 cadenas
(664 B) y a nosotros nos sobran 44 duplicados del bloque de cabecera (668 B). El dia que se
emitan las 44, el bloque de cabecera tiene que ENCOGER lo mismo.

Las 44, por si sirven al que siga: `Attrib::Gen::frontend`, `FrontendRenderingCar`,
`SelectCarCameraMover`, `CustomizeMainOption`, `CustomizePartOption`, `CustomizePaintDatum`,
`FEShoppingCartItem`, `SetStockPartOption`, `SelectablePart 1`..`SelectablePart 14`,
`FEGeometryModels`, `ShoppingCartItem`, `SelectableTrack`, `TrackDirection`,
`ImageArraySlot`, `HUDLayerOption`, `HUDColorOption`, `DebugCarOption`, `TrafficLevel`,
`NumOpponents`, `SplitScreen`, `CustomRace`, `QuickPlay`, `ArraySlot`, `MSOption`,
`CarDatum`, `DebugCar`, `NumLaps`, `AISkill`, `CatchUp`, `19.8.1`, `_WHITE`.

Casi todas son nombres de tipo con vtable en la unidad, o sea §3. **`SelectablePart 1`..`14`
son catorce literales distintos**: catorce `BNEW SelectablePart(...)` con nombre numerado, o
un bucle desenrollado. `CarCustomize.cpp` tiene 12 sitios `BNEW SelectablePart` y
`CustomizeManager.cpp` 7 mas: 19 candidatos para 14 nombres.

Ninguna se ha tocado porque **todas son cadenas NUEVAS** (§3.1: renumeran) y ademas, al ser
muertas y de mas de 8 B, necesitan su linea de `keep.lst`. **Es trabajo de ventana.**

### 4.2 zFeOverlay: los 9 B de orden de floats, y por que no se han tocado

El pool del final sale de la cola de `finish_file`, y el orden lo fija **el orden en que se
parsean los cuerpos inline** (el orden de `saved_inlines`):

    nuestro   $LC767 1.0f (__static_initialization_and_destruction_0)
              $LC770 0.0f (Default__Q37Physics4Info11Performance, PhysicsInfo.hpp:35)
              $LC771 4000.0f, $LC772 0.5f (SetTime__5Timerf, Timer.hpp:103)
    objetivo  1.0f, 4000.0f, 0.5f, 0.0f  ->  SetTime va ANTES que Physics::Info::Performance::Default

Arreglarlo pide adelantar `Timer.hpp` sobre `PhysicsInfo.hpp` en el orden de parseo de la
unidad, y eso mueve completados de clase (o sea vtables) en una unidad que acaba de quedarse
a `IGUAL`. **Nueve bytes no pagan ese riesgo.** Anotado aqui y no probado.

---

## 5. Regresiones (regla 5)

**Ninguna cabecera tocada.** Los seis ficheros editados son `.cpp`/SourceList y los cinco que
no son mias SourceLists estan en **UNA sola unidad**, comprobado con `grep` sobre las
SourceLists:

    uiInfractions.cpp      -> solo zFe2.cpp
    FeMinimapStreamer.cpp  -> solo zFe2.cpp
    FEPKg_PostRace.cpp     -> solo zFe2.cpp
    FEDatabase.cpp         -> solo zFe2.cpp   (solo comentario)
    feIconScrollerMenu.cpp -> solo zFe2.cpp   (solo comentario)

`zFeOverlay.cpp`, `zFe.cpp` y `zFe2.cpp` son mias. **Cero unidades ajenas expuestas, cero
regresiones que medir.** `fncmp` de las tres a 0 antes y despues, y despues de CADA fichero.

---

## 6. Sorpresas

### 6.1 El arbol se mueve bajo los pies, y el `sha1` lo delata

Mi primera compilacion de zFeOverlay dio `a3afa07e...` y las tres siguientes `888d5c6c...`
**sin que yo tocase el fuente**. No es `build_direct` mintiendo: entre una y otra, otro
agente edito una cabecera que zFeOverlay parsea (`git status` lista 23 ficheros de
`Animation/`, `Gameplay/`, `Interfaces/`, `World/WorldConn.h`... que no son mios).

**Consecuencia para el protocolo**: los numeros «antes» de un informe anterior NO son
comparables con los «despues» de este. Todos los ANTES de este informe se han medido HOY,
recompilando la version de HEAD de cada fuente en el arbol de hoy
(`scripts/agent_feov58_antes.py head|mia`, con respaldo en el scratchpad).

### 6.2 `build_direct.py <u>` empareja por SUBCADENA

`python scripts/build_direct.py zFe` compila **tres** unidades (zFe, zFe2, zFeOverlay) y
contesta `3 ok`. Con agentes en paralelo eso es un pisotón silencioso al objeto de otro si
los nombres se solapan. No me afecto —las tres son mias— pero conviene saberlo.

### 6.3 `prefijotu` a cero no significa que la `.rodata` este bien

Las tres unidades dan «nos faltan 0 cadenas» y a las tres les faltaba seccion. `prefijotu`
compara CONJUNTOS: una cadena que el objetivo tiene una vez y nosotros tenemos en otro sitio
cuenta como presente. Para el deficit hay que comparar **multiconjuntos y bytes**
(`scripts/agent_feov58_pool.py`), y para el orden `rodorden`.

### 6.4 Las «cadenas» raras del pool son colas de cadenas muertas

`'ox.cpp'`, `'rd.cpp'`, `'uction'`, `'icMode'`, `'stems'`, `'bosfx'`... no son cadenas: son
los `N & 7` bytes finales que `-strip-unused-data` deja de una cadena muerta de N bytes. En
zFe son 15 restos, 91 B. Si se cuentan como cadenas, la contabilidad del pool sale mal.

### 6.5 `.over` no lleva ventana de 32

Ver §1.1. Cada seccion tiene la ventana que le da SU `ALIGN` en `ldscript.ld`, y hay de 8, de
16 y de 32. Para `.rodata` -> `.data` la ventana SI es de 32, y eso es lo que convierte
`rodata-48` de zFe en `.data -32`.

---

## 7. Propuestas para la ventana (con su medida)

1. **Nada de `keep.lst` esta vez.** Las tres unidades quedan a `IGUAL` sin tocarlo, y
   `lcfix --check` deja **0 correcciones mias** (166 pendientes, todas de zAnim/zLua/zMain).
2. **zFeOverlay, 44 cadenas + 44 lineas de `keep.lst`** (§4.1). Es la unica cosa que le queda
   de tamano y vale sus 4.150 B de pool mas los ~2.000 B de punteros. Hay que hacerlo en
   ventana porque renumera `$LC` y porque el bloque `lbl_803c6a40` tiene que encoger lo mismo
   que se emita.
3. **Repasar `BNEW` en TODO el arbol** (§3). El censo es facil: cada ruta `src/...` que
   aparezca en la `.rodata` de un objeto nuestro y no en la del objetivo es un `BNEW` cuyo
   nombre hay que escribir. En zFe2 eran cinco. **Ojo con la regla de §3.1**: sustituir es
   gratis, anadir renumera.
4. **El bloque de cabecera alargado es una palanca general**, no un truco de zFeOverlay:
   `keepfalta.py` (r57-fe §10) dio 258 huecos reservados sin escribir en 29 unidades, y
   TODAS las que tienen `rodata-N` con `N` pequeno tienen `lbl_` de prefijo ya en `keep.lst`.
   La receta es de dos lineas y cierra el deficit de seccion sin tocar `keep.lst`:
   `zWorld rodata-472`, `zWorld2 rodata-408`, `zCamera rodata-1384`, `zTrack rodata-352`,
   `zPlatform rodata-728`, `zGameplay rodata-136`, `zAI rodata-456`, `zEagl4Anim rodata-640`,
   `zLua rodata-720`, `zSpeech rodata-232`, `zPhysicsBehaviors rodata-40`. **No lo he
   extrapolado: lo he medido en DOS unidades (zFe y zFe2) y en las dos ha funcionado**; en
   las once de arriba es una hipotesis con dos casos a favor, y hay que contarla unidad por
   unidad (`nfsmw-extrapolar-frentes`).

## 8. Herramientas nuevas (quedan en `scripts/`, con prefijo `agent_feov58_`)

    agent_feov58_link.py     enlaza sustituyendo unidades; vuelca secciones o simbolos
    agent_feov58_metric.py   bytes distintos del ELF enlazado contra la base, POR DIRECCION
    agent_feov58_diff.py     diff por rangos de la imagen enlazada en un rango de direcciones
    agent_feov58_shift.py    donde CAMBIA el desplazamiento acumulado de una seccion
    agent_feov58_falta.py    simbolos del objetivo que nuestro objeto no define
    agent_feov58_pool.py     multiconjunto de cadenas del pool: objetivo contra nuestro
    agent_feov58_paths.py    las rutas __FILE__ muertas y QUE tiene el objetivo en su sitio
    agent_feov58_ref.py      quien referencia una direccion en el .text del ELF original
    agent_feov58_asm.py      compila una unidad a .s con sus cflags reales
    agent_feov58_mini.py     compila un .cpp suelto con esos cflags (para probar mecanismos)
    agent_feov58_antes.py    pone HEAD / mi version de mis dos fuentes, para medir el ANTES
