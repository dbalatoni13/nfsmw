# r54 — control: ¿es el MISMO residuo en las otras del bloque A?

**NO.** Y no de refilón: `zMain` es la unidad **más limpia de las trece por un factor de
20 a 76**, y el mecanismo que domina su residuo **no aparece en ninguna de las otras tres**.
Lo que forense1 y forense2 nombren vale **159.776 B**, no 1.652.576 B.

Pero el trabajo ha destapado un mecanismo que sí es general y que **ninguna herramienta del
proyecto estaba midiendo**: el **cambio de dueño** de un símbolo entre unidades. Vale
439.304 B en `zMisc`, 852.004 B de radio en `zGameplay`, y afecta a **9 de las 12**
SourceLists del bloque A. `zMain` es una de las tres a las que **no** le pasa.

---

## La tabla

Medidas del 9-sep 22:07–22:21. Columna `DOL` = bytes **exactamente distintos** entre el DOL
original y el que sale al promocionar sólo esa unidad (ver «Cómo se mide» abajo: **no** es
la cifra de `dolwhere`, que cuenta rangos y multiplica por doce).

| | **zMain** | **zLua** | **zAnim** | **zMisc** |
|---|---:|---:|---:|---:|
| código que guarda | 159.776 B | 94.264 B | 42.292 B | 78.008 B |
| **DOL distinto** | **15.369 B** | **331.944 B** | **472.031 B** | **767.221 B** |
| `linkdelta` | **IGUAL (9/9)** | rodata −720, data −320 | rodata −1112, data −192, over −16 | **rodata +2840, data +32** |
| símbolos permutados | **183** | 7.383 | 16.097 | 8.440 |
| ⌐ `.text` | **138 (75 %)** | 2 (0,03 %) | 27 (0,2 %) | 2.256 (27 %) |
| ⌐ `.rodata` | 45 (25 %) | 5.721 (77 %) | 11.660 (72 %) | 4.542 (54 %) |
| ⌐ `.data` | **0** | 1.657 (22 %) | 2.789 (17 %) | 1.583 (19 %) |
| ⌐ `.bss` / `.over` | 0 | 0 | 1.412 / 206 | 56 / 0 |
| `permorden` | 11 de 1.380 | **1 de 537** | 13 de 315 | **0 de 450** |
| `rodorden` | **653 de 655 en secuencia** | 18 bloques de cambio | 131 de 207 (76 fuera) | 307 de 307 **+150 extra** |
| `vtord` | 21 de 189 (2 de más) | 0 de 0 | **23 de 24** (2 de más) | **10 de 10** (1 de más) |
| **cambio de dueño** | **0** | **0** | **14** | **1** |
| símbolo dominante | `~Listable<ActionQueue,20>::_List` | **ninguno** | el bloque de vtables | `_List_base<WGridManagedDynamicElem>::clear` |
| ⌐ cuánto vale | 8.397 B = **55 %** | — | −1112 B repartidos en 14 saltos | 439.304 B = **57 %** |
| `dolwhere` | funciona | `SECCIONES NO COINCIDEN` | `SECCIONES NO COINCIDEN` | `SECCIONES NO COINCIDEN` |
| `trypromo` | ROTO | **ROTO** | **ROTO** | **ROTO** |

### Las cuatro respuestas

1. **Bytes de DOL distintos**: 15.369 / 331.944 / 472.031 / 767.221. `zMain` está **21×,
   31× y 50×** por debajo de las otras tres. No es un espécimen, es un caso atípico.
2. **La firma del histograma NO es la misma.** `zMain` es **difusa** (57 valores distintos,
   el mayor con 97 símbolos = 53 %); `zLua` es **un escalón** (−720 en 5.657 símbolos =
   77 %, más −320 en 1.608); `zAnim` es **cuatro escalones** (−1112 / −136 / −16 / −192);
   `zMisc` es **un escalón de superávit** (+2840 en 4.518) **más un racimo de .text** (+120
   en 2.255).
3. **La proporción `.text`/`.rodata`/`.data` NO es la misma.** `zMain` es la única con
   mayoría `.text` (75 %) y **cero** `.data`. Las otras tres son 54–77 % `.rodata`.
4. **Símbolo dominante análogo: sólo en `zMisc`.** Y ahí es *más* dominante que en `zMain`
   (57 % contra 55 %) — pero por otro mecanismo. En `zLua` y `zAnim` no existe.
5. **El superávit de `zMisc` es de otro tipo**, y encima el más caro de los cuatro.

---

## Los cuatro mecanismos, separados

El residuo del bloque A no es un animal: son cuatro, y cada unidad lleva su mezcla.

### M1 · Orden de emisión DENTRO de la unidad — el de `zMain`

Un símbolo único, que sólo define esa unidad, sale en otro sitio de su propio `.text`.

    zMain  .text: 23 transiciones, size +0
      0x801F844C   +9204   180 B  ~UTL::Collections::Listable<ActionQueue,20>::_List
      0x801F8500    -180   164 B  BuildMessageTable__16MBreakerStopCops     <- y 96 mas
      0x801FA8F4      +0     8 B  AllocVectorSpace__...FixedVector3<ActionQueue*,20,16>

El destructor sale 9.204 B tarde y los 97 símbolos que quedan en medio retroceden **−180 B,
que es exactamente su tamaño**. Ese hueco vale **8.397 de los 15.369 B** del DOL.

Las otras 22 transiciones de `zMain` son la misma cosa a pequeña escala sobre `_IHandle__`,
`HandleMessage_LuaBinding__`, `TypeName__` y `_GetKind__` — thunks de 12 B y cuerpos de
100/252 B que se reordenan entre sí (±720…±4016 B).

**Dónde más aparece M1:**

| unidad | M1 |
|---|---|
| `zLua` | **un intercambio de dos** — `Thunk__10VoidBinder` (48 B) ⇄ `UTL::Std::map<...>::_type_map` (116 B). 164 B de `.text`. Trivial. |
| `zAnim` | 7 transiciones sobre thunks y accesores de 8-12 B (`_IHandle__14INISCarControl`, `GetTypeID__`, `_._11IAnimEntity`), ±448 B máximo. |
| `zMisc` | **cero**: `permorden` da 0 de 450. El orden de funciones de `zMisc` es perfecto. |

O sea: **M1 domina en `zMain` y es residual o inexistente en las tres**. Si forense1 y
forense2 nombran M1, lo que nombren cerrará `zMain` y arañará `zAnim`. No cierra el bloque.

### M2 · Déficit / superávit de `.rodata` — el pool de cadenas y las vtables

    zLua    rodata -720   -> 3 transiciones: -708, +4, -16, todas en el pool de cadenas
    zAnim   rodata -1112  -> 14 transiciones, TODAS en el bloque de vtables
                             0x803D0220..0x803D0710
    zMisc   rodata +2840  -> 150 cadenas de MAS, todas detras de las 307 que si casan
    zMain   rodata +0     -> nada

`rodorden` lo dice con más claridad que ninguna otra medida:

* `zMain`: **655 cadenas objetivo, 656 nuestras, 653 en secuencia.** Prácticamente cerrado.
* `zMisc`: **307 de 307 en secuencia** y luego `insert nue[307:457]` — 150 cadenas nuestras
  que el original no emite (`_EU`, `done`, `19.8.31`, `16.2.1`, `GManager`, `SMS_MESSAGE_%d`,
  nombres `Attrib::Gen::*`…). Superávit **puro y ordenado**: no hay que mover nada, hay que
  **dejar de emitir**.
* `zLua`: 18 bloques `insert`/`delete`/`replace` intercalados. Emitimos cadenas que no van y
  nos faltan las que sí. Churn, no déficit limpio.
* `zAnim`: **171 cadenas nuestras contra 207 del objetivo**, 76 fuera de secuencia. Nos
  faltan 36 cadenas.

Y el eje de vtables es independiente del de cadenas:

* `zAnim`: `vtord` **23 de 24 posiciones mal**, con 2 vtables de más (`11CAnimMarker`,
  `UTL::COM::IUnknown`). El bloque entero está barajado.
* `zMisc`: **10 de 10**, pero es un solo movimiento: el grupo `Average*` sale **antes** de
  `DisculatorDriver` en vez de después, y lleva una vtable de más (`11AverageBase`).
  Cuadra al byte: +96 B que se adelantan −64 B que se retrasan +32 B de la vtable nueva =
  **+64 B**, que es exactamente el desfase medido de `_vt.16DisculatorDriver` (+2904 contra
  el +2840 de fondo).
* `zLua`: **0 vtables**. Este eje no existe ahí.
* `zMain`: 21 de 189, y son **intercambios por parejas** de vtables de 32 B
  (`_vt.12EEngineBlown` ⇄ `_vt.20EShowRaceOverMessage`, `_vt.9ECellCall` ⇄
  `_vt.13ESndGameState`…). 1.809 B de DOL.

### M3 · Déficit de `.data`

    zLua   -320  en 6 saltos (-12, -8, -180, +68, -192, +4)
    zAnim  -192  en 8 saltos (-4, -68, -8, -4, -8, -4, -4, -36)
    zMisc   +32
    zMain     0

Coherente con la nota del brief: el `.data` **no es un frente**, es un puñado de datos de
4-180 B. Pero **arrastra la sección entera** y con ella cada reubicación que la apunta: los
161.276 B de DOL distinto que `zLua` tiene en `.data` los produce un déficit de 320 B.

### M4 · CAMBIO DE DUEÑO — el mecanismo que no estábamos midiendo

**Un símbolo que nuestro objeto define y el objeto original de esa unidad no**, y que
**otro** objeto del enlace también define, se lo lleva el enlazador a nuestra unidad si
enlazamos antes. El símbolo salta cientos de kB y **todo lo que queda entre su sitio viejo y
el nuevo se desplaza por su tamaño**.

Es invisible para todo lo que tenemos:

* `linkdelta` no lo ve: el `.text` total **no cambia** (una unidad crece 120 B, otra
  encoge 120 B).
* `permorden` no lo ve: compara el orden **dentro** de la unidad. En `zMisc` da **0 de 450**.
* `fncmp` no lo ve: la función casa al 100 %, sólo está en otra dirección.
* `movidos` lo ve pero lo entierra: sale como un desplazamiento raro entre miles.

**`zMisc`, el caso completo** — todo su residuo de `.text` son **tres transiciones**:

    zMisc .text: size +0
      0x8020C488     +120   372 B  _M_insert__..._Rb_tree<Ui,pair<CUi,FileRecord>,...>
      0x80286528  -499872   120 B  clear__..._STL::_List_base<WGridManagedDynamicElem,...>
      0x802865A0       +0   284 B  find_if__...GarbageNode<Sim::Activity,40>...

Un método de plantilla de **120 B** sale **499.872 B antes de su sitio**, y los 2.255
símbolos que quedan en medio se desplazan +120 B. **439.304 de los 764.341 B** de DOL
distinto de `zMisc` (**57 %**) caen dentro de ese rango. En el enlace base lo suministra
`obj/zSim.o`; nuestro `src/zMisc.o` lo define y le gana el turno.
Ningún `.cpp` de `zMisc` es de World: la instanciación entra por una **cabecera**
(`WGridManagedDynamicElem.h` / `WCollisionAssets.h`).

**`zAnim`, el mismo mecanismo por otra puerta** — 14 símbolos, todos `.data`, todos
−146.332 B, todos robados a `zFeOverlay`:

    g_bCustomizeManagerHasControl, g_bTestCareerCustomization,
    g_pCustomizeMainPkg, g_pCustomizeSubPkg, g_pCustomizeSubTopPkg,
    g_pCustomizePartsPkg, g_pCustomizePerfPkg, g_pCustomizeDecalsPkg,
    g_pCustomizePaintPkg, g_pCustomizeRimsPkg, g_pCustomizeSpoilerPkg,
    g_pCustomizeShoppingCartPkg, g_pCustomizeHudPkg, g_pCustomizeHudColorPkg

La causa está a un `sed -n '686,700p'`:

    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp:686
        bool g_bCustomizeManagerHasControl = false;
        bool g_bTestCareerCustomization = false;
        char *g_pCustomizeMainPkg = "CustomizeMain.fng";
        ...

**Una cabecera que DEFINE globales en vez de declararlos.** La arrastra
`uiQRCarSelect.hpp`, y por ahí llega a `zAnim` y a `zFe2`. Cada TU que la incluye emite su
propia definición; el original sólo la tiene en `zFeOverlay`.

---

## El censo del bloque A entero

No me limité a mis tres: el mecanismo M4 se mide sin compilar nada, así que lo pasé por las
doce SourceLists del bloque. `dueño` = símbolos que **cambian de objeto suministrador**
(salto > 20 kB, excluidos los `$LC*`, cuyo nombre colisiona entre objetos y da basura).

| unidad | DOL distinto | rodata | data | permutados | **dueño** | símbolo del mayor salto |
|---|---:|---:|---:|---:|---:|---|
| **zMain** | **15.369** | +0 | +0 | 183 | **0** | — |
| zGameModes | 317.834 | +104 | +0 | 6.363 | **0** | — |
| **zLua** | 331.944 | −720 | −320 | 7.383 | **0** | — |
| **zAnim** | 472.031 | −1112 | −192 | 16.097 | 14 | `g_bCustomizeManagerHasControl` −146.332 |
| zFe | 493.560 | −48 | −32 | 10.732 | **47** | `React__11MainOptions...` −161.700 |
| zEAXSound | 535.611 | −104 | −384 | 14.692 | 23 | `__12EAX_CarState...` (1.308 B) −133.880 |
| zAI | 593.382 | −240 | −384 | 15.562 | 7 | `GetBits__C9WRoadLaneii` −122.928 |
| zFe2 | 594.132 | −880 | −160 | 9.959 | **49** | `_IHandle__15IGenericMessage` −222.516 |
| zFeOverlay | 609.877 | +0 | +0 | 1.216 | 5 | (un hueco de `.bss`, sin nombre útil) |
| zPhysics | 664.976 | +8 | +0 | 7.317 | 11 | `_List_base<WGridManagedDynamicElem>::clear` −388.752 |
| **zMisc** | 767.221 | **+2840** | +32 | 8.440 | 1 | `_List_base<WGridManagedDynamicElem>::clear` −499.872 |
| zGameplay | 1.173.534 | −896 | −128 | 13.064 | 24 | `_List_base<WGridManagedDynamicElem>::clear` −852.004 |

Tres cosas saltan a la vista:

1. **`zMain` no se parece a ninguna.** 15.369 B contra una mediana de 564.000. Y es una de
   sólo **tres** unidades con `dueño = 0`.
2. **`_List_base<WGridManagedDynamicElem>::clear`, 120 bytes, envenena tres unidades a la
   vez** — `zMisc` (−499.872), `zPhysics` (−388.752) y `zGameplay` (−852.004). Un solo
   arreglo toca 288.132 B de código del bloque A. **Es el objetivo individual más rentable
   que he encontrado.**
3. **`zFe` y `zFe2` tienen 47 y 49 cambios de dueño** — el doble que nadie. Ahí M4 no es un
   detalle, es el frente.

Y `zGameModes`: 124 B de código, **cero** cambios de dueño, `rodata +104`… y aun así
317.834 B de DOL distinto. Esa unidad es su propio enigma y no cuesta casi nada mirarla.

---

## Propuestas (medidas, NO aplicadas)

1. **`CarCustomize.hpp:686-700` — `extern` en la cabecera, definición en un `.cpp` de
   `zFeOverlay`.** Quita los 14 cambios de dueño de `zAnim` y los 14 de `zFe2`.
   **Aviso**: el original define esos 15 globales dentro de `zFeOverlay`, así que hay que
   elegir el `.cpp` **y el punto** que reproduzca su orden en `.data`; hacerlo mal cambia el
   `.data` de `zFeOverlay`, que hoy está a delta 0. **Paquete atómico de tres unidades.**
   La cabecera queda compilable en todo momento (`extern` + una definición).
2. **La instanciación de `_STL::_List_base<WGridManagedDynamicElem,...>::clear` que entra
   por cabecera en `zMisc`, `zPhysics` y `zGameplay`.** Vale 439 kB + 389 kB + 852 kB de DOL
   distinto. Hay que averiguar qué cabecera de World la mete y por qué el original no la
   emite ahí — probablemente el patrón «en clase = inline» al revés.
3. **`zMisc` es la unidad más cerca de las tres mías**, aunque su cifra de DOL sea la peor:
   `permorden` **0 de 450**, `rodorden` **307 de 307 en secuencia**, `.data +32`. Le sobran
   exactamente tres cosas: el símbolo robado, 150 cadenas y la vtable `11AverageBase`. Es
   trabajo **de quitar**, no de reordenar. Recomiendo asignarla en la r55.

---

## Cómo se mide (y por qué no usé `dolwhere` para la tabla)

* **`movidos` resta un «arrastre» que es la MODA de los desplazamientos, no el
  desplazamiento real de la sección.** Cuando una sección tiene delta 0 y sólo unos pocos
  símbolos permutan, la moda de los que se movieron es espuria y **resta permutaciones de
  verdad**. En `zMain` eso escondía 100 de los 183 símbolos: `movidos` dice **83**, y son
  **183**. Toda la tabla usa el arrastre **exacto**, sacado de la dirección de inicio de
  cada sección en los dos ELF enlazados.
* **Los `$LC*` colisionan de nombre entre objetos.** Una tabla de símbolos indexada por
  nombre se queda con el último y fabrica desplazamientos de +124.000 B que no existen. Los
  «12 mayores saltos» de `zMain` y `zLua` son todos ese artefacto. Filtrarlos es obligatorio.
* **`dolwhere` cuenta RANGOS, no bytes.** Su bucle extiende el rango mientras alguno de los
  12 bytes siguientes difiera, así que una zona con una diferencia cada 4 B (lo normal
  cuando una reubicación cambia de medio-palabra) se cuenta entera. La columna `DOL` de esta
  tabla son bytes **byte-a-byte**, comparando por DIRECCIÓN y sumando la diferencia de
  tamaño — así funciona también cuando las secciones no cuadran, que es en 11 de las 12.

### Aviso sobre la fila de `zMain`

`zMain.o` es de forense1/forense2 y **se reconstruyó dos veces mientras yo medía**
(22:02 y 22:24:53). Mi fila es la instantánea de las **22:07**, que reproduce el brief al
detalle (nueve secciones IGUAL, el destructor a +9204 arrastrando 97 símbolos a −180).
La reconstrucción de las 22:24 da `dolwhere` **183.693 B** con ~170 kB de diferencias
nuevas en `.rodata`: **eso es trabajo en vuelo, no un dato — no lo useis.**

## Sondas

Están en el scratchpad de esta sesión, no en `scripts/` (regla 7). Ninguna escribe en el
árbol; todas son de sólo lectura sobre los ELF enlazados y los `.o`:

    r54ctl_probe.py   arrastre EXACTO + racimos + contexto, con cache de ELF
    r54ctl_trans.py   transiciones de desplazamiento recorriendo la seccion en orden
    r54ctl_only.py    simbolos que solo estan en uno de los dos enlaces
    r54ctl_robo.py    censo de cambios de dueno leyendo objetos, SIN enlazar
    r54ctl_robomov.py cruza el censo con el desplazamiento real
    r54ctl_bloqueA.py el perfil comparado de las doce
    r54ctl_dolb.py    bytes de DOL distintos aunque las secciones no cuadren
    r54ctl_rango.py   reparte esos bytes entre dentro y fuera de un rango

Si algo de esto se queda, mi voto es **`r54ctl_robo.py`** (el censo de cambio de dueño: no
enlaza, tarda segundos, y mide algo que hoy no mide nadie) y el **arrastre exacto** dentro
de `movidos.py`, que hoy da una cifra equivocada en toda unidad con secciones a delta 0.
