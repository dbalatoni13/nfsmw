# zSim: la `.data` cerrada byte a byte, y el mapa exacto de lo que queda

    al empezar   dolwhere 7.229 B    .text 1.982  .rodata 5.186  .data  61
    al acabar    dolwhere 5.925 B    .text 1.424  .rodata 4.500  .data   1   (-18,0 %)

    linkdelta zSim    +0  IGUAL          (intacto en todo el recorrido)
    measure           96.400/96.400 B, 402/402 funciones al 100 %
    lcfix --check     todas las entradas @lc estan al dia
    datacmp .data     348/348 B iguales (100 %) -- y el ORDEN de simbolos tambien

El unico byte que queda en `.data` es el puntero de `NisNamesToDisablePreculler`
(objetivo `0x804049AC`, nuestro `0x804049A0`): es una direccion de `.rodata`, no
un problema de `.data`. **La `.data` de zSim esta terminada.**

## La herramienta que faltaba: el orden de simbolos del objeto extraido

El encargo daba las ocho direcciones del objetivo, pero la palanca real es otra y
es de dos lineas: **volcar los simbolos de `.data` del objeto extraido por
desplazamiento da el ORDEN DE PARSEO del fichero original**, entero y sin
ambiguedad. GCC 2.9 emite `.data` en orden de definicion, asi que esa lista es
literalmente el indice del `.cpp` que hay que reconstruir.

    obj  0x00 mRenderFrame / mFrameTime / mWorkspace / mStackFrame
         0x10 gap_06_80435824
         0x14 Countable<SimTask>::_mCount      <- estaba al final del SourceList
         0x18 SimTask::mNextHandle
         0x1C Sim::SubSystem::mHead
         0x20 gap_06_80435834
         0x24 Sim::Internal::mUserMode          <- nosotros lo teniamos en 0x18
         0x28 Singleton<CDispatcher>::mInstance <- estaba al final del SourceList
         0x2C gap(8) / TheSurfaceClass / gap(4) / mUnknown / mNullSpec / gap(44)
         0x70 Tweak_GameBreaker* (x4)
         0x80 Countable<Sim::Object>            <- los cinco estaban al final
         0x84 Factory<Sim::Param, IEntity, UCrc32>
         0x88 Factory<const ConnectionData&, Connection, UCrc32>
         0x8C Factory<Sim::Packet*, int, UCrc32>
         0x90 Countable<Sim::Connection>
         0x94 gap(4) / bSawLoadingScreen / lbl_804358B0 / Tweak_GameSpeed / gap(12)
         0xB0 Singleton<INIS>::mInstance        <- estaba al final del SourceList
         0xB4 NisNamesToDisablePreculler        <- estaba DETRAS de mElapsedms
         0xB8 NISActivity::mElapsedmsAudioTime
         0xBC Tweak_EnableNISMomements

Script de un solo uso (esta en el scratchpad, merece subir a `scripts/`): leer la
tabla de simbolos, filtrar por `shndx == .data` y ordenar por `st_value`.

## Ensayos

### e1 -- `Countable<SimTask>` y el orden de `mUserMode` (Simulation.cpp)

El encargo decia que `IMPLEMENT_COUNTABLE(SimTask)` no compila delante de
`mStackFrame` porque la clase no esta declarada. Cierto, **pero la palanca no era
subir la clase: era BAJAR `mUserMode`.** El objetivo tiene `_mCount`,
`mNextHandle` y `mHead` DELANTE de `mUserMode`, y la clase `SimTask` ya esta
declarada donde se define `mNextHandle`.

- `IMPLEMENT_COUNTABLE(SimTask)` justo delante de `unsigned int SimTask::mNextHandle = 1;`
- el `asm` de `gap_06_80435834_data` y `static eUserMode mUserMode` bajan detras
  de `Sim::SubSystem::mHead`, reabriendo `namespace Sim { namespace Internal {`.

`mUserMode` solo se usa en `Sim::GetUserMode` y `Sim::Init` (mucho mas abajo), asi
que el movimiento es libre. **`.text` ni se entera.** Deja `0x00..0x24` exactos.

### e2 -- `Singleton<CDispatcher>` al final de Simulation.cpp

Va en `0x28`, entre `mUserMode` y el hueco de 8 B de SimSurface.cpp. `CDispatcher`
se declara en Simulation.cpp:~840, asi que `IMPLEMENT_SINGLETON` cabe al final del
fichero. Neutro en `.text` y `.rodata`. Deja `0x00..0x40` exactos.

### e3 -- los cinco de en medio, repartidos entre los `#include`

Van entre `Tweak_GameBreakerCollisionMass` (LocalPlayer.cpp) y el hueco de
QuickGame.cpp, o sea en el tramo SimActivity..SimModel. Como ninguno de esos siete
`.cpp` emite `.data`, basta escribirlos en `zSim.cpp` entre los includes, uno por
fichero y en el orden del objetivo:

    SimObject.cpp  -> IMPLEMENT_COUNTABLE(Sim::Object)
    SimEntity.cpp  -> Factory<Sim::Param, Sim::IEntity, UCrc32>
    SimEffect.cpp  -> Factory<const Sim::ConnectionData&, Sim::Connection, UCrc32>
    SimServer.cpp  -> Factory<Sim::Packet*, int, UCrc32>
    SimTypes.cpp   -> IMPLEMENT_COUNTABLE(Sim::Connection)

Todos los tipos ya estan completos en esos puntos. Neutro en `.text`/`.rodata`.

### e4 -- `Singleton<INIS>` y el orden interno de NISActivity.cpp

`IMPLEMENT_SINGLETON(INIS)` se va a NISActivity.cpp (que ya incluye `INIS.h` en su
linea 79), delante de los tres globales; y ademas **`NisNamesToDisablePreculler`
va DELANTE de `mElapsedmsAudioTime`**, no detras. Con esto la `.data` queda
completa: `dolwhere` de 61 B a 1 B en esa seccion, y ~390 B de `.text` (las
decenas de `addi rX,rX,0x58c4` contra `0x5954` repartidas por AI, Camera, ICE,
NIS...) se cierran solas.

### e5 -- las cadenas muertas que el enlace se lleva, y la REGLA que las gobierna

Con la `.data` cerrada, todo lo que queda es `.rodata`. La primera diferencia era
que `-strip-unused-data` se lleva literales que el objetivo SI tiene. La regla,
que esta en el docstring de `lcfix.py` y ahora esta medida en cuatro casos, es:

> de cada simbolo de dato MUERTO el enlazador quita **`size & ~7`**, no `size`.

    $LC481 "FEngHUD"       size  8 -> quita  8, deja 0   (hueco limpio)
    $LC514 "SimModel"      size  9 -> quita  8, deja 4   (RESIDUO de 4 B)
    $LC552 "MNISComplete"  size 13 -> quita  8, deja 8   (sobrevive "lete\0\0\0\0")
    $LC631 "MNotifySimTick" size 15 -> quita 8, deja 8   (sobrevive "imTick\0\0")

De ahi salen las dos formas de arreglarlo, y **la eleccion entre ellas no es de
gusto**:

- Si `size & ~7 == size` el hueco es limpio: basta **escribir la cadena a mano sin
  `.size`** en su posicion; el `$LC` de GCC se sigue estripando y el bloque cae
  exacto. Asi entro **"FEngHUD"** (8 B, detras de `LocalPlayer::SetHud`).
- Si deja residuo hay que quitarlo. Para **"SimModel"** (SimModel.cpp:278) el
  camino obvio era `HAND_POOL_TAG("SimModel")` -> el literal desaparece. **Funciona
  y es una TRAMPA: ver abajo.** La forma buena es **rellenar el literal hasta un
  multiplo de 8** — `::new ("SimModel\0\0\0\0\0\0\0", 0)` mide 16 B y se estripa
  entero — y escribir la cadena buena a mano en el hueco 3 de `zSim.cpp`.
  La etiqueta no la usa el codigo (la unica reubicacion a su `$LC` esta en
  `.debug`: el `operator new` de colocacion se expande en linea y la descarta),
  asi que ni el relleno ni el `HAND_POOL_TAG` mueven una instruccion. `fndiff` de
  `PlayEffect__Q23Sim5Model...` y `SetHud__11LocalPlayer...`: 100 % las dos.

### La trampa: quitar un literal RENUMERA todos los `$LC` de detras

`HAND_POOL_TAG("SimModel")` borra `$LC514`, y con el se corren en uno **todos** los
`$LC` posteriores de la unidad. `keep.lst` nombra seis literales de zSim por su
`$LC` (`"NISName"`, `"MPerpBusted"`, `"Perpetrator"`, `"ISimable"`, `"SimTime"`,
`"TimeStep"`), asi que a partir de ese momento cada entrada salvaba **la cadena de
al lado**. Medido:

    con keep.lst desfasado (lo que da HAND_POOL_TAG):  dolwhere 5.437 B
    con keep.lst al dia    (lo que hay que entregar):  dolwhere 5.925 B

O sea que el desfase **mejoraba la cifra 488 B por accidente** — se perdian
"NISName" y "SimTime" y a cambio se salvaba "MNotifyMovieFinished". Es exactamente
la rotura silenciosa de `nfsmw-lc-se-desplaza`, pero con el signo al reves, que es
peor: nadie la habria mirado. **La entrega lleva el relleno a 16 B en vez del
`HAND_POOL_TAG` justamente para que `lcfix --check` salga limpio** y la cifra sea
la honesta.

**Regla nueva, para cualquier unidad:** si vas a QUITAR un literal de una unidad
que tiene entradas `$LC` en `keep.lst`, no lo quites — rellenalo a multiplo de 8.
Y pasa `lcfix.py --check` DESPUES de cada ensayo que toque literales, no solo al
final: si sale `CORRIGE`, tu ultima medida no es comparable con la anterior.

### e6 -- el relleno de la r36c estaba en otro sitio del que decia

El bloque `lbl_80405BB0`/`BC0`/`BC4` de la r36c no cae en `0x80405BB0`: **un `asm`
de ambito de fichero se emite ANTES de las vtables** (que salen en `finish_file`),
asi que aterrizaba en `0x80404B6C`, detras del pool de GameplayActivity, donde el
objetivo solo tiene una palabra a cero. Era relleno de tamano, no colocacion.
Recortado a lo que hace falta para que `linkdelta` siga en `+0`.

Consecuencia general: **no hay forma de colocar dato a mano DETRAS de las vtables
de una TU con un `asm()`**. Lo que falte ahi (hoy, 8 B en `0x80405BC4`) solo se
puede compensar en tamano, no en posicion.

### e8 -- reconstruir un literal por delante

`$LC631` ("MNotifySimTick", 15 B) deja viva su COLA (`"imTick\0\0"`). Poniendo un
bloque de 8 B (`"MNotifyS"`) **delante del `#include` de `MNotifySimTick.h`** —
cc1plus emite las cadenas al crearlas, o sea al parsear la cabecera — prefijo +
cola reconstruyen `"MNotifySimTick\0\0"` exacto. Y el bloque de 20 B que la c36an1
escribia ahi ("SimTime"/"TimeStep") **duplicaba** dos cadenas que `keep.lst` ya
salva por su `$LC`: fuera. El tramo `0x80404B40..0x80404B64` queda byte a byte
(desplazado 8 B por lo que falta mas arriba).

## Lo que queda, medido y localizado

`.rodata` 4.500 B + `.text` 1.424 B. Son **tres frentes**, y los dos primeros
valen mas que todo lo que he cerrado hoy:

### 1. El orden de `.text` del bloque de plantillas -- ~2.754 B, FUERA DE MI TERRITORIO

Seis vtables tienen punteros mal (`_vt.Pkt_Effect_Send` 836 B,
`_vt.CAnimMomentScene` 820 B, `_vt.INIS` 484 B, `_vt.Sim::Activity.IAttachable`
344 B, `_vt.Sim::Attachments` 218 B, `_vt.Pkt_Effect_Open` 52 B) porque apuntan a
funciones de plantilla nuestras colocadas en otro sitio. `textorder.py zSim`:
**21 descolocadas, 7 saltos**, y son un solo bloque:

    OBJETIVO                              NUESTRO
    OnGrowRequest V<Act,Ent,Mod>          OnGrowRequest V<Act,Ent,Mod>
                                          _._FixedVector<Act>,<Ent>,<Mod>   <-- 540 B ADELANTADOS
    [FV accesores Mod, Ent, Act]          [FV accesores Mod, Ent, Act]
    GetGrowSize V<Act,Ent,Mod>            GetGrowSize V<Act,Ent,Mod>
    _._V<Act>, _._FV<Act>                 _._V<Act>, _._V<Ent>, _._V<Mod>
    _._V<Ent>, _._FV<Ent>
    _._V<Mod>, _._FV<Mod>
    GetMaxCapacity V<Mod,Ent,Act>         GetMaxCapacity V<Mod,Ent,Act>

Todo lo demas coincide. El objetivo **empareja** `_._Vector<X>` con
`_._FixedVector<X>`; nosotros emitimos los tres `_._FixedVector` juntos y mucho
antes. Es la misma firma que la c34ord3 arreglo para `_Storage` con
`UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR`, y el sospechoso es el destructor declarado
en clase de `UTLVector.h:336` (`~FixedVector() override { Vector<T,A>::clear(); }`).
El experimento es una guarda gemela en **`src/Speed/Indep/Libs/Support/Utility/UTLVector.h`**
(declarar el destructor en clase y DEFINIRLO fuera, o al reves), encendida solo
desde `zSim.cpp`. Esa cabecera no esta en mi lista, asi que lo dejo montado: es el
premio grande y es una guarda de tres lineas.

### 2. Las cadenas muertas de `Generated/Messages` -- ~930 B, FUERA DE MI TERRITORIO

Faltan 8 B de `"MNISComplete"` (`0x80404904`) y 24 B de `"MNotifyMovieFinished"`
(`0x80404958`), y por ellas todo el tramo `0x80404868..0x80404B70` sale desplazado.
Sus `$LC` miden 13 y 21 B: `size & ~7` deja residuo. La receta ya esta probada
(e5/e8) y son dos lineas, pero hay que **rellenar los literales a multiplo de 8 en
`src/Speed/Indep/Src/Generated/Messages/MNISComplete.h` y
`.../MNotifyMovieFinished.h`** (o bien anadir `# @lc zSim "MNotifyMovieFinished"`
a `keep.lst`, que es `config/`). Cualquiera de las dos vias necesita recortar el
relleno de `zSim.cpp` en la misma cantidad para no mover `linkdelta`.

### 3. Cosas que SI estan en el territorio de zSim y no me ha dado tiempo

- **`_vt.Sim::Connection` (40 B) esta 9 vtables tarde.** El objetivo lo emite
  justo detras de `_vt.Sim::Entity` (indice 50); nosotros detras de
  `_vt.Sim::Object` (indice 59). Vale 80 B directos y es orden de emision de
  vtable, no tamano. `SimConn.h` / `SimTypes.h` estan en territorio.
- **`"Param"` (8 B) sobra y `"IntroNisBL09"`/`"NISActivity"` van al reves** en
  `0x804049A0`. El literal de "NISActivity" sale de `NISActivity.cpp:305`
  (`return "NISActivity";`) y el de "IntroNisBL09" de la linea 415: en el
  original el array `NisNamesToDisablePreculler` se parsea ANTES de esa funcion.
  Subirlo arregla `.rodata` y **no rompe `.data`** (sigue siendo el primero de los
  tres globales de ese fichero).
- **`lbl_80404864`, la veda de la r36c.** Sigue abierta, pero hay una tercera
  forma sin probar que no necesita definir un objeto: **una asignacion de
  ensamblador relativa**, `lbl_80404864 = . - 12`, en un `asm()` colocado justo
  detras de `QuickGame::OnManageTime` (donde `.` en `.rodata` es el final del pool
  de 16 B de esa funcion). Define el simbolo sobre el `3f800000` que YA esta en el
  pool, sin emitir bytes y sin inicializador visible en el punto de uso. Vale los
  4 B que sobran en `0x80404904`. No la he medido.
- El orden del pool de `OnManageTime` (`{1.0f, 0.0f, 0.01f}` contra el nuestro
  `{1.0f, 0.01f, 0.0f}`) sigue costando 8 B y sigue sin compensar: el comentario
  medido de la r36c en QuickGame.cpp:425 aguanta.

## Ficheros tocados

    src/Speed/Indep/SourceLists/zSim.cpp
    src/Speed/Indep/Src/Sim/Common/Simulation.cpp
    src/Speed/Indep/Src/Sim/Common/SimModel.cpp
    src/Speed/Indep/Src/Sim/Entities/LocalPlayer.cpp
    src/Speed/Indep/Src/Sim/Activities/NISActivity.cpp
    src/Speed/Indep/Src/Sim/Activities/GameplayActivity.cpp

Nada de `config/`, nada de `splits.txt`, sin commits.
