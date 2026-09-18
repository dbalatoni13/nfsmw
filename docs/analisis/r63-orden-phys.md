# r63 — orden-phys: el destructor IMPLÍCITO sale el PRIMERO de la cola, y zPhysics baja 5.736 B de DOL

Encargo: `zPhysics` y `zPhysicsBehaviors`, en exclusiva. **Ninguna promociona** (las dos
siguen `DOL ROTO`), pero las dos bajan y hay **un mecanismo nuevo, medido y general**.

```
sellos (TRES compilaciones cada uno, regla 6)
  zPhysics           2b7c2b0422c9484963195ec317c9e7e712570f47  x3
  zPhysicsBehaviors  dd641fc7b7c28be0c0725a02928a416f61787803  x3

                        ANTES (HEAD 41c2bc13)          DESPUES
  zPhysics
    textorder           355/719 desc, 169 saltos       326/719 desc, 149 saltos
    fncmp               0 de 718                       0 de 718
    linkdelta           .text +0  resto IGUAL          .text +0  resto IGUAL
    dolwhere            35.673 B                       29.937 B      (-5.736 B, -16,1 %)
    reorden .text       0 palabras de CONTENIDO        (sin cambio: no falta codigo)
    reladdr             36 direcciones distintas       36            (sin regresion)
    trypromo            DOL ROTO                       DOL ROTO
  zPhysicsBehaviors
    textorder           817/1121 desc, 50 saltos       811/1121 desc, 38 saltos
    fncmp               1 de 1120 (UpdateLoaded 856B)  1 de 1120     (sin cambio)
    linkdelta           .text +4  resto IGUAL          .text +4  resto IGUAL
    dolwhere            NO MEDIBLE (secciones no coinciden: obj 803A41B8 / nue 803A41C0)
    reladdr             30 direcciones distintas       30            (sin regresion)

lcfix --check   21 correcciones pendientes: 20 de zWorld2 y 1 de zEagl4Anim.
                CERO mias. No he movido una sola cadena, ninguna es venenosa para nadie.
checksplits     LIMPIO (0 solapes, 0 cortes)
configure.py / config/GOWE69/* / splits.txt / symbols.txt:  NO TOCADOS. Nada que proponer.
```

---

## 1. El hallazgo: **un destructor IMPLÍCITO sale el PRIMERO de la cola de `finish_file`; uno declarado EN CLASE sale con sus hermanos**

`textorder` decía que la primera función del bloque diferido del objetivo es
`_._16PlaceableScenery` (rango 359, justo detrás del
`__static_initialization_and_destruction_0`) y que nosotros la poníamos en el **499**,
pegada a `Destroy__16PlaceableScenery` (504) y `OnRemoveOffScreen` (505) — **las tres
seguidas y en orden de declaración**, que es exactamente lo que hace GCC 2.9 con los
cuerpos escritos DENTRO de la clase.

`SmokeableInfo.hpp` las tenía las tres en clase:

```cpp
~PlaceableScenery() override {}                   // linea 150
void Destroy() override { ... }                   // linea 156
bool OnRemoveOffScreen(float dT) override { ... } // linea 159
```

Quitando **sólo** la declaración del destructor (la base ya lo tiene virtual, así que el
implícito sale virtual igual), el cuerpo es **byte a byte el mismo** (`fncmp` sigue en
`0 de 718`) y la función **salta al rango 359**, que es donde el objetivo la tiene.

| medida | con `~PlaceableScenery(){}` | implícito |
|---|---:|---:|
| descolocadas | 355 | 352 |
| saltos de delta | 169 | 167 |
| **dolwhere** | **35.673 B** | **34.975 B** |

**-698 B por borrar una línea.** Y con el resto de la ronda encima el mismo cambio vale
**-3.324 B** (33.261 → 29.937): no es aditivo, es multiplicativo, porque coloca la cabeza
de la cola y todo lo que viene detrás deja de arrastrar.

### Cómo se aplicó, y por qué NO es «tocar una cabecera compartida»

`SmokeableInfo.hpp` la incluyen cuatro `.cpp` y dos no son míos (`Main.cpp` → zMisc,
`World.cpp` → zWorld). Así que va con **guarda inerte**, el idioma que el propio árbol ya
usa (`ICAUSE_FWD_IEXPLOSION` r57, `EVENTSEQ_FWD_IENGINE` r51, `SIMCOLLISIONMAP_CTOR` r60):

```cpp
#ifndef PLACEABLESCENERY_IMPLICIT_DTOR
    ~PlaceableScenery() override {}
#endif
```

y `#define PLACEABLESCENERY_IMPLICIT_DTOR` **sólo** en `zPhysics.cpp`. Sin el macro el
flujo de tokens es idéntico.

**Los dos controles que TENÍAN que salir y salieron:**

1. `_._16PlaceableScenery` **no existe** — ni en el `.o` objetivo ni en el nuestro — en
   `zMisc`, `zWorld` ni `zPhysicsBehaviors`. Sólo en `zPhysics`. Leído del symtab de los
   ocho ELF. O sea que allí no puede cambiar ni un byte.
2. `zPhysicsBehaviors.o` (que incluye esa cabecera vía `DrawVehicle.cpp`) da el **mismo
   sha1** antes y después de editar la cabecera: `40c676ecf3d9fc03a098b326f654ecf05a4a77ec`.

### Por qué es general

El bloque diferido de una unidad se emite así (`decl2.c:3643`, `finish_file`): en cada
vuelta van primero `instantiate_pending_templates`, luego las vtables, luego el
`__static_initialization_and_destruction_0`, luego el lazo que **sintetiza** los
`DECL_ARTIFICIAL` de `saved_inlines`, y por último `wrapup_global_declarations` sobre ese
mismo array. **Un cuerpo escrito dentro de la clase entra en `saved_inlines` cuando se
acaba de parsear la clase, en orden de declaración; un método implícito entra por otra
puerta y sale delante.** Es la pareja de la regla ya conocida
(`nfsmw-en-clase-es-inline`): *en clase* decide si SE EMITE, *implícito* decide DÓNDE.

**Se busca a un grep**: cualquier `~Clase() override {}` o `~Clase() {}` vacío en una
cabecera cuya unidad tenga el bloque diferido descolocado.

---

## 2. zPhysics: de 169 saltos a 149 sin escribir una línea de código

`reorden.py` ya decía lo importante: **`.text` — 717 símbolos, 34.532 palabras, `0`
palabras de CONTENIDO real**. No falta nada que escribir; todo es orden. Y todo el
desorden vive en el bloque diferido: los rangos 0..358 (el código normal) estaban ya a
delta `+0` los 359.

Las cuatro palancas, en el orden en que se aplicaron (y ése es el orden que importa:
*coloca primero la que va más arriba en el orden del objetivo*):

| # | palanca | desc. | saltos | dolwhere |
|---|---|---:|---:|---:|
| — | base (HEAD) | 355 | 169 | 35.673 |
| 1 | `PLACEABLESCENERY_IMPLICIT_DTOR` | 352 | 167 | 34.975 |
| 2 | `#include ISimable.h` delante del bloque de interfaces | 353 | 167 | 34.851 |
| 3 | `+ IEntity.h, ITriggerableModel.h, IRenderable.h, IExplodeable.h` | 341 | 162 | 32.566 |
| 4 | `ICAUSE_FWD_IEXPLOSION` (la guarda ya existía, r57-zai) | 340 | 160 | — |
| 5 | reordenar el bloque de interfaces al orden de la cola del objetivo | 334 | 152 | 31.001 |
| 6 | `UTL_IMPLEMENT_NO_COPY_INIT` **+ quitar el `= T()` a las 7 estáticas escritas a mano** | **326** | **149** | **29.937** |

Detalle de cada una:

**(2)-(3) y (5).** La cola del objetivo empieza así: `ISimable`, `Sim::IEntity`,
`ITriggerableModel`, `Vector<IModel>::push_back`, `IRenderable`, `IExplodeable`,
`IExplosion`, `Vector<IExplosion>::push_back`, `pvehicle::operator=`,
`Vector<IInputPlayer>`, `IRaceEngine`, `IVehicle`, `Vector<ICollisionBody>`, `IBody`,
`ISimpleBody`, `Vector<ISimpleBody>`, `Vector<IRigidBody>`, `IDisposable`,
`IPlaceableScenery`, `Vector<IRecordablePlayer>`, `Vector<ISpikeable>`,
`EventSequencer::IContext`. El bloque de `#include` de `zPhysics.cpp` está ahora **en ese
orden**, y con ello los rangos 365..385 caen a delta `+0`.

**(4).** `ICause.h` arrastra `IExplosion.h` entero, y `ISimable.h` incluye `ICause.h` en
su línea 8: eso completaba `IExplosion` **antes** que `ISimable` y ponía su grupo el
primero de la cola cuando el objetivo lo tiene el octavo. La guarda `ICAUSE_FWD_IEXPLOSION`
ya existía desde la r57 (la puso zAI por el mismo motivo) y sólo había que encenderla.

**(6).** La nota r50-orden de `UCollections.h`/`UListable.h` explica que `X Y::z = X();`
es copia-inicialización: GCC sintetiza el constructor de copia, lo emite **a mitad de
unidad** y ahí referencia `_vt.FixedVector<...>`, lo que adelanta esa vtable a la primera
vuelta de `finish_file`. **La guarda sola no hacía nada** (Physics no usa
`IMPLEMENT_INSTANCABLE`/`IMPLEMENT_LISTABLESET` por macro): las siete definiciones que
importan están **escritas a mano** en `zPhysics.cpp`, en la forma
`template <> ...::_mList = ...::_List();`. Pasadas a definición por defecto
(`...::_List ...::_mList;`, sin `template<>` y sin inicializador) valen 8 descolocadas y
3 saltos. Mismo `.bss`, misma llamada al constructor, `linkdelta` sigue `IGUAL`.

### Lo que queda en zPhysics, y es UNA sola causa

Los 149 saltos que quedan son, en su mayor parte, **un solo bloque**: los
`_._Q23UTLt11FixedVector3Z<T>` salen **entre 121 y 135 rangos ANTES** de donde el objetivo
los pone (nuestro 539..564, objetivo 660..699), y los
`_._Q43UTL11Collectionst8Listable2Z<T>_4List` entre 98 y 134 **DESPUÉS**.

Y la forma del objetivo dice cuál es el mecanismo: allí `~FixedVector<T>` va **pegado
detrás de `~Vector<T>`, tipo por tipo** (659 `~Vector<IExplosion>`, 660
`~FixedVector<IExplosion,96>`, 661 `~Vector<IDisposable>`, 662
`~FixedVector<IDisposable,160>`…), que es exactamente el orden de COMPLETADO de las clases
(`FixedVector` deriva de `Vector`). Nosotros los sacamos **en bloque y mucho antes**: la
firma de que algo sigue referenciando `_vt.FixedVector<...>` durante el parseo y le mete
la vtable en la primera vuelta. Quitado el `= T()` de las siete estáticas, ese disparador
sigue vivo y **no lo he encontrado**. Es la próxima diana de esta unidad: vale unos 26
símbolos de 180 B más los `~Listable::List`.

---

## 3. zPhysicsBehaviors: de 50 saltos a 38, y todo el código normal ya en su sitio

Cuatro ediciones, ninguna toca una instrucción:

1. **`RigidBody.cpp`** — `SetAnimating` estaba delante de `ResolveGroundCollision`; el
   objetivo las tiene al revés (rangos 53/54). Permuta adyacente pura.
2. **`SimpleRigidBody.cpp`** — el objetivo emite `Construct`, `Volatile::Volatile`,
   ctor, dtor (89..92); nosotros ctor, dtor, `Construct`, `Volatile`. Movidas las dos
   primeras delante del constructor.
3. **`SuspensionSpline.cpp`** — `Tire::UpdateLoaded` va **antes** de
   `SuspensionSpline::Construct` (objetivo 364/365), no detrás del destructor.
4. **`zPhysicsBehaviors.cpp` — `ATTRIB_NO_INLINE_CLASSKEY` RETIRADO**, que es la receta
   de zAI en la r56 aplicada aquí por primera vez. El objetivo emite los diez
   `ClassKey__Q36Attrib3Gen<X>` **dentro** de la cola de `finish_file` (rangos 623..631 y
   775); un cuerpo fuera de línea al final de `DrawVehicle.cpp`/`SpikeStrip.cpp` sale en
   el **punto del parseo** y no puede caer ahí. Medido antes de retirarlo: los nueve de
   SpikeStrip en 579..587 y el de `effects` en el 501. Los bloques
   `#ifdef ATTRIB_NO_INLINE_CLASSKEY` de esos dos `.cpp` quedan **inactivos a propósito**
   (igual que `Gps.cpp` en zAI).

Con eso **todo el código normal de la unidad (rangos 0..607) está a delta `+0`**, salvo
el `+4` que arrastra `UpdateLoaded`. Los 38 saltos que quedan son 1 (ese `+4`) + 37 del
bloque diferido.

**Efecto colateral medido de (4)**: aparecen dos símbolos WEAK que el objetivo tiene
UNDEFINED en esta unidad — `ClassKey__Q36Attrib3Gen8pvehicle` y
`ClassKey__Q36Attrib3Gen14rigidbodyspecs` (24 B, los define zPhysics). **El enlazador se
los lleva**: `linkdelta` sigue `.text +4 / resto IGUAL` antes y después. Queda anotado por
si alguna vez deja de ser cierto.

---

## 4. `UpdateLoaded__Q217SuspensionTraffic4Tire`: NO reabierta, y es una corrección de auditoría

El encargo la daba como «familia A, que salió VIVA», citando del decisor de la r61 la
línea `COPY-PROP: Replacing reg 625 in insn 1210 with reg 753`. **Esa línea no es de esta
función**: es de `ActualReadJoystickData` (zPlatform), que es donde el mecanismo se cerró
(`docs/analisis/r61-plat.md` §1). El volcado FRESCO de la r61 **sobre esta función** dice
literalmente «**CERO líneas COPY-PROP, igual que en la r54**» — y `COPY-PROP` es el paso
(3) de la cadena que fabrica el `lis r30`; sin él no hay preservado.

La ficha de vedas (`r60b-auditoria-vedas.md`) es **anterior** al volcado de la r61 y la
r61 la reclasificó a **VEDA DURA con cita** (`cse.c:7191`; `rs6000.h:2513`
`CONST_COSTS case HIGH: return 0`; `cse.c:8508` `cse_end_of_basic_block`), con 30 formas
de fuente medidas en la r54 más las nuevas de la r61. **Manda la r61.** Lo he anotado
junto a la función para que `previo.py` lo encuentre antes de que otra ronda lo repita.

Lo que sí cambia: esos 4 B son ahora **el único obstáculo** para poder medir el DOL de
`zPhysicsBehaviors` — `dolwhere` se niega con `LAS SECCIONES NO COINCIDEN: obj 803A41B8 /
nue 803A41C0`, que son los `+4` de `.text` redondeados a 8. Y sigue siendo lo único que
`fncmp` ve en 1.120 funciones.

---

## 5. Negativos medidos esta ronda (anotados también en el fuente)

| ensayo | resultado | veredicto |
|---|---|---|
| `EVENTSEQ_FWD_IENGINE` en zPhysics | coloca `EventSequencer::IContext` donde el objetivo lo tiene y baja a **335 desc / 151 saltos**, pero `linkdelta` pasa a **`.rodata -120`** | **NEGATIVO** |
| ídem + `#include EventSequencer.h` explícito antes de `PhysicsObject.h` | **335 / 151**, `linkdelta` **`.rodata -56`** | **NEGATIVO** |
| bloque de tablas estáticas movido al FINAL de `zPhysics.cpp` | **360 desc** (de 326) y **`.text +4`** | **NEGATIVO en las dos** |
| `UTL_IMPLEMENT_NO_COPY_INIT` **solo** | cero cambio (Physics no usa los macros) | inerte; lo que vale es el `= T()` a mano |

El primero es el interesante y merece una regla: **una palanca de orden que baje
`.rodata` es negativa aunque mejore `textorder`.** Perder `.rodata` que el objetivo tiene
mueve el DOL (`nfsmw-secciones-sin-comparar`); ganar posiciones de `.text` sólo lo mueve
si además el `.rodata` no se toca. Aquí `.text` mejoraba 5 funciones y `.rodata` perdía
120 B: el saldo es rojo.

---

## 6. Ficheros tocados

```
src/Speed/Indep/SourceLists/zPhysics.cpp                    (mio)
src/Speed/Indep/SourceLists/zPhysicsBehaviors.cpp           (mio)
src/Speed/Indep/Src/Physics/Behaviors/RigidBody.cpp         (solo zPhysicsBehaviors)
src/Speed/Indep/Src/Physics/Behaviors/SimpleRigidBody.cpp   (solo zPhysicsBehaviors)
src/Speed/Indep/Src/Physics/Behaviors/SuspensionSpline.cpp  (solo zPhysicsBehaviors)
src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp (solo comentario: negativo de UpdateLoaded)
src/Speed/Indep/Src/Physics/SmokeableInfo.hpp               (COMPARTIDA -- guarda INERTE, ver 1, con sus dos controles)
```

Sin commit. `configure.py`, `config/GOWE69/*`, `splits.txt` y `symbols.txt` **no tocados**
y **sin nada que proponer**: ninguna palanca de esta ronda necesita un `pad_`, un rango
nuevo ni una entrada de `keep.lst`.
