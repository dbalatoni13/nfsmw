# Ronda 23 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes ganados: 0.** `measure.py --cmp` sobre zCamera: **+0 B, +0 funciones**.
Lo que sí se mueve: `TrackCarCameraMover::Update` **99,56855 -> 99,62904**
(+0,06 pp, tres diffs cerrados) y dos correcciones más byte-neutras confirmadas
por DWARF. Ninguna función baja.

Lo que traigo que sirve para la ronda siguiente son **tres vetos medidos**, **un
mecanismo compartido por las dos funciones más cercanas**, y **una discrepancia
de compilador que afecta a 28 unidades de compilación fuera de zCamera**.

## 0. El encargo estaba VIGENTE, y `audit.py` limpio

`build_direct.py zCamera` + `triage.py zCamera` reproducen el brief clavado:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2...  faltan 1, sobran 2, 1 SUST
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 3868 ICEMover::Update / 1192 TerrainVelocityNoise / 992 TrackCar / 948 TrackCop

`audit.py Speed/Indep/SourceLists/zCamera`: **446/446 ok, cero FALLA**, dos
pasadas idénticas, antes y después. **Lista de FALLA: vacía.**
`pines.py`: ningún pin de zCamera en función que no case (siguen siendo los dos
de siempre, `DynamicLoader::Initialize` y `WRoadNav::InitAtSegment`).

**Nota de lectura de `triage.py`** (no estaba escrita en ninguna parte y cuesta
media hora deducirla): en la columna de opcodes, **`op-N` = tenemos N de MÁS**,
**`op+N` = nos faltan N**. Verificado contra el diff instrucción a instrucción
de `_Storage` (`cmpwi-1, lwz-1, mr.+1`) y de la static-init (`addi-4`).

## 1. VETO MEDIDO: `-fno-force-addr` NO es la palanca de la static-init

La r21 midió que `-fno-force-addr` **pone a cero** los cuatro `addi` de más de
`__static_initialization_and_destruction_0`, y lo dejó como pista abierta. Lo he
medido **sobre la unidad entera** (compilando zCamera al scratchpad con los
cflags reales menos esa bandera; el arnés reproduce el `.o` del árbol **con el
mismo md5**, así que la medida es válida):

    con -fforce-addr (estado actual)   120.821/145.005 B   83,3220 %   609 fn al 100 %
    con -fno-force-addr                 38.077/145.005 B   26,2591 %   477 fn al 100 %

**−82.744 B.** La bandera es correcta y el frente está cerrado: no hay que
volver a mirarlo, ni por unidad ni por `#pragma` (GCC 2.95 no tiene atributo
`optimize`, así que tampoco existe la vía por función).

## 2. El mecanismo que comparten las DOS funciones más cercanas

`_Storage` (1.156 B, 97,834 %) e `ICEMover::Update` (3.868 B, 99,907 %) tienen
**la misma causa raíz**, y no estaba identificada:

> Donde hay una **copia de registro** (`mr rC, rS`) y después un uso del valor,
> **el objetivo usa el registro ORIGEN `rS` y nosotros usamos la COPIA `rC`.**

    _Storage   objetivo: mr. r27,r0 ; subf r9,r0,r24     (subf usa srcBeg)
               nuestro : mr  r27,r9 ; subf r0,r27,r24    (subf usa srcIt)
    ICEMover   objetivo: lwz r8,0x15c(r1) ; mr r11,r8 ; cmpwi r8,0
               nuestro : lwz r9,0x15c(r1) ; mr r11,r9 ; cmpwi r11,0

En `_Storage` eso es **exactamente** lo que cuesta los 4 B: con el `subf` usando
`srcBeg`, nada usa r27 entre el `mr` y el `cmpwi`, y **`combine` funde los dos en
`mr. r27,r0`**. Con el `subf` usando `srcIt`, `reg_used_between_p (i2dest, i2, i3)`
es cierto y `combine` rechaza. Un instrucción, 1.160 -> 1.156 B.

**Quién decide**: `canon_reg` de `cse.c` reescribe todo registro a
`qty_first_reg[]` de su cantidad, y quién es el canónico lo fija `make_regs_eqv`:

    la COPIA se hace canónica  sii  (su ultimo uso cae MAS ALLA del bloque basico
    de CSE  ||  su primer uso es anterior al bloque)  &&  (su ultimo uso es
    posterior al ultimo uso del ORIGEN)

En los dos casos la copia sobrevive al origen, así que la única salida es que
**el último uso de la copia caiga DENTRO del bloque básico de CSE**. Eso no lo
decide una sentencia: lo decide hasta dónde llega `cse_end_of_basic_block`
siguiendo saltos (`-fcse-follow-jumps` / `-fcse-skip-blocks`).

**Es el mismo fenómeno dos veces y en la misma dirección.** Quien lo retome
debería atacarlo por ahí (volcado `.cse2` con la receta de `r22-ae.md` §2 para
ver dónde acaba el bloque), no barriendo formas de fuente: yo he barrido cinco
y ninguna lo mueve (§4).

## 3. Lo que SÍ se queda (tres correcciones, +0,06 pp)

### 3.1 `TrackCarCameraMover::Update`: 99,56855 -> 99,62904 (ensayo t1)

`dwbody.py` sobre esta función **no se había pasado nunca** (la r22 la dejó
explícitamente sin abrir). Da **cinco** diferencias estructurales. La primera
cierra tres diffs:

    original DWARF:  unsigned short fov;        <- SIN registro
    nuestro       :  unsigned short fov; // r11

    -        fov = bClamp(fov, 800, 13100);
    -        pCamera->SetFieldOfView(fov);
    +        pCamera->SetFieldOfView(bClamp(fov, 800, 13100));

Sin la reasignación, `fov` deja de ser un pseudo con vida propia y cae en r0 como
en el objetivo. Cierra los índices 81/83/86 (`clrlslwi. r0,r3,17,1`,
`cmpwi r0,0x320`, `mr r9,r0`). **99,56855 -> 99,62904, SE QUEDA.**

Quedan 15 diffs, todos permutación de registros de coma flotante en tres grupos:
`Look = *pos` (107-111), `displacement /= distance` (113-123) y
`look_offset.x/y/z` (134-143).

### 3.2 `TrackCar`: el `bFill` era un `bScale`, igual que en TrackCop (t3)

La misma corrección que la r22 hizo en `TrackCop.cpp` (su c16) **hacía falta
también aquí**, y nadie la había mirado. El DWARF del original tiene
`bScale(dest, v, scale)` con locales `x,y,z` **entre los dos
`bVector3::bVector3()`**, o sea justo detrás del `bCross`:

    -    bCross(&hcomp, &displacement, &up);
    -    bVector3 look_offset; ...
    -    bFill(&hcomp, horiz_comp, horiz_comp, vert_comp);
    +    bCross(&hcomp, &displacement, &up);
    +    bScale(&hcomp, &hcomp, vert_comp);
    +    bVector3 look_offset; ...

**`.text` byte a byte idéntica** (con `-ffast-math`, `x*0.0f` pliega y las cargas
mueren). **SE QUEDA** por fidelidad, igual que se quedó la de TrackCop.

### 3.3 `GameFlow.hpp` estaba incompleta: faltan DOS envoltorios libres (t4/t5)

El original tiene **cuatro** funciones libres en `GameFlow.hpp`; nosotros
teníamos dos. Conteo en `symbols/mw_dwarfdump.nothpp`:

    inline bool IsGameFlowInFrontEnd()    36 apariciones   LA TENIAMOS
    inline bool IsGameFlowInGame()        46 apariciones   LA TENIAMOS
    inline bool IsGameFlowLoadingGame()   21 apariciones   FALTABA
    inline bool IsGameFlowPaused()        27 apariciones   FALTABA

Añadidas (en el sitio que dice el volcado, detrás de `IsGameFlowInGame`) y usadas
en los dos sitios que el DWARF confirma: `TrackCar.cpp:316` e `ICEMover.cpp:516`
(`dwbody` señalaba `inline bool IsGameFlowPaused() {}` en las dos). **Byte-neutro
en zCamera**; era una de las dos cosas que la r21 dejó como «NO probada».

Los otros dos sitios del árbol que llaman a `TheGameFlowManager.IsPaused()`
(`CameraAI.cpp:253`, `Cubic.cpp:824`) **NO los he tocado**: no tengo confirmación
de DWARF para ellos.

**A/B obligatorio de la cabecera compartida.** `GameFlow.hpp` la ven 43 ficheros,
que son 13 SourceLists: zCamera zMisc zWorld zFe zFe2 zFeOverlay zEcstasy
zGameplay zLua zSim zPlatform zMain zTrack. Compiladas las 12 que no son zCamera
**con y sin** las dos funciones nuevas y comparadas
`.text`/`.rodata`/`.data`/`.sdata`/`.sdata2`/`.rela.text` byte a byte:
**10 de 12 idénticas a la primera, y las otras dos idénticas también al aislarlas
bien** (ver §3.4). **Resultado: byte-neutro en las 12.**

Y en zCamera, comparación **por función** sobre los 621 símbolos de la unidad,
antes contra después de las tres correcciones:

    EMPEORAN: 0     MEJORAN: 1 (TrackCar 99,56855 -> 99,62904)     simbolos nuevos: 0

### 3.4 AVISO DE MÉTODO: con otros agentes trabajando, el A/B por objetos MIENTE

La primera pasada del A/B dio **zEcstasy** y **zMain** «DISTINTO», con ±4 B de
`.text`. Antes de creérmelo repetí en secuencia **con / sin / con**:

    zEcstasy   con1 == sin == con2   -> era CHURN DEL ARBOL, no mi cambio
    zMain      con1 != sin != con2   -> las TRES distintas

Y entonces medí lo que hacía falta medir: **compilé zMain dos veces seguidas sin
tocar nada**. Resultado: `.text` **de 179.964 B las dos veces pero con 36 bytes
distintos**, primera diferencia en el desplazamiento 0x117C1, con `.rodata`,
`.data` y `.rela.text` idénticas.

**No es el compilador.** Preprocesando zMain a un `.ii` y compilando ESE MISMO
`.ii` dos veces, el `.text` sale **byte a byte idéntico**: `ngccc`/`cc1plus` es
determinista. Lo que cambia entre una compilación y otra es **el árbol**: hay
otros agentes editando cabeceras que zMain incluye, y una compilación de zMain
tarda lo bastante como para caer a caballo de una edición ajena.

**La consecuencia práctica, que vale para cualquier ronda con agentes en
paralelo:** el A/B de una cabecera compartida hay que hacerlo **sobre ficheros
preprocesados**, no compilando el árbol dos veces. Preprocesar con y sin el
cambio (`-E`), **comprobar que el `diff` de los dos `.ii` es exactamente tu
cambio**, y compilar los dos `.ii`. Hecho así:

    zMain     .ii difiere en mis 7 lineas (+ un marcador `# 165 Gameflow.hpp`)
              .text  con=94d29c8c5dae (179964)  sin=94d29c8c5dae (179964)  IDENTICO
    zEcstasy  .ii difiere en mis 7 lineas
              .text  con=e3319654afd5 (154840)  sin=e3319654afd5 (154840)  IDENTICO

Si alguien midió una regresión de pocos bytes en zMain en las rondas 21-23,
**debería repetirla así antes de darla por buena**.

## 4. Lo que se ha probado y NO cierra (ensayos numerados)

    f1   -fno-force-addr sobre la UNIDAD ENTERA        83,3220 -> 26,2591 %   VETO (§1)

    s1   _Storage: size() en una local en la rama nula
         (`size_type currentSize = size(); ... currentSize = size();`)
                                          97,83391 -> 97,83391  (1160 B) SIN EFECTO
    s2   _Storage: `minSize` declarada antes que `srcIt` (= c14/c17 de la r22)
                                          97,83391 -> 96,60554  (1152 B) REVERTIDO
    s3   s2 + s1                          97,83391 -> 96,60554  (1152 B) REVERTIDO
    s4   _Storage: el test `if (srcIt==0)` ANTES de minSize, calculando
         `minSize` dentro de las dos ramas (esperaba que PRE lo rehoistara)
                                          97,83391 -> 21,07266            REVERTIDO
    s5   _Storage: rama nula = `resize(minSize)` (= c11 de la r22)
                                          97,83391 -> 94,68858  (1184 B) REVERTIDO
    s6   _Storage: `Contains()` nueva en UTLVector.h + la forma EXACTA del DWARF
         `if (minSize > capacity()) { if (Contains(srcBeg)) make_empty();
          reserve(minSize); }`            97,83391 -> 95,35986  (1180 B) REVERTIDO
    s7   s6 sin el guardian exterior (= c15 de la r22)
                                          97,83391 -> 95,82699  (1172 B) REVERTIDO

    t1   TrackCar: `SetFieldOfView(bClamp(fov,800,13100))` sin reasignar `fov`
                                          99,56855 -> 99,62904            SE QUEDA
    t2   TrackCar: `unsigned short fov = 2*bATan(...)` en el punto de uso
                                          99,56855 -> 99,56855            REVERTIDO
    t3   t1 + `bScale(&hcomp,&hcomp,vert_comp)` detrás del `bCross`
                                          99,62904 -> 99,62904, .text IDENTICA  SE QUEDA
    t4   t3 + IsGameFlowPaused()/IsGameFlowLoadingGame() en GameFlow.hpp,
         usada en TrackCar.cpp            99,62904 -> 99,62904            SE QUEDA
    t5   t4 + usada tambien en ICEMover.cpp
                                 ICEMover 99,90693 -> 99,90693            SE QUEDA

    m0,a..k  LoadCameraShakes: DOCE formas del bucle sobre un caso minimo de
             20 lineas con los cflags reales (2 s por compilacion), ver §5.
             LAS DOCE dan el MISMO ensamblador.                           VEDA

**La rama no nula del original, sacada del DWARF con rangos** (esto sí es nuevo:
la r22 sólo tenía las líneas del `lmap`). Entre el final de `resize` (0x80081A44)
y el `begin()` de la copia (0x80081AF8) el original tiene, todos con **rango
CERO** salvo el `reserve`:

    capacity()                        0x80081A48 -> 0x80081A48   <- if (minSize > capacity())
    Contains(p) { int index; capacity(); }   0x80081A50 (cero)
    make_empty() { int num; size(); { int ii; pop_back(){size();} } }  0x80081A50 (cero)
    reserve(num)                      0x80081A50 -> 0x80081AF8
    begin()                           0x80081AF8 -> 0x80081AFC

O sea la forma del original es **`if (minSize > capacity()) { if (Contains(srcBeg))
make_empty(); reserve(minSize); }`**, con el `reserve` DENTRO del guardián. Escrita
así (s6) cuesta **+20 B**: el guardián exterior emite `lwz r0,0x4(r31)` +
`cmplw r26,r0` + `ble` (12 B) porque es redundante con el `if (num > capacity())`
que ya lleva `reserve` dentro y GCC 2.95 no lo funde. Sin el guardián (s7) cuesta
**+12 B**, exactamente el número de la c15 de la r22. **Veda 3 de la r22
confirmada y ampliada**: no es sólo que `Contains` emita, es que **la forma
literal del DWARF es la peor de las dos**.

**Verificado que el ensayo s2 hace lo que la r22 dijo**, con el diff delante:
aparece `mr. r27,r9` y el `subf` **se pliega** — `minSize` pasa a calcularse como
`(mSize*4)>>2`, o sea CSE resuelve `(srcBeg + mSize*4) - srcBeg`. El objetivo
**tiene las dos cosas a la vez** (`add r24,r0,r9` ; `mr. r27,r0` ; `subf r9,r0,r24`),
y no he encontrado forma de fuente que lo dé.

## 5. `LoadCameraShakes` (168 B): el bucle QUEDA VEDADO, con doce formas más

El objetivo materializa el cero **dos veces**; nosotros una:

    objetivo:  lwz r28,0x8(r29) ; li r0,0 ; addi r30,r29,0xc ; li r27,0 ; cmpw r0,r28
    nuestro :  lwz r27,0x8(r29) ; li r28,0 ; addi r30,r29,0xc ;           cmpw r28,r27

`lmap` dice que **los dos `li` llevan la línea 6816, la del `for`**, y que la
guarda de entrada del bucle (`cmpw`+`bge`) compara un registro que **no es el
contador**. O sea: CSE reescribió el `i` de la guarda a otro pseudo de la misma
cantidad (`qty_first_reg`), y por eso `warned_overflow` acaba en r0 en el DWARF.
**Es el mismo mecanismo de la §2, en la otra dirección.**

Lo nuevo de esta ronda es el método: un **caso mínimo de 20 líneas** con los
cflags exactos de zCamera (2 s por compilación, `c23cam_min.py` +
`c23cam_sweep.py` en el scratchpad) que **reproduce nuestra forma clavada**. Con
él he barrido DOCE variantes:

    m0  for (int i = 0; i < n; i++)                   <- linea base, 1 li
    a   + bool warned_overflow = false; al principio        identica
    b   int zero = 0; for (int i = zero; ...)               identica
    c   int i = 0; fuera del if(group)                      identica
    d   if (i<n) { do {...; i++;} while (i<n); }            identica
    e   int j = 0; ... j = j; despues del bucle             identica
    f   bool warned_overflow; asignada dentro del if        identica
    g   int i = 0; while (i < n) { ...; i++; }              identica
    h   for (int i = 0; n > i; i++)                         CAMBIA (canonicaliza, peor)
    i   warned_overflow + `if (warned_overflow) {}` detras  identica
    j   unsigned id = GetID(...) muerta delante             identica
    k   int i; for (i = 0; ...)                             identica

**Las once primeras dan ensamblador IDÉNTICO** (mismo recuento de instrucciones,
mismo `li 0` único, mismo `cmpw`). Sumadas a las diez de la r21/r22 van **22
formas**. **El frente «forma del bucle / posición de `warned_overflow`» está
agotado**: lo que falta no es una sentencia, es el desempate de `canon_reg`.

## 6. Corrección a la ronda 21: la static-init NO casa 1:1 hasta el 795

La r21 escribió: «coincide instrucción a instrucción con el objetivo desde el
índice 0 hasta el 795». **No es cierto: hay seis diffs en 549-564**, mucho antes
de la tabla:

    549 li r9, 0x38e                    | lis r9, $LC1114@ha
    557 lis r10, HydraulicsLookAngle@ha | li r10, 0x38e
    558 lfs f24, lbl_803D3A80@l(r11)    | lfs f24, $LC1114@l(r9)
    561 lis r11, NOSFovWidening@ha      | lis r9, NOSFovWidening@ha
    562 sth r0, NOSFovWidening@l(r11)   | sth r0, NOSFovWidening@l(r9)
    564 sth r9, HydraulicsLookAngle@l(r10) | sth r10, HydraulicsLookAngle@l(r11)

`lmap` los sitúa en la zona de globales de **`Cubic.cpp:39/41`**
(`HydraulicsLookAngle`, `NOSFovWidening`, ambos por `bMath.hpp:132`), con presión
de registros extrema (r15..r18, r22, r25, r27, r28, r30 vivos a la vez). Son
compatibles con ser **consecuencia** de los cuatro allocnos de más, como decía la
r21 — pero la afirmación tal cual está escrita es falsa y la próxima ronda no
debe apoyarse en ella.

El orden de ficheros de la static-init del objetivo, sacado de `lmap` (sirve para
auditar el SourceList y **coincide con el nuestro**):

    idx  44 Camera.cpp   537 Cubic.cpp   587 CopView.cpp   711 DebugWorld.cpp
        731 Still.cpp    767 ICEManager.cpp   790 ICEReplay.cpp

## 7. HALLAZGO FUERA DE zCamera: 28 unidades del original NO son v1.76

Al buscar si el desempate de CSE podía venir de otro compilador, salió esto del
volcado DWARF (`Producer:` por unidad de compilación):

    SN BUILD v1.72   13 CU   D:/env/egami/rcmp/dev/source/{av,vd,decoder}/...
    SN BUILD v1.76  288 CU   TODAS las SourceLists (zCamera incluida) + vp6
    SN BUILD v1.83   15 CU   C:/packages/path/5.01.04/source/cmn/*.cpp

Y los compiladores que tenemos en `build/compilers/ProDG` son:

    3.5      GNU C++ 2.95.2 GameCube BUILD V1.37
    3.5b140  GNU C++ 2.95.2 SN BUILD v1.40
    3.7      GNU C++ 2.95.2 SN BUILD v1.46
    3.8.1    GNU C++ 2.95.2 SN BUILD v1.55
    3.9.3    GNU C++ 2.95.3 SN BUILD v1.76   <- el que usa todo el arbol

O sea: **`path` (15 CU) se compiló con un SN BUILD v1.83 que no tenemos, y
`rcmp`/`av` (13 CU) con un v1.72 que tampoco.** Para zCamera no cambia nada
(v1.76 es el correcto y es el que usamos), pero explica de golpe por qué esas dos
bibliotecas pueden resistirse, y **es la primera vez que se mide**. Merece un
encargo propio: comprobar el estado de `path` y de `rcmp`, y buscar los toolchains.

## 8. Lo que NO he probado

- **static-init (3.604 B): nada nuevo.** La veda de la r22 sigue en pie (no falta
  ninguna sentencia; hacen falta 25-31 insns RTL muertas en el prefijo). Lo único
  que he añadido es el veto de `-fno-force-addr` (§1) y la corrección del §6.
  **No he sacado el volcado RTL de la static-init**: con `-da` sobre un TU que es
  el 93 % de la unidad los dumps son de cientos de MB y el disco estaba al 98 %.
- **Los dos `bTan(unsigned short)` de `TrackCar`**, que el DWARF del original pone
  justo antes de `bVector3::operator/=` (o sea antes de `displacement /= distance`)
  con **rango CERO**. No los he escrito porque no sé qué calculan y **inventarme
  la expresión sería falsificar la fuente**. Posición exacta anotada arriba.
- **Los tres accesores de más de `TrackCar`** (`GetGeometryPosition()`,
  `GetVelocity()`, `GetGeometryPosition()`) entre el `bDistBetween` y el
  `SetTargetDistance`, también con rango cero. Mismo motivo. (La r22 encontró los
  mismos en `TrackCop` y tampoco los escribió.)
- **`_Storage`: el volcado `.cse2`.** Es lo que decide el §2 y no lo he sacado.
  El camino está servido: `assign` es una plantilla de `UTLVector.h`, así que un
  **caso mínimo** con `Vector<T*>`/`FixedVector<T*,2>` compila en segundos y
  permite volcar `.cse` y `.cse2` con la receta de `r22-ae.md` §2 sin tocar la
  unidad. Lo que hay que mirar es dónde termina el bloque básico de CSE respecto
  del último uso de `srcIt`.
- **`GetParameterLength()` sin `const`** en `ICEMover` — la otra pendiente de la
  r21. Toca cabecera fuera de zCamera y, a la vista de que las otras cinco formas
  confirmadas por DWARF dan exactamente cero, no la he gastado.
- **`CameraAI.cpp:253` y `Cubic.cpp:824`** con `IsGameFlowPaused()`: sin
  confirmación de DWARF, no los he tocado.
- **`TerrainVelocityNoise` (1.192 B)**: `dwbody.py` fresco da **diff VACÍO** — el
  árbol de inlines es idéntico. No hay lead estructural; lo que queda es un `lis`
  dos ranuras antes y una permutación f0/f12. **No he intentado moverlo.**
- **`TrackCop` (948 B)**: sigue siendo **una** instrucción intercambiada
  (`stfs f30,0x58(r1)` contra `mr r5,r3`, índices 153/155). Empate de `sched2`. No
  he encontrado palanca nueva y no he gastado ensayos.
- **`ICEMover::Update` (3.868 B)**: `lreg.py` pasado (ya sin comerse `-G0`), que
  era la primera pendiente de la r21 y la r22. **La vía no lleva a ninguna
  parte, y ahora está medido**: la función tiene **286 pseudos**, y en la tabla
  de allocnos de `global_alloc` **no aparecen ni r7 ni r8**, que son justo los
  dos registros en litigio (`mr r7,r0` del objetivo contra nuestro `mr r8,r0`).
  Los reparte `local-alloc`/`reload`, no `global_alloc`, así que **subir `n_refs`
  o acortar `live_length` no puede alcanzarlos**. Y los que sí salen están
  saturadísimos: 66 pseudos a r9, 22 a r11, 27 a r0, 8 a r10. **`lreg` agotado
  aquí**; lo que queda es el mecanismo de la §2.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): no aplicado. Sigue
  revertido, como pedía el encargo, porque no he cerrado bytes.

## 9. Verificación final

    build_direct.py zCamera                 ok
    triage.py zCamera                       identico al brief (3 near-miss, 4 muros)
    measure.py --cmp antes/final            +0 B, +0 funciones, 0 unidades cambian
    por funcion, 621 simbolos de zCamera    EMPEORAN 0   MEJORAN 1   nuevos 0
    audit.py Speed/Indep/SourceLists/zCamera  446/446 ok, CERO FALLA
      -> repetido: los dos ficheros de salida son byte a byte IDENTICOS
    A/B de GameFlow.hpp sobre las 12 unidades  byte-neutro (§3.3 y §3.4)

`frozen.py chk Speed/Indep/SourceLists/zCamera` da **«HA CAMBIADO»**: es
esperado, `TrackCar` sube 0,06 pp. **No lo he vuelto a congelar** porque no he
cerrado ninguna función; ojo a que la huella congelada es la de la **r19** y ya
venía desfasada desde la r22 (que también dejó `_Storage` movida sin recongelar).
Decisión tuya si se recongela.

## 10. Estado del árbol y convivencia

Ficheros modificados:

    src/Speed/Indep/Src/Camera/Movers/TrackCar.cpp   (t1 + t3 + t4)
    src/Speed/Indep/Src/Camera/ICE/ICEMover.cpp      (t5, una linea)
    src/Speed/Indep/Src/Misc/GameFlow.hpp            <-- COMPARTIDA (A/B en §3.3)

Finales de línea respetados por fichero (`TrackCar.cpp` es LF, los otros dos
CRLF); verificado tras editar. **Sin commit**, como pedía el encargo.

Herramientas nuevas dejadas en el scratchpad con prefijo `c23cam_`:

    c23cam_min.py    compila un .cpp minimo con los cflags EXACTOS de zCamera
                     y saca solo el cuerpo del .s  (2 s)
    c23cam_sweep.py  barrido de formas sobre ese minimo, resumen de una linea
    c23cam_ab.py     parchea el arbol, compila SOLO zCamera al scratchpad, mide
                     las 7 funciones que interesan y RESTAURA siempre (~40 s).
                     Verificado que el .o del arnes tiene el MISMO md5 que el
                     del arbol.
    c23cam_hdrab.py  el A/B por objetos de una cabecera compartida, comparando
                     secciones ELF (no el md5 del .o, que lleva el debug)

El disco arrancó la ronda con **9,7 GB libres** y acabó con ~13 GB; los `.o`
grandes del A/B se borran solos según se comparan.
