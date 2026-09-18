# r54 · `forense2`: los 17.269 B de `zMain` son **100 % orden y 0 B de contenido**

Va la cifra primero, y es un negativo con número: **no queda una sola palabra que
escribir en `zMain`**. Recolocando *nuestros* bytes en la secuencia de símbolos
del objetivo y enmascarando el campo de reubicación, de **39.617 palabras de
`.text` (1.378 símbolos) difieren CERO**. Lo mismo en `.rodata` y `.data`.

    seccion     simbs palabras   crudas  CONTENIDO  sin par
    .rodata       852     2118       50          0      661
    .data          43      811        0          0       13
    .text        1378    39617      809          0        0

Las 809 diferencias «crudas» de `.text` son mitades bajas de `@l`, `@ha` y
destinos de rama: campos que **el enlazador reescribe solo** en cuanto el símbolo
cae donde debe.

**No he tocado ni una línea del árbol.** Todo se ha medido con un objeto privado
compilado desde `git show HEAD:` (§6), fuera de `build/`. No hay `ninja`, ni
`configure.py`, ni commits, ni `git add`. Lo único que dejo escrito es
`scripts/reorden.py` (§5) — quitadlo si preferís que no esté.

---

## 1. La partición de los bytes, por naturaleza y por causa

Primero, la aritmética de la propia cifra: **`dolwhere` da 17.269 B pero los
bytes distintos exactos son 15.369**, en 4.359 palabras. `dolwhere` cuenta
*rangos* y funde dos diferencias separadas por menos de 12 bytes iguales. Uso
15.369 en todo lo que sigue.

| causa | palabras | bytes | % |
|---|---:|---:|---:|
| **R1** `~Listable<ActionQueue,20>::_List` (180 B) emitido 9.204 B tarde | 2.275 | **8.397** | 54,6 % |
| **R2** 40 símbolos `_IHandle__*` / `TypeName__*` / `LuaBinding` intercalados | 1.323 | **4.828** | 31,4 % |
| **R3** literal muerto `"EAGL4::SymbolEntry"` (+4 B) y un `0.0f` de pool que falta (−4 B) | 402 | **1.536** | 10,0 % |
| **R4** `"EventSequencerSystems"` y `"Scheduler"` mal colocados | 25 | 81 | 0,5 % |
| **R5** `_vt.11EAccelerate` ↔ `_vt.11InputDevice` | 17 | 55 | 0,4 % |
| **R6** 3 vtables de `FixedVector`/`Vector` rotadas | 18 | 34 | 0,2 % |
| **R7** 4 constantes de pool permutadas (`$LC637`..`$LC640`) | 4 | 5 | 0,0 % |
| **R8** dos parejas de vtable intercambiadas | 8 | 24 | 0,2 % |
| **SOMBRA** mitades bajas y ramas fuera de esas ventanas | 287 | 409 | 2,7 % |
| **contenido real** | 0 | **0** | **0 %** |
| | **4.359** | **15.369** | |

Y la sombra, repartida por el desplazamiento que la produce:

| delta | palabras | bytes | de quién es sombra |
|---:|---:|---:|---|
| `−180` | 88 | 140 | R1 |
| `+4` | 97 | 98 | R3 |
| 40 deltas más | 102 | 171 | R2, R5, R6 |

Sumando cada causa con su sombra: **R1 = 8.537 B (55,5 %)**, R2 ≈ 4.828,
**R3 = 1.634 B (10,6 %)**. Tres causas explican el **96,7 %**.

**Las clases del brief, contestadas una por una:**

* *mitades bajas de reubicación* — **409 B fuera de las ventanas permutadas**
  (2,7 %), más las 809 crudas de dentro. Todas son sombra, ninguna es problema
  propio.
* *entradas de vtable* — 37 B en `803F45B4..803F46B8` son punteros de los seis
  `_vt.Attrib::*_TypeHandler` que apuntan **−180** (sombra de R1), y 24 B son
  dos parejas de vtable intercambiadas (R8).
* *contenido real distinto* — **CERO**. Verificado dos veces: por recolocación
  (§2) y símbolo a símbolo (los 1.378 de `.text` tienen el mismo tamaño y el
  mismo contenido).
* *relleno de alineación* — cero; ninguna de las nueve secciones cambia de
  tamaño.

**39 de esas palabras (68 B) caen FUERA de `zMain`**: son `@l` y `bl` de
`zMisc`, `zPhysics`, `zSim` y `zAI` apuntando a símbolos de `zMain` que se
mueven (`Construct__9ExplosionG…`, `Place__16PlaceableScenery…`, `__7SoundAI`…).
El desorden de una unidad **se mide en el `.text` de las vecinas**.

## 2. La prueba: la región grande es una ROTACIÓN pura de 180 B

`801F844C..801FA8F4` son 9.384 B y 98 símbolos. El objetivo los emite
`[destructor 180 B][resto 9.204 B]` y nosotros `[resto 9.204 B][destructor
180 B]` — una rotación. Girando nuestros bytes y enmascarando el campo de
reubicación:

    region B rotada: 2346 palabras, 405 distintas crudas,
                     0 distintas ignorando el campo de reubicacion

**Cero.** Los 8.397 B son íntegramente la sombra de **una función de 180 B en la
ranura equivocada**. El orden relativo de las otras 97 es idéntico: sólo
`~UTL::Collections::Listable<ActionQueue,20>::_List` se mueve, del principio del
bloque al final.

    objetivo                                   nuestro
    801F844C  180  ~Listable<ActionQueue,20>   801F844C  164  BuildMessageTable__MBreakerStopCops
    801F8500  164  BuildMessageTable__MBreak…  801F84F0  180  ~Instanceable<HENGINE,IEngine,434>
    801F85A4  180  ~Instanceable<HENGINE,…>    801F85A4  192  BuildMessageTable__MNotifyEngineBlown
    …                                          …
    801FA8EC    8  GetMaxCapacity<…>           801FA838    8  GetMaxCapacity<…>
    801FA8F4    8  AllocVectorSpace<…>         801FA840  180  ~Listable<ActionQueue,20>

La región R2 (`801F20E4..801F35B0`, 40 símbolos) da lo mismo: **0 de 1.332
palabras** difieren una vez recolocada. Ahí no hay una función suelta sino un
entrelazado distinto de `_IHandle__*` (12 B), `TypeName__*` (100 B),
`ConnectionClass/Size/Type/~Pkt_Body_Send` y los `HandleMessage_LuaBinding__*`
(252 B). Los dos que más se van: `TypeName__15SmackableParams` **+3.580 B** y
`TypeName__15ExplosionParams` **+1.896 B**.

## 3. El mecanismo de la clase mayor: dos colas distintas, no una

`zMain` está entero en la **segunda pasada**, y la segunda pasada de GCC 2.95.3
no es una lista: son varias colas que se drenan por turnos. `cp/decl2.c:3643
finish_file` es un `do { … } while (reconsider)` con este orden fijo dentro:

1. `instantiate_pending_templates()` — cuerpos de plantilla
2. `walk_globals(vtable_decl_p, finish_vtable_vardecl)` — **vtables**
3. `static_aggregates` + la función de inicialización estática
4. `synthesize_method` sobre los *inline* artificiales
5. `wrapup_global_declarations(saved_inlines)` — **los cuerpos diferidos**
6. `wrapup_globals_for_namespace`, `pending_statics`

Y la cola del paso 1, en `cp/pt.c:9540`, se recorre **en orden de lista**. La
lista la construye `add_pending_template` (`pt.c:3559`), que **añade por la cola**
(`template_tail`) y marca `TI_PENDING_TEMPLATE_FLAG`:

```c
  if (TI_PENDING_TEMPLATE_FLAG (ti))
    return;                       /* ya esta en la cola: no se vuelve a meter */
  *template_tail = perm_tree_cons (build_srcloc_here (), d, NULL_TREE);
```

De ahí salen las dos consecuencias que mandan en R1:

* **La posición de un cuerpo de plantilla la decide el punto del análisis en que
  se pidió por PRIMERA VEZ la instanciación.** No la declaración, no el
  `#include`: el primer *uso* que obliga a tener el cuerpo.
* **Sólo cuenta el primero.** `TI_PENDING_TEMPLATE_FLAG` convierte cualquier uso
  posterior en un no-op, así que un uso añadido *después* no mueve nada.

**Y aquí está por qué la r53 no lo cerró.** La vtable sale del paso 2 —
`walk_globals`, o sea la lista de declaraciones del espacio de nombres, ordenada
por cuándo se completó la CLASE— y el cuerpo del destructor sale del paso 1,
ordenado por cuándo se pidió la INSTANCIACIÓN. Son colas distintas. El
`#include ActionQueue.h` de la r53 §3 movió la primera (emparejó las dos vtables,
`permorden` 15 → 11) y dejó la segunda intacta: el destructor sigue 9.204 B
tarde. **No es que el arreglo fuera insuficiente; es que actuaba sobre la cola
que no era.**

Corolario operativo para la r55: para R1 hay que **adelantar un uso** de
`Listable<ActionQueue,20>::_List` que obligue al cuerpo (una destrucción, o un
`delete`, o cualquier cosa que instancie el destructor) al punto del preprocesado
que va justo detrás de `BuildMessageTable__16MAudioReflection`. Adelantar la
declaración de la clase no vale, y añadir un uso detrás del primero tampoco.

## 4. R3 y R4: la firma de zSim, calcada — dos ediciones que suman CERO

El brief preguntaba si `zMain` tiene la firma de `zSim` (ediciones cuyos
desplazamientos se compensan). **La tiene, y está en el `.rodata`.** Alineando
la ventana byte a byte:

    =   111 B  hasta 803EF92F   (desfase +0)
    SOBRA 803EF92F    4 B  |.ry.|           desfase +4
    =  1605 B  hasta 803EFF74   (desfase +4)     <-- 1.536 B distintos AQUI
    FALTA 803EFF74    4 B  |....|           desfase +0
    =    72 B  hasta 803EFFC0   (desfase +0)

Dos ediciones, **+4 y −4**. Por eso `linkdelta` dice `IGUAL` mientras hay 1.536 B
distintos entre ellas. Las dos identificadas:

* **Los +4 B son `"EAGL4::SymbolEntry"`**, 19 B, literal MUERTO de cabecera
  (`Src/EAGL4Anim/eagl4supportdef.h:42`, dentro de `HAND_POOL_TAG`). Está en
  nuestro `.rodata` del objeto en el offset 944, entre `"EAudioSmackableTest"` y
  `"MAudioReflection"`. `-strip-unused-data` le quita `19 & ~7 = 16` B **por la
  cabeza** y sobrevive el rabo `"ry\0"` + 1 B de relleno = los 4 bytes de
  `803EF930`. El objetivo no lo tiene: `zMisc.cpp`, `zRender.cpp` y `zSim.cpp` ya
  lo apagan con `#define HAND_POOL_TAG(s) ((const char *)0)`; **`zMain.cpp`
  define `DEAD_STR` pero no `HAND_POOL_TAG`**.
* **Los −4 B son una palabra `0.0f` de pool** que falta en `803EFF74`. El
  objetivo tiene ahí `{100.0f, 0.0f, 176.0f, -0.0f, 1/65536}` y nosotros
  `{100.0f, 176.0f, -0.0f, 1/65536}`. Es el pool de uno de los eventos
  `EFireEventList`/`EFireRandomTrigger`.

**AVISO, medido, y es la lección de zSim §3.3 otra vez**: arreglar sólo una de las
dos mitades **empeora la cifra de bytes muchísimo**. Si se quita el literal sin
poner el `0.0f`, la `.rodata` de `zMain` sale 4 B corta y se desplazan los
**18,7 kB** que van de `803EFF74` al final de la ventana. El par es **atómico**.

Y el negativo con cifra: probé el remedio bruto —`#define HAND_POOL_TAG(s)
((const char *)0)` en una copia privada de `zMain.cpp`— y da **`.rodata −512`**,
no −4. La macro apaga muchos más literales de los dos que interesan y además
renumera los `$LC`, con lo que `keep.lst` queda rancio. **La palanca tiene que
ser quirúrgica** (sombrear sólo esa llamada de `eagl4supportdef.h`), no la macro
entera.

R4 es el mismo patrón, 81 B: `"EventSequencerSystems"` la emitimos en
`803F22E0` y el objetivo en `803F2304` (36 B más allá) porque nosotros la
internamos al analizar `EventSequencer.h:200` —también bajo `HAND_POOL_TAG`— y el
objetivo al llegar a `EventSequencer.cpp:626`, donde el literal está escrito a
pelo. Y donde el objetivo tiene `"Scheduler"` (10 B) nosotros tenemos el rabo
`"r.cpp\0"` de `"Scheduler.cpp"` (14 B, `14 & ~7 = 8` estripados), que sale del
`BNEW` de `Scheduler.cpp:30` (`#define BNEW new (__FILE__, __LINE__)`).

R5/R6/R7/R8 son permutaciones puras de 114 B en total, todas con los dos
elementos del mismo tamaño (por eso ninguna medida de tamaño las ve):

* R5 `_vt.11EAccelerate` (32 B) y `_vt.11InputDevice` (128 B) cambiados de sitio.
* R6 rotación de `_vt.FixedVector<_KeyedNode,434,16>`,
  `_vt.FixedVector<ActionQueue*,20,16>` y `_vt.Vector<ActionQueue*,16>` (64 B cada una).
* R7 el pool de `803F0608`: el objetivo `{1.0, 0.5, 0.25, 0.75}`, nosotros
  `{0.25, 1.0, 0.75, 0.5}` — orden de CREACIÓN, o sea orden de sentencia
  (`varasm.c:3797`).
* R8 `_vt.12EEngineBlown` ↔ `_vt.20EShowRaceOverMessage` y `_vt.9ECellCall` ↔
  `_vt.13ESndGameState`, las cuatro de 32 B (ya las listaba la r53 §3).

## 5. `scripts/reorden.py`: la medida que separa «me falta código» de «lo tengo mal colocado»

Ninguna herramienta del proyecto contestaba esa pregunta. `fncmp` compara el
objeto función a función y no ve la posición; `linkdelta` compara tamaños;
`dolwhere` cuenta bytes en la dirección en que caen, así que un bloque desplazado
sale como «contenido distinto» de punta a punta; `permorden` cuenta funciones
fuera de sitio pero no dice si su contenido es el bueno.

`reorden.py` enlaza dos veces, toma **la secuencia de símbolos del objetivo**,
vuelve a colocar **nuestros** bytes en ese orden y compara **enmascarando el
campo de reubicación** (mitad baja de una forma D, desplazamiento de rama).

    python scripts/reorden.py zMain
    python scripts/reorden.py zMain .rodata
    python scripts/reorden.py zMain 801F844C 801FA8F4
    python scripts/reorden.py zMain --o <ruta a un .o privado>

`CONTENIDO 0` significa *no queda nada que escribir*. Comprobado en una segunda
unidad, y ya dice algo: **`zFe2` NO está como `zMain`**.

    zFe2   .text  1306 simbs  62404 pal   6763 crudas   CONTENIDO 0  (17 de otro TAMANO)
           .rodata                         529 crudas   CONTENIDO 20
           .data                           401 crudas   CONTENIDO  3

Su `.text` también es orden puro, pero le quedan **23 palabras de contenido real**
en `.data` (`Minimap.kGameplayIconInfo`, `ScreenFactoryData`, `gDialogHandle`) y
en once vtables. Merece la pena pasarlo por las trece del bloque A: separa en una
corrida las que son sólo orden de las que aún tienen dato que escribir.

## 6. Cómo se ha medido, y una trampa de coordinación que cuesta una tarde

**`build/GOWE69/src/<unidad>.o` NO es una base estable con seis agentes en el
árbol.** A mitad de sesión otro agente recompiló `zMain.o` y mi medida saltó de
**15.369 B a 110.064** sin que yo tocara nada. La cifra estaba mal durante los
minutos que tardé en darme cuenta.

El remedio, y funciona:

    git show HEAD:src/Speed/Indep/SourceLists/zMain.cpp  >  <priv>/zMain_head.cpp
    ngccc <los cflags que build_direct saca de build.ninja> -c -o <priv>/head/zMain.o …
    python scripts/reorden.py zMain --o <priv>/head/zMain.o

**Y el objeto TIENE que llamarse `zMain.o`.** `keep.lst` empareja por el
*basename* del objeto (`zMain.o:$LC457`); con el fichero llamado `zMain_head.o`
las entradas no casan **en silencio**, los literales se estripan y `linkdelta`
sale `rodata −1056`. Perdí una medida entera por eso. (Por lo mismo hay que
poner `SN_NGC_PATH` al directorio del toolchain, como hace `build_direct.py:131`,
o `ngccc` no arranca.)

Con esa base privada, el `zMain` de `HEAD` reproduce **15.369 B exactos**.

## 7. Tres herramientas que engañan en esta unidad

1. **`movidos.py` resta como ARRASTRE justo el racimo que hay que ver.** En
   `zMain` dice `Arrastre por seccion: .rodata−32, .text−180` y descuenta los
   100 símbolos que se mueven ese `−180`… que son exactamente el bloque de R1.
   `linkdelta` dice `IGUAL` en las nueve secciones, así que **ahí no hay ningún
   arrastre**: la moda no es deriva de sección, es la causa. Con `zMain` el
   resumen queda en «83 permutados de verdad» cuando los desplazados son 183.
2. **Las filas `$LC` de `movidos` son basura.** `simbolos()` indexa por nombre y
   `$LCnnn` **colisiona entre objetos**: en el enlace base `$LC62` está en
   `0x803D0DC4`, fuera de la ventana de `zMain` (`803EF580..803F4878`), o sea que
   es de otra unidad. De ahí salen las filas de `+112.000` y `+124.000` B, y de
   ahí sale el `.rodata−32` del punto anterior. Emparejar `$LC` por nombre entre
   dos enlaces **no se puede hacer** sin filtrar por ventana.
3. **`permorden` «11 desplazadas» no son 11 causas.** Es el complemento de la
   subsecuencia creciente más larga. Lo que hay que leer son los **ciclos**:
   98/12/10/7/5/4/2, y mapean uno a uno sobre las regiones de §1 (el de 98 es R1,
   los de 12/10/7/5/4 son R2).

Y una cuarta, menor: **`dolwhere` da 17.269 B donde los bytes distintos son
15.369** porque funde rangos separados por menos de 12 bytes iguales. Al comparar
con informes viejos, mirad cuál de las dos cifras es.

## 8. Lo que le queda a `zMain`, ordenado por bytes

| # | qué | bytes | cuesta |
|---|---|---:|---|
| 1 | R1: adelantar el **primer uso** de `~Listable<ActionQueue,20>::_List` a la ranura de detrás de `BuildMessageTable__16MAudioReflection` | **8.537** | una sentencia, si se acierta el punto |
| 2 | R2: 40 símbolos (`_IHandle__*`, `TypeName__*`, `LuaBinding`) entrelazados de otra forma; entrada natural, `TypeName__15SmackableParams` (+3.580) y `TypeName__15ExplosionParams` (+1.896) | **4.828** | sin diagnosticar |
| 3 | R3: quitar `"EAGL4::SymbolEntry"` **y** poner el `0.0f` de pool de `803EFF74`. **Paquete atómico**; sólo una mitad desplaza 18,7 kB | **1.634** | `HAND_POOL_TAG` quirúrgico + un `asm()` en la función que crea ese pool + `lcfix` |
| 4 | R4/R5/R6/R7/R8: cinco permutaciones de tamaño idéntico | 199 | — |
| | **contenido que escribir** | **0** | — |

## 9. Coincidencia (y discrepancia) con `forense1`

No he leído su hipótesis mientras medía, como pedía el encargo. Por si sirve al
cotejo, esto es lo que digo yo desde los bytes:

* **coincide** que hay un destructor mal colocado y que es la causa mayor —
  aporto que vale **8.537 B, el 55,5 %**, y que la región es una **rotación
  pura**: cero contenido distinto;
* **añade** que **no es la única**: R2 vale otro 31,4 % y no tiene nada que ver
  con ese destructor (está 25 kB más arriba y sus 40 símbolos se mueven con
  cuarenta deltas distintos), y R3 vale un 10,6 % y es un par de ediciones
  `+4/−4` en el `.rodata`;
* **contradice** cualquier lectura de `movidos` que trate el `−180` como
  arrastre: no lo es (§7.1).

## 10. Reglas

Nada de `ninja` ni `configure.py`; sólo `ngccc` sobre una copia privada y el
enlazador. Ni un commit ni un `git add`. `configure.py`, `config/GOWE69/*`,
`splits.txt` y `keep.lst` **sin tocar**. Cero cabeceras compartidas y cero
ficheros del árbol modificados: el único fichero que dejo escrito es
`scripts/reorden.py` (§5) y este informe. `fncmp` de `zMain` sigue en **0 de
1.380** por construcción — no he cambiado una función. Volcados borrados.
