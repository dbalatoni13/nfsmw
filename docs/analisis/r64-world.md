# r64 - world (zWorld, zWorld2, madidct, LibSN/steering)

**Titular: el pool de cadenas de `zWorld` deja de ser adivinanza.** Un `asm()` de
fichero se emite DONDE ESTA ESCRITO, tambien dentro de un `.cpp` de la
SourceList, y eso permite meter un MARCADOR de cero coste en cada punto
candidato y LEER, del DOL enlazado, el mapa `posicion-de-pool -> fichero:linea`.
Con 470 marcadores en un solo enlace, **108 de las 138 cadenas escritas a mano
tienen ya su sitio medido y estan colocadas**: `dolwhere zWorld` **24.884 ->
23.513 B**, sin mover una instruccion (`fncmp` en las MISMAS 4 de 582) y con
`linkdelta` en `.text +0  IGUAL`.

**Y el segundo resultado, que es el que manda para la proxima ronda: el frente
NO era el orden de las 138 cadenas. Es la CABECERA del pool.** Los 472 B de la
extension del prefijo `lbl_80409D58` **duplican, letra por letra, lo que cc1plus
ya emite**: contado sobre el enlace, `Attrib::Attribute`, `Attrib::Instance`,
`GAMECUBE`, `WorldBodyConn`, `Attrib::Gen::gameplay` y `Attrib::Gen::simsurface`
salen **una vez mas en nuestro DOL que en el objetivo**, todas. Eso desplaza
472 B la cabecera del pool y con ella TODO lo que viene detras -- por eso mover
108 cadenas solo paga 1.371 B.

| unidad | sha1 `.o` (x3) | `fncmp` | `linkdelta` | `dolwhere` | `trypromo` |
|---|---|---|---|---|---|
| **zWorld** | `70e626bb0b599e2dfd2893977301a76eca3efcd2` | **4 de 582** (las mismas) | `.text +0  IGUAL` | **24.884 -> 23.513** | DOL ROTO |
| zWorld2 | `37eeecdec5772e3aa45dad305c63573902c551dc` | 2 de 357 (las mismas) | `.text +0  bss+64` | no se puede (bss+64) | DOL ROTO |
| madidct | `63c4851dd516be649fd2d299201017e82e2fc138` | 2 de 3 | `.text -20  IGUAL` | -- | DOL ROTO |
| LibSN/steering | `1962b7d16adc91a21e4f8070dacc3113935a219e` | 6 de 36 | `.text -8  bss+32` | -- | DOL ROTO |

Ninguna promociona. `madidct` y `steering` **no se han tocado** (mismo sha1 que
la r63); `zWorld2` solo lleva un comentario.

---

## 0. Lo aplicado, y donde

Seis ficheros, **los seis exclusivos de zWorld** (`grep -rln`: cada uno lo
incluye SOLO `SourceLists/zWorld.cpp`):

    src/Speed/Indep/SourceLists/zWorld.cpp
    src/Speed/Indep/Src/World/World.cpp
    src/Speed/Indep/Src/World/CarRender.cpp
    src/Speed/Indep/Src/World/SkyRender.cpp
    src/Speed/Indep/Src/World/CarInfo.cpp
    src/Speed/Indep/Src/World/CarLoader.cpp

mas **un comentario** en `src/Speed/Indep/Src/World/Common/WRoadNetwork.cpp`
(zWorld2) y **dos bloques de comentario** en `zWorld.cpp` con los negativos.

**Cero cabeceras compartidas. Cero `configure.py`, `config/`, `splits.txt`,
`symbols.txt`, `keep.lst`, `scripts/`.** `lcfix --check`: **zWorld 0 pendientes,
zWorld2 0 pendientes** (no se ha metido ni un literal C nuevo: solo `.asciz` de
`asm()`, que no renumeran ningun `$LC`).

---

## 1. La herramienta: marcadores de pool, y el mapa que dan

`textorder`/`ltorder` ordenan FUNCIONES. Para el `.rodata` no habia nada: el pool
es un mar de bytes sin simbolos, y `dolrod` dice que difiere pero no por que.

El mecanismo que lo abre es este, y esta comprobado:

> un `asm(".section .rodata\n .asciz \"...\"\n.previous\n")` a nivel de fichero
> se emite **en el punto del flujo de ensamblador donde esta escrito**, tambien
> si esta dentro de un `.cpp` que la SourceList incluye. El pool de `$LC` de la
> unidad se va formando alrededor.

**Prueba**: 470 marcadores `@@Fnnn` (uno detras de cada `}` a columna 0 de los
`.cpp` de la unidad, mas uno detras de cada `#include` del SourceList). En el DOL
enlazado **salen los 470 y salen EN ORDEN**, sin una sola inversion. Es decir:
dan el mapa `posicion-de-pool -> fichero:linea`, y cuesta **un enlace**.

Con ese mapa, colocar una cadena es mecanico:

1. se busca en el OBJETIVO el bloque contiguo de cadenas a mano;
2. se miran sus DOS vecinos compilados (los que nosotros tambien emitimos);
3. se traducen a posiciones NUESTRAS;
4. se busca el marcador que cae en medio. **Ese es el sitio.**
   Si no hay ninguno, el bloque va DENTRO de una funcion y un `asm()` de fichero
   no llega.

`scratchpad/world64/iter.py` hace los cuatro pasos (`mark` / `plan` / `apply` /
`restore`), y `pool.py` saca las dos secuencias de cadenas del DOL.

### 1.1 La escalera, con su cifra

| paso | cambio | `dolwhere` |
|---|---|---:|
| 0 | base | 24.884 |
| 1 | 65 cadenas a la frontera de `#include` (marcadores gruesos, 29) | 24.816 |
| 2 | 108 cadenas al fichero:linea exacto (marcadores finos, 470) | **23.513** |

**-1.371 B.** El paso 1 casi no paga y el paso 2 tampoco mucho: **el pool es
todo-o-nada**, igual que `matched_code`. Lo que la escalera compra no son los
1.371 B, es el DIAGNOSTICO de la seccion 2.

### 1.2 TRAMPA MEDIDA, y cuesta una vuelta entera

**Dos `asm()` seguidos en el mismo punto NO valen.** Con `Attrib::Gen::speech` en
su propio `asm()` justo delante del de `engineaudio`, `-strip-unused-data` se
llevo sus 20 B (`linkdelta` -> `rodata-24`) porque el trozo sin referencias se
estripa entero. Metidas las dos en **UN SOLO** `asm()`, sobrevive y `linkdelta`
vuelve a `IGUAL`. **Un bloque por punto, siempre.**

### 1.3 Las 30 que quedan, y por que no tienen sitio

Nueve bloques (30 cadenas). El motivo esta medido y es el mismo en los nueve:
sus dos vecinos del objetivo salen en NUESTRO pool o **pegados** (`prev=145
next=146` en `SceneryOverrideConn`) o **al reves** (`prev=576 next=162` en
`RemotePlayer`). O sea que **hay que arreglar antes el orden de lo COMPILADO**;
un marcador no cabe porque no hay hueco. La palanca para esas seria internar el
literal desde DENTRO de la funcion (`memory/nfsmw-primer-de-pool`), no un
`asm()` de fichero.

---

## 2. EL FRENTE DE VERDAD: los 472 B de la cabecera estan DUPLICADOS

Contado sobre el ENLACE (`scratchpad/world64/cnt.py`, ventana
`80409D58..8040E200`, que es la `.rodata` de zWorld mas el principio de zWorld2):

    GAMECUBE                  obj=3  nue=4
    Attrib::Attribute         obj=2  nue=3
    Attrib::Instance          obj=2  nue=3
    Attrib::Gen::simsurface   obj=2  nue=3
    WorldBodyConn             obj=1  nue=2
    Attrib::Gen::gameplay     obj=1  nue=2
    Attrib::Gen::audioimpact  obj=1  nue=2

(los `obj` de 2 y 3 son los prefijos de OTRAS unidades que caen en la ventana; el
saldo es **exactamente una copia de mas en cada una**).

Y se ve al ojo con `hexcmp.py`. El prefijo escrito a mano ocupa
`80409D58..80409F8C` (0x234 = 564 B) y **casa byte a byte con el objetivo**;
justo detras, en `80409F90`, empieza el pool de cc1plus:

    objetivo 80409F8C  Attrib::Gen::milestonetypes / 16.1.0 / 16.2.1 / 1.2.3 ...
    nuestro  80409F90  GAMECUBE / Attrib::Attribute / Attrib::Instance /
                       Attrib::Definition / Attrib::Class / Attrib::Database /
                       Attrib::TypeDesc / Attrib::RefSpec / Attrib::Blob / ...

que es **el arranque literal de los 472 B escritos a mano en `80409DB4`**.

O sea: cc1plus SI emite esas cadenas, y la extension del prefijo las escribe otra
vez delante. La cabecera del pool queda desplazada 472 B y, como el pool es un
mar de bytes, **eso descoloca todo lo que viene detras**. Los 12.774 B de
`$LC57` + `$LC227` + `$LC533` que `dolwhere` imputa al pool cuelgan de ahi.

### 2.1 Por que NO se ha quitado, y es la parte importante

    .size 0x234 -> 0x5C  (quitar los 472)              linkdelta  rodata-472  data-64
    lo mismo + `.space 472` en el bloque de la cola    linkdelta  rodata +0    data-64

**Los 472 B no son solo un duplicado: estan TAPANDO 472 B de contenido que la
unidad no emite en ningun sitio.** Por eso la `.rodata` media exacto con ellos.
Quitarlos sin escribir antes ese contenido rompe la seccion, y con
`data-64` ademas se descuadra la `.data` (aparece con la truncacion y **no lo
tengo explicado**: lo dejo escrito como lo que es, un cabo suelto).

### 2.2 El trabajo que queda, en orden

1. **Encontrar los 472 B que faltan.** No es adivinar: con la extension quitada,
   `hexcmp.py` compara las dos ventanas directamente y el hueco sale solo.
2. **Explicar el `data-64`.**
3. Quitar la extension y volver a colocar las 138 (`iter.py` lo rehace en tres
   ordenes).
4. Solo entonces atacar las 30 sin sitio con `primer de pool`.

Todo esta escrito tambien **en el fuente**, junto al prefijo y junto al bloque de
la cola de `zWorld.cpp`, para que `previo.py zWorld` lo encuentre.

---

## 3. zWorld2: el encargo manda repetir dos rondas de trabajo ya hecho

El encargo dice: *"La veda de la r48 busco su 'una insn menos' SOLO DENTRO del
bucle; tercera rama nunca evaluada: quitar UNO de los ocho movables entre
def(901) y def(505) ... PASO 0: sacar el `.greg` y escribir la lista de esas ocho
insns -- la r48 las conto y no las escribio."*

**Las dos afirmaciones son FALSAS, y las dos estan en el propio fuente**
(`WRoadNetwork.cpp:2026-2090`):

* **la r61 ESCRIBIO las ocho** (uids 3325, 3235, 3236, 3237, 3238, 3239, 3253,
  3268), con el cuadre al insn (`492-483 = 9 =` los ocho mas el propio def(505));
* **la r62 EVALUO la tercera rama y la CERRO**: reordenar el fuente no la mueve
  porque **el orden del preencabezado lo pone `sched1`, no `loop.c`**, y el
  volcado `.lreg` es POSTERIOR a `sched1`. Cinco variantes medidas, cuatro
  identicas a la base.

Lo unico que aporto es el **control de vigencia**, y es util porque el arbol se
ha movido: repetido hoy el volcado (`rtldump zWorld2 HolePunch -dl`), la ventana
sale **igual insn a insn**, con los mismos ocho uids. Lo unico que cambia es el
NUMERO del literal: el `0,2f` es hoy `*$LC551` y en la r61 era `*$LC425`.
Escrito junto a la funcion.

**Correccion de metodo para el que reparta**: antes de mandar un `PASO 0`,
`python scripts/previo.py <funcion>` -- que aqui lo habria dicho en 1 s.

Y una nota de ordenacion que no es mia: `linkdelta zWorld2` sigue en
`.text +0  bss+64`, y ese `bss+64` es el paquete **P3-A** de `r62-p3.md`
(la raya de `splits.txt` en `0x804AA560`). **Mientras no se aplique, zWorld2 no
puede ni medirse con `dolwhere`**, y es la unidad de mis cuatro que menos codigo
tiene pendiente (2 funciones, 4 y 12 insns).

---

## 4. madidct y steering: no tocadas, y por que

Las dos vienen de la r63 con el eje agotado y **el sello lo confirma**:
`steering 1962b7d1` es exactamente el sha1 que documento la r63, y
`madidct 63c4851d` el de la r62/r63. No he metido ni una medida nueva: con el
frente de `zWorld` abierto de par en par, gastar la ventana en 150 medidas mas
de pragmas de MWCC (la r63 ya hizo 150, todas negativas) no se sostiene.

Lo que queda escrito para el proximo, sin repetirlo:

* **madidct** esta a `.text -20  resto IGUAL`, y esos 20 B son **exactamente**
  los que le faltan a `IdctRow` (496/516). Es la unidad con menos distancia de
  seccion de mis cuatro. Lo que la bloquea es que el objetivo mantiene **CTR y
  LR vivos a la vez** (r44) y ninguna forma de fuente lo reproduce (r45).
* **steering**: `CookValues` sigue a **UNA transposicion** `r4<->r6` (260/260 B,
  99,08 %) y es la mas barata de las seis. La palanca nueva de la r63
  (`asm { li rN, 0 }`, que excluye `rN` del reparto por cero bytes) esta barrida
  en sus 63 subconjuntos y ninguno baja de 9 filas.

---

## 5. Regresiones comprobadas, no supuestas

    build_direct x3, iguales   zWorld 70e626bb  zWorld2 37eeecde
                               steering 1962b7d1  madidct 63c4851d
    fncmp   zWorld    4 de 582  (las MISMAS cuatro: RenderFlaresOnCar 480 insn,
                                 UpdateWheelYRenderOffset 79, DefragmentPool 23,
                                 SetMemoryPoolSize 2)
    fncmp   zWorld2   2 de 357  (HolePunchAvoidables 4 insn, InitAtSegment 12)
    fncmp   madidct   2 de 3    steering  6 de 36
    linkdelta  zWorld .text +0 IGUAL | zWorld2 .text +0 bss+64
               steering .text -8 bss+32 | madidct .text -20 IGUAL
    dolwhere   zWorld 24.884 -> 23.513 B
    lcfix --check                zWorld 0, zWorld2 0 pendientes
    gapchk  zWorld  total 5      (los 20 B PREVIOS de la r61/r62, sin cambio)
    UTF-8   los seis ficheros decodifican UTF-8 y tienen CERO bytes >127
    finales de linea             RESPETADOS uno a uno: World.cpp, SkyRender.cpp y
                                 CarInfo.cpp eran LF puro y siguen LF puro;
                                 CarRender.cpp, CarLoader.cpp, zWorld.cpp y
                                 WRoadNetwork.cpp eran CRLF puro y siguen CRLF puro
                                 (comprobado contando bytes, no con grep)
    cabeceras compartidas        NINGUNA tocada

---

## 6. Sorpresas

1. **Un `asm()` de fichero DENTRO de un `.cpp` de la SourceList vale como sonda
   de pool.** 470 marcadores en un enlace, y salen los 470 en orden. Es la
   primera herramienta que da el mapa `posicion-de-.rodata -> fichero:linea`, y
   se puede transponer a cualquier unidad con el `.rodata` descolocado.
2. **Dos `asm()` seguidos en el mismo punto se estripan; uno solo, no.** 20 B y
   una vuelta de reloj (1.2).
3. **El bloque "vocabulario compartido que el objetivo emite y cc1plus no" (138
   cadenas, c53) NO era eso.** cc1plus emite 472 B de ellas por su cuenta y el
   bloque de cabecera las escribe otra vez; lo que hacia era **tapar** un agujero
   de 472 B en otro sitio. La frase que justificaba el bloque llevaba mal desde
   la c53.
4. **El sha1 de un `.o` cambia sin que lo toques tu.** `zWorld` paso de
   `0162c8fa` a `70e626bb` con la fuente quieta y `dolwhere` clavado en 23.513:
   la causa es que **otro agente de esta ronda edito `WorldConn.h`** (guarda
   `WORLDCONN_H_IMPLICIT_PKT_BODY_OPEN_DTOR`, r64-phys), que zWorld incluye por
   `SmackableRender.cpp` y `VehicleRenderConn.h`. La guarda es neutra en codigo
   pero corre las lineas y con `-gdwarf+` eso cambia el objeto. **Un sello de
   sha1 solo vale contra una medida hecha en la misma ventana**; la que manda es
   `dolwhere`/`linkdelta`.
5. **`build_direct.py` en paralelo falla en silencio.** Lanzando las cuatro
   unidades a la vez, `zWorld2` dio `FAILED` con un error truncado a seis
   lineas; sola, compila. Y `build_direct zWorld` compila **zWorld y zWorld2**,
   porque casa por prefijo.
6. **El encargo puede mandar rehacer dos rondas.** El `PASO 0` de zWorld2 estaba
   hecho en la r61 y la rama estaba cerrada en la r62, las dos cosas escritas
   junto a la funcion (3).
