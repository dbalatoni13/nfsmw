# r60 · `phys` — zPhysics: 262.256 -> **35.673 B**, y no era zPhysics

**Una linea de fuente, cero instrucciones tocadas, cero cabeceras, cero `keep.lst`.**
Los 226.583 B que se van no eran de zPhysics: eran de **un simbolo que le roba `zMain`**,
promocionada en esta misma ventana. Y la palanca que lo arregla es nueva y **general**:
**una instanciacion explicita emite el simbolo `.globl` en vez de `.weak`, en la misma
ranura y sin mover un byte** — o sea que la victima de un cambio de dueno **puede
defenderse sola**, sin tocar la unidad del ladron.

    zPhysics   0ab50993ec35d55cf8949e3b6689f1d240be3059   TRES compilaciones iguales
               (y 5197079b21d278ef153e6f93373715e0db8958af a mitad de ventana: cabeceras
                ajenas cambiandose bajo los pies. LAS DOS dan dolwhere 35.673 y fncmp
                0/718 -- el DOL es invariante, el sha1 no. Ver seccion 6.)
    zGameplay  54c66cf43b6293b4018e32648ef59c6e3f1b6fca   (sin tocar por mi; ver seccion 6)
    zMisc      da892bbf6d8dced6050df7d698a8ac60dea20e2e   (sin tocar, sha1 identico)

    linkdelta  zPhysics   `.text +0  IGUAL`     antes y despues
    dolwhere   zPhysics   262.256 -> 35.673 B   (-226.583)
    dolwhere   zGameplay   35.438 -> 35.438 B   (sin tocar)
    linkdelta  zMisc      rodata+2840 data+32   (sin tocar)
    fncmp      0 de 718 / 0 de 768 / 0 de 450   antes y despues
    trypromo   zPhysics  DOL ROTO (96d71ee52427)
               zGameplay DOL ROTO (2320424c272e)
               zMisc     DOL ROTO (a2394557abb5)
    keep.lst   2f3f7a3455b7a07186cd3eb6ed8ebc889b196e0b   INTACTO al empezar y al acabar
    lcfix.py --check: 186 pendientes (143 zFe, 43 zSpeech), CERO mias y NINGUNA venenosa
               -- medido: con las 186 aplicadas en un keep.lst privado, zPhysics da
               35.673 B y zGameplay 35.438 B, las mismas cifras AL BYTE. No dependo de lcfix.

---

## 1. La partida no era la del brief, y la diferencia es la clave

El brief daba **35.673 B** para zPhysics. Al medir hoy salieron **262.256**. No es deriva
ni un `keep.lst` rancio: es que **entre la medida de la r59 y la mia, `zMain` PROMOCIONO**
(commit `18662066`). Prueba directa, enlazando el proyecto entero con `zPhysics`
promocionada y **sustituyendo `zMain` por su objeto extraido**:

    zPhysics promocionada, zMain NUESTRO      262.256 B
    zPhysics promocionada, zMain EXTRAIDO      35.673 B      <- la cifra de la r59

Los 226.583 B de diferencia son **un solo simbolo de 176 B**.

---

## 2. El mecanismo, con la direccion exacta

`find__H2ZPP11IDisposableZP11IDisposable_4_STLX01X01RCX11_X01` — o sea
`_STL::find<IDisposable**, IDisposable*>`, 176 B — sale **WEAK** de toda TU que lo
instancia implicitamente. Y:

| objeto | lo define | binding |
|---|---|---|
| `obj/zMain.o` (extraido) | **NO** | — |
| `src/zMain.o` (nuestro) | SI, en +0x1F228 | WEAK |
| `obj/zPhysics.o` (extraido) | SI, en +0x1947C | GLOBAL (artefacto de la extraccion) |
| `src/zPhysics.o` (nuestro) | SI, en +0x1978C | WEAK |

En el enlace **base** manda el GLOBAL del zPhysics extraido y el simbolo cae en
`0x80227414`, que es donde el objetivo lo tiene: pegado a `find<ISpikeable*>`
(`0x802274C4`) y justo detras de `Physics::Tunings::UpperLimit`. El DOL sale OK y
**nadie ve nada**.

Al promocionar zPhysics las dos copias son WEAK, **gana la primera del orden de enlace**
—`zMain` va en el puesto 15 y `zPhysics` en el 23— y los 176 B se meten **dentro de la
`.text` de zMain**, en `0x801F11A8`, entre sus dos `EventManager::EmbedField`.
Consecuencia:

    de 0x801F11A8 a 0x802274C4  ->  TODO desplazado +176 B     221.804 B de direcciones
    2.349 rangos de .text, 245.322 B en 113 "bloques"
    2.236 sombras de 1-2 B mas: las mitades @ha/@l de cada @sda21 que cruza el hueco

---

## 3. La palanca nueva: la instanciacion explicita sale `.globl`

Medido con `ngccc -S` sobre un caso minimo con rellenos, el **unico** cambio entre la
instanciacion implicita y la explicita es la directiva:

    implicita:   filler1  filler2  filler3   .weak  myfind...   <- cola de finish_file
    explicita:   filler1  filler2  filler3   .globl myfind...   <- MISMA ranura

Ni una instruccion, ni un byte de posicion. Y **GLOBAL gana a WEAK vaya donde vaya** en el
orden de enlace, asi que la victima recupera su simbolo sin que el ladron cambie.

En el arbol, comprobado sobre el `.o` real: el simbolo sigue en **+0x1978C**, byte a byte
donde estaba, y solo pasa de `WEAK` a `GLOBAL`. La edicion entera:

```cpp
namespace _STL {
template IDisposable **find(IDisposable **, IDisposable **, IDisposable *const &);
}
```

**GCC 2.95 no acepta la forma cualificada.** `template T _STL::find(...)` a nivel de
fichero da `should have been declared inside '_STL'` mas `non-template used as template`.
La directiva tiene que ir **dentro** del `namespace`.

**Esto no es un andamio**: el objetivo tiene ese simbolo en zPhysics, es suyo, y lo unico
que hacemos es dejar de cederlo. Lo que si es un andamio —y sigue pendiente— es que
**nuestro zMain emita una instanciacion que el zMain original no tiene**.

---

## 4. El censo completo: en TODO el proyecto habia UN robo, y era este

`duenos_all.py` (scratchpad) recorre los 616 objetos del enlace y, para cada una de las
**95 unidades pendientes**, busca simbolos que **algun objeto NUESTRO anterior define WEAK
y su propio original NO define**. Son dos diccionarios y tarda unos 20 s.

    promocionadas 521 / pendientes 95
    zPhysics.o   ROBADA en 1 simbolos, 176 B   <- zMain.o  find<IDisposable*>
    (ninguna otra unidad, ningun otro simbolo)

Y en los dos sentidos que faltaban, tambien medidos:

| pregunta | zPhysics | zGameplay | zMisc |
|---|---|---|---|
| roba a objetos POSTERIORES ya promocionados? | 0 | 0 | 0 |
| robaria en el mundo TODO-PROMOCIONADO? | 0 | 0 | 0 |
| le robarian las PENDIENTES anteriores al promocionar? | 0 | 0 | **1 / 32 B** |

El unico pendiente: **`__builtin_new` (32 B), que nuestro `zFe2.o` define WEAK, el
`zFe2.o` extraido no define, y el `zMisc.o` extraido SI**. El dia que zFe2 promocione le
robara ese simbolo a zMisc. Es la misma familia y admite la misma palanca.

---

## 5. Lo que le queda a zPhysics, y el brief estaba desactualizado

El brief decia "35.673 B con UNA sola causa: el orden del pool de `.rodata`, 3.150
diferencias de exactamente 2 bytes". Hoy, con el robo cerrado, el desglose real es:

| bloque | B | rangos | que es |
|---|---:|---:|---|
| `.text`, cola de `finish_file` | **23.872** | 490 | orden de las instanciaciones diferidas |
| `.rodata` | **11.792** | 73 | ~8.700 de orden de vtable + ~3.011 de pool |
| `.data` | 7 | 7 | |
| `.over` | 2 | 1 | |

**Las 3.150 diferencias de 2 B ya no existen**: eran la sombra `@sda21` del robo.

Y la geografia esta acotada: de `0x8020DF98` (primer simbolo de la unidad) a `0x8022C28B`
—**122.100 B, la mitad de la unidad**— el `.text` esta a **delta 0 exacto**. Todo lo que
queda vive en la cola, de `0x8022C28C` a `0x80231A64`: **360 simbolos, y solo 5 en su
indice**. Medido con `addrmap.py` (scratchpad), que empareja `config/GOWE69/symbols.txt`
con la tabla del ELF **enlazado** e imprime solo las TRANSICIONES de delta.

Los cuatro que mandan (el objetivo los tiene al principio de la cola y nosotros muy tarde):

    idx obj  0  _._16PlaceableScenery           -> nuestro 140   (+140)
    idx obj  3  _IHandle__17ITriggerableModel   -> nuestro 131   (+128)
    idx obj  4  _._17ITriggerableModel          -> nuestro 132   (+128)
    idx obj 14  _IHandle__11IRaceEngine         -> nuestro  65   ( +51)

El resto es un corrimiento de -7/-4 que es **consecuencia** de esos. Y hay una pista de
fuente: el orden de los `push_back<T*>` del objetivo **no es el de nuestro bloque
`IMPLEMENT_LISTABLE`**:

    objetivo:  IModel IExplosion IInputPlayer ICollisionBody ISimpleBody IRigidBody
               IDisposable IRecordablePlayer ISpikeable ... IVehicle(114) Smackable(134)
    nuestro:   IExplosion IDisposable IModel IRigidBody ICollisionBody ISimpleBody
               IRecordablePlayer IInputPlayer ISpikeable

**No lo he probado**: reordenar ese bloque mueve tambien los `_mList` estaticos de `.bss`
y `.data`, que hoy estan **bien** (`.data` a 7 B y `linkdelta` IGUAL). Es un experimento
que hay que hacer con `fncmp` mas `dolwhere` detras, no a ciegas.

`parseord.py zPhysics` da **CONTROL FALLIDO** (13 de 26): 26 vtables y probablemente los
dos bloques de `finish_file` del aviso del brief. **No usar su cifra.**

Y el pool, medido con `rodorden.py`: **160 de 183 cadenas en secuencia**. La fila grande es
un `insert nue[24:46]` —22 cadenas nuestras que el objetivo no tiene ahi—, y el primer de
pool **solo mueve hacia atras**, asi que esa no es accionable. Las dos que si lo serian
(`SceneryOverrideConn` / `Pkt_SceneryOverride_Open`, que el objetivo pone en 120-121 entre
`Pkt_VehicleFragment_Service` y `SmackableRenderConn`) **no se pueden colocar desde
`zPhysics.cpp`**: los dos vecinos salen de dentro de una cabecera y no hay punto de parseo
entre ellos.

---

## 6. Regresiones: NINGUNA -- y dos `sha1` que cambiaron sin ser mios

`fncmp` **0 de 718 / 0 de 768 / 0 de 450**, antes y despues. Un solo fichero tocado,
`src/Speed/Indep/SourceLists/zPhysics.cpp`, **15 lineas anadidas al final** (12 de
comentario). Es una SourceList: **no la incluye nadie** (`grep -rln` sobre `src/` da cero).
Ninguna cabecera, ningun `config/GOWE69/*`, ningun `splits.txt`, ningun `keep.lst`.

**Pero el `sha1` de zPhysics.o y zGameplay.o cambio DOS VECES durante la ventana sin que yo
tocara nada**: otros agentes editaron `src/Speed/Indep/Src/Sim/SimTypes.h` (un constructor
`SimCollisionMap()` **bajo `#ifdef SIMCOLLISIONMAP_CTOR`**, o sea inerte para mi) y
`World/HeliRenderConn.h`. zPhysics.o fue `0ab50993` -> `5197079b` -> `0ab50993` otra vez.

Lo importante es que **el DOL es invariante**: `dolwhere` da **35.673 B** y `fncmp`
**0 de 718** con LOS DOS objetos, y zGameplay **35.438 B** y **0 de 768** con los suyos.
Pero **el sello de "tres compilaciones con el mismo sha1" no vale nada si la ventana lo
atraviesa**: hay que sellar DESPUES de la ultima edicion ajena, **y volver a MEDIR**, no
solo a resellar. Un `sha1` distinto no es prueba de regresion, y uno igual no es prueba de
que nadie te haya tocado el arbol.

---

## 7. Propuestas (no las aplico)

1. **`zMain` no deberia emitir `find<IDisposable*>`.** Ya no bloquea a nadie —zPhysics se
   defiende sola— pero sigue siendo un simbolo que el zMain original no tiene, o sea una
   instanciacion de mas en su fuente. Vale la pena que su agente busque el uso.
2. **`__builtin_new` de zFe2 (32 B, seccion 4)**: robara a zMisc el dia que zFe2
   promocione. Se cierra igual, pero el simbolo nace de una cabecera compartida: mejor
   mirarlo antes de que sea una sorpresa a mitad de una promocion.
3. **`duenos_all.py` merece subir a `scripts/`**. Es la unica medida del proyecto que ve el
   cuarto mecanismo **desde la victima** (`movidos.py` lo ve desde el ladron, y ni
   `linkdelta` ni `permorden` ni `fncmp` lo ven en absoluto). Corre en 20 s sobre los 616
   objetos y contesta las tres preguntas de la tabla de la seccion 4, incluida la del
   mundo todo-promocionado.
4. **`addrmap.py` tambien**: empareja `symbols.txt` con el ELF enlazado e imprime solo las
   transiciones de delta. Es lo que convirtio "113 bloques y 245.322 B de `.text`" en "un
   simbolo de 176 B". `dolwhere` **no puede** dar eso: fusiona rangos hasta 12 bytes
   iguales, asi que un desplazamiento de 176 B le sale como un bloque de 51.768 B con el
   nombre de una funcion que no tiene nada malo (`OnGrowRequest`).
5. **La cifra de `dolwhere` de una unidad NO es comparable entre rondas** si entre medias
   promociono otra: mide el enlace ENTERO. Antes de usar una cifra del brief hay que
   comprobar que la base sigue limpia — `python scripts/dolwhere.py <unidad-YA-promocionada>`
   contesta `DOL OK` en 40 s si lo esta. **Es un chequeo de una linea y me habria ahorrado
   media hora de arqueologia.**
6. **Actualizar el DOSSIER**: la linea de zPhysics ("una sola causa: el orden del pool de
   `.rodata`, 3.150 diferencias de 2 B") describe la sombra del robo, no la unidad.

---

## 8. Sorpresas

1. **226.583 B invisibles para las cuatro herramientas del frente a la vez**:
   `linkdelta` IGUAL, `fncmp` 0/718, `permorden` 561/718, y `dolwhere zMain` dice
   **DOL OK**. La unica que lo ve es `dolwhere` de la VICTIMA — **y le echa la culpa a la
   victima**.
2. **`permorden` decia 561 de 718 "en su sitio" con 245 kB de `.text` distinto en el DOL.**
   No miente: compara el OBJETO, y el robo pasa en el ENLACE.
3. **La instanciacion explicita cambia SOLO la directiva.** Esperaba que la moviera de
   ranura (a `pending_templates` en vez de la cola de inlines) y no: mismo sitio, mismos
   bytes, `.weak` -> `.globl`.
4. **El `.globl` no se puede poner con un `asm()` de fichero.** Los `asm()` salen donde se
   escriben y las plantillas en `finish_file`, o sea siempre DESPUES: gas se queda con el
   `.weak`, que es el ultimo que ve.
5. **`lcfix --check` paso de 0 a 186 pendientes durante mi ventana** (143 zFe, 43 zSpeech),
   por trabajo de otros agentes. Aplicadas en un `keep.lst` privado, mis dos cifras salen
   **identicas al byte**. La regla util: no basta con contar las pendientes, hay que
   **medir con ellas puestas**; cuesta un enlace.
6. **`zMiscSmall` no compila**: `Could not open output file build/.../zMiscSmall.o`. No es
   un error de fuente — otro agente tiene el fichero abierto. `build_direct.py` lo reporta
   como `FAILED` con seis lineas de warnings delante y el error de verdad en la septima,
   asi que **parece** un fallo de compilacion y no lo es.
7. **`build_direct.py <unidad>` casa por PREFIJO**: pedir `zPhysics` compila tambien
   `zPhysicsBehaviors`, y `zMisc` arrastra `zMiscSmall`. Si el vecino esta roto, el
   resumen dice `1 ok, 1 fallidas` y parece tuyo.
