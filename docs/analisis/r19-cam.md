# Ronda 19 — zCamera + zFEng

Base `base_r19_cam.json`: 184.000/196.468 B, **93,6539%**, 788 funciones.
Final `despues_r19_cam.json`: 184.540/196.468 B, **93,9288%**, 789 funciones.
**+540 B, +1 función.** `pctsnap --cmp`: **EMPEORAN: ninguna**, mejora 1.
**`zFEng` queda al 100% (71.460/71.460 B, 343 funciones).**

El «antes» del `pctsnap` se reconstruyó revirtiendo mis cuatro ficheros; la
reconstrucción mide **184.000 B exactos**, o sea es fiel.

`frozen.py cong` hecho: zCamera `e2b057af9b3df178`, zFEng `0851254e65b3aca2`.

## 0. `audit.py` — limpio en las dos, dos pasadas

**zCamera 446/446 ok, zFEng 343/343 ok**, y la segunda pasada da un fichero
**byte a byte idéntico** al de la primera (sin fantasmas por build compartido).
Los 16+2 fallos de la 18 siguen arreglados y no ha aparecido ninguno nuevo.

## 1. `IssueScriptMessages` — CERRADA, +540 B, y la causa era un PIN

`regmap` dijo lo que objdiff no: los 3 únicos diffs eran
`mr r30,r3 / mr r25,r4 / mr r26,r5` permutados, y **nosotros teníamos una local
`self` que el original no tiene**:

    fn  self   -   r30   <-- SOLO NUESTRA (struct FEPackage *)

La fuente llevaba `register FEPackage *self asm("r30") = this;`. Como `self` se
inicializa en el CUERPO, su `mr` sale detrás del guardado de los parámetros; el
original copia `this` primero porque **usa `this` directamente**.

    c1  quitar el pin y poner `this`                  99,7778 -> 100,0000  <- cierra

Y de paso se pudo quitar **ensamblador escrito a mano**, que está prohibido:

    c2  quitar `register FEEvent *pEvents asm("r29")` +
        `asm("lwz %0, 0x28(%1)" : "=r"(pEvents) : "r"(pScript));`
        por `FEEvent *pEvents = &pScript->Events[0];`      sigue 100,0000

De las **tres** barreras `asm("")` que tenía la función sólo **una** hace falta.
Medido por bisección (cada quite por separado da 100%; los tres a la vez, 95%):

    A  quitar la del tope del `for`  (`asm("" : "+r"(eventOffset));`)   100,0000
    B  quitar las DOS con `pEvent`   (antes del for y en la cola)       100,0000  <- se queda
    C  quitar sólo la primera                                           100,0000
    A+B+C (las tres)                                                     95,0000

Queda **una sola** barrera vacía, documentada en el fuente. La función ya no
lleva ni pines de registro ni ensamblador escrito.

## 2. `__static_initialization_and_destruction_0` — las DOS pistas de la 18, cerradas

### 2a. «Leer nuestras expansiones inline y compararlas con el volcado» — HECHO

**Ya existía la herramienta**: `scripts/dwbody.py` (con `regmap.py` generando
`build/regmap/our_<unidad>_*.nothpp`). **Pero tiene un fallo para este símbolo**:
`bodies()` indexa por firma y se queda con la PRIMERA coincidencia, y
`__static_initialization_and_destruction_0` **existe en 30+ unidades del
volcado**. La primera es la de 0x8003F5DC (1.321 líneas) y da un diff falso
enorme (156 `RoadblockElement`, 88 `UCrc32`, 43 `Prototype`…). Hay que
seleccionar por `low_pc`: la de zCamera es **0x8008095C**.

Hecho eso (script en el scratchpad), el árbol de inlines salía **302 líneas el
original contra 308 el nuestro**, con **una sola** diferencia estructural, justo
antes de los seis ctores de la tabla:

| original | nuestro |
|---|---|
| 3 × `Vector3::Vector3(float,float,float)` **con cuerpo vacío** | 3 × `bVector4::bVector4(float,float,float,float)` **que llaman a `bFill`** |

Son `vIceAccelLagMin/Max/Scale` de `ICEMover.cpp:12-14`. El original los declara
`ICE::Vector3` (que ya existe en `ICEMath.hpp:25`, tamaño 0x10, ctor de 3 floats
con `pad = 0.0f` por asignaciones); nosotros `bVector4`.

    c3  los tres globales a `ICE::Vector3(x, y, z)`   97,1121 (mismo asm, byte a byte)

**El asm no cambia** pero **el árbol de inlines pasa a ser IDÉNTICO** (302 vs
302; lo único que queda es `char*` contra `const char*` y los nombres). O sea:
**queda PROBADO que no expandimos código distinto**, y la hipótesis de la 18
(«el contenido previo difiere») muere con medida. Se queda en el árbol porque es
una corrección de tipo confirmada por el DWARF, no un ensayo.

### 2b. Los parámetros `char *` del DWARF — MEDIDO, NO SIRVE

    c4  `ReplayCategory(char*, char*, ...)` + miembros `char *`   97,1121, mismo tamaño

Sin ningún efecto. **Revertido.**

### Lo que queda, y ya no es «fuente»

`regmap` sobre la función: **mismo conjunto de locales, mismo árbol de bloques y
MISMO REPARTO de registros**. El delta normalizado exacto es sólo los cuatro
`addi r27,r30,0x30/0x48/0x60/0x78`; los elementos 0 y 1 pliegan
(`0x10(r30)`, `0x28(r30)`…) y el 2..5 no. Con el árbol de inlines idéntico, las
locales idénticas y el reparto idéntico, **la causa no puede estar en la fuente
de esta unidad**: es el reparto de un pseudo temporal (`this` del ctor
expandido) que `-fforce-addr` materializa y que CSE repliega sólo en los dos
primeros. **Frente cerrado por mi parte**; lo único vivo sería tocar `cflags`,
que no me toca.

## 3. `_Storage<Director*,2>` — el árbol del original entero, y TRES muros medidos

El DWARF da la estructura completa (`symbols/mw_dwarfdump.nothpp:348666`):

    _Storage(const&) -> FixedVector(const&) -> Vector() -> operator=(rhs)
       -> Vector::assign(const Vector&) -> assign(srcBeg, srcEnd)
          locales: unsigned int minSize (r26), Director* const* srcIt (r27),
                   Director** destIt (r10)
          bloque anonimo { Director*& dest;  Director* const& src; }   <- REFERENCIAS
          -> resize(unsigned int num)   [0x80081888 -> 0x80081A44]     <- LLAMA A resize
             -> pop_back() [rango CERO]  -> reserve()  -> push_back()

Y `lmap` da las líneas del original: **301** `minSize = srcEnd - srcBeg`,
**306** `if (srcIt == 0)`, **307** `resize(minSize)`, **310** `return`,
**318** `reserve`, **320** `destIt = begin()`, **326** el while doble,
**328/329/330** las tres sentencias del cuerpo, **334** `while (end() != destIt)`,
**344** `push_back(*srcIt++)`. Y dentro de `resize`: **242** el test que pliega,
**246** el `else if`, **247** `reserve`, **249** el bucle de crecimiento.

### Ensayos (todos medidos con objdiff sobre zCamera)

    base                                                             96,1592
    e1  h4+h5+h6 (tres sentencias, `minSize > size()`, `end()!=destIt`)  97,7855
    e2  e1 + minSize ANTES de srcIt (h3)                             96,5571
    e3  assign llama a resize(); resize con el bucle de encoger primero 89,0450  (+44 B)
    e4  resize `if (num>size()){...} else {while(size()>num) pop_back();}` 94,1592 (+20 B)
    e5  e4 + guarda `else if (num < size())`                          93,1384 (+32 B)
    e6  resize `if (num<size()){encoger} else if (num>size()){crecer}` 94,4360  (tamano EXACTO)
    e7  e6 + crecimiento como `if(...) do{}while(...)`                96,5744

**Revertido todo** (`matched_code` es todo-o-nada; 97,79% aporta cero bytes).

### Los tres muros, cada uno medido

1. **La resta se pliega.** El objetivo emite `add r24,r0,r9` + `mr. r27,r0` +
   `subf r9,r0,r24` + `srawi r26,r9,2`; nosotros plegamos `(beg+n)-beg` y damos
   `srawi r26,r0,2` sin `subf`. Probado: con `minSize = srcEnd - srcBeg` **siempre**
   pliega; con `minSize = srcEnd - srcIt` **no** pliega pero entonces el `subf`
   usa r27 y **bloquea el `combine` que hace el `mr.`** (hay un uso de i2dest en
   medio). Hace falta las dos cosas a la vez y no encontré la forma.
   Probado y descartado: locales `b`/`e` en el `assign` de un argumento (sin efecto).
2. **La ROTACIÓN del bucle de crecimiento.** El objetivo tiene el test arriba
   (`lwz r4,0x8(r31); cmplw r26,r4; ble`), preencabezado (`li r25,0`) y **una
   copia del test al final** (`mr r4,r9; cmplw r26,r4; bgt`). Nuestro GCC **no
   rota** ningún `while` cuyo test lea memoria (el `for (ii…)` de `reserve`, que
   compara registros, **sí** rota en los dos lados). Con `if + do/while` sale el
   test de abajo **pero se pierde** la compartición de `r4` con `push_back`
   (el cuerpo pasa a ser cabecera de bucle y CSE no entra) — es lo que separa e6
   de e7. **Barrido de flags sobre un repro mínimo**: `-fstrength-reduce`,
   `-fthread-jumps`, `-fregmove`, `-funroll-loops`,
   `-fdelete-null-pointer-checks`, `-fstrict-aliasing` — **ninguno la rota**.
   Formas probadas: `while`, `while` sin llaves, `for(;cond;)`, `do/while` —
   ninguna.
3. **El orden del `stw` del vtable.** El objetivo pone `stw r11,0xc(r31)` ANTES
   de los tres ceros; nosotros después. Un repro mínimo con el hierarchy a mano
   (sin plantillas) **sí** da el orden del objetivo, luego es reproducible, pero
   con el `UTLVector.h` real sale al revés.

**Dato nuevo que sí vale**: el DWARF dice que el cuerpo del bucle de copia son
`Director*& dest` y `Director* const& src`, o sea **referencias al elemento**
(`reference dest = *destIt++; const_reference src = *srcIt++; dest = src;`), no
punteros. Las dos formas dan el mismo asm, pero la de referencias es la del
original.

## 4. `LoadCameraShakes` (168 B) — veda confirmada, y la explicación del `li r0,0`

El único diff es `li r0,0` + el intercambio `i`/`num_tracks`. Ahora sé **por
qué**: el objetivo hace `cmpw r0, r28` (0 contra `num_tracks`) en el test de
entrada del `for`, y ese 0 sale del registro donde vive `warned_overflow`. O
sea, CSE reusa el cero de la local para la comparación, y por eso el `li` no se
borra.

    s1  `bool warned_overflow = false;` a nivel de función   96,6667 (sin efecto)

GCC la borra por muerta antes de que CSE pueda usar su cero. Hace falta un uso
**real** que se pliegue a cero instrucciones, y sigo sin encontrarlo. **Veda de
la 18 confirmada, con la mecánica ahora explicada.**

## 5. Los muros: lo que `dwbody` destapa (para quien los coja)

`triage` deja 4 muros, 7.000 B, todos por encima del 99%. `dwbody` (comparando
el árbol de inlines) enseña diferencias ESTRUCTURALES reales en dos:

**`TrackCarCameraMover::Update` (992 B, 99,569%)** — nos faltan, respecto del
original: `IsGameFlowPaused()`, **dos** `bTan(unsigned short)`,
`bScale(dest,v,scale)` donde nosotros ponemos `bFill(dest,x,y,z)`, y tres
accesores al final (`GetGeometryPosition`, `GetVelocity`, `GetGeometryPosition`).
Además nuestra local `fov` **vive en r11 y en el original no tiene registro**
(se pliega dentro de los dos `bTan`).

**`ICEMover::Update` (3.868 B, 99,907%)** — 15 diffs, TODOS en una ventana de 30
instrucciones (`GetKey(frame)`); el original tiene `frame` en r8 y el nuestro
reparte el mismo valor entre r9 y r11. `dwbody` enseña que antes de esa ventana
divergen: `IsUsingRealTime()`+`IsGameFlowPaused()` contra nuestro
`GetUseRealTimeRaw() const`, `Normalize(dst,src)` contra `Normalize(v)`, **dos
`Copy(Vector4*, const Vector3*, float w)` que no tenemos**, y
**`RevToAng(float)`+`FloatToAng` donde nosotros ponemos
`FloatToInt(GetDutch(fT) * 65536.0f)`** (`ICEMover.cpp:653`). Cada una de esas
mueve la numeración de pseudos, que es lo que decide el reparto de la ventana.

Probados y **sin efecto en bytes** (los dos se quedan porque son la forma del
original confirmada por el DWARF y son byte a byte idénticos):

    d1  `ICEShakeTrack::GetKey` usa `NumKeys` en vez de `GetNumKeys()`   99,9069
    e8  `ICE::Scale(Vector3*,const Vector3*,const Vector3*)` +
        `ICE::Clamp(Vector3*,const Vector3*,const Vector3*)` nuevas en
        ICEMath.hpp, y el bloque `accel_offset` de ICEMover.cpp usándolas  99,9069

## 6. Hallazgo que NO he podido arreglar: `FindConditionBranchTarget`

`FEMessageResponse.cpp:93-125` (en zFEng, y **al 100%**) está sostenida por
**ensamblador escrito a mano**, que el brief prohíbe:

    register const FEMessageResponse *self asm("r11") = this;
    register u32 count asm("r3") = self->Count;
    register u32 Result asm("r10") = 1;
    register u32 countLimit asm("r0");
    asm("mr %0, %1" : "=r"(countLimit) : "r"(count), "r"(Result));
    register FEResponse *responseList asm("r3") = self->pResponseList;
    asm("b .+12");            // <- un SALTO escrito a mano

El `b .+12` salta al cuerpo del `while` saltándose el test de entrada (lo que
GCC haría con `-fthread-jumps`, que no está en nuestros cflags). Ensayos:

    h1  C++ natural con `do { } while (Result != 0);`   62,6389
    h2  C++ natural con `while (Result != 0)`           72,2222
    h3  h2 + `goto` al cuerpo del bucle                 74,8611  (+4 B)

Ninguna llega. **Restaurada la versión con asm** (no se puede regresar 144 B),
pero queda anotado: la función está al 100% **por escribir instrucciones**, y si
el reparto de registros cambia alguna vez el `b .+12` corrompe la función en
silencio. Decisión para el dueño de la ronda.

Censo completo de `asm` que queda en las dos unidades (todo en zFEng, que está
al 100%):

| sitio | qué es | veredicto |
|---|---|---|
| `FEMessageResponse.cpp:94-103` | 5 pines + `asm("mr …")` + **`asm("b .+12")`** | **prohibido**, no he sabido quitarlo sin perder 144 B |
| `FEngine.cpp:324` | `asm("" : "+r"(iIterationTicks), "+r"(iTicksRemaining))` | barrera vacía (mía, ronda 18) — permitida |
| `FEngine.cpp:564` | `register u32 padMask asm("r30")` | pin de registro — permitido, sin revisar |
| `FEPackage.cpp:232` | `asm("" : "+r"(eventOffset))` | barrera vacía, la ÚNICA que hace falta (bisecada) |

En `zCamera` **no queda ni un `asm`**.

## 7. Trampas de herramienta que me han costado tiempo

- **`dwbody.py` coge la PRIMERA firma que casa.** Con
  `__static_initialization_and_destruction_0` (30+ unidades en el volcado) eso da
  un diff falso de 1.021 contra 507 líneas que parece «nos faltan 156
  RoadblockElement». Hay que filtrar por `low_pc`.
- **El md5 del `.o` no sirve para el A/B de cabecera**: lleva la información de
  depuración, y añadir una línea a una cabecera mueve el hash de 13 de 15
  unidades sin cambiar una instrucción. Hay que comparar con
  `measure.py --cmp` (hecho: **+0 B, +0 funciones, 0 unidades cambian** sobre las
  15 SourceLists que ven `ICEManager.hpp`/`ICEMath.hpp`: zAI zAnim zEAXSound
  zEcstasy zFe zFe2 zFeOverlay zLua zMain zMisc zPhysicsBehaviors zPlatform zSim
  zTrack zWorld).
- **El árbol está compartido de verdad**: a mitad de sesión el commit `a19940ff`
  (de otro agente, titulado zEagl4Anim) **se llevó dentro mis cambios en vuelo**
  de `ICEMover.cpp`. No se pierde nada, pero `git checkout --` ya no sirve para
  reconstruir el «antes»: hay que guardarse copias en el scratchpad.

## 8. Qué NO he probado

- `_Storage`: nada que haga sobrevivir el `subf` **y** deje el `mr.`; y nada que
  rote el bucle de `resize` sin perder la compartición de `r4`.
- `ICEMover::Update`: **no** he escrito `RevToAng`/`FloatToAng`, ni el
  `Normalize(dst,src)`, ni los dos `Copy(Vector4*,const Vector3*,float)` que el
  DWARF pide. Es la vía que queda para los 3.868 B.
- `TrackCarCameraMover::Update`: no he tocado nada; el `fov` sin registro y los
  dos `bTan` son la pista.
- `LoadCameraShakes`: no he encontrado un uso real de `warned_overflow` que se
  pliegue a cero instrucciones.
- `FindConditionBranchTarget`: no he probado a llegar al 100% con `goto` **más**
  forzar `count` al registro de retorno (el objetivo hace `beqlr` porque `count`
  ya está en r3).
