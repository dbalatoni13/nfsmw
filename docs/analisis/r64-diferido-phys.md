# r64 — diferido-phys: el bloque diferido era 85 % PLANTILLA, y lo que lo desordenaba eran unos constructores de copia que el objetivo no tiene

Encargo: `zPhysics` y `zPhysicsBehaviors`, en exclusiva. **Ninguna promociona**, pero
zPhysics baja **4.278 B de DOL** (−14,3 %), pierde **69 de sus 149 saltos** y su distancia
real cae un **76 %**. Y el censo que pedía el encargo está hecho: dice dónde mirar, y ya
ha pagado.

```
sellos (TRES compilaciones cada uno, regla 6)
  zPhysics           sha1     18a1d0ca45fa2b2456f9a50e228cd96c9e6abaa6   x3
                     codehash c679021e3ba006fd4df81d8f92e80b9205a7c62c   x3
  zPhysicsBehaviors  sha1     fd8ce3652f3854bd5d7918ce68ab911e47383d79   x3
                     codehash 69937c71b58440fdf0b4001bb0b5f4d2e679c1d5   x3

                        ANTES (HEAD 63e8f06a)          DESPUES
  zPhysics
    textorder           326/719 desc, 149 saltos       252/719 desc,  80 saltos
    distancia |delta|   471.848 B                      112.744 B     (-76,1 %)
    fncmp               0 de 718                       0 de 718
    linkdelta           .text +0   resto IGUAL         .text +0   resto IGUAL
    dolwhere            29.937 B                       25.659 B      (-4.278 B, -14,3 %)
    reladdr             36 direcciones distintas       36            (sin regresion)
    simbolos SOLO nuestros  182                        151
    trypromo            DOL ROTO                       DOL ROTO (db6ae1f57779)
  zPhysicsBehaviors
    textorder           811/1121 desc, 38 saltos       811/1121 desc, 38 saltos
    fncmp               1 de 1120 (UpdateLoaded 856B)  1 de 1120     (sin cambio)
    linkdelta           .text +4   resto IGUAL         .text +4   resto IGUAL
    codehash            69937c71...                    69937c71...   (IDENTICO: ver 5)

checksplits     LIMPIO (0 solapes, 0 cortes)
lcfix           NO EJECUTADO (regla 2). No he movido ni una cadena: la .rodata del .o
                de zPhysics es IDENTICA byte a byte a la de HEAD.
configure.py / config/GOWE69/* / splits.txt / symbols.txt:  NO TOCADOS, nada que proponer.
```

---

## 1. El censo que pedía el encargo

`scratchpad/diferido-phys64/censo.py` clasifica cada símbolo descolocado por MECANISMO,
leyendo el mangling y el rango del objetivo. Sobre la base de HEAD:

```
bloque diferido del objetivo: empieza en el rango 359 (tras __static_init...)
descolocadas: 326 de 719
  en el bloque diferido: 326      en el codigo normal: 0     <-- confirma la r63
SALTOS: 149
  en el bloque diferido: 149      en el codigo normal: 0

  por mecanismo   descolocadas          saltos
  plantilla          209 (13.228 B)      127   (85,2 %)
  no-plantilla       117 ( 5.352 B)       21
  estatico             -                    1

  detalle de los 149 saltos
    plantilla   metodo   67      no-plantilla  dtor        9
    plantilla   dtor     57      no-plantilla  metodo      7
    plantilla   ctor      3      no-plantilla  _IHandle    4
                                 no-plantilla  operator=   1
                                 estatico      _GLOBAL_.I  1
```

**La cifra que manda: 127 de 149 saltos (85 %) son instanciaciones de plantilla.** O sea
que en zPhysics la cola que decide NO es `walk_globals` (vtables) ni el lazo de síntesis:
es la de plantillas y la de inlines (`saved_inlines`) que `wrapup_global_declarations`
drena. Y `pending_templates` es **FIFO** (`cp/pt.c:3572`, `*template_tail = ...;
template_tail = &TREE_CHAIN(...)`), no LIFO: la palanca ahí es el **orden de primera
petición diferida**, no el inverso. La regla del inverso vale para las vtables
(`walk_globals`), y en esta unidad las vtables no son el frente.

Y una segunda cifra, en el mismo censo, que es la que abrió la ronda: en `.text` teníamos
**182 símbolos que el objetivo no tiene** — y **32 de ellos eran de `_Storage`** cuando el
objetivo emite **UNO**.

Al terminar la ronda el censo queda así: 252 descolocadas (172 plantilla / 80
no-plantilla) y 80 saltos (59 plantilla / 20 no-plantilla / 1 estático), los 252 todavía
íntegramente dentro del bloque diferido.

---

## 2. El hallazgo: un constructor de copia SINTETIZADO enciende la vtable de la base y le roba la ranura a `~Vector<T>`

El objetivo emite el final del bloque diferido en **parejas**: `~Vector<T>` y justo detrás
`~FixedVector<T,N>`, tipo por tipo (659 `~Vector<IExplosion>`, 660
`~FixedVector<IExplosion,96>`, 661 `~Vector<IDisposable>`, 662
`~FixedVector<IDisposable,160>`…). Nosotros los sacábamos en **dos bloques separados por
160 rangos**, y en medio de los `~FixedVector` había, alternando uno a uno, unos
`_._Q33UTL11Collectionst8_Storage2Z...` que **el objetivo no tiene**:

```
  objetivo                                   nuestro (HEAD)
  659 _._Vector<IExplosion>                  690 _._FixedVector<IExplosion,96>
  660 _._FixedVector<IExplosion,96>          691 _._ _Storage<IExplosion,96>     <-- SOBRA
  661 _._Vector<IDisposable>                 692 _._FixedVector<IDisposable,160>
  662 _._FixedVector<IDisposable,160>        693 _._ _Storage<IDisposable,160>   <-- SOBRA
  ...                                        ...
                                             853 _._Vector<IExplosion>           <-- 163 tarde
```

`_Storage<T,N>` (UCollections.h:37) deriva de `FixedVector<T,Size,16>` y es la base de
`Listable<X>::List`, de `ListableSet<X>::List` y de los dos miembros `_NodeList` de
`GarbageNode::Collector`. **Emitíamos 32 símbolos suyos: 16 destructores y 16
constructores de COPIA**, más **13 vtables `_vt.Q33UTL11Collectionst8_Storage2Z...` que el
objetivo tampoco tiene** (167 vtables contra 142, y cero ausentes).

### De dónde salían

El compilador lo dice si se le pregunta. Declarando el constructor de copia de `_Storage`
como `private` y sin definirlo, `ngccc` escupe la lista entera de puntos de síntesis:

```
zPhysics.cpp: In method `UTL::Collections::Listable<IPursuit,8>::List::List(const ... &)':
UCollections.h:52: `_Storage<IPursuit *,8>::_Storage(const _Storage<IPursuit *,8> &)' is private
zPhysics.cpp:197: within this context
...
PhysicsObject.cpp: In method `GarbageNode<PhysicsObject,160>::Collector::Collector(const Collector &)':
PhysicsObject.cpp:494: within this context
```

Es decir: GCC 2.9 **sintetiza `List(const List&)`** al parsear la definición del estático
`_mTable`, y lo hace **aunque la definición sea por defecto, sin `= List()`** — que es la
forma a la que la r63 ya las había pasado con `UTL_IMPLEMENT_NO_COPY_INIT`. Ese
constructor sintetizado llama al de la base, sintetiza `_Storage(const _Storage&)`, y ése
**almacena el vptr**: enciende `_vt._Storage<T,N>`, que arrastra su destructor virtual y,
con él, `~FixedVector<T,N>` y sus cuatro virtuales a la **primera vuelta** de
`finish_file`. Justo en las ranuras donde el objetivo pone `~Vector<T>`.

Dicho al revés, que es como se usa: **el `= T()` no era la única puerta.** La r63 quitó el
inicializador de copia y el racimo siguió ahí porque la síntesis la dispara la
DEFINICIÓN del estático, con inicializador o sin él.

### La cura, y es el idioma que el árbol ya tenía COMENTADO

`UListable.h` llevaba escritas, en comentario, las tres declaraciones del idioma
no-copiable: `// List(const List &);` (dos veces) y `// _ListSet(_ListSet &);`. Bastó
**declararlas** (sin definirlas) más la equivalente de `GarbageNode::Collector`, bajo la
guarda `UTL_NO_COPY_CTOR`:

| medida | HEAD | con `UTL_NO_COPY_CTOR` |
|---|---:|---:|
| símbolos `_Storage` en `.text` | 32 | **1** (los mismos que el objetivo) |
| descolocadas | 326 | 312 |
| saltos | 149 | **120** |
| distancia \|delta\| | 471.848 B | **279.576 B** |
| **dolwhere** | 29.937 B | **27.631 B** |

`linkdelta` sigue `.text +0 / resto IGUAL` y `fncmp` `0 de 718`: no se toca una instrucción.

### Cuánto vale fuera de aquí (extrapolación MEDIDA, no supuesta)

`scratchpad/diferido-phys64/frente.py` recorre todas las unidades comparables y cuenta los
símbolos de `_Storage`/`Listable::List`/copia que emitimos y el objetivo no. Con el árbol
tal y como está hoy:

```
total _Stor  List copia    bytes  unidad
   19    10     4     5    13212  zEAXSound
    3     2     0     1     2492  zWorld
    3     2     0     1     2492  zEAXSound2
    3     2     0     1     2476  zCamera
    1     0     1     0      180  zPhysics
    1     0     1     0       56  zMain
30 simbolos sobrantes en 6 unidades, 20.908 B
```

**No es un frente grande: son 30 símbolos en cinco unidades ajenas, y 19 de ellos son de
zEAXSound.** zPhysics era el caso atípico (32 él solo). Lo digo con la cifra delante
porque `nfsmw-extrapolar-frentes` avisa de justo lo contrario. Quien lleve zEAXSound
tiene ahí 13.212 B a un `#define`.

---

## 3. La segunda guarda: `~List` declarado en clase sale una PASADA tarde

Con los constructores de copia apagados quedaba otro racimo. El objetivo emite
`~Listable<X>::List` **alternando con `OnGrowRequest__Vector<X*>`** (rangos 521, 522, 523,
524…) y las cuatro virtuales de `FixedVector` **en un bloque aparte más abajo** (559-642)
**y en orden INVERSO** al de los `~List` — la firma de `mark_vtable_entries` instanciando
por el recorrido de vtables. Nosotros pegábamos las cuatro virtuales **detrás de cada
`~List`**, porque emitir `~List` enciende `_vt.FixedVector<X*,N>` en la misma pasada.

`Listable<X>::List` y `ListableSet<X>::List` declaran `~List() override {}` **en clase**.
Quitando esa declaración (destructor implícito, guarda `UTL_IMPLICIT_LIST_DTOR`) el
destructor cambia de puerta en la cola y sale una pasada antes, que es donde el objetivo
lo tiene. Es la **pareja exacta** de `PLACEABLESCENERY_IMPLICIT_DTOR` de la r63: *en
clase* decide si SE EMITE, *implícito* decide DÓNDE.

| medida | con `~List(){}` | implícito |
|---|---:|---:|
| descolocadas | 312 | 298 |
| saltos | 120 | **94** |
| distancia | 279.576 B | **126.024 B** (−55 %) |
| **dolwhere** | 27.631 B | **26.036 B** |

Y con `UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR` encima (la guarda ya existía desde la r34,
sólo la usaba zSim): 292 descolocadas / 92 saltos / 124.760 B.

**Ojo, y es el dato de método de la ronda: esa guarda SOLA, sobre la base de HEAD, valía
CERO** — 326 descolocadas, 149 saltos y `dolwhere` clavado en 29.937 B, medido antes de
tocar nada más. Sólo paga una vez apagados los constructores de copia. **Las palancas del
bloque diferido no son aditivas: se habilitan unas a otras**, y una guarda que sale
neutra en una base no está refutada, está esperando.

Después de las tres, el racimo `~Vector`/`~FixedVector` sale **emparejado tipo por tipo,
como el objetivo** (659/660, 661/662, …, 698/699 -> nuestros 806/807, 808/809, …, 836/837).

---

## 4. El orden de PARSEO, dos veces, y una tercera que salió negativa

Con el racimo de plantillas colocado, lo que quedaba arriba eran interfaces sueltas:

1. **Reordenar el bloque de `#include` de interfaces** al orden en que el objetivo
   completa las clases (`IVehicleCache.h` delante de `IVehicle.h`; `IAI.h` y
   `ITrafficCenter.h` detrás de `IRigidBody.h`; `ISpikeable.h` delante de `IFengHud.h`):
   292/92 -> **287/80**, distancia 124.760 -> 117.260 B, `dolwhere` 26.036 -> 26.008 B.
2. **`#include Interfaces/Simables/IEngine.h` delante de `IVehicle.h`.** El objetivo pone
   `_IHandle__11IRaceEngine` en el rango 373, entre `push_back<IInputPlayer*>` (372) y
   `~IVehicle` (374); nosotros lo emitíamos en el 524, dentro del racimo de `_IHandle`
   que el objetivo tiene UNDEFINED. Un solo `#include`:
   287/80 -> **252/81**, distancia -> 112.728 B, **`dolwhere` 26.008 -> 25.659 B**.
   35 descolocadas por una línea.

Y el negativo de la pareja, que merece regla propia:

3. **`#include Interfaces/IBody.h` detrás de `ICollisionBody.h`.** El objetivo pone
   `_._5IBody` en el rango 376 y nosotros en el 465, así que sobre el papel es la misma
   jugada que (2). Y `textorder` lo confirma: 81 -> **79 saltos** y 112.728 -> **111.228 B**
   de distancia. Pero **`dolwhere` EMPEORA: 25.659 -> 25.733 B**. Revertido.
   Es el caso de libro del aviso 3 de la r63 (*`textorder` no siempre es la cifra que
   manda*), y ahora con las dos medidas enfrentadas en la MISMA unidad y con el MISMO
   tipo de cambio: **`textorder` mejorando y el DOL empeorando a la vez.** La regla
   operativa que dejo: *ninguna palanca de orden se queda sin pasarla por `dolwhere`,
   aunque `textorder` la aplauda.*

También queda la cuarta guarda, `WORLDCONN_H_IMPLICIT_PKT_BODY_OPEN_DTOR`: el objetivo
emite `_._Q29WorldConn13Pkt_Body_Open` (428) DELANTE de `ConnectionClass`/`Size`/`Type`
(429-431) y nosotros detrás — la firma exacta de `Pkt_Body_Send` de la r58-main, en la
misma cabecera y a tres clases de distancia. Aplicada: **neutra en DOL** (25.659 B antes y
después) porque el grupo entero sigue 50 rangos tarde, pero coloca las cuatro en el orden
del objetivo y deja de ser una forma que sabemos mal escrita. Queda anotada como tal.

### Los negativos, re-medidos sobre la base nueva

| ensayo | resultado | veredicto |
|---|---|---|
| `EVENTSEQ_FWD_IENGINE` (re-medido) | 252 -> **285** descolocadas, y sigue `linkdelta .rodata -120` | **NEGATIVO, confirmado 2ª vez** |
| bloque `IMPLEMENT_LISTABLE` reordenado al orden de la cola del objetivo | 312 -> **322** desc, 120 -> **131** saltos | **NEGATIVO** |
| `#include IBody.h` (arriba) | `textorder` mejora, `dolwhere` +74 B | **NEGATIVO** |
| las tres guardas nuevas en zPhysicsBehaviors | 811/38 antes y después, `codehash` idéntico | inerte: no instancia Listable |

Del primero saco además **el porqué, que hasta ahora no estaba medido**: la `.rodata` del
`.o` con y sin `EVENTSEQ_FWD_IENGINE` tiene **el mismo tamaño exacto (16.712 B)** pero
**1.194 bytes distintos, entre los offsets 364 y 10.489**. No se pierde `.rodata`: se
**permuta el pool**, y los 120 B los tira `-strip-unused-data` en el enlace. Es un caso de
`lcfix`/primer, no de orden — y por eso ninguna palanca de `#include` lo va a arreglar
sola.

---

## 5. Un aviso que afecta a TODA la ronda: el sha1 de un `.o` no sirve para sellar cuando se toca una cabecera

He tocado tres cabeceras compartidas (§6). Sus guardas son inertes, pero **añaden líneas**,
y eso cambia `.debug_line` (y su `.rela.debug_line`) de **cada `.o` que las incluya**. El
`.o` de zPhysicsBehaviors pasó de sha1 `dd641fc7…` a `fdef2e7f…` **sin cambiar un solo
byte de código**. Con ocho agentes sellando por sha1 eso es exactamente el tipo de ruido
que envenena una medida, y a mí me costó media hora creer que había roto algo.

La comprobación buena es un hash que ignore la información de depuración. Dejo la
herramienta, 20 líneas (vivía en `scratchpad/diferido-phys64/codehash.py`; **propongo
subirla a `scripts/codehash.py`**):

```python
# -*- coding: utf-8 -*-
"""sha1 de las secciones que IMPORTAN (sin DWARF): contenido + tabla de simbolos."""
import hashlib, struct, sys
def h(path):
    f = open(path, 'rb').read()
    shoff = struct.unpack('>I', f[0x20:0x24])[0]
    she, shn, shx = struct.unpack('>HHH', f[0x2E:0x34])
    S = [struct.unpack('>10I', f[shoff+i*she:shoff+i*she+40]) for i in range(shn)]
    shstr = S[shx][4]
    def nm(off, base):
        e = f.index(b'\0', base+off); return f[base+off:e].decode('utf-8', 'replace')
    out = []
    for s in S:
        n = nm(s[0], shstr)
        if 'debug' in n or 'line' in n or n == '.comment':
            continue
        data = b'' if s[1] == 8 else f[s[4]:s[4]+s[5]]      # 8 = SHT_NOBITS
        out.append((n, s[1], hashlib.sha1(data).hexdigest(), s[5]))
    out.sort()
    return hashlib.sha1(repr(out).encode()).hexdigest()
if __name__ == '__main__':
    print(h(sys.argv[1]))
```

Con ella, el **control de inercia** de esta ronda es limpio y está hecho tres veces:
`zPhysicsBehaviors.o` da `codehash 69937c71b58440fdf0b4001bb0b5f4d2e679c1d5` **con las
cabeceras revertidas y con las cabeceras editadas**. Idéntico. Lo único que cambia en las
unidades ajenas es el número de línea del DWARF, que no llega al DOL (`dtk elf2dol` sólo
toma las secciones cargables).

**Y hay una segunda fragilidad, aún más fina, que encontré por accidente al anotar la
nota de `UpdateLoaded` en `SuspensionTraffic.cpp`:** diez líneas de **comentario** dentro
de un `.cpp` mueven el sufijo `.NNNNN` de los símbolos LOCALES de estáticos de función.
Medido byte a byte: con el comentario `_.tmp_7.27178`, sin él `_.tmp_7.27175`; y lo mismo
en `hash.27177/27174`, `StaticToDynamicBrakeForceRatio.25816/25813`, etc. **Todo lo demás
del objeto es idéntico** — `.text`, `.rodata`, `.data`, `.symtab` y las cuatro secciones
`.rela` dan el mismo sha1; sólo cambia `.strtab`, y con la misma longitud. Son símbolos
locales que el enlazador descarta, así que el DOL no se entera. Pero significa que
**incluso `codehash` puede moverse por un comentario si incluye `.strtab`**: si a alguien
le sale una diferencia sólo en `.strtab`, que mire los sufijos antes de dar por rota una
medida. Lo dejo así (contando `.strtab`) a propósito: prefiero un falso positivo
explicable a un falso negativo que esconda un cambio de nombre real.

---

## 6. Ficheros tocados

```
src/Speed/Indep/SourceLists/zPhysics.cpp                       (mio: 4 #define, reorden de #include, negativos)
src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp    (mio: SOLO comentario, la nota r64 de UpdateLoaded)
src/Speed/Indep/Libs/Support/Utility/UListable.h               (COMPARTIDA -- guardas INERTES, control en 5)
src/Speed/Indep/Libs/Support/Utility/UCollections.h            (COMPARTIDA -- guarda INERTE,  control en 5)
src/Speed/Indep/Src/World/WorldConn.h                          (COMPARTIDA -- guarda INERTE,  control en 5)
```

Las guardas nuevas, todas `#ifdef`/`#ifndef` y sólo encendidas desde `zPhysics.cpp`:

```
UTL_NO_COPY_CTOR                        UListable.h (x3), UCollections.h (x1)
UTL_IMPLICIT_LIST_DTOR                  UListable.h (x2)
UCOLLECTIONS_H_IMPLICIT_STORAGE_DTOR    ya existia (r34); zPhysics es su segundo cliente
WORLDCONN_H_IMPLICIT_PKT_BODY_OPEN_DTOR WorldConn.h (x1)
```

**Aviso de coordinación, por honestidad:** para los controles de §5 tuve que revertir y
restaurar las tres cabeceras compartidas varias veces (copia del fichero entero, no
parche). Tomé la instantánea al empezar la ronda; si otro agente editó `UListable.h`,
`UCollections.h` o `WorldConn.h` **durante** mi tanda, su cambio se habrá perdido en ese
vaivén. Al cerrar, `git diff` sobre las tres muestra **exactamente mis cinco hunks y nada
más**, que es lo que se esperaría si nadie más las tocó — pero no puedo distinguir eso de
haberlas pisado. Merece una mirada antes de promocionar.

Sin commit. `configure.py`, `config/GOWE69/*`, `splits.txt` y `symbols.txt` **no tocados**
y **sin nada que proponer**: ninguna palanca de esta ronda necesita un `pad_`, un rango
nuevo ni una entrada de `keep.lst`. `lcfix.py` **no ejecutado** (regla 2) y sin motivo
para ejecutarlo: la `.rodata` del `.o` de zPhysics es idéntica byte a byte a la de HEAD.

## 7. zPhysicsBehaviors

Sigue exactamente donde la dejó la r63: `fncmp 1 de 1120` (`UpdateLoaded`, 856 B),
`linkdelta .text +4 / resto IGUAL`, `dolwhere` no medible por ese `+4`.

**No he reabierto `UpdateLoaded__Q217SuspensionTraffic4Tire`.** El encargo de la r64 la
vuelve a dar como «familia A que salió VIVA» con la misma cita
(`COPY-PROP: Replacing reg 625 in insn 1210 with reg 753`) que la r63 ya demostró que es
de `ActualReadJoystickData` (zPlatform), no de esta función; el volcado fresco de la r61
sobre ésta dice «CERO líneas COPY-PROP». No ha aparecido dato nuevo, así que sigue
**VEDA DURA**. Lo he anotado junto a la función, y `previo.py UpdateLoaded` ya lista
`rondas: … r61, r63, r64` para que la r65 lo vea antes de gastar la tanda.

Lo único que sí he medido aquí: las tres guardas nuevas de zPhysics aplicadas a esta
unidad dan **cero cambio** (811/1121 descolocadas y 38 saltos antes y después,
`codehash` idéntico). No instancia ni un `Listable`, así que el frente nuevo no la toca.

## 8. Lo que queda en zPhysics, por tamaño

| familia | n | distancia | descolocadas | dónde está la causa |
|---|---:|---:|---:|---|
| C métodos normales | 501 | 36.908 B | 94 | arrastre de los saltos de cabecera, no causa propia |
| G2 `~FixedVector<T,N>` | 22 | 14.820 B | 17 | ya EMPAREJADO con G1: es un desplazamiento constante |
| G1 `~Vector<T>` | 19 | 13.428 B | 14 | ídem |
| A2 dtores no-plantilla | 24 | 8.452 B | 13 | `_._5IBody`, `Pkt_Body_Open` (grupo 50 rangos tarde) |
| A1 `_IHandle` | 10 | 7.168 B | 9 | `IContext` (−4.896) — la diana, y es de `.rodata` (§4) |
| H2 `GetGrowSize` Vector | 19 | 6.668 B | 7 | |
| D1 `~Instanceable::_List` | 4 | 6.320 B | 4 | |
| E `OnGrowRequest` | 19 | 4.956 B | 15 | |
| F virtuales de FixedVector | 44 | 3.696 B | 36 | ya en el bloque bueno y en el orden inverso correcto |
| resto | | 10.328 B | 43 | |
| **TOTAL** | | **112.744 B** | **252** | |

**La diana de la r65 es una sola y está nombrada: `EventSequencer::IContext`.** Vale
−4.896 B de salto, es el primero del bloque diferido (arrastra a `~ISimable` y con ella a
toda la cabeza), y su palanca de orden (`EVENTSEQ_FWD_IENGINE`) está lista y medida dos
veces: lo único que la bloquea son 120 B de `.rodata` que `-strip-unused-data` tira porque
el pool queda permutado. Eso es trabajo de `lcfix`/primer sobre 1.194 bytes acotados entre
los offsets 364 y 10.489 de la `.rodata` de la unidad, **no** de `#include`.
