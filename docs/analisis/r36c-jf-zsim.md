# zSim: de 408 B a delta cero, y qué falta exactamente para el DOL

    al empezar   +0  rodata-184  data-224      (408 B)
    ahora        +0  IGUAL                     (cero en las nueve secciones)
    96.400/96.400 B de código, 402/402 funciones — intacta en todo el recorrido

**zSim es la unidad más cerca del DOL que queda**, y con 96.400 B es el mayor
premio abierto del proyecto.

## Cómo se cerró el tamaño

| paso | ganancia |
|---|---|
| Nueve huecos de `.data` escritos entre sus vecinos reales | **+240 B** |
| `ZMAIN_MESSAGES_LUA_INLINE` (receta de la r35, sólo la usaba zMain) | +16 B |
| Literales de `MPerpBusted` por orden de parseo + `deadstr` | +48 B |
| `Attrib::Gen::visuallook` escrita a mano | +24 B |
| `High/Medium/Low/Reflection` + `0.5f`/`speechtune` + tres etiquetas de pool | +88 B |

La regla que lo hizo posible es de `c36b-an2` y no se estaba usando: **un símbolo
muerto con etiqueta pero SIN `.size` no pierde un byte**. Las cadenas que
faltaban no existen en el árbol *y* no pertenecen a ningún símbolo en el objeto
extraído, así que van escritas a mano sin `.size` y ni siquiera necesitan
`keep.lst`.

Y el límite por el otro lado, medido: la cuarta etiqueta (`lbl_80405BC8`, 8 B)
**se pasa** — con ella sale `rodata+8`.

## Lo que falta: OCHO estáticos de plantilla mal colocados

`dolwhere` da **7.275 B distintos**, y casi todos son el mismo inmediato
desplazado (`58c4` contra `5954`, o sea 0x90 = 144 B). La causa es que estos
ocho caen **al final** de nuestra `.data` porque `zSim.cpp` los instancia ahí,
mientras el objetivo los tiene **intercalados**:

    simbolo                                        objetivo   nuestro    delta
    Countable<SimTask>::_mCount                    80435828   80435958   +304
    Singleton<Sim::Internal::...>::mInstance       8043583C   80435950   +276
    Countable<Sim::Object>                         80435894   80435960   +204
    Factory<Sim::Param, IEntity, UCrc32>           80435898   80435968   +208
    Factory<const ConnectionData&, Connection>     8043589C   8043596C   +208
    Factory<Sim::Packet*, int, UCrc32>             804358A0   80435964   +196
    Countable<Sim::Connection>                     804358A4   8043595C   +184
    Singleton<INIS>::mInstance                     804358C4   80435954   +144

Y arrastran a todo lo demás: el bloque de `Tweak_GameBreaker*` sale 8 B antes,
y el de `bSawLoadingScreen`/`Tweak_GameSpeed` 28 B antes.

**Reordenarlos entre sí NO sirve** (probado, neutro): siguen al final del TU.
Hay que moverlos **al fichero y al punto donde el objetivo los parsea** —
`Countable<SimTask>` va justo detrás de `mStackFrame` en `Simulation.cpp`.
Primer intento fallido y por qué: la macro `IMPLEMENT_COUNTABLE` no está
disponible en ese fichero; hay que llevar también su cabecera.

## Dos trampas que costaron trabajo

1. **`dtk` pone a CERO los campos reubicados del objeto extraído.** Rellené los
   nueve huecos de `.data` con ceros por eso, y el DOL real tiene ahí un puntero
   (`0x804049AC`), un `0x93C` y varios enteros. **Para el contenido de un hueco
   hay que leer el DOL, no el objeto.**
2. **El bloque de 448 B que nos «sobra» en `.rodata` son cinco vtables muertas**
   (`_vt.11IAttachable`, `_vt.6IModel`…) que el enlazador estripa enteras.
   Tenemos las 74 del objetivo; los «huecos» de 40 B que señalaba `dolrod` ahí
   eran artefactos de resincronización.

## El compromiso pendiente, medido

La definición escrita a mano de `lbl_80404864` duplica el `1.0f` y desplaza 4 B
todo el bloque desde `0x80404904`. Quitándola y usando `= 1.0f`, **320 B de
`.rodata` casan byte a byte**, pero `OnManageTime` baja de 564 a 560 B: con el
literal GCC **iza el `@ha` a un registro preservado** (`lis r29` en el prólogo,
`stmw r29`, marco 0x30) mientras el objetivo lo recalcula en **r9** —volátil— en
cada uso. La forma `extern` da exactamente la del objetivo.

O sea que hace falta una tercera forma: que la dirección **no se ice**, sin
definir un objeto aparte. Es el mismo eje preservado-contra-volátil de
`nfsmw-marco-y-reparto`.
