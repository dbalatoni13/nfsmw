# r64-diferido-cam -- zCamera: el bloque diferido, de 12.117 a 8.181 B

**Agente `diferido-cam`. Unidad en exclusiva: `zCamera`. Nada commiteado.**

| medida | ANTES (r63) | DESPUES (r64) |
|---|---:|---:|
| `dolwhere zCamera` **con** la linea de `keep.lst` | 19.529 B | **15.557 B** (`-3.972`) |
| `dolwhere zCamera` **con el `keep.lst` de hoy** | 25.359 B | **21.388 B** (`-3.971`) |
| de eso, `.text` (bloque diferido) | 12.117 B | **8.181 B** |
| de eso, `.rodata` | 7.292 B | 7.337 B |
| de eso, `.data` + otras unidades | 39 + 81 B | 39 + 0 B |
| `textorder` descolocadas | 124 de 453 | **40 de 453** |
| `textorder` saltos de delta | 31 | **21** |
| bytes mal colocados en `.text` (metrica propia) | 8.636 | **4.752** |
| `fncmp` funciones abiertas | 3 (8.464 B) | 3 (8.464 B) -- sin cambio |
| `linkdelta` | `.text +0 resto IGUAL` | `.text +0 resto IGUAL` |
| `lcfix --check zCamera` | al dia | **al dia, 0 pendientes** |

`.o` sellado: **`6669f4f37567f587e22023eb4d917b5f02c56320`**, TRES compilaciones
seguidas con el mismo sello. La base de la ronda era
`d3d8c792a449e2cec3342ca24aa663c8afffa98f` (el sello con el que cerro la r63,
o sea que el arbol llegaba intacto).

**AVISO DE TANDA, medido**: a media ronda el sello de zCamera paso solo de
`9e114c4a536c05e9e2bca410e632e4eb602b3406` a `6669f4f3...` **sin que yo tocara
una linea**. La causa esta identificada: otro agente de esta misma tanda edito
`Libs/Support/Utility/UCollections.h`, `UListable.h` y `World/WorldConn.h`, que
zCamera ve. **Y el DOL no se movio ni un byte**: 15.557 / 21.388 identicos con
los dos sellos. O sea que fue DWARF (numeros de linea), como el aviso de la
r63 sobre los comentarios. Pero es un recordatorio de que en una tanda **el
sello de una unidad no basta como control si otro agente toca una cabecera
compartida**: hay que reconfirmar la CIFRA, no solo el sha1.

`zCamera` **NO promociona**: le quedan 8.181 B de orden en `.text`, 7.337 B de
`.rodata` y tres funciones abiertas.

---

## 0. LO PRIMERO, Y ES LO MISMO QUE PEDIA LA r63: UNA LINEA DE `keep.lst`

La r63 la propuso y **sigue sin aplicarse**. Vuelta a medir hoy con el objeto
sellado de arriba, un `keep.lst` privado (copia byte a byte del de hoy mas la
linea, con sus CRLF) y el mismo `ngcld -strip-unused-data` + `dtk elf2dol`:

```
config/GOWE69/keep.lst   (insertar justo debajo de la linea 162,
                          `zCamera.o:gap_06_80416538_data`)

zCamera.o:gap_06_80416558_data
```

| enlace | DOL distinto |
|---|---:|
| con la linea | **15.557 B** |
| con el `keep.lst` de hoy | 21.388 B |

Son **5.830 B** que estan escritos y que `-strip-unused-data` tira porque el
simbolo (4 B de `.data` que emite `ChaseCamAI.cpp`) no esta protegido por
nombre. No crea ningun `pad_`, no toca `splits.txt`, `symbols.txt` ni
`configure.py`. **El trabajo de orden de esta ronda vale lo mismo con la linea
y sin ella** (`-3.972` contra `-3.971`), asi que las dos cosas son
independientes y ninguna espera a la otra.

---

## 1. Las DOS colas de `finish_file`, separadas y atacadas por separado

El encargo decia "los saltos estan en el bloque diferido". Lo estan, pero el
bloque diferido **son dos colas distintas y no se tocan la una a la otra**
(`cp/decl2.c:3689..3796`, una pasada):

1. `instantiate_pending_templates()` -- **plantillas**, FIFO de
   `add_pending_template` (pt.c:3559);
2. las vtables (`walk_globals`);
3. `__static_initialization_and_destruction_0`;
4. `wrapup_global_declarations(saved_inlines)` -- **inlines**, y ahi el orden
   lo fija `mark_inline_for_output` (decl2.c:2140), que llaman
   `cons_up_default_function` (lex.c:2045, **al COMPLETAR la clase**) y
   `finish_function` (decl.c:14701, al cerrar el cuerpo).

En zCamera la frontera esta clavada: las posiciones **320-335 del objetivo son
plantillas**, la **336 es el static-init** y de la **337 a la 452 son inlines**.
Las palancas son distintas para cada mitad y esta ronda usa las dos.

### 1.1 La cola de plantillas: el primer de ConversionUtil (-1.070 B)

El objetivo emite `find<const IVehicle* const*, const IVehicle*>` (176 B)
**detras** del racimo de `ConversionUtil` (880 B, diez funciones) y nosotros
delante.

El racimo entero cuelga de **CUATRO raices**, no de diez peticiones: en
`at_eof` la rama de diferir de `instantiate_decl` (pt.c:9439) ya no se toma, asi
que una dependencia se instancia **recursivamente y se emite ANTES que su
llamante**. Por eso el orden interno del racimo (Copy4, Scale3, Make4,
RightToLeftVector4, RightToLeftMatrix4, Make3, RightToLeftVector3, ...) **ya
casaba** sin tocar nada: lo genera el propio compilador. Las cuatro raices son
`RightToLeftMatrix4<Matrix4,Matrix4>`, `RightToLeftVector3<Vector3,Vector3>`,
`RightToLeftMatrix4<bMatrix4,Matrix4>` y `RightToLeftVector3<bVector3,Vector3>`.

La palanca es un **primer de cola**: una `static` muerta que las llama en ese
orden, colocada en el punto de parseo que le da el sitio. **El sitio esta
bisectado**, y la biseccion es la mitad del hallazgo:

| primer delante de | resultado |
|---|---|
| `CameraMover.cpp` (o antes) | el racimo sale el PRIMERO de la cola -- peor (8.972) |
| `CDActionDrive.cpp`, `Cubic.cpp`, `ICEPoint.cpp` | **no hace nada** (8.636, identico a la base) |
| `CameraAI.cpp` linea 588 o 597 | 8.556 |
| **`CameraAI.cpp` linea 604 o 611** | **7.580** |

O sea: las seis raices de la cola de plantillas de zCamera **se piden todas
dentro de `CameraAI.cpp`**, y la quinta (`find<_List_iterator<IBody*>>`) la pide
`CameraAI::AddAvoidable` (linea 600). El primer tiene que ir entre esa funcion
y la instanciacion explicita de `find<IVehicle*const*>` del final del fichero.

**Negativo medido de paso**: mover esa instanciacion explicita de `find` al
final del fichero **no cambia ni una posicion**. El simbolo ya estaba en la cola
por su uso real; la instanciacion explicita llega tarde y no manda.

El primer emite 96 B en el `.o` que el enlazador tira (`linkdelta` sigue
`.text +0`). Es un andamio: el original tendra ahi un uso de verdad.

### 1.2 La instanciacion explicita que SOBRABA (-674 B)

`CameraAI.cpp` llevaba `template class UTL::Collections::_Storage<CameraAI::Director *, 2>;`.
**Quitarla vale 674 B.** Con ella, `_._Q33UTL11Collectionst8_Storage2ZPQ28CameraAI8Directori2`
sale en la posicion 367 del bloque y el objetivo lo tiene en la **439**; sin
ella lo pide el uso real (`CameraAI::Shutdown` copia la `List`) y sale en la
**441**, a dos posiciones. El constructor no se pierde: siguen estando las 453
funciones del objetivo.

Es el reverso exacto de la receta de `zAI` (r56: *escribir* una instanciacion
explicita para adelantar un simbolo). **Sirve en los dos sentidos, y aqui el
util era el de quitar.**

### 1.3 La cola de inlines: la clase `IDebugWatchCar` (-1.296 B)

`_IHandle__14IDebugWatchCar`, `_._14IDebugWatchCar` y
`push_back<IDebugWatchCar*>` (704 B) salen en la posicion **426** y el objetivo
los tiene en la **341**. La clase esta definida a mano dentro de
`Actions/CDActionDebugWatchCar.cpp` (no tiene cabecera en nuestro arbol).

Como la posicion la fija el **completado de la clase**, mover la definicion
mueve los tres simbolos. Las cuatro posiciones, medidas:

| definicion de `class IDebugWatchCar` en | posicion | bytes mal |
|---|---:|---:|
| `Actions/CDActionDebugWatchCar.cpp` (como estaba) | 426 | 8.636 |
| final de `CameraAI.cpp` | 369 | 7.580 |
| `CameraAI.cpp` linea 28 **o** 260 | 365 | 7.580 |
| **`Camera.cpp`, delante del `#include` de `CameraMover.hpp`** | **341** | 6.904 -> **5.608** |

El tope que explica la tabla: los accesores en clase de `CameraMover`
(posiciones 346-356 del objetivo) se marcan al cerrar `CameraMover.hpp`, que
`Camera.cpp` incluye en su **linea 2**. Cualquier sitio posterior a ese
`#include` --incluida la primera linea de `CameraAI.cpp`-- cae detras de ellos.
`IMPLEMENT_LISTABLE(IDebugWatchCar)` se queda donde estaba: a ese lo fija el
`.bss`, no esto.

### 1.4 Los tres movers y `ITrafficCenter` (-956 B)

El objetivo emite `GetAnchor` de `RearViewMirrorCameraMover`,
`TrackCarCameraMover`, `TrackCopCameraMover` y `RenderCarPOV` de `TrackCop` en
las posiciones **357-360**; sin tocar nada salian en la **371/383/393/394**,
intercalados con los accesores de las `CDAction`. Ese intercalado era el que
producia los dos saltos gordos de 464 y 292 B en los bloques de `CDActionDrive`
y `CDActionTrackCar`.

Se arregla con tres `#include` en `Camera.cpp` detras de `CameraMover.hpp`
(`Movers/Rearview.hpp`, `Movers/TrackCar.hpp`, `Movers/TrackCop.hpp`): **-856 B**.
Y con `ITrafficCenter.h` delante de `MJumpCut.h` en `CDActionDrive.cpp`,
**-100 B mas** -- pero **solo combinado**: por separado ese `#include` no mueve
nada (4.852 en los dos casos).

**Negativo medido**: subir ese `#include` al principio del fichero (delante de
`UMath.h`) no cambia nada -- 4.752 antes y despues. El par
`push_back<ITrafficCenter*>` / `~ITrafficCenter` sigue saliendo AL REVES que el
objetivo, y eso no lo decide el orden de `#include`.

---

## 2. Los tres topes que quedan, y son CABECERA COMPARTIDA (propuestas)

Quedan 4.752 B mal colocados. La busqueda voraz sobre la secuencia dice que
**diez movimientos los llevan a CERO**, y los tres primeros son estos. Ninguno
se puede hacer desde un `.cpp`: **no los he aplicado.**

### P1. `CameraMover.hpp` -- `CameraAnchor` detras de `CameraMover`, y los movers en medio

El objetivo pide, en este orden: accesores de `CameraMover` (346-356), los tres
movers (357-360), `_._12CameraAnchor` (361), accesores de `CubicCameraMover`
(362-365). En nuestro `CameraMover.hpp` el orden de las clases es
`CameraAnchor` (l.71-265), `CameraMover` (l.268-374), `CubicCameraMover`
(l.384-440), y por eso `~CameraAnchor` sale **delante** de los accesores de
`CameraMover` y los movers **detras** de `CubicCameraMover`.

La cabecera la ven 38 unidades. La forma inerte para las demas es la que ya usa
esta misma cabecera con `ZCAM_ANCHOR_IMPLICIT_DTOR`: una guarda que solo define
`zCamera.cpp`. Vale **~180 B** por si sola y desbloquea los 704 B de los pasos
9-10 de la busqueda voraz.

### P2. `CameraAI.hpp` -- la maquinaria de la lista de `Director` delante de `Action`

El objetivo emite `__Q33UTL...t8_Storage2<Director*,2>` (338) y
`_._Q43UTL...Listable<Director,2>::List` (339) **delante** de
`_._Q28CameraAI6Action` (340). En nuestra cabecera `Action` se define en la
linea 34 y `Director` en la 53, asi que `~Action` se marca antes y la `List` no
se completa hasta `CameraAI::Shutdown`. Vale **836 B** (el paso 1 de la voraz).
La cabecera la ven `AIPursuit.cpp`, `PhotoFinish.cpp`, `Main.cpp`,
`PVehicle.cpp`, `PInput.cpp` y tres `Generated/Events`.

### P3. `IAttachable` -- completar la clase mas tarde

`_._11IAttachable` sale el PRIMERO de la cola de inlines (338) y el objetivo lo
tiene en la **366**, detras de los accesores de `CubicCameraMover`. La r63 ya
midio que el `#include` no sirve (entra por el grafo transitivo de
`Camera.hpp`). La forma que si funciona en el arbol es la guarda de
`ICause.h` de la r57 (`ICAUSE_FWD_IEXPLOSION`): declaracion adelantada en vez de
`#include`, inerte sin el macro. Vale 84 B directos y encadena con P1.

---

## 3. `__static_initialization_and_destruction_0`: LOS TRES ALLOCNOS, IDENTIFICADOS

La veda de `Movers/Cubic.cpp` se cerraba diciendo *"hace falta primero
identificar los TRES allocnos (SYM/LIT/CONST) en el `.greg`, que en una funcion
de 437 pseudos no se hace de oido"*. **Ya estan.** Receta, 40 s:

```
python scripts/rtldump.py zCamera static_initialization -dl
grep -n "const_int 910" scratchpad/rtl/zCamera_cpp.i.lreg     -> insn 2839
python scripts/lreg.py zCamera __static_initialization_and_destruction_0
```

| cantidad | insn | pseudo | n_refs | live_len | prioridad | reg |
|---|---:|---:|---:|---:|---:|---|
| CONST (`const_int 910`) | 2839 | **858** | 2 | 7 | 2857 | r10 |
| SYM (`high HydraulicsLookAngle`) | 2847 | **874** | 2 | 10 | 2000 | r11 |
| LIT (`high *$LC1122` = 12000.0f) | 3165 | **965** | 2 | 36 | 555 | r9 |

Y una lectura nueva del `fndiff`: **las tres RANURAS son las mismas en los dos
lados** (541, 549, 557) y **los tres REGISTROS tambien** (r11, r9, r10). Lo
unico que rota es que valor cae en cada par (ranura, registro): nosotros
`SYM,LIT,CONST` y el objetivo `LIT,CONST,SYM`. No es un problema de ranuras: es
el mapa valor-registro, exactamente como decia la r50.

Con el orden de proceso que se observa hoy (por prioridad: 858, 874, 965; y les
toca r10, r11, r9), para llegar al reparto del objetivo hace falta el orden
`SYM, LIT, CONST`. **Y ahi hay un tope aritmetico medido**: `965` tiene
`live_len 36`, asi que `n_refs=10` da prioridad 2777 y `n_refs=11` da 3055 --
**no hay entero que caiga entre los 2857 de CONST y los 3000 que necesita SYM**.
Con `n_refs` a secas **no se llega**: hay que mover tambien `live_length`.

### NEGATIVO NUEVO Y MEDIDO: la variante `"m"` de la palanca de la r53

El encargo mandaba la receta de la r53 (colgar una entrada de un `asm` que ya
exista para subir `n_refs` sin gastar ranura). **En esta funcion no hay ningun
`asm` previo**, asi que hay que crearlo. La variante `"+r"` sobre el valor ya la
midio la r61 (58 filas). Esta ronda se probo la que faltaba, `"m"` sobre el
SIMBOLO --que en PowerPC solo necesita el `@ha` que ya existe y deberia ser
gratis--:

```c
extern bAngle HydraulicsLookAngle;
static inline bAngle _cam64_m(bAngle a) { asm("" : : "m"(HydraulicsLookAngle)); return a; }
bAngle HydraulicsLookAngle = _cam64_m(bDegToAng(5.0f));
```

| | tamano | fuzzy | filas |
|---|---:|---:|---:|
| base | 3.604 / 3.604 | 99,839066 % | **6** (549/557/558/561/562/564) |
| con `"m"` | **3.644** / 3.604 | 89,59379 % | **236** |

**Falla el control duro del encargo** (tamano clavado en 3.604): el operando
`"m"` gasta ranura, el marco encoge de `0xa8` a `0xa0` y el bloque de `psq_st`
se corre entero --se preserva un FPR menos--. Revertida.

Con esto quedan medidas **las dos formas del pin** en esta funcion. Lo que
sigue sin probar, y es lo que dice la aritmetica de arriba: tocar
**`live_length`** en vez de `n_refs`, o sea la cantidad fantasma sobre los
pseudos 874 y 965. Escrito junto a la funcion.

El andamio `_r60_ec[20]` de `ICEReplay.cpp` **sigue en pie**, como pedia el
encargo. De los 687 andamios del arbol no se ha retirado ninguno; se ha anadido
uno (el primer de ConversionUtil).

## 4. `Update__19TrackCarCameraMoverf`: comprobado, sin cambio

Verificado sobre el arbol de hoy: **992/992 B, 99,629036 %, 15 filas**, y ni la
reordenacion del bloque diferido de esta ronda ni la de la r63 mueven una sola
fila. El "unico hallazgo de forma superviviente" del encargo (`look_offset` en
152/144/148) ya lo cerro la r63 en sus dos lecturas --el racimo R1
(`Look = *GetGeometryPosition()`, 95,979836 % y 31 filas) y el R3 (`look_offset`
en z,x,y, ensayo T2 de la r61, 99,616936 % y 15 filas)--, y las dos estan
escritas encima de `Update` en `Movers/TrackCar.cpp`. **No he construido nada
encima.**

## 5. Donde estan los 15.557 B que quedan

| region | bytes | que es |
|---|---:|---|
| `.text` | **8.181** | el bloque diferido: 21 saltos, 40 funciones descolocadas |
| `.rodata` | **7.337** | el pool de cadenas (~3.500) + el orden de vtables (~3.300) |
| `.data` | 39 | residuo |
| otras unidades | 0 | (eran 81 B de mitades `@l`; ya no) |

Los 8.181 de `.text` caen a **cero** con los diez movimientos que da la
busqueda voraz, y **los tres primeros son las propuestas P1/P2/P3 de la
seccion 2**. La `.rodata` sigue intacta y sigue valiendo lo que decia la r63:
**primero el pool de cadenas, DESPUES las vtables**.

## 6. Ficheros tocados

| fichero | que |
|---|---|
| `src/Speed/Indep/Src/Camera/CameraAI.cpp` | primer de ConversionUtil; fuera la instanciacion explicita de `_Storage<Director*,2>` |
| `src/Speed/Indep/Src/Camera/Camera.cpp` | clase `IDebugWatchCar`; tres `#include` de movers |
| `src/Speed/Indep/Src/Camera/Actions/CDActionDebugWatchCar.cpp` | la clase se va (queda la nota) |
| `src/Speed/Indep/Src/Camera/Actions/CDActionDrive.cpp` | `ITrafficCenter.h` delante de `MJumpCut.h` |
| `src/Speed/Indep/Src/Camera/Movers/Cubic.cpp` | **solo comentario**: los tres allocnos y el negativo de `"m"` |
| `src/Speed/Indep/SourceLists/zCamera.cpp` | **solo comentario**: la linea de `keep.lst` y el resumen |

Ninguna cabecera compartida tocada. Nada commiteado. `lcfix.py` no se ha
ejecutado (solo `--check`, que da 0 pendientes).
